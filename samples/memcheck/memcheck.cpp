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

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif

#ifndef __WXDEBUG__
#error This program must be compiled in debug mode.
#endif

// Normally, new is automatically defined to be the
// debugging version. If not, this does it.
#if !defined(new) && defined(WXDEBUG_NEW)
#define new WXDEBUG_NEW
#endif

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
  MyFrame *frame = new MyFrame((wxFrame *) NULL);

  // Give it an icon
  frame->SetIcon(wxICON(mondrian));

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

  wxDebugContext::SetCheckpoint();

  wxString *thing = new wxString;
  wxDate* date = new wxDate;

  // non-object allocation
  char *ordinaryNonObject = new char[1000];

  const char *data = (const char*) thing ;

  // On MSW, Dump() crashes if using wxLogGui,
  // so use wxLogStderr instead.
  wxLog* oldLog = wxLog::SetActiveTarget(new wxLogStderr);

  wxDebugContext::PrintClasses();
  wxDebugContext::Dump();
  wxDebugContext::PrintStatistics();

  // Set back to wxLogGui
  delete wxLog::SetActiveTarget(oldLog);

  // Don't delete these objects, to force wxApp to flag a memory leak.
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
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

