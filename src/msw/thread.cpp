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

enum thread_state
{
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_CANCELED,
    STATE_EXITED
};

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------

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

class wxThreadInternal
{
public:
    static DWORD WinThreadStart(wxThread *thread);

    HANDLE       hThread;
    thread_state state;
    int          prio, defer;
    DWORD        tid;
};

DWORD wxThreadInternal::WinThreadStart(wxThread *thread)
{
    DWORD ret = (DWORD)thread->Entry();
    thread->p_internal->state = STATE_EXITED;
    thread->OnExit();

    return ret;
}

wxThreadError wxThread::Create()
{
    p_internal->hThread = ::CreateThread
                          (
                            NULL,               // default security
                            0,                  // default stack size
                            (LPTHREAD_START_ROUTINE)
                            wxThreadInternal::WinThreadStart, // entry point
                            (void *)this,       // parameter
                            CREATE_SUSPENDED,   // flags
                            &p_internal->tid    // [out] thread id
                          );

    if ( p_internal->hThread == NULL )
    {
        wxLogSysError(_("Can't create thread"));
        return wxTHREAD_NO_RESOURCE;
    }

    int win_prio, prio = p_internal->prio;
    if (prio <= 20)
        win_prio = THREAD_PRIORITY_LOWEST;
    else if (prio <= 40)
        win_prio = THREAD_PRIORITY_BELOW_NORMAL;
    else if (prio <= 60)
        win_prio = THREAD_PRIORITY_NORMAL;
    else if (prio <= 80)
        win_prio = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (prio <= 100)
        win_prio = THREAD_PRIORITY_HIGHEST;
    else
    {
        wxFAIL_MSG("invalid value of thread priority parameter");
        win_prio = THREAD_PRIORITY_NORMAL;
    }

    if ( SetThreadPriority(p_internal->hThread, win_prio) == 0 )
    {
        wxLogSysError(_("Can't set thread priority"));
    }

    return Resume();
}

wxThreadError wxThread::Destroy()
{
    if ( p_internal->state != STATE_RUNNING )
        return wxTHREAD_NOT_RUNNING;

    if ( p_internal->defer )
    {
        // soft termination: just set the flag and wait until the thread
        // auto terminates
        p_internal->state = STATE_CANCELED;
    }
    else
    {
        // kill the thread
        OnExit();
        if ( ::TerminateThread(p_internal->hThread, 0) == 0 )
        {
            wxLogLastError("TerminateThread");
        }
    }

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Pause()
{
    DWORD nSuspendCount = ::SuspendThread(p_internal->hThread);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not suspend thread %x"), p_internal->hThread);

        return wxTHREAD_MISC_ERROR;   // no idea what might provoke this error...
    }

    p_internal->state = STATE_PAUSED;

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    DWORD nSuspendCount = ::ResumeThread(p_internal->hThread);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not resume thread %x"), p_internal->hThread);

        return wxTHREAD_MISC_ERROR;   // no idea what might provoke this error...
    }

    p_internal->state = STATE_RUNNING;

    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(void *status)
{
    p_internal->state = STATE_EXITED;
    ExitThread((DWORD)status);
}

void wxThread::SetPriority(int prio)
{
    p_internal->prio = prio;
}

int wxThread::GetPriority() const
{
    return p_internal->prio;
}

void wxThread::DeferDestroy(bool on)
{
    p_internal->defer = on;
}

bool wxThread::TestDestroy()
{
    return p_internal->state == STATE_CANCELED;
}

void *wxThread::Join()
{
    DWORD exit_code;

    if (p_internal->state == STATE_IDLE)
        return NULL;

    // FIXME this dead locks... wxThread class design must be changed
#if 0
    WaitForSingleObject(p_internal->hThread, INFINITE);
#else
    ::TerminateThread(p_internal->hThread, 0);
#endif // 0

    GetExitCodeThread(p_internal->hThread, &exit_code);
    CloseHandle(p_internal->hThread);

    p_internal->state = STATE_IDLE;

    return (void *)exit_code;
}

unsigned long wxThread::GetID() const
{
    return (unsigned long)p_internal->tid;
}

bool wxThread::IsRunning() const
{
    return (p_internal->state == STATE_RUNNING);
}

bool wxThread::IsAlive() const
{
    return (p_internal->state == STATE_RUNNING);
}

bool wxThread::IsMain()
{
    return ::GetCurrentThreadId() == s_idMainThread;
}

wxThread::wxThread()
{
    p_internal = new wxThreadInternal();

    p_internal->defer = FALSE;
    p_internal->prio  = WXTHREAD_DEFAULT_PRIORITY;
    p_internal->state = STATE_IDLE;
}

wxThread::~wxThread()
{
    Destroy();
    Join();
    delete p_internal;
}

void wxThread::OnExit()
{
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
    s_critsectWaitingForGui = new wxCriticalSection();

    s_critsectGui = new wxCriticalSection();
    s_critsectGui->Enter();

    s_idMainThread = ::GetCurrentThreadId();

    return TRUE;
}

void wxThreadModule::OnExit()
{
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

#endif // wxUSE_THREADS
