/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxMiniFrame sample
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Robert Roebling
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

IMPLEMENT_APP(MyApp)


// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame* frame = new MyFrame((wxFrame *) NULL, -1, (const wxString) "wxMiniFrame sample",
     wxPoint(100, 100), wxSize(205, 45));

#ifdef __WXMSW__
  frame->SetIcon(wxIcon("mondrian"));
#else
  frame->SetIcon( wxIcon(mondrian_xpm) );
#endif

  // Create the toolbar
  frame->CreateToolBar(wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT, ID_TOOLBAR);

  InitToolbar(frame->GetToolBar());

  frame->Show(TRUE);
  SetTopWindow(frame);

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
  toolBar->AddTool(wxID_PRINT, *(toolBarBitmaps[6]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Print");
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_HELP, *(toolBarBitmaps[7]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Help");

  toolBar->Realize();

  // Can delete the bitmaps since they're reference counted
  int i;
  for (i = 0; i < 8; i++)
    delete toolBarBitmaps[i];

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxMiniFrame)
    EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size ) :
  wxMiniFrame(parent, id, title, pos, size )
{
}

// - must delete all frames except for the main one.
void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  Destroy();
}

