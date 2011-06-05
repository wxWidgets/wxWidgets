/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/app.h
// Purpose:     Special application class for use on iPhone or
//              iPhone simulator
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:      $Id$
// Copyright:   Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_APP_H_
#define _WX_MOBILE_GENERIC_APP_H_

#include "wx/app.h"

class wxMoSimulatorFrame;

/**
    @class wxMoApp

    Use this as the base class for your application class, in place of wxApp.

    You need to remember to call wxMoApp::OnInit at the beginning of your
    OnInit function, and wxMoApp::OnExit at the end of your OnExit function.
    Otherwise, the simulator will not be set up and cleaned up properly.

    From within your OnInit function, create a wxMoFrame for your top-level frame.

    @category{wxMobile}
 */

class wxMoApp: public wxApp
{
    DECLARE_CLASS( wxMoApp )
    DECLARE_EVENT_TABLE( )

public:
    /// Constructor
    wxMoApp() { Init(); }

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

    /// Necessary after creating app frame
    virtual void UpdateLayout();

    // Stub this out since we don't want a wxMoFrame as a true top-level window.
    void SetTopWindow(wxWindow *win);

protected:
    void OnIdle(wxIdleEvent& event);

    bool    m_doneInitialUpdate;
};

#endif
    // _WX_MOBILE_GENERIC_APP_H_

