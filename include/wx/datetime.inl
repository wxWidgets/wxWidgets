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
// wxDateTime statics
// ----------------------------------------------------------------------------

/* static */
wxDateTime::Country wxDateTime::GetCountry()
{
    return ms_country;
}

// ----------------------------------------------------------------------------
// wxDateTime construction
// ----------------------------------------------------------------------------

// only define this once, when included from datetime.cpp
#ifdef wxDEFINE_TIME_CONSTANTS
    const unsigned int wxDateTime::TIME_T_FACTOR = 1000;
#endif // wxDEFINE_TIME_CONSTANTS

bool wxDateTime::IsInStdRange() const
{
    return m_time >= 0l && (m_time / (long)TIME_T_FACTOR) < LONG_MAX;
}

/* static */
wxDateTime wxDateTime::Now()
{
    return wxDateTime(GetTimeNow());
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
    wxASSERT_MSG( IsValid(), "invalid wxDateTime");

    return m_time;
}

time_t wxDateTime::GetTicks() const
{
    wxASSERT_MSG( IsValid(), "invalid wxDateTime");
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
    wxASSERT_MSG( IsValid() && datetime.IsValid(), "invalid wxDateTime");

    return m_time == datetime.m_time;
}

bool wxDateTime::operator==(const wxDateTime& datetime) const
{
    return IsEqualTo(datetime);
}

bool wxDateTime::operator!=(const wxDateTime& datetime) const
{
    return !IsEqualTo(datetime);
}

bool wxDateTime::IsEarlierThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), "invalid wxDateTime");

    return m_time < datetime.m_time;
}

bool wxDateTime::IsLaterThan(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), "invalid wxDateTime");

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

// ----------------------------------------------------------------------------
// wxDateTime arithmetics
// ----------------------------------------------------------------------------

wxDateTime& wxDateTime::Add(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), "invalid wxDateTime");

    m_time += diff.GetValue();

    return *this;
}

wxDateTime& wxDateTime::operator+=(const wxTimeSpan& diff)
{
    return Add(diff);
}

wxDateTime& wxDateTime::Substract(const wxTimeSpan& diff)
{
    wxASSERT_MSG( IsValid(), "invalid wxDateTime");

    m_time -= diff.GetValue();

    return *this;
}

wxDateTime& wxDateTime::operator-=(const wxTimeSpan& diff)
{
    return Substract(diff);
}

wxTimeSpan wxDateTime::Substract(const wxDateTime& datetime) const
{
    wxASSERT_MSG( IsValid() && datetime.IsValid(), "invalid wxDateTime");

    return wxTimeSpan(datetime.GetValue() - GetValue());
}

wxTimeSpan wxDateTime::operator-(const wxDateTime& datetime) const
{
    return Substract(datetime);
}

wxDateTime& wxDateTime::Substract(const wxDateSpan& diff)
{
    return Add(diff.Negate());
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

wxDateTime wxDateTime::ToUTC() const
{
    return wxDateTime(*this).MakeUTC();
}

wxDateTime wxDateTime::ToTimezone(const wxDateTime::TimeZone& tz) const
{
    return wxDateTime(*this).MakeTimezone(tz);
}

wxDateTime wxDateTime::ToLocalTime(const wxDateTime::TimeZone& tz) const
{
    return wxDateTime(*this).MakeLocalTime(tz);
}

// ----------------------------------------------------------------------------
// wxTimeSpan
// ----------------------------------------------------------------------------

wxTimeSpan::wxTimeSpan(int hours, int minutes, int seconds, int milliseconds)
{
    // assign first to avoid precision loss
    m_diff = hours;
    m_diff *= 60;
    m_diff += minutes;
    m_diff *= 60;
    m_diff += seconds;
    m_diff *= 1000;
    m_diff += milliseconds;
}

wxTimeSpan& wxTimeSpan::Add(const wxTimeSpan& diff)
{
    m_diff += diff.GetValue();

    return *this;
}

wxTimeSpan& wxTimeSpan::Substract(const wxTimeSpan& diff)
{
    m_diff -= diff.GetValue();

    return *this;
}

wxTimeSpan& wxTimeSpan::Multiply(int n)
{
    m_diff *= n;

    return *this;
}

wxTimeSpan wxTimeSpan::operator*(int n) const
{
    wxTimeSpan result(*this);
    result.Multiply(n);

    return result;
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

wxDateSpan& wxDateSpan::operator*=(int factor)
{
    m_years *= m_years;
    m_months *= m_months;
    m_weeks *= m_weeks;
    m_days *= m_days;

    return *this;
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

