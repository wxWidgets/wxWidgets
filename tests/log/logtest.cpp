///////////////////////////////////////////////////////////////////////////////
// Name:        tests/log/logtest.cpp
// Purpose:     wxLog unit test
// Author:      Vadim Zeitlin
// Created:     2009-07-07
// RCS-ID:      $Id$
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
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// test logger
// ----------------------------------------------------------------------------

// simple log sink which just stores the messages logged for each level
class TestLog : public wxLog
{
public:
    TestLog() { }

    wxString GetLog(wxLogLevel level) const
    {
        return m_logs[level];
    }

    void Clear()
    {
        for ( unsigned n = 0; n < WXSIZEOF(m_logs); n++ )
            m_logs[n].clear();
    }

protected:
    virtual void DoLog(wxLogLevel level, const wxString& str, time_t WXUNUSED(t))
    {
        m_logs[level] = str;
    }

    wxSUPPRESS_DOLOG_HIDE_WARNING()

private:
    wxString m_logs[wxLOG_Trace + 1];

    wxDECLARE_NO_COPY_CLASS(TestLog);
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class LogTestCase : public CppUnit::TestCase
{
public:
    LogTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( LogTestCase );
        CPPUNIT_TEST( Functions );
        CPPUNIT_TEST( Null );
#if wxDEBUG_LEVEL
        CPPUNIT_TEST( Trace );
#endif // wxDEBUG_LEVEL
    CPPUNIT_TEST_SUITE_END();

    void Functions();
    void Null();
#if wxDEBUG_LEVEL
    void Trace();
#endif // wxDEBUG_LEVEL

    TestLog *m_log;
    wxLog *m_logOld;
    bool m_logWasEnabled;

    wxDECLARE_NO_COPY_CLASS(LogTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( LogTestCase );

// also include in it's own registry so that these tests can be run alone
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

#if wxDEBUG_LEVEL

void LogTestCase::Trace()
{
    static const char *TEST_MASK = "test";

    wxLogTrace(TEST_MASK, "Not shown");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Trace) );

    wxLog::AddTraceMask(TEST_MASK);
    wxLogTrace(TEST_MASK, "Shown");
    CPPUNIT_ASSERT_EQUAL( wxString::Format("(%s) Shown", TEST_MASK),
                          m_log->GetLog(wxLOG_Trace) );

    wxLog::RemoveTraceMask(TEST_MASK);
    m_log->Clear();

    wxLogTrace(TEST_MASK, "Not shown again");
    CPPUNIT_ASSERT_EQUAL( "", m_log->GetLog(wxLOG_Trace) );
}

#endif // wxDEBUG_LEVEL
