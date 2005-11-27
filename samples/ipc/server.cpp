/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     IPC sample: server
// Author:      Julian Smart
// Modified by: Jurgen Doornik
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

#include "server.h"
#include "wx/textdlg.h"
#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU   (wxID_EXIT,         MyFrame::OnExit)
    EVT_CLOSE( MyFrame::OnClose )
    EVT_BUTTON( ID_START,         MyFrame::OnStart )
    EVT_CHOICE( ID_SERVERNAME,  MyFrame::OnServerName )
    EVT_BUTTON( ID_DISCONNECT,     MyFrame::OnDisconnect )
    EVT_BUTTON( ID_ADVISE,         MyFrame::OnAdvise )
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    // Create the main frame window
    m_frame = new MyFrame(NULL, _T("Server"));
    m_frame->Show(true);

    return true;
}

int MyApp::OnExit()
{
    return 0;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title)
       : wxFrame(frame, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300))
{
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

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
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );
    item2->AddGrowableCol( 1 );

    wxButton *item3 = new wxButton( this, ID_START, wxT("Start Server"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs4[] = 
    {
        IPC_SERVICE, _T("...")
    };
    wxChoice *item4 = new wxChoice( this, ID_SERVERNAME, wxDefaultPosition, wxSize(100,-1), 2, strs4, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item5 = new wxButton( this, ID_DISCONNECT, wxT("Disconnect Client"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( this, ID_ADVISE, wxT("Advise"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item2->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 1, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item8 = new wxStaticBox( this, -1, wxT("Server log") );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item8, wxVERTICAL );

    wxTextCtrl *item9 = new wxTextCtrl( this, ID_LOG, wxT(""), wxDefaultPosition, wxSize(500,140), wxTE_MULTILINE );
    item7->Add( item9, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    SetSizer( item0 );
    item0->SetSizeHints( this );

    // status
    m_server = NULL;
    GetServername()->SetSelection(0);
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(GetLog());
    delete wxLog::SetActiveTarget(logWindow);
    wxLogMessage(_T("Click on Start to start the server"));
    Enable();
}

void MyFrame::Enable()
{
    GetStart()->Enable(m_server == NULL);
    GetServername()->Enable(m_server == NULL);
    GetAdvise()->Enable(m_server && m_server->CanAdvise());
    GetDisconnect()->Enable(m_server && m_server->IsConnected());
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    if (m_server)
    {
        delete m_server;
        m_server = NULL;
    }
    event.Skip();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
    // Create a new server
    m_server = new MyServer;
    wxString servername = GetServername()->GetStringSelection();
    if (m_server->Create(servername))
    {
        wxLogMessage(_T("Server %s started"), servername.c_str());
  #if wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("Server uses DDE"));
  #else // !wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("Server uses TCP"));
  #endif // wxUSE_DDE_FOR_IPC/!wxUSE_DDE_FOR_IPC
    }
    else
    {
        wxLogMessage(_T("Server %s failed to start"), servername.c_str());
        delete m_server;
        m_server = NULL;
    }
    Enable();
}

void MyFrame::OnServerName( wxCommandEvent& WXUNUSED(event) )
{
    if (GetServername()->GetStringSelection() == _T("..."))
    {
        wxString s = wxGetTextFromUser(_T("Specify the name of the server"),
            _T("Server Name"), _(""), this);
        if (!s.IsEmpty() && s != IPC_SERVICE)
        {
            GetServername()->Insert(s, 0);
            GetServername()->SetSelection(0);
        }
    }
}

void MyFrame::Disconnect()
{
    m_server->Disconnect();
    Enable();
}

void MyFrame::OnDisconnect(wxCommandEvent& WXUNUSED(event))
{
    Disconnect();    
}

void MyFrame::OnAdvise(wxCommandEvent& WXUNUSED(event))
{
    m_server->Advise();
}

// ----------------------------------------------------------------------------
// MyServer
// ----------------------------------------------------------------------------

MyServer::MyServer() : wxServer()
{
    m_connection = NULL;
}

MyServer::~MyServer()
{
    Disconnect();
}

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
    wxLogMessage(_T("OnAcceptConnection(\"%s\")"), topic.c_str());

    if ( topic == IPC_TOPIC )
    {
        m_connection = new MyConnection();
        wxGetApp().GetFrame()->Enable();
        wxLogMessage(_T("Connection accepted"));
        return m_connection;
    }
    // unknown topic
    return NULL;
}

void MyServer::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
        wxGetApp().GetFrame()->Enable();
        wxLogMessage(_T("Disconnected client"));
    }
}

void MyServer::Advise()
{
    if (CanAdvise())
    {
        wxString s = wxDateTime::Now().Format();
        m_connection->Advise(m_connection->m_sAdvise, (wxChar *)s.c_str());
        s = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
        m_connection->Advise(m_connection->m_sAdvise, (wxChar *)s.c_str(), (s.Length() + 1) * sizeof(wxChar));

#if wxUSE_DDE_FOR_IPC
        wxLogMessage(_T("DDE Advise type argument cannot be wxIPC_PRIVATE. The client will receive it as wxIPC_TEXT, and receive the correct no of bytes, but not print a correct log entry."));
#endif
        char bytes[3];
        bytes[0] = '1'; bytes[1] = '2'; bytes[2] = '3';
        m_connection->Advise(m_connection->m_sAdvise, (wxChar *)bytes, 3, wxIPC_PRIVATE);
        // this works, but the log treats it as a string now
//        m_connection->Advise(m_connection->m_sAdvise, (wxChar *)bytes, 3, wxIPC_TEXT );
    }
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

MyConnection::MyConnection()
            : wxConnection()
{
}

MyConnection::~MyConnection()
{
}

bool MyConnection::OnExecute(const wxString& topic,
    wxChar *data, int size, wxIPCFormat format)
{
    Log(_T("OnExecute"), topic, _T(""), data, size, format);
    return true;
}

bool MyConnection::OnPoke(const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    Log(_T("OnPoke"), topic, item, data, size, format);
    return wxConnection::OnPoke(topic, item, data, size, format);
}

wxChar *MyConnection::OnRequest(const wxString& topic,
    const wxString& item, int * size, wxIPCFormat format)
{
    wxChar *data;
    if (item == _T("Date"))
    {
        m_sRequestDate = wxDateTime::Now().Format();
        data = (wxChar *)m_sRequestDate.c_str();
        *size = -1;
    }    
    else if (item == _T("Date+len"))
    {
        m_sRequestDate = wxDateTime::Now().FormatTime() + _T(" ") + wxDateTime::Now().FormatDate();
        data = (wxChar *)m_sRequestDate.c_str();
        *size = (m_sRequestDate.Length() + 1) * sizeof(wxChar);
    }    
    else if (item == _T("bytes[3]"))
    {
        data = (wxChar *)m_achRequestBytes;
        m_achRequestBytes[0] = '1'; m_achRequestBytes[1] = '2'; m_achRequestBytes[2] = '3';
        *size = 3;
    }
    else
    {
        data = NULL;
        *size = 0;
    }
     Log(_T("OnRequest"), topic, item, data, *size, format);
    return data;
}

bool MyConnection::OnStartAdvise(const wxString& topic,
                                 const wxString& item)
{
    wxLogMessage(_T("OnStartAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(_T("Returning true"));
    m_sAdvise = item;
    wxGetApp().GetFrame()->Enable();
    return true;
}

bool MyConnection::OnStopAdvise(const wxString& topic,
                                 const wxString& item)
{
    wxLogMessage(_T("OnStopAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(_T("Returning true"));
    m_sAdvise.Empty();
    wxGetApp().GetFrame()->Enable();
    return true;
}

void MyConnection::Log(const wxString& command, const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    wxString s;
    if (topic.IsEmpty() && item.IsEmpty())
        s.Printf(_T("%s("), command.c_str());
    else if (topic.IsEmpty())
        s.Printf(_T("%s(\"%s\","), command.c_str(), item.c_str());
    else if (item.IsEmpty())
        s.Printf(_T("%s(\"%s\","), command.c_str(), topic.c_str());
    else
        s.Printf(_T("%s(\"%s\",\"%s\","), command.c_str(), topic.c_str(), item.c_str());

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

bool MyConnection::Advise(const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    Log(_T("Advise"), _T(""), item, data, size, format);
    return wxConnection::Advise(item, data, size, format);
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage(_T("OnDisconnect()"));
    wxGetApp().GetFrame()->Disconnect();
    return true;
}
