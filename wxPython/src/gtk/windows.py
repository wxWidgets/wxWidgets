# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _windows

import core
wx = core 
#---------------------------------------------------------------------------

class Panel(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPanel instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxTAB_TRAVERSAL|wxNO_BORDER, 
            wxString name=PanelNameStr) -> Panel
        """
        newobj = _windows.new_Panel(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxTAB_TRAVERSAL|wxNO_BORDER, 
            wxString name=PanelNameStr) -> bool
        """
        return _windows.Panel_Create(*args, **kwargs)

    def InitDialog(*args, **kwargs):
        """InitDialog()"""
        return _windows.Panel_InitDialog(*args, **kwargs)


class PanelPtr(Panel):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Panel
_windows.Panel_swigregister(PanelPtr)

def PrePanel(*args, **kwargs):
    """PrePanel() -> Panel"""
    val = _windows.new_PrePanel(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ScrolledWindow(Panel):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxHSCROLL|wxVSCROLL, 
            wxString name=PanelNameStr) -> ScrolledWindow
        """
        newobj = _windows.new_ScrolledWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxHSCROLL|wxVSCROLL, 
            wxString name=PanelNameStr) -> bool
        """
        return _windows.ScrolledWindow_Create(*args, **kwargs)

    def SetScrollbars(*args, **kwargs):
        """
        SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY, int noUnitsX, 
            int noUnitsY, int xPos=0, int yPos=0, bool noRefresh=False)
        """
        return _windows.ScrolledWindow_SetScrollbars(*args, **kwargs)

    def Scroll(*args, **kwargs):
        """Scroll(int x, int y)"""
        return _windows.ScrolledWindow_Scroll(*args, **kwargs)

    def GetScrollPageSize(*args, **kwargs):
        """GetScrollPageSize(int orient) -> int"""
        return _windows.ScrolledWindow_GetScrollPageSize(*args, **kwargs)

    def SetScrollPageSize(*args, **kwargs):
        """SetScrollPageSize(int orient, int pageSize)"""
        return _windows.ScrolledWindow_SetScrollPageSize(*args, **kwargs)

    def SetScrollRate(*args, **kwargs):
        """SetScrollRate(int xstep, int ystep)"""
        return _windows.ScrolledWindow_SetScrollRate(*args, **kwargs)

    def GetScrollPixelsPerUnit(*args, **kwargs):
        """
        GetScrollPixelsPerUnit() -> (xUnit, yUnit)

        Get the size of one logical unit in physical units.
        """
        return _windows.ScrolledWindow_GetScrollPixelsPerUnit(*args, **kwargs)

    def EnableScrolling(*args, **kwargs):
        """EnableScrolling(bool x_scrolling, bool y_scrolling)"""
        return _windows.ScrolledWindow_EnableScrolling(*args, **kwargs)

    def GetViewStart(*args, **kwargs):
        """
        GetViewStart() -> (x,y)

        Get the view start
        """
        return _windows.ScrolledWindow_GetViewStart(*args, **kwargs)

    def SetScale(*args, **kwargs):
        """SetScale(double xs, double ys)"""
        return _windows.ScrolledWindow_SetScale(*args, **kwargs)

    def GetScaleX(*args, **kwargs):
        """GetScaleX() -> double"""
        return _windows.ScrolledWindow_GetScaleX(*args, **kwargs)

    def GetScaleY(*args, **kwargs):
        """GetScaleY() -> double"""
        return _windows.ScrolledWindow_GetScaleY(*args, **kwargs)

    def CalcScrolledPosition(*args):
        """
        CalcScrolledPosition(Point pt) -> Point
        CalcScrolledPosition(int x, int y) -> (sx, sy)

        Translate between scrolled and unscrolled coordinates.
        """
        return _windows.ScrolledWindow_CalcScrolledPosition(*args)

    def CalcUnscrolledPosition(*args):
        """
        CalcUnscrolledPosition(Point pt) -> Point
        CalcUnscrolledPosition(int x, int y) -> (ux, uy)

        Translate between scrolled and unscrolled coordinates.
        """
        return _windows.ScrolledWindow_CalcUnscrolledPosition(*args)

    def AdjustScrollbars(*args, **kwargs):
        """AdjustScrollbars()"""
        return _windows.ScrolledWindow_AdjustScrollbars(*args, **kwargs)

    def CalcScrollInc(*args, **kwargs):
        """CalcScrollInc(ScrollWinEvent event) -> int"""
        return _windows.ScrolledWindow_CalcScrollInc(*args, **kwargs)

    def SetTargetWindow(*args, **kwargs):
        """SetTargetWindow(Window target)"""
        return _windows.ScrolledWindow_SetTargetWindow(*args, **kwargs)

    def GetTargetWindow(*args, **kwargs):
        """GetTargetWindow() -> Window"""
        return _windows.ScrolledWindow_GetTargetWindow(*args, **kwargs)


class ScrolledWindowPtr(ScrolledWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrolledWindow
_windows.ScrolledWindow_swigregister(ScrolledWindowPtr)

def PreScrolledWindow(*args, **kwargs):
    """PreScrolledWindow() -> ScrolledWindow"""
    val = _windows.new_PreScrolledWindow(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

FULLSCREEN_NOMENUBAR = _windows.FULLSCREEN_NOMENUBAR
FULLSCREEN_NOTOOLBAR = _windows.FULLSCREEN_NOTOOLBAR
FULLSCREEN_NOSTATUSBAR = _windows.FULLSCREEN_NOSTATUSBAR
FULLSCREEN_NOBORDER = _windows.FULLSCREEN_NOBORDER
FULLSCREEN_NOCAPTION = _windows.FULLSCREEN_NOCAPTION
FULLSCREEN_ALL = _windows.FULLSCREEN_ALL
TOPLEVEL_EX_DIALOG = _windows.TOPLEVEL_EX_DIALOG
class TopLevelWindow(core.Window):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTopLevelWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Maximize(*args, **kwargs):
        """Maximize(bool maximize=True)"""
        return _windows.TopLevelWindow_Maximize(*args, **kwargs)

    def Restore(*args, **kwargs):
        """Restore()"""
        return _windows.TopLevelWindow_Restore(*args, **kwargs)

    def Iconize(*args, **kwargs):
        """Iconize(bool iconize=True)"""
        return _windows.TopLevelWindow_Iconize(*args, **kwargs)

    def IsMaximized(*args, **kwargs):
        """IsMaximized() -> bool"""
        return _windows.TopLevelWindow_IsMaximized(*args, **kwargs)

    def IsIconized(*args, **kwargs):
        """IsIconized() -> bool"""
        return _windows.TopLevelWindow_IsIconized(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon() -> wxIcon"""
        return _windows.TopLevelWindow_GetIcon(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(wxIcon icon)"""
        return _windows.TopLevelWindow_SetIcon(*args, **kwargs)

    def SetIcons(*args, **kwargs):
        """SetIcons(wxIconBundle icons)"""
        return _windows.TopLevelWindow_SetIcons(*args, **kwargs)

    def ShowFullScreen(*args, **kwargs):
        """ShowFullScreen(bool show, long style=FULLSCREEN_ALL) -> bool"""
        return _windows.TopLevelWindow_ShowFullScreen(*args, **kwargs)

    def IsFullScreen(*args, **kwargs):
        """IsFullScreen() -> bool"""
        return _windows.TopLevelWindow_IsFullScreen(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(wxString title)"""
        return _windows.TopLevelWindow_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle() -> wxString"""
        return _windows.TopLevelWindow_GetTitle(*args, **kwargs)

    def SetShape(*args, **kwargs):
        """SetShape(wxRegion region) -> bool"""
        return _windows.TopLevelWindow_SetShape(*args, **kwargs)


class TopLevelWindowPtr(TopLevelWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TopLevelWindow
_windows.TopLevelWindow_swigregister(TopLevelWindowPtr)

#---------------------------------------------------------------------------

class Frame(TopLevelWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_FRAME_STYLE, 
            wxString name=wxPyFrameNameStr) -> Frame
        """
        newobj = _windows.new_Frame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_FRAME_STYLE, 
            wxString name=wxPyFrameNameStr) -> bool
        """
        return _windows.Frame_Create(*args, **kwargs)

    def GetClientAreaOrigin(*args, **kwargs):
        """GetClientAreaOrigin() -> Point"""
        return _windows.Frame_GetClientAreaOrigin(*args, **kwargs)

    def SendSizeEvent(*args, **kwargs):
        """SendSizeEvent()"""
        return _windows.Frame_SendSizeEvent(*args, **kwargs)

    def SetMenuBar(*args, **kwargs):
        """SetMenuBar(MenuBar menubar)"""
        return _windows.Frame_SetMenuBar(*args, **kwargs)

    def GetMenuBar(*args, **kwargs):
        """GetMenuBar() -> MenuBar"""
        return _windows.Frame_GetMenuBar(*args, **kwargs)

    def ProcessCommand(*args, **kwargs):
        """ProcessCommand(int winid) -> bool"""
        return _windows.Frame_ProcessCommand(*args, **kwargs)

    Command = ProcessCommand 
    def CreateStatusBar(*args, **kwargs):
        """
        CreateStatusBar(int number=1, long style=ST_SIZEGRIP, int winid=0, 
            wxString name=wxPyStatusLineNameStr) -> StatusBar
        """
        return _windows.Frame_CreateStatusBar(*args, **kwargs)

    def GetStatusBar(*args, **kwargs):
        """GetStatusBar() -> StatusBar"""
        return _windows.Frame_GetStatusBar(*args, **kwargs)

    def SetStatusBar(*args, **kwargs):
        """SetStatusBar(StatusBar statBar)"""
        return _windows.Frame_SetStatusBar(*args, **kwargs)

    def SetStatusText(*args, **kwargs):
        """SetStatusText(wxString text, int number=0)"""
        return _windows.Frame_SetStatusText(*args, **kwargs)

    def SetStatusWidths(*args, **kwargs):
        """SetStatusWidths(int widths, int widths_field)"""
        return _windows.Frame_SetStatusWidths(*args, **kwargs)

    def PushStatusText(*args, **kwargs):
        """PushStatusText(wxString text, int number=0)"""
        return _windows.Frame_PushStatusText(*args, **kwargs)

    def PopStatusText(*args, **kwargs):
        """PopStatusText(int number=0)"""
        return _windows.Frame_PopStatusText(*args, **kwargs)

    def SetStatusBarPane(*args, **kwargs):
        """SetStatusBarPane(int n)"""
        return _windows.Frame_SetStatusBarPane(*args, **kwargs)

    def GetStatusBarPane(*args, **kwargs):
        """GetStatusBarPane() -> int"""
        return _windows.Frame_GetStatusBarPane(*args, **kwargs)

    def CreateToolBar(*args, **kwargs):
        """CreateToolBar(long style=-1, int winid=-1, wxString name=wxPyToolBarNameStr) -> wxToolBar"""
        return _windows.Frame_CreateToolBar(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar() -> wxToolBar"""
        return _windows.Frame_GetToolBar(*args, **kwargs)

    def SetToolBar(*args, **kwargs):
        """SetToolBar(wxToolBar toolbar)"""
        return _windows.Frame_SetToolBar(*args, **kwargs)

    def DoGiveHelp(*args, **kwargs):
        """DoGiveHelp(wxString text, bool show)"""
        return _windows.Frame_DoGiveHelp(*args, **kwargs)

    def DoMenuUpdates(*args, **kwargs):
        """DoMenuUpdates(Menu menu=None)"""
        return _windows.Frame_DoMenuUpdates(*args, **kwargs)


class FramePtr(Frame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Frame
_windows.Frame_swigregister(FramePtr)

def PreFrame(*args, **kwargs):
    """PreFrame() -> Frame"""
    val = _windows.new_PreFrame(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Dialog(TopLevelWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_DIALOG_STYLE, 
            wxString name=wxPyDialogNameStr) -> Dialog
        """
        newobj = _windows.new_Dialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_DIALOG_STYLE, 
            wxString name=wxPyDialogNameStr) -> bool
        """
        return _windows.Dialog_Create(*args, **kwargs)

    def SetReturnCode(*args, **kwargs):
        """SetReturnCode(int returnCode)"""
        return _windows.Dialog_SetReturnCode(*args, **kwargs)

    def GetReturnCode(*args, **kwargs):
        """GetReturnCode() -> int"""
        return _windows.Dialog_GetReturnCode(*args, **kwargs)

    def CreateTextSizer(*args, **kwargs):
        """CreateTextSizer(wxString message) -> Sizer"""
        return _windows.Dialog_CreateTextSizer(*args, **kwargs)

    def CreateButtonSizer(*args, **kwargs):
        """CreateButtonSizer(long flags) -> Sizer"""
        return _windows.Dialog_CreateButtonSizer(*args, **kwargs)

    def SetModal(*args, **kwargs):
        """SetModal(bool flag)"""
        return _windows.Dialog_SetModal(*args, **kwargs)

    def IsModal(*args, **kwargs):
        """IsModal() -> bool"""
        return _windows.Dialog_IsModal(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.Dialog_ShowModal(*args, **kwargs)

    def EndModal(*args, **kwargs):
        """EndModal(int retCode)"""
        return _windows.Dialog_EndModal(*args, **kwargs)

    def IsModalShowing(*args, **kwargs):
        """IsModalShowing() -> bool"""
        return _windows.Dialog_IsModalShowing(*args, **kwargs)


class DialogPtr(Dialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Dialog
_windows.Dialog_swigregister(DialogPtr)

def PreDialog(*args, **kwargs):
    """PreDialog() -> Dialog"""
    val = _windows.new_PreDialog(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class MiniFrame(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMiniFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_FRAME_STYLE, 
            wxString name=wxPyFrameNameStr) -> MiniFrame
        """
        newobj = _windows.new_MiniFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DEFAULT_FRAME_STYLE, 
            wxString name=wxPyFrameNameStr) -> bool
        """
        return _windows.MiniFrame_Create(*args, **kwargs)


class MiniFramePtr(MiniFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MiniFrame
_windows.MiniFrame_swigregister(MiniFramePtr)

def PreMiniFrame(*args, **kwargs):
    """PreMiniFrame() -> MiniFrame"""
    val = _windows.new_PreMiniFrame(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

SPLASH_CENTRE_ON_PARENT = _windows.SPLASH_CENTRE_ON_PARENT
SPLASH_CENTRE_ON_SCREEN = _windows.SPLASH_CENTRE_ON_SCREEN
SPLASH_NO_CENTRE = _windows.SPLASH_NO_CENTRE
SPLASH_TIMEOUT = _windows.SPLASH_TIMEOUT
SPLASH_NO_TIMEOUT = _windows.SPLASH_NO_TIMEOUT
class SplashScreenWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplashScreenWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(wxBitmap bitmap, Window parent, int id, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=NO_BORDER) -> SplashScreenWindow
        """
        newobj = _windows.new_SplashScreenWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(wxBitmap bitmap)"""
        return _windows.SplashScreenWindow_SetBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap() -> wxBitmap"""
        return _windows.SplashScreenWindow_GetBitmap(*args, **kwargs)


class SplashScreenWindowPtr(SplashScreenWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplashScreenWindow
_windows.SplashScreenWindow_swigregister(SplashScreenWindowPtr)

class SplashScreen(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplashScreen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(wxBitmap bitmap, long splashStyle, int milliseconds, 
            Window parent, int id, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP) -> SplashScreen
        """
        newobj = _windows.new_SplashScreen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetSplashStyle(*args, **kwargs):
        """GetSplashStyle() -> long"""
        return _windows.SplashScreen_GetSplashStyle(*args, **kwargs)

    def GetSplashWindow(*args, **kwargs):
        """GetSplashWindow() -> SplashScreenWindow"""
        return _windows.SplashScreen_GetSplashWindow(*args, **kwargs)

    def GetTimeout(*args, **kwargs):
        """GetTimeout() -> int"""
        return _windows.SplashScreen_GetTimeout(*args, **kwargs)


class SplashScreenPtr(SplashScreen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplashScreen
_windows.SplashScreen_swigregister(SplashScreenPtr)

#---------------------------------------------------------------------------

class StatusBar(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStatusBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, long style=ST_SIZEGRIP, wxString name=wxPyStatusLineNameStr) -> StatusBar"""
        newobj = _windows.new_StatusBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, long style=ST_SIZEGRIP, wxString name=wxPyStatusLineNameStr) -> bool"""
        return _windows.StatusBar_Create(*args, **kwargs)

    def SetFieldsCount(*args, **kwargs):
        """SetFieldsCount(int number=1)"""
        return _windows.StatusBar_SetFieldsCount(*args, **kwargs)

    def GetFieldsCount(*args, **kwargs):
        """GetFieldsCount() -> int"""
        return _windows.StatusBar_GetFieldsCount(*args, **kwargs)

    def SetStatusText(*args, **kwargs):
        """SetStatusText(wxString text, int number=0)"""
        return _windows.StatusBar_SetStatusText(*args, **kwargs)

    def GetStatusText(*args, **kwargs):
        """GetStatusText(int number=0) -> wxString"""
        return _windows.StatusBar_GetStatusText(*args, **kwargs)

    def PushStatusText(*args, **kwargs):
        """PushStatusText(wxString text, int number=0)"""
        return _windows.StatusBar_PushStatusText(*args, **kwargs)

    def PopStatusText(*args, **kwargs):
        """PopStatusText(int number=0)"""
        return _windows.StatusBar_PopStatusText(*args, **kwargs)

    def SetStatusWidths(*args, **kwargs):
        """SetStatusWidths(int widths, int widths_field)"""
        return _windows.StatusBar_SetStatusWidths(*args, **kwargs)

    def GetFieldRect(*args, **kwargs):
        """GetFieldRect(int i) -> Rect"""
        return _windows.StatusBar_GetFieldRect(*args, **kwargs)

    def SetMinHeight(*args, **kwargs):
        """SetMinHeight(int height)"""
        return _windows.StatusBar_SetMinHeight(*args, **kwargs)

    def GetBorderX(*args, **kwargs):
        """GetBorderX() -> int"""
        return _windows.StatusBar_GetBorderX(*args, **kwargs)

    def GetBorderY(*args, **kwargs):
        """GetBorderY() -> int"""
        return _windows.StatusBar_GetBorderY(*args, **kwargs)


class StatusBarPtr(StatusBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StatusBar
_windows.StatusBar_swigregister(StatusBarPtr)

def PreStatusBar(*args, **kwargs):
    """PreStatusBar() -> StatusBar"""
    val = _windows.new_PreStatusBar(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

SP_NOBORDER = _windows.SP_NOBORDER
SP_NOSASH = _windows.SP_NOSASH
SP_PERMIT_UNSPLIT = _windows.SP_PERMIT_UNSPLIT
SP_LIVE_UPDATE = _windows.SP_LIVE_UPDATE
SP_3DSASH = _windows.SP_3DSASH
SP_3DBORDER = _windows.SP_3DBORDER
SP_BORDER = _windows.SP_BORDER
SP_3D = _windows.SP_3D
SPLIT_HORIZONTAL = _windows.SPLIT_HORIZONTAL
SPLIT_VERTICAL = _windows.SPLIT_VERTICAL
SPLIT_DRAG_NONE = _windows.SPLIT_DRAG_NONE
SPLIT_DRAG_DRAGGING = _windows.SPLIT_DRAG_DRAGGING
SPLIT_DRAG_LEFT_DOWN = _windows.SPLIT_DRAG_LEFT_DOWN
class SplitterWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplitterWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point point=DefaultPosition, 
            Size size=DefaultSize, long style=SP_3D, wxString name=wxPySplitterNameStr) -> SplitterWindow
        """
        newobj = _windows.new_SplitterWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point point=DefaultPosition, 
            Size size=DefaultSize, long style=SP_3D, wxString name=wxPySplitterNameStr) -> bool
        """
        return _windows.SplitterWindow_Create(*args, **kwargs)

    def GetWindow1(*args, **kwargs):
        """GetWindow1() -> Window"""
        return _windows.SplitterWindow_GetWindow1(*args, **kwargs)

    def GetWindow2(*args, **kwargs):
        """GetWindow2() -> Window"""
        return _windows.SplitterWindow_GetWindow2(*args, **kwargs)

    def SetSplitMode(*args, **kwargs):
        """SetSplitMode(int mode)"""
        return _windows.SplitterWindow_SetSplitMode(*args, **kwargs)

    def GetSplitMode(*args, **kwargs):
        """GetSplitMode() -> wxSplitMode"""
        return _windows.SplitterWindow_GetSplitMode(*args, **kwargs)

    def Initialize(*args, **kwargs):
        """Initialize(Window window)"""
        return _windows.SplitterWindow_Initialize(*args, **kwargs)

    def SplitVertically(*args, **kwargs):
        """SplitVertically(Window window1, Window window2, int sashPosition=0) -> bool"""
        return _windows.SplitterWindow_SplitVertically(*args, **kwargs)

    def SplitHorizontally(*args, **kwargs):
        """SplitHorizontally(Window window1, Window window2, int sashPosition=0) -> bool"""
        return _windows.SplitterWindow_SplitHorizontally(*args, **kwargs)

    def Unsplit(*args, **kwargs):
        """Unsplit(Window toRemove=None) -> bool"""
        return _windows.SplitterWindow_Unsplit(*args, **kwargs)

    def ReplaceWindow(*args, **kwargs):
        """ReplaceWindow(Window winOld, Window winNew) -> bool"""
        return _windows.SplitterWindow_ReplaceWindow(*args, **kwargs)

    def IsSplit(*args, **kwargs):
        """IsSplit() -> bool"""
        return _windows.SplitterWindow_IsSplit(*args, **kwargs)

    def SetSashSize(*args, **kwargs):
        """SetSashSize(int width)"""
        return _windows.SplitterWindow_SetSashSize(*args, **kwargs)

    def SetBorderSize(*args, **kwargs):
        """SetBorderSize(int width)"""
        return _windows.SplitterWindow_SetBorderSize(*args, **kwargs)

    def GetSashSize(*args, **kwargs):
        """GetSashSize() -> int"""
        return _windows.SplitterWindow_GetSashSize(*args, **kwargs)

    def GetBorderSize(*args, **kwargs):
        """GetBorderSize() -> int"""
        return _windows.SplitterWindow_GetBorderSize(*args, **kwargs)

    def SetSashPosition(*args, **kwargs):
        """SetSashPosition(int position, bool redraw=True)"""
        return _windows.SplitterWindow_SetSashPosition(*args, **kwargs)

    def GetSashPosition(*args, **kwargs):
        """GetSashPosition() -> int"""
        return _windows.SplitterWindow_GetSashPosition(*args, **kwargs)

    def SetMinimumPaneSize(*args, **kwargs):
        """SetMinimumPaneSize(int min)"""
        return _windows.SplitterWindow_SetMinimumPaneSize(*args, **kwargs)

    def GetMinimumPaneSize(*args, **kwargs):
        """GetMinimumPaneSize() -> int"""
        return _windows.SplitterWindow_GetMinimumPaneSize(*args, **kwargs)

    def SashHitTest(*args, **kwargs):
        """SashHitTest(int x, int y, int tolerance=5) -> bool"""
        return _windows.SplitterWindow_SashHitTest(*args, **kwargs)

    def SizeWindows(*args, **kwargs):
        """SizeWindows()"""
        return _windows.SplitterWindow_SizeWindows(*args, **kwargs)

    def SetNeedUpdating(*args, **kwargs):
        """SetNeedUpdating(bool needUpdating)"""
        return _windows.SplitterWindow_SetNeedUpdating(*args, **kwargs)

    def GetNeedUpdating(*args, **kwargs):
        """GetNeedUpdating() -> bool"""
        return _windows.SplitterWindow_GetNeedUpdating(*args, **kwargs)


class SplitterWindowPtr(SplitterWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplitterWindow
_windows.SplitterWindow_swigregister(SplitterWindowPtr)

def PreSplitterWindow(*args, **kwargs):
    """PreSplitterWindow() -> SplitterWindow"""
    val = _windows.new_PreSplitterWindow(*args, **kwargs)
    val.thisown = 1
    return val

class SplitterEvent(core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplitterEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType type=wxEVT_NULL, SplitterWindow splitter=(wxSplitterWindow *) NULL) -> SplitterEvent"""
        newobj = _windows.new_SplitterEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetSashPosition(*args, **kwargs):
        """SetSashPosition(int pos)"""
        return _windows.SplitterEvent_SetSashPosition(*args, **kwargs)

    def GetSashPosition(*args, **kwargs):
        """GetSashPosition() -> int"""
        return _windows.SplitterEvent_GetSashPosition(*args, **kwargs)

    def GetWindowBeingRemoved(*args, **kwargs):
        """GetWindowBeingRemoved() -> Window"""
        return _windows.SplitterEvent_GetWindowBeingRemoved(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX() -> int"""
        return _windows.SplitterEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY() -> int"""
        return _windows.SplitterEvent_GetY(*args, **kwargs)


class SplitterEventPtr(SplitterEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplitterEvent
_windows.SplitterEvent_swigregister(SplitterEventPtr)

wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED = _windows.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING = _windows.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING
wxEVT_COMMAND_SPLITTER_DOUBLECLICKED = _windows.wxEVT_COMMAND_SPLITTER_DOUBLECLICKED
wxEVT_COMMAND_SPLITTER_UNSPLIT = _windows.wxEVT_COMMAND_SPLITTER_UNSPLIT
EVT_SPLITTER_SASH_POS_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, 1 )
EVT_SPLITTER_SASH_POS_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, 1 )
EVT_SPLITTER_DOUBLECLICKED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, 1 )
EVT_SPLITTER_UNSPLIT = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_UNSPLIT, 1 )

#---------------------------------------------------------------------------

SASH_DRAG_NONE = _windows.SASH_DRAG_NONE
SASH_DRAG_DRAGGING = _windows.SASH_DRAG_DRAGGING
SASH_DRAG_LEFT_DOWN = _windows.SASH_DRAG_LEFT_DOWN
SW_NOBORDER = _windows.SW_NOBORDER
SW_BORDER = _windows.SW_BORDER
SW_3DSASH = _windows.SW_3DSASH
SW_3DBORDER = _windows.SW_3DBORDER
SW_3D = _windows.SW_3D
SASH_TOP = _windows.SASH_TOP
SASH_RIGHT = _windows.SASH_RIGHT
SASH_BOTTOM = _windows.SASH_BOTTOM
SASH_LEFT = _windows.SASH_LEFT
SASH_NONE = _windows.SASH_NONE
class SashWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSashWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxSW_3D, 
            wxString name=wxPySashNameStr) -> SashWindow
        """
        newobj = _windows.new_SashWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxSW_3D, 
            wxString name=wxPySashNameStr) -> bool
        """
        return _windows.SashWindow_Create(*args, **kwargs)

    def SetSashVisible(*args, **kwargs):
        """SetSashVisible(wxSashEdgePosition edge, bool sash)"""
        return _windows.SashWindow_SetSashVisible(*args, **kwargs)

    def GetSashVisible(*args, **kwargs):
        """GetSashVisible(wxSashEdgePosition edge) -> bool"""
        return _windows.SashWindow_GetSashVisible(*args, **kwargs)

    def SetSashBorder(*args, **kwargs):
        """SetSashBorder(wxSashEdgePosition edge, bool border)"""
        return _windows.SashWindow_SetSashBorder(*args, **kwargs)

    def HasBorder(*args, **kwargs):
        """HasBorder(wxSashEdgePosition edge) -> bool"""
        return _windows.SashWindow_HasBorder(*args, **kwargs)

    def GetEdgeMargin(*args, **kwargs):
        """GetEdgeMargin(wxSashEdgePosition edge) -> int"""
        return _windows.SashWindow_GetEdgeMargin(*args, **kwargs)

    def SetDefaultBorderSize(*args, **kwargs):
        """SetDefaultBorderSize(int width)"""
        return _windows.SashWindow_SetDefaultBorderSize(*args, **kwargs)

    def GetDefaultBorderSize(*args, **kwargs):
        """GetDefaultBorderSize() -> int"""
        return _windows.SashWindow_GetDefaultBorderSize(*args, **kwargs)

    def SetExtraBorderSize(*args, **kwargs):
        """SetExtraBorderSize(int width)"""
        return _windows.SashWindow_SetExtraBorderSize(*args, **kwargs)

    def GetExtraBorderSize(*args, **kwargs):
        """GetExtraBorderSize() -> int"""
        return _windows.SashWindow_GetExtraBorderSize(*args, **kwargs)

    def SetMinimumSizeX(*args, **kwargs):
        """SetMinimumSizeX(int min)"""
        return _windows.SashWindow_SetMinimumSizeX(*args, **kwargs)

    def SetMinimumSizeY(*args, **kwargs):
        """SetMinimumSizeY(int min)"""
        return _windows.SashWindow_SetMinimumSizeY(*args, **kwargs)

    def GetMinimumSizeX(*args, **kwargs):
        """GetMinimumSizeX() -> int"""
        return _windows.SashWindow_GetMinimumSizeX(*args, **kwargs)

    def GetMinimumSizeY(*args, **kwargs):
        """GetMinimumSizeY() -> int"""
        return _windows.SashWindow_GetMinimumSizeY(*args, **kwargs)

    def SetMaximumSizeX(*args, **kwargs):
        """SetMaximumSizeX(int max)"""
        return _windows.SashWindow_SetMaximumSizeX(*args, **kwargs)

    def SetMaximumSizeY(*args, **kwargs):
        """SetMaximumSizeY(int max)"""
        return _windows.SashWindow_SetMaximumSizeY(*args, **kwargs)

    def GetMaximumSizeX(*args, **kwargs):
        """GetMaximumSizeX() -> int"""
        return _windows.SashWindow_GetMaximumSizeX(*args, **kwargs)

    def GetMaximumSizeY(*args, **kwargs):
        """GetMaximumSizeY() -> int"""
        return _windows.SashWindow_GetMaximumSizeY(*args, **kwargs)

    def SashHitTest(*args, **kwargs):
        """SashHitTest(int x, int y, int tolerance=2) -> wxSashEdgePosition"""
        return _windows.SashWindow_SashHitTest(*args, **kwargs)

    def SizeWindows(*args, **kwargs):
        """SizeWindows()"""
        return _windows.SashWindow_SizeWindows(*args, **kwargs)


class SashWindowPtr(SashWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SashWindow
_windows.SashWindow_swigregister(SashWindowPtr)

def PreSashWindow(*args, **kwargs):
    """PreSashWindow() -> SashWindow"""
    val = _windows.new_PreSashWindow(*args, **kwargs)
    val.thisown = 1
    return val

SASH_STATUS_OK = _windows.SASH_STATUS_OK
SASH_STATUS_OUT_OF_RANGE = _windows.SASH_STATUS_OUT_OF_RANGE
class SashEvent(core.CommandEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSashEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int id=0, wxSashEdgePosition edge=SASH_NONE) -> SashEvent"""
        newobj = _windows.new_SashEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetEdge(*args, **kwargs):
        """SetEdge(wxSashEdgePosition edge)"""
        return _windows.SashEvent_SetEdge(*args, **kwargs)

    def GetEdge(*args, **kwargs):
        """GetEdge() -> wxSashEdgePosition"""
        return _windows.SashEvent_GetEdge(*args, **kwargs)

    def SetDragRect(*args, **kwargs):
        """SetDragRect(Rect rect)"""
        return _windows.SashEvent_SetDragRect(*args, **kwargs)

    def GetDragRect(*args, **kwargs):
        """GetDragRect() -> Rect"""
        return _windows.SashEvent_GetDragRect(*args, **kwargs)

    def SetDragStatus(*args, **kwargs):
        """SetDragStatus(wxSashDragStatus status)"""
        return _windows.SashEvent_SetDragStatus(*args, **kwargs)

    def GetDragStatus(*args, **kwargs):
        """GetDragStatus() -> wxSashDragStatus"""
        return _windows.SashEvent_GetDragStatus(*args, **kwargs)


class SashEventPtr(SashEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SashEvent
_windows.SashEvent_swigregister(SashEventPtr)

wxEVT_SASH_DRAGGED = _windows.wxEVT_SASH_DRAGGED
EVT_SASH_DRAGGED = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 1 )
EVT_SASH_DRAGGED_RANGE = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 2 )

#---------------------------------------------------------------------------

LAYOUT_HORIZONTAL = _windows.LAYOUT_HORIZONTAL
LAYOUT_VERTICAL = _windows.LAYOUT_VERTICAL
LAYOUT_NONE = _windows.LAYOUT_NONE
LAYOUT_TOP = _windows.LAYOUT_TOP
LAYOUT_LEFT = _windows.LAYOUT_LEFT
LAYOUT_RIGHT = _windows.LAYOUT_RIGHT
LAYOUT_BOTTOM = _windows.LAYOUT_BOTTOM
LAYOUT_LENGTH_Y = _windows.LAYOUT_LENGTH_Y
LAYOUT_LENGTH_X = _windows.LAYOUT_LENGTH_X
LAYOUT_MRU_LENGTH = _windows.LAYOUT_MRU_LENGTH
LAYOUT_QUERY = _windows.LAYOUT_QUERY
wxEVT_QUERY_LAYOUT_INFO = _windows.wxEVT_QUERY_LAYOUT_INFO
wxEVT_CALCULATE_LAYOUT = _windows.wxEVT_CALCULATE_LAYOUT
class QueryLayoutInfoEvent(core.Event):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxQueryLayoutInfoEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int id=0) -> QueryLayoutInfoEvent"""
        newobj = _windows.new_QueryLayoutInfoEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetRequestedLength(*args, **kwargs):
        """SetRequestedLength(int length)"""
        return _windows.QueryLayoutInfoEvent_SetRequestedLength(*args, **kwargs)

    def GetRequestedLength(*args, **kwargs):
        """GetRequestedLength() -> int"""
        return _windows.QueryLayoutInfoEvent_GetRequestedLength(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(int flags)"""
        return _windows.QueryLayoutInfoEvent_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags() -> int"""
        return _windows.QueryLayoutInfoEvent_GetFlags(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(Size size)"""
        return _windows.QueryLayoutInfoEvent_SetSize(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> Size"""
        return _windows.QueryLayoutInfoEvent_GetSize(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(wxLayoutOrientation orient)"""
        return _windows.QueryLayoutInfoEvent_SetOrientation(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation() -> wxLayoutOrientation"""
        return _windows.QueryLayoutInfoEvent_GetOrientation(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(wxLayoutAlignment align)"""
        return _windows.QueryLayoutInfoEvent_SetAlignment(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> wxLayoutAlignment"""
        return _windows.QueryLayoutInfoEvent_GetAlignment(*args, **kwargs)


class QueryLayoutInfoEventPtr(QueryLayoutInfoEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = QueryLayoutInfoEvent
_windows.QueryLayoutInfoEvent_swigregister(QueryLayoutInfoEventPtr)

class CalculateLayoutEvent(core.Event):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCalculateLayoutEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int id=0) -> CalculateLayoutEvent"""
        newobj = _windows.new_CalculateLayoutEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetFlags(*args, **kwargs):
        """SetFlags(int flags)"""
        return _windows.CalculateLayoutEvent_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags() -> int"""
        return _windows.CalculateLayoutEvent_GetFlags(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(Rect rect)"""
        return _windows.CalculateLayoutEvent_SetRect(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect() -> Rect"""
        return _windows.CalculateLayoutEvent_GetRect(*args, **kwargs)


class CalculateLayoutEventPtr(CalculateLayoutEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CalculateLayoutEvent
_windows.CalculateLayoutEvent_swigregister(CalculateLayoutEventPtr)

EVT_QUERY_LAYOUT_INFO = wx.PyEventBinder( wxEVT_QUERY_LAYOUT_INFO )
EVT_CALCULATE_LAYOUT = wx.PyEventBinder( wxEVT_CALCULATE_LAYOUT )

class SashLayoutWindow(SashWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSashLayoutWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxSW_3D, 
            wxString name=wxPySashLayoutNameStr) -> SashLayoutWindow
        """
        newobj = _windows.new_SashLayoutWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxCLIP_CHILDREN|wxSW_3D, 
            wxString name=wxPySashLayoutNameStr) -> bool
        """
        return _windows.SashLayoutWindow_Create(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> wxLayoutAlignment"""
        return _windows.SashLayoutWindow_GetAlignment(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation() -> wxLayoutOrientation"""
        return _windows.SashLayoutWindow_GetOrientation(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(wxLayoutAlignment alignment)"""
        return _windows.SashLayoutWindow_SetAlignment(*args, **kwargs)

    def SetDefaultSize(*args, **kwargs):
        """SetDefaultSize(Size size)"""
        return _windows.SashLayoutWindow_SetDefaultSize(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(wxLayoutOrientation orientation)"""
        return _windows.SashLayoutWindow_SetOrientation(*args, **kwargs)


class SashLayoutWindowPtr(SashLayoutWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SashLayoutWindow
_windows.SashLayoutWindow_swigregister(SashLayoutWindowPtr)

def PreSashLayoutWindow(*args, **kwargs):
    """PreSashLayoutWindow() -> SashLayoutWindow"""
    val = _windows.new_PreSashLayoutWindow(*args, **kwargs)
    val.thisown = 1
    return val

class LayoutAlgorithm(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLayoutAlgorithm instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> LayoutAlgorithm"""
        newobj = _windows.new_LayoutAlgorithm(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_LayoutAlgorithm):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def LayoutMDIFrame(*args, **kwargs):
        """LayoutMDIFrame(MDIParentFrame frame, Rect rect=None) -> bool"""
        return _windows.LayoutAlgorithm_LayoutMDIFrame(*args, **kwargs)

    def LayoutFrame(*args, **kwargs):
        """LayoutFrame(Frame frame, Window mainWindow=None) -> bool"""
        return _windows.LayoutAlgorithm_LayoutFrame(*args, **kwargs)

    def LayoutWindow(*args, **kwargs):
        """LayoutWindow(Window parent, Window mainWindow=None) -> bool"""
        return _windows.LayoutAlgorithm_LayoutWindow(*args, **kwargs)


class LayoutAlgorithmPtr(LayoutAlgorithm):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LayoutAlgorithm
_windows.LayoutAlgorithm_swigregister(LayoutAlgorithmPtr)

#---------------------------------------------------------------------------

class PopupWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPopupWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int flags=BORDER_NONE) -> PopupWindow"""
        newobj = _windows.new_PopupWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int flags=BORDER_NONE) -> bool"""
        return _windows.PopupWindow_Create(*args, **kwargs)

    def Position(*args, **kwargs):
        """Position(Point ptOrigin, Size size)"""
        return _windows.PopupWindow_Position(*args, **kwargs)


class PopupWindowPtr(PopupWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PopupWindow
_windows.PopupWindow_swigregister(PopupWindowPtr)

def PrePopupWindow(*args, **kwargs):
    """PrePopupWindow() -> PopupWindow"""
    val = _windows.new_PrePopupWindow(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class PopupTransientWindow(PopupWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPopupTransientWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int style=BORDER_NONE) -> PopupTransientWindow"""
        newobj = _windows.new_PopupTransientWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, PopupTransientWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PopupTransientWindow__setCallbackInfo(*args, **kwargs)

    def Popup(*args, **kwargs):
        """Popup(Window focus=None)"""
        return _windows.PopupTransientWindow_Popup(*args, **kwargs)

    def Dismiss(*args, **kwargs):
        """Dismiss()"""
        return _windows.PopupTransientWindow_Dismiss(*args, **kwargs)


class PopupTransientWindowPtr(PopupTransientWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PopupTransientWindow
_windows.PopupTransientWindow_swigregister(PopupTransientWindowPtr)

def PrePopupTransientWindow(*args, **kwargs):
    """PrePopupTransientWindow() -> PopupTransientWindow"""
    val = _windows.new_PrePopupTransientWindow(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class TipWindow(PopupTransientWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTipWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, wxString text, int maxLength=100, Rect rectBound=None) -> TipWindow"""
        newobj = _windows.new_TipWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetBoundingRect(*args, **kwargs):
        """SetBoundingRect(Rect rectBound)"""
        return _windows.TipWindow_SetBoundingRect(*args, **kwargs)

    def Close(*args, **kwargs):
        """Close()"""
        return _windows.TipWindow_Close(*args, **kwargs)


class TipWindowPtr(TipWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TipWindow
_windows.TipWindow_swigregister(TipWindowPtr)

#---------------------------------------------------------------------------

class VScrolledWindow(Panel):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyVScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=PanelNameStr) -> VScrolledWindow
        """
        newobj = _windows.new_VScrolledWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, VScrolledWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.VScrolledWindow__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=PanelNameStr) -> bool
        """
        return _windows.VScrolledWindow_Create(*args, **kwargs)

    def SetLineCount(*args, **kwargs):
        """SetLineCount(size_t count)"""
        return _windows.VScrolledWindow_SetLineCount(*args, **kwargs)

    def ScrollToLine(*args, **kwargs):
        """ScrollToLine(size_t line) -> bool"""
        return _windows.VScrolledWindow_ScrollToLine(*args, **kwargs)

    def ScrollLines(*args, **kwargs):
        """ScrollLines(int lines) -> bool"""
        return _windows.VScrolledWindow_ScrollLines(*args, **kwargs)

    def ScrollPages(*args, **kwargs):
        """ScrollPages(int pages) -> bool"""
        return _windows.VScrolledWindow_ScrollPages(*args, **kwargs)

    def RefreshLine(*args, **kwargs):
        """RefreshLine(size_t line)"""
        return _windows.VScrolledWindow_RefreshLine(*args, **kwargs)

    def RefreshLines(*args, **kwargs):
        """RefreshLines(size_t from, size_t to)"""
        return _windows.VScrolledWindow_RefreshLines(*args, **kwargs)

    def HitTestXT(*args, **kwargs):
        """HitTestXT(int x, int y) -> int"""
        return _windows.VScrolledWindow_HitTestXT(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point pt) -> int"""
        return _windows.VScrolledWindow_HitTest(*args, **kwargs)

    def RefreshAll(*args, **kwargs):
        """RefreshAll()"""
        return _windows.VScrolledWindow_RefreshAll(*args, **kwargs)

    def GetLineCount(*args, **kwargs):
        """GetLineCount() -> size_t"""
        return _windows.VScrolledWindow_GetLineCount(*args, **kwargs)

    def GetFirstVisibleLine(*args, **kwargs):
        """GetFirstVisibleLine() -> size_t"""
        return _windows.VScrolledWindow_GetFirstVisibleLine(*args, **kwargs)

    def GetLastVisibleLine(*args, **kwargs):
        """GetLastVisibleLine() -> size_t"""
        return _windows.VScrolledWindow_GetLastVisibleLine(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(size_t line) -> bool"""
        return _windows.VScrolledWindow_IsVisible(*args, **kwargs)


class VScrolledWindowPtr(VScrolledWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = VScrolledWindow
_windows.VScrolledWindow_swigregister(VScrolledWindowPtr)

def PreVScrolledWindow(*args, **kwargs):
    """PreVScrolledWindow() -> VScrolledWindow"""
    val = _windows.new_PreVScrolledWindow(*args, **kwargs)
    val.thisown = 1
    return val

class VListBox(VScrolledWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyVListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=wxPyVListBoxNameStr) -> VListBox
        """
        newobj = _windows.new_VListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, VListBox)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.VListBox__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=wxPyVListBoxNameStr) -> bool
        """
        return _windows.VListBox_Create(*args, **kwargs)

    def GetItemCount(*args, **kwargs):
        """GetItemCount() -> size_t"""
        return _windows.VListBox_GetItemCount(*args, **kwargs)

    def HasMultipleSelection(*args, **kwargs):
        """HasMultipleSelection() -> bool"""
        return _windows.VListBox_HasMultipleSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> int"""
        return _windows.VListBox_GetSelection(*args, **kwargs)

    def IsCurrent(*args, **kwargs):
        """IsCurrent(size_t item) -> bool"""
        return _windows.VListBox_IsCurrent(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(size_t item) -> bool"""
        return _windows.VListBox_IsSelected(*args, **kwargs)

    def GetSelectedCount(*args, **kwargs):
        """GetSelectedCount() -> size_t"""
        return _windows.VListBox_GetSelectedCount(*args, **kwargs)

    def GetFirstSelected(*args, **kwargs):
        """GetFirstSelected(unsigned long cookie) -> int"""
        return _windows.VListBox_GetFirstSelected(*args, **kwargs)

    def GetNextSelected(*args, **kwargs):
        """GetNextSelected(unsigned long cookie) -> int"""
        return _windows.VListBox_GetNextSelected(*args, **kwargs)

    def GetMargins(*args, **kwargs):
        """GetMargins() -> Point"""
        return _windows.VListBox_GetMargins(*args, **kwargs)

    def GetSelectionBackground(*args, **kwargs):
        """GetSelectionBackground() -> wxColour"""
        return _windows.VListBox_GetSelectionBackground(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(size_t count)"""
        return _windows.VListBox_SetItemCount(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear()"""
        return _windows.VListBox_Clear(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int selection)"""
        return _windows.VListBox_SetSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(size_t item, bool select=True) -> bool"""
        return _windows.VListBox_Select(*args, **kwargs)

    def SelectRange(*args, **kwargs):
        """SelectRange(size_t from, size_t to) -> bool"""
        return _windows.VListBox_SelectRange(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(size_t item)"""
        return _windows.VListBox_Toggle(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll() -> bool"""
        return _windows.VListBox_SelectAll(*args, **kwargs)

    def DeselectAll(*args, **kwargs):
        """DeselectAll() -> bool"""
        return _windows.VListBox_DeselectAll(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(Point pt)"""
        return _windows.VListBox_SetMargins(*args, **kwargs)

    def SetMarginsXY(*args, **kwargs):
        """SetMarginsXY(int x, int y)"""
        return _windows.VListBox_SetMarginsXY(*args, **kwargs)

    def SetSelectionBackground(*args, **kwargs):
        """SetSelectionBackground(wxColour col)"""
        return _windows.VListBox_SetSelectionBackground(*args, **kwargs)


class VListBoxPtr(VListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = VListBox
_windows.VListBox_swigregister(VListBoxPtr)

def PreVListBox(*args, **kwargs):
    """PreVListBox() -> VListBox"""
    val = _windows.new_PreVListBox(*args, **kwargs)
    val.thisown = 1
    return val

class HtmlListBox(VListBox):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyHtmlListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=wxPyVListBoxNameStr) -> HtmlListBox
        """
        newobj = _windows.new_HtmlListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, HtmlListBox)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.HtmlListBox__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, wxString name=wxPyVListBoxNameStr) -> bool
        """
        return _windows.HtmlListBox_Create(*args, **kwargs)

    def RefreshAll(*args, **kwargs):
        """RefreshAll()"""
        return _windows.HtmlListBox_RefreshAll(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(size_t count)"""
        return _windows.HtmlListBox_SetItemCount(*args, **kwargs)


class HtmlListBoxPtr(HtmlListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HtmlListBox
_windows.HtmlListBox_swigregister(HtmlListBoxPtr)

def PreHtmlListBox(*args, **kwargs):
    """PreHtmlListBox() -> HtmlListBox"""
    val = _windows.new_PreHtmlListBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class TaskBarIcon(core.EvtHandler):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTaskBarIcon instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> TaskBarIcon"""
        newobj = _windows.new_TaskBarIcon(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_TaskBarIcon):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def IsOk(*args, **kwargs):
        """IsOk() -> bool"""
        return _windows.TaskBarIcon_IsOk(*args, **kwargs)

    def IsIconInstalled(*args, **kwargs):
        """IsIconInstalled() -> bool"""
        return _windows.TaskBarIcon_IsIconInstalled(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(wxIcon icon, wxString tooltip=wxPyEmptyString) -> bool"""
        return _windows.TaskBarIcon_SetIcon(*args, **kwargs)

    def RemoveIcon(*args, **kwargs):
        """RemoveIcon() -> bool"""
        return _windows.TaskBarIcon_RemoveIcon(*args, **kwargs)

    def PopupMenu(*args, **kwargs):
        """PopupMenu(Menu menu) -> bool"""
        return _windows.TaskBarIcon_PopupMenu(*args, **kwargs)


class TaskBarIconPtr(TaskBarIcon):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TaskBarIcon
_windows.TaskBarIcon_swigregister(TaskBarIconPtr)

class TaskBarIconEvent(core.Event):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTaskBarIconEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType evtType, TaskBarIcon tbIcon) -> TaskBarIconEvent"""
        newobj = _windows.new_TaskBarIconEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class TaskBarIconEventPtr(TaskBarIconEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TaskBarIconEvent
_windows.TaskBarIconEvent_swigregister(TaskBarIconEventPtr)

wxEVT_TASKBAR_MOVE = _windows.wxEVT_TASKBAR_MOVE
wxEVT_TASKBAR_LEFT_DOWN = _windows.wxEVT_TASKBAR_LEFT_DOWN
wxEVT_TASKBAR_LEFT_UP = _windows.wxEVT_TASKBAR_LEFT_UP
wxEVT_TASKBAR_RIGHT_DOWN = _windows.wxEVT_TASKBAR_RIGHT_DOWN
wxEVT_TASKBAR_RIGHT_UP = _windows.wxEVT_TASKBAR_RIGHT_UP
wxEVT_TASKBAR_LEFT_DCLICK = _windows.wxEVT_TASKBAR_LEFT_DCLICK
wxEVT_TASKBAR_RIGHT_DCLICK = _windows.wxEVT_TASKBAR_RIGHT_DCLICK
EVT_TASKBAR_MOVE = wx.PyEventBinder (         wxEVT_TASKBAR_MOVE )
EVT_TASKBAR_LEFT_DOWN = wx.PyEventBinder (    wxEVT_TASKBAR_LEFT_DOWN )
EVT_TASKBAR_LEFT_UP = wx.PyEventBinder (      wxEVT_TASKBAR_LEFT_UP )
EVT_TASKBAR_RIGHT_DOWN = wx.PyEventBinder (   wxEVT_TASKBAR_RIGHT_DOWN )
EVT_TASKBAR_RIGHT_UP = wx.PyEventBinder (     wxEVT_TASKBAR_RIGHT_UP )
EVT_TASKBAR_LEFT_DCLICK = wx.PyEventBinder (  wxEVT_TASKBAR_LEFT_DCLICK )
EVT_TASKBAR_RIGHT_DCLICK = wx.PyEventBinder ( wxEVT_TASKBAR_RIGHT_DCLICK )

#---------------------------------------------------------------------------

class ColourData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> ColourData"""
        newobj = _windows.new_ColourData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_ColourData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetChooseFull(*args, **kwargs):
        """GetChooseFull() -> bool"""
        return _windows.ColourData_GetChooseFull(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> wxColour"""
        return _windows.ColourData_GetColour(*args, **kwargs)

    def GetCustomColour(*args, **kwargs):
        """GetCustomColour(int i) -> wxColour"""
        return _windows.ColourData_GetCustomColour(*args, **kwargs)

    def SetChooseFull(*args, **kwargs):
        """SetChooseFull(int flag)"""
        return _windows.ColourData_SetChooseFull(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(wxColour colour)"""
        return _windows.ColourData_SetColour(*args, **kwargs)

    def SetCustomColour(*args, **kwargs):
        """SetCustomColour(int i, wxColour colour)"""
        return _windows.ColourData_SetCustomColour(*args, **kwargs)


class ColourDataPtr(ColourData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourData
_windows.ColourData_swigregister(ColourDataPtr)

class ColourDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, ColourData data=None) -> ColourDialog"""
        newobj = _windows.new_ColourDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetColourData(*args, **kwargs):
        """GetColourData() -> ColourData"""
        return _windows.ColourDialog_GetColourData(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.ColourDialog_ShowModal(*args, **kwargs)


class ColourDialogPtr(ColourDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourDialog
_windows.ColourDialog_swigregister(ColourDialogPtr)

class DirDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDirDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message=wxPyDirSelectorPromptStr, 
            wxString defaultPath=wxPyEmptyString, 
            long style=0, Point pos=DefaultPosition, Size size=DefaultSize, 
            wxString name=wxPyDirDialogNameStr) -> DirDialog
        """
        newobj = _windows.new_DirDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetPath(*args, **kwargs):
        """GetPath() -> wxString"""
        return _windows.DirDialog_GetPath(*args, **kwargs)

    def GetMessage(*args, **kwargs):
        """GetMessage() -> wxString"""
        return _windows.DirDialog_GetMessage(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> long"""
        return _windows.DirDialog_GetStyle(*args, **kwargs)

    def SetMessage(*args, **kwargs):
        """SetMessage(wxString message)"""
        return _windows.DirDialog_SetMessage(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(wxString path)"""
        return _windows.DirDialog_SetPath(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.DirDialog_ShowModal(*args, **kwargs)


class DirDialogPtr(DirDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DirDialog
_windows.DirDialog_swigregister(DirDialogPtr)

class FileDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFileDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message=wxPyFileSelectorPromptStr, 
            wxString defaultDir=wxPyEmptyString, 
            wxString defaultFile=wxPyEmptyString, wxString wildcard=wxPyFileSelectorDefaultWildcardStr, 
            long style=0, 
            Point pos=DefaultPosition) -> FileDialog
        """
        newobj = _windows.new_FileDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetMessage(*args, **kwargs):
        """SetMessage(wxString message)"""
        return _windows.FileDialog_SetMessage(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(wxString path)"""
        return _windows.FileDialog_SetPath(*args, **kwargs)

    def SetDirectory(*args, **kwargs):
        """SetDirectory(wxString dir)"""
        return _windows.FileDialog_SetDirectory(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(wxString name)"""
        return _windows.FileDialog_SetFilename(*args, **kwargs)

    def SetWildcard(*args, **kwargs):
        """SetWildcard(wxString wildCard)"""
        return _windows.FileDialog_SetWildcard(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(long style)"""
        return _windows.FileDialog_SetStyle(*args, **kwargs)

    def SetFilterIndex(*args, **kwargs):
        """SetFilterIndex(int filterIndex)"""
        return _windows.FileDialog_SetFilterIndex(*args, **kwargs)

    def GetMessage(*args, **kwargs):
        """GetMessage() -> wxString"""
        return _windows.FileDialog_GetMessage(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath() -> wxString"""
        return _windows.FileDialog_GetPath(*args, **kwargs)

    def GetDirectory(*args, **kwargs):
        """GetDirectory() -> wxString"""
        return _windows.FileDialog_GetDirectory(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename() -> wxString"""
        return _windows.FileDialog_GetFilename(*args, **kwargs)

    def GetWildcard(*args, **kwargs):
        """GetWildcard() -> wxString"""
        return _windows.FileDialog_GetWildcard(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> long"""
        return _windows.FileDialog_GetStyle(*args, **kwargs)

    def GetFilterIndex(*args, **kwargs):
        """GetFilterIndex() -> int"""
        return _windows.FileDialog_GetFilterIndex(*args, **kwargs)

    def GetFilenames(*args, **kwargs):
        """GetFilenames() -> PyObject"""
        return _windows.FileDialog_GetFilenames(*args, **kwargs)

    def GetPaths(*args, **kwargs):
        """GetPaths() -> PyObject"""
        return _windows.FileDialog_GetPaths(*args, **kwargs)


class FileDialogPtr(FileDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FileDialog
_windows.FileDialog_swigregister(FileDialogPtr)

CHOICEDLG_STYLE = _windows.CHOICEDLG_STYLE
class MultiChoiceDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMultiChoiceDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message, wxString caption, 
            int LCOUNT, wxString choices, long style=CHOICEDLG_STYLE, 
            Point pos=DefaultPosition) -> MultiChoiceDialog
        """
        newobj = _windows.new_MultiChoiceDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def SetSelections(*args, **kwargs):
        """SetSelections(wxArrayInt selections)"""
        return _windows.MultiChoiceDialog_SetSelections(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections() -> PyObject"""
        return _windows.MultiChoiceDialog_GetSelections(*args, **kwargs)


class MultiChoiceDialogPtr(MultiChoiceDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MultiChoiceDialog
_windows.MultiChoiceDialog_swigregister(MultiChoiceDialogPtr)

class SingleChoiceDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSingleChoiceDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message, wxString caption, 
            int choices, wxString choices_array, long style=CHOICEDLG_STYLE, 
            Point pos=DefaultPosition) -> SingleChoiceDialog
        """
        newobj = _windows.new_SingleChoiceDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> int"""
        return _windows.SingleChoiceDialog_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection() -> wxString"""
        return _windows.SingleChoiceDialog_GetStringSelection(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int sel)"""
        return _windows.SingleChoiceDialog_SetSelection(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.SingleChoiceDialog_ShowModal(*args, **kwargs)


class SingleChoiceDialogPtr(SingleChoiceDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SingleChoiceDialog
_windows.SingleChoiceDialog_swigregister(SingleChoiceDialogPtr)

class TextEntryDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextEntryDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message, wxString caption=wxPyGetTextFromUserPromptStr, 
            wxString defaultValue=wxPyEmptyString, 
            long style=wxOK|wxCANCEL|wxCENTRE, 
            Point pos=DefaultPosition) -> TextEntryDialog
        """
        newobj = _windows.new_TextEntryDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue() -> wxString"""
        return _windows.TextEntryDialog_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(wxString value)"""
        return _windows.TextEntryDialog_SetValue(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.TextEntryDialog_ShowModal(*args, **kwargs)


class TextEntryDialogPtr(TextEntryDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextEntryDialog
_windows.TextEntryDialog_swigregister(TextEntryDialogPtr)

class FontData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> FontData"""
        newobj = _windows.new_FontData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_FontData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def EnableEffects(*args, **kwargs):
        """EnableEffects(bool enable)"""
        return _windows.FontData_EnableEffects(*args, **kwargs)

    def GetAllowSymbols(*args, **kwargs):
        """GetAllowSymbols() -> bool"""
        return _windows.FontData_GetAllowSymbols(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> wxColour"""
        return _windows.FontData_GetColour(*args, **kwargs)

    def GetChosenFont(*args, **kwargs):
        """GetChosenFont() -> wxFont"""
        return _windows.FontData_GetChosenFont(*args, **kwargs)

    def GetEnableEffects(*args, **kwargs):
        """GetEnableEffects() -> bool"""
        return _windows.FontData_GetEnableEffects(*args, **kwargs)

    def GetInitialFont(*args, **kwargs):
        """GetInitialFont() -> wxFont"""
        return _windows.FontData_GetInitialFont(*args, **kwargs)

    def GetShowHelp(*args, **kwargs):
        """GetShowHelp() -> bool"""
        return _windows.FontData_GetShowHelp(*args, **kwargs)

    def SetAllowSymbols(*args, **kwargs):
        """SetAllowSymbols(bool allowSymbols)"""
        return _windows.FontData_SetAllowSymbols(*args, **kwargs)

    def SetChosenFont(*args, **kwargs):
        """SetChosenFont(wxFont font)"""
        return _windows.FontData_SetChosenFont(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(wxColour colour)"""
        return _windows.FontData_SetColour(*args, **kwargs)

    def SetInitialFont(*args, **kwargs):
        """SetInitialFont(wxFont font)"""
        return _windows.FontData_SetInitialFont(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(int min, int max)"""
        return _windows.FontData_SetRange(*args, **kwargs)

    def SetShowHelp(*args, **kwargs):
        """SetShowHelp(bool showHelp)"""
        return _windows.FontData_SetShowHelp(*args, **kwargs)


class FontDataPtr(FontData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontData
_windows.FontData_swigregister(FontDataPtr)

class FontDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, FontData data) -> FontDialog"""
        newobj = _windows.new_FontDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetFontData(*args, **kwargs):
        """GetFontData() -> FontData"""
        return _windows.FontDialog_GetFontData(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.FontDialog_ShowModal(*args, **kwargs)


class FontDialogPtr(FontDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontDialog
_windows.FontDialog_swigregister(FontDialogPtr)

class MessageDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMessageDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, wxString message, wxString caption=wxPyMessageBoxCaptionStr, 
            long style=wxOK|wxCANCEL|wxCENTRE, 
            Point pos=DefaultPosition) -> MessageDialog
        """
        newobj = _windows.new_MessageDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.MessageDialog_ShowModal(*args, **kwargs)


class MessageDialogPtr(MessageDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MessageDialog
_windows.MessageDialog_swigregister(MessageDialogPtr)

class ProgressDialog(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxProgressDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(wxString title, wxString message, int maximum=100, 
            Window parent=None, int style=wxPD_AUTO_HIDE|wxPD_APP_MODAL) -> ProgressDialog
        """
        newobj = _windows.new_ProgressDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Update(*args, **kwargs):
        """Update(int value, wxString newmsg=wxPyEmptyString) -> bool"""
        return _windows.ProgressDialog_Update(*args, **kwargs)

    def Resume(*args, **kwargs):
        """Resume()"""
        return _windows.ProgressDialog_Resume(*args, **kwargs)


class ProgressDialogPtr(ProgressDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ProgressDialog
_windows.ProgressDialog_swigregister(ProgressDialogPtr)

FR_DOWN = _windows.FR_DOWN
FR_WHOLEWORD = _windows.FR_WHOLEWORD
FR_MATCHCASE = _windows.FR_MATCHCASE
FR_REPLACEDIALOG = _windows.FR_REPLACEDIALOG
FR_NOUPDOWN = _windows.FR_NOUPDOWN
FR_NOMATCHCASE = _windows.FR_NOMATCHCASE
FR_NOWHOLEWORD = _windows.FR_NOWHOLEWORD
wxEVT_COMMAND_FIND = _windows.wxEVT_COMMAND_FIND
wxEVT_COMMAND_FIND_NEXT = _windows.wxEVT_COMMAND_FIND_NEXT
wxEVT_COMMAND_FIND_REPLACE = _windows.wxEVT_COMMAND_FIND_REPLACE
wxEVT_COMMAND_FIND_REPLACE_ALL = _windows.wxEVT_COMMAND_FIND_REPLACE_ALL
wxEVT_COMMAND_FIND_CLOSE = _windows.wxEVT_COMMAND_FIND_CLOSE
EVT_FIND = wx.PyEventBinder( wxEVT_COMMAND_FIND, 1 )
EVT_FIND_NEXT = wx.PyEventBinder( wxEVT_COMMAND_FIND_NEXT, 1 )
EVT_FIND_REPLACE = wx.PyEventBinder( wxEVT_COMMAND_FIND_REPLACE, 1 )
EVT_FIND_REPLACE_ALL = wx.PyEventBinder( wxEVT_COMMAND_FIND_REPLACE_ALL, 1 )
EVT_FIND_CLOSE = wx.PyEventBinder( wxEVT_COMMAND_FIND_CLOSE, 1 )

# For backwards compatibility.  Should they be removed?
EVT_COMMAND_FIND             = EVT_FIND 
EVT_COMMAND_FIND_NEXT        = EVT_FIND_NEXT
EVT_COMMAND_FIND_REPLACE     = EVT_FIND_REPLACE
EVT_COMMAND_FIND_REPLACE_ALL = EVT_FIND_REPLACE_ALL
EVT_COMMAND_FIND_CLOSE       = EVT_FIND_CLOSE        

class FindDialogEvent(core.CommandEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFindDialogEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0) -> FindDialogEvent"""
        newobj = _windows.new_FindDialogEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetFlags(*args, **kwargs):
        """GetFlags() -> int"""
        return _windows.FindDialogEvent_GetFlags(*args, **kwargs)

    def GetFindString(*args, **kwargs):
        """GetFindString() -> wxString"""
        return _windows.FindDialogEvent_GetFindString(*args, **kwargs)

    def GetReplaceString(*args, **kwargs):
        """GetReplaceString() -> wxString"""
        return _windows.FindDialogEvent_GetReplaceString(*args, **kwargs)

    def GetDialog(*args, **kwargs):
        """GetDialog() -> FindReplaceDialog"""
        return _windows.FindDialogEvent_GetDialog(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(int flags)"""
        return _windows.FindDialogEvent_SetFlags(*args, **kwargs)

    def SetFindString(*args, **kwargs):
        """SetFindString(wxString str)"""
        return _windows.FindDialogEvent_SetFindString(*args, **kwargs)

    def SetReplaceString(*args, **kwargs):
        """SetReplaceString(wxString str)"""
        return _windows.FindDialogEvent_SetReplaceString(*args, **kwargs)


class FindDialogEventPtr(FindDialogEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FindDialogEvent
_windows.FindDialogEvent_swigregister(FindDialogEventPtr)

class FindReplaceData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFindReplaceData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int flags=0) -> FindReplaceData"""
        newobj = _windows.new_FindReplaceData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_FindReplaceData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetFindString(*args, **kwargs):
        """GetFindString() -> wxString"""
        return _windows.FindReplaceData_GetFindString(*args, **kwargs)

    def GetReplaceString(*args, **kwargs):
        """GetReplaceString() -> wxString"""
        return _windows.FindReplaceData_GetReplaceString(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags() -> int"""
        return _windows.FindReplaceData_GetFlags(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(int flags)"""
        return _windows.FindReplaceData_SetFlags(*args, **kwargs)

    def SetFindString(*args, **kwargs):
        """SetFindString(wxString str)"""
        return _windows.FindReplaceData_SetFindString(*args, **kwargs)

    def SetReplaceString(*args, **kwargs):
        """SetReplaceString(wxString str)"""
        return _windows.FindReplaceData_SetReplaceString(*args, **kwargs)


class FindReplaceDataPtr(FindReplaceData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FindReplaceData
_windows.FindReplaceData_swigregister(FindReplaceDataPtr)

class FindReplaceDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFindReplaceDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, FindReplaceData data, wxString title, 
            int style=0) -> FindReplaceDialog
        """
        newobj = _windows.new_FindReplaceDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, FindReplaceData data, wxString title, 
            int style=0) -> bool
        """
        return _windows.FindReplaceDialog_Create(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData() -> FindReplaceData"""
        return _windows.FindReplaceDialog_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(FindReplaceData data)"""
        return _windows.FindReplaceDialog_SetData(*args, **kwargs)


class FindReplaceDialogPtr(FindReplaceDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FindReplaceDialog
_windows.FindReplaceDialog_swigregister(FindReplaceDialogPtr)

def PreFindReplaceDialog(*args, **kwargs):
    """PreFindReplaceDialog() -> FindReplaceDialog"""
    val = _windows.new_PreFindReplaceDialog(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

IDM_WINDOWTILE = _windows.IDM_WINDOWTILE
IDM_WINDOWTILEHOR = _windows.IDM_WINDOWTILEHOR
IDM_WINDOWCASCADE = _windows.IDM_WINDOWCASCADE
IDM_WINDOWICONS = _windows.IDM_WINDOWICONS
IDM_WINDOWNEXT = _windows.IDM_WINDOWNEXT
IDM_WINDOWTILEVERT = _windows.IDM_WINDOWTILEVERT
FIRST_MDI_CHILD = _windows.FIRST_MDI_CHILD
LAST_MDI_CHILD = _windows.LAST_MDI_CHILD
class MDIParentFrame(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMDIParentFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL, 
            wxString name=wxPyFrameNameStr) -> MDIParentFrame
        """
        newobj = _windows.new_MDIParentFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL, 
            wxString name=wxPyFrameNameStr) -> bool
        """
        return _windows.MDIParentFrame_Create(*args, **kwargs)

    def ActivateNext(*args, **kwargs):
        """ActivateNext()"""
        return _windows.MDIParentFrame_ActivateNext(*args, **kwargs)

    def ActivatePrevious(*args, **kwargs):
        """ActivatePrevious()"""
        return _windows.MDIParentFrame_ActivatePrevious(*args, **kwargs)

    def ArrangeIcons(*args, **kwargs):
        """ArrangeIcons()"""
        return _windows.MDIParentFrame_ArrangeIcons(*args, **kwargs)

    def Cascade(*args, **kwargs):
        """Cascade()"""
        return _windows.MDIParentFrame_Cascade(*args, **kwargs)

    def GetActiveChild(*args, **kwargs):
        """GetActiveChild() -> MDIChildFrame"""
        return _windows.MDIParentFrame_GetActiveChild(*args, **kwargs)

    def GetClientWindow(*args, **kwargs):
        """GetClientWindow() -> MDIClientWindow"""
        return _windows.MDIParentFrame_GetClientWindow(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar() -> Window"""
        return _windows.MDIParentFrame_GetToolBar(*args, **kwargs)

    def Tile(*args, **kwargs):
        """Tile()"""
        return _windows.MDIParentFrame_Tile(*args, **kwargs)


class MDIParentFramePtr(MDIParentFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MDIParentFrame
_windows.MDIParentFrame_swigregister(MDIParentFramePtr)

def PreMDIParentFrame(*args, **kwargs):
    """PreMDIParentFrame() -> MDIParentFrame"""
    val = _windows.new_PreMDIParentFrame(*args, **kwargs)
    val.thisown = 1
    return val

class MDIChildFrame(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMDIChildFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(MDIParentFrame parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, wxString name=wxPyFrameNameStr) -> MDIChildFrame
        """
        newobj = _windows.new_MDIChildFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(MDIParentFrame parent, int id, wxString title, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, wxString name=wxPyFrameNameStr) -> bool
        """
        return _windows.MDIChildFrame_Create(*args, **kwargs)

    def Activate(*args, **kwargs):
        """Activate()"""
        return _windows.MDIChildFrame_Activate(*args, **kwargs)

    def Maximize(*args, **kwargs):
        """Maximize(bool maximize)"""
        return _windows.MDIChildFrame_Maximize(*args, **kwargs)

    def Restore(*args, **kwargs):
        """Restore()"""
        return _windows.MDIChildFrame_Restore(*args, **kwargs)


class MDIChildFramePtr(MDIChildFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MDIChildFrame
_windows.MDIChildFrame_swigregister(MDIChildFramePtr)

def PreMDIChildFrame(*args, **kwargs):
    """PreMDIChildFrame() -> MDIChildFrame"""
    val = _windows.new_PreMDIChildFrame(*args, **kwargs)
    val.thisown = 1
    return val

class MDIClientWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMDIClientWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(MDIParentFrame parent, long style=0) -> MDIClientWindow"""
        newobj = _windows.new_MDIClientWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(MDIParentFrame parent, long style=0) -> bool"""
        return _windows.MDIClientWindow_Create(*args, **kwargs)


class MDIClientWindowPtr(MDIClientWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MDIClientWindow
_windows.MDIClientWindow_swigregister(MDIClientWindowPtr)

def PreMDIClientWindow(*args, **kwargs):
    """PreMDIClientWindow() -> MDIClientWindow"""
    val = _windows.new_PreMDIClientWindow(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class PyWindow(core.Window):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, wxString name=PanelNameStr) -> PyWindow
        """
        newobj = _windows.new_PyWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, PyWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PyWindow__setCallbackInfo(*args, **kwargs)

    def base_DoMoveWindow(*args, **kwargs):
        """base_DoMoveWindow(int x, int y, int width, int height)"""
        return _windows.PyWindow_base_DoMoveWindow(*args, **kwargs)

    def base_DoSetSize(*args, **kwargs):
        """base_DoSetSize(int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _windows.PyWindow_base_DoSetSize(*args, **kwargs)

    def base_DoSetClientSize(*args, **kwargs):
        """base_DoSetClientSize(int width, int height)"""
        return _windows.PyWindow_base_DoSetClientSize(*args, **kwargs)

    def base_DoSetVirtualSize(*args, **kwargs):
        """base_DoSetVirtualSize(int x, int y)"""
        return _windows.PyWindow_base_DoSetVirtualSize(*args, **kwargs)

    def base_DoGetSize(*args, **kwargs):
        """base_DoGetSize() -> (width, height)"""
        return _windows.PyWindow_base_DoGetSize(*args, **kwargs)

    def base_DoGetClientSize(*args, **kwargs):
        """base_DoGetClientSize() -> (width, height)"""
        return _windows.PyWindow_base_DoGetClientSize(*args, **kwargs)

    def base_DoGetPosition(*args, **kwargs):
        """base_DoGetPosition() -> (x,y)"""
        return _windows.PyWindow_base_DoGetPosition(*args, **kwargs)

    def base_DoGetVirtualSize(*args, **kwargs):
        """base_DoGetVirtualSize() -> Size"""
        return _windows.PyWindow_base_DoGetVirtualSize(*args, **kwargs)

    def base_DoGetBestSize(*args, **kwargs):
        """base_DoGetBestSize() -> Size"""
        return _windows.PyWindow_base_DoGetBestSize(*args, **kwargs)

    def base_InitDialog(*args, **kwargs):
        """base_InitDialog()"""
        return _windows.PyWindow_base_InitDialog(*args, **kwargs)

    def base_TransferDataToWindow(*args, **kwargs):
        """base_TransferDataToWindow() -> bool"""
        return _windows.PyWindow_base_TransferDataToWindow(*args, **kwargs)

    def base_TransferDataFromWindow(*args, **kwargs):
        """base_TransferDataFromWindow() -> bool"""
        return _windows.PyWindow_base_TransferDataFromWindow(*args, **kwargs)

    def base_Validate(*args, **kwargs):
        """base_Validate() -> bool"""
        return _windows.PyWindow_base_Validate(*args, **kwargs)

    def base_AcceptsFocus(*args, **kwargs):
        """base_AcceptsFocus() -> bool"""
        return _windows.PyWindow_base_AcceptsFocus(*args, **kwargs)

    def base_AcceptsFocusFromKeyboard(*args, **kwargs):
        """base_AcceptsFocusFromKeyboard() -> bool"""
        return _windows.PyWindow_base_AcceptsFocusFromKeyboard(*args, **kwargs)

    def base_GetMaxSize(*args, **kwargs):
        """base_GetMaxSize() -> Size"""
        return _windows.PyWindow_base_GetMaxSize(*args, **kwargs)

    def base_AddChild(*args, **kwargs):
        """base_AddChild(Window child)"""
        return _windows.PyWindow_base_AddChild(*args, **kwargs)

    def base_RemoveChild(*args, **kwargs):
        """base_RemoveChild(Window child)"""
        return _windows.PyWindow_base_RemoveChild(*args, **kwargs)


class PyWindowPtr(PyWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyWindow
_windows.PyWindow_swigregister(PyWindowPtr)

class PyPanel(Panel):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPanel instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, wxString name=PanelNameStr) -> PyPanel
        """
        newobj = _windows.new_PyPanel(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, PyPanel)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PyPanel__setCallbackInfo(*args, **kwargs)

    def base_DoMoveWindow(*args, **kwargs):
        """base_DoMoveWindow(int x, int y, int width, int height)"""
        return _windows.PyPanel_base_DoMoveWindow(*args, **kwargs)

    def base_DoSetSize(*args, **kwargs):
        """base_DoSetSize(int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _windows.PyPanel_base_DoSetSize(*args, **kwargs)

    def base_DoSetClientSize(*args, **kwargs):
        """base_DoSetClientSize(int width, int height)"""
        return _windows.PyPanel_base_DoSetClientSize(*args, **kwargs)

    def base_DoSetVirtualSize(*args, **kwargs):
        """base_DoSetVirtualSize(int x, int y)"""
        return _windows.PyPanel_base_DoSetVirtualSize(*args, **kwargs)

    def base_DoGetSize(*args, **kwargs):
        """base_DoGetSize() -> (width, height)"""
        return _windows.PyPanel_base_DoGetSize(*args, **kwargs)

    def base_DoGetClientSize(*args, **kwargs):
        """base_DoGetClientSize() -> (width, height)"""
        return _windows.PyPanel_base_DoGetClientSize(*args, **kwargs)

    def base_DoGetPosition(*args, **kwargs):
        """base_DoGetPosition() -> (x,y)"""
        return _windows.PyPanel_base_DoGetPosition(*args, **kwargs)

    def base_DoGetVirtualSize(*args, **kwargs):
        """base_DoGetVirtualSize() -> Size"""
        return _windows.PyPanel_base_DoGetVirtualSize(*args, **kwargs)

    def base_DoGetBestSize(*args, **kwargs):
        """base_DoGetBestSize() -> Size"""
        return _windows.PyPanel_base_DoGetBestSize(*args, **kwargs)

    def base_InitDialog(*args, **kwargs):
        """base_InitDialog()"""
        return _windows.PyPanel_base_InitDialog(*args, **kwargs)

    def base_TransferDataToWindow(*args, **kwargs):
        """base_TransferDataToWindow() -> bool"""
        return _windows.PyPanel_base_TransferDataToWindow(*args, **kwargs)

    def base_TransferDataFromWindow(*args, **kwargs):
        """base_TransferDataFromWindow() -> bool"""
        return _windows.PyPanel_base_TransferDataFromWindow(*args, **kwargs)

    def base_Validate(*args, **kwargs):
        """base_Validate() -> bool"""
        return _windows.PyPanel_base_Validate(*args, **kwargs)

    def base_AcceptsFocus(*args, **kwargs):
        """base_AcceptsFocus() -> bool"""
        return _windows.PyPanel_base_AcceptsFocus(*args, **kwargs)

    def base_AcceptsFocusFromKeyboard(*args, **kwargs):
        """base_AcceptsFocusFromKeyboard() -> bool"""
        return _windows.PyPanel_base_AcceptsFocusFromKeyboard(*args, **kwargs)

    def base_GetMaxSize(*args, **kwargs):
        """base_GetMaxSize() -> Size"""
        return _windows.PyPanel_base_GetMaxSize(*args, **kwargs)

    def base_AddChild(*args, **kwargs):
        """base_AddChild(Window child)"""
        return _windows.PyPanel_base_AddChild(*args, **kwargs)

    def base_RemoveChild(*args, **kwargs):
        """base_RemoveChild(Window child)"""
        return _windows.PyPanel_base_RemoveChild(*args, **kwargs)


class PyPanelPtr(PyPanel):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPanel
_windows.PyPanel_swigregister(PyPanelPtr)

#---------------------------------------------------------------------------

PRINT_MODE_NONE = _windows.PRINT_MODE_NONE
PRINT_MODE_PREVIEW = _windows.PRINT_MODE_PREVIEW
PRINT_MODE_FILE = _windows.PRINT_MODE_FILE
PRINT_MODE_PRINTER = _windows.PRINT_MODE_PRINTER
class PrintData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrintData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PrintData"""
        newobj = _windows.new_PrintData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_PrintData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetNoCopies(*args, **kwargs):
        """GetNoCopies() -> int"""
        return _windows.PrintData_GetNoCopies(*args, **kwargs)

    def GetCollate(*args, **kwargs):
        """GetCollate() -> bool"""
        return _windows.PrintData_GetCollate(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation() -> int"""
        return _windows.PrintData_GetOrientation(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _windows.PrintData_Ok(*args, **kwargs)

    def GetPrinterName(*args, **kwargs):
        """GetPrinterName() -> wxString"""
        return _windows.PrintData_GetPrinterName(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> bool"""
        return _windows.PrintData_GetColour(*args, **kwargs)

    def GetDuplex(*args, **kwargs):
        """GetDuplex() -> wxDuplexMode"""
        return _windows.PrintData_GetDuplex(*args, **kwargs)

    def GetPaperId(*args, **kwargs):
        """GetPaperId() -> wxPaperSize"""
        return _windows.PrintData_GetPaperId(*args, **kwargs)

    def GetPaperSize(*args, **kwargs):
        """GetPaperSize() -> Size"""
        return _windows.PrintData_GetPaperSize(*args, **kwargs)

    def GetQuality(*args, **kwargs):
        """GetQuality() -> wxPrintQuality"""
        return _windows.PrintData_GetQuality(*args, **kwargs)

    def SetNoCopies(*args, **kwargs):
        """SetNoCopies(int v)"""
        return _windows.PrintData_SetNoCopies(*args, **kwargs)

    def SetCollate(*args, **kwargs):
        """SetCollate(bool flag)"""
        return _windows.PrintData_SetCollate(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(int orient)"""
        return _windows.PrintData_SetOrientation(*args, **kwargs)

    def SetPrinterName(*args, **kwargs):
        """SetPrinterName(wxString name)"""
        return _windows.PrintData_SetPrinterName(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(bool colour)"""
        return _windows.PrintData_SetColour(*args, **kwargs)

    def SetDuplex(*args, **kwargs):
        """SetDuplex(wxDuplexMode duplex)"""
        return _windows.PrintData_SetDuplex(*args, **kwargs)

    def SetPaperId(*args, **kwargs):
        """SetPaperId(wxPaperSize sizeId)"""
        return _windows.PrintData_SetPaperId(*args, **kwargs)

    def SetPaperSize(*args, **kwargs):
        """SetPaperSize(Size sz)"""
        return _windows.PrintData_SetPaperSize(*args, **kwargs)

    def SetQuality(*args, **kwargs):
        """SetQuality(wxPrintQuality quality)"""
        return _windows.PrintData_SetQuality(*args, **kwargs)

    def GetPrinterCommand(*args, **kwargs):
        """GetPrinterCommand() -> wxString"""
        return _windows.PrintData_GetPrinterCommand(*args, **kwargs)

    def GetPrinterOptions(*args, **kwargs):
        """GetPrinterOptions() -> wxString"""
        return _windows.PrintData_GetPrinterOptions(*args, **kwargs)

    def GetPreviewCommand(*args, **kwargs):
        """GetPreviewCommand() -> wxString"""
        return _windows.PrintData_GetPreviewCommand(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename() -> wxString"""
        return _windows.PrintData_GetFilename(*args, **kwargs)

    def GetFontMetricPath(*args, **kwargs):
        """GetFontMetricPath() -> wxString"""
        return _windows.PrintData_GetFontMetricPath(*args, **kwargs)

    def GetPrinterScaleX(*args, **kwargs):
        """GetPrinterScaleX() -> double"""
        return _windows.PrintData_GetPrinterScaleX(*args, **kwargs)

    def GetPrinterScaleY(*args, **kwargs):
        """GetPrinterScaleY() -> double"""
        return _windows.PrintData_GetPrinterScaleY(*args, **kwargs)

    def GetPrinterTranslateX(*args, **kwargs):
        """GetPrinterTranslateX() -> long"""
        return _windows.PrintData_GetPrinterTranslateX(*args, **kwargs)

    def GetPrinterTranslateY(*args, **kwargs):
        """GetPrinterTranslateY() -> long"""
        return _windows.PrintData_GetPrinterTranslateY(*args, **kwargs)

    def GetPrintMode(*args, **kwargs):
        """GetPrintMode() -> wxPrintMode"""
        return _windows.PrintData_GetPrintMode(*args, **kwargs)

    def SetPrinterCommand(*args, **kwargs):
        """SetPrinterCommand(wxString command)"""
        return _windows.PrintData_SetPrinterCommand(*args, **kwargs)

    def SetPrinterOptions(*args, **kwargs):
        """SetPrinterOptions(wxString options)"""
        return _windows.PrintData_SetPrinterOptions(*args, **kwargs)

    def SetPreviewCommand(*args, **kwargs):
        """SetPreviewCommand(wxString command)"""
        return _windows.PrintData_SetPreviewCommand(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(wxString filename)"""
        return _windows.PrintData_SetFilename(*args, **kwargs)

    def SetFontMetricPath(*args, **kwargs):
        """SetFontMetricPath(wxString path)"""
        return _windows.PrintData_SetFontMetricPath(*args, **kwargs)

    def SetPrinterScaleX(*args, **kwargs):
        """SetPrinterScaleX(double x)"""
        return _windows.PrintData_SetPrinterScaleX(*args, **kwargs)

    def SetPrinterScaleY(*args, **kwargs):
        """SetPrinterScaleY(double y)"""
        return _windows.PrintData_SetPrinterScaleY(*args, **kwargs)

    def SetPrinterScaling(*args, **kwargs):
        """SetPrinterScaling(double x, double y)"""
        return _windows.PrintData_SetPrinterScaling(*args, **kwargs)

    def SetPrinterTranslateX(*args, **kwargs):
        """SetPrinterTranslateX(long x)"""
        return _windows.PrintData_SetPrinterTranslateX(*args, **kwargs)

    def SetPrinterTranslateY(*args, **kwargs):
        """SetPrinterTranslateY(long y)"""
        return _windows.PrintData_SetPrinterTranslateY(*args, **kwargs)

    def SetPrinterTranslation(*args, **kwargs):
        """SetPrinterTranslation(long x, long y)"""
        return _windows.PrintData_SetPrinterTranslation(*args, **kwargs)

    def SetPrintMode(*args, **kwargs):
        """SetPrintMode(wxPrintMode printMode)"""
        return _windows.PrintData_SetPrintMode(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PrintDataPtr(PrintData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrintData
_windows.PrintData_swigregister(PrintDataPtr)

class PageSetupDialogData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPageSetupDialogData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PageSetupDialogData"""
        newobj = _windows.new_PageSetupDialogData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_PageSetupDialogData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def EnableHelp(*args, **kwargs):
        """EnableHelp(bool flag)"""
        return _windows.PageSetupDialogData_EnableHelp(*args, **kwargs)

    def EnableMargins(*args, **kwargs):
        """EnableMargins(bool flag)"""
        return _windows.PageSetupDialogData_EnableMargins(*args, **kwargs)

    def EnableOrientation(*args, **kwargs):
        """EnableOrientation(bool flag)"""
        return _windows.PageSetupDialogData_EnableOrientation(*args, **kwargs)

    def EnablePaper(*args, **kwargs):
        """EnablePaper(bool flag)"""
        return _windows.PageSetupDialogData_EnablePaper(*args, **kwargs)

    def EnablePrinter(*args, **kwargs):
        """EnablePrinter(bool flag)"""
        return _windows.PageSetupDialogData_EnablePrinter(*args, **kwargs)

    def GetDefaultMinMargins(*args, **kwargs):
        """GetDefaultMinMargins() -> bool"""
        return _windows.PageSetupDialogData_GetDefaultMinMargins(*args, **kwargs)

    def GetEnableMargins(*args, **kwargs):
        """GetEnableMargins() -> bool"""
        return _windows.PageSetupDialogData_GetEnableMargins(*args, **kwargs)

    def GetEnableOrientation(*args, **kwargs):
        """GetEnableOrientation() -> bool"""
        return _windows.PageSetupDialogData_GetEnableOrientation(*args, **kwargs)

    def GetEnablePaper(*args, **kwargs):
        """GetEnablePaper() -> bool"""
        return _windows.PageSetupDialogData_GetEnablePaper(*args, **kwargs)

    def GetEnablePrinter(*args, **kwargs):
        """GetEnablePrinter() -> bool"""
        return _windows.PageSetupDialogData_GetEnablePrinter(*args, **kwargs)

    def GetEnableHelp(*args, **kwargs):
        """GetEnableHelp() -> bool"""
        return _windows.PageSetupDialogData_GetEnableHelp(*args, **kwargs)

    def GetDefaultInfo(*args, **kwargs):
        """GetDefaultInfo() -> bool"""
        return _windows.PageSetupDialogData_GetDefaultInfo(*args, **kwargs)

    def GetMarginTopLeft(*args, **kwargs):
        """GetMarginTopLeft() -> Point"""
        return _windows.PageSetupDialogData_GetMarginTopLeft(*args, **kwargs)

    def GetMarginBottomRight(*args, **kwargs):
        """GetMarginBottomRight() -> Point"""
        return _windows.PageSetupDialogData_GetMarginBottomRight(*args, **kwargs)

    def GetMinMarginTopLeft(*args, **kwargs):
        """GetMinMarginTopLeft() -> Point"""
        return _windows.PageSetupDialogData_GetMinMarginTopLeft(*args, **kwargs)

    def GetMinMarginBottomRight(*args, **kwargs):
        """GetMinMarginBottomRight() -> Point"""
        return _windows.PageSetupDialogData_GetMinMarginBottomRight(*args, **kwargs)

    def GetPaperId(*args, **kwargs):
        """GetPaperId() -> wxPaperSize"""
        return _windows.PageSetupDialogData_GetPaperId(*args, **kwargs)

    def GetPaperSize(*args, **kwargs):
        """GetPaperSize() -> Size"""
        return _windows.PageSetupDialogData_GetPaperSize(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData() -> PrintData"""
        return _windows.PageSetupDialogData_GetPrintData(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _windows.PageSetupDialogData_Ok(*args, **kwargs)

    def SetDefaultInfo(*args, **kwargs):
        """SetDefaultInfo(bool flag)"""
        return _windows.PageSetupDialogData_SetDefaultInfo(*args, **kwargs)

    def SetDefaultMinMargins(*args, **kwargs):
        """SetDefaultMinMargins(bool flag)"""
        return _windows.PageSetupDialogData_SetDefaultMinMargins(*args, **kwargs)

    def SetMarginTopLeft(*args, **kwargs):
        """SetMarginTopLeft(Point pt)"""
        return _windows.PageSetupDialogData_SetMarginTopLeft(*args, **kwargs)

    def SetMarginBottomRight(*args, **kwargs):
        """SetMarginBottomRight(Point pt)"""
        return _windows.PageSetupDialogData_SetMarginBottomRight(*args, **kwargs)

    def SetMinMarginTopLeft(*args, **kwargs):
        """SetMinMarginTopLeft(Point pt)"""
        return _windows.PageSetupDialogData_SetMinMarginTopLeft(*args, **kwargs)

    def SetMinMarginBottomRight(*args, **kwargs):
        """SetMinMarginBottomRight(Point pt)"""
        return _windows.PageSetupDialogData_SetMinMarginBottomRight(*args, **kwargs)

    def SetPaperId(*args, **kwargs):
        """SetPaperId(wxPaperSize id)"""
        return _windows.PageSetupDialogData_SetPaperId(*args, **kwargs)

    def SetPaperSize(*args, **kwargs):
        """SetPaperSize(Size size)"""
        return _windows.PageSetupDialogData_SetPaperSize(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(PrintData printData)"""
        return _windows.PageSetupDialogData_SetPrintData(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PageSetupDialogDataPtr(PageSetupDialogData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PageSetupDialogData
_windows.PageSetupDialogData_swigregister(PageSetupDialogDataPtr)

class PageSetupDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPageSetupDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, PageSetupDialogData data=None) -> PageSetupDialog"""
        newobj = _windows.new_PageSetupDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetPageSetupData(*args, **kwargs):
        """GetPageSetupData() -> PageSetupDialogData"""
        return _windows.PageSetupDialog_GetPageSetupData(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.PageSetupDialog_ShowModal(*args, **kwargs)


class PageSetupDialogPtr(PageSetupDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PageSetupDialog
_windows.PageSetupDialog_swigregister(PageSetupDialogPtr)

class PrintDialogData(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrintDialogData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PrintDialogData"""
        newobj = _windows.new_PrintDialogData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_PrintDialogData):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetFromPage(*args, **kwargs):
        """GetFromPage() -> int"""
        return _windows.PrintDialogData_GetFromPage(*args, **kwargs)

    def GetToPage(*args, **kwargs):
        """GetToPage() -> int"""
        return _windows.PrintDialogData_GetToPage(*args, **kwargs)

    def GetMinPage(*args, **kwargs):
        """GetMinPage() -> int"""
        return _windows.PrintDialogData_GetMinPage(*args, **kwargs)

    def GetMaxPage(*args, **kwargs):
        """GetMaxPage() -> int"""
        return _windows.PrintDialogData_GetMaxPage(*args, **kwargs)

    def GetNoCopies(*args, **kwargs):
        """GetNoCopies() -> int"""
        return _windows.PrintDialogData_GetNoCopies(*args, **kwargs)

    def GetAllPages(*args, **kwargs):
        """GetAllPages() -> bool"""
        return _windows.PrintDialogData_GetAllPages(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> bool"""
        return _windows.PrintDialogData_GetSelection(*args, **kwargs)

    def GetCollate(*args, **kwargs):
        """GetCollate() -> bool"""
        return _windows.PrintDialogData_GetCollate(*args, **kwargs)

    def GetPrintToFile(*args, **kwargs):
        """GetPrintToFile() -> bool"""
        return _windows.PrintDialogData_GetPrintToFile(*args, **kwargs)

    def GetSetupDialog(*args, **kwargs):
        """GetSetupDialog() -> bool"""
        return _windows.PrintDialogData_GetSetupDialog(*args, **kwargs)

    def SetFromPage(*args, **kwargs):
        """SetFromPage(int v)"""
        return _windows.PrintDialogData_SetFromPage(*args, **kwargs)

    def SetToPage(*args, **kwargs):
        """SetToPage(int v)"""
        return _windows.PrintDialogData_SetToPage(*args, **kwargs)

    def SetMinPage(*args, **kwargs):
        """SetMinPage(int v)"""
        return _windows.PrintDialogData_SetMinPage(*args, **kwargs)

    def SetMaxPage(*args, **kwargs):
        """SetMaxPage(int v)"""
        return _windows.PrintDialogData_SetMaxPage(*args, **kwargs)

    def SetNoCopies(*args, **kwargs):
        """SetNoCopies(int v)"""
        return _windows.PrintDialogData_SetNoCopies(*args, **kwargs)

    def SetAllPages(*args, **kwargs):
        """SetAllPages(bool flag)"""
        return _windows.PrintDialogData_SetAllPages(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(bool flag)"""
        return _windows.PrintDialogData_SetSelection(*args, **kwargs)

    def SetCollate(*args, **kwargs):
        """SetCollate(bool flag)"""
        return _windows.PrintDialogData_SetCollate(*args, **kwargs)

    def SetPrintToFile(*args, **kwargs):
        """SetPrintToFile(bool flag)"""
        return _windows.PrintDialogData_SetPrintToFile(*args, **kwargs)

    def SetSetupDialog(*args, **kwargs):
        """SetSetupDialog(bool flag)"""
        return _windows.PrintDialogData_SetSetupDialog(*args, **kwargs)

    def EnablePrintToFile(*args, **kwargs):
        """EnablePrintToFile(bool flag)"""
        return _windows.PrintDialogData_EnablePrintToFile(*args, **kwargs)

    def EnableSelection(*args, **kwargs):
        """EnableSelection(bool flag)"""
        return _windows.PrintDialogData_EnableSelection(*args, **kwargs)

    def EnablePageNumbers(*args, **kwargs):
        """EnablePageNumbers(bool flag)"""
        return _windows.PrintDialogData_EnablePageNumbers(*args, **kwargs)

    def EnableHelp(*args, **kwargs):
        """EnableHelp(bool flag)"""
        return _windows.PrintDialogData_EnableHelp(*args, **kwargs)

    def GetEnablePrintToFile(*args, **kwargs):
        """GetEnablePrintToFile() -> bool"""
        return _windows.PrintDialogData_GetEnablePrintToFile(*args, **kwargs)

    def GetEnableSelection(*args, **kwargs):
        """GetEnableSelection() -> bool"""
        return _windows.PrintDialogData_GetEnableSelection(*args, **kwargs)

    def GetEnablePageNumbers(*args, **kwargs):
        """GetEnablePageNumbers() -> bool"""
        return _windows.PrintDialogData_GetEnablePageNumbers(*args, **kwargs)

    def GetEnableHelp(*args, **kwargs):
        """GetEnableHelp() -> bool"""
        return _windows.PrintDialogData_GetEnableHelp(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _windows.PrintDialogData_Ok(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData() -> PrintData"""
        return _windows.PrintDialogData_GetPrintData(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(PrintData printData)"""
        return _windows.PrintDialogData_SetPrintData(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PrintDialogDataPtr(PrintDialogData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrintDialogData
_windows.PrintDialogData_swigregister(PrintDialogDataPtr)

class PrintDialog(Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrintDialog instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, PrintDialogData data=None) -> PrintDialog"""
        newobj = _windows.new_PrintDialog(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData() -> PrintDialogData"""
        return _windows.PrintDialog_GetPrintDialogData(*args, **kwargs)

    def GetPrintDC(*args, **kwargs):
        """GetPrintDC() -> wxDC"""
        return _windows.PrintDialog_GetPrintDC(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal() -> int"""
        return _windows.PrintDialog_ShowModal(*args, **kwargs)


class PrintDialogPtr(PrintDialog):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrintDialog
_windows.PrintDialog_swigregister(PrintDialogPtr)

PRINTER_NO_ERROR = _windows.PRINTER_NO_ERROR
PRINTER_CANCELLED = _windows.PRINTER_CANCELLED
PRINTER_ERROR = _windows.PRINTER_ERROR
class Printer(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrinter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(PrintDialogData data=None) -> Printer"""
        newobj = _windows.new_Printer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_windows.delete_Printer):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def CreateAbortWindow(*args, **kwargs):
        """CreateAbortWindow(Window parent, Printout printout)"""
        return _windows.Printer_CreateAbortWindow(*args, **kwargs)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData() -> PrintDialogData"""
        return _windows.Printer_GetPrintDialogData(*args, **kwargs)

    def Print(*args, **kwargs):
        """Print(Window parent, Printout printout, int prompt=True) -> bool"""
        return _windows.Printer_Print(*args, **kwargs)

    def PrintDialog(*args, **kwargs):
        """PrintDialog(Window parent) -> wxDC"""
        return _windows.Printer_PrintDialog(*args, **kwargs)

    def ReportError(*args, **kwargs):
        """ReportError(Window parent, Printout printout, wxString message)"""
        return _windows.Printer_ReportError(*args, **kwargs)

    def Setup(*args, **kwargs):
        """Setup(Window parent) -> bool"""
        return _windows.Printer_Setup(*args, **kwargs)

    def GetAbort(*args, **kwargs):
        """GetAbort() -> bool"""
        return _windows.Printer_GetAbort(*args, **kwargs)

    def GetLastError(*args, **kwargs):
        """Printer.GetLastError() -> wxPrinterError"""
        return _windows.Printer_GetLastError(*args, **kwargs)

    GetLastError = staticmethod(GetLastError)

class PrinterPtr(Printer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Printer
_windows.Printer_swigregister(PrinterPtr)

def Printer_GetLastError(*args, **kwargs):
    """Printer_GetLastError() -> wxPrinterError"""
    return _windows.Printer_GetLastError(*args, **kwargs)

class Printout(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPrintout instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString title=wxPyPrintoutTitleStr) -> Printout"""
        newobj = _windows.new_Printout(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, Printout)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.Printout__setCallbackInfo(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle() -> wxString"""
        return _windows.Printout_GetTitle(*args, **kwargs)

    def GetDC(*args, **kwargs):
        """GetDC() -> wxDC"""
        return _windows.Printout_GetDC(*args, **kwargs)

    def SetDC(*args, **kwargs):
        """SetDC(wxDC dc)"""
        return _windows.Printout_SetDC(*args, **kwargs)

    def SetPageSizePixels(*args, **kwargs):
        """SetPageSizePixels(int w, int h)"""
        return _windows.Printout_SetPageSizePixels(*args, **kwargs)

    def GetPageSizePixels(*args, **kwargs):
        """GetPageSizePixels() -> (w, h)"""
        return _windows.Printout_GetPageSizePixels(*args, **kwargs)

    def SetPageSizeMM(*args, **kwargs):
        """SetPageSizeMM(int w, int h)"""
        return _windows.Printout_SetPageSizeMM(*args, **kwargs)

    def GetPageSizeMM(*args, **kwargs):
        """GetPageSizeMM() -> (w, h)"""
        return _windows.Printout_GetPageSizeMM(*args, **kwargs)

    def SetPPIScreen(*args, **kwargs):
        """SetPPIScreen(int x, int y)"""
        return _windows.Printout_SetPPIScreen(*args, **kwargs)

    def GetPPIScreen(*args, **kwargs):
        """GetPPIScreen() -> (x,y)"""
        return _windows.Printout_GetPPIScreen(*args, **kwargs)

    def SetPPIPrinter(*args, **kwargs):
        """SetPPIPrinter(int x, int y)"""
        return _windows.Printout_SetPPIPrinter(*args, **kwargs)

    def GetPPIPrinter(*args, **kwargs):
        """GetPPIPrinter() -> (x,y)"""
        return _windows.Printout_GetPPIPrinter(*args, **kwargs)

    def IsPreview(*args, **kwargs):
        """IsPreview() -> bool"""
        return _windows.Printout_IsPreview(*args, **kwargs)

    def SetIsPreview(*args, **kwargs):
        """SetIsPreview(bool p)"""
        return _windows.Printout_SetIsPreview(*args, **kwargs)

    def base_OnBeginDocument(*args, **kwargs):
        """base_OnBeginDocument(int startPage, int endPage) -> bool"""
        return _windows.Printout_base_OnBeginDocument(*args, **kwargs)

    def base_OnEndDocument(*args, **kwargs):
        """base_OnEndDocument()"""
        return _windows.Printout_base_OnEndDocument(*args, **kwargs)

    def base_OnBeginPrinting(*args, **kwargs):
        """base_OnBeginPrinting()"""
        return _windows.Printout_base_OnBeginPrinting(*args, **kwargs)

    def base_OnEndPrinting(*args, **kwargs):
        """base_OnEndPrinting()"""
        return _windows.Printout_base_OnEndPrinting(*args, **kwargs)

    def base_OnPreparePrinting(*args, **kwargs):
        """base_OnPreparePrinting()"""
        return _windows.Printout_base_OnPreparePrinting(*args, **kwargs)

    def base_HasPage(*args, **kwargs):
        """base_HasPage(int page) -> bool"""
        return _windows.Printout_base_HasPage(*args, **kwargs)

    def base_GetPageInfo(*args, **kwargs):
        """base_GetPageInfo() -> (minPage, maxPage, pageFrom, pageTo)"""
        return _windows.Printout_base_GetPageInfo(*args, **kwargs)


class PrintoutPtr(Printout):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Printout
_windows.Printout_swigregister(PrintoutPtr)

class PreviewCanvas(ScrolledWindow):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPreviewCanvas instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(PrintPreview preview, Window parent, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, 
            wxString name=wxPyPreviewCanvasNameStr) -> PreviewCanvas
        """
        newobj = _windows.new_PreviewCanvas(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class PreviewCanvasPtr(PreviewCanvas):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PreviewCanvas
_windows.PreviewCanvas_swigregister(PreviewCanvasPtr)

class PreviewFrame(Frame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPreviewFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(PrintPreview preview, Frame parent, wxString title, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, wxString name=wxPyFrameNameStr) -> PreviewFrame
        """
        newobj = _windows.new_PreviewFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Initialize(*args, **kwargs):
        """Initialize()"""
        return _windows.PreviewFrame_Initialize(*args, **kwargs)

    def CreateControlBar(*args, **kwargs):
        """CreateControlBar()"""
        return _windows.PreviewFrame_CreateControlBar(*args, **kwargs)

    def CreateCanvas(*args, **kwargs):
        """CreateCanvas()"""
        return _windows.PreviewFrame_CreateCanvas(*args, **kwargs)

    def GetControlBar(*args, **kwargs):
        """GetControlBar() -> PreviewControlBar"""
        return _windows.PreviewFrame_GetControlBar(*args, **kwargs)


class PreviewFramePtr(PreviewFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PreviewFrame
_windows.PreviewFrame_swigregister(PreviewFramePtr)

PREVIEW_PRINT = _windows.PREVIEW_PRINT
PREVIEW_PREVIOUS = _windows.PREVIEW_PREVIOUS
PREVIEW_NEXT = _windows.PREVIEW_NEXT
PREVIEW_ZOOM = _windows.PREVIEW_ZOOM
PREVIEW_FIRST = _windows.PREVIEW_FIRST
PREVIEW_LAST = _windows.PREVIEW_LAST
PREVIEW_GOTO = _windows.PREVIEW_GOTO
PREVIEW_DEFAULT = _windows.PREVIEW_DEFAULT
ID_PREVIEW_CLOSE = _windows.ID_PREVIEW_CLOSE
ID_PREVIEW_NEXT = _windows.ID_PREVIEW_NEXT
ID_PREVIEW_PREVIOUS = _windows.ID_PREVIEW_PREVIOUS
ID_PREVIEW_PRINT = _windows.ID_PREVIEW_PRINT
ID_PREVIEW_ZOOM = _windows.ID_PREVIEW_ZOOM
ID_PREVIEW_FIRST = _windows.ID_PREVIEW_FIRST
ID_PREVIEW_LAST = _windows.ID_PREVIEW_LAST
ID_PREVIEW_GOTO = _windows.ID_PREVIEW_GOTO
class PreviewControlBar(Panel):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPreviewControlBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(PrintPreview preview, long buttons, Window parent, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=TAB_TRAVERSAL, wxString name=PanelNameStr) -> PreviewControlBar
        """
        newobj = _windows.new_PreviewControlBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetZoomControl(*args, **kwargs):
        """GetZoomControl() -> int"""
        return _windows.PreviewControlBar_GetZoomControl(*args, **kwargs)

    def SetZoomControl(*args, **kwargs):
        """SetZoomControl(int zoom)"""
        return _windows.PreviewControlBar_SetZoomControl(*args, **kwargs)

    def GetPrintPreview(*args, **kwargs):
        """GetPrintPreview() -> PrintPreview"""
        return _windows.PreviewControlBar_GetPrintPreview(*args, **kwargs)

    def OnNext(*args, **kwargs):
        """OnNext()"""
        return _windows.PreviewControlBar_OnNext(*args, **kwargs)

    def OnPrevious(*args, **kwargs):
        """OnPrevious()"""
        return _windows.PreviewControlBar_OnPrevious(*args, **kwargs)

    def OnFirst(*args, **kwargs):
        """OnFirst()"""
        return _windows.PreviewControlBar_OnFirst(*args, **kwargs)

    def OnLast(*args, **kwargs):
        """OnLast()"""
        return _windows.PreviewControlBar_OnLast(*args, **kwargs)

    def OnGoto(*args, **kwargs):
        """OnGoto()"""
        return _windows.PreviewControlBar_OnGoto(*args, **kwargs)


class PreviewControlBarPtr(PreviewControlBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PreviewControlBar
_windows.PreviewControlBar_swigregister(PreviewControlBarPtr)

class PrintPreview(core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrintPreview instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Printout printout, Printout printoutForPrinting, PrintData data=None) -> PrintPreview"""
        newobj = _windows.new_PrintPreview(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetCurrentPage(*args, **kwargs):
        """SetCurrentPage(int pageNum) -> bool"""
        return _windows.PrintPreview_SetCurrentPage(*args, **kwargs)

    def GetCurrentPage(*args, **kwargs):
        """GetCurrentPage() -> int"""
        return _windows.PrintPreview_GetCurrentPage(*args, **kwargs)

    def SetPrintout(*args, **kwargs):
        """SetPrintout(Printout printout)"""
        return _windows.PrintPreview_SetPrintout(*args, **kwargs)

    def GetPrintout(*args, **kwargs):
        """GetPrintout() -> Printout"""
        return _windows.PrintPreview_GetPrintout(*args, **kwargs)

    def GetPrintoutForPrinting(*args, **kwargs):
        """GetPrintoutForPrinting() -> Printout"""
        return _windows.PrintPreview_GetPrintoutForPrinting(*args, **kwargs)

    def SetFrame(*args, **kwargs):
        """SetFrame(Frame frame)"""
        return _windows.PrintPreview_SetFrame(*args, **kwargs)

    def SetCanvas(*args, **kwargs):
        """SetCanvas(PreviewCanvas canvas)"""
        return _windows.PrintPreview_SetCanvas(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame() -> Frame"""
        return _windows.PrintPreview_GetFrame(*args, **kwargs)

    def GetCanvas(*args, **kwargs):
        """GetCanvas() -> PreviewCanvas"""
        return _windows.PrintPreview_GetCanvas(*args, **kwargs)

    def PaintPage(*args, **kwargs):
        """PaintPage(PreviewCanvas canvas, wxDC dc) -> bool"""
        return _windows.PrintPreview_PaintPage(*args, **kwargs)

    def DrawBlankPage(*args, **kwargs):
        """DrawBlankPage(PreviewCanvas canvas, wxDC dc) -> bool"""
        return _windows.PrintPreview_DrawBlankPage(*args, **kwargs)

    def RenderPage(*args, **kwargs):
        """RenderPage(int pageNum) -> bool"""
        return _windows.PrintPreview_RenderPage(*args, **kwargs)

    def AdjustScrollbars(*args, **kwargs):
        """AdjustScrollbars(PreviewCanvas canvas)"""
        return _windows.PrintPreview_AdjustScrollbars(*args, **kwargs)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData() -> PrintDialogData"""
        return _windows.PrintPreview_GetPrintDialogData(*args, **kwargs)

    def SetZoom(*args, **kwargs):
        """SetZoom(int percent)"""
        return _windows.PrintPreview_SetZoom(*args, **kwargs)

    def GetZoom(*args, **kwargs):
        """GetZoom() -> int"""
        return _windows.PrintPreview_GetZoom(*args, **kwargs)

    def GetMaxPage(*args, **kwargs):
        """GetMaxPage() -> int"""
        return _windows.PrintPreview_GetMaxPage(*args, **kwargs)

    def GetMinPage(*args, **kwargs):
        """GetMinPage() -> int"""
        return _windows.PrintPreview_GetMinPage(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _windows.PrintPreview_Ok(*args, **kwargs)

    def SetOk(*args, **kwargs):
        """SetOk(bool ok)"""
        return _windows.PrintPreview_SetOk(*args, **kwargs)

    def Print(*args, **kwargs):
        """Print(bool interactive) -> bool"""
        return _windows.PrintPreview_Print(*args, **kwargs)

    def DetermineScaling(*args, **kwargs):
        """DetermineScaling()"""
        return _windows.PrintPreview_DetermineScaling(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PrintPreviewPtr(PrintPreview):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrintPreview
_windows.PrintPreview_swigregister(PrintPreviewPtr)

class PyPrintPreview(PrintPreview):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPrintPreview instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Printout printout, Printout printoutForPrinting, PrintData data=None) -> PyPrintPreview"""
        newobj = _windows.new_PyPrintPreview(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyPrintPreview)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PyPrintPreview__setCallbackInfo(*args, **kwargs)

    def base_SetCurrentPage(*args, **kwargs):
        """base_SetCurrentPage(int pageNum) -> bool"""
        return _windows.PyPrintPreview_base_SetCurrentPage(*args, **kwargs)

    def base_PaintPage(*args, **kwargs):
        """base_PaintPage(PreviewCanvas canvas, wxDC dc) -> bool"""
        return _windows.PyPrintPreview_base_PaintPage(*args, **kwargs)

    def base_DrawBlankPage(*args, **kwargs):
        """base_DrawBlankPage(PreviewCanvas canvas, wxDC dc) -> bool"""
        return _windows.PyPrintPreview_base_DrawBlankPage(*args, **kwargs)

    def base_RenderPage(*args, **kwargs):
        """base_RenderPage(int pageNum) -> bool"""
        return _windows.PyPrintPreview_base_RenderPage(*args, **kwargs)

    def base_SetZoom(*args, **kwargs):
        """base_SetZoom(int percent)"""
        return _windows.PyPrintPreview_base_SetZoom(*args, **kwargs)

    def base_Print(*args, **kwargs):
        """base_Print(bool interactive) -> bool"""
        return _windows.PyPrintPreview_base_Print(*args, **kwargs)

    def base_DetermineScaling(*args, **kwargs):
        """base_DetermineScaling()"""
        return _windows.PyPrintPreview_base_DetermineScaling(*args, **kwargs)


class PyPrintPreviewPtr(PyPrintPreview):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPrintPreview
_windows.PyPrintPreview_swigregister(PyPrintPreviewPtr)

class PyPreviewFrame(PreviewFrame):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPreviewFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(PrintPreview preview, Frame parent, wxString title, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, wxString name=wxPyFrameNameStr) -> PyPreviewFrame
        """
        newobj = _windows.new_PyPreviewFrame(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyPreviewFrame); self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PyPreviewFrame__setCallbackInfo(*args, **kwargs)

    def SetPreviewCanvas(*args, **kwargs):
        """SetPreviewCanvas(PreviewCanvas canvas)"""
        return _windows.PyPreviewFrame_SetPreviewCanvas(*args, **kwargs)

    def SetControlBar(*args, **kwargs):
        """SetControlBar(PreviewControlBar bar)"""
        return _windows.PyPreviewFrame_SetControlBar(*args, **kwargs)

    def base_Initialize(*args, **kwargs):
        """base_Initialize()"""
        return _windows.PyPreviewFrame_base_Initialize(*args, **kwargs)

    def base_CreateCanvas(*args, **kwargs):
        """base_CreateCanvas()"""
        return _windows.PyPreviewFrame_base_CreateCanvas(*args, **kwargs)

    def base_CreateControlBar(*args, **kwargs):
        """base_CreateControlBar()"""
        return _windows.PyPreviewFrame_base_CreateControlBar(*args, **kwargs)


class PyPreviewFramePtr(PyPreviewFrame):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPreviewFrame
_windows.PyPreviewFrame_swigregister(PyPreviewFramePtr)

class PyPreviewControlBar(PreviewControlBar):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPreviewControlBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(PrintPreview preview, long buttons, Window parent, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, wxString name=PanelNameStr) -> PyPreviewControlBar
        """
        newobj = _windows.new_PyPreviewControlBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyPreviewControlBar); self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _windows.PyPreviewControlBar__setCallbackInfo(*args, **kwargs)

    def SetPrintPreview(*args, **kwargs):
        """SetPrintPreview(PrintPreview preview)"""
        return _windows.PyPreviewControlBar_SetPrintPreview(*args, **kwargs)

    def base_CreateButtons(*args, **kwargs):
        """base_CreateButtons()"""
        return _windows.PyPreviewControlBar_base_CreateButtons(*args, **kwargs)

    def base_SetZoomControl(*args, **kwargs):
        """base_SetZoomControl(int zoom)"""
        return _windows.PyPreviewControlBar_base_SetZoomControl(*args, **kwargs)


class PyPreviewControlBarPtr(PyPreviewControlBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPreviewControlBar
_windows.PyPreviewControlBar_swigregister(PyPreviewControlBarPtr)


