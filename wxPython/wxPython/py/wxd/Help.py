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
from Window import Window


class PopupWindow(Window):
    """"""

    def Create(self):
        """"""
        pass

    def Position(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PopupTransientWindow(PopupWindow):
    """"""

    def Dismiss(self):
        """"""
        pass

    def Popup(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class TipProvider:
    """"""

    def GetCurrentTip(self):
        """"""
        pass

    def GetTip(self):
        """"""
        pass

    def PreprocessTip(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PyTipProvider(TipProvider):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class TipWindow(PopupTransientWindow):
    """"""

    def Close(self):
        """"""
        pass

    def SetBoundingRect(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ToolTip(Object):
    """"""

    def GetTip(self):
        """"""
        pass

    def GetWindow(self):
        """"""
        pass

    def SetTip(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


