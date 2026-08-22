///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/misc.cpp
// Purpose:     Miscellaneous wxThread test cases
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-10
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/thread.h"
#include "wx/utils.h"

#include <memory>
#include <vector>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

static size_t gs_counter = (size_t)-1;
static wxCriticalSection gs_critsect;
static wxSemaphore gs_cond;

class MyJoinableThread : public wxThread
{
public:
    MyJoinableThread(size_t n) : wxThread(wxTHREAD_JOINABLE)
        { m_n = n; Create(); }

    // thread execution starts here
    virtual ExitCode Entry() override;

private:
    size_t m_n;
};

wxThread::ExitCode MyJoinableThread::Entry()
{
    wxUIntPtr res = 1;
    for ( size_t n = 1; n < m_n; n++ )
    {
        res *= n;

        // it's a loooong calculation :-)
        wxMilliSleep(100);
    }

    return (ExitCode)res;
}

class MyDetachedThread : public wxThread
{
public:
    // If n == 0, thread must be cancelled to exit.
    MyDetachedThread(size_t n, wxChar ch)
    {
        m_n = n;
        m_ch = ch;
        m_cancelled = false;

        Create();
    }

    // thread execution starts here
    virtual ExitCode Entry() override;

    // and stops here
    virtual void OnExit() override;

private:
    size_t m_n; // number of characters to write
    wxChar m_ch;  // character to write

    bool m_cancelled;   // false if we exit normally
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

    for ( size_t n = 0;; n++ )
    {
        if ( m_n && n == m_n )
            break;

        if ( TestDestroy() )
        {
            m_cancelled = true;

            break;
        }

        //wxPutchar(m_ch);
        //fflush(stdout);

        wxMilliSleep(100);
    }

    return nullptr;
}

void MyDetachedThread::OnExit()
{
    //wxLogTrace(wxT("thread"), wxT("Thread %ld is in OnExit"), GetId());

    wxCriticalSectionLocker lock(gs_critsect);
    if ( !--gs_counter && !m_cancelled )
        gs_cond.Post();
}

class MyWaitingThread : public wxThread
{
public:
    MyWaitingThread( wxMutex *mutex, wxCondition *condition )
    {
        m_mutex = mutex;
        m_condition = condition;

        Create();
    }

    virtual ExitCode Entry() override
    {
        //wxPrintf(wxT("Thread %lu has started running.\n"), GetId());
        gs_cond.Post();

        //wxPrintf(wxT("Thread %lu starts to wait...\n"), GetId());

        m_mutex->Lock();
        m_condition->Wait();
        m_mutex->Unlock();

        //wxPrintf(wxT("Thread %lu finished to wait, exiting.\n"), GetId());
        gs_cond.Post();

        return nullptr;
    }

private:
    wxMutex *m_mutex;
    wxCondition *m_condition;
};

// semaphore tests
#include "wx/datetime.h"

class MySemaphoreThread : public wxThread
{
public:
    MySemaphoreThread(int i, wxSemaphore *sem)
        : wxThread(wxTHREAD_JOINABLE),
          m_sem(sem),
          m_i(i)
    {
        Create();
    }

    virtual ExitCode Entry() override
    {
        wxUnusedVar(m_i);

        //wxPrintf(wxT("%s: Thread #%d (%ld) starting to wait for semaphore...\n"),
        //         wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        m_sem->Wait();

        //wxPrintf(wxT("%s: Thread #%d (%ld) acquired the semaphore.\n"),
        //         wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        Sleep(1000);

        //wxPrintf(wxT("%s: Thread #%d (%ld) releasing the semaphore.\n"),
        //         wxDateTime::Now().FormatTime().c_str(), m_i, (long)GetId());

        m_sem->Post();

        return nullptr;
    }

private:
    wxSemaphore *m_sem;
    int m_i;
};

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

// Use all the available CPUs for the threads created by the tests below.
static const struct SetConcurrency
{
    SetConcurrency()
    {
        int nCPUs = wxThread::GetCPUCount();
        if ( nCPUs != -1 )
            wxThread::SetConcurrency(nCPUs);
    }
} gs_setConcurrency;

TEST_CASE("MiscThread::Joinable", "[thread]")
{
    // calc 10! in the background
    MyJoinableThread thread(10);
    CHECK( thread.Run() == wxTHREAD_NO_ERROR );
    CHECK( (wxUIntPtr)thread.Wait() == 362880 );
}

TEST_CASE("MiscThread::Detached", "[thread]")
{
    static const size_t nThreads = 3;
    MyDetachedThread *threads[nThreads];

    size_t n;
    for ( n = 0; n < nThreads; n++ )
    {
        threads[n] = new MyDetachedThread(10, 'A' + n);
    }

    threads[0]->SetPriority(wxPRIORITY_MIN);
    threads[1]->SetPriority(wxPRIORITY_MAX);

    for ( n = 0; n < nThreads; n++ )
    {
        CHECK( threads[n]->Run() == wxTHREAD_NO_ERROR );
    }

    // wait until all threads terminate
    CHECK( gs_cond.Wait() == wxSEMA_NO_ERROR );
}

TEST_CASE("MiscThread::Semaphore", "[thread]")
{
    static const int SEM_LIMIT = 3;

    wxSemaphore sem(SEM_LIMIT, SEM_LIMIT);
    std::vector<std::unique_ptr<MySemaphoreThread>> threads;

    for ( int i = 0; i < 3*SEM_LIMIT; i++ )
    {
        std::unique_ptr<MySemaphoreThread> t{new MySemaphoreThread(i, &sem)};
        CHECK( t->Run() == wxTHREAD_NO_ERROR );

        threads.push_back(std::move(t));
    }

    for ( auto& t : threads )
    {
        CHECK( (wxUIntPtr)t->Wait() == 0 );
    }
}

TEST_CASE("MiscThread::ThreadSuspend", "[thread]")
{
    MyDetachedThread *thread = new MyDetachedThread(15, 'X');

    CHECK( thread->Run() == wxTHREAD_NO_ERROR );

    // this is for this demo only, in a real life program we'd use another
    // condition variable which would be signaled from wxThread::Entry() to
    // tell us that the thread really started running - but here just wait a
    // bit and hope that it will be enough (the problem is, of course, that
    // the thread might still not run when we call Pause() which will result
    // in an error)
    wxMilliSleep(300);

    for ( size_t n = 0; n < 3; n++ )
    {
        thread->Pause();

        if ( n > 0 )
        {
            // don't sleep but resume immediately the first time
            wxMilliSleep(300);
        }

        CHECK( thread->Resume() == wxTHREAD_NO_ERROR );
    }

    // wait until the thread terminates
    CHECK( gs_cond.Wait() == wxSEMA_NO_ERROR );
}

TEST_CASE("MiscThread::ThreadDelete", "[thread]")
{
    // Check that deleting a thread which didn't start to run yet returns an
    // error.
    MyDetachedThread *thread0 = new MyDetachedThread(0, 'W');
    CHECK( thread0->Delete() == wxTHREAD_MISC_ERROR );

    // Check that deleting a running thread works.
    MyDetachedThread *thread1 = new MyDetachedThread(0, 'X');
    CHECK( thread1->Run() == wxTHREAD_NO_ERROR );
    CHECK( thread1->Delete() == wxTHREAD_NO_ERROR );

    // Create another thread and pause it before deleting.
    MyDetachedThread *thread2 = new MyDetachedThread(0, 'Z');
    CHECK( thread2->Run() == wxTHREAD_NO_ERROR );
    CHECK( thread2->Pause() == wxTHREAD_NO_ERROR );
    CHECK( thread2->Delete() == wxTHREAD_NO_ERROR );

    // Delete a running joinable thread.
    MyJoinableThread thread3(20);
    CHECK( thread3.Run() == wxTHREAD_NO_ERROR );
    CHECK( thread3.Delete() == wxTHREAD_NO_ERROR );

    // Delete a joinable thread which already terminated.
    MyJoinableThread thread4(2);
    CHECK( thread4.Run() == wxTHREAD_NO_ERROR );
    thread4.Wait();
    CHECK( thread4.Delete() == wxTHREAD_NO_ERROR );
}

TEST_CASE("MiscThread::ThreadRun", "[thread]")
{
    MyJoinableThread thread1(2);
    CHECK( thread1.Run() == wxTHREAD_NO_ERROR );
    thread1.Wait();     // wait until the thread ends

    // verify that running twice the same thread fails
    WX_ASSERT_FAILS_WITH_ASSERT( thread1.Run() );
}

TEST_CASE("MiscThread::ThreadConditions", "[thread]")
{
    wxMutex mutex;
    wxCondition condition(mutex);

    // otherwise its difficult to understand which log messages pertain to
    // which condition
    //wxLogTrace(wxT("thread"), wxT("Local condition var is %08x, gs_cond = %08x"),
    //           condition.GetId(), gs_cond.GetId());

    // create and launch threads
    MyWaitingThread *threads[10];

    size_t n;
    for ( n = 0; n < WXSIZEOF(threads); n++ )
    {
        threads[n] = new MyWaitingThread( &mutex, &condition );
    }

    for ( n = 0; n < WXSIZEOF(threads); n++ )
    {
        CHECK( threads[n]->Run() == wxTHREAD_NO_ERROR );
    }

    // wait until all threads run
    size_t nRunning = 0;
    while ( nRunning < WXSIZEOF(threads) )
    {
        CHECK( gs_cond.Wait() == wxSEMA_NO_ERROR );

        nRunning++;
    }

    wxMilliSleep(500);

    // now wake one of them up
    CHECK( condition.Signal() == wxCOND_NO_ERROR );

    CHECK( gs_cond.Wait() == wxSEMA_NO_ERROR );
    size_t nFinished = 1;

    // wake all the (remaining) threads up, so that they can exit
    CHECK( condition.Broadcast() == wxCOND_NO_ERROR );

    while ( nFinished < WXSIZEOF(threads) )
    {
        CHECK( gs_cond.Wait() == wxSEMA_NO_ERROR );

        nFinished++;
    }
}
