# This file was created automatically by SWIG.
import controlsc

from misc import *

from windows import *

from gdi import *

from clip_dnd import *

from events import *
import wx
class wxControlPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxControl_GetLabel,(self,) + _args, _kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxControl_SetLabel,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxControl instance at %s>" % (self.this,)
class wxControl(wxControlPtr):
    def __init__(self,this):
        self.this = this




class wxButtonPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetDefault(self, *_args, **_kwargs):
        val = apply(controlsc.wxButton_SetDefault,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxButton instance at %s>" % (self.this,)
class wxButton(wxButtonPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxButton,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxBitmapButtonPtr(wxButtonPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBitmapLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_GetBitmapLabel,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetBitmapDisabled(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_GetBitmapDisabled,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetBitmapFocus(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_GetBitmapFocus,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetBitmapSelected(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_GetBitmapSelected,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def SetBitmapDisabled(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_SetBitmapDisabled,(self,) + _args, _kwargs)
        return val
    def SetBitmapFocus(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_SetBitmapFocus,(self,) + _args, _kwargs)
        return val
    def SetBitmapSelected(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_SetBitmapSelected,(self,) + _args, _kwargs)
        return val
    def SetBitmapLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxBitmapButton_SetBitmapLabel,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBitmapButton instance at %s>" % (self.this,)
class wxBitmapButton(wxBitmapButtonPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxBitmapButton,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxCheckBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckBox_GetValue,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckBox_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCheckBox instance at %s>" % (self.this,)
class wxCheckBox(wxCheckBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxCheckBox,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxChoicePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_Append,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_Clear,(self,) + _args, _kwargs)
        return val
    def FindString(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_FindString,(self,) + _args, _kwargs)
        return val
    def GetColumns(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_GetColumns,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_GetString,(self,) + _args, _kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_GetStringSelection,(self,) + _args, _kwargs)
        return val
    def Number(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_Number,(self,) + _args, _kwargs)
        return val
    def SetColumns(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_SetColumns,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxChoice_SetStringSelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxChoice instance at %s>" % (self.this,)
class wxChoice(wxChoicePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxChoice,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxComboBoxPtr(wxChoicePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Append,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Clear,(self,) + _args, _kwargs)
        return val
    def Copy(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Copy,(self,) + _args, _kwargs)
        return val
    def Cut(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Cut,(self,) + _args, _kwargs)
        return val
    def Delete(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Delete,(self,) + _args, _kwargs)
        return val
    def FindString(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_FindString,(self,) + _args, _kwargs)
        return val
    def GetInsertionPoint(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetInsertionPoint,(self,) + _args, _kwargs)
        return val
    def GetLastPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetLastPosition,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetString,(self,) + _args, _kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetStringSelection,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_GetValue,(self,) + _args, _kwargs)
        return val
    def Number(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Number,(self,) + _args, _kwargs)
        return val
    def Paste(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Paste,(self,) + _args, _kwargs)
        return val
    def Replace(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Replace,(self,) + _args, _kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_Remove,(self,) + _args, _kwargs)
        return val
    def SetInsertionPoint(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_SetInsertionPoint,(self,) + _args, _kwargs)
        return val
    def SetInsertionPointEnd(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_SetInsertionPointEnd,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetMark(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_SetMark,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxComboBox_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxComboBox instance at %s>" % (self.this,)
class wxComboBox(wxComboBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxComboBox,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxGaugePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBezelFace(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_GetBezelFace,(self,) + _args, _kwargs)
        return val
    def GetRange(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_GetRange,(self,) + _args, _kwargs)
        return val
    def GetShadowWidth(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_GetShadowWidth,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_GetValue,(self,) + _args, _kwargs)
        return val
    def SetBezelFace(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_SetBezelFace,(self,) + _args, _kwargs)
        return val
    def SetRange(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_SetRange,(self,) + _args, _kwargs)
        return val
    def SetShadowWidth(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_SetShadowWidth,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxGauge_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxGauge instance at %s>" % (self.this,)
class wxGauge(wxGaugePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxGauge,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxStaticBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxStaticBox instance at %s>" % (self.this,)
class wxStaticBox(wxStaticBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxStaticBox,_args,_kwargs)
        self.thisown = 1




class wxStaticLinePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxStaticLine instance at %s>" % (self.this,)
class wxStaticLine(wxStaticLinePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxStaticLine,_args,_kwargs)
        self.thisown = 1




class wxStaticTextPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxStaticText_GetLabel,(self,) + _args, _kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxStaticText_SetLabel,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStaticText instance at %s>" % (self.this,)
class wxStaticText(wxStaticTextPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxStaticText,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxListBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Append,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Clear,(self,) + _args, _kwargs)
        return val
    def Delete(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Delete,(self,) + _args, _kwargs)
        return val
    def Deselect(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Deselect,(self,) + _args, _kwargs)
        return val
    def FindString(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_FindString,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetSelections(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_GetSelections,(self,) + _args, _kwargs)
        return val
    def InsertItems(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_InsertItems,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_GetString,(self,) + _args, _kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_GetStringSelection,(self,) + _args, _kwargs)
        return val
    def Number(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Number,(self,) + _args, _kwargs)
        return val
    def Selected(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Selected,(self,) + _args, _kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_Set,(self,) + _args, _kwargs)
        return val
    def SetFirstItem(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_SetFirstItem,(self,) + _args, _kwargs)
        return val
    def SetFirstItemStr(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_SetFirstItemStr,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetString(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_SetString,(self,) + _args, _kwargs)
        return val
    def SetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxListBox_SetStringSelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxListBox instance at %s>" % (self.this,)
class wxListBox(wxListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxListBox,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxCheckListBoxPtr(wxListBoxPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def IsChecked(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckListBox_IsChecked,(self,) + _args, _kwargs)
        return val
    def Check(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckListBox_Check,(self,) + _args, _kwargs)
        return val
    def InsertItems(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckListBox_InsertItems,(self,) + _args, _kwargs)
        return val
    def GetItemHeight(self, *_args, **_kwargs):
        val = apply(controlsc.wxCheckListBox_GetItemHeight,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCheckListBox instance at %s>" % (self.this,)
class wxCheckListBox(wxCheckListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxCheckListBox,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxTextCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Clear(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Clear,(self,) + _args, _kwargs)
        return val
    def Copy(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Copy,(self,) + _args, _kwargs)
        return val
    def Cut(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Cut,(self,) + _args, _kwargs)
        return val
    def DiscardEdits(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_DiscardEdits,(self,) + _args, _kwargs)
        return val
    def GetInsertionPoint(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetInsertionPoint,(self,) + _args, _kwargs)
        return val
    def GetLastPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetLastPosition,(self,) + _args, _kwargs)
        return val
    def GetLineLength(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetLineLength,(self,) + _args, _kwargs)
        return val
    def GetLineText(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetLineText,(self,) + _args, _kwargs)
        return val
    def GetNumberOfLines(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetNumberOfLines,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetValue,(self,) + _args, _kwargs)
        return val
    def IsModified(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_IsModified,(self,) + _args, _kwargs)
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_LoadFile,(self,) + _args, _kwargs)
        return val
    def Paste(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Paste,(self,) + _args, _kwargs)
        return val
    def PositionToXY(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_PositionToXY,(self,) + _args, _kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Remove,(self,) + _args, _kwargs)
        return val
    def Replace(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Replace,(self,) + _args, _kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SaveFile,(self,) + _args, _kwargs)
        return val
    def SetEditable(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SetEditable,(self,) + _args, _kwargs)
        return val
    def SetInsertionPoint(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SetInsertionPoint,(self,) + _args, _kwargs)
        return val
    def SetInsertionPointEnd(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SetInsertionPointEnd,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_SetValue,(self,) + _args, _kwargs)
        return val
    def ShowPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_ShowPosition,(self,) + _args, _kwargs)
        return val
    def WriteText(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_WriteText,(self,) + _args, _kwargs)
        return val
    def AppendText(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_AppendText,(self,) + _args, _kwargs)
        return val
    def XYToPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_XYToPosition,(self,) + _args, _kwargs)
        return val
    def CanCopy(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_CanCopy,(self,) + _args, _kwargs)
        return val
    def CanCut(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_CanCut,(self,) + _args, _kwargs)
        return val
    def CanPaste(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_CanPaste,(self,) + _args, _kwargs)
        return val
    def CanRedo(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_CanRedo,(self,) + _args, _kwargs)
        return val
    def CanUndo(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_CanUndo,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_GetSelection,(self,) + _args, _kwargs)
        return val
    def IsEditable(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_IsEditable,(self,) + _args, _kwargs)
        return val
    def Undo(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Undo,(self,) + _args, _kwargs)
        return val
    def Redo(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_Redo,(self,) + _args, _kwargs)
        return val
    def write(self, *_args, **_kwargs):
        val = apply(controlsc.wxTextCtrl_write,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTextCtrl instance at %s>" % (self.this,)
class wxTextCtrl(wxTextCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxTextCtrl,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxScrollBarPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRange(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_GetRange,(self,) + _args, _kwargs)
        return val
    def GetPageSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_GetPageSize,(self,) + _args, _kwargs)
        return val
    def GetThumbPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_GetThumbPosition,(self,) + _args, _kwargs)
        return val
    def GetThumbSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_GetThumbSize,(self,) + _args, _kwargs)
        return val
    def SetThumbPosition(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_SetThumbPosition,(self,) + _args, _kwargs)
        return val
    def SetScrollbar(self, *_args, **_kwargs):
        val = apply(controlsc.wxScrollBar_SetScrollbar,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxScrollBar instance at %s>" % (self.this,)
class wxScrollBar(wxScrollBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxScrollBar,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxSpinButtonPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetMax(self, *_args, **_kwargs):
        val = apply(controlsc.wxSpinButton_GetMax,(self,) + _args, _kwargs)
        return val
    def GetMin(self, *_args, **_kwargs):
        val = apply(controlsc.wxSpinButton_GetMin,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxSpinButton_GetValue,(self,) + _args, _kwargs)
        return val
    def SetRange(self, *_args, **_kwargs):
        val = apply(controlsc.wxSpinButton_SetRange,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxSpinButton_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSpinButton instance at %s>" % (self.this,)
class wxSpinButton(wxSpinButtonPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxSpinButton,_args,_kwargs)
        self.thisown = 1




class wxStaticBitmapPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBitmap(self, *_args, **_kwargs):
        val = apply(controlsc.wxStaticBitmap_GetBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def SetBitmap(self, *_args, **_kwargs):
        val = apply(controlsc.wxStaticBitmap_SetBitmap,(self,) + _args, _kwargs)
        return val
    def SetIcon(self, *_args, **_kwargs):
        val = apply(controlsc.wxStaticBitmap_SetIcon,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStaticBitmap instance at %s>" % (self.this,)
class wxStaticBitmap(wxStaticBitmapPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxStaticBitmap,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxRadioBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Enable(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_Enable,(self,) + _args, _kwargs)
        return val
    def EnableItem(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_EnableItem,(self,) + _args, _kwargs)
        return val
    def FindString(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_FindString,(self,) + _args, _kwargs)
        return val
    def GetItemLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_GetItemLabel,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetString(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_GetString,(self,) + _args, _kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_GetStringSelection,(self,) + _args, _kwargs)
        return val
    def Number(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_Number,(self,) + _args, _kwargs)
        return val
    def SetItemLabel(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_SetItemLabel,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetStringSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_SetStringSelection,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_Show,(self,) + _args, _kwargs)
        return val
    def ShowItem(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioBox_ShowItem,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRadioBox instance at %s>" % (self.this,)
class wxRadioBox(wxRadioBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxRadioBox,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxRadioButtonPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioButton_GetValue,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxRadioButton_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRadioButton instance at %s>" % (self.this,)
class wxRadioButton(wxRadioButtonPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxRadioButton,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxSliderPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ClearSel(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_ClearSel,(self,) + _args, _kwargs)
        return val
    def ClearTicks(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_ClearTicks,(self,) + _args, _kwargs)
        return val
    def GetLineSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetLineSize,(self,) + _args, _kwargs)
        return val
    def GetMax(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetMax,(self,) + _args, _kwargs)
        return val
    def GetMin(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetMin,(self,) + _args, _kwargs)
        return val
    def GetPageSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetPageSize,(self,) + _args, _kwargs)
        return val
    def GetSelEnd(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetSelEnd,(self,) + _args, _kwargs)
        return val
    def GetSelStart(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetSelStart,(self,) + _args, _kwargs)
        return val
    def GetThumbLength(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetThumbLength,(self,) + _args, _kwargs)
        return val
    def GetTickFreq(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetTickFreq,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_GetValue,(self,) + _args, _kwargs)
        return val
    def SetRange(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetRange,(self,) + _args, _kwargs)
        return val
    def SetTickFreq(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetTickFreq,(self,) + _args, _kwargs)
        return val
    def SetLineSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetLineSize,(self,) + _args, _kwargs)
        return val
    def SetPageSize(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetPageSize,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetSelection,(self,) + _args, _kwargs)
        return val
    def SetThumbLength(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetThumbLength,(self,) + _args, _kwargs)
        return val
    def SetTick(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetTick,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(controlsc.wxSlider_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSlider instance at %s>" % (self.this,)
class wxSlider(wxSliderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(controlsc.new_wxSlider,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

cvar = controlsc.cvar
wxDefaultValidator = wxValidatorPtr(controlsc.cvar.wxDefaultValidator)
