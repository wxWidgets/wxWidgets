"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


import wxParameters as wx


class wxLog:
    """"""

    def Flush(self):
        """"""
        pass

    def GetVerbose(self):
        """"""
        pass

    def HasPendingMessages(self):
        """"""
        pass

    def TimeStamp(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxPyLog(wxLog):
    """"""

    def Destroy(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class wxLogChain(wxLog):
    """"""

    def GetOldLog(self):
        """"""
        pass

    def IsPassingMessages(self):
        """"""
        pass

    def PassMessages(self):
        """"""
        pass

    def SetLog(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxLogGui(wxLog):
    """"""

    def __init__(self):
        """"""
        pass


class wxLogNull:
    """"""

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxLogStderr(wxLog):
    """"""

    def __init__(self):
        """"""
        pass


class wxLogTextCtrl(wxLog):
    """"""

    def __init__(self):
        """"""
        pass


class wxLogWindow(wxLog):
    """"""

    def GetFrame(self):
        """"""
        pass

    def GetOldLog(self):
        """"""
        pass

    def IsPassingMessages(self):
        """"""
        pass

    def PassMessages(self):
        """"""
        pass

    def Show(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


