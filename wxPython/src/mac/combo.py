# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
ComboCtrl class that can have any type ofconst wxBitmap& bitmap,  popup widget, and also an
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
    """
    Namespace for `wx.combo.ComboCtrl` feature flags.  See
    `wx.combo.ComboCtrl.GetFeatures`.
    """
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
    """
    A combo control is a generic combobox that allows for a totally custom
    popup. In addition it has other customization features. For instance,
    position and size of the dropdown button can be changed.

    To specify what to use for the popup control you need to derive a
    class from `wx.combo.ComboPopup` and pass it to the ComboCtrl with
    `SetPopupControl`.  It doesn't derive from any widget class so it can
    be used either as a mixin class combined with some standard or custom
    widget, or you can use the derived ComboPopup to create and hold an
    independent reference to the widget to be used for the popup.

    """
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
        """
        ShowPopup(self)

        Show the popup window.
        """
        return _combo.ComboCtrl_ShowPopup(*args, **kwargs)

    def HidePopup(*args, **kwargs):
        """
        HidePopup(self)

        Dismisses the popup window.
        """
        return _combo.ComboCtrl_HidePopup(*args, **kwargs)

    def OnButtonClick(*args, **kwargs):
        """
        OnButtonClick(self)

        Implement in a derived class to define what happens on dropdown button
        click.  Default action is to show the popup. 
        """
        return _combo.ComboCtrl_OnButtonClick(*args, **kwargs)

    def IsPopupShown(*args, **kwargs):
        """
        IsPopupShown(self) -> bool

        Returns true if the popup is currently shown.
        """
        return _combo.ComboCtrl_IsPopupShown(*args, **kwargs)

    def SetPopupControl(*args, **kwargs):
        """
        SetPopupControl(self, ComboPopup popup)

        Set popup interface class derived from `wx.combo.ComboPopup`. This
        method should be called as soon as possible after the control has been
        created, unless `OnButtonClick` has been overridden.
        """
        return _combo.ComboCtrl_SetPopupControl(*args, **kwargs)

    def GetPopupControl(*args, **kwargs):
        """
        GetPopupControl(self) -> ComboPopup

        Returns the current popup interface that has been set with
        `SetPopupControl`.
        """
        return _combo.ComboCtrl_GetPopupControl(*args, **kwargs)

    def GetPopupWindow(*args, **kwargs):
        """
        GetPopupWindow(self) -> Window

        Returns the popup window containing the popup control.
        """
        return _combo.ComboCtrl_GetPopupWindow(*args, **kwargs)

    def GetTextCtrl(*args, **kwargs):
        """
        GetTextCtrl(self) -> wxTextCtrl

        Get the text control which is part of the combo control.
        """
        return _combo.ComboCtrl_GetTextCtrl(*args, **kwargs)

    def GetButton(*args, **kwargs):
        """
        GetButton(self) -> Window

        Get the dropdown button which is part of the combobox.  Note: it's not
        necessarily a wx.Button or wx.BitmapButton.
        """
        return _combo.ComboCtrl_GetButton(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """
        GetValue(self) -> String

        Returns text representation of the current value. For writable combo
        control it always returns the value in the text field.
        """
        return _combo.ComboCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """
        SetValue(self, String value)

        Sets the text for the combo control text field.  For a combo control
        with wx.CB_READONLY style the string must be accepted by the popup (for
        instance, exist in the dropdown list), otherwise the call to
        SetValue is ignored.
        """
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
        """
        SetText(self, String value)

        Sets the text for the text field without affecting the popup. Thus,
        unlike `SetValue`, it works equally well with combo control using
        wx.CB_READONLY style.
        """
        return _combo.ComboCtrl_SetText(*args, **kwargs)

    def SetValueWithEvent(*args, **kwargs):
        """
        SetValueWithEvent(self, String value, bool withEvent=True)

        Same as `SetValue`, but also sends a EVT_TEXT event if withEvent is true.
        """
        return _combo.ComboCtrl_SetValueWithEvent(*args, **kwargs)

    def SetPopupMinWidth(*args, **kwargs):
        """
        SetPopupMinWidth(self, int width)

        Sets minimum width of the popup. If wider than combo control, it will
        extend to the left.  A value of -1 indicates to use the default.  The
        popup implementation may choose to ignore this.
        """
        return _combo.ComboCtrl_SetPopupMinWidth(*args, **kwargs)

    def SetPopupMaxHeight(*args, **kwargs):
        """
        SetPopupMaxHeight(self, int height)

        Sets preferred maximum height of the popup. A value of -1 indicates to
        use the default.  The popup implementation may choose to ignore this.
        """
        return _combo.ComboCtrl_SetPopupMaxHeight(*args, **kwargs)

    def SetPopupExtents(*args, **kwargs):
        """
        SetPopupExtents(self, int extLeft, int extRight)

        Extends popup size horizontally, relative to the edges of the combo
        control.  Values are given in pixels, and the defaults are zero.  It
        is up to the popup to fully take these values into account.
        """
        return _combo.ComboCtrl_SetPopupExtents(*args, **kwargs)

    def SetCustomPaintWidth(*args, **kwargs):
        """
        SetCustomPaintWidth(self, int width)

        Set width, in pixels, of custom painted area in control without
        wx.CB_READONLY style. In read-only OwnerDrawnComboBox, this is used
        to indicate the area that is not covered by the focus rectangle.
        """
        return _combo.ComboCtrl_SetCustomPaintWidth(*args, **kwargs)

    def GetCustomPaintWidth(*args, **kwargs):
        """GetCustomPaintWidth(self) -> int"""
        return _combo.ComboCtrl_GetCustomPaintWidth(*args, **kwargs)

    def SetPopupAnchor(*args, **kwargs):
        """
        SetPopupAnchor(self, int anchorSide)

        Set side of the control to which the popup will align itself. Valid
        values are wx.LEFT, wx.RIGHT and 0. The default value 0 means that the
        most appropriate side is used (which, currently, is always wx.LEFT).
        """
        return _combo.ComboCtrl_SetPopupAnchor(*args, **kwargs)

    def SetButtonPosition(*args, **kwargs):
        """
        SetButtonPosition(self, int width=-1, int height=-1, int side=RIGHT, int spacingX=0)

        Set the position of the dropdown button.
        """
        return _combo.ComboCtrl_SetButtonPosition(*args, **kwargs)

    def GetButtonSize(*args, **kwargs):
        """
        GetButtonSize(self) -> Size

        Returns current size of the dropdown button.
        """
        return _combo.ComboCtrl_GetButtonSize(*args, **kwargs)

    def SetButtonBitmaps(*args, **kwargs):
        """
        SetButtonBitmaps(self, Bitmap bmpNormal, bool pushButtonBg=False, Bitmap bmpPressed=wxNullBitmap, 
            Bitmap bmpHover=wxNullBitmap, 
            Bitmap bmpDisabled=wxNullBitmap)

        Sets custom dropdown button graphics.

            :param bmpNormal:  Default button image
            :param pushButtonBg: If ``True``, blank push button background is painted below the image.
            :param bmpPressed:  Depressed butotn image.
            :param bmpHover:  Button imate to use when the mouse hovers over it.
            :param bmpDisabled: Disabled button image.

        """
        return _combo.ComboCtrl_SetButtonBitmaps(*args, **kwargs)

    def SetTextIndent(*args, **kwargs):
        """
        SetTextIndent(self, int indent)

        This will set the space in pixels between left edge of the control and
        the text, regardless whether control is read-only or not. A value of -1 can
        be given to indicate platform default.
        """
        return _combo.ComboCtrl_SetTextIndent(*args, **kwargs)

    def GetTextIndent(*args, **kwargs):
        """
        GetTextIndent(self) -> int

        Returns actual indentation in pixels.
        """
        return _combo.ComboCtrl_GetTextIndent(*args, **kwargs)

    def GetTextRect(*args, **kwargs):
        """
        GetTextRect(self) -> Rect

        Returns area covered by the text field (includes everything except
        borders and the dropdown button).
        """
        return _combo.ComboCtrl_GetTextRect(*args, **kwargs)

    def UseAltPopupWindow(*args, **kwargs):
        """
        UseAltPopupWindow(self, bool enable=True)

        Enable or disable usage of an alternative popup window, which
        guarantees ability to focus the popup control, and allows common
        native controls to function normally. This alternative popup window is
        usually a wxDialog, and as such, when it is shown, its parent
        top-level window will appear as if the focus has been lost from it.
        """
        return _combo.ComboCtrl_UseAltPopupWindow(*args, **kwargs)

    def EnablePopupAnimation(*args, **kwargs):
        """
        EnablePopupAnimation(self, bool enable=True)

        Enables or disables popup animation, if any, depending on the value of
        the argument.
        """
        return _combo.ComboCtrl_EnablePopupAnimation(*args, **kwargs)

    def IsKeyPopupToggle(*args, **kwargs):
        """
        IsKeyPopupToggle(self, KeyEvent event) -> bool

        Returns true if given key combination should toggle the popup.
        """
        return _combo.ComboCtrl_IsKeyPopupToggle(*args, **kwargs)

    def PrepareBackground(*args, **kwargs):
        """
        PrepareBackground(self, DC dc, Rect rect, int flags)

        Prepare background of combo control or an item in a dropdown list in a
        way typical on platform. This includes painting the focus/disabled
        background and setting the clipping region.  Unless you plan to paint
        your own focus indicator, you should always call this in your
        wxComboPopup::PaintComboControl implementation.  In addition, it sets
        pen and text colour to what looks good and proper against the
        background.

        flags are the same as wx.RendererNative flags:

            ======================   ============================================
            wx.CONTROL_ISSUBMENU     drawing a list item instead of combo control
            wx.CONTROL_SELECTED      list item is selected
            wx.CONTROL_DISABLED       control/item is disabled
            ======================   ============================================

        """
        return _combo.ComboCtrl_PrepareBackground(*args, **kwargs)

    def ShouldDrawFocus(*args, **kwargs):
        """
        ShouldDrawFocus(self) -> bool

        Returns true if focus indicator should be drawn in the control.
        """
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
        """
        IsCreated(self) -> bool

        Return true if Create has finished
        """
        return _combo.ComboCtrl_IsCreated(*args, **kwargs)

    def OnPopupDismiss(*args, **kwargs):
        """
        OnPopupDismiss(self)

        Common code to be called on popup hide/dismiss
        """
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

    def GetMainWindowOfCompositeControl(*args, **kwargs):
        """GetMainWindowOfCompositeControl(self) -> Window"""
        return _combo.ComboCtrl_GetMainWindowOfCompositeControl(*args, **kwargs)

    def GetFeatures(*args, **kwargs):
        """
        GetFeatures() -> int

        Returns a bit-list of flags indicating which features of the ComboCtrl
        functionality are implemented by this implemetation.  See
        `wx.combo.ComboCtrlFeatures`.
        """
        return _combo.ComboCtrl_GetFeatures(*args, **kwargs)

    GetFeatures = staticmethod(GetFeatures)
    ShowBelow = _combo.ComboCtrl_ShowBelow
    ShowAbove = _combo.ComboCtrl_ShowAbove
    CanDeferShow = _combo.ComboCtrl_CanDeferShow
    def DoShowPopup(*args, **kwargs):
        """
        DoShowPopup(self, Rect rect, int flags)

        Shows and positions the popup.

        Flags:
            ============  =====================================================
            ShowBelow     Showing popup below the control
            ShowAbove     Showing popup above the control
            CanDeferShow  Can only return true from AnimateShow if this is set
            ============  =====================================================

        """
        return _combo.ComboCtrl_DoShowPopup(*args, **kwargs)

    def AnimateShow(*args, **kwargs):
        """
        AnimateShow(self, Rect rect, int flags) -> bool

        Implement in derived class to create a drop-down animation.  Return
        ``True`` if finished immediately. Otherwise the popup is only shown when the
        derived class calls `DoShowPopup`.  Flags are same as for `DoShowPopup`.

        """
        return _combo.ComboCtrl_AnimateShow(*args, **kwargs)

    PopupControl = property(GetPopupControl,SetPopupControl) 
    PopupWindow = property(GetPopupWindow) 
    TextCtrl = property(GetTextCtrl) 
    Button = property(GetButton) 
    Value = property(GetValue,SetValue) 
    InsertionPoint = property(GetInsertionPoint) 
    CustomPaintWidth = property(GetCustomPaintWidth,SetCustomPaintWidth) 
    ButtonSize = property(GetButtonSize) 
    TextIndent = property(GetTextIndent,SetTextIndent) 
    TextRect = property(GetTextRect) 
    BitmapNormal = property(GetBitmapNormal) 
    BitmapPressed = property(GetBitmapPressed) 
    BitmapHover = property(GetBitmapHover) 
    BitmapDisabled = property(GetBitmapDisabled) 
    PopupWindowState = property(GetPopupWindowState) 
_combo.ComboCtrl_swigregister(ComboCtrl)

def PreComboCtrl(*args, **kwargs):
    """PreComboCtrl() -> ComboCtrl"""
    val = _combo.new_PreComboCtrl(*args, **kwargs)
    return val

def ComboCtrl_GetFeatures(*args):
  """
    ComboCtrl_GetFeatures() -> int

    Returns a bit-list of flags indicating which features of the ComboCtrl
    functionality are implemented by this implemetation.  See
    `wx.combo.ComboCtrlFeatures`.
    """
  return _combo.ComboCtrl_GetFeatures(*args)

#---------------------------------------------------------------------------

class ComboPopup(object):
    """
    In order to use a custom popup with `wx.combo.ComboCtrl` an interface
    class derived from wx.combo.ComboPopup is used to manage the interface
    between the popup control and the popup.  You can either derive a new
    class from both the widget class and this ComboPopup class, or the
    derived class can have a reference to the widget used for the popup.
    In either case you simply need to return the widget from the
    `GetControl` method to allow the ComboCtrl to interact with it.

    Nearly all of the methods of this class are overridable in Python.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ComboPopup

        Constructor
        """
        _combo.ComboPopup_swiginit(self,_combo.new_ComboPopup(*args, **kwargs))
        ComboPopup._setCallbackInfo(self, self, ComboPopup)

    __swig_destroy__ = _combo.delete_ComboPopup
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _combo.ComboPopup__setCallbackInfo(*args, **kwargs)

    def Init(*args, **kwargs):
        """
        Init(self)

        This method is called after the popup is contructed and has been
        assigned to the ComboCtrl.  Derived classes can override this to do
        extra inialization or whatever.
        """
        return _combo.ComboPopup_Init(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent) -> bool

        The derived class must implement this method to create the popup
        control.  It should be a child of the ``parent`` passed in, but other
        than that there is much flexibility in what the widget can be, its
        style, etc.  Return ``True`` for success, ``False`` otherwise.  (NOTE:
        this return value is not currently checked...)
        """
        return _combo.ComboPopup_Create(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """
        GetControl(self) -> Window

        The derived class must implement this method and it should return a
        reference to the widget created in the `Create` method.  If the
        derived class inherits from both the widget class and ComboPopup then
        the return value is probably just ``self``.
        """
        return _combo.ComboPopup_GetControl(*args, **kwargs)

    def OnPopup(*args, **kwargs):
        """
        OnPopup(self)

        The derived class may implement this to do special processing when
        popup is shown.
        """
        return _combo.ComboPopup_OnPopup(*args, **kwargs)

    def OnDismiss(*args, **kwargs):
        """
        OnDismiss(self)

        The derived class may implement this to do special processing when
        popup is hidden.
        """
        return _combo.ComboPopup_OnDismiss(*args, **kwargs)

    def SetStringValue(*args, **kwargs):
        """
        SetStringValue(self, String value)

        Called just prior to displaying the popup.  The derived class can
        implement this to "select" the item in the popup that coresponds to
        the passed in string value, if appropriate.  The default
        implementation does nothing.
        """
        return _combo.ComboPopup_SetStringValue(*args, **kwargs)

    def GetStringValue(*args, **kwargs):
        """
        GetStringValue(self) -> String

        Gets the string representation of the currently selected value to be
        used to display in the combo widget.
        """
        return _combo.ComboPopup_GetStringValue(*args, **kwargs)

    def PaintComboControl(*args, **kwargs):
        """
        PaintComboControl(self, DC dc, Rect rect)

        This is called to custom paint in the combo control itself (ie. not
        the popup).  Default implementation draws the current value as string.
        """
        return _combo.ComboPopup_PaintComboControl(*args, **kwargs)

    def OnComboKeyEvent(*args, **kwargs):
        """
        OnComboKeyEvent(self, KeyEvent event)

        Receives key events from the parent ComboCtrl.  Events not handled
        should be skipped, as usual.
        """
        return _combo.ComboPopup_OnComboKeyEvent(*args, **kwargs)

    def OnComboDoubleClick(*args, **kwargs):
        """
        OnComboDoubleClick(self)

        Implement this method in the derived class if you need to support
        special actions when the user double-clicks on the parent ComboCtrl.
        """
        return _combo.ComboPopup_OnComboDoubleClick(*args, **kwargs)

    def GetAdjustedSize(*args, **kwargs):
        """
        GetAdjustedSize(self, int minWidth, int prefHeight, int maxHeight) -> Size

        The derived class may implement this method to return adjusted size
        for the popup control, according to the variables given.  It is called
        on every popup, just prior to `OnPopup`.

            :param minWidth:    Preferred minimum width.
            :param prefHeight:  Preferred height. May be -1 to indicate no preference.
            :maxWidth:          Max height for window, as limited by screen size, and
                                should only be rounded down, if necessary.

        """
        return _combo.ComboPopup_GetAdjustedSize(*args, **kwargs)

    def LazyCreate(*args, **kwargs):
        """
        LazyCreate(self) -> bool

        The derived class may implement this to return ``True`` if it wants to
        delay the call to `Create` until the popup is shown for the first
        time. It is more efficient, but on the other hand it is often more
        convenient to have the control created immediately.  The default
        implementation returns ``False``.
        """
        return _combo.ComboPopup_LazyCreate(*args, **kwargs)

    def Dismiss(*args, **kwargs):
        """
        Dismiss(self)

        Hides the popup
        """
        return _combo.ComboPopup_Dismiss(*args, **kwargs)

    def IsCreated(*args, **kwargs):
        """
        IsCreated(self) -> bool

        Returns true if `Create` has been called.
        """
        return _combo.ComboPopup_IsCreated(*args, **kwargs)

    def DefaultPaintComboControl(*args, **kwargs):
        """
        DefaultPaintComboControl(wxComboCtrlBase combo, DC dc, Rect rect)

        Default PaintComboControl behaviour
        """
        return _combo.ComboPopup_DefaultPaintComboControl(*args, **kwargs)

    DefaultPaintComboControl = staticmethod(DefaultPaintComboControl)
    def GetCombo(*args, **kwargs):
        """
        GetCombo(self) -> ComboCtrl

        Returns a reference to the `wx.combo.ComboCtrl` this ComboPopup object
        is associated with.
        """
        return _combo.ComboPopup_GetCombo(*args, **kwargs)

_combo.ComboPopup_swigregister(ComboPopup)

def ComboPopup_DefaultPaintComboControl(*args, **kwargs):
  """
    ComboPopup_DefaultPaintComboControl(wxComboCtrlBase combo, DC dc, Rect rect)

    Default PaintComboControl behaviour
    """
  return _combo.ComboPopup_DefaultPaintComboControl(*args, **kwargs)

#---------------------------------------------------------------------------

ODCB_DCLICK_CYCLES = _combo.ODCB_DCLICK_CYCLES
ODCB_STD_CONTROL_PAINT = _combo.ODCB_STD_CONTROL_PAINT
ODCB_PAINTING_CONTROL = _combo.ODCB_PAINTING_CONTROL
ODCB_PAINTING_SELECTED = _combo.ODCB_PAINTING_SELECTED
class OwnerDrawnComboBox(ComboCtrl,_core.ItemContainer):
    """
    wx.combo.OwnerDrawnComboBox is a combobox with owner-drawn list
    items. In essence, it is a `wx.combo.ComboCtrl` with a `wx.VListBox`
    popup and a `wx.ControlWithItems` API.

    Implementing item drawing and measuring is similar to wx.VListBox.
    The application needs to subclass wx.combo.OwnerDrawnComboBox and
    implement the `OnDrawItem`, `OnMeasureItem` and `OnMeasureItemWidth`
    methods.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxPyComboBoxNameStr) -> OwnerDrawnComboBox

        Standard constructor.
        """
        _combo.OwnerDrawnComboBox_swiginit(self,_combo.new_OwnerDrawnComboBox(*args, **kwargs))
        self._setOORInfo(self);OwnerDrawnComboBox._setCallbackInfo(self, self, OwnerDrawnComboBox)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _combo.OwnerDrawnComboBox__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxPyComboBoxNameStr) -> bool

        Create the UI object, and other initialization.
        """
        return _combo.OwnerDrawnComboBox_Create(*args, **kwargs)

    def GetWidestItemWidth(*args, **kwargs):
        """
        GetWidestItemWidth(self) -> int

        Return the widest item width (recalculating it if necessary.)
        """
        return _combo.OwnerDrawnComboBox_GetWidestItemWidth(*args, **kwargs)

    def GetWidestItem(*args, **kwargs):
        """
        GetWidestItem(self) -> int

        Return the index of the widest item (recalculating it if necessary.)
        """
        return _combo.OwnerDrawnComboBox_GetWidestItem(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """SetMark(self, long from, long to)"""
        return _combo.OwnerDrawnComboBox_SetMark(*args, **kwargs)

    def OnDrawItem(*args, **kwargs):
        """
        OnDrawItem(self, DC dc, Rect rect, int item, int flags)

        The derived class may implement this function to actually draw the
        item with the given index on the provided DC. If this method is not
        overridden, the item text is simply drawn as if the control was a
        normal combobox.

           :param dc:    The device context to use for drawing.
           :param rect:  The bounding rectangle for the item being drawn, the
                         DC's clipping region is set to this rectangle before
                         calling this method.
           :param item:  The index of the item to be drawn.

           :param flags: ``wx.combo.ODCB_PAINTING_CONTROL`` (The Combo control itself
                          is being painted, instead of a list item.  The ``item``
                          parameter may be ``wx.NOT_FOUND`` in this case.
                          ``wx.combo.ODCB_PAINTING_SELECTED``  (An item with
                          selection background is being painted. The DC's text colour
                          should already be correct.

        """
        return _combo.OwnerDrawnComboBox_OnDrawItem(*args, **kwargs)

    def OnMeasureItem(*args, **kwargs):
        """
        OnMeasureItem(self, size_t item) -> int

        The derived class may implement this method to return the height of
        the specified item (in pixels).  The default implementation returns
        text height, as if this control was a normal combobox.
        """
        return _combo.OwnerDrawnComboBox_OnMeasureItem(*args, **kwargs)

    def OnMeasureItemWidth(*args, **kwargs):
        """
        OnMeasureItemWidth(self, size_t item) -> int

        The derived class may implement this method to return the width of the
        specified item (in pixels). If -1 is returned, then the item text
        width is used.  The default implementation returns -1.
        """
        return _combo.OwnerDrawnComboBox_OnMeasureItemWidth(*args, **kwargs)

    def OnDrawBackground(*args, **kwargs):
        """
        OnDrawBackground(self, DC dc, Rect rect, int item, int flags)

        This method is used to draw the items background and, maybe, a border
        around it.

        The base class version implements a reasonable default behaviour which
        consists in drawing the selected item with the standard background
        colour and drawing a border around the item if it is either selected
        or current.  ``flags`` has the sam meaning as with `OnDrawItem`.
        """
        return _combo.OwnerDrawnComboBox_OnDrawBackground(*args, **kwargs)

_combo.OwnerDrawnComboBox_swigregister(OwnerDrawnComboBox)

def PreOwnerDrawnComboBox(*args, **kwargs):
    """
    PreOwnerDrawnComboBox() -> OwnerDrawnComboBox

    2-phase create constructor.
    """
    val = _combo.new_PreOwnerDrawnComboBox(*args, **kwargs)
    return val

class BitmapComboBox(OwnerDrawnComboBox):
    """
    A combobox that displays a bitmap in front of the list items. It
    currently only allows using bitmaps of one size, and resizes itself so
    that a bitmap can be shown next to the text field.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxBitmapComboBoxNameStr) -> BitmapComboBox

        Standard constructor
        """
        _combo.BitmapComboBox_swiginit(self,_combo.new_BitmapComboBox(*args, **kwargs))
        self._setOORInfo(self);

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxBitmapComboBoxNameStr) -> bool

        Create the UI object, and other initialization.
        """
        return _combo.BitmapComboBox_Create(*args, **kwargs)

    def Append(*args, **kwargs):
        """
        Append(self, String item, Bitmap bitmap=wxNullBitmap, PyObject clientData=None) -> int

        Adds the item to the control, associating the given data with the item
        if not None.  The return value is the index of the newly added item.
        """
        return _combo.BitmapComboBox_Append(*args, **kwargs)

    def GetItemBitmap(*args, **kwargs):
        """
        GetItemBitmap(self, unsigned int n) -> Bitmap

        Returns the image of the item with the given index.
        """
        return _combo.BitmapComboBox_GetItemBitmap(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, String item, Bitmap bitmap, unsigned int pos, PyObject clientData=None) -> int

        Insert an item into the control before the item at the ``pos`` index,
        optionally associating some data object with the item.
        """
        return _combo.BitmapComboBox_Insert(*args, **kwargs)

    def SetItemBitmap(*args, **kwargs):
        """
        SetItemBitmap(self, unsigned int n, Bitmap bitmap)

        Sets the image for the given item.
        """
        return _combo.BitmapComboBox_SetItemBitmap(*args, **kwargs)

    def GetBitmapSize(*args, **kwargs):
        """
        GetBitmapSize(self) -> Size

        Returns size of the image used in list.
        """
        return _combo.BitmapComboBox_GetBitmapSize(*args, **kwargs)

_combo.BitmapComboBox_swigregister(BitmapComboBox)

def PreBitmapComboBox(*args, **kwargs):
    """
    PreBitmapComboBox() -> BitmapComboBox

    2-phase create constructor.
    """
    val = _combo.new_PreBitmapComboBox(*args, **kwargs)
    return val



