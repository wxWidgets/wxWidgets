/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     Server for wxSocket demo
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Modified by:
// Created:     1999/09/19
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
#  pragma implementation "server.cpp"
#  pragma interface "server.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers 
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#  include "wx/socket.h"
#endif

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#  include "mondrian.xpm"
#endif

// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class MyApp : public wxApp
{
public:
  virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
  MyFrame();
  ~MyFrame();

  // event handlers (these functions should _not_ be virtual)
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnServerEvent(wxSocketEvent& event);
  void OnSocketEvent(wxSocketEvent& event);

  void Test1(wxSocketBase *sock);
  void Test2(wxSocketBase *sock);
  void Test3(wxSocketBase *sock);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketServer *m_server;
  wxPanel        *m_panel;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenuBar      *m_menuBar;
  bool            m_busy;
  int             m_numClients;

  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  SERVER_QUIT = 1000,
  SERVER_ABOUT,

  // id for sockets
  SERVER_ID,
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SERVER_QUIT,  MyFrame::OnQuit)
  EVT_MENU(SERVER_ABOUT, MyFrame::OnAbout)
  EVT_SOCKET(SERVER_ID,  MyFrame::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  MyFrame::OnSocketEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


// To append sockets for delayed deletion
extern wxList wxPendingDelete;


// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// the application class
// --------------------------------------------------------------------------

bool MyApp::OnInit()
{
  // Create the main application window
  MyFrame *frame = new MyFrame();

  // Show it and tell the application that it's our main window
  frame->Show(TRUE);
  SetTopWindow(frame);

  // success
  return TRUE;
}

// --------------------------------------------------------------------------
// main frame
// --------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame() : wxFrame((wxFrame *)NULL, -1,
                             _T("wxSocket demo: Server"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(mondrian));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(SERVER_ABOUT, _T("&About...\tCtrl-A"), _T("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(SERVER_QUIT, _T("E&xit\tAlt-X"), _T("Quit server"));

  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _T("&File"));
  SetMenuBar(m_menuBar);

  // Status bar
  CreateStatusBar(2);

  // Make a panel with a textctrl in it
  m_panel = new wxPanel(this, -1, wxPoint(0, 0), GetClientSize());
  m_text  = new wxTextCtrl(m_panel, -1,
                           _T("Welcome to wxSocket demo: Server\n"),
                           wxPoint(0, 0), m_panel->GetClientSize(),
                           wxTE_MULTILINE | wxTE_READONLY);

  // Create the socket
  wxIPV4address addr;
  addr.Service(3000);

  m_server = new wxSocketServer(addr);
  m_server->SetEventHandler(*this, SERVER_ID);
  m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
  m_server->Notify(TRUE);

  // We use Ok() here to see if the server is really listening
  if (m_server->Ok())
    m_text->AppendText(_T("Server listening.\n\n"));
  else
    m_text->AppendText(_T("Could not listen at the specified port !\n\n"));

  m_busy = FALSE;
  m_numClients = 0;
  UpdateStatusBar();
}

MyFrame::~MyFrame()
{
  delete m_server;
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_T("wxSocket demo: Server\n")
               _T("(c) 1999 Guillermo Rodriguez Garcia\n"),
               _T("About Server"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::Test1(wxSocketBase *sock)
{
  unsigned char len;
  char *buf;

  m_text->AppendText(_T("Test 1 begins\n"));

  // Receive data from socket and send it back. We will first
  // get a byte with the buffer size, so we can specify the
  // exact size and use the WAITALL flag. Also, we disabled
  // input events so we won't have unwanted reentrance. This
  // way we can avoid the infamous BLOCK (formerly SPEED) flag.
  //
  sock->SetFlags(wxSOCKET_WAITALL);

  sock->Read((char *)&len, 1);

  buf = (char *)malloc(len);
  sock->Read(buf, len);
  sock->Write(buf, len);
  free(buf);

  m_text->AppendText(_T("Test 1 ends\n"));
}

void MyFrame::Test2(wxSocketBase *sock)
{
#define MAX_MSG_SIZE 10000

  wxString s;
  char *buf = (char *)malloc(MAX_MSG_SIZE);
  wxUint32 len;

  m_text->AppendText(_T("Test 2 begins\n"));

  // We don't need to set flags because ReadMsg and WriteMsg
  // are not affected by them anyway.
  //
  len = sock->ReadMsg(buf, MAX_MSG_SIZE).LastCount();

  s.Printf(_T("Client says: %s\n"), buf);
  m_text->AppendText(s);

  sock->WriteMsg(buf, len);
  free(buf);

  m_text->AppendText(_T("Test 2 ends\n"));

#undef MAX_MSG_SIZE
}

void MyFrame::Test3(wxSocketBase *sock)
{
  m_text->AppendText(_T("Test 3 begins\n"));
  m_text->AppendText(_T("(not implemented)\n"));
  m_text->AppendText(_T("Test 3 ends\n"));
}

void MyFrame::OnServerEvent(wxSocketEvent& event)
{
  wxString s = _T("OnServerEvent: ");
  wxSocketBase *sock;

  switch(event.SocketEvent())
  {
    case wxSOCKET_CONNECTION : s.Append(_T("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_T("Unexpected event !\n")); break;
  }

  m_text->AppendText(s);

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(FALSE) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).
  //
  sock = m_server->Accept(FALSE);

  if (sock)
  {
    m_text->AppendText(_T("New client connection accepted\n"));
  }
  else
  {
    m_text->AppendText(_T("Error: couldn't accept a new connection"));
    return;
  }

  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(TRUE);

  m_numClients++;
  UpdateStatusBar();
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxSocketBase *sock = event.Socket();
  wxString s = _T("OnSocketEvent: ");

  // We first print a msg
  switch(event.SocketEvent())
  {
    case wxSOCKET_INPUT: s.Append(_T("wxSOCKET_INPUT\n")); break;
    case wxSOCKET_LOST:  s.Append(_T("wxSOCKET_LOST\n")); break;
    default:             s.Append(_T("unexpected event !\n"));
  }

  m_text->AppendText(s);

  // Now we process the event
  switch(event.SocketEvent())
  {
    case wxSOCKET_INPUT:
    {
      // We disable input events, so that the test doesn't trigger
      // wxSocketEvent again.
      sock->SetNotify(wxSOCKET_LOST_FLAG);

      // Which test are we going to run?
      unsigned char c;
      sock->Read((char *)&c ,1);

      switch (c)
      {
        case 0xBE: Test1(sock); break;
        case 0xCE: Test2(sock); break;
        case 0xDE: Test3(sock); break;
        default: s.Append(_T("Unknown test id received from client\n"));
      }

      // Enable input events again.
      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      m_numClients--;

      // We cannot delete the socket right now because we can
      // be in the middle of a test or something. So we append
      // it to the list of objects to be deleted.
      m_text->AppendText(_T("Deleting socket.\n"));
      wxPendingDelete.Append(sock);
      break;
    }
    default: ;
  }

  UpdateStatusBar();
}

// convenience functions

void MyFrame::UpdateStatusBar()
{
  wxString s;
  s.Printf(_T("%d clients connected"), m_numClients);
  SetStatusText(s, 1);
}
