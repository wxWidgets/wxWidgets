///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/appbase.cpp
// Purpose:     implements wxAppConsoleBase class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.06.2003 (extracted from common/appcmn.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include  "wx/msw/wrapwin.h"  // includes windows.h for MessageBox()
    #endif
    #include "wx/list.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/wxcrtvararg.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/cmdline.h"
#include "wx/confbase.h"
#include "wx/evtloop.h"
#include "wx/filename.h"
#include "wx/msgout.h"
#include "wx/ptr_scpd.h"
#include "wx/tokenzr.h"
#include "wx/thread.h"

#if wxUSE_EXCEPTIONS && wxUSE_STL
    #include <exception>
    #include <typeinfo>
#endif

#ifndef __WXPALMOS5__
#if !defined(__WXMSW__) || defined(__WXMICROWIN__)
  #include  <signal.h>      // for SIGTRAP used by wxTrap()
#endif  //Win/Unix

#include <locale.h>
#endif // ! __WXPALMOS5__

#if wxUSE_FONTMAP
    #include "wx/fontmap.h"
#endif // wxUSE_FONTMAP

#ifdef __WXDEBUG__
    #if wxUSE_STACKWALKER
        #include "wx/stackwalk.h"
        #ifdef __WXMSW__
            #include "wx/msw/debughlp.h"
        #endif
    #endif // wxUSE_STACKWALKER

    #include "wx/recguard.h"
#endif // __WXDEBUG__

// wxABI_VERSION can be defined when compiling applications but it should be
// left undefined when compiling the library itself, it is then set to its
// default value in version.h
#if wxABI_VERSION != wxMAJOR_VERSION * 10000 + wxMINOR_VERSION * 100 + 99
#error "wxABI_VERSION should not be defined when compiling the library"
#endif

// ----------------------------------------------------------------------------
// private functions prototypes
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    // really just show the assert dialog
    static bool DoShowAssertDialog(const wxString& msg);

    // prepare for showing the assert dialog, use the given traits or
    // DoShowAssertDialog() as last fallback to really show it
    static
    void ShowAssertDialog(const wxString& szFile,
                          int nLine,
                          const wxString& szFunc,
                          const wxString& szCond,
                          const wxString& szMsg,
                          wxAppTraits *traits = NULL);

    // turn on the trace masks specified in the env variable WXTRACE
    static void LINKAGEMODE SetTraceMasks();
#endif // __WXDEBUG__

// ----------------------------------------------------------------------------
// global vars
// ----------------------------------------------------------------------------

wxAppConsole *wxAppConsoleBase::ms_appInstance = NULL;

wxAppInitializerFunction wxAppConsoleBase::ms_appInitFn = NULL;

wxSocketManager *wxAppTraitsBase::ms_manager = NULL;

// ----------------------------------------------------------------------------
// wxEventLoopPtr
// ----------------------------------------------------------------------------

// this defines wxEventLoopPtr
wxDEFINE_TIED_SCOPED_PTR_TYPE(wxEventLoopBase)

// ============================================================================
// wxAppConsoleBase implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxAppConsoleBase::wxAppConsoleBase()
{
    m_traits = NULL;
    m_mainLoop = NULL;

    ms_appInstance = static_cast<wxAppConsole *>(this);

#ifdef __WXDEBUG__
    SetTraceMasks();
#if wxUSE_UNICODE
    // In unicode mode the SetTraceMasks call can cause an apptraits to be
    // created, but since we are still in the constructor the wrong kind will
    // be created for GUI apps.  Destroy it so it can be created again later.
    delete m_traits;
    m_traits = NULL;
#endif
#endif
}

wxAppConsoleBase::~wxAppConsoleBase()
{
    delete m_traits;
}

// ----------------------------------------------------------------------------
// initilization/cleanup
// ----------------------------------------------------------------------------

bool wxAppConsoleBase::Initialize(int& WXUNUSED(argc), wxChar **argv)
{
#if wxUSE_INTL
    GetTraits()->SetLocale();
#endif // wxUSE_INTL

#if wxUSE_THREADS
    wxHandlersWithPendingEventsLocker = new wxCriticalSection;
#endif

#ifndef __WXPALMOS__
    if ( m_appName.empty() && argv && argv[0] )
    {
        // the application name is, by default, the name of its executable file
        wxFileName::SplitPath(argv[0], NULL, &m_appName, NULL);
    }
#endif // !__WXPALMOS__

    return true;
}

wxEventLoopBase *wxAppConsoleBase::CreateMainLoop()
{
    return GetTraits()->CreateEventLoop();
}

void wxAppConsoleBase::CleanUp()
{
    if ( m_mainLoop )
    {
        delete m_mainLoop;
        m_mainLoop = NULL;
    }

    delete wxHandlersWithPendingEvents;
    wxHandlersWithPendingEvents = NULL;

#if wxUSE_THREADS
    delete wxHandlersWithPendingEventsLocker;
    wxHandlersWithPendingEventsLocker = NULL;
#endif // wxUSE_THREADS
}

// ----------------------------------------------------------------------------
// OnXXX() callbacks
// ----------------------------------------------------------------------------

bool wxAppConsoleBase::OnInit()
{
#if wxUSE_CMDLINE_PARSER
    wxCmdLineParser parser(argc, argv);

    OnInitCmdLine(parser);

    bool cont;
    switch ( parser.Parse(false /* don't show usage */) )
    {
        case -1:
            cont = OnCmdLineHelp(parser);
            break;

        case 0:
            cont = OnCmdLineParsed(parser);
            break;

        default:
            cont = OnCmdLineError(parser);
            break;
    }

    if ( !cont )
        return false;
#endif // wxUSE_CMDLINE_PARSER

    return true;
}

int wxAppConsoleBase::OnRun()
{
    return MainLoop();
}

int wxAppConsoleBase::OnExit()
{
#if wxUSE_CONFIG
    // delete the config object if any (don't use Get() here, but Set()
    // because Get() could create a new config object)
    delete wxConfigBase::Set((wxConfigBase *) NULL);
#endif // wxUSE_CONFIG

    return 0;
}

void wxAppConsoleBase::Exit()
{
    if (m_mainLoop != NULL)
        ExitMainLoop();
    else
        exit(-1);
}

// ----------------------------------------------------------------------------
// traits stuff
// ----------------------------------------------------------------------------

wxAppTraits *wxAppConsoleBase::CreateTraits()
{
    return new wxConsoleAppTraits;
}

wxAppTraits *wxAppConsoleBase::GetTraits()
{
    // FIXME-MT: protect this with a CS?
    if ( !m_traits )
    {
        m_traits = CreateTraits();

        wxASSERT_MSG( m_traits, _T("wxApp::CreateTraits() failed?") );
    }

    return m_traits;
}

/* static */
wxAppTraits *wxAppConsoleBase::GetTraitsIfExists()
{
    wxAppConsole * const app = GetInstance();
    return app ? app->GetTraits() : NULL;
}

// ----------------------------------------------------------------------------
// event processing
// ----------------------------------------------------------------------------

int wxAppConsoleBase::MainLoop()
{
    wxEventLoopBaseTiedPtr mainLoop(&m_mainLoop, CreateMainLoop());

    return m_mainLoop ? m_mainLoop->Run() : -1;
}

void wxAppConsoleBase::ExitMainLoop()
{
    // we should exit from the main event loop, not just any currently active
    // (e.g. modal dialog) event loop
    if ( m_mainLoop && m_mainLoop->IsRunning() )
    {
        m_mainLoop->Exit(0);
    }
}

bool wxAppConsoleBase::Pending()
{
    // use the currently active message loop here, not m_mainLoop, because if
    // we're showing a modal dialog (with its own event loop) currently the
    // main event loop is not running anyhow
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();

    return loop && loop->Pending();
}

bool wxAppConsoleBase::Dispatch()
{
    // see comment in Pending()
    wxEventLoopBase * const loop = wxEventLoopBase::GetActive();

    return loop && loop->Dispatch();
}

bool wxAppConsoleBase::HasPendingEvents() const
{
    wxENTER_CRIT_SECT( *wxHandlersWithPendingEventsLocker );

    bool has = wxHandlersWithPendingEvents && !wxHandlersWithPendingEvents->IsEmpty();

    wxLEAVE_CRIT_SECT( *wxHandlersWithPendingEventsLocker );

    return has;
}

/* static */
bool wxAppConsoleBase::IsMainLoopRunning()
{
    const wxAppConsole * const app = GetInstance();

    return app && app->m_mainLoop != NULL;
}

void wxAppConsoleBase::ProcessPendingEvents()
{
#if wxUSE_THREADS
    if ( !wxHandlersWithPendingEventsLocker )
        return;
#endif

    wxENTER_CRIT_SECT( *wxHandlersWithPendingEventsLocker );

    if (wxHandlersWithPendingEvents)
    {
        // iterate until the list becomes empty: the handlers remove themselves
        // from it when they don't have any more pending events
        wxList::compatibility_iterator node = wxHandlersWithPendingEvents->GetFirst();
        while (node)
        {
            // In ProcessPendingEvents(), new handlers might be add
            // and we can safely leave the critical section here.
            wxLEAVE_CRIT_SECT( *wxHandlersWithPendingEventsLocker );

            wxEvtHandler *handler = (wxEvtHandler *)node->GetData();
            handler->ProcessPendingEvents();

            wxENTER_CRIT_SECT( *wxHandlersWithPendingEventsLocker );

            // restart as the iterators could have been invalidated
            node = wxHandlersWithPendingEvents->GetFirst();
        }
    }

    wxLEAVE_CRIT_SECT( *wxHandlersWithPendingEventsLocker );
}

void wxAppConsoleBase::WakeUpIdle()
{
    if ( m_mainLoop )
        m_mainLoop->WakeUp();
}

bool wxAppConsoleBase::ProcessIdle()
{
    // process pending wx events before sending idle events
    ProcessPendingEvents();

    wxIdleEvent event;

    event.SetEventObject(this);
    ProcessEvent(event);
    return event.MoreRequested();
}

int wxAppConsoleBase::FilterEvent(wxEvent& WXUNUSED(event))
{
    // process the events normally by default
    return -1;
}

// ----------------------------------------------------------------------------
// exception handling
// ----------------------------------------------------------------------------

#if wxUSE_EXCEPTIONS

void
wxAppConsoleBase::HandleEvent(wxEvtHandler *handler,
                              wxEventFunction func,
                              wxEvent& event) const
{
    // by default, simply call the handler
    (handler->*func)(event);
}

void wxAppConsoleBase::OnUnhandledException()
{
#ifdef __WXDEBUG__
    // we're called from an exception handler so we can re-throw the exception
    // to recover its type
    wxString what;
    try
    {
        throw;
    }
#if wxUSE_STL
    catch ( std::exception& e )
    {
        what.Printf("std::exception of type \"%s\", what() = \"%s\"",
                    typeid(e).name(), e.what());
    }
#endif // wxUSE_STL
    catch ( ... )
    {
        what = "unknown exception";
    }

    wxMessageOutputBest().Printf(
        "*** Caught unhandled %s; terminating\n", what
    );
#endif // __WXDEBUG__
}

// ----------------------------------------------------------------------------
// exceptions support
// ----------------------------------------------------------------------------

bool wxAppConsoleBase::OnExceptionInMainLoop()
{
    throw;

    // some compilers are too stupid to know that we never return after throw
#if defined(__DMC__) || (defined(_MSC_VER) && _MSC_VER < 1200)
    return false;
#endif
}

#endif // wxUSE_EXCEPTIONS

// ----------------------------------------------------------------------------
// cmd line parsing
// ----------------------------------------------------------------------------

#if wxUSE_CMDLINE_PARSER

#define OPTION_VERBOSE "verbose"

void wxAppConsoleBase::OnInitCmdLine(wxCmdLineParser& parser)
{
    // the standard command line options
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        {
            wxCMD_LINE_SWITCH,
            "h",
            "help",
            gettext_noop("show this help message"),
            wxCMD_LINE_VAL_NONE,
            wxCMD_LINE_OPTION_HELP
        },

#if wxUSE_LOG
        {
            wxCMD_LINE_SWITCH,
            NULL,
            OPTION_VERBOSE,
            gettext_noop("generate verbose log messages"),
            wxCMD_LINE_VAL_NONE,
            0x0
        },
#endif // wxUSE_LOG

        // terminator
        wxCMD_LINE_DESC_END
    };

    parser.SetDesc(cmdLineDesc);
}

bool wxAppConsoleBase::OnCmdLineParsed(wxCmdLineParser& parser)
{
#if wxUSE_LOG
    if ( parser.Found(OPTION_VERBOSE) )
    {
        wxLog::SetVerbose(true);
    }
#else
    wxUnusedVar(parser);
#endif // wxUSE_LOG

    return true;
}

bool wxAppConsoleBase::OnCmdLineHelp(wxCmdLineParser& parser)
{
    parser.Usage();

    return false;
}

bool wxAppConsoleBase::OnCmdLineError(wxCmdLineParser& parser)
{
    parser.Usage();

    return false;
}

#endif // wxUSE_CMDLINE_PARSER

// ----------------------------------------------------------------------------
// debugging support
// ----------------------------------------------------------------------------

/* static */
bool wxAppConsoleBase::CheckBuildOptions(const char *optionsSignature,
                                         const char *componentName)
{
#if 0 // can't use wxLogTrace, not up and running yet
    printf("checking build options object '%s' (ptr %p) in '%s'\n",
             optionsSignature, optionsSignature, componentName);
#endif

    if ( strcmp(optionsSignature, WX_BUILD_OPTIONS_SIGNATURE) != 0 )
    {
        wxString lib = wxString::FromAscii(WX_BUILD_OPTIONS_SIGNATURE);
        wxString prog = wxString::FromAscii(optionsSignature);
        wxString progName = wxString::FromAscii(componentName);
        wxString msg;

        msg.Printf(_T("Mismatch between the program and library build versions detected.\nThe library used %s,\nand %s used %s."),
                   lib.c_str(), progName.c_str(), prog.c_str());

        wxLogFatalError(msg.c_str());

        // normally wxLogFatalError doesn't return
        return false;
    }
#undef wxCMP

    return true;
}

#ifdef __WXDEBUG__

void wxAppConsoleBase::OnAssertFailure(const wxChar *file,
                                       int line,
                                       const wxChar *func,
                                       const wxChar *cond,
                                       const wxChar *msg)
{
    ShowAssertDialog(file, line, func, cond, msg, GetTraits());
}

void wxAppConsoleBase::OnAssert(const wxChar *file,
                                int line,
                                const wxChar *cond,
                                const wxChar *msg)
{
    OnAssertFailure(file, line, NULL, cond, msg);
}

#endif // __WXDEBUG__

// ============================================================================
// other classes implementations
// ============================================================================

// ----------------------------------------------------------------------------
// wxConsoleAppTraitsBase
// ----------------------------------------------------------------------------

#if wxUSE_LOG

wxLog *wxConsoleAppTraitsBase::CreateLogTarget()
{
    return new wxLogStderr;
}

#endif // wxUSE_LOG

wxMessageOutput *wxConsoleAppTraitsBase::CreateMessageOutput()
{
    return new wxMessageOutputStderr;
}

#if wxUSE_FONTMAP

wxFontMapper *wxConsoleAppTraitsBase::CreateFontMapper()
{
    return (wxFontMapper *)new wxFontMapperBase;
}

#endif // wxUSE_FONTMAP

wxRendererNative *wxConsoleAppTraitsBase::CreateRenderer()
{
    // console applications don't use renderers
    return NULL;
}

#ifdef __WXDEBUG__
bool wxConsoleAppTraitsBase::ShowAssertDialog(const wxString& msg)
{
    return wxAppTraitsBase::ShowAssertDialog(msg);
}
#endif

bool wxConsoleAppTraitsBase::HasStderr()
{
    // console applications always have stderr, even under Mac/Windows
    return true;
}

void wxConsoleAppTraitsBase::ScheduleForDestroy(wxObject *object)
{
    delete object;
}

void wxConsoleAppTraitsBase::RemoveFromPendingDelete(wxObject * WXUNUSED(object))
{
    // nothing to do
}

// ----------------------------------------------------------------------------
// wxAppTraits
// ----------------------------------------------------------------------------

#if wxUSE_INTL
void wxAppTraitsBase::SetLocale()
{
    wxSetlocale(LC_ALL, "");
    wxUpdateLocaleIsUtf8();
}
#endif

#if wxUSE_THREADS
void wxMutexGuiEnterImpl();
void wxMutexGuiLeaveImpl();

void wxAppTraitsBase::MutexGuiEnter()
{
    wxMutexGuiEnterImpl();
}

void wxAppTraitsBase::MutexGuiLeave()
{
    wxMutexGuiLeaveImpl();
}

void WXDLLIMPEXP_BASE wxMutexGuiEnter()
{
    wxAppTraits * const traits = wxAppConsoleBase::GetTraitsIfExists();
    if ( traits )
        traits->MutexGuiEnter();
}

void WXDLLIMPEXP_BASE wxMutexGuiLeave()
{
    wxAppTraits * const traits = wxAppConsoleBase::GetTraitsIfExists();
    if ( traits )
        traits->MutexGuiLeave();
}
#endif // wxUSE_THREADS

#ifdef __WXDEBUG__

bool wxAppTraitsBase::ShowAssertDialog(const wxString& msgOriginal)
{
    wxString msg = msgOriginal;

#if wxUSE_STACKWALKER
#if !defined(__WXMSW__)
    // on Unix stack frame generation may take some time, depending on the
    // size of the executable mainly... warn the user that we are working
    wxFprintf(stderr, wxT("[Debug] Generating a stack trace... please wait"));
    fflush(stderr);
#endif

    const wxString stackTrace = GetAssertStackTrace();
    if ( !stackTrace.empty() )
        msg << _T("\n\nCall stack:\n") << stackTrace;
#endif // wxUSE_STACKWALKER

    return DoShowAssertDialog(msg);
}

#if wxUSE_STACKWALKER
wxString wxAppTraitsBase::GetAssertStackTrace()
{
    wxString stackTrace;

    class StackDump : public wxStackWalker
    {
    public:
        StackDump() { }

        const wxString& GetStackTrace() const { return m_stackTrace; }

    protected:
        virtual void OnStackFrame(const wxStackFrame& frame)
        {
            m_stackTrace << wxString::Format
                            (
                              _T("[%02d] "),
                              wx_truncate_cast(int, frame.GetLevel())
                            );

            wxString name = frame.GetName();
            if ( !name.empty() )
            {
                m_stackTrace << wxString::Format(_T("%-40s"), name.c_str());
            }
            else
            {
                m_stackTrace << wxString::Format(_T("%p"), frame.GetAddress());
            }

            if ( frame.HasSourceLocation() )
            {
                m_stackTrace << _T('\t')
                             << frame.GetFileName()
                             << _T(':')
                             << frame.GetLine();
            }

            m_stackTrace << _T('\n');
        }

    private:
        wxString m_stackTrace;
    };

    // don't show more than maxLines or we could get a dialog too tall to be
    // shown on screen: 20 should be ok everywhere as even with 15 pixel high
    // characters it is still only 300 pixels...
    static const int maxLines = 20;

    StackDump dump;
    dump.Walk(2, maxLines); // don't show OnAssert() call itself
    stackTrace = dump.GetStackTrace();

    const int count = stackTrace.Freq(wxT('\n'));
    for ( int i = 0; i < count - maxLines; i++ )
        stackTrace = stackTrace.BeforeLast(wxT('\n'));

    return stackTrace;
}
#endif // wxUSE_STACKWALKER


#endif // __WXDEBUG__

// ============================================================================
// global functions implementation
// ============================================================================

void wxExit()
{
    if ( wxTheApp )
    {
        wxTheApp->Exit();
    }
    else
    {
        // what else can we do?
        exit(-1);
    }
}

void wxWakeUpIdle()
{
    if ( wxTheApp )
    {
        wxTheApp->WakeUpIdle();
    }
    //else: do nothing, what can we do?
}

#ifdef  __WXDEBUG__

// wxASSERT() helper
bool wxAssertIsEqual(int x, int y)
{
    return x == y;
}

// break into the debugger
void wxTrap()
{
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    DebugBreak();
#elif defined(_MSL_USING_MW_C_HEADERS) && _MSL_USING_MW_C_HEADERS
    Debugger();
#elif defined(__UNIX__)
    raise(SIGTRAP);
#else
    // TODO
#endif // Win/Unix
}

// this function is called when an assert fails
static void wxDoOnAssert(const wxString& szFile,
                         int nLine,
                         const wxString& szFunc,
                         const wxString& szCond,
                         const wxString& szMsg = wxEmptyString)
{
    // FIXME MT-unsafe
    static int s_bInAssert = 0;

    wxRecursionGuard guard(s_bInAssert);
    if ( guard.IsInside() )
    {
        // can't use assert here to avoid infinite loops, so just trap
        wxTrap();

        return;
    }

    if ( !wxTheApp )
    {
        // by default, show the assert dialog box -- we can't customize this
        // behaviour
        ShowAssertDialog(szFile, nLine, szFunc, szCond, szMsg);
    }
    else
    {
        // let the app process it as it wants
        // FIXME-UTF8: use wc_str(), not c_str(), when ANSI build is removed
        wxTheApp->OnAssertFailure(szFile.c_str(), nLine, szFunc.c_str(),
                                  szCond.c_str(), szMsg.c_str());
    }
}

void wxOnAssert(const wxString& szFile,
                int nLine,
                const wxString& szFunc,
                const wxString& szCond,
                const wxString& szMsg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, szMsg);
}

void wxOnAssert(const wxString& szFile,
                int nLine,
                const wxString& szFunc,
                const wxString& szCond)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond);
}

void wxOnAssert(const wxChar *szFile,
                int nLine,
                const char *szFunc,
                const wxChar *szCond,
                const wxChar *szMsg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, szMsg);
}

void wxOnAssert(const char *szFile,
                int nLine,
                const char *szFunc,
                const char *szCond,
                const wxString& szMsg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, szMsg);
}

void wxOnAssert(const char *szFile,
                int nLine,
                const char *szFunc,
                const char *szCond,
                const wxCStrData& msg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, msg);
}

#if wxUSE_UNICODE
void wxOnAssert(const char *szFile,
                int nLine,
                const char *szFunc,
                const char *szCond)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond);
}

void wxOnAssert(const char *szFile,
                int nLine,
                const char *szFunc,
                const char *szCond,
                const char *szMsg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, szMsg);
}

void wxOnAssert(const char *szFile,
                int nLine,
                const char *szFunc,
                const char *szCond,
                const wxChar *szMsg)
{
    wxDoOnAssert(szFile, nLine, szFunc, szCond, szMsg);
}
#endif // wxUSE_UNICODE

#endif // __WXDEBUG__

// ============================================================================
// private functions implementation
// ============================================================================

#ifdef __WXDEBUG__

static void LINKAGEMODE SetTraceMasks()
{
#if wxUSE_LOG
    wxString mask;
    if ( wxGetEnv(wxT("WXTRACE"), &mask) )
    {
        wxStringTokenizer tkn(mask, wxT(",;:"));
        while ( tkn.HasMoreTokens() )
            wxLog::AddTraceMask(tkn.GetNextToken());
    }
#endif // wxUSE_LOG
}

static
bool DoShowAssertDialog(const wxString& msg)
{
    // under MSW we can show the dialog even in the console mode
#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
    wxString msgDlg(msg);

    // this message is intentionally not translated -- it is for
    // developpers only
    msgDlg += wxT("\nDo you want to stop the program?\n")
              wxT("You can also choose [Cancel] to suppress ")
              wxT("further warnings.");

    switch ( ::MessageBox(NULL, msgDlg.wx_str(), _T("wxWidgets Debug Alert"),
                          MB_YESNOCANCEL | MB_ICONSTOP ) )
    {
        case IDYES:
            wxTrap();
            break;

        case IDCANCEL:
            // stop the asserts
            return true;

        //case IDNO: nothing to do
    }
#else // !__WXMSW__
    wxFprintf(stderr, wxT("%s\n"), msg.c_str());
    fflush(stderr);

    // TODO: ask the user to enter "Y" or "N" on the console?
    wxTrap();
#endif // __WXMSW__/!__WXMSW__

    // continue with the asserts
    return false;
}

// show the assert modal dialog
static
void ShowAssertDialog(const wxString& szFile,
                      int nLine,
                      const wxString& szFunc,
                      const wxString& szCond,
                      const wxString& szMsg,
                      wxAppTraits *traits)
{
    // this variable can be set to true to suppress "assert failure" messages
    static bool s_bNoAsserts = false;

    wxString msg;
    msg.reserve(2048);

    // make life easier for people using VC++ IDE by using this format: like
    // this, clicking on the message will take us immediately to the place of
    // the failed assert
    msg.Printf(wxT("%s(%d): assert \"%s\" failed"), szFile, nLine, szCond);

    // add the function name, if any
    if ( !szFunc.empty() )
        msg << _T(" in ") << szFunc << _T("()");

    // and the message itself
    if ( !szMsg.empty() )
    {
        msg << _T(": ") << szMsg;
    }
    else // no message given
    {
        msg << _T('.');
    }

#if wxUSE_THREADS
    // if we are not in the main thread, output the assert directly and trap
    // since dialogs cannot be displayed
    if ( !wxThread::IsMain() )
    {
        msg += wxT(" [in child thread]");

#if defined(__WXMSW__) && !defined(__WXMICROWIN__)
        msg << wxT("\r\n");
        OutputDebugString(msg.wx_str());
#else
        // send to stderr
        wxFprintf(stderr, wxT("%s\n"), msg.c_str());
        fflush(stderr);
#endif
        // He-e-e-e-elp!! we're asserting in a child thread
        wxTrap();
    }
    else
#endif // wxUSE_THREADS

    if ( !s_bNoAsserts )
    {
        // send it to the normal log destination
        wxLogDebug(_T("%s"), msg.c_str());

        if ( traits )
        {
            // delegate showing assert dialog (if possible) to that class
            s_bNoAsserts = traits->ShowAssertDialog(msg);
        }
        else // no traits object
        {
            // fall back to the function of last resort
            s_bNoAsserts = DoShowAssertDialog(msg);
        }
    }
}

#endif // __WXDEBUG__
