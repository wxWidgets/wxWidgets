/////////////////////////////////////////////////////////////////////////////
// Name:	thread.cpp
// Purpose:	wxThread Implementation
// Author:	Original from Wolfram Gloger/Guilhem Lavaux/Vadim Zeitlin
// Modified by: Aj Lavin, Stefan Csomor
// Created:	04/22/98
// RCS-ID:	$Id$
// Copyright:	(c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998),
//		    Vadim Zeitlin (1999) , Stefan Csomor (2000)
// Licence:	wxWindows licence
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

#ifdef __WXMAC__
#include <CoreServices/CoreServices.h>
#include "wx/mac/uma.h"
#endif


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the possible states of the thread ("=>" shows all possible transitions from
// this state)
enum wxThreadState
{
  STATE_NEW,		// didn't start execution yet (=> RUNNING)
  STATE_RUNNING,	// thread is running (=> PAUSED, CANCELED)
  STATE_PAUSED,	// thread is temporarily suspended (=> RUNNING)
  STATE_CANCELED,	// thread should terminate a.s.a.p. (=> EXITED)
  STATE_EXITED	// thread is terminating
};

// ----------------------------------------------------------------------------
// this module globals
// ----------------------------------------------------------------------------

static MPTaskID gs_idMainThread = 0;
static bool gs_waitingForThread = FALSE ;
size_t g_numberOfThreads = 0;

#if wxUSE_GUI

MPCriticalRegionID gs_guiCritical = 0;

#endif

// ============================================================================
// MacOS implementation of thread classes
// ============================================================================

class wxMacStCritical
{
public :
  wxMacStCritical()
  {
    if ( ! s_criticalId)
      makeCritical();
	    
    MPEnterCriticalRegion( s_criticalId, kDurationForever);
  }
  ~wxMacStCritical()
  {
    MPExitCriticalRegion( s_criticalId);
  }
    
private:

  void makeCritical()
  {
    OSStatus err = MPCreateCriticalRegion( & s_criticalId);
    if ( err)
      {
	wxLogSysError(_("Could not make the static mutex."));
      }
  }
    
  static MPCriticalRegionID s_criticalId;
};

MPCriticalRegionID wxMacStCritical::s_criticalId = 0;

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------

class wxMutexInternal
{
public:
  // ALL MUTEXES WILL BE NON-RECURSIVE.
  wxMutexInternal(wxMutexType WXUNUSED(mutexType))
  {
    OSStatus err = MPCreateBinarySemaphore( & m_semaphore);
    if ( err)
      wxLogSysError(_("Could not construct mutex."));
  }

  ~wxMutexInternal()
  {
    MPDeleteSemaphore( m_semaphore);
  }

  bool IsOk() const { return true; }

  wxMutexError Lock() ;
  wxMutexError TryLock() ;
  wxMutexError Unlock();
public:
    
  MPSemaphoreID m_semaphore;
};

wxMutexError wxMutexInternal::Lock()
{
  OSStatus err = MPWaitOnSemaphore( m_semaphore, kDurationForever);
  if ( err)
    {
      wxLogSysError(_("Could not lock mutex"));
      return wxMUTEX_MISC_ERROR;
    }
    
  return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::TryLock()
{
  OSStatus err = MPWaitOnSemaphore( m_semaphore, kDurationImmediate);
  if ( err)
    {
      if ( err == kMPTimeoutErr)
	{
	  return wxMUTEX_BUSY;
	}
      wxLogSysError(_("Could not try lock mutex"));
      return wxMUTEX_MISC_ERROR;    
    }
    
  return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::Unlock()
{
  OSStatus err = MPSignalSemaphore( m_semaphore);
  if ( err)
    {
      wxLogSysError(_("Could not unlock mutex"));
      return wxMUTEX_MISC_ERROR;	  
    }
    
  return wxMUTEX_NO_ERROR;
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

class wxSemaphoreInternal
{
public:
  wxSemaphoreInternal(int initialcount, int maxcount);
  ~wxSemaphoreInternal();

  bool IsOk() const { return true ; }

  wxSemaError WaitTimeout(unsigned long milliseconds);

  wxSemaError Wait() { return WaitTimeout( kDurationForever); }

  wxSemaError TryWait() { return WaitTimeout(0); }

  wxSemaError Post();

private:
  MPSemaphoreID m_semaphore;
};

wxSemaphoreInternal::wxSemaphoreInternal(int initialcount, int maxcount)
{
  if ( maxcount == 0 )
    {
      // make it practically infinite
      maxcount = INT_MAX;
    }
  MPCreateSemaphore( maxcount, initialcount, & m_semaphore);
}

wxSemaphoreInternal::~wxSemaphoreInternal()
{
  MPDeleteSemaphore( m_semaphore);
}

wxSemaError wxSemaphoreInternal::WaitTimeout(unsigned long milliseconds)
{
  OSStatus err = MPWaitOnSemaphore( m_semaphore, milliseconds);
  if ( err)
    {
      if ( err == kMPTimeoutErr)
	{
	  return wxSEMA_TIMEOUT;
	}
      return wxSEMA_MISC_ERROR;
    }
  return wxSEMA_NO_ERROR;
}

wxSemaError wxSemaphoreInternal::Post()
{
  OSStatus err = MPSignalSemaphore( m_semaphore);
  if ( err)
    {
      return wxSEMA_MISC_ERROR;
    }
  return wxSEMA_NO_ERROR;
}

// ----------------------------------------------------------------------------
// wxCondition implementation
// ----------------------------------------------------------------------------

class wxConditionInternal
{
public:

  wxConditionInternal(wxMutex& mutex) 
    : m_mutex( mutex),
      m_semaphore( 0, 1),
      m_gate( 1, 1)
  {
    m_waiters = 0;
    m_signals = 0;
    m_canceled = 0;
  }

  ~wxConditionInternal()
  {
  }

  bool IsOk() const { return m_mutex.IsOk() ; }
    
  wxCondError Wait()
  {
    return WaitTimeout( kDurationForever);
  }

  wxCondError WaitTimeout(unsigned long msectimeout);

  wxCondError Signal()
  {
    return DoSignal( false);
  }

  wxCondError Broadcast()
  {
    return DoSignal( true);
  }
    
private:

  wxCondError DoSignal( bool signalAll);

  wxMutex&	  m_mutex;
  wxSemaphoreInternal     m_semaphore;  // Signals the waiting threads.
  wxSemaphoreInternal	  m_gate;
  wxCriticalSection m_varSection;
  size_t	  m_waiters;	// Number of threads waiting for a signal.
  size_t          m_signals;	// Number of signals to send.
  size_t	  m_canceled;   // Number of canceled waiters in m_waiters.
};


wxCondError wxConditionInternal::WaitTimeout(unsigned long msectimeout)
{	
  m_gate.Wait();
  if ( ++ m_waiters == INT_MAX)
    {
      m_varSection.Enter();
      m_waiters -= m_canceled;
      m_signals -= m_canceled;
      m_canceled = 0;
      m_varSection.Leave();
    }
  m_gate.Post();

  m_mutex.Unlock();
			
  wxSemaError err = m_semaphore.WaitTimeout( msectimeout);
  wxASSERT( err == wxSEMA_NO_ERROR || err == wxSEMA_TIMEOUT);

  m_varSection.Enter();
  if ( err != wxSEMA_NO_ERROR)
    {
      if ( m_signals > m_canceled)
	{
	  // A signal is being sent after we timed out.

	  if ( m_waiters == m_signals)
	    {
	      // There are no excess waiters to catch the signal, so
	      // we must throw it away.

	      wxSemaError err2 = m_semaphore.Wait();
	      if ( err2 != wxSEMA_NO_ERROR)
		{
		  wxLogSysError(_("Error while waiting on semaphore"));
		}
	      wxASSERT( err2 == wxSEMA_NO_ERROR);
	      -- m_waiters;
	      if ( -- m_signals == m_canceled)
		{
		  // This was the last signal. open the gate.
		  wxASSERT( m_waiters == m_canceled);
		  m_gate.Post();
		}
	    }
	  else
	    {
	      // There are excess waiters to catch the signal, leave
	      // it be.
	      -- m_waiters;
	    }
	}
      else
	{
	  // No signals is being sent.
	  // The gate may be open or closed, so we can't touch m_waiters.
	  ++ m_canceled;
	  ++ m_signals;
	}
    }
  else
    {
      // We caught a signal.
      wxASSERT( m_signals > m_canceled);
      -- m_waiters;
      if ( -- m_signals == m_canceled)
	{
	  // This was the last signal. open the gate.
	  wxASSERT( m_waiters == m_canceled);
	  m_gate.Post();
	}
    }
  m_varSection.Leave();

  m_mutex.Lock();

  if ( err)
    {
      return err == wxSEMA_TIMEOUT ? wxCOND_TIMEOUT : wxCOND_MISC_ERROR;
    }
    
  return wxCOND_NO_ERROR;
}
  

wxCondError wxConditionInternal::DoSignal( bool signalAll)
{
  m_gate.Wait();
  m_varSection.Enter();

  wxASSERT( m_signals == m_canceled);

  if ( m_waiters == m_canceled)
    {
      m_varSection.Leave();
      m_gate.Post();
      return wxCOND_NO_ERROR;
    }

  if ( m_canceled > 0)
    {
      m_waiters -= m_canceled;
      m_signals = 0;
      m_canceled = 0;
    }

  m_signals = signalAll ? m_waiters : 1;
  size_t n = m_signals;

  m_varSection.Leave();

  // Let the waiters inherit the gate lock.

  do
    {
      wxSemaError err = m_semaphore.Post();
      wxASSERT( err == wxSEMA_NO_ERROR);
    } while ( -- n);

  return wxCOND_NO_ERROR;
}



// ----------------------------------------------------------------------------
// wxCriticalSection implementation
// ----------------------------------------------------------------------------

// XXX currently implemented as mutex in headers. Change to critical section.

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
    m_tid = 0;
    m_state = STATE_NEW;
    m_priority = WXTHREAD_DEFAULT_PRIORITY;
    m_notifyQueueId = 0;
  }

  ~wxThreadInternal()
  {
    if ( m_notifyQueueId)
      MPDeleteQueue( m_notifyQueueId);
  }

  void Free()
  {
  }

  // create a new (suspended) thread (for the given thread object)
  // stacksize == 0 will give the default stack size of 4 KB.
  bool Create(wxThread *thread, unsigned int stackSize);

  void Run();

  // suspend/resume/terminate
  bool Suspend();
  bool Resume();
  void Cancel() 
  { 
    wxCriticalSectionLocker lock( m_critical);
    m_state = STATE_CANCELED; 
  }

  wxThread::ExitCode Wait();

  // thread state
  void SetState(wxThreadState state) 
  { 
    wxCriticalSectionLocker lock( m_critical);
    m_state = state; 
  }

  wxThreadState GetState() const
  {
    wxCriticalSectionLocker lock( m_critical);
    return m_state; 
  }

  // thread priority
  void SetPriority(unsigned int priority);
  unsigned int GetPriority() const 
  { 
    wxCriticalSectionLocker lock( m_critical);
    return m_priority; 
  }

  void SetResult( void *res ) 
  { 
    wxCriticalSectionLocker lock( m_critical);
    m_result = res ; 
  }
  
  void *GetResult() 
  { 
    wxCriticalSectionLocker lock( m_critical);
    return m_result ; 
  }

  // Get the ID of this thread's underlying MP Services task.
  MPTaskID  GetId() const 
  { 
    wxCriticalSectionLocker lock( m_critical);
    return m_tid; 
  }

  // thread function
  static OSStatus    MacThreadStart(void* arg);

private:

  wxThreadState	    m_state;	  // state, see wxThreadState enum
  unsigned int	    m_priority;	  // thread priority in "wx" units
  MPTaskID		    m_tid;	  // thread id
  void*		    m_result;
    
  mutable wxCriticalSection	  m_critical;
  MPQueueID		    m_notifyQueueId;
  unsigned int	    m_stackSize;
  wxThread*		    m_thread;
};

static wxArrayPtrVoid s_threads ;

OSStatus wxThreadInternal::MacThreadStart(void *parameter)
{
  wxThread* thread = (wxThread*) parameter ;
  // first of all, check whether we hadn't been cancelled already
  if ( thread->m_internal->GetState() == STATE_EXITED )
    {
      return kMPDeletedErr;
    }

  thread->Entry();

  thread->m_critsect.Enter();
  bool wasCancelled = thread->m_internal->GetState() == STATE_CANCELED;
  thread->m_internal->SetState(STATE_EXITED);
  thread->m_critsect.Leave();

  thread->OnExit();

  // If the thread was cancelled (from Delete()), then the handle is still
  // needed there.
  if ( thread->IsDetached() && !wasCancelled )
    {
      delete thread;
    }
  //else: the joinable threads handle will be closed when Wait() is done

  return noErr;
}

void wxThreadInternal::SetPriority(unsigned int priority)
{
  wxASSERT_MSG( priority >= 0, _T("Thread priority must be at least 0."));
  wxASSERT_MSG( priority <= 100, _T("Thread priority must be at most 100."));

  wxCriticalSectionLocker lock(m_critical);

  m_priority = priority;

  if ( m_tid)
    {
      // Mac priorities range from 1 to 10,000, with a default of 100.
      // wxWindows priorities range from 0 to 100 with a default of 50.
      // We can map wxWindows to Mac priorities easily by assuming
      // the former uses a logarithmic scale.
      const unsigned int macPriority = ( int)( exp( priority / 25.0 * log( 10.0)) + 0.5);

      MPSetTaskWeight( m_tid, macPriority);
    }
}


void wxThreadInternal::Run()
{
  OSStatus err;

  m_state = STATE_NEW;

  err = MPCreateTask( MacThreadStart,
		      (void*) m_thread,
		      m_stackSize,
		      m_notifyQueueId,
		      &m_result,
		      0,
		      0,
		      &m_tid);

  if ( err)
    {
      wxLogSysError(_("Can't create thread"));
	
      return;
    }

  if ( m_priority != WXTHREAD_DEFAULT_PRIORITY )
    {
      SetPriority(m_priority);
    }

  m_state = STATE_RUNNING;
}


bool wxThreadInternal::Create(wxThread *thread, unsigned int stackSize)
{
  m_stackSize = stackSize;
  m_thread = thread;

  if ( ! m_notifyQueueId)
    {
      OSStatus err = MPCreateQueue( & m_notifyQueueId);
      if( err)
	{
	  wxLogSysError(_("Cant create the thread event queue"));
	  return FALSE;
	}
    }

  return true;
}

bool wxThreadInternal::Suspend()
{
  wxCriticalSectionLocker lock(m_critical);

  if ( m_state != STATE_RUNNING )
    {
      wxLogSysError(_("Can not suspend thread %x"), m_tid);
      return FALSE;
    }

  m_state = STATE_PAUSED;
    
  OSStatus err = MPThrowException( m_tid, kMPTaskStoppedErr);

  if ( err)
    {
      wxLogSysError(_("Can not suspend thread %x"), m_tid);
      return FALSE;
    }

  return TRUE;
}

bool wxThreadInternal::Resume()
{
  wxCriticalSectionLocker lock(m_critical);

  if ( m_state != STATE_PAUSED && m_state != STATE_NEW )
    {
      wxLogSysError(_("Can not resume thread %x"), m_tid);
      return FALSE;
    }
    
  OSStatus err = MPDisposeTaskException( m_tid, kMPTaskResumeMask);
  if ( err)
    {
      wxLogSysError(_("Cannot resume thread %x"), m_tid);
      return FALSE;	 
    }

  m_state = STATE_RUNNING;

  return TRUE;
}


wxThread::ExitCode wxThreadInternal::Wait()
{
  void * param1;
  void * param2;
  void * rc;

  OSStatus err = MPWaitOnQueue ( m_notifyQueueId, 
				 & param1, 
				 & param2, 
				 & rc, 
				 kDurationForever);
  if ( err)
    {
      wxLogSysError( _( "Cannot wait on thread to exit."));
      return ( wxThread::ExitCode)-1;
    }

  m_result = rc;

  return ( wxThread::ExitCode)rc;
}


// static functions
// ----------------
wxThread *wxThread::This()
{
  MPTaskID current = MPCurrentTaskID();

  {
    wxMacStCritical critical ;

    for ( size_t i = 0 ; i < s_threads.Count() ; ++i )
      {
	if ( ( (wxThread*) s_threads[i] )->GetId() == (unsigned long)current )
	  return (wxThread*) s_threads[i] ;
      }
  }

  wxLogSysError(_("Couldn't get the current thread pointer"));
  return NULL;
}

bool wxThread::IsMain()
{
  return MPCurrentTaskID() == gs_idMainThread;
}

#ifdef Yield
#undef Yield
#endif

void wxThread::Yield()
{
#if TARGET_API_MAC_OSX
   CFRunLoopRunInMode( kCFRunLoopDefaultMode , 0 , true ) ;
#endif
  MPYield();
}


void wxThread::Sleep(unsigned long milliseconds)
{
  AbsoluteTime wakeup = AddDurationToAbsolute( milliseconds, UpTime());
  MPDelayUntil( & wakeup);
}


int wxThread::GetCPUCount()
{
  return MPProcessors();
}

unsigned long wxThread::GetCurrentId()
{
  return (unsigned long)MPCurrentTaskID();
}


// NOT IMPLEMENTED.
bool wxThread::SetConcurrency(size_t level)
{
  return false;
}


wxThread::wxThread(wxThreadKind kind)
{
  g_numberOfThreads++;
  m_internal = new wxThreadInternal();

  m_isDetached = kind == wxTHREAD_DETACHED;

  {
    wxMacStCritical critical;
    s_threads.Add( (void*) this) ;
  }
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

  {
    wxMacStCritical critical;
    s_threads.Remove( (void*) this ) ;
  }
  
  if (m_internal != NULL) {
    delete m_internal;
    m_internal = NULL;
  }
}


wxThreadError wxThread::Create(unsigned int stackSize)
{
  wxCriticalSectionLocker lock(m_critsect);

  if ( !m_internal->Create(this, stackSize) )
    return wxTHREAD_NO_RESOURCE;

  return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
  m_internal->Run();

  return wxTHREAD_NO_ERROR;
}

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


wxThread::ExitCode wxThread::Wait()
{
  // although under MacOS we can wait for any thread, it's an error to
  // wait for a detached one in wxWin API
  wxCHECK_MSG( !IsDetached(), 
	       (ExitCode)-1,
	       _T("can't wait for detached thread") );

  m_internal->Wait();

  ExitCode rc;
  (void)Delete(&rc);

  m_internal->Free();

  return rc;
}

wxThreadError wxThread::Delete( ExitCode *pRc)
{
  // Delete() is always safe to call, so consider all possible states

  bool shouldResume = FALSE;

  {
    wxCriticalSectionLocker lock(m_critsect);

    if ( m_internal->GetState() == STATE_NEW )
      {
	// MacThreadStart() will see it and terminate immediately
	m_internal->SetState(STATE_EXITED);

	shouldResume = TRUE;
      }
  }

  if ( shouldResume || IsPaused() )
    Resume();

  if ( IsRunning() )
    {
      if ( IsMain() )
	{
	  {
	    wxMacStCritical critical;
	    gs_waitingForThread = TRUE;
	  }
#if wxUSE_GUI
	  wxBeginBusyCursor();
#endif // wxUSE_GUI
	}

      {
	wxCriticalSectionLocker lock(m_critsect);
	m_internal->Cancel();
      }

      while( TestDestroy() )
	{
	  MPYield() ;
	}

      if ( IsMain() )
	{
	  {
	    wxMacStCritical critical;
	    gs_waitingForThread = FALSE;
	  }
#if wxUSE_GUI
	  wxEndBusyCursor();
#endif // wxUSE_GUI
	}
    }

  if ( pRc)
    * pRc = (ExitCode)m_internal->GetResult();

  if ( IsDetached() )
    {
      // If the thread exits normally, this is done in
      // MachreadStart, but in this case it would have been too
      // early, so we must do it here.
      delete this;
    }

  if ( pRc && * pRc == (ExitCode)-1)
    return wxTHREAD_MISC_ERROR;
  return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Kill()
{
  if ( !IsRunning() )
    return wxTHREAD_NOT_RUNNING;

  if ( MPTerminateTask( m_internal->GetId(), -1) )
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
  else
    {
      m_internal->SetResult( status ) ;
    }
}

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
  bool hasThreadManager ;
  hasThreadManager = MPLibraryIsLoaded();
    
#if !TARGET_CARBON
#if GENERATINGCFM
  // verify presence of shared library
  hasThreadManager = hasThreadManager && ((Ptr)NewThread != (Ptr)kUnresolvedCFragSymbolAddress);
#endif
#endif
  if ( !hasThreadManager )
    {
      wxMessageBox( "Error" , "Thread Support is not available on this System" , wxOK ) ;
      return FALSE ;
    }

  gs_idMainThread = MPCurrentTaskID();

#if wxUSE_GUI

  OSStatus  err = MPCreateCriticalRegion( & gs_guiCritical);
  if ( err)
    {
      wxLogSysError(_("Could not make the GUI critical region."));
    }
    
  // XXX wxMac never Exits the GUI Mutex.
  // XXX MPEnterCriticalRegion( gs_guiCritical, kDurationForever);
#endif

  return TRUE;
}

void wxThreadModule::OnExit()
{
#if wxUSE_GUI
  MPExitCriticalRegion( gs_guiCritical);

  MPDeleteCriticalRegion( gs_guiCritical);
#endif
}


void WXDLLEXPORT wxMutexGuiEnter()
{
#if wxUSE_GUI
  MPEnterCriticalRegion( gs_guiCritical, kDurationForever);
#endif
}

void WXDLLEXPORT wxMutexGuiLeave()
{
#if wxUSE_GUI
  MPExitCriticalRegion( gs_guiCritical);
#endif
}


// NOT IMPLEMENTED
void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
}

// NOT IMPLEMENTED
bool WXDLLEXPORT wxGuiOwnedByMainThread()
{
  return true;
}

// wake up the main thread
void WXDLLEXPORT wxWakeUpMainThread()
{
  wxMacWakeUp() ;
}

bool WXDLLEXPORT wxIsWaitingForThread()
{
  return gs_waitingForThread;
}

#include "wx/thrimpl.cpp"

#endif // wxUSE_THREADS

// vi:sts=4:sw=4:et

