///////////////////////////////////////////////////////////////////////////////
// Name:        tests/datetime/datetime.cpp
// Purpose:     wxDateTime unit test
// Author:      Vadim Zeitlin
// Created:     2004-06-23 (extracted from samples/console/console.cpp)
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_DATETIME

#ifndef WX_PRECOMP
    #include "wx/time.h"    // wxGetTimeZone()
    #include "wx/utils.h"   // wxMilliSleep()
#endif // WX_PRECOMP

#include "wx/wxcrt.h"       // for wxStrstr()

#include "wx/private/localeset.h"

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

// helper function translating week day/month names from English to the current
// locale
static wxString TranslateDate(const wxString& str)
{
    // small optimization: if there are no alphabetic characters in the string,
    // there is nothing to translate
    wxString::const_iterator i, end = str.end();
    for ( i = str.begin(); i != end; ++i )
    {
        if ( isalpha(*i) )
            break;
    }

    if ( i == end )
        return str;

    wxString trans(str);

    for ( wxDateTime::WeekDay wd = wxDateTime::Sun;
          wd < wxDateTime::Inv_WeekDay;
          wxNextWDay(wd) )
    {
        trans.Replace
              (
                wxDateTime::GetEnglishWeekDayName(wd, wxDateTime::Name_Abbr),
                wxDateTime::GetWeekDayName(wd, wxDateTime::Name_Abbr)
              );
    }

    for ( wxDateTime::Month mon = wxDateTime::Jan;
          mon < wxDateTime::Inv_Month;
          wxNextMonth(mon) )
    {
        trans.Replace
              (
                wxDateTime::GetEnglishMonthName(mon, wxDateTime::Name_Abbr),
                wxDateTime::GetMonthName(mon, wxDateTime::Name_Abbr)
              );
    }

    return trans;
}

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
    time_t gmticks;

    void Init(const wxDateTime::Tm& tm)
    {
        day = tm.mday;
        month = tm.mon;
        year = tm.year;
        hour = tm.hour;
        min = tm.min;
        sec = tm.sec;
        jdn = 0.0;
        gmticks = -1;
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
        s.Printf(wxT("%02d:%02d:%02d %10s %02d, %4d%s"),
                 hour, min, sec,
                 wxDateTime::GetMonthName(month).c_str(),
                 day,
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? wxT("AD") : wxT("BC"));
        return s;
    }

    wxString FormatDate() const
    {
        wxString s;
        s.Printf(wxT("%02d-%s-%4d%s"),
                 day,
                 wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? wxT("AD") : wxT("BC"));
        return s;
    }
};

// ----------------------------------------------------------------------------
// test data
// ----------------------------------------------------------------------------

static const Date testDates[] =
{
    {  1, wxDateTime::Jan,  1970, 00, 00, 00, 2440587.5, wxDateTime::Thu,         0 },
    {  7, wxDateTime::Feb,  2036, 00, 00, 00, 2464730.5, wxDateTime::Thu,        -1 },
    {  8, wxDateTime::Feb,  2036, 00, 00, 00, 2464731.5, wxDateTime::Fri,        -1 },
    {  1, wxDateTime::Jan,  2037, 00, 00, 00, 2465059.5, wxDateTime::Thu,        -1 },
    {  1, wxDateTime::Jan,  2038, 00, 00, 00, 2465424.5, wxDateTime::Fri,        -1 },
    { 21, wxDateTime::Jan,  2222, 00, 00, 00, 2532648.5, wxDateTime::Mon,        -1 },
    { 29, wxDateTime::May,  1976, 12, 00, 00, 2442928.0, wxDateTime::Sat, 202219200 },
    { 29, wxDateTime::Feb,  1976, 00, 00, 00, 2442837.5, wxDateTime::Sun, 194400000 },
    {  1, wxDateTime::Jan,  1900, 12, 00, 00, 2415021.0, wxDateTime::Mon,        -1 },
    {  1, wxDateTime::Jan,  1900, 00, 00, 00, 2415020.5, wxDateTime::Mon,        -1 },
    { 15, wxDateTime::Oct,  1582, 00, 00, 00, 2299160.5, wxDateTime::Fri,        -1 },
    {  4, wxDateTime::Oct,  1582, 00, 00, 00, 2299149.5, wxDateTime::Mon,        -1 },
    {  1, wxDateTime::Mar,     1, 00, 00, 00, 1721484.5, wxDateTime::Thu,        -1 },
    {  1, wxDateTime::Jan,     1, 00, 00, 00, 1721425.5, wxDateTime::Mon,        -1 },
    { 31, wxDateTime::Dec,     0, 00, 00, 00, 1721424.5, wxDateTime::Sun,        -1 },
    {  1, wxDateTime::Jan,     0, 00, 00, 00, 1721059.5, wxDateTime::Sat,        -1 },
    { 12, wxDateTime::Aug, -1234, 00, 00, 00, 1270573.5, wxDateTime::Fri,        -1 },
    { 12, wxDateTime::Aug, -4000, 00, 00, 00,  260313.5, wxDateTime::Sat,        -1 },
    { 24, wxDateTime::Nov, -4713, 00, 00, 00,      -0.5, wxDateTime::Mon,        -1 },
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
        CPPUNIT_TEST( TestTimeParse );
        CPPUNIT_TEST( TestTimeZoneParse );
        CPPUNIT_TEST( TestTimeSpanFormat );
        CPPUNIT_TEST( TestTimeTicks );
        CPPUNIT_TEST( TestParseRFC822 );
        CPPUNIT_TEST( TestDateParse );
        CPPUNIT_TEST( TestDateParseISO );
        CPPUNIT_TEST( TestDateTimeParse );
        CPPUNIT_TEST( TestDateWeekFormat );
        CPPUNIT_TEST( TestTimeArithmetics );
        CPPUNIT_TEST( TestDSTBug );
        CPPUNIT_TEST( TestDateOnly );
        CPPUNIT_TEST( TestTranslateFromUnicodeFormat );
        CPPUNIT_TEST( TestConvToFromLocalTZ );
    CPPUNIT_TEST_SUITE_END();

    void TestLeapYears();
    void TestTimeSet();
    void TestTimeJDN();
    void TestTimeWNumber();
    void TestTimeWDays();
    void TestTimeDST();
    void TestTimeFormat();
    void TestTimeParse();
    void TestTimeZoneParse();
    void TestTimeSpanFormat();
    void TestTimeTicks();
    void TestParseRFC822();
    void TestDateParse();
    void TestDateParseISO();
    void TestDateTimeParse();
    void TestDateWeekFormat();
    void TestTimeArithmetics();
    void TestDSTBug();
    void TestDateOnly();
    void TestTranslateFromUnicodeFormat();
    void TestConvToFromLocalTZ();

    wxDECLARE_NO_COPY_CLASS(DateTimeTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(DateTimeTestCase, "[datetime]");

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

        CPPUNIT_ASSERT_EQUAL( y.isLeap, wxDateTime::IsLeapYear(y.year) );
    }
}

// test constructing wxDateTime objects
void DateTimeTestCase::TestTimeSet()
{
    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d1 = testDates[n];
        const wxDateTime dt = d1.DT();

        Date d2;
        d2.Init(dt.GetTm());

        INFO("n=" << n);
        CHECK( d1.Format() == d2.Format() );
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

        INFO("n=" << n);
        CHECK( d.jdn == jdn );

        dt.Set(jdn);
        CHECK( jdn == dt.GetJulianDayNumber() );
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

        INFO("n=" << n);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        CHECK( d.wday == wday );
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
                case 1: which = wxT("first"); break;
                case 2: which = wxT("second"); break;
                case 3: which = wxT("third"); break;
                case 4: which = wxT("fourth"); break;
                case 5: which = wxT("fifth"); break;

                case -1: which = wxT("last"); break;
            }

            if ( nWeek < -1 )
            {
                which += wxT(" from end");
            }

            s.Printf(wxT("The %s %s of %s in %d"),
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
        { { 20, wxDateTime::Mar, 2045, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  3, wxDateTime::Mon, wxDateTime::Mar, 2045 },
        { {  5, wxDateTime::Jun, 1985, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -4, wxDateTime::Wed, wxDateTime::Jun, 1985 },
        { { 12, wxDateTime::Nov, 1961, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -3, wxDateTime::Sun, wxDateTime::Nov, 1961 },
        { { 27, wxDateTime::Feb, 2093, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -1, wxDateTime::Fri, wxDateTime::Feb, 2093 },
        { {  4, wxDateTime::Jul, 2070, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -4, wxDateTime::Fri, wxDateTime::Jul, 2070 },
        { {  2, wxDateTime::Apr, 1906, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -5, wxDateTime::Mon, wxDateTime::Apr, 1906 },
        { { 19, wxDateTime::Jul, 2023, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -2, wxDateTime::Wed, wxDateTime::Jul, 2023 },
        { {  5, wxDateTime::May, 1958, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -4, wxDateTime::Mon, wxDateTime::May, 1958 },
        { { 11, wxDateTime::Aug, 1900, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  2, wxDateTime::Sat, wxDateTime::Aug, 1900 },
        { { 14, wxDateTime::Feb, 1945, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  2, wxDateTime::Wed, wxDateTime::Feb, 1945 },
        { { 25, wxDateTime::Jul, 1967, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -1, wxDateTime::Tue, wxDateTime::Jul, 1967 },
        { {  9, wxDateTime::May, 1916, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -4, wxDateTime::Tue, wxDateTime::May, 1916 },
        { { 20, wxDateTime::Jun, 1927, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  3, wxDateTime::Mon, wxDateTime::Jun, 1927 },
        { {  2, wxDateTime::Aug, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, wxDateTime::Wed, wxDateTime::Aug, 2000 },
        { { 20, wxDateTime::Apr, 2044, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  3, wxDateTime::Wed, wxDateTime::Apr, 2044 },
        { { 20, wxDateTime::Feb, 1932, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -2, wxDateTime::Sat, wxDateTime::Feb, 1932 },
        { { 25, wxDateTime::Jul, 2069, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  4, wxDateTime::Thu, wxDateTime::Jul, 2069 },
        { {  3, wxDateTime::Apr, 1925, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, wxDateTime::Fri, wxDateTime::Apr, 1925 },
        { { 21, wxDateTime::Mar, 2093, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  3, wxDateTime::Sat, wxDateTime::Mar, 2093 },
        { {  3, wxDateTime::Dec, 2074, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, -5, wxDateTime::Mon, wxDateTime::Dec, 2074 }
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
        { { 27, wxDateTime::Dec, 1966, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 52, 5, 5, 361 },
        { { 22, wxDateTime::Jul, 1926, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 29, 4, 4, 203 },
        { { 22, wxDateTime::Oct, 2076, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 43, 4, 4, 296 },
        { {  1, wxDateTime::Jul, 1967, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 26, 1, 1, 182 },
        { {  8, wxDateTime::Nov, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 46, 2, 2, 313 },
        { { 21, wxDateTime::Mar, 1920, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 12, 3, 4,  81 },
        { {  7, wxDateTime::Jan, 1965, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 2, 2,   7 },
        { { 19, wxDateTime::Oct, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 42, 4, 4, 292 },
        { { 13, wxDateTime::Aug, 1955, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 32, 2, 2, 225 },
        { { 18, wxDateTime::Jul, 2087, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 29, 3, 3, 199 },
        { {  2, wxDateTime::Sep, 2028, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 35, 1, 1, 246 },
        { { 28, wxDateTime::Jul, 1945, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 30, 5, 4, 209 },
        { { 15, wxDateTime::Jun, 1901, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 24, 3, 3, 166 },
        { { 10, wxDateTime::Oct, 1939, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 41, 3, 2, 283 },
        { {  3, wxDateTime::Dec, 1965, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 48, 1, 1, 337 },
        { { 23, wxDateTime::Feb, 1940, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  8, 4, 4,  54 },
        { {  2, wxDateTime::Jan, 1987, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 1, 1,   2 },
        { { 11, wxDateTime::Aug, 2079, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 32, 2, 2, 223 },
        { {  2, wxDateTime::Feb, 2063, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  5, 1, 1,  33 },
        { { 16, wxDateTime::Oct, 1942, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 42, 3, 3, 289 },
        { { 30, wxDateTime::Dec, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 5, 5, 364 },
        { {  2, wxDateTime::Jan, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 1, 1,   2 },
        { {  5, wxDateTime::Jan, 2010, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 2, 2,   5 },
        { {  3, wxDateTime::Jan, 2011, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 2, 2,   3 },
        { { 31, wxDateTime::Dec, 2009, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 53, 5, 5, 365 },
        { { 31, wxDateTime::Dec, 2012, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 6, 6, 366 },
        { { 29, wxDateTime::Dec, 2013, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 }, 52, 5, 5, 363 },
        { { 30, wxDateTime::Dec, 2013, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 6, 5, 364 },
        { { 31, wxDateTime::Dec, 2013, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },  1, 6, 5, 365 },
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

        WX_ASSERT_EQUAL_MESSAGE( ("day of year for %s", d.Format()),
                                 wn.dnum, dnum );
        WX_ASSERT_EQUAL_MESSAGE( ("week of month (Monday) for %s", d.Format()),
                                 wn.wmon, wmon );
        WX_ASSERT_EQUAL_MESSAGE( ("week of month (Sunday) for %s", d.Format()),
                                 wn.wmon2, wmon2 );
        WX_ASSERT_EQUAL_MESSAGE( ("week of year for %s", d.Format()),
                                 wn.week, week );

        int year = d.year;
        if ( week == 1 && d.month != wxDateTime::Jan )
        {
            // this means we're in the first week of the next year
            year++;
        }

        wxDateTime
            dt2 = wxDateTime::SetToWeekOfYear(year, week, dt.GetWeekDay());
        CPPUNIT_ASSERT_EQUAL( dt, dt2 );
    }
}

// test DST applicability
void DateTimeTestCase::TestTimeDST()
{
    // taken from http://www.energy.ca.gov/daylightsaving.html
    static const Date datesDST[2][2009 - 1990 + 1] =
    {
        {
            { 1, wxDateTime::Apr, 1990, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 7, wxDateTime::Apr, 1991, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 5, wxDateTime::Apr, 1992, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 4, wxDateTime::Apr, 1993, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 3, wxDateTime::Apr, 1994, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 2, wxDateTime::Apr, 1995, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 7, wxDateTime::Apr, 1996, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 6, wxDateTime::Apr, 1997, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 5, wxDateTime::Apr, 1998, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 4, wxDateTime::Apr, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 2, wxDateTime::Apr, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 1, wxDateTime::Apr, 2001, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 7, wxDateTime::Apr, 2002, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 6, wxDateTime::Apr, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 4, wxDateTime::Apr, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 3, wxDateTime::Apr, 2005, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 2, wxDateTime::Apr, 2006, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            {11, wxDateTime::Mar, 2007, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 9, wxDateTime::Mar, 2008, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 8, wxDateTime::Mar, 2009, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
        },
        {
            { 28, wxDateTime::Oct, 1990, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 27, wxDateTime::Oct, 1991, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 25, wxDateTime::Oct, 1992, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 31, wxDateTime::Oct, 1993, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 30, wxDateTime::Oct, 1994, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 29, wxDateTime::Oct, 1995, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 27, wxDateTime::Oct, 1996, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 26, wxDateTime::Oct, 1997, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 25, wxDateTime::Oct, 1998, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 31, wxDateTime::Oct, 1999, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 29, wxDateTime::Oct, 2000, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 28, wxDateTime::Oct, 2001, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 27, wxDateTime::Oct, 2002, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 26, wxDateTime::Oct, 2003, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 31, wxDateTime::Oct, 2004, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 30, wxDateTime::Oct, 2005, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            { 29, wxDateTime::Oct, 2006, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            {  4, wxDateTime::Nov, 2007, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            {  2, wxDateTime::Nov, 2008, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },
            {  1, wxDateTime::Nov, 2009, 0, 0, 0, 0.0, wxDateTime::Inv_WeekDay, 0 },

        }
    };

    for ( size_t n = 0; n < WXSIZEOF(datesDST[0]); n++ )
    {
        const int year = 1990 + n;
        wxDateTime dtBegin = wxDateTime::GetBeginDST(year, wxDateTime::USA),
                   dtEnd = wxDateTime::GetEndDST(year, wxDateTime::USA);

        const Date& dBegin = datesDST[0][n];
        const Date& dEnd = datesDST[1][n];

        CPPUNIT_ASSERT_EQUAL( dBegin.DT().FormatDate(), dtBegin.FormatDate() );
        CPPUNIT_ASSERT_EQUAL( dEnd.DT().FormatDate(), dtEnd.FormatDate() );
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

    const char* const compareKindStrings[] =
    {
        "nothing",
        "both date and time",
        "both date and time but without century",
        "only dates",
        "only times",
    };

    static const struct
    {
        CompareKind compareKind;
        const char *format;
    } formatTestFormats[] =
    {
       { CompareYear, "---> %c" }, // %c could use 2 digit years
       { CompareDate, "Date is %A, %d of %B, in year %Y" },
       { CompareYear, "Date is %x, time is %X" }, // %x could use 2 digits
       { CompareTime, "Time is %H:%M:%S or %I:%M:%S %p" },
       { CompareNone, "The day of year: %j, the week of year: %W" },
       { CompareDate, "ISO date without separators: %Y%m%d" },
       { CompareBoth, "RFC 2822 string: %Y-%m-%d %H:%M:%S.%l %z" },

    };

    const long timeZonesOffsets[] =
    {
        -1, // This is pseudo-offset used for local time zone

        // Fictitious TimeZone offsets to ensure time zone formating and
        // interpretation works
        -(3600 + 2*60),
        3*3600 + 30*60
    };

    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const Date formatTestDates[] =
    {
        { 29, wxDateTime::May, 1976, 18, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        { 31, wxDateTime::Dec, 1999, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1937, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1856, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        {  6, wxDateTime::Feb, 1857, 23, 30, 00, 0.0, wxDateTime::Inv_WeekDay },
        { 29, wxDateTime::May, 2076, 18, 30, 00, 0.0, wxDateTime::Inv_WeekDay },

        // FIXME: the test with 02:15:25 time doesn't pass because of DST
        //        computation problems, we get back 03:15:25
        { 29, wxDateTime::Feb, 2400, 04, 15, 25, 0.0, wxDateTime::Inv_WeekDay },
#if 0
        // Need to add support for BCE dates.
        { 01, wxDateTime::Jan,  -52, 03, 16, 47, 0.0, wxDateTime::Inv_WeekDay },
#endif
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    for ( unsigned idxtz = 0; idxtz < WXSIZEOF(timeZonesOffsets); ++idxtz )
    {
        wxDateTime::TimeZone tz(timeZonesOffsets[idxtz]);
        const bool isLocalTz = tz.IsLocal();

        for ( size_t d = 0; d < WXSIZEOF(formatTestDates); d++ )
        {
            wxDateTime dt = formatTestDates[d].DT();
            for ( unsigned n = 0; n < WXSIZEOF(formatTestFormats); n++ )
            {
                const char *fmt = formatTestFormats[n].format;

                // skip the check with %p for those locales which have empty AM/PM strings:
                // for those locales it's impossible to pass the test with %p...
                wxString am, pm;
                wxDateTime::GetAmPmStrings(&am, &pm);
                if (am.empty() && pm.empty() && wxStrstr(fmt, "%p") != nullptr)
                    continue;

                // what can we recover?
                CompareKind kind = formatTestFormats[n].compareKind;

                // When using a different time zone we must perform a time zone
                // conversion below which doesn't always work correctly, check
                // for the cases when it doesn't.
                if ( !isLocalTz )
                {
                    // DST computation doesn't work correctly for dates above
                    // 2038 currently on the systems with 32 bit time_t.
                    if ( dt.GetYear() >= 2038 )
                        continue;

                    // We can't compare just dates nor just times when doing TZ
                    // conversion as both are affected by the DST: for the
                    // dates, the DST can switch midnight to 23:00 of the
                    // previous day while for the times DST can be different
                    // for the original date and today.
                    if ( kind == CompareDate || kind == CompareTime )
                        continue;
                }

                // do convert date to string
                wxString s = dt.Format(fmt, tz);

                // Normally, passing time zone to Format() should have exactly
                // the same effect as converting to this time zone before
                // calling it, however the former may use standard library date
                // handling in strftime() implementation while the latter
                // always uses our own code and they may disagree if the offset
                // for this time zone has changed since the given date, as the
                // standard library handles it correctly (at least under Unix),
                // while our code doesn't handle time zone changes at all.
                //
                // Short of implementing full support for time zone database,
                // we can't really do anything about this other than skipping
                // the test in this case.
                if ( s != dt.ToTimezone(tz).Format(fmt) )
                    continue;

                INFO("Test #" << n << " using format \"" << fmt << "\""
                     << ", format result=\"" << s << "\"");

                // convert back
                wxDateTime dt2;
                const char *result = dt2.ParseFormat(s, fmt);
                if ( !result )
                {
                    // conversion failed - should it have?
                    if ( kind != CompareNone )
                    {
                        FAIL_CHECK("Parsing formatted string failed");
                    }
                }
                else // conversion succeeded
                {
                    // currently ParseFormat() doesn't support "%Z" and so is
                    // incapable of parsing time zone part used at the end of date
                    // representations in many (but not "C") locales, compensate
                    // for it ourselves by simply consuming and ignoring it
                    while ( *result && (*result >= 'A' && *result <= 'Z') )
                        result++;

                    if ( *result )
                    {
                        INFO("Left unparsed: \"" << result << "\"");
                        FAIL_CHECK("Parsing didn't consume the entire string");
                        continue;
                    }

                    // Without "%z" we can't recover the time zone used in the
                    // call to Format() so we need to call MakeFromTimezone()
                    // explicitly.
                    if ( !strstr(fmt, "%z") && !isLocalTz )
                        dt2.MakeFromTimezone(tz);

                    INFO("Comparing " << compareKindStrings[kind] << " for "
                            << dt << " with " << dt2
                            << " (format result=\"" << s << "\")");

                    switch ( kind )
                    {
                        case CompareYear:
                            if ( dt2.GetCentury() != dt.GetCentury() )
                            {
                                CHECK( dt.GetYear() % 100 == dt2.GetYear() % 100);

                                dt2.SetYear(dt.GetYear());
                            }
                            // fall through and compare everything
                            wxFALLTHROUGH;

                        case CompareBoth:
                            CHECK( dt == dt2 );
                            break;

                        case CompareDate:
                            CHECK( dt.IsSameDate(dt2) );
                            break;

                        case CompareTime:
                            CHECK( dt.IsSameTime(dt2) );
                            break;

                        case CompareNone:
                            wxFAIL_MSG( wxT("unexpected") );
                            break;
                    }
                }
            }
        }
    }

    CPPUNIT_ASSERT(wxDateTime::Now().Format("%%") == "%");

    wxDateTime dt;

#if 0
    // special case which was known to fail
    CPPUNIT_ASSERT( dt.ParseFormat("02/06/1856", "%x") );
    CPPUNIT_ASSERT_EQUAL( 1856, dt.GetYear() );
#endif

    // also test %l separately
    CPPUNIT_ASSERT( dt.ParseFormat("12:23:45.678", "%H:%M:%S.%l") );
    CPPUNIT_ASSERT_EQUAL( 678, dt.GetMillisecond() );

    // test special case of %l matching 0 milliseconds
    CPPUNIT_ASSERT( dt.ParseFormat("12:23:45.000", "%H:%M:%S.%l") );
    CPPUNIT_ASSERT_EQUAL( 0, dt.GetMillisecond() );

    // test another format modifier not tested above.
    CPPUNIT_ASSERT( dt.ParseFormat("23", "%e") );
    CPPUNIT_ASSERT_EQUAL( 23, dt.GetDay() );

    // test partially specified dates too
    wxDateTime dtDef(26, wxDateTime::Sep, 2008);
    CPPUNIT_ASSERT( dt.ParseFormat("17", "%d", dtDef) );
    CPPUNIT_ASSERT_EQUAL( 17, dt.GetDay() );
    CPPUNIT_ASSERT_EQUAL( wxDateTime::Sep, dt.GetMonth() );
    CPPUNIT_ASSERT_EQUAL( 2008, dt.GetYear() );

    // test some degenerate cases
    CPPUNIT_ASSERT( !dt.ParseFormat("", "%z") );
    CPPUNIT_ASSERT( !dt.ParseFormat("", "%%") );

    // test compilation of some calls which should compile (and not result in
    // ambiguity because of char*<->wxCStrData<->wxString conversions)
    wxString s("foo");
    CPPUNIT_ASSERT( !dt.ParseFormat("foo") );
    CPPUNIT_ASSERT( !dt.ParseFormat(wxT("foo")) );
    CPPUNIT_ASSERT( !dt.ParseFormat(s) );
    dt.ParseFormat(s.c_str()); // Simply test compilation of this one.

    CPPUNIT_ASSERT( !dt.ParseFormat("foo", "%c") );
    CPPUNIT_ASSERT( !dt.ParseFormat(wxT("foo"), "%c") );
    CPPUNIT_ASSERT( !dt.ParseFormat(s, "%c") );
    dt.ParseFormat(s.c_str(), "%c");

    CPPUNIT_ASSERT( !dt.ParseFormat("foo", wxT("%c")) );
    CPPUNIT_ASSERT( !dt.ParseFormat(wxT("foo"), wxT("%c")) );
    CPPUNIT_ASSERT( !dt.ParseFormat(s, "%c") );
    dt.ParseFormat(s.c_str(), wxT("%c"));

    wxString spec("%c");
    CPPUNIT_ASSERT( !dt.ParseFormat("foo", spec) );
    CPPUNIT_ASSERT( !dt.ParseFormat(wxT("foo"), spec) );
    CPPUNIT_ASSERT( !dt.ParseFormat(s, spec) );
    dt.ParseFormat(s.c_str(), spec);
}

// Test parsing time in free format.
void DateTimeTestCase::TestTimeParse()
{
    wxDateTime dt;

    // Parsing standard formats should work.
    const char* end = dt.ParseTime("12:34:56");
    CPPUNIT_ASSERT( end );
    if ( end )
    {
        CPPUNIT_ASSERT_EQUAL( "12:34:56", dt.FormatISOTime() );
        CPPUNIT_ASSERT_EQUAL( "", wxString(end) );
    }
    dt.ResetTime();

    // Valid, but followed by something.
    end = dt.ParseTime("12:34:56 0123 something");
    CPPUNIT_ASSERT( end );
    if ( end )
    {
        CPPUNIT_ASSERT_EQUAL( "12:34:56", dt.FormatISOTime() );
        CPPUNIT_ASSERT_EQUAL( " 0123 something", wxString(end) );
    }
    dt.ResetTime();

    // Parsing just hours should work too.
    end = dt.ParseTime("17");
    CPPUNIT_ASSERT( end );
    if ( end )
    {
        CPPUNIT_ASSERT_EQUAL( "17:00:00", dt.FormatISOTime() );
        CPPUNIT_ASSERT_EQUAL( "", wxString(end) );
    }

    // Parsing gibberish shouldn't work.
    CPPUNIT_ASSERT( !dt.ParseTime("bloordyblop") );
}

void DateTimeTestCase::TestTimeZoneParse()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct
    {
        const char *str;
        bool good;
    } parseTestTimeZones[] =
    {
        // All of the good entries should result in 13:37 UTC+0.
        { "09:37-0400", true },
        { "13:37+0000", true },
        { "17:37+0400", true },

        // Z as UTC designator.
        { "13:37Z", true },

        // Only containing HH offset.
        { "09:37-04", true },
        { "13:37+00", true },
        { "17:37+04", true },

        // Colon as HH and MM separator.
        { "17:37+04:00", true },

        // // Colon separator and non-zero MM.
        { "09:07-04:30", true },
        { "19:22+05:45", true },

        // Containing minus sign (U+2212) as separator between time and tz.
        { "09:37" "\xe2\x88\x92" "0400", true },

        // Some invalid ones too.

        { "00:00-1600" }, // Offset out of range.
        { "00:00+1600" }, // Offset out of range.

        { "00:00+00:" }, // Minutes missing after colon separator.

        // Not exactly 2 digits for hours and minutes.
        { "17:37+4" },
        { "17:37+400" },
        { "17:37+040" },
        { "17:37+4:0" },
        { "17:37+4:00" },
        { "17:37+04:0" },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    for ( size_t n = 0; n < WXSIZEOF(parseTestTimeZones); ++n )
    {
        wxDateTime dt;
        wxString sTimeZone = wxString::FromUTF8(parseTestTimeZones[n].str);
        wxString::const_iterator end;
        if ( dt.ParseFormat(sTimeZone, wxS("%H:%M%z"), &end)
             && end == sTimeZone.end() )
        {
            CPPUNIT_ASSERT( parseTestTimeZones[n].good );
            CPPUNIT_ASSERT_EQUAL( 13, dt.GetHour(wxDateTime::UTC));
            CPPUNIT_ASSERT_EQUAL( 37, dt.GetMinute(wxDateTime::UTC));
        }
        else
        {
            // Failed to parse time zone.
            CPPUNIT_ASSERT( !parseTestTimeZones[n].good );
        }
    }
}

void DateTimeTestCase::TestTimeSpanFormat()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct TimeSpanFormatTestData
    {
        long h, min, sec, msec;
        const char *fmt;
        const char *result;
    } testSpans[] =
    {
        {   12, 34, 56, 789, "%H:%M:%S.%l",   "12:34:56.789"          },
        {    1,  2,  3,   0, "%H:%M:%S",      "01:02:03"              },
        {    1,  2,  3,   0, "%S",            "3723"                  },
        {   -1, -2, -3,   0, "%S",            "-3723"                 },
        {   -1, -2, -3,   0, "%H:%M:%S",      "-01:02:03"             },
        {   26,  0,  0,   0, "%H",            "26"                    },
        {   26,  0,  0,   0, "%D, %H",        "1, 02"                 },
        {  -26,  0,  0,   0, "%H",            "-26"                   },
        {  -26,  0,  0,   0, "%D, %H",        "-1, 02"                },
        {  219,  0,  0,   0, "%H",            "219"                   },
        {  219,  0,  0,   0, "%D, %H",        "9, 03"                 },
        {  219,  0,  0,   0, "%E, %D, %H",    "1, 2, 03"              },
        {    0, -1,  0,   0, "%H:%M:%S",      "-00:01:00"             },
        {    0,  0, -1,   0, "%H:%M:%S",      "-00:00:01"             },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    for ( size_t n = 0; n < WXSIZEOF(testSpans); n++ )
    {
        const TimeSpanFormatTestData& td = testSpans[n];
        wxTimeSpan ts(td.h, td.min, td.sec, td.msec);
        CPPUNIT_ASSERT_EQUAL( td.result, ts.Format(td.fmt) );
    }
}

void DateTimeTestCase::TestTimeTicks()
{
    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        if ( d.gmticks == -1 )
            continue;

        const wxDateTime dt = d.DT().FromTimezone(wxDateTime::UTC);

        INFO("n=" << n);

        time_t ticks = (dt.GetValue() / 1000).ToLong();
        CHECK( d.gmticks == ticks );
    }
}

// test parsing dates in RFC822 format
void DateTimeTestCase::TestParseRFC822()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct ParseTestData
    {
        const char *rfc822;
        Date date;              // NB: this should be in UTC
        bool good;
    } parseTestDates[] =
    {
        {
            "Sat, 18 Dec 1999 00:46:40 +0100",
            { 17, wxDateTime::Dec, 1999, 23, 46, 40 },
            true
        },
        {
            "Wed, 1 Dec 1999 05:17:20 +0300",
            {  1, wxDateTime::Dec, 1999, 2, 17, 20 },
            true
        },
        {
            "Sun, 28 Aug 2005 03:31:30 +0200",
            { 28, wxDateTime::Aug, 2005, 1, 31, 30 },
            true
        },

        {
            "Sat, 18 Dec 1999 10:48:30 -0500",
            { 18, wxDateTime::Dec, 1999, 15, 48, 30 },
            true
        },

        {
            "Sat, 18 Dec 1999 10:48:30 G", // military time zone
            { 18, wxDateTime::Dec, 1999, 17, 48, 30 },
            true
        },

        {
            "Sat, 18 Dec 1999 10:48:30 Q", // military time zone
            { 18, wxDateTime::Dec, 1999,  6, 48, 30 },
            true
        },

        // seconds are optional according to the RFC
        {
            "Sun, 01 Jun 2008 16:30 +0200",
            {  1, wxDateTime::Jun, 2008, 14, 30, 00 },
            true
        },

        // day of week is optional according to the RFC
        {
            "18 Dec 1999 10:48:30 -0500",
            { 18, wxDateTime::Dec, 1999, 15, 48, 30 },
            true
        },

        // 2-digit year is accepted by the RFC822
        {
            "Sat, 18 Dec 99 10:48:30 -0500",
            { 18, wxDateTime::Dec, 1999, 15, 48, 30 },
            true
        },

        // years 00..29 are considered to mean 20xx
        {
            "Tue, 18 Dec 29 10:48:30 -0500",
            { 18, wxDateTime::Dec, 2029, 15, 48, 30 },
            true
        },

        // try some bogus ones too
        {
            "Sun, 01 Jun 2008 16:30: +0200",
            { 0 },
            false
        },

        {
            "Sun, 01 Und 2008 16:30:10 +0200", // month: Undecimber
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:3:10 +0200", // missing digit
            { 0 },
            false
        },

        {
            "Sun 01 Jun 2008 16:39:10 +0200", // missing comma
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:39:10 +020", // truncated time zone
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:39:10 +02", // truncated time zone
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:39:10 +0", // truncated time zone
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:39:10 +", // truncated time zone
            { 0 },
            false
        },

        {
            "Sun, 01 Jun 2008 16:39:10 GM", // truncated time zone
            { 0 },
            false
        },

        {
            "Sat, 18 Dec 1999 10:48:30", // TZ missing
            { 0 },
            false
        },

        {
            "Sat, 18 Dec 1999", // time missing
            { 0 },
            false
        },

        {
            "Sat, 18 Dec 2", // most of year missing
            { 0 },
            false
        },

        {
            "Sun,", // missing date and time
            { 0 },
            false
        },

        {
            "", // empty input
            { 0 },
            false
        },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    for ( unsigned n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const char * const datestr = parseTestDates[n].rfc822;

        wxDateTime dt;
        if ( dt.ParseRfc822Date(datestr) )
        {
            WX_ASSERT_MESSAGE(
                ("Erroneously parsed \"%s\"", datestr),
                parseTestDates[n].good
            );

            wxDateTime dtReal = parseTestDates[n].date.DT().FromUTC();
            CPPUNIT_ASSERT_EQUAL( dtReal, dt );
        }
        else // failed to parse
        {
            WX_ASSERT_MESSAGE(
                ("Failed to parse \"%s\"", datestr),
                !parseTestDates[n].good
            );
        }
    }
}

// test parsing dates in free format
void DateTimeTestCase::TestDateParse()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct ParseTestData
    {
        const char *str;
        Date date;              // NB: this should be in UTC
        bool good;
        const char* beyondEnd;  // what remains unprocessed of the input
    } parseTestDates[] =
    {
        { "21 Mar 2006", { 21, wxDateTime::Mar, 2006 }, true, ""},
        { "29 Feb 1976", { 29, wxDateTime::Feb, 1976 }, true, "" },
        { "Feb 29 1976", { 29, wxDateTime::Feb, 1976 }, true, "" },
        { "31/03/06",    { 31, wxDateTime::Mar,    6 }, true, "" },
        { "31/03/2006",  { 31, wxDateTime::Mar, 2006 }, true, "" },
        { "Thu 20 Jun 2019", { 20, wxDateTime::Jun, 2019 }, true, "" },
        { "20 Jun 2019 Thu", { 20, wxDateTime::Jun, 2019 }, true, "" },
        { "Dec sixth 2017",  {  6, wxDateTime::Dec, 2017 }, true, "" },

        // valid, but followed by something
        { "Dec 31 1979 was the end of 70s",
             { 31, wxDateTime::Dec, 1979 }, true, " was the end of 70s" },

        // some invalid ones too
        { "29 Feb 2006" },
        { "31/04/06" },
        { "Sat 20 Jun 2019" }, // it was not a Saturday
        { "20 Jun 2019 Sat" }, // it was not a Saturday
        { "bloordyblop" },
        { "2 .  .    " },
        { "14:30:15" },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    // special cases
    wxDateTime dt;
    CPPUNIT_ASSERT( dt.ParseDate(wxT("today")) );
    CPPUNIT_ASSERT_EQUAL( wxDateTime::Today(), dt );

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const wxString datestr = TranslateDate(parseTestDates[n].str);

        const char * const end = dt.ParseDate(datestr);
        if ( end )
        {
            WX_ASSERT_MESSAGE(
                ("Erroneously parsed \"%s\"", datestr),
                parseTestDates[n].good
            );

            CPPUNIT_ASSERT_EQUAL( parseTestDates[n].date.DT(), dt );
            CPPUNIT_ASSERT_EQUAL( wxString(parseTestDates[n].beyondEnd), wxString(end) );
        }
        else // failed to parse
        {
            WX_ASSERT_MESSAGE(
                ("Failed to parse \"%s\"", datestr),
                !parseTestDates[n].good
            );
        }
    }

    // Check that incomplete parse works correctly.
    const char* p = dt.ParseFormat("2012-03-23 12:34:56", "%Y-%m-%d");
    CPPUNIT_ASSERT_EQUAL( " 12:34:56", wxString(p) );
}

void DateTimeTestCase::TestDateParseISO()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct
    {
        const char *str;
        Date date;              // NB: this should be in UTC
        bool good;
    } parseTestDates[] =
    {
        { "2006-03-21", { 21, wxDateTime::Mar, 2006 }, true },
        { "1976-02-29", { 29, wxDateTime::Feb, 1976 }, true },
        { "0006-03-31", { 31, wxDateTime::Mar,    6 }, true },

        // some invalid ones too
        { "2006:03:31" },
        { "31/04/06" },
        { "bloordyblop" },
        { "" },
    };

    static const struct
    {
        const char *str;
        wxDateTime::wxDateTime_t hour, min, sec;
        bool good;
    } parseTestTimes[] =
    {
        { "13:42:17", 13, 42, 17, true },
        { "02:17:01",  2, 17,  1, true },

        // some invalid ones too
        { "66:03:31" },
        { "31/04/06" },
        { "bloordyblop" },
        { "" },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        wxDateTime dt;
        if ( dt.ParseISODate(parseTestDates[n].str) )
        {
            CPPUNIT_ASSERT( parseTestDates[n].good );

            CPPUNIT_ASSERT_EQUAL( parseTestDates[n].date.DT(), dt );

            for ( size_t m = 0; m < WXSIZEOF(parseTestTimes); m++ )
            {
                wxString dtCombined;
                dtCombined << parseTestDates[n].str
                           << 'T'
                           << parseTestTimes[m].str;

                if ( dt.ParseISOCombined(dtCombined) )
                {
                    CPPUNIT_ASSERT( parseTestTimes[m].good );

                    CPPUNIT_ASSERT_EQUAL( parseTestTimes[m].hour, dt.GetHour()) ;
                    CPPUNIT_ASSERT_EQUAL( parseTestTimes[m].min, dt.GetMinute()) ;
                    CPPUNIT_ASSERT_EQUAL( parseTestTimes[m].sec, dt.GetSecond()) ;
                }
                else // failed to parse combined date/time
                {
                    CPPUNIT_ASSERT( !parseTestTimes[m].good );
                }
            }
        }
        else // failed to parse
        {
            CPPUNIT_ASSERT( !parseTestDates[n].good );
        }
    }
}

void DateTimeTestCase::TestDateTimeParse()
{
    wxGCC_WARNING_SUPPRESS(missing-field-initializers)

    static const struct ParseTestData
    {
        const char *str;
        Date date;      // either local time or UTC
        bool good;
        const char *beyondEnd;  // what remains unprocessed of the input
        bool dateIsUTC; // true when timezone is specified
    } parseTestDates[] =
    {
        {
            "Thu 22 Nov 2007 07:40:00 PM",
            { 22, wxDateTime::Nov, 2007, 19, 40,  0 },
            true,
            "",
            false
        },

        {
            "2010-01-04 14:30",
            {  4, wxDateTime::Jan, 2010, 14, 30,  0 },
            true,
            "",
            false
        },

        {
            // with 'T' separator
            "2010-01-04T14:30",
            {  4, wxDateTime::Jan, 2010, 14, 30,  0 },
            true,
            "",
            false
        },

        {
            // date after time
            "14:30:00 2020-01-04",
            {  4, wxDateTime::Jan, 2020, 14, 30,  0 },
            true,
            "",
            false
        },

        {
            "bloordyblop",
            {  1, wxDateTime::Jan, 9999,  0,  0,  0},
            false,
            "bloordyblop",
            false
        },

        {
            "2022-03-09 19:12:05 and some text after space",
            {  9, wxDateTime::Mar, 2022,  19,  12,  5, -1 },
            true,
            " and some text after space",
            false
        },

        {
            "2022-03-09 19:12:05 ", // just a trailing space
            {  9, wxDateTime::Mar, 2022,  19,  12,  5, -1 },
            true,
            " ",
            false
        },

        // something other than a space right after time
        {
            "2022-03-09 19:12:05AAaaaa",
            {  9, wxDateTime::Mar, 2022,  19,  12,  5, -1 },
            true,
            "AAaaaa",
            false
        },

        // the rest have a time zone specified, and when the
        // time zone is valid, the date to compare to is in UTC
        {
            "2012-01-01 10:12:05 +0100",
            {  1, wxDateTime::Jan, 2012,   9,  12,  5, -1 },
            true,
            "",
            true
        },

        {
            "2022-03-09 19:12:05 -0700",
            { 10, wxDateTime::Mar, 2022,   2,  12,  5, -1 },
            true,
            "",
            true
        },

        {
            "2022-03-09 19:12:05 +0615",
            {  9, wxDateTime::Mar, 2022,  12,  57,  5, -1 },
            true,
            "",
            true
        },

        {
            "2022-03-09 19:12:05 +0615 and some text",
            {  9, wxDateTime::Mar, 2022,  12,  57,  5, -1 },
            true,
            " and some text",
            true
        },

        {
            "2022-03-09 15:12:05 UTC",
            {  9, wxDateTime::Mar, 2022,  15,  12,  5, -1 },
            true,
            "",
            true
        },

        {
            "2022-03-09 15:12:05 UTC and some text",
            {  9, wxDateTime::Mar, 2022,  15,  12,  5, -1 },
            true,
            " and some text",
            true
        },

        {
            // date after time
            "15:12:05 2022-03-09 UTC",
            {  9, wxDateTime::Mar, 2022,  15,  12,  5, -1 },
            true,
            "",
            true
        },

        {
            "2022-03-09 15:12:05 +010", // truncated time zone
            {  9, wxDateTime::Mar, 2022,  15,  12,  5, -1 },
            true,
            " +010",
            false
        },

        {
            "2022-03-09 15:12:05 GM", // truncated time zone
            {  9, wxDateTime::Mar, 2022,  15,  12,  5, -1 },
            true,
            " GM",
            false
        },

    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    // the test strings here use "PM" which is not available in all locales so
    // we need to use "C" locale for them
    wxCLocaleSetter cloc;

    wxDateTime dt;
    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const wxString datestr = TranslateDate(parseTestDates[n].str);

        const char * const end = dt.ParseDateTime(datestr);
        if ( end )
        {
            WX_ASSERT_MESSAGE(
                ("Erroneously parsed \"%s\"", datestr),
                parseTestDates[n].good
            );

            wxDateTime dtReal = parseTestDates[n].dateIsUTC ?
                parseTestDates[n].date.DT().FromUTC() :
                parseTestDates[n].date.DT();
            CPPUNIT_ASSERT_EQUAL( dtReal, dt );
            CPPUNIT_ASSERT_EQUAL( wxString(parseTestDates[n].beyondEnd), wxString(end) );
        }
        else // failed to parse
        {
            WX_ASSERT_MESSAGE(
                ("Failed to parse \"%s\"", datestr),
                !parseTestDates[n].good
            );

            CPPUNIT_ASSERT( !parseTestDates[n].good );
        }
    }
}

void DateTimeTestCase::TestDateWeekFormat()
{
    static const struct DateWeekTestData
    {
        int y, m, d;
        const char* result; // expected output of "%G%V"
    } testWeeks[] =
    {
        // Some manual test cases.
        { 2014,  1,  1, "2014-01" },
        { 2014,  1,  2, "2014-01" },
        { 2014,  1,  3, "2014-01" },
        { 2014,  1,  4, "2014-01" },
        { 2014,  1,  5, "2014-01" },
        { 2014,  1,  6, "2014-02" },
        { 2014,  1,  7, "2014-02" },
        { 2014, 12, 24, "2014-52" },
        { 2014, 12, 25, "2014-52" },
        { 2014, 12, 26, "2014-52" },
        { 2014, 12, 27, "2014-52" },
        { 2014, 12, 28, "2014-52" },
        { 2014, 12, 29, "2015-01" },
        { 2014, 12, 30, "2015-01" },
        { 2014, 12, 31, "2015-01" },
        { 2015, 12, 24, "2015-52" },
        { 2015, 12, 25, "2015-52" },
        { 2015, 12, 26, "2015-52" },
        { 2015, 12, 27, "2015-52" },
        { 2015, 12, 28, "2015-53" },
        { 2015, 12, 29, "2015-53" },
        { 2015, 12, 30, "2015-53" },
        { 2015, 12, 31, "2015-53" },
        { 2016,  1,  1, "2015-53" },
        { 2016,  1,  2, "2015-53" },
        { 2016,  1,  3, "2015-53" },
        { 2016,  1,  4, "2016-01" },
        { 2016,  1,  5, "2016-01" },
        { 2016,  1,  6, "2016-01" },
        { 2016,  1,  7, "2016-01" },

        // The rest of the tests was generated using the following zsh command:
        //
        //  for n in {0..19}; date --date $((RANDOM%100 + 2000))-$((RANDOM%12+1))-$((RANDOM%31+1)) +'{ %Y, %_m, %_d, "%G-%V" },'
        //
        // (ignore invalid dates if any are randomly created).
        { 2017, 11, 28, "2017-48" },
        { 2086,  9,  6, "2086-36" },
        { 2060, 11, 11, "2060-46" },
        { 2009,  5, 10, "2009-19" },
        { 2032, 12,  8, "2032-50" },
        { 2025,  4,  7, "2025-15" },
        { 2080,  5, 20, "2080-21" },
        { 2077,  7, 19, "2077-29" },
        { 2084, 12, 17, "2084-50" },
        { 2071,  4, 13, "2071-16" },
        { 2006,  1,  3, "2006-01" },
        { 2053,  8,  1, "2053-31" },
        { 2097,  8, 14, "2097-33" },
        { 2067,  1,  3, "2067-01" },
        { 2039,  9, 27, "2039-39" },
        { 2095,  2, 10, "2095-06" },
        { 2004,  7,  7, "2004-28" },
        { 2049, 12, 27, "2049-52" },
        { 2071,  8, 19, "2071-34" },
        { 2010, 11, 30, "2010-48" },
    };

    for ( size_t n = 0; n < WXSIZEOF(testWeeks); n++ )
    {
        const DateWeekTestData& td = testWeeks[n];
        wxDateTime d(td.d, wxDateTime::Month(td.m - 1), td.y);

        CPPUNIT_ASSERT_EQUAL( td.result, d.Format("%G-%V") );

        if ( td.y > 2000 )
            CPPUNIT_ASSERT_EQUAL( td.result + 2, d.Format("%g-%V") );
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

        CPPUNIT_ASSERT_EQUAL( dt, dt1 - span );
        CPPUNIT_ASSERT_EQUAL( dt, dt2 + span );
        CPPUNIT_ASSERT_EQUAL( dt1, dt2 + 2*span );
        CPPUNIT_ASSERT_EQUAL( span, dt1.DiffAsDateSpan(dt) );
    }

    // More date span arithmetics tests
    wxDateTime dtd1(5, wxDateTime::Jun, 1998);
    wxDateTime dtd2(6, wxDateTime::Aug, 1999);

    // All parts in dtd2 is after dtd1
    CPPUNIT_ASSERT_EQUAL( wxDateSpan(1, 2, 0, 1), dtd2.DiffAsDateSpan(dtd1) );

    // Year and month after, day earlier, so no full month
    // Jul has 31 days, so it's 31 - 5 + 4 = 30, or 4w 2d
    dtd2.Set(4, wxDateTime::Aug, 1999);
    CPPUNIT_ASSERT_EQUAL( wxDateSpan(1, 1, 4, 2), dtd2.DiffAsDateSpan(dtd1) );

    // Year and day after, month earlier, so no full year, but same day diff as
    // first example
    dtd2.Set(6, wxDateTime::May, 1999);
    CPPUNIT_ASSERT_EQUAL( wxDateSpan(0, 11, 0, 1), dtd2.DiffAsDateSpan(dtd1) );

    // Year after, month and day earlier, so no full month and no full year
    // April has 30 days, so it's 30 - 5 + 4 = 29, or 4w 1d
    dtd2.Set(4, wxDateTime::May, 1999);
    CPPUNIT_ASSERT_EQUAL( wxDateSpan(0, 10, 4, 1), dtd2.DiffAsDateSpan(dtd1) );

    // And a reverse. Now we should use days in Jun (again 30 => 4w 1d)
    CPPUNIT_ASSERT_EQUAL( wxDateSpan(0, -10, -4, -1), dtd1.DiffAsDateSpan(dtd2) );

    const wxTimeSpan ts1 = wxTimeSpan::Seconds(30);
    const wxTimeSpan ts2 = wxTimeSpan::Seconds(5);
    CPPUNIT_ASSERT_EQUAL( wxTimeSpan::Seconds(25), ts1 - ts2 );
}

void DateTimeTestCase::TestDSTBug()
{
    /////////////////////////
    // Test GetEndDST()
    wxDateTime dt = wxDateTime::GetEndDST(2004, wxDateTime::France);
    CPPUNIT_ASSERT_EQUAL(31, (int)dt.GetDay());
    CPPUNIT_ASSERT_EQUAL(wxDateTime::Oct, dt.GetMonth());
    CPPUNIT_ASSERT_EQUAL(2004, (int)dt.GetYear());
    CPPUNIT_ASSERT_EQUAL(1, (int)dt.GetHour());
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

    // Verify that setting the date to the beginning of the DST period moves it
    // forward (as this date on its own would be invalid). The problem here is
    // that our GetBeginDST() is far from being trustworthy, so just try a
    // couple of dates for the common time zones and check that all of them are
    // either unchanged or moved forward.
    wxDateTime dtDST(10, wxDateTime::Mar, 2013, 2, 0, 0);
    if ( dtDST.GetHour() != 2 )
        CPPUNIT_ASSERT_EQUAL( 3, dtDST.GetHour() );

    dtDST = wxDateTime(31, wxDateTime::Mar, 2013, 2, 0, 0);
    if ( dtDST.GetHour() != 2 )
        CPPUNIT_ASSERT_EQUAL( 3, dtDST.GetHour() );
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

void DateTimeTestCase::TestTranslateFromUnicodeFormat()
{
#if defined(__WINDOWS__) || defined(__WXOSX__)
    // This function is defined in src/common/intl.cpp and as it is not public we
    // need to declare it here explicitly.
    WXDLLIMPEXP_BASE
    wxString wxTranslateFromUnicodeFormat(const wxString& fmt);


    // Test single quote handling...

    CPPUNIT_ASSERT_EQUAL("",   wxTranslateFromUnicodeFormat("'"));
    CPPUNIT_ASSERT_EQUAL("%H", wxTranslateFromUnicodeFormat("H'"));
    CPPUNIT_ASSERT_EQUAL("H",  wxTranslateFromUnicodeFormat("'H"));

    CPPUNIT_ASSERT_EQUAL("'",   wxTranslateFromUnicodeFormat("''"));
    CPPUNIT_ASSERT_EQUAL("%H'", wxTranslateFromUnicodeFormat("H''"));
    CPPUNIT_ASSERT_EQUAL("H",   wxTranslateFromUnicodeFormat("'H'"));
    CPPUNIT_ASSERT_EQUAL("'%H", wxTranslateFromUnicodeFormat("''H"));

    CPPUNIT_ASSERT_EQUAL("'",   wxTranslateFromUnicodeFormat("'''"));
    CPPUNIT_ASSERT_EQUAL("%H'", wxTranslateFromUnicodeFormat("H'''"));
    CPPUNIT_ASSERT_EQUAL("H'",  wxTranslateFromUnicodeFormat("'H''"));
    CPPUNIT_ASSERT_EQUAL("'%H", wxTranslateFromUnicodeFormat("''H'"));
    CPPUNIT_ASSERT_EQUAL("'H",  wxTranslateFromUnicodeFormat("'''H"));

    CPPUNIT_ASSERT_EQUAL("''",   wxTranslateFromUnicodeFormat("''''"));
    CPPUNIT_ASSERT_EQUAL("%H''", wxTranslateFromUnicodeFormat("H''''"));
    CPPUNIT_ASSERT_EQUAL("H'",   wxTranslateFromUnicodeFormat("'H'''"));
    CPPUNIT_ASSERT_EQUAL("'%H'", wxTranslateFromUnicodeFormat("''H''"));
    CPPUNIT_ASSERT_EQUAL("'H",   wxTranslateFromUnicodeFormat("'''H'"));
    CPPUNIT_ASSERT_EQUAL("''%H", wxTranslateFromUnicodeFormat("''''H"));

    CPPUNIT_ASSERT_EQUAL("'%H o'clock: It's about time'",
        wxTranslateFromUnicodeFormat("''H 'o''clock: It''s about time'''"));
#endif // ports having wxTranslateFromUnicodeFormat()
}

void DateTimeTestCase::TestConvToFromLocalTZ()
{
    // Choose a date when the DST is on in many time zones and verify that
    // converting from/to local time zone still doesn't modify time in this
    // case as this used to be broken.
    wxDateTime dt(18, wxDateTime::Apr, 2017, 19);

    CHECK( dt.FromTimezone(wxDateTime::Local) == dt );
    CHECK( dt.ToTimezone(wxDateTime::Local) == dt );

    // For a date when the DST is not used, this always worked, but still
    // verify that it continues to.
    dt = wxDateTime(18, wxDateTime::Jan, 2018, 19);

    CHECK( dt.FromTimezone(wxDateTime::Local) == dt );
    CHECK( dt.ToTimezone(wxDateTime::Local) == dt );
}

static void DoTestSetFunctionsOnDST(const wxDateTime &orig)
{
#define DST_TEST_FUN(func) \
    { \
        wxDateTime copy = orig; \
        copy.func; \
        INFO("Original date=" << orig << ", modified=" << copy); \
        CHECK(orig.GetHour() == copy.GetHour()); \
    }

    // Test the functions by just calling them with their existing values
    // This is primarily just ensuring that we're not converting to a Tm and back
    // but also if we do that we're handling it properly
    DST_TEST_FUN(SetMinute(orig.GetMinute()));
    DST_TEST_FUN(SetSecond(orig.GetSecond()));
    DST_TEST_FUN(SetMillisecond(orig.GetMillisecond()));
    DST_TEST_FUN(SetDay(orig.GetDay()));
    DST_TEST_FUN(SetMonth(orig.GetMonth()));
    DST_TEST_FUN(SetYear(orig.GetYear()));

    // Test again by actually changing the time (this shouldn't affect DST)
    // Can't test changing the date because that WILL affect DST
    DST_TEST_FUN(SetMinute((orig.GetMinute() + 1) % 60));
    DST_TEST_FUN(SetSecond((orig.GetSecond() + 1) % 60));
    DST_TEST_FUN(SetMillisecond((orig.GetMillisecond() + 1) % 1000));

    {
        // Special case for set hour since it's ambiguous at DST we don't care if IsDST matches
        wxDateTime copy = orig;
        copy.SetHour(orig.GetHour());
        INFO("Original date=" << orig << ", modified=" << copy);
        CHECK(orig.GetHour() == copy.GetHour());
    }
#undef DST_TEST_FUN
}

TEST_CASE("wxDateTime::SetOnDST", "[datetime][dst]")
{
    wxDateTime dst = wxDateTime::GetEndDST();
    if ( !dst.IsValid() )
    {
        WARN("Skipping test as DST period couldn't be determined.");
        return;
    }

    SECTION("An hour before DST end")
    {
        DoTestSetFunctionsOnDST(dst - wxTimeSpan::Hour());
    }

    SECTION("At DST end")
    {
        DoTestSetFunctionsOnDST(dst);
    }
}

// Tests random problems that used to appear in BST time zone during DST.
// This test is disabled by default as it only passes in BST time zone, due to
// the times hard-coded in it.
TEST_CASE("wxDateTime-BST-bugs", "[datetime][dst][BST][.]")
{
    SECTION("bug-17220")
    {
        wxDateTime dt;
        dt.Set(22, wxDateTime::Oct, 2015, 10, 10, 10, 10);
        REQUIRE( dt.IsDST() );

        CHECK( dt.GetTm().hour == 10 );
        CHECK( dt.GetTm(wxDateTime::UTC).hour == 9 );

        CHECK( dt.Format("%Y-%m-%d %H:%M:%S", wxDateTime::Local ) == "2015-10-22 10:10:10" );
        CHECK( dt.Format("%Y-%m-%d %H:%M:%S", wxDateTime::UTC   ) == "2015-10-22 09:10:10" );

        dt.MakeFromUTC();
        CHECK( dt.Format("%Y-%m-%d %H:%M:%S", wxDateTime::Local ) == "2015-10-22 11:10:10" );
        CHECK( dt.Format("%Y-%m-%d %H:%M:%S", wxDateTime::UTC   ) == "2015-10-22 10:10:10" );
    }
}

TEST_CASE("wxDateTime::UNow", "[datetime][now][unow]")
{
    // It's unlikely, but possible, that the consecutive functions are called
    // on different sides of some second boundary, but it really shouldn't
    // happen more than once in a row.
    wxDateTime now, unow;
    for ( int i = 0; i < 3; ++i )
    {
        now = wxDateTime::Now();
        unow = wxDateTime::UNow();
        if ( now.GetSecond() == unow.GetSecond() )
            break;

        WARN("wxDateTime::Now() and UNow() returned different "
             "second values ("
             << now.GetSecond() << " and " << unow.GetSecond() <<
             "), retrying.");

        wxMilliSleep(123);
    }

    CHECK( now.GetYear() == unow.GetYear() );
    CHECK( now.GetMonth() == unow.GetMonth() );
    CHECK( now.GetDay() == unow.GetDay() );
    CHECK( now.GetHour() == unow.GetHour() );
    CHECK( now.GetMinute() == unow.GetMinute() );
    CHECK( now.GetSecond() == unow.GetSecond() );

    CHECK( now.GetMillisecond() == 0 );

    // Just checking unow.GetMillisecond() == 0 would fail once per 1000 test
    // runs on average, which is certainly not a lot, but still try to avoid
    // such spurious failures.
    bool gotMS = false;
    for ( int i = 0; i < 3; ++i )
    {
        if ( wxDateTime::UNow().GetMillisecond() != 0 )
        {
            gotMS = true;
            break;
        }

        wxMilliSleep(123);
    }

    CHECK( gotMS );
}

TEST_CASE("Easter 1600s", "[datetime][holiday][easter]")
{
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1600).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1601).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1602).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1603).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1604).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1605).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1606).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1607).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1608).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1609).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1610).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1611).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1612).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1613).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1614).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1615).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1616).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1617).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1618).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1619).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1620).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1621).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1622).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1623).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1624).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1625).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1626).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1627).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1628).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1629).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1630).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1631).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1632).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1633).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1634).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1635).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1636).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1637).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1638).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1639).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1640).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1641).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1642).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1643).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1644).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1645).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1646).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1647).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1648).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1649).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1650).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1651).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1652).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1653).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1654).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1655).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1656).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1657).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1658).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1659).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1660).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1661).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1662).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1663).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1664).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1665).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1666).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1667).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1668).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1669).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1670).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1671).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1672).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1673).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1674).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1675).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1676).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1677).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1678).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1679).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1680).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1681).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1682).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1683).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1684).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1685).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1686).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1687).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1688).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1689).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1690).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1691).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1692).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1693).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1694).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1695).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1696).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1697).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1698).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1699).GetMonth() == 3);

    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1600).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1601).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1602).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1603).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1604).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1605).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1606).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1607).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1608).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1609).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1610).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1611).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1612).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1613).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1614).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1615).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1616).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1617).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1618).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1619).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1620).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1621).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1622).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1623).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1624).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1625).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1626).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1627).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1628).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1629).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1630).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1631).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1632).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1633).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1634).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1635).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1636).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1637).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1638).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1639).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1640).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1641).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1642).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1643).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1644).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1645).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1646).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1647).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1648).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1649).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1650).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1651).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1652).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1653).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1654).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1655).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1656).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1657).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1658).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1659).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1660).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1661).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1662).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1663).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1664).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1665).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1666).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1667).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1668).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1669).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1670).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1671).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1672).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1673).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1674).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1675).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1676).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1677).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1678).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1679).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1680).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1681).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1682).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1683).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1684).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1685).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1686).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1687).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1688).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1689).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1690).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1691).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1692).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1693).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1694).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1695).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1696).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1697).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1698).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1699).GetDay() == 19);
}

TEST_CASE("Easter 1700s", "[datetime][holiday][easter]")
{
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1700).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1701).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1702).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1703).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1704).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1705).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1706).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1707).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1708).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1709).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1710).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1711).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1712).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1713).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1714).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1715).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1716).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1717).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1718).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1719).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1720).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1721).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1722).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1723).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1724).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1725).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1726).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1727).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1728).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1729).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1730).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1731).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1732).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1733).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1734).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1735).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1736).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1737).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1738).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1739).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1740).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1741).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1742).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1743).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1744).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1745).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1746).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1747).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1748).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1749).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1750).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1751).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1752).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1753).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1754).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1755).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1756).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1757).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1758).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1759).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1760).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1761).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1762).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1763).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1764).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1765).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1766).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1767).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1768).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1769).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1770).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1771).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1772).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1773).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1774).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1775).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1776).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1777).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1778).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1779).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1780).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1781).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1782).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1783).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1784).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1785).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1786).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1787).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1788).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1789).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1790).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1791).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1792).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1793).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1794).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1795).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1796).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1797).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1798).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1799).GetMonth() == 2);

    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1700).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1701).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1702).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1703).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1704).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1705).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1706).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1707).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1708).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1709).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1710).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1711).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1712).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1713).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1714).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1715).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1716).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1717).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1718).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1719).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1720).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1721).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1722).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1723).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1724).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1725).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1726).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1727).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1728).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1729).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1730).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1731).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1732).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1733).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1734).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1735).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1736).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1737).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1738).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1739).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1740).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1741).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1742).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1743).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1744).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1745).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1746).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1747).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1748).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1749).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1750).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1751).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1752).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1753).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1754).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1755).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1756).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1757).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1758).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1759).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1760).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1761).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1762).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1763).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1764).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1765).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1766).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1767).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1768).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1769).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1770).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1771).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1772).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1773).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1774).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1775).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1776).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1777).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1778).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1779).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1780).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1781).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1782).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1783).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1784).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1785).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1786).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1787).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1788).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1789).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1790).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1791).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1792).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1793).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1794).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1795).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1796).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1797).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1798).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1799).GetDay() == 24);
}

TEST_CASE("Easter 1800s", "[datetime][holiday][easter]")
{
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1800).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1801).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1802).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1803).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1804).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1805).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1806).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1807).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1808).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1809).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1810).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1811).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1812).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1813).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1814).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1815).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1816).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1817).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1818).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1819).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1820).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1821).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1822).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1823).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1824).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1825).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1826).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1827).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1828).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1829).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1830).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1831).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1832).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1833).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1834).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1835).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1836).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1837).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1838).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1839).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1840).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1841).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1842).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1843).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1844).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1845).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1846).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1847).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1848).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1849).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1850).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1851).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1852).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1853).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1854).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1855).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1856).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1857).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1858).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1859).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1860).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1861).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1862).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1863).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1864).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1865).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1866).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1867).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1868).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1869).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1870).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1871).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1872).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1873).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1874).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1875).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1876).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1877).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1878).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1879).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1880).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1881).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1882).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1883).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1884).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1885).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1886).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1887).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1888).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1889).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1890).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1891).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1892).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1893).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1894).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1895).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1896).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1897).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1898).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1899).GetMonth() == 3);

    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1800).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1801).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1802).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1803).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1804).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1805).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1806).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1807).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1808).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1809).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1810).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1811).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1812).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1813).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1814).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1815).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1816).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1817).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1818).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1819).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1820).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1821).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1822).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1823).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1824).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1825).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1826).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1827).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1828).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1829).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1830).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1831).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1832).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1833).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1834).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1835).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1836).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1837).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1838).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1839).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1840).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1841).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1842).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1843).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1844).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1845).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1846).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1847).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1848).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1849).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1850).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1851).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1852).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1853).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1854).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1855).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1856).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1857).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1858).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1859).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1860).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1861).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1862).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1863).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1864).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1865).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1866).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1867).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1868).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1869).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1870).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1871).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1872).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1873).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1874).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1875).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1876).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1877).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1878).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1879).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1880).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1881).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1882).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1883).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1884).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1885).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1886).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1887).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1888).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1889).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1890).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1891).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1892).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1893).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1894).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1895).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1896).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1897).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1898).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1899).GetDay() == 2);
}

TEST_CASE("Easter 1900s", "[datetime][holiday][easter]")
{
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1900).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1901).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1902).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1903).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1904).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1905).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1906).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1907).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1908).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1909).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1910).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1911).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1912).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1913).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1914).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1915).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1916).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1917).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1918).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1919).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1920).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1921).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1922).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1923).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1924).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1925).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1926).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1927).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1928).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1929).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1930).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1931).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1932).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1933).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1934).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1935).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1936).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1937).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1938).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1939).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1940).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1941).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1942).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1943).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1944).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1945).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1946).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1947).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1948).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1949).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1950).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1951).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1952).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1953).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1954).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1955).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1956).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1957).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1958).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1959).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1960).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1961).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1962).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1963).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1964).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1965).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1966).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1967).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1968).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1969).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1970).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1971).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1972).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1973).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1974).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1975).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1976).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1977).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1978).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1979).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1980).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1981).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1982).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1983).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1984).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1985).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1986).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1987).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1988).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1989).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1990).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1991).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1992).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1993).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1994).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1995).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1996).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1997).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1998).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1999).GetMonth() == 3);

    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1900).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1901).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1902).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1903).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1904).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1905).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1906).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1907).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1908).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1909).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1910).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1911).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1912).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1913).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1914).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1915).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1916).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1917).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1918).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1919).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1920).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1921).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1922).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1923).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1924).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1925).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1926).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1927).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1928).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1929).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1930).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1931).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1932).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1933).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1934).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1935).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1936).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1937).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1938).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1939).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1940).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1941).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1942).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1943).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1944).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1945).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1946).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1947).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1948).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1949).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1950).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1951).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1952).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1953).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1954).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1955).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1956).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1957).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1958).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1959).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1960).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1961).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1962).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1963).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1964).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1965).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1966).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1967).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1968).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1969).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1970).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1971).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1972).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1973).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1974).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1975).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1976).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1977).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1978).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1979).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1980).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1981).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1982).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1983).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1984).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1985).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1986).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1987).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1988).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1989).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1990).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1991).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1992).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1993).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1994).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1995).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1996).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1997).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1998).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(1999).GetDay() == 4);
}

TEST_CASE("Easter 2000s", "[datetime][holiday][easter]")
{
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2000).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2001).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2002).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2003).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2004).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2005).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2006).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2007).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2008).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2009).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2010).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2011).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2012).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2013).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2014).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2015).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2016).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2017).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2018).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2019).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2020).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2021).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2022).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2023).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2024).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2025).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2026).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2027).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2028).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2029).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2030).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2031).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2032).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2033).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2034).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2035).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2036).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2037).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2038).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2039).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2040).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2041).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2042).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2043).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2044).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2045).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2046).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2047).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2048).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2049).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2050).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2051).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2052).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2053).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2054).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2055).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2056).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2057).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2058).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2059).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2060).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2061).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2062).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2063).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2064).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2065).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2066).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2067).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2068).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2069).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2070).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2071).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2072).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2073).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2074).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2075).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2076).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2077).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2078).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2079).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2080).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2081).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2082).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2083).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2084).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2085).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2086).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2087).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2088).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2089).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2090).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2091).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2092).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2093).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2094).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2095).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2096).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2097).GetMonth() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2098).GetMonth() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2099).GetMonth() == 3);

    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2000).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2001).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2002).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2003).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2004).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2005).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2006).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2007).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2008).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2009).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2010).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2011).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2012).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2013).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2014).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2015).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2016).GetDay() == 27);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2017).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2018).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2019).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2020).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2021).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2022).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2023).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2024).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2025).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2026).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2027).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2028).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2029).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2030).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2031).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2032).GetDay() == 28);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2033).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2034).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2035).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2036).GetDay() == 13);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2037).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2038).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2039).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2040).GetDay() == 1);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2041).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2042).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2043).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2044).GetDay() == 17);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2045).GetDay() == 9);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2046).GetDay() == 25);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2047).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2048).GetDay() == 5);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2049).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2050).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2051).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2052).GetDay() == 21);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2053).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2054).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2055).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2056).GetDay() == 2);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2057).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2058).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2059).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2060).GetDay() == 18);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2061).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2062).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2063).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2064).GetDay() == 6);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2065).GetDay() == 29);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2066).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2067).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2068).GetDay() == 22);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2069).GetDay() == 14);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2070).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2071).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2072).GetDay() == 10);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2073).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2074).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2075).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2076).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2077).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2078).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2079).GetDay() == 23);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2080).GetDay() == 7);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2081).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2082).GetDay() == 19);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2083).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2084).GetDay() == 26);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2085).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2086).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2087).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2088).GetDay() == 11);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2089).GetDay() == 3);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2090).GetDay() == 16);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2091).GetDay() == 8);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2092).GetDay() == 30);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2093).GetDay() == 12);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2094).GetDay() == 4);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2095).GetDay() == 24);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2096).GetDay() == 15);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2097).GetDay() == 31);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2098).GetDay() == 20);
    CHECK(wxDateTimeUSCatholicFeasts::GetEaster(2099).GetDay() == 12);
}

TEST_CASE("US Catholic Holidays", "[datetime][holiday]")
{
    SECTION("Ascension")
    {
        wxDateTime easter = wxDateTimeUSCatholicFeasts::GetThursdayAscension(2023);
        CHECK(easter.GetMonth() == wxDateTime::Month::May);
        CHECK(easter.GetDay() == 18);

        easter = wxDateTimeUSCatholicFeasts::GetSundayAscension(2023);
        CHECK(easter.GetMonth() == wxDateTime::Month::May);
        CHECK(easter.GetDay() == 21);
    }
    SECTION("Fixed date feasts")
    {
        wxDateTimeHolidayAuthority::AddAuthority(new wxDateTimeUSCatholicFeasts);
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(1, wxDateTime::Month::Jan, 2024)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(15, wxDateTime::Month::Aug, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(1, wxDateTime::Month::Nov, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(8, wxDateTime::Month::Dec, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(25, wxDateTime::Month::Dec, 2023)));
        // random days that should not be feasts of obligation
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(1, wxDateTime::Month::Dec, 2023)));
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(31, wxDateTime::Month::Oct, 2023)));
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(14, wxDateTime::Month::Feb, 2023)));
    }
}

#endif // wxUSE_DATETIME
