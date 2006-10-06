# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _misc_
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

SYS_OEM_FIXED_FONT = _misc_.SYS_OEM_FIXED_FONT
SYS_ANSI_FIXED_FONT = _misc_.SYS_ANSI_FIXED_FONT
SYS_ANSI_VAR_FONT = _misc_.SYS_ANSI_VAR_FONT
SYS_SYSTEM_FONT = _misc_.SYS_SYSTEM_FONT
SYS_DEVICE_DEFAULT_FONT = _misc_.SYS_DEVICE_DEFAULT_FONT
SYS_DEFAULT_PALETTE = _misc_.SYS_DEFAULT_PALETTE
SYS_SYSTEM_FIXED_FONT = _misc_.SYS_SYSTEM_FIXED_FONT
SYS_DEFAULT_GUI_FONT = _misc_.SYS_DEFAULT_GUI_FONT
SYS_ICONTITLE_FONT = _misc_.SYS_ICONTITLE_FONT
SYS_COLOUR_SCROLLBAR = _misc_.SYS_COLOUR_SCROLLBAR
SYS_COLOUR_BACKGROUND = _misc_.SYS_COLOUR_BACKGROUND
SYS_COLOUR_DESKTOP = _misc_.SYS_COLOUR_DESKTOP
SYS_COLOUR_ACTIVECAPTION = _misc_.SYS_COLOUR_ACTIVECAPTION
SYS_COLOUR_INACTIVECAPTION = _misc_.SYS_COLOUR_INACTIVECAPTION
SYS_COLOUR_MENU = _misc_.SYS_COLOUR_MENU
SYS_COLOUR_WINDOW = _misc_.SYS_COLOUR_WINDOW
SYS_COLOUR_WINDOWFRAME = _misc_.SYS_COLOUR_WINDOWFRAME
SYS_COLOUR_MENUTEXT = _misc_.SYS_COLOUR_MENUTEXT
SYS_COLOUR_WINDOWTEXT = _misc_.SYS_COLOUR_WINDOWTEXT
SYS_COLOUR_CAPTIONTEXT = _misc_.SYS_COLOUR_CAPTIONTEXT
SYS_COLOUR_ACTIVEBORDER = _misc_.SYS_COLOUR_ACTIVEBORDER
SYS_COLOUR_INACTIVEBORDER = _misc_.SYS_COLOUR_INACTIVEBORDER
SYS_COLOUR_APPWORKSPACE = _misc_.SYS_COLOUR_APPWORKSPACE
SYS_COLOUR_HIGHLIGHT = _misc_.SYS_COLOUR_HIGHLIGHT
SYS_COLOUR_HIGHLIGHTTEXT = _misc_.SYS_COLOUR_HIGHLIGHTTEXT
SYS_COLOUR_BTNFACE = _misc_.SYS_COLOUR_BTNFACE
SYS_COLOUR_3DFACE = _misc_.SYS_COLOUR_3DFACE
SYS_COLOUR_BTNSHADOW = _misc_.SYS_COLOUR_BTNSHADOW
SYS_COLOUR_3DSHADOW = _misc_.SYS_COLOUR_3DSHADOW
SYS_COLOUR_GRAYTEXT = _misc_.SYS_COLOUR_GRAYTEXT
SYS_COLOUR_BTNTEXT = _misc_.SYS_COLOUR_BTNTEXT
SYS_COLOUR_INACTIVECAPTIONTEXT = _misc_.SYS_COLOUR_INACTIVECAPTIONTEXT
SYS_COLOUR_BTNHIGHLIGHT = _misc_.SYS_COLOUR_BTNHIGHLIGHT
SYS_COLOUR_BTNHILIGHT = _misc_.SYS_COLOUR_BTNHILIGHT
SYS_COLOUR_3DHIGHLIGHT = _misc_.SYS_COLOUR_3DHIGHLIGHT
SYS_COLOUR_3DHILIGHT = _misc_.SYS_COLOUR_3DHILIGHT
SYS_COLOUR_3DDKSHADOW = _misc_.SYS_COLOUR_3DDKSHADOW
SYS_COLOUR_3DLIGHT = _misc_.SYS_COLOUR_3DLIGHT
SYS_COLOUR_INFOTEXT = _misc_.SYS_COLOUR_INFOTEXT
SYS_COLOUR_INFOBK = _misc_.SYS_COLOUR_INFOBK
SYS_COLOUR_LISTBOX = _misc_.SYS_COLOUR_LISTBOX
SYS_COLOUR_HOTLIGHT = _misc_.SYS_COLOUR_HOTLIGHT
SYS_COLOUR_GRADIENTACTIVECAPTION = _misc_.SYS_COLOUR_GRADIENTACTIVECAPTION
SYS_COLOUR_GRADIENTINACTIVECAPTION = _misc_.SYS_COLOUR_GRADIENTINACTIVECAPTION
SYS_COLOUR_MENUHILIGHT = _misc_.SYS_COLOUR_MENUHILIGHT
SYS_COLOUR_MENUBAR = _misc_.SYS_COLOUR_MENUBAR
SYS_COLOUR_MAX = _misc_.SYS_COLOUR_MAX
SYS_MOUSE_BUTTONS = _misc_.SYS_MOUSE_BUTTONS
SYS_BORDER_X = _misc_.SYS_BORDER_X
SYS_BORDER_Y = _misc_.SYS_BORDER_Y
SYS_CURSOR_X = _misc_.SYS_CURSOR_X
SYS_CURSOR_Y = _misc_.SYS_CURSOR_Y
SYS_DCLICK_X = _misc_.SYS_DCLICK_X
SYS_DCLICK_Y = _misc_.SYS_DCLICK_Y
SYS_DRAG_X = _misc_.SYS_DRAG_X
SYS_DRAG_Y = _misc_.SYS_DRAG_Y
SYS_EDGE_X = _misc_.SYS_EDGE_X
SYS_EDGE_Y = _misc_.SYS_EDGE_Y
SYS_HSCROLL_ARROW_X = _misc_.SYS_HSCROLL_ARROW_X
SYS_HSCROLL_ARROW_Y = _misc_.SYS_HSCROLL_ARROW_Y
SYS_HTHUMB_X = _misc_.SYS_HTHUMB_X
SYS_ICON_X = _misc_.SYS_ICON_X
SYS_ICON_Y = _misc_.SYS_ICON_Y
SYS_ICONSPACING_X = _misc_.SYS_ICONSPACING_X
SYS_ICONSPACING_Y = _misc_.SYS_ICONSPACING_Y
SYS_WINDOWMIN_X = _misc_.SYS_WINDOWMIN_X
SYS_WINDOWMIN_Y = _misc_.SYS_WINDOWMIN_Y
SYS_SCREEN_X = _misc_.SYS_SCREEN_X
SYS_SCREEN_Y = _misc_.SYS_SCREEN_Y
SYS_FRAMESIZE_X = _misc_.SYS_FRAMESIZE_X
SYS_FRAMESIZE_Y = _misc_.SYS_FRAMESIZE_Y
SYS_SMALLICON_X = _misc_.SYS_SMALLICON_X
SYS_SMALLICON_Y = _misc_.SYS_SMALLICON_Y
SYS_HSCROLL_Y = _misc_.SYS_HSCROLL_Y
SYS_VSCROLL_X = _misc_.SYS_VSCROLL_X
SYS_VSCROLL_ARROW_X = _misc_.SYS_VSCROLL_ARROW_X
SYS_VSCROLL_ARROW_Y = _misc_.SYS_VSCROLL_ARROW_Y
SYS_VTHUMB_Y = _misc_.SYS_VTHUMB_Y
SYS_CAPTION_Y = _misc_.SYS_CAPTION_Y
SYS_MENU_Y = _misc_.SYS_MENU_Y
SYS_NETWORK_PRESENT = _misc_.SYS_NETWORK_PRESENT
SYS_PENWINDOWS_PRESENT = _misc_.SYS_PENWINDOWS_PRESENT
SYS_SHOW_SOUNDS = _misc_.SYS_SHOW_SOUNDS
SYS_SWAP_BUTTONS = _misc_.SYS_SWAP_BUTTONS
SYS_CAN_DRAW_FRAME_DECORATIONS = _misc_.SYS_CAN_DRAW_FRAME_DECORATIONS
SYS_CAN_ICONIZE_FRAME = _misc_.SYS_CAN_ICONIZE_FRAME
SYS_TABLET_PRESENT = _misc_.SYS_TABLET_PRESENT
SYS_SCREEN_NONE = _misc_.SYS_SCREEN_NONE
SYS_SCREEN_TINY = _misc_.SYS_SCREEN_TINY
SYS_SCREEN_PDA = _misc_.SYS_SCREEN_PDA
SYS_SCREEN_SMALL = _misc_.SYS_SCREEN_SMALL
SYS_SCREEN_DESKTOP = _misc_.SYS_SCREEN_DESKTOP
class SystemSettings(object):
    """Proxy of C++ SystemSettings class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetColour(*args, **kwargs):
        """GetColour(int index) -> Colour"""
        return _misc_.SystemSettings_GetColour(*args, **kwargs)

    GetColour = staticmethod(GetColour)
    def GetFont(*args, **kwargs):
        """GetFont(int index) -> Font"""
        return _misc_.SystemSettings_GetFont(*args, **kwargs)

    GetFont = staticmethod(GetFont)
    def GetMetric(*args, **kwargs):
        """GetMetric(int index, Window win=None) -> int"""
        return _misc_.SystemSettings_GetMetric(*args, **kwargs)

    GetMetric = staticmethod(GetMetric)
    def HasFeature(*args, **kwargs):
        """HasFeature(int index) -> bool"""
        return _misc_.SystemSettings_HasFeature(*args, **kwargs)

    HasFeature = staticmethod(HasFeature)
    def GetScreenType(*args, **kwargs):
        """GetScreenType() -> int"""
        return _misc_.SystemSettings_GetScreenType(*args, **kwargs)

    GetScreenType = staticmethod(GetScreenType)
    def SetScreenType(*args, **kwargs):
        """SetScreenType(int screen)"""
        return _misc_.SystemSettings_SetScreenType(*args, **kwargs)

    SetScreenType = staticmethod(SetScreenType)
_misc_.SystemSettings_swigregister(SystemSettings)

def SystemSettings_GetColour(*args, **kwargs):
  """SystemSettings_GetColour(int index) -> Colour"""
  return _misc_.SystemSettings_GetColour(*args, **kwargs)

def SystemSettings_GetFont(*args, **kwargs):
  """SystemSettings_GetFont(int index) -> Font"""
  return _misc_.SystemSettings_GetFont(*args, **kwargs)

def SystemSettings_GetMetric(*args, **kwargs):
  """SystemSettings_GetMetric(int index, Window win=None) -> int"""
  return _misc_.SystemSettings_GetMetric(*args, **kwargs)

def SystemSettings_HasFeature(*args, **kwargs):
  """SystemSettings_HasFeature(int index) -> bool"""
  return _misc_.SystemSettings_HasFeature(*args, **kwargs)

def SystemSettings_GetScreenType(*args):
  """SystemSettings_GetScreenType() -> int"""
  return _misc_.SystemSettings_GetScreenType(*args)

def SystemSettings_SetScreenType(*args, **kwargs):
  """SystemSettings_SetScreenType(int screen)"""
  return _misc_.SystemSettings_SetScreenType(*args, **kwargs)

class SystemOptions(_core.Object):
    """Proxy of C++ SystemOptions class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> SystemOptions"""
        _misc_.SystemOptions_swiginit(self,_misc_.new_SystemOptions(*args, **kwargs))
    def SetOption(*args, **kwargs):
        """SetOption(String name, String value)"""
        return _misc_.SystemOptions_SetOption(*args, **kwargs)

    SetOption = staticmethod(SetOption)
    def SetOptionInt(*args, **kwargs):
        """SetOptionInt(String name, int value)"""
        return _misc_.SystemOptions_SetOptionInt(*args, **kwargs)

    SetOptionInt = staticmethod(SetOptionInt)
    def GetOption(*args, **kwargs):
        """GetOption(String name) -> String"""
        return _misc_.SystemOptions_GetOption(*args, **kwargs)

    GetOption = staticmethod(GetOption)
    def GetOptionInt(*args, **kwargs):
        """GetOptionInt(String name) -> int"""
        return _misc_.SystemOptions_GetOptionInt(*args, **kwargs)

    GetOptionInt = staticmethod(GetOptionInt)
    def HasOption(*args, **kwargs):
        """HasOption(String name) -> bool"""
        return _misc_.SystemOptions_HasOption(*args, **kwargs)

    HasOption = staticmethod(HasOption)
    def IsFalse(*args, **kwargs):
        """IsFalse(String name) -> bool"""
        return _misc_.SystemOptions_IsFalse(*args, **kwargs)

    IsFalse = staticmethod(IsFalse)
_misc_.SystemOptions_swigregister(SystemOptions)
cvar = _misc_.cvar
WINDOW_DEFAULT_VARIANT = cvar.WINDOW_DEFAULT_VARIANT

def SystemOptions_SetOption(*args, **kwargs):
  """SystemOptions_SetOption(String name, String value)"""
  return _misc_.SystemOptions_SetOption(*args, **kwargs)

def SystemOptions_SetOptionInt(*args, **kwargs):
  """SystemOptions_SetOptionInt(String name, int value)"""
  return _misc_.SystemOptions_SetOptionInt(*args, **kwargs)

def SystemOptions_GetOption(*args, **kwargs):
  """SystemOptions_GetOption(String name) -> String"""
  return _misc_.SystemOptions_GetOption(*args, **kwargs)

def SystemOptions_GetOptionInt(*args, **kwargs):
  """SystemOptions_GetOptionInt(String name) -> int"""
  return _misc_.SystemOptions_GetOptionInt(*args, **kwargs)

def SystemOptions_HasOption(*args, **kwargs):
  """SystemOptions_HasOption(String name) -> bool"""
  return _misc_.SystemOptions_HasOption(*args, **kwargs)

def SystemOptions_IsFalse(*args, **kwargs):
  """SystemOptions_IsFalse(String name) -> bool"""
  return _misc_.SystemOptions_IsFalse(*args, **kwargs)

# Until the new native control for wxMac is up to par, still use the generic one.
SystemOptions.SetOptionInt("mac.listctrl.always_use_generic", 1)

#---------------------------------------------------------------------------


def NewId(*args):
  """NewId() -> long"""
  return _misc_.NewId(*args)

def RegisterId(*args, **kwargs):
  """RegisterId(long id)"""
  return _misc_.RegisterId(*args, **kwargs)

def GetCurrentId(*args):
  """GetCurrentId() -> long"""
  return _misc_.GetCurrentId(*args)

def IsStockID(*args, **kwargs):
  """IsStockID(int id) -> bool"""
  return _misc_.IsStockID(*args, **kwargs)

def IsStockLabel(*args, **kwargs):
  """IsStockLabel(int id, String label) -> bool"""
  return _misc_.IsStockLabel(*args, **kwargs)
STOCK_NOFLAGS = _misc_.STOCK_NOFLAGS
STOCK_WITH_MNEMONIC = _misc_.STOCK_WITH_MNEMONIC
STOCK_WITH_ACCELERATOR = _misc_.STOCK_WITH_ACCELERATOR

def GetStockLabel(*args, **kwargs):
  """GetStockLabel(int id, long flags=STOCK_WITH_MNEMONIC) -> String"""
  return _misc_.GetStockLabel(*args, **kwargs)
STOCK_MENU = _misc_.STOCK_MENU

def GetStockHelpString(*args, **kwargs):
  """GetStockHelpString(int id, int client=STOCK_MENU) -> String"""
  return _misc_.GetStockHelpString(*args, **kwargs)

def Bell(*args):
  """Bell()"""
  return _misc_.Bell(*args)

def EndBusyCursor(*args):
  """EndBusyCursor()"""
  return _misc_.EndBusyCursor(*args)

def GetElapsedTime(*args, **kwargs):
  """GetElapsedTime(bool resetTimer=True) -> long"""
  return _misc_.GetElapsedTime(*args, **kwargs)
GetElapsedTime = wx._deprecated(GetElapsedTime) 

def IsBusy(*args):
  """IsBusy() -> bool"""
  return _misc_.IsBusy(*args)

def Now(*args):
  """Now() -> String"""
  return _misc_.Now(*args)

def Shell(*args, **kwargs):
  """Shell(String command=EmptyString) -> bool"""
  return _misc_.Shell(*args, **kwargs)

def StartTimer(*args):
  """StartTimer()"""
  return _misc_.StartTimer(*args)

def GetOsVersion(*args):
  """GetOsVersion() -> (platform, major, minor)"""
  return _misc_.GetOsVersion(*args)

def GetOsDescription(*args):
  """GetOsDescription() -> String"""
  return _misc_.GetOsDescription(*args)

def IsPlatformLittleEndian(*args):
  """IsPlatformLittleEndian() -> bool"""
  return _misc_.IsPlatformLittleEndian(*args)

def IsPlatform64Bit(*args):
  """IsPlatform64Bit() -> bool"""
  return _misc_.IsPlatform64Bit(*args)

def GetFreeMemory(*args):
  """GetFreeMemory() -> wxMemorySize"""
  return _misc_.GetFreeMemory(*args)
SHUTDOWN_POWEROFF = _misc_.SHUTDOWN_POWEROFF
SHUTDOWN_REBOOT = _misc_.SHUTDOWN_REBOOT

def Shutdown(*args, **kwargs):
  """Shutdown(int wFlags) -> bool"""
  return _misc_.Shutdown(*args, **kwargs)

def Sleep(*args, **kwargs):
  """Sleep(int secs)"""
  return _misc_.Sleep(*args, **kwargs)

def MilliSleep(*args, **kwargs):
  """MilliSleep(unsigned long milliseconds)"""
  return _misc_.MilliSleep(*args, **kwargs)

def MicroSleep(*args, **kwargs):
  """MicroSleep(unsigned long microseconds)"""
  return _misc_.MicroSleep(*args, **kwargs)
Usleep = MilliSleep 

def EnableTopLevelWindows(*args, **kwargs):
  """EnableTopLevelWindows(bool enable)"""
  return _misc_.EnableTopLevelWindows(*args, **kwargs)

def StripMenuCodes(*args, **kwargs):
  """StripMenuCodes(String in) -> String"""
  return _misc_.StripMenuCodes(*args, **kwargs)

def GetEmailAddress(*args):
  """GetEmailAddress() -> String"""
  return _misc_.GetEmailAddress(*args)

def GetHostName(*args):
  """GetHostName() -> String"""
  return _misc_.GetHostName(*args)

def GetFullHostName(*args):
  """GetFullHostName() -> String"""
  return _misc_.GetFullHostName(*args)

def GetUserId(*args):
  """GetUserId() -> String"""
  return _misc_.GetUserId(*args)

def GetUserName(*args):
  """GetUserName() -> String"""
  return _misc_.GetUserName(*args)

def GetHomeDir(*args):
  """GetHomeDir() -> String"""
  return _misc_.GetHomeDir(*args)

def GetUserHome(*args, **kwargs):
  """GetUserHome(String user=EmptyString) -> String"""
  return _misc_.GetUserHome(*args, **kwargs)

def GetProcessId(*args):
  """GetProcessId() -> unsigned long"""
  return _misc_.GetProcessId(*args)

def Trap(*args):
  """Trap()"""
  return _misc_.Trap(*args)

def FileSelector(*args, **kwargs):
  """
    FileSelector(String message=FileSelectorPromptStr, String default_path=EmptyString, 
        String default_filename=EmptyString, 
        String default_extension=EmptyString, 
        String wildcard=FileSelectorDefaultWildcardStr, 
        int flags=0, Window parent=None, int x=-1, 
        int y=-1) -> String
    """
  return _misc_.FileSelector(*args, **kwargs)

def LoadFileSelector(*args, **kwargs):
  """
    LoadFileSelector(String what, String extension, String default_name=EmptyString, 
        Window parent=None) -> String
    """
  return _misc_.LoadFileSelector(*args, **kwargs)

def SaveFileSelector(*args, **kwargs):
  """
    SaveFileSelector(String what, String extension, String default_name=EmptyString, 
        Window parent=None) -> String
    """
  return _misc_.SaveFileSelector(*args, **kwargs)

def DirSelector(*args, **kwargs):
  """
    DirSelector(String message=DirSelectorPromptStr, String defaultPath=EmptyString, 
        long style=wxDD_DEFAULT_STYLE, 
        Point pos=DefaultPosition, Window parent=None) -> String
    """
  return _misc_.DirSelector(*args, **kwargs)

def GetTextFromUser(*args, **kwargs):
  """
    GetTextFromUser(String message, String caption=EmptyString, String default_value=EmptyString, 
        Window parent=None, 
        int x=-1, int y=-1, bool centre=True) -> String
    """
  return _misc_.GetTextFromUser(*args, **kwargs)

def GetPasswordFromUser(*args, **kwargs):
  """
    GetPasswordFromUser(String message, String caption=EmptyString, String default_value=EmptyString, 
        Window parent=None) -> String
    """
  return _misc_.GetPasswordFromUser(*args, **kwargs)

def GetSingleChoice(*args, **kwargs):
  """
    GetSingleChoice(String message, String caption, int choices, Window parent=None, 
        int x=-1, int y=-1, bool centre=True, 
        int width=150, int height=200) -> String
    """
  return _misc_.GetSingleChoice(*args, **kwargs)

def GetSingleChoiceIndex(*args, **kwargs):
  """
    GetSingleChoiceIndex(String message, String caption, int choices, Window parent=None, 
        int x=-1, int y=-1, bool centre=True, 
        int width=150, int height=200) -> int
    """
  return _misc_.GetSingleChoiceIndex(*args, **kwargs)

def MessageBox(*args, **kwargs):
  """
    MessageBox(String message, String caption=EmptyString, int style=wxOK|wxCENTRE, 
        Window parent=None, int x=-1, 
        int y=-1) -> int
    """
  return _misc_.MessageBox(*args, **kwargs)

def GetNumberFromUser(*args, **kwargs):
  """
    GetNumberFromUser(String message, String prompt, String caption, long value, 
        long min=0, long max=100, Window parent=None, 
        Point pos=DefaultPosition) -> long
    """
  return _misc_.GetNumberFromUser(*args, **kwargs)

def ColourDisplay(*args):
  """ColourDisplay() -> bool"""
  return _misc_.ColourDisplay(*args)

def DisplayDepth(*args):
  """DisplayDepth() -> int"""
  return _misc_.DisplayDepth(*args)

def GetDisplayDepth(*args):
  """GetDisplayDepth() -> int"""
  return _misc_.GetDisplayDepth(*args)

def DisplaySize(*args):
  """DisplaySize() -> (width, height)"""
  return _misc_.DisplaySize(*args)

def GetDisplaySize(*args):
  """GetDisplaySize() -> Size"""
  return _misc_.GetDisplaySize(*args)

def DisplaySizeMM(*args):
  """DisplaySizeMM() -> (width, height)"""
  return _misc_.DisplaySizeMM(*args)

def GetDisplaySizeMM(*args):
  """GetDisplaySizeMM() -> Size"""
  return _misc_.GetDisplaySizeMM(*args)

def ClientDisplayRect(*args):
  """ClientDisplayRect() -> (x, y, width, height)"""
  return _misc_.ClientDisplayRect(*args)

def GetClientDisplayRect(*args):
  """GetClientDisplayRect() -> Rect"""
  return _misc_.GetClientDisplayRect(*args)

def SetCursor(*args, **kwargs):
  """SetCursor(Cursor cursor)"""
  return _misc_.SetCursor(*args, **kwargs)

def GetXDisplay(*args):
  """
    GetXDisplay() -> void

    Returns a swigified pointer to the X11 display.  Returns None on
    other platforms.
    """
  return _misc_.GetXDisplay(*args)

def BeginBusyCursor(*args, **kwargs):
  """BeginBusyCursor(Cursor cursor=wxHOURGLASS_CURSOR)"""
  return _misc_.BeginBusyCursor(*args, **kwargs)

def GetMousePosition(*args):
  """
    GetMousePosition() -> Point

    Get the current mouse position on the screen.
    """
  return _misc_.GetMousePosition(*args)

def FindWindowAtPointer(*args):
  """
    FindWindowAtPointer() -> Window

    Returns the window currently under the mouse pointer, if it belongs to
        this application.  Otherwise it returns None.
    """
  return _misc_.FindWindowAtPointer(*args)

def GetActiveWindow(*args):
  """
    GetActiveWindow() -> Window

    Get the currently active window of this application, or None
    """
  return _misc_.GetActiveWindow(*args)

def GenericFindWindowAtPoint(*args, **kwargs):
  """GenericFindWindowAtPoint(Point pt) -> Window"""
  return _misc_.GenericFindWindowAtPoint(*args, **kwargs)

def FindWindowAtPoint(*args, **kwargs):
  """FindWindowAtPoint(Point pt) -> Window"""
  return _misc_.FindWindowAtPoint(*args, **kwargs)

def GetTopLevelParent(*args, **kwargs):
  """GetTopLevelParent(Window win) -> Window"""
  return _misc_.GetTopLevelParent(*args, **kwargs)

def LaunchDefaultBrowser(*args, **kwargs):
  """
    LaunchDefaultBrowser(String url) -> bool

    Launches the user's default browser and tells it to open the location
    at ``url``.  Returns ``True`` if the application was successfully
    launched.
    """
  return _misc_.LaunchDefaultBrowser(*args, **kwargs)

def GetKeyState(*args, **kwargs):
  """
    GetKeyState(int key) -> bool

    Get the state of a key (true if pressed or toggled on, false if not.)
    This is generally most useful getting the state of the modifier or
    toggle keys.  On some platforms those may be the only keys that this
    function is able to detect.

    """
  return _misc_.GetKeyState(*args, **kwargs)
class MouseState(object):
    """
    `wx.MouseState` is used to hold information about mouse button and
    modifier key states and is what is returned from `wx.GetMouseState`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> MouseState

        `wx.MouseState` is used to hold information about mouse button and
        modifier key states and is what is returned from `wx.GetMouseState`.
        """
        _misc_.MouseState_swiginit(self,_misc_.new_MouseState(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_MouseState
    __del__ = lambda self : None;
    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _misc_.MouseState_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _misc_.MouseState_GetY(*args, **kwargs)

    def LeftDown(*args, **kwargs):
        """LeftDown(self) -> bool"""
        return _misc_.MouseState_LeftDown(*args, **kwargs)

    def MiddleDown(*args, **kwargs):
        """MiddleDown(self) -> bool"""
        return _misc_.MouseState_MiddleDown(*args, **kwargs)

    def RightDown(*args, **kwargs):
        """RightDown(self) -> bool"""
        return _misc_.MouseState_RightDown(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _misc_.MouseState_ControlDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _misc_.MouseState_ShiftDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _misc_.MouseState_AltDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _misc_.MouseState_MetaDown(*args, **kwargs)

    def CmdDown(*args, **kwargs):
        """CmdDown(self) -> bool"""
        return _misc_.MouseState_CmdDown(*args, **kwargs)

    def SetX(*args, **kwargs):
        """SetX(self, int x)"""
        return _misc_.MouseState_SetX(*args, **kwargs)

    def SetY(*args, **kwargs):
        """SetY(self, int y)"""
        return _misc_.MouseState_SetY(*args, **kwargs)

    def SetLeftDown(*args, **kwargs):
        """SetLeftDown(self, bool down)"""
        return _misc_.MouseState_SetLeftDown(*args, **kwargs)

    def SetMiddleDown(*args, **kwargs):
        """SetMiddleDown(self, bool down)"""
        return _misc_.MouseState_SetMiddleDown(*args, **kwargs)

    def SetRightDown(*args, **kwargs):
        """SetRightDown(self, bool down)"""
        return _misc_.MouseState_SetRightDown(*args, **kwargs)

    def SetControlDown(*args, **kwargs):
        """SetControlDown(self, bool down)"""
        return _misc_.MouseState_SetControlDown(*args, **kwargs)

    def SetShiftDown(*args, **kwargs):
        """SetShiftDown(self, bool down)"""
        return _misc_.MouseState_SetShiftDown(*args, **kwargs)

    def SetAltDown(*args, **kwargs):
        """SetAltDown(self, bool down)"""
        return _misc_.MouseState_SetAltDown(*args, **kwargs)

    def SetMetaDown(*args, **kwargs):
        """SetMetaDown(self, bool down)"""
        return _misc_.MouseState_SetMetaDown(*args, **kwargs)

    x = property(GetX, SetX)
    y = property(GetY, SetY)
    leftDown = property(LeftDown, SetLeftDown)
    middleDown = property(MiddleDown, SetMiddleDown)
    rightDown = property(RightDown, SetRightDown)
    controlDown = property(ControlDown, SetControlDown)
    shiftDown = property(ShiftDown, SetShiftDown)
    altDown = property(AltDown, SetAltDown)
    metaDown = property(MetaDown, SetMetaDown)
    cmdDown = property(CmdDown)

_misc_.MouseState_swigregister(MouseState)
FileSelectorPromptStr = cvar.FileSelectorPromptStr
FileSelectorDefaultWildcardStr = cvar.FileSelectorDefaultWildcardStr
DirSelectorPromptStr = cvar.DirSelectorPromptStr


def GetMouseState(*args):
  """
    GetMouseState() -> MouseState

    Returns the current state of the mouse.  Returns an instance of a
    `wx.MouseState` object that contains the current position of the mouse
    pointer in screen coordinants, as well as boolean values indicating
    the up/down status of the mouse buttons and the modifier keys.
    """
  return _misc_.GetMouseState(*args)

def WakeUpMainThread(*args):
  """WakeUpMainThread()"""
  return _misc_.WakeUpMainThread(*args)

def MutexGuiEnter(*args):
  """MutexGuiEnter()"""
  return _misc_.MutexGuiEnter(*args)

def MutexGuiLeave(*args):
  """MutexGuiLeave()"""
  return _misc_.MutexGuiLeave(*args)
class MutexGuiLocker(object):
    """Proxy of C++ MutexGuiLocker class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> MutexGuiLocker"""
        _misc_.MutexGuiLocker_swiginit(self,_misc_.new_MutexGuiLocker(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_MutexGuiLocker
    __del__ = lambda self : None;
_misc_.MutexGuiLocker_swigregister(MutexGuiLocker)


def Thread_IsMain(*args):
  """Thread_IsMain() -> bool"""
  return _misc_.Thread_IsMain(*args)
#---------------------------------------------------------------------------

class ToolTip(_core.Object):
    """Proxy of C++ ToolTip class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String tip) -> ToolTip"""
        _misc_.ToolTip_swiginit(self,_misc_.new_ToolTip(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_ToolTip
    __del__ = lambda self : None;
    def SetTip(*args, **kwargs):
        """SetTip(self, String tip)"""
        return _misc_.ToolTip_SetTip(*args, **kwargs)

    def GetTip(*args, **kwargs):
        """GetTip(self) -> String"""
        return _misc_.ToolTip_GetTip(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _misc_.ToolTip_GetWindow(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(bool flag)"""
        return _misc_.ToolTip_Enable(*args, **kwargs)

    Enable = staticmethod(Enable)
    def SetDelay(*args, **kwargs):
        """SetDelay(long milliseconds)"""
        return _misc_.ToolTip_SetDelay(*args, **kwargs)

    SetDelay = staticmethod(SetDelay)
    Tip = property(GetTip,SetTip,doc="See `GetTip` and `SetTip`") 
    Window = property(GetWindow,doc="See `GetWindow`") 
_misc_.ToolTip_swigregister(ToolTip)

def ToolTip_Enable(*args, **kwargs):
  """ToolTip_Enable(bool flag)"""
  return _misc_.ToolTip_Enable(*args, **kwargs)

def ToolTip_SetDelay(*args, **kwargs):
  """ToolTip_SetDelay(long milliseconds)"""
  return _misc_.ToolTip_SetDelay(*args, **kwargs)

class Caret(object):
    """Proxy of C++ Caret class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window window, Size size) -> Caret"""
        _misc_.Caret_swiginit(self,_misc_.new_Caret(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Caret
    __del__ = lambda self : None;
    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        val = _misc_.Caret_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _misc_.Caret_IsOk(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(self) -> bool"""
        return _misc_.Caret_IsVisible(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _misc_.Caret_GetPosition(*args, **kwargs)

    def GetPositionTuple(*args, **kwargs):
        """GetPositionTuple() -> (x,y)"""
        return _misc_.Caret_GetPositionTuple(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _misc_.Caret_GetSize(*args, **kwargs)

    def GetSizeTuple(*args, **kwargs):
        """GetSizeTuple() -> (width, height)"""
        return _misc_.Caret_GetSizeTuple(*args, **kwargs)

    def GetWindow(*args, **kwargs):
        """GetWindow(self) -> Window"""
        return _misc_.Caret_GetWindow(*args, **kwargs)

    def MoveXY(*args, **kwargs):
        """MoveXY(self, int x, int y)"""
        return _misc_.Caret_MoveXY(*args, **kwargs)

    def Move(*args, **kwargs):
        """Move(self, Point pt)"""
        return _misc_.Caret_Move(*args, **kwargs)

    def SetSizeWH(*args, **kwargs):
        """SetSizeWH(self, int width, int height)"""
        return _misc_.Caret_SetSizeWH(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size size)"""
        return _misc_.Caret_SetSize(*args, **kwargs)

    def Show(*args, **kwargs):
        """Show(self, int show=True)"""
        return _misc_.Caret_Show(*args, **kwargs)

    def Hide(*args, **kwargs):
        """Hide(self)"""
        return _misc_.Caret_Hide(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def GetBlinkTime(*args, **kwargs):
        """GetBlinkTime() -> int"""
        return _misc_.Caret_GetBlinkTime(*args, **kwargs)

    GetBlinkTime = staticmethod(GetBlinkTime)
    def SetBlinkTime(*args, **kwargs):
        """SetBlinkTime(int milliseconds)"""
        return _misc_.Caret_SetBlinkTime(*args, **kwargs)

    SetBlinkTime = staticmethod(SetBlinkTime)
    Position = property(GetPosition,doc="See `GetPosition`") 
    Size = property(GetSize,SetSize,doc="See `GetSize` and `SetSize`") 
    Window = property(GetWindow,doc="See `GetWindow`") 
_misc_.Caret_swigregister(Caret)

def Caret_GetBlinkTime(*args):
  """Caret_GetBlinkTime() -> int"""
  return _misc_.Caret_GetBlinkTime(*args)

def Caret_SetBlinkTime(*args, **kwargs):
  """Caret_SetBlinkTime(int milliseconds)"""
  return _misc_.Caret_SetBlinkTime(*args, **kwargs)

class BusyCursor(object):
    """Proxy of C++ BusyCursor class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Cursor cursor=wxHOURGLASS_CURSOR) -> BusyCursor"""
        _misc_.BusyCursor_swiginit(self,_misc_.new_BusyCursor(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_BusyCursor
    __del__ = lambda self : None;
_misc_.BusyCursor_swigregister(BusyCursor)

class WindowDisabler(object):
    """Proxy of C++ WindowDisabler class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window winToSkip=None) -> WindowDisabler"""
        _misc_.WindowDisabler_swiginit(self,_misc_.new_WindowDisabler(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_WindowDisabler
    __del__ = lambda self : None;
_misc_.WindowDisabler_swigregister(WindowDisabler)

class BusyInfo(_core.Object):
    """Proxy of C++ BusyInfo class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String message) -> BusyInfo"""
        _misc_.BusyInfo_swiginit(self,_misc_.new_BusyInfo(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_BusyInfo
    __del__ = lambda self : None;
    def Destroy(self): pass 
_misc_.BusyInfo_swigregister(BusyInfo)

class StopWatch(object):
    """Proxy of C++ StopWatch class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> StopWatch"""
        _misc_.StopWatch_swiginit(self,_misc_.new_StopWatch(*args, **kwargs))
    def Start(*args, **kwargs):
        """Start(self, long t0=0)"""
        return _misc_.StopWatch_Start(*args, **kwargs)

    def Pause(*args, **kwargs):
        """Pause(self)"""
        return _misc_.StopWatch_Pause(*args, **kwargs)

    def Resume(*args, **kwargs):
        """Resume(self)"""
        return _misc_.StopWatch_Resume(*args, **kwargs)

    def Time(*args, **kwargs):
        """Time(self) -> long"""
        return _misc_.StopWatch_Time(*args, **kwargs)

_misc_.StopWatch_swigregister(StopWatch)

class FileHistory(_core.Object):
    """Proxy of C++ FileHistory class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int maxFiles=9, int idBase=ID_FILE1) -> FileHistory"""
        _misc_.FileHistory_swiginit(self,_misc_.new_FileHistory(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_FileHistory
    __del__ = lambda self : None;
    def AddFileToHistory(*args, **kwargs):
        """AddFileToHistory(self, String file)"""
        return _misc_.FileHistory_AddFileToHistory(*args, **kwargs)

    def RemoveFileFromHistory(*args, **kwargs):
        """RemoveFileFromHistory(self, int i)"""
        return _misc_.FileHistory_RemoveFileFromHistory(*args, **kwargs)

    def GetMaxFiles(*args, **kwargs):
        """GetMaxFiles(self) -> int"""
        return _misc_.FileHistory_GetMaxFiles(*args, **kwargs)

    def UseMenu(*args, **kwargs):
        """UseMenu(self, Menu menu)"""
        return _misc_.FileHistory_UseMenu(*args, **kwargs)

    def RemoveMenu(*args, **kwargs):
        """RemoveMenu(self, Menu menu)"""
        return _misc_.FileHistory_RemoveMenu(*args, **kwargs)

    def Load(*args, **kwargs):
        """Load(self, ConfigBase config)"""
        return _misc_.FileHistory_Load(*args, **kwargs)

    def Save(*args, **kwargs):
        """Save(self, ConfigBase config)"""
        return _misc_.FileHistory_Save(*args, **kwargs)

    def AddFilesToMenu(*args, **kwargs):
        """AddFilesToMenu(self)"""
        return _misc_.FileHistory_AddFilesToMenu(*args, **kwargs)

    def AddFilesToThisMenu(*args, **kwargs):
        """AddFilesToThisMenu(self, Menu menu)"""
        return _misc_.FileHistory_AddFilesToThisMenu(*args, **kwargs)

    def GetHistoryFile(*args, **kwargs):
        """GetHistoryFile(self, int i) -> String"""
        return _misc_.FileHistory_GetHistoryFile(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> int"""
        return _misc_.FileHistory_GetCount(*args, **kwargs)

    GetNoHistoryFiles = GetCount 
    Count = property(GetCount,doc="See `GetCount`") 
    HistoryFile = property(GetHistoryFile,doc="See `GetHistoryFile`") 
    MaxFiles = property(GetMaxFiles,doc="See `GetMaxFiles`") 
    NoHistoryFiles = property(GetNoHistoryFiles,doc="See `GetNoHistoryFiles`") 
_misc_.FileHistory_swigregister(FileHistory)

class SingleInstanceChecker(object):
    """Proxy of C++ SingleInstanceChecker class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String name, String path=EmptyString) -> SingleInstanceChecker"""
        _misc_.SingleInstanceChecker_swiginit(self,_misc_.new_SingleInstanceChecker(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_SingleInstanceChecker
    __del__ = lambda self : None;
    def Create(*args, **kwargs):
        """Create(self, String name, String path=EmptyString) -> bool"""
        return _misc_.SingleInstanceChecker_Create(*args, **kwargs)

    def IsAnotherRunning(*args, **kwargs):
        """IsAnotherRunning(self) -> bool"""
        return _misc_.SingleInstanceChecker_IsAnotherRunning(*args, **kwargs)

_misc_.SingleInstanceChecker_swigregister(SingleInstanceChecker)

def PreSingleInstanceChecker(*args, **kwargs):
    """PreSingleInstanceChecker() -> SingleInstanceChecker"""
    val = _misc_.new_PreSingleInstanceChecker(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

OS_UNKNOWN = _misc_.OS_UNKNOWN
OS_MAC_OS = _misc_.OS_MAC_OS
OS_MAC_OSX_DARWIN = _misc_.OS_MAC_OSX_DARWIN
OS_MAC = _misc_.OS_MAC
OS_WINDOWS_9X = _misc_.OS_WINDOWS_9X
OS_WINDOWS_NT = _misc_.OS_WINDOWS_NT
OS_WINDOWS_MICRO = _misc_.OS_WINDOWS_MICRO
OS_WINDOWS_CE = _misc_.OS_WINDOWS_CE
OS_WINDOWS = _misc_.OS_WINDOWS
OS_UNIX_LINUX = _misc_.OS_UNIX_LINUX
OS_UNIX_FREEBSD = _misc_.OS_UNIX_FREEBSD
OS_UNIX_OPENBSD = _misc_.OS_UNIX_OPENBSD
OS_UNIX_NETBSD = _misc_.OS_UNIX_NETBSD
OS_UNIX_SOLARIS = _misc_.OS_UNIX_SOLARIS
OS_UNIX_AIX = _misc_.OS_UNIX_AIX
OS_UNIX_HPUX = _misc_.OS_UNIX_HPUX
OS_UNIX = _misc_.OS_UNIX
OS_DOS = _misc_.OS_DOS
OS_OS2 = _misc_.OS_OS2
PORT_UNKNOWN = _misc_.PORT_UNKNOWN
PORT_BASE = _misc_.PORT_BASE
PORT_MSW = _misc_.PORT_MSW
PORT_MOTIF = _misc_.PORT_MOTIF
PORT_GTK = _misc_.PORT_GTK
PORT_MGL = _misc_.PORT_MGL
PORT_X11 = _misc_.PORT_X11
PORT_PM = _misc_.PORT_PM
PORT_OS2 = _misc_.PORT_OS2
PORT_MAC = _misc_.PORT_MAC
PORT_COCOA = _misc_.PORT_COCOA
PORT_WINCE = _misc_.PORT_WINCE
PORT_PALMOS = _misc_.PORT_PALMOS
PORT_DFB = _misc_.PORT_DFB
ARCH_INVALID = _misc_.ARCH_INVALID
ARCH_32 = _misc_.ARCH_32
ARCH_64 = _misc_.ARCH_64
ARCH_MAX = _misc_.ARCH_MAX
ENDIAN_INVALID = _misc_.ENDIAN_INVALID
ENDIAN_BIG = _misc_.ENDIAN_BIG
ENDIAN_LITTLE = _misc_.ENDIAN_LITTLE
ENDIAN_PDP = _misc_.ENDIAN_PDP
ENDIAN_MAX = _misc_.ENDIAN_MAX
class PlatformInformation(object):
    """Proxy of C++ PlatformInformation class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> PlatformInformation"""
        _misc_.PlatformInformation_swiginit(self,_misc_.new_PlatformInformation(*args, **kwargs))
    def __eq__(*args, **kwargs):
        """__eq__(self, PlatformInformation t) -> bool"""
        return _misc_.PlatformInformation___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, PlatformInformation t) -> bool"""
        return _misc_.PlatformInformation___ne__(*args, **kwargs)

    def GetOSMajorVersion(*args, **kwargs):
        """GetOSMajorVersion(self) -> int"""
        return _misc_.PlatformInformation_GetOSMajorVersion(*args, **kwargs)

    def GetOSMinorVersion(*args, **kwargs):
        """GetOSMinorVersion(self) -> int"""
        return _misc_.PlatformInformation_GetOSMinorVersion(*args, **kwargs)

    def CheckOSVersion(*args, **kwargs):
        """CheckOSVersion(self, int major, int minor) -> bool"""
        return _misc_.PlatformInformation_CheckOSVersion(*args, **kwargs)

    def GetToolkitMajorVersion(*args, **kwargs):
        """GetToolkitMajorVersion(self) -> int"""
        return _misc_.PlatformInformation_GetToolkitMajorVersion(*args, **kwargs)

    def GetToolkitMinorVersion(*args, **kwargs):
        """GetToolkitMinorVersion(self) -> int"""
        return _misc_.PlatformInformation_GetToolkitMinorVersion(*args, **kwargs)

    def CheckToolkitVersion(*args, **kwargs):
        """CheckToolkitVersion(self, int major, int minor) -> bool"""
        return _misc_.PlatformInformation_CheckToolkitVersion(*args, **kwargs)

    def IsUsingUniversalWidgets(*args, **kwargs):
        """IsUsingUniversalWidgets(self) -> bool"""
        return _misc_.PlatformInformation_IsUsingUniversalWidgets(*args, **kwargs)

    def GetOperatingSystemId(*args, **kwargs):
        """GetOperatingSystemId(self) -> int"""
        return _misc_.PlatformInformation_GetOperatingSystemId(*args, **kwargs)

    def GetPortId(*args, **kwargs):
        """GetPortId(self) -> int"""
        return _misc_.PlatformInformation_GetPortId(*args, **kwargs)

    def GetArchitecture(*args, **kwargs):
        """GetArchitecture(self) -> int"""
        return _misc_.PlatformInformation_GetArchitecture(*args, **kwargs)

    def GetEndianness(*args, **kwargs):
        """GetEndianness(self) -> int"""
        return _misc_.PlatformInformation_GetEndianness(*args, **kwargs)

    def GetOperatingSystemFamilyName(*args, **kwargs):
        """GetOperatingSystemFamilyName(self) -> String"""
        return _misc_.PlatformInformation_GetOperatingSystemFamilyName(*args, **kwargs)

    def GetOperatingSystemIdName(*args, **kwargs):
        """GetOperatingSystemIdName(self) -> String"""
        return _misc_.PlatformInformation_GetOperatingSystemIdName(*args, **kwargs)

    def GetPortIdName(*args, **kwargs):
        """GetPortIdName(self) -> String"""
        return _misc_.PlatformInformation_GetPortIdName(*args, **kwargs)

    def GetPortIdShortName(*args, **kwargs):
        """GetPortIdShortName(self) -> String"""
        return _misc_.PlatformInformation_GetPortIdShortName(*args, **kwargs)

    def GetArchName(*args, **kwargs):
        """GetArchName(self) -> String"""
        return _misc_.PlatformInformation_GetArchName(*args, **kwargs)

    def GetEndiannessName(*args, **kwargs):
        """GetEndiannessName(self) -> String"""
        return _misc_.PlatformInformation_GetEndiannessName(*args, **kwargs)

    def SetOSVersion(*args, **kwargs):
        """SetOSVersion(self, int major, int minor)"""
        return _misc_.PlatformInformation_SetOSVersion(*args, **kwargs)

    def SetToolkitVersion(*args, **kwargs):
        """SetToolkitVersion(self, int major, int minor)"""
        return _misc_.PlatformInformation_SetToolkitVersion(*args, **kwargs)

    def SetOperatingSystemId(*args, **kwargs):
        """SetOperatingSystemId(self, int n)"""
        return _misc_.PlatformInformation_SetOperatingSystemId(*args, **kwargs)

    def SetPortId(*args, **kwargs):
        """SetPortId(self, int n)"""
        return _misc_.PlatformInformation_SetPortId(*args, **kwargs)

    def SetArchitecture(*args, **kwargs):
        """SetArchitecture(self, int n)"""
        return _misc_.PlatformInformation_SetArchitecture(*args, **kwargs)

    def SetEndianness(*args, **kwargs):
        """SetEndianness(self, int n)"""
        return _misc_.PlatformInformation_SetEndianness(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _misc_.PlatformInformation_IsOk(*args, **kwargs)

    ArchName = property(GetArchName,doc="See `GetArchName`") 
    Architecture = property(GetArchitecture,SetArchitecture,doc="See `GetArchitecture` and `SetArchitecture`") 
    Endianness = property(GetEndianness,SetEndianness,doc="See `GetEndianness` and `SetEndianness`") 
    EndiannessName = property(GetEndiannessName,doc="See `GetEndiannessName`") 
    OSMajorVersion = property(GetOSMajorVersion,doc="See `GetOSMajorVersion`") 
    OSMinorVersion = property(GetOSMinorVersion,doc="See `GetOSMinorVersion`") 
    OperatingSystemFamilyName = property(GetOperatingSystemFamilyName,doc="See `GetOperatingSystemFamilyName`") 
    OperatingSystemId = property(GetOperatingSystemId,SetOperatingSystemId,doc="See `GetOperatingSystemId` and `SetOperatingSystemId`") 
    OperatingSystemIdName = property(GetOperatingSystemIdName,doc="See `GetOperatingSystemIdName`") 
    PortId = property(GetPortId,SetPortId,doc="See `GetPortId` and `SetPortId`") 
    PortIdName = property(GetPortIdName,doc="See `GetPortIdName`") 
    PortIdShortName = property(GetPortIdShortName,doc="See `GetPortIdShortName`") 
    ToolkitMajorVersion = property(GetToolkitMajorVersion,doc="See `GetToolkitMajorVersion`") 
    ToolkitMinorVersion = property(GetToolkitMinorVersion,doc="See `GetToolkitMinorVersion`") 
_misc_.PlatformInformation_swigregister(PlatformInformation)


def DrawWindowOnDC(*args, **kwargs):
  """DrawWindowOnDC(Window window, DC dc) -> bool"""
  return _misc_.DrawWindowOnDC(*args, **kwargs)
#---------------------------------------------------------------------------

class TipProvider(object):
    """Proxy of C++ TipProvider class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _misc_.delete_TipProvider
    __del__ = lambda self : None;
    def GetTip(*args, **kwargs):
        """GetTip(self) -> String"""
        return _misc_.TipProvider_GetTip(*args, **kwargs)

    def GetCurrentTip(*args, **kwargs):
        """GetCurrentTip(self) -> size_t"""
        return _misc_.TipProvider_GetCurrentTip(*args, **kwargs)

    def PreprocessTip(*args, **kwargs):
        """PreprocessTip(self, String tip) -> String"""
        return _misc_.TipProvider_PreprocessTip(*args, **kwargs)

    CurrentTip = property(GetCurrentTip,doc="See `GetCurrentTip`") 
    Tip = property(GetTip,doc="See `GetTip`") 
_misc_.TipProvider_swigregister(TipProvider)

class PyTipProvider(TipProvider):
    """Proxy of C++ PyTipProvider class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, size_t currentTip) -> PyTipProvider"""
        _misc_.PyTipProvider_swiginit(self,_misc_.new_PyTipProvider(*args, **kwargs))
        self._setCallbackInfo(self, PyTipProvider)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.PyTipProvider__setCallbackInfo(*args, **kwargs)

_misc_.PyTipProvider_swigregister(PyTipProvider)


def ShowTip(*args, **kwargs):
  """ShowTip(Window parent, TipProvider tipProvider, bool showAtStartup=True) -> bool"""
  return _misc_.ShowTip(*args, **kwargs)

def CreateFileTipProvider(*args, **kwargs):
  """CreateFileTipProvider(String filename, size_t currentTip) -> TipProvider"""
  return _misc_.CreateFileTipProvider(*args, **kwargs)
#---------------------------------------------------------------------------

TIMER_CONTINUOUS = _misc_.TIMER_CONTINUOUS
TIMER_ONE_SHOT = _misc_.TIMER_ONE_SHOT
wxEVT_TIMER = _misc_.wxEVT_TIMER
class Timer(_core.EvtHandler):
    """Proxy of C++ Timer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EvtHandler owner=None, int id=ID_ANY) -> Timer"""
        _misc_.Timer_swiginit(self,_misc_.new_Timer(*args, **kwargs))
        self._setCallbackInfo(self, Timer, 0); self._setOORInfo(self, 0)

    __swig_destroy__ = _misc_.delete_Timer
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref=1)"""
        return _misc_.Timer__setCallbackInfo(*args, **kwargs)

    def SetOwner(*args, **kwargs):
        """SetOwner(self, EvtHandler owner, int id=ID_ANY)"""
        return _misc_.Timer_SetOwner(*args, **kwargs)

    def GetOwner(*args, **kwargs):
        """GetOwner(self) -> EvtHandler"""
        return _misc_.Timer_GetOwner(*args, **kwargs)

    def Start(*args, **kwargs):
        """Start(self, int milliseconds=-1, bool oneShot=False) -> bool"""
        return _misc_.Timer_Start(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self)"""
        return _misc_.Timer_Stop(*args, **kwargs)

    def Notify(*args, **kwargs):
        """Notify(self)"""
        return _misc_.Timer_Notify(*args, **kwargs)

    def IsRunning(*args, **kwargs):
        """IsRunning(self) -> bool"""
        return _misc_.Timer_IsRunning(*args, **kwargs)

    def GetInterval(*args, **kwargs):
        """GetInterval(self) -> int"""
        return _misc_.Timer_GetInterval(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return _misc_.Timer_GetId(*args, **kwargs)

    def IsOneShot(*args, **kwargs):
        """IsOneShot(self) -> bool"""
        return _misc_.Timer_IsOneShot(*args, **kwargs)

    def Destroy(self):
        """NO-OP: Timers must be destroyed by normal reference counting"""
        pass

    Id = property(GetId,doc="See `GetId`") 
    Interval = property(GetInterval,doc="See `GetInterval`") 
    Owner = property(GetOwner,SetOwner,doc="See `GetOwner` and `SetOwner`") 
_misc_.Timer_swigregister(Timer)

# For backwards compatibility with 2.4
class PyTimer(Timer):
    def __init__(self, notify):
        Timer.__init__(self)
        self.notify = notify

    def Notify(self):
        if self.notify:
            self.notify()


EVT_TIMER = wx.PyEventBinder( wxEVT_TIMER, 1 )


class TimerEvent(_core.Event):
    """Proxy of C++ TimerEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int timerid=0, int interval=0) -> TimerEvent"""
        _misc_.TimerEvent_swiginit(self,_misc_.new_TimerEvent(*args, **kwargs))
    def GetInterval(*args, **kwargs):
        """GetInterval(self) -> int"""
        return _misc_.TimerEvent_GetInterval(*args, **kwargs)

    Interval = property(GetInterval,doc="See `GetInterval`") 
_misc_.TimerEvent_swigregister(TimerEvent)

class TimerRunner(object):
    """Proxy of C++ TimerRunner class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, wxTimer timer) -> TimerRunner
        __init__(self, wxTimer timer, int milli, bool oneShot=False) -> TimerRunner
        """
        _misc_.TimerRunner_swiginit(self,_misc_.new_TimerRunner(*args))
    __swig_destroy__ = _misc_.delete_TimerRunner
    __del__ = lambda self : None;
    def Start(*args, **kwargs):
        """Start(self, int milli, bool oneShot=False)"""
        return _misc_.TimerRunner_Start(*args, **kwargs)

_misc_.TimerRunner_swigregister(TimerRunner)

#---------------------------------------------------------------------------

LOG_FatalError = _misc_.LOG_FatalError
LOG_Error = _misc_.LOG_Error
LOG_Warning = _misc_.LOG_Warning
LOG_Message = _misc_.LOG_Message
LOG_Status = _misc_.LOG_Status
LOG_Info = _misc_.LOG_Info
LOG_Debug = _misc_.LOG_Debug
LOG_Trace = _misc_.LOG_Trace
LOG_Progress = _misc_.LOG_Progress
LOG_User = _misc_.LOG_User
LOG_Max = _misc_.LOG_Max
TRACE_MemAlloc = _misc_.TRACE_MemAlloc
TRACE_Messages = _misc_.TRACE_Messages
TRACE_ResAlloc = _misc_.TRACE_ResAlloc
TRACE_RefCount = _misc_.TRACE_RefCount
TRACE_OleCalls = _misc_.TRACE_OleCalls
TraceMemAlloc = _misc_.TraceMemAlloc
TraceMessages = _misc_.TraceMessages
TraceResAlloc = _misc_.TraceResAlloc
TraceRefCount = _misc_.TraceRefCount
TraceOleCalls = _misc_.TraceOleCalls
class Log(object):
    """Proxy of C++ Log class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> Log"""
        _misc_.Log_swiginit(self,_misc_.new_Log(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Log
    __del__ = lambda self : None;
    def IsEnabled(*args, **kwargs):
        """IsEnabled() -> bool"""
        return _misc_.Log_IsEnabled(*args, **kwargs)

    IsEnabled = staticmethod(IsEnabled)
    def EnableLogging(*args, **kwargs):
        """EnableLogging(bool doIt=True) -> bool"""
        return _misc_.Log_EnableLogging(*args, **kwargs)

    EnableLogging = staticmethod(EnableLogging)
    def OnLog(*args, **kwargs):
        """OnLog(LogLevel level, wxChar szString, time_t t)"""
        return _misc_.Log_OnLog(*args, **kwargs)

    OnLog = staticmethod(OnLog)
    def Flush(*args, **kwargs):
        """Flush(self)"""
        return _misc_.Log_Flush(*args, **kwargs)

    def FlushActive(*args, **kwargs):
        """FlushActive()"""
        return _misc_.Log_FlushActive(*args, **kwargs)

    FlushActive = staticmethod(FlushActive)
    def GetActiveTarget(*args, **kwargs):
        """GetActiveTarget() -> Log"""
        return _misc_.Log_GetActiveTarget(*args, **kwargs)

    GetActiveTarget = staticmethod(GetActiveTarget)
    def SetActiveTarget(*args, **kwargs):
        """SetActiveTarget(Log pLogger) -> Log"""
        return _misc_.Log_SetActiveTarget(*args, **kwargs)

    SetActiveTarget = staticmethod(SetActiveTarget)
    def Suspend(*args, **kwargs):
        """Suspend()"""
        return _misc_.Log_Suspend(*args, **kwargs)

    Suspend = staticmethod(Suspend)
    def Resume(*args, **kwargs):
        """Resume()"""
        return _misc_.Log_Resume(*args, **kwargs)

    Resume = staticmethod(Resume)
    def SetVerbose(*args, **kwargs):
        """SetVerbose(bool bVerbose=True)"""
        return _misc_.Log_SetVerbose(*args, **kwargs)

    SetVerbose = staticmethod(SetVerbose)
    def SetLogLevel(*args, **kwargs):
        """SetLogLevel(LogLevel logLevel)"""
        return _misc_.Log_SetLogLevel(*args, **kwargs)

    SetLogLevel = staticmethod(SetLogLevel)
    def DontCreateOnDemand(*args, **kwargs):
        """DontCreateOnDemand()"""
        return _misc_.Log_DontCreateOnDemand(*args, **kwargs)

    DontCreateOnDemand = staticmethod(DontCreateOnDemand)
    def SetRepetitionCounting(*args, **kwargs):
        """SetRepetitionCounting(bool bRepetCounting=True)"""
        return _misc_.Log_SetRepetitionCounting(*args, **kwargs)

    SetRepetitionCounting = staticmethod(SetRepetitionCounting)
    def GetRepetitionCounting(*args, **kwargs):
        """GetRepetitionCounting() -> bool"""
        return _misc_.Log_GetRepetitionCounting(*args, **kwargs)

    GetRepetitionCounting = staticmethod(GetRepetitionCounting)
    def SetTraceMask(*args, **kwargs):
        """SetTraceMask(TraceMask ulMask)"""
        return _misc_.Log_SetTraceMask(*args, **kwargs)

    SetTraceMask = staticmethod(SetTraceMask)
    def AddTraceMask(*args, **kwargs):
        """AddTraceMask(String str)"""
        return _misc_.Log_AddTraceMask(*args, **kwargs)

    AddTraceMask = staticmethod(AddTraceMask)
    def RemoveTraceMask(*args, **kwargs):
        """RemoveTraceMask(String str)"""
        return _misc_.Log_RemoveTraceMask(*args, **kwargs)

    RemoveTraceMask = staticmethod(RemoveTraceMask)
    def ClearTraceMasks(*args, **kwargs):
        """ClearTraceMasks()"""
        return _misc_.Log_ClearTraceMasks(*args, **kwargs)

    ClearTraceMasks = staticmethod(ClearTraceMasks)
    def GetTraceMasks(*args, **kwargs):
        """GetTraceMasks() -> wxArrayString"""
        return _misc_.Log_GetTraceMasks(*args, **kwargs)

    GetTraceMasks = staticmethod(GetTraceMasks)
    def SetTimestamp(*args, **kwargs):
        """SetTimestamp(wxChar ts)"""
        return _misc_.Log_SetTimestamp(*args, **kwargs)

    SetTimestamp = staticmethod(SetTimestamp)
    def GetVerbose(*args, **kwargs):
        """GetVerbose() -> bool"""
        return _misc_.Log_GetVerbose(*args, **kwargs)

    GetVerbose = staticmethod(GetVerbose)
    def GetTraceMask(*args, **kwargs):
        """GetTraceMask() -> TraceMask"""
        return _misc_.Log_GetTraceMask(*args, **kwargs)

    GetTraceMask = staticmethod(GetTraceMask)
    def IsAllowedTraceMask(*args, **kwargs):
        """IsAllowedTraceMask(wxChar mask) -> bool"""
        return _misc_.Log_IsAllowedTraceMask(*args, **kwargs)

    IsAllowedTraceMask = staticmethod(IsAllowedTraceMask)
    def GetLogLevel(*args, **kwargs):
        """GetLogLevel() -> LogLevel"""
        return _misc_.Log_GetLogLevel(*args, **kwargs)

    GetLogLevel = staticmethod(GetLogLevel)
    def GetTimestamp(*args, **kwargs):
        """GetTimestamp() -> wxChar"""
        return _misc_.Log_GetTimestamp(*args, **kwargs)

    GetTimestamp = staticmethod(GetTimestamp)
    def TimeStamp(*args, **kwargs):
        """TimeStamp() -> String"""
        return _misc_.Log_TimeStamp(*args, **kwargs)

    TimeStamp = staticmethod(TimeStamp)
    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        val = _misc_.Log_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

_misc_.Log_swigregister(Log)

def Log_IsEnabled(*args):
  """Log_IsEnabled() -> bool"""
  return _misc_.Log_IsEnabled(*args)

def Log_EnableLogging(*args, **kwargs):
  """Log_EnableLogging(bool doIt=True) -> bool"""
  return _misc_.Log_EnableLogging(*args, **kwargs)

def Log_OnLog(*args, **kwargs):
  """Log_OnLog(LogLevel level, wxChar szString, time_t t)"""
  return _misc_.Log_OnLog(*args, **kwargs)

def Log_FlushActive(*args):
  """Log_FlushActive()"""
  return _misc_.Log_FlushActive(*args)

def Log_GetActiveTarget(*args):
  """Log_GetActiveTarget() -> Log"""
  return _misc_.Log_GetActiveTarget(*args)

def Log_SetActiveTarget(*args, **kwargs):
  """Log_SetActiveTarget(Log pLogger) -> Log"""
  return _misc_.Log_SetActiveTarget(*args, **kwargs)

def Log_Suspend(*args):
  """Log_Suspend()"""
  return _misc_.Log_Suspend(*args)

def Log_Resume(*args):
  """Log_Resume()"""
  return _misc_.Log_Resume(*args)

def Log_SetVerbose(*args, **kwargs):
  """Log_SetVerbose(bool bVerbose=True)"""
  return _misc_.Log_SetVerbose(*args, **kwargs)

def Log_SetLogLevel(*args, **kwargs):
  """Log_SetLogLevel(LogLevel logLevel)"""
  return _misc_.Log_SetLogLevel(*args, **kwargs)

def Log_DontCreateOnDemand(*args):
  """Log_DontCreateOnDemand()"""
  return _misc_.Log_DontCreateOnDemand(*args)

def Log_SetRepetitionCounting(*args, **kwargs):
  """Log_SetRepetitionCounting(bool bRepetCounting=True)"""
  return _misc_.Log_SetRepetitionCounting(*args, **kwargs)

def Log_GetRepetitionCounting(*args):
  """Log_GetRepetitionCounting() -> bool"""
  return _misc_.Log_GetRepetitionCounting(*args)

def Log_SetTraceMask(*args, **kwargs):
  """Log_SetTraceMask(TraceMask ulMask)"""
  return _misc_.Log_SetTraceMask(*args, **kwargs)

def Log_AddTraceMask(*args, **kwargs):
  """Log_AddTraceMask(String str)"""
  return _misc_.Log_AddTraceMask(*args, **kwargs)

def Log_RemoveTraceMask(*args, **kwargs):
  """Log_RemoveTraceMask(String str)"""
  return _misc_.Log_RemoveTraceMask(*args, **kwargs)

def Log_ClearTraceMasks(*args):
  """Log_ClearTraceMasks()"""
  return _misc_.Log_ClearTraceMasks(*args)

def Log_GetTraceMasks(*args):
  """Log_GetTraceMasks() -> wxArrayString"""
  return _misc_.Log_GetTraceMasks(*args)

def Log_SetTimestamp(*args, **kwargs):
  """Log_SetTimestamp(wxChar ts)"""
  return _misc_.Log_SetTimestamp(*args, **kwargs)

def Log_GetVerbose(*args):
  """Log_GetVerbose() -> bool"""
  return _misc_.Log_GetVerbose(*args)

def Log_GetTraceMask(*args):
  """Log_GetTraceMask() -> TraceMask"""
  return _misc_.Log_GetTraceMask(*args)

def Log_IsAllowedTraceMask(*args, **kwargs):
  """Log_IsAllowedTraceMask(wxChar mask) -> bool"""
  return _misc_.Log_IsAllowedTraceMask(*args, **kwargs)

def Log_GetLogLevel(*args):
  """Log_GetLogLevel() -> LogLevel"""
  return _misc_.Log_GetLogLevel(*args)

def Log_GetTimestamp(*args):
  """Log_GetTimestamp() -> wxChar"""
  return _misc_.Log_GetTimestamp(*args)

def Log_TimeStamp(*args):
  """Log_TimeStamp() -> String"""
  return _misc_.Log_TimeStamp(*args)

class LogStderr(Log):
    """Proxy of C++ LogStderr class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> LogStderr"""
        _misc_.LogStderr_swiginit(self,_misc_.new_LogStderr(*args, **kwargs))
_misc_.LogStderr_swigregister(LogStderr)

class LogTextCtrl(Log):
    """Proxy of C++ LogTextCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, wxTextCtrl pTextCtrl) -> LogTextCtrl"""
        _misc_.LogTextCtrl_swiginit(self,_misc_.new_LogTextCtrl(*args, **kwargs))
_misc_.LogTextCtrl_swigregister(LogTextCtrl)

class LogGui(Log):
    """Proxy of C++ LogGui class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> LogGui"""
        _misc_.LogGui_swiginit(self,_misc_.new_LogGui(*args, **kwargs))
_misc_.LogGui_swigregister(LogGui)

class LogWindow(Log):
    """Proxy of C++ LogWindow class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, wxFrame pParent, String szTitle, bool bShow=True, bool bPassToOld=True) -> LogWindow"""
        _misc_.LogWindow_swiginit(self,_misc_.new_LogWindow(*args, **kwargs))
    def Show(*args, **kwargs):
        """Show(self, bool bShow=True)"""
        return _misc_.LogWindow_Show(*args, **kwargs)

    def GetFrame(*args, **kwargs):
        """GetFrame(self) -> wxFrame"""
        return _misc_.LogWindow_GetFrame(*args, **kwargs)

    def GetOldLog(*args, **kwargs):
        """GetOldLog(self) -> Log"""
        return _misc_.LogWindow_GetOldLog(*args, **kwargs)

    def IsPassingMessages(*args, **kwargs):
        """IsPassingMessages(self) -> bool"""
        return _misc_.LogWindow_IsPassingMessages(*args, **kwargs)

    def PassMessages(*args, **kwargs):
        """PassMessages(self, bool bDoPass)"""
        return _misc_.LogWindow_PassMessages(*args, **kwargs)

    Frame = property(GetFrame,doc="See `GetFrame`") 
    OldLog = property(GetOldLog,doc="See `GetOldLog`") 
_misc_.LogWindow_swigregister(LogWindow)

class LogChain(Log):
    """Proxy of C++ LogChain class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Log logger) -> LogChain"""
        _misc_.LogChain_swiginit(self,_misc_.new_LogChain(*args, **kwargs))
    def SetLog(*args, **kwargs):
        """SetLog(self, Log logger)"""
        return _misc_.LogChain_SetLog(*args, **kwargs)

    def PassMessages(*args, **kwargs):
        """PassMessages(self, bool bDoPass)"""
        return _misc_.LogChain_PassMessages(*args, **kwargs)

    def IsPassingMessages(*args, **kwargs):
        """IsPassingMessages(self) -> bool"""
        return _misc_.LogChain_IsPassingMessages(*args, **kwargs)

    def GetOldLog(*args, **kwargs):
        """GetOldLog(self) -> Log"""
        return _misc_.LogChain_GetOldLog(*args, **kwargs)

    OldLog = property(GetOldLog,doc="See `GetOldLog`") 
_misc_.LogChain_swigregister(LogChain)

class LogBuffer(Log):
    """Proxy of C++ LogBuffer class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> LogBuffer"""
        _misc_.LogBuffer_swiginit(self,_misc_.new_LogBuffer(*args, **kwargs))
    def GetBuffer(*args, **kwargs):
        """GetBuffer(self) -> String"""
        return _misc_.LogBuffer_GetBuffer(*args, **kwargs)

    Buffer = property(GetBuffer,doc="See `GetBuffer`") 
_misc_.LogBuffer_swigregister(LogBuffer)


def SysErrorCode(*args):
  """SysErrorCode() -> unsigned long"""
  return _misc_.SysErrorCode(*args)

def SysErrorMsg(*args, **kwargs):
  """SysErrorMsg(unsigned long nErrCode=0) -> String"""
  return _misc_.SysErrorMsg(*args, **kwargs)

def LogFatalError(*args, **kwargs):
  """LogFatalError(String msg)"""
  return _misc_.LogFatalError(*args, **kwargs)

def LogError(*args, **kwargs):
  """LogError(String msg)"""
  return _misc_.LogError(*args, **kwargs)

def LogWarning(*args, **kwargs):
  """LogWarning(String msg)"""
  return _misc_.LogWarning(*args, **kwargs)

def LogMessage(*args, **kwargs):
  """LogMessage(String msg)"""
  return _misc_.LogMessage(*args, **kwargs)

def LogInfo(*args, **kwargs):
  """LogInfo(String msg)"""
  return _misc_.LogInfo(*args, **kwargs)

def LogDebug(*args, **kwargs):
  """LogDebug(String msg)"""
  return _misc_.LogDebug(*args, **kwargs)

def LogVerbose(*args, **kwargs):
  """LogVerbose(String msg)"""
  return _misc_.LogVerbose(*args, **kwargs)

def LogStatus(*args, **kwargs):
  """LogStatus(String msg)"""
  return _misc_.LogStatus(*args, **kwargs)

def LogStatusFrame(*args, **kwargs):
  """LogStatusFrame(wxFrame pFrame, String msg)"""
  return _misc_.LogStatusFrame(*args, **kwargs)

def LogSysError(*args, **kwargs):
  """LogSysError(String msg)"""
  return _misc_.LogSysError(*args, **kwargs)

def LogGeneric(*args, **kwargs):
  """LogGeneric(unsigned long level, String msg)"""
  return _misc_.LogGeneric(*args, **kwargs)

def SafeShowMessage(*args, **kwargs):
  """SafeShowMessage(String title, String text)"""
  return _misc_.SafeShowMessage(*args, **kwargs)
class LogNull(object):
    """Proxy of C++ LogNull class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> LogNull"""
        _misc_.LogNull_swiginit(self,_misc_.new_LogNull(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_LogNull
    __del__ = lambda self : None;
_misc_.LogNull_swigregister(LogNull)

def LogTrace(*args):
  """
    LogTrace(unsigned long mask, String msg)
    LogTrace(String mask, String msg)
    """
  return _misc_.LogTrace(*args)

class PyLog(Log):
    """Proxy of C++ PyLog class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> PyLog"""
        _misc_.PyLog_swiginit(self,_misc_.new_PyLog(*args, **kwargs))
        self._setCallbackInfo(self, PyLog)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.PyLog__setCallbackInfo(*args, **kwargs)

_misc_.PyLog_swigregister(PyLog)

#---------------------------------------------------------------------------

PROCESS_DEFAULT = _misc_.PROCESS_DEFAULT
PROCESS_REDIRECT = _misc_.PROCESS_REDIRECT
KILL_OK = _misc_.KILL_OK
KILL_BAD_SIGNAL = _misc_.KILL_BAD_SIGNAL
KILL_ACCESS_DENIED = _misc_.KILL_ACCESS_DENIED
KILL_NO_PROCESS = _misc_.KILL_NO_PROCESS
KILL_ERROR = _misc_.KILL_ERROR
KILL_NOCHILDREN = _misc_.KILL_NOCHILDREN
KILL_CHILDREN = _misc_.KILL_CHILDREN
SIGNONE = _misc_.SIGNONE
SIGHUP = _misc_.SIGHUP
SIGINT = _misc_.SIGINT
SIGQUIT = _misc_.SIGQUIT
SIGILL = _misc_.SIGILL
SIGTRAP = _misc_.SIGTRAP
SIGABRT = _misc_.SIGABRT
SIGIOT = _misc_.SIGIOT
SIGEMT = _misc_.SIGEMT
SIGFPE = _misc_.SIGFPE
SIGKILL = _misc_.SIGKILL
SIGBUS = _misc_.SIGBUS
SIGSEGV = _misc_.SIGSEGV
SIGSYS = _misc_.SIGSYS
SIGPIPE = _misc_.SIGPIPE
SIGALRM = _misc_.SIGALRM
SIGTERM = _misc_.SIGTERM
class Process(_core.EvtHandler):
    """Proxy of C++ Process class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def Kill(*args, **kwargs):
        """Kill(int pid, int sig=SIGTERM, int flags=KILL_NOCHILDREN) -> int"""
        return _misc_.Process_Kill(*args, **kwargs)

    Kill = staticmethod(Kill)
    def Exists(*args, **kwargs):
        """Exists(int pid) -> bool"""
        return _misc_.Process_Exists(*args, **kwargs)

    Exists = staticmethod(Exists)
    def Open(*args, **kwargs):
        """Open(String cmd, int flags=EXEC_ASYNC) -> Process"""
        return _misc_.Process_Open(*args, **kwargs)

    Open = staticmethod(Open)
    def __init__(self, *args, **kwargs): 
        """__init__(self, EvtHandler parent=None, int id=-1) -> Process"""
        _misc_.Process_swiginit(self,_misc_.new_Process(*args, **kwargs))
        self._setCallbackInfo(self, Process)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.Process__setCallbackInfo(*args, **kwargs)

    def OnTerminate(*args, **kwargs):
        """OnTerminate(self, int pid, int status)"""
        return _misc_.Process_OnTerminate(*args, **kwargs)

    def base_OnTerminate(*args, **kw):
        return Process.OnTerminate(*args, **kw)
    base_OnTerminate = wx._deprecated(base_OnTerminate,
                                   "Please use Process.OnTerminate instead.")

    def Redirect(*args, **kwargs):
        """Redirect(self)"""
        return _misc_.Process_Redirect(*args, **kwargs)

    def IsRedirected(*args, **kwargs):
        """IsRedirected(self) -> bool"""
        return _misc_.Process_IsRedirected(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return _misc_.Process_Detach(*args, **kwargs)

    def GetInputStream(*args, **kwargs):
        """GetInputStream(self) -> InputStream"""
        return _misc_.Process_GetInputStream(*args, **kwargs)

    def GetErrorStream(*args, **kwargs):
        """GetErrorStream(self) -> InputStream"""
        return _misc_.Process_GetErrorStream(*args, **kwargs)

    def GetOutputStream(*args, **kwargs):
        """GetOutputStream(self) -> OutputStream"""
        return _misc_.Process_GetOutputStream(*args, **kwargs)

    def CloseOutput(*args, **kwargs):
        """CloseOutput(self)"""
        return _misc_.Process_CloseOutput(*args, **kwargs)

    def IsInputOpened(*args, **kwargs):
        """IsInputOpened(self) -> bool"""
        return _misc_.Process_IsInputOpened(*args, **kwargs)

    def IsInputAvailable(*args, **kwargs):
        """IsInputAvailable(self) -> bool"""
        return _misc_.Process_IsInputAvailable(*args, **kwargs)

    def IsErrorAvailable(*args, **kwargs):
        """IsErrorAvailable(self) -> bool"""
        return _misc_.Process_IsErrorAvailable(*args, **kwargs)

    ErrorStream = property(GetErrorStream,doc="See `GetErrorStream`") 
    InputStream = property(GetInputStream,doc="See `GetInputStream`") 
    OutputStream = property(GetOutputStream,doc="See `GetOutputStream`") 
    InputOpened = property(IsInputOpened) 
    InputAvailable = property(IsInputAvailable) 
    ErrorAvailable = property(IsErrorAvailable) 
_misc_.Process_swigregister(Process)

def Process_Kill(*args, **kwargs):
  """Process_Kill(int pid, int sig=SIGTERM, int flags=KILL_NOCHILDREN) -> int"""
  return _misc_.Process_Kill(*args, **kwargs)

def Process_Exists(*args, **kwargs):
  """Process_Exists(int pid) -> bool"""
  return _misc_.Process_Exists(*args, **kwargs)

def Process_Open(*args, **kwargs):
  """Process_Open(String cmd, int flags=EXEC_ASYNC) -> Process"""
  return _misc_.Process_Open(*args, **kwargs)

class ProcessEvent(_core.Event):
    """Proxy of C++ ProcessEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int id=0, int pid=0, int exitcode=0) -> ProcessEvent"""
        _misc_.ProcessEvent_swiginit(self,_misc_.new_ProcessEvent(*args, **kwargs))
    def GetPid(*args, **kwargs):
        """GetPid(self) -> int"""
        return _misc_.ProcessEvent_GetPid(*args, **kwargs)

    def GetExitCode(*args, **kwargs):
        """GetExitCode(self) -> int"""
        return _misc_.ProcessEvent_GetExitCode(*args, **kwargs)

    m_pid = property(_misc_.ProcessEvent_m_pid_get, _misc_.ProcessEvent_m_pid_set)
    m_exitcode = property(_misc_.ProcessEvent_m_exitcode_get, _misc_.ProcessEvent_m_exitcode_set)
    ExitCode = property(GetExitCode,doc="See `GetExitCode`") 
    Pid = property(GetPid,doc="See `GetPid`") 
_misc_.ProcessEvent_swigregister(ProcessEvent)

wxEVT_END_PROCESS = _misc_.wxEVT_END_PROCESS
EVT_END_PROCESS = wx.PyEventBinder( wxEVT_END_PROCESS, 1 )

EXEC_ASYNC = _misc_.EXEC_ASYNC
EXEC_SYNC = _misc_.EXEC_SYNC
EXEC_NOHIDE = _misc_.EXEC_NOHIDE
EXEC_MAKE_GROUP_LEADER = _misc_.EXEC_MAKE_GROUP_LEADER
EXEC_NODISABLE = _misc_.EXEC_NODISABLE

def Execute(*args, **kwargs):
  """Execute(String command, int flags=EXEC_ASYNC, Process process=None) -> long"""
  return _misc_.Execute(*args, **kwargs)

def Kill(*args, **kwargs):
  """Kill(long pid, int sig=SIGTERM, int rc, int flags=KILL_NOCHILDREN) -> int"""
  return _misc_.Kill(*args, **kwargs)
#---------------------------------------------------------------------------

JOYSTICK1 = _misc_.JOYSTICK1
JOYSTICK2 = _misc_.JOYSTICK2
JOY_BUTTON_ANY = _misc_.JOY_BUTTON_ANY
JOY_BUTTON1 = _misc_.JOY_BUTTON1
JOY_BUTTON2 = _misc_.JOY_BUTTON2
JOY_BUTTON3 = _misc_.JOY_BUTTON3
JOY_BUTTON4 = _misc_.JOY_BUTTON4
class Joystick(object):
    """Proxy of C++ Joystick class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int joystick=JOYSTICK1) -> Joystick"""
        _misc_.Joystick_swiginit(self,_misc_.new_Joystick(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Joystick
    __del__ = lambda self : None;
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _misc_.Joystick_GetPosition(*args, **kwargs)

    def GetZPosition(*args, **kwargs):
        """GetZPosition(self) -> int"""
        return _misc_.Joystick_GetZPosition(*args, **kwargs)

    def GetButtonState(*args, **kwargs):
        """GetButtonState(self) -> int"""
        return _misc_.Joystick_GetButtonState(*args, **kwargs)

    def GetPOVPosition(*args, **kwargs):
        """GetPOVPosition(self) -> int"""
        return _misc_.Joystick_GetPOVPosition(*args, **kwargs)

    def GetPOVCTSPosition(*args, **kwargs):
        """GetPOVCTSPosition(self) -> int"""
        return _misc_.Joystick_GetPOVCTSPosition(*args, **kwargs)

    def GetRudderPosition(*args, **kwargs):
        """GetRudderPosition(self) -> int"""
        return _misc_.Joystick_GetRudderPosition(*args, **kwargs)

    def GetUPosition(*args, **kwargs):
        """GetUPosition(self) -> int"""
        return _misc_.Joystick_GetUPosition(*args, **kwargs)

    def GetVPosition(*args, **kwargs):
        """GetVPosition(self) -> int"""
        return _misc_.Joystick_GetVPosition(*args, **kwargs)

    def GetMovementThreshold(*args, **kwargs):
        """GetMovementThreshold(self) -> int"""
        return _misc_.Joystick_GetMovementThreshold(*args, **kwargs)

    def SetMovementThreshold(*args, **kwargs):
        """SetMovementThreshold(self, int threshold)"""
        return _misc_.Joystick_SetMovementThreshold(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _misc_.Joystick_IsOk(*args, **kwargs)

    def GetNumberJoysticks(*args, **kwargs):
        """GetNumberJoysticks(self) -> int"""
        return _misc_.Joystick_GetNumberJoysticks(*args, **kwargs)

    def GetManufacturerId(*args, **kwargs):
        """GetManufacturerId(self) -> int"""
        return _misc_.Joystick_GetManufacturerId(*args, **kwargs)

    def GetProductId(*args, **kwargs):
        """GetProductId(self) -> int"""
        return _misc_.Joystick_GetProductId(*args, **kwargs)

    def GetProductName(*args, **kwargs):
        """GetProductName(self) -> String"""
        return _misc_.Joystick_GetProductName(*args, **kwargs)

    def GetXMin(*args, **kwargs):
        """GetXMin(self) -> int"""
        return _misc_.Joystick_GetXMin(*args, **kwargs)

    def GetYMin(*args, **kwargs):
        """GetYMin(self) -> int"""
        return _misc_.Joystick_GetYMin(*args, **kwargs)

    def GetZMin(*args, **kwargs):
        """GetZMin(self) -> int"""
        return _misc_.Joystick_GetZMin(*args, **kwargs)

    def GetXMax(*args, **kwargs):
        """GetXMax(self) -> int"""
        return _misc_.Joystick_GetXMax(*args, **kwargs)

    def GetYMax(*args, **kwargs):
        """GetYMax(self) -> int"""
        return _misc_.Joystick_GetYMax(*args, **kwargs)

    def GetZMax(*args, **kwargs):
        """GetZMax(self) -> int"""
        return _misc_.Joystick_GetZMax(*args, **kwargs)

    def GetNumberButtons(*args, **kwargs):
        """GetNumberButtons(self) -> int"""
        return _misc_.Joystick_GetNumberButtons(*args, **kwargs)

    def GetNumberAxes(*args, **kwargs):
        """GetNumberAxes(self) -> int"""
        return _misc_.Joystick_GetNumberAxes(*args, **kwargs)

    def GetMaxButtons(*args, **kwargs):
        """GetMaxButtons(self) -> int"""
        return _misc_.Joystick_GetMaxButtons(*args, **kwargs)

    def GetMaxAxes(*args, **kwargs):
        """GetMaxAxes(self) -> int"""
        return _misc_.Joystick_GetMaxAxes(*args, **kwargs)

    def GetPollingMin(*args, **kwargs):
        """GetPollingMin(self) -> int"""
        return _misc_.Joystick_GetPollingMin(*args, **kwargs)

    def GetPollingMax(*args, **kwargs):
        """GetPollingMax(self) -> int"""
        return _misc_.Joystick_GetPollingMax(*args, **kwargs)

    def GetRudderMin(*args, **kwargs):
        """GetRudderMin(self) -> int"""
        return _misc_.Joystick_GetRudderMin(*args, **kwargs)

    def GetRudderMax(*args, **kwargs):
        """GetRudderMax(self) -> int"""
        return _misc_.Joystick_GetRudderMax(*args, **kwargs)

    def GetUMin(*args, **kwargs):
        """GetUMin(self) -> int"""
        return _misc_.Joystick_GetUMin(*args, **kwargs)

    def GetUMax(*args, **kwargs):
        """GetUMax(self) -> int"""
        return _misc_.Joystick_GetUMax(*args, **kwargs)

    def GetVMin(*args, **kwargs):
        """GetVMin(self) -> int"""
        return _misc_.Joystick_GetVMin(*args, **kwargs)

    def GetVMax(*args, **kwargs):
        """GetVMax(self) -> int"""
        return _misc_.Joystick_GetVMax(*args, **kwargs)

    def HasRudder(*args, **kwargs):
        """HasRudder(self) -> bool"""
        return _misc_.Joystick_HasRudder(*args, **kwargs)

    def HasZ(*args, **kwargs):
        """HasZ(self) -> bool"""
        return _misc_.Joystick_HasZ(*args, **kwargs)

    def HasU(*args, **kwargs):
        """HasU(self) -> bool"""
        return _misc_.Joystick_HasU(*args, **kwargs)

    def HasV(*args, **kwargs):
        """HasV(self) -> bool"""
        return _misc_.Joystick_HasV(*args, **kwargs)

    def HasPOV(*args, **kwargs):
        """HasPOV(self) -> bool"""
        return _misc_.Joystick_HasPOV(*args, **kwargs)

    def HasPOV4Dir(*args, **kwargs):
        """HasPOV4Dir(self) -> bool"""
        return _misc_.Joystick_HasPOV4Dir(*args, **kwargs)

    def HasPOVCTS(*args, **kwargs):
        """HasPOVCTS(self) -> bool"""
        return _misc_.Joystick_HasPOVCTS(*args, **kwargs)

    def SetCapture(*args, **kwargs):
        """SetCapture(self, Window win, int pollingFreq=0) -> bool"""
        return _misc_.Joystick_SetCapture(*args, **kwargs)

    def ReleaseCapture(*args, **kwargs):
        """ReleaseCapture(self) -> bool"""
        return _misc_.Joystick_ReleaseCapture(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    ButtonState = property(GetButtonState,doc="See `GetButtonState`") 
    ManufacturerId = property(GetManufacturerId,doc="See `GetManufacturerId`") 
    MaxAxes = property(GetMaxAxes,doc="See `GetMaxAxes`") 
    MaxButtons = property(GetMaxButtons,doc="See `GetMaxButtons`") 
    MovementThreshold = property(GetMovementThreshold,SetMovementThreshold,doc="See `GetMovementThreshold` and `SetMovementThreshold`") 
    NumberAxes = property(GetNumberAxes,doc="See `GetNumberAxes`") 
    NumberButtons = property(GetNumberButtons,doc="See `GetNumberButtons`") 
    NumberJoysticks = property(GetNumberJoysticks,doc="See `GetNumberJoysticks`") 
    POVCTSPosition = property(GetPOVCTSPosition,doc="See `GetPOVCTSPosition`") 
    POVPosition = property(GetPOVPosition,doc="See `GetPOVPosition`") 
    PollingMax = property(GetPollingMax,doc="See `GetPollingMax`") 
    PollingMin = property(GetPollingMin,doc="See `GetPollingMin`") 
    Position = property(GetPosition,doc="See `GetPosition`") 
    ProductId = property(GetProductId,doc="See `GetProductId`") 
    ProductName = property(GetProductName,doc="See `GetProductName`") 
    RudderMax = property(GetRudderMax,doc="See `GetRudderMax`") 
    RudderMin = property(GetRudderMin,doc="See `GetRudderMin`") 
    RudderPosition = property(GetRudderPosition,doc="See `GetRudderPosition`") 
    UMax = property(GetUMax,doc="See `GetUMax`") 
    UMin = property(GetUMin,doc="See `GetUMin`") 
    UPosition = property(GetUPosition,doc="See `GetUPosition`") 
    VMax = property(GetVMax,doc="See `GetVMax`") 
    VMin = property(GetVMin,doc="See `GetVMin`") 
    VPosition = property(GetVPosition,doc="See `GetVPosition`") 
    XMax = property(GetXMax,doc="See `GetXMax`") 
    XMin = property(GetXMin,doc="See `GetXMin`") 
    YMax = property(GetYMax,doc="See `GetYMax`") 
    YMin = property(GetYMin,doc="See `GetYMin`") 
    ZMax = property(GetZMax,doc="See `GetZMax`") 
    ZMin = property(GetZMin,doc="See `GetZMin`") 
    ZPosition = property(GetZPosition,doc="See `GetZPosition`") 
_misc_.Joystick_swigregister(Joystick)

wxEVT_JOY_BUTTON_DOWN = _misc_.wxEVT_JOY_BUTTON_DOWN
wxEVT_JOY_BUTTON_UP = _misc_.wxEVT_JOY_BUTTON_UP
wxEVT_JOY_MOVE = _misc_.wxEVT_JOY_MOVE
wxEVT_JOY_ZMOVE = _misc_.wxEVT_JOY_ZMOVE
class JoystickEvent(_core.Event):
    """Proxy of C++ JoystickEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int state=0, int joystick=JOYSTICK1, 
            int change=0) -> JoystickEvent
        """
        _misc_.JoystickEvent_swiginit(self,_misc_.new_JoystickEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _misc_.JoystickEvent_GetPosition(*args, **kwargs)

    def GetZPosition(*args, **kwargs):
        """GetZPosition(self) -> int"""
        return _misc_.JoystickEvent_GetZPosition(*args, **kwargs)

    def GetButtonState(*args, **kwargs):
        """GetButtonState(self) -> int"""
        return _misc_.JoystickEvent_GetButtonState(*args, **kwargs)

    def GetButtonChange(*args, **kwargs):
        """GetButtonChange(self) -> int"""
        return _misc_.JoystickEvent_GetButtonChange(*args, **kwargs)

    def GetJoystick(*args, **kwargs):
        """GetJoystick(self) -> int"""
        return _misc_.JoystickEvent_GetJoystick(*args, **kwargs)

    def SetJoystick(*args, **kwargs):
        """SetJoystick(self, int stick)"""
        return _misc_.JoystickEvent_SetJoystick(*args, **kwargs)

    def SetButtonState(*args, **kwargs):
        """SetButtonState(self, int state)"""
        return _misc_.JoystickEvent_SetButtonState(*args, **kwargs)

    def SetButtonChange(*args, **kwargs):
        """SetButtonChange(self, int change)"""
        return _misc_.JoystickEvent_SetButtonChange(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, Point pos)"""
        return _misc_.JoystickEvent_SetPosition(*args, **kwargs)

    def SetZPosition(*args, **kwargs):
        """SetZPosition(self, int zPos)"""
        return _misc_.JoystickEvent_SetZPosition(*args, **kwargs)

    def IsButton(*args, **kwargs):
        """IsButton(self) -> bool"""
        return _misc_.JoystickEvent_IsButton(*args, **kwargs)

    def IsMove(*args, **kwargs):
        """IsMove(self) -> bool"""
        return _misc_.JoystickEvent_IsMove(*args, **kwargs)

    def IsZMove(*args, **kwargs):
        """IsZMove(self) -> bool"""
        return _misc_.JoystickEvent_IsZMove(*args, **kwargs)

    def ButtonDown(*args, **kwargs):
        """ButtonDown(self, int but=JOY_BUTTON_ANY) -> bool"""
        return _misc_.JoystickEvent_ButtonDown(*args, **kwargs)

    def ButtonUp(*args, **kwargs):
        """ButtonUp(self, int but=JOY_BUTTON_ANY) -> bool"""
        return _misc_.JoystickEvent_ButtonUp(*args, **kwargs)

    def ButtonIsDown(*args, **kwargs):
        """ButtonIsDown(self, int but=JOY_BUTTON_ANY) -> bool"""
        return _misc_.JoystickEvent_ButtonIsDown(*args, **kwargs)

    m_pos = property(GetPosition, SetPosition)
    m_zPosition = property(GetZPosition, SetZPosition)
    m_buttonChange = property(GetButtonChange, SetButtonChange)
    m_buttonState = property(GetButtonState, SetButtonState)
    m_joyStick = property(GetJoystick, SetJoystick)

    ButtonChange = property(GetButtonChange,SetButtonChange,doc="See `GetButtonChange` and `SetButtonChange`") 
    ButtonState = property(GetButtonState,SetButtonState,doc="See `GetButtonState` and `SetButtonState`") 
    Joystick = property(GetJoystick,SetJoystick,doc="See `GetJoystick` and `SetJoystick`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
    ZPosition = property(GetZPosition,SetZPosition,doc="See `GetZPosition` and `SetZPosition`") 
_misc_.JoystickEvent_swigregister(JoystickEvent)

EVT_JOY_BUTTON_DOWN = wx.PyEventBinder( wxEVT_JOY_BUTTON_DOWN )
EVT_JOY_BUTTON_UP = wx.PyEventBinder( wxEVT_JOY_BUTTON_UP )
EVT_JOY_MOVE = wx.PyEventBinder( wxEVT_JOY_MOVE )
EVT_JOY_ZMOVE = wx.PyEventBinder( wxEVT_JOY_ZMOVE )

EVT_JOYSTICK_EVENTS = wx.PyEventBinder([ wxEVT_JOY_BUTTON_DOWN,
                                        wxEVT_JOY_BUTTON_UP, 
                                        wxEVT_JOY_MOVE, 
                                        wxEVT_JOY_ZMOVE,
                                        ])


#---------------------------------------------------------------------------

SOUND_SYNC = _misc_.SOUND_SYNC
SOUND_ASYNC = _misc_.SOUND_ASYNC
SOUND_LOOP = _misc_.SOUND_LOOP
class Sound(object):
    """Proxy of C++ Sound class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String fileName=EmptyString) -> Sound"""
        _misc_.Sound_swiginit(self,_misc_.new_Sound(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Sound
    __del__ = lambda self : None;
    def Create(*args, **kwargs):
        """Create(self, String fileName) -> bool"""
        return _misc_.Sound_Create(*args, **kwargs)

    def CreateFromData(*args, **kwargs):
        """CreateFromData(self, PyObject data) -> bool"""
        return _misc_.Sound_CreateFromData(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _misc_.Sound_IsOk(*args, **kwargs)

    def Play(*args, **kwargs):
        """Play(self, unsigned int flags=SOUND_ASYNC) -> bool"""
        return _misc_.Sound_Play(*args, **kwargs)

    def PlaySound(*args, **kwargs):
        """PlaySound(String filename, unsigned int flags=SOUND_ASYNC) -> bool"""
        return _misc_.Sound_PlaySound(*args, **kwargs)

    PlaySound = staticmethod(PlaySound)
    def Stop(*args, **kwargs):
        """Stop()"""
        return _misc_.Sound_Stop(*args, **kwargs)

    Stop = staticmethod(Stop)
    def __nonzero__(self): return self.IsOk() 
_misc_.Sound_swigregister(Sound)

def SoundFromData(*args, **kwargs):
    """SoundFromData(PyObject data) -> Sound"""
    val = _misc_.new_SoundFromData(*args, **kwargs)
    return val

def Sound_PlaySound(*args, **kwargs):
  """Sound_PlaySound(String filename, unsigned int flags=SOUND_ASYNC) -> bool"""
  return _misc_.Sound_PlaySound(*args, **kwargs)

def Sound_Stop(*args):
  """Sound_Stop()"""
  return _misc_.Sound_Stop(*args)

#---------------------------------------------------------------------------

MAILCAP_STANDARD = _misc_.MAILCAP_STANDARD
MAILCAP_NETSCAPE = _misc_.MAILCAP_NETSCAPE
MAILCAP_KDE = _misc_.MAILCAP_KDE
MAILCAP_GNOME = _misc_.MAILCAP_GNOME
MAILCAP_ALL = _misc_.MAILCAP_ALL
class FileTypeInfo(object):
    """Proxy of C++ FileTypeInfo class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String mimeType, String openCmd, String printCmd, String desc) -> FileTypeInfo"""
        _misc_.FileTypeInfo_swiginit(self,_misc_.new_FileTypeInfo(*args, **kwargs))
    def IsValid(*args, **kwargs):
        """IsValid(self) -> bool"""
        return _misc_.FileTypeInfo_IsValid(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(self, String iconFile, int iconIndex=0)"""
        return _misc_.FileTypeInfo_SetIcon(*args, **kwargs)

    def SetShortDesc(*args, **kwargs):
        """SetShortDesc(self, String shortDesc)"""
        return _misc_.FileTypeInfo_SetShortDesc(*args, **kwargs)

    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> String"""
        return _misc_.FileTypeInfo_GetMimeType(*args, **kwargs)

    def GetOpenCommand(*args, **kwargs):
        """GetOpenCommand(self) -> String"""
        return _misc_.FileTypeInfo_GetOpenCommand(*args, **kwargs)

    def GetPrintCommand(*args, **kwargs):
        """GetPrintCommand(self) -> String"""
        return _misc_.FileTypeInfo_GetPrintCommand(*args, **kwargs)

    def GetShortDesc(*args, **kwargs):
        """GetShortDesc(self) -> String"""
        return _misc_.FileTypeInfo_GetShortDesc(*args, **kwargs)

    def GetDescription(*args, **kwargs):
        """GetDescription(self) -> String"""
        return _misc_.FileTypeInfo_GetDescription(*args, **kwargs)

    def GetExtensions(*args, **kwargs):
        """GetExtensions(self) -> wxArrayString"""
        return _misc_.FileTypeInfo_GetExtensions(*args, **kwargs)

    def GetExtensionsCount(*args, **kwargs):
        """GetExtensionsCount(self) -> size_t"""
        return _misc_.FileTypeInfo_GetExtensionsCount(*args, **kwargs)

    def GetIconFile(*args, **kwargs):
        """GetIconFile(self) -> String"""
        return _misc_.FileTypeInfo_GetIconFile(*args, **kwargs)

    def GetIconIndex(*args, **kwargs):
        """GetIconIndex(self) -> int"""
        return _misc_.FileTypeInfo_GetIconIndex(*args, **kwargs)

    Description = property(GetDescription,doc="See `GetDescription`") 
    Extensions = property(GetExtensions,doc="See `GetExtensions`") 
    ExtensionsCount = property(GetExtensionsCount,doc="See `GetExtensionsCount`") 
    IconFile = property(GetIconFile,doc="See `GetIconFile`") 
    IconIndex = property(GetIconIndex,doc="See `GetIconIndex`") 
    MimeType = property(GetMimeType,doc="See `GetMimeType`") 
    OpenCommand = property(GetOpenCommand,doc="See `GetOpenCommand`") 
    PrintCommand = property(GetPrintCommand,doc="See `GetPrintCommand`") 
    ShortDesc = property(GetShortDesc,SetShortDesc,doc="See `GetShortDesc` and `SetShortDesc`") 
_misc_.FileTypeInfo_swigregister(FileTypeInfo)

def FileTypeInfoSequence(*args, **kwargs):
    """FileTypeInfoSequence(wxArrayString sArray) -> FileTypeInfo"""
    val = _misc_.new_FileTypeInfoSequence(*args, **kwargs)
    return val

def NullFileTypeInfo(*args, **kwargs):
    """NullFileTypeInfo() -> FileTypeInfo"""
    val = _misc_.new_NullFileTypeInfo(*args, **kwargs)
    return val

class FileType(object):
    """Proxy of C++ FileType class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, FileTypeInfo ftInfo) -> FileType"""
        _misc_.FileType_swiginit(self,_misc_.new_FileType(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_FileType
    __del__ = lambda self : None;
    def GetMimeType(*args, **kwargs):
        """GetMimeType(self) -> PyObject"""
        return _misc_.FileType_GetMimeType(*args, **kwargs)

    def GetMimeTypes(*args, **kwargs):
        """GetMimeTypes(self) -> PyObject"""
        return _misc_.FileType_GetMimeTypes(*args, **kwargs)

    def GetExtensions(*args, **kwargs):
        """GetExtensions(self) -> PyObject"""
        return _misc_.FileType_GetExtensions(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(self) -> Icon"""
        return _misc_.FileType_GetIcon(*args, **kwargs)

    def GetIconInfo(*args, **kwargs):
        """GetIconInfo(self) -> PyObject"""
        return _misc_.FileType_GetIconInfo(*args, **kwargs)

    def GetDescription(*args, **kwargs):
        """GetDescription(self) -> PyObject"""
        return _misc_.FileType_GetDescription(*args, **kwargs)

    def GetOpenCommand(*args, **kwargs):
        """GetOpenCommand(self, String filename, String mimetype=EmptyString) -> PyObject"""
        return _misc_.FileType_GetOpenCommand(*args, **kwargs)

    def GetPrintCommand(*args, **kwargs):
        """GetPrintCommand(self, String filename, String mimetype=EmptyString) -> PyObject"""
        return _misc_.FileType_GetPrintCommand(*args, **kwargs)

    def GetAllCommands(*args, **kwargs):
        """GetAllCommands(self, String filename, String mimetype=EmptyString) -> PyObject"""
        return _misc_.FileType_GetAllCommands(*args, **kwargs)

    def SetCommand(*args, **kwargs):
        """SetCommand(self, String cmd, String verb, bool overwriteprompt=True) -> bool"""
        return _misc_.FileType_SetCommand(*args, **kwargs)

    def SetDefaultIcon(*args, **kwargs):
        """SetDefaultIcon(self, String cmd=EmptyString, int index=0) -> bool"""
        return _misc_.FileType_SetDefaultIcon(*args, **kwargs)

    def Unassociate(*args, **kwargs):
        """Unassociate(self) -> bool"""
        return _misc_.FileType_Unassociate(*args, **kwargs)

    def ExpandCommand(*args, **kwargs):
        """ExpandCommand(String command, String filename, String mimetype=EmptyString) -> String"""
        return _misc_.FileType_ExpandCommand(*args, **kwargs)

    ExpandCommand = staticmethod(ExpandCommand)
    AllCommands = property(GetAllCommands,doc="See `GetAllCommands`") 
    Description = property(GetDescription,doc="See `GetDescription`") 
    Extensions = property(GetExtensions,doc="See `GetExtensions`") 
    Icon = property(GetIcon,doc="See `GetIcon`") 
    IconInfo = property(GetIconInfo,doc="See `GetIconInfo`") 
    MimeType = property(GetMimeType,doc="See `GetMimeType`") 
    MimeTypes = property(GetMimeTypes,doc="See `GetMimeTypes`") 
    OpenCommand = property(GetOpenCommand,doc="See `GetOpenCommand`") 
    PrintCommand = property(GetPrintCommand,doc="See `GetPrintCommand`") 
_misc_.FileType_swigregister(FileType)

def FileType_ExpandCommand(*args, **kwargs):
  """FileType_ExpandCommand(String command, String filename, String mimetype=EmptyString) -> String"""
  return _misc_.FileType_ExpandCommand(*args, **kwargs)

class MimeTypesManager(object):
    """Proxy of C++ MimeTypesManager class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def IsOfType(*args, **kwargs):
        """IsOfType(String mimeType, String wildcard) -> bool"""
        return _misc_.MimeTypesManager_IsOfType(*args, **kwargs)

    IsOfType = staticmethod(IsOfType)
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> MimeTypesManager"""
        _misc_.MimeTypesManager_swiginit(self,_misc_.new_MimeTypesManager(*args, **kwargs))
    def Initialize(*args, **kwargs):
        """Initialize(self, int mailcapStyle=MAILCAP_ALL, String extraDir=EmptyString)"""
        return _misc_.MimeTypesManager_Initialize(*args, **kwargs)

    def ClearData(*args, **kwargs):
        """ClearData(self)"""
        return _misc_.MimeTypesManager_ClearData(*args, **kwargs)

    def GetFileTypeFromExtension(*args, **kwargs):
        """GetFileTypeFromExtension(self, String ext) -> FileType"""
        return _misc_.MimeTypesManager_GetFileTypeFromExtension(*args, **kwargs)

    def GetFileTypeFromMimeType(*args, **kwargs):
        """GetFileTypeFromMimeType(self, String mimeType) -> FileType"""
        return _misc_.MimeTypesManager_GetFileTypeFromMimeType(*args, **kwargs)

    def ReadMailcap(*args, **kwargs):
        """ReadMailcap(self, String filename, bool fallback=False) -> bool"""
        return _misc_.MimeTypesManager_ReadMailcap(*args, **kwargs)

    def ReadMimeTypes(*args, **kwargs):
        """ReadMimeTypes(self, String filename) -> bool"""
        return _misc_.MimeTypesManager_ReadMimeTypes(*args, **kwargs)

    def EnumAllFileTypes(*args, **kwargs):
        """EnumAllFileTypes(self) -> PyObject"""
        return _misc_.MimeTypesManager_EnumAllFileTypes(*args, **kwargs)

    def AddFallback(*args, **kwargs):
        """AddFallback(self, FileTypeInfo ft)"""
        return _misc_.MimeTypesManager_AddFallback(*args, **kwargs)

    def Associate(*args, **kwargs):
        """Associate(self, FileTypeInfo ftInfo) -> FileType"""
        return _misc_.MimeTypesManager_Associate(*args, **kwargs)

    def Unassociate(*args, **kwargs):
        """Unassociate(self, FileType ft) -> bool"""
        return _misc_.MimeTypesManager_Unassociate(*args, **kwargs)

    __swig_destroy__ = _misc_.delete_MimeTypesManager
    __del__ = lambda self : None;
_misc_.MimeTypesManager_swigregister(MimeTypesManager)
TheMimeTypesManager = cvar.TheMimeTypesManager

def MimeTypesManager_IsOfType(*args, **kwargs):
  """MimeTypesManager_IsOfType(String mimeType, String wildcard) -> bool"""
  return _misc_.MimeTypesManager_IsOfType(*args, **kwargs)

#---------------------------------------------------------------------------

class ArtProvider(object):
    """
    The wx.ArtProvider class is used to customize the look of wxWidgets
    application. When wxWidgets needs to display an icon or a bitmap (e.g.
    in the standard file dialog), it does not use hard-coded resource but
    asks wx.ArtProvider for it instead. This way the users can plug in
    their own wx.ArtProvider class and easily replace standard art with
    his/her own version. It is easy thing to do: all that is needed is
    to derive a class from wx.ArtProvider, override it's CreateBitmap
    method and register the provider with `wx.ArtProvider.Push`::

        class MyArtProvider(wx.ArtProvider):
            def __init__(self):
                wx.ArtProvider.__init__(self)

            def CreateBitmap(self, artid, client, size):
                ...
                return bmp

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ArtProvider

        The wx.ArtProvider class is used to customize the look of wxWidgets
        application. When wxWidgets needs to display an icon or a bitmap (e.g.
        in the standard file dialog), it does not use hard-coded resource but
        asks wx.ArtProvider for it instead. This way the users can plug in
        their own wx.ArtProvider class and easily replace standard art with
        his/her own version. It is easy thing to do: all that is needed is
        to derive a class from wx.ArtProvider, override it's CreateBitmap
        method and register the provider with `wx.ArtProvider.Push`::

            class MyArtProvider(wx.ArtProvider):
                def __init__(self):
                    wx.ArtProvider.__init__(self)

                def CreateBitmap(self, artid, client, size):
                    ...
                    return bmp

        """
        _misc_.ArtProvider_swiginit(self,_misc_.new_ArtProvider(*args, **kwargs))
        self._setCallbackInfo(self, ArtProvider)

    __swig_destroy__ = _misc_.delete_ArtProvider
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.ArtProvider__setCallbackInfo(*args, **kwargs)

    def Push(*args, **kwargs):
        """
        Push(ArtProvider provider)

        Add new provider to the top of providers stack.
        """
        return _misc_.ArtProvider_Push(*args, **kwargs)

    Push = staticmethod(Push)
    PushProvider = Push 
    def Insert(*args, **kwargs):
        """
        Insert(ArtProvider provider)

        Add new provider to the bottom of providers stack.
        """
        return _misc_.ArtProvider_Insert(*args, **kwargs)

    Insert = staticmethod(Insert)
    InsertProvider = Insert 
    def Pop(*args, **kwargs):
        """
        Pop() -> bool

        Remove latest added provider and delete it.
        """
        return _misc_.ArtProvider_Pop(*args, **kwargs)

    Pop = staticmethod(Pop)
    PopProvider = Pop 
    def Delete(*args, **kwargs):
        """
        Delete(ArtProvider provider) -> bool

        Remove provider. The provider must have been added previously!  The
        provider is _not_ deleted.
        """
        val = _misc_.ArtProvider_Delete(*args, **kwargs)
        args[1].thisown = 1
        return val

    Delete = staticmethod(Delete)
    RemoveProvider = Delete 
    def GetBitmap(*args, **kwargs):
        """
        GetBitmap(String id, String client=ART_OTHER, Size size=DefaultSize) -> Bitmap

        Query the providers for bitmap with given ID and return it. Return
        wx.NullBitmap if no provider provides it.
        """
        return _misc_.ArtProvider_GetBitmap(*args, **kwargs)

    GetBitmap = staticmethod(GetBitmap)
    def GetIcon(*args, **kwargs):
        """
        GetIcon(String id, String client=ART_OTHER, Size size=DefaultSize) -> Icon

        Query the providers for icon with given ID and return it.  Return
        wx.NullIcon if no provider provides it.
        """
        return _misc_.ArtProvider_GetIcon(*args, **kwargs)

    GetIcon = staticmethod(GetIcon)
    def GetSizeHint(*args, **kwargs):
        """
        GetSizeHint(String client, bool platform_dependent=False) -> Size

        Get the size hint of an icon from a specific Art Client, queries the
        topmost provider if platform_dependent = false
        """
        return _misc_.ArtProvider_GetSizeHint(*args, **kwargs)

    GetSizeHint = staticmethod(GetSizeHint)
    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        val = _misc_.ArtProvider_Destroy(*args, **kwargs)
        args[0].thisown = 0
        return val

_misc_.ArtProvider_swigregister(ArtProvider)
ART_TOOLBAR = cvar.ART_TOOLBAR
ART_MENU = cvar.ART_MENU
ART_FRAME_ICON = cvar.ART_FRAME_ICON
ART_CMN_DIALOG = cvar.ART_CMN_DIALOG
ART_HELP_BROWSER = cvar.ART_HELP_BROWSER
ART_MESSAGE_BOX = cvar.ART_MESSAGE_BOX
ART_BUTTON = cvar.ART_BUTTON
ART_OTHER = cvar.ART_OTHER
ART_ADD_BOOKMARK = cvar.ART_ADD_BOOKMARK
ART_DEL_BOOKMARK = cvar.ART_DEL_BOOKMARK
ART_HELP_SIDE_PANEL = cvar.ART_HELP_SIDE_PANEL
ART_HELP_SETTINGS = cvar.ART_HELP_SETTINGS
ART_HELP_BOOK = cvar.ART_HELP_BOOK
ART_HELP_FOLDER = cvar.ART_HELP_FOLDER
ART_HELP_PAGE = cvar.ART_HELP_PAGE
ART_GO_BACK = cvar.ART_GO_BACK
ART_GO_FORWARD = cvar.ART_GO_FORWARD
ART_GO_UP = cvar.ART_GO_UP
ART_GO_DOWN = cvar.ART_GO_DOWN
ART_GO_TO_PARENT = cvar.ART_GO_TO_PARENT
ART_GO_HOME = cvar.ART_GO_HOME
ART_FILE_OPEN = cvar.ART_FILE_OPEN
ART_FILE_SAVE = cvar.ART_FILE_SAVE
ART_FILE_SAVE_AS = cvar.ART_FILE_SAVE_AS
ART_PRINT = cvar.ART_PRINT
ART_HELP = cvar.ART_HELP
ART_TIP = cvar.ART_TIP
ART_REPORT_VIEW = cvar.ART_REPORT_VIEW
ART_LIST_VIEW = cvar.ART_LIST_VIEW
ART_NEW_DIR = cvar.ART_NEW_DIR
ART_HARDDISK = cvar.ART_HARDDISK
ART_FLOPPY = cvar.ART_FLOPPY
ART_CDROM = cvar.ART_CDROM
ART_REMOVABLE = cvar.ART_REMOVABLE
ART_FOLDER = cvar.ART_FOLDER
ART_FOLDER_OPEN = cvar.ART_FOLDER_OPEN
ART_GO_DIR_UP = cvar.ART_GO_DIR_UP
ART_EXECUTABLE_FILE = cvar.ART_EXECUTABLE_FILE
ART_NORMAL_FILE = cvar.ART_NORMAL_FILE
ART_TICK_MARK = cvar.ART_TICK_MARK
ART_CROSS_MARK = cvar.ART_CROSS_MARK
ART_ERROR = cvar.ART_ERROR
ART_QUESTION = cvar.ART_QUESTION
ART_WARNING = cvar.ART_WARNING
ART_INFORMATION = cvar.ART_INFORMATION
ART_MISSING_IMAGE = cvar.ART_MISSING_IMAGE
ART_COPY = cvar.ART_COPY
ART_CUT = cvar.ART_CUT
ART_PASTE = cvar.ART_PASTE
ART_DELETE = cvar.ART_DELETE
ART_NEW = cvar.ART_NEW
ART_UNDO = cvar.ART_UNDO
ART_REDO = cvar.ART_REDO
ART_QUIT = cvar.ART_QUIT
ART_FIND = cvar.ART_FIND
ART_FIND_AND_REPLACE = cvar.ART_FIND_AND_REPLACE

def ArtProvider_Push(*args, **kwargs):
  """
    ArtProvider_Push(ArtProvider provider)

    Add new provider to the top of providers stack.
    """
  return _misc_.ArtProvider_Push(*args, **kwargs)

def ArtProvider_Insert(*args, **kwargs):
  """
    ArtProvider_Insert(ArtProvider provider)

    Add new provider to the bottom of providers stack.
    """
  return _misc_.ArtProvider_Insert(*args, **kwargs)

def ArtProvider_Pop(*args):
  """
    ArtProvider_Pop() -> bool

    Remove latest added provider and delete it.
    """
  return _misc_.ArtProvider_Pop(*args)

def ArtProvider_Delete(*args, **kwargs):
  """
    ArtProvider_Delete(ArtProvider provider) -> bool

    Remove provider. The provider must have been added previously!  The
    provider is _not_ deleted.
    """
  val = _misc_.ArtProvider_Delete(*args, **kwargs)
  args[1].thisown = 1
  return val

def ArtProvider_GetBitmap(*args, **kwargs):
  """
    ArtProvider_GetBitmap(String id, String client=ART_OTHER, Size size=DefaultSize) -> Bitmap

    Query the providers for bitmap with given ID and return it. Return
    wx.NullBitmap if no provider provides it.
    """
  return _misc_.ArtProvider_GetBitmap(*args, **kwargs)

def ArtProvider_GetIcon(*args, **kwargs):
  """
    ArtProvider_GetIcon(String id, String client=ART_OTHER, Size size=DefaultSize) -> Icon

    Query the providers for icon with given ID and return it.  Return
    wx.NullIcon if no provider provides it.
    """
  return _misc_.ArtProvider_GetIcon(*args, **kwargs)

def ArtProvider_GetSizeHint(*args, **kwargs):
  """
    ArtProvider_GetSizeHint(String client, bool platform_dependent=False) -> Size

    Get the size hint of an icon from a specific Art Client, queries the
    topmost provider if platform_dependent = false
    """
  return _misc_.ArtProvider_GetSizeHint(*args, **kwargs)

#---------------------------------------------------------------------------

CONFIG_USE_LOCAL_FILE = _misc_.CONFIG_USE_LOCAL_FILE
CONFIG_USE_GLOBAL_FILE = _misc_.CONFIG_USE_GLOBAL_FILE
CONFIG_USE_RELATIVE_PATH = _misc_.CONFIG_USE_RELATIVE_PATH
CONFIG_USE_NO_ESCAPE_CHARACTERS = _misc_.CONFIG_USE_NO_ESCAPE_CHARACTERS
class ConfigBase(object):
    """
    wx.ConfigBase class defines the basic interface of all config
    classes. It can not be used by itself (it is an abstract base class)
    and you will always use one of its derivations: wx.Config or
    wx.FileConfig.

    wx.ConfigBase organizes the items in a tree-like structure, modeled
    after the Unix/Dos filesystem. There are groups that act like
    directories and entries, key/value pairs that act like files.  There
    is always one current group given by the current path.  As in the file
    system case, to specify a key in the config class you must use a path
    to it.  Config classes also support the notion of the current group,
    which makes it possible to use relative paths.

    Keys are pairs "key_name = value" where value may be of string,
    integer floating point or boolean, you can not store binary data
    without first encoding it as a string.  For performance reasons items
    should be kept small, no more than a couple kilobytes.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _misc_.delete_ConfigBase
    __del__ = lambda self : None;
    Type_Unknown = _misc_.ConfigBase_Type_Unknown
    Type_String = _misc_.ConfigBase_Type_String
    Type_Boolean = _misc_.ConfigBase_Type_Boolean
    Type_Integer = _misc_.ConfigBase_Type_Integer
    Type_Float = _misc_.ConfigBase_Type_Float
    def Set(*args, **kwargs):
        """
        Set(ConfigBase config) -> ConfigBase

        Sets the global config object (the one returned by Get) and returns a
        reference to the previous global config object.
        """
        return _misc_.ConfigBase_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def Get(*args, **kwargs):
        """
        Get(bool createOnDemand=True) -> ConfigBase

        Returns the current global config object, creating one if neccessary.
        """
        return _misc_.ConfigBase_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def Create(*args, **kwargs):
        """
        Create() -> ConfigBase

        Create and return a new global config object.  This function will
        create the "best" implementation of wx.Config available for the
        current platform.
        """
        return _misc_.ConfigBase_Create(*args, **kwargs)

    Create = staticmethod(Create)
    def DontCreateOnDemand(*args, **kwargs):
        """
        DontCreateOnDemand()

        Should Get() try to create a new log object if there isn't a current
        one?
        """
        return _misc_.ConfigBase_DontCreateOnDemand(*args, **kwargs)

    DontCreateOnDemand = staticmethod(DontCreateOnDemand)
    def SetPath(*args, **kwargs):
        """
        SetPath(self, String path)

        Set current path: if the first character is '/', it's the absolute
        path, otherwise it's a relative path. '..' is supported. If the
        strPath doesn't exist it is created.
        """
        return _misc_.ConfigBase_SetPath(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """
        GetPath(self) -> String

        Retrieve the current path (always as absolute path)
        """
        return _misc_.ConfigBase_GetPath(*args, **kwargs)

    def GetFirstGroup(*args, **kwargs):
        """
        GetFirstGroup() -> (more, value, index)

        Allows enumerating the subgroups in a config object.  Returns a tuple
        containing a flag indicating there are more items, the name of the
        current item, and an index to pass to GetNextGroup to fetch the next
        item.
        """
        return _misc_.ConfigBase_GetFirstGroup(*args, **kwargs)

    def GetNextGroup(*args, **kwargs):
        """
        GetNextGroup(long index) -> (more, value, index)

        Allows enumerating the subgroups in a config object.  Returns a tuple
        containing a flag indicating there are more items, the name of the
        current item, and an index to pass to GetNextGroup to fetch the next
        item.
        """
        return _misc_.ConfigBase_GetNextGroup(*args, **kwargs)

    def GetFirstEntry(*args, **kwargs):
        """
        GetFirstEntry() -> (more, value, index)

        Allows enumerating the entries in the current group in a config
        object.  Returns a tuple containing a flag indicating there are more
        items, the name of the current item, and an index to pass to
        GetNextGroup to fetch the next item.
        """
        return _misc_.ConfigBase_GetFirstEntry(*args, **kwargs)

    def GetNextEntry(*args, **kwargs):
        """
        GetNextEntry(long index) -> (more, value, index)

        Allows enumerating the entries in the current group in a config
        object.  Returns a tuple containing a flag indicating there are more
        items, the name of the current item, and an index to pass to
        GetNextGroup to fetch the next item.
        """
        return _misc_.ConfigBase_GetNextEntry(*args, **kwargs)

    def GetNumberOfEntries(*args, **kwargs):
        """
        GetNumberOfEntries(self, bool recursive=False) -> size_t

        Get the number of entries in the current group, with or without its
        subgroups.
        """
        return _misc_.ConfigBase_GetNumberOfEntries(*args, **kwargs)

    def GetNumberOfGroups(*args, **kwargs):
        """
        GetNumberOfGroups(self, bool recursive=False) -> size_t

        Get the number of subgroups in the current group, with or without its
        subgroups.
        """
        return _misc_.ConfigBase_GetNumberOfGroups(*args, **kwargs)

    def HasGroup(*args, **kwargs):
        """
        HasGroup(self, String name) -> bool

        Returns True if the group by this name exists
        """
        return _misc_.ConfigBase_HasGroup(*args, **kwargs)

    def HasEntry(*args, **kwargs):
        """
        HasEntry(self, String name) -> bool

        Returns True if the entry by this name exists
        """
        return _misc_.ConfigBase_HasEntry(*args, **kwargs)

    def Exists(*args, **kwargs):
        """
        Exists(self, String name) -> bool

        Returns True if either a group or an entry with a given name exists
        """
        return _misc_.ConfigBase_Exists(*args, **kwargs)

    def GetEntryType(*args, **kwargs):
        """
        GetEntryType(self, String name) -> int

        Get the type of the entry.  Returns one of the wx.Config.Type_XXX values.
        """
        return _misc_.ConfigBase_GetEntryType(*args, **kwargs)

    def Read(*args, **kwargs):
        """
        Read(self, String key, String defaultVal=EmptyString) -> String

        Returns the value of key if it exists, defaultVal otherwise.
        """
        return _misc_.ConfigBase_Read(*args, **kwargs)

    def ReadInt(*args, **kwargs):
        """
        ReadInt(self, String key, long defaultVal=0) -> long

        Returns the value of key if it exists, defaultVal otherwise.
        """
        return _misc_.ConfigBase_ReadInt(*args, **kwargs)

    def ReadFloat(*args, **kwargs):
        """
        ReadFloat(self, String key, double defaultVal=0.0) -> double

        Returns the value of key if it exists, defaultVal otherwise.
        """
        return _misc_.ConfigBase_ReadFloat(*args, **kwargs)

    def ReadBool(*args, **kwargs):
        """
        ReadBool(self, String key, bool defaultVal=False) -> bool

        Returns the value of key if it exists, defaultVal otherwise.
        """
        return _misc_.ConfigBase_ReadBool(*args, **kwargs)

    def Write(*args, **kwargs):
        """
        Write(self, String key, String value) -> bool

        write the value (return True on success)
        """
        return _misc_.ConfigBase_Write(*args, **kwargs)

    def WriteInt(*args, **kwargs):
        """
        WriteInt(self, String key, long value) -> bool

        write the value (return True on success)
        """
        return _misc_.ConfigBase_WriteInt(*args, **kwargs)

    def WriteFloat(*args, **kwargs):
        """
        WriteFloat(self, String key, double value) -> bool

        write the value (return True on success)
        """
        return _misc_.ConfigBase_WriteFloat(*args, **kwargs)

    def WriteBool(*args, **kwargs):
        """
        WriteBool(self, String key, bool value) -> bool

        write the value (return True on success)
        """
        return _misc_.ConfigBase_WriteBool(*args, **kwargs)

    def Flush(*args, **kwargs):
        """
        Flush(self, bool currentOnly=False) -> bool

        permanently writes all changes
        """
        return _misc_.ConfigBase_Flush(*args, **kwargs)

    def RenameEntry(*args, **kwargs):
        """
        RenameEntry(self, String oldName, String newName) -> bool

        Rename an entry.  Returns False on failure (probably because the new
        name is already taken by an existing entry)
        """
        return _misc_.ConfigBase_RenameEntry(*args, **kwargs)

    def RenameGroup(*args, **kwargs):
        """
        RenameGroup(self, String oldName, String newName) -> bool

        Rename a group.  Returns False on failure (probably because the new
        name is already taken by an existing entry)
        """
        return _misc_.ConfigBase_RenameGroup(*args, **kwargs)

    def DeleteEntry(*args, **kwargs):
        """
        DeleteEntry(self, String key, bool deleteGroupIfEmpty=True) -> bool

        Deletes the specified entry and the group it belongs to if it was the
        last key in it and the second parameter is True
        """
        return _misc_.ConfigBase_DeleteEntry(*args, **kwargs)

    def DeleteGroup(*args, **kwargs):
        """
        DeleteGroup(self, String key) -> bool

        Delete the group (with all subgroups)
        """
        return _misc_.ConfigBase_DeleteGroup(*args, **kwargs)

    def DeleteAll(*args, **kwargs):
        """
        DeleteAll(self) -> bool

        Delete the whole underlying object (disk file, registry key, ...)
        primarly intended for use by deinstallation routine.
        """
        return _misc_.ConfigBase_DeleteAll(*args, **kwargs)

    def SetExpandEnvVars(*args, **kwargs):
        """
        SetExpandEnvVars(self, bool doIt=True)

        We can automatically expand environment variables in the config
        entries this option is on by default, you can turn it on/off at any
        time)
        """
        return _misc_.ConfigBase_SetExpandEnvVars(*args, **kwargs)

    def IsExpandingEnvVars(*args, **kwargs):
        """
        IsExpandingEnvVars(self) -> bool

        Are we currently expanding environment variables?
        """
        return _misc_.ConfigBase_IsExpandingEnvVars(*args, **kwargs)

    def SetRecordDefaults(*args, **kwargs):
        """
        SetRecordDefaults(self, bool doIt=True)

        Set whether the config objec should record default values.
        """
        return _misc_.ConfigBase_SetRecordDefaults(*args, **kwargs)

    def IsRecordingDefaults(*args, **kwargs):
        """
        IsRecordingDefaults(self) -> bool

        Are we currently recording default values?
        """
        return _misc_.ConfigBase_IsRecordingDefaults(*args, **kwargs)

    def ExpandEnvVars(*args, **kwargs):
        """
        ExpandEnvVars(self, String str) -> String

        Expand any environment variables in str and return the result
        """
        return _misc_.ConfigBase_ExpandEnvVars(*args, **kwargs)

    def GetAppName(*args, **kwargs):
        """GetAppName(self) -> String"""
        return _misc_.ConfigBase_GetAppName(*args, **kwargs)

    def GetVendorName(*args, **kwargs):
        """GetVendorName(self) -> String"""
        return _misc_.ConfigBase_GetVendorName(*args, **kwargs)

    def SetAppName(*args, **kwargs):
        """SetAppName(self, String appName)"""
        return _misc_.ConfigBase_SetAppName(*args, **kwargs)

    def SetVendorName(*args, **kwargs):
        """SetVendorName(self, String vendorName)"""
        return _misc_.ConfigBase_SetVendorName(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, long style)"""
        return _misc_.ConfigBase_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> long"""
        return _misc_.ConfigBase_GetStyle(*args, **kwargs)

    AppName = property(GetAppName,SetAppName,doc="See `GetAppName` and `SetAppName`") 
    EntryType = property(GetEntryType,doc="See `GetEntryType`") 
    FirstEntry = property(GetFirstEntry,doc="See `GetFirstEntry`") 
    FirstGroup = property(GetFirstGroup,doc="See `GetFirstGroup`") 
    NextEntry = property(GetNextEntry,doc="See `GetNextEntry`") 
    NextGroup = property(GetNextGroup,doc="See `GetNextGroup`") 
    NumberOfEntries = property(GetNumberOfEntries,doc="See `GetNumberOfEntries`") 
    NumberOfGroups = property(GetNumberOfGroups,doc="See `GetNumberOfGroups`") 
    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
    Style = property(GetStyle,SetStyle,doc="See `GetStyle` and `SetStyle`") 
    VendorName = property(GetVendorName,SetVendorName,doc="See `GetVendorName` and `SetVendorName`") 
_misc_.ConfigBase_swigregister(ConfigBase)

def ConfigBase_Set(*args, **kwargs):
  """
    ConfigBase_Set(ConfigBase config) -> ConfigBase

    Sets the global config object (the one returned by Get) and returns a
    reference to the previous global config object.
    """
  return _misc_.ConfigBase_Set(*args, **kwargs)

def ConfigBase_Get(*args, **kwargs):
  """
    ConfigBase_Get(bool createOnDemand=True) -> ConfigBase

    Returns the current global config object, creating one if neccessary.
    """
  return _misc_.ConfigBase_Get(*args, **kwargs)

def ConfigBase_Create(*args):
  """
    ConfigBase_Create() -> ConfigBase

    Create and return a new global config object.  This function will
    create the "best" implementation of wx.Config available for the
    current platform.
    """
  return _misc_.ConfigBase_Create(*args)

def ConfigBase_DontCreateOnDemand(*args):
  """
    ConfigBase_DontCreateOnDemand()

    Should Get() try to create a new log object if there isn't a current
    one?
    """
  return _misc_.ConfigBase_DontCreateOnDemand(*args)

class Config(ConfigBase):
    """
    This ConfigBase-derived class will use the registry on Windows,
    and will be a wx.FileConfig on other platforms.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String appName=EmptyString, String vendorName=EmptyString, 
            String localFilename=EmptyString, String globalFilename=EmptyString, 
            long style=wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE) -> Config
        """
        _misc_.Config_swiginit(self,_misc_.new_Config(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Config
    __del__ = lambda self : None;
_misc_.Config_swigregister(Config)

class FileConfig(ConfigBase):
    """This config class will use a file for storage on all platforms."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String appName=EmptyString, String vendorName=EmptyString, 
            String localFilename=EmptyString, String globalFilename=EmptyString, 
            long style=wxCONFIG_USE_LOCAL_FILE|wxCONFIG_USE_GLOBAL_FILE) -> FileConfig
        """
        _misc_.FileConfig_swiginit(self,_misc_.new_FileConfig(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_FileConfig
    __del__ = lambda self : None;
_misc_.FileConfig_swigregister(FileConfig)

class ConfigPathChanger(object):
    """
    A handy little class which changes current path to the path of given
    entry and restores it in the destructoir: so if you declare a local
    variable of this type, you work in the entry directory and the path is
    automatically restored when the function returns.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, ConfigBase config, String entry) -> ConfigPathChanger"""
        _misc_.ConfigPathChanger_swiginit(self,_misc_.new_ConfigPathChanger(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_ConfigPathChanger
    __del__ = lambda self : None;
    def Name(*args, **kwargs):
        """
        Name(self) -> String

        Get the key name
        """
        return _misc_.ConfigPathChanger_Name(*args, **kwargs)

_misc_.ConfigPathChanger_swigregister(ConfigPathChanger)


def ExpandEnvVars(*args, **kwargs):
  """
    ExpandEnvVars(String sz) -> String

    Replace environment variables ($SOMETHING) with their values. The
    format is $VARNAME or ${VARNAME} where VARNAME contains alphanumeric
    characters and '_' only. '$' must be escaped ('\$') in order to be
    taken literally.
    """
  return _misc_.ExpandEnvVars(*args, **kwargs)
#---------------------------------------------------------------------------

class DateTime(object):
    """Proxy of C++ DateTime class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    Local = _misc_.DateTime_Local
    GMT_12 = _misc_.DateTime_GMT_12
    GMT_11 = _misc_.DateTime_GMT_11
    GMT_10 = _misc_.DateTime_GMT_10
    GMT_9 = _misc_.DateTime_GMT_9
    GMT_8 = _misc_.DateTime_GMT_8
    GMT_7 = _misc_.DateTime_GMT_7
    GMT_6 = _misc_.DateTime_GMT_6
    GMT_5 = _misc_.DateTime_GMT_5
    GMT_4 = _misc_.DateTime_GMT_4
    GMT_3 = _misc_.DateTime_GMT_3
    GMT_2 = _misc_.DateTime_GMT_2
    GMT_1 = _misc_.DateTime_GMT_1
    GMT0 = _misc_.DateTime_GMT0
    GMT1 = _misc_.DateTime_GMT1
    GMT2 = _misc_.DateTime_GMT2
    GMT3 = _misc_.DateTime_GMT3
    GMT4 = _misc_.DateTime_GMT4
    GMT5 = _misc_.DateTime_GMT5
    GMT6 = _misc_.DateTime_GMT6
    GMT7 = _misc_.DateTime_GMT7
    GMT8 = _misc_.DateTime_GMT8
    GMT9 = _misc_.DateTime_GMT9
    GMT10 = _misc_.DateTime_GMT10
    GMT11 = _misc_.DateTime_GMT11
    GMT12 = _misc_.DateTime_GMT12
    WET = _misc_.DateTime_WET
    WEST = _misc_.DateTime_WEST
    CET = _misc_.DateTime_CET
    CEST = _misc_.DateTime_CEST
    EET = _misc_.DateTime_EET
    EEST = _misc_.DateTime_EEST
    MSK = _misc_.DateTime_MSK
    MSD = _misc_.DateTime_MSD
    AST = _misc_.DateTime_AST
    ADT = _misc_.DateTime_ADT
    EST = _misc_.DateTime_EST
    EDT = _misc_.DateTime_EDT
    CST = _misc_.DateTime_CST
    CDT = _misc_.DateTime_CDT
    MST = _misc_.DateTime_MST
    MDT = _misc_.DateTime_MDT
    PST = _misc_.DateTime_PST
    PDT = _misc_.DateTime_PDT
    HST = _misc_.DateTime_HST
    AKST = _misc_.DateTime_AKST
    AKDT = _misc_.DateTime_AKDT
    A_WST = _misc_.DateTime_A_WST
    A_CST = _misc_.DateTime_A_CST
    A_EST = _misc_.DateTime_A_EST
    A_ESST = _misc_.DateTime_A_ESST
    UTC = _misc_.DateTime_UTC
    Gregorian = _misc_.DateTime_Gregorian
    Julian = _misc_.DateTime_Julian
    Gr_Unknown = _misc_.DateTime_Gr_Unknown
    Gr_Standard = _misc_.DateTime_Gr_Standard
    Gr_Alaska = _misc_.DateTime_Gr_Alaska
    Gr_Albania = _misc_.DateTime_Gr_Albania
    Gr_Austria = _misc_.DateTime_Gr_Austria
    Gr_Austria_Brixen = _misc_.DateTime_Gr_Austria_Brixen
    Gr_Austria_Salzburg = _misc_.DateTime_Gr_Austria_Salzburg
    Gr_Austria_Tyrol = _misc_.DateTime_Gr_Austria_Tyrol
    Gr_Austria_Carinthia = _misc_.DateTime_Gr_Austria_Carinthia
    Gr_Austria_Styria = _misc_.DateTime_Gr_Austria_Styria
    Gr_Belgium = _misc_.DateTime_Gr_Belgium
    Gr_Bulgaria = _misc_.DateTime_Gr_Bulgaria
    Gr_Bulgaria_1 = _misc_.DateTime_Gr_Bulgaria_1
    Gr_Bulgaria_2 = _misc_.DateTime_Gr_Bulgaria_2
    Gr_Bulgaria_3 = _misc_.DateTime_Gr_Bulgaria_3
    Gr_Canada = _misc_.DateTime_Gr_Canada
    Gr_China = _misc_.DateTime_Gr_China
    Gr_China_1 = _misc_.DateTime_Gr_China_1
    Gr_China_2 = _misc_.DateTime_Gr_China_2
    Gr_Czechoslovakia = _misc_.DateTime_Gr_Czechoslovakia
    Gr_Denmark = _misc_.DateTime_Gr_Denmark
    Gr_Egypt = _misc_.DateTime_Gr_Egypt
    Gr_Estonia = _misc_.DateTime_Gr_Estonia
    Gr_Finland = _misc_.DateTime_Gr_Finland
    Gr_France = _misc_.DateTime_Gr_France
    Gr_France_Alsace = _misc_.DateTime_Gr_France_Alsace
    Gr_France_Lorraine = _misc_.DateTime_Gr_France_Lorraine
    Gr_France_Strasbourg = _misc_.DateTime_Gr_France_Strasbourg
    Gr_Germany = _misc_.DateTime_Gr_Germany
    Gr_Germany_Catholic = _misc_.DateTime_Gr_Germany_Catholic
    Gr_Germany_Prussia = _misc_.DateTime_Gr_Germany_Prussia
    Gr_Germany_Protestant = _misc_.DateTime_Gr_Germany_Protestant
    Gr_GreatBritain = _misc_.DateTime_Gr_GreatBritain
    Gr_Greece = _misc_.DateTime_Gr_Greece
    Gr_Hungary = _misc_.DateTime_Gr_Hungary
    Gr_Ireland = _misc_.DateTime_Gr_Ireland
    Gr_Italy = _misc_.DateTime_Gr_Italy
    Gr_Japan = _misc_.DateTime_Gr_Japan
    Gr_Japan_1 = _misc_.DateTime_Gr_Japan_1
    Gr_Japan_2 = _misc_.DateTime_Gr_Japan_2
    Gr_Japan_3 = _misc_.DateTime_Gr_Japan_3
    Gr_Latvia = _misc_.DateTime_Gr_Latvia
    Gr_Lithuania = _misc_.DateTime_Gr_Lithuania
    Gr_Luxemburg = _misc_.DateTime_Gr_Luxemburg
    Gr_Netherlands = _misc_.DateTime_Gr_Netherlands
    Gr_Netherlands_Groningen = _misc_.DateTime_Gr_Netherlands_Groningen
    Gr_Netherlands_Gelderland = _misc_.DateTime_Gr_Netherlands_Gelderland
    Gr_Netherlands_Utrecht = _misc_.DateTime_Gr_Netherlands_Utrecht
    Gr_Netherlands_Friesland = _misc_.DateTime_Gr_Netherlands_Friesland
    Gr_Norway = _misc_.DateTime_Gr_Norway
    Gr_Poland = _misc_.DateTime_Gr_Poland
    Gr_Portugal = _misc_.DateTime_Gr_Portugal
    Gr_Romania = _misc_.DateTime_Gr_Romania
    Gr_Russia = _misc_.DateTime_Gr_Russia
    Gr_Scotland = _misc_.DateTime_Gr_Scotland
    Gr_Spain = _misc_.DateTime_Gr_Spain
    Gr_Sweden = _misc_.DateTime_Gr_Sweden
    Gr_Switzerland = _misc_.DateTime_Gr_Switzerland
    Gr_Switzerland_Catholic = _misc_.DateTime_Gr_Switzerland_Catholic
    Gr_Switzerland_Protestant = _misc_.DateTime_Gr_Switzerland_Protestant
    Gr_Turkey = _misc_.DateTime_Gr_Turkey
    Gr_USA = _misc_.DateTime_Gr_USA
    Gr_Wales = _misc_.DateTime_Gr_Wales
    Gr_Yugoslavia = _misc_.DateTime_Gr_Yugoslavia
    Country_Unknown = _misc_.DateTime_Country_Unknown
    Country_Default = _misc_.DateTime_Country_Default
    Country_WesternEurope_Start = _misc_.DateTime_Country_WesternEurope_Start
    Country_EEC = _misc_.DateTime_Country_EEC
    France = _misc_.DateTime_France
    Germany = _misc_.DateTime_Germany
    UK = _misc_.DateTime_UK
    Country_WesternEurope_End = _misc_.DateTime_Country_WesternEurope_End
    Russia = _misc_.DateTime_Russia
    USA = _misc_.DateTime_USA
    Jan = _misc_.DateTime_Jan
    Feb = _misc_.DateTime_Feb
    Mar = _misc_.DateTime_Mar
    Apr = _misc_.DateTime_Apr
    May = _misc_.DateTime_May
    Jun = _misc_.DateTime_Jun
    Jul = _misc_.DateTime_Jul
    Aug = _misc_.DateTime_Aug
    Sep = _misc_.DateTime_Sep
    Oct = _misc_.DateTime_Oct
    Nov = _misc_.DateTime_Nov
    Dec = _misc_.DateTime_Dec
    Inv_Month = _misc_.DateTime_Inv_Month
    Sun = _misc_.DateTime_Sun
    Mon = _misc_.DateTime_Mon
    Tue = _misc_.DateTime_Tue
    Wed = _misc_.DateTime_Wed
    Thu = _misc_.DateTime_Thu
    Fri = _misc_.DateTime_Fri
    Sat = _misc_.DateTime_Sat
    Inv_WeekDay = _misc_.DateTime_Inv_WeekDay
    Inv_Year = _misc_.DateTime_Inv_Year
    Name_Full = _misc_.DateTime_Name_Full
    Name_Abbr = _misc_.DateTime_Name_Abbr
    Default_First = _misc_.DateTime_Default_First
    Monday_First = _misc_.DateTime_Monday_First
    Sunday_First = _misc_.DateTime_Sunday_First
    def SetCountry(*args, **kwargs):
        """SetCountry(int country)"""
        return _misc_.DateTime_SetCountry(*args, **kwargs)

    SetCountry = staticmethod(SetCountry)
    def GetCountry(*args, **kwargs):
        """GetCountry() -> int"""
        return _misc_.DateTime_GetCountry(*args, **kwargs)

    GetCountry = staticmethod(GetCountry)
    def IsWestEuropeanCountry(*args, **kwargs):
        """IsWestEuropeanCountry(int country=Country_Default) -> bool"""
        return _misc_.DateTime_IsWestEuropeanCountry(*args, **kwargs)

    IsWestEuropeanCountry = staticmethod(IsWestEuropeanCountry)
    def GetCurrentYear(*args, **kwargs):
        """GetCurrentYear(int cal=Gregorian) -> int"""
        return _misc_.DateTime_GetCurrentYear(*args, **kwargs)

    GetCurrentYear = staticmethod(GetCurrentYear)
    def ConvertYearToBC(*args, **kwargs):
        """ConvertYearToBC(int year) -> int"""
        return _misc_.DateTime_ConvertYearToBC(*args, **kwargs)

    ConvertYearToBC = staticmethod(ConvertYearToBC)
    def GetCurrentMonth(*args, **kwargs):
        """GetCurrentMonth(int cal=Gregorian) -> int"""
        return _misc_.DateTime_GetCurrentMonth(*args, **kwargs)

    GetCurrentMonth = staticmethod(GetCurrentMonth)
    def IsLeapYear(*args, **kwargs):
        """IsLeapYear(int year=Inv_Year, int cal=Gregorian) -> bool"""
        return _misc_.DateTime_IsLeapYear(*args, **kwargs)

    IsLeapYear = staticmethod(IsLeapYear)
    def GetCentury(*args, **kwargs):
        """GetCentury(int year=Inv_Year) -> int"""
        return _misc_.DateTime_GetCentury(*args, **kwargs)

    GetCentury = staticmethod(GetCentury)
    def GetNumberOfDaysinYear(*args, **kwargs):
        """GetNumberOfDaysinYear(int year, int cal=Gregorian) -> int"""
        return _misc_.DateTime_GetNumberOfDaysinYear(*args, **kwargs)

    GetNumberOfDaysinYear = staticmethod(GetNumberOfDaysinYear)
    def GetNumberOfDaysInMonth(*args, **kwargs):
        """GetNumberOfDaysInMonth(int month, int year=Inv_Year, int cal=Gregorian) -> int"""
        return _misc_.DateTime_GetNumberOfDaysInMonth(*args, **kwargs)

    GetNumberOfDaysInMonth = staticmethod(GetNumberOfDaysInMonth)
    def GetMonthName(*args, **kwargs):
        """GetMonthName(int month, int flags=Name_Full) -> String"""
        return _misc_.DateTime_GetMonthName(*args, **kwargs)

    GetMonthName = staticmethod(GetMonthName)
    def GetWeekDayName(*args, **kwargs):
        """GetWeekDayName(int weekday, int flags=Name_Full) -> String"""
        return _misc_.DateTime_GetWeekDayName(*args, **kwargs)

    GetWeekDayName = staticmethod(GetWeekDayName)
    def GetAmPmStrings(*args, **kwargs):
        """
        GetAmPmStrings() -> (am, pm)

        Get the AM and PM strings in the current locale (may be empty)
        """
        return _misc_.DateTime_GetAmPmStrings(*args, **kwargs)

    GetAmPmStrings = staticmethod(GetAmPmStrings)
    def IsDSTApplicable(*args, **kwargs):
        """IsDSTApplicable(int year=Inv_Year, int country=Country_Default) -> bool"""
        return _misc_.DateTime_IsDSTApplicable(*args, **kwargs)

    IsDSTApplicable = staticmethod(IsDSTApplicable)
    def GetBeginDST(*args, **kwargs):
        """GetBeginDST(int year=Inv_Year, int country=Country_Default) -> DateTime"""
        return _misc_.DateTime_GetBeginDST(*args, **kwargs)

    GetBeginDST = staticmethod(GetBeginDST)
    def GetEndDST(*args, **kwargs):
        """GetEndDST(int year=Inv_Year, int country=Country_Default) -> DateTime"""
        return _misc_.DateTime_GetEndDST(*args, **kwargs)

    GetEndDST = staticmethod(GetEndDST)
    def Now(*args, **kwargs):
        """Now() -> DateTime"""
        return _misc_.DateTime_Now(*args, **kwargs)

    Now = staticmethod(Now)
    def UNow(*args, **kwargs):
        """UNow() -> DateTime"""
        return _misc_.DateTime_UNow(*args, **kwargs)

    UNow = staticmethod(UNow)
    def Today(*args, **kwargs):
        """Today() -> DateTime"""
        return _misc_.DateTime_Today(*args, **kwargs)

    Today = staticmethod(Today)
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> DateTime"""
        _misc_.DateTime_swiginit(self,_misc_.new_DateTime(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_DateTime
    __del__ = lambda self : None;
    def SetToCurrent(*args, **kwargs):
        """SetToCurrent(self) -> DateTime"""
        return _misc_.DateTime_SetToCurrent(*args, **kwargs)

    def SetTimeT(*args, **kwargs):
        """SetTimeT(self, time_t timet) -> DateTime"""
        return _misc_.DateTime_SetTimeT(*args, **kwargs)

    def SetJDN(*args, **kwargs):
        """SetJDN(self, double jdn) -> DateTime"""
        return _misc_.DateTime_SetJDN(*args, **kwargs)

    def SetHMS(*args, **kwargs):
        """SetHMS(self, int hour, int minute=0, int second=0, int millisec=0) -> DateTime"""
        return _misc_.DateTime_SetHMS(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, int day, int month=Inv_Month, int year=Inv_Year, int hour=0, 
            int minute=0, int second=0, int millisec=0) -> DateTime
        """
        return _misc_.DateTime_Set(*args, **kwargs)

    def ResetTime(*args, **kwargs):
        """ResetTime(self) -> DateTime"""
        return _misc_.DateTime_ResetTime(*args, **kwargs)

    def SetYear(*args, **kwargs):
        """SetYear(self, int year) -> DateTime"""
        return _misc_.DateTime_SetYear(*args, **kwargs)

    def SetMonth(*args, **kwargs):
        """SetMonth(self, int month) -> DateTime"""
        return _misc_.DateTime_SetMonth(*args, **kwargs)

    def SetDay(*args, **kwargs):
        """SetDay(self, int day) -> DateTime"""
        return _misc_.DateTime_SetDay(*args, **kwargs)

    def SetHour(*args, **kwargs):
        """SetHour(self, int hour) -> DateTime"""
        return _misc_.DateTime_SetHour(*args, **kwargs)

    def SetMinute(*args, **kwargs):
        """SetMinute(self, int minute) -> DateTime"""
        return _misc_.DateTime_SetMinute(*args, **kwargs)

    def SetSecond(*args, **kwargs):
        """SetSecond(self, int second) -> DateTime"""
        return _misc_.DateTime_SetSecond(*args, **kwargs)

    def SetMillisecond(*args, **kwargs):
        """SetMillisecond(self, int millisecond) -> DateTime"""
        return _misc_.DateTime_SetMillisecond(*args, **kwargs)

    def SetToWeekDayInSameWeek(*args, **kwargs):
        """SetToWeekDayInSameWeek(self, int weekday, int flags=Monday_First) -> DateTime"""
        return _misc_.DateTime_SetToWeekDayInSameWeek(*args, **kwargs)

    def GetWeekDayInSameWeek(*args, **kwargs):
        """GetWeekDayInSameWeek(self, int weekday, int flags=Monday_First) -> DateTime"""
        return _misc_.DateTime_GetWeekDayInSameWeek(*args, **kwargs)

    def SetToNextWeekDay(*args, **kwargs):
        """SetToNextWeekDay(self, int weekday) -> DateTime"""
        return _misc_.DateTime_SetToNextWeekDay(*args, **kwargs)

    def GetNextWeekDay(*args, **kwargs):
        """GetNextWeekDay(self, int weekday) -> DateTime"""
        return _misc_.DateTime_GetNextWeekDay(*args, **kwargs)

    def SetToPrevWeekDay(*args, **kwargs):
        """SetToPrevWeekDay(self, int weekday) -> DateTime"""
        return _misc_.DateTime_SetToPrevWeekDay(*args, **kwargs)

    def GetPrevWeekDay(*args, **kwargs):
        """GetPrevWeekDay(self, int weekday) -> DateTime"""
        return _misc_.DateTime_GetPrevWeekDay(*args, **kwargs)

    def SetToWeekDay(*args, **kwargs):
        """SetToWeekDay(self, int weekday, int n=1, int month=Inv_Month, int year=Inv_Year) -> bool"""
        return _misc_.DateTime_SetToWeekDay(*args, **kwargs)

    def SetToLastWeekDay(*args, **kwargs):
        """SetToLastWeekDay(self, int weekday, int month=Inv_Month, int year=Inv_Year) -> bool"""
        return _misc_.DateTime_SetToLastWeekDay(*args, **kwargs)

    def GetLastWeekDay(*args, **kwargs):
        """GetLastWeekDay(self, int weekday, int month=Inv_Month, int year=Inv_Year) -> DateTime"""
        return _misc_.DateTime_GetLastWeekDay(*args, **kwargs)

    def SetToTheWeek(*args, **kwargs):
        """SetToTheWeek(self, int numWeek, int weekday=Mon, int flags=Monday_First) -> bool"""
        return _misc_.DateTime_SetToTheWeek(*args, **kwargs)

    def GetWeek(*args, **kwargs):
        """GetWeek(self, int numWeek, int weekday=Mon, int flags=Monday_First) -> DateTime"""
        return _misc_.DateTime_GetWeek(*args, **kwargs)

    SetToTheWeek = wx._deprecated(SetToTheWeek, "SetToTheWeek is deprecated, use (static) SetToWeekOfYear instead")
    GetWeek = wx._deprecated(GetWeek, "GetWeek is deprecated, use GetWeekOfYear instead")

    def SetToWeekOfYear(*args, **kwargs):
        """SetToWeekOfYear(int year, int numWeek, int weekday=Mon) -> DateTime"""
        return _misc_.DateTime_SetToWeekOfYear(*args, **kwargs)

    SetToWeekOfYear = staticmethod(SetToWeekOfYear)
    def SetToLastMonthDay(*args, **kwargs):
        """SetToLastMonthDay(self, int month=Inv_Month, int year=Inv_Year) -> DateTime"""
        return _misc_.DateTime_SetToLastMonthDay(*args, **kwargs)

    def GetLastMonthDay(*args, **kwargs):
        """GetLastMonthDay(self, int month=Inv_Month, int year=Inv_Year) -> DateTime"""
        return _misc_.DateTime_GetLastMonthDay(*args, **kwargs)

    def SetToYearDay(*args, **kwargs):
        """SetToYearDay(self, int yday) -> DateTime"""
        return _misc_.DateTime_SetToYearDay(*args, **kwargs)

    def GetYearDay(*args, **kwargs):
        """GetYearDay(self, int yday) -> DateTime"""
        return _misc_.DateTime_GetYearDay(*args, **kwargs)

    def GetJulianDayNumber(*args, **kwargs):
        """GetJulianDayNumber(self) -> double"""
        return _misc_.DateTime_GetJulianDayNumber(*args, **kwargs)

    def GetJDN(*args, **kwargs):
        """GetJDN(self) -> double"""
        return _misc_.DateTime_GetJDN(*args, **kwargs)

    def GetModifiedJulianDayNumber(*args, **kwargs):
        """GetModifiedJulianDayNumber(self) -> double"""
        return _misc_.DateTime_GetModifiedJulianDayNumber(*args, **kwargs)

    def GetMJD(*args, **kwargs):
        """GetMJD(self) -> double"""
        return _misc_.DateTime_GetMJD(*args, **kwargs)

    def GetRataDie(*args, **kwargs):
        """GetRataDie(self) -> double"""
        return _misc_.DateTime_GetRataDie(*args, **kwargs)

    def ToTimezone(*args, **kwargs):
        """ToTimezone(self, wxDateTime::TimeZone tz, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_ToTimezone(*args, **kwargs)

    def MakeTimezone(*args, **kwargs):
        """MakeTimezone(self, wxDateTime::TimeZone tz, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_MakeTimezone(*args, **kwargs)

    def FromTimezone(*args, **kwargs):
        """FromTimezone(self, wxDateTime::TimeZone tz, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_FromTimezone(*args, **kwargs)

    def MakeFromTimezone(*args, **kwargs):
        """MakeFromTimezone(self, wxDateTime::TimeZone tz, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_MakeFromTimezone(*args, **kwargs)

    def ToUTC(*args, **kwargs):
        """ToUTC(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_ToUTC(*args, **kwargs)

    def MakeUTC(*args, **kwargs):
        """MakeUTC(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_MakeUTC(*args, **kwargs)

    def ToGMT(*args, **kwargs):
        """ToGMT(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_ToGMT(*args, **kwargs)

    def MakeGMT(*args, **kwargs):
        """MakeGMT(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_MakeGMT(*args, **kwargs)

    def FromUTC(*args, **kwargs):
        """FromUTC(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_FromUTC(*args, **kwargs)

    def MakeFromUTC(*args, **kwargs):
        """MakeFromUTC(self, bool noDST=False) -> DateTime"""
        return _misc_.DateTime_MakeFromUTC(*args, **kwargs)

    def IsDST(*args, **kwargs):
        """IsDST(self, int country=Country_Default) -> int"""
        return _misc_.DateTime_IsDST(*args, **kwargs)

    def IsValid(*args, **kwargs):
        """IsValid(self) -> bool"""
        return _misc_.DateTime_IsValid(*args, **kwargs)

    Ok = IsValid 
    def __nonzero__(self): return self.Ok() 
    def GetTicks(*args, **kwargs):
        """GetTicks(self) -> time_t"""
        return _misc_.DateTime_GetTicks(*args, **kwargs)

    def GetYear(*args, **kwargs):
        """GetYear(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetYear(*args, **kwargs)

    def GetMonth(*args, **kwargs):
        """GetMonth(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetMonth(*args, **kwargs)

    def GetDay(*args, **kwargs):
        """GetDay(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetDay(*args, **kwargs)

    def GetWeekDay(*args, **kwargs):
        """GetWeekDay(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetWeekDay(*args, **kwargs)

    def GetHour(*args, **kwargs):
        """GetHour(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetHour(*args, **kwargs)

    def GetMinute(*args, **kwargs):
        """GetMinute(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetMinute(*args, **kwargs)

    def GetSecond(*args, **kwargs):
        """GetSecond(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetSecond(*args, **kwargs)

    def GetMillisecond(*args, **kwargs):
        """GetMillisecond(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetMillisecond(*args, **kwargs)

    def GetDayOfYear(*args, **kwargs):
        """GetDayOfYear(self, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetDayOfYear(*args, **kwargs)

    def GetWeekOfYear(*args, **kwargs):
        """GetWeekOfYear(self, int flags=Monday_First, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetWeekOfYear(*args, **kwargs)

    def GetWeekOfMonth(*args, **kwargs):
        """GetWeekOfMonth(self, int flags=Monday_First, wxDateTime::TimeZone tz=LOCAL_TZ) -> int"""
        return _misc_.DateTime_GetWeekOfMonth(*args, **kwargs)

    def IsWorkDay(*args, **kwargs):
        """IsWorkDay(self, int country=Country_Default) -> bool"""
        return _misc_.DateTime_IsWorkDay(*args, **kwargs)

    def IsEqualTo(*args, **kwargs):
        """IsEqualTo(self, DateTime datetime) -> bool"""
        return _misc_.DateTime_IsEqualTo(*args, **kwargs)

    def IsEarlierThan(*args, **kwargs):
        """IsEarlierThan(self, DateTime datetime) -> bool"""
        return _misc_.DateTime_IsEarlierThan(*args, **kwargs)

    def IsLaterThan(*args, **kwargs):
        """IsLaterThan(self, DateTime datetime) -> bool"""
        return _misc_.DateTime_IsLaterThan(*args, **kwargs)

    def IsStrictlyBetween(*args, **kwargs):
        """IsStrictlyBetween(self, DateTime t1, DateTime t2) -> bool"""
        return _misc_.DateTime_IsStrictlyBetween(*args, **kwargs)

    def IsBetween(*args, **kwargs):
        """IsBetween(self, DateTime t1, DateTime t2) -> bool"""
        return _misc_.DateTime_IsBetween(*args, **kwargs)

    def IsSameDate(*args, **kwargs):
        """IsSameDate(self, DateTime dt) -> bool"""
        return _misc_.DateTime_IsSameDate(*args, **kwargs)

    def IsSameTime(*args, **kwargs):
        """IsSameTime(self, DateTime dt) -> bool"""
        return _misc_.DateTime_IsSameTime(*args, **kwargs)

    def IsEqualUpTo(*args, **kwargs):
        """IsEqualUpTo(self, DateTime dt, TimeSpan ts) -> bool"""
        return _misc_.DateTime_IsEqualUpTo(*args, **kwargs)

    def AddTS(*args, **kwargs):
        """AddTS(self, TimeSpan diff) -> DateTime"""
        return _misc_.DateTime_AddTS(*args, **kwargs)

    def AddDS(*args, **kwargs):
        """AddDS(self, DateSpan diff) -> DateTime"""
        return _misc_.DateTime_AddDS(*args, **kwargs)

    def SubtractTS(*args, **kwargs):
        """SubtractTS(self, TimeSpan diff) -> DateTime"""
        return _misc_.DateTime_SubtractTS(*args, **kwargs)

    def SubtractDS(*args, **kwargs):
        """SubtractDS(self, DateSpan diff) -> DateTime"""
        return _misc_.DateTime_SubtractDS(*args, **kwargs)

    def Subtract(*args, **kwargs):
        """Subtract(self, DateTime dt) -> TimeSpan"""
        return _misc_.DateTime_Subtract(*args, **kwargs)

    def __iadd__(*args):
        """
        __iadd__(self, TimeSpan diff) -> DateTime
        __iadd__(self, DateSpan diff) -> DateTime
        """
        return _misc_.DateTime___iadd__(*args)

    def __isub__(*args):
        """
        __isub__(self, TimeSpan diff) -> DateTime
        __isub__(self, DateSpan diff) -> DateTime
        """
        return _misc_.DateTime___isub__(*args)

    def __add__(*args):
        """
        __add__(self, TimeSpan other) -> DateTime
        __add__(self, DateSpan other) -> DateTime
        """
        return _misc_.DateTime___add__(*args)

    def __sub__(*args):
        """
        __sub__(self, DateTime other) -> TimeSpan
        __sub__(self, TimeSpan other) -> DateTime
        __sub__(self, DateSpan other) -> DateTime
        """
        return _misc_.DateTime___sub__(*args)

    def __lt__(*args, **kwargs):
        """__lt__(self, DateTime other) -> bool"""
        return _misc_.DateTime___lt__(*args, **kwargs)

    def __le__(*args, **kwargs):
        """__le__(self, DateTime other) -> bool"""
        return _misc_.DateTime___le__(*args, **kwargs)

    def __gt__(*args, **kwargs):
        """__gt__(self, DateTime other) -> bool"""
        return _misc_.DateTime___gt__(*args, **kwargs)

    def __ge__(*args, **kwargs):
        """__ge__(self, DateTime other) -> bool"""
        return _misc_.DateTime___ge__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, DateTime other) -> bool"""
        return _misc_.DateTime___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, DateTime other) -> bool"""
        return _misc_.DateTime___ne__(*args, **kwargs)

    def ParseRfc822Date(*args, **kwargs):
        """ParseRfc822Date(self, String date) -> int"""
        return _misc_.DateTime_ParseRfc822Date(*args, **kwargs)

    def ParseFormat(*args, **kwargs):
        """ParseFormat(self, String date, String format=DefaultDateTimeFormat, DateTime dateDef=DefaultDateTime) -> int"""
        return _misc_.DateTime_ParseFormat(*args, **kwargs)

    def ParseDateTime(*args, **kwargs):
        """ParseDateTime(self, String datetime) -> int"""
        return _misc_.DateTime_ParseDateTime(*args, **kwargs)

    def ParseDate(*args, **kwargs):
        """ParseDate(self, String date) -> int"""
        return _misc_.DateTime_ParseDate(*args, **kwargs)

    def ParseTime(*args, **kwargs):
        """ParseTime(self, String time) -> int"""
        return _misc_.DateTime_ParseTime(*args, **kwargs)

    def Format(*args, **kwargs):
        """Format(self, String format=DefaultDateTimeFormat, wxDateTime::TimeZone tz=LOCAL_TZ) -> String"""
        return _misc_.DateTime_Format(*args, **kwargs)

    def FormatDate(*args, **kwargs):
        """FormatDate(self) -> String"""
        return _misc_.DateTime_FormatDate(*args, **kwargs)

    def FormatTime(*args, **kwargs):
        """FormatTime(self) -> String"""
        return _misc_.DateTime_FormatTime(*args, **kwargs)

    def FormatISODate(*args, **kwargs):
        """FormatISODate(self) -> String"""
        return _misc_.DateTime_FormatISODate(*args, **kwargs)

    def FormatISOTime(*args, **kwargs):
        """FormatISOTime(self) -> String"""
        return _misc_.DateTime_FormatISOTime(*args, **kwargs)

    def __repr__(self):
        if self.IsValid():
            f = self.Format().encode(wx.GetDefaultPyEncoding())
            return '<wx.DateTime: \"%s\" at %s>' % ( f, self.this)
        else:
            return '<wx.DateTime: \"INVALID\" at %s>' % self.this
    def __str__(self):
        if self.IsValid():
            return self.Format().encode(wx.GetDefaultPyEncoding())
        else:
            return "INVALID DateTime"

    Day = property(GetDay,SetDay,doc="See `GetDay` and `SetDay`") 
    DayOfYear = property(GetDayOfYear,doc="See `GetDayOfYear`") 
    Hour = property(GetHour,SetHour,doc="See `GetHour` and `SetHour`") 
    JDN = property(GetJDN,SetJDN,doc="See `GetJDN` and `SetJDN`") 
    JulianDayNumber = property(GetJulianDayNumber,doc="See `GetJulianDayNumber`") 
    LastMonthDay = property(GetLastMonthDay,doc="See `GetLastMonthDay`") 
    LastWeekDay = property(GetLastWeekDay,doc="See `GetLastWeekDay`") 
    MJD = property(GetMJD,doc="See `GetMJD`") 
    Millisecond = property(GetMillisecond,SetMillisecond,doc="See `GetMillisecond` and `SetMillisecond`") 
    Minute = property(GetMinute,SetMinute,doc="See `GetMinute` and `SetMinute`") 
    ModifiedJulianDayNumber = property(GetModifiedJulianDayNumber,doc="See `GetModifiedJulianDayNumber`") 
    Month = property(GetMonth,SetMonth,doc="See `GetMonth` and `SetMonth`") 
    NextWeekDay = property(GetNextWeekDay,doc="See `GetNextWeekDay`") 
    PrevWeekDay = property(GetPrevWeekDay,doc="See `GetPrevWeekDay`") 
    RataDie = property(GetRataDie,doc="See `GetRataDie`") 
    Second = property(GetSecond,SetSecond,doc="See `GetSecond` and `SetSecond`") 
    Ticks = property(GetTicks,doc="See `GetTicks`") 
    Week = property(GetWeek,doc="See `GetWeek`") 
    WeekDay = property(GetWeekDay,doc="See `GetWeekDay`") 
    WeekDayInSameWeek = property(GetWeekDayInSameWeek,doc="See `GetWeekDayInSameWeek`") 
    WeekOfMonth = property(GetWeekOfMonth,doc="See `GetWeekOfMonth`") 
    WeekOfYear = property(GetWeekOfYear,doc="See `GetWeekOfYear`") 
    Year = property(GetYear,SetYear,doc="See `GetYear` and `SetYear`") 
    YearDay = property(GetYearDay,doc="See `GetYearDay`") 
_misc_.DateTime_swigregister(DateTime)
DefaultDateTimeFormat = cvar.DefaultDateTimeFormat
DefaultTimeSpanFormat = cvar.DefaultTimeSpanFormat

def DateTime_SetCountry(*args, **kwargs):
  """DateTime_SetCountry(int country)"""
  return _misc_.DateTime_SetCountry(*args, **kwargs)

def DateTime_GetCountry(*args):
  """DateTime_GetCountry() -> int"""
  return _misc_.DateTime_GetCountry(*args)

def DateTime_IsWestEuropeanCountry(*args, **kwargs):
  """DateTime_IsWestEuropeanCountry(int country=Country_Default) -> bool"""
  return _misc_.DateTime_IsWestEuropeanCountry(*args, **kwargs)

def DateTime_GetCurrentYear(*args, **kwargs):
  """DateTime_GetCurrentYear(int cal=Gregorian) -> int"""
  return _misc_.DateTime_GetCurrentYear(*args, **kwargs)

def DateTime_ConvertYearToBC(*args, **kwargs):
  """DateTime_ConvertYearToBC(int year) -> int"""
  return _misc_.DateTime_ConvertYearToBC(*args, **kwargs)

def DateTime_GetCurrentMonth(*args, **kwargs):
  """DateTime_GetCurrentMonth(int cal=Gregorian) -> int"""
  return _misc_.DateTime_GetCurrentMonth(*args, **kwargs)

def DateTime_IsLeapYear(*args, **kwargs):
  """DateTime_IsLeapYear(int year=Inv_Year, int cal=Gregorian) -> bool"""
  return _misc_.DateTime_IsLeapYear(*args, **kwargs)

def DateTime_GetCentury(*args, **kwargs):
  """DateTime_GetCentury(int year=Inv_Year) -> int"""
  return _misc_.DateTime_GetCentury(*args, **kwargs)

def DateTime_GetNumberOfDaysinYear(*args, **kwargs):
  """DateTime_GetNumberOfDaysinYear(int year, int cal=Gregorian) -> int"""
  return _misc_.DateTime_GetNumberOfDaysinYear(*args, **kwargs)

def DateTime_GetNumberOfDaysInMonth(*args, **kwargs):
  """DateTime_GetNumberOfDaysInMonth(int month, int year=Inv_Year, int cal=Gregorian) -> int"""
  return _misc_.DateTime_GetNumberOfDaysInMonth(*args, **kwargs)

def DateTime_GetMonthName(*args, **kwargs):
  """DateTime_GetMonthName(int month, int flags=Name_Full) -> String"""
  return _misc_.DateTime_GetMonthName(*args, **kwargs)

def DateTime_GetWeekDayName(*args, **kwargs):
  """DateTime_GetWeekDayName(int weekday, int flags=Name_Full) -> String"""
  return _misc_.DateTime_GetWeekDayName(*args, **kwargs)

def DateTime_GetAmPmStrings(*args):
  """
    GetAmPmStrings() -> (am, pm)

    Get the AM and PM strings in the current locale (may be empty)
    """
  return _misc_.DateTime_GetAmPmStrings(*args)

def DateTime_IsDSTApplicable(*args, **kwargs):
  """DateTime_IsDSTApplicable(int year=Inv_Year, int country=Country_Default) -> bool"""
  return _misc_.DateTime_IsDSTApplicable(*args, **kwargs)

def DateTime_GetBeginDST(*args, **kwargs):
  """DateTime_GetBeginDST(int year=Inv_Year, int country=Country_Default) -> DateTime"""
  return _misc_.DateTime_GetBeginDST(*args, **kwargs)

def DateTime_GetEndDST(*args, **kwargs):
  """DateTime_GetEndDST(int year=Inv_Year, int country=Country_Default) -> DateTime"""
  return _misc_.DateTime_GetEndDST(*args, **kwargs)

def DateTime_Now(*args):
  """DateTime_Now() -> DateTime"""
  return _misc_.DateTime_Now(*args)

def DateTime_UNow(*args):
  """DateTime_UNow() -> DateTime"""
  return _misc_.DateTime_UNow(*args)

def DateTime_Today(*args):
  """DateTime_Today() -> DateTime"""
  return _misc_.DateTime_Today(*args)

def DateTimeFromTimeT(*args, **kwargs):
    """DateTimeFromTimeT(time_t timet) -> DateTime"""
    val = _misc_.new_DateTimeFromTimeT(*args, **kwargs)
    return val

def DateTimeFromJDN(*args, **kwargs):
    """DateTimeFromJDN(double jdn) -> DateTime"""
    val = _misc_.new_DateTimeFromJDN(*args, **kwargs)
    return val

def DateTimeFromHMS(*args, **kwargs):
    """DateTimeFromHMS(int hour, int minute=0, int second=0, int millisec=0) -> DateTime"""
    val = _misc_.new_DateTimeFromHMS(*args, **kwargs)
    return val

def DateTimeFromDMY(*args, **kwargs):
    """
    DateTimeFromDMY(int day, int month=Inv_Month, int year=Inv_Year, int hour=0, 
        int minute=0, int second=0, int millisec=0) -> DateTime
    """
    val = _misc_.new_DateTimeFromDMY(*args, **kwargs)
    return val

def DateTimeFromDateTime(*args, **kwargs):
    """DateTimeFromDateTime(DateTime date) -> DateTime"""
    val = _misc_.new_DateTimeFromDateTime(*args, **kwargs)
    return val

def DateTime_SetToWeekOfYear(*args, **kwargs):
  """DateTime_SetToWeekOfYear(int year, int numWeek, int weekday=Mon) -> DateTime"""
  return _misc_.DateTime_SetToWeekOfYear(*args, **kwargs)

class TimeSpan(object):
    """Proxy of C++ TimeSpan class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def Milliseconds(*args, **kwargs):
        """Milliseconds(long ms) -> TimeSpan"""
        return _misc_.TimeSpan_Milliseconds(*args, **kwargs)

    Milliseconds = staticmethod(Milliseconds)
    def Millisecond(*args, **kwargs):
        """Millisecond() -> TimeSpan"""
        return _misc_.TimeSpan_Millisecond(*args, **kwargs)

    Millisecond = staticmethod(Millisecond)
    def Seconds(*args, **kwargs):
        """Seconds(long sec) -> TimeSpan"""
        return _misc_.TimeSpan_Seconds(*args, **kwargs)

    Seconds = staticmethod(Seconds)
    def Second(*args, **kwargs):
        """Second() -> TimeSpan"""
        return _misc_.TimeSpan_Second(*args, **kwargs)

    Second = staticmethod(Second)
    def Minutes(*args, **kwargs):
        """Minutes(long min) -> TimeSpan"""
        return _misc_.TimeSpan_Minutes(*args, **kwargs)

    Minutes = staticmethod(Minutes)
    def Minute(*args, **kwargs):
        """Minute() -> TimeSpan"""
        return _misc_.TimeSpan_Minute(*args, **kwargs)

    Minute = staticmethod(Minute)
    def Hours(*args, **kwargs):
        """Hours(long hours) -> TimeSpan"""
        return _misc_.TimeSpan_Hours(*args, **kwargs)

    Hours = staticmethod(Hours)
    def Hour(*args, **kwargs):
        """Hour() -> TimeSpan"""
        return _misc_.TimeSpan_Hour(*args, **kwargs)

    Hour = staticmethod(Hour)
    def Days(*args, **kwargs):
        """Days(long days) -> TimeSpan"""
        return _misc_.TimeSpan_Days(*args, **kwargs)

    Days = staticmethod(Days)
    def Day(*args, **kwargs):
        """Day() -> TimeSpan"""
        return _misc_.TimeSpan_Day(*args, **kwargs)

    Day = staticmethod(Day)
    def Weeks(*args, **kwargs):
        """Weeks(long days) -> TimeSpan"""
        return _misc_.TimeSpan_Weeks(*args, **kwargs)

    Weeks = staticmethod(Weeks)
    def Week(*args, **kwargs):
        """Week() -> TimeSpan"""
        return _misc_.TimeSpan_Week(*args, **kwargs)

    Week = staticmethod(Week)
    def __init__(self, *args, **kwargs): 
        """__init__(self, long hours=0, long minutes=0, long seconds=0, long milliseconds=0) -> TimeSpan"""
        _misc_.TimeSpan_swiginit(self,_misc_.new_TimeSpan(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_TimeSpan
    __del__ = lambda self : None;
    def Add(*args, **kwargs):
        """Add(self, TimeSpan diff) -> TimeSpan"""
        return _misc_.TimeSpan_Add(*args, **kwargs)

    def Subtract(*args, **kwargs):
        """Subtract(self, TimeSpan diff) -> TimeSpan"""
        return _misc_.TimeSpan_Subtract(*args, **kwargs)

    def Multiply(*args, **kwargs):
        """Multiply(self, int n) -> TimeSpan"""
        return _misc_.TimeSpan_Multiply(*args, **kwargs)

    def Neg(*args, **kwargs):
        """Neg(self) -> TimeSpan"""
        return _misc_.TimeSpan_Neg(*args, **kwargs)

    def Abs(*args, **kwargs):
        """Abs(self) -> TimeSpan"""
        return _misc_.TimeSpan_Abs(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """__iadd__(self, TimeSpan diff) -> TimeSpan"""
        return _misc_.TimeSpan___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """__isub__(self, TimeSpan diff) -> TimeSpan"""
        return _misc_.TimeSpan___isub__(*args, **kwargs)

    def __imul__(*args, **kwargs):
        """__imul__(self, int n) -> TimeSpan"""
        return _misc_.TimeSpan___imul__(*args, **kwargs)

    def __neg__(*args, **kwargs):
        """__neg__(self) -> TimeSpan"""
        return _misc_.TimeSpan___neg__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """__add__(self, TimeSpan other) -> TimeSpan"""
        return _misc_.TimeSpan___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """__sub__(self, TimeSpan other) -> TimeSpan"""
        return _misc_.TimeSpan___sub__(*args, **kwargs)

    def __mul__(*args, **kwargs):
        """__mul__(self, int n) -> TimeSpan"""
        return _misc_.TimeSpan___mul__(*args, **kwargs)

    def __rmul__(*args, **kwargs):
        """__rmul__(self, int n) -> TimeSpan"""
        return _misc_.TimeSpan___rmul__(*args, **kwargs)

    def __lt__(*args, **kwargs):
        """__lt__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___lt__(*args, **kwargs)

    def __le__(*args, **kwargs):
        """__le__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___le__(*args, **kwargs)

    def __gt__(*args, **kwargs):
        """__gt__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___gt__(*args, **kwargs)

    def __ge__(*args, **kwargs):
        """__ge__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___ge__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, TimeSpan other) -> bool"""
        return _misc_.TimeSpan___ne__(*args, **kwargs)

    def IsNull(*args, **kwargs):
        """IsNull(self) -> bool"""
        return _misc_.TimeSpan_IsNull(*args, **kwargs)

    def IsPositive(*args, **kwargs):
        """IsPositive(self) -> bool"""
        return _misc_.TimeSpan_IsPositive(*args, **kwargs)

    def IsNegative(*args, **kwargs):
        """IsNegative(self) -> bool"""
        return _misc_.TimeSpan_IsNegative(*args, **kwargs)

    def IsEqualTo(*args, **kwargs):
        """IsEqualTo(self, TimeSpan ts) -> bool"""
        return _misc_.TimeSpan_IsEqualTo(*args, **kwargs)

    def IsLongerThan(*args, **kwargs):
        """IsLongerThan(self, TimeSpan ts) -> bool"""
        return _misc_.TimeSpan_IsLongerThan(*args, **kwargs)

    def IsShorterThan(*args, **kwargs):
        """IsShorterThan(self, TimeSpan t) -> bool"""
        return _misc_.TimeSpan_IsShorterThan(*args, **kwargs)

    def GetWeeks(*args, **kwargs):
        """GetWeeks(self) -> int"""
        return _misc_.TimeSpan_GetWeeks(*args, **kwargs)

    def GetDays(*args, **kwargs):
        """GetDays(self) -> int"""
        return _misc_.TimeSpan_GetDays(*args, **kwargs)

    def GetHours(*args, **kwargs):
        """GetHours(self) -> int"""
        return _misc_.TimeSpan_GetHours(*args, **kwargs)

    def GetMinutes(*args, **kwargs):
        """GetMinutes(self) -> int"""
        return _misc_.TimeSpan_GetMinutes(*args, **kwargs)

    def GetSeconds(*args, **kwargs):
        """GetSeconds(self) -> wxLongLong"""
        return _misc_.TimeSpan_GetSeconds(*args, **kwargs)

    def GetMilliseconds(*args, **kwargs):
        """GetMilliseconds(self) -> wxLongLong"""
        return _misc_.TimeSpan_GetMilliseconds(*args, **kwargs)

    def Format(*args, **kwargs):
        """Format(self, String format=DefaultTimeSpanFormat) -> String"""
        return _misc_.TimeSpan_Format(*args, **kwargs)

    def __repr__(self):
        f = self.Format().encode(wx.GetDefaultPyEncoding())
        return '<wx.TimeSpan: \"%s\" at %s>' % ( f, self.this)
    def __str__(self):
        return self.Format().encode(wx.GetDefaultPyEncoding())

    Days = property(GetDays,doc="See `GetDays`") 
    Hours = property(GetHours,doc="See `GetHours`") 
    Milliseconds = property(GetMilliseconds,doc="See `GetMilliseconds`") 
    Minutes = property(GetMinutes,doc="See `GetMinutes`") 
    Seconds = property(GetSeconds,doc="See `GetSeconds`") 
    Weeks = property(GetWeeks,doc="See `GetWeeks`") 
_misc_.TimeSpan_swigregister(TimeSpan)

def TimeSpan_Milliseconds(*args, **kwargs):
  """TimeSpan_Milliseconds(long ms) -> TimeSpan"""
  return _misc_.TimeSpan_Milliseconds(*args, **kwargs)

def TimeSpan_Millisecond(*args):
  """TimeSpan_Millisecond() -> TimeSpan"""
  return _misc_.TimeSpan_Millisecond(*args)

def TimeSpan_Seconds(*args, **kwargs):
  """TimeSpan_Seconds(long sec) -> TimeSpan"""
  return _misc_.TimeSpan_Seconds(*args, **kwargs)

def TimeSpan_Second(*args):
  """TimeSpan_Second() -> TimeSpan"""
  return _misc_.TimeSpan_Second(*args)

def TimeSpan_Minutes(*args, **kwargs):
  """TimeSpan_Minutes(long min) -> TimeSpan"""
  return _misc_.TimeSpan_Minutes(*args, **kwargs)

def TimeSpan_Minute(*args):
  """TimeSpan_Minute() -> TimeSpan"""
  return _misc_.TimeSpan_Minute(*args)

def TimeSpan_Hours(*args, **kwargs):
  """TimeSpan_Hours(long hours) -> TimeSpan"""
  return _misc_.TimeSpan_Hours(*args, **kwargs)

def TimeSpan_Hour(*args):
  """TimeSpan_Hour() -> TimeSpan"""
  return _misc_.TimeSpan_Hour(*args)

def TimeSpan_Days(*args, **kwargs):
  """TimeSpan_Days(long days) -> TimeSpan"""
  return _misc_.TimeSpan_Days(*args, **kwargs)

def TimeSpan_Day(*args):
  """TimeSpan_Day() -> TimeSpan"""
  return _misc_.TimeSpan_Day(*args)

def TimeSpan_Weeks(*args, **kwargs):
  """TimeSpan_Weeks(long days) -> TimeSpan"""
  return _misc_.TimeSpan_Weeks(*args, **kwargs)

def TimeSpan_Week(*args):
  """TimeSpan_Week() -> TimeSpan"""
  return _misc_.TimeSpan_Week(*args)

class DateSpan(object):
    """Proxy of C++ DateSpan class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int years=0, int months=0, int weeks=0, int days=0) -> DateSpan"""
        _misc_.DateSpan_swiginit(self,_misc_.new_DateSpan(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_DateSpan
    __del__ = lambda self : None;
    def Days(*args, **kwargs):
        """Days(int days) -> DateSpan"""
        return _misc_.DateSpan_Days(*args, **kwargs)

    Days = staticmethod(Days)
    def Day(*args, **kwargs):
        """Day() -> DateSpan"""
        return _misc_.DateSpan_Day(*args, **kwargs)

    Day = staticmethod(Day)
    def Weeks(*args, **kwargs):
        """Weeks(int weeks) -> DateSpan"""
        return _misc_.DateSpan_Weeks(*args, **kwargs)

    Weeks = staticmethod(Weeks)
    def Week(*args, **kwargs):
        """Week() -> DateSpan"""
        return _misc_.DateSpan_Week(*args, **kwargs)

    Week = staticmethod(Week)
    def Months(*args, **kwargs):
        """Months(int mon) -> DateSpan"""
        return _misc_.DateSpan_Months(*args, **kwargs)

    Months = staticmethod(Months)
    def Month(*args, **kwargs):
        """Month() -> DateSpan"""
        return _misc_.DateSpan_Month(*args, **kwargs)

    Month = staticmethod(Month)
    def Years(*args, **kwargs):
        """Years(int years) -> DateSpan"""
        return _misc_.DateSpan_Years(*args, **kwargs)

    Years = staticmethod(Years)
    def Year(*args, **kwargs):
        """Year() -> DateSpan"""
        return _misc_.DateSpan_Year(*args, **kwargs)

    Year = staticmethod(Year)
    def SetYears(*args, **kwargs):
        """SetYears(self, int n) -> DateSpan"""
        return _misc_.DateSpan_SetYears(*args, **kwargs)

    def SetMonths(*args, **kwargs):
        """SetMonths(self, int n) -> DateSpan"""
        return _misc_.DateSpan_SetMonths(*args, **kwargs)

    def SetWeeks(*args, **kwargs):
        """SetWeeks(self, int n) -> DateSpan"""
        return _misc_.DateSpan_SetWeeks(*args, **kwargs)

    def SetDays(*args, **kwargs):
        """SetDays(self, int n) -> DateSpan"""
        return _misc_.DateSpan_SetDays(*args, **kwargs)

    def GetYears(*args, **kwargs):
        """GetYears(self) -> int"""
        return _misc_.DateSpan_GetYears(*args, **kwargs)

    def GetMonths(*args, **kwargs):
        """GetMonths(self) -> int"""
        return _misc_.DateSpan_GetMonths(*args, **kwargs)

    def GetWeeks(*args, **kwargs):
        """GetWeeks(self) -> int"""
        return _misc_.DateSpan_GetWeeks(*args, **kwargs)

    def GetDays(*args, **kwargs):
        """GetDays(self) -> int"""
        return _misc_.DateSpan_GetDays(*args, **kwargs)

    def GetTotalDays(*args, **kwargs):
        """GetTotalDays(self) -> int"""
        return _misc_.DateSpan_GetTotalDays(*args, **kwargs)

    def Add(*args, **kwargs):
        """Add(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan_Add(*args, **kwargs)

    def Subtract(*args, **kwargs):
        """Subtract(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan_Subtract(*args, **kwargs)

    def Neg(*args, **kwargs):
        """Neg(self) -> DateSpan"""
        return _misc_.DateSpan_Neg(*args, **kwargs)

    def Multiply(*args, **kwargs):
        """Multiply(self, int factor) -> DateSpan"""
        return _misc_.DateSpan_Multiply(*args, **kwargs)

    def __iadd__(*args, **kwargs):
        """__iadd__(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan___iadd__(*args, **kwargs)

    def __isub__(*args, **kwargs):
        """__isub__(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan___isub__(*args, **kwargs)

    def __neg__(*args, **kwargs):
        """__neg__(self) -> DateSpan"""
        return _misc_.DateSpan___neg__(*args, **kwargs)

    def __imul__(*args, **kwargs):
        """__imul__(self, int factor) -> DateSpan"""
        return _misc_.DateSpan___imul__(*args, **kwargs)

    def __add__(*args, **kwargs):
        """__add__(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan___add__(*args, **kwargs)

    def __sub__(*args, **kwargs):
        """__sub__(self, DateSpan other) -> DateSpan"""
        return _misc_.DateSpan___sub__(*args, **kwargs)

    def __mul__(*args, **kwargs):
        """__mul__(self, int n) -> DateSpan"""
        return _misc_.DateSpan___mul__(*args, **kwargs)

    def __rmul__(*args, **kwargs):
        """__rmul__(self, int n) -> DateSpan"""
        return _misc_.DateSpan___rmul__(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, DateSpan other) -> bool"""
        return _misc_.DateSpan___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, DateSpan other) -> bool"""
        return _misc_.DateSpan___ne__(*args, **kwargs)

    Days = property(GetDays,SetDays,doc="See `GetDays` and `SetDays`") 
    Months = property(GetMonths,SetMonths,doc="See `GetMonths` and `SetMonths`") 
    TotalDays = property(GetTotalDays,doc="See `GetTotalDays`") 
    Weeks = property(GetWeeks,SetWeeks,doc="See `GetWeeks` and `SetWeeks`") 
    Years = property(GetYears,SetYears,doc="See `GetYears` and `SetYears`") 
_misc_.DateSpan_swigregister(DateSpan)

def DateSpan_Days(*args, **kwargs):
  """DateSpan_Days(int days) -> DateSpan"""
  return _misc_.DateSpan_Days(*args, **kwargs)

def DateSpan_Day(*args):
  """DateSpan_Day() -> DateSpan"""
  return _misc_.DateSpan_Day(*args)

def DateSpan_Weeks(*args, **kwargs):
  """DateSpan_Weeks(int weeks) -> DateSpan"""
  return _misc_.DateSpan_Weeks(*args, **kwargs)

def DateSpan_Week(*args):
  """DateSpan_Week() -> DateSpan"""
  return _misc_.DateSpan_Week(*args)

def DateSpan_Months(*args, **kwargs):
  """DateSpan_Months(int mon) -> DateSpan"""
  return _misc_.DateSpan_Months(*args, **kwargs)

def DateSpan_Month(*args):
  """DateSpan_Month() -> DateSpan"""
  return _misc_.DateSpan_Month(*args)

def DateSpan_Years(*args, **kwargs):
  """DateSpan_Years(int years) -> DateSpan"""
  return _misc_.DateSpan_Years(*args, **kwargs)

def DateSpan_Year(*args):
  """DateSpan_Year() -> DateSpan"""
  return _misc_.DateSpan_Year(*args)


def GetLocalTime(*args):
  """GetLocalTime() -> long"""
  return _misc_.GetLocalTime(*args)

def GetUTCTime(*args):
  """GetUTCTime() -> long"""
  return _misc_.GetUTCTime(*args)

def GetCurrentTime(*args):
  """GetCurrentTime() -> long"""
  return _misc_.GetCurrentTime(*args)

def GetLocalTimeMillis(*args):
  """GetLocalTimeMillis() -> wxLongLong"""
  return _misc_.GetLocalTimeMillis(*args)
#---------------------------------------------------------------------------

DF_INVALID = _misc_.DF_INVALID
DF_TEXT = _misc_.DF_TEXT
DF_BITMAP = _misc_.DF_BITMAP
DF_METAFILE = _misc_.DF_METAFILE
DF_SYLK = _misc_.DF_SYLK
DF_DIF = _misc_.DF_DIF
DF_TIFF = _misc_.DF_TIFF
DF_OEMTEXT = _misc_.DF_OEMTEXT
DF_DIB = _misc_.DF_DIB
DF_PALETTE = _misc_.DF_PALETTE
DF_PENDATA = _misc_.DF_PENDATA
DF_RIFF = _misc_.DF_RIFF
DF_WAVE = _misc_.DF_WAVE
DF_UNICODETEXT = _misc_.DF_UNICODETEXT
DF_ENHMETAFILE = _misc_.DF_ENHMETAFILE
DF_FILENAME = _misc_.DF_FILENAME
DF_LOCALE = _misc_.DF_LOCALE
DF_PRIVATE = _misc_.DF_PRIVATE
DF_HTML = _misc_.DF_HTML
DF_MAX = _misc_.DF_MAX
class DataFormat(object):
    """
    A wx.DataFormat is an encapsulation of a platform-specific format
    handle which is used by the system for the clipboard and drag and drop
    operations. The applications are usually only interested in, for
    example, pasting data from the clipboard only if the data is in a
    format the program understands.  A data format is is used to uniquely
    identify this format.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int type) -> DataFormat

        Constructs a data format object for one of the standard data formats
        or an empty data object (use SetType or SetId later in this case)
        """
        _misc_.DataFormat_swiginit(self,_misc_.new_DataFormat(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_DataFormat
    __del__ = lambda self : None;
    def __eq__(*args):
        """
        __eq__(self, int format) -> bool
        __eq__(self, DataFormat format) -> bool
        """
        return _misc_.DataFormat___eq__(*args)

    def __ne__(*args):
        """
        __ne__(self, int format) -> bool
        __ne__(self, DataFormat format) -> bool
        """
        return _misc_.DataFormat___ne__(*args)

    def SetType(*args, **kwargs):
        """
        SetType(self, int format)

        Sets the format to the given value, which should be one of wx.DF_XXX
        constants.
        """
        return _misc_.DataFormat_SetType(*args, **kwargs)

    def GetType(*args, **kwargs):
        """
        GetType(self) -> int

        Returns the platform-specific number identifying the format.
        """
        return _misc_.DataFormat_GetType(*args, **kwargs)

    def GetId(*args, **kwargs):
        """
        GetId(self) -> String

        Returns the name of a custom format (this function will fail for a
        standard format).
        """
        return _misc_.DataFormat_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, String format)

        Sets the format to be the custom format identified by the given name.
        """
        return _misc_.DataFormat_SetId(*args, **kwargs)

    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
    Type = property(GetType,SetType,doc="See `GetType` and `SetType`") 
_misc_.DataFormat_swigregister(DataFormat)
DefaultDateTime = cvar.DefaultDateTime

def CustomDataFormat(*args, **kwargs):
    """
    CustomDataFormat(String format) -> DataFormat

    Constructs a data format object for a custom format identified by its
    name.
    """
    val = _misc_.new_CustomDataFormat(*args, **kwargs)
    return val

class DataObject(object):
    """
    A wx.DataObject represents data that can be copied to or from the
    clipboard, or dragged and dropped. The important thing about
    wx.DataObject is that this is a 'smart' piece of data unlike usual
    'dumb' data containers such as memory buffers or files. Being 'smart'
    here means that the data object itself should know what data formats
    it supports and how to render itself in each of supported formats.

    **NOTE**: This class is an abstract base class and can not be used
    directly from Python.  If you need a custom type of data object then
    you should instead derive from `wx.PyDataObjectSimple` or use
    `wx.CustomDataObject`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    Get = _misc_.DataObject_Get
    Set = _misc_.DataObject_Set
    Both = _misc_.DataObject_Both
    __swig_destroy__ = _misc_.delete_DataObject
    __del__ = lambda self : None;
    def GetPreferredFormat(*args, **kwargs):
        """
        GetPreferredFormat(self, int dir=Get) -> DataFormat

        Returns the preferred format for either rendering the data (if dir is
        Get, its default value) or for setting it. Usually this will be the
        native format of the wx.DataObject.
        """
        return _misc_.DataObject_GetPreferredFormat(*args, **kwargs)

    def GetFormatCount(*args, **kwargs):
        """
        GetFormatCount(self, int dir=Get) -> size_t

        Returns the number of available formats for rendering or setting the
        data.
        """
        return _misc_.DataObject_GetFormatCount(*args, **kwargs)

    def IsSupported(*args, **kwargs):
        """
        IsSupported(self, DataFormat format, int dir=Get) -> bool

        Returns True if this format is supported.
        """
        return _misc_.DataObject_IsSupported(*args, **kwargs)

    def GetDataSize(*args, **kwargs):
        """
        GetDataSize(self, DataFormat format) -> size_t

        Get the (total) size of data for the given format
        """
        return _misc_.DataObject_GetDataSize(*args, **kwargs)

    def GetAllFormats(*args, **kwargs):
        """
        GetAllFormats(self, int dir=Get) -> [formats]

        Returns a list of all the wx.DataFormats that this dataobject supports
        in the given direction.
        """
        return _misc_.DataObject_GetAllFormats(*args, **kwargs)

    def GetDataHere(*args, **kwargs):
        """
        GetDataHere(self, DataFormat format) -> String

        Get the data bytes in the specified format, returns None on failure.
        """
        return _misc_.DataObject_GetDataHere(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, DataFormat format, String data) -> bool

        Set the data in the specified format from the bytes in the the data string.

        """
        return _misc_.DataObject_SetData(*args, **kwargs)

    AllFormats = property(GetAllFormats,doc="See `GetAllFormats`") 
    DataHere = property(GetDataHere,doc="See `GetDataHere`") 
    DataSize = property(GetDataSize,doc="See `GetDataSize`") 
    FormatCount = property(GetFormatCount,doc="See `GetFormatCount`") 
    PreferredFormat = property(GetPreferredFormat,doc="See `GetPreferredFormat`") 
_misc_.DataObject_swigregister(DataObject)
FormatInvalid = cvar.FormatInvalid

class DataObjectSimple(DataObject):
    """
    wx.DataObjectSimple is a `wx.DataObject` which only supports one
    format.  This is the simplest possible `wx.DataObject` implementation.

    This is still an "abstract base class" meaning that you can't use it
    directly.  You either need to use one of the predefined base classes,
    or derive your own class from `wx.PyDataObjectSimple`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, DataFormat format=FormatInvalid) -> DataObjectSimple

        Constructor accepts the supported format (none by default) which may
        also be set later with `SetFormat`.
        """
        _misc_.DataObjectSimple_swiginit(self,_misc_.new_DataObjectSimple(*args, **kwargs))
    def GetFormat(*args, **kwargs):
        """
        GetFormat(self) -> DataFormat

        Returns the (one and only one) format supported by this object. It is
        assumed that the format is supported in both directions.
        """
        return _misc_.DataObjectSimple_GetFormat(*args, **kwargs)

    def SetFormat(*args, **kwargs):
        """
        SetFormat(self, DataFormat format)

        Sets the supported format.
        """
        return _misc_.DataObjectSimple_SetFormat(*args, **kwargs)

    def GetDataSize(*args, **kwargs):
        """
        GetDataSize(self) -> size_t

        Get the size of our data.
        """
        return _misc_.DataObjectSimple_GetDataSize(*args, **kwargs)

    def GetDataHere(*args, **kwargs):
        """
        GetDataHere(self) -> String

        Returns the data bytes from the data object as a string, returns None
        on failure.  Must be implemented in the derived class if the object
        supports rendering its data.
        """
        return _misc_.DataObjectSimple_GetDataHere(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, String data) -> bool

        Copy the data value to the data object.  Must be implemented in the
        derived class if the object supports setting its data.

        """
        return _misc_.DataObjectSimple_SetData(*args, **kwargs)

    Format = property(GetFormat,SetFormat,doc="See `GetFormat` and `SetFormat`") 
_misc_.DataObjectSimple_swigregister(DataObjectSimple)

class PyDataObjectSimple(DataObjectSimple):
    """
    wx.PyDataObjectSimple is a version of `wx.DataObjectSimple` that is
    Python-aware and knows how to reflect calls to its C++ virtual methods
    to methods in the Python derived class.  You should derive from this
    class and overload `GetDataSize`, `GetDataHere` and `SetData` when you
    need to create your own simple single-format type of `wx.DataObject`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, DataFormat format=FormatInvalid) -> PyDataObjectSimple

        wx.PyDataObjectSimple is a version of `wx.DataObjectSimple` that is
        Python-aware and knows how to reflect calls to its C++ virtual methods
        to methods in the Python derived class.  You should derive from this
        class and overload `GetDataSize`, `GetDataHere` and `SetData` when you
        need to create your own simple single-format type of `wx.DataObject`.

        """
        _misc_.PyDataObjectSimple_swiginit(self,_misc_.new_PyDataObjectSimple(*args, **kwargs))
        self._setCallbackInfo(self, PyDataObjectSimple)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.PyDataObjectSimple__setCallbackInfo(*args, **kwargs)

_misc_.PyDataObjectSimple_swigregister(PyDataObjectSimple)

class DataObjectComposite(DataObject):
    """
    wx.DataObjectComposite is the simplest `wx.DataObject` derivation
    which may be sued to support multiple formats. It contains several
    'wx.DataObjectSimple` objects and supports any format supported by at
    least one of them. Only one of these data objects is *preferred* (the
    first one if not explicitly changed by using the second parameter of
    `Add`) and its format determines the preferred format of the composite
    data object as well.

    See `wx.DataObject` documentation for the reasons why you might prefer
    to use wx.DataObject directly instead of wx.DataObjectComposite for
    efficiency reasons.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> DataObjectComposite

        wx.DataObjectComposite is the simplest `wx.DataObject` derivation
        which may be sued to support multiple formats. It contains several
        'wx.DataObjectSimple` objects and supports any format supported by at
        least one of them. Only one of these data objects is *preferred* (the
        first one if not explicitly changed by using the second parameter of
        `Add`) and its format determines the preferred format of the composite
        data object as well.

        See `wx.DataObject` documentation for the reasons why you might prefer
        to use wx.DataObject directly instead of wx.DataObjectComposite for
        efficiency reasons.

        """
        _misc_.DataObjectComposite_swiginit(self,_misc_.new_DataObjectComposite(*args, **kwargs))
    def Add(*args, **kwargs):
        """
        Add(self, DataObjectSimple dataObject, bool preferred=False)

        Adds the dataObject to the list of supported objects and it becomes
        the preferred object if preferred is True.
        """
        return _misc_.DataObjectComposite_Add(*args, **kwargs)

    def GetReceivedFormat(*args, **kwargs):
        """
        GetReceivedFormat(self) -> DataFormat

        Report the format passed to the `SetData` method.  This should be the
        format of the data object within the composite that recieved data from
        the clipboard or the DnD operation.  You can use this method to find
        out what kind of data object was recieved.
        """
        return _misc_.DataObjectComposite_GetReceivedFormat(*args, **kwargs)

    ReceivedFormat = property(GetReceivedFormat,doc="See `GetReceivedFormat`") 
_misc_.DataObjectComposite_swigregister(DataObjectComposite)

class TextDataObject(DataObjectSimple):
    """
    wx.TextDataObject is a specialization of `wx.DataObject` for text
    data. It can be used without change to paste data into the `wx.Clipboard`
    or a `wx.DropSource`.

    Alternativly, you may wish to derive a new class from the
    `wx.PyTextDataObject` class for providing text on-demand in order to
    minimize memory consumption when offering data in several formats,
    such as plain text and RTF, because by default the text is stored in a
    string in this class, but it might as well be generated on demand when
    requested. For this, `GetTextLength` and `GetText` will have to be
    overridden.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String text=EmptyString) -> TextDataObject

        Constructor, may be used to initialise the text (otherwise `SetText`
        should be used later).
        """
        _misc_.TextDataObject_swiginit(self,_misc_.new_TextDataObject(*args, **kwargs))
    def GetTextLength(*args, **kwargs):
        """
        GetTextLength(self) -> size_t

        Returns the data size.  By default, returns the size of the text data
        set in the constructor or using `SetText`.  This can be overridden (via
        `wx.PyTextDataObject`) to provide text size data on-demand. It is
        recommended to return the text length plus 1 for a trailing zero, but
        this is not strictly required.
        """
        return _misc_.TextDataObject_GetTextLength(*args, **kwargs)

    def GetText(*args, **kwargs):
        """
        GetText(self) -> String

        Returns the text associated with the data object.
        """
        return _misc_.TextDataObject_GetText(*args, **kwargs)

    def SetText(*args, **kwargs):
        """
        SetText(self, String text)

        Sets the text associated with the data object. This method is called
        when the data object receives the data and, by default, copies the
        text into the member variable. If you want to process the text on the
        fly you may wish to override this function (via
        `wx.PyTextDataObject`.)
        """
        return _misc_.TextDataObject_SetText(*args, **kwargs)

    Text = property(GetText,SetText,doc="See `GetText` and `SetText`") 
    TextLength = property(GetTextLength,doc="See `GetTextLength`") 
_misc_.TextDataObject_swigregister(TextDataObject)

class PyTextDataObject(TextDataObject):
    """
    wx.PyTextDataObject is a version of `wx.TextDataObject` that is
    Python-aware and knows how to reflect calls to its C++ virtual methods
    to methods in the Python derived class.  You should derive from this
    class and overload `GetTextLength`, `GetText`, and `SetText` when you
    want to be able to provide text on demand instead of preloading it
    into the data object.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String text=EmptyString) -> PyTextDataObject

        wx.PyTextDataObject is a version of `wx.TextDataObject` that is
        Python-aware and knows how to reflect calls to its C++ virtual methods
        to methods in the Python derived class.  You should derive from this
        class and overload `GetTextLength`, `GetText`, and `SetText` when you
        want to be able to provide text on demand instead of preloading it
        into the data object.
        """
        _misc_.PyTextDataObject_swiginit(self,_misc_.new_PyTextDataObject(*args, **kwargs))
        self._setCallbackInfo(self, PyTextDataObject)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.PyTextDataObject__setCallbackInfo(*args, **kwargs)

_misc_.PyTextDataObject_swigregister(PyTextDataObject)

class BitmapDataObject(DataObjectSimple):
    """
    wx.BitmapDataObject is a specialization of wxDataObject for bitmap
    data. It can be used without change to paste data into the `wx.Clipboard`
    or a `wx.DropSource`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Bitmap bitmap=wxNullBitmap) -> BitmapDataObject

        Constructor, optionally passing a bitmap (otherwise use `SetBitmap`
        later).
        """
        _misc_.BitmapDataObject_swiginit(self,_misc_.new_BitmapDataObject(*args, **kwargs))
    def GetBitmap(*args, **kwargs):
        """
        GetBitmap(self) -> Bitmap

        Returns the bitmap associated with the data object.  You may wish to
        override this method (by deriving from `wx.PyBitmapDataObject`) when
        offering data on-demand, but this is not required by wxWidgets'
        internals. Use this method to get data in bitmap form from the
        `wx.Clipboard`.
        """
        return _misc_.BitmapDataObject_GetBitmap(*args, **kwargs)

    def SetBitmap(*args, **kwargs):
        """
        SetBitmap(self, Bitmap bitmap)

        Sets the bitmap associated with the data object. This method is called
        when the data object receives data. Usually there will be no reason to
        override this function.
        """
        return _misc_.BitmapDataObject_SetBitmap(*args, **kwargs)

    Bitmap = property(GetBitmap,SetBitmap,doc="See `GetBitmap` and `SetBitmap`") 
_misc_.BitmapDataObject_swigregister(BitmapDataObject)

class PyBitmapDataObject(BitmapDataObject):
    """
    wx.PyBitmapDataObject is a version of `wx.BitmapDataObject` that is
    Python-aware and knows how to reflect calls to its C++ virtual methods
    to methods in the Python derived class. To be able to provide bitmap
    data on demand derive from this class and overload `GetBitmap`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Bitmap bitmap=wxNullBitmap) -> PyBitmapDataObject

        wx.PyBitmapDataObject is a version of `wx.BitmapDataObject` that is
        Python-aware and knows how to reflect calls to its C++ virtual methods
        to methods in the Python derived class. To be able to provide bitmap
        data on demand derive from this class and overload `GetBitmap`.
        """
        _misc_.PyBitmapDataObject_swiginit(self,_misc_.new_PyBitmapDataObject(*args, **kwargs))
        self._setCallbackInfo(self, PyBitmapDataObject)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.PyBitmapDataObject__setCallbackInfo(*args, **kwargs)

_misc_.PyBitmapDataObject_swigregister(PyBitmapDataObject)

class FileDataObject(DataObjectSimple):
    """
    wx.FileDataObject is a specialization of `wx.DataObjectSimple` for
    file names. The program works with it just as if it were a list of
    absolute file names, but internally it uses the same format as
    Explorer and other compatible programs under Windows or GNOME/KDE
    filemanager under Unix which makes it possible to receive files from
    them using this class.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FileDataObject"""
        _misc_.FileDataObject_swiginit(self,_misc_.new_FileDataObject(*args, **kwargs))
    def GetFilenames(*args, **kwargs):
        """
        GetFilenames(self) -> [names]

        Returns a list of file names.
        """
        return _misc_.FileDataObject_GetFilenames(*args, **kwargs)

    def AddFile(*args, **kwargs):
        """
        AddFile(self, String filename)

        Adds a file to the list of files represented by this data object.
        """
        return _misc_.FileDataObject_AddFile(*args, **kwargs)

    Filenames = property(GetFilenames,doc="See `GetFilenames`") 
_misc_.FileDataObject_swigregister(FileDataObject)

class CustomDataObject(DataObjectSimple):
    """
    wx.CustomDataObject is a specialization of `wx.DataObjectSimple` for
    some application-specific data in arbitrary format.  Python strings
    are used for getting and setting data, but any picklable object can
    easily be transfered via strings.  A copy of the data is stored in the
    data object.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, DataFormat format) -> CustomDataObject
        __init__(self, String formatName) -> CustomDataObject
        __init__(self) -> CustomDataObject

        wx.CustomDataObject is a specialization of `wx.DataObjectSimple` for
        some application-specific data in arbitrary format.  Python strings
        are used for getting and setting data, but any picklable object can
        easily be transfered via strings.  A copy of the data is stored in the
        data object.
        """
        _misc_.CustomDataObject_swiginit(self,_misc_.new_CustomDataObject(*args))
    def SetData(*args, **kwargs):
        """
        SetData(self, String data) -> bool

        Copy the data value to the data object.
        """
        return _misc_.CustomDataObject_SetData(*args, **kwargs)

    TakeData = SetData 
    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> size_t

        Get the size of the data.
        """
        return _misc_.CustomDataObject_GetSize(*args, **kwargs)

    def GetData(*args, **kwargs):
        """
        GetData(self) -> String

        Returns the data bytes from the data object as a string.
        """
        return _misc_.CustomDataObject_GetData(*args, **kwargs)

    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
    Size = property(GetSize,doc="See `GetSize`") 
_misc_.CustomDataObject_swigregister(CustomDataObject)

class URLDataObject(DataObject):
    """
    This data object holds a URL in a format that is compatible with some
    browsers such that it is able to be dragged to or from them.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String url=EmptyString) -> URLDataObject

        This data object holds a URL in a format that is compatible with some
        browsers such that it is able to be dragged to or from them.
        """
        _misc_.URLDataObject_swiginit(self,_misc_.new_URLDataObject(*args, **kwargs))
    def GetURL(*args, **kwargs):
        """
        GetURL(self) -> String

        Returns a string containing the current URL.
        """
        return _misc_.URLDataObject_GetURL(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """
        SetURL(self, String url)

        Set the URL.
        """
        return _misc_.URLDataObject_SetURL(*args, **kwargs)

    URL = property(GetURL,SetURL,doc="See `GetURL` and `SetURL`") 
_misc_.URLDataObject_swigregister(URLDataObject)

class MetafileDataObject(DataObjectSimple):
    """Proxy of C++ MetafileDataObject class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> MetafileDataObject"""
        _misc_.MetafileDataObject_swiginit(self,_misc_.new_MetafileDataObject(*args, **kwargs))
_misc_.MetafileDataObject_swigregister(MetafileDataObject)

#---------------------------------------------------------------------------

Drag_CopyOnly = _misc_.Drag_CopyOnly
Drag_AllowMove = _misc_.Drag_AllowMove
Drag_DefaultMove = _misc_.Drag_DefaultMove
DragError = _misc_.DragError
DragNone = _misc_.DragNone
DragCopy = _misc_.DragCopy
DragMove = _misc_.DragMove
DragLink = _misc_.DragLink
DragCancel = _misc_.DragCancel

def IsDragResultOk(*args, **kwargs):
  """IsDragResultOk(int res) -> bool"""
  return _misc_.IsDragResultOk(*args, **kwargs)
class DropSource(object):
    """Proxy of C++ DropSource class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win, Icon copy=wxNullIcon, Icon move=wxNullIcon, 
            Icon none=wxNullIcon) -> DropSource
        """
        _misc_.DropSource_swiginit(self,_misc_.new_DropSource(*args, **kwargs))
        self._setCallbackInfo(self, DropSource, 0)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, int incref)"""
        return _misc_.DropSource__setCallbackInfo(*args, **kwargs)

    __swig_destroy__ = _misc_.delete_DropSource
    __del__ = lambda self : None;
    def SetData(*args, **kwargs):
        """SetData(self, DataObject data)"""
        return _misc_.DropSource_SetData(*args, **kwargs)

    def GetDataObject(*args, **kwargs):
        """GetDataObject(self) -> DataObject"""
        return _misc_.DropSource_GetDataObject(*args, **kwargs)

    def SetCursor(*args, **kwargs):
        """SetCursor(self, int res, Cursor cursor)"""
        return _misc_.DropSource_SetCursor(*args, **kwargs)

    def DoDragDrop(*args, **kwargs):
        """DoDragDrop(self, int flags=Drag_CopyOnly) -> int"""
        return _misc_.DropSource_DoDragDrop(*args, **kwargs)

    def GiveFeedback(*args, **kwargs):
        """GiveFeedback(self, int effect) -> bool"""
        return _misc_.DropSource_GiveFeedback(*args, **kwargs)

    def base_GiveFeedback(*args, **kw):
        return DropSource.GiveFeedback(*args, **kw)
    base_GiveFeedback = wx._deprecated(base_GiveFeedback,
                                   "Please use DropSource.GiveFeedback instead.")

    DataObject = property(GetDataObject,SetData,doc="See `GetDataObject` and `SetData`") 
_misc_.DropSource_swigregister(DropSource)

def DROP_ICON(filename):
    """
    Returns either a `wx.Cursor` or `wx.Icon` created from the image file
    ``filename``.  This function is useful with the `wx.DropSource` class
    which, depending on platform accepts either a icon or a cursor.
    """
    img = wx.Image(filename)
    if wx.Platform == '__WXGTK__':
        return wx.IconFromBitmap(wx.BitmapFromImage(img))
    else:
        return wx.CursorFromImage(img)

class DropTarget(object):
    """Proxy of C++ DropTarget class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, DataObject dataObject=None) -> DropTarget"""
        _misc_.DropTarget_swiginit(self,_misc_.new_DropTarget(*args, **kwargs))
        self._setCallbackInfo(self, DropTarget)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.DropTarget__setCallbackInfo(*args, **kwargs)

    __swig_destroy__ = _misc_.delete_DropTarget
    __del__ = lambda self : None;
    def GetDataObject(*args, **kwargs):
        """GetDataObject(self) -> DataObject"""
        return _misc_.DropTarget_GetDataObject(*args, **kwargs)

    def SetDataObject(*args, **kwargs):
        """SetDataObject(self, DataObject dataObject)"""
        return _misc_.DropTarget_SetDataObject(*args, **kwargs)

    def OnEnter(*args, **kwargs):
        """OnEnter(self, int x, int y, int def) -> int"""
        return _misc_.DropTarget_OnEnter(*args, **kwargs)

    def OnDragOver(*args, **kwargs):
        """OnDragOver(self, int x, int y, int def) -> int"""
        return _misc_.DropTarget_OnDragOver(*args, **kwargs)

    def OnLeave(*args, **kwargs):
        """OnLeave(self)"""
        return _misc_.DropTarget_OnLeave(*args, **kwargs)

    def OnDrop(*args, **kwargs):
        """OnDrop(self, int x, int y) -> bool"""
        return _misc_.DropTarget_OnDrop(*args, **kwargs)

    def base_OnEnter(*args, **kw):
        return DropTarget.OnEnter(*args, **kw)
    base_OnEnter = wx._deprecated(base_OnEnter,
                                   "Please use DropTarget.OnEnter instead.")

    def base_OnDragOver(*args, **kw):
        return DropTarget.OnDragOver(*args, **kw)
    base_OnDragOver = wx._deprecated(base_OnDragOver,
                                   "Please use DropTarget.OnDragOver instead.")

    def base_OnLeave(*args, **kw):
        return DropTarget.OnLeave(*args, **kw)
    base_OnLeave = wx._deprecated(base_OnLeave,
                                   "Please use DropTarget.OnLeave instead.")

    def base_OnDrop(*args, **kw):
        return DropTarget.OnDrop(*args, **kw)
    base_OnDrop = wx._deprecated(base_OnDrop,
                                   "Please use DropTarget.OnDrop instead.")

    def GetData(*args, **kwargs):
        """GetData(self) -> bool"""
        return _misc_.DropTarget_GetData(*args, **kwargs)

    def SetDefaultAction(*args, **kwargs):
        """SetDefaultAction(self, int action)"""
        return _misc_.DropTarget_SetDefaultAction(*args, **kwargs)

    def GetDefaultAction(*args, **kwargs):
        """GetDefaultAction(self) -> int"""
        return _misc_.DropTarget_GetDefaultAction(*args, **kwargs)

    Data = property(GetData,doc="See `GetData`") 
    DataObject = property(GetDataObject,SetDataObject,doc="See `GetDataObject` and `SetDataObject`") 
    DefaultAction = property(GetDefaultAction,SetDefaultAction,doc="See `GetDefaultAction` and `SetDefaultAction`") 
_misc_.DropTarget_swigregister(DropTarget)

PyDropTarget = DropTarget 
class TextDropTarget(DropTarget):
    """Proxy of C++ TextDropTarget class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> TextDropTarget"""
        _misc_.TextDropTarget_swiginit(self,_misc_.new_TextDropTarget(*args, **kwargs))
        self._setCallbackInfo(self, TextDropTarget)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.TextDropTarget__setCallbackInfo(*args, **kwargs)

    def OnDropText(*args, **kwargs):
        """OnDropText(self, int x, int y, String text) -> bool"""
        return _misc_.TextDropTarget_OnDropText(*args, **kwargs)

    def OnEnter(*args, **kwargs):
        """OnEnter(self, int x, int y, int def) -> int"""
        return _misc_.TextDropTarget_OnEnter(*args, **kwargs)

    def OnDragOver(*args, **kwargs):
        """OnDragOver(self, int x, int y, int def) -> int"""
        return _misc_.TextDropTarget_OnDragOver(*args, **kwargs)

    def OnLeave(*args, **kwargs):
        """OnLeave(self)"""
        return _misc_.TextDropTarget_OnLeave(*args, **kwargs)

    def OnDrop(*args, **kwargs):
        """OnDrop(self, int x, int y) -> bool"""
        return _misc_.TextDropTarget_OnDrop(*args, **kwargs)

    def OnData(*args, **kwargs):
        """OnData(self, int x, int y, int def) -> int"""
        return _misc_.TextDropTarget_OnData(*args, **kwargs)

    def base_OnDropText(*args, **kw):
        return TextDropTarget.OnDropText(*args, **kw)
    base_OnDropText = wx._deprecated(base_OnDropText,
                                   "Please use TextDropTarget.OnDropText instead.")

    def base_OnEnter(*args, **kw):
        return TextDropTarget.OnEnter(*args, **kw)
    base_OnEnter = wx._deprecated(base_OnEnter,
                                   "Please use TextDropTarget.OnEnter instead.")

    def base_OnDragOver(*args, **kw):
        return TextDropTarget.OnDragOver(*args, **kw)
    base_OnDragOver = wx._deprecated(base_OnDragOver,
                                   "Please use TextDropTarget.OnDragOver instead.")

    def base_OnLeave(*args, **kw):
        return TextDropTarget.OnLeave(*args, **kw)
    base_OnLeave = wx._deprecated(base_OnLeave,
                                   "Please use TextDropTarget.OnLeave instead.")

    def base_OnDrop(*args, **kw):
        return TextDropTarget.OnDrop(*args, **kw)
    base_OnDrop = wx._deprecated(base_OnDrop,
                                   "Please use TextDropTarget.OnDrop instead.")

    def base_OnData(*args, **kw):
        return TextDropTarget.OnData(*args, **kw)
    base_OnData = wx._deprecated(base_OnData,
                                   "Please use TextDropTarget.OnData instead.")

_misc_.TextDropTarget_swigregister(TextDropTarget)

class FileDropTarget(DropTarget):
    """Proxy of C++ FileDropTarget class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FileDropTarget"""
        _misc_.FileDropTarget_swiginit(self,_misc_.new_FileDropTarget(*args, **kwargs))
        self._setCallbackInfo(self, FileDropTarget)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _misc_.FileDropTarget__setCallbackInfo(*args, **kwargs)

    def OnDropFiles(*args, **kwargs):
        """OnDropFiles(self, int x, int y, wxArrayString filenames) -> bool"""
        return _misc_.FileDropTarget_OnDropFiles(*args, **kwargs)

    def OnEnter(*args, **kwargs):
        """OnEnter(self, int x, int y, int def) -> int"""
        return _misc_.FileDropTarget_OnEnter(*args, **kwargs)

    def OnDragOver(*args, **kwargs):
        """OnDragOver(self, int x, int y, int def) -> int"""
        return _misc_.FileDropTarget_OnDragOver(*args, **kwargs)

    def OnLeave(*args, **kwargs):
        """OnLeave(self)"""
        return _misc_.FileDropTarget_OnLeave(*args, **kwargs)

    def OnDrop(*args, **kwargs):
        """OnDrop(self, int x, int y) -> bool"""
        return _misc_.FileDropTarget_OnDrop(*args, **kwargs)

    def OnData(*args, **kwargs):
        """OnData(self, int x, int y, int def) -> int"""
        return _misc_.FileDropTarget_OnData(*args, **kwargs)

    def base_OnDropFiles(*args, **kw):
        return FileDropTarget.OnDropFiles(*args, **kw)
    base_OnDropFiles = wx._deprecated(base_OnDropFiles,
                                   "Please use FileDropTarget.OnDropFiles instead.")

    def base_OnEnter(*args, **kw):
        return FileDropTarget.OnEnter(*args, **kw)
    base_OnEnter = wx._deprecated(base_OnEnter,
                                   "Please use FileDropTarget.OnEnter instead.")

    def base_OnDragOver(*args, **kw):
        return FileDropTarget.OnDragOver(*args, **kw)
    base_OnDragOver = wx._deprecated(base_OnDragOver,
                                   "Please use FileDropTarget.OnDragOver instead.")

    def base_OnLeave(*args, **kw):
        return FileDropTarget.OnLeave(*args, **kw)
    base_OnLeave = wx._deprecated(base_OnLeave,
                                   "Please use FileDropTarget.OnLeave instead.")

    def base_OnDrop(*args, **kw):
        return FileDropTarget.OnDrop(*args, **kw)
    base_OnDrop = wx._deprecated(base_OnDrop,
                                   "Please use FileDropTarget.OnDrop instead.")

    def base_OnData(*args, **kw):
        return FileDropTarget.OnData(*args, **kw)
    base_OnData = wx._deprecated(base_OnData,
                                   "Please use FileDropTarget.OnData instead.")

_misc_.FileDropTarget_swigregister(FileDropTarget)

#---------------------------------------------------------------------------

class Clipboard(_core.Object):
    """
    wx.Clipboard represents the system clipboard and provides methods to
    copy data to it or paste data from it.  Normally, you should only use
    ``wx.TheClipboard`` which is a reference to a global wx.Clipboard
    instance.

    Call ``wx.TheClipboard``'s `Open` method to get ownership of the
    clipboard. If this operation returns True, you now own the
    clipboard. Call `SetData` to put data on the clipboard, or `GetData`
    to retrieve data from the clipboard.  Call `Close` to close the
    clipboard and relinquish ownership. You should keep the clipboard open
    only momentarily.

    :see: `wx.DataObject`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> Clipboard"""
        _misc_.Clipboard_swiginit(self,_misc_.new_Clipboard(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Clipboard
    __del__ = lambda self : None;
    def Open(*args, **kwargs):
        """
        Open(self) -> bool

        Call this function to open the clipboard before calling SetData and
        GetData.  Call Close when you have finished with the clipboard.  You
        should keep the clipboard open for only a very short time.  Returns
        True on success.
        """
        return _misc_.Clipboard_Open(*args, **kwargs)

    def Close(*args, **kwargs):
        """
        Close(self)

        Closes the clipboard.
        """
        return _misc_.Clipboard_Close(*args, **kwargs)

    def IsOpened(*args, **kwargs):
        """
        IsOpened(self) -> bool

        Query whether the clipboard is opened
        """
        return _misc_.Clipboard_IsOpened(*args, **kwargs)

    def AddData(*args, **kwargs):
        """
        AddData(self, DataObject data) -> bool

        Call this function to add the data object to the clipboard. You may
        call this function repeatedly after having cleared the clipboard.
        After this function has been called, the clipboard owns the data, so
        do not delete the data explicitly.

        :see: `wx.DataObject`
        """
        return _misc_.Clipboard_AddData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """
        SetData(self, DataObject data) -> bool

        Set the clipboard data, this is the same as `Clear` followed by
        `AddData`.

        :see: `wx.DataObject`
        """
        return _misc_.Clipboard_SetData(*args, **kwargs)

    def IsSupported(*args, **kwargs):
        """
        IsSupported(self, DataFormat format) -> bool

        Returns True if the given format is available in the data object(s) on
        the clipboard.
        """
        return _misc_.Clipboard_IsSupported(*args, **kwargs)

    def GetData(*args, **kwargs):
        """
        GetData(self, DataObject data) -> bool

        Call this function to fill data with data on the clipboard, if
        available in the required format. Returns true on success.
        """
        return _misc_.Clipboard_GetData(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self)

        Clears data from the clipboard object and also the system's clipboard
        if possible.
        """
        return _misc_.Clipboard_Clear(*args, **kwargs)

    def Flush(*args, **kwargs):
        """
        Flush(self) -> bool

        Flushes the clipboard: this means that the data which is currently on
        clipboard will stay available even after the application exits,
        possibly eating memory, otherwise the clipboard will be emptied on
        exit.  Returns False if the operation is unsuccesful for any reason.
        """
        return _misc_.Clipboard_Flush(*args, **kwargs)

    def UsePrimarySelection(*args, **kwargs):
        """
        UsePrimarySelection(self, bool primary=True)

        On platforms supporting it (the X11 based platforms), selects the
        so called PRIMARY SELECTION as the clipboard as opposed to the
        normal clipboard, if primary is True.
        """
        return _misc_.Clipboard_UsePrimarySelection(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> Clipboard

        Returns global instance (wxTheClipboard) of the object.
        """
        return _misc_.Clipboard_Get(*args, **kwargs)

    Get = staticmethod(Get)
    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
_misc_.Clipboard_swigregister(Clipboard)

def Clipboard_Get(*args):
  """
    Clipboard_Get() -> Clipboard

    Returns global instance (wxTheClipboard) of the object.
    """
  return _misc_.Clipboard_Get(*args)

class _wxPyDelayedInitWrapper(object):
    def __init__(self, initfunc, *args, **kwargs):
        self._initfunc = initfunc
        self._args = args
        self._kwargs = kwargs
        self._instance = None
    def _checkInstance(self):
        if self._instance is None:
            if wx.GetApp():
                self._instance = self._initfunc(*self._args, **self._kwargs)        
    def __getattr__(self, name):
        self._checkInstance()
        return getattr(self._instance, name)
    def __repr__(self):
        self._checkInstance()
        return repr(self._instance)
TheClipboard = _wxPyDelayedInitWrapper(Clipboard.Get)

class ClipboardLocker(object):
    """
    A helpful class for opening the clipboard and automatically
    closing it when the locker is destroyed.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Clipboard clipboard=None) -> ClipboardLocker

        A helpful class for opening the clipboard and automatically
        closing it when the locker is destroyed.
        """
        _misc_.ClipboardLocker_swiginit(self,_misc_.new_ClipboardLocker(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_ClipboardLocker
    __del__ = lambda self : None;
    def __nonzero__(*args, **kwargs):
        """
        __nonzero__(self) -> bool

        A ClipboardLocker instance evaluates to True if the clipboard was
        successfully opened.
        """
        return _misc_.ClipboardLocker___nonzero__(*args, **kwargs)

_misc_.ClipboardLocker_swigregister(ClipboardLocker)

#---------------------------------------------------------------------------

class VideoMode(object):
    """A simple struct containing video mode parameters for a display"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int width=0, int height=0, int depth=0, int freq=0) -> VideoMode

        A simple struct containing video mode parameters for a display
        """
        _misc_.VideoMode_swiginit(self,_misc_.new_VideoMode(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_VideoMode
    __del__ = lambda self : None;
    def Matches(*args, **kwargs):
        """
        Matches(self, VideoMode other) -> bool

        Returns True if this mode matches the other one in the sense that all
        non-zero fields of the other mode have the same value in this
        one (except for refresh which is allowed to have a greater value)
        """
        return _misc_.VideoMode_Matches(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """
        GetWidth(self) -> int

        Returns the screen width in pixels (e.g. 640*480), 0 means unspecified
        """
        return _misc_.VideoMode_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """
        GetHeight(self) -> int

        Returns the screen height in pixels (e.g. 640*480), 0 means unspecified
        """
        return _misc_.VideoMode_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """
        GetDepth(self) -> int

        Returns the screen's bits per pixel (e.g. 32), 1 is monochrome and 0
        means unspecified/known
        """
        return _misc_.VideoMode_GetDepth(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """
        IsOk(self) -> bool

        returns true if the object has been initialized
        """
        return _misc_.VideoMode_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def __eq__(*args, **kwargs):
        """__eq__(self, VideoMode other) -> bool"""
        return _misc_.VideoMode___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, VideoMode other) -> bool"""
        return _misc_.VideoMode___ne__(*args, **kwargs)

    w = property(_misc_.VideoMode_w_get, _misc_.VideoMode_w_set)
    h = property(_misc_.VideoMode_h_get, _misc_.VideoMode_h_set)
    bpp = property(_misc_.VideoMode_bpp_get, _misc_.VideoMode_bpp_set)
    refresh = property(_misc_.VideoMode_refresh_get, _misc_.VideoMode_refresh_set)
    Depth = property(GetDepth,doc="See `GetDepth`") 
    Height = property(GetHeight,doc="See `GetHeight`") 
    Width = property(GetWidth,doc="See `GetWidth`") 
_misc_.VideoMode_swigregister(VideoMode)

class Display(object):
    """Represents a display/monitor attached to the system"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, unsigned int index=0) -> Display

        Set up a Display instance with the specified display.  The displays
        are numbered from 0 to GetCount() - 1, 0 is always the primary display
        and the only one which is always supported
        """
        _misc_.Display_swiginit(self,_misc_.new_Display(*args, **kwargs))
    __swig_destroy__ = _misc_.delete_Display
    __del__ = lambda self : None;
    def GetCount(*args, **kwargs):
        """
        GetCount() -> unsigned int

        Return the number of available displays.
        """
        return _misc_.Display_GetCount(*args, **kwargs)

    GetCount = staticmethod(GetCount)
    def GetFromPoint(*args, **kwargs):
        """
        GetFromPoint(Point pt) -> int

        Find the display where the given point lies, return wx.NOT_FOUND if it
        doesn't belong to any display
        """
        return _misc_.Display_GetFromPoint(*args, **kwargs)

    GetFromPoint = staticmethod(GetFromPoint)
    def GetFromWindow(*args, **kwargs):
        """
        GetFromWindow(Window window) -> int

        Find the display where the given window lies, return wx.NOT_FOUND if
        it is not shown at all.
        """
        return _misc_.Display_GetFromWindow(*args, **kwargs)

    GetFromWindow = staticmethod(GetFromWindow)
    def IsOk(*args, **kwargs):
        """
        IsOk(self) -> bool

        Return true if the object was initialized successfully
        """
        return _misc_.Display_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def GetGeometry(*args, **kwargs):
        """
        GetGeometry(self) -> Rect

        Returns the bounding rectangle of the display whose index was passed
        to the constructor.
        """
        return _misc_.Display_GetGeometry(*args, **kwargs)

    def GetClientArea(*args, **kwargs):
        """
        GetClientArea(self) -> Rect

        Returns the bounding rectangle the client area of the display,
        i.e., without taskbars and such.
        """
        return _misc_.Display_GetClientArea(*args, **kwargs)

    def GetName(*args, **kwargs):
        """
        GetName(self) -> String

        Returns the display's name. A name is not available on all platforms.
        """
        return _misc_.Display_GetName(*args, **kwargs)

    def IsPrimary(*args, **kwargs):
        """
        IsPrimary(self) -> bool

        Returns True if the display is the primary display. The primary
        display is the one whose index is 0.
        """
        return _misc_.Display_IsPrimary(*args, **kwargs)

    def GetModes(*args, **kwargs):
        """
        GetModes(VideoMode mode=DefaultVideoMode) -> [videoMode...]

        Enumerate all video modes supported by this display matching the given
        one (in the sense of VideoMode.Match()).

        As any mode matches the default value of the argument and there is
        always at least one video mode supported by display, the returned
        array is only empty for the default value of the argument if this
        function is not supported at all on this platform.
        """
        return _misc_.Display_GetModes(*args, **kwargs)

    def GetCurrentMode(*args, **kwargs):
        """
        GetCurrentMode(self) -> VideoMode

        Get the current video mode.
        """
        return _misc_.Display_GetCurrentMode(*args, **kwargs)

    def ChangeMode(*args, **kwargs):
        """
        ChangeMode(self, VideoMode mode=DefaultVideoMode) -> bool

        Changes the video mode of this display to the mode specified in the
        mode parameter.

        If wx.DefaultVideoMode is passed in as the mode parameter, the defined
        behaviour is that wx.Display will reset the video mode to the default
        mode used by the display.  On Windows, the behavior is normal.
        However, there are differences on other platforms. On Unix variations
        using X11 extensions it should behave as defined, but some
        irregularities may occur.

        On wxMac passing in wx.DefaultVideoMode as the mode parameter does
        nothing.  This happens because Carbon no longer has access to
        DMUseScreenPrefs, an undocumented function that changed the video mode
        to the system default by using the system's 'scrn' resource.

        Returns True if succeeded, False otherwise
        """
        return _misc_.Display_ChangeMode(*args, **kwargs)

    def ResetMode(*args, **kwargs):
        """
        ResetMode(self)

        Restore the default video mode (just a more readable synonym)
        """
        return _misc_.Display_ResetMode(*args, **kwargs)

    ClientArea = property(GetClientArea,doc="See `GetClientArea`") 
    CurrentMode = property(GetCurrentMode,doc="See `GetCurrentMode`") 
    Geometry = property(GetGeometry,doc="See `GetGeometry`") 
    Modes = property(GetModes,doc="See `GetModes`") 
    Name = property(GetName,doc="See `GetName`") 
_misc_.Display_swigregister(Display)
DefaultVideoMode = cvar.DefaultVideoMode

def Display_GetCount(*args):
  """
    Display_GetCount() -> unsigned int

    Return the number of available displays.
    """
  return _misc_.Display_GetCount(*args)

def Display_GetFromPoint(*args, **kwargs):
  """
    Display_GetFromPoint(Point pt) -> int

    Find the display where the given point lies, return wx.NOT_FOUND if it
    doesn't belong to any display
    """
  return _misc_.Display_GetFromPoint(*args, **kwargs)

def Display_GetFromWindow(*args, **kwargs):
  """
    Display_GetFromWindow(Window window) -> int

    Find the display where the given window lies, return wx.NOT_FOUND if
    it is not shown at all.
    """
  return _misc_.Display_GetFromWindow(*args, **kwargs)

#---------------------------------------------------------------------------

class StandardPaths(object):
    """
    wx.StandardPaths returns standard locations in the file system and
    should be used by programs to find their data files in a portable way.

    In the description of the methods below, the example return values are
    given for the Unix, Windows and Mac OS X systems, however please note
    that these are just  examples and the actual values may differ. For
    example, under Windows the system administrator may change the
    standard directories locations, i.e. the Windows directory may be
    named W:\Win2003 instead of the default C:\Windows.

    The strings appname and username should be replaced with the value
    returned by `wx.App.GetAppName` and the name of the currently logged
    in user, respectively. The string prefix is only used under Unix and
    is /usr/local by default but may be changed using `SetInstallPrefix`.

    The directories returned by the methods of this class may or may not
    exist. If they don't exist, it's up to the caller to create them,
    wx.StandardPaths doesn't do it.

    Finally note that these functions only work with standardly packaged
    applications. I.e. under Unix you should follow the standard
    installation conventions and under Mac you should create your
    application bundle according to the Apple guidelines. Again, this
    class doesn't help you to do it.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    ResourceCat_None = _misc_.StandardPaths_ResourceCat_None
    ResourceCat_Messages = _misc_.StandardPaths_ResourceCat_Messages
    ResourceCat_Max = _misc_.StandardPaths_ResourceCat_Max
    def Get(*args, **kwargs):
        """
        Get() -> StandardPaths

        Return the global standard paths singleton
        """
        return _misc_.StandardPaths_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def GetConfigDir(*args, **kwargs):
        """
        GetConfigDir(self) -> String

        Return the directory with system config files: /etc under Unix,
        'c:\Documents and Settings\All Users\Application Data' under Windows,
        /Library/Preferences for Mac
        """
        return _misc_.StandardPaths_GetConfigDir(*args, **kwargs)

    def GetUserConfigDir(*args, **kwargs):
        """
        GetUserConfigDir(self) -> String

        Return the directory for the user config files: $HOME under Unix,
        'c:\Documents and Settings\username' under Windows, and 
        ~/Library/Preferences under Mac
            
        Only use this if you have a single file to put there, otherwise
        `GetUserDataDir` is more appropriate
        """
        return _misc_.StandardPaths_GetUserConfigDir(*args, **kwargs)

    def GetDataDir(*args, **kwargs):
        """
        GetDataDir(self) -> String

        Return the location of the application's global, (i.e. not
        user-specific,) data files: prefix/share/appname under Unix,
        'c:\Program Files\appname' under Windows,
        appname.app/Contents/SharedSupport app bundle directory under Mac.
        """
        return _misc_.StandardPaths_GetDataDir(*args, **kwargs)

    def GetLocalDataDir(*args, **kwargs):
        """
        GetLocalDataDir(self) -> String

        Return the location for application data files which are
        host-specific.  Same as `GetDataDir` except under Unix where it is
        /etc/appname
        """
        return _misc_.StandardPaths_GetLocalDataDir(*args, **kwargs)

    def GetUserDataDir(*args, **kwargs):
        """
        GetUserDataDir(self) -> String

        Return the directory for the user-dependent application data files:
        $HOME/.appname under Unix, c:\Documents and
        Settings\username\Application Data\appname under Windows and
        ~/Library/Application Support/appname under Mac
        """
        return _misc_.StandardPaths_GetUserDataDir(*args, **kwargs)

    def GetUserLocalDataDir(*args, **kwargs):
        """
        GetUserLocalDataDir(self) -> String

        Return the directory for user data files which shouldn't be shared
        with the other machines

        Same as `GetUserDataDir` for all platforms except Windows where it is
        the 'Local Settings\Application Data\appname' directory.
        """
        return _misc_.StandardPaths_GetUserLocalDataDir(*args, **kwargs)

    def GetPluginsDir(*args, **kwargs):
        """
        GetPluginsDir(self) -> String

        Return the directory where the loadable modules (plugins) live:
        prefix/lib/appname under Unix, program directory under Windows and
        Contents/Plugins app bundle subdirectory under Mac
        """
        return _misc_.StandardPaths_GetPluginsDir(*args, **kwargs)

    def GetResourcesDir(*args, **kwargs):
        """
        GetResourcesDir(self) -> String

        Get resources directory.  Resources are auxiliary files used by the
        application and include things like image and sound files.

        Same as `GetDataDir` for all platforms except Mac where it returns
        Contents/Resources subdirectory of the app bundle.
        """
        return _misc_.StandardPaths_GetResourcesDir(*args, **kwargs)

    def GetLocalizedResourcesDir(*args, **kwargs):
        """
        GetLocalizedResourcesDir(self, String lang, int category=ResourceCat_None) -> String

        Get localized resources directory containing the resource files of the
        specified category for the given language.

        In general this is just GetResourcesDir()/lang under Windows and Unix
        and GetResourcesDir()/lang.lproj under Mac but is something quite
        different under Unix for the message catalog category (namely the
        standard prefix/share/locale/lang/LC_MESSAGES.)
        """
        return _misc_.StandardPaths_GetLocalizedResourcesDir(*args, **kwargs)

    def GetDocumentsDir(*args, **kwargs):
        """
        GetDocumentsDir(self) -> String

        Return the Documents directory for the current user.

        C:\Documents and Settings\username\Documents under Windows,
        $HOME under Unix and ~/Documents under Mac
        """
        return _misc_.StandardPaths_GetDocumentsDir(*args, **kwargs)

    def SetInstallPrefix(*args, **kwargs):
        """
        SetInstallPrefix(self, String prefix)

        Set the program installation directory which is /usr/local by default.
        This value will be used by other methods such as `GetDataDir` and
        `GetPluginsDir` as the prefix for what they return. (This function
        only has meaning on Unix systems.)
        """
        return _misc_.StandardPaths_SetInstallPrefix(*args, **kwargs)

    def GetInstallPrefix(*args, **kwargs):
        """
        GetInstallPrefix(self) -> String

        Get the program installation prefix. The default is the prefix where
        Python is installed. (This function only has meaning on Unix systems.)
        """
        return _misc_.StandardPaths_GetInstallPrefix(*args, **kwargs)

_misc_.StandardPaths_swigregister(StandardPaths)

def StandardPaths_Get(*args):
  """
    StandardPaths_Get() -> StandardPaths

    Return the global standard paths singleton
    """
  return _misc_.StandardPaths_Get(*args)

#---------------------------------------------------------------------------

POWER_SOCKET = _misc_.POWER_SOCKET
POWER_BATTERY = _misc_.POWER_BATTERY
POWER_UNKNOWN = _misc_.POWER_UNKNOWN
BATTERY_NORMAL_STATE = _misc_.BATTERY_NORMAL_STATE
BATTERY_LOW_STATE = _misc_.BATTERY_LOW_STATE
BATTERY_CRITICAL_STATE = _misc_.BATTERY_CRITICAL_STATE
BATTERY_SHUTDOWN_STATE = _misc_.BATTERY_SHUTDOWN_STATE
BATTERY_UNKNOWN_STATE = _misc_.BATTERY_UNKNOWN_STATE
class PowerEvent(_core.Event):
    """
    wx.PowerEvent is generated when the system online status changes.
    Currently this is only implemented for Windows.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType evtType) -> PowerEvent

        wx.PowerEvent is generated when the system online status changes.
        Currently this is only implemented for Windows.
        """
        _misc_.PowerEvent_swiginit(self,_misc_.new_PowerEvent(*args, **kwargs))
    def Veto(*args, **kwargs):
        """Veto(self)"""
        return _misc_.PowerEvent_Veto(*args, **kwargs)

    def IsVetoed(*args, **kwargs):
        """IsVetoed(self) -> bool"""
        return _misc_.PowerEvent_IsVetoed(*args, **kwargs)

_misc_.PowerEvent_swigregister(PowerEvent)

wxEVT_POWER_SUSPENDING = _misc_.wxEVT_POWER_SUSPENDING
wxEVT_POWER_SUSPENDED = _misc_.wxEVT_POWER_SUSPENDED
wxEVT_POWER_SUSPEND_CANCEL = _misc_.wxEVT_POWER_SUSPEND_CANCEL
wxEVT_POWER_RESUME = _misc_.wxEVT_POWER_RESUME
EVT_POWER_SUSPENDING       = wx.PyEventBinder( wxEVT_POWER_SUSPENDING , 1 )
EVT_POWER_SUSPENDED        = wx.PyEventBinder( wxEVT_POWER_SUSPENDED , 1 )
EVT_POWER_SUSPEND_CANCEL   = wx.PyEventBinder( wxEVT_POWER_SUSPEND_CANCEL , 1 )
EVT_POWER_RESUME           = wx.PyEventBinder( wxEVT_POWER_RESUME , 1 )


def GetPowerType(*args):
  """
    GetPowerType() -> int

    return the current system power state: online or offline
    """
  return _misc_.GetPowerType(*args)

def GetBatteryState(*args):
  """
    GetBatteryState() -> int

    return approximate battery state
    """
  return _misc_.GetBatteryState(*args)


