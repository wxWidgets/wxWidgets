/////////////////////////////////////////////////////////////////////////////
// Name:        server.cpp
// Purpose:     DDE sample: server
// Author:      Julian Smart
// Modified by:
// Created:     25/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// Settings common to both executables: determines whether
// we're using TCP/IP or real DDE.

#include "ddesetup.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

#include "server.h"

MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

char ipc_buffer[4000];
MyConnection *the_connection = NULL;
MyServer *my_server ;

bool MyApp::OnInit()
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Server", wxDefaultPosition, wxSize(400, 500));

  frame->CreateStatusBar();

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(SERVER_QUIT, "&Exit");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, 0, 0, 400, 250);
  wxListBox *list = new wxListBox(frame->panel, SERVER_LISTBOX,
                                  wxPoint(5, 5), wxSize(150, 120));
  list->Append("Apple");
  list->Append("Pear");
  list->Append("Orange");
  list->Append("Banana");
  list->Append("Fruit");

  frame->panel->Fit();
  frame->Fit();

  wxString server_name = "4242";
  if (argc > 1)
    server_name = argv[1];

  // Create a new server
  my_server = new MyServer;
  my_server->Create(server_name);
  frame->Show(TRUE);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(SERVER_QUIT, MyFrame::OnExit)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_LISTBOX(SERVER_LISTBOX, MyFrame::OnListBoxClick)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, -1, title, pos, size)
{
  panel = NULL;
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    if (my_server)
        delete my_server;
    this->Destroy();
}

// Set the client process's listbox to this item
void MyFrame::OnListBoxClick(wxCommandEvent& event)
{
    wxListBox* listBox = (wxListBox*) panel->FindWindow(SERVER_LISTBOX);
    if (listBox)
    {
        wxString value = listBox->GetStringSelection();
        if (the_connection)
        {
            the_connection->Advise("Item", (char*) (const char*) value);
        }
    }
}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (my_server)
        delete my_server;
    this->Destroy();
}

BEGIN_EVENT_TABLE(IPCDialogBox, wxDialog)
    EVT_BUTTON(SERVER_QUIT_BUTTON, IPCDialogBox::OnQuit)
END_EVENT_TABLE()

IPCDialogBox::IPCDialogBox(wxFrame *parent, const wxString& title,
                         const wxPoint& pos, const wxSize& size, MyConnection *the_connection):
                     wxDialog(parent, -1, title, pos, size)
{
  connection = the_connection;
  (void)new wxButton(this, SERVER_QUIT_BUTTON, "Quit this connection", wxPoint(5, 5));
  Fit();
}

void IPCDialogBox::OnQuit(wxCommandEvent& event)
{
  connection->Disconnect();
  delete connection;
}

wxConnectionBase *MyServer::OnAcceptConnection(const wxString& topic)
{
  if (strcmp(topic, "STDIO") != 0 && strcmp(topic, "IPC TEST") == 0)
    return new MyConnection(ipc_buffer, 4000);
  else
    return NULL;
}

MyConnection::MyConnection(char *buf, int size):wxConnection(buf, size)
{
  dialog = new IPCDialogBox(frame, "Connection", wxPoint(100, 100), wxSize(500, 500), this);
  dialog->Show(TRUE);
  the_connection = this;
}

MyConnection::~MyConnection(void)
{
  if (the_connection)
  {
    dialog->Destroy();
    the_connection = NULL;
  }
}

bool MyConnection::OnExecute(const wxString& topic, char *data, int size, wxIPCFormat format)
{
  char buf[300];
  sprintf(buf, "Execute command: %s", data);
  frame->SetStatusText(buf);
  return TRUE;
}

bool MyConnection::OnPoke(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format)
{
  char buf[300];
  sprintf(buf, "Poke command: %s", data);
  frame->SetStatusText(buf);
  return TRUE;
}

char *MyConnection::OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format)
{
  return "Here, have your data, client!";
}

bool MyConnection::OnStartAdvise(const wxString& topic, const wxString& item)
{
  return TRUE;
}

