/////////////////////////////////////////////////////////////////////////////
// Name:        msgqueue.h
// Purpose:     documentation for wxMessageQueue<T> class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMessageQueueT
    @wxheader{msgqueue.h}

    wxMessageQueue allows passing messages between threads.

    This class should be typically used to communicate between the main and worker
    threads. The main thread calls wxMessageQueue::Post and
    the worker thread calls wxMessageQueue::Receive.

    For this class a message is an object of arbitrary type T. Notice that
    often there is a some special message indicating that the thread
    should terminate as there is no other way to gracefully shutdown a thread
    waiting on the message queue.

    @nolibrary
    @category{FIXME}

    @seealso
    wxThread
*/
class wxMessageQueue<T>
{
public:
    /**
        Returns @true if the object had been initialized successfully, @false
        if an error occurred.
    */
#define bool IsOk()     /* implementation is private */

    /**
        Add a message to this queue and signal the threads waiting for messages
        (i.e. the threads which called wxMessageQueue::Receive or
        wxMessageQueue::ReceiveTimeout).
        
        This method is safe to call from multiple threads in parallel.
    */
    wxMessageQueueError Post(T const& msg);

    /**
        Block until a message becomes available in the queue. Waits indefinitely long
        or until an error occurs.
        
        The message is returned in @e msg.
    */
    wxMessageQueueError Receive(T& msg);

    /**
        Block until a message becomes available in the queue, but no more than
        @e timeout milliseconds has elapsed.
        
        If no message is available after @e timeout milliseconds then returns
        @b wxMSGQUEUE_TIMEOUT.
        
        If @e timeout is 0 then checks for any messages present in the queue
        and returns immediately without waiting.
        
        The message is returned in @e msg.
    */
    wxMessageQueueError ReceiveTimeout(long timeout, T& msg);

    /**
        Default and only constructor. Use wxMessageQueue::IsOk to check
        if the object was successfully initialized.
    */
    wxMessageQueue();
};
