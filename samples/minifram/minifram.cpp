/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:     wxMiniFrame sample
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Robert Roebling
// Licence:     wxWindows licence
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
#include "minifram.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
// #include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/preview.xpm"
#include "bitmaps/help.xpm"
#endif

// start wxWindows

IMPLEMENT_APP(MyApp)

// globas

MyMainFrame   *main_frame = (MyMainFrame*) NULL;
MyMiniFrame   *mini_frame = (MyMiniFrame*) NULL;
bool           mini_frame_exists = FALSE;
wxButton      *button     = (wxButton*) NULL;

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
  // Create the main frame window
  main_frame = new MyMainFrame((wxFrame *) NULL, -1, "wxFrame sample",
     wxPoint(100, 100), wxSize(300, 200));

  main_frame->CreateToolBar(wxNO_BORDER|wxTB_VERTICAL, ID_TOOLBAR);
  InitToolbar(main_frame->GetToolBar());

  button = new wxButton( main_frame, ID_REPARENT, "Press to reparent!" );

  // Create the mini frame window
  mini_frame = new MyMiniFrame( main_frame, -1, "wxMiniFrame sample",
     wxPoint(100, 100), wxSize(220, 100));
  mini_frame_exists = TRUE;

  mini_frame->CreateToolBar(wxNO_BORDER|wxTB_HORIZONTAL|wxTB_FLAT, ID_TOOLBAR);
  InitToolbar(mini_frame->GetToolBar());

#ifdef __WXMSW__
  main_frame->SetIcon(wxIcon("mondrian"));
  mini_frame->SetIcon(wxIcon("mondrian"));
#else
  main_frame->SetIcon( wxIcon(mondrian_xpm) );
  mini_frame->SetIcon( wxIcon(mondrian_xpm) );
#endif

  SetTopWindow(main_frame);

  main_frame->Show(TRUE);
  mini_frame->Show(TRUE);

  return TRUE;
}

bool MyApp::InitToolbar(wxToolBar* toolBar)
{
  toolBar->SetMargins(5, 5);

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
#else
  toolBarBitmaps[0] = new wxBitmap( new_xpm );
  toolBarBitmaps[1] = new wxBitmap( open_xpm );
  toolBarBitmaps[2] = new wxBitmap( save_xpm );
  toolBarBitmaps[3] = new wxBitmap( copy_xpm );
  toolBarBitmaps[4] = new wxBitmap( cut_xpm );
//  toolBarBitmaps[5] = new wxBitmap( paste_xpm );
  toolBarBitmaps[5] = new wxBitmap( preview_xpm );
  toolBarBitmaps[6] = new wxBitmap( print_xpm );
  toolBarBitmaps[7] = new wxBitmap( help_xpm );
#endif

#ifdef __WXMSW__
  int width = 24;
#else
  int width = 16;
#endif
  int currentX = 5;

  toolBar->AddTool(wxID_NEW, *(toolBarBitmaps[0]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "New file");
  currentX += width + 5;
  toolBar->AddTool(wxID_OPEN, *(toolBarBitmaps[1]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Open file");
  currentX += width + 5;
  toolBar->AddTool(wxID_SAVE, *(toolBarBitmaps[2]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Save file");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_COPY, *(toolBarBitmaps[3]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Copy");
  currentX += width + 5;
  toolBar->AddTool(wxID_CUT, *(toolBarBitmaps[4]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Cut");
  currentX += width + 5;
  toolBar->AddTool(wxID_PASTE, *(toolBarBitmaps[5]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Paste");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_PRINT, *(toolBarBitmaps[6]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Reparent the button");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_HELP, *(toolBarBitmaps[7]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Help");

  toolBar->Realize();

  toolBar->EnableTool( wxID_HELP, FALSE );

  // Can delete the bitmaps since they're reference counted
  int i;
  for (i = 0; i < 8; i++)
    delete toolBarBitmaps[i];

  return TRUE;
}

// MyMiniFrame

BEGIN_EVENT_TABLE(MyMiniFrame, wxMiniFrame)
    EVT_CLOSE  (              MyMiniFrame::OnCloseWindow)
    EVT_BUTTON (ID_REPARENT,  MyMiniFrame::OnReparent)
    EVT_MENU   (wxID_PRINT,   MyMiniFrame::OnReparent)
END_EVENT_TABLE()

MyMiniFrame::MyMiniFrame(wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size ) :
  wxMiniFrame(parent, id, title, pos, size )
{
}

void MyMiniFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  // make it known that the miniframe is no more
  mini_frame_exists = FALSE;
  Destroy();
}

void MyMiniFrame::OnReparent(wxCommandEvent& WXUNUSED(event))
{
  button->Reparent( main_frame );
}

// MyMainFrame

BEGIN_EVENT_TABLE(MyMainFrame, wxFrame)
    EVT_CLOSE  (              MyMainFrame::OnCloseWindow)
    EVT_BUTTON (ID_REPARENT,  MyMainFrame::OnReparent)
    EVT_MENU   (wxID_PRINT,   MyMainFrame::OnReparent)
END_EVENT_TABLE()

MyMainFrame::MyMainFrame(wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size ) :
  wxFrame(parent, id, title, pos, size )
{
}

void MyMainFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  Destroy();
}

void MyMainFrame::OnReparent(wxCommandEvent& WXUNUSED(event))
{
  // practical jokers might find satisfaction in reparenting the button
  // after closing the mini_frame. We'll have the last laugh.
  if (! mini_frame_exists)
    wxMessageBox("The miniframe no longer exists.\n"
                 "You don't want to make this button an orphan, do you?",
                 "You got to be kidding");
  else
    button->Reparent( mini_frame );
}


