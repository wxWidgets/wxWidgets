/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxThread Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux/Vadim Zeitlin
// Modified by: Stefan Csomor
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998),
//                  Vadim Zeitlin (1999) , Stefan Csomor (2000)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "thread.h"
#endif

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
#endif

#if wxUSE_THREADS

#include "wx/module.h"
#include "wx/thread.h"

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
static bool gs_waitingForThread = FALSE ;

// ============================================================================
// MacOS implementation of thread classes
// ============================================================================

class wxMacStCritical
{
public :
	wxMacStCritical() 
	{
		ThreadBeginCritical() ;
	}
	~wxMacStCritical()
	{
		ThreadEndCritical() ;
	}
} ;

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
    wxMutexInternal()
    {
    	m_owner = kNoThreadID ;
    }

    ~wxMutexInternal() 
    {
    }

public:
    ThreadID m_owner ;
    wxArrayLong m_waiters ;
};

wxMutex::wxMutex()
{
    m_internal = new wxMutexInternal;

    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if ( m_locked > 0 )
    {
        wxLogDebug(_T("Warning: freeing a locked mutex (%d locks)."), m_locked);
    }

    delete m_internal;
}

wxMutexError wxMutex::Lock()
{
	wxMacStCritical critical ;
	
	OSErr err ;
	ThreadID current = kNoThreadID;
	err = ::MacGetCurrentThread(&current);
	// if we are not the owner, add this thread to the list of waiting threads, stop this thread
	// and invoke the scheduler to continue executing the owner's thread
	while ( m_internal->m_owner != kNoThreadID && m_internal->m_owner != current) 
	{
		m_internal->m_waiters.Add(current);
		err = ::SetThreadStateEndCritical(kCurrentThreadID, kStoppedThreadState, m_internal->m_owner);
		err = ::ThreadBeginCritical();
	}
	m_internal->m_owner = current;
    m_locked++;

    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
	wxMacStCritical critical ;
	
	OSErr err ;
	ThreadID current = kNoThreadID;
	::MacGetCurrentThread(&current);
	// if we are not the owner, give an error back
	if ( m_internal->m_owner != kNoThreadID && m_internal->m_owner != current ) 
        return wxMUTEX_BUSY;
		
	m_internal->m_owner = current;
    m_locked++;

   return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
	OSErr err;
	err = ::ThreadBeginCritical();
	
    if (m_locked > 0)
        m_locked--;

	// this mutex is not owned by anybody anmore
	m_internal->m_owner = kNoThreadID;

	// now pass on to the first waiting thread
	ThreadID firstWaiting = kNoThreadID;
	bool found = false;
	while (!m_internal->m_waiters.IsEmpty() && !found) 
	{
		firstWaiting = m_internal->m_waiters[0];
		err = ::SetThreadState(firstWaiting, kReadyThreadState, kNoThreadID);
		// in case this was not successful (dead thread), we just loop on and reset the id
		found = (err != threadNotFoundErr);	
		if ( !found )
			firstWaiting = kNoThreadID ;
		m_internal->m_waiters.RemoveAt(0) ;
	}
	// now we have a valid firstWaiting thread, which has been scheduled to run next, just end the
	// critical section and invoke the scheduler
	err = ::SetThreadStateEndCritical(kCurrentThreadID, kReadyThreadState, firstWaiting);

    return wxMUTEX_NO_ERROR;
}

// ----------------------------------------------------------------------------
// wxCondition implementation
// ----------------------------------------------------------------------------

class wxConditionInternal
{
public:
    wxConditionInternal()
    {
    	m_excessSignals = 0 ;
    }
    ~wxConditionInternal()
    {
    }

    bool Wait(unsigned long msectimeout)
    {
		wxMacStCritical critical ;
    	if ( m_excessSignals > 0 )
    	{
    		--m_excessSignals ;
    		return TRUE ;
    	}
    	else if ( msectimeout == 0 )
    	{
    		return FALSE ;
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
        return TRUE ;
    }
	void Signal()
	{
		wxMacStCritical critical ;
	}

    wxArrayLong m_waiters ;
    wxInt32		m_excessSignals ;
};

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
    (void)m_internal->Wait(0xFFFFFFFFL );
}

bool wxCondition::Wait(unsigned long sec,
                       unsigned long nsec)
{
    return m_internal->Wait(sec*1000 + nsec/1000000);
}

void wxCondition::Signal()
{
    // set the event to signaled: if a thread is already waiting on it, it will
    // be woken up, otherwise the event will remain in the signaled state until
    // someone waits on it. In any case, the system will return it to a non
    // signalled state afterwards. If multiple threads are waiting, only one
    // will be woken up.
	m_internal->Signal() ;
}

void wxCondition::Broadcast()
{
    // this works because all these threads are already waiting and so each
    // SetEvent() inside Signal() is really a PulseEvent() because the event
    // state is immediately returned to non-signaled
    for ( int i = 0; i < m_internal->m_waiters.Count(); i++ )
    {
        Signal();
    }
}

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
    bool Create(wxThread *thread);

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
	static pascal void*	MacThreadStart(wxThread* arg);

private:
    wxThreadState 	m_state;      // state, see wxThreadState enum
    unsigned int  	m_priority;   // thread priority in "wx" units
    ThreadID        m_tid;        // thread id
    void *			m_result ;
    static ThreadEntryUPP s_threadEntry ;
public :
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

bool wxThreadInternal::Create(wxThread *thread)
{
	if ( s_threadEntry == NULL )
	{
		s_threadEntry = NewThreadEntryUPP( (ThreadEntryProcPtr) MacThreadStart ) ;
	}
	OSErr err = NewThread(kCooperativeThread,
		s_threadEntry,
		(void*) thread ,
		0 ,
		kNewSuspend ,
		&m_result ,
		&m_tid ) ;

    if ( err != noErr )
    {
        wxLogSysError(_("Can't create thread"));
        return FALSE;
    }

    if ( m_priority != WXTHREAD_DEFAULT_PRIORITY )
    {
        SetPriority(m_priority);
    }

    return TRUE;
}

bool wxThreadInternal::Suspend()
{
	OSErr err ;
	
	::ThreadBeginCritical();

	if ( m_state != STATE_RUNNING )
    {
    	::ThreadEndCritical() ;
        wxLogSysError(_("Can not suspend thread %x"), m_tid);
        return FALSE;
    }

    m_state = STATE_PAUSED;

	err = ::SetThreadStateEndCritical(m_tid, kStoppedThreadState, kNoThreadID);

    return TRUE;
}

bool wxThreadInternal::Resume()
{
	ThreadID current ;
	OSErr err ;
	err = MacGetCurrentThread( &current ) ;

	wxASSERT( err == noErr ) ;
	wxASSERT( current != m_tid ) ;
		
	::ThreadBeginCritical();
	if ( m_state != STATE_PAUSED && m_state != STATE_NEW )
	{
    	::ThreadEndCritical() ;
        wxLogSysError(_("Can not resume thread %x"), m_tid);
        return FALSE;
		
	}
	err = ::SetThreadStateEndCritical(m_tid, kReadyThreadState, kNoThreadID);
	wxASSERT( err == noErr ) ;
	
    m_state = STATE_RUNNING;
	::ThreadEndCritical() ;
	::YieldToAnyThread() ;
    return TRUE;
}

// static functions
// ----------------
wxThread *wxThread::This()
{
	wxMacStCritical critical ;
	
	ThreadID current ;
	OSErr err ;
	
	err = MacGetCurrentThread( &current ) ;
	
	for ( int i = 0 ; i < s_threads.Count() ; ++i )
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
		clock_t start = clock() ;
		do 
		{
			YieldToAnyThread() ;
		} while( clock() - start < milliseconds / CLOCKS_PER_SEC ) ;
}

int wxThread::GetCPUCount()
{
	// we will use whatever MP API will be used for the new MP Macs
    return 1;
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
    
    return TRUE ;
}

// ctor and dtor
// -------------

wxThread::wxThread(wxThreadKind kind)
{
    m_internal = new wxThreadInternal();

    m_isDetached = kind == wxTHREAD_DETACHED;
    s_threads.Add( (void*) this ) ;
}

wxThread::~wxThread()
{
	s_threads.Remove( (void*) this ) ;
    delete m_internal;
}

// create/start thread
// -------------------

wxThreadError wxThread::Create()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( !m_internal->Create(this) )
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
    bool shouldResume = FALSE;

    {
        wxCriticalSectionLocker lock(m_critsect);

        if ( m_internal->GetState() == STATE_NEW )
        {
            // WinThreadStart() will see it and terminate immediately
            m_internal->SetState(STATE_EXITED);

            shouldResume = TRUE;
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
            gs_waitingForThread = TRUE;

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
            gs_waitingForThread = FALSE;

#if wxUSE_GUI
            wxEndBusyCursor();
#endif // wxUSE_GUI
        }
    }

 //   if ( !::GetExitCodeThread(hThread, (LPDWORD)&rc) )
    {
        wxLogLastError("GetExitCodeThread");

        rc = (ExitCode)-1;
    }

    if ( IsDetached() )
    {
        // if the thread exits normally, this is done in WinThreadStart, but in
        // this case it would have been too early because
        // MsgWaitForMultipleObject() would fail if the therad handle was
        // closed while we were waiting on it, so we must do it here
        delete this;
    }

 //   wxASSERT_MSG( (DWORD)rc != STILL_ACTIVE,
 //                 wxT("thread must be already terminated.") );

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
#if GENERATINGPOWERPC || GENERATINGCFM
	// verify presence of shared library
	hasThreadManager = hasThreadManager && ((Ptr)NewThread != (Ptr)kUnresolvedCFragSymbolAddress);
#endif
	if ( !hasThreadManager )
	{
		wxMessageBox( "Error" , "Thread Support is not available on this System" , wxOK ) ;
		return FALSE ;
	}

    // no error return for GetCurrentThreadId()
    MacGetCurrentThread( &gs_idMainThread ) ;

    return TRUE;
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

#endif // wxUSE_THREADS

