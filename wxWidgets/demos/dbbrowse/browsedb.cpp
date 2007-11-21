//----------------------------------------------------------------------------------------
// Name:        BrowserDB.h,cpp
// Purpose:     a wxDB class
// Author:      Mark Johnson
// Modified by:
// Created:     19991127.mj10777
// Copyright:   (c) Mark Johnson
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
#include "std.h"
//----------------------------------------------------------------------------------------
// Global structure for holding ODBC connection information
wxDbConnectInf DbConnectInf;

#if !wxUSE_ODBC
  #error Demo cannot be compiled unless setup.h has wxUSE_ODBC set to 1
#endif

//----------------------------------------------------------------------------------------
extern WXDLLEXPORT_DATA(wxDbList*) PtrBegDbList;    /* from db.cpp, used in getting back error results from db connections */

//----------------------------------------------------------------------------------------
wxChar *GetExtendedDBErrorMsg(wxChar *ErrFile, int ErrLine)
{
    static wxString msg;
    wxString tStr;
    if (ErrFile || ErrLine)
    {
        msg += _T("File: ");
        msg += ErrFile;
        msg += _T("   Line: ");
        tStr.Printf(_T("%d"),ErrLine);
        msg += tStr.GetData();
        // msg += _T("\n");
    }
    msg.Append (_T("\nODBC errors:\n"));
    // msg += _T("\n");
    /* Scan through each database connection displaying
    * any ODBC errors that have occurred. */
    wxDbList *pDbList;
    for (pDbList = PtrBegDbList; pDbList; pDbList = pDbList->PtrNext)
    {
        // Skip over any free connections
        if (pDbList->Free)
            continue;
        // Display errors for this connection
        for (int i = 0; i < DB_MAX_ERROR_HISTORY; i++)
        {
            if (pDbList->PtrDb->errorList[i])
            {
                msg.Append(pDbList->PtrDb->errorList[i]);
                if (wxStrcmp(pDbList->PtrDb->errorList[i],wxEmptyString) != 0)
                    msg.Append(_T("\n"));
            }
        }
    }
    msg += _T("\n");
    return (wxChar*) (const wxChar*) msg;
}  // GetExtendedDBErrorMsg

//----------------------------------------------------------------------------------------
BrowserDB::BrowserDB()
{
    PointerToNULL(0);
    ODBCSource = wxEmptyString;  // ODBC data source name (created with ODBC Administrator under Win95/NT)
    UserName   = wxEmptyString;  // database username - must already exist in the data source
    Password   = wxEmptyString;  // password database username
    OnFillSqlTyp();
    OnFilldbTyp();
}  // BrowserDB Constructor

//----------------------------------------------------------------------------------------
BrowserDB::~BrowserDB()
{
    PointerToNULL(1);  // Clean up Tables and Databases (Commit, Close and delete)
}  // BrowserDB destructor

//----------------------------------------------------------------------------------------
bool BrowserDB::Initialize(int Quiet)
{
    if (!OnStartDB(Quiet))
    {
        wxLogMessage(_("\n\n-E-> BrowserDB::OnStartDB(%s) : Failed ! "),ODBCSource.c_str());
        return false;
    }
    return true;
}  // BrowserDB:Initialize

//----------------------------------------------------------------------------------------
bool BrowserDB::OnStartDB(int Quiet)
{
    wxStopWatch sw;
    if (!Quiet)
        wxLogMessage(_("\n-I-> BrowserDB::OnStartDB(%s) : Begin "),ODBCSource.c_str());
    if (db_BrowserDB != NULL)
    {
        if (!Quiet)
            wxLogMessage(_("\n-I-> BrowserDB::OnStartDB() : DB is already open."));
        return true;
    }

    DbConnectInf.AllocHenv();

    //---------------------------------------------------------------------------------------
    // Connect to datasource
    //---------------------------------------------------------------------------------------
    DlgUser *p_Dlg;
    p_Dlg = new DlgUser(pDoc->p_MainFrame,pDoc,wxEmptyString);
    p_Dlg->s_DSN      = ODBCSource;
    p_Dlg->s_User     = UserName;
    p_Dlg->s_Password = Password;
    p_Dlg->OnInit();
    p_Dlg->Fit();

    bool OK = false;
    if (p_Dlg->ShowModal() == wxID_OK)
    {
        (pDoc->p_DSN+i_Which)->Usr = p_Dlg->s_User;
        (pDoc->p_DSN+i_Which)->Pas = p_Dlg->s_Password;
        UserName  = p_Dlg->s_User;
        Password  = p_Dlg->s_Password;
        OK = true;
    }
    delete p_Dlg;
    if (OK)
    {
        //--------------------------------------------------------------------------------------
        DbConnectInf.SetDsn(ODBCSource);         // ODBC data source name (created with ODBC Administrator under Win95/NT)
        DbConnectInf.SetUserID(UserName);        // database username - must already exist in the data source
        DbConnectInf.SetPassword(Password);      // password database username
        db_BrowserDB = wxDbGetConnection(&DbConnectInf);
        // wxLogMessage(">>>%s<<<>>>%s<<<",UserName.c_str(),Password.c_str());
        if (db_BrowserDB == NULL)
        {
            DbConnectInf.SetDsn(wxEmptyString);
            DbConnectInf.SetUserID(wxEmptyString);
            DbConnectInf.SetPassword(wxEmptyString);
            if (!Quiet)
            {
                wxLogMessage(_("\n-E-> BrowserDB::OnConnectDataSource() DB CONNECTION ERROR : Unable to connect to the data source.\n\nCheck the name of your data source to verify it has been correctly entered/spelled.\n\nWith some databases, the user name and password must\nbe created with full rights to the table prior to making a connection\n(using tools provided by the database manufacturer)"));
                wxLogMessage(_("-I-> BrowserDB::OnStartDB(%s) : End - Time needed : %ld ms"),ODBCSource.c_str(),sw.Time());
            }
            DbConnectInf.FreeHenv();
            return false;
        }
        //--------------------------------------------------------------------------------------
        if (!Quiet)
        {
            Temp1 = db_BrowserDB->GetDatabaseName();
            Temp2 = db_BrowserDB->GetDataSource();
            wxLogMessage(_("-I-> BrowserDB::OnGetDataSourceODBC() - DatabaseName(%s) ; DataSource(%s)"),Temp1.c_str(),Temp2.c_str());
            wxLogMessage(_("-I-> BrowserDB::OnStartDB(%s) : End - Time needed : %ld ms"),ODBCSource.c_str(),sw.Time());
        }
        return true;
    }
    else
    {
        DbConnectInf.FreeHenv();
        return false;
    }
}

//----------------------------------------------------------------------------------------
bool BrowserDB::OnCloseDB(int Quiet)
{
    if (!Quiet)
        wxLogMessage(_("-I-> BrowserDB::OnCloseDB() : Begin "));
    if (db_BrowserDB)
    {
//        db_BrowserDB->Close();
        wxDbFreeConnection(db_BrowserDB);

        DbConnectInf.FreeHenv();

        db_BrowserDB = NULL;
    }
    if (!Quiet)
        wxLogMessage(_("\n-I-> BrowserDB::OnCloseDB() : End "));
    return true;
}

//----------------------------------------------------------------------------------------
bool BrowserDB::OnGetNext(int Cols,int WXUNUSED(Quiet))
{
    SDWORD cb;
    int i_dbDataType;
    wxChar s_temp[1024+1];
    long l_temp;
    double f_temp;
    int AnzError=0;
    TIMESTAMP_STRUCT t_temp;
    wxString Temp0;
    //-----------------------------
    if (!db_BrowserDB->GetNext())
    {
#ifdef __WXDEBUG__
        Temp0.Printf(_("\n-E-> BrowserDB::OnGetNext - ODBC-Error with GetNext \n-E-> "));
        Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
        wxLogMessage(Temp0);
        wxMessageBox(Temp0);
#endif
        return false;
    }
    else
    {
        int i;
        for (i=0;i<Cols;i++)
        {
            wxStrcpy((cl_BrowserDB+i)->tableName,_T("-E->"));
            i_dbDataType = (cl_BrowserDB+i)->pColFor->i_dbDataType;
            if (i_dbDataType == 0)         // Filter unsupported dbDataTypes
            {
                if (((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_VARCHAR) ||
                          ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_LONGVARCHAR))
                    i_dbDataType = DB_DATA_TYPE_VARCHAR;
                if ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_C_DATE)
                    i_dbDataType = DB_DATA_TYPE_DATE;
                if ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_C_BIT)
                    i_dbDataType = DB_DATA_TYPE_INTEGER;
                if ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_NUMERIC)
                    i_dbDataType = DB_DATA_TYPE_VARCHAR;
                if ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_REAL)
                    i_dbDataType = DB_DATA_TYPE_FLOAT;
            }
            if ((i_dbDataType == DB_DATA_TYPE_INTEGER) &&
                     ((cl_BrowserDB+i)->pColFor->i_sqlDataType == SQL_C_DOUBLE))
            {    // DBASE Numeric
                i_dbDataType = DB_DATA_TYPE_FLOAT;
            }
            switch(i_dbDataType)
            {
            case DB_DATA_TYPE_VARCHAR:
                wxStrcpy(s_temp,wxEmptyString);
                if (!db_BrowserDB->GetData((UWORD)(i+1),(SWORD)((cl_BrowserDB+i)->pColFor->i_dbDataType),&s_temp[0],sizeof(s_temp), &cb))
                {
                    Temp0.Printf(_("\n-E-> BrowserDB::OnGetNext - ODBC-Error with GetNext of >%s<.\n-E-> "),(cl_BrowserDB+i)->tableName);
                    Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
                    wxLogMessage(Temp0);
                }
                Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,s_temp);
                wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                break;
            case DB_DATA_TYPE_INTEGER:
                l_temp = 0;
                if (!db_BrowserDB->GetData((UWORD)(i+1),(SWORD)((cl_BrowserDB+i)->pColFor->i_sqlDataType),&l_temp,sizeof(l_temp), &cb))
                {
                    Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
                    Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
                }
                else
                {
                    Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,l_temp);
                    wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                }
                break;
            case DB_DATA_TYPE_FLOAT:
                f_temp = 0;
                if (!db_BrowserDB->GetData((UWORD)(i+1),(SWORD)((cl_BrowserDB+i)->pColFor->i_sqlDataType),&f_temp,sizeof(f_temp), &cb))
                {
                    Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
                    Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
                    wxMessageBox(Temp0);
                }
                else
                {
                    Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,f_temp);
                    wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                }
                break;
            case DB_DATA_TYPE_DATE:
                t_temp.day = 0;
                t_temp.month = 0;
                t_temp.year = 0;
                t_temp.hour = 0;
                t_temp.minute = 0;
                t_temp.second = 0;
                t_temp.fraction = 0;
                if (!db_BrowserDB->GetData((UWORD)(i+1),(SWORD)((cl_BrowserDB+i)->pColFor->i_sqlDataType),&t_temp,sizeof(t_temp), &cb))
                {
                    Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
                    Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
                }
                else
                {
                    // i_Nation =  0 = timestamp , 1=EU, 2=UK, 3=International, 4=US
                    if (((cl_BrowserDB+i)->pColFor->i_Nation == 0)    ||    // TS  YYYY-MM-DD
                        ((cl_BrowserDB+i)->pColFor->i_Nation == 3))     // IT  YYYY-MM-DD
                    {
                        Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,t_temp.year,t_temp.month,t_temp.day,
                            t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
                        wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                    }
                    if (((cl_BrowserDB+i)->pColFor->i_Nation == 1) ||  // EU  DD.MM.YYYY
                        ((cl_BrowserDB+i)->pColFor->i_Nation == 2))   // UK  DD/MM/YYYY
                    {
                        Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,t_temp.day,t_temp.month,t_temp.year,
                            t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
                        wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                    }
                    if ((cl_BrowserDB+i)->pColFor->i_Nation == 3)    // US  MM/DD/YYYY
                    {
                        Temp0.Printf((cl_BrowserDB+i)->pColFor->s_Field,t_temp.month,t_temp.day,t_temp.year,
                            t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
                        wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                    }
                }
                break;
            default:
                AnzError++;
                if (AnzError <= 100)
                {
                    Temp0 = (cl_BrowserDB+i)->colName;
                    wxLogMessage(_("-E-> BrowserDB::OnGetNext - DB_DATA_TYPE_?? (%d) in Col(%s)"),(cl_BrowserDB+i)->pColFor->i_dbDataType,Temp0.c_str());
                }
                else
                    return true;
                Temp0.Printf(_("-E-> unknown Format(%d) - sql(%d)"),(cl_BrowserDB+i)->pColFor->i_dbDataType,(cl_BrowserDB+i)->pColFor->i_sqlDataType);
                wxStrcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
                break;
            };  // switch
        }  // for
    }  // else

    return true;
}

//----------------------------------------------------------------------------------------
bool BrowserDB::OnSelect(wxString tb_Name, int Quiet)
{
    wxStopWatch sw;
    wxString SQLStmt;
    i_Records = 0;
    //---------------------------------------------------------------------------------------
    wxString tablename = db_BrowserDB->SQLTableName(tb_Name.c_str());
    SQLStmt.sprintf(_T("SELECT * FROM %s"),tablename.c_str());
    if (!db_BrowserDB->ExecSql((wxChar *)(SQLStmt.GetData())))
    {
        Temp0.Printf(_("\n-E-> BrowserDB::OnSelect - ODBC-Error with ExecSql of >%s<.\n-E-> "),tb_Name.c_str());
        Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
        wxLogMessage(Temp0);
        wxMessageBox(_T("-E-> BrowserDB::OnSelect - GetData()"));
        return false;
    }
    //---------------------------------------------------------------------------------------
    while (db_BrowserDB->GetNext())
    {
        i_Records++;
    }
    //---------------------------------------------------------------------------------------
    if (!db_BrowserDB->ExecSql((wxChar *)(SQLStmt.GetData())))
    {
        Temp0.Printf(_("\n-E-> BrowserDB::OnSelect - ODBC-Error with ExecSql of >%s<.\n-E-> "),tb_Name.c_str());
        Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
        wxLogMessage(Temp0);
        return false;
    }
    //---------------------------------------------------------------------------------------
    // SetColDefs ( 0, "NAME", DB_DATA_TYPE_VARCHAR, Name, SQL_C_WXCHAR, sizeof(Name), true, true);  // Primary index
    //---------------------------------------------------------------------------------------
    if (!Quiet)
    {
        wxLogMessage(_("\n-I-> BrowserDB::OnSelect(%s) Records(%d): End - Time needed : %ld ms"),tb_Name.c_str(),i_Records,sw.Time());
    }
    return true;
}

//----------------------------------------------------------------------------------------
bool BrowserDB::OnExecSql(wxString SQLStmt, int Quiet)
{
    //---------------------------------------------------------------------------------------
    if (!db_BrowserDB->ExecSql((wxChar *)(SQLStmt.GetData())))
    {
        Temp0.Printf(_("\n-E-> BrowserDB::OnExecSQL - ODBC-Error with ExecSql of >%s<.\n-E-> "),SQLStmt.c_str());
        Temp0 += GetExtendedDBErrorMsg(__TFILE__,__LINE__);
        if (!Quiet)
            wxLogMessage(Temp0);
        else
            wxMessageBox(_T("-E-> BrowserDB::OnExecSql - ExecSql()"));
        return false;
    }
    if (!Quiet)
    {
        // wxLogMessage(_("\n-I-> BrowserDB::OnExecSql(%s) - End - Time needed : %ld ms"),SQLStmt.c_str(),sw.Time());
    }
    return true;
}

//----------------------------------------------------------------------------------------
wxDbInf* BrowserDB::OnGetCatalog(int WXUNUSED(Quiet))
{
    wxChar UName[255];
    wxStrcpy(UName,UserName);
    ct_BrowserDB = db_BrowserDB->GetCatalog(UName);
    return ct_BrowserDB;
}

//----------------------------------------------------------------------------------------
wxDbColInf* BrowserDB::OnGetColumns(wxChar *tableName, UWORD numCols, int WXUNUSED(Quiet))
{
    wxChar UName[255];
    int i;
    wxStrcpy(UName,UserName);
    cl_BrowserDB = db_BrowserDB->GetColumns(tableName,&numCols,UName);
//    cl_BrowserDB->pColFor = new wxDbColFor[numCols];
    for (i=0;i<numCols;i++)
    {
//        (cl_BrowserDB->pColFor+i)->Format(1,(cl_BrowserDB+i)->dbDataType,(cl_BrowserDB+i)->sqlDataType,
//            (cl_BrowserDB+i)->columnSize, (cl_BrowserDB+i)->decimalDigits);
        (cl_BrowserDB+i)->pColFor = new wxDbColFor;
        (cl_BrowserDB+i)->pColFor->Format(1,
                                        (cl_BrowserDB+i)->dbDataType,
                                        (cl_BrowserDB+i)->sqlDataType,
                                        (cl_BrowserDB+i)->columnLength,
                                        (cl_BrowserDB+i)->decimalDigits);
    }
    return cl_BrowserDB;
}

//----------------------------------------------------------------------------------------
void BrowserDB::PointerToNULL(int Art)
{
    if (Art == 1) // Löschen
    {
        if (cl_BrowserDB != NULL)
        { // Destroy the memory
            delete [] cl_BrowserDB;
        }
        if (ct_BrowserDB != NULL)
        { // Destroy the memory
            delete [] ct_BrowserDB;
        }
        if (db_BrowserDB != NULL)
        {
            db_BrowserDB->CommitTrans();
            db_BrowserDB->Close();
            wxDbCloseConnections();
            delete db_BrowserDB;
        }
    }
    cl_BrowserDB       = NULL;
    ct_BrowserDB       = NULL;
    db_BrowserDB       = NULL;
    p_LogWindow        = NULL;
}

//----------------------------------------------------------------------------------------
void BrowserDB::OnFillSqlTyp()
{
    i_SqlTyp[1]  = SQL_C_BINARY;    s_SqlTyp[1]  = _T("SQL_C_BINARY");
    i_SqlTyp[2]  = SQL_C_BIT;       s_SqlTyp[2]  = _T("SQL_C_BIT");
    i_SqlTyp[3]  = SQL_C_BOOKMARK;  s_SqlTyp[3]  = _T("SQL_C_BOOKMARK");
    i_SqlTyp[4]  = SQL_C_WXCHAR;    s_SqlTyp[4]  = _T("SQL_C_WXCHAR");
    i_SqlTyp[5]  = SQL_C_DATE;      s_SqlTyp[5]  = _T("SQL_C_DATE");
    i_SqlTyp[6]  = SQL_C_DEFAULT;   s_SqlTyp[6]  = _T("SQL_C_DEFAULT");
    i_SqlTyp[7]  = SQL_C_DOUBLE;    s_SqlTyp[7]  = _T("SQL_C_DOUBLE");
    i_SqlTyp[8]  = SQL_C_FLOAT;     s_SqlTyp[8]  = _T("SQL_C_FLOAT");
    i_SqlTyp[9]  = SQL_C_LONG;      s_SqlTyp[9]  = _T("SQL_C_LONG");
    i_SqlTyp[10] = SQL_C_SHORT;     s_SqlTyp[10] = _T("SQL_C_SHORT");
    i_SqlTyp[11] = SQL_C_SLONG;     s_SqlTyp[11] = _T("SQL_C_SLONG");
    i_SqlTyp[12] = SQL_C_SSHORT;    s_SqlTyp[12] = _T("SQL_C_SSHORT");
    i_SqlTyp[13] = SQL_C_STINYINT;  s_SqlTyp[13] = _T("SQL_C_STINYINT");
    i_SqlTyp[14] = SQL_C_TIME;      s_SqlTyp[14] = _T("SQL_C_TIME");
    i_SqlTyp[15] = SQL_C_TIMESTAMP; s_SqlTyp[15] = _T("SQL_C_TIMESTAMP");
    i_SqlTyp[16] = SQL_C_TINYINT;   s_SqlTyp[16] = _T("SQL_C_TINYINT");
    i_SqlTyp[17] = SQL_C_ULONG;     s_SqlTyp[17] = _T("SQL_C_ULONG");
    i_SqlTyp[18] = SQL_C_USHORT;    s_SqlTyp[18] = _T("SQL_C_USHORT");
    i_SqlTyp[19] = SQL_C_UTINYINT;  s_SqlTyp[19] = _T("SQL_C_UTINYINT");
    i_SqlTyp[20] = SQL_VARCHAR;     s_SqlTyp[20] = _T("SQL_VARCHAR");
    i_SqlTyp[21] = SQL_NUMERIC;     s_SqlTyp[21] = _T("SQL_NUMERIC");
    i_SqlTyp[22] = SQL_LONGVARCHAR; s_SqlTyp[22] = _T("SQL_LONGVARCHAR");
    i_SqlTyp[23] = SQL_REAL;        s_SqlTyp[23] = _T("SQL_REAL");
    i_SqlTyp[0]  = 23;              s_SqlTyp[0]  = wxEmptyString;
}

//----------------------------------------------------------------------------------------
void BrowserDB::OnFilldbTyp()
{
    i_dbTyp[1] = DB_DATA_TYPE_VARCHAR; s_dbTyp[1] = _T("DB_DATA_TYPE_VARCHAR");
    i_dbTyp[2] = DB_DATA_TYPE_INTEGER; s_dbTyp[2] = _T("DB_DATA_TYPE_INTEGER");
    i_dbTyp[3] = DB_DATA_TYPE_FLOAT;   s_dbTyp[3] = _T("DB_DATA_TYPE_FLOAT");
    i_dbTyp[4] = DB_DATA_TYPE_DATE;    s_dbTyp[4] = _T("DB_DATA_TYPE_DATE");
    i_dbTyp[0] = 4;                    s_dbTyp[0] = wxEmptyString;
}
//----------------------------------------------------------------------------------------
