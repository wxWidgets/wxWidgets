/////////////////////////////////////////////////////////////////////////////
// Name:        client.h
// Purpose:     DDE sample: client
// Author:      Julian Smart
// Modified by:
// Created:     25/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title);

    void OnExit(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnPoke(wxCommandEvent& event);
    void OnRequest(wxCommandEvent& event);

private:
    wxPanel *panel;

    DECLARE_EVENT_TABLE()
};

class MyConnection: public wxConnection
{
public:
    MyConnection();
    ~MyConnection();

    bool OnAdvise(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format);
    bool OnDisconnect();
};

class MyClient: public wxClient
{
public:
    wxConnectionBase *OnMakeConnection();
};

#define CLIENT_QUIT     wxID_EXIT
#define CLIENT_EXECUTE  2
#define CLIENT_REQUEST  3
#define CLIENT_POKE     4
#define CLIENT_LISTBOX  200
