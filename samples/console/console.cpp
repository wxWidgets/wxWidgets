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
#define TEST_ARRAYS
#undef TEST_THREADS

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

class MyThread : public wxThread
{
public:
    MyThread(char ch);

    // thread execution starts here
    virtual void *Entry();

    // and stops here
    virtual void OnExit();

public:
    char m_ch;
};

MyThread::MyThread(char ch)
{
    m_ch = ch;

    Create();
}

void *MyThread::Entry()
{
    {
        wxCriticalSectionLocker lock(gs_critsect);
        if ( gs_counter == (size_t)-1 )
            gs_counter = 1;
        else
            gs_counter++;
    }

    for ( size_t n = 0; n < 10; n++ )
    {
        if ( TestDestroy() )
            break;

        putchar(m_ch);
        fflush(stdout);

        wxThread::Sleep(100);
    }

    return NULL;
}

void MyThread::OnExit()
{
    wxCriticalSectionLocker lock(gs_critsect);
    gs_counter--;
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

#ifdef TEST_THREADS
    static const size_t nThreads = 3;
    MyThread *threads[nThreads];
    size_t n;
    for ( n = 0; n < nThreads; n++ )
    {
        threads[n] = new MyThread('+' + n);
        threads[n]->Run();
    }

    // wait until all threads terminate
    for ( ;; )
    {
        wxCriticalSectionLocker lock(gs_critsect);
        if ( !gs_counter )
            break;
    }

    puts("\nThat's all, folks!");

    for ( n = 0; n < nThreads; n++ )
    {
        threads[n]->Delete();
    }
#endif // TEST_THREADS

    wxUninitialize();

    return 0;
}
