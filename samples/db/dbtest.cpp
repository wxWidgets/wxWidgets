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
#include  "wx/wx.h"
#endif //WX_PRECOMP

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMAC__)
#include "db.xpm"
#endif

#include <stdio.h>                  /* Included strictly for reading the text file with the database parameters */

//#include "wx/db.h"                  /* Required in the file which will get the data source connection */
//#include "wx/dbtable.h"             /* Has the wxDbTable object from which all data objects will inherit their data table functionality */

//extern wxDbList WXDLLEXPORT *PtrBegDbList;    /* from db.cpp, used in getting back error results from db connections */

#if wxUSE_NEW_GRID
#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/dbgrid.h"

#define CHOICEINT
#endif

#include "dbtest.h"                 /* Header file for this demonstration program */
#include "listdb.h"                 /* Code to support the "Lookup" button on the editor dialog */

IMPLEMENT_APP(DatabaseDemoApp)

extern wxChar ListDB_Selection[];   /* Used to return the first column value for the selected line from the listDB routines */
extern wxChar ListDB_Selection2[];  /* Used to return the second column value for the selected line from the listDB routines */


#if !wxUSE_ODBC
  #error Sample cannot be compiled unless setup.h has wxUSE_ODBC set to 1
#endif


bool DataTypeSupported(wxDb *pDb, SWORD datatype, wxString *nativeDataTypeName)
{
    wxDbSqlTypeInfo sqlTypeInfo;

    bool breakpoint = FALSE;

    *nativeDataTypeName = wxEmptyString;
    if (pDb->GetDataTypeInfo(datatype, sqlTypeInfo))
    {
        *nativeDataTypeName = sqlTypeInfo.TypeName;
        breakpoint = TRUE;
    }

    return breakpoint;

}  // GetDataTypesSupported();



void CheckSupportForAllDataTypes(wxDb *pDb)
{
    wxString nativeDataTypeName;

    wxLogMessage("\nThe following datatypes are supported by the\ndatabase you are currently connected to:");
#ifdef SQL_C_BINARY
    if (DataTypeSupported(pDb,SQL_C_BINARY, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_BINARY (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_BIT
    if (DataTypeSupported(pDb,SQL_C_BIT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_BIT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_BOOKMARK
    if (DataTypeSupported(pDb,SQL_C_BOOKMARK, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_BOOKMARK (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_CHAR
    if (DataTypeSupported(pDb,SQL_C_CHAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_CHAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_DATE
    if (DataTypeSupported(pDb,SQL_C_DATE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_DATE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_DEFAULT
    if (DataTypeSupported(pDb,SQL_C_DEFAULT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_DEFAULT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_DOUBLE
    if (DataTypeSupported(pDb,SQL_C_DOUBLE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_DOUBLE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_FLOAT
    if (DataTypeSupported(pDb,SQL_C_FLOAT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_FLOAT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_GUID
    if (DataTypeSupported(pDb,SQL_C_GUID, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_GUID (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_DAY
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_DAY, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_DAY (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_HOUR
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_HOUR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_DAY_TO_HOUR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_MINUTE
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_MINUTE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_DAY_TO_MINUTE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_DAY_TO_SECOND
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_DAY_TO_SECOND, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_DAY_TO_SECOND (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_HOUR
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_HOUR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_HOUR_TO_MINUTE
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR_TO_MINUTE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_HOUR_TO_MINUTE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_HOUR_TO_SECOND
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_HOUR_TO_SECOND, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_HOUR_TO_SECOND (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_MINUTE
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_MINUTE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_MINUTE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_MINUTE_TO_SECOND
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_MINUTE_TO_SECOND, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_MINUTE_TO_SECOND (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_MONTH
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_MONTH, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_MONTH (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_SECOND
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_SECOND, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_SECOND (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_YEAR
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_YEAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_YEAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_INTERVAL_YEAR_TO_MONTH
    if (DataTypeSupported(pDb,SQL_C_INTERVAL_YEAR_TO_MONTH, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_INTERVAL_YEAR_TO_MONTH (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_LONG
    if (DataTypeSupported(pDb,SQL_C_LONG, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_LONG (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_NUMERIC
    if (DataTypeSupported(pDb,SQL_C_NUMERIC, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_NUMERIC (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_SBIGINT
    if (DataTypeSupported(pDb,SQL_C_SBIGINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_SBIGINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_SHORT
    if (DataTypeSupported(pDb,SQL_C_SHORT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_SHORT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_SLONG
    if (DataTypeSupported(pDb,SQL_C_SLONG, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_SLONG (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_SSHORT
    if (DataTypeSupported(pDb,SQL_C_SSHORT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_SSHORT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_STINYINT
    if (DataTypeSupported(pDb,SQL_C_STINYINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_STINYINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TIME
    if (DataTypeSupported(pDb,SQL_C_TIME, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TIME (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TIMESTAMP
    if (DataTypeSupported(pDb,SQL_C_TIMESTAMP, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TIMESTAMP (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TINYINT
    if (DataTypeSupported(pDb,SQL_C_TINYINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TINYINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TYPE_DATE
    if (DataTypeSupported(pDb,SQL_C_TYPE_DATE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TYPE_DATE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TYPE_TIME
    if (DataTypeSupported(pDb,SQL_C_TYPE_TIME, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TYPE_TIME (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_TYPE_TIMESTAMP
    if (DataTypeSupported(pDb,SQL_C_TYPE_TIMESTAMP, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TYPE_TIMESTAMP (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_UBIGINT
    if (DataTypeSupported(pDb,SQL_C_UBIGINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_UBIGINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_ULONG
    if (DataTypeSupported(pDb,SQL_C_ULONG, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_ULONG (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_USHORT
    if (DataTypeSupported(pDb,SQL_C_USHORT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_USHORT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_UTINYINT
    if (DataTypeSupported(pDb,SQL_C_UTINYINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_UTINYINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_C_VARBOOKMARK
    if (DataTypeSupported(pDb,SQL_C_VARBOOKMARK, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_VARBOOKMARK (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif

// Extended SQL types
#ifdef SQL_DATE
    if (DataTypeSupported(pDb,SQL_DATE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_DATE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_INTERVAL
    if (DataTypeSupported(pDb,SQL_INTERVAL, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_INTERVAL (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_TIME
    if (DataTypeSupported(pDb,SQL_TIME, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_TIME (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_TIMESTAMP
    if (DataTypeSupported(pDb,SQL_TIMESTAMP, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_TIMESTAMP (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_LONGVARCHAR
    if (DataTypeSupported(pDb,SQL_LONGVARCHAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_LONGVARCHAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_BINARY
    if (DataTypeSupported(pDb,SQL_BINARY, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_BINARY (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_VARBINARY
    if (DataTypeSupported(pDb,SQL_VARBINARY, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_VARBINARY (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_LONGVARBINARY
    if (DataTypeSupported(pDb,SQL_LONGVARBINARY, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_LOGVARBINARY (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_BIGINT
    if (DataTypeSupported(pDb,SQL_BIGINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_BIGINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_TINYINT
    if (DataTypeSupported(pDb,SQL_TINYINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_TINYINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_BIT
    if (DataTypeSupported(pDb,SQL_BIT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_BIT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_GUID
    if (DataTypeSupported(pDb,SQL_GUID, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_GUID (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif

#ifdef SQL_CHAR
    if (DataTypeSupported(pDb,SQL_CHAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_CHAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_INTEGER
    if (DataTypeSupported(pDb,SQL_INTEGER, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_INTEGER (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_SMALLINT
    if (DataTypeSupported(pDb,SQL_SMALLINT, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_SAMLLINT (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_REAL
    if (DataTypeSupported(pDb,SQL_REAL, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_REAL (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_DOUBLE
    if (DataTypeSupported(pDb,SQL_DOUBLE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_DOUBLE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_NUMERIC
    if (DataTypeSupported(pDb,SQL_NUMERIC, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_NUMERIC (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_DATE
    if (DataTypeSupported(pDb,SQL_DATE, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_DATE (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_TIME
    if (DataTypeSupported(pDb,SQL_TIME, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_TIME (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_TIMESTAMP
    if (DataTypeSupported(pDb,SQL_TIMESTAMP, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_TIMESTAMP (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif
#ifdef SQL_VARCHAR
    if (DataTypeSupported(pDb,SQL_VARCHAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_VARCHAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif

// UNICODE
#ifdef SQL_C_TCHAR
    if (DataTypeSupported(pDb,SQL_C_TCHAR, &nativeDataTypeName))
    {
        nativeDataTypeName = "SQL_C_TCHAR (" + nativeDataTypeName;
        nativeDataTypeName += ")";
        wxLogMessage(nativeDataTypeName);
    }
#endif

    wxLogMessage("\n");
}  // CheckSupportForAllDataTypes()


bool DatabaseDemoApp::OnInit()
{
    DbConnectInf    = NULL;
    Contact         = NULL;

    // Create the main frame window
    DemoFrame = new DatabaseDemoFrame(NULL, wxT("wxWindows Database Demo"), wxPoint(50, 50), wxSize(537, 480));

    // Give it an icon
    DemoFrame->SetIcon(wxICON(db));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(FILE_CREATE, wxT("&Create CONTACT table"));
    file_menu->Append(FILE_RECREATE_TABLE, wxT("&Recreate CONTACT table"));
    file_menu->Append(FILE_RECREATE_INDEXES, wxT("&Recreate CONTACT indexes"));
#if wxUSE_NEW_GRID
    file_menu->Append(FILE_DBGRID_TABLE,  wxT("&Open DB Grid example"));
#endif
    file_menu->Append(FILE_EXIT, wxT("E&xit"));

    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(EDIT_PARAMETERS, wxT("&Parameters..."));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(HELP_ABOUT, wxT("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(edit_menu, wxT("&Edit"));
    menu_bar->Append(help_menu, wxT("&Help"));
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
        wxDELETE(DbConnectInf);
    }

    if (!ReadParamFile(params))
        DemoFrame->BuildParameterDialog(NULL);

    if (!wxStrlen(params.ODBCSource))
    {
        wxDELETE(DbConnectInf);
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
        wxDELETE(DbConnectInf);
        wxMessageBox(wxT("Now exiting program.\n\nRestart program to try any new settings."),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        return(FALSE);
    }

    DemoFrame->BuildEditorDialog();

    // Show the frame
    DemoFrame->Refresh();

    return TRUE;
}  // DatabaseDemoApp::OnInit()


/*
* Remove CR or CR/LF from a character string.
*/
char* wxRemoveLineTerminator(char* aString)
{
    int len = strlen(aString);
    while (len > 0 && (aString[len-1] == '\r' || aString[len-1] == '\n')) {
        aString[len-1] = '\0';
        len--;
    }
    return aString;
}


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
    wxRemoveLineTerminator(buffer);
    wxStrcpy(params.ODBCSource,buffer);

    fgets(buffer, sizeof(params.UserName), paramFile);
    wxRemoveLineTerminator(buffer);
    wxStrcpy(params.UserName,buffer);

    fgets(buffer, sizeof(params.Password), paramFile);
    wxRemoveLineTerminator(buffer);
    wxStrcpy(params.Password,buffer);

    fgets(buffer, sizeof(params.DirPath), paramFile);
    wxRemoveLineTerminator(buffer);
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

    Contact->GetDb()->RollbackTrans();  // Make sure the current cursor is in a known/stable state

    if (!Contact->CreateTable(recreate))
    {
        wxEndBusyCursor();
        wxString tStr;
        tStr = wxT("Error creating CONTACTS table.\nTable was not created.\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

        success = FALSE;
    }
    else
    {
        if (!Contact->CreateIndexes(recreate))
        {
            wxEndBusyCursor();
            wxString tStr;
            tStr = wxT("Error creating CONTACTS indexes.\nIndexes will be unavailable.\n\n");
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

            success = FALSE;
        }
    }
    while (wxIsBusy())
        wxEndBusyCursor();

    if (success)
        wxMessageBox(wxT("Table and index(es) were successfully created."),wxT("Notice..."),wxOK | wxICON_INFORMATION);
}  // DatabaseDemoApp::CreateDataTable()


BEGIN_EVENT_TABLE(DatabaseDemoFrame, wxFrame)
    EVT_MENU(FILE_CREATE, DatabaseDemoFrame::OnCreate)
    EVT_MENU(FILE_RECREATE_TABLE, DatabaseDemoFrame::OnRecreateTable)
    EVT_MENU(FILE_RECREATE_INDEXES, DatabaseDemoFrame::OnRecreateIndexes)
#if wxUSE_NEW_GRID
    EVT_MENU(FILE_DBGRID_TABLE, DatabaseDemoFrame::OnDbGridTable)
#endif
    EVT_MENU(FILE_EXIT, DatabaseDemoFrame::OnExit)
    EVT_MENU(EDIT_PARAMETERS, DatabaseDemoFrame::OnEditParameters)
    EVT_MENU(HELP_ABOUT, DatabaseDemoFrame::OnAbout)
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

    delete wxLog::SetActiveTarget(new wxLogStderr);

}  // DatabaseDemoFrame constructor

DatabaseDemoFrame::~DatabaseDemoFrame()
{
    delete wxLog::SetActiveTarget(NULL);
}  // DatabaseDemoFrame destructor


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
    wxGetApp().Contact->GetDb()->RollbackTrans();  // Make sure the current cursor is in a known/stable state

    if (!wxGetApp().Contact->CreateIndexes(TRUE))
    {
        while (wxIsBusy())
            wxEndBusyCursor();
        wxString tStr;
        tStr = wxT("Error creating CONTACTS indexes.\nNew indexes will be unavailable.\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

    }
     else
        wxMessageBox(wxT("Index(es) were successfully recreated."),wxT("Notice..."),wxOK | wxICON_INFORMATION);

}  // DatabaseDemoFrame::OnRecreateIndexes()


#if wxUSE_NEW_GRID
void DatabaseDemoFrame::OnDbGridTable(wxCommandEvent& )
{
    DbGridFrame *frame = new DbGridFrame(this);
    if (frame->Initialize())
    {
        frame->Show();
    }
}
#endif

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

    wxDELETE(wxGetApp().Contact);

    // This function will close all the connections to the database that have been
    // previously cached.
    wxDbCloseConnections();

    // Deletion of the wxDbConnectInf instance must be the LAST thing done that
    // has anything to do with the database.  Deleting this before disconnecting,
    // freeing/closing connections, etc will result in a crash!
    wxDELETE(wxGetApp().DbConnectInf);

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
            tStr = wxT("Unable to Free the Ccontact data table handle\n\n");

            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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
    SetColDefs ( 9,wxT("CONTRIBS"),   DB_DATA_TYPE_INTEGER,    &Contributions,  SQL_C_UTINYINT,             sizeof(Contributions),  FALSE,TRUE);
    SetColDefs (10,wxT("LINE_CNT"),   DB_DATA_TYPE_INTEGER,    &LinesOfCode,    SQL_C_ULONG,                sizeof(LinesOfCode),    FALSE,TRUE);
    SetColDefs (11,wxT("LANGUAGE"),   DB_DATA_TYPE_INTEGER,    &NativeLanguage, SQL_C_ENUM,                 sizeof(NativeLanguage), FALSE,TRUE);
#if wxODBC_BLOB_EXPERIMENT > 0
    SetColDefs (12,wxT("PICTURE"),    DB_DATA_TYPE_BLOB,        Picture,        SQL_C_BINARY,          sizeof(Picture),        FALSE,TRUE);
#endif
}  // Ccontact::SetupColumns


bool Ccontact::CreateIndexes(bool recreate)
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
    Ok = CreateIndex(indexName.c_str(), TRUE, 2, idxDef, recreate);

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

    Show(FALSE);
}  // CeditorDlg constructor


void CeditorDlg::OnCloseWindow(wxCloseEvent& event)
{
    // Clean up time
    if ((mode != mCreate) && (mode != mEdit))
    {
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
        wxGetApp().Contact->Initialize();
        PutData();
        SetMode( mCreate );
        pNameTxt->SetValue(wxT(""));
        pNameTxt->SetFocus();
        return;
    }

    if (widgetName == pEditBtn->GetName())
    {
        saveName = wxGetApp().Contact->Name;
        SetMode( mEdit );
        pNameTxt->SetFocus();
        return;
    }

    if (widgetName == pCopyBtn->GetName())
    {
        SetMode(mCreate);
        pNameTxt->SetValue(wxT(""));
        pNameTxt->SetFocus();
        return;
    }

    if (widgetName == pDeleteBtn->GetName())
    {
        bool Ok = (wxMessageBox(wxT("Are you sure?"),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

        if (!Ok)
            return;

        if (Ok && wxGetApp().Contact->Delete())
        {
            // NOTE: Deletions are not finalized until a CommitTrans() is performed.  
            //       If the commit were not performed, the program will continue to 
            //       show the table contents as if they were deleted until this instance
            //       of Ccontact is deleted.  If the Commit wasn't performed, the 
            //       database will automatically Rollback the changes when the database
            //       connection is terminated
            wxGetApp().Contact->GetDb()->CommitTrans();

            // Try to get the row that followed the just deleted row in the orderBy sequence
            if (!GetNextRec())
            {
                // There was now row (in sequence) after the just deleted row, so get the
                // row which preceded the just deleted row
                if (!GetPrevRec())
                {
                    // There are now no rows remaining, so clear the dialog widgets
                    wxGetApp().Contact->Initialize();
                    PutData();
                }
            }
            SetMode(mode);    // force reset of button enable/disable
        }
        else
            // Delete failed
            wxGetApp().Contact->GetDb()->RollbackTrans();

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
            wxGetApp().Contact->Initialize();
            PutData();
            SetMode(mView);
            return;
        }
        else
        {
            // Requery previous record
            if (wxGetApp().Contact->FetchByName(saveName))
            {
                PutData();
                SetMode(mView);
                return;
            }
        }

        // Previous record not available, retrieve first record in table
        if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
            wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
        {
            wxGetApp().Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
            wxGetApp().Contact->whereStr += wxGetApp().Contact->GetTableName();
            wxGetApp().Contact->whereStr += wxT(")");
            wxGetApp().Contact->SetWhereClause(wxGetApp().Contact->whereStr.c_str());
        }
        else
            wxGetApp().Contact->SetWhereClause(wxT(""));

        if (!wxGetApp().Contact->Query())
        {
            wxString tStr;
            tStr = wxT("ODBC error during Query()\n\n");
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
            
            SetMode(mView);
            return;
        }
        if (wxGetApp().Contact->GetNext())  // Successfully read first record
        {
            PutData();
            SetMode(mView);
            return;
        }
        // No contacts are available, clear dialog
        wxGetApp().Contact->Initialize();
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
        wxStrcpy(qryWhere, (const wxChar*) wxGetApp().Contact->qryWhereStr);
        wxChar *tblName[] = {(wxChar *)CONTACT_TABLE_NAME, 0};
        new CqueryDlg(GetParent(), wxGetApp().Contact->GetDb(), tblName, qryWhere);

        // Query the first record in the new record set and
        // display it, if the query string has changed.
        if (wxStrcmp(qryWhere, (const wxChar*) wxGetApp().Contact->qryWhereStr))
        {
            wxGetApp().Contact->whereStr.Empty();
            wxGetApp().Contact->SetOrderByClause("NAME");

            if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
                wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
            {
                wxGetApp().Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
                wxGetApp().Contact->whereStr += CONTACT_TABLE_NAME;
            }
            
            // Append the query where string (if there is one)
            wxGetApp().Contact->qryWhereStr  = qryWhere;
            if (wxStrlen(qryWhere))
            {
                wxGetApp().Contact->whereStr += wxT(" WHERE ");
                wxGetApp().Contact->whereStr += wxGetApp().Contact->qryWhereStr;
            }
            // Close the expression with a right paren
            wxGetApp().Contact->whereStr += wxT(")");
            // Requery the table
            wxGetApp().Contact->SetWhereClause(wxGetApp().Contact->whereStr.c_str());
            if (!wxGetApp().Contact->Query())
            {
                wxString tStr;
                tStr = wxT("ODBC error during Query()\n\n");
                wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                             wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

                return;
            }
            // Display the first record from the query set
            if (!wxGetApp().Contact->GetNext())
                wxGetApp().Contact->Initialize();
            PutData();
        }

        // Enable/Disable the reset button
        pResetBtn->Enable(!wxGetApp().Contact->qryWhereStr.IsEmpty());

        return;
    }  // Query button


    if (widgetName == pResetBtn->GetName())
    {
        // Clear the additional where criteria established by the query feature
        wxGetApp().Contact->qryWhereStr = wxT("");
        wxGetApp().Contact->SetOrderByClause(wxT("NAME"));

        if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
            wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
        {
            wxGetApp().Contact->whereStr  = wxT("NAME = (SELECT MIN(NAME) FROM ");
            wxGetApp().Contact->whereStr += CONTACT_TABLE_NAME;
            wxGetApp().Contact->whereStr += wxT(")");
        }

        wxGetApp().Contact->SetWhereClause(wxGetApp().Contact->whereStr.c_str());
        if (!wxGetApp().Contact->Query())
        {
            wxString tStr;
            tStr = wxT("ODBC error during Query()\n\n");
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
            return;
        }
        if (!wxGetApp().Contact->GetNext())
            wxGetApp().Contact->Initialize();
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

    if (widgetName == pDataTypesBtn->GetName())
    {
        CheckSupportForAllDataTypes(wxGetApp().READONLY_DB);
        wxMessageBox("Support datatypes was dumped to stdout.");
        return;
    }  // Data types Button

    if (widgetName == pDbDiagsBtn->GetName())
    {
        DisplayDbDiagnostics(wxGetApp().READONLY_DB);
        wxMessageBox("Diagnostics info was dumped to stdout.");
        return;
    }

    if (widgetName == pCatalogBtn->GetName())
    {
        if (wxGetApp().Contact->GetDb()->Catalog("","catalog.txt"))
            wxMessageBox("The file 'catalog.txt' was created.");
        else
            wxMessageBox("Creation of the file 'catalog.txt' was failed.");
        return;
    }

}  // CeditorDlg::OnCommand()


bool CeditorDlg::Initialize()
{
    // Create the data structure and a new database connection.  
    // (As there is not a pDb being passed in the constructor, a new database
    // connection is created)
    wxGetApp().Contact = new Ccontact();

    if (!wxGetApp().Contact)
    {
        wxMessageBox(wxT("Unable to instantiate an instance of Ccontact"),wxT("Error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    // Check if the table exists or not.  If it doesn't, ask the user if they want to 
    // create the table.  Continue trying to create the table until it exists, or user aborts
    while (!wxGetApp().Contact->GetDb()->TableExists((wxChar *)CONTACT_TABLE_NAME, 
                                          wxGetApp().DbConnectInf->GetUserID(), 
                                          wxGetApp().DbConnectInf->GetDefaultDir()))
    {
        wxString tStr;
        tStr.Printf(wxT("Unable to open the table '%s'.  The table may\nneed to be created.\n\nDo you wish to try to create/clear the table?\n\n"),CONTACT_TABLE_NAME);
        bool createTable = (wxMessageBox(tStr.c_str(),wxT("Confirm"),wxYES_NO|wxICON_QUESTION) == wxYES);

        if (!createTable)
        {
//            Close();
            return FALSE;
        }
        else
            wxGetApp().CreateDataTable(FALSE);
    }

    // Tables must be "opened" before anything other than creating/deleting table can be done
    if (!wxGetApp().Contact->Open())
    {
        // Table does exist, or there was some problem opening it.  Currently this should
        // never fail, except in the case of the table not exisiting or the current
        // user has insufficent privileges to access the table
#if 1
// This code is experimenting with a new function that will hopefully be available
// in the 2.4 release.  This check will determine whether the open failing was due
// to the table not existing, or the users privileges being insufficient to
// open the table.
        if (!wxGetApp().Contact->GetDb()->TablePrivileges(CONTACT_TABLE_NAME, wxT("SELECT"),
                                                wxGetApp().Contact->GetDb()->GetUsername(),
                                                wxGetApp().Contact->GetDb()->GetUsername(),
                                                wxGetApp().DbConnectInf->GetDefaultDir()))
        {
            wxString tStr;
            tStr.Printf(wxT("Unable to open the table '%s' (likely due to\ninsufficient privileges of the logged in user).\n\n"),CONTACT_TABLE_NAME);

            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        }
        else 
#endif
        if (!wxGetApp().Contact->GetDb()->TableExists(CONTACT_TABLE_NAME,
                                           wxGetApp().Contact->GetDb()->GetUsername(),
                                           wxGetApp().DbConnectInf->GetDefaultDir()))
        {
            wxString tStr;
            tStr.Printf(wxT("Unable to open the table '%s' as the table\ndoes not appear to exist in the tablespace available\nto the currently logged in user.\n\n"),CONTACT_TABLE_NAME);
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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

    pCatalogBtn       = new wxButton(this, EDITOR_DIALOG_CATALOG,        wxT("Catalo&g"),         wxPoint(430, 287), wxSize( 70,  35), 0, wxDefaultValidator, wxT("CatalogBtn"));
    pDataTypesBtn     = new wxButton(this, EDITOR_DIALOG_DATATYPES,      wxT("Data&types"),       wxPoint(430, 337), wxSize( 70,  35), 0, wxDefaultValidator, wxT("DataTypesBtn"));
    pDbDiagsBtn       = new wxButton(this, EDITOR_DIALOG_DB_DIAGS,       wxT("DB Dia&gs"),        wxPoint(430, 387), wxSize( 70,  35), 0, wxDefaultValidator, wxT("DbDiagsBtn"));

    // Now that all the widgets on the panel are created, its safe to allow ::OnCommand() to 
    // handle all widget processing
    widgetPtrsSet = TRUE;

    // Setup the orderBy and where clauses to return back a single record as the result set, 
    // as there will only be one record being shown on the dialog at a time, this optimizes
    // network traffic by only returning a one row result
    
    wxGetApp().Contact->SetOrderByClause(wxT("NAME"));  // field name to sort by

    // The wxString "whereStr" is not a member of the wxDbTable object, it is a member variable
    // specifically in the Ccontact class.  It is used here for simpler construction of a varying
    // length string, and then after the string is built, the wxDbTable member variable "where" is
    // assigned the pointer to the constructed string.
    //
    // The constructed where clause below has a sub-query within it "SELECT MIN(NAME) FROM %s" 
    // to achieve a single row (in this case the first name in alphabetical order).
    
    if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
        wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        wxGetApp().Contact->whereStr.Printf(wxT("NAME = (SELECT MIN(NAME) FROM %s)"),
                                            wxGetApp().Contact->GetTableName().c_str());
        // NOTE: (const wxChar*) returns a pointer which may not be valid later, so this is short term use only
        wxGetApp().Contact->SetWhereClause(wxGetApp().Contact->whereStr);
    }
    else
       wxGetApp().Contact->SetWhereClause(wxT(""));

    // Perform the Query to get the result set.  
    // NOTE: If there are no rows returned, that is a valid result, so Query() would return TRUE.  
    //       Only if there is a database error will Query() come back as FALSE
    if (!wxGetApp().Contact->Query())
    {
        wxString tStr;
        tStr = wxT("ODBC error during Query()\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    // Since Query succeeded, now get the row that was returned
    if (!wxGetApp().Contact->GetNext())
        // If the GetNext() failed at this point, then there are no rows to retrieve, 
        // so clear the values in the members of "Contact" so that PutData() blanks the 
        // widgets on the dialog
        wxGetApp().Contact->Initialize();
/*
    wxGetApp().Contact->GetDb()->RollbackTrans();
*/
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
        pEditBtn->Enable( !edit && (wxStrcmp(wxGetApp().Contact->Name,wxT(""))!=0) );
        pDeleteBtn->Enable( !edit && (wxStrcmp(wxGetApp().Contact->Name,wxT(""))!=0) );
        pCopyBtn->Enable( !edit && (wxStrcmp(wxGetApp().Contact->Name,wxT(""))!=0) );
        pSaveBtn->Enable( edit );
        pCancelBtn->Enable( edit );
        pPrevBtn->Enable( !edit );
        pNextBtn->Enable( !edit );
        pQueryBtn->Enable( !edit );
        pResetBtn->Enable( !edit && !wxGetApp().Contact->qryWhereStr.IsEmpty() );
        pNameListBtn->Enable( !edit );
    }

    FieldsEditable();
}  // CeditorDlg::SetMode()


bool CeditorDlg::PutData()
{
    wxString tStr;

    pNameTxt->SetValue(wxGetApp().Contact->Name);
    pAddress1Txt->SetValue(wxGetApp().Contact->Addr1);
    pAddress2Txt->SetValue(wxGetApp().Contact->Addr2);
    pCityTxt->SetValue(wxGetApp().Contact->City);
    pStateTxt->SetValue(wxGetApp().Contact->State);
    pCountryTxt->SetValue(wxGetApp().Contact->Country);
    pPostalCodeTxt->SetValue(wxGetApp().Contact->PostalCode);

    tStr.Printf(wxT("%d/%d/%d"),wxGetApp().Contact->JoinDate.month,wxGetApp().Contact->JoinDate.day,wxGetApp().Contact->JoinDate.year);
    pJoinDateTxt->SetValue(tStr);

    tStr.Printf(wxT("%d"),wxGetApp().Contact->Contributions);
    pContribTxt->SetValue(tStr);

    tStr.Printf(wxT("%lu"),wxGetApp().Contact->LinesOfCode);
    pLinesTxt->SetValue(tStr);

    pNativeLangChoice->SetSelection(wxGetApp().Contact->NativeLanguage);

    pDeveloperRadio->SetSelection(wxGetApp().Contact->IsDeveloper);

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
    int    mm = 1,dd = 1,yyyy = 2001;
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
        wxGetApp().Contact->JoinDate.month = mm;
        wxGetApp().Contact->JoinDate.day   = dd;
        wxGetApp().Contact->JoinDate.year  = yyyy;
    }
    else
    {
        wxMessageBox(wxT("Improper date format.  Please check the date\nspecified and try again.\n\nNOTE: Dates are in american format (MM/DD/YYYY)"),wxT("Notice..."),wxOK | wxICON_INFORMATION);
        return FALSE;
    }

    tStr = pNameTxt->GetValue();
    wxStrcpy(wxGetApp().Contact->Name,(const wxChar*) tStr);
    wxStrcpy(wxGetApp().Contact->Addr1,pAddress1Txt->GetValue());
    wxStrcpy(wxGetApp().Contact->Addr2,pAddress2Txt->GetValue());
    wxStrcpy(wxGetApp().Contact->City,pCityTxt->GetValue());
    wxStrcpy(wxGetApp().Contact->State,pStateTxt->GetValue());
    wxStrcpy(wxGetApp().Contact->Country,pCountryTxt->GetValue());
    wxStrcpy(wxGetApp().Contact->PostalCode,pPostalCodeTxt->GetValue());

    wxGetApp().Contact->Contributions = atoi(pContribTxt->GetValue());
    wxGetApp().Contact->LinesOfCode = atol(pLinesTxt->GetValue());

    wxGetApp().Contact->NativeLanguage = (enum Language) pNativeLangChoice->GetSelection();
    wxGetApp().Contact->IsDeveloper = pDeveloperRadio->GetSelection() > 0;

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
            RETCODE result = wxGetApp().Contact->Insert();

            failed = (result != DB_SUCCESS);
            if (failed)
            {
                // Some errors may be expected, like a duplicate key, so handle those instances with
                // specific error messages.
                if (result == DB_ERR_INTEGRITY_CONSTRAINT_VIOL)
                {
                    wxString tStr;
                    tStr = wxT("A duplicate key value already exists in the table.\nUnable to save record\n\n");
                    wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                                 wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                }
                else
                {
                    // Some other unexpected error occurred
                    wxString tStr;
                    tStr = wxT("Database insert failed\n\n");
                    wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                                 wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                }
            }
        }
        else  // mode == mEdit
        {
            wxGetApp().Contact->GetDb()->RollbackTrans();
            wxGetApp().Contact->whereStr.Printf("NAME = '%s'",saveName.c_str());
            if (!wxGetApp().Contact->UpdateWhere(wxGetApp().Contact->whereStr))
            {
                wxString tStr;
                tStr = wxT("Database update failed\n\n");
                wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                             wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
                failed = TRUE;
            }
        }

        if (!failed)
        {
            wxGetApp().Contact->GetDb()->CommitTrans();
            SetMode(mView);  // Sets the dialog mode back to viewing after save is successful
        }
        else
            wxGetApp().Contact->GetDb()->RollbackTrans();

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

    if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
        wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        w  = wxT("NAME = (SELECT MIN(NAME) FROM ");
        w += wxGetApp().Contact->GetTableName();
        w += wxT(" WHERE NAME > '");
    }
    else
        w = wxT("(NAME > '");

    w += wxGetApp().Contact->Name;
    w += wxT("'");

    // If a query where string is currently set, append that criteria
    if (!wxGetApp().Contact->qryWhereStr.IsEmpty())
    {
        w += wxT(" AND (");
        w += wxGetApp().Contact->qryWhereStr;
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

    if (wxGetApp().Contact->GetDb()->Dbms() != dbmsPOSTGRES &&
        wxGetApp().Contact->GetDb()->Dbms() != dbmsMY_SQL)
    {
        w  = wxT("NAME = (SELECT MAX(NAME) FROM ");
        w += wxGetApp().Contact->GetTableName();
        w += wxT(" WHERE NAME < '");
    }
    else
        w = wxT("(NAME < '");

    w += wxGetApp().Contact->Name;
    w += wxT("'");

    // If a query where string is currently set, append that criteria
    if (!wxGetApp().Contact->qryWhereStr.IsEmpty())
    {
        w += wxT(" AND (");
        w += wxGetApp().Contact->qryWhereStr;
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
    wxGetApp().Contact->SetWhereClause(whereStr);
    wxGetApp().Contact->SetOrderByClause(wxT("NAME"));

    if (!wxGetApp().Contact->Query())
    {
        wxString tStr;
        tStr = wxT("ODBC error during Query()\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);

        return(FALSE);
    }

    if (wxGetApp().Contact->GetNext())
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
    {
        int index = pParamODBCSourceList->FindString(wxGetApp().params.ODBCSource);
        if (index != -1)
            pParamODBCSourceList->SetSelection(index);
    }
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

    wxDELETEA(p);
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
        tStr = wxT("ODBC error during GetColumns()\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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
    wxDELETEA(colInf);

    wxDELETE(dbTable);

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
            tStr = wxT("ODBC error during Open()\n\n");
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
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



void DisplayDbDiagnostics(wxDb *pDb)
{
    wxString s, t;
    bool comma = FALSE;

    s = langDBINF_DB_NAME;
    s += pDb->dbInf.dbmsName;
    s += "\n";

    s += langDBINF_DB_VER;
    s += pDb->dbInf.dbmsVer;
    s += "\n";

    s += langDBINF_DRIVER_NAME;
    s += pDb->dbInf.driverName;
    s += "\n";

    s += langDBINF_DRIVER_ODBC_VER;
    s += pDb->dbInf.odbcVer;
    s += "\n";

    s += langDBINF_DRIVER_MGR_ODBC_VER;
    s += pDb->dbInf.drvMgrOdbcVer;
    s += "\n";

    s += langDBINF_DRIVER_VER;
    s += pDb->dbInf.driverVer;
    s += "\n";

    s += langDBINF_SERVER_NAME;
    s += pDb->dbInf.serverName;
    s += "\n";

    s += langDBINF_FILENAME;
    s += pDb->dbInf.databaseName;
    s += "\n";

    s += langDBINF_OUTER_JOINS;
    s += pDb->dbInf.outerJoins;
    s += "\n";

    s += langDBINF_STORED_PROC;
    s += pDb->dbInf.procedureSupport;
    s += "\n";

    if (pDb->dbInf.maxConnections)
        t.sprintf("%s%d\n", langDBINF_MAX_HDBC, pDb->dbInf.maxConnections);
    else
        t.sprintf("%s%s\n", langDBINF_MAX_HDBC, langDBINF_UNLIMITED);
    s += t;

    if (pDb->dbInf.maxStmts)
        t.sprintf("%s%d\n", langDBINF_MAX_HSTMT, pDb->dbInf.maxStmts);
    else
        t.sprintf("%s%s\n", langDBINF_MAX_HSTMT, langDBINF_UNLIMITED);
    s += t;

    s += langDBINF_API_LVL;
    switch(pDb->dbInf.apiConfLvl)
    {
        case SQL_OAC_NONE:  s += langDBINF_NONE;        break;
        case SQL_OAC_LEVEL1:    s += langDBINF_LEVEL1;  break;
        case SQL_OAC_LEVEL2:    s += langDBINF_LEVEL2;  break;
    }
    s += "\n";

    s += langDBINF_CLI_LVL;
    switch(pDb->dbInf.cliConfLvl)
    {
        case SQL_OSCC_NOT_COMPLIANT:    s += langDBINF_NOT_COMPLIANT;   break;
        case SQL_OSCC_COMPLIANT:        s += langDBINF_COMPLIANT;           break;
    }
    s += "\n";

    s += langDBINF_SQL_LVL;
    switch(pDb->dbInf.sqlConfLvl)
    {
        case SQL_OSC_MINIMUM:   s += langDBINF_MIN_GRAMMAR; break;
        case SQL_OSC_CORE:      s += langDBINF_CORE_GRAMMAR;    break;
        case SQL_OSC_EXTENDED:  s += langDBINF_EXT_GRAMMAR; break;
    }
    s += "\n";

    s += langDBINF_COMMIT_BEHAVIOR;
    switch(pDb->dbInf.cursorCommitBehavior)
    {
        case SQL_CB_DELETE:     s += langDBINF_DELETE_CURSORS;  break;
        case SQL_CB_CLOSE:      s += langDBINF_CLOSE_CURSORS;       break;
        case SQL_CB_PRESERVE:   s += langDBINF_PRESERVE_CURSORS;    break;
    }
    s += "\n";

    s += langDBINF_ROLLBACK_BEHAVIOR;
    switch(pDb->dbInf.cursorRollbackBehavior)
    {
        case SQL_CB_DELETE:     s += langDBINF_DELETE_CURSORS;  break;
        case SQL_CB_CLOSE:      s += langDBINF_CLOSE_CURSORS;       break;
        case SQL_CB_PRESERVE:   s += langDBINF_PRESERVE_CURSORS;    break;
    }
    s += "\n";

    s += langDBINF_SUPP_NOT_NULL;
    switch(pDb->dbInf.supportNotNullClause)
    {
        case SQL_NNC_NULL:      s += langNO;    break;
        case SQL_NNC_NON_NULL:  s += langYES;   break;
    }
    s += "\n";

    s += langDBINF_SUPP_IEF;
    s += pDb->dbInf.supportIEF;
    s += "\n";

    // DEFAULT setting for "Transaction Isolation Level"
    s += langDBINF_TXN_ISOLATION;
    switch(pDb->dbInf.txnIsolation)
    {
        case SQL_TXN_READ_UNCOMMITTED:  s += langDBINF_READ_UNCOMMITTED;    break;
        case SQL_TXN_READ_COMMITTED:        s += langDBINF_READ_COMMITTED;  break;
        case SQL_TXN_REPEATABLE_READ:       s += langDBINF_REPEATABLE_READ; break;
        case SQL_TXN_SERIALIZABLE:          s += langDBINF_SERIALIZABLE;        break;
#ifdef ODBC_V20
        case SQL_TXN_VERSIONING:            s += langDBINF_VERSIONING;          break;
#endif
    }
    s += "\n";

    // CURRENT setting for "Transaction Isolation Level"
    long txnIsoLvl;
    s += langDBINF_TXN_ISOLATION_CURR;
    if (SQLGetConnectOption(pDb->GetHDBC(),SQL_TXN_ISOLATION,&txnIsoLvl) == SQL_SUCCESS)
    {
        switch(txnIsoLvl)
        {
            case SQL_TXN_READ_UNCOMMITTED:  s += langDBINF_READ_UNCOMMITTED;    break;
            case SQL_TXN_READ_COMMITTED:        s += langDBINF_READ_COMMITTED;  break;
            case SQL_TXN_REPEATABLE_READ:       s += langDBINF_REPEATABLE_READ; break;
            case SQL_TXN_SERIALIZABLE:          s += langDBINF_SERIALIZABLE;        break;
#ifdef ODBC_V20
            case SQL_TXN_VERSIONING:            s += langDBINF_VERSIONING;          break;
#endif
        }
    }
    s += "\n";

#ifdef __VMS__
#pragma message disable incboodep
#endif
    comma = FALSE;
    s += langDBINF_TXN_ISOLATION_OPTS;
    if (pDb->dbInf.txnIsolationOptions & SQL_TXN_READ_UNCOMMITTED)
        {s += langDBINF_READ_UNCOMMITTED; comma++;}
    if (pDb->dbInf.txnIsolationOptions & SQL_TXN_READ_COMMITTED)
        {if (comma++) s += ", "; s += langDBINF_READ_COMMITTED;}
    if (pDb->dbInf.txnIsolationOptions & SQL_TXN_REPEATABLE_READ)
        {if (comma++) s += ", "; s += langDBINF_REPEATABLE_READ;}
    if (pDb->dbInf.txnIsolationOptions & SQL_TXN_SERIALIZABLE)
        {if (comma++) s += ", "; s += langDBINF_SERIALIZABLE;}
#ifdef ODBC_V20
    if (pDb->dbInf.txnIsolationOptions & SQL_TXN_VERSIONING)
        {if (comma++) s += ", "; s += langDBINF_VERSIONING;}
#endif
    s += "\n";

    comma = FALSE;
    s += langDBINF_FETCH_DIRS;
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_NEXT)
        {s += langDBINF_NEXT; comma++;}
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_PRIOR)
        {if (comma++) s += ", "; s += langDBINF_PREV;}
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_FIRST)
        {if (comma++) s += ", "; s += langDBINF_FIRST;}
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_LAST)
        {if (comma++) s += ", "; s += langDBINF_LAST;}
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_ABSOLUTE)
        {if (comma++) s += ", "; s += langDBINF_ABSOLUTE;}
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_RELATIVE)
        {if (comma++) s += ", "; s += langDBINF_RELATIVE;}
#ifdef ODBC_V20
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_RESUME)
        {if (comma++) s += ", "; s += langDBINF_RESUME;}
#endif
    if (pDb->dbInf.fetchDirections & SQL_FD_FETCH_BOOKMARK)
        {if (comma++) s += ", "; s += langDBINF_BOOKMARK;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_LOCK_TYPES;
    if (pDb->dbInf.lockTypes & SQL_LCK_NO_CHANGE)
        {s += langDBINF_NO_CHANGE; comma++;}
    if (pDb->dbInf.lockTypes & SQL_LCK_EXCLUSIVE)
        {if (comma++) s += ", "; s += langDBINF_EXCLUSIVE;}
    if (pDb->dbInf.lockTypes & SQL_LCK_UNLOCK)
        {if (comma++) s += ", "; s += langDBINF_UNLOCK;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_POS_OPERS;
    if (pDb->dbInf.posOperations & SQL_POS_POSITION)
        {s += langDBINF_POSITION; comma++;}
    if (pDb->dbInf.posOperations & SQL_POS_REFRESH)
        {if (comma++) s += ", "; s += langDBINF_REFRESH;}
    if (pDb->dbInf.posOperations & SQL_POS_UPDATE)
        {if (comma++) s += ", "; s += langDBINF_UPD;}
    if (pDb->dbInf.posOperations & SQL_POS_DELETE)
        {if (comma++) s += ", "; s += langDBINF_DEL;}
    if (pDb->dbInf.posOperations & SQL_POS_ADD)
        {if (comma++) s += ", "; s += langDBINF_ADD;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_POS_STMTS;
    if (pDb->dbInf.posStmts & SQL_PS_POSITIONED_DELETE)
        {s += langDBINF_POS_DEL; comma++;}
    if (pDb->dbInf.posStmts & SQL_PS_POSITIONED_UPDATE)
        {if (comma++) s += ", "; s += langDBINF_POS_UPD;}
    if (pDb->dbInf.posStmts & SQL_PS_SELECT_FOR_UPDATE)
        {if (comma++) s += ", "; s += langDBINF_SELECT_FOR_UPD;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_SCROLL_CONCURR;
    if (pDb->dbInf.scrollConcurrency & SQL_SCCO_READ_ONLY)
        {s += langDBINF_READ_ONLY; comma++;}
    if (pDb->dbInf.scrollConcurrency & SQL_SCCO_LOCK)
        {if (comma++) s += ", "; s += langDBINF_LOCK;}
    if (pDb->dbInf.scrollConcurrency & SQL_SCCO_OPT_ROWVER)
        {if (comma++) s += ", "; s += langDBINF_OPT_ROWVER;}
    if (pDb->dbInf.scrollConcurrency & SQL_SCCO_OPT_VALUES)
        {if (comma++) s += ", "; s += langDBINF_OPT_VALUES;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_SCROLL_OPTS;
    if (pDb->dbInf.scrollOptions & SQL_SO_FORWARD_ONLY)
        {s += langDBINF_FWD_ONLY; comma++;}
    if (pDb->dbInf.scrollOptions & SQL_SO_STATIC)
        {if (comma++) s += ", "; s += langDBINF_STATIC;}
    if (pDb->dbInf.scrollOptions & SQL_SO_KEYSET_DRIVEN)
        {if (comma++) s += ", "; s += langDBINF_KEYSET_DRIVEN;}
    if (pDb->dbInf.scrollOptions & SQL_SO_DYNAMIC)
        {if (comma++) s += ", "; s += langDBINF_DYNAMIC;}
    if (pDb->dbInf.scrollOptions & SQL_SO_MIXED)
        {if (comma++) s += ", "; s += langDBINF_MIXED;}
    s += "\n";

    comma = FALSE;
    s += langDBINF_STATIC_SENS;
    if (pDb->dbInf.staticSensitivity & SQL_SS_ADDITIONS)
        {s += langDBINF_ADDITIONS; comma++;}
    if (pDb->dbInf.staticSensitivity & SQL_SS_DELETIONS)
        {if (comma++) s += ", "; s += langDBINF_DELETIONS;}
    if (pDb->dbInf.staticSensitivity & SQL_SS_UPDATES)
        {if (comma++) s += ", "; s += langDBINF_UPDATES;}
    s += "\n";
#ifdef __VMS__
#pragma message enable incboodep
#endif


    s += langDBINF_TXN_CAPABLE;
    switch(pDb->dbInf.txnCapable)
    {
        case SQL_TC_NONE:           s += langNO;                        break;
        case SQL_TC_DML:            s += langDBINF_DML_ONLY;        break;
        case SQL_TC_DDL_COMMIT: s += langDBINF_DDL_COMMIT;      break;
        case SQL_TC_DDL_IGNORE: s += langDBINF_DDL_IGNORE;      break;
        case SQL_TC_ALL:            s += langDBINF_DDL_AND_DML; break;
    }
    s += "\n";

    t.sprintf("%s%d\n", langDBINF_LOGIN_TIMEOUT, pDb->dbInf.loginTimeout);
    s += t;

    // Oracle specific information
    if (pDb->Dbms() == dbmsORACLE)
    {
        s += "\n";
        s += langDBINF_ORACLE_BANNER;
        s += "\n";

        // Oracle cache hit ratio
        SDWORD cb;
        ULONG dbBlockGets;
        pDb->ExecSql("SELECT VALUE FROM V$SYSSTAT WHERE NAME = 'db block gets'");
        pDb->GetNext();
        if (pDb->GetData(1, SQL_C_ULONG, &dbBlockGets, 0, &cb))
        {
            t.sprintf("%s: %lu\n", langDBINF_DB_BLOCK_GETS, dbBlockGets);
            s += t;
        }

        ULONG consistentGets;
        pDb->ExecSql("SELECT VALUE FROM V$SYSSTAT WHERE NAME = 'consistent gets'");
        pDb->GetNext();
        if (pDb->GetData(1, SQL_C_ULONG, &consistentGets, 0, &cb))
        {
            t.sprintf("%s: %lu\n", langDBINF_CONSISTENT_GETS, consistentGets);
            s += t;
        }

        ULONG physReads;
        pDb->ExecSql("SELECT VALUE FROM V$SYSSTAT WHERE NAME = 'physical reads'");
        pDb->GetNext();
        if (pDb->GetData(1, SQL_C_ULONG, &physReads, 0, &cb))
        {
            t.sprintf("%s: %lu\n", langDBINF_PHYSICAL_READS, physReads);
            s += t;
        }

        ULONG hitRatio = (ULONG)((1.00 - ((float)physReads / (float)(dbBlockGets + consistentGets))) * 100.00);
        t.sprintf("*** %s: %lu%%\n", langDBINF_CACHE_HIT_RATIO, hitRatio);
        s += t;

        // Tablespace information
        s += "\n";
        s += langDBINF_TABLESPACE_IO;
        s += "\n";
        ULONG physWrites;
        char tablespaceName[257];
        pDb->ExecSql("SELECT NAME,PHYRDS,PHYWRTS FROM V$DATAFILE, V$FILESTAT WHERE V$DATAFILE.FILE# = V$FILESTAT.FILE#");
        while (pDb->GetNext())
        {
            pDb->GetData(1, SQL_C_CHAR,  tablespaceName, 257, &cb);
            pDb->GetData(2, SQL_C_ULONG, &physReads,     0,   &cb);
            pDb->GetData(3, SQL_C_ULONG, &physWrites,    0,   &cb);
            t.sprintf("%s\n\t%s: %lu\t%s: %lu\n", tablespaceName,
                langDBINF_PHYSICAL_READS, physReads, langDBINF_PHYSICAL_WRITES, physWrites);
            s += t;
        }

        s += "\n";
    }

    wxLogMessage(s);

}  // DisplayDbDiagnostics()

#if wxUSE_NEW_GRID

BEGIN_EVENT_TABLE(DbGridFrame, wxFrame)
 //   EVT_CLOSE(DbGridFrame::OnCloseWindow)
END_EVENT_TABLE()


DbGridFrame::DbGridFrame(wxWindow *parent)
    : wxFrame (parent, -1, wxT("Database Table"),
               wxDefaultPosition, wxSize(400, 325))
{
    initialized = FALSE;
}


void DbGridFrame::OnCloseWindow(wxCloseEvent& event)
{
    this->Destroy();
}


bool DbGridFrame::Initialize()
{
    wxGrid *grid = new wxGrid(this, -1, wxDefaultPosition);

    grid->RegisterDataType(wxGRID_VALUE_DATETIME,
                             new wxGridCellDateTimeRenderer(_T("%d %b %Y")),
                             new wxGridCellTextEditor);
#ifdef CHOICEINT
    grid->RegisterDataType(wxGRID_VALUE_CHOICEINT,
                             new wxGridCellEnumRenderer,
                             new wxGridCellEnumEditor);
                             
    wxString NativeLangChoice( wxString::Format("%s:%s,%s,%s,%s,%s",wxGRID_VALUE_CHOICEINT, 
                            wxT("English"),
                            wxT("French"),
                            wxT("German"),
                            wxT("Spanish"),
                            wxT("Other") )); 
#endif

    // Columns must match the sequence specified in SetColDef() calls
    wxDbGridColInfo* cols =
        new wxDbGridColInfo( 0,wxGRID_VALUE_STRING,wxT("Name"),
        new wxDbGridColInfo( 1,wxGRID_VALUE_STRING,wxT("Address 1"),
        new wxDbGridColInfo( 2,wxGRID_VALUE_STRING,wxT("Address 2"),
        new wxDbGridColInfo( 3,wxGRID_VALUE_STRING,wxT("City"),
        new wxDbGridColInfo( 4,wxGRID_VALUE_STRING,wxT("State"),
        new wxDbGridColInfo( 5,wxGRID_VALUE_STRING,wxT("PostCode"),
        new wxDbGridColInfo( 6,wxGRID_VALUE_STRING,wxT("Country"),
        new wxDbGridColInfo( 7,wxGRID_VALUE_DBAUTO,wxT("Join Date"),
        new wxDbGridColInfo( 8,wxGRID_VALUE_BOOL,  wxT("Developer"),
        new wxDbGridColInfo( 9,wxGRID_VALUE_NUMBER,wxT("Contributions"),
        new wxDbGridColInfo(10,wxGRID_VALUE_NUMBER,wxT("Lines Of Code"),
#ifdef CHOICEINT
        new wxDbGridColInfo(11,NativeLangChoice,   wxT("Native Language"),NULL))))))))))));
#else        
        new wxDbGridColInfo(11,wxGRID_VALUE_NUMBER,wxT("Native Language"),NULL))))))))))));
#endif

    Ccontact *Contact = new Ccontact();
    //wxGetApp().Contact

    if (!Contact)
    {
        wxMessageBox(wxT("Unable to instantiate an instance of Ccontact"), wxT("Error..."), wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    if (!Contact->Open())
    {
        if (Contact->GetDb()->TableExists(CONTACT_TABLE_NAME, Contact->GetDb()->GetUsername(),
                                          wxGetApp().DbConnectInf->GetDefaultDir()))
        {
            wxString tStr;
            tStr.Printf(wxT("Unable to open the table '%s'.\n\n"),CONTACT_TABLE_NAME);
            wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                         wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        }

        return FALSE;
    }

    // Execute the following query using the cursor designated
    // for full table query
    Contact->SetRowMode(wxDbTable::WX_ROW_MODE_QUERY);

    if (!Contact->Query())
    {
        wxString tStr;
        tStr = wxT("ODBC error during Query()\n\n");
        wxMessageBox(wxDbLogExtendedErrorMsg(tStr.c_str(),wxGetApp().Contact->GetDb(),__FILE__,__LINE__),
                     wxT("ODBC Error..."),wxOK | wxICON_EXCLAMATION);
        return FALSE;
    }

    // No data has been read in from the database yet, so
    // we need to initialize the data members to valid values
    // so Fit() can correctly size the grid
    Contact->Initialize();

    wxDbGridTableBase* db = new wxDbGridTableBase(Contact, cols, wxUSE_QUERY, TRUE);

    delete cols;

    grid->SetTable(db,TRUE);
    grid->SetMargins(0, 0);

    grid->Fit();
    wxSize size = grid->GetSize();
    size.x += 10;
    size.y += 10;
    SetClientSize(size);
    initialized = TRUE;
    return TRUE;
}  // DbGridFrame::Initialize()

#endif // #if wxUSE_NEW_GRID

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


