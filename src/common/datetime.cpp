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
#include "wx/tokenzr.h"

#define wxDEFINE_TIME_CONSTANTS // before including datetime.h

#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#if defined(HAVE_STRPTIME) && defined(__LINUX__)
    // glibc 2.0.7 strptime() is broken - the following snippet causes it to
    // crash (instead of just failing):
    //
    //      strncpy(buf, "Tue Dec 21 20:25:40 1999", 128);
    //      strptime(buf, "%x", &tm);
    //
    // so don't use it
    #undef HAVE_STRPTIME
#endif // broken strptime()

#ifndef WX_TIMEZONE
    #if defined(__BORLANDC__) || defined(__MINGW32__) || defined(__VISAGECPP__)
        #define WX_TIMEZONE _timezone
    #else // unknown platform - try timezone
        #define WX_TIMEZONE timezone
    #endif
#endif // !WX_TIMEZONE

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
static const long EPOCH_JDN = 2440587l;

// the date of JDN -0.5 (as we don't work with fractional parts, this is the
// reference date for us) is Nov 24, 4714BC
static const int JDN_0_YEAR = -4713;
static const int JDN_0_MONTH = wxDateTime::Nov;
static const int JDN_0_DAY = 24;

// the constants used for JDN calculations
static const long JDN_OFFSET         = 32046l;
static const long DAYS_PER_5_MONTHS  = 153l;
static const long DAYS_PER_4_YEARS   = 1461l;
static const long DAYS_PER_400_YEARS = 146097l;

// this array contains the cumulated number of days in all previous months for
// normal and leap years
static const wxDateTime::wxDateTime_t gs_cumulatedDays[2][MONTHS_IN_YEAR] =
{
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// a critical section is needed to protect GetTimeZone() static
// variable in MT case
#if wxUSE_THREADS
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
    wxChar buf[4096];
    if ( !wxStrftime(buf, WXSIZEOF(buf), format, tm) )
    {
        // buffer is too small?
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

// fll the struct tm with default values
static void InitTm(struct tm& tm)
{
    // struct tm may have etxra fields (undocumented and with unportable
    // names) which, nevertheless, must be set to 0
    memset(&tm, 0, sizeof(struct tm));

    tm.tm_mday = 1;   // mday 0 is invalid
    tm.tm_year = 76;  // any valid year
    tm.tm_isdst = -1; // auto determine
}

// parsing helpers
// ---------------

// return the month if the string is a month name or Inv_Month otherwise
static wxDateTime::Month GetMonthFromName(const wxString& name, int flags)
{
    wxDateTime::Month mon;
    for ( mon = wxDateTime::Jan; mon < wxDateTime::Inv_Month; wxNextMonth(mon) )
    {
        // case-insensitive comparison either one of or with both abbreviated
        // and not versions
        if ( flags & wxDateTime::Name_Full )
        {
            if ( name.CmpNoCase(wxDateTime::
                        GetMonthName(mon, wxDateTime::Name_Full)) == 0 )
            {
                break;
            }
        }

        if ( flags & wxDateTime::Name_Abbr )
        {
            if ( name.CmpNoCase(wxDateTime::
                        GetMonthName(mon, wxDateTime::Name_Abbr)) == 0 )
            {
                break;
            }
        }
    }

    return mon;
}

// return the weekday if the string is a weekday name or Inv_WeekDay otherwise
static wxDateTime::WeekDay GetWeekDayFromName(const wxString& name, int flags)
{
    wxDateTime::WeekDay wd;
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        // case-insensitive comparison either one of or with both abbreviated
        // and not versions
        if ( flags & wxDateTime::Name_Full )
        {
            if ( name.CmpNoCase(wxDateTime::
                        GetWeekDayName(wd, wxDateTime::Name_Full)) == 0 )
            {
                break;
            }
        }

        if ( flags & wxDateTime::Name_Abbr )
        {
            if ( name.CmpNoCase(wxDateTime::
                        GetWeekDayName(wd, wxDateTime::Name_Abbr)) == 0 )
            {
                break;
            }
        }
    }

    return wd;
}

// scans all digits and returns the resulting number
static bool GetNumericToken(const wxChar*& p, unsigned long *number)
{
    wxString s;
    while ( wxIsdigit(*p) )
    {
        s += *p++;
    }

    return !!s && s.ToULong(number);
}

// scans all alphabetic characters and returns the resulting string
static wxString GetAlphaToken(const wxChar*& p)
{
    wxString s;
    while ( wxIsalpha(*p) )
    {
        s += *p++;
    }

    return s;
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

        monDiff -= MONTHS_IN_YEAR;
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
            m_offset = 60l*(9*60 + 30);
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
wxString wxDateTime::GetMonthName(wxDateTime::Month month,
                                  wxDateTime::NameFlags flags)
{
    wxCHECK_MSG( month != Inv_Month, _T(""), _T("invalid month") );

    // notice that we must set all the fields to avoid confusing libc (GNU one
    // gets confused to a crash if we don't do this)
    tm tm;
    InitTm(tm);
    tm.tm_mon = month;

    return CallStrftime(flags == Name_Abbr ? _T("%b") : _T("%B"), &tm);
}

/* static */
wxString wxDateTime::GetWeekDayName(wxDateTime::WeekDay wday,
                                    wxDateTime::NameFlags flags)
{
    wxCHECK_MSG( wday != Inv_WeekDay, _T(""), _T("invalid weekday") );

    // take some arbitrary Sunday
    tm tm;
    InitTm(tm);
    tm.tm_mday = 28;
    tm.tm_mon = Nov;
    tm.tm_year = 99;

    // and offset it by the number of days needed to get the correct wday
    tm.tm_mday += wday;

    // call mktime() to normalize it...
    (void)mktime(&tm);

    // ... and call strftime()
    return CallStrftime(flags == Name_Abbr ? _T("%a") : _T("%A"), &tm);
}

/* static */
void wxDateTime::GetAmPmStrings(wxString *am, wxString *pm)
{
    tm tm;
    InitTm(tm);
    if ( am )
    {
        *am = CallStrftime(_T("%p"), &tm);
    }
    if ( pm )
    {
        tm.tm_hour = 13;
        *pm = CallStrftime(_T("%p"), &tm);
    }
}

// ----------------------------------------------------------------------------
// Country stuff: date calculations depend on the country (DST, work days,
// ...), so we need to know which rules to follow.
// ----------------------------------------------------------------------------

/* static */
wxDateTime::Country wxDateTime::GetCountry()
{
    // TODO use LOCALE_ICOUNTRY setting under Win32

    if ( ms_country == Country_Unknown )
    {
        // try to guess from the time zone name
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);

        wxString tz = CallStrftime(_T("%Z"), tm);
        if ( tz == _T("WET") || tz == _T("WEST") )
        {
            ms_country = UK;
        }
        else if ( tz == _T("CET") || tz == _T("CEST") )
        {
            ms_country = Country_EEC;
        }
        else if ( tz == _T("MSK") || tz == _T("MSD") )
        {
            ms_country = Russia;
        }
        else if ( tz == _T("AST") || tz == _T("ADT") ||
                  tz == _T("EST") || tz == _T("EDT") ||
                  tz == _T("CST") || tz == _T("CDT") ||
                  tz == _T("MST") || tz == _T("MDT") ||
                  tz == _T("PST") || tz == _T("PDT") )
        {
            ms_country = USA;
        }
        else
        {
            // well, choose a default one
            ms_country = USA;
        }
    }

    return ms_country;
}

/* static */
void wxDateTime::SetCountry(wxDateTime::Country country)
{
    ms_country = country;
}

/* static */
bool wxDateTime::IsWestEuropeanCountry(Country country)
{
    if ( country == Country_Default )
    {
        country = GetCountry();
    }

    return (Country_WesternEurope_Start <= country) &&
           (country <= Country_WesternEurope_End);
}

// ----------------------------------------------------------------------------
// DST calculations: we use 3 different rules for the West European countries,
// USA and for the rest of the world. This is undoubtedly false for many
// countries, but I lack the necessary info (and the time to gather it),
// please add the other rules here!
// ----------------------------------------------------------------------------

/* static */
bool wxDateTime::IsDSTApplicable(int year, Country country)
{
    if ( year == Inv_Year )
    {
        // take the current year if none given
        year = GetCurrentYear();
    }

    if ( country == Country_Default )
    {
        country = GetCountry();
    }

    switch ( country )
    {
        case USA:
        case UK:
            // DST was first observed in the US and UK during WWI, reused
            // during WWII and used again since 1966
            return year >= 1966 ||
                   (year >= 1942 && year <= 1945) ||
                   (year == 1918 || year == 1919);

        default:
            // assume that it started after WWII
            return year > 1950;
    }
}

/* static */
wxDateTime wxDateTime::GetBeginDST(int year, Country country)
{
    if ( year == Inv_Year )
    {
        // take the current year if none given
        year = GetCurrentYear();
    }

    if ( country == Country_Default )
    {
        country = GetCountry();
    }

    if ( !IsDSTApplicable(year, country) )
    {
        return ms_InvDateTime;
    }

    wxDateTime dt;

    if ( IsWestEuropeanCountry(country) || (country == Russia) )
    {
        // DST begins at 1 a.m. GMT on the last Sunday of March
        if ( !dt.SetToLastWeekDay(Sun, Mar, year) )
        {
            // weird...
            wxFAIL_MSG( _T("no last Sunday in March?") );
        }

        dt += wxTimeSpan::Hours(1);

        // disable DST tests because it could result in an infinite recursion!
        dt.MakeGMT(TRUE);
    }
    else switch ( country )
    {
        case USA:
            switch ( year )
            {
                case 1918:
                case 1919:
                    // don't know for sure - assume it was in effect all year

                case 1943:
                case 1944:
                case 1945:
                    dt.Set(1, Jan, year);
                    break;

                case 1942:
                    // DST was installed Feb 2, 1942 by the Congress
                    dt.Set(2, Feb, year);
                    break;

                    // Oil embargo changed the DST period in the US
                case 1974:
                    dt.Set(6, Jan, 1974);
                    break;

                case 1975:
                    dt.Set(23, Feb, 1975);
                    break;

                default:
                    // before 1986, DST begun on the last Sunday of April, but
                    // in 1986 Reagan changed it to begin at 2 a.m. of the
                    // first Sunday in April
                    if ( year < 1986 )
                    {
                        if ( !dt.SetToLastWeekDay(Sun, Apr, year) )
                        {
                            // weird...
                            wxFAIL_MSG( _T("no first Sunday in April?") );
                        }
                    }
                    else
                    {
                        if ( !dt.SetToWeekDay(Sun, 1, Apr, year) )
                        {
                            // weird...
                            wxFAIL_MSG( _T("no first Sunday in April?") );
                        }
                    }

                    dt += wxTimeSpan::Hours(2);

                    // TODO what about timezone??
            }

            break;

        default:
            // assume Mar 30 as the start of the DST for the rest of the world
            // - totally bogus, of course
            dt.Set(30, Mar, year);
    }

    return dt;
}

/* static */
wxDateTime wxDateTime::GetEndDST(int year, Country country)
{
    if ( year == Inv_Year )
    {
        // take the current year if none given
        year = GetCurrentYear();
    }

    if ( country == Country_Default )
    {
        country = GetCountry();
    }

    if ( !IsDSTApplicable(year, country) )
    {
        return ms_InvDateTime;
    }

    wxDateTime dt;

    if ( IsWestEuropeanCountry(country) || (country == Russia) )
    {
        // DST ends at 1 a.m. GMT on the last Sunday of October
        if ( !dt.SetToLastWeekDay(Sun, Oct, year) )
        {
            // weirder and weirder...
            wxFAIL_MSG( _T("no last Sunday in October?") );
        }

        dt += wxTimeSpan::Hours(1);

        // disable DST tests because it could result in an infinite recursion!
        dt.MakeGMT(TRUE);
    }
    else switch ( country )
    {
        case USA:
            switch ( year )
            {
                case 1918:
                case 1919:
                    // don't know for sure - assume it was in effect all year

                case 1943:
                case 1944:
                    dt.Set(31, Dec, year);
                    break;

                case 1945:
                    // the time was reset after the end of the WWII
                    dt.Set(30, Sep, year);
                    break;

                default:
                    // DST ends at 2 a.m. on the last Sunday of October
                    if ( !dt.SetToLastWeekDay(Sun, Oct, year) )
                    {
                        // weirder and weirder...
                        wxFAIL_MSG( _T("no last Sunday in October?") );
                    }

                    dt += wxTimeSpan::Hours(2);

                    // TODO what about timezone??
            }
            break;

        default:
            // assume October 26th as the end of the DST - totally bogus too
            dt.Set(26, Oct, year);
    }

    return dt;
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

    jdn *= MILLISECONDS_PER_DAY;

    m_time.Assign(jdn);

    return *this;
}

// ----------------------------------------------------------------------------
// time_t <-> broken down time conversions
// ----------------------------------------------------------------------------

wxDateTime::Tm wxDateTime::GetTm(const TimeZone& tz) const
{
#ifdef __VMS__
   int time2;
#endif
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
#ifdef __VMS__ /* time is unsigned so VMS gives a warning on the original */
	   time2 = (int) time;
	   if ( time2 >= 0 )
#else
	   if ( time >= 0 )
#endif
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

wxDateTime& wxDateTime::SetToWeekDayInSameWeek(WeekDay weekday)
{
    wxCHECK_MSG( weekday != Inv_WeekDay, ms_InvDateTime, _T("invalid weekday") );

    WeekDay wdayThis = GetWeekDay();
    if ( weekday == wdayThis )
    {
        // nothing to do
        return *this;
    }
    else if ( weekday < wdayThis )
    {
        return Substract(wxTimeSpan::Days(wdayThis - weekday));
    }
    else // weekday > wdayThis
    {
        return Add(wxTimeSpan::Days(weekday - wdayThis));
    }
}

wxDateTime& wxDateTime::SetToNextWeekDay(WeekDay weekday)
{
    wxCHECK_MSG( weekday != Inv_WeekDay, ms_InvDateTime, _T("invalid weekday") );

    int diff;
    WeekDay wdayThis = GetWeekDay();
    if ( weekday == wdayThis )
    {
        // nothing to do
        return *this;
    }
    else if ( weekday < wdayThis )
    {
        // need to advance a week
        diff = 7 - (wdayThis - weekday);
    }
    else // weekday > wdayThis
    {
        diff = weekday - wdayThis;
    }

    return Add(wxTimeSpan::Days(diff));
}

wxDateTime& wxDateTime::SetToPrevWeekDay(WeekDay weekday)
{
    wxCHECK_MSG( weekday != Inv_WeekDay, ms_InvDateTime, _T("invalid weekday") );

    int diff;
    WeekDay wdayThis = GetWeekDay();
    if ( weekday == wdayThis )
    {
        // nothing to do
        return *this;
    }
    else if ( weekday > wdayThis )
    {
        // need to go to previous week
        diff = 7 - (weekday - wdayThis);
    }
    else // weekday < wdayThis
    {
        diff = wdayThis - weekday;
    }

    return Substract(wxTimeSpan::Days(diff));
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

        dt += wxDateSpan::Days(diff);
    }
    else // count from the end of the month
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
        diff += 7*(-n - 1);

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

wxDateTime::wxDateTime_t wxDateTime::GetDayOfYear(const TimeZone& tz) const
{
    Tm tm(GetTm(tz));

    return gs_cumulatedDays[IsLeapYear(tm.year)][tm.mon] + tm.mday;
}

wxDateTime::wxDateTime_t wxDateTime::GetWeekOfYear(const TimeZone& tz) const
{
#if 1
    // the first week of the year is the one which contains Jan, 4 (according
    // to ISO standard rule), so the year day N0 = 4 + 7*W always lies in the
    // week W+1. As any day N = 7*W + 4 + (N - 4)%7, it lies in the same week
    // as N0 or in the next one.

    // TODO this surely may be optimized - I got confused while writing it

    wxDateTime_t nDayInYear = GetDayOfYear(tz);

    // the week day of the day lying in the first week
    WeekDay wdayStart = wxDateTime(4, Jan, GetYear()).GetWeekDay();

    wxDateTime_t week = (nDayInYear - 4) / 7 + 1;

    // notice that Sunday shoould be counted as 7, not 0 here!
    if ( ((nDayInYear - 4) % 7) + (!wdayStart ? 7 : wdayStart) > 7 )
    {
        week++;
    }

    return week;
#else // 0
    // an attempt at doing it simpler - but this doesn't quite work...
    return (WeekDay)((GetDayOfYear(tz) - (GetWeekDay(tz) - 1 + 7) % 7 + 7) / 7);
#endif // 0/1
}

wxDateTime::wxDateTime_t wxDateTime::GetWeekOfMonth(const TimeZone& tz) const
{
    size_t nWeek = 0;

    wxDateTime dt(*this);
    do
    {
        nWeek++;

        dt -= wxTimeSpan::Week();
    }
    while ( dt.GetMonth(tz) == GetMonth(tz) );

    return nWeek;
}

wxDateTime& wxDateTime::SetToYearDay(wxDateTime::wxDateTime_t yday)
{
    int year = GetYear();
    wxCHECK_MSG( (0 < yday) && (yday <= GetNumberOfDays(year)),
                 ms_InvDateTime, _T("invalid year day") );

    bool isLeap = IsLeapYear(year);
    for ( Month mon = Jan; mon < Inv_Month; wxNextMonth(mon) )
    {
        // for Dec, we can't compare with gs_cumulatedDays[mon + 1], but we
        // don't need it neither - because of the CHECK above we know that
        // yday lies in December then
        if ( (mon == Dec) || (yday < gs_cumulatedDays[isLeap][mon + 1]) )
        {
            Set(yday - gs_cumulatedDays[isLeap][mon], mon, year);

            break;
        }
    }

    return *this;
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
        int year = GetYear();

        if ( !IsDSTApplicable(year, country) )
        {
            // no DST time in this year in this country
            return -1;
        }

        return IsBetween(GetBeginDST(year, country), GetEndDST(year, country));
    }
}

wxDateTime& wxDateTime::MakeTimezone(const TimeZone& tz, bool noDST)
{
    int secDiff = GetTimeZone() + tz.GetOffset();

    // we need to know whether DST is or not in effect for this date unless
    // the test disabled by the caller
    if ( !noDST && (IsDST() == 1) )
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
#ifdef __VMS__
   int time2;
#endif
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

#ifdef __VMS__ /* time is unsigned so VMS gives a warning on the original */
            time2 = (int) time;
            if ( time2 >= 0 )
#else
            if ( time >= 0 )
#endif
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

    // we only parse ANSI C format specifications here, no POSIX 2
    // complications, no GNU extensions
    Tm tm = GetTm(tz);

    // used for calls to strftime() when we only deal with time
    struct tm tmTimeOnly;
    tmTimeOnly.tm_hour = tm.hour;
    tmTimeOnly.tm_min = tm.min;
    tmTimeOnly.tm_sec = tm.sec;
    tmTimeOnly.tm_wday = 0;
    tmTimeOnly.tm_yday = 0;
    tmTimeOnly.tm_mday = 1;         // any date will do
    tmTimeOnly.tm_mon = 0;
    tmTimeOnly.tm_year = 76;
    tmTimeOnly.tm_isdst = 0;        // no DST, we adjust for tz ourselves

    wxString tmp, res, fmt;
    for ( const wxChar *p = format; *p; p++ )
    {
        if ( *p != _T('%') )
        {
            // copy as is
            res += *p;

            continue;
        }

        // set the default format
        switch ( *++p )
        {
            case _T('Y'):               // year has 4 digits
                fmt = _T("%04d");
                break;

            case _T('j'):               // day of year has 3 digits
                fmt = _T("%03d");
                break;

            default:
                // it's either another valid format specifier in which case
                // the format is "%02d" (for all the rest) or we have the
                // field width preceding the format in which case it will
                // override the default format anyhow
                fmt = _T("%02d");
        }

restart:
        // start of the format specification
        switch ( *p )
        {
            case _T('a'):       // a weekday name
            case _T('A'):
                // second parameter should be TRUE for abbreviated names
                res += GetWeekDayName(tm.GetWeekDay(),
                                      *p == _T('a') ? Name_Abbr : Name_Full);
                break;

            case _T('b'):       // a month name
            case _T('B'):
                res += GetMonthName(tm.mon,
                                    *p == _T('b') ? Name_Abbr : Name_Full);
                break;

            case _T('c'):       // locale default date and time  representation
            case _T('x'):       // locale default date representation
                //
                // the problem: there is no way to know what do these format
                // specifications correspond to for the current locale.
                //
                // the solution: use a hack and still use strftime(): first
                // find the YEAR which is a year in the strftime() range (1970
                // - 2038) whose Jan 1 falls on the same week day as the Jan 1
                // of the real year. Then make a copy of the format and
                // replace all occurences of YEAR in it with some unique
                // string not appearing anywhere else in it, then use
                // strftime() to format the date in year YEAR and then replace
                // YEAR back by the real year and the unique replacement
                // string back with YEAR. Notice that "all occurences of YEAR"
                // means all occurences of 4 digit as well as 2 digit form!
                //
                // the bugs: we assume that neither of %c nor %x contains any
                // fields which may change between the YEAR and real year. For
                // example, the week number (%U, %W) and the day number (%j)
                // will change if one of these years is leap and the other one
                // is not!
                {
                    // find the YEAR: normally, for any year X, Jan 1 or the
                    // year X + 28 is the same weekday as Jan 1 of X (because
                    // the weekday advances by 1 for each normal X and by 2
                    // for each leap X, hence by 5 every 4 years or by 35
                    // which is 0 mod 7 every 28 years) but this rule breaks
                    // down if there are years between X and Y which are
                    // divisible by 4 but not leap (i.e. divisible by 100 but
                    // not 400), hence the correction.

                    int yearReal = GetYear(tz);
                    int mod28 = yearReal % 28;

                    // be careful to not go too far - we risk to leave the
                    // supported range
                    int year;
                    if ( mod28 < 10 )
                    {
                        year = 1988 + mod28;      // 1988 == 0 (mod 28)
                    }
                    else
                    {
                        year = 1970 + mod28 - 10; // 1970 == 10 (mod 28)
                    }

                    int nCentury = year / 100,
                        nCenturyReal = yearReal / 100;

                    // need to adjust for the years divisble by 400 which are
                    // not leap but are counted like leap ones if we just take
                    // the number of centuries in between for nLostWeekDays
                    int nLostWeekDays = (nCentury - nCenturyReal) -
                                        (nCentury / 4 - nCenturyReal / 4);

                    // we have to gain back the "lost" weekdays: note that the
                    // effect of this loop is to not do anything to
                    // nLostWeekDays (which we won't use any more), but to
                    // (indirectly) set the year correctly
                    while ( (nLostWeekDays % 7) != 0 )
                    {
                        nLostWeekDays += year++ % 4 ? 1 : 2;
                    }

                    // at any rate, we couldn't go further than 1988 + 9 + 28!
                    wxASSERT_MSG( year < 2030,
                                  _T("logic error in wxDateTime::Format") );

                    wxString strYear, strYear2;
                    strYear.Printf(_T("%d"), year);
                    strYear2.Printf(_T("%d"), year % 100);

                    // find two strings not occuring in format (this is surely
                    // not optimal way of doing it... improvements welcome!)
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

                    // use strftime() to format the same date but in supported
                    // year
                    //
                    // NB: we assume that strftime() doesn't check for the
                    //     date validity and will happily format the date
                    //     corresponding to Feb 29 of a non leap year (which
                    //     may happen if yearReal was leap and year is not)
                    struct tm tmAdjusted;
                    InitTm(tmAdjusted);
                    tmAdjusted.tm_hour = tm.hour;
                    tmAdjusted.tm_min = tm.min;
                    tmAdjusted.tm_sec = tm.sec;
                    tmAdjusted.tm_wday = tm.GetWeekDay();
                    tmAdjusted.tm_yday = GetDayOfYear();
                    tmAdjusted.tm_mday = tm.mday;
                    tmAdjusted.tm_mon = tm.mon;
                    tmAdjusted.tm_year = year - 1900;
                    tmAdjusted.tm_isdst = 0; // no DST, already adjusted
                    wxString str = CallStrftime(*p == _T('c') ? _T("%c")
                                                              : _T("%x"),
                                                &tmAdjusted);

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

                    res += str;
                }
                break;

            case _T('d'):       // day of a month (01-31)
                res += wxString::Format(fmt, tm.mday);
                break;

            case _T('H'):       // hour in 24h format (00-23)
                res += wxString::Format(fmt, tm.hour);
                break;

            case _T('I'):       // hour in 12h format (01-12)
                {
                    // 24h -> 12h, 0h -> 12h too
                    int hour12 = tm.hour > 12 ? tm.hour - 12
                                              : tm.hour ? tm.hour : 12;
                    res += wxString::Format(fmt, hour12);
                }
                break;

            case _T('j'):       // day of the year
                res += wxString::Format(fmt, GetDayOfYear(tz));
                break;

            case _T('m'):       // month as a number (01-12)
                res += wxString::Format(fmt, tm.mon + 1);
                break;

            case _T('M'):       // minute as a decimal number (00-59)
                res += wxString::Format(fmt, tm.min);
                break;

            case _T('p'):       // AM or PM string
                res += CallStrftime(_T("%p"), &tmTimeOnly);
                break;

            case _T('S'):       // second as a decimal number (00-61)
                res += wxString::Format(fmt, tm.sec);
                break;

            case _T('U'):       // week number in the year (Sunday 1st week day)
                {
                    int week = (GetDayOfYear(tz) - tm.GetWeekDay() + 7) / 7;
                    res += wxString::Format(fmt, week);
                }
                break;

            case _T('W'):       // week number in the year (Monday 1st week day)
                res += wxString::Format(fmt, GetWeekOfYear(tz));
                break;

            case _T('w'):       // weekday as a number (0-6), Sunday = 0
                res += wxString::Format(fmt, tm.GetWeekDay());
                break;

            // case _T('x'): -- handled with "%c"

            case _T('X'):       // locale default time representation
                // just use strftime() to format the time for us
                res += CallStrftime(_T("%X"), &tmTimeOnly);
                break;

            case _T('y'):       // year without century (00-99)
                res += wxString::Format(fmt, tm.year % 100);
                break;

            case _T('Y'):       // year with century
                res += wxString::Format(fmt, tm.year);
                break;

            case _T('Z'):       // timezone name
                res += CallStrftime(_T("%Z"), &tmTimeOnly);
                break;

            default:
                // is it the format width?
                fmt.Empty();
                while ( *p == _T('-') || *p == _T('+') ||
                        *p == _T(' ') || wxIsdigit(*p) )
                {
                    fmt += *p;
                }

                if ( !fmt.IsEmpty() )
                {
                    // we've only got the flags and width so far in fmt
                    fmt.Prepend(_T('%'));
                    fmt.Append(_T('d'));

                    goto restart;
                }

                // no, it wasn't the width
                wxFAIL_MSG(_T("unknown format specificator"));

                // fall through and just copy it nevertheless

            case _T('%'):       // a percent sign
                res += *p;
                break;

            case 0:             // the end of string
                wxFAIL_MSG(_T("missing format at the end of string"));

                // just put the '%' which was the last char in format
                res += _T('%');
                break;
        }
    }

    return res;
}

// this function parses a string in (strict) RFC 822 format: see the section 5
// of the RFC for the detailed description, but briefly it's something of the
// form "Sat, 18 Dec 1999 00:48:30 +0100"
//
// this function is "strict" by design - it must reject anything except true
// RFC822 time specs.
//
// TODO a great candidate for using reg exps
const wxChar *wxDateTime::ParseRfc822Date(const wxChar* date)
{
    wxCHECK_MSG( date, (wxChar *)NULL, _T("NULL pointer in wxDateTime::Parse") );

    const wxChar *p = date;
    const wxChar *comma = wxStrchr(p, _T(','));
    if ( comma )
    {
        // the part before comma is the weekday

        // skip it for now - we don't use but might check that it really
        // corresponds to the specfied date
        p = comma + 1;

        if ( *p != _T(' ') )
        {
            wxLogDebug(_T("no space after weekday in RFC822 time spec"));

            return (wxChar *)NULL;
        }

        p++; // skip space
    }

    // the following 1 or 2 digits are the day number
    if ( !wxIsdigit(*p) )
    {
        wxLogDebug(_T("day number expected in RFC822 time spec, none found"));

        return (wxChar *)NULL;
    }

    wxDateTime_t day = *p++ - _T('0');
    if ( wxIsdigit(*p) )
    {
        day *= 10;
        day += *p++ - _T('0');
    }

    if ( *p++ != _T(' ') )
    {
        return (wxChar *)NULL;
    }

    // the following 3 letters specify the month
    wxString monName(p, 3);
    Month mon;
    if ( monName == _T("Jan") )
        mon = Jan;
    else if ( monName == _T("Feb") )
        mon = Feb;
    else if ( monName == _T("Mar") )
        mon = Mar;
    else if ( monName == _T("Apr") )
        mon = Apr;
    else if ( monName == _T("May") )
        mon = May;
    else if ( monName == _T("Jun") )
        mon = Jun;
    else if ( monName == _T("Jul") )
        mon = Jul;
    else if ( monName == _T("Aug") )
        mon = Aug;
    else if ( monName == _T("Sep") )
        mon = Sep;
    else if ( monName == _T("Oct") )
        mon = Oct;
    else if ( monName == _T("Nov") )
        mon = Nov;
    else if ( monName == _T("Dec") )
        mon = Dec;
    else
    {
        wxLogDebug(_T("Invalid RFC 822 month name '%s'"), monName.c_str());

        return (wxChar *)NULL;
    }

    p += 3;

    if ( *p++ != _T(' ') )
    {
        return (wxChar *)NULL;
    }

    // next is the year
    if ( !wxIsdigit(*p) )
    {
        // no year?
        return (wxChar *)NULL;
    }

    int year = *p++ - _T('0');

    if ( !wxIsdigit(*p) )
    {
        // should have at least 2 digits in the year
        return (wxChar *)NULL;
    }

    year *= 10;
    year += *p++ - _T('0');

    // is it a 2 digit year (as per original RFC 822) or a 4 digit one?
    if ( wxIsdigit(*p) )
    {
        year *= 10;
        year += *p++ - _T('0');

        if ( !wxIsdigit(*p) )
        {
            // no 3 digit years please
            return (wxChar *)NULL;
        }

        year *= 10;
        year += *p++ - _T('0');
    }

    if ( *p++ != _T(' ') )
    {
        return (wxChar *)NULL;
    }

    // time is in the format hh:mm:ss and seconds are optional
    if ( !wxIsdigit(*p) )
    {
        return (wxChar *)NULL;
    }

    wxDateTime_t hour = *p++ - _T('0');

    if ( !wxIsdigit(*p) )
    {
        return (wxChar *)NULL;
    }

    hour *= 10;
    hour += *p++ - _T('0');

    if ( *p++ != _T(':') )
    {
        return (wxChar *)NULL;
    }

    if ( !wxIsdigit(*p) )
    {
        return (wxChar *)NULL;
    }

    wxDateTime_t min = *p++ - _T('0');

    if ( !wxIsdigit(*p) )
    {
        return (wxChar *)NULL;
    }

    min *= 10;
    min += *p++ - _T('0');

    wxDateTime_t sec = 0;
    if ( *p++ == _T(':') )
    {
        if ( !wxIsdigit(*p) )
        {
            return (wxChar *)NULL;
        }

        sec = *p++ - _T('0');

        if ( !wxIsdigit(*p) )
        {
            return (wxChar *)NULL;
        }

        sec *= 10;
        sec += *p++ - _T('0');
    }

    if ( *p++ != _T(' ') )
    {
        return (wxChar *)NULL;
    }

    // and now the interesting part: the timezone
    int offset;
    if ( *p == _T('-') || *p == _T('+') )
    {
        // the explicit offset given: it has the form of hhmm
        bool plus = *p++ == _T('+');

        if ( !wxIsdigit(*p) || !wxIsdigit(*(p + 1)) )
        {
            return (wxChar *)NULL;
        }

        // hours
        offset = 60*(10*(*p - _T('0')) + (*(p + 1) - _T('0')));

        p += 2;

        if ( !wxIsdigit(*p) || !wxIsdigit(*(p + 1)) )
        {
            return (wxChar *)NULL;
        }

        // minutes
        offset += 10*(*p - _T('0')) + (*(p + 1) - _T('0'));

        if ( !plus )
        {
            offset = -offset;
        }

        p += 2;
    }
    else
    {
        // the symbolic timezone given: may be either military timezone or one
        // of standard abbreviations
        if ( !*(p + 1) )
        {
            // military: Z = UTC, J unused, A = -1, ..., Y = +12
            static const int offsets[26] =
            {
                //A  B   C   D   E   F   G   H   I    J    K    L    M
                -1, -2, -3, -4, -5, -6, -7, -8, -9,   0, -10, -11, -12,
                //N  O   P   R   Q   S   T   U   V    W    Z    Y    Z
                +1, +2, +3, +4, +5, +6, +7, +8, +9, +10, +11, +12, 0
            };

            if ( *p < _T('A') || *p > _T('Z') || *p == _T('J') )
            {
                wxLogDebug(_T("Invalid militaty timezone '%c'"), *p);

                return (wxChar *)NULL;
            }

            offset = offsets[*p++ - _T('A')];
        }
        else
        {
            // abbreviation
            wxString tz = p;
            if ( tz == _T("UT") || tz == _T("UTC") || tz == _T("GMT") )
                offset = 0;
            else if ( tz == _T("AST") )
                offset = AST - GMT0;
            else if ( tz == _T("ADT") )
                offset = ADT - GMT0;
            else if ( tz == _T("EST") )
                offset = EST - GMT0;
            else if ( tz == _T("EDT") )
                offset = EDT - GMT0;
            else if ( tz == _T("CST") )
                offset = CST - GMT0;
            else if ( tz == _T("CDT") )
                offset = CDT - GMT0;
            else if ( tz == _T("MST") )
                offset = MST - GMT0;
            else if ( tz == _T("MDT") )
                offset = MDT - GMT0;
            else if ( tz == _T("PST") )
                offset = PST - GMT0;
            else if ( tz == _T("PDT") )
                offset = PDT - GMT0;
            else
            {
                wxLogDebug(_T("Unknown RFC 822 timezone '%s'"), p);

                return (wxChar *)NULL;
            }

            p += tz.length();
        }

        // make it minutes
        offset *= 60;
    }

    // the spec was correct
    Set(day, mon, year, hour, min, sec);
    MakeTimezone((wxDateTime_t)(60*offset));

    return p;
}

const wxChar *wxDateTime::ParseFormat(const wxChar *date,
                                      const wxChar *format,
                                      const wxDateTime& dateDef)
{
    wxCHECK_MSG( date && format, (wxChar *)NULL,
                 _T("NULL pointer in wxDateTime::ParseFormat()") );

    wxString str;
    unsigned long num;

    // what fields have we found?
    bool haveWDay = FALSE,
         haveYDay = FALSE,
         haveDay = FALSE,
         haveMon = FALSE,
         haveYear = FALSE,
         haveHour = FALSE,
         haveMin = FALSE,
         haveSec = FALSE;

    bool hourIsIn12hFormat = FALSE, // or in 24h one?
         isPM = FALSE;              // AM by default

    // and the value of the items we have (init them to get rid of warnings)
    wxDateTime_t sec = 0,
                 min = 0,
                 hour = 0;
    WeekDay wday = Inv_WeekDay;
    wxDateTime_t yday = 0,
                 mday = 0;
    wxDateTime::Month mon = Inv_Month;
    int year = 0;

    const wxChar *input = date;
    for ( const wxChar *fmt = format; *fmt; fmt++ )
    {
        if ( *fmt != _T('%') )
        {
            if ( wxIsspace(*fmt) )
            {
                // a white space in the format string matches 0 or more white
                // spaces in the input
                while ( wxIsspace(*input) )
                {
                    input++;
                }
            }
            else // !space
            {
                // any other character (not whitespace, not '%') must be
                // matched by itself in the input
                if ( *input++ != *fmt )
                {
                    // no match
                    return (wxChar *)NULL;
                }
            }

            // done with this format char
            continue;
        }

        // start of a format specification
        switch ( *++fmt )
        {
            case _T('a'):       // a weekday name
            case _T('A'):
                {
                    int flag = *fmt == _T('a') ? Name_Abbr : Name_Full;
                    wday = GetWeekDayFromName(GetAlphaToken(input), flag);
                    if ( wday == Inv_WeekDay )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }
                }
                haveWDay = TRUE;
                break;

            case _T('b'):       // a month name
            case _T('B'):
                {
                    int flag = *fmt == _T('b') ? Name_Abbr : Name_Full;
                    mon = GetMonthFromName(GetAlphaToken(input), flag);
                    if ( mon == Inv_Month )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }
                }
                haveMon = TRUE;
                break;

            case _T('c'):       // locale default date and time  representation
                {
                    wxDateTime dt;

                    // this is the format which corresponds to ctime() output
                    // and strptime("%c") should parse it, so try it first
                    static const wxChar *fmtCtime = _T("%a %b %d %H:%M:%S %Y");

                    const wxChar *result = dt.ParseFormat(input, fmtCtime);
                    if ( !result )
                    {
                        result = dt.ParseFormat(input, _T("%x %X"));
                    }

                    if ( !result )
                    {
                        result = dt.ParseFormat(input, _T("%X %x"));
                    }

                    if ( !result )
                    {
                        // we've tried everything and still no match
                        return (wxChar *)NULL;
                    }

                    Tm tm = dt.GetTm();

                    haveDay = haveMon = haveYear =
                    haveHour = haveMin = haveSec = TRUE;

                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;

                    year = tm.year;
                    mon = tm.mon;
                    mday = tm.mday;

                    input = result;
                }
                break;

            case _T('d'):       // day of a month (01-31)
                if ( !GetNumericToken(input, &num) || (num > 31) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                // we can't check whether the day range is correct yet, will
                // do it later - assume ok for now
                haveDay = TRUE;
                mday = (wxDateTime_t)num;
                break;

            case _T('H'):       // hour in 24h format (00-23)
                if ( !GetNumericToken(input, &num) || (num > 23) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveHour = TRUE;
                hour = (wxDateTime_t)num;
                break;

            case _T('I'):       // hour in 12h format (01-12)
                if ( !GetNumericToken(input, &num) || !num || (num > 12) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveHour = TRUE;
                hourIsIn12hFormat = TRUE;
                hour = num % 12;        // 12 should be 0
                break;

            case _T('j'):       // day of the year
                if ( !GetNumericToken(input, &num) || !num || (num > 366) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveYDay = TRUE;
                yday = (wxDateTime_t)num;
                break;

            case _T('m'):       // month as a number (01-12)
                if ( !GetNumericToken(input, &num) || !num || (num > 12) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveMon = TRUE;
                mon = (Month)(num - 1);
                break;

            case _T('M'):       // minute as a decimal number (00-59)
                if ( !GetNumericToken(input, &num) || (num > 59) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveMin = TRUE;
                min = (wxDateTime_t)num;
                break;

            case _T('p'):       // AM or PM string
                {
                    wxString am, pm, token = GetAlphaToken(input);

                    GetAmPmStrings(&am, &pm);
                    if ( token.CmpNoCase(pm) == 0 )
                    {
                        isPM = TRUE;
                    }
                    else if ( token.CmpNoCase(am) != 0 )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }
                }
                break;

            case _T('r'):       // time as %I:%M:%S %p
                {
                    wxDateTime dt;
                    input = dt.ParseFormat(input, _T("%I:%M:%S %p"));
                    if ( !input )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }

                    haveHour = haveMin = haveSec = TRUE;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;
                }
                break;

            case _T('R'):       // time as %H:%M
                {
                    wxDateTime dt;
                    input = dt.ParseFormat(input, _T("%H:%M"));
                    if ( !input )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }

                    haveHour = haveMin = TRUE;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                }

            case _T('S'):       // second as a decimal number (00-61)
                if ( !GetNumericToken(input, &num) || (num > 61) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveSec = TRUE;
                sec = (wxDateTime_t)num;
                break;

            case _T('T'):       // time as %H:%M:%S
                {
                    wxDateTime dt;
                    input = dt.ParseFormat(input, _T("%H:%M:%S"));
                    if ( !input )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }

                    haveHour = haveMin = haveSec = TRUE;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;
                }
                break;

            case _T('w'):       // weekday as a number (0-6), Sunday = 0
                if ( !GetNumericToken(input, &num) || (wday > 6) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveWDay = TRUE;
                wday = (WeekDay)num;
                break;

            case _T('x'):       // locale default date representation
#ifdef HAVE_STRPTIME
                // try using strptime() - it may fail even if the input is
                // correct but the date is out of range, so we will fall back
                // to our generic code anyhow (FIXME !Unicode friendly)
                {
                    struct tm tm;
                    const wxChar *result = strptime(input, "%x", &tm);
                    if ( result )
                    {
                        input = result;

                        haveDay = haveMon = haveYear = TRUE;

                        year = 1900 + tm.tm_year;
                        mon = (Month)tm.tm_mon;
                        mday = tm.tm_mday;

                        break;
                    }
                }
#endif // HAVE_STRPTIME

                // TODO query the LOCALE_IDATE setting under Win32
                {
                    wxDateTime dt;

                    wxString fmtDate, fmtDateAlt;
                    if ( IsWestEuropeanCountry(GetCountry()) ||
                         GetCountry() == Russia )
                    {
                        fmtDate = _T("%d/%m/%y");
                        fmtDateAlt = _T("%m/%d/%y");
                    }
                    else // assume USA
                    {
                        fmtDate = _T("%m/%d/%y");
                        fmtDateAlt = _T("%d/%m/%y");
                    }

                    const wxChar *result = dt.ParseFormat(input, fmtDate);

                    if ( !result )
                    {
                        // ok, be nice and try another one
                        result = dt.ParseFormat(input, fmtDateAlt);
                    }

                    if ( !result )
                    {
                        // bad luck
                        return (wxChar *)NULL;
                    }

                    Tm tm = dt.GetTm();

                    haveDay = haveMon = haveYear = TRUE;

                    year = tm.year;
                    mon = tm.mon;
                    mday = tm.mday;

                    input = result;
                }

                break;

            case _T('X'):       // locale default time representation
#ifdef HAVE_STRPTIME
                {
                    // use strptime() to do it for us (FIXME !Unicode friendly)
                    struct tm tm;
                    input = strptime(input, "%X", &tm);
                    if ( !input )
                    {
                        return (wxChar *)NULL;
                    }

                    haveHour = haveMin = haveSec = TRUE;

                    hour = tm.tm_hour;
                    min = tm.tm_min;
                    sec = tm.tm_sec;
                }
#else // !HAVE_STRPTIME
                // TODO under Win32 we can query the LOCALE_ITIME system
                //      setting which says whether the default time format is
                //      24 or 12 hour
                {
                    // try to parse what follows as "%H:%M:%S" and, if this
                    // fails, as "%I:%M:%S %p" - this should catch the most
                    // common cases
                    wxDateTime dt;

                    const wxChar *result = dt.ParseFormat(input, _T("%T"));
                    if ( !result )
                    {
                        result = dt.ParseFormat(input, _T("%r"));
                    }

                    if ( !result )
                    {
                        // no match
                        return (wxChar *)NULL;
                    }

                    haveHour = haveMin = haveSec = TRUE;

                    Tm tm = dt.GetTm();
                    hour = tm.hour;
                    min = tm.min;
                    sec = tm.sec;

                    input = result;
                }
#endif // HAVE_STRPTIME/!HAVE_STRPTIME
                break;

            case _T('y'):       // year without century (00-99)
                if ( !GetNumericToken(input, &num) || (num > 99) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveYear = TRUE;
                year = 1900 + num;
                break;

            case _T('Y'):       // year with century
                if ( !GetNumericToken(input, &num) )
                {
                    // no match
                    return (wxChar *)NULL;
                }

                haveYear = TRUE;
                year = (wxDateTime_t)num;
                break;

            case _T('Z'):       // timezone name
                wxFAIL_MSG(_T("TODO"));
                break;

            case _T('%'):       // a percent sign
                if ( *input++ != _T('%') )
                {
                    // no match
                    return (wxChar *)NULL;
                }
                break;

            case 0:             // the end of string
                wxFAIL_MSG(_T("unexpected format end"));

                // fall through

            default:            // not a known format spec
                return (wxChar *)NULL;
        }
    }

    // format matched, try to construct a date from what we have now
    Tm tmDef;
    if ( dateDef.IsValid() )
    {
        // take this date as default
        tmDef = dateDef.GetTm();
    }
    else if ( m_time != 0 )
    {
        // if this date is valid, don't change it
        tmDef = GetTm();
    }
    else
    {
        // no default and this date is invalid - fall back to Today()
        tmDef = Today().GetTm();
    }

    Tm tm = tmDef;

    // set the date
    if ( haveYear )
    {
        tm.year = year;
    }

    // TODO we don't check here that the values are consistent, if both year
    //      day and month/day were found, we just ignore the year day and we
    //      also always ignore the week day
    if ( haveMon && haveDay )
    {
        if ( mday > GetNumOfDaysInMonth(tm.year, mon) )
        {
            wxLogDebug(_T("bad month day in wxDateTime::ParseFormat"));

            return (wxChar *)NULL;
        }

        tm.mon = mon;
        tm.mday = mday;
    }
    else if ( haveYDay )
    {
        if ( yday > GetNumberOfDays(tm.year) )
        {
            wxLogDebug(_T("bad year day in wxDateTime::ParseFormat"));

            return (wxChar *)NULL;
        }

        Tm tm2 = wxDateTime(1, Jan, tm.year).SetToYearDay(yday).GetTm();

        tm.mon = tm2.mon;
        tm.mday = tm2.mday;
    }

    // deal with AM/PM
    if ( haveHour && hourIsIn12hFormat && isPM )
    {
        // translate to 24hour format
        hour += 12;
    }
    //else: either already in 24h format or no translation needed

    // set the time
    if ( haveHour )
    {
        tm.hour = hour;
    }

    if ( haveMin )
    {
        tm.min = min;
    }

    if ( haveSec )
    {
        tm.sec = sec;
    }

    Set(tm);

    return input;
}

const wxChar *wxDateTime::ParseDateTime(const wxChar *date)
{
    wxCHECK_MSG( date, (wxChar *)NULL, _T("NULL pointer in wxDateTime::Parse") );

    // there is a public domain version of getdate.y, but it only works for
    // English...
    wxFAIL_MSG(_T("TODO"));

    return (wxChar *)NULL;
}

const wxChar *wxDateTime::ParseDate(const wxChar *date)
{
    // this is a simplified version of ParseDateTime() which understands only
    // "today" (for wxDate compatibility) and digits only otherwise (and not
    // all esoteric constructions ParseDateTime() knows about)

    wxCHECK_MSG( date, (wxChar *)NULL, _T("NULL pointer in wxDateTime::Parse") );

    const wxChar *p = date;
    while ( wxIsspace(*p) )
        p++;

    wxString today = _T("today");
    size_t len = today.length();
    if ( wxString(p, len).CmpNoCase(today) == 0 )
    {
        // nothing can follow this, so stop here
        p += len;

        *this = Today();

        return p;
    }

    // what do we have?
    bool haveDay = FALSE,       // the months day?
         haveWDay = FALSE,      // the day of week?
         haveMon = FALSE,       // the month?
         haveYear = FALSE;      // the year?

    // and the value of the items we have (init them to get rid of warnings)
    WeekDay wday = Inv_WeekDay;
    wxDateTime_t day = 0;
    wxDateTime::Month mon = Inv_Month;
    int year = 0;

    // tokenize the string
    wxStringTokenizer tok(p, _T(",/-\t "));
    while ( tok.HasMoreTokens() )
    {
        wxString token = tok.GetNextToken();

        // is it a number?
        unsigned long val;
        if ( token.ToULong(&val) )
        {
            // guess what this number is

            bool isDay = FALSE,
                 isMonth = FALSE,
                 // only years are counted from 0
                 isYear = (val == 0) || (val > 31);
            if ( !isYear )
            {
                // may be the month or month day or the year, assume the
                // month day by default and fallback to month if the range
                // allow it or to the year if our assumption doesn't work
                if ( haveDay )
                {
                    // we already have the day, so may only be a month or year
                    if ( val > 12 )
                    {
                        isYear = TRUE;
                    }
                    else
                    {
                        isMonth = TRUE;
                    }
                }
                else // it may be day
                {
                    isDay = TRUE;

                    // check the range
                    if ( haveMon )
                    {
                        if ( val > GetNumOfDaysInMonth(haveYear ? year
                                                                : Inv_Year,
                                                       mon) )
                        {
                            // oops, it can't be a day finally
                            isDay = FALSE;

                            if ( val > 12 )
                            {
                                isYear = TRUE;
                            }
                            else
                            {
                                isMonth = TRUE;
                            }
                        }
                    }
                }
            }

            // remember that we have this and stop the scan if it's the second
            // time we find this, except for the day logic (see there)
            if ( isYear )
            {
                if ( haveYear )
                {
                    break;
                }

                haveYear = TRUE;

                // no roll over - 99 means 99, not 1999 for us
                year = val;
            }
            else if ( isMonth )
            {
                if ( haveMon )
                {
                    break;
                }

                haveMon = TRUE;

                mon = (wxDateTime::Month)val;
            }
            else
            {
                wxASSERT_MSG( isDay, _T("logic error") );

                if ( haveDay )
                {
                    // may be were mistaken when we found it for the first
                    // time? may be it was a month or year instead?
                    //
                    // this ability to "backtrack" allows us to correctly parse
                    // both things like 01/13 and 13/01 - but, of course, we
                    // still can't resolve the ambiguity in 01/02 (it will be
                    // Feb 1 for us, not Jan 2 as americans might expect!)
                    if ( (day <= 12) && !haveMon )
                    {
                        // exchange day and month
                        mon = (wxDateTime::Month)day;

                        haveMon = TRUE;
                    }
                    else if ( !haveYear )
                    {
                        // exchange day and year
                        year = day;

                        haveYear = TRUE;
                    }
                }

                haveDay = TRUE;

                day = val;
            }
        }
        else // not a number
        {
            mon = GetMonthFromName(token, Name_Full | Name_Abbr);
            if ( mon != Inv_Month )
            {
                // it's a month
                if ( haveMon )
                {
                    break;
                }

                haveMon = TRUE;
            }
            else
            {
                wday = GetWeekDayFromName(token, Name_Full | Name_Abbr);
                if ( wday != Inv_WeekDay )
                {
                    // a week day
                    if ( haveWDay )
                    {
                        break;
                    }

                    haveWDay = TRUE;
                }
                else
                {
                    // try the ordinals
                    static const wxChar *ordinals[] =
                    {
                        wxTRANSLATE("first"),
                        wxTRANSLATE("second"),
                        wxTRANSLATE("third"),
                        wxTRANSLATE("fourth"),
                        wxTRANSLATE("fifth"),
                        wxTRANSLATE("sixth"),
                        wxTRANSLATE("seventh"),
                        wxTRANSLATE("eighth"),
                        wxTRANSLATE("ninth"),
                        wxTRANSLATE("tenth"),
                        wxTRANSLATE("eleventh"),
                        wxTRANSLATE("twelfth"),
                        wxTRANSLATE("thirteenth"),
                        wxTRANSLATE("fourteenth"),
                        wxTRANSLATE("fifteenth"),
                        wxTRANSLATE("sixteenth"),
                        wxTRANSLATE("seventeenth"),
                        wxTRANSLATE("eighteenth"),
                        wxTRANSLATE("nineteenth"),
                        wxTRANSLATE("twentieth"),
                        // that's enough - otherwise we'd have problems with
                        // composite (or not) ordinals otherwise
                    };

                    size_t n;
                    for ( n = 0; n < WXSIZEOF(ordinals); n++ )
                    {
                        if ( token.CmpNoCase(ordinals[n]) == 0 )
                        {
                            break;
                        }
                    }

                    if ( n == WXSIZEOF(ordinals) )
                    {
                        // stop here - something unknown
                        break;
                    }

                    // it's a day
                    if ( haveDay )
                    {
                        // don't try anything here (as in case of numeric day
                        // above) - the symbolic day spec should always
                        // precede the month/year
                        break;
                    }

                    haveDay = TRUE;

                    day = n + 1;
                }
            }
        }
    }

    // either no more tokens or the scan was stopped by something we couldn't
    // parse - in any case, see if we can construct a date from what we have
    if ( !haveDay && !haveWDay )
    {
        wxLogDebug(_T("no day, no weekday hence no date."));

        return (wxChar *)NULL;
    }

    if ( haveWDay && (haveMon || haveYear || haveDay) &&
         !(haveMon && haveMon && haveYear) )
    {
        // without adjectives (which we don't support here) the week day only
        // makes sense completely separately or with the full date
        // specification (what would "Wed 1999" mean?)
        return (wxChar *)NULL;
    }

    if ( !haveMon )
    {
        mon = GetCurrentMonth();
    }

    if ( !haveYear )
    {
        year = GetCurrentYear();
    }

    if ( haveDay )
    {
        Set(day, mon, year);

        if ( haveWDay )
        {
            // check that it is really the same
            if ( GetWeekDay() != wday )
            {
                // inconsistency detected
                return (wxChar *)NULL;
            }
        }
    }
    else // haveWDay
    {
        *this = Today();

        SetToWeekDayInSameWeek(wday);
    }

    // return the pointer to the next char
    return p + wxStrlen(p) - wxStrlen(tok.GetString());
}

const wxChar *wxDateTime::ParseTime(const wxChar *time)
{
    wxCHECK_MSG( time, (wxChar *)NULL, _T("NULL pointer in wxDateTime::Parse") );

    // first try some extra things
    static const struct
    {
        const wxChar *name;
        wxDateTime_t  hour;
    } stdTimes[] =
    {
        { wxTRANSLATE("noon"),      12 },
        { wxTRANSLATE("midnight"),  00 },
        // anything else?
    };

    for ( size_t n = 0; n < WXSIZEOF(stdTimes); n++ )
    {
        wxString timeString = wxGetTranslation(stdTimes[n].name);
        size_t len = timeString.length();
        if ( timeString.CmpNoCase(wxString(time, len)) == 0 )
        {
            Set(stdTimes[n].hour, 0, 0);

            return time + len;
        }
    }

    // try all time formats we may think about starting with the standard one
    const wxChar *result = ParseFormat(time, _T("%X"));
    if ( !result )
    {
        // normally, it's the same, but why not try it?
        result = ParseFormat(time, _T("%H:%M:%S"));
    }

    if ( !result )
    {
        // 12hour with AM/PM?
        result = ParseFormat(time, _T("%I:%M:%S %p"));
    }

    if ( !result )
    {
        // without seconds?
        result = ParseFormat(time, _T("%H:%M"));
    }

    if ( !result )
    {
        // 12hour with AM/PM but without seconds?
        result = ParseFormat(time, _T("%I:%M %p"));
    }

    if ( !result )
    {
        // just the hour?
        result = ParseFormat(time, _T("%H"));
    }

    if ( !result )
    {
        // just the hour and AM/PM?
        result = ParseFormat(time, _T("%I %p"));
    }

    // TODO: parse timezones

    return result;
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
                    tmp.Printf(_T("%03ld"), GetMilliseconds().ToLong());
                    break;

                case 'M':
                    tmp.Printf(_T("%02d"), GetMinutes());
                    break;

                case 'S':
                    tmp.Printf(_T("%02ld"), GetSeconds().ToLong());
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
