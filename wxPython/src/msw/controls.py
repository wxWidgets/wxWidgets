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
class Button(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Button(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Button_Create(*args, **kwargs)
    def SetDefault(*args, **kwargs): return _controls.Button_SetDefault(*args, **kwargs)
    def SetImageLabel(*args, **kwargs): return _controls.Button_SetImageLabel(*args, **kwargs)
    def SetImageMargins(*args, **kwargs): return _controls.Button_SetImageMargins(*args, **kwargs)
    GetDefaultSize = staticmethod(_controls.Button_GetDefaultSize)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ButtonPtr(Button):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Button
_controls.Button_swigregister(ButtonPtr)

def PreButton(*args, **kwargs):
    val = _controls.new_PreButton(*args, **kwargs)
    val.thisown = 1
    return val

Button_GetDefaultSize = _controls.Button_GetDefaultSize

class BitmapButton(Button):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_BitmapButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.BitmapButton_Create(*args, **kwargs)
    def GetBitmapLabel(*args, **kwargs): return _controls.BitmapButton_GetBitmapLabel(*args, **kwargs)
    def GetBitmapDisabled(*args, **kwargs): return _controls.BitmapButton_GetBitmapDisabled(*args, **kwargs)
    def GetBitmapFocus(*args, **kwargs): return _controls.BitmapButton_GetBitmapFocus(*args, **kwargs)
    def GetBitmapSelected(*args, **kwargs): return _controls.BitmapButton_GetBitmapSelected(*args, **kwargs)
    def SetBitmapDisabled(*args, **kwargs): return _controls.BitmapButton_SetBitmapDisabled(*args, **kwargs)
    def SetBitmapFocus(*args, **kwargs): return _controls.BitmapButton_SetBitmapFocus(*args, **kwargs)
    def SetBitmapSelected(*args, **kwargs): return _controls.BitmapButton_SetBitmapSelected(*args, **kwargs)
    def SetBitmapLabel(*args, **kwargs): return _controls.BitmapButton_SetBitmapLabel(*args, **kwargs)
    def SetMargins(*args, **kwargs): return _controls.BitmapButton_SetMargins(*args, **kwargs)
    def GetMarginX(*args, **kwargs): return _controls.BitmapButton_GetMarginX(*args, **kwargs)
    def GetMarginY(*args, **kwargs): return _controls.BitmapButton_GetMarginY(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmapButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BitmapButtonPtr(BitmapButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BitmapButton
_controls.BitmapButton_swigregister(BitmapButtonPtr)

def PreBitmapButton(*args, **kwargs):
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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_CheckBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.CheckBox_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.CheckBox_GetValue(*args, **kwargs)
    def IsChecked(*args, **kwargs): return _controls.CheckBox_IsChecked(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.CheckBox_SetValue(*args, **kwargs)
    def Get3StateValue(*args, **kwargs): return _controls.CheckBox_Get3StateValue(*args, **kwargs)
    def Set3StateValue(*args, **kwargs): return _controls.CheckBox_Set3StateValue(*args, **kwargs)
    def Is3State(*args, **kwargs): return _controls.CheckBox_Is3State(*args, **kwargs)
    def Is3rdStateAllowedForUser(*args, **kwargs): return _controls.CheckBox_Is3rdStateAllowedForUser(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CheckBoxPtr(CheckBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckBox
_controls.CheckBox_swigregister(CheckBoxPtr)

def PreCheckBox(*args, **kwargs):
    val = _controls.new_PreCheckBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Choice(core.ControlWithItems):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Choice(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Choice_Create(*args, **kwargs)
    def GetColumns(*args, **kwargs): return _controls.Choice_GetColumns(*args, **kwargs)
    def SetColumns(*args, **kwargs): return _controls.Choice_SetColumns(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.Choice_SetSelection(*args, **kwargs)
    def SetStringSelection(*args, **kwargs): return _controls.Choice_SetStringSelection(*args, **kwargs)
    def SetString(*args, **kwargs): return _controls.Choice_SetString(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxChoice instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ChoicePtr(Choice):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Choice
_controls.Choice_swigregister(ChoicePtr)

def PreChoice(*args, **kwargs):
    val = _controls.new_PreChoice(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ComboBox(Choice):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ComboBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ComboBox_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.ComboBox_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.ComboBox_SetValue(*args, **kwargs)
    def Copy(*args, **kwargs): return _controls.ComboBox_Copy(*args, **kwargs)
    def Cut(*args, **kwargs): return _controls.ComboBox_Cut(*args, **kwargs)
    def Paste(*args, **kwargs): return _controls.ComboBox_Paste(*args, **kwargs)
    def SetInsertionPoint(*args, **kwargs): return _controls.ComboBox_SetInsertionPoint(*args, **kwargs)
    def GetInsertionPoint(*args, **kwargs): return _controls.ComboBox_GetInsertionPoint(*args, **kwargs)
    def GetLastPosition(*args, **kwargs): return _controls.ComboBox_GetLastPosition(*args, **kwargs)
    def Replace(*args, **kwargs): return _controls.ComboBox_Replace(*args, **kwargs)
    def SetMark(*args, **kwargs): return _controls.ComboBox_SetMark(*args, **kwargs)
    def SetEditable(*args, **kwargs): return _controls.ComboBox_SetEditable(*args, **kwargs)
    def SetInsertionPointEnd(*args, **kwargs): return _controls.ComboBox_SetInsertionPointEnd(*args, **kwargs)
    def Remove(*args, **kwargs): return _controls.ComboBox_Remove(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxComboBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ComboBoxPtr(ComboBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ComboBox
_controls.ComboBox_swigregister(ComboBoxPtr)

def PreComboBox(*args, **kwargs):
    val = _controls.new_PreComboBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

GA_HORIZONTAL = _controls.GA_HORIZONTAL
GA_VERTICAL = _controls.GA_VERTICAL
GA_SMOOTH = _controls.GA_SMOOTH
GA_PROGRESSBAR = _controls.GA_PROGRESSBAR
class Gauge(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Gauge(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Gauge_Create(*args, **kwargs)
    def SetRange(*args, **kwargs): return _controls.Gauge_SetRange(*args, **kwargs)
    def GetRange(*args, **kwargs): return _controls.Gauge_GetRange(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.Gauge_SetValue(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.Gauge_GetValue(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.Gauge_IsVertical(*args, **kwargs)
    def SetShadowWidth(*args, **kwargs): return _controls.Gauge_SetShadowWidth(*args, **kwargs)
    def GetShadowWidth(*args, **kwargs): return _controls.Gauge_GetShadowWidth(*args, **kwargs)
    def SetBezelFace(*args, **kwargs): return _controls.Gauge_SetBezelFace(*args, **kwargs)
    def GetBezelFace(*args, **kwargs): return _controls.Gauge_GetBezelFace(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGauge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GaugePtr(Gauge):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Gauge
_controls.Gauge_swigregister(GaugePtr)

def PreGauge(*args, **kwargs):
    val = _controls.new_PreGauge(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticBox(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_StaticBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.StaticBox_Create(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class StaticBoxPtr(StaticBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBox
_controls.StaticBox_swigregister(StaticBoxPtr)

def PreStaticBox(*args, **kwargs):
    val = _controls.new_PreStaticBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticLine(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_StaticLine(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.StaticLine_Create(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.StaticLine_IsVertical(*args, **kwargs)
    GetDefaultSize = staticmethod(_controls.StaticLine_GetDefaultSize)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticLine instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class StaticLinePtr(StaticLine):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticLine
_controls.StaticLine_swigregister(StaticLinePtr)

def PreStaticLine(*args, **kwargs):
    val = _controls.new_PreStaticLine(*args, **kwargs)
    val.thisown = 1
    return val

StaticLine_GetDefaultSize = _controls.StaticLine_GetDefaultSize

#---------------------------------------------------------------------------

class StaticText(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_StaticText(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.StaticText_Create(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticText instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class StaticTextPtr(StaticText):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticText
_controls.StaticText_swigregister(StaticTextPtr)

def PreStaticText(*args, **kwargs):
    val = _controls.new_PreStaticText(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class StaticBitmap(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_StaticBitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Create(*args, **kwargs): return _controls.StaticBitmap_Create(*args, **kwargs)
    def GetBitmap(*args, **kwargs): return _controls.StaticBitmap_GetBitmap(*args, **kwargs)
    def SetBitmap(*args, **kwargs): return _controls.StaticBitmap_SetBitmap(*args, **kwargs)
    def SetIcon(*args, **kwargs): return _controls.StaticBitmap_SetIcon(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxStaticBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class StaticBitmapPtr(StaticBitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = StaticBitmap
_controls.StaticBitmap_swigregister(StaticBitmapPtr)

def PreStaticBitmap(*args, **kwargs):
    val = _controls.new_PreStaticBitmap(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ListBox(core.ControlWithItems):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ListBox_Create(*args, **kwargs)
    def Insert(*args, **kwargs): return _controls.ListBox_Insert(*args, **kwargs)
    def InsertItems(*args, **kwargs): return _controls.ListBox_InsertItems(*args, **kwargs)
    def Set(*args, **kwargs): return _controls.ListBox_Set(*args, **kwargs)
    def IsSelected(*args, **kwargs): return _controls.ListBox_IsSelected(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.ListBox_SetSelection(*args, **kwargs)
    def Select(*args, **kwargs): return _controls.ListBox_Select(*args, **kwargs)
    def Deselect(*args, **kwargs): return _controls.ListBox_Deselect(*args, **kwargs)
    def DeselectAll(*args, **kwargs): return _controls.ListBox_DeselectAll(*args, **kwargs)
    def SetStringSelection(*args, **kwargs): return _controls.ListBox_SetStringSelection(*args, **kwargs)
    def GetSelections(*args, **kwargs): return _controls.ListBox_GetSelections(*args, **kwargs)
    def SetFirstItem(*args, **kwargs): return _controls.ListBox_SetFirstItem(*args, **kwargs)
    def SetFirstItemStr(*args, **kwargs): return _controls.ListBox_SetFirstItemStr(*args, **kwargs)
    def EnsureVisible(*args, **kwargs): return _controls.ListBox_EnsureVisible(*args, **kwargs)
    def AppendAndEnsureVisible(*args, **kwargs): return _controls.ListBox_AppendAndEnsureVisible(*args, **kwargs)
    def IsSorted(*args, **kwargs): return _controls.ListBox_IsSorted(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListBoxPtr(ListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListBox
_controls.ListBox_swigregister(ListBoxPtr)

def PreListBox(*args, **kwargs):
    val = _controls.new_PreListBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class CheckListBox(ListBox):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_CheckListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Create(*args, **kwargs): return _controls.CheckListBox_Create(*args, **kwargs)
    def IsChecked(*args, **kwargs): return _controls.CheckListBox_IsChecked(*args, **kwargs)
    def Check(*args, **kwargs): return _controls.CheckListBox_Check(*args, **kwargs)
    def GetItemHeight(*args, **kwargs): return _controls.CheckListBox_GetItemHeight(*args, **kwargs)
    def HitTest(*args, **kwargs): return _controls.CheckListBox_HitTest(*args, **kwargs)
    def HitTestXY(*args, **kwargs): return _controls.CheckListBox_HitTestXY(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCheckListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CheckListBoxPtr(CheckListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = CheckListBox
_controls.CheckListBox_swigregister(CheckListBoxPtr)

def PreCheckListBox(*args, **kwargs):
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
    def __init__(self, *args):
        newobj = _controls.new_TextAttr(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Init(*args, **kwargs): return _controls.TextAttr_Init(*args, **kwargs)
    def SetTextColour(*args, **kwargs): return _controls.TextAttr_SetTextColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _controls.TextAttr_SetBackgroundColour(*args, **kwargs)
    def SetFont(*args, **kwargs): return _controls.TextAttr_SetFont(*args, **kwargs)
    def SetAlignment(*args, **kwargs): return _controls.TextAttr_SetAlignment(*args, **kwargs)
    def SetTabs(*args, **kwargs): return _controls.TextAttr_SetTabs(*args, **kwargs)
    def SetLeftIndent(*args, **kwargs): return _controls.TextAttr_SetLeftIndent(*args, **kwargs)
    def SetRightIndent(*args, **kwargs): return _controls.TextAttr_SetRightIndent(*args, **kwargs)
    def SetFlags(*args, **kwargs): return _controls.TextAttr_SetFlags(*args, **kwargs)
    def HasTextColour(*args, **kwargs): return _controls.TextAttr_HasTextColour(*args, **kwargs)
    def HasBackgroundColour(*args, **kwargs): return _controls.TextAttr_HasBackgroundColour(*args, **kwargs)
    def HasFont(*args, **kwargs): return _controls.TextAttr_HasFont(*args, **kwargs)
    def HasAlignment(*args, **kwargs): return _controls.TextAttr_HasAlignment(*args, **kwargs)
    def HasTabs(*args, **kwargs): return _controls.TextAttr_HasTabs(*args, **kwargs)
    def HasLeftIndent(*args, **kwargs): return _controls.TextAttr_HasLeftIndent(*args, **kwargs)
    def HasRightIndent(*args, **kwargs): return _controls.TextAttr_HasRightIndent(*args, **kwargs)
    def HasFlag(*args, **kwargs): return _controls.TextAttr_HasFlag(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _controls.TextAttr_GetTextColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _controls.TextAttr_GetBackgroundColour(*args, **kwargs)
    def GetFont(*args, **kwargs): return _controls.TextAttr_GetFont(*args, **kwargs)
    def GetAlignment(*args, **kwargs): return _controls.TextAttr_GetAlignment(*args, **kwargs)
    def GetTabs(*args, **kwargs): return _controls.TextAttr_GetTabs(*args, **kwargs)
    def GetLeftIndent(*args, **kwargs): return _controls.TextAttr_GetLeftIndent(*args, **kwargs)
    def GetRightIndent(*args, **kwargs): return _controls.TextAttr_GetRightIndent(*args, **kwargs)
    def GetFlags(*args, **kwargs): return _controls.TextAttr_GetFlags(*args, **kwargs)
    def IsDefault(*args, **kwargs): return _controls.TextAttr_IsDefault(*args, **kwargs)
    Combine = staticmethod(_controls.TextAttr_Combine)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TextAttrPtr(TextAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextAttr
_controls.TextAttr_swigregister(TextAttrPtr)

TextAttr_Combine = _controls.TextAttr_Combine

class TextCtrl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TextCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.TextCtrl_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.TextCtrl_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.TextCtrl_SetValue(*args, **kwargs)
    def GetRange(*args, **kwargs): return _controls.TextCtrl_GetRange(*args, **kwargs)
    def GetLineLength(*args, **kwargs): return _controls.TextCtrl_GetLineLength(*args, **kwargs)
    def GetLineText(*args, **kwargs): return _controls.TextCtrl_GetLineText(*args, **kwargs)
    def GetNumberOfLines(*args, **kwargs): return _controls.TextCtrl_GetNumberOfLines(*args, **kwargs)
    def IsModified(*args, **kwargs): return _controls.TextCtrl_IsModified(*args, **kwargs)
    def IsEditable(*args, **kwargs): return _controls.TextCtrl_IsEditable(*args, **kwargs)
    def IsSingleLine(*args, **kwargs): return _controls.TextCtrl_IsSingleLine(*args, **kwargs)
    def IsMultiLine(*args, **kwargs): return _controls.TextCtrl_IsMultiLine(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _controls.TextCtrl_GetSelection(*args, **kwargs)
    def GetStringSelection(*args, **kwargs): return _controls.TextCtrl_GetStringSelection(*args, **kwargs)
    def Clear(*args, **kwargs): return _controls.TextCtrl_Clear(*args, **kwargs)
    def Replace(*args, **kwargs): return _controls.TextCtrl_Replace(*args, **kwargs)
    def Remove(*args, **kwargs): return _controls.TextCtrl_Remove(*args, **kwargs)
    def LoadFile(*args, **kwargs): return _controls.TextCtrl_LoadFile(*args, **kwargs)
    def SaveFile(*args, **kwargs): return _controls.TextCtrl_SaveFile(*args, **kwargs)
    def MarkDirty(*args, **kwargs): return _controls.TextCtrl_MarkDirty(*args, **kwargs)
    def DiscardEdits(*args, **kwargs): return _controls.TextCtrl_DiscardEdits(*args, **kwargs)
    def SetMaxLength(*args, **kwargs): return _controls.TextCtrl_SetMaxLength(*args, **kwargs)
    def WriteText(*args, **kwargs): return _controls.TextCtrl_WriteText(*args, **kwargs)
    def AppendText(*args, **kwargs): return _controls.TextCtrl_AppendText(*args, **kwargs)
    def EmulateKeyPress(*args, **kwargs): return _controls.TextCtrl_EmulateKeyPress(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _controls.TextCtrl_SetStyle(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _controls.TextCtrl_GetStyle(*args, **kwargs)
    def SetDefaultStyle(*args, **kwargs): return _controls.TextCtrl_SetDefaultStyle(*args, **kwargs)
    def GetDefaultStyle(*args, **kwargs): return _controls.TextCtrl_GetDefaultStyle(*args, **kwargs)
    def XYToPosition(*args, **kwargs): return _controls.TextCtrl_XYToPosition(*args, **kwargs)
    def PositionToXY(*args, **kwargs): return _controls.TextCtrl_PositionToXY(*args, **kwargs)
    def ShowPosition(*args, **kwargs): return _controls.TextCtrl_ShowPosition(*args, **kwargs)
    def Copy(*args, **kwargs): return _controls.TextCtrl_Copy(*args, **kwargs)
    def Cut(*args, **kwargs): return _controls.TextCtrl_Cut(*args, **kwargs)
    def Paste(*args, **kwargs): return _controls.TextCtrl_Paste(*args, **kwargs)
    def CanCopy(*args, **kwargs): return _controls.TextCtrl_CanCopy(*args, **kwargs)
    def CanCut(*args, **kwargs): return _controls.TextCtrl_CanCut(*args, **kwargs)
    def CanPaste(*args, **kwargs): return _controls.TextCtrl_CanPaste(*args, **kwargs)
    def Undo(*args, **kwargs): return _controls.TextCtrl_Undo(*args, **kwargs)
    def Redo(*args, **kwargs): return _controls.TextCtrl_Redo(*args, **kwargs)
    def CanUndo(*args, **kwargs): return _controls.TextCtrl_CanUndo(*args, **kwargs)
    def CanRedo(*args, **kwargs): return _controls.TextCtrl_CanRedo(*args, **kwargs)
    def SetInsertionPoint(*args, **kwargs): return _controls.TextCtrl_SetInsertionPoint(*args, **kwargs)
    def SetInsertionPointEnd(*args, **kwargs): return _controls.TextCtrl_SetInsertionPointEnd(*args, **kwargs)
    def GetInsertionPoint(*args, **kwargs): return _controls.TextCtrl_GetInsertionPoint(*args, **kwargs)
    def GetLastPosition(*args, **kwargs): return _controls.TextCtrl_GetLastPosition(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.TextCtrl_SetSelection(*args, **kwargs)
    def SelectAll(*args, **kwargs): return _controls.TextCtrl_SelectAll(*args, **kwargs)
    def SetEditable(*args, **kwargs): return _controls.TextCtrl_SetEditable(*args, **kwargs)
    def ShowNativeCaret(*args, **kwargs): return _controls.TextCtrl_ShowNativeCaret(*args, **kwargs)
    def HideNativeCaret(*args, **kwargs): return _controls.TextCtrl_HideNativeCaret(*args, **kwargs)
    def write(*args, **kwargs): return _controls.TextCtrl_write(*args, **kwargs)
    def GetString(*args, **kwargs): return _controls.TextCtrl_GetString(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TextCtrlPtr(TextCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TextCtrl
_controls.TextCtrl_swigregister(TextCtrlPtr)

def PreTextCtrl(*args, **kwargs):
    val = _controls.new_PreTextCtrl(*args, **kwargs)
    val.thisown = 1
    return val

wxEVT_COMMAND_TEXT_UPDATED = _controls.wxEVT_COMMAND_TEXT_UPDATED
wxEVT_COMMAND_TEXT_ENTER = _controls.wxEVT_COMMAND_TEXT_ENTER
wxEVT_COMMAND_TEXT_URL = _controls.wxEVT_COMMAND_TEXT_URL
wxEVT_COMMAND_TEXT_MAXLEN = _controls.wxEVT_COMMAND_TEXT_MAXLEN
class TextUrlEvent(core.CommandEvent):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TextUrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetMouseEvent(*args, **kwargs): return _controls.TextUrlEvent_GetMouseEvent(*args, **kwargs)
    def GetURLStart(*args, **kwargs): return _controls.TextUrlEvent_GetURLStart(*args, **kwargs)
    def GetURLEnd(*args, **kwargs): return _controls.TextUrlEvent_GetURLEnd(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTextUrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ScrollBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ScrollBar_Create(*args, **kwargs)
    def GetThumbPosition(*args, **kwargs): return _controls.ScrollBar_GetThumbPosition(*args, **kwargs)
    def GetThumbSize(*args, **kwargs): return _controls.ScrollBar_GetThumbSize(*args, **kwargs)
    GetThumbLength = GetThumbSize 
    def GetPageSize(*args, **kwargs): return _controls.ScrollBar_GetPageSize(*args, **kwargs)
    def GetRange(*args, **kwargs): return _controls.ScrollBar_GetRange(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.ScrollBar_IsVertical(*args, **kwargs)
    def SetThumbPosition(*args, **kwargs): return _controls.ScrollBar_SetThumbPosition(*args, **kwargs)
    def SetScrollbar(*args, **kwargs): return _controls.ScrollBar_SetScrollbar(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScrollBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ScrollBarPtr(ScrollBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScrollBar
_controls.ScrollBar_swigregister(ScrollBarPtr)

def PreScrollBar(*args, **kwargs):
    val = _controls.new_PreScrollBar(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

SP_HORIZONTAL = _controls.SP_HORIZONTAL
SP_VERTICAL = _controls.SP_VERTICAL
SP_ARROW_KEYS = _controls.SP_ARROW_KEYS
SP_WRAP = _controls.SP_WRAP
class SpinButton(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_SpinButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.SpinButton_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.SpinButton_GetValue(*args, **kwargs)
    def GetMin(*args, **kwargs): return _controls.SpinButton_GetMin(*args, **kwargs)
    def GetMax(*args, **kwargs): return _controls.SpinButton_GetMax(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.SpinButton_SetValue(*args, **kwargs)
    def SetMin(*args, **kwargs): return _controls.SpinButton_SetMin(*args, **kwargs)
    def SetMax(*args, **kwargs): return _controls.SpinButton_SetMax(*args, **kwargs)
    def SetRange(*args, **kwargs): return _controls.SpinButton_SetRange(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.SpinButton_IsVertical(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class SpinButtonPtr(SpinButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinButton
_controls.SpinButton_swigregister(SpinButtonPtr)

def PreSpinButton(*args, **kwargs):
    val = _controls.new_PreSpinButton(*args, **kwargs)
    val.thisown = 1
    return val

class SpinCtrl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_SpinCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.SpinCtrl_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.SpinCtrl_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.SpinCtrl_SetValue(*args, **kwargs)
    def SetValueString(*args, **kwargs): return _controls.SpinCtrl_SetValueString(*args, **kwargs)
    def SetRange(*args, **kwargs): return _controls.SpinCtrl_SetRange(*args, **kwargs)
    def GetMin(*args, **kwargs): return _controls.SpinCtrl_GetMin(*args, **kwargs)
    def GetMax(*args, **kwargs): return _controls.SpinCtrl_GetMax(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.SpinCtrl_SetSelection(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSpinCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class SpinCtrlPtr(SpinCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SpinCtrl
_controls.SpinCtrl_swigregister(SpinCtrlPtr)

def PreSpinCtrl(*args, **kwargs):
    val = _controls.new_PreSpinCtrl(*args, **kwargs)
    val.thisown = 1
    return val

wxEVT_COMMAND_SPINCTRL_UPDATED = _controls.wxEVT_COMMAND_SPINCTRL_UPDATED
EVT_SPINCTRL = wx.PyEventBinder( wxEVT_COMMAND_SPINCTRL_UPDATED, 1)

#---------------------------------------------------------------------------

class RadioBox(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_RadioBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.RadioBox_Create(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.RadioBox_SetSelection(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _controls.RadioBox_GetSelection(*args, **kwargs)
    def GetStringSelection(*args, **kwargs): return _controls.RadioBox_GetStringSelection(*args, **kwargs)
    def SetStringSelection(*args, **kwargs): return _controls.RadioBox_SetStringSelection(*args, **kwargs)
    def GetCount(*args, **kwargs): return _controls.RadioBox_GetCount(*args, **kwargs)
    def FindString(*args, **kwargs): return _controls.RadioBox_FindString(*args, **kwargs)
    def GetString(*args, **kwargs): return _controls.RadioBox_GetString(*args, **kwargs)
    def SetString(*args, **kwargs): return _controls.RadioBox_SetString(*args, **kwargs)
    GetItemLabel = GetString 
    SetItemLabel = SetString 
    def EnableItem(*args, **kwargs): return _controls.RadioBox_EnableItem(*args, **kwargs)
    def ShowItem(*args, **kwargs): return _controls.RadioBox_ShowItem(*args, **kwargs)
    def GetColumnCount(*args, **kwargs): return _controls.RadioBox_GetColumnCount(*args, **kwargs)
    def GetRowCount(*args, **kwargs): return _controls.RadioBox_GetRowCount(*args, **kwargs)
    def GetNextItem(*args, **kwargs): return _controls.RadioBox_GetNextItem(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class RadioBoxPtr(RadioBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioBox
_controls.RadioBox_swigregister(RadioBoxPtr)

def PreRadioBox(*args, **kwargs):
    val = _controls.new_PreRadioBox(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class RadioButton(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_RadioButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.RadioButton_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.RadioButton_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.RadioButton_SetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRadioButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class RadioButtonPtr(RadioButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RadioButton
_controls.RadioButton_swigregister(RadioButtonPtr)

def PreRadioButton(*args, **kwargs):
    val = _controls.new_PreRadioButton(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Slider(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Slider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Slider_Create(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.Slider_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.Slider_SetValue(*args, **kwargs)
    def SetRange(*args, **kwargs): return _controls.Slider_SetRange(*args, **kwargs)
    def GetMin(*args, **kwargs): return _controls.Slider_GetMin(*args, **kwargs)
    def GetMax(*args, **kwargs): return _controls.Slider_GetMax(*args, **kwargs)
    def SetMin(*args, **kwargs): return _controls.Slider_SetMin(*args, **kwargs)
    def SetMax(*args, **kwargs): return _controls.Slider_SetMax(*args, **kwargs)
    def SetLineSize(*args, **kwargs): return _controls.Slider_SetLineSize(*args, **kwargs)
    def SetPageSize(*args, **kwargs): return _controls.Slider_SetPageSize(*args, **kwargs)
    def GetLineSize(*args, **kwargs): return _controls.Slider_GetLineSize(*args, **kwargs)
    def GetPageSize(*args, **kwargs): return _controls.Slider_GetPageSize(*args, **kwargs)
    def SetThumbLength(*args, **kwargs): return _controls.Slider_SetThumbLength(*args, **kwargs)
    def GetThumbLength(*args, **kwargs): return _controls.Slider_GetThumbLength(*args, **kwargs)
    def SetTickFreq(*args, **kwargs): return _controls.Slider_SetTickFreq(*args, **kwargs)
    def GetTickFreq(*args, **kwargs): return _controls.Slider_GetTickFreq(*args, **kwargs)
    def ClearTicks(*args, **kwargs): return _controls.Slider_ClearTicks(*args, **kwargs)
    def SetTick(*args, **kwargs): return _controls.Slider_SetTick(*args, **kwargs)
    def ClearSel(*args, **kwargs): return _controls.Slider_ClearSel(*args, **kwargs)
    def GetSelEnd(*args, **kwargs): return _controls.Slider_GetSelEnd(*args, **kwargs)
    def GetSelStart(*args, **kwargs): return _controls.Slider_GetSelStart(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.Slider_SetSelection(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSlider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class SliderPtr(Slider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Slider
_controls.Slider_swigregister(SliderPtr)

def PreSlider(*args, **kwargs):
    val = _controls.new_PreSlider(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

wxEVT_COMMAND_TOGGLEBUTTON_CLICKED = _controls.wxEVT_COMMAND_TOGGLEBUTTON_CLICKED
EVT_TOGGLEBUTTON = wx.PyEventBinder( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, 1)

class ToggleButton(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ToggleButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ToggleButton_Create(*args, **kwargs)
    def SetValue(*args, **kwargs): return _controls.ToggleButton_SetValue(*args, **kwargs)
    def GetValue(*args, **kwargs): return _controls.ToggleButton_GetValue(*args, **kwargs)
    def SetLabel(*args, **kwargs): return _controls.ToggleButton_SetLabel(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToggleButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ToggleButtonPtr(ToggleButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToggleButton
_controls.ToggleButton_swigregister(ToggleButtonPtr)

def PreToggleButton(*args, **kwargs):
    val = _controls.new_PreToggleButton(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class BookCtrl(core.Control):
    def GetPageCount(*args, **kwargs): return _controls.BookCtrl_GetPageCount(*args, **kwargs)
    def GetPage(*args, **kwargs): return _controls.BookCtrl_GetPage(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _controls.BookCtrl_GetSelection(*args, **kwargs)
    def SetPageText(*args, **kwargs): return _controls.BookCtrl_SetPageText(*args, **kwargs)
    def GetPageText(*args, **kwargs): return _controls.BookCtrl_GetPageText(*args, **kwargs)
    def SetImageList(*args, **kwargs): return _controls.BookCtrl_SetImageList(*args, **kwargs)
    def AssignImageList(*args, **kwargs): 
        val = _controls.BookCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def GetImageList(*args, **kwargs): return _controls.BookCtrl_GetImageList(*args, **kwargs)
    def GetPageImage(*args, **kwargs): return _controls.BookCtrl_GetPageImage(*args, **kwargs)
    def SetPageImage(*args, **kwargs): return _controls.BookCtrl_SetPageImage(*args, **kwargs)
    def SetPageSize(*args, **kwargs): return _controls.BookCtrl_SetPageSize(*args, **kwargs)
    def CalcSizeFromPage(*args, **kwargs): return _controls.BookCtrl_CalcSizeFromPage(*args, **kwargs)
    def DeletePage(*args, **kwargs): return _controls.BookCtrl_DeletePage(*args, **kwargs)
    def RemovePage(*args, **kwargs): return _controls.BookCtrl_RemovePage(*args, **kwargs)
    def DeleteAllPages(*args, **kwargs): return _controls.BookCtrl_DeleteAllPages(*args, **kwargs)
    def AddPage(*args, **kwargs): return _controls.BookCtrl_AddPage(*args, **kwargs)
    def InsertPage(*args, **kwargs): return _controls.BookCtrl_InsertPage(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.BookCtrl_SetSelection(*args, **kwargs)
    def AdvanceSelection(*args, **kwargs): return _controls.BookCtrl_AdvanceSelection(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BookCtrlPtr(BookCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrl
_controls.BookCtrl_swigregister(BookCtrlPtr)

class BookCtrlEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_BookCtrlEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetSelection(*args, **kwargs): return _controls.BookCtrlEvent_GetSelection(*args, **kwargs)
    def SetSelection(*args, **kwargs): return _controls.BookCtrlEvent_SetSelection(*args, **kwargs)
    def GetOldSelection(*args, **kwargs): return _controls.BookCtrlEvent_GetOldSelection(*args, **kwargs)
    def SetOldSelection(*args, **kwargs): return _controls.BookCtrlEvent_SetOldSelection(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BookCtrlEventPtr(BookCtrlEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlEvent
_controls.BookCtrlEvent_swigregister(BookCtrlEventPtr)

#---------------------------------------------------------------------------

wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = _controls.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = _controls.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Notebook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Notebook_Create(*args, **kwargs)
    def GetRowCount(*args, **kwargs): return _controls.Notebook_GetRowCount(*args, **kwargs)
    def SetPadding(*args, **kwargs): return _controls.Notebook_SetPadding(*args, **kwargs)
    def SetTabSize(*args, **kwargs): return _controls.Notebook_SetTabSize(*args, **kwargs)
    def HitTest(*args, **kwargs): return _controls.Notebook_HitTest(*args, **kwargs)
    def CalcSizeFromPage(*args, **kwargs): return _controls.Notebook_CalcSizeFromPage(*args, **kwargs)
    def ApplyThemeBackground(*args, **kwargs): return _controls.Notebook_ApplyThemeBackground(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class NotebookPtr(Notebook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Notebook
_controls.Notebook_swigregister(NotebookPtr)

def PreNotebook(*args, **kwargs):
    val = _controls.new_PreNotebook(*args, **kwargs)
    val.thisown = 1
    return val

class NotebookEvent(BookCtrlEvent):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_NotebookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class NotebookEventPtr(NotebookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NotebookEvent
_controls.NotebookEvent_swigregister(NotebookEventPtr)

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
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = _controls.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = _controls.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING
class Listbook(BookCtrl):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_Listbook(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.Listbook_Create(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.Listbook_IsVertical(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListbookPtr(Listbook):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Listbook
_controls.Listbook_swigregister(ListbookPtr)

def PreListbook(*args, **kwargs):
    val = _controls.new_PreListbook(*args, **kwargs)
    val.thisown = 1
    return val

class ListbookEvent(BookCtrlEvent):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListbookEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListbookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListbookEventPtr(ListbookEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListbookEvent
_controls.ListbookEvent_swigregister(ListbookEventPtr)

EVT_LISTBOOK_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, 1 )
EVT_LISTBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, 1 )

#---------------------------------------------------------------------------

class BookCtrlSizer(core.Sizer):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_BookCtrlSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def RecalcSizes(*args, **kwargs): return _controls.BookCtrlSizer_RecalcSizes(*args, **kwargs)
    def CalcMin(*args, **kwargs): return _controls.BookCtrlSizer_CalcMin(*args, **kwargs)
    def GetControl(*args, **kwargs): return _controls.BookCtrlSizer_GetControl(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBookCtrlSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BookCtrlSizerPtr(BookCtrlSizer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BookCtrlSizer
_controls.BookCtrlSizer_swigregister(BookCtrlSizerPtr)

class NotebookSizer(core.Sizer):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_NotebookSizer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def RecalcSizes(*args, **kwargs): return _controls.NotebookSizer_RecalcSizes(*args, **kwargs)
    def CalcMin(*args, **kwargs): return _controls.NotebookSizer_CalcMin(*args, **kwargs)
    def GetNotebook(*args, **kwargs): return _controls.NotebookSizer_GetNotebook(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNotebookSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def GetId(*args, **kwargs): return _controls.ToolBarToolBase_GetId(*args, **kwargs)
    def GetControl(*args, **kwargs): return _controls.ToolBarToolBase_GetControl(*args, **kwargs)
    def GetToolBar(*args, **kwargs): return _controls.ToolBarToolBase_GetToolBar(*args, **kwargs)
    def IsButton(*args, **kwargs): return _controls.ToolBarToolBase_IsButton(*args, **kwargs)
    def IsControl(*args, **kwargs): return _controls.ToolBarToolBase_IsControl(*args, **kwargs)
    def IsSeparator(*args, **kwargs): return _controls.ToolBarToolBase_IsSeparator(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _controls.ToolBarToolBase_GetStyle(*args, **kwargs)
    def GetKind(*args, **kwargs): return _controls.ToolBarToolBase_GetKind(*args, **kwargs)
    def IsEnabled(*args, **kwargs): return _controls.ToolBarToolBase_IsEnabled(*args, **kwargs)
    def IsToggled(*args, **kwargs): return _controls.ToolBarToolBase_IsToggled(*args, **kwargs)
    def CanBeToggled(*args, **kwargs): return _controls.ToolBarToolBase_CanBeToggled(*args, **kwargs)
    def GetNormalBitmap(*args, **kwargs): return _controls.ToolBarToolBase_GetNormalBitmap(*args, **kwargs)
    def GetDisabledBitmap(*args, **kwargs): return _controls.ToolBarToolBase_GetDisabledBitmap(*args, **kwargs)
    def GetBitmap(*args, **kwargs): return _controls.ToolBarToolBase_GetBitmap(*args, **kwargs)
    def GetLabel(*args, **kwargs): return _controls.ToolBarToolBase_GetLabel(*args, **kwargs)
    def GetShortHelp(*args, **kwargs): return _controls.ToolBarToolBase_GetShortHelp(*args, **kwargs)
    def GetLongHelp(*args, **kwargs): return _controls.ToolBarToolBase_GetLongHelp(*args, **kwargs)
    def Enable(*args, **kwargs): return _controls.ToolBarToolBase_Enable(*args, **kwargs)
    def Toggle(*args, **kwargs): return _controls.ToolBarToolBase_Toggle(*args, **kwargs)
    def SetToggle(*args, **kwargs): return _controls.ToolBarToolBase_SetToggle(*args, **kwargs)
    def SetShortHelp(*args, **kwargs): return _controls.ToolBarToolBase_SetShortHelp(*args, **kwargs)
    def SetLongHelp(*args, **kwargs): return _controls.ToolBarToolBase_SetLongHelp(*args, **kwargs)
    def SetNormalBitmap(*args, **kwargs): return _controls.ToolBarToolBase_SetNormalBitmap(*args, **kwargs)
    def SetDisabledBitmap(*args, **kwargs): return _controls.ToolBarToolBase_SetDisabledBitmap(*args, **kwargs)
    def SetLabel(*args, **kwargs): return _controls.ToolBarToolBase_SetLabel(*args, **kwargs)
    def Detach(*args, **kwargs): return _controls.ToolBarToolBase_Detach(*args, **kwargs)
    def Attach(*args, **kwargs): return _controls.ToolBarToolBase_Attach(*args, **kwargs)
    def GetClientData(*args, **kwargs): return _controls.ToolBarToolBase_GetClientData(*args, **kwargs)
    def SetClientData(*args, **kwargs): return _controls.ToolBarToolBase_SetClientData(*args, **kwargs)
    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap

    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarToolBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ToolBarToolBasePtr(ToolBarToolBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarToolBase
_controls.ToolBarToolBase_swigregister(ToolBarToolBasePtr)

class ToolBarBase(core.Control):
    def DoAddTool(*args, **kwargs): return _controls.ToolBarBase_DoAddTool(*args, **kwargs)
    def DoInsertTool(*args, **kwargs): return _controls.ToolBarBase_DoInsertTool(*args, **kwargs)
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

    def AddControl(*args, **kwargs): return _controls.ToolBarBase_AddControl(*args, **kwargs)
    def InsertControl(*args, **kwargs): return _controls.ToolBarBase_InsertControl(*args, **kwargs)
    def FindControl(*args, **kwargs): return _controls.ToolBarBase_FindControl(*args, **kwargs)
    def AddSeparator(*args, **kwargs): return _controls.ToolBarBase_AddSeparator(*args, **kwargs)
    def InsertSeparator(*args, **kwargs): return _controls.ToolBarBase_InsertSeparator(*args, **kwargs)
    def RemoveTool(*args, **kwargs): return _controls.ToolBarBase_RemoveTool(*args, **kwargs)
    def DeleteToolByPos(*args, **kwargs): return _controls.ToolBarBase_DeleteToolByPos(*args, **kwargs)
    def DeleteTool(*args, **kwargs): return _controls.ToolBarBase_DeleteTool(*args, **kwargs)
    def ClearTools(*args, **kwargs): return _controls.ToolBarBase_ClearTools(*args, **kwargs)
    def Realize(*args, **kwargs): return _controls.ToolBarBase_Realize(*args, **kwargs)
    def EnableTool(*args, **kwargs): return _controls.ToolBarBase_EnableTool(*args, **kwargs)
    def ToggleTool(*args, **kwargs): return _controls.ToolBarBase_ToggleTool(*args, **kwargs)
    def SetToggle(*args, **kwargs): return _controls.ToolBarBase_SetToggle(*args, **kwargs)
    def GetToolClientData(*args, **kwargs): return _controls.ToolBarBase_GetToolClientData(*args, **kwargs)
    def SetToolClientData(*args, **kwargs): return _controls.ToolBarBase_SetToolClientData(*args, **kwargs)
    def GetToolPos(*args, **kwargs): return _controls.ToolBarBase_GetToolPos(*args, **kwargs)
    def GetToolState(*args, **kwargs): return _controls.ToolBarBase_GetToolState(*args, **kwargs)
    def GetToolEnabled(*args, **kwargs): return _controls.ToolBarBase_GetToolEnabled(*args, **kwargs)
    def SetToolShortHelp(*args, **kwargs): return _controls.ToolBarBase_SetToolShortHelp(*args, **kwargs)
    def GetToolShortHelp(*args, **kwargs): return _controls.ToolBarBase_GetToolShortHelp(*args, **kwargs)
    def SetToolLongHelp(*args, **kwargs): return _controls.ToolBarBase_SetToolLongHelp(*args, **kwargs)
    def GetToolLongHelp(*args, **kwargs): return _controls.ToolBarBase_GetToolLongHelp(*args, **kwargs)
    def SetMarginsXY(*args, **kwargs): return _controls.ToolBarBase_SetMarginsXY(*args, **kwargs)
    def SetMargins(*args, **kwargs): return _controls.ToolBarBase_SetMargins(*args, **kwargs)
    def SetToolPacking(*args, **kwargs): return _controls.ToolBarBase_SetToolPacking(*args, **kwargs)
    def SetToolSeparation(*args, **kwargs): return _controls.ToolBarBase_SetToolSeparation(*args, **kwargs)
    def GetToolMargins(*args, **kwargs): return _controls.ToolBarBase_GetToolMargins(*args, **kwargs)
    def GetMargins(*args, **kwargs): return _controls.ToolBarBase_GetMargins(*args, **kwargs)
    def GetToolPacking(*args, **kwargs): return _controls.ToolBarBase_GetToolPacking(*args, **kwargs)
    def GetToolSeparation(*args, **kwargs): return _controls.ToolBarBase_GetToolSeparation(*args, **kwargs)
    def SetRows(*args, **kwargs): return _controls.ToolBarBase_SetRows(*args, **kwargs)
    def SetMaxRowsCols(*args, **kwargs): return _controls.ToolBarBase_SetMaxRowsCols(*args, **kwargs)
    def GetMaxRows(*args, **kwargs): return _controls.ToolBarBase_GetMaxRows(*args, **kwargs)
    def GetMaxCols(*args, **kwargs): return _controls.ToolBarBase_GetMaxCols(*args, **kwargs)
    def SetToolBitmapSize(*args, **kwargs): return _controls.ToolBarBase_SetToolBitmapSize(*args, **kwargs)
    def GetToolBitmapSize(*args, **kwargs): return _controls.ToolBarBase_GetToolBitmapSize(*args, **kwargs)
    def GetToolSize(*args, **kwargs): return _controls.ToolBarBase_GetToolSize(*args, **kwargs)
    def FindToolForPosition(*args, **kwargs): return _controls.ToolBarBase_FindToolForPosition(*args, **kwargs)
    def IsVertical(*args, **kwargs): return _controls.ToolBarBase_IsVertical(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBarBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ToolBarBasePtr(ToolBarBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBarBase
_controls.ToolBarBase_swigregister(ToolBarBasePtr)

class ToolBar(ToolBarBase):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ToolBar(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ToolBar_Create(*args, **kwargs)
    def FindToolForPosition(*args, **kwargs): return _controls.ToolBar_FindToolForPosition(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxToolBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ToolBarPtr(ToolBar):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ToolBar
_controls.ToolBar_swigregister(ToolBarPtr)

def PreToolBar(*args, **kwargs):
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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListItemAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetTextColour(*args, **kwargs): return _controls.ListItemAttr_SetTextColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _controls.ListItemAttr_SetBackgroundColour(*args, **kwargs)
    def SetFont(*args, **kwargs): return _controls.ListItemAttr_SetFont(*args, **kwargs)
    def HasTextColour(*args, **kwargs): return _controls.ListItemAttr_HasTextColour(*args, **kwargs)
    def HasBackgroundColour(*args, **kwargs): return _controls.ListItemAttr_HasBackgroundColour(*args, **kwargs)
    def HasFont(*args, **kwargs): return _controls.ListItemAttr_HasFont(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _controls.ListItemAttr_GetTextColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _controls.ListItemAttr_GetBackgroundColour(*args, **kwargs)
    def GetFont(*args, **kwargs): return _controls.ListItemAttr_GetFont(*args, **kwargs)
    def Destroy(*args, **kwargs): return _controls.ListItemAttr_Destroy(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItemAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListItemAttrPtr(ListItemAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItemAttr
_controls.ListItemAttr_swigregister(ListItemAttrPtr)

#---------------------------------------------------------------------------

class ListItem(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListItem(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_ListItem):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Clear(*args, **kwargs): return _controls.ListItem_Clear(*args, **kwargs)
    def ClearAttributes(*args, **kwargs): return _controls.ListItem_ClearAttributes(*args, **kwargs)
    def SetMask(*args, **kwargs): return _controls.ListItem_SetMask(*args, **kwargs)
    def SetId(*args, **kwargs): return _controls.ListItem_SetId(*args, **kwargs)
    def SetColumn(*args, **kwargs): return _controls.ListItem_SetColumn(*args, **kwargs)
    def SetState(*args, **kwargs): return _controls.ListItem_SetState(*args, **kwargs)
    def SetStateMask(*args, **kwargs): return _controls.ListItem_SetStateMask(*args, **kwargs)
    def SetText(*args, **kwargs): return _controls.ListItem_SetText(*args, **kwargs)
    def SetImage(*args, **kwargs): return _controls.ListItem_SetImage(*args, **kwargs)
    def SetData(*args, **kwargs): return _controls.ListItem_SetData(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _controls.ListItem_SetWidth(*args, **kwargs)
    def SetAlign(*args, **kwargs): return _controls.ListItem_SetAlign(*args, **kwargs)
    def SetTextColour(*args, **kwargs): return _controls.ListItem_SetTextColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _controls.ListItem_SetBackgroundColour(*args, **kwargs)
    def SetFont(*args, **kwargs): return _controls.ListItem_SetFont(*args, **kwargs)
    def GetMask(*args, **kwargs): return _controls.ListItem_GetMask(*args, **kwargs)
    def GetId(*args, **kwargs): return _controls.ListItem_GetId(*args, **kwargs)
    def GetColumn(*args, **kwargs): return _controls.ListItem_GetColumn(*args, **kwargs)
    def GetState(*args, **kwargs): return _controls.ListItem_GetState(*args, **kwargs)
    def GetText(*args, **kwargs): return _controls.ListItem_GetText(*args, **kwargs)
    def GetImage(*args, **kwargs): return _controls.ListItem_GetImage(*args, **kwargs)
    def GetData(*args, **kwargs): return _controls.ListItem_GetData(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _controls.ListItem_GetWidth(*args, **kwargs)
    def GetAlign(*args, **kwargs): return _controls.ListItem_GetAlign(*args, **kwargs)
    def GetAttributes(*args, **kwargs): return _controls.ListItem_GetAttributes(*args, **kwargs)
    def HasAttributes(*args, **kwargs): return _controls.ListItem_HasAttributes(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _controls.ListItem_GetTextColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _controls.ListItem_GetBackgroundColour(*args, **kwargs)
    def GetFont(*args, **kwargs): return _controls.ListItem_GetFont(*args, **kwargs)
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
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListItemPtr(ListItem):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListItem
_controls.ListItem_swigregister(ListItemPtr)

#---------------------------------------------------------------------------

class ListEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
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
    def GetKeyCode(*args, **kwargs): return _controls.ListEvent_GetKeyCode(*args, **kwargs)
    GetCode = GetKeyCode 
    def GetIndex(*args, **kwargs): return _controls.ListEvent_GetIndex(*args, **kwargs)
    def GetColumn(*args, **kwargs): return _controls.ListEvent_GetColumn(*args, **kwargs)
    def GetPoint(*args, **kwargs): return _controls.ListEvent_GetPoint(*args, **kwargs)
    GetPostiion = GetPoint 
    def GetLabel(*args, **kwargs): return _controls.ListEvent_GetLabel(*args, **kwargs)
    def GetText(*args, **kwargs): return _controls.ListEvent_GetText(*args, **kwargs)
    def GetImage(*args, **kwargs): return _controls.ListEvent_GetImage(*args, **kwargs)
    def GetData(*args, **kwargs): return _controls.ListEvent_GetData(*args, **kwargs)
    def GetMask(*args, **kwargs): return _controls.ListEvent_GetMask(*args, **kwargs)
    def GetItem(*args, **kwargs): return _controls.ListEvent_GetItem(*args, **kwargs)
    def GetCacheFrom(*args, **kwargs): return _controls.ListEvent_GetCacheFrom(*args, **kwargs)
    def GetCacheTo(*args, **kwargs): return _controls.ListEvent_GetCacheTo(*args, **kwargs)
    def IsEditCancelled(*args, **kwargs): return _controls.ListEvent_IsEditCancelled(*args, **kwargs)
    def SetEditCanceled(*args, **kwargs): return _controls.ListEvent_SetEditCanceled(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, ListCtrl)
    def Create(*args, **kwargs): return _controls.ListCtrl_Create(*args, **kwargs)
    def _setCallbackInfo(*args, **kwargs): return _controls.ListCtrl__setCallbackInfo(*args, **kwargs)
    def SetForegroundColour(*args, **kwargs): return _controls.ListCtrl_SetForegroundColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _controls.ListCtrl_SetBackgroundColour(*args, **kwargs)
    def GetColumn(*args, **kwargs): 
        val = _controls.ListCtrl_GetColumn(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val
    def SetColumn(*args, **kwargs): return _controls.ListCtrl_SetColumn(*args, **kwargs)
    def GetColumnWidth(*args, **kwargs): return _controls.ListCtrl_GetColumnWidth(*args, **kwargs)
    def SetColumnWidth(*args, **kwargs): return _controls.ListCtrl_SetColumnWidth(*args, **kwargs)
    def GetCountPerPage(*args, **kwargs): return _controls.ListCtrl_GetCountPerPage(*args, **kwargs)
    def GetViewRect(*args, **kwargs): return _controls.ListCtrl_GetViewRect(*args, **kwargs)
    def GetEditControl(*args, **kwargs): return _controls.ListCtrl_GetEditControl(*args, **kwargs)
    def GetItem(*args, **kwargs): 
        val = _controls.ListCtrl_GetItem(*args, **kwargs)
        if val is not None: val.thisown = 1
        return val
    def SetItem(*args, **kwargs): return _controls.ListCtrl_SetItem(*args, **kwargs)
    def SetStringItem(*args, **kwargs): return _controls.ListCtrl_SetStringItem(*args, **kwargs)
    def GetItemState(*args, **kwargs): return _controls.ListCtrl_GetItemState(*args, **kwargs)
    def SetItemState(*args, **kwargs): return _controls.ListCtrl_SetItemState(*args, **kwargs)
    def SetItemImage(*args, **kwargs): return _controls.ListCtrl_SetItemImage(*args, **kwargs)
    def GetItemText(*args, **kwargs): return _controls.ListCtrl_GetItemText(*args, **kwargs)
    def SetItemText(*args, **kwargs): return _controls.ListCtrl_SetItemText(*args, **kwargs)
    def GetItemData(*args, **kwargs): return _controls.ListCtrl_GetItemData(*args, **kwargs)
    def SetItemData(*args, **kwargs): return _controls.ListCtrl_SetItemData(*args, **kwargs)
    def GetItemPosition(*args, **kwargs): return _controls.ListCtrl_GetItemPosition(*args, **kwargs)
    def GetItemRect(*args, **kwargs): return _controls.ListCtrl_GetItemRect(*args, **kwargs)
    def SetItemPosition(*args, **kwargs): return _controls.ListCtrl_SetItemPosition(*args, **kwargs)
    def GetItemCount(*args, **kwargs): return _controls.ListCtrl_GetItemCount(*args, **kwargs)
    def GetColumnCount(*args, **kwargs): return _controls.ListCtrl_GetColumnCount(*args, **kwargs)
    def GetItemSpacing(*args, **kwargs): return _controls.ListCtrl_GetItemSpacing(*args, **kwargs)
    def GetSelectedItemCount(*args, **kwargs): return _controls.ListCtrl_GetSelectedItemCount(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _controls.ListCtrl_GetTextColour(*args, **kwargs)
    def SetTextColour(*args, **kwargs): return _controls.ListCtrl_SetTextColour(*args, **kwargs)
    def GetTopItem(*args, **kwargs): return _controls.ListCtrl_GetTopItem(*args, **kwargs)
    def SetSingleStyle(*args, **kwargs): return _controls.ListCtrl_SetSingleStyle(*args, **kwargs)
    def SetWindowStyleFlag(*args, **kwargs): return _controls.ListCtrl_SetWindowStyleFlag(*args, **kwargs)
    def GetNextItem(*args, **kwargs): return _controls.ListCtrl_GetNextItem(*args, **kwargs)
    def GetImageList(*args, **kwargs): return _controls.ListCtrl_GetImageList(*args, **kwargs)
    def SetImageList(*args, **kwargs): return _controls.ListCtrl_SetImageList(*args, **kwargs)
    def AssignImageList(*args, **kwargs): 
        val = _controls.ListCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def IsVirtual(*args, **kwargs): return _controls.ListCtrl_IsVirtual(*args, **kwargs)
    def RefreshItem(*args, **kwargs): return _controls.ListCtrl_RefreshItem(*args, **kwargs)
    def RefreshItems(*args, **kwargs): return _controls.ListCtrl_RefreshItems(*args, **kwargs)
    def Arrange(*args, **kwargs): return _controls.ListCtrl_Arrange(*args, **kwargs)
    def DeleteItem(*args, **kwargs): return _controls.ListCtrl_DeleteItem(*args, **kwargs)
    def DeleteAllItems(*args, **kwargs): return _controls.ListCtrl_DeleteAllItems(*args, **kwargs)
    def DeleteColumn(*args, **kwargs): return _controls.ListCtrl_DeleteColumn(*args, **kwargs)
    def DeleteAllColumns(*args, **kwargs): return _controls.ListCtrl_DeleteAllColumns(*args, **kwargs)
    def ClearAll(*args, **kwargs): return _controls.ListCtrl_ClearAll(*args, **kwargs)
    def EditLabel(*args, **kwargs): return _controls.ListCtrl_EditLabel(*args, **kwargs)
    def EndEditLabel(*args, **kwargs): return _controls.ListCtrl_EndEditLabel(*args, **kwargs)
    def EnsureVisible(*args, **kwargs): return _controls.ListCtrl_EnsureVisible(*args, **kwargs)
    def FindItem(*args, **kwargs): return _controls.ListCtrl_FindItem(*args, **kwargs)
    def FindItemData(*args, **kwargs): return _controls.ListCtrl_FindItemData(*args, **kwargs)
    def FindItemAtPos(*args, **kwargs): return _controls.ListCtrl_FindItemAtPos(*args, **kwargs)
    def HitTest(*args, **kwargs): return _controls.ListCtrl_HitTest(*args, **kwargs)
    def InsertItem(*args, **kwargs): return _controls.ListCtrl_InsertItem(*args, **kwargs)
    def InsertStringItem(*args, **kwargs): return _controls.ListCtrl_InsertStringItem(*args, **kwargs)
    def InsertImageItem(*args, **kwargs): return _controls.ListCtrl_InsertImageItem(*args, **kwargs)
    def InsertImageStringItem(*args, **kwargs): return _controls.ListCtrl_InsertImageStringItem(*args, **kwargs)
    def InsertColumnInfo(*args, **kwargs): return _controls.ListCtrl_InsertColumnInfo(*args, **kwargs)
    def InsertColumn(*args, **kwargs): return _controls.ListCtrl_InsertColumn(*args, **kwargs)
    def SetItemCount(*args, **kwargs): return _controls.ListCtrl_SetItemCount(*args, **kwargs)
    def ScrollList(*args, **kwargs): return _controls.ListCtrl_ScrollList(*args, **kwargs)
    def SetItemTextColour(*args, **kwargs): return _controls.ListCtrl_SetItemTextColour(*args, **kwargs)
    def GetItemTextColour(*args, **kwargs): return _controls.ListCtrl_GetItemTextColour(*args, **kwargs)
    def SetItemBackgroundColour(*args, **kwargs): return _controls.ListCtrl_SetItemBackgroundColour(*args, **kwargs)
    def GetItemBackgroundColour(*args, **kwargs): return _controls.ListCtrl_GetItemBackgroundColour(*args, **kwargs)
    #
    # Some helpers...
    def Select(self, idx, on=1):
        '''[de]select an item'''
        if on: state = wxLIST_STATE_SELECTED
        else: state = 0
        self.SetItemState(idx, state, wxLIST_STATE_SELECTED)

    def Focus(self, idx):
        '''Focus and show the given item'''
        self.SetItemState(idx, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED)
        self.EnsureVisible(idx)

    def GetFocusedItem(self):
        '''get the currently focused item or -1 if none'''
        return self.GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED)

    def GetFirstSelected(self, *args):
        '''return first selected item, or -1 when none'''
        return self.GetNextSelected(-1)

    def GetNextSelected(self, item):
        '''return subsequent selected items, or -1 when no more'''
        return self.GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)

    def IsSelected(self, idx):
        '''return TRUE if the item is selected'''
        return self.GetItemState(idx, wxLIST_STATE_SELECTED) != 0

    def SetColumnImage(self, col, image):
        item = self.GetColumn(col)
        
        item.SetMask( wxLIST_MASK_STATE |
                      wxLIST_MASK_TEXT  |
                      wxLIST_MASK_IMAGE |
                      wxLIST_MASK_DATA  |
                      wxLIST_SET_ITEM   |
                      wxLIST_MASK_WIDTH |
                      wxLIST_MASK_FORMAT )
        item.SetImage(image)
        self.SetColumn(col, item)

    def ClearColumnImage(self, col):
        self.SetColumnImage(col, -1)

    def Append(self, entry):
        '''Append an item to the list control.  The entry parameter should be a
           sequence with an item for each column'''
        if len(entry):
            if wx.wxUSE_UNICODE:
                cvtfunc = unicode
            else:
                cvtfunc = str
            pos = self.GetItemCount()
            self.InsertStringItem(pos, cvtfunc(entry[0]))
            for i in range(1, len(entry)):
                self.SetStringItem(pos, i, cvtfunc(entry[i]))
            return pos

    def SortItems(*args, **kwargs): return _controls.ListCtrl_SortItems(*args, **kwargs)
    def GetMainWindow(*args, **kwargs): return _controls.ListCtrl_GetMainWindow(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListCtrlPtr(ListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListCtrl
_controls.ListCtrl_swigregister(ListCtrlPtr)

def PreListCtrl(*args, **kwargs):
    val = _controls.new_PreListCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class ListView(ListCtrl):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ListView(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.ListView_Create(*args, **kwargs)
    def Select(*args, **kwargs): return _controls.ListView_Select(*args, **kwargs)
    def Focus(*args, **kwargs): return _controls.ListView_Focus(*args, **kwargs)
    def GetFocusedItem(*args, **kwargs): return _controls.ListView_GetFocusedItem(*args, **kwargs)
    def GetNextSelected(*args, **kwargs): return _controls.ListView_GetNextSelected(*args, **kwargs)
    def GetFirstSelected(*args, **kwargs): return _controls.ListView_GetFirstSelected(*args, **kwargs)
    def IsSelected(*args, **kwargs): return _controls.ListView_IsSelected(*args, **kwargs)
    def SetColumnImage(*args, **kwargs): return _controls.ListView_SetColumnImage(*args, **kwargs)
    def ClearColumnImage(*args, **kwargs): return _controls.ListView_ClearColumnImage(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxListView instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ListViewPtr(ListView):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ListView
_controls.ListView_swigregister(ListViewPtr)

def PreListView(*args, **kwargs):
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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TreeItemId(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_TreeItemId):
        try:
            if self.thisown: destroy(self)
        except: pass
    def IsOk(*args, **kwargs): return _controls.TreeItemId_IsOk(*args, **kwargs)
    def __eq__(*args, **kwargs): return _controls.TreeItemId___eq__(*args, **kwargs)
    def __ne__(*args, **kwargs): return _controls.TreeItemId___ne__(*args, **kwargs)
    m_pItem = property(_controls.TreeItemId_m_pItem_get, _controls.TreeItemId_m_pItem_set)
    Ok = IsOk
    def __nonzero__(self): return self.IsOk() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeItemId instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeItemIdPtr(TreeItemId):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeItemId
_controls.TreeItemId_swigregister(TreeItemIdPtr)

class TreeItemData(object):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TreeItemData(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetData(*args, **kwargs): return _controls.TreeItemData_GetData(*args, **kwargs)
    def SetData(*args, **kwargs): return _controls.TreeItemData_SetData(*args, **kwargs)
    def GetId(*args, **kwargs): return _controls.TreeItemData_GetId(*args, **kwargs)
    def SetId(*args, **kwargs): return _controls.TreeItemData_SetId(*args, **kwargs)
    def Destroy(*args, **kwargs): return _controls.TreeItemData_Destroy(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeItemData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TreeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetItem(*args, **kwargs): return _controls.TreeEvent_GetItem(*args, **kwargs)
    def SetItem(*args, **kwargs): return _controls.TreeEvent_SetItem(*args, **kwargs)
    def GetOldItem(*args, **kwargs): return _controls.TreeEvent_GetOldItem(*args, **kwargs)
    def SetOldItem(*args, **kwargs): return _controls.TreeEvent_SetOldItem(*args, **kwargs)
    def GetPoint(*args, **kwargs): return _controls.TreeEvent_GetPoint(*args, **kwargs)
    def SetPoint(*args, **kwargs): return _controls.TreeEvent_SetPoint(*args, **kwargs)
    def GetKeyEvent(*args, **kwargs): return _controls.TreeEvent_GetKeyEvent(*args, **kwargs)
    def GetKeyCode(*args, **kwargs): return _controls.TreeEvent_GetKeyCode(*args, **kwargs)
    def SetKeyEvent(*args, **kwargs): return _controls.TreeEvent_SetKeyEvent(*args, **kwargs)
    def GetLabel(*args, **kwargs): return _controls.TreeEvent_GetLabel(*args, **kwargs)
    def SetLabel(*args, **kwargs): return _controls.TreeEvent_SetLabel(*args, **kwargs)
    def IsEditCancelled(*args, **kwargs): return _controls.TreeEvent_IsEditCancelled(*args, **kwargs)
    def SetEditCanceled(*args, **kwargs): return _controls.TreeEvent_SetEditCanceled(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeEventPtr(TreeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeEvent
_controls.TreeEvent_swigregister(TreeEventPtr)

#---------------------------------------------------------------------------

class TreeCtrl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_TreeCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeCtrl)
    def Create(*args, **kwargs): return _controls.TreeCtrl_Create(*args, **kwargs)
    def _setCallbackInfo(*args, **kwargs): return _controls.TreeCtrl__setCallbackInfo(*args, **kwargs)
    def GetCount(*args, **kwargs): return _controls.TreeCtrl_GetCount(*args, **kwargs)
    def GetIndent(*args, **kwargs): return _controls.TreeCtrl_GetIndent(*args, **kwargs)
    def SetIndent(*args, **kwargs): return _controls.TreeCtrl_SetIndent(*args, **kwargs)
    def GetSpacing(*args, **kwargs): return _controls.TreeCtrl_GetSpacing(*args, **kwargs)
    def SetSpacing(*args, **kwargs): return _controls.TreeCtrl_SetSpacing(*args, **kwargs)
    def GetImageList(*args, **kwargs): return _controls.TreeCtrl_GetImageList(*args, **kwargs)
    def GetStateImageList(*args, **kwargs): return _controls.TreeCtrl_GetStateImageList(*args, **kwargs)
    def SetImageList(*args, **kwargs): return _controls.TreeCtrl_SetImageList(*args, **kwargs)
    def SetStateImageList(*args, **kwargs): return _controls.TreeCtrl_SetStateImageList(*args, **kwargs)
    def AssignImageList(*args, **kwargs): 
        val = _controls.TreeCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def AssignStateImageList(*args, **kwargs): 
        val = _controls.TreeCtrl_AssignStateImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def GetItemText(*args, **kwargs): return _controls.TreeCtrl_GetItemText(*args, **kwargs)
    def GetItemImage(*args, **kwargs): return _controls.TreeCtrl_GetItemImage(*args, **kwargs)
    def GetItemData(*args, **kwargs): return _controls.TreeCtrl_GetItemData(*args, **kwargs)
    def GetItemPyData(*args, **kwargs): return _controls.TreeCtrl_GetItemPyData(*args, **kwargs)
    GetPyData = GetItemPyData 
    def GetItemTextColour(*args, **kwargs): return _controls.TreeCtrl_GetItemTextColour(*args, **kwargs)
    def GetItemBackgroundColour(*args, **kwargs): return _controls.TreeCtrl_GetItemBackgroundColour(*args, **kwargs)
    def GetItemFont(*args, **kwargs): return _controls.TreeCtrl_GetItemFont(*args, **kwargs)
    def SetItemText(*args, **kwargs): return _controls.TreeCtrl_SetItemText(*args, **kwargs)
    def SetItemImage(*args, **kwargs): return _controls.TreeCtrl_SetItemImage(*args, **kwargs)
    def SetItemData(*args, **kwargs): return _controls.TreeCtrl_SetItemData(*args, **kwargs)
    def SetItemPyData(*args, **kwargs): return _controls.TreeCtrl_SetItemPyData(*args, **kwargs)
    SetPyData = SetItemPyData 
    def SetItemHasChildren(*args, **kwargs): return _controls.TreeCtrl_SetItemHasChildren(*args, **kwargs)
    def SetItemBold(*args, **kwargs): return _controls.TreeCtrl_SetItemBold(*args, **kwargs)
    def SetItemDropHighlight(*args, **kwargs): return _controls.TreeCtrl_SetItemDropHighlight(*args, **kwargs)
    def SetItemTextColour(*args, **kwargs): return _controls.TreeCtrl_SetItemTextColour(*args, **kwargs)
    def SetItemBackgroundColour(*args, **kwargs): return _controls.TreeCtrl_SetItemBackgroundColour(*args, **kwargs)
    def SetItemFont(*args, **kwargs): return _controls.TreeCtrl_SetItemFont(*args, **kwargs)
    def IsVisible(*args, **kwargs): return _controls.TreeCtrl_IsVisible(*args, **kwargs)
    def ItemHasChildren(*args, **kwargs): return _controls.TreeCtrl_ItemHasChildren(*args, **kwargs)
    def IsExpanded(*args, **kwargs): return _controls.TreeCtrl_IsExpanded(*args, **kwargs)
    def IsSelected(*args, **kwargs): return _controls.TreeCtrl_IsSelected(*args, **kwargs)
    def IsBold(*args, **kwargs): return _controls.TreeCtrl_IsBold(*args, **kwargs)
    def GetChildrenCount(*args, **kwargs): return _controls.TreeCtrl_GetChildrenCount(*args, **kwargs)
    def GetRootItem(*args, **kwargs): return _controls.TreeCtrl_GetRootItem(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _controls.TreeCtrl_GetSelection(*args, **kwargs)
    def GetSelections(*args, **kwargs): return _controls.TreeCtrl_GetSelections(*args, **kwargs)
    def GetItemParent(*args, **kwargs): return _controls.TreeCtrl_GetItemParent(*args, **kwargs)
    def GetFirstChild(*args, **kwargs): return _controls.TreeCtrl_GetFirstChild(*args, **kwargs)
    def GetNextChild(*args, **kwargs): return _controls.TreeCtrl_GetNextChild(*args, **kwargs)
    def GetLastChild(*args, **kwargs): return _controls.TreeCtrl_GetLastChild(*args, **kwargs)
    def GetNextSibling(*args, **kwargs): return _controls.TreeCtrl_GetNextSibling(*args, **kwargs)
    def GetPrevSibling(*args, **kwargs): return _controls.TreeCtrl_GetPrevSibling(*args, **kwargs)
    def GetFirstVisibleItem(*args, **kwargs): return _controls.TreeCtrl_GetFirstVisibleItem(*args, **kwargs)
    def GetNextVisible(*args, **kwargs): return _controls.TreeCtrl_GetNextVisible(*args, **kwargs)
    def GetPrevVisible(*args, **kwargs): return _controls.TreeCtrl_GetPrevVisible(*args, **kwargs)
    def AddRoot(*args, **kwargs): return _controls.TreeCtrl_AddRoot(*args, **kwargs)
    def PrependItem(*args, **kwargs): return _controls.TreeCtrl_PrependItem(*args, **kwargs)
    def InsertItem(*args, **kwargs): return _controls.TreeCtrl_InsertItem(*args, **kwargs)
    def InsertItemBefore(*args, **kwargs): return _controls.TreeCtrl_InsertItemBefore(*args, **kwargs)
    def AppendItem(*args, **kwargs): return _controls.TreeCtrl_AppendItem(*args, **kwargs)
    def Delete(*args, **kwargs): return _controls.TreeCtrl_Delete(*args, **kwargs)
    def DeleteChildren(*args, **kwargs): return _controls.TreeCtrl_DeleteChildren(*args, **kwargs)
    def DeleteAllItems(*args, **kwargs): return _controls.TreeCtrl_DeleteAllItems(*args, **kwargs)
    def Expand(*args, **kwargs): return _controls.TreeCtrl_Expand(*args, **kwargs)
    def Collapse(*args, **kwargs): return _controls.TreeCtrl_Collapse(*args, **kwargs)
    def CollapseAndReset(*args, **kwargs): return _controls.TreeCtrl_CollapseAndReset(*args, **kwargs)
    def Toggle(*args, **kwargs): return _controls.TreeCtrl_Toggle(*args, **kwargs)
    def Unselect(*args, **kwargs): return _controls.TreeCtrl_Unselect(*args, **kwargs)
    def UnselectAll(*args, **kwargs): return _controls.TreeCtrl_UnselectAll(*args, **kwargs)
    def SelectItem(*args, **kwargs): return _controls.TreeCtrl_SelectItem(*args, **kwargs)
    def EnsureVisible(*args, **kwargs): return _controls.TreeCtrl_EnsureVisible(*args, **kwargs)
    def ScrollTo(*args, **kwargs): return _controls.TreeCtrl_ScrollTo(*args, **kwargs)
    def EditLabel(*args, **kwargs): return _controls.TreeCtrl_EditLabel(*args, **kwargs)
    def GetEditControl(*args, **kwargs): return _controls.TreeCtrl_GetEditControl(*args, **kwargs)
    def EndEditLabel(*args, **kwargs): return _controls.TreeCtrl_EndEditLabel(*args, **kwargs)
    def SortChildren(*args, **kwargs): return _controls.TreeCtrl_SortChildren(*args, **kwargs)
    def HitTest(*args, **kwargs): return _controls.TreeCtrl_HitTest(*args, **kwargs)
    def GetBoundingRect(*args, **kwargs): return _controls.TreeCtrl_GetBoundingRect(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeCtrlPtr(TreeCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeCtrl
_controls.TreeCtrl_swigregister(TreeCtrlPtr)

def PreTreeCtrl(*args, **kwargs):
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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_GenericDirCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.GenericDirCtrl_Create(*args, **kwargs)
    def ExpandPath(*args, **kwargs): return _controls.GenericDirCtrl_ExpandPath(*args, **kwargs)
    def GetDefaultPath(*args, **kwargs): return _controls.GenericDirCtrl_GetDefaultPath(*args, **kwargs)
    def SetDefaultPath(*args, **kwargs): return _controls.GenericDirCtrl_SetDefaultPath(*args, **kwargs)
    def GetPath(*args, **kwargs): return _controls.GenericDirCtrl_GetPath(*args, **kwargs)
    def GetFilePath(*args, **kwargs): return _controls.GenericDirCtrl_GetFilePath(*args, **kwargs)
    def SetPath(*args, **kwargs): return _controls.GenericDirCtrl_SetPath(*args, **kwargs)
    def ShowHidden(*args, **kwargs): return _controls.GenericDirCtrl_ShowHidden(*args, **kwargs)
    def GetShowHidden(*args, **kwargs): return _controls.GenericDirCtrl_GetShowHidden(*args, **kwargs)
    def GetFilter(*args, **kwargs): return _controls.GenericDirCtrl_GetFilter(*args, **kwargs)
    def SetFilter(*args, **kwargs): return _controls.GenericDirCtrl_SetFilter(*args, **kwargs)
    def GetFilterIndex(*args, **kwargs): return _controls.GenericDirCtrl_GetFilterIndex(*args, **kwargs)
    def SetFilterIndex(*args, **kwargs): return _controls.GenericDirCtrl_SetFilterIndex(*args, **kwargs)
    def GetRootId(*args, **kwargs): return _controls.GenericDirCtrl_GetRootId(*args, **kwargs)
    def GetTreeCtrl(*args, **kwargs): return _controls.GenericDirCtrl_GetTreeCtrl(*args, **kwargs)
    def GetFilterListCtrl(*args, **kwargs): return _controls.GenericDirCtrl_GetFilterListCtrl(*args, **kwargs)
    def FindChild(*args, **kwargs): return _controls.GenericDirCtrl_FindChild(*args, **kwargs)
    def DoResize(*args, **kwargs): return _controls.GenericDirCtrl_DoResize(*args, **kwargs)
    def ReCreateTree(*args, **kwargs): return _controls.GenericDirCtrl_ReCreateTree(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDirCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GenericDirCtrlPtr(GenericDirCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GenericDirCtrl
_controls.GenericDirCtrl_swigregister(GenericDirCtrlPtr)

def PreGenericDirCtrl(*args, **kwargs):
    val = _controls.new_PreGenericDirCtrl(*args, **kwargs)
    val.thisown = 1
    return val

class DirFilterListCtrl(Choice):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_DirFilterListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _controls.DirFilterListCtrl_Create(*args, **kwargs)
    def FillFilterList(*args, **kwargs): return _controls.DirFilterListCtrl_FillFilterList(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDirFilterListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DirFilterListCtrlPtr(DirFilterListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DirFilterListCtrl
_controls.DirFilterListCtrl_swigregister(DirFilterListCtrlPtr)

def PreDirFilterListCtrl(*args, **kwargs):
    val = _controls.new_PreDirFilterListCtrl(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class PyControl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_PyControl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self); self._setCallbackInfo(self, PyControl)
    def _setCallbackInfo(*args, **kwargs): return _controls.PyControl__setCallbackInfo(*args, **kwargs)
    def base_DoMoveWindow(*args, **kwargs): return _controls.PyControl_base_DoMoveWindow(*args, **kwargs)
    def base_DoSetSize(*args, **kwargs): return _controls.PyControl_base_DoSetSize(*args, **kwargs)
    def base_DoSetClientSize(*args, **kwargs): return _controls.PyControl_base_DoSetClientSize(*args, **kwargs)
    def base_DoSetVirtualSize(*args, **kwargs): return _controls.PyControl_base_DoSetVirtualSize(*args, **kwargs)
    def base_DoGetSize(*args, **kwargs): return _controls.PyControl_base_DoGetSize(*args, **kwargs)
    def base_DoGetClientSize(*args, **kwargs): return _controls.PyControl_base_DoGetClientSize(*args, **kwargs)
    def base_DoGetPosition(*args, **kwargs): return _controls.PyControl_base_DoGetPosition(*args, **kwargs)
    def base_DoGetVirtualSize(*args, **kwargs): return _controls.PyControl_base_DoGetVirtualSize(*args, **kwargs)
    def base_DoGetBestSize(*args, **kwargs): return _controls.PyControl_base_DoGetBestSize(*args, **kwargs)
    def base_InitDialog(*args, **kwargs): return _controls.PyControl_base_InitDialog(*args, **kwargs)
    def base_TransferDataToWindow(*args, **kwargs): return _controls.PyControl_base_TransferDataToWindow(*args, **kwargs)
    def base_TransferDataFromWindow(*args, **kwargs): return _controls.PyControl_base_TransferDataFromWindow(*args, **kwargs)
    def base_Validate(*args, **kwargs): return _controls.PyControl_base_Validate(*args, **kwargs)
    def base_AcceptsFocus(*args, **kwargs): return _controls.PyControl_base_AcceptsFocus(*args, **kwargs)
    def base_AcceptsFocusFromKeyboard(*args, **kwargs): return _controls.PyControl_base_AcceptsFocusFromKeyboard(*args, **kwargs)
    def base_GetMaxSize(*args, **kwargs): return _controls.PyControl_base_GetMaxSize(*args, **kwargs)
    def base_AddChild(*args, **kwargs): return _controls.PyControl_base_AddChild(*args, **kwargs)
    def base_RemoveChild(*args, **kwargs): return _controls.PyControl_base_RemoveChild(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

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
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_HelpEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPosition(*args, **kwargs): return _controls.HelpEvent_GetPosition(*args, **kwargs)
    def SetPosition(*args, **kwargs): return _controls.HelpEvent_SetPosition(*args, **kwargs)
    def GetLink(*args, **kwargs): return _controls.HelpEvent_GetLink(*args, **kwargs)
    def SetLink(*args, **kwargs): return _controls.HelpEvent_SetLink(*args, **kwargs)
    def GetTarget(*args, **kwargs): return _controls.HelpEvent_GetTarget(*args, **kwargs)
    def SetTarget(*args, **kwargs): return _controls.HelpEvent_SetTarget(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HelpEventPtr(HelpEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpEvent
_controls.HelpEvent_swigregister(HelpEventPtr)

class ContextHelp(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ContextHelp(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_controls.delete_ContextHelp):
        try:
            if self.thisown: destroy(self)
        except: pass
    def BeginContextHelp(*args, **kwargs): return _controls.ContextHelp_BeginContextHelp(*args, **kwargs)
    def EndContextHelp(*args, **kwargs): return _controls.ContextHelp_EndContextHelp(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelp instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ContextHelpPtr(ContextHelp):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelp
_controls.ContextHelp_swigregister(ContextHelpPtr)

class ContextHelpButton(BitmapButton):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_ContextHelpButton(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxContextHelpButton instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ContextHelpButtonPtr(ContextHelpButton):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ContextHelpButton
_controls.ContextHelpButton_swigregister(ContextHelpButtonPtr)

class HelpProvider(object):
    Set = staticmethod(_controls.HelpProvider_Set)
    Get = staticmethod(_controls.HelpProvider_Get)
    def GetHelp(*args, **kwargs): return _controls.HelpProvider_GetHelp(*args, **kwargs)
    def ShowHelp(*args, **kwargs): return _controls.HelpProvider_ShowHelp(*args, **kwargs)
    def AddHelp(*args, **kwargs): return _controls.HelpProvider_AddHelp(*args, **kwargs)
    def AddHelpById(*args, **kwargs): return _controls.HelpProvider_AddHelpById(*args, **kwargs)
    def Destroy(*args, **kwargs): return _controls.HelpProvider_Destroy(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class HelpProviderPtr(HelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = HelpProvider
_controls.HelpProvider_swigregister(HelpProviderPtr)

HelpProvider_Set = _controls.HelpProvider_Set

HelpProvider_Get = _controls.HelpProvider_Get

class SimpleHelpProvider(HelpProvider):
    def __init__(self, *args, **kwargs):
        newobj = _controls.new_SimpleHelpProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSimpleHelpProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class SimpleHelpProviderPtr(SimpleHelpProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SimpleHelpProvider
_controls.SimpleHelpProvider_swigregister(SimpleHelpProviderPtr)


