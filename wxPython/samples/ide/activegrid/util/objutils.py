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
from types import *

def classForName(className):
    pathList = className.split('.')
    moduleName = '.'.join(pathList[:-1])
    code = __import__(moduleName)
    for name in pathList[1:]:
        code = code.__dict__[name]
    return code

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
    s = str(value)
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

def toString(value, options=0):
    if ((options & PRINT_OBJ_DIFFABLE) > 0):
        return toDiffableString(value)
    return value

def toTypeString(obj):
    if (isinstance(obj, BooleanType)):
        return "bool"
    elif (isinstance(obj, UnicodeType)):
        return "unicode"
    elif (isinstance(obj, basestring)):
        return "string"
    elif (isinstance(obj, IntType)):
        return "int"
    elif (isinstance(obj, FloatType)):
        return "float"
    elif (type(obj) == ListType):
        return "list"
    elif (isinstance(obj, DictType)):
        return "dict"
    elif (isinstance(obj, TupleType)):
        return "tuple"
    elif (isinstance(obj, InstanceType)):
        return type(obj)
    else:
        return type(obj)
            
PRINT_OBJ_GETATTR = 1
PRINT_OBJ_HIDE_INTERNAL = 2
PRINT_OBJ_COMPACT = 4
PRINT_OBJ_NONONE = 8
PRINT_OBJ_DIFFABLE = 16
PRINT_OBJ_INTERNAL = 512

def printObject(out, object, name="", indent=0, flags=0, exclude=None, maxIndent=30):
    if ((maxIndent != None) and (indent > maxIndent)):
        print >> out, " "*indent, "%s: %s" % (name, toString(str(object), flags)),
        if ((flags & PRINT_OBJ_INTERNAL) == 0):
            print >> out
        return True
    finalNewLine = False
    printed = True
##    if (exclude == None):
##        exclude = []
    if ((flags & PRINT_OBJ_COMPACT) > 0):
        if (exclude and object in exclude):
            return
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
    elif (name.startswith("_") and ((flags & PRINT_OBJ_HIDE_INTERNAL) > 0)):
        finalNewLine = False
        printed = False
    elif (isinstance(object, (list, tuple))):
        if ((exclude != None) and object in exclude):
            print >> out, " "*indent, name, " : ", toTypeString(object), " of length = ", len(object), " (already printed)",
        elif ((exclude != None) and name in exclude):
            print >> out, " "*indent, name, " : ", toTypeString(object), " of length = ", len(object), " (excluded)",
        else:
            if ((exclude != None) and (len(object) > 0)): exclude.append(object)
            print >> out, " "*indent, name, " : ", toTypeString(object), " of length = %d" % len(object),
            for i, o in enumerate(object):
                print >> out
                printObject(out, o, name="[%d]" % i, indent=indent+2, flags=flags, exclude=exclude, maxIndent=maxIndent)
    elif (isinstance(object, dict)):
        if ((exclude != None) and object in exclude):
            print >> out, " "*indent, name, " : ", toTypeString(object), " (already printed)",
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
                    if ((not n.startswith("_") or ((flags & PRINT_OBJ_HIDE_INTERNAL) == 0))):
                        if printObject(out, object[key], name=n, indent=indent+2, flags=(flags | PRINT_OBJ_INTERNAL), exclude=exclude, maxIndent=maxIndent):
                            if ((flags & PRINT_OBJ_COMPACT) == 0):
                                print >> out
                            else:
                                print >> out, ",",
            print >> out, " "*indent, "}",
    elif (hasattr(object, "__dict__")):
        if ((exclude != None) and object in exclude):
            print >> out, " "*indent, name, " : ", toTypeString(object), " (already printed) = ", toDiffableString(object),
        else:
            if (exclude != None): exclude.append(object)
            if (name.startswith("_")): ## and ((flags & PRINT_OBJ_HIDE_INTERNAL) > 0)):
                print >> out, " "*indent, name, " : ", toTypeString(object),
            elif ((exclude != None) and object.__dict__ in exclude):
                print >> out, " "*indent, name, " : ", toTypeString(object), " (already printed)",
            else:
                print >> out, " "*indent, name, " : ", toTypeString(object),
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
        if ((exclude != None) and name in exclude):
            print >> out, " "*indent, name, " : ", toTypeString(object), " of length = ", len(object), " (excluded)",
        elif (len(object) > 100):
            print >> out, " "*indent, name, ":", toTypeString(object), "[%d] = %s...%s" % (len(object), object[:50], object[-50:]),
        else:
            print >> out, " "*indent, name, ":", toTypeString(object), "=", str(object),
##    elif (isinstance(object, float)):
##        val = str(object)
##        if (len(val) > 17):
##            val = val[:17]
##        print >> out, " "*indent, name, ":", type(object), "=", val,
    else:
        print >> out, " "*indent, name, ":", toTypeString(object), "=", str(object),
    if (finalNewLine):
        print >> out
    return printed
