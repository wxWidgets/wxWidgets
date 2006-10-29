/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.h
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     23/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/minifram.h"

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit(void);
    bool InitToolbar(wxToolBar* toolBar);
};

// Define a new mini frame
class MyMiniFrame: public wxMiniFrame
{
public:
    MyMiniFrame(wxFrame *parent, wxWindowID id = wxID_ANY, const wxString& title = _T("wxToolBar Sample"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );

    void OnCloseWindow(wxCloseEvent& event);
    void OnReparent(wxCommandEvent& event);
    
DECLARE_EVENT_TABLE()
};

// Define a new frame
class MyMainFrame: public wxFrame
{
public:
    MyMainFrame(wxFrame *parent, wxWindowID id = wxID_ANY, const wxString& title = _T("wxToolBar Sample"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );

    void OnCloseWindow(wxCloseEvent& event);
    void OnReparent(wxCommandEvent& event);
    void OnSetSize_150_150(wxCommandEvent &event);
    void OnSetSize_200_200(wxCommandEvent &event);
    void OnSetMaxSize_150_150(wxCommandEvent &event);
    void OnSetMaxSize_300_300(wxCommandEvent &event);
    
DECLARE_EVENT_TABLE()
};

#define ID_TOOLBAR   500
#define ID_REPARENT  501

