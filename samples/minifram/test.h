/////////////////////////////////////////////////////////////////////////////
// Name:        test.h
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     23/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/minifram.h"

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit(void);
    bool InitToolbar(wxToolBar* toolBar);
};

// Define a new frame
class MyFrame: public wxMiniFrame
{
public:
    MyFrame(wxFrame *parent, wxWindowID id = -1, const wxString& title = "wxToolBar Sample",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );

    void OnCloseWindow(wxCloseEvent& event);
    
DECLARE_EVENT_TABLE()
};

#define ID_TOOLBAR  500

