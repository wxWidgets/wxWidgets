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

#include "wx/object.h"
#include "wx/setup.h"

typedef enum {
  MUTEX_NO_ERROR=0,
  MUTEX_DEAD_LOCK,		// Mutex has been already locked by THE CALLING thread 
  MUTEX_BUSY,			// Mutex has been already locked by ONE thread
  MUTEX_UNLOCKED
} wxMutexError;

typedef enum {
  THREAD_NO_ERROR=0,		// No error
  THREAD_NO_RESOURCE,		// No resource left to create a new thread
  THREAD_RUNNING,		// The thread is already running
  THREAD_NOT_RUNNING,		// The thread isn't running
  THREAD_MISC_ERROR             // Some other error
} wxThreadError;

// defines the interval of priority.
#define WXTHREAD_MIN_PRIORITY 0
#define WXTHREAD_DEFAULT_PRIORITY 50
#define WXTHREAD_MAX_PRIORITY 100

// ---------------------------------------------------------------------------
// Mutex handler
class WXDLLEXPORT wxMutexInternal;
class WXDLLEXPORT wxMutex {
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

  int m_locked;
  wxMutexInternal *p_internal;
};

// ---------------------------------------------------------------------------
// Condition handler.
class wxConditionInternal;
class WXDLLEXPORT wxCondition {
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

// ---------------------------------------------------------------------------
// Thread management class
class wxThreadInternal;
class WXDLLEXPORT wxThread {
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
  // Returns true if the thread is the main thread (aka the GUI thread).
  static bool IsMain();

  // Called when thread exits.
  virtual void OnExit();

  // Returns the wxThread object which corresponds to the ID.
  static wxThread *GetThreadFromID(unsigned long id);
protected:
  // In case, the DIFFER flag is true, enables another thread to kill this one.
  void TestDestroy();
  // Exits from the current thread.
  void Exit(void *status = NULL);
private:
  // Entry point for the thread.
  virtual void *Entry() = 0;

private:
  friend class wxThreadInternal;

  wxThreadInternal *p_internal;
};

// ---------------------------------------------------------------------------
// Global variables

// GUI mutex.
WXDLLEXPORT_DATA(extern wxMutex) wxMainMutex;

#endif
