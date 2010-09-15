#!/usr/bin/python

###############################################################################
# Name:         build/osx/fix_xcode_ids.py
# Author:       Dimitri Schoolwerth
# Created:      2010-09-08
# RCS-Id:       $Id$
# Copyright:    (c) 2010 wxWidgets team
# Licence:      wxWindows licence
###############################################################################

import sys
import re
import uuid

USAGE = """fix_xcode_ids - Modifies an Xcode project in-place to use the same identifiers (based on name) instead of being different on each regeneration"
Usage: fix_xcode_ids xcode_proj_dir"""

if len(sys.argv) < 2:
    print USAGE
    sys.exit(1)

projectFile = sys.argv[1] + "/project.pbxproj"

fin = open(projectFile, "r")
strIn = fin.read()
fin.close()


# Xcode identifiers (IDs) consist of 24 hexadecimal digits
idMask = "[A-Fa-f0-9]{24}"

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

# convert a name to an identifier for Xcode
def toUuid(name):
    return uuid.uuid3(uuid.NAMESPACE_DNS, name).hex[:24].upper()

for s in dict:
    # s[0] is the original ID, s[1] is the name
    newId = toUuid(s[1])
    # Some names can appear twice or even more (depending on number of
    # targets), make them unique
    while newId in idDict.values() :
        # [2:-1] to skip prepended 0x and trailing L
        newId = hex(int(newId, 16) + 1)[2:-1].upper()
 
    assert(not s[0] in idDict)
    idDict[s[0]] = newId


# replace all found identifiers with the new ones
def repl(match):
    return idDict[match.group(0)]

strOut = re.sub(idMask, repl, strIn)

fout = open(projectFile, "w")
fout.write(strOut)
fout.close()
