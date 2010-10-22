///////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     Test program for wxWidgets
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h"
// and "wx/cppunit.h"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/beforestd.h"
#ifdef __VISUALC__
    #pragma warning(disable:4100)
#endif

#include <cppunit/TestListener.h>
#include <cppunit/Protector.h>
#include <cppunit/Test.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFailure.h>
#include <cppunit/TestResultCollector.h>

#ifdef __VISUALC__
    #pragma warning(default:4100)
#endif
#include "wx/afterstd.h"

#include "wx/cmdline.h"
#include <exception>
#include <iostream>

#ifdef __WXMSW__
    #include "wx/msw/msvcrt.h"
#endif

#ifdef __WXOSX__
    #include "wx/osx/private.h"
#endif

#if wxUSE_GUI
    #include "testableframe.h"
#endif

#include "wx/socket.h"
#include "wx/evtloop.h"

using namespace std;

using CppUnit::Test;
using CppUnit::TestSuite;
using CppUnit::TestFactoryRegistry;


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

#endif // wxUSE_VC_CRTDBG

#if wxDEBUG_LEVEL

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
    if ( !wxIsMainThread() )
    {
        // Exceptions thrown from worker threads are not caught currently and
        // so we'd just die without any useful information -- abort instead.
        abortReason = "in a worker thread";
    }
    else if ( uncaught_exception() )
    {
        // Throwing while already handling an exception would result in
        // terminate() being called and we wouldn't get any useful information
        // about why the test failed then.
        abortReason = "while handling an exception";
    }
    else // Can "safely" throw from here.
    {
        throw TestAssertFailure(file, line, func, cond, msg);
    }

    wxFprintf(stderr, "%s %s -- aborting.",
              FormatAssertMessage(file, line, func, cond, msg),
              abortReason);
    fflush(stderr);
    _exit(-1);
}

#endif // wxDEBUG_LEVEL

// this function should only be called from a catch clause
static string GetExceptionMessage()
{
    wxString msg;

    try
    {
        throw;
    }
#if wxDEBUG_LEVEL
    catch ( TestAssertFailure& e )
    {
        msg << FormatAssertMessage(e.m_file, e.m_line, e.m_func,
                                   e.m_cond, e.m_msg);
    }
#endif // wxDEBUG_LEVEL
#ifdef wxUSE_VC_CRTDBG
    catch ( CrtAssertFailure& e )
    {
        msg << "CRT assert failure: " << e.m_msg;
    }
#endif // wxUSE_VC_CRTDBG
    catch ( std::exception& e )
    {
        msg << "std::exception: " << e.what();
    }
    catch ( ... )
    {
        msg = "Unknown exception caught.";
    }

    return string(msg.mb_str());
}

// Protector adding handling of wx-specific (this includes MSVC debug CRT in
// this context) exceptions
class wxUnitTestProtector : public CppUnit::Protector
{
public:
    virtual bool protect(const CppUnit::Functor &functor,
                         const CppUnit::ProtectorContext& context)
    {
        try
        {
            return functor();
        }
        catch ( std::exception& )
        {
            // cppunit deals with the standard exceptions itself, let it do as
            // it output more details (especially for std::exception-derived
            // CppUnit::Exception) than we do
            throw;
        }
        catch ( ... )
        {
            reportError(context, CppUnit::Message("Uncaught exception",
                                                  GetExceptionMessage()));
        }

        return false;
    }
};

// Displays the test name before starting to execute it: this helps with
// diagnosing where exactly does a test crash or hang when/if it does.
class DetailListener : public CppUnit::TestListener
{
public:
    DetailListener(bool doTiming = false):
        CppUnit::TestListener(),
        m_timing(doTiming)
    {
    }

    virtual void startTest(CppUnit::Test *test)
    {
        wxPrintf("  %-60s  ", test->getName());
        m_result = RESULT_OK;
        m_watch.Start();
    }

    virtual void addFailure(const CppUnit::TestFailure& failure)
    {
        m_result = failure.isError() ? RESULT_ERROR : RESULT_FAIL;
    }

    virtual void endTest(CppUnit::Test * WXUNUSED(test))
    {
        m_watch.Pause();
        wxPrintf(GetResultStr(m_result));
        if (m_timing)
            wxPrintf("  %6ld ms", m_watch.Time());
        wxPrintf("\n");
    }

protected :
    enum ResultType
    {
        RESULT_OK = 0,
        RESULT_FAIL,
        RESULT_ERROR,
        RESULT_MAX
    };

    wxString GetResultStr(ResultType type) const
    {
        static const char *resultTypeNames[] =
        {
            "  OK",
            "FAIL",
            " ERR"
        };

        wxCOMPILE_TIME_ASSERT( WXSIZEOF(resultTypeNames) == RESULT_MAX,
                               ResultTypeNamesMismatch );

        return resultTypeNames[type];
    }

    bool m_timing;
    wxStopWatch m_watch;
    ResultType m_result;
};

#if wxUSE_GUI
    typedef wxApp TestAppBase;
#else
    typedef wxAppConsole TestAppBase;
#endif

// The application class
//
class TestApp : public TestAppBase
{
public:
    TestApp();

    // standard overrides
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual bool OnInit();
    virtual int  OnRun();
    virtual int  OnExit();

    // used by events propagation test
    virtual int FilterEvent(wxEvent& event);
    virtual bool ProcessEvent(wxEvent& event);

    void SetFilterEventFunc(FilterEventFunc f) { m_filterEventFunc = f; }
    void SetProcessEventFunc(ProcessEventFunc f) { m_processEventFunc = f; }

private:
    void List(Test *test, const string& parent = "") const;

    // call List() if m_list or runner.addTest() otherwise
    void AddTest(CppUnit::TestRunner& runner, Test *test)
    {
        if (m_list)
            List(test);
        else
            runner.addTest(test);
    }

    // command lines options/parameters
    bool m_list;
    bool m_longlist;
    bool m_detail;
    bool m_timing;
    wxArrayString m_registries;
    wxLocale *m_locale;

    // event loop for GUI tests
    wxEventLoop* m_eventloop;

    // event handling hooks
    FilterEventFunc m_filterEventFunc;
    ProcessEventFunc m_processEventFunc;
};

IMPLEMENT_APP_NO_MAIN(TestApp)


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

    try
    {
        return wxEntry(argc, argv);
    }
    catch ( ... )
    {
        cerr << "\n" << GetExceptionMessage() << endl;
    }

    return -1;
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

// helper of OnRun(): gets the test with the given name, returning NULL (and
// not an empty test suite) if there is no such test
static Test *GetTestByName(const wxString& name)
{
    Test *
      test = TestFactoryRegistry::getRegistry(string(name.mb_str())).makeTest();
    if ( test )
    {
        TestSuite * const suite = dynamic_cast<TestSuite *>(test);
        if ( !suite || !suite->countTestCases() )
        {
            // it's a bogus test, don't use it
            delete test;
            test = NULL;
        }
    }

    return test;
}


// ----------------------------------------------------------------------------
// TestApp
// ----------------------------------------------------------------------------

TestApp::TestApp()
  : m_list(false),
    m_longlist(false)
{
    m_filterEventFunc = NULL;
    m_processEventFunc = NULL;

    m_locale = NULL;
    m_eventloop = NULL;
}

// Init
//
bool TestApp::OnInit()
{
    if ( !TestAppBase::OnInit() )
        return false;

#if wxUSE_GUI
    cout << "Test program for wxWidgets GUI features\n"
#else
    cout << "Test program for wxWidgets non-GUI features\n"
#endif
         << "build: " << WX_BUILD_OPTIONS_SIGNATURE << std::endl;

#if wxUSE_GUI
    // create a hidden parent window to be used as parent for the GUI controls
    wxTestableFrame* frame = new wxTestableFrame();
    frame->Show();

    m_eventloop = new wxEventLoop;
    wxEventLoop::SetActive(m_eventloop);
#endif // wxUSE_GUI

    return true;
}

// The table of command line options
//
void TestApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    TestAppBase::OnInitCmdLine(parser);

    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        { wxCMD_LINE_SWITCH, "l", "list",
            "list the test suites, do not run them",
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_SWITCH, "L", "longlist",
            "list the test cases, do not run them",
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_SWITCH, "d", "detail",
            "print the test case names, run them",
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_SWITCH, "t", "timing",
            "print names and measure running time of individual test, run them",
            wxCMD_LINE_VAL_NONE, 0 },
        { wxCMD_LINE_OPTION, "", "locale",
            "locale to use when running the program",
            wxCMD_LINE_VAL_STRING, 0 },
        { wxCMD_LINE_PARAM, NULL, NULL, "REGISTRY", wxCMD_LINE_VAL_STRING,
            wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE },
        wxCMD_LINE_DESC_END
    };

    parser.SetDesc(cmdLineDesc);
}

// Handle command line options
//
bool TestApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.GetParamCount())
    {
        for (size_t i = 0; i < parser.GetParamCount(); i++)
            m_registries.push_back(parser.GetParam(i));
    }

    m_longlist = parser.Found("longlist");
    m_list = m_longlist || parser.Found("list");
    m_timing = parser.Found("timing");
    m_detail = !m_timing && parser.Found("detail");

    wxString loc;
    if ( parser.Found("locale", &loc) )
    {
        const wxLanguageInfo * const info = wxLocale::FindLanguageInfo(loc);
        if ( !info )
        {
            cerr << "Locale \"" << string(loc.mb_str()) << "\" is unknown.\n";
            return false;
        }

        m_locale = new wxLocale(info->Language);
        if ( !m_locale->IsOk() )
        {
            cerr << "Using locale \"" << string(loc.mb_str()) << "\" failed.\n";
            return false;
        }
    }

    return TestAppBase::OnCmdLineParsed(parser);
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
int TestApp::OnRun()
{
#if wxUSE_GUI
#ifdef __WXOSX__
    // make sure there's always an autorelease pool ready
    wxMacAutoreleasePool autoreleasepool;
#endif
#endif

#if wxUSE_LOG
    // Switch off logging unless --verbose
    bool verbose = wxLog::GetVerbose();
    wxLog::EnableLogging(verbose);
#else
    bool verbose = false;
#endif

    CppUnit::TextTestRunner runner;

    if ( m_registries.empty() )
    {
        // run or list all tests which use the CPPUNIT_TEST_SUITE_REGISTRATION() macro
        // (i.e. those registered in the "All tests" registry); if there are other
        // tests not registered with the CPPUNIT_TEST_SUITE_REGISTRATION() macro
        // then they won't be listed/run!
        AddTest(runner, TestFactoryRegistry::getRegistry().makeTest());

        if (m_list)
        {
            cout << "\nNote that the list above is not complete as it doesn't include the \n";
            cout << "tests disabled by default.\n";
        }
    }
    else // run only the selected tests
    {
        for (size_t i = 0; i < m_registries.size(); i++)
        {
            const wxString reg = m_registries[i];
            Test *test = GetTestByName(reg);

            if ( !test && !reg.EndsWith("TestCase") )
            {
                test = GetTestByName(reg + "TestCase");
            }

            if ( !test )
            {
                cerr << "No such test suite: " << string(reg.mb_str()) << endl;
                return 2;
            }

            AddTest(runner, test);
        }
    }

    if ( m_list )
        return EXIT_SUCCESS;

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), cout));

    // there is a bug
    // (http://sf.net/tracker/index.php?func=detail&aid=1649369&group_id=11795&atid=111795)
    // in some versions of cppunit: they write progress dots to cout (and not
    // cerr) and don't flush it so all the dots appear at once at the end which
    // is not very useful so unbuffer cout to work around this
    cout.setf(ios::unitbuf);

    // add detail listener if needed
    DetailListener detailListener(m_timing);
    if ( m_detail || m_timing )
        runner.eventManager().addListener(&detailListener);

    // finally ensure that we report our own exceptions nicely instead of
    // giving "uncaught exception of unknown type" messages
    runner.eventManager().pushProtector(new wxUnitTestProtector);

    bool printProgress = !(verbose || m_detail || m_timing);
    runner.run("", false, true, printProgress);

    return runner.result().testFailures() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int TestApp::OnExit()
{
    delete m_locale;

#if wxUSE_GUI
    delete GetTopWindow();
    wxEventLoop::SetActive(NULL);
    delete m_eventloop;
#endif // wxUSE_GUI

    return 0;
}

// List the tests
//
void TestApp::List(Test *test, const string& parent /*=""*/) const
{
    TestSuite *suite = dynamic_cast<TestSuite*>(test);
    string name;

    if (suite) {
        // take the last component of the name and append to the parent
        name = test->getName();
        string::size_type i = name.find_last_of(".:");
        if (i != string::npos)
            name = name.substr(i + 1);
        name = parent + "." + name;

        // drop the 1st component from the display and indent
        if (parent != "") {
            string::size_type j = i = name.find('.', 1);
            while ((j = name.find('.', j + 1)) != string::npos)
                cout << "  ";
            cout << "  " << name.substr(i + 1) << "\n";
        }

        typedef vector<Test*> Tests;
        typedef Tests::const_iterator Iter;

        const Tests& tests = suite->getTests();

        for (Iter it = tests.begin(); it != tests.end(); ++it)
            List(*it, name);
    }
    else if (m_longlist) {
        string::size_type i = 0;
        while ((i = parent.find('.', i + 1)) != string::npos)
            cout << "  ";
        cout << "  " << test->getName() << "\n";
    }
}
