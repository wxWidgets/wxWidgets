/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface "docview.h"
#endif

#ifndef __DOCVIEWSAMPLEH__
#define __DOCVIEWSAMPLEH__

#include "wx/docmdi.h"

class wxDocManager;

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void);
    bool OnInit(void);
    int OnExit(void);

    wxFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas);

  protected:
    wxDocManager* m_docManager;
};

DECLARE_APP(MyApp)

// Define a new frame
class MyCanvas;
class MyFrame: public wxDocMDIParentFrame
{
  DECLARE_CLASS(MyFrame)
 public:
  wxMenu *editMenu;
  
  MyFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
    const long type);

  void OnAbout(wxCommandEvent& event);
  MyCanvas *CreateCanvas(wxView *view, wxFrame *parent);

DECLARE_EVENT_TABLE()
};

extern MyFrame *GetMainFrame(void);

#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   2

extern bool singleWindowMode;

#endif
