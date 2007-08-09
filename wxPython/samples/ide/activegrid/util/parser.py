#----------------------------------------------------------------------------
# Name:         parser.py
# Purpose:      parsing utilities
#
# Author:       Jeff Norton
#
# Created:      8/9/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import re
from activegrid.util.lang import *
ifDefPy()
import string
import array
endIfDef()

XPATH_ROOT_VAR = '__rootObj__'
GETOBJECTPARTNAMES  =   ["primaryRef", "ref", "orderings", "limit"]

class Tokenizer(object):
    
    TOKEN_IDENT = 1
    TOKEN_STRING = 2
    TOKEN_OP = 3
    TOKEN_WS = 4
##    TOKEN_PLACEHOLDER = 5
    
    def __init__(self, text, identStart=None, tokenSep=None, ignoreWhitespace=True):
        """
Turn a string into individual tokens.  Three types of tokens are recognized:
    TOKEN_IDENT:   identifiers (those that start with the identStart pattern)
    TOKEN_STRING:  quoted string
    TOKEN_OP:      everything else
Tokens are separated by white space or the tokenSep pattern.  
Constructor parameters:
    text:  The string to tokenize
    identStart:  A regular expression describing characters which start an identifier
                 The default expression accepts letters, "_", and "/".
    tokenSep:    A regular expression describing the characters which end a token 
                 (in addition to whitespace).  The default expression accepts
                 anything except alpha-numerics, "_", "/", and ":".
Usage:
    Invoke getNextToken (or next) to get the next token.  The instance variables
    token, and tokenVal will be populated with the current token type (TOKEN_IDENT,
    TOKEN_STRING, or TOEKN_OP) and value respectively.  nextToken and nextTokenVal
    will also be available for lookahead.   The next method is similar to
    getNextToken but also returns the token value.  A value of None signals end
    of stream.                                           
        """
        self.ignoreWhitespace=ignoreWhitespace
        ifDefPy()
        if (isinstance(text, array.array)):
            text = text.tostring()
        endIfDef()
        self.text = asString(text)
        self.textIndex = 0
        self.textLen = len(self.text)
        self.token = None
        self.tokenVal = None
        self.nextToken = None
        self.nextTokenVal = None
        if (identStart == None):
            identStart = "[a-zA-Z_/]"
        if (tokenSep == None):
            tokenSep = "[^a-zA-Z0-9_/:]"
        self.identStart = re.compile(identStart)
        self.tokenSep = re.compile(tokenSep)
        self.getNextToken() # Prime the pump
        
    def isEscaped(text, index):
        if ((index > 0) and (text[index-1] == '\\') and ((index < 2) or (text[index-2] != '\\'))):
            return True
        return False
    isEscaped = staticmethod(isEscaped)

    def findClosingQuote(text, index, char):
        index = index + 1
        while True:
            endIndex = text.find(char, index)
            if (endIndex < 1):
                return -1
            if (Tokenizer.isEscaped(text, endIndex)):
                index = endIndex+1
            else:
                break
        return endIndex + 1
    findClosingQuote = staticmethod(findClosingQuote)

    def _findClosing(self, char):
        if (self.textIndex >= self.textLen):
            raise Exception("The text \"%s\" has an unmatched string starting at %d" % (self.text, self.textIndex))
        index = Tokenizer.findClosingQuote(self.text, self.textIndex, char)
        if (index < 0):
            raise Exception("The text \"%s\" has an unmatched string starting at %d" % (self.text, self.textIndex-1))
        return index
        
    def next(self):
        self.getNextToken()
        if (self.token == None):
            raise StopIteration()
        return self.tokenVal

    def getNextToken(self):
        self.token = self.nextToken
        self.tokenVal = self.nextTokenVal
        while (self.textIndex < self.textLen):
            c = self.text[self.textIndex]
            if (c not in string.whitespace):
                if (c == '"' or c == "'" or c == '`'):
                    endIndex = self._findClosing(c)
                    self.nextToken = self.TOKEN_STRING
                    self.nextTokenVal = self.text[self.textIndex:endIndex]
                    self.textIndex = endIndex
                    return
                elif (self.identStart.search(c)):
                    endMatch = self.tokenSep.search(self.text, self.textIndex+1)
                    if (endMatch):
                        endIndex = endMatch.start()
                    else:
                        endIndex = self.textLen
                    self.nextToken = self.TOKEN_IDENT
                    self.nextTokenVal = self.text[self.textIndex:endIndex]
                    self.textIndex = endIndex
                    return
                else:
                    self.nextToken = self.TOKEN_OP
                    endIndex = self.textIndex + 1
                    if (c == '<' or c == '>' or c == '!' or c == '='):
                        if ((endIndex < self.textLen) and (self.text[endIndex] == '=')):
                            endIndex += 1
                    elif ((c == '%') and (endIndex < self.textLen)):
                        c = self.text[endIndex]
                        if (c in ['d', 'i', 'o', 'u', 'x', 'X', 'e', 'E', 'f', 'F', 'g', 'G', 'c', 'r', 's', '%']):
                            endIndex += 1
##                            self.nextToken = self.TOKEN_PLACEHOLDER # Should really be this but no one can handle it yet
                    self.nextTokenVal = self.text[self.textIndex:endIndex]
                    self.textIndex = endIndex
                    return
            elif not self.ignoreWhitespace:
                self.nextToken=self.TOKEN_WS
                self.nextTokenVal=""
                while c in string.whitespace:
                    self.nextTokenVal+=c
                    self.textIndex+=1
                    if self.textIndex==len(self.text):
                        break
                    c=self.text[self.textIndex]
                return
            self.textIndex += 1
        self.nextToken = None
        self.nextTokenVal = None

def isXPathNonVar(var):
    """Returns true iff var is a string ("foo" or 'foo') or a number."""
    if (var.startswith("'") and var.endswith("'")) or \
            (var.startswith('"') and var.endswith('"')):
        return True

    # list from XPathToCode, below
    if var.lower() in ["count", "empty", "true", "false", "null", "and", "or", \
            "like", "not"]:
        return True

    try:
        t=int(var)
        return True
    except TypeError, e:
        pass
    except ValueError, e:
        pass

    return False

def xpathToCode(xpaths, convertBracket=True):
    if ((xpaths == None) or (len(xpaths) < 1)):
        return "True"
    if (not isinstance(xpaths, (list, tuple))):
        xpaths = [xpaths]
    result = []
    for xpath in xpaths:
        t = Tokenizer(xpath, "[a-zA-Z0-9_/:\.]", "[^a-zA-Z0-9_/:\.]", ignoreWhitespace=False)
        expr = []
        lastToken=None
        while t.nextToken != None:
            t.getNextToken()
            if (t.token == Tokenizer.TOKEN_WS):
                expr.append(" ")
            elif (t.token == Tokenizer.TOKEN_OP):
                if (t.tokenVal == "="):
                    expr.append("==")
                elif (t.tokenVal == "[" and convertBracket):
                    expr.append("(")
                elif (t.tokenVal == "]" and convertBracket):
                    expr.append(")")
                else:
                    expr.append(t.tokenVal)
            elif (t.token == Tokenizer.TOKEN_IDENT):
                if (t.tokenVal == "and"):
                    expr.append(" and ")
                elif (t.tokenVal == "or"):
                    expr.append(" or ")
                elif (t.tokenVal == "not"):
                    expr.append(" not ")
                elif (t.tokenVal == "like"):
                    # REVIEW stoens@activegrid.com 02-Nov-05 --
                    # This is very limited support for like:
                    # typically like queries look like this: "foo like 'blah%'".
                    # So translate this into "foo.startswith(blah)".
                    # We should use a regular expression to support '%'s in
                    # arbitrary places in the string. After 1.1.
                    if t.nextToken and t.nextTokenVal.endswith("%'"):
                        t.getNextToken() # throw away the "like" token
                        last = len(expr) - 1
                        expr[last] = "%s.startswith(%s')"\
                            % (expr[last], t.tokenVal[:-2])
                    else:
                        # old behavior
                        expr.append(t.tokenVal)
                    
                elif (t.tokenVal == "count"):
                    expr.append("len")
                elif (t.tokenVal == 'empty'):
                    expr.append('ctx.isEmptyPath')
                elif (t.tokenVal == 'true'):
                    expr.append(_parseConstantFunction(t, 'True'))
                elif (t.tokenVal == 'false'):
                    expr.append(_parseConstantFunction(t, 'False'))
                elif (t.tokenVal == 'null'):
                    expr.append(_parseConstantFunction(t, 'None'))
                elif (-1!=t.tokenVal.find(':')):
                    serviceDef, args=_parseServiceFunction(t)

                    # XXX handle serviceDef, args being None

                    for i in range(len(args)):
                        args[i]=xpathToCode(args[i], False)
                    jargs="[%s]" % (",".join(args))

                    # XXX should be processmodel.DATASERVICE_PROCESS_NAME, not "dataservice"
                    if serviceDef[0]=='dataservice':
                        expr.append("runtimesupport.invokeDataServiceWrapper(%s, %s, ctx, locals())" % \
                                (serviceDef, jargs))
                    else:
                        expr.append("runtimesupport.invokeServiceWrapper(%s, %s, ctx)" % \
                                (serviceDef, jargs))
                else:
                    if (lastToken==')' or lastToken==']'):
                        wasFunc=True
                    else:
                        wasFunc=False
                    if (t.tokenVal.startswith('/')) and not wasFunc:
                        expr.append(XPATH_ROOT_VAR)
                    expr.append(t.tokenVal.replace('/','.'))
                lastToken=t.tokenVal
            else: 
                expr.append(t.tokenVal)
                

        if (len(expr) == 2 and expr[0]==" "):
            expr = "".join(expr)
            result.append(expr)
        elif (len(expr) > 1):
            expr = "".join(expr)
            result.append("(%s)" % expr)
        elif (len(expr) > 0):
            result.append(expr[0])
        
    return " and ".join(result)

def _parseArgs(t):
    args=[]
    argcon=""

    if t.tokenVal!='(':
        return []
    if t.nextTokenVal==')':
        t.getNextToken()
        return []

    depth=1

    while(depth!=0):
        if not t.nextToken:
            raise Exception("parameters list with no closing ) after token: %s" % t.tokenVal)
        t.getNextToken()

        if t.tokenVal=='(':
            depth+=1
        if t.tokenVal==')':
            depth-=1

        if depth==0 or (depth==1 and t.tokenVal==','):
            args.append(argcon)
            argcon=""
        else:
            argcon+=t.tokenVal
    return args

def _parseServiceFunction(t):
    """Parses what appears to be a service function call into serviceDefs and args lists.

    Returns None, None if the serviceFunction appears to be invalid.
    """
    if t.nextTokenVal!='(':
        return t.tokenVal, None

    serviceDef=t.tokenVal.split(':')
    t.getNextToken()
    args=_parseArgs(t)

    return serviceDef, args
    
def _parseConstantFunction(t, outputValue):
    firstVal = t.tokenVal
    if t.nextTokenVal != '(':
        return firstVal
    t.getNextToken()
    if t.nextTokenVal != ')':
        return "%s%s" % (firstVal, '(')
    t.getNextToken()
    return outputValue

def parseDSPredicate(ctx, str, vars, valueList=None):
    from activegrid.util.utillang import evalCode
    from activegrid.util.utillang import ObjAsDict

    if valueList == None:
        valueList = []
    indexVar=0
    oldIndexVar=0
    sourceStr=str
    inlinedPredicate=[]
    qualifications=[]
    while True:
        oldIndexVar = indexVar
        dollarCurlForm = False
        quoted = False
        indexVar = sourceStr.find("bpws:getVariableData", indexVar)
        if indexVar == -1:
            indexVar = sourceStr.find("${", oldIndexVar)
            if indexVar == -1:
                break
            dollarCurlForm = True
        if indexVar > 0 and sourceStr[indexVar-1] in ('"',"'"):
            quoted = True
        if not dollarCurlForm:
            openParen = sourceStr.find("(", indexVar)
            if openParen == -1:
                break
            closeParen = sourceStr.find(")", openParen)
            if closeParen == -1:
                break
        else:
            openParen = indexVar+1
            closeParen = sourceStr.find("}", openParen)
            if closeParen == -1:
                break
        varRef = sourceStr[openParen+1: closeParen]
        if varRef.startswith('"') or varRef.startswith("'"):
            varRef = varRef[1:]
        if varRef.endswith('"') or varRef.endswith("'"):
            varRef = varRef[:-1]
        if isinstance(vars, dict) or isinstance(vars, ObjAsDict):
            varRefCode = xpathToCode(varRef)
            value = evalCode(varRefCode, vars)
        else:
            value = ctx.evalPath(vars, varRef)
        inlinedPredicate.append(sourceStr[oldIndexVar:indexVar])
        if quoted:
            inlinedPredicate.append("%s" % value)
        else:
            inlinedPredicate.append('%s')
            valueList.append(value)
        indexVar = closeParen+1
    inlinedPredicate.append(sourceStr[oldIndexVar:])
    qualifications.append(''.join(inlinedPredicate))
    return qualifications, valueList
