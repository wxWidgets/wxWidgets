/////////////////////////////////////////////////////////////////////////////
// Name:        client.h
// Purpose:     DDE sample: client
// Author:      Julian Smart
// Created:     25/01/99
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "connection.h"

#define ID_START         10000
#define ID_DISCONNECT    10001
#define ID_STARTADVISE    10002
#define ID_LOG          10003
#define ID_SERVERNAME    10004
#define ID_STOPADVISE    10005
#define ID_POKE            10006
#define ID_REQUEST        10007
#define ID_EXECUTE        10008
#define ID_TOPIC        10009
#define ID_HOSTNAME        10010

// Define a new application
class MyClient;
class MyFrame;

class MyApp: public wxApp
{
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;
    MyFrame *GetFrame() { return m_frame; }

protected:
    MyFrame        *m_frame;
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title);

    void OnExit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void EnableControls();
    void Disconnect();

protected:
    wxButton* GetStart()  { return (wxButton*) FindWindow( ID_START ); }
    wxChoice* GetServername()  { return (wxChoice*) FindWindow( ID_SERVERNAME ); }
    wxChoice* GetHostname()  { return (wxChoice*) FindWindow( ID_HOSTNAME ); }
    wxChoice* GetTopic()  { return (wxChoice*) FindWindow( ID_TOPIC ); }
    wxButton* GetDisconnect()  { return (wxButton*) FindWindow( ID_DISCONNECT ); }
    wxButton* GetStartAdvise()  { return (wxButton*) FindWindow( ID_STARTADVISE ); }
    wxButton* GetStopAdvise()  { return (wxButton*) FindWindow( ID_STOPADVISE ); }
    wxButton* GetRequest()  { return (wxButton*) FindWindow( ID_REQUEST ); }
    wxButton* GetPoke()  { return (wxButton*) FindWindow( ID_POKE ); }
    wxButton* GetExecute()  { return (wxButton*) FindWindow( ID_EXECUTE ); }
    wxTextCtrl* GetLog()  { return (wxTextCtrl*) FindWindow( ID_LOG ); }

    MyClient         *m_client;

    void OnStart( wxCommandEvent &event );
    void OnServername( wxCommandEvent &event );
    void OnHostname( wxCommandEvent &event );
    void OnTopic( wxCommandEvent &event );
    void OnDisconnect( wxCommandEvent &event );
    void OnStartAdvise( wxCommandEvent &event );
    void OnStopAdvise( wxCommandEvent &event );
    void OnExecute(wxCommandEvent& event);
    void OnPoke(wxCommandEvent& event);
    void OnRequest(wxCommandEvent& event);

protected:
    wxDECLARE_EVENT_TABLE();
};

class MyConnection : public MyConnectionBase
{
public:
    virtual bool DoExecute(const void *data, size_t size, wxIPCFormat format) override;
    virtual const void *Request(const wxString& item, size_t *size = nullptr, wxIPCFormat format = wxIPC_TEXT) override;
    virtual bool DoPoke(const wxString& item, const void* data, size_t size, wxIPCFormat format) override;
    virtual bool OnAdvise(const wxString& topic, const wxString& item, const void *data, size_t size, wxIPCFormat format) override;
    virtual bool OnDisconnect() override;
};

class MyClient: public wxClient
{
public:
    MyClient();
    ~MyClient();
    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();
    wxConnectionBase *OnMakeConnection() override;
    bool IsConnected() { return m_connection != nullptr; }
    MyConnection *GetConnection() { return m_connection; }

protected:
    MyConnection     *m_connection;
};
