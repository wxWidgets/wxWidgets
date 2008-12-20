/////////////////////////////////////////////////////////////////////////////
// Name:        thread.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_thread Multithreading

Classes: wxThread, wxThreadHelper, wxMutex, wxCriticalSection, wxCondition, wxSemaphore

wxWidgets provides a complete set of classes encapsulating objects necessary in
multithreaded (MT) programs: the wxThread class itself and different
synchronization objects: mutexes (see wxMutex) and critical sections (see
wxCriticalSection)  with conditions (see wxCondition). The thread API in
wxWidgets resembles to POSIX1.c threads API (a.k.a. pthreads), although several
functions have different names and some features inspired by Win32 thread API
are there as well.

These classes will hopefully make writing MT programs easier and they also
provide some extra error checking (compared to the native (be it Win32 or
Posix) thread API), however it is still a non-trivial undertaking especially
for large projects. Before starting an MT application (or starting to add MT
features to an existing one) it is worth asking oneself if there is no easier
and safer way to implement the same functionality. Of course, in some
situations threads really make sense (classical example is a server application
which launches a new thread for each new client), but in others it might be an
overkill. On the other hand, the recent evolution of the computer hardware shows
an important trend towards multi-core systems, which are better exploited using
multiple threads (e.g. you may want to split a long task among as many threads
as many CPU (cores) the system reports; see wxThread::GetCPUCount).

To implement non-blocking operations without using multiple threads you have
two other possible implementation choices:
- using wxIdleEvent (e.g. to perform a long calculation while updating a progress dialog)
- simply do everything at once but call wxWindow::Update() periodically to update the screen.

Even if there are the ::wxMutexGuiEnter and ::wxMutexGuiLeave functions which allows
to use GUI functions from multiple threads, if you do decide to use threads in your
application, it is strongly recommended that <b>no more than one calls GUI functions</b>.
The design which uses one GUI thread and several worker threads which communicate
with the main one using @b events is much more robust and will undoubtedly save you
countless problems (example: under Win32 a thread can only access GDI objects such
as pens, brushes, c created by itself and not by the other threads).

For communication between secondary threads and the main thread, you may use
wxEvtHandler::QueueEvent or its short version ::wxQueueEvent. These functions
have a thread-safe implementation so that they can be used as they are for
sending events from one thread to another. However there is no built in method
to send messages to the worker threads and you will need to use the available
synchronization classes to implement the solution which suits your needs
yourself. In particular, please note that it is not enough to derive
your class from wxThread and wxEvtHandler to send messages to it: in fact, this
does not work at all.

See also the @sample{thread} for a sample showing some simple interactions
between the main and secondary threads.

*/

