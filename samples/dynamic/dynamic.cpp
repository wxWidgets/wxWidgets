/////////////////////////////////////////////////////////////////////////////
// Name:        dynamic.cpp
// Purpose:     Dynamic events wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dynamic.cpp"
#pragma interface "dynamic.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#include "mondrian.xpm"
#endif

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);

 public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

// ID for the menu commands
#define DYNAMIC_QUIT   1
#define DYNAMIC_TEXT   101
#define DYNAMIC_ABOUT   102

// Create a new application object
IMPLEMENT_APP  (MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Dynamic wxWindows App", 50, 50, 450, 340);

  frame->Connect( DYNAMIC_QUIT,  -1, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &MyFrame::OnQuit );
  frame->Connect( DYNAMIC_ABOUT, -1, wxEVT_COMMAND_MENU_SELECTED,
                  (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                  &MyFrame::OnAbout );

  // Give it an icon
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mondrian"));
#else
  frame->SetIcon(wxIcon(mondrian_xpm));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(DYNAMIC_ABOUT, "&About");
  file_menu->Append(DYNAMIC_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame, -1, wxPoint(0, 0), wxSize(400, 200), wxTAB_TRAVERSAL);

  (void)new wxStaticText(panel, 311, "Hello!", wxPoint(10, 10), wxSize(-1, -1), 0);

  // Show the frame
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, "This demonstrates dynamic event handling",
    "About Dynamic", wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}


