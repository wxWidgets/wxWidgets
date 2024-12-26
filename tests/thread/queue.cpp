///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/queue.cpp
// Purpose:     Unit test for wxMessageQueue
// Author:      Evgeniy Tarassov
// Created:     31/10/2007
// Copyright:   (c) 2007 Evgeniy Tarassov
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/thread.h"
#endif // WX_PRECOMP

#include "wx/msgqueue.h"

// ----------------------------------------------------------------------------
// thread class used in the tests
// ----------------------------------------------------------------------------

namespace
{

enum WaitTestType
{
    WaitWithTimeout = 0,
    WaitInfinitlyLong
};

typedef wxMessageQueue<int> Queue;

// This class represents a thread that waits (following WaitTestType type)
// for exactly maxMsgCount messages from its message queue and if another
// MyThread is specified, then every message received is posted
// to that next thread.
class MyThread : public wxThread
{
public:
    MyThread(WaitTestType type, MyThread *next, int maxMsgCount)
       : wxThread(wxTHREAD_JOINABLE),
         m_type(type), m_nextThread(next), m_maxMsgCount(maxMsgCount)
    {}

    // thread execution starts here
    virtual void *Entry() override;

    // Thread message queue
    Queue& GetQueue()
    {
        return m_queue;
    }

private:
    WaitTestType m_type;
    MyThread*    m_nextThread;
    int          m_maxMsgCount;
    Queue        m_queue;
};

} // anonymous namespace


// this function creates the given number of threads and posts msgCount
// messages to the last created thread which, in turn, posts all the messages
// it receives to the previously created thread which does the same and so on
// in cascade -- at the end, each thread will have received all msgCount
// messages directly or indirectly
TEST_CASE("wxMessageQueue::Receive", "[msgqueue]")
{
    const int msgCount = 100;
    const int threadCount = 10;

    std::vector<std::unique_ptr<MyThread>> threads;

    int i;
    for ( i = 0; i < threadCount; ++i )
    {
        MyThread *previousThread = i == 0 ? nullptr : threads[i-1].get();
        std::unique_ptr<MyThread>
            thread(new MyThread(WaitInfinitlyLong, previousThread, msgCount));

        CHECK( thread->Create() == wxTHREAD_NO_ERROR );
        threads.push_back(std::move(thread));
    }

    for ( i = 0; i < threadCount; ++i )
    {
        threads[i]->Run();
    }

    MyThread* const lastThread = threads[threadCount - 1].get();

    for ( i = 0; i < msgCount; ++i )
    {
        lastThread->GetQueue().Post(i);
    }

    for ( i = 0; i < threadCount; ++i )
    {
        // each thread should return the number of messages received.
        // if it returns a negative, then it detected some problem.
        wxThread::ExitCode code = threads[i]->Wait();
        CHECK( code == (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR );
    }
}

// this function creates two threads, each one waiting (with a timeout) for
// exactly two messages.
// Exactly to messages are posted into first thread queue, but
// only one message is posted to the second thread queue.
// Therefore first thread should return with wxMSGQUEUE_NO_ERROR, but the second
// should return wxMSGQUEUUE_TIMEOUT.
TEST_CASE("wxMessageQueue::ReceiveTimeout", "[msgqueue]")
{
    std::unique_ptr<MyThread> thread1(new MyThread(WaitWithTimeout, nullptr, 2));
    std::unique_ptr<MyThread> thread2(new MyThread(WaitWithTimeout, nullptr, 2));

    CHECK( thread1->Create() == wxTHREAD_NO_ERROR );
    CHECK( thread2->Create() == wxTHREAD_NO_ERROR );

    thread1->Run();
    thread2->Run();

    // Post two messages to the first thread
    CHECK( thread1->GetQueue().Post(0) == wxMSGQUEUE_NO_ERROR );
    CHECK( thread1->GetQueue().Post(1) == wxMSGQUEUE_NO_ERROR );

    // ...but only one message to the second
    CHECK( thread2->GetQueue().Post(0) == wxMSGQUEUE_NO_ERROR );

    wxThread::ExitCode code1 = thread1->Wait();
    wxThread::ExitCode code2 = thread2->Wait();

    CHECK( code1 == (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR );
    CHECK( code2 == (wxThread::ExitCode)wxMSGQUEUE_TIMEOUT );
}

// every thread tries to read exactly m_maxMsgCount messages from its queue
// following the waiting strategy specified in m_type. If it succeeds then it
// returns 0. Otherwise it returns the error code - one of wxMessageQueueError.
void *MyThread::Entry()
{
    int messagesReceived = 0;
    while ( messagesReceived < m_maxMsgCount )
    {
        wxMessageQueueError result;
        int msg = -1; // just to suppress "possibly uninitialized" warnings

        if ( m_type == WaitWithTimeout )
            result = m_queue.ReceiveTimeout(1000, msg);
        else
            result = m_queue.Receive(msg);

        if ( result == wxMSGQUEUE_MISC_ERROR )
            return (wxThread::ExitCode)wxMSGQUEUE_MISC_ERROR;

        if ( result == wxMSGQUEUE_NO_ERROR )
        {
            if ( m_nextThread != nullptr )
            {
                wxMessageQueueError res = m_nextThread->GetQueue().Post(msg);

                if ( res == wxMSGQUEUE_MISC_ERROR )
                    return (wxThread::ExitCode)wxMSGQUEUE_MISC_ERROR;

                // We can't use Catch asserts outside of the main thread
                // currently, unfortunately.
                wxASSERT( res == wxMSGQUEUE_NO_ERROR );
            }
            ++messagesReceived;
            continue;
        }

        wxASSERT( result == wxMSGQUEUE_TIMEOUT );

        break;
    }

    if ( messagesReceived != m_maxMsgCount )
    {
        wxASSERT( m_type == WaitWithTimeout );

        return (wxThread::ExitCode)wxMSGQUEUE_TIMEOUT;
    }

    return (wxThread::ExitCode)wxMSGQUEUE_NO_ERROR;
}

TEST_CASE("wxMessageQueue::NonCopyable", "[msgqueue]")
{
    struct NonCopyable
    {
        explicit NonCopyable(int n) : m_n(new int(n)) { }

        NonCopyable(NonCopyable&& other) = default;
        NonCopyable& operator=(NonCopyable&& other) = default;

        std::unique_ptr<int> m_n;
    };

    wxMessageQueue<NonCopyable> queue;

    NonCopyable nc(17);
    CHECK( queue.Post(std::move(nc)) == wxMSGQUEUE_NO_ERROR );

    NonCopyable nc2(0);
    CHECK( queue.Receive(nc2) == wxMSGQUEUE_NO_ERROR );
    CHECK( *nc2.m_n == 17 );

    CHECK( queue.ReceiveTimeout(0, nc2) == wxMSGQUEUE_TIMEOUT );
}
