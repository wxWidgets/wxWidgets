#
# htmlhelper.py 
# 
# A few helper functions for putting wxPython widgets in html pages
#
# Harm van der Heijden, 11 aug 1999.

import wx
import string
import htmlc

# Function to parse a param string (of the form 'item=value item2="value etc"'
# and creates a dictionary
def _param2dict(param):
    i = 0; j = 0; s = len(param); d = {}
    d['param_str'] = param
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
	d[string.lower(word)] = val
    return d

# This function gets called by the <python> tag handler.
# Arguments are the parent (wxHtmlWindow) SWIG pointer (in python, a string)
# and a string containing the parameters.
# The return value must be the SWIG pointer of the created widget (the 'this'
# attribute in python). The widget must be derived from a wxWindow or one
# of its descendants.
def _WidgetStarter(parentptr, param):
    # create a python instance of the parent
    parent = wx.wxWindowPtr(parentptr)
    # try to find the widget class in the htmlwinc (=htmlwidget) module
    dict = _param2dict(param)
    classname = dict['class']
    obj = htmlc.__dict__[classname]
    # now create the class with arguments parent, dictionary
    cls = apply(obj, (parent, dict))
    # return the class instance's pointer
    return cls.this

htmlc.WidgetStarter = _WidgetStarter
