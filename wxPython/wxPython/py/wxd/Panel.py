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
from Window import Window

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Panel(Window):
    """"""

    def __init__(self, parent, id, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.TAB_TRAVERSAL,
                 name=wx.PyPanelNameStr):
        """"""
        pass

    def Create(self, parent, id, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=wx.TAB_TRAVERSAL,
               name=wx.PyPanelNameStr):
        """"""
        pass

    def InitDialog(self):
        """"""
        pass


class PyPanel(Panel):
    """"""

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass

    def base_AcceptsFocus(self):
        """"""
        pass

    def base_AcceptsFocusFromKeyboard(self):
        """"""
        pass

    def base_AddChild(self):
        """"""
        pass

    def base_DoGetBestSize(self):
        """"""
        pass

    def base_DoGetClientSize(self):
        """"""
        pass

    def base_DoGetPosition(self):
        """"""
        pass

    def base_DoGetSize(self):
        """"""
        pass

    def base_DoGetVirtualSize(self):
        """"""
        pass

    def base_DoMoveWindow(self):
        """"""
        pass

    def base_DoSetClientSize(self):
        """"""
        pass

    def base_DoSetSize(self):
        """"""
        pass

    def base_DoSetVirtualSize(self):
        """"""
        pass

    def base_GetMaxSize(self):
        """"""
        pass

    def base_InitDialog(self):
        """"""
        pass

    def base_RemoveChild(self):
        """"""
        pass

    def base_TransferDataFromWindow(self):
        """"""
        pass

    def base_TransferDataToWindow(self):
        """"""
        pass

    def base_Validate(self):
        """"""
        pass


class ScrolledWindow(Panel):
    """"""

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.HSCROLL|wx.VSCROLL,
                 name=wx.PyPanelNameStr):
        """"""
        pass

    def Create(self, parent, id=-1, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=wx.HSCROLL|wx.VSCROLL,
               name=wx.PyPanelNameStr):
        """"""
        pass

    def AdjustScrollbars(self):
        """"""
        pass

    def CalcScrolledPosition(self, *args):
        """*args can be a point or (x, y) tuple"""
        pass

    def CalcScrolledPosition1(self, pt):
        """"""
        pass

    def CalcScrolledPosition2(self, x, y):
        """"""
        pass

    def CalcUnscrolledPosition(self, *args):
        """*args can be a point or (x, y) tuple"""
        pass

    def CalcUnscrolledPosition1(self, pt):
        """"""
        pass

    def CalcUnscrolledPosition2(self, x, y):
        """"""
        pass

    def EnableScrolling(self, xScrolling, yScrolling):
        """"""
        pass

    def GetScaleX(self):
        """"""
        pass

    def GetScaleY(self):
        """"""
        pass

    def GetScrollPageSize(self, orient):
        """"""
        pass

    def GetScrollPixelsPerUnit(self):
        """"""
        pass

    def GetTargetWindow(self):
        """"""
        pass

    def GetViewStart(self):
        """"""
        pass

    def IsRetained(self):
        """"""
        pass

    def Layout(self):
        """"""
        pass

    def PrepareDC(self, dc):
        """"""
        pass

    def Scroll(self, x, y):
        """"""
        pass

    def SetScale(self, xs, ys):
        """"""
        pass

    def SetScrollPageSize(self, orient, pageSize):
        """"""
        pass

    def SetScrollRate(self, xstep, ystep):
        """Set the x, y scrolling increments."""
        pass

    def SetScrollbars(self, pixelsPerUnitX, pixelsPerUnitY,
                      noUnitsX, noUnitsY, xPos=0, yPos=0, noRefresh=False):
        """"""
        pass

    def SetTargetWindow(self, window):
        """"""
        pass

    def ViewStart(self):
        """"""
        pass

