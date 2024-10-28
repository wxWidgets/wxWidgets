///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/time.cpp
// Purpose:     Implementation of time-related functions.
// Author:      Vadim Zeitlin
// Created:     2011-11-26
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
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


#include "wx/time.h"

#ifndef WX_PRECOMP
    #ifdef __WINDOWS__
        #include "wx/msw/wrapwin.h"
    #endif
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#ifndef WX_GMTOFF_IN_TM
    // Define it for some systems which don't (always) use configure but are
    // known to have tm_gmtoff field.
    #if defined(__DARWIN__)
        #define WX_GMTOFF_IN_TM
    #endif
#endif

#ifdef WX_GMTOFF_IN_TM
    #include <atomic>
#endif

#include <time.h>

wxDECL_FOR_STRICT_MINGW32(void, tzset, (void));

#if !defined(__WXMAC__)
    #include <sys/types.h>      // for time_t
#endif

#if defined(HAVE_GETTIMEOFDAY)
    #include <sys/time.h>
    #include <unistd.h>
#elif defined(HAVE_FTIME)
    #include <sys/timeb.h>
#endif

#if defined(__WINE__)
    #include <sys/timeb.h>
    #include <values.h>
#endif

namespace
{

const int MILLISECONDS_PER_SECOND = 1000;
#if !defined(__WINDOWS__)
const int MICROSECONDS_PER_MILLISECOND = 1000;
#ifdef HAVE_GETTIMEOFDAY
const int MICROSECONDS_PER_SECOND = 1000*1000;
#endif
#endif

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

#if (!defined(HAVE_LOCALTIME_R) || !defined(HAVE_GMTIME_R)) && wxUSE_THREADS && !defined(__WINDOWS__)
static wxMutex timeLock;
#endif

#ifndef HAVE_LOCALTIME_R
struct tm *wxLocaltime_r(const time_t* ticks, struct tm* temp)
{
#if wxUSE_THREADS && !defined(__WINDOWS__)
  // No need to waste time with a mutex on windows since it's using
  // thread local storage for localtime anyway.
  wxMutexLocker locker(timeLock);
#endif

  const tm * const t = localtime(ticks);
  if ( !t )
      return nullptr;

  memcpy(temp, t, sizeof(struct tm));
  return temp;
}
#endif // !HAVE_LOCALTIME_R

#ifndef HAVE_GMTIME_R
struct tm *wxGmtime_r(const time_t* ticks, struct tm* temp)
{
#if wxUSE_THREADS && !defined(__WINDOWS__)
  // No need to waste time with a mutex on windows since it's
  // using thread local storage for gmtime anyway.
  wxMutexLocker locker(timeLock);
#endif

  const tm * const t = gmtime(ticks);
  if ( !t )
      return nullptr;

  memcpy(temp, gmtime(ticks), sizeof(struct tm));
  return temp;
}
#endif // !HAVE_GMTIME_R

// returns the time zone in the C sense, i.e. the difference UTC - local
// (in seconds)
int wxGetTimeZone()
{
#ifdef WX_GMTOFF_IN_TM
    static std::atomic<int> s_gmtoffset{INT_MAX}; // invalid timezone

    // ensure that the timezone variable is set by calling wxLocaltime_r
    if ( s_gmtoffset == INT_MAX )
    {
        // just call wxLocaltime_r() instead of figuring out whether this
        // system supports tzset(), _tzset() or something else
        time_t t = time(nullptr);
        struct tm tm;

        wxLocaltime_r(&t, &tm);

        // note that GMT offset is the opposite of time zone and so to return
        // consistent results in both WX_GMTOFF_IN_TM and !WX_GMTOFF_IN_TM
        // cases we have to negate it
        int gmtoffset = static_cast<int>(-tm.tm_gmtoff);

        // this function is supposed to return the same value whether DST is
        // enabled or not, so we need to use an additional offset if DST is on
        // as tm_gmtoff already does include it
        if ( tm.tm_isdst )
            gmtoffset += 3600;

        s_gmtoffset = gmtoffset;
    }
    return s_gmtoffset;
#elif defined(__WINE__)
    struct timeb tb;
    ftime(&tb);
    return tb.timezone*60;
#elif defined(__VISUALC__)
    // We must initialize the time zone information before using it. It's not a
    // problem if we do it twice due to a race condition, as it's idempotent
    // anyhow, so don't bother with any locks here.
    static bool s_tzSet = (_tzset(), true);
    wxUnusedVar(s_tzSet);

    long t;
    _get_timezone(&t);
    return t;
#else // Use some kind of time zone variable.
    // In any case we must initialize the time zone before using it.
    static bool s_tzSet = (tzset(), true);
    wxUnusedVar(s_tzSet);

    #if defined(WX_TIMEZONE) // If WX_TIMEZONE was defined by configure, use it.
        return WX_TIMEZONE;
    #elif defined(__MINGW32__)
        #if defined(__MINGW32_TOOLCHAIN__) && defined(__STRICT_ANSI__)
            extern long _timezone;
        #endif

        return _timezone;
    #else // unknown platform -- assume it has timezone
        return timezone;
    #endif // different time zone variables
#endif // different ways to determine time zone
}

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
    return (long)time(nullptr);
}

wxLongLong wxGetUTCTimeUSec()
{
#if defined(__WINDOWS__)
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // FILETIME is in 100ns or 0.1us since 1601-01-01, transform to us since
    // 1970-01-01.
    wxLongLong t(ft.dwHighDateTime, ft.dwLowDateTime);
    t /= 10;
    t -= wxLL(11644473600000000); // Unix - Windows epochs difference in us.
    return t;
#else // non-MSW

#ifdef HAVE_GETTIMEOFDAY
    timeval tv;
    if ( wxGetTimeOfDay(&tv) != -1 )
    {
        wxLongLong val(tv.tv_sec);
        val *= MICROSECONDS_PER_SECOND;
        val += tv.tv_usec;
        return val;
    }
#endif // HAVE_GETTIMEOFDAY

    // Fall back to lesser precision function.
    return wxGetUTCTimeMillis()*MICROSECONDS_PER_MILLISECOND;
#endif // MSW/!MSW
}

// Get local time as milliseconds since 00:00:00, Jan 1st 1970
wxLongLong wxGetUTCTimeMillis()
{
    // If possible, use a function which avoids conversions from
    // broken-up time structures to milliseconds
#if defined(__WINDOWS__)
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    // FILETIME is expressed in 100ns (or 0.1us) units since 1601-01-01,
    // transform them to ms since 1970-01-01.
    wxLongLong t(ft.dwHighDateTime, ft.dwLowDateTime);
    t /= 10000;
    t -= wxLL(11644473600000); // Unix - Windows epochs difference in ms.
    return t;
#else // !__WINDOWS__
    wxLongLong val = MILLISECONDS_PER_SECOND;

#if defined(HAVE_GETTIMEOFDAY)
    struct timeval tp;
    if ( wxGetTimeOfDay(&tp) != -1 )
    {
        val *= tp.tv_sec;
        return (val + (tp.tv_usec / MICROSECONDS_PER_MILLISECOND));
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
    (void)::ftime(&tp);
    val *= tp.time;
    return (val + tp.millitm);
#else // no gettimeofday() nor ftime()
    // If your platform/compiler does not support ms resolution please
    // do NOT just shut off these warnings, drop me a line instead at
    // <guille@iies.es>

    #if defined(__VISUALC__)
        #pragma message("wxStopWatch will be up to second resolution!")
    #else
        #warning "wxStopWatch will be up to second resolution!"
    #endif // compiler

    val *= wxGetUTCTime();
    return val;
#endif // time functions

#endif // __WINDOWS__/!__WINDOWS__
}

wxLongLong wxGetLocalTimeMillis()
{
    return wxGetUTCTimeMillis() - wxGetTimeZone()*MILLISECONDS_PER_SECOND;
}
