#!/usr/bin/python

###############################################################################
# Name:         build/osx/fix_xcode_ids.py
# Author:       Dimitri Schoolwerth
# Created:      2010-09-08
# Copyright:    (c) 2010 wxWidgets team
# Licence:      wxWindows licence
###############################################################################

testFixStage = False

import os
import sys
import re

USAGE = """fix_xcode_ids - Modifies an Xcode project in-place to use the same identifiers (based on name) instead of being different on each regeneration"
Usage: fix_xcode_ids xcode_proj_dir"""

if not testFixStage:
    if len(sys.argv) < 2:
        print USAGE
        sys.exit(1)

    projectFile = sys.argv[1] + "/project.pbxproj"
    fin = open(projectFile, "r")
    strIn = fin.read()
    fin.close()



# Xcode identifiers (IDs) consist of 24 hexadecimal digits
idMask = "[A-Fa-f0-9]{24}"

idDict = {}

# convert a name to an identifier for Xcode
def toUuid(name):
    from uuid import uuid3, UUID
    id = uuid3(UUID("349f853c-91f8-4eba-b9b9-5e9f882e693c"), name).hex[:24].upper()

    # Some names can appear twice or even more (depending on number of
    # targets), make them unique
    while id in idDict.values() :
        id = "%024X" % (int(id, 16) + 1)
    return id

def insertBuildFileEntry(filePath, fileRefId):
    global strIn
    print "\tInsert PBXBuildFile for '%s'..." % filePath,

    matchBuildFileSection = re.search("/\* Begin PBXBuildFile section \*/\n", strIn)
    dirName, fileName = os.path.split(filePath)

    fileInSources = fileName + " in Sources"
    id = toUuid(fileInSources)
    idDict[id] = id
    insert = "\t\t%s /* %s */ = {isa = PBXBuildFile; fileRef = %s /* %s */; };\n" % (id, fileInSources, fileRefId, fileName)

    strIn = strIn[:matchBuildFileSection.end()] + insert + strIn[matchBuildFileSection.end():]

    print "OK"
    return id


def insertFileRefEntry(filePath, id = 0):
    global strIn
    print "\tInsert PBXFileReference for '%s'..." % filePath,

    matchFileRefSection = re.search("/\* Begin PBXFileReference section \*/\n", strIn)
    dirName, fileName = os.path.split(filePath)
    if id == 0:
        id = toUuid(fileName)
        idDict[id] = id

    insert = "\t\t%s /* %s */ = {isa = PBXFileReference; lastKnownFileType = file; name = %s; path = %s; sourceTree = \"<group>\"; };\n" % (id, fileName, fileName, filePath)
    strIn = strIn[:matchFileRefSection.end()] + insert + strIn[matchFileRefSection.end():]

    print "OK"
    return id


def insertSourcesBuildPhaseEntry(id, fileName, insertBeforeFileName, startSearchPos = 0):
    global strIn
    print "\tInsert PBXSourcesBuildPhase for '%s'..." % fileName,

    matchBuildPhase = re.compile(".+ /\* " + insertBeforeFileName + " in Sources \*/,") \
        .search(strIn, startSearchPos)
    insert = "\t\t\t\t%s /* %s in Sources */,\n" % (id, fileName)
    strIn = strIn[:matchBuildPhase.start()] \
        + insert \
        + strIn[matchBuildPhase.start():]

    print "OK"
    return matchBuildPhase.start() + len(insert) + len(matchBuildPhase.group(0))

# Detect and fix errors in the project file that might have been introduced.
# Sometimes two source files are concatenated. These are spottable by
# looking for patterns such as "filename.cppsrc/html/"
# Following is a stripped Xcode project containing several problems that
# are solved after finding the error.
strTest = \
"""/* Begin PBXBuildFile section */
        95DE8BAB1238EE1800B43069 /* m_fonts.cpp in Sources */ = {isa = PBXBuildFile; fileRef = 95DE8BAA1238EE1700B43069 /* m_fonts.cpp */; };
        95DE8BAC1238EE1800B43069 /* m_fonts.cpp in Sources */ = {isa = PBXBuildFile; fileRef = 95DE8BAA1238EE1700B43069 /* m_fonts.cpp */; };
/* End PBXBuildFile section */

/* Begin PBXFileReference section */
        95DE8BAA1238EE1700B43069 /* m_fonts.cpp */ = {isa = PBXFileReference; lastKnownFileType = file; name = m_fonts.cpp; path = ../../src/html/m_dflist.cppsrc/html/m_fonts.cpp; sourceTree = "<group>"; };
/* End PBXFileReference section */

/* Begin PBXGroup section */
        95DE8B831238EE1000B43069 /* html */ = {
            isa = PBXGroup;
            children = (
                95DE8B841238EE1000B43069 /* src/html */,
                95DE8BA91238EE1700B43069 /* src/html/m_dflist.cppsrc/html */,
                95DE8BCE1238EE1F00B43069 /* src/generic */,
            );
            name = html;
            sourceTree = "<group>";
        };
        95DE8B841238EE1000B43069 /* src/html */ = {
            isa = PBXGroup;
            children = (
                95DE8B851238EE1000B43069 /* chm.cpp */,
                95DE8BAD1238EE1800B43069 /* m_hline.cpp */,
            );
            name = src/html;
            sourceTree = "<group>";
        };

        95DE8BA91238EE1700B43069 /* src/html/m_dflist.cppsrc/html */ = {
            isa = PBXGroup;
            children = (
                95DE8BAA1238EE1700B43069 /* m_fonts.cpp */,
            );
            name = src/html/m_dflist.cppsrc/html;
            sourceTree = "<group>";
        };
/* End PBXGroup section */


/* Begin PBXSourcesBuildPhase section */
        404BEE5E10EC83280080E2B8 /* Sources */ = {
            files = (
                95DE8BAC1238EE1800B43069 /* m_fonts.cpp in Sources */,
            );
            runOnlyForDeploymentPostprocessing = 0;
        };
        D2AAC0C405546C1D00DB518D /* Sources */ = {
            files = (
                95DE8BAB1238EE1800B43069 /* m_fonts.cpp in Sources */,
            );
            runOnlyForDeploymentPostprocessing = 0;
        };

/* End PBXSourcesBuildPhase section */"""

if testFixStage:
    strIn = strTest

rc = re.compile(".+ (?P<path1>[\w/.]+(\.cpp|\.cxx|\.c))(?P<path2>\w+/[\w/.]+).+")
matchLine = rc.search(strIn)
while matchLine:
    line = matchLine.group(0)

    # is it a line from the PBXFileReference section containing 2 mixed paths?
    # example:
    # FEDCBA9876543210FEDCBA98 /* file2.cpp */ = {isa = PBXFileReference; lastKnownFileType = file; name = file2.cpp; path = ../../src/html/file1.cppsrc/html/file2.cpp; sourceTree = "<group>"; };
    if line.endswith("};") :
        path1 = matchLine.group('path1')
        path2 = matchLine.group('path2')
        print "Correcting mixed paths '%s' and '%s' at '%s':" % (path1, path2, line)
        # if so, make note of the ID used (belongs to path2), remove the line
        # and split the 2 paths inserting 2 new entries inside PBXFileReference
        fileRefId2 = re.search(idMask, line).group(0)

        print "\tDelete the offending PBXFileReference line...",
        # delete the PBXFileReference line that was found and which contains 2 mixed paths
        strIn = strIn[:matchLine.start()] + strIn[matchLine.end()+1:]
        print "OK"

        # insert corrected path1 entry in PBXFileReference
        fileRefId1 = insertFileRefEntry(path1)

        # do the same for path2 (which already had a ID)
        path2Corrected = path2
        if path2Corrected.startswith('src') :
            path2Corrected = '../../' + path2Corrected

        insertFileRefEntry(path2Corrected, fileRefId2)


        buildPhaseId = {}
        # insert a PBXBuildFile entry, 1 for each target
        # path2 already has correct PBXBuildFile entries
        targetCount = strIn.count("isa = PBXSourcesBuildPhase")
        for i in range(0, targetCount):
            buildPhaseId[i] = insertBuildFileEntry(path1, fileRefId1)


        fileName1 = os.path.split(path1)[1]
        dir2, fileName2 = os.path.split(path2)

        # refer to each PBXBuildFile in each PBXSourcesBuildPhase
        startSearchIndex = 0
        for i in range(0, targetCount):
            startSearchIndex = insertSourcesBuildPhaseEntry(buildPhaseId[i], fileName1, fileName2, startSearchIndex)


        # insert both paths in the group they belong to
        matchGroupStart = re.search("/\* %s \*/ = {" % dir2, strIn)
        endGroupIndex = strIn.find("};", matchGroupStart.start())

        for matchGroupLine in re.compile(".+" + idMask + " /\* (.+) \*/,").finditer(strIn, matchGroupStart.start(), endGroupIndex) :
            if matchGroupLine.group(1) > fileName1:
                print "\tInsert paths in PBXGroup '%s', just before '%s'..." % (dir2, matchGroupLine.group(1)),
                strIn = strIn[:matchGroupLine.start()] \
                    + "\t\t\t\t%s /* %s */,\n" % (fileRefId1, fileName1) \
                    + "\t\t\t\t%s /* %s */,\n" % (fileRefId2, fileName2) \
                    + strIn[matchGroupLine.start():]
                print "OK"

                break

    elif line.endswith("*/ = {") :
        print "Delete invalid PBXGroup starting at '%s'..." % line,
        find = "};\n"
        endGroupIndex = strIn.find(find, matchLine.start()) + len(find)
        strIn = strIn[:matchLine.start()] + strIn[endGroupIndex:]
        print "OK"

    elif line.endswith(" */,") :
        print "Delete invalid PBXGroup child '%s'..." % line,
        strIn = strIn[:matchLine.start()] + strIn[matchLine.end()+1:]
        print "OK"

    matchLine = rc.search(strIn)

if testFixStage:
    print "------------------------------------------"
    print strIn
    exit(1)


# key = original ID found in project
# value = ID it will be replaced by
idDict = {}

# some of the strings to match to find definitions of Xcode IDs:

# from PBXBuildFile section:
# 0123456789ABCDEF01234567 /* filename.cpp in Sources */ = {isa = PBXBuildFile; fileRef = FEDCBA9876543210FEDCBA98 /* filename.cpp */; };

# from PBXFileReference section:
# FEDCBA9876543210FEDCBA98 /* filename.cpp */ = {isa = PBXFileReference; lastKnownFileType = file; name = any.cpp; path = ../../src/common/filename.cpp; sourceTree = "<group>"; };

# from remaining sections:
# 890123456789ABCDEF012345 /* Name */ = {

# Capture the first comment between /* and */ (file/section name) as a group
rc = re.compile("\s+(" + idMask + ") /\* (.+) \*/ = {.*$", re.MULTILINE)
dict = rc.findall(strIn)

for s in dict:
    # s[0] is the original ID, s[1] is the name
    assert(not s[0] in idDict)
    idDict[s[0]] = toUuid(s[1])


# replace all found identifiers with the new ones
def repl(match):
    return idDict[match.group(0)]

strOut = re.sub(idMask, repl, strIn)

fout = open(projectFile, "w")
fout.write(strOut)
fout.close()
