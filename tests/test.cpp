///////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     Test program for wxWidgets
// Author:      Mike Wetherell
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h"
// and "catch.hpp"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Suppress some warnings in catch_impl.hpp.
wxCLANG_WARNING_SUPPRESS(missing-braces)
wxCLANG_WARNING_SUPPRESS(logical-op-parentheses)
wxCLANG_WARNING_SUPPRESS(inconsistent-missing-override)

// This file needs to get the CATCH definitions in addition to the usual
// assertion macros declarations from catch.hpp included by testprec.h.
// Including an internal file like this is ugly, but there doesn't seem to be
// any better way, see https://github.com/philsquared/Catch/issues/1061
#include "internal/catch_impl.hpp"

wxCLANG_WARNING_RESTORE(missing-braces)
wxCLANG_WARNING_RESTORE(logical-op-parentheses)
wxCLANG_WARNING_RESTORE(inconsistent-missing-override)

// This probably could be done by predefining CLARA_CONFIG_MAIN, but at the
// point where we are, just define this global variable manually.
namespace Catch { namespace Clara { UnpositionalTag _; } }

// Also define our own global variables.
namespace wxPrivate
{
std::string wxTheCurrentTestClass, wxTheCurrentTestMethod;
}

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/apptrait.h"
#include "wx/cmdline.h"
#include <exception>
#include <iostream>

#ifdef __WINDOWS__
    #include "wx/msw/msvcrt.h"
#endif

#ifdef __WXOSX__
    #include "wx/osx/private.h"
#endif

#if wxUSE_GUI
    #include "testableframe.h"

#ifdef __WXGTK__
    #include <glib.h>
#endif // __WXGTK__
#endif // wxUSE_GUI

#include "wx/socket.h"
#include "wx/evtloop.h"

using namespace std;

// ----------------------------------------------------------------------------
// helper classes
// ----------------------------------------------------------------------------

// exception class for MSVC debug CRT assertion failures
#ifdef wxUSE_VC_CRTDBG

struct CrtAssertFailure
{
    CrtAssertFailure(const char *message) : m_msg(message) { }

    const wxString m_msg;

    wxDECLARE_NO_ASSIGN_CLASS(CrtAssertFailure);
};

CATCH_TRANSLATE_EXCEPTION(CrtAssertFailure& e)
{
    return "CRT assert failure: " + e.m_msg.ToStdString(wxConvUTF8);
}

#endif // wxUSE_VC_CRTDBG

#if wxDEBUG_LEVEL

// Information about the last not yet handled assertion.
static wxString s_lastAssertMessage;

static wxString FormatAssertMessage(const wxString& file,
                                    int line,
                                    const wxString& func,
                                    const wxString& cond,
                                    const wxString& msg)
{
    wxString str;
    str << "wxWidgets assert: " << cond << " failed "
           "at " << file << ":" << line << " in " << func
        << " with message '" << msg << "'";
    return str;
}

static void TestAssertHandler(const wxString& file,
                              int line,
                              const wxString& func,
                              const wxString& cond,
                              const wxString& msg)
{
    // Determine whether we can safely throw an exception to just make the test
    // fail or whether we need to abort (in this case "msg" will contain the
    // explanation why did we decide to do it).
    wxString abortReason;

    const wxString
        assertMessage = FormatAssertMessage(file, line, func, cond, msg);

    if ( !wxIsMainThread() )
    {
        // Exceptions thrown from worker threads are not caught currently and
        // so we'd just die without any useful information -- abort instead.
        abortReason << assertMessage << "in a worker thread.";
    }
    else if ( uncaught_exception() )
    {
        // Throwing while already handling an exception would result in
        // terminate() being called and we wouldn't get any useful information
        // about why the test failed then.
        if ( s_lastAssertMessage.empty() )
        {
            abortReason << assertMessage << "while handling an exception";
        }
        else // In this case the exception is due to a previous assert.
        {
            abortReason << s_lastAssertMessage << "\n  and another "
                        << assertMessage << " while handling it.";
        }
    }
    else // Can "safely" throw from here.
    {
        // Remember this in case another assert happens while handling this
        // exception: we want to show the original assert as it's usually more
        // useful to determine the real root of the problem.
        s_lastAssertMessage = assertMessage;

        throw TestAssertFailure(file, line, func, cond, msg);
    }

    wxFputs(abortReason, stderr);
    fflush(stderr);
    _exit(-1);
}

CATCH_TRANSLATE_EXCEPTION(TestAssertFailure& e)
{
    return e.m_msg.ToStdString(wxConvUTF8);
}

#endif // wxDEBUG_LEVEL

#if wxUSE_GUI
    typedef wxApp TestAppBase;
    typedef wxGUIAppTraits TestAppTraitsBase;
#else
    typedef wxAppConsole TestAppBase;
    typedef wxConsoleAppTraits TestAppTraitsBase;
#endif

// The application class
//
class TestApp : public TestAppBase
{
public:
    TestApp();

    // standard overrides
    virtual bool OnInit() wxOVERRIDE;
    virtual int  OnExit() wxOVERRIDE;

#ifdef __WIN32__
    virtual wxAppTraits *CreateTraits() wxOVERRIDE
    {
        // Define a new class just to customize CanUseStderr() behaviour.
        class TestAppTraits : public TestAppTraitsBase
        {
        public:
            // We want to always use stderr, tests are also run unattended and
            // in this case we really don't want to show any message boxes, as
            // wxMessageOutputBest, used e.g. from the default implementation
            // of wxApp::OnUnhandledException(), would do by default.
            virtual bool CanUseStderr() wxOVERRIDE { return true; }

            // Overriding CanUseStderr() is not enough, we also need to
            // override this one to avoid returning false from it.
            virtual bool WriteToStderr(const wxString& text) wxOVERRIDE
            {
                wxFputs(text, stderr);
                fflush(stderr);

                // Intentionally ignore any errors, we really don't want to
                // show any message boxes in any case.
                return true;
            }
        };

        return new TestAppTraits;
    }
#endif // __WIN32__

    // Also override this method to avoid showing any dialogs from here -- and
    // show some details about the exception along the way.
    virtual bool OnExceptionInMainLoop() wxOVERRIDE
    {
        wxFprintf(stderr, "Unhandled exception in the main loop: %s\n",
                  Catch::translateActiveException());

        throw;
    }

    // used by events propagation test
    virtual int FilterEvent(wxEvent& event) wxOVERRIDE;
    virtual bool ProcessEvent(wxEvent& event) wxOVERRIDE;

    void SetFilterEventFunc(FilterEventFunc f) { m_filterEventFunc = f; }
    void SetProcessEventFunc(ProcessEventFunc f) { m_processEventFunc = f; }

    // In console applications we run the tests directly from the overridden
    // OnRun(), but in the GUI ones we run them when we get the first call to
    // our EVT_IDLE handler to ensure that we do everything from inside the
    // main event loop. This is especially important under wxOSX/Cocoa where
    // the main event loop is different from the others but it's also safer to
    // do it like this in the other ports as we test the GUI code in the same
    // context as it's used usually, in normal programs, and it might behave
    // differently without the event loop.
#if wxUSE_GUI
    void OnIdle(wxIdleEvent& event)
    {
        if ( m_runTests )
        {
            m_runTests = false;

#ifdef __WXOSX__
            // we need to wait until the window is activated and fully ready
            // otherwise no events can be posted
            wxEventLoopBase* const loop = wxEventLoop::GetActive();
            if ( loop )
            {
                loop->DispatchTimeout(1000);
                loop->Yield();
            }
#endif // __WXOSX__

            m_exitcode = RunTests();
            ExitMainLoop();
        }

        event.Skip();
    }

    virtual int OnRun() wxOVERRIDE
    {
        if ( TestAppBase::OnRun() != 0 )
            m_exitcode = EXIT_FAILURE;

        return m_exitcode;
    }
#else // !wxUSE_GUI
    virtual int OnRun() wxOVERRIDE
    {
        return RunTests();
    }
#endif // wxUSE_GUI/!wxUSE_GUI

private:
    int RunTests();

    // flag telling us whether we should run tests from our EVT_IDLE handler
    bool m_runTests;

    // event handling hooks
    FilterEventFunc m_filterEventFunc;
    ProcessEventFunc m_processEventFunc;

#if wxUSE_GUI
    // the program exit code
    int m_exitcode;
#endif // wxUSE_GUI
};

wxIMPLEMENT_APP_NO_MAIN(TestApp);


// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#ifdef wxUSE_VC_CRTDBG

static int TestCrtReportHook(int reportType, char *message, int *)
{
    if ( reportType != _CRT_ASSERT )
        return FALSE;

    throw CrtAssertFailure(message);
}

#endif // wxUSE_VC_CRTDBG

int main(int argc, char **argv)
{
    // tests can be ran non-interactively so make sure we don't show any assert
    // dialog boxes -- neither our own nor from MSVC debug CRT -- which would
    // prevent them from completing

#if wxDEBUG_LEVEL
    wxSetAssertHandler(TestAssertHandler);
#endif // wxDEBUG_LEVEL

#ifdef wxUSE_VC_CRTDBG
    _CrtSetReportHook(TestCrtReportHook);
#endif // wxUSE_VC_CRTDBG

    return wxEntry(argc, argv);
}

extern void SetFilterEventFunc(FilterEventFunc func)
{
    wxGetApp().SetFilterEventFunc(func);
}

extern void SetProcessEventFunc(ProcessEventFunc func)
{
    wxGetApp().SetProcessEventFunc(func);
}

extern bool IsNetworkAvailable()
{
    // Somehow even though network is available on Travis CI build machines,
    // attempts to open remote URIs sporadically fail, so don't run these tests
    // under Travis to avoid false positives.
    static int s_isTravis = -1;
    if ( s_isTravis == -1 )
        s_isTravis = wxGetEnv("TRAVIS", NULL);

    if ( s_isTravis )
        return false;

    // NOTE: we could use wxDialUpManager here if it was in wxNet; since it's in
    //       wxCore we use a simple rough test:

    wxSocketBase::Initialize();

    wxIPV4address addr;
    if (!addr.Hostname("www.google.com") || !addr.Service("www"))
    {
        wxSocketBase::Shutdown();
        return false;
    }

    wxSocketClient sock;
    sock.SetTimeout(10);    // 10 secs
    bool online = sock.Connect(addr);

    wxSocketBase::Shutdown();

    return online;
}

extern bool IsAutomaticTest()
{
    static int s_isAutomatic = -1;
    if ( s_isAutomatic == -1 )
    {
        // Allow setting an environment variable to emulate buildslave user for
        // testing.
        wxString username;
        if ( !wxGetEnv("WX_TEST_USER", &username) )
            username = wxGetUserId();

        username.MakeLower();
        s_isAutomatic = username == "buildbot" ||
                            username.Matches("sandbox*");

        // Also recognize Travis and AppVeyor CI environments.
        if ( !s_isAutomatic )
        {
            s_isAutomatic = wxGetEnv("TRAVIS", NULL) ||
                                wxGetEnv("APPVEYOR", NULL);
        }
    }

    return s_isAutomatic == 1;
}

#if wxUSE_GUI

bool EnableUITests()
{
    static int s_enabled = -1;
    if ( s_enabled == -1 )
    {
        // Allow explicitly configuring this via an environment variable under
        // all platforms.
        wxString enabled;
        if ( wxGetEnv("WX_UI_TESTS", &enabled) )
        {
            if ( enabled == "1" )
                s_enabled = 1;
            else if ( enabled == "0" )
                s_enabled = 0;
            else
                wxFprintf(stderr, "Unknown \"WX_UI_TESTS\" value \"%s\" ignored.\n", enabled);
        }

        if ( s_enabled == -1 )
        {
#ifdef __WXMSW__
            s_enabled = 1;
#else // !__WXMSW__
            s_enabled = 0;
#endif // __WXMSW__/!__WXMSW__
        }
    }

    return s_enabled == 1;
}

void DeleteTestWindow(wxWindow* win)
{
    if ( !win )
        return;

    wxWindow* const capture = wxWindow::GetCapture();
    if ( capture )
    {
        if ( capture == win ||
                capture->GetMainWindowOfCompositeControl() == win )
            capture->ReleaseMouse();
    }

    delete win;
}

#ifdef __WXGTK__

#ifdef GDK_WINDOWING_X11

#include "X11/Xlib.h"

extern "C"
int wxTestX11ErrorHandler(Display*, XErrorEvent*)
{
    fprintf(stderr, "\n*** X11 error while running %s(): ",
            wxGetCurrentTestName().c_str());
    return 0;
}

#endif // GDK_WINDOWING_X11

extern "C"
void
wxTestGLogHandler(const gchar* domain,
                  GLogLevelFlags level,
                  const gchar* message,
                  gpointer data)
{
    fprintf(stderr, "\n*** GTK log message while running %s(): ",
            wxGetCurrentTestName().c_str());

    g_log_default_handler(domain, level, message, data);

    fprintf(stderr, "\n");
}

#endif // __WXGTK__

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// TestApp
// ----------------------------------------------------------------------------

TestApp::TestApp()
{
    m_runTests = true;

    m_filterEventFunc = NULL;
    m_processEventFunc = NULL;

#if wxUSE_GUI
    m_exitcode = EXIT_SUCCESS;
#endif // wxUSE_GUI
}

// Init
//
bool TestApp::OnInit()
{
    // Hack: don't call TestAppBase::OnInit() to let CATCH handle command line.

    // Output some important information about the test environment.
#if wxUSE_GUI
    cout << "Test program for wxWidgets GUI features\n"
#else
    cout << "Test program for wxWidgets non-GUI features\n"
#endif
         << "build: " << WX_BUILD_OPTIONS_SIGNATURE << "\n"
         << "running under " << wxGetOsDescription()
         << " as " << wxGetUserId()
         << ", locale is " << setlocale(LC_ALL, NULL)
         << std::endl;

#if wxUSE_GUI
    // create a parent window to be used as parent for the GUI controls
    new wxTestableFrame();

    Connect(wxEVT_IDLE, wxIdleEventHandler(TestApp::OnIdle));

#ifdef __WXGTK20__
    g_log_set_default_handler(wxTestGLogHandler, NULL);
#endif // __WXGTK__

#ifdef GDK_WINDOWING_X11
    XSetErrorHandler(wxTestX11ErrorHandler);
#endif // GDK_WINDOWING_X11

#endif // wxUSE_GUI

    return true;
}

// Event handling
int TestApp::FilterEvent(wxEvent& event)
{
    if ( m_filterEventFunc )
        return (*m_filterEventFunc)(event);

    return TestAppBase::FilterEvent(event);
}

bool TestApp::ProcessEvent(wxEvent& event)
{
    if ( m_processEventFunc )
        return (*m_processEventFunc)(event);

    return TestAppBase::ProcessEvent(event);
}

// Run
//
int TestApp::RunTests()
{
#if wxUSE_LOG
    // Switch off logging unless --verbose
    bool verbose = wxLog::GetVerbose();
    wxLog::EnableLogging(verbose);
#else
    bool verbose = false;
#endif

    // Cast is needed under MSW where Catch also provides an overload taking
    // wchar_t, but as it simply converts arguments to char internally anyhow,
    // we can just as well always use the char version.
    return Catch::Session().run(argc, static_cast<char**>(argv));
}

int TestApp::OnExit()
{
#if wxUSE_GUI
    delete GetTopWindow();
#endif // wxUSE_GUI

    return TestAppBase::OnExit();
}
