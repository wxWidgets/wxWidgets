/////////////////////////////////////////////////////////////////////////////
// Name:        wx/datetime.inl
// Purpose:     definition of inline functions of wxDateTime and related
//              classes declared in datetime.h
// Author:      Vadim Zeitlin
// Remarks:     having the inline functions here allows us to minimize the
//              dependencies (and hence the rebuild time) in debug builds.
// Modified by:
// Created:     30.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_FROM_WX_DATETIME_H
    #error "This file is only included by wx/datetime.h, don't include it manually!"
#endif

// ----------------------------------------------------------------------------
// private macros
// ----------------------------------------------------------------------------

#define MILLISECONDS_PER_DAY 86400000l

// some broken compilers (HP-UX CC) refuse to compile the "normal" version, but
// using a temp variable always might prevent other compilers from optimising
// it away - hence use of this ugly macro
#ifndef __HPUX__
    #define MODIFY_AND_RETURN(op) return wxDateTime(*this).op
#else
    #define MODIFY_AND_RETURN(op) wxDateTime dt(*this); dt.op; return dt
#endif

// ----------------------------------------------------------------------------
// wxDateTime construction
// ----------------------------------------------------------------------------

// only define this once, when included from datetime.cpp
#ifdef wxDEFINE_TIME_CONSTANTS
    const long wxDateTime::TIME_T_FACTOR = 1000l;
#endif // wxDEFINE_TIME_CONSTANTS

WXDLLEXPORT bool wxDateTime::IsInStdRange() const
{
    return m_time >= 0l && (m_time / TIME_T_FACTOR) < LONG_MAX;
}

/* static */
WXDLLEXPORT wxDateTime wxDateTime::Now()
{
    return wxDateTime(*GetTmNow());
}

/* static */
WXDLLEXPORT wxDateTime wxDateTime::Today()
{
    struct tm *tm = GetTmNow();
    tm->tm_hour =
    tm->tm_min =
    tm->tm_sec = 0;

    return wxDateTime(*tm);
}

#if (!(defined(__VISAGECPP__) && __IBMCPP__ >= 400))
WXDLLEXPORT wxDateTime& wxDateTime::Set(time_t timet)
{
    // assign first to avoid long multiplication overflow!
    m_time = timet;
    m_time *= TIME_T_FACTOR;

    return *this;
}
#endif

WXDLLEXPORT wxDateTime& wxDateTime::SetToCurrent()
{
    *this = Now();
    return *this;
}

#if (!(defined(__VISAGECPP__) && __IBMCPP__ >= 400))
WXDLLEXPORT wxDateTime::wxDateTime(time_t timet)
{
    Set(timet);
}
#endif

WXDLLEXPORT wxDateTime::wxDateTime(const struct tm& tm)
{
    Set(tm);
}

WXDLLEXPORT wxDateTime::wxDateTime(const Tm& tm)
{
    Set(tm);
}

WXDLLEXPORT wxDateTime::wxDateTime(double jdn)
{
    Set(jdn);
}

WXDLLEXPORT wxDateTime& wxDateTime::Set(const Tm& tm)
{
    wxASSERT_MSG( tm.IsValid(), _T("invalid broken down date/time") );

    return Set(tm.mday, (Month)tm.mon, tm.year, tm.hour, tm.min, tm.sec);
}

WXDLLEXPORT wxDateTime::wxDateTime(wxDateTime_t hour,
                       wxDateTime_t minute,
                       wxDateTime_t second,
                       wxDateTime_t millisec)
{
    Set(hour, minute, second, millisec);
}

WXDLLEXPORT wxDateTime::wxDateTime(wxDateTime_t day,
                       Month        month,
                       int          year,
                       wxDateTime_t hour,
                       wxDateTime_t minute,
                       wxDateTime_t second,
                       wxDateTime_t millisec)
{
    Set(day, month, year, hour, minute, second, millisec);
}

// ----------------------------------------------------------------------------
// wxDateTime accessors
// ----------------------------------------------------------------------------

WXDLLEXPORT wxLongLong wxDateTime::GetValue() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return m_time;
}

WXDLLEXPORT time_t wxDateTime::GetTicks() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));
    if ( !IsInStdRange() )
    {
        return (time_t)-1;
    }

    return (time_t)((m_time / (long)TIME_T_FACTOR).GetLo());
}

WXDLLEXPORT bool wxDateTime::SetToLastWeekDay(WeekDay weekday,
                                  Month month,
                                  int year)
{
    return SetToWeekDay(weekday, -1, month, year);
}

WXDLLEXPORT wxDateTime wxDateTime::GetWeekDayInSameWeek(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToWeekDayInSameWeek(weekday) );
}

WXDLLEXPORT wxDateTime wxDateTime::GetNextWeekDay(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToNextWeekDay(weekday) );
}

WXDLLEXPORT wxDateTime wxDateTime::GetPrevWeekDay(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToPrevWeekDay(weekday) );
}

WXDLLEXPORT wxDateTime wxDateTime::GetWeekDay(WeekDay weekday,
                                  int n,
                                  Month month,
                                  int year) const
{
    wxDateTime dt(*this);

    return dt.SetToWeekDay(weekday, n, month, year) ? dt : wxInvalidDateTime;
}

WXDLLEXPORT wxDateTime wxDateTime::GetLastWeekDay(WeekDay weekday,
                                      Month month,
                                      int year)
{
    wxDateTime dt(*this);

    return dt.SetToLastWeekDay(weekday, month, year) ? dt : wxInvalidDateTime;
}

WXDLLEXPORT wxDateTime wxDateTime::GetWeek(wxDateTime_t numWeek, WeekDay weekday) const
{
    wxDateTime dt(*this);

    return dt.SetToTheWeek(numWeek, weekday) ? dt : wxInvalidDateTime;
}

WXDLLEXPORT wxDateTime wxDateTime::GetLastMonthDay(Month month, int year) const
{
    MODIFY_AND_RETURN( SetToLastMonthDay(month, year) );
}

WXDLLEXPORT wxDateTime wxDateTime::GetYearDay(wxDateTime_t yday) const
{
    MODIFY_AND_RETURN( SetToYearDay(yday) );
}

// ----------------------------------------------------------------------------
// wxDateTime comparison
// ----------------------------------------------------------------------------

WXDLLEXPORT bool wxDateTime::IsEqualTo(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time == datetime.m_time;
}

WXDLLEXPORT bool wxDateTime::IsEarlierThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time < datetime.m_time;
}

WXDLLEXPORT bool wxDateTime::IsLaterThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time > datetime.m_time;
}

WXDLLEXPORT bool wxDateTime::IsStrictlyBetween(const wxDateTime& t1,
                                   const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsLaterThan(t1) && IsEarlierThan(t2);
}

WXDLLEXPORT bool wxDateTime::IsBetween(const wxDateTime& t1, const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsEqualTo(t1) || IsEqualTo(t2) || IsStrictlyBetween(t1, t2);
}

WXDLLEXPORT bool wxDateTime::IsSameDate(const wxDateTime& dt) const
{
    return (m_time - dt.m_time).Abs() < MILLISECONDS_PER_DAY;
}

WXDLLEXPORT bool wxDateTime::IsSameTime(const wxDateTime& dt) const
{
    // notice that we can't do something like this:
    //
    //    m_time % MILLISECONDS_PER_DAY == dt.m_time % MILLISECONDS_PER_DAY
    //
    // because we have also to deal with (possibly) different DST settings!
    Tm tm1 = GetTm(),
       tm2 = dt.GetTm();

    return tm1.hour == tm2.hour &&
           tm1.min == tm2.min &&
           tm1.sec == tm2.sec &&
           tm1.msec == tm2.msec;
}

WXDLLEXPORT bool wxDateTime::IsEqualUpTo(const wxDateTime& dt, const wxTimeSpan& ts) const
{
    return IsBetween(dt.Substract(ts), dt.Add(ts));
}

// ----------------------------------------------------------------------------
// wxDateTime arithmetics
// ----------------------------------------------------------------------------

WXDLLEXPORT wxDateTime wxDateTime::Add(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time + diff.GetValue());
}

WXDLLEXPORT wxDateTime& wxDateTime::Add(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time += diff.GetValue();

    return *this;
}

WXDLLEXPORT wxDateTime& wxDateTime::operator+=(const wxTimeSpan& diff)
{
    return Add(diff);
}

WXDLLEXPORT wxDateTime wxDateTime::Substract(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time - diff.GetValue());
}

WXDLLEXPORT wxDateTime& wxDateTime::Substract(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time -= diff.GetValue();

    return *this;
}

WXDLLEXPORT wxDateTime& wxDateTime::operator-=(const wxTimeSpan& diff)
{
    return Substract(diff);
}

WXDLLEXPORT wxTimeSpan wxDateTime::Substract(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return wxTimeSpan(datetime.GetValue() - GetValue());
}

WXDLLEXPORT wxDateTime wxDateTime::Add(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Add(diff);
}

WXDLLEXPORT wxDateTime& wxDateTime::Substract(const wxDateSpan& diff)
{
    return Add(diff.Negate());
}

WXDLLEXPORT wxDateTime wxDateTime::Substract(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Substract(diff);
}

WXDLLEXPORT wxDateTime& wxDateTime::operator-=(const wxDateSpan& diff)
{
    return Substract(diff);
}

WXDLLEXPORT wxDateTime& wxDateTime::operator+=(const wxDateSpan& diff)
{
    return Add(diff);
}

// ----------------------------------------------------------------------------
// wxDateTime and timezones
// ----------------------------------------------------------------------------

WXDLLEXPORT wxDateTime wxDateTime::ToTimezone(const wxDateTime::TimeZone& tz,
                                  bool noDST) const
{
    MODIFY_AND_RETURN( MakeTimezone(tz, noDST) );
}

// ----------------------------------------------------------------------------
// wxTimeSpan construction
// ----------------------------------------------------------------------------

WXDLLEXPORT wxTimeSpan::wxTimeSpan(long hours, long minutes, long seconds, long milliseconds)
{
    // assign first to avoid precision loss
    m_diff = hours;
    m_diff *= 60l;
    m_diff += minutes;
    m_diff *= 60l;
    m_diff += seconds;
    m_diff *= 1000l;
    m_diff += milliseconds;
}

// ----------------------------------------------------------------------------
// wxTimeSpan accessors
// ----------------------------------------------------------------------------

WXDLLEXPORT wxLongLong wxTimeSpan::GetSeconds() const
{
    return m_diff / 1000l;
}

WXDLLEXPORT int wxTimeSpan::GetMinutes() const
{
    return (GetSeconds() / 60l).GetLo();
}

WXDLLEXPORT int wxTimeSpan::GetHours() const
{
    return GetMinutes() / 60;
}

WXDLLEXPORT int wxTimeSpan::GetDays() const
{
    return GetHours() / 24;
}

WXDLLEXPORT int wxTimeSpan::GetWeeks() const
{
    return GetDays() / 7;
}

// ----------------------------------------------------------------------------
// wxTimeSpan arithmetics
// ----------------------------------------------------------------------------

WXDLLEXPORT wxTimeSpan wxTimeSpan::Add(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff + diff.GetValue());
}

WXDLLEXPORT wxTimeSpan& wxTimeSpan::Add(const wxTimeSpan& diff)
{
    m_diff += diff.GetValue();

    return *this;
}

WXDLLEXPORT wxTimeSpan wxTimeSpan::Substract(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff - diff.GetValue());
}

WXDLLEXPORT wxTimeSpan& wxTimeSpan::Substract(const wxTimeSpan& diff)
{
    m_diff -= diff.GetValue();

    return *this;
}

WXDLLEXPORT wxTimeSpan& wxTimeSpan::Multiply(int n)
{
    m_diff *= (long)n;

    return *this;
}

WXDLLEXPORT wxTimeSpan wxTimeSpan::Multiply(int n) const
{
    return wxTimeSpan(m_diff * (long)n);
}

WXDLLEXPORT wxTimeSpan wxTimeSpan::Abs() const
{
    return wxTimeSpan(GetValue().Abs());
}

WXDLLEXPORT bool wxTimeSpan::IsEqualTo(const wxTimeSpan& ts) const
{
    return GetValue() == ts.GetValue();
}

WXDLLEXPORT bool wxTimeSpan::IsLongerThan(const wxTimeSpan& ts) const
{
    return GetValue().Abs() > ts.GetValue().Abs();
}

// ----------------------------------------------------------------------------
// wxDateSpan
// ----------------------------------------------------------------------------

WXDLLEXPORT wxDateSpan& 
wxDateSpan::operator+=(const wxDateSpan& other)
{
    m_years += other.m_years;
    m_months += other.m_months;
    m_weeks += other.m_weeks;
    m_days += other.m_days;

    return *this;
}

WXDLLEXPORT wxDateSpan& wxDateSpan::Multiply(int factor)
{
    m_years *= factor;
    m_months *= factor;
    m_weeks *= factor;
    m_days *= factor;

    return *this;
}

WXDLLEXPORT wxDateSpan wxDateSpan::Multiply(int factor) const
{
    return wxDateSpan(*this).Multiply(factor);
}

WXDLLEXPORT wxDateSpan wxDateSpan::Negate() const
{
    return wxDateSpan(-m_years, -m_months, -m_weeks, -m_days);
}

WXDLLEXPORT wxDateSpan& wxDateSpan::Neg()
{
    m_years = -m_years;
    m_months = -m_months;
    m_weeks = -m_weeks;
    m_days = -m_days;

    return *this;
}

#undef MILLISECONDS_PER_DAY

#undef MODIFY_AND_RETURN
