/////////////////////////////////////////////////////////////////////////////
// Name:        timercmn.cpp
// Purpose:     Common timer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
//#pragma implementation "timercmn.h"
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/list.h"
#endif

#include "wx/timer.h"

#if defined(__SVR4__) && !defined(__SYSV__)
#define __SYSV__
#endif

#include <time.h>

#ifndef __WXMAC__
#include <sys/types.h>
#endif

#if (!defined(__SC__) && !defined(__SGI__) && !defined(__GNUWIN32__) && !defined(__MWERKS__)) || defined(__MINGW32__)
#include <sys/timeb.h>
#endif

#if defined(__linux__) || defined(__SVR4__) || defined(__SYSV__) || defined(__SGI__) || \
    defined(__ALPHA__) || defined(__GNUWIN32__) || defined(__FreeBSD__) || defined(__NetBSD__) || \
    defined(__SALFORDC__) || defined(__EMX__)
#include <sys/time.h>
#endif

#ifdef __MINGW32__
#include "windows.h"
#endif

#if defined(__SUN__) || defined(__OSF__) || defined(__FreeBSD__)
// At least on Sun, ftime is undeclared.
// Need to be verified on other platforms.
extern "C" int ftime(struct timeb *tp);
//extern "C" int gettimeofday(struct timeval *tp, void *);
// extern "C" time_t time(time_t);
// #include <sys/timeb.h>
#if defined(__SVR4__) && !defined(__ALPHA__)
// ditto for gettimeofday on Solaris 2.x.
extern "C" int gettimeofday(struct timeval *tp, void *);
#endif
#endif

/*
 * Timer functions
 *
 */

long wxStartTime = 0;
void wxStartTimer(void)
{
  wxStartTime=wxGetCurrentMTime();
}

// Returns elapsed time in milliseconds
long wxGetElapsedTime(bool resetTimer)
{
  long oldTime = wxStartTime;
  long newTime=wxGetCurrentMTime();

  if (resetTimer) wxStartTime = newTime;
  return newTime - oldTime;
}


// Get number of seconds since 00:00:00 GMT, Jan 1st 1970.
long wxGetCurrentTime(void)
{
  return wxGetCurrentMTime()/1000;
}

// return GMT time in millisecond
long wxGetCurrentMTime()
{
#if defined(__xlC__) || defined(__AIX__) || defined(__SVR4__) || defined(__SYSV__) || \
    (defined(__GNUWIN32__) && !defined(__MINGW32__)) // || defined(__AIXV3__)
  struct timeval tp;
#if defined(__SYSV__) || (defined (__GNUWIN32__) && !defined (__MINGW32__))
  gettimeofday(&tp, (struct timezone *)NULL);
#else
  gettimeofday(&tp);
#endif
  return (1000*tp.tv_sec + tp.tv_usec / 1000);
#elif (defined(__SC__) || defined(__SGI__) || defined(___BSDI__) || defined(__ALPHA__) || \
  defined(__MINGW32__)|| defined(__MWERKS__) || defined(__FreeBSD__))
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
  return 1000*(60*(60*tp->tm_hour+tp->tm_min)+tp->tm_sec);
#else
  struct timeb tp;
  ftime(&tp);
  return (1000*tp.time + tp.millitm);
#endif
}

//---------------
// wxChrono class
// This class encapsulates the above fonctions,
// such that several wxChrono can be created 
// simultaneously

wxChrono::wxChrono()
{
  Start();
}

void wxChrono::Start(long t)
{
  m_t0=wxGetCurrentMTime()-t;
  m_pause=0;
}

void wxChrono::Pause()
{
  m_pause=wxGetCurrentMTime()-m_t0;
}

void wxChrono::Resume()
{
  m_t0=wxGetCurrentMTime()-m_pause;
  m_pause=0;
}

long wxChrono::Time()
{
  if (m_pause) return m_pause;
  return wxGetCurrentMTime()-m_t0;
}



// EXPERIMENTAL: comment this out if it doesn't compile.
#if !defined( __VMS__ ) || ( __VMS_VER >= 70000000 )
bool wxGetLocalTime(long *timeZone, int *dstObserved)
{
#if defined(__MINGW32__)
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
# if __GNUC__ == 2 && __GNUC_MINOR__ <= 8
  // gcc 2.8.x or earlier
  timeb tz;
  ftime(& tz);
  *timeZone = tz._timezone;
# else
  // egcs or gcc 2.95
  *timeZone = _timezone; // tp->tm_gmtoff; // ???
# endif
  *dstObserved = tp->tm_isdst;
#else
// not mingw32...
#if (((defined(__SYSV__) && !defined(__HPUX__)) || defined(__MSDOS__) || defined(__WXMSW__) || defined(__WXPM__)) \
   && !defined(__GNUWIN32__) && !defined(__MWERKS__) )
#  if defined(__BORLANDC__)
  /* Borland uses underscores */
  *timeZone = _timezone;
  *dstObserved = _daylight;
#  elif defined(__SALFORDC__)
  *timeZone = _timezone;
  *dstObserved = daylight;
#  elif defined(__VISAGECPP__)
  *timeZone = _timezone;
  *dstObserved = daylight;
#  else
  *timeZone = timezone;
  *dstObserved = daylight;
#  endif
#elif defined(__xlC__) || defined(__AIX__) || defined(__SVR4__) || \
   defined(__SYSV__) || defined(__MWERKS__) || (defined(__GNUWIN32__) && \
						   !defined(__MINGW32__))\
       || defined( __VMS__ ) // || defined(__AIXV3__)
#  if defined(__SYSV__) || (defined(__GNUWIN32__) && !defined(__MINGW32))
#  ifndef __MWERKS__
  struct timeval tp;
#  endif
  struct timezone tz;
  gettimeofday(&tp, &tz);
  *timeZone = 60*(tz.tz_minuteswest);
  *dstObserved = tz.tz_dsttime;
#  else
  time_t t0;
  struct tm *tp;
  time(&t0);
  tp = localtime(&t0);
#  ifndef __MWERKS__
  *timeZone = tp->tm_gmtoff; // ???
#  else
  *timeZone = 0 ;
#  endif
  *dstObserved = tp->tm_isdst;
#endif
#elif defined(__WXSTUBS__)
  return FALSE;
#else
// #error wxGetLocalTime not implemented.
  struct timeval tp;
  struct timezone tz;
  gettimeofday(&tp, &tz);
  *timeZone = 60*(tz.tz_minuteswest);
  *dstObserved = tz.tz_dsttime;
#endif
#endif
  // __MINGW32__
  return TRUE;
}
#endif
