/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     IPC sample: server
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

#include "server.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_LISTBOX(SERVER_LISTBOX, MyFrame::OnListBoxClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(IPCDialogBox, wxDialog)
    EVT_BUTTON(SERVER_QUIT_BUTTON, IPCDialogBox::OnQuit)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

char ipc_buffer[4000];
MyConnection *the_connection = NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    // Create the main frame window
    (new MyFrame(NULL, "Server"))->Show(TRUE);

    // create the server object
    wxString server_name = "4242";
    if (argc > 1)
        server_name = argv[1];

    // Create a new server
    m_server = new MyServer;
    m_server->Create(server_name);

    return TRUE;
}

int MyApp::OnExit()
{
    delete m_server;

    return 0;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title)
       : wxFrame(frame, -1, title)
{
    panel = NULL;

    CreateStatusBar();

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(SERVER_QUIT, "&Exit");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // Make a panel
    panel = new wxPanel(this);
    wxListBox *list = new wxListBox(panel, SERVER_LISTBOX, wxPoint(5, 5));
    list->Append("Apple");
    list->Append("Pear");
    list->Append("Orange");
    list->Append("Banana");
    list->Append("Fruit");

    panel->Fit();
    Fit();
}

// Set the client process's listbox to this item
void MyFrame::OnListBoxClick(wxCommandEvent& event)
{
    wxListBox* listBox = (wxListBox*) panel->FindWindow(SERVER_LISTBOX);
    if (listBox)
    {
        wxString value = listBox->GetStringSelection();
        if (the_connection)
        {
            the_connection->Advise("Item", (wxChar *)value.c_str());
        }
    }
}

// ----------------------------------------------------------------------------
// IPCDialogBox
// ----------------------------------------------------------------------------

IPCDialogBox::IPCDialogBox(wxWindow *parent, const wxString& title,
                           const wxPoint& pos, const wxSize& size,
                           MyConnection *connection)
            : wxDialog(parent, -1, title, pos, size)
{
    m_connection = connection;
    (void)new wxButton(this, SERVER_QUIT_BUTTON, "Quit this connection",
                       wxPoint(5, 5));
    Fit();
}

void IPCDialogBox::OnQuit(wxCommandEvent& event)
{
    m_connection->Disconnect();
    delete m_connection;
}

// ----------------------------------------------------------------------------
// MyServer
// ----------------------------------------------------------------------------

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    if (strcmp(topic, "STDIO") != 0 && strcmp(topic, "IPC TEST") == 0)
        return new MyConnection(ipc_buffer, WXSIZEOF(ipc_buffer));
    else
        return NULL;
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

MyConnection::MyConnection(char *buf, int size)
            : wxConnection(buf, size)
{
    dialog = new IPCDialogBox(wxTheApp->GetTopWindow(), "Connection",
                              wxPoint(100, 100), wxSize(500, 500), this);
    dialog->Show(TRUE);
    the_connection = this;
}

MyConnection::~MyConnection()
{
    if (the_connection)
    {
        dialog->Destroy();
        the_connection = NULL;
    }
}

bool MyConnection::OnExecute(const wxString& WXUNUSED(topic),
                             char *data,
                             int WXUNUSED(size),
                             wxIPCFormat WXUNUSED(format))
{
    wxLogStatus("Execute command: %s", data);
    return TRUE;
}

bool MyConnection::OnPoke(const wxString& WXUNUSED(topic),
                          const wxString& item,
                          char *data,
                          int WXUNUSED(size),
                          wxIPCFormat WXUNUSED(format))
{
    wxLogStatus("Poke command: %s = %s", item.c_str(), data);
    return TRUE;
}

char *MyConnection::OnRequest(const wxString& WXUNUSED(topic),
                              const wxString& WXUNUSED(item),
                              int * WXUNUSED(size),
                              wxIPCFormat WXUNUSED(format))
{
    return "Here, have your data, client!";
}

bool MyConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                 const wxString& WXUNUSED(item))
{
    return TRUE;
}

