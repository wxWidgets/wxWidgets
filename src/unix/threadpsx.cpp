/////////////////////////////////////////////////////////////////////////////
// Name:        threadpsx.cpp
// Purpose:     wxThread (Posix) Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by: K. S. Sreeram (2002): POSIXified wxCondition, added wxSemaphore
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997)
//                  Guilhem Lavaux (1998)
//                  Vadim Zeitlin (1999-2002)
//                  Robert Roebling (1999)
//                  K. S. Sreeram (2002)
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

#include "wx/defs.h"

#if wxUSE_THREADS

#include "wx/thread.h"
#include "wx/module.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/timer.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#if HAVE_SCHED_H
    #include <sched.h>
#endif

#ifdef HAVE_THR_SETCONCURRENCY
    #include <thread.h>
#endif

// we use wxFFile under Linux in GetCPUCount()
#ifdef __LINUX__
    #include "wx/ffile.h"
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
static wxMutex *gs_mutexDeleteThread = (wxMutex *)NULL;

// and a condition variable which will be signaled when all
// gs_nThreadsBeingDeleted will have been deleted
static wxCondition *gs_condAllDeleted = (wxCondition *)NULL;

#if wxUSE_GUI
    // this mutex must be acquired before any call to a GUI function
    static wxMutex *gs_mutexGui;
#endif // wxUSE_GUI

// ============================================================================
// wxMutex implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMutexInternal
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
    wxMutexInternal();
    ~wxMutexInternal();

    wxMutexError Lock();
    wxMutexError TryLock();
    wxMutexError Unlock();

private:
    pthread_mutex_t m_mutex;

    friend class wxConditionInternal;
};

wxMutexInternal::wxMutexInternal()
{
    // support recursive locks like Win32, i.e. a thread can lock a mutex which
    // it had itself already locked
    //
    // but initialization of recursive mutexes is non portable <sigh>, so try
    // several methods
#ifdef HAVE_PTHREAD_MUTEXATTR_T
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_mutex, &attr);
#elif defined(HAVE_PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
    // we can use this only as initializer so we have to assign it first to a
    // temp var - assigning directly to m_mutex wouldn't even compile
    pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    m_mutex = mutex;
#else // no recursive mutexes
    pthread_mutex_init(&m_mutex, NULL);

    // used by TryLock() below
    #define NO_RECURSIVE_MUTEXES
#endif // HAVE_PTHREAD_MUTEXATTR_T/...
}

wxMutexInternal::~wxMutexInternal()
{
    pthread_mutex_destroy(&m_mutex);
}

wxMutexError wxMutexInternal::Lock()
{
    int err = pthread_mutex_lock(&m_mutex);
    switch ( err )
    {
        case EDEADLK:
            wxLogDebug(wxT("Locking this mutex would lead to deadlock!"));
            return wxMUTEX_DEAD_LOCK;

        default:
            wxFAIL_MSG( _T("unexpected pthread_mutex_lock() return") );
            // fall through

        case EINVAL:
            wxLogDebug(_T("Failed to lock the mutex."));
            return wxMUTEX_MISC_ERROR;

        case 0:
            return wxMUTEX_NO_ERROR;
    }
}

wxMutexError wxMutexInternal::TryLock()
{
    int err = pthread_mutex_trylock(&m_mutex);
    switch ( err )
    {
        case EBUSY:
            return wxMUTEX_BUSY;

        default:
            wxFAIL_MSG( _T("unexpected pthread_mutex_trylock() return") );
            // fall through

        case EINVAL:
            wxLogDebug(_T("Failed to try to lock the mutex."));
            return wxMUTEX_MISC_ERROR;

        case 0:
            return wxMUTEX_NO_ERROR;
    }
}

wxMutexError wxMutexInternal::Unlock()
{
    int err = pthread_mutex_unlock(&m_mutex);
    switch ( err )
    {
        case EPERM:
            // we don't own the mutex
            return wxMUTEX_UNLOCKED;

        default:
            wxFAIL_MSG( _T("unexpected pthread_mutex_unlock() return") );
            // fall through

        case EINVAL:
            wxLogDebug(_T("Failed to unlock the mutex."));
            return wxMUTEX_MISC_ERROR;

        case 0:
            return wxMUTEX_NO_ERROR;
    }
}

// ----------------------------------------------------------------------------
// wxMutex
// ----------------------------------------------------------------------------

// TODO: this is completely generic, move it to common code?

wxMutex::wxMutex()
{
    m_internal = new wxMutexInternal;

    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if ( m_locked > 0 )
        wxLogDebug(wxT("Freeing a locked mutex (%d locks)"), m_locked);

    delete m_internal;
}

wxMutexError wxMutex::Lock()
{
    wxMutexError err = m_internal->Lock();

    if ( !err )
    {
        m_locked++;
    }

    return err;
}

wxMutexError wxMutex::TryLock()
{
    if ( m_locked )
    {
#ifdef NO_RECURSIVE_MUTEXES
        return wxMUTEX_DEAD_LOCK;
#else // have recursive mutexes on this platform
        // we will succeed in locking it when we have it already locked
        return wxMUTEX_NO_ERROR;
#endif // recursive/non-recursive mutexes
    }

    wxMutexError err = m_internal->TryLock();
    if ( !err )
    {
        m_locked++;
    }

    return err;
}

wxMutexError wxMutex::Unlock()
{
    if ( m_locked > 0 )
    {
        m_locked--;
    }
    else
    {
        wxLogDebug(wxT("Unlocking not locked mutex."));

        return wxMUTEX_UNLOCKED;
    }

    return m_internal->Unlock();
}

// ===========================================================================
// wxCondition implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxConditionInternal
// ---------------------------------------------------------------------------

class wxConditionInternal
{
public:
    wxConditionInternal( wxMutex *mutex );
    ~wxConditionInternal();

    void Wait();

    bool Wait( const timespec *ts );

    void Signal();

    void Broadcast();

private:

    wxMutex *m_mutex;
    pthread_cond_t m_cond;
};

wxConditionInternal::wxConditionInternal( wxMutex *mutex )
{
    m_mutex = mutex;
    if ( pthread_cond_init( &m_cond, NULL ) != 0 )
    {
        wxLogDebug(_T("pthread_cond_init() failed"));
    }
}

wxConditionInternal::~wxConditionInternal()
{
    if ( pthread_cond_destroy( &m_cond ) != 0 )
    {
        wxLogDebug(_T("pthread_cond_destroy() failed"));
    }
}

void wxConditionInternal::Wait()
{
    if ( pthread_cond_wait( &m_cond, &(m_mutex->m_internal->m_mutex) ) != 0 )
    {
        wxLogDebug(_T("pthread_cond_wait() failed"));
    }
}

bool wxConditionInternal::Wait( const timespec *ts )
{
    int result = pthread_cond_timedwait( &m_cond,
                                         &(m_mutex->m_internal->m_mutex),
                                         ts );
    if ( result == ETIMEDOUT )
        return FALSE;

    wxASSERT_MSG( result == 0, _T("pthread_cond_timedwait() failed") );

    return TRUE;
}

void wxConditionInternal::Signal()
{
    int result = pthread_cond_signal( &m_cond );
    if ( result != 0 )
{
        wxFAIL_MSG( _T("pthread_cond_signal() failed") );
    }
}

void wxConditionInternal::Broadcast()
{
    int result = pthread_cond_broadcast( &m_cond );
    if ( result != 0 )
{
        wxFAIL_MSG( _T("pthread_cond_broadcast() failed") );
    }
}


// ---------------------------------------------------------------------------
// wxCondition
// ---------------------------------------------------------------------------

wxCondition::wxCondition( wxMutex *mutex )
{
    if ( !mutex )
    {
        wxFAIL_MSG( _T("NULL mutex in wxCondition ctor") );

        m_internal = NULL;
    }
    else
    {
        m_internal = new wxConditionInternal( mutex );
    }
}

wxCondition::~wxCondition()
{
    delete m_internal;
}

void wxCondition::Wait()
{
    if ( m_internal )
        m_internal->Wait();
}

bool wxCondition::Wait( unsigned long timeout_millis )
{
    wxCHECK_MSG( m_internal, FALSE, _T("can't wait on uninitalized condition") );

    wxLongLong curtime = wxGetLocalTimeMillis();
    curtime += timeout_millis;
    wxLongLong temp = curtime / 1000;
    int sec = temp.GetLo();
    temp = temp * 1000;
    temp = curtime - temp;
    int millis = temp.GetLo();

    timespec tspec;

    tspec.tv_sec = sec;
    tspec.tv_nsec = millis * 1000L * 1000L;

    return m_internal->Wait(&tspec);
}

void wxCondition::Signal()
{
    if ( m_internal )
        m_internal->Signal();
}

void wxCondition::Broadcast()
{
    if ( m_internal )
        m_internal->Broadcast();
}

// ===========================================================================
// wxSemaphore implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxSemaphoreInternal
// ---------------------------------------------------------------------------

class wxSemaphoreInternal
{
public:
    wxSemaphoreInternal( int initialcount, int maxcount );

    void Wait();
    bool TryWait();

    bool Wait( unsigned long timeout_millis );

    void Post();

private:
    wxMutex m_mutex;
    wxCondition m_cond;

    int count,
        maxcount;
};

wxSemaphoreInternal::wxSemaphoreInternal( int initialcount, int maxcount )
                   : m_cond(m_mutex)
{

    if ( (initialcount < 0) || ((maxcount > 0) && (initialcount > maxcount)) )
    {
        wxFAIL_MSG( _T("wxSemaphore: invalid initial count") );
    }

    maxcount = maxcount;
    count = initialcount;
}

void wxSemaphoreInternal::Wait()
{
    wxMutexLocker locker(*m_mutex);

    while ( count <= 0 )
    {
        m_cond->Wait();
    }

    count--;
}

bool wxSemaphoreInternal::TryWait()
{
    wxMutexLocker locker(*m_mutex);

    if ( count <= 0 )
        return FALSE;

    count--;

    return TRUE;
}

bool wxSemaphoreInternal::Wait( unsigned long timeout_millis )
{
    wxMutexLocker locker( *m_mutex );

    wxLongLong startTime = wxGetLocalTimeMillis();

    while ( count <= 0 )
    {
        wxLongLong elapsed = wxGetLocalTimeMillis() - startTime;
        long remainingTime = (long)timeout_millis - (long)elapsed.GetLo();
        if ( remainingTime <= 0 )
            return FALSE;

        bool result = m_cond->Wait( remainingTime );
        if ( !result )
            return FALSE;
    }

    count--;

    return TRUE;
}

void wxSemaphoreInternal::Post()
{
    wxMutexLocker locker(*m_mutex);

    if ( (maxcount > 0) && (count == maxcount) )
    {
        wxFAIL_MSG( _T("wxSemaphore::Post() overflow") );
    }

    count++;

    m_cond->Signal();
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

wxSemaphore::wxSemaphore( int initialcount, int maxcount )
{
    m_internal = new wxSemaphoreInternal( initialcount, maxcount );
}

wxSemaphore::~wxSemaphore()
{
    delete m_internal;
}

void wxSemaphore::Wait()
{
    m_internal->Wait();
}

bool wxSemaphore::TryWait()
{
    return m_internal->TryWait();
}

bool wxSemaphore::Wait( unsigned long timeout_millis )
{
    return m_internal->Wait( timeout_millis );
}

void wxSemaphore::Post()
{
    m_internal->Post();
}

// This class is used by wxThreadInternal to support Delete() on
// a detached thread
class wxRefCountedCondition
{
public:
    // start with a initial reference count of 1
    wxRefCountedCondition()
    {
        m_refCount = 1;
        m_signaled = FALSE;

        m_mutex = new wxMutex();
        m_cond = new wxCondition( m_mutex );
    }

    // increment the reference count
    void AddRef()
    {
        wxMutexLocker locker( *m_mutex );

        m_refCount++;
    }

    // decrement the reference count if reference count is zero then delete the
    // object
    void DeleteRef()
    {
        bool shouldDelete = FALSE;

        m_mutex->Lock();

        if ( --m_refCount == 0 )
        {
            shouldDelete = TRUE;
        }

        m_mutex->Unlock();

        if ( shouldDelete )
        {
            delete this;
        }
    }


    // sets the object to signaled this signal will be a persistent signal all
    // further Wait()s on the object will return without blocking
    void SetSignaled()
    {
        wxMutexLocker locker( *m_mutex );

        m_signaled = TRUE;

        m_cond->Broadcast();
    }

    // wait till the object is signaled if the object was already signaled then
    // return immediately
    void Wait()
    {
        wxMutexLocker locker( *m_mutex );

        if ( !m_signaled )
        {
            m_cond->Wait();
        }
    }

private:
    int m_refCount;

    wxMutex *m_mutex;
    wxCondition *m_cond;

    bool m_signaled;

    // Cannot delete this object directly, call DeleteRef() instead
    ~wxRefCountedCondition()
    {
        delete m_cond;
        delete m_mutex;
    }

    // suppress gcc warning about the class having private dtor and not having
    // friend (so what??)
    friend class wxDummyFriend;
};

// ===========================================================================
// wxThread implementation
// ===========================================================================

// the thread callback functions must have the C linkage
extern "C"
{

#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // thread exit function
    void wxPthreadCleanup(void *ptr);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

void *wxPthreadStart(void *ptr);

} // extern "C"

// ----------------------------------------------------------------------------
// wxThreadInternal
// ----------------------------------------------------------------------------

class wxThreadInternal
{
public:
    wxThreadInternal();
    ~wxThreadInternal();

    // thread entry function
    static void *PthreadStart(wxThread *thread);

    // thread actions
        // start the thread
    wxThreadError Run();
        // ask the thread to terminate
    void Wait();
        // wake up threads waiting for our termination
    void SignalExit();
        // wake up threads waiting for our start
    void SignalRun() { m_semRun.Post(); }
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

        // the pause flag
    void SetReallyPaused(bool paused) { m_isPaused = paused; }
    bool IsReallyPaused() const { return m_isPaused; }

        // tell the thread that it is a detached one
    void Detach()
    {
        m_shouldBeJoined = m_shouldBroadcast = FALSE;
        m_isDetached = TRUE;
    }
        // but even detached threads need to notifyus about their termination
        // sometimes - tell the thread that it should do it
    void Notify() { m_shouldBroadcast = TRUE; }

#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // this is used by wxPthreadCleanup() only
    static void Cleanup(wxThread *thread);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

private:
    pthread_t     m_threadId;   // id of the thread
    wxThreadState m_state;      // see wxThreadState enum
    int           m_prio;       // in wxWindows units: from 0 to 100

    // this flag is set when the thread should terminate
    bool m_cancelled;

    // this flag is set when the thread is blocking on m_semSuspend
    bool m_isPaused;

    // the thread exit code - only used for joinable (!detached) threads and
    // is only valid after the thread termination
    wxThread::ExitCode m_exitcode;

    // many threads may call Wait(), but only one of them should call
    // pthread_join(), so we have to keep track of this
    wxCriticalSection m_csJoinFlag;
    bool m_shouldBeJoined;
    bool m_shouldBroadcast;
    bool m_isDetached;

    // this semaphore is posted by Run() and the threads Entry() is not
    // called before it is done
    wxSemaphore m_semRun;

    // this one is signaled when the thread should resume after having been
    // Pause()d
    wxSemaphore m_semSuspend;

    // finally this one is signalled when the thread exits
    // we are using a reference counted condition to support
    // Delete() for a detached thread
    wxRefCountedCondition *m_condEnd;
};

// ----------------------------------------------------------------------------
// thread startup and exit functions
// ----------------------------------------------------------------------------

void *wxPthreadStart(void *ptr)
{
    return wxThreadInternal::PthreadStart((wxThread *)ptr);
}

void *wxThreadInternal::PthreadStart(wxThread *thread)
{
    wxThreadInternal *pthread = thread->m_internal;

    wxLogTrace(TRACE_THREADS, _T("Thread %ld started."), pthread->GetId());

    // associate the thread pointer with the newly created thread so that
    // wxThread::This() will work
    int rc = pthread_setspecific(gs_keySelf, thread);
    if ( rc != 0 )
    {
        wxLogSysError(rc, _("Cannot start thread: error writing TLS"));

        return (void *)-1;
    }

    // have to declare this before pthread_cleanup_push() which defines a
    // block!
    bool dontRunAtAll;

#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // install the cleanup handler which will be called if the thread is
    // cancelled
    pthread_cleanup_push(wxPthreadCleanup, thread);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

    // wait for the semaphore to be posted from Run()
    pthread->m_semRun.Wait();

    // test whether we should run the run at all - may be it was deleted
    // before it started to Run()?
    {
        wxCriticalSectionLocker lock(thread->m_critsect);

        dontRunAtAll = pthread->GetState() == STATE_NEW &&
                       pthread->WasCancelled();
    }

    if ( !dontRunAtAll )
    {
        // call the main entry
        pthread->m_exitcode = thread->Entry();

        wxLogTrace(TRACE_THREADS, _T("Thread %ld left its Entry()."),
                   pthread->GetId());

        {
            wxCriticalSectionLocker lock(thread->m_critsect);

            wxLogTrace(TRACE_THREADS, _T("Thread %ld changes state to EXITED."),
                       pthread->GetId());

            // change the state of the thread to "exited" so that
            // wxPthreadCleanup handler won't do anything from now (if it's
            // called before we do pthread_cleanup_pop below)
            pthread->SetState(STATE_EXITED);
        }
    }

    // NB: at least under Linux, pthread_cleanup_push/pop are macros and pop
    //     contains the matching '}' for the '{' in push, so they must be used
    //     in the same block!
#if HAVE_THREAD_CLEANUP_FUNCTIONS
    // remove the cleanup handler without executing it
    pthread_cleanup_pop(FALSE);
#endif // HAVE_THREAD_CLEANUP_FUNCTIONS

    if ( dontRunAtAll )
    {
        delete thread;

        return EXITCODE_CANCELLED;
    }
    else
    {
        // terminate the thread
        thread->Exit(pthread->m_exitcode);

        wxFAIL_MSG(wxT("wxThread::Exit() can't return."));

        return NULL;
    }
}

#if HAVE_THREAD_CLEANUP_FUNCTIONS

// this handler is called when the thread is cancelled
extern "C" void wxPthreadCleanup(void *ptr)
{
    wxThreadInternal::Cleanup((wxThread *)ptr);
}

void wxThreadInternal::Cleanup(wxThread *thread)
{
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

    // set to TRUE only when the thread starts waiting on m_semSuspend
    m_isPaused = FALSE;

    // defaults for joinable threads
    m_shouldBeJoined = TRUE;
    m_shouldBroadcast = TRUE;
    m_isDetached = FALSE;

    m_condEnd = new wxRefCountedCondition();
}

wxThreadInternal::~wxThreadInternal()
{
    m_condEnd->DeleteRef();
}

wxThreadError wxThreadInternal::Run()
{
    wxCHECK_MSG( GetState() == STATE_NEW, wxTHREAD_RUNNING,
                 wxT("thread may only be started once after Create()") );

    SignalRun();

    SetState(STATE_RUNNING);

    return wxTHREAD_NO_ERROR;
}

void wxThreadInternal::Wait()
{
    // if the thread we're waiting for is waiting for the GUI mutex, we will
    // deadlock so make sure we release it temporarily
    if ( wxThread::IsMain() )
        wxMutexGuiLeave();

    bool isDetached = m_isDetached;
    wxThreadIdType id = (wxThreadIdType) GetId();

    wxLogTrace(TRACE_THREADS,
               _T("Starting to wait for thread %ld to exit."), id);

    // wait until the thread terminates (we're blocking in _another_ thread,
    // of course)

    // a reference counting condition is used to handle the
    // case where a detached thread deletes itself
    // before m_condEnd->Wait() returns
    // in this case the deletion of the condition object is deferred until
    // all Wait()ing threads have finished calling DeleteRef()
    m_condEnd->AddRef();
    m_condEnd->Wait();
    m_condEnd->DeleteRef();

    wxLogTrace(TRACE_THREADS, _T("Finished waiting for thread %ld."), id);

    // we can't use any member variables any more if the thread is detached
    // because it could be already deleted
    if ( !isDetached )
    {
        // to avoid memory leaks we should call pthread_join(), but it must
        // only be done once
        wxCriticalSectionLocker lock(m_csJoinFlag);

        if ( m_shouldBeJoined )
        {
            // FIXME shouldn't we set cancellation type to DISABLED here? If
            //       we're cancelled inside pthread_join(), things will almost
            //       certainly break - but if we disable the cancellation, we
            //       might deadlock
            if ( pthread_join((pthread_t)id, &m_exitcode) != 0 )
            {
                wxLogError(_("Failed to join a thread, potential memory leak "
                             "detected - please restart the program"));
            }

            m_shouldBeJoined = FALSE;
        }
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
        wxLogTrace(TRACE_THREADS, _T("Thread %ld signals end condition."),
                   GetId());

        m_condEnd->SetSignaled();
    }
}

void wxThreadInternal::Pause()
{
    // the state is set from the thread which pauses us first, this function
    // is called later so the state should have been already set
    wxCHECK_RET( m_state == STATE_PAUSED,
                 wxT("thread must first be paused with wxThread::Pause().") );

    wxLogTrace(TRACE_THREADS, _T("Thread %ld goes to sleep."), GetId());

    // wait until the semaphore is Post()ed from Resume()
    m_semSuspend.Wait();
}

void wxThreadInternal::Resume()
{
    wxCHECK_RET( m_state == STATE_PAUSED,
                 wxT("can't resume thread which is not suspended.") );

    // the thread might be not actually paused yet - if there were no call to
    // TestDestroy() since the last call to Pause() for example
    if ( IsReallyPaused() )
    {
        wxLogTrace(TRACE_THREADS, _T("Waking up thread %ld"), GetId());

        // wake up Pause()
        m_semSuspend.Post();

        // reset the flag
        SetReallyPaused(FALSE);
    }
    else
    {
        wxLogTrace(TRACE_THREADS, _T("Thread %ld is not yet really paused"),
                   GetId());
    }

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
#ifdef HAVE_SCHED_YIELD
    sched_yield();
#endif
}

void wxThread::Sleep(unsigned long milliseconds)
{
    wxUsleep(milliseconds);
}

int wxThread::GetCPUCount()
{
#if defined(__LINUX__) && wxUSE_FFILE
    // read from proc (can't use wxTextFile here because it's a special file:
    // it has 0 size but still can be read from)
    wxLogNull nolog;

    wxFFile file(_T("/proc/cpuinfo"));
    if ( file.IsOpened() )
    {
        // slurp the whole file
        wxString s;
        if ( file.ReadAll(&s) )
        {
            // (ab)use Replace() to find the number of "processor" strings
            size_t count = s.Replace(_T("processor"), _T(""));
            if ( count > 0 )
            {
                return count;
            }

            wxLogDebug(_T("failed to parse /proc/cpuinfo"));
        }
        else
        {
            wxLogDebug(_T("failed to read /proc/cpuinfo"));
        }
    }
#elif defined(_SC_NPROCESSORS_ONLN)
    // this works for Solaris
    int rc = sysconf(_SC_NPROCESSORS_ONLN);
    if ( rc != -1 )
    {
        return rc;
    }
#endif // different ways to get number of CPUs

    // unknown
    return -1;
}

#ifdef __VMS
  // VMS is a 64 bit system and threads have 64 bit pointers.
  // ??? also needed for other systems????
unsigned long long wxThread::GetCurrentId()
{
    return (unsigned long long)pthread_self();
#else
unsigned long wxThread::GetCurrentId()
{
    return (unsigned long)pthread_self();
#endif
}

bool wxThread::SetConcurrency(size_t level)
{
#ifdef HAVE_THR_SETCONCURRENCY
    int rc = thr_setconcurrency(level);
    if ( rc != 0 )
    {
        wxLogSysError(rc, _T("thr_setconcurrency() failed"));
    }

    return rc == 0;
#else // !HAVE_THR_SETCONCURRENCY
    // ok only for the default value
    return level == 0;
#endif // HAVE_THR_SETCONCURRENCY/!HAVE_THR_SETCONCURRENCY
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

wxThreadError wxThread::Create(unsigned int WXUNUSED(stackSize))
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

#ifdef __VMS__
   /* the pthread.h contains too many spaces. This is a work-around */
# undef sched_get_priority_max
#undef sched_get_priority_min
#define sched_get_priority_max(_pol_) \
     (_pol_ == SCHED_OTHER ? PRI_FG_MAX_NP : PRI_FIFO_MAX)
#define sched_get_priority_min(_pol_) \
     (_pol_ == SCHED_OTHER ? PRI_FG_MIN_NP : PRI_FIFO_MIN)
#endif

    int max_prio = sched_get_priority_max(policy),
        min_prio = sched_get_priority_min(policy),
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
                wxPthreadStart,
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

wxThreadIdType wxThread::GetId() const
{
    return (wxThreadIdType) m_internal->GetId();
}

// -----------------------------------------------------------------------------
// pause/resume
// -----------------------------------------------------------------------------

wxThreadError wxThread::Pause()
{
    wxCHECK_MSG( This() != this, wxTHREAD_MISC_ERROR,
                 _T("a thread can't pause itself") );

    wxCriticalSectionLocker lock(m_critsect);

    if ( m_internal->GetState() != STATE_RUNNING )
    {
        wxLogDebug(wxT("Can't pause thread which is not running."));

        return wxTHREAD_NOT_RUNNING;
    }

    wxLogTrace(TRACE_THREADS, _T("Asking thread %ld to pause."),
               GetId());

    // just set a flag, the thread will be really paused only during the next
    // call to TestDestroy()
    m_internal->SetState(STATE_PAUSED);

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    wxCHECK_MSG( This() != this, wxTHREAD_MISC_ERROR,
                 _T("a thread can't resume itself") );

    wxCriticalSectionLocker lock(m_critsect);

    wxThreadState state = m_internal->GetState();

    switch ( state )
    {
        case STATE_PAUSED:
            wxLogTrace(TRACE_THREADS, _T("Thread %ld suspended, resuming."),
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
    wxCHECK_MSG( This() != this, wxTHREAD_MISC_ERROR,
                 _T("a thread can't delete itself") );

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
            // we need to wake up the thread so that PthreadStart() will
            // terminate - right now it's blocking on m_semRun
            m_internal->SignalRun();

            // fall through

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
                // wxPthreadCleanup()
#if !HAVE_THREAD_CLEANUP_FUNCTIONS
                ScheduleThreadForDeletion();

                // don't call OnExit() here, it can only be called in the
                // threads context and we're in the context of another thread

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
    wxASSERT_MSG( This() == this,
                  _T("wxThread::Exit() can only be called in the "
                     "context of the same thread") );

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
    wxASSERT_MSG( This() == this,
                  _T("wxThread::TestDestroy() can only be called in the "
                     "context of the same thread") );

    m_critsect.Enter();

    if ( m_internal->GetState() == STATE_PAUSED )
    {
        m_internal->SetReallyPaused(TRUE);

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
        wxLogDebug(_T("The thread %ld is being destroyed although it is still "
                      "running! The application may crash."), GetId());
    }

    m_critsect.Leave();
#endif // __WXDEBUG__

    delete m_internal;

    // remove this thread from the global array
    gs_allThreads.Remove(this);

    // detached thread will decrement this counter in DeleteThread(), but it
    // is not called for the joinable threads, so do it here
    if ( !m_isDetached )
    {
        wxMutexLocker lock( *gs_mutexDeleteThread );

        gs_nThreadsBeingDeleted--;

        wxLogTrace(TRACE_THREADS, _T("%u scheduled for deletion threads left."),
                   gs_nThreadsBeingDeleted - 1);
    }
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

    gs_mutexDeleteThread = new wxMutex();
    gs_condAllDeleted = new wxCondition( gs_mutexDeleteThread );

    return TRUE;
}

void wxThreadModule::OnExit()
{
    wxASSERT_MSG( wxThread::IsMain(), wxT("only main thread can be here") );

    // are there any threads left which are being deleted right now?
    size_t nThreadsBeingDeleted;

    {
        wxMutexLocker lock( *gs_mutexDeleteThread );
        nThreadsBeingDeleted = gs_nThreadsBeingDeleted;

        if ( nThreadsBeingDeleted > 0 )
{
            wxLogTrace(TRACE_THREADS, _T("Waiting for %u threads to disappear"),
                           nThreadsBeingDeleted);

            // have to wait until all of them disappear
            gs_condAllDeleted->Wait();
        }
    }

    // terminate any threads left
    size_t count = gs_allThreads.GetCount();
    if ( count != 0u )
    {
        wxLogDebug(wxT("%u threads were not terminated by the application."),
                   count);
    }

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

    delete gs_condAllDeleted;
    delete gs_mutexDeleteThread;
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static void ScheduleThreadForDeletion()
{
    wxMutexLocker lock( *gs_mutexDeleteThread );

    gs_nThreadsBeingDeleted++;

    wxLogTrace(TRACE_THREADS, _T("%u thread%s waiting to be deleted"),
               gs_nThreadsBeingDeleted,
               gs_nThreadsBeingDeleted == 1 ? "" : "s");
}

static void DeleteThread(wxThread *This)
{
    // gs_mutexDeleteThread should be unlocked before signalling the condition
    // or wxThreadModule::OnExit() would deadlock
    wxMutexLocker locker( *gs_mutexDeleteThread );

    wxLogTrace(TRACE_THREADS, _T("Thread %ld auto deletes."), This->GetId());

    delete This;

    wxCHECK_RET( gs_nThreadsBeingDeleted > 0,
                     _T("no threads scheduled for deletion, yet we delete "
                        "one?") );

    wxLogTrace(TRACE_THREADS, _T("%u scheduled for deletion threads left."),
               gs_nThreadsBeingDeleted - 1);

    if ( !--gs_nThreadsBeingDeleted )
    {
        // no more threads left, signal it
        gs_condAllDeleted->Signal();
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
