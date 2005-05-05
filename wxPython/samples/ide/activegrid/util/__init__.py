#----------------------------------------------------------------------------
# Name:         __init__.py
# Purpose:      Utilities
#
# Author:       Joel Hare
#
# Created:      7/28/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import logging
import cStringIO
import traceback
import sys
import string
import os

def classForName(className):
    pathList = className.split('.')
    moduleName = string.join(pathList[:-1], '.')
    code = __import__(moduleName)
    for name in pathList[1:]:
        code = code.__dict__[name]
    return code

def hasattrignorecase(object, name):
    for attr in dir(object):
        if attr.lower() == name.lower():
            return True
    for attr in dir(object):
        if attr.lower() == '_' + name.lower():
            return True
    return False


def setattrignorecase(object, name, value):
    for attr in object.__dict__:
        if attr.lower() == name.lower():
            object.__dict__[attr] = value
            return
##    for attr in dir(object):
##        if attr.lower() == '_' + name.lower():
##            object.__dict__[attr] = value
##            return
    object.__dict__[name] = value
    
def getattrignorecase(object, name):
    for attr in object.__dict__:
        if attr.lower() == name.lower():
            return object.__dict__[attr]
##    for attr in dir(object):
##        if attr.lower() == '_' + name.lower():
##            return object.__dict__[attr]
    return object.__dict__[name]


def defaultLoad(fileObject, knownTypes=None):
    xml = fileObject.read()
    loadedObject = xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)
    if hasattr(fileObject, 'name'):
        loadedObject.fileName = os.path.abspath(fileObject.name)
    loadedObject.initialize()
    return loadedObject

def defaultSave(fileObject, objectToSave, knownTypes=None):
    xml = xmlmarshaller.marshal(objectToSave, prettyPrint=True, knownTypes=knownTypes)
    fileObject.write(xml)
    fileObject.close()
 
def clone(objectToClone):
    xml = xmlmarshaller.marshal(objectToClone, prettyPrint=True)
    clonedObject = xmlmarshaller.unmarshal(xml)
    if hasattr(objectToClone, 'fileName'):
        clonedObject.fileName = objectToClone.fileName
    clonedObject.initialize()
    return clonedObject
    
def exceptionToString(e):
    sio = cStringIO.StringIO()
    traceback.print_exception(e.__class__, e, sys.exc_traceback, file=sio)
    return sio.getvalue()

