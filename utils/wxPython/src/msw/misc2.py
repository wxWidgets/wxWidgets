# This file was created automatically by SWIG.
import misc2c

from windows import *

from misc import *

from gdi import *

from clip_dnd import *

from events import *
class wxToolTipPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_SetTip,(self,) + _args, _kwargs)
        return val
    def GetTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_GetTip,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxToolTip instance at %s>" % (self.this,)
class wxToolTip(wxToolTipPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxToolTip,_args,_kwargs)
        self.thisown = 1




class wxCaretPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,misc2c=misc2c):
        if self.thisown == 1 :
            misc2c.delete_wxCaret(self)
    def IsOk(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_IsOk,(self,) + _args, _kwargs)
        return val
    def IsVisible(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_IsVisible,(self,) + _args, _kwargs)
        return val
    def GetPositionTuple(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetPositionTuple,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetSizeTuple(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetSizeTuple,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def MoveXY(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_MoveXY,(self,) + _args, _kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Move,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Show,(self,) + _args, _kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Hide,(self,) + _args, _kwargs)
        return val
    def OnSetFocus(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_OnSetFocus,(self,) + _args, _kwargs)
        return val
    def OnKillFocus(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_OnKillFocus,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCaret instance at %s>" % (self.this,)
class wxCaret(wxCaretPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxCaret,_args,_kwargs)
        self.thisown = 1




class wxFontEnumeratorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,misc2c=misc2c):
        if self.thisown == 1 :
            misc2c.delete_wxFontEnumerator(self)
    def _setSelf(self, *_args, **_kwargs):
        val = apply(misc2c.wxFontEnumerator__setSelf,(self,) + _args, _kwargs)
        return val
    def EnumerateFacenames(self, *_args, **_kwargs):
        val = apply(misc2c.wxFontEnumerator_EnumerateFacenames,(self,) + _args, _kwargs)
        return val
    def EnumerateEncodings(self, *_args, **_kwargs):
        val = apply(misc2c.wxFontEnumerator_EnumerateEncodings,(self,) + _args, _kwargs)
        return val
    def GetEncodings(self, *_args, **_kwargs):
        val = apply(misc2c.wxFontEnumerator_GetEncodings,(self,) + _args, _kwargs)
        return val
    def GetFacenames(self, *_args, **_kwargs):
        val = apply(misc2c.wxFontEnumerator_GetFacenames,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontEnumerator instance at %s>" % (self.this,)
class wxFontEnumerator(wxFontEnumeratorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxFontEnumerator,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class wxBusyCursorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,misc2c=misc2c):
        if self.thisown == 1 :
            misc2c.delete_wxBusyCursor(self)
    def __repr__(self):
        return "<C wxBusyCursor instance at %s>" % (self.this,)
class wxBusyCursor(wxBusyCursorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxBusyCursor,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

wxFileSelector = misc2c.wxFileSelector

wxGetTextFromUser = misc2c.wxGetTextFromUser

wxGetSingleChoice = misc2c.wxGetSingleChoice

wxGetSingleChoiceIndex = misc2c.wxGetSingleChoiceIndex

wxMessageBox = misc2c.wxMessageBox

wxGetNumberFromUser = misc2c.wxGetNumberFromUser

wxColourDisplay = misc2c.wxColourDisplay

wxDisplayDepth = misc2c.wxDisplayDepth

wxSetCursor = misc2c.wxSetCursor

def wxFindWindowByLabel(*_args, **_kwargs):
    val = apply(misc2c.wxFindWindowByLabel,_args,_kwargs)
    if val: val = wxWindowPtr(val)
    return val

def wxFindWindowByName(*_args, **_kwargs):
    val = apply(misc2c.wxFindWindowByName,_args,_kwargs)
    if val: val = wxWindowPtr(val)
    return val

wxBeginBusyCursor = misc2c.wxBeginBusyCursor

def wxGetActiveWindow(*_args, **_kwargs):
    val = apply(misc2c.wxGetActiveWindow,_args,_kwargs)
    if val: val = wxWindowPtr(val)
    return val

wxResourceAddIdentifier = misc2c.wxResourceAddIdentifier

wxResourceClear = misc2c.wxResourceClear

def wxResourceCreateBitmap(*_args, **_kwargs):
    val = apply(misc2c.wxResourceCreateBitmap,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxResourceCreateIcon(*_args, **_kwargs):
    val = apply(misc2c.wxResourceCreateIcon,_args,_kwargs)
    if val: val = wxIconPtr(val); val.thisown = 1
    return val

def wxResourceCreateMenuBar(*_args, **_kwargs):
    val = apply(misc2c.wxResourceCreateMenuBar,_args,_kwargs)
    if val: val = wxMenuBarPtr(val)
    return val

wxResourceGetIdentifier = misc2c.wxResourceGetIdentifier

wxResourceParseData = misc2c.wxResourceParseData

wxResourceParseFile = misc2c.wxResourceParseFile

wxResourceParseString = misc2c.wxResourceParseString

def wxSystemSettings_GetSystemColour(*_args, **_kwargs):
    val = apply(misc2c.wxSystemSettings_GetSystemColour,_args,_kwargs)
    if val: val = wxColourPtr(val); val.thisown = 1
    return val

def wxSystemSettings_GetSystemFont(*_args, **_kwargs):
    val = apply(misc2c.wxSystemSettings_GetSystemFont,_args,_kwargs)
    if val: val = wxFontPtr(val); val.thisown = 1
    return val

wxSystemSettings_GetSystemMetric = misc2c.wxSystemSettings_GetSystemMetric

wxToolTip_Enable = misc2c.wxToolTip_Enable

wxToolTip_SetDelay = misc2c.wxToolTip_SetDelay

wxCaret_GetBlinkTime = misc2c.wxCaret_GetBlinkTime

wxCaret_SetBlinkTime = misc2c.wxCaret_SetBlinkTime

wxPostEvent = misc2c.wxPostEvent

wxWakeUpIdle = misc2c.wxWakeUpIdle



#-------------- VARIABLE WRAPPERS ------------------

wxSYS_WHITE_BRUSH = misc2c.wxSYS_WHITE_BRUSH
wxSYS_LTGRAY_BRUSH = misc2c.wxSYS_LTGRAY_BRUSH
wxSYS_GRAY_BRUSH = misc2c.wxSYS_GRAY_BRUSH
wxSYS_DKGRAY_BRUSH = misc2c.wxSYS_DKGRAY_BRUSH
wxSYS_BLACK_BRUSH = misc2c.wxSYS_BLACK_BRUSH
wxSYS_NULL_BRUSH = misc2c.wxSYS_NULL_BRUSH
wxSYS_HOLLOW_BRUSH = misc2c.wxSYS_HOLLOW_BRUSH
wxSYS_WHITE_PEN = misc2c.wxSYS_WHITE_PEN
wxSYS_BLACK_PEN = misc2c.wxSYS_BLACK_PEN
wxSYS_NULL_PEN = misc2c.wxSYS_NULL_PEN
wxSYS_OEM_FIXED_FONT = misc2c.wxSYS_OEM_FIXED_FONT
wxSYS_ANSI_FIXED_FONT = misc2c.wxSYS_ANSI_FIXED_FONT
wxSYS_ANSI_VAR_FONT = misc2c.wxSYS_ANSI_VAR_FONT
wxSYS_SYSTEM_FONT = misc2c.wxSYS_SYSTEM_FONT
wxSYS_DEVICE_DEFAULT_FONT = misc2c.wxSYS_DEVICE_DEFAULT_FONT
wxSYS_DEFAULT_PALETTE = misc2c.wxSYS_DEFAULT_PALETTE
wxSYS_SYSTEM_FIXED_FONT = misc2c.wxSYS_SYSTEM_FIXED_FONT
wxSYS_DEFAULT_GUI_FONT = misc2c.wxSYS_DEFAULT_GUI_FONT
wxSYS_COLOUR_SCROLLBAR = misc2c.wxSYS_COLOUR_SCROLLBAR
wxSYS_COLOUR_BACKGROUND = misc2c.wxSYS_COLOUR_BACKGROUND
wxSYS_COLOUR_ACTIVECAPTION = misc2c.wxSYS_COLOUR_ACTIVECAPTION
wxSYS_COLOUR_INACTIVECAPTION = misc2c.wxSYS_COLOUR_INACTIVECAPTION
wxSYS_COLOUR_MENU = misc2c.wxSYS_COLOUR_MENU
wxSYS_COLOUR_WINDOW = misc2c.wxSYS_COLOUR_WINDOW
wxSYS_COLOUR_WINDOWFRAME = misc2c.wxSYS_COLOUR_WINDOWFRAME
wxSYS_COLOUR_MENUTEXT = misc2c.wxSYS_COLOUR_MENUTEXT
wxSYS_COLOUR_WINDOWTEXT = misc2c.wxSYS_COLOUR_WINDOWTEXT
wxSYS_COLOUR_CAPTIONTEXT = misc2c.wxSYS_COLOUR_CAPTIONTEXT
wxSYS_COLOUR_ACTIVEBORDER = misc2c.wxSYS_COLOUR_ACTIVEBORDER
wxSYS_COLOUR_INACTIVEBORDER = misc2c.wxSYS_COLOUR_INACTIVEBORDER
wxSYS_COLOUR_APPWORKSPACE = misc2c.wxSYS_COLOUR_APPWORKSPACE
wxSYS_COLOUR_HIGHLIGHT = misc2c.wxSYS_COLOUR_HIGHLIGHT
wxSYS_COLOUR_HIGHLIGHTTEXT = misc2c.wxSYS_COLOUR_HIGHLIGHTTEXT
wxSYS_COLOUR_BTNFACE = misc2c.wxSYS_COLOUR_BTNFACE
wxSYS_COLOUR_BTNSHADOW = misc2c.wxSYS_COLOUR_BTNSHADOW
wxSYS_COLOUR_GRAYTEXT = misc2c.wxSYS_COLOUR_GRAYTEXT
wxSYS_COLOUR_BTNTEXT = misc2c.wxSYS_COLOUR_BTNTEXT
wxSYS_COLOUR_INACTIVECAPTIONTEXT = misc2c.wxSYS_COLOUR_INACTIVECAPTIONTEXT
wxSYS_COLOUR_BTNHIGHLIGHT = misc2c.wxSYS_COLOUR_BTNHIGHLIGHT
wxSYS_COLOUR_3DDKSHADOW = misc2c.wxSYS_COLOUR_3DDKSHADOW
wxSYS_COLOUR_3DLIGHT = misc2c.wxSYS_COLOUR_3DLIGHT
wxSYS_COLOUR_INFOTEXT = misc2c.wxSYS_COLOUR_INFOTEXT
wxSYS_COLOUR_INFOBK = misc2c.wxSYS_COLOUR_INFOBK
wxSYS_COLOUR_DESKTOP = misc2c.wxSYS_COLOUR_DESKTOP
wxSYS_COLOUR_3DFACE = misc2c.wxSYS_COLOUR_3DFACE
wxSYS_COLOUR_3DSHADOW = misc2c.wxSYS_COLOUR_3DSHADOW
wxSYS_COLOUR_3DHIGHLIGHT = misc2c.wxSYS_COLOUR_3DHIGHLIGHT
wxSYS_COLOUR_3DHILIGHT = misc2c.wxSYS_COLOUR_3DHILIGHT
wxSYS_COLOUR_BTNHILIGHT = misc2c.wxSYS_COLOUR_BTNHILIGHT
wxSYS_MOUSE_BUTTONS = misc2c.wxSYS_MOUSE_BUTTONS
wxSYS_BORDER_X = misc2c.wxSYS_BORDER_X
wxSYS_BORDER_Y = misc2c.wxSYS_BORDER_Y
wxSYS_CURSOR_X = misc2c.wxSYS_CURSOR_X
wxSYS_CURSOR_Y = misc2c.wxSYS_CURSOR_Y
wxSYS_DCLICK_X = misc2c.wxSYS_DCLICK_X
wxSYS_DCLICK_Y = misc2c.wxSYS_DCLICK_Y
wxSYS_DRAG_X = misc2c.wxSYS_DRAG_X
wxSYS_DRAG_Y = misc2c.wxSYS_DRAG_Y
wxSYS_EDGE_X = misc2c.wxSYS_EDGE_X
wxSYS_EDGE_Y = misc2c.wxSYS_EDGE_Y
wxSYS_HSCROLL_ARROW_X = misc2c.wxSYS_HSCROLL_ARROW_X
wxSYS_HSCROLL_ARROW_Y = misc2c.wxSYS_HSCROLL_ARROW_Y
wxSYS_HTHUMB_X = misc2c.wxSYS_HTHUMB_X
wxSYS_ICON_X = misc2c.wxSYS_ICON_X
wxSYS_ICON_Y = misc2c.wxSYS_ICON_Y
wxSYS_ICONSPACING_X = misc2c.wxSYS_ICONSPACING_X
wxSYS_ICONSPACING_Y = misc2c.wxSYS_ICONSPACING_Y
wxSYS_WINDOWMIN_X = misc2c.wxSYS_WINDOWMIN_X
wxSYS_WINDOWMIN_Y = misc2c.wxSYS_WINDOWMIN_Y
wxSYS_SCREEN_X = misc2c.wxSYS_SCREEN_X
wxSYS_SCREEN_Y = misc2c.wxSYS_SCREEN_Y
wxSYS_FRAMESIZE_X = misc2c.wxSYS_FRAMESIZE_X
wxSYS_FRAMESIZE_Y = misc2c.wxSYS_FRAMESIZE_Y
wxSYS_SMALLICON_X = misc2c.wxSYS_SMALLICON_X
wxSYS_SMALLICON_Y = misc2c.wxSYS_SMALLICON_Y
wxSYS_HSCROLL_Y = misc2c.wxSYS_HSCROLL_Y
wxSYS_VSCROLL_X = misc2c.wxSYS_VSCROLL_X
wxSYS_VSCROLL_ARROW_X = misc2c.wxSYS_VSCROLL_ARROW_X
wxSYS_VSCROLL_ARROW_Y = misc2c.wxSYS_VSCROLL_ARROW_Y
wxSYS_VTHUMB_Y = misc2c.wxSYS_VTHUMB_Y
wxSYS_CAPTION_Y = misc2c.wxSYS_CAPTION_Y
wxSYS_MENU_Y = misc2c.wxSYS_MENU_Y
wxSYS_NETWORK_PRESENT = misc2c.wxSYS_NETWORK_PRESENT
wxSYS_PENWINDOWS_PRESENT = misc2c.wxSYS_PENWINDOWS_PRESENT
wxSYS_SHOW_SOUNDS = misc2c.wxSYS_SHOW_SOUNDS
wxSYS_SWAP_BUTTONS = misc2c.wxSYS_SWAP_BUTTONS
