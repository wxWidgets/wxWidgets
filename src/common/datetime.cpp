/////////////////////////////////////////////////////////////////////////////
// Name:        wx/datetime.h
// Purpose:     implementation of time/date related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.05.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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

#define wxDEFINE_TIME_CONSTANTS

#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the number of days in month in Julian/Gregorian calendar: the first line is
// for normal years, the second one is for the leap ones
static wxDateTime::wxDateTime_t gs_daysInMonth[2][12] =
{
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

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
    hour = min = sec = 0;
    wday = wxDateTime::Inv_WeekDay;
}

wxDateTime::Tm::Tm(const struct tm& tm)
{
    sec = tm.tm_sec;
    min = tm.tm_min;
    hour = tm.tm_hour;
    mday = tm.tm_mday;
    mon = tm.tm_mon;
    year = 1900 + tm.tm_year;
    wday = tm.tm_wday;
    yday = tm.tm_yday;
}

bool wxDateTime::Tm::IsValid() const
{
    // we allow for the leap seconds, although we don't use them (yet)
    return (year != wxDateTime::Inv_Year) && (mon < 12) &&
           (mday < gs_daysInMonth[IsLeapYear(year)][mon]) &&
           (hour < 24) && (min < 60) && (sec < 62);
}

void wxDateTime::Tm::ComputeWeekDay()
{
    wxFAIL_MSG(_T("TODO"));
}

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------

/* static */
bool wxDateTime::IsLeapYear(int year, wxDateTime::Calendar cal)
{
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
wxDateTime::wxDateTime_t wxDateTime::GetNumberOfDays(wxDateTime::Month month,
                                                     int year,
                                                     wxDateTime::Calendar cal)
{
    wxCHECK_MSG( month < 12, 0, _T("invalid month") );

    if ( cal == Gregorian || cal == Julian )
    {
        if ( year == Inv_Year )
        {
            // take the current year if none given
            year = GetCurrentYear();
        }

        return gs_daysInMonth[IsLeapYear(year)][month];
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

    // and offset it by the number of days needed to get 
    tm.tm_mday += wday;

    return CallStrftime(abbr ? _T("%a") : _T("%A"), &tm);
}

// ----------------------------------------------------------------------------
// constructors and assignment operators
// ----------------------------------------------------------------------------

wxDateTime& wxDateTime::Set(const struct tm& tm1)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    tm tm2(tm1);
    time_t timet = mktime(&tm2);
    if ( timet == (time_t)(-1) )
    {
        wxFAIL_MSG(_T("Invalid time"));

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

    if ( year == Inv_Year )
        year = GetCurrentYear();
    if ( month == Inv_Month )
        month = GetCurrentMonth();

    wxCHECK_MSG( day < GetNumberOfDays(month, year), ms_InvDateTime,
                 _T("Invalid date in wxDateTime::Set()") );

    // the range of time_t type (inclusive)
    static const int yearMinInRange = 1970;
    static const int yearMaxInRange = 2037;

    // test only the year instead of testing for the exact end of the Unix
    // time_t range - it doesn't bring anything to do more precise checks
    if ( year >= yearMaxInRange && year <= yearMaxInRange )
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

        (void)Set(tm);

        // and finally adjust milliseconds
        return SetMillisecond(millisec);
    }
    else
    {
        // do time calculations ourselves: we want to calculate the number of
        // milliseconds between the given date and the epoch (necessarily
        // negative)
        wxFAIL_MSG(_T("TODO"));
    }

    return *this;
}

// ----------------------------------------------------------------------------
// time_t <-> broken down time conversions
// ----------------------------------------------------------------------------

wxDateTime::Tm wxDateTime::GetTm() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime") );

    time_t time = GetTicks();
    if ( time != (time_t)-1 )
    {
        // use C RTL functions
        tm *tm = localtime(&time);

        // should never happen
        wxCHECK_MSG( tm, Tm(), _T("localtime() failed") );

        return Tm(*tm);
    }
    else
    {
        wxFAIL_MSG(_T("TODO"));

        return Tm();
    }
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
    tm.mon += diff.GetMonths();
    tm.mday += diff.GetTotalDays();

    Set(tm);

    return *this;
}

// ----------------------------------------------------------------------------
// wxDateTime to/from text representations
// ----------------------------------------------------------------------------

wxString wxDateTime::Format(const wxChar *format) const
{
    time_t time = GetTicks();
    if ( time != (time_t)-1 )
    {
        // use strftime()
        tm *tm = localtime(&time);

        // should never happen
        wxCHECK_MSG( tm, _T(""), _T("localtime() failed") );

        return CallStrftime(format, tm);
    }
    else
    {
        wxFAIL_MSG(_T("TODO"));

        return _T("");
    }
}
