# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _controls

import core
wx = core 
#---------------------------------------------------------------------------

BU_LEFT = _controls.BU_LEFT
BU_TOP = _controls.BU_TOP
BU_RIGHT = _controls.BU_RIGHT
BU_BOTTOM = _controls.BU_BOTTOM
BU_EXACTFIT = _controls.BU_EXACTFIT
BU_AUTODRAW = _controls.BU_AUTODRAW
class Button(core.Control):
    """A button is a control that contains a text string, and is one of the most
common elements of a GUI.  It may be placed on a dialog box or panel, or
indeed almost any other window."""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyButtonNameStr) -> Button

Create and show a button."""
        newobj = _controls.new_Button(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyButtonNameStr) -> bool

Acutally create the GUI Button for 2-phase creation."""
        return _controls.Button_Create(*args, **kwargs)

    def SetDefault(*args, **kwargs):
        """SetDefault()

This sets the button to be the default item for the panel or dialog box."""
        return _controls.Button_SetDefault(*args, **kwargs)

    def GetDefaultSize(*args, **kwargs):
        """Button.GetDefaultSize() -> Size"""
        return _controls.Button_GetDefaultSize(*args, **kwargs)

    GetDefaultSize = staticmethod(GetDefaultSize)

class ButtonPtr(Button):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Button
_controls.Button_swigregister(ButtonPtr)

def PreButton(*args, **kwargs):
    """PreButton() -> Button

Precreate a Button for 2-phase creation."""
    val = _controls.new_PreButton(*args, **kwargs)
    val.thisown = 1
    return val

def Button_GetDefaultSize(*args, **kwargs):
    """Button_GetDefaultSize() -> Size"""
    return _controls.Button_GetDefaultSize(*args, **kwargs)

class BitmapButton(Button):
    """A Buttont that contains a bitmap."""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmapButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxBitmap bitmap, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=BU_AUTODRAW, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyButtonNameStr) -> BitmapButton

Create and show a button."""
        newobj = _controls.new_BitmapButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxBitmap bitmap, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=BU_AUTODRAW, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyButtonNameStr) -> bool

Acutally create the GUI BitmapButton for 2-phase creation."""
        return _controls.BitmapButton_Create(*args, **kwargs)

    def GetBitmapLabel(*args, **kwargs):
        """GetBitmapLabel() -> wxBitmap

Returns the label bitmap (the one passed to the constructor)."""
        return _controls.BitmapButton_GetBitmapLabel(*args, **kwargs)

    def GetBitmapDisabled(*args, **kwargs):
        """GetBitmapDisabled() -> wxBitmap

Returns the bitmap for the disabled state."""
        return _controls.BitmapButton_GetBitmapDisabled(*args, **kwargs)

    def GetBitmapFocus(*args, **kwargs):
        """GetBitmapFocus() -> wxBitmap

Returns the bitmap for the focused state."""
        return _controls.BitmapButton_GetBitmapFocus(*args, **kwargs)

    def GetBitmapSelected(*args, **kwargs):
        """GetBitmapSelected() -> wxBitmap

Returns the bitmap for the selected state."""
        return _controls.BitmapButton_GetBitmapSelected(*args, **kwargs)

    def SetBitmapDisabled(*args, **kwargs):
        """SetBitmapDisabled(wxBitmap bitmap)

Sets the bitmap for the disabled button appearance."""
        return _controls.BitmapButton_SetBitmapDisabled(*args, **kwargs)

    def SetBitmapFocus(*args, **kwargs):
        """SetBitmapFocus(wxBitmap bitmap)

Sets the bitmap for the button appearance when it has the keyboard focus."""
        return _controls.BitmapButton_SetBitmapFocus(*args, **kwargs)

    def SetBitmapSelected(*args, **kwargs):
        """SetBitmapSelected(wxBitmap bitmap)

Sets the bitmap for the selected (depressed) button appearance."""
        return _controls.BitmapButton_SetBitmapSelected(*args, **kwargs)

    def SetBitmapLabel(*args, **kwargs):
        """SetBitmapLabel(wxBitmap bitmap)

Sets the bitmap label for the button.  This is the bitmap used for the
unselected state, and for all other states if no other bitmaps are provided."""
        return _controls.BitmapButton_SetBitmapLabel(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(int x, int y)"""
        return _controls.BitmapButton_SetMargins(*args, **kwargs)

    def GetMarginX(*args, **kwargs):
        """GetMarginX() -> int"""
        return _controls.BitmapButton_GetMarginX(*args, **kwargs)

    def GetMarginY(*args, **kwargs):
        """GetMarginY() -> int"""
        return _controls.BitmapButton_GetMarginY(*args, **kwargs)


class BitmapButtonPtr(BitmapButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BitmapButton
_controls.BitmapButton_swigregister(BitmapButtonPtr)

def PreBitmapButton(*args, **kwargs):
    """PreBitmapButton() -> BitmapButton

Precreate a BitmapButton for 2-phase creation."""
    val = _controls.new_PreBitmapButton(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

CHK_2STATE = _controls.CHK_2STATE
CHK_3STATE = _controls.CHK_3STATE
CHK_ALLOW_3RD_STATE_FOR_USER = _controls.CHK_ALLOW_3RD_STATE_FOR_USER
CHK_UNCHECKED = _controls.CHK_UNCHECKED
CHK_CHECKED = _controls.CHK_CHECKED
CHK_UNDETERMINED = _controls.CHK_UNDETERMINED
class CheckBox(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyCheckBoxNameStr) -> CheckBox"""
        newobj = _controls.new_CheckBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyCheckBoxNameStr) -> bool"""
        return _controls.CheckBox_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> bool"""
        return _controls.CheckBox_GetValue(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked() -> bool"""
        return _controls.CheckBox_IsChecked(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(bool state)"""
        return _controls.CheckBox_SetValue(*args, **kwargs)

    def Get3StateValue(*args, **kwargs):
        """Get3StateValue() -> wxCheckBoxState"""
        return _controls.CheckBox_Get3StateValue(*args, **kwargs)

    def Set3StateValue(*args, **kwargs):
        """Set3StateValue(wxCheckBoxState state)"""
        return _controls.CheckBox_Set3StateValue(*args, **kwargs)

    def Is3State(*args, **kwargs):
        """Is3State() -> bool"""
        return _controls.CheckBox_Is3State(*args, **kwargs)

    def Is3rdStateAllowedForUser(*args, **kwargs):
        """Is3rdStateAllowedForUser() -> bool"""
        return _controls.CheckBox_Is3rdStateAllowedForUser(*args, **kwargs)


class CheckBoxPtr(CheckBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckBox
_controls.CheckBox_swigregister(CheckBoxPtr)

def PreCheckBox(*args, **kwargs):
    """PreCheckBox() -> CheckBox"""
    val = _controls.new_PreCheckBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Choice(core.ControlWithItems):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChoice instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyChoiceNameStr) -> Choice"""
        newobj = _controls.new_Choice(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyChoiceNameStr) -> bool"""
        return _controls.Choice_Create(*args, **kwargs)

    def GetColumns(*args, **kwargs):
        """GetColumns() -> int"""
        return _controls.Choice_GetColumns(*args, **kwargs)

    def SetColumns(*args, **kwargs):
        """SetColumns(int n=1)"""
        return _controls.Choice_SetColumns(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int n)"""
        return _controls.Choice_SetSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(wxString string)"""
        return _controls.Choice_SetStringSelection(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(int n, wxString s)"""
        return _controls.Choice_SetString(*args, **kwargs)


class ChoicePtr(Choice):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Choice
_controls.Choice_swigregister(ChoicePtr)

def PreChoice(*args, **kwargs):
    """PreChoice() -> Choice"""
    val = _controls.new_PreChoice(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ComboBox(core.Control,core.ItemContainer):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxComboBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyComboBoxNameStr) -> ComboBox"""
        newobj = _controls.new_ComboBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyComboBoxNameStr) -> bool"""
        return _controls.ComboBox_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> wxString"""
        return _controls.ComboBox_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(wxString value)"""
        return _controls.ComboBox_SetValue(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy()"""
        return _controls.ComboBox_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """Cut()"""
        return _controls.ComboBox_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste()"""
        return _controls.ComboBox_Paste(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """SetInsertionPoint(long pos)"""
        return _controls.ComboBox_SetInsertionPoint(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """GetInsertionPoint() -> long"""
        return _controls.ComboBox_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """GetLastPosition() -> long"""
        return _controls.ComboBox_GetLastPosition(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(long from, long to, wxString value)"""
        return _controls.ComboBox_Replace(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int n)"""
        return _controls.ComboBox_SetSelection(*args, **kwargs)

    def SetMark(*args, **kwargs):
        """SetMark(long from, long to)"""
        return _controls.ComboBox_SetMark(*args, **kwargs)

    def SetEditable(*args, **kwargs):
        """SetEditable(bool editable)"""
        return _controls.ComboBox_SetEditable(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """SetInsertionPointEnd()"""
        return _controls.ComboBox_SetInsertionPointEnd(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(long from, long to)"""
        return _controls.ComboBox_Remove(*args, **kwargs)


class ComboBoxPtr(ComboBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ComboBox
_controls.ComboBox_swigregister(ComboBoxPtr)

def PreComboBox(*args, **kwargs):
    """PreComboBox() -> ComboBox"""
    val = _controls.new_PreComboBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

GA_HORIZONTAL = _controls.GA_HORIZONTAL
GA_VERTICAL = _controls.GA_VERTICAL
GA_SMOOTH = _controls.GA_SMOOTH
GA_PROGRESSBAR = _controls.GA_PROGRESSBAR
class Gauge(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGauge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, int range, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=GA_HORIZONTAL, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyGaugeNameStr) -> Gauge"""
        newobj = _controls.new_Gauge(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, int range, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=GA_HORIZONTAL, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyGaugeNameStr) -> bool"""
        return _controls.Gauge_Create(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(int range)"""
        return _controls.Gauge_SetRange(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange() -> int"""
        return _controls.Gauge_GetRange(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(int pos)"""
        return _controls.Gauge_SetValue(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> int"""
        return _controls.Gauge_GetValue(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.Gauge_IsVertical(*args, **kwargs)

    def SetShadowWidth(*args, **kwargs):
        """SetShadowWidth(int w)"""
        return _controls.Gauge_SetShadowWidth(*args, **kwargs)

    def GetShadowWidth(*args, **kwargs):
        """GetShadowWidth() -> int"""
        return _controls.Gauge_GetShadowWidth(*args, **kwargs)

    def SetBezelFace(*args, **kwargs):
        """SetBezelFace(int w)"""
        return _controls.Gauge_SetBezelFace(*args, **kwargs)

    def GetBezelFace(*args, **kwargs):
        """GetBezelFace() -> int"""
        return _controls.Gauge_GetBezelFace(*args, **kwargs)


class GaugePtr(Gauge):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Gauge
_controls.Gauge_swigregister(GaugePtr)

def PreGauge(*args, **kwargs):
    """PreGauge() -> Gauge"""
    val = _controls.new_PreGauge(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticBox(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticBoxNameStr) -> StaticBox"""
        newobj = _controls.new_StaticBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticBoxNameStr) -> bool"""
        return _controls.StaticBox_Create(*args, **kwargs)


class StaticBoxPtr(StaticBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBox
_controls.StaticBox_swigregister(StaticBoxPtr)

def PreStaticBox(*args, **kwargs):
    """PreStaticBox() -> StaticBox"""
    val = _controls.new_PreStaticBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticLine(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticLine instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=LI_HORIZONTAL, 
    wxString name=wxPyStaticTextNameStr) -> StaticLine"""
        newobj = _controls.new_StaticLine(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=LI_HORIZONTAL, 
    wxString name=wxPyStaticTextNameStr) -> bool"""
        return _controls.StaticLine_Create(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.StaticLine_IsVertical(*args, **kwargs)

    def GetDefaultSize(*args, **kwargs):
        """StaticLine.GetDefaultSize() -> int"""
        return _controls.StaticLine_GetDefaultSize(*args, **kwargs)

    GetDefaultSize = staticmethod(GetDefaultSize)

class StaticLinePtr(StaticLine):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticLine
_controls.StaticLine_swigregister(StaticLinePtr)

def PreStaticLine(*args, **kwargs):
    """PreStaticLine() -> StaticLine"""
    val = _controls.new_PreStaticLine(*args, **kwargs)
    val.thisown = 1
    return val

def StaticLine_GetDefaultSize(*args, **kwargs):
    """StaticLine_GetDefaultSize() -> int"""
    return _controls.StaticLine_GetDefaultSize(*args, **kwargs)

#---------------------------------------------------------------------------

class StaticText(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticText instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticTextNameStr) -> StaticText"""
        newobj = _controls.new_StaticText(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticTextNameStr) -> bool"""
        return _controls.StaticText_Create(*args, **kwargs)


class StaticTextPtr(StaticText):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticText
_controls.StaticText_swigregister(StaticTextPtr)

def PreStaticText(*args, **kwargs):
    """PreStaticText() -> StaticText"""
    val = _controls.new_PreStaticText(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticBitmap(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxBitmap bitmap, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticBitmapNameStr) -> StaticBitmap"""
        newobj = _controls.new_StaticBitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxBitmap bitmap, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    wxString name=wxPyStaticBitmapNameStr) -> bool"""
        return _controls.StaticBitmap_Create(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap() -> wxBitmap"""
        return _controls.StaticBitmap_GetBitmap(*args, **kwargs)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(wxBitmap bitmap)"""
        return _controls.StaticBitmap_SetBitmap(*args, **kwargs)

    def SetIcon(*args, **kwargs):
        """SetIcon(wxIcon icon)"""
        return _controls.StaticBitmap_SetIcon(*args, **kwargs)


class StaticBitmapPtr(StaticBitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBitmap
_controls.StaticBitmap_swigregister(StaticBitmapPtr)

def PreStaticBitmap(*args, **kwargs):
    """PreStaticBitmap() -> StaticBitmap"""
    val = _controls.new_PreStaticBitmap(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ListBox(core.ControlWithItems):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyListBoxNameStr) -> ListBox"""
        newobj = _controls.new_ListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyListBoxNameStr) -> bool"""
        return _controls.ListBox_Create(*args, **kwargs)

    def Insert(*args, **kwargs):
        """Insert(wxString item, int pos, PyObject clientData=None)"""
        return _controls.ListBox_Insert(*args, **kwargs)

    def InsertItems(*args, **kwargs):
        """InsertItems(wxArrayString items, int pos)"""
        return _controls.ListBox_InsertItems(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(wxArrayString items)"""
        return _controls.ListBox_Set(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(int n) -> bool"""
        return _controls.ListBox_IsSelected(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int n, bool select=True)"""
        return _controls.ListBox_SetSelection(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(int n)"""
        return _controls.ListBox_Select(*args, **kwargs)

    def Deselect(*args, **kwargs):
        """Deselect(int n)"""
        return _controls.ListBox_Deselect(*args, **kwargs)

    def DeselectAll(*args, **kwargs):
        """DeselectAll(int itemToLeaveSelected=-1)"""
        return _controls.ListBox_DeselectAll(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(wxString s, bool select=True) -> bool"""
        return _controls.ListBox_SetStringSelection(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections() -> PyObject"""
        return _controls.ListBox_GetSelections(*args, **kwargs)

    def SetFirstItem(*args, **kwargs):
        """SetFirstItem(int n)"""
        return _controls.ListBox_SetFirstItem(*args, **kwargs)

    def SetFirstItemStr(*args, **kwargs):
        """SetFirstItemStr(wxString s)"""
        return _controls.ListBox_SetFirstItemStr(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(int n)"""
        return _controls.ListBox_EnsureVisible(*args, **kwargs)

    def AppendAndEnsureVisible(*args, **kwargs):
        """AppendAndEnsureVisible(wxString s)"""
        return _controls.ListBox_AppendAndEnsureVisible(*args, **kwargs)

    def IsSorted(*args, **kwargs):
        """IsSorted() -> bool"""
        return _controls.ListBox_IsSorted(*args, **kwargs)


class ListBoxPtr(ListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListBox
_controls.ListBox_swigregister(ListBoxPtr)

def PreListBox(*args, **kwargs):
    """PreListBox() -> ListBox"""
    val = _controls.new_PreListBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class CheckListBox(ListBox):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyListBoxNameStr) -> CheckListBox"""
        newobj = _controls.new_CheckListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    int choices=0, wxString choices_array=None, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyListBoxNameStr) -> bool"""
        return _controls.CheckListBox_Create(*args, **kwargs)

    def IsChecked(*args, **kwargs):
        """IsChecked(int index) -> bool"""
        return _controls.CheckListBox_IsChecked(*args, **kwargs)

    def Check(*args, **kwargs):
        """Check(int index, int check=True)"""
        return _controls.CheckListBox_Check(*args, **kwargs)

    def GetItemHeight(*args, **kwargs):
        """GetItemHeight() -> int"""
        return _controls.CheckListBox_GetItemHeight(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point pt) -> int"""
        return _controls.CheckListBox_HitTest(*args, **kwargs)

    def HitTestXY(*args, **kwargs):
        """HitTestXY(int x, int y) -> int"""
        return _controls.CheckListBox_HitTestXY(*args, **kwargs)


class CheckListBoxPtr(CheckListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckListBox
_controls.CheckListBox_swigregister(CheckListBoxPtr)

def PreCheckListBox(*args, **kwargs):
    """PreCheckListBox() -> CheckListBox"""
    val = _controls.new_PreCheckListBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

TE_NO_VSCROLL = _controls.TE_NO_VSCROLL
TE_AUTO_SCROLL = _controls.TE_AUTO_SCROLL
TE_READONLY = _controls.TE_READONLY
TE_MULTILINE = _controls.TE_MULTILINE
TE_PROCESS_TAB = _controls.TE_PROCESS_TAB
TE_LEFT = _controls.TE_LEFT
TE_CENTER = _controls.TE_CENTER
TE_RIGHT = _controls.TE_RIGHT
TE_CENTRE = _controls.TE_CENTRE
TE_RICH = _controls.TE_RICH
TE_PROCESS_ENTER = _controls.TE_PROCESS_ENTER
TE_PASSWORD = _controls.TE_PASSWORD
TE_AUTO_URL = _controls.TE_AUTO_URL
TE_NOHIDESEL = _controls.TE_NOHIDESEL
TE_DONTWRAP = _controls.TE_DONTWRAP
TE_LINEWRAP = _controls.TE_LINEWRAP
TE_WORDWRAP = _controls.TE_WORDWRAP
TE_RICH2 = _controls.TE_RICH2
TEXT_ALIGNMENT_DEFAULT = _controls.TEXT_ALIGNMENT_DEFAULT
TEXT_ALIGNMENT_LEFT = _controls.TEXT_ALIGNMENT_LEFT
TEXT_ALIGNMENT_CENTRE = _controls.TEXT_ALIGNMENT_CENTRE
TEXT_ALIGNMENT_CENTER = _controls.TEXT_ALIGNMENT_CENTER
TEXT_ALIGNMENT_RIGHT = _controls.TEXT_ALIGNMENT_RIGHT
TEXT_ALIGNMENT_JUSTIFIED = _controls.TEXT_ALIGNMENT_JUSTIFIED
TEXT_ATTR_TEXT_COLOUR = _controls.TEXT_ATTR_TEXT_COLOUR
TEXT_ATTR_BACKGROUND_COLOUR = _controls.TEXT_ATTR_BACKGROUND_COLOUR
TEXT_ATTR_FONT_FACE = _controls.TEXT_ATTR_FONT_FACE
TEXT_ATTR_FONT_SIZE = _controls.TEXT_ATTR_FONT_SIZE
TEXT_ATTR_FONT_WEIGHT = _controls.TEXT_ATTR_FONT_WEIGHT
TEXT_ATTR_FONT_ITALIC = _controls.TEXT_ATTR_FONT_ITALIC
TEXT_ATTR_FONT_UNDERLINE = _controls.TEXT_ATTR_FONT_UNDERLINE
TEXT_ATTR_FONT = _controls.TEXT_ATTR_FONT
TEXT_ATTR_ALIGNMENT = _controls.TEXT_ATTR_ALIGNMENT
TEXT_ATTR_LEFT_INDENT = _controls.TEXT_ATTR_LEFT_INDENT
TEXT_ATTR_RIGHT_INDENT = _controls.TEXT_ATTR_RIGHT_INDENT
TEXT_ATTR_TABS = _controls.TEXT_ATTR_TABS
class TextAttr(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__() -> TextAttr
__init__(wxColour colText, wxColour colBack=wxNullColour, wxFont font=wxNullFont, 
    wxTextAttrAlignment alignment=TEXT_ALIGNMENT_DEFAULT) -> TextAttr"""
        newobj = _controls.new_TextAttr(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Init(*args, **kwargs):
        """Init()"""
        return _controls.TextAttr_Init(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour colText)"""
        return _controls.TextAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour colBack)"""
        return _controls.TextAttr_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font, long flags=TEXT_ATTR_FONT)"""
        return _controls.TextAttr_SetFont(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(wxTextAttrAlignment alignment)"""
        return _controls.TextAttr_SetAlignment(*args, **kwargs)

    def SetTabs(*args, **kwargs):
        """SetTabs(wxArrayInt tabs)"""
        return _controls.TextAttr_SetTabs(*args, **kwargs)

    def SetLeftIndent(*args, **kwargs):
        """SetLeftIndent(int indent)"""
        return _controls.TextAttr_SetLeftIndent(*args, **kwargs)

    def SetRightIndent(*args, **kwargs):
        """SetRightIndent(int indent)"""
        return _controls.TextAttr_SetRightIndent(*args, **kwargs)

    def SetFlags(*args, **kwargs):
        """SetFlags(long flags)"""
        return _controls.TextAttr_SetFlags(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour() -> bool"""
        return _controls.TextAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour() -> bool"""
        return _controls.TextAttr_HasBackgroundColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont() -> bool"""
        return _controls.TextAttr_HasFont(*args, **kwargs)

    def HasAlignment(*args, **kwargs):
        """HasAlignment() -> bool"""
        return _controls.TextAttr_HasAlignment(*args, **kwargs)

    def HasTabs(*args, **kwargs):
        """HasTabs() -> bool"""
        return _controls.TextAttr_HasTabs(*args, **kwargs)

    def HasLeftIndent(*args, **kwargs):
        """HasLeftIndent() -> bool"""
        return _controls.TextAttr_HasLeftIndent(*args, **kwargs)

    def HasRightIndent(*args, **kwargs):
        """HasRightIndent() -> bool"""
        return _controls.TextAttr_HasRightIndent(*args, **kwargs)

    def HasFlag(*args, **kwargs):
        """HasFlag(long flag) -> bool"""
        return _controls.TextAttr_HasFlag(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour() -> wxColour"""
        return _controls.TextAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour() -> wxColour"""
        return _controls.TextAttr_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> wxFont"""
        return _controls.TextAttr_GetFont(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> wxTextAttrAlignment"""
        return _controls.TextAttr_GetAlignment(*args, **kwargs)

    def GetTabs(*args, **kwargs):
        """GetTabs() -> wxArrayInt"""
        return _controls.TextAttr_GetTabs(*args, **kwargs)

    def GetLeftIndent(*args, **kwargs):
        """GetLeftIndent() -> long"""
        return _controls.TextAttr_GetLeftIndent(*args, **kwargs)

    def GetRightIndent(*args, **kwargs):
        """GetRightIndent() -> long"""
        return _controls.TextAttr_GetRightIndent(*args, **kwargs)

    def GetFlags(*args, **kwargs):
        """GetFlags() -> long"""
        return _controls.TextAttr_GetFlags(*args, **kwargs)

    def IsDefault(*args, **kwargs):
        """IsDefault() -> bool"""
        return _controls.TextAttr_IsDefault(*args, **kwargs)

    def Combine(*args, **kwargs):
        """TextAttr.Combine(TextAttr attr, TextAttr attrDef, TextCtrl text) -> TextAttr"""
        return _controls.TextAttr_Combine(*args, **kwargs)

    Combine = staticmethod(Combine)

class TextAttrPtr(TextAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextAttr
_controls.TextAttr_swigregister(TextAttrPtr)

def TextAttr_Combine(*args, **kwargs):
    """TextAttr_Combine(TextAttr attr, TextAttr attrDef, TextCtrl text) -> TextAttr"""
    return _controls.TextAttr_Combine(*args, **kwargs)

class TextCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyTextCtrlNameStr) -> TextCtrl"""
        newobj = _controls.new_TextCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyTextCtrlNameStr) -> bool"""
        return _controls.TextCtrl_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> wxString"""
        return _controls.TextCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(wxString value)"""
        return _controls.TextCtrl_SetValue(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange(long from, long to) -> wxString"""
        return _controls.TextCtrl_GetRange(*args, **kwargs)

    def GetLineLength(*args, **kwargs):
        """GetLineLength(long lineNo) -> int"""
        return _controls.TextCtrl_GetLineLength(*args, **kwargs)

    def GetLineText(*args, **kwargs):
        """GetLineText(long lineNo) -> wxString"""
        return _controls.TextCtrl_GetLineText(*args, **kwargs)

    def GetNumberOfLines(*args, **kwargs):
        """GetNumberOfLines() -> int"""
        return _controls.TextCtrl_GetNumberOfLines(*args, **kwargs)

    def IsModified(*args, **kwargs):
        """IsModified() -> bool"""
        return _controls.TextCtrl_IsModified(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """IsEditable() -> bool"""
        return _controls.TextCtrl_IsEditable(*args, **kwargs)

    def IsSingleLine(*args, **kwargs):
        """IsSingleLine() -> bool"""
        return _controls.TextCtrl_IsSingleLine(*args, **kwargs)

    def IsMultiLine(*args, **kwargs):
        """IsMultiLine() -> bool"""
        return _controls.TextCtrl_IsMultiLine(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> (from, to)

If the return values from and to are the same, there is no selection."""
        return _controls.TextCtrl_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection() -> wxString"""
        return _controls.TextCtrl_GetStringSelection(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear()"""
        return _controls.TextCtrl_Clear(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(long from, long to, wxString value)"""
        return _controls.TextCtrl_Replace(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(long from, long to)"""
        return _controls.TextCtrl_Remove(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(wxString file) -> bool"""
        return _controls.TextCtrl_LoadFile(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(wxString file=wxPyEmptyString) -> bool"""
        return _controls.TextCtrl_SaveFile(*args, **kwargs)

    def MarkDirty(*args, **kwargs):
        """MarkDirty()"""
        return _controls.TextCtrl_MarkDirty(*args, **kwargs)

    def DiscardEdits(*args, **kwargs):
        """DiscardEdits()"""
        return _controls.TextCtrl_DiscardEdits(*args, **kwargs)

    def SetMaxLength(*args, **kwargs):
        """SetMaxLength(unsigned long len)"""
        return _controls.TextCtrl_SetMaxLength(*args, **kwargs)

    def WriteText(*args, **kwargs):
        """WriteText(wxString text)"""
        return _controls.TextCtrl_WriteText(*args, **kwargs)

    def AppendText(*args, **kwargs):
        """AppendText(wxString text)"""
        return _controls.TextCtrl_AppendText(*args, **kwargs)

    def EmulateKeyPress(*args, **kwargs):
        """EmulateKeyPress(KeyEvent event) -> bool"""
        return _controls.TextCtrl_EmulateKeyPress(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(long start, long end, TextAttr style) -> bool"""
        return _controls.TextCtrl_SetStyle(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(long position, TextAttr style) -> bool"""
        return _controls.TextCtrl_GetStyle(*args, **kwargs)

    def SetDefaultStyle(*args, **kwargs):
        """SetDefaultStyle(TextAttr style) -> bool"""
        return _controls.TextCtrl_SetDefaultStyle(*args, **kwargs)

    def GetDefaultStyle(*args, **kwargs):
        """GetDefaultStyle() -> TextAttr"""
        return _controls.TextCtrl_GetDefaultStyle(*args, **kwargs)

    def XYToPosition(*args, **kwargs):
        """XYToPosition(long x, long y) -> long"""
        return _controls.TextCtrl_XYToPosition(*args, **kwargs)

    def PositionToXY(*args, **kwargs):
        """PositionToXY(long pos) -> (x, y)"""
        return _controls.TextCtrl_PositionToXY(*args, **kwargs)

    def ShowPosition(*args, **kwargs):
        """ShowPosition(long pos)"""
        return _controls.TextCtrl_ShowPosition(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy()"""
        return _controls.TextCtrl_Copy(*args, **kwargs)

    def Cut(*args, **kwargs):
        """Cut()"""
        return _controls.TextCtrl_Cut(*args, **kwargs)

    def Paste(*args, **kwargs):
        """Paste()"""
        return _controls.TextCtrl_Paste(*args, **kwargs)

    def CanCopy(*args, **kwargs):
        """CanCopy() -> bool"""
        return _controls.TextCtrl_CanCopy(*args, **kwargs)

    def CanCut(*args, **kwargs):
        """CanCut() -> bool"""
        return _controls.TextCtrl_CanCut(*args, **kwargs)

    def CanPaste(*args, **kwargs):
        """CanPaste() -> bool"""
        return _controls.TextCtrl_CanPaste(*args, **kwargs)

    def Undo(*args, **kwargs):
        """Undo()"""
        return _controls.TextCtrl_Undo(*args, **kwargs)

    def Redo(*args, **kwargs):
        """Redo()"""
        return _controls.TextCtrl_Redo(*args, **kwargs)

    def CanUndo(*args, **kwargs):
        """CanUndo() -> bool"""
        return _controls.TextCtrl_CanUndo(*args, **kwargs)

    def CanRedo(*args, **kwargs):
        """CanRedo() -> bool"""
        return _controls.TextCtrl_CanRedo(*args, **kwargs)

    def SetInsertionPoint(*args, **kwargs):
        """SetInsertionPoint(long pos)"""
        return _controls.TextCtrl_SetInsertionPoint(*args, **kwargs)

    def SetInsertionPointEnd(*args, **kwargs):
        """SetInsertionPointEnd()"""
        return _controls.TextCtrl_SetInsertionPointEnd(*args, **kwargs)

    def GetInsertionPoint(*args, **kwargs):
        """GetInsertionPoint() -> long"""
        return _controls.TextCtrl_GetInsertionPoint(*args, **kwargs)

    def GetLastPosition(*args, **kwargs):
        """GetLastPosition() -> long"""
        return _controls.TextCtrl_GetLastPosition(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(long from, long to)"""
        return _controls.TextCtrl_SetSelection(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll()"""
        return _controls.TextCtrl_SelectAll(*args, **kwargs)

    def SetEditable(*args, **kwargs):
        """SetEditable(bool editable)"""
        return _controls.TextCtrl_SetEditable(*args, **kwargs)

    def write(*args, **kwargs):
        """write(wxString text)"""
        return _controls.TextCtrl_write(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(long from, long to) -> wxString"""
        return _controls.TextCtrl_GetString(*args, **kwargs)


class TextCtrlPtr(TextCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextCtrl
_controls.TextCtrl_swigregister(TextCtrlPtr)

def PreTextCtrl(*args, **kwargs):
    """PreTextCtrl() -> TextCtrl"""
    val = _controls.new_PreTextCtrl(*args, **kwargs)
    val.thisown = 1
    return val

wxEVT_COMMAND_TEXT_UPDATED = _controls.wxEVT_COMMAND_TEXT_UPDATED
wxEVT_COMMAND_TEXT_ENTER = _controls.wxEVT_COMMAND_TEXT_ENTER
wxEVT_COMMAND_TEXT_URL = _controls.wxEVT_COMMAND_TEXT_URL
wxEVT_COMMAND_TEXT_MAXLEN = _controls.wxEVT_COMMAND_TEXT_MAXLEN
class TextUrlEvent(core.CommandEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextUrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int winid, MouseEvent evtMouse, long start, long end) -> TextUrlEvent"""
        newobj = _controls.new_TextUrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMouseEvent(*args, **kwargs):
        """GetMouseEvent() -> MouseEvent"""
        return _controls.TextUrlEvent_GetMouseEvent(*args, **kwargs)

    def GetURLStart(*args, **kwargs):
        """GetURLStart() -> long"""
        return _controls.TextUrlEvent_GetURLStart(*args, **kwargs)

    def GetURLEnd(*args, **kwargs):
        """GetURLEnd() -> long"""
        return _controls.TextUrlEvent_GetURLEnd(*args, **kwargs)


class TextUrlEventPtr(TextUrlEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextUrlEvent
_controls.TextUrlEvent_swigregister(TextUrlEventPtr)

EVT_TEXT        = wx.PyEventBinder( wxEVT_COMMAND_TEXT_UPDATED, 1)
EVT_TEXT_ENTER  = wx.PyEventBinder( wxEVT_COMMAND_TEXT_ENTER, 1)
EVT_TEXT_URL    = wx.PyEventBinder( wxEVT_COMMAND_TEXT_URL, 1) 
EVT_TEXT_MAXLEN = wx.PyEventBinder( wxEVT_COMMAND_TEXT_MAXLEN, 1)

#---------------------------------------------------------------------------

class ScrollBar(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=SB_HORIZONTAL, 
    Validator validator=DefaultValidator, wxString name=wxPyScrollBarNameStr) -> ScrollBar"""
        newobj = _controls.new_ScrollBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=SB_HORIZONTAL, 
    Validator validator=DefaultValidator, wxString name=wxPyScrollBarNameStr) -> bool"""
        return _controls.ScrollBar_Create(*args, **kwargs)

    def GetThumbPosition(*args, **kwargs):
        """GetThumbPosition() -> int"""
        return _controls.ScrollBar_GetThumbPosition(*args, **kwargs)

    def GetThumbSize(*args, **kwargs):
        """GetThumbSize() -> int"""
        return _controls.ScrollBar_GetThumbSize(*args, **kwargs)

    GetThumbLength = GetThumbSize 
    def GetPageSize(*args, **kwargs):
        """GetPageSize() -> int"""
        return _controls.ScrollBar_GetPageSize(*args, **kwargs)

    def GetRange(*args, **kwargs):
        """GetRange() -> int"""
        return _controls.ScrollBar_GetRange(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.ScrollBar_IsVertical(*args, **kwargs)

    def SetThumbPosition(*args, **kwargs):
        """SetThumbPosition(int viewStart)"""
        return _controls.ScrollBar_SetThumbPosition(*args, **kwargs)

    def SetScrollbar(*args, **kwargs):
        """SetScrollbar(int position, int thumbSize, int range, int pageSize, 
    bool refresh=True)"""
        return _controls.ScrollBar_SetScrollbar(*args, **kwargs)


class ScrollBarPtr(ScrollBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrollBar
_controls.ScrollBar_swigregister(ScrollBarPtr)

def PreScrollBar(*args, **kwargs):
    """PreScrollBar() -> ScrollBar"""
    val = _controls.new_PreScrollBar(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

SP_HORIZONTAL = _controls.SP_HORIZONTAL
SP_VERTICAL = _controls.SP_VERTICAL
SP_ARROW_KEYS = _controls.SP_ARROW_KEYS
SP_WRAP = _controls.SP_WRAP
class SpinButton(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=SP_HORIZONTAL, 
    wxString name=wxPySPIN_BUTTON_NAME) -> SpinButton"""
        newobj = _controls.new_SpinButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=SP_HORIZONTAL, 
    wxString name=wxPySPIN_BUTTON_NAME) -> bool"""
        return _controls.SpinButton_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> int"""
        return _controls.SpinButton_GetValue(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin() -> int"""
        return _controls.SpinButton_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax() -> int"""
        return _controls.SpinButton_GetMax(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(int val)"""
        return _controls.SpinButton_SetValue(*args, **kwargs)

    def SetMin(*args, **kwargs):
        """SetMin(int minVal)"""
        return _controls.SpinButton_SetMin(*args, **kwargs)

    def SetMax(*args, **kwargs):
        """SetMax(int maxVal)"""
        return _controls.SpinButton_SetMax(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(int minVal, int maxVal)"""
        return _controls.SpinButton_SetRange(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.SpinButton_IsVertical(*args, **kwargs)


class SpinButtonPtr(SpinButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinButton
_controls.SpinButton_swigregister(SpinButtonPtr)

def PreSpinButton(*args, **kwargs):
    """PreSpinButton() -> SpinButton"""
    val = _controls.new_PreSpinButton(*args, **kwargs)
    val.thisown = 1
    return val

class SpinCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=SP_ARROW_KEYS, int min=0, 
    int max=100, int initial=0, wxString name=wxPySpinCtrlNameStr) -> SpinCtrl"""
        newobj = _controls.new_SpinCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, wxString value=wxPyEmptyString, 
    Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=SP_ARROW_KEYS, int min=0, 
    int max=100, int initial=0, wxString name=wxPySpinCtrlNameStr) -> bool"""
        return _controls.SpinCtrl_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> int"""
        return _controls.SpinCtrl_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(int value)"""
        return _controls.SpinCtrl_SetValue(*args, **kwargs)

    def SetValueString(*args, **kwargs):
        """SetValueString(wxString text)"""
        return _controls.SpinCtrl_SetValueString(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(int minVal, int maxVal)"""
        return _controls.SpinCtrl_SetRange(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin() -> int"""
        return _controls.SpinCtrl_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax() -> int"""
        return _controls.SpinCtrl_GetMax(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(long from, long to)"""
        return _controls.SpinCtrl_SetSelection(*args, **kwargs)


class SpinCtrlPtr(SpinCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinCtrl
_controls.SpinCtrl_swigregister(SpinCtrlPtr)

def PreSpinCtrl(*args, **kwargs):
    """PreSpinCtrl() -> SpinCtrl"""
    val = _controls.new_PreSpinCtrl(*args, **kwargs)
    val.thisown = 1
    return val

wxEVT_COMMAND_SPINCTRL_UPDATED = _controls.wxEVT_COMMAND_SPINCTRL_UPDATED
EVT_SPINCTRL = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)

#---------------------------------------------------------------------------

class RadioBox(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point point=DefaultPosition, 
    Size size=DefaultSize, int choices=0, 
    wxString choices_array=None, int majorDimension=0, 
    long style=RA_HORIZONTAL, Validator validator=DefaultValidator, 
    wxString name=wxPyRadioBoxNameStr) -> RadioBox"""
        newobj = _controls.new_RadioBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point point=DefaultPosition, 
    Size size=DefaultSize, int choices=0, 
    wxString choices_array=None, int majorDimension=0, 
    long style=RA_HORIZONTAL, Validator validator=DefaultValidator, 
    wxString name=wxPyRadioBoxNameStr) -> bool"""
        return _controls.RadioBox_Create(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int n)"""
        return _controls.RadioBox_SetSelection(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> int"""
        return _controls.RadioBox_GetSelection(*args, **kwargs)

    def GetStringSelection(*args, **kwargs):
        """GetStringSelection() -> wxString"""
        return _controls.RadioBox_GetStringSelection(*args, **kwargs)

    def SetStringSelection(*args, **kwargs):
        """SetStringSelection(wxString s) -> bool"""
        return _controls.RadioBox_SetStringSelection(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> int"""
        return _controls.RadioBox_GetCount(*args, **kwargs)

    def FindString(*args, **kwargs):
        """FindString(wxString s) -> int"""
        return _controls.RadioBox_FindString(*args, **kwargs)

    def GetString(*args, **kwargs):
        """GetString(int n) -> wxString"""
        return _controls.RadioBox_GetString(*args, **kwargs)

    def SetString(*args, **kwargs):
        """SetString(int n, wxString label)"""
        return _controls.RadioBox_SetString(*args, **kwargs)

    GetItemLabel = GetString 
    SetItemLabel = SetString 
    def EnableItem(*args, **kwargs):
        """EnableItem(int n, bool enable=True)"""
        return _controls.RadioBox_EnableItem(*args, **kwargs)

    def ShowItem(*args, **kwargs):
        """ShowItem(int n, bool show=True)"""
        return _controls.RadioBox_ShowItem(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount() -> int"""
        return _controls.RadioBox_GetColumnCount(*args, **kwargs)

    def GetRowCount(*args, **kwargs):
        """GetRowCount() -> int"""
        return _controls.RadioBox_GetRowCount(*args, **kwargs)

    def GetNextItem(*args, **kwargs):
        """GetNextItem(int item, wxDirection dir, long style) -> int"""
        return _controls.RadioBox_GetNextItem(*args, **kwargs)


class RadioBoxPtr(RadioBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioBox
_controls.RadioBox_swigregister(RadioBoxPtr)

def PreRadioBox(*args, **kwargs):
    """PreRadioBox() -> RadioBox"""
    val = _controls.new_PreRadioBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class RadioButton(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyRadioButtonNameStr) -> RadioButton"""
        newobj = _controls.new_RadioButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyRadioButtonNameStr) -> bool"""
        return _controls.RadioButton_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> bool"""
        return _controls.RadioButton_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(bool value)"""
        return _controls.RadioButton_SetValue(*args, **kwargs)


class RadioButtonPtr(RadioButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioButton
_controls.RadioButton_swigregister(RadioButtonPtr)

def PreRadioButton(*args, **kwargs):
    """PreRadioButton() -> RadioButton"""
    val = _controls.new_PreRadioButton(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Slider(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSlider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, int value, int minValue, int maxValue, 
    Point point=DefaultPosition, Size size=DefaultSize, 
    long style=SL_HORIZONTAL, Validator validator=DefaultValidator, 
    wxString name=wxPySliderNameStr) -> Slider"""
        newobj = _controls.new_Slider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, int value, int minValue, int maxValue, 
    Point point=DefaultPosition, Size size=DefaultSize, 
    long style=SL_HORIZONTAL, Validator validator=DefaultValidator, 
    wxString name=wxPySliderNameStr) -> bool"""
        return _controls.Slider_Create(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> int"""
        return _controls.Slider_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(int value)"""
        return _controls.Slider_SetValue(*args, **kwargs)

    def SetRange(*args, **kwargs):
        """SetRange(int minValue, int maxValue)"""
        return _controls.Slider_SetRange(*args, **kwargs)

    def GetMin(*args, **kwargs):
        """GetMin() -> int"""
        return _controls.Slider_GetMin(*args, **kwargs)

    def GetMax(*args, **kwargs):
        """GetMax() -> int"""
        return _controls.Slider_GetMax(*args, **kwargs)

    def SetMin(*args, **kwargs):
        """SetMin(int minValue)"""
        return _controls.Slider_SetMin(*args, **kwargs)

    def SetMax(*args, **kwargs):
        """SetMax(int maxValue)"""
        return _controls.Slider_SetMax(*args, **kwargs)

    def SetLineSize(*args, **kwargs):
        """SetLineSize(int lineSize)"""
        return _controls.Slider_SetLineSize(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(int pageSize)"""
        return _controls.Slider_SetPageSize(*args, **kwargs)

    def GetLineSize(*args, **kwargs):
        """GetLineSize() -> int"""
        return _controls.Slider_GetLineSize(*args, **kwargs)

    def GetPageSize(*args, **kwargs):
        """GetPageSize() -> int"""
        return _controls.Slider_GetPageSize(*args, **kwargs)

    def SetThumbLength(*args, **kwargs):
        """SetThumbLength(int lenPixels)"""
        return _controls.Slider_SetThumbLength(*args, **kwargs)

    def GetThumbLength(*args, **kwargs):
        """GetThumbLength() -> int"""
        return _controls.Slider_GetThumbLength(*args, **kwargs)

    def SetTickFreq(*args, **kwargs):
        """SetTickFreq(int n, int pos)"""
        return _controls.Slider_SetTickFreq(*args, **kwargs)

    def GetTickFreq(*args, **kwargs):
        """GetTickFreq() -> int"""
        return _controls.Slider_GetTickFreq(*args, **kwargs)

    def ClearTicks(*args, **kwargs):
        """ClearTicks()"""
        return _controls.Slider_ClearTicks(*args, **kwargs)

    def SetTick(*args, **kwargs):
        """SetTick(int tickPos)"""
        return _controls.Slider_SetTick(*args, **kwargs)

    def ClearSel(*args, **kwargs):
        """ClearSel()"""
        return _controls.Slider_ClearSel(*args, **kwargs)

    def GetSelEnd(*args, **kwargs):
        """GetSelEnd() -> int"""
        return _controls.Slider_GetSelEnd(*args, **kwargs)

    def GetSelStart(*args, **kwargs):
        """GetSelStart() -> int"""
        return _controls.Slider_GetSelStart(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int min, int max)"""
        return _controls.Slider_SetSelection(*args, **kwargs)


class SliderPtr(Slider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Slider
_controls.Slider_swigregister(SliderPtr)

def PreSlider(*args, **kwargs):
    """PreSlider() -> Slider"""
    val = _controls.new_PreSlider(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

wxEVT_COMMAND_TOGGLEBUTTON_CLICKED = _controls.wxEVT_COMMAND_TOGGLEBUTTON_CLICKED
EVT_TOGGLEBUTTON = wx.PyEventBinder( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 1)

class ToggleButton(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToggleButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyToggleButtonNameStr) -> ToggleButton"""
        newobj = _controls.new_ToggleButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, wxString label, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0, 
    Validator validator=DefaultValidator, 
    wxString name=wxPyToggleButtonNameStr) -> bool"""
        return _controls.ToggleButton_Create(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(bool value)"""
        return _controls.ToggleButton_SetValue(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue() -> bool"""
        return _controls.ToggleButton_GetValue(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(wxString label)"""
        return _controls.ToggleButton_SetLabel(*args, **kwargs)


class ToggleButtonPtr(ToggleButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToggleButton
_controls.ToggleButton_swigregister(ToggleButtonPtr)

def PreToggleButton(*args, **kwargs):
    """PreToggleButton() -> ToggleButton"""
    val = _controls.new_PreToggleButton(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class BookCtrl(core.Control):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetPageCount(*args, **kwargs):
        """GetPageCount() -> size_t"""
        return _controls.BookCtrl_GetPageCount(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(size_t n) -> Window"""
        return _controls.BookCtrl_GetPage(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> int"""
        return _controls.BookCtrl_GetSelection(*args, **kwargs)

    def SetPageText(*args, **kwargs):
        """SetPageText(size_t n, wxString strText) -> bool"""
        return _controls.BookCtrl_SetPageText(*args, **kwargs)

    def GetPageText(*args, **kwargs):
        """GetPageText(size_t n) -> wxString"""
        return _controls.BookCtrl_GetPageText(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(wxImageList imageList)"""
        return _controls.BookCtrl_SetImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(wxImageList imageList)"""
        val = _controls.BookCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def GetImageList(*args, **kwargs):
        """GetImageList() -> wxImageList"""
        return _controls.BookCtrl_GetImageList(*args, **kwargs)

    def GetPageImage(*args, **kwargs):
        """GetPageImage(size_t n) -> int"""
        return _controls.BookCtrl_GetPageImage(*args, **kwargs)

    def SetPageImage(*args, **kwargs):
        """SetPageImage(size_t n, int imageId) -> bool"""
        return _controls.BookCtrl_SetPageImage(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(Size size)"""
        return _controls.BookCtrl_SetPageSize(*args, **kwargs)

    def CalcSizeFromPage(*args, **kwargs):
        """CalcSizeFromPage(Size sizePage) -> Size"""
        return _controls.BookCtrl_CalcSizeFromPage(*args, **kwargs)

    def DeletePage(*args, **kwargs):
        """DeletePage(size_t n) -> bool"""
        return _controls.BookCtrl_DeletePage(*args, **kwargs)

    def RemovePage(*args, **kwargs):
        """RemovePage(size_t n) -> bool"""
        return _controls.BookCtrl_RemovePage(*args, **kwargs)

    def DeleteAllPages(*args, **kwargs):
        """DeleteAllPages() -> bool"""
        return _controls.BookCtrl_DeleteAllPages(*args, **kwargs)

    def AddPage(*args, **kwargs):
        """AddPage(Window page, wxString text, bool select=False, int imageId=-1) -> bool"""
        return _controls.BookCtrl_AddPage(*args, **kwargs)

    def InsertPage(*args, **kwargs):
        """InsertPage(size_t n, Window page, wxString text, bool select=False, 
    int imageId=-1) -> bool"""
        return _controls.BookCtrl_InsertPage(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(size_t n) -> int"""
        return _controls.BookCtrl_SetSelection(*args, **kwargs)

    def AdvanceSelection(*args, **kwargs):
        """AdvanceSelection(bool forward=True)"""
        return _controls.BookCtrl_AdvanceSelection(*args, **kwargs)


class BookCtrlPtr(BookCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrl
_controls.BookCtrl_swigregister(BookCtrlPtr)

class BookCtrlEvent(core.NotifyEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
    int nOldSel=-1) -> BookCtrlEvent"""
        newobj = _controls.new_BookCtrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSelection(*args, **kwargs):
        """GetSelection() -> int"""
        return _controls.BookCtrlEvent_GetSelection(*args, **kwargs)

    def SetSelection(*args, **kwargs):
        """SetSelection(int nSel)"""
        return _controls.BookCtrlEvent_SetSelection(*args, **kwargs)

    def GetOldSelection(*args, **kwargs):
        """GetOldSelection() -> int"""
        return _controls.BookCtrlEvent_GetOldSelection(*args, **kwargs)

    def SetOldSelection(*args, **kwargs):
        """SetOldSelection(int nOldSel)"""
        return _controls.BookCtrlEvent_SetOldSelection(*args, **kwargs)


class BookCtrlEventPtr(BookCtrlEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlEvent
_controls.BookCtrlEvent_swigregister(BookCtrlEventPtr)

#---------------------------------------------------------------------------

NB_FIXEDWIDTH = _controls.NB_FIXEDWIDTH
NB_TOP = _controls.NB_TOP
NB_LEFT = _controls.NB_LEFT
NB_RIGHT = _controls.NB_RIGHT
NB_BOTTOM = _controls.NB_BOTTOM
NB_MULTILINE = _controls.NB_MULTILINE
NB_HITTEST_NOWHERE = _controls.NB_HITTEST_NOWHERE
NB_HITTEST_ONICON = _controls.NB_HITTEST_ONICON
NB_HITTEST_ONLABEL = _controls.NB_HITTEST_ONLABEL
NB_HITTEST_ONITEM = _controls.NB_HITTEST_ONITEM
class Notebook(BookCtrl):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, wxString name=wxPyNOTEBOOK_NAME) -> Notebook"""
        newobj = _controls.new_Notebook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, wxString name=wxPyNOTEBOOK_NAME) -> bool"""
        return _controls.Notebook_Create(*args, **kwargs)

    def GetRowCount(*args, **kwargs):
        """GetRowCount() -> int"""
        return _controls.Notebook_GetRowCount(*args, **kwargs)

    def SetPadding(*args, **kwargs):
        """SetPadding(Size padding)"""
        return _controls.Notebook_SetPadding(*args, **kwargs)

    def SetTabSize(*args, **kwargs):
        """SetTabSize(Size sz)"""
        return _controls.Notebook_SetTabSize(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point pt) -> (tab, where)

Returns the tab which is hit, and flags indicating where using wxNB_HITTEST_ flags."""
        return _controls.Notebook_HitTest(*args, **kwargs)

    def CalcSizeFromPage(*args, **kwargs):
        """CalcSizeFromPage(Size sizePage) -> Size"""
        return _controls.Notebook_CalcSizeFromPage(*args, **kwargs)


class NotebookPtr(Notebook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Notebook
_controls.Notebook_swigregister(NotebookPtr)

def PreNotebook(*args, **kwargs):
    """PreNotebook() -> Notebook"""
    val = _controls.new_PreNotebook(*args, **kwargs)
    val.thisown = 1
    return val

class NotebookEvent(BookCtrlEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
    int nOldSel=-1) -> NotebookEvent"""
        newobj = _controls.new_NotebookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class NotebookEventPtr(NotebookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotebookEvent
_controls.NotebookEvent_swigregister(NotebookEventPtr)

wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = _controls.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = _controls.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
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

LB_DEFAULT = _controls.LB_DEFAULT
LB_TOP = _controls.LB_TOP
LB_BOTTOM = _controls.LB_BOTTOM
LB_LEFT = _controls.LB_LEFT
LB_RIGHT = _controls.LB_RIGHT
LB_ALIGN_MASK = _controls.LB_ALIGN_MASK
class Listbook(BookCtrl):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, wxString name=wxPyEmptyString) -> Listbook"""
        newobj = _controls.new_Listbook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, wxString name=wxPyEmptyString) -> bool"""
        return _controls.Listbook_Create(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.Listbook_IsVertical(*args, **kwargs)


class ListbookPtr(Listbook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Listbook
_controls.Listbook_swigregister(ListbookPtr)

def PreListbook(*args, **kwargs):
    """PreListbook() -> Listbook"""
    val = _controls.new_PreListbook(*args, **kwargs)
    val.thisown = 1
    return val

class ListbookEvent(BookCtrlEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0, int nSel=-1, 
    int nOldSel=-1) -> ListbookEvent"""
        newobj = _controls.new_ListbookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ListbookEventPtr(ListbookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListbookEvent
_controls.ListbookEvent_swigregister(ListbookEventPtr)

wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = _controls.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = _controls.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING
EVT_LISTBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, 1 )
EVT_LISTBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, 1 )

#---------------------------------------------------------------------------

class BookCtrlSizer(core.Sizer):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(BookCtrl nb) -> BookCtrlSizer"""
        newobj = _controls.new_BookCtrlSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def RecalcSizes(*args, **kwargs):
        """RecalcSizes()"""
        return _controls.BookCtrlSizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """CalcMin() -> Size"""
        return _controls.BookCtrlSizer_CalcMin(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl() -> BookCtrl"""
        return _controls.BookCtrlSizer_GetControl(*args, **kwargs)


class BookCtrlSizerPtr(BookCtrlSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlSizer
_controls.BookCtrlSizer_swigregister(BookCtrlSizerPtr)

class NotebookSizer(core.Sizer):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Notebook nb) -> NotebookSizer"""
        newobj = _controls.new_NotebookSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def RecalcSizes(*args, **kwargs):
        """RecalcSizes()"""
        return _controls.NotebookSizer_RecalcSizes(*args, **kwargs)

    def CalcMin(*args, **kwargs):
        """CalcMin() -> Size"""
        return _controls.NotebookSizer_CalcMin(*args, **kwargs)

    def GetNotebook(*args, **kwargs):
        """GetNotebook() -> Notebook"""
        return _controls.NotebookSizer_GetNotebook(*args, **kwargs)


class NotebookSizerPtr(NotebookSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotebookSizer
_controls.NotebookSizer_swigregister(NotebookSizerPtr)

#---------------------------------------------------------------------------

TOOL_STYLE_BUTTON = _controls.TOOL_STYLE_BUTTON
TOOL_STYLE_SEPARATOR = _controls.TOOL_STYLE_SEPARATOR
TOOL_STYLE_CONTROL = _controls.TOOL_STYLE_CONTROL
TB_HORIZONTAL = _controls.TB_HORIZONTAL
TB_VERTICAL = _controls.TB_VERTICAL
TB_3DBUTTONS = _controls.TB_3DBUTTONS
TB_FLAT = _controls.TB_FLAT
TB_DOCKABLE = _controls.TB_DOCKABLE
TB_NOICONS = _controls.TB_NOICONS
TB_TEXT = _controls.TB_TEXT
TB_NODIVIDER = _controls.TB_NODIVIDER
TB_NOALIGN = _controls.TB_NOALIGN
TB_HORZ_LAYOUT = _controls.TB_HORZ_LAYOUT
TB_HORZ_TEXT = _controls.TB_HORZ_TEXT
class ToolBarToolBase(core.Object):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarToolBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def GetId(*args, **kwargs):
        """GetId() -> int"""
        return _controls.ToolBarToolBase_GetId(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl() -> Control"""
        return _controls.ToolBarToolBase_GetControl(*args, **kwargs)

    def GetToolBar(*args, **kwargs):
        """GetToolBar() -> ToolBarBase"""
        return _controls.ToolBarToolBase_GetToolBar(*args, **kwargs)

    def IsButton(*args, **kwargs):
        """IsButton() -> int"""
        return _controls.ToolBarToolBase_IsButton(*args, **kwargs)

    def IsControl(*args, **kwargs):
        """IsControl() -> int"""
        return _controls.ToolBarToolBase_IsControl(*args, **kwargs)

    def IsSeparator(*args, **kwargs):
        """IsSeparator() -> int"""
        return _controls.ToolBarToolBase_IsSeparator(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> int"""
        return _controls.ToolBarToolBase_GetStyle(*args, **kwargs)

    def GetKind(*args, **kwargs):
        """GetKind() -> wxItemKind"""
        return _controls.ToolBarToolBase_GetKind(*args, **kwargs)

    def IsEnabled(*args, **kwargs):
        """IsEnabled() -> bool"""
        return _controls.ToolBarToolBase_IsEnabled(*args, **kwargs)

    def IsToggled(*args, **kwargs):
        """IsToggled() -> bool"""
        return _controls.ToolBarToolBase_IsToggled(*args, **kwargs)

    def CanBeToggled(*args, **kwargs):
        """CanBeToggled() -> bool"""
        return _controls.ToolBarToolBase_CanBeToggled(*args, **kwargs)

    def GetNormalBitmap(*args, **kwargs):
        """GetNormalBitmap() -> wxBitmap"""
        return _controls.ToolBarToolBase_GetNormalBitmap(*args, **kwargs)

    def GetDisabledBitmap(*args, **kwargs):
        """GetDisabledBitmap() -> wxBitmap"""
        return _controls.ToolBarToolBase_GetDisabledBitmap(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap() -> wxBitmap"""
        return _controls.ToolBarToolBase_GetBitmap(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel() -> wxString"""
        return _controls.ToolBarToolBase_GetLabel(*args, **kwargs)

    def GetShortHelp(*args, **kwargs):
        """GetShortHelp() -> wxString"""
        return _controls.ToolBarToolBase_GetShortHelp(*args, **kwargs)

    def GetLongHelp(*args, **kwargs):
        """GetLongHelp() -> wxString"""
        return _controls.ToolBarToolBase_GetLongHelp(*args, **kwargs)

    def Enable(*args, **kwargs):
        """Enable(bool enable) -> bool"""
        return _controls.ToolBarToolBase_Enable(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle()"""
        return _controls.ToolBarToolBase_Toggle(*args, **kwargs)

    def SetToggle(*args, **kwargs):
        """SetToggle(bool toggle) -> bool"""
        return _controls.ToolBarToolBase_SetToggle(*args, **kwargs)

    def SetShortHelp(*args, **kwargs):
        """SetShortHelp(wxString help) -> bool"""
        return _controls.ToolBarToolBase_SetShortHelp(*args, **kwargs)

    def SetLongHelp(*args, **kwargs):
        """SetLongHelp(wxString help) -> bool"""
        return _controls.ToolBarToolBase_SetLongHelp(*args, **kwargs)

    def SetNormalBitmap(*args, **kwargs):
        """SetNormalBitmap(wxBitmap bmp)"""
        return _controls.ToolBarToolBase_SetNormalBitmap(*args, **kwargs)

    def SetDisabledBitmap(*args, **kwargs):
        """SetDisabledBitmap(wxBitmap bmp)"""
        return _controls.ToolBarToolBase_SetDisabledBitmap(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(wxString label)"""
        return _controls.ToolBarToolBase_SetLabel(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach()"""
        return _controls.ToolBarToolBase_Detach(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(ToolBarBase tbar)"""
        return _controls.ToolBarToolBase_Attach(*args, **kwargs)

    def GetClientData(*args, **kwargs):
        """GetClientData() -> PyObject"""
        return _controls.ToolBarToolBase_GetClientData(*args, **kwargs)

    def SetClientData(*args, **kwargs):
        """SetClientData(PyObject clientData)"""
        return _controls.ToolBarToolBase_SetClientData(*args, **kwargs)

    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap


class ToolBarToolBasePtr(ToolBarToolBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarToolBase
_controls.ToolBarToolBase_swigregister(ToolBarToolBasePtr)

class ToolBarBase(core.Control):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def DoAddTool(*args, **kwargs):
        """DoAddTool(int id, wxString label, wxBitmap bitmap, wxBitmap bmpDisabled=wxNullBitmap, 
    wxItemKind kind=ITEM_NORMAL, 
    wxString shortHelp=wxPyEmptyString, 
    wxString longHelp=wxPyEmptyString, PyObject clientData=None) -> ToolBarToolBase"""
        return _controls.ToolBarBase_DoAddTool(*args, **kwargs)

    def DoInsertTool(*args, **kwargs):
        """DoInsertTool(size_t pos, int id, wxString label, wxBitmap bitmap, 
    wxBitmap bmpDisabled=wxNullBitmap, wxItemKind kind=ITEM_NORMAL, 
    wxString shortHelp=wxPyEmptyString, 
    wxString longHelp=wxPyEmptyString, 
    PyObject clientData=None) -> ToolBarToolBase"""
        return _controls.ToolBarBase_DoInsertTool(*args, **kwargs)

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

    def AddControl(*args, **kwargs):
        """AddControl(Control control) -> ToolBarToolBase"""
        return _controls.ToolBarBase_AddControl(*args, **kwargs)

    def InsertControl(*args, **kwargs):
        """InsertControl(size_t pos, Control control) -> ToolBarToolBase"""
        return _controls.ToolBarBase_InsertControl(*args, **kwargs)

    def FindControl(*args, **kwargs):
        """FindControl(int id) -> Control"""
        return _controls.ToolBarBase_FindControl(*args, **kwargs)

    def AddSeparator(*args, **kwargs):
        """AddSeparator() -> ToolBarToolBase"""
        return _controls.ToolBarBase_AddSeparator(*args, **kwargs)

    def InsertSeparator(*args, **kwargs):
        """InsertSeparator(size_t pos) -> ToolBarToolBase"""
        return _controls.ToolBarBase_InsertSeparator(*args, **kwargs)

    def RemoveTool(*args, **kwargs):
        """RemoveTool(int id) -> ToolBarToolBase"""
        return _controls.ToolBarBase_RemoveTool(*args, **kwargs)

    def DeleteToolByPos(*args, **kwargs):
        """DeleteToolByPos(size_t pos) -> bool"""
        return _controls.ToolBarBase_DeleteToolByPos(*args, **kwargs)

    def DeleteTool(*args, **kwargs):
        """DeleteTool(int id) -> bool"""
        return _controls.ToolBarBase_DeleteTool(*args, **kwargs)

    def ClearTools(*args, **kwargs):
        """ClearTools()"""
        return _controls.ToolBarBase_ClearTools(*args, **kwargs)

    def Realize(*args, **kwargs):
        """Realize() -> bool"""
        return _controls.ToolBarBase_Realize(*args, **kwargs)

    def EnableTool(*args, **kwargs):
        """EnableTool(int id, bool enable)"""
        return _controls.ToolBarBase_EnableTool(*args, **kwargs)

    def ToggleTool(*args, **kwargs):
        """ToggleTool(int id, bool toggle)"""
        return _controls.ToolBarBase_ToggleTool(*args, **kwargs)

    def SetToggle(*args, **kwargs):
        """SetToggle(int id, bool toggle)"""
        return _controls.ToolBarBase_SetToggle(*args, **kwargs)

    def GetToolClientData(*args, **kwargs):
        """GetToolClientData(int id) -> PyObject"""
        return _controls.ToolBarBase_GetToolClientData(*args, **kwargs)

    def SetToolClientData(*args, **kwargs):
        """SetToolClientData(int id, PyObject clientData)"""
        return _controls.ToolBarBase_SetToolClientData(*args, **kwargs)

    def GetToolPos(*args, **kwargs):
        """GetToolPos(int id) -> int"""
        return _controls.ToolBarBase_GetToolPos(*args, **kwargs)

    def GetToolState(*args, **kwargs):
        """GetToolState(int id) -> bool"""
        return _controls.ToolBarBase_GetToolState(*args, **kwargs)

    def GetToolEnabled(*args, **kwargs):
        """GetToolEnabled(int id) -> bool"""
        return _controls.ToolBarBase_GetToolEnabled(*args, **kwargs)

    def SetToolShortHelp(*args, **kwargs):
        """SetToolShortHelp(int id, wxString helpString)"""
        return _controls.ToolBarBase_SetToolShortHelp(*args, **kwargs)

    def GetToolShortHelp(*args, **kwargs):
        """GetToolShortHelp(int id) -> wxString"""
        return _controls.ToolBarBase_GetToolShortHelp(*args, **kwargs)

    def SetToolLongHelp(*args, **kwargs):
        """SetToolLongHelp(int id, wxString helpString)"""
        return _controls.ToolBarBase_SetToolLongHelp(*args, **kwargs)

    def GetToolLongHelp(*args, **kwargs):
        """GetToolLongHelp(int id) -> wxString"""
        return _controls.ToolBarBase_GetToolLongHelp(*args, **kwargs)

    def SetMarginsXY(*args, **kwargs):
        """SetMarginsXY(int x, int y)"""
        return _controls.ToolBarBase_SetMarginsXY(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(Size size)"""
        return _controls.ToolBarBase_SetMargins(*args, **kwargs)

    def SetToolPacking(*args, **kwargs):
        """SetToolPacking(int packing)"""
        return _controls.ToolBarBase_SetToolPacking(*args, **kwargs)

    def SetToolSeparation(*args, **kwargs):
        """SetToolSeparation(int separation)"""
        return _controls.ToolBarBase_SetToolSeparation(*args, **kwargs)

    def GetToolMargins(*args, **kwargs):
        """GetToolMargins() -> Size"""
        return _controls.ToolBarBase_GetToolMargins(*args, **kwargs)

    def GetMargins(*args, **kwargs):
        """GetMargins() -> Size"""
        return _controls.ToolBarBase_GetMargins(*args, **kwargs)

    def GetToolPacking(*args, **kwargs):
        """GetToolPacking() -> int"""
        return _controls.ToolBarBase_GetToolPacking(*args, **kwargs)

    def GetToolSeparation(*args, **kwargs):
        """GetToolSeparation() -> int"""
        return _controls.ToolBarBase_GetToolSeparation(*args, **kwargs)

    def SetRows(*args, **kwargs):
        """SetRows(int nRows)"""
        return _controls.ToolBarBase_SetRows(*args, **kwargs)

    def SetMaxRowsCols(*args, **kwargs):
        """SetMaxRowsCols(int rows, int cols)"""
        return _controls.ToolBarBase_SetMaxRowsCols(*args, **kwargs)

    def GetMaxRows(*args, **kwargs):
        """GetMaxRows() -> int"""
        return _controls.ToolBarBase_GetMaxRows(*args, **kwargs)

    def GetMaxCols(*args, **kwargs):
        """GetMaxCols() -> int"""
        return _controls.ToolBarBase_GetMaxCols(*args, **kwargs)

    def SetToolBitmapSize(*args, **kwargs):
        """SetToolBitmapSize(Size size)"""
        return _controls.ToolBarBase_SetToolBitmapSize(*args, **kwargs)

    def GetToolBitmapSize(*args, **kwargs):
        """GetToolBitmapSize() -> Size"""
        return _controls.ToolBarBase_GetToolBitmapSize(*args, **kwargs)

    def GetToolSize(*args, **kwargs):
        """GetToolSize() -> Size"""
        return _controls.ToolBarBase_GetToolSize(*args, **kwargs)

    def FindToolForPosition(*args, **kwargs):
        """FindToolForPosition(int x, int y) -> ToolBarToolBase"""
        return _controls.ToolBarBase_FindToolForPosition(*args, **kwargs)

    def IsVertical(*args, **kwargs):
        """IsVertical() -> bool"""
        return _controls.ToolBarBase_IsVertical(*args, **kwargs)


class ToolBarBasePtr(ToolBarBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarBase
_controls.ToolBarBase_swigregister(ToolBarBasePtr)

class ToolBar(ToolBarBase):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=wxNO_BORDER|wxTB_HORIZONTAL, 
    wxString name=wxPyToolBarNameStr) -> ToolBar"""
        newobj = _controls.new_ToolBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=wxNO_BORDER|wxTB_HORIZONTAL, 
    wxString name=wxPyToolBarNameStr) -> bool"""
        return _controls.ToolBar_Create(*args, **kwargs)

    def FindToolForPosition(*args, **kwargs):
        """FindToolForPosition(int x, int y) -> ToolBarToolBase"""
        return _controls.ToolBar_FindToolForPosition(*args, **kwargs)


class ToolBarPtr(ToolBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBar
_controls.ToolBar_swigregister(ToolBarPtr)

def PreToolBar(*args, **kwargs):
    """PreToolBar() -> ToolBar"""
    val = _controls.new_PreToolBar(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

LC_VRULES = _controls.LC_VRULES
LC_HRULES = _controls.LC_HRULES
LC_ICON = _controls.LC_ICON
LC_SMALL_ICON = _controls.LC_SMALL_ICON
LC_LIST = _controls.LC_LIST
LC_REPORT = _controls.LC_REPORT
LC_ALIGN_TOP = _controls.LC_ALIGN_TOP
LC_ALIGN_LEFT = _controls.LC_ALIGN_LEFT
LC_AUTOARRANGE = _controls.LC_AUTOARRANGE
LC_VIRTUAL = _controls.LC_VIRTUAL
LC_EDIT_LABELS = _controls.LC_EDIT_LABELS
LC_NO_HEADER = _controls.LC_NO_HEADER
LC_NO_SORT_HEADER = _controls.LC_NO_SORT_HEADER
LC_SINGLE_SEL = _controls.LC_SINGLE_SEL
LC_SORT_ASCENDING = _controls.LC_SORT_ASCENDING
LC_SORT_DESCENDING = _controls.LC_SORT_DESCENDING
LC_MASK_TYPE = _controls.LC_MASK_TYPE
LC_MASK_ALIGN = _controls.LC_MASK_ALIGN
LC_MASK_SORT = _controls.LC_MASK_SORT
LIST_MASK_STATE = _controls.LIST_MASK_STATE
LIST_MASK_TEXT = _controls.LIST_MASK_TEXT
LIST_MASK_IMAGE = _controls.LIST_MASK_IMAGE
LIST_MASK_DATA = _controls.LIST_MASK_DATA
LIST_SET_ITEM = _controls.LIST_SET_ITEM
LIST_MASK_WIDTH = _controls.LIST_MASK_WIDTH
LIST_MASK_FORMAT = _controls.LIST_MASK_FORMAT
LIST_STATE_DONTCARE = _controls.LIST_STATE_DONTCARE
LIST_STATE_DROPHILITED = _controls.LIST_STATE_DROPHILITED
LIST_STATE_FOCUSED = _controls.LIST_STATE_FOCUSED
LIST_STATE_SELECTED = _controls.LIST_STATE_SELECTED
LIST_STATE_CUT = _controls.LIST_STATE_CUT
LIST_STATE_DISABLED = _controls.LIST_STATE_DISABLED
LIST_STATE_FILTERED = _controls.LIST_STATE_FILTERED
LIST_STATE_INUSE = _controls.LIST_STATE_INUSE
LIST_STATE_PICKED = _controls.LIST_STATE_PICKED
LIST_STATE_SOURCE = _controls.LIST_STATE_SOURCE
LIST_HITTEST_ABOVE = _controls.LIST_HITTEST_ABOVE
LIST_HITTEST_BELOW = _controls.LIST_HITTEST_BELOW
LIST_HITTEST_NOWHERE = _controls.LIST_HITTEST_NOWHERE
LIST_HITTEST_ONITEMICON = _controls.LIST_HITTEST_ONITEMICON
LIST_HITTEST_ONITEMLABEL = _controls.LIST_HITTEST_ONITEMLABEL
LIST_HITTEST_ONITEMRIGHT = _controls.LIST_HITTEST_ONITEMRIGHT
LIST_HITTEST_ONITEMSTATEICON = _controls.LIST_HITTEST_ONITEMSTATEICON
LIST_HITTEST_TOLEFT = _controls.LIST_HITTEST_TOLEFT
LIST_HITTEST_TORIGHT = _controls.LIST_HITTEST_TORIGHT
LIST_HITTEST_ONITEM = _controls.LIST_HITTEST_ONITEM
LIST_NEXT_ABOVE = _controls.LIST_NEXT_ABOVE
LIST_NEXT_ALL = _controls.LIST_NEXT_ALL
LIST_NEXT_BELOW = _controls.LIST_NEXT_BELOW
LIST_NEXT_LEFT = _controls.LIST_NEXT_LEFT
LIST_NEXT_RIGHT = _controls.LIST_NEXT_RIGHT
LIST_ALIGN_DEFAULT = _controls.LIST_ALIGN_DEFAULT
LIST_ALIGN_LEFT = _controls.LIST_ALIGN_LEFT
LIST_ALIGN_TOP = _controls.LIST_ALIGN_TOP
LIST_ALIGN_SNAP_TO_GRID = _controls.LIST_ALIGN_SNAP_TO_GRID
LIST_FORMAT_LEFT = _controls.LIST_FORMAT_LEFT
LIST_FORMAT_RIGHT = _controls.LIST_FORMAT_RIGHT
LIST_FORMAT_CENTRE = _controls.LIST_FORMAT_CENTRE
LIST_FORMAT_CENTER = _controls.LIST_FORMAT_CENTER
LIST_AUTOSIZE = _controls.LIST_AUTOSIZE
LIST_AUTOSIZE_USEHEADER = _controls.LIST_AUTOSIZE_USEHEADER
LIST_RECT_BOUNDS = _controls.LIST_RECT_BOUNDS
LIST_RECT_ICON = _controls.LIST_RECT_ICON
LIST_RECT_LABEL = _controls.LIST_RECT_LABEL
LIST_FIND_UP = _controls.LIST_FIND_UP
LIST_FIND_DOWN = _controls.LIST_FIND_DOWN
LIST_FIND_LEFT = _controls.LIST_FIND_LEFT
LIST_FIND_RIGHT = _controls.LIST_FIND_RIGHT
#---------------------------------------------------------------------------

class ListItemAttr(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItemAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxColour colText=wxNullColour, wxColour colBack=wxNullColour, 
    wxFont font=wxNullFont) -> ListItemAttr"""
        newobj = _controls.new_ListItemAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour colText)"""
        return _controls.ListItemAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour colBack)"""
        return _controls.ListItemAttr_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font)"""
        return _controls.ListItemAttr_SetFont(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour() -> bool"""
        return _controls.ListItemAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour() -> bool"""
        return _controls.ListItemAttr_HasBackgroundColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont() -> bool"""
        return _controls.ListItemAttr_HasFont(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour() -> wxColour"""
        return _controls.ListItemAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour() -> wxColour"""
        return _controls.ListItemAttr_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> wxFont"""
        return _controls.ListItemAttr_GetFont(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy()"""
        return _controls.ListItemAttr_Destroy(*args, **kwargs)


class ListItemAttrPtr(ListItemAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItemAttr
_controls.ListItemAttr_swigregister(ListItemAttrPtr)

#---------------------------------------------------------------------------

class ListItem(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> ListItem"""
        newobj = _controls.new_ListItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_ListItem):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Clear(*args, **kwargs):
        """Clear()"""
        return _controls.ListItem_Clear(*args, **kwargs)

    def ClearAttributes(*args, **kwargs):
        """ClearAttributes()"""
        return _controls.ListItem_ClearAttributes(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """SetMask(long mask)"""
        return _controls.ListItem_SetMask(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(long id)"""
        return _controls.ListItem_SetId(*args, **kwargs)

    def SetColumn(*args, **kwargs):
        """SetColumn(int col)"""
        return _controls.ListItem_SetColumn(*args, **kwargs)

    def SetState(*args, **kwargs):
        """SetState(long state)"""
        return _controls.ListItem_SetState(*args, **kwargs)

    def SetStateMask(*args, **kwargs):
        """SetStateMask(long stateMask)"""
        return _controls.ListItem_SetStateMask(*args, **kwargs)

    def SetText(*args, **kwargs):
        """SetText(wxString text)"""
        return _controls.ListItem_SetText(*args, **kwargs)

    def SetImage(*args, **kwargs):
        """SetImage(int image)"""
        return _controls.ListItem_SetImage(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(long data)"""
        return _controls.ListItem_SetData(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(int width)"""
        return _controls.ListItem_SetWidth(*args, **kwargs)

    def SetAlign(*args, **kwargs):
        """SetAlign(wxListColumnFormat align)"""
        return _controls.ListItem_SetAlign(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour colText)"""
        return _controls.ListItem_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour colBack)"""
        return _controls.ListItem_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font)"""
        return _controls.ListItem_SetFont(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """GetMask() -> long"""
        return _controls.ListItem_GetMask(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId() -> long"""
        return _controls.ListItem_GetId(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn() -> int"""
        return _controls.ListItem_GetColumn(*args, **kwargs)

    def GetState(*args, **kwargs):
        """GetState() -> long"""
        return _controls.ListItem_GetState(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText() -> wxString"""
        return _controls.ListItem_GetText(*args, **kwargs)

    def GetImage(*args, **kwargs):
        """GetImage() -> int"""
        return _controls.ListItem_GetImage(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData() -> long"""
        return _controls.ListItem_GetData(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int"""
        return _controls.ListItem_GetWidth(*args, **kwargs)

    def GetAlign(*args, **kwargs):
        """GetAlign() -> wxListColumnFormat"""
        return _controls.ListItem_GetAlign(*args, **kwargs)

    def GetAttributes(*args, **kwargs):
        """GetAttributes() -> ListItemAttr"""
        return _controls.ListItem_GetAttributes(*args, **kwargs)

    def HasAttributes(*args, **kwargs):
        """HasAttributes() -> bool"""
        return _controls.ListItem_HasAttributes(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour() -> wxColour"""
        return _controls.ListItem_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour() -> wxColour"""
        return _controls.ListItem_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> wxFont"""
        return _controls.ListItem_GetFont(*args, **kwargs)

    m_mask = property(_controls.ListItem_m_mask_get, _controls.ListItem_m_mask_set)
    m_itemId = property(_controls.ListItem_m_itemId_get, _controls.ListItem_m_itemId_set)
    m_col = property(_controls.ListItem_m_col_get, _controls.ListItem_m_col_set)
    m_state = property(_controls.ListItem_m_state_get, _controls.ListItem_m_state_set)
    m_stateMask = property(_controls.ListItem_m_stateMask_get, _controls.ListItem_m_stateMask_set)
    m_text = property(_controls.ListItem_m_text_get, _controls.ListItem_m_text_set)
    m_image = property(_controls.ListItem_m_image_get, _controls.ListItem_m_image_set)
    m_data = property(_controls.ListItem_m_data_get, _controls.ListItem_m_data_set)
    m_format = property(_controls.ListItem_m_format_get, _controls.ListItem_m_format_set)
    m_width = property(_controls.ListItem_m_width_get, _controls.ListItem_m_width_set)

class ListItemPtr(ListItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItem
_controls.ListItem_swigregister(ListItemPtr)

#---------------------------------------------------------------------------

class ListEvent(core.NotifyEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0) -> ListEvent"""
        newobj = _controls.new_ListEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    m_code = property(_controls.ListEvent_m_code_get, _controls.ListEvent_m_code_set)
    m_oldItemIndex = property(_controls.ListEvent_m_oldItemIndex_get, _controls.ListEvent_m_oldItemIndex_set)
    m_itemIndex = property(_controls.ListEvent_m_itemIndex_get, _controls.ListEvent_m_itemIndex_set)
    m_col = property(_controls.ListEvent_m_col_get, _controls.ListEvent_m_col_set)
    m_pointDrag = property(_controls.ListEvent_m_pointDrag_get, _controls.ListEvent_m_pointDrag_set)
    m_item = property(_controls.ListEvent_m_item_get)
    def GetKeyCode(*args, **kwargs):
        """GetKeyCode() -> int"""
        return _controls.ListEvent_GetKeyCode(*args, **kwargs)

    GetCode = GetKeyCode 
    def GetIndex(*args, **kwargs):
        """GetIndex() -> long"""
        return _controls.ListEvent_GetIndex(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn() -> int"""
        return _controls.ListEvent_GetColumn(*args, **kwargs)

    def GetPoint(*args, **kwargs):
        """GetPoint() -> Point"""
        return _controls.ListEvent_GetPoint(*args, **kwargs)

    GetPostiion = GetPoint 
    def GetLabel(*args, **kwargs):
        """GetLabel() -> wxString"""
        return _controls.ListEvent_GetLabel(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText() -> wxString"""
        return _controls.ListEvent_GetText(*args, **kwargs)

    def GetImage(*args, **kwargs):
        """GetImage() -> int"""
        return _controls.ListEvent_GetImage(*args, **kwargs)

    def GetData(*args, **kwargs):
        """GetData() -> long"""
        return _controls.ListEvent_GetData(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """GetMask() -> long"""
        return _controls.ListEvent_GetMask(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """GetItem() -> ListItem"""
        return _controls.ListEvent_GetItem(*args, **kwargs)

    def GetCacheFrom(*args, **kwargs):
        """GetCacheFrom() -> long"""
        return _controls.ListEvent_GetCacheFrom(*args, **kwargs)

    def GetCacheTo(*args, **kwargs):
        """GetCacheTo() -> long"""
        return _controls.ListEvent_GetCacheTo(*args, **kwargs)

    def IsEditCancelled(*args, **kwargs):
        """IsEditCancelled() -> bool"""
        return _controls.ListEvent_IsEditCancelled(*args, **kwargs)

    def SetEditCanceled(*args, **kwargs):
        """SetEditCanceled(bool editCancelled)"""
        return _controls.ListEvent_SetEditCanceled(*args, **kwargs)


class ListEventPtr(ListEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListEvent
_controls.ListEvent_swigregister(ListEventPtr)

wxEVT_COMMAND_LIST_BEGIN_DRAG = _controls.wxEVT_COMMAND_LIST_BEGIN_DRAG
wxEVT_COMMAND_LIST_BEGIN_RDRAG = _controls.wxEVT_COMMAND_LIST_BEGIN_RDRAG
wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT = _controls.wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT
wxEVT_COMMAND_LIST_END_LABEL_EDIT = _controls.wxEVT_COMMAND_LIST_END_LABEL_EDIT
wxEVT_COMMAND_LIST_DELETE_ITEM = _controls.wxEVT_COMMAND_LIST_DELETE_ITEM
wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS = _controls.wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS
wxEVT_COMMAND_LIST_GET_INFO = _controls.wxEVT_COMMAND_LIST_GET_INFO
wxEVT_COMMAND_LIST_SET_INFO = _controls.wxEVT_COMMAND_LIST_SET_INFO
wxEVT_COMMAND_LIST_ITEM_SELECTED = _controls.wxEVT_COMMAND_LIST_ITEM_SELECTED
wxEVT_COMMAND_LIST_ITEM_DESELECTED = _controls.wxEVT_COMMAND_LIST_ITEM_DESELECTED
wxEVT_COMMAND_LIST_KEY_DOWN = _controls.wxEVT_COMMAND_LIST_KEY_DOWN
wxEVT_COMMAND_LIST_INSERT_ITEM = _controls.wxEVT_COMMAND_LIST_INSERT_ITEM
wxEVT_COMMAND_LIST_COL_CLICK = _controls.wxEVT_COMMAND_LIST_COL_CLICK
wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK = _controls.wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK
wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK = _controls.wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK
wxEVT_COMMAND_LIST_ITEM_ACTIVATED = _controls.wxEVT_COMMAND_LIST_ITEM_ACTIVATED
wxEVT_COMMAND_LIST_CACHE_HINT = _controls.wxEVT_COMMAND_LIST_CACHE_HINT
wxEVT_COMMAND_LIST_COL_RIGHT_CLICK = _controls.wxEVT_COMMAND_LIST_COL_RIGHT_CLICK
wxEVT_COMMAND_LIST_COL_BEGIN_DRAG = _controls.wxEVT_COMMAND_LIST_COL_BEGIN_DRAG
wxEVT_COMMAND_LIST_COL_DRAGGING = _controls.wxEVT_COMMAND_LIST_COL_DRAGGING
wxEVT_COMMAND_LIST_COL_END_DRAG = _controls.wxEVT_COMMAND_LIST_COL_END_DRAG
wxEVT_COMMAND_LIST_ITEM_FOCUSED = _controls.wxEVT_COMMAND_LIST_ITEM_FOCUSED
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

#---------------------------------------------------------------------------

class ListCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=LC_ICON, 
    Validator validator=DefaultValidator, wxString name=wxPyListCtrlNameStr) -> ListCtrl"""
        newobj = _controls.new_ListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, ListCtrl)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=LC_ICON, 
    Validator validator=DefaultValidator, wxString name=wxPyListCtrlNameStr) -> bool"""
        return _controls.ListCtrl_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _controls.ListCtrl__setCallbackInfo(*args, **kwargs)

    def SetForegroundColour(*args, **kwargs):
        """SetForegroundColour(wxColour col) -> bool"""
        return _controls.ListCtrl_SetForegroundColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour col) -> bool"""
        return _controls.ListCtrl_SetBackgroundColour(*args, **kwargs)

    def GetColumn(*args, **kwargs):
        """GetColumn(int col) -> ListItem"""
        val = _controls.ListCtrl_GetColumn(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val

    def SetColumn(*args, **kwargs):
        """SetColumn(int col, ListItem item) -> bool"""
        return _controls.ListCtrl_SetColumn(*args, **kwargs)

    def GetColumnWidth(*args, **kwargs):
        """GetColumnWidth(int col) -> int"""
        return _controls.ListCtrl_GetColumnWidth(*args, **kwargs)

    def SetColumnWidth(*args, **kwargs):
        """SetColumnWidth(int col, int width) -> bool"""
        return _controls.ListCtrl_SetColumnWidth(*args, **kwargs)

    def GetCountPerPage(*args, **kwargs):
        """GetCountPerPage() -> int"""
        return _controls.ListCtrl_GetCountPerPage(*args, **kwargs)

    def GetViewRect(*args, **kwargs):
        """GetViewRect() -> Rect"""
        return _controls.ListCtrl_GetViewRect(*args, **kwargs)

    def GetItem(*args, **kwargs):
        """GetItem(long itemId, int col=0) -> ListItem"""
        val = _controls.ListCtrl_GetItem(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val

    def SetItem(*args, **kwargs):
        """SetItem(ListItem info) -> bool"""
        return _controls.ListCtrl_SetItem(*args, **kwargs)

    def SetStringItem(*args, **kwargs):
        """SetStringItem(long index, int col, wxString label, int imageId=-1) -> long"""
        return _controls.ListCtrl_SetStringItem(*args, **kwargs)

    def GetItemState(*args, **kwargs):
        """GetItemState(long item, long stateMask) -> int"""
        return _controls.ListCtrl_GetItemState(*args, **kwargs)

    def SetItemState(*args, **kwargs):
        """SetItemState(long item, long state, long stateMask) -> bool"""
        return _controls.ListCtrl_SetItemState(*args, **kwargs)

    def SetItemImage(*args, **kwargs):
        """SetItemImage(long item, int image, int selImage) -> bool"""
        return _controls.ListCtrl_SetItemImage(*args, **kwargs)

    def GetItemText(*args, **kwargs):
        """GetItemText(long item) -> wxString"""
        return _controls.ListCtrl_GetItemText(*args, **kwargs)

    def SetItemText(*args, **kwargs):
        """SetItemText(long item, wxString str)"""
        return _controls.ListCtrl_SetItemText(*args, **kwargs)

    def GetItemData(*args, **kwargs):
        """GetItemData(long item) -> long"""
        return _controls.ListCtrl_GetItemData(*args, **kwargs)

    def SetItemData(*args, **kwargs):
        """SetItemData(long item, long data) -> bool"""
        return _controls.ListCtrl_SetItemData(*args, **kwargs)

    def GetItemPosition(*args, **kwargs):
        """GetItemPosition(long item) -> Point"""
        return _controls.ListCtrl_GetItemPosition(*args, **kwargs)

    def GetItemRect(*args, **kwargs):
        """GetItemRect(long item, int code=LIST_RECT_BOUNDS) -> Rect"""
        return _controls.ListCtrl_GetItemRect(*args, **kwargs)

    def SetItemPosition(*args, **kwargs):
        """SetItemPosition(long item, Point pos) -> bool"""
        return _controls.ListCtrl_SetItemPosition(*args, **kwargs)

    def GetItemCount(*args, **kwargs):
        """GetItemCount() -> int"""
        return _controls.ListCtrl_GetItemCount(*args, **kwargs)

    def GetColumnCount(*args, **kwargs):
        """GetColumnCount() -> int"""
        return _controls.ListCtrl_GetColumnCount(*args, **kwargs)

    def GetItemSpacing(*args, **kwargs):
        """GetItemSpacing() -> Size"""
        return _controls.ListCtrl_GetItemSpacing(*args, **kwargs)

    def SetItemSpacing(*args, **kwargs):
        """SetItemSpacing(int spacing, bool isSmall=False)"""
        return _controls.ListCtrl_SetItemSpacing(*args, **kwargs)

    def GetSelectedItemCount(*args, **kwargs):
        """GetSelectedItemCount() -> int"""
        return _controls.ListCtrl_GetSelectedItemCount(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour() -> wxColour"""
        return _controls.ListCtrl_GetTextColour(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour col)"""
        return _controls.ListCtrl_SetTextColour(*args, **kwargs)

    def GetTopItem(*args, **kwargs):
        """GetTopItem() -> long"""
        return _controls.ListCtrl_GetTopItem(*args, **kwargs)

    def SetSingleStyle(*args, **kwargs):
        """SetSingleStyle(long style, bool add=True)"""
        return _controls.ListCtrl_SetSingleStyle(*args, **kwargs)

    def SetWindowStyleFlag(*args, **kwargs):
        """SetWindowStyleFlag(long style)"""
        return _controls.ListCtrl_SetWindowStyleFlag(*args, **kwargs)

    def GetNextItem(*args, **kwargs):
        """GetNextItem(long item, int geometry=LIST_NEXT_ALL, int state=LIST_STATE_DONTCARE) -> long"""
        return _controls.ListCtrl_GetNextItem(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList(int which) -> wxImageList"""
        return _controls.ListCtrl_GetImageList(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(wxImageList imageList, int which)"""
        return _controls.ListCtrl_SetImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(wxImageList imageList, int which)"""
        val = _controls.ListCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def IsVirtual(*args, **kwargs):
        """IsVirtual() -> bool"""
        return _controls.ListCtrl_IsVirtual(*args, **kwargs)

    def RefreshItem(*args, **kwargs):
        """RefreshItem(long item)"""
        return _controls.ListCtrl_RefreshItem(*args, **kwargs)

    def RefreshItems(*args, **kwargs):
        """RefreshItems(long itemFrom, long itemTo)"""
        return _controls.ListCtrl_RefreshItems(*args, **kwargs)

    def Arrange(*args, **kwargs):
        """Arrange(int flag=LIST_ALIGN_DEFAULT) -> bool"""
        return _controls.ListCtrl_Arrange(*args, **kwargs)

    def DeleteItem(*args, **kwargs):
        """DeleteItem(long item) -> bool"""
        return _controls.ListCtrl_DeleteItem(*args, **kwargs)

    def DeleteAllItems(*args, **kwargs):
        """DeleteAllItems() -> bool"""
        return _controls.ListCtrl_DeleteAllItems(*args, **kwargs)

    def DeleteColumn(*args, **kwargs):
        """DeleteColumn(int col) -> bool"""
        return _controls.ListCtrl_DeleteColumn(*args, **kwargs)

    def DeleteAllColumns(*args, **kwargs):
        """DeleteAllColumns() -> bool"""
        return _controls.ListCtrl_DeleteAllColumns(*args, **kwargs)

    def ClearAll(*args, **kwargs):
        """ClearAll()"""
        return _controls.ListCtrl_ClearAll(*args, **kwargs)

    def EditLabel(*args, **kwargs):
        """EditLabel(long item)"""
        return _controls.ListCtrl_EditLabel(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(long item) -> bool"""
        return _controls.ListCtrl_EnsureVisible(*args, **kwargs)

    def FindItem(*args, **kwargs):
        """FindItem(long start, wxString str, bool partial=False) -> long"""
        return _controls.ListCtrl_FindItem(*args, **kwargs)

    def FindItemData(*args, **kwargs):
        """FindItemData(long start, long data) -> long"""
        return _controls.ListCtrl_FindItemData(*args, **kwargs)

    def FindItemAtPos(*args, **kwargs):
        """FindItemAtPos(long start, Point pt, int direction) -> long"""
        return _controls.ListCtrl_FindItemAtPos(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point point) -> (item, where)

Determines which item (if any) is at the specified point,
giving details in the second return value (see wxLIST_HITTEST_... flags.)"""
        return _controls.ListCtrl_HitTest(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(ListItem info) -> long"""
        return _controls.ListCtrl_InsertItem(*args, **kwargs)

    def InsertStringItem(*args, **kwargs):
        """InsertStringItem(long index, wxString label) -> long"""
        return _controls.ListCtrl_InsertStringItem(*args, **kwargs)

    def InsertImageItem(*args, **kwargs):
        """InsertImageItem(long index, int imageIndex) -> long"""
        return _controls.ListCtrl_InsertImageItem(*args, **kwargs)

    def InsertImageStringItem(*args, **kwargs):
        """InsertImageStringItem(long index, wxString label, int imageIndex) -> long"""
        return _controls.ListCtrl_InsertImageStringItem(*args, **kwargs)

    def InsertColumnInfo(*args, **kwargs):
        """InsertColumnInfo(long col, ListItem info) -> long"""
        return _controls.ListCtrl_InsertColumnInfo(*args, **kwargs)

    def InsertColumn(*args, **kwargs):
        """InsertColumn(long col, wxString heading, int format=LIST_FORMAT_LEFT, 
    int width=-1) -> long"""
        return _controls.ListCtrl_InsertColumn(*args, **kwargs)

    def SetItemCount(*args, **kwargs):
        """SetItemCount(long count)"""
        return _controls.ListCtrl_SetItemCount(*args, **kwargs)

    def ScrollList(*args, **kwargs):
        """ScrollList(int dx, int dy) -> bool"""
        return _controls.ListCtrl_ScrollList(*args, **kwargs)

    def SetItemTextColour(*args, **kwargs):
        """SetItemTextColour(long item, wxColour col)"""
        return _controls.ListCtrl_SetItemTextColour(*args, **kwargs)

    def GetItemTextColour(*args, **kwargs):
        """GetItemTextColour(long item) -> wxColour"""
        return _controls.ListCtrl_GetItemTextColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(long item, wxColour col)"""
        return _controls.ListCtrl_SetItemBackgroundColour(*args, **kwargs)

    def GetItemBackgroundColour(*args, **kwargs):
        """GetItemBackgroundColour(long item) -> wxColour"""
        return _controls.ListCtrl_GetItemBackgroundColour(*args, **kwargs)

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
        """SortItems(PyObject func) -> bool"""
        return _controls.ListCtrl_SortItems(*args, **kwargs)

    def GetMainWindow(*args, **kwargs):
        """GetMainWindow() -> Window"""
        return _controls.ListCtrl_GetMainWindow(*args, **kwargs)


class ListCtrlPtr(ListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListCtrl
_controls.ListCtrl_swigregister(ListCtrlPtr)

def PreListCtrl(*args, **kwargs):
    """PreListCtrl() -> ListCtrl"""
    val = _controls.new_PreListCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ListView(ListCtrl):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListView instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=LC_REPORT, 
    Validator validator=DefaultValidator, wxString name=wxPyListCtrlNameStr) -> ListView"""
        newobj = _controls.new_ListView(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=LC_REPORT, 
    Validator validator=DefaultValidator, wxString name=wxPyListCtrlNameStr) -> bool"""
        return _controls.ListView_Create(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(long n, bool on=True)"""
        return _controls.ListView_Select(*args, **kwargs)

    def Focus(*args, **kwargs):
        """Focus(long index)"""
        return _controls.ListView_Focus(*args, **kwargs)

    def GetFocusedItem(*args, **kwargs):
        """GetFocusedItem() -> long"""
        return _controls.ListView_GetFocusedItem(*args, **kwargs)

    def GetNextSelected(*args, **kwargs):
        """GetNextSelected(long item) -> long"""
        return _controls.ListView_GetNextSelected(*args, **kwargs)

    def GetFirstSelected(*args, **kwargs):
        """GetFirstSelected() -> long"""
        return _controls.ListView_GetFirstSelected(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(long index) -> bool"""
        return _controls.ListView_IsSelected(*args, **kwargs)

    def SetColumnImage(*args, **kwargs):
        """SetColumnImage(int col, int image)"""
        return _controls.ListView_SetColumnImage(*args, **kwargs)

    def ClearColumnImage(*args, **kwargs):
        """ClearColumnImage(int col)"""
        return _controls.ListView_ClearColumnImage(*args, **kwargs)


class ListViewPtr(ListView):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListView
_controls.ListView_swigregister(ListViewPtr)

def PreListView(*args, **kwargs):
    """PreListView() -> ListView"""
    val = _controls.new_PreListView(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

TR_NO_BUTTONS = _controls.TR_NO_BUTTONS
TR_HAS_BUTTONS = _controls.TR_HAS_BUTTONS
TR_NO_LINES = _controls.TR_NO_LINES
TR_LINES_AT_ROOT = _controls.TR_LINES_AT_ROOT
TR_SINGLE = _controls.TR_SINGLE
TR_MULTIPLE = _controls.TR_MULTIPLE
TR_EXTENDED = _controls.TR_EXTENDED
TR_HAS_VARIABLE_ROW_HEIGHT = _controls.TR_HAS_VARIABLE_ROW_HEIGHT
TR_EDIT_LABELS = _controls.TR_EDIT_LABELS
TR_HIDE_ROOT = _controls.TR_HIDE_ROOT
TR_ROW_LINES = _controls.TR_ROW_LINES
TR_FULL_ROW_HIGHLIGHT = _controls.TR_FULL_ROW_HIGHLIGHT
TR_DEFAULT_STYLE = _controls.TR_DEFAULT_STYLE
TR_TWIST_BUTTONS = _controls.TR_TWIST_BUTTONS
TR_MAC_BUTTONS = _controls.TR_MAC_BUTTONS
TR_AQUA_BUTTONS = _controls.TR_AQUA_BUTTONS
TreeItemIcon_Normal = _controls.TreeItemIcon_Normal
TreeItemIcon_Selected = _controls.TreeItemIcon_Selected
TreeItemIcon_Expanded = _controls.TreeItemIcon_Expanded
TreeItemIcon_SelectedExpanded = _controls.TreeItemIcon_SelectedExpanded
TreeItemIcon_Max = _controls.TreeItemIcon_Max
TREE_HITTEST_ABOVE = _controls.TREE_HITTEST_ABOVE
TREE_HITTEST_BELOW = _controls.TREE_HITTEST_BELOW
TREE_HITTEST_NOWHERE = _controls.TREE_HITTEST_NOWHERE
TREE_HITTEST_ONITEMBUTTON = _controls.TREE_HITTEST_ONITEMBUTTON
TREE_HITTEST_ONITEMICON = _controls.TREE_HITTEST_ONITEMICON
TREE_HITTEST_ONITEMINDENT = _controls.TREE_HITTEST_ONITEMINDENT
TREE_HITTEST_ONITEMLABEL = _controls.TREE_HITTEST_ONITEMLABEL
TREE_HITTEST_ONITEMRIGHT = _controls.TREE_HITTEST_ONITEMRIGHT
TREE_HITTEST_ONITEMSTATEICON = _controls.TREE_HITTEST_ONITEMSTATEICON
TREE_HITTEST_TOLEFT = _controls.TREE_HITTEST_TOLEFT
TREE_HITTEST_TORIGHT = _controls.TREE_HITTEST_TORIGHT
TREE_HITTEST_ONITEMUPPERPART = _controls.TREE_HITTEST_ONITEMUPPERPART
TREE_HITTEST_ONITEMLOWERPART = _controls.TREE_HITTEST_ONITEMLOWERPART
TREE_HITTEST_ONITEM = _controls.TREE_HITTEST_ONITEM
#---------------------------------------------------------------------------

class TreeItemId(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeItemId instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> TreeItemId"""
        newobj = _controls.new_TreeItemId(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_TreeItemId):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def IsOk(*args, **kwargs):
        """IsOk() -> bool"""
        return _controls.TreeItemId_IsOk(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(TreeItemId other) -> bool"""
        return _controls.TreeItemId___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(TreeItemId other) -> bool"""
        return _controls.TreeItemId___ne__(*args, **kwargs)

    m_pItem = property(_controls.TreeItemId_m_pItem_get, _controls.TreeItemId_m_pItem_set)
    Ok = IsOk
    def __nonzero__(self): return self.IsOk() 

class TreeItemIdPtr(TreeItemId):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeItemId
_controls.TreeItemId_swigregister(TreeItemIdPtr)

class TreeItemData(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeItemData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(PyObject obj=None) -> TreeItemData"""
        newobj = _controls.new_TreeItemData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetData(*args, **kwargs):
        """GetData() -> PyObject"""
        return _controls.TreeItemData_GetData(*args, **kwargs)

    def SetData(*args, **kwargs):
        """SetData(PyObject obj)"""
        return _controls.TreeItemData_SetData(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId() -> TreeItemId"""
        return _controls.TreeItemData_GetId(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(TreeItemId id)"""
        return _controls.TreeItemData_SetId(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy()"""
        return _controls.TreeItemData_Destroy(*args, **kwargs)


class TreeItemDataPtr(TreeItemData):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeItemData
_controls.TreeItemData_swigregister(TreeItemDataPtr)

#---------------------------------------------------------------------------

wxEVT_COMMAND_TREE_BEGIN_DRAG = _controls.wxEVT_COMMAND_TREE_BEGIN_DRAG
wxEVT_COMMAND_TREE_BEGIN_RDRAG = _controls.wxEVT_COMMAND_TREE_BEGIN_RDRAG
wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT = _controls.wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT
wxEVT_COMMAND_TREE_END_LABEL_EDIT = _controls.wxEVT_COMMAND_TREE_END_LABEL_EDIT
wxEVT_COMMAND_TREE_DELETE_ITEM = _controls.wxEVT_COMMAND_TREE_DELETE_ITEM
wxEVT_COMMAND_TREE_GET_INFO = _controls.wxEVT_COMMAND_TREE_GET_INFO
wxEVT_COMMAND_TREE_SET_INFO = _controls.wxEVT_COMMAND_TREE_SET_INFO
wxEVT_COMMAND_TREE_ITEM_EXPANDED = _controls.wxEVT_COMMAND_TREE_ITEM_EXPANDED
wxEVT_COMMAND_TREE_ITEM_EXPANDING = _controls.wxEVT_COMMAND_TREE_ITEM_EXPANDING
wxEVT_COMMAND_TREE_ITEM_COLLAPSED = _controls.wxEVT_COMMAND_TREE_ITEM_COLLAPSED
wxEVT_COMMAND_TREE_ITEM_COLLAPSING = _controls.wxEVT_COMMAND_TREE_ITEM_COLLAPSING
wxEVT_COMMAND_TREE_SEL_CHANGED = _controls.wxEVT_COMMAND_TREE_SEL_CHANGED
wxEVT_COMMAND_TREE_SEL_CHANGING = _controls.wxEVT_COMMAND_TREE_SEL_CHANGING
wxEVT_COMMAND_TREE_KEY_DOWN = _controls.wxEVT_COMMAND_TREE_KEY_DOWN
wxEVT_COMMAND_TREE_ITEM_ACTIVATED = _controls.wxEVT_COMMAND_TREE_ITEM_ACTIVATED
wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK = _controls.wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK
wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK = _controls.wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK
wxEVT_COMMAND_TREE_END_DRAG = _controls.wxEVT_COMMAND_TREE_END_DRAG
wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK = _controls.wxEVT_COMMAND_TREE_STATE_IMAGE_CLICK
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


class TreeEvent(core.NotifyEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType commandType=wxEVT_NULL, int id=0) -> TreeEvent"""
        newobj = _controls.new_TreeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetItem(*args, **kwargs):
        """GetItem() -> TreeItemId"""
        return _controls.TreeEvent_GetItem(*args, **kwargs)

    def SetItem(*args, **kwargs):
        """SetItem(TreeItemId item)"""
        return _controls.TreeEvent_SetItem(*args, **kwargs)

    def GetOldItem(*args, **kwargs):
        """GetOldItem() -> TreeItemId"""
        return _controls.TreeEvent_GetOldItem(*args, **kwargs)

    def SetOldItem(*args, **kwargs):
        """SetOldItem(TreeItemId item)"""
        return _controls.TreeEvent_SetOldItem(*args, **kwargs)

    def GetPoint(*args, **kwargs):
        """GetPoint() -> Point"""
        return _controls.TreeEvent_GetPoint(*args, **kwargs)

    def SetPoint(*args, **kwargs):
        """SetPoint(Point pt)"""
        return _controls.TreeEvent_SetPoint(*args, **kwargs)

    def GetKeyEvent(*args, **kwargs):
        """GetKeyEvent() -> KeyEvent"""
        return _controls.TreeEvent_GetKeyEvent(*args, **kwargs)

    def GetKeyCode(*args, **kwargs):
        """GetKeyCode() -> int"""
        return _controls.TreeEvent_GetKeyCode(*args, **kwargs)

    def SetKeyEvent(*args, **kwargs):
        """SetKeyEvent(KeyEvent evt)"""
        return _controls.TreeEvent_SetKeyEvent(*args, **kwargs)

    def GetLabel(*args, **kwargs):
        """GetLabel() -> wxString"""
        return _controls.TreeEvent_GetLabel(*args, **kwargs)

    def SetLabel(*args, **kwargs):
        """SetLabel(wxString label)"""
        return _controls.TreeEvent_SetLabel(*args, **kwargs)

    def IsEditCancelled(*args, **kwargs):
        """IsEditCancelled() -> bool"""
        return _controls.TreeEvent_IsEditCancelled(*args, **kwargs)

    def SetEditCanceled(*args, **kwargs):
        """SetEditCanceled(bool editCancelled)"""
        return _controls.TreeEvent_SetEditCanceled(*args, **kwargs)


class TreeEventPtr(TreeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeEvent
_controls.TreeEvent_swigregister(TreeEventPtr)

#---------------------------------------------------------------------------

class TreeCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
    Validator validator=DefaultValidator, 
    wxString name=wxPy_TreeCtrlNameStr) -> TreeCtrl"""
        newobj = _controls.new_TreeCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeCtrl)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=TR_DEFAULT_STYLE, 
    Validator validator=DefaultValidator, 
    wxString name=wxPy_TreeCtrlNameStr) -> bool"""
        return _controls.TreeCtrl_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _controls.TreeCtrl__setCallbackInfo(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> size_t"""
        return _controls.TreeCtrl_GetCount(*args, **kwargs)

    def GetIndent(*args, **kwargs):
        """GetIndent() -> unsigned int"""
        return _controls.TreeCtrl_GetIndent(*args, **kwargs)

    def SetIndent(*args, **kwargs):
        """SetIndent(unsigned int indent)"""
        return _controls.TreeCtrl_SetIndent(*args, **kwargs)

    def GetSpacing(*args, **kwargs):
        """GetSpacing() -> unsigned int"""
        return _controls.TreeCtrl_GetSpacing(*args, **kwargs)

    def SetSpacing(*args, **kwargs):
        """SetSpacing(unsigned int spacing)"""
        return _controls.TreeCtrl_SetSpacing(*args, **kwargs)

    def GetImageList(*args, **kwargs):
        """GetImageList() -> wxImageList"""
        return _controls.TreeCtrl_GetImageList(*args, **kwargs)

    def GetStateImageList(*args, **kwargs):
        """GetStateImageList() -> wxImageList"""
        return _controls.TreeCtrl_GetStateImageList(*args, **kwargs)

    def SetImageList(*args, **kwargs):
        """SetImageList(wxImageList imageList)"""
        return _controls.TreeCtrl_SetImageList(*args, **kwargs)

    def SetStateImageList(*args, **kwargs):
        """SetStateImageList(wxImageList imageList)"""
        return _controls.TreeCtrl_SetStateImageList(*args, **kwargs)

    def AssignImageList(*args, **kwargs):
        """AssignImageList(wxImageList imageList)"""
        val = _controls.TreeCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def AssignStateImageList(*args, **kwargs):
        """AssignStateImageList(wxImageList imageList)"""
        val = _controls.TreeCtrl_AssignStateImageList(*args, **kwargs)
        args[1].thisown = 0
        return val

    def GetItemText(*args, **kwargs):
        """GetItemText(TreeItemId item) -> wxString"""
        return _controls.TreeCtrl_GetItemText(*args, **kwargs)

    def GetItemImage(*args, **kwargs):
        """GetItemImage(TreeItemId item, wxTreeItemIcon which=TreeItemIcon_Normal) -> int"""
        return _controls.TreeCtrl_GetItemImage(*args, **kwargs)

    def GetItemData(*args, **kwargs):
        """GetItemData(TreeItemId item) -> TreeItemData"""
        return _controls.TreeCtrl_GetItemData(*args, **kwargs)

    def GetItemPyData(*args, **kwargs):
        """GetItemPyData(TreeItemId item) -> PyObject"""
        return _controls.TreeCtrl_GetItemPyData(*args, **kwargs)

    GetPyData = GetItemPyData 
    def GetItemTextColour(*args, **kwargs):
        """GetItemTextColour(TreeItemId item) -> wxColour"""
        return _controls.TreeCtrl_GetItemTextColour(*args, **kwargs)

    def GetItemBackgroundColour(*args, **kwargs):
        """GetItemBackgroundColour(TreeItemId item) -> wxColour"""
        return _controls.TreeCtrl_GetItemBackgroundColour(*args, **kwargs)

    def GetItemFont(*args, **kwargs):
        """GetItemFont(TreeItemId item) -> wxFont"""
        return _controls.TreeCtrl_GetItemFont(*args, **kwargs)

    def SetItemText(*args, **kwargs):
        """SetItemText(TreeItemId item, wxString text)"""
        return _controls.TreeCtrl_SetItemText(*args, **kwargs)

    def SetItemImage(*args, **kwargs):
        """SetItemImage(TreeItemId item, int image, wxTreeItemIcon which=TreeItemIcon_Normal)"""
        return _controls.TreeCtrl_SetItemImage(*args, **kwargs)

    def SetItemData(*args, **kwargs):
        """SetItemData(TreeItemId item, TreeItemData data)"""
        return _controls.TreeCtrl_SetItemData(*args, **kwargs)

    def SetItemPyData(*args, **kwargs):
        """SetItemPyData(TreeItemId item, PyObject obj)"""
        return _controls.TreeCtrl_SetItemPyData(*args, **kwargs)

    SetPyData = SetItemPyData 
    def SetItemHasChildren(*args, **kwargs):
        """SetItemHasChildren(TreeItemId item, bool has=True)"""
        return _controls.TreeCtrl_SetItemHasChildren(*args, **kwargs)

    def SetItemBold(*args, **kwargs):
        """SetItemBold(TreeItemId item, bool bold=True)"""
        return _controls.TreeCtrl_SetItemBold(*args, **kwargs)

    def SetItemTextColour(*args, **kwargs):
        """SetItemTextColour(TreeItemId item, wxColour col)"""
        return _controls.TreeCtrl_SetItemTextColour(*args, **kwargs)

    def SetItemBackgroundColour(*args, **kwargs):
        """SetItemBackgroundColour(TreeItemId item, wxColour col)"""
        return _controls.TreeCtrl_SetItemBackgroundColour(*args, **kwargs)

    def SetItemFont(*args, **kwargs):
        """SetItemFont(TreeItemId item, wxFont font)"""
        return _controls.TreeCtrl_SetItemFont(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(TreeItemId item) -> bool"""
        return _controls.TreeCtrl_IsVisible(*args, **kwargs)

    def ItemHasChildren(*args, **kwargs):
        """ItemHasChildren(TreeItemId item) -> bool"""
        return _controls.TreeCtrl_ItemHasChildren(*args, **kwargs)

    def IsExpanded(*args, **kwargs):
        """IsExpanded(TreeItemId item) -> bool"""
        return _controls.TreeCtrl_IsExpanded(*args, **kwargs)

    def IsSelected(*args, **kwargs):
        """IsSelected(TreeItemId item) -> bool"""
        return _controls.TreeCtrl_IsSelected(*args, **kwargs)

    def IsBold(*args, **kwargs):
        """IsBold(TreeItemId item) -> bool"""
        return _controls.TreeCtrl_IsBold(*args, **kwargs)

    def GetChildrenCount(*args, **kwargs):
        """GetChildrenCount(TreeItemId item, bool recursively=True) -> size_t"""
        return _controls.TreeCtrl_GetChildrenCount(*args, **kwargs)

    def GetRootItem(*args, **kwargs):
        """GetRootItem() -> TreeItemId"""
        return _controls.TreeCtrl_GetRootItem(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection() -> TreeItemId"""
        return _controls.TreeCtrl_GetSelection(*args, **kwargs)

    def GetSelections(*args, **kwargs):
        """GetSelections() -> PyObject"""
        return _controls.TreeCtrl_GetSelections(*args, **kwargs)

    def GetItemParent(*args, **kwargs):
        """GetItemParent(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetItemParent(*args, **kwargs)

    def GetFirstChild(*args, **kwargs):
        """GetFirstChild(TreeItemId item) -> PyObject"""
        return _controls.TreeCtrl_GetFirstChild(*args, **kwargs)

    def GetNextChild(*args, **kwargs):
        """GetNextChild(TreeItemId item, wxTreeItemIdValue cookie) -> PyObject"""
        return _controls.TreeCtrl_GetNextChild(*args, **kwargs)

    def GetLastChild(*args, **kwargs):
        """GetLastChild(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetLastChild(*args, **kwargs)

    def GetNextSibling(*args, **kwargs):
        """GetNextSibling(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetNextSibling(*args, **kwargs)

    def GetPrevSibling(*args, **kwargs):
        """GetPrevSibling(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetPrevSibling(*args, **kwargs)

    def GetFirstVisibleItem(*args, **kwargs):
        """GetFirstVisibleItem() -> TreeItemId"""
        return _controls.TreeCtrl_GetFirstVisibleItem(*args, **kwargs)

    def GetNextVisible(*args, **kwargs):
        """GetNextVisible(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetNextVisible(*args, **kwargs)

    def GetPrevVisible(*args, **kwargs):
        """GetPrevVisible(TreeItemId item) -> TreeItemId"""
        return _controls.TreeCtrl_GetPrevVisible(*args, **kwargs)

    def AddRoot(*args, **kwargs):
        """AddRoot(wxString text, int image=-1, int selectedImage=-1, 
    TreeItemData data=None) -> TreeItemId"""
        return _controls.TreeCtrl_AddRoot(*args, **kwargs)

    def PrependItem(*args, **kwargs):
        """PrependItem(TreeItemId parent, wxString text, int image=-1, int selectedImage=-1, 
    TreeItemData data=None) -> TreeItemId"""
        return _controls.TreeCtrl_PrependItem(*args, **kwargs)

    def InsertItem(*args, **kwargs):
        """InsertItem(TreeItemId parent, TreeItemId idPrevious, wxString text, 
    int image=-1, int selectedImage=-1, TreeItemData data=None) -> TreeItemId"""
        return _controls.TreeCtrl_InsertItem(*args, **kwargs)

    def InsertItemBefore(*args, **kwargs):
        """InsertItemBefore(TreeItemId parent, size_t index, wxString text, int image=-1, 
    int selectedImage=-1, TreeItemData data=None) -> TreeItemId"""
        return _controls.TreeCtrl_InsertItemBefore(*args, **kwargs)

    def AppendItem(*args, **kwargs):
        """AppendItem(TreeItemId parent, wxString text, int image=-1, int selectedImage=-1, 
    TreeItemData data=None) -> TreeItemId"""
        return _controls.TreeCtrl_AppendItem(*args, **kwargs)

    def Delete(*args, **kwargs):
        """Delete(TreeItemId item)"""
        return _controls.TreeCtrl_Delete(*args, **kwargs)

    def DeleteChildren(*args, **kwargs):
        """DeleteChildren(TreeItemId item)"""
        return _controls.TreeCtrl_DeleteChildren(*args, **kwargs)

    def DeleteAllItems(*args, **kwargs):
        """DeleteAllItems()"""
        return _controls.TreeCtrl_DeleteAllItems(*args, **kwargs)

    def Expand(*args, **kwargs):
        """Expand(TreeItemId item)"""
        return _controls.TreeCtrl_Expand(*args, **kwargs)

    def Collapse(*args, **kwargs):
        """Collapse(TreeItemId item)"""
        return _controls.TreeCtrl_Collapse(*args, **kwargs)

    def CollapseAndReset(*args, **kwargs):
        """CollapseAndReset(TreeItemId item)"""
        return _controls.TreeCtrl_CollapseAndReset(*args, **kwargs)

    def Toggle(*args, **kwargs):
        """Toggle(TreeItemId item)"""
        return _controls.TreeCtrl_Toggle(*args, **kwargs)

    def Unselect(*args, **kwargs):
        """Unselect()"""
        return _controls.TreeCtrl_Unselect(*args, **kwargs)

    def UnselectAll(*args, **kwargs):
        """UnselectAll()"""
        return _controls.TreeCtrl_UnselectAll(*args, **kwargs)

    def SelectItem(*args, **kwargs):
        """SelectItem(TreeItemId item)"""
        return _controls.TreeCtrl_SelectItem(*args, **kwargs)

    def EnsureVisible(*args, **kwargs):
        """EnsureVisible(TreeItemId item)"""
        return _controls.TreeCtrl_EnsureVisible(*args, **kwargs)

    def ScrollTo(*args, **kwargs):
        """ScrollTo(TreeItemId item)"""
        return _controls.TreeCtrl_ScrollTo(*args, **kwargs)

    def EditLabel(*args, **kwargs):
        """EditLabel(TreeItemId item)"""
        return _controls.TreeCtrl_EditLabel(*args, **kwargs)

    def GetEditControl(*args, **kwargs):
        """GetEditControl() -> TextCtrl"""
        return _controls.TreeCtrl_GetEditControl(*args, **kwargs)

    def SortChildren(*args, **kwargs):
        """SortChildren(TreeItemId item)"""
        return _controls.TreeCtrl_SortChildren(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(Point point) -> (item, where)

Determine which item (if any) belongs the given point.  The
coordinates specified are relative to the client area of tree ctrl
and the where return value is set to a bitmask of wxTREE_HITTEST_xxx
constants.
"""
        return _controls.TreeCtrl_HitTest(*args, **kwargs)

    def GetBoundingRect(*args, **kwargs):
        """GetBoundingRect(TreeItemId item, bool textOnly=False) -> PyObject"""
        return _controls.TreeCtrl_GetBoundingRect(*args, **kwargs)


class TreeCtrlPtr(TreeCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeCtrl
_controls.TreeCtrl_swigregister(TreeCtrlPtr)

def PreTreeCtrl(*args, **kwargs):
    """PreTreeCtrl() -> TreeCtrl"""
    val = _controls.new_PreTreeCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

DIRCTRL_DIR_ONLY = _controls.DIRCTRL_DIR_ONLY
DIRCTRL_SELECT_FIRST = _controls.DIRCTRL_SELECT_FIRST
DIRCTRL_SHOW_FILTERS = _controls.DIRCTRL_SHOW_FILTERS
DIRCTRL_3D_INTERNAL = _controls.DIRCTRL_3D_INTERNAL
DIRCTRL_EDIT_LABELS = _controls.DIRCTRL_EDIT_LABELS
class GenericDirCtrl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDirCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=-1, wxString dir=wxPyDirDialogDefaultFolderStr, 
    Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, 
    wxString filter=wxPyEmptyString, 
    int defaultFilter=0, wxString name=wxPy_TreeCtrlNameStr) -> GenericDirCtrl"""
        newobj = _controls.new_GenericDirCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(Window parent, int id=-1, wxString dir=wxPyDirDialogDefaultFolderStr, 
    Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER, 
    wxString filter=wxPyEmptyString, 
    int defaultFilter=0, wxString name=wxPy_TreeCtrlNameStr) -> bool"""
        return _controls.GenericDirCtrl_Create(*args, **kwargs)

    def ExpandPath(*args, **kwargs):
        """ExpandPath(wxString path) -> bool"""
        return _controls.GenericDirCtrl_ExpandPath(*args, **kwargs)

    def GetDefaultPath(*args, **kwargs):
        """GetDefaultPath() -> wxString"""
        return _controls.GenericDirCtrl_GetDefaultPath(*args, **kwargs)

    def SetDefaultPath(*args, **kwargs):
        """SetDefaultPath(wxString path)"""
        return _controls.GenericDirCtrl_SetDefaultPath(*args, **kwargs)

    def GetPath(*args, **kwargs):
        """GetPath() -> wxString"""
        return _controls.GenericDirCtrl_GetPath(*args, **kwargs)

    def GetFilePath(*args, **kwargs):
        """GetFilePath() -> wxString"""
        return _controls.GenericDirCtrl_GetFilePath(*args, **kwargs)

    def SetPath(*args, **kwargs):
        """SetPath(wxString path)"""
        return _controls.GenericDirCtrl_SetPath(*args, **kwargs)

    def ShowHidden(*args, **kwargs):
        """ShowHidden(bool show)"""
        return _controls.GenericDirCtrl_ShowHidden(*args, **kwargs)

    def GetShowHidden(*args, **kwargs):
        """GetShowHidden() -> bool"""
        return _controls.GenericDirCtrl_GetShowHidden(*args, **kwargs)

    def GetFilter(*args, **kwargs):
        """GetFilter() -> wxString"""
        return _controls.GenericDirCtrl_GetFilter(*args, **kwargs)

    def SetFilter(*args, **kwargs):
        """SetFilter(wxString filter)"""
        return _controls.GenericDirCtrl_SetFilter(*args, **kwargs)

    def GetFilterIndex(*args, **kwargs):
        """GetFilterIndex() -> int"""
        return _controls.GenericDirCtrl_GetFilterIndex(*args, **kwargs)

    def SetFilterIndex(*args, **kwargs):
        """SetFilterIndex(int n)"""
        return _controls.GenericDirCtrl_SetFilterIndex(*args, **kwargs)

    def GetRootId(*args, **kwargs):
        """GetRootId() -> TreeItemId"""
        return _controls.GenericDirCtrl_GetRootId(*args, **kwargs)

    def GetTreeCtrl(*args, **kwargs):
        """GetTreeCtrl() -> wxTreeCtrl"""
        return _controls.GenericDirCtrl_GetTreeCtrl(*args, **kwargs)

    def GetFilterListCtrl(*args, **kwargs):
        """GetFilterListCtrl() -> DirFilterListCtrl"""
        return _controls.GenericDirCtrl_GetFilterListCtrl(*args, **kwargs)

    def FindChild(*args, **kwargs):
        """FindChild(wxTreeItemId parentId, wxString path) -> (item, done)

Find the child that matches the first part of 'path'.  E.g. if a child path is
"/usr" and 'path' is "/usr/include" then the child for /usr is returned.
If the path string has been used (we're at the leaf), done is set to True
"""
        return _controls.GenericDirCtrl_FindChild(*args, **kwargs)

    def DoResize(*args, **kwargs):
        """DoResize()"""
        return _controls.GenericDirCtrl_DoResize(*args, **kwargs)

    def ReCreateTree(*args, **kwargs):
        """ReCreateTree()"""
        return _controls.GenericDirCtrl_ReCreateTree(*args, **kwargs)


class GenericDirCtrlPtr(GenericDirCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GenericDirCtrl
_controls.GenericDirCtrl_swigregister(GenericDirCtrlPtr)

def PreGenericDirCtrl(*args, **kwargs):
    """PreGenericDirCtrl() -> GenericDirCtrl"""
    val = _controls.new_PreGenericDirCtrl(*args, **kwargs)
    val.thisown = 1
    return val

class DirFilterListCtrl(Choice):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDirFilterListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(GenericDirCtrl parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0) -> DirFilterListCtrl"""
        newobj = _controls.new_DirFilterListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(GenericDirCtrl parent, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=0) -> bool"""
        return _controls.DirFilterListCtrl_Create(*args, **kwargs)

    def FillFilterList(*args, **kwargs):
        """FillFilterList(wxString filter, int defaultFilter)"""
        return _controls.DirFilterListCtrl_FillFilterList(*args, **kwargs)


class DirFilterListCtrlPtr(DirFilterListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DirFilterListCtrl
_controls.DirFilterListCtrl_swigregister(DirFilterListCtrlPtr)

def PreDirFilterListCtrl(*args, **kwargs):
    """PreDirFilterListCtrl() -> DirFilterListCtrl"""
    val = _controls.new_PreDirFilterListCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class PyControl(core.Control):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize, 
    long style=0, Validator validator=DefaultValidator, 
    wxString name=wxPyControlNameStr) -> PyControl"""
        newobj = _controls.new_PyControl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, PyControl)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _controls.PyControl__setCallbackInfo(*args, **kwargs)

    def base_DoMoveWindow(*args, **kwargs):
        """base_DoMoveWindow(int x, int y, int width, int height)"""
        return _controls.PyControl_base_DoMoveWindow(*args, **kwargs)

    def base_DoSetSize(*args, **kwargs):
        """base_DoSetSize(int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _controls.PyControl_base_DoSetSize(*args, **kwargs)

    def base_DoSetClientSize(*args, **kwargs):
        """base_DoSetClientSize(int width, int height)"""
        return _controls.PyControl_base_DoSetClientSize(*args, **kwargs)

    def base_DoSetVirtualSize(*args, **kwargs):
        """base_DoSetVirtualSize(int x, int y)"""
        return _controls.PyControl_base_DoSetVirtualSize(*args, **kwargs)

    def base_DoGetSize(*args, **kwargs):
        """base_DoGetSize() -> (width, height)"""
        return _controls.PyControl_base_DoGetSize(*args, **kwargs)

    def base_DoGetClientSize(*args, **kwargs):
        """base_DoGetClientSize() -> (width, height)"""
        return _controls.PyControl_base_DoGetClientSize(*args, **kwargs)

    def base_DoGetPosition(*args, **kwargs):
        """base_DoGetPosition() -> (x,y)"""
        return _controls.PyControl_base_DoGetPosition(*args, **kwargs)

    def base_DoGetVirtualSize(*args, **kwargs):
        """base_DoGetVirtualSize() -> Size"""
        return _controls.PyControl_base_DoGetVirtualSize(*args, **kwargs)

    def base_DoGetBestSize(*args, **kwargs):
        """base_DoGetBestSize() -> Size"""
        return _controls.PyControl_base_DoGetBestSize(*args, **kwargs)

    def base_InitDialog(*args, **kwargs):
        """base_InitDialog()"""
        return _controls.PyControl_base_InitDialog(*args, **kwargs)

    def base_TransferDataToWindow(*args, **kwargs):
        """base_TransferDataToWindow() -> bool"""
        return _controls.PyControl_base_TransferDataToWindow(*args, **kwargs)

    def base_TransferDataFromWindow(*args, **kwargs):
        """base_TransferDataFromWindow() -> bool"""
        return _controls.PyControl_base_TransferDataFromWindow(*args, **kwargs)

    def base_Validate(*args, **kwargs):
        """base_Validate() -> bool"""
        return _controls.PyControl_base_Validate(*args, **kwargs)

    def base_AcceptsFocus(*args, **kwargs):
        """base_AcceptsFocus() -> bool"""
        return _controls.PyControl_base_AcceptsFocus(*args, **kwargs)

    def base_AcceptsFocusFromKeyboard(*args, **kwargs):
        """base_AcceptsFocusFromKeyboard() -> bool"""
        return _controls.PyControl_base_AcceptsFocusFromKeyboard(*args, **kwargs)

    def base_GetMaxSize(*args, **kwargs):
        """base_GetMaxSize() -> Size"""
        return _controls.PyControl_base_GetMaxSize(*args, **kwargs)

    def base_AddChild(*args, **kwargs):
        """base_AddChild(Window child)"""
        return _controls.PyControl_base_AddChild(*args, **kwargs)

    def base_RemoveChild(*args, **kwargs):
        """base_RemoveChild(Window child)"""
        return _controls.PyControl_base_RemoveChild(*args, **kwargs)


class PyControlPtr(PyControl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyControl
_controls.PyControl_swigregister(PyControlPtr)

#---------------------------------------------------------------------------

FRAME_EX_CONTEXTHELP = _controls.FRAME_EX_CONTEXTHELP
DIALOG_EX_CONTEXTHELP = _controls.DIALOG_EX_CONTEXTHELP
wxEVT_HELP = _controls.wxEVT_HELP
wxEVT_DETAILED_HELP = _controls.wxEVT_DETAILED_HELP
EVT_HELP = wx.PyEventBinder( wxEVT_HELP, 1)
EVT_HELP_RANGE = wx.PyEventBinder(  wxEVT_HELP, 2)
EVT_DETAILED_HELP = wx.PyEventBinder( wxEVT_DETAILED_HELP, 1)
EVT_DETAILED_HELP_RANGE = wx.PyEventBinder( wxEVT_DETAILED_HELP, 2)

class HelpEvent(core.CommandEvent):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxEventType type=wxEVT_NULL, int winid=0, Point pt=DefaultPosition) -> HelpEvent"""
        newobj = _controls.new_HelpEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs):
        """GetPosition() -> Point"""
        return _controls.HelpEvent_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(Point pos)"""
        return _controls.HelpEvent_SetPosition(*args, **kwargs)

    def GetLink(*args, **kwargs):
        """GetLink() -> wxString"""
        return _controls.HelpEvent_GetLink(*args, **kwargs)

    def SetLink(*args, **kwargs):
        """SetLink(wxString link)"""
        return _controls.HelpEvent_SetLink(*args, **kwargs)

    def GetTarget(*args, **kwargs):
        """GetTarget() -> wxString"""
        return _controls.HelpEvent_GetTarget(*args, **kwargs)

    def SetTarget(*args, **kwargs):
        """SetTarget(wxString target)"""
        return _controls.HelpEvent_SetTarget(*args, **kwargs)


class HelpEventPtr(HelpEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpEvent
_controls.HelpEvent_swigregister(HelpEventPtr)

class ContextHelp(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelp instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window window=None, bool doNow=True) -> ContextHelp"""
        newobj = _controls.new_ContextHelp(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_ContextHelp):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def BeginContextHelp(*args, **kwargs):
        """BeginContextHelp(Window window=None) -> bool"""
        return _controls.ContextHelp_BeginContextHelp(*args, **kwargs)

    def EndContextHelp(*args, **kwargs):
        """EndContextHelp() -> bool"""
        return _controls.ContextHelp_EndContextHelp(*args, **kwargs)


class ContextHelpPtr(ContextHelp):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelp
_controls.ContextHelp_swigregister(ContextHelpPtr)

class ContextHelpButton(BitmapButton):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelpButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent, int id=ID_CONTEXT_HELP, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=BU_AUTODRAW) -> ContextHelpButton"""
        newobj = _controls.new_ContextHelpButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class ContextHelpButtonPtr(ContextHelpButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelpButton
_controls.ContextHelpButton_swigregister(ContextHelpButtonPtr)

class HelpProvider(object):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Set(*args, **kwargs):
        """HelpProvider.Set(HelpProvider helpProvider) -> HelpProvider"""
        return _controls.HelpProvider_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def Get(*args, **kwargs):
        """HelpProvider.Get() -> HelpProvider"""
        return _controls.HelpProvider_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def GetHelp(*args, **kwargs):
        """GetHelp(Window window) -> wxString"""
        return _controls.HelpProvider_GetHelp(*args, **kwargs)

    def ShowHelp(*args, **kwargs):
        """ShowHelp(Window window) -> bool"""
        return _controls.HelpProvider_ShowHelp(*args, **kwargs)

    def AddHelp(*args, **kwargs):
        """AddHelp(Window window, wxString text)"""
        return _controls.HelpProvider_AddHelp(*args, **kwargs)

    def AddHelpById(*args, **kwargs):
        """AddHelpById(int id, wxString text)"""
        return _controls.HelpProvider_AddHelpById(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy()"""
        return _controls.HelpProvider_Destroy(*args, **kwargs)


class HelpProviderPtr(HelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpProvider
_controls.HelpProvider_swigregister(HelpProviderPtr)

def HelpProvider_Set(*args, **kwargs):
    """HelpProvider_Set(HelpProvider helpProvider) -> HelpProvider"""
    return _controls.HelpProvider_Set(*args, **kwargs)

def HelpProvider_Get(*args, **kwargs):
    """HelpProvider_Get() -> HelpProvider"""
    return _controls.HelpProvider_Get(*args, **kwargs)

class SimpleHelpProvider(HelpProvider):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSimpleHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> SimpleHelpProvider"""
        newobj = _controls.new_SimpleHelpProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class SimpleHelpProviderPtr(SimpleHelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SimpleHelpProvider
_controls.SimpleHelpProvider_swigregister(SimpleHelpProviderPtr)

#---------------------------------------------------------------------------

class DragImage(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDragImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxBitmap image, wxCursor cursor=wxNullCursor) -> DragImage"""
        newobj = _controls.new_DragImage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_DragImage):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetBackingBitmap(*args, **kwargs):
        """SetBackingBitmap(wxBitmap bitmap)"""
        return _controls.DragImage_SetBackingBitmap(*args, **kwargs)

    def BeginDrag(*args, **kwargs):
        """BeginDrag(Point hotspot, Window window, bool fullScreen=False, 
    Rect rect=None) -> bool"""
        return _controls.DragImage_BeginDrag(*args, **kwargs)

    def BeginDragBounded(*args, **kwargs):
        """BeginDragBounded(Point hotspot, Window window, Window boundingWindow) -> bool"""
        return _controls.DragImage_BeginDragBounded(*args, **kwargs)

    def EndDrag(*args, **kwargs):
        """EndDrag() -> bool"""
        return _controls.DragImage_EndDrag(*args, **kwargs)

    def Move(*args, **kwargs):
        """Move(Point pt) -> bool"""
        return _controls.DragImage_Move(*args, **kwargs)

    def Show(*args, **kwargs):
        """Show() -> bool"""
        return _controls.DragImage_Show(*args, **kwargs)

    def Hide(*args, **kwargs):
        """Hide() -> bool"""
        return _controls.DragImage_Hide(*args, **kwargs)

    def GetImageRect(*args, **kwargs):
        """GetImageRect(Point pos) -> Rect"""
        return _controls.DragImage_GetImageRect(*args, **kwargs)

    def DoDrawImage(*args, **kwargs):
        """DoDrawImage(wxDC dc, Point pos) -> bool"""
        return _controls.DragImage_DoDrawImage(*args, **kwargs)

    def UpdateBackingFromWindow(*args, **kwargs):
        """UpdateBackingFromWindow(wxDC windowDC, wxMemoryDC destDC, Rect sourceRect, 
    Rect destRect) -> bool"""
        return _controls.DragImage_UpdateBackingFromWindow(*args, **kwargs)

    def RedrawImage(*args, **kwargs):
        """RedrawImage(Point oldPos, Point newPos, bool eraseOld, bool drawNew) -> bool"""
        return _controls.DragImage_RedrawImage(*args, **kwargs)


class DragImagePtr(DragImage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DragImage
_controls.DragImage_swigregister(DragImagePtr)

def DragIcon(*args, **kwargs):
    """DragIcon(wxIcon image, wxCursor cursor=wxNullCursor) -> DragImage"""
    val = _controls.new_DragIcon(*args, **kwargs)
    val.thisown = 1
    return val

def DragString(*args, **kwargs):
    """DragString(wxString str, wxCursor cursor=wxNullCursor) -> DragImage"""
    val = _controls.new_DragString(*args, **kwargs)
    val.thisown = 1
    return val

def DragTreeItem(*args, **kwargs):
    """DragTreeItem(TreeCtrl treeCtrl, TreeItemId id) -> DragImage"""
    val = _controls.new_DragTreeItem(*args, **kwargs)
    val.thisown = 1
    return val

def DragListItem(*args, **kwargs):
    """DragListItem(ListCtrl listCtrl, long id) -> DragImage"""
    val = _controls.new_DragListItem(*args, **kwargs)
    val.thisown = 1
    return val


