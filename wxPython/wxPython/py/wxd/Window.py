"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import EvtHandler
import Parameters as wx

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Window(EvtHandler):
    """"""

    def __init__(self, parent, id, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name=wx.PyPanelNameStr):
        """"""
        pass

    def AcceptsFocus(self):
        """"""
        pass

    def AddChild(self, child):
        """"""
        pass

    def CaptureMouse(self):
        """"""
        pass

    def Center(self, direction=wx.BOTH):
        """"""
        pass

    def CenterOnParent(self, direction=wx.BOTH):
        """"""
        pass

    def CenterOnScreen(self, direction=wx.BOTH):
        """"""
        pass

    def Centre(self, direction=wx.BOTH):
        """"""
        pass

    def CentreOnParent(self, direction=wx.BOTH):
        """"""
        pass

    def CentreOnScreen(self, direction=wx.BOTH):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def ClientToScreen(self, pt):
        """"""
        pass

    def ClientToScreenXY(self, x, y):
        """"""
        pass

    def Close(self, force=False):
        """"""
        pass

    def ConvertDialogPointToPixels(self, pt):
        """"""
        pass

    def ConvertDialogSizeToPixels(self, sz):
        """"""
        pass

    def ConvertPixelPointToDialog(self, pt):
        """"""
        pass

    def ConvertPixelSizeToDialog(self, sz):
        """"""
        pass

    def Create(self, parent, id, pos=wx.DefaultPosition,
               size=wx.DefaultSize, style=0, name=wx.PyPanelNameStr):
        """"""
        pass

    def DLG_PNT(self, win, point_or_x, y=None):
        """"""
        pass

    def DLG_SZE(self, win, size_width, height=None):
        """"""
        pass

    def Destroy(self):
        """"""
        pass

    def DestroyChildren(self):
        """"""
        pass

    def DragAcceptFiles(self, accept):
        """Windows only."""
        pass

    def Enable(self, enable):
        """"""
        pass

    def FindWindowById(self, id):
        """"""
        pass

    def FindWindowByName(self, name):
        """"""
        pass

    def Fit(self):
        """"""
        pass

    def FitInside(self):
        """"""
        pass

    def Freeze(self):
        """"""
        pass

    def GetAcceleratorTable(self):
        """"""
        pass

    def GetAdjustedBestSize(self):
        """"""
        pass

    def GetAutoLayout(self):
        """"""
        pass

    def GetBackgroundColour(self):
        """"""
        pass

    def GetBestSize(self):
        """"""
        pass

    def GetBestVirtualSize(self):
        """"""
        pass

    def GetBorder(self):
        """"""
        pass

    def GetCaret(self):
        """"""
        pass

    def GetCharHeight(self):
        """"""
        pass

    def GetCharWidth(self):
        """"""
        pass

    def GetChildren(self):
        """"""
        pass

    def GetClientAreaOrigin(self):
        """"""
        pass

    def GetClientRect(self):
        """"""
        pass

    def GetClientSize(self):
        """"""
        pass

    def GetClientSizeTuple(self):
        """"""
        pass

    def GetConstraints(self):
        """"""
        pass

    def GetContainingSizer(self):
        """"""
        pass

    def GetCursor(self):
        """"""
        pass

    def GetDefaultItem(self):
        """"""
        pass

    def GetDropTarget(self):
        """"""
        pass

    def GetEventHandler(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetForegroundColour(self):
        """"""
        pass

    def GetFullTextExtent(self):
        """"""
        pass

    def GetGrandParent(self):
        """"""
        pass

    def GetHandle(self):
        """"""
        pass

    def GetHelpText(self):
        """"""
        pass

    def GetId(self):
        """"""
        pass

    def GetLabel(self):
        """"""
        pass

    def GetMaxSize(self):
        """"""
        pass

    def GetName(self):
        """"""
        pass

    def GetParent(self):
        """"""
        pass

    def GetPosition(self):
        """"""
        pass

    def GetPositionTuple(self):
        """"""
        pass

    def GetRect(self):
        """"""
        pass

    def GetScrollPos(self, orientation):
        """"""
        pass

    def GetScrollRange(self, orientation):
        """"""
        pass

    def GetScrollThumb(self, orientation):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def GetSizeTuple(self):
        """"""
        pass

    def GetSizer(self):
        """"""
        pass

    def GetTextExtent(self, string):
        """"""
        pass

    def GetTitle(self):
        """"""
        pass

    def GetToolTip(self):
        """"""
        pass

    def GetUpdateRegion(self):
        """"""
        pass

    def GetValidator(self):
        """"""
        pass

    def GetVirtualSize(self):
        """"""
        pass

    def GetVirtualSizeTuple(self):
        """"""
        pass

    def GetWindowStyleFlag(self):
        """"""
        pass

    def HasCapture(self):
        """"""
        pass

    def HasScrollbar(self, orient):
        """"""
        pass

    def Hide(self):
        """"""
        pass

    def HitTest(self, pt):
        """"""
        pass

    def InitDialog(self):
        """"""
        pass

    def IsBeingDeleted(self):
        """"""
        pass

    def IsEnabled(self):
        """"""
        pass

    def IsExposed(self, x, y, w=0, h=0):
        """"""
        pass

    def IsExposedPoint(self, pt):
        """"""
        pass

    def IsExposedRect(self, rect):
        """"""
        pass

    def IsRetained(self):
        """"""
        pass

    def IsShown(self):
        """"""
        pass

    def IsTopLevel(self):
        """"""
        pass

    def Layout(self):
        """"""
        pass

    def LineDown(self):
        """"""
        pass

    def LineUp(self):
        """"""
        pass

    def LoadFromResource(self, parent, resourceName, resourceTable=wx.NULL):
        """Only if USE_WX_RESOURCES."""
        pass

    def Lower(self):
        """"""
        pass

    def MakeModal(self, flag=True):
        """"""
        pass

    def Move(self, point, flags=wx.SIZE_USE_EXISTING):
        """"""
        pass

    def MoveXY(self, x, y, flags=wx.SIZE_USE_EXISTING):
        """"""
        pass

    def OnPaint(self, event):
        """Windows only."""
        pass

    def PageDown(self):
        """"""
        pass

    def PageUp(self):
        """"""
        pass

    def PopEventHandler(self, deleteHandler=False):
        """"""
        pass

    def PopupMenu(self, menu, pos):
        """"""
        pass

    def PopupMenuXY(self, menu, x, y):
        """"""
        pass

    def PushEventHandler(self, handler):
        """"""
        pass

    def Raise(self):
        """"""
        pass

    def Refresh(self, eraseBackground=True, rect=wx.NULL):
        """"""
        pass

    def RefreshRect(self, rect):
        """"""
        pass

    def ReleaseMouse(self):
        """"""
        pass

    def RemoveChild(self, child):
        """"""
        pass

    def RemoveEventHandler(self, handler):
        """"""
        pass

    def Reparent(self, newParent):
        """"""
        pass

    def ScreenToClient(self, pt):
        """"""
        pass

    def ScreenToClientXY(self, x, y):
        """"""
        pass

    def ScrollLines(self, lines):
        """"""
        pass

    def ScrollPages(self, pages):
        """"""
        pass

    def ScrollWindow(self, dx, dy, rect=wx.NULL):
        """"""
        pass

    def SetAcceleratorTable(self, accel):
        """"""
        pass

    def SetAutoLayout(self, autoLayout):
        """"""
        pass

    def SetBackgroundColour(self, colour):
        """"""
        pass

    def SetCaret(self, caret):
        """"""
        pass

    def SetClientSize(self, size):
        """"""
        pass

    def SetClientSizeWH(self, width, height):
        """"""
        pass

    def SetConstraints(self, constraints):
        """"""
        pass

    def SetContainingSizer(self, sizer):
        """"""
        pass

    def SetCursor(self, cursor):
        """"""
        pass

    def SetDefaultItem(self, btn):
        """"""
        pass

    def SetDimensions(self):
        """"""
        pass

    def SetDropTarget(self, target):
        """"""
        pass

    def SetEventHandler(self, handler):
        """"""
        pass

    def SetExtraStyle(self, exStyle):
        """"""
        pass

    def SetFocus(self):
        """"""
        pass

    def SetFocusFromKbd(self):
        """"""
        pass

    def SetFont(self, font):
        """"""
        pass

    def SetForegroundColour(self, colour):
        """"""
        pass

    def SetHelpText(self, helpText):
        """"""
        pass

    def SetHelpTextForId(self, text):
        """"""
        pass

    def SetId(self, id):
        """"""
        pass

    def SetLabel(self, label):
        """"""
        pass

    def SetName(self, name):
        """"""
        pass

    def SetPosition(self, pos, flags=wx.SIZE_USE_EXISTING):
        """"""
        pass

    def SetRect(self, rect, sizeFlags=wx.SIZE_AUTO):
        """"""
        pass

    def SetScrollPos(self, orientation, pos, refresh=True):
        """"""
        pass

    def SetScrollbar(self, orientation, pos, thumbSize, range, refresh=True):
        """"""
        pass

    def SetSize(self, x, y, width, height, sizeFlags=wx.SIZE_AUTO):
        """"""
        pass

    def SetSizeHints(self, minW, minH, maxW=-1, maxH=-1, incW=-1, incH=-1):
        """"""
        pass

    def SetSizer(self, sizer, deleteOld=True):
        """"""
        pass

    def SetSizerAndFit(self, sizer, deleteOld=True):
        """"""
        pass

    def SetTitle(self, title):
        """"""
        pass

    def SetTmpDefaultItem(self, win):
        """"""
        pass

    def SetToolTip(self, tooltip):
        """"""
        pass

    def SetToolTipString(self, tip):
        """"""
        pass

    def SetValidator(self, validator):
        """"""
        pass

    def SetVirtualSize(self, size):
        """"""
        pass

    def SetVirtualSizeHints(self, minW, minH, maxW=-1, maxH=-1):
        """"""
        pass

    def SetVirtualSizeWH(self, x, y):
        """"""
        pass

    def SetWindowStyle(self, style):
        """"""
        pass

    def SetWindowStyleFlag(self, style):
        """"""
        pass

    def Show(self, show=True):
        """"""
        pass

    def Thaw(self):
        """"""
        pass

    def TransferDataFromWindow(self):
        """"""
        pass

    def TransferDataToWindow(self):
        """"""
        pass

    def UnsetConstraints(self, constraints):
        """"""
        pass

    def Update(self):
        """"""
        pass

    def UpdateWindowUI(self):
        """"""
        pass

    def Validate(self):
        """"""
        pass

    def WarpPointer(self, x, y):
        """"""
        pass


class PyWindow(Window):
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


class TopLevelWindow(Window):
    """"""

    def Create(self):
        """"""
        pass

    def GetIcon(self):
        """"""
        pass

    def GetTitle(self):
        """"""
        pass

    def Iconize(self):
        """"""
        pass

    def IsFullScreen(self):
        """"""
        pass

    def IsIconized(self):
        """"""
        pass

    def IsMaximized(self):
        """"""
        pass

    def Maximize(self):
        """"""
        pass

    def Restore(self):
        """"""
        pass

    def SetIcon(self):
        """"""
        pass

    def SetIcons(self):
        """"""
        pass

    def SetTitle(self):
        """"""
        pass

    def ShowFullScreen(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


