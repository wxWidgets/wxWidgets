///////////////////////////////////////////////////////////////////////////////
// Name:        bombs.cpp
// Purpose:     Bombs game
// Author:      P. Foggia 1996
// Modified by:
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "bombs.h"

#include <time.h>
#include <stdlib.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "bombs.xpm"
#endif

IMPLEMENT_APP(AppClass)

// Called to initialize the program
bool AppClass::OnInit()
{
  srand((unsigned)time(NULL));

  // Initialize all the top-level window members to NULL.
  BombsFrame = NULL;
  level=IDM_EASY;

  BombsFrame = 
    new BombsFrameClass(NULL, "wxBombs", wxPoint(155, 165), wxSize(300, 300), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION);

  int xmax=BombsFrame->BombsCanvas->field_width*BombsFrame->BombsCanvas->x_cell*X_UNIT;
  int ymax=BombsFrame->BombsCanvas->field_height*BombsFrame->BombsCanvas->y_cell*Y_UNIT;
  BombsFrame->SetClientSize(xmax, ymax);

  return TRUE;
}

BEGIN_EVENT_TABLE(BombsFrameClass, wxFrame)
    EVT_MENU(IDM_EASY, BombsFrameClass::OnEasy)
    EVT_MENU(IDM_MEDIUM, BombsFrameClass::OnMedium)
    EVT_MENU(IDM_DIFFICULT, BombsFrameClass::OnDifficult)
    EVT_MENU(IDM_EXIT, BombsFrameClass::OnExit)
    EVT_MENU(IDM_ABOUT, BombsFrameClass::OnAbout)
    EVT_MENU(IDM_RESTART, BombsFrameClass::OnRestart)
    EVT_CLOSE(BombsFrameClass::OnCloseWindow)
END_EVENT_TABLE()

BombsFrameClass::BombsFrameClass(wxFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxFrame(parent, -1, title, pos, size, style)
{
  // Initialize child subwindow members.
  BombsCanvas = NULL;

  SetIcon(wxICON(bombs));

  CreateStatusBar();
  
  // Create a menu bar for the frame
  wxMenuBar *menuBar1 = new wxMenuBar;
  wxMenu *menu1 = new wxMenu;
  menu1->Append(IDM_EXIT, "E&xit"); // , "Quit the program");
  menu1->AppendSeparator();
  menu1->Append(IDM_ABOUT, "&About..."); // , "Infos on wxBombs");
  menuBar1->Append(menu1, "&File");
  wxMenu *menu2 = new wxMenu;
  menu2->Append(IDM_RESTART, "&Restart"); // , "Clear the play field");
  menu2->AppendSeparator();
  menu2->Append(IDM_EASY, "&Easy", NULL, TRUE); // "10x10 play field", TRUE);
  menu2->Append(IDM_MEDIUM, "&Medium", NULL, TRUE); // "15x15 play field", TRUE);
  menu2->Append(IDM_DIFFICULT, "&Difficult", NULL, TRUE); // "25x20 play field", TRUE);
  menuBar1->Append(menu2, "&Game");
  SetMenuBar(menuBar1);
  menuBar=menuBar1;
  menuBar->Check(wxGetApp().level, TRUE);

  // Create child subwindows.
  BombsCanvas = new BombsCanvasClass(this);

  // Ensure the subwindows get resized o.k.
//  OnSize(width, height);

  // Centre frame on the screen.
  Centre(wxBOTH);

  // Show the frame.
  Show(TRUE);
}

BombsFrameClass::~BombsFrameClass(void)
{
}

void BombsFrameClass::OnCloseWindow(wxCloseEvent& event)
{
  this->Destroy();
}

void BombsFrameClass::OnExit(wxCommandEvent& event)
{
    this->Destroy();
}

void BombsFrameClass::OnRestart(wxCommandEvent& event)
{
    BombsCanvas->UpdateFieldSize();
    int xmax=BombsCanvas->field_width*BombsCanvas->x_cell*X_UNIT;
    int ymax=BombsCanvas->field_height*BombsCanvas->y_cell*Y_UNIT;
    wxGetApp().BombsFrame->SetClientSize(xmax, ymax);
}

void BombsFrameClass::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("wxBombs (c) 1996 by P. Foggia\n<foggia@amalfi.dis.unina.it>", "About wxBombs");
}

void BombsFrameClass::OnEasy(wxCommandEvent& event)
{
    menuBar->Check(wxGetApp().level, FALSE);
    wxGetApp().level=IDM_EASY;
    menuBar->Check(wxGetApp().level, TRUE);
}

void BombsFrameClass::OnMedium(wxCommandEvent& event)
{
    menuBar->Check(wxGetApp().level, FALSE);
    wxGetApp().level=IDM_MEDIUM;
    menuBar->Check(wxGetApp().level, TRUE);
}

void BombsFrameClass::OnDifficult(wxCommandEvent& event)
{
    menuBar->Check(wxGetApp().level, FALSE);
    wxGetApp().level=IDM_DIFFICULT;
    menuBar->Check(wxGetApp().level, TRUE);
}

BEGIN_EVENT_TABLE(BombsCanvasClass, wxWindow)
    EVT_PAINT(BombsCanvasClass::OnPaint)
    EVT_MOUSE_EVENTS(BombsCanvasClass::OnEvent)
END_EVENT_TABLE()

BombsCanvasClass::BombsCanvasClass(wxFrame *parent, const wxPoint& pos, const wxSize& size, long style):
  wxWindow(parent, -1, pos, size, style)
{ 
  int sx, sy;
  wxClientDC dc(this);
  wxFont font= BOMBS_FONT;
  dc.SetFont(font);

  long chw, chh;
  char buf[]="M";

  dc.GetTextExtent(buf, &chw, &chh);
  dc.SetFont(wxNullFont);

  dc.SetMapMode(MM_METRIC);

  int xcm = dc.LogicalToDeviceX(10.0);
  int ycm = dc.LogicalToDeviceY(10.0);
    // To have a square cell, there must be :
    //    sx*ycm == sy*xcm
  if (chw*ycm < chh*xcm)
    { sy=chh;
      sx=chh*xcm/ycm;
    }
  else
    { sx=chw;
      sy=chw*ycm/xcm;
    }
  x_cell = (sx+3+X_UNIT)/X_UNIT;
  y_cell = (sy+3+Y_UNIT)/Y_UNIT;
  dc.SetMapMode(MM_TEXT);
  bmp=NULL;
  UpdateFieldSize();
}

BombsCanvasClass::~BombsCanvasClass(void)
{
  if (bmp)
    delete bmp;
}

// Called when canvas needs to be repainted.
void BombsCanvasClass::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);

  // Insert your drawing code here.
  if (!bmp)
    { bmp=new wxBitmap(field_width*x_cell*X_UNIT+1,
                       field_height*y_cell*Y_UNIT+1);
      if (bmp)
        { wxMemoryDC memDC;
          memDC.SelectObject(* bmp);
          DrawField(&memDC, 0, 0, field_width-1, field_height-1);
          memDC.SelectObject(wxNullBitmap);
        }
    }
  if (bmp)
    { wxMemoryDC memDC;
      memDC.SelectObject(* bmp);
      dc.Blit(0, 0, field_width*x_cell*X_UNIT+1,
                          field_height*y_cell*Y_UNIT+1,
                    &memDC, 0, 0, wxCOPY);
      memDC.SelectObject(wxNullBitmap);
    }
  else
    DrawField(& dc, 0, 0, field_width-1, field_height-1);
}

// Updates the field size depending on wxGetApp().level and
// redraws the canvas
void BombsCanvasClass::UpdateFieldSize()
  { field_width=20;
    field_height=20;

    switch(wxGetApp().level)
      { case IDM_EASY:
          field_width=10;
          field_height=10;
          break;
        case IDM_MEDIUM:
          field_width=15;
          field_height=15;
          break;
        case IDM_DIFFICULT:
          field_width=25;
          field_height=20;
          break;
      }
    wxGetApp().Game.Init(field_width, field_height);

    if (bmp)
      delete bmp;
    bmp=NULL;
    
    wxWindow::Refresh();
  }
