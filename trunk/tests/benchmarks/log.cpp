/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/log.cpp
// Purpose:     Log-related benchmarks
// Author:      Vadim Zeitlin
// Created:     2012-01-21
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "bench.h"

#include "wx/log.h"

// This class is used to check that the arguments of log functions are not
// evaluated.
struct NotCreated
{
    NotCreated() { wxAbort(); }

    const char* AsStr() const { return "unreachable"; }
};

// Temporarily change the log level to the given one.
class LogLevelSetter
{
public:
    LogLevelSetter(wxLogLevel levelNew)
        : m_levelOld(wxLog::GetLogLevel())
    {
        wxLog::SetLogLevel(levelNew);
    }

    ~LogLevelSetter()
    {
        wxLog::SetLogLevel(m_levelOld);
    }

private:
    const wxLogLevel m_levelOld;

    wxDECLARE_NO_COPY_CLASS(LogLevelSetter);
};

BENCHMARK_FUNC(LogDebugDisabled)
{
    LogLevelSetter level(wxLOG_Info);

    wxLogDebug("Ignored debug message: %s", NotCreated().AsStr());

    return true;
}

BENCHMARK_FUNC(LogTraceDisabled)
{
    LogLevelSetter level(wxLOG_Info);

    wxLogTrace("", NotCreated().AsStr());

    return true;
}

BENCHMARK_FUNC(LogTraceActive)
{
    static bool s_added = false;
    if ( !s_added )
    {
        s_added = true;
        wxLog::AddTraceMask("logbench");
    }

    // Remove the actual logging overhead by simply throwing away the log
    // messages.
    class NulLog : public wxLog
    {
    public:
        NulLog()
            : m_logOld(wxLog::SetActiveTarget(this))
        {
        }

        virtual ~NulLog()
        {
            wxLog::SetActiveTarget(m_logOld);
        }

    protected:
        virtual void DoLogRecord(wxLogLevel,
                                 const wxString&,
                                 const wxLogRecordInfo&)
        {
        }

        wxLog* m_logOld;
    };

    NulLog nulLog;

    wxLogTrace("logbench", "Trace message");

    return true;
}

BENCHMARK_FUNC(LogTraceInactive)
{
    wxLogTrace("bloordyblop", "Trace message");

    return true;
}
