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
       { CompareDate, "ISO date using short form: %F" },
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
TEST_CASE("wxDateTime-BST-bugs", "[.]")
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

TEST_CASE("Easter", "[datetime][holiday][easter]")
{
    std::vector<wxDateTime> easters =
    {
        wxDateTime( 2, wxDateTime::Apr, 1600),
        wxDateTime(22, wxDateTime::Apr, 1601),
        wxDateTime( 7, wxDateTime::Apr, 1602),
        wxDateTime(30, wxDateTime::Mar, 1603),
        wxDateTime(18, wxDateTime::Apr, 1604),
        wxDateTime(10, wxDateTime::Apr, 1605),
        wxDateTime(26, wxDateTime::Mar, 1606),
        wxDateTime(15, wxDateTime::Apr, 1607),
        wxDateTime( 6, wxDateTime::Apr, 1608),
        wxDateTime(19, wxDateTime::Apr, 1609),
        wxDateTime(11, wxDateTime::Apr, 1610),
        wxDateTime( 3, wxDateTime::Apr, 1611),
        wxDateTime(22, wxDateTime::Apr, 1612),
        wxDateTime( 7, wxDateTime::Apr, 1613),
        wxDateTime(30, wxDateTime::Mar, 1614),
        wxDateTime(19, wxDateTime::Apr, 1615),
        wxDateTime( 3, wxDateTime::Apr, 1616),
        wxDateTime(26, wxDateTime::Mar, 1617),
        wxDateTime(15, wxDateTime::Apr, 1618),
        wxDateTime(31, wxDateTime::Mar, 1619),
        wxDateTime(19, wxDateTime::Apr, 1620),
        wxDateTime(11, wxDateTime::Apr, 1621),
        wxDateTime(27, wxDateTime::Mar, 1622),
        wxDateTime(16, wxDateTime::Apr, 1623),
        wxDateTime( 7, wxDateTime::Apr, 1624),
        wxDateTime(30, wxDateTime::Mar, 1625),
        wxDateTime(12, wxDateTime::Apr, 1626),
        wxDateTime( 4, wxDateTime::Apr, 1627),
        wxDateTime(23, wxDateTime::Apr, 1628),
        wxDateTime(15, wxDateTime::Apr, 1629),
        wxDateTime(31, wxDateTime::Mar, 1630),
        wxDateTime(20, wxDateTime::Apr, 1631),
        wxDateTime(11, wxDateTime::Apr, 1632),
        wxDateTime(27, wxDateTime::Mar, 1633),
        wxDateTime(16, wxDateTime::Apr, 1634),
        wxDateTime( 8, wxDateTime::Apr, 1635),
        wxDateTime(23, wxDateTime::Mar, 1636),
        wxDateTime(12, wxDateTime::Apr, 1637),
        wxDateTime( 4, wxDateTime::Apr, 1638),
        wxDateTime(24, wxDateTime::Apr, 1639),
        wxDateTime( 8, wxDateTime::Apr, 1640),
        wxDateTime(31, wxDateTime::Mar, 1641),
        wxDateTime(20, wxDateTime::Apr, 1642),
        wxDateTime( 5, wxDateTime::Apr, 1643),
        wxDateTime(27, wxDateTime::Mar, 1644),
        wxDateTime(16, wxDateTime::Apr, 1645),
        wxDateTime( 1, wxDateTime::Apr, 1646),
        wxDateTime(21, wxDateTime::Apr, 1647),
        wxDateTime(12, wxDateTime::Apr, 1648),
        wxDateTime( 4, wxDateTime::Apr, 1649),
        wxDateTime(17, wxDateTime::Apr, 1650),
        wxDateTime( 9, wxDateTime::Apr, 1651),
        wxDateTime(31, wxDateTime::Mar, 1652),
        wxDateTime(13, wxDateTime::Apr, 1653),
        wxDateTime( 5, wxDateTime::Apr, 1654),
        wxDateTime(28, wxDateTime::Mar, 1655),
        wxDateTime(16, wxDateTime::Apr, 1656),
        wxDateTime( 1, wxDateTime::Apr, 1657),
        wxDateTime(21, wxDateTime::Apr, 1658),
        wxDateTime(13, wxDateTime::Apr, 1659),
        wxDateTime(28, wxDateTime::Mar, 1660),
        wxDateTime(17, wxDateTime::Apr, 1661),
        wxDateTime( 9, wxDateTime::Apr, 1662),
        wxDateTime(25, wxDateTime::Mar, 1663),
        wxDateTime(13, wxDateTime::Apr, 1664),
        wxDateTime( 5, wxDateTime::Apr, 1665),
        wxDateTime(25, wxDateTime::Apr, 1666),
        wxDateTime(10, wxDateTime::Apr, 1667),
        wxDateTime( 1, wxDateTime::Apr, 1668),
        wxDateTime(21, wxDateTime::Apr, 1669),
        wxDateTime( 6, wxDateTime::Apr, 1670),
        wxDateTime(29, wxDateTime::Mar, 1671),
        wxDateTime(17, wxDateTime::Apr, 1672),
        wxDateTime( 2, wxDateTime::Apr, 1673),
        wxDateTime(25, wxDateTime::Mar, 1674),
        wxDateTime(14, wxDateTime::Apr, 1675),
        wxDateTime( 5, wxDateTime::Apr, 1676),
        wxDateTime(18, wxDateTime::Apr, 1677),
        wxDateTime(10, wxDateTime::Apr, 1678),
        wxDateTime( 2, wxDateTime::Apr, 1679),
        wxDateTime(21, wxDateTime::Apr, 1680),
        wxDateTime( 6, wxDateTime::Apr, 1681),
        wxDateTime(29, wxDateTime::Mar, 1682),
        wxDateTime(18, wxDateTime::Apr, 1683),
        wxDateTime( 2, wxDateTime::Apr, 1684),
        wxDateTime(22, wxDateTime::Apr, 1685),
        wxDateTime(14, wxDateTime::Apr, 1686),
        wxDateTime(30, wxDateTime::Mar, 1687),
        wxDateTime(18, wxDateTime::Apr, 1688),
        wxDateTime(10, wxDateTime::Apr, 1689),
        wxDateTime(26, wxDateTime::Mar, 1690),
        wxDateTime(15, wxDateTime::Apr, 1691),
        wxDateTime( 6, wxDateTime::Apr, 1692),
        wxDateTime(22, wxDateTime::Mar, 1693),
        wxDateTime(11, wxDateTime::Apr, 1694),
        wxDateTime( 3, wxDateTime::Apr, 1695),
        wxDateTime(22, wxDateTime::Apr, 1696),
        wxDateTime( 7, wxDateTime::Apr, 1697),
        wxDateTime(30, wxDateTime::Mar, 1698),
        wxDateTime(19, wxDateTime::Apr, 1699),
        wxDateTime(11, wxDateTime::Apr, 1700),
        wxDateTime(27, wxDateTime::Mar, 1701),
        wxDateTime(16, wxDateTime::Apr, 1702),
        wxDateTime( 8, wxDateTime::Apr, 1703),
        wxDateTime(23, wxDateTime::Mar, 1704),
        wxDateTime(12, wxDateTime::Apr, 1705),
        wxDateTime( 4, wxDateTime::Apr, 1706),
        wxDateTime(24, wxDateTime::Apr, 1707),
        wxDateTime( 8, wxDateTime::Apr, 1708),
        wxDateTime(31, wxDateTime::Mar, 1709),
        wxDateTime(20, wxDateTime::Apr, 1710),
        wxDateTime( 5, wxDateTime::Apr, 1711),
        wxDateTime(27, wxDateTime::Mar, 1712),
        wxDateTime(16, wxDateTime::Apr, 1713),
        wxDateTime( 1, wxDateTime::Apr, 1714),
        wxDateTime(21, wxDateTime::Apr, 1715),
        wxDateTime(12, wxDateTime::Apr, 1716),
        wxDateTime(28, wxDateTime::Mar, 1717),
        wxDateTime(17, wxDateTime::Apr, 1718),
        wxDateTime( 9, wxDateTime::Apr, 1719),
        wxDateTime(31, wxDateTime::Mar, 1720),
        wxDateTime(13, wxDateTime::Apr, 1721),
        wxDateTime( 5, wxDateTime::Apr, 1722),
        wxDateTime(28, wxDateTime::Mar, 1723),
        wxDateTime(16, wxDateTime::Apr, 1724),
        wxDateTime( 1, wxDateTime::Apr, 1725),
        wxDateTime(21, wxDateTime::Apr, 1726),
        wxDateTime(13, wxDateTime::Apr, 1727),
        wxDateTime(28, wxDateTime::Mar, 1728),
        wxDateTime(17, wxDateTime::Apr, 1729),
        wxDateTime( 9, wxDateTime::Apr, 1730),
        wxDateTime(25, wxDateTime::Mar, 1731),
        wxDateTime(13, wxDateTime::Apr, 1732),
        wxDateTime( 5, wxDateTime::Apr, 1733),
        wxDateTime(25, wxDateTime::Apr, 1734),
        wxDateTime(10, wxDateTime::Apr, 1735),
        wxDateTime( 1, wxDateTime::Apr, 1736),
        wxDateTime(21, wxDateTime::Apr, 1737),
        wxDateTime( 6, wxDateTime::Apr, 1738),
        wxDateTime(29, wxDateTime::Mar, 1739),
        wxDateTime(17, wxDateTime::Apr, 1740),
        wxDateTime( 2, wxDateTime::Apr, 1741),
        wxDateTime(25, wxDateTime::Mar, 1742),
        wxDateTime(14, wxDateTime::Apr, 1743),
        wxDateTime( 5, wxDateTime::Apr, 1744),
        wxDateTime(18, wxDateTime::Apr, 1745),
        wxDateTime(10, wxDateTime::Apr, 1746),
        wxDateTime( 2, wxDateTime::Apr, 1747),
        wxDateTime(14, wxDateTime::Apr, 1748),
        wxDateTime( 6, wxDateTime::Apr, 1749),
        wxDateTime(29, wxDateTime::Mar, 1750),
        wxDateTime(11, wxDateTime::Apr, 1751),
        wxDateTime( 2, wxDateTime::Apr, 1752),
        wxDateTime(22, wxDateTime::Apr, 1753),
        wxDateTime(14, wxDateTime::Apr, 1754),
        wxDateTime(30, wxDateTime::Mar, 1755),
        wxDateTime(18, wxDateTime::Apr, 1756),
        wxDateTime(10, wxDateTime::Apr, 1757),
        wxDateTime(26, wxDateTime::Mar, 1758),
        wxDateTime(15, wxDateTime::Apr, 1759),
        wxDateTime( 6, wxDateTime::Apr, 1760),
        wxDateTime(22, wxDateTime::Mar, 1761),
        wxDateTime(11, wxDateTime::Apr, 1762),
        wxDateTime( 3, wxDateTime::Apr, 1763),
        wxDateTime(22, wxDateTime::Apr, 1764),
        wxDateTime( 7, wxDateTime::Apr, 1765),
        wxDateTime(30, wxDateTime::Mar, 1766),
        wxDateTime(19, wxDateTime::Apr, 1767),
        wxDateTime( 3, wxDateTime::Apr, 1768),
        wxDateTime(26, wxDateTime::Mar, 1769),
        wxDateTime(15, wxDateTime::Apr, 1770),
        wxDateTime(31, wxDateTime::Mar, 1771),
        wxDateTime(19, wxDateTime::Apr, 1772),
        wxDateTime(11, wxDateTime::Apr, 1773),
        wxDateTime( 3, wxDateTime::Apr, 1774),
        wxDateTime(16, wxDateTime::Apr, 1775),
        wxDateTime( 7, wxDateTime::Apr, 1776),
        wxDateTime(30, wxDateTime::Mar, 1777),
        wxDateTime(19, wxDateTime::Apr, 1778),
        wxDateTime( 4, wxDateTime::Apr, 1779),
        wxDateTime(26, wxDateTime::Mar, 1780),
        wxDateTime(15, wxDateTime::Apr, 1781),
        wxDateTime(31, wxDateTime::Mar, 1782),
        wxDateTime(20, wxDateTime::Apr, 1783),
        wxDateTime(11, wxDateTime::Apr, 1784),
        wxDateTime(27, wxDateTime::Mar, 1785),
        wxDateTime(16, wxDateTime::Apr, 1786),
        wxDateTime( 8, wxDateTime::Apr, 1787),
        wxDateTime(23, wxDateTime::Mar, 1788),
        wxDateTime(12, wxDateTime::Apr, 1789),
        wxDateTime( 4, wxDateTime::Apr, 1790),
        wxDateTime(24, wxDateTime::Apr, 1791),
        wxDateTime( 8, wxDateTime::Apr, 1792),
        wxDateTime(31, wxDateTime::Mar, 1793),
        wxDateTime(20, wxDateTime::Apr, 1794),
        wxDateTime( 5, wxDateTime::Apr, 1795),
        wxDateTime(27, wxDateTime::Mar, 1796),
        wxDateTime(16, wxDateTime::Apr, 1797),
        wxDateTime( 8, wxDateTime::Apr, 1798),
        wxDateTime(24, wxDateTime::Mar, 1799),
        wxDateTime(13, wxDateTime::Apr, 1800),
        wxDateTime( 5, wxDateTime::Apr, 1801),
        wxDateTime(18, wxDateTime::Apr, 1802),
        wxDateTime(10, wxDateTime::Apr, 1803),
        wxDateTime( 1, wxDateTime::Apr, 1804),
        wxDateTime(14, wxDateTime::Apr, 1805),
        wxDateTime( 6, wxDateTime::Apr, 1806),
        wxDateTime(29, wxDateTime::Mar, 1807),
        wxDateTime(17, wxDateTime::Apr, 1808),
        wxDateTime( 2, wxDateTime::Apr, 1809),
        wxDateTime(22, wxDateTime::Apr, 1810),
        wxDateTime(14, wxDateTime::Apr, 1811),
        wxDateTime(29, wxDateTime::Mar, 1812),
        wxDateTime(18, wxDateTime::Apr, 1813),
        wxDateTime(10, wxDateTime::Apr, 1814),
        wxDateTime(26, wxDateTime::Mar, 1815),
        wxDateTime(14, wxDateTime::Apr, 1816),
        wxDateTime( 6, wxDateTime::Apr, 1817),
        wxDateTime(22, wxDateTime::Mar, 1818),
        wxDateTime(11, wxDateTime::Apr, 1819),
        wxDateTime( 2, wxDateTime::Apr, 1820),
        wxDateTime(22, wxDateTime::Apr, 1821),
        wxDateTime( 7, wxDateTime::Apr, 1822),
        wxDateTime(30, wxDateTime::Mar, 1823),
        wxDateTime(18, wxDateTime::Apr, 1824),
        wxDateTime( 3, wxDateTime::Apr, 1825),
        wxDateTime(26, wxDateTime::Mar, 1826),
        wxDateTime(15, wxDateTime::Apr, 1827),
        wxDateTime( 6, wxDateTime::Apr, 1828),
        wxDateTime(19, wxDateTime::Apr, 1829),
        wxDateTime(11, wxDateTime::Apr, 1830),
        wxDateTime( 3, wxDateTime::Apr, 1831),
        wxDateTime(22, wxDateTime::Apr, 1832),
        wxDateTime( 7, wxDateTime::Apr, 1833),
        wxDateTime(30, wxDateTime::Mar, 1834),
        wxDateTime(19, wxDateTime::Apr, 1835),
        wxDateTime( 3, wxDateTime::Apr, 1836),
        wxDateTime(26, wxDateTime::Mar, 1837),
        wxDateTime(15, wxDateTime::Apr, 1838),
        wxDateTime(31, wxDateTime::Mar, 1839),
        wxDateTime(19, wxDateTime::Apr, 1840),
        wxDateTime(11, wxDateTime::Apr, 1841),
        wxDateTime(27, wxDateTime::Mar, 1842),
        wxDateTime(16, wxDateTime::Apr, 1843),
        wxDateTime( 7, wxDateTime::Apr, 1844),
        wxDateTime(23, wxDateTime::Mar, 1845),
        wxDateTime(12, wxDateTime::Apr, 1846),
        wxDateTime( 4, wxDateTime::Apr, 1847),
        wxDateTime(23, wxDateTime::Apr, 1848),
        wxDateTime( 8, wxDateTime::Apr, 1849),
        wxDateTime(31, wxDateTime::Mar, 1850),
        wxDateTime(20, wxDateTime::Apr, 1851),
        wxDateTime(11, wxDateTime::Apr, 1852),
        wxDateTime(27, wxDateTime::Mar, 1853),
        wxDateTime(16, wxDateTime::Apr, 1854),
        wxDateTime( 8, wxDateTime::Apr, 1855),
        wxDateTime(23, wxDateTime::Mar, 1856),
        wxDateTime(12, wxDateTime::Apr, 1857),
        wxDateTime( 4, wxDateTime::Apr, 1858),
        wxDateTime(24, wxDateTime::Apr, 1859),
        wxDateTime( 8, wxDateTime::Apr, 1860),
        wxDateTime(31, wxDateTime::Mar, 1861),
        wxDateTime(20, wxDateTime::Apr, 1862),
        wxDateTime( 5, wxDateTime::Apr, 1863),
        wxDateTime(27, wxDateTime::Mar, 1864),
        wxDateTime(16, wxDateTime::Apr, 1865),
        wxDateTime( 1, wxDateTime::Apr, 1866),
        wxDateTime(21, wxDateTime::Apr, 1867),
        wxDateTime(12, wxDateTime::Apr, 1868),
        wxDateTime(28, wxDateTime::Mar, 1869),
        wxDateTime(17, wxDateTime::Apr, 1870),
        wxDateTime( 9, wxDateTime::Apr, 1871),
        wxDateTime(31, wxDateTime::Mar, 1872),
        wxDateTime(13, wxDateTime::Apr, 1873),
        wxDateTime( 5, wxDateTime::Apr, 1874),
        wxDateTime(28, wxDateTime::Mar, 1875),
        wxDateTime(16, wxDateTime::Apr, 1876),
        wxDateTime( 1, wxDateTime::Apr, 1877),
        wxDateTime(21, wxDateTime::Apr, 1878),
        wxDateTime(13, wxDateTime::Apr, 1879),
        wxDateTime(28, wxDateTime::Mar, 1880),
        wxDateTime(17, wxDateTime::Apr, 1881),
        wxDateTime( 9, wxDateTime::Apr, 1882),
        wxDateTime(25, wxDateTime::Mar, 1883),
        wxDateTime(13, wxDateTime::Apr, 1884),
        wxDateTime( 5, wxDateTime::Apr, 1885),
        wxDateTime(25, wxDateTime::Apr, 1886),
        wxDateTime(10, wxDateTime::Apr, 1887),
        wxDateTime( 1, wxDateTime::Apr, 1888),
        wxDateTime(21, wxDateTime::Apr, 1889),
        wxDateTime( 6, wxDateTime::Apr, 1890),
        wxDateTime(29, wxDateTime::Mar, 1891),
        wxDateTime(17, wxDateTime::Apr, 1892),
        wxDateTime( 2, wxDateTime::Apr, 1893),
        wxDateTime(25, wxDateTime::Mar, 1894),
        wxDateTime(14, wxDateTime::Apr, 1895),
        wxDateTime( 5, wxDateTime::Apr, 1896),
        wxDateTime(18, wxDateTime::Apr, 1897),
        wxDateTime(10, wxDateTime::Apr, 1898),
        wxDateTime( 2, wxDateTime::Apr, 1899),
        wxDateTime(15, wxDateTime::Apr, 1900),
        wxDateTime( 7, wxDateTime::Apr, 1901),
        wxDateTime(30, wxDateTime::Mar, 1902),
        wxDateTime(12, wxDateTime::Apr, 1903),
        wxDateTime( 3, wxDateTime::Apr, 1904),
        wxDateTime(23, wxDateTime::Apr, 1905),
        wxDateTime(15, wxDateTime::Apr, 1906),
        wxDateTime(31, wxDateTime::Mar, 1907),
        wxDateTime(19, wxDateTime::Apr, 1908),
        wxDateTime(11, wxDateTime::Apr, 1909),
        wxDateTime(27, wxDateTime::Mar, 1910),
        wxDateTime(16, wxDateTime::Apr, 1911),
        wxDateTime( 7, wxDateTime::Apr, 1912),
        wxDateTime(23, wxDateTime::Mar, 1913),
        wxDateTime(12, wxDateTime::Apr, 1914),
        wxDateTime( 4, wxDateTime::Apr, 1915),
        wxDateTime(23, wxDateTime::Apr, 1916),
        wxDateTime( 8, wxDateTime::Apr, 1917),
        wxDateTime(31, wxDateTime::Mar, 1918),
        wxDateTime(20, wxDateTime::Apr, 1919),
        wxDateTime( 4, wxDateTime::Apr, 1920),
        wxDateTime(27, wxDateTime::Mar, 1921),
        wxDateTime(16, wxDateTime::Apr, 1922),
        wxDateTime( 1, wxDateTime::Apr, 1923),
        wxDateTime(20, wxDateTime::Apr, 1924),
        wxDateTime(12, wxDateTime::Apr, 1925),
        wxDateTime( 4, wxDateTime::Apr, 1926),
        wxDateTime(17, wxDateTime::Apr, 1927),
        wxDateTime( 8, wxDateTime::Apr, 1928),
        wxDateTime(31, wxDateTime::Mar, 1929),
        wxDateTime(20, wxDateTime::Apr, 1930),
        wxDateTime( 5, wxDateTime::Apr, 1931),
        wxDateTime(27, wxDateTime::Mar, 1932),
        wxDateTime(16, wxDateTime::Apr, 1933),
        wxDateTime( 1, wxDateTime::Apr, 1934),
        wxDateTime(21, wxDateTime::Apr, 1935),
        wxDateTime(12, wxDateTime::Apr, 1936),
        wxDateTime(28, wxDateTime::Mar, 1937),
        wxDateTime(17, wxDateTime::Apr, 1938),
        wxDateTime( 9, wxDateTime::Apr, 1939),
        wxDateTime(24, wxDateTime::Mar, 1940),
        wxDateTime(13, wxDateTime::Apr, 1941),
        wxDateTime( 5, wxDateTime::Apr, 1942),
        wxDateTime(25, wxDateTime::Apr, 1943),
        wxDateTime( 9, wxDateTime::Apr, 1944),
        wxDateTime( 1, wxDateTime::Apr, 1945),
        wxDateTime(21, wxDateTime::Apr, 1946),
        wxDateTime( 6, wxDateTime::Apr, 1947),
        wxDateTime(28, wxDateTime::Mar, 1948),
        wxDateTime(17, wxDateTime::Apr, 1949),
        wxDateTime( 9, wxDateTime::Apr, 1950),
        wxDateTime(25, wxDateTime::Mar, 1951),
        wxDateTime(13, wxDateTime::Apr, 1952),
        wxDateTime( 5, wxDateTime::Apr, 1953),
        wxDateTime(18, wxDateTime::Apr, 1954),
        wxDateTime(10, wxDateTime::Apr, 1955),
        wxDateTime( 1, wxDateTime::Apr, 1956),
        wxDateTime(21, wxDateTime::Apr, 1957),
        wxDateTime( 6, wxDateTime::Apr, 1958),
        wxDateTime(29, wxDateTime::Mar, 1959),
        wxDateTime(17, wxDateTime::Apr, 1960),
        wxDateTime( 2, wxDateTime::Apr, 1961),
        wxDateTime(22, wxDateTime::Apr, 1962),
        wxDateTime(14, wxDateTime::Apr, 1963),
        wxDateTime(29, wxDateTime::Mar, 1964),
        wxDateTime(18, wxDateTime::Apr, 1965),
        wxDateTime(10, wxDateTime::Apr, 1966),
        wxDateTime(26, wxDateTime::Mar, 1967),
        wxDateTime(14, wxDateTime::Apr, 1968),
        wxDateTime( 6, wxDateTime::Apr, 1969),
        wxDateTime(29, wxDateTime::Mar, 1970),
        wxDateTime(11, wxDateTime::Apr, 1971),
        wxDateTime( 2, wxDateTime::Apr, 1972),
        wxDateTime(22, wxDateTime::Apr, 1973),
        wxDateTime(14, wxDateTime::Apr, 1974),
        wxDateTime(30, wxDateTime::Mar, 1975),
        wxDateTime(18, wxDateTime::Apr, 1976),
        wxDateTime(10, wxDateTime::Apr, 1977),
        wxDateTime(26, wxDateTime::Mar, 1978),
        wxDateTime(15, wxDateTime::Apr, 1979),
        wxDateTime( 6, wxDateTime::Apr, 1980),
        wxDateTime(19, wxDateTime::Apr, 1981),
        wxDateTime(11, wxDateTime::Apr, 1982),
        wxDateTime( 3, wxDateTime::Apr, 1983),
        wxDateTime(22, wxDateTime::Apr, 1984),
        wxDateTime( 7, wxDateTime::Apr, 1985),
        wxDateTime(30, wxDateTime::Mar, 1986),
        wxDateTime(19, wxDateTime::Apr, 1987),
        wxDateTime( 3, wxDateTime::Apr, 1988),
        wxDateTime(26, wxDateTime::Mar, 1989),
        wxDateTime(15, wxDateTime::Apr, 1990),
        wxDateTime(31, wxDateTime::Mar, 1991),
        wxDateTime(19, wxDateTime::Apr, 1992),
        wxDateTime(11, wxDateTime::Apr, 1993),
        wxDateTime( 3, wxDateTime::Apr, 1994),
        wxDateTime(16, wxDateTime::Apr, 1995),
        wxDateTime( 7, wxDateTime::Apr, 1996),
        wxDateTime(30, wxDateTime::Mar, 1997),
        wxDateTime(12, wxDateTime::Apr, 1998),
        wxDateTime( 4, wxDateTime::Apr, 1999),
        wxDateTime(23, wxDateTime::Apr, 2000),
        wxDateTime(15, wxDateTime::Apr, 2001),
        wxDateTime(31, wxDateTime::Mar, 2002),
        wxDateTime(20, wxDateTime::Apr, 2003),
        wxDateTime(11, wxDateTime::Apr, 2004),
        wxDateTime(27, wxDateTime::Mar, 2005),
        wxDateTime(16, wxDateTime::Apr, 2006),
        wxDateTime( 8, wxDateTime::Apr, 2007),
        wxDateTime(23, wxDateTime::Mar, 2008),
        wxDateTime(12, wxDateTime::Apr, 2009),
        wxDateTime( 4, wxDateTime::Apr, 2010),
        wxDateTime(24, wxDateTime::Apr, 2011),
        wxDateTime( 8, wxDateTime::Apr, 2012),
        wxDateTime(31, wxDateTime::Mar, 2013),
        wxDateTime(20, wxDateTime::Apr, 2014),
        wxDateTime( 5, wxDateTime::Apr, 2015),
        wxDateTime(27, wxDateTime::Mar, 2016),
        wxDateTime(16, wxDateTime::Apr, 2017),
        wxDateTime( 1, wxDateTime::Apr, 2018),
        wxDateTime(21, wxDateTime::Apr, 2019),
        wxDateTime(12, wxDateTime::Apr, 2020),
        wxDateTime( 4, wxDateTime::Apr, 2021),
        wxDateTime(17, wxDateTime::Apr, 2022),
        wxDateTime( 9, wxDateTime::Apr, 2023),
        wxDateTime(31, wxDateTime::Mar, 2024),
        wxDateTime(20, wxDateTime::Apr, 2025),
        wxDateTime( 5, wxDateTime::Apr, 2026),
        wxDateTime(28, wxDateTime::Mar, 2027),
        wxDateTime(16, wxDateTime::Apr, 2028),
        wxDateTime( 1, wxDateTime::Apr, 2029),
        wxDateTime(21, wxDateTime::Apr, 2030),
        wxDateTime(13, wxDateTime::Apr, 2031),
        wxDateTime(28, wxDateTime::Mar, 2032),
        wxDateTime(17, wxDateTime::Apr, 2033),
        wxDateTime( 9, wxDateTime::Apr, 2034),
        wxDateTime(25, wxDateTime::Mar, 2035),
        wxDateTime(13, wxDateTime::Apr, 2036),
        wxDateTime( 5, wxDateTime::Apr, 2037),
        wxDateTime(25, wxDateTime::Apr, 2038),
        wxDateTime(10, wxDateTime::Apr, 2039),
        wxDateTime( 1, wxDateTime::Apr, 2040),
        wxDateTime(21, wxDateTime::Apr, 2041),
        wxDateTime( 6, wxDateTime::Apr, 2042),
        wxDateTime(29, wxDateTime::Mar, 2043),
        wxDateTime(17, wxDateTime::Apr, 2044),
        wxDateTime( 9, wxDateTime::Apr, 2045),
        wxDateTime(25, wxDateTime::Mar, 2046),
        wxDateTime(14, wxDateTime::Apr, 2047),
        wxDateTime( 5, wxDateTime::Apr, 2048),
        wxDateTime(18, wxDateTime::Apr, 2049),
        wxDateTime(10, wxDateTime::Apr, 2050),
        wxDateTime( 2, wxDateTime::Apr, 2051),
        wxDateTime(21, wxDateTime::Apr, 2052),
        wxDateTime( 6, wxDateTime::Apr, 2053),
        wxDateTime(29, wxDateTime::Mar, 2054),
        wxDateTime(18, wxDateTime::Apr, 2055),
        wxDateTime( 2, wxDateTime::Apr, 2056),
        wxDateTime(22, wxDateTime::Apr, 2057),
        wxDateTime(14, wxDateTime::Apr, 2058),
        wxDateTime(30, wxDateTime::Mar, 2059),
        wxDateTime(18, wxDateTime::Apr, 2060),
        wxDateTime(10, wxDateTime::Apr, 2061),
        wxDateTime(26, wxDateTime::Mar, 2062),
        wxDateTime(15, wxDateTime::Apr, 2063),
        wxDateTime( 6, wxDateTime::Apr, 2064),
        wxDateTime(29, wxDateTime::Mar, 2065),
        wxDateTime(11, wxDateTime::Apr, 2066),
        wxDateTime( 3, wxDateTime::Apr, 2067),
        wxDateTime(22, wxDateTime::Apr, 2068),
        wxDateTime(14, wxDateTime::Apr, 2069),
        wxDateTime(30, wxDateTime::Mar, 2070),
        wxDateTime(19, wxDateTime::Apr, 2071),
        wxDateTime(10, wxDateTime::Apr, 2072),
        wxDateTime(26, wxDateTime::Mar, 2073),
        wxDateTime(15, wxDateTime::Apr, 2074),
        wxDateTime( 7, wxDateTime::Apr, 2075),
        wxDateTime(19, wxDateTime::Apr, 2076),
        wxDateTime(11, wxDateTime::Apr, 2077),
        wxDateTime( 3, wxDateTime::Apr, 2078),
        wxDateTime(23, wxDateTime::Apr, 2079),
        wxDateTime( 7, wxDateTime::Apr, 2080),
        wxDateTime(30, wxDateTime::Mar, 2081),
        wxDateTime(19, wxDateTime::Apr, 2082),
        wxDateTime( 4, wxDateTime::Apr, 2083),
        wxDateTime(26, wxDateTime::Mar, 2084),
        wxDateTime(15, wxDateTime::Apr, 2085),
        wxDateTime(31, wxDateTime::Mar, 2086),
        wxDateTime(20, wxDateTime::Apr, 2087),
        wxDateTime(11, wxDateTime::Apr, 2088),
        wxDateTime( 3, wxDateTime::Apr, 2089),
        wxDateTime(16, wxDateTime::Apr, 2090),
        wxDateTime( 8, wxDateTime::Apr, 2091),
        wxDateTime(30, wxDateTime::Mar, 2092),
        wxDateTime(12, wxDateTime::Apr, 2093),
        wxDateTime( 4, wxDateTime::Apr, 2094),
        wxDateTime(24, wxDateTime::Apr, 2095),
        wxDateTime(15, wxDateTime::Apr, 2096),
        wxDateTime(31, wxDateTime::Mar, 2097),
        wxDateTime(20, wxDateTime::Apr, 2098),
        wxDateTime(12, wxDateTime::Apr, 2099)
    };

    for (const auto& easter : easters)
    {
        INFO("Checking year " << easter.GetYear());
        CHECK(wxDateTimeUSCatholicFeasts::GetEaster(easter.GetYear()).IsSameDate(easter));
    }
}

TEST_CASE("US Catholic Holidays", "[datetime][holiday]")
{
    SECTION("Ascension")
    {
        wxDateTime ascension = wxDateTimeUSCatholicFeasts::GetThursdayAscension(2023);
        CHECK(ascension.GetMonth() == wxDateTime::Month::May);
        CHECK(ascension.GetDay() == 18);

        ascension = wxDateTimeUSCatholicFeasts::GetSundayAscension(2023);
        CHECK(ascension.GetMonth() == wxDateTime::Month::May);
        CHECK(ascension.GetDay() == 21);
    }
    SECTION("Fixed date feasts")
    {
        wxDateTimeHolidayAuthority::AddAuthority(new wxDateTimeUSCatholicFeasts);
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime( 1, wxDateTime::Month::Jan, 2024)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(15, wxDateTime::Month::Aug, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime( 1, wxDateTime::Month::Nov, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime( 8, wxDateTime::Month::Dec, 2023)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(25, wxDateTime::Month::Dec, 2023)));
        // random days that should not be feasts of obligation
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime( 1, wxDateTime::Month::Dec, 2023)));
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(31, wxDateTime::Month::Oct, 2023)));
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(14, wxDateTime::Month::Feb, 2023)));
    }
}

TEST_CASE("Christian Holidays", "[datetime][holiday][christian]")
{
    SECTION("Easter")
    {
        wxDateTime easter = wxDateTimeChristianHolidays::GetEaster(2023);
        CHECK(easter.GetMonth() == wxDateTime::Month::Apr);
        CHECK(easter.GetDay() == 9);

        easter = wxDateTimeChristianHolidays::GetEaster(2010);
        CHECK(easter.GetMonth() == wxDateTime::Month::Apr);
        CHECK(easter.GetDay() == 4);
    }
    SECTION("Christmas")
    {
        wxDateTimeHolidayAuthority::AddAuthority(new wxDateTimeChristianHolidays);
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(25, wxDateTime::Month::Dec, 1990)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(25, wxDateTime::Month::Dec, 1700)));
        CHECK(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(25, wxDateTime::Month::Dec, 2023)));
        // random days that are not Christmas or weekends
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(1, wxDateTime::Month::Dec, 2023)));
        CHECK_FALSE(wxDateTimeHolidayAuthority::IsHoliday(wxDateTime(29, wxDateTime::Month::Dec, 2023)));
    }
}

#endif // wxUSE_DATETIME
