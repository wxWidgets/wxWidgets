"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


import Parameters as wx


class FileType:
    """"""

    def GetAllCommands(self):
        """"""
        pass

    def GetDescription(self):
        """"""
        pass

    def GetExtensions(self):
        """"""
        pass

    def GetIcon(self):
        """"""
        pass

    def GetIconInfo(self):
        """"""
        pass

    def GetMimeType(self):
        """"""
        pass

    def GetMimeTypes(self):
        """"""
        pass

    def GetOpenCommand(self):
        """"""
        pass

    def GetPrintCommand(self):
        """"""
        pass

    def SetCommand(self):
        """"""
        pass

    def SetDefaultIcon(self):
        """"""
        pass

    def Unassociate(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FileTypeInfo:
    """"""

    def GetDescription(self):
        """"""
        pass

    def GetExtensions(self):
        """"""
        pass

    def GetExtensionsCount(self):
        """"""
        pass

    def GetIconFile(self):
        """"""
        pass

    def GetIconIndex(self):
        """"""
        pass

    def GetMimeType(self):
        """"""
        pass

    def GetOpenCommand(self):
        """"""
        pass

    def GetPrintCommand(self):
        """"""
        pass

    def GetShortDesc(self):
        """"""
        pass

    def IsValid(self):
        """"""
        pass

    def SetIcon(self):
        """"""
        pass

    def SetShortDesc(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class MimeTypesManager:
    """"""

    def AddFallback(self):
        """"""
        pass

    def Associate(self):
        """"""
        pass

    def ClearData(self):
        """"""
        pass

    def EnumAllFileTypes(self):
        """"""
        pass

    def GetFileTypeFromExtension(self):
        """"""
        pass

    def GetFileTypeFromMimeType(self):
        """"""
        pass

    def Initialize(self):
        """"""
        pass

    def ReadMailcap(self):
        """"""
        pass

    def ReadMimeTypes(self):
        """"""
        pass

    def Unassociate(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


