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


class ConfigBase:
    """"""

    def DeleteAll(self):
        """"""
        pass

    def DeleteEntry(self):
        """"""
        pass

    def DeleteGroup(self):
        """"""
        pass

    def Exists(self):
        """"""
        pass

    def ExpandEnvVars(self):
        """"""
        pass

    def Flush(self):
        """"""
        pass

    def GetAppName(self):
        """"""
        pass

    def GetEntryType(self):
        """"""
        pass

    def GetFirstEntry(self):
        """"""
        pass

    def GetFirstGroup(self):
        """"""
        pass

    def GetNextEntry(self):
        """"""
        pass

    def GetNextGroup(self):
        """"""
        pass

    def GetNumberOfEntries(self):
        """"""
        pass

    def GetNumberOfGroups(self):
        """"""
        pass

    def GetPath(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetVendorName(self):
        """"""
        pass

    def HasEntry(self):
        """"""
        pass

    def HasGroup(self):
        """"""
        pass

    def IsExpandingEnvVars(self):
        """"""
        pass

    def IsRecordingDefaults(self):
        """"""
        pass

    def Read(self):
        """"""
        pass

    def ReadBool(self):
        """"""
        pass

    def ReadFloat(self):
        """"""
        pass

    def ReadInt(self):
        """"""
        pass

    def RenameEntry(self):
        """"""
        pass

    def RenameGroup(self):
        """"""
        pass

    def SetAppName(self):
        """"""
        pass

    def SetExpandEnvVars(self):
        """"""
        pass

    def SetPath(self):
        """"""
        pass

    def SetRecordDefaults(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetVendorName(self):
        """"""
        pass

    def Write(self):
        """"""
        pass

    def WriteBool(self):
        """"""
        pass

    def WriteFloat(self):
        """"""
        pass

    def WriteInt(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Config(ConfigBase):
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class FileConfig(ConfigBase):
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass
