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
wxMutex wxMainMutex; // controls access to all GUI functions

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
  m_locked = 0;
}

wxMutex::~wxMutex()
{
  CloseHandle(p_internal->p_mutex);
}

wxMutexError wxMutex::Lock()
{
  DWORD ret;

  ret = WaitForSingleObject(p_internal->p_mutex, INFINITE);
  if (ret == WAIT_ABANDONED)
    return MUTEX_BUSY;

  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
  DWORD ret;

  ret = WaitForSingleObject(p_internal->p_mutex, 0);
  if (ret == WAIT_TIMEOUT || ret == WAIT_ABANDONED)
    return MUTEX_BUSY;

  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
  BOOL ret;

  if (m_locked > 0)
    m_locked--;

    // Why does this have 3 args? The redundant ones removed by JACS
//  ret = ReleaseMutex(p_internal->p_mutex, 1, NULL);
  ret = ReleaseMutex(p_internal->p_mutex);
  return MUTEX_NO_ERROR;
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
    SetEvent(p_internal->event);
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
  int win_prio, prio = p_internal->prio;

  p_internal->thread_id = CreateThread(NULL, 0,
                   (LPTHREAD_START_ROUTINE)wxThreadInternal::WinThreadStart,
                   (void *)this, CREATE_SUSPENDED, &p_internal->tid);
  if (p_internal->thread_id == NULL) {
    printf("Error = %d\n", GetLastError());
    return THREAD_NO_RESOURCE;
  }

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

  SetThreadPriority(p_internal->thread_id, win_prio);

  ResumeThread(p_internal->thread_id);
  p_internal->state = STATE_RUNNING;
 
  return THREAD_NO_ERROR;
}

wxThreadError wxThread::Destroy()
{
  if (p_internal->state != STATE_RUNNING)
    return THREAD_NOT_RUNNING;

  if (p_internal->defer == FALSE)
    TerminateThread(p_internal->thread_id, 0);
  else
    p_internal->state = STATE_CANCELED;

  return THREAD_NO_ERROR;
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

int wxThread::GetPriority()
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
    wxMainMutex.Unlock();
  WaitForSingleObject(p_internal->thread_id, INFINITE);
  if (wxThread::IsMain())
    wxMainMutex.Lock();

  GetExitCodeThread(p_internal->thread_id, &exit_code);
  CloseHandle(p_internal->thread_id);

  p_internal->state = STATE_IDLE;

  return (void *)exit_code;
}

unsigned long wxThread::GetID() const
{
  return (unsigned long)p_internal->tid;
}

bool wxThread::IsMain() const
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
    p_mainid = GetCurrentThread();
    wxMainMutex.Lock();
    return TRUE;
  }

  // Global cleanup
  virtual void OnExit() {
    wxMainMutex.Unlock();
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)

