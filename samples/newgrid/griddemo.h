/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.h
// Purpose:     Grid control wxWindows sample
// Author:      Michael Bedward
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifndef griddemo_h
#define griddemo_h

#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
    #error "This sample requires the new wxGrid class."
#endif

class wxGrid;

class GridApp : public wxApp
{
public:
    bool OnInit();
};


class GridFrame : public wxFrame
{
    wxGrid         *grid;
    wxTextCtrl     *logWin;
    wxLogTextCtrl  *logger;
    wxString       logBuf;

    void SetDefaults();

    void ToggleRowLabels( wxCommandEvent& );
    void ToggleColLabels( wxCommandEvent& );
    void ToggleEditing( wxCommandEvent& );
    void ToggleRowSizing( wxCommandEvent& );
    void ToggleColSizing( wxCommandEvent& );
    void SetLabelColour( wxCommandEvent& );
    void SetLabelTextColour( wxCommandEvent& );
    void SetRowLabelHorizAlignment( wxCommandEvent& );
    void SetRowLabelVertAlignment( wxCommandEvent& );
    void SetColLabelHorizAlignment( wxCommandEvent& );
    void SetColLabelVertAlignment( wxCommandEvent& );
    void SetGridLineColour( wxCommandEvent& );

    void SetCellFgColour(wxCommandEvent &);
    void SetCellBgColour(wxCommandEvent &);

    void InsertRow( wxCommandEvent& );
    void InsertCol( wxCommandEvent& );
    void DeleteSelectedRows( wxCommandEvent& );
    void DeleteSelectedCols( wxCommandEvent& );
    void ClearGrid( wxCommandEvent& );

    void OnLabelLeftClick( wxGridEvent& );
    void OnCellLeftClick( wxGridEvent& );
    void OnRowSize( wxGridSizeEvent& );
    void OnColSize( wxGridSizeEvent& );
    void OnSelectCell( wxGridEvent& );
    void OnRangeSelected( wxGridRangeSelectEvent& );
    void OnCellValueChanged( wxGridEvent& );

    void OnEditorShown(wxGridEvent&);
    void OnEditorHidden(wxGridEvent&);

public:
    GridFrame();
    ~GridFrame();

    void OnQuit( wxCommandEvent& );
    void About( wxCommandEvent& );
    void OnVTable( wxCommandEvent& );
    void OnBugsTable( wxCommandEvent& );

    enum
    {
        ID_TOGGLEROWLABELS = 100,
        ID_TOGGLECOLLABELS,
        ID_TOGGLEEDIT,
        ID_TOGGLEROWSIZING,
        ID_TOGGLECOLSIZING,
        ID_SETLABELCOLOUR,
        ID_SETLABELTEXTCOLOUR,
        ID_ROWLABELALIGN,
        ID_ROWLABELHORIZALIGN,
        ID_ROWLABELVERTALIGN,
        ID_COLLABELALIGN,
        ID_COLLABELHORIZALIGN,
        ID_COLLABELVERTALIGN,
        ID_GRIDLINECOLOUR,
        ID_INSERTROW,
        ID_INSERTCOL,
        ID_DELETEROW,
        ID_DELETECOL,
        ID_CLEARGRID,
        ID_SET_CELL_FG_COLOUR,
        ID_SET_CELL_BG_COLOUR,
        ID_ABOUT,
        ID_VTABLE,
        ID_BUGS_TABLE,

        ID_TESTFUNC
    };

    DECLARE_EVENT_TABLE()
};

class MyGridCellRenderer : public wxGridCellStringRenderer
{
public:
    virtual void Draw(wxGrid& grid,
                      wxGridCellAttr& attr,
                      wxDC& dc,
                      const wxRect& rect,
                      int row, int col,
                      bool isSelected);
};

// ----------------------------------------------------------------------------
// demonstration of virtual table which doesn't store all of its data in
// memory
// ----------------------------------------------------------------------------

class SimpleTable : public wxGridStringTable {
public:
    SimpleTable( int numRows, int numCols )
        : wxGridStringTable( numRows, numCols ) {}

    // override this to fake a row as all bools
    wxString GetTypeName( int row, int col )
        {
            if (row == 8)
                return wxT("bool");
            else if (row == 9 && col == 1)
                return wxT("unknown type");  // to test fallback
            else
                return wxGridStringTable::GetTypeName(row, col);
        }

};

class BigGridTable : public wxGridTableBase
{
public:
    BigGridTable(long sizeGrid) { m_sizeGrid = sizeGrid; }

    long GetNumberRows() { return m_sizeGrid; }
    long GetNumberCols() { return m_sizeGrid; }
    wxString GetValue( int row, int col )
    {
        return wxString::Format("(%d, %d)", row, col);
    }

    void SetValue( int , int , const wxString&  ) { /* ignore */ }
    bool IsEmptyCell( int , int  ) { return FALSE; }

private:
    long m_sizeGrid;
};

class BigGridFrame : public wxFrame
{
public:
    BigGridFrame(long sizeGrid);

private:
    wxGrid*       m_grid;
    BigGridTable* m_table;
};

// ----------------------------------------------------------------------------
// another, more realistic, grid example
// ----------------------------------------------------------------------------

class BugsGridFrame : public wxFrame
{
public:
    BugsGridFrame();
};

#endif // griddemo_h

