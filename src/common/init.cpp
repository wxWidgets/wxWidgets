/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/init.cpp
// Purpose:     initialisation for the library
// Author:      Vadim Zeitlin
// Created:     04.10.99
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


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/filefn.h"
    #include "wx/log.h"
    #include "wx/module.h"
#endif

#include "wx/init.h"
#include "wx/atomic.h"

#include "wx/except.h"

#if defined(__WINDOWS__)
    #include "wx/msw/private.h"
    #include "wx/msw/msvcrt.h"

    #ifdef wxCrtSetDbgFlag
        static struct EnableMemLeakChecking
        {
            EnableMemLeakChecking()
            {
                // check for memory leaks on program exit (another useful flag
                // is _CRTDBG_DELAY_FREE_MEM_DF which doesn't free deallocated
                // memory which may be used to simulate low-memory condition)
                wxCrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
            }
        } gs_enableLeakChecks;
    #endif // wxCrtSetDbgFlag
#endif // __WINDOWS__

#include "wx/private/init.h"
#include "wx/private/localeset.h"

#include <memory>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// we need a dummy app object if the user doesn't want to create a real one
class wxDummyConsoleApp : public wxAppConsole
{
public:
    wxDummyConsoleApp() { }

    virtual int OnRun() override { wxFAIL_MSG( wxT("unreachable code") ); return 0; }

    wxDECLARE_NO_COPY_CLASS(wxDummyConsoleApp);
};

// we need a special kind of auto pointer to wxApp which not only deletes the
// pointer it holds in its dtor but also resets the global application pointer
class wxAppPtr : public std::unique_ptr<wxAppConsole>
{
public:
    explicit wxAppPtr(wxAppConsole *ptr) : std::unique_ptr<wxAppConsole>(ptr) { }
    ~wxAppPtr()
    {
        if ( get() )
        {
            // the pointer is going to be deleted in the base class dtor, don't
            // leave the dangling pointer!
            wxApp::SetInstance(nullptr);
        }
    }

    void Set(wxAppConsole *ptr)
    {
        reset(ptr);

        wxApp::SetInstance(ptr);
    }

    wxDECLARE_NO_COPY_CLASS(wxAppPtr);
};

// class to ensure that wxAppBase::CleanUp() is called if our Initialize()
// fails
class wxCallAppCleanup
{
public:
    wxCallAppCleanup(wxAppConsole *app) : m_app(app) { }
    ~wxCallAppCleanup() { if ( m_app ) m_app->CleanUp(); }

    void Dismiss() { m_app = nullptr; }

private:
    wxAppConsole *m_app;
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// suppress warnings about unused variables
static inline void Use(void *) { }

#define WX_SUPPRESS_UNUSED_WARN(x) Use(&x)

// ----------------------------------------------------------------------------
// initialization data
// ----------------------------------------------------------------------------

namespace
{

    // number of times wxInitialize() was called minus the number of times
    // wxUninitialize() was
    //
    // it is atomic to allow more than one thread to call wxInitialize() but
    // only one of them to actually initialize the library
    wxAtomicInt gs_nInitCount{0};

} // anonymous namespace

/* static */
wxInitData& wxInitData::Get()
{
    static wxInitData s_initData;

    return s_initData;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// command line arguments ANSI -> Unicode conversion
// ----------------------------------------------------------------------------

void wxInitData::Initialize(int argcIn, char **argvIn)
{
    wxASSERT_MSG( !argc && !argv, "initializing twice?" );

#ifndef __WINDOWS__
    argvA = argvIn;
#endif

    argv = new wchar_t *[argcIn + 1];

    int wargc = 0;
    for ( int i = 0; i < argcIn; i++ )
    {
#ifdef __DARWIN__
        wxWCharBuffer buf(wxConvFileName->cMB2WX(argvIn[i]));
#else
        wxWCharBuffer buf(wxConvLocal.cMB2WX(argvIn[i]));
#endif
        if ( !buf )
        {
            wxLogWarning(_("Command line argument %d couldn't be converted to Unicode and will be ignored."),
                         i);
        }
        else // converted ok
        {
            argv[wargc] = wxStrdup(buf);
            wargc++;
        }
    }

    argc = wargc;
    argv[wargc] = nullptr;
}

#ifdef __WINDOWS__

void wxInitData::MSWInitialize()
{
    wxASSERT_MSG( !argc && !argvMSW, "initializing twice?" );

    // Prefer to use the standard function for tokenizing the command line,
    // instead of our own wxCmdLineParser::ConvertStringToArgs() which might
    // not use exactly the same logic.

    // This pointer will be freed in Free().
    argvMSW = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

    // And this one will be used by the rest of the code. It is separate from
    // argvMSW because it could be allocated by Initialize() if a custom entry
    // point is used.
    argv = argvMSW;
}

#endif // __WINDOWS__

void wxInitData::InitializeFromWide(int argcIn, wchar_t** argvIn)
{
    // For simplicity, make a copy of the arguments, even though we could avoid
    // it -- but this would complicate the cleanup.
    argc = argcIn;
    argv = new wchar_t*[argc + 1];
    argv[argc] = nullptr;

#ifdef __WINDOWS__
    // Not used in this case and shouldn't be passed to LocalFree().
    argvMSW = nullptr;
#else // !__WINDOWS__
    // We need to convert from wide arguments back to the narrow ones.
    argvA = new char*[argc + 1];
    argvA[argc] = nullptr;

    ownsArgvA = true;
#endif // __WINDOWS__/!__WINDOWS__

    for ( int i = 0; i < argc; i++ )
    {
        argv[i] = wxCRT_StrdupW(argvIn[i]);

#ifndef __WINDOWS__
        argvA[i] = wxStrdup(wxConvUTF8.cWC2MB(argv[i]));
#endif // !__WINDOWS__
    }
}

void wxInitData::Free()
{
#ifdef __WINDOWS__
    if ( argvMSW )
    {
        ::LocalFree(argvMSW);

        // If argvMSW is non-null, argv must be the same value, so reset it too.
        argv = argvMSW = nullptr;
        argc = 0;
    }
#endif // __WINDOWS__

    if ( argc )
    {
        for ( int i = 0; i < argc; i++ )
        {
            free(argv[i]);
        }

#ifndef __WINDOWS__
        if ( ownsArgvA )
        {
            for ( int i = 0; i < argc; i++ )
            {
                free(argvA[i]);
            }

            wxDELETEA(argvA);
        }
#endif // !__WINDOWS__

        wxDELETEA(argv);
        argc = 0;
    }
}

// ----------------------------------------------------------------------------
// start up
// ----------------------------------------------------------------------------

// initialization which is always done (not customizable) before wxApp creation
static bool DoCommonPreInit()
{
    // This is necessary even for the default locale, see comments in this
    // function.
    wxEnsureLocaleIsCompatibleWithCRT();

#if wxUSE_LOG
    // Reset logging in case we were cleaned up and are being reinitialized.
    wxLog::DoCreateOnDemand();

    // force wxLog to create a log target now: we do it because wxTheApp
    // doesn't exist yet so wxLog will create a special log target which is
    // safe to use even when the GUI is not available while without this call
    // we could create wxApp in wxEntryStart() below, then log an error about
    // e.g. failure to establish connection to the X server and wxLog would
    // send it to wxLogGui (because wxTheApp does exist already) which, of
    // course, can't be used in this case
    //
    // notice also that this does nothing if the user had set up a custom log
    // target before -- which is fine as we want to give him this possibility
    // (as it's impossible to override logging by overriding wxAppTraits::
    // CreateLogTarget() before wxApp is created) and we just assume he knows
    // what he is doing
    wxLog::GetActiveTarget();
#endif // wxUSE_LOG

#ifdef __WINDOWS__
    // GUI applications obtain HINSTANCE in their WinMain() but we also need to
    // initialize the global wxhInstance variable for the console programs as
    // they may need it too, so set it here if it wasn't done yet
    if ( !wxGetInstance() )
    {
        wxSetInstance(::GetModuleHandle(nullptr));
    }
#endif // __WINDOWS__

    return true;
}

// non customizable initialization done after wxApp creation and initialization
static bool DoCommonPostInit()
{
    wxModule::RegisterModules();

    if ( !wxModule::InitializeModules() )
    {
        wxLogError(_("Initialization failed in post init, aborting."));
        return false;
    }

    return true;
}

bool wxEntryStart(int& argc, wxChar **argv)
{
    // do minimal, always necessary, initialization
    // --------------------------------------------

    // typically the command line arguments would be already initialized before
    // we're called, e.g. both the entry point taking (narrow) char argv and
    // the MSW one, using the entire (wide) string command line do it, but if
    // this function is called directly from the application initialization
    // code this wouldn't be the case, and we need to handle this too
    auto& initData = wxInitData::Get();
    if ( !initData.argc )
        initData.InitializeFromWide(argc, argv);

    // initialize wxRTTI
    if ( !DoCommonPreInit() )
        return false;


    // first of all, we need an application object
    // -------------------------------------------

    // the user might have already created it himself somehow
    wxAppPtr app(wxTheApp);
    if ( !app.get() )
    {
        // if they didn't, we should normally have the initializer function set
        // up by wxIMPLEMENT_APP(), so use it to create one now
        if ( auto fnCreate = wxApp::GetInitializerFunction() )
        {
            app.Set((*fnCreate)());
        }
    }

    if ( !app.get() )
    {
        // either wxIMPLEMENT_APP() was not used at all or it failed -- in
        // any case we still need something
        app.Set(new wxDummyConsoleApp);
    }


    // wxApp initialization: this can be customized
    // --------------------------------------------

    if ( !app->Initialize(argc, argv) )
        return false;

    // remember, possibly modified (e.g. due to removal of toolkit-specific
    // parameters), command line arguments in member variables
    app->argc = argc;
    app->argv.Init(argc, argv);

    wxCallAppCleanup callAppCleanup(app.get());


    // common initialization after wxTheApp creation
    // ---------------------------------------------

    if ( !DoCommonPostInit() )
        return false;


    // prevent the smart pointer from destroying its contents
    app.release();

    // and the cleanup object from doing cleanup
    callAppCleanup.Dismiss();

#if wxUSE_LOG
    // now that we have a valid wxApp (wxLogGui would have crashed if we used
    // it before now), we can delete the temporary sink we had created for the
    // initialization messages -- the next time logging function is called, the
    // sink will be recreated but this time wxAppTraits will be used
    delete wxLog::SetActiveTarget(nullptr);
#endif // wxUSE_LOG

    return true;
}

// we provide a wxEntryStart() wrapper taking "char *" pointer too
bool wxEntryStart(int& argc, char **argv)
{
    auto& initData = wxInitData::Get();
    initData.Initialize(argc, argv);

    if ( !wxEntryStart(initData.argc, initData.argv) )
    {
        initData.Free();

        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------
// clean up
// ----------------------------------------------------------------------------

// cleanup done before destroying wxTheApp
static void DoCommonPreCleanup()
{
#if wxUSE_LOG
    // flush the logged messages if any and don't use the current probably
    // unsafe log target any more: the default one (wxLogGui) can't be used
    // after the resources are freed which happens when we return and the user
    // supplied one might be even more unsafe (using any wxWidgets GUI function
    // is unsafe starting from now)
    //
    // notice that wxLog will still recreate a default log target if any
    // messages are logged but that one will be safe to use until the very end
    delete wxLog::SetActiveTarget(nullptr);
#endif // wxUSE_LOG
}

// cleanup done after destroying wxTheApp
static void DoCommonPostCleanup()
{
    wxModule::CleanUpModules();

    // we can't do this in wxApp itself because it doesn't know if argv had
    // been allocated
    wxInitData::Get().Free();

    // use Set(nullptr) and not Get() to avoid creating a message output object on
    // demand when we just want to delete it
    delete wxMessageOutput::Set(nullptr);

#if wxUSE_LOG
    // call this first as it has a side effect: in addition to flushing all
    // logs for this thread, it also flushes everything logged from other
    // threads
    wxLog::FlushActive();

    // and now delete the last logger as well
    //
    // we still don't disable log target auto-vivification even if any log
    // objects created now will result in memory leaks because it seems better
    // to leak memory which doesn't matter much considering the application is
    // exiting anyhow than to not show messages which could still be logged
    // from the user code (e.g. static dtors and such)
    delete wxLog::SetActiveTarget(nullptr);
#endif // wxUSE_LOG
}

void wxEntryCleanup()
{
    DoCommonPreCleanup();


    // delete the application object
    if ( wxTheApp )
    {
        wxTheApp->CleanUp();

        // reset the global pointer to it to nullptr before destroying it as in
        // some circumstances this can result in executing the code using
        // wxTheApp and using half-destroyed object is no good
        wxAppConsole * const app = wxApp::GetInstance();
        wxApp::SetInstance(nullptr);
        delete app;
    }


    DoCommonPostCleanup();
}

// ----------------------------------------------------------------------------
// wxEntry
// ----------------------------------------------------------------------------

// for MSW the real wxEntry is defined in msw/main.cpp
#ifndef __WINDOWS__
    #define wxEntryReal wxEntry
#endif // !__WINDOWS__

int wxEntryReal(int& argc, wxChar **argv)
{
    // library initialization
    wxInitializer initializer(argc, argv);

    if ( !initializer.IsOk() )
    {
#if wxUSE_LOG
        // flush any log messages explaining why we failed
        delete wxLog::SetActiveTarget(nullptr);
#endif
        return -1;
    }

    wxTRY
    {
        // app initialization
        if ( !wxTheApp->CallOnInit() )
        {
            // don't call OnExit() if OnInit() failed
            return -1;
        }

        // ensure that OnExit() is called if OnInit() had succeeded
        class CallOnExit
        {
        public:
            ~CallOnExit() { wxTheApp->OnExit(); }
        } callOnExit;

        WX_SUPPRESS_UNUSED_WARN(callOnExit);

        // app execution
        return wxTheApp->OnRun();
    }
    wxCATCH_ALL( wxTheApp->OnUnhandledException(); return -1; )
}

// as with wxEntryStart, we provide an ANSI wrapper
int wxEntry(int& argc, char **argv)
{
    auto& initData = wxInitData::Get();
    initData.Initialize(argc, argv);

    return wxEntry(initData.argc, initData.argv);
}

// ----------------------------------------------------------------------------
// wxInitialize/wxUninitialize
// ----------------------------------------------------------------------------

bool wxInitialize()
{
    int argc = 0;
    return wxInitialize(argc, (wxChar**)nullptr);
}

bool wxInitialize(int& argc, wxChar **argv)
{
    if ( wxAtomicInc(gs_nInitCount) != 1 )
    {
        // already initialized
        return true;
    }

    return wxEntryStart(argc, argv);
}

bool wxInitialize(int& argc, char **argv)
{
    if ( wxAtomicInc(gs_nInitCount) != 1 )
    {
        // already initialized
        return true;
    }

    return wxEntryStart(argc, argv);
}

void wxUninitialize()
{
    if ( wxAtomicDec(gs_nInitCount) != 0 )
        return;

    wxEntryCleanup();
}
