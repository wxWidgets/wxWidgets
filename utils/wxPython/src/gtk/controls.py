# This file was created automatically by SWIG.
import controlsc

from misc import *

from windows import *

from gdi import *

from events import *
import wxp
class wxControlPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Command(self,arg0):
        val = controlsc.wxControl_Command(self.this,arg0.this)
        return val
    def GetLabel(self):
        val = controlsc.wxControl_GetLabel(self.this)
        return val
    def SetLabel(self,arg0):
        val = controlsc.wxControl_SetLabel(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxControl instance>"
class wxControl(wxControlPtr):
    def __init__(self,this):
        self.this = this




class wxButtonPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetDefault(self):
        val = controlsc.wxButton_SetDefault(self.this)
        return val
    def __repr__(self):
        return "<C wxButton instance>"
class wxButton(wxButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxButton,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxBitmapButtonPtr(wxButtonPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBitmapLabel(self):
        val = controlsc.wxBitmapButton_GetBitmapLabel(self.this)
        val = wxBitmapPtr(val)
        return val
    def SetBitmapLabel(self,arg0):
        val = controlsc.wxBitmapButton_SetBitmapLabel(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxBitmapButton instance>"
class wxBitmapButton(wxBitmapButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxBitmapButton,(arg0.this,arg1,arg2.this,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxCheckBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetValue(self):
        val = controlsc.wxCheckBox_GetValue(self.this)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxCheckBox_SetValue(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxCheckBox instance>"
class wxCheckBox(wxCheckBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxCheckBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxChoicePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self,arg0):
        val = controlsc.wxChoice_Append(self.this,arg0)
        return val
    def Clear(self):
        val = controlsc.wxChoice_Clear(self.this)
        return val
    def FindString(self,arg0):
        val = controlsc.wxChoice_FindString(self.this,arg0)
        return val
    def GetColumns(self):
        val = controlsc.wxChoice_GetColumns(self.this)
        return val
    def GetSelection(self):
        val = controlsc.wxChoice_GetSelection(self.this)
        return val
    def GetString(self,arg0):
        val = controlsc.wxChoice_GetString(self.this,arg0)
        return val
    def GetStringSelection(self):
        val = controlsc.wxChoice_GetStringSelection(self.this)
        return val
    def Number(self):
        val = controlsc.wxChoice_Number(self.this)
        return val
    def SetColumns(self,*args):
        val = apply(controlsc.wxChoice_SetColumns,(self.this,)+args)
        return val
    def SetSelection(self,arg0):
        val = controlsc.wxChoice_SetSelection(self.this,arg0)
        return val
    def SetStringSelection(self,arg0):
        val = controlsc.wxChoice_SetStringSelection(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxChoice instance>"
class wxChoice(wxChoicePtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxChoice,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxComboBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self,arg0):
        val = controlsc.wxComboBox_Append(self.this,arg0)
        return val
    def Clear(self):
        val = controlsc.wxComboBox_Clear(self.this)
        return val
    def Copy(self):
        val = controlsc.wxComboBox_Copy(self.this)
        return val
    def Cut(self):
        val = controlsc.wxComboBox_Cut(self.this)
        return val
    def Delete(self,arg0):
        val = controlsc.wxComboBox_Delete(self.this,arg0)
        return val
    def FindString(self,arg0):
        val = controlsc.wxComboBox_FindString(self.this,arg0)
        return val
    def GetInsertionPoint(self):
        val = controlsc.wxComboBox_GetInsertionPoint(self.this)
        return val
    def GetLastPosition(self):
        val = controlsc.wxComboBox_GetLastPosition(self.this)
        return val
    def GetSelection(self):
        val = controlsc.wxComboBox_GetSelection(self.this)
        return val
    def GetString(self,arg0):
        val = controlsc.wxComboBox_GetString(self.this,arg0)
        return val
    def GetStringSelection(self):
        val = controlsc.wxComboBox_GetStringSelection(self.this)
        return val
    def GetValue(self):
        val = controlsc.wxComboBox_GetValue(self.this)
        return val
    def Number(self):
        val = controlsc.wxComboBox_Number(self.this)
        return val
    def Paste(self):
        val = controlsc.wxComboBox_Paste(self.this)
        return val
    def Replace(self,arg0,arg1,arg2):
        val = controlsc.wxComboBox_Replace(self.this,arg0,arg1,arg2)
        return val
    def Remove(self,arg0,arg1):
        val = controlsc.wxComboBox_Remove(self.this,arg0,arg1)
        return val
    def SetInsertionPoint(self,arg0):
        val = controlsc.wxComboBox_SetInsertionPoint(self.this,arg0)
        return val
    def SetInsertionPointEnd(self):
        val = controlsc.wxComboBox_SetInsertionPointEnd(self.this)
        return val
    def SetSelection(self,arg0,*args):
        val = apply(controlsc.wxComboBox_SetSelection,(self.this,arg0,)+args)
        return val
    def SetMark(self,arg0,arg1):
        val = controlsc.wxComboBox_SetMark(self.this,arg0,arg1)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxComboBox_SetValue(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxComboBox instance>"
class wxComboBox(wxComboBoxPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxComboBox,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxGaugePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBezelFace(self):
        val = controlsc.wxGauge_GetBezelFace(self.this)
        return val
    def GetRange(self):
        val = controlsc.wxGauge_GetRange(self.this)
        return val
    def GetShadowWidth(self):
        val = controlsc.wxGauge_GetShadowWidth(self.this)
        return val
    def GetValue(self):
        val = controlsc.wxGauge_GetValue(self.this)
        return val
    def SetBezelFace(self,arg0):
        val = controlsc.wxGauge_SetBezelFace(self.this,arg0)
        return val
    def SetRange(self,arg0):
        val = controlsc.wxGauge_SetRange(self.this,arg0)
        return val
    def SetShadowWidth(self,arg0):
        val = controlsc.wxGauge_SetShadowWidth(self.this,arg0)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxGauge_SetValue(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxGauge instance>"
class wxGauge(wxGaugePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxGauge,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxStaticBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxStaticBox instance>"
class wxStaticBox(wxStaticBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1




class wxStaticTextPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetLabel(self):
        val = controlsc.wxStaticText_GetLabel(self.this)
        return val
    def SetLabel(self,arg0):
        val = controlsc.wxStaticText_SetLabel(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxStaticText instance>"
class wxStaticText(wxStaticTextPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticText,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxListBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self,arg0):
        val = controlsc.wxListBox_Append(self.this,arg0)
        return val
    def Clear(self):
        val = controlsc.wxListBox_Clear(self.this)
        return val
    def Delete(self,arg0):
        val = controlsc.wxListBox_Delete(self.this,arg0)
        return val
    def Deselect(self,arg0):
        val = controlsc.wxListBox_Deselect(self.this,arg0)
        return val
    def FindString(self,arg0):
        val = controlsc.wxListBox_FindString(self.this,arg0)
        return val
    def GetSelection(self):
        val = controlsc.wxListBox_GetSelection(self.this)
        return val
    def GetString(self,arg0):
        val = controlsc.wxListBox_GetString(self.this,arg0)
        return val
    def GetStringSelection(self):
        val = controlsc.wxListBox_GetStringSelection(self.this)
        return val
    def Number(self):
        val = controlsc.wxListBox_Number(self.this)
        return val
    def Selected(self,arg0):
        val = controlsc.wxListBox_Selected(self.this,arg0)
        return val
    def Set(self,arg0,*args):
        val = apply(controlsc.wxListBox_Set,(self.this,arg0,)+args)
        return val
    def SetFirstItem(self,arg0):
        val = controlsc.wxListBox_SetFirstItem(self.this,arg0)
        return val
    def SetFirstItemStr(self,arg0):
        val = controlsc.wxListBox_SetFirstItemStr(self.this,arg0)
        return val
    def SetSelection(self,arg0,*args):
        val = apply(controlsc.wxListBox_SetSelection,(self.this,arg0,)+args)
        return val
    def SetString(self,arg0,arg1):
        val = controlsc.wxListBox_SetString(self.this,arg0,arg1)
        return val
    def SetStringSelection(self,arg0,*args):
        val = apply(controlsc.wxListBox_SetStringSelection,(self.this,arg0,)+args)
        return val
    def __repr__(self):
        return "<C wxListBox instance>"
class wxListBox(wxListBoxPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxListBox,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxTextCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Clear(self):
        val = controlsc.wxTextCtrl_Clear(self.this)
        return val
    def Copy(self):
        val = controlsc.wxTextCtrl_Copy(self.this)
        return val
    def Cut(self):
        val = controlsc.wxTextCtrl_Cut(self.this)
        return val
    def DiscardEdits(self):
        val = controlsc.wxTextCtrl_DiscardEdits(self.this)
        return val
    def GetInsertionPoint(self):
        val = controlsc.wxTextCtrl_GetInsertionPoint(self.this)
        return val
    def GetLastPosition(self):
        val = controlsc.wxTextCtrl_GetLastPosition(self.this)
        return val
    def GetLineLength(self,arg0):
        val = controlsc.wxTextCtrl_GetLineLength(self.this,arg0)
        return val
    def GetLineText(self,arg0):
        val = controlsc.wxTextCtrl_GetLineText(self.this,arg0)
        return val
    def GetNumberOfLines(self):
        val = controlsc.wxTextCtrl_GetNumberOfLines(self.this)
        return val
    def GetValue(self):
        val = controlsc.wxTextCtrl_GetValue(self.this)
        return val
    def IsModified(self):
        val = controlsc.wxTextCtrl_IsModified(self.this)
        return val
    def LoadFile(self,arg0):
        val = controlsc.wxTextCtrl_LoadFile(self.this,arg0)
        return val
    def Paste(self):
        val = controlsc.wxTextCtrl_Paste(self.this)
        return val
    def PositionToXY(self,arg0):
        val = controlsc.wxTextCtrl_PositionToXY(self.this,arg0)
        return val
    def Remove(self,arg0,arg1):
        val = controlsc.wxTextCtrl_Remove(self.this,arg0,arg1)
        return val
    def Replace(self,arg0,arg1,arg2):
        val = controlsc.wxTextCtrl_Replace(self.this,arg0,arg1,arg2)
        return val
    def SaveFile(self,arg0):
        val = controlsc.wxTextCtrl_SaveFile(self.this,arg0)
        return val
    def SetEditable(self,arg0):
        val = controlsc.wxTextCtrl_SetEditable(self.this,arg0)
        return val
    def SetInsertionPoint(self,arg0):
        val = controlsc.wxTextCtrl_SetInsertionPoint(self.this,arg0)
        return val
    def SetInsertionPointEnd(self):
        val = controlsc.wxTextCtrl_SetInsertionPointEnd(self.this)
        return val
    def SetSelection(self,arg0,arg1):
        val = controlsc.wxTextCtrl_SetSelection(self.this,arg0,arg1)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxTextCtrl_SetValue(self.this,arg0)
        return val
    def ShowPosition(self,arg0):
        val = controlsc.wxTextCtrl_ShowPosition(self.this,arg0)
        return val
    def WriteText(self,arg0):
        val = controlsc.wxTextCtrl_WriteText(self.this,arg0)
        return val
    def XYToPosition(self,arg0,arg1):
        val = controlsc.wxTextCtrl_XYToPosition(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxTextCtrl instance>"
class wxTextCtrl(wxTextCtrlPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxTextCtrl,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxScrollBarPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRange(self):
        val = controlsc.wxScrollBar_GetRange(self.this)
        return val
    def GetPageSize(self):
        val = controlsc.wxScrollBar_GetPageSize(self.this)
        return val
    def GetPosition(self):
        val = controlsc.wxScrollBar_GetPosition(self.this)
        return val
    def GetThumbSize(self):
        val = controlsc.wxScrollBar_GetThumbSize(self.this)
        return val
    def SetPosition(self,arg0):
        val = controlsc.wxScrollBar_SetPosition(self.this,arg0)
        return val
    def SetScrollbar(self,arg0,arg1,arg2,arg3,*args):
        val = apply(controlsc.wxScrollBar_SetScrollbar,(self.this,arg0,arg1,arg2,arg3,)+args)
        return val
    def __repr__(self):
        return "<C wxScrollBar instance>"
class wxScrollBar(wxScrollBarPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxScrollBar,(arg0.this,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxStaticBitmapPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBitmap(self):
        val = controlsc.wxStaticBitmap_GetBitmap(self.this)
        val = wxBitmapPtr(val)
        return val
    def SetBitmap(self,arg0):
        val = controlsc.wxStaticBitmap_SetBitmap(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxStaticBitmap instance>"
class wxStaticBitmap(wxStaticBitmapPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxStaticBitmap,(arg0.this,arg1,arg2.this,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxRadioBoxPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def EnableBox(self,arg0):
        val = controlsc.wxRadioBox_EnableBox(self.this,arg0)
        return val
    def Enable(self,arg0,arg1):
        val = controlsc.wxRadioBox_Enable(self.this,arg0,arg1)
        return val
    def FindString(self,arg0):
        val = controlsc.wxRadioBox_FindString(self.this,arg0)
        return val
    def GetLabel(self,arg0):
        val = controlsc.wxRadioBox_GetLabel(self.this,arg0)
        return val
    def GetSelection(self):
        val = controlsc.wxRadioBox_GetSelection(self.this)
        return val
    def GetString(self,arg0):
        val = controlsc.wxRadioBox_GetString(self.this,arg0)
        return val
    def GetStringSelection(self):
        val = controlsc.wxRadioBox_GetStringSelection(self.this)
        return val
    def Number(self):
        val = controlsc.wxRadioBox_Number(self.this)
        return val
    def SetBoxLabel(self,arg0):
        val = controlsc.wxRadioBox_SetBoxLabel(self.this,arg0)
        return val
    def SetLabel(self,arg0,arg1):
        val = controlsc.wxRadioBox_SetLabel(self.this,arg0,arg1)
        return val
    def SetSelection(self,arg0):
        val = controlsc.wxRadioBox_SetSelection(self.this,arg0)
        return val
    def SetStringSelection(self,arg0):
        val = controlsc.wxRadioBox_SetStringSelection(self.this,arg0)
        return val
    def Show(self,arg0):
        val = controlsc.wxRadioBox_Show(self.this,arg0)
        return val
    def ShowItem(self,arg0,arg1):
        val = controlsc.wxRadioBox_ShowItem(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxRadioBox instance>"
class wxRadioBox(wxRadioBoxPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxRadioBox,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxRadioButtonPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetValue(self):
        val = controlsc.wxRadioButton_GetValue(self.this)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxRadioButton_SetValue(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxRadioButton instance>"
class wxRadioButton(wxRadioButtonPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxRadioButton,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxSliderPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ClearSel(self):
        val = controlsc.wxSlider_ClearSel(self.this)
        return val
    def ClearTicks(self):
        val = controlsc.wxSlider_ClearTicks(self.this)
        return val
    def GetLineSize(self):
        val = controlsc.wxSlider_GetLineSize(self.this)
        return val
    def GetMax(self):
        val = controlsc.wxSlider_GetMax(self.this)
        return val
    def GetMin(self):
        val = controlsc.wxSlider_GetMin(self.this)
        return val
    def GetPageSize(self):
        val = controlsc.wxSlider_GetPageSize(self.this)
        return val
    def GetSelEnd(self):
        val = controlsc.wxSlider_GetSelEnd(self.this)
        return val
    def GetSelStart(self):
        val = controlsc.wxSlider_GetSelStart(self.this)
        return val
    def GetThumbLength(self):
        val = controlsc.wxSlider_GetThumbLength(self.this)
        return val
    def GetTickFreq(self):
        val = controlsc.wxSlider_GetTickFreq(self.this)
        return val
    def GetValue(self):
        val = controlsc.wxSlider_GetValue(self.this)
        return val
    def SetRange(self,arg0,arg1):
        val = controlsc.wxSlider_SetRange(self.this,arg0,arg1)
        return val
    def SetTickFreq(self,arg0,arg1):
        val = controlsc.wxSlider_SetTickFreq(self.this,arg0,arg1)
        return val
    def SetLineSize(self,arg0):
        val = controlsc.wxSlider_SetLineSize(self.this,arg0)
        return val
    def SetPageSize(self,arg0):
        val = controlsc.wxSlider_SetPageSize(self.this,arg0)
        return val
    def SetSelection(self,arg0,arg1):
        val = controlsc.wxSlider_SetSelection(self.this,arg0,arg1)
        return val
    def SetThumbLength(self,arg0):
        val = controlsc.wxSlider_SetThumbLength(self.this,arg0)
        return val
    def SetTick(self,arg0):
        val = controlsc.wxSlider_SetTick(self.this,arg0)
        return val
    def SetValue(self,arg0):
        val = controlsc.wxSlider_SetValue(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxSlider instance>"
class wxSlider(wxSliderPtr):
    def __init__(self,arg0,arg1,arg2,arg3,arg4,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controlsc.new_wxSlider,(arg0.this,arg1,arg2,arg3,arg4,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

