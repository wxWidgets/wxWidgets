/////////////////////////////////////////////////////////////////////////////
// Name:        wx_exe.cpp
// Purpose:     Sample showing how to use wx from a DLL
// Author:      Vaclav Slavik
// Created:     2009-12-03
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "my_dll.h"

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"

#ifndef __WXMSW__
    #error "This sample is MSW-only"
#endif

#ifdef WXUSINGDLL
    #error "This sample doesn't work with DLL build of wxWidgets"
#endif

// ----------------------------------------------------------------------------
// GUI classes
// ----------------------------------------------------------------------------

static const int ID_RUN_DLL = wxNewId();

class MainFrame : public wxFrame
{
public:
    MainFrame();

    void OnRunDLL(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};


class MainApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MainFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_RUN_DLL, MainFrame::OnRunDLL)
END_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "Main wx app",
              wxDefaultPosition, wxSize(400, 300))
{
    wxPanel *p = new wxPanel(this, wxID_ANY);
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add
           (
               new wxStaticText
                   (
                       p, wxID_ANY,
                       wxString::Format
                       (
                           "Main wxApp instance is %p (%s),\n"
                           "thread ID %ld.\n",
                           wxApp::GetInstance(),
                           wxVERSION_STRING,
                           wxThread::GetCurrentId()
                       )
                   ),
               wxSizerFlags(1).Expand().Border(wxALL, 10)
           );

    sizer->Add
           (
               new wxButton(p, ID_RUN_DLL, "Run GUI from DLL"),
               wxSizerFlags(0).Right().Border(wxALL, 10)
           );

    p->SetSizerAndFit(sizer);

    wxSizer *fsizer = new wxBoxSizer(wxVERTICAL);
    fsizer->Add(p, wxSizerFlags(1).Expand());
    SetSizerAndFit(fsizer);
}

void MainFrame::OnRunDLL(wxCommandEvent& WXUNUSED(event))
{
    run_wx_gui_from_dll("child instance");
}


// ----------------------------------------------------------------------------
// MainApp
// ----------------------------------------------------------------------------

bool MainApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame *f = new MainFrame();
    f->Show(true);

    return true;
}

int MainApp::OnExit()
{
    wx_dll_cleanup();
    return wxApp::OnExit();
}


IMPLEMENT_APP(MainApp)
