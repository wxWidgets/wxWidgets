///////////////////////////////////////////////////////////////////////////////
// Name:        db.h
// Purpose:     Header file wxDb class.  The wxDb class represents a connection
//              to an ODBC data source.  The wxDb class allows operations on the data
//              source such as opening and closing the data source.
// Author:      Doug Card
// Modified by: George Tasker
//              Bart Jourquin
//              Mark Johnson, wxWindows@mj10777.de
// Mods:        Dec, 1998:
//                -Added support for SQL statement logging and database cataloging
//                     April, 1999
//                -Added QUERY_ONLY mode support to reduce default number of cursors
//                -Added additional SQL logging code
//                -Added DEBUG-ONLY tracking of Ctable objects to detect orphaned DB connections
//                -Set ODBC option to only read committed writes to the DB so all
//                     databases operate the same in that respect
//
// Created:     9.96
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence, plus:
// Notice:      This class library and its intellectual design are free of charge for use,
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


// BJO 20000503: introduce new GetColumns members which are more database independant and 
//               return columns in the order they were created
#define OLD_GETCOLUMNS 1
#define EXPERIMENTAL_WXDB_FUNCTIONS 1

// Use this line for wxWindows v1.x
//#include "wx_ver.h"
// Use this line for wxWindows v2.x
#include "wx/version.h"

#if wxMAJOR_VERSION == 2
    #ifdef __GNUG__
        #pragma interface "db.h"
    #endif
#endif

#include "wx/setup.h"

#if wxMAJOR_VERSION == 2
    extern "C" {
#ifdef __VISUALC__
// If you use the wxDbCreateDataSource() function with MSW/VC6,
// you cannot use the iODBC headers, you must use the VC headers,
// plus the odbcinst.h header - gt Nov 2 2000
//
//  Must add "odbccp32.lib" in \wx2\wxWindows\src\makevc.env to the WINLIBS= line
//
    #include "sql.h"
    #include "sqlext.h"
    #include "odbcinst.h"
#else
    #include "wx/isql.h"
    #include "wx/isqlext.h"
#endif
    }
#else  // version == 1
    extern "C" {
#ifdef __VISUALC__
// If you use the wxDbCreateDataSource() function with MSW/VC6,
// you cannot use the iODBC headers, you must use the VC headers,
// plus the odbcinst.h header - gt Nov 2 2000
    #include "sql.h"
    #include "sqlext.h"
    #include "odbcinst.h"
#else
    #include "iodbc.h"
    #include "isqlext.h"
#endif
    }
#endif

typedef float SFLOAT;
typedef double SDOUBLE;
typedef unsigned int UINT;
#define ULONG UDWORD

#ifndef wxODBC_FWD_ONLY_CURSORS
#define wxODBC_FWD_ONLY_CURSORS 1
#endif

enum enumDummy {enumDum1};

#define SQL_C_BOOLEAN(datatype) (sizeof(datatype) == 1 ? SQL_C_UTINYINT : (sizeof(datatype) == 2 ? SQL_C_USHORT : SQL_C_ULONG))
//    #define SQL_C_BOOLEAN (sizeof(Bool) == 2 ? SQL_C_USHORT : SQL_C_ULONG)

#define SQL_C_ENUM (sizeof(enumDummy) == 2 ? SQL_C_USHORT : SQL_C_ULONG)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

const int wxDB_PATH_MAX                 = 254;

extern wxChar const *SQL_LOG_FILENAME;
extern wxChar const *SQL_CATALOG_FILENAME;


// Database Globals
const int DB_TYPE_NAME_LEN            = 40;
const int DB_MAX_STATEMENT_LEN        = 4096;
const int DB_MAX_WHERE_CLAUSE_LEN     = 2048;
const int DB_MAX_ERROR_MSG_LEN        = 512;
const int DB_MAX_ERROR_HISTORY        = 5;
const int DB_MAX_TABLE_NAME_LEN       = 128;
const int DB_MAX_COLUMN_NAME_LEN      = 128;

const int DB_DATA_TYPE_VARCHAR        = 1;
const int DB_DATA_TYPE_INTEGER        = 2;
const int DB_DATA_TYPE_FLOAT          = 3;
const int DB_DATA_TYPE_DATE           = 4;

const int DB_SELECT_KEYFIELDS         = 1;
const int DB_SELECT_WHERE             = 2;
const int DB_SELECT_MATCHING          = 3;
const int DB_SELECT_STATEMENT         = 4;

const int DB_UPD_KEYFIELDS            = 1;
const int DB_UPD_WHERE                = 2;

const int DB_DEL_KEYFIELDS            = 1;
const int DB_DEL_WHERE                = 2;
const int DB_DEL_MATCHING             = 3;

const int DB_WHERE_KEYFIELDS          = 1;
const int DB_WHERE_MATCHING           = 2;

const int DB_GRANT_SELECT             = 1;
const int DB_GRANT_INSERT             = 2;
const int DB_GRANT_UPDATE             = 4;
const int DB_GRANT_DELETE             = 8;
const int DB_GRANT_ALL                = DB_GRANT_SELECT | DB_GRANT_INSERT | DB_GRANT_UPDATE | DB_GRANT_DELETE;

// ODBC Error codes (derived from ODBC SqlState codes)
enum wxODBC_ERRORS
{
    DB_FAILURE                        = 0,
    DB_SUCCESS                        = 1,
    DB_ERR_NOT_IN_USE,
    DB_ERR_GENERAL_WARNING,                            // SqlState = '01000'
    DB_ERR_DISCONNECT_ERROR,                           // SqlState = '01002'
    DB_ERR_DATA_TRUNCATED,                             // SqlState = '01004'
    DB_ERR_PRIV_NOT_REVOKED,                           // SqlState = '01006'
    DB_ERR_INVALID_CONN_STR_ATTR,                      // SqlState = '01S00'
    DB_ERR_ERROR_IN_ROW,                               // SqlState = '01S01'
    DB_ERR_OPTION_VALUE_CHANGED,                       // SqlState = '01S02'
    DB_ERR_NO_ROWS_UPD_OR_DEL,                         // SqlState = '01S03'
    DB_ERR_MULTI_ROWS_UPD_OR_DEL,                      // SqlState = '01S04'
    DB_ERR_WRONG_NO_OF_PARAMS,                         // SqlState = '07001'
    DB_ERR_DATA_TYPE_ATTR_VIOL,                        // SqlState = '07006'
    DB_ERR_UNABLE_TO_CONNECT,                          // SqlState = '08001'
    DB_ERR_CONNECTION_IN_USE,                          // SqlState = '08002'
    DB_ERR_CONNECTION_NOT_OPEN,                        // SqlState = '08003'
    DB_ERR_REJECTED_CONNECTION,                        // SqlState = '08004'
    DB_ERR_CONN_FAIL_IN_TRANS,                         // SqlState = '08007'
    DB_ERR_COMM_LINK_FAILURE,                          // SqlState = '08S01'
    DB_ERR_INSERT_VALUE_LIST_MISMATCH,                 // SqlState = '21S01'
    DB_ERR_DERIVED_TABLE_MISMATCH,                     // SqlState = '21S02'
    DB_ERR_STRING_RIGHT_TRUNC,                         // SqlState = '22001'
    DB_ERR_NUMERIC_VALUE_OUT_OF_RNG,                   // SqlState = '22003'
    DB_ERR_ERROR_IN_ASSIGNMENT,                        // SqlState = '22005'
    DB_ERR_DATETIME_FLD_OVERFLOW,                      // SqlState = '22008'
    DB_ERR_DIVIDE_BY_ZERO,                             // SqlState = '22012'
    DB_ERR_STR_DATA_LENGTH_MISMATCH,                   // SqlState = '22026'
    DB_ERR_INTEGRITY_CONSTRAINT_VIOL,                  // SqlState = '23000'
    DB_ERR_INVALID_CURSOR_STATE,                       // SqlState = '24000'
    DB_ERR_INVALID_TRANS_STATE,                        // SqlState = '25000'
    DB_ERR_INVALID_AUTH_SPEC,                          // SqlState = '28000'
    DB_ERR_INVALID_CURSOR_NAME,                        // SqlState = '34000'
    DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL,                // SqlState = '37000'
    DB_ERR_DUPLICATE_CURSOR_NAME,                      // SqlState = '3C000'
    DB_ERR_SERIALIZATION_FAILURE,                      // SqlState = '40001'
    DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL2,               // SqlState = '42000'
    DB_ERR_OPERATION_ABORTED,                          // SqlState = '70100'
    DB_ERR_UNSUPPORTED_FUNCTION,                       // SqlState = 'IM001'
    DB_ERR_NO_DATA_SOURCE,                             // SqlState = 'IM002'
    DB_ERR_DRIVER_LOAD_ERROR,                          // SqlState = 'IM003'
    DB_ERR_SQLALLOCENV_FAILED,                         // SqlState = 'IM004'
    DB_ERR_SQLALLOCCONNECT_FAILED,                     // SqlState = 'IM005'
    DB_ERR_SQLSETCONNECTOPTION_FAILED,                 // SqlState = 'IM006'
    DB_ERR_NO_DATA_SOURCE_DLG_PROHIB,                  // SqlState = 'IM007'
    DB_ERR_DIALOG_FAILED,                              // SqlState = 'IM008'
    DB_ERR_UNABLE_TO_LOAD_TRANSLATION_DLL,             // SqlState = 'IM009'
    DB_ERR_DATA_SOURCE_NAME_TOO_LONG,                  // SqlState = 'IM010'
    DB_ERR_DRIVER_NAME_TOO_LONG,                       // SqlState = 'IM011'
    DB_ERR_DRIVER_KEYWORD_SYNTAX_ERROR,                // SqlState = 'IM012'
    DB_ERR_TRACE_FILE_ERROR,                           // SqlState = 'IM013'
    DB_ERR_TABLE_OR_VIEW_ALREADY_EXISTS,               // SqlState = 'S0001'
    DB_ERR_TABLE_NOT_FOUND,                            // SqlState = 'S0002'
    DB_ERR_INDEX_ALREADY_EXISTS,                       // SqlState = 'S0011'
    DB_ERR_INDEX_NOT_FOUND,                            // SqlState = 'S0012'
    DB_ERR_COLUMN_ALREADY_EXISTS,                      // SqlState = 'S0021'
    DB_ERR_COLUMN_NOT_FOUND,                           // SqlState = 'S0022'
    DB_ERR_NO_DEFAULT_FOR_COLUMN,                      // SqlState = 'S0023'
    DB_ERR_GENERAL_ERROR,                              // SqlState = 'S1000'
    DB_ERR_MEMORY_ALLOCATION_FAILURE,                  // SqlState = 'S1001'
    DB_ERR_INVALID_COLUMN_NUMBER,                      // SqlState = 'S1002'
    DB_ERR_PROGRAM_TYPE_OUT_OF_RANGE,                  // SqlState = 'S1003'
    DB_ERR_SQL_DATA_TYPE_OUT_OF_RANGE,                 // SqlState = 'S1004'
    DB_ERR_OPERATION_CANCELLED,                        // SqlState = 'S1008'
    DB_ERR_INVALID_ARGUMENT_VALUE,                     // SqlState = 'S1009'
    DB_ERR_FUNCTION_SEQUENCE_ERROR,                    // SqlState = 'S1010'
    DB_ERR_OPERATION_INVALID_AT_THIS_TIME,             // SqlState = 'S1011'
    DB_ERR_INVALID_TRANS_OPERATION_CODE,               // SqlState = 'S1012'
    DB_ERR_NO_CURSOR_NAME_AVAIL,                       // SqlState = 'S1015'
    DB_ERR_INVALID_STR_OR_BUF_LEN,                     // SqlState = 'S1090'
    DB_ERR_DESCRIPTOR_TYPE_OUT_OF_RANGE,               // SqlState = 'S1091'
    DB_ERR_OPTION_TYPE_OUT_OF_RANGE,                   // SqlState = 'S1092'
    DB_ERR_INVALID_PARAM_NO,                           // SqlState = 'S1093'
    DB_ERR_INVALID_SCALE_VALUE,                        // SqlState = 'S1094'
    DB_ERR_FUNCTION_TYPE_OUT_OF_RANGE,                 // SqlState = 'S1095'
    DB_ERR_INF_TYPE_OUT_OF_RANGE,                      // SqlState = 'S1096'
    DB_ERR_COLUMN_TYPE_OUT_OF_RANGE,                   // SqlState = 'S1097'
    DB_ERR_SCOPE_TYPE_OUT_OF_RANGE,                    // SqlState = 'S1098'
    DB_ERR_NULLABLE_TYPE_OUT_OF_RANGE,                 // SqlState = 'S1099'
    DB_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE,        // SqlState = 'S1100'
    DB_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE,          // SqlState = 'S1101'
    DB_ERR_DIRECTION_OPTION_OUT_OF_RANGE,              // SqlState = 'S1103'
    DB_ERR_INVALID_PRECISION_VALUE,                    // SqlState = 'S1104'
    DB_ERR_INVALID_PARAM_TYPE,                         // SqlState = 'S1105'
    DB_ERR_FETCH_TYPE_OUT_OF_RANGE,                    // SqlState = 'S1106'
    DB_ERR_ROW_VALUE_OUT_OF_RANGE,                     // SqlState = 'S1107'
    DB_ERR_CONCURRENCY_OPTION_OUT_OF_RANGE,            // SqlState = 'S1108'
    DB_ERR_INVALID_CURSOR_POSITION,                    // SqlState = 'S1109'
    DB_ERR_INVALID_DRIVER_COMPLETION,                  // SqlState = 'S1110'
    DB_ERR_INVALID_BOOKMARK_VALUE,                     // SqlState = 'S1111'
    DB_ERR_DRIVER_NOT_CAPABLE,                         // SqlState = 'S1C00'
    DB_ERR_TIMEOUT_EXPIRED                             // SqlState = 'S1T00'
};

#ifndef MAXNAME
#define MAXNAME         31
#endif

#ifndef SQL_MAX_AUTHSTR_LEN
// There does not seem to be a standard for this, so I am
// defaulting to the value that MS uses
#define SQL_MAX_AUTHSTR_LEN MAXNAME
#endif

class WXDLLEXPORT wxDbConnectInf
{
    private:
        bool freeHenvOnDestroy;

    public:
	    HENV Henv;
	    wxChar Dsn[SQL_MAX_DSN_LENGTH+1];                  // Data Source Name
	    wxChar Uid[SQL_MAX_USER_NAME_LEN+1];               // User ID
	    wxChar AuthStr[SQL_MAX_AUTHSTR_LEN+1];             // Authorization string (password)

        wxString Description;                              // Not sure what the max length is
        wxString FileType;                                 // Not sure what the max length is

        // Optionals needed for some databases like dBase
        wxString DefaultDir;                               // Directory that db file resides in

    public:

        wxDbConnectInf();
        wxDbConnectInf(HENV henv, const wxString &dsn, const wxString &userID=wxEmptyString, 
                       const wxString &password=wxEmptyString, const wxString &defaultDir=wxEmptyString, 
                       const wxString &description=wxEmptyString, const wxString &fileType=wxEmptyString);

        ~wxDbConnectInf();

        bool             Initialize();

        bool             AllocHenv();
        void             FreeHenv();

        // Accessors
        const HENV       &GetHenv()          { return Henv; };

        const wxChar    *GetDsn()           { return Dsn; };

        const wxChar    *GetUid()           { return Uid; };
        const wxChar    *GetUserID()        { return Uid; };

        const wxChar    *GetAuthStr()       { return AuthStr; };
        const wxChar    *GetPassword()      { return AuthStr; };

        const wxChar    *GetDescription()   { return Description; };
        const wxChar    *GetFileType()      { return FileType; };
        const wxChar    *GetDefaultDir()    { return DefaultDir; };

        void             SetHenv(const HENV henv)               { Henv = henv; };

        void             SetDsn(const wxString &dsn);

        void             SetUserID(const wxString &userID);
        void             SetUid(const wxString &uid)            { SetUserID(uid); };

        void             SetPassword(const wxString &password);
        void             SetAuthStr(const wxString &authstr)    { SetPassword(authstr); };

        void             SetDescription(const wxString &desc)   { Description   = desc;     };
        void             SetFileType(const wxString &fileType)  { FileType      = fileType; };
        void             SetDefaultDir(const wxString &defDir)  { DefaultDir    = defDir;   };
};  // class wxDbConnectInf


struct WXDLLEXPORT wxDbSqlTypeInfo
{
    wxString    TypeName;
    int         FsqlType;
    long        Precision;
    short       CaseSensitive;
//    short     MinimumScale;
    short       MaximumScale;
};


class WXDLLEXPORT wxDbColFor
{
public:
    wxString       s_Field;              // Formated String for Output
    wxString       s_Format[7];          // Formated Objects - TIMESTAMP has the biggest (7)
    wxString       s_Amount[7];          // Formated Objects - amount of things that can be formatted
    int            i_Amount[7];          // Formated Objects - TT MM YYYY HH MM SS m
    int            i_Nation;             // 0 = timestamp , 1=EU, 2=UK, 3=International, 4=US
    int            i_dbDataType;         // conversion of the 'sqlDataType' to the generic data type used by these classes
    SWORD          i_sqlDataType;

    wxDbColFor();
    ~wxDbColFor();

    void           Initialize();
    int            Format(int Nation, int dbDataType, SWORD sqlDataType, short columnSize, short decimalDigits);
};


class WXDLLEXPORT wxDbColInf
{
public:
    wxChar       catalog[128+1];
    wxChar       schema[128+1];
    wxChar       tableName[DB_MAX_TABLE_NAME_LEN+1];
    wxChar       colName[DB_MAX_COLUMN_NAME_LEN+1];
    SWORD        sqlDataType;
    wxChar       typeName[128+1];
    SWORD        columnSize;
    SWORD        bufferLength;
    short        decimalDigits;
    short        numPrecRadix;
    short        nullable;
    wxChar       remarks[254+1];
    int          dbDataType;  // conversion of the 'sqlDataType' to the generic data type used by these classes
 // mj10777.19991224 : new
    int          PkCol;       // Primary key column       0=No; 1= First Key, 2 = Second Key etc.
    wxChar       PkTableName[DB_MAX_TABLE_NAME_LEN+1]; // Tables that use this PKey as a FKey
    int          FkCol;       // Foreign key column       0=No; 1= First Key, 2 = Second Key etc.
    wxChar       FkTableName[DB_MAX_TABLE_NAME_LEN+1]; // Foreign key table name
    wxDbColFor  *pColFor;                              // How should this columns be formatted

    wxDbColInf();
    ~wxDbColInf();

    bool Initialize();
};




class WXDLLEXPORT wxDbTableInf        // Description of a Table
{
public:
    wxChar      tableName[DB_MAX_TABLE_NAME_LEN+1];
    wxChar      tableType[254+1];           // "TABLE" or "SYSTEM TABLE" etc.
    wxChar      tableRemarks[254+1];
    int         numCols;                    // How many Columns does this Table have: GetColumnCount(..);
    wxDbColInf *pColInf;                    // pColInf = NULL ; User can later call GetColumns(..);

    wxDbTableInf();
    ~wxDbTableInf();

    bool             Initialize();
};


class WXDLLEXPORT wxDbInf     // Description of a Database
{
public:
    wxChar        catalog[128+1];
    wxChar        schema[128+1];
    int           numTables;           // How many tables does this database have
    wxDbTableInf *pTableInf;           // pTableInf = new wxDbTableInf[numTables];

    wxDbInf();
    ~wxDbInf();

    bool          Initialize();
};


enum wxDbSqlLogState
{
    sqlLogOFF,
    sqlLogON
};

// These are the databases currently tested and working with these classes
// See the comments in wxDb::Dbms() for exceptions/issues with
// each of these database engines
enum wxDBMS
{
    dbmsUNIDENTIFIED,
    dbmsORACLE,
    dbmsSYBASE_ASA,        // Adaptive Server Anywhere
    dbmsSYBASE_ASE,        // Adaptive Server Enterprise
    dbmsMS_SQL_SERVER,
    dbmsMY_SQL,
    dbmsPOSTGRES,
    dbmsACCESS,
    dbmsDBASE,
    dbmsINFORMIX,
    dbmsVIRTUOSO,
    dbmsDB2,
    dbmsINTERBASE
};


// The wxDb::errorList is copied to this variable when the wxDb object
// is closed.  This way, the error list is still available after the
// database object is closed.  This is necessary if the database
// connection fails so the calling application can show the operator
// why the connection failed.  Note: as each wxDb object is closed, it
// will overwrite the errors of the previously destroyed wxDb object in
// this variable.
extern wxChar DBerrorList[DB_MAX_ERROR_HISTORY][DB_MAX_ERROR_MSG_LEN];


class WXDLLEXPORT wxDb
{
private:
    bool             dbIsOpen;
    wxString         dsn;             // Data source name
    wxString         uid;             // User ID
    wxString         authStr;         // Authorization string (password)
    FILE            *fpSqlLog;        // Sql Log file pointer
    wxDbSqlLogState  sqlLogState;     // On or Off
    bool             fwdOnlyCursors;
    wxDBMS           dbmsType;        // Type of datasource - i.e. Oracle, dBase, SQLServer, etc

    // Private member functions
    bool             getDbInfo(void);
    bool             getDataTypeInfo(SWORD fSqlType, wxDbSqlTypeInfo &structSQLTypeInfo);
    bool             setConnectionOptions(void);
    void             logError(const wxString &errMsg, const wxString &SQLState);
    const wxChar    *convertUserID(const wxChar *userID, wxString &UserID);
    void             initialize();

#if !wxODBC_BACKWARD_COMPATABILITY
    // ODBC handles
    HENV  henv;        // ODBC Environment handle
    HDBC  hdbc;        // ODBC DB Connection handle
    HSTMT hstmt;       // ODBC Statement handle

    //Error reporting mode
    bool silent;

    // Number of Ctable objects connected to this db object.  FOR INTERNAL USE ONLY!!!
    unsigned int nTables;

    // Information about logical data types VARCHAR, INTEGER, FLOAT and DATE.
     //
    // This information is obtained from the ODBC driver by use of the
    // SQLGetTypeInfo() function.  The key piece of information is the
    // type name the data source uses for each logical data type.
    // e.g. VARCHAR; Oracle calls it VARCHAR2.
    wxDbSqlTypeInfo typeInfVarchar;
    wxDbSqlTypeInfo typeInfInteger;
    wxDbSqlTypeInfo typeInfFloat;
    wxDbSqlTypeInfo typeInfDate;
#endif

public:
#if wxODBC_BACKWARD_COMPATABILITY
    // ODBC handles
    HENV  henv;        // ODBC Environment handle
    HDBC  hdbc;        // ODBC DB Connection handle
    HSTMT hstmt;       // ODBC Statement handle

    //Error reporting mode
    bool silent;

    // Number of Ctable objects connected to this db object.  FOR INTERNAL USE ONLY!!!
    unsigned int nTables;
#endif

    // The following structure contains database information gathered from the
    // datasource when the datasource is first opened.
    struct
    {
        wxChar dbmsName[40];                             // Name of the dbms product
        wxChar dbmsVer[64];                              // Version # of the dbms product
        wxChar driverName[40];                           // Driver name
        wxChar odbcVer[60];                              // ODBC version of the driver
        wxChar drvMgrOdbcVer[60];                        // ODBC version of the driver manager
        wxChar driverVer[60];                            // Driver version
        wxChar serverName[80];                           // Server Name, typically a connect string
        wxChar databaseName[128];                        // Database filename
        wxChar outerJoins[2];                            // Indicates whether the data source supports outer joins
        wxChar procedureSupport[2];                      // Indicates whether the data source supports stored procedures
        wxChar accessibleTables[2];                      // Indicates whether the data source only reports accessible tables in SQLTables.
        UWORD  maxConnections;                           // Maximum # of connections the data source supports
        UWORD  maxStmts;                                 // Maximum # of HSTMTs per HDBC
        UWORD  apiConfLvl;                               // ODBC API conformance level
        UWORD  cliConfLvl;                               // Indicates whether the data source is SAG compliant
        UWORD  sqlConfLvl;                               // SQL conformance level
        UWORD  cursorCommitBehavior;                     // Indicates how cursors are affected by a db commit
        UWORD  cursorRollbackBehavior;                   // Indicates how cursors are affected by a db rollback
        UWORD  supportNotNullClause;                     // Indicates if data source supports NOT NULL clause
        wxChar supportIEF[2];                            // Integrity Enhancement Facility (Referential Integrity)
        UDWORD txnIsolation;                             // Default transaction isolation level supported by the driver
        UDWORD txnIsolationOptions;                      // Transaction isolation level options available
        UDWORD fetchDirections;                          // Fetch directions supported
        UDWORD lockTypes;                                // Lock types supported in SQLSetPos
        UDWORD posOperations;                            // Position operations supported in SQLSetPos
        UDWORD posStmts;                                 // Position statements supported
        UDWORD scrollConcurrency;                        // Concurrency control options supported for scrollable cursors
        UDWORD scrollOptions;                            // Scroll Options supported for scrollable cursors
        UDWORD staticSensitivity;                        // Indicates if additions, deletions and updates can be detected
        UWORD  txnCapable;                               // Indicates if the data source supports transactions
        UDWORD loginTimeout;                             // Number seconds to wait for a login request
    } dbInf;

    // ODBC Error Inf.
    SWORD  cbErrorMsg;
    int    DB_STATUS;
    wxChar errorList[DB_MAX_ERROR_HISTORY][DB_MAX_ERROR_MSG_LEN];
    wxChar errorMsg[SQL_MAX_MESSAGE_LENGTH];
    SDWORD nativeError;
    wxChar sqlState[20];

#if wxODBC_BACKWARD_COMPATABILITY
    // Information about logical data types VARCHAR, INTEGER, FLOAT and DATE.
     //
    // This information is obtained from the ODBC driver by use of the
    // SQLGetTypeInfo() function.  The key piece of information is the
    // type name the data source uses for each logical data type.
    // e.g. VARCHAR; Oracle calls it VARCHAR2.
    wxDbSqlTypeInfo typeInfVarchar;
    wxDbSqlTypeInfo typeInfInteger;
    wxDbSqlTypeInfo typeInfFloat;
    wxDbSqlTypeInfo typeInfDate;
#endif

    // Public member functions
    wxDb(const HENV &aHenv, bool FwdOnlyCursors=(bool)wxODBC_FWD_ONLY_CURSORS);

    bool         Open(const wxString &Dsn, const wxString &Uid, const wxString &AuthStr);  // Data Source Name, User ID, Password
    bool         Open(wxDbConnectInf *dbConnectInf);
    bool         Open(wxDb *copyDb);  // pointer to a wxDb whose connection info should be copied rather than re-queried
    void         Close(void);
    bool         CommitTrans(void);
    bool         RollbackTrans(void);
    bool         DispAllErrors(HENV aHenv, HDBC aHdbc = SQL_NULL_HDBC, HSTMT aHstmt = SQL_NULL_HSTMT);
    bool         GetNextError(HENV aHenv, HDBC aHdbc = SQL_NULL_HDBC, HSTMT aHstmt = SQL_NULL_HSTMT);
    void         DispNextError(void);
    bool         CreateView(const wxString &viewName, const wxString &colList, const wxString &pSqlStmt, bool attemptDrop=TRUE);
    bool         DropView(const wxString &viewName);
    bool         ExecSql(const wxString &pSqlStmt);
    bool         GetNext(void);
    bool         GetData(UWORD colNo, SWORD cType, PTR pData, SDWORD maxLen, SDWORD FAR *cbReturned);
    bool         Grant(int privileges, const wxString &tableName, const wxString &userList = wxT("PUBLIC"));
    int          TranslateSqlState(const wxString &SQLState);
    wxDbInf     *GetCatalog(const wxChar *userID=NULL);
    bool         Catalog(const wxChar *userID=NULL, const wxString &fileName=SQL_CATALOG_FILENAME);
    int          GetKeyFields(const wxString &tableName, wxDbColInf* colInf, int nocols);

    wxDbColInf  *GetColumns(wxChar *tableName[], const wxChar *userID=NULL);
    wxDbColInf  *GetColumns(const wxString &tableName, int *numCols, const wxChar *userID=NULL); 

    int          GetColumnCount(const wxString &tableName, const wxChar *userID=NULL);
    const wxChar *GetDatabaseName(void)  {return dbInf.dbmsName;}
    const wxString &GetDataSource(void)    {return dsn;}
    const wxString &GetDatasourceName(void){return dsn;}
    const wxString &GetUsername(void)      {return uid;}
    const wxString &GetPassword(void)      {return authStr;}
    bool         IsOpen(void)           {return dbIsOpen;}
    HENV         GetHENV(void)          {return henv;}
    HDBC         GetHDBC(void)          {return hdbc;}
    HSTMT        GetHSTMT(void)         {return hstmt;}
    int          GetTableCount()        {return nTables;};  // number of tables using this connection
    wxDbSqlTypeInfo GetTypeInfVarchar() {return typeInfVarchar;}
    wxDbSqlTypeInfo GetTypeInfInteger() {return typeInfInteger;}
    wxDbSqlTypeInfo GetTypeInfFloat()   {return typeInfFloat;}
    wxDbSqlTypeInfo GetTypeInfDate()    {return typeInfDate;}

    bool         TableExists(const wxString &tableName, const wxChar *userID=NULL, const wxString &tablePath=wxEmptyString);  // tableName can refer to a table, view, alias or synonym
    bool         TablePrivileges(const wxString &tableName, const wxString &priv, const wxChar *userID=NULL, const wxChar *schema=NULL, const wxString &path=wxEmptyString);  // tableName can refer to a table, view, alias or synonym
    void         LogError(const wxString &errMsg, const wxString &SQLState = wxEmptyString) { logError(errMsg, SQLState); }
    void         SetDebugErrorMessages(bool state) { silent = !state; }
    bool         SetSqlLogging(wxDbSqlLogState state, const wxString &filename = SQL_LOG_FILENAME, bool append = FALSE);
    bool         WriteSqlLog(const wxString &logMsg);
    wxDBMS       Dbms(void);
    bool         ModifyColumn(const wxString &tableName, const wxString &columnName,
                              int dataType, ULONG columnLength=0,
                              const wxString &optionalParam=wxEmptyString);

    bool         FwdOnlyCursors(void)  {return fwdOnlyCursors;}

    // These two functions are provided strictly for use by wxDbTable.
    // DO NOT USE THESE FUNCTIONS, OR MEMORY LEAKS MAY OCCUR
    void         incrementTableCount() { nTables++; return; }
    void         decrementTableCount() { nTables--; return; }

};  // wxDb


// This structure forms a node in a linked list.  The linked list of "DbList" objects
// keeps track of allocated database connections.  This allows the application to
// open more than one database connection through ODBC for multiple transaction support
// or for multiple database support.
struct wxDbList
{
    wxDbList *PtrPrev;       // Pointer to previous item in the list
    wxString  Dsn;           // Data Source Name
    wxString  Uid;           // User ID
    wxString  AuthStr;       // Authorization string (password)
    wxDb     *PtrDb;         // Pointer to the wxDb object
    bool      Free;          // Is item free or in use?
    wxDbList *PtrNext;       // Pointer to next item in the list
};


#ifdef __WXDEBUG__
#include "wx/object.h"
class wxTablesInUse : public wxObject
{
    public:
        const wxChar  *tableName;
        ULONG          tableID;
        class wxDb    *pDb;
};  // wxTablesInUse
#endif


// The following routines allow a user to get new database connections, free them
// for other code segments to use, or close all of them when the application has
// completed.
wxDb  WXDLLEXPORT *wxDbGetConnection(wxDbConnectInf *pDbConfig, bool FwdOnlyCursors=(bool)wxODBC_FWD_ONLY_CURSORS);
bool  WXDLLEXPORT  wxDbFreeConnection(wxDb *pDb);
void  WXDLLEXPORT  wxDbCloseConnections(void);
int   WXDLLEXPORT  wxDbConnectionsInUse(void);


// This function sets the sql log state for all open wxDb objects
bool wxDbSqlLog(wxDbSqlLogState state, const wxString &filename = SQL_LOG_FILENAME);


#if 0
// MSW/VC6 ONLY!!!  Experimental
int WXDLLEXPORT wxDbCreateDataSource(const wxString &driverName, const wxString &dsn, const wxString &description=wxEmptyString,
                                     bool sysDSN=FALSE, const wxString &defDir=wxEmptyString, wxWindow *parent=NULL);
#endif

// This routine allows you to query a driver manager
// for a list of available datasources.  Call this routine
// the first time using SQL_FETCH_FIRST.  Continue to call it
// using SQL_FETCH_NEXT until you've exhausted the list.
bool WXDLLEXPORT wxDbGetDataSource(HENV henv, wxChar *Dsn, SWORD DsnMax, wxChar *DsDesc,
                                   SWORD DsDescMax, UWORD direction = SQL_FETCH_NEXT);


// Change this to 0 to remove use of all deprecated functions
#if wxODBC_BACKWARD_COMPATABILITY
//#################################################################################
//############### DEPRECATED functions for backward compatability #################
//#################################################################################

// Backward compability structures/classes.  This will eventually go away
const int DB_PATH_MAX      = wxDB_PATH_MAX;

typedef wxDb                 wxDB;
typedef wxDbTableInf         wxTableInf;
typedef wxDbColInf           wxColInf;
typedef wxDbColInf           CcolInf;
typedef wxDbColFor           wxColFor;
typedef wxDbSqlTypeInfo      SqlTypeInfo;
typedef wxDbSqlTypeInfo      wxSqlTypeInfo;
typedef enum wxDbSqlLogState sqlLog;
typedef enum wxDbSqlLogState wxSqlLogState;
typedef enum wxDBMS          dbms;
typedef enum wxDBMS          DBMS;
typedef wxODBC_ERRORS        ODBC_ERRORS;
typedef wxDbConnectInf       DbStuff;
typedef wxDbList             DbList;
#ifdef __WXDEBUG__
typedef wxTablesInUse        CstructTablesInUse;
#endif

// Deprecated function names that are replaced by the function names listed above
wxDB  WXDLLEXPORT *GetDbConnection(DbStuff *pDbStuff, bool FwdOnlyCursors=(bool)wxODBC_FWD_ONLY_CURSORS);
bool  WXDLLEXPORT  FreeDbConnection(wxDB *pDb);
void  WXDLLEXPORT  CloseDbConnections(void);
int   WXDLLEXPORT  NumberDbConnectionsInUse(void);

bool SqlLog(sqlLog state, const char *filename = SQL_LOG_FILENAME);

bool WXDLLEXPORT GetDataSource(HENV henv, char *Dsn, SWORD DsnMax, char *DsDesc, SWORD DsDescMax,
                               UWORD direction = SQL_FETCH_NEXT);
#endif  // Deprecated structures/classes/functions

#endif
