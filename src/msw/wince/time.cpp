/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/wince/time.cpp
// Purpose:     Implements missing time functionality for WinCE
// Author:      
// Modified by: 
// Created:     2003-07-10
// RCS-ID:      $Id$
// Copyright:   
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "window.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <windows.h>
    #include "wx/msw/winundef.h"
#endif

#include "wx/msw/wince/time.h"

long timezone = 0;

// Hint: use GetSystemTime()

struct tm * localtime(const time_t *)
{
    // TODO
    return NULL;

    // Possible implementation
#if 0
  // Localtime for WinCE
  // By martin brown (mpatalberta@yahoo.com)
  // This is really stupid, converting FILETIME to timeval back and
  // forth.  It assumes FILETIME and DWORDLONG are the same structure
  // internally.
 
 TIME_ZONE_INFORMATION pTz;
 
 const unsigned short int __mon_yday[2][13] =
  {
     /* Normal years.  */
     { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
     { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
   };


  ULARGE_INTEGER _100ns;
  ::GetTimeZoneInformation(&pTz);

  // FIXME
  _100ns.QuadPart = (DWORDLONG) *t * 10000 * 1000 /* + ACE_Time_Value::FILETIME_to_timval_skew */;
  FILETIME file_time;
  file_time.dwLowDateTime = _100ns.LowPart;
  file_time.dwHighDateTime = _100ns.HighPart;
  
  FILETIME localtime;
  SYSTEMTIME systime;
  FileTimeToLocalFileTime (&file_time, &localtime);
  FileTimeToSystemTime (&localtime, &systime);

  res->tm_hour = systime.wHour;

  if(pTz.DaylightBias!=0)
  {
 res->tm_isdst = 1;
  }
  else
  {
 res->tm_isdst = 1;
  }

  int iLeap;
  iLeap = (res->tm_year % 4 == 0 && (res->tm_year% 100 != 0 || res->tm_year % 400 == 0));
  // based on leap select which group to use
  
  res->tm_mday = systime.wDay;
  res->tm_min = systime.wMinute;
  res->tm_mon = systime.wMonth;
  res->tm_sec = systime.wSecond;
  res->tm_wday = systime.wDayOfWeek;
  res->tm_yday = __mon_yday[iLeap][systime.wMonth] + systime.wDay;
  res->tm_year = systime.wYear;// this the correct year but bias the value to start at the 1900
  res->tm_year = res->tm_year - 1900;

  return res;
#endif
}

time_t time(time_t *)
{
    // TODO
    return 0;
}

size_t wcsftime(wchar_t *, size_t, const wchar_t *,
        const struct tm *)
{
    // TODO
    return 0;
}

time_t mktime(struct tm *)
{
    // TODO
    return 0;
}

struct tm * gmtime(const time_t *)
{
    // TODO
    return NULL;
}

