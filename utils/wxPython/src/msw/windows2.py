# This file was created automatically by SWIG.
import windows2c

from misc import *

from gdi import *

from windows import *

from controls import *

from events import *
import wx
class wxGridCellPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,windows2c=windows2c):
        if self.thisown == 1 :
            windows2c.delete_wxGridCell(self)
    def GetTextValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetTextValue,(self,) + _args, _kwargs)
        return val
    def SetTextValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetTextValue,(self,) + _args, _kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetFont,(self,) + _args, _kwargs)
        return val
    def GetTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetTextColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetTextColour,(self,) + _args, _kwargs)
        return val
    def GetBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetBackgroundColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetBackgroundColour,(self,) + _args, _kwargs)
        return val
    def GetBackgroundBrush(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetBackgroundBrush,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) 
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetAlignment,(self,) + _args, _kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetAlignment,(self,) + _args, _kwargs)
        return val
    def GetCellBitmap(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_GetCellBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def SetCellBitmap(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridCell_SetCellBitmap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxGridCell instance at %s>" % (self.this,)
class wxGridCell(wxGridCellPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxGridCell,_args,_kwargs)
        self.thisown = 1




class wxGridPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AdjustScrollbars(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_AdjustScrollbars,(self,) + _args, _kwargs)
        return val
    def AppendCols(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_AppendCols,(self,) + _args, _kwargs)
        return val
    def AppendRows(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_AppendRows,(self,) + _args, _kwargs)
        return val
    def BeginBatch(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_BeginBatch,(self,) + _args, _kwargs)
        return val
    def CellHitTest(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_CellHitTest,(self,) + _args, _kwargs)
        return val
    def CreateGrid(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_CreateGrid,(self,) + _args, _kwargs)
        return val
    def CurrentCellVisible(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_CurrentCellVisible,(self,) + _args, _kwargs)
        return val
    def DeleteCols(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_DeleteCols,(self,) + _args, _kwargs)
        return val
    def DeleteRows(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_DeleteRows,(self,) + _args, _kwargs)
        return val
    def EndBatch(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_EndBatch,(self,) + _args, _kwargs)
        return val
    def GetBatchCount(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetBatchCount,(self,) + _args, _kwargs)
        return val
    def GetCell(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCell,(self,) + _args, _kwargs)
        if val: val = wxGridCellPtr(val) 
        return val
    def GetCellAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCellAlignment,(self,) + _args, _kwargs)
        return val
    def GetDefCellAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetDefCellAlignment,(self,) + _args, _kwargs)
        return val
    def GetCellBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCellBackgroundColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetDefCellBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetDefCellBackgroundColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetCells(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCells,(self,) + _args, _kwargs)
        return val
    def GetCellTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCellTextColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetDefCellTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetDefCellTextColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetCellTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCellTextFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetDefCellTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetDefCellTextFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetCellValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCellValue,(self,) + _args, _kwargs)
        return val
    def GetCols(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCols,(self,) + _args, _kwargs)
        return val
    def GetColumnWidth(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetColumnWidth,(self,) + _args, _kwargs)
        return val
    def GetCurrentRect(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCurrentRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetCursorColumn(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCursorColumn,(self,) + _args, _kwargs)
        return val
    def GetCursorRow(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetCursorRow,(self,) + _args, _kwargs)
        return val
    def GetEditable(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetEditable,(self,) + _args, _kwargs)
        return val
    def GetHorizScrollBar(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetHorizScrollBar,(self,) + _args, _kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def GetLabelAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelAlignment,(self,) + _args, _kwargs)
        return val
    def GetLabelBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelBackgroundColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetLabelSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelSize,(self,) + _args, _kwargs)
        return val
    def GetLabelTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelTextColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetLabelTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelTextFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetLabelValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetLabelValue,(self,) + _args, _kwargs)
        return val
    def GetRowHeight(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetRowHeight,(self,) + _args, _kwargs)
        return val
    def GetRows(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetRows,(self,) + _args, _kwargs)
        return val
    def GetScrollPosX(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetScrollPosX,(self,) + _args, _kwargs)
        return val
    def GetScrollPosY(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetScrollPosY,(self,) + _args, _kwargs)
        return val
    def GetTextItem(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetTextItem,(self,) + _args, _kwargs)
        if val: val = wxTextCtrlPtr(val) 
        return val
    def GetVertScrollBar(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetVertScrollBar,(self,) + _args, _kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def InsertCols(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_InsertCols,(self,) + _args, _kwargs)
        return val
    def InsertRows(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_InsertRows,(self,) + _args, _kwargs)
        return val
    def OnActivate(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_OnActivate,(self,) + _args, _kwargs)
        return val
    def SetCellAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetCellAlignment,(self,) + _args, _kwargs)
        return val
    def SetDefCellAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetDefCellAlignment,(self,) + _args, _kwargs)
        return val
    def SetCellBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetCellBackgroundColour,(self,) + _args, _kwargs)
        return val
    def SetDefCellBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetDefCellBackgroundColour,(self,) + _args, _kwargs)
        return val
    def SetCellTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetCellTextColour,(self,) + _args, _kwargs)
        return val
    def SetDefCellTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetDefCellTextColour,(self,) + _args, _kwargs)
        return val
    def SetCellTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetCellTextFont,(self,) + _args, _kwargs)
        return val
    def SetDefCellTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetDefCellTextFont,(self,) + _args, _kwargs)
        return val
    def SetCellValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetCellValue,(self,) + _args, _kwargs)
        return val
    def SetColumnWidth(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetColumnWidth,(self,) + _args, _kwargs)
        return val
    def SetDividerPen(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetDividerPen,(self,) + _args, _kwargs)
        return val
    def SetEditable(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetEditable,(self,) + _args, _kwargs)
        return val
    def SetGridCursor(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetGridCursor,(self,) + _args, _kwargs)
        return val
    def SetLabelAlignment(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelAlignment,(self,) + _args, _kwargs)
        return val
    def SetLabelBackgroundColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelBackgroundColour,(self,) + _args, _kwargs)
        return val
    def SetLabelSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelSize,(self,) + _args, _kwargs)
        return val
    def SetLabelTextColour(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelTextColour,(self,) + _args, _kwargs)
        return val
    def SetLabelTextFont(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelTextFont,(self,) + _args, _kwargs)
        return val
    def SetLabelValue(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetLabelValue,(self,) + _args, _kwargs)
        return val
    def SetRowHeight(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetRowHeight,(self,) + _args, _kwargs)
        return val
    def UpdateDimensions(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_UpdateDimensions,(self,) + _args, _kwargs)
        return val
    def GetEditInPlace(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_GetEditInPlace,(self,) + _args, _kwargs)
        return val
    def SetEditInPlace(self, *_args, **_kwargs):
        val = apply(windows2c.wxGrid_SetEditInPlace,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxGrid instance at %s>" % (self.this,)
class wxGrid(wxGridPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxGrid,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._checkForCallback(self, 'OnSelectCell',           wxEVT_GRID_SELECT_CELL)
        wx._checkForCallback(self, 'OnCreateCell',           wxEVT_GRID_CREATE_CELL)
        wx._checkForCallback(self, 'OnChangeLabels',         wxEVT_GRID_CHANGE_LABELS)
        wx._checkForCallback(self, 'OnChangeSelectionLabel', wxEVT_GRID_CHANGE_SEL_LABEL)
        wx._checkForCallback(self, 'OnCellChange',           wxEVT_GRID_CELL_CHANGE)
        wx._checkForCallback(self, 'OnCellLeftClick',        wxEVT_GRID_CELL_LCLICK)
        wx._checkForCallback(self, 'OnCellRightClick',       wxEVT_GRID_CELL_RCLICK)
        wx._checkForCallback(self, 'OnLabelLeftClick',       wxEVT_GRID_LABEL_LCLICK)
        wx._checkForCallback(self, 'OnLabelRightClick',      wxEVT_GRID_LABEL_RCLICK)




class wxGridEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRow(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_GetRow,(self,) + _args, _kwargs)
        return val
    def GetCol(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_GetCol,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ControlDown(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_ControlDown,(self,) + _args, _kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_ShiftDown,(self,) + _args, _kwargs)
        return val
    def GetCell(self, *_args, **_kwargs):
        val = apply(windows2c.wxGridEvent_GetCell,(self,) + _args, _kwargs)
        if val: val = wxGridCellPtr(val) 
        return val
    def __setattr__(self,name,value):
        if name == "m_row" :
            windows2c.wxGridEvent_m_row_set(self,value)
            return
        if name == "m_col" :
            windows2c.wxGridEvent_m_col_set(self,value)
            return
        if name == "m_x" :
            windows2c.wxGridEvent_m_x_set(self,value)
            return
        if name == "m_y" :
            windows2c.wxGridEvent_m_y_set(self,value)
            return
        if name == "m_control" :
            windows2c.wxGridEvent_m_control_set(self,value)
            return
        if name == "m_shift" :
            windows2c.wxGridEvent_m_shift_set(self,value)
            return
        if name == "m_cell" :
            windows2c.wxGridEvent_m_cell_set(self,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_row" : 
            return windows2c.wxGridEvent_m_row_get(self)
        if name == "m_col" : 
            return windows2c.wxGridEvent_m_col_get(self)
        if name == "m_x" : 
            return windows2c.wxGridEvent_m_x_get(self)
        if name == "m_y" : 
            return windows2c.wxGridEvent_m_y_get(self)
        if name == "m_control" : 
            return windows2c.wxGridEvent_m_control_get(self)
        if name == "m_shift" : 
            return windows2c.wxGridEvent_m_shift_get(self)
        if name == "m_cell" : 
            return wxGridCellPtr(windows2c.wxGridEvent_m_cell_get(self))
        raise AttributeError,name
    def __repr__(self):
        return "<C wxGridEvent instance at %s>" % (self.this,)
class wxGridEvent(wxGridEventPtr):
    def __init__(self,this):
        self.this = this




class wxNotebookEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetOldSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_GetOldSelection,(self,) + _args, _kwargs)
        return val
    def SetOldSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_SetOldSelection,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_SetSelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotebookEvent instance at %s>" % (self.this,)
class wxNotebookEvent(wxNotebookEventPtr):
    def __init__(self,this):
        self.this = this




class wxNotebookPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageCount(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageCount,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetSelection,(self,) + _args, _kwargs)
        return val
    def AdvanceSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_AdvanceSelection,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetSelection,(self,) + _args, _kwargs)
        return val
    def SetPageText(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetPageText,(self,) + _args, _kwargs)
        return val
    def GetPageText(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageText,(self,) + _args, _kwargs)
        return val
    def SetImageList(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetImageList,(self,) + _args, _kwargs)
        return val
    def GetImageList(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetImageList,(self,) + _args, _kwargs)
        if val: val = wxImageListPtr(val) 
        return val
    def GetPageImage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageImage,(self,) + _args, _kwargs)
        return val
    def SetPageImage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetPageImage,(self,) + _args, _kwargs)
        return val
    def GetRowCount(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetRowCount,(self,) + _args, _kwargs)
        return val
    def DeletePage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_DeletePage,(self,) + _args, _kwargs)
        return val
    def RemovePage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_RemovePage,(self,) + _args, _kwargs)
        return val
    def DeleteAllPages(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_DeleteAllPages,(self,) + _args, _kwargs)
        return val
    def AddPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_AddPage,(self,) + _args, _kwargs)
        return val
    def InsertPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_InsertPage,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPage,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def ResizeChildren(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_ResizeChildren,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotebook instance at %s>" % (self.this,)
class wxNotebook(wxNotebookPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxNotebook,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxSplitterEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetSashPosition,(self,) + _args, _kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetY,(self,) + _args, _kwargs)
        return val
    def GetWindowBeingRemoved(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetWindowBeingRemoved,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_SetSashPosition,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSplitterEvent instance at %s>" % (self.this,)
class wxSplitterEvent(wxSplitterEventPtr):
    def __init__(self,this):
        self.this = this




class wxSplitterWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBorderSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetBorderSize,(self,) + _args, _kwargs)
        return val
    def GetMinimumPaneSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetMinimumPaneSize,(self,) + _args, _kwargs)
        return val
    def GetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSashPosition,(self,) + _args, _kwargs)
        return val
    def GetSashSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSashSize,(self,) + _args, _kwargs)
        return val
    def GetSplitMode(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSplitMode,(self,) + _args, _kwargs)
        return val
    def GetWindow1(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetWindow1,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def GetWindow2(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetWindow2,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def Initialize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_Initialize,(self,) + _args, _kwargs)
        return val
    def IsSplit(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_IsSplit,(self,) + _args, _kwargs)
        return val
    def ReplaceWindow(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_ReplaceWindow,(self,) + _args, _kwargs)
        return val
    def SetBorderSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetBorderSize,(self,) + _args, _kwargs)
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSashPosition,(self,) + _args, _kwargs)
        return val
    def SetSashSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSashSize,(self,) + _args, _kwargs)
        return val
    def SetMinimumPaneSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetMinimumPaneSize,(self,) + _args, _kwargs)
        return val
    def SetSplitMode(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSplitMode,(self,) + _args, _kwargs)
        return val
    def SplitHorizontally(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SplitHorizontally,(self,) + _args, _kwargs)
        return val
    def SplitVertically(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SplitVertically,(self,) + _args, _kwargs)
        return val
    def Unsplit(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_Unsplit,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSplitterWindow instance at %s>" % (self.this,)
class wxSplitterWindow(wxSplitterWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxSplitterWindow,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxTaskBarIconPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,windows2c=windows2c):
        if self.thisown == 1 :
            windows2c.delete_wxTaskBarIcon(self)
    def SetIcon(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_SetIcon,(self,) + _args, _kwargs)
        return val
    def RemoveIcon(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_RemoveIcon,(self,) + _args, _kwargs)
        return val
    def PopupMenu(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_PopupMenu,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTaskBarIcon instance at %s>" % (self.this,)
class wxTaskBarIcon(wxTaskBarIconPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxTaskBarIcon,_args,_kwargs)
        self.thisown = 1
        wx._checkForCallback(self, 'OnMouseMove',    wxEVT_TASKBAR_MOVE)
        wx._checkForCallback(self, 'OnLButtonDown',  wxEVT_TASKBAR_LEFT_DOWN)
        wx._checkForCallback(self, 'OnLButtonUp',    wxEVT_TASKBAR_LEFT_UP)
        wx._checkForCallback(self, 'OnRButtonDown',  wxEVT_TASKBAR_RIGHT_DOWN)
        wx._checkForCallback(self, 'OnRButtonUp',    wxEVT_TASKBAR_RIGHT_UP)
        wx._checkForCallback(self, 'OnLButtonDClick',wxEVT_TASKBAR_LEFT_DCLICK)
        wx._checkForCallback(self, 'OnRButtonDClick',wxEVT_TASKBAR_RIGHT_DCLICK)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxGRID_TEXT_CTRL = windows2c.wxGRID_TEXT_CTRL
wxGRID_HSCROLL = windows2c.wxGRID_HSCROLL
wxGRID_VSCROLL = windows2c.wxGRID_VSCROLL
wxEVT_GRID_SELECT_CELL = windows2c.wxEVT_GRID_SELECT_CELL
wxEVT_GRID_CREATE_CELL = windows2c.wxEVT_GRID_CREATE_CELL
wxEVT_GRID_CHANGE_LABELS = windows2c.wxEVT_GRID_CHANGE_LABELS
wxEVT_GRID_CHANGE_SEL_LABEL = windows2c.wxEVT_GRID_CHANGE_SEL_LABEL
wxEVT_GRID_CELL_CHANGE = windows2c.wxEVT_GRID_CELL_CHANGE
wxEVT_GRID_CELL_LCLICK = windows2c.wxEVT_GRID_CELL_LCLICK
wxEVT_GRID_CELL_RCLICK = windows2c.wxEVT_GRID_CELL_RCLICK
wxEVT_GRID_LABEL_LCLICK = windows2c.wxEVT_GRID_LABEL_LCLICK
wxEVT_GRID_LABEL_RCLICK = windows2c.wxEVT_GRID_LABEL_RCLICK
wxEVT_TASKBAR_MOVE = windows2c.wxEVT_TASKBAR_MOVE
wxEVT_TASKBAR_LEFT_DOWN = windows2c.wxEVT_TASKBAR_LEFT_DOWN
wxEVT_TASKBAR_LEFT_UP = windows2c.wxEVT_TASKBAR_LEFT_UP
wxEVT_TASKBAR_RIGHT_DOWN = windows2c.wxEVT_TASKBAR_RIGHT_DOWN
wxEVT_TASKBAR_RIGHT_UP = windows2c.wxEVT_TASKBAR_RIGHT_UP
wxEVT_TASKBAR_LEFT_DCLICK = windows2c.wxEVT_TASKBAR_LEFT_DCLICK
wxEVT_TASKBAR_RIGHT_DCLICK = windows2c.wxEVT_TASKBAR_RIGHT_DCLICK
