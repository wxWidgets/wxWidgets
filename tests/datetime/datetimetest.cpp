///////////////////////////////////////////////////////////////////////////////
// Name:        tests/datetime/datetime.cpp
// Purpose:     wxDateTime unit test
// Author:      Vadim Zeitlin
// Created:     2004-06-23 (extracted from samples/console/console.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#if wxUSE_DATETIME

#include "wx/datetime.h"
#include "wx/ioswrap.h"

// need this to be able to use CPPUNIT_ASSERT_EQUAL with wxDateTime objects
static wxSTD ostream& operator<<(wxSTD ostream& ostr, const wxDateTime& dt)
{
    ostr << dt.FormatISODate() << _T(" ") << dt.FormatISOTime();

    return ostr;
}

// to test Today() meaningfully we must be able to change the system date which
// is not usually the case, but if we're under Win32 we can try it -- define
// the macro below to do it
//#define CHANGE_SYSTEM_DATE

#ifndef __WINDOWS__
    #undef CHANGE_SYSTEM_DATE
#endif

#ifdef CHANGE_SYSTEM_DATE

class DateChanger
{
public:
    DateChanger(int year, int month, int day, int hour, int min, int sec)
    {
        SYSTEMTIME st;
        st.wDay = day;
        st.wMonth = month;
        st.wYear = year;
        st.wHour = hour;
        st.wMinute = min;
        st.wSecond = sec;
        st.wMilliseconds = 0;

        ::GetSystemTime(&m_savedTime);
        ::GetTimeZoneInformation(&m_tzi);

        m_changed = ::SetSystemTime(&st) != 0;
    }

    ~DateChanger()
    {
        if ( m_changed )
        {
            ::SetSystemTime(&m_savedTime);
            ::SetTimeZoneInformation(&m_tzi);
        }
    }

private:
    SYSTEMTIME m_savedTime;
    TIME_ZONE_INFORMATION m_tzi;
    bool m_changed;
};

#endif // CHANGE_SYSTEM_DATE

// ----------------------------------------------------------------------------
// broken down date representation used for testing
// ----------------------------------------------------------------------------

struct Date
{
    wxDateTime::wxDateTime_t day;
    wxDateTime::Month month;
    int year;
    wxDateTime::wxDateTime_t hour, min, sec;
    double jdn;
    wxDateTime::WeekDay wday;
    time_t gmticks, ticks;

    void Init(const wxDateTime::Tm& tm)
    {
        day = tm.mday;
        month = tm.mon;
        year = tm.year;
        hour = tm.hour;
        min = tm.min;
        sec = tm.sec;
        jdn = 0.0;
        gmticks = ticks = -1;
    }

    wxDateTime DT() const
        { return wxDateTime(day, month, year, hour, min, sec); }

    bool SameDay(const wxDateTime::Tm& tm) const
    {
        return day == tm.mday && month == tm.mon && year == tm.year;
    }

    wxString Format() const
    {
        wxString s;
        s.Printf(_T("%02d:%02d:%02d %10s %02d, %4d%s"),
                 hour, min, sec,
                 wxDateTime::GetMonthName(month).c_str(),
                 day,
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? _T("AD") : _T("BC"));
        return s;
    }

    wxString FormatDate() const
    {
        wxString s;
        s.Printf(_T("%02d-%s-%4d%s"),
                 day,
                 wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? _T("AD") : _T("BC"));
        return s;
    }
};

// ----------------------------------------------------------------------------
// test data
// ----------------------------------------------------------------------------

static const Date testDates[] =
{
    {  1, wxDateTime::Jan,  1970, 00, 00, 00, 2440587.5, wxDateTime::Thu,         0,     -3600 },
    {  7, wxDateTime::Feb,  2036, 00, 00, 00, 2464730.5, wxDateTime::Thu,        -1,        -1 },
    {  8, wxDateTime::Feb,  2036, 00, 00, 00, 2464731.5, wxDateTime::Fri,        -1,        -1 },
    {  1, wxDateTime::Jan,  2037, 00, 00, 00, 2465059.5, wxDateTime::Thu,        -1,        -1 },
    {  1, wxDateTime::Jan,  2038, 00, 00, 00, 2465424.5, wxDateTime::Fri,        -1,        -1 },
    { 21, wxDateTime::Jan,  2222, 00, 00, 00, 2532648.5, wxDateTime::Mon,        -1,        -1 },
    { 29, wxDateTime::May,  1976, 12, 00, 00, 2442928.0, wxDateTime::Sat, 202219200, 202212000 },
    { 29, wxDateTime::Feb,  1976, 00, 00, 00, 2442837.5, wxDateTime::Sun, 194400000, 194396400 },
    {  1, wxDateTime::Jan,  1900, 12, 00, 00, 2415021.0, wxDateTime::Mon,        -1,        -1 },
    {  1, wxDateTime::Jan,  1900, 00, 00, 00, 2415020.5, wxDateTime::Mon,        -1,        -1 },
    { 15, wxDateTime::Oct,  1582, 00, 00, 00, 2299160.5, wxDateTime::Fri,        -1,        -1 },
    {  4, wxDateTime::Oct,  1582, 00, 00, 00, 2299149.5, wxDateTime::Mon,        -1,        -1 },
    {  1, wxDateTime::Mar,     1, 00, 00, 00, 1721484.5, wxDateTime::Thu,        -1,        -1 },
    {  1, wxDateTime::Jan,     1, 00, 00, 00, 1721425.5, wxDateTime::Mon,        -1,        -1 },
    { 31, wxDateTime::Dec,     0, 00, 00, 00, 1721424.5, wxDateTime::Sun,        -1,        -1 },
    {  1, wxDateTime::Jan,     0, 00, 00, 00, 1721059.5, wxDateTime::Sat,        -1,        -1 },
    { 12, wxDateTime::Aug, -1234, 00, 00, 00, 1270573.5, wxDateTime::Fri,        -1,        -1 },
    { 12, wxDateTime::Aug, -4000, 00, 00, 00,  260313.5, wxDateTime::Sat,        -1,        -1 },
    { 24, wxDateTime::Nov, -4713, 00, 00, 00,      -0.5, wxDateTime::Mon,        -1,        -1 },
};


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DateTimeTestCase : public CppUnit::TestCase
{
public:
    DateTimeTestCase() { }

private:
    CPPUNIT_TEST_SUITE( DateTimeTestCase );
        CPPUNIT_TEST( TestLeapYears );
        CPPUNIT_TEST( TestTimeSet );
        CPPUNIT_TEST( TestTimeJDN );
        CPPUNIT_TEST( TestTimeWNumber );
        CPPUNIT_TEST( TestTimeWDays );
        CPPUNIT_TEST( TestTimeDST );
        CPPUNIT_TEST( TestTimeFormat );
        CPPUNIT_TEST( TestTimeSpanFormat );
        CPPUNIT_TEST( TestTimeTicks );
        CPPUNIT_TEST( TestParceRFC822 );
        CPPUNIT_TEST( TestDateParse );
        CPPUNIT_TEST( TestTimeArithmetics );
        CPPUNIT_TEST( TestDSTBug );
        CPPUNIT_TEST( TestDateOnly );
    CPPUNIT_TEST_SUITE_END();

    void TestLeapYears();
    void TestTimeSet();
    void TestTimeJDN();
    void TestTimeWNumber();
    void TestTimeWDays();
    void TestTimeDST();
    void TestTimeFormat();
    void TestTimeSpanFormat();
    void TestTimeTicks();
    void TestParceRFC822();
    void TestDateParse();
    void TestTimeArithmetics();
    void TestDSTBug();
    void TestDateOnly();

    DECLARE_NO_COPY_CLASS(DateTimeTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DateTimeTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DateTimeTestCase, "DateTimeTestCase" );

// ============================================================================
// implementation
// ============================================================================

// test leap years detection
void DateTimeTestCase::TestLeapYears()
{
    static const struct LeapYearTestData
    {
        int year;
        bool isLeap;
    } years[] =
    {
        { 1900, false },
        { 1990, false },
        { 1976, true },
        { 2000, true },
        { 2030, false },
        { 1984, true },
        { 2100, false },
        { 2400, true },
    };

    for ( size_t n = 0; n < WXSIZEOF(years); n++ )
    {
        const LeapYearTestData& y = years[n];

        CPPUNIT_ASSERT( wxDateTime::IsLeapYear(y.year) == y.isLeap );
    }
}

// test constructing wxDateTime objects
void DateTimeTestCase::TestTimeSet()
{
    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d1 = testDates[n];
        wxDateTime dt = d1.DT();

        Date d2;
        d2.Init(dt.GetTm());

        wxString s1 = d1.Format(),
                 s2 = d2.Format();

        CPPUNIT_ASSERT( s1 == s2 );
    }
}

// test conversions to JDN &c
void DateTimeTestCase::TestTimeJDN()
{
    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        // JDNs must be computed for UTC times
        double jdn = dt.FromUTC().GetJulianDayNumber();

        CPPUNIT_ASSERT( jdn == d.jdn );

        dt.Set(jdn);
        CPPUNIT_ASSERT( dt.GetJulianDayNumber() == jdn );
    }
}

// test week days computation
void DateTimeTestCase::TestTimeWDays()
{
    // test GetWeekDay()
    size_t n;
    for ( n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        CPPUNIT_ASSERT( wday == d.wday );
    }

    // test SetToWeekDay()
    struct WeekDateTestData
    {
        Date date;                  // the real date (precomputed)
        int nWeek;                  // its week index in the month
        wxDateTime::WeekDay wday;   // the weekday
        wxDateTime::Month month;    // the month
        int year;                   // and the year

        wxString Format() const
        {
            wxString s, which;
            switch ( nWeek < -1 ? -nWeek : nWeek )
            {
                case 1: which = _T("first"); break;
                case 2: which = _T("second"); break;
                case 3: which = _T("third"); break;
                case 4: which = _T("fourth"); break;
                case 5: which = _T("fifth"); break;

                case -1: which = _T("last"); break;
            }

            if ( nWeek < -1 )
            {
                which += _T(" from end");
            }

            s.Printf(_T("The %s %s of %s in %d"),
                     which.c_str(),
                     wxDateTime::GetWeekDayName(wday).c_str(),
                     wxDateTime::GetMonthName(month).c_str(),
                     year);

            return s;
        }
    };

    // the array data was generated by the following python program
    /*
from DateTime import *
from whrandom import *
from string import *

monthNames = [ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
wdayNames = [ 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun' ]

week = DateTimeDelta(7)

for n in range(20):
    year = randint(1900, 2100)
    month = randint(1, 12)
    day = randint(1, 28)
    dt = DateTime(year, month, day)
    wday = dt.day_of_week

    countFromEnd = choice([-1, 1])
    weekNum = 0;

    while dt.month is month:
        dt = dt - countFromEnd * week
        weekNum = weekNum + countFromEnd

    data = { 'day': rjust(`day`, 2), 'month': monthNames[month - 1], 'year': year, 'weekNum': rjust(`weekNum`, 2), 'wday': wdayNames[wday] }

    print "{ { %(day)s, wxDateTime::%(month)s, %(year)d }, %(weekNum)d, "\
          "wxDateTime::%(wday)s, wxDateTime::%(month)s, %(year)d }," % data
    */

    static const WeekDateTestData weekDatesTestData[] =
    {
        { { 20, wxDateTime::Mar, 2045, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  3, wxDateTime::Mon, wxDateTime::Mar, 2045 },
        { {  5, wxDateTime::Jun, 1985, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -4, wxDateTime::Wed, wxDateTime::Jun, 1985 },
        { { 12, wxDateTime::Nov, 1961, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -3, wxDateTime::Sun, wxDateTime::Nov, 1961 },
        { { 27, wxDateTime::Feb, 2093, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -1, wxDateTime::Fri, wxDateTime::Feb, 2093 },
        { {  4, wxDateTime::Jul, 2070, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -4, wxDateTime::Fri, wxDateTime::Jul, 2070 },
        { {  2, wxDateTime::Apr, 1906, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -5, wxDateTime::Mon, wxDateTime::Apr, 1906 },
        { { 19, wxDateTime::Jul, 2023, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -2, wxDateTime::Wed, wxDateTime::Jul, 2023 },
        { {  5, wxDateTime::May, 1958, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -4, wxDateTime::Mon, wxDateTime::May, 1958 },
        { { 11, wxDateTime::Aug, 1900, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  2, wxDateTime::Sat, wxDateTime::Aug, 1900 },
        { { 14, wxDateTime::Feb, 1945, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  2, wxDateTime::Wed, wxDateTime::Feb, 1945 },
        { { 25, wxDateTime::Jul, 1967, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -1, wxDateTime::Tue, wxDateTime::Jul, 1967 },
        { {  9, wxDateTime::May, 1916, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -4, wxDateTime::Tue, wxDateTime::May, 1916 },
        { { 20, wxDateTime::Jun, 1927, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  3, wxDateTime::Mon, wxDateTime::Jun, 1927 },
        { {  2, wxDateTime::Aug, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, wxDateTime::Wed, wxDateTime::Aug, 2000 },
        { { 20, wxDateTime::Apr, 2044, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  3, wxDateTime::Wed, wxDateTime::Apr, 2044 },
        { { 20, wxDateTime::Feb, 1932, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -2, wxDateTime::Sat, wxDateTime::Feb, 1932 },
        { { 25, wxDateTime::Jul, 2069, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  4, wxDateTime::Thu, wxDateTime::Jul, 2069 },
        { {  3, wxDateTime::Apr, 1925, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, wxDateTime::Fri, wxDateTime::Apr, 1925 },
        { { 21, wxDateTime::Mar, 2093, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  3, wxDateTime::Sat, wxDateTime::Mar, 2093 },
        { {  3, wxDateTime::Dec, 2074, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, -5, wxDateTime::Mon, wxDateTime::Dec, 2074 }
    };

    wxDateTime dt;
    for ( n = 0; n < WXSIZEOF(weekDatesTestData); n++ )
    {
        const WeekDateTestData& wd = weekDatesTestData[n];

        dt.SetToWeekDay(wd.wday, wd.nWeek, wd.month, wd.year);

        const Date& d = wd.date;
        CPPUNIT_ASSERT( d.SameDay(dt.GetTm()) );
    }
}

// test the computation of (ISO) week numbers
void DateTimeTestCase::TestTimeWNumber()
{
    struct WeekNumberTestData
    {
        Date date;                          // the date
        wxDateTime::wxDateTime_t week;      // the week number in the year
        wxDateTime::wxDateTime_t wmon;      // the week number in the month
        wxDateTime::wxDateTime_t wmon2;     // same but week starts with Sun
        wxDateTime::wxDateTime_t dnum;      // day number in the year
    };

    // data generated with the following python script:
    /*
from DateTime import *
from whrandom import *
from string import *

monthNames = [ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
wdayNames = [ 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun' ]

def GetMonthWeek(dt):
    weekNumMonth = dt.iso_week[1] - DateTime(dt.year, dt.month, 1).iso_week[1] + 1
    if weekNumMonth < 0:
        weekNumMonth = weekNumMonth + 53
    return weekNumMonth

def GetLastSundayBefore(dt):
    if dt.iso_week[2] == 7:
        return dt
    else:
        return dt - DateTimeDelta(dt.iso_week[2])

for n in range(20):
    year = randint(1900, 2100)
    month = randint(1, 12)
    day = randint(1, 28)
    dt = DateTime(year, month, day)
    dayNum = dt.day_of_year
    weekNum = dt.iso_week[1]
    weekNumMonth = GetMonthWeek(dt)

    weekNumMonth2 = 0
    dtSunday = GetLastSundayBefore(dt)

    while dtSunday >= GetLastSundayBefore(DateTime(dt.year, dt.month, 1)):
        weekNumMonth2 = weekNumMonth2 + 1
        dtSunday = dtSunday - DateTimeDelta(7)

    data = { 'day': rjust(`day`, 2), \
             'month': monthNames[month - 1], \
             'year': year, \
             'weekNum': rjust(`weekNum`, 2), \
             'weekNumMonth': weekNumMonth, \
             'weekNumMonth2': weekNumMonth2, \
             'dayNum': rjust(`dayNum`, 3) }

    print "        { { %(day)s, "\
          "wxDateTime::%(month)s, "\
          "%(year)d }, "\
          "%(weekNum)s, "\
          "%(weekNumMonth)s, "\
          "%(weekNumMonth2)s, "\
          "%(dayNum)s }," % data

    */
    static const WeekNumberTestData weekNumberTestDates[] =
    {
        { { 27, wxDateTime::Dec, 1966, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 52, 5, 5, 361 },
        { { 22, wxDateTime::Jul, 1926, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 29, 4, 4, 203 },
        { { 22, wxDateTime::Oct, 2076, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 43, 4, 4, 296 },
        { {  1, wxDateTime::Jul, 1967, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 26, 1, 1, 182 },
        { {  8, wxDateTime::Nov, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 46, 2, 2, 313 },
        { { 21, wxDateTime::Mar, 1920, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 12, 3, 4,  81 },
        { {  7, wxDateTime::Jan, 1965, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, 2, 2,   7 },
        { { 19, wxDateTime::Oct, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 42, 4, 4, 292 },
        { { 13, wxDateTime::Aug, 1955, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 32, 2, 2, 225 },
        { { 18, wxDateTime::Jul, 2087, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 29, 3, 3, 199 },
        { {  2, wxDateTime::Sep, 2028, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 35, 1, 1, 246 },
        { { 28, wxDateTime::Jul, 1945, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 30, 5, 4, 209 },
        { { 15, wxDateTime::Jun, 1901, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 24, 3, 3, 166 },
        { { 10, wxDateTime::Oct, 1939, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 41, 3, 2, 283 },
        { {  3, wxDateTime::Dec, 1965, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 48, 1, 1, 337 },
        { { 23, wxDateTime::Feb, 1940, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  8, 4, 4,  54 },
        { {  2, wxDateTime::Jan, 1987, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, 1, 1,   2 },
        { { 11, wxDateTime::Aug, 2079, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 32, 2, 2, 223 },
        { {  2, wxDateTime::Feb, 2063, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  5, 1, 1,  33 },
        { { 16, wxDateTime::Oct, 1942, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 }, 42, 3, 3, 289 },
        { { 30, wxDateTime::Dec, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, 5, 5, 364 },
        { {  2, wxDateTime::Jan, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },  1, 1, 1,   2 },
    };

    for ( size_t n = 0; n < WXSIZEOF(weekNumberTestDates); n++ )
    {
        const WeekNumberTestData& wn = weekNumberTestDates[n];
        const Date& d = wn.date;

        wxDateTime dt = d.DT();

        wxDateTime::wxDateTime_t
            week = dt.GetWeekOfYear(wxDateTime::Monday_First),
            wmon = dt.GetWeekOfMonth(wxDateTime::Monday_First),
            wmon2 = dt.GetWeekOfMonth(wxDateTime::Sunday_First),
            dnum = dt.GetDayOfYear();

        CPPUNIT_ASSERT( dnum == wn.dnum );
        CPPUNIT_ASSERT( wmon == wn.wmon );
        CPPUNIT_ASSERT( wmon2 == wn.wmon2 );
        CPPUNIT_ASSERT( week == wn.week );

        int year = d.year;
        if ( week == 1 && d.month != wxDateTime::Jan )
        {
            // this means we're in the first week of the next year
            year++;
        }

        wxDateTime
            dt2 = wxDateTime::SetToWeekOfYear(year, week, dt.GetWeekDay());
        CPPUNIT_ASSERT( dt2 == dt );
    }
}

// test DST applicability
void DateTimeTestCase::TestTimeDST()
{
    // taken from http://www.energy.ca.gov/daylightsaving.html
    static const Date datesDST[2][2004 - 1900 + 1] =
    {
        {
            { 1, wxDateTime::Apr, 1990, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 7, wxDateTime::Apr, 1991, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 5, wxDateTime::Apr, 1992, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 4, wxDateTime::Apr, 1993, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 3, wxDateTime::Apr, 1994, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 2, wxDateTime::Apr, 1995, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 7, wxDateTime::Apr, 1996, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 6, wxDateTime::Apr, 1997, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 5, wxDateTime::Apr, 1998, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 4, wxDateTime::Apr, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 2, wxDateTime::Apr, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 1, wxDateTime::Apr, 2001, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 7, wxDateTime::Apr, 2002, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 6, wxDateTime::Apr, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 4, wxDateTime::Apr, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
        },
        {
            { 28, wxDateTime::Oct, 1990, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 27, wxDateTime::Oct, 1991, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 25, wxDateTime::Oct, 1992, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 31, wxDateTime::Oct, 1993, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 30, wxDateTime::Oct, 1994, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 29, wxDateTime::Oct, 1995, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 27, wxDateTime::Oct, 1996, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 26, wxDateTime::Oct, 1997, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 25, wxDateTime::Oct, 1998, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 31, wxDateTime::Oct, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 29, wxDateTime::Oct, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 28, wxDateTime::Oct, 2001, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 27, wxDateTime::Oct, 2002, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 26, wxDateTime::Oct, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            { 31, wxDateTime::Oct, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
        }
    };

    for ( int year = 1990; year < 2005; year++ )
    {
        wxDateTime dtBegin = wxDateTime::GetBeginDST(year, wxDateTime::USA),
                   dtEnd = wxDateTime::GetEndDST(year, wxDateTime::USA);

        size_t n = year - 1990;
        const Date& dBegin = datesDST[0][n];
        const Date& dEnd = datesDST[1][n];

        CPPUNIT_ASSERT( dBegin.SameDay(dtBegin.GetTm()) );
        CPPUNIT_ASSERT( dEnd.SameDay(dtEnd.GetTm()) );
    }
}

// test wxDateTime -> text conversion
void DateTimeTestCase::TestTimeFormat()
{
    // some information may be lost during conversion, so store what kind
    // of info should we recover after a round trip
    enum CompareKind
    {
        CompareNone,        // don't try comparing
        CompareBoth,        // dates and times should be identical
        CompareYear,        // don't compare centuries (fails for 2 digit years)
        CompareDate,        // dates only
        CompareTime         // time only
    };

    static const struct
    {
        CompareKind compareKind;
        const wxChar *format;
    } formatTestFormats[] =
    {
       { CompareYear, _T("---> %c") }, // %c could use 2 digit years
       { CompareDate, _T("Date is %A, %d of %B, in year %Y") },
       { CompareYear, _T("Date is %x, time is %X") }, // %x could use 2 digits
       { CompareTime, _T("Time is %H:%M:%S or %I:%M:%S %p") },
       { CompareNone, _T("The day of year: %j, the week of year: %W") },
       { CompareDate, _T("ISO date without separators: %Y%m%d") },
    };

    static const Date formatTestDates[] =
    {
        { 29, wxDateTime::May, 1976, 18, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        { 31, wxDateTime::Dec, 1999, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1937, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1856, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1857, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        { 29, wxDateTime::May, 2076, 18, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        { 29, wxDateTime::Feb, 2400, 02, 15, 25, 0.0, wxDateTime::Inv_WeekDay },
#if 0
        // Need to add support for BCE dates.
        { 01, wxDateTime::Jan,  -52, 03, 16, 47, 0.0, wxDateTime::Inv_WeekDay },
#endif
    };

    for ( size_t d = 0; d < WXSIZEOF(formatTestDates); d++ )
    {
        wxDateTime dt = formatTestDates[d].DT();
        for ( size_t n = 0; n < WXSIZEOF(formatTestFormats); n++ )
        {
            const wxChar *fmt = formatTestFormats[n].format;
            wxString s = dt.Format(fmt);

            // what can we recover?
            CompareKind kind = formatTestFormats[n].compareKind;

            // convert back
            wxDateTime dt2;
            const wxChar *result = dt2.ParseFormat(s, fmt);
            if ( !result )
            {
                // converion failed - should it have?
                CPPUNIT_ASSERT( kind == CompareNone );
            }
            else // conversion succeeded
            {
                // should have parsed the entire string
                CPPUNIT_ASSERT( !*result );

                switch ( kind )
                {
                    case CompareYear:
                        if ( dt2.GetCentury() != dt.GetCentury() )
                        {
                            CPPUNIT_ASSERT_EQUAL(dt.GetYear() % 100,
                                                 dt2.GetYear() % 100);

                            dt2.SetYear(dt.GetYear());
                        }
                        // fall through and compare everything

                    case CompareBoth:
                        CPPUNIT_ASSERT_EQUAL( dt, dt2 );
                        break;

                    case CompareDate:
                        CPPUNIT_ASSERT( dt.IsSameDate(dt2) );
                        break;

                    case CompareTime:
                        CPPUNIT_ASSERT( dt.IsSameTime(dt2) );
                        break;

                    case CompareNone:
                        wxFAIL_MSG( _T("unexpected") );
                        break;
                }
            }
        }
    }
}

void DateTimeTestCase::TestTimeSpanFormat()
{
    static const struct TimeSpanFormatTestData
    {
        long h, min, sec, msec;
        const wxChar *fmt;
        const wxChar *result;
    } testSpans[] =
    {
        {   12, 34, 56, 789, _T("%H:%M:%S.%l"),   _T("12:34:56.789")          },
        {    1,  2,  3,   0, _T("%H:%M:%S"),      _T("01:02:03")              },
        {    1,  2,  3,   0, _T("%S"),            _T("3723")                  },
        {   -1, -2, -3,   0, _T("%S"),            _T("-3723")                 },
        {   -1, -2, -3,   0, _T("%H:%M:%S"),      _T("-01:02:03")             },
        {   26,  0,  0,   0, _T("%H"),            _T("26")                    },
        {   26,  0,  0,   0, _T("%D, %H"),        _T("1, 02")                 },
        {  -26,  0,  0,   0, _T("%H"),            _T("-26")                   },
        {  -26,  0,  0,   0, _T("%D, %H"),        _T("-1, 02")                },
        {  219,  0,  0,   0, _T("%H"),            _T("219")                   },
        {  219,  0,  0,   0, _T("%D, %H"),        _T("9, 03")                 },
        {  219,  0,  0,   0, _T("%E, %D, %H"),    _T("1, 2, 03")              },
    };

    for ( size_t n = 0; n < WXSIZEOF(testSpans); n++ )
    {
        const TimeSpanFormatTestData& td = testSpans[n];
        wxTimeSpan ts(td.h, td.min, td.sec, td.msec);
        CPPUNIT_ASSERT_EQUAL( wxString(td.result), ts.Format(td.fmt) );
    }
}

void DateTimeTestCase::TestTimeTicks()
{
    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        if ( d.ticks == -1 )
            continue;

        wxDateTime dt = d.DT();
        //RN:  Translate according to test's time zone
        //2nd param is to ignore DST - it's already factored
        //into Vadim's tests
        dt.MakeTimezone(wxDateTime::WEST, true);
        long ticks = (dt.GetValue() / 1000).ToLong();
        CPPUNIT_ASSERT( ticks == d.ticks );

        dt = d.DT().FromTimezone(wxDateTime::GMT0);
        ticks = (dt.GetValue() / 1000).ToLong();
        CPPUNIT_ASSERT( ticks == d.gmticks );
    }
}

// test parsing dates in RFC822 format
void DateTimeTestCase::TestParceRFC822()
{
    static const struct ParseTestData
    {
        const wxChar *rfc822;
        Date date;              // NB: this should be in UTC
        bool good;
    } parseTestDates[] =
    {
        {
            _T("Sat, 18 Dec 1999 00:46:40 +0100"),
            { 17, wxDateTime::Dec, 1999, 23, 46, 40, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            true
        },
        {
            _T("Wed, 1 Dec 1999 05:17:20 +0300"),
            {  1, wxDateTime::Dec, 1999, 2, 17, 20, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            true
        },
        {
            _T("Sun, 28 Aug 2005 03:31:30 +0200"),
            {  28, wxDateTime::Aug, 2005, 1, 31, 30, 0.0, wxDateTime::Inv_WeekDay, 0, 0 },
            true
        },
    };

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        wxDateTime dt;
        if ( dt.ParseRfc822Date(parseTestDates[n].rfc822) )
        {
            CPPUNIT_ASSERT( parseTestDates[n].good );

            wxDateTime dtReal = parseTestDates[n].date.DT().FromUTC();
            CPPUNIT_ASSERT_EQUAL( dtReal, dt );
        }
        else // failed to parse
        {
            CPPUNIT_ASSERT( !parseTestDates[n].good );
        }
    }
}

// test parsing dates in free format
void DateTimeTestCase::TestDateParse()
{
    static const struct ParseTestData
    {
        const wxChar *str;
        Date date;              // NB: this should be in UTC
        bool good;
    } parseTestDates[] =
    {
        { _T("21 Mar 2006"), { 21, wxDateTime::Mar, 2006 }, true },
        { _T("29 Feb 1976"), { 29, wxDateTime::Feb, 1976 }, true },
        { _T("Feb 29 1976"), { 29, wxDateTime::Feb, 1976 }, true },
        { _T("31/03/06"),    { 31, wxDateTime::Mar,    6 }, true },
        { _T("31/03/2006"),  { 31, wxDateTime::Mar, 2006 }, true },

        // some invalid ones too
        { _T("29 Feb 2006") },
        { _T("31/04/06") },
        { _T("bloordyblop") }
    };

    // special cases
    wxDateTime dt;
    CPPUNIT_ASSERT( dt.ParseDate(_T("today")) );
    CPPUNIT_ASSERT_EQUAL( wxDateTime::Today(), dt );

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        wxDateTime dt;
        if ( dt.ParseDate(parseTestDates[n].str) )
        {
            CPPUNIT_ASSERT( parseTestDates[n].good );

            CPPUNIT_ASSERT_EQUAL( parseTestDates[n].date.DT(), dt );
        }
        else // failed to parse
        {
            CPPUNIT_ASSERT( !parseTestDates[n].good );
        }
    }
}

void DateTimeTestCase::TestTimeArithmetics()
{
    static const wxDateSpan testArithmData[] =
    {
        wxDateSpan::Day(),
        wxDateSpan::Week(),
        wxDateSpan::Month(),
        wxDateSpan::Year(),
    };

    // the test will *not* work with arbitrary date!
    wxDateTime dt(2, wxDateTime::Dec, 1999),
               dt1,
               dt2;

    for ( size_t n = 0; n < WXSIZEOF(testArithmData); n++ )
    {
        const wxDateSpan& span = testArithmData[n];
        dt1 = dt + span;
        dt2 = dt - span;

        CPPUNIT_ASSERT( dt1 - span == dt );
        CPPUNIT_ASSERT( dt2 + span == dt );
        CPPUNIT_ASSERT( dt2 + 2*span == dt1 );
    }
}

void DateTimeTestCase::TestDSTBug()
{
    /////////////////////////
    // Test GetEndDST()
    wxDateTime dt = wxDateTime::GetEndDST(2004);
    CPPUNIT_ASSERT_EQUAL(31, (int)dt.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt.GetYear());
    CPPUNIT_ASSERT_EQUAL(2, (int)dt.GetHour());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMinute());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetSecond());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMillisecond());

    /////////////////////////
    // Test ResetTime()
    dt.SetHour(5);
    CPPUNIT_ASSERT_EQUAL(5, (int)dt.GetHour());
    dt.ResetTime();
    CPPUNIT_ASSERT_EQUAL(31, (int)dt.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt.GetYear());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetHour());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMinute());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetSecond());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMillisecond());

    dt.Set(1, 0, 0, 0);
    CPPUNIT_ASSERT_EQUAL(1, (int)dt.GetHour());

    /////////////////////////
    // Test Today()
#ifdef CHANGE_SYSTEM_DATE
    {
        DateChanger change(2004, 10, 31, 5, 0, 0);
        dt = wxDateTime::Today();
    }

    CPPUNIT_ASSERT_EQUAL(31, (int)dt.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt.GetYear());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetHour());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMinute());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetSecond());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMillisecond());

    /////////////////////////
    // Test Set(hour, minute, second, milli)
    wxDateTime dt2;
    {
        DateChanger change(2004, 10, 31, 5, 0, 0);
        dt.Set(1, 30, 0, 0);
        dt2.Set(5, 30, 0, 0);
    }

    CPPUNIT_ASSERT_EQUAL(31, (int)dt.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt.GetYear());
    CPPUNIT_ASSERT_EQUAL(1, (int)dt.GetHour());
    CPPUNIT_ASSERT_EQUAL(30, (int)dt.GetMinute());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetSecond());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt.GetMillisecond());

    CPPUNIT_ASSERT_EQUAL(31, (int)dt2.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt2.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt2.GetYear());
    CPPUNIT_ASSERT_EQUAL(5, (int)dt2.GetHour());
    CPPUNIT_ASSERT_EQUAL(30, (int)dt2.GetMinute());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt2.GetSecond());
    CPPUNIT_ASSERT_EQUAL(0, (int)dt2.GetMillisecond());
#endif // CHANGE_SYSTEM_DATE
}

void DateTimeTestCase::TestDateOnly()
{
    wxDateTime dt(19, wxDateTime::Jan, 2007, 15, 01, 00);

    static const wxDateTime::wxDateTime_t DATE_ZERO = 0;
    CPPUNIT_ASSERT_EQUAL( DATE_ZERO, dt.GetDateOnly().GetHour() );
    CPPUNIT_ASSERT_EQUAL( DATE_ZERO, dt.GetDateOnly().GetMinute() );
    CPPUNIT_ASSERT_EQUAL( DATE_ZERO, dt.GetDateOnly().GetSecond() );
    CPPUNIT_ASSERT_EQUAL( DATE_ZERO, dt.GetDateOnly().GetMillisecond() );

    dt.ResetTime();
    CPPUNIT_ASSERT_EQUAL( wxDateTime(19, wxDateTime::Jan, 2007), dt );

    CPPUNIT_ASSERT_EQUAL( wxDateTime::Today(), wxDateTime::Now().GetDateOnly() );
}

#endif // wxUSE_DATETIME
