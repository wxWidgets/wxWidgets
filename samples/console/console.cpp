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
//#define TEST_LOG
//#define TEST_STRINGS
//#define TEST_THREADS
#define TEST_TIME
//#define TEST_LONGLONG

// ============================================================================
// implementation
// ============================================================================

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

    printf("Current time:\t%s\n", wxDateTime::Now().Format().c_str());
    printf("Unix epoch:\t%s\n", wxDateTime((time_t)0).Format().c_str());
    printf("Today noon:\t%s\n", wxDateTime(12, 0).Format().c_str());
    printf("May 29, 1976:\t%s\n", wxDateTime(29, wxDateTime::May, 1976).Format().c_str());
}

// test time zones stuff
static void TestTimeZones()
{
    puts("\n*** wxDateTime timezone test ***");

    wxDateTime now = wxDateTime::Now();

    printf("Current GMT time:\t%s\n", now.ToGMT().Format().c_str());
    printf("Unix epoch (GMT):\t%s\n", wxDateTime((time_t)0).MakeGMT().Format().c_str());
    printf("Current time in Paris:\t%s\n", now.ToTimezone(wxDateTime::CET).Format().c_str());
    printf("               Moscow:\t%s\n", now.ToTimezone(wxDateTime::MSK).Format().c_str());
    printf("             New York:\t%s\n", now.ToTimezone(wxDateTime::EST).Format().c_str());
}

// test some minimal support for the dates outside the standard range
static void TestTimeRange()
{
    puts("\n*** wxDateTime out-of-standard-range dates test ***");

    printf("Unix epoch:\t%s\n",
           wxDateTime(2440587.5).Format().c_str());
    printf("Feb 29, 0: \t%s\n",
            wxDateTime(29, wxDateTime::Feb, 0).Format().c_str());
    printf("JDN 0:     \t%s\n",
            wxDateTime(0.0).Format().c_str());
    printf("Jan 1, 1AD:\t%s\n",
            wxDateTime(1, wxDateTime::Jan, 1).Format().c_str());
    printf("May 29, 2099:\t%s\n",
            wxDateTime(29, wxDateTime::May, 2099).Format().c_str());
}

// test conversions to JDN &c
static void TestTimeJDN()
{
    puts("\n*** wxDateTime to JDN test ***");

    struct Date
    {
        wxDateTime::wxDateTime_t day;
        wxDateTime::Month month;
        int year;
        double jdn;
    };

    static const Date testDates[] =
    {
        { 21, wxDateTime::Jan,  2222, 2532648.5 },
        { 29, wxDateTime::May,  1976, 2442927.5 },
        { 1,  wxDateTime::Jan,  1970, 2440587.5 },
        { 1,  wxDateTime::Jan,  1900, 2415020.5 },
        { 15, wxDateTime::Oct,  1582, 2299160.5 },
        { 4,  wxDateTime::Oct,  1582, 2299149.5 },
        { 1,  wxDateTime::Mar,     1, 1721484.5 },
        { 1,  wxDateTime::Jan,     1, 1721425.5 },
        { 31, wxDateTime::Dec,     0, 1721424.5 },
        { 1,  wxDateTime::Jan,     0, 1721059.5 },
        { 12, wxDateTime::Aug, -1234, 1270573.5 },
        { 12, wxDateTime::Aug, -4000,  260313.5 },
        { 24, wxDateTime::Nov, -4713,      -0.5 },
    };

    for ( size_t n = 0; n < WXSIZEOF(testDates); n++ )
    {
        const Date& d = testDates[n];
        wxDateTime dt(d.day, d.month, d.year);
        double jdn = dt.GetJulianDayNumber();

        printf("JDN of %s %02d, %4d%s is:\t%f",
               wxDateTime::GetMonthName(d.month).c_str(),
               d.day,
               wxDateTime::ConvertYearToBC(d.year),
               d.year > 0 ? "AD" : "BC",
               jdn);
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

void TestString()
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

void TestPChar()
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
    TestPChar();
    TestString();
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

#ifdef TEST_TIME
    TestTimeStatic();
    TestTimeSet();
    TestTimeZones();
    TestTimeRange();
    TestTimeJDN();
#endif // TEST_TIME

    wxUninitialize();

    return 0;
}
