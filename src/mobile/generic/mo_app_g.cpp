/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/mo_app_g.cpp
// Purpose:     Special application class for use on iPhone or
//              iPhone simulator
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/image.h"

#include "wx/mobile/generic/app.h"
#include "wx/mobile/generic/simulator.h"
#include "wx/mobile/generic/keyboard.h"

IMPLEMENT_CLASS( wxMoApp, wxApp )

BEGIN_EVENT_TABLE( wxMoApp, wxApp )
    EVT_IDLE(wxMoApp::OnIdle)
END_EVENT_TABLE()

void wxMoApp::Init()
{
    m_doneInitialUpdate = false;
}

// Initialises the application
bool wxMoApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);

    if (!wxMoSimulatorFrame::GetSimulatorFrame())
    {
        wxMoSimulatorFrame::SetSimulatorFrame(new wxMoSimulatorFrame(NULL));
        wxMoSimulatorFrame::GetSimulatorFrame()->Show(true);
    }

    return true;
}

// Called on exit
int wxMoApp::OnExit()
{
    if (wxMoSimulatorFrame::GetSimulatorFrame())
        wxMoSimulatorFrame::GetSimulatorFrame()->Close();

    return 0;
}

// Necessary after creating app frame - do it in idle time.
void wxMoApp::UpdateLayout()
{
    if (wxMoSimulatorFrame::GetSimulatorFrame())
    {
        wxMoSimulatorFrame::GetSimulatorFrame()->Layout();
        wxMoSimulatorFrame::GetSimulatorFrame()->GetSimulatorPanel()->DoLayout();

        wxMoSimulatorFrame::GetSimulatorFrame()->Refresh();
    }
}

void wxMoApp::OnIdle(wxIdleEvent& event)
{
    if (!m_doneInitialUpdate)
    {
        m_doneInitialUpdate = true;
        UpdateLayout();
    }

    event.Skip();
}

void wxMoApp::SetTopWindow(wxWindow *WXUNUSED(win))
{
    // Do nothing: we don't want a wxMoFrame as a true top-level window.
}
