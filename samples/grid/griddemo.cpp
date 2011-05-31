/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.cpp
// Purpose:     Grid control wxWidgets sample
// Author:      Michael Bedward
// Modified by: Santiago Palacios
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/aboutdlg.h"

#include "wx/grid.h"
#include "wx/headerctrl.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"

#include "griddemo.h"

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

// Custom renderer that renders column header cells without borders and in
// italic
class CustomColumnHeaderRenderer : public wxGridColumnHeaderRenderer
{
public:
    CustomColumnHeaderRenderer(const wxColour& colFg, const wxColour& colBg)
        : m_colFg(colFg),
          m_colBg(colBg)
    {
    }

    virtual void DrawLabel(const wxGrid& WXUNUSED(grid),
                           wxDC& dc,
                           const wxString& value,
                           const wxRect& rect,
                           int horizAlign,
                           int vertAlign,
                           int WXUNUSED(textOrientation)) const
    {
        dc.SetTextForeground(m_colFg);
        dc.SetFont(wxITALIC_FONT->Bold());
        dc.DrawLabel(value, rect, horizAlign | vertAlign);
    }

    virtual void DrawBorder(const wxGrid& WXUNUSED(grid),
                            wxDC& dc,
                            wxRect& rect) const
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(m_colBg));
        dc.DrawRectangle(rect);
    }

private:
    const wxColour m_colFg, m_colBg;

    wxDECLARE_NO_COPY_CLASS(CustomColumnHeaderRenderer);
};

// And a custom attributes provider which uses custom column header renderer
// defined above
class CustomColumnHeadersProvider : public wxGridCellAttrProvider
{
public:
    // by default custom column renderer is not used, call
    // UseCustomColHeaders() to enable it
    CustomColumnHeadersProvider()
        : m_customOddRenderer(*wxYELLOW, *wxBLUE),
          m_customEvenRenderer(*wxWHITE, *wxBLACK),
          m_useCustom(false)
    {
    }

    // enable or disable the use of custom renderer for column headers
    void UseCustomColHeaders(bool use = true) { m_useCustom = use; }

protected:
    virtual const wxGridColumnHeaderRenderer& GetColumnHeaderRenderer(int col)
    {
        // if enabled, use custom renderers
        if ( m_useCustom )
        {
            // and use different ones for odd and even columns -- just to show
            // that we can
            return col % 2 ? m_customOddRenderer : m_customEvenRenderer;
        }

        return wxGridCellAttrProvider::GetColumnHeaderRenderer(col);
    }

private:
    CustomColumnHeaderRenderer m_customOddRenderer,
                               m_customEvenRenderer;

    bool m_useCustom;

    wxDECLARE_NO_COPY_CLASS(CustomColumnHeadersProvider);
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP( GridApp )

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// GridApp
// ----------------------------------------------------------------------------

bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// GridFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( GridFrame, wxFrame )
    EVT_MENU( ID_TOGGLEROWLABELS,  GridFrame::ToggleRowLabels )
    EVT_MENU( ID_TOGGLECOLLABELS,  GridFrame::ToggleColLabels )
    EVT_MENU( ID_TOGGLEEDIT, GridFrame::ToggleEditing )
    EVT_MENU( ID_TOGGLEROWSIZING, GridFrame::ToggleRowSizing )
    EVT_MENU( ID_TOGGLECOLSIZING, GridFrame::ToggleColSizing )
    EVT_MENU( ID_TOGGLECOLMOVING, GridFrame::ToggleColMoving )
    EVT_MENU( ID_TOGGLEGRIDSIZING, GridFrame::ToggleGridSizing )
    EVT_MENU( ID_TOGGLEGRIDDRAGCELL, GridFrame::ToggleGridDragCell )
    EVT_MENU( ID_COLNATIVEHEADER, GridFrame::SetNativeColHeader )
    EVT_MENU( ID_COLDEFAULTHEADER, GridFrame::SetDefaultColHeader )
    EVT_MENU( ID_COLCUSTOMHEADER, GridFrame::SetCustomColHeader )
    EVT_MENU( ID_TOGGLEGRIDLINES, GridFrame::ToggleGridLines )
    EVT_MENU( ID_AUTOSIZECOLS, GridFrame::AutoSizeCols )
    EVT_MENU( ID_CELLOVERFLOW, GridFrame::CellOverflow )
    EVT_MENU( ID_RESIZECELL, GridFrame::ResizeCell )
    EVT_MENU( ID_SETLABELCOLOUR, GridFrame::SetLabelColour )
    EVT_MENU( ID_SETLABELTEXTCOLOUR, GridFrame::SetLabelTextColour )
    EVT_MENU( ID_SETLABEL_FONT, GridFrame::SetLabelFont )
    EVT_MENU( ID_ROWLABELHORIZALIGN, GridFrame::SetRowLabelHorizAlignment )
    EVT_MENU( ID_ROWLABELVERTALIGN, GridFrame::SetRowLabelVertAlignment )
    EVT_MENU( ID_COLLABELHORIZALIGN, GridFrame::SetColLabelHorizAlignment )
    EVT_MENU( ID_COLLABELVERTALIGN, GridFrame::SetColLabelVertAlignment )
    EVT_MENU( ID_GRIDLINECOLOUR, GridFrame::SetGridLineColour )
    EVT_MENU( ID_INSERTROW, GridFrame::InsertRow )
    EVT_MENU( ID_INSERTCOL, GridFrame::InsertCol )
    EVT_MENU( ID_DELETEROW, GridFrame::DeleteSelectedRows )
    EVT_MENU( ID_DELETECOL, GridFrame::DeleteSelectedCols )
    EVT_MENU( ID_CLEARGRID, GridFrame::ClearGrid )
    EVT_MENU( ID_SELCELLS,  GridFrame::SelectCells )
    EVT_MENU( ID_SELROWS,  GridFrame::SelectRows )
    EVT_MENU( ID_SELCOLS,  GridFrame::SelectCols )
    EVT_MENU( ID_SELROWSORCOLS,  GridFrame::SelectRowsOrCols )

    EVT_MENU( ID_SET_CELL_FG_COLOUR, GridFrame::SetCellFgColour )
    EVT_MENU( ID_SET_CELL_BG_COLOUR, GridFrame::SetCellBgColour )

    EVT_MENU( wxID_ABOUT, GridFrame::About )
    EVT_MENU( wxID_EXIT, GridFrame::OnQuit )
    EVT_MENU( ID_VTABLE, GridFrame::OnVTable)
    EVT_MENU( ID_BUGS_TABLE, GridFrame::OnBugsTable)
    EVT_MENU( ID_TABULAR_TABLE, GridFrame::OnTabularTable)

    EVT_MENU( ID_DESELECT_CELL, GridFrame::DeselectCell)
    EVT_MENU( ID_DESELECT_COL, GridFrame::DeselectCol)
    EVT_MENU( ID_DESELECT_ROW, GridFrame::DeselectRow)
    EVT_MENU( ID_DESELECT_ALL, GridFrame::DeselectAll)
    EVT_MENU( ID_SELECT_CELL, GridFrame::SelectCell)
    EVT_MENU( ID_SELECT_COL, GridFrame::SelectCol)
    EVT_MENU( ID_SELECT_ROW, GridFrame::SelectRow)
    EVT_MENU( ID_SELECT_ALL, GridFrame::SelectAll)
    EVT_MENU( ID_SELECT_UNSELECT, GridFrame::OnAddToSelectToggle)

    EVT_MENU( ID_SIZE_ROW, GridFrame::AutoSizeRow )
    EVT_MENU( ID_SIZE_COL, GridFrame::AutoSizeCol )
    EVT_MENU( ID_SIZE_ROW_LABEL, GridFrame::AutoSizeRowLabel )
    EVT_MENU( ID_SIZE_COL_LABEL, GridFrame::AutoSizeColLabel )
    EVT_MENU( ID_SIZE_LABELS_COL, GridFrame::AutoSizeLabelsCol )
    EVT_MENU( ID_SIZE_LABELS_ROW, GridFrame::AutoSizeLabelsRow )
    EVT_MENU( ID_SIZE_GRID, GridFrame::AutoSizeTable )

    EVT_MENU( ID_SET_HIGHLIGHT_WIDTH, GridFrame::OnSetHighlightWidth)
    EVT_MENU( ID_SET_RO_HIGHLIGHT_WIDTH, GridFrame::OnSetROHighlightWidth)

    EVT_GRID_LABEL_LEFT_CLICK( GridFrame::OnLabelLeftClick )
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_ROW_SIZE( GridFrame::OnRowSize )
    EVT_GRID_COL_SIZE( GridFrame::OnColSize )
    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECT( GridFrame::OnRangeSelected )
    EVT_GRID_CELL_CHANGING( GridFrame::OnCellValueChanging )
    EVT_GRID_CELL_CHANGED( GridFrame::OnCellValueChanged )
    EVT_GRID_CELL_BEGIN_DRAG( GridFrame::OnCellBeginDrag )

    EVT_GRID_EDITOR_SHOWN( GridFrame::OnEditorShown )
    EVT_GRID_EDITOR_HIDDEN( GridFrame::OnEditorHidden )
END_EVENT_TABLE()


GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, wxID_ANY, wxT("wxWidgets grid class demo"),
                   wxDefaultPosition,
                   wxDefaultSize )
{
    SetIcon(wxICON(sample));

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append( ID_VTABLE, wxT("&Virtual table test\tCtrl-V"));
    fileMenu->Append( ID_BUGS_TABLE, wxT("&Bugs table test\tCtrl-B"));
    fileMenu->Append( ID_TABULAR_TABLE, wxT("&Tabular table test\tCtrl-T"));
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT, wxT("E&xit\tAlt-X") );

    wxMenu *viewMenu = new wxMenu;
    viewMenu->AppendCheckItem(ID_TOGGLEROWLABELS, "&Row labels");
    viewMenu->AppendCheckItem(ID_TOGGLECOLLABELS, "&Col labels");
    viewMenu->AppendCheckItem(ID_TOGGLEEDIT,"&Editable");
    viewMenu->AppendCheckItem(ID_TOGGLEROWSIZING, "Ro&w drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLECOLSIZING, "C&ol drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLECOLMOVING, "Col drag-&move");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDSIZING, "&Grid drag-resize");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDDRAGCELL, "&Grid drag-cell");
    viewMenu->AppendCheckItem(ID_TOGGLEGRIDLINES, "&Grid Lines");
    viewMenu->AppendCheckItem(ID_SET_HIGHLIGHT_WIDTH, "&Set Cell Highlight Width...");
    viewMenu->AppendCheckItem(ID_SET_RO_HIGHLIGHT_WIDTH, "&Set Cell RO Highlight Width...");
    viewMenu->AppendCheckItem(ID_AUTOSIZECOLS, "&Auto-size cols");
    viewMenu->AppendCheckItem(ID_CELLOVERFLOW, "&Overflow cells");
    viewMenu->AppendCheckItem(ID_RESIZECELL, "&Resize cell (7,1)");

    wxMenu *rowLabelMenu = new wxMenu;

    viewMenu->Append( ID_ROWLABELALIGN, wxT("R&ow label alignment"),
                      rowLabelMenu,
                      wxT("Change alignment of row labels") );

    rowLabelMenu->AppendRadioItem( ID_ROWLABELHORIZALIGN, wxT("&Horizontal") );
    rowLabelMenu->AppendRadioItem( ID_ROWLABELVERTALIGN, wxT("&Vertical") );

    wxMenu *colLabelMenu = new wxMenu;

    viewMenu->Append( ID_COLLABELALIGN, wxT("Col l&abel alignment"),
                      colLabelMenu,
                      wxT("Change alignment of col labels") );

    colLabelMenu->AppendRadioItem( ID_COLLABELHORIZALIGN, wxT("&Horizontal") );
    colLabelMenu->AppendRadioItem( ID_COLLABELVERTALIGN, wxT("&Vertical") );

    wxMenu *colHeaderMenu = new wxMenu;

    viewMenu->Append( ID_ROWLABELALIGN, wxT("Col header style"),
                      colHeaderMenu,
                      wxT("Change style of col header") );

    colHeaderMenu->AppendRadioItem( ID_COLDEFAULTHEADER, wxT("&Default") );
    colHeaderMenu->AppendRadioItem( ID_COLNATIVEHEADER, wxT("&Native") );
    colHeaderMenu->AppendRadioItem( ID_COLCUSTOMHEADER, wxT("&Custom") );


    wxMenu *colMenu = new wxMenu;
    colMenu->Append( ID_SETLABELCOLOUR, wxT("Set &label colour...") );
    colMenu->Append( ID_SETLABELTEXTCOLOUR, wxT("Set label &text colour...") );
    colMenu->Append( ID_SETLABEL_FONT, wxT("Set label fo&nt...") );
    colMenu->Append( ID_GRIDLINECOLOUR, wxT("&Grid line colour...") );
    colMenu->Append( ID_SET_CELL_FG_COLOUR, wxT("Set cell &foreground colour...") );
    colMenu->Append( ID_SET_CELL_BG_COLOUR, wxT("Set cell &background colour...") );

    wxMenu *editMenu = new wxMenu;
    editMenu->Append( ID_INSERTROW, wxT("Insert &row") );
    editMenu->Append( ID_INSERTCOL, wxT("Insert &column") );
    editMenu->Append( ID_DELETEROW, wxT("Delete selected ro&ws") );
    editMenu->Append( ID_DELETECOL, wxT("Delete selected co&ls") );
    editMenu->Append( ID_CLEARGRID, wxT("Cl&ear grid cell contents") );

    wxMenu *selectMenu = new wxMenu;
    selectMenu->Append( ID_SELECT_UNSELECT, wxT("Add new cells to the selection"),
                        wxT("When off, old selection is deselected before ")
                        wxT("selecting the new cells"), wxITEM_CHECK );
    selectMenu->AppendSeparator();
    selectMenu->Append( ID_SELECT_ALL, wxT("Select all"));
    selectMenu->Append( ID_SELECT_ROW, wxT("Select row 2"));
    selectMenu->Append( ID_SELECT_COL, wxT("Select col 2"));
    selectMenu->Append( ID_SELECT_CELL, wxT("Select cell (3, 1)"));
    selectMenu->AppendSeparator();
    selectMenu->Append( ID_DESELECT_ALL, wxT("Deselect all"));
    selectMenu->Append( ID_DESELECT_ROW, wxT("Deselect row 2"));
    selectMenu->Append( ID_DESELECT_COL, wxT("Deselect col 2"));
    selectMenu->Append( ID_DESELECT_CELL, wxT("Deselect cell (3, 1)"));
    wxMenu *selectionMenu = new wxMenu;
    selectMenu->Append( ID_CHANGESEL, wxT("Change &selection mode"),
                      selectionMenu,
                      wxT("Change selection mode") );

    selectionMenu->Append( ID_SELCELLS, wxT("Select &cells") );
    selectionMenu->Append( ID_SELROWS, wxT("Select &rows") );
    selectionMenu->Append( ID_SELCOLS, wxT("Select col&umns") );
    selectionMenu->Append( ID_SELROWSORCOLS, wxT("Select rows &or columns") );

    wxMenu *autosizeMenu = new wxMenu;
    autosizeMenu->Append( ID_SIZE_ROW, wxT("Selected &row data") );
    autosizeMenu->Append( ID_SIZE_COL, wxT("Selected &column data") );
    autosizeMenu->Append( ID_SIZE_ROW_LABEL, wxT("Selected row la&bel") );
    autosizeMenu->Append( ID_SIZE_COL_LABEL, wxT("Selected column &label") );
    autosizeMenu->Append( ID_SIZE_LABELS_COL, wxT("Column la&bels") );
    autosizeMenu->Append( ID_SIZE_LABELS_ROW, wxT("Row label&s") );
    autosizeMenu->Append( ID_SIZE_GRID, wxT("Entire &grid") );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append( wxID_ABOUT, wxT("&About wxGrid demo") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( fileMenu, wxT("&File") );
    menuBar->Append( viewMenu, wxT("&Grid") );
    menuBar->Append( colMenu,  wxT("&Colours") );
    menuBar->Append( editMenu, wxT("&Edit") );
    menuBar->Append( selectMenu, wxT("&Select") );
    menuBar->Append( autosizeMenu, wxT("&Autosize") );
    menuBar->Append( helpMenu, wxT("&Help") );

    SetMenuBar( menuBar );

    m_addToSel = false;

    grid = new wxGrid( this,
                       wxID_ANY,
                       wxPoint( 0, 0 ),
                       wxSize( 400, 300 ) );


#if wxUSE_LOG
    int gridW = 600, gridH = 300;
    int logW = gridW, logH = 100;

    logWin = new wxTextCtrl( this,
                             wxID_ANY,
                             wxEmptyString,
                             wxPoint( 0, gridH + 20 ),
                             wxSize( logW, logH ),
                             wxTE_MULTILINE );

    logger = new wxLogTextCtrl( logWin );
    m_logOld = wxLog::SetActiveTarget( logger );
    wxLog::DisableTimestamp();
#endif // wxUSE_LOG

    // this will create a grid and, by default, an associated grid
    // table for strings
    grid->CreateGrid( 0, 0 );

    grid->GetTable()->SetAttrProvider(new CustomColumnHeadersProvider());

    grid->AppendRows(100);
    grid->AppendCols(100);

    int ir = grid->GetNumberRows();
    grid->DeleteRows(0, ir);
    grid->AppendRows(ir);

    grid->SetRowSize( 0, 60 );
    grid->SetCellValue( 0, 0, wxT("Ctrl+Home\nwill go to\nthis cell") );

    grid->SetCellValue( 0, 1, wxT("A long piece of text to demonstrate wrapping.") );
    grid->SetCellRenderer(0 , 1, new wxGridCellAutoWrapStringRenderer);
    grid->SetCellEditor( 0,  1 , new wxGridCellAutoWrapStringEditor);

    grid->SetCellValue( 0, 2, wxT("Blah") );
    grid->SetCellValue( 0, 3, wxT("Read only") );
    grid->SetReadOnly( 0, 3 );

    grid->SetCellValue( 0, 4, wxT("Can veto edit this cell") );

    grid->SetCellValue( 0, 5, wxT("Press\nCtrl+arrow\nto skip over\ncells") );

    grid->SetRowSize( 99, 60 );
    grid->SetCellValue( 99, 99, wxT("Ctrl+End\nwill go to\nthis cell") );
    grid->SetCellValue( 1, 0, wxT("This default cell will overflow into neighboring cells, but not if you turn overflow off."));

    grid->SetCellTextColour(1, 2, *wxRED);
    grid->SetCellBackgroundColour(1, 2, *wxGREEN);

    grid->SetCellValue( 1, 4, wxT("I'm in the middle"));

    grid->SetCellValue(2, 2, wxT("red"));

    grid->SetCellTextColour(2, 2, *wxRED);
    grid->SetCellValue(3, 3, wxT("green on grey"));
    grid->SetCellTextColour(3, 3, *wxGREEN);
    grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);

    grid->SetCellValue(4, 4, wxT("a weird looking cell"));
    grid->SetCellAlignment(4, 4, wxALIGN_CENTRE, wxALIGN_CENTRE);
    grid->SetCellRenderer(4, 4, new MyGridCellRenderer);

    grid->SetCellRenderer(3, 0, new wxGridCellBoolRenderer);
    grid->SetCellEditor(3, 0, new wxGridCellBoolEditor);

    wxGridCellAttr *attr;
    attr = new wxGridCellAttr;
    attr->SetTextColour(*wxBLUE);
    grid->SetColAttr(5, attr);
    attr = new wxGridCellAttr;
    attr->SetBackgroundColour(*wxRED);
    grid->SetRowAttr(5, attr);

    grid->SetCellValue(2, 4, wxT("a wider column"));
    grid->SetColSize(4, 120);
    grid->SetColMinimalWidth(4, 120);

    grid->SetCellTextColour(5, 8, *wxGREEN);
    grid->SetCellValue(5, 8, wxT("Bg from row attr\nText col from cell attr"));
    grid->SetCellValue(5, 5, wxT("Bg from row attr Text col from col attr and this text is so long that it covers over many many empty cells but is broken by one that isn't"));

    grid->SetColFormatFloat(6);
    grid->SetCellValue(0, 6, wxString::Format(wxT("%g"), 3.1415));
    grid->SetCellValue(1, 6, wxString::Format(wxT("%g"), 1415.0));
    grid->SetCellValue(2, 6, wxString::Format(wxT("%g"), 12345.67890));

    grid->SetColFormatFloat(7, 6, 2);
    grid->SetCellValue(0, 7, wxString::Format(wxT("%g"), 3.1415));
    grid->SetCellValue(1, 7, wxString::Format(wxT("%g"), 1415.0));
    grid->SetCellValue(2, 7, wxString::Format(wxT("%g"), 12345.67890));

    grid->SetColFormatNumber(8);
    grid->SetCellValue(0, 8, "17");
    grid->SetCellValue(1, 8, "0");
    grid->SetCellValue(2, 8, "-666");
    grid->SetCellAlignment(2, 8, wxALIGN_CENTRE, wxALIGN_TOP);
    grid->SetCellValue(2, 9, "<- This numeric cell should be centred");

    const wxString choices[] =
    {
        wxT("Please select a choice"),
        wxT("This takes two cells"),
        wxT("Another choice"),
    };
    grid->SetCellEditor(4, 0, new wxGridCellChoiceEditor(WXSIZEOF(choices), choices));
    grid->SetCellSize(4, 0, 1, 2);
    grid->SetCellValue(4, 0, choices[0]);
    grid->SetCellOverflow(4, 0, false);

    grid->SetCellSize(7, 1, 3, 4);
    grid->SetCellAlignment(7, 1, wxALIGN_CENTRE, wxALIGN_CENTRE);
    grid->SetCellValue(7, 1, wxT("Big box!"));

    // create a separator-like row: it's grey and it's non-resizable
    grid->DisableRowResize(10);
    grid->SetRowSize(10, 30);
    attr = new wxGridCellAttr;
    attr->SetBackgroundColour(*wxLIGHT_GREY);
    grid->SetRowAttr(10, attr);
    grid->SetCellValue(10, 0, "You can't resize this row interactively -- try it");

    // this does exactly nothing except testing that SetAttr() handles NULL
    // attributes and does reference counting correctly
    grid->SetAttr(11, 11, NULL);
    grid->SetAttr(11, 11, new wxGridCellAttr);
    grid->SetAttr(11, 11, NULL);

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( grid,
                   1,
                   wxEXPAND );

#if wxUSE_LOG
    topSizer->Add( logWin,
                   0,
                   wxEXPAND );
#endif // wxUSE_LOG

    SetSizerAndFit( topSizer );

    Centre();
    SetDefaults();
}


GridFrame::~GridFrame()
{
#if wxUSE_LOG
    delete wxLog::SetActiveTarget(m_logOld);
#endif // wxUSE_LOG
}


void GridFrame::SetDefaults()
{
    GetMenuBar()->Check( ID_TOGGLEROWLABELS, true );
    GetMenuBar()->Check( ID_TOGGLECOLLABELS, true );
    GetMenuBar()->Check( ID_TOGGLEEDIT, true );
    GetMenuBar()->Check( ID_TOGGLEROWSIZING, true );
    GetMenuBar()->Check( ID_TOGGLECOLSIZING, true );
    GetMenuBar()->Check( ID_TOGGLECOLMOVING, false );
    GetMenuBar()->Check( ID_TOGGLEGRIDSIZING, true );
    GetMenuBar()->Check( ID_TOGGLEGRIDDRAGCELL, false );
    GetMenuBar()->Check( ID_TOGGLEGRIDLINES, true );
    GetMenuBar()->Check( ID_CELLOVERFLOW, true );
}


void GridFrame::ToggleRowLabels( wxCommandEvent& WXUNUSED(ev) )
{
    if ( GetMenuBar()->IsChecked( ID_TOGGLEROWLABELS ) )
    {
        grid->SetRowLabelSize( grid->GetDefaultRowLabelSize() );
    }
    else
    {
        grid->SetRowLabelSize( 0 );
    }
}


void GridFrame::ToggleColLabels( wxCommandEvent& WXUNUSED(ev) )
{
    if ( GetMenuBar()->IsChecked( ID_TOGGLECOLLABELS ) )
    {
        grid->SetColLabelSize( grid->GetDefaultColLabelSize() );
    }
    else
    {
        grid->SetColLabelSize( 0 );
    }
}


void GridFrame::ToggleEditing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableEditing(
        GetMenuBar()->IsChecked( ID_TOGGLEEDIT ) );
}


void GridFrame::ToggleRowSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragRowSize(
        GetMenuBar()->IsChecked( ID_TOGGLEROWSIZING ) );
}


void GridFrame::ToggleColSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragColSize(
        GetMenuBar()->IsChecked( ID_TOGGLECOLSIZING ) );
}

void GridFrame::ToggleColMoving( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragColMove(
        GetMenuBar()->IsChecked( ID_TOGGLECOLMOVING ) );
}

void GridFrame::ToggleGridSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragGridSize(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDSIZING ) );
}

void GridFrame::ToggleGridDragCell( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragCell(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDDRAGCELL ) );
}

void GridFrame::SetNativeColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(false);
    grid->SetUseNativeColLabels(true);
}

void GridFrame::SetCustomColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(true);
    grid->SetUseNativeColLabels(false);
}

void GridFrame::SetDefaultColHeader( wxCommandEvent& WXUNUSED(ev) )
{
    CustomColumnHeadersProvider* provider =
        static_cast<CustomColumnHeadersProvider*>(grid->GetTable()->GetAttrProvider());
    provider->UseCustomColHeaders(false);
    grid->SetUseNativeColLabels(false);
}


void GridFrame::ToggleGridLines( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableGridLines(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDLINES ) );
}

void GridFrame::OnSetHighlightWidth( wxCommandEvent& WXUNUSED(ev) )
{
    wxString choices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10")};

    wxSingleChoiceDialog dlg(this, wxT("Choose the thickness of the highlight pen:"),
                             wxT("Pen Width"), 11, choices);

    int current = grid->GetCellHighlightPenWidth();
    dlg.SetSelection(current);
    if (dlg.ShowModal() == wxID_OK) {
        grid->SetCellHighlightPenWidth(dlg.GetSelection());
    }
}

void GridFrame::OnSetROHighlightWidth( wxCommandEvent& WXUNUSED(ev) )
{
    wxString choices[] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10")};

    wxSingleChoiceDialog dlg(this, wxT("Choose the thickness of the highlight pen:"),
                             wxT("Pen Width"), 11, choices);

    int current = grid->GetCellHighlightROPenWidth();
    dlg.SetSelection(current);
    if (dlg.ShowModal() == wxID_OK) {
        grid->SetCellHighlightROPenWidth(dlg.GetSelection());
    }
}



void GridFrame::AutoSizeCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->AutoSizeColumns();
    grid->Refresh();
}

void GridFrame::CellOverflow( wxCommandEvent& ev )
{
    grid->SetDefaultCellOverflow(ev.IsChecked());
    grid->Refresh();
}

void GridFrame::ResizeCell( wxCommandEvent& ev )
{
    if (ev.IsChecked())
        grid->SetCellSize( 7, 1, 5, 5 );
    else
        grid->SetCellSize( 7, 1, 1, 5 );
    grid->Refresh();
}

void GridFrame::SetLabelColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( NULL );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetLabelBackgroundColour( colour );
    }
}


void GridFrame::SetLabelTextColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( NULL );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetLabelTextColour( colour );
    }
}

void GridFrame::SetLabelFont( wxCommandEvent& WXUNUSED(ev) )
{
    wxFont font = wxGetFontFromUser(this);
    if ( font.IsOk() )
    {
        grid->SetLabelFont(font);
    }
}

void GridFrame::SetRowLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxALIGN_LEFT:
            horiz = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            horiz = wxALIGN_RIGHT;
            break;

        case wxALIGN_RIGHT:
            horiz = wxALIGN_LEFT;
            break;
    }

    grid->SetRowLabelAlignment( horiz, vert );
}

void GridFrame::SetRowLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxALIGN_TOP:
            vert = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            vert = wxALIGN_BOTTOM;
            break;

        case wxALIGN_BOTTOM:
            vert = wxALIGN_TOP;
            break;
    }

    grid->SetRowLabelAlignment( horiz, vert );
}


void GridFrame::SetColLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxALIGN_LEFT:
            horiz = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            horiz = wxALIGN_RIGHT;
            break;

        case wxALIGN_RIGHT:
            horiz = wxALIGN_LEFT;
            break;
    }

    grid->SetColLabelAlignment( horiz, vert );
}


void GridFrame::SetColLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxALIGN_TOP:
            vert = wxALIGN_CENTRE;
            break;

        case wxALIGN_CENTRE:
            vert = wxALIGN_BOTTOM;
            break;

        case wxALIGN_BOTTOM:
            vert = wxALIGN_TOP;
            break;
    }

    grid->SetColLabelAlignment( horiz, vert );
}


void GridFrame::SetGridLineColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColourDialog dlg( NULL );
    if ( dlg.ShowModal() == wxID_OK )
    {
        wxColourData retData;
        retData = dlg.GetColourData();
        wxColour colour = retData.GetColour();

        grid->SetGridLineColour( colour );
    }
}


void GridFrame::InsertRow( wxCommandEvent& WXUNUSED(ev) )
{
    grid->InsertRows( grid->GetGridCursorRow(), 1 );
}


void GridFrame::InsertCol( wxCommandEvent& WXUNUSED(ev) )
{
    grid->InsertCols( grid->GetGridCursorCol(), 1 );
}


void GridFrame::DeleteSelectedRows( wxCommandEvent& WXUNUSED(ev) )
{
    if ( grid->IsSelection() )
    {
        wxGridUpdateLocker locker(grid);
        for ( int n = 0; n < grid->GetNumberRows(); )
        {
            if ( grid->IsInSelection( n , 0 ) )
                grid->DeleteRows( n, 1 );
            else
                n++;
        }
    }
}


void GridFrame::AutoSizeRow(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedRows();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeRow( sels[n], false );
    }
}

void GridFrame::AutoSizeCol(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedCols();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeColumn( sels[n], false );
    }
}

void GridFrame::AutoSizeRowLabel(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedRows();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeRowLabelSize( sels[n] );
    }
}

void GridFrame::AutoSizeColLabel(wxCommandEvent& WXUNUSED(event))
{
    wxGridUpdateLocker locker(grid);
    const wxArrayInt sels  = grid->GetSelectedCols();
    for ( size_t n = 0, count = sels.size(); n < count; n++ )
    {
        grid->AutoSizeColLabelSize( sels[n] );
    }
}

void GridFrame::AutoSizeLabelsCol(wxCommandEvent& WXUNUSED(event))
{
    grid->SetColLabelSize( wxGRID_AUTOSIZE );
}

void GridFrame::AutoSizeLabelsRow(wxCommandEvent& WXUNUSED(event))
{
    grid->SetRowLabelSize( wxGRID_AUTOSIZE );
}

void GridFrame::AutoSizeTable(wxCommandEvent& WXUNUSED(event))
{
    grid->AutoSize();
}


void GridFrame::DeleteSelectedCols( wxCommandEvent& WXUNUSED(ev) )
{
    if ( grid->IsSelection() )
    {
        wxGridUpdateLocker locker(grid);
        for ( int n = 0; n < grid->GetNumberCols(); )
        {
            if ( grid->IsInSelection( 0 , n ) )
                grid->DeleteCols( n, 1 );
            else
                n++;
        }
    }
}


void GridFrame::ClearGrid( wxCommandEvent& WXUNUSED(ev) )
{
    grid->ClearGrid();
}

void GridFrame::SelectCells( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectCells );
}

void GridFrame::SelectRows( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectRows );
}

void GridFrame::SelectCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectColumns );
}

void GridFrame::SelectRowsOrCols( wxCommandEvent& WXUNUSED(ev) )
{
    grid->SetSelectionMode( wxGrid::wxGridSelectRowsOrColumns );
}

void GridFrame::SetCellFgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.IsOk() )
    {
        grid->SetDefaultCellTextColour(col);
        grid->Refresh();
    }
}

void GridFrame::SetCellBgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.IsOk() )
    {
        // Check the new Refresh function by passing it a rectangle
        // which exactly fits the grid.
        wxPoint pt(0, 0);
        wxRect r(pt, grid->GetSize());
        grid->SetDefaultCellBackgroundColour(col);
        grid->Refresh(true, &r);
    }
}

void GridFrame::DeselectCell(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectCell(3, 1);
}

void GridFrame::DeselectCol(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectCol(2);
}

void GridFrame::DeselectRow(wxCommandEvent& WXUNUSED(event))
{
      grid->DeselectRow(2);
}

void GridFrame::DeselectAll(wxCommandEvent& WXUNUSED(event))
{
      grid->ClearSelection();
}

void GridFrame::SelectCell(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectBlock(3, 1, 3, 1, m_addToSel);
}

void GridFrame::SelectCol(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectCol(2, m_addToSel);
}

void GridFrame::SelectRow(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectRow(2, m_addToSel);
}

void GridFrame::SelectAll(wxCommandEvent& WXUNUSED(event))
{
      grid->SelectAll();
}

void GridFrame::OnAddToSelectToggle(wxCommandEvent& event)
{
    m_addToSel = event.IsChecked();
}

void GridFrame::OnLabelLeftClick( wxGridEvent& ev )
{
    wxString logBuf;
    if ( ev.GetRow() != -1 )
    {
        logBuf << wxT("Left click on row label ") << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << wxT("Left click on col label ") << ev.GetCol();
    }
    else
    {
        logBuf << wxT("Left click on corner label");
    }

    if ( ev.ShiftDown() )
        logBuf << wxT(" (shift down)");
    if ( ev.ControlDown() )
        logBuf << wxT(" (control down)");
    wxLogMessage( wxT("%s"), logBuf.c_str() );

    // you must call event skip if you want default grid processing
    //
    ev.Skip();
}


void GridFrame::OnCellLeftClick( wxGridEvent& ev )
{
    wxLogMessage(wxT("Left click at row %d, col %d"), ev.GetRow(), ev.GetCol());

    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}


void GridFrame::OnRowSize( wxGridSizeEvent& ev )
{
    const int row = ev.GetRowOrCol();

    wxLogMessage("Resized row %d, new height = %d",
                 row, grid->GetRowSize(row));

    ev.Skip();
}


void GridFrame::OnColSize( wxGridSizeEvent& ev )
{
    const int col = ev.GetRowOrCol();

    wxLogMessage("Resized column %d, new width = %d",
                 col, grid->GetColSize(col));

    ev.Skip();
}


void GridFrame::OnSelectCell( wxGridEvent& ev )
{
    wxString logBuf;
    if ( ev.Selecting() )
        logBuf << wxT("Selected ");
    else
        logBuf << wxT("Deselected ");
    logBuf << wxT("cell at row ") << ev.GetRow()
           << wxT(" col ") << ev.GetCol()
           << wxT(" ( ControlDown: ")<< (ev.ControlDown() ? 'T':'F')
           << wxT(", ShiftDown: ")<< (ev.ShiftDown() ? 'T':'F')
           << wxT(", AltDown: ")<< (ev.AltDown() ? 'T':'F')
           << wxT(", MetaDown: ")<< (ev.MetaDown() ? 'T':'F') << wxT(" )");

    //Indicate whether this column was moved
    if ( ((wxGrid *)ev.GetEventObject())->GetColPos( ev.GetCol() ) != ev.GetCol() )
        logBuf << wxT(" *** Column moved, current position: ") << ((wxGrid *)ev.GetEventObject())->GetColPos( ev.GetCol() );

    wxLogMessage( wxT("%s"), logBuf.c_str() );

    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
}

void GridFrame::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
    wxString logBuf;
    if ( ev.Selecting() )
        logBuf << wxT("Selected ");
    else
        logBuf << wxT("Deselected ");
    logBuf << wxT("cells from row ") << ev.GetTopRow()
           << wxT(" col ") << ev.GetLeftCol()
           << wxT(" to row ") << ev.GetBottomRow()
           << wxT(" col ") << ev.GetRightCol()
           << wxT(" ( ControlDown: ")<< (ev.ControlDown() ? 'T':'F')
           << wxT(", ShiftDown: ")<< (ev.ShiftDown() ? 'T':'F')
           << wxT(", AltDown: ")<< (ev.AltDown() ? 'T':'F')
           << wxT(", MetaDown: ")<< (ev.MetaDown() ? 'T':'F') << wxT(" )");
    wxLogMessage( wxT("%s"), logBuf.c_str() );

    ev.Skip();
}

void GridFrame::OnCellValueChanging( wxGridEvent& ev )
{
    int row = ev.GetRow(),
        col = ev.GetCol();

    wxLogMessage("Value of cell at (%d, %d): about to change "
                 "from \"%s\" to \"%s\"",
                 row, col,
                 grid->GetCellValue(row, col), ev.GetString());

    // test how vetoing works
    if ( ev.GetString() == "42" )
    {
        wxLogMessage("Vetoing the change.");
        ev.Veto();
        return;
    }

    ev.Skip();
}

void GridFrame::OnCellValueChanged( wxGridEvent& ev )
{
    int row = ev.GetRow(),
        col = ev.GetCol();

    wxLogMessage("Value of cell at (%d, %d) changed and is now \"%s\" "
                 "(was \"%s\")",
                 row, col,
                 grid->GetCellValue(row, col), ev.GetString());

    ev.Skip();
}

void GridFrame::OnCellBeginDrag( wxGridEvent& ev )
{
    wxLogMessage(wxT("Got request to drag cell at row %d, col %d"),
                 ev.GetRow(), ev.GetCol());

    ev.Skip();
}

void GridFrame::OnEditorShown( wxGridEvent& ev )
{

    if ( (ev.GetCol() == 4) &&
         (ev.GetRow() == 0) &&
     (wxMessageBox(wxT("Are you sure you wish to edit this cell"),
                   wxT("Checking"),wxYES_NO) == wxNO ) ) {

     ev.Veto();
     return;
    }

    wxLogMessage( wxT("Cell editor shown.") );

    ev.Skip();
}

void GridFrame::OnEditorHidden( wxGridEvent& ev )
{

    if ( (ev.GetCol() == 4) &&
         (ev.GetRow() == 0) &&
     (wxMessageBox(wxT("Are you sure you wish to finish editing this cell"),
                   wxT("Checking"),wxYES_NO) == wxNO ) ) {

        ev.Veto();
        return;
    }

    wxLogMessage( wxT("Cell editor hidden.") );

    ev.Skip();
}

void GridFrame::About(  wxCommandEvent& WXUNUSED(ev) )
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(wxT("wxGrid demo"));
    aboutInfo.SetDescription(_("wxGrid sample program"));
    aboutInfo.AddDeveloper(wxT("Michael Bedward"));
    aboutInfo.AddDeveloper(wxT("Julian Smart"));
    aboutInfo.AddDeveloper(wxT("Vadim Zeitlin"));

    // this is just to force the generic version of the about
    // dialog under wxMSW so that it's easy to test if the grid
    // repaints correctly when it has lost focus and a dialog
    // (different from the Windows standard message box -- it doesn't
    // work with it for some reason) is moved over it.
    aboutInfo.SetWebSite(wxT("http://www.wxwidgets.org"));

    wxAboutBox(aboutInfo);
}


void GridFrame::OnQuit( wxCommandEvent& WXUNUSED(ev) )
{
    Close( true );
}

void GridFrame::OnBugsTable(wxCommandEvent& )
{
    BugsGridFrame *frame = new BugsGridFrame;
    frame->Show(true);
}

// ----------------------------------------------------------------------------
// MyGridCellAttrProvider
// ----------------------------------------------------------------------------

MyGridCellAttrProvider::MyGridCellAttrProvider()
{
    m_attrForOddRows = new wxGridCellAttr;
    m_attrForOddRows->SetBackgroundColour(*wxLIGHT_GREY);
}

MyGridCellAttrProvider::~MyGridCellAttrProvider()
{
    m_attrForOddRows->DecRef();
}

wxGridCellAttr *MyGridCellAttrProvider::GetAttr(int row, int col,
                           wxGridCellAttr::wxAttrKind  kind /* = wxGridCellAttr::Any */) const
{
    wxGridCellAttr *attr = wxGridCellAttrProvider::GetAttr(row, col, kind);

    if ( row % 2 )
    {
        if ( !attr )
        {
            attr = m_attrForOddRows;
            attr->IncRef();
        }
        else
        {
            if ( !attr->HasBackgroundColour() )
            {
                wxGridCellAttr *attrNew = attr->Clone();
                attr->DecRef();
                attr = attrNew;
                attr->SetBackgroundColour(*wxLIGHT_GREY);
            }
        }
    }

    return attr;
}

void GridFrame::OnVTable(wxCommandEvent& )
{
    static long s_sizeGrid = 10000;

    s_sizeGrid = wxGetNumberFromUser(wxT("Size of the table to create"),
                                     wxT("Size: "),
                                     wxT("wxGridDemo question"),
                                     s_sizeGrid,
                                     0, 32000, this);

    if ( s_sizeGrid != -1 )
    {
        BigGridFrame* win = new BigGridFrame(s_sizeGrid);
        win->Show(true);
    }
}

// ----------------------------------------------------------------------------
// MyGridCellRenderer
// ----------------------------------------------------------------------------

// do something that the default renderer doesn't here just to show that it is
// possible to alter the appearance of the cell beyond what the attributes
// allow
void MyGridCellRenderer::Draw(wxGrid& grid,
                              wxGridCellAttr& attr,
                              wxDC& dc,
                              const wxRect& rect,
                              int row, int col,
                              bool isSelected)
{
    wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);

    dc.SetPen(*wxGREEN_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawEllipse(rect);
}

// ============================================================================
// BigGridFrame and BigGridTable:  Sample of a non-standard table
// ============================================================================

BigGridFrame::BigGridFrame(long sizeGrid)
            : wxFrame(NULL, wxID_ANY, wxT("Plugin Virtual Table"),
                      wxDefaultPosition, wxSize(500, 450))
{
    m_grid = new wxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_table = new BigGridTable(sizeGrid);

    // VZ: I don't understand why this slows down the display that much,
    //     must profile it...
    //m_table->SetAttrProvider(new MyGridCellAttrProvider);

    m_grid->SetTable(m_table, true);

#if defined __WXMOTIF__
    // MB: the grid isn't getting a sensible default size under wxMotif
    int cw, ch;
    GetClientSize( &cw, &ch );
    m_grid->SetSize( cw, ch );
#endif
}

// ============================================================================
// BugsGridFrame: a "realistic" table
// ============================================================================

// ----------------------------------------------------------------------------
// bugs table data
// ----------------------------------------------------------------------------

enum Columns
{
    Col_Id,
    Col_Summary,
    Col_Severity,
    Col_Priority,
    Col_Platform,
    Col_Opened,
    Col_Max
};

enum Severity
{
    Sev_Wish,
    Sev_Minor,
    Sev_Normal,
    Sev_Major,
    Sev_Critical,
    Sev_Max
};

static const wxString severities[] =
{
    wxT("wishlist"),
    wxT("minor"),
    wxT("normal"),
    wxT("major"),
    wxT("critical"),
};

static struct BugsGridData
{
    int id;
    wxChar summary[80];
    Severity severity;
    int prio;
    wxChar platform[12];
    bool opened;
} gs_dataBugsGrid [] =
{
    { 18, wxT("foo doesn't work"), Sev_Major, 1, wxT("wxMSW"), true },
    { 27, wxT("bar crashes"), Sev_Critical, 1, wxT("all"), false },
    { 45, wxT("printing is slow"), Sev_Minor, 3, wxT("wxMSW"), true },
    { 68, wxT("Rectangle() fails"), Sev_Normal, 1, wxT("wxMSW"), false },
};

static const wxChar *headers[Col_Max] =
{
    wxT("Id"),
    wxT("Summary"),
    wxT("Severity"),
    wxT("Priority"),
    wxT("Platform"),
    wxT("Opened?"),
};

// ----------------------------------------------------------------------------
// BugsGridTable
// ----------------------------------------------------------------------------

wxString BugsGridTable::GetTypeName(int WXUNUSED(row), int col)
{
    switch ( col )
    {
        case Col_Id:
        case Col_Priority:
            return wxGRID_VALUE_NUMBER;;

        case Col_Severity:
            // fall thorugh (TODO should be a list)

        case Col_Summary:
            return wxString::Format(wxT("%s:80"), wxGRID_VALUE_STRING);

        case Col_Platform:
            return wxString::Format(wxT("%s:all,MSW,GTK,other"), wxGRID_VALUE_CHOICE);

        case Col_Opened:
            return wxGRID_VALUE_BOOL;
    }

    wxFAIL_MSG(wxT("unknown column"));

    return wxEmptyString;
}

int BugsGridTable::GetNumberRows()
{
    return WXSIZEOF(gs_dataBugsGrid);
}

int BugsGridTable::GetNumberCols()
{
    return Col_Max;
}

bool BugsGridTable::IsEmptyCell( int WXUNUSED(row), int WXUNUSED(col) )
{
    return false;
}

wxString BugsGridTable::GetValue( int row, int col )
{
    const BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
            return wxString::Format(wxT("%d"), gd.id);

        case Col_Priority:
            return wxString::Format(wxT("%d"), gd.prio);

        case Col_Opened:
            return gd.opened ? wxT("1") : wxT("0");

        case Col_Severity:
            return severities[gd.severity];

        case Col_Summary:
            return gd.summary;

        case Col_Platform:
            return gd.platform;
    }

    return wxEmptyString;
}

void BugsGridTable::SetValue( int row, int col, const wxString& value )
{
    BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
        case Col_Priority:
        case Col_Opened:
            wxFAIL_MSG(wxT("unexpected column"));
            break;

        case Col_Severity:
            {
                size_t n;
                for ( n = 0; n < WXSIZEOF(severities); n++ )
                {
                    if ( severities[n] == value )
                    {
                        gd.severity = (Severity)n;
                        break;
                    }
                }

                if ( n == WXSIZEOF(severities) )
                {
                    wxLogWarning(wxT("Invalid severity value '%s'."),
                                 value.c_str());
                    gd.severity = Sev_Normal;
                }
            }
            break;

        case Col_Summary:
            wxStrncpy(gd.summary, value, WXSIZEOF(gd.summary));
            break;

        case Col_Platform:
            wxStrncpy(gd.platform, value, WXSIZEOF(gd.platform));
            break;
    }
}

bool
BugsGridTable::CanGetValueAs(int WXUNUSED(row),
                             int col,
                             const wxString& typeName)
{
    if ( typeName == wxGRID_VALUE_STRING )
    {
        return true;
    }
    else if ( typeName == wxGRID_VALUE_BOOL )
    {
        return col == Col_Opened;
    }
    else if ( typeName == wxGRID_VALUE_NUMBER )
    {
        return col == Col_Id || col == Col_Priority || col == Col_Severity;
    }
    else
    {
        return false;
    }
}

bool BugsGridTable::CanSetValueAs( int row, int col, const wxString& typeName )
{
    return CanGetValueAs(row, col, typeName);
}

long BugsGridTable::GetValueAsLong( int row, int col )
{
    const BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
            return gd.id;

        case Col_Priority:
            return gd.prio;

        case Col_Severity:
            return gd.severity;

        default:
            wxFAIL_MSG(wxT("unexpected column"));
            return -1;
    }
}

bool BugsGridTable::GetValueAsBool( int row, int col )
{
    if ( col == Col_Opened )
    {
        return gs_dataBugsGrid[row].opened;
    }
    else
    {
        wxFAIL_MSG(wxT("unexpected column"));

        return false;
    }
}

void BugsGridTable::SetValueAsLong( int row, int col, long value )
{
    BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Priority:
            gd.prio = value;
            break;

        default:
            wxFAIL_MSG(wxT("unexpected column"));
    }
}

void BugsGridTable::SetValueAsBool( int row, int col, bool value )
{
    if ( col == Col_Opened )
    {
        gs_dataBugsGrid[row].opened = value;
    }
    else
    {
        wxFAIL_MSG(wxT("unexpected column"));
    }
}

wxString BugsGridTable::GetColLabelValue( int col )
{
    return headers[col];
}

// ----------------------------------------------------------------------------
// BugsGridFrame
// ----------------------------------------------------------------------------

BugsGridFrame::BugsGridFrame()
             : wxFrame(NULL, wxID_ANY, wxT("Bugs table"))
{
    wxGrid *grid = new wxGrid(this, wxID_ANY);
    wxGridTableBase *table = new BugsGridTable();
    table->SetAttrProvider(new MyGridCellAttrProvider);
    grid->SetTable(table, true);

    wxGridCellAttr *attrRO = new wxGridCellAttr,
                   *attrRangeEditor = new wxGridCellAttr,
                   *attrCombo = new wxGridCellAttr;

    attrRO->SetReadOnly();
    attrRangeEditor->SetEditor(new wxGridCellNumberEditor(1, 5));
    attrCombo->SetEditor(new wxGridCellChoiceEditor(WXSIZEOF(severities),
                                                    severities));

    grid->SetColAttr(Col_Id, attrRO);
    grid->SetColAttr(Col_Priority, attrRangeEditor);
    grid->SetColAttr(Col_Severity, attrCombo);

    grid->Fit();
    SetClientSize(grid->GetSize());
}

// ============================================================================
// TabularGrid: grid used for display of tabular data
// ============================================================================

class TabularGridTable : public wxGridTableBase
{
public:
    enum
    {
        COL_NAME,
        COL_EXT,
        COL_SIZE,
        COL_DATE,
        COL_MAX
    };

    enum
    {
        ROW_MAX = 3
    };

    TabularGridTable() { m_sortOrder = NULL; }

    virtual int GetNumberRows() { return ROW_MAX; }
    virtual int GetNumberCols() { return COL_MAX; }

    virtual wxString GetValue(int row, int col)
    {
        if ( m_sortOrder )
            row = m_sortOrder[row];

        switch ( col )
        {
            case COL_NAME:
            case COL_EXT:
                return GetNameOrExt(row, col);

            case COL_SIZE:
                return wxString::Format("%lu", GetSize(row));

            case COL_DATE:
                return GetDate(row).FormatDate();

            case COL_MAX:
            default:
                wxFAIL_MSG( "unknown column" );
        }

        return wxString();
    }

    virtual void SetValue(int, int, const wxString&)
    {
        wxFAIL_MSG( "shouldn't be called" );
    }

    virtual wxString GetColLabelValue(int col)
    {
        // notice that column parameter here always refers to the internal
        // column index, independently of its position on the screen
        static const char *labels[] = { "Name", "Extension", "Size", "Date" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(labels) == COL_MAX, LabelsMismatch );

        return labels[col];
    }

    virtual void SetColLabelValue(int, const wxString&)
    {
        wxFAIL_MSG( "shouldn't be called" );
    }

    void Sort(int col, bool ascending)
    {
        // we hardcode all sorting orders for simplicity here
        static int sortOrders[COL_MAX][2][ROW_MAX] =
        {
            // descending   ascending
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 2, 1, 0 }, { 0, 1, 2 } },
            { { 1, 0, 2 }, { 2, 0, 1 } },
        };

        m_sortOrder = col == wxNOT_FOUND ? NULL : sortOrders[col][ascending];
    }

private:
    wxString GetNameOrExt(int row, int col) const
    {
        static const char *
            names[] = { "autoexec.bat", "boot.ini", "io.sys" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(names) == ROW_MAX, NamesMismatch );

        const wxString s(names[row]);
        return col == COL_NAME ? s.BeforeFirst('.') : s.AfterLast('.');
    }

    unsigned long GetSize(int row) const
    {
        static const unsigned long
            sizes[] = { 412, 604, 40774 };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(sizes) == ROW_MAX, SizesMismatch );

        return sizes[row];
    }

    wxDateTime GetDate(int row) const
    {
        static const char *
            dates[] = { "2004-04-17", "2006-05-27", "1994-05-31" };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(dates) == ROW_MAX, DatesMismatch );

        wxDateTime dt;
        dt.ParseISODate(dates[row]);
        return dt;
    }

    int *m_sortOrder;
};

// specialized text control for column indexes entry
class ColIndexEntry : public wxTextCtrl
{
public:
    ColIndexEntry(wxWindow *parent)
        : wxTextCtrl(parent, wxID_ANY, "")
    {
        SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    }

    int GetCol()
    {
        unsigned long col;
        if ( !GetValue().ToULong(&col) || col > TabularGridTable::COL_MAX )
        {
            SetFocus();
            return -1;
        }

        return col;
    }

protected:
    virtual wxSize DoGetBestSize() const
    {
        wxSize size = wxTextCtrl::DoGetBestSize();
        size.x = 3*GetCharWidth();
        return size;
    }
};

class TabularGridFrame : public wxFrame
{
public:
    TabularGridFrame();

private:
    enum // control ids
    {
        Id_Check_UseNativeHeader,
        Id_Check_DrawNativeLabels,
        Id_Check_ShowRowLabels,
        Id_Check_EnableColMove
    };

    // event handlers

    void OnToggleUseNativeHeader(wxCommandEvent&)
    {
        m_grid->UseNativeColHeader(m_chkUseNative->IsChecked());
    }

    void OnUpdateDrawNativeLabelsUI(wxUpdateUIEvent& event)
    {
        // we don't draw labels at all, native or otherwise, if we use the
        // native header control
        event.Enable( !m_chkUseNative->GetValue() );
    }

    void OnToggleDrawNativeLabels(wxCommandEvent&)
    {
        m_grid->SetUseNativeColLabels(m_chkDrawNative->IsChecked());
    }

    void OnToggleShowRowLabels(wxCommandEvent&)
    {
        m_grid->SetRowLabelSize(m_chkShowRowLabels->IsChecked()
                                    ? wxGRID_AUTOSIZE
                                    : 0);
    }

    void OnToggleColMove(wxCommandEvent&)
    {
        m_grid->EnableDragColMove(m_chkEnableColMove->IsChecked());
    }

    void OnShowHideColumn(wxCommandEvent& event)
    {
        int col = m_txtColShowHide->GetCol();
        if ( col != -1 )
        {
            m_grid->SetColSize(col,
                               event.GetId() == wxID_ADD ? wxGRID_AUTOSIZE : 0);

            UpdateOrderAndVisibility();
        }
    }

    void OnMoveColumn(wxCommandEvent&)
    {
        int col = m_txtColIndex->GetCol();
        int pos = m_txtColPos->GetCol();
        if ( col == -1 || pos == -1 )
            return;

        m_grid->SetColPos(col, pos);

        UpdateOrderAndVisibility();
    }

    void OnResetColumnOrder(wxCommandEvent&)
    {
        m_grid->ResetColPos();

        UpdateOrderAndVisibility();
    }

    void OnGridColSort(wxGridEvent& event)
    {
        const int col = event.GetCol();
        m_table->Sort(col, !(m_grid->IsSortingBy(col) &&
                             m_grid->IsSortOrderAscending()));
    }

    void OnGridColMove(wxGridEvent& event)
    {
        // can't update it yet as the order hasn't been changed, so do it a bit
        // later
        m_shouldUpdateOrder = true;

        event.Skip();
    }

    void OnGridColSize(wxGridSizeEvent& event)
    {
        // we only catch this event to react to the user showing or hiding this
        // column using the header control menu and not because we're
        // interested in column resizing
        UpdateOrderAndVisibility();

        event.Skip();
    }

    void OnIdle(wxIdleEvent& event)
    {
        if ( m_shouldUpdateOrder )
        {
            m_shouldUpdateOrder = false;
            UpdateOrderAndVisibility();
        }

        event.Skip();
    }

    void UpdateOrderAndVisibility()
    {
        wxString s;
        for ( int pos = 0; pos < TabularGridTable::COL_MAX; pos++ )
        {
            const int col = m_grid->GetColAt(pos);
            const bool isHidden = m_grid->GetColSize(col) == 0;

            if ( isHidden )
                s << '[';
            s << col;
            if ( isHidden )
                s << ']';

            s << ' ';
        }

        m_statOrder->SetLabel(s);
    }

    // controls
    wxGrid *m_grid;
    TabularGridTable *m_table;
    wxCheckBox *m_chkUseNative,
               *m_chkDrawNative,
               *m_chkShowRowLabels,
               *m_chkEnableColMove;

    ColIndexEntry *m_txtColIndex,
                  *m_txtColPos,
                  *m_txtColShowHide;

    wxStaticText *m_statOrder;

    // fla for EVT_IDLE handler
    bool m_shouldUpdateOrder;

    wxDECLARE_NO_COPY_CLASS(TabularGridFrame);
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TabularGridFrame, wxFrame)
    EVT_CHECKBOX(Id_Check_UseNativeHeader,
                 TabularGridFrame::OnToggleUseNativeHeader)
    EVT_CHECKBOX(Id_Check_DrawNativeLabels,
                 TabularGridFrame::OnToggleDrawNativeLabels)
    EVT_CHECKBOX(Id_Check_ShowRowLabels,
                 TabularGridFrame::OnToggleShowRowLabels)
    EVT_CHECKBOX(Id_Check_EnableColMove,
                 TabularGridFrame::OnToggleColMove)

    EVT_UPDATE_UI(Id_Check_DrawNativeLabels,
                  TabularGridFrame::OnUpdateDrawNativeLabelsUI)

    EVT_BUTTON(wxID_APPLY, TabularGridFrame::OnMoveColumn)
    EVT_BUTTON(wxID_RESET, TabularGridFrame::OnResetColumnOrder)
    EVT_BUTTON(wxID_ADD, TabularGridFrame::OnShowHideColumn)
    EVT_BUTTON(wxID_DELETE, TabularGridFrame::OnShowHideColumn)

    EVT_GRID_COL_SORT(TabularGridFrame::OnGridColSort)
    EVT_GRID_COL_MOVE(TabularGridFrame::OnGridColMove)
    EVT_GRID_COL_SIZE(TabularGridFrame::OnGridColSize)

    EVT_IDLE(TabularGridFrame::OnIdle)
END_EVENT_TABLE()

TabularGridFrame::TabularGridFrame()
                : wxFrame(NULL, wxID_ANY, "Tabular table")
{
    m_shouldUpdateOrder = false;

    wxPanel * const panel = new wxPanel(this);

    // create and initialize the grid with the specified data
    m_table = new TabularGridTable;
    m_grid = new wxGrid(panel, wxID_ANY,
                        wxDefaultPosition, wxDefaultSize,
                        wxBORDER_STATIC | wxWANTS_CHARS);
    m_grid->SetTable(m_table, true, wxGrid::wxGridSelectRows);

    m_grid->EnableDragColMove();
    m_grid->UseNativeColHeader();
    m_grid->HideRowLabels();

    // add it and the other controls to the frame
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(m_grid, wxSizerFlags(1).Expand().Border());

    wxSizer * const sizerControls = new wxBoxSizer(wxHORIZONTAL);

    wxSizer * const sizerStyles = new wxBoxSizer(wxVERTICAL);
    m_chkUseNative = new wxCheckBox(panel, Id_Check_UseNativeHeader,
                                    "&Use native header");
    m_chkUseNative->SetValue(true);
    sizerStyles->Add(m_chkUseNative, wxSizerFlags().Border());

    m_chkDrawNative = new wxCheckBox(panel, Id_Check_DrawNativeLabels,
                                    "&Draw native column labels");
    sizerStyles->Add(m_chkDrawNative, wxSizerFlags().Border());

    m_chkShowRowLabels = new wxCheckBox(panel, Id_Check_ShowRowLabels,
                                        "Show &row labels");
    sizerStyles->Add(m_chkShowRowLabels, wxSizerFlags().Border());

    m_chkEnableColMove = new wxCheckBox(panel, Id_Check_EnableColMove,
                                        "Allow column re&ordering");
    m_chkEnableColMove->SetValue(true);
    sizerStyles->Add(m_chkEnableColMove, wxSizerFlags().Border());
    sizerControls->Add(sizerStyles);

    sizerControls->AddSpacer(10);

    wxSizer * const sizerColumns = new wxBoxSizer(wxVERTICAL);
    wxSizer * const sizerMoveCols = new wxBoxSizer(wxHORIZONTAL);
    const wxSizerFlags
        flagsHorz(wxSizerFlags().Border(wxLEFT | wxRIGHT).Centre());
    sizerMoveCols->Add(new wxStaticText(panel, wxID_ANY, "&Move column"),
                       flagsHorz);
    m_txtColIndex = new ColIndexEntry(panel);
    sizerMoveCols->Add(m_txtColIndex, flagsHorz);
    sizerMoveCols->Add(new wxStaticText(panel, wxID_ANY, "&to"), flagsHorz);
    m_txtColPos = new ColIndexEntry(panel);
    sizerMoveCols->Add(m_txtColPos, flagsHorz);
    sizerMoveCols->Add(new wxButton(panel, wxID_APPLY), flagsHorz);

    sizerColumns->Add(sizerMoveCols, wxSizerFlags().Expand().Border(wxBOTTOM));

    wxSizer * const sizerShowCols = new wxBoxSizer(wxHORIZONTAL);
    sizerShowCols->Add(new wxStaticText(panel, wxID_ANY, "Current order:"),
                       flagsHorz);
    m_statOrder = new wxStaticText(panel, wxID_ANY, "<<< default >>>");
    sizerShowCols->Add(m_statOrder, flagsHorz);
    sizerShowCols->Add(new wxButton(panel, wxID_RESET, "&Reset order"));
    sizerColumns->Add(sizerShowCols, wxSizerFlags().Expand().Border(wxTOP));

    wxSizer * const sizerShowHide = new wxBoxSizer(wxHORIZONTAL);
    sizerShowHide->Add(new wxStaticText(panel, wxID_ANY, "Show/hide column:"),
                       flagsHorz);
    m_txtColShowHide = new ColIndexEntry(panel);
    sizerShowHide->Add(m_txtColShowHide, flagsHorz);
    sizerShowHide->Add(new wxButton(panel, wxID_ADD, "&Show"), flagsHorz);
    sizerShowHide->Add(new wxButton(panel, wxID_DELETE, "&Hide"), flagsHorz);
    sizerColumns->Add(sizerShowHide, wxSizerFlags().Expand().Border(wxTOP));

    sizerControls->Add(sizerColumns, wxSizerFlags(1).Expand().Border());

    sizerTop->Add(sizerControls, wxSizerFlags().Expand().Border());

    panel->SetSizer(sizerTop);

    SetClientSize(panel->GetBestSize());
    SetSizeHints(GetSize());

    Show();
}

void GridFrame::OnTabularTable(wxCommandEvent&)
{
    new TabularGridFrame;
}
