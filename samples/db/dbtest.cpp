///////////////////////////////////////////////////////////////////////////////
// Name:        dbtest.cpp
// Purpose:     wxWindows database demo app
// Author:      George Tasker
// Modified by:
// Created:     1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Remstar International, Inc.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
 *	SYNOPSIS START

	This sample program demonstrates the cross-platform ODBC database classes
	donated by the development team at Remstar International.

	The table this sample is based on is developer contact table, and shows
	some of the simple uses of the database classes wxDB and wxTable.

	

 *	SYNOPSIS END
 */

#ifdef __GNUG__
#pragma implementation "dbtest.h"
#endif

#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
#pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
#include  <wx/wx.h>
#endif //WX_PRECOMP

IMPLEMENT_APP(DatabaseDemoApp)

#include <stdio.h>					// Included strictly for reading the text file with the database parameters

#include	<wx/db.h>					// Required in the file which will get the data source connection
#include    <wx/dbtable.h>				// Has the wxTable object from which all data objects will inherit their data table functionality

extern DbList *PtrBegDbList;		// from wx_db.cpp, used in getting back error results from db connections

#include "dbtest.h"				// Header file for this demonstration program
#include "listdb.h"					// Code to support the "Lookup" button on the editor dialog
extern char ListDB_Selection[];	// Used to return the first column value for the selected line from the listDB routines
extern char ListDB_Selection2[]; // Used to return the second column value for the selected line from the listDB routines

DatabaseDemoFrame	*DemoFrame;		// Pointer to the main frame


// This statement initializes the whole application and calls OnInit
DatabaseDemoApp DatabaseDemoApp;


/* Pointer to the main database connection used in the program.  This
 * pointer would normally be used for doing things as database lookups
 * for user login names and passwords, getting workstation settings, etc.
 * ---> IMPORTANT <---
 * 
 *		For each database object created which uses this wxDB pointer
 *    connection to the database, when a CommitTrans() or RollBackTrans()
 *    will commit or rollback EVERY object which uses this wxDB pointer.
 *
 *    To allow each table object (those derived from wxTable) to be 
 *    individually committed or rolled back, you MUST use a different
 *    instance of wxDB in the constructor of the table.  Doing so creates 
 *		more overhead, and will use more database connections (some DBs have
 *    connection limits...), so use connections sparringly.
 *
 *		It is recommended that one "main" database connection be created for
 *		the entire program to use for READ-ONLY database accesses, but for each
 *		table object which will do a CommitTrans() or RollbackTrans() that a
 *		new wxDB object be created and used for it.
 */
wxDB	*READONLY_DB;


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
char *GetExtendedDBErrorMsg(char *ErrFile, int ErrLine)
{
	static wxString msg;

	wxString tStr;

	if (ErrFile || ErrLine)
	{
		msg += "\n";
		msg.Append ('-',80);
		msg += "\nFile: ";
		msg += ErrFile;
		msg += "   Line: ";
		tStr.sprintf("%d",ErrLine);
		msg += tStr.GetData();
		msg += "\n";
	}

	msg.Append ('-',80);
	msg.Append ("\nODBC ERRORS\n");
	msg.Append ('-',80);
	msg += "\n";
	// Scan through each database connection displaying
	// any ODBC errors that have occured.
	for (DbList *pDbList = PtrBegDbList; pDbList; pDbList = pDbList->PtrNext)
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

	return msg.GetData();
}  // GetExtendedDBErrorMsg



// `Main program' equivalent, creating windows and returning main app frame
wxFrame *DatabaseDemoApp::OnInit(void)
{
	// Create the main frame window
	DemoFrame = new DatabaseDemoFrame(NULL, "wxWindows Database Demo", 50, 50, 537, 480);

	// Give it an icon
#ifdef __WXMSW__
	DemoFrame->SetIcon(wxIcon("db_icon"));
#endif
#ifdef __X__
	DemoFrame->SetIcon(wxIcon("db.xbm"));
#endif

	// Make a menubar
	wxMenu *file_menu = new wxMenu;
	file_menu->Append(FILE_CREATE, "&Create contact table");
	file_menu->Append(FILE_EXIT, "E&xit");

	wxMenu *edit_menu = new wxMenu;
	edit_menu->Append(EDIT_PARAMETERS, "&Parameters...");

	wxMenu *about_menu = new wxMenu;
	about_menu->Append(ABOUT_DEMO, "&About");

	wxMenuBar *menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu, "&File");
	menu_bar->Append(edit_menu, "&Edit");
	menu_bar->Append(about_menu, "&About");
	DemoFrame->SetMenuBar(menu_bar);

	// Initialize the ODBC Environment for Database Operations
	if (SQLAllocEnv(&DbConnectInf.Henv) != SQL_SUCCESS)
	{
		wxMessageBox("A problem occured while trying to get a connection to the data source","DB CONNECTION ERROR",wxOK | wxICON_EXCLAMATION);
		return NULL;
	}

	FILE *paramFile;
	if ((paramFile = fopen(paramFilename, "r")) == NULL)
	{
		wxString tStr;
		tStr.sprintf("Unable to open the parameter file '%s' for reading.\n\nYou must specify the data source, user name, and\npassword that will be used and save those settings.",paramFilename);
		wxMessageBox(tStr.GetData(),"File I/O Error...",wxOK | wxICON_EXCLAMATION);
		DemoFrame->BuildParameterDialog(NULL);
		if ((paramFile = fopen(paramFilename, "r")) == NULL)
			return FALSE;
	}

	char buffer[1000+1];
	fgets(buffer, sizeof(params.ODBCSource), paramFile);
	buffer[strlen(buffer)-1] = '\0';
	strcpy(params.ODBCSource,buffer);

	fgets(buffer, sizeof(params.UserName), paramFile);
	buffer[strlen(buffer)-1] = '\0';
	strcpy(params.UserName,buffer);

	fgets(buffer, sizeof(params.Password), paramFile);
	buffer[strlen(buffer)-1] = '\0';
	strcpy(params.Password,buffer);

	fclose(paramFile);

	// Connect to datasource
	strcpy(DbConnectInf.Dsn,		params.ODBCSource);	// ODBC data source name (created with ODBC Administrator under Win95/NT)
	strcpy(DbConnectInf.Uid,		params.UserName);		// database username - must already exist in the data source
	strcpy(DbConnectInf.AuthStr,	params.Password);		// password database username
	READONLY_DB = GetDbConnection(&DbConnectInf);
	if (READONLY_DB == 0)
	{
		wxMessageBox("Unable to connect to the data source.\n\nCheck the name of your data source to verify it has been correctly entered/spelled.\n\nWith some databases, the user name and password must\nbe created with full rights to the CONTACT table prior to making a connection\n(using tools provided by the database manufacturer)", "DB CONNECTION ERROR...",wxOK | wxICON_EXCLAMATION);
		DemoFrame->BuildParameterDialog(NULL);
		strcpy(DbConnectInf.Dsn,		"");
		strcpy(DbConnectInf.Uid,		"");
		strcpy(DbConnectInf.AuthStr,	"");
		wxMessageBox("Now exiting program.\n\nRestart program to try any new settings.","Notice...",wxOK | wxICON_INFORMATION);
		return(FALSE);
	}

	DemoFrame->BuildEditorDialog();

	// Show the frame
	DemoFrame->Show(TRUE);

	// Return the main frame window
	return DemoFrame;
}  // DatabaseDemoApp::OnInit()



// DatabaseDemoFrame constructor
DatabaseDemoFrame::DatabaseDemoFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
// Put any code in necessary for initializing the main frame here
}


// Intercept menu commands
void DatabaseDemoFrame::OnMenuCommand(int id)
{
	switch (id)
	{
		case FILE_CREATE:
			CreateDataTable();
			break;
		case FILE_EXIT:
			Close();
			break;
		case EDIT_PARAMETERS:
			if ((pEditorDlg->mode != mCreate) && (pEditorDlg->mode != mEdit))
				BuildParameterDialog(this);
			else
				wxMessageBox("Cannot change database parameters while creating or editing a record","Notice...",wxOK | wxICON_INFORMATION);
			break;
		case ABOUT_DEMO:
			wxMessageBox("wxWindows sample program for database classes\n\nContributed on 27 July 1998","About...",wxOK | wxICON_INFORMATION);
			break;
	}
}  // DatabaseDemoFrame::OnMenuCommand()


Bool DatabaseDemoFrame::OnClose(void)
{
	// Put any additional checking necessary to make certain it is alright
	// to close the program here that is not done elsewhere

	return TRUE;
}  // DatabaseDemoFrame::OnClose()


void DatabaseDemoFrame::CreateDataTable()
{
	Bool Ok = (wxMessageBox("Any data currently residing in the table will be erased.\n\nAre you sure?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

	if (!Ok)
		return;

	wxBeginBusyCursor();

	Bool success = TRUE;

	Ccontact *Contact = new Ccontact();
	if (!Contact)
	{
		wxEndBusyCursor();
		wxMessageBox("Error allocating memory for 'Ccontact'object.\n\nTable was not created.","Error...",wxOK | wxICON_EXCLAMATION);
		return;
	}

	if (!Contact->CreateTable())
	{
		wxEndBusyCursor();
		wxString tStr;
		tStr  = "Error creating CONTACTS table.\nTable was not created.\n\n";
		tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
		wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
		success = FALSE;
	}
 	else
	{
		if (!Contact->CreateIndexes())
		{
			wxEndBusyCursor();
			wxString tStr;
			tStr  = "Error creating CONTACTS indexes.\nIndexes will be unavailable.\n\n";
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
			success = FALSE;
		}
	}
	wxEndBusyCursor();

	delete Contact;

	if (success)
		wxMessageBox("Table and index(es) were successfully created.","Notice...",wxOK | wxICON_INFORMATION);
}  // DatabaseDemoFrame::CreateDataTable()


void DatabaseDemoFrame::BuildEditorDialog()
{
	pEditorDlg = new CeditorDlg(this);
	if (!pEditorDlg)
		wxMessageBox("Unable to create the editor dialog for some reason","Error...",wxOK | wxICON_EXCLAMATION);
}  // DatabaseDemoFrame::BuildEditorDialog()


void DatabaseDemoFrame::BuildParameterDialog(wxWindow *parent)
{
	pParamDlg = new CparameterDlg(parent);

	if (!pParamDlg)
		wxMessageBox("Unable to create the parameter dialog for some reason","Error...",wxOK | wxICON_EXCLAMATION);
}  // DatabaseDemoFrame::BuildParameterDialog()


/*
 * Constructor note: If no wxDB object is passed in, a new connection to the database
 *     is created for this instance of Ccontact.  This can be a slow process depending
 *     on the database engine being used, and some database engines have a limit on the
 *     number of connections (either hard limits, or license restricted) so care should 
 *     be used to use as few connections as is necessary.  
 * IMPORTANT: Objects which share a wxDB pointer are ALL acted upon whenever a member 
 *     function of pDb is called (i.e. CommitTrans() or RollbackTrans(), so if modifying 
 *     or creating a table objects which use the same pDb, know that all the objects
 *     will be committed or rolled back when any of the objects has this function call made.
 */
Ccontact::Ccontact (wxDB *pwxDB) : wxTable(pwxDB ? pwxDB : GetDbConnection(&DbConnectInf),CONTACT_TABLE_NAME,CONTACT_NO_COLS)
{
	// This is used to represent whether the database connection should be released
	// when this instance of the object is deleted.  If using the same connection
	// for multiple instance of database objects, then the connection should only be 
	// released when the last database instance using the connection is deleted
	freeDbConn = !pwxDB;
	
	SetupColumns();

}  // Ccontact Constructor


void Ccontact::Initialize()
{
	Name[0]					= 0;
	Addr1[0]					= 0;
	Addr2[0]					= 0;
	City[0]					= 0;
	State[0]					= 0;
	PostalCode[0]			= 0;
	Country[0]				= 0;
	JoinDate.year			= 1980;
	JoinDate.month			= 1;
	JoinDate.day			= 1;
	JoinDate.hour			= 0;
	JoinDate.minute		= 0;
	JoinDate.second		= 0;
	JoinDate.fraction		= 0;
	NativeLanguage			= langENGLISH;
	IsDeveloper				= FALSE;
	Contributions			= 0;
	LinesOfCode				= 0L;
}  // Ccontact::Initialize


Ccontact::~Ccontact()
{
	if (freeDbConn)
	{
		if (!FreeDbConnection(pDb))
		{
			wxString tStr;
			tStr  = "Unable to Free the Ccontact data table handle\n\n";
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
		}
	}
}  // Ccontract destructor


/*
 * Handles setting up all the connections for the interface from the wxTable
 * functions to interface to the data structure used to store records in 
 * memory, and for all the column definitions that define the table structure
 */
void Ccontact::SetupColumns()
{
	SetColDefs ( 0,"NAME",					DB_DATA_TYPE_VARCHAR,	 Name,					SQL_C_CHAR,			sizeof(Name),				TRUE, TRUE);  // Primary index
	SetColDefs ( 1,"ADDRESS1",				DB_DATA_TYPE_VARCHAR,	 Addr1,					SQL_C_CHAR,			sizeof(Addr1),				FALSE,TRUE);
	SetColDefs ( 2,"ADDRESS2",				DB_DATA_TYPE_VARCHAR,	 Addr2,					SQL_C_CHAR,			sizeof(Addr2),				FALSE,TRUE);
	SetColDefs ( 3,"CITY",					DB_DATA_TYPE_VARCHAR,	 City,					SQL_C_CHAR,			sizeof(City),				FALSE,TRUE);
	SetColDefs ( 4,"STATE",					DB_DATA_TYPE_VARCHAR,	 State,					SQL_C_CHAR,			sizeof(State),				FALSE,TRUE);
	SetColDefs ( 5,"POSTAL_CODE",			DB_DATA_TYPE_VARCHAR,	 PostalCode,			SQL_C_CHAR,			sizeof(PostalCode),		FALSE,TRUE);
	SetColDefs ( 6,"COUNTRY",				DB_DATA_TYPE_VARCHAR,	 Country,				SQL_C_CHAR,			sizeof(Country),			FALSE,TRUE);
	SetColDefs ( 7,"JOIN_DATE",			DB_DATA_TYPE_DATE,		&JoinDate,				SQL_C_TIMESTAMP,	sizeof(JoinDate),			FALSE,TRUE);
	SetColDefs ( 8,"NATIVE_LANGUAGE",	DB_DATA_TYPE_INTEGER,	&NativeLanguage,		SQL_C_ENUM,			sizeof(NativeLanguage),	FALSE,TRUE);
	SetColDefs ( 9,"IS_DEVELOPER",		DB_DATA_TYPE_INTEGER,	&IsDeveloper,			SQL_C_BOOLEAN,		sizeof(Bool),				FALSE,TRUE);
	SetColDefs (10,"CONTRIBUTIONS",		DB_DATA_TYPE_INTEGER,	&Contributions,		SQL_C_USHORT,		sizeof(Contributions),	FALSE,TRUE);
	SetColDefs (11,"LINES_OF_CODE",		DB_DATA_TYPE_INTEGER,	&LinesOfCode,			SQL_C_ULONG,		sizeof(LinesOfCode),		FALSE,TRUE);
}  // Ccontact::SetupColumns


Bool Ccontact::CreateIndexes(void)
{
	// This index could easily be accomplished with an "orderBy" clause, 
	// but is done to show how to construct a non-primary index.
	wxString	indexName;
	CidxDef	idxDef[2];

	Bool		Ok = TRUE;

	strcpy(idxDef[0].ColName, "IS_DEVELOPER");
	idxDef[0].Ascending = TRUE;

	strcpy(idxDef[1].ColName, "NAME");
	idxDef[1].Ascending = TRUE;

	indexName = CONTACT_TABLE_NAME;
	indexName += "_IDX1";
	Ok = CreateIndex(indexName.GetData(), TRUE, 2, idxDef);

	return Ok;
}  // Ccontact::CreateIndexes()


/*
 * Having a function to do a query on the primary key (and possibly others) is
 * very efficient and tighter coding so that it is available where ever the object
 * is.  Great for use with multiple tables when not using views or outer joins
 */
Bool Ccontact::FetchByName(char *name)
{
	whereStr.sprintf("NAME = '%s'",name);
	where = this->whereStr.GetData();
	orderBy = 0;

	if (!Query())
		return(FALSE);

	// Fetch the record
	return(GetNext());

}  // Ccontact::FetchByName()


/*
 *
 *  ************* DIALOGS ***************
 *
 */


/* CeditorDlg constructor
 *
 * Creates the dialog used for creating/editing/deleting/copying a Ccontact object.
 * This dialog actually is drawn in the main frame of the program
 *
 * An instance of Ccontact is created - "Contact" - which is used to hold the Ccontact
 * object that is currently being worked with.
 */
CeditorDlg::CeditorDlg(wxWindow *parent) : wxPanel (parent, 1, 1, 460, 455)
{
	// Since the ::OnCommand() function is overridden, this prevents the widget
	// detection in ::OnCommand() until all widgets have been initialized to prevent
	// uninitialized pointers from crashing the program
	widgetPtrsSet = FALSE;

	// Create the data structure and a new database connection.  
	// (As there is not a pDb being passed in the constructor, a new database 
	// connection is created)
	Contact = new Ccontact();

	if (!Contact)
	{
		wxMessageBox("Unable to instantiate an instance of Ccontact","Error...",wxOK | wxICON_EXCLAMATION);
		return;
	}

	// Check if the table exists or not.  If it doesn't, ask the user if they want to 
	// create the table.  Continue trying to create the table until it exists, or user aborts
	while (!Contact->pDb->TableExists((char *)CONTACT_TABLE_NAME))
	{
		wxString tStr;
		tStr.sprintf("Unable to open the table '%s'.\n\nTable may need to be created...?\n\n",CONTACT_TABLE_NAME);
		tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
		wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);

		Bool createTable = (wxMessageBox("Do you wish to try to create/clear the CONTACTS table?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

		if (!createTable)
		{
			delete Contact;
			Close();
			DemoFrame->Close();
			return;
		}
		else
			DemoFrame->CreateDataTable();
	}

	// Tables must be "opened" before anything other than creating/deleting table can be done
	if (!Contact->Open())
	{
		// Table does exist, there was some problem opening it.  Currently this should
		// never fail, except in the case of the table not exisiting.  Open() basically
		// only sets up variable/pointer values, other than checking for table existence.
		if (Contact->pDb->TableExists((char *)CONTACT_TABLE_NAME))
		{
			wxString tStr;
			tStr.sprintf("Unable to open the table '%s'.\n\n",CONTACT_TABLE_NAME);
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
			delete Contact;
			Close();
			DemoFrame->Close();
			return;
		}
	}

	// Build the dialog
	SetLabelPosition(wxHORIZONTAL);

	wxFont *ButtonFont = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD);
	wxFont *TextFont   = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);

	SetButtonFont(ButtonFont);
	SetLabelFont(TextFont);
	SetLabelPosition(wxVERTICAL);

	wxGroupBox *FunctionGrp		= new wxGroupBox(this, "",  15, 1, 497,  69, 0, "FunctionGrp");
	wxGroupBox *SearchGrp		= new wxGroupBox(this, "", 417, 1, 95, 242, 0, "SearchGrp");

	pCreateBtn		= new wxButton(this, NULL, "&Create",  25,  21, 70, 35, 0, "CreateBtn");
	pEditBtn			= new wxButton(this, NULL, "&Edit",	102,  21, 70, 35, 0, "EditBtn");
	pDeleteBtn		= new wxButton(this, NULL, "&Delete",	179,  21, 70, 35, 0, "DeleteBtn");
	pCopyBtn			= new wxButton(this, NULL, "Cop&y",	256,  21, 70, 35, 0, "CopyBtn");
	pSaveBtn			= new wxButton(this, NULL, "&Save",	333,  21, 70, 35, 0, "SaveBtn");
	pCancelBtn		= new wxButton(this, NULL, "C&ancel",	430,  21, 70, 35, 0, "CancelBtn");

	pPrevBtn			= new wxButton(this, NULL, "<< &Prev",430,  81, 70, 35, 0, "PrevBtn");
	pNextBtn			= new wxButton(this, NULL, "&Next >>",430, 121, 70, 35, 0, "NextBtn");
	pQueryBtn		= new wxButton(this, NULL, "&Query",	430, 161, 70, 35, 0, "QueryBtn");
	pResetBtn		= new wxButton(this, NULL, "&Reset",	430, 200, 70, 35, 0, "ResetBtn");

	pNameMsg			= new wxMessage(this, "Name:", 17, 80, -1, -1, 0, "NameMsg");
	pNameTxt			= new wxText(this, NULL, "", "", 17, 97, 308, 25, 0, "NameTxt");
	pNameListBtn	= new wxButton(this, NULL, "&Lookup",	333, 99, 70, 24, 0, "LookupBtn");

	pAddress1Msg	= new wxMessage(this, "Address:", 17, 130, -1, -1, 0, "Address1Msg");
	pAddress1Txt	= new wxText(this, NULL, "", "", 17, 147, 308, 25, 0, "Address1Txt");

	pAddress2Msg	= new wxMessage(this, "Address:", 17, 180, -1, -1, 0, "Address2Msg");
	pAddress2Txt	= new wxText(this, NULL, "", "", 17, 197, 308, 25, 0, "Address2Txt");

	pCityMsg			= new wxMessage(this, "City:", 17, 230, -1, -1, 0, "CityMsg");
	pCityTxt			= new wxText(this, NULL, "", "", 17, 247, 225, 25, 0, "CityTxt");

	pStateMsg		= new wxMessage(this, "State:", 250, 230, -1, -1, 0, "StateMsg");
	pStateTxt		= new wxText(this, NULL, "", "", 250, 247, 153, 25, 0, "StateTxt");

	pCountryMsg		= new wxMessage(this, "Country:", 17, 280, -1, -1, 0, "CountryMsg");
	pCountryTxt		= new wxText(this, NULL, "", "", 17, 297, 225, 25, 0, "CountryTxt");

	pPostalCodeMsg	= new wxMessage(this, "Postal Code:", 250, 280, -1, -1, 0, "PostalCodeMsg");
	pPostalCodeTxt	= new wxText(this, NULL, "", "", 250, 297, 153, 25, 0, "PostalCodeTxt");

	char *choice_strings[5];
	choice_strings[0] = "English";
	choice_strings[1] = "French";
	choice_strings[2] = "German";
	choice_strings[3] = "Spanish";
	choice_strings[4] = "Other";
	pNativeLangChoice = new wxChoice(this, NULL, "",17, 346, 277, -1, 5, choice_strings);
	pNativeLangMsg    = new wxMessage(this, "Native language:", 17, 330, -1, -1, 0, "NativeLangMsg");

	char *radio_strings[2];
	radio_strings[0]	= "No";
	radio_strings[1]	= "Yes";
	pDeveloperRadio	= new wxRadioBox(this,NULL,"Developer:",303,330,-1,-1,2,radio_strings,2,wxHORIZONTAL|wxFLAT);

	pJoinDateMsg		= new wxMessage(this, "Date joined:", 17, 380, -1, -1, 0, "JoinDateMsg");
	pJoinDateTxt		= new wxText(this, NULL, "", "", 17, 397, 150, 25, 0, "JoinDateTxt");

	pContribMsg			= new wxMessage(this, "Contributions:", 175, 380, -1, -1, 0, "ContribMsg");
	pContribTxt			= new wxText(this, NULL, "", "", 175, 397, 120, 25, 0, "ContribTxt");

	pLinesMsg			= new wxMessage(this, "Lines of code:", 303, 380, -1, -1, 0, "LinesMsg");
	pLinesTxt			= new wxText(this, NULL, "", "", 303, 397, 100, 25, 0, "LinesTxt");

	// Now that all the widgets on the panel are created, its safe to allow ::OnCommand() to 
	// handle all widget processing
	widgetPtrsSet = TRUE;

	// Setup the orderBy and where clauses to return back a single record as the result set, 
	// as there will only be one record being shown on the dialog at a time, this optimizes
	// network traffic by only returning a one row result
	
	Contact->orderBy = "NAME";  // field name to sort by

	// The wxString "whereStr" is not a member of the wxTable object, it is a member variable
	// specifically in the Ccontact class.  It is used here for simpler construction of a varying
	// length string, and then after the string is built, the wxTable member variable "where" is
	// assigned the pointer to the constructed string.
	//
	// The constructed where clause below has a sub-query within it "SELECT MIN(NAME) FROM %s" 
	// to achieve a single row (in this case the first name in alphabetical order).
	Contact->whereStr.sprintf("NAME = (SELECT MIN(NAME) FROM %s)",Contact->tableName);
	
	// NOTE: GetData() returns a pointer which may not be valid later, so this is short term use only
	Contact->where = Contact->whereStr.GetData();  

	// Perform the Query to get the result set.  
	// NOTE: If there are no rows returned, that is a valid result, so Query() would return TRUE.  
	//       Only if there is a database error will Query() come back as FALSE
	if (!Contact->Query())
	{
		wxString tStr;
		tStr  = "ODBC error during Query()\n\n";
		tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
		wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
		GetParent()->Close();
		return;
	}

	// Since Query succeeded, now get the row that was returned
	if (!Contact->GetNext())
		// If the GetNext() failed at this point, then there are no rows to retrieve, 
		// so clear the values in the members of "Contact" so that PutData() blanks the 
		// widgets on the dialog
		Contact->Initialize();

	SetMode(mView);
	PutData();

	Show(TRUE);
}  // CeditorDlg constructor


Bool CeditorDlg::OnClose()
{
	// Clean up time
 	if ((mode != mCreate) && (mode != mEdit))
	{
		if (Contact)
			delete Contact;
		return TRUE;
	}
	else
	{
		wxMessageBox("Must finish processing the current record being created/modified before exiting","Notice...",wxOK | wxICON_INFORMATION);
		return FALSE;
	}
}  // CeditorDlg::OnClose()


void CeditorDlg::OnCommand(wxWindow& win, wxCommandEvent& event)
{
	wxString widgetName;
	
	widgetName = win.GetName();

	if (!widgetPtrsSet)
		return;

	if (widgetName == pCreateBtn->GetName())
	{
		Contact->Initialize();
		PutData();
		SetMode( mCreate );
		pNameTxt->SetValue("");
		pNameTxt->SetFocus();
		return;
	}

	if (widgetName == pEditBtn->GetName())
	{
		saveName = Contact->Name;
		SetMode( mEdit );
		pNameTxt->SetFocus();
		return;
	}

	if (widgetName == pCopyBtn->GetName())
	{
		SetMode(mCreate);
		pNameTxt->SetValue("");
		pNameTxt->SetFocus();
		return;
	}

	if (widgetName == pDeleteBtn->GetName())
	{
		Bool Ok = (wxMessageBox("Are you sure?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

		if (!Ok)
			return;

		if (Ok && Contact->Delete())
		{
			// NOTE: Deletions are not finalized until a CommitTrans() is performed.  
			//       If the commit were not performed, the program will continue to 
			//       show the table contents as if they were deleted until this instance
			//       of Ccontact is deleted.  If the Commit wasn't performed, the 
			//       database will automatically Rollback the changes when the database
			//       connection is terminated
			Contact->pDb->CommitTrans();

			// Try to get the row that followed the just deleted row in the orderBy sequence
			if (!GetNextRec())
			{
				// There was now row (in sequence) after the just deleted row, so get the
				// row which preceded the just deleted row
				if (!GetPrevRec())
				{
					// There are now no rows remaining, so clear the dialog widgets
					Contact->Initialize();
					PutData();
				}
			}
			SetMode(mode);    // force reset of button enable/disable
		}
		else
			// Delete failed
			Contact->pDb->RollbackTrans();

		SetMode(mView);
		return;
	}

	if (widgetName == pSaveBtn->GetName())
	{
		Save();
		return;
	}

	if (widgetName == pCancelBtn->GetName())
	{
		Bool Ok = (wxMessageBox("Are you sure?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

		if (!Ok)
			return;

		if (!strcmp(saveName.GetData(),""))
		{
			Contact->Initialize();
			PutData();
			SetMode(mView);
			return;
		}
		else
		{
			// Requery previous record
			if (Contact->FetchByName(saveName.GetData()))
			{
				PutData();
				SetMode(mView);
				return;
			}
		}

		// Previous record not available, retrieve first record in table
		Contact->whereStr  = "NAME = (SELECT MIN(NAME) FROM ";
		Contact->whereStr += Contact->tableName;
		Contact->whereStr += ")";

		Contact->where = Contact->whereStr.GetData();
		if (!Contact->Query())
		{
			wxString tStr;
			tStr  = "ODBC error during Query()\n\n";
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
			SetMode(mView);
			return;
		}
		if (Contact->GetNext())  // Successfully read first record
		{
			PutData();
			SetMode(mView);
			return;
		}
		// No contacts are available, clear dialog
		Contact->Initialize();
		PutData();
		SetMode(mView);
		return;
	}  // Cancel Button

	if (widgetName == pPrevBtn->GetName())
	{
		if (!GetPrevRec())
			wxBell();
		return;
	}  // Prev Button

	if (widgetName == pNextBtn->GetName())
	{
		if (!GetNextRec())
			wxBell();
		return;
	}  // Next Button

	if (widgetName == pQueryBtn->GetName())
	{
		// Display the query dialog box
		char qryWhere[DB_MAX_WHERE_CLAUSE_LEN+1];
		strcpy(qryWhere, Contact->qryWhereStr.GetData());
		char *tblName[] = {(char *)CONTACT_TABLE_NAME, 0};
		new CqueryDlg(GetParent(), Contact->pDb, tblName, qryWhere);

		// Query the first record in the new record set and
		// display it, if the query string has changed.
		if (strcmp(qryWhere, Contact->qryWhereStr.GetData()))
		{
			Contact->orderBy		= "NAME";
			Contact->whereStr		= "NAME = (SELECT MIN(NAME) FROM ";
			Contact->whereStr		+= CONTACT_TABLE_NAME;
			// Append the query where string (if there is one)
			Contact->qryWhereStr	= qryWhere;
			if (strlen(qryWhere))
			{
				Contact->whereStr		+= " WHERE ";
				Contact->whereStr		+= Contact->qryWhereStr;
			}
			// Close the expression with a right paren
			Contact->whereStr += ")";
			// Requery the table
			Contact->where = Contact->whereStr.GetData();
			if (!Contact->Query())
			{
				wxString tStr;
				tStr  = "ODBC error during Query()\n\n";
				tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
				wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
				return;
			}
			// Display the first record from the query set
			if (!Contact->GetNext())
				Contact->Initialize();
			PutData();
		}

		// Enable/Disable the reset button
		pResetBtn->Enable(!Contact->qryWhereStr.Empty());

		return;
	}  // Query button


	if (widgetName == pResetBtn->GetName())
	{
		// Clear the additional where criteria established by the query feature
		Contact->qryWhereStr = "";

		// Query the first record in the table
		Contact->orderBy		= "NAME";
		Contact->whereStr		= "NAME = (SELECT MIN(NAME) FROM ";
		Contact->whereStr		+= CONTACT_TABLE_NAME;
		Contact->whereStr		+= ")";
		Contact->where			= Contact->whereStr.GetData();
		if (!Contact->Query())
		{
			wxString tStr;
			tStr  = "ODBC error during Query()\n\n";
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
			return;
		}
		if (!Contact->GetNext())
			Contact->Initialize();
		PutData();
		pResetBtn->Enable(FALSE);

		return;
	}  // Reset button


	if (widgetName == pNameListBtn->GetName())
	{
		new ClookUpDlg(/* wxWindow	*parent			*/ this,
							/* char		*windowTitle	*/ "Select contact name",
							/* char		*tableName		*/ (char *) CONTACT_TABLE_NAME,
							/* char		*dispCol1		*/ "NAME",
							/* char		*dispCol2		*/ "JOIN_DATE",
							/* char		*where			*/ "",
							/* char		*orderBy			*/ "NAME",
							/* Bool		distinctValues */ TRUE);

		if (ListDB_Selection && strlen(ListDB_Selection))
		{
			wxString w = "NAME = '";
			w += ListDB_Selection;
			w += "'";
			GetRec(w.GetData());
		}

		return;
	}

}  // CeditorDlg::OnCommand()


void CeditorDlg::FieldsEditable()
{
	pNameTxt->Enable((mode == mCreate) || (mode == mEdit));
	pAddress1Txt->Enable((mode == mCreate) || (mode == mEdit));
	pAddress2Txt->Enable((mode == mCreate) || (mode == mEdit));
	pCityTxt->Enable((mode == mCreate) || (mode == mEdit));
	pStateTxt->Enable((mode == mCreate) || (mode == mEdit));
	pPostalCodeTxt->Enable((mode == mCreate) || (mode == mEdit));
	pCountryTxt->Enable((mode == mCreate) || (mode == mEdit));

	pJoinDateTxt->Enable((mode == mCreate) || (mode == mEdit));
	pContribTxt->Enable((mode == mCreate) || (mode == mEdit));
	pLinesTxt->Enable((mode == mCreate) || (mode == mEdit));
	pNativeLangChoice->Enable((mode == mCreate) || (mode == mEdit));
	pDeveloperRadio->Enable((mode == mCreate) || (mode == mEdit));

}  // CeditorDlg::FieldsEditable()


void CeditorDlg::SetMode(enum DialogModes m)
{
	Bool	edit = FALSE;

	mode = m;
	switch (mode)
	{
		case mCreate:
		case mEdit:
			edit = TRUE;
			break;
		case mView:
		case mSearch:
			edit = FALSE;
			break;
		default:
				break;
	};

	if (widgetPtrsSet)
	{
  		pCreateBtn->Enable( !edit );
  		pEditBtn->Enable( !edit && (strcmp(Contact->Name,"")!=0) );
  		pDeleteBtn->Enable( !edit && (strcmp(Contact->Name,"")!=0) );
  		pCopyBtn->Enable( !edit && (strcmp(Contact->Name,"")!=0) );
  		pSaveBtn->Enable( edit );
		pCancelBtn->Enable( edit );
		pPrevBtn->Enable( !edit );
		pNextBtn->Enable( !edit );
		pQueryBtn->Enable( !edit );
		pResetBtn->Enable( !edit && !Contact->qryWhereStr.Empty() );
  		pNameListBtn->Enable( !edit );
	}

	FieldsEditable();
}  // CeditorDlg::SetMode()


Bool CeditorDlg::PutData()
{
	wxString tStr;

	pNameTxt->SetValue(Contact->Name);
	pAddress1Txt->SetValue(Contact->Addr1);
	pAddress2Txt->SetValue(Contact->Addr2);
	pCityTxt->SetValue(Contact->City);
	pStateTxt->SetValue(Contact->State);
	pCountryTxt->SetValue(Contact->Country);
	pPostalCodeTxt->SetValue(Contact->PostalCode);

	tStr.sprintf("%d/%d/%d",Contact->JoinDate.month,Contact->JoinDate.day,Contact->JoinDate.year);
	pJoinDateTxt->SetValue(tStr.GetData());

	tStr.sprintf("%d",Contact->Contributions);
	pContribTxt->SetValue(tStr.GetData());

	tStr.sprintf("%lu",Contact->LinesOfCode);
	pLinesTxt->SetValue(tStr.GetData());

	pNativeLangChoice->SetSelection(Contact->NativeLanguage);

	pDeveloperRadio->SetSelection(Contact->IsDeveloper);

	return TRUE;
}  // Ceditor::PutData()


/*
 * Reads the data out of all the widgets on the dialog.  Some data evaluation is done
 * to ensure that there is a name entered and that the date field is valid.
 *
 * A return value of TRUE means that valid data was retrieved from the dialog, otherwise
 * invalid data was found (and a message was displayed telling the user what to fix), and
 * the data was not placed into the appropraite fields of Ccontact
 */
Bool CeditorDlg::GetData()
{
	// Validate that the data currently entered into the widgets is valid data

	wxString tStr;
	tStr = pNameTxt->GetValue();
	if (!strcmp(tStr.GetData(),""))
	{
		wxMessageBox("A name is required for entry into the contact table","Notice...",wxOK | wxICON_INFORMATION);
		return FALSE;
	}

	Bool	invalid = FALSE;
	int	mm,dd,yyyy;
	int	first, second;

	tStr = pJoinDateTxt->GetValue();
	if (tStr.Freq('/') != 2)
		invalid = TRUE;

	// Find the month, day, and year tokens
	if (!invalid)
	{
		first		= tStr.First('/');
		second	= tStr.Last('/');

		mm = atoi(tStr.SubString(0,first));
		dd = atoi(tStr.SubString(first+1,second));
		yyyy = atoi(tStr.SubString(second+1,tStr.Length()-1));

		invalid = !(mm && dd && yyyy);
	}

	// Force Year 2000 compliance
	if (!invalid && (yyyy < 1000))
		invalid = TRUE;

	// Check the token ranges for validity
	if (!invalid)
	{
		if (yyyy > 9999)
			invalid = TRUE;
		else if ((mm < 1) || (mm > 12))
			invalid = TRUE;
		else
		{
			if (dd < 1)
				invalid = TRUE;
			else
			{
				int days[12] = {31,28,31,30,31,30,
									 31,31,30,31,30,31};
				if (dd > days[mm-1])
				{
					invalid = TRUE;
					if ((dd == 29) && (mm == 2))
					{
						if (((yyyy % 4) == 0) && (((yyyy % 100) != 0) || ((yyyy % 400) == 0)))
							invalid = FALSE;
					}
				}
			}
		}
	}

	if (!invalid)
	{
		Contact->JoinDate.month = mm;
		Contact->JoinDate.day   = dd;
		Contact->JoinDate.year  = yyyy;
	}
	else
	{
		wxMessageBox("Improper date format.  Please check the date\nspecified and try again.\n\nNOTE: Dates are in american format (MM/DD/YYYY)","Notice...",wxOK | wxICON_INFORMATION);
		return FALSE;
	}

	tStr = pNameTxt->GetValue();
	strcpy(Contact->Name,tStr.GetData());
	strcpy(Contact->Addr1,pAddress1Txt->GetValue());
	strcpy(Contact->Addr2,pAddress2Txt->GetValue());
	strcpy(Contact->City,pCityTxt->GetValue());
	strcpy(Contact->State,pStateTxt->GetValue());
	strcpy(Contact->Country,pCountryTxt->GetValue());
	strcpy(Contact->PostalCode,pPostalCodeTxt->GetValue());

	Contact->Contributions = atoi(pContribTxt->GetValue());
	Contact->LinesOfCode = atol(pLinesTxt->GetValue());

	Contact->NativeLanguage = (enum Language) pNativeLangChoice->GetSelection();
	Contact->IsDeveloper = pDeveloperRadio->GetSelection();

	return TRUE;
}  // CeditorDlg::GetData()


/*
 * Retrieve data from the dialog, verify the validity of the data, and if it is valid,
 * try to insert/update the data to the table based on the current 'mode' the dialog
 * is set to.
 *
 * A return value of TRUE means the insert/update was completed successfully, a return
 * value of FALSE means that Save() failed.  If returning FALSE, then this function
 * has displayed a detailed error message for the user.
 */
Bool CeditorDlg::Save()
{
	Bool failed = FALSE;

	// Read the data in the widgets of the dialog to get the user's data
	if (!GetData())
		failed = TRUE;

	// Perform any other required validations necessary before saving


	if (!failed)
	{
		wxBeginBusyCursor();

		if (mode == mCreate)
		{
			RETCODE result = Contact->Insert();

			failed = (result != DB_SUCCESS);
			if (failed)
			{
				// Some errors may be expected, like a duplicate key, so handle those instances with
				// specific error messages.
				if (result == DB_ERR_INTEGRITY_CONSTRAINT_VIOL)
				{
					wxString tStr;
					tStr  = "A duplicate key value already exists in the table.\nUnable to save record\n\n";
					tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
					wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
				}
				else
				{
					// Some other unexpexted error occurred
					wxString tStr;
					tStr  = "Database insert failed\n\n";
					tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
					wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
				}
			}
		}
		else  // mode == mEdit
		{
			if (!Contact->Update())
			{
				wxString tStr;
				tStr  = "Database update failed\n\n";
				tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
				wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
				failed = TRUE;
			}
		}

		if (!failed)
		{
			Contact->pDb->CommitTrans();
			SetMode(mView);  // Sets the dialog mode back to viewing after save is successful
		}
		else
			Contact->pDb->RollbackTrans();

		wxEndBusyCursor();
	}

	return !failed;
}  // CeditorDlg::Save()


/*
 * Where this program is only showing a single row at a time in the dialog,
 * a special where clause must be built to find just the single row which,
 * in sequence, would follow the currently displayed row.
 */
Bool CeditorDlg::GetNextRec()
{
	wxString w;

	w  = "NAME = (SELECT MIN(NAME) FROM ";
	w += Contact->tableName;
	w += " WHERE NAME > '";
	w += Contact->Name;
	w += "'";

	// If a query where string is currently set, append that criteria
	if (!Contact->qryWhereStr.Empty())
	{
		w += " AND (";
		w += Contact->qryWhereStr;
		w += ")";
	}

	w += ")";

	return(GetRec(w.GetData()));

}  // CeditorDlg::GetNextRec()


/*
 * Where this program is only showing a single row at a time in the dialog,
 * a special where clause must be built to find just the single row which,
 * in sequence, would precede the currently displayed row.
 */
Bool CeditorDlg::GetPrevRec()
{
	wxString w;

	w  = "NAME = (SELECT MAX(NAME) FROM ";
	w +=	Contact->tableName;
	w += " WHERE NAME < '";
	w += Contact->Name;
	w += "'";

	// If a query where string is currently set, append that criteria
	if (!Contact->qryWhereStr.Empty())
	{
		w += " AND (";
		w += Contact->qryWhereStr;
		w += ")";
	}

	w += ")";

	return(GetRec(w.GetData()));

}  // CeditorDlg::GetPrevRec()


/*
 * This function is here to avoid duplicating this same code in both the
 * GetPrevRec() and GetNextRec() functions
 */
Bool CeditorDlg::GetRec(char *whereStr)
{
	Contact->where = whereStr;
	Contact->orderBy = "NAME";
	
	if (!Contact->Query())
	{
		wxString tStr;
		tStr  = "ODBC error during Query()\n\n";
		tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
		wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);

		return(FALSE);
	}

	if (Contact->GetNext())
	{
		PutData();
		return(TRUE);
	}
	else
		return(FALSE);
}  // CeditorDlg::GetRec()



/*
 * CparameterDlg constructor
 */
CparameterDlg::CparameterDlg(wxWindow *parent) : wxDialogBox (parent, "ODBC parameter settings", 1, -1, -1, 400, 275)
{
	// Since the ::OnCommand() function is overridden, this prevents the widget
	// detection in ::OnCommand() until all widgets have been initialized to prevent
	// uninitialized pointers from crashing the program
	widgetPtrsSet = FALSE;

	// Build the dialog
	SetLabelPosition(wxVERTICAL);

	wxFont *ButtonFont = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD);
	wxFont *TextFont   = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);

	SetButtonFont(ButtonFont);
	SetLabelFont(TextFont);
	SetLabelPosition(wxVERTICAL);

	pParamODBCSourceMsg	= new wxMessage(this, "ODBC data sources:", 10, 10, -1, -1, 0, "ParamODBCSourceMsg");
	pParamODBCSourceList	= new wxListBox(this, NULL, "", wxSINGLE|wxALWAYS_SB, 10, 29, 285, 150, 0, 0, 0, "ParamODBCSourceList");

	pParamUserNameMsg		= new wxMessage(this, "Database user name:", 10, 193, -1, -1, 0, "ParamUserNameMsg");
	pParamUserNameTxt		= new wxText(this, NULL, "", "", 10, 209, 140, 25, 0, "ParamUserNameTxt");

	pParamPasswordMsg		= new wxMessage(this, "Password:", 156, 193, -1, -1, 0, "ParamPasswordMsg");
	pParamPasswordTxt		= new wxText(this, NULL, "", "", 156, 209, 140, 25, 0, "ParamPasswordTxt");
	
	pParamSaveBtn			= new wxButton(this, NULL, "&Save",	310,  21, 70, 35, 0, "ParamSaveBtn");
	pParamCancelBtn		= new wxButton(this, NULL, "C&ancel",	310,  66, 70, 35, 0, "ParamCancelBtn");

	// Now that all the widgets on the panel are created, its safe to allow ::OnCommand() to 
	// handle all widget processing
	widgetPtrsSet = TRUE;

	saved = FALSE;
	savedParamSettings = DatabaseDemoApp.params;

	Centre(wxBOTH);
	PutData();
	Show(TRUE);
}  // CparameterDlg constructor


Bool CparameterDlg::OnClose()
{
	// Put any additional checking necessary to make certain it is alright
	// to close the program here that is not done elsewhere
	if (!saved)
	{
		Bool Ok = (wxMessageBox("No changes have been saved.\n\nAre you sure you wish exit the parameter screen?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

		if (!Ok)
			return FALSE;

		DatabaseDemoApp.params = savedParamSettings;
	}

	if (GetParent() != NULL)
		GetParent()->SetFocus();
	return TRUE;
}  // Cparameter::OnClose()


void CparameterDlg::OnCommand(wxWindow& win, wxCommandEvent& event)
{
	wxString widgetName;
	
	widgetName = win.GetName();

	if (!widgetPtrsSet)
		return;

	if (widgetName == pParamSaveBtn->GetName())
	{
		if (Save())
		{
			wxString tStr;
			tStr = "Database parameters have been saved.";
			if (GetParent() != NULL)  // The parameter dialog was not called during startup due to a missing cfg file
				tStr += "\nNew parameters will take effect the next time the program is started.";
			wxMessageBox(tStr.GetData(),"Notice...",wxOK | wxICON_INFORMATION);
			saved = TRUE;
			Close();
		}
		return;
	}

	if (widgetName == pParamCancelBtn->GetName())
	{
		Close();
		return;
	}
}  // CparameterDlg::OnCommand()


Bool CparameterDlg::PutData()
{
	// Fill the data source list box
	FillDataSourceList();

	// Fill in the fields from the params object
	pParamODBCSourceList->SetStringSelection(DatabaseDemoApp.params.ODBCSource);
	pParamUserNameTxt->SetValue(DatabaseDemoApp.params.UserName);
	pParamPasswordTxt->SetValue(DatabaseDemoApp.params.Password);
	return TRUE;
}  // CparameterDlg::PutData()


Bool CparameterDlg::GetData()
{
	wxString tStr;
	if (pParamODBCSourceList->GetStringSelection())
	{
		tStr = pParamODBCSourceList->GetStringSelection();
		if (tStr.Length() > (sizeof(DatabaseDemoApp.params.ODBCSource)-1))
		{
			wxString errmsg;
			errmsg.sprintf("ODBC Data source name is longer than the data structure to hold it.\n'Cparameter.ODBCSource' must have a larger character array\nto handle a data source with this long of a name\n\nThe data source currently selected is %d characters long.",tStr.Length());
			wxMessageBox(errmsg.GetData(),"Internal program error...",wxOK | wxICON_EXCLAMATION);
			return FALSE;
		}
		strcpy(DatabaseDemoApp.params.ODBCSource, tStr.GetData());
	}
	else
		return FALSE;
	
	tStr = pParamUserNameTxt->GetValue();
	if (tStr.Length() > (sizeof(DatabaseDemoApp.params.UserName)-1))
	{
		wxString errmsg;
		errmsg.sprintf("User name is longer than the data structure to hold it.\n'Cparameter.UserName' must have a larger character array\nto handle a data source with this long of a name\n\nThe user name currently specified is %d characters long.",tStr.Length());
		wxMessageBox(errmsg.GetData(),"Internal program error...",wxOK | wxICON_EXCLAMATION);
		return FALSE;
	}
	strcpy(DatabaseDemoApp.params.UserName, tStr.GetData());

	tStr = pParamPasswordTxt->GetValue();
	if (tStr.Length() > (sizeof(DatabaseDemoApp.params.Password)-1))
	{
		wxString errmsg;
		errmsg.sprintf("Password is longer than the data structure to hold it.\n'Cparameter.Password' must have a larger character array\nto handle a data source with this long of a name\n\nThe password currently specified is %d characters long.",tStr.Length());
		wxMessageBox(errmsg.GetData(),"Internal program error...",wxOK | wxICON_EXCLAMATION);
		return FALSE;
	}
	strcpy(DatabaseDemoApp.params.Password,tStr.GetData());
	return TRUE;
}  // CparameterDlg::GetData()


Bool CparameterDlg::Save()
{
	Cparameters saveParams = DatabaseDemoApp.params;
	if (!GetData())
	{
		DatabaseDemoApp.params = saveParams;
		return FALSE;
	}

	FILE *paramFile;
	if ((paramFile = fopen(paramFilename, "wt")) == NULL)
	{
		wxString tStr;
		tStr.sprintf("Unable to write/overwrite '%s'.",paramFilename);
		wxMessageBox(tStr.GetData(),"File I/O Error...",wxOK | wxICON_EXCLAMATION);
		return FALSE;
	}

	fputs(DatabaseDemoApp.params.ODBCSource, paramFile);
	fputc('\n', paramFile);
	fputs(DatabaseDemoApp.params.UserName, paramFile);
	fputc('\n', paramFile);
	fputs(DatabaseDemoApp.params.Password, paramFile);
	fputc('\n', paramFile);
	fclose(paramFile);

	return TRUE;
}  // CparameterDlg::Save()


void CparameterDlg::FillDataSourceList()
{
	char Dsn[SQL_MAX_DSN_LENGTH + 1];
	char DsDesc[255];
	wxStringList strList;

	while(GetDataSource(DbConnectInf.Henv, Dsn, SQL_MAX_DSN_LENGTH+1, DsDesc, 255))
		strList.Add(Dsn);

	strList.Sort();
	strList.Add("");
	char **p = strList.ListToArray();

	for (int i = 0; strlen(p[i]); i++)
		pParamODBCSourceList->Append(p[i]);
}  // CparameterDlg::CparameterDlg::FillDataSourceList()


// CqueryDlg() constructor
CqueryDlg::CqueryDlg(wxWindow *parent, wxDB *pDb, char *tblName[], char *pWhereArg) : wxDialogBox (parent, "Query", 1, -1, -1, 480, 360)
{
	wxBeginBusyCursor();

	colInf = 0;
	dbTable = 0;
	masterTableName = tblName[0];
	widgetPtrsSet = FALSE;
	pDB = pDb;

	// Initialize the WHERE clause from the string passed in
	pWhere = pWhereArg;											// Save a pointer to the output buffer
	if (strlen(pWhere) > DB_MAX_WHERE_CLAUSE_LEN)		// Check the length of the buffer passed in
	{
		wxString s;
		s.sprintf("Maximum where clause length exceeded.\nLength must be less than %d", DB_MAX_WHERE_CLAUSE_LEN+1);
		wxMessageBox(s.GetData(),"Error...",wxOK | wxICON_EXCLAMATION);
		Close();
		return;
	}

	// Build the dialog
	SetLabelPosition(wxVERTICAL);

	wxFont *ButtonFont = new wxFont(12,wxSWISS,wxNORMAL,wxBOLD);
	wxFont *TextFont   = new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);

	SetButtonFont(ButtonFont);
	SetLabelFont(TextFont);
	SetLabelPosition(wxVERTICAL);

	pQueryCol1Msg			= new wxMessage(this, "Column 1:", 10, 10, 69, 16, 0, "QueryCol1Msg");
	pQueryCol1Choice		= new wxChoice(this, NULL, "", 10, 27, 250, 27, 0, 0, 0, "QueryCol1Choice");

	pQueryNotMsg			= new wxMessage(this, "NOT", 268, 10, -1, -1, 0, "QueryNotMsg");
	pQueryNotCheck			= new wxCheckBox(this, NULL, "", 275, 37, 20, 20, 0, "QueryNotCheck");

	char *choice_strings[9];
	choice_strings[0] = "=";
	choice_strings[1] = "<";
	choice_strings[2] = ">";
	choice_strings[3] = "<=";
	choice_strings[4] = ">=";
	choice_strings[5] = "Begins";
	choice_strings[6] = "Contains";
	choice_strings[7] = "Like";
	choice_strings[8] = "Between";
	pQueryOperatorMsg		= new wxMessage(this, "Operator:", 305, 10, -1, -1, 0, "QueryOperatorMsg");
	pQueryOperatorChoice	= new wxChoice(this, NULL, "", 305, 27, 80, 27, 9, choice_strings, 0, "QueryOperatorChoice");
	
	pQueryCol2Msg			= new wxMessage(this, "Column 2:", 10, 65, 69, 16, 0, "QueryCol2Msg");
	pQueryCol2Choice		= new wxChoice(this, NULL, "", 10, 82, 250, 27, 0, 0, 0, "QueryCol2Choice");

	pQuerySqlWhereMsg		= new wxMessage(this, "SQL where clause:", 10, 141, -1, -1, 0, "QuerySqlWhereMsg");
	pQuerySqlWhereMtxt	= new wxMultiText(this, NULL, "", "", 10, 159, 377, 134, 0, "QuerySqlWhereMtxt");

	pQueryAddBtn			= new wxButton(this, NULL, "&Add",	406,  24, 56, 26, 0, "QueryAddBtn");
	pQueryAndBtn			= new wxButton(this, NULL, "A&nd",	406,  58, 56, 26, 0, "QueryAndBtn");
	pQueryOrBtn				= new wxButton(this, NULL, "&Or",	406,  92, 56, 26, 0, "QueryOrBtn");

	pQueryLParenBtn		= new wxButton(this, NULL, "(",	406, 126, 26, 26, 0, "QueryLParenBtn");
	pQueryRParenBtn		= new wxButton(this, NULL, ")",	436, 126, 26, 26, 0, "QueryRParenBtn");

	pQueryDoneBtn			= new wxButton(this, NULL, "&Done",	 406, 185, 56, 26, 0, "QueryDoneBtn");
	pQueryClearBtn			= new wxButton(this, NULL, "C&lear", 406, 218, 56, 26, 0, "QueryClearBtn");
	pQueryCountBtn			= new wxButton(this, NULL, "&Count", 406, 252, 56, 26, 0, "QueryCountBtn");

	pQueryValue1Msg		= new wxMessage(this, "Value:", 277, 66, -1, -1, 0, "QueryValue1Msg");
	pQueryValue1Txt		= new wxText(this, NULL, "", "", 277, 83, 108, 25, 0, "QueryValue1Txt");

	pQueryValue2Msg		= new wxMessage(this, "AND", 238, 126, -1, -1, 0, "QueryValue2Msg");
	pQueryValue2Txt		= new wxText(this, NULL, "", "", 277, 120, 108, 25, 0, "QueryValue2Txt");

	pQueryHintGrp			= new wxGroupBox(this, "",  10, 291, 377, 40, 0, "QueryHintGrp");
	pQueryHintMsg			= new wxMessage(this, "", 16, 306, -1, -1, 0, "QueryHintMsg");

	widgetPtrsSet = TRUE;
	// Initialize the dialog
	wxString qualName;
	pQueryCol2Choice->Append("VALUE -->");
	colInf = pDB->GetColumns(tblName);
	for (int i = 0; colInf[i].colName && strlen(colInf[i].colName); i++)
	{
		// If there is more than one table being queried, qualify
		// the column names with the table name prefix.
		if (tblName[1] && strlen(tblName[1]))
		{
			qualName.sprintf("%s.%s", colInf[i].tableName, colInf[i].colName);
			pQueryCol1Choice->Append(qualName.GetData());
			pQueryCol2Choice->Append(qualName.GetData());
		}
		else  // Single table query, append just the column names
		{
			pQueryCol1Choice->Append(colInf[i].colName);
			pQueryCol2Choice->Append(colInf[i].colName);
		}
	}

	pQueryCol1Choice->SetSelection(0);
	pQueryCol2Choice->SetSelection(0);
	pQueryOperatorChoice->SetSelection(0);

	pQueryValue2Msg->Show(FALSE);
	pQueryValue2Txt->Show(FALSE);

	pQueryHintMsg->SetLabel(langQRY_EQ);

	pQuerySqlWhereMtxt->SetValue(pWhere);

	wxEndBusyCursor();

	// Display the dialog window
	SetModal(TRUE);
	Centre(wxBOTH);
	Show(TRUE);

}  // CqueryDlg() constructor


void CqueryDlg::OnCommand(wxWindow& win, wxCommandEvent& event)
{
	// Widget pointers won't be set when the dialog is constructed.
	// Control is passed through this function once for each widget on
	// a dialog as the dialog is constructed.
	if (!widgetPtrsSet)
		return;

	wxString widgetName = win.GetName();

	// Operator choice box
	if (widgetName == pQueryOperatorChoice->GetName())
	{
		// Set the help text
		switch((qryOp) pQueryOperatorChoice->GetSelection())
		{
		case qryOpEQ:
			pQueryHintMsg->SetLabel(langQRY_EQ);
			break;
		case qryOpLT:
			pQueryHintMsg->SetLabel(langQRY_LT);
			break;
		case qryOpGT:
			pQueryHintMsg->SetLabel(langQRY_GT);
			break;
		case qryOpLE:
			pQueryHintMsg->SetLabel(langQRY_LE);
			break;
		case qryOpGE:
			pQueryHintMsg->SetLabel(langQRY_GE);
			break;
		case qryOpBEGINS:
			pQueryHintMsg->SetLabel(langQRY_BEGINS);
			break;
		case qryOpCONTAINS:
			pQueryHintMsg->SetLabel(langQRY_CONTAINS);
			break;
		case qryOpLIKE:
			pQueryHintMsg->SetLabel(langQRY_LIKE);
			break;
		case qryOpBETWEEN:
			pQueryHintMsg->SetLabel(langQRY_BETWEEN);
			break;
		}

		// Hide the value2 widget
		pQueryValue2Msg->Show(FALSE);  // BETWEEN will show this widget
		pQueryValue2Txt->Show(FALSE);  // BETWEEN will show this widget

		// Disable the NOT operator for <, <=, >, >=
		switch((qryOp) pQueryOperatorChoice->GetSelection())
		{
		case qryOpLT:
		case qryOpGT:
		case qryOpLE:
		case qryOpGE:
			pQueryNotCheck->SetValue(0);
			pQueryNotCheck->Enable(FALSE);
			break;
		default:
			pQueryNotCheck->Enable(TRUE);
			break;
		}

		// Manipulate the dialog to handle the selected operator
		switch((qryOp) pQueryOperatorChoice->GetSelection())
		{
		case qryOpEQ:
		case qryOpLT:
		case qryOpGT:
		case qryOpLE:
		case qryOpGE:
			pQueryCol2Choice->Enable(TRUE);
			if (pQueryCol2Choice->GetSelection())	// Column name is highlighted
			{
				pQueryValue1Msg->Show(FALSE);
				pQueryValue1Txt->Show(FALSE);
			}
			else												// "Value" is highlighted
			{
				pQueryValue1Msg->Show(TRUE);
				pQueryValue1Txt->Show(TRUE);
				pQueryValue1Txt->SetFocus();
			}
			break;
		case qryOpBEGINS:
		case qryOpCONTAINS:
		case qryOpLIKE:
			pQueryCol2Choice->SetSelection(0);
			pQueryCol2Choice->Enable(FALSE);
			pQueryValue1Msg->Show(TRUE);
			pQueryValue1Txt->Show(TRUE);
			pQueryValue1Txt->SetFocus();
			break;
		case qryOpBETWEEN:
			pQueryCol2Choice->SetSelection(0);
			pQueryCol2Choice->Enable(FALSE);
			pQueryValue2Msg->Show(TRUE);
			pQueryValue2Txt->Show(TRUE);
			pQueryValue1Msg->Show(TRUE);
			pQueryValue1Txt->Show(TRUE);
			pQueryValue1Txt->SetFocus();
			break;
		}

		return;

	}  // Operator choice box

	// Column 2 choice
	if (widgetName == pQueryCol2Choice->GetName())
	{
		if (pQueryCol2Choice->GetSelection())	// Column name is highlighted
		{
			pQueryValue1Msg->Show(FALSE);
			pQueryValue1Txt->Show(FALSE);
		}
		else												// "Value" is highlighted
		{
			pQueryValue1Msg->Show(TRUE);
			pQueryValue1Txt->Show(TRUE);
			pQueryValue1Txt->SetFocus();
		}
		return;

	}  // Column 2 choice

	// Add button
	if (widgetName == pQueryAddBtn->GetName())
	{
		ProcessAddBtn();
		return;

	}  // Add button

	// And button
	if (widgetName == pQueryAndBtn->GetName())
	{
		AppendToWhere(" AND\n");
		return;

	}  // And button

	// Or button
	if (widgetName == pQueryOrBtn->GetName())
	{
		AppendToWhere(" OR\n");
		return;

	}  // Or button

	// Left Paren button
	if (widgetName == pQueryLParenBtn->GetName())
	{
		AppendToWhere("(");
		return;

	}  // Left Paren button

	// Right paren button
	if (widgetName == pQueryRParenBtn->GetName())
	{
		AppendToWhere(")");
		return;

	}  // Right Paren button

	// Done button
	if (widgetName == pQueryDoneBtn->GetName())
	{
		// Be sure the where clause will not overflow the output buffer
		if (strlen(pQuerySqlWhereMtxt->GetValue()) > DB_MAX_WHERE_CLAUSE_LEN)
		{
			wxString s;
			s.sprintf("Maximum where clause length exceeded.\nLength must be less than %d", DB_MAX_WHERE_CLAUSE_LEN+1);
			wxMessageBox(s.GetData(),"Error...",wxOK | wxICON_EXCLAMATION);
			return;
		}
		// Validate the where clause for things such as matching parens
		if (!ValidateWhereClause())
			return;
		// Copy the where clause to the output buffer and exit
		strcpy(pWhere, pQuerySqlWhereMtxt->GetValue());
		Close();
		return;

	}  // Done button

	// Clear button
	if (widgetName == pQueryClearBtn->GetName())
	{
		Bool Ok = (wxMessageBox("Are you sure you wish to clear the Query?","Confirm",wxYES_NO|wxICON_QUESTION) == wxYES);

		if (Ok)
			pQuerySqlWhereMtxt->SetValue("");
		return;

	}  // Clear button

	// Count button
	if (widgetName == pQueryCountBtn->GetName())
	{
		wxBeginBusyCursor();
		ProcessCountBtn();
		wxEndBusyCursor();
		return;

	}  // Count button

}  // CqueryDlg::OnCommand


Bool CqueryDlg::OnClose()
{
	// Clean up
	if (colInf)
	{
		delete [] colInf;
		colInf = 0;
	}

	if (dbTable)
	{
		delete dbTable;
		dbTable = 0;
	}

	GetParent()->SetFocus();
	wxEndBusyCursor();
	return TRUE;

}  // CqueryDlg::OnClose()

/*
Bool CqueryDlg::SetWidgetPtrs()
{
	Bool abort = FALSE;

	abort = abort || !(pQueryCol1Choice = (wxChoice *)GetWidgetPtr("QueryCol1Choice",this));
	abort = abort || !(pQueryNotCheck = (wxCheckBox *)GetWidgetPtr("QueryNotCheck",this));
	abort = abort || !(pQueryOperatorChoice = (wxChoice *)GetWidgetPtr("QueryOperatorChoice",this));
	abort = abort || !(pQueryCol2Choice = (wxChoice *)GetWidgetPtr("QueryCol2Choice",this));
	abort = abort || !(pQueryValue1Txt = (wxText *)GetWidgetPtr("QueryValue1Txt",this));
	abort = abort || !(pQueryValue2Txt = (wxText *)GetWidgetPtr("QueryValue2Txt",this));
	abort = abort || !(pQuerySqlWhereMtxt = (wxMultiText *)GetWidgetPtr("QuerySqlWhereMtxt",this));
	abort = abort || !(pQueryAddBtn = (wxButton *)GetWidgetPtr("QueryAddBtn",this));
	abort = abort || !(pQueryAndBtn = (wxButton *)GetWidgetPtr("QueryAndBtn",this));
	abort = abort || !(pQueryOrBtn = (wxButton *)GetWidgetPtr("QueryOrBtn",this));
	abort = abort || !(pQueryLParenBtn = (wxButton *)GetWidgetPtr("QueryLParenBtn",this));
	abort = abort || !(pQueryRParenBtn = (wxButton *)GetWidgetPtr("QueryRParenBtn",this));
	abort = abort || !(pQueryDoneBtn = (wxButton *)GetWidgetPtr("QueryDoneBtn",this));
	abort = abort || !(pQueryClearBtn = (wxButton *)GetWidgetPtr("QueryClearBtn",this));
	abort = abort || !(pQueryCountBtn = (wxButton *)GetWidgetPtr("QueryCountBtn",this));
	abort = abort || !(pQueryHelpBtn = (wxButton *)GetWidgetPtr("QueryHelpBtn",this));
	abort = abort || !(pQueryHintMsg = (wxMessage *)GetWidgetPtr("QueryHintMsg",this));

	pFocusTxt = NULL;

	return(widgetPtrsSet = !abort);

}  // CqueryDlg::SetWidgetPtrs
*/

void CqueryDlg::AppendToWhere(char *s)
{
		wxString whereStr = pQuerySqlWhereMtxt->GetValue();
		whereStr += s;
		pQuerySqlWhereMtxt->SetValue(whereStr.GetData());

}  // CqueryDlg::AppendToWhere()


void CqueryDlg::ProcessAddBtn()
{
	qryOp oper = (qryOp) pQueryOperatorChoice->GetSelection();

	// Verify that eveything is filled in correctly
	if (pQueryCol2Choice->GetSelection() == 0)			// "Value" is selected
	{
		// Verify that value 1 is filled in
		if (strlen(pQueryValue1Txt->GetValue()) == 0)
		{
			wxBell();
			pQueryValue1Txt->SetFocus();
			return;
		}
		// For the BETWEEN operator, value 2 must be filled in as well
		if (oper == qryOpBETWEEN &&
			 strlen(pQueryValue2Txt->GetValue()) == 0)
		{
			wxBell();
			pQueryValue2Txt->SetFocus();
			return;
		}
	}

	// Build the expression and append it to the where clause window
	wxString s = pQueryCol1Choice->GetStringSelection();
	
	if (pQueryNotCheck->GetValue() && (oper != qryOpEQ))
		s += " NOT";
	
	switch(oper)
	{
	case qryOpEQ:
		if (pQueryNotCheck->GetValue())	// NOT box is checked
			s += " <>";
		else
			s += " =";
		break;
	case qryOpLT:
		s += " <";
		break;
	case qryOpGT:
		s += " >";
		break;
	case qryOpLE:
		s += " <=";
		break;
	case qryOpGE:
		s += " >=";
		break;
	case qryOpBEGINS:
	case qryOpCONTAINS:
	case qryOpLIKE:
		s += " LIKE";
		break;
	case qryOpBETWEEN:
		s += " BETWEEN";
		break;
	}

	s += " ";

	int col1Idx = pQueryCol1Choice->GetSelection();

	Bool quote = FALSE;
	if (colInf[col1Idx].sqlDataType == SQL_VARCHAR	|| 
		oper == qryOpBEGINS									||
		oper == qryOpCONTAINS								||
		oper == qryOpLIKE)
		quote = TRUE;

	if (pQueryCol2Choice->GetSelection())	// Column name
		s += pQueryCol2Choice->GetStringSelection();
	else  // Column 2 is a "value"
	{
		if (quote)
			s += "'";
		if (oper == qryOpCONTAINS)
			s += "%";
		s += pQueryValue1Txt->GetValue();
		if (oper == qryOpCONTAINS || oper == qryOpBEGINS)
			s += "%";
		if (quote)
			s += "'";
	}

	if (oper == qryOpBETWEEN)
	{
		s += " AND ";
		if (quote)
			s += "'";
		s += pQueryValue2Txt->GetValue();
		if (quote)
			s += "'";
	}

	AppendToWhere(s.GetData());

}  // CqueryDlg::ProcessAddBtn()


void CqueryDlg::ProcessCountBtn()
{
	if (!ValidateWhereClause())
		return;

	if (dbTable == 0)  // wxTable object needs to be created and opened
	{
		if (!(dbTable = new wxTable(pDB, masterTableName, 0)))
		{
			wxMessageBox("Memory allocation failed creating a wxTable object.","Error...",wxOK | wxICON_EXCLAMATION);
			return;
		}
		if (!dbTable->Open())
		{
			wxString tStr;
			tStr  = "ODBC error during Open()\n\n";
			tStr += GetExtendedDBErrorMsg(__FILE__,__LINE__);
			wxMessageBox(tStr.GetData(),"ODBC Error...",wxOK | wxICON_EXCLAMATION);
			return;
		}
	}

	// Count() with WHERE clause
	dbTable->where = pQuerySqlWhereMtxt->GetValue();
	ULONG whereCnt = dbTable->Count();

	// Count() of all records in the table
	dbTable->where = 0;
	ULONG totalCnt = dbTable->Count();

	if (whereCnt > 0 || totalCnt == 0)
	{
		wxString tStr;
		tStr.sprintf("%lu of %lu records match the query criteria.",whereCnt,totalCnt);
		wxMessageBox(tStr.GetData(),"Notice...",wxOK | wxICON_INFORMATION);
	}
	else
	{
		wxString tStr;
		tStr.sprintf("%lu of %lu records match the query criteria.\n\nEither the criteria entered produced a result set\nwith no records, or there was a syntactical error\nin the clause you entered.\n\nPress the details button to see if any database errors were reported.",whereCnt,totalCnt);
		wxMessageBox(tStr.GetData(),"Notice...",wxOK | wxICON_INFORMATION);
	}

	// After a wxMessageBox, the focus does not necessarily return to the
	// window which was the focus when the message box popped up, so return
	// focus to the Query dialog for certain
	SetFocus();

}  // CqueryDlg::ProcessCountBtn()


Bool CqueryDlg::ValidateWhereClause()
{
	wxString where = pQuerySqlWhereMtxt->GetValue();

	if (where.Freq('(') != where.Freq(')'))
	{
		wxMessageBox("There are mismatched parenthesis in the constructed where clause","Error...",wxOK | wxICON_EXCLAMATION);
		return(FALSE);
	}
	// After a wxMessageBox, the focus does not necessarily return to the
	// window which was the focus when the message box popped up, so return
	// focus to the Query dialog for certain
	SetFocus();

	return(TRUE);

}  // CqueryDlg::ValidateWhereClause()
