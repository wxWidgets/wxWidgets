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

        CreateStatusBar();

        SetIcon(wxICON(sample));

        UpdatePowerSettings(wxPOWER_UNKNOWN, wxBATTERY_UNKNOWN_STATE);

        Show();
    }

    virtual ~MyFrame()
    {
        delete wxLog::SetActiveTarget(m_logOld);
    }

private:
    void OnIdle(wxIdleEvent& WXUNUSED(event))
    {
        const wxPowerType powerType = wxGetPowerType();
        const wxBatteryState batteryState = wxGetBatteryState();
        if ( powerType != m_powerType || batteryState != m_batteryState )
        {
            UpdatePowerSettings(powerType, batteryState);
        }
    }

#ifdef wxHAS_POWER_EVENTS
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
#endif // wxHAS_POWER_EVENTS


    void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState)
    {
        wxString powerStr;
        switch ( m_powerType = powerType )
        {
            case wxPOWER_SOCKET:
                powerStr = _T("wall");
                break;

            case wxPOWER_BATTERY:
                powerStr = _T("battery");
                break;

            default:
                wxFAIL_MSG(_T("unknown wxPowerType value"));
                // fall through

            case wxPOWER_UNKNOWN:
                powerStr = _T("psychic");
                break;
        }

        wxString batteryStr;
        switch ( m_batteryState = batteryState )
        {
            case wxBATTERY_NORMAL_STATE:
                batteryStr = _T("charged");
                break;

            case wxBATTERY_LOW_STATE:
                batteryStr = _T("low");
                break;

            case wxBATTERY_CRITICAL_STATE:
                batteryStr = _T("critical");
                break;

            case wxBATTERY_SHUTDOWN_STATE:
                batteryStr = _T("empty");
                break;

            default:
                wxFAIL_MSG(_T("unknown wxBatteryState value"));
                // fall through

            case wxBATTERY_UNKNOWN_STATE:
                batteryStr = _T("unknown");
                break;
        }

        SetStatusText(wxString::Format(
                        _T("System is on %s power, battery state is %s"),
                        powerStr.c_str(),
                        batteryStr.c_str()));
    }

    wxPowerType m_powerType;
    wxBatteryState m_batteryState;

    wxLog *m_logOld;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_IDLE(MyFrame::OnIdle)

#ifdef wxHAS_POWER_EVENTS
    EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
    EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
    EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
    EVT_POWER_RESUME(MyFrame::OnResume)
#endif // wxHAS_POWER_EVENTS
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
