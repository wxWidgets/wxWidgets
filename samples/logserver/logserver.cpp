/////////////////////////////////////////////////////////////////////////////
// Name:        logserver.cpp
// Purpose:     Logserver sample/tool
// Author:      Carlos Alberto Rodrigues Azevedo
// Created:     04/09/2022
// Copyright:   (c) Carlos Alberto Rodrigues Azevedo
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/valnum.h>
    #include <wx/tokenzr.h>
    #include <wx/settings.h>
    #include <wx/clipbrd.h>
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "logserver.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(LogServerApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// LogServer
// ----------------------------------------------------------------------------

bool LogServerApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    m_frame = new LogFrame(NULL, "Log Server");
    m_frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// LogFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
LogFrame::LogFrame(wxFrame *frame, const wxString& title) : wxFrame(frame, wxID_ANY, title), m_refresh_timer(this)
{
    SetIcon(wxICON(sample));

    m_server = NULL;

    wxPanel* panel = new wxPanel(this);

    wxBoxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Port:");
    hsizer->Add(label, 0, wxLEFT | wxRIGHT | wxALIGN_CENTRE_VERTICAL, 8);
    m_port = new wxTextCtrl(panel, wxID_ANY, "4242");
    hsizer->Add(m_port, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 8);
    hsizer->AddStretchSpacer();
    wxButton* btn = new wxButton(panel, wxID_ANY, "&Restart Server");
    hsizer->Add(btn, 0, wxRIGHT, 8);
    vsizer->Add(hsizer, 0, wxEXPAND);

    // Create the tree control to show the log entries
    m_tree_ctrl = new wxDataViewCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);
    m_tree_ctrl->SetMinSize(wxSize(400, 300));
    m_tree_model = new LogDataModel(m_tree_ctrl);
    m_tree_ctrl->AssociateModel(m_tree_model);
    m_tree_model->DecRef();
    // Create the message column
    unsigned ts_col_width = m_tree_ctrl->GetTextExtent("Timestamp").GetWidth() + 24;
    wxDataViewTextRenderer *tr = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    msg_col = new wxDataViewColumn("Message", tr, 0, FromDIP(200), wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    msg_col->SetMinWidth(FromDIP(m_tree_ctrl->GetClientRect().GetWidth() - ts_col_width) - 4);
    m_tree_ctrl->AppendColumn(msg_col);
    // Create the timestamp column
    tr = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
    wxDataViewColumn* ts_col = new wxDataViewColumn("Timestamp", tr, 1, FromDIP(ts_col_width),
                                                    wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
    m_tree_ctrl->AppendColumn(ts_col);
    vsizer->Add(m_tree_ctrl, 1, wxEXPAND | wxALL, 8);

    panel->SetSizer(vsizer);
    vsizer->SetSizeHints(panel);
    SetClientSize(panel->GetSize());

    btn->Bind(wxEVT_BUTTON, &LogFrame::OnRestart, this);
    Bind(wxEVT_CLOSE_WINDOW, &LogFrame::OnClose, this);
    Bind(wxEVT_SIZE, &LogFrame::OnSize, this);
    m_tree_ctrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &LogFrame::OnMenu, this);

    m_auto_refresh = false;
    Bind(wxEVT_TIMER, &LogFrame::OnRefresh, this);
    m_refresh_timer.Start(1000);

    CreateServer();
}

void LogFrame::CreateServer()
{
    unsigned long port_number;
    if (m_port->GetValue().ToULong(&port_number) ? (port_number > 65535) : true)
    {
        m_port->SetFocus();
        return;
    }
    // Create a new server
    m_server = new LogServer;
    if (m_server->Create(wxString::Format("%lu", port_number)))
    {
        m_tree_model->Start();
    }
    else
    {
        m_tree_model->Fail();
        wxDELETE(m_server);
    }
}

void LogFrame::OnClose(wxCloseEvent& event)
{
    if (m_server != nullptr) m_server->Disconnect();
    wxDELETE(m_server);
    event.Skip();
}

void LogFrame::OnSize(wxSizeEvent& event)
{
    unsigned ts_col_width = m_tree_ctrl->GetTextExtent("Timestamp").GetWidth() + 24;
    msg_col->SetMinWidth(FromDIP(m_tree_ctrl->GetClientRect().GetWidth() - ts_col_width) - 4);
    event.Skip();
}

void LogFrame::OnRestart(wxCommandEvent& WXUNUSED(event))
{
    // Disconnect the old connection, if active
    if (m_server != nullptr) m_server->Disconnect();
    wxDELETE(m_server);
    m_tree_model->Clear();
    CreateServer();
}

void LogFrame::OnMenu(wxDataViewEvent& WXUNUSED(event))
{
    wxMenu menu;
    menu.Append(ID_CLEAR, "Clear");
    Bind(wxEVT_MENU, &LogFrame::OnClearLog, this, ID_CLEAR);
    menu.AppendSeparator();
    menu.Append(ID_COPY, "Copy selected");
    Bind(wxEVT_MENU, &LogFrame::OnCopySelectedLog, this, ID_COPY);
    menu.AppendSeparator();
    wxMenuItem* check = menu.AppendCheckItem(ID_TRACK, "Always show last");
    check->Check(m_auto_refresh);
    Bind(wxEVT_MENU, &LogFrame::OnShowLast, this, ID_TRACK);
    PopupMenu(&menu);
}

void LogFrame::OnClearLog(wxCommandEvent& event)
{
    m_tree_model->Clear();
    m_tree_model->Reset();
}

void LogFrame::OnCopySelectedLog(wxCommandEvent& event)
{
    if (wxTheClipboard->Open())
    {
        wxDataViewItemArray selected;
        int count = m_tree_ctrl->GetSelections(selected);
        wxString text;
        text.Alloc(1024);
        for (int i = 0; i < count; i++)
        {
            wxVariant data;
            m_tree_model->GetValue(data, selected[i], 0);
            if (!text.IsEmpty()) text.Append("\n");
            text.Append(data.GetString());
        }
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}

void LogFrame::OnRefresh(wxTimerEvent& event)
{
    if (m_auto_refresh) m_tree_ctrl->EnsureVisible(m_tree_model->GetLastEntry());
}

void LogFrame::OnShowLast(wxCommandEvent& event)
{
    m_auto_refresh = event.IsChecked();
}

// ----------------------------------------------------------------------------
// LogServer
// ----------------------------------------------------------------------------

LogServer::LogServer() : wxServer()
{
    m_connection = nullptr;
}

LogServer::~LogServer()
{
    Disconnect();
}

void LogServer::Disconnect()
{
    wxDELETE(m_connection);
}

wxConnectionBase* LogServer::OnAcceptConnection(const wxString& topic)
{
    if ((topic == "LOG_MESSAGE") && (m_connection == nullptr))
    {
        m_connection = new LogConnection(this, wxGetApp().GetFrame()->GetDataModel());
        return m_connection;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------
// LogConnection
// ----------------------------------------------------------------------------

bool LogConnection::OnPoke(const wxString& topic, const wxString& item, const void* data, size_t size, wxIPCFormat format)
{
    wxString text(wxString::FromUTF8((const char*)data, size));
    wxStringTokenizer params(text, "\r");
    if (params.HasMoreTokens())
    {
        wxString msg(params.GetNextToken());
        if (params.HasMoreTokens())
        {
            wxColour colour;
            wxString clr_text(params.GetNextToken());
            if (clr_text.StartsWith("00"))
                colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            else
                colour = wxColour("#" + clr_text.Mid(2));
            if (params.HasMoreTokens())
            {
                wxDateTime ts;
                ts.ParseTime(params.GetNextToken());
                m_model->AddEntry(msg, colour, ts);
            }
        }
    }
    return true;
}

bool LogConnection::OnDisconnect()
{
    m_server->Disconnect();
    m_model = nullptr;
    return true;
}
