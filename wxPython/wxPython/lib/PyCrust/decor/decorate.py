"""Decorator utility for documentation and shell scripting.

When you import wx and stc from this module, all of their classes get
decorated with docstrings from our decorator class definitions.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# The classes in these modules will get decorated.
#-------------------------------------------------

from wxPython import wx
from wxPython import stc


# Code for decorating existing classes.
# -------------------------------------

import inspect

def decorate(item, wxdict):
    """Use item to decorate the matching class in wxdict."""
    if inspect.isclass(item):
        decorateClass(item, wxdict)
    elif inspect.isfunction(item):
        decorateFunction(item, wxdict)

def decorateClass(item, wxdict):
    classname = item.__name__
    if classname in wxdict.keys():
        wxclass = wxdict[classname]
## XXX Not sure if we'll need this or not, but it's here just in case.
##        pointername = classname + 'Ptr'
##        if pointer in wxdict:
##            wxclassPtr = wxdict[pointername]
        if item.__doc__:
            wxclass.__doc__ = item.__doc__
        # Get attributes from only the item's local dictionary!
        for attrname, attr in item.__dict__.items():
            # If the attribute has a docstring, and the wx class has a
            # matching attribute.
            if hasattr(attr, '__doc__') and hasattr(wxclass, attrname):
                if inspect.isfunction(attr):
                    # Class methods are functions.
                    doc = getdoc(attr, drop=True)
                    # Is getattr() okay, or do we want to only look in
                    # the wxclass.__dict__ and wxclassPtr.__dict__?
                    wxattr = getattr(wxclass, attrname)
                    # Our class code may be defined incorrectly, and
                    # the wxattr may not actually be a class method,
                    # but that's okay because the following attempt
                    # will simply fail.
                    try:
                        func = wxattr.im_func
                        func.__doc__ = doc
                    except:
                        pass

def decorateFunction(item, wxdict):
    funcname = item.__name__
    if funcname in wxdict.keys():
        func = wxdict[funcname]
        doc = getdoc(item, drop=False)
        try:
            # Built-in functions have a read-only docstring. :-(
            func.__doc__ = doc
        except:
            # print funcname
            pass

def getdoc(attr, drop=False):
    """Return a docstring for attr, which should be a method."""
    doc = ''
    if attr.__doc__:
        doc = inspect.getdoc(attr).strip()
    name = attr.__name__
    # tip is a string with name(argspec), like: "SetLabel(label)"
    tip = ''
    argspec = apply(inspect.formatargspec, inspect.getargspec(attr))
    # The first parameter to a method is a reference to an instance,
    # usually coded as "self", and is usually passed automatically by
    # Python and therefore we want to drop it.
    temp = argspec.split(',')
    if len(temp) == 1:  # No other arguments.
        argspec = '()'
    elif drop:  # Drop the first argument.
        argspec = '(' + ','.join(temp[1:]).lstrip()
    else:
        argspec = ','.join(temp).lstrip()
    tip = name + argspec
    firstline = doc.split('\n')[0].lstrip()
    if tip != firstline:
        doc = '%s\n\n%s' % (tip, doc)
    return doc


# For each decorator class, apply docstrings to the matching class.
#---------------------------------------------------------------------
# The order in which docstrings are applied could be important,
# depending on how we handle multiple inheritance.  We may want to
# work from parent to child.  We may also have to be clever about how
# we apply the docstrings in the case where our class definitions are
# not an exact match with the wxPython class definitions.  For
# example, if we define a method in a child class that wxPython does
# not, our docstring for the child method may end up on the parent or
# grandparent method.  If the parent or grandparent is not just an
# abstract class, or if the method is shared by more than one
# subclass, none of which override it, the docstring would be
# misleading.


import wxDecor
[decorate(item, wx.__dict__) for item in wxDecor.__dict__.values()]

import stcDecor
[decorate(item, stc.__dict__) for item in stcDecor.__dict__.values()]


# Test the timing of the list comprehension approach vs. the for loop:

##for item in wxDecor.__dict__.itervalues():
##    decorate(item, wx.__dict__)
##
##for item in stcDecor.__dict__.itervalues():
##    decorate(item, stc.__dict__)
