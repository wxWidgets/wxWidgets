# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _windows_
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
#---------------------------------------------------------------------------

class Panel(_core.Window):
    """Proxy of C++ Panel class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxTAB_TRAVERSAL|wxNO_BORDER, 
            String name=PanelNameStr) -> Panel
        """
        _windows_.Panel_swiginit(self,_windows_.new_Panel(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxTAB_TRAVERSAL|wxNO_BORDER, 
            String name=PanelNameStr) -> bool

        Create the GUI part of the Window for 2-phase creation mode.
        """
        return _windows_.Panel_Create(*args, **kwargs)

    def SetFocusIgnoringChildren(*args, **kwargs):
        """
        SetFocusIgnoringChildren(self)

        In contrast to `SetFocus` (see above) this will set the focus to the
        panel even of there are child windows in the panel. This is only
        rarely needed.
        """
        return _windows_.Panel_SetFocusIgnoringChildren(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.Panel_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_windows_.Panel_swigregister(Panel)

def PrePanel(*args, **kwargs):
    """PrePanel() -> Panel"""
    val = _windows_.new_PrePanel(*args, **kwargs)
    return val

def Panel_GetClassDefaultAttributes(*args, **kwargs):
  """
    Panel_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.Panel_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class ScrolledWindow(Panel):
    """Proxy of C++ ScrolledWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxHSCROLL|wxVSCROLL, 
            String name=PanelNameStr) -> ScrolledWindow
        """
        _windows_.ScrolledWindow_swiginit(self,_windows_.new_ScrolledWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxHSCROLL|wxVSCROLL, 
            String name=PanelNameStr) -> bool

        Create the GUI part of the Window for 2-phase creation mode.
        """
        return _windows_.ScrolledWindow_Create(*args, **kwargs)

    def SetScrollbars(*args, **kwargs):
        """
        SetScrollbars(self, int pixelsPerUnitX, int pixelsPerUnitY, int noUnitsX, 
            int noUnitsY, int xPos=0, int yPos=0, bool noRefresh=False)
        """
        return _windows_.ScrolledWindow_SetScrollbars(*args, **kwargs)

    def Scroll(*args, **kwargs):
        """Scroll(self, int x, int y)"""
        return _windows_.ScrolledWindow_Scroll(*args, **kwargs)

    def GetScrollPageSize(*args, **kwargs):
        """GetScrollPageSize(self, int orient) -> int"""
        return _windows_.ScrolledWindow_GetScrollPageSize(*args, **kwargs)

    def SetScrollPageSize(*args, **kwargs):
        """SetScrollPageSize(self, int orient, int pageSize)"""
        return _windows_.ScrolledWindow_SetScrollPageSize(*args, **kwargs)

    def SetScrollRate(*args, **kwargs):
        """SetScrollRate(self, int xstep, int ystep)"""
        return _windows_.ScrolledWindow_SetScrollRate(*args, **kwargs)

    def GetScrollPixelsPerUnit(*args, **kwargs):
        """
        GetScrollPixelsPerUnit() -> (xUnit, yUnit)

        Get the size of one logical unit in physical units.
        """
        return _windows_.ScrolledWindow_GetScrollPixelsPerUnit(*args, **kwargs)

    def EnableScrolling(*args, **kwargs):
        """EnableScrolling(self, bool x_scrolling, bool y_scrolling)"""
        return _windows_.ScrolledWindow_EnableScrolling(*args, **kwargs)

    def GetViewStart(*args, **kwargs):
        """
        GetViewStart() -> (x,y)

        Get the view start
        """
        return _windows_.ScrolledWindow_GetViewStart(*args, **kwargs)

    def SetScale(*args, **kwargs):
        """SetScale(self, double xs, double ys)"""
        return _windows_.ScrolledWindow_SetScale(*args, **kwargs)

    def GetScaleX(*args, **kwargs):
        """GetScaleX(self) -> double"""
        return _windows_.ScrolledWindow_GetScaleX(*args, **kwargs)

    def GetScaleY(*args, **kwargs):
        """GetScaleY(self) -> double"""
        return _windows_.ScrolledWindow_GetScaleY(*args, **kwargs)

    def CalcScrolledPosition(*args):
        """
        CalcScrolledPosition(self, Point pt) -> Point
        CalcScrolledPosition(int x, int y) -> (sx, sy)

        Translate between scrolled and unscrolled coordinates.
        """
        return _windows_.ScrolledWindow_CalcScrolledPosition(*args)

    def CalcUnscrolledPosition(*args):
        """
        CalcUnscrolledPosition(self, Point pt) -> Point
        CalcUnscrolledPosition(int x, int y) -> (ux, uy)

        Translate between scrolled and unscrolled coordinates.
        """
        return _windows_.ScrolledWindow_CalcUnscrolledPosition(*args)

    def AdjustScrollbars(*args, **kwargs):
        """AdjustScrollbars(self)"""
        return _windows_.ScrolledWindow_AdjustScrollbars(*args, **kwargs)

    def CalcScrollInc(*args, **kwargs):
        """CalcScrollInc(self, ScrollWinEvent event) -> int"""
        return _windows_.ScrolledWindow_CalcScrollInc(*args, **kwargs)

    def SetTargetWindow(*args, **kwargs):
        """SetTargetWindow(self, Window target)"""
        return _windows_.ScrolledWindow_SetTargetWindow(*args, **kwargs)

    def GetTargetWindow(*args, **kwargs):
        """GetTargetWindow(self) -> Window"""
        return _windows_.ScrolledWindow_GetTargetWindow(*args, **kwargs)

    def SetTargetRect(*args, **kwargs):
        """SetTargetRect(self, Rect rect)"""
        return _windows_.ScrolledWindow_SetTargetRect(*args, **kwargs)

    def GetTargetRect(*args, **kwargs):
        """GetTargetRect(self) -> Rect"""
        return _windows_.ScrolledWindow_GetTargetRect(*args, **kwargs)

    def DoPrepareDC(*args, **kwargs):
        """
        DoPrepareDC(self, DC dc)

        Normally what is called by `PrepareDC`.
        """
        return _windows_.ScrolledWindow_DoPrepareDC(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.ScrolledWindow_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    ScaleX = property(GetScaleX,doc="See `GetScaleX`") 
    ScaleY = property(GetScaleY,doc="See `GetScaleY`") 
    TargetWindow = property(GetTargetWindow,SetTargetWindow,doc="See `GetTargetWindow` and `SetTargetWindow`") 
    ViewStart = property(GetViewStart,doc="See `GetViewStart`") 
_windows_.ScrolledWindow_swigregister(ScrolledWindow)

def PreScrolledWindow(*args, **kwargs):
    """PreScrolledWindow() -> ScrolledWindow"""
    val = _windows_.new_PreScrolledWindow(*args, **kwargs)
    return val

def ScrolledWindow_GetClassDefaultAttributes(*args, **kwargs):
  """
    ScrolledWindow_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.ScrolledWindow_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

STAY_ON_TOP = _windows_.STAY_ON_TOP
ICONIZE = _windows_.ICONIZE
MINIMIZE = _windows_.MINIMIZE
MAXIMIZE = _windows_.MAXIMIZE
CLOSE_BOX = _windows_.CLOSE_BOX
THICK_FRAME = _windows_.THICK_FRAME
SYSTEM_MENU = _windows_.SYSTEM_MENU
MINIMIZE_BOX = _windows_.MINIMIZE_BOX
MAXIMIZE_BOX = _windows_.MAXIMIZE_BOX
TINY_CAPTION_HORIZ = _windows_.TINY_CAPTION_HORIZ
TINY_CAPTION_VERT = _windows_.TINY_CAPTION_VERT
RESIZE_BOX = _windows_.RESIZE_BOX
RESIZE_BORDER = _windows_.RESIZE_BORDER
DIALOG_NO_PARENT = _windows_.DIALOG_NO_PARENT
DEFAULT_FRAME_STYLE = _windows_.DEFAULT_FRAME_STYLE
DEFAULT_DIALOG_STYLE = _windows_.DEFAULT_DIALOG_STYLE
FRAME_TOOL_WINDOW = _windows_.FRAME_TOOL_WINDOW
FRAME_FLOAT_ON_PARENT = _windows_.FRAME_FLOAT_ON_PARENT
FRAME_NO_WINDOW_MENU = _windows_.FRAME_NO_WINDOW_MENU
FRAME_NO_TASKBAR = _windows_.FRAME_NO_TASKBAR
FRAME_SHAPED = _windows_.FRAME_SHAPED
FRAME_DRAWER = _windows_.FRAME_DRAWER
FRAME_EX_METAL = _windows_.FRAME_EX_METAL
DIALOG_EX_METAL = _windows_.DIALOG_EX_METAL
WS_EX_CONTEXTHELP = _windows_.WS_EX_CONTEXTHELP
DIALOG_MODAL = _windows_.DIALOG_MODAL
DIALOG_MODELESS = _windows_.DIALOG_MODELESS
USER_COLOURS = _windows_.USER_COLOURS
NO_3D = _windows_.NO_3D
FRAME_EX_CONTEXTHELP = _windows_.FRAME_EX_CONTEXTHELP
DIALOG_EX_CONTEXTHELP = _windows_.DIALOG_EX_CONTEXTHELP
FULLSCREEN_NOMENUBAR = _windows_.FULLSCREEN_NOMENUBAR
FULLSCREEN_NOTOOLBAR = _windows_.FULLSCREEN_NOTOOLBAR
FULLSCREEN_NOSTATUSBAR = _windows_.FULLSCREEN_NOSTATUSBAR
FULLSCREEN_NOBORDER = _windows_.FULLSCREEN_NOBORDER
FULLSCREEN_NOCAPTION = _windows_.FULLSCREEN_NOCAPTION
FULLSCREEN_ALL = _windows_.FULLSCREEN_ALL
TOPLEVEL_EX_DIALOG = _windows_.TOPLEVEL_EX_DIALOG
USER_ATTENTION_INFO = _windows_.USER_ATTENTION_INFO
USER_ATTENTION_ERROR = _windows_.USER_ATTENTION_ERROR
class TopLevelWindow(_core.Window):
    """Proxy of C++ TopLevelWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def Maximize(*args, **kwargs):
        """Maximize(self, bool maximize=True)"""
        return _windows_.TopLevelWindow_Maximize(*args, **kwargs)

    def Restore(*args, **kwargs):
        """Restore(self)"""
        return _windows_.TopLevelWindow_Restore(*args, **kwargs)

    def Iconize(*args, **kwargs):
        """Iconize(self, bool iconize=True)"""
        return _windows_.TopLevelWindow_Iconize(*args, **kwargs)

    def IsMaximized(*args, **kwargs):
        """IsMaximized(self) -> bool"""
        return _windows_.TopLevelWindow_IsMaximized(*args, **kwargs)

    def IsAlwaysMaximized(*args, **kwargs):
        """IsAlwaysMaximized(self) -> bool"""
        return _windows_.TopLevelWindow_IsAlwaysMaximized(*args, **kwargs)

    def IsIconized(*args, **kwargs):
        """IsIconized(self) -> bool"""
        return _windows_.TopLevelWindow_IsIconized(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(self) -> Icon"""
        return _windows_.TopLevelWindow_GetIcon(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(self, Icon icon)"""
        return _windows_.TopLevelWindow_SetIcon(*args, **kwargs)

    def SetIcons(*args, **kwargs):
        """SetIcons(self, wxIconBundle icons)"""
        return _windows_.TopLevelWindow_SetIcons(*args, **kwargs)

    def ShowFullScreen(*args, **kwargs):
        """ShowFullScreen(self, bool show, long style=FULLSCREEN_ALL) -> bool"""
        return _windows_.TopLevelWindow_ShowFullScreen(*args, **kwargs)

    def IsFullScreen(*args, **kwargs):
        """IsFullScreen(self) -> bool"""
        return _windows_.TopLevelWindow_IsFullScreen(*args, **kwargs)

    def SetTitle(*args, **kwargs):
        """SetTitle(self, String title)"""
        return _windows_.TopLevelWindow_SetTitle(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle(self) -> String"""
        return _windows_.TopLevelWindow_GetTitle(*args, **kwargs)

    def EnableCloseButton(*args, **kwargs):
        """EnableCloseButton(self, bool enable) -> bool"""
        return _windows_.TopLevelWindow_EnableCloseButton(*args, **kwargs)

    def SetShape(*args, **kwargs):
        """SetShape(self, Region region) -> bool"""
        return _windows_.TopLevelWindow_SetShape(*args, **kwargs)

    def RequestUserAttention(*args, **kwargs):
        """RequestUserAttention(self, int flags=USER_ATTENTION_INFO)"""
        return _windows_.TopLevelWindow_RequestUserAttention(*args, **kwargs)

    def IsActive(*args, **kwargs):
        """IsActive(self) -> bool"""
        return _windows_.TopLevelWindow_IsActive(*args, **kwargs)

    def MacSetMetalAppearance(*args, **kwargs):
        """MacSetMetalAppearance(self, bool on)"""
        return _windows_.TopLevelWindow_MacSetMetalAppearance(*args, **kwargs)

    def MacGetMetalAppearance(*args, **kwargs):
        """MacGetMetalAppearance(self) -> bool"""
        return _windows_.TopLevelWindow_MacGetMetalAppearance(*args, **kwargs)

    def CenterOnScreen(*args, **kwargs):
        """
        CenterOnScreen(self, int dir=BOTH)

        Center the window on screen
        """
        return _windows_.TopLevelWindow_CenterOnScreen(*args, **kwargs)

    CentreOnScreen = CenterOnScreen 
    def GetDefaultItem(*args, **kwargs):
        """
        GetDefaultItem(self) -> Window

        Get the default child of this parent, i.e. the one which is activated
        by pressing <Enter> such as the OK button on a wx.Dialog.
        """
        return _windows_.TopLevelWindow_GetDefaultItem(*args, **kwargs)

    def SetDefaultItem(*args, **kwargs):
        """
        SetDefaultItem(self, Window child) -> Window

        Set this child as default, return the old default.
        """
        return _windows_.TopLevelWindow_SetDefaultItem(*args, **kwargs)

    def SetTmpDefaultItem(*args, **kwargs):
        """
        SetTmpDefaultItem(self, Window win)

        Set this child as temporary default
        """
        return _windows_.TopLevelWindow_SetTmpDefaultItem(*args, **kwargs)

    def GetTmpDefaultItem(*args, **kwargs):
        """
        GetTmpDefaultItem(self) -> Window

        Return the temporary default item, which can be None.
        """
        return _windows_.TopLevelWindow_GetTmpDefaultItem(*args, **kwargs)

    DefaultItem = property(GetDefaultItem,SetDefaultItem,doc="See `GetDefaultItem` and `SetDefaultItem`") 
    Icon = property(GetIcon,SetIcon,doc="See `GetIcon` and `SetIcon`") 
    Title = property(GetTitle,SetTitle,doc="See `GetTitle` and `SetTitle`") 
    TmpDefaultItem = property(GetTmpDefaultItem,SetTmpDefaultItem,doc="See `GetTmpDefaultItem` and `SetTmpDefaultItem`") 
_windows_.TopLevelWindow_swigregister(TopLevelWindow)
cvar = _windows_.cvar
FrameNameStr = cvar.FrameNameStr
DialogNameStr = cvar.DialogNameStr
StatusLineNameStr = cvar.StatusLineNameStr
ToolBarNameStr = cvar.ToolBarNameStr

#---------------------------------------------------------------------------

class Frame(TopLevelWindow):
    """Proxy of C++ Frame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, String name=FrameNameStr) -> Frame
        """
        _windows_.Frame_swiginit(self,_windows_.new_Frame(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, String name=FrameNameStr) -> bool
        """
        return _windows_.Frame_Create(*args, **kwargs)

    def SendSizeEvent(*args, **kwargs):
        """SendSizeEvent(self)"""
        return _windows_.Frame_SendSizeEvent(*args, **kwargs)

    def SetMenuBar(*args, **kwargs):
        """SetMenuBar(self, MenuBar menubar)"""
        return _windows_.Frame_SetMenuBar(*args, **kwargs)

    def GetMenuBar(*args, **kwargs):
        """GetMenuBar(self) -> MenuBar"""
        return _windows_.Frame_GetMenuBar(*args, **kwargs)

    def ProcessCommand(*args, **kwargs):
        """ProcessCommand(self, int winid) -> bool"""
        return _windows_.Frame_ProcessCommand(*args, **kwargs)

    Command = ProcessCommand 
    def CreateStatusBar(*args, **kwargs):
        """
        CreateStatusBar(self, int number=1, long style=DEFAULT_STATUSBAR_STYLE, int winid=0, 
            String name=StatusLineNameStr) -> StatusBar
        """
        return _windows_.Frame_CreateStatusBar(*args, **kwargs)

    def GetStatusBar(*args, **kwargs):
        """GetStatusBar(self) -> StatusBar"""
        return _windows_.Frame_GetStatusBar(*args, **kwargs)

    def SetStatusBar(*args, **kwargs):
        """SetStatusBar(self, StatusBar statBar)"""
        return _windows_.Frame_SetStatusBar(*args, **kwargs)

    def SetStatusText(*args, **kwargs):
        """SetStatusText(self, String text, int number=0)"""
        return _windows_.Frame_SetStatusText(*args, **kwargs)

    def SetStatusWidths(*args, **kwargs):
        """SetStatusWidths(self, int widths)"""
        return _windows_.Frame_SetStatusWidths(*args, **kwargs)

    def PushStatusText(*args, **kwargs):
        """PushStatusText(self, String text, int number=0)"""
        return _windows_.Frame_PushStatusText(*args, **kwargs)

    def PopStatusText(*args, **kwargs):
        """PopStatusText(self, int number=0)"""
        return _windows_.Frame_PopStatusText(*args, **kwargs)

    def SetStatusBarPane(*args, **kwargs):
        """SetStatusBarPane(self, int n)"""
        return _windows_.Frame_SetStatusBarPane(*args, **kwargs)

    def GetStatusBarPane(*args, **kwargs):
        """GetStatusBarPane(self) -> int"""
        return _windows_.Frame_GetStatusBarPane(*args, **kwargs)

    def CreateToolBar(*args, **kwargs):
        """CreateToolBar(self, long style=-1, int winid=-1, String name=ToolBarNameStr) -> wxToolBar"""
        return _windows_.Frame_CreateToolBar(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar(self) -> wxToolBar"""
        return _windows_.Frame_GetToolBar(*args, **kwargs)

    def SetToolBar(*args, **kwargs):
        """SetToolBar(self, wxToolBar toolbar)"""
        return _windows_.Frame_SetToolBar(*args, **kwargs)

    def DoGiveHelp(*args, **kwargs):
        """DoGiveHelp(self, String text, bool show)"""
        return _windows_.Frame_DoGiveHelp(*args, **kwargs)

    def DoMenuUpdates(*args, **kwargs):
        """DoMenuUpdates(self, Menu menu=None)"""
        return _windows_.Frame_DoMenuUpdates(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.Frame_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    MenuBar = property(GetMenuBar,SetMenuBar,doc="See `GetMenuBar` and `SetMenuBar`") 
    StatusBar = property(GetStatusBar,SetStatusBar,doc="See `GetStatusBar` and `SetStatusBar`") 
    StatusBarPane = property(GetStatusBarPane,SetStatusBarPane,doc="See `GetStatusBarPane` and `SetStatusBarPane`") 
    ToolBar = property(GetToolBar,SetToolBar,doc="See `GetToolBar` and `SetToolBar`") 
_windows_.Frame_swigregister(Frame)

def PreFrame(*args, **kwargs):
    """PreFrame() -> Frame"""
    val = _windows_.new_PreFrame(*args, **kwargs)
    return val

def Frame_GetClassDefaultAttributes(*args, **kwargs):
  """
    Frame_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.Frame_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class Dialog(TopLevelWindow):
    """Proxy of C++ Dialog class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    ButtonSizerFlags = _windows_.Dialog_ButtonSizerFlags
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_DIALOG_STYLE, String name=DialogNameStr) -> Dialog
        """
        _windows_.Dialog_swiginit(self,_windows_.new_Dialog(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_DIALOG_STYLE, String name=DialogNameStr) -> bool
        """
        return _windows_.Dialog_Create(*args, **kwargs)

    def SetReturnCode(*args, **kwargs):
        """SetReturnCode(self, int returnCode)"""
        return _windows_.Dialog_SetReturnCode(*args, **kwargs)

    def GetReturnCode(*args, **kwargs):
        """GetReturnCode(self) -> int"""
        return _windows_.Dialog_GetReturnCode(*args, **kwargs)

    def SetAffirmativeId(*args, **kwargs):
        """SetAffirmativeId(self, int affirmativeId)"""
        return _windows_.Dialog_SetAffirmativeId(*args, **kwargs)

    def GetAffirmativeId(*args, **kwargs):
        """GetAffirmativeId(self) -> int"""
        return _windows_.Dialog_GetAffirmativeId(*args, **kwargs)

    def SetEscapeId(*args, **kwargs):
        """SetEscapeId(self, int escapeId)"""
        return _windows_.Dialog_SetEscapeId(*args, **kwargs)

    def GetEscapeId(*args, **kwargs):
        """GetEscapeId(self) -> int"""
        return _windows_.Dialog_GetEscapeId(*args, **kwargs)

    def CreateTextSizer(*args, **kwargs):
        """CreateTextSizer(self, String message) -> Sizer"""
        return _windows_.Dialog_CreateTextSizer(*args, **kwargs)

    def _CreateButtonSizer(*args, **kwargs):
        """_CreateButtonSizer(self, long flags) -> Sizer"""
        return _windows_.Dialog__CreateButtonSizer(*args, **kwargs)

    def CreateButtonSizer(self, flags, *ignored):
        return self._CreateButtonSizer(flags)

    def CreateSeparatedButtonSizer(*args, **kwargs):
        """CreateSeparatedButtonSizer(self, long flags) -> Sizer"""
        return _windows_.Dialog_CreateSeparatedButtonSizer(*args, **kwargs)

    def CreateStdDialogButtonSizer(*args, **kwargs):
        """CreateStdDialogButtonSizer(self, long flags) -> StdDialogButtonSizer"""
        return _windows_.Dialog_CreateStdDialogButtonSizer(*args, **kwargs)

    def IsModal(*args, **kwargs):
        """IsModal(self) -> bool"""
        return _windows_.Dialog_IsModal(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal(self) -> int"""
        return _windows_.Dialog_ShowModal(*args, **kwargs)

    def EndModal(*args, **kwargs):
        """EndModal(self, int retCode)"""
        return _windows_.Dialog_EndModal(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.Dialog_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    AffirmativeId = property(GetAffirmativeId,SetAffirmativeId,doc="See `GetAffirmativeId` and `SetAffirmativeId`") 
    EscapeId = property(GetEscapeId,SetEscapeId,doc="See `GetEscapeId` and `SetEscapeId`") 
    ReturnCode = property(GetReturnCode,SetReturnCode,doc="See `GetReturnCode` and `SetReturnCode`") 
_windows_.Dialog_swigregister(Dialog)

def PreDialog(*args, **kwargs):
    """PreDialog() -> Dialog"""
    val = _windows_.new_PreDialog(*args, **kwargs)
    return val

def Dialog_GetClassDefaultAttributes(*args, **kwargs):
  """
    Dialog_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.Dialog_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

DEFAULT_MINIFRAME_STYLE = _windows_.DEFAULT_MINIFRAME_STYLE
class MiniFrame(Frame):
    """Proxy of C++ MiniFrame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_MINIFRAME_STYLE, String name=FrameNameStr) -> MiniFrame
        """
        _windows_.MiniFrame_swiginit(self,_windows_.new_MiniFrame(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_MINIFRAME_STYLE, String name=FrameNameStr) -> bool
        """
        return _windows_.MiniFrame_Create(*args, **kwargs)

_windows_.MiniFrame_swigregister(MiniFrame)

def PreMiniFrame(*args, **kwargs):
    """PreMiniFrame() -> MiniFrame"""
    val = _windows_.new_PreMiniFrame(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

SPLASH_CENTRE_ON_PARENT = _windows_.SPLASH_CENTRE_ON_PARENT
SPLASH_CENTRE_ON_SCREEN = _windows_.SPLASH_CENTRE_ON_SCREEN
SPLASH_NO_CENTRE = _windows_.SPLASH_NO_CENTRE
SPLASH_TIMEOUT = _windows_.SPLASH_TIMEOUT
SPLASH_NO_TIMEOUT = _windows_.SPLASH_NO_TIMEOUT
class SplashScreenWindow(_core.Window):
    """Proxy of C++ SplashScreenWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Bitmap bitmap, Window parent, int id, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=NO_BORDER) -> SplashScreenWindow
        """
        _windows_.SplashScreenWindow_swiginit(self,_windows_.new_SplashScreenWindow(*args, **kwargs))
        self._setOORInfo(self)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(self, Bitmap bitmap)"""
        return _windows_.SplashScreenWindow_SetBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _windows_.SplashScreenWindow_GetBitmap(*args, **kwargs)

    Bitmap = property(GetBitmap,SetBitmap,doc="See `GetBitmap` and `SetBitmap`") 
_windows_.SplashScreenWindow_swigregister(SplashScreenWindow)

class SplashScreen(Frame):
    """Proxy of C++ SplashScreen class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Bitmap bitmap, long splashStyle, int milliseconds, 
            Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP) -> SplashScreen
        """
        _windows_.SplashScreen_swiginit(self,_windows_.new_SplashScreen(*args, **kwargs))
        self._setOORInfo(self)

    def GetSplashStyle(*args, **kwargs):
        """GetSplashStyle(self) -> long"""
        return _windows_.SplashScreen_GetSplashStyle(*args, **kwargs)

    def GetSplashWindow(*args, **kwargs):
        """GetSplashWindow(self) -> SplashScreenWindow"""
        return _windows_.SplashScreen_GetSplashWindow(*args, **kwargs)

    def GetTimeout(*args, **kwargs):
        """GetTimeout(self) -> int"""
        return _windows_.SplashScreen_GetTimeout(*args, **kwargs)

    SplashStyle = property(GetSplashStyle,doc="See `GetSplashStyle`") 
    SplashWindow = property(GetSplashWindow,doc="See `GetSplashWindow`") 
    Timeout = property(GetTimeout,doc="See `GetTimeout`") 
_windows_.SplashScreen_swigregister(SplashScreen)

#---------------------------------------------------------------------------

SB_NORMAL = _windows_.SB_NORMAL
SB_FLAT = _windows_.SB_FLAT
SB_RAISED = _windows_.SB_RAISED
class StatusBar(_core.Window):
    """Proxy of C++ StatusBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, long style=DEFAULT_STATUSBAR_STYLE, 
            String name=StatusLineNameStr) -> StatusBar
        """
        _windows_.StatusBar_swiginit(self,_windows_.new_StatusBar(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(self, Window parent, int id=-1, long style=ST_SIZEGRIP, String name=StatusLineNameStr) -> bool"""
        return _windows_.StatusBar_Create(*args, **kwargs)

    def SetFieldsCount(*args, **kwargs):
        """SetFieldsCount(self, int number=1)"""
        return _windows_.StatusBar_SetFieldsCount(*args, **kwargs)

    def GetFieldsCount(*args, **kwargs):
        """GetFieldsCount(self) -> int"""
        return _windows_.StatusBar_GetFieldsCount(*args, **kwargs)

    def SetStatusText(*args, **kwargs):
        """SetStatusText(self, String text, int number=0)"""
        return _windows_.StatusBar_SetStatusText(*args, **kwargs)

    def GetStatusText(*args, **kwargs):
        """GetStatusText(self, int number=0) -> String"""
        return _windows_.StatusBar_GetStatusText(*args, **kwargs)

    def PushStatusText(*args, **kwargs):
        """PushStatusText(self, String text, int number=0)"""
        return _windows_.StatusBar_PushStatusText(*args, **kwargs)

    def PopStatusText(*args, **kwargs):
        """PopStatusText(self, int number=0)"""
        return _windows_.StatusBar_PopStatusText(*args, **kwargs)

    def SetStatusWidths(*args, **kwargs):
        """SetStatusWidths(self, int widths)"""
        return _windows_.StatusBar_SetStatusWidths(*args, **kwargs)

    def SetStatusStyles(*args, **kwargs):
        """SetStatusStyles(self, int styles)"""
        return _windows_.StatusBar_SetStatusStyles(*args, **kwargs)

    def GetFieldRect(*args, **kwargs):
        """GetFieldRect(self, int i) -> Rect"""
        return _windows_.StatusBar_GetFieldRect(*args, **kwargs)

    def SetMinHeight(*args, **kwargs):
        """SetMinHeight(self, int height)"""
        return _windows_.StatusBar_SetMinHeight(*args, **kwargs)

    def GetBorderX(*args, **kwargs):
        """GetBorderX(self) -> int"""
        return _windows_.StatusBar_GetBorderX(*args, **kwargs)

    def GetBorderY(*args, **kwargs):
        """GetBorderY(self) -> int"""
        return _windows_.StatusBar_GetBorderY(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.StatusBar_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def GetFields(self):
        """Return a list of field values in the status bar. """
        return [self.GetStatusText(i) for i in range(self.GetFieldsCount())]
        
    def SetFields(self, items):
        """Set the values of the statusbar fields from a list of strings. """
        self.SetFieldsCount(len(items))
        for i in range(len(items)):
            self.SetStatusText(items[i], i)

    BorderX = property(GetBorderX,doc="See `GetBorderX`") 
    BorderY = property(GetBorderY,doc="See `GetBorderY`") 
    FieldRect = property(GetFieldRect,doc="See `GetFieldRect`") 
    Fields = property(GetFields,SetFields,doc="See `GetFields` and `SetFields`") 
    FieldsCount = property(GetFieldsCount,SetFieldsCount,doc="See `GetFieldsCount` and `SetFieldsCount`") 
    StatusText = property(GetStatusText,SetStatusText,doc="See `GetStatusText` and `SetStatusText`") 
_windows_.StatusBar_swigregister(StatusBar)

def PreStatusBar(*args, **kwargs):
    """PreStatusBar() -> StatusBar"""
    val = _windows_.new_PreStatusBar(*args, **kwargs)
    return val

def StatusBar_GetClassDefaultAttributes(*args, **kwargs):
  """
    StatusBar_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.StatusBar_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

SP_NOBORDER = _windows_.SP_NOBORDER
SP_NOSASH = _windows_.SP_NOSASH
SP_PERMIT_UNSPLIT = _windows_.SP_PERMIT_UNSPLIT
SP_LIVE_UPDATE = _windows_.SP_LIVE_UPDATE
SP_3DSASH = _windows_.SP_3DSASH
SP_3DBORDER = _windows_.SP_3DBORDER
SP_NO_XP_THEME = _windows_.SP_NO_XP_THEME
SP_BORDER = _windows_.SP_BORDER
SP_3D = _windows_.SP_3D
SPLIT_HORIZONTAL = _windows_.SPLIT_HORIZONTAL
SPLIT_VERTICAL = _windows_.SPLIT_VERTICAL
SPLIT_DRAG_NONE = _windows_.SPLIT_DRAG_NONE
SPLIT_DRAG_DRAGGING = _windows_.SPLIT_DRAG_DRAGGING
SPLIT_DRAG_LEFT_DOWN = _windows_.SPLIT_DRAG_LEFT_DOWN
class SplitterWindow(_core.Window):
    """
    wx.SplitterWindow manages up to two subwindows or panes, with an
    optional vertical or horizontal split which can be used with the mouse
    or programmatically.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SP_3D, String name=SplitterNameStr) -> SplitterWindow

        Constructor.  Creates and shows a SplitterWindow.
        """
        if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']
        _windows_.SplitterWindow_swiginit(self,_windows_.new_SplitterWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SP_3D, String name=SplitterNameStr) -> bool

        Create the GUI part of the SplitterWindow for the 2-phase create.
        """
        return _windows_.SplitterWindow_Create(*args, **kwargs)

    def GetWindow1(*args, **kwargs):
        """
        GetWindow1(self) -> Window

        Gets the only or left/top pane.
        """
        return _windows_.SplitterWindow_GetWindow1(*args, **kwargs)

    def GetWindow2(*args, **kwargs):
        """
        GetWindow2(self) -> Window

        Gets the right/bottom pane.
        """
        return _windows_.SplitterWindow_GetWindow2(*args, **kwargs)

    def SetSplitMode(*args, **kwargs):
        """
        SetSplitMode(self, int mode)

        Sets the split mode.  The mode can be wx.SPLIT_VERTICAL or
        wx.SPLIT_HORIZONTAL.  This only sets the internal variable; does not
        update the display.
        """
        return _windows_.SplitterWindow_SetSplitMode(*args, **kwargs)

    def GetSplitMode(*args, **kwargs):
        """
        GetSplitMode(self) -> int

        Gets the split mode
        """
        return _windows_.SplitterWindow_GetSplitMode(*args, **kwargs)

    def Initialize(*args, **kwargs):
        """
        Initialize(self, Window window)

        Initializes the splitter window to have one pane.  This should be
        called if you wish to initially view only a single pane in the
        splitter window.  The child window is shown if it is currently hidden.
        """
        return _windows_.SplitterWindow_Initialize(*args, **kwargs)

    def SplitVertically(*args, **kwargs):
        """
        SplitVertically(self, Window window1, Window window2, int sashPosition=0) -> bool

        Initializes the left and right panes of the splitter window.  The
        child windows are shown if they are currently hidden.
        """
        return _windows_.SplitterWindow_SplitVertically(*args, **kwargs)

    def SplitHorizontally(*args, **kwargs):
        """
        SplitHorizontally(self, Window window1, Window window2, int sashPosition=0) -> bool

        Initializes the top and bottom panes of the splitter window.  The
        child windows are shown if they are currently hidden.
        """
        return _windows_.SplitterWindow_SplitHorizontally(*args, **kwargs)

    def Unsplit(*args, **kwargs):
        """
        Unsplit(self, Window toRemove=None) -> bool

        Unsplits the window.  Pass the pane to remove, or None to remove the
        right or bottom pane.  Returns True if successful, False otherwise (the
        window was not split).

        This function will not actually delete the pane being
        removed; it sends EVT_SPLITTER_UNSPLIT which can be handled
        for the desired behaviour. By default, the pane being
        removed is only hidden.
        """
        return _windows_.SplitterWindow_Unsplit(*args, **kwargs)

    def ReplaceWindow(*args, **kwargs):
        """
        ReplaceWindow(self, Window winOld, Window winNew) -> bool

        This function replaces one of the windows managed by the
        SplitterWindow with another one. It is in general better to use it
        instead of calling Unsplit() and then resplitting the window back
        because it will provoke much less flicker. It is valid to call this
        function whether the splitter has two windows or only one.

        Both parameters should be non-None and winOld must specify one of the
        windows managed by the splitter. If the parameters are incorrect or
        the window couldn't be replaced, False is returned. Otherwise the
        function will return True, but please notice that it will not Destroy
        the replaced window and you may wish to do it yourself.
        """
        return _windows_.SplitterWindow_ReplaceWindow(*args, **kwargs)

    def UpdateSize(*args, **kwargs):
        """
        UpdateSize(self)

        Causes any pending sizing of the sash and child panes to take place
        immediately.

        Such resizing normally takes place in idle time, in order to wait for
        layout to be completed. However, this can cause unacceptable flicker
        as the panes are resized after the window has been shown. To work
        around this, you can perform window layout (for example by sending a
        size event to the parent window), and then call this function, before
        showing the top-level window.
        """
        return _windows_.SplitterWindow_UpdateSize(*args, **kwargs)

    def IsSplit(*args, **kwargs):
        """
        IsSplit(self) -> bool

        Is the window split?
        """
        return _windows_.SplitterWindow_IsSplit(*args, **kwargs)

    def SetSashSize(*args, **kwargs):
        """
        SetSashSize(self, int width)

        Sets the sash size.
        """
        return _windows_.SplitterWindow_SetSashSize(*args, **kwargs)

    def SetBorderSize(*args, **kwargs):
        """
        SetBorderSize(self, int width)

        Sets the border size. Currently a NOP.
        """
        return _windows_.SplitterWindow_SetBorderSize(*args, **kwargs)

    def GetSashSize(*args, **kwargs):
        """
        GetSashSize(self) -> int

        Gets the sash size
        """
        return _windows_.SplitterWindow_GetSashSize(*args, **kwargs)

    def GetBorderSize(*args, **kwargs):
        """
        GetBorderSize(self) -> int

        Gets the border size
        """
        return _windows_.SplitterWindow_GetBorderSize(*args, **kwargs)

    def SetSashPosition(*args, **kwargs):
        """
        SetSashPosition(self, int position, bool redraw=True)

        Sets the sash position, in pixels.  If redraw is Ttrue then the panes
        are resized and the sash and border are redrawn.
        """
        return _windows_.SplitterWindow_SetSashPosition(*args, **kwargs)

    def GetSashPosition(*args, **kwargs):
        """
        GetSashPosition(self) -> int

        Returns the surrent sash position.
        """
        return _windows_.SplitterWindow_GetSashPosition(*args, **kwargs)

    def SetSashGravity(*args, **kwargs):
        """
        SetSashGravity(self, double gravity)

        Set the sash gravity.  Gravity is a floating-point factor between 0.0
        and 1.0 which controls position of sash while resizing the
        `wx.SplitterWindow`.  The gravity specifies how much the left/top
        window will grow while resizing.
        """
        return _windows_.SplitterWindow_SetSashGravity(*args, **kwargs)

    def GetSashGravity(*args, **kwargs):
        """
        GetSashGravity(self) -> double

        Gets the sash gravity.

        :see: `SetSashGravity`

        """
        return _windows_.SplitterWindow_GetSashGravity(*args, **kwargs)

    def SetMinimumPaneSize(*args, **kwargs):
        """
        SetMinimumPaneSize(self, int min)

        Sets the minimum pane size in pixels.

        The default minimum pane size is zero, which means that either pane
        can be reduced to zero by dragging the sash, thus removing one of the
        panes. To prevent this behaviour (and veto out-of-range sash
        dragging), set a minimum size, for example 20 pixels. If the
        wx.SP_PERMIT_UNSPLIT style is used when a splitter window is created,
        the window may be unsplit even if minimum size is non-zero.
        """
        return _windows_.SplitterWindow_SetMinimumPaneSize(*args, **kwargs)

    def GetMinimumPaneSize(*args, **kwargs):
        """
        GetMinimumPaneSize(self) -> int

        Gets the minimum pane size in pixels.
        """
        return _windows_.SplitterWindow_GetMinimumPaneSize(*args, **kwargs)

    def SashHitTest(*args, **kwargs):
        """
        SashHitTest(self, int x, int y, int tolerance=5) -> bool

        Tests for x, y over the sash
        """
        return _windows_.SplitterWindow_SashHitTest(*args, **kwargs)

    def SizeWindows(*args, **kwargs):
        """
        SizeWindows(self)

        Resizes subwindows
        """
        return _windows_.SplitterWindow_SizeWindows(*args, **kwargs)

    def SetNeedUpdating(*args, **kwargs):
        """SetNeedUpdating(self, bool needUpdating)"""
        return _windows_.SplitterWindow_SetNeedUpdating(*args, **kwargs)

    def GetNeedUpdating(*args, **kwargs):
        """GetNeedUpdating(self) -> bool"""
        return _windows_.SplitterWindow_GetNeedUpdating(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _windows_.SplitterWindow_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    BorderSize = property(GetBorderSize,SetBorderSize,doc="See `GetBorderSize` and `SetBorderSize`") 
    MinimumPaneSize = property(GetMinimumPaneSize,SetMinimumPaneSize,doc="See `GetMinimumPaneSize` and `SetMinimumPaneSize`") 
    NeedUpdating = property(GetNeedUpdating,SetNeedUpdating,doc="See `GetNeedUpdating` and `SetNeedUpdating`") 
    SashGravity = property(GetSashGravity,SetSashGravity,doc="See `GetSashGravity` and `SetSashGravity`") 
    SashPosition = property(GetSashPosition,SetSashPosition,doc="See `GetSashPosition` and `SetSashPosition`") 
    SashSize = property(GetSashSize,SetSashSize,doc="See `GetSashSize` and `SetSashSize`") 
    SplitMode = property(GetSplitMode,SetSplitMode,doc="See `GetSplitMode` and `SetSplitMode`") 
    Window1 = property(GetWindow1,doc="See `GetWindow1`") 
    Window2 = property(GetWindow2,doc="See `GetWindow2`") 
_windows_.SplitterWindow_swigregister(SplitterWindow)
SplitterNameStr = cvar.SplitterNameStr

def PreSplitterWindow(*args, **kwargs):
    """
    PreSplitterWindow() -> SplitterWindow

    Precreate a SplitterWindow for 2-phase creation.
    """
    val = _windows_.new_PreSplitterWindow(*args, **kwargs)
    return val

def SplitterWindow_GetClassDefaultAttributes(*args, **kwargs):
  """
    SplitterWindow_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
  return _windows_.SplitterWindow_GetClassDefaultAttributes(*args, **kwargs)

class SplitterEvent(_core.NotifyEvent):
    """This class represents the events generated by a splitter control."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, SplitterWindow splitter=(wxSplitterWindow *) NULL) -> SplitterEvent

        This class represents the events generated by a splitter control.
        """
        _windows_.SplitterEvent_swiginit(self,_windows_.new_SplitterEvent(*args, **kwargs))
    def SetSashPosition(*args, **kwargs):
        """
        SetSashPosition(self, int pos)

        This function is only meaningful during EVT_SPLITTER_SASH_POS_CHANGING
        and EVT_SPLITTER_SASH_POS_CHANGED events.  In the case of _CHANGED
        events, sets the the new sash position. In the case of _CHANGING
        events, sets the new tracking bar position so visual feedback during
        dragging will represent that change that will actually take place. Set
        to -1 from the event handler code to prevent repositioning.
        """
        return _windows_.SplitterEvent_SetSashPosition(*args, **kwargs)

    def GetSashPosition(*args, **kwargs):
        """
        GetSashPosition(self) -> int

        Returns the new sash position while in EVT_SPLITTER_SASH_POS_CHANGING
        and EVT_SPLITTER_SASH_POS_CHANGED events.
        """
        return _windows_.SplitterEvent_GetSashPosition(*args, **kwargs)

    def GetWindowBeingRemoved(*args, **kwargs):
        """
        GetWindowBeingRemoved(self) -> Window

        Returns a pointer to the window being removed when a splitter window
        is unsplit.
        """
        return _windows_.SplitterEvent_GetWindowBeingRemoved(*args, **kwargs)

    def GetX(*args, **kwargs):
        """
        GetX(self) -> int

        Returns the x coordinate of the double-click point in a
        EVT_SPLITTER_DCLICK event.
        """
        return _windows_.SplitterEvent_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """
        GetY(self) -> int

        Returns the y coordinate of the double-click point in a
        EVT_SPLITTER_DCLICK event.
        """
        return _windows_.SplitterEvent_GetY(*args, **kwargs)

    SashPosition = property(GetSashPosition,SetSashPosition,doc="See `GetSashPosition` and `SetSashPosition`") 
    WindowBeingRemoved = property(GetWindowBeingRemoved,doc="See `GetWindowBeingRemoved`") 
    X = property(GetX,doc="See `GetX`") 
    Y = property(GetY,doc="See `GetY`") 
_windows_.SplitterEvent_swigregister(SplitterEvent)

wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED = _windows_.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING = _windows_.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING
wxEVT_COMMAND_SPLITTER_DOUBLECLICKED = _windows_.wxEVT_COMMAND_SPLITTER_DOUBLECLICKED
wxEVT_COMMAND_SPLITTER_UNSPLIT = _windows_.wxEVT_COMMAND_SPLITTER_UNSPLIT
EVT_SPLITTER_SASH_POS_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, 1 )
EVT_SPLITTER_SASH_POS_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING, 1 )
EVT_SPLITTER_DOUBLECLICKED = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_DOUBLECLICKED, 1 )
EVT_SPLITTER_UNSPLIT = wx.PyEventBinder( wxEVT_COMMAND_SPLITTER_UNSPLIT, 1 )
EVT_SPLITTER_DCLICK = EVT_SPLITTER_DOUBLECLICKED

#---------------------------------------------------------------------------

SASH_DRAG_NONE = _windows_.SASH_DRAG_NONE
SASH_DRAG_DRAGGING = _windows_.SASH_DRAG_DRAGGING
SASH_DRAG_LEFT_DOWN = _windows_.SASH_DRAG_LEFT_DOWN
SW_NOBORDER = _windows_.SW_NOBORDER
SW_BORDER = _windows_.SW_BORDER
SW_3DSASH = _windows_.SW_3DSASH
SW_3DBORDER = _windows_.SW_3DBORDER
SW_3D = _windows_.SW_3D
SASH_TOP = _windows_.SASH_TOP
SASH_RIGHT = _windows_.SASH_RIGHT
SASH_BOTTOM = _windows_.SASH_BOTTOM
SASH_LEFT = _windows_.SASH_LEFT
SASH_NONE = _windows_.SASH_NONE
class SashWindow(_core.Window):
    """Proxy of C++ SashWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxCLIP_CHILDREN|wxSW_3D, 
            String name=SashNameStr) -> SashWindow
        """
        _windows_.SashWindow_swiginit(self,_windows_.new_SashWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxCLIP_CHILDREN|wxSW_3D, 
            String name=SashNameStr) -> bool
        """
        return _windows_.SashWindow_Create(*args, **kwargs)

    def SetSashVisible(*args, **kwargs):
        """SetSashVisible(self, int edge, bool sash)"""
        return _windows_.SashWindow_SetSashVisible(*args, **kwargs)

    def GetSashVisible(*args, **kwargs):
        """GetSashVisible(self, int edge) -> bool"""
        return _windows_.SashWindow_GetSashVisible(*args, **kwargs)

    def SetSashBorder(*args, **kwargs):
        """SetSashBorder(self, int edge, bool border)"""
        return _windows_.SashWindow_SetSashBorder(*args, **kwargs)

    def HasBorder(*args, **kwargs):
        """HasBorder(self, int edge) -> bool"""
        return _windows_.SashWindow_HasBorder(*args, **kwargs)

    def GetEdgeMargin(*args, **kwargs):
        """GetEdgeMargin(self, int edge) -> int"""
        return _windows_.SashWindow_GetEdgeMargin(*args, **kwargs)

    def SetDefaultBorderSize(*args, **kwargs):
        """SetDefaultBorderSize(self, int width)"""
        return _windows_.SashWindow_SetDefaultBorderSize(*args, **kwargs)

    def GetDefaultBorderSize(*args, **kwargs):
        """GetDefaultBorderSize(self) -> int"""
        return _windows_.SashWindow_GetDefaultBorderSize(*args, **kwargs)

    def SetExtraBorderSize(*args, **kwargs):
        """SetExtraBorderSize(self, int width)"""
        return _windows_.SashWindow_SetExtraBorderSize(*args, **kwargs)

    def GetExtraBorderSize(*args, **kwargs):
        """GetExtraBorderSize(self) -> int"""
        return _windows_.SashWindow_GetExtraBorderSize(*args, **kwargs)

    def SetMinimumSizeX(*args, **kwargs):
        """SetMinimumSizeX(self, int min)"""
        return _windows_.SashWindow_SetMinimumSizeX(*args, **kwargs)

    def SetMinimumSizeY(*args, **kwargs):
        """SetMinimumSizeY(self, int min)"""
        return _windows_.SashWindow_SetMinimumSizeY(*args, **kwargs)

    def GetMinimumSizeX(*args, **kwargs):
        """GetMinimumSizeX(self) -> int"""
        return _windows_.SashWindow_GetMinimumSizeX(*args, **kwargs)

    def GetMinimumSizeY(*args, **kwargs):
        """GetMinimumSizeY(self) -> int"""
        return _windows_.SashWindow_GetMinimumSizeY(*args, **kwargs)

    def SetMaximumSizeX(*args, **kwargs):
        """SetMaximumSizeX(self, int max)"""
        return _windows_.SashWindow_SetMaximumSizeX(*args, **kwargs)

    def SetMaximumSizeY(*args, **kwargs):
        """SetMaximumSizeY(self, int max)"""
        return _windows_.SashWindow_SetMaximumSizeY(*args, **kwargs)

    def GetMaximumSizeX(*args, **kwargs):
        """GetMaximumSizeX(self) -> int"""
        return _windows_.SashWindow_GetMaximumSizeX(*args, **kwargs)

    def GetMaximumSizeY(*args, **kwargs):
        """GetMaximumSizeY(self) -> int"""
        return _windows_.SashWindow_GetMaximumSizeY(*args, **kwargs)

    def SashHitTest(*args, **kwargs):
        """SashHitTest(self, int x, int y, int tolerance=2) -> int"""
        return _windows_.SashWindow_SashHitTest(*args, **kwargs)

    def SizeWindows(*args, **kwargs):
        """SizeWindows(self)"""
        return _windows_.SashWindow_SizeWindows(*args, **kwargs)

    DefaultBorderSize = property(GetDefaultBorderSize,SetDefaultBorderSize,doc="See `GetDefaultBorderSize` and `SetDefaultBorderSize`") 
    ExtraBorderSize = property(GetExtraBorderSize,SetExtraBorderSize,doc="See `GetExtraBorderSize` and `SetExtraBorderSize`") 
    MaximumSizeX = property(GetMaximumSizeX,SetMaximumSizeX,doc="See `GetMaximumSizeX` and `SetMaximumSizeX`") 
    MaximumSizeY = property(GetMaximumSizeY,SetMaximumSizeY,doc="See `GetMaximumSizeY` and `SetMaximumSizeY`") 
    MinimumSizeX = property(GetMinimumSizeX,SetMinimumSizeX,doc="See `GetMinimumSizeX` and `SetMinimumSizeX`") 
    MinimumSizeY = property(GetMinimumSizeY,SetMinimumSizeY,doc="See `GetMinimumSizeY` and `SetMinimumSizeY`") 
_windows_.SashWindow_swigregister(SashWindow)
SashNameStr = cvar.SashNameStr
SashLayoutNameStr = cvar.SashLayoutNameStr

def PreSashWindow(*args, **kwargs):
    """PreSashWindow() -> SashWindow"""
    val = _windows_.new_PreSashWindow(*args, **kwargs)
    return val

SASH_STATUS_OK = _windows_.SASH_STATUS_OK
SASH_STATUS_OUT_OF_RANGE = _windows_.SASH_STATUS_OUT_OF_RANGE
class SashEvent(_core.CommandEvent):
    """Proxy of C++ SashEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int id=0, int edge=SASH_NONE) -> SashEvent"""
        _windows_.SashEvent_swiginit(self,_windows_.new_SashEvent(*args, **kwargs))
    def SetEdge(*args, **kwargs):
        """SetEdge(self, int edge)"""
        return _windows_.SashEvent_SetEdge(*args, **kwargs)

    def GetEdge(*args, **kwargs):
        """GetEdge(self) -> int"""
        return _windows_.SashEvent_GetEdge(*args, **kwargs)

    def SetDragRect(*args, **kwargs):
        """SetDragRect(self, Rect rect)"""
        return _windows_.SashEvent_SetDragRect(*args, **kwargs)

    def GetDragRect(*args, **kwargs):
        """GetDragRect(self) -> Rect"""
        return _windows_.SashEvent_GetDragRect(*args, **kwargs)

    def SetDragStatus(*args, **kwargs):
        """SetDragStatus(self, int status)"""
        return _windows_.SashEvent_SetDragStatus(*args, **kwargs)

    def GetDragStatus(*args, **kwargs):
        """GetDragStatus(self) -> int"""
        return _windows_.SashEvent_GetDragStatus(*args, **kwargs)

    DragRect = property(GetDragRect,SetDragRect,doc="See `GetDragRect` and `SetDragRect`") 
    DragStatus = property(GetDragStatus,SetDragStatus,doc="See `GetDragStatus` and `SetDragStatus`") 
    Edge = property(GetEdge,SetEdge,doc="See `GetEdge` and `SetEdge`") 
_windows_.SashEvent_swigregister(SashEvent)

wxEVT_SASH_DRAGGED = _windows_.wxEVT_SASH_DRAGGED
EVT_SASH_DRAGGED = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 1 )
EVT_SASH_DRAGGED_RANGE = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 2 )

#---------------------------------------------------------------------------

LAYOUT_HORIZONTAL = _windows_.LAYOUT_HORIZONTAL
LAYOUT_VERTICAL = _windows_.LAYOUT_VERTICAL
LAYOUT_NONE = _windows_.LAYOUT_NONE
LAYOUT_TOP = _windows_.LAYOUT_TOP
LAYOUT_LEFT = _windows_.LAYOUT_LEFT
LAYOUT_RIGHT = _windows_.LAYOUT_RIGHT
LAYOUT_BOTTOM = _windows_.LAYOUT_BOTTOM
LAYOUT_LENGTH_Y = _windows_.LAYOUT_LENGTH_Y
LAYOUT_LENGTH_X = _windows_.LAYOUT_LENGTH_X
LAYOUT_MRU_LENGTH = _windows_.LAYOUT_MRU_LENGTH
LAYOUT_QUERY = _windows_.LAYOUT_QUERY
wxEVT_QUERY_LAYOUT_INFO = _windows_.wxEVT_QUERY_LAYOUT_INFO
wxEVT_CALCULATE_LAYOUT = _windows_.wxEVT_CALCULATE_LAYOUT
class QueryLayoutInfoEvent(_core.Event):
    """Proxy of C++ QueryLayoutInfoEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int id=0) -> QueryLayoutInfoEvent"""
        _windows_.QueryLayoutInfoEvent_swiginit(self,_windows_.new_QueryLayoutInfoEvent(*args, **kwargs))
    def SetRequestedLength(*args, **kwargs):
        """SetRequestedLength(self, int length)"""
        return _windows_.QueryLayoutInfoEvent_SetRequestedLength(*args, **kwargs)

    def GetRequestedLength(*args, **kwargs):
        """GetRequestedLength(self) -> int"""
        return _windows_.QueryLayoutInfoEvent_GetRequestedLength(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _windows_.QueryLayoutInfoEvent_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> int"""
        return _windows_.QueryLayoutInfoEvent_GetFlags(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size size)"""
        return _windows_.QueryLayoutInfoEvent_SetSize(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _windows_.QueryLayoutInfoEvent_GetSize(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _windows_.QueryLayoutInfoEvent_SetOrientation(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return _windows_.QueryLayoutInfoEvent_GetOrientation(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(self, int align)"""
        return _windows_.QueryLayoutInfoEvent_SetAlignment(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment(self) -> int"""
        return _windows_.QueryLayoutInfoEvent_GetAlignment(*args, **kwargs)

    Alignment = property(GetAlignment,SetAlignment,doc="See `GetAlignment` and `SetAlignment`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
    RequestedLength = property(GetRequestedLength,SetRequestedLength,doc="See `GetRequestedLength` and `SetRequestedLength`") 
    Size = property(GetSize,SetSize,doc="See `GetSize` and `SetSize`") 
_windows_.QueryLayoutInfoEvent_swigregister(QueryLayoutInfoEvent)

class CalculateLayoutEvent(_core.Event):
    """Proxy of C++ CalculateLayoutEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int id=0) -> CalculateLayoutEvent"""
        _windows_.CalculateLayoutEvent_swiginit(self,_windows_.new_CalculateLayoutEvent(*args, **kwargs))
    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _windows_.CalculateLayoutEvent_SetFlags(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> int"""
        return _windows_.CalculateLayoutEvent_GetFlags(*args, **kwargs)

    def SetRect(*args, **kwargs):
        """SetRect(self, Rect rect)"""
        return _windows_.CalculateLayoutEvent_SetRect(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _windows_.CalculateLayoutEvent_GetRect(*args, **kwargs)

    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Rect = property(GetRect,SetRect,doc="See `GetRect` and `SetRect`") 
_windows_.CalculateLayoutEvent_swigregister(CalculateLayoutEvent)

EVT_QUERY_LAYOUT_INFO = wx.PyEventBinder( wxEVT_QUERY_LAYOUT_INFO )
EVT_CALCULATE_LAYOUT = wx.PyEventBinder( wxEVT_CALCULATE_LAYOUT )

class SashLayoutWindow(SashWindow):
    """Proxy of C++ SashLayoutWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxCLIP_CHILDREN|wxSW_3D, 
            String name=SashLayoutNameStr) -> SashLayoutWindow
        """
        _windows_.SashLayoutWindow_swiginit(self,_windows_.new_SashLayoutWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxCLIP_CHILDREN|wxSW_3D, 
            String name=SashLayoutNameStr) -> bool
        """
        return _windows_.SashLayoutWindow_Create(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment(self) -> int"""
        return _windows_.SashLayoutWindow_GetAlignment(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return _windows_.SashLayoutWindow_GetOrientation(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(self, int alignment)"""
        return _windows_.SashLayoutWindow_SetAlignment(*args, **kwargs)

    def SetDefaultSize(*args, **kwargs):
        """SetDefaultSize(self, Size size)"""
        return _windows_.SashLayoutWindow_SetDefaultSize(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orientation)"""
        return _windows_.SashLayoutWindow_SetOrientation(*args, **kwargs)

    Alignment = property(GetAlignment,SetAlignment,doc="See `GetAlignment` and `SetAlignment`") 
    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
_windows_.SashLayoutWindow_swigregister(SashLayoutWindow)

def PreSashLayoutWindow(*args, **kwargs):
    """PreSashLayoutWindow() -> SashLayoutWindow"""
    val = _windows_.new_PreSashLayoutWindow(*args, **kwargs)
    return val

class LayoutAlgorithm(_core.Object):
    """Proxy of C++ LayoutAlgorithm class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> LayoutAlgorithm"""
        _windows_.LayoutAlgorithm_swiginit(self,_windows_.new_LayoutAlgorithm(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_LayoutAlgorithm
    __del__ = lambda self : None;
    def LayoutMDIFrame(*args, **kwargs):
        """LayoutMDIFrame(self, MDIParentFrame frame, Rect rect=None) -> bool"""
        return _windows_.LayoutAlgorithm_LayoutMDIFrame(*args, **kwargs)

    def LayoutFrame(*args, **kwargs):
        """LayoutFrame(self, Frame frame, Window mainWindow=None) -> bool"""
        return _windows_.LayoutAlgorithm_LayoutFrame(*args, **kwargs)

    def LayoutWindow(*args, **kwargs):
        """LayoutWindow(self, Window parent, Window mainWindow=None) -> bool"""
        return _windows_.LayoutAlgorithm_LayoutWindow(*args, **kwargs)

_windows_.LayoutAlgorithm_swigregister(LayoutAlgorithm)

#---------------------------------------------------------------------------

class PopupWindow(_core.Window):
    """Proxy of C++ PopupWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window parent, int flags=BORDER_NONE) -> PopupWindow"""
        _windows_.PopupWindow_swiginit(self,_windows_.new_PopupWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(self, Window parent, int flags=BORDER_NONE) -> bool"""
        return _windows_.PopupWindow_Create(*args, **kwargs)

    def Position(*args, **kwargs):
        """Position(self, Point ptOrigin, Size size)"""
        return _windows_.PopupWindow_Position(*args, **kwargs)

_windows_.PopupWindow_swigregister(PopupWindow)

def PrePopupWindow(*args, **kwargs):
    """PrePopupWindow() -> PopupWindow"""
    val = _windows_.new_PrePopupWindow(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class PopupTransientWindow(PopupWindow):
    """Proxy of C++ PopupTransientWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window parent, int style=BORDER_NONE) -> PopupTransientWindow"""
        _windows_.PopupTransientWindow_swiginit(self,_windows_.new_PopupTransientWindow(*args, **kwargs))
        self._setOORInfo(self);PopupTransientWindow._setCallbackInfo(self, self, PopupTransientWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PopupTransientWindow__setCallbackInfo(*args, **kwargs)

    def Popup(*args, **kwargs):
        """Popup(self, Window focus=None)"""
        return _windows_.PopupTransientWindow_Popup(*args, **kwargs)

    def Dismiss(*args, **kwargs):
        """Dismiss(self)"""
        return _windows_.PopupTransientWindow_Dismiss(*args, **kwargs)

_windows_.PopupTransientWindow_swigregister(PopupTransientWindow)

def PrePopupTransientWindow(*args, **kwargs):
    """PrePopupTransientWindow() -> PopupTransientWindow"""
    val = _windows_.new_PrePopupTransientWindow(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class TipWindow(PopupTransientWindow):
    """Proxy of C++ TipWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window parent, String text, int maxLength=100, Rect rectBound=None) -> TipWindow"""
        _windows_.TipWindow_swiginit(self,_windows_.new_TipWindow(*args, **kwargs))
        self._setOORInfo(self)

    def SetBoundingRect(*args, **kwargs):
        """SetBoundingRect(self, Rect rectBound)"""
        return _windows_.TipWindow_SetBoundingRect(*args, **kwargs)

    def Close(*args, **kwargs):
        """
        Close(self)

        This function simply generates a EVT_CLOSE event whose handler usually
        tries to close the window. It doesn't close the window itself,
        however.  If force is False (the default) then the window's close
        handler will be allowed to veto the destruction of the window.
        """
        return _windows_.TipWindow_Close(*args, **kwargs)

_windows_.TipWindow_swigregister(TipWindow)

#---------------------------------------------------------------------------

class VScrolledWindow(Panel):
    """Proxy of C++ VScrolledWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> VScrolledWindow
        """
        _windows_.VScrolledWindow_swiginit(self,_windows_.new_VScrolledWindow(*args, **kwargs))
        self._setOORInfo(self);VScrolledWindow._setCallbackInfo(self, self, VScrolledWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.VScrolledWindow__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> bool
        """
        return _windows_.VScrolledWindow_Create(*args, **kwargs)

    def SetLineCount(*args, **kwargs):
        """SetLineCount(self, size_t count)"""
        return _windows_.VScrolledWindow_SetLineCount(*args, **kwargs)

    def ScrollToLine(*args, **kwargs):
        """ScrollToLine(self, size_t line) -> bool"""
        return _windows_.VScrolledWindow_ScrollToLine(*args, **kwargs)

    def RefreshLine(*args, **kwargs):
        """RefreshLine(self, size_t line)"""
        return _windows_.VScrolledWindow_RefreshLine(*args, **kwargs)

    def RefreshLines(*args, **kwargs):
        """RefreshLines(self, size_t from, size_t to)"""
        return _windows_.VScrolledWindow_RefreshLines(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """
        HitTestXY(self, int x, int y) -> int

        Test where the given (in client coords) point lies
        """
        return _windows_.VScrolledWindow_HitTestXY(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, Point pt) -> int

        Test where the given (in client coords) point lies
        """
        return _windows_.VScrolledWindow_HitTest(*args, **kwargs)

    def RefreshAll(*args, **kwargs):
        """RefreshAll(self)"""
        return _windows_.VScrolledWindow_RefreshAll(*args, **kwargs)

    def GetLineCount(*args, **kwargs):
        """GetLineCount(self) -> size_t"""
        return _windows_.VScrolledWindow_GetLineCount(*args, **kwargs)

    def GetVisibleBegin(*args, **kwargs):
        """GetVisibleBegin(self) -> size_t"""
        return _windows_.VScrolledWindow_GetVisibleBegin(*args, **kwargs)

    def GetVisibleEnd(*args, **kwargs):
        """GetVisibleEnd(self) -> size_t"""
        return _windows_.VScrolledWindow_GetVisibleEnd(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(self, size_t line) -> bool"""
        return _windows_.VScrolledWindow_IsVisible(*args, **kwargs)

    def GetFirstVisibleLine(*args, **kwargs):
        """GetFirstVisibleLine(self) -> size_t"""
        return _windows_.VScrolledWindow_GetFirstVisibleLine(*args, **kwargs)

    def GetLastVisibleLine(*args, **kwargs):
        """GetLastVisibleLine(self) -> size_t"""
        return _windows_.VScrolledWindow_GetLastVisibleLine(*args, **kwargs)

    def FindFirstFromBottom(*args, **kwargs):
        """FindFirstFromBottom(self, size_t lineLast, bool fullyVisible=False) -> size_t"""
        return _windows_.VScrolledWindow_FindFirstFromBottom(*args, **kwargs)

    def GetLinesHeight(*args, **kwargs):
        """GetLinesHeight(self, size_t lineMin, size_t lineMax) -> int"""
        return _windows_.VScrolledWindow_GetLinesHeight(*args, **kwargs)

    FirstVisibleLine = property(GetFirstVisibleLine,doc="See `GetFirstVisibleLine`") 
    LastVisibleLine = property(GetLastVisibleLine,doc="See `GetLastVisibleLine`") 
    LineCount = property(GetLineCount,SetLineCount,doc="See `GetLineCount` and `SetLineCount`") 
    VisibleBegin = property(GetVisibleBegin,doc="See `GetVisibleBegin`") 
    VisibleEnd = property(GetVisibleEnd,doc="See `GetVisibleEnd`") 
_windows_.VScrolledWindow_swigregister(VScrolledWindow)

def PreVScrolledWindow(*args, **kwargs):
    """PreVScrolledWindow() -> VScrolledWindow"""
    val = _windows_.new_PreVScrolledWindow(*args, **kwargs)
    return val

class VListBox(VScrolledWindow):
    """Proxy of C++ VListBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=VListBoxNameStr) -> VListBox
        """
        _windows_.VListBox_swiginit(self,_windows_.new_VListBox(*args, **kwargs))
        self._setOORInfo(self);VListBox._setCallbackInfo(self, self, VListBox)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.VListBox__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=VListBoxNameStr) -> bool
        """
        return _windows_.VListBox_Create(*args, **kwargs)

    def GetItemCount(*args, **kwargs):
        """GetItemCount(self) -> size_t"""
        return _windows_.VListBox_GetItemCount(*args, **kwargs)

    def HasMultipleSelection(*args, **kwargs):
        """HasMultipleSelection(self) -> bool"""
        return _windows_.VListBox_HasMultipleSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _windows_.VListBox_GetSelection(*args, **kwargs)

    def IsCurrent(*args, **kwargs):
        """IsCurrent(self, size_t item) -> bool"""
        return _windows_.VListBox_IsCurrent(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(self, size_t item) -> bool"""
        return _windows_.VListBox_IsSelected(*args, **kwargs)

    def GetSelectedCount(*args, **kwargs):
        """GetSelectedCount(self) -> size_t"""
        return _windows_.VListBox_GetSelectedCount(*args, **kwargs)

    def GetFirstSelected(*args, **kwargs):
        """GetFirstSelected(self) -> PyObject"""
        return _windows_.VListBox_GetFirstSelected(*args, **kwargs)

    def GetNextSelected(*args, **kwargs):
        """GetNextSelected(self, unsigned long cookie) -> PyObject"""
        return _windows_.VListBox_GetNextSelected(*args, **kwargs)

    def GetMargins(*args, **kwargs):
        """GetMargins(self) -> Point"""
        return _windows_.VListBox_GetMargins(*args, **kwargs)

    def GetSelectionBackground(*args, **kwargs):
        """GetSelectionBackground(self) -> Colour"""
        return _windows_.VListBox_GetSelectionBackground(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(self, size_t count)"""
        return _windows_.VListBox_SetItemCount(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _windows_.VListBox_Clear(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int selection)"""
        return _windows_.VListBox_SetSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(self, size_t item, bool select=True) -> bool"""
        return _windows_.VListBox_Select(*args, **kwargs)

    def SelectRange(*args, **kwargs):
        """SelectRange(self, size_t from, size_t to) -> bool"""
        return _windows_.VListBox_SelectRange(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(self, size_t item)"""
        return _windows_.VListBox_Toggle(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll(self) -> bool"""
        return _windows_.VListBox_SelectAll(*args, **kwargs)

    def DeselectAll(*args, **kwargs):
        """DeselectAll(self) -> bool"""
        return _windows_.VListBox_DeselectAll(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(self, Point pt)"""
        return _windows_.VListBox_SetMargins(*args, **kwargs)

    def SetMarginsXY(*args, **kwargs):
        """SetMarginsXY(self, int x, int y)"""
        return _windows_.VListBox_SetMarginsXY(*args, **kwargs)

    def SetSelectionBackground(*args, **kwargs):
        """SetSelectionBackground(self, Colour col)"""
        return _windows_.VListBox_SetSelectionBackground(*args, **kwargs)

    def OnDrawSeparator(*args, **kwargs):
        """OnDrawSeparator(self, DC dc, Rect rect, size_t n)"""
        return _windows_.VListBox_OnDrawSeparator(*args, **kwargs)

    def OnDrawBackground(*args, **kwargs):
        """OnDrawBackground(self, DC dc, Rect rect, size_t n)"""
        return _windows_.VListBox_OnDrawBackground(*args, **kwargs)

    FirstSelected = property(GetFirstSelected,doc="See `GetFirstSelected`") 
    ItemCount = property(GetItemCount,SetItemCount,doc="See `GetItemCount` and `SetItemCount`") 
    Margins = property(GetMargins,SetMargins,doc="See `GetMargins` and `SetMargins`") 
    SelectedCount = property(GetSelectedCount,doc="See `GetSelectedCount`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    SelectionBackground = property(GetSelectionBackground,SetSelectionBackground,doc="See `GetSelectionBackground` and `SetSelectionBackground`") 
_windows_.VListBox_swigregister(VListBox)
VListBoxNameStr = cvar.VListBoxNameStr

def PreVListBox(*args, **kwargs):
    """PreVListBox() -> VListBox"""
    val = _windows_.new_PreVListBox(*args, **kwargs)
    return val

class HtmlListBox(VListBox):
    """Proxy of C++ HtmlListBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=VListBoxNameStr) -> HtmlListBox
        """
        _windows_.HtmlListBox_swiginit(self,_windows_.new_HtmlListBox(*args, **kwargs))
        self._setOORInfo(self);HtmlListBox._setCallbackInfo(self, self, HtmlListBox)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.HtmlListBox__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=ID_ANY, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=VListBoxNameStr) -> bool
        """
        return _windows_.HtmlListBox_Create(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(self, size_t count)"""
        return _windows_.HtmlListBox_SetItemCount(*args, **kwargs)

    def GetFileSystem(*args, **kwargs):
        """GetFileSystem(self) -> FileSystem"""
        return _windows_.HtmlListBox_GetFileSystem(*args, **kwargs)

    def OnLinkClicked(*args, **kwargs):
        """OnLinkClicked(self, size_t n, wxHtmlLinkInfo link)"""
        return _windows_.HtmlListBox_OnLinkClicked(*args, **kwargs)

    FileSystem = property(GetFileSystem,doc="See `GetFileSystem`") 
_windows_.HtmlListBox_swigregister(HtmlListBox)

def PreHtmlListBox(*args, **kwargs):
    """PreHtmlListBox() -> HtmlListBox"""
    val = _windows_.new_PreHtmlListBox(*args, **kwargs)
    return val

HLB_DEFAULT_STYLE = _windows_.HLB_DEFAULT_STYLE
HLB_MULTIPLE = _windows_.HLB_MULTIPLE
class SimpleHtmlListBox(HtmlListBox,_core.ItemContainer):
    """Proxy of C++ SimpleHtmlListBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=HLB_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=SimpleHtmlListBoxNameStr) -> SimpleHtmlListBox
        """
        _windows_.SimpleHtmlListBox_swiginit(self,_windows_.new_SimpleHtmlListBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=HLB_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=SimpleHtmlListBoxNameStr) -> bool
        """
        return _windows_.SimpleHtmlListBox_Create(*args, **kwargs)

_windows_.SimpleHtmlListBox_swigregister(SimpleHtmlListBox)
SimpleHtmlListBoxNameStr = cvar.SimpleHtmlListBoxNameStr

def PreSimpleHtmlListBox(*args, **kwargs):
    """PreSimpleHtmlListBox() -> SimpleHtmlListBox"""
    val = _windows_.new_PreSimpleHtmlListBox(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class TaskBarIcon(_core.EvtHandler):
    """Proxy of C++ TaskBarIcon class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> TaskBarIcon"""
        _windows_.TaskBarIcon_swiginit(self,_windows_.new_TaskBarIcon(*args, **kwargs))
        TaskBarIcon._setCallbackInfo(self, self, TaskBarIcon)

    __swig_destroy__ = _windows_.delete_TaskBarIcon
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref=0)"""
        return _windows_.TaskBarIcon__setCallbackInfo(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        args[0].this.own(False)
        return _windows_.TaskBarIcon_Destroy(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _windows_.TaskBarIcon_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def IsIconInstalled(*args, **kwargs):
        """IsIconInstalled(self) -> bool"""
        return _windows_.TaskBarIcon_IsIconInstalled(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(self, Icon icon, String tooltip=EmptyString) -> bool"""
        return _windows_.TaskBarIcon_SetIcon(*args, **kwargs)

    def RemoveIcon(*args, **kwargs):
        """RemoveIcon(self) -> bool"""
        return _windows_.TaskBarIcon_RemoveIcon(*args, **kwargs)

    def PopupMenu(*args, **kwargs):
        """PopupMenu(self, Menu menu) -> bool"""
        return _windows_.TaskBarIcon_PopupMenu(*args, **kwargs)

_windows_.TaskBarIcon_swigregister(TaskBarIcon)

class TaskBarIconEvent(_core.Event):
    """Proxy of C++ TaskBarIconEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType evtType, wxTaskBarIcon tbIcon) -> TaskBarIconEvent"""
        _windows_.TaskBarIconEvent_swiginit(self,_windows_.new_TaskBarIconEvent(*args, **kwargs))
_windows_.TaskBarIconEvent_swigregister(TaskBarIconEvent)

wxEVT_TASKBAR_MOVE = _windows_.wxEVT_TASKBAR_MOVE
wxEVT_TASKBAR_LEFT_DOWN = _windows_.wxEVT_TASKBAR_LEFT_DOWN
wxEVT_TASKBAR_LEFT_UP = _windows_.wxEVT_TASKBAR_LEFT_UP
wxEVT_TASKBAR_RIGHT_DOWN = _windows_.wxEVT_TASKBAR_RIGHT_DOWN
wxEVT_TASKBAR_RIGHT_UP = _windows_.wxEVT_TASKBAR_RIGHT_UP
wxEVT_TASKBAR_LEFT_DCLICK = _windows_.wxEVT_TASKBAR_LEFT_DCLICK
wxEVT_TASKBAR_RIGHT_DCLICK = _windows_.wxEVT_TASKBAR_RIGHT_DCLICK
wxEVT_TASKBAR_CLICK = _windows_.wxEVT_TASKBAR_CLICK
EVT_TASKBAR_MOVE = wx.PyEventBinder (         wxEVT_TASKBAR_MOVE )
EVT_TASKBAR_LEFT_DOWN = wx.PyEventBinder (    wxEVT_TASKBAR_LEFT_DOWN )
EVT_TASKBAR_LEFT_UP = wx.PyEventBinder (      wxEVT_TASKBAR_LEFT_UP )
EVT_TASKBAR_RIGHT_DOWN = wx.PyEventBinder (   wxEVT_TASKBAR_RIGHT_DOWN )
EVT_TASKBAR_RIGHT_UP = wx.PyEventBinder (     wxEVT_TASKBAR_RIGHT_UP )
EVT_TASKBAR_LEFT_DCLICK = wx.PyEventBinder (  wxEVT_TASKBAR_LEFT_DCLICK )
EVT_TASKBAR_RIGHT_DCLICK = wx.PyEventBinder ( wxEVT_TASKBAR_RIGHT_DCLICK )
EVT_TASKBAR_CLICK =  wx.PyEventBinder (       wxEVT_TASKBAR_CLICK )

#---------------------------------------------------------------------------

class ColourData(_core.Object):
    """
    This class holds a variety of information related to the colour
    chooser dialog, used to transfer settings and results to and from the
    `wx.ColourDialog`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ColourData

        Constructor, sets default values.
        """
        _windows_.ColourData_swiginit(self,_windows_.new_ColourData(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_ColourData
    __del__ = lambda self : None;
    def GetChooseFull(*args, **kwargs):
        """
        GetChooseFull(self) -> bool

        Under Windows, determines whether the Windows colour dialog will
        display the full dialog with custom colour selection controls. Has no
        meaning under other platforms.  The default value is true.
        """
        return _windows_.ColourData_GetChooseFull(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """
        GetColour(self) -> Colour

        Gets the colour (pre)selected by the dialog.
        """
        return _windows_.ColourData_GetColour(*args, **kwargs)

    def GetCustomColour(*args, **kwargs):
        """
        GetCustomColour(self, int i) -> Colour

        Gets the i'th custom colour associated with the colour dialog. i
        should be an integer between 0 and 15. The default custom colours are
        all invalid colours.
        """
        return _windows_.ColourData_GetCustomColour(*args, **kwargs)

    def SetChooseFull(*args, **kwargs):
        """
        SetChooseFull(self, int flag)

        Under Windows, tells the Windows colour dialog to display the full
        dialog with custom colour selection controls. Under other platforms,
        has no effect.  The default value is true.
        """
        return _windows_.ColourData_SetChooseFull(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """
        SetColour(self, Colour colour)

        Sets the default colour for the colour dialog.  The default colour is
        black.
        """
        return _windows_.ColourData_SetColour(*args, **kwargs)

    def SetCustomColour(*args, **kwargs):
        """
        SetCustomColour(self, int i, Colour colour)

        Sets the i'th custom colour for the colour dialog. i should be an
        integer between 0 and 15. The default custom colours are all invalid colours.
        """
        return _windows_.ColourData_SetCustomColour(*args, **kwargs)

    ChooseFull = property(GetChooseFull,SetChooseFull,doc="See `GetChooseFull` and `SetChooseFull`") 
    Colour = property(GetColour,SetColour,doc="See `GetColour` and `SetColour`") 
    CustomColour = property(GetCustomColour,SetCustomColour,doc="See `GetCustomColour` and `SetCustomColour`") 
_windows_.ColourData_swigregister(ColourData)
FileSelectorPromptStr = cvar.FileSelectorPromptStr
DirSelectorPromptStr = cvar.DirSelectorPromptStr
DirDialogNameStr = cvar.DirDialogNameStr
FileSelectorDefaultWildcardStr = cvar.FileSelectorDefaultWildcardStr
GetTextFromUserPromptStr = cvar.GetTextFromUserPromptStr
MessageBoxCaptionStr = cvar.MessageBoxCaptionStr

class ColourDialog(Dialog):
    """This class represents the colour chooser dialog."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, ColourData data=None) -> ColourDialog

        Constructor. Pass a parent window, and optionally a `wx.ColourData`,
        which will be copied to the colour dialog's internal ColourData
        instance.
        """
        _windows_.ColourDialog_swiginit(self,_windows_.new_ColourDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetColourData(*args, **kwargs):
        """
        GetColourData(self) -> ColourData

        Returns a reference to the `wx.ColourData` used by the dialog.
        """
        return _windows_.ColourDialog_GetColourData(*args, **kwargs)

    ColourData = property(GetColourData,doc="See `GetColourData`") 
_windows_.ColourDialog_swigregister(ColourDialog)


def GetColourFromUser(*args, **kwargs):
  """
    GetColourFromUser(Window parent=(wxWindow *) NULL, Colour colInit=wxNullColour, 
        String caption=EmptyString) -> Colour
    """
  return _windows_.GetColourFromUser(*args, **kwargs)
DD_NEW_DIR_BUTTON = _windows_.DD_NEW_DIR_BUTTON
DD_DEFAULT_STYLE = _windows_.DD_DEFAULT_STYLE
DD_CHANGE_DIR = _windows_.DD_CHANGE_DIR
class DirDialog(Dialog):
    """
    wx.DirDialog allows the user to select a directory by browising the
    file system.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message=DirSelectorPromptStr, 
            String defaultPath=EmptyString, long style=DD_DEFAULT_STYLE, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            String name=DirDialogNameStr) -> DirDialog

        Constructor.  Use ShowModal method to show the dialog.
        """
        _windows_.DirDialog_swiginit(self,_windows_.new_DirDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetPath(*args, **kwargs):
        """
        GetPath(self) -> String

        Returns the default or user-selected path.
        """
        return _windows_.DirDialog_GetPath(*args, **kwargs)

    def GetMessage(*args, **kwargs):
        """
        GetMessage(self) -> String

        Returns the message that will be displayed on the dialog.
        """
        return _windows_.DirDialog_GetMessage(*args, **kwargs)

    def SetMessage(*args, **kwargs):
        """
        SetMessage(self, String message)

        Sets the message that will be displayed on the dialog.
        """
        return _windows_.DirDialog_SetMessage(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """
        SetPath(self, String path)

        Sets the default path.
        """
        return _windows_.DirDialog_SetPath(*args, **kwargs)

    Message = property(GetMessage,SetMessage,doc="See `GetMessage` and `SetMessage`") 
    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
_windows_.DirDialog_swigregister(DirDialog)

OPEN = _windows_.OPEN
SAVE = _windows_.SAVE
OVERWRITE_PROMPT = _windows_.OVERWRITE_PROMPT
FILE_MUST_EXIST = _windows_.FILE_MUST_EXIST
MULTIPLE = _windows_.MULTIPLE
CHANGE_DIR = _windows_.CHANGE_DIR
HIDE_READONLY = _windows_.HIDE_READONLY
FD_OPEN = _windows_.FD_OPEN
FD_SAVE = _windows_.FD_SAVE
FD_OVERWRITE_PROMPT = _windows_.FD_OVERWRITE_PROMPT
FD_FILE_MUST_EXIST = _windows_.FD_FILE_MUST_EXIST
FD_MULTIPLE = _windows_.FD_MULTIPLE
FD_CHANGE_DIR = _windows_.FD_CHANGE_DIR
FD_PREVIEW = _windows_.FD_PREVIEW
FD_DEFAULT_STYLE = _windows_.FD_DEFAULT_STYLE
class FileDialog(Dialog):
    """
    wx.FileDialog allows the user to select one or more files from the
    filesystem.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message=FileSelectorPromptStr, 
            String defaultDir=EmptyString, String defaultFile=EmptyString, 
            String wildcard=FileSelectorDefaultWildcardStr, 
            long style=FD_DEFAULT_STYLE, 
            Point pos=DefaultPosition) -> FileDialog

        Constructor.  Use ShowModal method to show the dialog.
        """
        _windows_.FileDialog_swiginit(self,_windows_.new_FileDialog(*args, **kwargs))
        self._setOORInfo(self)

    def SetMessage(*args, **kwargs):
        """
        SetMessage(self, String message)

        Sets the message that will be displayed on the dialog.
        """
        return _windows_.FileDialog_SetMessage(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """
        SetPath(self, String path)

        Sets the path (the combined directory and filename that will be
        returned when the dialog is dismissed).
        """
        return _windows_.FileDialog_SetPath(*args, **kwargs)

    def SetDirectory(*args, **kwargs):
        """
        SetDirectory(self, String dir)

        Sets the default directory.
        """
        return _windows_.FileDialog_SetDirectory(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """
        SetFilename(self, String name)

        Sets the default filename.
        """
        return _windows_.FileDialog_SetFilename(*args, **kwargs)

    def SetWildcard(*args, **kwargs):
        """
        SetWildcard(self, String wildCard)

        Sets the wildcard, which can contain multiple file types, for
        example::

            "BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif"

        """
        return _windows_.FileDialog_SetWildcard(*args, **kwargs)

    def SetFilterIndex(*args, **kwargs):
        """
        SetFilterIndex(self, int filterIndex)

        Sets the default filter index, starting from zero.
        """
        return _windows_.FileDialog_SetFilterIndex(*args, **kwargs)

    def GetMessage(*args, **kwargs):
        """
        GetMessage(self) -> String

        Returns the message that will be displayed on the dialog.
        """
        return _windows_.FileDialog_GetMessage(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """
        GetPath(self) -> String

        Returns the full path (directory and filename) of the selected file.
        """
        return _windows_.FileDialog_GetPath(*args, **kwargs)

    def GetDirectory(*args, **kwargs):
        """
        GetDirectory(self) -> String

        Returns the default directory.
        """
        return _windows_.FileDialog_GetDirectory(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """
        GetFilename(self) -> String

        Returns the default filename.
        """
        return _windows_.FileDialog_GetFilename(*args, **kwargs)

    def GetWildcard(*args, **kwargs):
        """
        GetWildcard(self) -> String

        Returns the file dialog wildcard.
        """
        return _windows_.FileDialog_GetWildcard(*args, **kwargs)

    def GetFilterIndex(*args, **kwargs):
        """
        GetFilterIndex(self) -> int

        Returns the index into the list of filters supplied, optionally, in
        the wildcard parameter. Before the dialog is shown, this is the index
        which will be used when the dialog is first displayed. After the
        dialog is shown, this is the index selected by the user.
        """
        return _windows_.FileDialog_GetFilterIndex(*args, **kwargs)

    def GetFilenames(*args, **kwargs):
        """
        GetFilenames(self) -> PyObject

        Returns a list of filenames chosen in the dialog.  This function
        should only be used with the dialogs which have wx.MULTIPLE style, use
        GetFilename for the others.
        """
        return _windows_.FileDialog_GetFilenames(*args, **kwargs)

    def GetPaths(*args, **kwargs):
        """
        GetPaths(self) -> PyObject

        Fills the array paths with the full paths of the files chosen. This
        function should only be used with the dialogs which have wx.MULTIPLE
        style, use GetPath for the others.
        """
        return _windows_.FileDialog_GetPaths(*args, **kwargs)

    Directory = property(GetDirectory,SetDirectory,doc="See `GetDirectory` and `SetDirectory`") 
    Filename = property(GetFilename,SetFilename,doc="See `GetFilename` and `SetFilename`") 
    Filenames = property(GetFilenames,doc="See `GetFilenames`") 
    FilterIndex = property(GetFilterIndex,SetFilterIndex,doc="See `GetFilterIndex` and `SetFilterIndex`") 
    Message = property(GetMessage,SetMessage,doc="See `GetMessage` and `SetMessage`") 
    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
    Paths = property(GetPaths,doc="See `GetPaths`") 
    Wildcard = property(GetWildcard,SetWildcard,doc="See `GetWildcard` and `SetWildcard`") 
_windows_.FileDialog_swigregister(FileDialog)

CHOICEDLG_STYLE = _windows_.CHOICEDLG_STYLE
class MultiChoiceDialog(Dialog):
    """A simple dialog with a multi selection listbox."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message, String caption,
            List choices=EmptyList, long style=CHOICEDLG_STYLE,
            Point pos=DefaultPosition) -> MultiChoiceDialog

        Constructor.  Use the `ShowModal` method to show the dialog.

            :param parent: The parent window.
            :param message: Text to display above the list of selections.
            :param caption: Text to use in the title bar of the dialog.
            :param choices: A list of strings or unicode objects that the
                user is allowed to choose from.
            :param style: Styles to apply to the dialog.  The default value is
                equivallent to wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.OK|wx.CANCEL|wx.CENTER.
            :param pos: Where to position the dialog (not used on Windows)


        """
        _windows_.MultiChoiceDialog_swiginit(self,_windows_.new_MultiChoiceDialog(*args, **kwargs))
        self._setOORInfo(self)

    def SetSelections(*args, **kwargs):
        """
        SetSelections(List selections)

        Specify the items in the list that should be selected, using a list of
        integers.  The list should specify the indexes of the items that
        should be selected.
        """
        return _windows_.MultiChoiceDialog_SetSelections(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """
        GetSelections() -> [selections]

        Returns a list of integers representing the items that are selected.
        If an item is selected then its index will appear in the list.
        """
        return _windows_.MultiChoiceDialog_GetSelections(*args, **kwargs)

    Selections = property(GetSelections,SetSelections,doc="See `GetSelections` and `SetSelections`") 
_windows_.MultiChoiceDialog_swigregister(MultiChoiceDialog)

class SingleChoiceDialog(Dialog):
    """A simple dialog with a single selection listbox."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(Window parent, String message, String caption,
            List choices=EmptyList, long style=CHOICEDLG_STYLE,
            Point pos=DefaultPosition) -> SingleChoiceDialog

        Constructor.  Use ShowModal method to show the dialog.
        """
        _windows_.SingleChoiceDialog_swiginit(self,_windows_.new_SingleChoiceDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Get the index of the currently selected item.
        """
        return _windows_.SingleChoiceDialog_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """
        GetStringSelection(self) -> String

        Returns the string value of the currently selected item
        """
        return _windows_.SingleChoiceDialog_GetStringSelection(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, int sel)

        Set the current selected item to sel
        """
        return _windows_.SingleChoiceDialog_SetSelection(*args, **kwargs)

    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    StringSelection = property(GetStringSelection,doc="See `GetStringSelection`") 
_windows_.SingleChoiceDialog_swigregister(SingleChoiceDialog)

TextEntryDialogStyle = _windows_.TextEntryDialogStyle
class TextEntryDialog(Dialog):
    """A dialog with text control, [ok] and [cancel] buttons"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message, String caption=GetTextFromUserPromptStr, 
            String defaultValue=EmptyString, 
            long style=TextEntryDialogStyle, Point pos=DefaultPosition) -> TextEntryDialog

        Constructor.  Use ShowModal method to show the dialog.
        """
        _windows_.TextEntryDialog_swiginit(self,_windows_.new_TextEntryDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """
        GetValue(self) -> String

        Returns the text that the user has entered if the user has pressed OK,
        or the original value if the user has pressed Cancel.
        """
        return _windows_.TextEntryDialog_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """
        SetValue(self, String value)

        Sets the default text value.
        """
        return _windows_.TextEntryDialog_SetValue(*args, **kwargs)

    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_windows_.TextEntryDialog_swigregister(TextEntryDialog)

class PasswordEntryDialog(TextEntryDialog):
    """Proxy of C++ PasswordEntryDialog class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message, String caption=GetPasswordFromUserPromptStr, 
            String value=EmptyString, 
            long style=TextEntryDialogStyle, Point pos=DefaultPosition) -> PasswordEntryDialog
        """
        _windows_.PasswordEntryDialog_swiginit(self,_windows_.new_PasswordEntryDialog(*args, **kwargs))
_windows_.PasswordEntryDialog_swigregister(PasswordEntryDialog)
GetPasswordFromUserPromptStr = cvar.GetPasswordFromUserPromptStr

class NumberEntryDialog(Dialog):
    """A dialog with spin control, ok and cancel buttons."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message, String prompt, String caption, 
            long value, long min, long max, Point pos=DefaultPosition) -> NumberEntryDialog

        Constructor.  Use ShowModal method to show the dialog.
        """
        _windows_.NumberEntryDialog_swiginit(self,_windows_.new_NumberEntryDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> long"""
        return _windows_.NumberEntryDialog_GetValue(*args, **kwargs)

    Value = property(GetValue,doc="See `GetValue`") 
_windows_.NumberEntryDialog_swigregister(NumberEntryDialog)

class FontData(_core.Object):
    """
    This class holds a variety of information related to font dialogs and
    is used to transfer settings to and results from a `wx.FontDialog`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> FontData

        This class holds a variety of information related to font dialogs and
        is used to transfer settings to and results from a `wx.FontDialog`.
        """
        _windows_.FontData_swiginit(self,_windows_.new_FontData(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_FontData
    __del__ = lambda self : None;
    def EnableEffects(*args, **kwargs):
        """
        EnableEffects(self, bool enable)

        Enables or disables 'effects' under MS Windows only. This refers to
        the controls for manipulating colour, strikeout and underline
        properties.  The default value is true.
        """
        return _windows_.FontData_EnableEffects(*args, **kwargs)

    def GetAllowSymbols(*args, **kwargs):
        """
        GetAllowSymbols(self) -> bool

        Under MS Windows, returns a flag determining whether symbol fonts can
        be selected. Has no effect on other platforms. The default value is
        true.
        """
        return _windows_.FontData_GetAllowSymbols(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """
        GetColour(self) -> Colour

        Gets the colour associated with the font dialog. The default value is
        black.
        """
        return _windows_.FontData_GetColour(*args, **kwargs)

    def GetChosenFont(*args, **kwargs):
        """
        GetChosenFont(self) -> Font

        Gets the font chosen by the user.
        """
        return _windows_.FontData_GetChosenFont(*args, **kwargs)

    def GetEnableEffects(*args, **kwargs):
        """
        GetEnableEffects(self) -> bool

        Determines whether 'effects' are enabled under Windows.
        """
        return _windows_.FontData_GetEnableEffects(*args, **kwargs)

    def GetInitialFont(*args, **kwargs):
        """
        GetInitialFont(self) -> Font

        Gets the font that will be initially used by the font dialog. This
        should have previously been set by the application.
        """
        return _windows_.FontData_GetInitialFont(*args, **kwargs)

    def GetShowHelp(*args, **kwargs):
        """
        GetShowHelp(self) -> bool

        Returns true if the Help button will be shown (Windows only).  The
        default value is false.
        """
        return _windows_.FontData_GetShowHelp(*args, **kwargs)

    def SetAllowSymbols(*args, **kwargs):
        """
        SetAllowSymbols(self, bool allowSymbols)

        Under MS Windows, determines whether symbol fonts can be selected. Has
        no effect on other platforms.  The default value is true.
        """
        return _windows_.FontData_SetAllowSymbols(*args, **kwargs)

    def SetChosenFont(*args, **kwargs):
        """
        SetChosenFont(self, Font font)

        Sets the font that will be returned to the user (normally for internal
        use only).
        """
        return _windows_.FontData_SetChosenFont(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """
        SetColour(self, Colour colour)

        Sets the colour that will be used for the font foreground colour.  The
        default colour is black.
        """
        return _windows_.FontData_SetColour(*args, **kwargs)

    def SetInitialFont(*args, **kwargs):
        """
        SetInitialFont(self, Font font)

        Sets the font that will be initially used by the font dialog.
        """
        return _windows_.FontData_SetInitialFont(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """
        SetRange(self, int min, int max)

        Sets the valid range for the font point size (Windows only).  The
        default is 0, 0 (unrestricted range).
        """
        return _windows_.FontData_SetRange(*args, **kwargs)

    def SetShowHelp(*args, **kwargs):
        """
        SetShowHelp(self, bool showHelp)

        Determines whether the Help button will be displayed in the font
        dialog (Windows only).  The default value is false.
        """
        return _windows_.FontData_SetShowHelp(*args, **kwargs)

    AllowSymbols = property(GetAllowSymbols,SetAllowSymbols,doc="See `GetAllowSymbols` and `SetAllowSymbols`") 
    ChosenFont = property(GetChosenFont,SetChosenFont,doc="See `GetChosenFont` and `SetChosenFont`") 
    Colour = property(GetColour,SetColour,doc="See `GetColour` and `SetColour`") 
    InitialFont = property(GetInitialFont,SetInitialFont,doc="See `GetInitialFont` and `SetInitialFont`") 
    ShowHelp = property(GetShowHelp,SetShowHelp,doc="See `GetShowHelp` and `SetShowHelp`") 
_windows_.FontData_swigregister(FontData)

class FontDialog(Dialog):
    """
    wx.FontDialog allows the user to select a system font and its attributes.

    :see: `wx.FontData`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, FontData data) -> FontDialog

        Constructor. Pass a parent window and the `wx.FontData` object to be
        used to initialize the dialog controls.  Call `ShowModal` to display
        the dialog.  If ShowModal returns ``wx.ID_OK`` then you can fetch the
        results with via the `wx.FontData` returned by `GetFontData`.
        """
        _windows_.FontDialog_swiginit(self,_windows_.new_FontDialog(*args, **kwargs))
        self._setOORInfo(self)

    def GetFontData(*args, **kwargs):
        """
        GetFontData(self) -> FontData

        Returns a reference to the internal `wx.FontData` used by the
        wx.FontDialog.
        """
        return _windows_.FontDialog_GetFontData(*args, **kwargs)

    FontData = property(GetFontData,doc="See `GetFontData`") 
_windows_.FontDialog_swigregister(FontDialog)


def GetFontFromUser(*args, **kwargs):
  """GetFontFromUser(Window parent=None, Font fontInit=wxNullFont, String caption=EmptyString) -> Font"""
  return _windows_.GetFontFromUser(*args, **kwargs)
class MessageDialog(Dialog):
    """
    This class provides a simple dialog that shows a single or multi-line
    message, with a choice of OK, Yes, No and/or Cancel buttons.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, String message, String caption=MessageBoxCaptionStr, 
            long style=wxOK|wxCANCEL|wxCENTRE, 
            Point pos=DefaultPosition) -> MessageDialog

        Constructor, use `ShowModal` to display the dialog.
        """
        _windows_.MessageDialog_swiginit(self,_windows_.new_MessageDialog(*args, **kwargs))
        self._setOORInfo(self)

_windows_.MessageDialog_swigregister(MessageDialog)

PD_AUTO_HIDE = _windows_.PD_AUTO_HIDE
PD_APP_MODAL = _windows_.PD_APP_MODAL
PD_CAN_ABORT = _windows_.PD_CAN_ABORT
PD_ELAPSED_TIME = _windows_.PD_ELAPSED_TIME
PD_ESTIMATED_TIME = _windows_.PD_ESTIMATED_TIME
PD_REMAINING_TIME = _windows_.PD_REMAINING_TIME
PD_SMOOTH = _windows_.PD_SMOOTH
PD_CAN_SKIP = _windows_.PD_CAN_SKIP
class ProgressDialog(Frame):
    """
    A dialog that shows a short message and a progress bar. Optionally, it
    can display an ABORT button.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String title, String message, int maximum=100, Window parent=None, 
            int style=wxPD_AUTO_HIDE|wxPD_APP_MODAL) -> ProgressDialog

        Constructor. Creates the dialog, displays it and disables user input
        for other windows, or, if wx.PD_APP_MODAL flag is not given, for its
        parent window only.
        """
        _windows_.ProgressDialog_swiginit(self,_windows_.new_ProgressDialog(*args, **kwargs))
        self._setOORInfo(self)

    def Update(*args, **kwargs):
        """
        Update(self, int value, String newmsg) --> (continue, skip)

        Updates the dialog, setting the progress bar to the new value and, if
        given changes the message above it. The value given should be less
        than or equal to the maximum value given to the constructor and the
        dialog is closed if it is equal to the maximum.  Returns a tuple of
        boolean values, ``(continue, skip)`` where ``continue`` is ``True``
        unless the Cancel button has been pressed, and ``skip`` is ``False``
        unless the Skip button (if any) has been pressed.

        If the ``continue`` return value is ``False``, the application can either
        immediately destroy the dialog or ask the user for confirmation, and if the
        abort is not confirmed the dialog may be resumed with `Resume` function.

        """
        return _windows_.ProgressDialog_Update(*args, **kwargs)

    def Pulse(*args, **kwargs):
        """
        Pulse(self, String newmsg) --> (continue, skip)

        Just like `Update` but switches the dialog to use a gauge in
        interminante mode and calls `wx.Gauge.Pulse` to show the user a bit of
        progress.
        """
        return _windows_.ProgressDialog_Pulse(*args, **kwargs)

    UpdatePulse =  Pulse 
    def Resume(*args, **kwargs):
        """
        Resume(self)

        Can be used to continue with the dialog, after the user had chosen to
        abort.
        """
        return _windows_.ProgressDialog_Resume(*args, **kwargs)

_windows_.ProgressDialog_swigregister(ProgressDialog)

FR_DOWN = _windows_.FR_DOWN
FR_WHOLEWORD = _windows_.FR_WHOLEWORD
FR_MATCHCASE = _windows_.FR_MATCHCASE
FR_REPLACEDIALOG = _windows_.FR_REPLACEDIALOG
FR_NOUPDOWN = _windows_.FR_NOUPDOWN
FR_NOMATCHCASE = _windows_.FR_NOMATCHCASE
FR_NOWHOLEWORD = _windows_.FR_NOWHOLEWORD
wxEVT_COMMAND_FIND = _windows_.wxEVT_COMMAND_FIND
wxEVT_COMMAND_FIND_NEXT = _windows_.wxEVT_COMMAND_FIND_NEXT
wxEVT_COMMAND_FIND_REPLACE = _windows_.wxEVT_COMMAND_FIND_REPLACE
wxEVT_COMMAND_FIND_REPLACE_ALL = _windows_.wxEVT_COMMAND_FIND_REPLACE_ALL
wxEVT_COMMAND_FIND_CLOSE = _windows_.wxEVT_COMMAND_FIND_CLOSE
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

class FindDialogEvent(_core.CommandEvent):
    """Events for the FindReplaceDialog"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0) -> FindDialogEvent

        Events for the FindReplaceDialog
        """
        _windows_.FindDialogEvent_swiginit(self,_windows_.new_FindDialogEvent(*args, **kwargs))
    def GetFlags(*args, **kwargs):
        """
        GetFlags(self) -> int

        Get the currently selected flags: this is the combination of
        wx.FR_DOWN, wx.FR_WHOLEWORD and wx.FR_MATCHCASE flags.
        """
        return _windows_.FindDialogEvent_GetFlags(*args, **kwargs)

    def GetFindString(*args, **kwargs):
        """
        GetFindString(self) -> String

        Return the string to find (never empty).
        """
        return _windows_.FindDialogEvent_GetFindString(*args, **kwargs)

    def GetReplaceString(*args, **kwargs):
        """
        GetReplaceString(self) -> String

        Return the string to replace the search string with (only for replace
        and replace all events).
        """
        return _windows_.FindDialogEvent_GetReplaceString(*args, **kwargs)

    def GetDialog(*args, **kwargs):
        """
        GetDialog(self) -> FindReplaceDialog

        Return the pointer to the dialog which generated this event.
        """
        return _windows_.FindDialogEvent_GetDialog(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, int flags)"""
        return _windows_.FindDialogEvent_SetFlags(*args, **kwargs)

    def SetFindString(*args, **kwargs):
        """SetFindString(self, String str)"""
        return _windows_.FindDialogEvent_SetFindString(*args, **kwargs)

    def SetReplaceString(*args, **kwargs):
        """SetReplaceString(self, String str)"""
        return _windows_.FindDialogEvent_SetReplaceString(*args, **kwargs)

    Dialog = property(GetDialog,doc="See `GetDialog`") 
    FindString = property(GetFindString,SetFindString,doc="See `GetFindString` and `SetFindString`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    ReplaceString = property(GetReplaceString,SetReplaceString,doc="See `GetReplaceString` and `SetReplaceString`") 
_windows_.FindDialogEvent_swigregister(FindDialogEvent)

class FindReplaceData(_core.Object):
    """
    wx.FindReplaceData holds the data for wx.FindReplaceDialog. It is used
    to initialize the dialog with the default values and will keep the
    last values from the dialog when it is closed. It is also updated each
    time a `wx.FindDialogEvent` is generated so instead of using the
    `wx.FindDialogEvent` methods you can also directly query this object.

    Note that all SetXXX() methods may only be called before showing the
    dialog and calling them has no effect later.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int flags=0) -> FindReplaceData

        Constuctor initializes the flags to default value (0).
        """
        _windows_.FindReplaceData_swiginit(self,_windows_.new_FindReplaceData(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_FindReplaceData
    __del__ = lambda self : None;
    def GetFindString(*args, **kwargs):
        """
        GetFindString(self) -> String

        Get the string to find.
        """
        return _windows_.FindReplaceData_GetFindString(*args, **kwargs)

    def GetReplaceString(*args, **kwargs):
        """
        GetReplaceString(self) -> String

        Get the replacement string.
        """
        return _windows_.FindReplaceData_GetReplaceString(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """
        GetFlags(self) -> int

        Get the combination of flag values.
        """
        return _windows_.FindReplaceData_GetFlags(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """
        SetFlags(self, int flags)

        Set the flags to use to initialize the controls of the dialog.
        """
        return _windows_.FindReplaceData_SetFlags(*args, **kwargs)

    def SetFindString(*args, **kwargs):
        """
        SetFindString(self, String str)

        Set the string to find (used as initial value by the dialog).
        """
        return _windows_.FindReplaceData_SetFindString(*args, **kwargs)

    def SetReplaceString(*args, **kwargs):
        """
        SetReplaceString(self, String str)

        Set the replacement string (used as initial value by the dialog).
        """
        return _windows_.FindReplaceData_SetReplaceString(*args, **kwargs)

    FindString = property(GetFindString,SetFindString,doc="See `GetFindString` and `SetFindString`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    ReplaceString = property(GetReplaceString,SetReplaceString,doc="See `GetReplaceString` and `SetReplaceString`") 
_windows_.FindReplaceData_swigregister(FindReplaceData)

class FindReplaceDialog(Dialog):
    """
    wx.FindReplaceDialog is a standard modeless dialog which is used to
    allow the user to search for some text (and possibly replace it with
    something else). The actual searching is supposed to be done in the
    owner window which is the parent of this dialog. Note that it means
    that unlike for the other standard dialogs this one must have a parent
    window. Also note that there is no way to use this dialog in a modal
    way; it is always, by design and implementation, modeless.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, FindReplaceData data, String title, 
            int style=0) -> FindReplaceDialog

        Create a FindReplaceDialog.  The parent and data parameters must be
        non-None.  Use Show to display the dialog.
        """
        _windows_.FindReplaceDialog_swiginit(self,_windows_.new_FindReplaceDialog(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, FindReplaceData data, String title, 
            int style=0) -> bool

        Create the dialog, for 2-phase create.
        """
        return _windows_.FindReplaceDialog_Create(*args, **kwargs)

    def GetData(*args, **kwargs):
        """
        GetData(self) -> FindReplaceData

        Get the FindReplaceData object used by this dialog.
        """
        return _windows_.FindReplaceDialog_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, FindReplaceData data)

        Set the FindReplaceData object used by this dialog.
        """
        return _windows_.FindReplaceDialog_SetData(*args, **kwargs)

    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
_windows_.FindReplaceDialog_swigregister(FindReplaceDialog)

def PreFindReplaceDialog(*args, **kwargs):
    """
    PreFindReplaceDialog() -> FindReplaceDialog

    Precreate a FindReplaceDialog for 2-phase creation
    """
    val = _windows_.new_PreFindReplaceDialog(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

IDM_WINDOWTILE = _windows_.IDM_WINDOWTILE
IDM_WINDOWTILEHOR = _windows_.IDM_WINDOWTILEHOR
IDM_WINDOWCASCADE = _windows_.IDM_WINDOWCASCADE
IDM_WINDOWICONS = _windows_.IDM_WINDOWICONS
IDM_WINDOWNEXT = _windows_.IDM_WINDOWNEXT
IDM_WINDOWTILEVERT = _windows_.IDM_WINDOWTILEVERT
IDM_WINDOWPREV = _windows_.IDM_WINDOWPREV
FIRST_MDI_CHILD = _windows_.FIRST_MDI_CHILD
LAST_MDI_CHILD = _windows_.LAST_MDI_CHILD
class MDIParentFrame(Frame):
    """Proxy of C++ MDIParentFrame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL, 
            String name=FrameNameStr) -> MDIParentFrame
        """
        _windows_.MDIParentFrame_swiginit(self,_windows_.new_MDIParentFrame(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL, 
            String name=FrameNameStr) -> bool
        """
        return _windows_.MDIParentFrame_Create(*args, **kwargs)

    def ActivateNext(*args, **kwargs):
        """ActivateNext(self)"""
        return _windows_.MDIParentFrame_ActivateNext(*args, **kwargs)

    def ActivatePrevious(*args, **kwargs):
        """ActivatePrevious(self)"""
        return _windows_.MDIParentFrame_ActivatePrevious(*args, **kwargs)

    def ArrangeIcons(*args, **kwargs):
        """ArrangeIcons(self)"""
        return _windows_.MDIParentFrame_ArrangeIcons(*args, **kwargs)

    def Cascade(*args, **kwargs):
        """Cascade(self)"""
        return _windows_.MDIParentFrame_Cascade(*args, **kwargs)

    def GetActiveChild(*args, **kwargs):
        """GetActiveChild(self) -> MDIChildFrame"""
        return _windows_.MDIParentFrame_GetActiveChild(*args, **kwargs)

    def GetClientWindow(*args, **kwargs):
        """GetClientWindow(self) -> MDIClientWindow"""
        return _windows_.MDIParentFrame_GetClientWindow(*args, **kwargs)

    def GetWindowMenu(*args, **kwargs):
        """GetWindowMenu(self) -> Menu"""
        return _windows_.MDIParentFrame_GetWindowMenu(*args, **kwargs)

    def SetWindowMenu(*args, **kwargs):
        """SetWindowMenu(self, Menu menu)"""
        return _windows_.MDIParentFrame_SetWindowMenu(*args, **kwargs)

    def Tile(*args, **kwargs):
        """Tile(self, int orient=HORIZONTAL)"""
        return _windows_.MDIParentFrame_Tile(*args, **kwargs)

    ActiveChild = property(GetActiveChild,doc="See `GetActiveChild`") 
    ClientWindow = property(GetClientWindow,doc="See `GetClientWindow`") 
_windows_.MDIParentFrame_swigregister(MDIParentFrame)

def PreMDIParentFrame(*args, **kwargs):
    """PreMDIParentFrame() -> MDIParentFrame"""
    val = _windows_.new_PreMDIParentFrame(*args, **kwargs)
    return val

class MDIChildFrame(Frame):
    """Proxy of C++ MDIChildFrame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, MDIParentFrame parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, 
            String name=FrameNameStr) -> MDIChildFrame
        """
        _windows_.MDIChildFrame_swiginit(self,_windows_.new_MDIChildFrame(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, MDIParentFrame parent, int id=-1, String title=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, 
            String name=FrameNameStr) -> bool
        """
        return _windows_.MDIChildFrame_Create(*args, **kwargs)

    def Activate(*args, **kwargs):
        """Activate(self)"""
        return _windows_.MDIChildFrame_Activate(*args, **kwargs)

_windows_.MDIChildFrame_swigregister(MDIChildFrame)

def PreMDIChildFrame(*args, **kwargs):
    """PreMDIChildFrame() -> MDIChildFrame"""
    val = _windows_.new_PreMDIChildFrame(*args, **kwargs)
    return val

class MDIClientWindow(_core.Window):
    """Proxy of C++ MDIClientWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, MDIParentFrame parent, long style=0) -> MDIClientWindow"""
        _windows_.MDIClientWindow_swiginit(self,_windows_.new_MDIClientWindow(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(self, MDIParentFrame parent, long style=0) -> bool"""
        return _windows_.MDIClientWindow_Create(*args, **kwargs)

_windows_.MDIClientWindow_swigregister(MDIClientWindow)

def PreMDIClientWindow(*args, **kwargs):
    """PreMDIClientWindow() -> MDIClientWindow"""
    val = _windows_.new_PreMDIClientWindow(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class PyWindow(_core.Window):
    """Proxy of C++ PyWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> PyWindow
        """
        _windows_.PyWindow_swiginit(self,_windows_.new_PyWindow(*args, **kwargs))
        self._setOORInfo(self);PyWindow._setCallbackInfo(self, self, PyWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyWindow__setCallbackInfo(*args, **kwargs)

    SetBestSize = wx.Window.SetInitialSize 
    def DoEraseBackground(*args, **kwargs):
        """DoEraseBackground(self, DC dc) -> bool"""
        return _windows_.PyWindow_DoEraseBackground(*args, **kwargs)

    def DoMoveWindow(*args, **kwargs):
        """DoMoveWindow(self, int x, int y, int width, int height)"""
        return _windows_.PyWindow_DoMoveWindow(*args, **kwargs)

    def DoSetSize(*args, **kwargs):
        """DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _windows_.PyWindow_DoSetSize(*args, **kwargs)

    def DoSetClientSize(*args, **kwargs):
        """DoSetClientSize(self, int width, int height)"""
        return _windows_.PyWindow_DoSetClientSize(*args, **kwargs)

    def DoSetVirtualSize(*args, **kwargs):
        """DoSetVirtualSize(self, int x, int y)"""
        return _windows_.PyWindow_DoSetVirtualSize(*args, **kwargs)

    def DoGetSize(*args, **kwargs):
        """DoGetSize() -> (width, height)"""
        return _windows_.PyWindow_DoGetSize(*args, **kwargs)

    def DoGetClientSize(*args, **kwargs):
        """DoGetClientSize() -> (width, height)"""
        return _windows_.PyWindow_DoGetClientSize(*args, **kwargs)

    def DoGetPosition(*args, **kwargs):
        """DoGetPosition() -> (x,y)"""
        return _windows_.PyWindow_DoGetPosition(*args, **kwargs)

    def DoGetVirtualSize(*args, **kwargs):
        """DoGetVirtualSize(self) -> Size"""
        return _windows_.PyWindow_DoGetVirtualSize(*args, **kwargs)

    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _windows_.PyWindow_DoGetBestSize(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """GetDefaultAttributes(self) -> VisualAttributes"""
        return _windows_.PyWindow_GetDefaultAttributes(*args, **kwargs)

    def OnInternalIdle(*args, **kwargs):
        """OnInternalIdle(self)"""
        return _windows_.PyWindow_OnInternalIdle(*args, **kwargs)

    def base_DoMoveWindow(*args, **kw):
        return PyWindow.DoMoveWindow(*args, **kw)
    base_DoMoveWindow = wx._deprecated(base_DoMoveWindow,
                                   "Please use PyWindow.DoMoveWindow instead.")

    def base_DoSetSize(*args, **kw):
        return PyWindow.DoSetSize(*args, **kw)
    base_DoSetSize = wx._deprecated(base_DoSetSize,
                                   "Please use PyWindow.DoSetSize instead.")

    def base_DoSetClientSize(*args, **kw):
        return PyWindow.DoSetClientSize(*args, **kw)
    base_DoSetClientSize = wx._deprecated(base_DoSetClientSize,
                                   "Please use PyWindow.DoSetClientSize instead.")

    def base_DoSetVirtualSize(*args, **kw):
        return PyWindow.DoSetVirtualSize(*args, **kw)
    base_DoSetVirtualSize = wx._deprecated(base_DoSetVirtualSize,
                                   "Please use PyWindow.DoSetVirtualSize instead.")

    def base_DoGetSize(*args, **kw):
        return PyWindow.DoGetSize(*args, **kw)
    base_DoGetSize = wx._deprecated(base_DoGetSize,
                                   "Please use PyWindow.DoGetSize instead.")

    def base_DoGetClientSize(*args, **kw):
        return PyWindow.DoGetClientSize(*args, **kw)
    base_DoGetClientSize = wx._deprecated(base_DoGetClientSize,
                                   "Please use PyWindow.DoGetClientSize instead.")

    def base_DoGetPosition(*args, **kw):
        return PyWindow.DoGetPosition(*args, **kw)
    base_DoGetPosition = wx._deprecated(base_DoGetPosition,
                                   "Please use PyWindow.DoGetPosition instead.")

    def base_DoGetVirtualSize(*args, **kw):
        return PyWindow.DoGetVirtualSize(*args, **kw)
    base_DoGetVirtualSize = wx._deprecated(base_DoGetVirtualSize,
                                   "Please use PyWindow.DoGetVirtualSize instead.")

    def base_DoGetBestSize(*args, **kw):
        return PyWindow.DoGetBestSize(*args, **kw)
    base_DoGetBestSize = wx._deprecated(base_DoGetBestSize,
                                   "Please use PyWindow.DoGetBestSize instead.")

    def base_InitDialog(*args, **kw):
        return PyWindow.InitDialog(*args, **kw)
    base_InitDialog = wx._deprecated(base_InitDialog,
                                   "Please use PyWindow.InitDialog instead.")

    def base_TransferDataToWindow(*args, **kw):
        return PyWindow.TransferDataToWindow(*args, **kw)
    base_TransferDataToWindow = wx._deprecated(base_TransferDataToWindow,
                                   "Please use PyWindow.TransferDataToWindow instead.")

    def base_TransferDataFromWindow(*args, **kw):
        return PyWindow.TransferDataFromWindow(*args, **kw)
    base_TransferDataFromWindow = wx._deprecated(base_TransferDataFromWindow,
                                   "Please use PyWindow.TransferDataFromWindow instead.")

    def base_Validate(*args, **kw):
        return PyWindow.Validate(*args, **kw)
    base_Validate = wx._deprecated(base_Validate,
                                   "Please use PyWindow.Validate instead.")

    def base_AcceptsFocus(*args, **kw):
        return PyWindow.AcceptsFocus(*args, **kw)
    base_AcceptsFocus = wx._deprecated(base_AcceptsFocus,
                                   "Please use PyWindow.AcceptsFocus instead.")

    def base_AcceptsFocusFromKeyboard(*args, **kw):
        return PyWindow.AcceptsFocusFromKeyboard(*args, **kw)
    base_AcceptsFocusFromKeyboard = wx._deprecated(base_AcceptsFocusFromKeyboard,
                                   "Please use PyWindow.AcceptsFocusFromKeyboard instead.")

    def base_GetMaxSize(*args, **kw):
        return PyWindow.GetMaxSize(*args, **kw)
    base_GetMaxSize = wx._deprecated(base_GetMaxSize,
                                   "Please use PyWindow.GetMaxSize instead.")

    def base_AddChild(*args, **kw):
        return PyWindow.AddChild(*args, **kw)
    base_AddChild = wx._deprecated(base_AddChild,
                                   "Please use PyWindow.AddChild instead.")

    def base_RemoveChild(*args, **kw):
        return PyWindow.RemoveChild(*args, **kw)
    base_RemoveChild = wx._deprecated(base_RemoveChild,
                                   "Please use PyWindow.RemoveChild instead.")

    def base_ShouldInheritColours(*args, **kw):
        return PyWindow.ShouldInheritColours(*args, **kw)
    base_ShouldInheritColours = wx._deprecated(base_ShouldInheritColours,
                                   "Please use PyWindow.ShouldInheritColours instead.")

    def base_GetDefaultAttributes(*args, **kw):
        return PyWindow.GetDefaultAttributes(*args, **kw)
    base_GetDefaultAttributes = wx._deprecated(base_GetDefaultAttributes,
                                   "Please use PyWindow.GetDefaultAttributes instead.")

    def base_OnInternalIdle(*args, **kw):
        return PyWindow.OnInternalIdle(*args, **kw)
    base_OnInternalIdle = wx._deprecated(base_OnInternalIdle,
                                   "Please use PyWindow.OnInternalIdle instead.")

_windows_.PyWindow_swigregister(PyWindow)

def PrePyWindow(*args, **kwargs):
    """PrePyWindow() -> PyWindow"""
    val = _windows_.new_PrePyWindow(*args, **kwargs)
    return val

class PyPanel(Panel):
    """Proxy of C++ PyPanel class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> PyPanel
        """
        _windows_.PyPanel_swiginit(self,_windows_.new_PyPanel(*args, **kwargs))
        self._setOORInfo(self);PyPanel._setCallbackInfo(self, self, PyPanel)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyPanel__setCallbackInfo(*args, **kwargs)

    SetBestSize = wx.Window.SetInitialSize 
    def DoEraseBackground(*args, **kwargs):
        """DoEraseBackground(self, DC dc) -> bool"""
        return _windows_.PyPanel_DoEraseBackground(*args, **kwargs)

    def DoMoveWindow(*args, **kwargs):
        """DoMoveWindow(self, int x, int y, int width, int height)"""
        return _windows_.PyPanel_DoMoveWindow(*args, **kwargs)

    def DoSetSize(*args, **kwargs):
        """DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _windows_.PyPanel_DoSetSize(*args, **kwargs)

    def DoSetClientSize(*args, **kwargs):
        """DoSetClientSize(self, int width, int height)"""
        return _windows_.PyPanel_DoSetClientSize(*args, **kwargs)

    def DoSetVirtualSize(*args, **kwargs):
        """DoSetVirtualSize(self, int x, int y)"""
        return _windows_.PyPanel_DoSetVirtualSize(*args, **kwargs)

    def DoGetSize(*args, **kwargs):
        """DoGetSize() -> (width, height)"""
        return _windows_.PyPanel_DoGetSize(*args, **kwargs)

    def DoGetClientSize(*args, **kwargs):
        """DoGetClientSize() -> (width, height)"""
        return _windows_.PyPanel_DoGetClientSize(*args, **kwargs)

    def DoGetPosition(*args, **kwargs):
        """DoGetPosition() -> (x,y)"""
        return _windows_.PyPanel_DoGetPosition(*args, **kwargs)

    def DoGetVirtualSize(*args, **kwargs):
        """DoGetVirtualSize(self) -> Size"""
        return _windows_.PyPanel_DoGetVirtualSize(*args, **kwargs)

    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _windows_.PyPanel_DoGetBestSize(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """GetDefaultAttributes(self) -> VisualAttributes"""
        return _windows_.PyPanel_GetDefaultAttributes(*args, **kwargs)

    def OnInternalIdle(*args, **kwargs):
        """OnInternalIdle(self)"""
        return _windows_.PyPanel_OnInternalIdle(*args, **kwargs)

    def base_DoMoveWindow(*args, **kw):
        return PyPanel.DoMoveWindow(*args, **kw)
    base_DoMoveWindow = wx._deprecated(base_DoMoveWindow,
                                   "Please use PyPanel.DoMoveWindow instead.")

    def base_DoSetSize(*args, **kw):
        return PyPanel.DoSetSize(*args, **kw)
    base_DoSetSize = wx._deprecated(base_DoSetSize,
                                   "Please use PyPanel.DoSetSize instead.")

    def base_DoSetClientSize(*args, **kw):
        return PyPanel.DoSetClientSize(*args, **kw)
    base_DoSetClientSize = wx._deprecated(base_DoSetClientSize,
                                   "Please use PyPanel.DoSetClientSize instead.")

    def base_DoSetVirtualSize(*args, **kw):
        return PyPanel.DoSetVirtualSize(*args, **kw)
    base_DoSetVirtualSize = wx._deprecated(base_DoSetVirtualSize,
                                   "Please use PyPanel.DoSetVirtualSize instead.")

    def base_DoGetSize(*args, **kw):
        return PyPanel.DoGetSize(*args, **kw)
    base_DoGetSize = wx._deprecated(base_DoGetSize,
                                   "Please use PyPanel.DoGetSize instead.")

    def base_DoGetClientSize(*args, **kw):
        return PyPanel.DoGetClientSize(*args, **kw)
    base_DoGetClientSize = wx._deprecated(base_DoGetClientSize,
                                   "Please use PyPanel.DoGetClientSize instead.")

    def base_DoGetPosition(*args, **kw):
        return PyPanel.DoGetPosition(*args, **kw)
    base_DoGetPosition = wx._deprecated(base_DoGetPosition,
                                   "Please use PyPanel.DoGetPosition instead.")

    def base_DoGetVirtualSize(*args, **kw):
        return PyPanel.DoGetVirtualSize(*args, **kw)
    base_DoGetVirtualSize = wx._deprecated(base_DoGetVirtualSize,
                                   "Please use PyPanel.DoGetVirtualSize instead.")

    def base_DoGetBestSize(*args, **kw):
        return PyPanel.DoGetBestSize(*args, **kw)
    base_DoGetBestSize = wx._deprecated(base_DoGetBestSize,
                                   "Please use PyPanel.DoGetBestSize instead.")

    def base_InitDialog(*args, **kw):
        return PyPanel.InitDialog(*args, **kw)
    base_InitDialog = wx._deprecated(base_InitDialog,
                                   "Please use PyPanel.InitDialog instead.")

    def base_TransferDataToWindow(*args, **kw):
        return PyPanel.TransferDataToWindow(*args, **kw)
    base_TransferDataToWindow = wx._deprecated(base_TransferDataToWindow,
                                   "Please use PyPanel.TransferDataToWindow instead.")

    def base_TransferDataFromWindow(*args, **kw):
        return PyPanel.TransferDataFromWindow(*args, **kw)
    base_TransferDataFromWindow = wx._deprecated(base_TransferDataFromWindow,
                                   "Please use PyPanel.TransferDataFromWindow instead.")

    def base_Validate(*args, **kw):
        return PyPanel.Validate(*args, **kw)
    base_Validate = wx._deprecated(base_Validate,
                                   "Please use PyPanel.Validate instead.")

    def base_AcceptsFocus(*args, **kw):
        return PyPanel.AcceptsFocus(*args, **kw)
    base_AcceptsFocus = wx._deprecated(base_AcceptsFocus,
                                   "Please use PyPanel.AcceptsFocus instead.")

    def base_AcceptsFocusFromKeyboard(*args, **kw):
        return PyPanel.AcceptsFocusFromKeyboard(*args, **kw)
    base_AcceptsFocusFromKeyboard = wx._deprecated(base_AcceptsFocusFromKeyboard,
                                   "Please use PyPanel.AcceptsFocusFromKeyboard instead.")

    def base_GetMaxSize(*args, **kw):
        return PyPanel.GetMaxSize(*args, **kw)
    base_GetMaxSize = wx._deprecated(base_GetMaxSize,
                                   "Please use PyPanel.GetMaxSize instead.")

    def base_AddChild(*args, **kw):
        return PyPanel.AddChild(*args, **kw)
    base_AddChild = wx._deprecated(base_AddChild,
                                   "Please use PyPanel.AddChild instead.")

    def base_RemoveChild(*args, **kw):
        return PyPanel.RemoveChild(*args, **kw)
    base_RemoveChild = wx._deprecated(base_RemoveChild,
                                   "Please use PyPanel.RemoveChild instead.")

    def base_ShouldInheritColours(*args, **kw):
        return PyPanel.ShouldInheritColours(*args, **kw)
    base_ShouldInheritColours = wx._deprecated(base_ShouldInheritColours,
                                   "Please use PyPanel.ShouldInheritColours instead.")

    def base_GetDefaultAttributes(*args, **kw):
        return PyPanel.GetDefaultAttributes(*args, **kw)
    base_GetDefaultAttributes = wx._deprecated(base_GetDefaultAttributes,
                                   "Please use PyPanel.GetDefaultAttributes instead.")

    def base_OnInternalIdle(*args, **kw):
        return PyPanel.OnInternalIdle(*args, **kw)
    base_OnInternalIdle = wx._deprecated(base_OnInternalIdle,
                                   "Please use PyPanel.OnInternalIdle instead.")

_windows_.PyPanel_swigregister(PyPanel)

def PrePyPanel(*args, **kwargs):
    """PrePyPanel() -> PyPanel"""
    val = _windows_.new_PrePyPanel(*args, **kwargs)
    return val

class PyScrolledWindow(ScrolledWindow):
    """Proxy of C++ PyScrolledWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=PanelNameStr) -> PyScrolledWindow
        """
        _windows_.PyScrolledWindow_swiginit(self,_windows_.new_PyScrolledWindow(*args, **kwargs))
        self._setOORInfo(self);PyScrolledWindow._setCallbackInfo(self, self, PyScrolledWindow)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyScrolledWindow__setCallbackInfo(*args, **kwargs)

    SetBestSize = wx.Window.SetInitialSize 
    def DoEraseBackground(*args, **kwargs):
        """DoEraseBackground(self, DC dc) -> bool"""
        return _windows_.PyScrolledWindow_DoEraseBackground(*args, **kwargs)

    def DoMoveWindow(*args, **kwargs):
        """DoMoveWindow(self, int x, int y, int width, int height)"""
        return _windows_.PyScrolledWindow_DoMoveWindow(*args, **kwargs)

    def DoSetSize(*args, **kwargs):
        """DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _windows_.PyScrolledWindow_DoSetSize(*args, **kwargs)

    def DoSetClientSize(*args, **kwargs):
        """DoSetClientSize(self, int width, int height)"""
        return _windows_.PyScrolledWindow_DoSetClientSize(*args, **kwargs)

    def DoSetVirtualSize(*args, **kwargs):
        """DoSetVirtualSize(self, int x, int y)"""
        return _windows_.PyScrolledWindow_DoSetVirtualSize(*args, **kwargs)

    def DoGetSize(*args, **kwargs):
        """DoGetSize() -> (width, height)"""
        return _windows_.PyScrolledWindow_DoGetSize(*args, **kwargs)

    def DoGetClientSize(*args, **kwargs):
        """DoGetClientSize() -> (width, height)"""
        return _windows_.PyScrolledWindow_DoGetClientSize(*args, **kwargs)

    def DoGetPosition(*args, **kwargs):
        """DoGetPosition() -> (x,y)"""
        return _windows_.PyScrolledWindow_DoGetPosition(*args, **kwargs)

    def DoGetVirtualSize(*args, **kwargs):
        """DoGetVirtualSize(self) -> Size"""
        return _windows_.PyScrolledWindow_DoGetVirtualSize(*args, **kwargs)

    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _windows_.PyScrolledWindow_DoGetBestSize(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """GetDefaultAttributes(self) -> VisualAttributes"""
        return _windows_.PyScrolledWindow_GetDefaultAttributes(*args, **kwargs)

    def OnInternalIdle(*args, **kwargs):
        """OnInternalIdle(self)"""
        return _windows_.PyScrolledWindow_OnInternalIdle(*args, **kwargs)

    def base_DoMoveWindow(*args, **kw):
        return PyScrolledWindow.DoMoveWindow(*args, **kw)
    base_DoMoveWindow = wx._deprecated(base_DoMoveWindow,
                                   "Please use PyScrolledWindow.DoMoveWindow instead.")

    def base_DoSetSize(*args, **kw):
        return PyScrolledWindow.DoSetSize(*args, **kw)
    base_DoSetSize = wx._deprecated(base_DoSetSize,
                                   "Please use PyScrolledWindow.DoSetSize instead.")

    def base_DoSetClientSize(*args, **kw):
        return PyScrolledWindow.DoSetClientSize(*args, **kw)
    base_DoSetClientSize = wx._deprecated(base_DoSetClientSize,
                                   "Please use PyScrolledWindow.DoSetClientSize instead.")

    def base_DoSetVirtualSize(*args, **kw):
        return PyScrolledWindow.DoSetVirtualSize(*args, **kw)
    base_DoSetVirtualSize = wx._deprecated(base_DoSetVirtualSize,
                                   "Please use PyScrolledWindow.DoSetVirtualSize instead.")

    def base_DoGetSize(*args, **kw):
        return PyScrolledWindow.DoGetSize(*args, **kw)
    base_DoGetSize = wx._deprecated(base_DoGetSize,
                                   "Please use PyScrolledWindow.DoGetSize instead.")

    def base_DoGetClientSize(*args, **kw):
        return PyScrolledWindow.DoGetClientSize(*args, **kw)
    base_DoGetClientSize = wx._deprecated(base_DoGetClientSize,
                                   "Please use PyScrolledWindow.DoGetClientSize instead.")

    def base_DoGetPosition(*args, **kw):
        return PyScrolledWindow.DoGetPosition(*args, **kw)
    base_DoGetPosition = wx._deprecated(base_DoGetPosition,
                                   "Please use PyScrolledWindow.DoGetPosition instead.")

    def base_DoGetVirtualSize(*args, **kw):
        return PyScrolledWindow.DoGetVirtualSize(*args, **kw)
    base_DoGetVirtualSize = wx._deprecated(base_DoGetVirtualSize,
                                   "Please use PyScrolledWindow.DoGetVirtualSize instead.")

    def base_DoGetBestSize(*args, **kw):
        return PyScrolledWindow.DoGetBestSize(*args, **kw)
    base_DoGetBestSize = wx._deprecated(base_DoGetBestSize,
                                   "Please use PyScrolledWindow.DoGetBestSize instead.")

    def base_InitDialog(*args, **kw):
        return PyScrolledWindow.InitDialog(*args, **kw)
    base_InitDialog = wx._deprecated(base_InitDialog,
                                   "Please use PyScrolledWindow.InitDialog instead.")

    def base_TransferDataToWindow(*args, **kw):
        return PyScrolledWindow.TransferDataToWindow(*args, **kw)
    base_TransferDataToWindow = wx._deprecated(base_TransferDataToWindow,
                                   "Please use PyScrolledWindow.TransferDataToWindow instead.")

    def base_TransferDataFromWindow(*args, **kw):
        return PyScrolledWindow.TransferDataFromWindow(*args, **kw)
    base_TransferDataFromWindow = wx._deprecated(base_TransferDataFromWindow,
                                   "Please use PyScrolledWindow.TransferDataFromWindow instead.")

    def base_Validate(*args, **kw):
        return PyScrolledWindow.Validate(*args, **kw)
    base_Validate = wx._deprecated(base_Validate,
                                   "Please use PyScrolledWindow.Validate instead.")

    def base_AcceptsFocus(*args, **kw):
        return PyScrolledWindow.AcceptsFocus(*args, **kw)
    base_AcceptsFocus = wx._deprecated(base_AcceptsFocus,
                                   "Please use PyScrolledWindow.AcceptsFocus instead.")

    def base_AcceptsFocusFromKeyboard(*args, **kw):
        return PyScrolledWindow.AcceptsFocusFromKeyboard(*args, **kw)
    base_AcceptsFocusFromKeyboard = wx._deprecated(base_AcceptsFocusFromKeyboard,
                                   "Please use PyScrolledWindow.AcceptsFocusFromKeyboard instead.")

    def base_GetMaxSize(*args, **kw):
        return PyScrolledWindow.GetMaxSize(*args, **kw)
    base_GetMaxSize = wx._deprecated(base_GetMaxSize,
                                   "Please use PyScrolledWindow.GetMaxSize instead.")

    def base_AddChild(*args, **kw):
        return PyScrolledWindow.AddChild(*args, **kw)
    base_AddChild = wx._deprecated(base_AddChild,
                                   "Please use PyScrolledWindow.AddChild instead.")

    def base_RemoveChild(*args, **kw):
        return PyScrolledWindow.RemoveChild(*args, **kw)
    base_RemoveChild = wx._deprecated(base_RemoveChild,
                                   "Please use PyScrolledWindow.RemoveChild instead.")

    def base_ShouldInheritColours(*args, **kw):
        return PyScrolledWindow.ShouldInheritColours(*args, **kw)
    base_ShouldInheritColours = wx._deprecated(base_ShouldInheritColours,
                                   "Please use PyScrolledWindow.ShouldInheritColours instead.")

    def base_GetDefaultAttributes(*args, **kw):
        return PyScrolledWindow.GetDefaultAttributes(*args, **kw)
    base_GetDefaultAttributes = wx._deprecated(base_GetDefaultAttributes,
                                   "Please use PyScrolledWindow.GetDefaultAttributes instead.")

    def base_OnInternalIdle(*args, **kw):
        return PyScrolledWindow.OnInternalIdle(*args, **kw)
    base_OnInternalIdle = wx._deprecated(base_OnInternalIdle,
                                   "Please use PyScrolledWindow.OnInternalIdle instead.")

_windows_.PyScrolledWindow_swigregister(PyScrolledWindow)

def PrePyScrolledWindow(*args, **kwargs):
    """PrePyScrolledWindow() -> PyScrolledWindow"""
    val = _windows_.new_PrePyScrolledWindow(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

PRINT_MODE_NONE = _windows_.PRINT_MODE_NONE
PRINT_MODE_PREVIEW = _windows_.PRINT_MODE_PREVIEW
PRINT_MODE_FILE = _windows_.PRINT_MODE_FILE
PRINT_MODE_PRINTER = _windows_.PRINT_MODE_PRINTER
PRINT_MODE_STREAM = _windows_.PRINT_MODE_STREAM
PRINTBIN_DEFAULT = _windows_.PRINTBIN_DEFAULT
PRINTBIN_ONLYONE = _windows_.PRINTBIN_ONLYONE
PRINTBIN_LOWER = _windows_.PRINTBIN_LOWER
PRINTBIN_MIDDLE = _windows_.PRINTBIN_MIDDLE
PRINTBIN_MANUAL = _windows_.PRINTBIN_MANUAL
PRINTBIN_ENVELOPE = _windows_.PRINTBIN_ENVELOPE
PRINTBIN_ENVMANUAL = _windows_.PRINTBIN_ENVMANUAL
PRINTBIN_AUTO = _windows_.PRINTBIN_AUTO
PRINTBIN_TRACTOR = _windows_.PRINTBIN_TRACTOR
PRINTBIN_SMALLFMT = _windows_.PRINTBIN_SMALLFMT
PRINTBIN_LARGEFMT = _windows_.PRINTBIN_LARGEFMT
PRINTBIN_LARGECAPACITY = _windows_.PRINTBIN_LARGECAPACITY
PRINTBIN_CASSETTE = _windows_.PRINTBIN_CASSETTE
PRINTBIN_FORMSOURCE = _windows_.PRINTBIN_FORMSOURCE
PRINTBIN_USER = _windows_.PRINTBIN_USER
class PrintData(_core.Object):
    """Proxy of C++ PrintData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self) -> PrintData
        __init__(self, PrintData data) -> PrintData
        """
        _windows_.PrintData_swiginit(self,_windows_.new_PrintData(*args))
    __swig_destroy__ = _windows_.delete_PrintData
    __del__ = lambda self : None;
    def GetNoCopies(*args, **kwargs):
        """GetNoCopies(self) -> int"""
        return _windows_.PrintData_GetNoCopies(*args, **kwargs)

    def GetCollate(*args, **kwargs):
        """GetCollate(self) -> bool"""
        return _windows_.PrintData_GetCollate(*args, **kwargs)

    def GetOrientation(*args, **kwargs):
        """GetOrientation(self) -> int"""
        return _windows_.PrintData_GetOrientation(*args, **kwargs)

    def IsOrientationReversed(*args, **kwargs):
        """IsOrientationReversed(self) -> bool"""
        return _windows_.PrintData_IsOrientationReversed(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _windows_.PrintData_IsOk(*args, **kwargs)

    Ok = IsOk 
    def GetPrinterName(*args, **kwargs):
        """GetPrinterName(self) -> String"""
        return _windows_.PrintData_GetPrinterName(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour(self) -> bool"""
        return _windows_.PrintData_GetColour(*args, **kwargs)

    def GetDuplex(*args, **kwargs):
        """GetDuplex(self) -> int"""
        return _windows_.PrintData_GetDuplex(*args, **kwargs)

    def GetPaperId(*args, **kwargs):
        """GetPaperId(self) -> int"""
        return _windows_.PrintData_GetPaperId(*args, **kwargs)

    def GetPaperSize(*args, **kwargs):
        """GetPaperSize(self) -> Size"""
        return _windows_.PrintData_GetPaperSize(*args, **kwargs)

    def GetQuality(*args, **kwargs):
        """GetQuality(self) -> int"""
        return _windows_.PrintData_GetQuality(*args, **kwargs)

    def GetBin(*args, **kwargs):
        """GetBin(self) -> int"""
        return _windows_.PrintData_GetBin(*args, **kwargs)

    def GetPrintMode(*args, **kwargs):
        """GetPrintMode(self) -> int"""
        return _windows_.PrintData_GetPrintMode(*args, **kwargs)

    def GetMedia(*args, **kwargs):
        """GetMedia(self) -> int"""
        return _windows_.PrintData_GetMedia(*args, **kwargs)

    def SetNoCopies(*args, **kwargs):
        """SetNoCopies(self, int v)"""
        return _windows_.PrintData_SetNoCopies(*args, **kwargs)

    def SetCollate(*args, **kwargs):
        """SetCollate(self, bool flag)"""
        return _windows_.PrintData_SetCollate(*args, **kwargs)

    def SetOrientation(*args, **kwargs):
        """SetOrientation(self, int orient)"""
        return _windows_.PrintData_SetOrientation(*args, **kwargs)

    def SetOrientationReversed(*args, **kwargs):
        """SetOrientationReversed(self, bool reversed)"""
        return _windows_.PrintData_SetOrientationReversed(*args, **kwargs)

    def SetPrinterName(*args, **kwargs):
        """SetPrinterName(self, String name)"""
        return _windows_.PrintData_SetPrinterName(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(self, bool colour)"""
        return _windows_.PrintData_SetColour(*args, **kwargs)

    def SetDuplex(*args, **kwargs):
        """SetDuplex(self, int duplex)"""
        return _windows_.PrintData_SetDuplex(*args, **kwargs)

    def SetPaperId(*args, **kwargs):
        """SetPaperId(self, int sizeId)"""
        return _windows_.PrintData_SetPaperId(*args, **kwargs)

    def SetPaperSize(*args, **kwargs):
        """SetPaperSize(self, Size sz)"""
        return _windows_.PrintData_SetPaperSize(*args, **kwargs)

    def SetQuality(*args, **kwargs):
        """SetQuality(self, int quality)"""
        return _windows_.PrintData_SetQuality(*args, **kwargs)

    def SetBin(*args, **kwargs):
        """SetBin(self, int bin)"""
        return _windows_.PrintData_SetBin(*args, **kwargs)

    def SetPrintMode(*args, **kwargs):
        """SetPrintMode(self, int printMode)"""
        return _windows_.PrintData_SetPrintMode(*args, **kwargs)

    def SetMedia(*args, **kwargs):
        """SetMedia(self, int media)"""
        return _windows_.PrintData_SetMedia(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename(self) -> String"""
        return _windows_.PrintData_GetFilename(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(self, String filename)"""
        return _windows_.PrintData_SetFilename(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def GetPrivData(*args, **kwargs):
        """GetPrivData(self) -> PyObject"""
        return _windows_.PrintData_GetPrivData(*args, **kwargs)

    def SetPrivData(*args, **kwargs):
        """SetPrivData(self, PyObject data)"""
        return _windows_.PrintData_SetPrivData(*args, **kwargs)

    Bin = property(GetBin,SetBin,doc="See `GetBin` and `SetBin`") 
    Collate = property(GetCollate,SetCollate,doc="See `GetCollate` and `SetCollate`") 
    Colour = property(GetColour,SetColour,doc="See `GetColour` and `SetColour`") 
    Duplex = property(GetDuplex,SetDuplex,doc="See `GetDuplex` and `SetDuplex`") 
    Filename = property(GetFilename,SetFilename,doc="See `GetFilename` and `SetFilename`") 
    NoCopies = property(GetNoCopies,SetNoCopies,doc="See `GetNoCopies` and `SetNoCopies`") 
    Orientation = property(GetOrientation,SetOrientation,doc="See `GetOrientation` and `SetOrientation`") 
    PaperId = property(GetPaperId,SetPaperId,doc="See `GetPaperId` and `SetPaperId`") 
    PaperSize = property(GetPaperSize,SetPaperSize,doc="See `GetPaperSize` and `SetPaperSize`") 
    PrintMode = property(GetPrintMode,SetPrintMode,doc="See `GetPrintMode` and `SetPrintMode`") 
    PrinterName = property(GetPrinterName,SetPrinterName,doc="See `GetPrinterName` and `SetPrinterName`") 
    PrivData = property(GetPrivData,SetPrivData,doc="See `GetPrivData` and `SetPrivData`") 
    Quality = property(GetQuality,SetQuality,doc="See `GetQuality` and `SetQuality`") 
_windows_.PrintData_swigregister(PrintData)
PrintoutTitleStr = cvar.PrintoutTitleStr
PreviewCanvasNameStr = cvar.PreviewCanvasNameStr

class PageSetupDialogData(_core.Object):
    """Proxy of C++ PageSetupDialogData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self) -> PageSetupDialogData
        __init__(self, PageSetupDialogData data) -> PageSetupDialogData
        __init__(self, PrintData data) -> PageSetupDialogData
        """
        _windows_.PageSetupDialogData_swiginit(self,_windows_.new_PageSetupDialogData(*args))
    __swig_destroy__ = _windows_.delete_PageSetupDialogData
    __del__ = lambda self : None;
    def EnableHelp(*args, **kwargs):
        """EnableHelp(self, bool flag)"""
        return _windows_.PageSetupDialogData_EnableHelp(*args, **kwargs)

    def EnableMargins(*args, **kwargs):
        """EnableMargins(self, bool flag)"""
        return _windows_.PageSetupDialogData_EnableMargins(*args, **kwargs)

    def EnableOrientation(*args, **kwargs):
        """EnableOrientation(self, bool flag)"""
        return _windows_.PageSetupDialogData_EnableOrientation(*args, **kwargs)

    def EnablePaper(*args, **kwargs):
        """EnablePaper(self, bool flag)"""
        return _windows_.PageSetupDialogData_EnablePaper(*args, **kwargs)

    def EnablePrinter(*args, **kwargs):
        """EnablePrinter(self, bool flag)"""
        return _windows_.PageSetupDialogData_EnablePrinter(*args, **kwargs)

    def GetDefaultMinMargins(*args, **kwargs):
        """GetDefaultMinMargins(self) -> bool"""
        return _windows_.PageSetupDialogData_GetDefaultMinMargins(*args, **kwargs)

    def GetEnableMargins(*args, **kwargs):
        """GetEnableMargins(self) -> bool"""
        return _windows_.PageSetupDialogData_GetEnableMargins(*args, **kwargs)

    def GetEnableOrientation(*args, **kwargs):
        """GetEnableOrientation(self) -> bool"""
        return _windows_.PageSetupDialogData_GetEnableOrientation(*args, **kwargs)

    def GetEnablePaper(*args, **kwargs):
        """GetEnablePaper(self) -> bool"""
        return _windows_.PageSetupDialogData_GetEnablePaper(*args, **kwargs)

    def GetEnablePrinter(*args, **kwargs):
        """GetEnablePrinter(self) -> bool"""
        return _windows_.PageSetupDialogData_GetEnablePrinter(*args, **kwargs)

    def GetEnableHelp(*args, **kwargs):
        """GetEnableHelp(self) -> bool"""
        return _windows_.PageSetupDialogData_GetEnableHelp(*args, **kwargs)

    def GetDefaultInfo(*args, **kwargs):
        """GetDefaultInfo(self) -> bool"""
        return _windows_.PageSetupDialogData_GetDefaultInfo(*args, **kwargs)

    def GetMarginTopLeft(*args, **kwargs):
        """GetMarginTopLeft(self) -> Point"""
        return _windows_.PageSetupDialogData_GetMarginTopLeft(*args, **kwargs)

    def GetMarginBottomRight(*args, **kwargs):
        """GetMarginBottomRight(self) -> Point"""
        return _windows_.PageSetupDialogData_GetMarginBottomRight(*args, **kwargs)

    def GetMinMarginTopLeft(*args, **kwargs):
        """GetMinMarginTopLeft(self) -> Point"""
        return _windows_.PageSetupDialogData_GetMinMarginTopLeft(*args, **kwargs)

    def GetMinMarginBottomRight(*args, **kwargs):
        """GetMinMarginBottomRight(self) -> Point"""
        return _windows_.PageSetupDialogData_GetMinMarginBottomRight(*args, **kwargs)

    def GetPaperId(*args, **kwargs):
        """GetPaperId(self) -> int"""
        return _windows_.PageSetupDialogData_GetPaperId(*args, **kwargs)

    def GetPaperSize(*args, **kwargs):
        """GetPaperSize(self) -> Size"""
        return _windows_.PageSetupDialogData_GetPaperSize(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData(self) -> PrintData"""
        return _windows_.PageSetupDialogData_GetPrintData(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _windows_.PageSetupDialogData_IsOk(*args, **kwargs)

    Ok = IsOk 
    def SetDefaultInfo(*args, **kwargs):
        """SetDefaultInfo(self, bool flag)"""
        return _windows_.PageSetupDialogData_SetDefaultInfo(*args, **kwargs)

    def SetDefaultMinMargins(*args, **kwargs):
        """SetDefaultMinMargins(self, bool flag)"""
        return _windows_.PageSetupDialogData_SetDefaultMinMargins(*args, **kwargs)

    def SetMarginTopLeft(*args, **kwargs):
        """SetMarginTopLeft(self, Point pt)"""
        return _windows_.PageSetupDialogData_SetMarginTopLeft(*args, **kwargs)

    def SetMarginBottomRight(*args, **kwargs):
        """SetMarginBottomRight(self, Point pt)"""
        return _windows_.PageSetupDialogData_SetMarginBottomRight(*args, **kwargs)

    def SetMinMarginTopLeft(*args, **kwargs):
        """SetMinMarginTopLeft(self, Point pt)"""
        return _windows_.PageSetupDialogData_SetMinMarginTopLeft(*args, **kwargs)

    def SetMinMarginBottomRight(*args, **kwargs):
        """SetMinMarginBottomRight(self, Point pt)"""
        return _windows_.PageSetupDialogData_SetMinMarginBottomRight(*args, **kwargs)

    def SetPaperId(*args, **kwargs):
        """SetPaperId(self, int id)"""
        return _windows_.PageSetupDialogData_SetPaperId(*args, **kwargs)

    def SetPaperSize(*args, **kwargs):
        """SetPaperSize(self, Size size)"""
        return _windows_.PageSetupDialogData_SetPaperSize(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(self, PrintData printData)"""
        return _windows_.PageSetupDialogData_SetPrintData(*args, **kwargs)

    def CalculateIdFromPaperSize(*args, **kwargs):
        """CalculateIdFromPaperSize(self)"""
        return _windows_.PageSetupDialogData_CalculateIdFromPaperSize(*args, **kwargs)

    def CalculatePaperSizeFromId(*args, **kwargs):
        """CalculatePaperSizeFromId(self)"""
        return _windows_.PageSetupDialogData_CalculatePaperSizeFromId(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    DefaultInfo = property(GetDefaultInfo,SetDefaultInfo,doc="See `GetDefaultInfo` and `SetDefaultInfo`") 
    DefaultMinMargins = property(GetDefaultMinMargins,SetDefaultMinMargins,doc="See `GetDefaultMinMargins` and `SetDefaultMinMargins`") 
    MarginBottomRight = property(GetMarginBottomRight,SetMarginBottomRight,doc="See `GetMarginBottomRight` and `SetMarginBottomRight`") 
    MarginTopLeft = property(GetMarginTopLeft,SetMarginTopLeft,doc="See `GetMarginTopLeft` and `SetMarginTopLeft`") 
    MinMarginBottomRight = property(GetMinMarginBottomRight,SetMinMarginBottomRight,doc="See `GetMinMarginBottomRight` and `SetMinMarginBottomRight`") 
    MinMarginTopLeft = property(GetMinMarginTopLeft,SetMinMarginTopLeft,doc="See `GetMinMarginTopLeft` and `SetMinMarginTopLeft`") 
    PaperId = property(GetPaperId,SetPaperId,doc="See `GetPaperId` and `SetPaperId`") 
    PaperSize = property(GetPaperSize,SetPaperSize,doc="See `GetPaperSize` and `SetPaperSize`") 
    PrintData = property(GetPrintData,SetPrintData,doc="See `GetPrintData` and `SetPrintData`") 
_windows_.PageSetupDialogData_swigregister(PageSetupDialogData)

class PageSetupDialog(_core.Object):
    """Proxy of C++ PageSetupDialog class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window parent, PageSetupDialogData data=None) -> PageSetupDialog"""
        _windows_.PageSetupDialog_swiginit(self,_windows_.new_PageSetupDialog(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_PageSetupDialog
    __del__ = lambda self : None;
    def GetPageSetupData(*args, **kwargs):
        """GetPageSetupData(self) -> PageSetupDialogData"""
        return _windows_.PageSetupDialog_GetPageSetupData(*args, **kwargs)

    def GetPageSetupDialogData(*args, **kwargs):
        """GetPageSetupDialogData(self) -> PageSetupDialogData"""
        return _windows_.PageSetupDialog_GetPageSetupDialogData(*args, **kwargs)

    def ShowModal(*args, **kwargs):
        """ShowModal(self) -> int"""
        return _windows_.PageSetupDialog_ShowModal(*args, **kwargs)

    def Destroy(self): pass 
    PageSetupData = property(GetPageSetupData,doc="See `GetPageSetupData`") 
    PageSetupDialogData = property(GetPageSetupDialogData,doc="See `GetPageSetupDialogData`") 
_windows_.PageSetupDialog_swigregister(PageSetupDialog)

class PrintDialogData(_core.Object):
    """Proxy of C++ PrintDialogData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self) -> PrintDialogData
        __init__(self, PrintData printData) -> PrintDialogData
        __init__(self, PrintDialogData printData) -> PrintDialogData
        """
        _windows_.PrintDialogData_swiginit(self,_windows_.new_PrintDialogData(*args))
    __swig_destroy__ = _windows_.delete_PrintDialogData
    __del__ = lambda self : None;
    def GetFromPage(*args, **kwargs):
        """GetFromPage(self) -> int"""
        return _windows_.PrintDialogData_GetFromPage(*args, **kwargs)

    def GetToPage(*args, **kwargs):
        """GetToPage(self) -> int"""
        return _windows_.PrintDialogData_GetToPage(*args, **kwargs)

    def GetMinPage(*args, **kwargs):
        """GetMinPage(self) -> int"""
        return _windows_.PrintDialogData_GetMinPage(*args, **kwargs)

    def GetMaxPage(*args, **kwargs):
        """GetMaxPage(self) -> int"""
        return _windows_.PrintDialogData_GetMaxPage(*args, **kwargs)

    def GetNoCopies(*args, **kwargs):
        """GetNoCopies(self) -> int"""
        return _windows_.PrintDialogData_GetNoCopies(*args, **kwargs)

    def GetAllPages(*args, **kwargs):
        """GetAllPages(self) -> bool"""
        return _windows_.PrintDialogData_GetAllPages(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> bool"""
        return _windows_.PrintDialogData_GetSelection(*args, **kwargs)

    def GetCollate(*args, **kwargs):
        """GetCollate(self) -> bool"""
        return _windows_.PrintDialogData_GetCollate(*args, **kwargs)

    def GetPrintToFile(*args, **kwargs):
        """GetPrintToFile(self) -> bool"""
        return _windows_.PrintDialogData_GetPrintToFile(*args, **kwargs)

    def SetFromPage(*args, **kwargs):
        """SetFromPage(self, int v)"""
        return _windows_.PrintDialogData_SetFromPage(*args, **kwargs)

    def SetToPage(*args, **kwargs):
        """SetToPage(self, int v)"""
        return _windows_.PrintDialogData_SetToPage(*args, **kwargs)

    def SetMinPage(*args, **kwargs):
        """SetMinPage(self, int v)"""
        return _windows_.PrintDialogData_SetMinPage(*args, **kwargs)

    def SetMaxPage(*args, **kwargs):
        """SetMaxPage(self, int v)"""
        return _windows_.PrintDialogData_SetMaxPage(*args, **kwargs)

    def SetNoCopies(*args, **kwargs):
        """SetNoCopies(self, int v)"""
        return _windows_.PrintDialogData_SetNoCopies(*args, **kwargs)

    def SetAllPages(*args, **kwargs):
        """SetAllPages(self, bool flag)"""
        return _windows_.PrintDialogData_SetAllPages(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, bool flag)"""
        return _windows_.PrintDialogData_SetSelection(*args, **kwargs)

    def SetCollate(*args, **kwargs):
        """SetCollate(self, bool flag)"""
        return _windows_.PrintDialogData_SetCollate(*args, **kwargs)

    def SetPrintToFile(*args, **kwargs):
        """SetPrintToFile(self, bool flag)"""
        return _windows_.PrintDialogData_SetPrintToFile(*args, **kwargs)

    def EnablePrintToFile(*args, **kwargs):
        """EnablePrintToFile(self, bool flag)"""
        return _windows_.PrintDialogData_EnablePrintToFile(*args, **kwargs)

    def EnableSelection(*args, **kwargs):
        """EnableSelection(self, bool flag)"""
        return _windows_.PrintDialogData_EnableSelection(*args, **kwargs)

    def EnablePageNumbers(*args, **kwargs):
        """EnablePageNumbers(self, bool flag)"""
        return _windows_.PrintDialogData_EnablePageNumbers(*args, **kwargs)

    def EnableHelp(*args, **kwargs):
        """EnableHelp(self, bool flag)"""
        return _windows_.PrintDialogData_EnableHelp(*args, **kwargs)

    def GetEnablePrintToFile(*args, **kwargs):
        """GetEnablePrintToFile(self) -> bool"""
        return _windows_.PrintDialogData_GetEnablePrintToFile(*args, **kwargs)

    def GetEnableSelection(*args, **kwargs):
        """GetEnableSelection(self) -> bool"""
        return _windows_.PrintDialogData_GetEnableSelection(*args, **kwargs)

    def GetEnablePageNumbers(*args, **kwargs):
        """GetEnablePageNumbers(self) -> bool"""
        return _windows_.PrintDialogData_GetEnablePageNumbers(*args, **kwargs)

    def GetEnableHelp(*args, **kwargs):
        """GetEnableHelp(self) -> bool"""
        return _windows_.PrintDialogData_GetEnableHelp(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _windows_.PrintDialogData_IsOk(*args, **kwargs)

    Ok = IsOk 
    def GetPrintData(*args, **kwargs):
        """GetPrintData(self) -> PrintData"""
        return _windows_.PrintDialogData_GetPrintData(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(self, PrintData printData)"""
        return _windows_.PrintDialogData_SetPrintData(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    AllPages = property(GetAllPages,SetAllPages,doc="See `GetAllPages` and `SetAllPages`") 
    Collate = property(GetCollate,SetCollate,doc="See `GetCollate` and `SetCollate`") 
    FromPage = property(GetFromPage,SetFromPage,doc="See `GetFromPage` and `SetFromPage`") 
    MaxPage = property(GetMaxPage,SetMaxPage,doc="See `GetMaxPage` and `SetMaxPage`") 
    MinPage = property(GetMinPage,SetMinPage,doc="See `GetMinPage` and `SetMinPage`") 
    NoCopies = property(GetNoCopies,SetNoCopies,doc="See `GetNoCopies` and `SetNoCopies`") 
    PrintData = property(GetPrintData,SetPrintData,doc="See `GetPrintData` and `SetPrintData`") 
    PrintToFile = property(GetPrintToFile,SetPrintToFile,doc="See `GetPrintToFile` and `SetPrintToFile`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    ToPage = property(GetToPage,SetToPage,doc="See `GetToPage` and `SetToPage`") 
_windows_.PrintDialogData_swigregister(PrintDialogData)

class PrintDialog(_core.Object):
    """Proxy of C++ PrintDialog class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window parent, PrintDialogData data=None) -> PrintDialog"""
        _windows_.PrintDialog_swiginit(self,_windows_.new_PrintDialog(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_PrintDialog
    __del__ = lambda self : None;
    def ShowModal(*args, **kwargs):
        """ShowModal(self) -> int"""
        return _windows_.PrintDialog_ShowModal(*args, **kwargs)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData(self) -> PrintDialogData"""
        return _windows_.PrintDialog_GetPrintDialogData(*args, **kwargs)

    def GetPrintData(*args, **kwargs):
        """GetPrintData(self) -> PrintData"""
        return _windows_.PrintDialog_GetPrintData(*args, **kwargs)

    def GetPrintDC(*args, **kwargs):
        """GetPrintDC(self) -> DC"""
        return _windows_.PrintDialog_GetPrintDC(*args, **kwargs)

    def Destroy(self): pass 
    PrintDC = property(GetPrintDC,doc="See `GetPrintDC`") 
    PrintData = property(GetPrintData,doc="See `GetPrintData`") 
    PrintDialogData = property(GetPrintDialogData,doc="See `GetPrintDialogData`") 
_windows_.PrintDialog_swigregister(PrintDialog)

PRINTER_NO_ERROR = _windows_.PRINTER_NO_ERROR
PRINTER_CANCELLED = _windows_.PRINTER_CANCELLED
PRINTER_ERROR = _windows_.PRINTER_ERROR
class Printer(_core.Object):
    """Proxy of C++ Printer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, PrintDialogData data=None) -> Printer"""
        _windows_.Printer_swiginit(self,_windows_.new_Printer(*args, **kwargs))
    __swig_destroy__ = _windows_.delete_Printer
    __del__ = lambda self : None;
    def CreateAbortWindow(*args, **kwargs):
        """CreateAbortWindow(self, Window parent, Printout printout) -> Window"""
        return _windows_.Printer_CreateAbortWindow(*args, **kwargs)

    def ReportError(*args, **kwargs):
        """ReportError(self, Window parent, Printout printout, String message)"""
        return _windows_.Printer_ReportError(*args, **kwargs)

    def Setup(*args, **kwargs):
        """Setup(self, Window parent) -> bool"""
        return _windows_.Printer_Setup(*args, **kwargs)

    def Print(*args, **kwargs):
        """Print(self, Window parent, Printout printout, bool prompt=True) -> bool"""
        return _windows_.Printer_Print(*args, **kwargs)

    def PrintDialog(*args, **kwargs):
        """PrintDialog(self, Window parent) -> DC"""
        return _windows_.Printer_PrintDialog(*args, **kwargs)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData(self) -> PrintDialogData"""
        return _windows_.Printer_GetPrintDialogData(*args, **kwargs)

    def GetAbort(*args, **kwargs):
        """GetAbort(self) -> bool"""
        return _windows_.Printer_GetAbort(*args, **kwargs)

    def GetLastError(*args, **kwargs):
        """GetLastError() -> int"""
        return _windows_.Printer_GetLastError(*args, **kwargs)

    GetLastError = staticmethod(GetLastError)
    Abort = property(GetAbort,doc="See `GetAbort`") 
    PrintDialogData = property(GetPrintDialogData,doc="See `GetPrintDialogData`") 
_windows_.Printer_swigregister(Printer)

def Printer_GetLastError(*args):
  """Printer_GetLastError() -> int"""
  return _windows_.Printer_GetLastError(*args)

class Printout(_core.Object):
    """Proxy of C++ Printout class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String title=PrintoutTitleStr) -> Printout"""
        _windows_.Printout_swiginit(self,_windows_.new_Printout(*args, **kwargs))
        Printout._setCallbackInfo(self, self, Printout)

    __swig_destroy__ = _windows_.delete_Printout
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.Printout__setCallbackInfo(*args, **kwargs)

    def GetTitle(*args, **kwargs):
        """GetTitle(self) -> String"""
        return _windows_.Printout_GetTitle(*args, **kwargs)

    def GetDC(*args, **kwargs):
        """GetDC(self) -> DC"""
        return _windows_.Printout_GetDC(*args, **kwargs)

    def SetDC(*args, **kwargs):
        """SetDC(self, DC dc)"""
        return _windows_.Printout_SetDC(*args, **kwargs)

    def FitThisSizeToPaper(*args, **kwargs):
        """FitThisSizeToPaper(self, Size imageSize)"""
        return _windows_.Printout_FitThisSizeToPaper(*args, **kwargs)

    def FitThisSizeToPage(*args, **kwargs):
        """FitThisSizeToPage(self, Size imageSize)"""
        return _windows_.Printout_FitThisSizeToPage(*args, **kwargs)

    def FitThisSizeToPageMargins(*args, **kwargs):
        """FitThisSizeToPageMargins(self, Size imageSize, PageSetupDialogData pageSetupData)"""
        return _windows_.Printout_FitThisSizeToPageMargins(*args, **kwargs)

    def MapScreenSizeToPaper(*args, **kwargs):
        """MapScreenSizeToPaper(self)"""
        return _windows_.Printout_MapScreenSizeToPaper(*args, **kwargs)

    def MapScreenSizeToPage(*args, **kwargs):
        """MapScreenSizeToPage(self)"""
        return _windows_.Printout_MapScreenSizeToPage(*args, **kwargs)

    def MapScreenSizeToPageMargins(*args, **kwargs):
        """MapScreenSizeToPageMargins(self, PageSetupDialogData pageSetupData)"""
        return _windows_.Printout_MapScreenSizeToPageMargins(*args, **kwargs)

    def MapScreenSizeToDevice(*args, **kwargs):
        """MapScreenSizeToDevice(self)"""
        return _windows_.Printout_MapScreenSizeToDevice(*args, **kwargs)

    def GetLogicalPaperRect(*args, **kwargs):
        """GetLogicalPaperRect(self) -> Rect"""
        return _windows_.Printout_GetLogicalPaperRect(*args, **kwargs)

    def GetLogicalPageRect(*args, **kwargs):
        """GetLogicalPageRect(self) -> Rect"""
        return _windows_.Printout_GetLogicalPageRect(*args, **kwargs)

    def GetLogicalPageMarginsRect(*args, **kwargs):
        """GetLogicalPageMarginsRect(self, PageSetupDialogData pageSetupData) -> Rect"""
        return _windows_.Printout_GetLogicalPageMarginsRect(*args, **kwargs)

    def SetLogicalOrigin(*args, **kwargs):
        """SetLogicalOrigin(self, int x, int y)"""
        return _windows_.Printout_SetLogicalOrigin(*args, **kwargs)

    def OffsetLogicalOrigin(*args, **kwargs):
        """OffsetLogicalOrigin(self, int xoff, int yoff)"""
        return _windows_.Printout_OffsetLogicalOrigin(*args, **kwargs)

    def SetPageSizePixels(*args, **kwargs):
        """SetPageSizePixels(self, int w, int h)"""
        return _windows_.Printout_SetPageSizePixels(*args, **kwargs)

    def GetPageSizePixels(*args, **kwargs):
        """GetPageSizePixels() -> (w, h)"""
        return _windows_.Printout_GetPageSizePixels(*args, **kwargs)

    def SetPageSizeMM(*args, **kwargs):
        """SetPageSizeMM(self, int w, int h)"""
        return _windows_.Printout_SetPageSizeMM(*args, **kwargs)

    def GetPageSizeMM(*args, **kwargs):
        """GetPageSizeMM() -> (w, h)"""
        return _windows_.Printout_GetPageSizeMM(*args, **kwargs)

    def SetPPIScreen(*args, **kwargs):
        """SetPPIScreen(self, int x, int y)"""
        return _windows_.Printout_SetPPIScreen(*args, **kwargs)

    def GetPPIScreen(*args, **kwargs):
        """GetPPIScreen() -> (x,y)"""
        return _windows_.Printout_GetPPIScreen(*args, **kwargs)

    def SetPPIPrinter(*args, **kwargs):
        """SetPPIPrinter(self, int x, int y)"""
        return _windows_.Printout_SetPPIPrinter(*args, **kwargs)

    def GetPPIPrinter(*args, **kwargs):
        """GetPPIPrinter() -> (x,y)"""
        return _windows_.Printout_GetPPIPrinter(*args, **kwargs)

    def SetPaperRectPixels(*args, **kwargs):
        """SetPaperRectPixels(self, Rect paperRectPixels)"""
        return _windows_.Printout_SetPaperRectPixels(*args, **kwargs)

    def GetPaperRectPixels(*args, **kwargs):
        """GetPaperRectPixels(self) -> Rect"""
        return _windows_.Printout_GetPaperRectPixels(*args, **kwargs)

    def IsPreview(*args, **kwargs):
        """IsPreview(self) -> bool"""
        return _windows_.Printout_IsPreview(*args, **kwargs)

    def SetIsPreview(*args, **kwargs):
        """SetIsPreview(self, bool p)"""
        return _windows_.Printout_SetIsPreview(*args, **kwargs)

    def OnBeginDocument(*args, **kwargs):
        """OnBeginDocument(self, int startPage, int endPage) -> bool"""
        return _windows_.Printout_OnBeginDocument(*args, **kwargs)

    def OnEndDocument(*args, **kwargs):
        """OnEndDocument(self)"""
        return _windows_.Printout_OnEndDocument(*args, **kwargs)

    def OnBeginPrinting(*args, **kwargs):
        """OnBeginPrinting(self)"""
        return _windows_.Printout_OnBeginPrinting(*args, **kwargs)

    def OnEndPrinting(*args, **kwargs):
        """OnEndPrinting(self)"""
        return _windows_.Printout_OnEndPrinting(*args, **kwargs)

    def OnPreparePrinting(*args, **kwargs):
        """OnPreparePrinting(self)"""
        return _windows_.Printout_OnPreparePrinting(*args, **kwargs)

    def HasPage(*args, **kwargs):
        """HasPage(self, int page) -> bool"""
        return _windows_.Printout_HasPage(*args, **kwargs)

    def GetPageInfo(*args, **kwargs):
        """GetPageInfo() -> (minPage, maxPage, pageFrom, pageTo)"""
        return _windows_.Printout_GetPageInfo(*args, **kwargs)

    def base_OnBeginDocument(*args, **kw):
        return Printout.OnBeginDocument(*args, **kw)
    base_OnBeginDocument = wx._deprecated(base_OnBeginDocument,
                                   "Please use Printout.OnBeginDocument instead.")

    def base_OnEndDocument(*args, **kw):
        return Printout.OnEndDocument(*args, **kw)
    base_OnEndDocument = wx._deprecated(base_OnEndDocument,
                                   "Please use Printout.OnEndDocument instead.")

    def base_OnBeginPrinting(*args, **kw):
        return Printout.OnBeginPrinting(*args, **kw)
    base_OnBeginPrinting = wx._deprecated(base_OnBeginPrinting,
                                   "Please use Printout.OnBeginPrinting instead.")

    def base_OnEndPrinting(*args, **kw):
        return Printout.OnEndPrinting(*args, **kw)
    base_OnEndPrinting = wx._deprecated(base_OnEndPrinting,
                                   "Please use Printout.OnEndPrinting instead.")

    def base_OnPreparePrinting(*args, **kw):
        return Printout.OnPreparePrinting(*args, **kw)
    base_OnPreparePrinting = wx._deprecated(base_OnPreparePrinting,
                                   "Please use Printout.OnPreparePrinting instead.")

    def base_GetPageInfo(*args, **kw):
        return Printout.GetPageInfo(*args, **kw)
    base_GetPageInfo = wx._deprecated(base_GetPageInfo,
                                   "Please use Printout.GetPageInfo instead.")

    DC = property(GetDC,SetDC,doc="See `GetDC` and `SetDC`") 
    PPIPrinter = property(GetPPIPrinter,SetPPIPrinter,doc="See `GetPPIPrinter` and `SetPPIPrinter`") 
    PPIScreen = property(GetPPIScreen,SetPPIScreen,doc="See `GetPPIScreen` and `SetPPIScreen`") 
    PageSizeMM = property(GetPageSizeMM,SetPageSizeMM,doc="See `GetPageSizeMM` and `SetPageSizeMM`") 
    PageSizePixels = property(GetPageSizePixels,SetPageSizePixels,doc="See `GetPageSizePixels` and `SetPageSizePixels`") 
    Title = property(GetTitle,doc="See `GetTitle`") 
_windows_.Printout_swigregister(Printout)

class PreviewCanvas(ScrolledWindow):
    """Proxy of C++ PreviewCanvas class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, PrintPreview preview, Window parent, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, 
            String name=PreviewCanvasNameStr) -> PreviewCanvas
        """
        _windows_.PreviewCanvas_swiginit(self,_windows_.new_PreviewCanvas(*args, **kwargs))
        self._setOORInfo(self)

_windows_.PreviewCanvas_swigregister(PreviewCanvas)

class PreviewFrame(Frame):
    """Proxy of C++ PreviewFrame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, PrintPreview preview, Frame parent, String title, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, String name=FrameNameStr) -> PreviewFrame
        """
        _windows_.PreviewFrame_swiginit(self,_windows_.new_PreviewFrame(*args, **kwargs))
        self._setOORInfo(self)

    def Initialize(*args, **kwargs):
        """Initialize(self)"""
        return _windows_.PreviewFrame_Initialize(*args, **kwargs)

    def CreateControlBar(*args, **kwargs):
        """CreateControlBar(self)"""
        return _windows_.PreviewFrame_CreateControlBar(*args, **kwargs)

    def CreateCanvas(*args, **kwargs):
        """CreateCanvas(self)"""
        return _windows_.PreviewFrame_CreateCanvas(*args, **kwargs)

    def GetControlBar(*args, **kwargs):
        """GetControlBar(self) -> PreviewControlBar"""
        return _windows_.PreviewFrame_GetControlBar(*args, **kwargs)

    ControlBar = property(GetControlBar,doc="See `GetControlBar`") 
_windows_.PreviewFrame_swigregister(PreviewFrame)

PREVIEW_PRINT = _windows_.PREVIEW_PRINT
PREVIEW_PREVIOUS = _windows_.PREVIEW_PREVIOUS
PREVIEW_NEXT = _windows_.PREVIEW_NEXT
PREVIEW_ZOOM = _windows_.PREVIEW_ZOOM
PREVIEW_FIRST = _windows_.PREVIEW_FIRST
PREVIEW_LAST = _windows_.PREVIEW_LAST
PREVIEW_GOTO = _windows_.PREVIEW_GOTO
PREVIEW_DEFAULT = _windows_.PREVIEW_DEFAULT
ID_PREVIEW_CLOSE = _windows_.ID_PREVIEW_CLOSE
ID_PREVIEW_NEXT = _windows_.ID_PREVIEW_NEXT
ID_PREVIEW_PREVIOUS = _windows_.ID_PREVIEW_PREVIOUS
ID_PREVIEW_PRINT = _windows_.ID_PREVIEW_PRINT
ID_PREVIEW_ZOOM = _windows_.ID_PREVIEW_ZOOM
ID_PREVIEW_FIRST = _windows_.ID_PREVIEW_FIRST
ID_PREVIEW_LAST = _windows_.ID_PREVIEW_LAST
ID_PREVIEW_GOTO = _windows_.ID_PREVIEW_GOTO
class PreviewControlBar(Panel):
    """Proxy of C++ PreviewControlBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, PrintPreview preview, long buttons, Window parent, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=TAB_TRAVERSAL, String name=PanelNameStr) -> PreviewControlBar
        """
        _windows_.PreviewControlBar_swiginit(self,_windows_.new_PreviewControlBar(*args, **kwargs))
        self._setOORInfo(self)

    def GetZoomControl(*args, **kwargs):
        """GetZoomControl(self) -> int"""
        return _windows_.PreviewControlBar_GetZoomControl(*args, **kwargs)

    def SetZoomControl(*args, **kwargs):
        """SetZoomControl(self, int zoom)"""
        return _windows_.PreviewControlBar_SetZoomControl(*args, **kwargs)

    def GetPrintPreview(*args, **kwargs):
        """GetPrintPreview(self) -> PrintPreview"""
        return _windows_.PreviewControlBar_GetPrintPreview(*args, **kwargs)

    def OnNext(*args, **kwargs):
        """OnNext(self)"""
        return _windows_.PreviewControlBar_OnNext(*args, **kwargs)

    def OnPrevious(*args, **kwargs):
        """OnPrevious(self)"""
        return _windows_.PreviewControlBar_OnPrevious(*args, **kwargs)

    def OnFirst(*args, **kwargs):
        """OnFirst(self)"""
        return _windows_.PreviewControlBar_OnFirst(*args, **kwargs)

    def OnLast(*args, **kwargs):
        """OnLast(self)"""
        return _windows_.PreviewControlBar_OnLast(*args, **kwargs)

    def OnGoto(*args, **kwargs):
        """OnGoto(self)"""
        return _windows_.PreviewControlBar_OnGoto(*args, **kwargs)

    PrintPreview = property(GetPrintPreview,doc="See `GetPrintPreview`") 
    ZoomControl = property(GetZoomControl,SetZoomControl,doc="See `GetZoomControl` and `SetZoomControl`") 
_windows_.PreviewControlBar_swigregister(PreviewControlBar)

class PrintPreview(_core.Object):
    """Proxy of C++ PrintPreview class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, Printout printout, Printout printoutForPrinting, PrintDialogData data=None) -> PrintPreview
        __init__(self, Printout printout, Printout printoutForPrinting, PrintData data) -> PrintPreview
        """
        _windows_.PrintPreview_swiginit(self,_windows_.new_PrintPreview(*args))
    __swig_destroy__ = _windows_.delete_PrintPreview
    __del__ = lambda self : None;
    def SetCurrentPage(*args, **kwargs):
        """SetCurrentPage(self, int pageNum) -> bool"""
        return _windows_.PrintPreview_SetCurrentPage(*args, **kwargs)

    def GetCurrentPage(*args, **kwargs):
        """GetCurrentPage(self) -> int"""
        return _windows_.PrintPreview_GetCurrentPage(*args, **kwargs)

    def SetPrintout(*args, **kwargs):
        """SetPrintout(self, Printout printout)"""
        return _windows_.PrintPreview_SetPrintout(*args, **kwargs)

    def GetPrintout(*args, **kwargs):
        """GetPrintout(self) -> Printout"""
        return _windows_.PrintPreview_GetPrintout(*args, **kwargs)

    def GetPrintoutForPrinting(*args, **kwargs):
        """GetPrintoutForPrinting(self) -> Printout"""
        return _windows_.PrintPreview_GetPrintoutForPrinting(*args, **kwargs)

    def SetFrame(*args, **kwargs):
        """SetFrame(self, Frame frame)"""
        return _windows_.PrintPreview_SetFrame(*args, **kwargs)

    def SetCanvas(*args, **kwargs):
        """SetCanvas(self, PreviewCanvas canvas)"""
        return _windows_.PrintPreview_SetCanvas(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self) -> Frame"""
        return _windows_.PrintPreview_GetFrame(*args, **kwargs)

    def GetCanvas(*args, **kwargs):
        """GetCanvas(self) -> PreviewCanvas"""
        return _windows_.PrintPreview_GetCanvas(*args, **kwargs)

    def PaintPage(*args, **kwargs):
        """PaintPage(self, PreviewCanvas canvas, DC dc) -> bool"""
        return _windows_.PrintPreview_PaintPage(*args, **kwargs)

    def DrawBlankPage(*args, **kwargs):
        """DrawBlankPage(self, PreviewCanvas canvas, DC dc) -> bool"""
        return _windows_.PrintPreview_DrawBlankPage(*args, **kwargs)

    def RenderPage(*args, **kwargs):
        """RenderPage(self, int pageNum) -> bool"""
        return _windows_.PrintPreview_RenderPage(*args, **kwargs)

    def AdjustScrollbars(*args, **kwargs):
        """AdjustScrollbars(self, PreviewCanvas canvas)"""
        return _windows_.PrintPreview_AdjustScrollbars(*args, **kwargs)

    def GetPrintDialogData(*args, **kwargs):
        """GetPrintDialogData(self) -> PrintDialogData"""
        return _windows_.PrintPreview_GetPrintDialogData(*args, **kwargs)

    def SetZoom(*args, **kwargs):
        """SetZoom(self, int percent)"""
        return _windows_.PrintPreview_SetZoom(*args, **kwargs)

    def GetZoom(*args, **kwargs):
        """GetZoom(self) -> int"""
        return _windows_.PrintPreview_GetZoom(*args, **kwargs)

    def GetMaxPage(*args, **kwargs):
        """GetMaxPage(self) -> int"""
        return _windows_.PrintPreview_GetMaxPage(*args, **kwargs)

    def GetMinPage(*args, **kwargs):
        """GetMinPage(self) -> int"""
        return _windows_.PrintPreview_GetMinPage(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _windows_.PrintPreview_IsOk(*args, **kwargs)

    Ok = IsOk 
    def SetOk(*args, **kwargs):
        """SetOk(self, bool ok)"""
        return _windows_.PrintPreview_SetOk(*args, **kwargs)

    def Print(*args, **kwargs):
        """Print(self, bool interactive) -> bool"""
        return _windows_.PrintPreview_Print(*args, **kwargs)

    def DetermineScaling(*args, **kwargs):
        """DetermineScaling(self)"""
        return _windows_.PrintPreview_DetermineScaling(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    Canvas = property(GetCanvas,SetCanvas,doc="See `GetCanvas` and `SetCanvas`") 
    CurrentPage = property(GetCurrentPage,SetCurrentPage,doc="See `GetCurrentPage` and `SetCurrentPage`") 
    Frame = property(GetFrame,SetFrame,doc="See `GetFrame` and `SetFrame`") 
    MaxPage = property(GetMaxPage,doc="See `GetMaxPage`") 
    MinPage = property(GetMinPage,doc="See `GetMinPage`") 
    PrintDialogData = property(GetPrintDialogData,doc="See `GetPrintDialogData`") 
    Printout = property(GetPrintout,SetPrintout,doc="See `GetPrintout` and `SetPrintout`") 
    PrintoutForPrinting = property(GetPrintoutForPrinting,doc="See `GetPrintoutForPrinting`") 
    Zoom = property(GetZoom,SetZoom,doc="See `GetZoom` and `SetZoom`") 
_windows_.PrintPreview_swigregister(PrintPreview)

class PyPrintPreview(PrintPreview):
    """Proxy of C++ PyPrintPreview class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, Printout printout, Printout printoutForPrinting, PrintDialogData data=None) -> PyPrintPreview
        __init__(self, Printout printout, Printout printoutForPrinting, PrintData data) -> PyPrintPreview
        """
        _windows_.PyPrintPreview_swiginit(self,_windows_.new_PyPrintPreview(*args))
        PyPrintPreview._setCallbackInfo(self, self, PyPrintPreview)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyPrintPreview__setCallbackInfo(*args, **kwargs)

    def base_SetCurrentPage(*args, **kw):
        return PyPrintPreview.SetCurrentPage(*args, **kw)
    base_SetCurrentPage = wx._deprecated(base_SetCurrentPage,
                                   "Please use PyPrintPreview.SetCurrentPage instead.")

    def base_PaintPage(*args, **kw):
        return PyPrintPreview.PaintPage(*args, **kw)
    base_PaintPage = wx._deprecated(base_PaintPage,
                                   "Please use PyPrintPreview.PaintPage instead.")

    def base_DrawBlankPage(*args, **kw):
        return PyPrintPreview.DrawBlankPage(*args, **kw)
    base_DrawBlankPage = wx._deprecated(base_DrawBlankPage,
                                   "Please use PyPrintPreview.DrawBlankPage instead.")

    def base_RenderPage(*args, **kw):
        return PyPrintPreview.RenderPage(*args, **kw)
    base_RenderPage = wx._deprecated(base_RenderPage,
                                   "Please use PyPrintPreview.RenderPage instead.")

    def base_SetZoom(*args, **kw):
        return PyPrintPreview.SetZoom(*args, **kw)
    base_SetZoom = wx._deprecated(base_SetZoom,
                                   "Please use PyPrintPreview.SetZoom instead.")

    def base_Print(*args, **kw):
        return PyPrintPreview.Print(*args, **kw)
    base_Print = wx._deprecated(base_Print,
                                   "Please use PyPrintPreview.Print instead.")

    def base_DetermineScaling(*args, **kw):
        return PyPrintPreview.DetermineScaling(*args, **kw)
    base_DetermineScaling = wx._deprecated(base_DetermineScaling,
                                   "Please use PyPrintPreview.DetermineScaling instead.")

_windows_.PyPrintPreview_swigregister(PyPrintPreview)

class PyPreviewFrame(PreviewFrame):
    """Proxy of C++ PyPreviewFrame class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, PrintPreview preview, Frame parent, String title, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=DEFAULT_FRAME_STYLE, String name=FrameNameStr) -> PyPreviewFrame
        """
        _windows_.PyPreviewFrame_swiginit(self,_windows_.new_PyPreviewFrame(*args, **kwargs))
        self._setOORInfo(self);PyPreviewFrame._setCallbackInfo(self, self, PyPreviewFrame)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyPreviewFrame__setCallbackInfo(*args, **kwargs)

    def SetPreviewCanvas(*args, **kwargs):
        """SetPreviewCanvas(self, PreviewCanvas canvas)"""
        return _windows_.PyPreviewFrame_SetPreviewCanvas(*args, **kwargs)

    def SetControlBar(*args, **kwargs):
        """SetControlBar(self, PreviewControlBar bar)"""
        return _windows_.PyPreviewFrame_SetControlBar(*args, **kwargs)

    def Initialize(*args, **kwargs):
        """Initialize(self)"""
        return _windows_.PyPreviewFrame_Initialize(*args, **kwargs)

    def CreateCanvas(*args, **kwargs):
        """CreateCanvas(self)"""
        return _windows_.PyPreviewFrame_CreateCanvas(*args, **kwargs)

    def CreateControlBar(*args, **kwargs):
        """CreateControlBar(self)"""
        return _windows_.PyPreviewFrame_CreateControlBar(*args, **kwargs)

    def base_Initialize(*args, **kw):
        return PyPreviewFrame.Initialize(*args, **kw)
    base_Initialize = wx._deprecated(base_Initialize,
                                   "Please use PyPreviewFrame.Initialize instead.")

    def base_CreateCanvas(*args, **kw):
        return PyPreviewFrame.CreateCanvas(*args, **kw)
    base_CreateCanvas = wx._deprecated(base_CreateCanvas,
                                   "Please use PyPreviewFrame.CreateCanvas instead.")

    def base_CreateControlBar(*args, **kw):
        return PyPreviewFrame.CreateControlBar(*args, **kw)
    base_CreateControlBar = wx._deprecated(base_CreateControlBar,
                                   "Please use PyPreviewFrame.CreateControlBar instead.")

_windows_.PyPreviewFrame_swigregister(PyPreviewFrame)

class PyPreviewControlBar(PreviewControlBar):
    """Proxy of C++ PyPreviewControlBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, PrintPreview preview, long buttons, Window parent, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=PanelNameStr) -> PyPreviewControlBar
        """
        _windows_.PyPreviewControlBar_swiginit(self,_windows_.new_PyPreviewControlBar(*args, **kwargs))
        self._setOORInfo(self);PyPreviewControlBar._setCallbackInfo(self, self, PyPreviewControlBar)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _windows_.PyPreviewControlBar__setCallbackInfo(*args, **kwargs)

    def SetPrintPreview(*args, **kwargs):
        """SetPrintPreview(self, PrintPreview preview)"""
        return _windows_.PyPreviewControlBar_SetPrintPreview(*args, **kwargs)

    def CreateButtons(*args, **kwargs):
        """CreateButtons(self)"""
        return _windows_.PyPreviewControlBar_CreateButtons(*args, **kwargs)

    def SetZoomControl(*args, **kwargs):
        """SetZoomControl(self, int zoom)"""
        return _windows_.PyPreviewControlBar_SetZoomControl(*args, **kwargs)

    def base_CreateButtons(*args, **kw):
        return PreviewControlBar.CreateButtons(*args, **kw)
    base_CreateButtons = wx._deprecated(base_CreateButtons,
                                   "Please use PreviewControlBar.CreateButtons instead.")

    def base_SetZoomControl(*args, **kw):
        return PreviewControlBar.SetZoomControl(*args, **kw)
    base_SetZoomControl = wx._deprecated(base_SetZoomControl,
                                   "Please use PreviewControlBar.SetZoomControl instead.")

_windows_.PyPreviewControlBar_swigregister(PyPreviewControlBar)



