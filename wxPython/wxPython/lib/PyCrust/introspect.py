"""Provides a variety of introspective-type support functions for things
like call tips and command auto completion."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import inspect
import string
import types

def getAutoCompleteList(command='', locals=None, includeMagic=1, \
                        includeSingle=1, includeDouble=1):
    """Return list of auto-completion options for command.
    
    The list of options will be based on the locals namespace."""
    attributes = []
    # Get the proper chunk of code from the command.
    root = getRoot(command, terminator='.')
    try:
        if locals is not None:
            object = eval(root, locals)
        else:
            object = eval(root)
    except:
        pass
    else:
        attributes = getAttributeNames(object, includeMagic, \
                                       includeSingle, includeDouble)
    return attributes
    
def getAttributeNames(object, includeMagic=1, includeSingle=1, includeDouble=1):
    """Return list of unique attributes, including inherited, for an object."""
    attributes = []
    dict = {}
    if not hasattrAlwaysReturnsTrue(object):
        # Add some attributes that don't always get picked up.
        # If they don't apply, they'll get filtered out at the end.
        attributes += ['__bases__', '__class__', '__dict__', '__name__', \
                       'func_closure', 'func_code', 'func_defaults', \
                       'func_dict', 'func_doc', 'func_globals', 'func_name']
    if includeMagic:
        try: attributes += object._getAttributeNames()
        except: pass
    # Get all attribute names.
    attrdict = getAllAttributeNames(object)
    for attrlist in attrdict.values():
        attributes += attrlist
    # Remove duplicates from the attribute list.
    for item in attributes:
        dict[item] = None
    attributes = dict.keys()
    attributes.sort(lambda x, y: cmp(x.upper(), y.upper()))
    if not includeSingle:
        attributes = filter(lambda item: item[0]!='_' \
                            or item[1]=='_', attributes)
    if not includeDouble:
        attributes = filter(lambda item: item[:2]!='__', attributes)
    # Make sure we haven't picked up any bogus attributes somehow.
    attributes = [attribute for attribute in attributes \
                  if hasattr(object, attribute)]
    return attributes

def hasattrAlwaysReturnsTrue(object):
    return hasattr(object, 'bogu5_123_aTTri8ute')

def getAllAttributeNames(object):
    """Return mapping of all attributes, including inherited, for an object.
    
    Recursively walk through a class and all base classes.
    """
    attrdict = {}  # (object, technique, count): [list of attributes]
    # !!!
    # !!! Do Not use hasattr() as a test anywhere in this function,
    # !!! because it is unreliable with remote objects - xmlrpc, soap, etc.
    # !!! They always return true for hasattr().
    # !!! 
    key = str(object)
    # Wake up sleepy objects - a hack for ZODB objects in "ghost" state.
    wakeupcall = dir(object)
    del wakeupcall
    # Get attributes available through the normal convention.
    attributes = dir(object)
    attrdict[(key, 'dir', len(attributes))] = attributes
    # Get attributes from the object's dictionary, if it has one.
    try:
        attributes = object.__dict__.keys()
        attributes.sort()
    except:  # Must catch all because object might have __getattr__.
        pass
    else:
        attrdict[(key, '__dict__', len(attributes))] = attributes
    # For a class instance, get the attributes for the class.
    try:
        klass = object.__class__
    except:  # Must catch all because object might have __getattr__.
        pass
    else:
        if klass is object:
            # Break a circular reference. This happens with extension classes.
            pass
        else:
            attrdict.update(getAllAttributeNames(klass))
    # Also get attributes from any and all parent classes.
    try:
        bases = object.__bases__
    except:  # Must catch all because object might have __getattr__.
        pass
    else:
        if isinstance(bases, types.TupleType):
            for base in bases:
                if type(base) is types.TypeType:
                    # Break a circular reference. Happens in Python 2.2.
                    pass
                else:
                    attrdict.update(getAllAttributeNames(base))
    return attrdict

def getCallTip(command='', locals=None):
    """For a command, return a tuple of object name, argspec, tip text.
    
    The call tip information will be based on the locals namespace."""
    calltip = ('', '', '')  # object name, argspec, tip text.
    # Get the proper chunk of code from the command.
    root = getRoot(command, terminator='(')
    try:
        if locals is not None:
            object = eval(root, locals)
        else:
            object = eval(root)
    except:
        return calltip
    name = ''
    object, dropSelf = getBaseObject(object)
    try:
        name = object.__name__
    except AttributeError:
        pass
    tip1 = ''
    argspec = ''
    if inspect.isbuiltin(object):
        # Builtin functions don't have an argspec that we can get.
        pass
    elif inspect.isfunction(object):
        # tip1 is a string like: "getCallTip(command='', locals=None)"
        argspec = apply(inspect.formatargspec, inspect.getargspec(object))
        if dropSelf:
            # The first parameter to a method is a reference to an
            # instance, usually coded as "self", and is usually passed
            # automatically by Python and therefore we want to drop it.
            temp = argspec.split(',')
            if len(temp) == 1:  # No other arguments.
                argspec = '()'
            else:  # Drop the first argument.
                argspec = '(' + ','.join(temp[1:]).lstrip()
        tip1 = name + argspec
    doc = ''
    if callable(object):
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
    calltip = (name, argspec[1:-1], tip.strip())
    return calltip

def getRoot(command, terminator=None):
    """Return the rightmost root portion of an arbitrary Python command.
    
    Return only the root portion that can be eval()'d without side effects.
    The command would normally terminate with a "(" or ".". The terminator
    and anything after the terminator will be dropped."""
    root = ''
    validChars = "._" + string.uppercase + string.lowercase + string.digits
    emptyTypes = ("''", '""', '""""""', "''''''", '[]', '()', '{}')
    validSeparators = string.whitespace + ',+-*/=%<>&|^~:([{'
    # Drop the final terminator and anything that follows.
    command = rtrimTerminus(command, terminator)
    if len(command) == 0:
        root = ''
    elif command in emptyTypes and terminator in ('.', '', None):
        # Let empty type delimiter pairs go through.
        root = command
    else:
        # Go backward through the command until we hit an "invalid" character.
        i = len(command)
        while i and command[i-1] in validChars:
            i -= 1
        # Default to everything from the "invalid" character to the end.
        root = command[i:]
        # Override certain exceptions.
        if i > 0 and command[i-1] in ("'", '"'):
            # Detect situations where we are in the middle of a string.
            # This code catches the simplest case, but needs to catch others.
            root = ''
        elif ((2 <= i < len(command) and command[i] == '.') \
           or (2 <= i <= len(command) and terminator in ('.', '', None))) \
        and command[i-2:i] in emptyTypes:
            # Allow empty types to get through.
            # Don't confuse an empty tupple with an argumentless callable.
            if i == 2 or (i >= 3 and command[i-3] in validSeparators):
                root = command[i-2:]
    return root

def rtrimTerminus(command, terminator=None):
    """Return command minus the final terminator and anything that follows."""
    if terminator:
        pieces = command.split(terminator)
        if len(pieces) > 1:
            command = terminator.join(pieces[:-1])
    return command

def getBaseObject(object):
    """Return base object and dropSelf indicator for an object."""
    if inspect.isbuiltin(object):
        # Builtin functions don't have an argspec that we can get.
        dropSelf = 0
    elif inspect.ismethod(object):
        # Get the function from the object otherwise inspect.getargspec()
        # complains that the object isn't a Python function.
        try:
            if object.im_self is None:
                # This is an unbound method so we do not drop self from the
                # argspec, since an instance must be passed as the first arg.
                dropSelf = 0
            else:
                dropSelf = 1
            object = object.im_func
        except AttributeError:
            dropSelf = 0
    elif inspect.isclass(object):
        # Get the __init__ method function for the class.
        constructor = getConstructor(object)
        if constructor is not None:
            object = constructor
            dropSelf = 1
        else:
            dropSelf = 0
    elif callable(object):
        # Get the __call__ method instead.
        try:
            object = object.__call__.im_func
            dropSelf = 1
        except AttributeError:
            dropSelf = 0
    else:
        dropSelf = 0
    return object, dropSelf

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

     
 
