# This file was created automatically by SWIG.
import eventsc

from misc import *
class wxEventPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetEventObject(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_GetEventObject,(self,) + _args, _kwargs)
        return val
    def GetEventType(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_GetEventType,(self,) + _args, _kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_GetId,(self,) + _args, _kwargs)
        return val
    def GetSkipped(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_GetSkipped,(self,) + _args, _kwargs)
        return val
    def GetTimestamp(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_GetTimestamp,(self,) + _args, _kwargs)
        return val
    def SetEventObject(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_SetEventObject,(self,) + _args, _kwargs)
        return val
    def SetEventType(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_SetEventType,(self,) + _args, _kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_SetId,(self,) + _args, _kwargs)
        return val
    def SetTimestamp(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_SetTimestamp,(self,) + _args, _kwargs)
        return val
    def Skip(self, *_args, **_kwargs):
        val = apply(eventsc.wxEvent_Skip,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxEvent instance at %s>" % (self.this,)
class wxEvent(wxEventPtr):
    def __init__(self,this):
        self.this = this




class wxSizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSize(self, *_args, **_kwargs):
        val = apply(eventsc.wxSizeEvent_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxSizeEvent instance at %s>" % (self.this,)
class wxSizeEvent(wxSizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxCloseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetLoggingOff(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_SetLoggingOff,(self,) + _args, _kwargs)
        return val
    def GetLoggingOff(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_GetLoggingOff,(self,) + _args, _kwargs)
        return val
    def Veto(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_Veto,(self,) + _args, _kwargs)
        return val
    def CanVeto(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_CanVeto,(self,) + _args, _kwargs)
        return val
    def GetVeto(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_GetVeto,(self,) + _args, _kwargs)
        return val
    def SetCanVeto(self, *_args, **_kwargs):
        val = apply(eventsc.wxCloseEvent_SetCanVeto,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCloseEvent instance at %s>" % (self.this,)
class wxCloseEvent(wxCloseEventPtr):
    def __init__(self,this):
        self.this = this




class wxCommandEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Checked(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_Checked,(self,) + _args, _kwargs)
        return val
    def GetExtraLong(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_GetExtraLong,(self,) + _args, _kwargs)
        return val
    def GetInt(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_GetInt,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_GetString,(self,) + _args, _kwargs)
        return val
    def IsSelection(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_IsSelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCommandEvent instance at %s>" % (self.this,)
class wxCommandEvent(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this




class wxScrollEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(eventsc.wxScrollEvent_GetOrientation,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxScrollEvent_GetPosition,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxScrollEvent instance at %s>" % (self.this,)
class wxScrollEvent(wxScrollEventPtr):
    def __init__(self,this):
        self.this = this




class wxSpinEventPtr(wxScrollEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSpinEvent instance at %s>" % (self.this,)
class wxSpinEvent(wxSpinEventPtr):
    def __init__(self,this):
        self.this = this




class wxMouseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsButton(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_IsButton,(self,) + _args, _kwargs)
        return val
    def ButtonDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ButtonDown,(self,) + _args, _kwargs)
        return val
    def ButtonDClick(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ButtonDClick,(self,) + _args, _kwargs)
        return val
    def ButtonUp(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ButtonUp,(self,) + _args, _kwargs)
        return val
    def Button(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Button,(self,) + _args, _kwargs)
        return val
    def ButtonIsDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ButtonIsDown,(self,) + _args, _kwargs)
        return val
    def ControlDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ControlDown,(self,) + _args, _kwargs)
        return val
    def MetaDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_MetaDown,(self,) + _args, _kwargs)
        return val
    def AltDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_AltDown,(self,) + _args, _kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_ShiftDown,(self,) + _args, _kwargs)
        return val
    def LeftDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_LeftDown,(self,) + _args, _kwargs)
        return val
    def MiddleDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_MiddleDown,(self,) + _args, _kwargs)
        return val
    def RightDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_RightDown,(self,) + _args, _kwargs)
        return val
    def LeftUp(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_LeftUp,(self,) + _args, _kwargs)
        return val
    def MiddleUp(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_MiddleUp,(self,) + _args, _kwargs)
        return val
    def RightUp(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_RightUp,(self,) + _args, _kwargs)
        return val
    def LeftDClick(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_LeftDClick,(self,) + _args, _kwargs)
        return val
    def MiddleDClick(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_MiddleDClick,(self,) + _args, _kwargs)
        return val
    def RightDClick(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_RightDClick,(self,) + _args, _kwargs)
        return val
    def LeftIsDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_LeftIsDown,(self,) + _args, _kwargs)
        return val
    def MiddleIsDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_MiddleIsDown,(self,) + _args, _kwargs)
        return val
    def RightIsDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_RightIsDown,(self,) + _args, _kwargs)
        return val
    def Dragging(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Dragging,(self,) + _args, _kwargs)
        return val
    def Moving(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Moving,(self,) + _args, _kwargs)
        return val
    def Entering(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Entering,(self,) + _args, _kwargs)
        return val
    def Leaving(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Leaving,(self,) + _args, _kwargs)
        return val
    def Position(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_Position,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetLogicalPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetLogicalPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetY,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMouseEvent instance at %s>" % (self.this,)
class wxMouseEvent(wxMouseEventPtr):
    def __init__(self,this):
        self.this = this




class wxKeyEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ControlDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_ControlDown,(self,) + _args, _kwargs)
        return val
    def MetaDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_MetaDown,(self,) + _args, _kwargs)
        return val
    def AltDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_AltDown,(self,) + _args, _kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_ShiftDown,(self,) + _args, _kwargs)
        return val
    def KeyCode(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_KeyCode,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxKeyEvent instance at %s>" % (self.this,)
class wxKeyEvent(wxKeyEventPtr):
    def __init__(self,this):
        self.this = this




class wxMoveEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxMoveEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxMoveEvent instance at %s>" % (self.this,)
class wxMoveEvent(wxMoveEventPtr):
    def __init__(self,this):
        self.this = this




class wxPaintEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintEvent instance at %s>" % (self.this,)
class wxPaintEvent(wxPaintEventPtr):
    def __init__(self,this):
        self.this = this




class wxEraseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDC(self, *_args, **_kwargs):
        val = apply(eventsc.wxEraseEvent_GetDC,(self,) + _args, _kwargs)
        if val: val = wxDCPtr(val) 
        return val
    def __repr__(self):
        return "<C wxEraseEvent instance at %s>" % (self.this,)
class wxEraseEvent(wxEraseEventPtr):
    def __init__(self,this):
        self.this = this




class wxFocusEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxFocusEvent instance at %s>" % (self.this,)
class wxFocusEvent(wxFocusEventPtr):
    def __init__(self,this):
        self.this = this




class wxActivateEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetActive(self, *_args, **_kwargs):
        val = apply(eventsc.wxActivateEvent_GetActive,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxActivateEvent instance at %s>" % (self.this,)
class wxActivateEvent(wxActivateEventPtr):
    def __init__(self,this):
        self.this = this




class wxInitDialogEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxInitDialogEvent instance at %s>" % (self.this,)
class wxInitDialogEvent(wxInitDialogEventPtr):
    def __init__(self,this):
        self.this = this




class wxMenuEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetMenuId(self, *_args, **_kwargs):
        val = apply(eventsc.wxMenuEvent_GetMenuId,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMenuEvent instance at %s>" % (self.this,)
class wxMenuEvent(wxMenuEventPtr):
    def __init__(self,this):
        self.this = this




class wxShowEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetShow(self, *_args, **_kwargs):
        val = apply(eventsc.wxShowEvent_SetShow,(self,) + _args, _kwargs)
        return val
    def GetShow(self, *_args, **_kwargs):
        val = apply(eventsc.wxShowEvent_GetShow,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxShowEvent instance at %s>" % (self.this,)
class wxShowEvent(wxShowEventPtr):
    def __init__(self,this):
        self.this = this




class wxIconizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxIconizeEvent instance at %s>" % (self.this,)
class wxIconizeEvent(wxIconizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxMaximizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMaximizeEvent instance at %s>" % (self.this,)
class wxMaximizeEvent(wxMaximizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxJoystickEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetZPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_GetZPosition,(self,) + _args, _kwargs)
        return val
    def GetButtonState(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_GetButtonState,(self,) + _args, _kwargs)
        return val
    def GetButtonChange(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_GetButtonChange,(self,) + _args, _kwargs)
        return val
    def GetJoystick(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_GetJoystick,(self,) + _args, _kwargs)
        return val
    def SetJoystick(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_SetJoystick,(self,) + _args, _kwargs)
        return val
    def SetButtonState(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_SetButtonState,(self,) + _args, _kwargs)
        return val
    def SetButtonChange(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_SetButtonChange,(self,) + _args, _kwargs)
        return val
    def SetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_SetPosition,(self,) + _args, _kwargs)
        return val
    def SetZPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_SetZPosition,(self,) + _args, _kwargs)
        return val
    def IsButton(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_IsButton,(self,) + _args, _kwargs)
        return val
    def IsMove(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_IsMove,(self,) + _args, _kwargs)
        return val
    def IsZMove(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_IsZMove,(self,) + _args, _kwargs)
        return val
    def ButtonDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_ButtonDown,(self,) + _args, _kwargs)
        return val
    def ButtonUp(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_ButtonUp,(self,) + _args, _kwargs)
        return val
    def ButtonIsDown(self, *_args, **_kwargs):
        val = apply(eventsc.wxJoystickEvent_ButtonIsDown,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxJoystickEvent instance at %s>" % (self.this,)
class wxJoystickEvent(wxJoystickEventPtr):
    def __init__(self,this):
        self.this = this




class wxDropFilesEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxDropFilesEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetNumberOfFiles(self, *_args, **_kwargs):
        val = apply(eventsc.wxDropFilesEvent_GetNumberOfFiles,(self,) + _args, _kwargs)
        return val
    def GetFiles(self, *_args, **_kwargs):
        val = apply(eventsc.wxDropFilesEvent_GetFiles,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDropFilesEvent instance at %s>" % (self.this,)
class wxDropFilesEvent(wxDropFilesEventPtr):
    def __init__(self,this):
        self.this = this




class wxIdleEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RequestMore(self, *_args, **_kwargs):
        val = apply(eventsc.wxIdleEvent_RequestMore,(self,) + _args, _kwargs)
        return val
    def MoreRequested(self, *_args, **_kwargs):
        val = apply(eventsc.wxIdleEvent_MoreRequested,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxIdleEvent instance at %s>" % (self.this,)
class wxIdleEvent(wxIdleEventPtr):
    def __init__(self,this):
        self.this = this




class wxUpdateUIEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetChecked(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetChecked,(self,) + _args, _kwargs)
        return val
    def GetEnabled(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetEnabled,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetText,(self,) + _args, _kwargs)
        return val
    def GetSetText(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetSetText,(self,) + _args, _kwargs)
        return val
    def GetSetChecked(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetSetChecked,(self,) + _args, _kwargs)
        return val
    def GetSetEnabled(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_GetSetEnabled,(self,) + _args, _kwargs)
        return val
    def Check(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_Check,(self,) + _args, _kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_Enable,(self,) + _args, _kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = apply(eventsc.wxUpdateUIEvent_SetText,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxUpdateUIEvent instance at %s>" % (self.this,)
class wxUpdateUIEvent(wxUpdateUIEventPtr):
    def __init__(self,this):
        self.this = this




class wxSysColourChangedEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSysColourChangedEvent instance at %s>" % (self.this,)
class wxSysColourChangedEvent(wxSysColourChangedEventPtr):
    def __init__(self,this):
        self.this = this




class wxPyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,eventsc=eventsc):
        if self.thisown == 1 :
            eventsc.delete_wxPyEvent(self)
    def SetUserData(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyEvent_SetUserData,(self,) + _args, _kwargs)
        return val
    def GetUserData(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyEvent_GetUserData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyEvent instance at %s>" % (self.this,)
class wxPyEvent(wxPyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxPyEvent,_args,_kwargs)
        self.thisown = 1




class wxNotifyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsAllowed(self, *_args, **_kwargs):
        val = apply(eventsc.wxNotifyEvent_IsAllowed,(self,) + _args, _kwargs)
        return val
    def Veto(self, *_args, **_kwargs):
        val = apply(eventsc.wxNotifyEvent_Veto,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotifyEvent instance at %s>" % (self.this,)
class wxNotifyEvent(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

