///////////////////////////////////////////////////////////////////////////////
// Name:        bombs.h
// Purpose:     Bombs game
// Author:      P. Foggia 1996
// Modified by:
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _INC_BOMBS_H
#define _INC_BOMBS_H

#include "game.h"

/*
 * Forward declarations of all top-level window classes.
 */
class  BombsFrameClass;
class  AboutFrameClass;

/*
 * Class representing the entire Application
 */
class AppClass: public wxApp
{
 public:
  BombsFrameClass *BombsFrame;
  int level;
  BombsGame Game;

  bool OnInit();
};

DECLARE_APP(AppClass)

class BombsCanvasClass;

class BombsFrameClass: public wxFrame
{
 private:
 protected:
 public:
  // Subwindows for reference within the program.
  BombsCanvasClass *BombsCanvas;
  wxMenuBar *menuBar;

  // Constructor and destructor
  BombsFrameClass(wxFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style);
  ~BombsFrameClass(void);

 void OnCloseWindow(wxCloseEvent& event);
 void OnExit(wxCommandEvent& event);
 void OnRestart(wxCommandEvent& event);
 void OnAbout(wxCommandEvent& event);
 void OnEasy(wxCommandEvent& event);
 void OnMedium(wxCommandEvent& event);
 void OnDifficult(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

/* Menu identifiers
 */
// File
#define BOMBSFRAMECLASS_FILE 1
// E&xit
#define IDM_EXIT 2
// About...
#define IDM_ABOUT 3
// Game
#define BOMBSFRAMECLASS_GAME 4
// &Restart
#define IDM_RESTART 5
// &Easy
#define IDM_EASY 6
// &Medium
#define IDM_MEDIUM 7
// &Difficult
#define IDM_DIFFICULT 8

class BombsCanvasClass: public wxWindow
{
 private:
 protected:
 public:
   int field_width, field_height;
   int x_cell, y_cell;
   wxBitmap *bmp;
  // Constructor and destructor
  BombsCanvasClass(wxFrame *parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
  ~BombsCanvasClass(void);

 void OnPaint(wxPaintEvent& event);
 void DrawField(wxDC *, int xc1, int yc1, int xc2, int yc2);
 void Refresh(int xc1, int yc1, int xc2, int yc2);
 void OnEvent(wxMouseEvent& event);
 void UpdateFieldSize();

DECLARE_EVENT_TABLE()
};

/* Menu identifiers
 */

/* The following sizes should probably be redefined */
/* dimensions of a scroll unit, in pixels */
#define X_UNIT 4
#define Y_UNIT 4

/* the dimensions of a cell, in scroll units are in
 * BombsCanvasClass::x_cell and y_cell
 */

#define BOMBS_FONT wxFont(14, wxROMAN, wxNORMAL, wxNORMAL)

#endif /* mutual exclusion */

