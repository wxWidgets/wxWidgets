///////////////////////////////////////////////////////////////////////////////
// Name:        tests/log/logtest.cpp
// Purpose:     wxLog unit test
// Author:      Vadim Zeitlin
// Created:     2009-07-07
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/filefn.h"
#endif // WX_PRECOMP

#include "wx/scopeguard.h"

#if wxUSE_LOG

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
#else
    #include <errno.h>
#endif

// all calls to wxLogXXX() functions from this file will use this log component
#define wxLOG_COMPONENT "test"

// ----------------------------------------------------------------------------
// test loggers
// ----------------------------------------------------------------------------

// base class for all test loggers which simply store all logged messages for
// future examination in the test code
class TestLogBase : public wxLog
{
public:
    TestLogBase() { }

    const wxString& GetLog(wxLogLevel level) const
    {
        return m_logs[level];
    }

    const wxLogRecordInfo& GetInfo(wxLogLevel level) const
    {
        return m_logsInfo[level];
    }

    void Clear()
    {
        for ( unsigned n = 0; n < WXSIZEOF(m_logs); n++ )
        {
            m_logs[n].clear();
            m_logsInfo[n] = wxLogRecordInfo();
        }
    }

protected:
    wxString m_logs[wxLOG_Trace + 1];
    wxLogRecordInfo m_logsInfo[wxLOG_Trace + 1];

    wxDECLARE_NO_COPY_CLASS(TestLogBase);
};

// simple log sink which just stores the messages logged for each level
class TestLog : public TestLogBase
{
public:
    TestLog() { }

protected:
    virtual void DoLogRecord(wxLogLevel level,
                             const wxString& msg,
                             const wxLogRecordInfo& info) override
    {
        m_logs[level] = msg;
        m_logsInfo[level] = info;
    }

private:
    wxDECLARE_NO_COPY_CLASS(TestLog);
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class LogTestCase : public CppUnit::TestCase
{
public:
    LogTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( LogTestCase );
        CPPUNIT_TEST( Functions );
        CPPUNIT_TEST( Null );
        CPPUNIT_TEST( Component );
#if wxDEBUG_LEVEL
        CPPUNIT_TEST( Trace );
#endif // wxDEBUG_LEVEL
        CPPUNIT_TEST( SysError );
        CPPUNIT_TEST( NoWarnings );
    CPPUNIT_TEST_SUITE_END();

    void Functions();
    void Null();
    void Component();
#if wxDEBUG_LEVEL
    void Trace();
#endif // wxDEBUG_LEVEL
    void SysError();
    void NoWarnings();

    TestLog *m_log;
    wxLog *m_logOld;
    bool m_logWasEnabled;

    wxDECLARE_NO_COPY_CLASS(LogTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( LogTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( LogTestCase, "LogTestCase" );

void LogTestCase::setUp()
{
    m_logOld = wxLog::SetActiveTarget(m_log = new TestLog);
    m_logWasEnabled = wxLog::EnableLogging();
}

void LogTestCase::tearDown()
{
    delete wxLog::SetActiveTarget(m_logOld);
    wxLog::EnableLogging(m_logWasEnabled);
}

void LogTestCase::Functions()
{
    wxLogMessage("Message");
    CPPUNIT_ASSERT_EQUAL( "Message", m_log->GetLog(wxLOG_Message) );

    wxLogError("Error %d", 17);
    CPPUNIT_ASSERT_EQUAL( "Error 17", m_log->GetLog(wxLOG_Error) );

    wxLogDebug("Debug");
#if wxDEBUG_LEVEL
    CPPUNIT_ASSERT_EQUAL( "Debug", m_log->GetLog(wxLOG_Debug) );
#else
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Debug) );
#endif
}

void LogTestCase::Null()
{
    {
        wxLogNull noLog;
        wxLogWarning("%s warning", "Not important");

        CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Warning) );
    }

    wxLogWarning("%s warning", "Important");
    CPPUNIT_ASSERT_EQUAL( "Important warning", m_log->GetLog(wxLOG_Warning) );
}

void LogTestCase::Component()
{
    wxLogMessage("Message");
    CPPUNIT_ASSERT_EQUAL( std::string(wxLOG_COMPONENT),
                          m_log->GetInfo(wxLOG_Message).component );

    // completely disable logging for this component
    wxLog::SetComponentLevel("test/ignore", wxLOG_FatalError);

    // but enable it for one of its subcomponents
    wxLog::SetComponentLevel("test/ignore/not", wxLOG_Max);

    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore"

    // this shouldn't be output as this component is ignored
    wxLogError("Error");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Error) );

    // and so are its subcomponents
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore/sub/subsub"
    wxLogError("Error");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Error) );

    // but one subcomponent is not
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test/ignore/not"
    wxLogError("Error");
    CPPUNIT_ASSERT_EQUAL( "Error", m_log->GetLog(wxLOG_Error) );

    // restore the original value
    #undef wxLOG_COMPONENT
    #define wxLOG_COMPONENT "test"
}

#if wxDEBUG_LEVEL

namespace
{

const char *TEST_MASK = "test";

// this is a test vararg function (a real one, not a variadic-template-like as
// wxVLogTrace(), so care should be taken with its arguments)
void TraceTest(const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    wxVLogTrace(TEST_MASK, format, argptr);
    va_end(argptr);
}

} // anonymous namespace

void LogTestCase::Trace()
{
    // we use wxLogTrace() or wxVLogTrace() from inside TraceTest()
    // interchangeably here, it shouldn't make any difference

    wxLogTrace(TEST_MASK, "Not shown");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Trace) );

    wxLog::AddTraceMask(TEST_MASK);
    TraceTest("Shown");
    CPPUNIT_ASSERT_EQUAL( wxString::Format("(%s) Shown", TEST_MASK),
                          m_log->GetLog(wxLOG_Trace) );

    wxLog::RemoveTraceMask(TEST_MASK);
    m_log->Clear();

    TraceTest("Not shown again");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Trace) );
}

#endif // wxDEBUG_LEVEL

void LogTestCase::SysError()
{
    wxString s;

    wxLogSysError(17, "Error");
    CPPUNIT_ASSERT( m_log->GetLog(wxLOG_Error).StartsWith("Error (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 17") );

    // Try to ensure that the system error is 0.
#ifdef __WINDOWS__
    ::SetLastError(0);
#else
    errno = 0;
#endif

    wxLogSysError("Success");
    CPPUNIT_ASSERT( m_log->GetLog(wxLOG_Error).StartsWith("Success (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 0") );

    wxOpen("no-such-file", 0, 0);
    wxLogSysError("Not found");
    CPPUNIT_ASSERT( m_log->GetLog(wxLOG_Error).StartsWith("Not found (", &s) );
    WX_ASSERT_MESSAGE( ("Error message is \"(%s\"", s), s.StartsWith("error 2") );
}

void LogTestCase::NoWarnings()
{
    // Check that "else" branch is [not] taken as expected and that this code
    // compiles without warnings (which used to not be the case).

    bool b = wxFalse;
    if ( b )
        wxLogError("Not logged");
    else
        b = !b;

    CPPUNIT_ASSERT( b );

    if ( b )
        wxLogError("If");
    else
        CPPUNIT_FAIL("Should not be taken");

    CPPUNIT_ASSERT_EQUAL( "If", m_log->GetLog(wxLOG_Error) );
}

// The following two functions (v, macroCompilabilityTest) are not run by
// any test, and their purpose is merely to guarantee that the wx(V)LogXXX
// macros compile without 'dangling else' warnings.
#if defined(__clang__) || defined(__GNUC__)
    // gcc 7 split -Wdangling-else from the much older -Wparentheses, so use
    // the new warning if it's available or the old one otherwise.
    #if wxCHECK_GCC_VERSION(7, 0)
        #pragma GCC diagnostic error "-Wdangling-else"
    #else
        #pragma GCC diagnostic error "-Wparentheses"
    #endif
#endif

static void v(int x, ...)
{
    va_list list;
    va_start(list, x);

    if (true)
        wxVLogGeneric(Info, "vhello generic %d", list);
    if (true)
        wxVLogTrace(wxTRACE_Messages, "vhello trace %d", list);
    if (true)
        wxVLogError("vhello error %d", list);
    if (true)
        wxVLogMessage("vhello message %d", list);
    if (true)
        wxVLogVerbose("vhello verbose %d", list);
    if (true)
        wxVLogWarning("vhello warning %d", list);
    if (true)
        wxVLogFatalError("vhello fatal %d", list);
    if (true)
        wxVLogSysError("vhello syserror %d", list);
    if (true)
        wxVLogDebug("vhello debug %d", list);

    va_end(list);
}

void macroCompilabilityTest()
{
    v(123, 456);
    if (true)
        wxLogGeneric(wxLOG_Info, "hello generic %d", 42);
    if (true)
        wxLogTrace(wxTRACE_Messages, "hello trace %d", 42);
    if (true)
        wxLogError("hello error %d", 42);
    if (true)
        wxLogMessage("hello message %d", 42);
    if (true)
        wxLogVerbose("hello verbose %d", 42);
    if (true)
        wxLogWarning("hello warning %d", 42);
    if (true)
        wxLogFatalError("hello fatal %d", 42);
    if (true)
        wxLogSysError("hello syserror %d", 42);
    if (true)
        wxLogDebug("hello debug %d", 42);
}

// This allows to check wxLogTrace() interactively by running this test with
// WXTRACE=logtest.
TEST_CASE("wxLog::Trace", "[log][.]")
{
    // Running this test without setting WXTRACE is useless.
    REQUIRE( wxGetEnv("WXTRACE", nullptr) );

    wxLogTrace("logtest", "Starting test");
    wxMilliSleep(250);
    wxLogTrace("logtest", "Ending test 1/4s later");
}

#endif // wxUSE_LOG
