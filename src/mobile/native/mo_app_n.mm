/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/mo_app_n.mm
// Purpose:     Special application class for use on iPhone or
//              iPhone simulator
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"
#include "wx/image.h"

#include "wx/mobile/native/app.h"
#include "wx/mobile/native/keyboard.h"

IMPLEMENT_CLASS( wxMoApp, wxApp )

BEGIN_EVENT_TABLE( wxMoApp, wxApp )
    EVT_IDLE(wxMoApp::OnIdle)
END_EVENT_TABLE()

void wxMoApp::Init()
{
    // FIXME stub
}

// Initialises the application
bool wxMoApp::OnInit()
{
    // FIXME stub

    return true;
}

// Called on exit
int wxMoApp::OnExit()
{
    // FIXME stub

    return 0;
}

// Necessary after creating app frame - do it in idle time.
void wxMoApp::UpdateLayout()
{
    // FIXME stub
}

void wxMoApp::OnIdle(wxIdleEvent& event)
{
    // FIXME stub
}

void wxMoApp::SetTopWindow(wxWindow *WXUNUSED(win))
{
    // Do nothing: we don't want a wxMoFrame as a true top-level window.
}
