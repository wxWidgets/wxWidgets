///////////////////////////////////////////////////////////////////////////////
// Name:        listdb.cpp
// Purpose:     Data table lookup listbox code
// Author:      George Tasker/Doug Card
// Modified by:
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
// SYNOPSIS START

    Member functions for the classes defined in LISTDB.H

     This class is used to present a generic ListBox lookup window for
     use with any of the object creation/selection choice widgets.   This 
     dialog window will present a (possibly) scrolling list of values
     that come from a data table source.  Based on the object type passed
     in the constructor, a ListBox is built to present the user with a
     single selection listbox.

     The string selected from the list box is stored in the Global variable
     "ListDB_Seclection", and will remain set until another interation of this
     routine is called.

     For each object (database) type that is to be used, an overridden
     constructor should be written to appropriately link to the proper
     data table/object for building the list.
       
    The data table record access is all handled through the routines
    in this module, interfacing with the methods defined in wxTable.

     All objects which use data table access must be initialized and
     have opened the table prior to passing them in the dialog
     constructor, and the 'where' query should already have been set
     and performed before creating this dialog instance.
    
// SYNOPSIS STOP
*/

#ifdef __GNUG__
#pragma implementation "listdb.h"
#endif

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
#pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
#include  <wx/wx.h>
#endif //WX_PRECOMP

#include <wx/dbtable.h>

extern wxDbList WXDLLEXPORT *PtrBegDbList;    /* from db.cpp, used in getting back error results from db connections */

#include "listdb.h"

// Global structure for holding ODBC connection information
extern wxDbConnectInf DbConnectInf;

// Global database connection
extern wxDB *READONLY_DB;


// Used for passing the selected listbox selection back to the calling
// routine.  This variable must be declared as 'extern' in the calling
// source module
char ListDB_Selection[LOOKUP_COL_LEN+1];

// If the listbox contains two columns of data, the second column is
// returned in this variable.
char ListDB_Selection2[LOOKUP_COL_LEN+1];

// Constants
const int LISTDB_NO_SPACES_BETWEEN_COLS = 3;



/*
 * This function will return the exact string(s) from the database engine
 * indicating all error conditions which have just occured during the
 * last call to the database engine.
 *
 * This demo uses the returned string by displaying it in a wxMessageBox.  The
 * formatting therefore is not the greatest, but this is just a demo, not a
 * finished product.  :-) gt
 *
 * NOTE: The value returned by this function is for temporary use only and
 *       should be copied for long term use
 */
char *GetExtendedDBErrorMsg2(char *ErrFile, int ErrLine)
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
        msg += "\n";
    }

    msg.Append ("\nODBC errors:\n");
    msg += "\n";
    
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



// Clookup constructor
Clookup::Clookup(char *tblName, char *colName) : wxTable(READONLY_DB, tblName, 1, NULL, !QUERY_ONLY, DbConnectInf.defaultDir)
{

    SetColDefs (0, colName, DB_DATA_TYPE_VARCHAR, lookupCol, SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

}  // Clookup()


// Clookup2 constructor
Clookup2::Clookup2(char *tblName, char *colName1, char *colName2, wxDB *pDb)
   : wxTable(pDb, tblName, (1 + (strlen(colName2) > 0)), NULL, !QUERY_ONLY, DbConnectInf.defaultDir)
{
    int i = 0;

    SetColDefs (i, colName1, DB_DATA_TYPE_VARCHAR, lookupCol1, SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

    if (strlen(colName2) > 0)
        SetColDefs (++i, colName2, DB_DATA_TYPE_VARCHAR, lookupCol2, SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

}  // Clookup2()


BEGIN_EVENT_TABLE(ClookUpDlg, wxDialog)
//     EVT_LISTBOX(LOOKUP_DIALOG_SELECT, ClookUpDlg::SelectCallback)
    EVT_BUTTON(LOOKUP_DIALOG_OK,  ClookUpDlg::OnButton)
    EVT_BUTTON(LOOKUP_DIALOG_CANCEL,  ClookUpDlg::OnButton)
    EVT_CLOSE(ClookUpDlg::OnClose)
END_EVENT_TABLE()

// This is a generic lookup constructor that will work with any table and any column
ClookUpDlg::ClookUpDlg(wxWindow *parent, char *windowTitle, char *tableName, char *colName,
    char *where, char *orderBy)  : wxDialog (parent, LOOKUP_DIALOG, "Select...", wxPoint(-1, -1), wxSize(400, 290))
{
    wxBeginBusyCursor();
    
    strcpy(ListDB_Selection,"");
    widgetPtrsSet = FALSE;
    lookup  = 0;
    lookup2 = 0;
    noDisplayCols = 1;
    col1Len = 0;

    pLookUpSelectList       = new wxListBox(this, LOOKUP_DIALOG_SELECT,             wxPoint(  5,  15), wxSize(384, 195), 0, 0, wxLB_SINGLE|wxLB_ALWAYS_SB, wxDefaultValidator, "LookUpSelectList");
    pLookUpOkBtn            = new wxButton(this, LOOKUP_DIALOG_OK,      "&Ok",      wxPoint(113, 222), wxSize( 70,  35), 0, wxDefaultValidator, "LookUpOkBtn");
    pLookUpCancelBtn        = new wxButton(this, LOOKUP_DIALOG_CANCEL,  "C&ancel",  wxPoint(212, 222), wxSize( 70,  35), 0, wxDefaultValidator, "LookUpCancelBtn");

    widgetPtrsSet = TRUE;

    // Query the lookup table and display the result set
    if (!(lookup = new Clookup(tableName, colName)))
    {
        wxMessageBox("Error allocating memory for 'Clookup'object.","Error...");
        Close();
        return;
    }

    if (!lookup->Open())
    {
        wxString tStr;
        tStr.Printf("Unable to open the table '%s'.",tableName);
        wxMessageBox(tStr,"ODBC Error...");
        Close();
        return;
    }

    lookup->orderBy = orderBy;
    lookup->where = where;
    if (!lookup->Query())
    {
        wxMessageBox("ODBC error during Query()","ODBC Error...");
        Close();
        return;
    }

    // Fill in the list box from the query result set
    while (lookup->GetNext())
        pLookUpSelectList->Append(lookup->lookupCol);

    // Highlight the first list item
    pLookUpSelectList->SetSelection(0);

    // Make the OK activate by pressing Enter
    if (pLookUpSelectList->Number())
        pLookUpOkBtn->SetDefault();
    else
    {
        pLookUpCancelBtn->SetDefault();
        pLookUpOkBtn->Enable(FALSE);
    }

    // Display the dialog window
    SetTitle(windowTitle);
    Centre(wxBOTH);
    wxEndBusyCursor();
    ShowModal();

}  // Generic lookup constructor


//
// This is a generic lookup constructor that will work with any table and any column.
// It extends the capabilites of the lookup dialog in the following ways:
//
//    1) 2 columns rather than one
// 2) The ability to select DISTINCT column values
//
// Only set distinctValues equal to true if necessary.  In many cases, the constraints
// of the index(es) will enforce this uniqueness.  Selecting DISTINCT does require
// overhead by the database to ensure that all values returned are distinct.  Therefore,
// use this ONLY when you need it.
//
// For complicated queries, you can pass in the sql select statement.  This would be
// necessary if joins are involved since by default both columns must come from the
// same table.
//
// If you do query by sql statement, you must pass in the maximum lenght of column1,
// since it cannot be derived when you query using your own sql statement.
//
// The optional database connection can be used if you'd like the lookup class
// to use a database pointer other than the global READONLY_DB.  This is necessary if
// records are being saved, but not committed to the db, yet should be included
// in the lookup window.
//
ClookUpDlg::ClookUpDlg(wxWindow *parent, char *windowTitle, char *tableName,
    char *dispCol1, char *dispCol2, char *where, char *orderBy, bool distinctValues,
    char *selectStmt, int maxLenCol1, wxDB *pDb, bool allowOk)  : wxDialog (parent, LOOKUP_DIALOG, "Select...", wxPoint(-1, -1), wxSize(400, 290))
{
    wxBeginBusyCursor();
    
    strcpy(ListDB_Selection,"");
    strcpy(ListDB_Selection2,"");
    widgetPtrsSet = FALSE;
    lookup  = 0;
    lookup2 = 0;
    noDisplayCols = (strlen(dispCol2) ? 2 : 1);
    col1Len = 0;

    wxFont fixedFont(12,wxMODERN,wxNORMAL,wxNORMAL);

    // this is done with fixed font so that the second column (if any) will be left
    // justified in the second column
    pLookUpSelectList        = new wxListBox(this, LOOKUP_DIALOG_SELECT, wxPoint(5, 15), wxSize(384, 195), 0, 0, wxLB_SINGLE|wxLB_ALWAYS_SB, wxDefaultValidator, "LookUpSelectList");

    pLookUpSelectList->SetFont(fixedFont);

    pLookUpOkBtn            = new wxButton(this, LOOKUP_DIALOG_OK,      "&Ok",        wxPoint(113, 222), wxSize(70, 35), 0, wxDefaultValidator, "LookUpOkBtn");
    pLookUpCancelBtn        = new wxButton(this, LOOKUP_DIALOG_CANCEL,  "C&ancel",    wxPoint(212, 222), wxSize(70, 35), 0, wxDefaultValidator, "LookUpCancelBtn");

    widgetPtrsSet = TRUE;

    // Query the lookup table and display the result set
    if (!(lookup2 = new Clookup2(tableName, dispCol1, dispCol2, pDb)))
    {
        wxMessageBox("Error allocating memory for 'Clookup2'object.","Error...");
        Close();
        return;
    }

    if (!lookup2->Open())
    {
        wxString tStr;
        tStr.Printf("Unable to open the table '%s'.",tableName);
        tStr += GetExtendedDBErrorMsg2(__FILE__,__LINE__);
        wxMessageBox(tStr,"ODBC Error...");
        Close();
        return;
    }

    // If displaying 2 columns, determine the maximum length of column1
    int maxColLen;
    if (maxLenCol1)
        maxColLen = col1Len = maxLenCol1;  // user passed in max col length for column 1
    else
    {
        maxColLen = LOOKUP_COL_LEN;
        if (strlen(dispCol2))
        {
            wxString q = "SELECT MAX({fn LENGTH(";
            q += dispCol1;
            q += ")}), NULL";
            q += " FROM ";
            q += tableName;
            if (strlen(where))
            {
                q += " WHERE ";
                q += where;
            }
            if (!lookup2->QueryBySqlStmt((char*) (const char*) q))
            {
                wxMessageBox("ODBC error during QueryBySqlStmt()","ODBC Error...");
                Close();
                return;
            }
            if (lookup2->GetNext())
                maxColLen = col1Len = atoi(lookup2->lookupCol1);
            else
                wxMessageBox("ODBC error during GetNext()","ODBC Error...");
        }
    }

    // Query the actual record set
    if (selectStmt && strlen(selectStmt))    // Query by sql stmt passed in
    {
        if (!lookup2->QueryBySqlStmt(selectStmt))
        {
            wxMessageBox("ODBC error during QueryBySqlStmt()","ODBC Error...");
            Close();
            return;
        }
    }
    else    // Query using where and order by clauses
    {
        lookup2->orderBy = orderBy;
        lookup2->where = where;
        if (!lookup2->Query(FALSE, distinctValues))
        {
            wxMessageBox("ODBC error during Query()","ODBC Error...");
            Close();
            return;
        }
    }

    // Fill in the list box from the query result set
    wxString s;
    while (lookup2->GetNext())
    {
        s = lookup2->lookupCol1;
        if (strlen(dispCol2))        // Append the optional column 2
        {
            s.Append(' ', (maxColLen + LISTDB_NO_SPACES_BETWEEN_COLS - strlen(lookup2->lookupCol1)));
            s.Append(lookup2->lookupCol2);
        }
        pLookUpSelectList->Append(s);
    }

    // Highlight the first list item
    pLookUpSelectList->SetSelection(0);

    // Make the OK activate by pressing Enter
    if (pLookUpSelectList->Number())
        pLookUpOkBtn->SetDefault();
    else
    {
        pLookUpCancelBtn->SetDefault();
        pLookUpOkBtn->Enable(FALSE);
    }

    pLookUpOkBtn->Enable(allowOk);

    // Display the dialog window
    SetTitle(windowTitle);
    Centre(wxBOTH);
    wxEndBusyCursor();
    ShowModal();

}  // Generic lookup constructor 2


void ClookUpDlg::OnClose(wxCloseEvent& event)
{
    widgetPtrsSet = FALSE;
    GetParent()->Enable(TRUE);

    if (lookup)
        delete lookup;
    if (lookup2)
        delete lookup2;

    while (wxIsBusy()) wxEndBusyCursor();
   event.Skip();

//    return TRUE;

}  // ClookUpDlg::OnClose


void ClookUpDlg::OnButton( wxCommandEvent &event )
{
  wxWindow *win = (wxWindow*) event.GetEventObject();
  OnCommand( *win, event );
}


void ClookUpDlg::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    wxString widgetName = win.GetName();

    if (widgetPtrsSet)
    {
        // OK Button
        if (widgetName == pLookUpOkBtn->GetName())
        {
            if (pLookUpSelectList->GetSelection() != -1)
            {
                if (noDisplayCols == 1)
                    strcpy (ListDB_Selection, pLookUpSelectList->GetStringSelection());
                else  // 2 display columns
                {
                    wxString s = pLookUpSelectList->GetStringSelection();
                    // Column 1
                    s = s.SubString(0, col1Len-1);
                    s = s.Strip();
                    strcpy(ListDB_Selection, s);
                    // Column 2
                    s = pLookUpSelectList->GetStringSelection();
                    s = s.Mid(col1Len + LISTDB_NO_SPACES_BETWEEN_COLS);
                    s = s.Strip();
                    strcpy(ListDB_Selection2, s);
                }
            }
            else
            {
                strcpy(ListDB_Selection,"");
                strcpy(ListDB_Selection2,"");
            }
            Close();
        }  // OK Button

        // Cancel Button
        if (widgetName == pLookUpCancelBtn->GetName())
        {
            strcpy (ListDB_Selection,"");
            strcpy (ListDB_Selection2,"");
            Close();
        }  // Cancel Button
    }

};  // ClookUpDlg::OnCommand

// *********************************** listdb.cpp **********************************
