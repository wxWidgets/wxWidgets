# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _controls_
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

BU_LEFT = _controls_.BU_LEFT
BU_TOP = _controls_.BU_TOP
BU_RIGHT = _controls_.BU_RIGHT
BU_BOTTOM = _controls_.BU_BOTTOM
BU_ALIGN_MASK = _controls_.BU_ALIGN_MASK
BU_EXACTFIT = _controls_.BU_EXACTFIT
BU_AUTODRAW = _controls_.BU_AUTODRAW
class Button(_core.Control):
    """
    A button is a control that contains a text string, and is one of the most
    common elements of a GUI.  It may be placed on a dialog box or panel, or
    indeed almost any other window.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ButtonNameStr) -> Button

        Create and show a button.  The preferred way to create standard
        buttons is to use a standard ID and an empty label.  In this case
        wxWigets will automatically use a stock label that coresponds to the
        ID given.  In additon, the button will be decorated with stock icons
        under GTK+ 2.
        """
        _controls_.Button_swiginit(self,_controls_.new_Button(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ButtonNameStr) -> bool

        Acutally create the GUI Button for 2-phase creation.
        """
        return _controls_.Button_Create(*args, **kwargs)

    def SetDefault(*args, **kwargs):
        """
        SetDefault(self)

        This sets the button to be the default item for the panel or dialog box.
        """
        return _controls_.Button_SetDefault(*args, **kwargs)

    def GetDefaultSize(*args, **kwargs):
        """
        GetDefaultSize() -> Size

        Returns the default button size for this platform.
        """
        return _controls_.Button_GetDefaultSize(*args, **kwargs)

    GetDefaultSize = staticmethod(GetDefaultSize)
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
        return _controls_.Button_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.Button_swigregister(Button)
cvar = _controls_.cvar
ButtonNameStr = cvar.ButtonNameStr

def PreButton(*args, **kwargs):
    """
    PreButton() -> Button

    Precreate a Button for 2-phase creation.
    """
    val = _controls_.new_PreButton(*args, **kwargs)
    return val

def Button_GetDefaultSize(*args):
  """
    Button_GetDefaultSize() -> Size

    Returns the default button size for this platform.
    """
  return _controls_.Button_GetDefaultSize(*args)

def Button_GetClassDefaultAttributes(*args, **kwargs):
  """
    Button_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.Button_GetClassDefaultAttributes(*args, **kwargs)

class BitmapButton(Button):
    """
    A Button that contains a bitmap.  A bitmap button can be supplied with a
    single bitmap, and wxWidgets will draw all button states using this bitmap. If
    the application needs more control, additional bitmaps for the selected state,
    unpressed focused state, and greyed-out state may be supplied.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BU_AUTODRAW, Validator validator=DefaultValidator, 
            String name=ButtonNameStr) -> BitmapButton

        Create and show a button with a bitmap for the label.
        """
        _controls_.BitmapButton_swiginit(self,_controls_.new_BitmapButton(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BU_AUTODRAW, Validator validator=DefaultValidator, 
            String name=ButtonNameStr) -> bool

        Acutally create the GUI BitmapButton for 2-phase creation.
        """
        return _controls_.BitmapButton_Create(*args, **kwargs)

    def GetBitmapLabel(*args, **kwargs):
        """
        GetBitmapLabel(self) -> Bitmap

        Returns the label bitmap (the one passed to the constructor).
        """
        return _controls_.BitmapButton_GetBitmapLabel(*args, **kwargs)

    def GetBitmapDisabled(*args, **kwargs):
        """
        GetBitmapDisabled(self) -> Bitmap

        Returns the bitmap for the disabled state.
        """
        return _controls_.BitmapButton_GetBitmapDisabled(*args, **kwargs)

    def GetBitmapFocus(*args, **kwargs):
        """
        GetBitmapFocus(self) -> Bitmap

        Returns the bitmap for the focused state.
        """
        return _controls_.BitmapButton_GetBitmapFocus(*args, **kwargs)

    def GetBitmapSelected(*args, **kwargs):
        """
        GetBitmapSelected(self) -> Bitmap

        Returns the bitmap for the selected state.
        """
        return _controls_.BitmapButton_GetBitmapSelected(*args, **kwargs)

    def GetBitmapHover(*args, **kwargs):
        """
        GetBitmapHover(self) -> Bitmap

        Returns the bitmap used when the mouse is over the button, may be invalid.
        """
        return _controls_.BitmapButton_GetBitmapHover(*args, **kwargs)

    def SetBitmapDisabled(*args, **kwargs):
        """
        SetBitmapDisabled(self, Bitmap bitmap)

        Sets the bitmap for the disabled button appearance.
        """
        return _controls_.BitmapButton_SetBitmapDisabled(*args, **kwargs)

    def SetBitmapFocus(*args, **kwargs):
        """
        SetBitmapFocus(self, Bitmap bitmap)

        Sets the bitmap for the button appearance when it has the keyboard focus.
        """
        return _controls_.BitmapButton_SetBitmapFocus(*args, **kwargs)

    def SetBitmapSelected(*args, **kwargs):
        """
        SetBitmapSelected(self, Bitmap bitmap)

        Sets the bitmap for the selected (depressed) button appearance.
        """
        return _controls_.BitmapButton_SetBitmapSelected(*args, **kwargs)

    def SetBitmapLabel(*args, **kwargs):
        """
        SetBitmapLabel(self, Bitmap bitmap)

        Sets the bitmap label for the button.  This is the bitmap used for the
        unselected state, and for all other states if no other bitmaps are provided.
        """
        return _controls_.BitmapButton_SetBitmapLabel(*args, **kwargs)

    def SetBitmapHover(*args, **kwargs):
        """
        SetBitmapHover(self, Bitmap hover)

        Sets the bitmap to be shown when the mouse is over the button.  This function
        is new since wxWidgets version 2.7.0 and the hover bitmap is currently only
        supported in wxMSW.
        """
        return _controls_.BitmapButton_SetBitmapHover(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(self, int x, int y)"""
        return _controls_.BitmapButton_SetMargins(*args, **kwargs)

    def GetMarginX(*args, **kwargs):
        """GetMarginX(self) -> int"""
        return _controls_.BitmapButton_GetMarginX(*args, **kwargs)

    def GetMarginY(*args, **kwargs):
        """GetMarginY(self) -> int"""
        return _controls_.BitmapButton_GetMarginY(*args, **kwargs)

    BitmapDisabled = property(GetBitmapDisabled,SetBitmapDisabled,doc="See `GetBitmapDisabled` and `SetBitmapDisabled`") 
    BitmapFocus = property(GetBitmapFocus,SetBitmapFocus,doc="See `GetBitmapFocus` and `SetBitmapFocus`") 
    BitmapHover = property(GetBitmapHover,SetBitmapHover,doc="See `GetBitmapHover` and `SetBitmapHover`") 
    BitmapLabel = property(GetBitmapLabel,SetBitmapLabel,doc="See `GetBitmapLabel` and `SetBitmapLabel`") 
    BitmapSelected = property(GetBitmapSelected,SetBitmapSelected,doc="See `GetBitmapSelected` and `SetBitmapSelected`") 
    MarginX = property(GetMarginX,doc="See `GetMarginX`") 
    MarginY = property(GetMarginY,doc="See `GetMarginY`") 
_controls_.BitmapButton_swigregister(BitmapButton)

def PreBitmapButton(*args, **kwargs):
    """
    PreBitmapButton() -> BitmapButton

    Precreate a BitmapButton for 2-phase creation.
    """
    val = _controls_.new_PreBitmapButton(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

CHK_2STATE = _controls_.CHK_2STATE
CHK_3STATE = _controls_.CHK_3STATE
CHK_ALLOW_3RD_STATE_FOR_USER = _controls_.CHK_ALLOW_3RD_STATE_FOR_USER
CHK_UNCHECKED = _controls_.CHK_UNCHECKED
CHK_CHECKED = _controls_.CHK_CHECKED
CHK_UNDETERMINED = _controls_.CHK_UNDETERMINED
class CheckBox(_core.Control):
    """
    A checkbox is a labelled box which by default is either on (the
    checkmark is visible) or off (no checkmark). Optionally (When the
    wx.CHK_3STATE style flag is set) it can have a third state, called the
    mixed or undetermined state. Often this is used as a "Does Not
    Apply" state.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=CheckBoxNameStr) -> CheckBox

        Creates and shows a CheckBox control
        """
        _controls_.CheckBox_swiginit(self,_controls_.new_CheckBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=CheckBoxNameStr) -> bool

        Actually create the GUI CheckBox for 2-phase creation.
        """
        return _controls_.CheckBox_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """
        GetValue(self) -> bool

        Gets the state of a 2-state CheckBox.  Returns True if it is checked,
        False otherwise.
        """
        return _controls_.CheckBox_GetValue(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """
        IsChecked(self) -> bool

        Similar to GetValue, but raises an exception if it is not a 2-state
        CheckBox.
        """
        return _controls_.CheckBox_IsChecked(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """
        SetValue(self, bool state)

        Set the state of a 2-state CheckBox.  Pass True for checked, False for
        unchecked.
        """
        return _controls_.CheckBox_SetValue(*args, **kwargs)

    def Get3StateValue(*args, **kwargs):
        """
        Get3StateValue(self) -> int

        Returns wx.CHK_UNCHECKED when the CheckBox is unchecked,
        wx.CHK_CHECKED when it is checked and wx.CHK_UNDETERMINED when it's in
        the undetermined state.  Raises an exceptiion when the function is
        used with a 2-state CheckBox.
        """
        return _controls_.CheckBox_Get3StateValue(*args, **kwargs)

    def Set3StateValue(*args, **kwargs):
        """
        Set3StateValue(self, int state)

        Sets the CheckBox to the given state.  The state parameter can be one
        of the following: wx.CHK_UNCHECKED (Check is off), wx.CHK_CHECKED (the
        Check is on) or wx.CHK_UNDETERMINED (Check is mixed). Raises an
        exception when the CheckBox is a 2-state checkbox and setting the
        state to wx.CHK_UNDETERMINED.
        """
        return _controls_.CheckBox_Set3StateValue(*args, **kwargs)

    def Is3State(*args, **kwargs):
        """
        Is3State(self) -> bool

        Returns whether or not the CheckBox is a 3-state CheckBox.
        """
        return _controls_.CheckBox_Is3State(*args, **kwargs)

    def Is3rdStateAllowedForUser(*args, **kwargs):
        """
        Is3rdStateAllowedForUser(self) -> bool

        Returns whether or not the user can set the CheckBox to the third
        state.
        """
        return _controls_.CheckBox_Is3rdStateAllowedForUser(*args, **kwargs)

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
        return _controls_.CheckBox_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    ThreeStateValue = property(Get3StateValue,Set3StateValue,doc="See `Get3StateValue` and `Set3StateValue`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.CheckBox_swigregister(CheckBox)
CheckBoxNameStr = cvar.CheckBoxNameStr

def PreCheckBox(*args, **kwargs):
    """
    PreCheckBox() -> CheckBox

    Precreate a CheckBox for 2-phase creation.
    """
    val = _controls_.new_PreCheckBox(*args, **kwargs)
    return val

def CheckBox_GetClassDefaultAttributes(*args, **kwargs):
  """
    CheckBox_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.CheckBox_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class Choice(_core.ControlWithItems):
    """
    A Choice control is used to select one of a list of strings.
    Unlike a `wx.ListBox`, only the selection is visible until the
    user pulls down the menu of choices.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
            String name=ChoiceNameStr) -> Choice

        Create and show a Choice control
        """
        _controls_.Choice_swiginit(self,_controls_.new_Choice(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
            String name=ChoiceNameStr) -> bool

        Actually create the GUI Choice control for 2-phase creation
        """
        return _controls_.Choice_Create(*args, **kwargs)

    def GetCurrentSelection(*args, **kwargs):
        """
        GetCurrentSelection(self) -> int

        Unlike `GetSelection` which only returns the accepted selection value,
        i.e. the selection in the control once the user closes the dropdown
        list, this function returns the current selection.  That is, while the
        dropdown list is shown, it returns the currently selected item in
        it. When it is not shown, its result is the same as for the other
        function.
        """
        return _controls_.Choice_GetCurrentSelection(*args, **kwargs)

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
        return _controls_.Choice_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    CurrentSelection = property(GetCurrentSelection,doc="See `GetCurrentSelection`") 
_controls_.Choice_swigregister(Choice)
ChoiceNameStr = cvar.ChoiceNameStr

def PreChoice(*args, **kwargs):
    """
    PreChoice() -> Choice

    Precreate a Choice control for 2-phase creation.
    """
    val = _controls_.new_PreChoice(*args, **kwargs)
    return val

def Choice_GetClassDefaultAttributes(*args, **kwargs):
  """
    Choice_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.Choice_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class ComboBox(_core.Control,_core.ItemContainer):
    """
    A combobox is like a combination of an edit control and a
    listbox. It can be displayed as static list with editable or
    read-only text field; or a drop-down list with text field.

    A combobox permits a single selection only. Combobox items are
    numbered from zero.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(Window parent, int id, String value=EmptyString,
            Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
            String name=ComboBoxNameStr) -> ComboBox

        Constructor, creates and shows a ComboBox control.
        """
        _controls_.ComboBox_swiginit(self,_controls_.new_ComboBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, String value=EmptyString,
            Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
            String name=ChoiceNameStr) -> bool

        Actually create the GUI wxComboBox control for 2-phase creation
        """
        return _controls_.ComboBox_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """
        GetValue(self) -> String

        Returns the current value in the combobox text field.
        """
        return _controls_.ComboBox_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, String value)"""
        return _controls_.ComboBox_SetValue(*args, **kwargs)

    def Copy(*args, **kwargs):
        """
        Copy(self)

        Copies the selected text to the clipboard.
        """
        return _controls_.ComboBox_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """
        Cut(self)

        Copies the selected text to the clipboard and removes the selection.
        """
        return _controls_.ComboBox_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """
        Paste(self)

        Pastes text from the clipboard to the text field.
        """
        return _controls_.ComboBox_Paste(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """
        SetInsertionPoint(self, long pos)

        Sets the insertion point in the combobox text field.
        """
        return _controls_.ComboBox_SetInsertionPoint(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """
        GetInsertionPoint(self) -> long

        Returns the insertion point for the combobox's text field.
        """
        return _controls_.ComboBox_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """
        GetLastPosition(self) -> long

        Returns the last position in the combobox text field.
        """
        return _controls_.ComboBox_GetLastPosition(*args, **kwargs)

    def Replace(*args, **kwargs):
        """
        Replace(self, long from, long to, String value)

        Replaces the text between two positions with the given text, in the
        combobox text field.
        """
        return _controls_.ComboBox_Replace(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """
        SetMark(self, long from, long to)

        Selects the text between the two positions in the combobox text field.
        """
        return _controls_.ComboBox_SetMark(*args, **kwargs)

    def GetMark(self):
        return (0,0)

    def GetCurrentSelection(*args, **kwargs):
        """
        GetCurrentSelection(self) -> int

        Unlike `GetSelection` which only returns the accepted selection value,
        i.e. the selection in the control once the user closes the dropdown
        list, this function returns the current selection.  That is, while the
        dropdown list is shown, it returns the currently selected item in
        it. When it is not shown, its result is the same as for the other
        function.
        """
        return _controls_.ComboBox_GetCurrentSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """
        SetStringSelection(self, String string) -> bool

        Select the item with the specifed string
        """
        return _controls_.ComboBox_SetStringSelection(*args, **kwargs)

    def SetEditable(*args, **kwargs):
        """SetEditable(self, bool editable)"""
        return _controls_.ComboBox_SetEditable(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """
        SetInsertionPointEnd(self)

        Sets the insertion point at the end of the combobox text field.
        """
        return _controls_.ComboBox_SetInsertionPointEnd(*args, **kwargs)

    def Remove(*args, **kwargs):
        """
        Remove(self, long from, long to)

        Removes the text between the two positions in the combobox text field.
        """
        return _controls_.ComboBox_Remove(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """
        IsEditable(self) -> bool

        Returns True if the combo is ediatable (not read-only.)
        """
        return _controls_.ComboBox_IsEditable(*args, **kwargs)

    def Undo(*args, **kwargs):
        """
        Undo(self)

        Redoes the last undo in the text field. Windows only.
        """
        return _controls_.ComboBox_Undo(*args, **kwargs)

    def Redo(*args, **kwargs):
        """
        Redo(self)

        Undoes the last edit in the text field. Windows only.
        """
        return _controls_.ComboBox_Redo(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """
        SelectAll(self)

        Select all the text in the combo's text field.
        """
        return _controls_.ComboBox_SelectAll(*args, **kwargs)

    def CanCopy(*args, **kwargs):
        """
        CanCopy(self) -> bool

        Returns True if the combobox is editable and there is a text selection
        to copy to the clipboard.  Only available on Windows.
        """
        return _controls_.ComboBox_CanCopy(*args, **kwargs)

    def CanCut(*args, **kwargs):
        """
        CanCut(self) -> bool

        Returns True if the combobox is editable and there is a text selection
        to copy to the clipboard.  Only available on Windows.
        """
        return _controls_.ComboBox_CanCut(*args, **kwargs)

    def CanPaste(*args, **kwargs):
        """
        CanPaste(self) -> bool

        Returns True if the combobox is editable and there is text on the
        clipboard that can be pasted into the text field. Only available on
        Windows.
        """
        return _controls_.ComboBox_CanPaste(*args, **kwargs)

    def CanUndo(*args, **kwargs):
        """
        CanUndo(self) -> bool

        Returns True if the combobox is editable and the last edit can be
        undone.  Only available on Windows.
        """
        return _controls_.ComboBox_CanUndo(*args, **kwargs)

    def CanRedo(*args, **kwargs):
        """
        CanRedo(self) -> bool

        Returns True if the combobox is editable and the last undo can be
        redone.  Only available on Windows.
        """
        return _controls_.ComboBox_CanRedo(*args, **kwargs)

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
        return _controls_.ComboBox_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    CurrentSelection = property(GetCurrentSelection,doc="See `GetCurrentSelection`") 
    InsertionPoint = property(GetInsertionPoint,SetInsertionPoint,doc="See `GetInsertionPoint` and `SetInsertionPoint`") 
    LastPosition = property(GetLastPosition,doc="See `GetLastPosition`") 
    Mark = property(GetMark,SetMark,doc="See `GetMark` and `SetMark`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.ComboBox_swigregister(ComboBox)
ComboBoxNameStr = cvar.ComboBoxNameStr

def PreComboBox(*args, **kwargs):
    """
    PreComboBox() -> ComboBox

    Precreate a ComboBox control for 2-phase creation.
    """
    val = _controls_.new_PreComboBox(*args, **kwargs)
    return val

def ComboBox_GetClassDefaultAttributes(*args, **kwargs):
  """
    ComboBox_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ComboBox_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

GA_HORIZONTAL = _controls_.GA_HORIZONTAL
GA_VERTICAL = _controls_.GA_VERTICAL
GA_SMOOTH = _controls_.GA_SMOOTH
GA_PROGRESSBAR = 0 # obsolete 
class Gauge(_core.Control):
    """Proxy of C++ Gauge class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, int range=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=GA_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=GaugeNameStr) -> Gauge
        """
        _controls_.Gauge_swiginit(self,_controls_.new_Gauge(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, int range=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=GA_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=GaugeNameStr) -> bool
        """
        return _controls_.Gauge_Create(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, int range)"""
        return _controls_.Gauge_SetRange(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self) -> int"""
        return _controls_.Gauge_GetRange(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int pos)"""
        return _controls_.Gauge_SetValue(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return _controls_.Gauge_GetValue(*args, **kwargs)

    def Pulse(*args, **kwargs):
        """Pulse(self)"""
        return _controls_.Gauge_Pulse(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.Gauge_IsVertical(*args, **kwargs)

    def SetShadowWidth(*args, **kwargs):
        """SetShadowWidth(self, int w)"""
        return _controls_.Gauge_SetShadowWidth(*args, **kwargs)

    def GetShadowWidth(*args, **kwargs):
        """GetShadowWidth(self) -> int"""
        return _controls_.Gauge_GetShadowWidth(*args, **kwargs)

    def SetBezelFace(*args, **kwargs):
        """SetBezelFace(self, int w)"""
        return _controls_.Gauge_SetBezelFace(*args, **kwargs)

    def GetBezelFace(*args, **kwargs):
        """GetBezelFace(self) -> int"""
        return _controls_.Gauge_GetBezelFace(*args, **kwargs)

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
        return _controls_.Gauge_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    BezelFace = property(GetBezelFace,SetBezelFace,doc="See `GetBezelFace` and `SetBezelFace`") 
    Range = property(GetRange,SetRange,doc="See `GetRange` and `SetRange`") 
    ShadowWidth = property(GetShadowWidth,SetShadowWidth,doc="See `GetShadowWidth` and `SetShadowWidth`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.Gauge_swigregister(Gauge)
GaugeNameStr = cvar.GaugeNameStr

def PreGauge(*args, **kwargs):
    """PreGauge() -> Gauge"""
    val = _controls_.new_PreGauge(*args, **kwargs)
    return val

def Gauge_GetClassDefaultAttributes(*args, **kwargs):
  """
    Gauge_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.Gauge_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class StaticBox(_core.Control):
    """Proxy of C++ StaticBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBoxNameStr) -> StaticBox
        """
        _controls_.StaticBox_swiginit(self,_controls_.new_StaticBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBoxNameStr) -> bool
        """
        return _controls_.StaticBox_Create(*args, **kwargs)

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
        return _controls_.StaticBox_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.StaticBox_swigregister(StaticBox)
StaticBitmapNameStr = cvar.StaticBitmapNameStr
StaticBoxNameStr = cvar.StaticBoxNameStr
StaticTextNameStr = cvar.StaticTextNameStr
StaticLineNameStr = cvar.StaticLineNameStr

def PreStaticBox(*args, **kwargs):
    """PreStaticBox() -> StaticBox"""
    val = _controls_.new_PreStaticBox(*args, **kwargs)
    return val

def StaticBox_GetClassDefaultAttributes(*args, **kwargs):
  """
    StaticBox_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.StaticBox_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class StaticLine(_core.Control):
    """Proxy of C++ StaticLine class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LI_HORIZONTAL, 
            String name=StaticLineNameStr) -> StaticLine
        """
        _controls_.StaticLine_swiginit(self,_controls_.new_StaticLine(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LI_HORIZONTAL, 
            String name=StaticLineNameStr) -> bool
        """
        return _controls_.StaticLine_Create(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.StaticLine_IsVertical(*args, **kwargs)

    def GetDefaultSize(*args, **kwargs):
        """GetDefaultSize() -> int"""
        return _controls_.StaticLine_GetDefaultSize(*args, **kwargs)

    GetDefaultSize = staticmethod(GetDefaultSize)
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
        return _controls_.StaticLine_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.StaticLine_swigregister(StaticLine)

def PreStaticLine(*args, **kwargs):
    """PreStaticLine() -> StaticLine"""
    val = _controls_.new_PreStaticLine(*args, **kwargs)
    return val

def StaticLine_GetDefaultSize(*args):
  """StaticLine_GetDefaultSize() -> int"""
  return _controls_.StaticLine_GetDefaultSize(*args)

def StaticLine_GetClassDefaultAttributes(*args, **kwargs):
  """
    StaticLine_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.StaticLine_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class StaticText(_core.Control):
    """Proxy of C++ StaticText class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticTextNameStr) -> StaticText
        """
        _controls_.StaticText_swiginit(self,_controls_.new_StaticText(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticTextNameStr) -> bool
        """
        return _controls_.StaticText_Create(*args, **kwargs)

    def Wrap(*args, **kwargs):
        """
        Wrap(self, int width)

        This functions wraps the control's label so that each of its lines
        becomes at most ``width`` pixels wide if possible (the lines are
        broken at words boundaries so it might not be the case if words are
        too long). If ``width`` is negative, no wrapping is done.
        """
        return _controls_.StaticText_Wrap(*args, **kwargs)

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
        return _controls_.StaticText_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.StaticText_swigregister(StaticText)

def PreStaticText(*args, **kwargs):
    """PreStaticText() -> StaticText"""
    val = _controls_.new_PreStaticText(*args, **kwargs)
    return val

def StaticText_GetClassDefaultAttributes(*args, **kwargs):
  """
    StaticText_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.StaticText_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class StaticBitmap(_core.Control):
    """Proxy of C++ StaticBitmap class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBitmapNameStr) -> StaticBitmap
        """
        _controls_.StaticBitmap_swiginit(self,_controls_.new_StaticBitmap(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBitmapNameStr) -> bool
        """
        return _controls_.StaticBitmap_Create(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _controls_.StaticBitmap_GetBitmap(*args, **kwargs)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(self, Bitmap bitmap)"""
        return _controls_.StaticBitmap_SetBitmap(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(self, Icon icon)"""
        return _controls_.StaticBitmap_SetIcon(*args, **kwargs)

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
        return _controls_.StaticBitmap_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.StaticBitmap_swigregister(StaticBitmap)

def PreStaticBitmap(*args, **kwargs):
    """PreStaticBitmap() -> StaticBitmap"""
    val = _controls_.new_PreStaticBitmap(*args, **kwargs)
    return val

def StaticBitmap_GetClassDefaultAttributes(*args, **kwargs):
  """
    StaticBitmap_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.StaticBitmap_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class ListBox(_core.ControlWithItems):
    """Proxy of C++ ListBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> ListBox
        """
        _controls_.ListBox_swiginit(self,_controls_.new_ListBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> bool
        """
        return _controls_.ListBox_Create(*args, **kwargs)

    def Insert(*args, **kwargs):
        """
        Insert(self, String item, int pos, PyObject clientData=None)

        Insert an item into the control before the item at the ``pos`` index,
        optionally associating some data object with the item.
        """
        return _controls_.ListBox_Insert(*args, **kwargs)

    def InsertItems(*args, **kwargs):
        """InsertItems(self, wxArrayString items, unsigned int pos)"""
        return _controls_.ListBox_InsertItems(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, wxArrayString items)"""
        return _controls_.ListBox_Set(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(self, int n) -> bool"""
        return _controls_.ListBox_IsSelected(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int n, bool select=True)"""
        return _controls_.ListBox_SetSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """
        Select(self, int n)

        This is the same as `SetSelection` and exists only because it is
        slightly more natural for controls which support multiple selection.
        """
        return _controls_.ListBox_Select(*args, **kwargs)

    def Deselect(*args, **kwargs):
        """Deselect(self, int n)"""
        return _controls_.ListBox_Deselect(*args, **kwargs)

    def DeselectAll(*args, **kwargs):
        """DeselectAll(self, int itemToLeaveSelected=-1)"""
        return _controls_.ListBox_DeselectAll(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(self, String s, bool select=True) -> bool"""
        return _controls_.ListBox_SetStringSelection(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections(self) -> PyObject"""
        return _controls_.ListBox_GetSelections(*args, **kwargs)

    def SetFirstItem(*args, **kwargs):
        """SetFirstItem(self, int n)"""
        return _controls_.ListBox_SetFirstItem(*args, **kwargs)

    def SetFirstItemStr(*args, **kwargs):
        """SetFirstItemStr(self, String s)"""
        return _controls_.ListBox_SetFirstItemStr(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(self, int n)"""
        return _controls_.ListBox_EnsureVisible(*args, **kwargs)

    def AppendAndEnsureVisible(*args, **kwargs):
        """AppendAndEnsureVisible(self, String s)"""
        return _controls_.ListBox_AppendAndEnsureVisible(*args, **kwargs)

    def IsSorted(*args, **kwargs):
        """IsSorted(self) -> bool"""
        return _controls_.ListBox_IsSorted(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, Point pt) -> int

        Test where the given (in client coords) point lies
        """
        return _controls_.ListBox_HitTest(*args, **kwargs)

    def SetItemForegroundColour(*args, **kwargs):
        """SetItemForegroundColour(self, int item, Colour c)"""
        return _controls_.ListBox_SetItemForegroundColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(self, int item, Colour c)"""
        return _controls_.ListBox_SetItemBackgroundColour(*args, **kwargs)

    def SetItemFont(*args, **kwargs):
        """SetItemFont(self, int item, Font f)"""
        return _controls_.ListBox_SetItemFont(*args, **kwargs)

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
        return _controls_.ListBox_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    Selections = property(GetSelections,doc="See `GetSelections`") 
_controls_.ListBox_swigregister(ListBox)
ListBoxNameStr = cvar.ListBoxNameStr

def PreListBox(*args, **kwargs):
    """PreListBox() -> ListBox"""
    val = _controls_.new_PreListBox(*args, **kwargs)
    return val

def ListBox_GetClassDefaultAttributes(*args, **kwargs):
  """
    ListBox_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ListBox_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class CheckListBox(ListBox):
    """Proxy of C++ CheckListBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> CheckListBox
        """
        _controls_.CheckListBox_swiginit(self,_controls_.new_CheckListBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> bool
        """
        return _controls_.CheckListBox_Create(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(self, unsigned int index) -> bool"""
        return _controls_.CheckListBox_IsChecked(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, unsigned int index, int check=True)"""
        return _controls_.CheckListBox_Check(*args, **kwargs)

    def GetItemHeight(self):
        raise NotImplementedError

    ItemHeight = property(GetItemHeight,doc="See `GetItemHeight`") 
_controls_.CheckListBox_swigregister(CheckListBox)

def PreCheckListBox(*args, **kwargs):
    """PreCheckListBox() -> CheckListBox"""
    val = _controls_.new_PreCheckListBox(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

TE_NO_VSCROLL = _controls_.TE_NO_VSCROLL
TE_AUTO_SCROLL = _controls_.TE_AUTO_SCROLL
TE_READONLY = _controls_.TE_READONLY
TE_MULTILINE = _controls_.TE_MULTILINE
TE_PROCESS_TAB = _controls_.TE_PROCESS_TAB
TE_LEFT = _controls_.TE_LEFT
TE_CENTER = _controls_.TE_CENTER
TE_RIGHT = _controls_.TE_RIGHT
TE_CENTRE = _controls_.TE_CENTRE
TE_RICH = _controls_.TE_RICH
TE_PROCESS_ENTER = _controls_.TE_PROCESS_ENTER
TE_PASSWORD = _controls_.TE_PASSWORD
TE_AUTO_URL = _controls_.TE_AUTO_URL
TE_NOHIDESEL = _controls_.TE_NOHIDESEL
TE_DONTWRAP = _controls_.TE_DONTWRAP
TE_CHARWRAP = _controls_.TE_CHARWRAP
TE_WORDWRAP = _controls_.TE_WORDWRAP
TE_BESTWRAP = _controls_.TE_BESTWRAP
TE_RICH2 = _controls_.TE_RICH2
TE_CAPITALIZE = _controls_.TE_CAPITALIZE
TE_LINEWRAP = TE_CHARWRAP 
TEXT_ALIGNMENT_DEFAULT = _controls_.TEXT_ALIGNMENT_DEFAULT
TEXT_ALIGNMENT_LEFT = _controls_.TEXT_ALIGNMENT_LEFT
TEXT_ALIGNMENT_CENTRE = _controls_.TEXT_ALIGNMENT_CENTRE
TEXT_ALIGNMENT_CENTER = _controls_.TEXT_ALIGNMENT_CENTER
TEXT_ALIGNMENT_RIGHT = _controls_.TEXT_ALIGNMENT_RIGHT
TEXT_ALIGNMENT_JUSTIFIED = _controls_.TEXT_ALIGNMENT_JUSTIFIED
TEXT_ATTR_TEXT_COLOUR = _controls_.TEXT_ATTR_TEXT_COLOUR
TEXT_ATTR_BACKGROUND_COLOUR = _controls_.TEXT_ATTR_BACKGROUND_COLOUR
TEXT_ATTR_FONT_FACE = _controls_.TEXT_ATTR_FONT_FACE
TEXT_ATTR_FONT_SIZE = _controls_.TEXT_ATTR_FONT_SIZE
TEXT_ATTR_FONT_WEIGHT = _controls_.TEXT_ATTR_FONT_WEIGHT
TEXT_ATTR_FONT_ITALIC = _controls_.TEXT_ATTR_FONT_ITALIC
TEXT_ATTR_FONT_UNDERLINE = _controls_.TEXT_ATTR_FONT_UNDERLINE
TEXT_ATTR_FONT = _controls_.TEXT_ATTR_FONT
TEXT_ATTR_ALIGNMENT = _controls_.TEXT_ATTR_ALIGNMENT
TEXT_ATTR_LEFT_INDENT = _controls_.TEXT_ATTR_LEFT_INDENT
TEXT_ATTR_RIGHT_INDENT = _controls_.TEXT_ATTR_RIGHT_INDENT
TEXT_ATTR_TABS = _controls_.TEXT_ATTR_TABS
TE_HT_UNKNOWN = _controls_.TE_HT_UNKNOWN
TE_HT_BEFORE = _controls_.TE_HT_BEFORE
TE_HT_ON_TEXT = _controls_.TE_HT_ON_TEXT
TE_HT_BELOW = _controls_.TE_HT_BELOW
TE_HT_BEYOND = _controls_.TE_HT_BEYOND
OutOfRangeTextCoord = _controls_.OutOfRangeTextCoord
InvalidTextCoord = _controls_.InvalidTextCoord
TEXT_TYPE_ANY = _controls_.TEXT_TYPE_ANY
class TextAttr(object):
    """Proxy of C++ TextAttr class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            Font font=wxNullFont, int alignment=TEXT_ALIGNMENT_DEFAULT) -> TextAttr
        """
        _controls_.TextAttr_swiginit(self,_controls_.new_TextAttr(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_TextAttr
    __del__ = lambda self : None;
    def Init(*args, **kwargs):
        """Init(self)"""
        return _controls_.TextAttr_Init(*args, **kwargs)

    def Merge(*args, **kwargs):
        """Merge(TextAttr base, TextAttr overlay) -> TextAttr"""
        return _controls_.TextAttr_Merge(*args, **kwargs)

    Merge = staticmethod(Merge)
    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _controls_.TextAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _controls_.TextAttr_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font, long flags=TEXT_ATTR_FONT)"""
        return _controls_.TextAttr_SetFont(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(self, int alignment)"""
        return _controls_.TextAttr_SetAlignment(*args, **kwargs)

    def SetTabs(*args, **kwargs):
        """SetTabs(self, wxArrayInt tabs)"""
        return _controls_.TextAttr_SetTabs(*args, **kwargs)

    def SetLeftIndent(*args, **kwargs):
        """SetLeftIndent(self, int indent, int subIndent=0)"""
        return _controls_.TextAttr_SetLeftIndent(*args, **kwargs)

    def SetRightIndent(*args, **kwargs):
        """SetRightIndent(self, int indent)"""
        return _controls_.TextAttr_SetRightIndent(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(self, long flags)"""
        return _controls_.TextAttr_SetFlags(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour(self) -> bool"""
        return _controls_.TextAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour(self) -> bool"""
        return _controls_.TextAttr_HasBackgroundColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont(self) -> bool"""
        return _controls_.TextAttr_HasFont(*args, **kwargs)

    def HasAlignment(*args, **kwargs):
        """HasAlignment(self) -> bool"""
        return _controls_.TextAttr_HasAlignment(*args, **kwargs)

    def HasTabs(*args, **kwargs):
        """HasTabs(self) -> bool"""
        return _controls_.TextAttr_HasTabs(*args, **kwargs)

    def HasLeftIndent(*args, **kwargs):
        """HasLeftIndent(self) -> bool"""
        return _controls_.TextAttr_HasLeftIndent(*args, **kwargs)

    def HasRightIndent(*args, **kwargs):
        """HasRightIndent(self) -> bool"""
        return _controls_.TextAttr_HasRightIndent(*args, **kwargs)

    def HasFlag(*args, **kwargs):
        """HasFlag(self, long flag) -> bool"""
        return _controls_.TextAttr_HasFlag(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _controls_.TextAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _controls_.TextAttr_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _controls_.TextAttr_GetFont(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment(self) -> int"""
        return _controls_.TextAttr_GetAlignment(*args, **kwargs)

    def GetTabs(*args, **kwargs):
        """GetTabs(self) -> wxArrayInt"""
        return _controls_.TextAttr_GetTabs(*args, **kwargs)

    def GetLeftIndent(*args, **kwargs):
        """GetLeftIndent(self) -> long"""
        return _controls_.TextAttr_GetLeftIndent(*args, **kwargs)

    def GetLeftSubIndent(*args, **kwargs):
        """GetLeftSubIndent(self) -> long"""
        return _controls_.TextAttr_GetLeftSubIndent(*args, **kwargs)

    def GetRightIndent(*args, **kwargs):
        """GetRightIndent(self) -> long"""
        return _controls_.TextAttr_GetRightIndent(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags(self) -> long"""
        return _controls_.TextAttr_GetFlags(*args, **kwargs)

    def IsDefault(*args, **kwargs):
        """IsDefault(self) -> bool"""
        return _controls_.TextAttr_IsDefault(*args, **kwargs)

    def Combine(*args, **kwargs):
        """Combine(TextAttr attr, TextAttr attrDef, TextCtrl text) -> TextAttr"""
        return _controls_.TextAttr_Combine(*args, **kwargs)

    Combine = staticmethod(Combine)
    Alignment = property(GetAlignment,SetAlignment,doc="See `GetAlignment` and `SetAlignment`") 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    Flags = property(GetFlags,SetFlags,doc="See `GetFlags` and `SetFlags`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    LeftIndent = property(GetLeftIndent,SetLeftIndent,doc="See `GetLeftIndent` and `SetLeftIndent`") 
    LeftSubIndent = property(GetLeftSubIndent,doc="See `GetLeftSubIndent`") 
    RightIndent = property(GetRightIndent,SetRightIndent,doc="See `GetRightIndent` and `SetRightIndent`") 
    Tabs = property(GetTabs,SetTabs,doc="See `GetTabs` and `SetTabs`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
_controls_.TextAttr_swigregister(TextAttr)
TextCtrlNameStr = cvar.TextCtrlNameStr

def TextAttr_Merge(*args, **kwargs):
  """TextAttr_Merge(TextAttr base, TextAttr overlay) -> TextAttr"""
  return _controls_.TextAttr_Merge(*args, **kwargs)

def TextAttr_Combine(*args, **kwargs):
  """TextAttr_Combine(TextAttr attr, TextAttr attrDef, TextCtrl text) -> TextAttr"""
  return _controls_.TextAttr_Combine(*args, **kwargs)

class TextCtrl(_core.Control):
    """Proxy of C++ TextCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=TextCtrlNameStr) -> TextCtrl
        """
        _controls_.TextCtrl_swiginit(self,_controls_.new_TextCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=TextCtrlNameStr) -> bool
        """
        return _controls_.TextCtrl_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _controls_.TextCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, String value)"""
        return _controls_.TextCtrl_SetValue(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return _controls_.TextCtrl_IsEmpty(*args, **kwargs)

    def ChangeValue(*args, **kwargs):
        """ChangeValue(self, String value)"""
        return _controls_.TextCtrl_ChangeValue(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self, long from, long to) -> String"""
        return _controls_.TextCtrl_GetRange(*args, **kwargs)

    def GetLineLength(*args, **kwargs):
        """GetLineLength(self, long lineNo) -> int"""
        return _controls_.TextCtrl_GetLineLength(*args, **kwargs)

    def GetLineText(*args, **kwargs):
        """GetLineText(self, long lineNo) -> String"""
        return _controls_.TextCtrl_GetLineText(*args, **kwargs)

    def GetNumberOfLines(*args, **kwargs):
        """GetNumberOfLines(self) -> int"""
        return _controls_.TextCtrl_GetNumberOfLines(*args, **kwargs)

    def IsModified(*args, **kwargs):
        """IsModified(self) -> bool"""
        return _controls_.TextCtrl_IsModified(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """IsEditable(self) -> bool"""
        return _controls_.TextCtrl_IsEditable(*args, **kwargs)

    def IsSingleLine(*args, **kwargs):
        """IsSingleLine(self) -> bool"""
        return _controls_.TextCtrl_IsSingleLine(*args, **kwargs)

    def IsMultiLine(*args, **kwargs):
        """IsMultiLine(self) -> bool"""
        return _controls_.TextCtrl_IsMultiLine(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """
        GetSelection() -> (from, to)

        If the return values from and to are the same, there is no selection.
        """
        return _controls_.TextCtrl_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection(self) -> String"""
        return _controls_.TextCtrl_GetStringSelection(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _controls_.TextCtrl_Clear(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, long from, long to, String value)"""
        return _controls_.TextCtrl_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, long from, long to)"""
        return _controls_.TextCtrl_Remove(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(self, String file, int fileType=TEXT_TYPE_ANY) -> bool"""
        return _controls_.TextCtrl_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, String file=EmptyString, int fileType=TEXT_TYPE_ANY) -> bool"""
        return _controls_.TextCtrl_SaveFile(*args, **kwargs)

    def MarkDirty(*args, **kwargs):
        """MarkDirty(self)"""
        return _controls_.TextCtrl_MarkDirty(*args, **kwargs)

    def DiscardEdits(*args, **kwargs):
        """DiscardEdits(self)"""
        return _controls_.TextCtrl_DiscardEdits(*args, **kwargs)

    def SetModified(*args, **kwargs):
        """SetModified(self, bool modified)"""
        return _controls_.TextCtrl_SetModified(*args, **kwargs)

    def SetMaxLength(*args, **kwargs):
        """SetMaxLength(self, unsigned long len)"""
        return _controls_.TextCtrl_SetMaxLength(*args, **kwargs)

    def WriteText(*args, **kwargs):
        """WriteText(self, String text)"""
        return _controls_.TextCtrl_WriteText(*args, **kwargs)

    def AppendText(*args, **kwargs):
        """AppendText(self, String text)"""
        return _controls_.TextCtrl_AppendText(*args, **kwargs)

    def EmulateKeyPress(*args, **kwargs):
        """EmulateKeyPress(self, KeyEvent event) -> bool"""
        return _controls_.TextCtrl_EmulateKeyPress(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, long start, long end, TextAttr style) -> bool"""
        return _controls_.TextCtrl_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self, long position, TextAttr style) -> bool"""
        return _controls_.TextCtrl_GetStyle(*args, **kwargs)

    def SetDefaultStyle(*args, **kwargs):
        """SetDefaultStyle(self, TextAttr style) -> bool"""
        return _controls_.TextCtrl_SetDefaultStyle(*args, **kwargs)

    def GetDefaultStyle(*args, **kwargs):
        """GetDefaultStyle(self) -> TextAttr"""
        return _controls_.TextCtrl_GetDefaultStyle(*args, **kwargs)

    def XYToPosition(*args, **kwargs):
        """XYToPosition(self, long x, long y) -> long"""
        return _controls_.TextCtrl_XYToPosition(*args, **kwargs)

    def PositionToXY(*args, **kwargs):
        """PositionToXY(long pos) -> (x, y)"""
        return _controls_.TextCtrl_PositionToXY(*args, **kwargs)

    def ShowPosition(*args, **kwargs):
        """ShowPosition(self, long pos)"""
        return _controls_.TextCtrl_ShowPosition(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(Point pt) -> (result, col, row)

        Find the row, col coresponding to the character at the point given in
        pixels. NB: pt is in device coords but is not adjusted for the client
        area origin nor scrolling.
        """
        return _controls_.TextCtrl_HitTest(*args, **kwargs)

    def HitTestPos(*args, **kwargs):
        """
        HitTestPos(Point pt) -> (result, position)

        Find the character position in the text coresponding to the point
        given in pixels. NB: pt is in device coords but is not adjusted for
        the client area origin nor scrolling. 
        """
        return _controls_.TextCtrl_HitTestPos(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(self)"""
        return _controls_.TextCtrl_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """Cut(self)"""
        return _controls_.TextCtrl_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste(self)"""
        return _controls_.TextCtrl_Paste(*args, **kwargs)

    def CanCopy(*args, **kwargs):
        """CanCopy(self) -> bool"""
        return _controls_.TextCtrl_CanCopy(*args, **kwargs)

    def CanCut(*args, **kwargs):
        """CanCut(self) -> bool"""
        return _controls_.TextCtrl_CanCut(*args, **kwargs)

    def CanPaste(*args, **kwargs):
        """CanPaste(self) -> bool"""
        return _controls_.TextCtrl_CanPaste(*args, **kwargs)

    def Undo(*args, **kwargs):
        """Undo(self)"""
        return _controls_.TextCtrl_Undo(*args, **kwargs)

    def Redo(*args, **kwargs):
        """Redo(self)"""
        return _controls_.TextCtrl_Redo(*args, **kwargs)

    def CanUndo(*args, **kwargs):
        """CanUndo(self) -> bool"""
        return _controls_.TextCtrl_CanUndo(*args, **kwargs)

    def CanRedo(*args, **kwargs):
        """CanRedo(self) -> bool"""
        return _controls_.TextCtrl_CanRedo(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """SetInsertionPoint(self, long pos)"""
        return _controls_.TextCtrl_SetInsertionPoint(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """SetInsertionPointEnd(self)"""
        return _controls_.TextCtrl_SetInsertionPointEnd(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """GetInsertionPoint(self) -> long"""
        return _controls_.TextCtrl_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """GetLastPosition(self) -> long"""
        return _controls_.TextCtrl_GetLastPosition(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, long from, long to)"""
        return _controls_.TextCtrl_SetSelection(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll(self)"""
        return _controls_.TextCtrl_SelectAll(*args, **kwargs)

    def SetEditable(*args, **kwargs):
        """SetEditable(self, bool editable)"""
        return _controls_.TextCtrl_SetEditable(*args, **kwargs)

    def MacCheckSpelling(*args, **kwargs):
        """MacCheckSpelling(self, bool check)"""
        return _controls_.TextCtrl_MacCheckSpelling(*args, **kwargs)

    def SendTextUpdatedEvent(*args, **kwargs):
        """SendTextUpdatedEvent(self)"""
        return _controls_.TextCtrl_SendTextUpdatedEvent(*args, **kwargs)

    def write(*args, **kwargs):
        """write(self, String text)"""
        return _controls_.TextCtrl_write(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(self, long from, long to) -> String"""
        return _controls_.TextCtrl_GetString(*args, **kwargs)

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
        return _controls_.TextCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    DefaultStyle = property(GetDefaultStyle,SetDefaultStyle,doc="See `GetDefaultStyle` and `SetDefaultStyle`") 
    InsertionPoint = property(GetInsertionPoint,SetInsertionPoint,doc="See `GetInsertionPoint` and `SetInsertionPoint`") 
    LastPosition = property(GetLastPosition,doc="See `GetLastPosition`") 
    NumberOfLines = property(GetNumberOfLines,doc="See `GetNumberOfLines`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    StringSelection = property(GetStringSelection,doc="See `GetStringSelection`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.TextCtrl_swigregister(TextCtrl)

def PreTextCtrl(*args, **kwargs):
    """PreTextCtrl() -> TextCtrl"""
    val = _controls_.new_PreTextCtrl(*args, **kwargs)
    return val

def TextCtrl_GetClassDefaultAttributes(*args, **kwargs):
  """
    TextCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.TextCtrl_GetClassDefaultAttributes(*args, **kwargs)

wxEVT_COMMAND_TEXT_UPDATED = _controls_.wxEVT_COMMAND_TEXT_UPDATED
wxEVT_COMMAND_TEXT_ENTER = _controls_.wxEVT_COMMAND_TEXT_ENTER
wxEVT_COMMAND_TEXT_URL = _controls_.wxEVT_COMMAND_TEXT_URL
wxEVT_COMMAND_TEXT_MAXLEN = _controls_.wxEVT_COMMAND_TEXT_MAXLEN
class TextUrlEvent(_core.CommandEvent):
    """Proxy of C++ TextUrlEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int winid, MouseEvent evtMouse, long start, long end) -> TextUrlEvent"""
        _controls_.TextUrlEvent_swiginit(self,_controls_.new_TextUrlEvent(*args, **kwargs))
    def GetMouseEvent(*args, **kwargs):
        """GetMouseEvent(self) -> MouseEvent"""
        return _controls_.TextUrlEvent_GetMouseEvent(*args, **kwargs)

    def GetURLStart(*args, **kwargs):
        """GetURLStart(self) -> long"""
        return _controls_.TextUrlEvent_GetURLStart(*args, **kwargs)

    def GetURLEnd(*args, **kwargs):
        """GetURLEnd(self) -> long"""
        return _controls_.TextUrlEvent_GetURLEnd(*args, **kwargs)

    MouseEvent = property(GetMouseEvent,doc="See `GetMouseEvent`") 
    URLEnd = property(GetURLEnd,doc="See `GetURLEnd`") 
    URLStart = property(GetURLStart,doc="See `GetURLStart`") 
_controls_.TextUrlEvent_swigregister(TextUrlEvent)

EVT_TEXT        = wx.PyEventBinder( wxEVT_COMMAND_TEXT_UPDATED, 1)
EVT_TEXT_ENTER  = wx.PyEventBinder( wxEVT_COMMAND_TEXT_ENTER, 1)
EVT_TEXT_URL    = wx.PyEventBinder( wxEVT_COMMAND_TEXT_URL, 1) 
EVT_TEXT_MAXLEN = wx.PyEventBinder( wxEVT_COMMAND_TEXT_MAXLEN, 1)

#---------------------------------------------------------------------------

class ScrollBar(_core.Control):
    """Proxy of C++ ScrollBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SB_HORIZONTAL, 
            Validator validator=DefaultValidator, String name=ScrollBarNameStr) -> ScrollBar
        """
        _controls_.ScrollBar_swiginit(self,_controls_.new_ScrollBar(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SB_HORIZONTAL, 
            Validator validator=DefaultValidator, String name=ScrollBarNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _controls_.ScrollBar_Create(*args, **kwargs)

    def GetThumbPosition(*args, **kwargs):
        """GetThumbPosition(self) -> int"""
        return _controls_.ScrollBar_GetThumbPosition(*args, **kwargs)

    def GetThumbSize(*args, **kwargs):
        """GetThumbSize(self) -> int"""
        return _controls_.ScrollBar_GetThumbSize(*args, **kwargs)

    GetThumbLength = GetThumbSize 
    def GetPageSize(*args, **kwargs):
        """GetPageSize(self) -> int"""
        return _controls_.ScrollBar_GetPageSize(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(self) -> int"""
        return _controls_.ScrollBar_GetRange(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.ScrollBar_IsVertical(*args, **kwargs)

    def SetThumbPosition(*args, **kwargs):
        """SetThumbPosition(self, int viewStart)"""
        return _controls_.ScrollBar_SetThumbPosition(*args, **kwargs)

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
        return _controls_.ScrollBar_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    PageSize = property(GetPageSize,doc="See `GetPageSize`") 
    Range = property(GetRange,doc="See `GetRange`") 
    ThumbPosition = property(GetThumbPosition,SetThumbPosition,doc="See `GetThumbPosition` and `SetThumbPosition`") 
    ThumbSize = property(GetThumbSize,doc="See `GetThumbSize`") 
_controls_.ScrollBar_swigregister(ScrollBar)
ScrollBarNameStr = cvar.ScrollBarNameStr

def PreScrollBar(*args, **kwargs):
    """PreScrollBar() -> ScrollBar"""
    val = _controls_.new_PreScrollBar(*args, **kwargs)
    return val

def ScrollBar_GetClassDefaultAttributes(*args, **kwargs):
  """
    ScrollBar_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ScrollBar_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

SP_HORIZONTAL = _controls_.SP_HORIZONTAL
SP_VERTICAL = _controls_.SP_VERTICAL
SP_ARROW_KEYS = _controls_.SP_ARROW_KEYS
SP_WRAP = _controls_.SP_WRAP
class SpinButton(_core.Control):
    """Proxy of C++ SpinButton class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SP_HORIZONTAL, 
            String name=SPIN_BUTTON_NAME) -> SpinButton
        """
        _controls_.SpinButton_swiginit(self,_controls_.new_SpinButton(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SP_HORIZONTAL, 
            String name=SPIN_BUTTON_NAME) -> bool
        """
        return _controls_.SpinButton_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return _controls_.SpinButton_GetValue(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin(self) -> int"""
        return _controls_.SpinButton_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax(self) -> int"""
        return _controls_.SpinButton_GetMax(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int val)"""
        return _controls_.SpinButton_SetValue(*args, **kwargs)

    def SetMin(*args, **kwargs):
        """SetMin(self, int minVal)"""
        return _controls_.SpinButton_SetMin(*args, **kwargs)

    def SetMax(*args, **kwargs):
        """SetMax(self, int maxVal)"""
        return _controls_.SpinButton_SetMax(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, int minVal, int maxVal)"""
        return _controls_.SpinButton_SetRange(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.SpinButton_IsVertical(*args, **kwargs)

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
        return _controls_.SpinButton_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    Max = property(GetMax,SetMax,doc="See `GetMax` and `SetMax`") 
    Min = property(GetMin,SetMin,doc="See `GetMin` and `SetMin`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.SpinButton_swigregister(SpinButton)
SPIN_BUTTON_NAME = cvar.SPIN_BUTTON_NAME
SpinCtrlNameStr = cvar.SpinCtrlNameStr

def PreSpinButton(*args, **kwargs):
    """PreSpinButton() -> SpinButton"""
    val = _controls_.new_PreSpinButton(*args, **kwargs)
    return val

def SpinButton_GetClassDefaultAttributes(*args, **kwargs):
  """
    SpinButton_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.SpinButton_GetClassDefaultAttributes(*args, **kwargs)

class SpinCtrl(_core.Control):
    """Proxy of C++ SpinCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=SP_ARROW_KEYS, int min=0, int max=100, 
            int initial=0, String name=SpinCtrlNameStr) -> SpinCtrl
        """
        _controls_.SpinCtrl_swiginit(self,_controls_.new_SpinCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=SP_ARROW_KEYS, int min=0, int max=100, 
            int initial=0, String name=SpinCtrlNameStr) -> bool
        """
        return _controls_.SpinCtrl_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return _controls_.SpinCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int value)"""
        return _controls_.SpinCtrl_SetValue(*args, **kwargs)

    def SetValueString(*args, **kwargs):
        """SetValueString(self, String text)"""
        return _controls_.SpinCtrl_SetValueString(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, int minVal, int maxVal)"""
        return _controls_.SpinCtrl_SetRange(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin(self) -> int"""
        return _controls_.SpinCtrl_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax(self) -> int"""
        return _controls_.SpinCtrl_GetMax(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, long from, long to)"""
        return _controls_.SpinCtrl_SetSelection(*args, **kwargs)

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
        return _controls_.SpinCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    Max = property(GetMax,doc="See `GetMax`") 
    Min = property(GetMin,doc="See `GetMin`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.SpinCtrl_swigregister(SpinCtrl)

def PreSpinCtrl(*args, **kwargs):
    """PreSpinCtrl() -> SpinCtrl"""
    val = _controls_.new_PreSpinCtrl(*args, **kwargs)
    return val

def SpinCtrl_GetClassDefaultAttributes(*args, **kwargs):
  """
    SpinCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.SpinCtrl_GetClassDefaultAttributes(*args, **kwargs)

class SpinEvent(_core.NotifyEvent):
    """Proxy of C++ SpinEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType commandType=wxEVT_NULL, int winid=0) -> SpinEvent"""
        _controls_.SpinEvent_swiginit(self,_controls_.new_SpinEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return _controls_.SpinEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _controls_.SpinEvent_SetPosition(*args, **kwargs)

    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
_controls_.SpinEvent_swigregister(SpinEvent)

wxEVT_COMMAND_SPINCTRL_UPDATED = _controls_.wxEVT_COMMAND_SPINCTRL_UPDATED
EVT_SPIN_UP   = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEUP, 1)
EVT_SPIN_DOWN = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEDOWN, 1)
EVT_SPIN      = wx.PyEventBinder( wx.wxEVT_SCROLL_THUMBTRACK, 1)
EVT_SPINCTRL  = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)

#---------------------------------------------------------------------------

class RadioBox(_core.Control):
    """Proxy of C++ RadioBox class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            int majorDimension=0, long style=RA_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=RadioBoxNameStr) -> RadioBox
        """
        if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']
        _controls_.RadioBox_swiginit(self,_controls_.new_RadioBox(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            wxArrayString choices=wxPyEmptyStringArray, 
            int majorDimension=0, long style=RA_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=RadioBoxNameStr) -> bool
        """
        return _controls_.RadioBox_Create(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int n)"""
        return _controls_.RadioBox_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _controls_.RadioBox_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection(self) -> String"""
        return _controls_.RadioBox_GetStringSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(self, String s) -> bool"""
        return _controls_.RadioBox_SetStringSelection(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> size_t"""
        return _controls_.RadioBox_GetCount(*args, **kwargs)

    def FindString(*args, **kwargs):
        """FindString(self, String s) -> int"""
        return _controls_.RadioBox_FindString(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(self, int n) -> String"""
        return _controls_.RadioBox_GetString(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(self, int n, String label)"""
        return _controls_.RadioBox_SetString(*args, **kwargs)

    GetItemLabel = GetString 
    SetItemLabel = SetString 
    def EnableItem(*args, **kwargs):
        """EnableItem(self, unsigned int n, bool enable=True)"""
        return _controls_.RadioBox_EnableItem(*args, **kwargs)

    def ShowItem(*args, **kwargs):
        """ShowItem(self, unsigned int n, bool show=True)"""
        return _controls_.RadioBox_ShowItem(*args, **kwargs)

    def IsItemEnabled(*args, **kwargs):
        """IsItemEnabled(self, unsigned int n) -> bool"""
        return _controls_.RadioBox_IsItemEnabled(*args, **kwargs)

    def IsItemShown(*args, **kwargs):
        """IsItemShown(self, unsigned int n) -> bool"""
        return _controls_.RadioBox_IsItemShown(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount(self) -> unsigned int"""
        return _controls_.RadioBox_GetColumnCount(*args, **kwargs)

    def GetRowCount(*args, **kwargs):
        """GetRowCount(self) -> unsigned int"""
        return _controls_.RadioBox_GetRowCount(*args, **kwargs)

    def GetNextItem(*args, **kwargs):
        """GetNextItem(self, int item, int dir, long style) -> int"""
        return _controls_.RadioBox_GetNextItem(*args, **kwargs)

    def SetItemToolTip(*args, **kwargs):
        """SetItemToolTip(self, unsigned int item, String text)"""
        return _controls_.RadioBox_SetItemToolTip(*args, **kwargs)

    def GetItemToolTip(*args, **kwargs):
        """GetItemToolTip(self, unsigned int item) -> ToolTip"""
        return _controls_.RadioBox_GetItemToolTip(*args, **kwargs)

    def SetItemHelpText(*args, **kwargs):
        """SetItemHelpText(self, unsigned int n, String helpText)"""
        return _controls_.RadioBox_SetItemHelpText(*args, **kwargs)

    def GetItemHelpText(*args, **kwargs):
        """GetItemHelpText(self, unsigned int n) -> String"""
        return _controls_.RadioBox_GetItemHelpText(*args, **kwargs)

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
        return _controls_.RadioBox_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    ColumnCount = property(GetColumnCount,doc="See `GetColumnCount`") 
    Count = property(GetCount,doc="See `GetCount`") 
    RowCount = property(GetRowCount,doc="See `GetRowCount`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
    StringSelection = property(GetStringSelection,SetStringSelection,doc="See `GetStringSelection` and `SetStringSelection`") 
_controls_.RadioBox_swigregister(RadioBox)
RadioBoxNameStr = cvar.RadioBoxNameStr
RadioButtonNameStr = cvar.RadioButtonNameStr

def PreRadioBox(*args, **kwargs):
    """PreRadioBox() -> RadioBox"""
    val = _controls_.new_PreRadioBox(*args, **kwargs)
    return val

def RadioBox_GetClassDefaultAttributes(*args, **kwargs):
  """
    RadioBox_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.RadioBox_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class RadioButton(_core.Control):
    """Proxy of C++ RadioButton class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=RadioButtonNameStr) -> RadioButton
        """
        _controls_.RadioButton_swiginit(self,_controls_.new_RadioButton(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=RadioButtonNameStr) -> bool
        """
        return _controls_.RadioButton_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> bool"""
        return _controls_.RadioButton_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, bool value)"""
        return _controls_.RadioButton_SetValue(*args, **kwargs)

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
        return _controls_.RadioButton_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.RadioButton_swigregister(RadioButton)

def PreRadioButton(*args, **kwargs):
    """PreRadioButton() -> RadioButton"""
    val = _controls_.new_PreRadioButton(*args, **kwargs)
    return val

def RadioButton_GetClassDefaultAttributes(*args, **kwargs):
  """
    RadioButton_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.RadioButton_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

SL_HORIZONTAL = _controls_.SL_HORIZONTAL
SL_VERTICAL = _controls_.SL_VERTICAL
SL_TICKS = _controls_.SL_TICKS
SL_AUTOTICKS = _controls_.SL_AUTOTICKS
SL_LABELS = _controls_.SL_LABELS
SL_LEFT = _controls_.SL_LEFT
SL_TOP = _controls_.SL_TOP
SL_RIGHT = _controls_.SL_RIGHT
SL_BOTTOM = _controls_.SL_BOTTOM
SL_BOTH = _controls_.SL_BOTH
SL_SELRANGE = _controls_.SL_SELRANGE
SL_INVERSE = _controls_.SL_INVERSE
class Slider(_core.Control):
    """Proxy of C++ Slider class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, int value=0, int minValue=0, 
            int maxValue=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SL_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=SliderNameStr) -> Slider
        """
        if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']
        _controls_.Slider_swiginit(self,_controls_.new_Slider(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, int value=0, int minValue=0, 
            int maxValue=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SL_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=SliderNameStr) -> bool
        """
        return _controls_.Slider_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> int"""
        return _controls_.Slider_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int value)"""
        return _controls_.Slider_SetValue(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(self, int minValue, int maxValue)"""
        return _controls_.Slider_SetRange(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin(self) -> int"""
        return _controls_.Slider_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax(self) -> int"""
        return _controls_.Slider_GetMax(*args, **kwargs)

    def SetMin(*args, **kwargs):
        """SetMin(self, int minValue)"""
        return _controls_.Slider_SetMin(*args, **kwargs)

    def SetMax(*args, **kwargs):
        """SetMax(self, int maxValue)"""
        return _controls_.Slider_SetMax(*args, **kwargs)

    def SetLineSize(*args, **kwargs):
        """SetLineSize(self, int lineSize)"""
        return _controls_.Slider_SetLineSize(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(self, int pageSize)"""
        return _controls_.Slider_SetPageSize(*args, **kwargs)

    def GetLineSize(*args, **kwargs):
        """GetLineSize(self) -> int"""
        return _controls_.Slider_GetLineSize(*args, **kwargs)

    def GetPageSize(*args, **kwargs):
        """GetPageSize(self) -> int"""
        return _controls_.Slider_GetPageSize(*args, **kwargs)

    def SetThumbLength(*args, **kwargs):
        """SetThumbLength(self, int lenPixels)"""
        return _controls_.Slider_SetThumbLength(*args, **kwargs)

    def GetThumbLength(*args, **kwargs):
        """GetThumbLength(self) -> int"""
        return _controls_.Slider_GetThumbLength(*args, **kwargs)

    def SetTickFreq(*args, **kwargs):
        """SetTickFreq(self, int n, int pos=1)"""
        return _controls_.Slider_SetTickFreq(*args, **kwargs)

    def GetTickFreq(*args, **kwargs):
        """GetTickFreq(self) -> int"""
        return _controls_.Slider_GetTickFreq(*args, **kwargs)

    def ClearTicks(*args, **kwargs):
        """ClearTicks(self)"""
        return _controls_.Slider_ClearTicks(*args, **kwargs)

    def SetTick(*args, **kwargs):
        """SetTick(self, int tickPos)"""
        return _controls_.Slider_SetTick(*args, **kwargs)

    def ClearSel(*args, **kwargs):
        """ClearSel(self)"""
        return _controls_.Slider_ClearSel(*args, **kwargs)

    def GetSelEnd(*args, **kwargs):
        """GetSelEnd(self) -> int"""
        return _controls_.Slider_GetSelEnd(*args, **kwargs)

    def GetSelStart(*args, **kwargs):
        """GetSelStart(self) -> int"""
        return _controls_.Slider_GetSelStart(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int min, int max)"""
        return _controls_.Slider_SetSelection(*args, **kwargs)

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
        return _controls_.Slider_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    LineSize = property(GetLineSize,SetLineSize,doc="See `GetLineSize` and `SetLineSize`") 
    Max = property(GetMax,SetMax,doc="See `GetMax` and `SetMax`") 
    Min = property(GetMin,SetMin,doc="See `GetMin` and `SetMin`") 
    PageSize = property(GetPageSize,SetPageSize,doc="See `GetPageSize` and `SetPageSize`") 
    SelEnd = property(GetSelEnd,doc="See `GetSelEnd`") 
    SelStart = property(GetSelStart,doc="See `GetSelStart`") 
    ThumbLength = property(GetThumbLength,SetThumbLength,doc="See `GetThumbLength` and `SetThumbLength`") 
    TickFreq = property(GetTickFreq,SetTickFreq,doc="See `GetTickFreq` and `SetTickFreq`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.Slider_swigregister(Slider)
SliderNameStr = cvar.SliderNameStr

def PreSlider(*args, **kwargs):
    """PreSlider() -> Slider"""
    val = _controls_.new_PreSlider(*args, **kwargs)
    return val

def Slider_GetClassDefaultAttributes(*args, **kwargs):
  """
    Slider_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.Slider_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

wxEVT_COMMAND_TOGGLEBUTTON_CLICKED = _controls_.wxEVT_COMMAND_TOGGLEBUTTON_CLICKED
EVT_TOGGLEBUTTON = wx.PyEventBinder( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 1)

class ToggleButton(_core.Control):
    """Proxy of C++ ToggleButton class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ToggleButtonNameStr) -> ToggleButton
        """
        _controls_.ToggleButton_swiginit(self,_controls_.new_ToggleButton(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ToggleButtonNameStr) -> bool
        """
        return _controls_.ToggleButton_Create(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, bool value)"""
        return _controls_.ToggleButton_SetValue(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> bool"""
        return _controls_.ToggleButton_GetValue(*args, **kwargs)

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
        return _controls_.ToggleButton_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.ToggleButton_swigregister(ToggleButton)
ToggleButtonNameStr = cvar.ToggleButtonNameStr

def PreToggleButton(*args, **kwargs):
    """PreToggleButton() -> ToggleButton"""
    val = _controls_.new_PreToggleButton(*args, **kwargs)
    return val

def ToggleButton_GetClassDefaultAttributes(*args, **kwargs):
  """
    ToggleButton_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ToggleButton_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

BK_DEFAULT = _controls_.BK_DEFAULT
BK_TOP = _controls_.BK_TOP
BK_BOTTOM = _controls_.BK_BOTTOM
BK_LEFT = _controls_.BK_LEFT
BK_RIGHT = _controls_.BK_RIGHT
BK_ALIGN_MASK = _controls_.BK_ALIGN_MASK
BK_BUTTONBAR = _controls_.BK_BUTTONBAR
BK_HITTEST_NOWHERE = _controls_.BK_HITTEST_NOWHERE
BK_HITTEST_ONICON = _controls_.BK_HITTEST_ONICON
BK_HITTEST_ONLABEL = _controls_.BK_HITTEST_ONLABEL
BK_HITTEST_ONITEM = _controls_.BK_HITTEST_ONITEM
BK_HITTEST_ONPAGE = _controls_.BK_HITTEST_ONPAGE
class BookCtrlBase(_core.Control):
    """Proxy of C++ BookCtrlBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetPageCount(*args, **kwargs):
        """GetPageCount(self) -> size_t"""
        return _controls_.BookCtrlBase_GetPageCount(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self, size_t n) -> Window"""
        return _controls_.BookCtrlBase_GetPage(*args, **kwargs)

    def GetCurrentPage(*args, **kwargs):
        """GetCurrentPage(self) -> Window"""
        return _controls_.BookCtrlBase_GetCurrentPage(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _controls_.BookCtrlBase_GetSelection(*args, **kwargs)

    def SetPageText(*args, **kwargs):
        """SetPageText(self, size_t n, String strText) -> bool"""
        return _controls_.BookCtrlBase_SetPageText(*args, **kwargs)

    def GetPageText(*args, **kwargs):
        """GetPageText(self, size_t n) -> String"""
        return _controls_.BookCtrlBase_GetPageText(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(self, ImageList imageList)"""
        return _controls_.BookCtrlBase_SetImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(self, ImageList imageList)"""
        return _controls_.BookCtrlBase_AssignImageList(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList(self) -> ImageList"""
        return _controls_.BookCtrlBase_GetImageList(*args, **kwargs)

    def GetPageImage(*args, **kwargs):
        """GetPageImage(self, size_t n) -> int"""
        return _controls_.BookCtrlBase_GetPageImage(*args, **kwargs)

    def SetPageImage(*args, **kwargs):
        """SetPageImage(self, size_t n, int imageId) -> bool"""
        return _controls_.BookCtrlBase_SetPageImage(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(self, Size size)"""
        return _controls_.BookCtrlBase_SetPageSize(*args, **kwargs)

    def CalcSizeFromPage(*args, **kwargs):
        """CalcSizeFromPage(self, Size sizePage) -> Size"""
        return _controls_.BookCtrlBase_CalcSizeFromPage(*args, **kwargs)

    def GetInternalBorder(*args, **kwargs):
        """GetInternalBorder(self) -> unsigned int"""
        return _controls_.BookCtrlBase_GetInternalBorder(*args, **kwargs)

    def SetInternalBorder(*args, **kwargs):
        """SetInternalBorder(self, unsigned int internalBorder)"""
        return _controls_.BookCtrlBase_SetInternalBorder(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.BookCtrlBase_IsVertical(*args, **kwargs)

    def SetControlMargin(*args, **kwargs):
        """SetControlMargin(self, int margin)"""
        return _controls_.BookCtrlBase_SetControlMargin(*args, **kwargs)

    def GetControlMargin(*args, **kwargs):
        """GetControlMargin(self) -> int"""
        return _controls_.BookCtrlBase_GetControlMargin(*args, **kwargs)

    def SetFitToCurrentPage(*args, **kwargs):
        """SetFitToCurrentPage(self, bool fit)"""
        return _controls_.BookCtrlBase_SetFitToCurrentPage(*args, **kwargs)

    def GetFitToCurrentPage(*args, **kwargs):
        """GetFitToCurrentPage(self) -> bool"""
        return _controls_.BookCtrlBase_GetFitToCurrentPage(*args, **kwargs)

    def GetControlSizer(*args, **kwargs):
        """GetControlSizer(self) -> Sizer"""
        return _controls_.BookCtrlBase_GetControlSizer(*args, **kwargs)

    def DeletePage(*args, **kwargs):
        """DeletePage(self, size_t n) -> bool"""
        return _controls_.BookCtrlBase_DeletePage(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(self, size_t n) -> bool"""
        return _controls_.BookCtrlBase_RemovePage(*args, **kwargs)

    def DeleteAllPages(*args, **kwargs):
        """DeleteAllPages(self) -> bool"""
        return _controls_.BookCtrlBase_DeleteAllPages(*args, **kwargs)

    def AddPage(*args, **kwargs):
        """AddPage(self, Window page, String text, bool select=False, int imageId=-1) -> bool"""
        return _controls_.BookCtrlBase_AddPage(*args, **kwargs)

    def InsertPage(*args, **kwargs):
        """
        InsertPage(self, size_t n, Window page, String text, bool select=False, 
            int imageId=-1) -> bool
        """
        return _controls_.BookCtrlBase_InsertPage(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, size_t n) -> int"""
        return _controls_.BookCtrlBase_SetSelection(*args, **kwargs)

    def ChangeSelection(*args, **kwargs):
        """ChangeSelection(self, size_t n) -> int"""
        return _controls_.BookCtrlBase_ChangeSelection(*args, **kwargs)

    def AdvanceSelection(*args, **kwargs):
        """AdvanceSelection(self, bool forward=True)"""
        return _controls_.BookCtrlBase_AdvanceSelection(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(Point pt) -> (tab, where)

        Returns the page/tab which is hit, and flags indicating where using
        wx.NB_HITTEST flags.
        """
        return _controls_.BookCtrlBase_HitTest(*args, **kwargs)

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
        return _controls_.BookCtrlBase_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    ControlMargin = property(GetControlMargin,SetControlMargin,doc="See `GetControlMargin` and `SetControlMargin`") 
    ControlSizer = property(GetControlSizer,doc="See `GetControlSizer`") 
    CurrentPage = property(GetCurrentPage,doc="See `GetCurrentPage`") 
    FitToCurrentPage = property(GetFitToCurrentPage,SetFitToCurrentPage,doc="See `GetFitToCurrentPage` and `SetFitToCurrentPage`") 
    ImageList = property(GetImageList,SetImageList,doc="See `GetImageList` and `SetImageList`") 
    InternalBorder = property(GetInternalBorder,SetInternalBorder,doc="See `GetInternalBorder` and `SetInternalBorder`") 
    Page = property(GetPage,doc="See `GetPage`") 
    PageCount = property(GetPageCount,doc="See `GetPageCount`") 
    PageImage = property(GetPageImage,SetPageImage,doc="See `GetPageImage` and `SetPageImage`") 
    PageText = property(GetPageText,SetPageText,doc="See `GetPageText` and `SetPageText`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
_controls_.BookCtrlBase_swigregister(BookCtrlBase)
NotebookNameStr = cvar.NotebookNameStr

def BookCtrlBase_GetClassDefaultAttributes(*args, **kwargs):
  """
    BookCtrlBase_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.BookCtrlBase_GetClassDefaultAttributes(*args, **kwargs)

class BookCtrlBaseEvent(_core.NotifyEvent):
    """Proxy of C++ BookCtrlBaseEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> BookCtrlBaseEvent
        """
        _controls_.BookCtrlBaseEvent_swiginit(self,_controls_.new_BookCtrlBaseEvent(*args, **kwargs))
    def GetSelection(*args, **kwargs):
        """
        GetSelection(self) -> int

        Returns item index for a listbox or choice selection event (not valid
        for a deselection).
        """
        return _controls_.BookCtrlBaseEvent_GetSelection(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int nSel)"""
        return _controls_.BookCtrlBaseEvent_SetSelection(*args, **kwargs)

    def GetOldSelection(*args, **kwargs):
        """GetOldSelection(self) -> int"""
        return _controls_.BookCtrlBaseEvent_GetOldSelection(*args, **kwargs)

    def SetOldSelection(*args, **kwargs):
        """SetOldSelection(self, int nOldSel)"""
        return _controls_.BookCtrlBaseEvent_SetOldSelection(*args, **kwargs)

    OldSelection = property(GetOldSelection,SetOldSelection,doc="See `GetOldSelection` and `SetOldSelection`") 
    Selection = property(GetSelection,SetSelection,doc="See `GetSelection` and `SetSelection`") 
_controls_.BookCtrlBaseEvent_swigregister(BookCtrlBaseEvent)

#---------------------------------------------------------------------------

NB_FIXEDWIDTH = _controls_.NB_FIXEDWIDTH
NB_TOP = _controls_.NB_TOP
NB_LEFT = _controls_.NB_LEFT
NB_RIGHT = _controls_.NB_RIGHT
NB_BOTTOM = _controls_.NB_BOTTOM
NB_MULTILINE = _controls_.NB_MULTILINE
NB_NOPAGETHEME = _controls_.NB_NOPAGETHEME
NB_HITTEST_NOWHERE = _controls_.NB_HITTEST_NOWHERE
NB_HITTEST_ONICON = _controls_.NB_HITTEST_ONICON
NB_HITTEST_ONLABEL = _controls_.NB_HITTEST_ONLABEL
NB_HITTEST_ONITEM = _controls_.NB_HITTEST_ONITEM
NB_HITTEST_ONPAGE = _controls_.NB_HITTEST_ONPAGE
class Notebook(BookCtrlBase):
    """Proxy of C++ Notebook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=NotebookNameStr) -> Notebook
        """
        _controls_.Notebook_swiginit(self,_controls_.new_Notebook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=NotebookNameStr) -> bool
        """
        return _controls_.Notebook_Create(*args, **kwargs)

    def GetRowCount(*args, **kwargs):
        """GetRowCount(self) -> int"""
        return _controls_.Notebook_GetRowCount(*args, **kwargs)

    def SetPadding(*args, **kwargs):
        """SetPadding(self, Size padding)"""
        return _controls_.Notebook_SetPadding(*args, **kwargs)

    def SetTabSize(*args, **kwargs):
        """SetTabSize(self, Size sz)"""
        return _controls_.Notebook_SetTabSize(*args, **kwargs)

    def GetThemeBackgroundColour(*args, **kwargs):
        """GetThemeBackgroundColour(self) -> Colour"""
        return _controls_.Notebook_GetThemeBackgroundColour(*args, **kwargs)

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
        return _controls_.Notebook_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def SendPageChangingEvent(*args, **kwargs):
        """SendPageChangingEvent(self, int nPage) -> bool"""
        return _controls_.Notebook_SendPageChangingEvent(*args, **kwargs)

    def SendPageChangedEvent(*args, **kwargs):
        """SendPageChangedEvent(self, int nPageOld, int nPageNew=-1)"""
        return _controls_.Notebook_SendPageChangedEvent(*args, **kwargs)

    RowCount = property(GetRowCount,doc="See `GetRowCount`") 
    ThemeBackgroundColour = property(GetThemeBackgroundColour,doc="See `GetThemeBackgroundColour`") 
_controls_.Notebook_swigregister(Notebook)

def PreNotebook(*args, **kwargs):
    """PreNotebook() -> Notebook"""
    val = _controls_.new_PreNotebook(*args, **kwargs)
    return val

def Notebook_GetClassDefaultAttributes(*args, **kwargs):
  """
    Notebook_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.Notebook_GetClassDefaultAttributes(*args, **kwargs)

class NotebookEvent(BookCtrlBaseEvent):
    """Proxy of C++ NotebookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> NotebookEvent
        """
        _controls_.NotebookEvent_swiginit(self,_controls_.new_NotebookEvent(*args, **kwargs))
_controls_.NotebookEvent_swigregister(NotebookEvent)

wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
# wxNotebook events
EVT_NOTEBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, 1 )
EVT_NOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, 1 )

#----------------------------------------------------------------------------

class NotebookPage(wx.Panel):
    """
    There is an old (and apparently unsolvable) bug when placing a
    window with a nonstandard background colour in a wx.Notebook on
    wxGTK1, as the notbooks's background colour would always be used
    when the window is refreshed.  The solution is to place a panel in
    the notbook and the coloured window on the panel, sized to cover
    the panel.  This simple class does that for you, just put an
    instance of this in the notebook and make your regular window a
    child of this one and it will handle the resize for you.
    """
    def __init__(self, parent, id=-1,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.TAB_TRAVERSAL, name="panel"):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)
        self.child = None
        self.Bind(wx.EVT_SIZE, self.OnSize)

    def OnSize(self, evt):
        if self.child is None:
            children = self.GetChildren()
            if len(children):
                self.child = children[0]
        if self.child:
            self.child.SetPosition((0,0))
            self.child.SetSize(self.GetSize())


#---------------------------------------------------------------------------

LB_DEFAULT = _controls_.LB_DEFAULT
LB_TOP = _controls_.LB_TOP
LB_BOTTOM = _controls_.LB_BOTTOM
LB_LEFT = _controls_.LB_LEFT
LB_RIGHT = _controls_.LB_RIGHT
LB_ALIGN_MASK = _controls_.LB_ALIGN_MASK
class Listbook(BookCtrlBase):
    """Proxy of C++ Listbook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=EmptyString) -> Listbook
        """
        _controls_.Listbook_swiginit(self,_controls_.new_Listbook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=EmptyString) -> bool
        """
        return _controls_.Listbook_Create(*args, **kwargs)

    def GetListView(*args, **kwargs):
        """GetListView(self) -> ListView"""
        return _controls_.Listbook_GetListView(*args, **kwargs)

    ListView = property(GetListView,doc="See `GetListView`") 
_controls_.Listbook_swigregister(Listbook)

def PreListbook(*args, **kwargs):
    """PreListbook() -> Listbook"""
    val = _controls_.new_PreListbook(*args, **kwargs)
    return val

class ListbookEvent(BookCtrlBaseEvent):
    """Proxy of C++ ListbookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> ListbookEvent
        """
        _controls_.ListbookEvent_swiginit(self,_controls_.new_ListbookEvent(*args, **kwargs))
_controls_.ListbookEvent_swigregister(ListbookEvent)

wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING
EVT_LISTBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, 1 )
EVT_LISTBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, 1 )

CHB_DEFAULT = _controls_.CHB_DEFAULT
CHB_TOP = _controls_.CHB_TOP
CHB_BOTTOM = _controls_.CHB_BOTTOM
CHB_LEFT = _controls_.CHB_LEFT
CHB_RIGHT = _controls_.CHB_RIGHT
CHB_ALIGN_MASK = _controls_.CHB_ALIGN_MASK
class Choicebook(BookCtrlBase):
    """Proxy of C++ Choicebook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=EmptyString) -> Choicebook
        """
        _controls_.Choicebook_swiginit(self,_controls_.new_Choicebook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=EmptyString) -> bool
        """
        return _controls_.Choicebook_Create(*args, **kwargs)

    def GetChoiceCtrl(*args, **kwargs):
        """GetChoiceCtrl(self) -> Choice"""
        return _controls_.Choicebook_GetChoiceCtrl(*args, **kwargs)

    ChoiceCtrl = property(GetChoiceCtrl,doc="See `GetChoiceCtrl`") 
_controls_.Choicebook_swigregister(Choicebook)

def PreChoicebook(*args, **kwargs):
    """PreChoicebook() -> Choicebook"""
    val = _controls_.new_PreChoicebook(*args, **kwargs)
    return val

class ChoicebookEvent(BookCtrlBaseEvent):
    """Proxy of C++ ChoicebookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> ChoicebookEvent
        """
        _controls_.ChoicebookEvent_swiginit(self,_controls_.new_ChoicebookEvent(*args, **kwargs))
_controls_.ChoicebookEvent_swigregister(ChoicebookEvent)

wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED
wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING
EVT_CHOICEBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, 1 )
EVT_CHOICEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, 1 )

#---------------------------------------------------------------------------

class Treebook(BookCtrlBase):
    """Proxy of C++ Treebook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BK_DEFAULT, 
            String name=EmptyString) -> Treebook
        """
        _controls_.Treebook_swiginit(self,_controls_.new_Treebook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BK_DEFAULT, 
            String name=EmptyString) -> bool
        """
        return _controls_.Treebook_Create(*args, **kwargs)

    def InsertSubPage(*args, **kwargs):
        """
        InsertSubPage(self, size_t pos, Window page, String text, bool select=False, 
            int imageId=NOT_FOUND) -> bool
        """
        return _controls_.Treebook_InsertSubPage(*args, **kwargs)

    def AddSubPage(*args, **kwargs):
        """AddSubPage(self, Window page, String text, bool select=False, int imageId=NOT_FOUND) -> bool"""
        return _controls_.Treebook_AddSubPage(*args, **kwargs)

    def IsNodeExpanded(*args, **kwargs):
        """IsNodeExpanded(self, size_t pos) -> bool"""
        return _controls_.Treebook_IsNodeExpanded(*args, **kwargs)

    def ExpandNode(*args, **kwargs):
        """ExpandNode(self, size_t pos, bool expand=True) -> bool"""
        return _controls_.Treebook_ExpandNode(*args, **kwargs)

    def CollapseNode(*args, **kwargs):
        """CollapseNode(self, size_t pos) -> bool"""
        return _controls_.Treebook_CollapseNode(*args, **kwargs)

    def GetPageParent(*args, **kwargs):
        """GetPageParent(self, size_t pos) -> int"""
        return _controls_.Treebook_GetPageParent(*args, **kwargs)

    def GetTreeCtrl(*args, **kwargs):
        """GetTreeCtrl(self) -> TreeCtrl"""
        return _controls_.Treebook_GetTreeCtrl(*args, **kwargs)

    TreeCtrl = property(GetTreeCtrl,doc="See `GetTreeCtrl`") 
_controls_.Treebook_swigregister(Treebook)

def PreTreebook(*args, **kwargs):
    """PreTreebook() -> Treebook"""
    val = _controls_.new_PreTreebook(*args, **kwargs)
    return val

class TreebookEvent(BookCtrlBaseEvent):
    """Proxy of C++ TreebookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=NOT_FOUND, 
            int nOldSel=NOT_FOUND) -> TreebookEvent
        """
        _controls_.TreebookEvent_swiginit(self,_controls_.new_TreebookEvent(*args, **kwargs))
_controls_.TreebookEvent_swigregister(TreebookEvent)

wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED
wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING
wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED = _controls_.wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED
wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED = _controls_.wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED
EVT_TREEBOOK_PAGE_CHANGED= wx.PyEventBinder( wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, 1 )
EVT_TREEBOOK_PAGE_CHANGING= wx.PyEventBinder( wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, 1)
EVT_TREEBOOK_NODE_COLLAPSED= wx.PyEventBinder( wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED, 1 )
EVT_TREEBOOK_NODE_EXPANDED= wx.PyEventBinder( wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED, 1 )

#---------------------------------------------------------------------------

class Toolbook(BookCtrlBase):
    """Proxy of C++ Toolbook class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BK_DEFAULT, 
            String name=EmptyString) -> Toolbook
        """
        _controls_.Toolbook_swiginit(self,_controls_.new_Toolbook(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=wxEmptyString) -> bool
        """
        return _controls_.Toolbook_Create(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar(self) -> ToolBarBase"""
        return _controls_.Toolbook_GetToolBar(*args, **kwargs)

    def Realize(*args, **kwargs):
        """Realize(self)"""
        return _controls_.Toolbook_Realize(*args, **kwargs)

    ToolBar = property(GetToolBar,doc="See `GetToolBar`") 
_controls_.Toolbook_swigregister(Toolbook)

def PreToolbook(*args, **kwargs):
    """PreToolbook() -> Toolbook"""
    val = _controls_.new_PreToolbook(*args, **kwargs)
    return val

class ToolbookEvent(BookCtrlBaseEvent):
    """Proxy of C++ ToolbookEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0, int nSel=NOT_FOUND, 
            int nOldSel=NOT_FOUND) -> ToolbookEvent
        """
        _controls_.ToolbookEvent_swiginit(self,_controls_.new_ToolbookEvent(*args, **kwargs))
_controls_.ToolbookEvent_swigregister(ToolbookEvent)

wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED
wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING
EVT_TOOLBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED, 1)
EVT_TOOLBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING, 1)

#---------------------------------------------------------------------------

TOOL_STYLE_BUTTON = _controls_.TOOL_STYLE_BUTTON
TOOL_STYLE_SEPARATOR = _controls_.TOOL_STYLE_SEPARATOR
TOOL_STYLE_CONTROL = _controls_.TOOL_STYLE_CONTROL
TB_HORIZONTAL = _controls_.TB_HORIZONTAL
TB_VERTICAL = _controls_.TB_VERTICAL
TB_TOP = _controls_.TB_TOP
TB_LEFT = _controls_.TB_LEFT
TB_BOTTOM = _controls_.TB_BOTTOM
TB_RIGHT = _controls_.TB_RIGHT
TB_3DBUTTONS = _controls_.TB_3DBUTTONS
TB_FLAT = _controls_.TB_FLAT
TB_DOCKABLE = _controls_.TB_DOCKABLE
TB_NOICONS = _controls_.TB_NOICONS
TB_TEXT = _controls_.TB_TEXT
TB_NODIVIDER = _controls_.TB_NODIVIDER
TB_NOALIGN = _controls_.TB_NOALIGN
TB_HORZ_LAYOUT = _controls_.TB_HORZ_LAYOUT
TB_HORZ_TEXT = _controls_.TB_HORZ_TEXT
TB_NO_TOOLTIPS = _controls_.TB_NO_TOOLTIPS
class ToolBarToolBase(_core.Object):
    """Proxy of C++ ToolBarToolBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return _controls_.ToolBarToolBase_GetId(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl(self) -> Control"""
        return _controls_.ToolBarToolBase_GetControl(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar(self) -> ToolBarBase"""
        return _controls_.ToolBarToolBase_GetToolBar(*args, **kwargs)

    def IsButton(*args, **kwargs):
        """IsButton(self) -> int"""
        return _controls_.ToolBarToolBase_IsButton(*args, **kwargs)

    def IsControl(*args, **kwargs):
        """IsControl(self) -> int"""
        return _controls_.ToolBarToolBase_IsControl(*args, **kwargs)

    def IsSeparator(*args, **kwargs):
        """IsSeparator(self) -> int"""
        return _controls_.ToolBarToolBase_IsSeparator(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> int"""
        return _controls_.ToolBarToolBase_GetStyle(*args, **kwargs)

    def GetKind(*args, **kwargs):
        """GetKind(self) -> int"""
        return _controls_.ToolBarToolBase_GetKind(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled(self) -> bool"""
        return _controls_.ToolBarToolBase_IsEnabled(*args, **kwargs)

    def IsToggled(*args, **kwargs):
        """IsToggled(self) -> bool"""
        return _controls_.ToolBarToolBase_IsToggled(*args, **kwargs)

    def CanBeToggled(*args, **kwargs):
        """CanBeToggled(self) -> bool"""
        return _controls_.ToolBarToolBase_CanBeToggled(*args, **kwargs)

    def GetNormalBitmap(*args, **kwargs):
        """GetNormalBitmap(self) -> Bitmap"""
        return _controls_.ToolBarToolBase_GetNormalBitmap(*args, **kwargs)

    def GetDisabledBitmap(*args, **kwargs):
        """GetDisabledBitmap(self) -> Bitmap"""
        return _controls_.ToolBarToolBase_GetDisabledBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _controls_.ToolBarToolBase_GetBitmap(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> String"""
        return _controls_.ToolBarToolBase_GetLabel(*args, **kwargs)

    def GetShortHelp(*args, **kwargs):
        """GetShortHelp(self) -> String"""
        return _controls_.ToolBarToolBase_GetShortHelp(*args, **kwargs)

    def GetLongHelp(*args, **kwargs):
        """GetLongHelp(self) -> String"""
        return _controls_.ToolBarToolBase_GetLongHelp(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(self, bool enable) -> bool"""
        return _controls_.ToolBarToolBase_Enable(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(self)"""
        return _controls_.ToolBarToolBase_Toggle(*args, **kwargs)

    def SetToggle(*args, **kwargs):
        """SetToggle(self, bool toggle) -> bool"""
        return _controls_.ToolBarToolBase_SetToggle(*args, **kwargs)

    def SetShortHelp(*args, **kwargs):
        """SetShortHelp(self, String help) -> bool"""
        return _controls_.ToolBarToolBase_SetShortHelp(*args, **kwargs)

    def SetLongHelp(*args, **kwargs):
        """SetLongHelp(self, String help) -> bool"""
        return _controls_.ToolBarToolBase_SetLongHelp(*args, **kwargs)

    def SetNormalBitmap(*args, **kwargs):
        """SetNormalBitmap(self, Bitmap bmp)"""
        return _controls_.ToolBarToolBase_SetNormalBitmap(*args, **kwargs)

    def SetDisabledBitmap(*args, **kwargs):
        """SetDisabledBitmap(self, Bitmap bmp)"""
        return _controls_.ToolBarToolBase_SetDisabledBitmap(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, String label)"""
        return _controls_.ToolBarToolBase_SetLabel(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach(self)"""
        return _controls_.ToolBarToolBase_Detach(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(self, ToolBarBase tbar)"""
        return _controls_.ToolBarToolBase_Attach(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """GetClientData(self) -> PyObject"""
        return _controls_.ToolBarToolBase_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """SetClientData(self, PyObject clientData)"""
        return _controls_.ToolBarToolBase_SetClientData(*args, **kwargs)

    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap

    Bitmap = property(GetBitmap,doc="See `GetBitmap`") 
    ClientData = property(GetClientData,SetClientData,doc="See `GetClientData` and `SetClientData`") 
    Control = property(GetControl,doc="See `GetControl`") 
    DisabledBitmap = property(GetDisabledBitmap,SetDisabledBitmap,doc="See `GetDisabledBitmap` and `SetDisabledBitmap`") 
    Id = property(GetId,doc="See `GetId`") 
    Kind = property(GetKind,doc="See `GetKind`") 
    Label = property(GetLabel,SetLabel,doc="See `GetLabel` and `SetLabel`") 
    LongHelp = property(GetLongHelp,SetLongHelp,doc="See `GetLongHelp` and `SetLongHelp`") 
    NormalBitmap = property(GetNormalBitmap,SetNormalBitmap,doc="See `GetNormalBitmap` and `SetNormalBitmap`") 
    ShortHelp = property(GetShortHelp,SetShortHelp,doc="See `GetShortHelp` and `SetShortHelp`") 
    Style = property(GetStyle,doc="See `GetStyle`") 
    ToolBar = property(GetToolBar,doc="See `GetToolBar`") 
_controls_.ToolBarToolBase_swigregister(ToolBarToolBase)

class ToolBarBase(_core.Control):
    """Proxy of C++ ToolBarBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def DoAddTool(*args, **kwargs):
        """
        DoAddTool(self, int id, String label, Bitmap bitmap, Bitmap bmpDisabled=wxNullBitmap, 
            int kind=ITEM_NORMAL, String shortHelp=EmptyString, 
            String longHelp=EmptyString, 
            PyObject clientData=None) -> ToolBarToolBase
        """
        return _controls_.ToolBarBase_DoAddTool(*args, **kwargs)

    def DoInsertTool(*args, **kwargs):
        """
        DoInsertTool(self, size_t pos, int id, String label, Bitmap bitmap, Bitmap bmpDisabled=wxNullBitmap, 
            int kind=ITEM_NORMAL, 
            String shortHelp=EmptyString, String longHelp=EmptyString, 
            PyObject clientData=None) -> ToolBarToolBase
        """
        return _controls_.ToolBarBase_DoInsertTool(*args, **kwargs)

    # These match the original Add methods for this class, kept for
    # backwards compatibility with versions < 2.3.3.


    def AddTool(self, id, bitmap,
                pushedBitmap = wx.NullBitmap,
                isToggle = 0,
                clientData = None,
                shortHelpString = '',
                longHelpString = '') :
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoAddTool(id, '', bitmap, pushedBitmap, kind,
                              shortHelpString, longHelpString, clientData)

    def AddSimpleTool(self, id, bitmap,
                      shortHelpString = '',
                      longHelpString = '',
                      isToggle = 0):
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoAddTool(id, '', bitmap, wx.NullBitmap, kind,
                              shortHelpString, longHelpString, None)

    def InsertTool(self, pos, id, bitmap,
                   pushedBitmap = wx.NullBitmap,
                   isToggle = 0,
                   clientData = None,
                   shortHelpString = '',
                   longHelpString = ''):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, pushedBitmap, kind,
                                 shortHelpString, longHelpString, clientData)

    def InsertSimpleTool(self, pos, id, bitmap,
                         shortHelpString = '',
                         longHelpString = '',
                         isToggle = 0):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, wx.NullBitmap, kind,
                                 shortHelpString, longHelpString, None)


    # The following are the new toolbar Add methods starting with
    # 2.3.3.  They are renamed to have 'Label' in the name so as to be
    # able to keep backwards compatibility with using the above
    # methods.  Eventually these should migrate to be the methods used
    # primarily and lose the 'Label' in the name...

    def AddLabelTool(self, id, label, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     kind = wx.ITEM_NORMAL,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        The full AddTool() function.

        If bmpDisabled is wx.NullBitmap, a shadowed version of the normal bitmap
        is created and used as the disabled image.
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, kind,
                              shortHelp, longHelp, clientData)


    def InsertLabelTool(self, pos, id, label, bitmap,
                        bmpDisabled = wx.NullBitmap,
                        kind = wx.ITEM_NORMAL,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''
        Insert the new tool at the given position, if pos == GetToolsCount(), it
        is equivalent to AddTool()
        '''
        return self.DoInsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, clientData)

    def AddCheckLabelTool(self, id, label, bitmap,
                        bmpDisabled = wx.NullBitmap,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.ITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioLabelTool(self, id, label, bitmap,
                          bmpDisabled = wx.NullBitmap,
                          shortHelp = '', longHelp = '',
                          clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.ITEM_RADIO,
                              shortHelp, longHelp, clientData)


    # For consistency with the backwards compatible methods above, here are
    # some non-'Label' versions of the Check and Radio methods

    def AddCheckTool(self, id, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.ITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioTool(self, id, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.ITEM_RADIO,
                              shortHelp, longHelp, clientData)

    def AddToolItem(*args, **kwargs):
        """AddToolItem(self, ToolBarToolBase tool) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_AddToolItem(*args, **kwargs)

    def InsertToolItem(*args, **kwargs):
        """InsertToolItem(self, size_t pos, ToolBarToolBase tool) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_InsertToolItem(*args, **kwargs)

    def AddControl(*args, **kwargs):
        """AddControl(self, Control control) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_AddControl(*args, **kwargs)

    def InsertControl(*args, **kwargs):
        """InsertControl(self, size_t pos, Control control) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_InsertControl(*args, **kwargs)

    def FindControl(*args, **kwargs):
        """FindControl(self, int id) -> Control"""
        return _controls_.ToolBarBase_FindControl(*args, **kwargs)

    def AddSeparator(*args, **kwargs):
        """AddSeparator(self) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_AddSeparator(*args, **kwargs)

    def InsertSeparator(*args, **kwargs):
        """InsertSeparator(self, size_t pos) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_InsertSeparator(*args, **kwargs)

    def RemoveTool(*args, **kwargs):
        """RemoveTool(self, int id) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_RemoveTool(*args, **kwargs)

    def DeleteToolByPos(*args, **kwargs):
        """DeleteToolByPos(self, size_t pos) -> bool"""
        return _controls_.ToolBarBase_DeleteToolByPos(*args, **kwargs)

    def DeleteTool(*args, **kwargs):
        """DeleteTool(self, int id) -> bool"""
        return _controls_.ToolBarBase_DeleteTool(*args, **kwargs)

    def ClearTools(*args, **kwargs):
        """ClearTools(self)"""
        return _controls_.ToolBarBase_ClearTools(*args, **kwargs)

    def Realize(*args, **kwargs):
        """Realize(self) -> bool"""
        return _controls_.ToolBarBase_Realize(*args, **kwargs)

    def EnableTool(*args, **kwargs):
        """EnableTool(self, int id, bool enable)"""
        return _controls_.ToolBarBase_EnableTool(*args, **kwargs)

    def ToggleTool(*args, **kwargs):
        """ToggleTool(self, int id, bool toggle)"""
        return _controls_.ToolBarBase_ToggleTool(*args, **kwargs)

    def SetToggle(*args, **kwargs):
        """SetToggle(self, int id, bool toggle)"""
        return _controls_.ToolBarBase_SetToggle(*args, **kwargs)

    def GetToolClientData(*args, **kwargs):
        """GetToolClientData(self, int id) -> PyObject"""
        return _controls_.ToolBarBase_GetToolClientData(*args, **kwargs)

    def SetToolClientData(*args, **kwargs):
        """SetToolClientData(self, int id, PyObject clientData)"""
        return _controls_.ToolBarBase_SetToolClientData(*args, **kwargs)

    def GetToolPos(*args, **kwargs):
        """GetToolPos(self, int id) -> int"""
        return _controls_.ToolBarBase_GetToolPos(*args, **kwargs)

    def GetToolState(*args, **kwargs):
        """GetToolState(self, int id) -> bool"""
        return _controls_.ToolBarBase_GetToolState(*args, **kwargs)

    def GetToolEnabled(*args, **kwargs):
        """GetToolEnabled(self, int id) -> bool"""
        return _controls_.ToolBarBase_GetToolEnabled(*args, **kwargs)

    def SetToolShortHelp(*args, **kwargs):
        """SetToolShortHelp(self, int id, String helpString)"""
        return _controls_.ToolBarBase_SetToolShortHelp(*args, **kwargs)

    def GetToolShortHelp(*args, **kwargs):
        """GetToolShortHelp(self, int id) -> String"""
        return _controls_.ToolBarBase_GetToolShortHelp(*args, **kwargs)

    def SetToolLongHelp(*args, **kwargs):
        """SetToolLongHelp(self, int id, String helpString)"""
        return _controls_.ToolBarBase_SetToolLongHelp(*args, **kwargs)

    def GetToolLongHelp(*args, **kwargs):
        """GetToolLongHelp(self, int id) -> String"""
        return _controls_.ToolBarBase_GetToolLongHelp(*args, **kwargs)

    def SetMarginsXY(*args, **kwargs):
        """SetMarginsXY(self, int x, int y)"""
        return _controls_.ToolBarBase_SetMarginsXY(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(self, Size size)"""
        return _controls_.ToolBarBase_SetMargins(*args, **kwargs)

    def SetToolPacking(*args, **kwargs):
        """SetToolPacking(self, int packing)"""
        return _controls_.ToolBarBase_SetToolPacking(*args, **kwargs)

    def SetToolSeparation(*args, **kwargs):
        """SetToolSeparation(self, int separation)"""
        return _controls_.ToolBarBase_SetToolSeparation(*args, **kwargs)

    def GetToolMargins(*args, **kwargs):
        """GetToolMargins(self) -> Size"""
        return _controls_.ToolBarBase_GetToolMargins(*args, **kwargs)

    def GetMargins(*args, **kwargs):
        """GetMargins(self) -> Size"""
        return _controls_.ToolBarBase_GetMargins(*args, **kwargs)

    def GetToolPacking(*args, **kwargs):
        """GetToolPacking(self) -> int"""
        return _controls_.ToolBarBase_GetToolPacking(*args, **kwargs)

    def GetToolSeparation(*args, **kwargs):
        """GetToolSeparation(self) -> int"""
        return _controls_.ToolBarBase_GetToolSeparation(*args, **kwargs)

    def SetRows(*args, **kwargs):
        """SetRows(self, int nRows)"""
        return _controls_.ToolBarBase_SetRows(*args, **kwargs)

    def SetMaxRowsCols(*args, **kwargs):
        """SetMaxRowsCols(self, int rows, int cols)"""
        return _controls_.ToolBarBase_SetMaxRowsCols(*args, **kwargs)

    def GetMaxRows(*args, **kwargs):
        """GetMaxRows(self) -> int"""
        return _controls_.ToolBarBase_GetMaxRows(*args, **kwargs)

    def GetMaxCols(*args, **kwargs):
        """GetMaxCols(self) -> int"""
        return _controls_.ToolBarBase_GetMaxCols(*args, **kwargs)

    def SetToolBitmapSize(*args, **kwargs):
        """SetToolBitmapSize(self, Size size)"""
        return _controls_.ToolBarBase_SetToolBitmapSize(*args, **kwargs)

    def GetToolBitmapSize(*args, **kwargs):
        """GetToolBitmapSize(self) -> Size"""
        return _controls_.ToolBarBase_GetToolBitmapSize(*args, **kwargs)

    def GetToolSize(*args, **kwargs):
        """GetToolSize(self) -> Size"""
        return _controls_.ToolBarBase_GetToolSize(*args, **kwargs)

    def FindToolForPosition(*args, **kwargs):
        """FindToolForPosition(self, int x, int y) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_FindToolForPosition(*args, **kwargs)

    def FindById(*args, **kwargs):
        """FindById(self, int toolid) -> ToolBarToolBase"""
        return _controls_.ToolBarBase_FindById(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.ToolBarBase_IsVertical(*args, **kwargs)

    def GetToolsCount(*args, **kwargs):
        """GetToolsCount(self) -> size_t"""
        return _controls_.ToolBarBase_GetToolsCount(*args, **kwargs)

    Margins = property(GetMargins,SetMargins,doc="See `GetMargins` and `SetMargins`") 
    MaxCols = property(GetMaxCols,doc="See `GetMaxCols`") 
    MaxRows = property(GetMaxRows,doc="See `GetMaxRows`") 
    ToolBitmapSize = property(GetToolBitmapSize,SetToolBitmapSize,doc="See `GetToolBitmapSize` and `SetToolBitmapSize`") 
    ToolMargins = property(GetToolMargins,doc="See `GetToolMargins`") 
    ToolPacking = property(GetToolPacking,SetToolPacking,doc="See `GetToolPacking` and `SetToolPacking`") 
    ToolSeparation = property(GetToolSeparation,SetToolSeparation,doc="See `GetToolSeparation` and `SetToolSeparation`") 
    ToolSize = property(GetToolSize,doc="See `GetToolSize`") 
    ToolsCount = property(GetToolsCount,doc="See `GetToolsCount`") 
_controls_.ToolBarBase_swigregister(ToolBarBase)

class ToolBar(ToolBarBase):
    """Proxy of C++ ToolBar class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, 
            String name=wxPyToolBarNameStr) -> ToolBar
        """
        _controls_.ToolBar_swiginit(self,_controls_.new_ToolBar(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, 
            String name=wxPyToolBarNameStr) -> bool
        """
        return _controls_.ToolBar_Create(*args, **kwargs)

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
        return _controls_.ToolBar_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
_controls_.ToolBar_swigregister(ToolBar)

def PreToolBar(*args, **kwargs):
    """PreToolBar() -> ToolBar"""
    val = _controls_.new_PreToolBar(*args, **kwargs)
    return val

def ToolBar_GetClassDefaultAttributes(*args, **kwargs):
  """
    ToolBar_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ToolBar_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

LC_VRULES = _controls_.LC_VRULES
LC_HRULES = _controls_.LC_HRULES
LC_ICON = _controls_.LC_ICON
LC_SMALL_ICON = _controls_.LC_SMALL_ICON
LC_LIST = _controls_.LC_LIST
LC_REPORT = _controls_.LC_REPORT
LC_ALIGN_TOP = _controls_.LC_ALIGN_TOP
LC_ALIGN_LEFT = _controls_.LC_ALIGN_LEFT
LC_AUTOARRANGE = _controls_.LC_AUTOARRANGE
LC_VIRTUAL = _controls_.LC_VIRTUAL
LC_EDIT_LABELS = _controls_.LC_EDIT_LABELS
LC_NO_HEADER = _controls_.LC_NO_HEADER
LC_NO_SORT_HEADER = _controls_.LC_NO_SORT_HEADER
LC_SINGLE_SEL = _controls_.LC_SINGLE_SEL
LC_SORT_ASCENDING = _controls_.LC_SORT_ASCENDING
LC_SORT_DESCENDING = _controls_.LC_SORT_DESCENDING
LC_MASK_TYPE = _controls_.LC_MASK_TYPE
LC_MASK_ALIGN = _controls_.LC_MASK_ALIGN
LC_MASK_SORT = _controls_.LC_MASK_SORT
LIST_MASK_STATE = _controls_.LIST_MASK_STATE
LIST_MASK_TEXT = _controls_.LIST_MASK_TEXT
LIST_MASK_IMAGE = _controls_.LIST_MASK_IMAGE
LIST_MASK_DATA = _controls_.LIST_MASK_DATA
LIST_SET_ITEM = _controls_.LIST_SET_ITEM
LIST_MASK_WIDTH = _controls_.LIST_MASK_WIDTH
LIST_MASK_FORMAT = _controls_.LIST_MASK_FORMAT
LIST_STATE_DONTCARE = _controls_.LIST_STATE_DONTCARE
LIST_STATE_DROPHILITED = _controls_.LIST_STATE_DROPHILITED
LIST_STATE_FOCUSED = _controls_.LIST_STATE_FOCUSED
LIST_STATE_SELECTED = _controls_.LIST_STATE_SELECTED
LIST_STATE_CUT = _controls_.LIST_STATE_CUT
LIST_STATE_DISABLED = _controls_.LIST_STATE_DISABLED
LIST_STATE_FILTERED = _controls_.LIST_STATE_FILTERED
LIST_STATE_INUSE = _controls_.LIST_STATE_INUSE
LIST_STATE_PICKED = _controls_.LIST_STATE_PICKED
LIST_STATE_SOURCE = _controls_.LIST_STATE_SOURCE
LIST_HITTEST_ABOVE = _controls_.LIST_HITTEST_ABOVE
LIST_HITTEST_BELOW = _controls_.LIST_HITTEST_BELOW
LIST_HITTEST_NOWHERE = _controls_.LIST_HITTEST_NOWHERE
LIST_HITTEST_ONITEMICON = _controls_.LIST_HITTEST_ONITEMICON
LIST_HITTEST_ONITEMLABEL = _controls_.LIST_HITTEST_ONITEMLABEL
LIST_HITTEST_ONITEMRIGHT = _controls_.LIST_HITTEST_ONITEMRIGHT
LIST_HITTEST_ONITEMSTATEICON = _controls_.LIST_HITTEST_ONITEMSTATEICON
LIST_HITTEST_TOLEFT = _controls_.LIST_HITTEST_TOLEFT
LIST_HITTEST_TORIGHT = _controls_.LIST_HITTEST_TORIGHT
LIST_HITTEST_ONITEM = _controls_.LIST_HITTEST_ONITEM
LIST_GETSUBITEMRECT_WHOLEITEM = _controls_.LIST_GETSUBITEMRECT_WHOLEITEM
LIST_NEXT_ABOVE = _controls_.LIST_NEXT_ABOVE
LIST_NEXT_ALL = _controls_.LIST_NEXT_ALL
LIST_NEXT_BELOW = _controls_.LIST_NEXT_BELOW
LIST_NEXT_LEFT = _controls_.LIST_NEXT_LEFT
LIST_NEXT_RIGHT = _controls_.LIST_NEXT_RIGHT
LIST_ALIGN_DEFAULT = _controls_.LIST_ALIGN_DEFAULT
LIST_ALIGN_LEFT = _controls_.LIST_ALIGN_LEFT
LIST_ALIGN_TOP = _controls_.LIST_ALIGN_TOP
LIST_ALIGN_SNAP_TO_GRID = _controls_.LIST_ALIGN_SNAP_TO_GRID
LIST_FORMAT_LEFT = _controls_.LIST_FORMAT_LEFT
LIST_FORMAT_RIGHT = _controls_.LIST_FORMAT_RIGHT
LIST_FORMAT_CENTRE = _controls_.LIST_FORMAT_CENTRE
LIST_FORMAT_CENTER = _controls_.LIST_FORMAT_CENTER
LIST_AUTOSIZE = _controls_.LIST_AUTOSIZE
LIST_AUTOSIZE_USEHEADER = _controls_.LIST_AUTOSIZE_USEHEADER
LIST_RECT_BOUNDS = _controls_.LIST_RECT_BOUNDS
LIST_RECT_ICON = _controls_.LIST_RECT_ICON
LIST_RECT_LABEL = _controls_.LIST_RECT_LABEL
LIST_FIND_UP = _controls_.LIST_FIND_UP
LIST_FIND_DOWN = _controls_.LIST_FIND_DOWN
LIST_FIND_LEFT = _controls_.LIST_FIND_LEFT
LIST_FIND_RIGHT = _controls_.LIST_FIND_RIGHT
#---------------------------------------------------------------------------

class ListItemAttr(object):
    """Proxy of C++ ListItemAttr class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            Font font=wxNullFont) -> ListItemAttr
        """
        _controls_.ListItemAttr_swiginit(self,_controls_.new_ListItemAttr(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_ListItemAttr
    __del__ = lambda self : None;
    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _controls_.ListItemAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _controls_.ListItemAttr_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font)"""
        return _controls_.ListItemAttr_SetFont(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour(self) -> bool"""
        return _controls_.ListItemAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour(self) -> bool"""
        return _controls_.ListItemAttr_HasBackgroundColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont(self) -> bool"""
        return _controls_.ListItemAttr_HasFont(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _controls_.ListItemAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _controls_.ListItemAttr_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _controls_.ListItemAttr_GetFont(*args, **kwargs)

    def AssignFrom(*args, **kwargs):
        """AssignFrom(self, ListItemAttr source)"""
        return _controls_.ListItemAttr_AssignFrom(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        args[0].this.own(False)
        return _controls_.ListItemAttr_Destroy(*args, **kwargs)

    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
_controls_.ListItemAttr_swigregister(ListItemAttr)
ListCtrlNameStr = cvar.ListCtrlNameStr

#---------------------------------------------------------------------------

class ListItem(_core.Object):
    """Proxy of C++ ListItem class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> ListItem"""
        _controls_.ListItem_swiginit(self,_controls_.new_ListItem(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_ListItem
    __del__ = lambda self : None;
    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _controls_.ListItem_Clear(*args, **kwargs)

    def ClearAttributes(*args, **kwargs):
        """ClearAttributes(self)"""
        return _controls_.ListItem_ClearAttributes(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """SetMask(self, long mask)"""
        return _controls_.ListItem_SetMask(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, long id)"""
        return _controls_.ListItem_SetId(*args, **kwargs)

    def SetColumn(*args, **kwargs):
        """SetColumn(self, int col)"""
        return _controls_.ListItem_SetColumn(*args, **kwargs)

    def SetState(*args, **kwargs):
        """SetState(self, long state)"""
        return _controls_.ListItem_SetState(*args, **kwargs)

    def SetStateMask(*args, **kwargs):
        """SetStateMask(self, long stateMask)"""
        return _controls_.ListItem_SetStateMask(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(self, String text)"""
        return _controls_.ListItem_SetText(*args, **kwargs)

    def SetImage(*args, **kwargs):
        """SetImage(self, int image)"""
        return _controls_.ListItem_SetImage(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(self, long data)"""
        return _controls_.ListItem_SetData(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int width)"""
        return _controls_.ListItem_SetWidth(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(self, int align)"""
        return _controls_.ListItem_SetAlign(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _controls_.ListItem_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _controls_.ListItem_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font)"""
        return _controls_.ListItem_SetFont(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """GetMask(self) -> long"""
        return _controls_.ListItem_GetMask(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> long"""
        return _controls_.ListItem_GetId(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn(self) -> int"""
        return _controls_.ListItem_GetColumn(*args, **kwargs)

    def GetState(*args, **kwargs):
        """GetState(self) -> long"""
        return _controls_.ListItem_GetState(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _controls_.ListItem_GetText(*args, **kwargs)

    def GetImage(*args, **kwargs):
        """GetImage(self) -> int"""
        return _controls_.ListItem_GetImage(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData(self) -> long"""
        return _controls_.ListItem_GetData(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _controls_.ListItem_GetWidth(*args, **kwargs)

    def GetAlign(*args, **kwargs):
        """GetAlign(self) -> int"""
        return _controls_.ListItem_GetAlign(*args, **kwargs)

    def GetAttributes(*args, **kwargs):
        """GetAttributes(self) -> ListItemAttr"""
        return _controls_.ListItem_GetAttributes(*args, **kwargs)

    def HasAttributes(*args, **kwargs):
        """HasAttributes(self) -> bool"""
        return _controls_.ListItem_HasAttributes(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _controls_.ListItem_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _controls_.ListItem_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _controls_.ListItem_GetFont(*args, **kwargs)

    m_mask = property(_controls_.ListItem_m_mask_get, _controls_.ListItem_m_mask_set)
    m_itemId = property(_controls_.ListItem_m_itemId_get, _controls_.ListItem_m_itemId_set)
    m_col = property(_controls_.ListItem_m_col_get, _controls_.ListItem_m_col_set)
    m_state = property(_controls_.ListItem_m_state_get, _controls_.ListItem_m_state_set)
    m_stateMask = property(_controls_.ListItem_m_stateMask_get, _controls_.ListItem_m_stateMask_set)
    m_text = property(_controls_.ListItem_m_text_get, _controls_.ListItem_m_text_set)
    m_image = property(_controls_.ListItem_m_image_get, _controls_.ListItem_m_image_set)
    m_data = property(_controls_.ListItem_m_data_get, _controls_.ListItem_m_data_set)
    m_format = property(_controls_.ListItem_m_format_get, _controls_.ListItem_m_format_set)
    m_width = property(_controls_.ListItem_m_width_get, _controls_.ListItem_m_width_set)
    Align = property(GetAlign,SetAlign,doc="See `GetAlign` and `SetAlign`") 
    Attributes = property(GetAttributes,doc="See `GetAttributes`") 
    BackgroundColour = property(GetBackgroundColour,SetBackgroundColour,doc="See `GetBackgroundColour` and `SetBackgroundColour`") 
    Column = property(GetColumn,SetColumn,doc="See `GetColumn` and `SetColumn`") 
    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
    Image = property(GetImage,SetImage,doc="See `GetImage` and `SetImage`") 
    Mask = property(GetMask,SetMask,doc="See `GetMask` and `SetMask`") 
    State = property(GetState,SetState,doc="See `GetState` and `SetState`") 
    Text = property(GetText,SetText,doc="See `GetText` and `SetText`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
    Width = property(GetWidth,SetWidth,doc="See `GetWidth` and `SetWidth`") 
_controls_.ListItem_swigregister(ListItem)

#---------------------------------------------------------------------------

class ListEvent(_core.NotifyEvent):
    """Proxy of C++ ListEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType commandType=wxEVT_NULL, int id=0) -> ListEvent"""
        _controls_.ListEvent_swiginit(self,_controls_.new_ListEvent(*args, **kwargs))
    m_code = property(_controls_.ListEvent_m_code_get, _controls_.ListEvent_m_code_set)
    m_oldItemIndex = property(_controls_.ListEvent_m_oldItemIndex_get, _controls_.ListEvent_m_oldItemIndex_set)
    m_itemIndex = property(_controls_.ListEvent_m_itemIndex_get, _controls_.ListEvent_m_itemIndex_set)
    m_col = property(_controls_.ListEvent_m_col_get, _controls_.ListEvent_m_col_set)
    m_pointDrag = property(_controls_.ListEvent_m_pointDrag_get, _controls_.ListEvent_m_pointDrag_set)
    m_item = property(_controls_.ListEvent_m_item_get)
    def GetKeyCode(*args, **kwargs):
        """GetKeyCode(self) -> int"""
        return _controls_.ListEvent_GetKeyCode(*args, **kwargs)

    GetCode = GetKeyCode 
    def GetIndex(*args, **kwargs):
        """GetIndex(self) -> long"""
        return _controls_.ListEvent_GetIndex(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn(self) -> int"""
        return _controls_.ListEvent_GetColumn(*args, **kwargs)

    def GetPoint(*args, **kwargs):
        """GetPoint(self) -> Point"""
        return _controls_.ListEvent_GetPoint(*args, **kwargs)

    GetPosition = GetPoint 
    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> String"""
        return _controls_.ListEvent_GetLabel(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText(self) -> String"""
        return _controls_.ListEvent_GetText(*args, **kwargs)

    def GetImage(*args, **kwargs):
        """GetImage(self) -> int"""
        return _controls_.ListEvent_GetImage(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData(self) -> long"""
        return _controls_.ListEvent_GetData(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """GetMask(self) -> long"""
        return _controls_.ListEvent_GetMask(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """GetItem(self) -> ListItem"""
        return _controls_.ListEvent_GetItem(*args, **kwargs)

    def GetCacheFrom(*args, **kwargs):
        """GetCacheFrom(self) -> long"""
        return _controls_.ListEvent_GetCacheFrom(*args, **kwargs)

    def GetCacheTo(*args, **kwargs):
        """GetCacheTo(self) -> long"""
        return _controls_.ListEvent_GetCacheTo(*args, **kwargs)

    def IsEditCancelled(*args, **kwargs):
        """IsEditCancelled(self) -> bool"""
        return _controls_.ListEvent_IsEditCancelled(*args, **kwargs)

    def SetEditCanceled(*args, **kwargs):
        """SetEditCanceled(self, bool editCancelled)"""
        return _controls_.ListEvent_SetEditCanceled(*args, **kwargs)

    CacheFrom = property(GetCacheFrom,doc="See `GetCacheFrom`") 
    CacheTo = property(GetCacheTo,doc="See `GetCacheTo`") 
    Column = property(GetColumn,doc="See `GetColumn`") 
    Data = property(GetData,doc="See `GetData`") 
    Image = property(GetImage,doc="See `GetImage`") 
    Index = property(GetIndex,doc="See `GetIndex`") 
    Item = property(GetItem,doc="See `GetItem`") 
    KeyCode = property(GetKeyCode,doc="See `GetKeyCode`") 
    Label = property(GetLabel,doc="See `GetLabel`") 
    Mask = property(GetMask,doc="See `GetMask`") 
    Point = property(GetPoint,doc="See `GetPoint`") 
    Text = property(GetText,doc="See `GetText`") 
_controls_.ListEvent_swigregister(ListEvent)

wxEVT_COMMAND_LIST_BEGIN_DRAG = _controls_.wxEVT_COMMAND_LIST_BEGIN_DRAG
wxEVT_COMMAND_LIST_BEGIN_RDRAG = _controls_.wxEVT_COMMAND_LIST_BEGIN_RDRAG
wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT = _controls_.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
wxEVT_COMMAND_LIST_END_LABEL_EDIT = _controls_.wxEVT_COMMAND_LIST_END_LABEL_EDIT
wxEVT_COMMAND_LIST_DELETE_ITEM = _controls_.wxEVT_COMMAND_LIST_DELETE_ITEM
wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS = _controls_.wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
wxEVT_COMMAND_LIST_ITEM_SELECTED = _controls_.wxEVT_COMMAND_LIST_ITEM_SELECTED
wxEVT_COMMAND_LIST_ITEM_DESELECTED = _controls_.wxEVT_COMMAND_LIST_ITEM_DESELECTED
wxEVT_COMMAND_LIST_KEY_DOWN = _controls_.wxEVT_COMMAND_LIST_KEY_DOWN
wxEVT_COMMAND_LIST_INSERT_ITEM = _controls_.wxEVT_COMMAND_LIST_INSERT_ITEM
wxEVT_COMMAND_LIST_COL_CLICK = _controls_.wxEVT_COMMAND_LIST_COL_CLICK
wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK = _controls_.wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK
wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK = _controls_.wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK
wxEVT_COMMAND_LIST_ITEM_ACTIVATED = _controls_.wxEVT_COMMAND_LIST_ITEM_ACTIVATED
wxEVT_COMMAND_LIST_CACHE_HINT = _controls_.wxEVT_COMMAND_LIST_CACHE_HINT
wxEVT_COMMAND_LIST_COL_RIGHT_CLICK = _controls_.wxEVT_COMMAND_LIST_COL_RIGHT_CLICK
wxEVT_COMMAND_LIST_COL_BEGIN_DRAG = _controls_.wxEVT_COMMAND_LIST_COL_BEGIN_DRAG
wxEVT_COMMAND_LIST_COL_DRAGGING = _controls_.wxEVT_COMMAND_LIST_COL_DRAGGING
wxEVT_COMMAND_LIST_COL_END_DRAG = _controls_.wxEVT_COMMAND_LIST_COL_END_DRAG
wxEVT_COMMAND_LIST_ITEM_FOCUSED = _controls_.wxEVT_COMMAND_LIST_ITEM_FOCUSED
EVT_LIST_BEGIN_DRAG        = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_DRAG       , 1)
EVT_LIST_BEGIN_RDRAG       = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_RDRAG      , 1)
EVT_LIST_BEGIN_LABEL_EDIT  = wx.PyEventBinder(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT , 1)
EVT_LIST_END_LABEL_EDIT    = wx.PyEventBinder(wxEVT_COMMAND_LIST_END_LABEL_EDIT   , 1)
EVT_LIST_DELETE_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_LIST_DELETE_ITEM      , 1)
EVT_LIST_DELETE_ALL_ITEMS  = wx.PyEventBinder(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS , 1)




EVT_LIST_ITEM_SELECTED     = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_SELECTED    , 1)
EVT_LIST_ITEM_DESELECTED   = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_DESELECTED  , 1)
EVT_LIST_KEY_DOWN          = wx.PyEventBinder(wxEVT_COMMAND_LIST_KEY_DOWN         , 1)
EVT_LIST_INSERT_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_LIST_INSERT_ITEM      , 1)
EVT_LIST_COL_CLICK         = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_CLICK        , 1)
EVT_LIST_ITEM_RIGHT_CLICK  = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK , 1)
EVT_LIST_ITEM_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, 1)
EVT_LIST_ITEM_ACTIVATED    = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_ACTIVATED   , 1)
EVT_LIST_CACHE_HINT        = wx.PyEventBinder(wxEVT_COMMAND_LIST_CACHE_HINT       , 1)
EVT_LIST_COL_RIGHT_CLICK   = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK  , 1)
EVT_LIST_COL_BEGIN_DRAG    = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG   , 1)
EVT_LIST_COL_DRAGGING      = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_DRAGGING     , 1)
EVT_LIST_COL_END_DRAG      = wx.PyEventBinder(wxEVT_COMMAND_LIST_COL_END_DRAG     , 1)
EVT_LIST_ITEM_FOCUSED      = wx.PyEventBinder(wxEVT_COMMAND_LIST_ITEM_FOCUSED     , 1)





#---------------------------------------------------------------------------

class ListCtrl(_core.Control):
    """Proxy of C++ ListCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_ICON, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> ListCtrl
        """
        _controls_.ListCtrl_swiginit(self,_controls_.new_ListCtrl(*args, **kwargs))
        self._setOORInfo(self);ListCtrl._setCallbackInfo(self, self, ListCtrl)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_ICON, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _controls_.ListCtrl_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _controls_.ListCtrl__setCallbackInfo(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn(self, int col) -> ListItem"""
        val = _controls_.ListCtrl_GetColumn(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val

    def SetColumn(*args, **kwargs):
        """SetColumn(self, int col, ListItem item) -> bool"""
        return _controls_.ListCtrl_SetColumn(*args, **kwargs)

    def GetColumnWidth(*args, **kwargs):
        """GetColumnWidth(self, int col) -> int"""
        return _controls_.ListCtrl_GetColumnWidth(*args, **kwargs)

    def SetColumnWidth(*args, **kwargs):
        """SetColumnWidth(self, int col, int width) -> bool"""
        return _controls_.ListCtrl_SetColumnWidth(*args, **kwargs)

    def GetCountPerPage(*args, **kwargs):
        """GetCountPerPage(self) -> int"""
        return _controls_.ListCtrl_GetCountPerPage(*args, **kwargs)

    def GetViewRect(*args, **kwargs):
        """GetViewRect(self) -> Rect"""
        return _controls_.ListCtrl_GetViewRect(*args, **kwargs)

    def GetEditControl(*args, **kwargs):
        """GetEditControl(self) -> TextCtrl"""
        return _controls_.ListCtrl_GetEditControl(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """GetItem(self, long itemId, int col=0) -> ListItem"""
        val = _controls_.ListCtrl_GetItem(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val

    def SetItem(*args, **kwargs):
        """SetItem(self, ListItem info) -> bool"""
        return _controls_.ListCtrl_SetItem(*args, **kwargs)

    def SetStringItem(*args, **kwargs):
        """SetStringItem(self, long index, int col, String label, int imageId=-1) -> long"""
        return _controls_.ListCtrl_SetStringItem(*args, **kwargs)

    def GetItemState(*args, **kwargs):
        """GetItemState(self, long item, long stateMask) -> int"""
        return _controls_.ListCtrl_GetItemState(*args, **kwargs)

    def SetItemState(*args, **kwargs):
        """SetItemState(self, long item, long state, long stateMask) -> bool"""
        return _controls_.ListCtrl_SetItemState(*args, **kwargs)

    def SetItemImage(*args, **kwargs):
        """SetItemImage(self, long item, int image, int selImage=-1) -> bool"""
        return _controls_.ListCtrl_SetItemImage(*args, **kwargs)

    def SetItemColumnImage(*args, **kwargs):
        """SetItemColumnImage(self, long item, long column, int image) -> bool"""
        return _controls_.ListCtrl_SetItemColumnImage(*args, **kwargs)

    def GetItemText(*args, **kwargs):
        """GetItemText(self, long item) -> String"""
        return _controls_.ListCtrl_GetItemText(*args, **kwargs)

    def SetItemText(*args, **kwargs):
        """SetItemText(self, long item, String str)"""
        return _controls_.ListCtrl_SetItemText(*args, **kwargs)

    def GetItemData(*args, **kwargs):
        """GetItemData(self, long item) -> long"""
        return _controls_.ListCtrl_GetItemData(*args, **kwargs)

    def SetItemData(*args, **kwargs):
        """SetItemData(self, long item, long data) -> bool"""
        return _controls_.ListCtrl_SetItemData(*args, **kwargs)

    def GetItemPosition(*args, **kwargs):
        """GetItemPosition(self, long item) -> Point"""
        return _controls_.ListCtrl_GetItemPosition(*args, **kwargs)

    def GetItemRect(*args, **kwargs):
        """GetItemRect(self, long item, int code=LIST_RECT_BOUNDS) -> Rect"""
        return _controls_.ListCtrl_GetItemRect(*args, **kwargs)

    def SetItemPosition(*args, **kwargs):
        """SetItemPosition(self, long item, Point pos) -> bool"""
        return _controls_.ListCtrl_SetItemPosition(*args, **kwargs)

    def GetItemCount(*args, **kwargs):
        """GetItemCount(self) -> int"""
        return _controls_.ListCtrl_GetItemCount(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount(self) -> int"""
        return _controls_.ListCtrl_GetColumnCount(*args, **kwargs)

    def GetItemSpacing(*args, **kwargs):
        """GetItemSpacing(self) -> Size"""
        return _controls_.ListCtrl_GetItemSpacing(*args, **kwargs)

    GetItemSpacing = wx._deprecated(GetItemSpacing) 
    def SetItemSpacing(*args, **kwargs):
        """SetItemSpacing(self, int spacing, bool isSmall=False)"""
        return _controls_.ListCtrl_SetItemSpacing(*args, **kwargs)

    SetItemSpacing = wx._deprecated(SetItemSpacing) 
    def GetSelectedItemCount(*args, **kwargs):
        """GetSelectedItemCount(self) -> int"""
        return _controls_.ListCtrl_GetSelectedItemCount(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _controls_.ListCtrl_GetTextColour(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour col)"""
        return _controls_.ListCtrl_SetTextColour(*args, **kwargs)

    def GetTopItem(*args, **kwargs):
        """GetTopItem(self) -> long"""
        return _controls_.ListCtrl_GetTopItem(*args, **kwargs)

    def SetSingleStyle(*args, **kwargs):
        """SetSingleStyle(self, long style, bool add=True)"""
        return _controls_.ListCtrl_SetSingleStyle(*args, **kwargs)

    def GetNextItem(*args, **kwargs):
        """GetNextItem(self, long item, int geometry=LIST_NEXT_ALL, int state=LIST_STATE_DONTCARE) -> long"""
        return _controls_.ListCtrl_GetNextItem(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList(self, int which) -> ImageList"""
        return _controls_.ListCtrl_GetImageList(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(self, ImageList imageList, int which)"""
        return _controls_.ListCtrl_SetImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(self, ImageList imageList, int which)"""
        return _controls_.ListCtrl_AssignImageList(*args, **kwargs)

    def InReportView(*args, **kwargs):
        """InReportView(self) -> bool"""
        return _controls_.ListCtrl_InReportView(*args, **kwargs)

    def IsVirtual(*args, **kwargs):
        """IsVirtual(self) -> bool"""
        return _controls_.ListCtrl_IsVirtual(*args, **kwargs)

    def RefreshItem(*args, **kwargs):
        """RefreshItem(self, long item)"""
        return _controls_.ListCtrl_RefreshItem(*args, **kwargs)

    def RefreshItems(*args, **kwargs):
        """RefreshItems(self, long itemFrom, long itemTo)"""
        return _controls_.ListCtrl_RefreshItems(*args, **kwargs)

    def Arrange(*args, **kwargs):
        """Arrange(self, int flag=LIST_ALIGN_DEFAULT) -> bool"""
        return _controls_.ListCtrl_Arrange(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(self, long item) -> bool"""
        return _controls_.ListCtrl_DeleteItem(*args, **kwargs)

    def DeleteAllItems(*args, **kwargs):
        """DeleteAllItems(self) -> bool"""
        return _controls_.ListCtrl_DeleteAllItems(*args, **kwargs)

    def DeleteColumn(*args, **kwargs):
        """DeleteColumn(self, int col) -> bool"""
        return _controls_.ListCtrl_DeleteColumn(*args, **kwargs)

    def DeleteAllColumns(*args, **kwargs):
        """DeleteAllColumns(self) -> bool"""
        return _controls_.ListCtrl_DeleteAllColumns(*args, **kwargs)

    def ClearAll(*args, **kwargs):
        """ClearAll(self)"""
        return _controls_.ListCtrl_ClearAll(*args, **kwargs)

    def EditLabel(*args, **kwargs):
        """EditLabel(self, long item)"""
        return _controls_.ListCtrl_EditLabel(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(self, long item) -> bool"""
        return _controls_.ListCtrl_EnsureVisible(*args, **kwargs)

    def FindItem(*args, **kwargs):
        """FindItem(self, long start, String str, bool partial=False) -> long"""
        return _controls_.ListCtrl_FindItem(*args, **kwargs)

    def FindItemData(*args, **kwargs):
        """FindItemData(self, long start, long data) -> long"""
        return _controls_.ListCtrl_FindItemData(*args, **kwargs)

    def FindItemAtPos(*args, **kwargs):
        """FindItemAtPos(self, long start, Point pt, int direction) -> long"""
        return _controls_.ListCtrl_FindItemAtPos(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(Point point) -> (item, where)

        Determines which item (if any) is at the specified point, giving
         in the second return value (see wx.LIST_HITTEST flags.)
        """
        return _controls_.ListCtrl_HitTest(*args, **kwargs)

    def HitTestSubItem(*args, **kwargs):
        """
        HitTestSubItem(Point point) -> (item, where, subItem)

        Determines which item (if any) is at the specified point, giving  in
        the second return value (see wx.LIST_HITTEST flags) and also the subItem, if
        any.
        """
        return _controls_.ListCtrl_HitTestSubItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, ListItem info) -> long"""
        return _controls_.ListCtrl_InsertItem(*args, **kwargs)

    def InsertStringItem(*args, **kwargs):
        """InsertStringItem(self, long index, String label, int imageIndex=-1) -> long"""
        return _controls_.ListCtrl_InsertStringItem(*args, **kwargs)

    def InsertImageItem(*args, **kwargs):
        """InsertImageItem(self, long index, int imageIndex) -> long"""
        return _controls_.ListCtrl_InsertImageItem(*args, **kwargs)

    def InsertImageStringItem(*args, **kwargs):
        """InsertImageStringItem(self, long index, String label, int imageIndex) -> long"""
        return _controls_.ListCtrl_InsertImageStringItem(*args, **kwargs)

    def InsertColumnItem(*args, **kwargs):
        """InsertColumnItem(self, long col, ListItem info) -> long"""
        return _controls_.ListCtrl_InsertColumnItem(*args, **kwargs)

    InsertColumnInfo = InsertColumnItem 
    def InsertColumn(*args, **kwargs):
        """
        InsertColumn(self, long col, String heading, int format=LIST_FORMAT_LEFT, 
            int width=-1) -> long
        """
        return _controls_.ListCtrl_InsertColumn(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(self, long count)"""
        return _controls_.ListCtrl_SetItemCount(*args, **kwargs)

    def ScrollList(*args, **kwargs):
        """ScrollList(self, int dx, int dy) -> bool"""
        return _controls_.ListCtrl_ScrollList(*args, **kwargs)

    def SetItemTextColour(*args, **kwargs):
        """SetItemTextColour(self, long item, Colour col)"""
        return _controls_.ListCtrl_SetItemTextColour(*args, **kwargs)

    def GetItemTextColour(*args, **kwargs):
        """GetItemTextColour(self, long item) -> Colour"""
        return _controls_.ListCtrl_GetItemTextColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(self, long item, Colour col)"""
        return _controls_.ListCtrl_SetItemBackgroundColour(*args, **kwargs)

    def GetItemBackgroundColour(*args, **kwargs):
        """GetItemBackgroundColour(self, long item) -> Colour"""
        return _controls_.ListCtrl_GetItemBackgroundColour(*args, **kwargs)

    def SetItemFont(*args, **kwargs):
        """SetItemFont(self, long item, Font f)"""
        return _controls_.ListCtrl_SetItemFont(*args, **kwargs)

    def GetItemFont(*args, **kwargs):
        """GetItemFont(self, long item) -> Font"""
        return _controls_.ListCtrl_GetItemFont(*args, **kwargs)

    #
    # Some helpers...
    def Select(self, idx, on=1):
        '''[de]select an item'''
        if on: state = wx.LIST_STATE_SELECTED
        else: state = 0
        self.SetItemState(idx, state, wx.LIST_STATE_SELECTED)

    def Focus(self, idx):
        '''Focus and show the given item'''
        self.SetItemState(idx, wx.LIST_STATE_FOCUSED, wx.LIST_STATE_FOCUSED)
        self.EnsureVisible(idx)

    def GetFocusedItem(self):
        '''get the currently focused item or -1 if none'''
        return self.GetNextItem(-1, wx.LIST_NEXT_ALL, wx.LIST_STATE_FOCUSED)

    def GetFirstSelected(self, *args):
        '''return first selected item, or -1 when none'''
        return self.GetNextSelected(-1)

    def GetNextSelected(self, item):
        '''return subsequent selected items, or -1 when no more'''
        return self.GetNextItem(item, wx.LIST_NEXT_ALL, wx.LIST_STATE_SELECTED)

    def IsSelected(self, idx):
        '''return True if the item is selected'''
        return (self.GetItemState(idx, wx.LIST_STATE_SELECTED) & wx.LIST_STATE_SELECTED) != 0

    def SetColumnImage(self, col, image):
        item = self.GetColumn(col)
        # preserve all other attributes too
        item.SetMask( wx.LIST_MASK_STATE |
                      wx.LIST_MASK_TEXT  |
                      wx.LIST_MASK_IMAGE |
                      wx.LIST_MASK_DATA  |
                      wx.LIST_SET_ITEM   |
                      wx.LIST_MASK_WIDTH |
                      wx.LIST_MASK_FORMAT )
        item.SetImage(image)
        self.SetColumn(col, item)

    def ClearColumnImage(self, col):
        self.SetColumnImage(col, -1)

    def Append(self, entry):
        '''Append an item to the list control.  The entry parameter should be a
           sequence with an item for each column'''
        if len(entry):
            if wx.USE_UNICODE:
                cvtfunc = unicode
            else:
                cvtfunc = str
            pos = self.GetItemCount()
            self.InsertStringItem(pos, cvtfunc(entry[0]))
            for i in range(1, len(entry)):
                self.SetStringItem(pos, i, cvtfunc(entry[i]))
            return pos

    def SortItems(*args, **kwargs):
        """SortItems(self, PyObject func) -> bool"""
        return _controls_.ListCtrl_SortItems(*args, **kwargs)

    def GetMainWindow(*args, **kwargs):
        """GetMainWindow(self) -> Window"""
        return _controls_.ListCtrl_GetMainWindow(*args, **kwargs)

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
        return _controls_.ListCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    ColumnCount = property(GetColumnCount,doc="See `GetColumnCount`") 
    CountPerPage = property(GetCountPerPage,doc="See `GetCountPerPage`") 
    EditControl = property(GetEditControl,doc="See `GetEditControl`") 
    FocusedItem = property(GetFocusedItem,doc="See `GetFocusedItem`") 
    ImageList = property(GetImageList,SetImageList,doc="See `GetImageList` and `SetImageList`") 
    ItemCount = property(GetItemCount,SetItemCount,doc="See `GetItemCount` and `SetItemCount`") 
    MainWindow = property(GetMainWindow,doc="See `GetMainWindow`") 
    SelectedItemCount = property(GetSelectedItemCount,doc="See `GetSelectedItemCount`") 
    TextColour = property(GetTextColour,SetTextColour,doc="See `GetTextColour` and `SetTextColour`") 
    TopItem = property(GetTopItem,doc="See `GetTopItem`") 
    ViewRect = property(GetViewRect,doc="See `GetViewRect`") 
_controls_.ListCtrl_swigregister(ListCtrl)

def PreListCtrl(*args, **kwargs):
    """PreListCtrl() -> ListCtrl"""
    val = _controls_.new_PreListCtrl(*args, **kwargs)
    return val

def ListCtrl_GetClassDefaultAttributes(*args, **kwargs):
  """
    ListCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.ListCtrl_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

class ListView(ListCtrl):
    """Proxy of C++ ListView class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_REPORT, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> ListView
        """
        _controls_.ListView_swiginit(self,_controls_.new_ListView(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_REPORT, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _controls_.ListView_Create(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(self, long n, bool on=True)"""
        return _controls_.ListView_Select(*args, **kwargs)

    def Focus(*args, **kwargs):
        """Focus(self, long index)"""
        return _controls_.ListView_Focus(*args, **kwargs)

    def GetFocusedItem(*args, **kwargs):
        """GetFocusedItem(self) -> long"""
        return _controls_.ListView_GetFocusedItem(*args, **kwargs)

    def GetNextSelected(*args, **kwargs):
        """GetNextSelected(self, long item) -> long"""
        return _controls_.ListView_GetNextSelected(*args, **kwargs)

    def GetFirstSelected(*args, **kwargs):
        """GetFirstSelected(self) -> long"""
        return _controls_.ListView_GetFirstSelected(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(self, long index) -> bool"""
        return _controls_.ListView_IsSelected(*args, **kwargs)

    def SetColumnImage(*args, **kwargs):
        """SetColumnImage(self, int col, int image)"""
        return _controls_.ListView_SetColumnImage(*args, **kwargs)

    def ClearColumnImage(*args, **kwargs):
        """ClearColumnImage(self, int col)"""
        return _controls_.ListView_ClearColumnImage(*args, **kwargs)

    FocusedItem = property(GetFocusedItem,doc="See `GetFocusedItem`") 
_controls_.ListView_swigregister(ListView)

def PreListView(*args, **kwargs):
    """PreListView() -> ListView"""
    val = _controls_.new_PreListView(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

TR_NO_BUTTONS = _controls_.TR_NO_BUTTONS
TR_HAS_BUTTONS = _controls_.TR_HAS_BUTTONS
TR_NO_LINES = _controls_.TR_NO_LINES
TR_LINES_AT_ROOT = _controls_.TR_LINES_AT_ROOT
TR_SINGLE = _controls_.TR_SINGLE
TR_MULTIPLE = _controls_.TR_MULTIPLE
TR_EXTENDED = _controls_.TR_EXTENDED
TR_HAS_VARIABLE_ROW_HEIGHT = _controls_.TR_HAS_VARIABLE_ROW_HEIGHT
TR_EDIT_LABELS = _controls_.TR_EDIT_LABELS
TR_HIDE_ROOT = _controls_.TR_HIDE_ROOT
TR_ROW_LINES = _controls_.TR_ROW_LINES
TR_FULL_ROW_HIGHLIGHT = _controls_.TR_FULL_ROW_HIGHLIGHT
TR_DEFAULT_STYLE = _controls_.TR_DEFAULT_STYLE
TR_TWIST_BUTTONS = _controls_.TR_TWIST_BUTTONS
# obsolete
TR_MAC_BUTTONS = 0
wxTR_AQUA_BUTTONS = 0

TreeItemIcon_Normal = _controls_.TreeItemIcon_Normal
TreeItemIcon_Selected = _controls_.TreeItemIcon_Selected
TreeItemIcon_Expanded = _controls_.TreeItemIcon_Expanded
TreeItemIcon_SelectedExpanded = _controls_.TreeItemIcon_SelectedExpanded
TreeItemIcon_Max = _controls_.TreeItemIcon_Max
TREE_HITTEST_ABOVE = _controls_.TREE_HITTEST_ABOVE
TREE_HITTEST_BELOW = _controls_.TREE_HITTEST_BELOW
TREE_HITTEST_NOWHERE = _controls_.TREE_HITTEST_NOWHERE
TREE_HITTEST_ONITEMBUTTON = _controls_.TREE_HITTEST_ONITEMBUTTON
TREE_HITTEST_ONITEMICON = _controls_.TREE_HITTEST_ONITEMICON
TREE_HITTEST_ONITEMINDENT = _controls_.TREE_HITTEST_ONITEMINDENT
TREE_HITTEST_ONITEMLABEL = _controls_.TREE_HITTEST_ONITEMLABEL
TREE_HITTEST_ONITEMRIGHT = _controls_.TREE_HITTEST_ONITEMRIGHT
TREE_HITTEST_ONITEMSTATEICON = _controls_.TREE_HITTEST_ONITEMSTATEICON
TREE_HITTEST_TOLEFT = _controls_.TREE_HITTEST_TOLEFT
TREE_HITTEST_TORIGHT = _controls_.TREE_HITTEST_TORIGHT
TREE_HITTEST_ONITEMUPPERPART = _controls_.TREE_HITTEST_ONITEMUPPERPART
TREE_HITTEST_ONITEMLOWERPART = _controls_.TREE_HITTEST_ONITEMLOWERPART
TREE_HITTEST_ONITEM = _controls_.TREE_HITTEST_ONITEM
#---------------------------------------------------------------------------

class TreeItemId(object):
    """Proxy of C++ TreeItemId class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> TreeItemId"""
        _controls_.TreeItemId_swiginit(self,_controls_.new_TreeItemId(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_TreeItemId
    __del__ = lambda self : None;
    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _controls_.TreeItemId_IsOk(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, TreeItemId other) -> bool"""
        return _controls_.TreeItemId___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, TreeItemId other) -> bool"""
        return _controls_.TreeItemId___ne__(*args, **kwargs)

    m_pItem = property(_controls_.TreeItemId_m_pItem_get, _controls_.TreeItemId_m_pItem_set)
    Ok = IsOk
    def __nonzero__(self): return self.IsOk() 
_controls_.TreeItemId_swigregister(TreeItemId)
TreeCtrlNameStr = cvar.TreeCtrlNameStr

class TreeItemData(object):
    """Proxy of C++ TreeItemData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, PyObject obj=None) -> TreeItemData"""
        _controls_.TreeItemData_swiginit(self,_controls_.new_TreeItemData(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_TreeItemData
    __del__ = lambda self : None;
    def GetData(*args, **kwargs):
        """GetData(self) -> PyObject"""
        return _controls_.TreeItemData_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(self, PyObject obj)"""
        return _controls_.TreeItemData_SetData(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> TreeItemId"""
        return _controls_.TreeItemData_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, TreeItemId id)"""
        return _controls_.TreeItemData_SetId(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        args[0].this.own(False)
        return _controls_.TreeItemData_Destroy(*args, **kwargs)

    Data = property(GetData,SetData,doc="See `GetData` and `SetData`") 
    Id = property(GetId,SetId,doc="See `GetId` and `SetId`") 
_controls_.TreeItemData_swigregister(TreeItemData)

#---------------------------------------------------------------------------

wxEVT_COMMAND_TREE_BEGIN_DRAG = _controls_.wxEVT_COMMAND_TREE_BEGIN_DRAG
wxEVT_COMMAND_TREE_BEGIN_RDRAG = _controls_.wxEVT_COMMAND_TREE_BEGIN_RDRAG
wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT = _controls_.wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
wxEVT_COMMAND_TREE_END_LABEL_EDIT = _controls_.wxEVT_COMMAND_TREE_END_LABEL_EDIT
wxEVT_COMMAND_TREE_DELETE_ITEM = _controls_.wxEVT_COMMAND_TREE_DELETE_ITEM
wxEVT_COMMAND_TREE_GET_INFO = _controls_.wxEVT_COMMAND_TREE_GET_INFO
wxEVT_COMMAND_TREE_SET_INFO = _controls_.wxEVT_COMMAND_TREE_SET_INFO
wxEVT_COMMAND_TREE_ITEM_EXPANDED = _controls_.wxEVT_COMMAND_TREE_ITEM_EXPANDED
wxEVT_COMMAND_TREE_ITEM_EXPANDING = _controls_.wxEVT_COMMAND_TREE_ITEM_EXPANDING
wxEVT_COMMAND_TREE_ITEM_COLLAPSED = _controls_.wxEVT_COMMAND_TREE_ITEM_COLLAPSED
wxEVT_COMMAND_TREE_ITEM_COLLAPSING = _controls_.wxEVT_COMMAND_TREE_ITEM_COLLAPSING
wxEVT_COMMAND_TREE_SEL_CHANGED = _controls_.wxEVT_COMMAND_TREE_SEL_CHANGED
wxEVT_COMMAND_TREE_SEL_CHANGING = _controls_.wxEVT_COMMAND_TREE_SEL_CHANGING
wxEVT_COMMAND_TREE_KEY_DOWN = _controls_.wxEVT_COMMAND_TREE_KEY_DOWN
wxEVT_COMMAND_TREE_ITEM_ACTIVATED = _controls_.wxEVT_COMMAND_TREE_ITEM_ACTIVATED
wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK = _controls_.wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK
wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK = _controls_.wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK
wxEVT_COMMAND_TREE_END_DRAG = _controls_.wxEVT_COMMAND_TREE_END_DRAG
wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK = _controls_.wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK
wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP = _controls_.wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP
wxEVT_COMMAND_TREE_ITEM_MENU = _controls_.wxEVT_COMMAND_TREE_ITEM_MENU
EVT_TREE_BEGIN_DRAG        = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_DRAG       , 1)
EVT_TREE_BEGIN_RDRAG       = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_RDRAG      , 1)
EVT_TREE_BEGIN_LABEL_EDIT  = wx.PyEventBinder(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT , 1)
EVT_TREE_END_LABEL_EDIT    = wx.PyEventBinder(wxEVT_COMMAND_TREE_END_LABEL_EDIT   , 1)
EVT_TREE_DELETE_ITEM       = wx.PyEventBinder(wxEVT_COMMAND_TREE_DELETE_ITEM      , 1)
EVT_TREE_GET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_TREE_GET_INFO         , 1)
EVT_TREE_SET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_TREE_SET_INFO         , 1)
EVT_TREE_ITEM_EXPANDED     = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_EXPANDED    , 1)
EVT_TREE_ITEM_EXPANDING    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_EXPANDING   , 1)
EVT_TREE_ITEM_COLLAPSED    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_COLLAPSED   , 1)
EVT_TREE_ITEM_COLLAPSING   = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_COLLAPSING  , 1)
EVT_TREE_SEL_CHANGED       = wx.PyEventBinder(wxEVT_COMMAND_TREE_SEL_CHANGED      , 1)
EVT_TREE_SEL_CHANGING      = wx.PyEventBinder(wxEVT_COMMAND_TREE_SEL_CHANGING     , 1)
EVT_TREE_KEY_DOWN          = wx.PyEventBinder(wxEVT_COMMAND_TREE_KEY_DOWN         , 1)
EVT_TREE_ITEM_ACTIVATED    = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_ACTIVATED   , 1)
EVT_TREE_ITEM_RIGHT_CLICK  = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK , 1)
EVT_TREE_ITEM_MIDDLE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, 1)
EVT_TREE_END_DRAG          = wx.PyEventBinder(wxEVT_COMMAND_TREE_END_DRAG         , 1)
EVT_TREE_STATE_IMAGE_CLICK = wx.PyEventBinder(wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK, 1)
EVT_TREE_ITEM_GETTOOLTIP   = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,   1)
EVT_TREE_ITEM_MENU         = wx.PyEventBinder(wxEVT_COMMAND_TREE_ITEM_MENU,         1)

class TreeEvent(_core.NotifyEvent):
    """Proxy of C++ TreeEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, EventType commandType=wxEVT_NULL, int id=0) -> TreeEvent
        __init__(self, EventType commandType, TreeCtrl tree, TreeItemId item=NullTreeItemId) -> TreeEvent
        """
        _controls_.TreeEvent_swiginit(self,_controls_.new_TreeEvent(*args))
    def GetItem(*args, **kwargs):
        """GetItem(self) -> TreeItemId"""
        return _controls_.TreeEvent_GetItem(*args, **kwargs)

    def SetItem(*args, **kwargs):
        """SetItem(self, TreeItemId item)"""
        return _controls_.TreeEvent_SetItem(*args, **kwargs)

    def GetOldItem(*args, **kwargs):
        """GetOldItem(self) -> TreeItemId"""
        return _controls_.TreeEvent_GetOldItem(*args, **kwargs)

    def SetOldItem(*args, **kwargs):
        """SetOldItem(self, TreeItemId item)"""
        return _controls_.TreeEvent_SetOldItem(*args, **kwargs)

    def GetPoint(*args, **kwargs):
        """GetPoint(self) -> Point"""
        return _controls_.TreeEvent_GetPoint(*args, **kwargs)

    def SetPoint(*args, **kwargs):
        """SetPoint(self, Point pt)"""
        return _controls_.TreeEvent_SetPoint(*args, **kwargs)

    def GetKeyEvent(*args, **kwargs):
        """GetKeyEvent(self) -> KeyEvent"""
        return _controls_.TreeEvent_GetKeyEvent(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """GetKeyCode(self) -> int"""
        return _controls_.TreeEvent_GetKeyCode(*args, **kwargs)

    def SetKeyEvent(*args, **kwargs):
        """SetKeyEvent(self, KeyEvent evt)"""
        return _controls_.TreeEvent_SetKeyEvent(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel(self) -> String"""
        return _controls_.TreeEvent_GetLabel(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(self, String label)"""
        return _controls_.TreeEvent_SetLabel(*args, **kwargs)

    def IsEditCancelled(*args, **kwargs):
        """IsEditCancelled(self) -> bool"""
        return _controls_.TreeEvent_IsEditCancelled(*args, **kwargs)

    def SetEditCanceled(*args, **kwargs):
        """SetEditCanceled(self, bool editCancelled)"""
        return _controls_.TreeEvent_SetEditCanceled(*args, **kwargs)

    def SetToolTip(*args, **kwargs):
        """SetToolTip(self, String toolTip)"""
        return _controls_.TreeEvent_SetToolTip(*args, **kwargs)

    def GetToolTip(*args, **kwargs):
        """GetToolTip(self) -> String"""
        return _controls_.TreeEvent_GetToolTip(*args, **kwargs)

    Item = property(GetItem,SetItem,doc="See `GetItem` and `SetItem`") 
    KeyCode = property(GetKeyCode,doc="See `GetKeyCode`") 
    KeyEvent = property(GetKeyEvent,SetKeyEvent,doc="See `GetKeyEvent` and `SetKeyEvent`") 
    Label = property(GetLabel,SetLabel,doc="See `GetLabel` and `SetLabel`") 
    OldItem = property(GetOldItem,SetOldItem,doc="See `GetOldItem` and `SetOldItem`") 
    Point = property(GetPoint,SetPoint,doc="See `GetPoint` and `SetPoint`") 
    ToolTip = property(GetToolTip,SetToolTip,doc="See `GetToolTip` and `SetToolTip`") 
    EditCancelled = property(IsEditCancelled,SetEditCanceled,doc="See `IsEditCancelled` and `SetEditCanceled`") 
_controls_.TreeEvent_swigregister(TreeEvent)

#---------------------------------------------------------------------------

class TreeCtrl(_core.Control):
    """Proxy of C++ TreeCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=TreeCtrlNameStr) -> TreeCtrl
        """
        _controls_.TreeCtrl_swiginit(self,_controls_.new_TreeCtrl(*args, **kwargs))
        self._setOORInfo(self);TreeCtrl._setCallbackInfo(self, self, TreeCtrl)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=TreeCtrlNameStr) -> bool

        Do the 2nd phase and create the GUI control.
        """
        return _controls_.TreeCtrl_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _controls_.TreeCtrl__setCallbackInfo(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> unsigned int"""
        return _controls_.TreeCtrl_GetCount(*args, **kwargs)

    def GetIndent(*args, **kwargs):
        """GetIndent(self) -> unsigned int"""
        return _controls_.TreeCtrl_GetIndent(*args, **kwargs)

    def SetIndent(*args, **kwargs):
        """SetIndent(self, unsigned int indent)"""
        return _controls_.TreeCtrl_SetIndent(*args, **kwargs)

    def GetSpacing(*args, **kwargs):
        """GetSpacing(self) -> unsigned int"""
        return _controls_.TreeCtrl_GetSpacing(*args, **kwargs)

    def SetSpacing(*args, **kwargs):
        """SetSpacing(self, unsigned int spacing)"""
        return _controls_.TreeCtrl_SetSpacing(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList(self) -> ImageList"""
        return _controls_.TreeCtrl_GetImageList(*args, **kwargs)

    def GetStateImageList(*args, **kwargs):
        """GetStateImageList(self) -> ImageList"""
        return _controls_.TreeCtrl_GetStateImageList(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(self, ImageList imageList)"""
        return _controls_.TreeCtrl_SetImageList(*args, **kwargs)

    def SetStateImageList(*args, **kwargs):
        """SetStateImageList(self, ImageList imageList)"""
        return _controls_.TreeCtrl_SetStateImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(self, ImageList imageList)"""
        return _controls_.TreeCtrl_AssignImageList(*args, **kwargs)

    def AssignStateImageList(*args, **kwargs):
        """AssignStateImageList(self, ImageList imageList)"""
        return _controls_.TreeCtrl_AssignStateImageList(*args, **kwargs)

    def GetItemText(*args, **kwargs):
        """GetItemText(self, TreeItemId item) -> String"""
        return _controls_.TreeCtrl_GetItemText(*args, **kwargs)

    def GetItemImage(*args, **kwargs):
        """GetItemImage(self, TreeItemId item, int which=TreeItemIcon_Normal) -> int"""
        return _controls_.TreeCtrl_GetItemImage(*args, **kwargs)

    def GetItemData(*args, **kwargs):
        """GetItemData(self, TreeItemId item) -> TreeItemData"""
        return _controls_.TreeCtrl_GetItemData(*args, **kwargs)

    def GetItemPyData(*args, **kwargs):
        """GetItemPyData(self, TreeItemId item) -> PyObject"""
        return _controls_.TreeCtrl_GetItemPyData(*args, **kwargs)

    GetPyData = GetItemPyData 
    def GetItemTextColour(*args, **kwargs):
        """GetItemTextColour(self, TreeItemId item) -> Colour"""
        return _controls_.TreeCtrl_GetItemTextColour(*args, **kwargs)

    def GetItemBackgroundColour(*args, **kwargs):
        """GetItemBackgroundColour(self, TreeItemId item) -> Colour"""
        return _controls_.TreeCtrl_GetItemBackgroundColour(*args, **kwargs)

    def GetItemFont(*args, **kwargs):
        """GetItemFont(self, TreeItemId item) -> Font"""
        return _controls_.TreeCtrl_GetItemFont(*args, **kwargs)

    def SetItemText(*args, **kwargs):
        """SetItemText(self, TreeItemId item, String text)"""
        return _controls_.TreeCtrl_SetItemText(*args, **kwargs)

    def SetItemImage(*args, **kwargs):
        """SetItemImage(self, TreeItemId item, int image, int which=TreeItemIcon_Normal)"""
        return _controls_.TreeCtrl_SetItemImage(*args, **kwargs)

    def SetItemData(*args, **kwargs):
        """SetItemData(self, TreeItemId item, TreeItemData data)"""
        return _controls_.TreeCtrl_SetItemData(*args, **kwargs)

    def SetItemPyData(*args, **kwargs):
        """SetItemPyData(self, TreeItemId item, PyObject obj)"""
        return _controls_.TreeCtrl_SetItemPyData(*args, **kwargs)

    SetPyData = SetItemPyData 
    def SetItemHasChildren(*args, **kwargs):
        """SetItemHasChildren(self, TreeItemId item, bool has=True)"""
        return _controls_.TreeCtrl_SetItemHasChildren(*args, **kwargs)

    def SetItemBold(*args, **kwargs):
        """SetItemBold(self, TreeItemId item, bool bold=True)"""
        return _controls_.TreeCtrl_SetItemBold(*args, **kwargs)

    def SetItemDropHighlight(*args, **kwargs):
        """SetItemDropHighlight(self, TreeItemId item, bool highlight=True)"""
        return _controls_.TreeCtrl_SetItemDropHighlight(*args, **kwargs)

    def SetItemTextColour(*args, **kwargs):
        """SetItemTextColour(self, TreeItemId item, Colour col)"""
        return _controls_.TreeCtrl_SetItemTextColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(self, TreeItemId item, Colour col)"""
        return _controls_.TreeCtrl_SetItemBackgroundColour(*args, **kwargs)

    def SetItemFont(*args, **kwargs):
        """SetItemFont(self, TreeItemId item, Font font)"""
        return _controls_.TreeCtrl_SetItemFont(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(self, TreeItemId item) -> bool"""
        return _controls_.TreeCtrl_IsVisible(*args, **kwargs)

    def ItemHasChildren(*args, **kwargs):
        """ItemHasChildren(self, TreeItemId item) -> bool"""
        return _controls_.TreeCtrl_ItemHasChildren(*args, **kwargs)

    def IsExpanded(*args, **kwargs):
        """IsExpanded(self, TreeItemId item) -> bool"""
        return _controls_.TreeCtrl_IsExpanded(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(self, TreeItemId item) -> bool"""
        return _controls_.TreeCtrl_IsSelected(*args, **kwargs)

    def IsBold(*args, **kwargs):
        """IsBold(self, TreeItemId item) -> bool"""
        return _controls_.TreeCtrl_IsBold(*args, **kwargs)

    def GetChildrenCount(*args, **kwargs):
        """GetChildrenCount(self, TreeItemId item, bool recursively=True) -> size_t"""
        return _controls_.TreeCtrl_GetChildrenCount(*args, **kwargs)

    def GetRootItem(*args, **kwargs):
        """GetRootItem(self) -> TreeItemId"""
        return _controls_.TreeCtrl_GetRootItem(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> TreeItemId"""
        return _controls_.TreeCtrl_GetSelection(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections(self) -> PyObject"""
        return _controls_.TreeCtrl_GetSelections(*args, **kwargs)

    def GetItemParent(*args, **kwargs):
        """GetItemParent(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetItemParent(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild(self, TreeItemId item) -> PyObject"""
        return _controls_.TreeCtrl_GetFirstChild(*args, **kwargs)

    def GetNextChild(*args, **kwargs):
        """GetNextChild(self, TreeItemId item, void cookie) -> PyObject"""
        return _controls_.TreeCtrl_GetNextChild(*args, **kwargs)

    def GetLastChild(*args, **kwargs):
        """GetLastChild(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetLastChild(*args, **kwargs)

    def GetNextSibling(*args, **kwargs):
        """GetNextSibling(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetNextSibling(*args, **kwargs)

    def GetPrevSibling(*args, **kwargs):
        """GetPrevSibling(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetPrevSibling(*args, **kwargs)

    def GetFirstVisibleItem(*args, **kwargs):
        """GetFirstVisibleItem(self) -> TreeItemId"""
        return _controls_.TreeCtrl_GetFirstVisibleItem(*args, **kwargs)

    def GetNextVisible(*args, **kwargs):
        """GetNextVisible(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetNextVisible(*args, **kwargs)

    def GetPrevVisible(*args, **kwargs):
        """GetPrevVisible(self, TreeItemId item) -> TreeItemId"""
        return _controls_.TreeCtrl_GetPrevVisible(*args, **kwargs)

    def AddRoot(*args, **kwargs):
        """AddRoot(self, String text, int image=-1, int selectedImage=-1, TreeItemData data=None) -> TreeItemId"""
        return _controls_.TreeCtrl_AddRoot(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """
        PrependItem(self, TreeItemId parent, String text, int image=-1, int selectedImage=-1, 
            TreeItemData data=None) -> TreeItemId
        """
        return _controls_.TreeCtrl_PrependItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """
        InsertItem(self, TreeItemId parent, TreeItemId idPrevious, String text, 
            int image=-1, int selectedImage=-1, TreeItemData data=None) -> TreeItemId
        """
        return _controls_.TreeCtrl_InsertItem(*args, **kwargs)

    def InsertItemBefore(*args, **kwargs):
        """
        InsertItemBefore(self, TreeItemId parent, size_t index, String text, int image=-1, 
            int selectedImage=-1, TreeItemData data=None) -> TreeItemId
        """
        return _controls_.TreeCtrl_InsertItemBefore(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """
        AppendItem(self, TreeItemId parent, String text, int image=-1, int selectedImage=-1, 
            TreeItemData data=None) -> TreeItemId
        """
        return _controls_.TreeCtrl_AppendItem(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(self, TreeItemId item)"""
        return _controls_.TreeCtrl_Delete(*args, **kwargs)

    def DeleteChildren(*args, **kwargs):
        """DeleteChildren(self, TreeItemId item)"""
        return _controls_.TreeCtrl_DeleteChildren(*args, **kwargs)

    def DeleteAllItems(*args, **kwargs):
        """DeleteAllItems(self)"""
        return _controls_.TreeCtrl_DeleteAllItems(*args, **kwargs)

    def Expand(*args, **kwargs):
        """Expand(self, TreeItemId item)"""
        return _controls_.TreeCtrl_Expand(*args, **kwargs)

    def ExpandAllChildren(*args, **kwargs):
        """ExpandAllChildren(self, TreeItemId item)"""
        return _controls_.TreeCtrl_ExpandAllChildren(*args, **kwargs)

    def ExpandAll(*args, **kwargs):
        """ExpandAll(self)"""
        return _controls_.TreeCtrl_ExpandAll(*args, **kwargs)

    def Collapse(*args, **kwargs):
        """Collapse(self, TreeItemId item)"""
        return _controls_.TreeCtrl_Collapse(*args, **kwargs)

    def CollapseAndReset(*args, **kwargs):
        """CollapseAndReset(self, TreeItemId item)"""
        return _controls_.TreeCtrl_CollapseAndReset(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(self, TreeItemId item)"""
        return _controls_.TreeCtrl_Toggle(*args, **kwargs)

    def Unselect(*args, **kwargs):
        """Unselect(self)"""
        return _controls_.TreeCtrl_Unselect(*args, **kwargs)

    def UnselectItem(*args, **kwargs):
        """UnselectItem(self, TreeItemId item)"""
        return _controls_.TreeCtrl_UnselectItem(*args, **kwargs)

    def UnselectAll(*args, **kwargs):
        """UnselectAll(self)"""
        return _controls_.TreeCtrl_UnselectAll(*args, **kwargs)

    def SelectItem(*args, **kwargs):
        """SelectItem(self, TreeItemId item, bool select=True)"""
        return _controls_.TreeCtrl_SelectItem(*args, **kwargs)

    def ToggleItemSelection(*args, **kwargs):
        """ToggleItemSelection(self, TreeItemId item)"""
        return _controls_.TreeCtrl_ToggleItemSelection(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(self, TreeItemId item)"""
        return _controls_.TreeCtrl_EnsureVisible(*args, **kwargs)

    def ScrollTo(*args, **kwargs):
        """ScrollTo(self, TreeItemId item)"""
        return _controls_.TreeCtrl_ScrollTo(*args, **kwargs)

    def EditLabel(*args, **kwargs):
        """EditLabel(self, TreeItemId item)"""
        return _controls_.TreeCtrl_EditLabel(*args, **kwargs)

    def GetEditControl(*args, **kwargs):
        """GetEditControl(self) -> TextCtrl"""
        return _controls_.TreeCtrl_GetEditControl(*args, **kwargs)

    def SortChildren(*args, **kwargs):
        """SortChildren(self, TreeItemId item)"""
        return _controls_.TreeCtrl_SortChildren(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(Point point) -> (item, where)

        Determine which item (if any) belongs the given point.  The coordinates
        specified are relative to the client area of tree ctrl and the where return
        value is set to a bitmask of wxTREE_HITTEST_xxx constants.

        """
        return _controls_.TreeCtrl_HitTest(*args, **kwargs)

    def GetBoundingRect(*args, **kwargs):
        """GetBoundingRect(self, TreeItemId item, bool textOnly=False) -> PyObject"""
        return _controls_.TreeCtrl_GetBoundingRect(*args, **kwargs)

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
        return _controls_.TreeCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)
    def SetQuickBestSize(*args, **kwargs):
        """SetQuickBestSize(self, bool q)"""
        return _controls_.TreeCtrl_SetQuickBestSize(*args, **kwargs)

    def GetQuickBestSize(*args, **kwargs):
        """GetQuickBestSize(self) -> bool"""
        return _controls_.TreeCtrl_GetQuickBestSize(*args, **kwargs)

    Count = property(GetCount,doc="See `GetCount`") 
    EditControl = property(GetEditControl,doc="See `GetEditControl`") 
    FirstVisibleItem = property(GetFirstVisibleItem,doc="See `GetFirstVisibleItem`") 
    ImageList = property(GetImageList,SetImageList,doc="See `GetImageList` and `SetImageList`") 
    Indent = property(GetIndent,SetIndent,doc="See `GetIndent` and `SetIndent`") 
    QuickBestSize = property(GetQuickBestSize,SetQuickBestSize,doc="See `GetQuickBestSize` and `SetQuickBestSize`") 
    RootItem = property(GetRootItem,doc="See `GetRootItem`") 
    Selection = property(GetSelection,doc="See `GetSelection`") 
    Selections = property(GetSelections,doc="See `GetSelections`") 
    Spacing = property(GetSpacing,SetSpacing,doc="See `GetSpacing` and `SetSpacing`") 
    StateImageList = property(GetStateImageList,SetStateImageList,doc="See `GetStateImageList` and `SetStateImageList`") 
_controls_.TreeCtrl_swigregister(TreeCtrl)

def PreTreeCtrl(*args, **kwargs):
    """PreTreeCtrl() -> TreeCtrl"""
    val = _controls_.new_PreTreeCtrl(*args, **kwargs)
    return val

def TreeCtrl_GetClassDefaultAttributes(*args, **kwargs):
  """
    TreeCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
  return _controls_.TreeCtrl_GetClassDefaultAttributes(*args, **kwargs)

#---------------------------------------------------------------------------

DIRCTRL_DIR_ONLY = _controls_.DIRCTRL_DIR_ONLY
DIRCTRL_SELECT_FIRST = _controls_.DIRCTRL_SELECT_FIRST
DIRCTRL_SHOW_FILTERS = _controls_.DIRCTRL_SHOW_FILTERS
DIRCTRL_3D_INTERNAL = _controls_.DIRCTRL_3D_INTERNAL
DIRCTRL_EDIT_LABELS = _controls_.DIRCTRL_EDIT_LABELS
class GenericDirCtrl(_core.Control):
    """Proxy of C++ GenericDirCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String dir=DirDialogDefaultFolderStr, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, 
            String filter=EmptyString, 
            int defaultFilter=0, String name=TreeCtrlNameStr) -> GenericDirCtrl
        """
        _controls_.GenericDirCtrl_swiginit(self,_controls_.new_GenericDirCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String dir=DirDialogDefaultFolderStr, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, 
            String filter=EmptyString, 
            int defaultFilter=0, String name=TreeCtrlNameStr) -> bool
        """
        return _controls_.GenericDirCtrl_Create(*args, **kwargs)

    def ExpandPath(*args, **kwargs):
        """ExpandPath(self, String path) -> bool"""
        return _controls_.GenericDirCtrl_ExpandPath(*args, **kwargs)

    def CollapsePath(*args, **kwargs):
        """CollapsePath(self, String path) -> bool"""
        return _controls_.GenericDirCtrl_CollapsePath(*args, **kwargs)

    def GetDefaultPath(*args, **kwargs):
        """GetDefaultPath(self) -> String"""
        return _controls_.GenericDirCtrl_GetDefaultPath(*args, **kwargs)

    def SetDefaultPath(*args, **kwargs):
        """SetDefaultPath(self, String path)"""
        return _controls_.GenericDirCtrl_SetDefaultPath(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath(self) -> String"""
        return _controls_.GenericDirCtrl_GetPath(*args, **kwargs)

    def GetFilePath(*args, **kwargs):
        """GetFilePath(self) -> String"""
        return _controls_.GenericDirCtrl_GetFilePath(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(self, String path)"""
        return _controls_.GenericDirCtrl_SetPath(*args, **kwargs)

    def ShowHidden(*args, **kwargs):
        """ShowHidden(self, bool show)"""
        return _controls_.GenericDirCtrl_ShowHidden(*args, **kwargs)

    def GetShowHidden(*args, **kwargs):
        """GetShowHidden(self) -> bool"""
        return _controls_.GenericDirCtrl_GetShowHidden(*args, **kwargs)

    def GetFilter(*args, **kwargs):
        """GetFilter(self) -> String"""
        return _controls_.GenericDirCtrl_GetFilter(*args, **kwargs)

    def SetFilter(*args, **kwargs):
        """SetFilter(self, String filter)"""
        return _controls_.GenericDirCtrl_SetFilter(*args, **kwargs)

    def GetFilterIndex(*args, **kwargs):
        """GetFilterIndex(self) -> int"""
        return _controls_.GenericDirCtrl_GetFilterIndex(*args, **kwargs)

    def SetFilterIndex(*args, **kwargs):
        """SetFilterIndex(self, int n)"""
        return _controls_.GenericDirCtrl_SetFilterIndex(*args, **kwargs)

    def GetRootId(*args, **kwargs):
        """GetRootId(self) -> TreeItemId"""
        return _controls_.GenericDirCtrl_GetRootId(*args, **kwargs)

    def GetTreeCtrl(*args, **kwargs):
        """GetTreeCtrl(self) -> TreeCtrl"""
        return _controls_.GenericDirCtrl_GetTreeCtrl(*args, **kwargs)

    def GetFilterListCtrl(*args, **kwargs):
        """GetFilterListCtrl(self) -> DirFilterListCtrl"""
        return _controls_.GenericDirCtrl_GetFilterListCtrl(*args, **kwargs)

    def FindChild(*args, **kwargs):
        """
        FindChild(wxTreeItemId parentId, wxString path) -> (item, done)

        Find the child that matches the first part of 'path'.  E.g. if a child
        path is "/usr" and 'path' is "/usr/include" then the child for
        /usr is returned.  If the path string has been used (we're at the
        leaf), done is set to True.

        """
        return _controls_.GenericDirCtrl_FindChild(*args, **kwargs)

    def DoResize(*args, **kwargs):
        """DoResize(self)"""
        return _controls_.GenericDirCtrl_DoResize(*args, **kwargs)

    def ReCreateTree(*args, **kwargs):
        """ReCreateTree(self)"""
        return _controls_.GenericDirCtrl_ReCreateTree(*args, **kwargs)

    DefaultPath = property(GetDefaultPath,SetDefaultPath,doc="See `GetDefaultPath` and `SetDefaultPath`") 
    FilePath = property(GetFilePath,doc="See `GetFilePath`") 
    Filter = property(GetFilter,SetFilter,doc="See `GetFilter` and `SetFilter`") 
    FilterIndex = property(GetFilterIndex,SetFilterIndex,doc="See `GetFilterIndex` and `SetFilterIndex`") 
    FilterListCtrl = property(GetFilterListCtrl,doc="See `GetFilterListCtrl`") 
    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
    RootId = property(GetRootId,doc="See `GetRootId`") 
    TreeCtrl = property(GetTreeCtrl,doc="See `GetTreeCtrl`") 
_controls_.GenericDirCtrl_swigregister(GenericDirCtrl)
DirDialogDefaultFolderStr = cvar.DirDialogDefaultFolderStr

def PreGenericDirCtrl(*args, **kwargs):
    """PreGenericDirCtrl() -> GenericDirCtrl"""
    val = _controls_.new_PreGenericDirCtrl(*args, **kwargs)
    return val

class DirFilterListCtrl(Choice):
    """Proxy of C++ DirFilterListCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, GenericDirCtrl parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> DirFilterListCtrl
        """
        _controls_.DirFilterListCtrl_swiginit(self,_controls_.new_DirFilterListCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, GenericDirCtrl parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> bool
        """
        return _controls_.DirFilterListCtrl_Create(*args, **kwargs)

    def FillFilterList(*args, **kwargs):
        """FillFilterList(self, String filter, int defaultFilter)"""
        return _controls_.DirFilterListCtrl_FillFilterList(*args, **kwargs)

_controls_.DirFilterListCtrl_swigregister(DirFilterListCtrl)

def PreDirFilterListCtrl(*args, **kwargs):
    """PreDirFilterListCtrl() -> DirFilterListCtrl"""
    val = _controls_.new_PreDirFilterListCtrl(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

class PyControl(_core.Control):
    """Proxy of C++ PyControl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> PyControl
        """
        _controls_.PyControl_swiginit(self,_controls_.new_PyControl(*args, **kwargs))
        self._setOORInfo(self);PyControl._setCallbackInfo(self, self, PyControl)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _controls_.PyControl__setCallbackInfo(*args, **kwargs)

    SetBestSize = wx.Window.SetInitialSize 
    def DoEraseBackground(*args, **kwargs):
        """DoEraseBackground(self, DC dc) -> bool"""
        return _controls_.PyControl_DoEraseBackground(*args, **kwargs)

    def DoMoveWindow(*args, **kwargs):
        """DoMoveWindow(self, int x, int y, int width, int height)"""
        return _controls_.PyControl_DoMoveWindow(*args, **kwargs)

    def DoSetSize(*args, **kwargs):
        """DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _controls_.PyControl_DoSetSize(*args, **kwargs)

    def DoSetClientSize(*args, **kwargs):
        """DoSetClientSize(self, int width, int height)"""
        return _controls_.PyControl_DoSetClientSize(*args, **kwargs)

    def DoSetVirtualSize(*args, **kwargs):
        """DoSetVirtualSize(self, int x, int y)"""
        return _controls_.PyControl_DoSetVirtualSize(*args, **kwargs)

    def DoGetSize(*args, **kwargs):
        """DoGetSize() -> (width, height)"""
        return _controls_.PyControl_DoGetSize(*args, **kwargs)

    def DoGetClientSize(*args, **kwargs):
        """DoGetClientSize() -> (width, height)"""
        return _controls_.PyControl_DoGetClientSize(*args, **kwargs)

    def DoGetPosition(*args, **kwargs):
        """DoGetPosition() -> (x,y)"""
        return _controls_.PyControl_DoGetPosition(*args, **kwargs)

    def DoGetVirtualSize(*args, **kwargs):
        """DoGetVirtualSize(self) -> Size"""
        return _controls_.PyControl_DoGetVirtualSize(*args, **kwargs)

    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _controls_.PyControl_DoGetBestSize(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """GetDefaultAttributes(self) -> VisualAttributes"""
        return _controls_.PyControl_GetDefaultAttributes(*args, **kwargs)

    def OnInternalIdle(*args, **kwargs):
        """OnInternalIdle(self)"""
        return _controls_.PyControl_OnInternalIdle(*args, **kwargs)

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

_controls_.PyControl_swigregister(PyControl)

def PrePyControl(*args, **kwargs):
    """PrePyControl() -> PyControl"""
    val = _controls_.new_PrePyControl(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

wxEVT_HELP = _controls_.wxEVT_HELP
wxEVT_DETAILED_HELP = _controls_.wxEVT_DETAILED_HELP
EVT_HELP = wx.PyEventBinder( wxEVT_HELP, 1)
EVT_HELP_RANGE = wx.PyEventBinder(  wxEVT_HELP, 2)
EVT_DETAILED_HELP = wx.PyEventBinder( wxEVT_DETAILED_HELP, 1)
EVT_DETAILED_HELP_RANGE = wx.PyEventBinder( wxEVT_DETAILED_HELP, 2)

class HelpEvent(_core.CommandEvent):
    """
    A help event is sent when the user has requested context-sensitive
    help. This can either be caused by the application requesting
    context-sensitive help mode via wx.ContextHelp, or (on MS Windows) by
    the system generating a WM_HELP message when the user pressed F1 or
    clicked on the query button in a dialog caption.

    A help event is sent to the window that the user clicked on, and is
    propagated up the window hierarchy until the event is processed or
    there are no more event handlers. The application should call
    event.GetId to check the identity of the clicked-on window, and then
    either show some suitable help or call event.Skip if the identifier is
    unrecognised. Calling Skip is important because it allows wxWindows to
    generate further events for ancestors of the clicked-on
    window. Otherwise it would be impossible to show help for container
    windows, since processing would stop after the first window found.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    Origin_Unknown = _controls_.HelpEvent_Origin_Unknown
    Origin_Keyboard = _controls_.HelpEvent_Origin_Keyboard
    Origin_HelpButton = _controls_.HelpEvent_Origin_HelpButton
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, EventType type=wxEVT_NULL, int winid=0, Point pt=DefaultPosition, 
            int origin=Origin_Unknown) -> HelpEvent
        """
        _controls_.HelpEvent_swiginit(self,_controls_.new_HelpEvent(*args, **kwargs))
    def GetPosition(*args, **kwargs):
        """
        GetPosition(self) -> Point

        Returns the left-click position of the mouse, in screen
        coordinates. This allows the application to position the help
        appropriately.
        """
        return _controls_.HelpEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """
        SetPosition(self, Point pos)

        Sets the left-click position of the mouse, in screen coordinates.
        """
        return _controls_.HelpEvent_SetPosition(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """
        GetLink(self) -> String

        Get an optional link to further help
        """
        return _controls_.HelpEvent_GetLink(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """
        SetLink(self, String link)

        Set an optional link to further help
        """
        return _controls_.HelpEvent_SetLink(*args, **kwargs)

    def GetTarget(*args, **kwargs):
        """
        GetTarget(self) -> String

        Get an optional target to display help in. E.g. a window specification
        """
        return _controls_.HelpEvent_GetTarget(*args, **kwargs)

    def SetTarget(*args, **kwargs):
        """
        SetTarget(self, String target)

        Set an optional target to display help in. E.g. a window specification
        """
        return _controls_.HelpEvent_SetTarget(*args, **kwargs)

    def GetOrigin(*args, **kwargs):
        """
        GetOrigin(self) -> int

        Optiononal indication of the source of the event.
        """
        return _controls_.HelpEvent_GetOrigin(*args, **kwargs)

    def SetOrigin(*args, **kwargs):
        """SetOrigin(self, int origin)"""
        return _controls_.HelpEvent_SetOrigin(*args, **kwargs)

    Link = property(GetLink,SetLink,doc="See `GetLink` and `SetLink`") 
    Origin = property(GetOrigin,SetOrigin,doc="See `GetOrigin` and `SetOrigin`") 
    Position = property(GetPosition,SetPosition,doc="See `GetPosition` and `SetPosition`") 
    Target = property(GetTarget,SetTarget,doc="See `GetTarget` and `SetTarget`") 
_controls_.HelpEvent_swigregister(HelpEvent)

class ContextHelp(_core.Object):
    """
    This class changes the cursor to a query and puts the application into
    a 'context-sensitive help mode'. When the user left-clicks on a window
    within the specified window, a ``EVT_HELP`` event is sent to that
    control, and the application may respond to it by popping up some
    help.

    There are a couple of ways to invoke this behaviour implicitly:

        * Use the wx.WS_EX_CONTEXTHELP extended style for a dialog or frame
          (Windows only). This will put a question mark in the titlebar,
          and Windows will put the application into context-sensitive help
          mode automatically, with further programming.

        * Create a `wx.ContextHelpButton`, whose predefined behaviour is
          to create a context help object. Normally you will write your
          application so that this button is only added to a dialog for
          non-Windows platforms (use ``wx.WS_EX_CONTEXTHELP`` on
          Windows).

    :see: `wx.ContextHelpButton`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window window=None, bool doNow=True) -> ContextHelp

        Constructs a context help object, calling BeginContextHelp if doNow is
        true (the default).

        If window is None, the top window is used.
        """
        _controls_.ContextHelp_swiginit(self,_controls_.new_ContextHelp(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_ContextHelp
    __del__ = lambda self : None;
    def BeginContextHelp(*args, **kwargs):
        """
        BeginContextHelp(self, Window window=None) -> bool

        Puts the application into context-sensitive help mode. window is the
        window which will be used to catch events; if NULL, the top window
        will be used.

        Returns true if the application was successfully put into
        context-sensitive help mode. This function only returns when the event
        loop has finished.
        """
        return _controls_.ContextHelp_BeginContextHelp(*args, **kwargs)

    def EndContextHelp(*args, **kwargs):
        """
        EndContextHelp(self) -> bool

        Ends context-sensitive help mode. Not normally called by the
        application.
        """
        return _controls_.ContextHelp_EndContextHelp(*args, **kwargs)

_controls_.ContextHelp_swigregister(ContextHelp)

class ContextHelpButton(BitmapButton):
    """
    Instances of this class may be used to add a question mark button that
    when pressed, puts the application into context-help mode. It does
    this by creating a wx.ContextHelp object which itself generates a
    ``EVT_HELP`` event when the user clicks on a window.

    On Windows, you may add a question-mark icon to a dialog by use of the
    ``wx.DIALOG_EX_CONTEXTHELP`` extra style, but on other platforms you
    will have to add a button explicitly, usually next to OK, Cancel or
    similar buttons.

    :see: `wx.ContextHelp`, `wx.ContextHelpButton`

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=ID_CONTEXT_HELP, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=BU_AUTODRAW) -> ContextHelpButton

        Constructor, creating and showing a context help button.
        """
        _controls_.ContextHelpButton_swiginit(self,_controls_.new_ContextHelpButton(*args, **kwargs))
        self._setOORInfo(self)

_controls_.ContextHelpButton_swigregister(ContextHelpButton)

class HelpProvider(object):
    """
    wx.HelpProvider is an abstract class used by a program
    implementing context-sensitive help to show the help text for the
    given window.

    The current help provider must be explicitly set by the
    application using wx.HelpProvider.Set().
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _controls_.delete_HelpProvider
    __del__ = lambda self : None;
    def Set(*args, **kwargs):
        """
        Set(HelpProvider helpProvider) -> HelpProvider

        Sset the current, application-wide help provider. Returns the previous
        one.  Unlike some other classes, the help provider is not created on
        demand. This must be explicitly done by the application.
        """
        return _controls_.HelpProvider_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def Get(*args, **kwargs):
        """
        Get() -> HelpProvider

        Return the current application-wide help provider.
        """
        return _controls_.HelpProvider_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def GetHelp(*args, **kwargs):
        """
        GetHelp(self, Window window) -> String

        Gets the help string for this window. Its interpretation is dependent
        on the help provider except that empty string always means that no
        help is associated with the window.
        """
        return _controls_.HelpProvider_GetHelp(*args, **kwargs)

    def ShowHelp(*args, **kwargs):
        """
        ShowHelp(self, Window window) -> bool

        Shows help for the given window. Uses GetHelp internally if
        applicable. Returns True if it was done, or False if no help was
        available for this window.
        """
        return _controls_.HelpProvider_ShowHelp(*args, **kwargs)

    def ShowHelpAtPoint(*args, **kwargs):
        """
        ShowHelpAtPoint(self, wxWindowBase window, Point pt, int origin) -> bool

        Show help for the given window (uses window.GetHelpAtPoint()
        internally if applicable), return true if it was done or false if no
        help available for this window.
        """
        return _controls_.HelpProvider_ShowHelpAtPoint(*args, **kwargs)

    def AddHelp(*args, **kwargs):
        """
        AddHelp(self, Window window, String text)

        Associates the text with the given window.
        """
        return _controls_.HelpProvider_AddHelp(*args, **kwargs)

    def AddHelpById(*args, **kwargs):
        """
        AddHelpById(self, int id, String text)

        This version associates the given text with all windows with this
        id. May be used to set the same help string for all Cancel buttons in
        the application, for example.
        """
        return _controls_.HelpProvider_AddHelpById(*args, **kwargs)

    def RemoveHelp(*args, **kwargs):
        """
        RemoveHelp(self, Window window)

        Removes the association between the window pointer and the help
        text. This is called by the wx.Window destructor. Without this, the
        table of help strings will fill up and when window pointers are
        reused, the wrong help string will be found.
        """
        return _controls_.HelpProvider_RemoveHelp(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        args[0].this.own(False)
        return _controls_.HelpProvider_Destroy(*args, **kwargs)

_controls_.HelpProvider_swigregister(HelpProvider)

def HelpProvider_Set(*args, **kwargs):
  """
    HelpProvider_Set(HelpProvider helpProvider) -> HelpProvider

    Sset the current, application-wide help provider. Returns the previous
    one.  Unlike some other classes, the help provider is not created on
    demand. This must be explicitly done by the application.
    """
  return _controls_.HelpProvider_Set(*args, **kwargs)

def HelpProvider_Get(*args):
  """
    HelpProvider_Get() -> HelpProvider

    Return the current application-wide help provider.
    """
  return _controls_.HelpProvider_Get(*args)

class SimpleHelpProvider(HelpProvider):
    """
    wx.SimpleHelpProvider is an implementation of `wx.HelpProvider` which
    supports only plain text help strings, and shows the string associated
    with the control (if any) in a tooltip.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> SimpleHelpProvider

        wx.SimpleHelpProvider is an implementation of `wx.HelpProvider` which
        supports only plain text help strings, and shows the string associated
        with the control (if any) in a tooltip.
        """
        _controls_.SimpleHelpProvider_swiginit(self,_controls_.new_SimpleHelpProvider(*args, **kwargs))
_controls_.SimpleHelpProvider_swigregister(SimpleHelpProvider)

#---------------------------------------------------------------------------

class DragImage(_core.Object):
    """Proxy of C++ DragImage class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Bitmap image, Cursor cursor=wxNullCursor) -> DragImage"""
        _controls_.DragImage_swiginit(self,_controls_.new_DragImage(*args, **kwargs))
    __swig_destroy__ = _controls_.delete_DragImage
    __del__ = lambda self : None;
    def SetBackingBitmap(*args, **kwargs):
        """SetBackingBitmap(self, Bitmap bitmap)"""
        return _controls_.DragImage_SetBackingBitmap(*args, **kwargs)

    def BeginDrag(*args, **kwargs):
        """
        BeginDrag(self, Point hotspot, Window window, bool fullScreen=False, 
            Rect rect=None) -> bool
        """
        return _controls_.DragImage_BeginDrag(*args, **kwargs)

    def BeginDragBounded(*args, **kwargs):
        """BeginDragBounded(self, Point hotspot, Window window, Window boundingWindow) -> bool"""
        return _controls_.DragImage_BeginDragBounded(*args, **kwargs)

    def EndDrag(*args, **kwargs):
        """EndDrag(self) -> bool"""
        return _controls_.DragImage_EndDrag(*args, **kwargs)

    def Move(*args, **kwargs):
        """Move(self, Point pt) -> bool"""
        return _controls_.DragImage_Move(*args, **kwargs)

    def Show(*args, **kwargs):
        """Show(self) -> bool"""
        return _controls_.DragImage_Show(*args, **kwargs)

    def Hide(*args, **kwargs):
        """Hide(self) -> bool"""
        return _controls_.DragImage_Hide(*args, **kwargs)

    def GetImageRect(*args, **kwargs):
        """GetImageRect(self, Point pos) -> Rect"""
        return _controls_.DragImage_GetImageRect(*args, **kwargs)

    def DoDrawImage(*args, **kwargs):
        """DoDrawImage(self, DC dc, Point pos) -> bool"""
        return _controls_.DragImage_DoDrawImage(*args, **kwargs)

    def UpdateBackingFromWindow(*args, **kwargs):
        """UpdateBackingFromWindow(self, DC windowDC, MemoryDC destDC, Rect sourceRect, Rect destRect) -> bool"""
        return _controls_.DragImage_UpdateBackingFromWindow(*args, **kwargs)

    def RedrawImage(*args, **kwargs):
        """RedrawImage(self, Point oldPos, Point newPos, bool eraseOld, bool drawNew) -> bool"""
        return _controls_.DragImage_RedrawImage(*args, **kwargs)

    ImageRect = property(GetImageRect,doc="See `GetImageRect`") 
_controls_.DragImage_swigregister(DragImage)

def DragIcon(*args, **kwargs):
    """DragIcon(Icon image, Cursor cursor=wxNullCursor) -> DragImage"""
    val = _controls_.new_DragIcon(*args, **kwargs)
    return val

def DragString(*args, **kwargs):
    """DragString(String str, Cursor cursor=wxNullCursor) -> DragImage"""
    val = _controls_.new_DragString(*args, **kwargs)
    return val

def DragTreeItem(*args, **kwargs):
    """DragTreeItem(TreeCtrl treeCtrl, TreeItemId id) -> DragImage"""
    val = _controls_.new_DragTreeItem(*args, **kwargs)
    return val

def DragListItem(*args, **kwargs):
    """DragListItem(ListCtrl listCtrl, long id) -> DragImage"""
    val = _controls_.new_DragListItem(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

DP_DEFAULT = _controls_.DP_DEFAULT
DP_SPIN = _controls_.DP_SPIN
DP_DROPDOWN = _controls_.DP_DROPDOWN
DP_SHOWCENTURY = _controls_.DP_SHOWCENTURY
DP_ALLOWNONE = _controls_.DP_ALLOWNONE
class DatePickerCtrl(_core.Control):
    """
    This control allows the user to select a date. Unlike
    `wx.calendar.CalendarCtrl`, which is a relatively big control,
    `wx.DatePickerCtrl` is implemented as a small window showing the
    currently selected date. The control can be edited using the keyboard,
    and can also display a popup window for more user-friendly date
    selection, depending on the styles used and the platform.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, DateTime dt=wxDefaultDateTime, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDP_DEFAULT|wxDP_SHOWCENTURY, 
            Validator validator=DefaultValidator, 
            String name=DatePickerCtrlNameStr) -> DatePickerCtrl

        Create a new DatePickerCtrl.
        """
        _controls_.DatePickerCtrl_swiginit(self,_controls_.new_DatePickerCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, DateTime dt=wxDefaultDateTime, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDP_DEFAULT|wxDP_SHOWCENTURY, 
            Validator validator=DefaultValidator, 
            String name=DatePickerCtrlNameStr) -> bool

        Create the GUI parts of the DatePickerCtrl, for use in 2-phase
        creation.
        """
        return _controls_.DatePickerCtrl_Create(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """
        SetValue(self, DateTime dt)

        Changes the current value of the control. The date should be valid and
        included in the currently selected range, if any.

        Calling this method does not result in a date change event.
        """
        return _controls_.DatePickerCtrl_SetValue(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """
        GetValue(self) -> DateTime

        Returns the currently selected date. If there is no selection or the
        selection is outside of the current range, an invalid `wx.DateTime`
        object is returned.
        """
        return _controls_.DatePickerCtrl_GetValue(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """
        SetRange(self, DateTime dt1, DateTime dt2)

        Sets the valid range for the date selection. If dt1 is valid, it
        becomes the earliest date (inclusive) accepted by the control. If dt2
        is valid, it becomes the latest possible date.

        If the current value of the control is outside of the newly set range
        bounds, the behaviour is undefined.
        """
        return _controls_.DatePickerCtrl_SetRange(*args, **kwargs)

    def GetLowerLimit(*args, **kwargs):
        """
        GetLowerLimit(self) -> DateTime

        Get the lower limit of the valid range for the date selection, if any.
        If there is no range or there is no lower limit, then the
        `wx.DateTime` value returned will be invalid.
        """
        return _controls_.DatePickerCtrl_GetLowerLimit(*args, **kwargs)

    def GetUpperLimit(*args, **kwargs):
        """
        GetUpperLimit(self) -> DateTime

        Get the upper limit of the valid range for the date selection, if any.
        If there is no range or there is no upper limit, then the
        `wx.DateTime` value returned will be invalid.
        """
        return _controls_.DatePickerCtrl_GetUpperLimit(*args, **kwargs)

    LowerLimit = property(GetLowerLimit,doc="See `GetLowerLimit`") 
    UpperLimit = property(GetUpperLimit,doc="See `GetUpperLimit`") 
    Value = property(GetValue,SetValue,doc="See `GetValue` and `SetValue`") 
_controls_.DatePickerCtrl_swigregister(DatePickerCtrl)
DatePickerCtrlNameStr = cvar.DatePickerCtrlNameStr

def PreDatePickerCtrl(*args, **kwargs):
    """
    PreDatePickerCtrl() -> DatePickerCtrl

    Precreate a DatePickerCtrl for use in 2-phase creation.
    """
    val = _controls_.new_PreDatePickerCtrl(*args, **kwargs)
    return val

HL_CONTEXTMENU = _controls_.HL_CONTEXTMENU
HL_ALIGN_LEFT = _controls_.HL_ALIGN_LEFT
HL_ALIGN_RIGHT = _controls_.HL_ALIGN_RIGHT
HL_ALIGN_CENTRE = _controls_.HL_ALIGN_CENTRE
HL_DEFAULT_STYLE = _controls_.HL_DEFAULT_STYLE
#---------------------------------------------------------------------------

class HyperlinkCtrl(_core.Control):
    """
    A static text control that emulates a hyperlink. The link is displayed
    in an appropriate text style, derived from the control's normal font.
    When the mouse rolls over the link, the cursor changes to a hand and
    the link's color changes to the active color.

    Clicking on the link does not launch a web browser; instead, a
    wx.HyperlinkEvent is fired. Use the wx.EVT_HYPERLINK to catch link
    events.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id, String label, String url, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=HL_DEFAULT_STYLE, String name=HyperlinkCtrlNameStr) -> HyperlinkCtrl

        A static text control that emulates a hyperlink. The link is displayed
        in an appropriate text style, derived from the control's normal font.
        When the mouse rolls over the link, the cursor changes to a hand and
        the link's color changes to the active color.

        Clicking on the link does not launch a web browser; instead, a
        wx.HyperlinkEvent is fired. Use the wx.EVT_HYPERLINK to catch link
        events.

        """
        _controls_.HyperlinkCtrl_swiginit(self,_controls_.new_HyperlinkCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, String label, String url, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=HL_DEFAULT_STYLE, String name=HyperlinkCtrlNameStr) -> bool
        """
        return _controls_.HyperlinkCtrl_Create(*args, **kwargs)

    def GetHoverColour(*args, **kwargs):
        """GetHoverColour(self) -> Colour"""
        return _controls_.HyperlinkCtrl_GetHoverColour(*args, **kwargs)

    def SetHoverColour(*args, **kwargs):
        """SetHoverColour(self, Colour colour)"""
        return _controls_.HyperlinkCtrl_SetHoverColour(*args, **kwargs)

    def GetNormalColour(*args, **kwargs):
        """GetNormalColour(self) -> Colour"""
        return _controls_.HyperlinkCtrl_GetNormalColour(*args, **kwargs)

    def SetNormalColour(*args, **kwargs):
        """SetNormalColour(self, Colour colour)"""
        return _controls_.HyperlinkCtrl_SetNormalColour(*args, **kwargs)

    def GetVisitedColour(*args, **kwargs):
        """GetVisitedColour(self) -> Colour"""
        return _controls_.HyperlinkCtrl_GetVisitedColour(*args, **kwargs)

    def SetVisitedColour(*args, **kwargs):
        """SetVisitedColour(self, Colour colour)"""
        return _controls_.HyperlinkCtrl_SetVisitedColour(*args, **kwargs)

    def GetURL(*args, **kwargs):
        """GetURL(self) -> String"""
        return _controls_.HyperlinkCtrl_GetURL(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """SetURL(self, String url)"""
        return _controls_.HyperlinkCtrl_SetURL(*args, **kwargs)

    def SetVisited(*args, **kwargs):
        """SetVisited(self, bool visited=True)"""
        return _controls_.HyperlinkCtrl_SetVisited(*args, **kwargs)

    def GetVisited(*args, **kwargs):
        """GetVisited(self) -> bool"""
        return _controls_.HyperlinkCtrl_GetVisited(*args, **kwargs)

    HoverColour = property(GetHoverColour,SetHoverColour,doc="See `GetHoverColour` and `SetHoverColour`") 
    NormalColour = property(GetNormalColour,SetNormalColour,doc="See `GetNormalColour` and `SetNormalColour`") 
    URL = property(GetURL,SetURL,doc="See `GetURL` and `SetURL`") 
    Visited = property(GetVisited,SetVisited,doc="See `GetVisited` and `SetVisited`") 
    VisitedColour = property(GetVisitedColour,SetVisitedColour,doc="See `GetVisitedColour` and `SetVisitedColour`") 
_controls_.HyperlinkCtrl_swigregister(HyperlinkCtrl)
HyperlinkCtrlNameStr = cvar.HyperlinkCtrlNameStr

def PreHyperlinkCtrl(*args, **kwargs):
    """
    PreHyperlinkCtrl() -> HyperlinkCtrl

    A static text control that emulates a hyperlink. The link is displayed
    in an appropriate text style, derived from the control's normal font.
    When the mouse rolls over the link, the cursor changes to a hand and
    the link's color changes to the active color.

    Clicking on the link does not launch a web browser; instead, a
    wx.HyperlinkEvent is fired. Use the wx.EVT_HYPERLINK to catch link
    events.

    """
    val = _controls_.new_PreHyperlinkCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_HYPERLINK = _controls_.wxEVT_COMMAND_HYPERLINK
class HyperlinkEvent(_core.CommandEvent):
    """Proxy of C++ HyperlinkEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Object generator, int id, String url) -> HyperlinkEvent"""
        _controls_.HyperlinkEvent_swiginit(self,_controls_.new_HyperlinkEvent(*args, **kwargs))
    def GetURL(*args, **kwargs):
        """GetURL(self) -> String"""
        return _controls_.HyperlinkEvent_GetURL(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """SetURL(self, String url)"""
        return _controls_.HyperlinkEvent_SetURL(*args, **kwargs)

    URL = property(GetURL,SetURL,doc="See `GetURL` and `SetURL`") 
_controls_.HyperlinkEvent_swigregister(HyperlinkEvent)

EVT_HYPERLINK = wx.PyEventBinder( wxEVT_COMMAND_HYPERLINK, 1 )

#---------------------------------------------------------------------------

PB_USE_TEXTCTRL = _controls_.PB_USE_TEXTCTRL
class PickerBase(_core.Control):
    """
    Base abstract class for all pickers which support an auxiliary text
    control. This class handles all positioning and sizing of the text
    control like a an horizontal `wx.BoxSizer` would do, with the text
    control on the left of the picker button and the proportion of the
    picker fixed to value 1.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def CreateBase(*args, **kwargs):
        """
        CreateBase(self, Window parent, int id, String text=wxEmptyString, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=wxButtonNameStr) -> bool
        """
        return _controls_.PickerBase_CreateBase(*args, **kwargs)

    def SetInternalMargin(*args, **kwargs):
        """
        SetInternalMargin(self, int newmargin)

        Sets the margin (in pixels) between the picker and the text control.
        """
        return _controls_.PickerBase_SetInternalMargin(*args, **kwargs)

    def GetInternalMargin(*args, **kwargs):
        """
        GetInternalMargin(self) -> int

        Returns the margin (in pixels) between the picker and the text
        control.
        """
        return _controls_.PickerBase_GetInternalMargin(*args, **kwargs)

    def SetTextCtrlProportion(*args, **kwargs):
        """
        SetTextCtrlProportion(self, int prop)

        Sets the proportion between the text control and the picker button.
        This is used to set relative sizes of the text contorl and the picker.
        The value passed to this function must be >= 1.
        """
        return _controls_.PickerBase_SetTextCtrlProportion(*args, **kwargs)

    def GetTextCtrlProportion(*args, **kwargs):
        """
        GetTextCtrlProportion(self) -> int

        Returns the proportion between the text control and the picker.
        """
        return _controls_.PickerBase_GetTextCtrlProportion(*args, **kwargs)

    def SetPickerCtrlProportion(*args, **kwargs):
        """
        SetPickerCtrlProportion(self, int prop)

        Sets the proportion value of the picker.
        """
        return _controls_.PickerBase_SetPickerCtrlProportion(*args, **kwargs)

    def GetPickerCtrlProportion(*args, **kwargs):
        """
        GetPickerCtrlProportion(self) -> int

        Gets the proportion value of the picker.
        """
        return _controls_.PickerBase_GetPickerCtrlProportion(*args, **kwargs)

    def IsTextCtrlGrowable(*args, **kwargs):
        """IsTextCtrlGrowable(self) -> bool"""
        return _controls_.PickerBase_IsTextCtrlGrowable(*args, **kwargs)

    def SetTextCtrlGrowable(*args, **kwargs):
        """SetTextCtrlGrowable(self, bool grow=True)"""
        return _controls_.PickerBase_SetTextCtrlGrowable(*args, **kwargs)

    def IsPickerCtrlGrowable(*args, **kwargs):
        """IsPickerCtrlGrowable(self) -> bool"""
        return _controls_.PickerBase_IsPickerCtrlGrowable(*args, **kwargs)

    def SetPickerCtrlGrowable(*args, **kwargs):
        """SetPickerCtrlGrowable(self, bool grow=True)"""
        return _controls_.PickerBase_SetPickerCtrlGrowable(*args, **kwargs)

    def HasTextCtrl(*args, **kwargs):
        """
        HasTextCtrl(self) -> bool

        Returns true if this class has a valid text control (i.e. if the
        wx.PB_USE_TEXTCTRL style was given when creating this control).
        """
        return _controls_.PickerBase_HasTextCtrl(*args, **kwargs)

    def GetTextCtrl(*args, **kwargs):
        """
        GetTextCtrl(self) -> TextCtrl

        Returns a pointer to the text control handled by this class or None if
        the wx.PB_USE_TEXTCTRL style was not specified when this control was
        created.

        Very important: the contents of the text control could be containing
        an invalid representation of the entity which can be chosen through
        the picker (e.g. the user entered an invalid colour syntax because of
        a typo). Thus you should never parse the content of the textctrl to
        get the user's input; rather use the derived-class getter
        (e.g. `wx.ColourPickerCtrl.GetColour`, `wx.FilePickerCtrl.GetPath`,
        etc).
        """
        return _controls_.PickerBase_GetTextCtrl(*args, **kwargs)

    def GetPickerCtrl(*args, **kwargs):
        """GetPickerCtrl(self) -> Control"""
        return _controls_.PickerBase_GetPickerCtrl(*args, **kwargs)

    InternalMargin = property(GetInternalMargin,SetInternalMargin,doc="See `GetInternalMargin` and `SetInternalMargin`") 
    PickerCtrl = property(GetPickerCtrl,doc="See `GetPickerCtrl`") 
    PickerCtrlProportion = property(GetPickerCtrlProportion,SetPickerCtrlProportion,doc="See `GetPickerCtrlProportion` and `SetPickerCtrlProportion`") 
    TextCtrl = property(GetTextCtrl,doc="See `GetTextCtrl`") 
    TextCtrlProportion = property(GetTextCtrlProportion,SetTextCtrlProportion,doc="See `GetTextCtrlProportion` and `SetTextCtrlProportion`") 
    TextCtrlGrowable = property(IsTextCtrlGrowable,SetTextCtrlGrowable,doc="See `IsTextCtrlGrowable` and `SetTextCtrlGrowable`") 
    PickerCtrlGrowable = property(IsPickerCtrlGrowable,SetPickerCtrlGrowable,doc="See `IsPickerCtrlGrowable` and `SetPickerCtrlGrowable`") 
_controls_.PickerBase_swigregister(PickerBase)

#---------------------------------------------------------------------------

CLRP_SHOW_LABEL = _controls_.CLRP_SHOW_LABEL
CLRP_USE_TEXTCTRL = _controls_.CLRP_USE_TEXTCTRL
CLRP_DEFAULT_STYLE = _controls_.CLRP_DEFAULT_STYLE
class ColourPickerCtrl(PickerBase):
    """
    This control allows the user to select a colour. The generic
    implementation is a button which brings up a `wx.ColourDialog` when
    clicked. Native implementations may differ but this is usually a
    (small) widget which give access to the colour-chooser dialog.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Colour col=*wxBLACK, Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=CLRP_DEFAULT_STYLE, Validator validator=DefaultValidator, 
            String name=ColourPickerCtrlNameStr) -> ColourPickerCtrl

        This control allows the user to select a colour. The generic
        implementation is a button which brings up a `wx.ColourDialog` when
        clicked. Native implementations may differ but this is usually a
        (small) widget which give access to the colour-chooser dialog.
        """
        _controls_.ColourPickerCtrl_swiginit(self,_controls_.new_ColourPickerCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Colour col=*wxBLACK, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=CLRP_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=ColourPickerCtrlNameStr) -> bool
        """
        return _controls_.ColourPickerCtrl_Create(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """
        GetColour(self) -> Colour

        Returns the currently selected colour.
        """
        return _controls_.ColourPickerCtrl_GetColour(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """
        SetColour(self, Colour col)

        Set the displayed colour.
        """
        return _controls_.ColourPickerCtrl_SetColour(*args, **kwargs)

    Colour = property(GetColour,SetColour,doc="See `GetColour` and `SetColour`") 
_controls_.ColourPickerCtrl_swigregister(ColourPickerCtrl)
ColourPickerCtrlNameStr = cvar.ColourPickerCtrlNameStr

def PreColourPickerCtrl(*args, **kwargs):
    """
    PreColourPickerCtrl() -> ColourPickerCtrl

    This control allows the user to select a colour. The generic
    implementation is a button which brings up a `wx.ColourDialog` when
    clicked. Native implementations may differ but this is usually a
    (small) widget which give access to the colour-chooser dialog.
    """
    val = _controls_.new_PreColourPickerCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_COLOURPICKER_CHANGED = _controls_.wxEVT_COMMAND_COLOURPICKER_CHANGED
EVT_COLOURPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_COLOURPICKER_CHANGED, 1 )

class ColourPickerEvent(_core.CommandEvent):
    """Proxy of C++ ColourPickerEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Object generator, int id, Colour col) -> ColourPickerEvent"""
        _controls_.ColourPickerEvent_swiginit(self,_controls_.new_ColourPickerEvent(*args, **kwargs))
    def GetColour(*args, **kwargs):
        """GetColour(self) -> Colour"""
        return _controls_.ColourPickerEvent_GetColour(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(self, Colour c)"""
        return _controls_.ColourPickerEvent_SetColour(*args, **kwargs)

    Colour = property(GetColour,SetColour,doc="See `GetColour` and `SetColour`") 
_controls_.ColourPickerEvent_swigregister(ColourPickerEvent)

#---------------------------------------------------------------------------

FLP_OPEN = _controls_.FLP_OPEN
FLP_SAVE = _controls_.FLP_SAVE
FLP_OVERWRITE_PROMPT = _controls_.FLP_OVERWRITE_PROMPT
FLP_FILE_MUST_EXIST = _controls_.FLP_FILE_MUST_EXIST
FLP_CHANGE_DIR = _controls_.FLP_CHANGE_DIR
DIRP_DIR_MUST_EXIST = _controls_.DIRP_DIR_MUST_EXIST
DIRP_CHANGE_DIR = _controls_.DIRP_CHANGE_DIR
FLP_USE_TEXTCTRL = _controls_.FLP_USE_TEXTCTRL
FLP_DEFAULT_STYLE = _controls_.FLP_DEFAULT_STYLE
DIRP_USE_TEXTCTRL = _controls_.DIRP_USE_TEXTCTRL
DIRP_DEFAULT_STYLE = _controls_.DIRP_DEFAULT_STYLE
class FilePickerCtrl(PickerBase):
    """Proxy of C++ FilePickerCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String path=EmptyString, 
            String message=FileSelectorPromptStr, String wildcard=FileSelectorDefaultWildcardStr, 
            Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=FLP_DEFAULT_STYLE, Validator validator=DefaultValidator, 
            String name=FilePickerCtrlNameStr) -> FilePickerCtrl
        """
        _controls_.FilePickerCtrl_swiginit(self,_controls_.new_FilePickerCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String path=EmptyString, 
            String message=FileSelectorPromptStr, String wildcard=FileSelectorDefaultWildcardStr, 
            Point pos=DefaultPosition, 
            Size size=DefaultSize, 
            long style=FLP_DEFAULT_STYLE, Validator validator=DefaultValidator, 
            String name=FilePickerCtrlNameStr) -> bool
        """
        return _controls_.FilePickerCtrl_Create(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath(self) -> String"""
        return _controls_.FilePickerCtrl_GetPath(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(self, String str)"""
        return _controls_.FilePickerCtrl_SetPath(*args, **kwargs)

    def CheckPath(*args, **kwargs):
        """CheckPath(self, String path) -> bool"""
        return _controls_.FilePickerCtrl_CheckPath(*args, **kwargs)

    def GetTextCtrlValue(*args, **kwargs):
        """GetTextCtrlValue(self) -> String"""
        return _controls_.FilePickerCtrl_GetTextCtrlValue(*args, **kwargs)

    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
    TextCtrlValue = property(GetTextCtrlValue,doc="See `GetTextCtrlValue`") 
_controls_.FilePickerCtrl_swigregister(FilePickerCtrl)
FilePickerCtrlNameStr = cvar.FilePickerCtrlNameStr
FileSelectorPromptStr = cvar.FileSelectorPromptStr
DirPickerCtrlNameStr = cvar.DirPickerCtrlNameStr
DirSelectorPromptStr = cvar.DirSelectorPromptStr
FileSelectorDefaultWildcardStr = cvar.FileSelectorDefaultWildcardStr

def PreFilePickerCtrl(*args, **kwargs):
    """PreFilePickerCtrl() -> FilePickerCtrl"""
    val = _controls_.new_PreFilePickerCtrl(*args, **kwargs)
    return val

class DirPickerCtrl(PickerBase):
    """Proxy of C++ DirPickerCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String path=EmptyString, 
            String message=DirSelectorPromptStr, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DIRP_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=DirPickerCtrlNameStr) -> DirPickerCtrl
        """
        _controls_.DirPickerCtrl_swiginit(self,_controls_.new_DirPickerCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String path=EmptyString, 
            String message=DirSelectorPromptStr, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=DIRP_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=DirPickerCtrlNameStr) -> bool
        """
        return _controls_.DirPickerCtrl_Create(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath(self) -> String"""
        return _controls_.DirPickerCtrl_GetPath(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(self, String str)"""
        return _controls_.DirPickerCtrl_SetPath(*args, **kwargs)

    def CheckPath(*args, **kwargs):
        """CheckPath(self, String path) -> bool"""
        return _controls_.DirPickerCtrl_CheckPath(*args, **kwargs)

    def GetTextCtrlValue(*args, **kwargs):
        """GetTextCtrlValue(self) -> String"""
        return _controls_.DirPickerCtrl_GetTextCtrlValue(*args, **kwargs)

    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
    TextCtrlValue = property(GetTextCtrlValue,doc="See `GetTextCtrlValue`") 
_controls_.DirPickerCtrl_swigregister(DirPickerCtrl)

def PreDirPickerCtrl(*args, **kwargs):
    """PreDirPickerCtrl() -> DirPickerCtrl"""
    val = _controls_.new_PreDirPickerCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_FILEPICKER_CHANGED = _controls_.wxEVT_COMMAND_FILEPICKER_CHANGED
wxEVT_COMMAND_DIRPICKER_CHANGED = _controls_.wxEVT_COMMAND_DIRPICKER_CHANGED
EVT_FILEPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_FILEPICKER_CHANGED, 1 )
EVT_DIRPICKER_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_DIRPICKER_CHANGED,  1 )

class FileDirPickerEvent(_core.CommandEvent):
    """Proxy of C++ FileDirPickerEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, EventType type, Object generator, int id, String path) -> FileDirPickerEvent"""
        _controls_.FileDirPickerEvent_swiginit(self,_controls_.new_FileDirPickerEvent(*args, **kwargs))
    def GetPath(*args, **kwargs):
        """GetPath(self) -> String"""
        return _controls_.FileDirPickerEvent_GetPath(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(self, String p)"""
        return _controls_.FileDirPickerEvent_SetPath(*args, **kwargs)

    Path = property(GetPath,SetPath,doc="See `GetPath` and `SetPath`") 
_controls_.FileDirPickerEvent_swigregister(FileDirPickerEvent)

#---------------------------------------------------------------------------

FNTP_FONTDESC_AS_LABEL = _controls_.FNTP_FONTDESC_AS_LABEL
FNTP_USEFONT_FOR_LABEL = _controls_.FNTP_USEFONT_FOR_LABEL
FNTP_USE_TEXTCTRL = _controls_.FNTP_USE_TEXTCTRL
FNTP_DEFAULT_STYLE = _controls_.FNTP_DEFAULT_STYLE
class FontPickerCtrl(PickerBase):
    """Proxy of C++ FontPickerCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, Font initial=wxNullFont, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=FNTP_DEFAULT_STYLE, Validator validator=DefaultValidator, 
            String name=FontPickerCtrlNameStr) -> FontPickerCtrl
        """
        _controls_.FontPickerCtrl_swiginit(self,_controls_.new_FontPickerCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Font initial=wxNullFont, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=FNTP_DEFAULT_STYLE, Validator validator=DefaultValidator, 
            String name=FontPickerCtrlNameStr) -> bool
        """
        return _controls_.FontPickerCtrl_Create(*args, **kwargs)

    def GetSelectedFont(*args, **kwargs):
        """GetSelectedFont(self) -> Font"""
        return _controls_.FontPickerCtrl_GetSelectedFont(*args, **kwargs)

    def SetSelectedFont(*args, **kwargs):
        """SetSelectedFont(self, Font f)"""
        return _controls_.FontPickerCtrl_SetSelectedFont(*args, **kwargs)

    def SetMaxPointSize(*args, **kwargs):
        """SetMaxPointSize(self, unsigned int max)"""
        return _controls_.FontPickerCtrl_SetMaxPointSize(*args, **kwargs)

    def GetMaxPointSize(*args, **kwargs):
        """GetMaxPointSize(self) -> unsigned int"""
        return _controls_.FontPickerCtrl_GetMaxPointSize(*args, **kwargs)

    MaxPointSize = property(GetMaxPointSize,SetMaxPointSize,doc="See `GetMaxPointSize` and `SetMaxPointSize`") 
    SelectedFont = property(GetSelectedFont,SetSelectedFont,doc="See `GetSelectedFont` and `SetSelectedFont`") 
_controls_.FontPickerCtrl_swigregister(FontPickerCtrl)
FontPickerCtrlNameStr = cvar.FontPickerCtrlNameStr

def PreFontPickerCtrl(*args, **kwargs):
    """PreFontPickerCtrl() -> FontPickerCtrl"""
    val = _controls_.new_PreFontPickerCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_FONTPICKER_CHANGED = _controls_.wxEVT_COMMAND_FONTPICKER_CHANGED
EVT_FONTPICKER_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_FONTPICKER_CHANGED, 1 )

class FontPickerEvent(_core.CommandEvent):
    """Proxy of C++ FontPickerEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Object generator, int id, Font f) -> FontPickerEvent"""
        _controls_.FontPickerEvent_swiginit(self,_controls_.new_FontPickerEvent(*args, **kwargs))
    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _controls_.FontPickerEvent_GetFont(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font c)"""
        return _controls_.FontPickerEvent_SetFont(*args, **kwargs)

    Font = property(GetFont,SetFont,doc="See `GetFont` and `SetFont`") 
_controls_.FontPickerEvent_swigregister(FontPickerEvent)

#---------------------------------------------------------------------------

CP_DEFAULT_STYLE = _controls_.CP_DEFAULT_STYLE
CP_NO_TLW_RESIZE = _controls_.CP_NO_TLW_RESIZE
class CollapsiblePane(_core.Control):
    """
    A collapsable pane is a container with an embedded button-like
    control which can be used by the user to collapse or expand the pane's
    contents.

    Once constructed you should use the `GetPane` function to access the
    pane and add your controls inside it (i.e. use the window returned
    from `GetPane` as the parent for the controls which must go in the
    pane, NOT the wx.CollapsiblePane itself!).

    Note that because of its nature of control which can dynamically (and
    drastically) change its size at run-time under user-input, when
    putting a wx.CollapsiblePane inside a `wx.Sizer` you should be careful
    to add it with a proportion value of zero; this is because otherwise
    all other windows with non-zero proportion values would automatically
    get resized each time the user expands or collapses the pane window,
    usually resulting a weird, flickering effect.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int winid=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=CP_DEFAULT_STYLE, Validator val=DefaultValidator, 
            String name=CollapsiblePaneNameStr) -> CollapsiblePane

        Create and show a wx.CollapsiblePane
        """
        _controls_.CollapsiblePane_swiginit(self,_controls_.new_CollapsiblePane(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int winid=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=CP_DEFAULT_STYLE, Validator val=DefaultValidator, 
            String name=CollapsiblePaneNameStr) -> bool
        """
        return _controls_.CollapsiblePane_Create(*args, **kwargs)

    def Collapse(*args, **kwargs):
        """
        Collapse(self, bool collapse=True)

        Collapses or expands the pane window.
        """
        return _controls_.CollapsiblePane_Collapse(*args, **kwargs)

    def Expand(*args, **kwargs):
        """
        Expand(self)

        Same as Collapse(False).
        """
        return _controls_.CollapsiblePane_Expand(*args, **kwargs)

    def IsCollapsed(*args, **kwargs):
        """
        IsCollapsed(self) -> bool

        Returns ``True`` if the pane window is currently hidden.
        """
        return _controls_.CollapsiblePane_IsCollapsed(*args, **kwargs)

    def IsExpanded(*args, **kwargs):
        """
        IsExpanded(self) -> bool

        Returns ``True`` if the pane window is currently shown.
        """
        return _controls_.CollapsiblePane_IsExpanded(*args, **kwargs)

    def GetPane(*args, **kwargs):
        """
        GetPane(self) -> Window

        Returns a reference to the pane window.  Use the returned `wx.Window`
        as the parent of widgets to make them part of the collapsible area.
        """
        return _controls_.CollapsiblePane_GetPane(*args, **kwargs)

    Expanded = property(IsExpanded) 
    Collapsed = property(IsCollapsed) 
_controls_.CollapsiblePane_swigregister(CollapsiblePane)
CollapsiblePaneNameStr = cvar.CollapsiblePaneNameStr

def PreCollapsiblePane(*args, **kwargs):
    """
    PreCollapsiblePane() -> CollapsiblePane

    Precreate a wx.CollapsiblePane for 2-phase creation.
    """
    val = _controls_.new_PreCollapsiblePane(*args, **kwargs)
    return val

wxEVT_COMMAND_COLLPANE_CHANGED = _controls_.wxEVT_COMMAND_COLLPANE_CHANGED
EVT_COLLAPSIBLEPANE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_COLLPANE_CHANGED, 1 )

class CollapsiblePaneEvent(_core.CommandEvent):
    """Proxy of C++ CollapsiblePaneEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Object generator, int id, bool collapsed) -> CollapsiblePaneEvent"""
        _controls_.CollapsiblePaneEvent_swiginit(self,_controls_.new_CollapsiblePaneEvent(*args, **kwargs))
    def GetCollapsed(*args, **kwargs):
        """GetCollapsed(self) -> bool"""
        return _controls_.CollapsiblePaneEvent_GetCollapsed(*args, **kwargs)

    def SetCollapsed(*args, **kwargs):
        """SetCollapsed(self, bool c)"""
        return _controls_.CollapsiblePaneEvent_SetCollapsed(*args, **kwargs)

    Collapsed = property(GetCollapsed,SetCollapsed) 
_controls_.CollapsiblePaneEvent_swigregister(CollapsiblePaneEvent)

#---------------------------------------------------------------------------

class SearchCtrl(TextCtrl):
    """
    A search control is a composite of a `wx.TextCtrl` with optional
    bitmap buttons and a drop-down menu.  Controls like this can typically
    be found on a toolbar of applications that support some form of search
    functionality.  On the Mac this control is implemented using the
    native HISearchField control, on the other platforms a generic control
    is used, although that may change in the future as more platforms
    introduce native search widgets.

    If you wish to use a drop-down menu with your wx.SearchCtrl then you
    will need to manage its content and handle the menu events yourself,
    but this is an easy thing to do.  Simply build the menu, pass it to
    `SetMenu`, and also bind a handler for a range of EVT_MENU events.
    This gives you the flexibility to use the drop-down menu however you
    wish, such as for a history of searches, or as a way to select
    different kinds of searches.  The ToolBar.py sample in the demo shows
    one way to do this.

    Since the control derives from `wx.TextCtrl` it is convenient to use
    the styles and events designed for `wx.TextCtrl`.  For example you can
    use the ``wx.TE_PROCESS_ENTER`` style and catch the
    ``wx.EVT_TEXT_ENTER`` event to know when the user has pressed the
    Enter key in the control and wishes to start a search.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int id=-1, String value=wxEmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=SearchCtrlNameStr) -> SearchCtrl

        A search control is a composite of a `wx.TextCtrl` with optional
        bitmap buttons and a drop-down menu.  Controls like this can typically
        be found on a toolbar of applications that support some form of search
        functionality.  On the Mac this control is implemented using the
        native HISearchField control, on the other platforms a generic control
        is used, although that may change in the future as more platforms
        introduce native search widgets.

        If you wish to use a drop-down menu with your wx.SearchCtrl then you
        will need to manage its content and handle the menu events yourself,
        but this is an easy thing to do.  Simply build the menu, pass it to
        `SetMenu`, and also bind a handler for a range of EVT_MENU events.
        This gives you the flexibility to use the drop-down menu however you
        wish, such as for a history of searches, or as a way to select
        different kinds of searches.  The ToolBar.py sample in the demo shows
        one way to do this.

        Since the control derives from `wx.TextCtrl` it is convenient to use
        the styles and events designed for `wx.TextCtrl`.  For example you can
        use the ``wx.TE_PROCESS_ENTER`` style and catch the
        ``wx.EVT_TEXT_ENTER`` event to know when the user has pressed the
        Enter key in the control and wishes to start a search.

        """
        _controls_.SearchCtrl_swiginit(self,_controls_.new_SearchCtrl(*args, **kwargs))
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String value=wxEmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=SearchCtrlNameStr) -> bool
        """
        return _controls_.SearchCtrl_Create(*args, **kwargs)

    def SetMenu(*args, **kwargs):
        """
        SetMenu(self, Menu menu)

        Sets the search control's menu object. If there is already a menu
        associated with the search control it is deleted.
        """
        return _controls_.SearchCtrl_SetMenu(*args, **kwargs)

    def GetMenu(*args, **kwargs):
        """
        GetMenu(self) -> Menu

        Returns a pointer to the search control's menu object or None if there
        is no menu attached.
        """
        return _controls_.SearchCtrl_GetMenu(*args, **kwargs)

    def ShowSearchButton(*args, **kwargs):
        """
        ShowSearchButton(self, bool show)

        Sets the search button visibility value on the search control. If
        there is a menu attached, the search button will be visible regardless
        of the search button visibility value.  This has no effect in Mac OS X
        v10.3
        """
        return _controls_.SearchCtrl_ShowSearchButton(*args, **kwargs)

    def IsSearchButtonVisible(*args, **kwargs):
        """
        IsSearchButtonVisible(self) -> bool

        Returns the search button visibility value. If there is a menu
        attached, the search button will be visible regardless of the search
        button visibility value.  This always returns false in Mac OS X v10.3
        """
        return _controls_.SearchCtrl_IsSearchButtonVisible(*args, **kwargs)

    def ShowCancelButton(*args, **kwargs):
        """
        ShowCancelButton(self, bool show)

        Shows or hides the cancel button.
        """
        return _controls_.SearchCtrl_ShowCancelButton(*args, **kwargs)

    def IsCancelButtonVisible(*args, **kwargs):
        """
        IsCancelButtonVisible(self) -> bool

        Indicates whether the cancel button is visible. 
        """
        return _controls_.SearchCtrl_IsCancelButtonVisible(*args, **kwargs)

    def SetSearchBitmap(*args, **kwargs):
        """
        SetSearchBitmap(self, Bitmap ?)

        Sets the bitmap to use for the search button.  This currently does not
        work on the Mac.
        """
        return _controls_.SearchCtrl_SetSearchBitmap(*args, **kwargs)

    def SetSearchMenuBitmap(*args, **kwargs):
        """
        SetSearchMenuBitmap(self, Bitmap ?)

        Sets the bitmap to use for the search button when there is a drop-down
        menu associated with the search control.  This currently does not work
        on the Mac.
        """
        return _controls_.SearchCtrl_SetSearchMenuBitmap(*args, **kwargs)

    def SetCancelBitmap(*args, **kwargs):
        """
        SetCancelBitmap(self, Bitmap ?)

        Sets the bitmap to use for the cancel button.  This currently does not
        work on the Mac.
        """
        return _controls_.SearchCtrl_SetCancelBitmap(*args, **kwargs)

    Menu = property(GetMenu,SetMenu) 
    SearchButtonVisible = property(IsSearchButtonVisible,ShowSearchButton) 
    CancelButtonVisible = property(IsCancelButtonVisible,ShowCancelButton) 
_controls_.SearchCtrl_swigregister(SearchCtrl)
SearchCtrlNameStr = cvar.SearchCtrlNameStr

def PreSearchCtrl(*args, **kwargs):
    """
    PreSearchCtrl() -> SearchCtrl

    Precreate a wx.SearchCtrl for 2-phase creation.
    """
    val = _controls_.new_PreSearchCtrl(*args, **kwargs)
    return val

wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN = _controls_.wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN
wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN = _controls_.wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN
EVT_SEARCHCTRL_CANCEL_BTN = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, 1)
EVT_SEARCHCTRL_SEARCH_BTN = wx.PyEventBinder( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, 1)



