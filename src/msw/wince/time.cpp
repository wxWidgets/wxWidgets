/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/wince/time.cpp
// Purpose:     Implements missing time functionality for WinCE
// Author:      Marco Cavallini (MCK) - wx@koansoftware.com
// Modified by:
// Created:     31-08-2003
// RCS-ID:
// Copyright:   (c) Marco Cavallini
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "window.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
#endif

#include "wx/msw/wince/time.h"
#include <winbase.h>

#define is_leap(y)   (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
#define SECONDS_IN_ONE_MINUTE      60
#define DAYS_IN_ONE_YEAR          365
#define SECONDS_IN_ONE_MIN         60
#define SECONDS_IN_ONE_HOUR      3600
#define SECONDS_IN_ONE_DAY      86400
#define DEFAULT_TIMEZONE        28800
#define DO_GMTIME                   0
#define DO_LOCALTIME                1


long timezone ; // global variable


////////////////////////////////////////////////////////////////////////
// Common code for localtime and gmtime (static)
////////////////////////////////////////////////////////////////////////
static struct tm * __cdecl common_localtime(const time_t * WXUNUSED(t), BOOL bLocal)
{
    wxString str ;
    SYSTEMTIME SystemTime;
    TIME_ZONE_INFORMATION pTz;
    static struct tm st_res ;             // data holder
    static struct tm * res = &st_res ;    // data pointer
    int iLeap;
    const unsigned short int __mon_yday[2][13] =
    {
        // Normal years
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
        // Leap years
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
    };

    ::GetLocalTime(&SystemTime) ;
    ::GetTimeZoneInformation(&pTz);

    ///////////////////////////////////////////////
    // Set timezone
    timezone = pTz.Bias * SECONDS_IN_ONE_MINUTE ;
    ///////////////////////////////////////////////

    iLeap = is_leap(SystemTime.wYear) ;

    res->tm_hour = SystemTime.wHour;
    res->tm_min  = SystemTime.wMinute;
    res->tm_sec  = SystemTime.wSecond;

    res->tm_mday = SystemTime.wDay;
    res->tm_mon = SystemTime.wMonth - 1; // this the correct month but localtime returns month aligned to zero
    res->tm_year = SystemTime.wYear;     // this the correct year
    res->tm_year = res->tm_year - 1900;  // but localtime returns the value starting at the 1900

    res->tm_wday = SystemTime.wDayOfWeek;
    res->tm_yday = __mon_yday[iLeap][res->tm_mon] + SystemTime.wDay - 1; // localtime returns year-day aligned to zero

    // if localtime behavior and daylight saving
    if (bLocal && pTz.DaylightBias != 0)
        res->tm_isdst = 1;
    else
        res->tm_isdst = 0; // without daylight saving or gmtime

    return res;
}

////////////////////////////////////////////////////////////////////////
// Receive the number of seconds elapsed since midnight(00:00:00)
// and convert a time value and corrects for the local time zone
////////////////////////////////////////////////////////////////////////
struct tm * __cdecl localtime(const time_t * t)
{
    return common_localtime(t, DO_LOCALTIME) ;
}

////////////////////////////////////////////////////////////////////////
// Receives the number of seconds elapsed since midnight(00:00:00)
// and converts a time value WITHOUT correcting for the local time zone
////////////////////////////////////////////////////////////////////////
struct tm * __cdecl gmtime(const time_t *t)
{
    return common_localtime(t, DO_GMTIME) ;
}


////////////////////////////////////////////////////////////////////////
// Common code for conversion of struct tm into time_t   (static)
////////////////////////////////////////////////////////////////////////
static time_t __cdecl common_tm_to_time(int day, int month, int year, int hour, int minute, int second)
{
    time_t prog = 0 ;
    static int mdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 } ;

    while (--month)
    {
        prog += mdays[month - 1] ;
        if (month == 2 && is_leap(year))
            prog++ ;
    }

    // Calculate seconds in elapsed days
    prog = day - 1 ;        // align first day of the year to zero
    prog += (DAYS_IN_ONE_YEAR * (year - 1970) + (year - 1901) / 4 - 19) ;
    prog *= SECONDS_IN_ONE_DAY ;

    // Add Calculated elapsed seconds in the current day
    prog += (hour * SECONDS_IN_ONE_HOUR + minute *
                               SECONDS_IN_ONE_MIN + second) ;

    return prog ;
}


////////////////////////////////////////////////////////////////////////
// Returns the number of seconds elapsed since
// midnight(00:00:00) of 1 January 1970
////////////////////////////////////////////////////////////////////////
time_t __cdecl time(time_t *t)
{
    time_t prog = 0 ;

    if (t != NULL)
    {
        SYSTEMTIME SystemTime;

        ::GetLocalTime(&SystemTime) ;
        prog = common_tm_to_time(SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
                                 SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond) ;
        *t = prog ;
    }

    return prog ;
}

////////////////////////////////////////////////////////////////////////
// Converts the local time provided by struct tm
// into a time_t calendar value
////////////////////////////////////////////////////////////////////////
time_t __cdecl mktime(struct tm *t)
{
    return (common_tm_to_time(t->tm_mday, t->tm_mon+1, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec)) ;
}


size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm *)
{
    // TODO : maybe copy something from wxString::Printf

    return 0;
}
