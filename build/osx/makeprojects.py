#!/usr/bin/python

import sys
import os
import shutil

import xml.etree.ElementTree as ET

from pbxproj import XcodeProject
from pbxproj.pbxextensions import ProjectFiles
ProjectFiles._FILE_TYPES['.cxx'] = ('sourcecode.cpp.cpp', 'PBXSourcesBuildPhase')

from fix_xcode_ids import processFile

bklfiles = ["../bakefiles/files.bkl", "../bakefiles/zlib.bkl", "../bakefiles/regex.bkl", "../bakefiles/tiff.bkl",
            "../bakefiles/png.bkl", "../bakefiles/jpeg.bkl", "../bakefiles/scintilla.bkl", "../bakefiles/lexilla.bkl",
            "../bakefiles/expat.bkl"]
nodes = [
    # xcode group, entries[], targets []
    ["base", ["$(BASE_SRC)"], ["dynamic", "static", "base"]],
    ["base", ["$(BASE_AND_GUI_SRC)"], ["dynamic", "static", "base", "core"]],
    ["core", ["$(CORE_SRC)"], ["dynamic", "static", "core"]],
    ["net", ["$(NET_SRC)"], ["dynamic", "static", "net"]],
    ["adv", ["$(ADVANCED_SRC)"], ["dynamic", "static", "adv"]],
    ["webview", ["$(WEBVIEW_SRC)"], ["dynamic", "static", "webview"]],
    ["media", ["$(MEDIA_SRC)"], ["dynamic", "static", "media"]],
    ["html", ["$(HTML_SRC)"], ["dynamic", "static", "html"]],
    ["xrc", ["$(XRC_SRC)"], ["dynamic", "static", "xrc"]],
    ["qa", ["$(QA_SRC)"], ["dynamic", "static", "qa"]],
    ["xml", ["$(XML_SRC)"], ["dynamic", "static", "xml"]],
    ["opengl", ["$(OPENGL_SRC)"], ["dynamic", "static", "gl"]],
    ["aui", ["$(AUI_SRC)"], ["dynamic", "static", "aui"]],
    ["ribbon", ["$(RIBBON_SRC)"], ["dynamic", "static", "ribbon"]],
    ["propgrid", ["$(PROPGRID_SRC)"], ["dynamic", "static", "propgrid"]],
    ["richtext", ["$(RICHTEXT_SRC)"], ["dynamic", "static", "richttext"]],
    ["stc", ["$(STC_SRC)"], ["dynamic", "static", "stc"]],
    ["libzlib", ["$(wxzlib)"], ["dynamic", "static", "wxzlib"]],
    ["libtiff", ["$(wxtiff)"], ["dynamic", "static", "wxtiff"]],
    ["libjpeg", ["$(wxjpeg)"], ["dynamic", "static", "wxjpeg"]],
    ["libpng", ["$(wxpng)"], ["dynamic", "static", "wxpng"]],
    ["libregex", ["$(wxregex)"], ["dynamic", "static", "wxregex"]],
    ["libscintilla", ["$(wxscintilla)"], ["dynamic", "static", "wxscintilla"]],
    ["liblexilla", ["$(wxlexilla)"], ["dynamic", "static", "wxlexilla"]],
    ["libexpat", ["$(wxexpat)"], ["dynamic", "static", "wxexpat"]]
]

def addNode(project, groupName, entries, fileGroups, targets):
    group = project.get_or_create_group(groupName)
    for entry in entries:
        if entry.startswith("$("):
            varname = entry[2:-1]
            addNode(project, groupName, fileGroups[varname], fileGroups, targets)
        else:
            project.add_file("../../"+entry, parent=group, target_name=targets)


def populateProject(projectfile, fileGroups, nodes):
    project = XcodeProject.load(projectfile)
    for node in nodes:
        groupName = node[0]
        entries = node[1]
        targets = node[2]
        addNode(project, groupName, entries, fileGroups, targets)
    project.save()


def parseSources(theName, xmlNode, conditions, fileGroups):
    files = xmlNode.text
    for ifs in xmlNode.findall("if"):
        condition = ifs.attrib['cond']
        if condition in conditions:
            files += ifs.text
    fileList = files.split() if files != None else []
    fileGroups[theName] = fileList


def parseFile(bklFile, conditions, fileGroups):
    tree = ET.parse(os.path.join(osxBuildFolder, bklFile))
    for elem in tree.iter():
        if elem.tag == 'set':
            theName = elem.attrib['var']
            parseSources(theName, elem, conditions, fileGroups)
        elif elem.tag == 'lib':
            theName = elem.attrib['id']
            parseSources(theName, elem.find("sources"), conditions, fileGroups)


def readFilesList(bklFileList, conditions):
    fileGroups = {}
    for bklFile in bklFileList:
        parseFile(bklFile, conditions, fileGroups)
    return fileGroups


def makeProject(projectName, conditions):
    # make new copy from template
    template = os.path.join(osxBuildFolder, projectName + "_in.xcodeproj")
    projectFile = os.path.join(osxBuildFolder, projectName + ".xcodeproj")
    if os.path.exists(projectFile):
        shutil.rmtree(projectFile)
    shutil.copytree(template, projectFile)
    # read file list from bkls
    fileGroups = readFilesList(bklfiles, conditions)
    # create xcode project
    populateProject(projectFile + "/project.pbxproj", fileGroups, nodes)
    processFile(projectFile + "/project.pbxproj")

osxBuildFolder = os.getcwd()

makeProject("wxcocoa", ["PLATFORM_MACOSX=='1'", "TOOLKIT=='OSX_COCOA'", "WXUNIV=='0'", "USE_GUI=='1' and WXUNIV=='0'"])
makeProject("wxiphone", ["PLATFORM_MACOSX=='1'", "TOOLKIT=='OSX_IPHONE'", "WXUNIV=='0'", "USE_GUI=='1' and WXUNIV=='0'"])
