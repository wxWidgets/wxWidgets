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
//-- all #ifdefs that the whole Project needs. -------------------------------------------
//----------------------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif
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
 EVT_GRID_LABEL_LEFT_CLICK( DBGrid::OnLabelLeftClick )
 EVT_GRID_LABEL_RIGHT_CLICK( DBGrid::OnLabelRightClick )
 EVT_GRID_LABEL_LEFT_DCLICK( DBGrid::OnLabelLeftDClick )
 EVT_GRID_LABEL_RIGHT_DCLICK( DBGrid::OnLabelRightDClick )
 EVT_GRID_CELL_LEFT_CLICK( DBGrid::OnCellLeftClick )
 EVT_GRID_CELL_RIGHT_CLICK( DBGrid::OnCellRightClick )
 EVT_GRID_CELL_LEFT_DCLICK( DBGrid::OnCellLeftDClick )
 EVT_GRID_CELL_RIGHT_DCLICK( DBGrid::OnCellRightDClick )
 EVT_GRID_ROW_SIZE( DBGrid::OnRowSize )
 // EVT_GRID_COL_SIZE( DBGrid::OnColSize )
 EVT_GRID_RANGE_SELECT( DBGrid::OnRangeSelected )
 EVT_GRID_CELL_CHANGE( DBGrid::OnCellChange )
 EVT_MENU(GRID_EDIT,DBGrid::OnModusEdit)
 EVT_MENU(GRID_BROWSE,DBGrid::OnModusBrowse)
END_EVENT_TABLE()
//----------------------------------------------------------------------------------------
//           wxListCtrl(parent, id, pos, size, style)
//           wxGrid(parent,-1,wxPoint( 0, 0 ), wxSize( 400, 300 ) );
//----------------------------------------------------------------------------------------
// DBGrid
//----------------------------------------------------------------------------------------
// DBGrid::DBGrid(wxWindow *parent, const wxWindowID id,const wxPoint& pos,const wxSize& size):
//          wxGrid(parent, id, pos, size)
DBGrid::DBGrid(wxWindow *parent, const wxWindowID id,const wxPoint& pos,const wxSize& size, long style):
  wxGrid(parent, id, pos, size, style)
{
 b_EditModus = TRUE;
 //---------------------------------------------------------------------------------------
 popupMenu1 = new wxMenu("");
 popupMenu1->Append(GRID_EDIT, _("Edit Modus"));
 popupMenu2 = new wxMenu("");
 popupMenu2->Append(GRID_BROWSE, _("Browse Modus"));
}
//----------------------------------------------------------------------------------------
DBGrid::~DBGrid()
{
}
//----------------------------------------------------------------------------------------
int  DBGrid::OnTableView(wxString Table)
{
 //---------------------------------------------------------------------------------------
 int  i=0,x,y,z, ValidTable=0;
 wxString Temp0;
 wxBeginBusyCursor();
 SetFont(* pDoc->ft_Doc);
 // wxFont *ft_Temp;
 // ft_Temp = new wxFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
 // SetFont(*ft_Temp);
 //---------------------------------------------------------------------------------------
 ct_BrowserDB = (db_Br+i_Which)->ct_BrowserDB;                       // Get the DSN Pointer
 //---------------------------------------------------------------------------------------
 if (ct_BrowserDB)                                                   // Valid pointer (!= NULL) ?
 {      // Pointer is Valid, use the wxDatabase Information
  for (x=0;x<ct_BrowserDB->numTables;x++)                            // go through the Tables
  {
   if (!wxStrcmp((ct_BrowserDB->pTableInf+x)->tableName,Table))      // is this our Table ?
   {    // Yes, the Data of this Table shall be put into the Grid
    ValidTable = x;                                                  // Save the Tablenumber
    (db_Br+i_Which)->OnSelect(Table,FALSE);                          // Select * from "table"
     // Set the local Pointer to the Column Information we are going to use
    (db_Br+i_Which)->cl_BrowserDB = (ct_BrowserDB->pTableInf+x)->pColInf;
    if ((ct_BrowserDB->pTableInf+x)->pColInf)                        // Valid pointer (!= NULL) ?
    {   // Pointer is Valid, Column Informationen sind Vorhanden
     i = (db_Br+i_Which)->i_Records;                                 // How many Records are there
     (db_Br+i_Which)->i_Which = ValidTable;                          // Still used ???? mj10777
     if (i == 0)     // If the Table is empty, then show one empty row
      i++;
     // wxLogMessage(_("\n-I-> DBGrid::OnTableView():: Vor CreateGrid"));
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
      Temp0.Printf("%06d",z+1);  SetRowLabelValue(z,Temp0);          // Set Row Lable Value
      (db_Br+i_Which)->OnGetNext((ct_BrowserDB->pTableInf+ValidTable)->numCols,FALSE);
      for (y=0;y<(ct_BrowserDB->pTableInf+ValidTable)->numCols;y++) // Loop through the Fields
      { // BrowserDB::OnGetNext Formats the field Value into tablename
       SetCellValue(z, y,((db_Br+i_Which)->cl_BrowserDB+y)->tableName);
      }
      if (z % 50 == 0)
      {
       Temp0.Printf(_("-I-> DBGrid::OnTableView(%s) - Record %6d has been read."),Table.c_str(),z);
       pDoc->p_MainFrame->SetStatusText(Temp0, 0);
      }
     }  // for (z=0;z<(db_Br+i_Which)->i_Records;z++)
     Temp0.Printf(_("-I-> DBGrid::OnTableView(%s) - %6d Records have been read."),Table.c_str(),z);
     pDoc->p_MainFrame->SetStatusText(Temp0, 0);
     // The Grid has been filled, now leave
     goto Weiter;
    }   // if ((ct_BrowserDB->pTableInf+x)->pColInf)
    else
     wxLogMessage(_("\n-E-> DBGrid::OnTableView():: Invalid Column Pointer : Failed"));
   }    // if ((ct_BrowserDB->pTableInf+x)->tableType == "TABLE")
  }     // for (x=0;x<ct_BrowserDB->numTables;x++)
 }      // if (ct_BrowserDB)
 else
  wxLogMessage(_("\n-E-> DBGrid::OnTableView():: Invalid DSN Pointer : Failed"));
 //---------------------------------------------------------------------------------------
 Weiter:
 SetEditInPlace(b_EditModus);   // Activate in-place Editing (FALSE)
 wxEndBusyCursor();
 //---------------------------------------------------------------------------------------
 wxLogMessage(_("-I-> DBGrid::OnTableView() - End"));
 return 0;
}
//----------------------------------------------------------------------------------------
void DBGrid::OnModusEdit(wxMenu& menu, wxCommandEvent& event)
{
 b_EditModus = TRUE;             // Needed by PopupMenu
 SetEditable(FALSE);             // Do not Edit with Text Edit Control
 SetEditInPlace(b_EditModus);    // Deactivate in-place Editing
 UpdateDimensions();             // Redraw the Grid
}
//----------------------------------------------------------------------------------------
void DBGrid::OnModusBrowse(wxMenu& menu, wxCommandEvent& event)
{
 b_EditModus = FALSE;            // Needed by PopupMenu
 SetEditInPlace(b_EditModus);    // Deactivate in-place Editing
 UpdateDimensions();             // Redraw the Grid
}
//----------------------------------------------------------------------------------------
void DBGrid::OnMouseMove(wxMouseEvent &event)
{
 MousePos = event.GetPosition();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnLabelLeftClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnLabelLeftClick : ";
 if ( ev.GetRow() != -1 )
 {
  logBuf << "row label " << ev.GetRow();
 }
 else if ( ev.GetCol() != -1 )
 {
  logBuf << "col label " << ev.GetCol();
 }
 else
 {
  logBuf << "corner label";
 }
 if ( ev.ShiftDown() )
  logBuf << " (shift down)";
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnLabelRightClick( wxGridEvent& ev )
{
 //-------------------
 if (b_EditModus)
  PopupMenu(popupMenu2,MousePos.x,MousePos.y);
 else
  PopupMenu(popupMenu1,MousePos.x,MousePos.y);
 //-------------------
 logBuf = "DBGrid::OnLabelRightClick : ";
 if ( ev.GetRow() != -1 )
 {
  logBuf << "row label " << ev.GetRow();
 }
 else if ( ev.GetCol() != -1 )
 {
  logBuf << "col label " << ev.GetCol();
 }
 else
 {
  logBuf << "corner label";
 }
 if ( ev.ShiftDown() )
  logBuf << " (shift down)";
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnLabelLeftDClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnLabelLeftDClick : ";
 if ( ev.GetRow() != -1 )
 {
  logBuf << "row label " << ev.GetRow();
 }
 else if ( ev.GetCol() != -1 )
 {
  logBuf << "col label " << ev.GetCol();
 }
 else
 {
  logBuf << "corner label";
 }
 if ( ev.ShiftDown() )
  logBuf << " (shift down)";
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnLabelRightDClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnLabelRightDClick : ";
 if ( ev.GetRow() != -1 )
 {
  logBuf << "row label " << ev.GetRow();
 }
 else if ( ev.GetCol() != -1 )
 {
  logBuf << "col label " << ev.GetCol();
 }
 else
 {
  logBuf << "corner label";
 }
 if ( ev.ShiftDown() )
  logBuf << " (shift down)";
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnCellLeftClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnCellLeftClick : ";
 logBuf << "Cell at row " << ev.GetRow()
 << " col " << ev.GetCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 // wxMessageBox(logBuf);
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 // you must call event skip if you want default grid processing
 // (cell highlighting etc.)
 //
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnCellRightClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnCellRightClick : ";
 logBuf << "Cell at row " << ev.GetRow()
 << " col " << ev.GetCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 // wxMessageBox(logBuf);
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 // you must call event skip if you want default grid processing
 // (cell highlighting etc.)
 //
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnCellLeftDClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnCellLeftDClick : ";
 logBuf << "Cell at row " << ev.GetRow()
 << " col " << ev.GetCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 // wxMessageBox(logBuf);
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 // you must call event skip if you want default grid processing
 // (cell highlighting etc.)
 //
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnCellRightDClick( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnCellRightDClick : ";
 logBuf << "Cell at row " << ev.GetRow()
 << " col " << ev.GetCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 // wxMessageBox(logBuf);
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 // you must call event skip if you want default grid processing
 // (cell highlighting etc.)
 //
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnCellChange( wxGridEvent& ev )
{
 logBuf = "DBGrid::OnCellChange : ";
 logBuf << "Cell at row " << ev.GetRow()
 << " col " << ev.GetCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 // wxMessageBox(logBuf);
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 // you must call event skip if you want default grid processing
 // (cell highlighting etc.)
 //
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnRowSize( wxGridSizeEvent& ev )
{
 logBuf = "DBGrid::OnRowSize : ";
 logBuf << "Resized row " << ev.GetRowOrCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnColSize( wxGridSizeEvent& ev )
{
 logBuf = "DBGrid::OnColSize : ";
 logBuf << "Resized col " << ev.GetRowOrCol();
 // wxLogMessage( "%s", logBuf.c_str() );
 logBuf += "\n";
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
void DBGrid::OnRangeSelected( wxGridRangeSelectEvent& ev )
{
 logBuf = "DBGrid::OnRangeSelected : ";
 logBuf  << "Selected cells from row " << ev.GetTopRow()
  << " col " << ev.GetLeftCol()
  << " to row " << ev.GetBottomRow()
  << " col " << ev.GetRightCol();
 logBuf += "\n";
 // wxLogMessage( "%s", logBuf.c_str() );
 wxLogMessage(logBuf.c_str());
 ev.Skip();
}
//----------------------------------------------------------------------------------------
