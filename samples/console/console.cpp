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
#define TEST_THREADS
//#define TEST_TIME
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
    wxLongLong ll = 0x38417388; // some number < LONG_MAX

    wxASSERT( (ll / 1000l)*1000l == ll );
}

#endif // TEST_LONGLONG

// ----------------------------------------------------------------------------
// date time
// ----------------------------------------------------------------------------

#ifdef TEST_TIME

#include <wx/datetime.h>

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
    MyDetachedThread(size_t n, char ch) { m_n = n; m_ch = ch; Create(); }

    // thread execution starts here
    virtual ExitCode Entry();

    // and stops here
    virtual void OnExit();

private:
    size_t m_n; // number of characters to write
    char m_ch;  // character to write
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
            break;

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
    if ( !--gs_counter )
        gs_cond.Signal();
}

void TestDetachedThreads()
{
    puts("*** Testing detached threads ***");

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
    puts("*** Testing a joinable thread (a loooong calculation...) ***");

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    printf("\nThread terminated with exit code %lu.\n",
           (unsigned long)thread.Wait());
}

void TestThreadSuspend()
{
    MyDetachedThread *thread = new MyDetachedThread(30, 'X');

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

    // wait until the thread terminates
    gs_cond.Wait();

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

    TestThreadSuspend();
    if ( 0 )
    {
    TestDetachedThreads();
    TestJoinableThreads();
    }
#endif // TEST_THREADS

#ifdef TEST_LONGLONG
    if ( 0 )
        TestSpeed();
    if ( 1 )
        TestDivision();
#endif // TEST_LONGLONG

#ifdef TEST_TIME
    wxDateTime time = wxDateTime::Now();
    printf("Current time: '%s', current year %u is %sa leap one",
           time.Format().c_str(),
           time.GetYear(),
           wxDateTime::IsLeapYear(time.GetYear()) ? "" : "not");
#endif // TEST_TIME

    wxUninitialize();

    return 0;
}
