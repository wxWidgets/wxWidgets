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

#include "listdb.h"

// Global structure for holding ODBC connection information
extern DbStuff DbConnectInf;

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
const LISTDB_NO_SPACES_BETWEEN_COLS = 3;


// Clookup constructor
Clookup::Clookup(char *tblName, char *colName) : wxTable(READONLY_DB, tblName, 1)
{

	SetColDefs (0, colName,	DB_DATA_TYPE_VARCHAR, lookupCol,	SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

}  // Clookup()


// Clookup2 constructor
Clookup2::Clookup2(char *tblName, char *colName1, char *colName2, wxDB *pDb)
   : wxTable(pDb, tblName, (1 + (strlen(colName2) > 0)))
{
	int i = 0;

	SetColDefs (i, colName1, DB_DATA_TYPE_VARCHAR, lookupCol1,	SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

	if (strlen(colName2) > 0)
		SetColDefs (++i, colName2, DB_DATA_TYPE_VARCHAR, lookupCol2,	SQL_C_CHAR, LOOKUP_COL_LEN+1, FALSE, FALSE);

}  // Clookup2()


// This is a generic lookup constructor that will work with any table and any column
ClookUpDlg::ClookUpDlg(wxWindow *parent, char *windowTitle, char *tableName, char *colName,
	char *where, char *orderBy)  : wxDialogBox (parent, "Select...", 1, -1, -1, 400, 290)
{
	wxBeginBusyCursor();
	
	strcpy(ListDB_Selection,"");
	widgetPtrsSet = FALSE;
	lookup  = 0;
	lookup2 = 0;
	noDisplayCols = 1;
	col1Len = 0;

	// Build the dialog
	SetLabelPosition(wxVERTICAL);

	wxFont *ButtonFont = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD);
	wxFont *TextFont   = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);

	SetButtonFont(ButtonFont);
	SetLabelFont(TextFont);
	SetLabelPosition(wxVERTICAL);

	pLookUpSelectList		= new wxListBox(this, NULL, "", wxSINGLE|wxALWAYS_SB, 5, 15, 384, 195, 0, 0, 0, "LookUpSelectList");
	pLookUpOkBtn			= new wxButton(this, NULL, "&Ok",		113, 222, 70, 35, 0, "LookUpOkBtn");
	pLookUpCancelBtn		= new wxButton(this, NULL, "C&ancel",	212, 222, 70, 35, 0, "LookUpCancelBtn");

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
		tStr.sprintf("Unable to open the table '%s'.",tableName);
		wxMessageBox(tStr.GetData(),"ODBC Error...");
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
	Show(TRUE);

}  // Generic lookup constructor


//
// This is a generic lookup constructor that will work with any table and any column.
// It extends the capabilites of the lookup dialog in the following ways:
//
//	1) 2 columns rather than one
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
	char *selectStmt, int maxLenCol1, wxDB *pDb, bool allowOk)  : wxDialogBox (parent, "Select...", 1, -1, -1, 400, 290)
{
	wxBeginBusyCursor();
	
	strcpy(ListDB_Selection,"");
	strcpy(ListDB_Selection2,"");
	widgetPtrsSet = FALSE;
	lookup  = 0;
	lookup2 = 0;
	noDisplayCols = (strlen(dispCol2) ? 2 : 1);
	col1Len = 0;

	// Build the dialog
	SetLabelPosition(wxVERTICAL);

	wxFont *ButtonFont = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD);
	wxFont *TextFont   = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	wxFont *FixedFont  = new wxFont(12,wxMODERN,wxNORMAL,wxNORMAL);

	SetButtonFont(ButtonFont);
	SetLabelFont(TextFont);
	SetLabelPosition(wxVERTICAL);

	// this is done with fixed font so that the second column (if any) will be left
	// justified in the second column
	SetButtonFont(FixedFont);
	pLookUpSelectList		= new wxListBox(this, NULL, "", wxSINGLE|wxALWAYS_SB, 5, 15, 384, 195, 0, 0, 0, "LookUpSelectList");
	SetButtonFont(ButtonFont);
	pLookUpOkBtn			= new wxButton(this, NULL, "&Ok",		113, 222, 70, 35, 0, "LookUpOkBtn");
	pLookUpCancelBtn		= new wxButton(this, NULL, "C&ancel",	212, 222, 70, 35, 0, "LookUpCancelBtn");

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
		tStr.sprintf("Unable to open the table '%s'.",tableName);
		wxMessageBox(tStr.GetData(),"ODBC Error...");
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
			if (!lookup2->QueryBySqlStmt(q.GetData()))
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
	if (selectStmt && strlen(selectStmt))	// Query by sql stmt passed in
	{
		if (!lookup2->QueryBySqlStmt(selectStmt))
		{
			wxMessageBox("ODBC error during QueryBySqlStmt()","ODBC Error...");
			Close();
			return;
		}
	}
	else	// Query using where and order by clauses
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
		if (strlen(dispCol2))		// Append the optional column 2
		{
			s.Append(' ', (maxColLen + LISTDB_NO_SPACES_BETWEEN_COLS - strlen(lookup2->lookupCol1)));
			s.Append(lookup2->lookupCol2);
		}
		pLookUpSelectList->Append(s.GetData());
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
	Show(TRUE);

}  // Generic lookup constructor 2


bool ClookUpDlg::OnClose(void)
{
	widgetPtrsSet = FALSE;
	GetParent()->Enable(TRUE);

	if (lookup)
		delete lookup;
	if (lookup2)
		delete lookup2;

	wxEndBusyCursor();
	return TRUE;

}  // ClookUpDlg::OnClose


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
					strcpy(ListDB_Selection, s.GetData());
					// Column 2
					s = pLookUpSelectList->GetStringSelection();
					s = s.From(col1Len + LISTDB_NO_SPACES_BETWEEN_COLS);
					s = s.Strip();
					strcpy(ListDB_Selection2, s.GetData());
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
