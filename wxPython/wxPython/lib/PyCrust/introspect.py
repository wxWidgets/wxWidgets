"""Provides a variety of introspective-type support functions for things
like call tips and command auto completion."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "August 8, 2001"
__version__ = "$Revision$"[11:-2]

import inspect
import string

def getAutoCompleteList(command='', locals=None):
    """Return list of auto-completion options for command.
    
    The list of options will be based on the locals namespace."""

    # Get the proper chunk of code from the command.
    root = getRoot(command, terminator='.')
    try:
        object = eval(root, locals)
        attributes = getAttributes(object)
        return attributes
    except:
        return []
    
def getAttributes(object):
    """Return list of unique attributes, including inherited, for an object."""
    attributes = []
    dict = {}
    # Remove duplicates from the attribute list.
    for item in getAllAttributes(object):
        dict[item] = None
    attributes += dict.keys()
    attributes.sort()
    return attributes

def getAllAttributes(object):
    """Return list of all attributes, including inherited, for an object.
    
    Recursively walk through a class and all base classes.
    """
    attributes = []
    try:
        attributes += dir(object)
        if hasattr(object, '__class__'):
            attributes += getAllAttributes(object.__class__)
        if hasattr(object, '__bases__'):
            for base in object.__bases__:
                attributes += getAllAttributes(base)
    finally:
        return attributes

def getCallTip(command='', locals=None):
    """Return call tip text for a command.
    
    The call tip information will be based on the locals namespace."""

    # Get the proper chunk of code from the command.
    root = getRoot(command, terminator='(')
    try:
        object = eval(root, locals)
    except:
        return ''
    dropSelf = 0
    if hasattr(object, '__name__'):  # Make sure this is a useable object.
        # Switch to the object that has the information we need.
        if inspect.ismethod(object):
            # Get the function from the object otherwise inspec.getargspec()
            # complains that the object isn't a Python function.
            object = object.im_func
            dropSelf = 1
        elif inspect.isclass(object):
            # Get the __init__ method for the class.
            try:
                object = object.__init__.im_func
                dropSelf = 1
            except AttributeError:
                for base in object.__bases__:
                    constructor = _find_constructor(base)
                    if constructor is not None:
                        object = constructor
                        dropSelf = 1
                        break
        name = object.__name__
        if inspect.isbuiltin(object):
            # Builtin functions don't have an argspec that we can get.
            tip1 = ''
        else:
            # tip1 is a string like: "getCallTip(command='', locals=None)"
            argspec = apply(inspect.formatargspec, inspect.getargspec(object))
            if dropSelf:
                # The first parameter to a method is a reference to the
                # instance, usually coded as "self", and is passed
                # automatically by Python and therefore we want to drop it.
                temp = argspec.split(',')
                if len(temp) == 1:  # No other arguments.
                    argspec = '()'
                else:  # Drop the first argument.
                    argspec = '(' + ','.join(temp[1:]).lstrip()
            tip1 = name + argspec
        doc = inspect.getdoc(object)
        if doc:
            # tip2 is the first separated line of the docstring, like:
            # "Return call tip text for a command."
            # tip3 is the rest of the docstring, like:
            # "The call tip information will be based on ... <snip>
            docpieces = doc.split('\n\n')
            tip2 = docpieces[0]
            tip3 = '\n\n'.join(docpieces[1:])
            tip = '%s\n\n%s\n\n%s' % (tip1, tip2, tip3)
        else:
            tip = tip1
        return tip.strip()
    else:
        return ''

def getRoot(command, terminator=None):
    """Return the rightmost root portion of an arbitrary Python command.
    
    The command would normally terminate with a "(" or ".". Anything after 
    the terminator will be dropped, allowing you to get back to the root.
    """
    root = ''
    validChars = "._" + string.uppercase + string.lowercase + string.digits
    # Remove all whitespace from the command.
    command = ''.join(command.split())
    # Deal with the terminator.
    if terminator:
        pieces = command.split(terminator)
        if len(pieces) > 1:
            # Drop the final terminator and anything that follows.
            command = terminator.join(pieces[:-1])
    # Go backward through the command until we hit an "invalid" character.
    i = len(command)
    while i and command[i-1] in validChars:
        i -= 1
    # Grab everything from the "invalid" character to the end.
    root = command[i:]
    return root

