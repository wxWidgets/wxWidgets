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

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxAppBase
// ----------------------------------------------------------------------------

void wxAppBase::ProcessPendingEvents()
{
    // ensure that we're the only thread to modify the pending events list
    wxCRIT_SECT_LOCKER(locker, wxPendingEventsLocker);

    if ( !wxPendingEvents )
        return;

    // iterate until the list becomes empty
    wxNode *node = wxPendingEvents->First();
    while (node)
    {
        wxEvtHandler *handler = (wxEvtHandler *)node->Data();

        handler->ProcessPendingEvents();

        delete node;
        node = wxPendingEvents->First();
    }
}

int wxAppBase::OnExit()
{
#if wxUSE_CONFIG
    // delete the config object if any (don't use Get() here, but Set()
    // because Get() could create a new config object)
    delete wxConfigBase::Set((wxConfigBase *) NULL);
#endif // wxUSE_CONFIG

    return 0;
}
