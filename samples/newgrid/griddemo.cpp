/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.cpp
// Purpose:     Grid control wxWindows sample
// Author:      Michael Bedward
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/colordlg.h"

#include "wx/grid.h"

#include "griddemo.h"

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
    frame->Show( TRUE );

    return TRUE;
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
    EVT_MENU( ID_TOGGLEGRIDSIZING, GridFrame::ToggleGridSizing )
    EVT_MENU( ID_SETLABELCOLOUR, GridFrame::SetLabelColour )
    EVT_MENU( ID_SETLABELTEXTCOLOUR, GridFrame::SetLabelTextColour )
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

    EVT_MENU( ID_SET_CELL_FG_COLOUR, GridFrame::SetCellFgColour )
    EVT_MENU( ID_SET_CELL_BG_COLOUR, GridFrame::SetCellBgColour )

    EVT_MENU( ID_ABOUT, GridFrame::About )
    EVT_MENU( wxID_EXIT, GridFrame::OnQuit )
    EVT_MENU( ID_VTABLE, GridFrame::OnVTable)
    EVT_MENU( ID_BUGS_TABLE, GridFrame::OnBugsTable)

    EVT_GRID_LABEL_LEFT_CLICK( GridFrame::OnLabelLeftClick )
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_ROW_SIZE( GridFrame::OnRowSize )
    EVT_GRID_COL_SIZE( GridFrame::OnColSize )
    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECT( GridFrame::OnRangeSelected )
    EVT_GRID_CELL_CHANGE( GridFrame::OnCellValueChanged )

    EVT_GRID_EDITOR_SHOWN( GridFrame::OnEditorShown )
    EVT_GRID_EDITOR_HIDDEN( GridFrame::OnEditorHidden )
END_EVENT_TABLE()


GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, -1, "wxWindows grid class demo",
                   wxDefaultPosition,
                   wxDefaultSize )
{
    int gridW = 600, gridH = 300;
    int logW = gridW, logH = 100;

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append( ID_VTABLE, "&Virtual table test\tCtrl-V");
    fileMenu->Append( ID_BUGS_TABLE, "&Bugs table test\tCtrl-B");
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT, "E&xit\tAlt-X" );

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append( ID_TOGGLEROWLABELS,  "&Row labels", "", TRUE );
    viewMenu->Append( ID_TOGGLECOLLABELS,  "&Col labels", "", TRUE );
    viewMenu->Append( ID_TOGGLEEDIT,  "&Editable", "", TRUE );
    viewMenu->Append( ID_TOGGLEROWSIZING, "Ro&w drag-resize", "", TRUE );
    viewMenu->Append( ID_TOGGLECOLSIZING, "C&ol drag-resize", "", TRUE );
    viewMenu->Append( ID_TOGGLEGRIDSIZING, "&Grid drag-resize", "", TRUE );

    wxMenu *rowLabelMenu = new wxMenu;

    viewMenu->Append( ID_ROWLABELALIGN, "R&ow label alignment",
                      rowLabelMenu,
                      "Change alignment of row labels" );

    rowLabelMenu->Append( ID_ROWLABELHORIZALIGN, "&Horizontal" );
    rowLabelMenu->Append( ID_ROWLABELVERTALIGN, "&Vertical" );

    wxMenu *colLabelMenu = new wxMenu;

    viewMenu->Append( ID_COLLABELALIGN, "Col l&abel alignment",
                      colLabelMenu,
                      "Change alignment of col labels" );

    colLabelMenu->Append( ID_COLLABELHORIZALIGN, "&Horizontal" );
    colLabelMenu->Append( ID_COLLABELVERTALIGN, "&Vertical" );

    wxMenu *colMenu = new wxMenu;
    colMenu->Append( ID_SETLABELCOLOUR, "Set &label colour" );
    colMenu->Append( ID_SETLABELTEXTCOLOUR, "Set label &text colour" );
    colMenu->Append( ID_GRIDLINECOLOUR, "&Grid line colour" );
    colMenu->Append( ID_SET_CELL_FG_COLOUR, "Set cell &foreground colour" );
    colMenu->Append( ID_SET_CELL_BG_COLOUR, "Set cell &background colour" );

    wxMenu *editMenu = new wxMenu;
    editMenu->Append( ID_INSERTROW, "Insert &row" );
    editMenu->Append( ID_INSERTCOL, "Insert &column" );
    editMenu->Append( ID_DELETEROW, "Delete selected ro&ws" );
    editMenu->Append( ID_DELETECOL, "Delete selected co&ls" );
    editMenu->Append( ID_CLEARGRID, "Cl&ear grid cell contents" );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append( ID_ABOUT, "&About wxGrid demo" );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( fileMenu, "&File" );
    menuBar->Append( viewMenu, "&View" );
    menuBar->Append( colMenu,  "&Colours" );
    menuBar->Append( editMenu, "&Edit" );
    menuBar->Append( helpMenu, "&Help" );

    SetMenuBar( menuBar );

    grid = new wxGrid( this,
                       -1,
                       wxPoint( 0, 0 ),
                       wxSize( 400, 300 ) );

    logWin = new wxTextCtrl( this,
                             -1,
                             wxEmptyString,
                             wxPoint( 0, gridH + 20 ),
                             wxSize( logW, logH ),
                             wxTE_MULTILINE );

    logger = new wxLogTextCtrl( logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->SetTimestamp( NULL );

    // this will create a grid and, by default, an associated grid
    // table for strings
    grid->CreateGrid( 100, 100 );

    grid->SetRowSize( 0, 60 );
    grid->SetCellValue( 0, 0, "Ctrl+Home\nwill go to\nthis cell" );

    grid->SetCellValue( 0, 1, "Blah" );
    grid->SetCellValue( 0, 2, "Blah" );
    grid->SetCellValue( 0, 3, "Read only" );
    grid->SetReadOnly( 0, 3 );

    grid->SetCellValue( 0, 5, "Press\nCtrl+arrow\nto skip over\ncells" );

    grid->SetRowSize( 99, 60 );
    grid->SetCellValue( 99, 99, "Ctrl+End\nwill go to\nthis cell" );

    grid->SetCellValue(2, 2, "red");

    grid->SetCellTextColour(2, 2, *wxRED);
    grid->SetCellValue(3, 3, "green on grey");
    grid->SetCellTextColour(3, 3, *wxGREEN);
    grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);

    grid->SetCellValue(4, 4, "a weird looking cell");
    grid->SetCellAlignment(4, 4, wxCENTRE, wxCENTRE);
    grid->SetCellRenderer(4, 4, new MyGridCellRenderer);

    grid->SetCellValue(3, 0, "1");
    grid->SetCellRenderer(3, 0, new wxGridCellBoolRenderer);
    grid->SetCellEditor(3, 0, new wxGridCellBoolEditor);

    wxGridCellAttr *attr;
    attr = new wxGridCellAttr;
    attr->SetTextColour(*wxBLUE);
    grid->SetColAttr(5, attr);
    attr = new wxGridCellAttr;
    attr->SetBackgroundColour(*wxBLUE);
    grid->SetRowAttr(5, attr);

    grid->SetCellValue(2, 4, "a wider column");
    grid->SetColSize(4, 120);
    grid->SetColMinimalWidth(4, 120);

    grid->SetColFormatFloat(5);
    grid->SetCellValue(0, 5, "3.1415");
    grid->SetCellValue(1, 5, "1415");
    grid->SetCellValue(2, 5, "12345.67890");

    grid->SetColFormatFloat(6, 6, 2);
    grid->SetCellValue(0, 6, "3.1415");
    grid->SetCellValue(1, 6, "1415");
    grid->SetCellValue(2, 6, "12345.67890");

    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topSizer->Add( grid,
                   1,
                   wxEXPAND );

    topSizer->Add( logWin,
                   0,
                   wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( topSizer );

    topSizer->Fit( this );
    topSizer->SetSizeHints( this );

    Centre();
    SetDefaults();
}


GridFrame::~GridFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}


void GridFrame::SetDefaults()
{
    GetMenuBar()->Check( ID_TOGGLEROWLABELS, TRUE );
    GetMenuBar()->Check( ID_TOGGLECOLLABELS, TRUE );
    GetMenuBar()->Check( ID_TOGGLEEDIT, TRUE );
    GetMenuBar()->Check( ID_TOGGLEROWSIZING, TRUE );
    GetMenuBar()->Check( ID_TOGGLECOLSIZING, TRUE );
    GetMenuBar()->Check( ID_TOGGLEGRIDSIZING, TRUE );
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

void GridFrame::ToggleGridSizing( wxCommandEvent& WXUNUSED(ev) )
{
    grid->EnableDragGridSize(
        GetMenuBar()->IsChecked( ID_TOGGLEGRIDSIZING ) );
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


void GridFrame::SetRowLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxLEFT:
            horiz = wxCENTRE;
            break;

        case wxCENTRE:
            horiz = wxRIGHT;
            break;

        case wxRIGHT:
            horiz = wxLEFT;
            break;
    }

    grid->SetRowLabelAlignment( horiz, -1 );
}

void GridFrame::SetRowLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetRowLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxTOP:
            vert = wxCENTRE;
            break;

        case wxCENTRE:
            vert = wxBOTTOM;
            break;

        case wxBOTTOM:
            vert = wxTOP;
            break;
    }

    grid->SetRowLabelAlignment( -1, vert );
}


void GridFrame::SetColLabelHorizAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( horiz )
    {
        case wxLEFT:
            horiz = wxCENTRE;
            break;

        case wxCENTRE:
            horiz = wxRIGHT;
            break;

        case wxRIGHT:
            horiz = wxLEFT;
            break;
    }

    grid->SetColLabelAlignment( horiz, -1 );
}


void GridFrame::SetColLabelVertAlignment( wxCommandEvent& WXUNUSED(ev) )
{
    int horiz, vert;
    grid->GetColLabelAlignment( &horiz, &vert );

    switch ( vert )
    {
        case wxTOP:
            vert = wxCENTRE;
            break;

        case wxCENTRE:
            vert = wxBOTTOM;
            break;

        case wxBOTTOM:
            vert = wxTOP;
            break;
    }

    grid->SetColLabelAlignment( -1, vert );
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
        int topRow, bottomRow, leftCol, rightCol;
        grid->GetSelection( &topRow, &leftCol, &bottomRow, &rightCol );
        grid->DeleteRows( topRow, bottomRow - topRow + 1 );
    }
}


void GridFrame::DeleteSelectedCols( wxCommandEvent& WXUNUSED(ev) )
{
    if ( grid->IsSelection() )
    {
        int topRow, bottomRow, leftCol, rightCol;
        grid->GetSelection( &topRow, &leftCol, &bottomRow, &rightCol );
        grid->DeleteCols( leftCol, rightCol - leftCol + 1 );
    }
}


void GridFrame::ClearGrid( wxCommandEvent& WXUNUSED(ev) )
{
    grid->ClearGrid();
}

void GridFrame::SetCellFgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.Ok() )
    {
        grid->SetDefaultCellTextColour(col);
        grid->Refresh();
    }
}

void GridFrame::SetCellBgColour( wxCommandEvent& WXUNUSED(ev) )
{
    wxColour col = wxGetColourFromUser(this);
    if ( col.Ok() )
    {
        grid->SetDefaultCellBackgroundColour(col);
        grid->Refresh();
    }
}

void GridFrame::OnLabelLeftClick( wxGridEvent& ev )
{
    logBuf = "";
    if ( ev.GetRow() != -1 )
    {
        logBuf << "Left click on row label " << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << "Left click on col label " << ev.GetCol();
    }
    else
    {
        logBuf << "Left click on corner label";
    }

    if ( ev.ShiftDown() ) logBuf << " (shift down)";
    wxLogMessage( "%s", logBuf.c_str() );

    // you must call event skip if you want default grid processing
    //
    ev.Skip();
}


void GridFrame::OnCellLeftClick( wxGridEvent& ev )
{
    logBuf = "";
    logBuf << "Left click at row " << ev.GetRow()
           << " col " << ev.GetCol();
    wxLogMessage( "%s", logBuf.c_str() );

    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}


void GridFrame::OnRowSize( wxGridSizeEvent& ev )
{
    logBuf = "";
    logBuf << "Resized row " << ev.GetRowOrCol();
    wxLogMessage( "%s", logBuf.c_str() );

    ev.Skip();
}


void GridFrame::OnColSize( wxGridSizeEvent& ev )
{
    logBuf = "";
    logBuf << "Resized col " << ev.GetRowOrCol();
    wxLogMessage( "%s", logBuf.c_str() );

    ev.Skip();
}


void GridFrame::OnSelectCell( wxGridEvent& ev )
{
    logBuf = "";
    logBuf << "Selected cell at row " << ev.GetRow()
           << " col " << ev.GetCol();
    wxLogMessage( "%s", logBuf.c_str() );

    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
}

void GridFrame::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
    logBuf = "";
    logBuf  << "Selected cells from row " << ev.GetTopRow()
            << " col " << ev.GetLeftCol()
            << " to row " << ev.GetBottomRow()
            << " col " << ev.GetRightCol();

    wxLogMessage( "%s", logBuf.c_str() );

    ev.Skip();
}

void GridFrame::OnCellValueChanged( wxGridEvent& ev )
{
    logBuf = "";
    logBuf  << "Value changed for cell at"
            << " row " << ev.GetRow()
            << " col " << ev.GetCol();

    wxLogMessage( "%s", logBuf.c_str() );

    ev.Skip();
}

void GridFrame::OnEditorShown( wxGridEvent& ev )
{
    wxLogMessage( "Cell editor shown." );

    ev.Skip();
}

void GridFrame::OnEditorHidden( wxGridEvent& ev )
{
    wxLogMessage( "Cell editor hidden." );

    ev.Skip();
}

void GridFrame::About(  wxCommandEvent& WXUNUSED(ev) )
{
    (void)wxMessageBox( "\n\nwxGrid demo \n\n"
                        "Michael Bedward \n"
                        "mbedward@ozemail.com.au \n\n",
                        "About",
                        wxOK );
}


void GridFrame::OnQuit( wxCommandEvent& WXUNUSED(ev) )
{
    Close( TRUE );
}

void GridFrame::OnBugsTable(wxCommandEvent& )
{
    BugsGridFrame *frame = new BugsGridFrame;
    frame->Show(TRUE);
}

void GridFrame::OnVTable(wxCommandEvent& )
{
    static long s_sizeGrid = 10000;

#ifdef __WXMOTIF__
    // MB: wxGetNumberFromUser doesn't work properly for wxMotif
    wxString s;
    s << s_sizeGrid;
    s = wxGetTextFromUser( "Size of the table to create",
                           "Size:",
                           s );
    
    s.ToLong( &s_sizeGrid );
    
#else
    s_sizeGrid = wxGetNumberFromUser("Size of the table to create",
                                     "Size: ",
                                     "wxGridDemo question",
                                     s_sizeGrid,
                                     0, 32000, this);
#endif
    
    if ( s_sizeGrid != -1 )
    {
        BigGridFrame* win = new BigGridFrame(s_sizeGrid);
        win->Show(TRUE);
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

wxGridCellAttr *MyGridCellAttrProvider::GetAttr(int row, int col) const
{
    wxGridCellAttr *attr = wxGridCellAttrProvider::GetAttr(row, col);

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

// ============================================================================
// BigGridFrame and BigGridTable:  Sample of a non-standard table
// ============================================================================

BigGridFrame::BigGridFrame(long sizeGrid)
            : wxFrame(NULL, -1, "Plugin Virtual Table",
                      wxDefaultPosition, wxSize(500, 450))
{
    m_grid = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize);
    m_table = new BigGridTable(sizeGrid);

    // VZ: I don't understand why this slows down the display that much,
    //     must profile it...
    //m_table->SetAttrProvider(new MyGridCellAttrProvider);

    m_grid->SetTable(m_table, TRUE);

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

static const wxChar* severities[] =
{
    _T("wishlist"),
    _T("minor"),
    _T("normal"),
    _T("major"),
    _T("critical"),
};

static struct BugsGridData
{
    int id;

    // Borland can't deal with global wxStrings and will generate a
    // compile time error with the initializing the gs_dataGridBugs
    // array (below)
    //
#ifndef __BORLANDC__
    wxString summary;
#else
    wxChar *summary;
#endif

    Severity severity;
    int prio;
    
#ifndef __BORLANDC__
    wxString platform;
#else
    wxChar *platform;
#endif

    bool opened;
} gs_dataBugsGrid [] = 
{
    { 18, _T("foo doesn't work"), Sev_Major, 1, _T("wxMSW"), TRUE },
    { 27, _T("bar crashes"), Sev_Critical, 1, _T("all"), FALSE },
    { 45, _T("printing is slow"), Sev_Minor, 3, _T("wxMSW"), TRUE },
    { 68, _T("Rectangle() fails"), Sev_Normal, 1, _T("wxMSW"), FALSE },
};

static const wxChar *headers[Col_Max] =
{
    _T("Id"),
    _T("Summary"),
    _T("Severity"),
    _T("Priority"),
    _T("Platform"),
    _T("Opened?"),
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
        case Col_Platform:
            return wxGRID_VALUE_STRING;

        case Col_Opened:
            return wxGRID_VALUE_BOOL;
    }

    wxFAIL_MSG(_T("unknown column"));

    return wxEmptyString;
}

long BugsGridTable::GetNumberRows()
{
    return WXSIZEOF(gs_dataBugsGrid);
}

long BugsGridTable::GetNumberCols()
{
    return Col_Max;
}

bool BugsGridTable::IsEmptyCell( int row, int col )
{
    return FALSE;
}

wxString BugsGridTable::GetValue( int row, int col )
{
    const BugsGridData& gd = gs_dataBugsGrid[row];

    switch ( col )
    {
        case Col_Id:
        case Col_Priority:
        case Col_Opened:
            wxFAIL_MSG(_T("unexpected column"));
            break;

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
            wxFAIL_MSG(_T("unexpected column"));
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
                    wxLogWarning(_T("Invalid severity value '%s'."),
                                 value.c_str());
                    gd.severity = Sev_Normal;
                }
            }
            break;

        case Col_Summary:
#ifndef __BORLANDC__
            gd.summary = value;
#else
            // this generates a warning message if you are using the
            // memory tracing code but it should be ok :MB
            //
            delete gd.summary;
            gd.summary = copystring( value.c_str() );
#endif
            break;

        case Col_Platform:
#ifndef __BORLANDC__
            gd.platform = value;
#else            
            // this generates a warning message if you are using the
            // memory tracing code but it should be ok :MB
            //
            delete gd.platform;
            gd.platform = copystring( value.c_str() );
#endif
            break;
    }
}

bool BugsGridTable::CanGetValueAs( int WXUNUSED(row), int col, const wxString& typeName )
{
    if ( typeName == wxGRID_VALUE_STRING )
    {
        return TRUE;
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
        return FALSE;
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
            wxFAIL_MSG(_T("unexpected column"));
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
        wxFAIL_MSG(_T("unexpected column"));

        return FALSE;
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
            wxFAIL_MSG(_T("unexpected column"));
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
        wxFAIL_MSG(_T("unexpected column"));
    }
}

wxString BugsGridTable::GetColLabelValue( int col )
{
    return headers[col];
}

BugsGridTable::BugsGridTable()
{
}

// ----------------------------------------------------------------------------
// BugsGridFrame
// ----------------------------------------------------------------------------

BugsGridFrame::BugsGridFrame()
             : wxFrame(NULL, -1, "Bugs table",
                       wxDefaultPosition, wxSize(500, 300))
{
    wxGrid *grid = new wxGrid(this, -1, wxDefaultPosition);
    wxGridTableBase *table = new BugsGridTable();
    table->SetAttrProvider(new MyGridCellAttrProvider);
    grid->SetTable(table, TRUE);

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

    grid->SetMargins(0, 0);

    grid->Fit();
    wxSize size = grid->GetSize();
    size.x += 10;
    size.y += 10;
    SetClientSize(size);
}
