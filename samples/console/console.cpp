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
#define TEST_THREADS

// ============================================================================
// implementation
// ============================================================================

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
    MyDetachedThread(char ch) { m_ch = ch; Create(); }

    // thread execution starts here
    virtual ExitCode Entry();

    // and stops here
    virtual void OnExit();

private:
    char m_ch;
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

    static const size_t nIter = 10;
    for ( size_t n = 0; n < nIter; n++ )
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
    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter )
        gs_cond.Signal();
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
// entry point
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if ( !wxInitialize() )
    {
        fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");
    }

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
    puts("Testing detached threads...");

    static const size_t nThreads = 3;
    MyDetachedThread *threads[nThreads];
    size_t n;
    for ( n = 0; n < nThreads; n++ )
    {
        threads[n] = new MyDetachedThread('A' + n);
    }

    threads[0]->SetPriority(WXTHREAD_MIN_PRIORITY);
    threads[1]->SetPriority(WXTHREAD_MAX_PRIORITY);

    for ( n = 0; n < nThreads; n++ )
    {
        threads[n]->Run();
    }

    // wait until all threads terminate
    wxMutex mutex;
    mutex.Lock();
    gs_cond.Wait(mutex);
    mutex.Unlock();

    puts("\n\nTesting a joinable thread used for a loooong calculation...");

    // calc 10! in the background
    MyJoinableThread thread(10);
    thread.Run();

    printf("\nThread terminated with exit code %lu.\n",
           (unsigned long)thread.Wait());
#endif // TEST_THREADS

    wxUninitialize();

    return 0;
}
