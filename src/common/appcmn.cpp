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
#endif

#include "wx/thread.h"

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// some global data defined here
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule, wxModule)

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

