///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/stopwatch.cpp
// Purpose:     wxStopWatch and other non-GUI stuff from wx/timer.h
// Author:
//    Original version by Julian Smart
//    Vadim Zeitlin got rid of all ifdefs (11.12.99)
//    Sylvain Bougnoux added wxStopWatch class
//    Guillermo Rodriguez <guille@iies.es> rewrote from scratch (Dic/99)
// Created:     20.06.2003 (extracted from common/timercmn.cpp)
// Copyright:   (c) 1998-2003 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/stopwatch.h"

#if wxUSE_STOPWATCH

#ifndef WX_PRECOMP
    #ifdef __WINDOWS__
        #include "wx/msw/wrapwin.h"
    #endif
    #include "wx/log.h"
    #include "wx/thread.h"
#endif //WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStopWatch
// ----------------------------------------------------------------------------

namespace
{

#ifdef __WINDOWS__

struct PerfCounter
{
    PerfCounter()
    {
        init = false;
    }

    wxCRIT_SECT_DECLARE_MEMBER(cs);
    LARGE_INTEGER freq;
    bool init;
};

// Return the global perf counter state.
//
// This is wrapped in a function to avoid initialization order problems,
// otherwise simply creating a global wxStopWatch variable could crash because
// it would be using a (possibly) still uninitialized critical section.
PerfCounter& GetPerfCounterState()
{
    static PerfCounter s_perfCounter;

    return s_perfCounter;
}

#endif // __WINDOWS__

const int MILLISECONDS_PER_SECOND = 1000;
const int MICROSECONDS_PER_SECOND = 1000*1000;

} // anonymous namespace

void wxStopWatch::DoStart()
{
#ifdef __WINDOWS__
    PerfCounter& perfCounter = GetPerfCounterState();
    if ( !perfCounter.init )
    {
        wxCRIT_SECT_LOCKER(lock, perfCounter.cs);
        ::QueryPerformanceFrequency(&perfCounter.freq);

        perfCounter.init = true;
    }
#endif // __WINDOWS__

    m_t0 = GetCurrentClockValue();
}

wxLongLong wxStopWatch::GetClockFreq() const
{
#ifdef __WINDOWS__
    // Under MSW we use the high resolution performance counter timer which has
    // its own frequency (usually related to the CPU clock speed).
    return GetPerfCounterState().freq.QuadPart;
#elif defined(HAVE_GETTIMEOFDAY)
    // With gettimeofday() we can have nominally microsecond precision and
    // while this is not the case in practice, it's still better than
    // millisecond.
    return MICROSECONDS_PER_SECOND;
#else // !HAVE_GETTIMEOFDAY
    // Currently milliseconds are used everywhere else.
    return MILLISECONDS_PER_SECOND;
#endif // __WINDOWS__/HAVE_GETTIMEOFDAY/else
}

void wxStopWatch::Start(long t0)
{
    // Calling Start() makes the stop watch run however many times it was
    // paused before.
    m_pauseCount = 0;

    DoStart();

    m_t0 -= (wxLongLong(t0)*GetClockFreq())/MILLISECONDS_PER_SECOND;
}

wxLongLong wxStopWatch::GetCurrentClockValue() const
{
#ifdef __WINDOWS__
    LARGE_INTEGER counter;
    ::QueryPerformanceCounter(&counter);
    return counter.QuadPart;
#elif defined(HAVE_GETTIMEOFDAY)
    return wxGetUTCTimeUSec();
#else // !HAVE_GETTIMEOFDAY
    return wxGetUTCTimeMillis();
#endif // __WINDOWS__/HAVE_GETTIMEOFDAY/else
}

wxLongLong wxStopWatch::TimeInMicro() const
{
    const wxLongLong elapsed(m_pauseCount ? m_elapsedBeforePause
                                          : GetCurrentClockValue() - m_t0);

    return (elapsed*MICROSECONDS_PER_SECOND)/GetClockFreq();
}

#endif // wxUSE_STOPWATCH

// ----------------------------------------------------------------------------
// old timer functions superceded by wxStopWatch
// ----------------------------------------------------------------------------

static wxLongLong wxStartTime = 0l;

// starts the global timer
void wxStartTimer()
{
    wxStartTime = wxGetUTCTimeMillis();
}

// Returns elapsed time in milliseconds
long wxGetElapsedTime(bool resetTimer)
{
    wxLongLong oldTime = wxStartTime;
    wxLongLong newTime = wxGetUTCTimeMillis();

    if ( resetTimer )
        wxStartTime = newTime;

    return (newTime - oldTime).GetLo();
}
