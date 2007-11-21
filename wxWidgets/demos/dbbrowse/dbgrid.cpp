//----------------------------------------------------------------------------------------
// Name:        DBGrid.cpp
// Purpose:     wxGrid sample
// Author:      Mark Johnson
// Modified by: 19990929.mj10777 a reuseable DBGrid
// Created:     19990929
// Copyright:   (c)
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//----------------------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma hdrstop
#endif
//----------------------------------------------------------------------------------------
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
//----------------------------------------------------------------------------------------
//-- all #includes that every .cpp needs             --- 19990807.mj10777 ----------------
//----------------------------------------------------------------------------------------
#include "std.h"    // sorgsam Pflegen !
//----------------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DBGrid, wxGrid)
    EVT_MOTION (DBGrid::OnMouseMove)
    // DBGrid
    // ------------
    EVT_GRID_CELL_CHANGE( DBGrid::OnCellChange )
    EVT_GRID_CELL_LEFT_CLICK( DBGrid::OnCellLeftClick )
    EVT_GRID_CELL_LEFT_DCLICK( DBGrid::OnCellLeftDClick )
    EVT_GRID_CELL_RIGHT_CLICK( DBGrid::OnCellRightClick )
    EVT_GRID_CELL_RIGHT_DCLICK( DBGrid::OnCellRightDClick )
    // EVT_GRID_COL_SIZE( DBGrid::OnColSize )
    // EVT_GRID_ROW_SIZE( DBGrid::OnRowSize )
    EVT_GRID_EDITOR_SHOWN( DBGrid::OnEditorShown )
    EVT_GRID_EDITOR_HIDDEN( DBGrid::OnEditorHidden )
    EVT_GRID_LABEL_LEFT_CLICK( DBGrid::OnLabelLeftClick )
    EVT_GRID_LABEL_LEFT_DCLICK( DBGrid::OnLabelLeftDClick )
    EVT_GRID_LABEL_RIGHT_CLICK( DBGrid::OnLabelRightClick )
    EVT_GRID_LABEL_RIGHT_DCLICK( DBGrid::OnLabelRightDClick )
    EVT_GRID_RANGE_SELECT( DBGrid::OnRangeSelected )
    EVT_GRID_ROW_SIZE( DBGrid::OnRowSize )
    EVT_GRID_SELECT_CELL( DBGrid::OnSelectCell )
    EVT_MENU(GRID_EDIT,DBGrid::OnModusEdit)
    EVT_MENU(GRID_BROWSE,DBGrid::OnModusBrowse)
END_EVENT_TABLE()

//----------------------------------------------------------------------------------------
//           wxListCtrl(parent, id, pos, size, style)
//           wxGrid(parent,wxID_ANY,wxPoint( 0, 0 ), wxSize( 400, 300 ) );
//----------------------------------------------------------------------------------------
// DBGrid
//----------------------------------------------------------------------------------------
// DBGrid::DBGrid(wxWindow *parent, const wxWindowID id,const wxPoint& pos,const wxSize& size):
//          wxGrid(parent, id, pos, size)
DBGrid::DBGrid(wxWindow *parent, const wxWindowID id,const wxPoint& pos,const wxSize& size, long style):
wxGrid(parent, id, pos, size, style)
{
    b_EditModus = false;
    //---------------------------------------------------------------------------------------
    popupMenu1 = new wxMenu;
    popupMenu1->Append(GRID_EDIT, _("Edit Modus"));
    popupMenu2 = new wxMenu;
    popupMenu2->Append(GRID_BROWSE, _("Browse Modus"));
}

//----------------------------------------------------------------------------------------
DBGrid::~DBGrid()
{
    delete popupMenu1;
    delete popupMenu2;
}

//----------------------------------------------------------------------------------------
int  DBGrid::OnTableView(wxString Table)
{
    wxStopWatch sw;
    //---------------------------------------------------------------------------------------
    int  x,y,z;
    wxString Temp0;
    wxBeginBusyCursor();
    SetDefaultCellFont(* pDoc->ft_Doc);
    //---------------------------------------------------------------------------------------
    ct_BrowserDB = (db_Br+i_Which)->ct_BrowserDB;                       // Get the DSN Pointer
    //---------------------------------------------------------------------------------------
    if (ct_BrowserDB)                                                   // Valid pointer (!= NULL) ?
    {      // Pointer is Valid, use the wxDatabase Information
        for (x=0;x<ct_BrowserDB->numTables;x++)                            // go through the Tables
        {
            if (!wxStrcmp((ct_BrowserDB->pTableInf+x)->tableName,Table))      // is this our Table ?
            {    // Yes, the Data of this Table shall be put into the Grid
                int ValidTable = x;                                              // Save the Tablenumber
                (db_Br+i_Which)->OnSelect(Table,false);                          // Select * from "table"
                // Set the local Pointer to the Column Information we are going to use
                (db_Br+i_Which)->cl_BrowserDB = (ct_BrowserDB->pTableInf+x)->pColInf;
                if ((ct_BrowserDB->pTableInf+x)->pColInf)                        // Valid pointer (!= NULL) ?
                {   // Pointer is Valid, Column Informationen sind Vorhanden
                    int i = (db_Br+i_Which)->i_Records;                             // How many Records are there
                    (db_Br+i_Which)->i_Which = ValidTable;                          // Still used ???? mj10777
                    if (i == 0)     // If the Table is empty, then show one empty row
                        i++;
                    // wxLogMessage(_("\n-I-> DBGrid::OnTableView() : Vor CreateGrid"));
                    CreateGrid(i,(ct_BrowserDB->pTableInf+x)->numCols);             // Records , Columns
                    for (y=0;y<(ct_BrowserDB->pTableInf+x)->numCols;y++)            // Loop through the Fields
                    {  // The Field / Column name is used here as Row Titel
                        SetColLabelValue(y,((ct_BrowserDB->pTableInf+x)->pColInf+y)->colName);
                        SetColSize(y,95);
                    }  // for (y=0;y<(ct_BrowserDB->pTableInf+x)->numCols;y++)
                    SetColSize(((ct_BrowserDB->pTableInf+x)->numCols-1),120);       // Make the last Column Wider
                    // The Grid has been created, now fill it
                    for (z=0;z<(db_Br+i_Which)->i_Records;z++)                      // Loop through the Records
                    {
                        Temp0.Printf(_T("%06d"),z+1);  SetRowLabelValue(z,Temp0);          // Set Row Lable Value
                        (db_Br+i_Which)->OnGetNext((ct_BrowserDB->pTableInf+ValidTable)->numCols,false);
                        for (y=0;y<(ct_BrowserDB->pTableInf+ValidTable)->numCols;y++) // Loop through the Fields
                        { // BrowserDB::OnGetNext Formats the field Value into tablename
                            SetCellValue(z, y,((db_Br+i_Which)->cl_BrowserDB+y)->tableName);
                        }
#if wxUSE_STATUSBAR
                        if (z % 50 == 0)
                        {
                            Temp0.Printf(_("-I-> DBGrid::OnTableView(%s) - Record %6d (from %d) has been read."),Table.c_str(),z,(db_Br+i_Which)->i_Records);
                            pDoc->p_MainFrame->SetStatusText(Temp0, 0);
                        }
#endif // wxUSE_STATUSBAR
                    }  // for (z=0;z<(db_Br+i_Which)->i_Records;z++)
                    Temp0.Printf(_("-I-> DBGrid::OnTableView(%s) - %6d Records have been read. - Time needed : %ld ms"),Table.c_str(),z,sw.Time());
                    wxLogMessage(Temp0);
#if wxUSE_STATUSBAR
                    pDoc->p_MainFrame->SetStatusText(Temp0, 0);
#endif // wxUSE_STATUSBAR
                    // The Grid has been filled, now leave
                    goto Weiter;
                }   // if ((ct_BrowserDB->pTableInf+x)->pColInf)
                else
                    wxLogMessage(_("\n-E-> DBGrid::OnTableView() : Invalid Column Pointer : Failed"));
            }    // if ((ct_BrowserDB->pTableInf+x)->tableType == "TABLE")
        }     // for (x=0;x<ct_BrowserDB->numTables;x++)
    }      // if (ct_BrowserDB)
    else
        wxLogMessage(_("\n-E-> DBGrid::OnTableView() : Invalid DSN Pointer : Failed"));
    //---------------------------------------------------------------------------------------
Weiter:
    EnableEditing(b_EditModus);     // Deactivate in-place Editing
    wxEndBusyCursor();
    //---------------------------------------------------------------------------------------
    wxLogMessage(_("-I-> DBGrid::OnTableView() - End"));
    return 0;
}

//----------------------------------------------------------------------------------------
void DBGrid::OnModusEdit(wxCommandEvent& WXUNUSED(event))
{
    b_EditModus = true;             // Needed by PopupMenu
    EnableEditing(b_EditModus);     // Activate in-place Editing
    UpdateDimensions();             // Redraw the Grid
    // wxLogMessage(_("-I-> DBGrid::OnModusEdit() - End"));
}

//----------------------------------------------------------------------------------------
void DBGrid::OnModusBrowse(wxCommandEvent& WXUNUSED(event))
{
    b_EditModus = false;            // Needed by PopupMenu
    EnableEditing(b_EditModus);     // Deactivate in-place Editing
    UpdateDimensions();             // Redraw the Grid
    // wxLogMessage(_("-I-> DBGrid::OnModusBrowse() - End"));
}

//----------------------------------------------------------------------------------------
void DBGrid::OnEditorShown( wxGridEvent& ev )
{
    // wxLogMessage(_("-I-> DBGrid::OnEditorShown() - End"));
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnEditorHidden( wxGridEvent& ev )
{
    // wxLogMessage(_("-I-> DBGrid::OnEditorHidden() - End"));
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnSelectCell( wxGridEvent& ev )
{
    logBuf = wxEmptyString;
    logBuf << _T("Selected cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    wxLogMessage( _T("%s"), logBuf.c_str() );
    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnMouseMove(wxMouseEvent &event)
{
    MousePos = event.GetPosition();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnLabelLeftClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnLabelLeftClick : ");
    if ( ev.GetRow() != -1 )
    {
        logBuf << _T("row label ") << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << _T("col label ") << ev.GetCol();
    }
    else
    {
        logBuf << _T("corner label");
    }
    if ( ev.ShiftDown() )
        logBuf << _T(" (shift down)");

    // wxLogMessage( "%s", logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnLabelRightClick( wxGridEvent& ev )
{
    //-------------------------------------------------------
    if (b_EditModus)
        PopupMenu(popupMenu2,MousePos.x,MousePos.y);
    else
        PopupMenu(popupMenu1,MousePos.x,MousePos.y);
    //-------------------
    logBuf = _T("DBGrid::OnLabelRightClick : ");
    if ( ev.GetRow() != -1 )
    {
        logBuf << _T("row label ") << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << _T("col label ") << ev.GetCol();
    }
    else
    {
        logBuf << _T("corner label");
    }
    if ( ev.ShiftDown() )
        logBuf << _T(" (shift down)");

    // wxLogMessage( "%s", logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnLabelLeftDClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnLabelLeftDClick : ");
    if ( ev.GetRow() != -1 )
    {
        logBuf << _T("row label ") << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << _T("col label ") << ev.GetCol();
    }
    else
    {
        logBuf << _T("corner label");
    }
    if ( ev.ShiftDown() )
        logBuf << _T(" (shift down)");

    // wxLogMessage( _T("%s"), logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnLabelRightDClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnLabelRightDClick : ");
    if ( ev.GetRow() != -1 )
    {
        logBuf << _T("row label ") << ev.GetRow();
    }
    else if ( ev.GetCol() != -1 )
    {
        logBuf << _T("col label ") << ev.GetCol();
    }
    else
    {
        logBuf << _T("corner label");
    }
    if ( ev.ShiftDown() )
        logBuf << _T(" (shift down)");
    // wxLogMessage( "%s", logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnCellLeftClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnCellLeftClick : ");
    logBuf << _T("Cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    // wxMessageBox(logBuf);
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnCellRightClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnCellRightClick : ");
    logBuf << _T("Cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    // wxMessageBox(logBuf);
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnCellLeftDClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnCellLeftDClick : ");
    logBuf << _T("Cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    // wxMessageBox(logBuf);
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnCellRightDClick( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnCellRightDClick : ");
    logBuf << _T("Cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    // wxMessageBox(logBuf);
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnCellChange( wxGridEvent& ev )
{
    logBuf = _T("DBGrid::OnCellChange : ");
    logBuf << _T("Cell at row ") << ev.GetRow()
        << _T(" col ") << ev.GetCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    // wxMessageBox(logBuf);
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    // you must call event skip if you want default grid processing
    // (cell highlighting etc.)
    //
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnRowSize( wxGridSizeEvent& ev )
{
    logBuf = _T("DBGrid::OnRowSize : ");
    logBuf << _T("Resized row ") << ev.GetRowOrCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnColSize( wxGridSizeEvent& ev )
{
    logBuf = _T("DBGrid::OnColSize : ");
    logBuf << _T("Resized col ") << ev.GetRowOrCol();
    // wxLogMessage( "%s", logBuf.c_str() );
    logBuf += _T("\n");
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}

//----------------------------------------------------------------------------------------
void DBGrid::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
    logBuf = _T("DBGrid::OnRangeSelected : ");
    logBuf  << _T("Selected cells from row ") << ev.GetTopRow()
        << _T(" col ") << ev.GetLeftCol()
        << _T(" to row ") << ev.GetBottomRow()
        << _T(" col ") << ev.GetRightCol();
    logBuf += _T("\n");
    // wxLogMessage( "%s", logBuf.c_str() );
    wxLogMessage(logBuf.c_str());
    ev.Skip();
}
//----------------------------------------------------------------------------------------
