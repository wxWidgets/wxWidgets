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
    { 31, wxDateTime::Dec,     0, 00, 00, 00, 1721424.5, wxDateTime::Tue,        -1,        -1 },
    {  1, wxDateTime::Jan,     0, 00, 00, 00, 1721059.5, wxDateTime::Wed,        -1,        -1 },
    { 12, wxDateTime::Aug, -1234, 00, 00, 00, 1270573.5, wxDateTime::Thu,        -1,        -1 },
    { 12, wxDateTime::Aug, -4000, 00, 00, 00,  260313.5, wxDateTime::Wed,        -1,        -1 },
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
           wxDateTime::GetMonthName(month, TRUE).c_str(),
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
        bool should = years[1][n] != 0;

        printf("Year %d is %sa leap year (should be: %s)\n",
               year,
               wxDateTime::IsLeapYear(year) ? "" : "not ",
               should ? "yes" : "no");

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

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year, d.hour, d.min, d.sec);

        wxDateTime::WeekDay wday = dt.GetWeekDay();
        printf("%s is: %s",
               d.Format().c_str(),
               wxDateTime::GetWeekDayName(wday));
        if ( wday == d.wday )
        {
            puts(" (ok)");
        }
        else
        {
            printf(" (ERROR: should be %s)\n",
                   wxDateTime::GetWeekDayName(d.wday));
        }
    }
}

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
    TestStringSub();
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
    }
    TestTimeWDays();
#endif // TEST_TIME

    wxUninitialize();

    return 0;
}
