///////////////////////////////////////////////////////////////////////////////
// Name:        dbtable.cpp
// Purpose:     Implementation of the wxDbTable class.
// Author:      Doug Card
// Modified by: George Tasker
//              Bart Jourquin
//              Mark Johnson
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

// Use this line for wxWindows v1.x
//#include "wx_ver.h"
// Use this line for wxWindows v2.x
#include  "wx/wxprec.h"
#include "wx/version.h"

#if wxMAJOR_VERSION == 2
    #ifdef __GNUG__
        #pragma implementation "dbtable.h"
    #endif
#endif

#ifdef DBDEBUG_CONSOLE
    #include "wx/ioswrap.h"
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
        #include "wx/log.h"
    #endif
    #include "wx/filefn.h"
#endif

#if wxMAJOR_VERSION == 1
#   if defined(wx_msw) || defined(wx_x)
#       ifdef WX_PRECOMP
#           include "wx_prec.h"
#       else
#           include "wx.h"
#       endif
#   endif
#   define wxUSE_ODBC 1
#endif


#if wxUSE_ODBC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

#if   wxMAJOR_VERSION == 1
    #include "table.h"
#elif wxMAJOR_VERSION == 2
    #include "wx/dbtable.h"
#endif

#ifdef __UNIX__
// The HPUX preprocessor lines below were commented out on 8/20/97
// because macros.h currently redefines DEBUG and is unneeded.
// #  ifdef HPUX
// #    include <macros.h>
// #  endif
#  ifdef LINUX
#    include <sys/minmax.h>
#  endif
#endif

ULONG lastTableID = 0;


#ifdef __WXDEBUG__
    wxList TablesInUse;
#endif


/********** wxDbColDef::wxDbColDef() Constructor **********/
wxDbColDef::wxDbColDef()
{
    Initialize();
}  // Constructor


bool wxDbColDef::Initialize()
{
    ColName[0]      = 0;
    DbDataType      = DB_DATA_TYPE_INTEGER;
    SqlCtype        = SQL_C_LONG;
    PtrDataObj      = NULL;
    SzDataObj       = 0;
    KeyField        = FALSE;
    Updateable      = FALSE;
    InsertAllowed   = FALSE;
    DerivedCol      = FALSE;
    CbValue         = 0;
    Null = FALSE;

    return TRUE;
}  // wxDbColDef::Initialize()


/********** wxDbTable::wxDbTable() Constructor **********/
wxDbTable::wxDbTable(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
                    const wxString &qryTblName, bool qryOnly, const wxString &tblPath)
{
    if (!initialize(pwxDb, tblName, numColumns, qryTblName, qryOnly, tblPath))
        cleanup();
}  // wxDbTable::wxDbTable()


/***** DEPRECATED: use wxDbTable::wxDbTable() format above *****/
wxDbTable::wxDbTable(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
                    const wxChar *qryTblName, bool qryOnly, const wxString &tblPath)
{
    wxString tempQryTblName;
    tempQryTblName = qryTblName;
    if (!initialize(pwxDb, tblName, numColumns, tempQryTblName, qryOnly, tblPath))
        cleanup();
}  // wxDbTable::wxDbTable()


/********** wxDbTable::~wxDbTable() **********/
wxDbTable::~wxDbTable()
{
    this->cleanup();
}  // wxDbTable::~wxDbTable()


bool wxDbTable::initialize(wxDb *pwxDb, const wxString &tblName, const UWORD numColumns,
                    const wxString &qryTblName, bool qryOnly, const wxString &tblPath)
{
    // Initializing member variables
    pDb                 = pwxDb;                    // Pointer to the wxDb object
    henv                = 0;
    hdbc                = 0;
    hstmt               = 0;
    hstmtDefault        = 0;                        // Initialized below
    hstmtCount          = 0;                        // Initialized first time it is needed
    hstmtInsert         = 0;
    hstmtDelete         = 0;
    hstmtUpdate         = 0;
    hstmtInternal       = 0;
    colDefs             = 0;
    tableID             = 0;
    noCols              = numColumns;               // Number of cols in the table
    where.Empty();                                  // Where clause
    orderBy.Empty();                                // Order By clause
    from.Empty();                                   // From clause
    selectForUpdate     = FALSE;                    // SELECT ... FOR UPDATE; Indicates whether to include the FOR UPDATE phrase
    queryOnly           = qryOnly;
    insertable          = TRUE;
    tablePath.Empty();
    tableName.Empty();
    queryTableName.Empty();

    wxASSERT(tblName.Length());
    wxASSERT(pDb);

    if (!pDb)
        return FALSE;

    tableName = tblName;                        // Table Name
    if (tblPath.Length())
        tablePath = tblPath;                    // Table Path - used for dBase files
    else
        tablePath.Empty();
    
    if (qryTblName.Length())                    // Name of the table/view to query
        queryTableName = qryTblName;
    else
        queryTableName = tblName;
    
    pDb->incrementTableCount();
    
    wxString s;
    tableID = ++lastTableID;
    s.Printf(wxT("wxDbTable constructor (%-20s) tableID:[%6lu] pDb:[%p]"), tblName.c_str(), tableID, pDb);
    
#ifdef __WXDEBUG__
    wxTablesInUse *tableInUse;
    tableInUse            = new wxTablesInUse();
    tableInUse->tableName = tblName;
    tableInUse->tableID   = tableID;
    tableInUse->pDb       = pDb;
    TablesInUse.Append(tableInUse);
#endif
    
    pDb->WriteSqlLog(s);
    
    // Grab the HENV and HDBC from the wxDb object
    henv = pDb->GetHENV();
    hdbc = pDb->GetHDBC();
    
    // Allocate space for column definitions
    if (noCols)
        colDefs = new wxDbColDef[noCols];  // Points to the first column definition
    
    // Allocate statement handles for the table
    if (!queryOnly)
    {
        // Allocate a separate statement handle for performing inserts
        if (SQLAllocStmt(hdbc, &hstmtInsert) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc);
        // Allocate a separate statement handle for performing deletes
        if (SQLAllocStmt(hdbc, &hstmtDelete) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc);
        // Allocate a separate statement handle for performing updates
        if (SQLAllocStmt(hdbc, &hstmtUpdate) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc);
    }
    // Allocate a separate statement handle for internal use
    if (SQLAllocStmt(hdbc, &hstmtInternal) != SQL_SUCCESS)
        pDb->DispAllErrors(henv, hdbc);
    
    // Set the cursor type for the statement handles
    cursorType = SQL_CURSOR_STATIC;
    
    if (SQLSetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
    { 
        // Check to see if cursor type is supported
        pDb->GetNextError(henv, hdbc, hstmtInternal);
        if (! wxStrcmp(pDb->sqlState, wxT("01S02")))  // Option Value Changed
        {
            // Datasource does not support static cursors.  Driver
            // will substitute a cursor type.  Call SQLGetStmtOption()
            // to determine which cursor type was selected.
            if (SQLGetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, &cursorType) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc, hstmtInternal);
#ifdef DBDEBUG_CONSOLE
            cout << wxT("Static cursor changed to: ");
            switch(cursorType)
            {
            case SQL_CURSOR_FORWARD_ONLY:
                cout << wxT("Forward Only");
                break;
            case SQL_CURSOR_STATIC:
                cout << wxT("Static");
                break;
            case SQL_CURSOR_KEYSET_DRIVEN:
                cout << wxT("Keyset Driven");
                break;
            case SQL_CURSOR_DYNAMIC:
                cout << wxT("Dynamic");
                break;
            }
            cout << endl << endl;
#endif
            // BJO20000425
            if (pDb->FwdOnlyCursors() && cursorType != SQL_CURSOR_FORWARD_ONLY)
            {
                // Force the use of a forward only cursor...
                cursorType = SQL_CURSOR_FORWARD_ONLY;
                if (SQLSetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
                {
                    // Should never happen
                    pDb->GetNextError(henv, hdbc, hstmtInternal);
                    return FALSE;
                }
            }
        }
        else
        {
            pDb->DispNextError();
            pDb->DispAllErrors(henv, hdbc, hstmtInternal);
        }
    }
#ifdef DBDEBUG_CONSOLE
    else
        cout << wxT("Cursor Type set to STATIC") << endl << endl;
#endif
    
    if (!queryOnly)
    {
        // Set the cursor type for the INSERT statement handle
        if (SQLSetStmtOption(hstmtInsert, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc, hstmtInsert);
        // Set the cursor type for the DELETE statement handle
        if (SQLSetStmtOption(hstmtDelete, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc, hstmtDelete);
        // Set the cursor type for the UPDATE statement handle
        if (SQLSetStmtOption(hstmtUpdate, SQL_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc, hstmtUpdate);
    }
    
    // Make the default cursor the active cursor
    hstmtDefault = GetNewCursor(FALSE,FALSE);
    wxASSERT(hstmtDefault);
    hstmt = *hstmtDefault;

    return TRUE;

}  // wxDbTable::initialize()


void wxDbTable::cleanup()
{
    wxString s;
    if (pDb)
    {
        s.Printf(wxT("wxDbTable destructor (%-20s) tableID:[%6lu] pDb:[%p]"), tableName.c_str(), tableID, pDb);
        pDb->WriteSqlLog(s);
    }

#ifdef __WXDEBUG__
    if (tableID)
    {
        TablesInUse.DeleteContents(TRUE);
        bool found = FALSE;

        wxNode *pNode;
        pNode = TablesInUse.First();
        while (pNode && !found)
        {
            if (((wxTablesInUse *)pNode->Data())->tableID == tableID)
            {
                found = TRUE;
                if (!TablesInUse.DeleteNode(pNode))
                    wxLogDebug (s,wxT("Unable to delete node!"));
            }
            else
                pNode = pNode->Next();
        }
        if (!found)
        {
            wxString msg;
            msg.Printf(wxT("Unable to find the tableID in the linked\nlist of tables in use.\n\n%s"),s);
            wxLogDebug (msg,wxT("NOTICE..."));
        }
    }
#endif

    // Decrement the wxDb table count
    if (pDb)
        pDb->decrementTableCount();

    // Delete memory allocated for column definitions
    if (colDefs)
        delete [] colDefs;

    // Free statement handles
    if (!queryOnly)
    {
        if (hstmtInsert)
        {
/*
ODBC 3.0 says to use this form
            if (SQLFreeHandle(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
*/
            if (SQLFreeStmt(hstmtInsert, SQL_DROP) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc);
        }

        if (hstmtDelete)
        {
/*
ODBC 3.0 says to use this form
            if (SQLFreeHandle(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
*/
            if (SQLFreeStmt(hstmtDelete, SQL_DROP) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc);
        }

        if (hstmtUpdate)
        {
/*
ODBC 3.0 says to use this form
            if (SQLFreeHandle(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
*/
            if (SQLFreeStmt(hstmtUpdate, SQL_DROP) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc);
        }
    }

    if (hstmtInternal)
    {
        if (SQLFreeStmt(hstmtInternal, SQL_DROP) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc);
    }

    // Delete dynamically allocated cursors
    if (hstmtDefault)
        DeleteCursor(hstmtDefault);

    if (hstmtCount)
        DeleteCursor(hstmtCount);
}  // wxDbTable::cleanup()


/***************************** PRIVATE FUNCTIONS *****************************/


/********** wxDbTable::bindUpdateParams() **********/
bool wxDbTable::bindParams(bool forUpdate)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);
    
    SWORD   fSqlType    = 0;
    UDWORD  precision   = 0;
    SWORD   scale       = 0;
    
    // Bind each column of the table that should be bound
    // to a parameter marker
    int i;
	 UWORD colNo;
    for (i = 0, colNo = 1; i < noCols; i++)
    {
        if (forUpdate)
        {
            if (! colDefs[i].Updateable)
                continue;
        }
        else
        {
            if (! colDefs[i].InsertAllowed)
                continue;
        }

        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                fSqlType = pDb->GetTypeInfVarchar().FsqlType;
                precision = colDefs[i].SzDataObj;
                scale = 0;
                if (colDefs[i].Null)
                    colDefs[i].CbValue = SQL_NULL_DATA;
                else
                    colDefs[i].CbValue = SQL_NTS;
                break;
            case DB_DATA_TYPE_INTEGER:
                fSqlType = pDb->GetTypeInfInteger().FsqlType;
                precision = pDb->GetTypeInfInteger().Precision;
                scale = 0;
                if (colDefs[i].Null)
                    colDefs[i].CbValue = SQL_NULL_DATA;
                else
                    colDefs[i].CbValue = 0;
                break;
            case DB_DATA_TYPE_FLOAT:
                fSqlType = pDb->GetTypeInfFloat().FsqlType;
                precision = pDb->GetTypeInfFloat().Precision;
                scale = pDb->GetTypeInfFloat().MaximumScale;       
                // SQL Sybase Anywhere v5.5 returned a negative number for the
                // MaxScale.  This caused ODBC to kick out an error on ibscale.
                // I check for this here and set the scale = precision.
                //if (scale < 0)
                // scale = (short) precision;
                if (colDefs[i].Null)
                    colDefs[i].CbValue = SQL_NULL_DATA;
                else
                    colDefs[i].CbValue = 0;
                break;
            case DB_DATA_TYPE_DATE:
                fSqlType = pDb->GetTypeInfDate().FsqlType;
                precision = pDb->GetTypeInfDate().Precision;
                scale = 0;
                if (colDefs[i].Null)
                    colDefs[i].CbValue = SQL_NULL_DATA;
                else
                    colDefs[i].CbValue = 0;
                break;
            case DB_DATA_TYPE_BLOB:
                fSqlType = pDb->GetTypeInfBlob().FsqlType;
                precision = 50000;
                scale = 0;
                if (colDefs[i].Null)
                    colDefs[i].CbValue = SQL_NULL_DATA;
                else
                    colDefs[i].CbValue = SQL_LEN_DATA_AT_EXEC(colDefs[i].SzDataObj);
                break;
        }
        if (forUpdate)
        {
            if (SQLBindParameter(hstmtUpdate, colNo++, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
                                 fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
                                 precision+1, &colDefs[i].CbValue) != SQL_SUCCESS)
            {
                return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
            }
        }
        else
        {
            if (SQLBindParameter(hstmtInsert, colNo++, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
                                 fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
                                 precision+1,&colDefs[i].CbValue) != SQL_SUCCESS)
            {
                return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
            }
        }
    }
    
    // Completed successfully
    return(TRUE);

}  // wxDbTable::bindParams()


/********** wxDbTable::bindInsertParams() **********/
bool wxDbTable::bindInsertParams(void)
{
    return bindParams(FALSE);
}  // wxDbTable::bindInsertParams()


/********** wxDbTable::bindUpdateParams() **********/
bool wxDbTable::bindUpdateParams(void)
{
    return bindParams(TRUE);
}  // wxDbTable::bindUpdateParams()


/********** wxDbTable::bindCols() **********/
bool wxDbTable::bindCols(HSTMT cursor)
{
//RG-NULL    static SDWORD  cb;
    
    // Bind each column of the table to a memory address for fetching data
    UWORD i;
    for (i = 0; i < noCols; i++)
    {
        if (SQLBindCol(cursor, (UWORD)(i+1), colDefs[i].SqlCtype, (UCHAR*) colDefs[i].PtrDataObj,
                       colDefs[i].SzDataObj, &colDefs[i].CbValue ) != SQL_SUCCESS)
        {
          return (pDb->DispAllErrors(henv, hdbc, cursor));
        }
    }

    // Completed successfully
    return(TRUE);

}  // wxDbTable::bindCols()


/********** wxDbTable::getRec() **********/
bool wxDbTable::getRec(UWORD fetchType)
{
    RETCODE retcode;

    if (!pDb->FwdOnlyCursors())
    {
        // Fetch the NEXT, PREV, FIRST or LAST record, depending on fetchType
        UDWORD  cRowsFetched;
        UWORD   rowStatus;

        retcode = SQLExtendedFetch(hstmt, fetchType, 0, &cRowsFetched, &rowStatus);
        if (retcode  != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            if (retcode == SQL_NO_DATA_FOUND)
                return(FALSE);
            else
                return(pDb->DispAllErrors(henv, hdbc, hstmt));
        }
        else
        {
            // Set the Null member variable to indicate the Null state
            // of each column just read in.
            int i;
            for (i = 0; i < noCols; i++)
                colDefs[i].Null = (colDefs[i].CbValue == SQL_NULL_DATA);
        }
    }
    else
    {
        // Fetch the next record from the record set
        retcode = SQLFetch(hstmt);
        if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        {
            if (retcode == SQL_NO_DATA_FOUND)
                return(FALSE);
            else
                return(pDb->DispAllErrors(henv, hdbc, hstmt));
        }
        else
        {
            // Set the Null member variable to indicate the Null state
            // of each column just read in.
            int i;
            for (i = 0; i < noCols; i++)
                colDefs[i].Null = (colDefs[i].CbValue == SQL_NULL_DATA);
        }
    }

    // Completed successfully
    return(TRUE);

}  // wxDbTable::getRec()


/********** wxDbTable::execDelete() **********/
bool wxDbTable::execDelete(const wxString &pSqlStmt)
{
    // Execute the DELETE statement
    if (SQLExecDirect(hstmtDelete, (UCHAR FAR *) pSqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));

    // Record deleted successfully
    return(TRUE);

}  // wxDbTable::execDelete()


/********** wxDbTable::execUpdate() **********/
bool wxDbTable::execUpdate(const wxString &pSqlStmt)
{
    // Execute the UPDATE statement
    if (SQLExecDirect(hstmtUpdate, (UCHAR FAR *) pSqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));

    // Record deleted successfully
    return(TRUE);

}  // wxDbTable::execUpdate()


/********** wxDbTable::query() **********/
bool wxDbTable::query(int queryType, bool forUpdate, bool distinct, const wxString &pSqlStmt)
{
    wxString sqlStmt;

    if (forUpdate)
        // The user may wish to select for update, but the DBMS may not be capable
        selectForUpdate = CanSelectForUpdate();
    else
        selectForUpdate = FALSE;

    // Set the SQL SELECT string
    if (queryType != DB_SELECT_STATEMENT)               // A select statement was not passed in,
    {                                                   // so generate a select statement.
        BuildSelectStmt(sqlStmt, queryType, distinct);
        pDb->WriteSqlLog(sqlStmt);
    }
/*
   This is the block of code that got added during the 2.2.1 merge with 
   the 2.2 main branch that somehow got added here when it should not have.  - gt

    else 
        wxStrcpy(sqlStmt, pSqlStmt);

    SQLFreeStmt(hstmt, SQL_CLOSE);
    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt, SQL_NTS) == SQL_SUCCESS)
        return(TRUE);
    else
    {
        pDb->DispAllErrors(henv, hdbc, hstmt);
        return(FALSE);
    }
*/
    // Make sure the cursor is closed first
    if (!CloseCursor(hstmt))
        return(FALSE);

    // Execute the SQL SELECT statement
    int retcode;     
    retcode = SQLExecDirect(hstmt, (UCHAR FAR *) (queryType == DB_SELECT_STATEMENT ? pSqlStmt.c_str() : sqlStmt.c_str()), SQL_NTS);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        return(pDb->DispAllErrors(henv, hdbc, hstmt));

    // Completed successfully
    return(TRUE);

}  // wxDbTable::query()


/***************************** PUBLIC FUNCTIONS *****************************/


/********** wxDbTable::Open() **********/
bool wxDbTable::Open(bool checkPrivileges, bool checkTableExists)
{
    if (!pDb)
        return FALSE;   

    int i;
    wxString sqlStmt;
    wxString s;

    s.Empty();
    // Verify that the table exists in the database
    if (checkTableExists && !pDb->TableExists(tableName, pDb->GetUsername(), tablePath))
    {
        s = wxT("Table/view does not exist in the database");
        if ( *(pDb->dbInf.accessibleTables) == wxT('Y'))
            s += wxT(", or you have no permissions.\n");
        else
            s += wxT(".\n");
    }
    else if (checkPrivileges)
    {
        // Verify the user has rights to access the table.
        // Shortcut boolean evaluation to optimize out call to 
        // TablePrivileges
        //
        // Unfortunately this optimization doesn't seem to be
        // reliable!
        if (// *(pDb->dbInf.accessibleTables) == 'N' && 
            !pDb->TablePrivileges(tableName,wxT("SELECT"), pDb->GetUsername(), pDb->GetUsername(), tablePath))
            s = wxT("Current logged in user does not have sufficient privileges to access this table.\n");
    }

    if (!s.IsEmpty())
    {
        wxString p;

        if (!tablePath.IsEmpty())
            p.Printf(wxT("Error opening '%s/%s'.\n"),tablePath.c_str(),tableName.c_str());
        else
            p.Printf(wxT("Error opening '%s'.\n"), tableName.c_str());

        p += s;
        pDb->LogError(p.GetData());

        return(FALSE);
    }

    // Bind the member variables for field exchange between
    // the wxDbTable object and the ODBC record.
    if (!queryOnly)
    {
        if (!bindInsertParams())                    // Inserts
            return(FALSE);
       
        if (!bindUpdateParams())                    // Updates
            return(FALSE);
    }

    if (!bindCols(*hstmtDefault))                   // Selects
        return(FALSE);
    
    if (!bindCols(hstmtInternal))                   // Internal use only
        return(FALSE);

     /*
     * Do NOT bind the hstmtCount cursor!!!
     */

    // Build an insert statement using parameter markers
    if (!queryOnly && noCols > 0)
    {
        bool needComma = FALSE;
        sqlStmt.Printf(wxT("INSERT INTO %s ("), tableName.c_str());
        for (i = 0; i < noCols; i++)
        {
            if (! colDefs[i].InsertAllowed)
                continue;
            if (needComma)
                sqlStmt += wxT(",");
            sqlStmt += colDefs[i].ColName;
            needComma = TRUE;
        }
        needComma = FALSE;
        sqlStmt += wxT(") VALUES (");

        int insertableCount = 0;

        for (i = 0; i < noCols; i++)
        {
            if (! colDefs[i].InsertAllowed)
                continue;
            if (needComma)
                sqlStmt += wxT(",");
            sqlStmt += wxT("?");
            needComma = TRUE;
            insertableCount++;
        }
        sqlStmt += wxT(")");
        
        // Prepare the insert statement for execution
        if (insertableCount)  
        {
            if (SQLPrepare(hstmtInsert, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
                return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
        }
        else 
            insertable= FALSE;
    }
    
    // Completed successfully
    return(TRUE);

}  // wxDbTable::Open()


/********** wxDbTable::Query() **********/
bool wxDbTable::Query(bool forUpdate, bool distinct)
{

    return(query(DB_SELECT_WHERE, forUpdate, distinct));

}  // wxDbTable::Query()


/********** wxDbTable::QueryBySqlStmt() **********/
bool wxDbTable::QueryBySqlStmt(const wxString &pSqlStmt)
{
    pDb->WriteSqlLog(pSqlStmt);

    return(query(DB_SELECT_STATEMENT, FALSE, FALSE, pSqlStmt));

}  // wxDbTable::QueryBySqlStmt()


/********** wxDbTable::QueryMatching() **********/
bool wxDbTable::QueryMatching(bool forUpdate, bool distinct)
{

    return(query(DB_SELECT_MATCHING, forUpdate, distinct));

}  // wxDbTable::QueryMatching()


/********** wxDbTable::QueryOnKeyFields() **********/
bool wxDbTable::QueryOnKeyFields(bool forUpdate, bool distinct)
{

    return(query(DB_SELECT_KEYFIELDS, forUpdate, distinct));

}  // wxDbTable::QueryOnKeyFields()


/********** wxDbTable::GetPrev() **********/
bool wxDbTable::GetPrev(void)
{
    if (pDb->FwdOnlyCursors())
    {
        wxFAIL_MSG(wxT("GetPrev()::Backward scrolling cursors are not enabled for this instance of wxDbTable"));
        return FALSE;
    }
    else
        return(getRec(SQL_FETCH_PRIOR));

}  // wxDbTable::GetPrev()


/********** wxDbTable::operator-- **********/
bool wxDbTable::operator--(int)
{
    if (pDb->FwdOnlyCursors())
    {
        wxFAIL_MSG(wxT("operator--:Backward scrolling cursors are not enabled for this instance of wxDbTable"));
        return FALSE;
    }
    else
        return(getRec(SQL_FETCH_PRIOR));

}  // wxDbTable::operator--


/********** wxDbTable::GetFirst() **********/
bool wxDbTable::GetFirst(void)
{
    if (pDb->FwdOnlyCursors())
    {
        wxFAIL_MSG(wxT("GetFirst():Backward scrolling cursors are not enabled for this instance of wxDbTable"));
        return FALSE;
    }
    else
        return(getRec(SQL_FETCH_FIRST));

}  // wxDbTable::GetFirst()


/********** wxDbTable::GetLast() **********/
bool wxDbTable::GetLast(void)
{
    if (pDb->FwdOnlyCursors())
    {
        wxFAIL_MSG(wxT("GetLast()::Backward scrolling cursors are not enabled for this instance of wxDbTable"));
        return FALSE;
    }
    else 
        return(getRec(SQL_FETCH_LAST));

}  // wxDbTable::GetLast()


/********** wxDbTable::BuildDeleteStmt() **********/
void wxDbTable::BuildDeleteStmt(wxString &pSqlStmt, int typeOfDel, const wxString &pWhereClause)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return;

    wxString whereClause;

    whereClause.Empty();

    // Handle the case of DeleteWhere() and the where clause is blank.  It should
    // delete all records from the database in this case.
    if (typeOfDel == DB_DEL_WHERE && (pWhereClause.Length() == 0))
    {
        pSqlStmt.Printf(wxT("DELETE FROM %s"), tableName.c_str());
        return;
    }

    pSqlStmt.Printf(wxT("DELETE FROM %s WHERE "), tableName.c_str());

    // Append the WHERE clause to the SQL DELETE statement
    switch(typeOfDel)
    {
        case DB_DEL_KEYFIELDS:
            // If the datasource supports the ROWID column, build
            // the where on ROWID for efficiency purposes.
            // e.g. DELETE FROM PARTS WHERE ROWID = '111.222.333'
            if (CanUpdByROWID())
            {
                SDWORD cb;
                wxChar   rowid[wxDB_ROWID_LEN+1];

                // Get the ROWID value.  If not successful retreiving the ROWID,
                // simply fall down through the code and build the WHERE clause
                // based on the key fields.
                if (SQLGetData(hstmt, (UWORD)(noCols+1), SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
                {
                    pSqlStmt += wxT("ROWID = '");
                    pSqlStmt += rowid;
                    pSqlStmt += wxT("'");
                    break;
                }
            }
            // Unable to delete by ROWID, so build a WHERE
            // clause based on the keyfields.
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            pSqlStmt += whereClause;
            break;
        case DB_DEL_WHERE:
            pSqlStmt += pWhereClause;
            break;
        case DB_DEL_MATCHING:
            BuildWhereClause(whereClause, DB_WHERE_MATCHING);
            pSqlStmt += whereClause;
            break;
    }

}  // BuildDeleteStmt()


/***** DEPRECATED: use wxDbTable::BuildDeleteStmt(wxString &....) form *****/
void wxDbTable::BuildDeleteStmt(wxChar *pSqlStmt, int typeOfDel, const wxString &pWhereClause)
{
    wxString tempSqlStmt;
    BuildDeleteStmt(tempSqlStmt, typeOfDel, pWhereClause);
    wxStrcpy(pSqlStmt, tempSqlStmt);
}  // wxDbTable::BuildDeleteStmt()


/********** wxDbTable::BuildSelectStmt() **********/
void wxDbTable::BuildSelectStmt(wxString &pSqlStmt, int typeOfSelect, bool distinct)
{
    wxString whereClause;
    whereClause.Empty();

    // Build a select statement to query the database
    pSqlStmt = wxT("SELECT ");

    // SELECT DISTINCT values only?
    if (distinct)
        pSqlStmt += wxT("DISTINCT ");

    // Was a FROM clause specified to join tables to the base table?
    // Available for ::Query() only!!!
    bool appendFromClause = FALSE;
#if wxODBC_BACKWARD_COMPATABILITY
    if (typeOfSelect == DB_SELECT_WHERE && from && wxStrlen(from))
        appendFromClause = TRUE;
#else
    if (typeOfSelect == DB_SELECT_WHERE && from.Length())
        appendFromClause = TRUE;
#endif

    // Add the column list
    int i;
    for (i = 0; i < noCols; i++)
    {
        // If joining tables, the base table column names must be qualified to avoid ambiguity
        if (appendFromClause)
        {
            pSqlStmt += queryTableName;
            pSqlStmt += wxT(".");
        }
        pSqlStmt += colDefs[i].ColName;
        if (i + 1 < noCols)
            pSqlStmt += wxT(",");
    }

    // If the datasource supports ROWID, get this column as well.  Exception: Don't retrieve
    // the ROWID if querying distinct records.  The rowid will always be unique.
    if (!distinct && CanUpdByROWID())
    {
        // If joining tables, the base table column names must be qualified to avoid ambiguity
        if (appendFromClause)
        {
            pSqlStmt += wxT(",");
            pSqlStmt += queryTableName;
            pSqlStmt += wxT(".ROWID");
        }
        else
            pSqlStmt += wxT(",ROWID");
    }

    // Append the FROM tablename portion
    pSqlStmt += wxT(" FROM ");
    pSqlStmt += queryTableName;

    // Sybase uses the HOLDLOCK keyword to lock a record during query.
    // The HOLDLOCK keyword follows the table name in the from clause.
    // Each table in the from clause must specify HOLDLOCK or
    // NOHOLDLOCK (the default).  Note: The "FOR UPDATE" clause
    // is parsed but ignored in SYBASE Transact-SQL.
    if (selectForUpdate && (pDb->Dbms() == dbmsSYBASE_ASA || pDb->Dbms() == dbmsSYBASE_ASE))
        pSqlStmt += wxT(" HOLDLOCK");

    if (appendFromClause)
        pSqlStmt += from;

    // Append the WHERE clause.  Either append the where clause for the class
    // or build a where clause.  The typeOfSelect determines this.
    switch(typeOfSelect)
    {
        case DB_SELECT_WHERE:
#if wxODBC_BACKWARD_COMPATABILITY
            if (where && wxStrlen(where))   // May not want a where clause!!!
#else
            if (where.Length())   // May not want a where clause!!!
#endif
            {
                pSqlStmt += wxT(" WHERE ");
                pSqlStmt += where;
            }
            break;
        case DB_SELECT_KEYFIELDS:
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            if (whereClause.Length())
            {
                pSqlStmt += wxT(" WHERE ");
                pSqlStmt += whereClause;
            }
            break;
        case DB_SELECT_MATCHING:
            BuildWhereClause(whereClause, DB_WHERE_MATCHING);
            if (whereClause.Length())
            {
                pSqlStmt += wxT(" WHERE ");
                pSqlStmt += whereClause;
            }
            break;
    }

    // Append the ORDER BY clause
#if wxODBC_BACKWARD_COMPATABILITY
    if (orderBy && wxStrlen(orderBy))
#else
    if (orderBy.Length())
#endif
    {
        pSqlStmt += wxT(" ORDER BY ");
        pSqlStmt += orderBy;
    }

    // SELECT FOR UPDATE if told to do so and the datasource is capable.  Sybase
    // parses the FOR UPDATE clause but ignores it.  See the comment above on the
    // HOLDLOCK for Sybase.
    if (selectForUpdate && CanSelectForUpdate())
        pSqlStmt += wxT(" FOR UPDATE");

}  // wxDbTable::BuildSelectStmt()


/***** DEPRECATED: use wxDbTable::BuildSelectStmt(wxString &....) form *****/
void wxDbTable::BuildSelectStmt(wxChar *pSqlStmt, int typeOfSelect, bool distinct)
{
    wxString tempSqlStmt;
    BuildSelectStmt(tempSqlStmt, typeOfSelect, distinct);
    wxStrcpy(pSqlStmt, tempSqlStmt);
}  // wxDbTable::BuildSelectStmt()


/********** wxDbTable::BuildUpdateStmt() **********/
void wxDbTable::BuildUpdateStmt(wxString &pSqlStmt, int typeOfUpd, const wxString &pWhereClause)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return;

    wxString whereClause;
    whereClause.Empty();

    bool firstColumn = TRUE;

    pSqlStmt.Printf(wxT("UPDATE %s SET "), tableName.c_str());

    // Append a list of columns to be updated
    int i;
    for (i = 0; i < noCols; i++)
    {
        // Only append Updateable columns
        if (colDefs[i].Updateable)
        {
            if (! firstColumn)
                pSqlStmt += wxT(",");
            else
                firstColumn = FALSE;
            pSqlStmt += colDefs[i].ColName;
            pSqlStmt += wxT(" = ?");
        }
    }

    // Append the WHERE clause to the SQL UPDATE statement
    pSqlStmt += wxT(" WHERE ");
    switch(typeOfUpd)
    {
        case DB_UPD_KEYFIELDS:
            // If the datasource supports the ROWID column, build
            // the where on ROWID for efficiency purposes.
            // e.g. UPDATE PARTS SET Col1 = ?, Col2 = ? WHERE ROWID = '111.222.333'
            if (CanUpdByROWID())
            {
                SDWORD cb;
                wxChar rowid[wxDB_ROWID_LEN+1];

                // Get the ROWID value.  If not successful retreiving the ROWID,
                // simply fall down through the code and build the WHERE clause
                // based on the key fields.
                if (SQLGetData(hstmt, (UWORD)(noCols+1), SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
                {
                    pSqlStmt += wxT("ROWID = '");
                    pSqlStmt += rowid;
                    pSqlStmt += wxT("'");
                    break;
                }
            }
            // Unable to delete by ROWID, so build a WHERE
            // clause based on the keyfields.
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            pSqlStmt += whereClause;
            break;
        case DB_UPD_WHERE:
            pSqlStmt += pWhereClause;
            break;
    }
}  // BuildUpdateStmt()


/***** DEPRECATED: use wxDbTable::BuildUpdateStmt(wxString &....) form *****/
void wxDbTable::BuildUpdateStmt(wxChar *pSqlStmt, int typeOfUpd, const wxString &pWhereClause)
{
    wxString tempSqlStmt;
    BuildUpdateStmt(tempSqlStmt, typeOfUpd, pWhereClause);
    wxStrcpy(pSqlStmt, tempSqlStmt);
}  // BuildUpdateStmt()


/********** wxDbTable::BuildWhereClause() **********/
void wxDbTable::BuildWhereClause(wxString &pWhereClause, int typeOfWhere,
                                 const wxString &qualTableName, bool useLikeComparison)
/*
 * Note: BuildWhereClause() currently ignores timestamp columns.
 *       They are not included as part of the where clause.
 */
{
    bool moreThanOneColumn = FALSE;
    wxString colValue;

    // Loop through the columns building a where clause as you go
    int i;
    for (i = 0; i < noCols; i++)
    {
        // Determine if this column should be included in the WHERE clause
        if ((typeOfWhere == DB_WHERE_KEYFIELDS && colDefs[i].KeyField) ||
             (typeOfWhere == DB_WHERE_MATCHING  && (!IsColNull(i))))
        {
            // Skip over timestamp columns
            if (colDefs[i].SqlCtype == SQL_C_TIMESTAMP)
                continue;
            // If there is more than 1 column, join them with the keyword "AND"
            if (moreThanOneColumn)
                pWhereClause += wxT(" AND ");
            else
                moreThanOneColumn = TRUE;
            // Concatenate where phrase for the column
            if (qualTableName.Length())
            {
                pWhereClause += qualTableName;
                pWhereClause += wxT(".");
            }
            pWhereClause += colDefs[i].ColName;
            if (useLikeComparison && (colDefs[i].SqlCtype == SQL_C_CHAR))
                pWhereClause += wxT(" LIKE ");
            else
                pWhereClause += wxT(" = ");
            switch(colDefs[i].SqlCtype)
            {
                case SQL_C_CHAR:
                    colValue.Printf(wxT("'%s'"), (UCHAR FAR *) colDefs[i].PtrDataObj);
                    break;
                case SQL_C_SSHORT:
                    colValue.Printf(wxT("%hi"), *((SWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_USHORT:
                    colValue.Printf(wxT("%hu"), *((UWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_SLONG:
                    colValue.Printf(wxT("%li"), *((SDWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_ULONG:
                    colValue.Printf(wxT("%lu"), *((UDWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_FLOAT:
                    colValue.Printf(wxT("%.6f"), *((SFLOAT *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_DOUBLE:
                    colValue.Printf(wxT("%.6f"), *((SDOUBLE *) colDefs[i].PtrDataObj));
                    break;
            }
            pWhereClause += colValue;
        }
    }
}  // wxDbTable::BuildWhereClause()


/***** DEPRECATED: use wxDbTable::BuildWhereClause(wxString &....) form *****/
void wxDbTable::BuildWhereClause(wxChar *pWhereClause, int typeOfWhere,
                                 const wxString &qualTableName, bool useLikeComparison)
{
    wxString tempSqlStmt;
    BuildWhereClause(tempSqlStmt, typeOfWhere, qualTableName, useLikeComparison);
    wxStrcpy(pWhereClause, tempSqlStmt);
}  // wxDbTable::BuildWhereClause()


/********** wxDbTable::GetRowNum() **********/
UWORD wxDbTable::GetRowNum(void)
{
    UDWORD rowNum;

    if (SQLGetStmtOption(hstmt, SQL_ROW_NUMBER, (UCHAR*) &rowNum) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, hstmt);
        return(0);
    }

    // Completed successfully
    return((UWORD) rowNum);

}  // wxDbTable::GetRowNum()


/********** wxDbTable::CloseCursor() **********/
bool wxDbTable::CloseCursor(HSTMT cursor)
{
    if (SQLFreeStmt(cursor, SQL_CLOSE) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, cursor));

    // Completed successfully
    return(TRUE);

}  // wxDbTable::CloseCursor()


/********** wxDbTable::CreateTable() **********/
bool wxDbTable::CreateTable(bool attemptDrop)
{
    if (!pDb)
        return FALSE;

    int i, j;
    wxString sqlStmt;

#ifdef DBDEBUG_CONSOLE
    cout << wxT("Creating Table ") << tableName << wxT("...") << endl;
#endif

    // Drop table first
    if (attemptDrop && !DropTable())
        return FALSE;

    // Create the table
#ifdef DBDEBUG_CONSOLE
    for (i = 0; i < noCols; i++)
    {
        // Exclude derived columns since they are NOT part of the base table
        if (colDefs[i].DerivedCol)
            continue;
        cout << i + 1 << wxT(": ") << colDefs[i].ColName << wxT("; ");
        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                cout << pDb->typeInfVarchar.TypeName << wxT("(") << colDefs[i].SzDataObj << wxT(")");
                break;
            case DB_DATA_TYPE_INTEGER:
                cout << pDb->typeInfInteger.TypeName;
                break;
            case DB_DATA_TYPE_FLOAT:
                cout << pDb->typeInfFloat.TypeName;
                break;
            case DB_DATA_TYPE_DATE:
                cout << pDb->typeInfDate.TypeName;
                break;
            case DB_DATA_TYPE_BLOB:
                cout << pDb->typeInfBlob.TypeName;
                break;
        }
        cout << endl;
    }
#endif

    // Build a CREATE TABLE string from the colDefs structure.
    bool needComma = FALSE;
    sqlStmt.Printf(wxT("CREATE TABLE %s ("), tableName.c_str());

    for (i = 0; i < noCols; i++)
    {
        // Exclude derived columns since they are NOT part of the base table
        if (colDefs[i].DerivedCol)
            continue;
        // Comma Delimiter
        if (needComma)
            sqlStmt += wxT(",");
        // Column Name
        sqlStmt += colDefs[i].ColName;
        sqlStmt += wxT(" ");
        // Column Type
        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                sqlStmt += pDb->GetTypeInfVarchar().TypeName;
                break;
            case DB_DATA_TYPE_INTEGER:
                sqlStmt += pDb->GetTypeInfInteger().TypeName;
                break;
            case DB_DATA_TYPE_FLOAT:
                sqlStmt += pDb->GetTypeInfFloat().TypeName;
                break;
            case DB_DATA_TYPE_DATE:
                sqlStmt += pDb->GetTypeInfDate().TypeName;
                break;
            case DB_DATA_TYPE_BLOB:
                sqlStmt += pDb->GetTypeInfBlob().TypeName;
                break;
        }
        // For varchars, append the size of the string
        if (colDefs[i].DbDataType == DB_DATA_TYPE_VARCHAR)// ||
//            colDefs[i].DbDataType == DB_DATA_TYPE_BLOB)
        {
            wxString s;
            s.Printf(wxT("(%d)"), colDefs[i].SzDataObj);
            sqlStmt += s;
        }

        if (pDb->Dbms() == dbmsDB2 ||
            pDb->Dbms() == dbmsMY_SQL ||
            pDb->Dbms() == dbmsSYBASE_ASE  ||
            pDb->Dbms() == dbmsMS_SQL_SERVER)
        {
            if (colDefs[i].KeyField)
            {
                sqlStmt += wxT(" NOT NULL");
            }
        }
        
        needComma = TRUE;
    }
    // If there is a primary key defined, include it in the create statement
    for (i = j = 0; i < noCols; i++)
    {
        if (colDefs[i].KeyField)
        {
            j++;
            break;
        }
    }
    if (j && pDb->Dbms() != dbmsDBASE)  // Found a keyfield
    {
        if (pDb->Dbms() != dbmsMY_SQL)
        {
            sqlStmt += wxT(",CONSTRAINT ");
            sqlStmt += tableName;
            sqlStmt += wxT("_PIDX PRIMARY KEY (");
        }
        else
        {
            /* MySQL goes out on this one. We also declare the relevant key NON NULL above */
            sqlStmt += wxT(", PRIMARY KEY (");
        }

        // List column name(s) of column(s) comprising the primary key
        for (i = j = 0; i < noCols; i++)
        {
            if (colDefs[i].KeyField)
            {
                if (j++) // Multi part key, comma separate names
                    sqlStmt += wxT(",");
                sqlStmt += colDefs[i].ColName;
            }
        }
       sqlStmt += wxT(")");
    }
    // Append the closing parentheses for the create table statement
    sqlStmt += wxT(")");

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl;
#endif

    // Execute the CREATE TABLE statement
    RETCODE retcode = SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
        pDb->DispAllErrors(henv, hdbc, hstmt);
        pDb->RollbackTrans();
        CloseCursor(hstmt);
        return(FALSE);
    }

    // Commit the transaction and close the cursor
    if (!pDb->CommitTrans())
        return(FALSE);
    if (!CloseCursor(hstmt))
        return(FALSE);

    // Database table created successfully
    return(TRUE);

} // wxDbTable::CreateTable()


/********** wxDbTable::DropTable() **********/
bool wxDbTable::DropTable()
{
    // NOTE: This function returns TRUE if the Table does not exist, but
    //       only for identified databases.  Code will need to be added
    //       below for any other databases when those databases are defined
    //       to handle this situation consistently

    wxString sqlStmt;

    sqlStmt.Printf(wxT("DROP TABLE %s"), tableName.c_str());

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl;
#endif

    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        // Check for "Base table not found" error and ignore
        pDb->GetNextError(henv, hdbc, hstmt);   
        if (wxStrcmp(pDb->sqlState, wxT("S0002")) &&
            wxStrcmp(pDb->sqlState, wxT("S1000")))  // "Base table not found" 
        {    
            // Check for product specific error codes
            if (!((pDb->Dbms() == dbmsSYBASE_ASA  && !wxStrcmp(pDb->sqlState,wxT("42000")))   ||  // 5.x (and lower?)
                (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,wxT("37000")))   ||   
                (pDb->Dbms() == dbmsPOSTGRES      && !wxStrcmp(pDb->sqlState,wxT("08S01")))))     
            {
                pDb->DispNextError();
                pDb->DispAllErrors(henv, hdbc, hstmt);
                pDb->RollbackTrans();
                CloseCursor(hstmt);
                return(FALSE);
            }
        }
    }

    // Commit the transaction and close the cursor
    if (! pDb->CommitTrans())
        return(FALSE);
    if (! CloseCursor(hstmt))
        return(FALSE);

    return(TRUE);
}  // wxDbTable::DropTable()


/********** wxDbTable::CreateIndex() **********/
bool wxDbTable::CreateIndex(const wxString &idxName, bool unique, int noIdxCols, wxDbIdxDef *pIdxDefs, bool attemptDrop)
{
    wxString sqlStmt;

    // Drop the index first
    if (attemptDrop && !DropIndex(idxName))
        return (FALSE);

    // MySQL (and possibly Sybase ASE?? - gt) require that any columns which are used as portions
    // of an index have the columns defined as "NOT NULL".  During initial table creation though,
    // it may not be known which columns are necessarily going to be part of an index (e.g. the
    // table was created, then months later you determine that an additional index while
    // give better performance, so you want to add an index).
    //
    // The following block of code will modify the column definition to make the column be
    // defined with the "NOT NULL" qualifier.
    if (pDb->Dbms() == dbmsMY_SQL)
    {
        wxString sqlStmt;
        int i;
        bool ok = TRUE;
        for (i = 0; i < noIdxCols && ok; i++)
        {
            int   j = 0;
            bool  found = FALSE;
            // Find the column definition that has the ColName that matches the
            // index column name.  We need to do this to get the DB_DATA_TYPE of
            // the index column, as MySQL's syntax for the ALTER column requires
            // this information
            while (!found && (j < this->noCols))
            {
                if (wxStrcmp(colDefs[j].ColName,pIdxDefs[i].ColName) == 0)
                    found = TRUE;
                if (!found)
                    j++;
            }
            
            if (found)
            {
                ok = pDb->ModifyColumn(tableName, pIdxDefs[i].ColName,
                                        colDefs[j].DbDataType, colDefs[j].SzDataObj,
                                        wxT("NOT NULL"));

                if (!ok)
                {
                    wxODBC_ERRORS retcode;
                    // Oracle returns a DB_ERR_GENERAL_ERROR if the column is already
                    // defined to be NOT NULL, but reportedly MySQL doesn't mind.
                    // This line is just here for debug checking of the value
                    retcode = (wxODBC_ERRORS)pDb->DB_STATUS;
                }
            }
            else
                ok = FALSE;
        }
        if (ok)
            pDb->CommitTrans();
        else
        {
            pDb->RollbackTrans();
            return(FALSE);
        }
    }
    
    // Build a CREATE INDEX statement
    sqlStmt = wxT("CREATE ");
    if (unique)
        sqlStmt += wxT("UNIQUE ");
    
    sqlStmt += wxT("INDEX ");
    sqlStmt += idxName;
    sqlStmt += wxT(" ON ");
    sqlStmt += tableName;
    sqlStmt += wxT(" (");
    
    // Append list of columns making up index
    int i;
    for (i = 0; i < noIdxCols; i++)
    {
        sqlStmt += pIdxDefs[i].ColName;
        /* Postgres doesn't cope with ASC */
        if (pDb->Dbms() != dbmsPOSTGRES)
        {
            if (pIdxDefs[i].Ascending)
                sqlStmt += wxT(" ASC");
            else
                sqlStmt += wxT(" DESC");
        }

        if ((i + 1) < noIdxCols)
            sqlStmt += wxT(",");
    }
    
    // Append closing parentheses
    sqlStmt += wxT(")");

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl << endl;
#endif

    // Execute the CREATE INDEX statement
    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, hstmt);
        pDb->RollbackTrans();
        CloseCursor(hstmt);
        return(FALSE);
    }

    // Commit the transaction and close the cursor
    if (! pDb->CommitTrans())
        return(FALSE);
    if (! CloseCursor(hstmt))
        return(FALSE);

    // Index Created Successfully
    return(TRUE);

}  // wxDbTable::CreateIndex()


/********** wxDbTable::DropIndex() **********/
bool wxDbTable::DropIndex(const wxString &idxName)
{
    // NOTE: This function returns TRUE if the Index does not exist, but
    //       only for identified databases.  Code will need to be added
    //          below for any other databases when those databases are defined
    //       to handle this situation consistently

    wxString sqlStmt;

    if (pDb->Dbms() == dbmsACCESS || pDb->Dbms() == dbmsMY_SQL)
        sqlStmt.Printf(wxT("DROP INDEX %s ON %s"),idxName.c_str(), tableName.c_str());
    else if ((pDb->Dbms() == dbmsMS_SQL_SERVER) ||
             (pDb->Dbms() == dbmsSYBASE_ASE))
        sqlStmt.Printf(wxT("DROP INDEX %s.%s"),tableName.c_str(), idxName.c_str());
    else
        sqlStmt.Printf(wxT("DROP INDEX %s"),idxName.c_str());

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl;
#endif

    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        // Check for "Index not found" error and ignore
        pDb->GetNextError(henv, hdbc, hstmt);
        if (wxStrcmp(pDb->sqlState,wxT("S0012")))  // "Index not found"
        {
            // Check for product specific error codes
            if (!((pDb->Dbms() == dbmsSYBASE_ASA    && !wxStrcmp(pDb->sqlState,wxT("42000"))) ||  // v5.x (and lower?)
                  (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,wxT("37000"))) ||
                  (pDb->Dbms() == dbmsMS_SQL_SERVER && !wxStrcmp(pDb->sqlState,wxT("S1000"))) ||
                  (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,wxT("S0002"))) ||  // Base table not found
                  (pDb->Dbms() == dbmsMY_SQL        && !wxStrcmp(pDb->sqlState,wxT("42S12"))) ||  // tested by Christopher Ludwik Marino-Cebulski using v3.23.21beta
                  (pDb->Dbms() == dbmsPOSTGRES      && !wxStrcmp(pDb->sqlState,wxT("08S01")))
               ))
            {
                pDb->DispNextError();
                pDb->DispAllErrors(henv, hdbc, hstmt);
                pDb->RollbackTrans();
                CloseCursor(hstmt);
                return(FALSE);
            }
        }
    }

    // Commit the transaction and close the cursor
    if (! pDb->CommitTrans())
        return(FALSE);
    if (! CloseCursor(hstmt))
        return(FALSE);

    return(TRUE);
}  // wxDbTable::DropIndex()


/********** wxDbTable::SetOrderByColNums() **********/
bool wxDbTable::SetOrderByColNums(int first, ... )
{
    int         colNo = first;
    va_list     argptr;

    bool        abort = FALSE;
    wxString    tempStr;

    va_start(argptr, first);     /* Initialize variable arguments. */
    while (!abort && (colNo != wxDB_NO_MORE_COLUMN_NUMBERS))
    {
        // Make sure the passed in column number
        // is within the valid range of columns
        //
        // Valid columns are 0 thru noCols-1
        if (colNo >= noCols || colNo < 0)
        {
            abort = TRUE;
            continue;
        }

        if (colNo != first)
            tempStr += wxT(",");

        tempStr += colDefs[colNo].ColName;
        colNo = va_arg (argptr, int);
    }
    va_end (argptr);              /* Reset variable arguments.      */

    SetOrderByClause(tempStr);

    return (!abort);
}  // wxDbTable::SetOrderByColNums()


/********** wxDbTable::Insert() **********/
int wxDbTable::Insert(void)
{
    wxASSERT(!queryOnly);
    if (queryOnly || !insertable)
        return(DB_FAILURE);

    bindInsertParams();

    // Insert the record by executing the already prepared insert statement
    RETCODE retcode;
    retcode=SQLExecute(hstmtInsert);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
        // Check to see if integrity constraint was violated
        pDb->GetNextError(henv, hdbc, hstmtInsert);
        if (! wxStrcmp(pDb->sqlState, wxT("23000")))  // Integrity constraint violated
            return(DB_ERR_INTEGRITY_CONSTRAINT_VIOL);
        else
        {
            pDb->DispNextError();
            pDb->DispAllErrors(henv, hdbc, hstmtInsert);
            return(DB_FAILURE);
        }
    }

    // Record inserted into the datasource successfully
    return(DB_SUCCESS);

}  // wxDbTable::Insert()


/********** wxDbTable::Update() **********/
bool wxDbTable::Update(void)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    wxString sqlStmt;

    // Build the SQL UPDATE statement
    BuildUpdateStmt(sqlStmt, DB_UPD_KEYFIELDS);

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl << endl;
#endif

    // Execute the SQL UPDATE statement
    return(execUpdate(sqlStmt));

}  // wxDbTable::Update()


/********** wxDbTable::Update(pSqlStmt) **********/
bool wxDbTable::Update(const wxString &pSqlStmt)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    pDb->WriteSqlLog(pSqlStmt);

    return(execUpdate(pSqlStmt));

}  // wxDbTable::Update(pSqlStmt)


/********** wxDbTable::UpdateWhere() **********/
bool wxDbTable::UpdateWhere(const wxString &pWhereClause)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    wxString sqlStmt;

    // Build the SQL UPDATE statement
    BuildUpdateStmt(sqlStmt, DB_UPD_WHERE, pWhereClause);

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl << endl;
#endif

    // Execute the SQL UPDATE statement
    return(execUpdate(sqlStmt));

}  // wxDbTable::UpdateWhere()


/********** wxDbTable::Delete() **********/
bool wxDbTable::Delete(void)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    wxString sqlStmt;
    sqlStmt.Empty();

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_KEYFIELDS);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::Delete()


/********** wxDbTable::DeleteWhere() **********/
bool wxDbTable::DeleteWhere(const wxString &pWhereClause)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    wxString sqlStmt;
    sqlStmt.Empty();

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_WHERE, pWhereClause);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::DeleteWhere()


/********** wxDbTable::DeleteMatching() **********/
bool wxDbTable::DeleteMatching(void)
{
    wxASSERT(!queryOnly);
    if (queryOnly)
        return(FALSE);

    wxString sqlStmt;
    sqlStmt.Empty();

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_MATCHING);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::DeleteMatching()


/********** wxDbTable::IsColNull() **********/
bool wxDbTable::IsColNull(int colNo)
{
/*
    This logic is just not right.  It would indicate TRUE
    if a numeric field were set to a value of 0.

    switch(colDefs[colNo].SqlCtype)
    {
        case SQL_C_CHAR:
            return(((UCHAR FAR *) colDefs[colNo].PtrDataObj)[0] == 0);
        case SQL_C_SSHORT:
            return((  *((SWORD *) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_USHORT:
            return((   *((UWORD*) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_SLONG:
            return(( *((SDWORD *) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_ULONG:
            return(( *((UDWORD *) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_FLOAT:
            return(( *((SFLOAT *) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_DOUBLE:
            return((*((SDOUBLE *) colDefs[colNo].PtrDataObj))   == 0);
        case SQL_C_TIMESTAMP:
            TIMESTAMP_STRUCT *pDt;
            pDt = (TIMESTAMP_STRUCT *) colDefs[colNo].PtrDataObj;
            if (pDt->year == 0 && pDt->month == 0 && pDt->day == 0)
                return(TRUE);
            else
                return(FALSE);
        default:
            return(TRUE);
    }
*/
    return (colDefs[colNo].Null);
}  // wxDbTable::IsColNull()


/********** wxDbTable::CanSelectForUpdate() **********/
bool wxDbTable::CanSelectForUpdate(void)
{
    if (queryOnly)
        return FALSE;

    if (pDb->Dbms() == dbmsMY_SQL)
        return FALSE;

    if ((pDb->Dbms() == dbmsORACLE) ||
        (pDb->dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE))
        return(TRUE);
    else
        return(FALSE);

}  // wxDbTable::CanSelectForUpdate()


/********** wxDbTable::CanUpdByROWID() **********/
bool wxDbTable::CanUpdByROWID(void)
{
/*
 * NOTE: Returning FALSE for now until this can be debugged,
 *        as the ROWID is not getting updated correctly
 */
    return FALSE;
/*
    if (pDb->Dbms() == dbmsORACLE)
        return(TRUE);
    else
        return(FALSE);
*/
}  // wxDbTable::CanUpdByROWID()


/********** wxDbTable::IsCursorClosedOnCommit() **********/
bool wxDbTable::IsCursorClosedOnCommit(void)
{
    if (pDb->dbInf.cursorCommitBehavior == SQL_CB_PRESERVE)
        return(FALSE);
    else
        return(TRUE);

}  // wxDbTable::IsCursorClosedOnCommit()



/********** wxDbTable::ClearMemberVar() **********/
void wxDbTable::ClearMemberVar(int colNo, bool setToNull)
{
    wxASSERT(colNo < noCols);

    switch(colDefs[colNo].SqlCtype)
    {
        case SQL_C_CHAR:
            ((UCHAR FAR *) colDefs[colNo].PtrDataObj)[0]    = 0;
            break;
        case SQL_C_SSHORT:
            *((SWORD *) colDefs[colNo].PtrDataObj)          = 0;
            break;
        case SQL_C_USHORT:
            *((UWORD*) colDefs[colNo].PtrDataObj)           = 0;
            break;
        case SQL_C_SLONG:
            *((SDWORD *) colDefs[colNo].PtrDataObj)         = 0;
            break;
        case SQL_C_ULONG:
            *((UDWORD *) colDefs[colNo].PtrDataObj)         = 0;
            break;
        case SQL_C_FLOAT:
            *((SFLOAT *) colDefs[colNo].PtrDataObj)         = 0.0f;
            break;
        case SQL_C_DOUBLE:
            *((SDOUBLE *) colDefs[colNo].PtrDataObj)        = 0.0f;
            break;
        case SQL_C_TIMESTAMP:
            TIMESTAMP_STRUCT *pDt;
            pDt = (TIMESTAMP_STRUCT *) colDefs[colNo].PtrDataObj;
            pDt->year = 0;
            pDt->month = 0;
            pDt->day = 0;
            pDt->hour = 0;
            pDt->minute = 0;
            pDt->second = 0;
            pDt->fraction = 0;
            break;
    }

    if (setToNull)
        SetColNull(colNo);
}  // wxDbTable::ClearMemberVar()


/********** wxDbTable::ClearMemberVars() **********/
void wxDbTable::ClearMemberVars(bool setToNull)
{
    int i;

    // Loop through the columns setting each member variable to zero
    for (i=0; i < noCols; i++)
        ClearMemberVar(i,setToNull);

}  // wxDbTable::ClearMemberVars()


/********** wxDbTable::SetQueryTimeout() **********/
bool wxDbTable::SetQueryTimeout(UDWORD nSeconds)
{
    if (SQLSetStmtOption(hstmtInsert, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
    if (SQLSetStmtOption(hstmtUpdate, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
    if (SQLSetStmtOption(hstmtDelete, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));
    if (SQLSetStmtOption(hstmtInternal, SQL_QUERY_TIMEOUT, nSeconds) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtInternal));

    // Completed Successfully
    return(TRUE);

}  // wxDbTable::SetQueryTimeout()


/********** wxDbTable::SetColDefs() **********/
void wxDbTable::SetColDefs(int index, const wxString &fieldName, int dataType, void *pData,
                           SWORD cType, int size, bool keyField, bool upd,
                           bool insAllow, bool derivedCol)
{
    if (!colDefs)  // May happen if the database connection fails
        return;

    if (fieldName.Length() > (unsigned int) DB_MAX_COLUMN_NAME_LEN)
    {
        wxStrncpy (colDefs[index].ColName, fieldName, DB_MAX_COLUMN_NAME_LEN);
        colDefs[index].ColName[DB_MAX_COLUMN_NAME_LEN] = 0;
    }
    else
        wxStrcpy(colDefs[index].ColName, fieldName);

    colDefs[index].DbDataType       = dataType;
    colDefs[index].PtrDataObj       = pData;
    colDefs[index].SqlCtype         = cType;
    colDefs[index].SzDataObj        = size;
    colDefs[index].KeyField         = keyField;
    colDefs[index].DerivedCol       = derivedCol;
    // Derived columns by definition would NOT be "Insertable" or "Updateable"
    if (derivedCol)
    {
        colDefs[index].Updateable       = FALSE;
        colDefs[index].InsertAllowed    = FALSE;
    }
    else
    {
        colDefs[index].Updateable       = upd;
        colDefs[index].InsertAllowed    = insAllow;
    }

    colDefs[index].Null                 = FALSE;
    
}  // wxDbTable::SetColDefs()


/********** wxDbTable::SetColDefs() **********/
wxDbColDataPtr* wxDbTable::SetColDefs(wxDbColInf *pColInfs, ULONG numCols)
{
    wxASSERT(pColInfs);
    wxDbColDataPtr *pColDataPtrs = NULL;

    if (pColInfs)
    {
        ULONG index;
       
        pColDataPtrs = new wxDbColDataPtr[numCols+1];

        for (index = 0; index < numCols; index++)
        {
            // Process the fields
            switch (pColInfs[index].dbDataType)
            {
                case DB_DATA_TYPE_VARCHAR:
                   pColDataPtrs[index].PtrDataObj = new wxChar[pColInfs[index].bufferLength+1];
                   pColDataPtrs[index].SzDataObj  = pColInfs[index].columnSize;
                   pColDataPtrs[index].SqlCtype   = SQL_C_CHAR;
                   break;
                case DB_DATA_TYPE_INTEGER:
                    // Can be long or short
                    if (pColInfs[index].bufferLength == sizeof(long))
                    {
                      pColDataPtrs[index].PtrDataObj = new long;
                      pColDataPtrs[index].SzDataObj  = sizeof(long);
                      pColDataPtrs[index].SqlCtype   = SQL_C_SLONG;
                    }
                    else
                    {
                        pColDataPtrs[index].PtrDataObj = new short;
                        pColDataPtrs[index].SzDataObj  = sizeof(short);
                        pColDataPtrs[index].SqlCtype   = SQL_C_SSHORT;
                    }
                    break;
                case DB_DATA_TYPE_FLOAT:
                    // Can be float or double
                    if (pColInfs[index].bufferLength == sizeof(float))
                    {
                        pColDataPtrs[index].PtrDataObj = new float;
                        pColDataPtrs[index].SzDataObj  = sizeof(float);
                        pColDataPtrs[index].SqlCtype   = SQL_C_FLOAT;
                    }
                    else
                    {
                        pColDataPtrs[index].PtrDataObj = new double;
                        pColDataPtrs[index].SzDataObj  = sizeof(double);
                        pColDataPtrs[index].SqlCtype   = SQL_C_DOUBLE;
                    }  
                    break;
                case DB_DATA_TYPE_DATE:
                    pColDataPtrs[index].PtrDataObj = new TIMESTAMP_STRUCT;
                    pColDataPtrs[index].SzDataObj  = sizeof(TIMESTAMP_STRUCT);
                    pColDataPtrs[index].SqlCtype   = SQL_C_TIMESTAMP;
                    break;
                case DB_DATA_TYPE_BLOB:
						  int notSupportedYet = 0;
                    wxASSERT_MSG(notSupportedYet, wxT("This form of ::SetColDefs() cannot be used with BLOB columns"));
                    pColDataPtrs[index].PtrDataObj = /*BLOB ADDITION NEEDED*/NULL;
                    pColDataPtrs[index].SzDataObj  = /*BLOB ADDITION NEEDED*/sizeof(void *);
                    pColDataPtrs[index].SqlCtype   = SQL_VARBINARY;
                    break;
            }
            if (pColDataPtrs[index].PtrDataObj != NULL)
                SetColDefs (index,pColInfs[index].colName,pColInfs[index].dbDataType, pColDataPtrs[index].PtrDataObj, pColDataPtrs[index].SqlCtype, pColDataPtrs[index].SzDataObj);
            else
            {
                // Unable to build all the column definitions, as either one of 
                // the calls to "new" failed above, or there was a BLOB field
                // to have a column definition for.  If BLOBs are to be used,
                // the other form of ::SetColDefs() must be used, as it is impossible
                // to know the maximum size to create the PtrDataObj to be.
                delete [] pColDataPtrs;
                return NULL;
            }
        }
    }

    return (pColDataPtrs);

} // wxDbTable::SetColDefs()


/********** wxDbTable::SetCursor() **********/
void wxDbTable::SetCursor(HSTMT *hstmtActivate)
{
    if (hstmtActivate == wxDB_DEFAULT_CURSOR)
        hstmt = *hstmtDefault;
    else
        hstmt = *hstmtActivate;

}  // wxDbTable::SetCursor()


/********** wxDbTable::Count(const wxString &) **********/
ULONG wxDbTable::Count(const wxString &args)
{
    ULONG count;
    wxString sqlStmt;
    SDWORD cb;

    // Build a "SELECT COUNT(*) FROM queryTableName [WHERE whereClause]" SQL Statement
    sqlStmt  = wxT("SELECT COUNT(");
    sqlStmt += args;
    sqlStmt += wxT(") FROM ");
    sqlStmt += queryTableName;
#if wxODBC_BACKWARD_COMPATABILITY
    if (from && wxStrlen(from))
#else
    if (from.Length())
#endif
        sqlStmt += from;

    // Add the where clause if one is provided
#if wxODBC_BACKWARD_COMPATABILITY
    if (where && wxStrlen(where))
#else
    if (where.Length())
#endif
    {
        sqlStmt += wxT(" WHERE ");
        sqlStmt += where;
    }

    pDb->WriteSqlLog(sqlStmt);

    // Initialize the Count cursor if it's not already initialized
    if (!hstmtCount)
    {
        hstmtCount = GetNewCursor(FALSE,FALSE);
        wxASSERT(hstmtCount);
        if (!hstmtCount)
            return(0);
    }

    // Execute the SQL statement
    if (SQLExecDirect(*hstmtCount, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, *hstmtCount);
        return(0);
    }

    // Fetch the record
    if (SQLFetch(*hstmtCount) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, *hstmtCount);
        return(0);
    }

    // Obtain the result
    if (SQLGetData(*hstmtCount, (UWORD)1, SQL_C_ULONG, &count, sizeof(count), &cb) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, *hstmtCount);
        return(0);
    }

    // Free the cursor
    if (SQLFreeStmt(*hstmtCount, SQL_CLOSE) != SQL_SUCCESS)
        pDb->DispAllErrors(henv, hdbc, *hstmtCount);

    // Return the record count
    return(count);

}  // wxDbTable::Count()


/********** wxDbTable::Refresh() **********/
bool wxDbTable::Refresh(void)
{
    bool result = TRUE;

    // Switch to the internal cursor so any active cursors are not corrupted
    HSTMT currCursor = GetCursor();
    hstmt = hstmtInternal;
#if wxODBC_BACKWARD_COMPATABILITY
    // Save the where and order by clauses
    char *saveWhere = where;
    char *saveOrderBy = orderBy;
#else
    wxString saveWhere = where;
    wxString saveOrderBy = orderBy;
#endif
    // Build a where clause to refetch the record with.  Try and use the
    // ROWID if it's available, ow use the key fields.
    wxString whereClause;
    whereClause.Empty();

    if (CanUpdByROWID())
    {
        SDWORD cb;
        wxChar   rowid[wxDB_ROWID_LEN+1];

        // Get the ROWID value.  If not successful retreiving the ROWID,
        // simply fall down through the code and build the WHERE clause
        // based on the key fields.
        if (SQLGetData(hstmt, (UWORD)(noCols+1), SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
        {
            whereClause += queryTableName;
            whereClause += wxT(".ROWID = '");
            whereClause += rowid;
            whereClause += wxT("'");
        }
    }

    // If unable to use the ROWID, build a where clause from the keyfields
    if (wxStrlen(whereClause) == 0)
        BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS, queryTableName);

    // Requery the record
    where = whereClause;
    orderBy.Empty();
    if (!Query())
        result = FALSE;

    if (result && !GetNext())
        result = FALSE;

    // Switch back to original cursor
    SetCursor(&currCursor);

    // Free the internal cursor
    if (SQLFreeStmt(hstmtInternal, SQL_CLOSE) != SQL_SUCCESS)
        pDb->DispAllErrors(henv, hdbc, hstmtInternal);

    // Restore the original where and order by clauses
    where   = saveWhere;
    orderBy = saveOrderBy;

    return(result);

}  // wxDbTable::Refresh()


/********** wxDbTable::SetColNull(int colNo, bool set) **********/
bool wxDbTable::SetColNull(int colNo, bool set)
{
    if (colNo < noCols)
    {
        colDefs[colNo].Null = set;
        if (set)  // Blank out the values in the member variable
            ClearMemberVar(colNo,FALSE);  // Must call with FALSE, or infinite recursion will happen
        return(TRUE);
    }
    else
        return(FALSE);

}  // wxDbTable::SetColNull()


/********** wxDbTable::SetColNull(const wxString &colName, bool set) **********/
bool wxDbTable::SetColNull(const wxString &colName, bool set)
{
    int i;
    for (i = 0; i < noCols; i++)
    {
        if (!wxStricmp(colName, colDefs[i].ColName))
            break;
    }

    if (i < noCols)
    {
        colDefs[i].Null = set;
        if (set)  // Blank out the values in the member variable
            ClearMemberVar(i,FALSE);  // Must call with FALSE, or infinite recursion will happen
        return(TRUE);
    }
    else
        return(FALSE);

}  // wxDbTable::SetColNull()


/********** wxDbTable::GetNewCursor() **********/
HSTMT *wxDbTable::GetNewCursor(bool setCursor, bool bindColumns)
{
    HSTMT *newHSTMT = new HSTMT;
    wxASSERT(newHSTMT);
    if (!newHSTMT)
        return(0);

    if (SQLAllocStmt(hdbc, newHSTMT) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc);
        delete newHSTMT;
        return(0);
    }

    if (SQLSetStmtOption(*newHSTMT, SQL_CURSOR_TYPE, cursorType) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc, *newHSTMT);
        delete newHSTMT;
        return(0);
    }

    if (bindColumns)
    {
        if(!bindCols(*newHSTMT))
        {
            delete newHSTMT;
            return(0);
        }
    }

    if (setCursor)
        SetCursor(newHSTMT);

    return(newHSTMT);

}   // wxDbTable::GetNewCursor()


/********** wxDbTable::DeleteCursor() **********/
bool wxDbTable::DeleteCursor(HSTMT *hstmtDel)
{
    bool result = TRUE;

    if (!hstmtDel)  // Cursor already deleted
        return(result);

/*
ODBC 3.0 says to use this form
    if (SQLFreeHandle(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
    
*/
    if (SQLFreeStmt(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc);
        result = FALSE;
    }

    delete hstmtDel;

    return(result);

}  // wxDbTable::DeleteCursor()

#endif  // wxUSE_ODBC

