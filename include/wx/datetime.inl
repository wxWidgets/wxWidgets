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

inline bool wxDateTime::IsInStdRange() const
{
    return m_time >= 0l && (m_time / TIME_T_FACTOR) < LONG_MAX;
}

/* static */
inline wxDateTime wxDateTime::Now()
{
    return wxDateTime(*GetTmNow());
}

/* static */
inline wxDateTime wxDateTime::Today()
{
    struct tm *tm = GetTmNow();
    tm->tm_hour =
    tm->tm_min =
    tm->tm_sec = 0;

    return wxDateTime(*tm);
}

#if (!(defined(__VISAGECPP__) && __IBMCPP__ >= 400))
inline wxDateTime& wxDateTime::Set(time_t timet)
{
    // assign first to avoid long multiplication overflow!
    m_time = timet;
    m_time *= TIME_T_FACTOR;

    return *this;
}
#endif

inline wxDateTime& wxDateTime::SetToCurrent()
{
    *this = Now();
    return *this;
}

#if (!(defined(__VISAGECPP__) && __IBMCPP__ >= 400))
inline wxDateTime::wxDateTime(time_t timet)
{
    Set(timet);
}
#endif

inline wxDateTime::wxDateTime(const struct tm& tm)
{
    Set(tm);
}

inline wxDateTime::wxDateTime(const Tm& tm)
{
    Set(tm);
}

inline wxDateTime::wxDateTime(double jdn)
{
    Set(jdn);
}

inline wxDateTime& wxDateTime::Set(const Tm& tm)
{
    wxASSERT_MSG( tm.IsValid(), _T("invalid broken down date/time") );

    return Set(tm.mday, (Month)tm.mon, tm.year, tm.hour, tm.min, tm.sec);
}

inline wxDateTime::wxDateTime(wxDateTime_t hour,
                              wxDateTime_t minute,
                              wxDateTime_t second,
                              wxDateTime_t millisec)
{
    Set(hour, minute, second, millisec);
}

inline wxDateTime::wxDateTime(wxDateTime_t day,
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

inline wxLongLong wxDateTime::GetValue() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return m_time;
}

inline time_t wxDateTime::GetTicks() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));
    if ( !IsInStdRange() )
    {
        return (time_t)-1;
    }

    return (time_t)((m_time / (long)TIME_T_FACTOR).GetLo());
}

inline bool wxDateTime::SetToLastWeekDay(WeekDay weekday,
                                         Month month,
                                         int year)
{
    return SetToWeekDay(weekday, -1, month, year);
}

inline wxDateTime wxDateTime::GetWeekDayInSameWeek(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToWeekDayInSameWeek(weekday) );
}

inline wxDateTime wxDateTime::GetNextWeekDay(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToNextWeekDay(weekday) );
}

inline wxDateTime wxDateTime::GetPrevWeekDay(WeekDay weekday) const
{
    MODIFY_AND_RETURN( SetToPrevWeekDay(weekday) );
}

inline wxDateTime wxDateTime::GetWeekDay(WeekDay weekday,
                                         int n,
                                         Month month,
                                         int year) const
{
    wxDateTime dt(*this);

    return dt.SetToWeekDay(weekday, n, month, year) ? dt : wxInvalidDateTime;
}

inline wxDateTime wxDateTime::GetLastWeekDay(WeekDay weekday,
                                             Month month,
                                             int year)
{
    wxDateTime dt(*this);

    return dt.SetToLastWeekDay(weekday, month, year) ? dt : wxInvalidDateTime;
}

inline wxDateTime wxDateTime::GetWeek(wxDateTime_t numWeek,
                                      WeekDay weekday) const
{
    wxDateTime dt(*this);

    return dt.SetToTheWeek(numWeek, weekday) ? dt : wxInvalidDateTime;
}

inline wxDateTime wxDateTime::GetLastMonthDay(Month month, int year) const
{
    MODIFY_AND_RETURN( SetToLastMonthDay(month, year) );
}

inline wxDateTime wxDateTime::GetYearDay(wxDateTime_t yday) const
{
    MODIFY_AND_RETURN( SetToYearDay(yday) );
}

// ----------------------------------------------------------------------------
// wxDateTime comparison
// ----------------------------------------------------------------------------

inline bool wxDateTime::IsEqualTo(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time == datetime.m_time;
}

inline bool wxDateTime::IsEarlierThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time < datetime.m_time;
}

inline bool wxDateTime::IsLaterThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time > datetime.m_time;
}

inline bool wxDateTime::IsStrictlyBetween(const wxDateTime& t1,
                                          const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsLaterThan(t1) && IsEarlierThan(t2);
}

inline bool wxDateTime::IsBetween(const wxDateTime& t1,
                                  const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsEqualTo(t1) || IsEqualTo(t2) || IsStrictlyBetween(t1, t2);
}

inline bool wxDateTime::IsSameDate(const wxDateTime& dt) const
{
    return (m_time - dt.m_time).Abs() < MILLISECONDS_PER_DAY;
}

inline bool wxDateTime::IsSameTime(const wxDateTime& dt) const
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

inline bool wxDateTime::IsEqualUpTo(const wxDateTime& dt,
                                    const wxTimeSpan& ts) const
{
    return IsBetween(dt.Subtract(ts), dt.Add(ts));
}

// ----------------------------------------------------------------------------
// wxDateTime arithmetics
// ----------------------------------------------------------------------------

inline wxDateTime wxDateTime::Add(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time + diff.GetValue());
}

inline wxDateTime& wxDateTime::Add(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time += diff.GetValue();

    return *this;
}

inline wxDateTime& wxDateTime::operator+=(const wxTimeSpan& diff)
{
    return Add(diff);
}

inline wxDateTime wxDateTime::Subtract(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time - diff.GetValue());
}

inline wxDateTime& wxDateTime::Subtract(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time -= diff.GetValue();

    return *this;
}

inline wxDateTime& wxDateTime::operator-=(const wxTimeSpan& diff)
{
    return Subtract(diff);
}

inline wxTimeSpan wxDateTime::Subtract(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return wxTimeSpan(datetime.GetValue() - GetValue());
}

inline wxDateTime wxDateTime::Add(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Add(diff);
}

inline wxDateTime& wxDateTime::Subtract(const wxDateSpan& diff)
{
    return Add(diff.Negate());
}

inline wxDateTime wxDateTime::Subtract(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Subtract(diff);
}

inline wxDateTime& wxDateTime::operator-=(const wxDateSpan& diff)
{
    return Subtract(diff);
}

inline wxDateTime& wxDateTime::operator+=(const wxDateSpan& diff)
{
    return Add(diff);
}

// ----------------------------------------------------------------------------
// wxDateTime and timezones
// ----------------------------------------------------------------------------

inline wxDateTime wxDateTime::ToTimezone(const wxDateTime::TimeZone& tz,
                                         bool noDST) const
{
    MODIFY_AND_RETURN( MakeTimezone(tz, noDST) );
}

// ----------------------------------------------------------------------------
// wxTimeSpan construction
// ----------------------------------------------------------------------------

inline wxTimeSpan::wxTimeSpan(long hours,
                              long minutes,
                              long seconds,
                              long milliseconds)
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

inline wxLongLong wxTimeSpan::GetSeconds() const
{
    return m_diff / 1000l;
}

inline int wxTimeSpan::GetMinutes() const
{
    return (GetSeconds() / 60l).GetLo();
}

inline int wxTimeSpan::GetHours() const
{
    return GetMinutes() / 60;
}

inline int wxTimeSpan::GetDays() const
{
    return GetHours() / 24;
}

inline int wxTimeSpan::GetWeeks() const
{
    return GetDays() / 7;
}

// ----------------------------------------------------------------------------
// wxTimeSpan arithmetics
// ----------------------------------------------------------------------------

inline wxTimeSpan wxTimeSpan::Add(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff + diff.GetValue());
}

inline wxTimeSpan& wxTimeSpan::Add(const wxTimeSpan& diff)
{
    m_diff += diff.GetValue();

    return *this;
}

inline wxTimeSpan wxTimeSpan::Subtract(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff - diff.GetValue());
}

inline wxTimeSpan& wxTimeSpan::Subtract(const wxTimeSpan& diff)
{
    m_diff -= diff.GetValue();

    return *this;
}

inline wxTimeSpan& wxTimeSpan::Multiply(int n)
{
    m_diff *= (long)n;

    return *this;
}

inline wxTimeSpan wxTimeSpan::Multiply(int n) const
{
    return wxTimeSpan(m_diff * (long)n);
}

inline wxTimeSpan wxTimeSpan::Abs() const
{
    return wxTimeSpan(GetValue().Abs());
}

inline bool wxTimeSpan::IsEqualTo(const wxTimeSpan& ts) const
{
    return GetValue() == ts.GetValue();
}

inline bool wxTimeSpan::IsLongerThan(const wxTimeSpan& ts) const
{
    return GetValue().Abs() > ts.GetValue().Abs();
}

// ----------------------------------------------------------------------------
// wxDateSpan
// ----------------------------------------------------------------------------

inline wxDateSpan& wxDateSpan::operator+=(const wxDateSpan& other)
{
    m_years += other.m_years;
    m_months += other.m_months;
    m_weeks += other.m_weeks;
    m_days += other.m_days;

    return *this;
}

inline wxDateSpan& wxDateSpan::Add(const wxDateSpan& other)
{
    return *this += other;
}

inline wxDateSpan wxDateSpan::Add(const wxDateSpan& other) const
{
    wxDateSpan ds(*this);
    ds.Add(other);
    return ds;
}

inline wxDateSpan& wxDateSpan::Multiply(int factor)
{
    m_years *= factor;
    m_months *= factor;
    m_weeks *= factor;
    m_days *= factor;

    return *this;
}

inline wxDateSpan wxDateSpan::Multiply(int factor) const
{
    wxDateSpan ds(*this);
    ds.Multiply(factor);
    return ds;
}

inline wxDateSpan wxDateSpan::Negate() const
{
    return wxDateSpan(-m_years, -m_months, -m_weeks, -m_days);
}

inline wxDateSpan& wxDateSpan::Neg()
{
    m_years = -m_years;
    m_months = -m_months;
    m_weeks = -m_weeks;
    m_days = -m_days;

    return *this;
}

inline wxDateSpan& wxDateSpan::operator-=(const wxDateSpan& other)
{
    return *this += other.Negate();
}

inline wxDateSpan& wxDateSpan::Subtract(const wxDateSpan& other)
{
    return *this -= other;
}

inline wxDateSpan wxDateSpan::Subtract(const wxDateSpan& other) const
{
    wxDateSpan ds(*this);
    ds.Subtract(other);
    return ds;
}

#undef MILLISECONDS_PER_DAY

#undef MODIFY_AND_RETURN
