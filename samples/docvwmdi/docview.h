/////////////////////////////////////////////////////////////////////////////
// Name:        docview.h
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DOCVIEW_H__
#define __DOCVIEW_H__

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"

class wxDocManager;

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp(void);
    virtual bool OnInit();
    virtual int OnExit();

    wxMDIChildFrame* CreateChildFrame(wxDocument*, wxView*, bool isCanvas);

protected:
    wxDocManager* m_docManager;
};

DECLARE_APP(MyApp)

// Define a new frame
class MyCanvas;
class DrawingView;
class MyFrame: public wxDocMDIParentFrame
{
public:
    wxMenu* m_editMenu;

    MyFrame(wxDocManager*, wxFrame*, const wxString& title, const wxPoint&, const wxSize&, long type);

    MyCanvas* CreateCanvas(DrawingView*, wxMDIChildFrame*);

protected:
    void OnAbout(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(MyFrame)
};

extern MyFrame* GetMainFrame();

#define DOCVIEW_CUT     1
#define DOCVIEW_ABOUT   wxID_ABOUT

extern bool singleWindowMode;

#endif
