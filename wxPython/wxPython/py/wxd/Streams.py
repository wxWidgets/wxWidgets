"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


class InputStream:
    """"""

    def CanRead(self):
        """"""
        pass

    def Eof(self):
        """"""
        pass

    def GetC(self):
        """"""
        pass

    def LastRead(self):
        """"""
        pass

    def Peek(self):
        """"""
        pass

    def SeekI(self):
        """"""
        pass

    def TellI(self):
        """"""
        pass

    def Ungetch(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def close(self):
        """"""
        pass

    def eof(self):
        """"""
        pass

    def flush(self):
        """"""
        pass

    def read(self):
        """"""
        pass

    def readline(self):
        """"""
        pass

    def readlines(self):
        """"""
        pass

    def seek(self):
        """"""
        pass

    def tell(self):
        """"""
        pass


class OutputStream:
    """"""

    def __init__(self):
        """"""
        pass

    def write(self):
        """"""
        pass
