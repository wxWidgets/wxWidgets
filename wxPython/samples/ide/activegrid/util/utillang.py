#----------------------------------------------------------------------------
# Name:         utillang.py
# Purpose:      Provide language specific utilities 
#
# Author:       Joel Hare
#
# Created:      8/23/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import os
import sys
import UserDict
import tempfile
import xml.sax.saxutils as saxutils

import activegrid.util.parser as parser

PY2WEB_codepages = {
     'cp1251' : 'CP-1251',
     'koi8_r' : 'KOI8-R',
}

def evalXPath(xpath, data, specialEntries=None):
    codeStr = parser.xpathToCode(xpath)
    return evalCode(codeStr, data, specialEntries)

def evalCode(codeStr, data, specialEntries=None):
    if isinstance(data, ObjAsDict):
        namespace = data
    elif isinstance(data, dict):
        namespace = dict(data)
    else:
        namespace = ObjAsDict(data)
    if specialEntries:
        for key, value in specialEntries.items():
            namespace.addSpecialEntry(key, value)
    return eval(codeStr, {}, namespace)

def deriveCharset():
    charset = None
    encodingString = sys.getdefaultencoding()
    if encodingString != 'ascii':
        charset = PY2WEB_codepages.get(encodingString.lower())
        if charset == None:
            charset = encodingString
    return charset

def toUTF8(value):
    """
    Converts all unicode and non-string values to utf-8.
    This assumes string instances are already encoded in utf-8.
    Note that us-ascii is a subset of utf-8.
    """
    if isinstance(value, unicode):
        return value.encode('utf-8')
    return str(value)

def toUnicode(value):
    """
    Converts all strings non-string values to unicode.
    This assumes string instances are encoded in utf-8.
    Note that us-ascii is a subset of utf-8.
    """
    if not isinstance(value, unicode):
        if not isinstance(value, str):
            return unicode(value)
        return unicode(value, 'utf-8')
    return value
    

def getSystemTempDir():
    return tempfile.gettempdir()

def getEnvVar(name, defaultVal=None):
    if os.environ.has_key(name):
        return os.environ[name]
    return defaultVal

class ObjAsDict(UserDict.DictMixin):
    """
    Passing this to eval as the local variables dictionary allows the
    evaluated code to access properties in the wrapped object
    """
    def __init__(self, obj):
        self.obj = obj
        self.specialEntries = {}

    def __getitem__(self, key):
        try:
            return getattr(self.obj, key)
        except AttributeError, e:
            if self.specialEntries.has_key(key):
                return self.specialEntries[key]
            raise KeyError(e.args)
    def __setitem__(self, key, item): setattr(self.obj, key, item)
    def __delitem__(self, key): delattr(self.obj, key)
    def keys(self):
        ret=[]
        for i in list(dir(self.obj)+self.specialEntries.keys()):
            if i=="__doc__" or i=="__module__":
                pass
            elif i not in ret:
                ret.append(i)
        return ret

    def addSpecialEntry(self, key, value):
        self.specialEntries[key] = value

global saxXMLescapeDoubleQuote
saxXMLescapeDoubleQuote = {'"':'&quot;'}

global saxXMLescapesAllQuotes
# IE doesn't support &apos; but it doesn't seem like we should need this escaped at all so I took it out.
saxXMLescapesAllQuotes = {'"':'&quot;', "'":"&#039;"}

global saxXMLunescapes
saxXMLunescapes = {'&quot;':'"', "&#039;":"'"}

def escape(data, extraEscapes=None):
    """Escape ', ", &, <, and > in a string of data.

    Basically, everything that saxutils.escape does (and this calls that, at
    least for now), but with " and ' added as well.

    TODO: make this faster; saxutils.escape() is really slow
    """
    
    global saxXMLescapeDoubleQuote
    if (extraEscapes == None):
        extraEscapes = saxXMLescapeDoubleQuote
    return saxutils.escape(data, extraEscapes)

def unescape(data):
    """Unescape ', ", &, <, and > in a string of data.

    Basically, everything that saxutils.unescape does (and this calls that, at
    least for now), but with " and ' added as well.

    TODO: make this faster; saxutils.unescape() is really slow
    """
    
    global saxXMLunescapes
    return saxutils.unescape(data, saxXMLunescapes)
