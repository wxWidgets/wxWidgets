///////////////////////////////////////////////////////////////////////////////
// Name:        db.cpp
// Purpose:     Implementation of the wxDB class.  The wxDB class represents a connection
//              to an ODBC data source.  The wxDB class allows operations on the data
//              source such as opening and closing the data source.
// Author:      Doug Card
// Modified by: George Tasker
// Mods:        Dec, 1998:
//                -Added support for SQL statement logging and database cataloging
// Mods:        April, 1999
//                -Added QUERY_ONLY mode support to reduce default number of cursors
//                -Added additional SQL logging code
//                -Added DEBUG-ONLY tracking of wxTable objects to detect orphaned DB connections
//                -Set ODBC option to only read committed writes to the DB so all
//                   databases operate the same in that respect
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
///////////////////////////////////////////////////////////////////////////////

/*
// SYNOPSIS START
// SYNOPSIS STOP
*/

#include "wx/wxprec.h"

// Use this line for wxWindows v1.x
//#include "wx_ver.h"
// Use this line for wxWindows v2.x
#include "wx/version.h"

#if wxMAJOR_VERSION == 2
    #ifdef __GNUG__
    #pragma implementation "db.h"
    #endif
#endif

#ifdef DBDEBUG_CONSOLE
    #include <iostream.h>
#endif

#ifdef    __BORLANDC__
    #pragma hdrstop
#endif  //__BORLANDC__

#if wxMAJOR_VERSION == 2
    #ifndef WX_PRECOMP
        #include "wx/string.h"
        #include "wx/object.h"
        #include "wx/list.h"
        #include "wx/utils.h"
        #include "wx/msgdlg.h"
    #endif
    #include "wx/filefn.h"
    #include "wx/wxchar.h"
#endif

#if wxMAJOR_VERSION == 1
#    if defined(wx_msw) || defined(wx_x)
#        ifdef WX_PRECOMP
#            include "wx_prec.h"
#        else
#            include "wx.h"
#        endif
#    endif
#    define wxUSE_ODBC 1
#endif

#if wxUSE_ODBC

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#if   wxMAJOR_VERSION == 1
    #include "db.h"
#elif wxMAJOR_VERSION == 2
    #include "wx/db.h"
#endif

DbList WXDLLEXPORT *PtrBegDbList = 0;

char const *SQL_LOG_FILENAME         = "sqllog.txt";
char const *SQL_CATALOG_FILENAME     = "catalog.txt";

#ifdef __WXDEBUG__
    extern wxList TablesInUse;
#endif

// SQL Log defaults to be used by GetDbConnection
enum sqlLog SQLLOGstate = sqlLogOFF;

//char SQLLOGfn[DB_PATH_MAX+1] = SQL_LOG_FILENAME;
char *SQLLOGfn         = (char*) SQL_LOG_FILENAME;

// The wxDB::errorList is copied to this variable when the wxDB object
// is closed.  This way, the error list is still available after the
// database object is closed.  This is necessary if the database
// connection fails so the calling application can show the operator
// why the connection failed.  Note: as each wxDB object is closed, it
// will overwrite the errors of the previously destroyed wxDB object in
// this variable.
char DBerrorList[DB_MAX_ERROR_HISTORY][DB_MAX_ERROR_MSG_LEN];


/********** wxColFor Constructor **********/
wxColFor::wxColFor()
{
    i_Nation = 0;                // 0=EU, 1=UK, 2=International, 3=US
    s_Field  = "";
    int i;
    for (i=0;i<7;i++)
    {
        s_Format[i] = "";
        s_Menge[i]  = "";
        i_Menge[i]  = 0;
    }
    Format(1,DB_DATA_TYPE_VARCHAR,0,0,0);      // the Function that does the work
}  // wxColFor::wxColFor()


wxColFor::~wxColFor()
{
}  // wxColFor::~wxColFor()


int wxColFor::Format(int Nation,int dbDataType,SWORD sqlDataType,short columnSize,short decimalDigits)
{
    // ----------------------------------------------------------------------------------------
    // -- 19991224 : mj10777@gmx.net : Create
    // There is still a lot of work to do here, but it is a start
    // It handles all the basic data-types that I have run into up to now
    // The main work will have be with Dates and float Formatting (US 1,000.00 ; EU 1.000,00)
    // There are wxWindow plans for locale support and the new wxDateTime.
    // - if they define some constants (wxEUROPEAN) that can be gloably used,
    //    they should be used here.
    // ----------------------------------------------------------------------------------------
    // There should also be a Function to scan in a string to fill the variable
    // ----------------------------------------------------------------------------------------
    wxString Temp0;
    i_Nation      = Nation;                                 // 0 = timestamp , 1=EU, 2=UK, 3=International, 4=US
    i_dbDataType  = dbDataType;
    i_sqlDataType = sqlDataType;
    s_Field.Printf("%s%d",s_Menge[1].c_str(),i_Menge[1]);   // OK for VARCHAR, INTEGER and FLOAT
    if (i_dbDataType == 0)                                  // Filter unsupported dbDataTypes
    {
        if ((i_sqlDataType == SQL_VARCHAR) || (i_sqlDataType == SQL_LONGVARCHAR))
            i_dbDataType = DB_DATA_TYPE_VARCHAR;
        if (i_sqlDataType == SQL_C_DATE)
            i_dbDataType = DB_DATA_TYPE_DATE;
        if (i_sqlDataType == SQL_C_BIT)
            i_dbDataType = DB_DATA_TYPE_INTEGER;
        if (i_sqlDataType == SQL_NUMERIC)
            i_dbDataType = DB_DATA_TYPE_VARCHAR;
        if (i_sqlDataType == SQL_REAL)
            i_dbDataType = DB_DATA_TYPE_FLOAT;
    }
    if ((i_dbDataType == DB_DATA_TYPE_INTEGER) && (i_sqlDataType == SQL_C_DOUBLE))
    {   // DBASE Numeric
        i_dbDataType = DB_DATA_TYPE_FLOAT;
    }
    switch(i_dbDataType)     // -A-> Still a lot of proper formatting to do
    {
        case DB_DATA_TYPE_VARCHAR:
            s_Field = "%s";        //
            break;
        case DB_DATA_TYPE_INTEGER:
            s_Field = "%d";        //
            break;
        case DB_DATA_TYPE_FLOAT:
            if (decimalDigits == 0)
                decimalDigits = 2;
            Temp0 = "%";
            Temp0.Printf("%s%d.%d",Temp0.c_str(),columnSize,decimalDigits);
            s_Field.Printf("%sf",Temp0.c_str());        //
            break;
        case DB_DATA_TYPE_DATE:
            if (i_Nation == 0)      // timestamp       YYYY-MM-DD HH:MM:SS.SSS (tested for SYBASE)
            {
                s_Field = "%04d-%02d-%02d %02d:%02d:%02d.%03d";
            }
            if (i_Nation == 1)      // European        DD.MM.YYYY HH:MM:SS.SSS
            {
                s_Field = "%02d.%02d.%04d %02d:%02d:%02d.%03d";
            }
            if (i_Nation == 2)      // UK              DD/MM/YYYY HH:MM:SS.SSS
            {
                s_Field = "%02d/%02d/%04d %02d:%02d:%02d.%03d";
            }
            if (i_Nation == 3)      // International   YYYY-MM-DD HH:MM:SS.SSS
            {
                s_Field = "%04d-%02d-%02d %02d:%02d:%02d.%03d";
            }
            if (i_Nation == 4)      // US              MM/DD/YYYY HH:MM:SS.SSS
            {
                s_Field = "%02d/%02d/%04d %02d:%02d:%02d.%03d";
            }
            break;
        default:
            s_Field.Printf("-E-> unknown Format(%d)-sql(%d)",dbDataType,sqlDataType);        //
            break;
    };
    return TRUE;
}  // wxColFor::Format()


/********** wxDB Constructor **********/
wxDB::wxDB(HENV &aHenv, bool FwdOnlyCursors)
{
    int i;

    fpSqlLog      = 0;            // Sql Log file pointer
    sqlLogState   = sqlLogOFF;    // By default, logging is turned off
    nTables       = 0;

    wxStrcpy(sqlState,"");
    wxStrcpy(errorMsg,"");
    nativeError = cbErrorMsg = 0;
    for (i = 0; i < DB_MAX_ERROR_HISTORY; i++)
        wxStrcpy(errorList[i], "");

    // Init typeInf structures
    wxStrcpy(typeInfVarchar.TypeName,"");
    typeInfVarchar.FsqlType      = 0;
    typeInfVarchar.Precision     = 0;
    typeInfVarchar.CaseSensitive = 0;
    typeInfVarchar.MaximumScale  = 0;

    wxStrcpy(typeInfInteger.TypeName,"");
    typeInfInteger.FsqlType      = 0;
    typeInfInteger.Precision     = 0;
    typeInfInteger.CaseSensitive = 0;
    typeInfInteger.MaximumScale  = 0;

    wxStrcpy(typeInfFloat.TypeName,"");
    typeInfFloat.FsqlType      = 0;
    typeInfFloat.Precision     = 0;
    typeInfFloat.CaseSensitive = 0;
    typeInfFloat.MaximumScale  = 0;

    wxStrcpy(typeInfDate.TypeName,"");
    typeInfDate.FsqlType      = 0;
    typeInfDate.Precision     = 0;
    typeInfDate.CaseSensitive = 0;
    typeInfDate.MaximumScale  = 0;

    // Error reporting is turned OFF by default
    silent = TRUE;

    // Copy the HENV into the db class
    henv = aHenv;
    fwdOnlyCursors = FwdOnlyCursors;

    // Allocate a data source connection handle
    if (SQLAllocConnect(henv, &hdbc) != SQL_SUCCESS)
        DispAllErrors(henv);

    // Initialize the db status flag
    DB_STATUS = 0;

    // Mark database as not open as of yet
    dbIsOpen = FALSE;

} // wxDB::wxDB()


/********** wxDB::Open() **********/
bool wxDB::Open(char *Dsn, char *Uid, char *AuthStr)
{
    assert(Dsn && wxStrlen(Dsn));
    dsn        = Dsn;
    uid        = Uid;
    authStr    = AuthStr;

    RETCODE retcode;

    if (!FwdOnlyCursors())
    {
        // Specify that the ODBC cursor library be used, if needed.  This must be
        // specified before the connection is made.
        retcode = SQLSetConnectOption(hdbc, SQL_ODBC_CURSORS, SQL_CUR_USE_IF_NEEDED);

#ifdef DBDEBUG_CONSOLE
        if (retcode == SQL_SUCCESS)
            cout << "SQLSetConnectOption(CURSOR_LIB) successful" << endl;
        else
            cout << "SQLSetConnectOption(CURSOR_LIB) failed" << endl;
#endif
    }

    // Connect to the data source
    retcode = SQLConnect(hdbc, (UCHAR FAR *) Dsn, SQL_NTS,
                         (UCHAR FAR *) Uid, SQL_NTS,
                         (UCHAR FAR *) AuthStr,SQL_NTS);
    if (retcode == SQL_SUCCESS_WITH_INFO)
        DispAllErrors(henv, hdbc);
    else if (retcode != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

/*
    If using Intersolv branded ODBC drivers, this is the place where you would substitute
    your branded driver license information

    SQLSetConnectOption(hdbc, 1041, (UDWORD) "");
    SQLSetConnectOption(hdbc, 1042, (UDWORD) "");
*/

    // Mark database as open
    dbIsOpen = TRUE;

    // Allocate a statement handle for the database connection
    if (SQLAllocStmt(hdbc, &hstmt) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    // Set Connection Options
    if (! setConnectionOptions())
        return(FALSE);

    // Query the data source for inf. about itself
    if (! getDbInfo())
        return(FALSE);

    // Query the data source regarding data type information

    //
    // The way I determined which SQL data types to use was by calling SQLGetInfo
    // for all of the possible SQL data types to see which ones were supported.  If
    // a type is not supported, the SQLFetch() that's called from getDataTypeInfo()
    // fails with SQL_NO_DATA_FOUND.  This is ugly because I'm sure the three SQL data
    // types I've selected below will not alway's be what we want.  These are just
    // what happened to work against an Oracle 7/Intersolv combination.  The following is
    // a complete list of the results I got back against the Oracle 7 database:
    //
    // SQL_BIGINT             SQL_NO_DATA_FOUND
    // SQL_BINARY             SQL_NO_DATA_FOUND
    // SQL_BIT                SQL_NO_DATA_FOUND
    // SQL_CHAR               type name = 'CHAR', Precision = 255
    // SQL_DATE               SQL_NO_DATA_FOUND
    // SQL_DECIMAL            type name = 'NUMBER', Precision = 38
    // SQL_DOUBLE             type name = 'NUMBER', Precision = 15
    // SQL_FLOAT              SQL_NO_DATA_FOUND
    // SQL_INTEGER            SQL_NO_DATA_FOUND
    // SQL_LONGVARBINARY      type name = 'LONG RAW', Precision = 2 billion
    // SQL_LONGVARCHAR        type name = 'LONG', Precision = 2 billion
    // SQL_NUMERIC            SQL_NO_DATA_FOUND
    // SQL_REAL               SQL_NO_DATA_FOUND
    // SQL_SMALLINT           SQL_NO_DATA_FOUND
    // SQL_TIME               SQL_NO_DATA_FOUND
    // SQL_TIMESTAMP          type name = 'DATE', Precision = 19
    // SQL_VARBINARY          type name = 'RAW', Precision = 255
    // SQL_VARCHAR            type name = 'VARCHAR2', Precision = 2000
    // =====================================================================
    // Results from a Microsoft Access 7.0 db, using a driver from Microsoft
    //
    // SQL_VARCHAR            type name = 'TEXT', Precision = 255
    // SQL_TIMESTAMP          type name = 'DATETIME'
    // SQL_DECIMAL            SQL_NO_DATA_FOUND
    // SQL_NUMERIC            type name = 'CURRENCY', Precision = 19
    // SQL_FLOAT              SQL_NO_DATA_FOUND
    // SQL_REAL               type name = 'SINGLE', Precision = 7
    // SQL_DOUBLE             type name = 'DOUBLE', Precision = 15
    // SQL_INTEGER            type name = 'LONG', Precision = 10

    // VARCHAR = Variable length character string
    if (! getDataTypeInfo(SQL_VARCHAR, typeInfVarchar))
        if (! getDataTypeInfo(SQL_CHAR, typeInfVarchar))
            return(FALSE);
        else
            typeInfVarchar.FsqlType = SQL_CHAR;
    else
        typeInfVarchar.FsqlType = SQL_VARCHAR;

    // Float
    if (! getDataTypeInfo(SQL_DOUBLE, typeInfFloat))
        if (! getDataTypeInfo(SQL_REAL, typeInfFloat))
            if (! getDataTypeInfo(SQL_FLOAT, typeInfFloat))
                if (! getDataTypeInfo(SQL_DECIMAL, typeInfFloat))
                    if (! getDataTypeInfo(SQL_NUMERIC, typeInfFloat))
                        return(FALSE);
                    else
                        typeInfFloat.FsqlType = SQL_NUMERIC;
                else
                    typeInfFloat.FsqlType = SQL_DECIMAL;
            else
                typeInfFloat.FsqlType = SQL_FLOAT;
        else
            typeInfFloat.FsqlType = SQL_REAL;
    else
        typeInfFloat.FsqlType = SQL_DOUBLE;

    // Integer
    if (! getDataTypeInfo(SQL_INTEGER, typeInfInteger))
        // If SQL_INTEGER is not supported, use the floating point
        // data type to store integers as well as floats
        if (! getDataTypeInfo(typeInfFloat.FsqlType, typeInfInteger))
            return(FALSE);
        else
            typeInfInteger.FsqlType = typeInfFloat.FsqlType;
    else
        typeInfInteger.FsqlType = SQL_INTEGER;

    // Date/Time
    if (Dbms() != dbmsDBASE)
   {
        if (! getDataTypeInfo(SQL_TIMESTAMP, typeInfDate))
            return(FALSE);
        else
            typeInfDate.FsqlType = SQL_TIMESTAMP;
    }
    else
    {
        if (! getDataTypeInfo(SQL_DATE, typeInfDate))
            return(FALSE);
        else
            typeInfDate.FsqlType = SQL_DATE;
    }

#ifdef DBDEBUG_CONSOLE
    cout << "VARCHAR DATA TYPE: " << typeInfVarchar.TypeName << endl;
    cout << "INTEGER DATA TYPE: " << typeInfInteger.TypeName << endl;
    cout << "FLOAT   DATA TYPE: " << typeInfFloat.TypeName << endl;
    cout << "DATE    DATA TYPE: " << typeInfDate.TypeName << endl;
    cout << endl;
#endif

    // Completed Successfully
    return(TRUE);

} // wxDB::Open()


/********** wxDB::setConnectionOptions() **********/
bool wxDB::setConnectionOptions(void)
/*
 * NOTE: The Intersolv/Oracle 7 driver was "Not Capable" of setting the login timeout.
 */
{
    SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
    SQLSetConnectOption(hdbc, SQL_OPT_TRACE, SQL_OPT_TRACE_OFF);

    // Display the connection options to verify them
#ifdef DBDEBUG_CONSOLE
    long l;
    cout << "****** CONNECTION OPTIONS ******" << endl;

    if (SQLGetConnectOption(hdbc, SQL_AUTOCOMMIT, &l) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));
    cout << "AUTOCOMMIT: " << (l == SQL_AUTOCOMMIT_OFF ? "OFF" : "ON") << endl;

    if (SQLGetConnectOption(hdbc, SQL_ODBC_CURSORS, &l) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));
    cout << "ODBC CURSORS: ";
    switch(l)
        {
        case(SQL_CUR_USE_IF_NEEDED):
            cout << "SQL_CUR_USE_IF_NEEDED";
            break;
        case(SQL_CUR_USE_ODBC):
            cout << "SQL_CUR_USE_ODBC";
            break;
        case(SQL_CUR_USE_DRIVER):
            cout << "SQL_CUR_USE_DRIVER";
            break;
        }
        cout << endl;

    if (SQLGetConnectOption(hdbc, SQL_OPT_TRACE, &l) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));
    cout << "TRACING: " << (l == SQL_OPT_TRACE_OFF ? "OFF" : "ON") << endl;

    cout << endl;
#endif

    // Completed Successfully
    return(TRUE);

} // wxDB::setConnectionOptions()


/********** wxDB::getDbInfo() **********/
bool wxDB::getDbInfo(void)
{
    SWORD cb;
    RETCODE retcode;

    if (SQLGetInfo(hdbc, SQL_SERVER_NAME, (UCHAR*) dbInf.serverName, 80, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DATABASE_NAME, (UCHAR*) dbInf.databaseName, 128, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DBMS_NAME, (UCHAR*) dbInf.dbmsName, 40, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    // 16-Mar-1999
    // After upgrading to MSVC6, the original 20 char buffer below was insufficient,
    // causing database connectivity to fail in some cases.
    retcode = SQLGetInfo(hdbc, SQL_DBMS_VER, (UCHAR*) dbInf.dbmsVer, 64, &cb);

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ACTIVE_CONNECTIONS, (UCHAR*) &dbInf.maxConnections, sizeof(dbInf.maxConnections), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ACTIVE_STATEMENTS, (UCHAR*) &dbInf.maxStmts, sizeof(dbInf.maxStmts), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DRIVER_NAME, (UCHAR*) dbInf.driverName, 40, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DRIVER_ODBC_VER, (UCHAR*) dbInf.odbcVer, 60, &cb) == SQL_ERROR)
        return(DispAllErrors(henv, hdbc));

    retcode = SQLGetInfo(hdbc, SQL_ODBC_VER, (UCHAR*) dbInf.drvMgrOdbcVer, 60, &cb);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DRIVER_VER, (UCHAR*) dbInf.driverVer, 60, &cb) == SQL_ERROR)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ODBC_API_CONFORMANCE, (UCHAR*) &dbInf.apiConfLvl, sizeof(dbInf.apiConfLvl), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ODBC_SAG_CLI_CONFORMANCE, (UCHAR*) &dbInf.cliConfLvl, sizeof(dbInf.cliConfLvl), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ODBC_SQL_CONFORMANCE, (UCHAR*) &dbInf.sqlConfLvl, sizeof(dbInf.sqlConfLvl), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_OUTER_JOINS, (UCHAR*) dbInf.outerJoins, 2, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_PROCEDURES, (UCHAR*) dbInf.procedureSupport, 2, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_CURSOR_COMMIT_BEHAVIOR, (UCHAR*) &dbInf.cursorCommitBehavior, sizeof(dbInf.cursorCommitBehavior), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_CURSOR_ROLLBACK_BEHAVIOR, (UCHAR*) &dbInf.cursorRollbackBehavior, sizeof(dbInf.cursorRollbackBehavior), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_NON_NULLABLE_COLUMNS, (UCHAR*) &dbInf.supportNotNullClause, sizeof(dbInf.supportNotNullClause), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_ODBC_SQL_OPT_IEF, (UCHAR*) dbInf.supportIEF, 2, &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_DEFAULT_TXN_ISOLATION, (UCHAR*) &dbInf.txnIsolation, sizeof(dbInf.txnIsolation), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_TXN_ISOLATION_OPTION, (UCHAR*) &dbInf.txnIsolationOptions, sizeof(dbInf.txnIsolationOptions), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_FETCH_DIRECTION, (UCHAR*) &dbInf.fetchDirections, sizeof(dbInf.fetchDirections), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_LOCK_TYPES, (UCHAR*) &dbInf.lockTypes, sizeof(dbInf.lockTypes), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_POS_OPERATIONS, (UCHAR*) &dbInf.posOperations, sizeof(dbInf.posOperations), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_POSITIONED_STATEMENTS, (UCHAR*) &dbInf.posStmts, sizeof(dbInf.posStmts), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_SCROLL_CONCURRENCY, (UCHAR*) &dbInf.scrollConcurrency, sizeof(dbInf.scrollConcurrency), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_SCROLL_OPTIONS, (UCHAR*) &dbInf.scrollOptions, sizeof(dbInf.scrollOptions), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_STATIC_SENSITIVITY, (UCHAR*) &dbInf.staticSensitivity, sizeof(dbInf.staticSensitivity), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_TXN_CAPABLE, (UCHAR*) &dbInf.txnCapable, sizeof(dbInf.txnCapable), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    if (SQLGetInfo(hdbc, SQL_LOGIN_TIMEOUT, (UCHAR*) &dbInf.loginTimeout, sizeof(dbInf.loginTimeout), &cb) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

#ifdef DBDEBUG_CONSOLE
    cout << "***** DATA SOURCE INFORMATION *****" << endl;
    cout << "SERVER Name: " << dbInf.serverName << endl;
    cout << "DBMS Name: " << dbInf.dbmsName << "; DBMS Version: " << dbInf.dbmsVer << endl;
    cout << "ODBC Version: " << dbInf.odbcVer << "; Driver Version: " << dbInf.driverVer << endl;

    cout << "API Conf. Level: ";
    switch(dbInf.apiConfLvl)
    {
        case SQL_OAC_NONE:      cout << "None";       break;
        case SQL_OAC_LEVEL1:    cout << "Level 1";    break;
        case SQL_OAC_LEVEL2:    cout << "Level 2";    break;
    }
    cout << endl;

    cout << "SAG CLI Conf. Level: ";
    switch(dbInf.cliConfLvl)
    {
        case SQL_OSCC_NOT_COMPLIANT:    cout << "Not Compliant";    break;
        case SQL_OSCC_COMPLIANT:        cout << "Compliant";        break;
    }
    cout << endl;

    cout << "SQL Conf. Level: ";
    switch(dbInf.sqlConfLvl)
    {
        case SQL_OSC_MINIMUM:     cout << "Minimum Grammer";     break;
        case SQL_OSC_CORE:        cout << "Core Grammer";        break;
        case SQL_OSC_EXTENDED:    cout << "Extended Grammer";    break;
    }
    cout << endl;

    cout << "Max. Connections: "       << dbInf.maxConnections   << endl;
    cout << "Outer Joins: "            << dbInf.outerJoins       << endl;
    cout << "Support for Procedures: " << dbInf.procedureSupport << endl;

    cout << "Cursor COMMIT Behavior: ";
    switch(dbInf.cursorCommitBehavior)
    {
        case SQL_CB_DELETE:        cout << "Delete cursors";      break;
        case SQL_CB_CLOSE:         cout << "Close cursors";       break;
        case SQL_CB_PRESERVE:      cout << "Preserve cursors";    break;
    }
    cout << endl;

    cout << "Cursor ROLLBACK Behavior: ";
    switch(dbInf.cursorRollbackBehavior)
    {
        case SQL_CB_DELETE:      cout << "Delete cursors";      break;
        case SQL_CB_CLOSE:       cout << "Close cursors";       break;
        case SQL_CB_PRESERVE:    cout << "Preserve cursors";    break;
    }
    cout << endl;

    cout << "Support NOT NULL clause: ";
    switch(dbInf.supportNotNullClause)
    {
        case SQL_NNC_NULL:        cout << "No";        break;
        case SQL_NNC_NON_NULL:    cout << "Yes";       break;
    }
    cout << endl;

    cout << "Support IEF (Ref. Integrity): " << dbInf.supportIEF   << endl;
    cout << "Login Timeout: "                << dbInf.loginTimeout << endl;

    cout << endl << endl << "more ..." << endl;
    getchar();

    cout << "Default Transaction Isolation: ";
    switch(dbInf.txnIsolation)
    {
        case SQL_TXN_READ_UNCOMMITTED:  cout << "Read Uncommitted";    break;
        case SQL_TXN_READ_COMMITTED:    cout << "Read Committed";      break;
        case SQL_TXN_REPEATABLE_READ:   cout << "Repeatable Read";     break;
        case SQL_TXN_SERIALIZABLE:      cout << "Serializable";        break;
#ifdef ODBC_V20
        case SQL_TXN_VERSIONING:        cout << "Versioning";          break;
#endif
    }
    cout << endl;

    cout << "Transaction Isolation Options: ";
    if (dbInf.txnIsolationOptions & SQL_TXN_READ_UNCOMMITTED)
        cout << "Read Uncommitted, ";
    if (dbInf.txnIsolationOptions & SQL_TXN_READ_COMMITTED)
        cout << "Read Committed, ";
    if (dbInf.txnIsolationOptions & SQL_TXN_REPEATABLE_READ)
        cout << "Repeatable Read, ";
    if (dbInf.txnIsolationOptions & SQL_TXN_SERIALIZABLE)
        cout << "Serializable, ";
#ifdef ODBC_V20
    if (dbInf.txnIsolationOptions & SQL_TXN_VERSIONING)
        cout << "Versioning";
#endif
    cout << endl;

    cout << "Fetch Directions Supported:" << endl << "   ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_NEXT)
        cout << "Next, ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_PRIOR)
        cout << "Prev, ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_FIRST)
        cout << "First, ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_LAST)
        cout << "Last, ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_ABSOLUTE)
        cout << "Absolute, ";
    if (dbInf.fetchDirections & SQL_FD_FETCH_RELATIVE)
        cout << "Relative, ";
#ifdef ODBC_V20
    if (dbInf.fetchDirections & SQL_FD_FETCH_RESUME)
        cout << "Resume, ";
#endif
    if (dbInf.fetchDirections & SQL_FD_FETCH_BOOKMARK)
        cout << "Bookmark";
    cout << endl;

    cout << "Lock Types Supported (SQLSetPos): ";
    if (dbInf.lockTypes & SQL_LCK_NO_CHANGE)
        cout << "No Change, ";
    if (dbInf.lockTypes & SQL_LCK_EXCLUSIVE)
        cout << "Exclusive, ";
    if (dbInf.lockTypes & SQL_LCK_UNLOCK)
        cout << "UnLock";
    cout << endl;

    cout << "Position Operations Supported (SQLSetPos): ";
    if (dbInf.posOperations & SQL_POS_POSITION)
        cout << "Position, ";
    if (dbInf.posOperations & SQL_POS_REFRESH)
        cout << "Refresh, ";
    if (dbInf.posOperations & SQL_POS_UPDATE)
        cout << "Upd, ";
    if (dbInf.posOperations & SQL_POS_DELETE)
        cout << "Del, ";
    if (dbInf.posOperations & SQL_POS_ADD)
        cout << "Add";
    cout << endl;

    cout << "Positioned Statements Supported: ";
    if (dbInf.posStmts & SQL_PS_POSITIONED_DELETE)
        cout << "Pos delete, ";
    if (dbInf.posStmts & SQL_PS_POSITIONED_UPDATE)
        cout << "Pos update, ";
    if (dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE)
        cout << "Select for update";
    cout << endl;

    cout << "Scroll Concurrency: ";
    if (dbInf.scrollConcurrency & SQL_SCCO_READ_ONLY)
        cout << "Read Only, ";
    if (dbInf.scrollConcurrency & SQL_SCCO_LOCK)
        cout << "Lock, ";
    if (dbInf.scrollConcurrency & SQL_SCCO_OPT_ROWVER)
        cout << "Opt. Rowver, ";
    if (dbInf.scrollConcurrency & SQL_SCCO_OPT_VALUES)
        cout << "Opt. Values";
    cout << endl;

    cout << "Scroll Options: ";
    if (dbInf.scrollOptions & SQL_SO_FORWARD_ONLY)
        cout << "Fwd Only, ";
    if (dbInf.scrollOptions & SQL_SO_STATIC)
        cout << "Static, ";
    if (dbInf.scrollOptions & SQL_SO_KEYSET_DRIVEN)
        cout << "Keyset Driven, ";
    if (dbInf.scrollOptions & SQL_SO_DYNAMIC)
        cout << "Dynamic, ";
    if (dbInf.scrollOptions & SQL_SO_MIXED)
        cout << "Mixed";
    cout << endl;

    cout << "Static Sensitivity: ";
    if (dbInf.staticSensitivity & SQL_SS_ADDITIONS)
        cout << "Additions, ";
    if (dbInf.staticSensitivity & SQL_SS_DELETIONS)
        cout << "Deletions, ";
    if (dbInf.staticSensitivity & SQL_SS_UPDATES)
        cout << "Updates";
    cout << endl;

    cout << "Transaction Capable?: ";
    switch(dbInf.txnCapable)
    {
        case SQL_TC_NONE:          cout << "No";            break;
        case SQL_TC_DML:           cout << "DML Only";      break;
        case SQL_TC_DDL_COMMIT:    cout << "DDL Commit";    break;
        case SQL_TC_DDL_IGNORE:    cout << "DDL Ignore";    break;
        case SQL_TC_ALL:           cout << "DDL & DML";     break;
    }
    cout << endl;

    cout << endl;
#endif

    // Completed Successfully
    return(TRUE);

} // wxDB::getDbInfo()


/********** wxDB::getDataTypeInfo() **********/
bool wxDB::getDataTypeInfo(SWORD fSqlType, SqlTypeInfo &structSQLTypeInfo)
{
/*
 * fSqlType will be something like SQL_VARCHAR.  This parameter determines
 * the data type inf. is gathered for.
 *
 * SqlTypeInfo is a structure that is filled in with data type information,
 */
    RETCODE retcode;
    SDWORD cbRet;

    // Get information about the data type specified
    if (SQLGetTypeInfo(hstmt, fSqlType) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));
    // Fetch the record
    if ((retcode = SQLFetch(hstmt)) != SQL_SUCCESS)
    {
#ifdef DBDEBUG_CONSOLE
        if (retcode == SQL_NO_DATA_FOUND)
            cout << "SQL_NO_DATA_FOUND fetching inf. about data type." << endl;
#endif
        DispAllErrors(henv, hdbc, hstmt);
        SQLFreeStmt(hstmt, SQL_CLOSE);
        return(FALSE);
    }
    // Obtain columns from the record
    if (SQLGetData(hstmt, 1, SQL_C_CHAR, (UCHAR*) structSQLTypeInfo.TypeName, DB_TYPE_NAME_LEN, &cbRet) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));

    // BJO 991209
    if (Dbms() == dbmsMY_SQL)
    {
        if (!strcmp(structSQLTypeInfo.TypeName, "middleint")) strcpy(structSQLTypeInfo.TypeName, "mediumint");
        if (!strcmp(structSQLTypeInfo.TypeName, "middleint unsigned")) strcpy(structSQLTypeInfo.TypeName, "mediumint unsigned");
        if (!strcmp(structSQLTypeInfo.TypeName, "integer")) strcpy(structSQLTypeInfo.TypeName, "int");
        if (!strcmp(structSQLTypeInfo.TypeName, "integer unsigned")) strcpy(structSQLTypeInfo.TypeName, "int unsigned");
        if (!strcmp(structSQLTypeInfo.TypeName, "middleint")) strcpy(structSQLTypeInfo.TypeName, "mediumint");
        if (!strcmp(structSQLTypeInfo.TypeName, "varchar")) strcpy(structSQLTypeInfo.TypeName, "char");
    }

    if (SQLGetData(hstmt, 3, SQL_C_LONG, (UCHAR*) &structSQLTypeInfo.Precision, 0, &cbRet) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));
    if (SQLGetData(hstmt, 8, SQL_C_SHORT, (UCHAR*) &structSQLTypeInfo.CaseSensitive, 0, &cbRet) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));
//    if (SQLGetData(hstmt, 14, SQL_C_SHORT, (UCHAR*) &structSQLTypeInfo.MinimumScale, 0, &cbRet) != SQL_SUCCESS)
//        return(DispAllErrors(henv, hdbc, hstmt));

    if (SQLGetData(hstmt, 15, SQL_C_SHORT,(UCHAR*)  &structSQLTypeInfo.MaximumScale, 0, &cbRet) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));

    if (structSQLTypeInfo.MaximumScale < 0)
        structSQLTypeInfo.MaximumScale = 0;

    // Close the statement handle which closes open cursors
    if (SQLFreeStmt(hstmt, SQL_CLOSE) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));

    // Completed Successfully
    return(TRUE);

} // wxDB::getDataTypeInfo()


/********** wxDB::Close() **********/
void wxDB::Close(void)
{
    // Close the Sql Log file
    if (fpSqlLog)
    {
        fclose(fpSqlLog);
        fpSqlLog = 0;
    }

    // Free statement handle
    if (dbIsOpen)
    {
        if (SQLFreeStmt(hstmt, SQL_DROP) != SQL_SUCCESS)
            DispAllErrors(henv, hdbc);
    }

    // Disconnect from the datasource
    if (SQLDisconnect(hdbc) != SQL_SUCCESS)
        DispAllErrors(henv, hdbc);

    // Free the connection to the datasource
    if (SQLFreeConnect(hdbc) != SQL_SUCCESS)
        DispAllErrors(henv, hdbc);

    // There should be zero Ctable objects still connected to this db object
    assert(nTables == 0);

#ifdef __WXDEBUG__
    CstructTablesInUse *tiu;
    wxNode *pNode;
    pNode = TablesInUse.First();
    wxString s,s2;
    while (pNode)
    {
        tiu = (CstructTablesInUse *)pNode->Data();
        if (tiu->pDb == this)
        {
            s.sprintf("(%-20s)     tableID:[%6lu]     pDb:[%p]", tiu->tableName,tiu->tableID,tiu->pDb);
            s2.sprintf("Orphaned found using pDb:[%p]",this);
            wxMessageBox (s,s2);
        }
        pNode = pNode->Next();
    }
#endif

    // Copy the error messages to a global variable
    int i;
    for (i = 0; i < DB_MAX_ERROR_HISTORY; i++)
        wxStrcpy(DBerrorList[i],errorList[i]);

} // wxDB::Close()


/********** wxDB::CommitTrans() **********/
bool wxDB::CommitTrans(void)
{
    if (this)
    {
        // Commit the transaction
        if (SQLTransact(henv, hdbc, SQL_COMMIT) != SQL_SUCCESS)
            return(DispAllErrors(henv, hdbc));
    }

    // Completed successfully
    return(TRUE);

} // wxDB::CommitTrans()


/********** wxDB::RollbackTrans() **********/
bool wxDB::RollbackTrans(void)
{
    // Rollback the transaction
    if (SQLTransact(henv, hdbc, SQL_ROLLBACK) != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc));

    // Completed successfully
    return(TRUE);

} // wxDB::RollbackTrans()


/********** wxDB::DispAllErrors() **********/
bool wxDB::DispAllErrors(HENV aHenv, HDBC aHdbc, HSTMT aHstmt)
{
//    char odbcErrMsg[DB_MAX_ERROR_MSG_LEN];
    wxString odbcErrMsg;

    while (SQLError(aHenv, aHdbc, aHstmt, (UCHAR FAR *) sqlState, &nativeError, (UCHAR FAR *) errorMsg, SQL_MAX_MESSAGE_LENGTH - 1, &cbErrorMsg) == SQL_SUCCESS)
    {
        odbcErrMsg.sprintf("SQL State = %s\nNative Error Code = %li\nError Message = %s\n", sqlState, nativeError, errorMsg);
        logError(odbcErrMsg.GetData(), sqlState);
        if (!silent)
        {
#ifdef DBDEBUG_CONSOLE
            // When run in console mode, use standard out to display errors.
            cout << odbcErrMsg.GetData() << endl;
            cout << "Press any key to continue..." << endl;
            getchar();
#endif
        }

#ifdef __WXDEBUG__
        wxMessageBox(odbcErrMsg.GetData(),"DEBUG MESSAGE from DispAllErrors()");
#endif
    }

    return(FALSE);  // This function always returns false.

} // wxDB::DispAllErrors()


/********** wxDB::GetNextError() **********/
bool wxDB::GetNextError(HENV aHenv, HDBC aHdbc, HSTMT aHstmt)
{
    if (SQLError(aHenv, aHdbc, aHstmt, (UCHAR FAR *) sqlState, &nativeError, (UCHAR FAR *) errorMsg, SQL_MAX_MESSAGE_LENGTH - 1, &cbErrorMsg) == SQL_SUCCESS)
        return(TRUE);
    else
        return(FALSE);

} // wxDB::GetNextError()


/********** wxDB::DispNextError() **********/
void wxDB::DispNextError(void)
{
//    char odbcErrMsg[DB_MAX_ERROR_MSG_LEN];
    wxString odbcErrMsg;

    odbcErrMsg.sprintf("SQL State = %s\nNative Error Code = %li\nError Message = %s\n", sqlState, nativeError, errorMsg);
    logError(odbcErrMsg.GetData(), sqlState);

    if (silent)
        return;

#ifdef DBDEBUG_CONSOLE
    // When run in console mode, use standard out to display errors.
    cout << odbcErrMsg.GetData() << endl;
    cout << "Press any key to continue..."  << endl;
    getchar();
#endif

} // wxDB::DispNextError()


/********** wxDB::logError() **********/
void wxDB::logError(const char *errMsg, const char *SQLState)
{
    assert(errMsg && wxStrlen(errMsg));

    static int pLast = -1;
    int dbStatus;

    if (++pLast == DB_MAX_ERROR_HISTORY)
    {
        int i;
        for (i = 0; i < DB_MAX_ERROR_HISTORY; i++)
            wxStrcpy(errorList[i], errorList[i+1]);
        pLast--;
    }

    wxStrcpy(errorList[pLast], errMsg);

    if (SQLState && wxStrlen(SQLState))
        if ((dbStatus = TranslateSqlState(SQLState)) != DB_ERR_FUNCTION_SEQUENCE_ERROR)
            DB_STATUS = dbStatus;

    // Add the errmsg to the sql log
    WriteSqlLog(errMsg);

}  // wxDB::logError()


/**********wxDB::TranslateSqlState()  **********/
int wxDB::TranslateSqlState(const char *SQLState)
{
    if (!wxStrcmp(SQLState, "01000"))
        return(DB_ERR_GENERAL_WARNING);
    if (!wxStrcmp(SQLState, "01002"))
        return(DB_ERR_DISCONNECT_ERROR);
    if (!wxStrcmp(SQLState, "01004"))
        return(DB_ERR_DATA_TRUNCATED);
    if (!wxStrcmp(SQLState, "01006"))
        return(DB_ERR_PRIV_NOT_REVOKED);
    if (!wxStrcmp(SQLState, "01S00"))
        return(DB_ERR_INVALID_CONN_STR_ATTR);
    if (!wxStrcmp(SQLState, "01S01"))
        return(DB_ERR_ERROR_IN_ROW);
    if (!wxStrcmp(SQLState, "01S02"))
        return(DB_ERR_OPTION_VALUE_CHANGED);
    if (!wxStrcmp(SQLState, "01S03"))
        return(DB_ERR_NO_ROWS_UPD_OR_DEL);
    if (!wxStrcmp(SQLState, "01S04"))
        return(DB_ERR_MULTI_ROWS_UPD_OR_DEL);
    if (!wxStrcmp(SQLState, "07001"))
        return(DB_ERR_WRONG_NO_OF_PARAMS);
    if (!wxStrcmp(SQLState, "07006"))
        return(DB_ERR_DATA_TYPE_ATTR_VIOL);
    if (!wxStrcmp(SQLState, "08001"))
        return(DB_ERR_UNABLE_TO_CONNECT);
    if (!wxStrcmp(SQLState, "08002"))
        return(DB_ERR_CONNECTION_IN_USE);
    if (!wxStrcmp(SQLState, "08003"))
        return(DB_ERR_CONNECTION_NOT_OPEN);
    if (!wxStrcmp(SQLState, "08004"))
        return(DB_ERR_REJECTED_CONNECTION);
    if (!wxStrcmp(SQLState, "08007"))
        return(DB_ERR_CONN_FAIL_IN_TRANS);
    if (!wxStrcmp(SQLState, "08S01"))
        return(DB_ERR_COMM_LINK_FAILURE);
    if (!wxStrcmp(SQLState, "21S01"))
        return(DB_ERR_INSERT_VALUE_LIST_MISMATCH);
    if (!wxStrcmp(SQLState, "21S02"))
        return(DB_ERR_DERIVED_TABLE_MISMATCH);
    if (!wxStrcmp(SQLState, "22001"))
        return(DB_ERR_STRING_RIGHT_TRUNC);
    if (!wxStrcmp(SQLState, "22003"))
        return(DB_ERR_NUMERIC_VALUE_OUT_OF_RNG);
    if (!wxStrcmp(SQLState, "22005"))
        return(DB_ERR_ERROR_IN_ASSIGNMENT);
    if (!wxStrcmp(SQLState, "22008"))
        return(DB_ERR_DATETIME_FLD_OVERFLOW);
    if (!wxStrcmp(SQLState, "22012"))
        return(DB_ERR_DIVIDE_BY_ZERO);
    if (!wxStrcmp(SQLState, "22026"))
        return(DB_ERR_STR_DATA_LENGTH_MISMATCH);
    if (!wxStrcmp(SQLState, "23000"))
        return(DB_ERR_INTEGRITY_CONSTRAINT_VIOL);
    if (!wxStrcmp(SQLState, "24000"))
        return(DB_ERR_INVALID_CURSOR_STATE);
    if (!wxStrcmp(SQLState, "25000"))
        return(DB_ERR_INVALID_TRANS_STATE);
    if (!wxStrcmp(SQLState, "28000"))
        return(DB_ERR_INVALID_AUTH_SPEC);
    if (!wxStrcmp(SQLState, "34000"))
        return(DB_ERR_INVALID_CURSOR_NAME);
    if (!wxStrcmp(SQLState, "37000"))
        return(DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL);
    if (!wxStrcmp(SQLState, "3C000"))
        return(DB_ERR_DUPLICATE_CURSOR_NAME);
    if (!wxStrcmp(SQLState, "40001"))
        return(DB_ERR_SERIALIZATION_FAILURE);
    if (!wxStrcmp(SQLState, "42000"))
        return(DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL2);
    if (!wxStrcmp(SQLState, "70100"))
        return(DB_ERR_OPERATION_ABORTED);
    if (!wxStrcmp(SQLState, "IM001"))
        return(DB_ERR_UNSUPPORTED_FUNCTION);
    if (!wxStrcmp(SQLState, "IM002"))
        return(DB_ERR_NO_DATA_SOURCE);
    if (!wxStrcmp(SQLState, "IM003"))
        return(DB_ERR_DRIVER_LOAD_ERROR);
    if (!wxStrcmp(SQLState, "IM004"))
        return(DB_ERR_SQLALLOCENV_FAILED);
    if (!wxStrcmp(SQLState, "IM005"))
        return(DB_ERR_SQLALLOCCONNECT_FAILED);
    if (!wxStrcmp(SQLState, "IM006"))
        return(DB_ERR_SQLSETCONNECTOPTION_FAILED);
    if (!wxStrcmp(SQLState, "IM007"))
        return(DB_ERR_NO_DATA_SOURCE_DLG_PROHIB);
    if (!wxStrcmp(SQLState, "IM008"))
        return(DB_ERR_DIALOG_FAILED);
    if (!wxStrcmp(SQLState, "IM009"))
        return(DB_ERR_UNABLE_TO_LOAD_TRANSLATION_DLL);
    if (!wxStrcmp(SQLState, "IM010"))
        return(DB_ERR_DATA_SOURCE_NAME_TOO_LONG);
    if (!wxStrcmp(SQLState, "IM011"))
        return(DB_ERR_DRIVER_NAME_TOO_LONG);
    if (!wxStrcmp(SQLState, "IM012"))
        return(DB_ERR_DRIVER_KEYWORD_SYNTAX_ERROR);
    if (!wxStrcmp(SQLState, "IM013"))
        return(DB_ERR_TRACE_FILE_ERROR);
    if (!wxStrcmp(SQLState, "S0001"))
        return(DB_ERR_TABLE_OR_VIEW_ALREADY_EXISTS);
    if (!wxStrcmp(SQLState, "S0002"))
        return(DB_ERR_TABLE_NOT_FOUND);
    if (!wxStrcmp(SQLState, "S0011"))
        return(DB_ERR_INDEX_ALREADY_EXISTS);
    if (!wxStrcmp(SQLState, "S0012"))
        return(DB_ERR_INDEX_NOT_FOUND);
    if (!wxStrcmp(SQLState, "S0021"))
        return(DB_ERR_COLUMN_ALREADY_EXISTS);
    if (!wxStrcmp(SQLState, "S0022"))
        return(DB_ERR_COLUMN_NOT_FOUND);
    if (!wxStrcmp(SQLState, "S0023"))
        return(DB_ERR_NO_DEFAULT_FOR_COLUMN);
    if (!wxStrcmp(SQLState, "S1000"))
        return(DB_ERR_GENERAL_ERROR);
    if (!wxStrcmp(SQLState, "S1001"))
        return(DB_ERR_MEMORY_ALLOCATION_FAILURE);
    if (!wxStrcmp(SQLState, "S1002"))
        return(DB_ERR_INVALID_COLUMN_NUMBER);
    if (!wxStrcmp(SQLState, "S1003"))
        return(DB_ERR_PROGRAM_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1004"))
        return(DB_ERR_SQL_DATA_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1008"))
        return(DB_ERR_OPERATION_CANCELLED);
    if (!wxStrcmp(SQLState, "S1009"))
        return(DB_ERR_INVALID_ARGUMENT_VALUE);
    if (!wxStrcmp(SQLState, "S1010"))
        return(DB_ERR_FUNCTION_SEQUENCE_ERROR);
    if (!wxStrcmp(SQLState, "S1011"))
        return(DB_ERR_OPERATION_INVALID_AT_THIS_TIME);
    if (!wxStrcmp(SQLState, "S1012"))
        return(DB_ERR_INVALID_TRANS_OPERATION_CODE);
    if (!wxStrcmp(SQLState, "S1015"))
        return(DB_ERR_NO_CURSOR_NAME_AVAIL);
    if (!wxStrcmp(SQLState, "S1090"))
        return(DB_ERR_INVALID_STR_OR_BUF_LEN);
    if (!wxStrcmp(SQLState, "S1091"))
        return(DB_ERR_DESCRIPTOR_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1092"))
        return(DB_ERR_OPTION_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1093"))
        return(DB_ERR_INVALID_PARAM_NO);
    if (!wxStrcmp(SQLState, "S1094"))
        return(DB_ERR_INVALID_SCALE_VALUE);
    if (!wxStrcmp(SQLState, "S1095"))
        return(DB_ERR_FUNCTION_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1096"))
        return(DB_ERR_INF_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1097"))
        return(DB_ERR_COLUMN_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1098"))
        return(DB_ERR_SCOPE_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1099"))
        return(DB_ERR_NULLABLE_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1100"))
        return(DB_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1101"))
        return(DB_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1103"))
        return(DB_ERR_DIRECTION_OPTION_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1104"))
        return(DB_ERR_INVALID_PRECISION_VALUE);
    if (!wxStrcmp(SQLState, "S1105"))
        return(DB_ERR_INVALID_PARAM_TYPE);
    if (!wxStrcmp(SQLState, "S1106"))
        return(DB_ERR_FETCH_TYPE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1107"))
        return(DB_ERR_ROW_VALUE_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1108"))
        return(DB_ERR_CONCURRENCY_OPTION_OUT_OF_RANGE);
    if (!wxStrcmp(SQLState, "S1109"))
        return(DB_ERR_INVALID_CURSOR_POSITION);
    if (!wxStrcmp(SQLState, "S1110"))
        return(DB_ERR_INVALID_DRIVER_COMPLETION);
    if (!wxStrcmp(SQLState, "S1111"))
        return(DB_ERR_INVALID_BOOKMARK_VALUE);
    if (!wxStrcmp(SQLState, "S1C00"))
        return(DB_ERR_DRIVER_NOT_CAPABLE);
    if (!wxStrcmp(SQLState, "S1T00"))
        return(DB_ERR_TIMEOUT_EXPIRED);

    // No match
    return(0);

}  // wxDB::TranslateSqlState()


/**********  wxDB::Grant() **********/
bool wxDB::Grant(int privileges, const char *tableName, const char *userList)
{
//    char sqlStmt[DB_MAX_STATEMENT_LEN];
    wxString sqlStmt;

    // Build the grant statement
    sqlStmt  = "GRANT ";
    if (privileges == DB_GRANT_ALL)
        sqlStmt += "ALL";
    else
    {
        int c = 0;
        if (privileges & DB_GRANT_SELECT)
        {
            sqlStmt += "SELECT";
            c++;
        }
        if (privileges & DB_GRANT_INSERT)
        {
            if (c++)
                sqlStmt += ", ";
            sqlStmt += "INSERT";
        }
        if (privileges & DB_GRANT_UPDATE)
        {
            if (c++)
                sqlStmt += ", ";
            sqlStmt += "UPDATE";
        }
        if (privileges & DB_GRANT_DELETE)
        {
            if (c++)
                sqlStmt += ", ";
            sqlStmt += "DELETE";
        }
    }

    sqlStmt += " ON ";
    sqlStmt += tableName;
    sqlStmt += " TO ";
    sqlStmt += userList;

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.GetData() << endl;
#endif

    WriteSqlLog(sqlStmt.GetData());

    return(ExecSql(sqlStmt.GetData()));

}  // wxDB::Grant()


/********** wxDB::CreateView() **********/
bool wxDB::CreateView(const char *viewName, const char *colList, const char *pSqlStmt, bool attemptDrop)
{
//    char sqlStmt[DB_MAX_STATEMENT_LEN];
    wxString sqlStmt;

    // Drop the view first
    if (attemptDrop && !DropView(viewName))
        return FALSE;

    // Build the create view statement
    sqlStmt  = "CREATE VIEW ";
    sqlStmt += viewName;

    if (wxStrlen(colList))
    {
        sqlStmt += " (";
        sqlStmt += colList;
        sqlStmt += ")";
    }

    sqlStmt += " AS ";
    sqlStmt += pSqlStmt;

    WriteSqlLog(sqlStmt.GetData());

#ifdef DBDEBUG_CONSOLE
    cout << sqlStmt.GetData() << endl;
#endif

    return(ExecSql(sqlStmt.GetData()));

}  // wxDB::CreateView()


/********** wxDB::DropView()  **********/
bool wxDB::DropView(const char *viewName)
{
/*
 * NOTE: This function returns TRUE if the View does not exist, but
 *       only for identified databases.  Code will need to be added
 *            below for any other databases when those databases are defined
 *       to handle this situation consistently
 */
//    char sqlStmt[DB_MAX_STATEMENT_LEN];
    wxString sqlStmt;

    sqlStmt.sprintf("DROP VIEW %s", viewName);

    WriteSqlLog(sqlStmt.GetData());

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.GetData() << endl;
#endif

    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.GetData(), SQL_NTS) != SQL_SUCCESS)
    {
        // Check for "Base table not found" error and ignore
        GetNextError(henv, hdbc, hstmt);
        if (wxStrcmp(sqlState,"S0002"))  // "Base table not found"
        {
            // Check for product specific error codes
            if (!((Dbms() == dbmsSYBASE_ASA    && !wxStrcmp(sqlState,"42000"))))  // 5.x (and lower?)
            {
                DispNextError();
                DispAllErrors(henv, hdbc, hstmt);
                RollbackTrans();
                return(FALSE);
            }
        }
    }

    // Commit the transaction
    if (! CommitTrans())
        return(FALSE);

    return TRUE;

}  // wxDB::DropView()


/********** wxDB::ExecSql()  **********/
bool wxDB::ExecSql(const char *pSqlStmt)
{
    SQLFreeStmt(hstmt, SQL_CLOSE);
    if (SQLExecDirect(hstmt, (UCHAR FAR *) pSqlStmt, SQL_NTS) == SQL_SUCCESS)
        return(TRUE);
    else
    {
        DispAllErrors(henv, hdbc, hstmt);
        return(FALSE);
    }

}  // wxDB::ExecSql()


/********** wxDB::GetNext()  **********/
bool wxDB::GetNext(void)
{
    if (SQLFetch(hstmt) == SQL_SUCCESS)
        return(TRUE);
    else
    {
        DispAllErrors(henv, hdbc, hstmt);
        return(FALSE);
    }

}  // wxDB::GetNext()


/********** wxDB::GetData()  **********/
bool wxDB::GetData(UWORD colNo, SWORD cType, PTR pData, SDWORD maxLen, SDWORD FAR *cbReturned)
{
    assert(pData);
    assert(cbReturned);

    if (SQLGetData(hstmt, colNo, cType, pData, maxLen, cbReturned) == SQL_SUCCESS)
        return(TRUE);
    else
    {
        DispAllErrors(henv, hdbc, hstmt);
        return(FALSE);
    }

}  // wxDB::GetData()


/********** wxDB::GetKeyFields() **********/
int wxDB::GetKeyFields(char *tableName, wxColInf* colInf,int noCols)
{
    char         szPkTable[DB_MAX_TABLE_NAME_LEN+1];  /* Primary key table name */
    char         szFkTable[DB_MAX_TABLE_NAME_LEN+1];  /* Foreign key table name */
    short        iKeySeq;
//    SQLSMALLINT  iKeySeq;
    char         szPkCol[DB_MAX_COLUMN_NAME_LEN+1];   /* Primary key column     */
    char         szFkCol[DB_MAX_COLUMN_NAME_LEN+1];   /* Foreign key column     */
    SQLRETURN    retcode;
    SDWORD       cb;
    int          i;
    wxString     Temp0;
    /*
     * ---------------------------------------------------------------------
     * -- 19991224 : mj10777@gmx.net : Create                         ------
     * --          : Three things are done and stored here :          ------
     * --          : 1) which Column(s) is/are Primary Key(s)         ------
     * --          : 2) which tables use this Key as a Foreign Key    ------
     * --          : 3) which columns are Foreign Key and the name    ------
     * --          :     of the Table where the Key is the Primary Key -----
     * --          : Called from GetColumns(char *tableName,          ------
     * --                           int *numCols,const char *userID ) ------
     * ---------------------------------------------------------------------
     */

    /*---------------------------------------------------------------------*/
    /* Get the names of the columns in the primary key.                    */
    /*---------------------------------------------------------------------*/
    retcode = SQLPrimaryKeys(hstmt,
                             NULL, 0,                       /* Catalog name  */
                             NULL, 0,                       /* Schema name   */
                             (UCHAR *) tableName, SQL_NTS); /* Table name    */

    /*---------------------------------------------------------------------*/
    /* Fetch and display the result set. This will be a list of the        */
    /* columns in the primary key of the tableName table.                  */
    /*---------------------------------------------------------------------*/
    while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    {
        retcode = SQLFetch(hstmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
        {
            GetData( 4, SQL_C_CHAR,   szPkCol,     DB_MAX_COLUMN_NAME_LEN+1, &cb);
            GetData( 5, SQL_C_SSHORT, &iKeySeq,    0,                        &cb);
            //-------
            for (i=0;i<noCols;i++)                          // Find the Column name
                if (!wxStrcmp(colInf[i].colName,szPkCol))   // We have found the Column
                    colInf[i].PkCol = iKeySeq;              // Which Primary Key is this (first, second usw.) ?
        }   // if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
    }    // while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    SQLFreeStmt(hstmt, SQL_CLOSE);  /* Close the cursor (the hstmt is still allocated).      */

    /*---------------------------------------------------------------------*/
    /* Get all the foreign keys that refer to tableName primary key.       */
    /*---------------------------------------------------------------------*/
    retcode = SQLForeignKeys(hstmt,
                             NULL, 0,                       /* Primary catalog */
                             NULL, 0,                       /* Primary schema  */
                             (UCHAR *)tableName, SQL_NTS,   /* Primary table   */
                             NULL, 0,                       /* Foreign catalog */
                             NULL, 0,                       /* Foreign schema  */
                             NULL, 0);                      /* Foreign table   */

    /*---------------------------------------------------------------------*/
    /* Fetch and display the result set. This will be all of the foreign   */
    /* keys in other tables that refer to the tableName  primary key.      */
    /*---------------------------------------------------------------------*/
    Temp0.Empty();
    szPkCol[0] = 0;
    while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    {
        retcode = SQLFetch(hstmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
        {
            GetData( 3, SQL_C_CHAR,   szPkTable,   DB_MAX_TABLE_NAME_LEN+1,  &cb);
            GetData( 4, SQL_C_CHAR,   szPkCol,     DB_MAX_COLUMN_NAME_LEN+1, &cb);
            GetData( 5, SQL_C_SSHORT, &iKeySeq,    0,                        &cb);
            GetData( 7, SQL_C_CHAR,   szFkTable,   DB_MAX_TABLE_NAME_LEN+1,  &cb);
            GetData( 8, SQL_C_CHAR,   szFkCol,     DB_MAX_COLUMN_NAME_LEN+1, &cb);
            Temp0.Printf("%s[%s] ",Temp0.c_str(),szFkTable);  // [ ] in case there is a blank in the Table name
        }  // if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
    }   // while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    Temp0.Trim();     // Get rid of any unneeded blanks
    if (Temp0 != "")
    {
        for (i=0;i<noCols;i++)                          // Find the Column name
            if (!wxStrcmp(colInf[i].colName,szPkCol))   // We have found the Column, store the Information
                strcpy(colInf[i].PkTableName,Temp0);    // Name of the Tables where this Primary Key is used as a Foreign Key
    }  // if (Temp0 != "")
    SQLFreeStmt(hstmt, SQL_CLOSE);  /* Close the cursor (the hstmt is still allocated). */

    /*---------------------------------------------------------------------*/
    /* Get all the foreign keys in the tablename table.                    */
    /*---------------------------------------------------------------------*/
    retcode = SQLForeignKeys(hstmt,
                             NULL, 0,                        /* Primary catalog   */
                             NULL, 0,                        /* Primary schema    */
                             NULL, 0,                        /* Primary table     */
                             NULL, 0,                        /* Foreign catalog   */
                             NULL, 0,                        /* Foreign schema    */
                             (UCHAR *)tableName, SQL_NTS);   /* Foreign table     */

    /*---------------------------------------------------------------------*/
    /*  Fetch and display the result set. This will be all of the          */
    /*  primary keys in other tables that are referred to by foreign       */
    /*  keys in the tableName table.                                       */
    /*---------------------------------------------------------------------*/
    i = 0;
    while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    {
        retcode = SQLFetch(hstmt);
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
        {
            GetData( 3, SQL_C_CHAR,   szPkTable,   DB_MAX_TABLE_NAME_LEN+1,  &cb);
            GetData( 5, SQL_C_SSHORT, &iKeySeq,    0,                        &cb);
            GetData( 8, SQL_C_CHAR,   szFkCol,     DB_MAX_COLUMN_NAME_LEN+1, &cb);
            //-------
            for (i=0;i<noCols;i++)                              // Find the Column name
            {
                if (!wxStrcmp(colInf[i].colName,szFkCol))       // We have found the (Foreign Key) Column
                {
                    colInf[i].FkCol = iKeySeq;                  // Which Foreign Key is this (first, second usw.) ?
                    strcpy(colInf[i].FkTableName,szPkTable);    // Name of the Table where this Foriegn is the Primary Key
                } // if (!wxStrcmp(colInf[i].colName,szFkCol))
            }  // for (i=0;i<noCols;i++)
        }   // if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
    }    // while ((retcode == SQL_SUCCESS) || (retcode == SQL_SUCCESS_WITH_INFO))
    SQLFreeStmt(hstmt, SQL_CLOSE);  /* Close the cursor (the hstmt is still allocated). */

    /*---------------------------------------------------------------------*/
    return TRUE;
}  // wxDB::GetKeyFields()


/********** wxDB::GetColumns() **********/
wxColInf *wxDB::GetColumns(char *tableName[], const char *userID)
/*
 *        1) The last array element of the tableName[] argument must be zero (null).
 *            This is how the end of the array is detected.
 *        2) This function returns an array of wxColInf structures.  If no columns
 *            were found, or an error occured, this pointer will be zero (null).  THE
 *            CALLING FUNCTION IS RESPONSIBLE FOR DELETING THE MEMORY RETURNED WHEN IT
 *            IS FINISHED WITH IT.  i.e.
 *
 *            wxColInf *colInf = pDb->GetColumns(tableList, userID);
 *            if (colInf)
 *            {
 *                // Use the column inf
 *                .......
 *                // Destroy the memory
 *                delete [] colInf;
 *            }
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 *
 * NOTE: ALL column bindings associated with this wxDB instance are unbound
 *       by this function.  This function should use its own wxDB instance
 *       to avoid undesired unbinding of columns.
 */
{
    int      noCols = 0;
    int      colNo  = 0;
    wxColInf *colInf = 0;

    RETCODE  retcode;
    SDWORD   cb;

    wxString UserID;
    wxString TableName;

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // dBase does not use user names, and some drivers fail if you try to pass one
    if (Dbms() == dbmsDBASE)
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    // Pass 1 - Determine how many columns there are.
    // Pass 2 - Allocate the wxColInf array and fill in
    //                the array with the column information.
    int pass;
    for (pass = 1; pass <= 2; pass++)
    {
        if (pass == 2)
        {
            if (noCols == 0)  // Probably a bogus table name(s)
                break;
            // Allocate n wxColInf objects to hold the column information
            colInf = new wxColInf[noCols+1];
            if (!colInf)
                break;
            // Mark the end of the array
            wxStrcpy(colInf[noCols].tableName, "");
            wxStrcpy(colInf[noCols].colName, "");
            colInf[noCols].sqlDataType = 0;
        }
        // Loop through each table name
        int tbl;
        for (tbl = 0; tableName[tbl]; tbl++)
        {
            TableName = tableName[tbl];
            // Oracle table names are uppercase only, so force
            // the name to uppercase just in case programmer forgot to do this
            if (Dbms() == dbmsORACLE)
                TableName = TableName.Upper();

            SQLFreeStmt(hstmt, SQL_CLOSE);

            // MySQL and Access cannot accept a user name when looking up column names, so we
            // use the call below that leaves out the user name
            if (wxStrcmp(UserID.GetData(),"") &&
                 Dbms() != dbmsMY_SQL &&
                 Dbms() != dbmsACCESS)
            {
                retcode = SQLColumns(hstmt,
                                     NULL, 0,                                // All qualifiers
                                     (UCHAR *) UserID.GetData(), SQL_NTS,    // Owner
                                     (UCHAR *) TableName.GetData(), SQL_NTS,
                                     NULL, 0);                               // All columns
            }
            else
            {
                retcode = SQLColumns(hstmt,
                                     NULL, 0,                                // All qualifiers
                                     NULL, 0,                                // Owner
                                     (UCHAR *) TableName.GetData(), SQL_NTS,
                                     NULL, 0);                               // All columns
            }
            if (retcode != SQL_SUCCESS)
            {  // Error occured, abort
                DispAllErrors(henv, hdbc, hstmt);
                if (colInf)
                    delete [] colInf;
                SQLFreeStmt(hstmt, SQL_CLOSE);
                return(0);
            }

            while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
            {
                if (pass == 1)  // First pass, just add up the number of columns
                    noCols++;
                else  // Pass 2; Fill in the array of structures
                {
                    if (colNo < noCols)  // Some extra error checking to prevent memory overwrites
                    {
                        // NOTE: Only the ODBC 1.x fields are retrieved
                        GetData( 1, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].catalog,      128+1,                    &cb);
                        GetData( 2, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].schema,       128+1,                    &cb);
                        GetData( 3, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].tableName,    DB_MAX_TABLE_NAME_LEN+1,  &cb);
                        GetData( 4, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].colName,      DB_MAX_COLUMN_NAME_LEN+1, &cb);
                        GetData( 5, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].sqlDataType,  0,                        &cb);
                        GetData( 6, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].typeName,     128+1,                    &cb);
                        GetData( 7, SQL_C_SLONG,  (UCHAR*) &colInf[colNo].columnSize,   0,                        &cb);
                        GetData( 8, SQL_C_SLONG,  (UCHAR*) &colInf[colNo].bufferLength, 0,                        &cb);
                        GetData( 9, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].decimalDigits,0,                        &cb);
                        GetData(10, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].numPrecRadix, 0,                        &cb);
                        GetData(11, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].nullable,     0,                        &cb);
                        GetData(12, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].remarks,         254+1,                    &cb);

                        // Determine the wxDB data type that is used to represent the native data type of this data source
                        colInf[colNo].dbDataType = 0;
                        if (!wxStricmp(typeInfVarchar.TypeName,colInf[colNo].typeName))
                            colInf[colNo].dbDataType = DB_DATA_TYPE_VARCHAR;
                        else if (!wxStricmp(typeInfInteger.TypeName,colInf[colNo].typeName))
                            colInf[colNo].dbDataType = DB_DATA_TYPE_INTEGER;
                        else if (!wxStricmp(typeInfFloat.TypeName,colInf[colNo].typeName))
                            colInf[colNo].dbDataType = DB_DATA_TYPE_FLOAT;
                        else if (!wxStricmp(typeInfDate.TypeName,colInf[colNo].typeName))
                            colInf[colNo].dbDataType = DB_DATA_TYPE_DATE;

                        colNo++;
                    }
                }
            }
            if (retcode != SQL_NO_DATA_FOUND)
            {  // Error occured, abort
                DispAllErrors(henv, hdbc, hstmt);
                if (colInf)
                    delete [] colInf;
                SQLFreeStmt(hstmt, SQL_CLOSE);
                return(0);
            }
        }
    }

    SQLFreeStmt(hstmt, SQL_CLOSE);
    return colInf;

}  // wxDB::GetColumns()


/********** wxDB::GetColumns() **********/
wxColInf *wxDB::GetColumns(char *tableName, int *numCols, const char *userID)
/*
 * Same as the above GetColumns() function except this one gets columns
 * only for a single table, and if 'numCols' is not NULL, the number of
 * columns stored in the returned wxColInf is set in '*numCols'
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 *
 * NOTE: ALL column bindings associated with this wxDB instance are unbound
 *       by this function.  This function should use its own wxDB instance
 *       to avoid undesired unbinding of columns.
 */
{
    int       noCols = 0;
    int       colNo  = 0;
    wxColInf *colInf = 0;

    RETCODE  retcode;
    SDWORD   cb;

    wxString UserID;
    wxString TableName;

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // dBase does not use user names, and some drivers fail if you try to pass one
    if (Dbms() == dbmsDBASE)
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    // Pass 1 - Determine how many columns there are.
    // Pass 2 - Allocate the wxColInf array and fill in
    //                the array with the column information.
    int pass;
    for (pass = 1; pass <= 2; pass++)
    {
        if (pass == 2)
        {
            if (noCols == 0)  // Probably a bogus table name(s)
                break;
            // Allocate n wxColInf objects to hold the column information
            colInf = new wxColInf[noCols+1];
            if (!colInf)
                break;
            // Mark the end of the array
            wxStrcpy(colInf[noCols].tableName, "");
            wxStrcpy(colInf[noCols].colName, "");
            colInf[noCols].sqlDataType = 0;
        }

        TableName = tableName;
        // Oracle table names are uppercase only, so force
        // the name to uppercase just in case programmer forgot to do this
        if (Dbms() == dbmsORACLE)
            TableName = TableName.Upper();

        SQLFreeStmt(hstmt, SQL_CLOSE);

        // MySQL and Access cannot accept a user name when looking up column names, so we
        // use the call below that leaves out the user name
        if (wxStrcmp(UserID.GetData(),"") &&
             Dbms() != dbmsMY_SQL &&
             Dbms() != dbmsACCESS)
        {
            retcode = SQLColumns(hstmt,
                                 NULL, 0,                                // All qualifiers
                                 (UCHAR *) UserID.GetData(), SQL_NTS,    // Owner
                                 (UCHAR *) TableName.GetData(), SQL_NTS,
                                 NULL, 0);                               // All columns
        }
        else
        {
            retcode = SQLColumns(hstmt,
                                 NULL, 0,                                 // All qualifiers
                                 NULL, 0,                                 // Owner
                                 (UCHAR *) TableName.GetData(), SQL_NTS,
                                 NULL, 0);                                // All columns
        }
        if (retcode != SQL_SUCCESS)
        {  // Error occured, abort
            DispAllErrors(henv, hdbc, hstmt);
            if (colInf)
                delete [] colInf;
            SQLFreeStmt(hstmt, SQL_CLOSE);
            if (numCols)
                *numCols = 0;
            return(0);
        }

        while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
        {
            if (pass == 1)  // First pass, just add up the number of columns
                noCols++;
            else  // Pass 2; Fill in the array of structures
            {
                if (colNo < noCols)  // Some extra error checking to prevent memory overwrites
                {
                    // NOTE: Only the ODBC 1.x fields are retrieved
                    GetData( 1, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].catalog,      128+1,                    &cb);
                    GetData( 2, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].schema,       128+1,                    &cb);
                    GetData( 3, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].tableName,    DB_MAX_TABLE_NAME_LEN+1,  &cb);
                    GetData( 4, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].colName,      DB_MAX_COLUMN_NAME_LEN+1, &cb);
                    GetData( 5, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].sqlDataType,  0,                        &cb);
                    GetData( 6, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].typeName,     128+1,                    &cb);
                    GetData( 7, SQL_C_SLONG,  (UCHAR*) &colInf[colNo].columnSize,   0,                        &cb);
                    // BJO 991214 : SQL_C_SSHORT instead of SQL_C_SLONG, otherwise fails on Sparc (probably all 64 bit architectures)
                    GetData( 8, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].bufferLength, 0,                        &cb);
                    GetData( 9, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].decimalDigits,0,                        &cb);
                    GetData(10, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].numPrecRadix, 0,                        &cb);
                    GetData(11, SQL_C_SSHORT, (UCHAR*) &colInf[colNo].nullable,     0,                        &cb);
                    GetData(12, SQL_C_CHAR,   (UCHAR*)  colInf[colNo].remarks,      254+1,                    &cb);
                    // Start Values for Primary/Foriegn Key (=No)
                    colInf[colNo].PkCol = 0;           // Primary key column   0=No; 1= First Key, 2 = Second Key etc.
                    colInf[colNo].PkTableName[0] = 0;  // Tablenames where Primary Key is used as a Foreign Key
                    colInf[colNo].FkCol = 0;           // Foreign key column   0=No; 1= First Key, 2 = Second Key etc.
                    colInf[colNo].FkTableName[0] = 0;  // Foreign key table name

                    // Determine the wxDB data type that is used to represent the native data type of this data source
                    colInf[colNo].dbDataType = 0;
                    if (!wxStricmp(typeInfVarchar.TypeName,colInf[colNo].typeName))
                        colInf[colNo].dbDataType = DB_DATA_TYPE_VARCHAR;
                    else if (!wxStricmp(typeInfInteger.TypeName,colInf[colNo].typeName))
                        colInf[colNo].dbDataType = DB_DATA_TYPE_INTEGER;
                    else if (!wxStricmp(typeInfFloat.TypeName,colInf[colNo].typeName))
                        colInf[colNo].dbDataType = DB_DATA_TYPE_FLOAT;
                    else if (!wxStricmp(typeInfDate.TypeName,colInf[colNo].typeName))
                        colInf[colNo].dbDataType = DB_DATA_TYPE_DATE;

                    colNo++;
                }
            }
        }
        if (retcode != SQL_NO_DATA_FOUND)
        {  // Error occured, abort
            DispAllErrors(henv, hdbc, hstmt);
            if (colInf)
                delete [] colInf;
            SQLFreeStmt(hstmt, SQL_CLOSE);
            if (numCols)
                *numCols = 0;
            return(0);
        }
    }

    SQLFreeStmt(hstmt, SQL_CLOSE);

    // Store Primary and Foriegn Keys
    GetKeyFields(tableName,colInf,noCols);

    if (numCols)
        *numCols = noCols;
    return colInf;

}  // wxDB::GetColumns()


/********** wxDB::GetColumnCount() **********/
int wxDB::GetColumnCount(char *tableName, const char *userID)
/*
 * Returns a count of how many columns are in a table.
 * If an error occurs in computing the number of columns
 * this function will return a -1 for the count
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 *
 * NOTE: ALL column bindings associated with this wxDB instance are unbound
 *       by this function.  This function should use its own wxDB instance
 *       to avoid undesired unbinding of columns.
 */
{
    int      noCols = 0;

    RETCODE  retcode;

    wxString UserID;
    wxString TableName;

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // dBase does not use user names, and some drivers fail if you try to pass one
    if (Dbms() == dbmsDBASE)
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    {
        // Loop through each table name
        {
            TableName = tableName;
            // Oracle table names are uppercase only, so force
            // the name to uppercase just in case programmer forgot to do this
            if (Dbms() == dbmsORACLE)
                TableName = TableName.Upper();

            SQLFreeStmt(hstmt, SQL_CLOSE);

            // MySQL and Access cannot accept a user name when looking up column names, so we
            // use the call below that leaves out the user name
            if (wxStrcmp(UserID.GetData(),"") &&
                 Dbms() != dbmsMY_SQL &&
                 Dbms() != dbmsACCESS)
            {
                retcode = SQLColumns(hstmt,
                                     NULL, 0,                                // All qualifiers
                                     (UCHAR *) UserID.GetData(), SQL_NTS,    // Owner
                                     (UCHAR *) TableName.GetData(), SQL_NTS,
                                     NULL, 0);                               // All columns
            }
            else
            {
                retcode = SQLColumns(hstmt,
                                     NULL, 0,                                // All qualifiers
                                     NULL, 0,                                // Owner
                                     (UCHAR *) TableName.GetData(), SQL_NTS,
                                     NULL, 0);                               // All columns
            }
            if (retcode != SQL_SUCCESS)
            {  // Error occured, abort
                DispAllErrors(henv, hdbc, hstmt);
                SQLFreeStmt(hstmt, SQL_CLOSE);
                return(-1);
            }

            // Count the columns
            while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
                noCols++;

            if (retcode != SQL_NO_DATA_FOUND)
            {  // Error occured, abort
                DispAllErrors(henv, hdbc, hstmt);
                SQLFreeStmt(hstmt, SQL_CLOSE);
                return(-1);
            }
        }
    }

    SQLFreeStmt(hstmt, SQL_CLOSE);
    return noCols;

}  // wxDB::GetColumnCount()


/********** wxDB::GetCatalog() *******/
wxDbInf *wxDB::GetCatalog(char *userID)
/*
 * ---------------------------------------------------------------------
 * -- 19991203 : mj10777@gmx.net : Create                         ------
 * --          : Creates a wxDbInf with Tables / Cols Array       ------
 * --          : uses SQLTables and fills pTableInf;              ------
 * --          : pColInf is set to NULL and numCols to 0;         ------
 * --          : returns pDbInf (wxDbInf)                         ------
 * --            - if unsuccesfull (pDbInf == NULL)               ------
 * --          : pColInf can be filled with GetColumns(..);       ------
 * --          : numCols   can be filled with GetColumnCount(..); ------
 * ---------------------------------------------------------------------
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 *
 * NOTE: ALL column bindings associated with this wxDB instance are unbound
 *       by this function.  This function should use its own wxDB instance
 *       to avoid undesired unbinding of columns.
 */
{
    wxDbInf *pDbInf = NULL; // Array of catalog entries
    int      noTab = 0;     // Counter while filling table entries
    int      pass;
    RETCODE  retcode;
    SDWORD   cb;
//    char     tblNameSave[DB_MAX_TABLE_NAME_LEN+1];
    wxString tblNameSave;

    wxString UserID;

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // dBase does not use user names, and some drivers fail if you try to pass one
    if (Dbms() == dbmsDBASE)
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    //-------------------------------------------------------------
    pDbInf = new wxDbInf;          // Create the Database Arrray
    pDbInf->catalog[0]     = 0;
    pDbInf->schema[0]      = 0;
    pDbInf->numTables      = 0;    // Counter for Tables
    pDbInf->pTableInf      = NULL; // Array of Tables
    //-------------------------------------------------------------
    // Table Information
    // Pass 1 - Determine how many Tables there are.
    // Pass 2 - Create the Table array and fill it
    //        - Create the Cols array = NULL
    //-------------------------------------------------------------
    for (pass = 1; pass <= 2; pass++)
    {
        SQLFreeStmt(hstmt, SQL_CLOSE);   // Close if Open
        tblNameSave = "";

        if (wxStrcmp(UserID.GetData(),"") &&
             Dbms() != dbmsMY_SQL &&
             Dbms() != dbmsACCESS)
        {
            retcode = SQLTables(hstmt,
                                NULL, 0,                             // All qualifiers
                                (UCHAR *) UserID.GetData(), SQL_NTS, // User specified
                                NULL, 0,                             // All tables
                                NULL, 0);                            // All columns
        }
        else
        {
            retcode = SQLTables(hstmt,
                                NULL, 0,           // All qualifiers
                                NULL, 0,           // User specified
                                NULL, 0,           // All tables
                                NULL, 0);          // All columns
        }
        if (retcode != SQL_SUCCESS)
        {
            DispAllErrors(henv, hdbc, hstmt);
            pDbInf = NULL;
            SQLFreeStmt(hstmt, SQL_CLOSE);
            return pDbInf;
        }

        while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)   // Table Information
        {
            if (pass == 1)  // First pass, just count the Tables
            {
                if (pDbInf->numTables == 0)
                {
                    GetData( 1, SQL_C_CHAR,   (UCHAR*)  pDbInf->catalog,  128+1, &cb);
                    GetData( 2, SQL_C_CHAR,   (UCHAR*)  pDbInf->schema,   128+1, &cb);
                 }
                 pDbInf->numTables++;      // Counter for Tables
            }  // if (pass == 1)
            if (pass == 2) // Create and fill the Table entries
            {
                if (pDbInf->pTableInf == NULL)   // Has the Table Array been created
                {  // no, then create the Array
                    pDbInf->pTableInf = new wxTableInf[pDbInf->numTables];
                    for (noTab=0;noTab<pDbInf->numTables;noTab++)
                    {
                        (pDbInf->pTableInf+noTab)->tableName[0]    = 0;
                        (pDbInf->pTableInf+noTab)->tableType[0]    = 0;
                        (pDbInf->pTableInf+noTab)->tableRemarks[0] = 0;
                        (pDbInf->pTableInf+noTab)->numCols         = 0;
                        (pDbInf->pTableInf+noTab)->pColInf         = NULL;
                    }
                    noTab = 0;
                } // if (pDbInf->pTableInf == NULL)   // Has the Table Array been created
                GetData( 3, SQL_C_CHAR,   (UCHAR*)  (pDbInf->pTableInf+noTab)->tableName,    DB_MAX_TABLE_NAME_LEN+1, &cb);
                GetData( 4, SQL_C_CHAR,   (UCHAR*)  (pDbInf->pTableInf+noTab)->tableType,    30+1,                    &cb);
                GetData( 5, SQL_C_CHAR,   (UCHAR*)  (pDbInf->pTableInf+noTab)->tableRemarks, 254+1,                   &cb);
                noTab++;
            }  // if (pass == 2)  We now know the amount of Tables
        }   // while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
    }    // for (pass = 1; pass <= 2; pass++)
    SQLFreeStmt(hstmt, SQL_CLOSE);

    // Query how many columns are in each table
    for (noTab=0;noTab<pDbInf->numTables;noTab++)
    {
        (pDbInf->pTableInf+noTab)->numCols = GetColumnCount((pDbInf->pTableInf+noTab)->tableName,UserID);
    }
    return pDbInf;
}  // wxDB::GetCatalog()


/********** wxDB::Catalog() **********/
bool wxDB::Catalog(const char *userID, const char *fileName)
/*
 * Creates the text file specified in 'filename' which will contain
 * a minimal data dictionary of all tables accessible by the user specified
 * in 'userID'
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 *
 * NOTE: ALL column bindings associated with this wxDB instance are unbound
 *       by this function.  This function should use its own wxDB instance
 *       to avoid undesired unbinding of columns.
 */
{
    assert(fileName && wxStrlen(fileName));

    RETCODE   retcode;
    SDWORD    cb;
    char      tblName[DB_MAX_TABLE_NAME_LEN+1];
    wxString  tblNameSave;
    char      colName[DB_MAX_COLUMN_NAME_LEN+1];
    SWORD     sqlDataType;
    char      typeName[30+1];
    SWORD     precision, length;

    wxString UserID;

    FILE *fp = fopen(fileName,"wt");
    if (fp == NULL)
        return(FALSE);

    SQLFreeStmt(hstmt, SQL_CLOSE);

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // dBase does not use user names, and some drivers fail if you try to pass one
    if (Dbms() == dbmsDBASE)
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    if (wxStrcmp(UserID.GetData(),"") &&
         Dbms() != dbmsMY_SQL &&
         Dbms() != dbmsACCESS)
    {
        retcode = SQLColumns(hstmt,
                             NULL, 0,                                // All qualifiers
                             (UCHAR *) UserID.GetData(), SQL_NTS,    // User specified
                             NULL, 0,                                // All tables
                             NULL, 0);                               // All columns
    }
    else
    {
        retcode = SQLColumns(hstmt,
                             NULL, 0,    // All qualifiers
                             NULL, 0,    // User specified
                             NULL, 0,    // All tables
                             NULL, 0);   // All columns
    }
    if (retcode != SQL_SUCCESS)
    {
        DispAllErrors(henv, hdbc, hstmt);
        fclose(fp);
        return(FALSE);
    }

    wxString outStr;
    tblNameSave = "";
    int cnt = 0;

    while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
    {
        if (wxStrcmp(tblName,tblNameSave.GetData()))
        {
            if (cnt)
                fputs("\n", fp);
            fputs("================================ ", fp);
            fputs("================================ ", fp);
            fputs("===================== ", fp);
            fputs("========= ", fp);
            fputs("=========\n", fp);
            outStr.sprintf("%-32s %-32s %-21s %9s %9s\n",
                "TABLE NAME", "COLUMN NAME", "DATA TYPE", "PRECISION", "LENGTH");
            fputs(outStr.GetData(), fp);
            fputs("================================ ", fp);
            fputs("================================ ", fp);
            fputs("===================== ", fp);
            fputs("========= ", fp);
            fputs("=========\n", fp);
            tblNameSave = tblName;
        }

      GetData(3,SQL_C_CHAR,  (UCHAR *)tblName,     DB_MAX_TABLE_NAME_LEN+1, &cb);
      GetData(4,SQL_C_CHAR,  (UCHAR *)colName,     DB_MAX_COLUMN_NAME_LEN+1,&cb);
      GetData(5,SQL_C_SSHORT,(UCHAR *)&sqlDataType,0,                       &cb);
      GetData(6,SQL_C_CHAR,  (UCHAR *)typeName,    sizeof(typeName),        &cb);
      GetData(7,SQL_C_SSHORT,(UCHAR *)&precision,  0,                       &cb);
      GetData(8,SQL_C_SSHORT,(UCHAR *)&length,     0,                       &cb);

        outStr.sprintf("%-32s %-32s (%04d)%-15s %9d %9d\n",
            tblName, colName, sqlDataType, typeName, precision, length);
        if (fputs(outStr.GetData(), fp) == EOF)
        {
            SQLFreeStmt(hstmt, SQL_CLOSE);
            fclose(fp);
            return(FALSE);
        }
        cnt++;
    }

    if (retcode != SQL_NO_DATA_FOUND)
        DispAllErrors(henv, hdbc, hstmt);

    SQLFreeStmt(hstmt, SQL_CLOSE);

    fclose(fp);
    return(retcode == SQL_NO_DATA_FOUND);

}  // wxDB::Catalog()


bool wxDB::TableExists(const char *tableName, const char *userID, const char *tablePath)
/*
 * Table name can refer to a table, view, alias or synonym.  Returns true
 * if the object exists in the database.  This function does not indicate
 * whether or not the user has privleges to query or perform other functions
 * on the table.
 *
 * userID is evaluated in the following manner:
 *        userID == NULL  ... UserID is ignored
 *        userID == ""    ... UserID set equal to 'this->uid'
 *        userID != ""    ... UserID set equal to 'userID'
 */
{
    wxString UserID;
    wxString TableName;

    assert(tableName && wxStrlen(tableName));

    if (Dbms() == dbmsDBASE)
    {
        wxString dbName;
        if (tablePath && wxStrlen(tablePath))
            dbName.sprintf("%s/%s.dbf",tablePath,tableName);
        else
            dbName.sprintf("%s.dbf",tableName);

        bool exists;
        exists = wxFileExists(dbName.GetData());
        return exists;
    }

    if (userID)
    {
        if (!wxStrlen(userID))
            UserID = uid;
        else
            UserID = userID;
    }
    else
        UserID = "";

    // Oracle user names may only be in uppercase, so force
    // the name to uppercase
    if (Dbms() == dbmsORACLE)
        UserID = UserID.Upper();

    TableName = tableName;
    // Oracle table names are uppercase only, so force
    // the name to uppercase just in case programmer forgot to do this
    if (Dbms() == dbmsORACLE)
        TableName = TableName.Upper();

    SQLFreeStmt(hstmt, SQL_CLOSE);
    RETCODE retcode;

    // MySQL and Access cannot accept a user name when looking up table names, so we
    // use the call below that leaves out the user name
    if (wxStrcmp(UserID,"") &&
         Dbms() != dbmsMY_SQL &&
         Dbms() != dbmsACCESS)
    {
        retcode = SQLTables(hstmt,
                            NULL, 0,                                    // All qualifiers
                            (UCHAR *) UserID.GetData(), SQL_NTS,        // All owners
                            (UCHAR FAR *)TableName.GetData(), SQL_NTS,
                            NULL, 0);                                   // All table types
    }
    else
    {
        retcode = SQLTables(hstmt,
                            NULL, 0,                                    // All qualifiers
                            NULL, 0,                                    // All owners
                            (UCHAR FAR *)TableName.GetData(), SQL_NTS,
                            NULL, 0);                                   // All table types
    }
    if (retcode != SQL_SUCCESS)
        return(DispAllErrors(henv, hdbc, hstmt));

    retcode = SQLFetch(hstmt);
    if (retcode  != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
        SQLFreeStmt(hstmt, SQL_CLOSE);
        return(DispAllErrors(henv, hdbc, hstmt));
    }

    SQLFreeStmt(hstmt, SQL_CLOSE);
    return(TRUE);

}  // wxDB::TableExists()


/********** wxDB::SqlLog() **********/
bool wxDB::SqlLog(enum sqlLog state, const char *filename, bool append)
{
    assert(state == sqlLogON  || state == sqlLogOFF);
    assert(state == sqlLogOFF || filename);

    if (state == sqlLogON)
    {
        if (fpSqlLog == 0)
        {
            fpSqlLog = fopen(filename, (append ? "at" : "wt"));
            if (fpSqlLog == NULL)
                return(FALSE);
        }
    }
    else  // sqlLogOFF
    {
        if (fpSqlLog)
        {
            if (fclose(fpSqlLog))
                return(FALSE);
            fpSqlLog = 0;
        }
    }

    sqlLogState = state;
    return(TRUE);

}  // wxDB::SqlLog()


/********** wxDB::WriteSqlLog() **********/
bool wxDB::WriteSqlLog(const char *logMsg)
{
    assert(logMsg);

    if (fpSqlLog == 0 || sqlLogState == sqlLogOFF)
        return(FALSE);

    if (fputs("\n",   fpSqlLog) == EOF) return(FALSE);
    if (fputs(logMsg, fpSqlLog) == EOF) return(FALSE);
    if (fputs("\n",   fpSqlLog) == EOF) return(FALSE);

    return(TRUE);

}  // wxDB::WriteSqlLog()


/********** wxDB::Dbms() **********/
DBMS wxDB::Dbms(void)
/*
 * Be aware that not all database engines use the exact same syntax, and not
 * every ODBC compliant database is compliant to the same level of compliancy.
 * Some manufacturers support the minimum Level 1 compliancy, and others up
 * through Level 3.  Others support subsets of features for levels above 1.
 *
 * If you find an inconsistency between the wxDB class and a specific database
 * engine, and an identifier to this section, and special handle the database in
 * the area where behavior is non-conforming with the other databases.
 *
 *
 * NOTES ABOUT ISSUES SPECIFIC TO EACH DATABASE ENGINE
 * ---------------------------------------------------
 *
 * ORACLE
 *        - Currently the only database supported by the class to support VIEWS
 *
 * DBASE
 *        - Does not support the SQL_TIMESTAMP structure
 *        - Supports only one cursor and one connect (apparently? with Microsoft driver only?)
 *    - Does not automatically create the primary index if the 'keyField' param of SetColDef
 *      is TRUE.  The user must create ALL indexes from their program.
 *        - Table names can only be 8 characters long
 *        - Column names can only be 10 characters long
 *
 * SYBASE (all)
 *        - To lock a record during QUERY functions, the reserved word 'HOLDLOCK' must be added
 *            after every table name involved in the query/join if that tables matching record(s)
 *            are to be locked
 *        - Ignores the keywords 'FOR UPDATE'.  Use the HOLDLOCK functionality described above
 *
 * SYBASE (Enterprise)
 *        - If a column is part of the Primary Key, the column cannot be NULL
 *
 * MY_SQL
 *        - If a column is part of the Primary Key, the column cannot be NULL
 *        - Cannot support selecting for update [::CanSelectForUpdate()].  Always returns FALSE
 *
 * POSTGRES
 *        - Does not support the keywords 'ASC' or 'DESC' as of release v6.5.0
 *
 *
 */
{
    wxChar baseName[25+1];

    wxStrncpy(baseName,dbInf.dbmsName,25);
    if (!wxStricmp(dbInf.dbmsName,"Adaptive Server Anywhere"))
        return(dbmsSYBASE_ASA);
    if (!wxStricmp(dbInf.dbmsName,"SQL Server"))  // Sybase Adaptive Server
        return(dbmsSYBASE_ASE);
    if (!wxStricmp(dbInf.dbmsName,"Microsoft SQL Server"))
        return(dbmsMS_SQL_SERVER);
    if (!wxStricmp(dbInf.dbmsName,"MySQL"))
        return(dbmsMY_SQL);
    if (!wxStricmp(dbInf.dbmsName,"PostgreSQL"))  // v6.5.0
        return(dbmsPOSTGRES);

    baseName[8] = 0;
    if (!wxStricmp(baseName,"Informix"))
        return(dbmsINFORMIX);

    baseName[6] = 0;
    if (!wxStricmp(baseName,"Oracle"))
        return(dbmsORACLE);
    if (!wxStricmp(dbInf.dbmsName,"ACCESS"))
        return(dbmsACCESS);
    if (!wxStricmp(dbInf.dbmsName,"MySQL"))
        return(dbmsMY_SQL);

    baseName[5] = 0;
    if (!wxStricmp(baseName,"DBASE"))
        return(dbmsDBASE);

    return(dbmsUNIDENTIFIED);
}  // wxDB::Dbms()


/********** GetDbConnection() **********/
wxDB WXDLLEXPORT *GetDbConnection(DbStuff *pDbStuff, bool FwdOnlyCursors)
{
    DbList *pList;

    // Scan the linked list searching for an available database connection
    // that's already been opened but is currently not in use.
    for (pList = PtrBegDbList; pList; pList = pList->PtrNext)
    {
        // The database connection must be for the same datasource
        // name and must currently not be in use.
        if (pList->Free && (! wxStrcmp(pDbStuff->Dsn, pList->Dsn)))  // Found a free connection
        {
            pList->Free = FALSE;
            return(pList->PtrDb);
        }
    }

    // No available connections.  A new connection must be made and
    // appended to the end of the linked list.
    if (PtrBegDbList)
    {
        // Find the end of the list
        for (pList = PtrBegDbList; pList->PtrNext; pList = pList->PtrNext);
        // Append a new list item
        pList->PtrNext = new DbList;
        pList->PtrNext->PtrPrev = pList;
        pList = pList->PtrNext;
    }
    else    // Empty list
    {
        // Create the first node on the list
        pList = PtrBegDbList = new DbList;
        pList->PtrPrev = 0;
    }

    // Initialize new node in the linked list
    pList->PtrNext = 0;
    pList->Free = FALSE;
    wxStrcpy(pList->Dsn, pDbStuff->Dsn);
    pList->PtrDb = new wxDB(pDbStuff->Henv,FwdOnlyCursors);

    // Connect to the datasource
    if (pList->PtrDb->Open(pDbStuff->Dsn, pDbStuff->Uid, pDbStuff->AuthStr))
    {
        pList->PtrDb->SqlLog(SQLLOGstate,SQLLOGfn,TRUE);
        return(pList->PtrDb);
    }
    else  // Unable to connect, destroy list item
    {
        if (pList->PtrPrev)
            pList->PtrPrev->PtrNext = 0;
        else
            PtrBegDbList = 0;                // Empty list again
        pList->PtrDb->CommitTrans();    // Commit any open transactions on wxDB object
        pList->PtrDb->Close();            // Close the wxDB object
        delete pList->PtrDb;                // Deletes the wxDB object
        delete pList;                        // Deletes the linked list object
        return(0);
    }

}  // GetDbConnection()


/********** FreeDbConnection() **********/
bool WXDLLEXPORT FreeDbConnection(wxDB *pDb)
{
    DbList *pList;

    // Scan the linked list searching for the database connection
    for (pList = PtrBegDbList; pList; pList = pList->PtrNext)
    {
        if (pList->PtrDb == pDb)        // Found it!!!
            return(pList->Free = TRUE);
    }

    // Never found the database object, return failure
    return(FALSE);

}  // FreeDbConnection()


/********** CloseDbConnections() **********/
void WXDLLEXPORT CloseDbConnections(void)
{
    DbList *pList, *pNext;

    // Traverse the linked list closing database connections and freeing memory as I go.
    for (pList = PtrBegDbList; pList; pList = pNext)
    {
        pNext = pList->PtrNext;       // Save the pointer to next
        pList->PtrDb->CommitTrans();  // Commit any open transactions on wxDB object
        pList->PtrDb->Close();        // Close the wxDB object
        delete pList->PtrDb;          // Deletes the wxDB object
        delete pList;                 // Deletes the linked list object
    }

    // Mark the list as empty
    PtrBegDbList = 0;

}  // CloseDbConnections()


/********** NumberDbConnectionsInUse() **********/
int WXDLLEXPORT NumberDbConnectionsInUse(void)
{
    DbList *pList;
    int cnt = 0;

    // Scan the linked list counting db connections that are currently in use
    for (pList = PtrBegDbList; pList; pList = pList->PtrNext)
    {
        if (pList->Free == FALSE)
            cnt++;
    }

    return(cnt);

}  // NumberDbConnectionsInUse()


/********** SqlLog() **********/
bool SqlLog(enum sqlLog state, char *filename)
{
    bool append = FALSE;
    DbList *pList;

    for (pList = PtrBegDbList; pList; pList = pList->PtrNext)
    {
        if (!pList->PtrDb->SqlLog(state,filename,append))
            return(FALSE);
        append = TRUE;
    }

    SQLLOGstate = state;
    wxStrcpy(SQLLOGfn,filename);

    return(TRUE);

}  // SqlLog()


/********** GetDataSource() **********/
bool GetDataSource(HENV henv, char *Dsn, SWORD DsnMax, char *DsDesc, SWORD DsDescMax,
                         UWORD direction)
/*
 * Dsn and DsDesc will contain the data source name and data source
 * description upon return
 */
{
    SWORD cb1,cb2;

    if (SQLDataSources(henv, direction, (UCHAR FAR *) Dsn, DsnMax, &cb1,
                             (UCHAR FAR *) DsDesc, DsDescMax, &cb2) == SQL_SUCCESS)
        return(TRUE);
    else
        return(FALSE);

}  // GetDataSource()

#endif
 // wxUSE_ODBC

