# This file was created automatically by SWIG.
import eventsc

from misc import *
class wxEventPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetEventObject(self):
        val = eventsc.wxEvent_GetEventObject(self.this)
        return val
    def GetEventType(self):
        val = eventsc.wxEvent_GetEventType(self.this)
        return val
    def GetId(self):
        val = eventsc.wxEvent_GetId(self.this)
        return val
    def GetSkipped(self):
        val = eventsc.wxEvent_GetSkipped(self.this)
        return val
    def GetTimestamp(self):
        val = eventsc.wxEvent_GetTimestamp(self.this)
        return val
    def SetEventObject(self,arg0):
        val = eventsc.wxEvent_SetEventObject(self.this,arg0)
        return val
    def SetEventType(self,arg0):
        val = eventsc.wxEvent_SetEventType(self.this,arg0)
        return val
    def SetId(self,arg0):
        val = eventsc.wxEvent_SetId(self.this,arg0)
        return val
    def SetTimestamp(self,arg0):
        val = eventsc.wxEvent_SetTimestamp(self.this,arg0)
        return val
    def Skip(self,*args):
        val = apply(eventsc.wxEvent_Skip,(self.this,)+args)
        return val
    def __repr__(self):
        return "<C wxEvent instance>"
class wxEvent(wxEventPtr):
    def __init__(self,this):
        self.this = this




class wxSizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSize(self):
        val = eventsc.wxSizeEvent_GetSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxSizeEvent instance>"
class wxSizeEvent(wxSizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxCloseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetLoggingOff(self,arg0):
        val = eventsc.wxCloseEvent_SetLoggingOff(self.this,arg0)
        return val
    def GetLoggingOff(self):
        val = eventsc.wxCloseEvent_GetLoggingOff(self.this)
        return val
    def Veto(self,*args):
        val = apply(eventsc.wxCloseEvent_Veto,(self.this,)+args)
        return val
    def CanVeto(self):
        val = eventsc.wxCloseEvent_CanVeto(self.this)
        return val
    def GetVeto(self):
        val = eventsc.wxCloseEvent_GetVeto(self.this)
        return val
    def SetCanVeto(self,arg0):
        val = eventsc.wxCloseEvent_SetCanVeto(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxCloseEvent instance>"
class wxCloseEvent(wxCloseEventPtr):
    def __init__(self,this):
        self.this = this




class wxCommandEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Checked(self):
        val = eventsc.wxCommandEvent_Checked(self.this)
        return val
    def GetExtraLong(self):
        val = eventsc.wxCommandEvent_GetExtraLong(self.this)
        return val
    def GetInt(self):
        val = eventsc.wxCommandEvent_GetInt(self.this)
        return val
    def GetSelection(self):
        val = eventsc.wxCommandEvent_GetSelection(self.this)
        return val
    def GetString(self):
        val = eventsc.wxCommandEvent_GetString(self.this)
        return val
    def IsSelection(self):
        val = eventsc.wxCommandEvent_IsSelection(self.this)
        return val
    def __repr__(self):
        return "<C wxCommandEvent instance>"
class wxCommandEvent(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this




class wxScrollEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self):
        val = eventsc.wxScrollEvent_GetOrientation(self.this)
        return val
    def GetPosition(self):
        val = eventsc.wxScrollEvent_GetPosition(self.this)
        return val
    def __repr__(self):
        return "<C wxScrollEvent instance>"
class wxScrollEvent(wxScrollEventPtr):
    def __init__(self,this):
        self.this = this




class wxSpinEventPtr(wxScrollEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSpinEvent instance>"
class wxSpinEvent(wxSpinEventPtr):
    def __init__(self,this):
        self.this = this




class wxMouseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsButton(self):
        val = eventsc.wxMouseEvent_IsButton(self.this)
        return val
    def ButtonDown(self,*args):
        val = apply(eventsc.wxMouseEvent_ButtonDown,(self.this,)+args)
        return val
    def ButtonDClick(self,*args):
        val = apply(eventsc.wxMouseEvent_ButtonDClick,(self.this,)+args)
        return val
    def ButtonUp(self,*args):
        val = apply(eventsc.wxMouseEvent_ButtonUp,(self.this,)+args)
        return val
    def Button(self,arg0):
        val = eventsc.wxMouseEvent_Button(self.this,arg0)
        return val
    def ButtonIsDown(self,arg0):
        val = eventsc.wxMouseEvent_ButtonIsDown(self.this,arg0)
        return val
    def ControlDown(self):
        val = eventsc.wxMouseEvent_ControlDown(self.this)
        return val
    def MetaDown(self):
        val = eventsc.wxMouseEvent_MetaDown(self.this)
        return val
    def AltDown(self):
        val = eventsc.wxMouseEvent_AltDown(self.this)
        return val
    def ShiftDown(self):
        val = eventsc.wxMouseEvent_ShiftDown(self.this)
        return val
    def LeftDown(self):
        val = eventsc.wxMouseEvent_LeftDown(self.this)
        return val
    def MiddleDown(self):
        val = eventsc.wxMouseEvent_MiddleDown(self.this)
        return val
    def RightDown(self):
        val = eventsc.wxMouseEvent_RightDown(self.this)
        return val
    def LeftUp(self):
        val = eventsc.wxMouseEvent_LeftUp(self.this)
        return val
    def MiddleUp(self):
        val = eventsc.wxMouseEvent_MiddleUp(self.this)
        return val
    def RightUp(self):
        val = eventsc.wxMouseEvent_RightUp(self.this)
        return val
    def LeftDClick(self):
        val = eventsc.wxMouseEvent_LeftDClick(self.this)
        return val
    def MiddleDClick(self):
        val = eventsc.wxMouseEvent_MiddleDClick(self.this)
        return val
    def RightDClick(self):
        val = eventsc.wxMouseEvent_RightDClick(self.this)
        return val
    def LeftIsDown(self):
        val = eventsc.wxMouseEvent_LeftIsDown(self.this)
        return val
    def MiddleIsDown(self):
        val = eventsc.wxMouseEvent_MiddleIsDown(self.this)
        return val
    def RightIsDown(self):
        val = eventsc.wxMouseEvent_RightIsDown(self.this)
        return val
    def Dragging(self):
        val = eventsc.wxMouseEvent_Dragging(self.this)
        return val
    def Moving(self):
        val = eventsc.wxMouseEvent_Moving(self.this)
        return val
    def Entering(self):
        val = eventsc.wxMouseEvent_Entering(self.this)
        return val
    def Leaving(self):
        val = eventsc.wxMouseEvent_Leaving(self.this)
        return val
    def Position(self):
        val = eventsc.wxMouseEvent_Position(self.this)
        return val
    def GetPosition(self):
        val = eventsc.wxMouseEvent_GetPosition(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetLogicalPosition(self,arg0):
        val = eventsc.wxMouseEvent_GetLogicalPosition(self.this,arg0.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetX(self):
        val = eventsc.wxMouseEvent_GetX(self.this)
        return val
    def GetY(self):
        val = eventsc.wxMouseEvent_GetY(self.this)
        return val
    def __repr__(self):
        return "<C wxMouseEvent instance>"
class wxMouseEvent(wxMouseEventPtr):
    def __init__(self,this):
        self.this = this




class wxKeyEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ControlDown(self):
        val = eventsc.wxKeyEvent_ControlDown(self.this)
        return val
    def MetaDown(self):
        val = eventsc.wxKeyEvent_MetaDown(self.this)
        return val
    def AltDown(self):
        val = eventsc.wxKeyEvent_AltDown(self.this)
        return val
    def ShiftDown(self):
        val = eventsc.wxKeyEvent_ShiftDown(self.this)
        return val
    def KeyCode(self):
        val = eventsc.wxKeyEvent_KeyCode(self.this)
        return val
    def __repr__(self):
        return "<C wxKeyEvent instance>"
class wxKeyEvent(wxKeyEventPtr):
    def __init__(self,this):
        self.this = this




class wxMoveEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self):
        val = eventsc.wxMoveEvent_GetPosition(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxMoveEvent instance>"
class wxMoveEvent(wxMoveEventPtr):
    def __init__(self,this):
        self.this = this




class wxPaintEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintEvent instance>"
class wxPaintEvent(wxPaintEventPtr):
    def __init__(self,this):
        self.this = this




class wxEraseEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDC(self):
        val = eventsc.wxEraseEvent_GetDC(self.this)
        val = wxDCPtr(val)
        return val
    def __repr__(self):
        return "<C wxEraseEvent instance>"
class wxEraseEvent(wxEraseEventPtr):
    def __init__(self,this):
        self.this = this




class wxFocusEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxFocusEvent instance>"
class wxFocusEvent(wxFocusEventPtr):
    def __init__(self,this):
        self.this = this




class wxActivateEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetActive(self):
        val = eventsc.wxActivateEvent_GetActive(self.this)
        return val
    def __repr__(self):
        return "<C wxActivateEvent instance>"
class wxActivateEvent(wxActivateEventPtr):
    def __init__(self,this):
        self.this = this




class wxInitDialogEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxInitDialogEvent instance>"
class wxInitDialogEvent(wxInitDialogEventPtr):
    def __init__(self,this):
        self.this = this




class wxMenuEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetMenuId(self):
        val = eventsc.wxMenuEvent_GetMenuId(self.this)
        return val
    def __repr__(self):
        return "<C wxMenuEvent instance>"
class wxMenuEvent(wxMenuEventPtr):
    def __init__(self,this):
        self.this = this




class wxShowEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetShow(self,arg0):
        val = eventsc.wxShowEvent_SetShow(self.this,arg0)
        return val
    def GetShow(self):
        val = eventsc.wxShowEvent_GetShow(self.this)
        return val
    def __repr__(self):
        return "<C wxShowEvent instance>"
class wxShowEvent(wxShowEventPtr):
    def __init__(self,this):
        self.this = this




class wxIconizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxIconizeEvent instance>"
class wxIconizeEvent(wxIconizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxMaximizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMaximizeEvent instance>"
class wxMaximizeEvent(wxMaximizeEventPtr):
    def __init__(self,this):
        self.this = this




class wxJoystickEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self):
        val = eventsc.wxJoystickEvent_GetPosition(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetZPosition(self):
        val = eventsc.wxJoystickEvent_GetZPosition(self.this)
        return val
    def GetButtonState(self):
        val = eventsc.wxJoystickEvent_GetButtonState(self.this)
        return val
    def GetButtonChange(self):
        val = eventsc.wxJoystickEvent_GetButtonChange(self.this)
        return val
    def GetJoystick(self):
        val = eventsc.wxJoystickEvent_GetJoystick(self.this)
        return val
    def SetJoystick(self,arg0):
        val = eventsc.wxJoystickEvent_SetJoystick(self.this,arg0)
        return val
    def SetButtonState(self,arg0):
        val = eventsc.wxJoystickEvent_SetButtonState(self.this,arg0)
        return val
    def SetButtonChange(self,arg0):
        val = eventsc.wxJoystickEvent_SetButtonChange(self.this,arg0)
        return val
    def SetPosition(self,arg0):
        val = eventsc.wxJoystickEvent_SetPosition(self.this,arg0.this)
        return val
    def SetZPosition(self,arg0):
        val = eventsc.wxJoystickEvent_SetZPosition(self.this,arg0)
        return val
    def IsButton(self):
        val = eventsc.wxJoystickEvent_IsButton(self.this)
        return val
    def IsMove(self):
        val = eventsc.wxJoystickEvent_IsMove(self.this)
        return val
    def IsZMove(self):
        val = eventsc.wxJoystickEvent_IsZMove(self.this)
        return val
    def ButtonDown(self,*args):
        val = apply(eventsc.wxJoystickEvent_ButtonDown,(self.this,)+args)
        return val
    def ButtonUp(self,*args):
        val = apply(eventsc.wxJoystickEvent_ButtonUp,(self.this,)+args)
        return val
    def ButtonIsDown(self,*args):
        val = apply(eventsc.wxJoystickEvent_ButtonIsDown,(self.this,)+args)
        return val
    def __repr__(self):
        return "<C wxJoystickEvent instance>"
class wxJoystickEvent(wxJoystickEventPtr):
    def __init__(self,this):
        self.this = this




class wxDropFilesEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self):
        val = eventsc.wxDropFilesEvent_GetPosition(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetNumberOfFiles(self):
        val = eventsc.wxDropFilesEvent_GetNumberOfFiles(self.this)
        return val
    def GetFiles(self):
        val = eventsc.wxDropFilesEvent_GetFiles(self.this)
        return val
    def __repr__(self):
        return "<C wxDropFilesEvent instance>"
class wxDropFilesEvent(wxDropFilesEventPtr):
    def __init__(self,this):
        self.this = this




class wxIdleEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RequestMore(self,*args):
        val = apply(eventsc.wxIdleEvent_RequestMore,(self.this,)+args)
        return val
    def MoreRequested(self):
        val = eventsc.wxIdleEvent_MoreRequested(self.this)
        return val
    def __repr__(self):
        return "<C wxIdleEvent instance>"
class wxIdleEvent(wxIdleEventPtr):
    def __init__(self,this):
        self.this = this




class wxUpdateUIEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetChecked(self):
        val = eventsc.wxUpdateUIEvent_GetChecked(self.this)
        return val
    def GetEnabled(self):
        val = eventsc.wxUpdateUIEvent_GetEnabled(self.this)
        return val
    def GetText(self):
        val = eventsc.wxUpdateUIEvent_GetText(self.this)
        return val
    def GetSetText(self):
        val = eventsc.wxUpdateUIEvent_GetSetText(self.this)
        return val
    def GetSetChecked(self):
        val = eventsc.wxUpdateUIEvent_GetSetChecked(self.this)
        return val
    def GetSetEnabled(self):
        val = eventsc.wxUpdateUIEvent_GetSetEnabled(self.this)
        return val
    def Check(self,arg0):
        val = eventsc.wxUpdateUIEvent_Check(self.this,arg0)
        return val
    def Enable(self,arg0):
        val = eventsc.wxUpdateUIEvent_Enable(self.this,arg0)
        return val
    def SetText(self,arg0):
        val = eventsc.wxUpdateUIEvent_SetText(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxUpdateUIEvent instance>"
class wxUpdateUIEvent(wxUpdateUIEventPtr):
    def __init__(self,this):
        self.this = this




class wxSysColourChangedEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSysColourChangedEvent instance>"
class wxSysColourChangedEvent(wxSysColourChangedEventPtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

