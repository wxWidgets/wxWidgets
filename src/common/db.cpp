///////////////////////////////////////////////////////////////////////////////
// Name:        db.cpp
// Purpose:     Implementation of the wxDB class.  The wxDB class represents a connection
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
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "db.h"
#endif

/*
// SYNOPSIS START
// SYNOPSIS STOP
*/

/*
#ifdef DBDEBUG_CONSOLE
	#include <iostream.h>
#endif
*/

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
  #include  "wx/string.h"
#endif //WX_PRECOMP

#if wxUSE_ODBC

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "wx/db.h"

DbList* WXDLLEXPORT PtrBegDbList = 0;

/********** wxDB Constructor **********/
wxDB::wxDB(HENV &aHenv)
{
	int i;

	fpSqlLog		= 0;				// Sql Log file pointer
	sqlLogState = sqlLogOFF;	// By default, logging is turned off
	
	strcpy(sqlState,"");
	strcpy(errorMsg,"");
	nativeError = cbErrorMsg = 0;
	for (i = 0; i < DB_MAX_ERROR_HISTORY; i++)
		strcpy(errorList[i], "");

	// Init typeInf structures
	strcpy(typeInfVarchar.TypeName,"");
	typeInfVarchar.FsqlType = 0;
	typeInfVarchar.Precision = 0;
	typeInfVarchar.CaseSensitive = 0;
	typeInfVarchar.MaximumScale = 0;

	strcpy(typeInfInteger.TypeName,"");
	typeInfInteger.FsqlType = 0;
	typeInfInteger.Precision = 0;
	typeInfInteger.CaseSensitive = 0;
	typeInfInteger.MaximumScale = 0;

	strcpy(typeInfFloat.TypeName,"");
	typeInfFloat.FsqlType = 0;
	typeInfFloat.Precision = 0;
	typeInfFloat.CaseSensitive = 0;
	typeInfFloat.MaximumScale = 0;

	strcpy(typeInfDate.TypeName,"");
	typeInfDate.FsqlType = 0;
	typeInfDate.Precision = 0;
	typeInfDate.CaseSensitive = 0;
	typeInfDate.MaximumScale = 0;
	
	// Error reporting is turned OFF by default
	silent = TRUE;
	
	// Copy the HENV into the db class
	henv = aHenv;

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
	assert(Dsn && strlen(Dsn));
	dsn		= Dsn;
	uid		= Uid;
	authStr	= AuthStr;

#ifndef FWD_ONLY_CURSORS

	RETCODE retcode;

	// Specify that the ODBC cursor library be used, if needed.  This must be
	// specified before the connection is made.
	retcode = SQLSetConnectOption(hdbc, SQL_ODBC_CURSORS, SQL_CUR_USE_IF_NEEDED);

	#ifdef DBDEBUG_CONSOLE
		if (retcode == SQL_SUCCESS)
			cout << "SQLSetConnectOption(CURSOR_LIB) successful" << endl;
		else
			cout << "SQLSetConnectOption(CURSOR_LIB) failed" << endl;
	#endif

#endif

	// Connect to the data source
	if (SQLConnect(hdbc, (UCHAR FAR *) Dsn,		SQL_NTS,
								(UCHAR FAR *) Uid,		SQL_NTS,
								(UCHAR FAR *) AuthStr,	SQL_NTS) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

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
	// SQL_BIGINT						SQL_NO_DATA_FOUND
	// SQL_BINARY						SQL_NO_DATA_FOUND
	// SQL_BIT							SQL_NO_DATA_FOUND
	// SQL_CHAR							type name = 'CHAR', Precision = 255
	// SQL_DATE							SQL_NO_DATA_FOUND
	// SQL_DECIMAL						type name = 'NUMBER', Precision = 38
	// SQL_DOUBLE						type name = 'NUMBER', Precision = 15
	// SQL_FLOAT						SQL_NO_DATA_FOUND
	// SQL_INTEGER						SQL_NO_DATA_FOUND
	// SQL_LONGVARBINARY				type name = 'LONG RAW', Precision = 2 billion
	// SQL_LONGVARCHAR				type name = 'LONG', Precision = 2 billion
	// SQL_NUMERIC						SQL_NO_DATA_FOUND
	// SQL_REAL							SQL_NO_DATA_FOUND
	// SQL_SMALLINT					SQL_NO_DATA_FOUND
	// SQL_TIME							SQL_NO_DATA_FOUND
	// SQL_TIMESTAMP					type name = 'DATE', Precision = 19
	// SQL_VARBINARY					type name = 'RAW', Precision = 255
	// SQL_VARCHAR						type name = 'VARCHAR2', Precision = 2000
	// =====================================================================
	// Results from a Microsoft Access 7.0 db, using a driver from Microsoft
	//
	// SQL_VARCHAR						type name = 'TEXT(', Precision = 255
	// SQL_TIMESTAMP					type name = 'DATETIME'
	// SQL_DECIMAL						SQL_NO_DATA_FOUND
	// SQL_NUMERIC						type name = 'CURRENCY', Precision = 19
	// SQL_FLOAT						SQL_NO_DATA_FOUND
	// SQL_REAL							type name = 'SINGLE', Precision = 7
	// SQL_DOUBLE						type name = 'DOUBLE', Precision = 15
	// SQL_INTEGER						type name = 'LONG', Precision = 10

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
	if (! getDataTypeInfo(SQL_TIMESTAMP, typeInfDate))
		return(FALSE);
	else
		typeInfDate.FsqlType = SQL_TIMESTAMP;

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

// The Intersolv/Oracle 7 driver was "Not Capable" of setting the login timeout.

/********** wxDB::setConnectionOptions() **********/
bool wxDB::setConnectionOptions(void)
{
	SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
	SQLSetConnectOption(hdbc, SQL_OPT_TRACE, SQL_OPT_TRACE_OFF);

	// Display the connection options to verify them
#ifdef DBDEBUG_CONSOLE
	long l;
	cout << ">>>>> CONNECTION OPTIONS <<<<<<" << endl;
	
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

	if (SQLGetInfo(hdbc, SQL_SERVER_NAME, (UCHAR*) dbInf.serverName, 40, &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_DATABASE_NAME, (UCHAR*) dbInf.databaseName, 128, &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_DBMS_NAME, (UCHAR*) dbInf.dbmsName, 40, &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_DBMS_VER, (UCHAR*) dbInf.dbmsVer, 40, &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_ACTIVE_CONNECTIONS, (UCHAR*) &dbInf.maxConnections, sizeof(dbInf.maxConnections), &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_ACTIVE_STATEMENTS, (UCHAR*) &dbInf.maxStmts, sizeof(dbInf.maxStmts), &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_DRIVER_NAME, (UCHAR*) dbInf.driverName, 40, &cb) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_DRIVER_ODBC_VER, (UCHAR*) dbInf.odbcVer, 60, &cb) == SQL_ERROR)
		return(DispAllErrors(henv, hdbc));

	if (SQLGetInfo(hdbc, SQL_ODBC_VER, (UCHAR*) dbInf.drvMgrOdbcVer, 60, &cb) == SQL_ERROR)
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
	cout << ">>>>> DATA SOURCE INFORMATION <<<<<" << endl;
	cout << "SERVER Name: " << dbInf.serverName << endl;
	cout << "DBMS Name: " << dbInf.dbmsName << "; DBMS Version: " << dbInf.dbmsVer << endl;
	cout << "ODBC Version: " << dbInf.odbcVer << "; Driver Version: " << dbInf.driverVer << endl;

	cout << "API Conf. Level: ";
	switch(dbInf.apiConfLvl)
	{
		case SQL_OAC_NONE:	cout << "None";		break;
		case SQL_OAC_LEVEL1:	cout << "Level 1";	break;
		case SQL_OAC_LEVEL2:	cout << "Level 2";	break;
	}
	cout << endl;

	cout << "SAG CLI Conf. Level: ";
	switch(dbInf.cliConfLvl)
	{
		case SQL_OSCC_NOT_COMPLIANT:	cout << "Not Compliant";	break;
		case SQL_OSCC_COMPLIANT:		cout << "Compliant";			break;
	}
	cout << endl;

	cout << "SQL Conf. Level: ";
	switch(dbInf.sqlConfLvl)
	{
		case SQL_OSC_MINIMUM:	cout << "Minimum Grammer";		break;
		case SQL_OSC_CORE:		cout << "Core Grammer";			break;
		case SQL_OSC_EXTENDED:	cout << "Extended Grammer";	break;
	}
	cout << endl;

	cout << "Max. Connections: " << dbInf.maxConnections << endl;
	cout << "Outer Joins: " << dbInf.outerJoins << endl;
	cout << "Support for Procedures: " << dbInf.procedureSupport << endl;

	cout << "Cursor COMMIT Behavior: ";
	switch(dbInf.cursorCommitBehavior)
	{
		case SQL_CB_DELETE:		cout << "Delete cursors";		break;
		case SQL_CB_CLOSE:		cout << "Close cursors";		break;
		case SQL_CB_PRESERVE:	cout << "Preserve cursors";	break;
	}
	cout << endl;

	cout << "Cursor ROLLBACK Behavior: ";
	switch(dbInf.cursorRollbackBehavior)
	{
		case SQL_CB_DELETE:		cout << "Delete cursors";		break;
		case SQL_CB_CLOSE:		cout << "Close cursors";		break;
		case SQL_CB_PRESERVE:	cout << "Preserve cursors";	break;
	}
	cout << endl;

	cout << "Support NOT NULL clause: ";
	switch(dbInf.supportNotNullClause)
	{
		case SQL_NNC_NULL:		cout << "No";		break;
		case SQL_NNC_NON_NULL:	cout << "Yes";		break;
	}
	cout << endl;

	cout << "Support IEF (Ref. Integrity): " << dbInf.supportIEF << endl;
	cout << "Login Timeout: " << dbInf.loginTimeout << endl;

	cout << endl << endl << "more ..." << endl;
	getchar();

	cout << "Default Transaction Isolation: ";
	switch(dbInf.txnIsolation)
	{
		case SQL_TXN_READ_UNCOMMITTED:	cout << "Read Uncommitted";	break;
		case SQL_TXN_READ_COMMITTED:		cout << "Read Committed";		break;
		case SQL_TXN_REPEATABLE_READ:		cout << "Repeatable Read";		break;
		case SQL_TXN_SERIALIZABLE:			cout << "Serializable";			break;
#ifdef ODBC_V20
		case SQL_TXN_VERSIONING:			cout << "Versioning";			break;
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
		case SQL_TC_NONE:			cout << "No";				break;
		case SQL_TC_DML:			cout << "DML Only";		break;
		case SQL_TC_DDL_COMMIT:	cout << "DDL Commit";	break;
		case SQL_TC_DDL_IGNORE:	cout << "DDL Ignore";	break;
		case SQL_TC_ALL:			cout << "DDL & DML";		break;
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
	// fSqlType will be something like SQL_VARCHAR.  This parameter determines
	// the data type inf. is gathered for.
	//
	// SqlTypeInfo is a structure that is filled in with data type information,

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
	if (SQLGetData(hstmt, 3, SQL_C_LONG, (UCHAR*) &structSQLTypeInfo.Precision, 0, &cbRet) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc, hstmt));
	if (SQLGetData(hstmt, 8, SQL_C_SHORT, (UCHAR*) &structSQLTypeInfo.CaseSensitive, 0, &cbRet) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc, hstmt));
//	if (SQLGetData(hstmt, 14, SQL_C_SHORT, (UCHAR*) &structSQLTypeInfo.MinimumScale, 0, &cbRet) != SQL_SUCCESS)
//		return(DispAllErrors(henv, hdbc, hstmt));
	if (SQLGetData(hstmt, 15, SQL_C_SHORT, (UCHAR*) &structSQLTypeInfo.MaximumScale, 0, &cbRet) != SQL_SUCCESS)
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
		fpSqlLog = 0;  //glt
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

} // wxDB::Close()

/********** wxDB::CommitTrans() **********/
bool wxDB::CommitTrans(void)
{
	// Commit the transaction
	if (SQLTransact(henv, hdbc, SQL_COMMIT) != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc));

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
	char odbcErrMsg[DB_MAX_ERROR_MSG_LEN];

	while (SQLError(aHenv, aHdbc, aHstmt, (UCHAR FAR *) sqlState, &nativeError, (UCHAR FAR *) errorMsg, SQL_MAX_MESSAGE_LENGTH - 1, &cbErrorMsg) == SQL_SUCCESS)
	{
		sprintf(odbcErrMsg, "SQL State = %s\nNative Error Code = %li\nError Message = %s\n", sqlState, nativeError, errorMsg);
		logError(odbcErrMsg, sqlState);
		if (!silent)
		{
#ifdef DBDEBUG_CONSOLE
			// When run in console mode, use standard out to display errors.
			cout << odbcErrMsg << endl;
			cout << "Press any key to continue..." << endl;
			getchar();
#endif
		}
	}

	return(FALSE);  // This function alway's returns false.

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
	char odbcErrMsg[DB_MAX_ERROR_MSG_LEN];

	sprintf(odbcErrMsg, "SQL State = %s\nNative Error Code = %li\nError Message = %s\n", sqlState, nativeError, errorMsg);
	logError(odbcErrMsg, sqlState);

	if (silent)
		return;

#ifdef DBDEBUG_CONSOLE
	// When run in console mode, use standard out to display errors.
	cout << odbcErrMsg << endl;
	cout << "Press any key to continue..."  << endl;
	getchar();
#endif

} // wxDB::DispNextError()

/********** wxDB::logError() **********/
void wxDB::logError(char *errMsg, char *SQLState)
{
	assert(errMsg && strlen(errMsg));

	static int pLast = -1;
	int dbStatus;

	if (++pLast == DB_MAX_ERROR_HISTORY)
	{
		for (int i = 0; i < DB_MAX_ERROR_HISTORY; i++)
			strcpy(errorList[i], errorList[i+1]);
		pLast--;
	}

	strcpy(errorList[pLast], errMsg);

	if (SQLState && strlen(SQLState))
		if ((dbStatus = TranslateSqlState(SQLState)) != DB_ERR_FUNCTION_SEQUENCE_ERROR)
			DB_STATUS = dbStatus;

}  // wxDB::logError()

/**********wxDB::TranslateSqlState()  **********/
int wxDB::TranslateSqlState(char *SQLState)
{
	if (!strcmp(SQLState, "01000"))
		return(DB_ERR_GENERAL_WARNING);
	if (!strcmp(SQLState, "01002"))
		return(DB_ERR_DISCONNECT_ERROR);
	if (!strcmp(SQLState, "01004"))
		return(DB_ERR_DATA_TRUNCATED);
	if (!strcmp(SQLState, "01006"))
		return(DB_ERR_PRIV_NOT_REVOKED);
	if (!strcmp(SQLState, "01S00"))
		return(DB_ERR_INVALID_CONN_STR_ATTR);
	if (!strcmp(SQLState, "01S01"))
		return(DB_ERR_ERROR_IN_ROW);
	if (!strcmp(SQLState, "01S02"))
		return(DB_ERR_OPTION_VALUE_CHANGED);
	if (!strcmp(SQLState, "01S03"))
		return(DB_ERR_NO_ROWS_UPD_OR_DEL);
	if (!strcmp(SQLState, "01S04"))
		return(DB_ERR_MULTI_ROWS_UPD_OR_DEL);
	if (!strcmp(SQLState, "07001"))
		return(DB_ERR_WRONG_NO_OF_PARAMS);
	if (!strcmp(SQLState, "07006"))
		return(DB_ERR_DATA_TYPE_ATTR_VIOL);
	if (!strcmp(SQLState, "08001"))
		return(DB_ERR_UNABLE_TO_CONNECT);
	if (!strcmp(SQLState, "08002"))
		return(DB_ERR_CONNECTION_IN_USE);
	if (!strcmp(SQLState, "08003"))
		return(DB_ERR_CONNECTION_NOT_OPEN);
	if (!strcmp(SQLState, "08004"))
		return(DB_ERR_REJECTED_CONNECTION);
	if (!strcmp(SQLState, "08007"))
		return(DB_ERR_CONN_FAIL_IN_TRANS);
	if (!strcmp(SQLState, "08S01"))
		return(DB_ERR_COMM_LINK_FAILURE);
	if (!strcmp(SQLState, "21S01"))
		return(DB_ERR_INSERT_VALUE_LIST_MISMATCH);
	if (!strcmp(SQLState, "21S02"))
		return(DB_ERR_DERIVED_TABLE_MISMATCH);
	if (!strcmp(SQLState, "22001"))
		return(DB_ERR_STRING_RIGHT_TRUNC);
	if (!strcmp(SQLState, "22003"))
		return(DB_ERR_NUMERIC_VALUE_OUT_OF_RNG);
	if (!strcmp(SQLState, "22005"))
		return(DB_ERR_ERROR_IN_ASSIGNMENT);
	if (!strcmp(SQLState, "22008"))
		return(DB_ERR_DATETIME_FLD_OVERFLOW);
	if (!strcmp(SQLState, "22012"))
		return(DB_ERR_DIVIDE_BY_ZERO);
	if (!strcmp(SQLState, "22026"))
		return(DB_ERR_STR_DATA_LENGTH_MISMATCH);
	if (!strcmp(SQLState, "23000"))
		return(DB_ERR_INTEGRITY_CONSTRAINT_VIOL);
	if (!strcmp(SQLState, "24000"))
		return(DB_ERR_INVALID_CURSOR_STATE);
	if (!strcmp(SQLState, "25000"))
		return(DB_ERR_INVALID_TRANS_STATE);
	if (!strcmp(SQLState, "28000"))
		return(DB_ERR_INVALID_AUTH_SPEC);
	if (!strcmp(SQLState, "34000"))
		return(DB_ERR_INVALID_CURSOR_NAME);
	if (!strcmp(SQLState, "37000"))
		return(DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL);
	if (!strcmp(SQLState, "3C000"))
		return(DB_ERR_DUPLICATE_CURSOR_NAME);
	if (!strcmp(SQLState, "40001"))
		return(DB_ERR_SERIALIZATION_FAILURE);
	if (!strcmp(SQLState, "42000"))
		return(DB_ERR_SYNTAX_ERROR_OR_ACCESS_VIOL2);
	if (!strcmp(SQLState, "70100"))
		return(DB_ERR_OPERATION_ABORTED);
	if (!strcmp(SQLState, "IM001"))
		return(DB_ERR_UNSUPPORTED_FUNCTION);
	if (!strcmp(SQLState, "IM002"))
		return(DB_ERR_NO_DATA_SOURCE);
	if (!strcmp(SQLState, "IM003"))
		return(DB_ERR_DRIVER_LOAD_ERROR);
	if (!strcmp(SQLState, "IM004"))
		return(DB_ERR_SQLALLOCENV_FAILED);
	if (!strcmp(SQLState, "IM005"))
		return(DB_ERR_SQLALLOCCONNECT_FAILED);
	if (!strcmp(SQLState, "IM006"))
		return(DB_ERR_SQLSETCONNECTOPTION_FAILED);
	if (!strcmp(SQLState, "IM007"))
		return(DB_ERR_NO_DATA_SOURCE_DLG_PROHIB);
	if (!strcmp(SQLState, "IM008"))
		return(DB_ERR_DIALOG_FAILED);
	if (!strcmp(SQLState, "IM009"))
		return(DB_ERR_UNABLE_TO_LOAD_TRANSLATION_DLL);
	if (!strcmp(SQLState, "IM010"))
		return(DB_ERR_DATA_SOURCE_NAME_TOO_LONG);
	if (!strcmp(SQLState, "IM011"))
		return(DB_ERR_DRIVER_NAME_TOO_LONG);
	if (!strcmp(SQLState, "IM012"))
		return(DB_ERR_DRIVER_KEYWORD_SYNTAX_ERROR);
	if (!strcmp(SQLState, "IM013"))
		return(DB_ERR_TRACE_FILE_ERROR);
	if (!strcmp(SQLState, "S0001"))
		return(DB_ERR_TABLE_OR_VIEW_ALREADY_EXISTS);
	if (!strcmp(SQLState, "S0002"))
		return(DB_ERR_TABLE_NOT_FOUND);
	if (!strcmp(SQLState, "S0011"))
		return(DB_ERR_INDEX_ALREADY_EXISTS);
	if (!strcmp(SQLState, "S0012"))
		return(DB_ERR_INDEX_NOT_FOUND);
	if (!strcmp(SQLState, "S0021"))
		return(DB_ERR_COLUMN_ALREADY_EXISTS);
	if (!strcmp(SQLState, "S0022"))
		return(DB_ERR_COLUMN_NOT_FOUND);
	if (!strcmp(SQLState, "S0023"))
		return(DB_ERR_NO_DEFAULT_FOR_COLUMN);
	if (!strcmp(SQLState, "S1000"))
		return(DB_ERR_GENERAL_ERROR);
	if (!strcmp(SQLState, "S1001"))
		return(DB_ERR_MEMORY_ALLOCATION_FAILURE);
	if (!strcmp(SQLState, "S1002"))
		return(DB_ERR_INVALID_COLUMN_NUMBER);
	if (!strcmp(SQLState, "S1003"))
		return(DB_ERR_PROGRAM_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1004"))
		return(DB_ERR_SQL_DATA_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1008"))
		return(DB_ERR_OPERATION_CANCELLED);
	if (!strcmp(SQLState, "S1009"))
		return(DB_ERR_INVALID_ARGUMENT_VALUE);
	if (!strcmp(SQLState, "S1010"))
		return(DB_ERR_FUNCTION_SEQUENCE_ERROR);
	if (!strcmp(SQLState, "S1011"))
		return(DB_ERR_OPERATION_INVALID_AT_THIS_TIME);
	if (!strcmp(SQLState, "S1012"))
		return(DB_ERR_INVALID_TRANS_OPERATION_CODE);
	if (!strcmp(SQLState, "S1015"))
		return(DB_ERR_NO_CURSOR_NAME_AVAIL);
	if (!strcmp(SQLState, "S1090"))
		return(DB_ERR_INVALID_STR_OR_BUF_LEN);
	if (!strcmp(SQLState, "S1091"))
		return(DB_ERR_DESCRIPTOR_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1092"))
		return(DB_ERR_OPTION_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1093"))
		return(DB_ERR_INVALID_PARAM_NO);
	if (!strcmp(SQLState, "S1094"))
		return(DB_ERR_INVALID_SCALE_VALUE);
	if (!strcmp(SQLState, "S1095"))
		return(DB_ERR_FUNCTION_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1096"))
		return(DB_ERR_INF_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1097"))
		return(DB_ERR_COLUMN_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1098"))
		return(DB_ERR_SCOPE_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1099"))
		return(DB_ERR_NULLABLE_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1100"))
		return(DB_ERR_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1101"))
		return(DB_ERR_ACCURACY_OPTION_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1103"))
		return(DB_ERR_DIRECTION_OPTION_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1104"))
		return(DB_ERR_INVALID_PRECISION_VALUE);
	if (!strcmp(SQLState, "S1105"))
		return(DB_ERR_INVALID_PARAM_TYPE);
	if (!strcmp(SQLState, "S1106"))
		return(DB_ERR_FETCH_TYPE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1107"))
		return(DB_ERR_ROW_VALUE_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1108"))
		return(DB_ERR_CONCURRENCY_OPTION_OUT_OF_RANGE);
	if (!strcmp(SQLState, "S1109"))
		return(DB_ERR_INVALID_CURSOR_POSITION);
	if (!strcmp(SQLState, "S1110"))
		return(DB_ERR_INVALID_DRIVER_COMPLETION);
	if (!strcmp(SQLState, "S1111"))
		return(DB_ERR_INVALID_BOOKMARK_VALUE);
	if (!strcmp(SQLState, "S1C00"))
		return(DB_ERR_DRIVER_NOT_CAPABLE);
	if (!strcmp(SQLState, "S1T00"))
		return(DB_ERR_TIMEOUT_EXPIRED);

	// No match
	return(0);

}  // wxDB::TranslateSqlState()
	
/**********  wxDB::Grant() **********/
bool wxDB::Grant(int privileges, char *tableName, char *userList)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Build the grant statement
	strcpy(sqlStmt, "GRANT ");
	if (privileges == DB_GRANT_ALL)
		strcat(sqlStmt, "ALL");
	else
	{
		int c = 0;
		if (privileges & DB_GRANT_SELECT)
		{
			strcat(sqlStmt, "SELECT(");
			c++;
		}
		if (privileges & DB_GRANT_INSERT)
		{
			if (c++)
				strcat(sqlStmt, ", ");
			strcat(sqlStmt, "INSERT(");
		}
		if (privileges & DB_GRANT_UPDATE)
		{
			if (c++)
				strcat(sqlStmt, ", ");
			strcat(sqlStmt, "UPDATE");
		}
		if (privileges & DB_GRANT_DELETE)
		{
			if (c++)
				strcat(sqlStmt, ", ");
			strcat(sqlStmt, "DELETE");
		}
	}

	strcat(sqlStmt, " ON ");
	strcat(sqlStmt, tableName);
	strcat(sqlStmt, " TO ");
	strcat(sqlStmt, userList);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl;
#endif

	WriteSqlLog(sqlStmt);

	return(ExecSql(sqlStmt));

}  // wxDB::Grant()

/********** wxDB::CreateView() **********/
bool wxDB::CreateView(char *viewName, char *colList, char *pSqlStmt)
{
	char sqlStmt[DB_MAX_STATEMENT_LEN];

	// Drop the view first
	sprintf(sqlStmt, "DROP VIEW %s", viewName);
	if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) != SQL_SUCCESS)
	{
		// Check for sqlState = S0002, "Table or view not found".
		// Ignore this error, bomb out on any other error.
		// SQL Sybase Anwhere v5.5 returns an access violation error here
		// (sqlstate = 42000) rather than an S0002.
		GetNextError(henv, hdbc, hstmt);
		if (strcmp(sqlState, "S0002") && strcmp(sqlState, "42000"))
		{
			DispNextError();
			DispAllErrors(henv, hdbc, hstmt);
			RollbackTrans();
			return(FALSE);
		}
	}

	WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << endl << sqlStmt << endl;
#endif

	// Build the create view statement
	strcpy(sqlStmt, "CREATE VIEW ");
	strcat(sqlStmt, viewName);
	
	if (strlen(colList))
	{
		strcat(sqlStmt, " (");
		strcat(sqlStmt, colList);
		strcat(sqlStmt, ")");
	}

	strcat(sqlStmt, " AS ");
	strcat(sqlStmt, pSqlStmt);

	WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
	cout << sqlStmt << endl;
#endif

	return(ExecSql(sqlStmt));

}  // wxDB::CreateView()

/********** wxDB::ExecSql()  **********/
bool wxDB::ExecSql(char *pSqlStmt)
{
	if (SQLExecDirect(hstmt, (UCHAR FAR *) pSqlStmt, SQL_NTS) == SQL_SUCCESS)
		return(TRUE);
	else
	{
		DispAllErrors(henv, hdbc, hstmt);
		return(FALSE);
	}

}  // wxDB::ExecSql()

/********** wxDB::GetColumns() **********/
/*
 *		1) The last array element of the tableName[] argument must be zero (null).
 *			This is how the end of the array is detected.
 *		2) This function returns an array of CcolInf structures.  If no columns
 *			were found, or an error occured, this pointer will be zero (null).  THE
 *			CALLING FUNCTION IS RESPONSIBLE FOR DELETING THE MEMORY RETURNED WHEN IT
 *			IS FINISHED WITH IT.  i.e.
 *
 *			CcolInf *colInf = pDb->GetColumns(tableList);
 *			if (colInf)
 *			{
 *				// Use the column inf
 *				.......
 *				// Destroy the memory
 *				delete [] colInf;
 *			}
 */
CcolInf *wxDB::GetColumns(char *tableName[])
{
	UINT noCols = 0;
	UINT colNo = 0;
	CcolInf *colInf = 0;
	RETCODE retcode;
	SDWORD cb;
	char tblName[DB_MAX_TABLE_NAME_LEN+1];
	char colName[DB_MAX_COLUMN_NAME_LEN+1];
	SWORD sqlDataType;

	// Pass 1 - Determine how many columns there are.
	// Pass 2 - Allocate the CcolInf array and fill in
	//				the array with the column information.
	for (int pass = 1; pass <= 2; pass++)
	{
		if (pass == 2)
		{
			if (noCols == 0)  // Probably a bogus table name(s)
				break;
			// Allocate n CcolInf objects to hold the column information
			colInf = new CcolInf[noCols+1];
			if (!colInf)
				break;
			// Mark the end of the array
			strcpy(colInf[noCols].tableName, "");
			strcpy(colInf[noCols].colName, "");
			colInf[noCols].sqlDataType = 0;
		}
		// Loop through each table name
		for (int tbl = 0; tableName[tbl]; tbl++)
		{
			SQLFreeStmt(hstmt, SQL_CLOSE);
			retcode = SQLColumns(hstmt,
										NULL, 0,											// All qualifiers
										NULL, 0,											// All owners
										(UCHAR *) tableName[tbl], SQL_NTS,
										NULL, 0);										// All columns
			if (retcode != SQL_SUCCESS)
			{  // Error occured, abort
				DispAllErrors(henv, hdbc, hstmt);
				if (colInf)
					delete [] colInf;
				return(0);
			}
			SQLBindCol(hstmt, 3, SQL_C_CHAR,   (UCHAR*) tblName,      DB_MAX_TABLE_NAME_LEN+1,  &cb);
			SQLBindCol(hstmt, 4, SQL_C_CHAR,   (UCHAR*) colName,      DB_MAX_COLUMN_NAME_LEN+1, &cb);
			SQLBindCol(hstmt, 5, SQL_C_SSHORT, (UCHAR*) &sqlDataType, 0,                        &cb);
			while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
			{
				if (pass == 1)  // First pass, just add up the number of columns
					noCols++;
				else  // Pass 2; Fill in the array of structures
				{
					if (colNo < noCols)  // Some extra error checking to prevent memory overwrites
					{
						strcpy(colInf[colNo].tableName, tblName);
						strcpy(colInf[colNo].colName, colName);
						colInf[colNo].sqlDataType = sqlDataType;
						colNo++;
					}
				}
			}
			if (retcode != SQL_NO_DATA_FOUND)
			{  // Error occured, abort
				DispAllErrors(henv, hdbc, hstmt);
				if (colInf)
					delete [] colInf;
				return(0);
			}
		}
	}

	SQLFreeStmt(hstmt, SQL_CLOSE);
	return colInf;

}  // wxDB::GetColumns()


/********** wxDB::Catalog() **********/
bool wxDB::Catalog(char *userID, char *fileName)
{
	assert(userID && strlen(userID));
	assert(fileName && strlen(fileName));

	RETCODE	retcode;
	SDWORD	cb;
	char		tblName[DB_MAX_TABLE_NAME_LEN+1];
	char		tblNameSave[DB_MAX_TABLE_NAME_LEN+1];
	char		colName[DB_MAX_COLUMN_NAME_LEN+1];
	SWORD		sqlDataType;
	char		typeName[16];
	SWORD		precision, length;

	FILE *fp = fopen(fileName,"wt");
	if (fp == NULL)
		return(FALSE);

	SQLFreeStmt(hstmt, SQL_CLOSE);

	int i = 0;
	char userIdUC[81];
	for (char *p = userID; *p; p++)
		userIdUC[i++] = toupper(*p);
	userIdUC[i] = 0;

	retcode = SQLColumns(hstmt,
								NULL, 0,											// All qualifiers
								(UCHAR *) userIdUC, SQL_NTS,				// User specified
								NULL, 0,											// All tables
								NULL, 0);										// All columns
	if (retcode != SQL_SUCCESS)
	{
		DispAllErrors(henv, hdbc, hstmt);
		fclose(fp);
		return(FALSE);
	}

	SQLBindCol(hstmt, 3, SQL_C_CHAR,   (UCHAR*) tblName,      DB_MAX_TABLE_NAME_LEN+1,  &cb);
	SQLBindCol(hstmt, 4, SQL_C_CHAR,   (UCHAR*) colName,      DB_MAX_COLUMN_NAME_LEN+1, &cb);
	SQLBindCol(hstmt, 5, SQL_C_SSHORT, (UCHAR*) &sqlDataType, 0,                        &cb);
	SQLBindCol(hstmt, 6, SQL_C_CHAR,   (UCHAR*) typeName,		 16,                       &cb);
	SQLBindCol(hstmt, 7, SQL_C_SSHORT, (UCHAR*) &precision,	 0,                        &cb);
	SQLBindCol(hstmt, 8, SQL_C_SSHORT, (UCHAR*) &length,   	 0,                        &cb);

	char outStr[256];
	strcpy(tblNameSave,"");
	int cnt = 0;

	while ((retcode = SQLFetch(hstmt)) == SQL_SUCCESS)
	{
		if (strcmp(tblName,tblNameSave))
		{
			if (cnt)
				fputs("\n", fp);
			fputs("================================ ", fp);
			fputs("================================ ", fp);
			fputs("===================== ", fp);
			fputs("========= ", fp);
			fputs("=========\n", fp);
			sprintf(outStr, "%-32s %-32s %-21s %9s %9s\n",
				"TABLE NAME", "COLUMN NAME", "DATA TYPE", "PRECISION", "LENGTH");
			fputs(outStr, fp);
			fputs("================================ ", fp);
			fputs("================================ ", fp);
			fputs("===================== ", fp);
			fputs("========= ", fp);
			fputs("=========\n", fp);
			strcpy(tblNameSave,tblName);
		}
		sprintf(outStr, "%-32s %-32s (%04d)%-15s %9d %9d\n",
			tblName, colName, sqlDataType, typeName, precision, length);
		if (fputs(outStr, fp) == EOF)
		{
			fclose(fp);
			return(FALSE);
		}
		cnt++;
	}

	if (retcode != SQL_NO_DATA_FOUND)
	{
		DispAllErrors(henv, hdbc, hstmt);
		fclose(fp);
		return(FALSE);
	}

	SQLFreeStmt(hstmt, SQL_CLOSE);
	fclose(fp);
	return(TRUE);

}  // wxDB::Catalog()


// Table name can refer to a table, view, alias or synonym.  Returns true
// if the object exists in the database.  This function does not indicate
// whether or not the user has privleges to query or perform other functions
// on the table.
bool wxDB::TableExists(char *tableName)
{
	assert(tableName && strlen(tableName));

	SQLFreeStmt(hstmt, SQL_CLOSE);
	RETCODE retcode = SQLTables(hstmt,
										 NULL, 0,										// All qualifiers
										 NULL, 0,										// All owners
										 (UCHAR FAR *)tableName, SQL_NTS,
										 NULL, 0);										// All table types
	if (retcode != SQL_SUCCESS)
		return(DispAllErrors(henv, hdbc, hstmt));

	if (SQLFetch(hstmt) != SQL_SUCCESS)
	{
		SQLFreeStmt(hstmt, SQL_CLOSE);
		return(DispAllErrors(henv, hdbc, hstmt));
	}

	SQLFreeStmt(hstmt, SQL_CLOSE);
	return(TRUE);

}  // wxDB::TableExists()


/********** wxDB::SqlLog() **********/
bool wxDB::SqlLog(enum sqlLog state, char *filename, bool append)
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
bool wxDB::WriteSqlLog(char *logMsg)
{
	assert(logMsg);

	if (fpSqlLog == 0 || sqlLogState == sqlLogOFF)
		return(FALSE);

	if (fputs("\n",   fpSqlLog) == EOF) return(FALSE);
	if (fputs(logMsg, fpSqlLog) == EOF) return(FALSE);
	if (fputs("\n",   fpSqlLog) == EOF) return(FALSE);

	return(TRUE);

}  // wxDB::WriteSqlLog()


/********** GetDbConnection() **********/
wxDB* WXDLLEXPORT GetDbConnection(DbStuff *pDbStuff)
{
	DbList *pList;

	// Scan the linked list searching for an available database connection
	// that's already been opened but is currently not in use.
	for (pList = PtrBegDbList; pList; pList = pList->PtrNext)
	{
		// The database connection must be for the same datasource
		// name and must currently not be in use.
		if (pList->Free && (! strcmp(pDbStuff->Dsn, pList->Dsn)))  // Found a free connection
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
	else	// Empty list
	{
		// Create the first node on the list
		pList = PtrBegDbList = new DbList;
		pList->PtrPrev = 0;
	}

	// Initialize new node in the linked list
	pList->PtrNext = 0;
	pList->Free = FALSE;
	strcpy(pList->Dsn, pDbStuff->Dsn);
	pList->PtrDb = new wxDB(pDbStuff->Henv);

	// Connect to the datasource
	if (pList->PtrDb->Open(pDbStuff->Dsn, pDbStuff->Uid, pDbStuff->AuthStr))
		return(pList->PtrDb);
	else  // Unable to connect, destroy list item
	{
		if (pList->PtrPrev)
			pList->PtrPrev->PtrNext = 0;
		else
			PtrBegDbList = 0;				// Empty list again
		pList->PtrDb->CommitTrans();	// Commit any open transactions on wxDB object
		pList->PtrDb->Close();			// Close the wxDB object
		delete pList->PtrDb;				// Deletes the wxDB object
		delete pList;						// Deletes the linked list object
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
		if (pList->PtrDb == pDb)		// Found it!!!
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
		pNext = pList->PtrNext;			// Save the pointer to next
		pList->PtrDb->CommitTrans();	// Commit any open transactions on wxDB object
		pList->PtrDb->Close();			// Close the wxDB object
		delete pList->PtrDb;				// Deletes the wxDB object
		delete pList;						// Deletes the linked list object
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

/********** GetDataSource() **********/
bool GetDataSource(HENV henv, char *Dsn, SWORD DsnMax, char *DsDesc, SWORD DsDescMax,
						 UWORD direction)
{
	SWORD cb;

	if (SQLDataSources(henv, direction, (UCHAR FAR *) Dsn, DsnMax, &cb,
							 (UCHAR FAR *) DsDesc, DsDescMax, &cb) == SQL_SUCCESS)
		return(TRUE);
	else
		return(FALSE);

}  // GetDataSource()

#endif
 // wxUSE_ODBC

