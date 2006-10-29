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

#if !defined(__WXMSW__) && !defined(__WXPM__)
#include "mondrian.xpm"
#endif
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
// #include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/preview.xpm"
#include "bitmaps/help.xpm"


// start wxWidgets

IMPLEMENT_APP(MyApp)

// globas

MyMainFrame   *main_frame = (MyMainFrame*) NULL;
MyMiniFrame   *mini_frame = (MyMiniFrame*) NULL;
bool           mini_frame_exists = false;
wxButton      *button     = (wxButton*) NULL;

#define ID_SET_SIZE_TO_150_150      100
#define ID_SET_SIZE_TO_200_200      101
#define ID_SET_MAX_SIZE_TO_150_150  102
#define ID_SET_MAX_SIZE_TO_300_300  103

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
  // Create the main frame window
  main_frame = new MyMainFrame((wxFrame *) NULL, wxID_ANY, _T("wxFrame sample"),
     wxPoint(100, 100), wxSize(300, 200));

  main_frame->SetSizeHints( 100,100, 400,400 );

  wxMenu *file_menu = new wxMenu;
  file_menu->Append(wxID_EXIT, _T("E&xit\tAlt-Q"));
  file_menu->Append(ID_SET_SIZE_TO_150_150, _T("Set frame size to 150,150\tF2"));
  file_menu->Append(ID_SET_SIZE_TO_200_200, _T("Set frame size to 200,200\tF3"));
  file_menu->Append(ID_SET_MAX_SIZE_TO_150_150, _T("Set frame max size to 150,150\tF4"));
  file_menu->Append(ID_SET_MAX_SIZE_TO_300_300, _T("Set frame max size to 300,300\tF5"));
  
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, _T("&File"));
  main_frame->SetMenuBar(menu_bar);

  main_frame->Connect( ID_SET_SIZE_TO_150_150,  wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyMainFrame::OnSetSize_150_150) );
  main_frame->Connect( ID_SET_SIZE_TO_200_200, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyMainFrame::OnSetSize_200_200) );
  main_frame->Connect( ID_SET_MAX_SIZE_TO_150_150,  wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyMainFrame::OnSetMaxSize_150_150) );
  main_frame->Connect( ID_SET_MAX_SIZE_TO_300_300, wxID_ANY,
                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyMainFrame::OnSetMaxSize_300_300) );

  main_frame->CreateToolBar(wxNO_BORDER|wxTB_VERTICAL, ID_TOOLBAR);
  InitToolbar(main_frame->GetToolBar());

  button = new wxButton( main_frame, ID_REPARENT, _T("Press to reparent!") );

  // Create the mini frame window
  mini_frame = new MyMiniFrame( main_frame, wxID_ANY, _T("wxMiniFrame sample"),
     wxPoint(100, 100), wxSize(220, 100));
  mini_frame_exists = true;

  mini_frame->CreateToolBar(wxNO_BORDER|wxTB_HORIZONTAL|wxTB_FLAT, ID_TOOLBAR);
  InitToolbar(mini_frame->GetToolBar());

  main_frame->SetIcon(wxICON(mondrian));
  mini_frame->SetIcon(wxICON(mondrian));

  SetTopWindow(main_frame);

  main_frame->Show(true);
  mini_frame->Show(true);

  return true;
}

bool MyApp::InitToolbar(wxToolBar* toolBar)
{
  toolBar->SetMargins(5, 5);

  // Set up toolbar
  wxBitmap* toolBarBitmaps[8];

  toolBarBitmaps[0] = new wxBitmap( new_xpm );
  toolBarBitmaps[1] = new wxBitmap( open_xpm );
  toolBarBitmaps[2] = new wxBitmap( save_xpm );
  toolBarBitmaps[3] = new wxBitmap( copy_xpm );
  toolBarBitmaps[4] = new wxBitmap( cut_xpm );
//  toolBarBitmaps[5] = new wxBitmap( paste_xpm );
  toolBarBitmaps[5] = new wxBitmap( preview_xpm );
  toolBarBitmaps[6] = new wxBitmap( print_xpm );
  toolBarBitmaps[7] = new wxBitmap( help_xpm );

  int width = 16;
  int currentX = 5;

  toolBar->AddTool(wxID_NEW, *(toolBarBitmaps[0]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("New file"));
  currentX += width + 5;
  toolBar->AddTool(wxID_OPEN, *(toolBarBitmaps[1]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Open file"));
  currentX += width + 5;
  toolBar->AddTool(wxID_SAVE, *(toolBarBitmaps[2]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Save file"));
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_COPY, *(toolBarBitmaps[3]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Copy"));
  currentX += width + 5;
  toolBar->AddTool(wxID_CUT, *(toolBarBitmaps[4]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Cut"));
  currentX += width + 5;
  toolBar->AddTool(wxID_PASTE, *(toolBarBitmaps[5]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Paste"));
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_PRINT, *(toolBarBitmaps[6]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Reparent the button"));
  currentX += width + 5;
  toolBar->AddSeparator();
  toolBar->AddTool(wxID_HELP, *(toolBarBitmaps[7]), wxNullBitmap, false, currentX, wxDefaultCoord, (wxObject *) NULL, _T("Help"));

  toolBar->Realize();

  toolBar->EnableTool( wxID_HELP, false );

  // Can delete the bitmaps since they're reference counted
  int i;
  for (i = 0; i < 8; i++)
    delete toolBarBitmaps[i];

  return true;
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
  mini_frame_exists = false;
  Destroy();
}

void MyMiniFrame::OnReparent(wxCommandEvent& WXUNUSED(event))
{
  button->Reparent( main_frame );

  // we need to force the frame to size its (new) child correctly
  main_frame->SendSizeEvent();
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
    wxMessageBox(_T("The miniframe no longer exists.\n")
                 _T("You don't want to make this button an orphan, do you?"),
                 _T("You got to be kidding"));
  else
  {
    button->Reparent( mini_frame );

    // same as above
    mini_frame->SendSizeEvent();
  }
}

void MyMainFrame::OnSetSize_150_150(wxCommandEvent& WXUNUSED(event))
{
    SetSize( 150, 150 );
}

void MyMainFrame::OnSetSize_200_200(wxCommandEvent& WXUNUSED(event))
{
    SetSize( 200, 200 );
}

void MyMainFrame::OnSetMaxSize_150_150(wxCommandEvent& WXUNUSED(event))
{
    SetSizeHints( -1, -1, 150, 150 );
}

void MyMainFrame::OnSetMaxSize_300_300(wxCommandEvent& WXUNUSED(event))
{
    SetSizeHints( -1, -1, 300, 300 );
}
