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
  fileMenu->Append(TEST_QUIT, "E&xit");

  wxMenu *helpMenu = new wxMenu;
  helpMenu->Append(TEST_ABOUT, "&About");

  wxMenuBar* menuBar = new wxMenuBar;

  menuBar->Append(fileMenu, "&File");
  menuBar->Append(helpMenu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menuBar);

  // Create the toolbar
  TestToolBar* toolRibbon = new TestToolBar(frame, -1, wxPoint(0, 0), wxSize(100, 30),
    wxNO_BORDER|wxTB_FLAT, wxVERTICAL, 1);
  toolRibbon->SetMargins(5, 5);

  // Tell the frame about it
  frame->SetToolBar(toolRibbon);

  // Force a resize, just in case.
  frame->OnSize(wxSizeEvent(wxSize(-1, -1), frame->GetId()));
  frame->Show(TRUE);

  frame->SetStatusText("Hello, wxWindows");
  
  SetTopWindow(frame);

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU_HIGHLIGHT_ALL(MyFrame::OnMenuHighlight)
    EVT_CLOSE(MyFrame::OnCloseWindow)
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

// Intercept menu item selection - only has an effect in Windows
void MyFrame::OnMenuHighlight(wxMenuEvent& event)
{
  char *msg = NULL;
  switch (event.GetMenuId())
  {
    case TEST_QUIT:
      msg = "Quit program";
      break;
    case -1:
      msg = "";
      break;
  }
  if (msg)
    SetStatusText(msg);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
  Destroy();
}

BEGIN_EVENT_TABLE(TestToolBar, wxToolBar95)
    EVT_PAINT(TestToolBar::OnPaint)
END_EVENT_TABLE()

TestToolBar::TestToolBar(wxFrame* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, int direction, int RowsOrColumns):
  wxToolBar(parent, id, pos, size, style, direction, RowsOrColumns)
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

  AddTool(0, *(toolBarBitmaps[0]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "New file");
  currentX += width + 5;
  AddTool(1, *(toolBarBitmaps[1]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Open file");
  currentX += width + 5;
  AddTool(2, *(toolBarBitmaps[2]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Save file");
  currentX += width + 5;
  AddSeparator();
  AddTool(3, *(toolBarBitmaps[3]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Copy");
  currentX += width + 5;
  AddTool(4, *(toolBarBitmaps[4]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Cut");
  currentX += width + 5;
  AddTool(5, *(toolBarBitmaps[5]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Paste");
  currentX += width + 5;
  AddSeparator();
  AddTool(6, *(toolBarBitmaps[6]), wxNullBitmap, FALSE, (float)currentX, -1, NULL, "Print");
  currentX += width + 5;
  AddSeparator();
  AddTool(7, *(toolBarBitmaps[7]), wxNullBitmap, TRUE, currentX, -1, NULL, "Help");

  CreateTools();

  // Can delete the bitmaps since they're reference counted
  int i;
  for (i = 0; i < 8; i++)
    delete toolBarBitmaps[i];
}

bool TestToolBar::OnLeftClick(int toolIndex, bool toggled)
{
  char buf[200];
  sprintf(buf, "Clicked on tool %d", toolIndex);
  ((wxFrame*) GetParent())->SetStatusText(buf);
  return TRUE;
}

void TestToolBar::OnMouseEnter(int toolIndex)
{
  char buf[200];
  if (toolIndex > -1)
  {
    sprintf(buf, "This is tool number %d", toolIndex);
    ((wxFrame*)GetParent())->SetStatusText(buf);
  }
  else ((wxFrame*)GetParent())->SetStatusText("");
}

void TestToolBar::OnPaint(wxPaintEvent& event)
{
  wxToolBar::OnPaint(event);

  wxPaintDC dc(this);
  
  int w, h;
  GetSize(&w, &h);
  dc.SetPen(wxBLACK_PEN);
  dc.SetBrush(wxTRANSPARENT_BRUSH);
  dc.DrawLine(0, h-1, w, h-1);
}


