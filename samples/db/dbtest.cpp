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
 *  SYNOPSIS START

    This sample program demonstrates the cross-platform ODBC database classes
    donated by the development team at Remstar International.

    The table this sample is based on is developer contact table, and shows
    some of the simple uses of the database classes wxDb and wxDbTable.

 *  SYNOPSIS END
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

#ifdef __WXGTK__
#include "db.xpm"
#endif

#include <stdio.h>                  /* Included strictly for reading the text file with the database parameters */

//#include <wx/db.h>                  /* Required in the file which will get the data source connection */
//#include <wx/dbtable.h>             /* Has the wxDbTable object from which all data objects will inherit their data table functionality */

//extern wxDbList WXDLLEXPORT *PtrBegDbList;    /* from db.cpp, used in getting back error results from db connections */

#include "dbtest.h"                 /* Header file for this demonstration program */
#include "listdb.h"                 /* Code to support the "Lookup" button on the editor dialog */

IMPLEMENT_APP(DatabaseDemoApp)

extern wxChar ListDB_Selection[];   /* Used to return the first column value for the selected line from the listDB routines */
extern wxChar ListDB_Selection2[];  /* Used to return the second column value for the selected line from the listDB routines */

const char *GetExtendedDBErrorMsg(wxDb *pDb, char *ErrFile, int ErrLine)
{
    static wxString msg;
    msg = wxT("");

    wxString tStr;

    if (ErrFile || ErrLine)
    {
        msg += wxT("File: ");
        msg += ErrFile;
        msg += wxT("   Line: ");
        tStr.Printf(wxT("%d"),ErrLine);
        msg += tStr.c_str();
        msg += wxT("\n");
    }

    msg.Append (wxT("\nODBC errors:\n"));
    msg += wxT("\n");
    
    // Display errors for this connection
    int i;
    for (i = 0; i < DB_MAX_ERROR_HISTORY; i++)
    {
        if (pDb->errorList[i])
        {
            msg.Append(pDb->errorList[i]);
            if (wxStrcmp(pDb->errorList[i],wxT("")) != 0)
                msg.Append(wxT("\n"));
            // Clear the errmsg buffer so the next error will not
            // end up showing the previous error that have occurred
            wxStrcpy(pDb->errorList[i],wxT(""));
        }
    }
    msg += wxT("\n");

    return msg.c_str();
}  // GetExtendedDBErrorMsg


bool DataTypeSupported(wxDb *pDb, SWORD datatype)
{
    wxDbSqlTypeInfo sqlTypeInfo;

    bool breakpoint = FALSE;

    if (pDb->GetDataTypeInfo(datatype, sqlTypeInfo))
        breakpoint = TRUE;

    return breakpoint;

}  // GetDataTypesSupported();



void CheckSupportForAllDataTypes(wxDb *pDb)
{
    bool supported;
#ifdef SQL_C_BINARY
    supported = DataTypeSupported(pDb,SQL_C_BINARY);
#endif
#ifdef SQL_C_BIT
    supported = DataTypeSupported(pDb,SQL_C_BIT);
#endif
#ifdef SQL_C_BOOKMARK
    supported = DataTypeSupported(pDb,SQL_C_BOOKMARK);
#endif
#ifdef SQL_C_CHAR
    supported = DataTypeSupported(pDb,SQL_C_CHAR);
#endif
#ifdef SQL_C_DATE
    supported = DataTypeSupported(pDb,SQL_C_DATE);
#endif
#ifdef SQL_C_DEFAULT
    supported = DataTypeSupported(pDb,SQL_C_DEFAULT);
#endif
#ifdef SQL_C_DOUBLE
    supported = DataTypeSupported(pDb,SQL_C_DOUBLE);
#endif
#ifdef SQL_C_FLOAT
    supported = DataTypeSupported(pDb,SQL_C_FLOAT);
#endif
#ifdef SQL_C_GUID
    supported = DataTypeSupported(pDb,SQL_C_GUID);
#endif
#ifdef SQL_C_INTERVAL_DAY
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_DAY);
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_HOUR
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_HOUR);
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_MINUTE
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_MINUTE);
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_SECOND
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_SECOND);
#endif
#ifdef SQL_C_INTERVAL_HOUR
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR);
#endif
#ifdef SQL_C_INTERVAL_HOUR_TO_MINUTE
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR_TO_MINUTE);
#endif
#ifdef SQL_C_INTERVAL_HOUR_TO_SECOND
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR_TO_SECOND);
#endif
#ifdef SQL_C_INTERVAL_MINUTE
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_MINUTE);
#endif
#ifdef SQL_C_INTERVAL_MINUTE_TO_SECOND
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_MINUTE_TO_SECOND);
#endif
#ifdef SQL_C_INTERVAL_MONTH
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_MONTH);
#endif
#ifdef SQL_C_INTERVAL_SECOND
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_SECOND);
#endif
#ifdef SQL_C_INTERVAL_YEAR
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_YEAR);
#endif
#ifdef SQL_C_INTERVAL_YEAR_TO_MONTH
    supported = DataTypeSupported(pDb,SQL_C_INTERVAL_YEAR_TO_MONTH);
#endif
#ifdef SQL_C_LONG
    supported = DataTypeSupported(pDb,SQL_C_LONG);
#endif
#ifdef SQL_C_NUMERIC
    supported = DataTypeSupported(pDb,SQL_C_NUMERIC);
#endif
#ifdef SQL_C_SBIGINT
    supported = DataTypeSupported(pDb,SQL_C_SBIGINT);
#endif
#ifdef SQL_C_SHORT
    supported = DataTypeSupported(pDb,SQL_C_SHORT);
#endif
#ifdef SQL_C_SLONG
    supported = DataTypeSupported(pDb,SQL_C_SLONG);
#endif
#ifdef SQL_C_SSHORT
    supported = DataTypeSupported(pDb,SQL_C_SSHORT);
#endif
#ifdef SQL_C_STINYINT
    supported = DataTypeSupported(pDb,SQL_C_STINYINT);
#endif
#ifdef SQL_C_TIME
    supported = DataTypeSupported(pDb,SQL_C_TIME);
#endif
#ifdef SQL_C_TIMESTAMP
    supported = DataTypeSupported(pDb,SQL_C_TIMESTAMP);
#endif
#ifdef SQL_C_TINYINT
    supported = DataTypeSupported(pDb,SQL_C_TINYINT);
#endif
#ifdef SQL_C_TYPE_DATE
    supported = DataTypeSupported(pDb,SQL_C_TYPE_DATE);
#endif
#ifdef SQL_C_TYPE_TIME
    supported = DataTypeSupported(pDb,SQL_C_TYPE_TIME);
#endif
#ifdef SQL_C_TYPE_TIMESTAMP
    supported = DataTypeSupported(pDb,SQL_C_TYPE_TIMESTAMP);
#endif
#ifdef SQL_C_UBIGINT
    supported = DataTypeSupported(pDb,SQL_C_UBIGINT);
#endif
#ifdef SQL_C_ULONG
    supported = DataTypeSupported(pDb,SQL_C_ULONG);
#endif
#ifdef SQL_C_USHORT
    supported = DataTypeSupported(pDb,SQL_C_USHORT);
#endif
#ifdef SQL_C_UTINYINT
    supported = DataTypeSupported(pDb,SQL_C_UTINYINT);
#endif
#ifdef SQL_C_VARBOOKMARK
    supported = DataTypeSupported(pDb,SQL_C_VARBOOKMARK);
#endif

// Extended SQL types
#ifdef SQL_DATE
    supported = DataTypeSupported(pDb,SQL_DATE);
#endif
#ifdef SQL_INTERVAL
    supported = DataTypeSupported(pDb,SQL_INTERVAL);
#endif
#ifdef SQL_TIME
    supported = DataTypeSupported(pDb,SQL_TIME);
#endif
#ifdef SQL_TIMESTAMP
    supported = DataTypeSupported(pDb,SQL_TIMESTAMP);
#endif
#ifdef SQL_LONGVARCHAR
    supported = DataTypeSupported(pDb,SQL_LONGVARCHAR);
#endif
#ifdef SQL_BINARY
    supported = DataTypeSupported(pDb,SQL_BINARY);
#endif
#ifdef SQL_VARBINARY
    supported = DataTypeSupported(pDb,SQL_VARBINARY);
#endif
#ifdef SQL_LONGVARBINARY
    supported = DataTypeSupported(pDb,SQL_LONGVARBINARY);
#endif
#ifdef SQL_BIGINT
    supported = DataTypeSupported(pDb,SQL_BIGINT);
#endif
#ifdef SQL_TINYINT
    supported = DataTypeSupported(pDb,SQL_TINYINT);
#endif
#ifdef SQL_BIT
    supported = DataTypeSupported(pDb,SQL_BIT);
#endif
#ifdef SQL_GUID
    supported = DataTypeSupported(pDb,SQL_GUID);
#endif

#ifdef SQL_CHAR
    supported = DataTypeSupported(pDb,SQL_CHAR);
#endif
#ifdef SQL_INTEGER
    supported = DataTypeSupported(pDb,SQL_INTEGER);
#endif
#ifdef SQL_SMALLINT
    supported = DataTypeSupported(pDb,SQL_SMALLINT);
#endif
#ifdef SQL_REAL
    supported = DataTypeSupported(pDb,SQL_REAL);
#endif
#ifdef SQL_DOUBLE
    supported = DataTypeSupported(pDb,SQL_DOUBLE);
#endif
#ifdef SQL_NUMERIC
    supported = DataTypeSupported(pDb,SQL_NUMERIC);
#endif
#ifdef SQL_DATE
    supported = DataTypeSupported(pDb,SQL_DATE);
#endif
#ifdef SQL_TIME
    supported = DataTypeSupported(pDb,SQL_TIME);
#endif
#ifdef SQL_TIMESTAMP
    supported = DataTypeSupported(pDb,SQL_TIMESTAMP);
#endif
#ifdef SQL_VARCHAR
    supported = DataTypeSupported(pDb,SQL_VARCHAR);
#endif


// UNICODE
#ifdef SQL_C_TCHAR
    supported = DataTypeSupported(pDb,SQL_C_TCHAR);
#endif
}  // CheckSupportForAllDataTypes()


bool DatabaseDemoApp::OnInit()
{
    DbConnectInf = NULL;

    // Create the main frame window
    DemoFrame = new DatabaseDemoFrame(NULL, wxT("wxWindows Database Demo"), wxPoint(50, 50), wxSize(537, 480));

    // Give it an icon
    DemoFrame->SetIcon(wxICON(db));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(FILE_CREATE, wxT("&Create CONTACT table"));
    file_menu->Append(FILE_RECREATE_TABLE, wxT("&Recreate CONTACT table"));
    file_menu->Append(FILE_RECREATE_INDEXES, wxT("&Recreate CONTACT indexes"));
    file_menu->Append(FILE_EXIT, wxT("E&xit"));

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(EDIT_PARAMETERS, wxT("&Parameters..."));

    wxMenu *about_menu = new wxMenu;
    about_menu->Append(ABOUT_DEMO, wxT("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(edit_menu, wxT("&Edit"));
    menu_bar->Append(about_menu, wxT("&About"));
    DemoFrame->SetMenuBar(menu_bar);

    params.ODBCSource[0] = 0;
    params.UserName[0]   = 0;
    params.Password[0]   = 0;
    params.DirPath[0]    = 0;

    // Show the frame
    DemoFrame->Show(TRUE);

    // Passing NULL for the SQL environment handle causes
    // the wxDbConnectInf constructor to obtain a handle
    // for you.
    //
    // WARNING: Be certain that you do not free this handle
    //          directly with SQLFreeEnv().  Use either the
    //          method ::FreeHenv() or delete the DbConnectInf.
    DbConnectInf = new wxDbConnectInf(NULL, params.ODBCSource, params.UserName, 
                                      params.Password, params.DirPath);

    if (!DbConnectInf || !DbConnectInf->GetHenv())
    {
        wxMessageBox(wxT("Unable to define data source connection info."), wxT("DB CONNECTION ERROR..."),wxOK | wxICON_EXCLAMATION);
        delete DbConnectInf;
    }

    if (!ReadParamFile(params))
        DemoFrame->BuildParameterDialog(NULL);

    if (!wxStrlen(params.ODBCSource))
    {
        delete DbConnectInf;
        return(FALSE);
    }

    DbConnectInf->SetDsn(params.ODBCSource);
    DbConnectInf->SetUserID(params.UserName);
    DbConnectInf->SetPassword(params.Password);
    DbConnectInf->SetDefaultDir(params.DirPath);

    READONLY_DB = wxDbGetConnection(DbConnectInf);
    if (READONLY_DB == 0)
    {
        wxMessageBox(wxT("Unable to connect to the data source.\n\nCheck the name of your data source to verify it has been correctly entered/spelled.\n\nWith some databases, the user name and password must\nbe created with full rights to the CONTACT table prior to making a connection\n(using tools provided by the database manufacturer)"), wxT("DB CONNECTION ERROR..."),wxOK | wxICON_EXCLAMATION);
        DemoFrame->BuildParameterDialog(NULL);
        delete DbConnectInf;
        wxMessageBox(wxT("Now exiting program.\n\nRestart program to try any new settings."),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        return(FALSE);
    }

    DemoFrame->BuildEditorDialog();

    // Show the frame
    DemoFrame->Refresh();

    return TRUE;
}  // DatabaseDemoApp::OnInit()


bool DatabaseDemoApp::ReadParamFile(Cparameters &params)
{
    FILE *paramFile;
    if ((paramFile = fopen(PARAM_FILENAME, wxT("r"))) == NULL)
    {
        wxString tStr;
        tStr.Printf(wxT("Unable to open the parameter file '%s' for reading.\n\nYou must specify the data source, user name, and\npassword that will be used and save those settings."),PARAM_FILENAME);
        wxMessageBox(tStr,wxT("File I/O Error..."),wxOK | wxICON_EXCLAMATION);

        return FALSE;
    }

    wxChar buffer[1000+1];
    fgets(buffer, sizeof(params.ODBCSource), paramFile);
    buffer[wxStrlen(buffer)-1] = wxT('\0');
    wxStrcpy(params.ODBCSource,buffer);

    fgets(buffer, sizeof(params.UserName), paramFile);
    buffer[wxStrlen(buffer)-1] = wxT('\0');
    wxStrcpy(params.UserName,buffer);

    fgets(buffer, sizeof(params.Password), paramFile);
    buffer[wxStrlen(buffer)-1] = wxT('\0');
    wxStrcpy(params.Password,buffer);

    fgets(buffer, sizeof(params.DirPath), paramFile);
    buffer[wxStrlen(buffer)-1] = wxT('\0');
    wxStrcpy(params.DirPath,buffer);

    fclose(paramFile);

    return TRUE;
}  // DatabaseDemoApp::ReadParamFile()


bool DatabaseDemoApp::WriteParamFile(Cparameters &params)
{
    FILE *paramFile;
    if ((paramFile = fopen(PARAM_FILENAME, wxT("wt"))) == NULL)
    {
        wxString tStr;
        tStr.Printf(wxT("Unable to write/overwrite '%s'."),PARAM_FILENAME);
        wxMessageBox(tStr,wxT("File I/O Error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    fputs(wxGetApp().params.ODBCSource, paramFile);
    fputc(wxT('\n'), paramFile);
    fputs(wxGetApp().params.UserName, paramFile);
    fputc(wxT('\n'), paramFile);
    fputs(wxGetApp().params.Password, paramFile);
    fputc(wxT('\n'), paramFile);
    fputs(wxGetApp().params.DirPath, paramFile);
    fputc(wxT('\n'), paramFile);
    fclose(paramFile);

    return TRUE;
}  // DatabaseDemoApp::WriteParamFile()


void DatabaseDemoApp::CreateDataTable(bool recreate)
{
    bool Ok = TRUE;
    if (recreate)
       Ok = (wxMessageBox(wxT("Any data currently residing in the table will be erased.\n\nAre you sure?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

    if (!Ok)
        return;

    wxBeginBusyCursor();

    bool success = TRUE;

    // Use a temporary instance of a new Ccontact table object
    // for creating the table within the datasource.
    Ccontact *Contact = new Ccontact();

    if (!Contact)
    {
        wxEndBusyCursor();
        wxMessageBox(wxT("Error allocating memory for 'Ccontact'object.\n\nTable was not created."),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        return;
    }

    if (!Contact->CreateTable(recreate))
    {
        wxEndBusyCursor();
        wxString tStr;
        tStr  = wxT("Error creating CONTACTS table.\nTable was not created.\n\n");
        tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
        wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        success = FALSE;
    }
    else
    {
        if (!Contact->CreateIndexes())
        {
            wxEndBusyCursor();
            wxString tStr;
            tStr  = wxT("Error creating CONTACTS indexes.\nIndexes will be unavailable.\n\n");
            tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
            success = FALSE;
        }
    }
    while (wxIsBusy())
        wxEndBusyCursor();

    delete Contact;
    Contact = NULL;

    if (success)
        wxMessageBox(wxT("Table and index(es) were successfully created."),wxT("Notice..."),wxOK | wxICON_INFORMATION);
}  // DatabaseDemoApp::CreateDataTable()


BEGIN_EVENT_TABLE(DatabaseDemoFrame, wxFrame)
    EVT_MENU(FILE_CREATE, DatabaseDemoFrame::OnCreate)
    EVT_MENU(FILE_RECREATE_TABLE, DatabaseDemoFrame::OnRecreateTable)
    EVT_MENU(FILE_RECREATE_INDEXES, DatabaseDemoFrame::OnRecreateIndexes)
    EVT_MENU(FILE_EXIT, DatabaseDemoFrame::OnExit)
    EVT_MENU(EDIT_PARAMETERS, DatabaseDemoFrame::OnEditParameters)
    EVT_MENU(ABOUT_DEMO, DatabaseDemoFrame::OnAbout)
    EVT_CLOSE(DatabaseDemoFrame::OnCloseWindow)
END_EVENT_TABLE()


// DatabaseDemoFrame constructor
DatabaseDemoFrame::DatabaseDemoFrame(wxFrame *frame, const wxString& title,
                                     const wxPoint& pos, const wxSize& size):
                                        wxFrame(frame, -1, title, pos, size)
{
    // Put any code in necessary for initializing the main frame here
    pEditorDlg = NULL;
    pParamDlg  = NULL;
}  // DatabaseDemoFrame constructor


void DatabaseDemoFrame::OnCreate(wxCommandEvent& event)
{
    wxGetApp().CreateDataTable(FALSE);
}  // DatabaseDemoFrame::OnCreate()


void DatabaseDemoFrame::OnRecreateTable(wxCommandEvent& event)
{
    wxGetApp().CreateDataTable(TRUE);
}  // DatabaseDemoFrame::OnRecreate()


void DatabaseDemoFrame::OnRecreateIndexes(wxCommandEvent& event)
{
    // Using a new connection to the database so as not to disturb
    // the current cursors on the table in use in the editor dialog
    Ccontact *Contact = new Ccontact();

    if (!Contact)
    {
        wxEndBusyCursor();
        wxMessageBox(wxT("Error allocating memory for 'Ccontact'object.\n\nTable could not be opened."),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        return;
    }

    if (!Contact->CreateIndexes())
    {
       wxEndBusyCursor();
       wxString tStr;
       tStr  = wxT("Error creating CONTACTS indexes.\nNew indexes will be unavailable.\n\n");
       tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
       wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
    }

    delete Contact;
    Contact = NULL;
}  // DatabaseDemoFrame::OnRecreateIndexes()

void DatabaseDemoFrame::OnExit(wxCommandEvent& event)
{
    Close();
}  // DatabaseDemoFrame::OnExit()


void DatabaseDemoFrame::OnEditParameters(wxCommandEvent& event)
{
    if ((pEditorDlg->mode != mCreate) && (pEditorDlg->mode != mEdit))
        BuildParameterDialog(this);
    else
        wxMessageBox(wxT("Cannot change database parameters while creating or editing a record"),wxT("Notice..."),wxOK | wxICON_INFORMATION);
}  // DatabaseDemoFrame::OnEditParameters()


void DatabaseDemoFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(wxT("wxWindows sample program for database classes\n\nContributed on 27 July 1998"),wxT("About..."),wxOK | wxICON_INFORMATION);
}  // DatabaseDemoFrame::OnAbout()


// Put any additional checking necessary to make certain it is alright
// to close the program here that is not done elsewhere
void DatabaseDemoFrame::OnCloseWindow(wxCloseEvent& event)
{
    // Clean up time
    if (pEditorDlg && pEditorDlg->Close())
        pEditorDlg = NULL;
    else
    {
        if (pEditorDlg)
        {
            event.Veto();
            return;
        }
    }

    // This function will close all the connections to the database that have been
    // previously cached.
    wxDbCloseConnections();

    // Deletion of the wxDbConnectInf instance must be the LAST thing done that
    // has anything to do with the database.  Deleting this before disconnecting,
    // freeing/closing connections, etc will result in a crash!
    delete wxGetApp().DbConnectInf;
    wxGetApp().DbConnectInf = NULL;

    this->Destroy();

}  // DatabaseDemoFrame::OnCloseWindow()


void DatabaseDemoFrame::BuildEditorDialog()
{
    pEditorDlg = NULL;
    pEditorDlg = new CeditorDlg(this);
    if (pEditorDlg)
    {
        pEditorDlg->Initialize();
        if (!pEditorDlg->initialized)
        {
            pEditorDlg->Close();
            pEditorDlg = NULL;
            wxMessageBox(wxT("Unable to initialize the editor dialog for some reason"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
            Close();
        }
    } 
    else
    {
        wxMessageBox(wxT("Unable to create the editor dialog for some reason"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        Close();
    }
}  // DatabaseDemoFrame::BuildEditorDialog()


void DatabaseDemoFrame::BuildParameterDialog(wxWindow *parent)
{
    pParamDlg = new CparameterDlg(parent);

    if (!pParamDlg)
        wxMessageBox(wxT("Unable to create the parameter dialog for some reason"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
}  // DatabaseDemoFrame::BuildParameterDialog()


/*
 * Constructor note: If no wxDb object is passed in, a new connection to the database
 *     is created for this instance of Ccontact.  This can be a slow process depending
 *     on the database engine being used, and some database engines have a limit on the
 *     number of connections (either hard limits, or license restricted) so care should 
 *     be used to use as few connections as is necessary.  
 *
 * IMPORTANT: Objects which share a wxDb pointer are ALL acted upon whenever a member 
 *     function of pDb is called (i.e. CommitTrans() or RollbackTrans(), so if modifying 
 *     or creating a table objects which use the same pDb, know that all the objects
 *     will be committed or rolled back when any of the objects has this function call made.
 */
Ccontact::Ccontact (wxDb *pwxDb) : wxDbTable(pwxDb ? pwxDb : wxDbGetConnection(wxGetApp().DbConnectInf),
                                             CONTACT_TABLE_NAME, CONTACT_NO_COLS, wxT(""),
                                             !wxDB_QUERY_ONLY, wxGetApp().DbConnectInf->GetDefaultDir())
{
    // This is used to represent whether the database connection should be released
    // when this instance of the object is deleted.  If using the same connection
    // for multiple instance of database objects, then the connection should only be 
    // released when the last database instance using the connection is deleted
    freeDbConn = !pwxDb;
    
    if (GetDb())
        GetDb()->SetSqlLogging(sqlLogON);

    SetupColumns();

}  // Ccontact Constructor


void Ccontact::Initialize()
{
    Name[0]            = 0;
    Addr1[0]           = 0;
    Addr2[0]           = 0;
    City[0]            = 0;
    State[0]           = 0;
    PostalCode[0]      = 0;
    Country[0]         = 0;
    JoinDate.year      = 1980;
    JoinDate.month     = 1;
    JoinDate.day       = 1;
    JoinDate.hour      = 0;
    JoinDate.minute    = 0;
    JoinDate.second    = 0;
    JoinDate.fraction  = 0;
    NativeLanguage     = langENGLISH;
    IsDeveloper        = FALSE;
    Contributions      = 0;
    LinesOfCode        = 0L;
}  // Ccontact::Initialize


Ccontact::~Ccontact()
{
    if (freeDbConn)
    {
        if (!wxDbFreeConnection(GetDb()))
        {
            wxString tStr;
            tStr  = wxT("Unable to Free the Ccontact data table handle\n\n");
            tStr += GetExtendedDBErrorMsg(GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        }
    }
}  // Ccontract destructor


/*
 * Handles setting up all the connections for the interface from the wxDbTable
 * functions to interface to the data structure used to store records in 
 * memory, and for all the column definitions that define the table structure
 */
void Ccontact::SetupColumns()
{
    // NOTE: Columns now are 8 character names, as that is all dBase can support.  Longer
    //       names can be used for other database engines
    SetColDefs ( 0,wxT("NAME"),       DB_DATA_TYPE_VARCHAR,     Name,           SQL_C_CHAR,                 sizeof(Name),           TRUE, TRUE);  // Primary index
    SetColDefs ( 1,wxT("ADDRESS1"),   DB_DATA_TYPE_VARCHAR,     Addr1,          SQL_C_CHAR,                 sizeof(Addr1),          FALSE,TRUE);
    SetColDefs ( 2,wxT("ADDRESS2"),   DB_DATA_TYPE_VARCHAR,     Addr2,          SQL_C_CHAR,                 sizeof(Addr2),          FALSE,TRUE);
    SetColDefs ( 3,wxT("CITY"),       DB_DATA_TYPE_VARCHAR,     City,           SQL_C_CHAR,                 sizeof(City),           FALSE,TRUE);
    SetColDefs ( 4,wxT("STATE"),      DB_DATA_TYPE_VARCHAR,     State,          SQL_C_CHAR,                 sizeof(State),          FALSE,TRUE);
    SetColDefs ( 5,wxT("POSTCODE"),   DB_DATA_TYPE_VARCHAR,     PostalCode,     SQL_C_CHAR,                 sizeof(PostalCode),     FALSE,TRUE);
    SetColDefs ( 6,wxT("COUNTRY"),    DB_DATA_TYPE_VARCHAR,     Country,        SQL_C_CHAR,                 sizeof(Country),        FALSE,TRUE);
    SetColDefs ( 7,wxT("JOINDATE"),   DB_DATA_TYPE_DATE,       &JoinDate,       SQL_C_TIMESTAMP,            sizeof(JoinDate),       FALSE,TRUE);
    SetColDefs ( 8,wxT("IS_DEV"),     DB_DATA_TYPE_INTEGER,    &IsDeveloper,    SQL_C_BOOLEAN(IsDeveloper), sizeof(IsDeveloper),    FALSE,TRUE);
    SetColDefs ( 9,wxT("CONTRIBS"),   DB_DATA_TYPE_INTEGER,    &Contributions,  SQL_C_USHORT,               sizeof(Contributions),  FALSE,TRUE);
    SetColDefs (10,wxT("LINE_CNT"),   DB_DATA_TYPE_INTEGER,    &LinesOfCode,    SQL_C_ULONG,                sizeof(LinesOfCode),    FALSE,TRUE);
    SetColDefs (11,wxT("LANGUAGE"),   DB_DATA_TYPE_INTEGER,    &NativeLanguage, SQL_C_ENUM,                 sizeof(NativeLanguage), FALSE,TRUE);
#if wxODBC_BLOB_EXPERIMENT > 0
    SetColDefs (12,wxT("PICTURE"),    DB_DATA_TYPE_BLOB,        Picture,        SQL_LONGVARBINARY,          sizeof(Picture),        FALSE,TRUE);
#endif
}  // Ccontact::SetupColumns


bool Ccontact::CreateIndexes(void)
{
    // This index could easily be accomplished with an "orderBy" clause, 
    // but is done to show how to construct a non-primary index.
    wxString    indexName;
    wxDbIdxDef  idxDef[2];

    bool        Ok = TRUE;

    wxStrcpy(idxDef[0].ColName, "IS_DEV");
    idxDef[0].Ascending = TRUE;

    wxStrcpy(idxDef[1].ColName, "NAME");
    idxDef[1].Ascending = TRUE;

    indexName = GetTableName();
    indexName += "_IDX1";
    Ok = CreateIndex(indexName.c_str(), TRUE, 2, idxDef);

    return Ok;
}  // Ccontact::CreateIndexes()


/*
 * Having a function to do a query on the primary key (and possibly others) is
 * very efficient and tighter coding so that it is available where ever the object
 * is.  Great for use with multiple tables when not using views or outer joins
 */
bool Ccontact::FetchByName(const wxString &name)
{
    whereStr.Printf(wxT("NAME = '%s'"),name.c_str());
    SetWhereClause(whereStr.c_str());
    SetOrderByClause(wxT(""));

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
 
BEGIN_EVENT_TABLE(CeditorDlg, wxPanel)
    EVT_BUTTON(-1,  CeditorDlg::OnButton)
    EVT_CLOSE(CeditorDlg::OnCloseWindow)
END_EVENT_TABLE()
 
CeditorDlg::CeditorDlg(wxWindow *parent) : wxPanel (parent, 0, 0, 537, 480)
{
    // Since the ::OnCommand() function is overridden, this prevents the widget
    // detection in ::OnCommand() until all widgets have been initialized to prevent
    // uninitialized pointers from crashing the program
    widgetPtrsSet = FALSE;

    initialized = FALSE;

    SetMode(mView);

    Contact = NULL;

    Show(FALSE);
}  // CeditorDlg constructor


void CeditorDlg::OnCloseWindow(wxCloseEvent& event)
{
    // Clean up time
    if ((mode != mCreate) && (mode != mEdit))
    {
        if (Contact)
        {
            delete Contact;
            Contact = NULL;
        }
        this->Destroy();
    }
    else
    {
        wxMessageBox(wxT("Must finish processing the current record being created/modified before exiting"),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        event.Veto();
    }
}  // CeditorDlg::OnCloseWindow()


void CeditorDlg::OnButton(wxCommandEvent &event)
{
    wxWindow *win = (wxWindow*) event.GetEventObject();
    OnCommand( *win, event );
}  // CeditorDlg::OnButton()


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
        pNameTxt->SetValue(wxT(""));
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

        CheckSupportForAllDataTypes(wxGetApp().READONLY_DB);
/*
        SetMode(mCreate);
        pNameTxt->SetValue(wxT(""));
        pNameTxt->SetFocus();
*/
        return;
    }

    if (widgetName == pDeleteBtn->GetName())
    {
        bool Ok = (wxMessageBox(wxT("Are you sure?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

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
            Contact->GetDb()->CommitTrans();

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
            Contact->GetDb()->RollbackTrans();

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
        bool Ok = (wxMessageBox(wxT("Are you sure?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

        if (!Ok)
            return;

        if (saveName.IsEmpty())
        {
            Contact->Initialize();
            PutData();
            SetMode(mView);
            return;
        }
        else
        {
            // Requery previous record
            if (Contact->FetchByName(saveName))
            {
                PutData();
                SetMode(mView);
                return;
            }
        }

        // Previous record not available, retrieve first record in table
        if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
        {
            Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
            Contact->whereStr += Contact->GetTableName();
            Contact->whereStr += wxT(")");
            Contact->SetWhereClause(Contact->whereStr.c_str());
        }
        else
            Contact->SetWhereClause(wxT(""));

        if (!Contact->Query())
        {
            wxString tStr;
            tStr  = wxT("ODBC error during Query()\n\n");
            tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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
        wxChar qryWhere[DB_MAX_WHERE_CLAUSE_LEN+1];
        wxStrcpy(qryWhere, (const wxChar*) Contact->qryWhereStr);
        wxChar *tblName[] = {(wxChar *)CONTACT_TABLE_NAME, 0};
        new CqueryDlg(GetParent(), Contact->GetDb(), tblName, qryWhere);

        // Query the first record in the new record set and
        // display it, if the query string has changed.
        if (wxStrcmp(qryWhere, (const wxChar*) Contact->qryWhereStr))
        {
            Contact->whereStr.Empty();
            Contact->SetOrderByClause("NAME");

            if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
            {
                Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
                Contact->whereStr += CONTACT_TABLE_NAME;
            }
            
            // Append the query where string (if there is one)
            Contact->qryWhereStr  = qryWhere;
            if (wxStrlen(qryWhere))
            {
                Contact->whereStr += wxT(" WHERE ");
                Contact->whereStr += Contact->qryWhereStr;
            }
            // Close the expression with a right paren
            Contact->whereStr += wxT(")");
            // Requery the table
            Contact->SetWhereClause(Contact->whereStr.c_str());
            if (!Contact->Query())
            {
                wxString tStr;
                tStr  = wxT("ODBC error during Query()\n\n");
                tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
                wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                return;
            }
            // Display the first record from the query set
            if (!Contact->GetNext())
                Contact->Initialize();
            PutData();
        }

        // Enable/Disable the reset button
        pResetBtn->Enable(!Contact->qryWhereStr.IsEmpty());

        return;
    }  // Query button


    if (widgetName == pResetBtn->GetName())
    {
        // Clear the additional where criteria established by the query feature
        Contact->qryWhereStr = wxT("");
        Contact->SetOrderByClause(wxT("NAME"));

        if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
        {
            Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
            Contact->whereStr += CONTACT_TABLE_NAME;
            Contact->whereStr += wxT(")");
        }

        Contact->SetWhereClause(Contact->whereStr.c_str());
        if (!Contact->Query())
        {
            wxString tStr;
            tStr  = wxT("ODBC error during Query()\n\n");
            tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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
        new ClookUpDlg(/* wxWindow    *parent        */ this,
                       /* wxChar      *windowTitle   */ wxT("Select contact name"),
                       /* wxChar      *tableName     */ (wxChar *) CONTACT_TABLE_NAME,
                       /* wxChar      *dispCol1      */ wxT("NAME"),
                       /* wxChar      *dispCol2      */ wxT("JOINDATE"),
                       /* wxChar      *where         */ wxT(""),
                       /* wxChar      *orderBy       */ wxT("NAME"),
                       /* wxDb        *pDb           */ wxGetApp().READONLY_DB,
                       /* const wxString &defDir     */ wxGetApp().DbConnectInf->GetDefaultDir(),
                       /* bool        distinctValues */ TRUE);

        if (ListDB_Selection && wxStrlen(ListDB_Selection))
        {
            wxString w = wxT("NAME = '");
            w += ListDB_Selection;
            w += wxT("'");
            GetRec(w);
        }

        return;
    }
}  // CeditorDlg::OnCommand()


bool CeditorDlg::Initialize()
{
    // Create the data structure and a new database connection.  
    // (As there is not a pDb being passed in the constructor, a new database
    // connection is created)
    Contact = new Ccontact();

    if (!Contact)
    {
        wxMessageBox(wxT("Unable to instantiate an instance of Ccontact"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    // Check if the table exists or not.  If it doesn't, ask the user if they want to 
    // create the table.  Continue trying to create the table until it exists, or user aborts
    while (!Contact->GetDb()->TableExists((wxChar *)CONTACT_TABLE_NAME, 
                                          wxGetApp().DbConnectInf->GetUserID(), 
                                          wxGetApp().DbConnectInf->GetDefaultDir()))
    {
        wxString tStr;
        tStr.Printf(wxT("Unable to open the table '%s'.\n\nTable may need to be created...?\n\n"),CONTACT_TABLE_NAME);
        tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
        wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

        bool createTable = (wxMessageBox(wxT("Do you wish to try to create/clear the CONTACTS table?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

        if (!createTable)
        {
//            Close();
            return FALSE;
        }
        else
            wxGetApp().CreateDataTable(FALSE);
    }

    // Tables must be "opened" before anything other than creating/deleting table can be done
    if (!Contact->Open())
    {
        // Table does exist, or there was some problem opening it.  Currently this should
        // never fail, except in the case of the table not exisiting or the current
        // user has insufficent privileges to access the table
#if 1
// This code is experimenting with a new function that will hopefully be available
// in the 2.4 release.  This check will determine whether the open failing was due
// to the table not existing, or the users privileges being insufficient to
// open the table.
        if (!Contact->GetDb()->TablePrivileges(CONTACT_TABLE_NAME, wxT("SELECT"),
                                               Contact->GetDb()->GetUsername(),
															  Contact->GetDb()->GetUsername(),
															  wxGetApp().DbConnectInf->GetDefaultDir()))
        {
            wxString tStr;
            tStr.Printf(wxT("Unable to open the table '%s' (likely due to\ninsufficient privileges of the logged in user).\n\n"),CONTACT_TABLE_NAME);
            tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        }
        else 
#endif
        if (!Contact->GetDb()->TableExists(CONTACT_TABLE_NAME,
                                           Contact->GetDb()->GetUsername(),
                                           wxGetApp().DbConnectInf->GetDefaultDir()))
        {
            wxString tStr;
            tStr.Printf(wxT("Unable to open the table '%s' as the table\ndoes not appear to exist in the tablespace available\nto the currently logged in user.\n\n"),CONTACT_TABLE_NAME);
            tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        }

        return FALSE;
    }

    // Build the dialog

    (void)new wxStaticBox(this, EDITOR_DIALOG_FN_GROUP, wxT(""),  wxPoint(15, 1), wxSize(497,  69), 0, wxT("FunctionGrp"));
    (void)new wxStaticBox(this, EDITOR_DIALOG_SEARCH_GROUP, wxT(""), wxPoint(417, 1), wxSize(95, 242), 0, wxT("SearchGrp"));

    pCreateBtn      = new wxButton(this, EDITOR_DIALOG_CREATE,           wxT("&Create"),     wxPoint( 25,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("CreateBtn"));
    pEditBtn        = new wxButton(this, EDITOR_DIALOG_EDIT,             wxT("&Edit"),       wxPoint(102,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("EditBtn"));
    pDeleteBtn      = new wxButton(this, EDITOR_DIALOG_DELETE,           wxT("&Delete"),     wxPoint(179,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("DeleteBtn"));
    pCopyBtn        = new wxButton(this, EDITOR_DIALOG_COPY,             wxT("Cop&y"),       wxPoint(256,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("CopyBtn"));
    pSaveBtn        = new wxButton(this, EDITOR_DIALOG_SAVE,             wxT("&Save"),       wxPoint(333,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("SaveBtn"));
    pCancelBtn      = new wxButton(this, EDITOR_DIALOG_CANCEL,           wxT("C&ancel"),     wxPoint(430,  21), wxSize( 70,  35), 0, wxDefaultValidator, wxT("CancelBtn"));
    pPrevBtn        = new wxButton(this, EDITOR_DIALOG_PREV,             wxT("<< &Prev"),    wxPoint(430,  81), wxSize( 70,  35), 0, wxDefaultValidator, wxT("PrevBtn"));
    pNextBtn        = new wxButton(this, EDITOR_DIALOG_NEXT,             wxT("&Next >>"),    wxPoint(430, 121), wxSize( 70,  35), 0, wxDefaultValidator, wxT("NextBtn"));
    pQueryBtn       = new wxButton(this, EDITOR_DIALOG_QUERY,            wxT("&Query"),      wxPoint(430, 161), wxSize( 70,  35), 0, wxDefaultValidator, wxT("QueryBtn"));
    pResetBtn       = new wxButton(this, EDITOR_DIALOG_RESET,            wxT("&Reset"),      wxPoint(430, 200), wxSize( 70,  35), 0, wxDefaultValidator, wxT("ResetBtn"));
    pNameMsg        = new wxStaticText(this, EDITOR_DIALOG_NAME_MSG,     wxT("Name:"),       wxPoint( 17,  80), wxSize( -1,  -1), 0, wxT("NameMsg"));
    pNameTxt        = new wxTextCtrl(this, EDITOR_DIALOG_NAME_TEXT,      wxT(""),            wxPoint( 17,  97), wxSize(308,  25), 0, wxDefaultValidator, wxT("NameTxt"));
    pNameListBtn    = new wxButton(this, EDITOR_DIALOG_LOOKUP,           wxT("&Lookup"),     wxPoint(333,  97), wxSize( 70,  24), 0, wxDefaultValidator, wxT("LookupBtn"));
    pAddress1Msg    = new wxStaticText(this, EDITOR_DIALOG_ADDRESS1_MSG, wxT("Address:"),    wxPoint( 17, 130), wxSize( -1,  -1), 0, wxT("Address1Msg"));
    pAddress1Txt    = new wxTextCtrl(this, EDITOR_DIALOG_ADDRESS2_TEXT,  wxT(""),            wxPoint( 17, 147), wxSize(308,  25), 0, wxDefaultValidator, wxT("Address1Txt"));
    pAddress2Msg    = new wxStaticText(this, EDITOR_DIALOG_ADDRESS2_MSG, wxT("Address:"),    wxPoint( 17, 180), wxSize( -1,  -1), 0, wxT("Address2Msg"));
    pAddress2Txt    = new wxTextCtrl(this, EDITOR_DIALOG_ADDRESS2_TEXT,  wxT(""),            wxPoint( 17, 197), wxSize(308,  25), 0, wxDefaultValidator, wxT("Address2Txt"));
    pCityMsg        = new wxStaticText(this, EDITOR_DIALOG_CITY_MSG,     wxT("City:"),       wxPoint( 17, 230), wxSize( -1,  -1), 0, wxT("CityMsg"));
    pCityTxt        = new wxTextCtrl(this, EDITOR_DIALOG_CITY_TEXT,      wxT(""),            wxPoint( 17, 247), wxSize(225,  25), 0, wxDefaultValidator, wxT("CityTxt"));
    pStateMsg       = new wxStaticText(this, EDITOR_DIALOG_STATE_MSG,    wxT("State:"),      wxPoint(250, 230), wxSize( -1,  -1), 0, wxT("StateMsg"));
    pStateTxt       = new wxTextCtrl(this, EDITOR_DIALOG_STATE_TEXT,     wxT(""),            wxPoint(250, 247), wxSize(153,  25), 0, wxDefaultValidator, wxT("StateTxt"));
    pCountryMsg     = new wxStaticText(this, EDITOR_DIALOG_COUNTRY_MSG,  wxT("Country:"),    wxPoint( 17, 280), wxSize( -1,  -1), 0, wxT("CountryMsg"));
    pCountryTxt     = new wxTextCtrl(this, EDITOR_DIALOG_COUNTRY_TEXT,   wxT(""),            wxPoint( 17, 297), wxSize(225,  25), 0, wxDefaultValidator, wxT("CountryTxt"));
    pPostalCodeMsg  = new wxStaticText(this, EDITOR_DIALOG_POSTAL_MSG,   wxT("Postal Code:"),wxPoint(250, 280), wxSize( -1,  -1), 0, wxT("PostalCodeMsg"));
    pPostalCodeTxt  = new wxTextCtrl(this, EDITOR_DIALOG_POSTAL_TEXT,    wxT(""),            wxPoint(250, 297), wxSize(153,  25), 0, wxDefaultValidator, wxT("PostalCodeTxt"));

    wxString choice_strings[5];
    choice_strings[0] = wxT("English");
    choice_strings[1] = wxT("French");
    choice_strings[2] = wxT("German");
    choice_strings[3] = wxT("Spanish");
    choice_strings[4] = wxT("Other");

    pNativeLangChoice = new wxChoice(this, EDITOR_DIALOG_LANG_CHOICE,                             wxPoint( 17, 346), wxSize(277,  -1), 5, choice_strings);
    pNativeLangMsg    = new wxStaticText(this, EDITOR_DIALOG_LANG_MSG,   wxT("Native language:"), wxPoint( 17, 330), wxSize( -1,  -1), 0, wxT("NativeLangMsg"));

    wxString radio_strings[2];
    radio_strings[0]  = wxT("No");
    radio_strings[1]  = wxT("Yes");
    pDeveloperRadio   = new wxRadioBox(this,EDITOR_DIALOG_DEVELOPER,     wxT("Developer:"),       wxPoint(303, 330), wxSize( -1,  -1), 2, radio_strings, 2, wxHORIZONTAL);
    pJoinDateMsg      = new wxStaticText(this, EDITOR_DIALOG_JOIN_MSG,   wxT("Date joined:"),     wxPoint( 17, 380), wxSize( -1,  -1), 0, wxT("JoinDateMsg"));
    pJoinDateTxt      = new wxTextCtrl(this, EDITOR_DIALOG_JOIN_TEXT,    wxT(""),                 wxPoint( 17, 397), wxSize(150,  25), 0, wxDefaultValidator, wxT("JoinDateTxt"));
    pContribMsg       = new wxStaticText(this, EDITOR_DIALOG_CONTRIB_MSG,wxT("Contributions:"),   wxPoint(175, 380), wxSize( -1,  -1), 0, wxT("ContribMsg"));
    pContribTxt       = new wxTextCtrl(this, EDITOR_DIALOG_CONTRIB_TEXT, wxT(""),                 wxPoint(175, 397), wxSize(120,  25), 0, wxDefaultValidator, wxT("ContribTxt"));
    pLinesMsg         = new wxStaticText(this, EDITOR_DIALOG_LINES_MSG,  wxT("Lines of code:"),   wxPoint(303, 380), wxSize( -1,  -1), 0, wxT("LinesMsg"));
    pLinesTxt         = new wxTextCtrl(this, EDITOR_DIALOG_LINES_TEXT,   wxT(""),                 wxPoint(303, 397), wxSize(100,  25), 0, wxDefaultValidator, wxT("LinesTxt"));

    // Now that all the widgets on the panel are created, its safe to allow ::OnCommand() to 
    // handle all widget processing
    widgetPtrsSet = TRUE;

    // Setup the orderBy and where clauses to return back a single record as the result set, 
    // as there will only be one record being shown on the dialog at a time, this optimizes
    // network traffic by only returning a one row result
    
    Contact->SetOrderByClause(wxT("NAME"));  // field name to sort by

    // The wxString "whereStr" is not a member of the wxDbTable object, it is a member variable
    // specifically in the Ccontact class.  It is used here for simpler construction of a varying
    // length string, and then after the string is built, the wxDbTable member variable "where" is
    // assigned the pointer to the constructed string.
    //
    // The constructed where clause below has a sub-query within it "SELECT MIN(NAME) FROM %s" 
    // to achieve a single row (in this case the first name in alphabetical order).
    
    if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        Contact->whereStr.Printf(wxT("NAME = (SELECT MIN(NAME) FROM %s)"),Contact->GetTableName().c_str());
        // NOTE: (const wxChar*) returns a pointer which may not be valid later, so this is short term use only
        Contact->SetWhereClause(Contact->whereStr);
    }
    else
       Contact->SetWhereClause(wxT(""));

    // Perform the Query to get the result set.  
    // NOTE: If there are no rows returned, that is a valid result, so Query() would return TRUE.  
    //       Only if there is a database error will Query() come back as FALSE
    if (!Contact->Query())
    {
        wxString tStr;
        tStr  = wxT("ODBC error during Query()\n\n");
        tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
        wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
//        GetParent()->Close();
        return FALSE;
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

    initialized = TRUE;
    return TRUE;
}  // CeditorDlg::Initialize()


void CeditorDlg::FieldsEditable()
{
    if (!widgetPtrsSet)
        return;

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
    bool edit = FALSE;

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
        pEditBtn->Enable( !edit && (wxStrcmp(Contact->Name,wxT(""))!=0) );
        pDeleteBtn->Enable( !edit && (wxStrcmp(Contact->Name,wxT(""))!=0) );
        pCopyBtn->Enable( !edit && (wxStrcmp(Contact->Name,wxT(""))!=0) );
        pSaveBtn->Enable( edit );
        pCancelBtn->Enable( edit );
        pPrevBtn->Enable( !edit );
        pNextBtn->Enable( !edit );
        pQueryBtn->Enable( !edit );
        pResetBtn->Enable( !edit && !Contact->qryWhereStr.IsEmpty() );
        pNameListBtn->Enable( !edit );
    }

    FieldsEditable();
}  // CeditorDlg::SetMode()


bool CeditorDlg::PutData()
{
    wxString tStr;

    pNameTxt->SetValue(Contact->Name);
    pAddress1Txt->SetValue(Contact->Addr1);
    pAddress2Txt->SetValue(Contact->Addr2);
    pCityTxt->SetValue(Contact->City);
    pStateTxt->SetValue(Contact->State);
    pCountryTxt->SetValue(Contact->Country);
    pPostalCodeTxt->SetValue(Contact->PostalCode);

    tStr.Printf(wxT("%d/%d/%d"),Contact->JoinDate.month,Contact->JoinDate.day,Contact->JoinDate.year);
    pJoinDateTxt->SetValue(tStr);

    tStr.Printf(wxT("%d"),Contact->Contributions);
    pContribTxt->SetValue(tStr);

    tStr.Printf(wxT("%lu"),Contact->LinesOfCode);
    pLinesTxt->SetValue(tStr);

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
bool CeditorDlg::GetData()
{
    // Validate that the data currently entered into the widgets is valid data

    wxString tStr;
    tStr = pNameTxt->GetValue();
    if (!wxStrcmp((const wxChar*) tStr,wxT("")))
    {
        wxMessageBox(wxT("A name is required for entry into the contact table"),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        return FALSE;
    }

    bool   invalid = FALSE;
    int    mm,dd,yyyy;
    int    first, second;

    tStr = pJoinDateTxt->GetValue();
    if (tStr.Freq(wxT('/')) != 2)
        invalid = TRUE;

    // Find the month, day, and year tokens
    if (!invalid)
    {
        first   = tStr.First(wxT('/'));
        second  = tStr.Last(wxT('/'));

        mm      = atoi(tStr.SubString(0,first));
        dd      = atoi(tStr.SubString(first+1,second));
        yyyy    = atoi(tStr.SubString(second+1,tStr.Length()-1));

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
        wxMessageBox(wxT("Improper date format.  Please check the date\nspecified and try again.\n\nNOTE: Dates are in american format (MM/DD/YYYY)"),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        return FALSE;
    }

    tStr = pNameTxt->GetValue();
    wxStrcpy(Contact->Name,(const wxChar*) tStr);
    wxStrcpy(Contact->Addr1,pAddress1Txt->GetValue());
    wxStrcpy(Contact->Addr2,pAddress2Txt->GetValue());
    wxStrcpy(Contact->City,pCityTxt->GetValue());
    wxStrcpy(Contact->State,pStateTxt->GetValue());
    wxStrcpy(Contact->Country,pCountryTxt->GetValue());
    wxStrcpy(Contact->PostalCode,pPostalCodeTxt->GetValue());

    Contact->Contributions = atoi(pContribTxt->GetValue());
    Contact->LinesOfCode = atol(pLinesTxt->GetValue());

    Contact->NativeLanguage = (enum Language) pNativeLangChoice->GetSelection();
    Contact->IsDeveloper = pDeveloperRadio->GetSelection() > 0;

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
bool CeditorDlg::Save()
{
    bool failed = FALSE;

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
                    tStr  = wxT("A duplicate key value already exists in the table.\nUnable to save record\n\n");
                    tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
                    wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                }
                else
                {
                    // Some other unexpexted error occurred
                    wxString tStr;
                    tStr  = wxT("Database insert failed\n\n");
                    tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
                    wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                }
            }
        }
        else  // mode == mEdit
        {
            Contact->whereStr.Printf("NAME = '%s'",saveName.c_str());
            if (!Contact->UpdateWhere(Contact->whereStr))
            {
                wxString tStr;
                tStr  = wxT("Database update failed\n\n");
                tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
                wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                failed = TRUE;
            }
        }

        if (!failed)
        {
            Contact->GetDb()->CommitTrans();
            SetMode(mView);  // Sets the dialog mode back to viewing after save is successful
        }
        else
            Contact->GetDb()->RollbackTrans();

        wxEndBusyCursor();
    }

    return !failed;
}  // CeditorDlg::Save()


/*
 * Where this program is only showing a single row at a time in the dialog,
 * a special where clause must be built to find just the single row which,
 * in sequence, would follow the currently displayed row.
 */
bool CeditorDlg::GetNextRec()
{
    wxString w;

    if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        w  = wxT("NAME = (SELECT MIN(NAME) FROM ");
        w += Contact->GetTableName();
        w += wxT(" WHERE NAME > '");
    }
    else
        w = wxT("(NAME > '");

    w += Contact->Name;
    w += wxT("'");

    // If a query where string is currently set, append that criteria
    if (!Contact->qryWhereStr.IsEmpty())
    {
        w += wxT(" AND (");
        w += Contact->qryWhereStr;
        w += wxT(")");
    }

    w += wxT(")");
    return(GetRec(w));

}  // CeditorDlg::GetNextRec()


/*
 * Where this program is only showing a single row at a time in the dialog,
 * a special where clause must be built to find just the single row which,
 * in sequence, would precede the currently displayed row.
 */
bool CeditorDlg::GetPrevRec()
{
    wxString w;

    if (Contact->GetDb()->Dbms() != dbmsPOSTGRES && Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        w  = wxT("NAME = (SELECT MAX(NAME) FROM ");
        w += Contact->GetTableName();
        w += wxT(" WHERE NAME < '");
    }
    else
        w = wxT("(NAME < '");

    w += Contact->Name;
    w += wxT("'");

    // If a query where string is currently set, append that criteria
    if (!Contact->qryWhereStr.IsEmpty())
    {
        w += wxT(" AND (");
        w += Contact->qryWhereStr;
        w += wxT(")");
    }

    w += wxT(")");

    return(GetRec(w));

}  // CeditorDlg::GetPrevRec()


/*
 * This function is here to avoid duplicating this same code in both the
 * GetPrevRec() and GetNextRec() functions
 */
bool CeditorDlg::GetRec(const wxString &whereStr)
{
    Contact->SetWhereClause(whereStr);
    Contact->SetOrderByClause(wxT("NAME"));

    if (!Contact->Query())
    {
        wxString tStr;
        tStr  = wxT("ODBC error during Query()\n\n");
        tStr += GetExtendedDBErrorMsg(Contact->GetDb(),__FILE__,__LINE__);
        wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

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

BEGIN_EVENT_TABLE(CparameterDlg, wxDialog)
    EVT_BUTTON(PARAMETER_DIALOG_SAVE,  CparameterDlg::OnButton)
    EVT_BUTTON(PARAMETER_DIALOG_CANCEL,  CparameterDlg::OnButton)
    EVT_CLOSE(CparameterDlg::OnCloseWindow)
END_EVENT_TABLE()

CparameterDlg::CparameterDlg(wxWindow *parent) : wxDialog (parent, PARAMETER_DIALOG, wxT("ODBC parameter settings"), wxPoint(-1, -1), wxSize(400, 325))
{
    // Since the ::OnCommand() function is overridden, this prevents the widget
    // detection in ::OnCommand() until all widgets have been initialized to prevent
    // uninitialized pointers from crashing the program
    widgetPtrsSet = FALSE;

    pParamODBCSourceMsg  = new wxStaticText(this, PARAMETER_DIALOG_SOURCE_MSG,   wxT("ODBC data sources:"),   wxPoint( 10, 10),    wxSize( -1,  -1), 0, wxT("ParamODBCSourceMsg"));
    pParamODBCSourceList = new wxListBox(this, PARAMETER_DIALOG_SOURCE_LISTBOX,                          wxPoint( 10, 29),    wxSize(285, 150), 0, 0, wxLB_SINGLE|wxLB_ALWAYS_SB, wxDefaultValidator, wxT("ParamODBCSourceList"));
    pParamUserNameMsg    = new wxStaticText(this, PARAMETER_DIALOG_NAME_MSG,     wxT("Database user name:"),  wxPoint( 10, 193),   wxSize( -1,  -1), 0, wxT("ParamUserNameMsg"));
    pParamUserNameTxt    = new wxTextCtrl(this, PARAMETER_DIALOG_NAME_TEXT,      wxT(""), wxPoint(10, 209),   wxSize( 140, 25),    0, wxDefaultValidator, wxT("ParamUserNameTxt"));
    pParamPasswordMsg    = new wxStaticText(this, PARAMETER_DIALOG_PASSWORD_MSG, wxT("Password:"),            wxPoint(156, 193),   wxSize( -1,  -1), 0, wxT("ParamPasswordMsg"));
    pParamPasswordTxt    = new wxTextCtrl(this, PARAMETER_DIALOG_PASSWORD_TEXT,  wxT(""), wxPoint(156, 209),  wxSize( 140,  25),   0, wxDefaultValidator, wxT("ParamPasswordTxt"));
    pParamDirPathMsg     = new wxStaticText(this, PARAMETER_DIALOG_DIRPATH_MSG,  wxT("Directory:"),           wxPoint( 10, 243),   wxSize( -1,  -1), 0, wxT("ParamDirPathMsg"));
    pParamDirPathTxt     = new wxTextCtrl(this, PARAMETER_DIALOG_DIRPATH_TEXT,   wxT(""),                     wxPoint( 10, 259),   wxSize(140,  25), 0, wxDefaultValidator, wxT("ParamDirPathTxt"));
    pParamSaveBtn        = new wxButton(this, PARAMETER_DIALOG_SAVE,             wxT("&Save"),                wxPoint(310,  21),   wxSize( 70,  35), 0, wxDefaultValidator, wxT("ParamSaveBtn"));
    pParamCancelBtn      = new wxButton(this, PARAMETER_DIALOG_CANCEL,           wxT("C&ancel"),              wxPoint(310,  66),   wxSize( 70,  35), 0, wxDefaultValidator, wxT("ParamCancelBtn"));

    // Now that all the widgets on the panel are created, its safe to allow ::OnCommand() to 
    // handle all widget processing
    widgetPtrsSet = TRUE;

    saved = FALSE;
    savedParamSettings = wxGetApp().params;

    Centre(wxBOTH);
    PutData();
    ShowModal();
}  // CparameterDlg constructor


void CparameterDlg::OnCloseWindow(wxCloseEvent& event)
{
    // Put any additional checking necessary to make certain it is alright
    // to close the program here that is not done elsewhere
    if (!saved)
    {
        bool Ok = (wxMessageBox(wxT("No changes have been saved.\n\nAre you sure you wish exit the parameter screen?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);
        
        if (!Ok)
        {
            event.Veto();
            return;
        }
        
        wxGetApp().params = savedParamSettings;
    }

    if (GetParent() != NULL)
        GetParent()->SetFocus();

    while (wxIsBusy())
        wxEndBusyCursor();

    Show(FALSE);
    SetReturnCode(0);  // added so BoundsChecker would not report use of uninitialized variable

    this->Destroy();
}  // CparameterDlg::OnCloseWindow()


void CparameterDlg::OnButton( wxCommandEvent &event )
{
    wxWindow *win = (wxWindow*) event.GetEventObject();
    OnCommand( *win, event );
}


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
            tStr = wxT("Database parameters have been saved.");
            if (GetParent() != NULL)  // The parameter dialog was not called during startup due to a missing cfg file
                tStr += wxT("\nNew parameters will take effect the next time the program is started.");
            wxMessageBox(tStr,wxT("Notice..."),wxOK | wxICON_INFORMATION);
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


bool CparameterDlg::PutData()
{
    // Fill the data source list box
    FillDataSourceList();

    // Fill in the fields from the params object
    if (wxGetApp().params.ODBCSource && wxStrlen(wxGetApp().params.ODBCSource))
        pParamODBCSourceList->SetStringSelection(wxGetApp().params.ODBCSource);
    pParamUserNameTxt->SetValue(wxGetApp().params.UserName);
    pParamPasswordTxt->SetValue(wxGetApp().params.Password);
    pParamDirPathTxt->SetValue(wxGetApp().params.DirPath);
    return TRUE;
}  // CparameterDlg::PutData()


bool CparameterDlg::GetData()
{
    wxString tStr;
    if (pParamODBCSourceList->GetStringSelection() != wxT(""))
    {
        tStr = pParamODBCSourceList->GetStringSelection();
        if (tStr.Length() > (sizeof(wxGetApp().params.ODBCSource)-1))
        {
            wxString errmsg;
            errmsg.Printf(wxT("ODBC Data source name is longer than the data structure to hold it.\n'Cparameter.ODBCSource' must have a larger character array\nto handle a data source with this long of a name\n\nThe data source currently selected is %d characters long."),tStr.Length());
            wxMessageBox(errmsg,wxT("Internal program error..."),wxOK | wxICON_EXCLAMATION);
            return FALSE;
        }
        wxStrcpy(wxGetApp().params.ODBCSource, tStr);
    }
    else
        return FALSE;
    
    tStr = pParamUserNameTxt->GetValue();
    if (tStr.Length() > (sizeof(wxGetApp().params.UserName)-1))
    {
        wxString errmsg;
        errmsg.Printf(wxT("User name is longer than the data structure to hold it.\n'Cparameter.UserName' must have a larger character array\nto handle a data source with this long of a name\n\nThe user name currently specified is %d characters long."),tStr.Length());
        wxMessageBox(errmsg,wxT("Internal program error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }
    wxStrcpy(wxGetApp().params.UserName, tStr);

    tStr = pParamPasswordTxt->GetValue();
    if (tStr.Length() > (sizeof(wxGetApp().params.Password)-1))
    {
        wxString errmsg;
        errmsg.Printf(wxT("Password is longer than the data structure to hold it.\n'Cparameter.Password' must have a larger character array\nto handle a data source with this long of a name\n\nThe password currently specified is %d characters long."),tStr.Length());
        wxMessageBox(errmsg,wxT("Internal program error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }
    wxStrcpy(wxGetApp().params.Password,tStr);

    tStr = pParamDirPathTxt->GetValue();
    tStr.Replace(wxT("\\"),wxT("/"));
    if (tStr.Length() > (sizeof(wxGetApp().params.DirPath)-1))
    {
        wxString errmsg;
        errmsg.Printf(wxT("DirPath is longer than the data structure to hold it.\n'Cparameter.DirPath' must have a larger character array\nto handle a data source with this long of a name\n\nThe password currently specified is %d characters long."),tStr.Length());
        wxMessageBox(errmsg,wxT("Internal program error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }
    wxStrcpy(wxGetApp().params.DirPath,tStr);
    return TRUE;
}  // CparameterDlg::GetData()


bool CparameterDlg::Save()
{
    // Copy the current params in case user cancels changing
    // the params, so that we can reset them.
    if (!GetData())
    {
        wxGetApp().params = savedParamSettings;
        return FALSE;
    }

    wxGetApp().WriteParamFile(wxGetApp().params);

    return TRUE;
}  // CparameterDlg::Save()


void CparameterDlg::FillDataSourceList()
{
    wxChar Dsn[SQL_MAX_DSN_LENGTH + 1];
    wxChar DsDesc[255];
    wxStringList strList;

    while (wxDbGetDataSource(wxGetApp().DbConnectInf->GetHenv(), Dsn,
                             SQL_MAX_DSN_LENGTH+1, DsDesc, 255))
        strList.Add(Dsn);

    strList.Sort();
    strList.Add(wxT(""));
    wxChar **p = strList.ListToArray();

    int i;
    for (i = 0; wxStrlen(p[i]); i++)
        pParamODBCSourceList->Append(p[i]);

    delete [] p;
}  // CparameterDlg::FillDataSourceList()


BEGIN_EVENT_TABLE(CqueryDlg, wxDialog)
    EVT_BUTTON(-1,  CqueryDlg::OnButton)
    EVT_CLOSE(CqueryDlg::OnCloseWindow)
END_EVENT_TABLE()

 
// CqueryDlg() constructor
CqueryDlg::CqueryDlg(wxWindow *parent, wxDb *pDb, wxChar *tblName[], 
                     const wxString &pWhereArg) :
    wxDialog (parent, QUERY_DIALOG, wxT("Query"), wxPoint(-1, -1), wxSize(480, 360))
{
    wxBeginBusyCursor();

    colInf = 0;
    dbTable = 0;
    masterTableName = tblName[0];
    widgetPtrsSet = FALSE;
    pDB = pDb;

    // Initialize the WHERE clause from the string passed in
    pWhere = pWhereArg;                                           // Save a pointer to the output buffer
    if (pWhere.Length() > (unsigned int)DB_MAX_WHERE_CLAUSE_LEN)  // Check the length of the buffer passed in
    {
        wxString s;
        s.Printf(wxT("Maximum where clause length exceeded.\nLength must be less than %d"), DB_MAX_WHERE_CLAUSE_LEN+1);
        wxMessageBox(s,wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        Close();
        return;
    }

    pQueryCol1Msg           = new wxStaticText(this, QUERY_DIALOG_COL_MSG,    wxT("Column 1:"),   wxPoint( 10,  10), wxSize( 69,  16), 0, wxT("QueryCol1Msg"));
    pQueryCol1Choice        = new wxChoice(this, QUERY_DIALOG_COL_CHOICE,                    wxPoint( 10,  27), wxSize(250,  27), 0, 0, 0, wxDefaultValidator, wxT("QueryCol1Choice"));
    pQueryNotMsg            = new wxStaticText(this, QUERY_DIALOG_NOT_MSG,    wxT("NOT"),         wxPoint(268,  10), wxSize( -1,  -1), 0, wxT("QueryNotMsg"));
    pQueryNotCheck          = new wxCheckBox(this, QUERY_DIALOG_NOT_CHECKBOX, wxT(""),            wxPoint(275,  37), wxSize( 20,  20), 0, wxDefaultValidator, wxT("QueryNotCheck"));

    wxString choice_strings[9];
    choice_strings[0] = wxT("=");
    choice_strings[1] = wxT("<");
    choice_strings[2] = wxT(">");
    choice_strings[3] = wxT("<=");
    choice_strings[4] = wxT(">=");
    choice_strings[5] = wxT("Begins");
    choice_strings[6] = wxT("Contains");
    choice_strings[7] = wxT("Like");
    choice_strings[8] = wxT("Between");

    pQueryOperatorMsg       = new wxStaticText(this, QUERY_DIALOG_OP_MSG,       wxT("Operator:"),         wxPoint(305,  10), wxSize( -1,  -1), 0, wxT("QueryOperatorMsg"));
    pQueryOperatorChoice    = new wxChoice(this, QUERY_DIALOG_OP_CHOICE,                                  wxPoint(305,  27), wxSize( 80,  27), 9, choice_strings, 0, wxDefaultValidator, wxT("QueryOperatorChoice"));
    pQueryCol2Msg           = new wxStaticText(this, QUERY_DIALOG_COL2_MSG,     wxT("Column 2:"),         wxPoint( 10,  65), wxSize( 69,  16), 0, wxT("QueryCol2Msg"));
    pQueryCol2Choice        = new wxChoice(this, QUERY_DIALOG_COL2_CHOICE,                                wxPoint( 10,  82), wxSize(250,  27), 0, 0, 0, wxDefaultValidator, wxT("QueryCol2Choice"));
    pQuerySqlWhereMsg       = new wxStaticText(this, QUERY_DIALOG_WHERE_MSG,    wxT("SQL where clause:"), wxPoint( 10, 141), wxSize( -1,  -1), 0, wxT("QuerySqlWhereMsg"));
    pQuerySqlWhereMtxt      = new wxTextCtrl(this, QUERY_DIALOG_WHERE_TEXT,     wxT(""),                  wxPoint( 10, 159), wxSize(377, 134), wxTE_MULTILINE, wxDefaultValidator, wxT("QuerySqlWhereMtxt"));
    pQueryAddBtn            = new wxButton(this, QUERY_DIALOG_ADD,              wxT("&Add"),              wxPoint(406,  24), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryAddBtn"));
    pQueryAndBtn            = new wxButton(this, QUERY_DIALOG_AND,              wxT("A&nd"),              wxPoint(406,  58), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryAndBtn"));
    pQueryOrBtn             = new wxButton(this, QUERY_DIALOG_OR,               wxT("&Or"),               wxPoint(406,  92), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryOrBtn"));
    pQueryLParenBtn         = new wxButton(this, QUERY_DIALOG_LPAREN,           wxT("("),                 wxPoint(406, 126), wxSize( 26,  26), 0, wxDefaultValidator, wxT("QueryLParenBtn"));
    pQueryRParenBtn         = new wxButton(this, QUERY_DIALOG_RPAREN,           wxT(")"),                 wxPoint(436, 126), wxSize( 26,  26), 0, wxDefaultValidator, wxT("QueryRParenBtn"));
    pQueryDoneBtn           = new wxButton(this, QUERY_DIALOG_DONE,             wxT("&Done"),             wxPoint(406, 185), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryDoneBtn"));
    pQueryClearBtn          = new wxButton(this, QUERY_DIALOG_CLEAR,            wxT("C&lear"),            wxPoint(406, 218), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryClearBtn"));
    pQueryCountBtn          = new wxButton(this, QUERY_DIALOG_COUNT,            wxT("&Count"),            wxPoint(406, 252), wxSize( 56,  26), 0, wxDefaultValidator, wxT("QueryCountBtn"));
    pQueryValue1Msg         = new wxStaticText(this, QUERY_DIALOG_VALUE1_MSG,   wxT("Value:"),            wxPoint(277,  66), wxSize( -1,  -1), 0, wxT("QueryValue1Msg"));
    pQueryValue1Txt         = new wxTextCtrl(this, QUERY_DIALOG_VALUE1_TEXT,    wxT(""),                  wxPoint(277,  83), wxSize(108,  25), 0, wxDefaultValidator, wxT("QueryValue1Txt"));
    pQueryValue2Msg         = new wxStaticText(this, QUERY_DIALOG_VALUE2_MSG,   wxT("AND"),               wxPoint(238, 126), wxSize( -1,  -1), 0, wxT("QueryValue2Msg"));
    pQueryValue2Txt         = new wxTextCtrl(this, QUERY_DIALOG_VALUE2_TEXT,    wxT(""),                  wxPoint(277, 120), wxSize(108,  25), 0, wxDefaultValidator, wxT("QueryValue2Txt"));
    pQueryHintGrp           = new wxStaticBox(this, QUERY_DIALOG_HINT_GROUP,    wxT(""),                  wxPoint( 10, 291), wxSize(377,  40), 0, wxT("QueryHintGrp"));
    pQueryHintMsg           = new wxStaticText(this, QUERY_DIALOG_HINT_MSG,     wxT(""),                  wxPoint( 16, 306), wxSize( -1,  -1), 0, wxT("QueryHintMsg"));

    widgetPtrsSet = TRUE;
    // Initialize the dialog
    wxString qualName;
    pQueryCol2Choice->Append(wxT("VALUE -->"));
    colInf = pDB->GetColumns(tblName);

    if (!colInf)
    {
        wxEndBusyCursor();
        wxString tStr;
        tStr  = wxT("ODBC error during GetColumns()\n\n");
        tStr += GetExtendedDBErrorMsg(pDb,__FILE__,__LINE__);
        wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        return;
    }

    int i;
    for (i = 0; colInf[i].colName && wxStrlen(colInf[i].colName); i++)
    {
        // If there is more than one table being queried, qualify
        // the column names with the table name prefix.
        if (tblName[1] && wxStrlen(tblName[1]))
        {
            qualName.Printf(wxT("%s.%s"), colInf[i].tableName, colInf[i].colName);
            pQueryCol1Choice->Append(qualName);
            pQueryCol2Choice->Append(qualName);
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

    pQuerySqlWhereMtxt->SetValue(pWhere.c_str());

    wxEndBusyCursor();

    // Display the dialog window
    Centre(wxBOTH);
    ShowModal();
}  // CqueryDlg() constructor


CqueryDlg::~CqueryDlg()
{
}  // CqueryDlg::~CqueryDlg() destructor


void CqueryDlg::OnButton(wxCommandEvent &event)
{
  wxWindow *win = (wxWindow*) event.GetEventObject();
  OnCommand( *win, event );
}  // CqueryDlg::OnButton()


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
                if (pQueryCol2Choice->GetSelection())    // Column name is highlighted
                {
                    pQueryValue1Msg->Show(FALSE);
                    pQueryValue1Txt->Show(FALSE);
                }
                else                                                // "Value" is highlighted
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
        if (pQueryCol2Choice->GetSelection())    // Column name is highlighted
        {
            pQueryValue1Msg->Show(FALSE);
            pQueryValue1Txt->Show(FALSE);
        }
        else                                                // "Value" is highlighted
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
        AppendToWhere(wxT(" AND\n"));
        return;
    }  // And button

    // Or button
    if (widgetName == pQueryOrBtn->GetName())
    {
        AppendToWhere(wxT(" OR\n"));
        return;
    }  // Or button

    // Left Paren button
    if (widgetName == pQueryLParenBtn->GetName())
    {
        AppendToWhere(wxT("("));
        return;
    }  // Left Paren button

    // Right paren button
    if (widgetName == pQueryRParenBtn->GetName())
    {
        AppendToWhere(wxT(")"));
        return;
    }  // Right Paren button

    // Done button
    if (widgetName == pQueryDoneBtn->GetName())
    {
        // Be sure the where clause will not overflow the output buffer
        if (wxStrlen(pQuerySqlWhereMtxt->GetValue()) > (unsigned int)DB_MAX_WHERE_CLAUSE_LEN)
        {
            wxString s;
            s.Printf(wxT("Maximum where clause length exceeded.\nLength must be less than %d"), DB_MAX_WHERE_CLAUSE_LEN+1);
            wxMessageBox(s,wxT("Error..."),wxOK | wxICON_EXCLAMATION);
            return;
        }
        // Validate the where clause for things such as matching parens
        if (!ValidateWhereClause())
            return;
        // Copy the where clause to the output buffer and exit
        pWhere = pQuerySqlWhereMtxt->GetValue();
        Close();
        return;
    }  // Done button

    // Clear button
    if (widgetName == pQueryClearBtn->GetName())
    {
        bool Ok = (wxMessageBox(wxT("Are you sure you wish to clear the Query?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

        if (Ok)
            pQuerySqlWhereMtxt->SetValue(wxT(""));
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


void CqueryDlg::OnCloseWindow(wxCloseEvent& event)
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
    while (wxIsBusy())
        wxEndBusyCursor();

    Show(FALSE);
    SetReturnCode(1);  // added so BoundsChecker would not report use of uninitialized variable

    this->Destroy();
}  // CqueryDlg::OnCloseWindow()


void CqueryDlg::AppendToWhere(wxChar *s)
{
    wxString whereStr = pQuerySqlWhereMtxt->GetValue();
    whereStr += s;
    pQuerySqlWhereMtxt->SetValue(whereStr);
}  // CqueryDlg::AppendToWhere()


void CqueryDlg::ProcessAddBtn()
{
    qryOp oper = (qryOp) pQueryOperatorChoice->GetSelection();

    // Verify that eveything is filled in correctly
    if (pQueryCol2Choice->GetSelection() == 0)            // "Value" is selected
    {
        // Verify that value 1 is filled in
        if (wxStrlen(pQueryValue1Txt->GetValue()) == 0)
        {
            wxBell();
            pQueryValue1Txt->SetFocus();
            return;
        }
        // For the BETWEEN operator, value 2 must be filled in as well
        if (oper == qryOpBETWEEN &&
             wxStrlen(pQueryValue2Txt->GetValue()) == 0)
        {
            wxBell();
            pQueryValue2Txt->SetFocus();
            return;
        }
    }

    // Build the expression and append it to the where clause window
    wxString s = pQueryCol1Choice->GetStringSelection();
    
    if (pQueryNotCheck->GetValue() && (oper != qryOpEQ))
        s += wxT(" NOT");
    
    switch(oper)
    {
    case qryOpEQ:
        if (pQueryNotCheck->GetValue())    // NOT box is checked
            s += wxT(" <>");
        else
            s += wxT(" =");
        break;
    case qryOpLT:
        s += wxT(" <");
        break;
    case qryOpGT:
        s += wxT(" >");
        break;
    case qryOpLE:
        s += wxT(" <=");
        break;
    case qryOpGE:
        s += wxT(" >=");
        break;
    case qryOpBEGINS:
    case qryOpCONTAINS:
    case qryOpLIKE:
        s += wxT(" LIKE");
        break;
    case qryOpBETWEEN:
        s += wxT(" BETWEEN");
        break;
    }

    s += wxT(" ");

    int col1Idx = pQueryCol1Choice->GetSelection();

    bool quote = FALSE;
    if (colInf[col1Idx].sqlDataType == SQL_VARCHAR  ||
        oper == qryOpBEGINS                         ||
        oper == qryOpCONTAINS                       ||
        oper == qryOpLIKE)
        quote = TRUE;

    if (pQueryCol2Choice->GetSelection())    // Column name
        s += pQueryCol2Choice->GetStringSelection();
    else  // Column 2 is a "value"
    {
        if (quote)
            s += wxT("'");
        if (oper == qryOpCONTAINS)
            s += wxT("%");
        s += pQueryValue1Txt->GetValue();
        if (oper == qryOpCONTAINS || oper == qryOpBEGINS)
            s += wxT("%");
        if (quote)
            s += wxT("'");
    }

    if (oper == qryOpBETWEEN)
    {
        s += wxT(" AND ");
        if (quote)
            s += wxT("'");
        s += pQueryValue2Txt->GetValue();
        if (quote)
            s += wxT("'");
    }

    AppendToWhere((wxChar*) (const wxChar*) s);

}  // CqueryDlg::ProcessAddBtn()


void CqueryDlg::ProcessCountBtn()
{
    if (!ValidateWhereClause())
        return;

    if (!dbTable)  // wxDbTable object needs to be created and opened
    {
        dbTable = new wxDbTable(pDB, masterTableName, 0, wxT(""),
                                !wxDB_QUERY_ONLY, 
                                wxGetApp().DbConnectInf->GetDefaultDir());
        if (!dbTable)
        {
            wxMessageBox(wxT("Memory allocation failed creating a wxDbTable object."),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
            return;
        }
        if (!dbTable->Open())
        {
            wxString tStr;
            tStr  = wxT("ODBC error during Open()\n\n");
            tStr += GetExtendedDBErrorMsg(dbTable->GetDb(),__FILE__,__LINE__);
            wxMessageBox(tStr,wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
            return;
        }
    }

    // Count() with WHERE clause
    wxString whereStr;

    whereStr = pQuerySqlWhereMtxt->GetValue();
    dbTable->SetWhereClause(whereStr.c_str());

    ULONG whereCnt = dbTable->Count();

    // Count() of all records in the table
    dbTable->SetWhereClause(wxT(""));
    ULONG totalCnt = dbTable->Count();

    if (whereCnt > 0 || totalCnt == 0)
    {
        wxString tStr;
        tStr.Printf(wxT("%lu of %lu records match the query criteria."),whereCnt,totalCnt);
        wxMessageBox(tStr,wxT("Notice..."),wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxString tStr;
        tStr.Printf(wxT("%lu of %lu records match the query criteria.\n\nEither the criteria entered produced a result set\nwith no records, or there was a syntactical error\nin the clause you entered.\n\nPress the details button to see if any database errors were reported."),whereCnt,totalCnt);
        wxMessageBox(tStr,wxT("Notice..."),wxOK | wxICON_INFORMATION);
    }

    // After a wxMessageBox, the focus does not necessarily return to the
    // window which was the focus when the message box popped up, so return
    // focus to the Query dialog for certain
    SetFocus();

}  // CqueryDlg::ProcessCountBtn()


bool CqueryDlg::ValidateWhereClause()
{
    wxString where = pQuerySqlWhereMtxt->GetValue();

    if (where.Freq(wxT('(')) != where.Freq(wxT(')')))
    {
        wxMessageBox(wxT("There are mismatched parenthesis in the constructed where clause"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        return(FALSE);
    }
    // After a wxMessageBox, the focus does not necessarily return to the
    // window which was the focus when the message box popped up, so return
    // focus to the Query dialog for certain
    SetFocus();

    return(TRUE);

}  // CqueryDlg::ValidateWhereClause()




/*
    TEST CODE FOR TESTING THE wxDbCreateDataSource() FUNCTION

        int result = 0;
        result = wxDbCreateDataSource(wxT("Microsoft Access Driver (*.mdb)"),wxT("GLT-TEST2"),wxT("GLT-Descrip"),FALSE,wxT(""),this);
        if (!result)
        {
            // check for errors caused by ConfigDSN based functions
            DWORD retcode = 0;
            WORD cb;
            wxChar errMsg[500+1];
            errMsg[0] = wxT('\0');

            SQLInstallerError(1,&retcode,errMsg,500,&cb);

            wxMessageBox(wxT("FAILED creating data source"),wxT("FAILED"));
        }
        else
            wxMessageBox(wxT("SUCCEEDED creating data source"),wxT("SUCCESS"));
*/


