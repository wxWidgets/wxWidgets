/////////////////////////////////////////////////////////////////////////////
// Name:        odbc.cpp
// Purpose:     ODBC implementation
// Author:      Julian Smart, Olaf Klein (oklein@smallo.ruhr.de),
//               Patrick Halke (patrick@zaphod.ruhr.de)
// Modified by:	
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "odbc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_ODBC

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dialog.h"
#endif

#include "wx/string.h"
#include "wx/odbc.h"

#include <math.h>
#include <stdlib.h>

#if defined(__WXMSW__) && !defined(__WIN32__)
#include <print.h>
#endif

HENV wxDatabase::hEnv = 0;
int wxDatabase::refCount = 0;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxDatabase, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxQueryCol, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxQueryField, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxRecordSet, wxObject)
#endif

wxDatabase::wxDatabase(void)
{
  hDBC = 0;
  username = NULL;
  password = NULL;
  datasource = NULL;
  dbname = NULL;
  connectstring = NULL;
  isOpen = FALSE;
  refCount ++;
  retcode = 0;
  username = NULL;
  password = NULL;
  err_occured = FALSE;

  memset(sqlstate, 0, sizeof sqlstate);
  memset(errmsg, 0, sizeof errmsg);
  
  if (hEnv == 0)
  {
    retcode = SQLAllocEnv(&hEnv);

    if (retcode != SQL_SUCCESS)
      hEnv = 0;
  }
}

wxDatabase::~wxDatabase(void)
{
  Close();
  DeleteRecordSets(); // Added JACS

  if (connectstring)
    delete[] connectstring;
  if (datasource)
    delete[] datasource;
  if (username)
    delete[] username;
  if (password)
    delete[] password;
  if (dbname)
    delete[] dbname;
  
  refCount --;
  if (!refCount && hEnv)
  {
    retcode = SQLFreeEnv(hEnv);
    hEnv = 0; // JACS 17/6
  }
}

void wxDatabase::ErrorSnapshot(HSTMT hstmt)
{
  SWORD len = 0; // JACS: sometimes doesn't get filled in by SQLError.
  
  err_occured = TRUE;
  SQLError(hEnv, hDBC, hstmt, (unsigned char *)sqlstate, &nat_err, (unsigned char *)errmsg, SQL_MAX_MESSAGE_LENGTH-1, &len);
  errmsg[len] = '\0';
}

bool wxDatabase::ErrorOccured(void) 
{
  return err_occured;
}

char* wxDatabase::GetErrorMessage(void)
{
  return errmsg;
}

long wxDatabase::GetErrorNumber(void) 
{
  return nat_err;
}

char* wxDatabase::GetErrorClass(void) {
  return sqlstate;
}

bool wxDatabase::Open(char *thedatasource, bool WXUNUSED(exclusive), 
  bool WXUNUSED(readOnly), char *username, char *password)
{
  err_occured = FALSE;
  
  if (isOpen)
    return FALSE;
  
  SetUsername(username);
  SetPassword(password);
  SetDataSource(thedatasource);
  
  if (!hEnv)
    return FALSE;

  retcode = SQLAllocConnect(hEnv, &hDBC);
  if (retcode != SQL_SUCCESS) {
    hDBC = 0;
    return FALSE;
  }
  
  retcode = SQLConnect(hDBC, (UCHAR FAR*)thedatasource, strlen(thedatasource), (UCHAR FAR*)username, strlen(username),
		       (UCHAR FAR*)password, strlen(password));
  
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
    ErrorSnapshot();
    return FALSE;
  }

  isOpen = TRUE;
  
  return TRUE;
}

bool wxDatabase::Close(void)
{
  // JACS: make sure the record set statements are all released.
  ResetRecordSets();
  
  err_occured = FALSE;
  if (hDBC != 0)
  {
    retcode = SQLDisconnect(hDBC);

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
      ErrorSnapshot();      
      return FALSE;
    }

    retcode = SQLFreeConnect(hDBC);

    hDBC = 0;
    isOpen = FALSE;
    
    return TRUE;
  }
  
  return FALSE;
}

// Database attributes
char *wxDatabase::GetDatabaseName(void)
{
  err_occured = FALSE;
  if (hDBC == 0)
    return NULL;
    
  char nameBuf[400];
  int nameSize = 0;

  retcode = SQLGetInfo(hDBC, SQL_DATABASE_NAME, (unsigned char*)nameBuf, sizeof(nameBuf), (short *)&nameSize);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    return NULL;

  delete[] dbname;
  dbname = NULL;
  
  if (nameSize > 0)
  {
    dbname = copystring(nameBuf);
    return dbname;
  }
  else
    return NULL;
}
  
bool wxDatabase::CanUpdate(void)
{
  return FALSE;
}

bool wxDatabase::CanTransact(void)
{
  return FALSE;
}

bool wxDatabase::InWaitForDataSource(void)
{
  return FALSE;
}

void wxDatabase::SetLoginTimeout(long WXUNUSED(seconds))
{
}

void wxDatabase::SetQueryTimeout(long WXUNUSED(seconds))
{
}

void wxDatabase::SetSynchronousMode(bool WXUNUSED(synchronous))
{
}

// Database operations
bool wxDatabase::BeginTrans(void)
{
  return FALSE;
}

bool wxDatabase::CommitTrans(void)
{
  return FALSE;
}

bool wxDatabase::RollbackTrans(void)
{
  return FALSE;
}

void wxDatabase::Cancel(void)
{
}

// Overridables
void wxDatabase::OnSetOptions(wxRecordSet *WXUNUSED(recordSet))
{
}

void wxDatabase::OnWaitForDataSource(bool WXUNUSED(stillExecuting))
{
}

void wxDatabase::SetPassword(char *s)
{
  if (password)
    delete[] password;
  if (s)
  {
    password = copystring(s);
  }
  else
    password = NULL;
}

void wxDatabase::SetUsername(char *s)
{
  delete[] username;
  
  if (s)
    username = copystring(s);
  else
    username = NULL;
}

void wxDatabase::SetDataSource(char *s)
{
  delete[] datasource;
  
  if (s)
    datasource = copystring(s);
  else
    datasource = NULL;
}

/*
 * Added by JACS
 */

void wxDatabase::DeleteRecordSets(void)
{
  wxNode *node = recordSets.First();
  while (node)
  {
    wxNode *next = node->Next();
    wxRecordSet *rec = (wxRecordSet *)node->Data();
    delete rec;
    // The node is implicitly deleted by ~wxRecordSet
    node = next;
  }
}

void wxDatabase::ResetRecordSets(void)
{
  wxNode *node = recordSets.First();
  while (node)
  {
    wxRecordSet *rec = (wxRecordSet *)node->Data();
    rec->ReleaseHandle();

    node = node->Next();
  }
}

bool wxDatabase::GetInfo(long infoType, long *buf)
{
  short sz = 0;
  retcode = SQLGetInfo(hDBC, (UWORD)infoType, (unsigned char*)buf, sizeof(buf), &sz);
  
  if (retcode != SQL_ERROR)
    return TRUE;
  else
    return FALSE;
}

bool wxDatabase::GetInfo(long infoType, char *buf, int bufSize)
{
  if (bufSize == -1)
    bufSize = sizeof(buf);
    
  short sz = 0;
  retcode = SQLGetInfo(hDBC, (UWORD)infoType, (unsigned char*)buf, bufSize, &sz);
  
  if (retcode != SQL_ERROR)
    return TRUE;
  else
    return FALSE;
}

wxString wxDatabase::GetODBCVersionString(bool implementation)
{
  char buf[50];
  if (!implementation)
  {
#ifdef SQL_SPEC_MAJOR
    sprintf(buf, "%d%d.%d", (int)(SQL_SPEC_MAJOR/10), (int)(SQL_SPEC_MAJOR - (((int)(SQL_SPEC_MAJOR/10))*10)),
             SQL_SPEC_MINOR);
    return wxString(buf);
#else
    return wxString("00.00");
#endif
  }
  
  bool noDBC = FALSE;
  if (hDBC == 0)
  {
    noDBC = TRUE;
    retcode = SQLAllocConnect(hEnv, &hDBC);
    if (retcode != SQL_SUCCESS)
    {
      hDBC = 0;
      return wxString("00.00");
    }
  }

  int bufSize = sizeof(buf);
    
  short sz = 0;
  retcode = SQLGetInfo(hDBC, (UWORD)SQL_ODBC_VER, (unsigned char*)buf, bufSize, &sz);
  
  if (hDBC != 0 && noDBC)
  {
    retcode = SQLFreeConnect(hDBC);
    hDBC = 0;
  }

  if (retcode == SQL_ERROR)
    return wxString("");
  else
    return wxString(buf);
}

float wxDatabase::GetODBCVersionFloat(bool implementation)
{
  if (!implementation)
  {
#ifdef SQL_SPEC_MAJOR
    return (float)(SQL_SPEC_MAJOR + (SQL_SPEC_MINOR/100.0));
#else
    return 0.0;
#endif
  }

  bool noDBC = FALSE;
  if (hDBC == 0)
  {
    noDBC = TRUE;
    retcode = SQLAllocConnect(hEnv, &hDBC);
    if (retcode != SQL_SUCCESS)
    {
      hDBC = 0;
      return (float)0.0;
    }
  }

  char buf[50];
  int bufSize = sizeof(buf);
 
  short sz = 0;
  retcode = SQLGetInfo(hDBC, (UWORD)SQL_ODBC_VER, (unsigned char*)buf, bufSize, &sz);

  if (hDBC != 0 && noDBC)
  {
    retcode = SQLFreeConnect(hDBC);
    hDBC = 0;
  }

  if (retcode == SQL_ERROR)
    return 0.0;
  else
    return atof(buf);
}

/*
 * wxRecordSet
 */
 
wxRecordSet::wxRecordSet(wxDatabase *db, int typ, int opt):
    cols(wxKEY_STRING)
{
  parentdb = db;
  hStmt = 0;
  nFields = 0;
  nParams = 0;
  recordFilter = NULL;
  sortString = NULL;
  retcode = 0;
  cursor = 0;
  tablename = NULL;
  nCols = 0;
  nRecords = 0;
  
  type = typ;
  options = opt;

  // Added JACS
  if (parentdb)
    parentdb->GetRecordSets().Append(this);
}

wxRecordSet::~wxRecordSet(void)
{
  ReleaseHandle();
  
  // JACS
  if (parentdb)
    parentdb->GetRecordSets().DeleteObject(this);
    
  if (recordFilter)
    delete[] recordFilter;
  if (sortString)
    delete[] sortString;
  if (tablename)
    delete[] tablename;
}

// If SQL is non-NULL, table and columns can be NULL.
bool wxRecordSet::BeginQuery(int WXUNUSED(openType), char *WXUNUSED(sql), int WXUNUSED(options))
{
  // Needs to construct an appropriate SQL statement. By default
  // (i.e. if table and columns are provided) then
  // SELECT <columns> FROM <table>
  // will be used.
  if (!parentdb)
    return FALSE;
  if (!parentdb->GetHDBC())
    return FALSE;
    
  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = 0;
  }

  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  if (retcode != SQL_SUCCESS)
    return FALSE;

  return TRUE;
}

bool wxRecordSet::Query(char *columns, char *table, char *filter)
{
  // Needs to construct an appropriate SQL statement. By default
  // (i.e. if table and columns are provided) then
  // SELECT <columns> FROM <table>
  // will be used.

  char* thetable = table ? table : tablename;
  
  if (!thetable)
    return FALSE;
  if (!columns)
    return FALSE;
    
  wxString query;

  query += "SELECT ";
  query += columns;
  query += " FROM ";
  query += thetable;

  if (filter)  {
    query += " WHERE ";
    query += filter;
  }
  retcode = SQLPrepare(hStmt, (UCHAR FAR *)query.GetData(), strlen(query.GetData()));
  if (retcode != SQL_SUCCESS) {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  retcode = SQLExecute(hStmt);

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return TRUE;
}

bool wxRecordSet::EndQuery(void)
{
  return TRUE;
}

void wxRecordSet::FillVars(int recnum) {
  wxNode* node = cols.First();
  
  do {
    ((wxQueryCol*)node->Data())->FillVar(recnum);
  } while ((node = node->Next()));
}

bool wxRecordSet::GetResultSet(void)
{
//  long trash = SQL_NULL_DATA; // value added by JACS
  long trash;
               // contains the number of bytes transferred by SQLFetch()
               // who needs this ?
  wxNode *currow, *fetch, *curcol;
  
  retcode = SQLNumResultCols(hStmt, &nCols);

  if (!nCols)
    return TRUE;
  
  // delete old data first
  cols.DeleteContents(TRUE);
  cols.Clear();
  fetchbuf.DeleteContents(TRUE);
  fetchbuf.Clear();
  
  nRecords = 0;
  cursor = 0;

  int i;
  for (i=0; i<nCols; i++) {
    char name[512];
    short type, scale, nullable, namelen;
    unsigned long len;
    
    retcode = SQLDescribeCol(hStmt, i+1, (unsigned char *)name, 511, &namelen, &type, &len, &scale, &nullable);
    if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }

    wxQueryCol *col1 = new wxQueryCol;
    curcol = cols.Append(name, col1);
    col1->SetName(name);
    col1->SetType(type);
    col1->SetNullable((nullable != 0));

    wxQueryField *field1 = new wxQueryField;
    fetch = fetchbuf.Append(field1);
    field1->SetType(type);
    field1->SetSize(len);
    
    SQLBindCol(hStmt, i+1, SQL_C_BINARY, (unsigned char*)field1->GetData(), field1->GetSize(), &trash);
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    // get it all !
    // After we've done an SQLFetch, copy the data in the fetch buffer into
    // new fields, for each column.
    while (SQL_SUCCESS == (retcode = SQLFetch(hStmt)) || SQL_SUCCESS_WITH_INFO == retcode) {
      nRecords++;
      
      curcol = cols.First();
      fetch = fetchbuf.First();
      for (i=0; i<nCols; i++) {

        wxQueryField *fetchField = (wxQueryField *)fetch->Data();
        wxQueryCol *col = (wxQueryCol *)curcol->Data();
        wxQueryField *field = new wxQueryField;

	currow = col->fields.Append(field);
	
	field->SetType(fetchField->GetType());
	field->SetData(fetchField->GetData(), fetchField->GetSize());
        curcol = curcol->Next();
        fetchField->ClearData(); // Runs ok if this commented out and SetData commented out
	fetch = fetch->Next();
      }
    }
    // while loop should only be left, when no more data was found;
    // otherwise it seems, that there was an error
    if (SQL_NO_DATA_FOUND != retcode) {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    break;
    case wxOPEN_TYPE_DYNASET:
    // get first record only
    if (SQL_SUCCESS == (retcode = SQLFetch(hStmt)) || retcode == SQL_SUCCESS_WITH_INFO) {
      nRecords = 1;  // TO DO! # of records in the ODBC result set should be set here.
      
      curcol = cols.First();
      fetch = fetchbuf.First();
      for (i=0; i<nCols; i++) {
	currow = ((wxQueryCol*)curcol->Data())->fields.Append(new wxQueryField);
	
	((wxQueryField*)currow->Data())->SetType(((wxQueryField*)fetch->Data())->GetType());
	((wxQueryField*)currow->Data())->SetData(((wxQueryField*)fetch->Data())->GetData(), ((wxQueryField*)fetch->Data())->GetSize());
        curcol = curcol->Next();
        ((wxQueryField*)fetch->Data())->ClearData();
	fetch = fetch->Next();
      }
    }
    if (SQL_NO_DATA_FOUND != retcode) {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    break;
    default:
    return FALSE;
  }

  FillVars(0);
  
  return TRUE;
}

bool wxRecordSet::ExecuteSQL(char *sql)
{
  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = NULL;
  }
  
  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  
  if (SQL_SUCCESS != retcode) {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  retcode = SQLExecDirect(hStmt, (UCHAR FAR*)sql, SQL_NTS);
  
  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return GetResultSet();
}

bool wxRecordSet::GetDataSources(void) {

  char *dsname = "Name", *dsdesc = "Description";
  char namebuf[64];
  char descbuf[512];
  short namelen, desclen;
  
  cursor = 0;
  
  // delete old data first
  cols.DeleteContents(TRUE);
  cols.Clear();
  nRecords = 0;

  // JACS This is a snapshot, not a dynaset.
  type = wxOPEN_TYPE_SNAPSHOT;
  
  wxNode *namecol, *desccol;
  
  namecol = cols.Append(dsname, new wxQueryCol);
  ((wxQueryCol*)namecol->Data())->SetName(dsname);
  ((wxQueryCol*)namecol->Data())->SetType(SQL_CHAR);
  desccol = cols.Append(dsdesc, new wxQueryCol);
  ((wxQueryCol*)desccol->Data())->SetName(dsdesc);
  ((wxQueryCol*)desccol->Data())->SetType(SQL_CHAR);
  
  retcode = SQLDataSources(parentdb->GetHENV(), SQL_FETCH_FIRST, (unsigned char *)namebuf, 63, &namelen, (unsigned char *)descbuf, 511, &desclen);
  while (SQL_SUCCESS == retcode || SQL_SUCCESS_WITH_INFO == retcode) {
    nRecords++;
    ((wxQueryCol*)namecol->Data())->AppendField(namebuf, namelen);
    ((wxQueryCol*)desccol->Data())->AppendField(descbuf, desclen);
    retcode = SQLDataSources(parentdb->GetHENV(), SQL_FETCH_NEXT, (unsigned char *)namebuf, 63, &namelen, (unsigned char *)descbuf, 511, &desclen);
  }

  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode && SQL_NO_DATA_FOUND != retcode)  {
    parentdb->ErrorSnapshot();
    return FALSE;
  }

  cursor = 0;
  
  return TRUE;
}

// Attributes
void wxRecordSet::SetTableName(char* name) {
  delete[] tablename;
  tablename = NULL;
  
  if (name)
    tablename = copystring(name);
}

bool wxRecordSet::GetTables(void)
{
  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
      
    hStmt = NULL;
  }
  
  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  
  if (SQL_SUCCESS != retcode) {
    parentdb->ErrorSnapshot();
    return FALSE;
  }

  retcode = SQLTables(hStmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
  
  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return GetResultSet();
}

bool wxRecordSet::GetColumns(char* table)
{
  char* name=NULL;
//  char* wildcard = "%";

  name = table ? table : tablename;
  
  if (!name)
    return FALSE;

  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = NULL;
  }
  
  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  
  if (SQL_SUCCESS != retcode) {
    parentdb->ErrorSnapshot();
    return FALSE;
  }

  //retcode = SQLColumns(hstmt, (unsigned char*)parentdb->GetDataSource(), strlen(parentdb->GetDataSource()), wildcard, 1, name, strlen(name), wildcard, 1);
  retcode = SQLColumns(hStmt, NULL, 0, NULL, 0, (unsigned char *)name, strlen(name), NULL, 0);
  
  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return GetResultSet();
}

// It is derived from previous GetColumns
bool wxRecordSet::GetPrimaryKeys(char* table)
{
  char* name=NULL;
//  char* wildcard = "%";

  name = table ? table : tablename;
  
  if (!name)
    return FALSE;

  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = NULL;
  }
  
  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  
  if (SQL_SUCCESS != retcode) {
    parentdb->ErrorSnapshot();
    return FALSE;
  }

  retcode = SQLPrimaryKeys(hStmt, NULL, 0, NULL, 0, (unsigned char *)name, SQL_NTS);  

  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return GetResultSet();
}

bool wxRecordSet::GetForeignKeys(char* PkTableName, char * FkTableName)
{
  char* Pkname=NULL;
  char* Fkname=NULL;
//  char* wildcard = "%";

// Try to disable situation: both PkTableName and FkTableName are NULL 
//   set Pkname from tablename
  if( !PkTableName && !FkTableName ) {
     Pkname = PkTableName ? PkTableName : tablename;
     Fkname = FkTableName ;
     if (!Pkname )
       return FALSE;
  } else {
     Pkname = PkTableName ;
     Fkname = FkTableName ;
  }

  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = NULL;
  }
  
  retcode = SQLAllocStmt(parentdb->GetHDBC(), &hStmt);
  
  if (SQL_SUCCESS != retcode) {
    parentdb->ErrorSnapshot();
    return FALSE;
  }

  retcode = SQLForeignKeys(hStmt, NULL, 0, NULL, 0, (unsigned char *)Pkname,
    (Pkname ? SQL_NTS : 0), NULL, 0, NULL, 0, (unsigned char *)Fkname ,(Fkname ?SQL_NTS : 0) );  

  if (SQL_SUCCESS != retcode && SQL_SUCCESS_WITH_INFO != retcode)  {
    parentdb->ErrorSnapshot(hStmt);
    return FALSE;
  }

  return GetResultSet();
}

long wxRecordSet::GetNumberRecords(void) 
{
  return nRecords;
}

long wxRecordSet::GetNumberCols(void)
{
  return nCols;
}

char* wxRecordSet::GetColName(int col)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return NULL;
    
  return ((wxQueryCol*)node->Data())->GetName();
}

short wxRecordSet::GetColType(int col)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return SQL_TYPE_NULL;
    
  return ((wxQueryCol*)node->Data())->GetType();
}

short wxRecordSet::GetColType(const char *col)
{
  wxNode* node = cols.Find(col);
    
  if (!node)
    return SQL_TYPE_NULL;
    
  return ((wxQueryCol*)node->Data())->GetType();
}

bool wxRecordSet::GetFieldData(int col, int type, void* data)
{
  wxNode* node = cols.Nth(col);

  if (!node)
    return FALSE;
  
  if (((wxQueryCol*)node->Data())->GetType() != type)
    return FALSE;

  void* src = ((wxQueryCol*)node->Data())->GetData(cursor);

  if (!src)
    return FALSE;
  
  memcpy(data, src, ((wxQueryCol*)node->Data())->GetSize(cursor));
  
  return TRUE;
}

bool wxRecordSet::GetFieldData(const char* name, int type, void *data)
{
  wxNode* node = cols.Find(name);

  if (!node)
    return FALSE;
  
  if (((wxQueryCol*)node->Data())->GetType() != type)
    return FALSE;

  void* src = ((wxQueryCol*)node->Data())->GetData(cursor);

  if (!src)
    return FALSE;
  
  memcpy(data, src, ((wxQueryCol*)node->Data())->GetSize(cursor));
  
  return TRUE;
}

void* wxRecordSet::GetFieldDataPtr(int col, int type)
{
  wxNode* node = cols.Nth(col);

  if (!node)
    return NULL;
  
  if (((wxQueryCol*)node->Data())->GetType() != type)
    return NULL;

  return ((wxQueryCol*)node->Data())->GetData(cursor);
}

void* wxRecordSet::GetFieldDataPtr(const char* name, int type)
{
  wxNode* node = cols.Find(name);

  if (!node)
    return NULL;
  
  if (((wxQueryCol*)node->Data())->GetType() != type)
    return NULL;

  return ((wxQueryCol*)node->Data())->GetData(cursor);
}

void* wxRecordSet::BindVar(int col, void* var, long size) {
  wxNode* node = cols.Nth(col);
  
  if (!node)
    return NULL;
  
  return ((wxQueryCol*)node->Data())->BindVar(var, size);
}

void* wxRecordSet::BindVar(const char* name, void* var, long size) {
  wxNode* node = cols.Find(name);
  
  if (!node)
    return NULL;
  
  return ((wxQueryCol*)node->Data())->BindVar(var, size);
}

void wxRecordSet::SetType(int typ) {
  type = typ;
}

int wxRecordSet::GetType(void) {
  return type;
}

void wxRecordSet::SetOptions(int opts) {
  options = opts;
}

int wxRecordSet::GetOptions(void) {
  return options;
}

bool wxRecordSet::CanAppend(void)
{
  return FALSE;
}

bool wxRecordSet::CanRestart(void)
{
  return FALSE;
}

bool wxRecordSet::CanScroll(void)
{
  return FALSE;
}

bool wxRecordSet::CanTransact(void)
{
  return FALSE;
}

bool wxRecordSet::CanUpdate(void)
{
  return FALSE;
}

long wxRecordSet::GetCurrentRecord(void)
{
  return -1L;
}

bool wxRecordSet::RecordCountFinal(void)
{
  return FALSE;
}

char* wxRecordSet::GetTableName(void)
{
  return tablename;
}

char *wxRecordSet::GetSQL(void)
{
  return NULL;
}

bool wxRecordSet::IsOpen(void)
{
  return parentdb->IsOpen();
}

bool wxRecordSet::IsBOF(void)
{
  return cursor < 0;
}

bool wxRecordSet::IsEOF(void)
{
  return cursor >= nRecords;
}

bool wxRecordSet::IsDeleted(void)
{
  return FALSE;
}

// Update operations
void wxRecordSet::AddNew(void)
{
}

bool wxRecordSet::Delete(void)
{
  return FALSE;
}

void wxRecordSet::Edit(void)
{
}

bool wxRecordSet::Update(void)
{
  return FALSE;
}

// Record navigation
bool wxRecordSet::Move(long rows)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor += (int)rows;
    if (cursor < 0) {
      cursor = -1;
      return FALSE;
    }
    if (cursor > nRecords-1) {
      cursor = nRecords;
      return FALSE;
    }
    return TRUE;
      
    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

bool wxRecordSet::GoTo(long row)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor = (int)row;
    if (cursor < 0) {
      cursor = -1;
      return FALSE;
    }
    if (cursor > nRecords-1) {
      cursor = nRecords;
      return FALSE;
    }
    return TRUE;
      
    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

bool wxRecordSet::MoveFirst(void)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor = 0;
    return TRUE;
    
    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

bool wxRecordSet::MoveLast(void)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor = nRecords-1;
    return TRUE;
    
    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

bool wxRecordSet::MoveNext(void)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor++;
    if (cursor >= nRecords) {
      cursor = nRecords;
      return FALSE;
    }
    return TRUE;
    
    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

bool wxRecordSet::MovePrev(void)
{
  if (!nRecords) {
    cursor = -1;
    return FALSE;
  }
  
  switch (type) {
    case wxOPEN_TYPE_SNAPSHOT:
    cursor--;
    if (cursor < 0) {
      cursor = 0;
      return FALSE;
    }
    return TRUE;

    case wxOPEN_TYPE_DYNASET:
    return FALSE;
    default:
    return FALSE;
  }
}

// Others
void wxRecordSet::Cancel(void)
{
}

bool wxRecordSet::IsFieldDirty(int col)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return FALSE;
    
  return ((wxQueryCol*)node->Data())->IsFieldDirty(cursor);
}

bool wxRecordSet::IsFieldDirty(const char* name)
{
  wxNode* node = cols.Find(name);
    
  if (!node)
    return FALSE;
    
  return ((wxQueryCol*)node->Data())->IsFieldDirty(cursor);
}

bool wxRecordSet::IsFieldNull(int col)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return TRUE;
    
  return NULL != ((wxQueryCol*)node->Data())->GetData(cursor);
}

bool wxRecordSet::IsFieldNull(const char* name)
{
  wxNode* node = cols.Find(name);
    
  if (!node)
    return TRUE;
    
  return NULL != ((wxQueryCol*)node->Data())->GetData(cursor);
}

bool wxRecordSet::IsColNullable(int col)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return FALSE;
    
  return ((wxQueryCol*)node->Data())->IsNullable();
}

bool wxRecordSet::IsColNullable(const char* name)
{
  wxNode* node = cols.Find(name);
    
  if (!node)
    return FALSE;
    
  return ((wxQueryCol*)node->Data())->IsNullable();
}

bool wxRecordSet::Requery(void)
{
  return FALSE;
}

void wxRecordSet::SetFieldDirty(int col, bool dirty)
{
  wxNode* node = cols.Nth(col);
    
  if (!node)
    return;
    
  ((wxQueryCol*)node->Data())->SetFieldDirty(cursor, dirty);
}

void wxRecordSet::SetFieldDirty(const char* name, bool dirty)
{
  wxNode* node = cols.Find(name);
    
  if (!node)
    return;
    
  ((wxQueryCol*)node->Data())->SetFieldDirty(cursor, dirty);
}

void wxRecordSet::SetFieldNull(void *WXUNUSED(p), bool WXUNUSED(isNull))
{
}
   
// Overridables
char *wxRecordSet::GetDefaultConnect(void)
{
  return NULL;
}

char *wxRecordSet::GetDefaultSQL(void)
{
  return NULL;
}

void wxRecordSet::SetDefaultSQL(char *s)
{
  delete[] defaultSQL;
  
  if (s)
    defaultSQL = copystring(s);
  else
    defaultSQL = NULL;
}

// Build SQL query from column specification
bool wxRecordSet::ConstructDefaultSQL(void)
{
//  if (queryCols.Number() == 0)
    return FALSE;
}

bool wxRecordSet::ReleaseHandle(void)
{
  if (hStmt)
  {
    retcode = SQLFreeStmt(hStmt, SQL_DROP);
    if (retcode == SQL_ERROR)
    {
      if (parentdb)
        parentdb->ErrorSnapshot(hStmt);
      return FALSE;
    }
    hStmt = 0;
  }
  return TRUE;
}

wxQueryCol::wxQueryCol(void) {
//  __type = wxTYPE_QUERYCOL;
  name = NULL;
  type = SQL_TYPE_NULL;
  nullable = FALSE;
  var = NULL;
  varsize = 0;
}

wxQueryCol::~wxQueryCol(void) {
  // delete all data
  fields.DeleteContents(TRUE);
  fields.Clear();

  if (name)
    delete[] name;
}

void wxQueryCol::SetName(char* n) {
  name = new char[strlen(n)+1];
  strcpy(name, n);
}

bool wxQueryCol::SetData(int row, void* buf, long len) {
  wxNode* node = fields.Nth(row);
  
  if (!node)
    return FALSE;
  
  return ((wxQueryField*)node->Data())->SetData(buf, len);
}

void wxQueryCol::SetFieldDirty(int row, bool dirty) {
  wxNode* node = fields.Nth(row);
  
  if (!node)
    return;
  
  ((wxQueryField*)node->Data())->SetDirty(dirty);
}

void wxQueryCol::AppendField(void* buf, long len) {
  wxNode* node = fields.Append(new wxQueryField);
  ((wxQueryField*)node->Data())->SetType(type);
  ((wxQueryField*)node->Data())->SetData(buf, len);
}

void wxQueryCol::SetType(short t) {
  type = t;
}

void* wxQueryCol::BindVar(void* v, long s) {
  void* oldvar = var;
  
  var = v;
  varsize = s;
  
  return oldvar;
}

void wxQueryCol::FillVar(int recnum) {
  if (!var)
    return;
  
  wxNode* node = fields.Nth(recnum);
  
  if (!node)
    return;

  long actsize = ((wxQueryField*)node->Data())->GetSize();
  if (actsize > varsize)
    actsize = varsize;

  memcpy(var, ((wxQueryField*)node->Data())->GetData(), actsize);
}

void wxQueryCol::SetNullable(bool n) {
  nullable = n;
}
  
char* wxQueryCol::GetName(void) {
  return name;
}

short wxQueryCol::GetType(void)  {
  return type;
}

bool wxQueryCol::IsNullable(void) {
  return nullable;
}


bool wxQueryCol::IsFieldDirty(int row) {
  wxNode* node = fields.Nth(row);
  
  if (!node)
    return FALSE;
  
  return ((wxQueryField*)node->Data())->IsDirty();
}

void* wxQueryCol::GetData(int row) {
  wxNode* node = fields.Nth(row);
  
  if (!node)
    return NULL;
  
  return ((wxQueryField*)node->Data())->GetData();
}

long wxQueryCol::GetSize(int row) {
  wxNode* node = fields.Nth(row);
  
  if (!node)
    return 0;
  
  return ((wxQueryField*)node->Data())->GetSize();
}

wxQueryField::wxQueryField(void) {
//  __type = wxTYPE_QUERYROW;
  data = NULL;
  type = SQL_TYPE_NULL;
  size = 0;
  dirty = FALSE;
}

wxQueryField::~wxQueryField(void) {
  switch (type)
  {
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_CHAR:
    case SQL_VARCHAR:
      if (data) // JACS
        delete[] (char*)data;
      break;
    case SQL_INTEGER:
      if (data) // JACS
        delete (long*)data;
      break;
    case SQL_SMALLINT:
      if (data)
        delete (short*)data;
      break;
    case SQL_FLOAT:
    case SQL_DOUBLE:
      if (data)
        delete (double*)data;
      break;
    case SQL_REAL:
      if (data)
        delete (float*)data;
      break;
    case SQL_TIME:
      if (data)
        delete (TIME_STRUCT *)data;
      break;
    case SQL_DATE:
      if (data)
        delete (DATE_STRUCT *)data;
      break;
    case SQL_TIMESTAMP:
      if (data)
        delete (TIMESTAMP_STRUCT *)data;
      break;
  }
}

bool wxQueryField::AllocData(void) {
  switch (type)
  {
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    case SQL_CHAR:
    case SQL_VARCHAR:
    {
      if (data) // JACS
        delete[] (char*)data;
      if ((data = new char[size+1]))
      {
        char *str = (char *)data;
        int i;
        for (i = 0; i < size; i++)
          str[i] = 0;
//      memset(data, 0, size+1);
      }
      break;
    }
    case SQL_INTEGER:
    {
      if (data) // JACS
        delete (long*)data;
      if ((data = new long))
        *(long*)data = 0L;
      break;
    }
    case SQL_SMALLINT:
    {
      if (data)
        delete (short*)data;
      if ((data = new short))
        *(short*)data = 0;
      break;
    }
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      if (data)
        delete (double*)data;
      if ((data = new double))
        *(double*)data = 0;
      break;
    }
    case SQL_REAL:
    {
      if (data)
        delete (float*)data;
      if ((data = new float))
        *(float*)data = (float)0;
      break;
    }
    case SQL_TIME:
    {
      if (data)
        delete (TIME_STRUCT *)data;
      data = new TIME_STRUCT;
      memset(data, 0, sizeof(TIME_STRUCT));
      break;
    }
    case SQL_DATE:
    {
      if (data)
        delete (DATE_STRUCT *)data;
      data = new DATE_STRUCT;
      memset(data, 0, sizeof(DATE_STRUCT));
      break;
    }
    case SQL_TIMESTAMP:
    {
      if (data)
        delete (TIMESTAMP_STRUCT *)data;
      data = new TIMESTAMP_STRUCT;
      memset(data, 0, sizeof(TIMESTAMP_STRUCT));
      break;
    }
    default:
      return FALSE;
  }
  
  return TRUE;
}

bool wxQueryField::SetData(void* d, long s) {
  size = s;
  if (AllocData() && d)
  {
//    memcpy(data, d, s);
    switch (type)
    {
      case SQL_NUMERIC:
      case SQL_DECIMAL:
      case SQL_CHAR:
      case SQL_VARCHAR:
      {
        char *str = (char *)data;
        int i;
        for (i = 0; i < size; i++)
          str[i] = 0;
          
        strncpy(str, (char *)d, (int)size);
        str[size] = 0;
        break;
      }
      case SQL_INTEGER:
      {
        *(long*)data = *((long *)d);
        break;
      }
      case SQL_SMALLINT:
      {
        *(short*)data = *((short*)d);
        break;
      }
      case SQL_FLOAT:
      case SQL_DOUBLE:
      {
        *(double*)data = *((double*)d);
        break;
      }
      case SQL_REAL:
      {
        *(float*)data = *((float*)d);
        break;
      }
      case SQL_TIME:
      {
        *(TIME_STRUCT *)data = *((TIME_STRUCT*)d);
        break;
      }
      case SQL_TIMESTAMP:
      {
        *(TIMESTAMP_STRUCT *)data = *((TIMESTAMP_STRUCT*)d);
        break;
      }
      case SQL_DATE:
      {
        *(DATE_STRUCT *)data = *((DATE_STRUCT*)d);
        break;
      }
      default:
        return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

void wxQueryField::ClearData(void) {
  if (data)
  {
  //    memset(data, 0, size);
    switch (type)
    {
      case SQL_NUMERIC:
      case SQL_DECIMAL:
      case SQL_CHAR:
      case SQL_VARCHAR:
      {
        char *str = (char *)data;
        int i;
        for (i = 0; i < size; i++)
          str[i] = 0;
        break;
      }
      case SQL_INTEGER:
      {
        *(long*)data = 0L;
        break;
      }
      case SQL_SMALLINT:
      {
        *(short*)data = 0;
        break;
      }
      case SQL_FLOAT:
      case SQL_DOUBLE:
      {
        *(double*)data = (double)0.0;
        break;
      }
      case SQL_REAL:
      {
        *(float*)data = (float)0.0;
        break;
      }
      case SQL_TIME:
      {
        memset(data, 0, sizeof(TIME_STRUCT));
        break;
      }
      case SQL_DATE:
      {
        memset(data, 0, sizeof(DATE_STRUCT));
        break;
      }
      case SQL_TIMESTAMP:
      {
        memset(data, 0, sizeof(TIMESTAMP_STRUCT));
        break;
      }
      default:
        return;
    }
  }
}

void wxQueryField::SetDirty(bool d) {
  dirty = d;
}

void wxQueryField::SetType(short t) {
  type = t;
}

void wxQueryField::SetSize(long s) {
  size = s;
  AllocData();
}

void* wxQueryField::GetData(void) {
  return data;
}

short wxQueryField::GetType(void)  {
  return type;
}

long wxQueryField::GetSize(void) {
  return size;
}

bool wxQueryField::IsDirty(void) {
  return dirty;
}

#endif // wxUSE_ODBC
