///////////////////////////////////////////////////////////////////////////////
// Name:        wx/time.h
// Purpose:     wxTime class, from NIHCL: this class is deprecated, use
//              wxDateTime instead
// Author:      Julian Smart, after K. E. Gorlen
// Modified by: 18.12.99 by VZ to use the new wxDateTime class
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMEH__
#define _WX_TIMEH__

#include "wx/defs.h"

#if wxUSE_TIMEDATE

#include "wx/object.h"
#include "wx/datetime.h"
#include "wx/date.h"

#ifdef __GNUG__
    #pragma interface "time.h"
#endif

class WXDLLEXPORT wxDate;

typedef unsigned short hourTy;
typedef unsigned short minuteTy;
typedef unsigned short secondTy;
typedef unsigned long clockTy;

// seconds from 1/1/01 to 1/1/70
#define wxTIME_EPOCH_DIFF 2177452800UL

class WXDLLEXPORT wxTime : public wxObject
{
public:
    // type definitions
    enum tFormat { wx12h, wx24h };
    enum tPrecision { wxStdMinSec, wxStdMin };

public:
    // current time 
    wxTime() : m_time(wxDateTime::Now()) { }
    wxTime(clockTy s) : m_time((time_t)(s - wxTIME_EPOCH_DIFF)) { }
    void operator=(const wxTime& t) { m_time = t.m_time; }
    wxTime(const wxTime& t) { *this = t; }
    wxTime(hourTy h, minuteTy m, secondTy s = 0, bool WXUNUSED(dst) = FALSE)
        : m_time(h, m, s) { }

    wxTime(const wxDate& d, hourTy h = 0, minuteTy m = 0, secondTy s = 0,
           bool WXUNUSED(dst) = FALSE)
           : m_time(d.GetDay(), (wxDateTime::Month)d.GetMonth(), d.GetYear(),
                    h, m, s) { }

    wxTime(const wxDateTime& time) : m_time(time) { }

    // Convert to string
#ifndef __SALFORDC__
    operator wxChar *() const { return FormatTime(); }
    operator wxDate() const { return wxDate(m_time); }
#endif

    bool operator< (const wxTime& t) const { return m_time < t.m_time; }
    bool operator<=(const wxTime& t) const { return m_time <= t.m_time; }
    bool operator> (const wxTime& t) const { return m_time > t.m_time; }
    bool operator>=(const wxTime& t) const { return m_time >= t.m_time; }
    bool operator==(const wxTime& t) const { return m_time == t.m_time; }
    bool operator!=(const wxTime& t) const { return m_time != t.m_time; }

    friend wxTime WXDLLEXPORT operator+(const wxTime& t, long s)
        { return wxTime(t.m_time + wxTimeSpan::Seconds((int)s)); }
    friend wxTime WXDLLEXPORT operator+(long s, const wxTime& t)
        { return wxTime(t.m_time + wxTimeSpan::Seconds((int)s)); }

    long operator-(const wxTime& t) const
        { return (m_time - t.m_time).GetValue().ToLong(); }
    wxTime operator-(long s) const
        { return wxTime(m_time - wxTimeSpan::Seconds((int)s)); }
    void operator+=(long s) { m_time += wxTimeSpan::Seconds((int)s); }
    void operator-=(long s) { m_time -= wxTimeSpan::Seconds((int)s); }
    bool IsBetween(const wxTime& a, const wxTime& b) const
        { return *this >= a && *this <= b; }

    // Get day
    int GetDay() const { return m_time.GetDay(); }
    // Get month
    int GetMonth() const { return m_time.GetMonth(); }
    // Get year
    int GetYear() const { return m_time.GetYear(); }
    // Get day of week (0=Sunday 6=Saturday)
    int GetDayOfWeek() const { return m_time.GetWeekDay(); }

    hourTy GetHour() const { return (hourTy)m_time.GetHour(); }
    hourTy GetHourGMT() const { return (hourTy)m_time.GetHour(wxDateTime::GMT0); }
    minuteTy GetMinute() const { return (hourTy)m_time.GetMinute(); }
    minuteTy GetMinuteGMT() const { return (hourTy)m_time.GetMinute(wxDateTime::GMT0); }
    secondTy GetSecond() const { return (hourTy)m_time.GetSecond(); }
    secondTy GetSecondGMT() const { return (hourTy)m_time.GetSecond(wxDateTime::GMT0); }

    clockTy GetSeconds() const { return (clockTy)m_time.GetValue().ToLong(); }

    wxTime Max(const wxTime& t) const { return (t < *this) ? *this : t; }
    wxTime Min(const wxTime& t) const { return (t > *this) ? *this : t; }

    static void SetFormat(const tFormat lFormat = wx12h,
                          const tPrecision lPrecision = wxStdMinSec)
    {
        ms_Format    = lFormat;
        ms_Precision  = lPrecision;
    }

    // (VZ: DANGER: returns pointer to static buffer)
    wxChar *FormatTime() const
    {
        static const wxChar *formats[2][2] =
        {
            // wxStdMinSec          wxStdMin
            { _T("%I:%M:%S %p"), _T("%I:%M %p") },  // wx12h
            { _T("%H:%M:%S"),    _T("%H:%M")    }   // wx24h
        };

        static wxChar s_bufTime[128];

        wxStrncpy(s_bufTime, m_time.Format(formats[ms_Format][ms_Precision]),
                  WXSIZEOF(s_bufTime));

        return s_bufTime;
    }

private:
    static tFormat      ms_Format;
    static tPrecision   ms_Precision;

#if 0 // old wxTime members unused any more
    clockTy    sec;        /* seconds since 1/1/1901 */

    bool IsDST() const;
    wxTime GetLocalTime() const;

    // static member functions
    static wxTime GetLocalTime(const wxDate& date, hourTy h=0, minuteTy m=0, secondTy s=0);
    static wxTime GetBeginDST(unsigned year);
    static wxTime GetEndDST(unsigned year);
#endif // 0

    wxDateTime m_time;

    DECLARE_DYNAMIC_CLASS(wxTime)
};

#endif
  // wxUSE_TIMEDATE
#endif
    // _WX_TIMEH__

