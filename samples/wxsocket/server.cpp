/*
 * File:	server.cpp
 * Purpose:	wxSocket: server demo
 * Author:	LAVAUX Guilhem
 * Created:	June 1997
 * Updated:	
 * Copyright: (C) 1997, LAVAUX Guilhem
 */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"

#if defined(__WXMOTIF__) || defined(__WXGTK__)
#include "mondrian.xpm"
#endif

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

class MyServer;

// Define a new frame type
class MyFrame: public wxFrame
{ 
  DECLARE_EVENT_TABLE()
public:
  wxSocketServer *sock;
  int nb_clients;

  MyFrame(wxFrame *frame);
  virtual ~MyFrame();
  void Menu_Exit(wxCommandEvent& evt);
  void OnSockRequest(wxSocketEvent& evt);
  void OnSockRequestServer(wxSocketEvent& evt);
  void ExecTest1(wxSocketBase *sock_o);
  void UpdateStatus(int incr);
};

#define SKDEMO_QUIT 101
#define SKDEMO_SOCKET_SERV 102
#define SKDEMO_SOCKET 103

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SKDEMO_QUIT, MyFrame::Menu_Exit)
  EVT_SOCKET(SKDEMO_SOCKET_SERV, MyFrame::OnSockRequestServer)
  EVT_SOCKET(SKDEMO_SOCKET, MyFrame::OnSockRequest)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL);

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(SKDEMO_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  (void)new wxPanel(frame, 0, 0, 300, 100);

  // Show the frame
  frame->Show(TRUE);
  
  // Return the main frame window
  return TRUE;
}

extern wxList wxPendingDelete;

void MyFrame::OnSockRequest(wxSocketEvent& evt)
{
  wxSocketBase *sock = evt.Socket();

  printf("OnSockRequest OK\n");
  printf("OnSockRequest (event = %d)\n",evt.SocketEvent());
  switch (evt.SocketEvent()) {
  case wxSocketBase::EVT_READ:
    unsigned char c;

    sock->Read((char *)&c, 1);
    if (c == 0xbe)
      ExecTest1(sock);

    break;
  case wxSocketBase::EVT_LOST:
    UpdateStatus(-1);
    printf("Destroying socket\n");
    wxPendingDelete.Append(sock);
    return;
    break;
  }
  printf("OnSockRequest Exiting\n");
  sock->SetNotify(wxSocketBase::REQ_READ | wxSocketBase::REQ_LOST);
}

void MyFrame::OnSockRequestServer(wxSocketEvent& evt)
{
  wxSocketBase *sock2;
  wxSocketServer *server = (wxSocketServer *) evt.Socket();

  printf("OnSockRequestServer OK\n");

  sock2 = server->Accept();
  if (sock2 == NULL)
    return;

  sock2->SetFlags(wxSocketBase::NONE);
  sock2->Notify(TRUE);
  sock2->SetEventHandler(*this, SKDEMO_SOCKET);
  server->SetNotify(wxSocketBase::REQ_ACCEPT);
  UpdateStatus(1);
}

// My frame Constructor
MyFrame::MyFrame(wxFrame *frame):
  wxFrame(frame, -1, "wxSocket sample (server)", wxDefaultPosition,
          wxSize(300, 200))
{
  wxIPV4address addr;
  addr.Service(3000);

  // Init all
  wxSocketHandler::Master();

  sock = new wxSocketServer(addr);
  wxSocketHandler::Master().Register(sock);
  sock->SetNotify(wxSocketBase::REQ_ACCEPT);
  sock->SetEventHandler(*this, SKDEMO_SOCKET_SERV);
  sock->Notify(TRUE);
  nb_clients = 0;

  CreateStatusBar(1);
  UpdateStatus(0);
}

MyFrame::~MyFrame()
{
  delete sock;
}

// Intercept menu commands
void MyFrame::Menu_Exit(wxCommandEvent& WXUNUSED(event))
{
  Close(TRUE);
}

void MyFrame::ExecTest1(wxSocketBase *sock_o)
{
  char *buf = new char[50];
  size_t l;

  l = sock_o->Read(buf, 50).LastCount();
  sock_o->Write(buf, l);
}

void MyFrame::UpdateStatus(int incr)
{
  char s[30];
  nb_clients += incr;
  sprintf(s, "%d clients connected", nb_clients);
  SetStatusText(s);
}
