/////////////////////////////////////////////////////////////////////////////
// Name:        wx/stopwatch.h
// Purpose:     wxStopWatch and global time-related functions
// Author:      Julian Smart (wxTimer), Sylvain Bougnoux (wxStopWatch)
// Created:     26.06.03 (extracted from wx/timer.h)
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2003 Julian Smart, Sylvain Bougnoux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STOPWATCH_H_
#define _WX_STOPWATCH_H_

#include "wx/defs.h"
#include "wx/longlong.h"

// ----------------------------------------------------------------------------
// wxStopWatch: measure time intervals with up to 1ms resolution
// ----------------------------------------------------------------------------

#if wxUSE_STOPWATCH

class WXDLLIMPEXP_BASE wxStopWatch
{
public:
    // ctor starts the stop watch
    wxStopWatch() { m_pauseCount = 0; Start(); }

    // Start the stop watch at the moment t0 expressed in milliseconds (i.e.
    // calling Time() immediately afterwards returns t0). This can be used to
    // restart an existing stopwatch.
    void Start(long t0 = 0);

    // pause the stop watch
    void Pause()
    {
        if ( m_pauseCount++ == 0 )
            m_elapsedBeforePause = GetCurrentClockValue() - m_t0;
    }

    // resume it
    void Resume()
    {
        wxASSERT_MSG( m_pauseCount > 0,
                      wxT("Resuming stop watch which is not paused") );

        if ( --m_pauseCount == 0 )
        {
            DoStart();
            m_t0 -= m_elapsedBeforePause;
        }
    }

    // Get elapsed time since the last Start() in microseconds.
    wxLongLong TimeInMicro() const;

    // get elapsed time since the last Start() in milliseconds
    long Time() const { return (TimeInMicro()/1000).ToLong(); }

private:
    // Really starts the stop watch. The initial time is set to current clock
    // value.
    void DoStart();

    // Returns the current clock value in its native units.
    wxLongLong GetCurrentClockValue() const;

    // Return the frequency of the clock used in its ticks per second.
    wxLongLong GetClockFreq() const;


    // The clock value when the stop watch was last started. Its units vary
    // depending on the platform.
    wxLongLong m_t0;

    // The elapsed time as of last Pause() call (only valid if m_pauseCount >
    // 0) in the same units as m_t0.
    wxLongLong m_elapsedBeforePause;

    // if > 0, the stop watch is paused, otherwise it is running
    int m_pauseCount;
};

#endif // wxUSE_STOPWATCH

#if wxUSE_LONGLONG && WXWIN_COMPATIBILITY_2_6

    // Starts a global timer
    // -- DEPRECATED: use wxStopWatch instead
    wxDEPRECATED( void WXDLLIMPEXP_BASE wxStartTimer() );

    // Gets elapsed milliseconds since last wxStartTimer or wxGetElapsedTime
    // -- DEPRECATED: use wxStopWatch instead
    wxDEPRECATED( long WXDLLIMPEXP_BASE wxGetElapsedTime(bool resetTimer = true) );

#endif // wxUSE_LONGLONG && WXWIN_COMPATIBILITY_2_6

// ----------------------------------------------------------------------------
// global time functions
// ----------------------------------------------------------------------------

// Get number of seconds since local time 00:00:00 Jan 1st 1970.
extern long WXDLLIMPEXP_BASE wxGetLocalTime();

// Get number of seconds since GMT 00:00:00, Jan 1st 1970.
extern long WXDLLIMPEXP_BASE wxGetUTCTime();

#if wxUSE_LONGLONG
    typedef wxLongLong wxMilliClock_t;
    inline long wxMilliClockToLong(wxLongLong ll) { return ll.ToLong(); }
#else
    typedef double wxMilliClock_t;
    inline long wxMilliClockToLong(double d) { return wx_truncate_cast(long, d); }
#endif // wxUSE_LONGLONG

// Get number of milliseconds since local time 00:00:00 Jan 1st 1970
extern wxMilliClock_t WXDLLIMPEXP_BASE wxGetLocalTimeMillis();

#define wxGetCurrentTime() wxGetLocalTime()

// on some really old systems gettimeofday() doesn't have the second argument,
// define wxGetTimeOfDay() to hide this difference
#ifdef HAVE_GETTIMEOFDAY
    #ifdef WX_GETTIMEOFDAY_NO_TZ
        #define wxGetTimeOfDay(tv)      gettimeofday(tv)
    #else
        #define wxGetTimeOfDay(tv)      gettimeofday((tv), NULL)
    #endif
#endif // HAVE_GETTIMEOFDAY

#endif // _WX_STOPWATCH_H_
