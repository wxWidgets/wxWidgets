"""Provides a variety of classes to create pseudo keywords and pseudo files."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


class PseudoKeyword:
    """A callable class that calls a method passed as a parameter.

    Good for creating a pseudo keyword in the python runtime
    environment. The keyword is really an object that has a repr()
    that calls itself which calls the method that was passed in the
    init of the object. All this just to avoid having to type in the
    closing parens on a method. So, for example:

        >>> quit = PseudoKeyword(SomeObject.someMethod)
        >>> quit

    SomeObject.someMethod gets executed as if it had been called
    directly and the user didn't have to type the parens, like
    'quit()'. This technique is most applicable for pseudo keywords
    like quit, exit and help.

    If SomeObject.someMethod can take parameters, they can still be
    passed by using the keyword in the traditional way with parens."""

    def __init__(self, method):
        """Create a callable object that executes method when called."""

        if callable(method):
            self.method = method
        else:
            raise ValueError, 'method must be callable'

    def __call__(self, *args, **kwds):
        self.method(*args, **kwds)

    def __repr__(self):
        self()
        return ''


class PseudoFile:

    def __init__(self):
        """Create a file-like object."""
        pass

    def readline(self):
        pass

    def write(self, s):
        pass

    def writelines(self, l):
        map(self.write, l)

    def flush(self):
        pass

    def isatty(self):
        pass


class PseudoFileIn(PseudoFile):

    def __init__(self, readline, readlines=None):
        if callable(readline):
            self.readline = readline
        else:
            raise ValueError, 'readline must be callable'
        if callable(readlines):
            self.readlines = readlines

    def isatty(self):
        return 1
        
        
class PseudoFileOut(PseudoFile):

    def __init__(self, write):
        if callable(write):
            self.write = write
        else:
            raise ValueError, 'write must be callable'

    def isatty(self):
        return 1


class PseudoFileErr(PseudoFile):

    def __init__(self, write):
        if callable(write):
            self.write = write
        else:
            raise ValueError, 'write must be callable'

    def isatty(self):
        return 1
