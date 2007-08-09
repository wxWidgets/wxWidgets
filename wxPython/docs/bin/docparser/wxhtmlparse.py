import sys, os, string, glob
import re
from docparser.wxclasses import *
import wx


outputdir = "output"

#
# Class REs
#

class_desc_re = """<H2>.*?</H2>(.*?)<B><FONT COLOR="#FF0000">"""
win_styles_re = """<B><FONT COLOR="#FF0000">Window styles</FONT></B><P>(.*?)<B><FONT COLOR="#FF0000">"""
win_styles_extra_re = """<B><FONT COLOR="#FF0000">Extra window styles</FONT></B><P>(.*?)<B><FONT COLOR="#FF0000">"""
win_style_re = """<TR><TD VALIGN=TOP WIDTH=.*?>\s*?<FONT FACE=".*?">\s*?<B>(.*?)</B>\s*?</FONT></TD>\s*?<TD VALIGN=TOP>\s*?<FONT FACE=".*?">(.*?)</FONT></TD></TR>"""
derived_re = """<B><FONT COLOR="#FF0000">Derived from</FONT></B><P>(.*?)<P>"""
derived_class_re = """<A HREF=".*?">(.*?)</A>"""

#
# Method REs
#

# groups - header, description
method_re = "<H3>(.*?)</H3>\s*?<P>(.*?)<HR>"
lastmethod_re = "<H3>(.*?)</H3>\s*?<P>(.*?)\s*?<P>\s*?</FONT>"
headings_re = "<B><FONT COLOR=\"#FF0000\">(.*?)</FONT></B><P>(.*?)"
# groups = param name, param value 
param_re = "<I>(.*?)</I><UL><UL>(.*?)</UL></UL>"
# groups - return type, method name, arguments
proto_re = "<B>(.*?)</B>.*?<B>(.*?)</B>\s*?\((.*?)\)"
# groups - arg type, arg name
args_re = "<B>(.*?)</B>.*?<I>(.*?)</I>"
code_re = "<PRE>(.*?)</PRE>"
link_re = "<A href=\"(.*?)\"><B>(.*?)</B></A><BR>"

#
# wxPython/wxPerl note REs
# 

wx_re = "wx[A-Z]\S+"
wxperl_overload_re = "<B><FONT COLOR=\"#0000C8\">wxPerl note:</FONT></B> In wxPerl there are two methods instead of a single overloaded method:<P>\s*?<UL><UL>(.*?)</UL></UL>"
wxperl_re = "<B><FONT COLOR=\"#0000C8\">wxPerl note:</FONT></B>(.*?)<P>"

wxpython_constructors_re = """<B><FONT COLOR="#0000C8">wxPython note:</FONT></B> Constructors supported by wxPython are:<P>\s*?<UL><UL>(.*?)</UL></UL>"""
wxpython_overload_re = """<TR><TD VALIGN=TOP.*?>\s*?<FONT FACE=".*?">\s*?<B>(.*?)</B>\s*?</FONT></TD>\s*?<TD VALIGN=TOP>\s*?<FONT FACE=".*?">(.*?)</FONT></TD></TR>"""

wxpython_overloads_re = "<B><FONT COLOR=\"#0000C8\">wxPython note:</FONT></B> In place of a single overloaded method name, wxPython\s*?implements the following methods:<P>\s*?<UL><UL>(.*?)</UL></UL>"
wxpython_re = "<B><FONT COLOR=\"#0000C8\">wxPython note:</FONT></B>(.*?)<P>"


# convert wxWhatever to wx.Whatever
def namespacify_wxClasses(contents):
    wx_regex = re.compile(wx_re, re.MULTILINE | re.DOTALL)
    
    result = wx_regex.sub(wxReplaceFunc, contents)
    return result

def wxReplaceFunc(match):
    text = match.group()
    if text.find("wxWidgets") == -1 and text.find("wxPython") == -1 and text.find("wxPerl") == -1:
        text = text.replace("wx", "wx.")
    return text



# Methods to de-C++itize data.
def pythonize_text(contents):
    """
    Remove C++isms that definitely shouldn't be in any text.
    """
    contents = contents.replace("false", "False")
    contents = contents.replace("true", "True")
    contents = contents.replace("non-NULL", "not None")
    contents = contents.replace("NULL", "None")
    contents = contents.replace("const ", "")
    contents = contents.replace("::", ".")
    contents = contents.replace("\r\n", "\n")
    contents = contents.replace("\r", "\n")
    contents = contents.replace("''", "\"")
    return namespacify_wxClasses(contents)

def pythonize_args(contents):
    """
    Remove C++isms from arguments (some of these terms may be used in other
    contexts in actual documentation, so we don't remove them there).
    """
    contents = contents.replace("static", "")
    contents = contents.replace("virtual void", "")
    contents = contents.replace("virtual", "")
    contents = contents.replace("void*", "int")
    contents = contents.replace("void", "")
    
    contents = contents.replace("off_t", "long")
    contents = contents.replace("size_t", "long")
    contents = contents.replace("*", "")
    contents = contents.replace("&amp;", "")
    contents = contents.replace("&", "")
    contents = contents.replace("char", "string") 
    contents = contents.replace("wxChar", "string") 
    contents = contents.replace("wxCoord", "int")
    contents = contents.replace("<A HREF=\"wx_wxstring.html#wxstring\">wxString</A>", "string")
    
    return pythonize_text(contents)
    
def formatMethodProtos(protos):
    """
    Remove C++isms in the method prototypes. 
    """
    for proto in protos:
        proto[0] = pythonize_args(proto[0])
        proto[0] = proto[0].strip()
        
        proto[1] = namespacify_wxClasses(proto[1])
        for arg in proto[2]:
            arg[0] = pythonize_args(arg[0])
            arg[0].strip()
            
            # for arg names, we should be more careful about what we replace
            arg[1] = pythonize_text(arg[1])
            arg[1] = arg[1].replace("*", "")
            arg[1] = arg[1].replace("&", "")
    
    return protos



# functions for getting data from methods 
def getMethodWxPythonOverrides(text, isConstructor=False):
    overloads_re = wxpython_overloads_re
    if isConstructor:
        overloads_re = wxpython_constructors_re
    overload_regex = re.compile(overloads_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = overload_regex.search(text, 0)
    note = ""
    start = -1
    end = -1
    overrides = []
    if match:
        def getWxPythonOverridesFromMatch(match):
            return [namespacify_wxClasses(match.group(1)), pythonize_text(match.group(2))]
            
        start = match.start()
        end = match.end()
        overrides, returntext = findAllMatches(wxpython_overload_re, match.group(1), getWxPythonOverridesFromMatch)
        
    returntext = text
    
    if start != -1 and end != -1:
        #print "note is: " + text[start:end]
        returntext = text.replace(text[start:end], "")
        
    return overrides, returntext

def getMethodWxPythonNote(text):
    python_regex = re.compile(wxpython_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = python_regex.search(text)
    start = -1
    end = -1
    note = ""
    if match:
        start = match.start()
        end = match.end()
        note = match.group(1)
            
    returntext = text
    
    if start != -1 and end != -1:
        #print "note is: " + text[start:end]
        returntext = text.replace(text[start:end], "")
            
    return note, returntext
    
def findAllMatches(re_string, text, handler, start=0):
    """
    findAllMatches finds matches for a given regex, then runs the handler function
    on each match, and returns a list of objects, along with a version of the 
    text with the area matches were found stripped.
    Note the stripping of text is not generally usable yet, it assumes matches
    are in continuous blocks, which is true of the wx docs.
    """
    regex = re.compile(re_string, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = regex.search(text, start)
    results = []
    
    startpoint = -1
    endpoint = -1
    
    if match:
        startpoint = match.start()
    
    while match:
        start = match.end()
        results.append(handler(match))
        endpoint = match.end()
        match = regex.search(text, start)
        
    returntext = text
    if startpoint != -1 and endpoint != -1:
        returntext = text.replace(text[startpoint:endpoint], "")

    return results, returntext

def getMethodParams(text):
    paramstart = text.find("<B><FONT COLOR=\"#FF0000\">Parameters</FONT></B><P>")
    params, returntext = findAllMatches(param_re, text, getMethodParamsFromMatch, paramstart)
    
    return params, returntext
    
def getMethodParamsFromMatch(match):
    return [match.group(1).strip(), pythonize_text(match.group(2)).strip()]

def getPrototypeFromMatch(match):
    return [match.group(1), match.group(2), getProtoArgs(match.group(3))]

def getProtoArgsFromMatch(match):
    return [match.group(1), match.group(2)]



# These methods parse the docs, finding matches and then using the FromMatch
# functions to parse the data. After that, the results are "Pythonized"
# by removing C++isms.
def getMethodProtos(text):
    protos, returntext = findAllMatches(proto_re, text, getPrototypeFromMatch)
    return formatMethodProtos(protos), returntext
    
def getProtoArgs(text):
    args, returntext = findAllMatches(args_re, text, getProtoArgsFromMatch)
    return args
    
def getMethodDesc(text):
    heading_text = "<B><FONT COLOR=\"#FF0000\">"
    return_text = text
    end = text.find(heading_text)
    if end != -1:
        return_text = text[0:end]
        
    return pythonize_text(return_text)
    

def removeWxPerlNotes(text):
    perl_overload_regex = re.compile(wxperl_overload_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    result = perl_overload_regex.sub("", text)
    
    perl_regex = re.compile(wxperl_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    result = perl_regex.sub("", result)
    
    return result
    
def removeCPPCode(text):
    code_regex = re.compile(code_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    
    result = code_regex.sub("", text)
    return result


def getMethod(match, parent):
    name = match.group(1)
    if name.find("::") != -1:
        name = name.split("::")[1]
    name = namespacify_wxClasses(name).strip()
    start = match.end()
    protos, remainder = getMethodProtos(match.group(2))
    
    isConstructor = False
    #print "name: %s, parent name: %s" % (name, parent.name)
    if name == parent.name.replace("wx", "wx."):
        isConstructor = True
    overrides, remainder = getMethodWxPythonOverrides(remainder, isConstructor)
    
    note, remainder = getMethodWxPythonNote(remainder)
    params, remainder = getMethodParams(remainder)
    desc = getMethodDesc(remainder)
    method = wxMethod(name, parent, protos, params, desc)
    method.pythonNote = note
    method.pythonOverrides = overrides
    if len(method.pythonOverrides) > 0:
        print "has overrides!\n\n\n\n"
    return method

def getClassDerivedFrom(text):

    def getDerivedClassesFromMatch(match):
        return namespacify_wxClasses(match.group(1))

    derived_classes = []
    derived_regex = re.compile(derived_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = derived_regex.search(text)
    if match:
        derived_classes, returntext = findAllMatches(derived_class_re, match.group(1), getDerivedClassesFromMatch)
        
    return derived_classes
    
def getClassDescription(text):
    
    def getClassDescriptionFromMatch(match):
        return match.group(1)
    
    desc, returntext = findAllMatches(class_desc_re, text, getClassDescriptionFromMatch)
    
    return pythonize_text(desc[0])
    
def getClassStyles(text, extraStyles=False):
    styles_re = win_styles_re
    if extraStyles:
        styles_re = win_styles_extra_re
    styles_regex = re.compile(styles_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = styles_regex.search(text)
    
    styles = []
    if match:
        def getClassStyleFromMatch(match):
            return [namespacify_wxClasses(match.group(1)), pythonize_text(match.group(2))]
            
        styles, remainder = findAllMatches(win_style_re, match.group(1), getClassStyleFromMatch)
        
    return styles

# Main functions - these drive the process.
def getClassMethods(doc, parent):
    contents = open(doc, "rb").read()
    
    # get rid of some particularly tricky parts before parsing
    contents = contents.replace("<B>const</B>", "")
    contents = removeWxPerlNotes(contents)
    contents = removeCPPCode(contents)
    
    method_regex = re.compile(method_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = method_regex.search(contents)
    start = 0
    methods = {}
    while match:
        start = match.end()
        newmethod = getMethod(match, parent)
        basename = parent.name.replace("wx", "")
        isConstructor = (basename == newmethod.name.replace("wx.", ""))
        if isConstructor or eval("newmethod.name in dir(wx.%s)" % basename):
            print "Adding %s.%s" % (parent.name, newmethod.name)
            methods[newmethod.name] = newmethod
        match = method_regex.search(contents, start)
    
    lastmethod_regex = re.compile(lastmethod_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    match = lastmethod_regex.search(contents, start)
    if match: 
        newmethod = getMethod(match, parent)
        basename = parent.name.replace("wx", "")
        isConstructor = (basename == newmethod.name.replace("wx.", ""))
        if isConstructor or eval("newmethod.name in dir(wx.%s)" % basename):
            print "Adding %s.%s" % (parent.name, newmethod.name)
            methods[newmethod.name] = newmethod
    
    for name in methods:
        if name[0:3] == "Get":
            propname = name[3:]
            basename = parent.name.replace("wx", "")
            if not propname in eval("dir(wx.%s)" % basename):
                parent.props.append(propname)
            else:
                parent.propConflicts.append(parent.name + "." + propname)
    # get rid of the destructor and operator methods
    ignore_methods = ["~" + namespacify_wxClasses(parent.name), "operator ==", 
                        "operator &lt;&lt;", "operator &gt;&gt;", "operator =", 
                        "operator !=", "operator*", "operator++" ]
    for method in ignore_methods:
        if method in methods:
            methods.pop(method)
            
    return methods
        
def getClasses(doc):
    global docspath
    contents = open(doc, "rb").read()
    link_regex = re.compile(link_re, re.MULTILINE | re.DOTALL | re.IGNORECASE)
    start = contents.find("<H2>Alphabetical class reference</H2>")
    result = link_regex.search(contents, start)
    classes = {}
    while result:
        start = result.end()
        name = result.group(2).strip()
        classpage = result.group(1).split("#")[0]
        basename = name.replace("wx", "")
        if basename in dir(wx):
            classfile = os.path.join(os.path.dirname(doc), classpage)
            classtext = open(classfile, "rb").read()
            derivedClasses = getClassDerivedFrom(classtext)
            description = getClassDescription(classtext)
            styles = getClassStyles(classtext)
            extra_styles = getClassStyles(classtext, extraStyles=True)
            classes[name] = wxClass(name, description, derivedClasses, styles, extra_styles)
            classes[name].methods = getClassMethods(classfile, classes[name])
        result = link_regex.search(contents, start)

    return classes
