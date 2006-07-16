#----------------------------------------------------------------------------
# Name:         strutils.py
# Purpose:      String Utilities
#
# Author:       Morgan Hua
#
# Created:      11/3/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------


def caseInsensitiveCompare(s1, s2):
    """ Method used by sort() to sort values in case insensitive order """
    s1L = s1.lower()
    s2L = s2.lower()
    if s1L == s2L:
        return 0
    elif s1L < s2L:
        return -1
    else:
        return 1

def multiSplit(stringList, tokenList=[" "]):
    """Splits strings in stringList by tokens, returns list of string."""
    if not stringList: return []
    if isinstance(tokenList, basestring):
        tokenList = [tokenList]
    if isinstance(stringList, basestring):
        stringList = [stringList]
    rtnList = stringList
    for token in tokenList:
        rtnList = rtnList[:]
        for string in rtnList:
            if string.find(token) > -1:
                rtnList.remove(string)
                names = string.split(token)
                for name in names:
                    name = name.strip()
                    if name:
                        rtnList.append(name)
    return rtnList

QUOTES = ("\"", "'")

def _findArgStart(argStr):
    i = -1
    for c in argStr:
        i += 1
        if (c == " "):
            continue
        elif (c == ","):
            continue
        return i
    return None

def _findArgEnd(argStr):
    quotedArg = True
    argEndChar = argStr[0]
    if (not argEndChar in QUOTES):
        argEndChar = ","
        quotedArg = False
    i = -1
    firstChar = True
    for c in argStr:
        i+= 1
        if (firstChar):
            firstChar = False
            if (quotedArg):
                continue
        if (c == argEndChar):
            if (quotedArg):
                return min(i+1, len(argStr))
            else:
                return i
    return i

def parseArgs(argStr, stripQuotes=False):
    """
    Given a str representation of method arguments, returns list arguments (as
    strings).
    
    Input: "('[a,b]', 'c', 1)" -> Output: ["'[a,b]'", "'c'", "1"].

    If stripQuotes, removes quotes from quoted arg.
    """
    if (argStr.startswith("(")):
        argStr = argStr[1:]
        if (argStr.endswith(")")):
            argStr = argStr[:-1]
        else:
            raise AssertionError("Expected argStr to end with ')'")

    rtn = []
    argsStr = argStr.strip()
    while (True):
        startIndex = _findArgStart(argStr)
        if (startIndex == None):
            break
        argStr = argStr[startIndex:]
        endIndex = _findArgEnd(argStr)
        if (endIndex == len(argStr) - 1):
            rtn.append(argStr.strip())
            break        
        t = argStr[:endIndex].strip()
        if (stripQuotes and t[0] in QUOTES and t[-1] in QUOTES):
            t = t[1:-1]
        rtn.append(t)
        argStr = argStr[endIndex:]
    return rtn
