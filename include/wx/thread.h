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
  MUTEX_BUSY			// Mutex has been already locked by ONE thread
} wxMutexError;

typedef enum {
  THREAD_NO_ERROR=0,		// No error
  THREAD_NO_RESOURCE,		// No resource left to create a new thread
  THREAD_RUNNING,		// The thread is already running
  THREAD_NOT_RUNNING		// The thread isn't running
} wxThreadError;

// defines the interval of priority.
#define WXTHREAD_MIN_PRIORITY 0
#define WXTHREAD_DEFAULT_PRIORITY 50
#define WXTHREAD_MAX_PRIORITY 100

// ---------------------------------------------------------------------------
// Mutex handler
class wxMutexInternal;
class WXDLLEXPORT wxMutex {
public:
  // constructor & destructor 
  wxMutex(void);
  ~wxMutex(void);

  // Lock the mutex.
  wxMutexError Lock(void);
  // Try to lock the mutex: if it can't, returns immediately with an error.
  wxMutexError TryLock(void);
  // Unlock the mutex.
  wxMutexError Unlock(void);

  // Returns true if the mutex is locked.
  bool IsLocked(void) { return (m_locked > 0); }
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
  wxCondition(void);
  ~wxCondition(void);

  // Waits undefinitely.
  void Wait(wxMutex& mutex);
  // Waits until a signal is raised or the timeout is elapsed.
  bool Wait(wxMutex& mutex, unsigned long sec, unsigned long nsec);
  // Raises a signal: only one "Waiter" is released.
  void Signal(void);
  // Broadcasts to all "Waiters".
  void Broadcast(void);
private:
  wxConditionInternal *p_internal;
};

// ---------------------------------------------------------------------------
// Thread management class
class wxThreadInternal;
class WXDLLEXPORT wxThread {
public:
  // constructor & destructor.
  wxThread(void);
  virtual ~wxThread(void);

  // Create a new thread, this method should check there is only one thread
  // running by object.
  wxThreadError Create(void);

  // Destroys the thread immediately if the flag DIFFER isn't true.
  wxThreadError Destroy(void);

  // Switches on the DIFFER flag.
  void DeferDestroy(bool on);

  // Waits for the termination of the thread.
  void *Join(void);

  // Sets the priority to "prio". (Warning: The priority can only be set before
  // the thread is created)
  void SetPriority(int prio);
  // Get the current priority.
  int GetPriority(void);

  // Get the thread ID
  unsigned long GetID(void);

  // Returns true if the thread is alive.
  bool IsAlive(void);
  // Returns true if the thread is the main thread (aka the GUI thread).
  static bool IsMain(void);

  // Called when thread exits.
  virtual void OnExit(void);
protected:
  // In case, the DIFFER flag is true, enables another thread to kill this one.
  void TestDestroy(void);
  // Exits from the current thread.
  void Exit(void *status = NULL);
private:
  // Entry point for the thread.
  virtual void *Entry(void) = 0;

private:
  friend class wxThreadInternal;

  wxThreadInternal *p_internal;
};

// ---------------------------------------------------------------------------
// Global variables

// GUI mutex.
WXDLLEXPORT_DATA(extern wxMutex) wxMainMutex;

#endif
