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

#include <stdio.h>

#include <windows.h>

#include "wx/module.h"
#include "wx/thread.h"

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
// static variables
// ----------------------------------------------------------------------------

// TLS index of the slot where we store the pointer to the current thread
static DWORD s_tlsThisThread = 0xFFFFFFFF;

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
static DWORD s_idMainThread = 0;

// if it's FALSE, some secondary thread is holding the GUI lock
static bool s_bGuiOwnedByMainThread = TRUE;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *s_critsectGui = NULL;

// critical section which protects s_nWaitingForGui variable
static wxCriticalSection *s_critsectWaitingForGui = NULL;

// number of threads waiting for GUI in wxMutexGuiEnter()
static size_t s_nWaitingForGui = 0;

// are we waiting for a thread termination?
static bool s_waitingForThread = FALSE;

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
    p_internal = new wxMutexInternal;
    p_internal->p_mutex = CreateMutex(NULL, FALSE, NULL);
    if ( !p_internal->p_mutex )
    {
        wxLogSysError(_("Can not create mutex."));
    }

    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxLogDebug("Warning: freeing a locked mutex (%d locks).", m_locked);
    CloseHandle(p_internal->p_mutex);
}

wxMutexError wxMutex::Lock()
{
    DWORD ret;

    ret = WaitForSingleObject(p_internal->p_mutex, INFINITE);
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
            wxFAIL_MSG("impossible return value in wxMutex::Lock");
    }

    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
    DWORD ret;

    ret = WaitForSingleObject(p_internal->p_mutex, 0);
    if (ret == WAIT_TIMEOUT || ret == WAIT_ABANDONED)
        return wxMUTEX_BUSY;

    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
    if (m_locked > 0)
        m_locked--;

    BOOL ret = ReleaseMutex(p_internal->p_mutex);
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
    HANDLE event;
    int waiters;
};

wxCondition::wxCondition()
{
    p_internal = new wxConditionInternal;
    p_internal->event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if ( !p_internal->event )
    {
        wxLogSysError(_("Can not create event object."));
    }

    p_internal->waiters = 0;
}

wxCondition::~wxCondition()
{
    CloseHandle(p_internal->event);
}

void wxCondition::Wait(wxMutex& mutex)
{
    mutex.Unlock();
    p_internal->waiters++;
    WaitForSingleObject(p_internal->event, INFINITE);
    p_internal->waiters--;
    mutex.Lock();
}

bool wxCondition::Wait(wxMutex& mutex,
                       unsigned long sec,
                       unsigned long nsec)
{
    DWORD ret;

    mutex.Unlock();
    p_internal->waiters++;
    ret = WaitForSingleObject(p_internal->event, (sec*1000)+(nsec/1000000));
    p_internal->waiters--;
    mutex.Lock();

    return (ret != WAIT_TIMEOUT);
}

void wxCondition::Signal()
{
    SetEvent(p_internal->event);
}

void wxCondition::Broadcast()
{
    int i;

    for (i=0;i<p_internal->waiters;i++)
    {
        if ( SetEvent(p_internal->event) == 0 )
        {
            wxLogSysError(_("Couldn't change the state of event object."));
        }
    }
}

// ----------------------------------------------------------------------------
// wxCriticalSection implementation
// ----------------------------------------------------------------------------

class wxCriticalSectionInternal
{
public:
    // init the critical section object
    wxCriticalSectionInternal()
        { ::InitializeCriticalSection(&m_data); }

    // implicit cast to the associated data
    operator CRITICAL_SECTION *() { return &m_data; }

    // free the associated ressources
    ~wxCriticalSectionInternal()
        { ::DeleteCriticalSection(&m_data); }

private:
    CRITICAL_SECTION m_data;
};

wxCriticalSection::wxCriticalSection()
{
    m_critsect = new wxCriticalSectionInternal;
}

wxCriticalSection::~wxCriticalSection()
{
    delete m_critsect;
}

void wxCriticalSection::Enter()
{
    ::EnterCriticalSection(*m_critsect);
}

void wxCriticalSection::Leave()
{
    ::LeaveCriticalSection(*m_critsect);
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
    void SetPriority(unsigned int priority) { m_priority = priority; }
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
    if ( !::TlsSetValue(s_tlsThisThread, thread) )
    {
        wxLogSysError(_("Can not start thread: error writing TLS."));

        return (DWORD)-1;
    }

    DWORD ret = (DWORD)thread->Entry();
    thread->p_internal->SetState(STATE_EXITED);
    thread->OnExit();

    delete thread;

    return ret;
}

bool wxThreadInternal::Create(wxThread *thread)
{
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

    if ( m_hThread == NULL )
    {
        wxLogSysError(_("Can't create thread"));

        return FALSE;
    }

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
        wxFAIL_MSG("invalid value of thread priority parameter");
        win_priority = THREAD_PRIORITY_NORMAL;
    }

    if ( ::SetThreadPriority(m_hThread, win_priority) == 0 )
    {
        wxLogSysError(_("Can't set thread priority"));
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
    wxThread *thread = (wxThread *)::TlsGetValue(s_tlsThisThread);

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
    return ::GetCurrentThreadId() == s_idMainThread;
}

#ifdef Yield
    #undef Yield
#endif

void wxThread::Yield()
{
    // 0 argument to Sleep() is special
    ::Sleep(0);
}

void wxThread::Sleep(unsigned long milliseconds)
{
    ::Sleep(milliseconds);
}

// create/start thread
// -------------------

wxThreadError wxThread::Create()
{
    if ( !p_internal->Create(this) )
        return wxTHREAD_NO_RESOURCE;

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
    wxCriticalSectionLocker lock(m_critsect);

    if ( p_internal->GetState() != STATE_NEW )
    {
        // actually, it may be almost any state at all, not only STATE_RUNNING
        return wxTHREAD_RUNNING;
    }

    return Resume();
}

// suspend/resume thread
// ---------------------

wxThreadError wxThread::Pause()
{
    wxCriticalSectionLocker lock(m_critsect);

    return p_internal->Suspend() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

wxThreadError wxThread::Resume()
{
    wxCriticalSectionLocker lock(m_critsect);

    return p_internal->Resume() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

// stopping thread
// ---------------

wxThread::ExitCode wxThread::Delete()
{
    ExitCode rc = 0;

    // Delete() is always safe to call, so consider all possible states
    if ( IsPaused() )
        Resume();

    if ( IsRunning() )
    {
        if ( IsMain() )
        {
            // set flag for wxIsWaitingForThread()
            s_waitingForThread = TRUE;

            wxBeginBusyCursor();
        }

        HANDLE hThread;
        {
            wxCriticalSectionLocker lock(m_critsect);

            p_internal->Cancel();
            hThread = p_internal->GetHandle();
        }

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
                    return (ExitCode)-1;

                case WAIT_OBJECT_0:
                    // thread we're waiting for terminated
                    break;

                case WAIT_OBJECT_0 + 1:
                    // new message arrived, process it
                    if ( !wxTheApp->DoMessage() )
                    {
                        // WM_QUIT received: kill the thread
                        Kill();

                        return (ExitCode)-1;
                    }

                    if ( IsMain() )
                    {
                        // give the thread we're waiting for chance to exit
                        // from the GUI call it might have been in
                        if ( (s_nWaitingForGui > 0) && wxGuiOwnedByMainThread() )
                        {
                            wxMutexGuiLeave();
                        }
                    }

                    break;

                default:
                    wxFAIL_MSG("unexpected result of MsgWaitForMultipleObject");
            }
        } while ( result != WAIT_OBJECT_0 );

        if ( IsMain() )
        {
            s_waitingForThread = FALSE;

            wxEndBusyCursor();
        }

        if ( !::GetExitCodeThread(hThread, (LPDWORD)&rc) )
        {
            wxLogLastError("GetExitCodeThread");

            rc = (ExitCode)-1;
        }

        wxASSERT_MSG( (LPVOID)rc != (LPVOID)STILL_ACTIVE,
                      "thread must be already terminated." );

        ::CloseHandle(hThread);
    }

    return rc;
}

wxThreadError wxThread::Kill()
{
    if ( !IsRunning() )
        return wxTHREAD_NOT_RUNNING;

    if ( !::TerminateThread(p_internal->GetHandle(), (DWORD)-1) )
    {
        wxLogSysError(_("Couldn't terminate thread"));

        return wxTHREAD_MISC_ERROR;
    }

    delete this;

    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(void *status)
{
    delete this;

    ::ExitThread((DWORD)status);

    wxFAIL_MSG("Couldn't return from ExitThread()!");
}

void wxThread::SetPriority(unsigned int prio)
{
    wxCriticalSectionLocker lock(m_critsect);

    p_internal->SetPriority(prio);
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return p_internal->GetPriority();
}

unsigned long wxThread::GetID() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return (unsigned long)p_internal->GetId();
}

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return p_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return (p_internal->GetState() == STATE_RUNNING) ||
           (p_internal->GetState() == STATE_PAUSED);
}

bool wxThread::IsPaused() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return (p_internal->GetState() == STATE_PAUSED);
}

bool wxThread::TestDestroy()
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    return p_internal->GetState() == STATE_CANCELED;
}

wxThread::wxThread()
{
    p_internal = new wxThreadInternal();
}

wxThread::~wxThread()
{
    delete p_internal;
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
    s_tlsThisThread = ::TlsAlloc();
    if ( s_tlsThisThread == 0xFFFFFFFF )
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
    if ( !::TlsSetValue(s_tlsThisThread, (LPVOID)0) )
    {
        ::TlsFree(s_tlsThisThread);
        s_tlsThisThread = 0xFFFFFFFF;

        wxLogSysError(_("Thread module initialization failed: "
                        "can not store value in thread local storage"));

        return FALSE;
    }

    s_critsectWaitingForGui = new wxCriticalSection();

    s_critsectGui = new wxCriticalSection();
    s_critsectGui->Enter();

    // no error return for GetCurrentThreadId()
    s_idMainThread = ::GetCurrentThreadId();

    return TRUE;
}

void wxThreadModule::OnExit()
{
    if ( !::TlsFree(s_tlsThisThread) )
    {
        wxLogLastError("TlsFree failed.");
    }

    if ( s_critsectGui )
    {
        s_critsectGui->Leave();
        delete s_critsectGui;
        s_critsectGui = NULL;
    }

    wxDELETE(s_critsectWaitingForGui);
}

// ----------------------------------------------------------------------------
// under Windows, these functions are implemented usign a critical section and
// not a mutex, so the names are a bit confusing
// ----------------------------------------------------------------------------

void WXDLLEXPORT wxMutexGuiEnter()
{
    // this would dead lock everything...
    wxASSERT_MSG( !wxThread::IsMain(),
                  "main thread doesn't want to block in wxMutexGuiEnter()!" );

    // the order in which we enter the critical sections here is crucial!!

    // set the flag telling to the main thread that we want to do some GUI
    {
        wxCriticalSectionLocker enter(*s_critsectWaitingForGui);

        s_nWaitingForGui++;
    }

    wxWakeUpMainThread();

    // now we may block here because the main thread will soon let us in
    // (during the next iteration of OnIdle())
    s_critsectGui->Enter();
}

void WXDLLEXPORT wxMutexGuiLeave()
{
    wxCriticalSectionLocker enter(*s_critsectWaitingForGui);

    if ( wxThread::IsMain() )
    {
        s_bGuiOwnedByMainThread = FALSE;
    }
    else
    {
        // decrement the number of waiters now
        wxASSERT_MSG( s_nWaitingForGui > 0,
                      "calling wxMutexGuiLeave() without entering it first?" );

        s_nWaitingForGui--;

        wxWakeUpMainThread();
    }

    s_critsectGui->Leave();
}

void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
    wxASSERT_MSG( wxThread::IsMain(),
                  "only main thread may call wxMutexGuiLeaveOrEnter()!" );

    wxCriticalSectionLocker enter(*s_critsectWaitingForGui);

    if ( s_nWaitingForGui == 0 )
    {
        // no threads are waiting for GUI - so we may acquire the lock without
        // any danger (but only if we don't already have it)
        if ( !wxGuiOwnedByMainThread() )
        {
            s_critsectGui->Enter();

            s_bGuiOwnedByMainThread = TRUE;
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
    return s_bGuiOwnedByMainThread;
}

// wake up the main thread if it's in ::GetMessage()
void WXDLLEXPORT wxWakeUpMainThread()
{
    // sending any message would do - hopefully WM_NULL is harmless enough
    if ( !::PostThreadMessage(s_idMainThread, WM_NULL, 0, 0) )
    {
        // should never happen
        wxLogLastError("PostThreadMessage(WM_NULL)");
    }
}

bool WXDLLEXPORT wxIsWaitingForThread()
{
    return s_waitingForThread;
}

#endif // wxUSE_THREADS
