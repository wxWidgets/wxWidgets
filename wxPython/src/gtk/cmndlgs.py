# This file was created automatically by SWIG.
import cmndlgsc

from misc import *

from gdi import *

from fonts import *

from windows import *

from clip_dnd import *

from events import *

from frames import *

from stattool import *

from controls import *
import wx


def EVT_COMMAND_FIND(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND, func)

def EVT_COMMAND_FIND_NEXT(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_NEXT, func)

def EVT_COMMAND_FIND_REPLACE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_REPLACE, func)

def EVT_COMMAND_FIND_REPLACE_ALL(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_REPLACE_ALL, func)

def EVT_COMMAND_FIND_CLOSE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_FIND_CLOSE, func)


class wxColourDataPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=cmndlgsc.delete_wxColourData):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetChooseFull(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_GetChooseFull,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetCustomColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_GetCustomColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetChooseFull(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_SetChooseFull,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_SetColour,(self,) + _args, _kwargs)
        return val
    def SetCustomColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourData_SetCustomColour,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxColourData instance at %s>" % (self.this,)
class wxColourData(wxColourDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxColourData,_args,_kwargs)
        self.thisown = 1




class wxColourDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetColourData(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourDialog_GetColourData,(self,) + _args, _kwargs)
        if val: val = wxColourDataPtr(val) 
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxColourDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxColourDialog instance at %s>" % (self.this,)
class wxColourDialog(wxColourDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxColourDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxDirDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPath(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_GetPath,(self,) + _args, _kwargs)
        return val
    def GetMessage(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_GetMessage,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_GetStyle,(self,) + _args, _kwargs)
        return val
    def SetMessage(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_SetMessage,(self,) + _args, _kwargs)
        return val
    def SetPath(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_SetPath,(self,) + _args, _kwargs)
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxDirDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDirDialog instance at %s>" % (self.this,)
class wxDirDialog(wxDirDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxDirDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxFileDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDirectory(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetDirectory,(self,) + _args, _kwargs)
        return val
    def GetFilename(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetFilename,(self,) + _args, _kwargs)
        return val
    def GetFilterIndex(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetFilterIndex,(self,) + _args, _kwargs)
        return val
    def GetMessage(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetMessage,(self,) + _args, _kwargs)
        return val
    def GetPath(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetPath,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWildcard(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetWildcard,(self,) + _args, _kwargs)
        return val
    def SetDirectory(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetDirectory,(self,) + _args, _kwargs)
        return val
    def SetFilename(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetFilename,(self,) + _args, _kwargs)
        return val
    def SetFilterIndex(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetFilterIndex,(self,) + _args, _kwargs)
        return val
    def SetMessage(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetMessage,(self,) + _args, _kwargs)
        return val
    def SetPath(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetPath,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWildcard(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_SetWildcard,(self,) + _args, _kwargs)
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def GetFilenames(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetFilenames,(self,) + _args, _kwargs)
        return val
    def GetPaths(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFileDialog_GetPaths,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFileDialog instance at %s>" % (self.this,)
class wxFileDialog(wxFileDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFileDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxMultiChoiceDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetSelections(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxMultiChoiceDialog_SetSelections,(self,) + _args, _kwargs)
        return val
    def GetSelections(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxMultiChoiceDialog_GetSelections,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMultiChoiceDialog instance at %s>" % (self.this,)
class wxMultiChoiceDialog(wxMultiChoiceDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxMultiChoiceDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxSingleChoiceDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxSingleChoiceDialog_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetStringSelection(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxSingleChoiceDialog_GetStringSelection,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxSingleChoiceDialog_SetSelection,(self,) + _args, _kwargs)
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxSingleChoiceDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSingleChoiceDialog instance at %s>" % (self.this,)
class wxSingleChoiceDialog(wxSingleChoiceDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxSingleChoiceDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxTextEntryDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetValue(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxTextEntryDialog_GetValue,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxTextEntryDialog_SetValue,(self,) + _args, _kwargs)
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxTextEntryDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTextEntryDialog instance at %s>" % (self.this,)
class wxTextEntryDialog(wxTextEntryDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxTextEntryDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxFontDataPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=cmndlgsc.delete_wxFontData):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def EnableEffects(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_EnableEffects,(self,) + _args, _kwargs)
        return val
    def GetAllowSymbols(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetAllowSymbols,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetChosenFont(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetChosenFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetEnableEffects(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetEnableEffects,(self,) + _args, _kwargs)
        return val
    def GetInitialFont(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetInitialFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetShowHelp(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_GetShowHelp,(self,) + _args, _kwargs)
        return val
    def SetAllowSymbols(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetAllowSymbols,(self,) + _args, _kwargs)
        return val
    def SetChosenFont(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetChosenFont,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetColour,(self,) + _args, _kwargs)
        return val
    def SetInitialFont(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetInitialFont,(self,) + _args, _kwargs)
        return val
    def SetRange(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetRange,(self,) + _args, _kwargs)
        return val
    def SetShowHelp(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontData_SetShowHelp,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontData instance at %s>" % (self.this,)
class wxFontData(wxFontDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFontData,_args,_kwargs)
        self.thisown = 1




class wxFontDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFontData(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontDialog_GetFontData,(self,) + _args, _kwargs)
        if val: val = wxFontDataPtr(val) 
        return val
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFontDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFontDialog instance at %s>" % (self.this,)
class wxFontDialog(wxFontDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFontDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxMessageDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ShowModal(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxMessageDialog_ShowModal,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMessageDialog instance at %s>" % (self.this,)
class wxMessageDialog(wxMessageDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxMessageDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxProgressDialogPtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Update(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxProgressDialog_Update,(self,) + _args, _kwargs)
        return val
    def Resume(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxProgressDialog_Resume,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxProgressDialog instance at %s>" % (self.this,)
class wxProgressDialog(wxProgressDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxProgressDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxFindDialogEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFlags(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_GetFlags,(self,) + _args, _kwargs)
        return val
    def GetFindString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_GetFindString,(self,) + _args, _kwargs)
        return val
    def GetReplaceString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_GetReplaceString,(self,) + _args, _kwargs)
        return val
    def GetDialog(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_GetDialog,(self,) + _args, _kwargs)
        if val: val = wxFindReplaceDialogPtr(val) 
        return val
    def SetFlags(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_SetFlags,(self,) + _args, _kwargs)
        return val
    def SetFindString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_SetFindString,(self,) + _args, _kwargs)
        return val
    def SetReplaceString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindDialogEvent_SetReplaceString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFindDialogEvent instance at %s>" % (self.this,)
class wxFindDialogEvent(wxFindDialogEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFindDialogEvent,_args,_kwargs)
        self.thisown = 1




class wxFindReplaceDataPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=cmndlgsc.delete_wxFindReplaceData):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetFindString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_GetFindString,(self,) + _args, _kwargs)
        return val
    def GetReplaceString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_GetReplaceString,(self,) + _args, _kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_GetFlags,(self,) + _args, _kwargs)
        return val
    def SetFlags(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_SetFlags,(self,) + _args, _kwargs)
        return val
    def SetFindString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_SetFindString,(self,) + _args, _kwargs)
        return val
    def SetReplaceString(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceData_SetReplaceString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFindReplaceData instance at %s>" % (self.this,)
class wxFindReplaceData(wxFindReplaceDataPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFindReplaceData,_args,_kwargs)
        self.thisown = 1




class wxFindReplaceDialogPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceDialog_Create,(self,) + _args, _kwargs)
        return val
    def GetData(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceDialog_GetData,(self,) + _args, _kwargs)
        if val: val = wxFindReplaceDataPtr(val) 
        return val
    def SetData(self, *_args, **_kwargs):
        val = apply(cmndlgsc.wxFindReplaceDialog_SetData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFindReplaceDialog instance at %s>" % (self.this,)
class wxFindReplaceDialog(wxFindReplaceDialogPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(cmndlgsc.new_wxFindReplaceDialog,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreFindReplaceDialog(*_args,**_kwargs):
    val = wxFindReplaceDialogPtr(apply(cmndlgsc.new_wxPreFindReplaceDialog,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxCHOICEDLG_STYLE = cmndlgsc.wxCHOICEDLG_STYLE
wxFR_DOWN = cmndlgsc.wxFR_DOWN
wxFR_WHOLEWORD = cmndlgsc.wxFR_WHOLEWORD
wxFR_MATCHCASE = cmndlgsc.wxFR_MATCHCASE
wxFR_REPLACEDIALOG = cmndlgsc.wxFR_REPLACEDIALOG
wxFR_NOUPDOWN = cmndlgsc.wxFR_NOUPDOWN
wxFR_NOMATCHCASE = cmndlgsc.wxFR_NOMATCHCASE
wxFR_NOWHOLEWORD = cmndlgsc.wxFR_NOWHOLEWORD
wxEVT_COMMAND_FIND = cmndlgsc.wxEVT_COMMAND_FIND
wxEVT_COMMAND_FIND_NEXT = cmndlgsc.wxEVT_COMMAND_FIND_NEXT
wxEVT_COMMAND_FIND_REPLACE = cmndlgsc.wxEVT_COMMAND_FIND_REPLACE
wxEVT_COMMAND_FIND_REPLACE_ALL = cmndlgsc.wxEVT_COMMAND_FIND_REPLACE_ALL
wxEVT_COMMAND_FIND_CLOSE = cmndlgsc.wxEVT_COMMAND_FIND_CLOSE
