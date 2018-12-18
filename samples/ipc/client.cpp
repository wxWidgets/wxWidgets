/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     DDE sample: client
// Author:      Julian Smart
// Modified by:    Jurgen Doornik
// Created:     25/01/99
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
#include "ipcsetup.h"

#include "wx/datetime.h"
#include "client.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_CLOSE( MyFrame::OnClose )
    EVT_BUTTON( ID_START,       MyFrame::OnStart )
    EVT_CHOICE( ID_SERVERNAME,  MyFrame::OnServername )
    EVT_CHOICE( ID_HOSTNAME,    MyFrame::OnHostname )
    EVT_CHOICE( ID_TOPIC,       MyFrame::OnTopic )
    EVT_BUTTON( ID_DISCONNECT,  MyFrame::OnDisconnect )
    EVT_BUTTON( ID_STARTADVISE, MyFrame::OnStartAdvise )
    EVT_BUTTON( ID_STOPADVISE,  MyFrame::OnStopAdvise )
    EVT_BUTTON( ID_POKE,        MyFrame::OnPoke )
    EVT_BUTTON( ID_EXECUTE,     MyFrame::OnExecute )
    EVT_BUTTON( ID_REQUEST,     MyFrame::OnRequest )
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

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
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    m_frame = new MyFrame(NULL, "Client");
    m_frame->Show(true);

    return true;
}

int MyApp::OnExit()
{

    return 0;
}

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title)
        : wxFrame(frame, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300))
{
    // Give it an icon
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_EXIT, "&Quit\tCtrl-Q");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // set a dialog background
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    // add the controls to the frame
    wxString strs4[] =
    {
        IPC_SERVICE, "..."
    };
    wxString strs5[] =
    {
        IPC_HOST, "..."
    };
    wxString strs6[] =
    {
        IPC_TOPIC, "..."
    };

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxGridSizer *item2 = new wxGridSizer( 4, 0, 0 );

    wxButton *item3 = new wxButton( this, ID_START, "Connect to server", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxChoice *item5 = new wxChoice( this, ID_HOSTNAME, wxDefaultPosition, wxSize(100,-1), 2, strs5, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxChoice *item4 = new wxChoice( this, ID_SERVERNAME, wxDefaultPosition, wxSize(100,-1), 2, strs4, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxChoice *item6 = new wxChoice( this, ID_TOPIC, wxDefaultPosition, wxSize(100,-1), 2, strs6, 0 );
    item2->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item7 = new wxButton( this, ID_DISCONNECT, "Disconnect ", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item8 = new wxButton( this, ID_STARTADVISE, "StartAdvise", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item9 = new wxButton( this, ID_STOPADVISE, "StopAdvise", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( this, ID_EXECUTE, "Execute", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( this, ID_POKE, "Poke", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item12 = new wxButton( this, ID_REQUEST, "Request", wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 1, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, wxSizerFlags().Expand().Border(wxALL, 5) );

    wxStaticBox *item14 = new wxStaticBox( this, -1, "Client log" );
    wxStaticBoxSizer *item13 = new wxStaticBoxSizer( item14, wxVERTICAL );

    wxTextCtrl *item15 = new wxTextCtrl( this, ID_LOG, wxEmptyString, wxDefaultPosition, wxSize(500,140), wxTE_MULTILINE );
    item13->Add( item15, wxSizerFlags(1).Expand().Border(wxALL, 5) );

    item0->Add( item13, wxSizerFlags(0).Expand().Border(wxALL, 5) );

    this->SetSizer( item0 );
    item0->SetSizeHints( this );

    // status
    m_client = NULL;
    GetServername()->SetSelection(0);
    GetHostname()->SetSelection(0);
    GetTopic()->SetSelection(0);
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(GetLog());
    delete wxLog::SetActiveTarget(logWindow);
    wxLogMessage("Click on Connect to connect to the server");
    EnableControls();
}

void MyFrame::EnableControls()
{
    GetStart()->Enable(m_client == NULL);
    GetServername()->Enable(m_client == NULL);
    GetHostname()->Enable(m_client == NULL);
    GetTopic()->Enable(m_client == NULL);

    const bool isConnected = (m_client != NULL && m_client->IsConnected());
    GetDisconnect()->Enable(m_client != NULL && isConnected);
    GetStartAdvise()->Enable(m_client != NULL && isConnected);
    GetStopAdvise()->Enable(m_client != NULL && isConnected);
    GetExecute()->Enable(m_client != NULL && isConnected);
    GetPoke()->Enable(m_client != NULL && isConnected);
    GetRequest()->Enable(m_client != NULL && isConnected);
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    wxDELETE(m_client);

    event.Skip();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
    // Connect to the client
    wxString servername = GetServername()->GetStringSelection();
    wxString hostname = GetHostname()->GetStringSelection();
    wxString topic = GetTopic()->GetStringSelection();

    m_client = new MyClient;
    bool retval = m_client->Connect(hostname, servername, topic);

    wxLogMessage("Client host=\"%s\" port=\"%s\" topic=\"%s\" %s",
        hostname, servername, topic,
        retval ? "connected" : "failed to connect");

    if (!retval)
    {
        wxDELETE(m_client);
    }
    EnableControls();
}

void MyFrame::OnServername( wxCommandEvent& WXUNUSED(event) )
{
    if (GetServername()->GetStringSelection() == "...")
    {
        wxString s = wxGetTextFromUser("Specify the name of the server",
            "Server Name", wxEmptyString, this);
        if (!s.IsEmpty() && s != IPC_SERVICE)
        {
            GetServername()->Insert(s, 0);
            GetServername()->SetSelection(0);
        }
    }
}

void MyFrame::OnHostname( wxCommandEvent& WXUNUSED(event) )
{
    if (GetHostname()->GetStringSelection() == "...")
    {
        wxString s = wxGetTextFromUser("Specify the name of the host (ignored under DDE)",
            "Host Name", wxEmptyString, this);
        if (!s.IsEmpty() && s != IPC_HOST)
        {
            GetHostname()->Insert(s, 0);
            GetHostname()->SetSelection(0);
        }
    }
}

void MyFrame::OnTopic( wxCommandEvent& WXUNUSED(event) )
{
    if (GetTopic()->GetStringSelection() == "...")
    {
        wxString s = wxGetTextFromUser("Specify the name of the topic",
            "Topic Name", wxEmptyString, this);
        if (!s.IsEmpty() && s != IPC_TOPIC)
        {
            GetTopic()->Insert(s, 0);
            GetTopic()->SetSelection(0);
        }
    }
}

void MyFrame::OnDisconnect(wxCommandEvent& WXUNUSED(event))
{
    Disconnect();
}

void MyFrame::Disconnect()
{
    wxDELETE(m_client);
    EnableControls();
}

void MyFrame::OnStartAdvise(wxCommandEvent& WXUNUSED(event))
{
    m_client->GetConnection()->StartAdvise("something");
}

void MyFrame::OnStopAdvise(wxCommandEvent& WXUNUSED(event))
{
    m_client->GetConnection()->StopAdvise("something");
}

void MyFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        wxString s = "Date";

        m_client->GetConnection()->Execute(s);
        m_client->GetConnection()->Execute((const char *)s.c_str(), s.length() + 1);
        char bytes[3];
        bytes[0] = '1';
        bytes[1] = '2';
        bytes[2] = '3';
        m_client->GetConnection()->Execute(bytes, WXSIZEOF(bytes));
    }
}

void MyFrame::OnPoke(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        wxString s = wxDateTime::Now().Format();
        m_client->GetConnection()->Poke("Date", s);
        s = wxDateTime::Now().FormatTime() + " " + wxDateTime::Now().FormatDate();
        m_client->GetConnection()->Poke("Date", (const char *)s.c_str(), s.length() + 1);
        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        m_client->GetConnection()->Poke("bytes[3]", bytes, 3, wxIPC_PRIVATE);
    }
}

void MyFrame::OnRequest(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        size_t size;
        m_client->GetConnection()->Request("Date");
        m_client->GetConnection()->Request("Date+len", &size);
        m_client->GetConnection()->Request("bytes[3]", &size, wxIPC_PRIVATE);
    }
}

// ----------------------------------------------------------------------------
// MyClient
// ----------------------------------------------------------------------------
MyClient::MyClient() : wxClient()
{
    m_connection = NULL;
}

bool MyClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    // suppress the log messages from MakeConnection()
    wxLogNull nolog;

    m_connection = (MyConnection *)MakeConnection(sHost, sService, sTopic);
    return m_connection    != NULL;
}

wxConnectionBase *MyClient::OnMakeConnection()
{
    return new MyConnection;
}

void MyClient::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        wxDELETE(m_connection);
        wxGetApp().GetFrame()->EnableControls();
        wxLogMessage("Client disconnected from server");
    }
}

MyClient::~MyClient()
{
    Disconnect();
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, const void *data,
    size_t size, wxIPCFormat format)
{
    Log("OnAdvise", topic, item, data, size, format);
    return true;
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage("OnDisconnect()");
    wxGetApp().GetFrame()->Disconnect();
    return true;
}

bool MyConnection::DoExecute(const void *data, size_t size, wxIPCFormat format)
{
    Log("Execute", wxEmptyString, wxEmptyString, data, size, format);
    bool retval = wxConnection::DoExecute(data, size, format);
    if (!retval)
    {
        wxLogMessage("Execute failed!");
    }
    return retval;
}

const void *MyConnection::Request(const wxString& item, size_t *size, wxIPCFormat format)
{
    const void *data =  wxConnection::Request(item, size, format);
    Log("Request", wxEmptyString, item, data, size ? *size : wxNO_LEN, format);
    return data;
}

bool MyConnection::DoPoke(const wxString& item, const void *data, size_t size, wxIPCFormat format)
{
    Log("Poke", wxEmptyString, item, data, size, format);
    return wxConnection::DoPoke(item, data, size, format);
}
