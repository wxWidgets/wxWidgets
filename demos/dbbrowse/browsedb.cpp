//----------------------------------------------------------------------------------------
// Name:        BrowserDB.h,cpp
// Purpose:     a wxDB class
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// Created:     19991127.mj10777
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
//-- 1)
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
#include "std.h"
//----------------------------------------------------------------------------------------
// Global structure for holding ODBC connection information
// - darf nur einmal im Projekte definiert werden ?? Extra Databasse Klasse ?
wxDbConnectInf ConnectInf;      // Für DBase
//----------------------------------------------------------------------------------------
extern wxDbList* WXDLLEXPORT PtrBegDbList;    /* from db.cpp, used in getting back error results from db connections */
//----------------------------------------------------------------------------------------
char *GetExtendedDBErrorMsg(char *ErrFile, int ErrLine)
{
 static wxString msg;
 wxString tStr;
 if (ErrFile || ErrLine)
 {
  msg += "File: ";
  msg += ErrFile;
  msg += "   Line: ";
  tStr.Printf("%d",ErrLine);
  msg += tStr.GetData();
  // msg += "\n";
 }
 msg.Append ("\nODBC errors:\n");
 // msg += "\n";
 /* Scan through each database connection displaying
  * any ODBC errors that have occured. */
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
    if (strcmp(pDbList->PtrDb->errorList[i],"") != 0)
     msg.Append("\n");
   }
  }
 }
 msg += "\n";
 return (char*) (const char*) msg;
}  // GetExtendedDBErrorMsg

//----------------------------------------------------------------------------------------
BrowserDB::BrowserDB()
{
 Zeiger_auf_NULL(0);
 ODBCSource = "";             // ODBC data source name (created with ODBC Administrator under Win95/NT)
 UserName   = "";             // database username - must already exist in the data source
 Password   = "";             // password database username
 OnFillSqlTyp();
 OnFilldbTyp();
}  // BrowserDB Constructor
//----------------------------------------------------------------------------------------
BrowserDB::~BrowserDB()
{
 Zeiger_auf_NULL(1);  // Clean up Tables and Databases (Commit, Close und delete)
}  // BrowserDB destructor
//----------------------------------------------------------------------------------------
bool BrowserDB::Initialize(int Quite)
{
 if (!OnStartDB(Quite))
 {
  wxLogMessage(_("\n\n-E-> BrowserDB::OnStartDB(%s) : Failed ! "),ODBCSource.c_str());
  return FALSE;
 }
 return TRUE;
}  // BrowserDB:Initialize
//----------------------------------------------------------------------------------------
bool BrowserDB::OnStartDB(int Quite)
{
 wxStopWatch sw;
 if (!Quite)
  wxLogMessage(_("\n-I-> BrowserDB::OnStartDB(%s) : Begin "),ODBCSource.c_str());
 if (db_BrowserDB != NULL)
 {
  if (!Quite)
   wxLogMessage(_("\n-I-> BrowserDB::OnStartDB() : DB is allready open."));
  return TRUE;
 }
 // Initialize the ODBC Environment for Database Operations
 if (SQLAllocEnv(&ConnectInf.Henv) != SQL_SUCCESS)
 {
  if (!Quite)
   wxLogMessage(_("\n-E-> BrowserDB::OnStartDB() : DB CONNECTION ERROR : A problem occured while trying to get a connection to the data source"));
  return FALSE;
 }
 //---------------------------------------------------------------------------------------
 // Connect to datasource
 //---------------------------------------------------------------------------------------
 DlgUser *p_Dlg;
 p_Dlg = new DlgUser(pDoc->p_MainFrame,pDoc,"");
 p_Dlg->s_DSN      = ODBCSource;
 p_Dlg->s_User     = UserName;
 p_Dlg->s_Password = Password;
 p_Dlg->OnInit();
 p_Dlg->Fit();

 bool OK = FALSE;
 if (p_Dlg->ShowModal() == wxID_OK)
 {
  (pDoc->p_DSN+i_Which)->Usr = p_Dlg->s_User;
  (pDoc->p_DSN+i_Which)->Pas = p_Dlg->s_Password;
  UserName  = p_Dlg->s_User;
  Password  = p_Dlg->s_Password;
  OK = TRUE;
 }
 delete p_Dlg;
 if (OK)
 {
  //--------------------------------------------------------------------------------------
  strcpy(ConnectInf.Dsn, ODBCSource);           // ODBC data source name (created with ODBC Administrator under Win95/NT)
  strcpy(ConnectInf.Uid, UserName);             // database username - must already exist in the data source
  strcpy(ConnectInf.AuthStr, Password);         // password database username
  db_BrowserDB = wxDbGetConnection(&ConnectInf);
  // wxLogMessage(">>>%s<<<>>>%s<<<",UserName.c_str(),Password.c_str());
  if (db_BrowserDB == NULL)
  {
   strcpy(ConnectInf.Dsn, "");
   strcpy(ConnectInf.Uid, "");
   strcpy(ConnectInf.AuthStr, "");
   if (!Quite)
   {
    wxLogMessage(_("\n-E-> BrowserDB::OnConnectDataSource() DB CONNECTION ERROR : Unable to connect to the data source.\n\nCheck the name of your data source to verify it has been correctly entered/spelled.\n\nWith some databases, the user name and password must\nbe created with full rights to the table prior to making a connection\n(using tools provided by the database manufacturer)"));
    wxLogMessage(_("-I-> BrowserDB::OnStartDB(%s) : End - Time needed : %ld ms"),ODBCSource.c_str(),sw.Time());
   }
   return FALSE;
  }
  //--------------------------------------------------------------------------------------
  if (!Quite)
  {
   Temp1 = db_BrowserDB->GetDatabaseName();
   Temp2 = db_BrowserDB->GetDataSource();
   wxLogMessage(_("-I-> BrowserDB::OnGetDataSourceODBC() - DatabaseName(%s) ; DataSource(%s)"),Temp1.c_str(),Temp2.c_str());
   wxLogMessage(_("-I-> BrowserDB::OnStartDB(%s) : End - Time needed : %ld ms"),ODBCSource.c_str(),sw.Time());
  }
  return TRUE;
 } else return FALSE;
}
//----------------------------------------------------------------------------------------
bool BrowserDB::OnCloseDB(int Quite)
{
 if (!Quite)
  wxLogMessage(_("-I-> BrowserDB::OnCloseDB() : Begin "));
 if (db_BrowserDB)
 {
  db_BrowserDB->Close();
  wxDbFreeConnection(db_BrowserDB);

  // Free Environment Handle that ODBC uses
  if (SQLFreeEnv(&ConnectInf.Henv) != SQL_SUCCESS)
  {
   // Error freeing environment handle
  }
  db_BrowserDB = NULL;
 }
 if (!Quite)
  wxLogMessage(_("\n-I-> BrowserDB::OnCloseDB() : End "));
 return TRUE;
}
//----------------------------------------------------------------------------------------
bool BrowserDB::OnGetNext(int Cols,int Quite)
{
 SDWORD cb;
 int      i_dbDataType;
 int   i=0;
 char             s_temp[1024+1];
 long             l_temp;
 double           f_temp;
 int              AnzError=0;
 TIMESTAMP_STRUCT t_temp;
 wxString Temp0;
 //-----------------------------
 if (!db_BrowserDB->GetNext())
 {
  return FALSE;
  Temp0.Printf(_("\n-E-> BrowserDB::OnGetNext - ODBC-Error with GetNext \n-E-> "));
  Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
  wxLogMessage(Temp0);
  wxMessageBox(Temp0);
 }
 else
 {
  for (i=0;i<Cols;i++)
  {
   strcpy((cl_BrowserDB+i)->tableName,"-E->");
   i_dbDataType = (cl_BrowserDB->pColFor+i)->i_dbDataType;
   if (i_dbDataType == 0)                        // Filter unsupported dbDataTypes
   {
    if (((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_VARCHAR) || ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_LONGVARCHAR))
     i_dbDataType = DB_DATA_TYPE_VARCHAR;
    if ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_C_DATE)
     i_dbDataType = DB_DATA_TYPE_DATE;
    if ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_C_BIT)
     i_dbDataType = DB_DATA_TYPE_INTEGER;
    if ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_NUMERIC)
     i_dbDataType = DB_DATA_TYPE_VARCHAR;
    if ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_REAL)
     i_dbDataType = DB_DATA_TYPE_FLOAT;
   }
   if ((i_dbDataType == DB_DATA_TYPE_INTEGER) && ((cl_BrowserDB->pColFor+i)->i_sqlDataType == SQL_C_DOUBLE))
   {   // DBASE Numeric
    i_dbDataType = DB_DATA_TYPE_FLOAT;
   }
   switch(i_dbDataType)
   {
    case DB_DATA_TYPE_VARCHAR:
     strcpy(s_temp,"");
     if (!db_BrowserDB->GetData(i+1,(cl_BrowserDB->pColFor+i)->i_dbDataType,&s_temp,sizeof(s_temp), &cb))
     {
      Temp0.Printf(_("\n-E-> BrowserDB::OnGetNext - ODBC-Error with GetNext of >%s<.\n-E-> "),(cl_BrowserDB+i)->tableName);
      Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
      wxLogMessage(Temp0);
     }
     Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,s_temp);
     strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
    break;
    case DB_DATA_TYPE_INTEGER:
     l_temp = 0;
     if (!db_BrowserDB->GetData(i+1,(cl_BrowserDB->pColFor+i)->i_sqlDataType,&l_temp,sizeof(l_temp), &cb))
     {
      Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
      Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
     }
     else
     {
      Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,l_temp);
      strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
     }
    break;
    case DB_DATA_TYPE_FLOAT:
     f_temp = 0;
     if (!db_BrowserDB->GetData(i+1,(cl_BrowserDB->pColFor+i)->i_sqlDataType,&f_temp,sizeof(f_temp), &cb))
     {
      Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
      Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
      wxMessageBox(Temp0);
     }
     else
     {
      Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,f_temp);
      strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
     }
    break;
    case DB_DATA_TYPE_DATE:
     t_temp.day = t_temp.month = t_temp.year = t_temp.hour = t_temp.minute = t_temp.second = t_temp.fraction = 0;
     if (!db_BrowserDB->GetData(i+1,(cl_BrowserDB->pColFor+i)->i_sqlDataType,&t_temp,sizeof(t_temp), &cb))
     {
      Temp0.Printf(_("\n-E-> BrowserDB::OnGetData - ODBC-Error with GetNext \n-E-> "));
      Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
     }
     else
     {
      // i_Nation =  0 = timestamp , 1=EU, 2=UK, 3=International, 4=US
      if (((cl_BrowserDB->pColFor+i)->i_Nation == 0)  ||  // TS  YYYY-MM-DD
          ((cl_BrowserDB->pColFor+i)->i_Nation == 3))     // IT  YYYY-MM-DD
      {
       Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,t_temp.year,t_temp.month,t_temp.day,
                                   t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
       strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
      }
      if (((cl_BrowserDB->pColFor+i)->i_Nation == 1) ||  // EU  DD.MM.YYYY
           ((cl_BrowserDB->pColFor+i)->i_Nation == 2))   // UK  DD/MM/YYYY
      {
       Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,t_temp.day,t_temp.month,t_temp.year,
                                   t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
       strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
      }
      if ((cl_BrowserDB->pColFor+i)->i_Nation == 3)      // US  MM/DD/YYYY
      {
       Temp0.Printf((cl_BrowserDB->pColFor+i)->s_Field,t_temp.month,t_temp.day,t_temp.year,
                                   t_temp.hour, t_temp.minute, t_temp.second, t_temp.fraction);
       strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
      }
     }
    break;
    default:
     AnzError++;
     if (AnzError <= 100)
     {
      Temp0 = (cl_BrowserDB+i)->colName;
      wxLogMessage(_("-E-> BrowserDB::OnGetNext - DB_DATA_TYPE_?? (%d) in Col(%s)"),(cl_BrowserDB->pColFor+i)->i_dbDataType,Temp0.c_str());
     }
     else
      return TRUE;
     Temp0.Printf(_("-E-> unknown Format(%d) - sql(%d)"),(cl_BrowserDB->pColFor+i)->i_dbDataType,(cl_BrowserDB->pColFor+i)->i_sqlDataType);
     strcpy((cl_BrowserDB+i)->tableName,Temp0.c_str());
    break;
   };
  }
 }
 // wxLogMessage("-E-> BrowserDB::OnGetNext - End");
 return TRUE;
}
//----------------------------------------------------------------------------------------
bool BrowserDB::OnSelect(wxString tb_Name, int Quite)
{
 wxStopWatch sw;
 wxString SQLStmt;
 i_Records = 0;
 //---------------------------------------------------------------------------------------
 SQLStmt.sprintf("SELECT * FROM %s",tb_Name.c_str());
 if (!db_BrowserDB->ExecSql((char *)(SQLStmt.GetData())))
 {
  Temp0.Printf(_("\n-E-> BrowserDB::OnSelect - ODBC-Error with ExecSql of >%s<.\n-E-> "),tb_Name.c_str());
  Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
  wxLogMessage(Temp0);
  wxMessageBox("-E-> BrowserDB::OnSelect - GetData()");
  return FALSE;
 }
 //---------------------------------------------------------------------------------------
 while (db_BrowserDB->GetNext())
 {
  i_Records++;
 }
 //---------------------------------------------------------------------------------------
 if (!db_BrowserDB->ExecSql((char *)(SQLStmt.GetData())))
 {
  Temp0.Printf(_("\n-E-> BrowserDB::OnSelect - ODBC-Error with ExecSql of >%s<.\n-E-> "),tb_Name.c_str());
  Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
  wxLogMessage(Temp0);
  return FALSE;
 }
 //---------------------------------------------------------------------------------------
 // SetColDefs ( 0,"NAME",     DB_DATA_TYPE_VARCHAR,  Name,     SQL_C_CHAR,      sizeof(Name),    TRUE, TRUE);  // Primary index
 //---------------------------------------------------------------------------------------
 if (!Quite)
 {
  wxLogMessage(_("\n-I-> BrowserDB::OnSelect(%s) Records(%d): End - Time needed : %ld ms"),tb_Name.c_str(),i_Records,sw.Time());
 }
 return TRUE;
}
//----------------------------------------------------------------------------------------
bool BrowserDB::OnExecSql(wxString SQLStmt, int Quite)
{
 //---------------------------------------------------------------------------------------
 if (!db_BrowserDB->ExecSql((char *)(SQLStmt.GetData())))
 {
  Temp0.Printf(_("\n-E-> BrowserDB::OnExecSQL - ODBC-Error with ExecSql of >%s<.\n-E-> "),SQLStmt.c_str());
  Temp0 += GetExtendedDBErrorMsg(__FILE__,__LINE__);
  if (!Quite)
   wxLogMessage(Temp0);
  else
   wxMessageBox("-E-> BrowserDB::OnExecSql - ExecSql()");
  return FALSE;
 }
 if (!Quite)
 {
  // wxLogMessage(_("\n-I-> BrowserDB::OnExecSql(%s) - End - Time needed : %ld ms"),SQLStmt.c_str(),sw.Time());
 }
 return TRUE;
}
//----------------------------------------------------------------------------------------
wxDbInf* BrowserDB::OnGetCatalog(int Quite)
{
 char UName[255];
 strcpy(UName,UserName);
 ct_BrowserDB = db_BrowserDB->GetCatalog(UName);
 return ct_BrowserDB;
}
//----------------------------------------------------------------------------------------
wxColInf* BrowserDB::OnGetColumns(char *tableName, int numCols,int Quite)
{
 char UName[255];
 int i;
 strcpy(UName,UserName);
 cl_BrowserDB = db_BrowserDB->GetColumns(tableName,&numCols,UName);
 cl_BrowserDB->pColFor = new wxColFor[numCols];
 for (i=0;i<numCols;i++)
 {
  (cl_BrowserDB->pColFor+i)->Format(1,(cl_BrowserDB+i)->dbDataType,(cl_BrowserDB+i)->sqlDataType,
  (cl_BrowserDB+i)->columnSize, (cl_BrowserDB+i)->decimalDigits);
 }
 return cl_BrowserDB;
}
//----------------------------------------------------------------------------------------
void BrowserDB::Zeiger_auf_NULL(int Art)
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
 i_SqlTyp[1]  = SQL_C_BINARY;     s_SqlTyp[1]  = "SQL_C_BINARY";
 i_SqlTyp[2]  = SQL_C_BIT;        s_SqlTyp[2]  = "SQL_C_BIT";
 i_SqlTyp[3]  = SQL_C_BOOKMARK;   s_SqlTyp[3]  = "SQL_C_BOOKMARK";
 i_SqlTyp[4]  = SQL_C_CHAR;       s_SqlTyp[4]  = "SQL_C_CHAR";
 i_SqlTyp[5]  = SQL_C_DATE;       s_SqlTyp[5]  = "SQL_C_DATE";
 i_SqlTyp[6]  = SQL_C_DEFAULT;    s_SqlTyp[6]  = "SQL_C_DEFAULT";
 i_SqlTyp[7]  = SQL_C_DOUBLE;     s_SqlTyp[7]  = "SQL_C_DOUBLE";
 i_SqlTyp[8]  = SQL_C_FLOAT;      s_SqlTyp[8]  = "SQL_C_FLOAT";
 i_SqlTyp[9]  = SQL_C_LONG;       s_SqlTyp[9]  = "SQL_C_LONG";
 i_SqlTyp[10] = SQL_C_SHORT;      s_SqlTyp[10] = "SQL_C_SHORT";
 i_SqlTyp[11] = SQL_C_SLONG;      s_SqlTyp[11] = "SQL_C_SLONG";
 i_SqlTyp[12] = SQL_C_SSHORT;     s_SqlTyp[12] = "SQL_C_SSHORT";
 i_SqlTyp[13] = SQL_C_STINYINT;   s_SqlTyp[13] = "SQL_C_STINYINT";
 i_SqlTyp[14] = SQL_C_TIME;       s_SqlTyp[14] = "SQL_C_TIME";
 i_SqlTyp[15] = SQL_C_TIMESTAMP;  s_SqlTyp[15] = "SQL_C_TIMESTAMP";
 i_SqlTyp[16] = SQL_C_TINYINT;    s_SqlTyp[16] = "SQL_C_TINYINT";
 i_SqlTyp[17] = SQL_C_ULONG;      s_SqlTyp[17] = "SQL_C_ULONG";
 i_SqlTyp[18] = SQL_C_USHORT;     s_SqlTyp[18] = "SQL_C_USHORT";
 i_SqlTyp[19] = SQL_C_UTINYINT;   s_SqlTyp[19] = "SQL_C_UTINYINT";
 i_SqlTyp[20] = SQL_VARCHAR;      s_SqlTyp[20] = "SQL_VARCHAR";
 i_SqlTyp[21] = SQL_NUMERIC;      s_SqlTyp[21] = "SQL_NUMERIC";
 i_SqlTyp[22] = SQL_LONGVARCHAR;  s_SqlTyp[22] = "SQL_LONGVARCHAR";
 i_SqlTyp[23] = SQL_REAL;         s_SqlTyp[23] = "SQL_REAL";
 i_SqlTyp[0]  = 23;               s_SqlTyp[0]  = "";
}
//----------------------------------------------------------------------------------------
void BrowserDB::OnFilldbTyp()
{
 i_dbTyp[1]   = DB_DATA_TYPE_VARCHAR;     s_dbTyp[1]  = "DB_DATA_TYPE_VARCHAR";
 i_dbTyp[2]   = DB_DATA_TYPE_INTEGER;     s_dbTyp[2]  = "DB_DATA_TYPE_INTEGER";
 i_dbTyp[3]   = DB_DATA_TYPE_FLOAT;       s_dbTyp[3]  = "DB_DATA_TYPE_FLOAT";
 i_dbTyp[4]   = DB_DATA_TYPE_DATE;        s_dbTyp[4]  = "DB_DATA_TYPE_DATE";
 i_dbTyp[0]   = 4;                        s_dbTyp[0]   = "";
}
//----------------------------------------------------------------------------------------
