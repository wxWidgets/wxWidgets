/////////////////////////////////////////////////////////////////////////////
// Name:        thread.h
// Purpose:     Thread API
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin (modifications partly inspired by omnithreads
//              package from Olivetti & Oracle Research Laboratory)
// Created:     04/13/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __THREADH__
#define __THREADH__

#ifdef __GNUG__
    #pragma interface "thread.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// get the value of wxUSE_THREADS configuration flag
#include "wx/setup.h"

#if wxUSE_THREADS

// Windows headers define it
#ifdef Yield
    #undef Yield
#endif

#include "wx/module.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

typedef enum
{
  wxMUTEX_NO_ERROR = 0,
  wxMUTEX_DEAD_LOCK,      // Mutex has been already locked by THE CALLING thread
  wxMUTEX_BUSY,           // Mutex has been already locked by ONE thread
  wxMUTEX_UNLOCKED,
  wxMUTEX_MISC_ERROR
} wxMutexError;

typedef enum
{
  wxTHREAD_NO_ERROR = 0,      // No error
  wxTHREAD_NO_RESOURCE,       // No resource left to create a new thread
  wxTHREAD_RUNNING,           // The thread is already running
  wxTHREAD_NOT_RUNNING,       // The thread isn't running
  wxTHREAD_MISC_ERROR         // Some other error
} wxThreadError;

// defines the interval of priority
#define WXTHREAD_MIN_PRIORITY     0u
#define WXTHREAD_DEFAULT_PRIORITY 50u
#define WXTHREAD_MAX_PRIORITY     100u

// ----------------------------------------------------------------------------
// A mutex object is a synchronization object whose state is set to signaled
// when it is not owned by any thread, and nonsignaled when it is owned. Its
// name comes from its usefulness in coordinating mutually-exclusive access to
// a shared resource. Only one thread at a time can own a mutex object.
// ----------------------------------------------------------------------------

// you should consider wxMutexLocker whenever possible instead of directly
// working with wxMutex class - it is safer
class WXDLLEXPORT wxMutexInternal;
class WXDLLEXPORT wxMutex
{
public:
    // constructor & destructor
    wxMutex();
    ~wxMutex();

    // Lock the mutex.
    wxMutexError Lock();
    // Try to lock the mutex: if it can't, returns immediately with an error.
    wxMutexError TryLock();
    // Unlock the mutex.
    wxMutexError Unlock();

    // Returns true if the mutex is locked.
    bool IsLocked() const { return (m_locked > 0); }

protected:
    friend class wxCondition;

    // no assignment operator nor copy ctor
    wxMutex(const wxMutex&);
    wxMutex& operator=(const wxMutex&);

    int m_locked;
    wxMutexInternal *p_internal;
};

// a helper class which locks the mutex in the ctor and unlocks it in the dtor:
// this ensures that mutex is always unlocked, even if the function returns or
// throws an exception before it reaches the end
class WXDLLEXPORT wxMutexLocker
{
public:
    // lock the mutex in the ctor
    wxMutexLocker(wxMutex *mutex)
        { m_isOk = mutex && ((m_mutex = mutex)->Lock() == wxMUTEX_NO_ERROR); }

    // returns TRUE if mutex was successfully locked in ctor
    bool IsOk() const { return m_isOk; }

    // unlock the mutex in dtor
    ~wxMutexLocker() { if ( IsOk() ) m_mutex->Unlock(); }

private:
    // no assignment operator nor copy ctor
    wxMutexLocker(const wxMutexLocker&);
    wxMutexLocker& operator=(const wxMutexLocker&);

    bool     m_isOk;
    wxMutex *m_mutex;
};

// ----------------------------------------------------------------------------
// Critical section: this is the same as mutex but is only visible to the
// threads of the same process. For the platforms which don't have native
// support for critical sections, they're implemented entirely in terms of
// mutexes
// ----------------------------------------------------------------------------

// in order to avoid any overhead under !MSW make all wxCriticalSection class
// functions inline - but this can't be done under MSW
#ifdef __WXMSW__
    class WXDLLEXPORT wxCriticalSectionInternal;
    #define WXCRITICAL_INLINE
#else // !MSW
    #define WXCRITICAL_INLINE   inline
#endif // MSW/!MSW

// you should consider wxCriticalSectionLocker whenever possible instead of
// directly working with wxCriticalSection class - it is safer
class WXDLLEXPORT wxCriticalSection
{
public:
    // ctor & dtor
    WXCRITICAL_INLINE wxCriticalSection();
    WXCRITICAL_INLINE ~wxCriticalSection();

    // enter the section (the same as locking a mutex)
    void WXCRITICAL_INLINE Enter();
    // leave the critical section (same as unlocking a mutex)
    void WXCRITICAL_INLINE Leave();

private:
    // no assignment operator nor copy ctor
    wxCriticalSection(const wxCriticalSection&);
    wxCriticalSection& operator=(const wxCriticalSection&);

#ifdef __WXMSW__
    wxCriticalSectionInternal *m_critsect;
#else // !MSW
    wxMutex m_mutex;
#endif // MSW/!MSW
};

// keep your preprocessor name space clean
#undef WXCRITICAL_INLINE

// wxCriticalSectionLocker is the same to critical sections as wxMutexLocker is
// to th mutexes
class WXDLLEXPORT wxCriticalSectionLocker
{
public:
    wxCriticalSectionLocker(wxCriticalSection& critsect) : m_critsect(critsect)
        { m_critsect.Enter(); }
    ~wxCriticalSectionLocker()
        { m_critsect.Leave(); }

private:
    // no assignment operator nor copy ctor
    wxCriticalSectionLocker(const wxCriticalSectionLocker&);
    wxCriticalSectionLocker& operator=(const wxCriticalSectionLocker&);

    wxCriticalSection& m_critsect;
};

// ----------------------------------------------------------------------------
// Condition handler.
// ----------------------------------------------------------------------------

class wxConditionInternal;
class WXDLLEXPORT wxCondition
{
public:
  // constructor & destructor
  wxCondition();
  ~wxCondition();

  // Waits indefinitely.
  void Wait(wxMutex& mutex);
  // Waits until a signal is raised or the timeout is elapsed.
  bool Wait(wxMutex& mutex, unsigned long sec, unsigned long nsec);
  // Raises a signal: only one "Waiter" is released.
  void Signal();
  // Broadcasts to all "Waiters".
  void Broadcast();

private:
  wxConditionInternal *p_internal;
};

// ----------------------------------------------------------------------------
// Thread management class
// ----------------------------------------------------------------------------

// FIXME Thread termination model is still unclear. Delete() should probably
//       have a timeout after which the thread must be Kill()ed.

// NB: in the function descriptions the words "this thread" mean the thread
//     created by the wxThread object while "main thread" is the thread created
//     during the process initialization (a.k.a. the GUI thread)
class wxThreadInternal;
class WXDLLEXPORT wxThread
{
public:
    // the return type for the thread function
    typedef void *ExitCode;

    // static functions
        // Returns the wxThread object for the calling thread. NULL is returned
        // if the caller is the main thread (but it's recommended to use
        // IsMain() and only call This() for threads other than the main one
        // because NULL is also returned on error). If the thread wasn't
        // created with wxThread class, the returned value is undefined.
    static wxThread *This();

        // Returns true if current thread is the main thread.
    static bool IsMain();

        // Release the rest of our time slice leting the other threads run
    static void Yield();

        // Sleep during the specified period of time in milliseconds
        //
        // NB: at least under MSW worker threads can not call ::wxSleep()!
    static void Sleep(unsigned long milliseconds);

    // default constructor
    wxThread();

    // function that change the thread state
        // create a new thread - call Run() to start it
    wxThreadError Create();

        // starts execution of the thread - from the moment Run() is called the
        // execution of wxThread::Entry() may start at any moment, caller
        // shouldn't suppose that it starts after (or before) Run() returns.
    wxThreadError Run();

        // stops the thread if it's running and deletes the wxThread object
        // freeing its memory. This function should also be called if the
        // Create() or Run() fails to free memory (otherwise it will be done by
        // the thread itself when it terminates). The return value is the
        // thread exit code if the thread was gracefully terminated, 0 if it
        // wasn't running and -1 if an error occured.
    ExitCode Delete();

        // kills the thread without giving it any chance to clean up - should
        // not be used in normal circumstances, use Delete() instead. It is a
        // dangerous function that should only be used in the most extreme
        // cases! The wxThread object is deleted by Kill() if thread was
        // killed (i.e. no errors occured).
    wxThreadError Kill();

        // pause a running thread
    wxThreadError Pause();

        // resume a paused thread
    wxThreadError Resume();

    // priority
        // Sets the priority to "prio": see WXTHREAD_XXX_PRIORITY constants
        //
        // NB: the priority can only be set before the thread is created
    void SetPriority(unsigned int prio);

        // Get the current priority.
    unsigned int GetPriority() const;

    // Get the thread ID - a platform dependent number which uniquely
    // identifies a thread inside a process
    unsigned long GetID() const;

    // thread status inquiries
        // Returns true if the thread is alive: i.e. running or suspended
    bool IsAlive() const;
        // Returns true if the thread is running (not paused, not killed).
    bool IsRunning() const;
        // Returns true if the thread is suspended
    bool IsPaused() const { return IsAlive() && !IsRunning(); }

    // called when the thread exits - in the context of this thread
    //
    // NB: this function will not be called if the thread is Kill()ed
    virtual void OnExit() { }

protected:
    // Returns TRUE if the thread was asked to terminate: this function should
    // be called by the thread from time to time, otherwise the main thread
    // will be left forever in Delete()!
    bool TestDestroy() const;

    // exits from the current thread - can be called only from this thread
    void Exit(void *exitcode = 0);

    // destructor is private - user code can't delete thread objects, they will
    // auto-delete themselves (and thus must be always allocated on the heap).
    // Use Delete() or Kill() instead.
    //
    // NB: derived classes dtors shouldn't be public neither!
    virtual ~wxThread();

    // entry point for the thread - called by Run() and executes in the context
    // of this thread.
    virtual void *Entry() = 0;

private:
    // no copy ctor/assignment operator
    wxThread(const wxThread&);
    wxThread& operator=(const wxThread&);

    friend class wxThreadInternal;

    // the (platform-dependent) thread class implementation
    wxThreadInternal *p_internal;

    // protects access to any methods of wxThreadInternal object
    wxCriticalSection m_critsect;
};

// ----------------------------------------------------------------------------
// Automatic initialization
// ----------------------------------------------------------------------------

// GUI mutex handling.
void WXDLLEXPORT wxMutexGuiEnter();
void WXDLLEXPORT wxMutexGuiLeave();

#else // !wxUSE_THREADS

// no thread support
inline void WXDLLEXPORT wxMutexGuiEnter() { }
inline void WXDLLEXPORT wxMutexGuiLeave() { }

#endif // wxUSE_THREADS

// automatically unlock GUI mutex in dtor
class WXDLLEXPORT wxMutexGuiLocker
{
public:
    wxMutexGuiLocker() { wxMutexGuiEnter(); }
   ~wxMutexGuiLocker() { wxMutexGuiLeave(); }
};

// -----------------------------------------------------------------------------
// implementation only until the end of file
// -----------------------------------------------------------------------------
#ifdef wxUSE_THREADS
#ifdef __WXMSW__
    // unlock GUI if there are threads waiting for and lock it back when
    // there are no more of them - should be called periodically by the main
    // thread
    extern void WXDLLEXPORT wxMutexGuiLeaveOrEnter();

    // returns TRUE if the main thread has GUI lock
    extern bool WXDLLEXPORT wxGuiOwnedByMainThread();

    // wakes up the main thread if it's sleeping inside ::GetMessage()
    extern void WXDLLEXPORT wxWakeUpMainThread();

    // return TRUE if the main thread is waiting for some other to terminate:
    // wxApp then should block all "dangerous" messages
    extern bool WXDLLEXPORT wxIsWaitingForThread();
#else // !MSW
    // implement wxCriticalSection using mutexes
    inline wxCriticalSection::wxCriticalSection() { }
    inline wxCriticalSection::~wxCriticalSection() { }

    inline void wxCriticalSection::Enter() { (void)m_mutex.Lock(); }
    inline void wxCriticalSection::Leave() { (void)m_mutex.Unlock(); }
#endif // MSW/!MSW
#endif // wxUSE_THREADS

#endif // __THREADH__
