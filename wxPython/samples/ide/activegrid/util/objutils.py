#----------------------------------------------------------------------------
# Name:         objutils.py
# Purpose:      Object Utilities
#
# Author:       Alan Mullendore
#
# Created:      5/10/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import logging
import traceback
import sys
import os

import xmlmarshaller

def defaultLoad(fileObject, knownTypes=None):
    xml = fileObject.read()
    loadedObject = xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)
    if hasattr(fileObject, 'name'):
        loadedObject.fileName = os.path.abspath(fileObject.name)
    loadedObject.initialize()
    return loadedObject

def defaultSave(fileObject, objectToSave, knownTypes=None, withEncoding=1, encoding='utf-8'):
    xml = xmlmarshaller.marshal(objectToSave, prettyPrint=True, knownTypes=knownTypes, withEncoding=withEncoding, encoding=encoding)
    fileObject.write(xml)
    fileObject.flush()
 
def clone(objectToClone, knownTypes=None, encoding='utf-8'):
    xml = xmlmarshaller.marshal(objectToClone, prettyPrint=True, knownTypes=knownTypes, encoding=encoding)
    clonedObject = xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)
    if hasattr(objectToClone, 'fileName'):
        clonedObject.fileName = objectToClone.fileName
    try:
        clonedObject.initialize()
    except AttributeError:
        pass
    return clonedObject

def classForName(className):
    pathList = className.split('.')
    moduleName = '.'.join(pathList[:-1])
    code = __import__(moduleName)
    for name in pathList[1:]:
        code = code.__dict__[name]
    return code

def hasattrignorecase(object, name):
    namelow = name.lower()
    for attr in dir(object):
        if attr.lower() == namelow:
            return True
    for attr in dir(object):
        if attr.lower() == '_' + namelow:
            return True
    return False

def setattrignorecase(object, name, value):
    namelow = name.lower()
    for attr in object.__dict__:
        if attr.lower() == namelow:
            object.__dict__[attr] = value
            return
    object.__dict__[name] = value
    
def getattrignorecase(object, name):
    namelow = name.lower()
    for attr in object.__dict__:
        if attr.lower() == namelow:
            return object.__dict__[attr]
    return object.__dict__[name]

def hasPropertyValue(obj, attr):
    hasProp = False
    try:
        prop = obj.__class__.__dict__[attr]
        if (isinstance(prop, property)):
            hasProp = hasattr(obj, attr)
            if (hasProp):
                # It's a property and it has a value but sometimes we don't want it.
                # If there is a _hasattr method execute it and the
                # result will tell us whether to include this value
                try:
                    hasProp = obj._hasattr(attr)
                except:
                    pass
    except KeyError:
        pass
    return hasProp
