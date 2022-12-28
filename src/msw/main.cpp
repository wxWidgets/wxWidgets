/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/main.cpp
// Purpose:     WinMain/DllMain
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/dynlib.h"

#include "wx/msw/private.h"
#include "wx/msw/seh.h"

#if wxUSE_ON_FATAL_EXCEPTION
    #include "wx/datetime.h"
    #include "wx/msw/crashrpt.h"
#endif // wxUSE_ON_FATAL_EXCEPTION

// defined in common/init.cpp
extern int wxEntryReal(int& argc, wxChar **argv);
extern void wxEntryCleanupReal();

// ============================================================================
// implementation: various entry points
// ============================================================================

#if wxUSE_BASE

// ----------------------------------------------------------------------------
// wrapper wxEntry catching all Win32 exceptions occurring in a wx program
// ----------------------------------------------------------------------------

// wrap real wxEntry in a try-except block to be able to call
// OnFatalException() if necessary
#if wxUSE_ON_FATAL_EXCEPTION

// global pointer to exception information, only valid inside OnFatalException,
// used by wxStackWalker and wxCrashReport
extern EXCEPTION_POINTERS *wxGlobalSEInformation = nullptr;

// flag telling us whether the application wants to handle exceptions at all
static bool gs_handleExceptions = false;

static void wxFatalExit()
{
    // use the same exit code as abort()
    ::ExitProcess(3);
}

unsigned long wxGlobalSEHandler(EXCEPTION_POINTERS *pExcPtrs)
{
    if ( gs_handleExceptions && wxTheApp )
    {
        // store the pointer to exception info
        wxGlobalSEInformation = pExcPtrs;

        // give the user a chance to do something special about this
        wxSEH_TRY
        {
            wxTheApp->OnFatalException();
        }
        wxSEH_IGNORE      // ignore any exceptions inside the exception handler

        wxGlobalSEInformation = nullptr;

        // this will execute our handler and terminate the process
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef __VISUALC__

void wxSETranslator(unsigned int WXUNUSED(code), EXCEPTION_POINTERS *ep)
{
    switch ( wxGlobalSEHandler(ep) )
    {
        default:
            wxFAIL_MSG( wxT("unexpected wxGlobalSEHandler() return value") );
            // fall through

        case EXCEPTION_EXECUTE_HANDLER:
            // if wxApp::OnFatalException() had been called we should exit the
            // application -- but we shouldn't kill our host when we're a DLL
#ifndef WXMAKINGDLL
            wxFatalExit();
#endif // not a DLL
            break;

        case EXCEPTION_CONTINUE_SEARCH:
            // we're called for each "catch ( ... )" and if we (re)throw from
            // here, the catch handler body is not executed, so the effect is
            // as if had inhibited translation of SE to C++ ones because the
            // handler will never see any structured exceptions
            throw;
    }
}

#endif // __VISUALC__

bool wxHandleFatalExceptions(bool doit)
{
    // assume this can only be called from the main thread
    gs_handleExceptions = doit;

#if wxUSE_CRASHREPORT
    if ( doit )
    {
        // try to find a place where we can put out report file later
        wxChar fullname[MAX_PATH];
        if ( !::GetTempPath(WXSIZEOF(fullname), fullname) )
        {
            wxLogLastError(wxT("GetTempPath"));

            // when all else fails...
            wxStrcpy(fullname, wxT("c:\\"));
        }

        // use PID and date to make the report file name more unique
        wxString name = wxString::Format
                        (
#if wxUSE_DATETIME
                            wxT("%s_%s_%lu.dmp"),
#else
                            wxT("%s_%lu.dmp"),
#endif
                            wxTheApp ? (const wxChar*)wxTheApp->GetAppDisplayName().c_str()
                                     : wxT("wxwindows"),
#if wxUSE_DATETIME
                            wxDateTime::Now().Format(wxT("%Y%m%dT%H%M%S")).c_str(),
#endif
                            ::GetCurrentProcessId()
                        );

        wxStrncat(fullname, name, WXSIZEOF(fullname) - wxStrlen(fullname) - 1);

        wxCrashReport::SetFileName(fullname);
    }
#endif // wxUSE_CRASHREPORT

    return true;
}

int wxEntry(int& argc, wxChar **argv)
{
    DisableAutomaticSETranslator();

    wxSEH_TRY
    {
        return wxEntryReal(argc, argv);
    }
    wxSEH_HANDLE(-1)
}

#else // !wxUSE_ON_FATAL_EXCEPTION

int wxEntry(int& argc, wxChar **argv)
{
    return wxEntryReal(argc, argv);
}

#endif // wxUSE_ON_FATAL_EXCEPTION/!wxUSE_ON_FATAL_EXCEPTION

#endif // wxUSE_BASE

// ----------------------------------------------------------------------------
// Windows-specific wxEntry
// ----------------------------------------------------------------------------

// Declare the functions used in wxCore to access the command line arguments
// data in wxBase.
WXDLLIMPEXP_BASE void wxMSWCommandLineInit();
WXDLLIMPEXP_BASE void wxMSWCommandLineCleanup();
WXDLLIMPEXP_BASE int& wxMSWCommandLineGetArgc();
WXDLLIMPEXP_BASE wchar_t** wxMSWCommandLineGetArgv();

#if wxUSE_BASE

namespace
{

struct wxMSWCommandLineArguments
{
    wxMSWCommandLineArguments() { argc = 0; argv = nullptr; }

    // Initialize this object from the current process command line.
    //
    // In Unicode build prefer to use the standard function for tokenizing the
    // command line, but we can't use it with narrow strings, so use our own
    // approximation instead then.
    void Init()
    {
        argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    }

    void Cleanup()
    {
        if ( argc )
        {
            ::LocalFree(argv);
            argc = 0;
        }
    }

    ~wxMSWCommandLineArguments()
    {
        Cleanup();
    }

    int argc;
    wxChar **argv;

    wxDECLARE_NO_COPY_CLASS(wxMSWCommandLineArguments);
};

static wxMSWCommandLineArguments wxArgs;

} // anonymous namespace

WXDLLIMPEXP_BASE void wxMSWCommandLineInit()
{
    wxArgs.Init();
}

WXDLLIMPEXP_BASE void wxMSWCommandLineCleanup()
{
    wxArgs.Cleanup();
}

WXDLLIMPEXP_BASE int& wxMSWCommandLineGetArgc()
{
    return wxArgs.argc;
}

WXDLLIMPEXP_BASE wchar_t** wxMSWCommandLineGetArgv()
{
    return wxArgs.argv;
}

#endif // wxUSE_BASE

#if wxUSE_GUI

// common part of wxMSW-specific wxEntryStart() and wxEntry() overloads
static bool
wxMSWEntryCommon(HINSTANCE hInstance, int nCmdShow)
{
    // remember the parameters Windows gave us
    wxSetInstance(hInstance);
#ifdef __WXMSW__
    wxApp::m_nCmdShow = nCmdShow;
#else
    wxUnusedVar(nCmdShow);
#endif

    wxMSWCommandLineInit();

    return true;
}

WXDLLEXPORT bool wxEntryStart(HINSTANCE hInstance,
                              HINSTANCE WXUNUSED(hPrevInstance),
                              wxCmdLineArgType WXUNUSED(pCmdLine),
                              int nCmdShow)
{
    if ( !wxMSWEntryCommon(hInstance, nCmdShow) )
       return false;

    return wxEntryStart(wxMSWCommandLineGetArgc(), wxMSWCommandLineGetArgv());
}

WXDLLEXPORT int wxEntry(HINSTANCE hInstance,
                        HINSTANCE WXUNUSED(hPrevInstance),
                        wxCmdLineArgType WXUNUSED(pCmdLine),
                        int nCmdShow)
{
    if ( !wxMSWEntryCommon(hInstance, nCmdShow) )
        return -1;

    return wxEntry(wxMSWCommandLineGetArgc(), wxMSWCommandLineGetArgv());
}

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// global HINSTANCE
// ----------------------------------------------------------------------------

#if wxUSE_BASE

int wxEntry()
{
    wxMSWCommandLineInit();

    return wxEntry(wxMSWCommandLineGetArgc(), wxMSWCommandLineGetArgv());
}

void wxEntryCleanup()
{
    wxEntryCleanupReal();

    wxMSWCommandLineCleanup();
}

HINSTANCE wxhInstance = 0;

extern "C" HINSTANCE wxGetInstance()
{
    return wxhInstance;
}

void wxSetInstance(HINSTANCE hInst)
{
    wxhInstance = hInst;
}

#endif // wxUSE_BASE
