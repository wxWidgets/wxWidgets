/////////////////////////////////////////////////////////////////////////////
// Name:        samples/console/console.cpp
// Purpose:     a sample console (as opposed to GUI) progam using wxWindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.10.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <stdio.h>

#include <wx/string.h>
#include <wx/file.h>
#include <wx/app.h>

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// what to test?

//#define TEST_ARRAYS
//#define TEST_DIR
//#define TEST_LOG
//#define TEST_MIME
//#define TEST_STRINGS
//#define TEST_THREADS
#define TEST_TIME
//#define TEST_LONGLONG

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDir
// ----------------------------------------------------------------------------

#ifdef TEST_DIR

#include <wx/dir.h>

static void TestDirEnumHelper(wxDir& dir,
                              int flags = wxDIR_DEFAULT,
                              const wxString& filespec = wxEmptyString)
{
    wxString filename;

    if ( !dir.IsOpened() )
        return;

    bool cont = dir.GetFirst(&filename, filespec, flags);
    while ( cont )
    {
        printf("\t%s\n", filename.c_str());

        cont = dir.GetNext(&filename);
    }

    puts("");
}

static void TestDirEnum()
{
    wxDir dir(wxGetCwd());

    puts("Enumerating everything in current directory:");
    TestDirEnumHelper(dir);

    puts("Enumerating really everything in current directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT | wxDIR_DOTDOT);

    puts("Enumerating object files in current directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT, "*.o");

    puts("Enumerating directories in current directory:");
    TestDirEnumHelper(dir, wxDIR_DIRS);

    puts("Enumerating files in current directory:");
    TestDirEnumHelper(dir, wxDIR_FILES);

    puts("Enumerating files including hidden in current directory:");
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

#ifdef __UNIX__
    dir.Open("/");
#elif defined(__WXMSW__)
    dir.Open("c:\\");
#else
    #error "don't know where the root directory is"
#endif

    puts("Enumerating everything in root directory:");
    TestDirEnumHelper(dir, wxDIR_DEFAULT);

    puts("Enumerating directories in root directory:");
    TestDirEnumHelper(dir, wxDIR_DIRS);

    puts("Enumerating files in root directory:");
    TestDirEnumHelper(dir, wxDIR_FILES);

    puts("Enumerating files including hidden in root directory:");
    TestDirEnumHelper(dir, wxDIR_FILES | wxDIR_HIDDEN);

    puts("Enumerating files in non existing directory:");
    wxDir dirNo("nosuchdir");
    TestDirEnumHelper(dirNo);
}

#endif // TEST_DIR

// ----------------------------------------------------------------------------
// MIME types
// ----------------------------------------------------------------------------

#ifdef TEST_MIME

#include <wx/mimetype.h>

static void TestMimeEnum()
{
    wxMimeTypesManager mimeTM;
    wxArrayString mimetypes;

    size_t count = mimeTM.EnumAllFileTypes(mimetypes);

    printf("*** All %u known filetypes: ***\n", count);

    wxArrayString exts;
    wxString desc;

    for ( size_t n = 0; n < count; n++ )
    {
        wxFileType *filetype = mimeTM.GetFileTypeFromMimeType(mimetypes[n]);
        if ( !filetype )
        {
            printf("nothing known about the filetype '%s'!\n",
                   mimetypes[n].c_str());
            continue;
        }

        filetype->GetDescription(&desc);
        filetype->GetExtensions(exts);

        filetype->GetIcon(NULL);

        wxString extsAll;
        for ( size_t e = 0; e < exts.GetCount(); e++ )
        {
            if ( e > 0 )
                extsAll << _T(", ");
            extsAll += exts[e];
        }

        printf("\t%s: %s (%s)\n",
               mimetypes[n].c_str(), desc.c_str(), extsAll.c_str());
    }
}

#endif // TEST_MIME

// ----------------------------------------------------------------------------
// long long
// ----------------------------------------------------------------------------

#ifdef TEST_LONGLONG

#include <wx/longlong.h>
#include <wx/timer.h>

static void TestSpeed()
{
    static const long max = 100000000;
    long n;

    {
        wxStopWatch sw;

        long l = 0;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        printf("Summing longs took %ld milliseconds.\n", sw.Time());
    }

    {
        wxStopWatch sw;

        __int64 l = 0;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        printf("Summing __int64s took %ld milliseconds.\n", sw.Time());
    }

    {
        wxStopWatch sw;

        wxLongLong l;
        for ( n = 0; n < max; n++ )
        {
            l += n;
        }

        printf("Summing wxLongLongs took %ld milliseconds.\n", sw.Time());
    }
}

static void TestDivision()
{
    #define MAKE_LL(x1, x2, x3, x4) wxLongLong((x1 << 16) | x2, (x3 << 16) | x3)

    // seed pseudo random generator
    //srand((unsigned)time(NULL));

    size_t nTested = 0;
    for ( size_t n = 0; n < 10000; n++ )
    {
        // get a random wxLongLong (shifting by 12 the MSB ensures that the
        // multiplication will not overflow)
        wxLongLong ll = MAKE_LL((rand() >> 12), rand(), rand(), rand());

        wxASSERT( (ll * 1000l)/1000l == ll );

        nTested++;
    }

    printf("\n*** Tested %u divisions/multiplications: ok\n", nTested);

    #undef MAKE_LL
}

#endif // TEST_LONGLONG

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_TIME

#include <wx/date.h>

#include <wx/datetime.h>

// the test data
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
        s.Printf("%02d:%02d:%02d %10s %02d, %4d%s",
                 hour, min, sec,
                 wxDateTime::GetMonthName(month).c_str(),
                 day,
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? "AD" : "BC");
        return s;
    }

    wxString FormatDate() const
    {
        wxString s;
        s.Printf("%02d-%s-%4d%s",
                 day,
                 wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
                 abs(wxDateTime::ConvertYearToBC(year)),
                 year > 0 ? "AD" : "BC");
        return s;
    }
};

static const Date testDates[] =
{
    {  1, wxDateTime::Jan,  1970, 00, 00, 00, 2440587.5, wxDateTime::Thu,         0,     -3600 },
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

// this test miscellaneous static wxDateTime functions
static void TestTimeStatic()
{
    puts("\n*** wxDateTime static methods test ***");

    // some info about the current date
    int year = wxDateTime::GetCurrentYear();
    printf("Current year %d is %sa leap one and has %d days.\n",
           year,
           wxDateTime::IsLeapYear(year) ? "" : "not ",
           wxDateTime::GetNumberOfDays(year));

    wxDateTime::Month month = wxDateTime::GetCurrentMonth();
    printf("Current month is '%s' ('%s') and it has %d days\n",
           wxDateTime::GetMonthName(month, wxDateTime::Name_Abbr).c_str(),
           wxDateTime::GetMonthName(month).c_str(),
           wxDateTime::GetNumberOfDays(month));

    // leap year logic
    static const size_t nYears = 5;
    static const size_t years[2][nYears] =
    {
        // first line: the years to test
        { 1990, 1976, 2000, 2030, 1984, },

        // second line: TRUE if leap, FALSE otherwise
        { FALSE, TRUE, TRUE, FALSE, TRUE }
    };

    for ( size_t n = 0; n < nYears; n++ )
    {
        int year = years[0][n];
        bool should = years[1][n] != 0,
             is = wxDateTime::IsLeapYear(year);

        printf("Year %d is %sa leap year (%s)\n",
               year,
               is ? "" : "not ",
               should == is ? "ok" : "ERROR");

        wxASSERT( should == wxDateTime::IsLeapYear(year) );
    }
}

// test constructing wxDateTime objects
static void TestTimeSet()
{
    puts("\n*** wxDateTime construction test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d1 = testDates[n];
        wxDateTime dt = d1.DT();

        Date d2;
        d2.Init(dt.GetTm());

        wxString s1 = d1.Format(),
                 s2 = d2.Format();

        printf("Date: %s == %s (%s)\n",
               s1.c_str(), s2.c_str(),
               s1 == s2 ? "ok" : "ERROR");
    }
}

// test time zones stuff
static void TestTimeZones()
{
    puts("\n*** wxDateTime timezone test ***");

    wxDateTime now = wxDateTime::Now();

    printf("Current GMT time:\t%s\n", now.Format("%c", wxDateTime::GMT0).c_str());
    printf("Unix epoch (GMT):\t%s\n", wxDateTime((time_t)0).Format("%c", wxDateTime::GMT0).c_str());
    printf("Unix epoch (EST):\t%s\n", wxDateTime((time_t)0).Format("%c", wxDateTime::EST).c_str());
    printf("Current time in Paris:\t%s\n", now.Format("%c", wxDateTime::CET).c_str());
    printf("               Moscow:\t%s\n", now.Format("%c", wxDateTime::MSK).c_str());
    printf("             New York:\t%s\n", now.Format("%c", wxDateTime::EST).c_str());
}

// test some minimal support for the dates outside the standard range
static void TestTimeRange()
{
    puts("\n*** wxDateTime out-of-standard-range dates test ***");

    static const char *fmt = "%d-%b-%Y %H:%M:%S";

    printf("Unix epoch:\t%s\n",
           wxDateTime(2440587.5).Format(fmt).c_str());
    printf("Feb 29, 0: \t%s\n",
            wxDateTime(29, wxDateTime::Feb, 0).Format(fmt).c_str());
    printf("JDN 0:     \t%s\n",
            wxDateTime(0.0).Format(fmt).c_str());
    printf("Jan 1, 1AD:\t%s\n",
            wxDateTime(1, wxDateTime::Jan, 1).Format(fmt).c_str());
    printf("May 29, 2099:\t%s\n",
            wxDateTime(29, wxDateTime::May, 2099).Format(fmt).c_str());
}

static void TestTimeTicks()
{
    puts("\n*** wxDateTime ticks test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        if ( d.ticks == -1 )
            continue;

        wxDateTime dt = d.DT();
        long ticks = (dt.GetValue() / 1000).ToLong();
        printf("Ticks of %s:\t% 10ld", d.Format().c_str(), ticks);
        if ( ticks == d.ticks )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %ld, delta = %ld)\n",
                   d.ticks, ticks - d.ticks);
        }

        dt = d.DT().ToTimezone(wxDateTime::GMT0);
        ticks = (dt.GetValue() / 1000).ToLong();
        printf("GMtks of %s:\t% 10ld", d.Format().c_str(), ticks);
        if ( ticks == d.gmticks )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %ld, delta = %ld)\n",
                   d.gmticks, ticks - d.gmticks);
        }
    }

    puts("");
}

// test conversions to JDN &c
static void TestTimeJDN()
{
    puts("\n*** wxDateTime to JDN test ***");

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);
        double jdn = dt.GetJulianDayNumber();

        printf("JDN of %s is:\t% 15.6f", d.Format().c_str(), jdn);
        if ( jdn == d.jdn )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %f, delta = %f)\n",
                   d.jdn, jdn - d.jdn);
        }
    }
}

// test week days computation
static void TestTimeWDays()
{
    puts("\n*** wxDateTime weekday test ***");

    // test GetWeekDay()
    size_t n;
    for ( n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        printf("%s is: %s",
               d.Format().c_str(),
               wxDateTime::GetWeekDayName(wday).c_str());
        if ( wday == d.wday )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n",
                   wxDateTime::GetWeekDayName(d.wday).c_str());
        }
    }

    puts("");

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
                case 1: which = "first"; break;
                case 2: which = "second"; break;
                case 3: which = "third"; break;
                case 4: which = "fourth"; break;
                case 5: which = "fifth"; break;

                case -1: which = "last"; break;
            }

            if ( nWeek < -1 )
            {
                which += " from end";
            }

            s.Printf("The %s %s of %s in %d",
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
        { { 20, wxDateTime::Mar, 2045 },  3, wxDateTime::Mon, wxDateTime::Mar, 2045 },
        { {  5, wxDateTime::Jun, 1985 }, -4, wxDateTime::Wed, wxDateTime::Jun, 1985 },
        { { 12, wxDateTime::Nov, 1961 }, -3, wxDateTime::Sun, wxDateTime::Nov, 1961 },
        { { 27, wxDateTime::Feb, 2093 }, -1, wxDateTime::Fri, wxDateTime::Feb, 2093 },
        { {  4, wxDateTime::Jul, 2070 }, -4, wxDateTime::Fri, wxDateTime::Jul, 2070 },
        { {  2, wxDateTime::Apr, 1906 }, -5, wxDateTime::Mon, wxDateTime::Apr, 1906 },
        { { 19, wxDateTime::Jul, 2023 }, -2, wxDateTime::Wed, wxDateTime::Jul, 2023 },
        { {  5, wxDateTime::May, 1958 }, -4, wxDateTime::Mon, wxDateTime::May, 1958 },
        { { 11, wxDateTime::Aug, 1900 },  2, wxDateTime::Sat, wxDateTime::Aug, 1900 },
        { { 14, wxDateTime::Feb, 1945 },  2, wxDateTime::Wed, wxDateTime::Feb, 1945 },
        { { 25, wxDateTime::Jul, 1967 }, -1, wxDateTime::Tue, wxDateTime::Jul, 1967 },
        { {  9, wxDateTime::May, 1916 }, -4, wxDateTime::Tue, wxDateTime::May, 1916 },
        { { 20, wxDateTime::Jun, 1927 },  3, wxDateTime::Mon, wxDateTime::Jun, 1927 },
        { {  2, wxDateTime::Aug, 2000 },  1, wxDateTime::Wed, wxDateTime::Aug, 2000 },
        { { 20, wxDateTime::Apr, 2044 },  3, wxDateTime::Wed, wxDateTime::Apr, 2044 },
        { { 20, wxDateTime::Feb, 1932 }, -2, wxDateTime::Sat, wxDateTime::Feb, 1932 },
        { { 25, wxDateTime::Jul, 2069 },  4, wxDateTime::Thu, wxDateTime::Jul, 2069 },
        { {  3, wxDateTime::Apr, 1925 },  1, wxDateTime::Fri, wxDateTime::Apr, 1925 },
        { { 21, wxDateTime::Mar, 2093 },  3, wxDateTime::Sat, wxDateTime::Mar, 2093 },
        { {  3, wxDateTime::Dec, 2074 }, -5, wxDateTime::Mon, wxDateTime::Dec, 2074 },
    };

    static const char *fmt = "%d-%b-%Y";

    wxDateTime dt;
    for ( n = 0; n < WXSIZEOF(weekDatesTestData); n++ )
    {
        const WeekDateTestData& wd = weekDatesTestData[n];

        dt.SetToWeekDay(wd.wday, wd.nWeek, wd.month, wd.year);

        printf("%s is %s", wd.Format().c_str(), dt.Format(fmt).c_str());

        const Date& d = wd.date;
        if ( d.SameDay(dt.GetTm()) )
        {
            puts(" (ok)");
        }
        else
        {
            dt.Set(d.day, d.month, d.year);

            printf(" (ERROR: should be %s)\n", dt.Format(fmt).c_str());
        }
    }
}

// test the computation of (ISO) week numbers
static void TestTimeWNumber()
{
    puts("\n*** wxDateTime week number test ***");

    struct WeekNumberTestData
    {
        Date date;                          // the date
        wxDateTime::wxDateTime_t week;      // the week number
        wxDateTime::wxDateTime_t dnum;      // day number in the year
    };

    // data generated with the following python script:
    /*
from DateTime import *
from whrandom import *
from string import *

monthNames = [ 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' ]
wdayNames = [ 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun' ]

for n in range(20):
    year = randint(1900, 2100)
    month = randint(1, 12)
    day = randint(1, 28)
    dt = DateTime(year, month, day)
    dayNum = dt.day_of_year
    weekNum = dt.iso_week[1]

    data = { 'day': rjust(`day`, 2), 'month': monthNames[month - 1], 'year': year, 'weekNum': rjust(`weekNum`, 2), 'dayNum': rjust(`dayNum`, 3) }

    print "{ { %(day)s, wxDateTime::%(month)s, %(year)d }, %(weekNum)s, "\
          "%(dayNum)s }," % data
    */
    static const WeekNumberTestData weekNumberTestDates[] =
    {
        { {  2, wxDateTime::Jul, 2093 }, 27, 183 },
        { { 25, wxDateTime::Jun, 1986 }, 26, 176 },
        { { 15, wxDateTime::Jun, 2014 }, 24, 166 },
        { { 20, wxDateTime::Jul, 2018 }, 29, 201 },
        { {  3, wxDateTime::Aug, 2074 }, 31, 215 },
        { { 26, wxDateTime::Jul, 2012 }, 30, 208 },
        { {  4, wxDateTime::Nov, 1915 }, 44, 308 },
        { { 11, wxDateTime::Feb, 2035 },  6,  42 },
        { { 15, wxDateTime::Feb, 1942 },  7,  46 },
        { {  5, wxDateTime::Jan, 2087 },  1,   5 },
        { {  6, wxDateTime::Nov, 2016 }, 44, 311 },
        { {  6, wxDateTime::Jun, 2057 }, 23, 157 },
        { { 25, wxDateTime::Feb, 1976 },  9,  56 },
        { { 12, wxDateTime::Jan, 2073 },  2,  12 },
        { { 12, wxDateTime::Sep, 2040 }, 37, 256 },
        { { 15, wxDateTime::Jul, 1931 }, 29, 196 },
        { { 23, wxDateTime::Mar, 2084 }, 12,  83 },
        { { 12, wxDateTime::Dec, 1970 }, 50, 346 },
        { {  6, wxDateTime::Sep, 1996 }, 36, 250 },
        { {  7, wxDateTime::Jan, 2076 },  2,   7 },
    };

    for ( size_t n = 0; n < WXSIZEOF(weekNumberTestDates); n++ )
    {
        const WeekNumberTestData& wn = weekNumberTestDates[n];
        const Date& d = wn.date;

        wxDateTime dt = d.DT();

        wxDateTime::wxDateTime_t week = dt.GetWeekOfYear(),
                                 dnum = dt.GetDayOfYear();

        printf("%s: the day number is %d",
               d.FormatDate().c_str(), dnum);
        if ( dnum == wn.dnum )
        {
            printf(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)", wn.dnum);
        }

        printf(", week number is %d", week);
        if ( week == wn.week )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %d)\n", wn.week);
        }
    }
}

// test DST calculations
static void TestTimeDST()
{
    puts("\n*** wxDateTime DST test ***");

    printf("DST is%s in effect now.\n\n",
           wxDateTime::Now().IsDST() ? "" : " not");

    // taken from http://www.energy.ca.gov/daylightsaving.html
    static const Date datesDST[2][2004 - 1900 + 1] =
    {
        {
            { 1, wxDateTime::Apr, 1990 },
            { 7, wxDateTime::Apr, 1991 },
            { 5, wxDateTime::Apr, 1992 },
            { 4, wxDateTime::Apr, 1993 },
            { 3, wxDateTime::Apr, 1994 },
            { 2, wxDateTime::Apr, 1995 },
            { 7, wxDateTime::Apr, 1996 },
            { 6, wxDateTime::Apr, 1997 },
            { 5, wxDateTime::Apr, 1998 },
            { 4, wxDateTime::Apr, 1999 },
            { 2, wxDateTime::Apr, 2000 },
            { 1, wxDateTime::Apr, 2001 },
            { 7, wxDateTime::Apr, 2002 },
            { 6, wxDateTime::Apr, 2003 },
            { 4, wxDateTime::Apr, 2004 },
        },
        {
            { 28, wxDateTime::Oct, 1990 },
            { 27, wxDateTime::Oct, 1991 },
            { 25, wxDateTime::Oct, 1992 },
            { 31, wxDateTime::Oct, 1993 },
            { 30, wxDateTime::Oct, 1994 },
            { 29, wxDateTime::Oct, 1995 },
            { 27, wxDateTime::Oct, 1996 },
            { 26, wxDateTime::Oct, 1997 },
            { 25, wxDateTime::Oct, 1998 },
            { 31, wxDateTime::Oct, 1999 },
            { 29, wxDateTime::Oct, 2000 },
            { 28, wxDateTime::Oct, 2001 },
            { 27, wxDateTime::Oct, 2002 },
            { 26, wxDateTime::Oct, 2003 },
            { 31, wxDateTime::Oct, 2004 },
        }
    };

    int year;
    for ( year = 1990; year < 2005; year++ )
    {
        wxDateTime dtBegin = wxDateTime::GetBeginDST(year, wxDateTime::USA),
                   dtEnd = wxDateTime::GetEndDST(year, wxDateTime::USA);

        printf("DST period in the US for year %d: from %s to %s",
               year, dtBegin.Format().c_str(), dtEnd.Format().c_str());

        size_t n = year - 1990;
        const Date& dBegin = datesDST[0][n];
        const Date& dEnd = datesDST[1][n];

        if ( dBegin.SameDay(dtBegin.GetTm()) && dEnd.SameDay(dtEnd.GetTm()) )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s %d to %s %d)\n",
                    wxDateTime::GetMonthName(dBegin.month).c_str(), dBegin.day,
                    wxDateTime::GetMonthName(dEnd.month).c_str(), dEnd.day);
        }
    }

    puts("");

    for ( year = 1990; year < 2005; year++ )
    {
        printf("DST period in Europe for year %d: from %s to %s\n",
               year,
               wxDateTime::GetBeginDST(year, wxDateTime::Country_EEC).Format().c_str(),
               wxDateTime::GetEndDST(year, wxDateTime::Country_EEC).Format().c_str());
    }
}

// test wxDateTime -> text conversion
static void TestTimeFormat()
{
    puts("\n*** wxDateTime formatting test ***");

    // some information may be lost during conversion, so store what kind
    // of info should we recover after a round trip
    enum CompareKind
    {
        CompareNone,        // don't try comparing
        CompareBoth,        // dates and times should be identical
        CompareDate,        // dates only
        CompareTime         // time only
    };

    static const struct
    {
        CompareKind compareKind;
        const char *format;
    } formatTestFormats[] =
    {
       { CompareBoth, "---> %c" },
       { CompareDate, "Date is %A, %d of %B, in year %Y" },
       { CompareBoth, "Date is %x, time is %X" },
       { CompareTime, "Time is %H:%M:%S or %I:%M:%S %p" },
       { CompareNone, "The day of year: %j, the week of year: %W" },
    };

    static const Date formatTestDates[] =
    {
        { 29, wxDateTime::May, 1976, 18, 30, 00 },
        { 31, wxDateTime::Dec, 1999, 23, 30, 00 },
#if 0
        // this test can't work for other centuries because it uses two digit
        // years in formats, so don't even try it
        { 29, wxDateTime::May, 2076, 18, 30, 00 },
        { 29, wxDateTime::Feb, 2400, 02, 15, 25 },
        { 01, wxDateTime::Jan,  -52, 03, 16, 47 },
#endif
    };

    // an extra test (as it doesn't depend on date, don't do it in the loop)
    printf("%s\n", wxDateTime::Now().Format("Our timezone is %Z").c_str());

    for ( size_t d = 0; d < WXSIZEOF(formatTestDates) + 1; d++ )
    {
        puts("");

        wxDateTime dt = d == 0 ? wxDateTime::Now() : formatTestDates[d - 1].DT();
        for ( size_t n = 0; n < WXSIZEOF(formatTestFormats); n++ )
        {
            wxString s = dt.Format(formatTestFormats[n].format);
            printf("%s", s.c_str());

            // what can we recover?
            int kind = formatTestFormats[n].compareKind;

            // convert back
            wxDateTime dt2;
            const wxChar *result = dt2.ParseFormat(s, formatTestFormats[n].format);
            if ( !result )
            {
                // converion failed - should it have?
                if ( kind == CompareNone )
                    puts(" (ok)");
                else
                    puts(" (ERROR: conversion back failed)");
            }
            else if ( *result )
            {
                // should have parsed the entire string
                puts(" (ERROR: conversion back stopped too soon)");
            }
            else
            {
                bool equal = FALSE; // suppress compilaer warning
                switch ( kind )
                {
                    case CompareBoth:
                        equal = dt2 == dt;
                        break;

                    case CompareDate:
                        equal = dt.IsSameDate(dt2);
                        break;

                    case CompareTime:
                        equal = dt.IsSameTime(dt2);
                        break;
                }

                if ( !equal )
                {
                    printf(" (ERROR: got back '%s' instead of '%s')\n",
                           dt2.Format().c_str(), dt.Format().c_str());
                }
                else
                {
                    puts(" (ok)");
                }
            }
        }
    }
}

// test text -> wxDateTime conversion
static void TestTimeParse()
{
    puts("\n*** wxDateTime parse test ***");

    struct ParseTestData
    {
        const char *format;
        Date date;
        bool good;
    };

    static const ParseTestData parseTestDates[] =
    {
        { "Sat, 18 Dec 1999 00:46:40 +0100", { 18, wxDateTime::Dec, 1999, 00, 46, 40 }, TRUE },
        { "Wed, 1 Dec 1999 05:17:20 +0300",  {  1, wxDateTime::Dec, 1999, 03, 17, 20 }, TRUE },
    };

    for ( size_t n = 0; n < WXSIZEOF(parseTestDates); n++ )
    {
        const char *format = parseTestDates[n].format;

        printf("%s => ", format);

        wxDateTime dt;
        if ( dt.ParseRfc822Date(format) )
        {
            printf("%s ", dt.Format().c_str());

            if ( parseTestDates[n].good )
            {
                wxDateTime dtReal = parseTestDates[n].date.DT();
                if ( dt == dtReal )
                {
                    puts("(ok)");
                }
                else
                {
                    printf("(ERROR: should be %s)\n", dtReal.Format().c_str());
                }
            }
            else
            {
                puts("(ERROR: bad format)");
            }
        }
        else
        {
            printf("bad format (%s)\n",
                   parseTestDates[n].good ? "ERROR" : "ok");
        }
    }
}

#if 0

// test compatibility with the old wxDate/wxTime classes
static void TestTimeCompatibility()
{
    puts("\n*** wxDateTime compatibility test ***");

    printf("wxDate for JDN 0: %s\n", wxDate(0l).FormatDate().c_str());
    printf("wxDate for MJD 0: %s\n", wxDate(2400000).FormatDate().c_str());

    double jdnNow = wxDateTime::Now().GetJDN();
    long jdnMidnight = (long)(jdnNow - 0.5);
    printf("wxDate for today: %s\n", wxDate(jdnMidnight).FormatDate().c_str());

    jdnMidnight = wxDate().Set().GetJulianDate();
    printf("wxDateTime for today: %s\n",
            wxDateTime((double)(jdnMidnight + 0.5)).Format("%c", wxDateTime::GMT0).c_str());

    int flags = wxEUROPEAN;//wxFULL;
    wxDate date;
    date.Set();
    printf("Today is %s\n", date.FormatDate(flags).c_str());
    for ( int n = 0; n < 7; n++ )
    {
        printf("Previous %s is %s\n",
               wxDateTime::GetWeekDayName((wxDateTime::WeekDay)n),
               date.Previous(n + 1).FormatDate(flags).c_str());
    }
}

#endif // 0

#endif // TEST_TIME

// ----------------------------------------------------------------------------
// threads
// ----------------------------------------------------------------------------

#ifdef TEST_THREADS

#include <wx/thread.h>

static size_t gs_counter = (size_t)-1;
static wxCriticalSection gs_critsect;
static wxCondition gs_cond;

class MyJoinableThread : public wxThread
{
public:
    MyJoinableThread(size_t n) : wxThread(wxTHREAD_JOINABLE)
        { m_n = n; Create(); }

    // thread execution starts here
    virtual ExitCode Entry();

private:
    size_t m_n;
};

wxThread::ExitCode MyJoinableThread::Entry()
{
    unsigned long res = 1;
    for ( size_t n = 1; n < m_n; n++ )
    {
        res *= n;

        // it's a loooong calculation :-)
        Sleep(100);
    }

    return (ExitCode)res;
}

class MyDetachedThread : public wxThread
{
public:
    MyDetachedThread(size_t n, char ch)
    {
        m_n = n;
        m_ch = ch;
        m_cancelled = FALSE;

        Create();
    }

    // thread execution starts here
    virtual ExitCode Entry();

    // and stops here
    virtual void OnExit();

private:
    size_t m_n; // number of characters to write
    char m_ch;  // character to write

    bool m_cancelled;   // FALSE if we exit normally
};

wxThread::ExitCode MyDetachedThread::Entry()
{
    {
        wxCriticalSectionLocker lock(gs_critsect);
        if ( gs_counter == (size_t)-1 )
            gs_counter = 1;
        else
            gs_counter++;
    }

    for ( size_t n = 0; n < m_n; n++ )
    {
        if ( TestDestroy() )
        {
            m_cancelled = TRUE;

            break;
        }

        putchar(m_ch);
        fflush(stdout);

        wxThread::Sleep(100);
    }

    return 0;
}

void MyDetachedThread::OnExit()
{
    wxLogTrace("thread", "Thread %ld is in OnExit", GetId());

    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter && !m_cancelled )
        gs_cond.Signal();
}

void TestDetachedThreads()
{
    puts("\n*** Testing detached threads ***");

    static const size_t nThreads = 3;
    MyDetachedThread *threads[nThreads];
    size_t n;
    for ( n = 0; n < nThreads; n++ )
    {
        threads[n] = new MyDetachedThread(10, 'A' + n);
    }

    threads[0]->SetPriority(WXTHREAD_MIN_PRIORITY);
    threads[1]->SetPriority(WXTHREAD_MAX_PRIORITY);

    for ( n = 0; n < nThreads; n++ )
    {
        threads[n]->Run();
    }

    // wait until all threads terminate
    gs_cond.Wait();

    puts("");
}

void TestJoinableThreads()
{
    puts("\n*** Testing a joinable thread (a loooong calculation...) ***");

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    printf("\nThread terminated with exit code %lu.\n",
           (unsigned long)thread.Wait());
}

void TestThreadSuspend()
{
    puts("\n*** Testing thread suspend/resume functions ***");

    MyDetachedThread *thread = new MyDetachedThread(15, 'X');

    thread->Run();

    // this is for this demo only, in a real life program we'd use another
    // condition variable which would be signaled from wxThread::Entry() to
    // tell us that the thread really started running - but here just wait a
    // bit and hope that it will be enough (the problem is, of course, that
    // the thread might still not run when we call Pause() which will result
    // in an error)
    wxThread::Sleep(300);

    for ( size_t n = 0; n < 3; n++ )
    {
        thread->Pause();

        puts("\nThread suspended");
        if ( n > 0 )
        {
            // don't sleep but resume immediately the first time
            wxThread::Sleep(300);
        }
        puts("Going to resume the thread");

        thread->Resume();
    }

    puts("Waiting until it terminates now");

    // wait until the thread terminates
    gs_cond.Wait();

    puts("");
}

void TestThreadDelete()
{
    // As above, using Sleep() is only for testing here - we must use some
    // synchronisation object instead to ensure that the thread is still
    // running when we delete it - deleting a detached thread which already
    // terminated will lead to a crash!

    puts("\n*** Testing thread delete function ***");

    MyDetachedThread *thread0 = new MyDetachedThread(30, 'W');

    thread0->Delete();

    puts("\nDeleted a thread which didn't start to run yet.");

    MyDetachedThread *thread1 = new MyDetachedThread(30, 'Y');

    thread1->Run();

    wxThread::Sleep(300);

    thread1->Delete();

    puts("\nDeleted a running thread.");

    MyDetachedThread *thread2 = new MyDetachedThread(30, 'Z');

    thread2->Run();

    wxThread::Sleep(300);

    thread2->Pause();

    thread2->Delete();

    puts("\nDeleted a sleeping thread.");

    MyJoinableThread thread3(20);
    thread3.Run();

    thread3.Delete();

    puts("\nDeleted a joinable thread.");

    MyJoinableThread thread4(2);
    thread4.Run();

    wxThread::Sleep(300);

    thread4.Delete();

    puts("\nDeleted a joinable thread which already terminated.");

    puts("");
}

#endif // TEST_THREADS

// ----------------------------------------------------------------------------
// arrays
// ----------------------------------------------------------------------------

#ifdef TEST_ARRAYS

void PrintArray(const char* name, const wxArrayString& array)
{
    printf("Dump of the array '%s'\n", name);

    size_t nCount = array.GetCount();
    for ( size_t n = 0; n < nCount; n++ )
    {
        printf("\t%s[%u] = '%s'\n", name, n, array[n].c_str());
    }
}

#endif // TEST_ARRAYS

// ----------------------------------------------------------------------------
// strings
// ----------------------------------------------------------------------------

#ifdef TEST_STRINGS

#include "wx/timer.h"

static void TestString()
{
    wxStopWatch sw;

    wxString a, b, c;

    a.reserve (128);
    b.reserve (128);
    c.reserve (128);

    for (int i = 0; i < 1000000; ++i)
    {
        a = "Hello";
        b = " world";
        c = "! How'ya doin'?";
        a += b;
        a += c;
        c = "Hello world! What's up?";
        if (c != a)
            c = "Doh!";
    }

    printf ("TestString elapsed time: %ld\n", sw.Time());
}

static void TestPChar()
{
    wxStopWatch sw;

    char a [128];
    char b [128];
    char c [128];

    for (int i = 0; i < 1000000; ++i)
    {
        strcpy (a, "Hello");
        strcpy (b, " world");
        strcpy (c, "! How'ya doin'?");
        strcat (a, b);
        strcat (a, c);
        strcpy (c, "Hello world! What's up?");
        if (strcmp (c, a) == 0)
            strcpy (c, "Doh!");
    }

    printf ("TestPChar elapsed time: %ld\n", sw.Time());
}

static void TestStringSub()
{
    wxString s("Hello, world!");

    puts("*** Testing wxString substring extraction ***");

    printf("String = '%s'\n", s.c_str());
    printf("Left(5) = '%s'\n", s.Left(5).c_str());
    printf("Right(6) = '%s'\n", s.Right(6).c_str());
    printf("Mid(3, 5) = '%s'\n", s(3, 5).c_str());
    printf("Mid(3) = '%s'\n", s.Mid(3).c_str());
    printf("substr(3, 5) = '%s'\n", s.substr(3, 5).c_str());
    printf("substr(3) = '%s'\n", s.substr(3).c_str());

    puts("");
}

static void TestStringFormat()
{
    puts("*** Testing wxString formatting ***");

    wxString s;
    s.Printf("%03d", 18);

    printf("Number 18: %s\n", wxString::Format("%03d", 18).c_str());
    printf("Number 18: %s\n", s.c_str());

    puts("");
}

#endif // TEST_STRINGS

// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if ( !wxInitialize() )
    {
        fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");
    }

#ifdef TEST_STRINGS
    if ( 0 )
    {
        TestPChar();
        TestString();
    }
    if ( 0 )
    {
        TestStringSub();
    }
    TestStringFormat();
#endif // TEST_STRINGS

#ifdef TEST_ARRAYS
    wxArrayString a1;
    a1.Add("tiger");
    a1.Add("cat");
    a1.Add("lion");
    a1.Add("dog");
    a1.Add("human");
    a1.Add("ape");

    puts("*** Initially:");

    PrintArray("a1", a1);

    wxArrayString a2(a1);
    PrintArray("a2", a2);

    wxSortedArrayString a3(a1);
    PrintArray("a3", a3);

    puts("*** After deleting a string from a1");
    a1.Remove(2);

    PrintArray("a1", a1);
    PrintArray("a2", a2);
    PrintArray("a3", a3);

    puts("*** After reassigning a1 to a2 and a3");
    a3 = a2 = a1;
    PrintArray("a2", a2);
    PrintArray("a3", a3);
#endif // TEST_ARRAYS

#ifdef TEST_DIR
    TestDirEnum();
#endif // TEST_DIR

#ifdef TEST_LOG
    wxString s;
    for ( size_t n = 0; n < 8000; n++ )
    {
        s << (char)('A' + (n % 26));
    }

    wxString msg;
    msg.Printf("A very very long message: '%s', the end!\n", s.c_str());

    // this one shouldn't be truncated
    printf(msg);

    // but this one will because log functions use fixed size buffer
    // (note that it doesn't need '\n' at the end neither - will be added
    //  by wxLog anyhow)
    wxLogMessage("A very very long message 2: '%s', the end!", s.c_str());
#endif // TEST_LOG

#ifdef TEST_THREADS
    int nCPUs = wxThread::GetCPUCount();
    printf("This system has %d CPUs\n", nCPUs);
    if ( nCPUs != -1 )
        wxThread::SetConcurrency(nCPUs);

    if ( argc > 1 && argv[1][0] == 't' )
        wxLog::AddTraceMask("thread");

    if ( 1 )
        TestDetachedThreads();
    if ( 1 )
        TestJoinableThreads();
    if ( 1 )
        TestThreadSuspend();
    if ( 1 )
        TestThreadDelete();

#endif // TEST_THREADS

#ifdef TEST_LONGLONG
    if ( 0 )
        TestSpeed();
    if ( 1 )
        TestDivision();
#endif // TEST_LONGLONG

#ifdef TEST_MIME
    TestMimeEnum();
#endif // TEST_MIME

#ifdef TEST_TIME
    if ( 0 )
    {
    TestTimeSet();
    TestTimeStatic();
    TestTimeZones();
    TestTimeRange();
    TestTimeTicks();
    TestTimeJDN();
    TestTimeDST();
    TestTimeWDays();
    TestTimeWNumber();
    TestTimeParse();
    }

    TestTimeFormat();
#endif // TEST_TIME

    wxUninitialize();

    return 0;
}
