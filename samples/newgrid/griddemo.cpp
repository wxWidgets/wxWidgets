/////////////////////////////////////////////////////////////////////////////
// Name:        griddemo.cpp
// Purpose:     Grid control wxWindows sample
// Author:      Michael Bedward
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Michael Bedward, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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

IMPLEMENT_APP( GridApp )



bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show( TRUE );
    
    return TRUE;
}



BEGIN_EVENT_TABLE( GridFrame, wxFrame )
    EVT_MENU( ID_TOGGLEROWLABELS,  GridFrame::ToggleRowLabels )
    EVT_MENU( ID_TOGGLECOLLABELS,  GridFrame::ToggleColLabels )    
    EVT_MENU( ID_TOGGLEEDIT, GridFrame::ToggleEditing )
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
    
    EVT_MENU( ID_ABOUT, GridFrame::About )
    EVT_MENU( wxID_EXIT, GridFrame::OnQuit )

    EVT_GRID_LABEL_LEFT_CLICK( GridFrame::OnLabelLeftClick )
    EVT_GRID_CELL_LEFT_CLICK( GridFrame::OnCellLeftClick )
    EVT_GRID_ROW_SIZE( GridFrame::OnRowSize )
    EVT_GRID_COL_SIZE( GridFrame::OnColSize )
    EVT_GRID_SELECT_CELL( GridFrame::OnSelectCell )
    EVT_GRID_RANGE_SELECT( GridFrame::OnRangeSelected )
    EVT_GRID_CELL_CHANGE( GridFrame::OnCellValueChanged )
    
END_EVENT_TABLE()

    
GridFrame::GridFrame()
        : wxFrame( (wxFrame *)NULL, -1, "wxWindows grid class demo",
                   wxDefaultPosition,
                   wxDefaultSize )
{
    int gridW = 600, gridH = 300;
    int logW = gridW, logH = 80;
    
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append( wxID_EXIT, "E&xit" );
    
    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append( ID_TOGGLEROWLABELS,  "&Row labels", "", TRUE );
    viewMenu->Append( ID_TOGGLECOLLABELS,  "&Col labels", "", TRUE );
    viewMenu->Append( ID_TOGGLEEDIT,  "&Editable", "", TRUE );
    viewMenu->Append( ID_SETLABELCOLOUR, "Set &label colour" );
    viewMenu->Append( ID_SETLABELTEXTCOLOUR, "Set label &text colour" );
    
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

    viewMenu->Append( ID_GRIDLINECOLOUR, "&Grid line colour" );

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
    logger->SetActiveTarget( logger );
    logger->SetTimestamp( NULL );

    // this will create a grid and, by default, an associated grid
    // table for string data
    //
    grid->CreateGrid( 100, 100 );

    grid->SetRowSize( 0, 60 );
    grid->SetCellValue( 0, 0, "Ctrl+Home\nwill go to\nthis cell" );
    
    grid->SetCellValue( 0, 1, "Blah" );
    grid->SetCellValue( 0, 2, "Blah" );

    grid->SetCellValue( 0, 5, "Press\nCtrl+arrow\nto skip over\ncells" );

    grid->SetRowSize( 99, 60 );
    grid->SetCellValue( 99, 99, "Ctrl+End\nwill go to\nthis cell" );

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
}


void GridFrame::SetDefaults()
{
    GetMenuBar()->Check( ID_TOGGLEROWLABELS, TRUE );
    GetMenuBar()->Check( ID_TOGGLECOLLABELS, TRUE );
    GetMenuBar()->Check( ID_TOGGLEEDIT, TRUE );
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
    grid->InsertRows( 0, 1 );
}


void GridFrame::InsertCol( wxCommandEvent& WXUNUSED(ev) )
{
    grid->InsertCols( 0, 1 );
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

