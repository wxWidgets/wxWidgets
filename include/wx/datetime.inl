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

#define MILLISECONDS_PER_DAY 86400000l

// ----------------------------------------------------------------------------
// wxDateTime construction
// ----------------------------------------------------------------------------

// only define this once, when included from datetime.cpp
#ifdef wxDEFINE_TIME_CONSTANTS
    const long wxDateTime::TIME_T_FACTOR = 1000l;
#endif // wxDEFINE_TIME_CONSTANTS

bool wxDateTime::IsInStdRange() const
{
    return m_time >= 0l && (m_time / TIME_T_FACTOR) < LONG_MAX;
}

/* static */
wxDateTime wxDateTime::Now()
{
    return wxDateTime(GetTimeNow());
}

/* static */
wxDateTime wxDateTime::Today()
{
    return wxDateTime((time_t)(86400*(GetTimeNow() / 86400)));
}

wxDateTime& wxDateTime::Set(time_t timet)
{
    // assign first to avoid long multiplication overflow!
    m_time = timet;
    m_time *= TIME_T_FACTOR;

    return *this;
}

wxDateTime& wxDateTime::SetToCurrent()
{
    return Set(GetTimeNow());
}

wxDateTime::wxDateTime(time_t timet)
{
    Set(timet);
}

wxDateTime::wxDateTime(const struct tm& tm)
{
    Set(tm);
}

wxDateTime::wxDateTime(const Tm& tm)
{
    Set(tm);
}

wxDateTime::wxDateTime(double jdn)
{
    Set(jdn);
}

wxDateTime& wxDateTime::Set(const Tm& tm)
{
    wxASSERT_MSG( tm.IsValid(), _T("invalid broken down date/time") );

    return Set(tm.mday, (Month)tm.mon, tm.year, tm.hour, tm.min, tm.sec);
}

wxDateTime::wxDateTime(wxDateTime_t hour,
                       wxDateTime_t minute,
                       wxDateTime_t second,
                       wxDateTime_t millisec)
{
    Set(hour, minute, second, millisec);
}

wxDateTime::wxDateTime(wxDateTime_t day,
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

wxLongLong wxDateTime::GetValue() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return m_time;
}

time_t wxDateTime::GetTicks() const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));
    if ( !IsInStdRange() )
    {
        return (time_t)-1;
    }

    return (time_t)((m_time / (long)TIME_T_FACTOR).GetLo());
}

bool wxDateTime::SetToLastWeekDay(WeekDay weekday,
                                  Month month,
                                  int year)
{
    return SetToWeekDay(weekday, -1, month, year);
}

// ----------------------------------------------------------------------------
// wxDateTime comparison
// ----------------------------------------------------------------------------

bool wxDateTime::IsEqualTo(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time == datetime.m_time;
}

bool wxDateTime::IsEarlierThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time < datetime.m_time;
}

bool wxDateTime::IsLaterThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return m_time > datetime.m_time;
}

bool wxDateTime::IsStrictlyBetween(const wxDateTime& t1,
                                   const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsLaterThan(t1) && IsEarlierThan(t2);
}

bool wxDateTime::IsBetween(const wxDateTime& t1, const wxDateTime& t2) const
{
    // no need for assert, will be checked by the functions we call
    return IsEqualTo(t1) || IsEqualTo(t2) || IsStrictlyBetween(t1, t2);
}

bool wxDateTime::IsSameDate(const wxDateTime& dt) const
{
    return (m_time - dt.m_time).Abs() < MILLISECONDS_PER_DAY;
}

bool wxDateTime::IsSameTime(const wxDateTime& dt) const
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

bool wxDateTime::IsEqualUpTo(const wxDateTime& dt, const wxTimeSpan& ts) const
{
    return IsBetween(dt.Substract(ts), dt.Add(ts));
}

// ----------------------------------------------------------------------------
// wxDateTime arithmetics
// ----------------------------------------------------------------------------

wxDateTime wxDateTime::Add(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time + diff.GetValue());
}

wxDateTime& wxDateTime::Add(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time += diff.GetValue();

    return *this;
}

wxDateTime& wxDateTime::operator+=(const wxTimeSpan& diff)
{
    return Add(diff);
}

wxDateTime wxDateTime::Substract(const wxTimeSpan& diff) const
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    return wxDateTime(m_time - diff.GetValue());
}

wxDateTime& wxDateTime::Substract(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), _T("invalid wxDateTime"));

    m_time -= diff.GetValue();

    return *this;
}

wxDateTime& wxDateTime::operator-=(const wxTimeSpan& diff)
{
    return Substract(diff);
}

wxTimeSpan wxDateTime::Substract(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), _T("invalid wxDateTime"));

    return wxTimeSpan(datetime.GetValue() - GetValue());
}

wxDateTime wxDateTime::Add(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Add(diff);
}

wxDateTime& wxDateTime::Substract(const wxDateSpan& diff)
{
    return Add(diff.Negate());
}

wxDateTime wxDateTime::Substract(const wxDateSpan& diff) const
{
    return wxDateTime(*this).Substract(diff);
}

wxDateTime& wxDateTime::operator-=(const wxDateSpan& diff)
{
    return Substract(diff);
}

wxDateTime& wxDateTime::operator+=(const wxDateSpan& diff)
{
    return Add(diff);
}

// ----------------------------------------------------------------------------
// wxDateTime and timezones
// ----------------------------------------------------------------------------

wxDateTime wxDateTime::ToTimezone(const wxDateTime::TimeZone& tz) const
{
    return wxDateTime(*this).MakeTimezone(tz);
}

// ----------------------------------------------------------------------------
// wxTimeSpan construction
// ----------------------------------------------------------------------------

wxTimeSpan::wxTimeSpan(int hours, int minutes, int seconds, int milliseconds)
{
    // assign first to avoid precision loss
    m_diff = (long)hours;
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

wxLongLong wxTimeSpan::GetSeconds() const
{
    return m_diff / 1000l;
}

int wxTimeSpan::GetMinutes() const
{
    return (GetSeconds() / 60l).GetLo();
}

int wxTimeSpan::GetHours() const
{
    return GetMinutes() / 60;
}

int wxTimeSpan::GetDays() const
{
    return GetHours() / 24;
}

int wxTimeSpan::GetWeeks() const
{
    return GetDays() / 7;
}

// ----------------------------------------------------------------------------
// wxTimeSpan arithmetics
// ----------------------------------------------------------------------------

wxTimeSpan wxTimeSpan::Add(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff + diff.GetValue());
}

wxTimeSpan& wxTimeSpan::Add(const wxTimeSpan& diff)
{
    m_diff += diff.GetValue();

    return *this;
}

wxTimeSpan wxTimeSpan::Substract(const wxTimeSpan& diff) const
{
    return wxTimeSpan(m_diff - diff.GetValue());
}

wxTimeSpan& wxTimeSpan::Substract(const wxTimeSpan& diff)
{
    m_diff -= diff.GetValue();

    return *this;
}

wxTimeSpan& wxTimeSpan::Multiply(int n)
{
    m_diff *= (long)n;

    return *this;
}

wxTimeSpan wxTimeSpan::Multiply(int n) const
{
    return wxTimeSpan(m_diff * (long)n);
}

wxTimeSpan wxTimeSpan::Abs() const
{
    return wxTimeSpan(GetValue().Abs());
}

bool wxTimeSpan::IsEqualTo(const wxTimeSpan& ts) const
{
    return GetValue() == ts.GetValue();
}

bool wxTimeSpan::IsLongerThan(const wxTimeSpan& ts) const
{
    return GetValue().Abs() > ts.GetValue().Abs();
}

// ----------------------------------------------------------------------------
// wxDateSpan
// ----------------------------------------------------------------------------

wxDateSpan&
wxDateSpan::operator+=(const wxDateSpan& other)
{
    m_years += other.m_years;
    m_months += other.m_months;
    m_weeks += other.m_weeks;
    m_days += other.m_days;

    return *this;
}

wxDateSpan& wxDateSpan::Multiply(int factor)
{
    m_years *= m_years;
    m_months *= m_months;
    m_weeks *= m_weeks;
    m_days *= m_days;

    return *this;
}

wxDateSpan wxDateSpan::Multiply(int factor) const
{
    return wxDateSpan(*this).Multiply(factor);
}

wxDateSpan wxDateSpan::Negate() const
{
    return wxDateSpan(-m_years, -m_months, -m_weeks, -m_days);
}

wxDateSpan& wxDateSpan::Neg()
{
    m_years = -m_years;
    m_months = -m_months;
    m_weeks = -m_weeks;
    m_days = -m_days;

    return *this;
}

#undef MILLISECONDS_PER_DAY
