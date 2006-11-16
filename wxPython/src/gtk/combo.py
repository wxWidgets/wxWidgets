# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
ComboCtrl class that can have any type of popup widget, and also an
owner-drawn combobox control.
"""

import _combo
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


import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
#---------------------------------------------------------------------------

CC_BUTTON_OUTSIDE_BORDER = _combo.CC_BUTTON_OUTSIDE_BORDER
CC_POPUP_ON_MOUSE_UP = _combo.CC_POPUP_ON_MOUSE_UP
CC_NO_TEXT_AUTO_SELECT = _combo.CC_NO_TEXT_AUTO_SELECT
CC_MF_ON_BUTTON = _combo.CC_MF_ON_BUTTON
CC_MF_ON_CLICK_AREA = _combo.CC_MF_ON_CLICK_AREA
class ComboCtrlFeatures(object):
    """Proxy of C++ ComboCtrlFeatures class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    MovableButton = _combo.ComboCtrlFeatures_MovableButton
    BitmapButton = _combo.ComboCtrlFeatures_BitmapButton
    ButtonSpacing = _combo.ComboCtrlFeatures_ButtonSpacing
    TextIndent = _combo.ComboCtrlFeatures_TextIndent
    PaintControl = _combo.ComboCtrlFeatures_PaintControl
    PaintWritable = _combo.ComboCtrlFeatures_PaintWritable
    Borderless = _combo.ComboCtrlFeatures_Borderless
    All = _combo.ComboCtrlFeatures_All
_combo.ComboCtrlFeatures_swigregister(ComboCtrlFeatures)

class ComboCtrl(_core.Control):
    """Proxy of C++ ComboCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_ANY, String value=wxEmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxPyComboBoxNameStr) -> ComboCtrl
        """
        _combo.ComboCtrl_swiginit(self,_combo.new_ComboCtrl(*args, **kwargs))
        self._setOORInfo(self);ComboCtrl._setCallbackInfo(self, self, ComboCtrl)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _combo.ComboCtrl__setCallbackInfo(*args, **kwargs)

    def ShowPopup(*args, **kwargs):
        """ShowPopup(self)"""
        return _combo.ComboCtrl_ShowPopup(*args, **kwargs)

    def HidePopup(*args, **kwargs):
        """HidePopup(self)"""
        return _combo.ComboCtrl_HidePopup(*args, **kwargs)

    def OnButtonClick(*args, **kwargs):
        """OnButtonClick(self)"""
        return _combo.ComboCtrl_OnButtonClick(*args, **kwargs)

    def IsPopupShown(*args, **kwargs):
        """IsPopupShown(self) -> bool"""
        return _combo.ComboCtrl_IsPopupShown(*args, **kwargs)

    def SetPopupControl(*args, **kwargs):
        """SetPopupControl(self, ComboPopup popup)"""
        return _combo.ComboCtrl_SetPopupControl(*args, **kwargs)

    def GetPopupControl(*args, **kwargs):
        """GetPopupControl(self) -> ComboPopup"""
        return _combo.ComboCtrl_GetPopupControl(*args, **kwargs)

    def GetPopupWindow(*args, **kwargs):
        """GetPopupWindow(self) -> Window"""
        return _combo.ComboCtrl_GetPopupWindow(*args, **kwargs)

    def GetTextCtrl(*args, **kwargs):
        """GetTextCtrl(self) -> wxTextCtrl"""
        return _combo.ComboCtrl_GetTextCtrl(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """GetButton(self) -> Window"""
        return _combo.ComboCtrl_GetButton(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _combo.ComboCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, String value)"""
        return _combo.ComboCtrl_SetValue(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self)"""
        return _combo.ComboCtrl_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """Cut(self)"""
        return _combo.ComboCtrl_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste(self)"""
        return _combo.ComboCtrl_Paste(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """SetInsertionPoint(self, long pos)"""
        return _combo.ComboCtrl_SetInsertionPoint(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """SetInsertionPointEnd(self)"""
        return _combo.ComboCtrl_SetInsertionPointEnd(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """GetInsertionPoint(self) -> long"""
        return _combo.ComboCtrl_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """GetLastPosition(self) -> long"""
        return _combo.ComboCtrl_GetLastPosition(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, long from, long to, String value)"""
        return _combo.ComboCtrl_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, long from, long to)"""
        return _combo.ComboCtrl_Remove(*args, **kwargs)

    def Undo(*args, **kwargs):
        """Undo(self)"""
        return _combo.ComboCtrl_Undo(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """SetMark(self, long from, long to)"""
        return _combo.ComboCtrl_SetMark(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, String value)"""
        return _combo.ComboCtrl_SetText(*args, **kwargs)

    def SetValueWithEvent(*args, **kwargs):
        """SetValueWithEvent(self, String value, bool withEvent=True)"""
        return _combo.ComboCtrl_SetValueWithEvent(*args, **kwargs)

    def SetPopupMinWidth(*args, **kwargs):
        """SetPopupMinWidth(self, int width)"""
        return _combo.ComboCtrl_SetPopupMinWidth(*args, **kwargs)

    def SetPopupMaxHeight(*args, **kwargs):
        """SetPopupMaxHeight(self, int height)"""
        return _combo.ComboCtrl_SetPopupMaxHeight(*args, **kwargs)

    def SetPopupExtents(*args, **kwargs):
        """SetPopupExtents(self, int extLeft, int extRight)"""
        return _combo.ComboCtrl_SetPopupExtents(*args, **kwargs)

    def SetCustomPaintWidth(*args, **kwargs):
        """SetCustomPaintWidth(self, int width)"""
        return _combo.ComboCtrl_SetCustomPaintWidth(*args, **kwargs)

    def GetCustomPaintWidth(*args, **kwargs):
        """GetCustomPaintWidth(self) -> int"""
        return _combo.ComboCtrl_GetCustomPaintWidth(*args, **kwargs)

    def SetPopupAnchor(*args, **kwargs):
        """SetPopupAnchor(self, int anchorSide)"""
        return _combo.ComboCtrl_SetPopupAnchor(*args, **kwargs)

    def SetButtonPosition(*args, **kwargs):
        """SetButtonPosition(self, int width=-1, int height=-1, int side=RIGHT, int spacingX=0)"""
        return _combo.ComboCtrl_SetButtonPosition(*args, **kwargs)

    def GetButtonSize(*args, **kwargs):
        """GetButtonSize(self) -> Size"""
        return _combo.ComboCtrl_GetButtonSize(*args, **kwargs)

    def SetButtonBitmaps(*args, **kwargs):
        """
        SetButtonBitmaps(self, Bitmap bmpNormal, bool pushButtonBg=False, Bitmap bmpPressed=wxNullBitmap, 
            Bitmap bmpHover=wxNullBitmap, 
            Bitmap bmpDisabled=wxNullBitmap)
        """
        return _combo.ComboCtrl_SetButtonBitmaps(*args, **kwargs)

    def SetTextIndent(*args, **kwargs):
        """SetTextIndent(self, int indent)"""
        return _combo.ComboCtrl_SetTextIndent(*args, **kwargs)

    def GetTextIndent(*args, **kwargs):
        """GetTextIndent(self) -> int"""
        return _combo.ComboCtrl_GetTextIndent(*args, **kwargs)

    def GetTextRect(*args, **kwargs):
        """GetTextRect(self) -> Rect"""
        return _combo.ComboCtrl_GetTextRect(*args, **kwargs)

    def UseAltPopupWindow(*args, **kwargs):
        """UseAltPopupWindow(self, bool enable=True)"""
        return _combo.ComboCtrl_UseAltPopupWindow(*args, **kwargs)

    def EnablePopupAnimation(*args, **kwargs):
        """EnablePopupAnimation(self, bool enable=True)"""
        return _combo.ComboCtrl_EnablePopupAnimation(*args, **kwargs)

    def IsKeyPopupToggle(*args, **kwargs):
        """IsKeyPopupToggle(self, KeyEvent event) -> bool"""
        return _combo.ComboCtrl_IsKeyPopupToggle(*args, **kwargs)

    def PrepareBackground(*args, **kwargs):
        """PrepareBackground(self, DC dc, Rect rect, int flags)"""
        return _combo.ComboCtrl_PrepareBackground(*args, **kwargs)

    def ShouldDrawFocus(*args, **kwargs):
        """ShouldDrawFocus(self) -> bool"""
        return _combo.ComboCtrl_ShouldDrawFocus(*args, **kwargs)

    def GetBitmapNormal(*args, **kwargs):
        """GetBitmapNormal(self) -> Bitmap"""
        return _combo.ComboCtrl_GetBitmapNormal(*args, **kwargs)

    def GetBitmapPressed(*args, **kwargs):
        """GetBitmapPressed(self) -> Bitmap"""
        return _combo.ComboCtrl_GetBitmapPressed(*args, **kwargs)

    def GetBitmapHover(*args, **kwargs):
        """GetBitmapHover(self) -> Bitmap"""
        return _combo.ComboCtrl_GetBitmapHover(*args, **kwargs)

    def GetBitmapDisabled(*args, **kwargs):
        """GetBitmapDisabled(self) -> Bitmap"""
        return _combo.ComboCtrl_GetBitmapDisabled(*args, **kwargs)

    def GetInternalFlags(*args, **kwargs):
        """GetInternalFlags(self) -> unsigned int"""
        return _combo.ComboCtrl_GetInternalFlags(*args, **kwargs)

    def IsCreated(*args, **kwargs):
        """IsCreated(self) -> bool"""
        return _combo.ComboCtrl_IsCreated(*args, **kwargs)

    def OnPopupDismiss(*args, **kwargs):
        """OnPopupDismiss(self)"""
        return _combo.ComboCtrl_OnPopupDismiss(*args, **kwargs)

    Hidden = _combo.ComboCtrl_Hidden
    Animating = _combo.ComboCtrl_Animating
    Visible = _combo.ComboCtrl_Visible
    def IsPopupWindowState(*args, **kwargs):
        """IsPopupWindowState(self, int state) -> bool"""
        return _combo.ComboCtrl_IsPopupWindowState(*args, **kwargs)

    def GetPopupWindowState(*args, **kwargs):
        """GetPopupWindowState(self) -> int"""
        return _combo.ComboCtrl_GetPopupWindowState(*args, **kwargs)

    def SetCtrlMainWnd(*args, **kwargs):
        """SetCtrlMainWnd(self, Window wnd)"""
        return _combo.ComboCtrl_SetCtrlMainWnd(*args, **kwargs)

    def GetFeatures(*args, **kwargs):
        """GetFeatures() -> int"""
        return _combo.ComboCtrl_GetFeatures(*args, **kwargs)

    GetFeatures = staticmethod(GetFeatures)
    ShowBelow = _combo.ComboCtrl_ShowBelow
    ShowAbove = _combo.ComboCtrl_ShowAbove
    CanDeferShow = _combo.ComboCtrl_CanDeferShow
    def DoShowPopup(*args, **kwargs):
        """DoShowPopup(self, Rect rect, int flags)"""
        return _combo.ComboCtrl_DoShowPopup(*args, **kwargs)

    def AnimateShow(*args, **kwargs):
        """AnimateShow(self, Rect rect, int flags) -> bool"""
        return _combo.ComboCtrl_AnimateShow(*args, **kwargs)

_combo.ComboCtrl_swigregister(ComboCtrl)

def PreComboCtrl(*args, **kwargs):
    """PreComboCtrl() -> ComboCtrl"""
    val = _combo.new_PreComboCtrl(*args, **kwargs)
    return val

def ComboCtrl_GetFeatures(*args):
  """ComboCtrl_GetFeatures() -> int"""
  return _combo.ComboCtrl_GetFeatures(*args)

#---------------------------------------------------------------------------

class ComboPopup(object):
    """Proxy of C++ ComboPopup class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> ComboPopup"""
        _combo.ComboPopup_swiginit(self,_combo.new_ComboPopup(*args, **kwargs))
        ComboPopup._setCallbackInfo(self, self, ComboPopup)

    __swig_destroy__ = _combo.delete_ComboPopup
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _combo.ComboPopup__setCallbackInfo(*args, **kwargs)

    def Init(*args, **kwargs):
        """Init(self)"""
        return _combo.ComboPopup_Init(*args, **kwargs)

    def Create(*args, **kwargs):
        """Create(self, Window parent) -> bool"""
        return _combo.ComboPopup_Create(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl(self) -> Window"""
        return _combo.ComboPopup_GetControl(*args, **kwargs)

    def OnPopup(*args, **kwargs):
        """OnPopup(self)"""
        return _combo.ComboPopup_OnPopup(*args, **kwargs)

    def OnDismiss(*args, **kwargs):
        """OnDismiss(self)"""
        return _combo.ComboPopup_OnDismiss(*args, **kwargs)

    def SetStringValue(*args, **kwargs):
        """SetStringValue(self, String value)"""
        return _combo.ComboPopup_SetStringValue(*args, **kwargs)

    def GetStringValue(*args, **kwargs):
        """GetStringValue(self) -> String"""
        return _combo.ComboPopup_GetStringValue(*args, **kwargs)

    def PaintComboControl(*args, **kwargs):
        """PaintComboControl(self, DC dc, Rect rect)"""
        return _combo.ComboPopup_PaintComboControl(*args, **kwargs)

    def OnComboKeyEvent(*args, **kwargs):
        """OnComboKeyEvent(self, KeyEvent event)"""
        return _combo.ComboPopup_OnComboKeyEvent(*args, **kwargs)

    def OnComboDoubleClick(*args, **kwargs):
        """OnComboDoubleClick(self)"""
        return _combo.ComboPopup_OnComboDoubleClick(*args, **kwargs)

    def GetAdjustedSize(*args, **kwargs):
        """GetAdjustedSize(self, int minWidth, int prefHeight, int maxHeight) -> Size"""
        return _combo.ComboPopup_GetAdjustedSize(*args, **kwargs)

    def LazyCreate(*args, **kwargs):
        """LazyCreate(self) -> bool"""
        return _combo.ComboPopup_LazyCreate(*args, **kwargs)

    def Dismiss(*args, **kwargs):
        """Dismiss(self)"""
        return _combo.ComboPopup_Dismiss(*args, **kwargs)

    def IsCreated(*args, **kwargs):
        """IsCreated(self) -> bool"""
        return _combo.ComboPopup_IsCreated(*args, **kwargs)

    def DefaultPaintComboControl(*args, **kwargs):
        """DefaultPaintComboControl(wxComboCtrlBase combo, DC dc, Rect rect)"""
        return _combo.ComboPopup_DefaultPaintComboControl(*args, **kwargs)

    DefaultPaintComboControl = staticmethod(DefaultPaintComboControl)
    def GetCombo(*args, **kwargs):
        """GetCombo(self) -> ComboCtrl"""
        return _combo.ComboPopup_GetCombo(*args, **kwargs)

_combo.ComboPopup_swigregister(ComboPopup)

def ComboPopup_DefaultPaintComboControl(*args, **kwargs):
  """ComboPopup_DefaultPaintComboControl(wxComboCtrlBase combo, DC dc, Rect rect)"""
  return _combo.ComboPopup_DefaultPaintComboControl(*args, **kwargs)

#---------------------------------------------------------------------------

ODCB_DCLICK_CYCLES = _combo.ODCB_DCLICK_CYCLES
ODCB_STD_CONTROL_PAINT = _combo.ODCB_STD_CONTROL_PAINT
ODCB_PAINTING_CONTROL = _combo.ODCB_PAINTING_CONTROL
ODCB_PAINTING_SELECTED = _combo.ODCB_PAINTING_SELECTED
class OwnerDrawnComboBox(ComboCtrl,_core.ItemContainer):
    """Proxy of C++ OwnerDrawnComboBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxPyComboBoxNameStr) -> OwnerDrawnComboBox
        """
        _combo.OwnerDrawnComboBox_swiginit(self,_combo.new_OwnerDrawnComboBox(*args, **kwargs))
        self._setOORInfo(self);OwnerDrawnComboBox._setCallbackInfo(self, self, OwnerDrawnComboBox)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxPyComboBoxNameStr) -> bool
        """
        return _combo.OwnerDrawnComboBox_Create(*args, **kwargs)

    def GetWidestItemWidth(*args, **kwargs):
        """GetWidestItemWidth(self) -> int"""
        return _combo.OwnerDrawnComboBox_GetWidestItemWidth(*args, **kwargs)

    def GetWidestItem(*args, **kwargs):
        """GetWidestItem(self) -> int"""
        return _combo.OwnerDrawnComboBox_GetWidestItem(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """SetMark(self, long from, long to)"""
        return _combo.OwnerDrawnComboBox_SetMark(*args, **kwargs)

_combo.OwnerDrawnComboBox_swigregister(OwnerDrawnComboBox)

def PreOwnerDrawnComboBox(*args, **kwargs):
    """PreOwnerDrawnComboBox() -> OwnerDrawnComboBox"""
    val = _combo.new_PreOwnerDrawnComboBox(*args, **kwargs)
    return val



