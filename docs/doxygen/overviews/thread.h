/////////////////////////////////////////////////////////////////////////////
// Name:        thread.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    NOTE: we explicitly don't name wxMutexGUIEnter() and wxMutexGUILeave()
          as they're not safe. See also ticket #10366.
*/

/**

@page overview_thread Multithreading Overview

@tableofcontents

@note In the new code, it is highly recommended to use concurrency classes
      provided in C++11 and newer, instead of their wxWidgets counterparts.
      The warning about not using GUI classes from non-GUI threads still applies.

wxWidgets provides a complete set of classes encapsulating objects necessary in
multi-threaded (MT) applications: the wxThread class itself and different
synchronization objects: mutexes (see wxMutex) and critical sections (see
wxCriticalSection) with conditions (see wxCondition). The thread API in
wxWidgets resembles to POSIX thread API (a.k.a. pthreads), although several
functions have different names and some features inspired by Win32 thread API
are there as well.

These classes hopefully make writing MT programs easier and they also provide
some extra error checking (compared to the native - be it Win32 or Posix -
thread API).

To implement non-blocking operations @e without using multiple threads you have
two possible implementation choices:

- use wxIdleEvent (e.g. to perform a long calculation while updating a progress dialog)
- do everything at once but call wxWindow::Update() or wxEventLoopBase::YieldFor(wxEVT_CATEGORY_UI)
  periodically to update the screen.

However, it is generally much better to run time-consuming tasks in worker threads instead
of trying to work around blocked GUI (and risk reentrancy problems).

@see wxThread, wxThreadHelper, wxMutex, wxCriticalSection, wxCondition,
     wxSemaphore



@section overview_thread_notes Important Notes for Multi-threaded Applications

When writing a multi-threaded application, it is strongly recommended that
<b>no secondary threads call GUI functions</b>. The design which uses one GUI
thread and several worker threads which communicate with the main one using
@b events is much more robust and will undoubtedly save you countless problems
(example: under Win32 a thread can only access GDI objects such as pens,
brushes, device contexts created by itself and not by the other threads).
The GUI thread is the thread in which wxWidgets was initialized, where
wxIsMainThread() returns @true.

For communication between secondary threads and the main thread, you may use
wxEvtHandler::QueueEvent() or its short version ::wxQueueEvent(). These functions
have a thread-safe implementation so that they can be used as they are for
sending events from one thread to another. However there is no built in method
to send messages to the worker threads and you will need to use the available
synchronization classes to implement the solution which suits your needs
yourself. In particular, please note that it is not enough to derive your class
from wxThread and wxEvtHandler to send messages to it: in fact, this does not
work at all. You're instead encouraged to use wxThreadHelper as it greatly
simplifies the communication and the sharing of resources.

For communication between the main thread and worker threads, you can use
wxMessageQueue<> class that allows to send any kind of custom messages. It is
often convenient to have a special message asking the thread to terminate.

You should also look at the wxThread docs for important notes about secondary
threads and their deletion.

Last, remember that if wxEventLoopBase::YieldFor() is used directly or
indirectly (e.g. through wxProgressDialog) in your code, then you may have both
re-entrancy problems and also problems caused by the processing of events out
of order. To resolve the last problem wxThreadEvent can be used: thanks to its
implementation of the wxThreadEvent::GetEventCategory function wxThreadEvent
classes in fact do not get processed by wxEventLoopBase::YieldFor() unless you
specify the @c wxEVT_CATEGORY_THREAD flag.

See also the @sample{thread} for a sample showing some simple interactions
between the main and secondary threads.

*/
