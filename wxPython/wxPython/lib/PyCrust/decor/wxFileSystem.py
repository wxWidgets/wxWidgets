"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from wxBase import wxObject
import wxParameters as wx


class wxFSFile(wxObject):
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


class wxFileSystem(wxObject):
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


class wxCPPFileSystemHandler(wxObject):
    """"""

    def __init__(self):
        """"""
        pass


class wxFileSystemHandler(wxCPPFileSystemHandler):
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


class wxInternetFSHandler(wxCPPFileSystemHandler):
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


class wxMemoryFSHandler(wxCPPFileSystemHandler):
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


class wxZipFSHandler(wxCPPFileSystemHandler):
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

