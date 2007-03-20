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
import __builtin__
import types
import activegrid.util.utillang as utillang
import activegrid.util.datetimeparser as datetimeparser
from types import *
from activegrid.util.lang import *

FUNCTION_HAS_ATTR = '_hasAttr'
FUNCTION_GET_ATTR = '_getAttr'
FUNCTION_SET_ATTR = '_setAttr'
FUNCTION_DEL_ATTR = '_delAttr'

def hasRawAttr(obj, name):
    if obj == None:
        return False
    if name != FUNCTION_HAS_ATTR and hasattr(obj, FUNCTION_HAS_ATTR):
        return obj._hasAttr(name)
    return obj.__dict__.has_key(name)
    
def getRawAttr(obj, name):
    if name != FUNCTION_GET_ATTR and hasattr(obj, FUNCTION_GET_ATTR):
        return obj._getAttr(name)
    return obj.__dict__.get(name)
    
def setRawAttr(obj, name, value):
    if name != FUNCTION_SET_ATTR and hasattr(obj, FUNCTION_SET_ATTR):
        obj._setAttr(name, value)
    else:
        obj.__dict__[name] = value
    
def delRawAttr(obj, name):
    if name != FUNCTION_DEL_ATTR and hasattr(obj, FUNCTION_DEL_ATTR):
        obj._delAttr(name)
    else:
        del obj.__dict__[name]

def getStaticAttr(obj, attr):
    if (isinstance(obj, types.TypeType)):
        classDesc = obj
    else:
        classDesc = obj.__class__
    if (hasattr(classDesc, attr)):
        return getattr(classDesc, attr)
    return None
    
def setStaticAttr(obj, attr, value):
    if (isinstance(obj, types.TypeType)):
        classDesc = obj
    else:
        classDesc = obj.__class__
    setattr(classDesc, attr, value)

def hasAttrFast(obj, name):
    if hasRawAttr(obj, name):
        return True
    if hasattr(obj, '_complexType'):
        complexType=obj._complexType
        element=complexType.findElement(name)
        if element:
            return True
    if hasattr(obj, name):
        return True
    return False

def moduleForName(moduleName):
    module = None
    pathList = moduleName.split('.')
    if (len(moduleName) > 0):
        module = __import__(moduleName)
        for name in pathList[1:]:
            if (name in module.__dict__):
                module = module.__dict__[name]
            else:
                module = None
                break
    return module
    
def typeForName(typeName):
    i = typeName.rfind('.')
    if (i >= 0):
        module = moduleForName(typeName[:i])
        if (module != None):
            name = typeName[i+1:]
            if (name in module.__dict__):
                return module.__dict__[name]
    elif __builtin__.__dict__.has_key(typeName):
        return __builtin__.__dict__[typeName]
    return None
    
def functionForName(functionName):
    ftype = typeForName(functionName)
    if (isinstance(ftype, (types.FunctionType, types.MethodType, types.BuiltinFunctionType, types.BuiltinMethodType))):
        return ftype
    return None
    
def classForName(className):
    ctype = typeForName(className)
    if (isinstance(ctype, (types.ClassType, types.TypeType))):
        return ctype
    return None

def newInstance(className, objargs=None):
    "dynamically create an object based on the className and return it."

    if not isinstance(objargs, list):
        objargs = [objargs]

    if className == "None":
        return None
    elif className == "bool":
        if ((len(objargs) < 1) or (objargs[0].lower() == "false") or (not objargs[0])):
            return False
        return True
    if className == "str" or className == "unicode": # don't strip: blanks are significant
        if len(objargs) > 0:
            try:
                return utillang.unescape(objargs[0]).encode()
            except:
                return "?"
        else:
            return ""
            
    if className == "date":
        return datetimeparser.parse(objargs[0], asdate=True)
    if className == "datetime":
        return datetimeparser.parse(objargs[0])
    if className == "time":
        return datetimeparser.parse(objargs[0], astime=True)
        
    classtype = classForName(className)
    if (classtype == None):
        raise Exception("Could not find class %s" % className)
        
    if (len(objargs) > 0):
        return classtype(*objargs)
    else:
        return classtype()

def getClassProperty(classType, propertyName):
    return getattr(classType, propertyName)
    
def toDiffableRepr(value, maxLevel=None):
    if (value == None):
        return "None"
    if (maxLevel == None):
        maxLevel = 8
    maxLevel -= 1
    if (maxLevel < 0):
        return typeToString(value, PRINT_OBJ_DIFFABLE)
##    if ((exclude != None) and not isinstance(value, (basestring, int))):
##        for v in exclude:
##            if (v is value):
##                return "<recursive reference>"
##        exclude.append(value)
##    elif (isinstance(value, ObjectType) and hasattr(value, "__dict__")):
##        if (exclude == None):
##            exclude = []
##        s = "%s(%s)" % (type(value), toDiffableString(value.__dict__, exclude))
    if (not isinstance(value, (BooleanType, ClassType, ComplexType, DictType, DictionaryType, 
                               FloatType, IntType, ListType, LongType, StringType, TupleType, 
                               UnicodeType, BufferType, BuiltinFunctionType, BuiltinMethodType,
                               CodeType, FrameType, FunctionType, GeneratorType, InstanceType,
                               LambdaType, MethodType, ModuleType, SliceType, TracebackType,
                               TypeType, XRangeType))):
        if (hasattr(value, "_toDiffableString")):
            s = value._toDiffableString(maxLevel)
        elif (hasattr(value, "__str__")):
            s = str(value)
        elif (hasattr(value, "__dict__")):
            s = "%s(%s)" % (type(value), toDiffableString(value.__dict__, maxLevel))
        else:
            s = str(type(value))
        ix2 = s.find(" object at 0x")
        if (ix2 > 0):
            ix = s.rfind(".")
            if (ix > 0):
                s = "<class %s>" %s[ix+1:ix2]
    elif (isinstance(value, bool)):
        if (value):
            return "True"
        else:
            return "False"
    elif (isinstance(value, (tuple, list))):
        items = []
        for v in value:
            if (isinstance(v, basestring)):
                if (v.find("'") >= 0):
                    items.append('"%s"' % v)
                else:
                    items.append("'%s'" % v)
            else:
                items.append(toDiffableString(v, maxLevel))
        s = "[" + ", ".join(items) + "]"
    elif (isinstance(value, dict)):
        items = []
        for key, val in value.iteritems():
            if (isinstance(val, UnicodeType)):
                items.append("'%s': u'%s'" % (key, toDiffableString(val, maxLevel)))
            elif (isinstance(val, basestring)):
                items.append("'%s': '%s'" % (key, toDiffableString(val, maxLevel)))
            else:
                items.append("'%s': %s" % (key, toDiffableString(val, maxLevel)))
        s = "{" + ", ".join(items) + "}"
    else:
        s = str(value)
    return s
    
def toDiffableString(value, maxLevel=None):
##    if (value == None):
##        return "None"
##    if ((exclude != None) and not isinstance(value, (basestring, int))):
##        for v in exclude:
##            if (v is value):
##                return "<recursive reference>"
##        exclude.append(value)
    s = toDiffableRepr(value, maxLevel)
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
    ds = ds + s[start:]
    i = ds.find("\\src\\")
    if (i < 0):
        i = ds.find("/src/")
    else:
        ds = ds.replace("\\", "/")
    if (i > 0):
        i += 4
        if (ds[i:i+5] == "\\php\\"):
            i += 4
        elif (ds[i:i+8] == "\\python\\"):
            i += 7
        ds = "filepath: ..." + ds[i:]
    return ds
        
def toString(value, options=0):
    if ((options & PRINT_OBJ_DIFFABLE) > 0):
        return toDiffableString(value)
    elif (not isinstance(value, basestring)):
        return str(value)
    return value

def typeToString(obj, options=0):
    if (isinstance(obj, BooleanType)):
        return "bool"
    elif (isinstance(obj, UnicodeType)):
        if ((options & PRINT_OBJ_DIFFABLE) > 0):
            return "string"
        return "unicode"
    elif (isinstance(obj, basestring)):
        return "string"
    elif (isinstance(obj, IntType)):
        return "int"
    elif (isinstance(obj, LongType)):
        if ((options & PRINT_OBJ_DIFFABLE) > 0):
            return "int"
        return "long"
    elif (isinstance(obj, FloatType)):
        return "float"
    elif (type(obj) == ListType):
        return "list"
    elif (isinstance(obj, DictType)):
        return "dict"
    elif (isinstance(obj, TupleType)):
        return "tuple"
    elif (isinstance(obj, InstanceType)):
##        ds = str(type(obj))
        ds = "<class %s.%s> " % (obj.__module__, obj.__class__.__name__)
    else:
        ds = str(type(obj))
    if (options == 0):
        import activegrid.util.aglogging
        options = activegrid.util.aglogging.testMode(0, PRINT_OBJ_DIFFABLE)
    if ((options & PRINT_OBJ_DIFFABLE) > 0):
        if (ds.startswith("<class ")):
            ix = ds.rfind(".")
            if (ix < 0):
                ix = 8
            ds = "<class %s>" % ds[ix+1:-2]
    return ds
    
def nameToString(name, options=0):
    if (name.startswith("_v_")):
        return name[3:]
    if ((options & PRINT_OBJ_DIFFABLE) > 0):
        ix = name.find("__")
        if ((ix > 1) and name.startswith("_")):
            name = name[ix:]
        return toDiffableString(name)
    return name
            
PRINT_OBJ_GETATTR = 1
PRINT_OBJ_HIDE_INTERNAL = 2
PRINT_OBJ_COMPACT = 4
PRINT_OBJ_NONONE = 8
PRINT_OBJ_DIFFABLE = 16
PRINT_OBJ_HIDE_EXCLUDED = 32
PRINT_OBJ_INTERNAL = 512

def printObject(out, object, name=None, indent=0, flags=0, exclude=None, remove=None, maxIndent=30):
    if (name == None):
        name = ""
##    elif (name.endswith("_") and not name.endswith("__")):
##        name = name[:-1]
    if ((remove != None) and (name in asDict(remove))):
        return False
    if ((maxIndent != None) and (indent > maxIndent)):
        print >> out, " "*indent, "%s: %s" % (name, toString(str(object), flags)),
        if ((flags & PRINT_OBJ_INTERNAL) == 0):
            print >> out
        return True
    finalNewLine = False
    printed = True
    if ((flags & (PRINT_OBJ_COMPACT | PRINT_OBJ_HIDE_EXCLUDED)) > 0):
        if ((exclude != None) and ((object in exclude) or (name in exclude))):
            return
        if ((flags & PRINT_OBJ_COMPACT) > 0):
            indent = 0
    if ((flags & PRINT_OBJ_INTERNAL) == 0):
        finalNewLine = True
    flags |= PRINT_OBJ_INTERNAL
    if (object is None):
        if (flags & PRINT_OBJ_NONONE) == 0:
            print >> out, " "*indent, name, " = None",
        else:
            finalNewLine = False
            printed = False
    elif (name.startswith("_") and ((flags & PRINT_OBJ_HIDE_INTERNAL) > 0) and not name.startswith("_v_")):
        finalNewLine = False
        printed = False
    elif (isinstance(object, (list, tuple))):
        if ((exclude != None) and object in exclude):
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " of length = ", len(object), " (already printed)",
        elif ((exclude != None) and name in exclude):
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " of length = ", len(object), " (excluded)",
        else:
            if ((exclude != None) and (len(object) > 0)): exclude.append(object)
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " of length = %d" % len(object),
            for i, o in enumerate(object):
                print >> out
                printObject(out, o, name="[%d]" % i, indent=indent+2, flags=flags, exclude=exclude, remove=remove, maxIndent=maxIndent)
    elif (isinstance(object, dict)):
        if ((exclude != None) and object in exclude):
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " (already printed)",
        else:
            if ((exclude != None) and (len(object) > 0)): exclude.append(object)
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
            for key in keys:
                if (key != None):
                    n = key
                    if (not (isinstance(n, basestring))):
                        n = str(n)
                    else:
                        n = nameToString(n, flags)
                    if ((not n.startswith("_") or ((flags & PRINT_OBJ_HIDE_INTERNAL) == 0))):
                        if printObject(out, object[key], name=n, indent=indent+2, flags=(flags | PRINT_OBJ_INTERNAL), exclude=exclude, remove=remove, maxIndent=maxIndent):
                            if ((flags & PRINT_OBJ_COMPACT) == 0):
                                print >> out
                            else:
                                print >> out, ",",
            print >> out, " "*indent, "}",
    elif (hasattr(object, "__dict__")):
        if (name.startswith("_")): ## and ((flags & PRINT_OBJ_HIDE_INTERNAL) > 0)):
            print >> out, " "*indent, name, " : ", typeToString(object, flags),
        elif ((exclude != None) and ((object in exclude) or (object.__dict__ in exclude))):
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " (already printed)",
        else:
            if (exclude != None): exclude.append(object)
            print >> out, " "*indent, name, " : ", typeToString(object, flags),
            if ((flags & PRINT_OBJ_GETATTR) == 0):
                if ((flags & PRINT_OBJ_COMPACT) == 0):
                    print >> out
                printObject(out, object.__dict__, indent=indent, flags=flags, exclude=exclude, remove=remove, maxIndent=maxIndent)
            else:
                if ((flags & PRINT_OBJ_COMPACT) == 0):
                    print >> out
##                    indent += 2
                print >> out, " "*indent, "{",
                keys = object.__dict__.keys()
                keys.sort()
                printed = True
                for key in keys:
                    if ((exclude != None) and (key in exclude)):
                        continue
                    if (printed and ((flags & PRINT_OBJ_COMPACT) == 0)):
                        print >> out
                    n = nameToString(key, flags)
                    printed = printObject(out, getattr(object, n), name=n, indent=indent+2, flags=flags, exclude=exclude, remove=remove, maxIndent=maxIndent)
                if ((flags & PRINT_OBJ_COMPACT) == 0):
                    print >> out
                print >> out, " "*indent, "}",
    elif (indent < 0):
        print >> out, object,
    elif isinstance(object, basestring):
        if ((exclude != None) and name in exclude):
            print >> out, " "*indent, name, " : ", typeToString(object, flags), " of length = ", len(object), " (excluded)",
        elif (len(object) > 100):
            object = toString(object, flags)
            print >> out, " "*indent, name, ":", typeToString(object, flags), "[%d] = %s...%s" % (len(object), object[:50], object[-50:]),
        else:
            print >> out, " "*indent, name, ":", typeToString(object, flags), "=", toString(object, flags),
##    elif (isinstance(object, float)):
##        val = str(object)
##        if (len(val) > 17):
##            val = val[:17]
##        print >> out, " "*indent, name, ":", type(object), "=", val,
    else:
        print >> out, " "*indent, name, ":", typeToString(object, flags), "=", toString(object, flags),
    if (finalNewLine):
        print >> out
    return printed
