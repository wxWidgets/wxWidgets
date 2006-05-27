/////////////////////////////////////////////////////////////////////////////
// Name:        power.cpp
// Purpose:     wxWidgets power management sample
// Author:      Vadim Zeitlin
// Created:     2006-05-27
// RCS-ID:      $Id$
// Copyright:   (C) 2006 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
#endif

#include "wx/textctrl.h"
#include "wx/msgdlg.h"

#include "wx/power.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// main frame class
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame()
        : wxFrame(NULL, wxID_ANY, _T("wxWidgets Power Management Sample"),
                  wxDefaultPosition, wxSize(500, 200))
    {
        wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, _T(""),
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTE_MULTILINE | wxTE_READONLY);
        m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(text));

        SetIcon(wxICON(sample));

        Show();
    }

    virtual ~MyFrame()
    {
        delete wxLog::SetActiveTarget(m_logOld);
    }

private:
    void OnSuspending(wxPowerEvent& event)
    {
        wxLogMessage(_T("System suspend starting..."));
        if ( wxMessageBox(_T("Veto suspend?"), _T("Please answer"),
                          wxYES_NO, this) == wxYES )
        {
            event.Veto();
            wxLogMessage(_T("Vetoed suspend."));
        }
    }

    void OnSuspended(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(_T("System is going to suspend."));
    }

    void OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(_T("System suspend was cancelled."));
    }

    void OnResume(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(_T("System resumed from suspend."));
    }


    wxLog *m_logOld;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
    EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
    EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
    EVT_POWER_RESUME(MyFrame::OnResume)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main application class
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        new MyFrame;

        return true;
    }
};

IMPLEMENT_APP(MyApp)
