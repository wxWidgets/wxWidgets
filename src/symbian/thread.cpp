/////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/thread.cpp
// Purpose:     wxThread Implementation
// Author:      Andrei Matuk, based on the wxMSW thread code
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
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

#if wxUSE_THREADS

#include "wx/thread.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/module.h"
#endif

#include "wx/apptrait.h"
#include "wx/scopeguard.h"

#include "wx/module.h"
#include "wx/thread.h"
#include <e32std.h>
#include <e32base.h>
#include "EIKDEF.H"
#include "wx/utils.h"

#include "wx/except.h"

typedef TInt THREAD_RETVAL;
static const THREAD_RETVAL THREAD_ERROR_EXIT = (THREAD_RETVAL)-1;

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

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
wxThreadIdType wxThread::ms_idMainThread = 0;
/*
// TLS index of the slot where we store the pointer to the current thread
static TUint gs_tlsThisThread = 0xFFFFFFFF;

// if it's false, some secondary thread is holding the GUI lock
static bool gs_bGuiOwnedByMainThread = true;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *gs_critsectGui = NULL;

// critical section which protects gs_nWaitingForGui variable
static wxCriticalSection *gs_critsectWaitingForGui = NULL;

// critical section which serializes WinThreadStart() and WaitForTerminate()
// (this is a potential bottleneck, we use a single crit sect for all threads
// in the system, but normally time spent inside it should be quite short)
static wxCriticalSection *gs_critsectThreadDelete = NULL;

// number of threads waiting for GUI in wxMutexGuiEnter()
static size_t gs_nWaitingForGui = 0;

// are we waiting for a thread termination?
static bool gs_waitingForThread = false;
*/
// ============================================================================
// Symbian implementation of thread and related classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxCriticalSection
// ----------------------------------------------------------------------------

wxCriticalSection::wxCriticalSection( wxCriticalSectionType WXUNUSED(critSecType) )
{
    // If you put m_critsec.CreateLocal() here then you will get an AV on app startup if you have any global variables
    m_created = false;
}

wxCriticalSection::~wxCriticalSection()
{
    if (m_created)
        m_critsect.Close();
}

void wxCriticalSection::Enter()
{
    // In Symbian SDK 1.X and 2.X these don't seem to be recursive. On 3.X they are.
    // So simulate recursive to make it all the same. Any performance hit should be minor (NOTE even some widgets code has recursive
    // wxCriticalSections so this has to be supported).
    RThread activeThread;

    if (!m_created)
        m_created = (m_critsect.CreateLocal() == KErrNone);

    if (m_critsect.IsBlocked() && m_lockthread == activeThread.Id())
        return;

    m_critsect.Wait();
    m_lockthread = activeThread.Id();
}

void wxCriticalSection::Leave()
{
    RThread activeThread;
    if (m_created && m_critsect.IsBlocked() && m_lockthread == activeThread.Id())
        m_critsect.Signal();
}

// ----------------------------------------------------------------------------
// wxMutex
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
    wxMutexInternal(wxMutexType mutexType);
    ~wxMutexInternal();

    bool IsOk() const { return m_ok; }
    bool IsLocked();

    wxMutexError Lock() { return LockTimeout(0); }
    wxMutexError Lock(unsigned long ms) { return LockTimeout(ms); }
    wxMutexError TryLock();
    wxMutexError Unlock();

private:
    wxMutexError LockTimeout(TUint milliseconds);

    RMutex m_mutex;
    bool   m_ok;
    bool   m_created;
    TThreadId m_lockthread;

    void CheckCreate() { if (!m_created)  m_created = (m_mutex.CreateLocal() == KErrNone); };

    wxDECLARE_NO_COPY_CLASS(wxMutexInternal);
};

// Mutexes on Symbian SDK 1.X and 2.X seem to be non-recursive, but those afterwards are recursive.
// Simulate recursive so they all behave the same.
wxMutexInternal::wxMutexInternal(wxMutexType WXUNUSED(mutexType))
{
    // If you put m_critsec.CreateLocal() here then you will get an AV on app startup if you have any global variables
    m_ok = true;
    m_created = false;
    m_lockthread = NULL;
}

wxMutexInternal::~wxMutexInternal()
{
    m_mutex.Close();
}

wxMutexError wxMutexInternal::TryLock()
{
    CheckCreate();
    if (!m_created)
        return wxMUTEX_INVALID;

    RThread activeThread;
    if (IsLocked())
    {
        if (m_lockthread == activeThread.Id())
            return wxMUTEX_NO_ERROR;
        else
            return wxMUTEX_BUSY;
    }
    else
    {
        m_mutex.Wait();
        m_lockthread = activeThread.Id();
        return wxMUTEX_NO_ERROR;
    }
}

bool wxMutexInternal::IsLocked()
{
#if defined(__SERIES60_1X__) || defined(__SERIES60_2X__)
    return (m_mutex.Count() <= 0);
#else
    return (m_mutex.IsHeld());
#endif
}

wxMutexError wxMutexInternal::LockTimeout(TUint milliseconds)
{
    CheckCreate();
    if (!m_created)
        return wxMUTEX_INVALID;

    RThread activeThread;
    if (IsLocked() && m_lockthread == activeThread.Id())
        return wxMUTEX_NO_ERROR;

    m_mutex.Wait();
    m_lockthread = activeThread.Id();
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::Unlock()
{
    RThread activeThread;
    if (m_created && IsLocked() && m_lockthread == activeThread.Id())
        m_mutex.Signal();

    return wxMUTEX_NO_ERROR;
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

// a trivial wrapper around Win32 semaphore
class wxSemaphoreInternal
{
public:
    wxSemaphoreInternal(int initialcount, int maxcount);
    ~wxSemaphoreInternal();

    bool IsOk() const { return m_ok; }

    wxSemaError Wait();
    wxSemaError TryWait();
    wxSemaError WaitTimeout(unsigned long milliseconds);
    wxSemaError Post();

private:
    RSemaphore m_semaphore;
    int           m_initialcount;
    bool       m_ok;
    bool       m_created;

    void CheckCreate();

    DECLARE_NO_COPY_CLASS(wxSemaphoreInternal)
};

wxSemaphoreInternal::wxSemaphoreInternal(int initialcount, int maxcount)
{
    // If you put m_semaphore.CreateLocal() here then you will get an AV on app startup if you have any global variables
    m_initialcount = initialcount;
    m_ok = true;
    m_created = false;
}

wxSemaphoreInternal::~wxSemaphoreInternal()
{
    if (m_created)
        m_semaphore.Close();
}

void wxSemaphoreInternal::CheckCreate()
{
    if (!m_created)
        m_created = (m_semaphore.CreateLocal(m_initialcount) == KErrNone);
}

wxSemaError wxSemaphoreInternal::Wait()
{
    CheckCreate();
    if (!m_created)
        return wxSEMA_INVALID;

    m_semaphore.Wait();
    return wxSEMA_NO_ERROR;
}

wxSemaError wxSemaphoreInternal::TryWait()
{
    CheckCreate();
    if (!m_created)
        return wxSEMA_INVALID;

#if defined(__SERIES60_1X__) || defined(__SERIES60_2X__)
    if (m_semaphore.Count() <= 0)
        return wxSEMA_BUSY;
    m_semaphore.Wait();
#else
    // Starting at SDK 3X, Count() no longer exists and a new Wait(timeout) is available
    if (m_semaphore.Wait(1) != KErrNone)
        return wxSEMA_BUSY;
#endif

    return wxSEMA_NO_ERROR;
}

wxSemaError wxSemaphoreInternal::WaitTimeout(unsigned long milliseconds)
{
    CheckCreate();
    if (!m_created)
        return wxSEMA_INVALID;

#if defined(__SERIES60_1X__) || defined(__SERIES60_2X__)
    wxASSERT(_T("Semaphore timeout not supported prior to Symbian SDK 3.0"));
    return wxSEMA_MISC_ERROR;
#else
    if (m_semaphore.Wait(milliseconds * 1000) != KErrNone)
        return wxSEMA_BUSY;
#endif
    return wxSEMA_NO_ERROR;
}

wxSemaError wxSemaphoreInternal::Post()
{
    CheckCreate();
    if (!m_created)
        return wxSEMA_INVALID;

    m_semaphore.Signal();
    return wxSEMA_NO_ERROR;
}

// ----------------------------------------------------------------------------
// wxThread implementation
// ----------------------------------------------------------------------------

// wxThreadInternal class
// ----------------------

class wxThreadInternal
{
public:
    wxThreadInternal(wxThread *thread)
    {
        m_thread = thread;
        m_hThread = 0;
        m_state = STATE_NEW;
        m_priority = WXTHREAD_DEFAULT_PRIORITY;
        m_nRef = 1;
    }

    ~wxThreadInternal()
    {
        Free();
    }

    void Free()
    {
        if ( m_hThread )
        {
            m_hThread->Close();
            delete m_hThread;
            m_hThread = 0;
        }
    }

    // create a new (suspended) thread (for the given thread object)
    bool Create(wxThread *thread, unsigned int stackSize);

    // wait for the thread to terminate, either by itself, or by asking it
    // (politely, this is not Kill()!) to do it
    wxThreadError WaitForTerminate(wxCriticalSection& cs,
                                   wxThread::ExitCode *pRc,
                                   wxThread *threadToDelete = NULL);

    // kill the thread unconditionally
    wxThreadError Kill();

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
    TInt GetHandle() const { return m_hThread->Handle(); }
    wxThreadIdType GetId() const { return m_hThread->Id(); }

    // thread function
    static THREAD_RETVAL SymbianThreadStart(void *thread);

    void KeepAlive()
    {
        if ( m_thread->IsDetached() )
        {
            wxCriticalSectionLocker lock(m_thread->m_critsect);
            m_nRef++;
        }
    }

    void LetDie()
    {
        if ( m_thread->IsDetached())
        {
            wxCriticalSectionLocker lock(m_thread->m_critsect);
            m_nRef--;
        }
        if (!m_nRef)
        {
            delete m_thread;
            m_thread =0;
        }
    }

private:
    // the thread we're associated with
    wxThread *m_thread;

    RThread      *m_hThread;    // handle of the thread
    wxThreadState m_state;      // state, see wxThreadState enum
    unsigned int  m_priority;   // thread priority in "wx" units

    // number of threads which need this thread to remain alive, when the count
    // reaches 0 we kill the owning wxThread -- and die ourselves with it
    TInt64 m_nRef;

    wxDECLARE_NO_COPY_CLASS(wxThreadInternal);
};

// small class which keeps a thread alive during its lifetime
class wxThreadKeepAlive
{
public:
    wxThreadKeepAlive(wxThreadInternal& thrImpl) : m_thrImpl(thrImpl)
        { m_thrImpl.KeepAlive(); }
    ~wxThreadKeepAlive()
        { m_thrImpl.LetDie(); }

private:
    wxThreadInternal& m_thrImpl;
};

THREAD_RETVAL wxThreadInternal::SymbianThreadStart(void *param)
{
    int rc;

    wxThread * const thread = (wxThread *)param;

    // first of all, check whether we hadn't been cancelled already and don't
    // start the user code at all then
    const bool hasExited = thread->m_internal->GetState() == STATE_EXITED;

    if ( hasExited )
    {
        rc = -1;
    }
    else // do run thread
    {
        // 1. Add cleanup stack support.
        CTrapCleanup* cleanupStack = CTrapCleanup::New();

        // 2. Get pointer to thread host
//        thread = (wxThread *)param;

/*        is this code necessery? CleanupStack error =(

        CActiveScheduler* activeScheduler;
        //3. Add support for active objects
        //TRAPD(err,
          activeScheduler = new (ELeave) CActiveScheduler;
//        CleanupStack::PushL(activeScheduler);
        CActiveScheduler::Install(activeScheduler);

        CActiveScheduler::Start();
*/

        TRAP(rc, thread->Entry());

//        delete activeScheduler;
        delete cleanupStack;
    }

    thread->OnExit();

    // save IsDetached because thread object can be deleted by joinable
    // threads after state is changed to STATE_EXITED.
    const bool isDetached = thread->IsDetached();
    if ( !hasExited )
    {
        // enter m_critsect before changing the thread state
        wxCriticalSectionLocker lock(thread->m_critsect);
        thread->m_internal->SetState(STATE_EXITED);
    }

    // the thread may delete itself now if it wants, we don't need it any more
    if ( isDetached )
        thread->m_internal->LetDie();

    return rc;
}

void wxThreadInternal::SetPriority(unsigned int priority)
{
    m_priority = priority;

    // translate wxWidgets priority to the Symbian one
    TThreadPriority symb_priority;
    if (m_priority <= 10)
        symb_priority = EPriorityAbsoluteVeryLow;
    else if (m_priority <= 20)
        symb_priority = EPriorityAbsoluteLow;
    else if (m_priority <= 40)
        symb_priority = EPriorityAbsoluteBackground;
    else if (m_priority <= 60)
        symb_priority = EPriorityAbsoluteForeground;
    else if (m_priority <= 80)
        symb_priority = EPriorityAbsoluteHigh;
    else if (m_priority <= 100)
        symb_priority = EPriorityRealTime;
    else
    {
        wxFAIL_MSG(wxT("invalid value of thread priority parameter"));
        symb_priority = EPriorityNormal;
    }

    m_hThread->SetPriority(EPriorityNormal);
}

bool wxThreadInternal::Create(wxThread *thread, unsigned int stackSize)
{
     if ( stackSize == 0 /*|| stackSize > 8092*/)
        stackSize = 8*KDefaultStackSize;

    m_hThread = new (ELeave) RThread();
    TBuf<255> threadname;
    if (m_hThread->Create(threadname, SymbianThreadStart, stackSize, &User::Heap(), thread) != KErrNone)
        return false;

    if ( m_priority != WXTHREAD_DEFAULT_PRIORITY )
    {
        SetPriority(m_priority);
    }

    return true;
}

wxThreadError wxThreadInternal::Kill()
{
    m_hThread->Kill(0);
    Free();

    return wxTHREAD_NO_ERROR;
}

wxThreadError
wxThreadInternal::WaitForTerminate(wxCriticalSection& cs,
                                   wxThread::ExitCode *pRc,
                                   wxThread *threadToDelete)
{
    // prevent the thread C++ object from disappearing as long as we are using
    // it here
    wxThreadKeepAlive keepAlive(*this);


    // we may either wait passively for the thread to terminate (when called
    // from Wait()) or ask it to terminate (when called from Delete())
    bool shouldDelete = threadToDelete != NULL;

    wxThread::ExitCode rc = 0;

    // we might need to resume the thread if it's currently stopped
    bool shouldResume = false;

    // as Delete() (which calls us) is always safe to call we need to consider
    // all possible states
    {
        wxCriticalSectionLocker lock(cs);

        if ( m_state == STATE_NEW )
        {
            if ( shouldDelete )
            {
                // SymbianThreadStart() will see it and terminate immediately, no
                // need to cancel the thread -- but we still need to resume it
                // to let it run
                m_state = STATE_EXITED;

                // we must call Resume() as the thread hasn't been initially
                // resumed yet (and as Resume() it knows about STATE_EXITED
                // special case, it won't touch it and WinThreadStart() will
                // just exit immediately)
                shouldResume = true;
                shouldDelete = false;
            }
            //else: shouldResume is correctly set to false here, wait until
            //      someone else runs the thread and it finishes
        }
        else // running, paused, cancelled or even exited
        {
            shouldResume = m_state == STATE_PAUSED;
        }
    }

    // resume the thread if it is paused
    if ( shouldResume )
        Resume();

    // ask the thread to terminate
    if ( shouldDelete )
    {
        wxCriticalSectionLocker lock(cs);

        Cancel();
    }

    TRequestStatus terminated;
    m_hThread->Logon(terminated);        // Blocks until thread exits
    User::WaitForRequest(terminated);

    if ( pRc )
        *pRc = rc;

    // we don't need the thread handle any more in any case
    Free();


    return rc == (wxThread::ExitCode)-1 ? wxTHREAD_MISC_ERROR
                                        : wxTHREAD_NO_ERROR;
}

bool wxThreadInternal::Suspend()
{
    m_hThread->Suspend();
    m_state = STATE_PAUSED;
    return true;
}

bool wxThreadInternal::Resume()
{
    m_hThread->Resume();


    // don't change the state from STATE_EXITED because it's special and means
    // we are going to terminate without running any user code - if we did it,
    // the code in WaitForTerminate() wouldn't work
    if ( m_state != STATE_EXITED )
    {
        m_state = STATE_RUNNING;
    }

    return true;
}

// static functions
// ----------------

wxThread *wxThread::This()
{
//TODO: Didn't get around to storing a list of wxThread.
/*
    RThread activeThread;
    TThreadId = activeThread.Id();
    // Find that id in whatever list and return
*/
    return NULL;
}

void wxThread::Yield()
{
    wxMicroSleep(1);
}

int wxThread::GetCPUCount()
{
    return 1;
}

unsigned long wxThread::GetCurrentId()
{
    RThread activeThread;
    return (unsigned long)activeThread.Id();
}

bool wxThread::SetConcurrency(size_t level)
{
    return true;    // Not supported in Symbian
}

// ctor and dtor
// -------------

wxThread::wxThread(wxThreadKind kind)
{
    m_internal = new wxThreadInternal(this);

    m_isDetached = kind == wxTHREAD_DETACHED;
}

wxThread::~wxThread()
{
    delete m_internal;
}

// create/start thread
// -------------------

wxThreadError wxThread::Create(unsigned int stackSize)
{
    wxCriticalSectionLocker lock(m_critsect);

    TRAPD(ret,m_internal->Create(this, stackSize) );
    if (ret != KErrNone)
    {
        return wxTHREAD_NO_RESOURCE;
    } else
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
    ExitCode rc = wxUIntToPtr(THREAD_ERROR_EXIT);

    // although we can wait for any thread, it's an error to
    // wait for a detached one in wxWin API
    wxCHECK_MSG( !IsDetached(), rc,
                 wxT("wxThread::Wait(): can't wait for detached thread") );

    (void)m_internal->WaitForTerminate(m_critsect, &rc);

    return rc;
}

wxThreadError wxThread::Delete(ExitCode *pRc)
{
    return m_internal->WaitForTerminate(m_critsect, pRc, this);
}

wxThreadError wxThread::Kill()
{
    if ( !IsRunning() )
        return wxTHREAD_NOT_RUNNING;

    wxThreadError rc = m_internal->Kill();

    if ( IsDetached() )
    {
        delete this;
    }
    else // joinable
    {
        // update the status of the joinable thread
        wxCriticalSectionLocker lock(m_critsect);
        m_internal->SetState(STATE_EXITED);
    }

    return rc;
}

void wxThread::Exit(ExitCode status)
{
    m_internal->Free();

    if ( IsDetached() )
    {
        delete this;
    }
    else // joinable
    {
        // update the status of the joinable thread
        wxCriticalSectionLocker lock(m_critsect);
        m_internal->SetState(STATE_EXITED);
    }
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

void wxMutexGuiEnterImpl()
{
}

void wxMutexGuiLeaveImpl()
{
}

void WXDLLIMPEXP_BASE wxMutexGuiLeaveOrEnter()
{
}

bool WXDLLIMPEXP_BASE wxGuiOwnedByMainThread()
{
    return true;
}

// wake up the main thread if it's in ::GetMessage()
void WXDLLIMPEXP_BASE wxWakeUpMainThread()
{
}

bool WXDLLIMPEXP_BASE wxIsWaitingForThread()
{
    return false;
}

// ----------------------------------------------------------------------------
// include common implementation code
// ----------------------------------------------------------------------------

#include "wx/thrimpl.cpp"

#endif // wxUSE_THREADS

