/////////////////////////////////////////////////////////////////////////////
// Name:        common/timercmn.cpp
// Purpose:     Common timer implementation
// Author:      Julian Smart
// Modified by: Vadim Zeitlin on 12.11.99 to get rid of all ifdefs
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "timerbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/timer.h"

// I'm told VMS is POSIX, so should have localtime()
#if defined(__WXMSW__) || defined(__VMS__) || defined(__WXPM__) || defined(__WXMAC__)
    // configure might have found it already for us
    #ifndef HAVE_LOCALTIME
        #define HAVE_LOCALTIME
    #endif
#endif

// TODO: #define WX_GMTOFF_IN_TM for Windows compilers which have it here

#if defined(__WIN32__) && !defined(WX_GMTOFF_IN_TM)
    #include <windows.h>
#endif

#if defined(HAVE_GETTIMEOFDAY)
    #include <sys/time.h>
    #include <unistd.h>
#elif defined(HAVE_LOCALTIME)
    #include <time.h>
    #ifndef __WXMAC__
        #include <sys/types.h>      // for time_t
    #endif
#elif defined(HAVE_FTIME)
    #include <sys/timeb.h>
#else
    #error "no function to find the current time on this system"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// on some really old systems gettimeofday() doesn't have the second argument,
// define wxGetTimeOfDay() to hide this difference
#ifdef HAVE_GETTIMEOFDAY
    #ifdef WX_GETTIMEOFDAY_NO_TZ
        struct timezone;
        #define wxGetTimeOfDay(tv, tz)      gettimeofday(tv)
    #else
        #define wxGetTimeOfDay(tv, tz)      gettimeofday((tv), (tz))
    #endif
#endif // HAVE_GETTIMEOFDAY

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStopWatch
// ----------------------------------------------------------------------------

void wxStopWatch::Start(long t)
{
    m_t0 = wxGetCurrentMTime() - t;

    m_pause = 0;
}

long wxStopWatch::Time() const
{
    return m_pause ? m_pause : GetElapsedTime();
}

// ----------------------------------------------------------------------------
// old timer functions superceded by wxStopWatch
// ----------------------------------------------------------------------------

static long wxStartTime = 0;

// starts the global timer
void wxStartTimer()
{
    wxStartTime = wxGetCurrentMTime();
}

// Returns elapsed time in milliseconds
long wxGetElapsedTime(bool resetTimer)
{
    long oldTime = wxStartTime;
    long newTime = wxGetCurrentMTime();

    if ( resetTimer )
        wxStartTime = newTime;

    return newTime - oldTime;
}


// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long wxGetCurrentTime()
{
    return wxGetCurrentMTime() / 1000;
}

// ----------------------------------------------------------------------------
// the functions to get the current time and timezone info
// ----------------------------------------------------------------------------

// return GMT time in millisecond
long wxGetCurrentMTime()
{
#if defined(HAVE_LOCALTIME)
    time_t t0 = time(&t0);
    if ( t0 != (time_t)-1 )
    {
        struct tm *tp = localtime(&t0);

        if ( tp )
        {
            return 1000*(60*(60*tp->tm_hour+tp->tm_min)+tp->tm_sec);
        }
    }
#elif defined(HAVE_GETTIMEOFDAY)
    struct timeval tp;
    if ( wxGetTimeOfDay(&tp, (struct timezone *)NULL) != -1 )
    {
        return (1000*tp.tv_sec + tp.tv_usec / 1000);
    }
#elif defined(HAVE_FTIME)
    struct timeb tp;
    if ( ftime(&tp) == 0 )
    {
        return (1000*tp.time + tp.millitm);
    }
#else
    #error "no function to find the current time on this system"
#endif

    wxLogSysError(_("Failed to get the system time"));

    return -1;
}

bool wxGetLocalTime(long *timeZone, int *dstObserved)
{
#if defined(HAVE_LOCALTIME) && defined(WX_GMTOFF_IN_TM)
    time_t t0 = time(&t0);
    if ( t0 != (time_t)-1 )
    {
        struct tm *tm = localtime(&t0);

        if ( tm )
        {
            *timeZone = tm->tm_gmtoff;
            *dstObserved = tm->tm_isdst;

            return TRUE;
        }
    }
#elif defined(HAVE_GETTIMEOFDAY) && !defined(WX_GETTIMEOFDAY_NO_TZ)
    struct timeval tp;
    struct timezone tz;
    if ( gettimeofday(&tp, &tz) != -1 )
    {
        *timeZone = 60*tz.tz_minuteswest;
        *dstObserved = tz.tz_dsttime;

        return TRUE;
    }
#elif defined(HAVE_FTIME)
    struct timeb tb;
    if ( ftime(&tb) == 0 )
    {
        *timeZone = 60*tb.timezone;
        *dstObserved = tb.dstflag;
    }
#else // no standard function return tz info
    // special hacks for known compilers
    #if defined(__BORLANDC__) || defined(__VISUALC__)
        *timeZone = _timezone;
        *dstObserved = _daylight;
    #elif defined(__SALFORDC__)
        *timeZone = _timezone;
        *dstObserved = daylight;
    #elif defined(__VISAGECPP__)
        *timeZone = _timezone;
        *dstObserved = daylight;
    #elif defined(__WIN32__)
        TIME_ZONE_INFORMATION tzInfo;
        switch ( GetTimeZoneInformation(&tzInfo) )
        {
            default:
                wxFAIL_MSG(_T("unknown GetTimeZoneInformation return code"));
                // fall through

            case TIME_ZONE_ID_UNKNOWN:
            case TIME_ZONE_ID_STANDARD:
                *dstObserved = FALSE;
                break;

            case TIME_ZONE_ID_DAYLIGHT:
                *dstObserved = TRUE;
                break;
        }

        *timeZone = 60*tzInfo.Bias;
    #else
        wxFAIL_MSG(_T("wxGetLocalTime() not implemented"));
    #endif // compiler
#endif // all ways in the known Universe to get tz info

    return FALSE;
}
