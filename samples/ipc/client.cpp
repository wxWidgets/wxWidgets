/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     DDE sample: client
// Author:      Julian Smart
// Modified by:
// Created:     25/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Settings common to both executables: determines whether
// we're using TCP/IP or real DDE.

#include "ddesetup.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

#include "client.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(CLIENT_QUIT, MyFrame::OnExit)
    EVT_MENU(CLIENT_EXECUTE, MyFrame::OnExecute)
    EVT_MENU(CLIENT_POKE, MyFrame::OnPoke)
    EVT_MENU(CLIENT_REQUEST, MyFrame::OnRequest)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

char ipc_buffer[4000];
wxListBox *the_list = NULL;

MyConnection *the_connection = NULL;
MyClient *my_client ;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    wxString server = "4242";
#if wxUSE_DDE_FOR_SAMPLE
    wxString hostName = wxGetHostName();
#else
    wxString hostName = "localhost";
#endif

    if (argc > 1)
        server = argv[1];
    if (argc > 2)
        hostName = argv[2];

    // Create a new client
    my_client = new MyClient;
    the_connection = (MyConnection *)my_client->MakeConnection(hostName, server, "IPC TEST");

    while ( !the_connection )
    {
        if ( wxMessageBox("Failed to make connection to server.\nRetry?",
                          "Client Demo Error",
                          wxICON_ERROR | wxYES_NO | wxCANCEL ) != wxYES )
        {
            // no server
            return FALSE;
        }

        the_connection = (MyConnection *)my_client->MakeConnection(hostName, server, "IPC TEST");
    }

    if (!the_connection->StartAdvise("Item"))
        wxMessageBox("StartAdvise failed", "Client Demo Error");

    // Create the main frame window
    (new MyFrame(NULL, "Client"))->Show(TRUE);

    return TRUE;
}

int MyApp::OnExit()
{
    if (the_connection)
    {
        the_connection->Disconnect();
    }

    // will delete the connection too
    delete my_client;

    return 0;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title)
        : wxFrame(frame, -1, title)
{
    panel = NULL;

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(CLIENT_EXECUTE, "Execute");
    file_menu->Append(CLIENT_REQUEST, "Request");
    file_menu->Append(CLIENT_POKE, "Poke");
    file_menu->Append(CLIENT_QUIT, "Quit");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "File");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // Make a panel
    panel = new wxPanel(this);
    the_list = new wxListBox(panel, CLIENT_LISTBOX, wxPoint(5, 5));
    the_list->Append("Apple");
    the_list->Append("Pear");
    the_list->Append("Orange");
    the_list->Append("Banana");
    the_list->Append("Fruit");

    panel->Fit();
    Fit();
}

void MyFrame::OnExecute(wxCommandEvent& event)
{
    if (the_connection)
        if (!the_connection->Execute("Hello from the client!"))
            wxMessageBox("Execute failed", "Client Demo Error");
}

void MyFrame::OnPoke(wxCommandEvent& event)
{
    if (the_connection)
        if (!the_connection->Poke("An item", "Some data to poke at the server!"))
            wxMessageBox("Poke failed", "Client Demo Error");
}

void MyFrame::OnRequest(wxCommandEvent& event)
{
    if (the_connection)
    {
        char *data = the_connection->Request("An item");
        if (data)
            wxMessageBox(data, "Client: Request", wxOK);
        else
            wxMessageBox("Request failed", "Client Demo Error");
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close();
}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyConnection;
}

MyConnection::MyConnection()
            : wxConnection(ipc_buffer, WXSIZEOF(ipc_buffer))
{
}

MyConnection::~MyConnection()
{
    the_connection = NULL;
}

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format)
{
    if (the_list)
    {
        int n = the_list->FindString(data);
        if (n > -1)
            the_list->SetSelection(n);
    }
    return TRUE;
}

bool MyConnection::OnDisconnect()
{
    wxWindow *win = wxTheApp->GetTopWindow();
    if ( win )
        win->Destroy();

    return TRUE;
}

