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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/filefn.h"
#endif // WX_PRECOMP

#include "wx/scopeguard.h"

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
#else
    #include <errno.h>
#endif

#if WXWIN_COMPATIBILITY_2_8
    // we override deprecated DoLog() and DoLogString() in this test, suppress
    // warnings about it
    #ifdef __VISUALC__
        #pragma warning(disable: 4996)
    #endif // VC++ 7+
#endif // WXWIN_COMPATIBILITY_2_8

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
                             const wxLogRecordInfo& info)
    {
        m_logs[level] = msg;
        m_logsInfo[level] = info;
    }

private:
    wxDECLARE_NO_COPY_CLASS(TestLog);
};

#if WXWIN_COMPATIBILITY_2_8

// log sink overriding the old DoLogXXX() functions should still work too

// this one overrides DoLog(char*)
class CompatTestLog : public TestLogBase
{
public:
    CompatTestLog() { }

protected:
    virtual void DoLog(wxLogLevel level, const char *str, time_t WXUNUSED(t))
    {
        m_logs[level] = str;
    }

    // get rid of the warning about hiding the other overload
    virtual void DoLog(wxLogLevel WXUNUSED(level),
                       const wchar_t *WXUNUSED(str),
                       time_t WXUNUSED(t))
    {
    }

private:
    wxDECLARE_NO_COPY_CLASS(CompatTestLog);
};

// and this one overload DoLogString(wchar_t*)
class CompatTestLog2 : public wxLog
{
public:
    CompatTestLog2() { }

    const wxString& Get() const { return m_msg; }

protected:
    virtual void DoLogString(const wchar_t *msg, time_t WXUNUSED(t))
    {
        m_msg = msg;
    }

    // get rid of the warning
    virtual void DoLogString(const char *WXUNUSED(msg), time_t WXUNUSED(t))
    {
    }

private:
    wxString m_msg;

    wxDECLARE_NO_COPY_CLASS(CompatTestLog2);
};

#endif // WXWIN_COMPATIBILITY_2_8

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class LogTestCase : public CppUnit::TestCase
{
public:
    LogTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( LogTestCase );
        CPPUNIT_TEST( Functions );
        CPPUNIT_TEST( Null );
        CPPUNIT_TEST( Component );
#if wxDEBUG_LEVEL
        CPPUNIT_TEST( Trace );
#endif // wxDEBUG_LEVEL
#if WXWIN_COMPATIBILITY_2_8
        CPPUNIT_TEST( CompatLogger );
        CPPUNIT_TEST( CompatLogger2 );
#endif // WXWIN_COMPATIBILITY_2_8
        CPPUNIT_TEST( SysError );
        CPPUNIT_TEST( NoWarnings );
    CPPUNIT_TEST_SUITE_END();

    void Functions();
    void Null();
    void Component();
#if wxDEBUG_LEVEL
    void Trace();
#endif // wxDEBUG_LEVEL
#if WXWIN_COMPATIBILITY_2_8
    void CompatLogger();
    void CompatLogger2();
#endif // WXWIN_COMPATIBILITY_2_8
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

#if WXWIN_COMPATIBILITY_2_8

void LogTestCase::CompatLogger()
{
    CompatTestLog log;
    wxLog * const logOld = wxLog::SetActiveTarget(&log);
    wxON_BLOCK_EXIT1( wxLog::SetActiveTarget, logOld );

    wxLogError("Old error");
    CPPUNIT_ASSERT_EQUAL( "Old error", log.GetLog(wxLOG_Error) );
}

void LogTestCase::CompatLogger2()
{
    CompatTestLog2 log;
    wxLog * const logOld = wxLog::SetActiveTarget(&log);
    wxON_BLOCK_EXIT1( wxLog::SetActiveTarget, logOld );

    wxLogWarning("Old warning");
    CPPUNIT_ASSERT_EQUAL( "Old warning", log.Get() );
}

#endif // WXWIN_COMPATIBILITY_2_8

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
