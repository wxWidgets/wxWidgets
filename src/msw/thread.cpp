/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxThread Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by:
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998)
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
    STATE_CANCELED,
    STATE_EXITED
};

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
static HANDLE s_idMainThread;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *s_critsectGui;

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
    static DWORD WinThreadStart(LPVOID arg);

    HANDLE thread_id;
    int state;
    int prio, defer;
    DWORD tid;
};

DWORD wxThreadInternal::WinThreadStart(LPVOID arg)
{
    wxThread *ptr = (wxThread *)arg;
    DWORD ret;

    ret = (DWORD)ptr->Entry();
    ptr->p_internal->state = STATE_EXITED;

    return ret;
}

wxThreadError wxThread::Create()
{
    int prio = p_internal->prio;

    p_internal->thread_id = CreateThread(NULL, 0,
            (LPTHREAD_START_ROUTINE)wxThreadInternal::WinThreadStart,
            (void *)this, CREATE_SUSPENDED, &p_internal->tid);

    if ( p_internal->thread_id == NULL )
    {
        wxLogSysError(_("Can't create thread"));
        return wxTHREAD_NO_RESOURCE;
    }

    int win_prio;
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

    SetThreadPriority(p_internal->thread_id, win_prio);

    ResumeThread(p_internal->thread_id);
    p_internal->state = STATE_RUNNING;

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Destroy()
{
    if (p_internal->state != STATE_RUNNING)
        return wxTHREAD_NOT_RUNNING;

    if ( p_internal->defer )
        // soft termination: just set the flag and wait until the thread
        // auto terminates
        p_internal->state = STATE_CANCELED;
    else
        // kill the thread
        TerminateThread(p_internal->thread_id, 0);

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Pause()
{
    DWORD nSuspendCount = ::SuspendThread(p_internal->thread_id);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not suspend thread %x"), p_internal->thread_id);

        return wxTHREAD_MISC_ERROR;   // no idea what might provoke this error...
    }

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    DWORD nSuspendCount = ::ResumeThread(p_internal->thread_id);
    if ( nSuspendCount == (DWORD)-1 )
    {
        wxLogSysError(_("Can not resume thread %x"), p_internal->thread_id);

        return wxTHREAD_MISC_ERROR;   // no idea what might provoke this error...
    }

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

    if (wxThread::IsMain())
        s_critsectGui->Leave();
    WaitForSingleObject(p_internal->thread_id, INFINITE);
    if (wxThread::IsMain())
        s_critsectGui->Enter();

    GetExitCodeThread(p_internal->thread_id, &exit_code);
    CloseHandle(p_internal->thread_id);

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
    return (GetCurrentThread() == s_idMainThread);
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
    s_critsectGui = new wxCriticalSection();
    s_critsectGui->Enter();
    s_idMainThread = GetCurrentThread();

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
}

// under Windows, these functions are implemented usign a critical section and
// not a mutex, so the names are a bit confusing
void WXDLLEXPORT wxMutexGuiEnter()
{
    //s_critsectGui->Enter();
}

void WXDLLEXPORT wxMutexGuiLeave()
{
    //s_critsectGui->Leave();
}

#endif // wxUSE_THREADS
