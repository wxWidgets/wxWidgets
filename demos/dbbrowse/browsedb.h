//----------------------------------------------------------------------------------------
// Name:        browsedb.h
// Purpose:     a wxDb class
// Author:      Mark Johnson
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
//extern wxDbConnectInf DbConnectInf;
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
    //  For each database object created which uses this wxDb pointer
    //    connection to the database, when a CommitTrans() or RollBackTrans()
    //    will commit or rollback EVERY object which uses this wxDb pointer.
    //
    //    To allow each table object (those derived from wxDbTable) to be
    //    individually committed or rolled back, you MUST use a different
    //    instance of wxDb in the constructor of the table.  Doing so creates
    //  more overhead, and will use more database connections (some DBs have
    //    connection limits...), so use connections sparringly.
    //
    //  It is recommended that one "main" database connection be created for
    //  the entire program to use for READ-ONLY database accesses, but for each
    //  table object which will do a CommitTrans() or RollbackTrans() that a
    // new wxDb object be created and used for it.
    //---------------------------------------------------------------------------------------
    wxDb*          db_BrowserDB;
    wxDbInf*       ct_BrowserDB;
    wxDbColInf*    cl_BrowserDB;
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
    void PointerToNULL(int Art);
    bool Initialize(int Quiet);
    //---------------------------------------------------------------------------------------
    bool           OnStartDB(int Quiet);
    bool           OnCloseDB(int Quiet);
    bool           OnSelect(wxString tb_Name,int Quiet);
    bool           OnExecSql(wxString SQLStmt,int Quiet);
    bool           OnGetNext(int Cols,int Quiet);
    wxDbInf*       OnGetCatalog(int Quiet);
    wxDbColInf*    OnGetColumns(wxChar *tableName, UWORD numCols,int Quiet);
    void           OnFillSqlTyp();
    void           OnFilldbTyp();
    //---------------------------------------------------------------------------------------
};  // BrowserDB class definition
    //----------------------------------------------------------------------------------------
