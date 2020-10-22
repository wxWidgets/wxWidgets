/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     Server for wxSocket demo
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Created:     1999/09/19
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
//              (c) 2009 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/busyinfo.h"
#include "wx/socket.h"

// this example is currently written to use only IP or only IPv6 sockets, it
// should be extended to allow using either in the future
#if wxUSE_IPV6
    typedef wxIPV6address IPaddress;
#else
    typedef wxIPV4address IPaddress;
#endif

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class MyApp : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
  MyFrame();
  ~MyFrame();

  // event handlers (these functions should _not_ be virtual)
  void OnUDPTest(wxCommandEvent& event);
  void OnWaitForAccept(wxCommandEvent& event);
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
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenuBar      *m_menuBar;
  bool            m_busy;
  int             m_numClients;

  // any class wishing to process wxWidgets events must use this macro
  wxDECLARE_EVENT_TABLE();
};

// simple helper class to log start and end of each test
class TestLogger
{
public:
    TestLogger(const wxString& name) : m_name(name)
    {
        wxLogMessage("=== %s begins ===", m_name);
    }

    ~TestLogger()
    {
        wxLogMessage("=== %s ends ===", m_name);
    }

private:
    const wxString m_name;
};

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  SERVER_UDPTEST = 10,
  SERVER_WAITFORACCEPT,
  SERVER_QUIT = wxID_EXIT,
  SERVER_ABOUT = wxID_ABOUT,

  // id for sockets
  SERVER_ID = 100,
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(SERVER_QUIT,  MyFrame::OnQuit)
  EVT_MENU(SERVER_ABOUT, MyFrame::OnAbout)
  EVT_MENU(SERVER_UDPTEST, MyFrame::OnUDPTest)
  EVT_MENU(SERVER_WAITFORACCEPT, MyFrame::OnWaitForAccept)
  EVT_SOCKET(SERVER_ID,  MyFrame::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  MyFrame::OnSocketEvent)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);


// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// the application class
// --------------------------------------------------------------------------

bool MyApp::OnInit()
{
  if ( !wxApp::OnInit() )
      return false;

  // Create the main application window
  MyFrame *frame = new MyFrame();

  // Show it
  frame->Show(true);

  // Success
  return true;
}

// --------------------------------------------------------------------------
// main frame
// --------------------------------------------------------------------------

// frame constructor

MyFrame::MyFrame() : wxFrame((wxFrame *)NULL, wxID_ANY,
                             _("wxSocket demo: Server"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(sample));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(SERVER_WAITFORACCEPT, "&Wait for connection\tCtrl-W");
  m_menuFile->Append(SERVER_UDPTEST, "&UDP test\tCtrl-U");
  m_menuFile->AppendSeparator();
  m_menuFile->Append(SERVER_ABOUT, _("&About\tCtrl-A"), _("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(SERVER_QUIT, _("E&xit\tAlt-X"), _("Quit server"));

  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _("&File"));
  SetMenuBar(m_menuBar);

#if wxUSE_STATUSBAR
  // Status bar
  CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

  // Make a textctrl for logging
  m_text  = new wxTextCtrl(this, wxID_ANY,
                           _("Welcome to wxSocket demo: Server\n"),
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_READONLY);
  delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));

  // Create the address - defaults to localhost:0 initially
  IPaddress addr;
  addr.Service(3000);

  wxLogMessage("Creating server at %s:%u", addr.IPAddress(), addr.Service());

  // Create the socket
  m_server = new wxSocketServer(addr);

  // We use IsOk() here to see if the server is really listening
  if (! m_server->IsOk())
  {
    wxLogMessage("Could not listen at the specified port !");
    return;
  }

  IPaddress addrReal;
  if ( !m_server->GetLocal(addrReal) )
  {
    wxLogMessage("ERROR: couldn't get the address we bound to");
  }
  else
  {
    wxLogMessage("Server listening at %s:%u",
                 addrReal.IPAddress(), addrReal.Service());
  }

  // Setup the event handler and subscribe to connection events
  m_server->SetEventHandler(*this, SERVER_ID);
  m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
  m_server->Notify(true);

  m_busy = false;
  m_numClients = 0;
  UpdateStatusBar();
}

MyFrame::~MyFrame()
{
  // No delayed deletion here, as the frame is dying anyway
  delete m_server;
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // true is to force the frame to close
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("wxSocket demo: Server\n(c) 1999 Guillermo Rodriguez Garcia\n"),
               _("About Server"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnUDPTest(wxCommandEvent& WXUNUSED(event))
{
    TestLogger logtest("UDP test");

    IPaddress addr;
    addr.Service(3000);
    wxDatagramSocket sock(addr);

    char buf[1024];
    size_t n = sock.RecvFrom(addr, buf, sizeof(buf)).LastCount();
    if ( !n )
    {
        wxLogMessage("ERROR: failed to receive data");
        return;
    }

    wxLogMessage("Received \"%s\" from %s:%u.",
                 wxString::From8BitData(buf, n),
                 addr.IPAddress(), addr.Service());

    for ( size_t i = 0; i < n; i++ )
    {
        char& c = buf[i];
        if ( (c >= 'A' && c <= 'M') || (c >= 'a' && c <= 'm') )
            c += 13;
        else if ( (c >= 'N' && c <= 'Z') || (c >= 'n' && c <= 'z') )
            c -= 13;
    }

    if ( sock.SendTo(addr, buf, n).LastCount() != n )
    {
        wxLogMessage("ERROR: failed to send data");
        return;
    }
}

void MyFrame::OnWaitForAccept(wxCommandEvent& WXUNUSED(event))
{
    TestLogger logtest("WaitForAccept() test");

    wxBusyInfo info("Waiting for connection for 10 seconds...", this);
    if ( m_server->WaitForAccept(10) )
        wxLogMessage("Accepted client connection.");
    else
        wxLogMessage("Connection error or timeout expired.");
}

void MyFrame::Test1(wxSocketBase *sock)
{
  TestLogger logtest("Test 1");

  // Receive data from socket and send it back. We will first
  // get a byte with the buffer size, so we can specify the
  // exact size and use the wxSOCKET_WAITALL flag. Also, we
  // disabled input events so we won't have unwanted reentrance.
  // This way we can avoid the infamous wxSOCKET_BLOCK flag.

  sock->SetFlags(wxSOCKET_WAITALL);

  // Read the size
  unsigned char len;
  sock->Read(&len, 1);
  wxCharBuffer buf(len);

  // Read the data
  sock->Read(buf.data(), len);
  wxLogMessage("Got the data, sending it back");

  // Write it back
  sock->Write(buf, len);
}

void MyFrame::Test2(wxSocketBase *sock)
{
  char buf[4096];

  TestLogger logtest("Test 2");

  // We don't need to set flags because ReadMsg and WriteMsg
  // are not affected by them anyway.

  // Read the message
  wxUint32 len = sock->ReadMsg(buf, sizeof(buf)).LastCount();
  if ( !len )
  {
      wxLogError("Failed to read message.");
      return;
  }

  wxLogMessage("Got \"%s\" from client.", wxString::FromUTF8(buf, len));
  wxLogMessage("Sending the data back");

  // Write it back
  sock->WriteMsg(buf, len);
}

void MyFrame::Test3(wxSocketBase *sock)
{
  TestLogger logtest("Test 3");

  // This test is similar to the first one, but the len is
  // expressed in kbytes - this tests large data transfers.

  sock->SetFlags(wxSOCKET_WAITALL);

  // Read the size
  unsigned char len;
  sock->Read(&len, 1);
  wxCharBuffer buf(len*1024);

  // Read the data
  sock->Read(buf.data(), len * 1024);
  wxLogMessage("Got the data, sending it back");

  // Write it back
  sock->Write(buf, len * 1024);
}

void MyFrame::OnServerEvent(wxSocketEvent& event)
{
  wxString s = _("OnServerEvent: ");
  wxSocketBase *sock;

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  m_text->AppendText(s);

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  sock = m_server->Accept(false);

  if (sock)
  {
    IPaddress addr;
    if ( !sock->GetPeer(addr) )
    {
      wxLogMessage("New connection from unknown client accepted.");
    }
    else
    {
      wxLogMessage("New client connection from %s:%u accepted",
                   addr.IPAddress(), addr.Service());
    }
  }
  else
  {
    wxLogMessage("Error: couldn't accept a new connection");
    return;
  }

  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);

  m_numClients++;
  UpdateStatusBar();
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxString s = _("OnSocketEvent: ");
  wxSocketBase *sock = event.GetSocket();

  // First, print a message
  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
    case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
    default             : s.Append(_("Unexpected event !\n")); break;
  }

  m_text->AppendText(s);

  // Now we process the event
  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT:
    {
      // We disable input events, so that the test doesn't trigger
      // wxSocketEvent again.
      sock->SetNotify(wxSOCKET_LOST_FLAG);

      // Which test are we going to run?
      unsigned char c;
      sock->Read(&c, 1);

      switch (c)
      {
        case 0xBE: Test1(sock); break;
        case 0xCE: Test2(sock); break;
        case 0xDE: Test3(sock); break;
        default:
          wxLogMessage("Unknown test id received from client");
      }

      // Enable input events again.
      sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
      break;
    }
    case wxSOCKET_LOST:
    {
      m_numClients--;

      // Destroy() should be used instead of delete wherever possible,
      // due to the fact that wxSocket uses 'delayed events' (see the
      // documentation for wxPostEvent) and we don't want an event to
      // arrive to the event handler (the frame, here) after the socket
      // has been deleted. Also, we might be doing some other thing with
      // the socket at the same time; for example, we might be in the
      // middle of a test or something. Destroy() takes care of all
      // this for us.

      wxLogMessage("Deleting socket.");
      sock->Destroy();
      break;
    }
    default: ;
  }

  UpdateStatusBar();
}

// convenience functions

void MyFrame::UpdateStatusBar()
{
#if wxUSE_STATUSBAR
  wxString s;
  s.Printf(_("%d clients connected"), m_numClients);
  SetStatusText(s, 1);
#endif // wxUSE_STATUSBAR
}
