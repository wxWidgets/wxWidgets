/*
 * File:	server.cpp
 * Purpose:	wxSocket: server demo
 * Author:	LAVAUX Guilhem (from minimal.cc)
 * Created:	June 1997
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 *		(C) 1997, LAVAUX Guilhem
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
  MyServer *sock;
  int nb_clients;

  MyFrame(wxFrame *frame);
  virtual ~MyFrame();
  void Menu_Exit(wxCommandEvent& evt);
  void ExecTest1(wxSocketBase *sock_o);
  void UpdateStatus(int incr);
};

#define SKDEMO_QUIT 101

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SKDEMO_QUIT, MyFrame::Menu_Exit)
END_EVENT_TABLE()

class MySock: public wxSocketBase {
public:
  MyFrame *frame;

  void OldOnNotify(wxRequestEvent flags);
};

class MyServer: public wxSocketServer {
public:
  MyFrame *frame;

  MyServer(wxSockAddress& addr) : wxSocketServer(addr) { }
  void OldOnNotify(wxRequestEvent flags);
};

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("mondrian"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

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

void MySock::OldOnNotify(wxRequestEvent flags)
{
  extern wxList WXDLLEXPORT wxPendingDelete;

  switch (flags) {
  case EVT_READ:
    unsigned char c;

    ReadMsg((char *)&c, 1);
    if (c == 0xbe)
      frame->ExecTest1(this);

    break;
  case EVT_LOST:
    frame->UpdateStatus(-1);
    wxPendingDelete.Append(this);
    break;
  }
}

void MyServer::OldOnNotify(wxRequestEvent WXUNUSED(flags))
{
  MySock *sock2 = new MySock();

  if (!AcceptWith(*sock2))
    return;

  m_handler->Register(sock2);

  sock2->SetFlags(NONE);
  sock2->frame = frame;
  sock2->SetNotify(REQ_READ | REQ_LOST);
  sock2->Notify(TRUE);
  frame->UpdateStatus(1);
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

  sock = new MyServer(addr);
  wxSocketHandler::Master().Register(sock);
  sock->frame = this;
  sock->SetNotify(wxSocketBase::REQ_ACCEPT);
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
