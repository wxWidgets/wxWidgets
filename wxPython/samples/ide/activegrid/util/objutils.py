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

AG_TYPE_MAPPING =    { "ag:append"      : "activegrid.model.processmodel.AppendOperation",
                       "ag:body"        : "activegrid.model.processmodel.Body",
                       "ag:copy"        : "activegrid.model.processmodel.CopyOperation",
                       "ag:cssRule"     : "activegrid.model.processmodel.CssRule",
                       "ag:datasource"  : "activegrid.data.dataservice.DataSource",
                       "ag:debug"       : "activegrid.model.processmodel.DebugOperation",
                       "ag:deployment"  : "activegrid.server.deployment.Deployment",
                       "ag:glue"        : "activegrid.model.processmodel.Glue",
                       "ag:hr"          : "activegrid.model.processmodel.HorizontalRow",
                       "ag:image"       : "activegrid.model.processmodel.Image",
                       "ag:inputs"      : "activegrid.model.processmodel.Inputs",
                       "ag:label"       : "activegrid.model.processmodel.Label",
                       "ag:processmodel": "activegrid.model.processmodel.ProcessModel",
                       "ag:processmodelref" : "activegrid.server.deployment.ProcessModelRef",
                       "ag:query"       : "activegrid.model.processmodel.Query",
                       "ag:schemaOptions" : "activegrid.model.schema.SchemaOptions",
                       "ag:schemaref"   : "activegrid.server.deployment.SchemaRef",
                       "ag:set"         : "activegrid.model.processmodel.SetOperation",
                       "ag:text"        : "activegrid.model.processmodel.Text",
                       "ag:title"       : "activegrid.model.processmodel.Title",
                       "ag:view"        : "activegrid.model.processmodel.View",
                       "bpws:case"      : "activegrid.model.processmodel.BPELCase",
                       "bpws:catch"     : "activegrid.model.processmodel.BPELCatch",
                       "bpws:faultHandlers" : "activegrid.model.processmodel.BPELFaultHandlers",
                       "bpws:invoke"    : "activegrid.model.processmodel.BPELInvoke",
                       "bpws:onMessage" : "activegrid.model.processmodel.BPELOnMessage",
                       "bpws:otherwise" : "activegrid.model.processmodel.BPELOtherwise",
                       "bpws:pick"      : "activegrid.model.processmodel.BPELPick",
                       "bpws:process"   : "activegrid.model.processmodel.BPELProcess",
                       "bpws:receive"   : "activegrid.model.processmodel.BPELReceive",
                       "bpws:reply"     : "activegrid.model.processmodel.BPELReply",
                       "bpws:scope"     : "activegrid.model.processmodel.BPELScope",
                       "bpws:sequence"  : "activegrid.model.processmodel.BPELSequence",
                       "bpws:switch"    : "activegrid.model.processmodel.BPELSwitch",
                       "bpws:terminate" : "activegrid.model.processmodel.BPELTerminate",
                       "bpws:variable"  : "activegrid.model.processmodel.BPELVariable",
                       "bpws:variables" : "activegrid.model.processmodel.BPELVariables",
                       "bpws:while"     : "activegrid.model.processmodel.BPELWhile",
                       "wsdl:message"   : "activegrid.model.processmodel.WSDLMessage",
                       "wsdl:part"      : "activegrid.model.processmodel.WSDLPart",
                       "xforms:group"   : "activegrid.model.processmodel.XFormsGroup",
                       "xforms:input"   : "activegrid.model.processmodel.XFormsInput",
                       "xforms:label"   : "activegrid.model.processmodel.XFormsLabel",
                       "xforms:output"  : "activegrid.model.processmodel.XFormsOutput",
                       "xforms:secret"  : "activegrid.model.processmodel.XFormsSecret",
                       "xforms:submit"  : "activegrid.model.processmodel.XFormsSubmit",
                       "xs:all"         : "activegrid.model.schema.XsdSequence",
                       "xs:complexType" : "activegrid.model.schema.XsdComplexType",
                       "xs:element"     : "activegrid.model.schema.XsdElement",
                       "xs:field"       : "activegrid.model.schema.XsdKeyField",
                       "xs:key"         : "activegrid.model.schema.XsdKey",
                       "xs:keyref"      : "activegrid.model.schema.XsdKeyRef",
                       "xs:schema"      : "activegrid.model.schema.Schema",
                       "xs:selector"    : "activegrid.model.schema.XsdKeySelector",              
                       "xs:sequence"    : "activegrid.model.schema.XsdSequence",
                       "projectmodel"   : "activegrid.tool.ProjectEditor.ProjectModel",
                     }

def defaultLoad(fileObject, knownTypes=None):
    xml = fileObject.read()
    loadedObject = defaultUnmarshal(xml, knownTypes=knownTypes)
    if hasattr(fileObject, 'name'):
        loadedObject.fileName = os.path.abspath(fileObject.name)
    loadedObject.initialize()
    return loadedObject

def defaultUnmarshal(xml, knownTypes=None):
    if not knownTypes: knownTypes = AG_TYPE_MAPPING
    return xmlmarshaller.unmarshal(xml, knownTypes=knownTypes)    

def defaultSave(fileObject, objectToSave, prettyPrint=True, knownTypes=None, withEncoding=1, encoding='utf-8'):
    xml = defaultMarshal(objectToSave, prettyPrint=prettyPrint, knownTypes=knownTypes, withEncoding=withEncoding, encoding=encoding)
    fileObject.write(xml)
    fileObject.flush()

def defaultMarshal(objectToSave, prettyPrint=True, knownTypes=None, withEncoding=1, encoding='utf-8'):
    if not knownTypes: knownTypes = AG_TYPE_MAPPING
    return xmlmarshaller.marshal(objectToSave, prettyPrint=prettyPrint, knownTypes=knownTypes, withEncoding=withEncoding, encoding=encoding)
    
def clone(objectToClone, knownTypes=None, encoding='utf-8'):
    if not knownTypes: knownTypes = AG_TYPE_MAPPING
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

def toDiffableString(value):
    s = repr(value)
    ds = ""
    i = s.find(" at 0x") 
    start = 0
    while (i >= 0):
        j = s.find(">", i)
        if (j < i):
            break
        ds += s[start:i]
        start = j
        i = s.find(" at 0x", start) 
    return ds + s[start:]
    
PRINT_OBJ_GETATTR = 1
PRINT_OBJ_HIDE_INTERNAL = 2
PRINT_OBJ_COMPACT = 4
PRINT_OBJ_NONONE = 8
PRINT_OBJ_INTERNAL = 512

def printObject(out, object, name="", indent=0, flags=0, exclude=None, maxIndent=30):
    if ((maxIndent != None) and (indent > maxIndent)):
        print >> out, " "*indent, name, str(object)
        return True
    finalNewLine = False
    printed = True
    if ((flags & PRINT_OBJ_COMPACT) > 0):
        if (exclude and object in exclude):
            return
        indent = 0
    if ((flags & PRINT_OBJ_INTERNAL) == 0):
        finalNewLine = True
    flags |= PRINT_OBJ_INTERNAL
    if (object == None):
        if (flags & PRINT_OBJ_NONONE) == 0:
            print >> out, " "*indent, name, " = None",
        else:
            finalNewLine = False
            printed = False
    elif (name.startswith("_") and ((flags & PRINT_OBJ_HIDE_INTERNAL) > 0)):
        finalNewLine = False
        printed = False
    elif (isinstance(object, (list, tuple))):
        if (exclude and object in exclude):
            print >> out, " "*indent, name, " : ", type(object), " of length = ", len(object), " (already printed)",
        elif (exclude and name in exclude):
            print >> out, " "*indent, name, " : ", type(object), " of length = ", len(object), " (excluded)",
        else:
            if (exclude != None): exclude.append(object)
            print >> out, " "*indent, name, " : ", type(object), " of length = %i" % len(object),
            for i, o in enumerate(object):
                print >> out
                printObject(out, o, name="[%i]" % i, indent=indent+2, flags=flags, exclude=exclude, maxIndent=maxIndent)
    elif (isinstance(object, dict)):
        if (exclude and object in exclude):
            print >> out, " "*indent, name, " : ", type(object), " (already printed)",
        else:
            if (exclude != None): exclude.append(object)
            if (len(name) > 0):
                print >> out, " "*indent, name,
                if ((flags & PRINT_OBJ_COMPACT) == 0):
                    print >> out
                    indent += 2
            print >> out, " "*indent, "{",
            if ((flags & PRINT_OBJ_COMPACT) == 0):
                print >> out
            keys = object.keys()
            keys.sort()
            for n in keys:
                if ((n != None) and (not n.startswith("_") or ((flags & PRINT_OBJ_HIDE_INTERNAL) == 0))):
                    if printObject(out, object[n], name=n, indent=indent+2, flags=flags, exclude=exclude, maxIndent=maxIndent):
                        if ((flags & PRINT_OBJ_COMPACT) == 0):
                            print >> out
                        else:
                            print >> out, ",",
            print >> out, " "*indent, "}",
    elif (hasattr(object, "__dict__")):
        if (exclude and object in exclude):
            print >> out, " "*indent, name, " : ", type(object), " (already printed) = ", toDiffableString(object),
        else:
            if (exclude != None): exclude.append(object)
            if (name.startswith("_")):
                print >> out, " "*indent, name, " : ", type(object),
            elif (exclude and object.__dict__ in exclude):
                print >> out, " "*indent, name, " : ", type(object), " (already printed)",
            else:
                print >> out, " "*indent, name, " : ", type(object),
                if ((flags & PRINT_OBJ_GETATTR) == 0):
                    if ((flags & PRINT_OBJ_COMPACT) == 0):
                        print >> out
                    printObject(out, object.__dict__, indent=indent, flags=flags, exclude=exclude, maxIndent=maxIndent)
                else:
                    keys = object.__dict__.keys()
                    keys.sort()
                    for n in keys:
                        if ((flags & PRINT_OBJ_COMPACT) == 0):
                            print >> out
                        printObject(out, getattr(object, n), name=n, indent=indent+2, flags=flags, exclude=exclude, maxIndent=maxIndent)
    elif (indent < 0):
        print >> out, object,
    elif isinstance(object, basestring):
        if (exclude and name in exclude):
            print >> out, " "*indent, name, " : ", type(object), " of length = ", len(object), " (excluded)",
        elif (len(object) > 100):
            print >> out, " "*indent, name, ":", type(object), "[%i] = %s...%s" % (len(object), object[:50], object[-50:]),
        else:
            print >> out, " "*indent, name, ":", type(object), "=", str(object),
    else:
        print >> out, " "*indent, name, ":", type(object), "=", str(object),
    if (finalNewLine):
        print >> out
    return printed
