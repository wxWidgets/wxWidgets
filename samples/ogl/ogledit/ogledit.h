/////////////////////////////////////////////////////////////////////////////
// Name:        ogledit.h
// Purpose:     OGL sample
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _OGLSAMPLE_OGLEDIT_H_
#define _OGLSAMPLE_OGLEDIT_H_

#include <wx/docview.h>

// Define a new application
class MyFrame;
class EditorToolPalette;
class MyApp: public wxApp
{
 public:
  MyFrame *frame;
  wxDocManager* myDocManager;

  MyApp(void);
  bool OnInit(void);
  int OnExit(void);

  // Palette stuff
  EditorToolPalette *CreatePalette(wxFrame *parent);
};

DECLARE_APP(MyApp)

// Define a new frame
class MyCanvas;
class MyFrame: public wxDocParentFrame
{
  DECLARE_CLASS(MyFrame)
 public:
  wxMenu *editMenu;
  
  MyCanvas *canvas;
  EditorToolPalette *palette;
  
  MyFrame(wxDocManager *manager, wxFrame *parent, const wxString& title,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxDEFAULT_FRAME_STYLE);

  MyCanvas *CreateCanvas(wxView *view, wxFrame *parent);
  void OnSize(wxSizeEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

extern MyFrame *GetMainFrame(void);

// Menu/undo/redo commands

#define OGLEDIT_CUT                         1
#define OGLEDIT_ADD_SHAPE                   2
#define OGLEDIT_ADD_LINE                    3
#define OGLEDIT_EDIT_LABEL                  4
#define OGLEDIT_CHANGE_BACKGROUND_COLOUR    5

#define OGLEDIT_ABOUT   100

#endif
    // _OGLSAMPLE_OGLEDIT_H_
