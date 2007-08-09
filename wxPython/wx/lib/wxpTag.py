#----------------------------------------------------------------------
# Name:        wxPython.lib.wxpTag
# Purpose:     A wxHtmlTagHandler that knows how to build and place
#              wxPython widgets onto web pages.
#
# Author:      Robin Dunn
#
# Created:     13-Sept-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/13/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for V2.5 compatability
#

'''
wxPython.lib.wxpTag

This module contains a wxHtmlTagHandler that knows how to build
and place wxPython widgets onto wxHtmlWindow web pages.

You don\'t need to use anything in this module directly, just
importing it will create the tag handler and add it to any
wxHtmlWinParsers created from that time forth.

Tags of the following form are recognised::

    <WXP class="classname" [module="modulename"] [width="num"] [height="num"]>
        <PARAM name="parameterName" value="parameterValue>
        ...
    </WXP>

where modulename is the name of a module (possibly in package
notation) to import and classname is the name of a class in that
module to create an instance of.  If the module tag-attribute is not
given or is an empty string, then wx is used.  The width and height
attributes are expected to be integers and will be passed to the
__init__ method of the class as a wxSize object named size.  However,
if the width attribute ends with the percent (%) symbol then the value
will be used as a percentage of the available width and the
wxHtmlWindow will manage the size.

The name-value pairs in all the nested PARAM tags are packaged up as
strings into a python dictionary and passed to the __init__ method of
the class as keyword arguments.  This means that they are all
accessible from the __init__ method as regular parameters, or you use
the special Python \*\*kw syntax in your __init__ method to get the
dictionary directly.

Some parameter values are special and if they are present then they will
be converted from strings to alternate datatypes.  They are:

    id           If the value of id can be converted to an integer, it will
                 be.  Otherwise it is assumed to be the name of an integer
                 variable in the module.

    colours      Any value of the form "#123ABC" will automatically be
                 converted to a wxColour object.

    Py Types     Any value begining with "(", "[" or "{" are expected to
                 be a Python tuple, list, or dictionary and eval()
                 will be used to convert them to that type.  If the
                 eval() fails then the original string value will be
                 preserved.

    wx Types     Any value begining with "wx" is expected to be an attempt
                 to create a wxPython object, such as a wxSize, etc.
                 The eval() will be used to try and construct the
                 object and if it fails then the original string value
                 will be used instead.

An example::

    <wxp module="wx" class="Button">
        <param name="label" value="Click here">
        <param name="id" value="ID_OK">
    </wxp>

Both the begining and ending WXP tags are required.

In the future support will be added for another tag that can be
embedded between the two begining and ending WXP tags and will
facilitate calling methods of the widget to help initialize it.
Additionally, support may be added to fetch the module from a web
server as is done with java applets.

'''
#----------------------------------------------------------------------

import  types

import  wx
import  wx.html


#----------------------------------------------------------------------

WXPTAG   = 'WXP'
PARAMTAG = 'PARAM'

#----------------------------------------------------------------------

class wxpTagHandler(wx.html.HtmlWinTagHandler):
    def __init__(self):
        wx.html.HtmlWinTagHandler.__init__(self)
        self.ctx = None

    def GetSupportedTags(self):
        return WXPTAG+','+PARAMTAG


    def HandleTag(self, tag):
        name = tag.GetName()
        if name == WXPTAG:
            return self.HandleWxpTag(tag)
        elif name == PARAMTAG:
            return self.HandleParamTag(tag)
        else:
            raise ValueError, 'unknown tag: ' + name


    def HandleWxpTag(self, tag):
        # create a new context object
        self.ctx = _Context()

        # find and import the module
        modName = ''
        if tag.HasParam('MODULE'):
            modName = tag.GetParam('MODULE')
        if modName:
            self.ctx.classMod = _my_import(modName)
        else:
            self.ctx.classMod = wx

        # find and verify the class
        if not tag.HasParam('CLASS'):
            raise AttributeError, "WXP tag requires a CLASS attribute"

        className = tag.GetParam('CLASS')
        self.ctx.classObj = getattr(self.ctx.classMod, className)
        if type(self.ctx.classObj) not in [ types.ClassType, types.TypeType]:
            raise TypeError, "WXP tag attribute CLASS must name a class"

        # now look for width and height
        width = -1
        height = -1
        if tag.HasParam('WIDTH'):
            width = tag.GetParam('WIDTH')
            if width[-1] == '%':
                self.ctx.floatWidth = int(width[:-1], 0)
                width = self.ctx.floatWidth
            else:
                width = int(width)
        if tag.HasParam('HEIGHT'):
            height = int(tag.GetParam('HEIGHT'))
        self.ctx.kwargs['size'] = wx.Size(width, height)

        # parse up to the closing tag, and gather any nested Param tags.
        self.ParseInner(tag)

        # create the object
        parent = self.GetParser().GetWindowInterface().GetHTMLWindow()
        if parent:
            obj = self.ctx.classObj(parent, **self.ctx.kwargs)
            obj.Show(True)

            # add it to the HtmlWindow
            self.GetParser().GetContainer().InsertCell(
                wx.html.HtmlWidgetCell(obj, self.ctx.floatWidth))
            self.ctx = None
        return True


    def HandleParamTag(self, tag):
        if not tag.HasParam('NAME'):
            return False

        name = tag.GetParam('NAME')
        value = ""
        if tag.HasParam('VALUE'):
            value = tag.GetParam('VALUE')

        # check for a param named 'id'
        if name == 'id':
            theID = -1
            try:
                theID = int(value)
            except ValueError:
                theID = getattr(self.ctx.classMod, value)
            value = theID


        # check for something that should be evaluated
        elif value and value[0] in '[{(' or value[:2] == 'wx':
            saveVal = value
            try:
                value = eval(value, self.ctx.classMod.__dict__)
            except:
                value = saveVal

        # convert to wx.Colour
        elif value and value[0] == '#':
            try:
                red   = int('0x'+value[1:3], 16)
                green = int('0x'+value[3:5], 16)
                blue  = int('0x'+value[5:], 16)
                value = wx.Color(red, green, blue)
            except:
                pass

        if self.ctx:
          self.ctx.kwargs[str(name)] = value
        return False


#----------------------------------------------------------------------
# just a place to hold some values
class _Context:
    def __init__(self):
        self.kwargs = {}
        self.width = -1
        self.height = -1
        self.classMod = None
        self.classObj = None
        self.floatWidth = 0


#----------------------------------------------------------------------
# Function to assist with importing packages
def _my_import(name):
    mod = __import__(name)
    components = name.split('.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod


#----------------------------------------------------------------------
# Function to parse a param string (of the form 'item=value item2="value etc"'
# and creates a dictionary
def _param2dict(param):
    i = 0; j = 0; s = len(param); d = {}
    while 1:
        while i<s and param[i] == " " : i = i+1
        if i>=s: break
        j = i
        while j<s and param[j] != "=": j=j+1
        if j+1>=s:
            break
        word = param[i:j]
        i=j+1
        if (param[i] == '"'):
            j=i+1
            while j<s and param[j] != '"' : j=j+1
            if j == s: break
            val = param[i+1:j]
        elif (param[i] != " "):
            j=i+1
            while j<s and param[j] != " " : j=j+1
            val = param[i:j]
        else:
            val = ""
        i=j+1
        d[word] = val
    return d

#----------------------------------------------------------------------



wx.html.HtmlWinParser_AddTagHandler(wxpTagHandler)
