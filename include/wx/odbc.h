/////////////////////////////////////////////////////////////////////////////
// Name:        odbc.h
// Purpose:     ODBC classes
// Author:      Olaf Klein, Patrick Halke, Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"

#if wxUSE_ODBC

#ifdef __GNUG__
#pragma interface "odbc.h"
#endif

#ifndef _WX_ODBCH__
#define _WX_ODBCH__

//#ifdef __WXMSW__
//#include <windows.h>
//#endif


#ifdef __UNIX__
extern "C"
{
    #include "../../src/iodbc/isql.h"
    #include "../../src/iodbc/isqlext.h"
    #include "../../src/iodbc/odbc_funcs.h"
    #include "../../src/iodbc/odbc_types.h"

    typedef float   SFLOAT;
    typedef double  SDOUBLE;

    #define ULONG UDWORD
}
#else // !Unix
    #include <sqlext.h>
#endif // Unix/!Unix

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
  DECLARE_DYNAMIC_CLASS(wxDatabase)
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
  wxDatabase();
  ~wxDatabase();

  bool Open(char *, bool exclusive =FALSE, bool readOnly =TRUE, char *username ="ODBC", char *password ="");
  bool Close();

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
  char *GetDatabaseName();
  bool CanUpdate();
  bool CanTransact();
  bool InWaitForDataSource();
  void SetLoginTimeout(long seconds);
  void SetQueryTimeout(long seconds);
  void SetSynchronousMode(bool synchronous);

  // Database operations
  bool BeginTrans();
  bool CommitTrans();
  bool RollbackTrans();
  void Cancel();

  // Error handling
  bool ErrorOccured();
  char* GetErrorMessage();
  long  GetErrorNumber();
  char* GetErrorClass();
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

  bool AllocData();

  public:
  wxQueryField();
  ~wxQueryField();

  bool SetData(void*, long);
  void SetDirty(bool =TRUE);
  void ClearData();
  void SetType(short);
  void SetSize(long);

  void* GetData();
  short GetType();
  long GetSize();

  bool IsDirty();
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

  wxQueryCol();
  ~wxQueryCol();

  void* BindVar(void*, long);
  void FillVar(int);
  void AppendField(void*, long);
  bool SetData(int, void*, long);
  void SetName(char*);
  void SetNullable(bool);
  void SetFieldDirty(int, bool =TRUE);
  void SetType(short);

  char* GetName();
  short GetType();
  bool IsNullable();
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
  wxRecordSet(wxDatabase *parent = NULL,
              int = wxOPEN_TYPE_DYNASET,
              int = wxOPTION_DEFAULT);
  ~wxRecordSet();

  // My own, lower-level functions.
  bool BeginQuery(int openType, char *sql = NULL, int options = wxOPTION_DEFAULT);
  bool EndQuery();
  bool Query(char* columns, char* table =NULL, char *filter =NULL);

  // Attributes
  inline int GetNumberFields(void) { return nFields; }
  inline int GetNumberParams(void) { return nParams; }
  long GetNumberRecords();
  long GetNumberCols();
  inline char *GetFilter(void) { return recordFilter; }
  inline char *GetSortString(void) { return sortString; }
  inline wxDatabase *GetDatabase(void) { return parentdb; }
  inline wxRETCODE GetErrorCode(void) { return retcode; }
  bool CanAppend();
  bool CanRestart();
  bool CanScroll();
  bool CanTransact();
  bool CanUpdate();
  long GetCurrentRecord();
  bool RecordCountFinal();
  bool GetResultSet();
  bool ExecuteSQL(char*);
  bool GetTables();
  bool GetColumns(char* =NULL);
  bool GetPrimaryKeys(char* =NULL);
  bool GetForeignKeys(char* , char * );
  char *GetTableName();
  void SetTableName(char*);
  char *GetSQL();
  bool IsOpen();
  bool IsBOF();
  bool IsEOF();
  bool IsDeleted();

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
  int GetType();
  void SetOptions(int);
  int GetOptions();

  // Update operations
  void AddNew();
  bool Delete();
  void Edit();
  bool Update();

  // Record navigation
  virtual bool Move(long rows);
  virtual bool MoveFirst();
  virtual bool MoveLast();
  virtual bool MoveNext();
  virtual bool MovePrev();
  virtual bool GoTo(long);

  // Others
  bool GetDataSources();

  // Associate a column name/position with a data location
  //   bool BindColumn(int colPos, int dataType, void *dataPtr);

  void Cancel();
  bool IsFieldDirty(int);
  bool IsFieldDirty(const char*);
  bool IsFieldNull(int);
  bool IsFieldNull(const char*);
  bool IsColNullable(int);
  bool IsColNullable(const char*);
  virtual bool Requery();
  virtual void SetFieldDirty(int, bool dirty = TRUE);
  virtual void SetFieldDirty(const char*, bool dirty = TRUE);
  void SetFieldNull(void *p, bool isNull = TRUE);

  // Overridables
  virtual char *GetDefaultConnect();
  virtual char *GetDefaultSQL();

  // Internal

  // Build SQL query from column specification
  bool ConstructDefaultSQL();
  void SetDefaultSQL(char *s);
  bool ReleaseHandle(void); // Added JACS
};

#endif

#endif  // wxUSE_ODBC
