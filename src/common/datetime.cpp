///////////////////////////////////////////////////////////////////////////////
// Name:        wx/datetime.h
// Purpose:     implementation of time/date related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.05.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              parts of code taken from sndcal library by Scott E. Lee:
//
//               Copyright 1993-1995, Scott E. Lee, all rights reserved.
//               Permission granted to use, copy, modify, distribute and sell
//               so long as the above copyright and this permission statement
//               are retained in all copies.
//
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

/*
 * Implementation notes:
 *
 * 1. the time is stored as a 64bit integer containing the signed number of
 *    milliseconds since Jan 1. 1970 (the Unix Epoch) - so it is always
 *    expressed in GMT.
 *
 * 2. the range is thus something about 580 million years, but due to current
 *    algorithms limitations, only dates from Nov 24, 4714BC are handled
 *
 * 3. standard ANSI C functions are used to do time calculations whenever
 *    possible, i.e. when the date is in the range Jan 1, 1970 to 2038
 *
 * 4. otherwise, the calculations are done by converting the date to/from JDN
 *    first (the range limitation mentioned above comes from here: the
 *    algorithm used by Scott E. Lee's code only works for positive JDNs, more
 *    or less)
 *
 * 5. the object constructed for the given DD-MM-YYYY HH:MM:SS corresponds to
 *    this moment in local time and may be converted to the object
 *    corresponding to the same date/time in another time zone by using
 *    ToTimezone()
 *
 * 6. the conversions to the current (or any other) timezone are done when the
 *    internal time representation is converted to the broken-down one in
 *    wxDateTime::Tm.
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "datetime.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/thread.h"

#define wxDEFINE_TIME_CONSTANTS

#include "wx/datetime.h"

#ifndef WX_TIMEZONE
    #define WX_TIMEZONE timezone
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// some trivial ones
static const int MONTHS_IN_YEAR = 12;

static const int SECONDS_IN_MINUTE = 60;

static const long SECONDS_PER_DAY = 86400l;

static const long MILLISECONDS_PER_DAY = 86400000l;

// this is the integral part of JDN of the midnight of Jan 1, 1970
// (i.e. JDN(Jan 1, 1970) = 2440587.5)
static const int EPOCH_JDN = 2440587;

// the date of JDN -0.5 (as we don't work with fractional parts, this is the
// reference date for us) is Nov 24, 4714BC
static const int JDN_0_YEAR = -4713;
static const int JDN_0_MONTH = wxDateTime::Nov;
static const int JDN_0_DAY = 24;

// the constants used for JDN calculations
static const int JDN_OFFSET         = 32046;
static const int DAYS_PER_5_MONTHS  = 153;
static const int DAYS_PER_4_YEARS   = 1461;
static const int DAYS_PER_400_YEARS = 146097;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// a critical section is needed to protect GetTimeZone() static
// variable in MT case
#ifdef wxUSE_THREADS
    wxCriticalSection gs_critsectTimezone;
#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// get the number of days in the given month of the given year
static inline
wxDateTime::wxDateTime_t GetNumOfDaysInMonth(int year, wxDateTime::Month month)
{
    // the number of days in month in Julian/Gregorian calendar: the first line
    // is for normal years, the second one is for the leap ones
    static wxDateTime::wxDateTime_t daysInMonth[2][MONTHS_IN_YEAR] =
    {
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
        { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

    return daysInMonth[wxDateTime::IsLeapYear(year)][month];
}

// ensure that the timezone variable is set by calling localtime
static int GetTimeZone()
{
    // set to TRUE when the timezone is set
    static bool s_timezoneSet = FALSE;

    wxCRIT_SECT_LOCKER(lock, gs_critsectTimezone);

    if ( !s_timezoneSet )
    {
        // just call localtime() instead of figuring out whether this system
        // supports tzset(), _tzset() or something else
        time_t t;
        (void)localtime(&t);

        s_timezoneSet = TRUE;
    }

    return (int)WX_TIMEZONE;
}

// return the integral part of the JDN for the midnight of the given date (to
// get the real JDN you need to add 0.5, this is, in fact, JDN of the
// noon of the previous day)
static long GetTruncatedJDN(wxDateTime::wxDateTime_t day,
                            wxDateTime::Month mon,
                            int year)
{
    // CREDIT: code below is by Scott E. Lee (but bugs are mine)

    // check the date validity
    wxASSERT_MSG(
      (year > JDN_0_YEAR) ||
      ((year == JDN_0_YEAR) && (mon > JDN_0_MONTH)) ||
      ((year == JDN_0_YEAR) && (mon == JDN_0_MONTH) && (day >= JDN_0_DAY)),
      _T("date out of range - can't convert to JDN")
                );

    // make the year positive to avoid problems with negative numbers division
    year += 4800;

    // months are counted from March here
    int month;
    if ( mon >= wxDateTime::Mar )
    {
        month = mon - 2;
    }
    else
    {
        month = mon + 10;
        year--;
    }

    // now we can simply add all the contributions together
    return ((year / 100) * DAYS_PER_400_YEARS) / 4
            + ((year % 100) * DAYS_PER_4_YEARS) / 4
            + (month * DAYS_PER_5_MONTHS + 2) / 5
            + day
            - JDN_OFFSET;
}

// this function is a wrapper around strftime(3)
static wxString CallStrftime(const wxChar *format, const tm* tm)
{
    wxChar buf[1024];
    if ( !wxStrftime(buf, WXSIZEOF(buf), format, tm) )
    {
        // is ti really possible that 1024 is too short?
        wxFAIL_MSG(_T("strftime() failed"));
    }

    return wxString(buf);
}

// if year and/or month have invalid values, replace them with the current ones
static void ReplaceDefaultYearMonthWithCurrent(int *year,
                                               wxDateTime::Month *month)
{
    struct tm *tmNow = NULL;

    if ( *year == wxDateTime::Inv_Year )
    {
        tmNow = wxDateTime::GetTmNow();

        *year = 1900 + tmNow->tm_year;
    }

    if ( *month == wxDateTime::Inv_Month )
    {
        if ( !tmNow )
            tmNow = wxDateTime::GetTmNow();

        *month = (wxDateTime::Month)tmNow->tm_mon;
    }
}

// ============================================================================
// implementation of wxDateTime
// ============================================================================

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

wxDateTime::Country wxDateTime::ms_country = wxDateTime::Country_Unknown;
wxDateTime wxDateTime::ms_InvDateTime;

// ----------------------------------------------------------------------------
// struct Tm
// ----------------------------------------------------------------------------

wxDateTime::Tm::Tm()
{
    year = (wxDateTime_t)wxDateTime::Inv_Year;
    mon = wxDateTime::Inv_Month;
    mday = 0;
    hour = min = sec = msec = 0;
    wday = wxDateTime::Inv_WeekDay;
}

wxDateTime::Tm::Tm(const struct tm& tm, const TimeZone& tz)
              : m_tz(tz)
{
    msec = 0;
    sec = tm.tm_sec;
    min = tm.tm_min;
    hour = tm.tm_hour;
    mday = tm.tm_mday;
    mon = (wxDateTime::Month)tm.tm_mon;
    year = 1900 + tm.tm_year;
    wday = tm.tm_wday;
    yday = tm.tm_yday;
}

bool wxDateTime::Tm::IsValid() const
{
    // we allow for the leap seconds, although we don't use them (yet)
    return (year != wxDateTime::Inv_Year) && (mon != wxDateTime::Inv_Month) &&
           (mday <= GetNumOfDaysInMonth(year, mon)) &&
           (hour < 24) && (min < 60) && (sec < 62) && (msec < 1000);
}

void wxDateTime::Tm::ComputeWeekDay()
{
    // compute the week day from day/month/year: we use the dumbest algorithm
    // possible: just compute our JDN and then use the (simple to derive)
    // formula: weekday = (JDN + 1.5) % 7
    wday = (wxDateTime::WeekDay)(GetTruncatedJDN(mday, mon, year) + 2) % 7;
}

void wxDateTime::Tm::AddMonths(int monDiff)
{
    // normalize the months field
    while ( monDiff < -mon )
    {
        year--;

        monDiff += MONTHS_IN_YEAR;
    }

    while ( monDiff + mon > MONTHS_IN_YEAR )
    {
        year++;
    }

    mon = (wxDateTime::Month)(mon + monDiff);

    wxASSERT_MSG( mon >= 0 && mon < MONTHS_IN_YEAR, _T("logic error") );
}

void wxDateTime::Tm::AddDays(int dayDiff)
{
    // normalize the days field
    mday += dayDiff;
    while ( mday < 1 )
    {
        AddMonths(-1);

        mday += GetNumOfDaysInMonth(year, mon);
    }

    while ( mday > GetNumOfDaysInMonth(year, mon) )
    {
        mday -= GetNumOfDaysInMonth(year, mon);

        AddMonths(1);
    }

    wxASSERT_MSG( mday > 0 && mday <= GetNumOfDaysInMonth(year, mon),
                  _T("logic error") );
}

// ----------------------------------------------------------------------------
// class TimeZone
// ----------------------------------------------------------------------------

wxDateTime::TimeZone::TimeZone(wxDateTime::TZ tz)
{
    switch ( tz )
    {
        case wxDateTime::Local:
            // get the offset from C RTL: it returns the difference GMT-local
            // while we want to have the offset _from_ GMT, hence the '-'
            m_offset = -GetTimeZone();
            break;

        case wxDateTime::GMT_12:
        case wxDateTime::GMT_11:
        case wxDateTime::GMT_10:
        case wxDateTime::GMT_9:
        case wxDateTime::GMT_8:
        case wxDateTime::GMT_7:
        case wxDateTime::GMT_6:
        case wxDateTime::GMT_5:
        case wxDateTime::GMT_4:
        case wxDateTime::GMT_3:
        case wxDateTime::GMT_2:
        case wxDateTime::GMT_1:
            m_offset = -3600*(wxDateTime::GMT0 - tz);
            break;

        case wxDateTime::GMT0:
        case wxDateTime::GMT1:
        case wxDateTime::GMT2:
        case wxDateTime::GMT3:
        case wxDateTime::GMT4:
        case wxDateTime::GMT5:
        case wxDateTime::GMT6:
        case wxDateTime::GMT7:
        case wxDateTime::GMT8:
        case wxDateTime::GMT9:
        case wxDateTime::GMT10:
        case wxDateTime::GMT11:
        case wxDateTime::GMT12:
            m_offset = 3600*(tz - wxDateTime::GMT0);
            break;

        case wxDateTime::A_CST:
            // Central Standard Time in use in Australia = UTC + 9.5
            m_offset = 60*(9*60 + 30);
            break;

        default:
            wxFAIL_MSG( _T("unknown time zone") );
    }
}

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

/* static */
bool wxDateTime::IsLeapYear(int year, wxDateTime::Calendar cal)
{
    if ( year == Inv_Year )
        year = GetCurrentYear();

    if ( cal == Gregorian )
    {
        // in Gregorian calendar leap years are those divisible by 4 except
        // those divisible by 100 unless they're also divisible by 400
        // (in some countries, like Russia and Greece, additional corrections
        // exist, but they won't manifest themselves until 2700)
        return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
    }
    else if ( cal == Julian )
    {
        // in Julian calendar the rule is simpler
        return year % 4 == 0;
    }
    else
    {
        wxFAIL_MSG(_T("unknown calendar"));

        return FALSE;
    }
}

/* static */
int wxDateTime::GetCentury(int year)
{
    return year > 0 ? year / 100 : year / 100 - 1;
}

/* static */
void wxDateTime::SetCountry(wxDateTime::Country country)
{
    ms_country = country;
}

/* static */
int wxDateTime::ConvertYearToBC(int year)
{
    // year 0 is BC 1
    return year > 0 ? year : year - 1;
}

/* static */
int wxDateTime::GetCurrentYear(wxDateTime::Calendar cal)
{
    switch ( cal )
    {
        case Gregorian:
            return Now().GetYear();

        case Julian:
            wxFAIL_MSG(_T("TODO"));
            break;

        default:
            wxFAIL_MSG(_T("unsupported calendar"));
            break;
    }

    return Inv_Year;
}

/* static */
wxDateTime::Month wxDateTime::GetCurrentMonth(wxDateTime::Calendar cal)
{
    switch ( cal )
    {
        case Gregorian:
            return Now().GetMonth();
            break;

        case Julian:
            wxFAIL_MSG(_T("TODO"));
            break;

        default:
            wxFAIL_MSG(_T("unsupported calendar"));
            break;
    }

    return Inv_Month;
}

/* static */
wxDateTime::wxDateTime_t wxDateTime::GetNumberOfDays(int year, Calendar cal)
{
    if ( year == Inv_Year )
    {
        // take the current year if none given
        year = GetCurrentYear();
    }

    switch ( cal )
    {
        case Gregorian:
        case Julian:
            return IsLeapYear(year) ? 366 : 365;
            break;

        default:
            wxFAIL_MSG(_T("unsupported calendar"));
            break;
    }

    return 0;
}

/* static */
wxDateTime::wxDateTime_t wxDateTime::GetNumberOfDays(wxDateTime::Month month,
                                                     int year,
                                                     wxDateTime::Calendar cal)
{
    wxCHECK_MSG( month < MONTHS_IN_YEAR, 0, _T("invalid month") );

    if ( cal == Gregorian || cal == Julian )
    {
        if ( year == Inv_Year )
        {
            // take the current year if none given
            year = GetCurrentYear();
        }

        return GetNumOfDaysInMonth(year, month);
    }
    else
    {
        wxFAIL_MSG(_T("unsupported calendar"));

        return 0;
    }
}

/* static */
wxString wxDateTime::GetMonthName(wxDateTime::Month month, bool abbr)
{
    wxCHECK_MSG( month != Inv_Month, _T(""), _T("invalid month") );

    tm tm = { 0, 0, 0, 1, month, 76 };  // any year will do

    return CallStrftime(abbr ? _T("%b") : _T("%B"), &tm);
}

/* static */
wxString wxDateTime::GetWeekDayName(wxDateTime::WeekDay wday, bool abbr)
{
    wxCHECK_MSG( wday != Inv_WeekDay, _T(""), _T("invalid weekday") );

    // take some arbitrary Sunday
    tm tm = { 0, 0, 0, 28, Nov, 99 };

    // and offset it by the number of days needed to get the correct wday
    tm.tm_mday += wday;

    // call mktime() to normalize it...
    (void)mktime(&tm);

    // ... and call strftime()
    return CallStrftime(abbr ? _T("%a") : _T("%A"), &tm);
}

// ----------------------------------------------------------------------------
// constructors and assignment operators
// ----------------------------------------------------------------------------

// the values in the tm structure contain the local time
wxDateTime& wxDateTime::Set(const struct tm& tm)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    struct tm tm2(tm);
    time_t timet = mktime(&tm2);

    if ( timet == (time_t)-1 )
    {
        // mktime() rather unintuitively fails for Jan 1, 1970 if the hour is
        // less than timezone - try to make it work for this case
        if ( tm2.tm_year == 70 && tm2.tm_mon == 0 && tm2.tm_mday == 1 )
        {
            // add timezone to make sure that date is in range
            tm2.tm_sec -= GetTimeZone();

            timet = mktime(&tm2);
            if ( timet != (time_t)-1 )
            {
                timet += GetTimeZone();

                return Set(timet);
            }
        }

        wxFAIL_MSG( _T("mktime() failed") );

        return ms_InvDateTime;
    }
    else
    {
        return Set(timet);
    }
}

wxDateTime& wxDateTime::Set(wxDateTime_t hour,
                            wxDateTime_t minute,
                            wxDateTime_t second,
                            wxDateTime_t millisec)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    // we allow seconds to be 61 to account for the leap seconds, even if we
    // don't use them really
    wxCHECK_MSG( hour < 24 && second < 62 && minute < 60 && millisec < 1000,
                 ms_InvDateTime,
                 _T("Invalid time in wxDateTime::Set()") );

    // get the current date from system
    time_t timet = GetTimeNow();
    struct tm *tm = localtime(&timet);

    wxCHECK_MSG( tm, ms_InvDateTime, _T("localtime() failed") );

    // adjust the time
    tm->tm_hour = hour;
    tm->tm_min = minute;
    tm->tm_sec = second;

    (void)Set(*tm);

    // and finally adjust milliseconds
    return SetMillisecond(millisec);
}

wxDateTime& wxDateTime::Set(wxDateTime_t day,
                            Month        month,
                            int          year,
                            wxDateTime_t hour,
                            wxDateTime_t minute,
                            wxDateTime_t second,
                            wxDateTime_t millisec)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    wxCHECK_MSG( hour < 24 && second < 62 && minute < 60 && millisec < 1000,
                 ms_InvDateTime,
                 _T("Invalid time in wxDateTime::Set()") );

    ReplaceDefaultYearMonthWithCurrent(&year, &month);

    wxCHECK_MSG( (0 < day) && (day <= GetNumberOfDays(month, year)),
                 ms_InvDateTime,
                 _T("Invalid date in wxDateTime::Set()") );

    // the range of time_t type (inclusive)
    static const int yearMinInRange = 1970;
    static const int yearMaxInRange = 2037;

    // test only the year instead of testing for the exact end of the Unix
    // time_t range - it doesn't bring anything to do more precise checks
    if ( year >= yearMinInRange && year <= yearMaxInRange )
    {
        // use the standard library version if the date is in range - this is
        // probably more efficient than our code
        struct tm tm;
        tm.tm_year = year - 1900;
        tm.tm_mon = month;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = second;
        tm.tm_isdst = -1;       // mktime() will guess it

        (void)Set(tm);

        // and finally adjust milliseconds
        return SetMillisecond(millisec);
    }
    else
    {
        // do time calculations ourselves: we want to calculate the number of
        // milliseconds between the given date and the epoch

        // get the JDN for the midnight of this day
        m_time = GetTruncatedJDN(day, month, year);
        m_time -= EPOCH_JDN;
        m_time *= SECONDS_PER_DAY * TIME_T_FACTOR;

        // JDN corresponds to GMT, we take localtime
        Add(wxTimeSpan(hour, minute, second + GetTimeZone(), millisec));
    }

    return *this;
}

wxDateTime& wxDateTime::Set(double jdn)
{
    // so that m_time will be 0 for the midnight of Jan 1, 1970 which is jdn
    // EPOCH_JDN + 0.5
    jdn -= EPOCH_JDN + 0.5;

    m_time = jdn;
    m_time *= MILLISECONDS_PER_DAY;

    return *this;
}

// ----------------------------------------------------------------------------
// time_t <-> broken down time conversions
// ----------------------------------------------------------------------------

wxDateTime::Tm wxDateTime::GetTm(const TimeZone& tz) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    time_t time = GetTicks();
    if ( time != (time_t)-1 )
    {
        // use C RTL functions
        tm *tm;
        if ( tz.GetOffset() == -GetTimeZone() )
        {
            // we are working with local time
            tm = localtime(&time);

            // should never happen
            wxCHECK_MSG( tm, Tm(), _T("gmtime() failed") );
        }
        else
        {
            time += tz.GetOffset();
            if ( time >= 0 )
            {
                tm = gmtime(&time);

                // should never happen
                wxCHECK_MSG( tm, Tm(), _T("gmtime() failed") );
            }
            else
            {
                tm = (struct tm *)NULL;
            }
        }

        if ( tm )
        {
            return Tm(*tm, tz);
        }
        //else: use generic code below
    }

    // remember the time and do the calculations with the date only - this
    // eliminates rounding errors of the floating point arithmetics

    wxLongLong timeMidnight = m_time + tz.GetOffset() * 1000;

    long timeOnly = (timeMidnight % MILLISECONDS_PER_DAY).ToLong();

    // we want to always have positive time and timeMidnight to be really
    // the midnight before it
    if ( timeOnly < 0 )
    {
        timeOnly = MILLISECONDS_PER_DAY + timeOnly;
    }

    timeMidnight -= timeOnly;

    // calculate the Gregorian date from JDN for the midnight of our date:
    // this will yield day, month (in 1..12 range) and year

    // actually, this is the JDN for the noon of the previous day
    long jdn = (timeMidnight / MILLISECONDS_PER_DAY).ToLong() + EPOCH_JDN;

    // CREDIT: code below is by Scott E. Lee (but bugs are mine)

    wxASSERT_MSG( jdn > -2, _T("JDN out of range") );

    // calculate the century
    int temp = (jdn + JDN_OFFSET) * 4 - 1;
    int century = temp / DAYS_PER_400_YEARS;

    // then the year and day of year (1 <= dayOfYear <= 366)
    temp = ((temp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
    int year = (century * 100) + (temp / DAYS_PER_4_YEARS);
    int dayOfYear = (temp % DAYS_PER_4_YEARS) / 4 + 1;

    // and finally the month and day of the month
    temp = dayOfYear * 5 - 3;
    int month = temp / DAYS_PER_5_MONTHS;
    int day = (temp % DAYS_PER_5_MONTHS) / 5 + 1;

    // month is counted from March - convert to normal
    if ( month < 10 )
    {
        month += 3;
    }
    else
    {
        year += 1;
        month -= 9;
    }

    // year is offset by 4800
    year -= 4800;

    // check that the algorithm gave us something reasonable
    wxASSERT_MSG( (0 < month) && (month <= 12), _T("invalid month") );
    wxASSERT_MSG( (1 <= day) && (day < 32), _T("invalid day") );
    wxASSERT_MSG( (INT_MIN <= year) && (year <= INT_MAX),
                  _T("year range overflow") );

    // construct Tm from these values
    Tm tm;
    tm.year = (int)year;
    tm.mon = (Month)(month - 1); // algorithm yields 1 for January, not 0
    tm.mday = (wxDateTime_t)day;
    tm.msec = timeOnly % 1000;
    timeOnly -= tm.msec;
    timeOnly /= 1000;               // now we have time in seconds

    tm.sec = timeOnly % 60;
    timeOnly -= tm.sec;
    timeOnly /= 60;                 // now we have time in minutes

    tm.min = timeOnly % 60;
    timeOnly -= tm.min;

    tm.hour = timeOnly / 60;

    return tm;
}

wxDateTime& wxDateTime::SetYear(int year)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.year = year;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetMonth(Month month)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.mon = month;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetDay(wxDateTime_t mday)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.mday = mday;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetHour(wxDateTime_t hour)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.hour = hour;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetMinute(wxDateTime_t min)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.min = min;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetSecond(wxDateTime_t sec)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    Tm tm(GetTm());
    tm.sec = sec;
    Set(tm);

    return *this;
}

wxDateTime& wxDateTime::SetMillisecond(wxDateTime_t millisecond)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    // we don't need to use GetTm() for this one
    m_time -= m_time % 1000l;
    m_time += millisecond;

    return *this;
}

// ----------------------------------------------------------------------------
// wxDateTime arithmetics
// ----------------------------------------------------------------------------

wxDateTime& wxDateTime::Add(const wxDateSpan& diff)
{
    Tm tm(GetTm());

    tm.year += diff.GetYears();
    tm.AddMonths(diff.GetMonths());
    tm.AddDays(diff.GetTotalDays());

    Set(tm);

    return *this;
}

// ----------------------------------------------------------------------------
// Weekday and monthday stuff
// ----------------------------------------------------------------------------

wxDateTime& wxDateTime::SetToLastMonthDay(Month month,
                                          int year)
{
    // take the current month/year if none specified
    ReplaceDefaultYearMonthWithCurrent(&year, &month);

    return Set(GetNumOfDaysInMonth(year, month), month, year);
}

bool wxDateTime::SetToWeekDay(WeekDay weekday,
                              int n,
                              Month month,
                              int year)
{
    wxCHECK_MSG( weekday != Inv_WeekDay, FALSE, _T("invalid weekday") );

    // we don't check explicitly that -5 <= n <= 5 because we will return FALSE
    // anyhow in such case - but may be should still give an assert for it?

    // take the current month/year if none specified
    ReplaceDefaultYearMonthWithCurrent(&year, &month);

    wxDateTime dt;

    // TODO this probably could be optimised somehow...

    if ( n > 0 )
    {
        // get the first day of the month
        dt.Set(1, month, year);

        // get its wday
        WeekDay wdayFirst = dt.GetWeekDay();

        // go to the first weekday of the month
        int diff = weekday - wdayFirst;
        if ( diff < 0 )
            diff += 7;

        // add advance n-1 weeks more
        diff += 7*(n - 1);

        dt -= wxDateSpan::Days(diff);
    }
    else
    {
        // get the last day of the month
        dt.SetToLastMonthDay(month, year);

        // get its wday
        WeekDay wdayLast = dt.GetWeekDay();

        // go to the last weekday of the month
        int diff = wdayLast - weekday;
        if ( diff < 0 )
            diff += 7;

        // and rewind n-1 weeks from there
        diff += 7*(n - 1);

        dt -= wxDateSpan::Days(diff);
    }

    // check that it is still in the same month
    if ( dt.GetMonth() == month )
    {
        *this = dt;

        return TRUE;
    }
    else
    {
        // no such day in this month
        return FALSE;
    }
}

// ----------------------------------------------------------------------------
// Julian day number conversion and related stuff
// ----------------------------------------------------------------------------

double wxDateTime::GetJulianDayNumber() const
{
    // JDN are always expressed for the GMT dates
    Tm tm(ToTimezone(GMT0).GetTm(GMT0));

    double result = GetTruncatedJDN(tm.mday, tm.mon, tm.year);

    // add the part GetTruncatedJDN() neglected
    result += 0.5;

    // and now add the time: 86400 sec = 1 JDN
    return result + ((double)(60*(60*tm.hour + tm.min) + tm.sec)) / 86400;
}

double wxDateTime::GetRataDie() const
{
    // March 1 of the year 0 is Rata Die day -306 and JDN 1721119.5
    return GetJulianDayNumber() - 1721119.5 - 306;
}

// ----------------------------------------------------------------------------
// timezone and DST stuff
// ----------------------------------------------------------------------------

int wxDateTime::IsDST(wxDateTime::Country country) const
{
    wxCHECK_MSG( country == Country_Default, -1,
                 _T("country support not implemented") );

    // use the C RTL for the dates in the standard range
    time_t timet = GetTicks();
    if ( timet != (time_t)-1 )
    {
        tm *tm = localtime(&timet);

        wxCHECK_MSG( tm, -1, _T("localtime() failed") );

        return tm->tm_isdst;
    }
    else
    {
        // wxFAIL_MSG( _T("TODO") );

        return -1;
    }
}

wxDateTime& wxDateTime::MakeTimezone(const TimeZone& tz)
{
    int secDiff = GetTimeZone() + tz.GetOffset();

    // we need to know whether DST is or not in effect for this date
    if ( IsDST() == 1 )
    {
        // FIXME we assume that the DST is always shifted by 1 hour
        secDiff -= 3600;
    }

    return Substract(wxTimeSpan::Seconds(secDiff));
}

// ----------------------------------------------------------------------------
// wxDateTime to/from text representations
// ----------------------------------------------------------------------------

wxString wxDateTime::Format(const wxChar *format, const TimeZone& tz) const
{
    wxCHECK_MSG( format, _T(""), _T("NULL format in wxDateTime::Format") );

    time_t time = GetTicks();
    if ( time != (time_t)-1 )
    {
        // use strftime()
        tm *tm;
        if ( tz.GetOffset() == -GetTimeZone() )
        {
            // we are working with local time
            tm = localtime(&time);

            // should never happen
            wxCHECK_MSG( tm, wxEmptyString, _T("localtime() failed") );
        }
        else
        {
            time += tz.GetOffset();

            if ( time >= 0 )
            {
                tm = gmtime(&time);

                // should never happen
                wxCHECK_MSG( tm, wxEmptyString, _T("gmtime() failed") );
            }
            else
            {
                tm = (struct tm *)NULL;
            }
        }

        if ( tm )
        {
            return CallStrftime(format, tm);
        }
        //else: use generic code below
    }

    // use a hack and still use strftime(): first find the YEAR which is a year
    // in the strftime() range (1970 - 2038) whose Jan 1 falls on the same week
    // day as the Jan 1 of the real year. Then make a copy of the format and
    // replace all occurences of YEAR in it with some unique string not
    // appearing anywhere else in it, then use strftime() to format the date in
    // year YEAR and then replace YEAR back by the real year and the unique
    // replacement string back with YEAR. Notice that "all occurences of YEAR"
    // means all occurences of 4 digit as well as 2 digit form!

    // NB: may be it would be simpler to "honestly" reimplement strftime()?

    // find the YEAR: normally, for any year X, Jan 1 or the year X + 28 is the
    // same weekday as Jan 1 of X (because the weekday advances by 1 for each
    // normal X and by 2 for each leap X, hence by 5 every 4 years or by 35
    // which is 0 mod 7 every 28 years) but this rule breaks down if there are
    // years between X and Y which are divisible by 4 but not leap (i.e.
    // divisible by 100 but not 400), hence the correction.

    int yearReal = GetYear(tz);
    int year = 1970 + yearReal % 28;

    int nCenturiesInBetween = (year / 100) - (yearReal / 100);
    int nLostWeekDays = nCenturiesInBetween - (nCenturiesInBetween / 400);

    // we have to gain back the "lost" weekdays...
    while ( (nLostWeekDays % 7) != 0 )
    {
        nLostWeekDays += year++ % 4 ? 1 : 2;
    }

    // at any rate, we can't go further than 1997 + 28!
    wxASSERT_MSG( year < 2030, _T("logic error in wxDateTime::Format") );

    wxString strYear, strYear2;
    strYear.Printf(_T("%d"), year);
    strYear2.Printf(_T("%d"), year % 100);

    // find two strings not occuring in format (this is surely not optimal way
    // of doing it... improvements welcome!)
    wxString fmt = format;
    wxString replacement = (wxChar)-1;
    while ( fmt.Find(replacement) != wxNOT_FOUND )
    {
        replacement << (wxChar)-1;
    }

    wxString replacement2 = (wxChar)-2;
    while ( fmt.Find(replacement) != wxNOT_FOUND )
    {
        replacement << (wxChar)-2;
    }

    // replace all occurences of year with it
    bool wasReplaced = fmt.Replace(strYear, replacement) > 0;
    if ( !wasReplaced )
        wasReplaced = fmt.Replace(strYear2, replacement2) > 0;

    // use strftime() to format the same date but in supported year
    wxDateTime dt(*this);
    dt.SetYear(year);
    wxString str = dt.Format(format, tz);

    // now replace the occurence of 1999 with the real year
    wxString strYearReal, strYearReal2;
    strYearReal.Printf(_T("%04d"), yearReal);
    strYearReal2.Printf(_T("%02d"), yearReal % 100);
    str.Replace(strYear, strYearReal);
    str.Replace(strYear2, strYearReal2);

    // and replace back all occurences of replacement string
    if ( wasReplaced )
    {
        str.Replace(replacement2, strYear2);
        str.Replace(replacement, strYear);
    }

    return str;
}

// ============================================================================
// wxTimeSpan
// ============================================================================

// not all strftime(3) format specifiers make sense here because, for example,
// a time span doesn't have a year nor a timezone
//
// Here are the ones which are supported (all of them are supported by strftime
// as well):
//  %H          hour in 24 hour format
//  %M          minute (00 - 59)
//  %S          second (00 - 59)
//  %%          percent sign
//
// Also, for MFC CTimeSpan compatibility, we support
//  %D          number of days
//
// And, to be better than MFC :-), we also have
//  %E          number of wEeks
//  %l          milliseconds (000 - 999)
wxString wxTimeSpan::Format(const wxChar *format) const
{
    wxCHECK_MSG( format, _T(""), _T("NULL format in wxTimeSpan::Format") );

    wxString str;
    str.Alloc(strlen(format));

    for ( const wxChar *pch = format; pch; pch++ )
    {
        wxChar ch = *pch;

        if ( ch == '%' )
        {
            wxString tmp;

            ch = *pch++;
            switch ( ch )
            {
                default:
                    wxFAIL_MSG( _T("invalid format character") );
                    // fall through

                case '%':
                    // will get to str << ch below
                    break;

                case 'D':
                    tmp.Printf(_T("%d"), GetDays());
                    break;

                case 'E':
                    tmp.Printf(_T("%d"), GetWeeks());
                    break;

                case 'H':
                    tmp.Printf(_T("%02d"), GetHours());
                    break;

                case 'l':
                    tmp.Printf(_T("%03d"), GetMilliseconds());
                    break;

                case 'M':
                    tmp.Printf(_T("%02d"), GetMinutes());
                    break;

                case 'S':
                    tmp.Printf(_T("%02d"), GetSeconds());
                    break;
            }

            if ( !!tmp )
            {
                str += tmp;

                // skip str += ch below
                continue;
            }
        }

        str += ch;
    }

    return str;
}
