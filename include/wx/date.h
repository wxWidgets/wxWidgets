///////////////////////////////////////////////////////////////////////////////
// Name:        wx/date.h
// Purpose:     wxDate class: this class is deprecated, use wxDateTime instead!
// Author:      Julian Smart, Steve Marcus, Eric Simon, Chris Hill,
//              Charles D. Price
// Modified by: 18.12.99 by VZ to use the new wxDateTime class
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:(c)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATE_H_
#define _WX_DATE_H_

#ifdef __GNUG__
    #pragma interface "date.h"
#endif

#include "wx/defs.h"

#if wxUSE_TIMEDATE

#include "wx/object.h"
#include "wx/string.h"
#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum wxdate_format_type
{
    wxMDY,
    wxDAY,
    wxMONTH,
    wxFULL,
    wxEUROPEAN
};

enum // wxdate_format_flags
{
    wxNO_CENTURY = 0x02,
    wxDATE_ABBR  = 0x04
};

// ----------------------------------------------------------------------------
// wxDate
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDate : public wxObject
{
public:
    wxDate() { Init(); }
    wxDate(long j) : m_date((double)(j + 0.5)) { Init(); }
    wxDate(int m, int d, int y) : m_date(d, (wxDateTime::Month)m, y) { Init(); }
    wxDate(const wxString& dat) { Init(); (void)m_date.ParseDate(dat); }
    wxDate(const wxDate &date) { *this = date; }

    wxDate(const wxDateTime& dt) { Init(); m_date = dt; }

#ifndef __SALFORDC__
    operator wxString() const { return FormatDate(); }
#endif

    void operator=(const wxDate& date)
    {
        m_date = date.m_date;
        m_displayFormat = date.m_displayFormat;
        m_displayOptions = date.m_displayOptions;
    }

    void operator=(const wxString& dat) { (void)m_date.ParseDate(dat); }

    wxDate operator+(long i) { return wxDate(GetJulianDate() + i); }
    wxDate operator+(int  i) { return wxDate(GetJulianDate() + (long)i); }

    wxDate operator-(long i) { return wxDate(GetJulianDate() - i); }
    wxDate operator-(int  i) { return wxDate(GetJulianDate() - (long)i); }

    long operator-(const wxDate &dt) const
        { return GetJulianDate() - dt.GetJulianDate(); }

    wxDate &operator+=(long i) { m_date += wxTimeSpan::Days((int)i); return *this; }
    wxDate &operator-=(long i) { m_date -= wxTimeSpan::Days((int)i); return *this; }

    wxDate &operator++() { return *this += 1; }
    wxDate &operator++(int) { return *this += 1; }
    wxDate &operator--() { return *this -= 1; }
    wxDate &operator--(int) { return *this -= 1; }

#if wxUSE_STD_IOSTREAM
    friend ostream WXDLLEXPORT & operator <<(ostream &os, const wxDate &dt)
        { return os << dt.FormatDate().mb_str(); }
#endif

    void  SetFormat(int format) { m_displayFormat = format; }
    int   SetOption(int option, bool enable = TRUE)
    {
        if ( enable )
            m_displayOptions |= option;
        else
            m_displayOptions &= ~option;

        return 1; // (VZ: whatever it means)
    }

    // returns julian date (VZ: the integral part of Julian Day Number)
    long GetJulianDate() const
        { return (long)(m_date.GetJulianDayNumber() - 0.5); }

    // returns relative date since Jan. 1
    int GetDayOfYear() const
        { return m_date.GetDayOfYear(); }

    // returns TRUE if leap year, FALSE if not
    bool IsLeapYear() const
        { return wxDateTime::IsLeapYear(m_date.GetYear()); }

    // Sets to current system date
    wxDate &Set()
        { m_date = wxDateTime::Today(); return (wxDate&)*this; }
    wxDate &Set(long lJulian)
        { m_date.Set((double)(lJulian + 0.5)); return (wxDate&)*this; }
    wxDate &Set(int nMonth, int nDay, int nYear)
        { m_date.Set(nDay, (wxDateTime::Month)nMonth, nYear); return *this; }

    // May also pass neg# to decrement
    wxDate &AddWeeks(int nCount = 1)
        { m_date += wxDateSpan::Weeks(nCount); return *this; }
    wxDate &AddMonths(int nCount = 1)
        { m_date += wxDateSpan::Months(nCount); return *this; }
    wxDate &AddYears(int nCount = 1)
        { m_date += wxDateSpan::Years(nCount); return *this; }

    // Numeric Day of date object
    int GetDay() const { return m_date.GetDay(); }
    // Number of days in month(1..31)
    int GetDaysInMonth() const
    {
        return wxDateTime::GetNumberOfDays((wxDateTime::Month)m_date.GetMonth(),
                                           m_date.GetYear());
    }

    // First Day Of Month(1..7)
    int GetFirstDayOfMonth() const
        { return wxDate(GetMonth(), 1, GetYear()).GetDayOfWeek(); }

    // Character Day Of Week('Sunday'..'Saturday')
    wxString GetDayOfWeekName() const { return FormatDate(wxDAY); }
    int GetDayOfWeek() const { return (int)m_date.GetWeekDay() + 1; }

    // Numeric Week Of Month(1..6) (VZ: I'd love to see a month with 6 weeks)
    int GetWeekOfMonth() const { return m_date.GetWeekOfMonth(); }
    // Numeric Week Of Year(1..52) (VZ: but there are years with 53 weeks)
    int GetWeekOfYear() const { return m_date.GetWeekOfYear(); }

    // Character Month name
    wxString GetMonthName() { return FormatDate(wxMONTH); }
    // Month Number(1..12)
    int GetMonth() const { return m_date.GetMonth() + 1; }

    // First Date Of Month
    wxDate GetMonthStart() const { return(wxDate(GetMonth(), 1, GetYear())); }
    // Last Date Of Month
    wxDate GetMonthEnd() const { return wxDate(GetMonth()+1, 1, GetYear())-1; }

    // eg. 1992
    int GetYear() const { return m_date.GetYear(); }
    // First Date Of Year
    wxDate GetYearStart() const { return wxDate(1, 1, GetYear()); }
    // Last Date Of Year
    wxDate GetYearEnd() const { return wxDate(1, 1, GetYear()+1) - 1; }

    bool IsBetween(const wxDate& first, const wxDate& second) const
    {
        return m_date.IsBetween(first.m_date, second.m_date);
    }

    wxDate Previous(int dayOfWeek) const
    {
        wxDate prev = *this;
        int dow = GetDayOfWeek();
        prev -= dayOfWeek > dow ? 7 - (dayOfWeek - dow) : dow - dayOfWeek;

        return prev;
    }

    wxString FormatDate(int type = -1) const
    {
        static const wxChar *formats[] =
        {
            // MDY          (week)DAY   MONTH   FULL                 EUROPEAN
            _T("%m/%d/%Y"), _T("%A"), _T("%B"), _T("%A, %B %d, %Y"), _T("%d %B %Y")
        };

        wxString fmt = formats[type == -1 ? m_displayFormat : type];

        if ( m_displayOptions & wxDATE_ABBR )
        {
            fmt.Replace(_T("A"), _T("a"));
            fmt.Replace(_T("B"), _T("b"));
        }
        if ( m_displayOptions & wxNO_CENTURY )
        {
            fmt.Replace(_T("Y"), _T("y"));
        }

        return m_date.Format(fmt);
    }

protected:
    void Init() { m_displayFormat = wxMDY; m_displayOptions = 0; }

#if 0 // the old wxDate members - unused any more
    unsigned long julian; // see julDate();  days since 1/1/4713 B.C.
    int month;            // see NMonth()
    int day;              // see Day()
    int year;             // see NYear4()
    int day_of_week;      // see NDOW();  1 = Sunday, ... 7 = Saturday

    void julian_to_mdy();         // convert julian day to mdy
    void julian_to_wday();        // convert julian day to day_of_week
    void mdy_to_julian();         // convert mdy to julian day
#endif // 0

private:
    wxDateTime m_date;

    int m_displayFormat;
    int m_displayOptions;

private:
    DECLARE_DYNAMIC_CLASS(wxDate)
};

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

inline bool WXDLLEXPORT operator <(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() < dt2.GetJulianDate(); }
inline bool WXDLLEXPORT operator <=(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() <= dt2.GetJulianDate(); }
inline bool WXDLLEXPORT operator >(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() > dt2.GetJulianDate(); }
inline bool WXDLLEXPORT operator >=(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() >= dt2.GetJulianDate(); }
inline bool WXDLLEXPORT operator ==(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() == dt2.GetJulianDate(); }
inline bool WXDLLEXPORT operator !=(const wxDate &dt1, const wxDate &dt2)
    { return dt1.GetJulianDate() != dt2.GetJulianDate(); }

#endif // wxUSE_TIMEDATE
#endif
    // _WX_DATE_H_
