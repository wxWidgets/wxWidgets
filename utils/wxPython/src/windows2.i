/////////////////////////////////////////////////////////////////////////////
// Name:        windows2.i
// Purpose:     SWIG definitions of MORE window classes
//
// Author:      Robin Dunn
//
// Created:     6/2/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module windows2

%{
#include "helpers.h"
#include <wx/grid.h>
#include <wx/notebook.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import controls.i
%import events.i

//---------------------------------------------------------------------------

enum {
    wxGRID_TEXT_CTRL,
    wxGRID_HSCROLL,
    wxGRID_VSCROLL,
};



class wxGridCell {
public:
    wxString& GetTextValue();
    void SetTextValue(const wxString& str);
    wxFont *GetFont();
    void SetFont(wxFont *f);
    wxColour& GetTextColour();
    void SetTextColour(const wxColour& colour);
    wxColour& GetBackgroundColour();
    void SetBackgroundColour(const wxColour& colour);
    wxBrush *GetBackgroundBrush();
    int GetAlignment();
    void SetAlignment(int align);
    wxBitmap *GetCellBitmap();
    void SetCellBitmap(wxBitmap *bitmap);
};





class wxGrid : public wxPanel {
public:
    wxGrid(wxWindow* parent, wxWindowID id,
           const wxPoint& pos=wxPyDefaultPosition,
           const wxSize& size=wxPyDefaultSize,
           long style=0,
           char* name="grid");

    void AdjustScrollbars();
    bool AppendCols(int n=1, bool updateLabels=TRUE);
    bool AppendRows(int n=1, bool updateLabels=TRUE);
    void BeginBatch();
    bool CellHitTest(int x, int y, int *OUTPUT, int *OUTPUT);

    %addmethods {
        // TODO:  For now we are just ignoring the initial cellValues
        //        and widths.  Add support for loading them from
        //        Python sequence objects.
        bool CreateGrid(int rows, int cols,
                        //PyObject* cellValues  = NULL,
                        //PyObject* widths      = NULL,
                        short defaultWidth      = wxGRID_DEFAULT_CELL_WIDTH,
                        short defaultHeight     = wxGRID_DEFAULT_CELL_HEIGHT) {
            return self->CreateGrid(rows, cols, NULL, NULL,
                                    defaultWidth, defaultHeight);
        }
    }

    bool CurrentCellVisible();
    bool DeleteCols(int pos=0, int n=1, bool updateLabels=TRUE);
    bool DeleteRows(int pos=0, int n=1, bool updateLabels=TRUE);
    void EndBatch();

    int  GetBatchCount();
    wxGridCell* GetCell(int row, int col);
    int GetCellAlignment(int row, int col);
    %name(GetDefCellAlignment)int GetCellAlignment();
    wxColour& GetCellBackgroundColour(int row, int col);
    %name(GetDefCellBackgroundColour)
        wxColour& GetCellBackgroundColour();
    //wxGridCell *** GetCells();
    wxColour& GetCellTextColour(int row, int col);
    %name(GetDefCellTextColour)wxColour& GetCellTextColour();
    wxFont* GetCellTextFont(int row, int col);
    %name(GetDefCellTextFont)wxFont* GetCellTextFont();
    wxString& GetCellValue(int row, int col);
    int GetCols();
    int GetColumnWidth(int col);
    wxRect& GetCurrentRect();
    int GetCursorColumn();
    int GetCursorRow();
    bool GetEditable();
    wxScrollBar * GetHorizScrollBar();
    int GetLabelAlignment(int orientation);
    wxColour& GetLabelBackgroundColour();
    int GetLabelSize(int orientation);
    wxColour& GetLabelTextColour();
    wxFont* GetLabelTextFont();
    wxString& GetLabelValue(int orientation, int pos);
    int GetRowHeight(int row);
    int GetRows();
    int GetScrollPosX();
    int GetScrollPosY();
    wxTextCtrl* GetTextItem();
    wxScrollBar* GetVertScrollBar();

    bool InsertCols(int pos=0, int n=1, bool updateLabels=TRUE);
    bool InsertRows(int pos=0, int n=1, bool updateLabels=TRUE);

    // TODO:  How to handle callbacks that don't come from
    //        event system???
    //
    //void OnActivate(bool active);
    //void OnChangeLabels();
    //void OnChangeSelectionLabel();
    //wxGridCell* OnCreateCell();
    //void OnLeftClick(int row, int col, int x, int y, bool control, bool shift);
    //void OnRightClick(int row, int col, int x, int y, bool control, bool shift);
    //void OnLabelLeftClick(int row, int col, int x, int y, bool control, bool shift);
    //void OnLabelRightClick(int row, int col, int x, int y, bool control, bool shift);
    //void OnSelectCell(int row, int col);
    //void OnSelectCellImplementation(wxDC *dc, int row, int col);

    void SetCellAlignment(int alignment, int row, int col);
    %name(SetDefCellAlignment)void SetCellAlignment(int alignment);
    void SetCellBackgroundColour(const wxColour& colour, int row, int col);
    %name(SetDefCellBackgroundColour)
        void SetCellBackgroundColour(const wxColour& colour);
    void SetCellTextColour(const wxColour& colour, int row, int col);
    %name(SetDefCellTextColour)void SetCellTextColour(const wxColour& colour);
    void SetCellTextFont(wxFont *font, int row, int col);
    %name(SetDefCellTextFont)void SetCellTextFont(wxFont *font);
    void SetCellValue(const wxString& val, int row, int col);
    void SetColumnWidth(int col, int width);
    void SetDividerPen(wxPen *pen);
    void SetEditable(bool editable);
    void SetGridCursor(int row, int col);
    void SetLabelAlignment(int orientation, int alignment);
    void SetLabelBackgroundColour(const wxColour& value);
    void SetLabelSize(int orientation, int size);
    void SetLabelTextColour(const wxColour& value);
    void SetLabelTextFont(wxFont *font);
    void SetLabelValue(int orientation, const wxString& value, int pos);
    void SetRowHeight(int row, int height);

    void UpdateDimensions();
};

//---------------------------------------------------------------------------

class wxNotebookEvent : public wxCommandEvent {
public:
    int GetSelection();
    int GetOldSelection();
};



class wxNotebook : public wxControl {
public:
    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               char* name = "notebook");

    int GetPageCount();
    int SetSelection(int nPage);
    void AdvanceSelection(bool bForward = TRUE);
    int GetSelection();
    bool SetPageText(int nPage, const wxString& strText);
    wxString GetPageText(int nPage) const;
    void SetImageList(wxImageList* imageList);
    wxImageList* GetImageList();
    int  GetPageImage(int nPage);
    bool SetPageImage(int nPage, int nImage);
    int GetRowCount();

    // LINK ERROR: void SetPageSize(const wxSize& size);
    // LINK ERROR: void SetPadding(const wxSize& padding);
    bool DeletePage(int nPage);
    bool DeleteAllPages();
    bool AddPage(/*wxNotebookPage*/ wxWindow *pPage,
                 const wxString& strText,
                 bool bSelect = FALSE,
                 int imageId = -1);
    bool InsertPage(int nPage,
                    /*wxNotebookPage*/ wxWindow *pPage,
                    const wxString& strText,
                    bool bSelect = FALSE,
                    int imageId = -1);
    wxNotebookPage *GetPage(int nPage);

};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.2  1998/08/15 07:36:50  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:52  RD
// Initial version
//
//
