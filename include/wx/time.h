///////////////////////////////////////////////////////////////////////////////
// Name:        wx/time.h
// Purpose:     Miscellaneous time-related functions.
// Author:      Vadim Zeitlin
// Created:     2011-11-26
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIME_H_
#define _WX_TIME_H_

#include "wx/longlong.h"

// Returns the difference between UTC and local time in seconds.
WXDLLIMPEXP_BASE int wxGetTimeZone();

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

#if wxUSE_LONGLONG

// Get the number of milliseconds or microseconds since the Epoch.
wxLongLong WXDLLIMPEXP_BASE wxGetUTCTimeMillis();
wxLongLong WXDLLIMPEXP_BASE wxGetUTCTimeUSec();

#endif // wxUSE_LONGLONG

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

#endif // _WX_TIME_H_
