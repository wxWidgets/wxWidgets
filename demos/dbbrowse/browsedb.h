//----------------------------------------------------------------------------------------
// Name:        browsedb.h
// Purpose:     a wxDB class
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// Created:     19991127.mj10777
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
//-- Zu tun in browsedb.h
//----------------------------------------------------------------------------------------
//-- 1)
//----------------------------------------------------------------------------------------
// Global structure for holding ODBC connection information
//----------------------------------------------------------------------------------------
extern wxDbConnectInf DbConnectInf;
class MainDoc;
//----------------------------------------------------------------------------------------
class BrowserDB
{
 public:
 //---------------------------------------------------------------------------------------
 // Pointer to the main database connection used in the program.  This
 // pointer would normally be used for doing things as database lookups
 // for user login names and passwords, getting workstation settings, etc.
 // ---> IMPORTANT <---
 //
 //  For each database object created which uses this wxDB pointer
 //    connection to the database, when a CommitTrans() or RollBackTrans()
 //    will commit or rollback EVERY object which uses this wxDB pointer.
 //
 //    To allow each table object (those derived from wxTable) to be
 //    individually committed or rolled back, you MUST use a different
 //    instance of wxDB in the constructor of the table.  Doing so creates
 //  more overhead, and will use more database connections (some DBs have
 //    connection limits...), so use connections sparringly.
 //
 //  It is recommended that one "main" database connection be created for
 //  the entire program to use for READ-ONLY database accesses, but for each
 //  table object which will do a CommitTrans() or RollbackTrans() that a
 // new wxDB object be created and used for it.
 //---------------------------------------------------------------------------------------
  wxDB*          db_BrowserDB;
  wxDbInf*       ct_BrowserDB;
  wxColInf*      cl_BrowserDB;
  wxString       ODBCSource, UserName, Password;
  MainDoc *pDoc;
 //---------------------------------------------------------------------------------------
  wxString       Temp0, Temp1, Temp2, Temp3, Temp4, Temp5;
  wxString       ODBCText;
  wxTextCtrl*    p_LogWindow;
  wxString       s_SqlTyp[25];
  int            i_SqlTyp[25];
  wxString       s_dbTyp[5];
  int            i_dbTyp[5];
  int            i_Which;
  int            i_Records;
  int            i_Cols;
 //---------------------------------------------------------------------------------------
  BrowserDB();
  ~BrowserDB();
  void Zeiger_auf_NULL(int Art);
  bool Initialize(int Quite);
 //---------------------------------------------------------------------------------------
  bool           OnStartDB(int Quite);
  bool           OnCloseDB(int Quite);
  bool           OnSelect(wxString tb_Name,int Quite);
  bool           OnExecSql(wxString SQLStmt,int Quite);
  bool           OnGetNext(int Cols,int Quite);
  wxDbInf*       OnGetCatalog(int Quite);
  wxColInf*      OnGetColumns(char *tableName, int numCols,int Quite);
  void           OnFillSqlTyp();
  void           OnFilldbTyp();
 //---------------------------------------------------------------------------------------
};  // BrowserDB class definition
//----------------------------------------------------------------------------------------
