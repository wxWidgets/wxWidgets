/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     DDE sample: client
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

#include "client.h"

MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

char ipc_buffer[4000];
wxListBox *the_list = NULL;

MyConnection *the_connection = NULL;
MyClient *my_client ;

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
  // Create the main frame window
  frame = new MyFrame(NULL, "Client", wxPoint(400, 0), wxSize(400, 300));

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(CLIENT_EXECUTE, "Execute");
  file_menu->Append(CLIENT_REQUEST, "Request");
  file_menu->Append(CLIENT_POKE, "Poke");
  file_menu->Append(CLIENT_QUIT, "Quit");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "File");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  // Make a panel
  frame->panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 250));
  the_list = new wxListBox(frame->panel, CLIENT_LISTBOX, wxPoint(5, 5), wxSize(150, 120));
  the_list->Append("Apple");
  the_list->Append("Pear");
  the_list->Append("Orange");
  the_list->Append("Banana");
  the_list->Append("Fruit");

  frame->panel->Fit();
  frame->Fit();

  wxString server = "4242";
  wxString hostName = wxGetHostName();

  if (argc > 1)
    server = argv[1];
  if (argc > 2)
    hostName = argv[2];

  // Create a new client
  my_client = new MyClient;
  the_connection = (MyConnection *)my_client->MakeConnection(hostName, server, "IPC TEST");

  if (!the_connection)
  {
    wxMessageBox("Failed to make connection to server", "Client Demo Error");
#ifdef __WXMSW__
//    extern void wxPrintDDEError();
//    wxPrintDDEError();
#endif
    return FALSE;
  }
  the_connection->StartAdvise("Item");

  frame->Show(TRUE);

  return TRUE;
}

int MyApp::OnExit()
{
    if (my_client)
      delete my_client ;

    return 0;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(CLIENT_QUIT, MyFrame::OnExit)
    EVT_MENU(CLIENT_EXECUTE, MyFrame::OnExecute)
    EVT_MENU(CLIENT_POKE, MyFrame::OnPoke)
    EVT_MENU(CLIENT_REQUEST, MyFrame::OnRequest)
    EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, -1, title, pos, size)
{
    panel = NULL;
}

void MyFrame::OnExecute(wxCommandEvent& event)
{
      if (the_connection)
        the_connection->Execute("Hello from the client!");
}

void MyFrame::OnPoke(wxCommandEvent& event)
{
      if (the_connection)
        the_connection->Poke("An item", "Some data to poke at the server!");
}

void MyFrame::OnRequest(wxCommandEvent& event)
{
      if (the_connection)
      {
        char *data = the_connection->Request("An item");
        if (data)
          wxMessageBox(data, "Client: Request", wxOK);
      }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
  if (the_connection)
    the_connection->Disconnect();

  this->Destroy();
}

// Define the behaviour for the frame closing
void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (the_connection)
  {
    the_connection->Disconnect();
  }
  this->Destroy();
}

MyClient::MyClient(void)
{
}

wxConnectionBase *MyClient::OnMakeConnection(void)
{
  return new MyConnection;
}

MyConnection::MyConnection(void):wxConnection(ipc_buffer, 3999)
{
}

MyConnection::~MyConnection(void)
{
  the_connection = NULL;
}

bool MyConnection::OnAdvise(const wxString& topic, const wxString& item, char *data, int size, wxIPCFormat format)
{
  if (the_list)
  {
    int n = the_list->FindString(data);
    if (n > -1)
      the_list->SetSelection(n);
  }
  return TRUE;
}

bool MyConnection::OnDisconnect()
{
    frame->Destroy();

    the_connection = NULL;
    delete this;

    return TRUE;
}

