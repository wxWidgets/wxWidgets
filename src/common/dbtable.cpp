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
#include <assert.h>

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


/********** wxDbTable::wxDbTable() **********/
wxDbTable::wxDbTable(wxDb *pwxDb, const char *tblName, const int nCols,
                    const char *qryTblName, bool qryOnly, const char *tblPath)
{
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
    noCols              = nCols;                    // No. of cols in the table
    where               = "";                       // Where clause
    orderBy             = "";                       // Order By clause
    from                = "";                       // From clause
    selectForUpdate     = FALSE;                    // SELECT ... FOR UPDATE; Indicates whether to include the FOR UPDATE phrase
    queryOnly           = qryOnly;
    insertable          = TRUE;

    assert (tblName);

    wxStrcpy(tableName, tblName);               // Table Name
    if (tblPath)
        wxStrcpy(tablePath, tblPath);           // Table Path - used for dBase files
    
    if (qryTblName)                             // Name of the table/view to query
        wxStrcpy(queryTableName, qryTblName);
    else
        wxStrcpy(queryTableName, tblName);
    
    if (!pDb)
        return;
    
    pDb->incrementTableCount();
    
    wxString s;
    tableID = ++lastTableID;
    s.sprintf("wxDbTable constructor (%-20s) tableID:[%6lu] pDb:[%p]", tblName,tableID,pDb);
    
#ifdef __WXDEBUG__
    wxTablesInUse *tableInUse;
    tableInUse            = new wxTablesInUse();
    tableInUse->tableName = tblName;
    tableInUse->tableID   = tableID;
    tableInUse->pDb       = pDb;
    TablesInUse.Append(tableInUse);
#endif
    
    pDb->WriteSqlLog(s.c_str());
    
    // Grab the HENV and HDBC from the wxDb object
    henv = pDb->GetHENV();
    hdbc = pDb->GetHDBC();
    
    // Allocate space for column definitions
    if (noCols)
        colDefs = new wxDbColDef[noCols];  // Points to the first column defintion
    
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
        if (! wxStrcmp(pDb->sqlState, "01S02"))  // Option Value Changed
        {
            // Datasource does not support static cursors.  Driver
            // will substitute a cursor type.  Call SQLGetStmtOption()
            // to determine which cursor type was selected.
            if (SQLGetStmtOption(hstmtInternal, SQL_CURSOR_TYPE, &cursorType) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc, hstmtInternal);
#ifdef DBDEBUG_CONSOLE
            cout << "Static cursor changed to: ";
            switch(cursorType)
            {
            case SQL_CURSOR_FORWARD_ONLY:
                cout << "Forward Only";
                break;
            case SQL_CURSOR_STATIC:
                cout << "Static";
                break;
            case SQL_CURSOR_KEYSET_DRIVEN:
                cout << "Keyset Driven";
                break;
            case SQL_CURSOR_DYNAMIC:
                cout << "Dynamic";
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
                    return;
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
        cout << "Cursor Type set to STATIC" << endl << endl;
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
    assert(hstmtDefault);
    hstmt = *hstmtDefault;
    
}  // wxDbTable::wxDbTable()


/********** wxDbTable::~wxDbTable() **********/
wxDbTable::~wxDbTable()
{
    wxString s;
    if (pDb)
    {
        s.sprintf("wxDbTable destructor (%-20s) tableID:[%6lu] pDb:[%p]", tableName,tableID,pDb);
        pDb->WriteSqlLog(s.c_str());
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
                    wxLogDebug (s.c_str(),wxT("Unable to delete node!"));
            }
            else
                pNode = pNode->Next();
        }
        if (!found)
        {
            wxString msg;
            msg.sprintf(wxT("Unable to find the tableID in the linked\nlist of tables in use.\n\n%s"),s.c_str());
            wxLogDebug (msg.c_str(),wxT("NOTICE..."));
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
            if (SQLFreeStmt(hstmtInsert, SQL_DROP) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc);

        if (hstmtDelete)
            if (SQLFreeStmt(hstmtDelete, SQL_DROP) != SQL_SUCCESS)

        if (hstmtUpdate)
            if (SQLFreeStmt(hstmtUpdate, SQL_DROP) != SQL_SUCCESS)
                pDb->DispAllErrors(henv, hdbc);
    }

    if (hstmtInternal)
        if (SQLFreeStmt(hstmtInternal, SQL_DROP) != SQL_SUCCESS)
            pDb->DispAllErrors(henv, hdbc);

    // Delete dynamically allocated cursors
    if (hstmtDefault)
        DeleteCursor(hstmtDefault);

    if (hstmtCount)
        DeleteCursor(hstmtCount);

}  // wxDbTable::~wxDbTable()



/***************************** PRIVATE FUNCTIONS *****************************/



/********** wxDbTable::bindInsertParams() **********/
bool wxDbTable::bindInsertParams(void)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    SWORD   fSqlType    = 0;
    UDWORD  precision   = 0;
    SWORD   scale       = 0;

    // Bind each column (that can be inserted) of the table to a parameter marker
    int i,colNo;
    for (i = 0, colNo = 1; i < noCols; i++)
    {
        if (! colDefs[i].InsertAllowed)
            continue;
        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                fSqlType = pDb->GetTypeInfVarchar().FsqlType;      
                precision = colDefs[i].SzDataObj;
                scale = 0;
                colDefs[i].CbValue = SQL_NTS;
                break;
            case DB_DATA_TYPE_INTEGER:
                fSqlType = pDb->GetTypeInfInteger().FsqlType;
                precision = pDb->GetTypeInfInteger().Precision;
                scale = 0;
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
                //  scale = (short) precision;
                colDefs[i].CbValue = 0;
                break;
            case DB_DATA_TYPE_DATE:
                fSqlType = pDb->GetTypeInfDate().FsqlType;
                precision = pDb->GetTypeInfDate().Precision;
                scale = 0;
                colDefs[i].CbValue = 0;
                break;
        }
        // Null values
        if (colDefs[i].Null)
        {
            colDefs[i].CbValue = SQL_NULL_DATA;
            colDefs[i].Null = FALSE;
        }

        if (SQLBindParameter(hstmtInsert, colNo++, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
                             fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
                             precision+1,&colDefs[i].CbValue) != SQL_SUCCESS)
        {
            return(pDb->DispAllErrors(henv, hdbc, hstmtInsert));
        }
    }

    // Completed successfully
    return(TRUE);

}  // wxDbTable::bindInsertParams()


/********** wxDbTable::bindUpdateParams() **********/
bool wxDbTable::bindUpdateParams(void)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);
    
    SWORD   fSqlType    = 0;
    UDWORD  precision   = 0;
    SWORD   scale       = 0;
    
    // Bind each UPDATEABLE column of the table to a parameter marker
    int i,colNo;
    for (i = 0, colNo = 1; i < noCols; i++)
    {
        if (! colDefs[i].Updateable)
            continue;
        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                fSqlType = pDb->GetTypeInfVarchar().FsqlType;
                precision = colDefs[i].SzDataObj;
                scale = 0;
                colDefs[i].CbValue = SQL_NTS;
                break;
            case DB_DATA_TYPE_INTEGER:
                fSqlType = pDb->GetTypeInfInteger().FsqlType;
                precision = pDb->GetTypeInfInteger().Precision;
                scale = 0;
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
                colDefs[i].CbValue = 0;
                break;
            case DB_DATA_TYPE_DATE:
                fSqlType = pDb->GetTypeInfDate().FsqlType;
                precision = pDb->GetTypeInfDate().Precision;
                scale = 0;
                colDefs[i].CbValue = 0;
                break;
        }
        
        if (SQLBindParameter(hstmtUpdate, colNo++, SQL_PARAM_INPUT, colDefs[i].SqlCtype,
                             fSqlType, precision, scale, (UCHAR*) colDefs[i].PtrDataObj, 
                             precision+1, &colDefs[i].CbValue) != SQL_SUCCESS)
        {
            return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));
        }
    }
    
    // Completed successfully
    return(TRUE);

}  // wxDbTable::bindUpdateParams()


/********** wxDbTable::bindCols() **********/
bool wxDbTable::bindCols(HSTMT cursor)
{
    static SDWORD  cb;
    
    // Bind each column of the table to a memory address for fetching data
    int i;
    for (i = 0; i < noCols; i++)
    {
        if (SQLBindCol(cursor, i+1, colDefs[i].SqlCtype, (UCHAR*) colDefs[i].PtrDataObj,
                       colDefs[i].SzDataObj, &cb) != SQL_SUCCESS)     
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
    }

    // Completed successfully
    return(TRUE);

}  // wxDbTable::getRec()


/********** wxDbTable::execDelete() **********/
bool wxDbTable::execDelete(const char *pSqlStmt)
{
    // Execute the DELETE statement
    if (SQLExecDirect(hstmtDelete, (UCHAR FAR *) pSqlStmt, SQL_NTS) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtDelete));

    // Record deleted successfully
    return(TRUE);

}  // wxDbTable::execDelete()


/********** wxDbTable::execUpdate() **********/
bool wxDbTable::execUpdate(const char *pSqlStmt)
{
    // Execute the UPDATE statement
    if (SQLExecDirect(hstmtUpdate, (UCHAR FAR *) pSqlStmt, SQL_NTS) != SQL_SUCCESS)
        return(pDb->DispAllErrors(henv, hdbc, hstmtUpdate));

    // Record deleted successfully
    return(TRUE);

}  // wxDbTable::execUpdate()


/********** wxDbTable::query() **********/
bool wxDbTable::query(int queryType, bool forUpdate, bool distinct, const char *pSqlStmt)
{
    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Set the selectForUpdate member variable
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

    // Make sure the cursor is closed first
    if (! CloseCursor(hstmt))
        return(FALSE);

    // Execute the SQL SELECT statement
    int retcode;     
    retcode = SQLExecDirect(hstmt, (UCHAR FAR *) (queryType == DB_SELECT_STATEMENT ? pSqlStmt : sqlStmt), SQL_NTS);      
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
        return(pDb->DispAllErrors(henv, hdbc, hstmt));

    // Completed successfully
    return(TRUE);

}  // wxDbTable::query()


/***************************** PUBLIC FUNCTIONS *****************************/


/********** wxDbTable::Open() **********/
bool wxDbTable::Open(void)
{
    if (!pDb)
        return FALSE;   

    int i;
    wxString sqlStmt;

    // Verify that the table exists in the database
    if (!pDb->TableExists(tableName,pDb->GetUsername(),tablePath))
    {
        wxString s;
        if (wxStrcmp(tablePath,""))
            s.sprintf(wxT("Error opening '%s/%s'.\n"),tablePath,tableName);
        else
            s.sprintf(wxT("Error opening '%s'.\n"), tableName);
        if (!pDb->TableExists(tableName,NULL,tablePath))
            s += wxT("Table/view does not exist in the database.\n");
        else
            s += wxT("Current logged in user does not have sufficient privileges to access this table.\n");
        pDb->LogError(s.c_str());
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
        sqlStmt.sprintf("INSERT INTO %s (", tableName);
        for (i = 0; i < noCols; i++)
        {
            if (! colDefs[i].InsertAllowed)
                continue;
            if (needComma)
                sqlStmt += ",";
            sqlStmt += colDefs[i].ColName;
            needComma = TRUE;
        }
        needComma = FALSE;
        sqlStmt += ") VALUES (";

        int insertableCount = 0;

        for (i = 0; i < noCols; i++)
        {
            if (! colDefs[i].InsertAllowed)
                continue;
            if (needComma)
                sqlStmt += ",";
            sqlStmt += "?";
            needComma = TRUE;
            insertableCount++;
        }
        sqlStmt += ")";
        
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
bool wxDbTable::QueryBySqlStmt(const char *pSqlStmt)
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


/********** wxDbTable::BuildSelectStmt() **********/
void wxDbTable::BuildSelectStmt(char *pSqlStmt, int typeOfSelect, bool distinct)
{
    char whereClause[DB_MAX_WHERE_CLAUSE_LEN];

    whereClause[0] = 0;

    // Build a select statement to query the database
    wxStrcpy(pSqlStmt, "SELECT ");

    // SELECT DISTINCT values only?
    if (distinct)
        wxStrcat(pSqlStmt, "DISTINCT ");

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
            wxStrcat(pSqlStmt, queryTableName);
            wxStrcat(pSqlStmt, ".");
        }
        wxStrcat(pSqlStmt, colDefs[i].ColName);
        if (i + 1 < noCols)
            wxStrcat(pSqlStmt, ",");
    }

    // If the datasource supports ROWID, get this column as well.  Exception: Don't retrieve
    // the ROWID if querying distinct records.  The rowid will always be unique.
    if (!distinct && CanUpdByROWID())
    {
        // If joining tables, the base table column names must be qualified to avoid ambiguity
        if (appendFromClause)
        {
            wxStrcat(pSqlStmt, ",");
            wxStrcat(pSqlStmt, queryTableName);
            wxStrcat(pSqlStmt, ".ROWID");
        }
        else
            wxStrcat(pSqlStmt, ",ROWID");
    }

    // Append the FROM tablename portion
    wxStrcat(pSqlStmt, " FROM ");
    wxStrcat(pSqlStmt, queryTableName);

    // Sybase uses the HOLDLOCK keyword to lock a record during query.
    // The HOLDLOCK keyword follows the table name in the from clause.
    // Each table in the from clause must specify HOLDLOCK or
    // NOHOLDLOCK (the default).  Note: The "FOR UPDATE" clause
    // is parsed but ignored in SYBASE Transact-SQL.
    if (selectForUpdate && (pDb->Dbms() == dbmsSYBASE_ASA || pDb->Dbms() == dbmsSYBASE_ASE))
        wxStrcat(pSqlStmt, " HOLDLOCK");

    if (appendFromClause)
        wxStrcat(pSqlStmt, from);

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
                wxStrcat(pSqlStmt, " WHERE ");
                wxStrcat(pSqlStmt, where);
            }
            break;
        case DB_SELECT_KEYFIELDS:
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            if (wxStrlen(whereClause))
            {
                wxStrcat(pSqlStmt, " WHERE ");
                wxStrcat(pSqlStmt, whereClause);
            }
            break;
        case DB_SELECT_MATCHING:
            BuildWhereClause(whereClause, DB_WHERE_MATCHING);
            if (wxStrlen(whereClause))
            {
                wxStrcat(pSqlStmt, " WHERE ");
                wxStrcat(pSqlStmt, whereClause);
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
        wxStrcat(pSqlStmt, " ORDER BY ");
        wxStrcat(pSqlStmt, orderBy);
    }

    // SELECT FOR UPDATE if told to do so and the datasource is capable.  Sybase
    // parses the FOR UPDATE clause but ignores it.  See the comment above on the
    // HOLDLOCK for Sybase.
    if (selectForUpdate && CanSelectForUpdate())
        wxStrcat(pSqlStmt, " FOR UPDATE");

}  // wxDbTable::BuildSelectStmt()


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
    cout << "Creating Table " << tableName << "..." << endl;
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
        cout << i + 1 << ": " << colDefs[i].ColName << "; ";
        switch(colDefs[i].DbDataType)
        {
            case DB_DATA_TYPE_VARCHAR:
                cout << pDb->typeInfVarchar.TypeName << "(" << colDefs[i].SzDataObj << ")";
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
        }
        cout << endl;
    }
#endif

    // Build a CREATE TABLE string from the colDefs structure.
    bool needComma = FALSE;
    sqlStmt.sprintf("CREATE TABLE %s (", tableName);

    for (i = 0; i < noCols; i++)
    {
        // Exclude derived columns since they are NOT part of the base table
        if (colDefs[i].DerivedCol)
            continue;
        // Comma Delimiter
        if (needComma)
            sqlStmt += ",";
        // Column Name
        sqlStmt += colDefs[i].ColName;
        sqlStmt += " ";
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
        }
        // For varchars, append the size of the string
        if (colDefs[i].DbDataType == DB_DATA_TYPE_VARCHAR)
        {
            wxString s;
            // wxStrcat(sqlStmt, "(");
            // wxStrcat(sqlStmt, itoa(colDefs[i].SzDataObj, s, 10));
            // wxStrcat(sqlStmt, ")");
            s.sprintf("(%d)", colDefs[i].SzDataObj);
            sqlStmt += s.c_str();
        }

        if (pDb->Dbms() == dbmsSYBASE_ASE || pDb->Dbms() == dbmsMY_SQL)
        {
            if (colDefs[i].KeyField)
            {
                sqlStmt += " NOT NULL";
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
            sqlStmt += ",CONSTRAINT ";
            sqlStmt += tableName;
            sqlStmt += "_PIDX PRIMARY KEY (";
        }
        else
        {
            /* MySQL goes out on this one. We also declare the relevant key NON NULL above */
            sqlStmt += ", PRIMARY KEY (";
        }

        // List column name(s) of column(s) comprising the primary key
        for (i = j = 0; i < noCols; i++)
        {
            if (colDefs[i].KeyField)
            {
                if (j++) // Multi part key, comma separate names
                    sqlStmt += ",";
                sqlStmt += colDefs[i].ColName;
            }
        }
       sqlStmt += ")";
    }
    // Append the closing parentheses for the create table statement
    sqlStmt += ")";

    pDb->WriteSqlLog(sqlStmt.c_str());

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
        rEturn(FALSE);
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

    sqlStmt.sprintf("DROP TABLE %s", tableName);

    pDb->WriteSqlLog(sqlStmt.c_str());

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl;
#endif

    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        // Check for "Base table not found" error and ignore
        pDb->GetNextError(henv, hdbc, hstmt);   
        if (wxStrcmp(pDb->sqlState,"S0002") && wxStrcmp(pDb->sqlState, "S1000"))  // "Base table not found" 
        {    
            // Check for product specific error codes
            if (!((pDb->Dbms() == dbmsSYBASE_ASA  && !wxStrcmp(pDb->sqlState,"42000"))   ||  // 5.x (and lower?)
                (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,"37000"))   ||   
                (pDb->Dbms() == dbmsPOSTGRES      && !wxStrcmp(pDb->sqlState,"08S01"))))     
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
bool wxDbTable::CreateIndex(const char * idxName, bool unique, int noIdxCols, wxDbIdxDef *pIdxDefs, bool attemptDrop)
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
                wxString typeNameAndSize;
                
                switch(colDefs[j].DbDataType)
                {
                    case DB_DATA_TYPE_VARCHAR:
                        typeNameAndSize = pDb->GetTypeInfVarchar().TypeName;
                        break;
                    case DB_DATA_TYPE_INTEGER:
                        typeNameAndSize = pDb->GetTypeInfInteger().TypeName;
                        break;
                    case DB_DATA_TYPE_FLOAT:
                        typeNameAndSize = pDb->GetTypeInfFloat().TypeName;
                        break;
                    case DB_DATA_TYPE_DATE:
                        typeNameAndSize = pDb->GetTypeInfDate().TypeName;
                        break;
                }
                
                // For varchars, append the size of the string
                if (colDefs[j].DbDataType == DB_DATA_TYPE_VARCHAR)
                {
                    wxString s;
                    s.sprintf("(%d)", colDefs[i].SzDataObj);
                    typeNameAndSize += s.c_str();
                }
                
                sqlStmt.sprintf("ALTER TABLE %s MODIFY %s %s NOT NULL",tableName,pIdxDefs[i].ColName,typeNameAndSize.c_str());
                ok = pDb->ExecSql(sqlStmt.c_str());
                
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
    sqlStmt = "CREATE ";
    if (unique)
        sqlStmt += "UNIQUE ";
    
    sqlStmt += "INDEX ";
    sqlStmt += idxName;
    sqlStmt += " ON ";
    sqlStmt += tableName;
    sqlStmt += " (";
    
    // Append list of columns making up index
    int i;
    for (i = 0; i < noIdxCols; i++)
    {
        sqlStmt += pIdxDefs[i].ColName;
        /* Postgres doesn't cope with ASC */
        if (pDb->Dbms() != dbmsPOSTGRES)
        {
            if (pIdxDefs[i].Ascending)
                sqlStmt += " ASC";
            else
                sqlStmt += " DESC";
        }

        if ((i + 1) < noIdxCols)
            sqlStmt += ",";
    }
    
    // Append closing parentheses
    sqlStmt += ")";

    pDb->WriteSqlLog(sqlStmt.c_str());

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
bool wxDbTable::DropIndex(const char * idxName)
{
    // NOTE: This function returns TRUE if the Index does not exist, but
    //       only for identified databases.  Code will need to be added
    //          below for any other databases when those databases are defined
    //       to handle this situation consistently

    wxString sqlStmt;

    if (pDb->Dbms() == dbmsACCESS || pDb->Dbms() == dbmsMY_SQL)
        sqlStmt.sprintf("DROP INDEX %s ON %s",idxName,tableName);
    else if (pDb->Dbms() == dbmsSYBASE_ASE)
        sqlStmt.sprintf("DROP INDEX %s.%s",tableName,idxName);
    else
        sqlStmt.sprintf("DROP INDEX %s",idxName);

    pDb->WriteSqlLog(sqlStmt.c_str());

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt.c_str() << endl;
#endif

    if (SQLExecDirect(hstmt, (UCHAR FAR *) sqlStmt.c_str(), SQL_NTS) != SQL_SUCCESS)
    {
        // Check for "Index not found" error and ignore
        pDb->GetNextError(henv, hdbc, hstmt);
        if (wxStrcmp(pDb->sqlState,"S0012"))  // "Index not found"
        {
            // Check for product specific error codes
            if (!((pDb->Dbms() == dbmsSYBASE_ASA    && !wxStrcmp(pDb->sqlState,"42000")) ||  // v5.x (and lower?)
                  (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,"37000")) ||
                  (pDb->Dbms() == dbmsMS_SQL_SERVER && !wxStrcmp(pDb->sqlState,"S1000")) ||
                  (pDb->Dbms() == dbmsSYBASE_ASE    && !wxStrcmp(pDb->sqlState,"S0002")) ||  // Base table not found
                  (pDb->Dbms() == dbmsMY_SQL        && !wxStrcmp(pDb->sqlState,"42S12")) ||  // tested by Christopher Ludwik Marino-Cebulski using v3.23.21beta
                  (pDb->Dbms() == dbmsPOSTGRES      && !wxStrcmp(pDb->sqlState,"08S01"))
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


/********** wxDbTable::Insert() **********/
int wxDbTable::Insert(void)
{
    assert(!queryOnly);
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
        if (! wxStrcmp(pDb->sqlState, "23000"))  // Integrity constraint violated
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
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Build the SQL UPDATE statement
    BuildUpdateStmt(sqlStmt, DB_UPD_KEYFIELDS);

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt << endl << endl;
#endif

    // Execute the SQL UPDATE statement
    return(execUpdate(sqlStmt));

}  // wxDbTable::Update()


/********** wxDbTable::Update(pSqlStmt) **********/
bool wxDbTable::Update(const char *pSqlStmt)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    pDb->WriteSqlLog(pSqlStmt);

    return(execUpdate(pSqlStmt));

}  // wxDbTable::Update(pSqlStmt)


/********** wxDbTable::UpdateWhere() **********/
bool wxDbTable::UpdateWhere(const char *pWhereClause)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Build the SQL UPDATE statement
    BuildUpdateStmt(sqlStmt, DB_UPD_WHERE, pWhereClause);

    pDb->WriteSqlLog(sqlStmt);

#ifdef DBDEBUG_CONSOLE
    cout << endl << sqlStmt << endl << endl;
#endif

    // Execute the SQL UPDATE statement
    return(execUpdate(sqlStmt));

}  // wxDbTable::UpdateWhere()


/********** wxDbTable::Delete() **********/
bool wxDbTable::Delete(void)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_KEYFIELDS);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::Delete()


/********** wxDbTable::DeleteWhere() **********/
bool wxDbTable::DeleteWhere(const char *pWhereClause)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_WHERE, pWhereClause);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::DeleteWhere()


/********** wxDbTable::DeleteMatching() **********/
bool wxDbTable::DeleteMatching(void)
{
    assert(!queryOnly);
    if (queryOnly)
        return(FALSE);

    char sqlStmt[DB_MAX_STATEMENT_LEN];

    // Build the SQL DELETE statement
    BuildDeleteStmt(sqlStmt, DB_DEL_MATCHING);

    pDb->WriteSqlLog(sqlStmt);

    // Execute the SQL DELETE statement
    return(execDelete(sqlStmt));

}  // wxDbTable::DeleteMatching()


/********** wxDbTable::BuildUpdateStmt() **********/
void wxDbTable::BuildUpdateStmt(char *pSqlStmt, int typeOfUpd, const char *pWhereClause)
{
    assert(!queryOnly);
    if (queryOnly)
        return;

    char whereClause[DB_MAX_WHERE_CLAUSE_LEN];
    bool firstColumn = TRUE;

    whereClause[0] = 0;
    sprintf(pSqlStmt, "UPDATE %s SET ", tableName);

    // Append a list of columns to be updated
    int i;
    for (i = 0; i < noCols; i++)
    {
        // Only append Updateable columns
        if (colDefs[i].Updateable)
        {
            if (! firstColumn)
                wxStrcat(pSqlStmt, ",");
            else
                firstColumn = FALSE;
            wxStrcat(pSqlStmt, colDefs[i].ColName);
            wxStrcat(pSqlStmt, " = ?");
        }
    }

    // Append the WHERE clause to the SQL UPDATE statement
    wxStrcat(pSqlStmt, " WHERE ");
    switch(typeOfUpd)
    {
        case DB_UPD_KEYFIELDS:
            // If the datasource supports the ROWID column, build
            // the where on ROWID for efficiency purposes.
            // e.g. UPDATE PARTS SET Col1 = ?, Col2 = ? WHERE ROWID = '111.222.333'
            if (CanUpdByROWID())
            {
                SDWORD cb;
                char   rowid[wxDB_ROWID_LEN];

                // Get the ROWID value.  If not successful retreiving the ROWID,
                // simply fall down through the code and build the WHERE clause
                // based on the key fields.
                if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
                {
                    wxStrcat(pSqlStmt, "ROWID = '");
                    wxStrcat(pSqlStmt, rowid);
                    wxStrcat(pSqlStmt, "'");
                    break;
                }
            }
            // Unable to delete by ROWID, so build a WHERE
            // clause based on the keyfields.
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            wxStrcat(pSqlStmt, whereClause);
            break;
        case DB_UPD_WHERE:
            wxStrcat(pSqlStmt, pWhereClause);
            break;
    }
}  // BuildUpdateStmt()


/********** wxDbTable::BuildDeleteStmt() **********/
void wxDbTable::BuildDeleteStmt(char *pSqlStmt, int typeOfDel, const char *pWhereClause)
{
    assert(!queryOnly);
    if (queryOnly)
        return;

    char whereClause[DB_MAX_WHERE_CLAUSE_LEN];

    whereClause[0] = 0;

    // Handle the case of DeleteWhere() and the where clause is blank.  It should
    // delete all records from the database in this case.
    if (typeOfDel == DB_DEL_WHERE && (pWhereClause == 0 || wxStrlen(pWhereClause) == 0))
    {
        sprintf(pSqlStmt, "DELETE FROM %s", tableName);
        return;
    }

    sprintf(pSqlStmt, "DELETE FROM %s WHERE ", tableName);

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
                char   rowid[wxDB_ROWID_LEN];

                // Get the ROWID value.  If not successful retreiving the ROWID,
                // simply fall down through the code and build the WHERE clause
                // based on the key fields.
                if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
                {
                    wxStrcat(pSqlStmt, "ROWID = '");
                    wxStrcat(pSqlStmt, rowid);
                    wxStrcat(pSqlStmt, "'");
                    break;
                }
            }
            // Unable to delete by ROWID, so build a WHERE
            // clause based on the keyfields.
            BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS);
            wxStrcat(pSqlStmt, whereClause);
            break;
        case DB_DEL_WHERE:
            wxStrcat(pSqlStmt, pWhereClause);
            break;
        case DB_DEL_MATCHING:
            BuildWhereClause(whereClause, DB_WHERE_MATCHING);
            wxStrcat(pSqlStmt, whereClause);
            break;
    }

}  // BuildDeleteStmt()


/********** wxDbTable::BuildWhereClause() **********/
void wxDbTable::BuildWhereClause(char *pWhereClause, int typeOfWhere,
                                 const char *qualTableName, bool useLikeComparison)
/*
 * Note: BuildWhereClause() currently ignores timestamp columns.
 *       They are not included as part of the where clause.
 */
{
    bool moreThanOneColumn = FALSE;
    char colValue[255];

    // Loop through the columns building a where clause as you go
    int i;
    for (i = 0; i < noCols; i++)
    {
        // Determine if this column should be included in the WHERE clause
        if ((typeOfWhere == DB_WHERE_KEYFIELDS && colDefs[i].KeyField) ||
             (typeOfWhere == DB_WHERE_MATCHING  && (! IsColNull(i))))
        {
            // Skip over timestamp columns
            if (colDefs[i].SqlCtype == SQL_C_TIMESTAMP)
                continue;
            // If there is more than 1 column, join them with the keyword "AND"
            if (moreThanOneColumn)
                wxStrcat(pWhereClause, " AND ");
            else
                moreThanOneColumn = TRUE;
            // Concatenate where phrase for the column
            if (qualTableName && wxStrlen(qualTableName))
            {
                wxStrcat(pWhereClause, qualTableName);
                wxStrcat(pWhereClause, ".");
            }
            wxStrcat(pWhereClause, colDefs[i].ColName);
            if (useLikeComparison && (colDefs[i].SqlCtype == SQL_C_CHAR))
                wxStrcat(pWhereClause, " LIKE ");
            else
                wxStrcat(pWhereClause, " = ");
            switch(colDefs[i].SqlCtype)
            {
                case SQL_C_CHAR:
                    sprintf(colValue, "'%s'", (UCHAR FAR *) colDefs[i].PtrDataObj);
                    break;
                case SQL_C_SSHORT:
                    sprintf(colValue, "%hi", *((SWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_USHORT:
                    sprintf(colValue, "%hu", *((UWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_SLONG:
                    sprintf(colValue, "%li", *((SDWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_ULONG:
                    sprintf(colValue, "%lu", *((UDWORD *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_FLOAT:
                    sprintf(colValue, "%.6f", *((SFLOAT *) colDefs[i].PtrDataObj));
                    break;
                case SQL_C_DOUBLE:
                    sprintf(colValue, "%.6f", *((SDOUBLE *) colDefs[i].PtrDataObj));
                    break;
            }
            wxStrcat(pWhereClause, colValue);
        }
    }
}  // wxDbTable::BuildWhereClause()


/********** wxDbTable::IsColNull() **********/
bool wxDbTable::IsColNull(int colNo)
{
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
}  // wxDbTable::IsColNull()


/********** wxDbTable::CanSelectForUpdate() **********/
bool wxDbTable::CanSelectForUpdate(void)
{
    if (pDb->Dbms() == dbmsMY_SQL)
        return FALSE;

    if (pDb->dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE)
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

    if (pDb->Dbms() == dbmsORACLE)
        return(TRUE);
    else
        return(FALSE);

}  // wxDbTable::CanUpdByROWID()


/********** wxDbTable::IsCursorClosedOnCommit() **********/
bool wxDbTable::IsCursorClosedOnCommit(void)
{
    if (pDb->dbInf.cursorCommitBehavior == SQL_CB_PRESERVE)
        return(FALSE);
    else
        return(TRUE);

}  // wxDbTable::IsCursorClosedOnCommit()


/********** wxDbTable::ClearMemberVars() **********/
void wxDbTable::ClearMemberVars(void)
{
    // Loop through the columns setting each member variable to zero
    int i;
    for (i = 0; i < noCols; i++)
    {
        switch(colDefs[i].SqlCtype)
        {
            case SQL_C_CHAR:
                ((UCHAR FAR *) colDefs[i].PtrDataObj)[0]    = 0;
                break;
            case SQL_C_SSHORT:
                *((SWORD *) colDefs[i].PtrDataObj)          = 0;
                break;
            case SQL_C_USHORT:
                *((UWORD*) colDefs[i].PtrDataObj)           = 0;
                break;
            case SQL_C_SLONG:
                *((SDWORD *) colDefs[i].PtrDataObj)         = 0;
                break;
            case SQL_C_ULONG:
                *((UDWORD *) colDefs[i].PtrDataObj)         = 0;
                break;
            case SQL_C_FLOAT:
                *((SFLOAT *) colDefs[i].PtrDataObj)         = 0.0f;
                break;
            case SQL_C_DOUBLE:
                *((SDOUBLE *) colDefs[i].PtrDataObj)        = 0.0f;
                break;
            case SQL_C_TIMESTAMP:
                TIMESTAMP_STRUCT *pDt;
                pDt = (TIMESTAMP_STRUCT *) colDefs[i].PtrDataObj;
                pDt->year = 0;
                pDt->month = 0;
                pDt->day = 0;
                pDt->hour = 0;
                pDt->minute = 0;
                pDt->second = 0;
                pDt->fraction = 0;
                break;
        }
    }

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
void wxDbTable::SetColDefs (int index, const char *fieldName, int dataType, void *pData,
                            int cType, int size, bool keyField, bool upd,
                            bool insAllow, bool derivedCol)
{
    if (!colDefs)  // May happen if the database connection fails
        return;

    if (wxStrlen(fieldName) > (unsigned int) DB_MAX_COLUMN_NAME_LEN)
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


/********** wxDbTable::SetColDef() **********/
wxDbColDataPtr* wxDbTable::SetColDefs (wxDbColInf *pColInfs, ULONG numCols)
{
    assert(pColInfs);
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
                   pColDataPtrs[index].PtrDataObj = new char[pColInfs[index].bufferLength+1];
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
            }
            SetColDefs (index,pColInfs[index].colName,pColInfs[index].dbDataType, pColDataPtrs[index].PtrDataObj, pColDataPtrs[index].SqlCtype, pColDataPtrs[index].SzDataObj);
        }
    }

    return (pColDataPtrs);

} // wxDbTable::SetColDef()


/********** wxDbTable::SetCursor() **********/
void wxDbTable::SetCursor(HSTMT *hstmtActivate)
{
    if (hstmtActivate == wxDB_DEFAULT_CURSOR)
        hstmt = *hstmtDefault;
    else
        hstmt = *hstmtActivate;

}  // wxDbTable::SetCursor()


/********** wxDbTable::Count(const char *) **********/
ULONG wxDbTable::Count(const char *args)
{
    ULONG count;
    wxString sqlStmt;
    SDWORD cb;

    // Build a "SELECT COUNT(*) FROM queryTableName [WHERE whereClause]" SQL Statement
    sqlStmt  = "SELECT COUNT(";
    sqlStmt += args;
    sqlStmt += ") FROM ";
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
        sqlStmt += " WHERE ";
        sqlStmt += where;
    }

    pDb->WriteSqlLog(sqlStmt.c_str());

    // Initialize the Count cursor if it's not already initialized
    if (!hstmtCount)
    {
        hstmtCount = GetNewCursor(FALSE,FALSE);
        assert(hstmtCount);
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
    if (SQLGetData(*hstmtCount, 1, SQL_C_ULONG, &count, sizeof(count), &cb) != SQL_SUCCESS)
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
    char whereClause[DB_MAX_WHERE_CLAUSE_LEN+1];
    wxStrcpy(whereClause, "");
    if (CanUpdByROWID())
    {
        SDWORD cb;
        char   rowid[wxDB_ROWID_LEN+1];

        // Get the ROWID value.  If not successful retreiving the ROWID,
        // simply fall down through the code and build the WHERE clause
        // based on the key fields.
        if (SQLGetData(hstmt, noCols+1, SQL_C_CHAR, (UCHAR*) rowid, wxDB_ROWID_LEN, &cb) == SQL_SUCCESS)
        {
            wxStrcat(whereClause, queryTableName);
            wxStrcat(whereClause, ".ROWID = '");
            wxStrcat(whereClause, rowid);
            wxStrcat(whereClause, "'");
        }
    }

    // If unable to use the ROWID, build a where clause from the keyfields
    if (wxStrlen(whereClause) == 0)
        BuildWhereClause(whereClause, DB_WHERE_KEYFIELDS, queryTableName);

    // Requery the record
    where = whereClause;
    orderBy = "";
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


/********** wxDbTable::SetNull(int colNo) **********/
bool wxDbTable::SetNull(int colNo)
{
    if (colNo < noCols)
        return(colDefs[colNo].Null = TRUE);
    else
        return(FALSE);

}  // wxDbTable::SetNull(int colNo)


/********** wxDbTable::SetNull(char *colName) **********/
bool wxDbTable::SetNull(const char *colName)
{
    int i;
    for (i = 0; i < noCols; i++)
    {
        if (!wxStricmp(colName, colDefs[i].ColName))
            break;
    }

    if (i < noCols)
        return(colDefs[i].Null = TRUE);
    else
        return(FALSE);

}  // wxDbTable::SetNull(char *colName)


/********** wxDbTable::GetNewCursor() **********/
HSTMT *wxDbTable::GetNewCursor(bool setCursor, bool bindColumns)
{
    HSTMT *newHSTMT = new HSTMT;
    assert(newHSTMT);
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

    if (SQLFreeStmt(*hstmtDel, SQL_DROP) != SQL_SUCCESS)
    {
        pDb->DispAllErrors(henv, hdbc);
        result = FALSE;
    }

    delete hstmtDel;

    return(result);

}  // wxDbTable::DeleteCursor()

#endif  // wxUSE_ODBC

