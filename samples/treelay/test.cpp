///////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxTreeLayout sample
// Author:      Julian Smart
// Modified by: 
// Created:     7/4/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/treelay.h>

#include "test.h"

wxTreeLayoutStored *myTree = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_APP(MyApp)

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame* frame = new MyFrame(NULL, "Tree Test", wxPoint(-1, -1), wxSize(400, 550));

  // Give it a status line
  frame->CreateStatusBar(2);

  // Give it an icon
#ifdef __WINDOWS__
  wxIcon icon("tree_icn");
  frame->SetIcon(icon);
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(TEST_LEFT_RIGHT, "&Left to right",                "Redraw left to right");
  file_menu->Append(TEST_TOP_BOTTOM, "&Top to bottom",                "Redraw top to bottom");
  file_menu->AppendSeparator();
  file_menu->Append(TEST_QUIT, "E&xit",                "Quit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(TEST_ABOUT, "&About",              "About Tree Test");

  wxMenuBar* menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

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

  frame->Show(TRUE);

  frame->SetStatusText("Hello, tree!");

  // Return the main frame window
  return TRUE;
}

void MyApp::TreeTest(wxTreeLayoutStored& tree, wxDC& dc)
{
  tree.Initialize(200);
  
  tree.AddChild("animal");
  tree.AddChild("mammal", "animal");
  tree.AddChild("insect", "animal");
  tree.AddChild("bird", "animal");

  tree.AddChild("man", "mammal");
  tree.AddChild("cat", "mammal");
  tree.AddChild("dog", "mammal");
  tree.AddChild("giraffe", "mammal");
  tree.AddChild("elephant", "mammal");
  tree.AddChild("donkey", "mammal");
  tree.AddChild("horse", "mammal");

  tree.AddChild("fido", "dog");
  tree.AddChild("domestic cat", "cat");
  tree.AddChild("lion", "cat");
  tree.AddChild("tiger", "cat");
  tree.AddChild("felix", "domestic cat");
  tree.AddChild("socks", "domestic cat");

  tree.AddChild("beetle", "insect");
  tree.AddChild("earwig", "insect");
  tree.AddChild("eagle", "bird");
  tree.AddChild("bluetit", "bird");
  tree.AddChild("sparrow", "bird");
  tree.AddChild("blackbird", "bird");
  tree.AddChild("emu", "bird");
  tree.AddChild("crow", "bird");

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
  wxFrame(parent, -1, title, pos, size)
{
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void MyFrame::OnLeftRight(wxCommandEvent& event)
{
      if (myTree)
      {
        myTree->SetOrientation(FALSE);
        wxClientDC dc(canvas);
        wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
        dc.SetFont(font);
        wxGetApp().TreeTest(*myTree, dc);
        canvas->Refresh();
      }
}

void MyFrame::OnTopBottom(wxCommandEvent& event)
{
      if (myTree)
      {
        myTree->SetOrientation(TRUE);
        wxClientDC dc(canvas);
        wxFont font(10, wxROMAN, wxNORMAL, wxBOLD);
        dc.SetFont(font);
        wxGetApp().TreeTest(*myTree, dc);
        canvas->Refresh();
      }
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
      (void)wxMessageBox("wxWindows tree library demo Vsn 2.0\nAuthor: Julian Smart (c) 1998", "About tree test");
}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
  Destroy();
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent):
 wxScrolledWindow(parent, -1)
{
    SetBackgroundColour(*wxWHITE);
}

// Define the repainting behaviour
void MyCanvas::OnPaint(wxPaintEvent& event)
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

