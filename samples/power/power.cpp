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
    #include "wx/log.h"
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
        : wxFrame(NULL, wxID_ANY, wxT("wxWidgets Power Management Sample"),
                  wxDefaultPosition, wxSize(500, 200))
    {
        wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxT(""),
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
        wxLogMessage(wxT("System suspend starting..."));
        if ( wxMessageBox(wxT("Veto suspend?"), wxT("Please answer"),
                          wxYES_NO, this) == wxYES )
        {
            event.Veto();
            wxLogMessage(wxT("Vetoed suspend."));
        }
    }

    void OnSuspended(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(wxT("System is going to suspend."));
    }

    void OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(wxT("System suspend was cancelled."));
    }

    void OnResume(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage(wxT("System resumed from suspend."));
    }
#endif // wxHAS_POWER_EVENTS


    void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState)
    {
        wxString powerStr;
        switch ( m_powerType = powerType )
        {
            case wxPOWER_SOCKET:
                powerStr = wxT("wall");
                break;

            case wxPOWER_BATTERY:
                powerStr = wxT("battery");
                break;

            default:
                wxFAIL_MSG(wxT("unknown wxPowerType value"));
                // fall through

            case wxPOWER_UNKNOWN:
                powerStr = wxT("psychic");
                break;
        }

        wxString batteryStr;
        switch ( m_batteryState = batteryState )
        {
            case wxBATTERY_NORMAL_STATE:
                batteryStr = wxT("charged");
                break;

            case wxBATTERY_LOW_STATE:
                batteryStr = wxT("low");
                break;

            case wxBATTERY_CRITICAL_STATE:
                batteryStr = wxT("critical");
                break;

            case wxBATTERY_SHUTDOWN_STATE:
                batteryStr = wxT("empty");
                break;

            default:
                wxFAIL_MSG(wxT("unknown wxBatteryState value"));
                // fall through

            case wxBATTERY_UNKNOWN_STATE:
                batteryStr = wxT("unknown");
                break;
        }

        SetStatusText(wxString::Format(
                        wxT("System is on %s power, battery state is %s"),
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
