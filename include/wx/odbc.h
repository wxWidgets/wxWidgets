/////////////////////////////////////////////////////////////////////////////
// Name:        odbc.h
// Purpose:     ODBC classes
// Author:      Olaf Klein, Patrick Halke, Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"

#if wxUSE_ODBC

#ifdef __GNUG__
#pragma interface "odbc.h"
#endif

#ifndef _WX_ODBCH__
#define _WX_ODBCH__

#ifdef __WXMSW__
#include <windows.h>
#endif

#ifdef __WXGTK__
extern "C" {
#include <../iodbc/postgres/isqlext.h>
#include <../iodbc/postgres/odbc_funcs.h>
#include <../iodbc/postgres/odbc_types.h>
}
#else
#include <sqlext.h>
#endif

#include "wx/defs.h"
#include "wx/list.h"
#include "wx/string.h"

typedef RETCODE wxRETCODE;

// Recordset open types
#define wxOPEN_TYPE_DYNASET         1
#define wxOPEN_TYPE_SNAPSHOT        2
#define wxOPEN_TYPE_FORWARD_ONLY    3

// Recordset open options
#define wxOPTION_DEFAULT            1
#define wxOPTION_APPEND_ONLY        2
#define wxOPTION_READ_ONLY          3

// Data types
class WXDLLEXPORT wxRecordSet;

class WXDLLEXPORT wxDatabase: public wxObject
{
  // JACS
  DECLARE_DYNAMIC_CLASS(wxDatabase)
 private:
 protected:
  static HENV hEnv;
  static int refCount;
  
  HDBC hDBC;
  char* username;
  char* password;
  char* datasource;
  char* dbname;
  char* connectstring;
  bool isOpen;

  // error-handling variables
  wxRETCODE retcode;
  char sqlstate[SQL_SQLSTATE_SIZE+1];  // error class and subclass
  char errmsg[SQL_MAX_MESSAGE_LENGTH]; // error message
  long nat_err;                        // error number by ODBC driver
  bool err_occured;

  wxList recordSets; // Record sets: Added by JACS

 public:
  wxDatabase(void);
  ~wxDatabase(void);
  
  bool Open(char *, bool exclusive =FALSE, bool readOnly =TRUE, char *username ="ODBC", char *password ="");
  bool Close(void);

  // Cleanup operations, added by JACS
  void DeleteRecordSets(void); // Called when the database is deleted
  void ResetRecordSets(void); // Required if the database is closed
  inline wxList& GetRecordSets(void) { return recordSets; }
  
  inline char *GetUsername(void) { return username; }
  inline char *GetPassword(void) { return password; }
  inline char *GetDataSource(void) { return datasource; }
  inline bool IsOpen(void) { return isOpen; }
  inline wxRETCODE GetErrorCode(void) { return retcode; }
  inline HDBC GetHDBC(void) { return hDBC; }
  inline HENV GetHENV(void) { return hEnv; }
  
  void SetPassword(char *s);
  void SetUsername(char *s);
  void SetDataSource(char *s);
  
  // Database attributes
  char *GetDatabaseName(void);
  bool CanUpdate(void);
  bool CanTransact(void);
  bool InWaitForDataSource(void);
  void SetLoginTimeout(long seconds);
  void SetQueryTimeout(long seconds);
  void SetSynchronousMode(bool synchronous);

  // Database operations
  bool BeginTrans(void);
  bool CommitTrans(void);
  bool RollbackTrans(void);
  void Cancel(void);

  // Error handling
  bool ErrorOccured(void);
  char* GetErrorMessage(void);
  long  GetErrorNumber(void);
  char* GetErrorClass(void);
  inline void ErrorSnapshot(HSTMT =SQL_NULL_HSTMT);

  // Overridables
  virtual void OnSetOptions(wxRecordSet *recordSet);
  virtual void OnWaitForDataSource(bool stillExecuting);

  bool GetInfo(long infoType, long *buf);
  bool GetInfo(long infoType, char *buf, int bufSize = -1);

  // implementation = TRUE means get the DLL version.
  // Otherwise, returns header file version.
  wxString GetODBCVersionString(bool implementation = TRUE);
  float GetODBCVersionFloat(bool implementation = TRUE);
};

// Represents a data row
class WXDLLEXPORT wxQueryField: public wxObject
{
  // JACS
  DECLARE_DYNAMIC_CLASS(wxQueryField)
 private:
  void *data;
  short type;
  long size;
  bool dirty;

  bool AllocData(void);

  public:
  wxQueryField(void);
  ~wxQueryField(void);
  
  bool SetData(void*, long);
  void SetDirty(bool =TRUE);
  void ClearData(void);
  void SetType(short);
  void SetSize(long);
  
  void* GetData(void);
  short GetType(void);
  long GetSize(void);
  
  bool IsDirty(void);
};

// Represents a column description
class WXDLLEXPORT wxQueryCol: public wxObject
{
  // JACS
  DECLARE_DYNAMIC_CLASS(wxQueryCol)
 private:
  short type;
  char *name;
  bool nullable;
  long varsize;
  void* var;
  
  public:
  wxList fields;
  
  wxQueryCol(void);
  ~wxQueryCol(void);
  
  void* BindVar(void*, long);
  void FillVar(int);
  void AppendField(void*, long);
  bool SetData(int, void*, long);
  void SetName(char*);
  void SetNullable(bool);
  void SetFieldDirty(int, bool =TRUE);
  void SetType(short);
  
  char* GetName(void);
  short GetType(void);
  bool IsNullable(void);
  void* GetData(int);
  long GetSize(int);

  bool IsFieldDirty(int);
};

class WXDLLEXPORT wxRecordSet: public wxObject
{
  // JACS
  DECLARE_DYNAMIC_CLASS(wxRecordSet)
 private:
  int cursor;
  int type;
  int options;
  
  protected:
  HSTMT hStmt;
  int nFields;
  int nParams;
  int nRecords;
  short nCols;
  char *recordFilter;
  char *sortString;
  char *defaultSQL;
  char* tablename;
  wxDatabase *parentdb;
  wxRETCODE retcode;
  wxList cols;
  wxList fetchbuf;
  
  void FillVars(int);

  public:
  // JACS gave parent a default value for benefit of IMPLEMENT_DYNAMIC_CLASS
  wxRecordSet(wxDatabase *parent = NULL, int =wxOPEN_TYPE_DYNASET, int =wxOPTION_DEFAULT);
  ~wxRecordSet(void);
  
  // My own, lower-level functions.
  bool BeginQuery(int openType, char *sql = NULL, int options = wxOPTION_DEFAULT);
  bool EndQuery(void);
  bool Query(char* columns, char* table =NULL, char *filter =NULL);

  // Attributes
  inline int GetNumberFields(void) { return nFields; }
  inline int GetNumberParams(void) { return nParams; }
  long GetNumberRecords(void);
  long GetNumberCols(void);
  inline char *GetFilter(void) { return recordFilter; }
  inline char *GetSortString(void) { return sortString; }
  inline wxDatabase *GetDatabase(void) { return parentdb; }
  inline wxRETCODE GetErrorCode(void) { return retcode; }
  bool CanAppend(void);
  bool CanRestart(void);
  bool CanScroll(void);
  bool CanTransact(void);
  bool CanUpdate(void);
  long GetCurrentRecord(void);
  bool RecordCountFinal(void);
  bool GetResultSet(void);
  bool ExecuteSQL(char*);
  bool GetTables(void);
  bool GetColumns(char* =NULL);
  bool GetPrimaryKeys(char* =NULL);
  bool GetForeignKeys(char* , char * );
  char *GetTableName(void);
  void SetTableName(char*);
  char *GetSQL(void);
  bool IsOpen(void);
  bool IsBOF(void);
  bool IsEOF(void);
  bool IsDeleted(void);

  bool GetFieldData(int colPos, int dataType, void *dataPtr);
  bool GetFieldData(const char*, int dataType, void *dataPtr);
  void* GetFieldDataPtr(int, int);
  void* GetFieldDataPtr(const char*, int);
  char* GetColName(int);
  short GetColType(int);
  short GetColType(const char*);
  void* BindVar(int, void*, long);
  void* BindVar(const char*, void*, long);

  void SetType(int);
  int GetType(void);
  void SetOptions(int);
  int GetOptions(void);
    
  // Update operations
  void AddNew(void);
  bool Delete(void);
  void Edit(void);
  bool Update(void);

  // Record navigation
  virtual bool Move(long rows);
  virtual bool MoveFirst(void);
  virtual bool MoveLast(void);
  virtual bool MoveNext(void);
  virtual bool MovePrev(void);
  virtual bool GoTo(long);

  // Others
  bool GetDataSources(void);

  // Associate a column name/position with a data location
  //   bool BindColumn(int colPos, int dataType, void *dataPtr);

  void Cancel(void);
  bool IsFieldDirty(int);
  bool IsFieldDirty(const char*);
  bool IsFieldNull(int);
  bool IsFieldNull(const char*);
  bool IsColNullable(int);
  bool IsColNullable(const char*);
  virtual bool Requery(void);
  virtual void SetFieldDirty(int, bool dirty = TRUE);
  virtual void SetFieldDirty(const char*, bool dirty = TRUE);
  void SetFieldNull(void *p, bool isNull = TRUE);

  // Overridables
  virtual char *GetDefaultConnect(void);
  virtual char *GetDefaultSQL(void);
  
  // Internal
  
  // Build SQL query from column specification
  bool ConstructDefaultSQL(void);
  void SetDefaultSQL(char *s);
  bool ReleaseHandle(void); // Added JACS
};

#endif

#endif  // wxUSE_ODBC
