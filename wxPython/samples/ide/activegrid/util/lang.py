#----------------------------------------------------------------------------
# Name:         lang.py
# Purpose:      Active grid language specific utilities -- provides portability
#               for common idiom's that have language specific implementations
#
# Author:       Jeff Norton
#
# Created:      04/27/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

def isMain(caller):
    return caller == '__main__'

def ag_className(obj):
    return obj.__class__.__name__
    
def asDict(src):
    return src
    
def asList(src):
    return src
    
def asTuple(src):
    return src
    
def asString(src):
    return src
    
def asInt(src):
    return src
    
def asBool(src):
    return src
    
def asObject(src):
    return src
    
def cast(src, type):
    return src
    
def asRef(src):
    return src
    
def asClass(src):
    return src
    
def localize(text):
    return text

# Pass in Python code as a string.  The cross-compiler will convert to PHP 
# and in-line the result.
def pyToPHP(expr):
    pass

# Pass in PHP code as a string.  The cross-compiler will drop it in-line verbatim.
def PHP(expr):
    pass
    
# Bracket Python only code.  The Cross-compiler will ignore the bracketed code.
def ifDefPy(comment=False):
    pass
    
def endIfDef():
    pass
    
def ag_isPHP():
    return False
    
def ag_isPython():
    return True
