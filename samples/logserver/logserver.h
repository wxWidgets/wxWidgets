/////////////////////////////////////////////////////////////////////////////
// Name:        logserver.h
// Purpose:     Logserver sample/tool
// Author:      Carlos Alberto Rodrigues Azevedo
// Created:     04/09/2022
// Copyright:   (c) Carlos Alberto Rodrigues Azevedo
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#define wxUSE_DDE_FOR_IPC 0

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/dataview.h>
    #include <wx/ipc.h>
#endif

#include "logctrl.h"

/*
#include "connection.h"
*/

// Define a new application
class LogFrame;
class LogServer;

class LogServerApp : public wxApp
{
    public:
        virtual bool OnInit() wxOVERRIDE;
        LogFrame *GetFrame() { return m_frame; }
    protected:
        LogFrame *m_frame;
};

wxDECLARE_APP(LogServerApp);

// Define a new frame
class LogFrame : public wxFrame
{
    const wxWindowIDRef ID_CLEAR = wxWindow::NewControlId();
    const wxWindowIDRef ID_COPY = wxWindow::NewControlId();
    const wxWindowIDRef ID_TRACK = wxWindow::NewControlId();

    public:
        LogFrame(wxFrame *frame, const wxString& title);
        LogDataModel* GetDataModel() { return m_tree_model; }
        void CreateServer();
    private:
        LogServer* m_server;
        wxTextCtrl* m_port;
        wxDataViewCtrl* m_tree_ctrl;
        wxDataViewColumn* msg_col;
        LogDataModel* m_tree_model;
        wxTimer m_refresh_timer;
        bool m_auto_refresh;

        void OnClose(wxCloseEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnRestart(wxCommandEvent &event);
        void OnMenu(wxDataViewEvent& WXUNUSED(event));
        void OnClearLog(wxCommandEvent& event);
        void OnCopySelectedLog(wxCommandEvent& event);
        void OnRefresh(wxTimerEvent& event);
        void OnShowLast(wxCommandEvent& event);
};

class LogServer;

class LogConnection : public wxConnection
{
    public:
        LogConnection(LogServer* server, LogDataModel* model) : m_server(server), m_model(model) {}
        virtual bool OnExecute(const wxString& topic, const void *data,
                               size_t size, wxIPCFormat format) wxOVERRIDE { return false; }
        virtual const void* OnRequest(const wxString& topic, const wxString& item, size_t *size,
                                      wxIPCFormat format) wxOVERRIDE  { return nullptr; }
        virtual bool OnPoke(const wxString& topic, const wxString& item, const void *data, size_t size, wxIPCFormat format) wxOVERRIDE;
        virtual bool OnStartAdvise(const wxString& topic, const wxString& item) wxOVERRIDE { return false; }
        virtual bool OnStopAdvise(const wxString& topic, const wxString& item) wxOVERRIDE { return false; }
        virtual bool DoAdvise(const wxString& item, const void *data, size_t size, wxIPCFormat format) wxOVERRIDE { return false; }
        virtual bool OnDisconnect() wxOVERRIDE;
    private:
        LogServer* m_server;
        LogDataModel* m_model;
};

class LogServer : public wxServer
{
    public:
        LogServer();
        virtual ~LogServer();
        void Disconnect();
        bool IsConnected() { return m_connection != NULL; }
        LogConnection *GetConnection() { return m_connection; }
    private:
        LogConnection* m_connection;

        virtual wxConnectionBase* OnAcceptConnection(const wxString& topic) wxOVERRIDE;
};
