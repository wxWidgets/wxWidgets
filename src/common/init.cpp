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

#include "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/debug.h"
    #include "wx/filefn.h"
#endif

#include "wx/module.h"

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
    virtual int OnRun() { wxFAIL_MSG(wxT("unreachable")); return 0; }
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static bool DoInit();
static void DoCleanUp();

// ----------------------------------------------------------------------------
// private vars
// ----------------------------------------------------------------------------

static size_t gs_nInitCount = 0;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// stubs for some GUI functions
// ----------------------------------------------------------------------------

void WXDLLEXPORT wxExit()
{
    abort();
}

// Yield to other apps/messages
bool WXDLLEXPORT wxYield()
{
    // do nothing
    return TRUE;
}

// Yield to other apps/messages
void WXDLLEXPORT wxWakeUpIdle()
{
    // do nothing
}

// ----------------------------------------------------------------------------
// wxBase-specific functions
// ----------------------------------------------------------------------------

bool WXDLLEXPORT wxInitialize()
{
    if ( gs_nInitCount )
    {
        // already initialized
        return TRUE;
    }

    wxASSERT_MSG( !wxTheApp,
                  wxT("either call wxInitialize or create app, not both!") );

    if ( !DoInit() )
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
        DoCleanUp();
    }
}

int wxEntry(int argc, char **argv)
{
    // library initialization
    if ( !DoInit() )
    {
        return -1;
    }

    // create the app
    if ( !wxTheApp )
    {
        wxCHECK_MSG( wxApp::GetInitializerFunction(), -1,
                wxT("No application object: use IMPLEMENT_APP macro.") );

        wxAppInitializerFunction fnCreate = wxApp::GetInitializerFunction();

        wxTheApp = (wxApp *)fnCreate();
    }

    wxCHECK_MSG( wxTheApp, -1, wxT("wxWindows error: no application object") );

    // app preinitialization
    wxTheApp->argc = argc;

#if wxUSE_UNICODE
    wxTheApp->argv = new wxChar*[argc+1];
    int mb_argc = 0;
    while (mb_argc < argc)
    {
        wxTheApp->argv[mb_argc] = wxStrdup(wxConvLibc.cMB2WX(argv[mb_argc]));
        mb_argc++;
    }
    wxTheApp->argv[mb_argc] = (wxChar *)NULL;
#else
    wxTheApp->argv = argv;
#endif

    wxString name = wxFileNameFromPath(argv[0]);
    wxStripExtension(name);
    wxTheApp->SetAppName(name);

    int retValue = 0;

    // app initialization
    if ( !wxTheApp->OnInit() )
        retValue = -1;

    // app execution
    if ( retValue == 0 )
    {
        retValue = wxTheApp->OnRun();

        // app clean up
        wxTheApp->OnExit();
    }

    // library clean up
    DoCleanUp();

    return retValue;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static bool DoInit()
{
    wxClassInfo::InitializeClasses();

    wxModule::RegisterModules();
    if ( !wxModule::InitializeModules() )
    {
        return FALSE;
    }

    return TRUE;
}

static void DoCleanUp()
{
#if wxUSE_LOG
    // flush the logged messages if any
    wxLog *log = wxLog::GetActiveTarget();
    if (log != NULL && log->HasPendingMessages())
        log->Flush();

    // continuing to use user defined log target is unsafe from now on because
    // some resources may be already unavailable, so replace it by something
    // more safe
    wxLog::DontCreateOnDemand();
    delete wxLog::SetActiveTarget(new wxLogStderr);
#endif // wxUSE_LOG

    wxModule::CleanUpModules();

    wxClassInfo::CleanUpClasses();

    // delete the application object
    delete wxTheApp;
    wxTheApp = (wxApp *)NULL;

#if wxUSE_LOG
    // and now delete the last logger as well
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
}
