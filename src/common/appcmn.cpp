/////////////////////////////////////////////////////////////////////////////
// Name:        common/appcmn.cpp
// Purpose:     wxAppBase methods common to all platforms
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "appbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/list.h"
#endif

#include "wx/thread.h"
#include "wx/confbase.h"

#ifdef __WXUNIVERSAL__
    #include "wx/univ/theme.h"
#endif // __WXUNIVERSAL__

// ===========================================================================
// implementation
// ===========================================================================

wxAppBase::wxAppBase()
{
    wxTheApp = (wxApp *)this;

    // VZ: what's this? is it obsolete?
    m_wantDebugOutput = FALSE;

#if wxUSE_GUI
    m_topWindow = (wxWindow *)NULL;
    m_useBestVisual = FALSE;
    m_exitOnFrameDelete = TRUE;
    m_isActive = TRUE;
#endif // wxUSE_GUI
}

// ----------------------------------------------------------------------------
// initialization and termination
// ----------------------------------------------------------------------------

bool wxAppBase::OnInitGui()
{
#ifdef __WXUNIVERSAL__
    if ( !wxTheme::CreateDefault() )
        return FALSE;
#endif // __WXUNIVERSAL__

    return TRUE;
}

int wxAppBase::OnExit()
{
#if wxUSE_CONFIG
    // delete the config object if any (don't use Get() here, but Set()
    // because Get() could create a new config object)
    delete wxConfigBase::Set((wxConfigBase *) NULL);
#endif // wxUSE_CONFIG

#ifdef __WXUNIVERSAL__
    delete wxTheme::Set(NULL);
#endif // __WXUNIVERSAL__

    return 0;
}

// ---------------------------------------------------------------------------
// wxAppBase
// ----------------------------------------------------------------------------

void wxAppBase::ProcessPendingEvents()
{
    // ensure that we're the only thread to modify the pending events list
    wxENTER_CRIT_SECT( *wxPendingEventsLocker );

    if ( !wxPendingEvents )
    {
        wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
        return;
    }

    // iterate until the list becomes empty
    wxNode *node = wxPendingEvents->First();
    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->Data();
        delete node;

        // In ProcessPendingEvents(), new handlers might be add
        // and we can safely leave the critical section here.
        wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
        handler->ProcessPendingEvents();
        wxENTER_CRIT_SECT( *wxPendingEventsLocker );

        node = wxPendingEvents->First();
    }

    wxLEAVE_CRIT_SECT( *wxPendingEventsLocker );
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

#if wxUSE_GUI

void wxAppBase::SetActive(bool active, wxWindow *lastFocus)
{
    static wxWindow *s_lastFocus = (wxWindow *)NULL;

    m_isActive = active;

    // if we're being deactivated remember the last focused window
    if ( !active )
    {
        s_lastFocus = lastFocus;
    }

    if ( s_lastFocus )
    {
        // give the focused window the chance to refresh itself if its
        // appearance depends on the app activation state
        wxActivateEvent event(wxEVT_ACTIVATE, active);
        s_lastFocus->GetEventHandler()->ProcessEvent(event);
    }
}

#endif // wxUSE_GUI
