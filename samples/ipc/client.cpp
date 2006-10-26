/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     DDE sample: client
// Author:      Julian Smart
// Modified by:    Jurgen Doornik
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
#include "ipcsetup.h"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#include "mondrian.xpm"
#endif

#include "wx/datetime.h"
#include "client.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
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
END_EVENT_TABLE()

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
    // Create the main frame window
    m_frame = new MyFrame(NULL, _T("Client"));
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
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_EXIT, _T("&Quit\tCtrl-Q"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // set a dialog background
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

    // add the controls to the frame
    wxString strs4[] =
    {
        IPC_SERVICE, _T("...")
    };
    wxString strs5[] =
    {
        IPC_HOST, _T("...")
    };
    wxString strs6[] =
    {
        IPC_TOPIC, _T("...")
    };

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxGridSizer *item2 = new wxGridSizer( 4, 0, 0 );

    wxButton *item3 = new wxButton( this, ID_START, wxT("Connect to server"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxChoice *item5 = new wxChoice( this, ID_HOSTNAME, wxDefaultPosition, wxSize(100,-1), 2, strs5, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxChoice *item4 = new wxChoice( this, ID_SERVERNAME, wxDefaultPosition, wxSize(100,-1), 2, strs4, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxChoice *item6 = new wxChoice( this, ID_TOPIC, wxDefaultPosition, wxSize(100,-1), 2, strs6, 0 );
    item2->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item7 = new wxButton( this, ID_DISCONNECT, wxT("Disconnect "), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item8 = new wxButton( this, ID_STARTADVISE, wxT("StartAdvise"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item9 = new wxButton( this, ID_STOPADVISE, wxT("StopAdvise"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( this, ID_EXECUTE, wxT("Execute"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( this, ID_POKE, wxT("Poke"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item12 = new wxButton( this, ID_REQUEST, wxT("Request"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 1, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item14 = new wxStaticBox( this, -1, wxT("Client log") );
    wxStaticBoxSizer *item13 = new wxStaticBoxSizer( item14, wxVERTICAL );

    wxTextCtrl *item15 = new wxTextCtrl( this, ID_LOG, wxEmptyString, wxDefaultPosition, wxSize(500,140), wxTE_MULTILINE );
    item13->Add( item15, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    this->SetSizer( item0 );
    item0->SetSizeHints( this );

    // status
    m_client = NULL;
    GetServername()->SetSelection(0);
    GetHostname()->SetSelection(0);
    GetTopic()->SetSelection(0);
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(GetLog());
    delete wxLog::SetActiveTarget(logWindow);
    wxLogMessage(_T("Click on Connect to connect to the server"));
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
    if (m_client)
    {
        delete m_client;
        m_client = NULL;
    }
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

    wxLogMessage(_T("Client host=\"%s\" port=\"%s\" topic=\"%s\" %s"),
        hostname.c_str(), servername.c_str(), topic.c_str(),
        retval ? _T("connected") : _T("failed to connect"));

    if (!retval)
    {
        delete m_client;
        m_client = NULL;
    }
    EnableControls();
}

void MyFrame::OnServername( wxCommandEvent& WXUNUSED(event) )
{
    if (GetServername()->GetStringSelection() == _T("..."))
    {
        wxString s = wxGetTextFromUser(_T("Specify the name of the server"),
            _T("Server Name"), wxEmptyString, this);
        if (!s.IsEmpty() && s != IPC_SERVICE)
        {
            GetServername()->Insert(s, 0);
            GetServername()->SetSelection(0);
        }
    }
}

void MyFrame::OnHostname( wxCommandEvent& WXUNUSED(event) )
{
    if (GetHostname()->GetStringSelection() == _T("..."))
    {
        wxString s = wxGetTextFromUser(_T("Specify the name of the host (ignored under DDE)"),
            _T("Host Name"), wxEmptyString, this);
        if (!s.IsEmpty() && s != IPC_HOST)
        {
            GetHostname()->Insert(s, 0);
            GetHostname()->SetSelection(0);
        }
    }
}

void MyFrame::OnTopic( wxCommandEvent& WXUNUSED(event) )
{
    if (GetTopic()->GetStringSelection() == _T("..."))
    {
        wxString s = wxGetTextFromUser(_T("Specify the name of the topic"),
            _T("Topic Name"), wxEmptyString, this);
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
    delete m_client;
    m_client = NULL;
    EnableControls();
}

void MyFrame::OnStartAdvise(wxCommandEvent& WXUNUSED(event))
{
    m_client->GetConnection()->StartAdvise(_T("something"));
}

void MyFrame::OnStopAdvise(wxCommandEvent& WXUNUSED(event))
{
    m_client->GetConnection()->StopAdvise(_T("something"));
}

void MyFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        wxString s = _T("Date");

        m_client->GetConnection()->Execute((wxChar *)s.c_str());
        m_client->GetConnection()->Execute((wxChar *)s.c_str(), (s.Length() + 1) * sizeof(wxChar));
#if wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("DDE Execute can only be used to send text strings, not arbitrary data.\nThe type argument will be ignored, text truncated, converted to Unicode and null terminated."));
#endif
        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        m_client->GetConnection()->Execute((wxChar *)bytes, 3, wxIPC_PRIVATE);
    }
}

void MyFrame::OnPoke(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        wxString s = wxDateTime::Now().Format();
        m_client->GetConnection()->Poke(_T("Date"), (wxChar *)s.c_str());
        s = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
        m_client->GetConnection()->Poke(_T("Date"), (wxChar *)s.c_str(), (s.Length() + 1) * sizeof(wxChar));
        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        m_client->GetConnection()->Poke(_T("bytes[3]"), (wxChar *)bytes, 3, wxIPC_PRIVATE);
    }
}

void MyFrame::OnRequest(wxCommandEvent& WXUNUSED(event))
{
    if (m_client->IsConnected())
    {
        int size;
        m_client->GetConnection()->Request(_T("Date"));
        m_client->GetConnection()->Request(_T("Date+len"), &size);
        m_client->GetConnection()->Request(_T("bytes[3]"), &size, wxIPC_PRIVATE);
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
        delete m_connection;
        m_connection = NULL;
        wxGetApp().GetFrame()->EnableControls();
        wxLogMessage(_T("Client disconnected from server"));
    }
}

MyClient::~MyClient()
{
    Disconnect();
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

void MyConnection::Log(const wxString& command, const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    wxString s;
    if (topic.IsEmpty() && item.IsEmpty())
        s.Printf(_T("%s("), command.c_str());
    else if (topic.IsEmpty())
        s.Printf(_T("%s(item=\"%s\","), command.c_str(), item.c_str());
    else if (item.IsEmpty())
        s.Printf(_T("%s(topic=\"%s\","), command.c_str(), topic.c_str());
    else
        s.Printf(_T("%s(topic=\"%s\",item=\"%s\","), command.c_str(), topic.c_str(), item.c_str());

    if (format == wxIPC_TEXT || format == wxIPC_UNICODETEXT)
        wxLogMessage(_T("%s\"%s\",%d)"), s.c_str(), data, size);
    else if (format == wxIPC_PRIVATE)
    {
        if (size == 3)
        {
            char *bytes = (char *)data;
            wxLogMessage(_T("%s'%c%c%c',%d)"), s.c_str(), bytes[0], bytes[1], bytes[2], size);
        }
        else
            wxLogMessage(_T("%s...,%d)"), s.c_str(), size);
    }
    else if (format == wxIPC_INVALID)
        wxLogMessage(_T("%s[invalid data],%d)"), s.c_str(), size);
}

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, wxChar *data,
    int size, wxIPCFormat format)
{
    Log(_T("OnAdvise"), topic, item, data, size, format);
    return true;
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage(_T("OnDisconnect()"));
    wxGetApp().GetFrame()->Disconnect();
    return true;
}

bool MyConnection::Execute(const wxChar *data, int size, wxIPCFormat format)
{
    Log(_T("Execute"), wxEmptyString, wxEmptyString, (wxChar *)data, size, format);
    bool retval = wxConnection::Execute(data, size, format);
    if (!retval)
        wxLogMessage(_T("Execute failed!"));
    return retval;
}

wxChar *MyConnection::Request(const wxString& item, int *size, wxIPCFormat format)
{
    wxChar *data =  wxConnection::Request(item, size, format);
    Log(_T("Request"), wxEmptyString, item, data, size ? *size : -1, format);
    return data;
}

bool MyConnection::Poke(const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    Log(_T("Poke"), wxEmptyString, item, data, size, format);
    return wxConnection::Poke(item, data, size, format);
}
