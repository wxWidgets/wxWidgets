/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxThread Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by: Vadim Zeitlin to make it work :-)
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998),
//                  Vadim Zeitlin (1999)
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

#include "wx/msw/private.h"

#include "wx/module.h"
#include "wx/thread.h"

// must have this symbol defined to get _beginthread/_endthread declarations
#ifndef _MT
    #define _MT
#endif

#ifdef __VISUALC__
    #include <process.h>
#endif

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

// TLS index of the slot where we store the pointer to the current thread
static DWORD gs_tlsThisThread = 0xFFFFFFFF;

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
static DWORD gs_idMainThread = 0;

// if it's FALSE, some secondary thread is holding the GUI lock
static bool gs_bGuiOwnedByMainThread = TRUE;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *gs_critsectGui = NULL;

// critical section which protects gs_nWaitingForGui variable
static wxCriticalSection *gs_critsectWaitingForGui = NULL;

// number of threads waiting for GUI in wxMutexGuiEnter()
static size_t gs_nWaitingForGui = 0;

// are we waiting for a thread termination?
static bool gs_waitingForThread = FALSE;

// ============================================================================
// Windows implementation of thread classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
    HANDLE p_mutex;
};

wxMutex::wxMutex()
{
    m_internal = new wxMutexInternal;
    m_internal->p_mutex = CreateMutex(NULL, FALSE, NULL);
    if ( !m_internal->p_mutex )
    {
        wxLogSysError(_("Can not create mutex."));
    }

    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxLogDebug(wxT("Warning: freeing a locked mutex (%d locks)."), m_locked);
    CloseHandle(m_internal->p_mutex);
}

wxMutexError wxMutex::Lock()
{
    DWORD ret;

    ret = WaitForSingleObject(m_internal->p_mutex, INFINITE);
    switch ( ret )
    {
        case WAIT_ABANDONED:
            return wxMUTEX_BUSY;

        case WAIT_OBJECT_0:
            // ok
            break;

        case WAIT_FAILED:
            wxLogSysError(_("Couldn't acquire a mutex lock"));
            return wxMUTEX_MISC_ERROR;

        case WAIT_TIMEOUT:
        default:
            wxFAIL_MSG(wxT("impossible return value in wxMutex::Lock"));
    }

    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
    DWORD ret;

    ret = WaitForSingleObject(m_internal->p_mutex, 0);
    if (ret == WAIT_TIMEOUT || ret == WAIT_ABANDONED)
        return wxMUTEX_BUSY;

    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
    if (m_locked > 0)
        m_locked--;

    BOOL ret = ReleaseMutex(m_internal->p_mutex);
    if ( ret == 0 )
    {
        wxLogSysError(_("Couldn't release a mutex"));
        return wxMUTEX_MISC_ERROR;
    }

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
        event = ::CreateEvent(
                              NULL,   // default secutiry
                              FALSE,  // not manual reset
                              FALSE,  // nonsignaled initially
                              NULL    // nameless event
                             );
        if ( !event )
        {
            wxLogSysError(_("Can not create event object."));
        }
        waiters = 0;
    }

    bool Wait(DWORD timeout)
    {
        waiters++;

        // FIXME this should be MsgWaitForMultipleObjects() as well probably
        DWORD rc = ::WaitForSingleObject(event, timeout);

        waiters--;

        return rc != WAIT_TIMEOUT;
    }

    ~wxConditionInternal()
    {
        if ( event )
        {
            if ( !::CloseHandle(event) )
            {
                wxLogLastError("CloseHandle(event)");
            }
        }
    }

    HANDLE event;
    int waiters;
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
    (void)m_internal->Wait(INFINITE);
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
    if ( !::SetEvent(m_internal->event) )
    {
        wxLogLastError("SetEvent");
    }
}

void wxCondition::Broadcast()
{
    // this works because all these threads are already waiting and so each
    // SetEvent() inside Signal() is really a PulseEvent() because the event
    // state is immediately returned to non-signaled
    for ( int i = 0; i < m_internal->waiters; i++ )
    {
        Signal();
    }
}

// ----------------------------------------------------------------------------
// wxCriticalSection implementation
// ----------------------------------------------------------------------------

wxCriticalSection::wxCriticalSection()
{
    wxASSERT_MSG( sizeof(CRITICAL_SECTION) <= sizeof(m_buffer),
                  _T("must increase buffer size in wx/thread.h") );

    ::InitializeCriticalSection((CRITICAL_SECTION *)m_buffer);
}

wxCriticalSection::~wxCriticalSection()
{
    ::DeleteCriticalSection((CRITICAL_SECTION *)m_buffer);
}

void wxCriticalSection::Enter()
{
    ::EnterCriticalSection((CRITICAL_SECTION *)m_buffer);
}

void wxCriticalSection::Leave()
{
    ::LeaveCriticalSection((CRITICAL_SECTION *)m_buffer);
}

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
        m_hThread = 0;
        m_state = STATE_NEW;
        m_priority = WXTHREAD_DEFAULT_PRIORITY;
    }

    ~wxThreadInternal()
    {
        Free();
    }

    void Free()
    {
        if ( m_hThread )
        {
            if ( !::CloseHandle(m_hThread) )
            {
                wxLogLastError("CloseHandle(thread)");
            }

            m_hThread = 0;
        }
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

    // thread handle and id
    HANDLE GetHandle() const { return m_hThread; }
    DWORD  GetId() const { return m_tid; }

    // thread function
    static DWORD WinThreadStart(wxThread *thread);

private:
    HANDLE        m_hThread;    // handle of the thread
    wxThreadState m_state;      // state, see wxThreadState enum
    unsigned int  m_priority;   // thread priority in "wx" units
    DWORD         m_tid;        // thread id
};

DWORD wxThreadInternal::WinThreadStart(wxThread *thread)
{
    // store the thread object in the TLS
    if ( !::TlsSetValue(gs_tlsThisThread, thread) )
    {
        wxLogSysError(_("Can not start thread: error writing TLS."));

        return (DWORD)-1;
    }

    DWORD rc = (DWORD)thread->Entry();

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
    m_priority = priority;

    // translate wxWindows priority to the Windows one
    int win_priority;
    if (m_priority <= 20)
        win_priority = THREAD_PRIORITY_LOWEST;
    else if (m_priority <= 40)
        win_priority = THREAD_PRIORITY_BELOW_NORMAL;
    else if (m_priority <= 60)
        win_priority = THREAD_PRIORITY_NORMAL;
    else if (m_priority <= 80)
        win_priority = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (m_priority <= 100)
        win_priority = THREAD_PRIORITY_HIGHEST;
    else
    {
        wxFAIL_MSG(wxT("invalid value of thread priority parameter"));
        win_priority = THREAD_PRIORITY_NORMAL;
    }

    if ( !::SetThreadPriority(m_hThread, win_priority) )
    {
        wxLogSysError(_("Can't set thread priority"));
    }
}

bool wxThreadInternal::Create(wxThread *thread)
{
    // for compilers which have it, we should use C RTL function for thread
    // creation instead of Win32 API one because otherwise we will have memory
    // leaks if the thread uses C RTL (and most threads do)
#ifdef __VISUALC__
    typedef unsigned (__stdcall *RtlThreadStart)(void *);

    m_hThread = (HANDLE)_beginthreadex(NULL, 0,
                                       (RtlThreadStart)    
                                       wxThreadInternal::WinThreadStart,
                                       thread, CREATE_SUSPENDED,
                                       (unsigned int *)&m_tid);
#else // !VC++
    m_hThread = ::CreateThread
                  (
                    NULL,                               // default security
                    0,                                  // default stack size
                    (LPTHREAD_START_ROUTINE)            // thread entry point
                    wxThreadInternal::WinThreadStart,   //
                    (LPVOID)thread,                     // parameter
                    CREATE_SUSPENDED,                   // flags
                    &m_tid                              // [out] thread id
                  );
#endif // VC++/!VC++

    if ( m_hThread == NULL )
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
    DWORD nSuspendCount = ::SuspendThread(m_hThread);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not suspend thread %x"), m_hThread);

        return FALSE;
    }

    m_state = STATE_PAUSED;

    return TRUE;
}

bool wxThreadInternal::Resume()
{
    DWORD nSuspendCount = ::ResumeThread(m_hThread);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not resume thread %x"), m_hThread);

        return FALSE;
    }

    m_state = STATE_RUNNING;

    return TRUE;
}

// static functions
// ----------------

wxThread *wxThread::This()
{
    wxThread *thread = (wxThread *)::TlsGetValue(gs_tlsThisThread);

    // be careful, 0 may be a valid return value as well
    if ( !thread && (::GetLastError() != NO_ERROR) )
    {
        wxLogSysError(_("Couldn't get the current thread pointer"));

        // return NULL...
    }

    return thread;
}

bool wxThread::IsMain()
{
    return ::GetCurrentThreadId() == gs_idMainThread;
}

void wxThread::Yield()
{
    // 0 argument to Sleep() is special and means to just give away the rest of
    // our timeslice
    ::Sleep(0);
}

void wxThread::Sleep(unsigned long milliseconds)
{
    ::Sleep(milliseconds);
}

// ctor and dtor
// -------------

wxThread::wxThread(wxThreadKind kind)
{
    m_internal = new wxThreadInternal();

    m_isDetached = kind == wxTHREAD_DETACHED;
}

wxThread::~wxThread()
{
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
    // although under Windows we can wait for any thread, it's an error to
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
    if ( IsPaused() )
        Resume();

    HANDLE hThread = m_internal->GetHandle();

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
        // we can't just wait for the thread to terminate because it might be
        // calling some GUI functions and so it will never terminate before we
        // process the Windows messages that result from these functions
        DWORD result;
        do
        {
            result = ::MsgWaitForMultipleObjects
                     (
                       1,              // number of objects to wait for
                       &hThread,       // the objects
                       FALSE,          // don't wait for all objects
                       INFINITE,       // no timeout
                       QS_ALLEVENTS    // return as soon as there are any events
                     );

            switch ( result )
            {
                case 0xFFFFFFFF:
                    // error
                    wxLogSysError(_("Can not wait for thread termination"));
                    Kill();
                    return wxTHREAD_KILLED;

                case WAIT_OBJECT_0:
                    // thread we're waiting for terminated
                    break;

                case WAIT_OBJECT_0 + 1:
                    // new message arrived, process it
                    if ( !wxTheApp->DoMessage() )
                    {
                        // WM_QUIT received: kill the thread
                        Kill();

                        return wxTHREAD_KILLED;
                    }

                    if ( IsMain() )
                    {
                        // give the thread we're waiting for chance to exit
                        // from the GUI call it might have been in
                        if ( (gs_nWaitingForGui > 0) && wxGuiOwnedByMainThread() )
                        {
                            wxMutexGuiLeave();
                        }
                    }

                    break;

                default:
                    wxFAIL_MSG(wxT("unexpected result of MsgWaitForMultipleObject"));
            }
        } while ( result != WAIT_OBJECT_0 );
#else // !wxUSE_GUI
        // simply wait for the thread to terminate
        //
        // OTOH, even console apps create windows (in wxExecute, for WinSock
        // &c), so may be use MsgWaitForMultipleObject() too here?
        if ( WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0 )
        {
            wxFAIL_MSG(wxT("unexpected result of WaitForSingleObject"));
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

    if ( !::GetExitCodeThread(hThread, (LPDWORD)&rc) )
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

    wxASSERT_MSG( (DWORD)rc != STILL_ACTIVE,
                  wxT("thread must be already terminated.") );

    if ( pRc )
        *pRc = rc;

    return rc == (ExitCode)-1 ? wxTHREAD_MISC_ERROR : wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Kill()
{
    if ( !IsRunning() )
        return wxTHREAD_NOT_RUNNING;

    if ( !::TerminateThread(m_internal->GetHandle(), (DWORD)-1) )
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

#ifdef __VISUALC__
    _endthreadex((unsigned)status);
#else // !VC++
    ::ExitThread((DWORD)status);
#endif // VC++/!VC++

    wxFAIL_MSG(wxT("Couldn't return from ExitThread()!"));
}

// priority setting
// ----------------

void wxThread::SetPriority(unsigned int prio)
{
    wxCriticalSectionLocker lock(m_critsect);

    m_internal->SetPriority(prio);
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return m_internal->GetPriority();
}

unsigned long wxThread::GetId() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return (unsigned long)m_internal->GetId();
}

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return m_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return (m_internal->GetState() == STATE_RUNNING) ||
           (m_internal->GetState() == STATE_PAUSED);
}

bool wxThread::IsPaused() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return m_internal->GetState() == STATE_PAUSED;
}

bool wxThread::TestDestroy()
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

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
    // allocate TLS index for storing the pointer to the current thread
    gs_tlsThisThread = ::TlsAlloc();
    if ( gs_tlsThisThread == 0xFFFFFFFF )
    {
        // in normal circumstances it will only happen if all other
        // TLS_MINIMUM_AVAILABLE (>= 64) indices are already taken - in other
        // words, this should never happen
        wxLogSysError(_("Thread module initialization failed: "
                        "impossible to allocate index in thread "
                        "local storage"));

        return FALSE;
    }

    // main thread doesn't have associated wxThread object, so store 0 in the
    // TLS instead
    if ( !::TlsSetValue(gs_tlsThisThread, (LPVOID)0) )
    {
        ::TlsFree(gs_tlsThisThread);
        gs_tlsThisThread = 0xFFFFFFFF;

        wxLogSysError(_("Thread module initialization failed: "
                        "can not store value in thread local storage"));

        return FALSE;
    }

    gs_critsectWaitingForGui = new wxCriticalSection();

    gs_critsectGui = new wxCriticalSection();
    gs_critsectGui->Enter();

    // no error return for GetCurrentThreadId()
    gs_idMainThread = ::GetCurrentThreadId();

    return TRUE;
}

void wxThreadModule::OnExit()
{
    if ( !::TlsFree(gs_tlsThisThread) )
    {
        wxLogLastError("TlsFree failed.");
    }

    if ( gs_critsectGui )
    {
        gs_critsectGui->Leave();
        delete gs_critsectGui;
        gs_critsectGui = NULL;
    }

    delete gs_critsectWaitingForGui;
    gs_critsectWaitingForGui = NULL;
}

// ----------------------------------------------------------------------------
// under Windows, these functions are implemented using a critical section and
// not a mutex, so the names are a bit confusing
// ----------------------------------------------------------------------------

void WXDLLEXPORT wxMutexGuiEnter()
{
    // this would dead lock everything...
    wxASSERT_MSG( !wxThread::IsMain(),
                  wxT("main thread doesn't want to block in wxMutexGuiEnter()!") );

    // the order in which we enter the critical sections here is crucial!!

    // set the flag telling to the main thread that we want to do some GUI
    {
        wxCriticalSectionLocker enter(*gs_critsectWaitingForGui);

        gs_nWaitingForGui++;
    }

    wxWakeUpMainThread();

    // now we may block here because the main thread will soon let us in
    // (during the next iteration of OnIdle())
    gs_critsectGui->Enter();
}

void WXDLLEXPORT wxMutexGuiLeave()
{
    wxCriticalSectionLocker enter(*gs_critsectWaitingForGui);

    if ( wxThread::IsMain() )
    {
        gs_bGuiOwnedByMainThread = FALSE;
    }
    else
    {
        // decrement the number of waiters now
        wxASSERT_MSG( gs_nWaitingForGui > 0,
                      wxT("calling wxMutexGuiLeave() without entering it first?") );

        gs_nWaitingForGui--;

        wxWakeUpMainThread();
    }

    gs_critsectGui->Leave();
}

void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
    wxASSERT_MSG( wxThread::IsMain(),
                  wxT("only main thread may call wxMutexGuiLeaveOrEnter()!") );

    wxCriticalSectionLocker enter(*gs_critsectWaitingForGui);

    if ( gs_nWaitingForGui == 0 )
    {
        // no threads are waiting for GUI - so we may acquire the lock without
        // any danger (but only if we don't already have it)
        if ( !wxGuiOwnedByMainThread() )
        {
            gs_critsectGui->Enter();

            gs_bGuiOwnedByMainThread = TRUE;
        }
        //else: already have it, nothing to do
    }
    else
    {
        // some threads are waiting, release the GUI lock if we have it
        if ( wxGuiOwnedByMainThread() )
        {
            wxMutexGuiLeave();
        }
        //else: some other worker thread is doing GUI
    }
}

bool WXDLLEXPORT wxGuiOwnedByMainThread()
{
    return gs_bGuiOwnedByMainThread;
}

// wake up the main thread if it's in ::GetMessage()
void WXDLLEXPORT wxWakeUpMainThread()
{
    // sending any message would do - hopefully WM_NULL is harmless enough
    if ( !::PostThreadMessage(gs_idMainThread, WM_NULL, 0, 0) )
    {
        // should never happen
        wxLogLastError("PostThreadMessage(WM_NULL)");
    }
}

bool WXDLLEXPORT wxIsWaitingForThread()
{
    return gs_waitingForThread;
}

#endif // wxUSE_THREADS
