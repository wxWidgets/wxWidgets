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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
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

wxListBox *the_list = NULL;

MyConnection *the_connection = NULL;
MyClient *my_client;

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
    // service name (DDE classes) or port number (TCP/IP based classes)
    wxString service = IPC_SERVICE;

    // ignored under DDE, host name in TCP/IP based classes
    wxString hostName = _T("localhost");

    if (argc > 1)
        service = argv[1];
    if (argc > 2)
        hostName = argv[2];

    // Create a new client
    my_client = new MyClient;

    // suppress the log messages from MakeConnection()
    {
        wxLogNull nolog;
        the_connection = (MyConnection *)
            my_client->MakeConnection(hostName, service, IPC_TOPIC);

        while ( !the_connection )
        {
            if ( wxMessageBox(_T("Failed to make connection to server.\nRetry?"),
                              _T("Client Demo Error"),
                              wxICON_ERROR | wxYES_NO | wxCANCEL ) != wxYES )
            {
                // no server
                return false;
            }

            the_connection = (MyConnection *)my_client->MakeConnection(hostName, service, _T("IPC TEST"));
        }
    }

    if (!the_connection->StartAdvise(IPC_ADVISE_NAME))
        wxMessageBox(_T("StartAdvise failed"), _T("Client Demo Error"));

    // Create the main frame window
    (new MyFrame(NULL, _T("Client")))->Show(true);

    return true;
}

int MyApp::OnExit()
{
    // will delete the connection too
    // Update: Seems it didn't delete the_connection, because there's a leak.
    // Deletion is now explicitly done a few lines up.
    // another Update: in fact it's because OnDisconnect should delete it, but
    // it wasn't
    delete my_client;



    return 0;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title)
        : wxFrame(frame, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(CLIENT_EXECUTE, _T("&Execute\tCtrl-E"));
    file_menu->Append(CLIENT_REQUEST, _T("&Request\tCtrl-R"));
    file_menu->Append(CLIENT_POKE, _T("&Poke\tCtrl-P"));
    file_menu->Append(CLIENT_QUIT, _T("&Quit\tCtrl-Q"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // Make a listbox which shows the choices made in the server
    the_list = new wxListBox(this, CLIENT_LISTBOX, wxPoint(5, 5));
    the_list->Append(_T("Apple"));
    the_list->Append(_T("Pear"));
    the_list->Append(_T("Orange"));
    the_list->Append(_T("Banana"));
    the_list->Append(_T("Fruit"));
}

void MyFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
    if (the_connection)
        if (!the_connection->Execute(_T("Hello from the client!")))
            wxMessageBox(_T("Execute failed"), _T("Client Demo Error"));
}

void MyFrame::OnPoke(wxCommandEvent& WXUNUSED(event))
{
    if (the_connection)
        if (!the_connection->Poke(_T("An item"), _T("Some data to poke at the server!")))
            wxMessageBox(_T("Poke failed"), _T("Client Demo Error"));
}

void MyFrame::OnRequest(wxCommandEvent& WXUNUSED(event))
{
    if (the_connection)
    {
        wxChar *data = the_connection->Request(_T("An item"));
        if (data)
            wxMessageBox(data, _T("Client: Request"), wxOK);
        else
            wxMessageBox(_T("Request failed"), _T("Client Demo Error"));
    }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyConnection;
}

bool MyConnection::OnAdvise(const wxString& WXUNUSED(topic), const wxString& WXUNUSED(item), wxChar *data, int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
    if (the_list)
    {
        int n = the_list->FindString(data);
        if (n > wxNOT_FOUND)
            the_list->SetSelection(n);
    }
    return true;
}

bool MyConnection::OnDisconnect()
{
    // when connection is terminated, quit whole program
    wxWindow *win = wxTheApp->GetTopWindow();
    if ( win )
        win->Destroy();

    // delete self
    the_connection = NULL;
    return wxConnection::OnDisconnect();
}

