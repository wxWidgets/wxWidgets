/////////////////////////////////////////////////////////////////////////////
// Name:        memcheck.cpp
// Purpose:     Memory-checking sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/date.h"

#if !DEBUG
#error You must set DEBUG to 1 on the 'make' command line or make.env.
#endif

#define new WXDEBUG_NEW

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *parent);
    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef wx_x
  frame->SetIcon(wxIcon("mondrian.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame);

  (void)new wxStaticText(panel, -1, "Hello, this is a minimal debugging wxWindows program!", wxPoint(10, 10));

  // Show the frame
  frame->Show(TRUE);

//  wxDebugContext::SetCheckpoint();
  wxDebugContext::SetFile("debug.log");

  wxString *thing = new wxString; // WXDEBUG_NEW wxString;
  wxDate* date = new wxDate;

  // Proves that defining 'new' to be 'WXDEBUG_NEW' doesn't mess up
  // non-object allocation
  char *ordinaryNonObject = new char[1000];

  const char *data = (const char*) thing ;

  wxDebugContext::PrintClasses();
  wxDebugContext::Dump();
  wxDebugContext::PrintStatistics();

  // Don't delete these two objects, to force wxApp to flag a memory leak.
//  delete thing;
//  delete date;
//  delete[] ordinaryNonObject;
  
  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *parent):
  wxFrame(parent, -1, "MemCheck wxWindows Sample", wxPoint(-1, -1), wxSize(400, 200))
{}

// Intercept menu commands
void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

