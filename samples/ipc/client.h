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
    bool OnInit();
    int OnExit();
};

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;

    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnPoke(wxCommandEvent& event);
    void OnRequest(wxCommandEvent& event);
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
  MyClient();
  wxConnectionBase *OnMakeConnection();
};

#define CLIENT_QUIT     wxID_EXIT
#define CLIENT_EXECUTE  2
#define CLIENT_REQUEST  3
#define CLIENT_POKE     4
#define CLIENT_LISTBOX  200
