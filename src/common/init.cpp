/////////////////////////////////////////////////////////////////////////////
// Name:        common/init.cpp
// Purpose:     initialisation for the library
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "appbase.h"
#endif

#include "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__


#include "wx/app.h"
#include "wx/debug.h"

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

wxApp * WXDLLEXPORT wxTheApp = NULL;

wxAppInitializerFunction
    wxAppBase::m_appInitFn = (wxAppInitializerFunction)NULL;

#if wxUSE_THREADS
    // List of events pending processing
    wxList *wxPendingEvents = NULL;
    wxCriticalSection *wxPendingEventsLocker = NULL;
#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class /* no WXDLLEXPORT */ wxConsoleApp : public wxApp
{
public:
    virtual int OnRun() { wxFAIL_MSG(wxT("unreachable")); return 0; }
};

// ----------------------------------------------------------------------------
// private vars
// ----------------------------------------------------------------------------

static size_t gs_nInitCount = 0;

// ============================================================================
// implementation
// ============================================================================

bool WXDLLEXPORT wxInitialize()
{
    if ( gs_nInitCount )
    {
        // already initialized
        return TRUE;
    }

    wxASSERT_MSG( !wxTheApp,
                  wxT("either call wxInitialize or create app, not both!") );

    wxClassInfo::InitializeClasses();

    wxModule::RegisterModules();
    if ( !wxModule::InitializeModules() )
    {
        return FALSE;
    }

    wxTheApp = new wxConsoleApp;

    if ( !wxTheApp )
    {
        return FALSE;
    }

    gs_nInitCount++;

    return TRUE;
}

void WXDLLEXPORT wxUninitialize()
{
    if ( !--gs_nInitCount )
    {
        wxModule::CleanUpModules();

        wxClassInfo::CleanUpClasses();

        // delete the application object
        delete wxTheApp;
        wxTheApp = (wxApp *)NULL;
    }
}
