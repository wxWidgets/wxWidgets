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

#include "wx/thread.h"

#if !wxUSE_THREADS
    #error This file needs wxUSE_THREADS
#endif

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

#ifdef HAVE_SCHED_H
    #include <sched.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum thread_state
{
    STATE_NEW,          // didn't start execution yet (=> RUNNING)
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_CANCELED,
    STATE_EXITED
};

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

// this mutex must be acquired before any call to a GUI function
static wxMutex *gs_mutexGui;

// ============================================================================
// implementation
// ============================================================================

//--------------------------------------------------------------------
// wxMutex (Posix implementation)
//--------------------------------------------------------------------

class wxMutexInternal
{
public:
    pthread_mutex_t p_mutex;
};

wxMutex::wxMutex()
{
    p_internal = new wxMutexInternal;
    pthread_mutex_init( &(p_internal->p_mutex), (const pthread_mutexattr_t*) NULL );
    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxLogDebug("Freeing a locked mutex (%d locks)", m_locked);

    pthread_mutex_destroy( &(p_internal->p_mutex) );
    delete p_internal;
}

wxMutexError wxMutex::Lock()
{
    int err = pthread_mutex_lock( &(p_internal->p_mutex) );
    if (err == EDEADLK)
    {
        wxLogDebug("Locking this mutex would lead to deadlock!");

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

    int err = pthread_mutex_trylock( &(p_internal->p_mutex) );
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
        wxLogDebug("Unlocking not locked mutex.");

        return wxMUTEX_UNLOCKED;
    }

    pthread_mutex_unlock( &(p_internal->p_mutex) );

    return wxMUTEX_NO_ERROR;
}

//--------------------------------------------------------------------
// wxCondition (Posix implementation)
//--------------------------------------------------------------------

class wxConditionInternal
{
public:
  pthread_cond_t p_condition;
};

wxCondition::wxCondition()
{
    p_internal = new wxConditionInternal;
    pthread_cond_init( &(p_internal->p_condition), (const pthread_condattr_t *) NULL );
}

wxCondition::~wxCondition()
{
    pthread_cond_destroy( &(p_internal->p_condition) );

    delete p_internal;
}

void wxCondition::Wait(wxMutex& mutex)
{
    pthread_cond_wait( &(p_internal->p_condition), &(mutex.p_internal->p_mutex) );
}

bool wxCondition::Wait(wxMutex& mutex, unsigned long sec, unsigned long nsec)
{
    struct timespec tspec;

    tspec.tv_sec = time(0L)+sec;
    tspec.tv_nsec = nsec;
    return (pthread_cond_timedwait(&(p_internal->p_condition), &(mutex.p_internal->p_mutex), &tspec) != ETIMEDOUT);
}

void wxCondition::Signal()
{
    pthread_cond_signal( &(p_internal->p_condition) );
}

void wxCondition::Broadcast()
{
    pthread_cond_broadcast( &(p_internal->p_condition) );
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

    // thread actions
        // start the thread
    wxThreadError Run();
        // ask the thread to terminate
    void Cancel();
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
    thread_state GetState() const { return m_state; }
    void SetState(thread_state state) { m_state = state; }
        // id
    pthread_t GetId() const { return thread_id; }
        // "cancelled" flag
    bool WasCancelled() const { return m_cancelled; }

//private: -- should be!
    pthread_t thread_id;

private:
    thread_state m_state;    // see thread_state enum
    int          m_prio;     // in wxWindows units: from 0 to 100

    // set when the thread should terminate
    bool m_cancelled;

    // this (mutex, cond) pair is used to synchronize the main thread and this
    // thread in several situations:
    //  1. The thread function blocks until condition is signaled by Run() when
    //     it's initially created - this allows create thread in "suspended"
    //     state
    //  2. The Delete() function blocks until the condition is signaled when the
    //     thread exits.
    wxMutex     m_mutex;
    wxCondition m_cond;

    // another (mutex, cond) pair for Pause()/Resume() usage
    //
    // VZ: it's possible that we might reuse the mutex and condition from above
    //     for this too, but as I'm not at all sure that it won't create subtle
    //     problems with race conditions between, say, Pause() and Delete() I
    //     prefer this may be a bit less efficient but much safer solution
    wxMutex     m_mutexSuspend;
    wxCondition m_condSuspend;
};

void *wxThreadInternal::PthreadStart(void *ptr)
{
    wxThread *thread = (wxThread *)ptr;
    wxThreadInternal *pthread = thread->p_internal;

    if ( pthread_setspecific(gs_keySelf, thread) != 0 )
    {
        wxLogError(_("Can not start thread: error writing TLS."));

        return (void *)-1;
    }

    // wait for the condition to be signaled from Run()
    // mutex state: currently locked by the thread which created us
    pthread->m_cond.Wait(pthread->m_mutex);

    // mutex state: locked again on exit of Wait()

    // call the main entry
    void* status = thread->Entry();

    // terminate the thread
    thread->Exit(status);

    wxFAIL_MSG("wxThread::Exit() can't return.");

    return NULL;
}

wxThreadInternal::wxThreadInternal()
{
    m_state = STATE_NEW;
    m_cancelled = FALSE;

    // this mutex is locked during almost all thread lifetime - it will only be
    // unlocked in the very end
    m_mutex.Lock();

    // this mutex is used in Pause()/Resume() and is also locked all the time
    // unless the thread is paused
    m_mutexSuspend.Lock();
}

wxThreadInternal::~wxThreadInternal()
{
    m_mutexSuspend.Unlock();

    // note that m_mutex will be unlocked by the thread which waits for our
    // termination
}

wxThreadError wxThreadInternal::Run()
{
    wxCHECK_MSG( GetState() == STATE_NEW, wxTHREAD_RUNNING,
                 "thread may only be started once after successful Create()" );

    // the mutex was locked on Create(), so we will be able to lock it again
    // only when the thread really starts executing and enters the wait -
    // otherwise we might signal the condition before anybody is waiting for it
    wxMutexLocker lock(m_mutex);
    m_cond.Signal();

    m_state = STATE_RUNNING;

    return wxTHREAD_NO_ERROR;

    // now the mutex is unlocked back - but just to allow Wait() function to
    // terminate by relocking it, so the net result is that the worker thread
    // starts executing and the mutex is still locked
}

void wxThreadInternal::Cancel()
{
    // if the thread we're waiting for is waiting for the GUI mutex, we will
    // deadlock so make sure we release it temporarily
    if ( wxThread::IsMain() )
        wxMutexGuiLeave();

    // nobody ever writes this variable so it's safe to not use any
    // synchronization here
    m_cancelled = TRUE;

    // entering Wait() releases the mutex thus allowing SignalExit() to acquire
    // it and to signal us its termination
    m_cond.Wait(m_mutex);

    // mutex is still in the locked state - relocked on exit from Wait(), so
    // unlock it - we don't need it any more, the thread has already terminated
    m_mutex.Unlock();

    // reacquire GUI mutex
    if ( wxThread::IsMain() )
        wxMutexGuiEnter();
}

void wxThreadInternal::SignalExit()
{
    // as mutex is currently locked, this will block until some other thread
    // (normally the same which created this one) unlocks it by entering Wait()
    m_mutex.Lock();

    // wake up all the threads waiting for our termination
    m_cond.Broadcast();

    // after this call mutex will be finally unlocked
    m_mutex.Unlock();
}

void wxThreadInternal::Pause()
{
    wxCHECK_RET( m_state == STATE_PAUSED,
                 "thread must first be paused with wxThread::Pause()." );

    // wait until the condition is signaled from Resume()
    m_condSuspend.Wait(m_mutexSuspend);
}

void wxThreadInternal::Resume()
{
    wxCHECK_RET( m_state == STATE_PAUSED,
                 "can't resume thread which is not suspended." );

    // we will be able to lock this mutex only when Pause() starts waiting
    wxMutexLocker lock(m_mutexSuspend);
    m_condSuspend.Signal();

    SetState(STATE_RUNNING);
}

// -----------------------------------------------------------------------------
// static functions
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

wxThread::wxThread()
{
    // add this thread to the global list of all threads
    gs_allThreads.Add(this);

    p_internal = new wxThreadInternal();
}

wxThreadError wxThread::Create()
{
    if (p_internal->GetState() != STATE_NEW)
        return wxTHREAD_RUNNING;

    // set up the thread attribute: right now, we only set thread priority
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int prio;
    if ( pthread_attr_getschedpolicy(&attr, &prio) != 0 )
    {
        wxLogError(_("Can not retrieve thread scheduling policy."));
    }

    int min_prio = sched_get_priority_min(prio),
        max_prio = sched_get_priority_max(prio);

    if ( min_prio == -1 || max_prio == -1 )
    {
        wxLogError(_("Can not get priority range for scheduling policy %d."),
                   prio);
    }
    else
    {
        struct sched_param sp;
        pthread_attr_getschedparam(&attr, &sp);
        sp.sched_priority = min_prio +
                           (p_internal->GetPriority()*(max_prio-min_prio))/100;
        pthread_attr_setschedparam(&attr, &sp);
    }

    // create the new OS thread object
    int rc = pthread_create(&p_internal->thread_id, &attr,
                            wxThreadInternal::PthreadStart, (void *)this);
    pthread_attr_destroy(&attr);

    if ( rc != 0 )
    {
        p_internal->SetState(STATE_EXITED);
        return wxTHREAD_NO_RESOURCE;
    }

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
    return p_internal->Run();
}

// -----------------------------------------------------------------------------
// misc accessors
// -----------------------------------------------------------------------------

void wxThread::SetPriority(unsigned int prio)
{
    wxCHECK_RET( (WXTHREAD_MIN_PRIORITY <= prio) &&
                 (prio <= WXTHREAD_MAX_PRIORITY), "invalid thread priority" );

    wxCriticalSectionLocker lock(m_critsect);

    switch ( p_internal->GetState() )
    {
        case STATE_NEW:
            // thread not yet started, priority will be set when it is
            p_internal->SetPriority(prio);
            break;

        case STATE_RUNNING:
        case STATE_PAUSED:
            {
                struct sched_param sparam;
                sparam.sched_priority = prio;

                if ( pthread_setschedparam(p_internal->GetId(),
                                           SCHED_OTHER, &sparam) != 0 )
                {
                    wxLogError(_("Failed to set thread priority %d."), prio);
                }
            }
            break;

        case STATE_EXITED:
        default:
            wxFAIL_MSG("impossible to set thread priority in this state");
    }
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return p_internal->GetPriority();
}

unsigned long wxThread::GetID() const
{
    return (unsigned long)p_internal->thread_id;
}

// -----------------------------------------------------------------------------
// pause/resume
// -----------------------------------------------------------------------------

wxThreadError wxThread::Pause()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( p_internal->GetState() != STATE_RUNNING )
    {
        wxLogDebug("Can't pause thread which is not running.");

        return wxTHREAD_NOT_RUNNING;
    }

    p_internal->SetState(STATE_PAUSED);

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( p_internal->GetState() == STATE_PAUSED )
    {
        p_internal->Resume();

        return wxTHREAD_NO_ERROR;
    }
    else
    {
        wxLogDebug("Attempt to resume a thread which is not paused.");

        return wxTHREAD_MISC_ERROR;
    }
}

// -----------------------------------------------------------------------------
// exiting thread
// -----------------------------------------------------------------------------

wxThread::ExitCode wxThread::Delete()
{
    m_critsect.Enter();
    thread_state state = p_internal->GetState();
    m_critsect.Leave();

    switch ( state )
    {
        case STATE_NEW:
        case STATE_EXITED:
            // nothing to do
            break;

        case STATE_PAUSED:
            // resume the thread first
            Resume();

            // fall through

        default:
            // set the flag telling to the thread to stop and wait
            p_internal->Cancel();
    }

    return NULL;
}

wxThreadError wxThread::Kill()
{
    switch ( p_internal->GetState() )
    {
        case STATE_NEW:
        case STATE_EXITED:
            return wxTHREAD_NOT_RUNNING;

        default:
            if ( pthread_cancel(p_internal->GetId()) != 0 )
            {
                wxLogError(_("Failed to terminate a thread."));

                return wxTHREAD_MISC_ERROR;
            }

            return wxTHREAD_NO_ERROR;
    }
}

void wxThread::Exit(void *status)
{
    // first call user-level clean up code
    OnExit();

    // next wake up the threads waiting for us (OTOH, this function won't return
    // until someone waited for us!)
    p_internal->SignalExit();

    p_internal->SetState(STATE_EXITED);

    // delete both C++ thread object and terminate the OS thread object
    delete this;
    pthread_exit(status);
}

// also test whether we were paused
bool wxThread::TestDestroy()
{
    wxCriticalSectionLocker lock((wxCriticalSection&)m_critsect);

    if ( p_internal->GetState() == STATE_PAUSED )
    {
        // leave the crit section or the other threads will stop too if they try
        // to call any of (seemingly harmless) IsXXX() functions while we sleep
        m_critsect.Leave();

        p_internal->Pause();

        // enter it back before it's finally left in lock object dtor
        m_critsect.Enter();
    }

    return p_internal->WasCancelled();
}

wxThread::~wxThread()
{
    // remove this thread from the global array
    gs_allThreads.Remove(this);
}

// -----------------------------------------------------------------------------
// state tests
// -----------------------------------------------------------------------------

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return p_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker lock((wxCriticalSection&)m_critsect);

    switch ( p_internal->GetState() )
    {
        case STATE_RUNNING:
        case STATE_PAUSED:
            return TRUE;

        default:
            return FALSE;
    }
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
    if ( pthread_key_create(&gs_keySelf, NULL /* dtor function */) != 0 )
    {
        wxLogError(_("Thread module initialization failed: "
                     "failed to create pthread key."));

        return FALSE;
    }

    gs_mutexGui = new wxMutex();

    //wxThreadGuiInit();

    gs_tidMain = pthread_self();
    gs_mutexGui->Lock();

    return TRUE;
}

void wxThreadModule::OnExit()
{
    wxASSERT_MSG( wxThread::IsMain(), "only main thread can be here" );

    // terminate any threads left
    size_t count = gs_allThreads.GetCount();
    if ( count != 0u )
        wxLogDebug("Some threads were not terminated by the application.");

    for ( size_t n = 0u; n < count; n++ )
    {
        gs_allThreads[n]->Delete();
    }

    // destroy GUI mutex
    gs_mutexGui->Unlock();

    //wxThreadGuiExit();

    delete gs_mutexGui;

    // and free TLD slot
    (void)pthread_key_delete(gs_keySelf);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

void wxMutexGuiEnter()
{
  gs_mutexGui->Lock();
}

void wxMutexGuiLeave()
{
  gs_mutexGui->Unlock();
}
