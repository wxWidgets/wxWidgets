///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/time.cpp
// Purpose:     Implementation of time-related functions.
// Author:      Vadim Zeitlin
// Created:     2011-11-26
// RCS-ID:      $Id: wxhead.cpp,v 1.11 2010-04-22 12:44:51 zeitlin Exp $
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/time.h"

#ifndef WX_GMTOFF_IN_TM
    // Define it for some systems which don't (always) use configure but are
    // known to have tm_gmtoff field.
    #if defined(__WXPALMOS__) || defined(__DARWIN__)
        #define WX_GMTOFF_IN_TM
    #endif
#endif

// ============================================================================
// implementation
// ============================================================================

// returns the time zone in the C sense, i.e. the difference UTC - local
// (in seconds)
int wxGetTimeZone()
{
#ifdef WX_GMTOFF_IN_TM
    // set to true when the timezone is set
    static bool s_timezoneSet = false;
    static long gmtoffset = LONG_MAX; // invalid timezone

    // ensure that the timezone variable is set by calling wxLocaltime_r
    if ( !s_timezoneSet )
    {
        // just call wxLocaltime_r() instead of figuring out whether this
        // system supports tzset(), _tzset() or something else
        time_t t = time(NULL);
        struct tm tm;

        wxLocaltime_r(&t, &tm);
        s_timezoneSet = true;

        // note that GMT offset is the opposite of time zone and so to return
        // consistent results in both WX_GMTOFF_IN_TM and !WX_GMTOFF_IN_TM
        // cases we have to negate it
        gmtoffset = -tm.tm_gmtoff;

        // this function is supposed to return the same value whether DST is
        // enabled or not, so we need to use an additional offset if DST is on
        // as tm_gmtoff already does include it
        if ( tm.tm_isdst )
            gmtoffset += 3600;
    }
    return (int)gmtoffset;
#elif defined(__DJGPP__) || defined(__WINE__)
    struct timeb tb;
    ftime(&tb);
    return tb.timezone*60;
#elif defined(__VISUALC__)
    // We must initialize the time zone information before using it (this will
    // be done only once internally).
    _tzset();

    // Starting with VC++ 8 timezone variable is deprecated and is not even
    // available in some standard library version so use the new function for
    // accessing it instead.
    #if wxCHECK_VISUALC_VERSION(8)
        long t;
        _get_timezone(&t);
        return t;
    #else // VC++ < 8
        return timezone;
    #endif
#elif defined(WX_TIMEZONE) // If WX_TIMEZONE was defined by configure, use it.
    return WX_TIMEZONE;
#elif defined(__BORLANDC__) || defined(__MINGW32__) || defined(__VISAGECPP__)
    return _timezone;
#elif defined(__MWERKS__)
    return 28800;
#else // unknown platform -- assume it has timezone
    return timezone;
#endif // WX_GMTOFF_IN_TM/!WX_GMTOFF_IN_TM
}
