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
                if (am.empty() && pm.empty() && wxStrstr(fmt, "%p") != NULL)
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

                // convert back
                wxDateTime dt2;
                const char *result = dt2.ParseFormat(s, fmt);
                if ( !result )
                {
                    // conversion failed - should it have?
                    WX_ASSERT_MESSAGE(
                        ("Test #%u failed: failed to parse \"%s\"", n, s),
                        kind == CompareNone
                    );
                }
                else // conversion succeeded
                {
                    // currently ParseFormat() doesn't support "%Z" and so is
                    // incapable of parsing time zone part used at the end of date
                    // representations in many (but not "C") locales, compensate
                    // for it ourselves by simply consuming and ignoring it
                    while ( *result && (*result >= 'A' && *result <= 'Z') )
                        result++;

                    WX_ASSERT_MESSAGE(
                        ("Test #%u failed: \"%s\" was left unparsed in \"%s\"",
                         n, result, s),
                        !*result
                    );

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
    CPPUNIT_ASSERT( dt.ParseTime("12:34:56") );
    CPPUNIT_ASSERT_EQUAL( "12:34:56", dt.FormatISOTime() );

    // Parsing just hours should work too.
    dt.ResetTime();
    CPPUNIT_ASSERT( dt.ParseTime("17") );
    CPPUNIT_ASSERT_EQUAL( "17:00:00", dt.FormatISOTime() );

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

#if wxUSE_UNICODE
        // Containing minus sign (U+2212) as separator between time and tz.
        { "09:37" "\xe2\x88\x92" "0400", true },
#endif

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

        // seconds are optional according to the RFC
        {
            "Sun, 01 Jun 2008 16:30 +0200",
            {  1, wxDateTime::Jun, 2008, 14, 30, 00 },
            true
        },

        // try some bogus ones too
        {
            "Sun, 01 Jun 2008 16:30: +0200",
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
    } parseTestDates[] =
    {
        { "21 Mar 2006", { 21, wxDateTime::Mar, 2006 }, true },
        { "29 Feb 1976", { 29, wxDateTime::Feb, 1976 }, true },
        { "Feb 29 1976", { 29, wxDateTime::Feb, 1976 }, true },
        { "31/03/06",    { 31, wxDateTime::Mar,    6 }, true },
        { "31/03/2006",  { 31, wxDateTime::Mar, 2006 }, true },

        // some invalid ones too
        { "29 Feb 2006" },
        { "31/04/06" },
        { "bloordyblop" },
        { "2 .  .    " },
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
        if ( end && !*end )
        {
            WX_ASSERT_MESSAGE(
                ("Erroneously parsed \"%s\"", datestr),
                parseTestDates[n].good
            );

            CPPUNIT_ASSERT_EQUAL( parseTestDates[n].date.DT(), dt );
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
        Date date;              // NB: this should be in UTC
        bool good;
    } parseTestDates[] =
    {
        {
            "Thu 22 Nov 2007 07:40:00 PM",
            { 22, wxDateTime::Nov, 2007, 19, 40,  0 },
            true
        },

        {
            "2010-01-04 14:30",
            {  4, wxDateTime::Jan, 2010, 14, 30,  0 },
            true
        },

        {
            "bloordyblop",
            {  1, wxDateTime::Jan, 9999,  0,  0,  0},
            false
        },

        {
            "2012-01-01 10:12:05 +0100",
            {  1, wxDateTime::Jan, 2012,  10,  12,  5, -1 },
            false // ParseDateTime does know yet +0100
        },
    };

    wxGCC_WARNING_RESTORE(missing-field-initializers)

    // the test strings here use "PM" which is not available in all locales so
    // we need to use "C" locale for them
    CLocaleSetter cloc;

    wxDateTime dt;
    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const wxString datestr = TranslateDate(parseTestDates[n].str);

        const char * const end = dt.ParseDateTime(datestr);
        if ( end && !*end )
        {
            WX_ASSERT_MESSAGE(
                ("Erroneously parsed \"%s\"", datestr),
                parseTestDates[n].good
            );

            CPPUNIT_ASSERT_EQUAL( parseTestDates[n].date.DT(), dt );
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

#endif // wxUSE_DATETIME
