//----------------------------------------------------------------------------------------
// Name:        DBGrid.h
// Purpose:     shows seleted Table in a Grid
// Author:      Mark Johnson
// Modified by: 20000126.mj10777
// Created:
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
#if !defined(wxUSE_NEW_GRID) || !(wxUSE_NEW_GRID)
 #error "DBGrid requires the new wxGrid class"
#endif
//----------------------------------------------------------------------------------------
class MainDoc;
//----------------------------------------------------------------------------------------
class DBGrid: public wxGrid
{
 public:
  DBGrid(wxWindow *parent, const wxWindowID id,const wxPoint& pos,const wxSize& size, long style);
  virtual ~DBGrid();
 //---------------------------------------------------------------------------------------
  int        i_TabArt;   // Tab = 0 ; Page = 1;
  int        i_ViewNr;   // View Nummer in Tab / Page
  int        i_Which;    // Which View, Database is this/using
 //---------------------------------------------------------------------------------------
  MainDoc*   pDoc;
  wxDbInf*   ct_BrowserDB;
  BrowserDB* db_Br;
  wxMenu   *popupMenu1; // OnDBClass
  wxMenu   *popupMenu2; // OnDBGrid & OnTableclass
  bool     b_EditModus;
 //---------------------------------------------------------------------------------------
  wxString logBuf,Temp0;
  wxPoint  MousePos;
 //---------------------------------------------------------------------------------------
 void OnModusEdit(wxCommandEvent& event);
 void OnModusBrowse(wxCommandEvent& event);
 void OnMouseMove(wxMouseEvent& event);
 int  OnTableView(wxString Table);
 //-----
 void OnCellChange( wxGridEvent& );
 void OnCellLeftClick( wxGridEvent& );
 void OnCellLeftDClick( wxGridEvent& );
 void OnCellRightClick( wxGridEvent& );
 void OnCellRightDClick( wxGridEvent& );
 void OnColSize( wxGridSizeEvent& );
 void OnEditorShown( wxGridEvent& );
 void OnEditorHidden( wxGridEvent& );
 void OnLabelLeftClick( wxGridEvent& );
 void OnLabelLeftDClick( wxGridEvent& );
 void OnLabelRightClick( wxGridEvent& );
 void OnLabelRightDClick( wxGridEvent& );
 void OnRangeSelected( wxGridRangeSelectEvent& );
 void OnRowSize( wxGridSizeEvent& );
 void OnSelectCell( wxGridEvent& );
 //---------------------------------------------------------------------------------------
 DECLARE_EVENT_TABLE()
};
//----------------------------------------------------------------------------------------
#define GRID_01_BEGIN      1200
#define GRID_BROWSE        1201
#define GRID_EDIT          1202
#define GRID_01_END        1203
//----------------------------------------------------------------------------------------
