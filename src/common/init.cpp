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

#include "wx/app.h"
#include "wx/debug.h"

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

wxApp * WXDLLEXPORT wxTheApp = NULL;

wxAppInitializerFunction
    wxAppBase::m_appInitFn = (wxAppInitializerFunction)NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class /* no WXDLLEXPORT */ wxConsoleApp : public wxApp
{
public:
    virtual int OnRun() { wxFAIL_MSG(T("unreachable")); return 0; }
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
    if ( gs_nInitCount++ )
    {
        // already initialized
        return TRUE;
    }

    wxASSERT_MSG( !wxTheApp,
                  T("either call wxInitialize or create app, not both!") );

    wxTheApp = new wxConsoleApp;

    return wxTheApp != NULL;
}

void WXDLLEXPORT wxUninitialize()
{
    if ( !--gs_nInitCount )
    {
        // delete the application object
        delete wxTheApp;
        wxTheApp = (wxApp *)NULL;
    }
}
