# This file was created automatically by SWIG.
import windows2c

from misc import *

from gdi import *

from windows import *

from controls import *

from events import *
class wxGridCellPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
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
    def __init__(self,this):
        self.this = this




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
        val = windows2c.wxNotebook_SetImageList(self.this,arg0)
        return val
    def GetImageList(self):
        val = windows2c.wxNotebook_GetImageList(self.this)
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
    def DeleteAllPages(self):
        val = windows2c.wxNotebook_DeleteAllPages(self.this)
        return val
    def AddPage(self,arg0,arg1,*args):
        val = apply(windows2c.wxNotebook_AddPage,(self.this,arg0.this,arg1,)+args)
        return val
    def GetPage(self,arg0):
        val = windows2c.wxNotebook_GetPage(self.this,arg0)
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






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxGRID_TEXT_CTRL = windows2c.wxGRID_TEXT_CTRL
wxGRID_HSCROLL = windows2c.wxGRID_HSCROLL
wxGRID_VSCROLL = windows2c.wxGRID_VSCROLL
