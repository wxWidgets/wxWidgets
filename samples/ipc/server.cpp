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

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
    #include "mondrian.xpm"
#endif

#include "server.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU   (SERVER_EXIT, MyFrame::OnExit)
    EVT_LISTBOX(SERVER_LISTBOX, MyFrame::OnListBoxClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(IPCDialogBox, wxDialog)
    EVT_BUTTON(SERVER_QUIT_BUTTON, IPCDialogBox::OnQuit)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

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
    (new MyFrame(NULL, _T("Server")))->Show(true);

    // service name (DDE classes) or port number (TCP/IP based classes)
    wxString service = IPC_SERVICE;

    if (argc > 1)
        service = argv[1];

    // Create a new server
    m_server = new MyServer;
    m_server->Create(service);

    return true;
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
       : wxFrame(frame, wxID_ANY, title, wxDefaultPosition, wxSize(350, 250))
{
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(SERVER_EXIT, _T("&Quit\tCtrl-Q"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // Make a listbox
    wxListBox *list = new wxListBox(this, SERVER_LISTBOX);
    list->Append(_T("Apple"));
    list->Append(_T("Pear"));
    list->Append(_T("Orange"));
    list->Append(_T("Banana"));
    list->Append(_T("Fruit"));
}

// Set the client process's listbox to this item
void MyFrame::OnListBoxClick(wxCommandEvent& WXUNUSED(event))
{
    wxListBox* listBox = (wxListBox*) FindWindow(SERVER_LISTBOX);
    if (listBox)
    {
        wxString value = listBox->GetStringSelection();

        /* Because the_connection only holds one connection, in this sample only
           one connection can receive advise messages */
        if (the_connection)
        {
            the_connection->Advise(IPC_ADVISE_NAME, (wxChar*)value.c_str());
        }
    }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

// ----------------------------------------------------------------------------
// IPCDialogBox
// ----------------------------------------------------------------------------

IPCDialogBox::IPCDialogBox(wxWindow *parent, const wxString& title,
                           const wxPoint& pos, const wxSize& size,
                           MyConnection *connection)
            : wxDialog(parent, wxID_ANY, title, pos, size)
{
    m_connection = connection;
    (void)new wxButton(this, SERVER_QUIT_BUTTON, _T("Quit this connection"),
                       wxPoint(5, 5));
    Fit();
}

IPCDialogBox::~IPCDialogBox( )
{
    // wxWidgets exit code destroys dialog before destroying the connection in
    // OnExit, so make sure connection won't try to delete the dialog later.
    if (m_connection)
        m_connection->dialog = NULL;
}

void IPCDialogBox::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    m_connection->Disconnect();
    delete m_connection;
}

// ----------------------------------------------------------------------------
// MyServer
// ----------------------------------------------------------------------------

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    if ( topic == IPC_TOPIC )
        return new MyConnection();

    // unknown topic
    return NULL;
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

MyConnection::MyConnection()
            : wxConnection()
{
    dialog = new IPCDialogBox(wxTheApp->GetTopWindow(), _T("Connection"),
                              wxDefaultPosition, wxDefaultSize, this);
    dialog->Show(true);
    the_connection = this;
}

MyConnection::~MyConnection()
{
    if (the_connection)
    {
        if (dialog)
        {
            dialog->m_connection = NULL;
            dialog->Destroy();
        }
        the_connection = NULL;
    }
}

bool MyConnection::OnExecute(const wxString& WXUNUSED(topic),
                             wxChar *data,
                             int WXUNUSED(size),
                             wxIPCFormat WXUNUSED(format))
{
    wxLogStatus(wxT("Execute command: %s"), data);
    return true;
}

bool MyConnection::OnPoke(const wxString& WXUNUSED(topic),
                          const wxString& item,
                          wxChar *data,
                          int WXUNUSED(size),
                          wxIPCFormat WXUNUSED(format))
{
    wxLogStatus(wxT("Poke command: %s = %s"), item.c_str(), data);
    return true;
}

wxChar *MyConnection::OnRequest(const wxString& WXUNUSED(topic),
                              const wxString& WXUNUSED(item),
                              int * WXUNUSED(size),
                              wxIPCFormat WXUNUSED(format))
{
    return _T("Here, have your data, client!");
}

bool MyConnection::OnStartAdvise(const wxString& WXUNUSED(topic),
                                 const wxString& WXUNUSED(item))
{
    return true;
}

