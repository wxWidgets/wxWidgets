/////////////////////////////////////////////////////////////////////////////
// Name:        common/timercmn.cpp
// Purpose:     Common timer implementation
// Author:
//    Original version by Julian Smart
//    Vadim Zeitlin got rid of all ifdefs (11.12.99)
//    Sylvain Bougnoux added wxStopWatch class
//    Guillermo Rodriguez <guille@iies.es> rewrote from scratch (Dic/99)
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
//              (c) 1999 Guillermo Rodriguez <guille@iies.es>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin headers
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
#include "wx/longlong.h"

// ----------------------------------------------------------------------------
// System headers
// ----------------------------------------------------------------------------

#if defined(__WIN32__)
    #include <windows.h>
#endif

#if defined(__WIN32__) && !defined(HAVE_FTIME)
    #define HAVE_FTIME
#endif

#include <time.h>
#ifndef __WXMAC__
    #include <sys/types.h>      // for time_t
#endif

#if defined(HAVE_GETTIMEOFDAY)
    #include <sys/time.h>
    #include <unistd.h>
#elif defined(HAVE_FTIME)
    #include <sys/timeb.h>
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_GUI
    IMPLEMENT_DYNAMIC_CLASS(wxTimerEvent, wxEvent)
#endif // wxUSE_GUI

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
// wxTimerBase
// ----------------------------------------------------------------------------

#if wxUSE_GUI

void wxTimerBase::Notify()
{
    // the base class version generates an event if it has owner - which it
    // should because otherwise nobody can process timer events
    wxCHECK_RET( m_owner, _T("wxTimer::Notify() should be overridden.") );

    wxTimerEvent event(m_idTimer, m_milli);
    (void)m_owner->ProcessEvent(event);
}

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// wxStopWatch
// ----------------------------------------------------------------------------

void wxStopWatch::Start(long t)
{
    m_t0 = wxGetLocalTimeMillis() - t;
    m_pause = 0;
}

long wxStopWatch::GetElapsedTime() const
{
  return (wxGetLocalTimeMillis() - m_t0).GetLo();
}

long wxStopWatch::Time() const
{
    return (m_pause ? m_pause : GetElapsedTime());
}

// ----------------------------------------------------------------------------
// old timer functions superceded by wxStopWatch
// ----------------------------------------------------------------------------

static wxLongLong wxStartTime = 0l;

// starts the global timer
void wxStartTimer()
{
    wxStartTime = wxGetLocalTimeMillis();
}

// Returns elapsed time in milliseconds
long wxGetElapsedTime(bool resetTimer)
{
    wxLongLong oldTime = wxStartTime;
    wxLongLong newTime = wxGetLocalTimeMillis();

    if ( resetTimer )
        wxStartTime = newTime;

    return (newTime - oldTime).GetLo();
}


// ----------------------------------------------------------------------------
// the functions to get the current time and timezone info
// ----------------------------------------------------------------------------

// Get local time as seconds since 00:00:00, Jan 1st 1970
long wxGetLocalTime()
{
    struct tm tm;
    time_t t0, t1;

    // This cannot be made static because mktime can overwrite it.
    //
    memset(&tm, 0, sizeof(tm));
    tm.tm_year  = 70;
    tm.tm_mon   = 0;
    tm.tm_mday  = 5;        // not Jan 1st 1970 due to mktime 'feature'
    tm.tm_hour  = 0;
    tm.tm_min   = 0;
    tm.tm_sec   = 0;
    tm.tm_isdst = -1;       // let mktime guess

    // Note that mktime assumes that the struct tm contains local time.
    //
    t1 = time(&t1);         // now
    t0 = mktime(&tm);       // origin

    // Return the difference in seconds.
    //
    if (( t0 != (time_t)-1 ) && ( t1 != (time_t)-1 ))
        return (long)difftime(t1, t0) + (60 * 60 * 24 * 4);

    wxLogSysError(_("Failed to get the local system time"));
    return -1;
}

// Get UTC time as seconds since 00:00:00, Jan 1st 1970
long wxGetUTCTime()
{
    struct tm tm, *ptm;
    time_t t0, t1;

    // This cannot be made static because mktime can overwrite it
    //
    memset(&tm, 0, sizeof(tm));
    tm.tm_year  = 70;
    tm.tm_mon   = 0;
    tm.tm_mday  = 5;        // not Jan 1st 1970 due to mktime 'feature'
    tm.tm_hour  = 0;
    tm.tm_min   = 0;
    tm.tm_sec   = 0;
    tm.tm_isdst = -1;       // let mktime guess

    // Note that mktime assumes that the struct tm contains local time.
    //
    t1 = time(&t1);         // now
    t0 = mktime(&tm);       // origin in localtime

    if (( t0 != (time_t)-1 ) && ( t1 != (time_t)-1 ))
    {
        // To get t0 as GMT we convert to a struct tm with gmtime,
        // and then back again.
        //
        ptm = gmtime(&t0);

        if (ptm)
        {
            memcpy(&tm, ptm, sizeof(tm));
            t0 = mktime(&tm);

            if (t0 != (time_t)-1 )
                return (long)difftime(t1, t0) + (60 * 60 * 24 * 4);
            wxLogSysError(_("mktime() failed"));
        }
        else
        {
            wxLogSysError(_("gmtime() failed"));
        }
    }

    wxLogError(_("Failed to get the UTC system time."));

    return -1;
}


// Get local time as milliseconds since 00:00:00, Jan 1st 1970
wxLongLong wxGetLocalTimeMillis()
{
    wxLongLong val = 1000l;

    // If possible, use a functin which avoids conversions from
    // broken-up time structures to milliseconds,

#if defined(HAVE_GETTIMEOFDAY)
    struct timeval tp;
    if ( wxGetTimeOfDay(&tp, (struct timezone *)NULL) != -1 )
    {
        val *= tp.tv_sec;
        return (val + (tp.tv_usec / 1000));
    }
    else
    {
        wxLogError(_("wxGetTimeOfDay failed."));
        return 0;
    }
#elif defined(HAVE_FTIME)
    struct timeb tp;

    // ftime() is void and not int in some mingw32 headers, so don't
    // test the return code (well, it shouldn't fail anyhow...)
    (void)ftime(&tp);
    val *= tp.time;
    return (val + tp.millitm);
#else // no gettimeofday() nor ftime()
    // We use wxGetLocalTime() to get the seconds since
    // 00:00:00 Jan 1st 1970 and then whatever is available
    // to get millisecond resolution.
    //
    // NOTE that this might lead to a problem if the clocks
    // use different sources, so this approach should be
    // avoided where possible.

    val *= wxGetLocalTime();

#if defined(__VISAGECPP__)
    // If your platform/compiler needs to use two different functions
    // to get ms resolution, please do NOT just shut off these warnings,
    // drop me a line instead at <guille@iies.es>
    #warning "Possible clock skew bug in wxGetLocalTimeMillis()!"

    DATETIME dt;
    ::DosGetDateTime(&dt);
    val += (dt.hundredths*10);
#elif defined (__WIN32__)
    // If your platform/compiler needs to use two different functions
    // to get ms resolution, please do NOT just shut off these warnings,
    // drop me a line instead at <guille@iies.es>
    #warning "Possible clock skew bug in wxGetLocalTimeMillis()!"

    SYSTEMTIME st;
    ::GetLocalTime(&st);
    val += st.wMilliseconds;
#else // !Win32
    // If your platform/compiler does not support ms resolution please
    // do NOT just shut off these warnings, drop me a line instead at
    // <guille@iies.es>

    #if defined(__VISUALC__)
        #pragma message("wxStopWatch will be up to second resolution!")
    #elif defined(__BORLANDC__)
        #pragma message "wxStopWatch will be up to second resolution!"
    #else
        #warning "wxStopWatch will be up to second resolution!"
    #endif // compiler
#endif

    return val;

#endif // time functions
}
