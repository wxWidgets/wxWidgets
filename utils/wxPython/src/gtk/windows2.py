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
    def __del__(self):
        if self.thisown == 1 :
            windows2c.delete_wxGridCell(self.this)
    def GetTextValue(self):
        val = windows2c.wxGridCell_GetTextValue(self.this)
        return val
    def SetTextValue(self,arg0):
        val = windows2c.wxGridCell_SetTextValue(self.this,arg0)
        return val
    def GetFont(self):
        val = windows2c.wxGridCell_GetFont(self.this)
        val = wxFontPtr(val)
        return val
    def SetFont(self,arg0):
        val = windows2c.wxGridCell_SetFont(self.this,arg0.this)
        return val
    def GetTextColour(self):
        val = windows2c.wxGridCell_GetTextColour(self.this)
        val = wxColourPtr(val)
        return val
    def SetTextColour(self,arg0):
        val = windows2c.wxGridCell_SetTextColour(self.this,arg0.this)
        return val
    def GetBackgroundColour(self):
        val = windows2c.wxGridCell_GetBackgroundColour(self.this)
        val = wxColourPtr(val)
        return val
    def SetBackgroundColour(self,arg0):
        val = windows2c.wxGridCell_SetBackgroundColour(self.this,arg0.this)
        return val
    def GetBackgroundBrush(self):
        val = windows2c.wxGridCell_GetBackgroundBrush(self.this)
        val = wxBrushPtr(val)
        return val
    def GetAlignment(self):
        val = windows2c.wxGridCell_GetAlignment(self.this)
        return val
    def SetAlignment(self,arg0):
        val = windows2c.wxGridCell_SetAlignment(self.this,arg0)
        return val
    def GetCellBitmap(self):
        val = windows2c.wxGridCell_GetCellBitmap(self.this)
        val = wxBitmapPtr(val)
        return val
    def SetCellBitmap(self,arg0):
        val = windows2c.wxGridCell_SetCellBitmap(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxGridCell instance>"
class wxGridCell(wxGridCellPtr):
    def __init__(self) :
        self.this = windows2c.new_wxGridCell()
        self.thisown = 1




class wxGridPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AdjustScrollbars(self):
        val = windows2c.wxGrid_AdjustScrollbars(self.this)
        return val
    def AppendCols(self,*args):
        val = apply(windows2c.wxGrid_AppendCols,(self.this,)+args)
        return val
    def AppendRows(self,*args):
        val = apply(windows2c.wxGrid_AppendRows,(self.this,)+args)
        return val
    def BeginBatch(self):
        val = windows2c.wxGrid_BeginBatch(self.this)
        return val
    def CellHitTest(self,arg0,arg1):
        val = windows2c.wxGrid_CellHitTest(self.this,arg0,arg1)
        return val
    def CreateGrid(self,arg0,arg1,*args):
        val = apply(windows2c.wxGrid_CreateGrid,(self.this,arg0,arg1,)+args)
        return val
    def CurrentCellVisible(self):
        val = windows2c.wxGrid_CurrentCellVisible(self.this)
        return val
    def DeleteCols(self,*args):
        val = apply(windows2c.wxGrid_DeleteCols,(self.this,)+args)
        return val
    def DeleteRows(self,*args):
        val = apply(windows2c.wxGrid_DeleteRows,(self.this,)+args)
        return val
    def EndBatch(self):
        val = windows2c.wxGrid_EndBatch(self.this)
        return val
    def GetBatchCount(self):
        val = windows2c.wxGrid_GetBatchCount(self.this)
        return val
    def GetCell(self,arg0,arg1):
        val = windows2c.wxGrid_GetCell(self.this,arg0,arg1)
        val = wxGridCellPtr(val)
        return val
    def GetCellAlignment(self,arg0,arg1):
        val = windows2c.wxGrid_GetCellAlignment(self.this,arg0,arg1)
        return val
    def GetDefCellAlignment(self):
        val = windows2c.wxGrid_GetDefCellAlignment(self.this)
        return val
    def GetCellBackgroundColour(self,arg0,arg1):
        val = windows2c.wxGrid_GetCellBackgroundColour(self.this,arg0,arg1)
        val = wxColourPtr(val)
        return val
    def GetDefCellBackgroundColour(self):
        val = windows2c.wxGrid_GetDefCellBackgroundColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetCellTextColour(self,arg0,arg1):
        val = windows2c.wxGrid_GetCellTextColour(self.this,arg0,arg1)
        val = wxColourPtr(val)
        return val
    def GetDefCellTextColour(self):
        val = windows2c.wxGrid_GetDefCellTextColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetCellTextFont(self,arg0,arg1):
        val = windows2c.wxGrid_GetCellTextFont(self.this,arg0,arg1)
        val = wxFontPtr(val)
        return val
    def GetDefCellTextFont(self):
        val = windows2c.wxGrid_GetDefCellTextFont(self.this)
        val = wxFontPtr(val)
        return val
    def GetCellValue(self,arg0,arg1):
        val = windows2c.wxGrid_GetCellValue(self.this,arg0,arg1)
        return val
    def GetCols(self):
        val = windows2c.wxGrid_GetCols(self.this)
        return val
    def GetColumnWidth(self,arg0):
        val = windows2c.wxGrid_GetColumnWidth(self.this,arg0)
        return val
    def GetCurrentRect(self):
        val = windows2c.wxGrid_GetCurrentRect(self.this)
        val = wxRectPtr(val)
        val.thisown = 1
        return val
    def GetCursorColumn(self):
        val = windows2c.wxGrid_GetCursorColumn(self.this)
        return val
    def GetCursorRow(self):
        val = windows2c.wxGrid_GetCursorRow(self.this)
        return val
    def GetEditable(self):
        val = windows2c.wxGrid_GetEditable(self.this)
        return val
    def GetHorizScrollBar(self):
        val = windows2c.wxGrid_GetHorizScrollBar(self.this)
        val = wxScrollBarPtr(val)
        return val
    def GetLabelAlignment(self,arg0):
        val = windows2c.wxGrid_GetLabelAlignment(self.this,arg0)
        return val
    def GetLabelBackgroundColour(self):
        val = windows2c.wxGrid_GetLabelBackgroundColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetLabelSize(self,arg0):
        val = windows2c.wxGrid_GetLabelSize(self.this,arg0)
        return val
    def GetLabelTextColour(self):
        val = windows2c.wxGrid_GetLabelTextColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetLabelTextFont(self):
        val = windows2c.wxGrid_GetLabelTextFont(self.this)
        val = wxFontPtr(val)
        return val
    def GetLabelValue(self,arg0,arg1):
        val = windows2c.wxGrid_GetLabelValue(self.this,arg0,arg1)
        return val
    def GetRowHeight(self,arg0):
        val = windows2c.wxGrid_GetRowHeight(self.this,arg0)
        return val
    def GetRows(self):
        val = windows2c.wxGrid_GetRows(self.this)
        return val
    def GetScrollPosX(self):
        val = windows2c.wxGrid_GetScrollPosX(self.this)
        return val
    def GetScrollPosY(self):
        val = windows2c.wxGrid_GetScrollPosY(self.this)
        return val
    def GetTextItem(self):
        val = windows2c.wxGrid_GetTextItem(self.this)
        val = wxTextCtrlPtr(val)
        return val
    def GetVertScrollBar(self):
        val = windows2c.wxGrid_GetVertScrollBar(self.this)
        val = wxScrollBarPtr(val)
        return val
    def InsertCols(self,*args):
        val = apply(windows2c.wxGrid_InsertCols,(self.this,)+args)
        return val
    def InsertRows(self,*args):
        val = apply(windows2c.wxGrid_InsertRows,(self.this,)+args)
        return val
    def OnActivate(self,arg0):
        val = windows2c.wxGrid_OnActivate(self.this,arg0)
        return val
    def SetCellAlignment(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetCellAlignment(self.this,arg0,arg1,arg2)
        return val
    def SetDefCellAlignment(self,arg0):
        val = windows2c.wxGrid_SetDefCellAlignment(self.this,arg0)
        return val
    def SetCellBackgroundColour(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetCellBackgroundColour(self.this,arg0.this,arg1,arg2)
        return val
    def SetDefCellBackgroundColour(self,arg0):
        val = windows2c.wxGrid_SetDefCellBackgroundColour(self.this,arg0.this)
        return val
    def SetCellTextColour(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetCellTextColour(self.this,arg0.this,arg1,arg2)
        return val
    def SetDefCellTextColour(self,arg0):
        val = windows2c.wxGrid_SetDefCellTextColour(self.this,arg0.this)
        return val
    def SetCellTextFont(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetCellTextFont(self.this,arg0.this,arg1,arg2)
        return val
    def SetDefCellTextFont(self,arg0):
        val = windows2c.wxGrid_SetDefCellTextFont(self.this,arg0.this)
        return val
    def SetCellValue(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetCellValue(self.this,arg0,arg1,arg2)
        return val
    def SetColumnWidth(self,arg0,arg1):
        val = windows2c.wxGrid_SetColumnWidth(self.this,arg0,arg1)
        return val
    def SetDividerPen(self,arg0):
        val = windows2c.wxGrid_SetDividerPen(self.this,arg0.this)
        return val
    def SetEditable(self,arg0):
        val = windows2c.wxGrid_SetEditable(self.this,arg0)
        return val
    def SetGridCursor(self,arg0,arg1):
        val = windows2c.wxGrid_SetGridCursor(self.this,arg0,arg1)
        return val
    def SetLabelAlignment(self,arg0,arg1):
        val = windows2c.wxGrid_SetLabelAlignment(self.this,arg0,arg1)
        return val
    def SetLabelBackgroundColour(self,arg0):
        val = windows2c.wxGrid_SetLabelBackgroundColour(self.this,arg0.this)
        return val
    def SetLabelSize(self,arg0,arg1):
        val = windows2c.wxGrid_SetLabelSize(self.this,arg0,arg1)
        return val
    def SetLabelTextColour(self,arg0):
        val = windows2c.wxGrid_SetLabelTextColour(self.this,arg0.this)
        return val
    def SetLabelTextFont(self,arg0):
        val = windows2c.wxGrid_SetLabelTextFont(self.this,arg0.this)
        return val
    def SetLabelValue(self,arg0,arg1,arg2):
        val = windows2c.wxGrid_SetLabelValue(self.this,arg0,arg1,arg2)
        return val
    def SetRowHeight(self,arg0,arg1):
        val = windows2c.wxGrid_SetRowHeight(self.this,arg0,arg1)
        return val
    def UpdateDimensions(self):
        val = windows2c.wxGrid_UpdateDimensions(self.this)
        return val
    def __repr__(self):
        return "<C wxGrid instance>"
class wxGrid(wxGridPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windows2c.new_wxGrid,(arg0.this,arg1,)+args)
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
    def __setattr__(self,name,value):
        if name == "m_row" :
            windows2c.wxGridEvent_m_row_set(self.this,value)
            return
        if name == "m_col" :
            windows2c.wxGridEvent_m_col_set(self.this,value)
            return
        if name == "m_x" :
            windows2c.wxGridEvent_m_x_set(self.this,value)
            return
        if name == "m_y" :
            windows2c.wxGridEvent_m_y_set(self.this,value)
            return
        if name == "m_control" :
            windows2c.wxGridEvent_m_control_set(self.this,value)
            return
        if name == "m_shift" :
            windows2c.wxGridEvent_m_shift_set(self.this,value)
            return
        if name == "m_cell" :
            windows2c.wxGridEvent_m_cell_set(self.this,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_row" : 
            return windows2c.wxGridEvent_m_row_get(self.this)
        if name == "m_col" : 
            return windows2c.wxGridEvent_m_col_get(self.this)
        if name == "m_x" : 
            return windows2c.wxGridEvent_m_x_get(self.this)
        if name == "m_y" : 
            return windows2c.wxGridEvent_m_y_get(self.this)
        if name == "m_control" : 
            return windows2c.wxGridEvent_m_control_get(self.this)
        if name == "m_shift" : 
            return windows2c.wxGridEvent_m_shift_get(self.this)
        if name == "m_cell" : 
            return wxGridCellPtr(windows2c.wxGridEvent_m_cell_get(self.this))
        raise AttributeError,name
    def __repr__(self):
        return "<C wxGridEvent instance>"
class wxGridEvent(wxGridEventPtr):
    def __init__(self,this):
        self.this = this




class wxNotebookEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self):
        val = windows2c.wxNotebookEvent_GetSelection(self.this)
        return val
    def GetOldSelection(self):
        val = windows2c.wxNotebookEvent_GetOldSelection(self.this)
        return val
    def __repr__(self):
        return "<C wxNotebookEvent instance>"
class wxNotebookEvent(wxNotebookEventPtr):
    def __init__(self,this):
        self.this = this




class wxNotebookPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageCount(self):
        val = windows2c.wxNotebook_GetPageCount(self.this)
        return val
    def SetSelection(self,arg0):
        val = windows2c.wxNotebook_SetSelection(self.this,arg0)
        return val
    def AdvanceSelection(self,*args):
        val = apply(windows2c.wxNotebook_AdvanceSelection,(self.this,)+args)
        return val
    def GetSelection(self):
        val = windows2c.wxNotebook_GetSelection(self.this)
        return val
    def SetPageText(self,arg0,arg1):
        val = windows2c.wxNotebook_SetPageText(self.this,arg0,arg1)
        return val
    def GetPageText(self,arg0):
        val = windows2c.wxNotebook_GetPageText(self.this,arg0)
        return val
    def SetImageList(self,arg0):
        val = windows2c.wxNotebook_SetImageList(self.this,arg0.this)
        return val
    def GetImageList(self):
        val = windows2c.wxNotebook_GetImageList(self.this)
        val = wxImageListPtr(val)
        return val
    def GetPageImage(self,arg0):
        val = windows2c.wxNotebook_GetPageImage(self.this,arg0)
        return val
    def SetPageImage(self,arg0,arg1):
        val = windows2c.wxNotebook_SetPageImage(self.this,arg0,arg1)
        return val
    def GetRowCount(self):
        val = windows2c.wxNotebook_GetRowCount(self.this)
        return val
    def DeletePage(self,arg0):
        val = windows2c.wxNotebook_DeletePage(self.this,arg0)
        return val
    def RemovePage(self,arg0):
        val = windows2c.wxNotebook_RemovePage(self.this,arg0)
        return val
    def DeleteAllPages(self):
        val = windows2c.wxNotebook_DeleteAllPages(self.this)
        return val
    def AddPage(self,arg0,arg1,*args):
        val = apply(windows2c.wxNotebook_AddPage,(self.this,arg0.this,arg1,)+args)
        return val
    def GetPage(self,arg0):
        val = windows2c.wxNotebook_GetPage(self.this,arg0)
        return val
    def ResizeChildren(self):
        val = windows2c.wxNotebook_ResizeChildren(self.this)
        return val
    def __repr__(self):
        return "<C wxNotebook instance>"
class wxNotebook(wxNotebookPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windows2c.new_wxNotebook,(arg0.this,arg1,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxSplitterWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBorderSize(self):
        val = windows2c.wxSplitterWindow_GetBorderSize(self.this)
        return val
    def GetMinimumPaneSize(self):
        val = windows2c.wxSplitterWindow_GetMinimumPaneSize(self.this)
        return val
    def GetSashPosition(self):
        val = windows2c.wxSplitterWindow_GetSashPosition(self.this)
        return val
    def GetSashSize(self):
        val = windows2c.wxSplitterWindow_GetSashSize(self.this)
        return val
    def GetSplitMode(self):
        val = windows2c.wxSplitterWindow_GetSplitMode(self.this)
        return val
    def GetWindow1(self):
        val = windows2c.wxSplitterWindow_GetWindow1(self.this)
        val = wxWindowPtr(val)
        return val
    def GetWindow2(self):
        val = windows2c.wxSplitterWindow_GetWindow2(self.this)
        val = wxWindowPtr(val)
        return val
    def Initialize(self,arg0):
        val = windows2c.wxSplitterWindow_Initialize(self.this,arg0.this)
        return val
    def IsSplit(self):
        val = windows2c.wxSplitterWindow_IsSplit(self.this)
        return val
    def SetBorderSize(self,arg0):
        val = windows2c.wxSplitterWindow_SetBorderSize(self.this,arg0)
        return val
    def SetSashPosition(self,arg0,*args):
        val = apply(windows2c.wxSplitterWindow_SetSashPosition,(self.this,arg0,)+args)
        return val
    def SetSashSize(self,arg0):
        val = windows2c.wxSplitterWindow_SetSashSize(self.this,arg0)
        return val
    def SetMinimumPaneSize(self,arg0):
        val = windows2c.wxSplitterWindow_SetMinimumPaneSize(self.this,arg0)
        return val
    def SetSplitMode(self,arg0):
        val = windows2c.wxSplitterWindow_SetSplitMode(self.this,arg0)
        return val
    def SplitHorizontally(self,arg0,arg1,*args):
        val = apply(windows2c.wxSplitterWindow_SplitHorizontally,(self.this,arg0.this,arg1.this,)+args)
        return val
    def SplitVertically(self,arg0,arg1,*args):
        val = apply(windows2c.wxSplitterWindow_SplitVertically,(self.this,arg0.this,arg1.this,)+args)
        return val
    def Unsplit(self,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(windows2c.wxSplitterWindow_Unsplit,(self.this,)+args)
        return val
    def __repr__(self):
        return "<C wxSplitterWindow instance>"
class wxSplitterWindow(wxSplitterWindowPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windows2c.new_wxSplitterWindow,(arg0.this,arg1,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)






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
