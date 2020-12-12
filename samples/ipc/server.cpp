/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     IPC sample: server
// Author:      Julian Smart
// Modified by: Jurgen Doornik
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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Settings common to both executables: determines whether
// we're using TCP/IP or real DDE.
#include "ipcsetup.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "server.h"
#include "wx/textdlg.h"
#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE( MyFrame::OnClose )

    EVT_BUTTON( ID_START,          MyFrame::OnStart )
    EVT_CHOICE( ID_SERVERNAME,     MyFrame::OnServerName )
    EVT_BUTTON( ID_DISCONNECT,     MyFrame::OnDisconnect )
    EVT_BUTTON( ID_ADVISE,         MyFrame::OnAdvise )
wxEND_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    m_frame = new MyFrame(NULL, "Server");
    m_frame->Show(true);

    return true;
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

    SetIcon(wxICON(sample));

    m_server = NULL;

    wxPanel * const panel = new wxPanel(this);

    wxBoxSizer * const sizerMain = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer * const sizerCmds = new wxFlexGridSizer( 2, 0, 0 );
    sizerCmds->AddGrowableCol( 1 );

    wxButton *btn;

    btn = new wxButton(panel, ID_START, "&Start Server");
    sizerCmds->Add(btn, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    const wxString choices[] = { IPC_SERVICE, "..." };
    wxChoice * const choice = new wxChoice
                                  (
                                    panel,
                                    ID_SERVERNAME,
                                    wxDefaultPosition, wxSize(100, -1),
                                    WXSIZEOF(choices), choices
                                  );
    sizerCmds->Add(choice, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    btn = new wxButton(panel, ID_DISCONNECT, "&Disconnect Client");
    sizerCmds->Add(btn, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    sizerCmds->AddSpacer(20);

    btn = new wxButton( panel, ID_ADVISE, "&Advise");
    sizerCmds->Add(btn, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    sizerCmds->AddSpacer(20);

    sizerMain->Add(sizerCmds, wxSizerFlags().Expand().Border(wxALL, 5));

    wxStaticBoxSizer * const
        sizerLog = new wxStaticBoxSizer(wxVERTICAL, panel, "Server &log");

    wxTextCtrl * const textLog = new wxTextCtrl
                                 (
                                    panel,
                                    wxID_ANY,
                                    "",
                                    wxDefaultPosition, wxSize(500, 140),
                                    wxTE_MULTILINE
                                 );
    sizerLog->Add(textLog, wxSizerFlags(1).Expand().Border(wxALL, 5));

    sizerMain->Add(sizerLog, wxSizerFlags(1).Expand().Border(wxALL, 5));

    panel->SetSizer(sizerMain);
    sizerMain->SetSizeHints(panel);
    SetClientSize(panel->GetSize());

    GetServername()->SetSelection(0);
    wxLogTextCtrl *logWindow = new wxLogTextCtrl(textLog);
    delete wxLog::SetActiveTarget(logWindow);
    wxLogMessage("Click on Start to start the server");
    UpdateUI();
}

void MyFrame::UpdateUI()
{
    GetStart()->Enable(m_server == NULL);
    GetServername()->Enable(m_server == NULL);
    GetAdvise()->Enable(m_server && m_server->CanAdvise());
    GetDisconnect()->Enable(m_server && m_server->IsConnected());
}

void MyFrame::OnClose(wxCloseEvent& event)
{
    wxDELETE(m_server);
    event.Skip();
}

void MyFrame::OnStart(wxCommandEvent& WXUNUSED(event))
{
    // Create a new server
    m_server = new MyServer;
    wxString servername = GetServername()->GetStringSelection();
    if (m_server->Create(servername))
    {
        wxLogMessage("Server %s started", servername);
  #if wxUSE_DDE_FOR_IPC
        wxLogMessage("Server uses DDE");
  #else // !wxUSE_DDE_FOR_IPC
        wxLogMessage("Server uses TCP");
  #endif // wxUSE_DDE_FOR_IPC/!wxUSE_DDE_FOR_IPC
    }
    else
    {
        wxLogMessage("Server %s failed to start", servername);
        wxDELETE(m_server);
    }
    UpdateUI();
}

void MyFrame::OnServerName( wxCommandEvent& WXUNUSED(event) )
{
    if ( GetServername()->GetStringSelection() == "..." )
    {
        wxString s = wxGetTextFromUser
                     (
                        "Specify the name of the server",
                        "Server Name",
                        "",
                        this
                     );

        if ( !s.empty() && s != IPC_SERVICE )
        {
            GetServername()->Insert(s, 0);
            GetServername()->SetSelection(0);
        }
    }
}

void MyFrame::Disconnect()
{
    m_server->Disconnect();
    UpdateUI();
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
    wxLogMessage("OnAcceptConnection(\"%s\")", topic);

    if ( topic == IPC_TOPIC )
    {
        m_connection = new MyConnection();
        wxGetApp().GetFrame()->UpdateUI();
        wxLogMessage("Connection accepted");
        return m_connection;
    }
    //else: unknown topic

    wxLogMessage("Unknown topic, connection refused");
    return NULL;
}

void MyServer::Disconnect()
{
    if ( m_connection )
    {
        wxDELETE(m_connection);
        wxGetApp().GetFrame()->UpdateUI();
        wxLogMessage("Disconnected client");
    }
}

void MyServer::Advise()
{
    if ( CanAdvise() )
    {
        const wxDateTime now = wxDateTime::Now();

        wxString str = wxString::FromUTF8("\xd0\x9f\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
        m_connection->Advise(m_connection->m_advise, str + " (using UTF-8)");

        str += " (using wchar_t)";
        m_connection->Advise(m_connection->m_advise,
                             str.wc_str(), (str.length() + 1)*sizeof(wchar_t),
                             wxIPC_UNICODETEXT);

        // This one uses wxIPC_TEXT by default.
        const wxString s = now.FormatTime() + " " + now.FormatDate();
        m_connection->Advise(m_connection->m_advise, s.mb_str(), wxNO_LEN);

        char bytes[3] = { '1', '2', '3' };
        m_connection->Advise(m_connection->m_advise, bytes, 3, wxIPC_PRIVATE);
    }
}

// ----------------------------------------------------------------------------
// MyConnection
// ----------------------------------------------------------------------------

bool
MyConnection::OnExecute(const wxString& topic,
                        const void *data,
                        size_t size,
                        wxIPCFormat format)
{
    Log("OnExecute", topic, "", data, size, format);
    return true;
}

bool
MyConnection::OnPoke(const wxString& topic,
                     const wxString& item,
                     const void *data,
                     size_t size,
                     wxIPCFormat format)
{
    Log("OnPoke", topic, item, data, size, format);
    return wxConnection::OnPoke(topic, item, data, size, format);
}

const void *
MyConnection::OnRequest(const wxString& topic,
                        const wxString& item,
                        size_t *size,
                        wxIPCFormat format)
{
    *size = 0;

    wxString s,
             afterDate;
    if ( item.StartsWith("Date", &afterDate) )
    {
        const wxDateTime now = wxDateTime::Now();

        if ( afterDate.empty() )
        {
            s = now.Format();
            *size = wxNO_LEN;
        }
        else if ( afterDate == "+len" )
        {
            s = now.FormatTime() + " " + now.FormatDate();
            *size = strlen(s.mb_str()) + 1;
        }
    }
    else if ( item == "bytes[3]" )
    {
        s = "123";
        *size = 3;
    }

    if ( !*size )
    {
        wxLogMessage("Unknown request for \"%s\"", item);
        return NULL;
    }

    // store the data pointer to which we return in a member variable to ensure
    // that the pointer remains valid even after we return
    m_requestData = s.mb_str();
    const void * const data = m_requestData;
    Log("OnRequest", topic, item, data, *size, format);
    return data;
}

bool MyConnection::OnStartAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage("OnStartAdvise(\"%s\", \"%s\")", topic, item);
    wxLogMessage("Returning true");
    m_advise = item;
    wxGetApp().GetFrame()->UpdateUI();
    return true;
}

bool MyConnection::OnStopAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage("OnStopAdvise(\"%s\",\"%s\")", topic, item);
    wxLogMessage("Returning true");
    m_advise.clear();
    wxGetApp().GetFrame()->UpdateUI();
    return true;
}

bool
MyConnection::DoAdvise(const wxString& item,
                       const void *data,
                       size_t size,
                       wxIPCFormat format)
{
    Log("Advise", "", item, data, size, format);
    return wxConnection::DoAdvise(item, data, size, format);
}

bool MyConnection::OnDisconnect()
{
    wxLogMessage("OnDisconnect()");
    wxGetApp().GetFrame()->Disconnect();
    return true;
}
