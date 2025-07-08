///////////////////////////////////////////////////////////////////////////////
// Name:        tests/testlog.h
// Purpose:     Helper for testing wxLogXXX() functions.
// Author:      Vadim Zeitlin
// Created:     2025-07-06 (extracted from tests/log/logtest.cpp)
// Copyright:   (c) 2009-2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_TESTLOG_H_
#define _WX_TESTS_TESTLOG_H_

#include "wx/log.h"

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
// Class to use with TEST_CASE_METHOD() for tests checking logging results.
// ----------------------------------------------------------------------------

class LogTestCase
{
public:
    LogTestCase()
        : m_log(new TestLog),
          m_logOld(wxLog::SetActiveTarget(m_log)),
          m_logWasEnabled(wxLog::EnableLogging())
    {
    }

    ~LogTestCase()
    {
        delete wxLog::SetActiveTarget(m_logOld);
        wxLog::EnableLogging(m_logWasEnabled);
    }

protected:
    TestLog* const m_log;
    wxLog* const m_logOld;
    const bool m_logWasEnabled;

    wxDECLARE_NO_COPY_CLASS(LogTestCase);
};

#endif // _WX_TESTS_TESTLOG_H_
