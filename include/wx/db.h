///////////////////////////////////////////////////////////////////////////////
// Name:        db.h
// Purpose:     Header file wxDB class.  The wxDB class represents a connection
//              to an ODBC data source.  The wxDB class allows operations on the data
//              source such as opening and closing the data source.
// Author:      Doug Card
// Modified by:
// Mods:        Dec, 1998: Added support for SQL statement logging and database
//              cataloging
// Created:     9.96
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence, plus:
// Notice:		This class library and its intellectual design are free of charge for use,
//              modification, enhancement, debugging under the following conditions:
//              1) These classes may only be used as part of the implementation of a
//                 wxWindows-based application
//              2) All enhancements and bug fixes are to be submitted back to the wxWindows
//                 user groups free of all charges for use with the wxWindows library.
//              3) These classes may not be distributed as part of any other class library,
//                 DLL, text (written or electronic), other than a complete distribution of
//                 the wxWindows GUI development toolkit.
//
///////////////////////////////////////////////////////////////////////////////

/*
// SYNOPSIS START
// SYNOPSIS STOP
*/

#ifndef DB_DOT_H
#define DB_DOT_H
 
#ifdef __GNUG__
#pragma interface "db.h"
#endif

#if defined(__WXMSW__) || defined(WIN32)
#include <windows.h>
#endif


#ifdef __WXGTK__

extern "C" {
#include <../iodbc/isql.h>
#include <../iodbc/isqlext.h>
typedef float         SFLOAT;
typedef double        SDOUBLE;
typedef unsigned int  UINT;
#define ULONG UDWORD

}

#else

#define ODBCVER 0x0250
#include <sql.h>
#include <sqlext.h>

#endif

enum		enumDummy		{enumDum1};

#define SQL_C_BOOLEAN (sizeof(int) == 2 ? SQL_C_USHORT : SQL_C_ULONG)
#define SQL_C_ENUM (sizeof(enumDummy) == 2 ? SQL_C_USHORT : SQL_C_ULONG)    //glt 2-21-97

// Database Globals
const int DB_TYPE_NAME_LEN						= 40;
const int DB_MAX_STATEMENT_LEN					= 2048;
const int DB_MAX_WHERE_CLAUSE_LEN				= 1024;
const int DB_MAX_ERROR_MSG_LEN					= 512;
const int DB_MAX_ERROR_HISTORY					= 5;
const int DB_MAX_TABLE_NAME_LEN				= 128;
const int DB_MAX_COLUMN_NAME_LEN				= 128;

const int DB_DATA_TYPE_VARCHAR					= 1;
const int DB_DATA_TYPE_INTEGER					= 2;
const int DB_DATA_TYPE_FLOAT					= 3;
const int DB_DATA_TYPE_DATE						= 4;

const int DB_SELECT_KEYFIELDS					= 1;
const int DB_SELECT_WHERE						= 2;
const int DB_SELECT_MATCHING					= 3;
const int DB_SELECT_STATEMENT					= 4;

const int DB_UPD_KEYFIELDS						= 1;
const int DB_UPD_WHERE							= 2;

const int DB_DEL_KEYFIELDS						= 1;
const int DB_DEL_WHERE							= 2;
const int DB_DEL_MATCHING						= 3;

const int DB_WHERE_KEYFIELDS					= 1;
const int DB_WHERE_MATCHING						= 2;

const int DB_CURSOR0								= 0;
const int DB_CURSOR1								= 1;
const int DB_CURSOR2								= 2;
//const int DB_CURSOR3							= 3;
//const int DB_CURSOR4							= 4;
//const int DB_CURSOR5							= 5;

const int DB_GRANT_SELECT						= 1;
const int DB_GRANT_INSERT						= 2;
const int DB_GRANT_UPDATE						= 4;
const int DB_GRANT_DELETE						= 8;
const int DB_GRANT_ALL							= DB_GRANT_SELECT | DB_GRANT_INSERT | DB_GRANT_UPDATE | DB_GRANT_DELETE;

// ODBC Error codes (derived from ODBC SqlState codes)
enum ODBC_ERRORS
{
	DB_FAILURE									= 0,
	DB_SUCCESS									= 1,
	DB_ERR_NOT_IN_USE,
	DB_ERR_GENERAL_WARNING,									// SqlState = '01000'
	DB_ERR_DISCONNECT_ERROR,								// SqlState = '01002'
	DB_ERR_DATA_TRUNCATED,									// SqlState = '01004'
	DB_ERR_PRIV_NOT_REVOKED,								// SqlState = '01006'
	DB_ERR_INVALID_CONN_STR_ATTR,							// SqlState = '01S00'
	DB_ERR_ERROR_IN_ROW,										// SqlState = '01S01'
	DB_ERR_OPTION_VALUE_CHANGED,							// SqlState = '01S02'
	DB_ERR_NO_ROWS_UPD_OR_DEL,								// SqlState = '01S03'
	DB_ERR_MULTI_ROWS_UPD_OR_DEL,							// SqlState = '01S04'
	DB_ERR_WRONG_NO_OF_PARAMS,								// SqlState = '07001'
	DB_ERR_DATA_TYPE_ATTR_VIOL,							// SqlState = '07006'
	DB_ERR_UNABLE_TO_CONNECT,								// SqlState = '08001'
	DB_ERR_CONNECTION_IN_USE,								// SqlState = '08002'
	DB_ERR_CONNECTION_NOT_OPEN,							// SqlState = '08003'
	DB_ERR_REJECTED_CONNECTION,							// SqlState = '08004'
	DB_ERR_CONN_FAIL_IN_TRANS,								// SqlState = '08007'
	DB_ERR_COMM_LINK_FAILURE,								// SqlState = '08S01'
	DB_ERR_INSERT_VALUE_LIST_MISMATCH,					// SqlState = '21S01'
	DB_ERR_DERIVED_TABLE_MISMATCH,						// SqlState = '21S02'
	DB_ERR_STRING_RIGHT_TRUNC,								// SqlState = '22001'
	DB_ERR_NUMERIC_VALUE_OUT_OF_RNG,						// SqlState = '22003'
	DB_ERR_ERROR_IN_ASSIGNMENT,							// SqlState = '22005'
	DB_ERR_DATETIME_FLD_OVERFLOW,							// SqlState = '22008'
	DB_ERR_DIVIDE_BY_ZERO,									// SqlState = '22012'
	DB_ERR_STR_DATA_LENGTH_MISMATCH,						// SqlState = '22026'
	DB_ERR_INTEGRITY_CONSTRAINT_VIOL,					// SqlState = '23000'
	DB_ERR_INVALID_CURSOR_STATE,							// SqlState = '24000'
	DB_ERR_INVALID_TRANS_STATE,							// SqlState = '25000'
	DB_ERR_INVALID_AUTH_SPEC,								// SqlState = '28000'
	DB_ERR_INVALID_CURSOR_NAME,							// SqlState = '34000'
	DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL,					// SqlState = '37000'
	DB_ERR_DUPLICATE_CURSOR_NAME,							// SqlState = '3C000'
	DB_ERR_SERIALIZATION_FAILURE,							// SqlState = '40001'
	DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL2,				// SqlState = '42000'
	DB_ERR_OPERATION_ABORTED,								// SqlState = '70100'
	DB_ERR_UNSUPPORTED_FUNCTION,							// SqlState = 'IM001'
	DB_ERR_NO_DATA_SOURCE,									// SqlState = 'IM002'
	DB_ERR_DRIVER_LOAD_ERROR,								// SqlState = 'IM003'
	DB_ERR_SQLALLOCENV_FAILED,								// SqlState = 'IM004'
	DB_ERR_SQLALLOCCONNECT_FAILED,						// SqlState = 'IM005'
	DB_ERR_SQLSETCONNECTOPTION_FAILED,					// SqlState = 'IM006'
	DB_ERR_NO_DATA_SOURCE_DLG_PROHIB,					// SqlState = 'IM007'
	DB_ERR_DIALOG_FAILED,									// SqlState = 'IM008'
	DB_ERR_UNABLE_TO_LOAD_TRANSLATION_DLL,				// SqlState = 'IM009'
	DB_ERR_DATA_SOURCE_NAME_TOO_LONG,					// SqlState = 'IM010'
	DB_ERR_DRIVER_NAME_TOO_LONG,							// SqlState = 'IM011'
	DB_ERR_DRIVER_KEYWORD_SYNTAX_ERROR,					// SqlState = 'IM012'
	DB_ERR_TRACE_FILE_ERROR,								// SqlState = 'IM013'
	DB_ERR_TABLE_OR_VIEW_ALREADY_EXISTS,				// SqlState = 'S0001'
	DB_ERR_TABLE_NOT_FOUND,									// SqlState = 'S0002'
	DB_ERR_INDEX_ALREADY_EXISTS,							// SqlState = 'S0011'
	DB_ERR_INDEX_NOT_FOUND,									// SqlState = 'S0012'
	DB_ERR_COLUMN_ALREADY_EXISTS,							// SqlState = 'S0021'
	DB_ERR_COLUMN_NOT_FOUND,								// SqlState = 'S0022'
	DB_ERR_NO_DEFAULT_FOR_COLUMN,							// SqlState = 'S0023'
	DB_ERR_GENERAL_ERROR,									// SqlState = 'S1000'
	DB_ERR_MEMORY_ALLOCATION_FAILURE,					// SqlState = 'S1001'
	DB_ERR_INVALID_COLUMN_NUMBER,							// SqlState = 'S1002'
	DB_ERR_PROGRAM_TYPE_OUT_OF_RANGE,					// SqlState = 'S1003'
	DB_ERR_SQL_DATA_TYPE_OUT_OF_RANGE,					// SqlState = 'S1004'
	DB_ERR_OPERATION_CANCELLED,							// SqlState = 'S1008'
	DB_ERR_INVALID_ARGUMENT_VALUE,						// SqlState = 'S1009'
	DB_ERR_FUNCTION_SEQUENCE_ERROR,						// SqlState = 'S1010'
	DB_ERR_OPERATION_INVALID_AT_THIS_TIME,				// SqlState = 'S1011'
	DB_ERR_INVALID_TRANS_OPERATION_CODE,				// SqlState = 'S1012'
	DB_ERR_NO_CURSOR_NAME_AVAIL,							// SqlState = 'S1015'
	DB_ERR_INVALID_STR_OR_BUF_LEN,						// SqlState = 'S1090'
	DB_ERR_DESCRIPTOR_TYPE_OUT_OF_RANGE,				// SqlState = 'S1091'
	DB_ERR_OPTION_TYPE_OUT_OF_RANGE,						// SqlState = 'S1092'
	DB_ERR_INVALID_PARAM_NO,								// SqlState = 'S1093'
	DB_ERR_INVALID_SCALE_VALUE,							// SqlState = 'S1094'
	DB_ERR_FUNCTION_TYPE_OUT_OF_RANGE,					// SqlState = 'S1095'
	DB_ERR_INF_TYPE_OUT_OF_RANGE,							// SqlState = 'S1096'
	DB_ERR_COLUMN_TYPE_OUT_OF_RANGE,						// SqlState = 'S1097'
	DB_ERR_SCOPE_TYPE_OUT_OF_RANGE,						// SqlState = 'S1098'
	DB_ERR_NULLABLE_TYPE_OUT_OF_RANGE,					// SqlState = 'S1099'
	DB_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE,		// SqlState = 'S1100'
	DB_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE,			// SqlState = 'S1101'
	DB_ERR_DIRECTION_OPTION_OUT_OF_RANGE,				// SqlState = 'S1103'
	DB_ERR_INVALID_PRECISION_VALUE,						// SqlState = 'S1104'
	DB_ERR_INVALID_PARAM_TYPE,								// SqlState = 'S1105'
	DB_ERR_FETCH_TYPE_OUT_OF_RANGE,						// SqlState = 'S1106'
	DB_ERR_ROW_VALUE_OUT_OF_RANGE,						// SqlState = 'S1107'
	DB_ERR_CONCURRENCY_OPTION_OUT_OF_RANGE,			// SqlState = 'S1108'
	DB_ERR_INVALID_CURSOR_POSITION,						// SqlState = 'S1109'
	DB_ERR_INVALID_DRIVER_COMPLETION,					// SqlState = 'S1110'
	DB_ERR_INVALID_BOOKMARK_VALUE,						// SqlState = 'S1111'
	DB_ERR_DRIVER_NOT_CAPABLE,								// SqlState = 'S1C00'
	DB_ERR_TIMEOUT_EXPIRED									// SqlState = 'S1T00'
};

struct DbStuff
{
	HENV Henv;
	char Dsn[SQL_MAX_DSN_LENGTH+1];	// Data Source Name
	char Uid[20];							// User ID
	char AuthStr[20];						// Authorization string (password)
};

typedef struct
{
	char	TypeName[DB_TYPE_NAME_LEN];
	int	FsqlType;
	long	Precision;
	short	CaseSensitive;
//	short MinimumScale;
	short MaximumScale;
} SqlTypeInfo;

class WXDLLEXPORT CcolInf
{
public:
	char tableName[DB_MAX_TABLE_NAME_LEN+1];
	char colName[DB_MAX_COLUMN_NAME_LEN+1];
	int  sqlDataType;
};

enum sqlLog
{
	sqlLogOFF,
	sqlLogON
};

class WXDLLEXPORT wxDB
{
private:

	// Private data
	bool			dbIsOpen;
	char			*dsn;				// Data source name
	char			*uid;				// User ID
	char			*authStr;		// Authorization string (password)
	FILE			*fpSqlLog;		// Sql Log file pointer
	enum sqlLog sqlLogState;	// On or Off

	// Private member functions
	bool getDbInfo(void);
	bool getDataTypeInfo(SWORD fSqlType, SqlTypeInfo &structSQLTypeInfo);
	bool setConnectionOptions(void);
	void logError(char *errMsg, char *SQLState);

public:

	// The following structure contains database information gathered from the
	// datasource when the datasource is first opened.
	struct
	{
		char   dbmsName[40];								// Name of the dbms product
		char   dbmsVer[40];								// Version # of the dbms product
		char   driverName[40];							// Driver name
		char   odbcVer[60];								// ODBC version of the driver
		char   drvMgrOdbcVer[60];						// ODBC version of the driver manager
		char   driverVer[60];							// Driver version
		char   serverName[80];							// Server Name, typically a connect string
		char	 databaseName[128];						// Database filename
		char   outerJoins[2];							// Indicates whether the data source supports outer joins
		char	 procedureSupport[2];					// Indicates whether the data source supports stored procedures
		UWORD  maxConnections;							// Maximum # of connections the data source supports
		UWORD  maxStmts;									// Maximum # of HSTMTs per HDBC
		UWORD  apiConfLvl;								// ODBC API conformance level
		UWORD  cliConfLvl;								// Indicates whether the data source is SAG compliant
		UWORD  sqlConfLvl;								// SQL conformance level
		UWORD  cursorCommitBehavior;					// Indicates how cursors are affected by a db commit
		UWORD  cursorRollbackBehavior;				// Indicates how cursors are affected by a db rollback
		UWORD  supportNotNullClause;					// Indicates if data source supports NOT NULL clause
		char   supportIEF[2];							// Integrity Enhancement Facility (Referential Integrity)
		UDWORD txnIsolation;								// Default transaction isolation level supported by the driver
		UDWORD txnIsolationOptions;					// Transaction isolation level options available
		UDWORD fetchDirections;							// Fetch directions supported
		UDWORD lockTypes;									// Lock types supported in SQLSetPos
		UDWORD posOperations;							// Position operations supported in SQLSetPos
		UDWORD posStmts;									// Position statements supported
		UDWORD scrollConcurrency;						// Concurrency control options supported for scrollable cursors
		UDWORD scrollOptions;							// Scroll Options supported for scrollable cursors
		UDWORD staticSensitivity;						// Indicates if additions, deletions and updates can be detected
		UWORD  txnCapable;								// Indicates if the data source supports transactions
		UDWORD loginTimeout;								// Number seconds to wait for a login request
	} dbInf;

	// ODBC handles
	HENV  henv;		// ODBC Environment handle
	HDBC  hdbc;		// ODBC DB Connection handle
	HSTMT hstmt;	// ODBC Statement handle

	// ODBC Error Inf.
	char   sqlState[20];
	SDWORD nativeError;
	char   errorMsg[SQL_MAX_MESSAGE_LENGTH];
	SWORD  cbErrorMsg;
	char   errorList[DB_MAX_ERROR_HISTORY][DB_MAX_ERROR_MSG_LEN];
	int    DB_STATUS;

	//Error reporting mode
	bool silent;

	// Inf. about logical data types VARCHAR, INTEGER, FLOAT and DATE.
	// This inf. is obtained from the ODBC driver by use of the
	// SQLGetTypeInfo() function.  The key piece of inf. is the
	// type name the data source uses for each logical data type.
	// e.g. VARCHAR; Oracle calls it VARCHAR2.
	SqlTypeInfo typeInfVarchar, typeInfInteger, typeInfFloat, typeInfDate;
	
	// Public member functions
	wxDB(HENV &aHenv);
	bool		 Open(char *Dsn, char *Uid, char *AuthStr);  // Data Source Name, User ID, Password
	void		 Close(void);
	bool		 CommitTrans(void);
	bool		 RollbackTrans(void);
	bool		 DispAllErrors(HENV aHenv, HDBC aHdbc = SQL_NULL_HDBC, HSTMT aHstmt = SQL_NULL_HSTMT);
	bool		 GetNextError(HENV aHenv, HDBC aHdbc = SQL_NULL_HDBC, HSTMT aHstmt = SQL_NULL_HSTMT);
	void		 DispNextError(void);
	bool		 CreateView(char *viewName, char *colList, char *pSqlStmt);
	bool		 ExecSql(char *pSqlStmt);
	bool	    Grant(int privileges, char *tableName, char *userList = "PUBLIC");
	int	    TranslateSqlState(char *SQLState);
	bool		 Catalog(char *userID, char *fileName = "Catalog.txt");
	CcolInf	*GetColumns(char *tableName[]);
	char		*GetDatabaseName(void)	{return dbInf.dbmsName;}
	char		*GetDataSource(void)		{return dsn;}
	char		*GetUsername(void)		{return uid;}
	char		*GetPassword(void)		{return authStr;}
	bool		 IsOpen(void)				{return dbIsOpen;}
	HENV		 GetHENV(void)				{return henv;}
	HDBC		 GetHDBC(void)				{return hdbc;}
	HSTMT		 GetHSTMT(void)			{return hstmt;}
	bool		 TableExists(char *tableName);  // Table name can refer to a table, view, alias or synonym
	void		 LogError(char *errMsg, char *SQLState = 0) {logError(errMsg, SQLState);}
	bool		 SqlLog(enum sqlLog state, char *filename = "sqllog.txt", bool append = FALSE);
	bool		 WriteSqlLog(char *logMsg);

};  // wxDB

// This structure forms a node in a linked list.  The linked list of "DbList" objects
// keeps track of allocated database connections.  This allows the application to
// open more than one database connection through ODBC for multiple transaction support
// or for multiple database support.

struct DbList
{
	DbList *PtrPrev;							// Pointer to previous item in the list
	char    Dsn[SQL_MAX_DSN_LENGTH+1];	// Data Source Name
	wxDB	 *PtrDb;								// Pointer to the wxDB object
	bool    Free;								// Is item free or in use?
	DbList *PtrNext;							// Pointer to next item in the list
};

// The following routines allow a user to get new database connections, free them
// for other code segments to use, or close all of them when the application has
// completed.

wxDB* WXDLLEXPORT GetDbConnection(DbStuff *pDbStuff);
bool  WXDLLEXPORT FreeDbConnection(wxDB *pDb);
void  WXDLLEXPORT CloseDbConnections(void);
int   WXDLLEXPORT NumberDbConnectionsInUse(void);

// This routine allows you to query a driver manager
// for a list of available datasources.  Call this routine
// the first time using SQL_FETCH_FIRST.  Continue to call it
// using SQL_FETCH_NEXT until you've exhausted the list.
bool WXDLLEXPORT GetDataSource(HENV henv, char *Dsn, SWORD DsnMax, char *DsDesc, SWORD DsDescMax,
						 UWORD direction = SQL_FETCH_NEXT);

#endif
