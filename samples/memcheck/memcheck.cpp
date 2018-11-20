/////////////////////////////////////////////////////////////////////////////
// Name:        memcheck.cpp
// Purpose:     Memory-checking sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
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

#include "wx/datetime.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#ifndef __WXDEBUG__
#error This program must be compiled in debug mode.
#endif

// Normally, new is automatically defined to be the
// debugging version. If not, this does it.
#if !defined(new) && defined(WXDEBUG_NEW) && wxUSE_MEMORY_TRACING && wxUSE_GLOBAL_MEMORY_OPERATORS
#define new WXDEBUG_NEW
#endif

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void) wxOVERRIDE;
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *parent);
    void OnQuit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

wxIMPLEMENT_APP(MyApp);

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  if ( !wxApp::OnInit() )
      return false;

  // Create the main frame window
  MyFrame *frame = new MyFrame((wxFrame *) NULL);

  // Give it an icon
  frame->SetIcon(wxICON(sample));

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "File");
  frame->SetMenuBar(menu_bar);

  // Make a panel with a message
  wxPanel *panel = new wxPanel(frame);

  (void)new wxStaticText(panel, wxID_ANY, "Hello, this is a minimal debugging wxWidgets program!", wxPoint(10, 10));

  // Show the frame
  frame->Show(true);

#if wxUSE_MEMORY_TRACING
  wxDebugContext::SetCheckpoint();
#endif

  // object allocation
  wxBrush* brush = new wxBrush(*wxRED_BRUSH);
  wxBitmap* bitmap = new wxBitmap(100, 100);

  // non-object allocation
  char *ordinaryNonObject = new char[1000];

  wxString *thing = new wxString;

#if wxUSE_DATETIME
  wxDateTime* date = new wxDateTime;
#endif // wxUSE_DATETIME

  const char *data = (const char*) thing ;

#if wxUSE_MEMORY_TRACING
  // On MSW, Dump() crashes if using wxLogGui,
  // so use wxLogStderr instead.
  wxLog* oldLog = wxLog::SetActiveTarget(new wxLogStderr);

  wxDebugContext::PrintClasses();
  wxDebugContext::Dump();
  wxDebugContext::PrintStatistics();

  // Set back to wxLogGui
  delete wxLog::SetActiveTarget(oldLog);
#endif

  // Don't delete these objects, to force wxApp to flag a memory leak.
//  delete thing;
//  delete date;
//  delete[] ordinaryNonObject;

  return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
wxEND_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *parent):
  wxFrame(parent, wxID_ANY, "MemCheck wxWidgets Sample", wxDefaultPosition, wxSize(400, 200))
{}

// Intercept menu commands
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

