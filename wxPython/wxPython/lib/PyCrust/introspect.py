"""Provides a variety of introspective-type support functions for things
like call tips and command auto completion."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__version__ = "$Revision$"[11:-2]

import inspect
import string

def getAutoCompleteList(command='', locals=None, includeMagic=1, \
                        includeSingle=1, includeDouble=1):
    """Return list of auto-completion options for command.
    
    The list of options will be based on the locals namespace."""

    # Get the proper chunk of code from the command.
    root = getRoot(command, terminator='.')
    try:
        object = eval(root, locals)
        attributes = getAttributeNames(object, includeMagic, \
                                       includeSingle, includeDouble)
        return attributes
    except:
        return []
    
def getAttributeNames(object, includeMagic=1, includeSingle=1, includeDouble=1):
    """Return list of unique attributes, including inherited, for an object."""
    attributes = []
    dict = {}
    if includeMagic:
        try: attributes += object._getAttributeNames()
        except: pass
    # Get all attribute names, removing duplicates from the attribute list.
    for item in getAllAttributeNames(object):
        dict[item] = None
    attributes += dict.keys()
    attributes.sort(lambda x, y: cmp(x.lower(), y.lower()))
    if not includeSingle:
        attributes = filter(lambda item: item[0]!='_' or item[1]=='_', attributes)
    if not includeDouble:
        attributes = filter(lambda item: item[:2]!='__', attributes)
    return attributes

def getAllAttributeNames(object):
    """Return list of all attributes, including inherited, for an object.
    
    Recursively walk through a class and all base classes.
    """
    attributes = []
    # Wake up sleepy objects - a hack for ZODB objects in "ghost" state.
    wakeupcall = dir(object)
    del wakeupcall
    # Get attributes available through the normal convention.
    attributes += dir(object)
    # For a class instance, get the attributes for the class.
    if hasattr(object, '__class__'):
        # Break a circular reference. This happens with extension classes.
        if object.__class__ is object:
            pass
        else:
            attributes += getAllAttributeNames(object.__class__)
    # Also get attributes from any and all parent classes.
    if hasattr(object, '__bases__'):
        for base in object.__bases__:
            attributes += getAllAttributeNames(base)
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
        if inspect.ismethod(object) or hasattr(object, 'im_func'):
            # Get the function from the object otherwise inspect.getargspec()
            # complains that the object isn't a Python function.
            object = object.im_func
            dropSelf = 1
        elif inspect.isclass(object):
            # Get the __init__ method function for the class.
            constructor = getConstructor(object)
            if constructor is not None:
                object = constructor
                dropSelf = 1
        name = object.__name__
        tip1 = ''
        if inspect.isbuiltin(object):
            # Builtin functions don't have an argspec that we can get.
            pass
        elif inspect.isfunction(object):
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

def getConstructor(object):
    """Return constructor for class object, or None if there isn't one."""
    try:
        return object.__init__.im_func
    except AttributeError:
        for base in object.__bases__:
            constructor = getConstructor(base)
            if constructor is not None:
                return constructor
    return None

def getRoot(command, terminator=None):
    """Return the rightmost root portion of an arbitrary Python command.
    
    The command would normally terminate with a "(" or ".". Anything after 
    the terminator will be dropped, allowing you to get back to the root.
    Return only the root portion that can be eval()'d without side effects.
    """
    root = ''
    validChars = "._" + string.uppercase + string.lowercase + string.digits
    if terminator:
        # Drop the final terminator and anything that follows.
        pieces = command.split(terminator)
        if len(pieces) > 1:
            command = terminator.join(pieces[:-1])
    if len(command) == 0:
        root = ''
    elif command in ("''", '""', '""""""', '[]', '()', '{}'):
        # Let empty type delimiter pairs go through.
        root = command
    else:
        # Go backward through the command until we hit an "invalid" character.
        i = len(command)
        while i and command[i-1] in validChars:
            i -= 1
        # Detect situations where we are in the middle of a string.
        # This code catches the simplest case, but needs to catch others.
        if command[i-1] in ("'", '"'):
            # We're in the middle of a string so we aren't dealing with an
            # object and it would be misleading to return anything here.
            root = ''
        else:
            # Grab everything from the "invalid" character to the end.
            root = command[i:]
    return root

     
 
