"""Decorator utility for documentation and shell scripting."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


import inspect

try:
    True
except NameError:
    True = 1==1
    False = 1==0

def decorate(real, decoration):
    """Decorate real module with docstrings from decoration module."""
    realdict = real.__dict__
    for item in decoration.__dict__.values():
        if inspect.isclass(item):
            decorateClass(item, realdict)
        elif inspect.isfunction(item):
            decorateFunction(item, realdict)

def decorateClass(item, realdict):
    classname = item.__name__
    if not classname.startswith('wx'):
        classname = 'wx' + classname
    try:
        wxclass = realdict[classname]
    except:
        print classname
    else:
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

def decorateFunction(item, realdict):
    funcname = item.__name__
    if funcname in realdict.keys():
        func = realdict[funcname]
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
