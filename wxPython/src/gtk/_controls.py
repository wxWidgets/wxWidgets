# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _controls_

import _core
wx = _core 
#---------------------------------------------------------------------------

BU_LEFT = _controls_.BU_LEFT
BU_TOP = _controls_.BU_TOP
BU_RIGHT = _controls_.BU_RIGHT
BU_BOTTOM = _controls_.BU_BOTTOM
BU_EXACTFIT = _controls_.BU_EXACTFIT
BU_AUTODRAW = _controls_.BU_AUTODRAW
class Button(_core.Control):
    """
    A button is a control that contains a text string, and is one of the most
    common elements of a GUI.  It may be placed on a dialog box or panel, or
    indeed almost any other window.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
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
        newobj = _controls_.new_Button(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class ButtonPtr(Button):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Button
_controls_.Button_swigregister(ButtonPtr)
cvar = _controls_.cvar
ButtonNameStr = cvar.ButtonNameStr

def PreButton(*args, **kwargs):
    """
    PreButton() -> Button

    Precreate a Button for 2-phase creation.
    """
    val = _controls_.new_PreButton(*args, **kwargs)
    val.thisown = 1
    return val

def Button_GetDefaultSize(*args, **kwargs):
    """
    Button_GetDefaultSize() -> Size

    Returns the default button size for this platform.
    """
    return _controls_.Button_GetDefaultSize(*args, **kwargs)

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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmapButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=BU_AUTODRAW, Validator validator=DefaultValidator, 
            String name=ButtonNameStr) -> BitmapButton

        Create and show a button with a bitmap for the label.
        """
        newobj = _controls_.new_BitmapButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

    def SetMargins(*args, **kwargs):
        """SetMargins(self, int x, int y)"""
        return _controls_.BitmapButton_SetMargins(*args, **kwargs)

    def GetMarginX(*args, **kwargs):
        """GetMarginX(self) -> int"""
        return _controls_.BitmapButton_GetMarginX(*args, **kwargs)

    def GetMarginY(*args, **kwargs):
        """GetMarginY(self) -> int"""
        return _controls_.BitmapButton_GetMarginY(*args, **kwargs)


class BitmapButtonPtr(BitmapButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BitmapButton
_controls_.BitmapButton_swigregister(BitmapButtonPtr)

def PreBitmapButton(*args, **kwargs):
    """
    PreBitmapButton() -> BitmapButton

    Precreate a BitmapButton for 2-phase creation.
    """
    val = _controls_.new_PreBitmapButton(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=CheckBoxNameStr) -> CheckBox

        Creates and shows a CheckBox control
        """
        newobj = _controls_.new_CheckBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class CheckBoxPtr(CheckBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckBox
_controls_.CheckBox_swigregister(CheckBoxPtr)
CheckBoxNameStr = cvar.CheckBoxNameStr

def PreCheckBox(*args, **kwargs):
    """
    PreCheckBox() -> CheckBox

    Precreate a CheckBox for 2-phase creation.
    """
    val = _controls_.new_PreCheckBox(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChoice instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=[], long style=0, Validator validator=DefaultValidator,
            String name=ChoiceNameStr) -> Choice

        Create and show a Choice control
        """
        newobj = _controls_.new_Choice(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=[], long style=0, Validator validator=DefaultValidator,
            String name=ChoiceNameStr) -> bool

        Actually create the GUI Choice control for 2-phase creation
        """
        return _controls_.Choice_Create(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, int n)

        Select the n'th item (zero based) in the list.
        """
        return _controls_.Choice_SetSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """
        SetStringSelection(self, String string) -> bool

        Select the item with the specifed string
        """
        return _controls_.Choice_SetStringSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """
        SetString(self, int n, String string)

        Set the label for the n'th item (zero based) in the list.
        """
        return _controls_.Choice_SetString(*args, **kwargs)

    Select = SetSelection 
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

class ChoicePtr(Choice):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Choice
_controls_.Choice_swigregister(ChoicePtr)
ChoiceNameStr = cvar.ChoiceNameStr

def PreChoice(*args, **kwargs):
    """
    PreChoice() -> Choice

    Precreate a Choice control for 2-phase creation.
    """
    val = _controls_.new_PreChoice(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxComboBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(Window parent, int id, String value=EmptyString,
            Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=[], long style=0, Validator validator=DefaultValidator,
            String name=ComboBoxNameStr) -> ComboBox

        Constructor, creates and shows a ComboBox control.
        """
        newobj = _controls_.new_ComboBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(Window parent, int id, String value=EmptyString,
            Point pos=DefaultPosition, Size size=DefaultSize,
            List choices=[], long style=0, Validator validator=DefaultValidator,
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

    def SetSelection(*args, **kwargs):
        """
        SetSelection(self, int n)

        Sets the item at index 'n' to be the selected item.
        """
        return _controls_.ComboBox_SetSelection(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """
        SetMark(self, long from, long to)

        Selects the text between the two positions in the combobox text field.
        """
        return _controls_.ComboBox_SetMark(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """
        SetStringSelection(self, String string) -> bool

        Select the item with the specifed string
        """
        return _controls_.ComboBox_SetStringSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """
        SetString(self, int n, String string)

        Set the label for the n'th item (zero based) in the list.
        """
        return _controls_.ComboBox_SetString(*args, **kwargs)

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

class ComboBoxPtr(ComboBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ComboBox
_controls_.ComboBox_swigregister(ComboBoxPtr)
ComboBoxNameStr = cvar.ComboBoxNameStr

def PreComboBox(*args, **kwargs):
    """
    PreComboBox() -> ComboBox

    Precreate a ComboBox control for 2-phase creation.
    """
    val = _controls_.new_PreComboBox(*args, **kwargs)
    val.thisown = 1
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
GA_PROGRESSBAR = _controls_.GA_PROGRESSBAR
class Gauge(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGauge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, int range=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=GA_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=GaugeNameStr) -> Gauge
        """
        newobj = _controls_.new_Gauge(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class GaugePtr(Gauge):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Gauge
_controls_.Gauge_swigregister(GaugePtr)
GaugeNameStr = cvar.GaugeNameStr

def PreGauge(*args, **kwargs):
    """PreGauge() -> Gauge"""
    val = _controls_.new_PreGauge(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBoxNameStr) -> StaticBox
        """
        newobj = _controls_.new_StaticBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class StaticBoxPtr(StaticBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBox
_controls_.StaticBox_swigregister(StaticBoxPtr)
StaticBitmapNameStr = cvar.StaticBitmapNameStr
StaticBoxNameStr = cvar.StaticBoxNameStr
StaticTextNameStr = cvar.StaticTextNameStr

def PreStaticBox(*args, **kwargs):
    """PreStaticBox() -> StaticBox"""
    val = _controls_.new_PreStaticBox(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticLine instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LI_HORIZONTAL, 
            String name=StaticTextNameStr) -> StaticLine
        """
        newobj = _controls_.new_StaticLine(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LI_HORIZONTAL, 
            String name=StaticTextNameStr) -> bool
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

class StaticLinePtr(StaticLine):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticLine
_controls_.StaticLine_swigregister(StaticLinePtr)

def PreStaticLine(*args, **kwargs):
    """PreStaticLine() -> StaticLine"""
    val = _controls_.new_PreStaticLine(*args, **kwargs)
    val.thisown = 1
    return val

def StaticLine_GetDefaultSize(*args, **kwargs):
    """StaticLine_GetDefaultSize() -> int"""
    return _controls_.StaticLine_GetDefaultSize(*args, **kwargs)

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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticText instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticTextNameStr) -> StaticText
        """
        newobj = _controls_.new_StaticText(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticTextNameStr) -> bool
        """
        return _controls_.StaticText_Create(*args, **kwargs)

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

class StaticTextPtr(StaticText):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticText
_controls_.StaticText_swigregister(StaticTextPtr)

def PreStaticText(*args, **kwargs):
    """PreStaticText() -> StaticText"""
    val = _controls_.new_PreStaticText(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Bitmap bitmap=wxNullBitmap, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=StaticBitmapNameStr) -> StaticBitmap
        """
        newobj = _controls_.new_StaticBitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class StaticBitmapPtr(StaticBitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBitmap
_controls_.StaticBitmap_swigregister(StaticBitmapPtr)

def PreStaticBitmap(*args, **kwargs):
    """PreStaticBitmap() -> StaticBitmap"""
    val = _controls_.new_PreStaticBitmap(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> ListBox
        """
        newobj = _controls_.new_ListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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
        """InsertItems(self, wxArrayString items, int pos)"""
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

        Sets the item at index 'n' to be the selected item.
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

class ListBoxPtr(ListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListBox
_controls_.ListBox_swigregister(ListBoxPtr)
ListBoxNameStr = cvar.ListBoxNameStr

def PreListBox(*args, **kwargs):
    """PreListBox() -> ListBox"""
    val = _controls_.new_PreListBox(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, wxArrayString choices=wxPyEmptyStringArray, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ListBoxNameStr) -> CheckListBox
        """
        newobj = _controls_.new_CheckListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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
        """IsChecked(self, int index) -> bool"""
        return _controls_.CheckListBox_IsChecked(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(self, int index, int check=True)"""
        return _controls_.CheckListBox_Check(*args, **kwargs)

    def GetItemHeight(*args, **kwargs):
        """GetItemHeight(self) -> int"""
        return _controls_.CheckListBox_GetItemHeight(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """
        HitTest(self, Point pt) -> int

        Test where the given (in client coords) point lies
        """
        return _controls_.CheckListBox_HitTest(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """
        HitTestXY(self, int x, int y) -> int

        Test where the given (in client coords) point lies
        """
        return _controls_.CheckListBox_HitTestXY(*args, **kwargs)


class CheckListBoxPtr(CheckListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckListBox
_controls_.CheckListBox_swigregister(CheckListBoxPtr)

def PreCheckListBox(*args, **kwargs):
    """PreCheckListBox() -> CheckListBox"""
    val = _controls_.new_PreCheckListBox(*args, **kwargs)
    val.thisown = 1
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
TE_LINEWRAP = _controls_.TE_LINEWRAP
TE_WORDWRAP = _controls_.TE_WORDWRAP
TE_RICH2 = _controls_.TE_RICH2
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
class TextAttr(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            Font font=wxNullFont, int alignment=TEXT_ALIGNMENT_DEFAULT) -> TextAttr
        """
        newobj = _controls_.new_TextAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls_.delete_TextAttr):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init(*args, **kwargs):
        """Init(self)"""
        return _controls_.TextAttr_Init(*args, **kwargs)

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

class TextAttrPtr(TextAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextAttr
_controls_.TextAttr_swigregister(TextAttrPtr)
TextCtrlNameStr = cvar.TextCtrlNameStr

def TextAttr_Combine(*args, **kwargs):
    """TextAttr_Combine(TextAttr attr, TextAttr attrDef, TextCtrl text) -> TextAttr"""
    return _controls_.TextAttr_Combine(*args, **kwargs)

class TextCtrl(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=TextCtrlNameStr) -> TextCtrl
        """
        newobj = _controls_.new_TextCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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
        """LoadFile(self, String file) -> bool"""
        return _controls_.TextCtrl_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(self, String file=EmptyString) -> bool"""
        return _controls_.TextCtrl_SaveFile(*args, **kwargs)

    def MarkDirty(*args, **kwargs):
        """MarkDirty(self)"""
        return _controls_.TextCtrl_MarkDirty(*args, **kwargs)

    def DiscardEdits(*args, **kwargs):
        """DiscardEdits(self)"""
        return _controls_.TextCtrl_DiscardEdits(*args, **kwargs)

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
        HitTest(Point pt) -> (result, row, col)

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

class TextCtrlPtr(TextCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextCtrl
_controls_.TextCtrl_swigregister(TextCtrlPtr)

def PreTextCtrl(*args, **kwargs):
    """PreTextCtrl() -> TextCtrl"""
    val = _controls_.new_PreTextCtrl(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextUrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int winid, MouseEvent evtMouse, long start, long end) -> TextUrlEvent"""
        newobj = _controls_.new_TextUrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMouseEvent(*args, **kwargs):
        """GetMouseEvent(self) -> MouseEvent"""
        return _controls_.TextUrlEvent_GetMouseEvent(*args, **kwargs)

    def GetURLStart(*args, **kwargs):
        """GetURLStart(self) -> long"""
        return _controls_.TextUrlEvent_GetURLStart(*args, **kwargs)

    def GetURLEnd(*args, **kwargs):
        """GetURLEnd(self) -> long"""
        return _controls_.TextUrlEvent_GetURLEnd(*args, **kwargs)


class TextUrlEventPtr(TextUrlEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextUrlEvent
_controls_.TextUrlEvent_swigregister(TextUrlEventPtr)

EVT_TEXT        = wx.PyEventBinder( wxEVT_COMMAND_TEXT_UPDATED, 1)
EVT_TEXT_ENTER  = wx.PyEventBinder( wxEVT_COMMAND_TEXT_ENTER, 1)
EVT_TEXT_URL    = wx.PyEventBinder( wxEVT_COMMAND_TEXT_URL, 1) 
EVT_TEXT_MAXLEN = wx.PyEventBinder( wxEVT_COMMAND_TEXT_MAXLEN, 1)

#---------------------------------------------------------------------------

class ScrollBar(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SB_HORIZONTAL, 
            Validator validator=DefaultValidator, String name=ScrollBarNameStr) -> ScrollBar
        """
        newobj = _controls_.new_ScrollBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

    def SetScrollbar(*args, **kwargs):
        """
        SetScrollbar(self, int position, int thumbSize, int range, int pageSize, 
            bool refresh=True)

        Sets the scrollbar properties of a built-in scrollbar.
        """
        return _controls_.ScrollBar_SetScrollbar(*args, **kwargs)

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

class ScrollBarPtr(ScrollBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrollBar
_controls_.ScrollBar_swigregister(ScrollBarPtr)
ScrollBarNameStr = cvar.ScrollBarNameStr

def PreScrollBar(*args, **kwargs):
    """PreScrollBar() -> ScrollBar"""
    val = _controls_.new_PreScrollBar(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SP_HORIZONTAL, 
            String name=SPIN_BUTTON_NAME) -> SpinButton
        """
        newobj = _controls_.new_SpinButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class SpinButtonPtr(SpinButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinButton
_controls_.SpinButton_swigregister(SpinButtonPtr)
SPIN_BUTTON_NAME = cvar.SPIN_BUTTON_NAME
SpinCtrlNameStr = cvar.SpinCtrlNameStr

def PreSpinButton(*args, **kwargs):
    """PreSpinButton() -> SpinButton"""
    val = _controls_.new_PreSpinButton(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String value=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=SP_ARROW_KEYS, int min=0, int max=100, 
            int initial=0, String name=SpinCtrlNameStr) -> SpinCtrl
        """
        newobj = _controls_.new_SpinCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class SpinCtrlPtr(SpinCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinCtrl
_controls_.SpinCtrl_swigregister(SpinCtrlPtr)

def PreSpinCtrl(*args, **kwargs):
    """PreSpinCtrl() -> SpinCtrl"""
    val = _controls_.new_PreSpinCtrl(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int winid=0) -> SpinEvent"""
        newobj = _controls_.new_SpinEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> int"""
        return _controls_.SpinEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(self, int pos)"""
        return _controls_.SpinEvent_SetPosition(*args, **kwargs)


class SpinEventPtr(SpinEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinEvent
_controls_.SpinEvent_swigregister(SpinEventPtr)

wxEVT_COMMAND_SPINCTRL_UPDATED = _controls_.wxEVT_COMMAND_SPINCTRL_UPDATED
EVT_SPIN_UP   = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEUP, 1)
EVT_SPIN_DOWN = wx.PyEventBinder( wx.wxEVT_SCROLL_LINEDOWN, 1)
EVT_SPIN      = wx.PyEventBinder( wx.wxEVT_SCROLL_THUMBTRACK, 1)
EVT_SPINCTRL  = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)

#---------------------------------------------------------------------------

class RadioBox(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
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
        newobj = _controls_.new_RadioBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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
        """GetCount(self) -> int"""
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
        """EnableItem(self, int n, bool enable=True)"""
        return _controls_.RadioBox_EnableItem(*args, **kwargs)

    def ShowItem(*args, **kwargs):
        """ShowItem(self, int n, bool show=True)"""
        return _controls_.RadioBox_ShowItem(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount(self) -> int"""
        return _controls_.RadioBox_GetColumnCount(*args, **kwargs)

    def GetRowCount(*args, **kwargs):
        """GetRowCount(self) -> int"""
        return _controls_.RadioBox_GetRowCount(*args, **kwargs)

    def GetNextItem(*args, **kwargs):
        """GetNextItem(self, int item, int dir, long style) -> int"""
        return _controls_.RadioBox_GetNextItem(*args, **kwargs)

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

class RadioBoxPtr(RadioBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioBox
_controls_.RadioBox_swigregister(RadioBoxPtr)
RadioBoxNameStr = cvar.RadioBoxNameStr
RadioButtonNameStr = cvar.RadioButtonNameStr

def PreRadioBox(*args, **kwargs):
    """PreRadioBox() -> RadioBox"""
    val = _controls_.new_PreRadioBox(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=RadioButtonNameStr) -> RadioButton
        """
        newobj = _controls_.new_RadioButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class RadioButtonPtr(RadioButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioButton
_controls_.RadioButton_swigregister(RadioButtonPtr)

def PreRadioButton(*args, **kwargs):
    """PreRadioButton() -> RadioButton"""
    val = _controls_.new_PreRadioButton(*args, **kwargs)
    val.thisown = 1
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

class Slider(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSlider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, int value=0, int minValue=0, 
            int maxValue=100, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=SL_HORIZONTAL, 
            Validator validator=DefaultValidator, 
            String name=SliderNameStr) -> Slider
        """
        if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']
        newobj = _controls_.new_Slider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

class SliderPtr(Slider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Slider
_controls_.Slider_swigregister(SliderPtr)
SliderNameStr = cvar.SliderNameStr

def PreSlider(*args, **kwargs):
    """PreSlider() -> Slider"""
    val = _controls_.new_PreSlider(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToggleButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String label=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=ToggleButtonNameStr) -> ToggleButton
        """
        newobj = _controls_.new_ToggleButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

    def SetLabel(*args, **kwargs):
        """
        SetLabel(self, String label)

        Sets the item's text.
        """
        return _controls_.ToggleButton_SetLabel(*args, **kwargs)

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

class ToggleButtonPtr(ToggleButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToggleButton
_controls_.ToggleButton_swigregister(ToggleButtonPtr)
ToggleButtonNameStr = cvar.ToggleButtonNameStr

def PreToggleButton(*args, **kwargs):
    """PreToggleButton() -> ToggleButton"""
    val = _controls_.new_PreToggleButton(*args, **kwargs)
    val.thisown = 1
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

class BookCtrl(_core.Control):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetPageCount(*args, **kwargs):
        """GetPageCount(self) -> size_t"""
        return _controls_.BookCtrl_GetPageCount(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self, size_t n) -> Window"""
        return _controls_.BookCtrl_GetPage(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _controls_.BookCtrl_GetSelection(*args, **kwargs)

    def SetPageText(*args, **kwargs):
        """SetPageText(self, size_t n, String strText) -> bool"""
        return _controls_.BookCtrl_SetPageText(*args, **kwargs)

    def GetPageText(*args, **kwargs):
        """GetPageText(self, size_t n) -> String"""
        return _controls_.BookCtrl_GetPageText(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(self, ImageList imageList)"""
        return _controls_.BookCtrl_SetImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(self, ImageList imageList)"""
        return _controls_.BookCtrl_AssignImageList(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList(self) -> ImageList"""
        return _controls_.BookCtrl_GetImageList(*args, **kwargs)

    def GetPageImage(*args, **kwargs):
        """GetPageImage(self, size_t n) -> int"""
        return _controls_.BookCtrl_GetPageImage(*args, **kwargs)

    def SetPageImage(*args, **kwargs):
        """SetPageImage(self, size_t n, int imageId) -> bool"""
        return _controls_.BookCtrl_SetPageImage(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(self, Size size)"""
        return _controls_.BookCtrl_SetPageSize(*args, **kwargs)

    def CalcSizeFromPage(*args, **kwargs):
        """CalcSizeFromPage(self, Size sizePage) -> Size"""
        return _controls_.BookCtrl_CalcSizeFromPage(*args, **kwargs)

    def DeletePage(*args, **kwargs):
        """DeletePage(self, size_t n) -> bool"""
        return _controls_.BookCtrl_DeletePage(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(self, size_t n) -> bool"""
        return _controls_.BookCtrl_RemovePage(*args, **kwargs)

    def DeleteAllPages(*args, **kwargs):
        """DeleteAllPages(self) -> bool"""
        return _controls_.BookCtrl_DeleteAllPages(*args, **kwargs)

    def AddPage(*args, **kwargs):
        """AddPage(self, Window page, String text, bool select=False, int imageId=-1) -> bool"""
        return _controls_.BookCtrl_AddPage(*args, **kwargs)

    def InsertPage(*args, **kwargs):
        """
        InsertPage(self, size_t n, Window page, String text, bool select=False, 
            int imageId=-1) -> bool
        """
        return _controls_.BookCtrl_InsertPage(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, size_t n) -> int"""
        return _controls_.BookCtrl_SetSelection(*args, **kwargs)

    def AdvanceSelection(*args, **kwargs):
        """AdvanceSelection(self, bool forward=True)"""
        return _controls_.BookCtrl_AdvanceSelection(*args, **kwargs)

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
        return _controls_.BookCtrl_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)

class BookCtrlPtr(BookCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrl
_controls_.BookCtrl_swigregister(BookCtrlPtr)
NOTEBOOK_NAME = cvar.NOTEBOOK_NAME

def BookCtrl_GetClassDefaultAttributes(*args, **kwargs):
    """
    BookCtrl_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

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
    return _controls_.BookCtrl_GetClassDefaultAttributes(*args, **kwargs)

class BookCtrlEvent(_core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> BookCtrlEvent
        """
        newobj = _controls_.new_BookCtrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> int"""
        return _controls_.BookCtrlEvent_GetSelection(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(self, int nSel)"""
        return _controls_.BookCtrlEvent_SetSelection(*args, **kwargs)

    def GetOldSelection(*args, **kwargs):
        """GetOldSelection(self) -> int"""
        return _controls_.BookCtrlEvent_GetOldSelection(*args, **kwargs)

    def SetOldSelection(*args, **kwargs):
        """SetOldSelection(self, int nOldSel)"""
        return _controls_.BookCtrlEvent_SetOldSelection(*args, **kwargs)


class BookCtrlEventPtr(BookCtrlEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlEvent
_controls_.BookCtrlEvent_swigregister(BookCtrlEventPtr)

#---------------------------------------------------------------------------

NB_FIXEDWIDTH = _controls_.NB_FIXEDWIDTH
NB_TOP = _controls_.NB_TOP
NB_LEFT = _controls_.NB_LEFT
NB_RIGHT = _controls_.NB_RIGHT
NB_BOTTOM = _controls_.NB_BOTTOM
NB_MULTILINE = _controls_.NB_MULTILINE
NB_HITTEST_NOWHERE = _controls_.NB_HITTEST_NOWHERE
NB_HITTEST_ONICON = _controls_.NB_HITTEST_ONICON
NB_HITTEST_ONLABEL = _controls_.NB_HITTEST_ONLABEL
NB_HITTEST_ONITEM = _controls_.NB_HITTEST_ONITEM
class Notebook(BookCtrl):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=NOTEBOOK_NAME) -> Notebook
        """
        newobj = _controls_.new_Notebook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=NOTEBOOK_NAME) -> bool
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

    def HitTest(*args, **kwargs):
        """
        HitTest(Point pt) -> (tab, where)

        Returns the tab which is hit, and flags indicating where using
        wx.NB_HITTEST flags.
        """
        return _controls_.Notebook_HitTest(*args, **kwargs)

    def CalcSizeFromPage(*args, **kwargs):
        """CalcSizeFromPage(self, Size sizePage) -> Size"""
        return _controls_.Notebook_CalcSizeFromPage(*args, **kwargs)

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

class NotebookPtr(Notebook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Notebook
_controls_.Notebook_swigregister(NotebookPtr)

def PreNotebook(*args, **kwargs):
    """PreNotebook() -> Notebook"""
    val = _controls_.new_PreNotebook(*args, **kwargs)
    val.thisown = 1
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

class NotebookEvent(BookCtrlEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> NotebookEvent
        """
        newobj = _controls_.new_NotebookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class NotebookEventPtr(NotebookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotebookEvent
_controls_.NotebookEvent_swigregister(NotebookEventPtr)

wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
# wxNotebook events
EVT_NOTEBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, 1 )
EVT_NOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, 1 )

#----------------------------------------------------------------------------

class NotebookPage(wx.Panel):
    """
    There is an old (and apparently unsolvable) bug when placing a
    window with a nonstandard background colour in a wxNotebook on
    wxGTK, as the notbooks's background colour would always be used
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
        EVT_SIZE(self, self.OnSize)

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
class Listbook(BookCtrl):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=EmptyString) -> Listbook
        """
        newobj = _controls_.new_Listbook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, String name=EmptyString) -> bool
        """
        return _controls_.Listbook_Create(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.Listbook_IsVertical(*args, **kwargs)

    def GetListView(*args, **kwargs):
        """GetListView(self) -> ListView"""
        return _controls_.Listbook_GetListView(*args, **kwargs)


class ListbookPtr(Listbook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Listbook
_controls_.Listbook_swigregister(ListbookPtr)

def PreListbook(*args, **kwargs):
    """PreListbook() -> Listbook"""
    val = _controls_.new_PreListbook(*args, **kwargs)
    val.thisown = 1
    return val

class ListbookEvent(BookCtrlEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> ListbookEvent
        """
        newobj = _controls_.new_ListbookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ListbookEventPtr(ListbookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListbookEvent
_controls_.ListbookEvent_swigregister(ListbookEventPtr)

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
class Choicebook(BookCtrl):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChoicebook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=EmptyString) -> Choicebook
        """
        newobj = _controls_.new_Choicebook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, String name=EmptyString) -> bool
        """
        return _controls_.Choicebook_Create(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical(self) -> bool"""
        return _controls_.Choicebook_IsVertical(*args, **kwargs)

    def DeleteAllPages(*args, **kwargs):
        """DeleteAllPages(self) -> bool"""
        return _controls_.Choicebook_DeleteAllPages(*args, **kwargs)


class ChoicebookPtr(Choicebook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Choicebook
_controls_.Choicebook_swigregister(ChoicebookPtr)

def PreChoicebook(*args, **kwargs):
    """PreChoicebook() -> Choicebook"""
    val = _controls_.new_PreChoicebook(*args, **kwargs)
    val.thisown = 1
    return val

class ChoicebookEvent(BookCtrlEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChoicebookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
            int nOldSel=-1) -> ChoicebookEvent
        """
        newobj = _controls_.new_ChoicebookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ChoicebookEventPtr(ChoicebookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ChoicebookEvent
_controls_.ChoicebookEvent_swigregister(ChoicebookEventPtr)

wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED = _controls_.wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED
wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING = _controls_.wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING
EVT_CHOICEBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, 1 )
EVT_CHOICEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING, 1 )

#---------------------------------------------------------------------------

class BookCtrlSizer(_core.Sizer):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, BookCtrl nb) -> BookCtrlSizer"""
        newobj = _controls_.new_BookCtrlSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def RecalcSizes(*args, **kwargs):
        """
        RecalcSizes(self)

        Using the sizes calculated by `CalcMin` reposition and resize all the
        items managed by this sizer.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return _controls_.BookCtrlSizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> Size

        This method is where the sizer will do the actual calculation of its
        children's minimal sizes.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return _controls_.BookCtrlSizer_CalcMin(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl(self) -> BookCtrl"""
        return _controls_.BookCtrlSizer_GetControl(*args, **kwargs)


class BookCtrlSizerPtr(BookCtrlSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlSizer
_controls_.BookCtrlSizer_swigregister(BookCtrlSizerPtr)

class NotebookSizer(_core.Sizer):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Notebook nb) -> NotebookSizer"""
        newobj = _controls_.new_NotebookSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def RecalcSizes(*args, **kwargs):
        """
        RecalcSizes(self)

        Using the sizes calculated by `CalcMin` reposition and resize all the
        items managed by this sizer.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return _controls_.NotebookSizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """
        CalcMin(self) -> Size

        This method is where the sizer will do the actual calculation of its
        children's minimal sizes.  You should not need to call this directly as
        it is called by `Layout`.
        """
        return _controls_.NotebookSizer_CalcMin(*args, **kwargs)

    def GetNotebook(*args, **kwargs):
        """GetNotebook(self) -> Notebook"""
        return _controls_.NotebookSizer_GetNotebook(*args, **kwargs)


class NotebookSizerPtr(NotebookSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotebookSizer
_controls_.NotebookSizer_swigregister(NotebookSizerPtr)

#---------------------------------------------------------------------------

TOOL_STYLE_BUTTON = _controls_.TOOL_STYLE_BUTTON
TOOL_STYLE_SEPARATOR = _controls_.TOOL_STYLE_SEPARATOR
TOOL_STYLE_CONTROL = _controls_.TOOL_STYLE_CONTROL
TB_HORIZONTAL = _controls_.TB_HORIZONTAL
TB_VERTICAL = _controls_.TB_VERTICAL
TB_3DBUTTONS = _controls_.TB_3DBUTTONS
TB_FLAT = _controls_.TB_FLAT
TB_DOCKABLE = _controls_.TB_DOCKABLE
TB_NOICONS = _controls_.TB_NOICONS
TB_TEXT = _controls_.TB_TEXT
TB_NODIVIDER = _controls_.TB_NODIVIDER
TB_NOALIGN = _controls_.TB_NOALIGN
TB_HORZ_LAYOUT = _controls_.TB_HORZ_LAYOUT
TB_HORZ_TEXT = _controls_.TB_HORZ_TEXT
class ToolBarToolBase(_core.Object):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarToolBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
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


class ToolBarToolBasePtr(ToolBarToolBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarToolBase
_controls_.ToolBarToolBase_swigregister(ToolBarToolBasePtr)

class ToolBarBase(_core.Control):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
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


class ToolBarBasePtr(ToolBarBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarBase
_controls_.ToolBarBase_swigregister(ToolBarBasePtr)

class ToolBar(ToolBarBase):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, 
            String name=wxPyToolBarNameStr) -> ToolBar
        """
        newobj = _controls_.new_ToolBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, 
            String name=wxPyToolBarNameStr) -> bool
        """
        return _controls_.ToolBar_Create(*args, **kwargs)

    def FindToolForPosition(*args, **kwargs):
        """FindToolForPosition(self, int x, int y) -> ToolBarToolBase"""
        return _controls_.ToolBar_FindToolForPosition(*args, **kwargs)

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

class ToolBarPtr(ToolBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBar
_controls_.ToolBar_swigregister(ToolBarPtr)

def PreToolBar(*args, **kwargs):
    """PreToolBar() -> ToolBar"""
    val = _controls_.new_PreToolBar(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItemAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Colour colText=wxNullColour, Colour colBack=wxNullColour, 
            Font font=wxNullFont) -> ListItemAttr
        """
        newobj = _controls_.new_ListItemAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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

    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        return _controls_.ListItemAttr_Destroy(*args, **kwargs)


class ListItemAttrPtr(ListItemAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItemAttr
_controls_.ListItemAttr_swigregister(ListItemAttrPtr)
ListCtrlNameStr = cvar.ListCtrlNameStr

#---------------------------------------------------------------------------

class ListItem(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ListItem"""
        newobj = _controls_.new_ListItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls_.delete_ListItem):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

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

class ListItemPtr(ListItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItem
_controls_.ListItem_swigregister(ListItemPtr)

#---------------------------------------------------------------------------

class ListEvent(_core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int id=0) -> ListEvent"""
        newobj = _controls_.new_ListEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class ListEventPtr(ListEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListEvent
_controls_.ListEvent_swigregister(ListEventPtr)

wxEVT_COMMAND_LIST_BEGIN_DRAG = _controls_.wxEVT_COMMAND_LIST_BEGIN_DRAG
wxEVT_COMMAND_LIST_BEGIN_RDRAG = _controls_.wxEVT_COMMAND_LIST_BEGIN_RDRAG
wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT = _controls_.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
wxEVT_COMMAND_LIST_END_LABEL_EDIT = _controls_.wxEVT_COMMAND_LIST_END_LABEL_EDIT
wxEVT_COMMAND_LIST_DELETE_ITEM = _controls_.wxEVT_COMMAND_LIST_DELETE_ITEM
wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS = _controls_.wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
wxEVT_COMMAND_LIST_GET_INFO = _controls_.wxEVT_COMMAND_LIST_GET_INFO
wxEVT_COMMAND_LIST_SET_INFO = _controls_.wxEVT_COMMAND_LIST_SET_INFO
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
EVT_LIST_GET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_LIST_GET_INFO         , 1)
EVT_LIST_SET_INFO          = wx.PyEventBinder(wxEVT_COMMAND_LIST_SET_INFO         , 1)
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

EVT_LIST_GET_INFO = wx._deprecated(EVT_LIST_GET_INFO)
EVT_LIST_SET_INFO = wx._deprecated(EVT_LIST_SET_INFO)

#---------------------------------------------------------------------------

class ListCtrl(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_ICON, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> ListCtrl
        """
        newobj = _controls_.new_ListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, ListCtrl)

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

    def SetForegroundColour(*args, **kwargs):
        """SetForegroundColour(self, Colour col) -> bool"""
        return _controls_.ListCtrl_SetForegroundColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour col) -> bool"""
        return _controls_.ListCtrl_SetBackgroundColour(*args, **kwargs)

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

    def SetItemSpacing(*args, **kwargs):
        """SetItemSpacing(self, int spacing, bool isSmall=False)"""
        return _controls_.ListCtrl_SetItemSpacing(*args, **kwargs)

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

    def SetWindowStyleFlag(*args, **kwargs):
        """
        SetWindowStyleFlag(self, long style)

        Sets the style of the window. Please note that some styles cannot be
        changed after the window creation and that Refresh() might need to be
        called after changing the others for the change to take place
        immediately.
        """
        return _controls_.ListCtrl_SetWindowStyleFlag(*args, **kwargs)

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
         in the second return value (see wxLIST_HITTEST_... flags.)
        """
        return _controls_.ListCtrl_HitTest(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(self, ListItem info) -> long"""
        return _controls_.ListCtrl_InsertItem(*args, **kwargs)

    def InsertStringItem(*args, **kwargs):
        """InsertStringItem(self, long index, String label) -> long"""
        return _controls_.ListCtrl_InsertStringItem(*args, **kwargs)

    def InsertImageItem(*args, **kwargs):
        """InsertImageItem(self, long index, int imageIndex) -> long"""
        return _controls_.ListCtrl_InsertImageItem(*args, **kwargs)

    def InsertImageStringItem(*args, **kwargs):
        """InsertImageStringItem(self, long index, String label, int imageIndex) -> long"""
        return _controls_.ListCtrl_InsertImageStringItem(*args, **kwargs)

    def InsertColumnInfo(*args, **kwargs):
        """InsertColumnInfo(self, long col, ListItem info) -> long"""
        return _controls_.ListCtrl_InsertColumnInfo(*args, **kwargs)

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
        return self.GetItemState(idx, wx.LIST_STATE_SELECTED) != 0

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

class ListCtrlPtr(ListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListCtrl
_controls_.ListCtrl_swigregister(ListCtrlPtr)

def PreListCtrl(*args, **kwargs):
    """PreListCtrl() -> ListCtrl"""
    val = _controls_.new_PreListCtrl(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListView instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=LC_REPORT, 
            Validator validator=DefaultValidator, String name=ListCtrlNameStr) -> ListView
        """
        newobj = _controls_.new_ListView(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class ListViewPtr(ListView):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListView
_controls_.ListView_swigregister(ListViewPtr)

def PreListView(*args, **kwargs):
    """PreListView() -> ListView"""
    val = _controls_.new_PreListView(*args, **kwargs)
    val.thisown = 1
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
TR_MAC_BUTTONS = _controls_.TR_MAC_BUTTONS
TR_AQUA_BUTTONS = _controls_.TR_AQUA_BUTTONS
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeItemId instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> TreeItemId"""
        newobj = _controls_.new_TreeItemId(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls_.delete_TreeItemId):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

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

class TreeItemIdPtr(TreeItemId):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeItemId
_controls_.TreeItemId_swigregister(TreeItemIdPtr)
TreeCtrlNameStr = cvar.TreeCtrlNameStr

class TreeItemData(object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeItemData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, PyObject obj=None) -> TreeItemData"""
        newobj = _controls_.new_TreeItemData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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
        return _controls_.TreeItemData_Destroy(*args, **kwargs)


class TreeItemDataPtr(TreeItemData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeItemData
_controls_.TreeItemData_swigregister(TreeItemDataPtr)

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

class TreeEvent(_core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType commandType=wxEVT_NULL, int id=0) -> TreeEvent"""
        newobj = _controls_.new_TreeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class TreeEventPtr(TreeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeEvent
_controls_.TreeEvent_swigregister(TreeEventPtr)

#---------------------------------------------------------------------------

class TreeCtrl(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
            Validator validator=DefaultValidator, 
            String name=TreeCtrlNameStr) -> TreeCtrl
        """
        newobj = _controls_.new_TreeCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeCtrl)

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
        """GetCount(self) -> size_t"""
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

class TreeCtrlPtr(TreeCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeCtrl
_controls_.TreeCtrl_swigregister(TreeCtrlPtr)

def PreTreeCtrl(*args, **kwargs):
    """PreTreeCtrl() -> TreeCtrl"""
    val = _controls_.new_PreTreeCtrl(*args, **kwargs)
    val.thisown = 1
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDirCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String dir=DirDialogDefaultFolderStr, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, 
            String filter=EmptyString, 
            int defaultFilter=0, String name=TreeCtrlNameStr) -> GenericDirCtrl
        """
        newobj = _controls_.new_GenericDirCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class GenericDirCtrlPtr(GenericDirCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GenericDirCtrl
_controls_.GenericDirCtrl_swigregister(GenericDirCtrlPtr)
DirDialogDefaultFolderStr = cvar.DirDialogDefaultFolderStr

def PreGenericDirCtrl(*args, **kwargs):
    """PreGenericDirCtrl() -> GenericDirCtrl"""
    val = _controls_.new_PreGenericDirCtrl(*args, **kwargs)
    val.thisown = 1
    return val

class DirFilterListCtrl(Choice):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDirFilterListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, GenericDirCtrl parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0) -> DirFilterListCtrl
        """
        newobj = _controls_.new_DirFilterListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class DirFilterListCtrlPtr(DirFilterListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DirFilterListCtrl
_controls_.DirFilterListCtrl_swigregister(DirFilterListCtrlPtr)

def PreDirFilterListCtrl(*args, **kwargs):
    """PreDirFilterListCtrl() -> DirFilterListCtrl"""
    val = _controls_.new_PreDirFilterListCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class PyControl(_core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, Validator validator=DefaultValidator, 
            String name=ControlNameStr) -> PyControl
        """
        newobj = _controls_.new_PyControl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, PyControl)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _controls_.PyControl__setCallbackInfo(*args, **kwargs)

    def SetBestSize(*args, **kwargs):
        """SetBestSize(self, Size size)"""
        return _controls_.PyControl_SetBestSize(*args, **kwargs)

    def base_DoMoveWindow(*args, **kwargs):
        """base_DoMoveWindow(self, int x, int y, int width, int height)"""
        return _controls_.PyControl_base_DoMoveWindow(*args, **kwargs)

    def base_DoSetSize(*args, **kwargs):
        """base_DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _controls_.PyControl_base_DoSetSize(*args, **kwargs)

    def base_DoSetClientSize(*args, **kwargs):
        """base_DoSetClientSize(self, int width, int height)"""
        return _controls_.PyControl_base_DoSetClientSize(*args, **kwargs)

    def base_DoSetVirtualSize(*args, **kwargs):
        """base_DoSetVirtualSize(self, int x, int y)"""
        return _controls_.PyControl_base_DoSetVirtualSize(*args, **kwargs)

    def base_DoGetSize(*args, **kwargs):
        """base_DoGetSize() -> (width, height)"""
        return _controls_.PyControl_base_DoGetSize(*args, **kwargs)

    def base_DoGetClientSize(*args, **kwargs):
        """base_DoGetClientSize() -> (width, height)"""
        return _controls_.PyControl_base_DoGetClientSize(*args, **kwargs)

    def base_DoGetPosition(*args, **kwargs):
        """base_DoGetPosition() -> (x,y)"""
        return _controls_.PyControl_base_DoGetPosition(*args, **kwargs)

    def base_DoGetVirtualSize(*args, **kwargs):
        """base_DoGetVirtualSize(self) -> Size"""
        return _controls_.PyControl_base_DoGetVirtualSize(*args, **kwargs)

    def base_DoGetBestSize(*args, **kwargs):
        """base_DoGetBestSize(self) -> Size"""
        return _controls_.PyControl_base_DoGetBestSize(*args, **kwargs)

    def base_InitDialog(*args, **kwargs):
        """base_InitDialog(self)"""
        return _controls_.PyControl_base_InitDialog(*args, **kwargs)

    def base_TransferDataToWindow(*args, **kwargs):
        """base_TransferDataToWindow(self) -> bool"""
        return _controls_.PyControl_base_TransferDataToWindow(*args, **kwargs)

    def base_TransferDataFromWindow(*args, **kwargs):
        """base_TransferDataFromWindow(self) -> bool"""
        return _controls_.PyControl_base_TransferDataFromWindow(*args, **kwargs)

    def base_Validate(*args, **kwargs):
        """base_Validate(self) -> bool"""
        return _controls_.PyControl_base_Validate(*args, **kwargs)

    def base_AcceptsFocus(*args, **kwargs):
        """base_AcceptsFocus(self) -> bool"""
        return _controls_.PyControl_base_AcceptsFocus(*args, **kwargs)

    def base_AcceptsFocusFromKeyboard(*args, **kwargs):
        """base_AcceptsFocusFromKeyboard(self) -> bool"""
        return _controls_.PyControl_base_AcceptsFocusFromKeyboard(*args, **kwargs)

    def base_GetMaxSize(*args, **kwargs):
        """base_GetMaxSize(self) -> Size"""
        return _controls_.PyControl_base_GetMaxSize(*args, **kwargs)

    def base_AddChild(*args, **kwargs):
        """base_AddChild(self, Window child)"""
        return _controls_.PyControl_base_AddChild(*args, **kwargs)

    def base_RemoveChild(*args, **kwargs):
        """base_RemoveChild(self, Window child)"""
        return _controls_.PyControl_base_RemoveChild(*args, **kwargs)

    def base_ShouldInheritColours(*args, **kwargs):
        """base_ShouldInheritColours(self) -> bool"""
        return _controls_.PyControl_base_ShouldInheritColours(*args, **kwargs)

    def base_ApplyParentThemeBackground(*args, **kwargs):
        """base_ApplyParentThemeBackground(self, Colour c)"""
        return _controls_.PyControl_base_ApplyParentThemeBackground(*args, **kwargs)

    def base_GetDefaultAttributes(*args, **kwargs):
        """base_GetDefaultAttributes(self) -> VisualAttributes"""
        return _controls_.PyControl_base_GetDefaultAttributes(*args, **kwargs)


class PyControlPtr(PyControl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyControl
_controls_.PyControl_swigregister(PyControlPtr)

def PrePyControl(*args, **kwargs):
    """PrePyControl() -> PyControl"""
    val = _controls_.new_PrePyControl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

FRAME_EX_CONTEXTHELP = _controls_.FRAME_EX_CONTEXTHELP
DIALOG_EX_CONTEXTHELP = _controls_.DIALOG_EX_CONTEXTHELP
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxEventType type=wxEVT_NULL, int winid=0, Point pt=DefaultPosition) -> HelpEvent"""
        newobj = _controls_.new_HelpEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
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


class HelpEventPtr(HelpEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpEvent
_controls_.HelpEvent_swigregister(HelpEventPtr)

class ContextHelp(_core.Object):
    """
    This class changes the cursor to a query and puts the application into
    a 'context-sensitive help mode'. When the user left-clicks on a window
    within the specified window, a ``EVT_HELP`` event is sent to that
    control, and the application may respond to it by popping up some
    help.

    There are a couple of ways to invoke this behaviour implicitly:

        * Use the wx.DIALOG_EX_CONTEXTHELP extended style for a dialog
          (Windows only). This will put a question mark in the titlebar,
          and Windows will put the application into context-sensitive help
          mode automatically, with further programming.

        * Create a `wx.ContextHelpButton`, whose predefined behaviour is
          to create a context help object. Normally you will write your
          application so that this button is only added to a dialog for
          non-Windows platforms (use ``wx.DIALOG_EX_CONTEXTHELP`` on
          Windows).

    :see: `wx.ContextHelpButton`

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelp instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window window=None, bool doNow=True) -> ContextHelp

        Constructs a context help object, calling BeginContextHelp if doNow is
        true (the default).

        If window is None, the top window is used.
        """
        newobj = _controls_.new_ContextHelp(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls_.delete_ContextHelp):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

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


class ContextHelpPtr(ContextHelp):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelp
_controls_.ContextHelp_swigregister(ContextHelpPtr)

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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelpButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=ID_CONTEXT_HELP, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=BU_AUTODRAW) -> ContextHelpButton

        Constructor, creating and showing a context help button.
        """
        newobj = _controls_.new_ContextHelpButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class ContextHelpButtonPtr(ContextHelpButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelpButton
_controls_.ContextHelpButton_swigregister(ContextHelpButtonPtr)

class HelpProvider(object):
    """
    wx.HelpProvider is an abstract class used by a program
    implementing context-sensitive help to show the help text for the
    given window.

    The current help provider must be explicitly set by the
    application using wx.HelpProvider.Set().
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
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
        return _controls_.HelpProvider_Destroy(*args, **kwargs)


class HelpProviderPtr(HelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpProvider
_controls_.HelpProvider_swigregister(HelpProviderPtr)

def HelpProvider_Set(*args, **kwargs):
    """
    HelpProvider_Set(HelpProvider helpProvider) -> HelpProvider

    Sset the current, application-wide help provider. Returns the previous
    one.  Unlike some other classes, the help provider is not created on
    demand. This must be explicitly done by the application.
    """
    return _controls_.HelpProvider_Set(*args, **kwargs)

def HelpProvider_Get(*args, **kwargs):
    """
    HelpProvider_Get() -> HelpProvider

    Return the current application-wide help provider.
    """
    return _controls_.HelpProvider_Get(*args, **kwargs)

class SimpleHelpProvider(HelpProvider):
    """
    wx.SimpleHelpProvider is an implementation of `wx.HelpProvider` which
    supports only plain text help strings, and shows the string associated
    with the control (if any) in a tooltip.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSimpleHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> SimpleHelpProvider

        wx.SimpleHelpProvider is an implementation of `wx.HelpProvider` which
        supports only plain text help strings, and shows the string associated
        with the control (if any) in a tooltip.
        """
        newobj = _controls_.new_SimpleHelpProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class SimpleHelpProviderPtr(SimpleHelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SimpleHelpProvider
_controls_.SimpleHelpProvider_swigregister(SimpleHelpProviderPtr)

#---------------------------------------------------------------------------

class DragImage(_core.Object):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDragImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Bitmap image, Cursor cursor=wxNullCursor) -> DragImage"""
        newobj = _controls_.new_DragImage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls_.delete_DragImage):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

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


class DragImagePtr(DragImage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DragImage
_controls_.DragImage_swigregister(DragImagePtr)

def DragIcon(*args, **kwargs):
    """DragIcon(Icon image, Cursor cursor=wxNullCursor) -> DragImage"""
    val = _controls_.new_DragIcon(*args, **kwargs)
    val.thisown = 1
    return val

def DragString(*args, **kwargs):
    """DragString(String str, Cursor cursor=wxNullCursor) -> DragImage"""
    val = _controls_.new_DragString(*args, **kwargs)
    val.thisown = 1
    return val

def DragTreeItem(*args, **kwargs):
    """DragTreeItem(TreeCtrl treeCtrl, TreeItemId id) -> DragImage"""
    val = _controls_.new_DragTreeItem(*args, **kwargs)
    val.thisown = 1
    return val

def DragListItem(*args, **kwargs):
    """DragListItem(ListCtrl listCtrl, long id) -> DragImage"""
    val = _controls_.new_DragListItem(*args, **kwargs)
    val.thisown = 1
    return val


