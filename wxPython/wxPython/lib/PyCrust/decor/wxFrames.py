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
from wxWindow import wxTopLevelWindow, wxWindow


class wxFrame(wxTopLevelWindow):
    """"""

    def Command(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def CreateStatusBar(self):
        """"""
        pass

    def CreateToolBar(self):
        """"""
        pass

    def DoGiveHelp(self):
        """"""
        pass

    def GetClientAreaOrigin(self):
        """"""
        pass

    def GetMenuBar(self):
        """"""
        pass

    def GetStatusBar(self):
        """"""
        pass

    def GetStatusBarPane(self):
        """"""
        pass

    def GetToolBar(self):
        """"""
        pass

    def PopStatusText(self):
        """"""
        pass

    def ProcessCommand(self):
        """"""
        pass

    def PushStatusText(self):
        """"""
        pass

    def SendSizeEvent(self):
        """"""
        pass

    def SetMenuBar(self):
        """"""
        pass

    def SetStatusBar(self):
        """"""
        pass

    def SetStatusBarPane(self):
        """"""
        pass

    def SetStatusText(self):
        """"""
        pass

    def SetStatusWidths(self):
        """"""
        pass

    def SetToolBar(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxLayoutAlgorithm(wxObject):
    """"""

    def LayoutFrame(self):
        """"""
        pass

    def LayoutMDIFrame(self):
        """"""
        pass

    def LayoutWindow(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxMDIChildFrame(wxFrame):
    """"""

    def Activate(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Maximize(self):
        """"""
        pass

    def Restore(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxMDIClientWindow(wxWindow):
    """"""

    def Create(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxMDIParentFrame(wxFrame):
    """"""

    def ActivateNext(self):
        """"""
        pass

    def ActivatePrevious(self):
        """"""
        pass

    def ArrangeIcons(self):
        """"""
        pass

    def Cascade(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def GetActiveChild(self):
        """"""
        pass

    def GetClientWindow(self):
        """"""
        pass

    def GetToolBar(self):
        """"""
        pass

    def Tile(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxMiniFrame(wxFrame):
    """"""

    def Create(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxSplashScreen(wxFrame):
    """"""

    def GetSplashStyle(self):
        """"""
        pass

    def GetSplashWindow(self):
        """"""
        pass

    def GetTimeout(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxSplashScreenWindow(wxWindow):
    """"""

    def GetBitmap(self):
        """"""
        pass

    def SetBitmap(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class wxStatusBar(wxWindow):
    """"""

    def Create(self):
        """"""
        pass

    def GetBorderX(self):
        """"""
        pass

    def GetBorderY(self):
        """"""
        pass

    def GetFieldRect(self):
        """"""
        pass

    def GetFieldsCount(self):
        """"""
        pass

    def GetStatusText(self):
        """"""
        pass

    def PopStatusText(self):
        """"""
        pass

    def PushStatusText(self):
        """"""
        pass

    def SetFieldsCount(self):
        """"""
        pass

    def SetMinHeight(self):
        """"""
        pass

    def SetStatusText(self):
        """"""
        pass

    def SetStatusWidths(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


