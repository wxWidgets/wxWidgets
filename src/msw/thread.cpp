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

// this is here to regen the precompiled header in the ide compile otherwise the
// compiler crashes in vc5 (nfi why)
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdio.h>

#include <windows.h>
#include "wx/module.h"
#include "wx/thread.h"

enum thread_state {
  STATE_IDLE = 0,
  STATE_RUNNING,
  STATE_CANCELED,
  STATE_EXITED
};

/////////////////////////////////////////////////////////////////////////////
// Static variables
/////////////////////////////////////////////////////////////////////////////

static HANDLE p_mainid;
wxMutex *wxMainMutex; // controls access to all GUI functions

/////////////////////////////////////////////////////////////////////////////
// Windows implementation
/////////////////////////////////////////////////////////////////////////////

class wxMutexInternal {
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
  if (ret == WAIT_ABANDONED)
    return wxMUTEX_BUSY;

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
  if ( ret != 0 )
  {
      wxLogSysError(_("Couldn't release a mutex"));
      return wxMUTEX_MISC_ERROR;
  }

  return wxMUTEX_NO_ERROR;
}

class wxConditionInternal {
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

bool wxCondition::Wait(wxMutex& mutex, unsigned long sec,
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

class wxThreadInternal {
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

  if (p_internal->defer == FALSE)
    TerminateThread(p_internal->thread_id, 0);
  else
    p_internal->state = STATE_CANCELED;

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

void wxThread::TestDestroy()
{
  if (p_internal->state == STATE_CANCELED)
    ExitThread(0);
}

void *wxThread::Join()
{
  DWORD exit_code;

  if (p_internal->state == STATE_IDLE)
    return NULL;

  if (wxThread::IsMain())
    wxMainMutex->Unlock();
  WaitForSingleObject(p_internal->thread_id, INFINITE);
  if (wxThread::IsMain())
    wxMainMutex->Lock();

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
  return (GetCurrentThread() == p_mainid);
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

// The default callback just joins the thread and throws away the result.
void wxThread::OnExit()
{
  Join();
}

// Automatic initialization
class wxThreadModule : public wxModule {
  DECLARE_DYNAMIC_CLASS(wxThreadModule)
public:
  virtual bool OnInit() {
    wxMainMutex = new wxMutex();
    p_mainid = GetCurrentThread();
    wxMainMutex->Lock();
    return TRUE;
  }

  // Global cleanup
  virtual void OnExit() {
    wxMainMutex->Unlock();
    delete wxMainMutex;
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)

void WXDLLEXPORT wxMutexGuiEnter()
{
  wxFAIL_MSG("not implemented");
}

void WXDLLEXPORT wxMutexGuiLeave()
{
  wxFAIL_MSG("not implemented");
}
