"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx


class FSFile(Object):
    """"""

    def GetAnchor(self):
        """"""
        pass

    def GetLocation(self):
        """"""
        pass

    def GetMimeType(self):
        """"""
        pass

    def GetModificationTime(self):
        """"""
        pass

    def GetStream(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FileSystem(Object):
    """"""

    def ChangePathTo(self):
        """"""
        pass

    def FindFirst(self):
        """"""
        pass

    def FindNext(self):
        """"""
        pass

    def GetPath(self):
        """"""
        pass

    def OpenFile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class CPPFileSystemHandler(Object):
    """"""

    def __init__(self):
        """"""
        pass


class FileSystemHandler(CPPFileSystemHandler):
    """"""

    def CanOpen(self):
        """"""
        pass

    def FindFirst(self):
        """"""
        pass

    def FindNext(self):
        """"""
        pass

    def GetAnchor(self):
        """"""
        pass

    def GetLeftLocation(self):
        """"""
        pass

    def GetMimeTypeFromExt(self):
        """"""
        pass

    def GetProtocol(self):
        """"""
        pass

    def GetRightLocation(self):
        """"""
        pass

    def OpenFile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class InternetFSHandler(CPPFileSystemHandler):
    """"""

    def CanOpen(self):
        """"""
        pass

    def OpenFile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class MemoryFSHandler(CPPFileSystemHandler):
    """"""

    def CanOpen(self):
        """"""
        pass

    def FindFirst(self):
        """"""
        pass

    def FindNext(self):
        """"""
        pass

    def OpenFile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ZipFSHandler(CPPFileSystemHandler):
    """"""

    def CanOpen(self):
        """"""
        pass

    def FindFirst(self):
        """"""
        pass

    def FindNext(self):
        """"""
        pass

    def OpenFile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

