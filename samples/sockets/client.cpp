/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     Client for wxSocket demo
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
#  pragma implementation "client.cpp"
#  pragma interface "client.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers 
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/socket.h"
#include "wx/url.h"
#include "wx/protocol/http.h"
#include "wx/progdlg.h"

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
  void OnOpenConnection(wxCommandEvent& event);
  void OnTest1(wxCommandEvent& event);
  void OnTest2(wxCommandEvent& event);
  void OnTest3(wxCommandEvent& event);
  void OnCloseConnection(wxCommandEvent& event);
  void OnSocketEvent(wxSocketEvent& event);
  void OnDatagram(wxCommandEvent& event);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketClient *m_sock;
  wxPanel        *m_panel;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenu         *m_menuSocket;
  wxMenu         *m_menuDatagramSocket;
  wxMenuBar      *m_menuBar;
  bool            m_busy;

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
  CLIENT_QUIT = 1000,
  CLIENT_ABOUT,
  CLIENT_OPEN,
  CLIENT_TEST1,
  CLIENT_TEST2,
  CLIENT_TEST3,
  CLIENT_CLOSE,
  CLIENT_DGRAM,

  // id for socket
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(CLIENT_QUIT,  MyFrame::OnQuit)
  EVT_MENU(CLIENT_ABOUT, MyFrame::OnAbout)
  EVT_MENU(CLIENT_OPEN,  MyFrame::OnOpenConnection)
  EVT_MENU(CLIENT_TEST1, MyFrame::OnTest1)
  EVT_MENU(CLIENT_TEST2, MyFrame::OnTest2)
  EVT_MENU(CLIENT_TEST3, MyFrame::OnTest3)
  EVT_MENU(CLIENT_CLOSE, MyFrame::OnCloseConnection)
  EVT_MENU(CLIENT_DGRAM, MyFrame::OnDatagram)
  EVT_SOCKET(SOCKET_ID,  MyFrame::OnSocketEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

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
                             _T("wxSocket demo: Client"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(mondrian));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(CLIENT_ABOUT, _T("&About...\tCtrl-A"), _T("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(CLIENT_QUIT, _T("E&xit\tAlt-X"), _T("Quit client"));

  m_menuSocket = new wxMenu();
  m_menuSocket->Append(CLIENT_OPEN, _T("&Open session"), _T("Connect to server"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_TEST1, _T("Test &1"), _T("Test basic functionality"));
  m_menuSocket->Append(CLIENT_TEST2, _T("Test &2"), _T("Test ReadMsg and WriteMsg"));
  m_menuSocket->Append(CLIENT_TEST3, _T("Test &3"), _T("Test large data transfer"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_CLOSE, _T("&Close session"), _T("Close connection"));

  m_menuDatagramSocket = new wxMenu();
  m_menuDatagramSocket->Append(CLIENT_DGRAM, _T("Send Datagram"), _("Test UDP sockets"));

  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _T("&File"));
  m_menuBar->Append(m_menuSocket, _T("&Socket"));
  m_menuBar->Append(m_menuDatagramSocket, _T("&DatagramSocket"));
  SetMenuBar(m_menuBar);

  // Status bar
  CreateStatusBar(2);

  // Make a panel with a textctrl in it
  m_panel = new wxPanel(this, -1, wxPoint(0, 0), GetClientSize());
  m_text  = new wxTextCtrl(m_panel, -1,
                           _T("Welcome to wxSocket demo: Client\n")
                           _T("Client ready\n\n"),
                           wxPoint(0, 0), m_panel->GetClientSize(),
                           wxTE_MULTILINE | wxTE_READONLY);

  // Create the socket
  m_sock = new wxSocketClient();
  m_sock->SetEventHandler(*this, SOCKET_ID);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(TRUE);

  m_busy = FALSE;
  UpdateStatusBar();
}

MyFrame::~MyFrame()
{
  delete m_sock;
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_T("wxSocket demo: Client\n")
               _T("(c) 1999 Guillermo Rodriguez Garcia\n"),
               _T("About Client"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
  wxIPV4address addr;

  m_menuSocket->Enable(CLIENT_OPEN, FALSE);
  m_menuSocket->Enable(CLIENT_CLOSE, FALSE);

  // Ask server address
  wxString hostname = wxGetTextFromUser(
    _T("Enter the address of the wxSocket demo server:"),
    _T("Connect ..."),
    _T("localhost"));

  addr.Hostname(hostname);
  addr.Service(3000);

  // Non-blocking connect
  m_text->AppendText(_T("Trying to connect (timeout = 10 sec) ...\n"));
  m_sock->Connect(addr, TRUE);
//  m_sock->WaitOnConnect(10);

  if (m_sock->IsConnected())
    m_text->AppendText(_T("Succeeded ! Connection established\n"));
  else
  {
    m_sock->Close();
    m_text->AppendText(_T("Failed ! Unable to connect\n"));
    wxMessageBox(_T("Can't connect to the specified host"), _T("Alert !"));
  }
  
  UpdateStatusBar();
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
  char *buf1;
  char *buf2;
  unsigned char len;

  // Disable socket menu entries (exception: Close Session)
  m_busy = TRUE;
  UpdateStatusBar();

  m_text->AppendText(_T("\n=== Test 1 begins ===\n"));

  // Tell the server which test we are running
  char c = 0xBE;
  m_sock->Write(&c, 1);

  // Send some data and read it back. We know the size of the
  // buffer, so we can specify the exact number of bytes to be
  // sent or received and use the wxSOCKET_WAITALL flag. Also,
  // we have disabled menu entries which could interfere with
  // the test, so we can safely avoid the wxSOCKET_BLOCK flag.
  //
  // First we send a byte with the length of the string, then
  // we send the string itself (do NOT try to send any integral
  // value larger than a byte "as is" across the network, or
  // you might be in trouble! Ever heard about big and little
  // endian computers?)
  //
  m_sock->SetFlags(wxSOCKET_WAITALL);

  buf1 = _T("Test string (less than 256 chars!)");
  len  = wxStrlen(buf1) + 1;
  buf2 = new char[len];

  m_text->AppendText(_T("Sending a test buffer to the server ..."));
  m_sock->Write((char *)&len, 1);
  m_sock->Write(buf1, len);
  m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));

  m_text->AppendText(_T("Receiving the buffer back from server ..."));
  m_sock->Read(buf2, len);
  m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));

  m_text->AppendText(_T("Comparing the two buffers ..."));
  if (memcmp(buf1, buf2, len) != 0)
  {
    m_text->AppendText(_T("failed!\n"));
    m_text->AppendText(_T("Test 1 failed !\n"));
  }
  else
  {
    m_text->AppendText(_T("done\n"));
    m_text->AppendText(_T("Test 1 passed !\n"));
  }
  m_text->AppendText(_T("=== Test 1 ends ===\n"));

  delete[] buf2;
  m_busy = FALSE;
  UpdateStatusBar();
}

void MyFrame::OnTest2(wxCommandEvent& WXUNUSED(event))
{
  char *msg1;
  char *msg2;
  size_t len;

  // Disable socket menu entries (exception: Close Session)
  m_busy = TRUE;
  UpdateStatusBar();

  m_text->AppendText(_T("\n=== Test 2 begins ===\n"));

  // Tell the server which test we are running
  char c = 0xCE;
  m_sock->Write(&c, 1);

  // Here we use ReadMsg and WriteMsg to send messages with
  // a header with size information. Also, the reception is
  // event triggered, so we test input events as well.
  //
  // We need to set no flags here (ReadMsg and WriteMsg are
  // not affected by flags)
  //
  m_sock->SetFlags(wxSOCKET_WAITALL);

  wxString s = wxGetTextFromUser(
    _T("Enter an arbitrary string to send to the server:"),
    _T("Test 2 ..."),
    _T("Yes I like wxWindows!"));

  msg1 = (char *)s.c_str();
  len  = wxStrlen(msg1) + 1;
  msg2 = new char[len];

  m_text->AppendText(_T("Sending the string with WriteMsg ..."));
  m_sock->WriteMsg(msg1, len);
  m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));
  m_text->AppendText(_T("Waiting for an event (timeout = 2 sec)\n"));

  // Wait until data available (will also return if the connection is lost)
  m_sock->WaitForRead(2);

  if (m_sock->IsData())
  {
    m_text->AppendText(_T("Reading the string back with ReadMsg ..."));
    m_sock->ReadMsg(msg2, len);
    m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));
    m_text->AppendText(_T("Comparing the two buffers ..."));
    if (memcmp(msg1, msg2, len) != 0)
    {
      m_text->AppendText(_T("failed!\n"));  
      m_text->AppendText(_T("Test 2 failed !\n"));
    }
    else
    {
      m_text->AppendText(_T("done\n"));
      m_text->AppendText(_T("Test 2 passed !\n"));
    }
  }
  else
    m_text->AppendText(_T("Timeout ! Test 2 failed.\n"));

  m_text->AppendText(_T("=== Test 2 ends ===\n"));

  delete[] msg2;
  m_busy = FALSE;
  UpdateStatusBar();
}

void MyFrame::OnTest3(wxCommandEvent& WXUNUSED(event))
{
  char *buf1;
  char *buf2;
  unsigned char len;

  // Disable socket menu entries (exception: Close Session)
  m_busy = TRUE;
  UpdateStatusBar();

  m_text->AppendText(_T("\n=== Test 3 begins ===\n"));

  // Tell the server which test we are running
  char c = 0xDE;
  m_sock->Write(&c, 1);

  // This test also is similar to the first one but it sends a
  // large buffer so that wxSocket is actually forced to split
  // it into pieces and take care of sending everything before
  // returning.
  //
  m_sock->SetFlags(wxSOCKET_WAITALL);

  // Note that len is in kbytes here!
  len  = 32;
  buf1 = new char[len * 1024];
  buf2 = new char[len * 1024];

  for (int i = 0; i < len * 1024; i ++)
    buf1[i] = (char)(i % 256);

  m_text->AppendText(_T("Sending a large buffer (32K) to the server ..."));
  m_sock->Write((char *)&len, 1);
  m_sock->Write(buf1, len * 1024);
  m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));

  m_text->AppendText(_T("Receiving the buffer back from server ..."));
  m_sock->Read(buf2, len * 1024);
  m_text->AppendText(m_sock->Error() ? _T("failed !\n") : _T("done\n"));

  m_text->AppendText(_T("Comparing the two buffers ..."));
  if (memcmp(buf1, buf2, len) != 0)
  {
    m_text->AppendText(_T("failed!\n"));
    m_text->AppendText(_T("Test 3 failed !\n"));
  }
  else
  {
    m_text->AppendText(_T("done\n"));
    m_text->AppendText(_T("Test 3 passed !\n"));
  }
  m_text->AppendText(_T("=== Test 3 ends ===\n"));

  delete[] buf2;
  m_busy = FALSE;
  UpdateStatusBar();
}

void MyFrame::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
  m_sock->Close();
  UpdateStatusBar();
}

void MyFrame::OnDatagram(wxCommandEvent& WXUNUSED(event))
{
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxString s = _T("OnSocketEvent: ");

  switch(event.SocketEvent())
  {
    case wxSOCKET_INPUT      : s.Append(_T("wxSOCKET_INPUT\n")); break;
    case wxSOCKET_LOST       : s.Append(_T("wxSOCKET_LOST\n")); break;
    case wxSOCKET_CONNECTION : s.Append(_T("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_T("Unexpected event !\n")); break;
  }

  m_text->AppendText(s);
  UpdateStatusBar();
}

// convenience functions

void MyFrame::UpdateStatusBar()
{
  wxString s;

  if (!m_sock->IsConnected())
  {
    s.Printf(_T("Not connected"));
  }
  else
  {
    wxIPV4address addr;

    m_sock->GetPeer(addr);
    s.Printf(_T("%s : %d"), (addr.Hostname()).c_str(), addr.Service());
  }

  SetStatusText(s, 1);

  m_menuSocket->Enable(CLIENT_OPEN, !m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST1, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST2, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST3, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_CLOSE, m_sock->IsConnected());
}
