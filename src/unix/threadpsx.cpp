/////////////////////////////////////////////////////////////////////////////
// Name:        threadpsx.cpp
// Purpose:     wxThread (Posix) Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by:
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997)
//                  Guilhem Lavaux (1998)
//                  Vadim Zeitlin (1999)
//                  Robert Roebling (1999)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declaration
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "thread.h"
#endif

// With simple makefiles, we must ignore the file body if not using
// threads.
#include "wx/setup.h"

#if wxUSE_THREADS

#include "wx/thread.h"
#include "wx/module.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#if HAVE_SCHED_H
    #include <sched.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the possible states of the thread and transitions from them
enum wxThreadState
{
    STATE_NEW,          // didn't start execution yet (=> RUNNING)
    STATE_RUNNING,      // running (=> PAUSED or EXITED)
    STATE_PAUSED,       // suspended (=> RUNNING or EXITED)
    STATE_EXITED        // thread doesn't exist any more
};

// the exit value of a thread which has been cancelled
static const wxThread::ExitCode EXITCODE_CANCELLED = (wxThread::ExitCode)-1;

// our trace mask
#define TRACE_THREADS   _T("thread")

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void ScheduleThreadForDeletion();
static void DeleteThread(wxThread *This);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// same as wxMutexLocker but for "native" mutex
class MutexLock
{
public:
    MutexLock(pthread_mutex_t& mutex)
    {
        m_mutex = &mutex;
        if ( pthread_mutex_lock(m_mutex) != 0 )
        {
            wxLogDebug(_T("pthread_mutex_lock() failed"));
        }
    }

    ~MutexLock()
    {
        if ( pthread_mutex_unlock(m_mutex) != 0 )
        {
            wxLogDebug(_T("pthread_mutex_unlock() failed"));
        }
    }

private:
    pthread_mutex_t *m_mutex;
};

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxThread *, wxArrayThread);

// -----------------------------------------------------------------------------
// global data
// -----------------------------------------------------------------------------

// we keep the list of all threads created by the application to be able to
// terminate them on exit if there are some left - otherwise the process would
// be left in memory
static wxArrayThread gs_allThreads;

// the id of the main thread
static pthread_t gs_tidMain;

// the key for the pointer to the associated wxThread object
static pthread_key_t gs_keySelf;

// the number of threads which are being deleted - the program won't exit
// until there are any left
static size_t gs_nThreadsBeingDeleted = 0;

// a mutex to protect gs_nThreadsBeingDeleted
static pthread_mutex_t gs_mutexDeleteThread = PTHREAD_MUTEX_INITIALIZER;

// and a condition variable which will be signaled when all
// gs_nThreadsBeingDeleted will have been deleted
static wxCondition *gs_condAllDeleted = (wxCondition *)NULL;

#if wxUSE_GUI
    // this mutex must be acquired before any call to a GUI function
    static wxMutex *gs_mutexGui;
#endif // wxUSE_GUI

// ============================================================================
// implementation
// ============================================================================

//--------------------------------------------------------------------
// wxMutex (Posix implementation)
//--------------------------------------------------------------------

class wxMutexInternal
{
public:
    pthread_mutex_t m_mutex;
};

wxMutex::wxMutex()
{
    m_internal = new wxMutexInternal;

    pthread_mutex_init(&(m_internal->m_mutex),
                       (pthread_mutexattr_t*) NULL );
    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxLogDebug(wxT("Freeing a locked mutex (%d locks)"), m_locked);

    pthread_mutex_destroy( &(m_internal->m_mutex) );
    delete m_internal;
}

wxMutexError wxMutex::Lock()
{
    int err = pthread_mutex_lock( &(m_internal->m_mutex) );
    if (err == EDEADLK)
    {
        wxLogDebug(wxT("Locking this mutex would lead to deadlock!"));

        return wxMUTEX_DEAD_LOCK;
    }

    m_locked++;

    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
    if (m_locked)
    {
        return wxMUTEX_BUSY;
    }

    int err = pthread_mutex_trylock( &(m_internal->m_mutex) );
    switch (err)
    {
        case EBUSY: return wxMUTEX_BUSY;
    }

    m_locked++;

    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
    if (m_locked > 0)
    {
        m_locked--;
    }
    else
    {
        wxLogDebug(wxT("Unlocking not locked mutex."));

        return wxMUTEX_UNLOCKED;
    }

    pthread_mutex_unlock( &(m_internal->m_mutex) );

    return wxMUTEX_NO_ERROR;
}

//--------------------------------------------------------------------
// wxCondition (Posix implementation)
//--------------------------------------------------------------------

// notice that we must use a mutex with POSIX condition variables to ensure
// that the worker thread doesn't signal condition before the waiting thread
// starts to wait for it
class wxConditionInternal
{
public:
    wxConditionInternal();
    ~wxConditionInternal();

    void Wait();
    bool WaitWithTimeout(const timespec* ts);

    void Signal();
    void Broadcast();

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_condition;
};

wxConditionInternal::wxConditionInternal()
{
    if ( pthread_cond_init(&m_condition, (pthread_condattr_t *)NULL) != 0 )
    {
        // this is supposed to never happen
        wxFAIL_MSG( _T("pthread_cond_init() failed") );
    }

    if ( pthread_mutex_init(&m_mutex, (pthread_mutexattr_t*)NULL) != 0 )
    {
        // neither this
        wxFAIL_MSG( _T("wxCondition: pthread_mutex_init() failed") );
    }

    // initially the mutex is locked, so no thread can Signal() or Broadcast()
    // until another thread starts to Wait()
    if ( pthread_mutex_lock(&m_mutex) != 0 )
    {
        wxFAIL_MSG( _T("wxCondition: pthread_mutex_lock() failed") );
    }
}

wxConditionInternal::~wxConditionInternal()
{
    if ( pthread_cond_destroy( &m_condition ) != 0 )
    {
        wxLogDebug(_T("Failed to destroy condition variable (some "
                      "threads are probably still waiting on it?)"));
    }

    if ( pthread_mutex_unlock( &m_mutex ) != 0 )
    {
        wxLogDebug(_T("wxCondition: failed to unlock the mutex"));
    }

    if ( pthread_mutex_destroy( &m_mutex ) != 0 )
    {
        wxLogDebug(_T("Failed to destroy mutex (it is probably locked)"));
    }
}

void wxConditionInternal::Wait()
{
    if ( pthread_cond_wait( &m_condition, &m_mutex ) != 0 )
    {
        // not supposed to ever happen
        wxFAIL_MSG( _T("pthread_cond_wait() failed") );
    }
}

bool wxConditionInternal::WaitWithTimeout(const timespec* ts)
{
    switch ( pthread_cond_timedwait( &m_condition, &m_mutex, ts ) )
    {
        case 0:
            // condition signaled
            return TRUE;

        default:
            wxLogDebug(_T("pthread_cond_timedwait() failed"));

            // fall through

        case ETIMEDOUT:
        case EINTR:
            // wait interrupted or timeout elapsed
            return FALSE;
    }
}

void wxConditionInternal::Signal()
{
    MutexLock lock(m_mutex);

    if ( pthread_cond_signal( &m_condition ) != 0 )
    {
        // shouldn't ever happen
        wxFAIL_MSG(_T("pthread_cond_signal() failed"));
    }
}

void wxConditionInternal::Broadcast()
{
    MutexLock lock(m_mutex);

    if ( pthread_cond_broadcast( &m_condition ) != 0 )
    {
        // shouldn't ever happen
        wxFAIL_MSG(_T("pthread_cond_broadcast() failed"));
    }
}

wxCondition::wxCondition()
{
    m_internal = new wxConditionInternal;
}

wxCondition::~wxCondition()
{
    delete m_internal;
}

void wxCondition::Wait()
{
    m_internal->Wait();
}

bool wxCondition::Wait(unsigned long sec, unsigned long nsec)
{
    timespec tspec;

    tspec.tv_sec = time(0L) + sec;  // FIXME is time(0) correct here?
    tspec.tv_nsec = nsec;

    return m_internal->WaitWithTimeout(&tspec);
}

void wxCondition::Signal()
{
    m_internal->Signal();
}

void wxCondition::Broadcast()
{
    m_internal->Broadcast();
}

//--------------------------------------------------------------------
// wxThread (Posix implementation)
//--------------------------------------------------------------------

class wxThreadInternal
{
public:
    wxThreadInternal();
    ~wxThreadInternal();

    // thread entry function
    static void *PthreadStart(void *ptr);

#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // thread exit function
    static void PthreadCleanup(void *ptr);
#endif

    // thread actions
        // start the thread
    wxThreadError Run();
        // ask the thread to terminate
    void Wait();
        // wake up threads waiting for our termination
    void SignalExit();
        // go to sleep until Resume() is called
    void Pause();
        // resume the thread
    void Resume();

    // accessors
        // priority
    int GetPriority() const { return m_prio; }
    void SetPriority(int prio) { m_prio = prio; }
        // state
    wxThreadState GetState() const { return m_state; }
    void SetState(wxThreadState state) { m_state = state; }
        // id
    pthread_t GetId() const { return m_threadId; }
    pthread_t *GetIdPtr() { return &m_threadId; }
        // "cancelled" flag
    void SetCancelFlag() { m_cancelled = TRUE; }
    bool WasCancelled() const { return m_cancelled; }
        // exit code
    void SetExitCode(wxThread::ExitCode exitcode) { m_exitcode = exitcode; }
    wxThread::ExitCode GetExitCode() const { return m_exitcode; }

        // tell the thread that it is a detached one
    void Detach() { m_shouldBeJoined = m_shouldBroadcast = FALSE; }
        // but even detached threads need to notifyus about their termination
        // sometimes - tell the thread that it should do it
    void Notify() { m_shouldBroadcast = TRUE; }

private:
    pthread_t     m_threadId;   // id of the thread
    wxThreadState m_state;      // see wxThreadState enum
    int           m_prio;       // in wxWindows units: from 0 to 100

    // this flag is set when the thread should terminate
    bool m_cancelled;

    // the thread exit code - only used for joinable (!detached) threads and
    // is only valid after the thread termination
    wxThread::ExitCode m_exitcode;

    // many threads may call Wait(), but only one of them should call
    // pthread_join(), so we have to keep track of this
    wxCriticalSection m_csJoinFlag;
    bool m_shouldBeJoined;
    bool m_shouldBroadcast;

    // VZ: it's possible that we might do with less than three different
    //     condition objects - for example, m_condRun and m_condEnd a priori
    //     won't be used in the same time. But for now I prefer this may be a
    //     bit less efficient but safer solution of having distinct condition
    //     variables for each purpose.

    // this condition is signaled by Run() and the threads Entry() is not
    // called before it is done
    wxCondition m_condRun;

    // this one is signaled when the thread should resume after having been
    // Pause()d
    wxCondition m_condSuspend;

    // finally this one is signalled when the thread exits
    wxCondition m_condEnd;
};

// ----------------------------------------------------------------------------
// thread startup and exit functions
// ----------------------------------------------------------------------------

void *wxThreadInternal::PthreadStart(void *ptr)
{
    wxThread *thread = (wxThread *)ptr;
    wxThreadInternal *pthread = thread->m_internal;

    // associate the thread pointer with the newly created thread so that
    // wxThread::This() will work
    int rc = pthread_setspecific(gs_keySelf, thread);
    if ( rc != 0 )
    {
        wxLogSysError(rc, _("Cannot start thread: error writing TLS"));

        return (void *)-1;
    }

#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // install the cleanup handler which will be called if the thread is
    // cancelled
    pthread_cleanup_push(wxThreadInternal::PthreadCleanup, ptr);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

    // wait for the condition to be signaled from Run()
    pthread->m_condRun.Wait();

    // call the main entry
    pthread->m_exitcode = thread->Entry();

    wxLogTrace(TRACE_THREADS, _T("Thread %ld left its Entry()."),
               pthread->GetId());

    {
        wxCriticalSectionLocker lock(thread->m_critsect);

        wxLogTrace(TRACE_THREADS, _T("Thread %ld changes state to EXITED."),
                   pthread->GetId());

        // change the state of the thread to "exited" so that PthreadCleanup
        // handler won't do anything from now (if it's called before we do
        // pthread_cleanup_pop below)
        pthread->SetState(STATE_EXITED);
    }

    // NB: at least under Linux, pthread_cleanup_push/pop are macros and pop
    //     contains the matching '}' for the '{' in push, so they must be used
    //     in the same block!
#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // remove the cleanup handler without executing it
    pthread_cleanup_pop(FALSE);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

    // terminate the thread
    thread->Exit(pthread->m_exitcode);

    wxFAIL_MSG(wxT("wxThread::Exit() can't return."));

    return NULL;
}

#if HAVE_THREAD_CLEANUP_FUNCTIONS

// this handler is called when the thread is cancelled
void wxThreadInternal::PthreadCleanup(void *ptr)
{
    wxThread *thread = (wxThread *) ptr;

    {
        wxCriticalSectionLocker lock(thread->m_critsect);
        if ( thread->m_internal->GetState() == STATE_EXITED )
        {
            // thread is already considered as finished.
            return;
        }
    }

    // exit the thread gracefully
    thread->Exit(EXITCODE_CANCELLED);
}

#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

// ----------------------------------------------------------------------------
// wxThreadInternal
// ----------------------------------------------------------------------------

wxThreadInternal::wxThreadInternal()
{
    m_state = STATE_NEW;
    m_cancelled = FALSE;
    m_prio = WXTHREAD_DEFAULT_PRIORITY;
    m_threadId = 0;
    m_exitcode = 0;

    // defaults for joinable threads
    m_shouldBeJoined = TRUE;
    m_shouldBroadcast = TRUE;
}

wxThreadInternal::~wxThreadInternal()
{
}

wxThreadError wxThreadInternal::Run()
{
    wxCHECK_MSG( GetState() == STATE_NEW, wxTHREAD_RUNNING,
                 wxT("thread may only be started once after Create()") );

    m_condRun.Signal();

    SetState(STATE_RUNNING);

    return wxTHREAD_NO_ERROR;
}

void wxThreadInternal::Wait()
{
    // if the thread we're waiting for is waiting for the GUI mutex, we will
    // deadlock so make sure we release it temporarily
    if ( wxThread::IsMain() )
        wxMutexGuiLeave();

    // wait until the thread terminates (we're blocking in _another_ thread,
    // of course)
    m_condEnd.Wait();

    // to avoid memory leaks we should call pthread_join(), but it must only
    // be done once
    wxCriticalSectionLocker lock(m_csJoinFlag);

    if ( m_shouldBeJoined )
    {
        // FIXME shouldn't we set cancellation type to DISABLED here? If we're
        //       cancelled inside pthread_join(), things will almost certainly
        //       break - but if we disable the cancellation, we might deadlock
        if ( pthread_join(GetId(), &m_exitcode) != 0 )
        {
            wxLogError(_T("Failed to join a thread, potential memory leak "
                          "detected - please restart the program"));
        }

        m_shouldBeJoined = FALSE;
    }

    // reacquire GUI mutex
    if ( wxThread::IsMain() )
        wxMutexGuiEnter();
}

void wxThreadInternal::SignalExit()
{
    wxLogTrace(TRACE_THREADS, _T("Thread %ld about to exit."), GetId());

    SetState(STATE_EXITED);

    // wake up all the threads waiting for our termination - if there are any
    if ( m_shouldBroadcast )
    {
        m_condEnd.Broadcast();
    }
}

void wxThreadInternal::Pause()
{
    // the state is set from the thread which pauses us first, this function
    // is called later so the state should have been already set
    wxCHECK_RET( m_state == STATE_PAUSED,
                 wxT("thread must first be paused with wxThread::Pause().") );

    wxLogTrace(TRACE_THREADS, _T("Thread %ld goes to sleep."), GetId());

    // wait until the condition is signaled from Resume()
    m_condSuspend.Wait();
}

void wxThreadInternal::Resume()
{
    wxCHECK_RET( m_state == STATE_PAUSED,
                 wxT("can't resume thread which is not suspended.") );

    wxLogTrace(TRACE_THREADS, _T("Waking up thread %ld"), GetId());

    // wake up Pause()
    m_condSuspend.Signal();

    SetState(STATE_RUNNING);
}

// -----------------------------------------------------------------------------
// wxThread static functions
// -----------------------------------------------------------------------------

wxThread *wxThread::This()
{
    return (wxThread *)pthread_getspecific(gs_keySelf);
}

bool wxThread::IsMain()
{
    return (bool)pthread_equal(pthread_self(), gs_tidMain);
}

void wxThread::Yield()
{
    sched_yield();
}

void wxThread::Sleep(unsigned long milliseconds)
{
    wxUsleep(milliseconds);
}

// -----------------------------------------------------------------------------
// creating thread
// -----------------------------------------------------------------------------

wxThread::wxThread(wxThreadKind kind)
{
    // add this thread to the global list of all threads
    gs_allThreads.Add(this);

    m_internal = new wxThreadInternal();

    m_isDetached = kind == wxTHREAD_DETACHED;
}

wxThreadError wxThread::Create()
{
    if ( m_internal->GetState() != STATE_NEW )
    {
        // don't recreate thread
        return wxTHREAD_RUNNING;
    }

    // set up the thread attribute: right now, we only set thread priority
    pthread_attr_t attr;
    pthread_attr_init(&attr);

#ifdef HAVE_THREAD_PRIORITY_FUNCTIONS
    int policy;
    if ( pthread_attr_getschedpolicy(&attr, &policy) != 0 )
    {
        wxLogError(_("Cannot retrieve thread scheduling policy."));
    }

    int min_prio = sched_get_priority_min(policy),
        max_prio = sched_get_priority_max(policy),
        prio = m_internal->GetPriority();

    if ( min_prio == -1 || max_prio == -1 )
    {
        wxLogError(_("Cannot get priority range for scheduling policy %d."),
                   policy);
    }
    else if ( max_prio == min_prio )
    {
        if ( prio != WXTHREAD_DEFAULT_PRIORITY )
        {
            // notify the programmer that this doesn't work here
            wxLogWarning(_("Thread priority setting is ignored."));
        }
        //else: we have default priority, so don't complain

        // anyhow, don't do anything because priority is just ignored
    }
    else
    {
        struct sched_param sp;
        if ( pthread_attr_getschedparam(&attr, &sp) != 0 )
        {
            wxFAIL_MSG(_T("pthread_attr_getschedparam() failed"));
        }

        sp.sched_priority = min_prio + (prio*(max_prio - min_prio))/100;

        if ( pthread_attr_setschedparam(&attr, &sp) != 0 )
        {
            wxFAIL_MSG(_T("pthread_attr_setschedparam(priority) failed"));
        }
    }
#endif // HAVE_THREAD_PRIORITY_FUNCTIONS

#ifdef HAVE_PTHREAD_ATTR_SETSCOPE
    // this will make the threads created by this process really concurrent
    if ( pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) != 0 )
    {
        wxFAIL_MSG(_T("pthread_attr_setscope(PTHREAD_SCOPE_SYSTEM) failed"));
    }
#endif // HAVE_PTHREAD_ATTR_SETSCOPE

    // VZ: assume that this one is always available (it's rather fundamental),
    //     if this function is ever missing we should try to use
    //     pthread_detach() instead (after thread creation)
    if ( m_isDetached )
    {
        if ( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0 )
        {
            wxFAIL_MSG(_T("pthread_attr_setdetachstate(DETACHED) failed"));
        }

        // never try to join detached threads
        m_internal->Detach();
    }
    //else: threads are created joinable by default, it's ok

    // create the new OS thread object
    int rc = pthread_create
             (
                m_internal->GetIdPtr(),
                &attr,
                wxThreadInternal::PthreadStart,
                (void *)this
             );

    if ( pthread_attr_destroy(&attr) != 0 )
    {
        wxFAIL_MSG(_T("pthread_attr_destroy() failed"));
    }

    if ( rc != 0 )
    {
        m_internal->SetState(STATE_EXITED);

        return wxTHREAD_NO_RESOURCE;
    }

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
    wxCriticalSectionLocker lock(m_critsect);

    wxCHECK_MSG( m_internal->GetId(), wxTHREAD_MISC_ERROR,
                 wxT("must call wxThread::Create() first") );

    return m_internal->Run();
}

// -----------------------------------------------------------------------------
// misc accessors
// -----------------------------------------------------------------------------

void wxThread::SetPriority(unsigned int prio)
{
    wxCHECK_RET( ((int)WXTHREAD_MIN_PRIORITY <= (int)prio) &&
                 ((int)prio <= (int)WXTHREAD_MAX_PRIORITY),
                 wxT("invalid thread priority") );

    wxCriticalSectionLocker lock(m_critsect);

    switch ( m_internal->GetState() )
    {
        case STATE_NEW:
            // thread not yet started, priority will be set when it is
            m_internal->SetPriority(prio);
            break;

        case STATE_RUNNING:
        case STATE_PAUSED:
#ifdef HAVE_THREAD_PRIORITY_FUNCTIONS
            {
                struct sched_param sparam;
                sparam.sched_priority = prio;

                if ( pthread_setschedparam(m_internal->GetId(),
                                           SCHED_OTHER, &sparam) != 0 )
                {
                    wxLogError(_("Failed to set thread priority %d."), prio);
                }
            }
#endif // HAVE_THREAD_PRIORITY_FUNCTIONS
            break;

        case STATE_EXITED:
        default:
            wxFAIL_MSG(wxT("impossible to set thread priority in this state"));
    }
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return m_internal->GetPriority();
}

unsigned long wxThread::GetId() const
{
    return (unsigned long)m_internal->GetId();
}

// -----------------------------------------------------------------------------
// pause/resume
// -----------------------------------------------------------------------------

wxThreadError wxThread::Pause()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( m_internal->GetState() != STATE_RUNNING )
    {
        wxLogDebug(wxT("Can't pause thread which is not running."));

        return wxTHREAD_NOT_RUNNING;
    }

    // just set a flag, the thread will be really paused only during the next
    // call to TestDestroy()
    m_internal->SetState(STATE_PAUSED);

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    m_critsect.Enter();

    wxThreadState state = m_internal->GetState();

    // the thread might be not actually paused yet - if there were no call to
    // TestDestroy() since the last call to Pause(), so avoid that
    // TestDestroy() deadlocks trying to enter m_critsect by leaving it before
    // calling Resume()
    m_critsect.Leave();

    switch ( state )
    {
        case STATE_PAUSED:
            wxLogTrace(TRACE_THREADS, _T("Thread %ld is suspended, resuming."),
                       GetId());

            m_internal->Resume();

            return wxTHREAD_NO_ERROR;

        case STATE_EXITED:
            wxLogTrace(TRACE_THREADS, _T("Thread %ld exited, won't resume."),
                       GetId());
            return wxTHREAD_NO_ERROR;

        default:
            wxLogDebug(_T("Attempt to resume a thread which is not paused."));

            return wxTHREAD_MISC_ERROR;
    }
}

// -----------------------------------------------------------------------------
// exiting thread
// -----------------------------------------------------------------------------

wxThread::ExitCode wxThread::Wait()
{
    wxCHECK_MSG( This() != this, (ExitCode)-1,
                 _T("a thread can't wait for itself") );

    wxCHECK_MSG( !m_isDetached, (ExitCode)-1,
                 _T("can't wait for detached thread") );

    m_internal->Wait();

    return m_internal->GetExitCode();
}

wxThreadError wxThread::Delete(ExitCode *rc)
{
    m_critsect.Enter();
    wxThreadState state = m_internal->GetState();

    // ask the thread to stop
    m_internal->SetCancelFlag();

    if ( m_isDetached )
    {
        // detached threads won't broadcast about their termination by default
        // because usually nobody waits for them - but here we do, so ask the
        // thread to notify us
        m_internal->Notify();
    }

    m_critsect.Leave();

    switch ( state )
    {
        case STATE_NEW:
        case STATE_EXITED:
            // nothing to do
            break;

        case STATE_PAUSED:
            // resume the thread first (don't call our Resume() because this
            // would dead lock when it tries to enter m_critsect)
            m_internal->Resume();

            // fall through

        default:
            // wait until the thread stops
            m_internal->Wait();

            if ( rc )
            {
                wxASSERT_MSG( !m_isDetached,
                              _T("no return code for detached threads") );

                // if it's a joinable thread, it's not deleted yet
                *rc = m_internal->GetExitCode();
            }
    }

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Kill()
{
    wxCHECK_MSG( This() != this, wxTHREAD_MISC_ERROR,
                 _T("a thread can't kill itself") );

    switch ( m_internal->GetState() )
    {
        case STATE_NEW:
        case STATE_EXITED:
            return wxTHREAD_NOT_RUNNING;

        case STATE_PAUSED:
            // resume the thread first
            Resume();

            // fall through

        default:
#ifdef HAVE_PTHREAD_CANCEL
            if ( pthread_cancel(m_internal->GetId()) != 0 )
#endif
            {
                wxLogError(_("Failed to terminate a thread."));

                return wxTHREAD_MISC_ERROR;
            }

            if ( m_isDetached )
            {
                // if we use cleanup function, this will be done from
                // PthreadCleanup()
#if !HAVE_THREAD_CLEANUP_FUNCTIONS
                ScheduleThreadForDeletion();

                OnExit();

                DeleteThread(this);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS
            }
            else
            {
                m_internal->SetExitCode(EXITCODE_CANCELLED);
            }

            return wxTHREAD_NO_ERROR;
    }
}

void wxThread::Exit(ExitCode status)
{
    // from the moment we call OnExit(), the main program may terminate at any
    // moment, so mark this thread as being already in process of being
    // deleted or wxThreadModule::OnExit() will try to delete it again
    ScheduleThreadForDeletion();

    // don't enter m_critsect before calling OnExit() because the user code
    // might deadlock if, for example, it signals a condition in OnExit() (a
    // common case) while the main thread calls any of functions entering
    // m_critsect on us (almost all of them do)
    OnExit();

    // now do enter it because SignalExit() will change our state
    m_critsect.Enter();

    // next wake up the threads waiting for us (OTOH, this function won't return
    // until someone waited for us!)
    m_internal->SignalExit();

    // leave the critical section before entering the dtor which tries to
    // enter it
    m_critsect.Leave();

    // delete C++ thread object if this is a detached thread - user is
    // responsible for doing this for joinable ones
    if ( m_isDetached )
    {
        // FIXME I'm feeling bad about it - what if another thread function is
        //       called (in another thread context) now? It will try to access
        //       half destroyed object which will probably result in something
        //       very bad - but we can't protect this by a crit section unless
        //       we make it a global object, but this would mean that we can
        //       only call one thread function at a time :-(
        DeleteThread(this);
    }

    // terminate the thread (pthread_exit() never returns)
    pthread_exit(status);

    wxFAIL_MSG(_T("pthread_exit() failed"));
}

// also test whether we were paused
bool wxThread::TestDestroy()
{
    m_critsect.Enter();

    if ( m_internal->GetState() == STATE_PAUSED )
    {
        // leave the crit section or the other threads will stop too if they
        // try to call any of (seemingly harmless) IsXXX() functions while we
        // sleep
        m_critsect.Leave();

        m_internal->Pause();
    }
    else
    {
        // thread wasn't requested to pause, nothing to do
        m_critsect.Leave();
    }

    return m_internal->WasCancelled();
}

wxThread::~wxThread()
{
#ifdef __WXDEBUG__
    m_critsect.Enter();

    // check that the thread either exited or couldn't be created
    if ( m_internal->GetState() != STATE_EXITED &&
         m_internal->GetState() != STATE_NEW )
    {
        wxLogDebug(_T("The thread is being destroyed although it is still "
                      "running! The application may crash."));
    }

    m_critsect.Leave();
#endif // __WXDEBUG__

    delete m_internal;

    // remove this thread from the global array
    gs_allThreads.Remove(this);
}

// -----------------------------------------------------------------------------
// state tests
// -----------------------------------------------------------------------------

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return m_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker lock((wxCriticalSection&)m_critsect);

    switch ( m_internal->GetState() )
    {
        case STATE_RUNNING:
        case STATE_PAUSED:
            return TRUE;

        default:
            return FALSE;
    }
}

bool wxThread::IsPaused() const
{
    wxCriticalSectionLocker lock((wxCriticalSection&)m_critsect);

    return (m_internal->GetState() == STATE_PAUSED);
}

//--------------------------------------------------------------------
// wxThreadModule
//--------------------------------------------------------------------

class wxThreadModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxThreadModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)

bool wxThreadModule::OnInit()
{
    int rc = pthread_key_create(&gs_keySelf, NULL /* dtor function */);
    if ( rc != 0 )
    {
        wxLogSysError(rc, _("Thread module initialization failed: "
                            "failed to create thread key"));

        return FALSE;
    }

    gs_tidMain = pthread_self();

#if wxUSE_GUI
    gs_mutexGui = new wxMutex();

    gs_mutexGui->Lock();
#endif // wxUSE_GUI

    return TRUE;
}

void wxThreadModule::OnExit()
{
    wxASSERT_MSG( wxThread::IsMain(), wxT("only main thread can be here") );

    // are there any threads left which are being deleted right now?
    size_t nThreadsBeingDeleted;
    {
        MutexLock lock(gs_mutexDeleteThread);
        nThreadsBeingDeleted = gs_nThreadsBeingDeleted;
    }

    if ( nThreadsBeingDeleted > 0 )
    {
        wxLogTrace(TRACE_THREADS, _T("Waiting for %u threads to disappear"),
                   nThreadsBeingDeleted);

        // have to wait until all of them disappear
        gs_condAllDeleted->Wait();
    }

    // terminate any threads left
    size_t count = gs_allThreads.GetCount();
    if ( count != 0u )
        wxLogDebug(wxT("Some threads were not terminated by the application."));

    for ( size_t n = 0u; n < count; n++ )
    {
        // Delete calls the destructor which removes the current entry. We
        // should only delete the first one each time.
        gs_allThreads[0]->Delete();
    }

#if wxUSE_GUI
    // destroy GUI mutex
    gs_mutexGui->Unlock();

    delete gs_mutexGui;
#endif // wxUSE_GUI

    // and free TLD slot
    (void)pthread_key_delete(gs_keySelf);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static void ScheduleThreadForDeletion()
{
    MutexLock lock(gs_mutexDeleteThread);

    if ( gs_nThreadsBeingDeleted == 0 )
    {
        gs_condAllDeleted = new wxCondition;
    }

    gs_nThreadsBeingDeleted++;

    wxLogTrace(TRACE_THREADS, _T("%u threads waiting to be deleted"),
               gs_nThreadsBeingDeleted);
}

static void DeleteThread(wxThread *This)
{
    // gs_mutexDeleteThread should be unlocked before signalling the condition
    // or wxThreadModule::OnExit() would deadlock
    {
        MutexLock lock(gs_mutexDeleteThread);

        wxLogTrace(TRACE_THREADS, _T("Thread %ld auto deletes."), This->GetId());

        delete This;

        wxCHECK_RET( gs_nThreadsBeingDeleted > 0,
                     _T("no threads scheduled for deletion, yet we delete "
                        "one?") );
    }

    if ( !--gs_nThreadsBeingDeleted )
    {
        // no more threads left, signal it
        gs_condAllDeleted->Signal();

        delete gs_condAllDeleted;
        gs_condAllDeleted = (wxCondition *)NULL;
    }
}

void wxMutexGuiEnter()
{
#if wxUSE_GUI
    gs_mutexGui->Lock();
#endif // wxUSE_GUI
}

void wxMutexGuiLeave()
{
#if wxUSE_GUI
    gs_mutexGui->Unlock();
#endif // wxUSE_GUI
}

#endif
  // wxUSE_THREADS
