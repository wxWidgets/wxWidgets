/////////////////////////////////////////////////////////////////////////////
// Name:        server.h
// Purpose:     DDE sample: server
// Author:      Julian Smart
// Modified by:
// Created:     25/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyServer;
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    MyServer *m_server;
};

DECLARE_APP(MyApp)

// Define a new frame
class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title);

    void OnListBoxClick(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

private:
    wxPanel *panel;

    DECLARE_EVENT_TABLE()
};

class IPCDialogBox;
class MyConnection : public wxConnection
{
public:
    MyConnection(char *buf, int size);
    ~MyConnection();

    bool OnExecute(const wxString& topic, char *data, int size, wxIPCFormat format);
    char *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
    bool OnPoke(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format);
    bool OnStartAdvise(const wxString& topic, const wxString& item);

private:
    IPCDialogBox *dialog;
};

class MyServer: public wxServer
{
public:
    wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

class IPCDialogBox: public wxDialog
{
public:
    IPCDialogBox(wxWindow *parent,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 MyConnection *the_connection);

    void OnQuit(wxCommandEvent& event);

private:
    MyConnection *m_connection;

    DECLARE_EVENT_TABLE()
};

#define SERVER_EXIT         wxID_EXIT
#define SERVER_LISTBOX      500
#define SERVER_QUIT_BUTTON  501
