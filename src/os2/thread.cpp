/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxThread Implementation. For Unix ports, see e.g. src/gtk
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by: David Webster
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_THREADS

#include <stdio.h>

#include "wx/module.h"
#include "wx/thread.h"

#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_ERRORS
#include <os2.h>
#include <bseerr.h>

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

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
static ULONG                        s_ulIdMainThread = 0;
wxMutex*                            p_wxMainMutex;

// OS2 substitute for Tls pointer the current parent thread object
wxThread*                           m_pThread;    // pointer to the wxWindows thread object

// if it's FALSE, some secondary thread is holding the GUI lock
static bool s_bGuiOwnedByMainThread = TRUE;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *s_pCritsectGui = NULL;

// critical section which protects s_nWaitingForGui variable
static wxCriticalSection *s_pCritsectWaitingForGui = NULL;

// number of threads waiting for GUI in wxMutexGuiEnter()
static size_t s_nWaitingForGui = 0;

// are we waiting for a thread termination?
static bool s_bWaitingForThread = FALSE;

// ============================================================================
// OS/2 implementation of thread classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------
class wxMutexInternal
{
public:
    HMTX                            m_vMutex;
};

wxMutex::wxMutex()
{
    APIRET                          ulrc;

    p_internal = new wxMutexInternal;
    ulrc = ::DosCreateMutexSem(NULL, &p_internal->m_vMutex, 0L, FALSE);
    if (ulrc != 0)
    {
        wxLogSysError(_("Can not create mutex."));
    }
    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxLogDebug(wxT("Warning: freeing a locked mutex (%d locks)."), m_locked);
    ::DosCloseMutexSem(p_internal->m_vMutex);
    p_internal->m_vMutex = NULL;
}

wxMutexError wxMutex::Lock()
{
    APIRET                          ulrc;

    ulrc = ::DosRequestMutexSem(p_internal->m_vMutex, SEM_INDEFINITE_WAIT);

    switch (ulrc)
    {
        case ERROR_TOO_MANY_SEM_REQUESTS:
            return wxMUTEX_BUSY;

        case NO_ERROR:
            // ok
            break;

        case ERROR_INVALID_HANDLE:
        case ERROR_INTERRUPT:
        case ERROR_SEM_OWNER_DIED:
            wxLogSysError(_("Couldn't acquire a mutex lock"));
            return wxMUTEX_MISC_ERROR;

        case ERROR_TIMEOUT:
        default:
            wxFAIL_MSG(wxT("impossible return value in wxMutex::Lock"));
    }
    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
    ULONG                           ulrc;

    ulrc = ::DosRequestMutexSem(p_internal->m_vMutex, SEM_IMMEDIATE_RETURN /*0L*/);
    if (ulrc == ERROR_TIMEOUT || ulrc == ERROR_TOO_MANY_SEM_REQUESTS)
        return wxMUTEX_BUSY;

    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
    APIRET                          ulrc;

    if (m_locked > 0)
        m_locked--;

    ulrc = ::DosReleaseMutexSem(p_internal->m_vMutex);
    if (ulrc != 0)
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
    HEV                             m_vEvent;
    int                             m_nWaiters;
};

wxCondition::wxCondition()
{
    APIRET                          ulrc;
    ULONG                           ulCount;

    p_internal = new wxConditionInternal;
    ulrc = ::DosCreateEventSem(NULL, &p_internal->m_vEvent, 0L, FALSE);
    if (ulrc != 0)
    {
        wxLogSysError(_("Can not create event object."));
    }
    p_internal->m_nWaiters = 0;
    // ?? just for good measure?
    ::DosResetEventSem(p_internal->m_vEvent, &ulCount);
}

wxCondition::~wxCondition()
{
    ::DosCloseEventSem(p_internal->m_vEvent);
    delete p_internal;
    p_internal = NULL;
}

void wxCondition::Wait(
  wxMutex&                          rMutex
)
{
    rMutex.Unlock();
    p_internal->m_nWaiters++;
    ::DosWaitEventSem(p_internal->m_vEvent, SEM_INDEFINITE_WAIT);
    p_internal->m_nWaiters--;
    rMutex.Lock();
}

bool wxCondition::Wait(
  wxMutex&                          rMutex
, unsigned long                     ulSec
, unsigned long                     ulMillisec)
{
    APIRET                          ulrc;

    rMutex.Unlock();
    p_internal->m_nWaiters++;
    ulrc = ::DosWaitEventSem(p_internal->m_vEvent, ULONG((ulSec * 1000L) + ulMillisec));
    p_internal->m_nWaiters--;
    rMutex.Lock();

    return (ulrc != ERROR_TIMEOUT);
}

void wxCondition::Signal()
{
    ::DosPostEventSem(p_internal->m_vEvent);
}

void wxCondition::Broadcast()
{
    int                             i;

    for (i = 0; i < p_internal->m_nWaiters; i++)
    {
        if (::DosPostEventSem(p_internal->m_vEvent) != 0)
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
        { }

    // free the associated ressources
    ~wxCriticalSectionInternal()
        { }

private:
};

// ----------------------------------------------------------------------------
// wxCriticalSection implementation
// ----------------------------------------------------------------------------

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
    ::DosEnterCritSec();
}

void wxCriticalSection::Leave()
{
    ::DosExitCritSec();
}

// ----------------------------------------------------------------------------
// wxThread implementation
// ----------------------------------------------------------------------------

// wxThreadInternal class
// ----------------------

class wxThreadInternal
{
public:
    inline wxThreadInternal()
    {
        m_hThread = 0;
        m_eState = STATE_NEW;
        m_nPriority = 0;
    }

    // create a new (suspended) thread (for the given thread object)
    bool Create(wxThread* pThread);

    // suspend/resume/terminate
    bool Suspend();
    bool Resume();
    inline void Cancel() { m_eState = STATE_CANCELED; }

    // thread state
    inline void SetState(wxThreadState eState) { m_eState = eState; }
    inline wxThreadState GetState() const { return m_eState; }

    // thread priority
    inline void SetPriority(unsigned int nPriority) { m_nPriority = nPriority; }
    inline unsigned int GetPriority() const { return m_nPriority; }

    // thread handle and id
    inline TID GetHandle() const { return m_hThread; }
    TID  GetId() const { return m_hThread; }

    // thread function
    static DWORD OS2ThreadStart(wxThread *thread);

private:
    // Threads in OS/2 have only an ID, so m_hThread is both it's handle and ID
    // PM also has no real Tls mechanism to index pointers by so we'll just
    // keep track of the wxWindows parent object here.
    TID                             m_hThread;    // handle and ID of the thread
    wxThreadState                   m_eState;     // state, see wxThreadState enum
    unsigned int                    m_nPriority;  // thread priority in "wx" units
};

ULONG wxThreadInternal::OS2ThreadStart(
  wxThread*                         pThread
)
{
    m_pThread = pThread;

    DWORD                           dwRet = (DWORD)pThread->Entry();

    pThread->p_internal->SetState(STATE_EXITED);
    pThread->OnExit();

    delete pThread;
    m_pThread = NULL;
    return dwRet;
}

bool wxThreadInternal::Create(
  wxThread*                         pThread
)
{
    APIRET                          ulrc;

    ulrc = ::DosCreateThread( &m_hThread
                             ,(PFNTHREAD)wxThreadInternal::OS2ThreadStart
                             ,(ULONG)pThread
                             ,CREATE_SUSPENDED | STACK_SPARSE
                             ,8192L
                            );
    if(ulrc != 0)
    {
        wxLogSysError(_("Can't create thread"));

        return FALSE;
    }

    // translate wxWindows priority to the PM one
    ULONG                           ulOS2_Priority;

    if (m_nPriority <= 20)
        ulOS2_Priority = PRTYC_NOCHANGE;
    else if (m_nPriority <= 40)
        ulOS2_Priority = PRTYC_IDLETIME;
    else if (m_nPriority <= 60)
        ulOS2_Priority = PRTYC_REGULAR;
    else if (m_nPriority <= 80)
        ulOS2_Priority = PRTYC_TIMECRITICAL;
    else if (m_nPriority <= 100)
        ulOS2_Priority = PRTYC_FOREGROUNDSERVER;
    else
    {
        wxFAIL_MSG(wxT("invalid value of thread priority parameter"));
        ulOS2_Priority = PRTYC_REGULAR;
    }
    ulrc = ::DosSetPriority( PRTYS_THREAD
                            ,ulOS2_Priority
                            ,0
                            ,(ULONG)m_hThread
                           );
    if (ulrc != 0)
    {
        wxLogSysError(_("Can't set thread priority"));
    }
    return TRUE;
}

bool wxThreadInternal::Suspend()
{
    ULONG                           ulrc = ::DosSuspendThread(m_hThread);

    if (ulrc != 0)
    {
        wxLogSysError(_("Can not suspend thread %lu"), m_hThread);
        return FALSE;
    }
    m_eState = STATE_PAUSED;
    return TRUE;
}

bool wxThreadInternal::Resume()
{
    ULONG                           ulrc = ::DosResumeThread(m_hThread);

    if (ulrc != 0)
    {
        wxLogSysError(_("Can not suspend thread %lu"), m_hThread);
        return FALSE;
    }
    m_eState = STATE_PAUSED;
    return TRUE;
}

// static functions
// ----------------

wxThread *wxThread::This()
{
    wxThread*                       pThread = m_pThread;
    return pThread;
}

bool wxThread::IsMain()
{
    PTIB                            ptib;
    PPIB                            ppib;

    ::DosGetInfoBlocks(&ptib, &ppib);

    if (ptib->tib_ptib2->tib2_ultid == s_ulIdMainThread)
        return TRUE;
    return FALSE;
}

#ifdef Yield
    #undef Yield
#endif

void wxThread::Yield()
{
    ::DosSleep(0);
}

void wxThread::Sleep(
  unsigned long                     ulMilliseconds
)
{
    ::DosSleep(ulMilliseconds);
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
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

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
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return p_internal->Suspend() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

wxThreadError wxThread::Resume()
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return p_internal->Resume() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

// stopping thread
// ---------------

wxThread::ExitCode wxThread::Delete()
{
    ExitCode                        rc = 0;
    ULONG                           ulrc;

    // Delete() is always safe to call, so consider all possible states
    if (IsPaused())
        Resume();

    if (IsRunning())
    {
        if (IsMain())
        {
            // set flag for wxIsWaitingForThread()
            s_bWaitingForThread = TRUE;
            wxBeginBusyCursor();
        }

        TID                         hThread;

        {
            wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

            p_internal->Cancel();
            hThread = p_internal->GetHandle();
        }

        // we can't just wait for the thread to terminate because it might be
        // calling some GUI functions and so it will never terminate before we
        // process the Windows messages that result from these functions

        do
        {
            ulrc = ::DosWaitThread( &hThread
                                   ,DCWW_NOWAIT
                                  );
            switch (ulrc)
            {
                case ERROR_INTERRUPT:
                case ERROR_INVALID_THREADID:
                    // error
                    wxLogSysError(_("Can not wait for thread termination"));
                    Kill();
                    return (ExitCode)-1;

                case 0:
                    // thread we're waiting for terminated
                    break;

                case ERROR_THREAD_NOT_TERMINATED:
                    // new message arrived, process it
                    if (!wxTheApp->DoMessage())
                    {
                        // WM_QUIT received: kill the thread
                        Kill();
                        return (ExitCode)-1;
                    }
                    if (IsMain())
                    {
                        // give the thread we're waiting for chance to exit
                        // from the GUI call it might have been in
                        if ((s_nWaitingForGui > 0) && wxGuiOwnedByMainThread())
                        {
                            wxMutexGuiLeave();
                        }
                    }
                    break;

                default:
                    wxFAIL_MSG(wxT("unexpected result of DosWatiThread"));
            }
        } while (ulrc != 0);

        if (IsMain())
        {
            s_bWaitingForThread = FALSE;
            wxEndBusyCursor();
        }

        ::DosExit(EXIT_THREAD, ulrc);
    }
    rc = (ExitCode)ulrc;
    return rc;
}

wxThreadError wxThread::Kill()
{
    if (!IsRunning())
        return wxTHREAD_NOT_RUNNING;

    ::DosKillThread(p_internal->GetHandle());
    delete this;
    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(
  void*                             pStatus
)
{
    delete this;
    ::DosExit(EXIT_THREAD, ULONG(pStatus));
    wxFAIL_MSG(wxT("Couldn't return from DosExit()!"));
}

void wxThread::SetPriority(
  unsigned int                      nPrio
)
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    p_internal->SetPriority(nPrio);
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return p_internal->GetPriority();
}

unsigned long wxThread::GetID() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return (unsigned long)p_internal->GetId();
}

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return p_internal->GetState() == STATE_RUNNING;
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return (p_internal->GetState() == STATE_RUNNING) ||
           (p_internal->GetState() == STATE_PAUSED);
}

bool wxThread::IsPaused() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return (p_internal->GetState() == STATE_PAUSED);
}

bool wxThread::TestDestroy()
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

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
    s_pCritsectWaitingForGui = new wxCriticalSection();

    s_pCritsectGui = new wxCriticalSection();
    s_pCritsectGui->Enter();

    PTIB                            ptib;
    PPIB                            ppib;

    ::DosGetInfoBlocks(&ptib, &ppib);

    s_ulIdMainThread = ptib->tib_ptib2->tib2_ultid;
    return TRUE;
}

void wxThreadModule::OnExit()
{
    if (s_pCritsectGui)
    {
        s_pCritsectGui->Leave();
        delete s_pCritsectGui;
        s_pCritsectGui = NULL;
    }

    wxDELETE(s_pCritsectWaitingForGui);
}

// ----------------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------------

// Does nothing under OS/2 [for now]
void WXDLLEXPORT wxWakeUpMainThread()
{
}

void WXDLLEXPORT wxMutexGuiLeave()
{
    wxCriticalSectionLocker enter(*s_pCritsectWaitingForGui);

    if ( wxThread::IsMain() )
    {
        s_bGuiOwnedByMainThread = FALSE;
    }
    else
    {
        // decrement the number of waiters now
        wxASSERT_MSG( s_nWaitingForGui > 0,
                      wxT("calling wxMutexGuiLeave() without entering it first?") );

        s_nWaitingForGui--;

        wxWakeUpMainThread();
    }

    s_pCritsectGui->Leave();
}

void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
    wxASSERT_MSG( wxThread::IsMain(),
                  wxT("only main thread may call wxMutexGuiLeaveOrEnter()!") );

    wxCriticalSectionLocker enter(*s_pCritsectWaitingForGui);

    if ( s_nWaitingForGui == 0 )
    {
        // no threads are waiting for GUI - so we may acquire the lock without
        // any danger (but only if we don't already have it)
        if (!wxGuiOwnedByMainThread())
        {
            s_pCritsectGui->Enter();

            s_bGuiOwnedByMainThread = TRUE;
        }
        //else: already have it, nothing to do
    }
    else
    {
        // some threads are waiting, release the GUI lock if we have it
        if (wxGuiOwnedByMainThread())
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

#endif
  // wxUSE_THREADS
