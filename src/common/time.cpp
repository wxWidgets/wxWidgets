/////////////////////////////////////////////////////////////////////////////
// Name:        time.cpp
// Purpose:     wxTime class, from NIHCL
// Author:      Julian Smart, after K. E. Gorlen
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "time.h"
#endif

/*
Provides an object that represents a Time, stored as the number of
seconds since January 1, 1901, GMT.
*/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/setup.h"

#if wxUSE_TIMEDATE

#include "wx/time.h"
#include "wx/date.h"
#include "wx/utils.h"
#include <wx/intl.h>

#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#  ifdef _MSC_VER
      using namespace std;
#  endif
#endif

#include <string.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTime, wxObject)
#endif


extern bool wxGetLocalTime(long *timeZone, int *dstObserved);
extern long wxGetCurrentTime(void);

static long TIME_ZONE;          /* seconds west of GMT */
static int DST_OBSERVED;        /* flags U.S. daylight saving time observed */

static bool wxTimeInitialized = FALSE;

wxTime::tFormat		wxTime::Format		= wxTime::wx12h;
wxTime::tPrecision	wxTime::Precision	= wxTime::wxStdMinSec;

static const unsigned long seconds_in_day = 24*60*60L;
static const wxDate refDate(1,1,1901);
// static const wxDate maxDate(49709L);      /* ((2**32)-1)/seconds_in_day -1 */

wxTime wxTime::GetLocalTime(const wxDate& date, hourTy h, minuteTy m, secondTy s)
/*
		  Return a local wxTime for the specified Standard Time date, hour, minute,
		  and second.
*/
{
  if (!wxTimeInitialized)
  {
	 wxGetLocalTime(&TIME_ZONE, &DST_OBSERVED);
	 wxTimeInitialized = TRUE;
  }
/*
		  if (!date.IsBetween(refDate,maxDate))
					 setError(NIHCL_DATERANGE,DEFAULT,
								date.dayOfMonth(),date.nameOfMonth(),date.year());
*/
  // The following line causes an error in GCC 2.1
//   long daysBetween = date-refDate;
  // ... but this seems to get round it.
  wxDate tmp1(date);
  wxDate tmp2(refDate);
  long daysBetween = tmp1 - tmp2;

  return wxTime(seconds_in_day*daysBetween + 60*60L*h + 60*m + s);
}

wxTime::wxTime()
/*
		  Construct a wxTime for this instant.
*/
{
  if (!wxTimeInitialized)
  {
	 wxGetLocalTime(&TIME_ZONE, &DST_OBSERVED);
	 wxTimeInitialized = TRUE;
  }
  sec = wxGetCurrentTime();
  sec += 2177452800UL;     /* seconds from 1/1/01 to 1/1/70 */
}

wxTime::wxTime(hourTy h, minuteTy m, secondTy s, bool dst)
/*
		  Construct a wxTime for today at the specified (local) hour, minute, and
		  second.
*/
{
  if (!wxTimeInitialized)
  {
	 wxGetLocalTime(&TIME_ZONE, &DST_OBSERVED);
	 wxTimeInitialized = TRUE;
  }

  sec = wxTime(wxDate(),h,m,s,dst).sec;
}


wxTime::wxTime(const wxDate& date, hourTy h, minuteTy m, secondTy s, bool dst)
/*
        Construct a wxTime for the specified (local) Date, hour, minute, and
        second.
*/
{
  if (!wxTimeInitialized)
  {
    wxGetLocalTime(&TIME_ZONE, &DST_OBSERVED);
	 wxTimeInitialized = TRUE;
  }
  sec = GetLocalTime(date,h,m,s).sec-3600;
  if (IsDST())
  {
    sec += 3600;
    if (IsDST() || dst) sec -= 3600;
  }
  else
  {
    sec += 3600;
/*
                if (IsDST()) setError(NIHCL_BADTIME,DEFAULT,
								date.dayOfMonth(),date.nameOfMonth(),date.year(),
								h,m,s,(dst?_("DST"):""));
*/
  }
  sec += TIME_ZONE;                               // adjust to GMT
}

wxTime::operator wxDate() const
/*
		  Convert a wxTime to a local wxDate
*/
{
//      return wxDate((int)(GetLocalTime().sec/seconds_in_day));     4.2 cc bug
		  long daycount = (long)(GetLocalTime().sec/seconds_in_day);

        wxDate date(1,1,1901);
        date += daycount;
        return date;
}

bool wxTime::IsBetween(const wxTime& a, const wxTime& b) const
{
        return *this >= a && *this <= b;
}

hourTy wxTime::GetHour() const
/*
		  Return the hour of this wxTime in local time; i.e., adjust for
        time zone and Daylight Savings Time.
*/
{
		  return GetLocalTime().GetHourGMT();
}

hourTy wxTime::GetHourGMT() const
/*
        Return the hour of this Time in GMT.
*/
{
		  return (hourTy)((sec % 86400) / 3600);
}

wxTime wxTime::GetBeginDST(unsigned year)
/*
		  Return the local Standard Time at which Daylight Savings Time
        begins in the specified year.
*/
{
        // Previous Sunday
        wxTime DSTtime(GetLocalTime(wxDate(3,31,year).Previous(1)+7,2));
        if (year<=1986) {
                // Previous Sunday
					 DSTtime = GetLocalTime(wxDate(4,30,year).Previous(1),2);
					 if (year==1974) DSTtime = GetLocalTime(wxDate(1,6,1974),2);
                if (year==1975) DSTtime = GetLocalTime(wxDate(2,23,1975),2);
        }
        return DSTtime;
}

wxTime wxTime::GetEndDST(unsigned year)
/*
        Return the local Standard Time at which Daylight Savings Time
        ends in the specified year.
*/
{
		  wxTime STDtime(GetLocalTime(wxDate(10,31,year).Previous(1),2-1));
		  return STDtime;
}

bool wxTime::IsDST() const
/*
        Return TRUE if this local Standard Time should be adjusted
        for Daylight Savings Time.
*/
{
        long daycount = (long)(sec/seconds_in_day);

        // At this point, daycount is the number of days from 1/1/1901.
		  // Need to convert to julian date (which starts at 1/1/4713 B.C.)
		  wxDate date(1,1,1901);
        date += daycount;

        unsigned year = date.GetYear();
		  if (DST_OBSERVED)
        {
                if (*this >= GetBeginDST(year))
                        if (*this < GetEndDST(year)) return TRUE;
        }
        return FALSE;
}

wxTime wxTime::GetLocalTime() const
/*
        Adjusts this GM Time for local time zone and Daylight Savings Time.
*/
{
		  wxTime local_time(sec-TIME_ZONE);
        if (local_time.IsDST()) local_time.sec += 3600;
        return local_time;
}

minuteTy wxTime::GetMinute() const
/*
        Return the minute of this wxTime in local time; i.e., adjust
		  for time zone and Daylight Savings Time.
*/
{
        return GetLocalTime().GetMinuteGMT();
}

minuteTy wxTime::GetMinuteGMT() const
/*
        Return the minute of this wxTime in GMT.
*/
{
		  return (minuteTy)(((sec % 86400) % 3600) / 60);
}

secondTy wxTime::GetSecond() const
/*
        Return the second of this wxTime.
*/
{
		  return (secondTy)(((sec % 86400) % 3600) % 60);
}

secondTy wxTime::GetSecondGMT() const
/*
        Return the minute of this wxTime in GMT.
*/
{
        return (secondTy)(((sec % 86400) % 3600) % 60);
}

int wxTime::GetDay() const
{
    wxDate da((wxDate) *this);
    return da.GetDay();
}

int wxTime::GetDayOfWeek() const
{
    wxDate da((wxDate) *this);
    return da.GetDayOfWeek();
}

int wxTime::GetMonth() const
{
    wxDate da((wxDate) *this);
    return da.GetMonth();
}

int wxTime::GetYear() const
{
    wxDate da((wxDate) *this);
    return da.GetYear();
}

wxTime wxTime::Max(const wxTime& t) const
{
        if (t < *this) return *this;
        return t;
}

wxTime wxTime::Min(const wxTime& t) const
{
        if (t > *this) return *this;
		  return t;
}

wxTime::operator char *(void)
{
  return FormatTime();
}

void wxTime::SetFormat(const wxTime::tFormat lFormat,
							  const wxTime::tPrecision lPrecision) {

	wxTime::Format		= lFormat;
	wxTime::Precision	= lPrecision;
}

char *wxTime::FormatTime() const {
	static char	timeBuf[30];
	unsigned		hh(GetHour());

	switch (Format) {
	case wx12h:
		hh	-= 12;
		break;
	case wx24h:
		break;
	}

	switch (Precision) {
	case wxStdMinSec:
		sprintf(timeBuf,"%2d:%02d:%02d",hh,GetMinute(),GetSecond());
		break;
	case wxStdMin:
		sprintf(timeBuf,"%2d:%02d",hh,GetMinute());
		break;
	}

	if (Format == wx12h)
		if (GetHour() <= 12)
			strcat(timeBuf,_("am"));
		else
			strcat(timeBuf,_("pm"));

	return timeBuf;
}

/*
int wxTime::compare(const Object& ob) const
{
		  assertArgSpecies(ob,classDesc,"compare");
        register clockTy t = castdown(ob).sec;
        if (sec < t) return -1;
        if (sec > t) return 1;
        return 0;
}

void wxTime::deepenShallowCopy()  {}

unsigned wxTime::hash() const     { return sec; }

bool wxTime::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* wxTime::species() const { return &classDesc; }

void wxTime::printOn(ostream& strm) const
{
        register unsigned hh = GetHour();
        wxDate(*this).printOn(strm);
        strm << ' ' << ((hh <= 12) ? hh : hh-12) << ':'
             << setfill('0') << setw(2) << GetMinute() << ':'
				 << setfill('0') << setw(2) << GetSecond() << ' ';
        if (hh < 12) strm << _("am");
        else strm << _("pm");
}

wxTime::wxTime(OIOin& strm)
		  : BASE(strm)
{
		  unsigned long usec;
		  strm >> sec >> usec;
}

void wxTime::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << sec << 0l;
}


wxTime::wxTime(OIOifd& fd)
		  : BASE(fd)
{
		  unsigned long usec;
		  fd >> sec >> usec;
}

void wxTime::storer(OIOofd& fd) const
{
		  BASE::storer(fd);
		  fd << sec << 0l;
}
*/

#endif
