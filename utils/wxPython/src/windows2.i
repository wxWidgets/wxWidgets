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
    %name(GetDefCellBackgroundColour) wxColour& GetCellBackgroundColour();
    //wxGridCell *** GetCells();
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
    wxNotebookPage *GetPage(int nPage);

    %addmethods {
        void ResizeChildren() {
            wxSizeEvent evt(self->GetClientSize());
            self->OnSize(evt);
        }
    }


};

//---------------------------------------------------------------------------

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

    // TODO:  How to handle callbacks that don't come from
    //        event system???
    //
    //void OnDoubleClickSash(int x, int y);
    //bool OnSashPositionChange(int newSashPosition);
    //void OnUnsplit(wxWindow* removed);

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
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.10  1999/04/30 03:29:19  RD
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
// Revision 1.9.4.1  1999/03/16 06:04:05  RD
//
// wxPython 2.0b7
//
// Revision 1.9  1999/02/25 07:08:36  RD
//
// wxPython version 2.0b5
//
// Revision 1.8  1998/12/17 18:05:53  RD
//
// wxPython 0.5.2
// Minor fixes and SWIG code generation for RR's changes.  MSW and GTK
// versions are much closer now!
//
// Revision 1.7  1998/12/15 20:41:25  RD
// Changed the import semantics from "from wxPython import *" to "from
// wxPython.wx import *"  This is for people who are worried about
// namespace pollution, they can use "from wxPython import wx" and then
// prefix all the wxPython identifiers with "wx."
//
// Added wxTaskbarIcon for wxMSW.
//
// Made the events work for wxGrid.
//
// Added wxConfig.
//
// Added wxMiniFrame for wxGTK, (untested.)
//
// Changed many of the args and return values that were pointers to gdi
// objects to references to reflect changes in the wxWindows API.
//
// Other assorted fixes and additions.
//
// Revision 1.6  1998/11/25 08:45:28  RD
//
// Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
// Added events for wxGrid
// Other various fixes and additions
//
// Revision 1.5  1998/11/03 09:21:57  RD
// fixed a typo
//
// Revision 1.4  1998/10/02 06:40:43  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.3  1998/08/18 19:48:20  RD
// more wxGTK compatibility things.
//
// It builds now but there are serious runtime problems...
//
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
