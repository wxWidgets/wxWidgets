///////////////////////////////////////////////////////////////////////////////
// Name:        listdb.h
// Purpose:     wxWindows database demo app
// Author:      George Tasker
// Modified by:
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 Remstar International, Inc.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "listdb.h"
#endif

/*
    Contains dialog class for creating a data table lookup listbox
*/

#ifndef LISTDB_DOT_H
#define LISTDB_DOT_H


#include <wx/dbtable.h>

const int LOOKUP_COL_LEN = 250;

// Global database connection
extern wxDB *READONLY_DB;

// Clookup class
class Clookup : public wxTable
{
    public:

        char lookupCol[LOOKUP_COL_LEN+1];

        Clookup(char *tblName, char *colName);

};  // Clookup

// Clookup2 class
class Clookup2 : public wxTable
{
    public:

        char lookupCol1[LOOKUP_COL_LEN+1];
        char lookupCol2[LOOKUP_COL_LEN+1];

        Clookup2(char *tblName, char *colName1, char *colName2, wxDB *pDb);

};  // Clookup2

class ClookUpDlg : public wxDialog
{
    private:
        bool             widgetPtrsSet;
        int              currentCursor;
        Clookup         *lookup;
        Clookup2        *lookup2;
        int              noDisplayCols;
        int              col1Len;

        wxListBox       *pLookUpSelectList;
        wxButton        *pLookUpOkBtn;
        wxButton        *pLookUpCancelBtn;

    public:

        // This is a generic lookup constructor that will work with any table and any column
        ClookUpDlg(wxWindow *parent,
                      char  *windowTitle,
                      char  *tableName,
                      char  *colName,
                      char  *where,
                      char  *orderBy);

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
        // If you do query by sql statement, you must pass in the maximum length of column1,
        // since it cannot be derived when you query using your own sql statement.
        //
        // The optional database connection can be used if you'd like the lookup class
        // to use a database pointer other than the global READONLY_DB.  This is necessary if
        // records are being saved, but not committed to the db, yet should be included
        // in the lookup window.
        //
        ClookUpDlg(wxWindow *parent,
                      char  *windowTitle,
                      char  *tableName,
                      char  *dispCol1,                  // Must have at least 1 display column
                      char  *dispCol2,                  // Optional
                      char  *where,
                      char  *orderBy,
                      bool   distinctValues,            // e.g. SELECT DISTINCT ...
                      char  *selectStmt = 0,            // If you wish to query by SQLstmt (complicated lookups)
                      int    maxLenCol1 = 0,            // Mandatory if querying by SQLstmt
                      wxDB  *pDb        = READONLY_DB,  // Database connection pointer
                      bool   allowOk    = TRUE);        // is the OK button enabled

        void        OnButton( wxCommandEvent &event );
        void        OnCommand(wxWindow& win, wxCommandEvent& event);
        void        OnClose(wxCloseEvent& event);
        void        OnActivate(bool) {};  // necessary for hot keys

DECLARE_EVENT_TABLE()
};

#define LOOKUP_DIALOG                   500

#define LOOKUP_DIALOG_SELECT            501
#define LOOKUP_DIALOG_OK                502
#define LOOKUP_DIALOG_CANCEL            503

#endif  // LISTDB_DOT_H

// ************************************ listdb.h *********************************
