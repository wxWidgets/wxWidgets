/////////////////////////////////////////////////////////////////////////////
// Name:        thread.h
// Purpose:     Thread API
// Author:      Guilhem Lavaux
// Modified by:
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
#include "wx/setup.h"

#if wxUSE_THREADS
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

/* defines the interval of priority. */
#define WXTHREAD_MIN_PRIORITY     0
#define WXTHREAD_DEFAULT_PRIORITY 50
#define WXTHREAD_MAX_PRIORITY     100

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

// you should consider wxCriticalSectionLocker whenever possible instead of
// directly working with wxCriticalSection class - it is safer
#ifdef __WXMSW__
    class WXDLLEXPORT wxCriticalSectionInternal;
    #define WXCRITICAL_INLINE
#else // !MSW
    #define WXCRITICAL_INLINE   inline
#endif // MSW/!MSW
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

class wxThreadInternal;
class WXDLLEXPORT wxThread 
{
public:
  // constructor & destructor.
  wxThread();
  virtual ~wxThread();

  // Create a new thread, this method should check there is only one thread
  // running by object.
  wxThreadError Create();

  // Destroys the thread immediately if the defer flag isn't true.
  wxThreadError Destroy();

  // Pause a running thread
  wxThreadError Pause();

  // Resume a paused thread
  wxThreadError Resume();

  // Switches on the defer flag.
  void DeferDestroy(bool on);

  // Waits for the termination of the thread.
  void *Join();

  // Sets the priority to "prio". (Warning: The priority can only be set before
  // the thread is created)
  void SetPriority(int prio);
  // Get the current priority.
  int GetPriority() const;

  // Get the thread ID
  unsigned long GetID() const;

  // Returns true if the thread is alive.
  bool IsAlive() const;
  // Returns true if the thread is running (not paused, not killed).
  bool IsRunning() const;
  // Returns true if the thread is suspended
  bool IsPaused() const { return IsAlive() && !IsRunning(); }

  // Returns true if current thread is the main thread (aka the GUI thread)
  static bool IsMain();

  // Called when thread exits.
  virtual void OnExit();

protected:
  // Returns TRUE if the thread was asked to terminate
  bool TestDestroy();

  // Exits from the current thread.
  void Exit(void *status = NULL);

private:
  // Entry point for the thread.
  virtual void *Entry() = 0;

private:
  friend class wxThreadInternal;

  wxThreadInternal *p_internal;
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
    void WXDLLEXPORT wxMutexGuiLeaveOrEnter();

    // returns TRUE if the main thread has GUI lock
    inline bool WXDLLEXPORT wxGuiOwnedByMainThread();

    // wakes up the main thread if it's sleeping inside ::GetMessage()
    inline void WXDLLEXPORT wxWakeUpMainThread();
#else // !MSW
    // implement wxCriticalSection using mutexes
    inline wxCriticalSection::wxCriticalSection() { }
    inline wxCriticalSection::~wxCriticalSection() { }

    inline void wxCriticalSection::Enter() { (void)m_mutex.Lock(); }
    inline void wxCriticalSection::Leave() { (void)m_mutex.Unlock(); }
#endif // MSW/!MSW
#endif // wxUSE_THREADS

#endif // __THREADH__
