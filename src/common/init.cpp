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
    #include "wx/log.h"
    #include "wx/thread.h"
#endif

#include "wx/init.h"

#include "wx/ptr_scpd.h"
#include "wx/module.h"

#if defined(__WXMSW__) && defined(__WXDEBUG__)
    #include "wx/msw/msvcrt.h"

    static struct EnableMemLeakChecking
    {
        EnableMemLeakChecking()
        {
            // do check for memory leaks on program exit (another useful flag
            // is _CRTDBG_DELAY_FREE_MEM_DF which doesn't free deallocated
            // memory which may be used to simulate low-memory condition)
            wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
        }
    } gs_enableLeakChecks;
#endif // __WXMSW__ && __WXDEBUG__

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// we need a dummy app object if the user doesn't want to create a real one
class wxDummyConsoleApp : public wxAppConsole
{
public:
    virtual int OnRun() { wxFAIL_MSG( _T("unreachable code") ); return 0; }
};

// we need a special kind of auto pointer to wxApp which not only deletes the
// pointer it holds in its dtor but also resets wxTheApp
wxDECLARE_SCOPED_PTR(wxApp, wxAppPtrBase);
wxDEFINE_SCOPED_PTR(wxApp, wxAppPtrBase);

class wxAppPtr : public wxAppPtrBase
{
public:
    wxEXPLICIT wxAppPtr(wxApp *ptr = NULL) : wxAppPtrBase(ptr) { }
    ~wxAppPtr()
    {
        if ( get() )
        {
            // the pointer is going to be deleted in the base class dtor, don't
            // leave the dangling pointer!
            wxTheApp = NULL;
        }
    }

    void Set(wxApp *ptr)
    {
        reset(ptr);

        wxTheApp = ptr;
    }
};

// class to ensure that wxAppBase::CleanUp() is called if our Initialize()
// fails
class wxCallAppCleanup
{
public:
    wxCallAppCleanup(wxApp *app) : m_app(app) { }
    ~wxCallAppCleanup() { if ( m_app ) m_app->CleanUp(); }

    void Dismiss() { m_app = NULL; }

private:
    wxApp *m_app;
};

// another tiny class which simply exists to ensure that wxEntryCleanup is
// always called
class wxCleanupOnExit
{
public:
    ~wxCleanupOnExit() { wxEntryCleanup(); }
};

// ----------------------------------------------------------------------------
// initialization data
// ----------------------------------------------------------------------------

static struct InitData
{
    InitData()
    {
        nInitCount = 0;

#if wxUSE_UNICODE
        argc = 0;
        // argv = NULL; -- not even really needed
#endif // wxUSE_UNICODE
    }

    // critical section protecting this struct
    wxCRIT_SECT_DECLARE_MEMBER(csInit);

    // number of times wxInitialize() was called minus the number of times
    // wxUninitialize() was
    size_t nInitCount;

#if wxUSE_UNICODE
    int argc;

    // if we receive the command line arguments as ASCII and have to convert
    // them to Unicode ourselves (this is the case under Unix but not Windows,
    // for example), we remember the converted argv here because we'll have to
    // free it when doing cleanup to avoid memory leaks
    wchar_t *argv;
#endif // wxUSE_UNICODE
} gs_initData;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// command line arguments ANSI -> Unicode conversion
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE

static void ConvertArgsToUnicode(int argc, char **argv)
{
    gs_initData.argv = new wchar_t *[argc + 1];
    for ( int i = 0; i < argc; i++ )
    {
        gs_initData.argv[i] = wxStrdup(wxConvLocal.cMB2WX(argv[i]));
    }

    gs_initData.argv[argc] = NULL;
}

static void FreeConvertedArgs()
{
    for ( int mb_argc = 0; mb_argc < wxTheApp->argc; mb_argc++ )
    {
        free(wxTheApp->argv[mb_argc]);
    }
}

#endif // wxUSE_UNICODE

// ----------------------------------------------------------------------------
// start up
// ----------------------------------------------------------------------------

// initialization which is always done (not customizable) before wxApp creation
static bool DoCommonPreInit()
{
    wxClassInfo::InitializeClasses();

    return true;
}

// non customizable initialization done after wxApp creation and initialization
static bool DoCommonPostInit()
{
    wxModule::RegisterModules();

    return wxModule::InitializeModules();
}

bool wxEntryStart(int& argc, wxChar **argv)
{
    // do minimal, always necessary, initialization
    // --------------------------------------------

    // initialize wxRTTI
    if ( !DoCommonPreInit() )
    {
        return false;
    }


    // first of all, we need an application object
    // -------------------------------------------

    // the user might have already created it himself somehow
    wxAppPtr app(wxTheApp);
    if ( !app.get() )
    {
        // if not, he might have used IMPLEMENT_APP() to give us a function to
        // create it
        wxAppInitializerFunction fnCreate = wxApp::GetInitializerFunction();

        if ( fnCreate )
        {
            // he did, try to create the custom wxApp object
            app.Set((*fnCreate)());
        }
    }

    if ( !app.get() )
    {
        // either IMPLEMENT_APP() was not used at all or it failed -- in any
        // case we still need something
        //
        // NB: cast is needed because for the backwards-compatibility reasons
        //     wxTheApp is really a wxApp and not just wxAppConsole...
        app.Set((wxApp *)new wxDummyConsoleApp);
    }


    // wxApp initialization: this can be customized
    // --------------------------------------------

    if ( !wxTheApp->Initialize(argc, argv) )
    {
        return false;
    }

    wxCallAppCleanup callAppCleanup(wxTheApp);

    // for compatibility call the old initialization function too
    if ( !wxTheApp->OnInitGui() )
        return false;


    // common initialization after wxTheApp creation
    // ---------------------------------------------

    if ( !DoCommonPostInit() )
        return false;


    // prevent the smart pointer from destroying its contents
    app.release();

    // and the cleanup object from doing cleanup
    callAppCleanup.Dismiss();

    return true;
}

#if wxUSE_UNICODE

// we provide a wxEntryStart() wrapper taking "char *" pointer too
bool wxEntryStart(int& argc, char **argv)
{
    ConvertArgsToUnicode(argc, argv);

    if ( !wxEntryStart(argc, gs_initData.argv) )
    {
        FreeConvertedArgs();

        return false;
    }

    return true;
}

#endif // wxUSE_UNICODE

// ----------------------------------------------------------------------------
// clean up
// ----------------------------------------------------------------------------

// cleanup done before destroying wxTheApp 
static void DoCommonPreCleanup()
{
#if wxUSE_LOG
    // flush the logged messages if any and install a 'safer' log target: the
    // default one (wxLogGui) can't be used after the resources are freed just
    // below and the user supplied one might be even more unsafe (using any
    // wxWindows GUI function is unsafe starting from now)
    wxLog::DontCreateOnDemand();

    // this will flush the old messages if any
    delete wxLog::SetActiveTarget(new wxLogStderr);
#endif // wxUSE_LOG

    wxModule::CleanUpModules();
}

// cleanup done after destroying wxTheApp
static void DoCommonPostCleanup()
{
    wxClassInfo::CleanUpClasses();

    // we can't do this in wxApp itself because it doesn't know if argv had
    // been allocated
#if wxUSE_UNICODE
    FreeConvertedArgs();
#endif // wxUSE_UNICODE

#if wxUSE_LOG
    // and now delete the last logger as well
    delete wxLog::SetActiveTarget(NULL);
#endif // wxUSE_LOG
}

void wxEntryCleanup()
{
    DoCommonPreCleanup();


    // delete the application object
    if ( wxTheApp )
    {
        wxTheApp->CleanUp();

        delete wxTheApp;
        wxTheApp = NULL;
    }


    DoCommonPostCleanup();

    // check for memory leaks
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
    if (wxDebugContext::CountObjectsLeft(TRUE) > 0)
    {
        wxLogDebug(wxT("There were memory leaks.\n"));
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
    }
#endif // Debug

}

// ----------------------------------------------------------------------------
// wxEntry
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) || !wxUSE_ON_FATAL_EXCEPTION
    #define wxEntryReal wxEntry
#endif // !(__WXMSW__ && wxUSE_ON_FATAL_EXCEPTION)

int wxEntryReal(int& argc, wxChar **argv)
{
    // library initialization
    if ( !wxEntryStart(argc, argv) )
    {
        return -1;
    }

    // if wxEntryStart succeeded, we must call wxEntryCleanup even if the code
    // below returns or throws
    wxCleanupOnExit cleanupOnExit;

    // app initialization
    if ( !wxTheApp->OnInit() )
    {
        // don't call OnExit() if OnInit() failed
        return -1;
    }

    // app execution
    int retValue = wxTheApp->OnRun();

    // why should we do this? it doesn't close all window, just one of them and
    // this shouldn't be necessary anyhow...
#if 0
    // close any remaining windows
    wxWindow *topWindow = wxTheApp->GetTopWindow();
    if ( topWindow )
    {
        // forcibly delete the window.
        topWindow->Destroy();

        // collect the dead objects
        wxTheApp->DeletePendingObjects();
    }
#endif // 0

    // app clean up
    wxTheApp->OnExit();

    return retValue;
}

// wrap real wxEntry in a try-except block to be able to call
// OnFatalException() if necessary
#if defined(__WXMSW__) && wxUSE_ON_FATAL_EXCEPTION

extern unsigned long wxGlobalSEHandler();

int wxEntry(int& argc, wxChar **argv)
{
    __try
    {
        return wxEntryReal(argc, argv);
    }
    __except ( wxGlobalSEHandler() )
    {
        ::ExitProcess(3); // the same exit code as abort()

        // this code is unreachable but put it here to suppress warnings
        return -1;
    }
}

#endif // __WXMSW__ && wxUSE_ON_FATAL_EXCEPTION

#if wxUSE_UNICODE

// as with wxEntryStart, we provide an ANSI wrapper
int wxEntry(int& argc, char **argv)
{
    ConvertArgsToUnicode(argc, argv);

    return wxEntry(argc, gs_initData.argv);
}

#endif // wxUSE_UNICODE

// ----------------------------------------------------------------------------
// wxInitialize/wxUninitialize
// ----------------------------------------------------------------------------

bool wxInitialize(int argc, wxChar **argv)
{
    wxCRIT_SECT_LOCKER(lockInit, gs_initData.csInit);

    if ( gs_initData.nInitCount++ )
    {
        // already initialized
        return true;
    }

    return wxEntryStart(argc, argv);
}

void wxUninitialize()
{
    wxCRIT_SECT_LOCKER(lockInit, gs_initData.csInit);

    if ( !--gs_initData.nInitCount )
    {
        wxEntryCleanup();
    }
}

