/*
 * File:	client.cpp
 * Purpose:	wxSocket: client demo
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
#include "wx/url.h"
#include "wx/protocol/http.h"

// Define a new application type
class MyApp: public wxApp
{ public:
    virtual bool OnInit(void);
};

class MyClient;

// Define a new frame type
class MyFrame: public wxFrame
{ 
  DECLARE_CLASS(MyFrame)
public:
  MyClient *sock;

  MyFrame(void);
  virtual ~MyFrame();
  void OnCloseTest(wxCommandEvent& evt);
  void OnExecTest1(wxCommandEvent& evt);
  void OnExecUrlTest(wxCommandEvent& evt);
  void OnQuitApp(wxCommandEvent& evt);
  void OnExecOpenConnection(wxCommandEvent& evt);
  void OnExecCloseConnection(wxCommandEvent& evt);
  void UpdateStatus();

  DECLARE_EVENT_TABLE()
};


IMPLEMENT_CLASS(MyFrame, wxFrame)

/*
 * Define a new derived SocketClient
 */
class MyClient: public wxSocketClient
{
public:
  MyFrame *frame;

  void OnNotify(wxRequestNotify WXUNUSED(flags)) { frame->UpdateStatus(); }
};

// ID for the menu quit command
const int SKDEMO_QUIT    = 101;
const int SKDEMO_CONNECT = 102;
const int SKDEMO_TEST1   = 103;
const int SKDEMO_TEST2   = 104;
const int SKDEMO_CLOSE   = 105;
const int SKDEMO_TEST3   = 106;
const int ID_TEST_CLOSE  = 107;

IMPLEMENT_APP(MyApp)

/*
 * `Main program' equivalent, creating windows and returning main app frame
 */
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame();

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("mondrian"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("mondrian.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu();

  file_menu->Append(SKDEMO_QUIT, "Exit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");

  wxMenu *socket_menu = new wxMenu();
  socket_menu->Append(SKDEMO_CONNECT, "Open session");
  socket_menu->AppendSeparator();
  socket_menu->Append(SKDEMO_TEST1, "Start test 1");
  socket_menu->AppendSeparator();
  socket_menu->Append(SKDEMO_CLOSE, "Close session");
  socket_menu->AppendSeparator();
  socket_menu->Append(SKDEMO_TEST3, "Start URL test");

  menu_bar->Append(socket_menu, "Socket");

  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  (void)new wxPanel(frame, 0, 0, 300, 100);

  // Show the frame
  frame->Show(TRUE);
  
  // Return the main frame window
  return TRUE;
}

/*
 * MyFrame Constructor
 */
MyFrame::MyFrame():
  wxFrame(NULL, -1, "wxSocket client demo",
          wxDefaultPosition, wxSize(300, 200), wxDEFAULT_FRAME_STYLE)
{
  // Init all
  wxSocketHandler::Master();

  sock = new MyClient();
  sock->SetFlags(wxSocketBase::WAITALL);
  wxSocketHandler::Master().Register(sock);
  sock->frame = this;
  sock->SetNotify(wxSocketBase::REQ_LOST);
  CreateStatusBar(2);
  UpdateStatus();
}

MyFrame::~MyFrame()
{
  delete sock;
}

void MyFrame::OnQuitApp(wxCommandEvent& WXUNUSED(evt))
{
  Close(TRUE);
}

void MyFrame::OnExecOpenConnection(wxCommandEvent& WXUNUSED(evt))
{
  wxIPV4address addr;
    
  if (sock->IsConnected())
    sock->Close();

  wxString hname = wxGetTextFromUser("Enter the address of the wxSocket Sample Server", 
                                  "Connect ...", "localhost");
  addr.Hostname(hname);
  addr.Service(3000);
  sock->SetNotify(0);
  sock->Connect(addr, TRUE);
  if (!sock->IsConnected())
    wxMessageBox("Can't connect to the specified host", "Alert !");

  UpdateStatus();
}

void MyFrame::OnExecCloseConnection(wxCommandEvent& WXUNUSED(evt))
{
  if (sock)
    sock->Close();
  UpdateStatus();
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_BUTTON(ID_TEST_CLOSE, MyFrame::OnCloseTest)
  EVT_MENU(SKDEMO_TEST1, MyFrame::OnExecTest1)
  EVT_MENU(SKDEMO_TEST3, MyFrame::OnExecUrlTest)
  EVT_MENU(SKDEMO_QUIT, MyFrame::OnQuitApp)
  EVT_MENU(SKDEMO_CONNECT, MyFrame::OnExecOpenConnection)
  EVT_MENU(SKDEMO_CLOSE, MyFrame::OnExecCloseConnection)
END_EVENT_TABLE()

void MyFrame::OnCloseTest(wxCommandEvent& evt)
{
  wxButton *button = (wxButton *)evt.GetEventObject();
  wxDialog *dlg = (wxDialog *)button->GetParent();

  dlg->EndModal(0);
}

void MyFrame::UpdateStatus()
{
  if (!sock->IsConnected()) {
    SetStatusText("Not connected", 0);
    SetStatusText("", 1);
  } else {
    wxIPV4address addr;
    char s[100];

    sock->GetPeer(addr);
    sprintf(s, "Connected to %s", (const char *)addr.Hostname());
    SetStatusText(s, 0);
    sprintf(s, "Service: %d", addr.Service());
    SetStatusText(s, 1);
  } 
}

void MyFrame::OnExecTest1(wxCommandEvent& WXUNUSED(evt))
{
  if (!sock->IsConnected())
    return;

  wxDialog *dlgbox = new wxDialog(this, -1, "Test 1", wxDefaultPosition, wxSize(414, 250));
  wxTextCtrl *text_win = new wxTextCtrl(dlgbox, -1, "",
                                        wxPoint(0, 0), wxSize(400, 200),
					wxTE_MULTILINE);
  (void)new wxButton(dlgbox, ID_TEST_CLOSE, "Close",
                     wxPoint(100, 210), wxSize(100, -1));
  char *buf, *buf2;

  dlgbox->Layout();
  dlgbox->Show(TRUE);

  text_win->WriteText("Initializing test 1 ...\n");
  
  wxYield();
  
  /* Init */
  buf = copystring("Salut ! Salut ! Salut ! Salut Toto\n");
  buf2 = new char[strlen(buf)+1];
  char c = 0xbe;
  sock->WriteMsg(&c, 1);

  /* No 1 */
  text_win->WriteText("Sending some byte to the server ...");
  sock->Write(buf, strlen(buf)+1);
  text_win->WriteText("done\n");
  text_win->WriteText("Receiving some byte from the server ...");
  sock->Read(buf2, strlen(buf)+1);
  text_win->WriteText("done\n");
  
  text_win->WriteText("Comparing the two buffers ...");
  if (memcmp(buf, buf2, strlen(buf)+1) != 0) {
    text_win->WriteText("Fail\n");
    sock->Close();
    UpdateStatus();
  } else
    text_win->WriteText("done\nTest 1 passed !\n");

  dlgbox->Layout();
  dlgbox->ShowModal();

  delete [] buf;
  delete [] buf2;
  delete text_win;
  delete dlgbox;
}

void MyFrame::OnExecUrlTest(wxCommandEvent& WXUNUSED(evt))
{
  wxString urlname = wxGetTextFromUser("Enter an URL to get",
                                     "URL:", "http://localhost");

  wxURL url(urlname);
  wxInputStream *datas = url.GetInputStream();

  if (!datas)
    wxMessageBox("Error in getting data from the URL.", "Alert !");
  else {
    wxMessageBox("Success !! Click on OK to see the text.", "OK");
    delete datas;
  }
}
