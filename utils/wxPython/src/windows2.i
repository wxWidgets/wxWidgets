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
#include <wx/splitter.h>
#ifdef __WXMSW__
#include <wx/msw/taskbar.h>
#endif
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

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

//#ifdef OLD_GRID

enum {
    wxGRID_TEXT_CTRL,
    wxGRID_HSCROLL,
    wxGRID_VSCROLL
};

class wxGridCell {
public:
    wxGridCell();
    ~wxGridCell();

    wxString& GetTextValue();
    void      SetTextValue(const wxString& str);
    wxFont&   GetFont();
    void      SetFont(wxFont& f);
    wxColour& GetTextColour();
    void      SetTextColour(const wxColour& colour);
    wxColour& GetBackgroundColour();
    void      SetBackgroundColour(const wxColour& colour);
    wxBrush&  GetBackgroundBrush();
    int       GetAlignment();
    void      SetAlignment(int align);
    wxBitmap* GetCellBitmap();
    void      SetCellBitmap(wxBitmap* bitmap);
};




class wxGrid : public wxPanel {
public:
    wxGrid(wxWindow* parent, wxWindowID id,
           const wxPoint& pos=wxPyDefaultPosition,
           const wxSize& size=wxPyDefaultSize,
           long style=0,
           char* name="grid");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnSelectCell',           wxEVT_GRID_SELECT_CELL)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnCreateCell',           wxEVT_GRID_CREATE_CELL)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnChangeLabels',         wxEVT_GRID_CHANGE_LABELS)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnChangeSelectionLabel', wxEVT_GRID_CHANGE_SEL_LABEL)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnCellChange',           wxEVT_GRID_CELL_CHANGE)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnCellLeftClick',        wxEVT_GRID_CELL_LCLICK)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnCellRightClick',       wxEVT_GRID_CELL_RCLICK)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLabelLeftClick',       wxEVT_GRID_LABEL_LCLICK)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLabelRightClick',      wxEVT_GRID_LABEL_RCLICK)"


    void AdjustScrollbars();
    bool AppendCols(int n=1, int updateLabels=TRUE);
    bool AppendRows(int n=1, int updateLabels=TRUE);
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
    %name(GetDefCellBackgroundColour) wxColour& GetCellBackgroundColour();

    //wxGridCell *** GetCells();
    %addmethods {
        PyObject* GetCells() {
            int row, col;
            PyObject* rows = PyList_New(0);
            for (row=0; row < self->GetRows(); row++) {
                PyObject* rowList = PyList_New(0);
                for (col=0; col < self->GetCols(); col++) {
                    wxGridCell* cell = self->GetCell(row, col);

                    bool doSave = wxPyRestoreThread();
                    PyObject* pyCell = wxPyConstructObject(cell, "wxGridCell");
                    wxPySaveThread(doSave);

                    if (PyList_Append(rowList, pyCell) == -1)
                        return NULL;
                }
                if (PyList_Append(rows, rowList) == -1)
                    return NULL;
            }
            return rows;
        }
    }
    wxColour& GetCellTextColour(int row, int col);
    %name(GetDefCellTextColour)wxColour& GetCellTextColour();
    wxFont& GetCellTextFont(int row, int col);
    %name(GetDefCellTextFont)wxFont& GetCellTextFont();
    wxString& GetCellValue(int row, int col);
    int GetCols();
    int GetColumnWidth(int col);
    wxRect GetCurrentRect();
    int GetCursorColumn();
    int GetCursorRow();
    bool GetEditable();
    wxScrollBar * GetHorizScrollBar();
    int GetLabelAlignment(int orientation);
    wxColour& GetLabelBackgroundColour();
    int GetLabelSize(int orientation);
    wxColour& GetLabelTextColour();
    wxFont& GetLabelTextFont();
    wxString& GetLabelValue(int orientation, int pos);
    int GetRowHeight(int row);
    int GetRows();
    int GetScrollPosX();
    int GetScrollPosY();
    wxTextCtrl* GetTextItem();
    wxScrollBar* GetVertScrollBar();

    bool InsertCols(int pos=0, int n=1, bool updateLabels=TRUE);
    bool InsertRows(int pos=0, int n=1, bool updateLabels=TRUE);

    void OnActivate(bool active);

    void SetCellAlignment(int alignment, int row, int col);
    %name(SetDefCellAlignment)void SetCellAlignment(int alignment);
    void SetCellBackgroundColour(const wxColour& colour, int row, int col);
    %name(SetDefCellBackgroundColour)
        void SetCellBackgroundColour(const wxColour& colour);
    void SetCellTextColour(const wxColour& colour, int row, int col);
    %name(SetDefCellTextColour)void SetCellTextColour(const wxColour& colour);
    void SetCellTextFont(wxFont& font, int row, int col);
    %name(SetDefCellTextFont)void SetCellTextFont(wxFont& font);
    void SetCellValue(const wxString& val, int row, int col);
    void SetColumnWidth(int col, int width);
    void SetDividerPen(wxPen& pen);
    void SetEditable(bool editable);
    void SetGridCursor(int row, int col);
    void SetLabelAlignment(int orientation, int alignment);
    void SetLabelBackgroundColour(const wxColour& value);
    void SetLabelSize(int orientation, int size);
    void SetLabelTextColour(const wxColour& value);
    void SetLabelTextFont(wxFont& font);
    void SetLabelValue(int orientation, const wxString& value, int pos);
    void SetRowHeight(int row, int height);

    void UpdateDimensions();

    bool GetEditInPlace();
    void SetEditInPlace(int edit = TRUE);

};


class wxGridEvent : public wxEvent {
public:
    int         m_row;
    int         m_col;
    int         m_x;
    int         m_y;
    bool        m_control;
    bool        m_shift;
    wxGridCell* m_cell;

    int GetRow();
    int GetCol();
    wxPoint GetPosition();
    bool ControlDown();
    bool ShiftDown();
    wxGridCell* GetCell();
};


enum {
    wxEVT_GRID_SELECT_CELL,
    wxEVT_GRID_CREATE_CELL,
    wxEVT_GRID_CHANGE_LABELS,
    wxEVT_GRID_CHANGE_SEL_LABEL,
    wxEVT_GRID_CELL_CHANGE,
    wxEVT_GRID_CELL_LCLICK,
    wxEVT_GRID_CELL_RCLICK,
    wxEVT_GRID_LABEL_LCLICK,
    wxEVT_GRID_LABEL_RCLICK,
};

//#endif

//---------------------------------------------------------------------------

class wxNotebookEvent : public wxNotifyEvent {
public:
    int GetSelection();
    int GetOldSelection();
    void SetOldSelection(int page);
    void SetSelection(int page);
};



class wxNotebook : public wxControl {
public:
    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               char* name = "notebook");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

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
    bool RemovePage(int nPage);
    bool DeleteAllPages();
    bool AddPage(/*wxNotebookPage*/ wxWindow *pPage,
                 const wxString& strText,
                 int bSelect = FALSE,
                 int imageId = -1);
#ifdef __WXMSW__
    bool InsertPage(int nPage,
                    /*wxNotebookPage*/ wxWindow *pPage,
                    const wxString& strText,
                    bool bSelect = FALSE,
                    int imageId = -1);
#endif
    /*wxNotebookPage*/ wxWindow *GetPage(int nPage);

    %addmethods {
        void ResizeChildren() {
            wxSizeEvent evt(self->GetClientSize());
            self->GetEventHandler()->ProcessEvent(evt);
        }
    }


};

//---------------------------------------------------------------------------

class wxSplitterEvent : public wxCommandEvent {
public:
    int GetSashPosition();
    int GetX();
    int GetY();
    wxWindow* GetWindowBeingRemoved();
    void SetSashPosition(int pos);
}




class wxSplitterWindow : public wxWindow {
public:
    wxSplitterWindow(wxWindow* parent, wxWindowID id,
                     const wxPoint& point = wxPyDefaultPosition,
                     const wxSize& size = wxPyDefaultSize,
                     long style=wxSP_3D|wxCLIP_CHILDREN,
                     char* name = "splitterWindow");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    int GetBorderSize();
    int GetMinimumPaneSize();
    int GetSashPosition();
    int GetSashSize();
    int GetSplitMode();
    wxWindow* GetWindow1();
    wxWindow* GetWindow2();
    void Initialize(wxWindow* window);
    bool IsSplit();

    bool ReplaceWindow(wxWindow * winOld, wxWindow * winNew);
    void SetBorderSize(int width);
    void SetSashPosition(int position, int redraw = TRUE);
    void SetSashSize(int width);
    void SetMinimumPaneSize(int paneSize);
    void SetSplitMode(int mode);
    bool SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition = 0);
    bool SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition = 0);
    bool Unsplit(wxWindow* toRemove = NULL);
};

//---------------------------------------------------------------------------

#ifdef __WXMSW__

enum {
    wxEVT_TASKBAR_MOVE,
    wxEVT_TASKBAR_LEFT_DOWN,
    wxEVT_TASKBAR_LEFT_UP,
    wxEVT_TASKBAR_RIGHT_DOWN,
    wxEVT_TASKBAR_RIGHT_UP,
    wxEVT_TASKBAR_LEFT_DCLICK,
    wxEVT_TASKBAR_RIGHT_DCLICK
};


class wxTaskBarIcon : public wxEvtHandler {
public:
    wxTaskBarIcon();
    ~wxTaskBarIcon();

    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnMouseMove',    wxEVT_TASKBAR_MOVE)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonDown',  wxEVT_TASKBAR_LEFT_DOWN)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonUp',    wxEVT_TASKBAR_LEFT_UP)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonDown',  wxEVT_TASKBAR_RIGHT_DOWN)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonUp',    wxEVT_TASKBAR_RIGHT_UP)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonDClick',wxEVT_TASKBAR_LEFT_DCLICK)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonDClick',wxEVT_TASKBAR_RIGHT_DCLICK)"

    bool SetIcon(const wxIcon& icon, const char* tooltip = "");
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu);

};
#endif

//---------------------------------------------------------------------------
