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
#include "wx/wfstream.h"

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

  // event handlers for File menu
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  // event handlers for Socket menu
  void OnOpenConnection(wxCommandEvent& event);
  void OnTest1(wxCommandEvent& event);
  void OnTest2(wxCommandEvent& event);
  void OnTest3(wxCommandEvent& event);
  void OnCloseConnection(wxCommandEvent& event);

  // event handlers for Protocols menu
  void OnTestURL(wxCommandEvent& event);

  // event handlers for DatagramSocket menu (stub)
  void OnDatagram(wxCommandEvent& event);

  // socket event handler
  void OnSocketEvent(wxSocketEvent& event);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketClient *m_sock;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenu         *m_menuSocket;
  wxMenu         *m_menuDatagramSocket;
  wxMenu         *m_menuProtocols;
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
  CLIENT_TESTURL,
  CLIENT_DGRAM,

  // id for socket
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(CLIENT_QUIT,     MyFrame::OnQuit)
  EVT_MENU(CLIENT_ABOUT,    MyFrame::OnAbout)
  EVT_MENU(CLIENT_OPEN,     MyFrame::OnOpenConnection)
  EVT_MENU(CLIENT_TEST1,    MyFrame::OnTest1)
  EVT_MENU(CLIENT_TEST2,    MyFrame::OnTest2)
  EVT_MENU(CLIENT_TEST3,    MyFrame::OnTest3)
  EVT_MENU(CLIENT_CLOSE,    MyFrame::OnCloseConnection)
  EVT_MENU(CLIENT_DGRAM,    MyFrame::OnDatagram)
  EVT_MENU(CLIENT_TESTURL,  MyFrame::OnTestURL)
  EVT_SOCKET(SOCKET_ID,     MyFrame::OnSocketEvent)
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
                             _("wxSocket demo: Client"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(mondrian));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(CLIENT_ABOUT, _("&About...\tCtrl-A"), _("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(CLIENT_QUIT, _("E&xit\tAlt-X"), _("Quit client"));

  m_menuSocket = new wxMenu();
  m_menuSocket->Append(CLIENT_OPEN, _("&Open session"), _("Connect to server"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_TEST1, _("Test &1"), _("Test basic functionality"));
  m_menuSocket->Append(CLIENT_TEST2, _("Test &2"), _("Test ReadMsg and WriteMsg"));
  m_menuSocket->Append(CLIENT_TEST3, _("Test &3"), _("Test large data transfer"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_CLOSE, _("&Close session"), _("Close connection"));

  m_menuDatagramSocket = new wxMenu();
  m_menuDatagramSocket->Append(CLIENT_DGRAM, _("Send Datagram"), _("Test UDP sockets"));

  m_menuProtocols = new wxMenu();
  m_menuProtocols->Append(CLIENT_TESTURL, _("Test URL"), _("Get data from the specified URL"));

  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _("&File"));
  m_menuBar->Append(m_menuSocket, _("&SocketClient"));
  m_menuBar->Append(m_menuDatagramSocket, _("&DatagramSocket"));
  m_menuBar->Append(m_menuProtocols, _("&Protocols"));
  SetMenuBar(m_menuBar);

  // Status bar
  CreateStatusBar(2);

  // Make a textctrl for logging
  m_text  = new wxTextCtrl(this, -1,
                           _("Welcome to wxSocket demo: Client\n"
                             "Client ready\n"),
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_READONLY);

  // Create the socket
  m_sock = new wxSocketClient();

  // Setup the event handler and subscribe to most events
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
  // No delayed deletion here, as the frame is dying anyway
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
  wxMessageBox(_("wxSocket demo: Client\n"
                 "(c) 1999 Guillermo Rodriguez Garcia\n"),
               _("About Client"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
  wxIPV4address addr;

  m_menuSocket->Enable(CLIENT_OPEN, FALSE);
  m_menuSocket->Enable(CLIENT_CLOSE, FALSE);

  // Ask user for server address
  wxString hostname = wxGetTextFromUser(
    _("Enter the address of the wxSocket demo server:"),
    _("Connect ..."),
    _("localhost"));

  addr.Hostname(hostname);
  addr.Service(3000);

  // Mini-tutorial for Connect() :-)
  // ---------------------------
  //
  // There are two ways to use Connect(): blocking and non-blocking,
  // depending on the value passed as the 'wait' (2nd) parameter.
  //
  // Connect(addr, TRUE) will wait until the connection completes,
  // returning TRUE on success and FALSE on failure. This call blocks
  // the GUI (this might be changed in future releases to honour the
  // wxSOCKET_BLOCK flag).
  //
  // Connect(addr, FALSE) will issue a nonblocking connection request
  // and return immediately. If the return value is TRUE, then the
  // connection has been already succesfully established. If it is
  // FALSE, you must wait for the request to complete, either with
  // WaitOnConnect() or by watching wxSOCKET_CONNECTION / LOST
  // events (please read the documentation).
  //
  // WaitOnConnect() itself never blocks the GUI (this might change
  // in the future to honour the wxSOCKET_BLOCK flag). This call will
  // return FALSE on timeout, or TRUE if the connection request
  // completes, which in turn might mean:
  //
  //   a) That the connection was successfully established
  //   b) That the connection request failed (for example, because
  //      it was refused by the peer.
  //
  // Use IsConnected() to distinguish between these two.
  //
  // So, in a brief, you should do one of the following things:
  //
  // For blocking Connect:
  //
  //   bool success = client->Connect(addr, TRUE);
  //
  // For nonblocking Connect:
  //
  //   client->Connect(addr, FALSE);
  //
  //   bool waitmore = TRUE;
  //   while (! client->WaitOnConnect(seconds, millis) && waitmore )
  //   {
  //     // possibly give some feedback to the user,
  //     // update waitmore if needed.
  //   }
  //   bool success = client->IsConnected();
  // 
  // And that's all :-)

  m_text->AppendText(_("\nTrying to connect (timeout = 10 sec) ...\n"));
  m_sock->Connect(addr, FALSE);
  m_sock->WaitOnConnect(10);

  if (m_sock->IsConnected())
    m_text->AppendText(_("Succeeded ! Connection established\n"));
  else
  {
    m_sock->Close();
    m_text->AppendText(_("Failed ! Unable to connect\n"));
    wxMessageBox(_("Can't connect to the specified host"), _("Alert !"));
  }
  
  UpdateStatusBar();
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
  const char *buf1;
  char       *buf2;
  unsigned char len;

  // Disable socket menu entries (exception: Close Session)
  m_busy = TRUE;
  UpdateStatusBar();

  m_text->AppendText(_("\n=== Test 1 begins ===\n"));

  // Tell the server which test we are running
  unsigned char c = 0xBE;
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

  m_sock->SetFlags(wxSOCKET_WAITALL);

  buf1 = _("Test string (less than 256 chars!)");
  len  = wxStrlen(buf1) + 1;
  buf2 = new char[len];

  m_text->AppendText(_("Sending a test buffer to the server ..."));
  m_sock->Write(&len, 1);
  m_sock->Write(buf1, len);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Receiving the buffer back from server ..."));
  m_sock->Read(buf2, len);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Comparing the two buffers ..."));
  if (memcmp(buf1, buf2, len) != 0)
  {
    m_text->AppendText(_("failed!\n"));
    m_text->AppendText(_("Test 1 failed !\n"));
  }
  else
  {
    m_text->AppendText(_("done\n"));
    m_text->AppendText(_("Test 1 passed !\n"));
  }
  m_text->AppendText(_("=== Test 1 ends ===\n"));

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

  m_text->AppendText(_("\n=== Test 2 begins ===\n"));

  // Tell the server which test we are running
  unsigned char c = 0xCE;
  m_sock->Write(&c, 1);

  // Here we use ReadMsg and WriteMsg to send messages with
  // a header with size information. Also, the reception is
  // event triggered, so we test input events as well.
  //
  // We need to set no flags here (ReadMsg and WriteMsg are
  // not affected by flags)

  m_sock->SetFlags(wxSOCKET_WAITALL);

  wxString s = wxGetTextFromUser(
    _("Enter an arbitrary string to send to the server:"),
    _("Test 2 ..."),
    _("Yes I like wxWindows!"));

  msg1 = (char *)s.c_str();
  len  = wxStrlen(msg1) + 1;
  msg2 = new char[len];

  m_text->AppendText(_("Sending the string with WriteMsg ..."));
  m_sock->WriteMsg(msg1, len);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));
  m_text->AppendText(_("Waiting for an event (timeout = 2 sec)\n"));

  // Wait until data available (will also return if the connection is lost)
  m_sock->WaitForRead(2);

  if (m_sock->IsData())
  {
    m_text->AppendText(_("Reading the string back with ReadMsg ..."));
    m_sock->ReadMsg(msg2, len);
    m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));
    m_text->AppendText(_("Comparing the two buffers ..."));
    if (memcmp(msg1, msg2, len) != 0)
    {
      m_text->AppendText(_("failed!\n"));  
      m_text->AppendText(_("Test 2 failed !\n"));
    }
    else
    {
      m_text->AppendText(_("done\n"));
      m_text->AppendText(_("Test 2 passed !\n"));
    }
  }
  else
    m_text->AppendText(_("Timeout ! Test 2 failed.\n"));

  m_text->AppendText(_("=== Test 2 ends ===\n"));

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

  m_text->AppendText(_("\n=== Test 3 begins ===\n"));

  // Tell the server which test we are running
  unsigned char c = 0xDE;
  m_sock->Write(&c, 1);

  // This test also is similar to the first one but it sends a
  // large buffer so that wxSocket is actually forced to split
  // it into pieces and take care of sending everything before
  // returning.

  m_sock->SetFlags(wxSOCKET_WAITALL);

  // Note that len is in kbytes here!
  // Also note that Linux kernel 2.0.36 gives up at len > 27.
  len  = 32;
  buf1 = new char[len * 1024];
  buf2 = new char[len * 1024];

  for (int i = 0; i < len * 1024; i ++)
    buf1[i] = (char)(i % 256);

  m_text->AppendText(_("Sending a large buffer (32K) to the server ..."));
  m_sock->Write(&len, 1);
  m_sock->Write(buf1, len * 1024);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Receiving the buffer back from server ..."));
  m_sock->Read(buf2, len * 1024);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Comparing the two buffers ..."));
  if (memcmp(buf1, buf2, len) != 0)
  {
    m_text->AppendText(_("failed!\n"));
    m_text->AppendText(_("Test 3 failed !\n"));
  }
  else
  {
    m_text->AppendText(_("done\n"));
    m_text->AppendText(_("Test 3 passed !\n"));
  }
  m_text->AppendText(_("=== Test 3 ends ===\n"));

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
  m_text->AppendText(_("\n=== Datagram test begins ===\n"));
  m_text->AppendText(_("Sorry, not implemented\n"));
  m_text->AppendText(_("=== Datagram test ends ===\n"));
}

void MyFrame::OnTestURL(wxCommandEvent& WXUNUSED(event))
{
  // Note that we are creating a new socket here, so this
  // won't mess with the client/server demo.

  // Ask for the URL
  m_text->AppendText(_("\n=== URL test begins ===\n"));
  wxString urlname = wxGetTextFromUser(_("Enter an URL to get"),
                                       _("URL:"),
                                       _("http://localhost"));

  // Parse the URL
  wxURL url(urlname);
  if (url.GetError() != wxURL_NOERR)
  {
    m_text->AppendText(_("Error: couldn't parse URL\n"));
    m_text->AppendText(_("=== URL test ends ===\n"));
    return;
  }

  // Try to get the input stream (connects to the given URL)
  m_text->AppendText(_("Trying to establish connection...\n"));
  wxYield();
  wxInputStream *data = url.GetInputStream();
  if (!data)
  {
    m_text->AppendText(_("Error: couldn't read from URL\n"));
    m_text->AppendText(_("=== URL test ends ===\n"));
    return;
  }

  // Print the contents type and file size
  wxString s;
  s.Printf(_("Contents type: %s\n"
             "File size: %i\n"
             "Starting to download...\n"),
             url.GetProtocol().GetContentType().c_str(),
             data->GetSize());
  m_text->AppendText(s);
  wxYield();

  // Get the data
  wxFileOutputStream sout(wxString("test.url"));
  if (!sout.Ok())
  {
    m_text->AppendText(_("Error: couldn't open file for output\n"));
    m_text->AppendText(_("=== URL test ends ===\n"));
    return;
  }

  data->Read(sout);
  m_text->AppendText(_("Results written to file: test.url\n"));
  m_text->AppendText(_("Done.\n"));
  m_text->AppendText(_("=== URL test ends ===\n"));

  delete data;
}

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxString s = _("OnSocketEvent: ");

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT      : s.Append(_("wxSOCKET_INPUT\n")); break;
    case wxSOCKET_LOST       : s.Append(_("wxSOCKET_LOST\n")); break;
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
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
    s.Printf(_("Not connected"));
  }
  else
  {
    wxIPV4address addr;

    m_sock->GetPeer(addr);
    s.Printf(_("%s : %d"), (addr.Hostname()).c_str(), addr.Service());
  }

  SetStatusText(s, 1);

  m_menuSocket->Enable(CLIENT_OPEN, !m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST1, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST2, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST3, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_CLOSE, m_sock->IsConnected());
}
