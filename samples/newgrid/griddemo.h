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
    void ToggleGridSizing( wxCommandEvent& );
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
    void SelectCells( wxCommandEvent& );
    void SelectRows( wxCommandEvent& );
    void SelectCols( wxCommandEvent& );

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
        ID_TOGGLEGRIDSIZING,
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
	ID_CHANGESEL,
	ID_SELCELLS,
	ID_SELROWS,
	ID_SELCOLS,
        ID_SET_CELL_FG_COLOUR,
        ID_SET_CELL_BG_COLOUR,
        ID_ABOUT,
        ID_VTABLE,
        ID_BUGS_TABLE,

        ID_TESTFUNC
    };

    wxLog *m_logOld;

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

class BigGridTable : public wxGridTableBase
{
public:
    BigGridTable(long sizeGrid) { m_sizeGrid = sizeGrid; }

    int GetNumberRows() { return m_sizeGrid; }
    int GetNumberCols() { return m_sizeGrid; }
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
// an example of custom attr provider: this one makes all odd rows appear grey
// ----------------------------------------------------------------------------

class MyGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    MyGridCellAttrProvider();
    virtual ~MyGridCellAttrProvider();

    virtual wxGridCellAttr *GetAttr(int row, int col) const;

private:
    wxGridCellAttr *m_attrForOddRows;
};

// ----------------------------------------------------------------------------
// another, more realistic, grid example: shows typed columns and more
// ----------------------------------------------------------------------------

class BugsGridTable : public wxGridTableBase
{
public:
    BugsGridTable();

    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell( int row, int col );
    virtual wxString GetValue( int row, int col );
    virtual void SetValue( int row, int col, const wxString& value );

    virtual wxString GetColLabelValue( int col );

    virtual wxString GetTypeName( int row, int col );
    virtual bool CanGetValueAs( int row, int col, const wxString& typeName );
    virtual bool CanSetValueAs( int row, int col, const wxString& typeName );

    virtual long GetValueAsLong( int row, int col );
    virtual bool GetValueAsBool( int row, int col );

    virtual void SetValueAsLong( int row, int col, long value );
    virtual void SetValueAsBool( int row, int col, bool value );
};

class BugsGridFrame : public wxFrame
{
public:
    BugsGridFrame();
};

#endif // griddemo_h

