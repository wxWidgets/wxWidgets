/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/toolbar.h"
#include "test.h"

IMPLEMENT_APP(MyApp)

#ifdef __X__
// TODO: include XBM or XPM icons for X apps
#endif

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame* frame = new MyFrame(NULL, -1, "wxToolBar Sample", wxPoint(100, 100), wxSize(450, 300));

  // Give it a status line
  frame->CreateStatusBar();

  // Give it an icon
#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mondrian"));
#endif
#ifdef __X__
  frame->SetIcon(wxIcon("mondrian.xbm"));
#endif

  // Make a menubar
  wxMenu *fileMenu = new wxMenu;
  fileMenu->Append(wxID_EXIT, "E&xit");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, "&About");

  wxMenuBar* menuBar = new wxMenuBar;

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(helpMenu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menuBar);

  // Create the toolbar
  wxToolBar* toolBar = new wxToolBar(frame, -1, wxPoint(0, 0), wxSize(100, 30),
    wxNO_BORDER|wxTB_FLAT, wxVERTICAL, 1);
  toolBar->SetMargins(5, 5);

  InitToolbar(toolBar);

  // Tell the frame about it
  frame->SetToolBar(toolBar);

  // Force a resize. This should probably be replaced by a call to a wxFrame
  // function that lays out default decorations and the remaining content window.
  frame->OnSize(wxSizeEvent(wxSize(-1, -1), frame->GetId()));
  frame->Show(TRUE);

  frame->SetStatusText("Hello, wxWindows");
  
  SetTopWindow(frame);

  return TRUE;
}

bool MyApp::InitToolbar(wxToolBar* toolBar)
{
  // Set up toolbar
  wxBitmap* toolBarBitmaps[8];

#ifdef __WXMSW__
  toolBarBitmaps[0] = new wxBitmap("icon1");
  toolBarBitmaps[1] = new wxBitmap("icon2");
  toolBarBitmaps[2] = new wxBitmap("icon3");
  toolBarBitmaps[3] = new wxBitmap("icon4");
  toolBarBitmaps[4] = new wxBitmap("icon5");
  toolBarBitmaps[5] = new wxBitmap("icon6");
  toolBarBitmaps[6] = new wxBitmap("icon7");
  toolBarBitmaps[7] = new wxBitmap("icon8");
#endif
#ifdef __X__
  // TODO
  toolBarBitmaps[0] = new wxBitmap(...);
  toolBarBitmaps[1] = new wxBitmap(...);
  toolBarBitmaps[2] = new wxBitmap(...);
  toolBarBitmaps[3] = new wxBitmap(...);
  toolBarBitmaps[4] = new wxBitmap(...);
  toolBarBitmaps[5] = new wxBitmap(...);
  toolBarBitmaps[6] = new wxBitmap(...);
  toolBarBitmaps[7] = new wxBitmap(...);
#endif

#ifdef __WXMSW__
  int width = 24;
#else
  int width = 16;
#endif
  int offX = 5;
  int currentX = 5;

  toolBar->AddTool(wxID_NEW, *(toolBarBitmaps[0]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "New file");
  currentX += width + 5;
  toolBar->AddTool(wxID_OPEN, *(toolBarBitmaps[1]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Open file");
  currentX += width + 5;
  toolBar->AddTool(wxID_SAVE, *(toolBarBitmaps[2]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Save file");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_COPY, *(toolBarBitmaps[3]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Copy");
  currentX += width + 5;
  toolBar->AddTool(wxID_CUT, *(toolBarBitmaps[4]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Cut");
  currentX += width + 5;
  toolBar->AddTool(wxID_PASTE, *(toolBarBitmaps[5]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Paste");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_PRINT, *(toolBarBitmaps[6]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Print");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_HELP, *(toolBarBitmaps[7]), wxNullBitmap, FALSE, currentX, -1, NULL, "Help");

  toolBar->CreateTools();

  // Can delete the bitmaps since they're reference counted
  int i;
  for (i = 0; i < 8; i++)
    delete toolBarBitmaps[i];

  return TRUE;
}

// wxID_HELP will be processed for the 'About' menu and the toolbar help button.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_HELP, MyFrame::OnAbout)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_TOOL_RANGE(wxID_OPEN, wxID_PASTE, MyFrame::OnToolLeftClick)
    EVT_TOOL_ENTER_RANGE(wxID_OPEN, wxID_PASTE, MyFrame::OnToolEnter)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style):
  wxFrame(parent, id, title, pos, size, style)
{
  m_textWindow = new wxTextCtrl(this, -1, "", wxPoint(0, 0), wxSize(-1, -1), wxTE_MULTILINE);
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    (void)wxMessageBox("wxWindows wxToolBar demo\n", "About wxToolBar");
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
  Destroy();
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
  wxString str;
  str.Printf("Clicked on tool %d", event.GetId());
  SetStatusText(str);
}

void MyFrame::OnToolEnter(wxCommandEvent& event)
{
  if (event.GetId() > -1)
  {
    wxString str;
    str.Printf("This is tool number %d", event.GetId());
    SetStatusText(str);
  }
  else
    SetStatusText("");
}

