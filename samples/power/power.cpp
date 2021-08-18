/////////////////////////////////////////////////////////////////////////////
// Name:        power.cpp
// Purpose:     wxWidgets power management sample
// Author:      Vadim Zeitlin
// Created:     2006-05-27
// Copyright:   (C) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/log.h"
#endif

#include "wx/textctrl.h"
#include "wx/msgdlg.h"
#include "wx/menu.h"
#include "wx/timer.h"

#include "wx/power.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// main frame class
// ----------------------------------------------------------------------------

class MyFrame : public wxFrame
{
public:
    MyFrame()
        : wxFrame(NULL, wxID_ANY, "wxWidgets Power Management Sample",
                  wxDefaultPosition, wxSize(500, 200))
    {
        m_powerResourceBlocker = NULL;

        wxMenu *fileMenu = new wxMenu;
        fileMenu->Append(wxID_NEW, "Start long running task\tCtrl-S");
        fileMenu->Append(wxID_ABORT, "Stop long running task");

        wxMenuBar* menuBar = new wxMenuBar();
        menuBar->Append(fileMenu, "&Task");
        SetMenuBar(menuBar);

        wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, "",
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTE_MULTILINE | wxTE_READONLY);
        m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(text));

        CreateStatusBar();

        SetIcon(wxICON(sample));

        UpdatePowerSettings(wxPOWER_UNKNOWN, wxBATTERY_UNKNOWN_STATE);

        StopLongTask();
        Bind(wxEVT_COMMAND_MENU_SELECTED,
             &MyFrame::OnStartTaskClicked, this, wxID_NEW);
        Bind(wxEVT_COMMAND_MENU_SELECTED,
             &MyFrame::OnStopTaskClicked, this, wxID_ABORT);
        m_taskTimer.Bind(wxEVT_TIMER, &MyFrame::OnTaskTimer, this);

        Show();
    }

    virtual ~MyFrame()
    {
        delete m_powerResourceBlocker;

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
        wxLogMessage("System suspend starting...");
        if ( wxMessageBox("Veto suspend?", "Please answer",
                          wxYES_NO, this) == wxYES )
        {
            event.Veto();
            wxLogMessage("Vetoed suspend.");
        }
    }

    void OnSuspended(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage("System is going to suspend.");
    }

    void OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage("System suspend was cancelled.");
    }

    void OnResume(wxPowerEvent& WXUNUSED(event))
    {
        wxLogMessage("System resumed from suspend.");
    }
#endif // wxHAS_POWER_EVENTS


    void UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState)
    {
        wxString powerStr;
        switch ( m_powerType = powerType )
        {
            case wxPOWER_SOCKET:
                powerStr = "wall";
                break;

            case wxPOWER_BATTERY:
                powerStr = "battery";
                break;

            default:
                wxFAIL_MSG("unknown wxPowerType value");
                wxFALLTHROUGH;

            case wxPOWER_UNKNOWN:
                powerStr = "psychic";
                break;
        }

        wxString batteryStr;
        switch ( m_batteryState = batteryState )
        {
            case wxBATTERY_NORMAL_STATE:
                batteryStr = "charged";
                break;

            case wxBATTERY_LOW_STATE:
                batteryStr = "low";
                break;

            case wxBATTERY_CRITICAL_STATE:
                batteryStr = "critical";
                break;

            case wxBATTERY_SHUTDOWN_STATE:
                batteryStr = "empty";
                break;

            default:
                wxFAIL_MSG("unknown wxBatteryState value");
                wxFALLTHROUGH;

            case wxBATTERY_UNKNOWN_STATE:
                batteryStr = "unknown";
                break;
        }

        SetStatusText(wxString::Format(
                        "System is on %s power, battery state is %s",
                        powerStr,
                        batteryStr));
    }

    void OnStartTaskClicked( wxCommandEvent& WXUNUSED(event) )
    {
        wxLogMessage("Starting long running task "
                     "(screen should keep powered on while running)...");
        StartLongTask();
    }

    void OnStopTaskClicked(wxCommandEvent& WXUNUSED(event))
    {
        StopLongTask();
        wxLogMessage("Stopped long running task");
    }

    void OnTaskTimer(wxTimerEvent& WXUNUSED(event))
    {
        ++m_taskProgress;

        if ( m_taskProgress == 100 )
        {
            StopLongTask();
            wxLogMessage("Long running task finished");
        }
        else
        {
            wxLogMessage("Long running task at %d%%...", m_taskProgress);
        }
    }

    void StartLongTask()
    {
        m_taskProgress = 0;
        m_taskTimer.Start(12000);
        GetMenuBar()->Enable(wxID_NEW, false);
        GetMenuBar()->Enable(wxID_ABORT, true);

        m_powerResourceBlocker
            = new wxPowerResourceBlocker(wxPOWER_RESOURCE_SYSTEM);

        if ( !m_powerResourceBlocker->IsInEffect() )
        {
            wxLogMessage("Power resource could not be acquired, "
                         "user input is required to prevent system standby");
        }
    }

    void StopLongTask()
    {
        GetMenuBar()->Enable(wxID_NEW, true);
        GetMenuBar()->Enable(wxID_ABORT, false);
        m_taskTimer.Stop();

        wxDELETE(m_powerResourceBlocker);
    }

    wxPowerType m_powerType;
    wxBatteryState m_batteryState;

    wxLog *m_logOld;
    wxTimer m_taskTimer;
    wxPowerResourceBlocker *m_powerResourceBlocker;
    int m_taskProgress;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_IDLE(MyFrame::OnIdle)

#ifdef wxHAS_POWER_EVENTS
    EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
    EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
    EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
    EVT_POWER_RESUME(MyFrame::OnResume)
#endif // wxHAS_POWER_EVENTS
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// main application class
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE
    {
        new MyFrame;

        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
