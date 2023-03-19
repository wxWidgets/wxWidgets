/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     Client for wxSocket demo
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Modified by:
// Created:     1999/09/19
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
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

#include "wx/socket.h"
#include "wx/url.h"
#include "wx/sstream.h"
#include "wx/thread.h"

#include <memory>

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
  virtual bool OnInit() override;
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

#if wxUSE_URL
  // event handlers for Protocols menu
  void OnTestURL(wxCommandEvent& event);
#endif
#if wxUSE_IPV6
  void OnOpenConnectionIPv6(wxCommandEvent& event);
#endif

  void OpenConnection(wxSockAddress::Family family);

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

  // any class wishing to process wxWidgets events must use this macro
  wxDECLARE_EVENT_TABLE();
};

// simple helper class to log start and end of each test
class TestLogger
{
public:
    TestLogger(const wxString& name) : m_name(name)
    {
        wxLogMessage("=== %s test begins ===", m_name);
    }

    ~TestLogger()
    {
        wxLogMessage("=== %s test ends ===", m_name);
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
  CLIENT_QUIT = wxID_EXIT,
  CLIENT_ABOUT = wxID_ABOUT,
  CLIENT_OPEN = 100,
#if wxUSE_IPV6
  CLIENT_OPENIPV6,
#endif
  CLIENT_TEST1,
  CLIENT_TEST2,
  CLIENT_TEST3,
  CLIENT_CLOSE,
#if wxUSE_URL
  CLIENT_TESTURL,
#endif
  CLIENT_DGRAM,

  // id for socket
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(CLIENT_QUIT,     MyFrame::OnQuit)
  EVT_MENU(CLIENT_ABOUT,    MyFrame::OnAbout)
  EVT_MENU(CLIENT_OPEN,     MyFrame::OnOpenConnection)
#if wxUSE_IPV6
  EVT_MENU(CLIENT_OPENIPV6, MyFrame::OnOpenConnectionIPv6)
#endif
  EVT_MENU(CLIENT_TEST1,    MyFrame::OnTest1)
  EVT_MENU(CLIENT_TEST2,    MyFrame::OnTest2)
  EVT_MENU(CLIENT_TEST3,    MyFrame::OnTest3)
  EVT_MENU(CLIENT_CLOSE,    MyFrame::OnCloseConnection)
  EVT_MENU(CLIENT_DGRAM,    MyFrame::OnDatagram)
#if wxUSE_URL
  EVT_MENU(CLIENT_TESTURL,  MyFrame::OnTestURL)
#endif
  EVT_SOCKET(SOCKET_ID,     MyFrame::OnSocketEvent)
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

  // success
  return true;
}

// --------------------------------------------------------------------------
// main frame
// --------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY,
                             _("wxSocket demo: Client"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(sample));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(CLIENT_ABOUT, _("&About\tCtrl-A"), _("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(CLIENT_QUIT, _("E&xit\tAlt-X"), _("Quit client"));

  m_menuSocket = new wxMenu();
  m_menuSocket->Append(CLIENT_OPEN, _("&Open session\tCtrl-O"), _("Connect to server"));
#if wxUSE_IPV6
  m_menuSocket->Append(CLIENT_OPENIPV6, _("&Open session(IPv6)\tShift-Ctrl-O"), _("Connect to server(IPv6)"));
#endif
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_TEST1, _("Test &1\tCtrl-F1"), _("Test basic functionality"));
  m_menuSocket->Append(CLIENT_TEST2, _("Test &2\tCtrl-F2"), _("Test ReadMsg and WriteMsg"));
  m_menuSocket->Append(CLIENT_TEST3, _("Test &3\tCtrl-F3"), _("Test large data transfer"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_CLOSE, _("&Close session\tCtrl-C"), _("Close connection"));

  m_menuDatagramSocket = new wxMenu();
  m_menuDatagramSocket->Append(CLIENT_DGRAM, _("&Datagram test\tCtrl-D"), _("Test UDP sockets"));

#if wxUSE_URL
  m_menuProtocols = new wxMenu();
  m_menuProtocols->Append(CLIENT_TESTURL, _("Test URL\tCtrl-U"),
                          _("Get data from the specified URL"));
#endif

  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _("&File"));
  m_menuBar->Append(m_menuSocket, _("&TCP"));
  m_menuBar->Append(m_menuDatagramSocket, _("&UDP"));
#if wxUSE_URL
  m_menuBar->Append(m_menuProtocols, _("&Protocols"));
#endif
  SetMenuBar(m_menuBar);

#if wxUSE_STATUSBAR
  // Status bar
  CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

  // Make a textctrl for logging
  m_text  = new wxTextCtrl(this, wxID_ANY,
                           _("Welcome to wxSocket demo: Client\nClient ready\n"),
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_READONLY);
  delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));

  // Create the socket
  m_sock = new wxSocketClient();

  // Setup the event handler and subscribe to most events
  m_sock->SetEventHandler(*this, SOCKET_ID);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(true);

  m_busy = false;
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
  // true is to force the frame to close
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("wxSocket demo: Client\n(c) 1999 Guillermo Rodriguez Garcia\n"),
               _("About Client"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
    OpenConnection(wxSockAddress::IPV4);
}
#if wxUSE_IPV6
void MyFrame::OnOpenConnectionIPv6(wxCommandEvent& WXUNUSED(event))
{
    OpenConnection(wxSockAddress::IPV6);
}
#endif // wxUSE_IPV6

void MyFrame::OpenConnection(wxSockAddress::Family family)
{
    wxUnusedVar(family); // unused in !wxUSE_IPV6 case

  wxIPaddress * addr;
  wxIPV4address addr4;
#if wxUSE_IPV6
  wxIPV6address addr6;
  if ( family == wxSockAddress::IPV6 )
    addr = &addr6;
  else
#endif
    addr = &addr4;

  m_menuSocket->Enable(CLIENT_OPEN, false);
#if wxUSE_IPV6
  m_menuSocket->Enable(CLIENT_OPENIPV6, false);
#endif
  m_menuSocket->Enable(CLIENT_CLOSE, false);

  // Ask user for server address
  wxString hostname = wxGetTextFromUser(
    _("Enter the address of the wxSocket demo server:"),
    _("Connect ..."),
    "localhost");
  if ( hostname.empty() )
    return;

  addr->Hostname(hostname);
  addr->Service(3000);

  // we connect asynchronously and will get a wxSOCKET_CONNECTION event when
  // the connection is really established
  //
  // if you want to make sure that connection is established right here you
  // could call WaitOnConnect(timeout) instead
  wxLogMessage("Trying to connect to %s:%d", hostname, addr->Service());

  m_sock->Connect(*addr, false);
}

void MyFrame::OnTest1(wxCommandEvent& WXUNUSED(event))
{
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
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

  const char *buf1 = "Test string (less than 256 chars!)";
  unsigned char len  = (unsigned char)(wxStrlen(buf1) + 1);
  wxCharBuffer buf2(wxStrlen(buf1));

  m_text->AppendText(_("Sending a test buffer to the server ..."));
  m_sock->Write(&len, 1);
  m_sock->Write(buf1, len);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Receiving the buffer back from server ..."));
  m_sock->Read(buf2.data(), len);
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

  m_busy = false;
  UpdateStatusBar();
}

void MyFrame::OnTest2(wxCommandEvent& WXUNUSED(event))
{
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
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
    _("Yes I like wxWidgets!"));

  const wxScopedCharBuffer msg1(s.utf8_str());
  size_t len  = wxStrlen(msg1) + 1;
  wxCharBuffer msg2(wxStrlen(msg1));

  m_text->AppendText(_("Sending the string with WriteMsg ..."));
  m_sock->WriteMsg(msg1, len);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));
  m_text->AppendText(_("Waiting for an event (timeout = 2 sec)\n"));

  // Wait until data available (will also return if the connection is lost)
  m_sock->WaitForRead(2);

  if (m_sock->IsData())
  {
    m_text->AppendText(_("Reading the string back with ReadMsg ..."));
    m_sock->ReadMsg(msg2.data(), len);
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

  m_busy = false;
  UpdateStatusBar();
}

void MyFrame::OnTest3(wxCommandEvent& WXUNUSED(event))
{
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
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
  const unsigned char len  = 32;
  wxCharBuffer buf1(len * 1024),
               buf2(len * 1024);

  for (size_t i = 0; i < len * 1024; i ++)
    buf1.data()[i] = (char)(i % 256);

  m_text->AppendText(_("Sending a large buffer (32K) to the server ..."));
  m_sock->Write(&len, 1);
  m_sock->Write(buf1, len * 1024);
  m_text->AppendText(m_sock->Error() ? _("failed !\n") : _("done\n"));

  m_text->AppendText(_("Receiving the buffer back from server ..."));
  m_sock->Read(buf2.data(), len * 1024);
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

  m_busy = false;
  UpdateStatusBar();
}

void MyFrame::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
  m_sock->Close();
  UpdateStatusBar();
}

void MyFrame::OnDatagram(wxCommandEvent& WXUNUSED(event))
{
    wxString hostname = wxGetTextFromUser
                        (
                         "Enter the address of the wxSocket demo server:",
                         "UDP peer",
                         "localhost"
                        );
    if ( hostname.empty() )
        return;

    TestLogger logtest("UDP");

    wxIPV4address addrLocal;
    addrLocal.Hostname();
    wxDatagramSocket sock(addrLocal);
    if ( !sock.IsOk() )
    {
        wxLogMessage("ERROR: failed to create UDP socket");
        return;
    }

    wxLogMessage("Created UDP socket at %s:%u",
                 addrLocal.IPAddress(), addrLocal.Service());

    wxIPV4address addrPeer;
    addrPeer.Hostname(hostname);
    addrPeer.Service(3000);

    wxLogMessage("Testing UDP with peer at %s:%u",
                 addrPeer.IPAddress(), addrPeer.Service());

    char buf[] = "Uryyb sebz pyvrag!";
    if ( sock.SendTo(addrPeer, buf, sizeof(buf)).LastCount() != sizeof(buf) )
    {
        wxLogMessage("ERROR: failed to send data");
        return;
    }

    if ( sock.RecvFrom(addrPeer, buf, sizeof(buf)).LastCount() != sizeof(buf) )
    {
        wxLogMessage("ERROR: failed to receive data");
        return;
    }

    wxLogMessage("Received \"%s\" from %s:%u.",
                 wxString::From8BitData(buf, sock.LastCount()),
                 addrPeer.IPAddress(), addrPeer.Service());
}

#if wxUSE_URL

void DoDownload(const wxString& urlname)
{
    wxString testname("URL");
    if ( !wxIsMainThread() )
        testname += " in worker thread";

    TestLogger logtest(testname);

    // Parse the URL
    wxURL url(urlname);
    if ( url.GetError() != wxURL_NOERR )
    {
        wxLogError("Failed to parse URL \"%s\"", urlname);
        return;
    }

    // Try to get the input stream (connects to the given URL)
    wxLogMessage("Establishing connection to \"%s\"...", urlname);
    const std::unique_ptr<wxInputStream> data(url.GetInputStream());
    if ( !data.get() )
    {
        wxLogError("Failed to retrieve URL \"%s\"", urlname);
        return;
    }

    // Print the contents type and file size
    wxLogMessage("Contents type: %s\nFile size: %lu\nStarting to download...",
                 url.GetProtocol().GetContentType(),
                 static_cast<unsigned long>( data->GetSize() ));

    // Get the data
    wxStringOutputStream sout;
    if ( data->Read(sout).GetLastError() != wxSTREAM_EOF )
    {
        wxLogError("Error reading the input stream.");
    }

    wxLogMessage("Text retrieved from URL \"%s\" follows:\n%s",
                 urlname, sout.GetString());
}

void MyFrame::OnTestURL(wxCommandEvent& WXUNUSED(event))
{
    // Ask for the URL
    static wxString s_urlname("http://www.google.com/");
    wxString urlname = wxGetTextFromUser
                       (
                        _("Enter an URL to get"),
                        _("URL:"),
                        s_urlname
                       );
    if ( urlname.empty() )
        return; // cancelled by user

    s_urlname = urlname;

    // First do it in this thread.
    DoDownload(urlname);

    // And then also in a worker thread.
#if wxUSE_THREADS
    class DownloadThread : public wxThread
    {
    public:
        explicit DownloadThread(const wxString& url): m_url(url)
        {
            Run();
        }

        virtual void* Entry() override
        {
            DoDownload(m_url);

            return nullptr;
        }

    private:
        const wxString m_url;
    };

    // NB: there is a race condition here, we don't check for this thread
    // termination before exiting the application, don't do this in real code!
    new DownloadThread(urlname);
#endif // wxUSE_THREADS
}

#endif // wxUSE_URL

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
    switch ( event.GetSocketEvent() )
    {
        case wxSOCKET_INPUT:
            wxLogMessage("Input available on the socket");
            break;

        case wxSOCKET_LOST:
            wxLogMessage("Socket connection was unexpectedly lost.");
            UpdateStatusBar();
            break;

        case wxSOCKET_CONNECTION:
            wxLogMessage("... socket is now connected.");
            UpdateStatusBar();
            break;

        default:
            wxLogMessage("Unknown socket event!!!");
            break;
    }
}

// convenience functions

void MyFrame::UpdateStatusBar()
{
#if wxUSE_STATUSBAR
  wxString s;

  if (!m_sock->IsConnected())
  {
    s = "Not connected";
  }
  else
  {
#if wxUSE_IPV6
    wxIPV6address addr;
#else
    wxIPV4address addr;
#endif

    m_sock->GetPeer(addr);
    s.Printf("%s : %d", addr.Hostname(), addr.Service());
  }

  SetStatusText(s, 1);
#endif // wxUSE_STATUSBAR

  m_menuSocket->Enable(CLIENT_OPEN, !m_sock->IsConnected() && !m_busy);
#if wxUSE_IPV6
  m_menuSocket->Enable(CLIENT_OPENIPV6, !m_sock->IsConnected() && !m_busy);
#endif
  m_menuSocket->Enable(CLIENT_TEST1, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST2, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_TEST3, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_CLOSE, m_sock->IsConnected());
}
