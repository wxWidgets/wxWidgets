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

#include <stdio.h>

#include <wx/string.h>
#include <wx/file.h>
#include <wx/app.h>
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

int main(int argc, char **argv)
{
    if ( !wxInitialize() )
    {
        fprintf(stderr, "Failed to initialize the wxWindows library, aborting.");
    }

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

    wxUninitialize();

    return 0;
}
