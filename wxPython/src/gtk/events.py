# This file was created automatically by SWIG.
import eventsc

from misc import *

from gdi import *
class wxEventPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,eventsc=eventsc):
        if self.thisown == 1 :
            eventsc.delete_wxEvent(self)
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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxSizeEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxCloseEvent,_args,_kwargs)
        self.thisown = 1




class wxCommandEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsChecked(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_IsChecked,(self,) + _args, _kwargs)
        return val
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
    def SetString(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_SetString,(self,) + _args, _kwargs)
        return val
    def SetExtraLong(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_SetExtraLong,(self,) + _args, _kwargs)
        return val
    def SetInt(self, *_args, **_kwargs):
        val = apply(eventsc.wxCommandEvent_SetInt,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCommandEvent instance at %s>" % (self.this,)
class wxCommandEvent(wxCommandEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxCommandEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxScrollEvent,_args,_kwargs)
        self.thisown = 1




class wxScrollWinEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(eventsc.wxScrollWinEvent_GetOrientation,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxScrollWinEvent_GetPosition,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxScrollWinEvent instance at %s>" % (self.this,)
class wxScrollWinEvent(wxScrollWinEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxScrollWinEvent,_args,_kwargs)
        self.thisown = 1




class wxSpinEventPtr(wxScrollEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSpinEvent instance at %s>" % (self.this,)
class wxSpinEvent(wxSpinEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxSpinEvent,_args,_kwargs)
        self.thisown = 1




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
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetPositionTuple(self, *_args, **_kwargs):
        val = apply(eventsc.wxMouseEvent_GetPositionTuple,(self,) + _args, _kwargs)
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
    def __setattr__(self,name,value):
        if name == "m_x" :
            eventsc.wxMouseEvent_m_x_set(self,value)
            return
        if name == "m_y" :
            eventsc.wxMouseEvent_m_y_set(self,value)
            return
        if name == "m_leftDown" :
            eventsc.wxMouseEvent_m_leftDown_set(self,value)
            return
        if name == "m_middleDown" :
            eventsc.wxMouseEvent_m_middleDown_set(self,value)
            return
        if name == "m_rightDown" :
            eventsc.wxMouseEvent_m_rightDown_set(self,value)
            return
        if name == "m_controlDown" :
            eventsc.wxMouseEvent_m_controlDown_set(self,value)
            return
        if name == "m_shiftDown" :
            eventsc.wxMouseEvent_m_shiftDown_set(self,value)
            return
        if name == "m_altDown" :
            eventsc.wxMouseEvent_m_altDown_set(self,value)
            return
        if name == "m_metaDown" :
            eventsc.wxMouseEvent_m_metaDown_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_x" : 
            return eventsc.wxMouseEvent_m_x_get(self)
        if name == "m_y" : 
            return eventsc.wxMouseEvent_m_y_get(self)
        if name == "m_leftDown" : 
            return eventsc.wxMouseEvent_m_leftDown_get(self)
        if name == "m_middleDown" : 
            return eventsc.wxMouseEvent_m_middleDown_get(self)
        if name == "m_rightDown" : 
            return eventsc.wxMouseEvent_m_rightDown_get(self)
        if name == "m_controlDown" : 
            return eventsc.wxMouseEvent_m_controlDown_get(self)
        if name == "m_shiftDown" : 
            return eventsc.wxMouseEvent_m_shiftDown_get(self)
        if name == "m_altDown" : 
            return eventsc.wxMouseEvent_m_altDown_get(self)
        if name == "m_metaDown" : 
            return eventsc.wxMouseEvent_m_metaDown_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxMouseEvent instance at %s>" % (self.this,)
class wxMouseEvent(wxMouseEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxMouseEvent,_args,_kwargs)
        self.thisown = 1




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
    def GetKeyCode(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_GetKeyCode,(self,) + _args, _kwargs)
        return val
    def HasModifiers(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_HasModifiers,(self,) + _args, _kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_GetY,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetPositionTuple(self, *_args, **_kwargs):
        val = apply(eventsc.wxKeyEvent_GetPositionTuple,(self,) + _args, _kwargs)
        return val
    def __setattr__(self,name,value):
        if name == "m_x" :
            eventsc.wxKeyEvent_m_x_set(self,value)
            return
        if name == "m_y" :
            eventsc.wxKeyEvent_m_y_set(self,value)
            return
        if name == "m_keyCode" :
            eventsc.wxKeyEvent_m_keyCode_set(self,value)
            return
        if name == "m_controlDown" :
            eventsc.wxKeyEvent_m_controlDown_set(self,value)
            return
        if name == "m_shiftDown" :
            eventsc.wxKeyEvent_m_shiftDown_set(self,value)
            return
        if name == "m_altDown" :
            eventsc.wxKeyEvent_m_altDown_set(self,value)
            return
        if name == "m_metaDown" :
            eventsc.wxKeyEvent_m_metaDown_set(self,value)
            return
        if name == "m_scanCode" :
            eventsc.wxKeyEvent_m_scanCode_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_x" : 
            return eventsc.wxKeyEvent_m_x_get(self)
        if name == "m_y" : 
            return eventsc.wxKeyEvent_m_y_get(self)
        if name == "m_keyCode" : 
            return eventsc.wxKeyEvent_m_keyCode_get(self)
        if name == "m_controlDown" : 
            return eventsc.wxKeyEvent_m_controlDown_get(self)
        if name == "m_shiftDown" : 
            return eventsc.wxKeyEvent_m_shiftDown_get(self)
        if name == "m_altDown" : 
            return eventsc.wxKeyEvent_m_altDown_get(self)
        if name == "m_metaDown" : 
            return eventsc.wxKeyEvent_m_metaDown_get(self)
        if name == "m_scanCode" : 
            return eventsc.wxKeyEvent_m_scanCode_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxKeyEvent instance at %s>" % (self.this,)
class wxKeyEvent(wxKeyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxKeyEvent,_args,_kwargs)
        self.thisown = 1




class wxNavigationKeyEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDirection(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_GetDirection,(self,) + _args, _kwargs)
        return val
    def SetDirection(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_SetDirection,(self,) + _args, _kwargs)
        return val
    def IsWindowChange(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_IsWindowChange,(self,) + _args, _kwargs)
        return val
    def SetWindowChange(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_SetWindowChange,(self,) + _args, _kwargs)
        return val
    def GetCurrentFocus(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_GetCurrentFocus,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetCurrentFocus(self, *_args, **_kwargs):
        val = apply(eventsc.wxNavigationKeyEvent_SetCurrentFocus,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNavigationKeyEvent instance at %s>" % (self.this,)
class wxNavigationKeyEvent(wxNavigationKeyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxNavigationKeyEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxMoveEvent,_args,_kwargs)
        self.thisown = 1




class wxPaintEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintEvent instance at %s>" % (self.this,)
class wxPaintEvent(wxPaintEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxPaintEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxEraseEvent,_args,_kwargs)
        self.thisown = 1




class wxFocusEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxFocusEvent instance at %s>" % (self.this,)
class wxFocusEvent(wxFocusEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxFocusEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxActivateEvent,_args,_kwargs)
        self.thisown = 1




class wxInitDialogEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxInitDialogEvent instance at %s>" % (self.this,)
class wxInitDialogEvent(wxInitDialogEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxInitDialogEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxMenuEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxShowEvent,_args,_kwargs)
        self.thisown = 1




class wxIconizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxIconizeEvent instance at %s>" % (self.this,)
class wxIconizeEvent(wxIconizeEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxIconizeEvent,_args,_kwargs)
        self.thisown = 1




class wxMaximizeEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMaximizeEvent instance at %s>" % (self.this,)
class wxMaximizeEvent(wxMaximizeEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxMaximizeEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxJoystickEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxIdleEvent,_args,_kwargs)
        self.thisown = 1




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
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxUpdateUIEvent,_args,_kwargs)
        self.thisown = 1




class wxSysColourChangedEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSysColourChangedEvent instance at %s>" % (self.this,)
class wxSysColourChangedEvent(wxSysColourChangedEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxSysColourChangedEvent,_args,_kwargs)
        self.thisown = 1




class wxNotifyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsAllowed(self, *_args, **_kwargs):
        val = apply(eventsc.wxNotifyEvent_IsAllowed,(self,) + _args, _kwargs)
        return val
    def Allow(self, *_args, **_kwargs):
        val = apply(eventsc.wxNotifyEvent_Allow,(self,) + _args, _kwargs)
        return val
    def Veto(self, *_args, **_kwargs):
        val = apply(eventsc.wxNotifyEvent_Veto,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotifyEvent instance at %s>" % (self.this,)
class wxNotifyEvent(wxNotifyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxNotifyEvent,_args,_kwargs)
        self.thisown = 1




class wxPaletteChangedEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetChangedWindow(self, *_args, **_kwargs):
        val = apply(eventsc.wxPaletteChangedEvent_SetChangedWindow,(self,) + _args, _kwargs)
        return val
    def GetChangedWindow(self, *_args, **_kwargs):
        val = apply(eventsc.wxPaletteChangedEvent_GetChangedWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxPaletteChangedEvent instance at %s>" % (self.this,)
class wxPaletteChangedEvent(wxPaletteChangedEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxPaletteChangedEvent,_args,_kwargs)
        self.thisown = 1




class wxQueryNewPaletteEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetPaletteRealized(self, *_args, **_kwargs):
        val = apply(eventsc.wxQueryNewPaletteEvent_SetPaletteRealized,(self,) + _args, _kwargs)
        return val
    def GetPaletteRealized(self, *_args, **_kwargs):
        val = apply(eventsc.wxQueryNewPaletteEvent_GetPaletteRealized,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxQueryNewPaletteEvent instance at %s>" % (self.this,)
class wxQueryNewPaletteEvent(wxQueryNewPaletteEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxQueryNewPaletteEvent,_args,_kwargs)
        self.thisown = 1




class wxWindowCreateEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetWindow(self, *_args, **_kwargs):
        val = apply(eventsc.wxWindowCreateEvent_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxWindowCreateEvent instance at %s>" % (self.this,)
class wxWindowCreateEvent(wxWindowCreateEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxWindowCreateEvent,_args,_kwargs)
        self.thisown = 1




class wxWindowDestroyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetWindow(self, *_args, **_kwargs):
        val = apply(eventsc.wxWindowDestroyEvent_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxWindowDestroyEvent instance at %s>" % (self.this,)
class wxWindowDestroyEvent(wxWindowDestroyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxWindowDestroyEvent,_args,_kwargs)
        self.thisown = 1




class wxTimerEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetInterval(self, *_args, **_kwargs):
        val = apply(eventsc.wxTimerEvent_GetInterval,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTimerEvent instance at %s>" % (self.this,)
class wxTimerEvent(wxTimerEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxTimerEvent,_args,_kwargs)
        self.thisown = 1




class wxPyEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,eventsc=eventsc):
        if self.thisown == 1 :
            eventsc.delete_wxPyEvent(self)
    def SetSelf(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyEvent_SetSelf,(self,) + _args, _kwargs)
        return val
    def GetSelf(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyEvent_GetSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyEvent instance at %s>" % (self.this,)
class wxPyEvent(wxPyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxPyEvent,_args,_kwargs)
        self.thisown = 1
        self.SetSelf(self)




class wxPyCommandEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,eventsc=eventsc):
        if self.thisown == 1 :
            eventsc.delete_wxPyCommandEvent(self)
    def SetSelf(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyCommandEvent_SetSelf,(self,) + _args, _kwargs)
        return val
    def GetSelf(self, *_args, **_kwargs):
        val = apply(eventsc.wxPyCommandEvent_GetSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyCommandEvent instance at %s>" % (self.this,)
class wxPyCommandEvent(wxPyCommandEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(eventsc.new_wxPyCommandEvent,_args,_kwargs)
        self.thisown = 1
        self.SetSelf(self)






#-------------- FUNCTION WRAPPERS ------------------

wxNewEventType = eventsc.wxNewEventType



#-------------- VARIABLE WRAPPERS ------------------

