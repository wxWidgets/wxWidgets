///////////////////////////////////////////////////////////////////////////////
// Name:        treelay.cpp
// Purpose:     wxTreeLayout sample
// Author:      Julian Smart
// Modified by:
// Created:     7/4/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/deprecated/setup.h"

#if !wxUSE_TREELAYOUT
#error Please set wxUSE_TREELAYOUT to 1 in contrib/include/wx/deprecated/setup.h and recompile.
#endif

#include "wx/deprecated/treelay.h"

#include "treelay.h"

wxTreeLayoutStored *myTree = (wxTreeLayoutStored *) NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_APP(MyApp)

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame* frame = new MyFrame(NULL, _T("Tree Test"), wxDefaultPosition, wxSize(400, 550));

#if wxUSE_STATUSBAR
  // Give it a status line
  frame->CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

  // Give it an icon
#ifdef __WINDOWS__
  wxIcon icon(_T("tree_icn"));
  frame->SetIcon(icon);
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_LEFT_RIGHT, _T("&Left to right"),                _T("Redraw left to right"));
  file_menu->Append(TEST_TOP_BOTTOM, _T("&Top to bottom"),                _T("Redraw top to bottom"));
  file_menu->AppendSeparator();
  file_menu->Append(TEST_QUIT, _T("E&xit"),                _T("Quit program"));

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, _T("&About"),              _T("About Tree Test"));

  wxMenuBar* menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  MyCanvas *canvas = new MyCanvas(frame);

  // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
  canvas->SetScrollbars(20, 20, 50, 50);
  frame->canvas = canvas;

  myTree = new wxTreeLayoutStored();

  wxClientDC dc(canvas);
  wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
  dc.SetFont(font);
  TreeTest(*myTree, dc);

  frame->Show(true);

#if wxUSE_STATUSBAR
  frame->SetStatusText(_T("Hello, tree!"));
#endif // wxUSE_STATUSBAR

  // Return the main frame window
  return true;
}

int MyApp::OnExit()
{
    if (myTree)
    {
        delete myTree;
        myTree = (wxTreeLayoutStored *) NULL;
    }

    return 0;
}

void MyApp::TreeTest(wxTreeLayoutStored& tree, wxDC& dc)
{
  tree.Initialize(200);

  tree.AddChild(_T("animal"));
  tree.AddChild(_T("mammal"), _T("animal"));
  tree.AddChild(_T("insect"), _T("animal"));
  tree.AddChild(_T("bird"), _T("animal"));

  tree.AddChild(_T("man"), _T("mammal"));
  tree.AddChild(_T("cat"), _T("mammal"));
  tree.AddChild(_T("dog"), _T("mammal"));
  tree.AddChild(_T("giraffe"), _T("mammal"));
  tree.AddChild(_T("elephant"), _T("mammal"));
  tree.AddChild(_T("donkey"), _T("mammal"));
  tree.AddChild(_T("horse"), _T("mammal"));

  tree.AddChild(_T("fido"), _T("dog"));
  tree.AddChild(_T("domestic cat"), _T("cat"));
  tree.AddChild(_T("lion"), _T("cat"));
  tree.AddChild(_T("tiger"), _T("cat"));
  tree.AddChild(_T("felix"), _T("domestic cat"));
  tree.AddChild(_T("socks"), _T("domestic cat"));

  tree.AddChild(_T("beetle"), _T("insect"));
  tree.AddChild(_T("earwig"), _T("insect"));
  tree.AddChild(_T("eagle"), _T("bird"));
  tree.AddChild(_T("bluetit"), _T("bird"));
  tree.AddChild(_T("sparrow"), _T("bird"));
  tree.AddChild(_T("blackbird"), _T("bird"));
  tree.AddChild(_T("emu"), _T("bird"));
  tree.AddChild(_T("crow"), _T("bird"));

  tree.DoLayout(dc);
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(TEST_QUIT, MyFrame::OnQuit)
  EVT_MENU(TEST_ABOUT, MyFrame::OnAbout)
  EVT_MENU(TEST_LEFT_RIGHT, MyFrame::OnLeftRight)
  EVT_MENU(TEST_TOP_BOTTOM, MyFrame::OnTopBottom)
  EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(parent, wxID_ANY, title, pos, size)
{
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnLeftRight(wxCommandEvent& WXUNUSED(event))
{
      if (myTree)
      {
        myTree->SetOrientation(false);
        wxClientDC dc(canvas);
        wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
        dc.SetFont(font);
        wxGetApp().TreeTest(*myTree, dc);
        canvas->Refresh();
      }
}

void MyFrame::OnTopBottom(wxCommandEvent& WXUNUSED(event))
{
      if (myTree)
      {
        myTree->SetOrientation(true);
        wxClientDC dc(canvas);
        wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
        dc.SetFont(font);
        wxGetApp().TreeTest(*myTree, dc);
        canvas->Refresh();
      }
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
      (void)wxMessageBox(_T("wxWidgets tree library demo Vsn 2.0\nAuthor: Julian Smart (c) 1998"), _T("About tree test"));
}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  Destroy();
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent):
 wxScrolledWindow(parent, wxID_ANY)
{
    SetBackgroundColour(*wxWHITE);
}

// Define the repainting behaviour
void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    if (myTree)
    {
        wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
        dc.SetFont(font);
        myTree->Draw(dc);
    }
}

