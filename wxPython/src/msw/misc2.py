# This file was created automatically by SWIG.
import misc2c

from windows import *

from misc import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

# Art clients
wxART_TOOLBAR              = 'wxART_TOOLBAR_C'
wxART_MENU                 = 'wxART_MENU_C'
wxART_FRAME_ICON           = 'wxART_FRAME_ICON_C'
wxART_CMN_DIALOG           = 'wxART_CMN_DIALOG_C'
wxART_HELP_BROWSER         = 'wxART_HELP_BROWSER_C'
wxART_MESSAGE_BOX          = 'wxART_MESSAGE_BOX_C'
wxART_OTHER                = 'wxART_OTHER_C'

# Art IDs
wxART_ADD_BOOKMARK         = 'wxART_ADD_BOOKMARK'
wxART_DEL_BOOKMARK         = 'wxART_DEL_BOOKMARK'
wxART_HELP_SIDE_PANEL      = 'wxART_HELP_SIDE_PANEL'
wxART_HELP_SETTINGS        = 'wxART_HELP_SETTINGS'
wxART_HELP_BOOK            = 'wxART_HELP_BOOK'
wxART_HELP_FOLDER          = 'wxART_HELP_FOLDER'
wxART_HELP_PAGE            = 'wxART_HELP_PAGE'
wxART_GO_BACK              = 'wxART_GO_BACK'
wxART_GO_FORWARD           = 'wxART_GO_FORWARD'
wxART_GO_UP                = 'wxART_GO_UP'
wxART_GO_DOWN              = 'wxART_GO_DOWN'
wxART_GO_TO_PARENT         = 'wxART_GO_TO_PARENT'
wxART_GO_HOME              = 'wxART_GO_HOME'
wxART_FILE_OPEN            = 'wxART_FILE_OPEN'
wxART_PRINT                = 'wxART_PRINT'
wxART_HELP                 = 'wxART_HELP'
wxART_TIP                  = 'wxART_TIP'
wxART_REPORT_VIEW          = 'wxART_REPORT_VIEW'
wxART_LIST_VIEW            = 'wxART_LIST_VIEW'
wxART_NEW_DIR              = 'wxART_NEW_DIR'
wxART_FOLDER               = 'wxART_FOLDER'
wxART_GO_DIR_UP            = 'wxART_GO_DIR_UP'
wxART_EXECUTABLE_FILE      = 'wxART_EXECUTABLE_FILE'
wxART_NORMAL_FILE          = 'wxART_NORMAL_FILE'
wxART_TICK_MARK            = 'wxART_TICK_MARK'
wxART_CROSS_MARK           = 'wxART_CROSS_MARK'
wxART_ERROR                = 'wxART_ERROR'
wxART_QUESTION             = 'wxART_QUESTION'
wxART_WARNING              = 'wxART_WARNING'
wxART_INFORMATION          = 'wxART_INFORMATION'
wxART_MISSING_IMAGE        = 'wxART_MISSING_IMAGE'

class wxSystemSettingsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSystemSettings instance at %s>" % (self.this,)
class wxSystemSettings(wxSystemSettingsPtr):
    def __init__(self,this):
        self.this = this




class wxToolTipPtr(wxObjectPtr):
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
    def __del__(self, delfunc=misc2c.delete_wxCaret):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
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
        return val
    def MoveXY(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_MoveXY,(self,) + _args, _kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Move,(self,) + _args, _kwargs)
        return val
    def SetSizeWH(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_SetSizeWH,(self,) + _args, _kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_SetSize,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Show,(self,) + _args, _kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Hide,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCaret instance at %s>" % (self.this,)
class wxCaret(wxCaretPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxCaret,_args,_kwargs)
        self.thisown = 1




class wxBusyCursorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxBusyCursor):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxBusyCursor instance at %s>" % (self.this,)
class wxBusyCursor(wxBusyCursorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxBusyCursor,_args,_kwargs)
        self.thisown = 1




class wxWindowDisablerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxWindowDisabler):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxWindowDisabler instance at %s>" % (self.this,)
class wxWindowDisabler(wxWindowDisablerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxWindowDisabler,_args,_kwargs)
        self.thisown = 1




class wxMutexGuiLockerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxMutexGuiLocker):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxMutexGuiLocker instance at %s>" % (self.this,)
class wxMutexGuiLocker(wxMutexGuiLockerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxMutexGuiLocker,_args,_kwargs)
        self.thisown = 1




class wxTipProviderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxTipProvider):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxTipProvider_GetTip,(self,) + _args, _kwargs)
        return val
    def GetCurrentTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxTipProvider_GetCurrentTip,(self,) + _args, _kwargs)
        return val
    def PreprocessTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxTipProvider_PreprocessTip,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTipProvider instance at %s>" % (self.this,)
class wxTipProvider(wxTipProviderPtr):
    def __init__(self,this):
        self.this = this




class wxPyTipProviderPtr(wxTipProviderPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTipProvider__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyTipProvider instance at %s>" % (self.this,)
class wxPyTipProvider(wxPyTipProviderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxPyTipProvider,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyTipProvider)




class wxDragImagePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxDragImage):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def SetBackingBitmap(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_SetBackingBitmap,(self,) + _args, _kwargs)
        return val
    def BeginDrag(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_BeginDrag,(self,) + _args, _kwargs)
        return val
    def BeginDragBounded(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_BeginDragBounded,(self,) + _args, _kwargs)
        return val
    def EndDrag(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_EndDrag,(self,) + _args, _kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_Move,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_Show,(self,) + _args, _kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_Hide,(self,) + _args, _kwargs)
        return val
    def GetImageRect(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_GetImageRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def RedrawImage(self, *_args, **_kwargs):
        val = apply(misc2c.wxDragImage_RedrawImage,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDragImage instance at %s>" % (self.this,)
    BeginDrag2 = BeginDragBounded
class wxDragImage(wxDragImagePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxDragImage,_args,_kwargs)
        self.thisown = 1



def wxDragIcon(*_args,**_kwargs):
    val = wxDragImagePtr(apply(misc2c.new_wxDragIcon,_args,_kwargs))
    val.thisown = 1
    return val

def wxDragString(*_args,**_kwargs):
    val = wxDragImagePtr(apply(misc2c.new_wxDragString,_args,_kwargs))
    val.thisown = 1
    return val

def wxDragTreeItem(*_args,**_kwargs):
    val = wxDragImagePtr(apply(misc2c.new_wxDragTreeItem,_args,_kwargs))
    val.thisown = 1
    return val

def wxDragListItem(*_args,**_kwargs):
    val = wxDragImagePtr(apply(misc2c.new_wxDragListItem,_args,_kwargs))
    val.thisown = 1
    return val


class wxPyTimerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxPyTimer):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetInterval(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_GetInterval,(self,) + _args, _kwargs)
        return val
    def IsOneShot(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_IsOneShot,(self,) + _args, _kwargs)
        return val
    def IsRunning(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_IsRunning,(self,) + _args, _kwargs)
        return val
    def SetOwner(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_SetOwner,(self,) + _args, _kwargs)
        return val
    def Start(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_Start,(self,) + _args, _kwargs)
        return val
    def Stop(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyTimer_Stop,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyTimer instance at %s>" % (self.this,)
class wxPyTimer(wxPyTimerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxPyTimer,_args,_kwargs)
        self.thisown = 1




class wxStopWatchPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxStopWatch):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Start(self, *_args, **_kwargs):
        val = apply(misc2c.wxStopWatch_Start,(self,) + _args, _kwargs)
        return val
    def Pause(self, *_args, **_kwargs):
        val = apply(misc2c.wxStopWatch_Pause,(self,) + _args, _kwargs)
        return val
    def Resume(self, *_args, **_kwargs):
        val = apply(misc2c.wxStopWatch_Resume,(self,) + _args, _kwargs)
        return val
    def Time(self, *_args, **_kwargs):
        val = apply(misc2c.wxStopWatch_Time,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStopWatch instance at %s>" % (self.this,)
class wxStopWatch(wxStopWatchPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxStopWatch,_args,_kwargs)
        self.thisown = 1




class wxLogPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Flush(self, *_args, **_kwargs):
        val = apply(misc2c.wxLog_Flush,(self,) + _args, _kwargs)
        return val
    def GetVerbose(self, *_args, **_kwargs):
        val = apply(misc2c.wxLog_GetVerbose,(self,) + _args, _kwargs)
        return val
    def TimeStamp(self, *_args, **_kwargs):
        val = apply(misc2c.wxLog_TimeStamp,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxLog instance at %s>" % (self.this,)
class wxLog(wxLogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLog,_args,_kwargs)
        self.thisown = 1




class wxLogStderrPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxLogStderr instance at %s>" % (self.this,)
class wxLogStderr(wxLogStderrPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogStderr,_args,_kwargs)
        self.thisown = 1




class wxLogTextCtrlPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxLogTextCtrl instance at %s>" % (self.this,)
class wxLogTextCtrl(wxLogTextCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogTextCtrl,_args,_kwargs)
        self.thisown = 1




class wxLogGuiPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxLogGui instance at %s>" % (self.this,)
class wxLogGui(wxLogGuiPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogGui,_args,_kwargs)
        self.thisown = 1




class wxLogWindowPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Show(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogWindow_Show,(self,) + _args, _kwargs)
        return val
    def GetFrame(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogWindow_GetFrame,(self,) + _args, _kwargs)
        return val
    def GetOldLog(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogWindow_GetOldLog,(self,) + _args, _kwargs)
        if val: val = wxLogPtr(val) 
        return val
    def IsPassingMessages(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogWindow_IsPassingMessages,(self,) + _args, _kwargs)
        return val
    def PassMessages(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogWindow_PassMessages,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxLogWindow instance at %s>" % (self.this,)
class wxLogWindow(wxLogWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogWindow,_args,_kwargs)
        self.thisown = 1




class wxLogChainPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetLog(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogChain_SetLog,(self,) + _args, _kwargs)
        return val
    def PassMessages(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogChain_PassMessages,(self,) + _args, _kwargs)
        return val
    def IsPassingMessages(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogChain_IsPassingMessages,(self,) + _args, _kwargs)
        return val
    def GetOldLog(self, *_args, **_kwargs):
        val = apply(misc2c.wxLogChain_GetOldLog,(self,) + _args, _kwargs)
        if val: val = wxLogPtr(val) 
        return val
    def __repr__(self):
        return "<C wxLogChain instance at %s>" % (self.this,)
class wxLogChain(wxLogChainPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogChain,_args,_kwargs)
        self.thisown = 1




class wxLogNullPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxLogNull):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxLogNull instance at %s>" % (self.this,)
class wxLogNull(wxLogNullPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxLogNull,_args,_kwargs)
        self.thisown = 1




class wxPyLogPtr(wxLogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyLog__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(misc2c.wxPyLog_Destroy,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyLog instance at %s>" % (self.this,)
class wxPyLog(wxPyLogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxPyLog,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyLog)




class wxProcessEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPid(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcessEvent_GetPid,(self,) + _args, _kwargs)
        return val
    def GetExitCode(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcessEvent_GetExitCode,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "m_pid" :
            misc2c.wxProcessEvent_m_pid_set(self,value)
            return
        if name == "m_exitcode" :
            misc2c.wxProcessEvent_m_exitcode_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_pid" : 
            return misc2c.wxProcessEvent_m_pid_get(self)
        if name == "m_exitcode" : 
            return misc2c.wxProcessEvent_m_exitcode_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxProcessEvent instance at %s>" % (self.this,)
class wxProcessEvent(wxProcessEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxProcessEvent,_args,_kwargs)
        self.thisown = 1




class wxProcessPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Destroy(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_Destroy,(self,) + _args, _kwargs)
        return val
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def base_OnTerminate(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_base_OnTerminate,(self,) + _args, _kwargs)
        return val
    def Redirect(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_Redirect,(self,) + _args, _kwargs)
        return val
    def IsRedirected(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_IsRedirected,(self,) + _args, _kwargs)
        return val
    def Detach(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_Detach,(self,) + _args, _kwargs)
        return val
    def GetInputStream(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_GetInputStream,(self,) + _args, _kwargs)
        return val
    def GetErrorStream(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_GetErrorStream,(self,) + _args, _kwargs)
        return val
    def GetOutputStream(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_GetOutputStream,(self,) + _args, _kwargs)
        if val: val = wxOutputStreamPtr(val) 
        return val
    def CloseOutput(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_CloseOutput,(self,) + _args, _kwargs)
        return val
    def IsInputOpened(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_IsInputOpened,(self,) + _args, _kwargs)
        return val
    def IsInputAvailable(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_IsInputAvailable,(self,) + _args, _kwargs)
        return val
    def IsErrorAvailable(self, *_args, **_kwargs):
        val = apply(misc2c.wxProcess_IsErrorAvailable,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxProcess instance at %s>" % (self.this,)
class wxProcess(wxProcessPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxProcess,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxProcess)




class wxJoystickPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxJoystick):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetZPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetZPosition,(self,) + _args, _kwargs)
        return val
    def GetButtonState(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetButtonState,(self,) + _args, _kwargs)
        return val
    def GetPOVPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetPOVPosition,(self,) + _args, _kwargs)
        return val
    def GetPOVCTSPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetPOVCTSPosition,(self,) + _args, _kwargs)
        return val
    def GetRudderPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetRudderPosition,(self,) + _args, _kwargs)
        return val
    def GetUPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetUPosition,(self,) + _args, _kwargs)
        return val
    def GetVPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetVPosition,(self,) + _args, _kwargs)
        return val
    def GetMovementThreshold(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetMovementThreshold,(self,) + _args, _kwargs)
        return val
    def SetMovementThreshold(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_SetMovementThreshold,(self,) + _args, _kwargs)
        return val
    def IsOk(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_IsOk,(self,) + _args, _kwargs)
        return val
    def GetNumberJoysticks(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetNumberJoysticks,(self,) + _args, _kwargs)
        return val
    def GetManufacturerId(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetManufacturerId,(self,) + _args, _kwargs)
        return val
    def GetProductId(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetProductId,(self,) + _args, _kwargs)
        return val
    def GetProductName(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetProductName,(self,) + _args, _kwargs)
        return val
    def GetXMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetXMin,(self,) + _args, _kwargs)
        return val
    def GetYMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetYMin,(self,) + _args, _kwargs)
        return val
    def GetZMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetZMin,(self,) + _args, _kwargs)
        return val
    def GetXMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetXMax,(self,) + _args, _kwargs)
        return val
    def GetYMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetYMax,(self,) + _args, _kwargs)
        return val
    def GetZMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetZMax,(self,) + _args, _kwargs)
        return val
    def GetNumberButtons(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetNumberButtons,(self,) + _args, _kwargs)
        return val
    def GetNumberAxes(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetNumberAxes,(self,) + _args, _kwargs)
        return val
    def GetMaxButtons(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetMaxButtons,(self,) + _args, _kwargs)
        return val
    def GetMaxAxes(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetMaxAxes,(self,) + _args, _kwargs)
        return val
    def GetPollingMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetPollingMin,(self,) + _args, _kwargs)
        return val
    def GetPollingMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetPollingMax,(self,) + _args, _kwargs)
        return val
    def GetRudderMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetRudderMin,(self,) + _args, _kwargs)
        return val
    def GetRudderMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetRudderMax,(self,) + _args, _kwargs)
        return val
    def GetUMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetUMin,(self,) + _args, _kwargs)
        return val
    def GetUMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetUMax,(self,) + _args, _kwargs)
        return val
    def GetVMin(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetVMin,(self,) + _args, _kwargs)
        return val
    def GetVMax(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_GetVMax,(self,) + _args, _kwargs)
        return val
    def HasRudder(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasRudder,(self,) + _args, _kwargs)
        return val
    def HasZ(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasZ,(self,) + _args, _kwargs)
        return val
    def HasU(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasU,(self,) + _args, _kwargs)
        return val
    def HasV(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasV,(self,) + _args, _kwargs)
        return val
    def HasPOV(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasPOV,(self,) + _args, _kwargs)
        return val
    def HasPOV4Dir(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasPOV4Dir,(self,) + _args, _kwargs)
        return val
    def HasPOVCTS(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_HasPOVCTS,(self,) + _args, _kwargs)
        return val
    def SetCapture(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_SetCapture,(self,) + _args, _kwargs)
        return val
    def ReleaseCapture(self, *_args, **_kwargs):
        val = apply(misc2c.wxJoystick_ReleaseCapture,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxJoystick instance at %s>" % (self.this,)
class wxJoystick(wxJoystickPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxJoystick,_args,_kwargs)
        self.thisown = 1




class wxWavePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxWave):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def IsOk(self, *_args, **_kwargs):
        val = apply(misc2c.wxWave_IsOk,(self,) + _args, _kwargs)
        return val
    def Play(self, *_args, **_kwargs):
        val = apply(misc2c.wxWave_Play,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxWave instance at %s>" % (self.this,)
class wxWave(wxWavePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxWave,_args,_kwargs)
        self.thisown = 1




class wxFileTypeInfoPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsValid(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_IsValid,(self,) + _args, _kwargs)
        return val
    def SetIcon(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_SetIcon,(self,) + _args, _kwargs)
        return val
    def SetShortDesc(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_SetShortDesc,(self,) + _args, _kwargs)
        return val
    def GetMimeType(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetMimeType,(self,) + _args, _kwargs)
        return val
    def GetOpenCommand(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetOpenCommand,(self,) + _args, _kwargs)
        return val
    def GetPrintCommand(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetPrintCommand,(self,) + _args, _kwargs)
        return val
    def GetShortDesc(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetShortDesc,(self,) + _args, _kwargs)
        return val
    def GetDescription(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetDescription,(self,) + _args, _kwargs)
        return val
    def GetExtensions(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetExtensions,(self,) + _args, _kwargs)
        return val
    def GetExtensionsCount(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetExtensionsCount,(self,) + _args, _kwargs)
        return val
    def GetIconFile(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetIconFile,(self,) + _args, _kwargs)
        return val
    def GetIconIndex(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileTypeInfo_GetIconIndex,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileTypeInfo instance at %s>" % (self.this,)
class wxFileTypeInfo(wxFileTypeInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxFileTypeInfo,_args,_kwargs)
        self.thisown = 1



def wxFileTypeInfoSequence(*_args,**_kwargs):
    val = wxFileTypeInfoPtr(apply(misc2c.new_wxFileTypeInfoSequence,_args,_kwargs))
    val.thisown = 1
    return val

def wxNullFileTypeInfo(*_args,**_kwargs):
    val = wxFileTypeInfoPtr(apply(misc2c.new_wxNullFileTypeInfo,_args,_kwargs))
    val.thisown = 1
    return val


class wxFileTypePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetMimeType(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetMimeType,(self,) + _args, _kwargs)
        return val
    def GetMimeTypes(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetMimeTypes,(self,) + _args, _kwargs)
        return val
    def GetExtensions(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetExtensions,(self,) + _args, _kwargs)
        return val
    def GetIcon(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) ; val.thisown = 1
        return val
    def GetIconInfo(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetIconInfo,(self,) + _args, _kwargs)
        return val
    def GetDescription(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetDescription,(self,) + _args, _kwargs)
        return val
    def GetOpenCommand(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetOpenCommand,(self,) + _args, _kwargs)
        return val
    def GetPrintCommand(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetPrintCommand,(self,) + _args, _kwargs)
        return val
    def GetAllCommands(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_GetAllCommands,(self,) + _args, _kwargs)
        return val
    def SetCommand(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_SetCommand,(self,) + _args, _kwargs)
        return val
    def SetDefaultIcon(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_SetDefaultIcon,(self,) + _args, _kwargs)
        return val
    def Unassociate(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileType_Unassociate,(self,) + _args, _kwargs)
        return val
    def __del__(self, delfunc=misc2c.delete_wxFileType):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxFileType instance at %s>" % (self.this,)
class wxFileType(wxFileTypePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxFileType,_args,_kwargs)
        self.thisown = 1




class wxMimeTypesManagerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Initialize(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_Initialize,(self,) + _args, _kwargs)
        return val
    def ClearData(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_ClearData,(self,) + _args, _kwargs)
        return val
    def GetFileTypeFromExtension(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_GetFileTypeFromExtension,(self,) + _args, _kwargs)
        if val: val = wxFileTypePtr(val) ; val.thisown = 1
        return val
    def GetFileTypeFromMimeType(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_GetFileTypeFromMimeType,(self,) + _args, _kwargs)
        if val: val = wxFileTypePtr(val) ; val.thisown = 1
        return val
    def ReadMailcap(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_ReadMailcap,(self,) + _args, _kwargs)
        return val
    def ReadMimeTypes(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_ReadMimeTypes,(self,) + _args, _kwargs)
        return val
    def EnumAllFileTypes(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_EnumAllFileTypes,(self,) + _args, _kwargs)
        return val
    def AddFallback(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_AddFallback,(self,) + _args, _kwargs)
        return val
    def Associate(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_Associate,(self,) + _args, _kwargs)
        if val: val = wxFileTypePtr(val) ; val.thisown = 1
        return val
    def Unassociate(self, *_args, **_kwargs):
        val = apply(misc2c.wxMimeTypesManager_Unassociate,(self,) + _args, _kwargs)
        return val
    def __del__(self, delfunc=misc2c.delete_wxMimeTypesManager):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def __repr__(self):
        return "<C wxMimeTypesManager instance at %s>" % (self.this,)
class wxMimeTypesManager(wxMimeTypesManagerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxMimeTypesManager,_args,_kwargs)
        self.thisown = 1




class wxArtProviderPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(misc2c.wxArtProvider__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxArtProvider instance at %s>" % (self.this,)
class wxArtProvider(wxArtProviderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxArtProvider,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxArtProvider)




class wxFileHistoryPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxFileHistory):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def AddFileToHistory(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_AddFileToHistory,(self,) + _args, _kwargs)
        return val
    def RemoveFileFromHistory(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_RemoveFileFromHistory,(self,) + _args, _kwargs)
        return val
    def GetMaxFiles(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_GetMaxFiles,(self,) + _args, _kwargs)
        return val
    def UseMenu(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_UseMenu,(self,) + _args, _kwargs)
        return val
    def RemoveMenu(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_RemoveMenu,(self,) + _args, _kwargs)
        return val
    def Load(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_Load,(self,) + _args, _kwargs)
        return val
    def Save(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_Save,(self,) + _args, _kwargs)
        return val
    def AddFilesToMenu(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_AddFilesToMenu,(self,) + _args, _kwargs)
        return val
    def AddFilesToThisMenu(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_AddFilesToThisMenu,(self,) + _args, _kwargs)
        return val
    def GetHistoryFile(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_GetHistoryFile,(self,) + _args, _kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_GetCount,(self,) + _args, _kwargs)
        return val
    def GetNoHistoryFiles(self, *_args, **_kwargs):
        val = apply(misc2c.wxFileHistory_GetNoHistoryFiles,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileHistory instance at %s>" % (self.this,)
class wxFileHistory(wxFileHistoryPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxFileHistory,_args,_kwargs)
        self.thisown = 1




class wxEffectsPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetHighlightColour(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_GetHighlightColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetLightShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_GetLightShadow,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetFaceColour(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_GetFaceColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetMediumShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_GetMediumShadow,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetDarkShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_GetDarkShadow,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetHighlightColour(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_SetHighlightColour,(self,) + _args, _kwargs)
        return val
    def SetLightShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_SetLightShadow,(self,) + _args, _kwargs)
        return val
    def SetFaceColour(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_SetFaceColour,(self,) + _args, _kwargs)
        return val
    def SetMediumShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_SetMediumShadow,(self,) + _args, _kwargs)
        return val
    def SetDarkShadow(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_SetDarkShadow,(self,) + _args, _kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_Set,(self,) + _args, _kwargs)
        return val
    def DrawSunkenEdge(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_DrawSunkenEdge,(self,) + _args, _kwargs)
        return val
    def TileBitmap(self, *_args, **_kwargs):
        val = apply(misc2c.wxEffects_TileBitmap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxEffects instance at %s>" % (self.this,)
class wxEffects(wxEffectsPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxEffects,_args,_kwargs)
        self.thisown = 1




class wxSingleInstanceCheckerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=misc2c.delete_wxSingleInstanceChecker):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def Create(self, *_args, **_kwargs):
        val = apply(misc2c.wxSingleInstanceChecker_Create,(self,) + _args, _kwargs)
        return val
    def IsAnotherRunning(self, *_args, **_kwargs):
        val = apply(misc2c.wxSingleInstanceChecker_IsAnotherRunning,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSingleInstanceChecker instance at %s>" % (self.this,)
class wxSingleInstanceChecker(wxSingleInstanceCheckerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxSingleInstanceChecker,_args,_kwargs)
        self.thisown = 1



def wxPreSingleInstanceChecker(*_args,**_kwargs):
    val = wxSingleInstanceCheckerPtr(apply(misc2c.new_wxPreSingleInstanceChecker,_args,_kwargs))
    val.thisown = 1
    return val




#-------------- FUNCTION WRAPPERS ------------------

wxFileSelector = misc2c.wxFileSelector

wxLoadFileSelector = misc2c.wxLoadFileSelector

wxSaveFileSelector = misc2c.wxSaveFileSelector

wxDirSelector = misc2c.wxDirSelector

wxGetTextFromUser = misc2c.wxGetTextFromUser

wxGetPasswordFromUser = misc2c.wxGetPasswordFromUser

wxGetSingleChoice = misc2c.wxGetSingleChoice

wxGetSingleChoiceIndex = misc2c.wxGetSingleChoiceIndex

wxMessageBox = misc2c.wxMessageBox

wxGetNumberFromUser = misc2c.wxGetNumberFromUser

wxColourDisplay = misc2c.wxColourDisplay

wxDisplayDepth = misc2c.wxDisplayDepth

wxGetDisplayDepth = misc2c.wxGetDisplayDepth

wxDisplaySize = misc2c.wxDisplaySize

def wxGetDisplaySize(*_args, **_kwargs):
    val = apply(misc2c.wxGetDisplaySize,_args,_kwargs)
    if val: val = wxSizePtr(val); val.thisown = 1
    return val

wxDisplaySizeMM = misc2c.wxDisplaySizeMM

def wxGetDisplaySizeMM(*_args, **_kwargs):
    val = apply(misc2c.wxGetDisplaySizeMM,_args,_kwargs)
    if val: val = wxSizePtr(val); val.thisown = 1
    return val

wxClientDisplayRect = misc2c.wxClientDisplayRect

def wxGetClientDisplayRect(*_args, **_kwargs):
    val = apply(misc2c.wxGetClientDisplayRect,_args,_kwargs)
    if val: val = wxRectPtr(val); val.thisown = 1
    return val

wxSetCursor = misc2c.wxSetCursor

wxBeginBusyCursor = misc2c.wxBeginBusyCursor

def wxGetActiveWindow(*_args, **_kwargs):
    val = apply(misc2c.wxGetActiveWindow,_args,_kwargs)
    return val

def wxGenericFindWindowAtPoint(*_args, **_kwargs):
    val = apply(misc2c.wxGenericFindWindowAtPoint,_args,_kwargs)
    return val

def wxFindWindowAtPoint(*_args, **_kwargs):
    val = apply(misc2c.wxFindWindowAtPoint,_args,_kwargs)
    return val

wxCheckForInterrupt = misc2c.wxCheckForInterrupt

wxFlushEvents = misc2c.wxFlushEvents

def wxGetTopLevelParent(*_args, **_kwargs):
    val = apply(misc2c.wxGetTopLevelParent,_args,_kwargs)
    return val

wxCaret_GetBlinkTime = misc2c.wxCaret_GetBlinkTime

wxCaret_SetBlinkTime = misc2c.wxCaret_SetBlinkTime

wxSafeYield = misc2c.wxSafeYield

wxPostEvent = misc2c.wxPostEvent

wxWakeUpIdle = misc2c.wxWakeUpIdle

wxWakeUpMainThread = misc2c.wxWakeUpMainThread

wxMutexGuiEnter = misc2c.wxMutexGuiEnter

wxMutexGuiLeave = misc2c.wxMutexGuiLeave

wxThread_IsMain = misc2c.wxThread_IsMain

wxShowTip = misc2c.wxShowTip

def wxCreateFileTipProvider(*_args, **_kwargs):
    val = apply(misc2c.wxCreateFileTipProvider,_args,_kwargs)
    if val: val = wxTipProviderPtr(val); val.thisown = 1
    return val

wxSysErrorCode = misc2c.wxSysErrorCode

wxSysErrorMsg = misc2c.wxSysErrorMsg

wxLogFatalError = misc2c.wxLogFatalError

wxLogError = misc2c.wxLogError

wxLogWarning = misc2c.wxLogWarning

wxLogMessage = misc2c.wxLogMessage

wxLogInfo = misc2c.wxLogInfo

wxLogDebug = misc2c.wxLogDebug

wxLogVerbose = misc2c.wxLogVerbose

wxLogStatus = misc2c.wxLogStatus

wxLogStatusFrame = misc2c.wxLogStatusFrame

wxLogSysError = misc2c.wxLogSysError

wxLogTrace = misc2c.wxLogTrace

wxLogTraceMask = misc2c.wxLogTraceMask

wxLogGeneric = misc2c.wxLogGeneric

wxSafeShowMessage = misc2c.wxSafeShowMessage

wxExecute = misc2c.wxExecute

def wxWaveData(*_args, **_kwargs):
    val = apply(misc2c.wxWaveData,_args,_kwargs)
    if val: val = wxWavePtr(val); val.thisown = 1
    return val

def wxSystemSettings_GetColour(*_args, **_kwargs):
    val = apply(misc2c.wxSystemSettings_GetColour,_args,_kwargs)
    if val: val = wxColourPtr(val); val.thisown = 1
    return val

def wxSystemSettings_GetFont(*_args, **_kwargs):
    val = apply(misc2c.wxSystemSettings_GetFont,_args,_kwargs)
    if val: val = wxFontPtr(val); val.thisown = 1
    return val

wxSystemSettings_GetMetric = misc2c.wxSystemSettings_GetMetric

wxSystemSettings_HasFeature = misc2c.wxSystemSettings_HasFeature

wxSystemSettings_GetScreenType = misc2c.wxSystemSettings_GetScreenType

wxSystemSettings_SetScreenType = misc2c.wxSystemSettings_SetScreenType

wxToolTip_Enable = misc2c.wxToolTip_Enable

wxToolTip_SetDelay = misc2c.wxToolTip_SetDelay

wxLog_IsEnabled = misc2c.wxLog_IsEnabled

wxLog_EnableLogging = misc2c.wxLog_EnableLogging

wxLog_OnLog = misc2c.wxLog_OnLog

wxLog_FlushActive = misc2c.wxLog_FlushActive

def wxLog_GetActiveTarget(*_args, **_kwargs):
    val = apply(misc2c.wxLog_GetActiveTarget,_args,_kwargs)
    if val: val = wxLogPtr(val)
    return val

def wxLog_SetActiveTarget(*_args, **_kwargs):
    val = apply(misc2c.wxLog_SetActiveTarget,_args,_kwargs)
    if val: val = wxLogPtr(val)
    return val

wxLog_Suspend = misc2c.wxLog_Suspend

wxLog_Resume = misc2c.wxLog_Resume

wxLog_SetVerbose = misc2c.wxLog_SetVerbose

wxLog_DontCreateOnDemand = misc2c.wxLog_DontCreateOnDemand

wxLog_SetTraceMask = misc2c.wxLog_SetTraceMask

wxLog_AddTraceMask = misc2c.wxLog_AddTraceMask

wxLog_RemoveTraceMask = misc2c.wxLog_RemoveTraceMask

wxLog_ClearTraceMasks = misc2c.wxLog_ClearTraceMasks

wxLog_GetTraceMasks = misc2c.wxLog_GetTraceMasks

wxLog_SetTimestamp = misc2c.wxLog_SetTimestamp

wxLog_GetTimestamp = misc2c.wxLog_GetTimestamp

wxLog_GetTraceMask = misc2c.wxLog_GetTraceMask

wxLog_IsAllowedTraceMask = misc2c.wxLog_IsAllowedTraceMask

wxLog_SetLogLevel = misc2c.wxLog_SetLogLevel

wxLog_GetLogLevel = misc2c.wxLog_GetLogLevel

wxProcess_Kill = misc2c.wxProcess_Kill

wxProcess_Exists = misc2c.wxProcess_Exists

def wxProcess_Open(*_args, **_kwargs):
    val = apply(misc2c.wxProcess_Open,_args,_kwargs)
    if val: val = wxProcessPtr(val)
    return val

wxFileType_ExpandCommand = misc2c.wxFileType_ExpandCommand

wxMimeTypesManager_IsOfType = misc2c.wxMimeTypesManager_IsOfType

wxArtProvider_PushProvider = misc2c.wxArtProvider_PushProvider

wxArtProvider_PopProvider = misc2c.wxArtProvider_PopProvider

wxArtProvider_RemoveProvider = misc2c.wxArtProvider_RemoveProvider

def wxArtProvider_GetBitmap(*_args, **_kwargs):
    val = apply(misc2c.wxArtProvider_GetBitmap,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxArtProvider_GetIcon(*_args, **_kwargs):
    val = apply(misc2c.wxArtProvider_GetIcon,_args,_kwargs)
    if val: val = wxIconPtr(val); val.thisown = 1
    return val



#-------------- VARIABLE WRAPPERS ------------------

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
wxSYS_COLOUR_DESKTOP = misc2c.wxSYS_COLOUR_DESKTOP
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
wxSYS_COLOUR_3DFACE = misc2c.wxSYS_COLOUR_3DFACE
wxSYS_COLOUR_BTNSHADOW = misc2c.wxSYS_COLOUR_BTNSHADOW
wxSYS_COLOUR_3DSHADOW = misc2c.wxSYS_COLOUR_3DSHADOW
wxSYS_COLOUR_GRAYTEXT = misc2c.wxSYS_COLOUR_GRAYTEXT
wxSYS_COLOUR_BTNTEXT = misc2c.wxSYS_COLOUR_BTNTEXT
wxSYS_COLOUR_INACTIVECAPTIONTEXT = misc2c.wxSYS_COLOUR_INACTIVECAPTIONTEXT
wxSYS_COLOUR_BTNHIGHLIGHT = misc2c.wxSYS_COLOUR_BTNHIGHLIGHT
wxSYS_COLOUR_BTNHILIGHT = misc2c.wxSYS_COLOUR_BTNHILIGHT
wxSYS_COLOUR_3DHIGHLIGHT = misc2c.wxSYS_COLOUR_3DHIGHLIGHT
wxSYS_COLOUR_3DHILIGHT = misc2c.wxSYS_COLOUR_3DHILIGHT
wxSYS_COLOUR_3DDKSHADOW = misc2c.wxSYS_COLOUR_3DDKSHADOW
wxSYS_COLOUR_3DLIGHT = misc2c.wxSYS_COLOUR_3DLIGHT
wxSYS_COLOUR_INFOTEXT = misc2c.wxSYS_COLOUR_INFOTEXT
wxSYS_COLOUR_INFOBK = misc2c.wxSYS_COLOUR_INFOBK
wxSYS_COLOUR_LISTBOX = misc2c.wxSYS_COLOUR_LISTBOX
wxSYS_COLOUR_HOTLIGHT = misc2c.wxSYS_COLOUR_HOTLIGHT
wxSYS_COLOUR_GRADIENTACTIVECAPTION = misc2c.wxSYS_COLOUR_GRADIENTACTIVECAPTION
wxSYS_COLOUR_GRADIENTINACTIVECAPTION = misc2c.wxSYS_COLOUR_GRADIENTINACTIVECAPTION
wxSYS_COLOUR_MENUHILIGHT = misc2c.wxSYS_COLOUR_MENUHILIGHT
wxSYS_COLOUR_MENUBAR = misc2c.wxSYS_COLOUR_MENUBAR
wxSYS_COLOUR_MAX = misc2c.wxSYS_COLOUR_MAX
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
wxSYS_CAN_DRAW_FRAME_DECORATIONS = misc2c.wxSYS_CAN_DRAW_FRAME_DECORATIONS
wxSYS_CAN_ICONIZE_FRAME = misc2c.wxSYS_CAN_ICONIZE_FRAME
wxSYS_SCREEN_NONE = misc2c.wxSYS_SCREEN_NONE
wxSYS_SCREEN_TINY = misc2c.wxSYS_SCREEN_TINY
wxSYS_SCREEN_PDA = misc2c.wxSYS_SCREEN_PDA
wxSYS_SCREEN_SMALL = misc2c.wxSYS_SCREEN_SMALL
wxSYS_SCREEN_DESKTOP = misc2c.wxSYS_SCREEN_DESKTOP
wxLOG_FatalError = misc2c.wxLOG_FatalError
wxLOG_Error = misc2c.wxLOG_Error
wxLOG_Warning = misc2c.wxLOG_Warning
wxLOG_Message = misc2c.wxLOG_Message
wxLOG_Info = misc2c.wxLOG_Info
wxLOG_Status = misc2c.wxLOG_Status
wxLOG_Debug = misc2c.wxLOG_Debug
wxLOG_Trace = misc2c.wxLOG_Trace
wxLOG_Progress = misc2c.wxLOG_Progress
wxLOG_User = misc2c.wxLOG_User
wxTRACE_MemAlloc = misc2c.wxTRACE_MemAlloc
wxTRACE_Messages = misc2c.wxTRACE_Messages
wxTRACE_ResAlloc = misc2c.wxTRACE_ResAlloc
wxTRACE_RefCount = misc2c.wxTRACE_RefCount
wxTRACE_OleCalls = misc2c.wxTRACE_OleCalls
wxTraceMemAlloc = misc2c.wxTraceMemAlloc
wxTraceMessages = misc2c.wxTraceMessages
wxTraceResAlloc = misc2c.wxTraceResAlloc
wxTraceRefCount = misc2c.wxTraceRefCount
wxTraceOleCalls = misc2c.wxTraceOleCalls
wxKILL_OK = misc2c.wxKILL_OK
wxKILL_BAD_SIGNAL = misc2c.wxKILL_BAD_SIGNAL
wxKILL_ACCESS_DENIED = misc2c.wxKILL_ACCESS_DENIED
wxKILL_NO_PROCESS = misc2c.wxKILL_NO_PROCESS
wxKILL_ERROR = misc2c.wxKILL_ERROR
wxSIGNONE = misc2c.wxSIGNONE
wxSIGHUP = misc2c.wxSIGHUP
wxSIGINT = misc2c.wxSIGINT
wxSIGQUIT = misc2c.wxSIGQUIT
wxSIGILL = misc2c.wxSIGILL
wxSIGTRAP = misc2c.wxSIGTRAP
wxSIGABRT = misc2c.wxSIGABRT
wxSIGIOT = misc2c.wxSIGIOT
wxSIGEMT = misc2c.wxSIGEMT
wxSIGFPE = misc2c.wxSIGFPE
wxSIGKILL = misc2c.wxSIGKILL
wxSIGBUS = misc2c.wxSIGBUS
wxSIGSEGV = misc2c.wxSIGSEGV
wxSIGSYS = misc2c.wxSIGSYS
wxSIGPIPE = misc2c.wxSIGPIPE
wxSIGALRM = misc2c.wxSIGALRM
wxSIGTERM = misc2c.wxSIGTERM
wxEVT_END_PROCESS = misc2c.wxEVT_END_PROCESS
wxEXEC_ASYNC = misc2c.wxEXEC_ASYNC
wxEXEC_SYNC = misc2c.wxEXEC_SYNC
wxEXEC_NOHIDE = misc2c.wxEXEC_NOHIDE
wxEXEC_MAKE_GROUP_LEADER = misc2c.wxEXEC_MAKE_GROUP_LEADER
wxJOYSTICK1 = misc2c.wxJOYSTICK1
wxJOYSTICK2 = misc2c.wxJOYSTICK2
wxJOY_BUTTON_ANY = misc2c.wxJOY_BUTTON_ANY
wxJOY_BUTTON1 = misc2c.wxJOY_BUTTON1
wxJOY_BUTTON2 = misc2c.wxJOY_BUTTON2
wxJOY_BUTTON3 = misc2c.wxJOY_BUTTON3
wxJOY_BUTTON4 = misc2c.wxJOY_BUTTON4
wxMAILCAP_STANDARD = misc2c.wxMAILCAP_STANDARD
wxMAILCAP_NETSCAPE = misc2c.wxMAILCAP_NETSCAPE
wxMAILCAP_KDE = misc2c.wxMAILCAP_KDE
wxMAILCAP_GNOME = misc2c.wxMAILCAP_GNOME
wxMAILCAP_ALL = misc2c.wxMAILCAP_ALL
cvar = misc2c.cvar
wxTheMimeTypesManager = wxMimeTypesManagerPtr(misc2c.cvar.wxTheMimeTypesManager)
