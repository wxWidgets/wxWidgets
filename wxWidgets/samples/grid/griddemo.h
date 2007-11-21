/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.h
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifndef griddemo_h
#define griddemo_h

class wxGrid;

class GridApp : public wxApp
{
public:
    bool OnInit();
};


class GridFrame : public wxFrame
{
    wxGrid         *grid;
#if wxUSE_LOG
    wxTextCtrl     *logWin;
    wxLogTextCtrl  *logger;
#endif // wxUSE_LOG

    void SetDefaults();

    void ToggleRowLabels( wxCommandEvent& );
    void ToggleColLabels( wxCommandEvent& );
    void ToggleEditing( wxCommandEvent& );
    void ToggleRowSizing( wxCommandEvent& );
    void ToggleColSizing( wxCommandEvent& );
    void ToggleColMoving( wxCommandEvent& );
    void ToggleGridSizing( wxCommandEvent& );
    void ToggleGridDragCell ( wxCommandEvent& );
    void ToggleGridLines( wxCommandEvent& );
    void AutoSizeCols( wxCommandEvent& );
    void CellOverflow( wxCommandEvent& );
    void ResizeCell( wxCommandEvent& );
    void SetLabelColour( wxCommandEvent& );
    void SetLabelTextColour( wxCommandEvent& );
    void SetLabelFont(wxCommandEvent &);
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

    void DeselectCell(wxCommandEvent& event);
    void DeselectCol(wxCommandEvent& event);
    void DeselectRow(wxCommandEvent& event);
    void DeselectAll(wxCommandEvent& event);
    void SelectCell(wxCommandEvent& event);
    void SelectCol(wxCommandEvent& event);
    void SelectRow(wxCommandEvent& event);
    void SelectAll(wxCommandEvent& event);
    void OnAddToSelectToggle(wxCommandEvent& event);
    void OnShowSelection(wxCommandEvent& event);

    void OnLabelLeftClick( wxGridEvent& );
    void OnCellLeftClick( wxGridEvent& );
    void OnRowSize( wxGridSizeEvent& );
    void OnColSize( wxGridSizeEvent& );
    void OnSelectCell( wxGridEvent& );
    void OnRangeSelected( wxGridRangeSelectEvent& );
    void OnCellValueChanged( wxGridEvent& );
    void OnCellBeginDrag( wxGridEvent& );

    void OnEditorShown(wxGridEvent&);
    void OnEditorHidden(wxGridEvent&);

    void OnSetHighlightWidth(wxCommandEvent&);
    void OnSetROHighlightWidth(wxCommandEvent&);

public:
    GridFrame();
    ~GridFrame();

    void OnQuit( wxCommandEvent& );
    void About( wxCommandEvent& );
    void OnVTable( wxCommandEvent& );
    void OnBugsTable( wxCommandEvent& );
    void OnSmallGrid( wxCommandEvent& );

    enum
    {
        ID_TOGGLEROWLABELS = 100,
        ID_TOGGLECOLLABELS,
        ID_TOGGLEEDIT,
        ID_TOGGLEROWSIZING,
        ID_TOGGLECOLSIZING,
        ID_TOGGLECOLMOVING,
        ID_TOGGLEGRIDSIZING,
        ID_TOGGLEGRIDDRAGCELL,
        ID_TOGGLEGRIDLINES,
        ID_AUTOSIZECOLS,
        ID_CELLOVERFLOW,
        ID_RESIZECELL,
        ID_SETLABELCOLOUR,
        ID_SETLABELTEXTCOLOUR,
        ID_SETLABEL_FONT,
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
        ID_VTABLE,
        ID_BUGS_TABLE,
        ID_SMALL_GRID,
        ID_SELECT_UNSELECT,
        ID_SHOW_SELECTION,
        ID_SELECT_ALL,
        ID_SELECT_ROW,
        ID_SELECT_COL,
        ID_SELECT_CELL,
        ID_DESELECT_ALL,
        ID_DESELECT_ROW,
        ID_DESELECT_COL,
        ID_DESELECT_CELL,

        ID_SET_HIGHLIGHT_WIDTH,
        ID_SET_RO_HIGHLIGHT_WIDTH,

        ID_TESTFUNC
    };

#if wxUSE_LOG
    wxLog *m_logOld;
#endif // wxUSE_LOG

    // add the cells to selection when using commands from select menu?
    bool m_addToSel;

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
        return wxString::Format(wxT("(%d, %d)"), row, col);
    }

    void SetValue( int , int , const wxString&  ) { /* ignore */ }
    bool IsEmptyCell( int , int  ) { return false; }

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

    virtual wxGridCellAttr *GetAttr(int row, int col,
                                    wxGridCellAttr::wxAttrKind  kind) const;

private:
    wxGridCellAttr *m_attrForOddRows;
};

// ----------------------------------------------------------------------------
// another, more realistic, grid example: shows typed columns and more
// ----------------------------------------------------------------------------

class BugsGridTable : public wxGridTableBase
{
public:
    BugsGridTable(){};

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

