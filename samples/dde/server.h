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
class MyApp: public wxApp
{
  public:
    bool OnInit();
};

DECLARE_APP(MyApp)

// Define a new frame
class MyFrame: public wxFrame
{
  public:
    wxPanel *panel;

    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnListBoxClick(wxCommandEvent& event);
DECLARE_EVENT_TABLE()
};

class IPCDialogBox;
class MyConnection: public wxConnection
{
 public:
  IPCDialogBox *dialog;

  MyConnection(char *buf, int size);
  ~MyConnection();

  bool OnExecute(const wxString& topic, char *data, int size, wxIPCFormat format);
  char *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
  bool OnPoke(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format);
  bool OnStartAdvise(const wxString& topic, const wxString& item);
};

class MyServer: public wxServer
{
public:
    wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

class IPCDialogBox: public wxDialog
{
public:
    MyConnection *connection;
    IPCDialogBox(wxFrame *parent, const wxString& title,
                         const wxPoint& pos, const wxSize& size, MyConnection *the_connection);

    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#define SERVER_QUIT         wxID_EXIT
#define SERVER_LISTBOX      500
#define SERVER_QUIT_BUTTON  501
