/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/thread.cpp
// Purpose:     wxThread Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux/Vadim Zeitlin
// Modified by: Stefan Csomor
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998),
//                  Vadim Zeitlin (1999) , Stefan Csomor (2000)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/module.h"
#endif

#if wxUSE_THREADS

#include "wx/thread.h"

#ifdef __WXMAC__
#include <Threads.h>
#include "wx/mac/uma.h"
#include "wx/mac/macnotfy.h"
#include <Timer.h>
#endif

#define INFINITE 0xFFFFFFFF


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the possible states of the thread ("=>" shows all possible transitions from
// this state)
enum wxThreadState
{
    STATE_NEW,          // didn't start execution yet (=> RUNNING)
    STATE_RUNNING,      // thread is running (=> PAUSED, CANCELED)
    STATE_PAUSED,       // thread is temporarily suspended (=> RUNNING)
    STATE_CANCELED,     // thread should terminate a.s.a.p. (=> EXITED)
    STATE_EXITED        // thread is terminating
};

// ----------------------------------------------------------------------------
// this module globals
// ----------------------------------------------------------------------------

static ThreadID gs_idMainThread = kNoThreadID ;
static bool gs_waitingForThread = false ;
size_t g_numberOfThreads = 0;

// ============================================================================
// MacOS implementation of thread classes
// ============================================================================

class wxMacStCritical
{
public :
    wxMacStCritical()
    {
        if ( UMASystemIsInitialized() )
        {
            OSErr err = ThreadBeginCritical() ;
            wxASSERT( err == noErr ) ;
        }
    }
    ~wxMacStCritical()
    {
        if ( UMASystemIsInitialized() )
        {
            OSErr err = ThreadEndCritical() ;
            wxASSERT( err == noErr ) ;
    }
    }
};

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
    wxMutexInternal(wxMutexType WXUNUSED(mutexType))
    {
        m_owner = kNoThreadID ;
        m_locked = 0;
    }

    ~wxMutexInternal()
    {
        if ( m_locked > 0 )
        {
            wxLogDebug(_T("Warning: freeing a locked mutex (%ld locks)."), m_locked);
        }
    }

    bool IsOk() const { return true; }

    wxMutexError Lock() ;
    wxMutexError TryLock() ;
    wxMutexError Unlock();
public:
    ThreadID m_owner ;
    wxArrayLong m_waiters ;
    long m_locked ;
};

wxMutexError wxMutexInternal::Lock()
{
    wxMacStCritical critical ;
    if ( UMASystemIsInitialized() )
    {
        OSErr err ;
        ThreadID current = kNoThreadID;
        err = ::MacGetCurrentThread(&current);
        // if we are not the owner, add this thread to the list of waiting threads, stop this thread
        // and invoke the scheduler to continue executing the owner's thread
        while ( m_owner != kNoThreadID && m_owner != current)
        {
            m_waiters.Add(current);
            err = ::SetThreadStateEndCritical(kCurrentThreadID, kStoppedThreadState, m_owner);
            err = ::ThreadBeginCritical();
            wxASSERT( err == noErr ) ;
        }
        m_owner = current;
    }
    m_locked++;

    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::TryLock()
{
    wxMacStCritical critical ;
    if ( UMASystemIsInitialized() )
    {
        ThreadID current = kNoThreadID;
        ::MacGetCurrentThread(&current);
        // if we are not the owner, give an error back
        if ( m_owner != kNoThreadID && m_owner != current )
            return wxMUTEX_BUSY;

        m_owner = current;
    }
    m_locked++;

   return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::Unlock()
{
    if ( UMASystemIsInitialized() )
    {
        OSErr err;
        err = ::ThreadBeginCritical();
        wxASSERT( err == noErr ) ;

        if (m_locked > 0)
            m_locked--;

        // this mutex is not owned by anybody anmore
        m_owner = kNoThreadID;

        // now pass on to the first waiting thread
        ThreadID firstWaiting = kNoThreadID;
        bool found = false;
        while (!m_waiters.IsEmpty() && !found)
        {
            firstWaiting = m_waiters[0];
            err = ::SetThreadState(firstWaiting, kReadyThreadState, kNoThreadID);
            // in case this was not successful (dead thread), we just loop on and reset the id
            found = (err != threadNotFoundErr);
            if ( !found )
                firstWaiting = kNoThreadID ;
            m_waiters.RemoveAt(0) ;
        }
        // now we have a valid firstWaiting thread, which has been scheduled to run next, just end the
        // critical section and invoke the scheduler
        err = ::SetThreadStateEndCritical(kCurrentThreadID, kReadyThreadState, firstWaiting);
    }
    else
    {
        if (m_locked > 0)
            m_locked--;
    }
    return wxMUTEX_NO_ERROR;
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

// TODO not yet implemented

class wxSemaphoreInternal
{
public:
    wxSemaphoreInternal(int initialcount, int maxcount);
    ~wxSemaphoreInternal();

    bool IsOk() const { return true ; }

    wxSemaError Wait() { return WaitTimeout(INFINITE); }
    wxSemaError TryWait() { return WaitTimeout(0); }
    wxSemaError WaitTimeout(unsigned long milliseconds);

    wxSemaError Post();

private:
};

wxSemaphoreInternal::wxSemaphoreInternal(int initialcount, int maxcount)
{
    if ( maxcount == 0 )
    {
        // make it practically infinite
        maxcount = INT_MAX;
    }
}

wxSemaphoreInternal::~wxSemaphoreInternal()
{
}

wxSemaError wxSemaphoreInternal::WaitTimeout(unsigned long milliseconds)
{
    return wxSEMA_MISC_ERROR;
}

wxSemaError wxSemaphoreInternal::Post()
{
    return wxSEMA_MISC_ERROR;
}

// ----------------------------------------------------------------------------
// wxCondition implementation
// ----------------------------------------------------------------------------

// TODO this is not yet completed

class wxConditionInternal
{
public:
    wxConditionInternal(wxMutex& mutex) : m_mutex(mutex)
    {
        m_excessSignals = 0 ;
    }
    ~wxConditionInternal()
    {
    }

    bool IsOk() const { return m_mutex.IsOk() ; }

    wxCondError Wait()
    {
        return WaitTimeout(0xFFFFFFFF );
    }

    wxCondError WaitTimeout(unsigned long msectimeout)
    {
        wxMacStCritical critical ;
        if ( m_excessSignals > 0 )
        {
            --m_excessSignals ;
            return wxCOND_NO_ERROR ;
        }
        else if ( msectimeout == 0 )
        {
            return wxCOND_MISC_ERROR ;
        }
        else
        {
        }
        /*
        waiters++;

        // FIXME this should be MsgWaitForMultipleObjects() as well probably
        DWORD rc = ::WaitForSingleObject(event, timeout);

        waiters--;

        return rc != WAIT_TIMEOUT;
        */
        return wxCOND_NO_ERROR ;
    }
    wxCondError Signal()
    {
        wxMacStCritical critical ;
        return wxCOND_NO_ERROR;
    }

    wxCondError Broadcast()
    {
        wxMacStCritical critical ;
        return wxCOND_NO_ERROR;
    }

    wxArrayLong m_waiters ;
    wxInt32     m_excessSignals ;
    wxMutex&    m_mutex;
};

// ----------------------------------------------------------------------------
// wxCriticalSection implementation
// ----------------------------------------------------------------------------

// it's implemented as a mutex on mac os, so it is defined in the headers

// ----------------------------------------------------------------------------
// wxThread implementation
// ----------------------------------------------------------------------------

// wxThreadInternal class
// ----------------------

class wxThreadInternal
{
public:
    wxThreadInternal()
    {
        m_tid = kNoThreadID ;
        m_state = STATE_NEW;
        m_priority = WXTHREAD_DEFAULT_PRIORITY;
    }

    ~wxThreadInternal()
    {
    }

    void Free()
    {
    }

    // create a new (suspended) thread (for the given thread object)
    bool Create(wxThread *thread, unsigned int stackSize);

    // suspend/resume/terminate
    bool Suspend();
    bool Resume();
    void Cancel() { m_state = STATE_CANCELED; }

    // thread state
    void SetState(wxThreadState state) { m_state = state; }
    wxThreadState GetState() const { return m_state; }

    // thread priority
    void SetPriority(unsigned int priority);
    unsigned int GetPriority() const { return m_priority; }

    void SetResult( void *res ) { m_result = res ; }
    void *GetResult() { return m_result ; }

    // thread handle and id
    ThreadID  GetId() const { return m_tid; }

    // thread function
    static pascal void*    MacThreadStart(wxThread* arg);

private:
    wxThreadState           m_state;      // state, see wxThreadState enum
    unsigned int            m_priority;   // thread priority in "wx" units
    ThreadID                m_tid;        // thread id
    void*                   m_result;
    static ThreadEntryUPP   s_threadEntry ;
};

static wxArrayPtrVoid s_threads ;

ThreadEntryUPP wxThreadInternal::s_threadEntry = NULL ;
pascal void* wxThreadInternal::MacThreadStart(wxThread *thread)
{
    // first of all, check whether we hadn't been cancelled already
    if ( thread->m_internal->GetState() == STATE_EXITED )
    {
        return (void*)-1;
    }

    void* rc = thread->Entry();

    // enter m_critsect before changing the thread state
    thread->m_critsect.Enter();
    bool wasCancelled = thread->m_internal->GetState() == STATE_CANCELED;
    thread->m_internal->SetState(STATE_EXITED);
    thread->m_critsect.Leave();

    thread->OnExit();

    // if the thread was cancelled (from Delete()), then it the handle is still
    // needed there
    if ( thread->IsDetached() && !wasCancelled )
    {
        // auto delete
        delete thread;
    }
    //else: the joinable threads handle will be closed when Wait() is done

    return rc;
}
void wxThreadInternal::SetPriority(unsigned int priority)
{
    // Priorities don't exist on Mac
}

bool wxThreadInternal::Create(wxThread *thread, unsigned int stackSize)
{
    if ( s_threadEntry == NULL )
    {
        s_threadEntry = NewThreadEntryUPP( (ThreadEntryProcPtr) MacThreadStart ) ;
    }
    OSErr err = NewThread( kCooperativeThread,
                           s_threadEntry,
                           (void*) thread,
                           stackSize,
                           kNewSuspend,
                           &m_result,
                           &m_tid );

    if ( err != noErr )
    {
        wxLogSysError(_("Can't create thread"));
        return false;
    }

    if ( m_priority != WXTHREAD_DEFAULT_PRIORITY )
    {
        SetPriority(m_priority);
    }

    m_state = STATE_NEW;

    return true;
}

bool wxThreadInternal::Suspend()
{
    OSErr err ;

    err = ::ThreadBeginCritical();
    wxASSERT( err == noErr ) ;

    if ( m_state != STATE_RUNNING )
    {
        err = ::ThreadEndCritical() ;
        wxASSERT( err == noErr ) ;
        wxLogSysError(_("Can not suspend thread %x"), m_tid);
        return false;
    }

    m_state = STATE_PAUSED;

    err = ::SetThreadStateEndCritical(m_tid, kStoppedThreadState, kNoThreadID);

    return true;
}

bool wxThreadInternal::Resume()
{
    ThreadID current ;
    OSErr err ;
    err = MacGetCurrentThread( &current ) ;

    wxASSERT( err == noErr ) ;
    wxASSERT( current != m_tid ) ;

    err = ::ThreadBeginCritical();
    wxASSERT( err == noErr ) ;

    if ( m_state != STATE_PAUSED && m_state != STATE_NEW )
    {
        err = ::ThreadEndCritical() ;
        wxASSERT( err == noErr ) ;
        wxLogSysError(_("Can not resume thread %x"), m_tid);
        return false;

    }
    err = ::SetThreadStateEndCritical(m_tid, kReadyThreadState, kNoThreadID);

    m_state = STATE_RUNNING;
    err = ::ThreadEndCritical() ;
    wxASSERT( err == noErr ) ;
    ::YieldToAnyThread() ;
    return true;
}

// static functions
// ----------------
wxThread *wxThread::This()
{
    wxMacStCritical critical ;

    ThreadID current ;
    OSErr err ;

    err = MacGetCurrentThread( &current ) ;

    for ( size_t i = 0 ; i < s_threads.Count() ; ++i )
    {
        if ( ( (wxThread*) s_threads[i] )->GetId() == current )
            return (wxThread*) s_threads[i] ;
    }

    wxLogSysError(_("Couldn't get the current thread pointer"));
    return NULL;
}

bool wxThread::IsMain()
{
    ThreadID current ;
    OSErr err ;

    err = MacGetCurrentThread( &current ) ;
    return current == gs_idMainThread;
}

#ifdef Yield
#undef Yield
#endif

void wxThread::Yield()
{
    ::YieldToAnyThread() ;
}

void wxThread::Sleep(unsigned long milliseconds)
{
    UnsignedWide start, now;

    Microseconds(&start);

    double mssleep = milliseconds * 1000 ;
    double msstart, msnow ;
    msstart = (start.hi * 4294967296.0 + start.lo) ;

    do
    {
        YieldToAnyThread();
        Microseconds(&now);
        msnow = (now.hi * 4294967296.0 + now.lo) ;
    } while( msnow - msstart < mssleep );
}

int wxThread::GetCPUCount()
{
    // we will use whatever MP API will be used for the new MP Macs
    return 1;
}

unsigned long wxThread::GetCurrentId()
{
    ThreadID current ;
    MacGetCurrentThread( &current ) ;
    return (unsigned long)current;
}

bool wxThread::SetConcurrency(size_t level)
{
    wxASSERT_MSG( IsMain(), _T("should only be called from the main thread") );

    // ok only for the default one
    if ( level == 0 )
        return 0;

    // how many CPUs have we got?
    if ( GetCPUCount() == 1 )
    {
        // don't bother with all this complicated stuff - on a single
        // processor system it doesn't make much sense anyhow
        return level == 1;
    }

    return true ;
}

// ctor and dtor
// -------------

wxThread::wxThread(wxThreadKind kind)
{
    g_numberOfThreads++;
    m_internal = new wxThreadInternal();

    m_isDetached = kind == wxTHREAD_DETACHED;
    s_threads.Add( (void*) this ) ;
}

wxThread::~wxThread()
{
    if (g_numberOfThreads>0)
    {
        g_numberOfThreads--;
    }
#ifdef __WXDEBUG__
    else
    {
        wxFAIL_MSG(wxT("More threads deleted than created."));
    }
#endif

    s_threads.Remove( (void*) this ) ;
    if (m_internal != NULL) {
        delete m_internal;
        m_internal = NULL;
    }
}

// create/start thread
// -------------------

wxThreadError wxThread::Create(unsigned int stackSize)
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( !m_internal->Create(this, stackSize) )
        return wxTHREAD_NO_RESOURCE;

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( m_internal->GetState() != STATE_NEW )
    {
        // actually, it may be almost any state at all, not only STATE_RUNNING
        return wxTHREAD_RUNNING;
    }

    // the thread has just been created and is still suspended - let it run
    return Resume();
}

// suspend/resume thread
// ---------------------

wxThreadError wxThread::Pause()
{
    wxCriticalSectionLocker lock(m_critsect);

    return m_internal->Suspend() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

wxThreadError wxThread::Resume()
{
    wxCriticalSectionLocker lock(m_critsect);

    return m_internal->Resume() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

// stopping thread
// ---------------

wxThread::ExitCode wxThread::Wait()
{
    // although under MacOS we can wait for any thread, it's an error to
    // wait for a detached one in wxWin API
    wxCHECK_MSG( !IsDetached(), (ExitCode)-1,
                 _T("can't wait for detached thread") );

    ExitCode rc = (ExitCode)-1;

    (void)Delete(&rc);

    m_internal->Free();

    return rc;
}

wxThreadError wxThread::Delete(ExitCode *pRc)
{
    ExitCode rc = 0;

    // Delete() is always safe to call, so consider all possible states

    // has the thread started to run?
    bool shouldResume = false;

    {
        wxCriticalSectionLocker lock(m_critsect);

        if ( m_internal->GetState() == STATE_NEW )
        {
            // WinThreadStart() will see it and terminate immediately
            m_internal->SetState(STATE_EXITED);

            shouldResume = true;
        }
    }

    // is the thread paused?
    if ( shouldResume || IsPaused() )
        Resume();

    // does is still run?
    if ( IsRunning() )
    {
        if ( IsMain() )
        {
            // set flag for wxIsWaitingForThread()
            gs_waitingForThread = true;

#if wxUSE_GUI
            wxBeginBusyCursor();
#endif // wxUSE_GUI
        }

        // ask the thread to terminate
        {
            wxCriticalSectionLocker lock(m_critsect);

            m_internal->Cancel();
        }

#if wxUSE_GUI
        // simply wait for the thread to terminate
        while( TestDestroy() )
        {
            ::YieldToAnyThread() ;
        }
#else // !wxUSE_GUI
        // simply wait for the thread to terminate
        while( TestDestroy() )
        {
            ::YieldToAnyThread() ;
        }
#endif // wxUSE_GUI/!wxUSE_GUI

        if ( IsMain() )
        {
            gs_waitingForThread = false;

#if wxUSE_GUI
            wxEndBusyCursor();
#endif // wxUSE_GUI
        }
    }

    if ( IsDetached() )
    {
        // if the thread exits normally, this is done in WinThreadStart, but in
        // this case it would have been too early because
        // MsgWaitForMultipleObject() would fail if the therad handle was
        // closed while we were waiting on it, so we must do it here
        delete this;
    }

    if ( pRc )
        *pRc = rc;

    return rc == (ExitCode)-1 ? wxTHREAD_MISC_ERROR : wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Kill()
{
    if ( !IsRunning() )
        return wxTHREAD_NOT_RUNNING;

//    if ( !::TerminateThread(m_internal->GetHandle(), (DWORD)-1) )
    {
        wxLogSysError(_("Couldn't terminate thread"));

        return wxTHREAD_MISC_ERROR;
    }

    m_internal->Free();

    if ( IsDetached() )
    {
        delete this;
    }

    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(ExitCode status)
{
    m_internal->Free();

    if ( IsDetached() )
    {
        delete this;
    }

    m_internal->SetResult( status ) ;

/*
#if defined(__VISUALC__) || (defined(__BORLANDC__) && (__BORLANDC__ >= 0x500))
    _endthreadex((unsigned)status);
#else // !VC++
    ::ExitThread((DWORD)status);
#endif // VC++/!VC++
*/
    wxFAIL_MSG(wxT("Couldn't return from ExitThread()!"));
}

// priority setting
// ----------------

// since all these calls are execute cooperatively we don't have to use the critical section

void wxThread::SetPriority(unsigned int prio)
{
    m_internal->SetPriority(prio);
}

unsigned int wxThread::GetPriority() const
{
    return m_internal->GetPriority();
}

unsigned long wxThread::GetId() const
{
    return (unsigned long)m_internal->GetId();
}

bool wxThread::IsRunning() const
{
    return m_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    return (m_internal->GetState() == STATE_RUNNING) ||
           (m_internal->GetState() == STATE_PAUSED);
}

bool wxThread::IsPaused() const
{
    return m_internal->GetState() == STATE_PAUSED;
}

bool wxThread::TestDestroy()
{
    return m_internal->GetState() == STATE_CANCELED;
}

// ----------------------------------------------------------------------------
// Automatic initialization for thread module
// ----------------------------------------------------------------------------

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
    long response;
    bool hasThreadManager ;
    hasThreadManager = Gestalt( gestaltThreadMgrAttr, &response) == noErr && response & 1;
#if !TARGET_CARBON
#if GENERATINGCFM
    // verify presence of shared library
    hasThreadManager = hasThreadManager && ((Ptr)NewThread != (Ptr)kUnresolvedCFragSymbolAddress);
#endif
#endif
    if ( !hasThreadManager )
    {
        wxLogSysError( wxT("Thread Support is not available on this System") );
        return false ;
    }

    // no error return for GetCurrentThreadId()
    MacGetCurrentThread( &gs_idMainThread ) ;

    return true;
}

void wxThreadModule::OnExit()
{
}

// ----------------------------------------------------------------------------
// under MacOS we don't have currently preemptive threads, so any thread may access
// the GUI at any time
// ----------------------------------------------------------------------------

void WXDLLEXPORT wxMutexGuiEnter()
{
}

void WXDLLEXPORT wxMutexGuiLeave()
{
}

void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
}

bool WXDLLEXPORT wxGuiOwnedByMainThread()
{
    return false ;
}

// wake up the main thread
void WXDLLEXPORT wxWakeUpMainThread()
{
    wxMacWakeUp() ;
}

bool WXDLLEXPORT wxIsWaitingForThread()
{
    return false ;
}

#include "wx/thrimpl.cpp"

#endif // wxUSE_THREADS
