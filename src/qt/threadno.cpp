/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     No thread support
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

#include "wx/wx.h"
#include "wx/module.h"
#include "wx/thread.h"

wxMutex::wxMutex()
{
  m_locked = 0;
}

wxMutex::~wxMutex()
{
  if (m_locked)
    wxDebugMsg("wxMutex warning: destroying a locked mutex (%d locks)\n", m_locked);
}

wxMutexError wxMutex::Lock()
{
  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
  if (m_locked > 0)
    return MUTEX_BUSY;
  m_locked++;
  return MUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
  if (m_locked == 0)
    return MUTEX_UNLOCKED;
  m_locked--;
  return MUTEX_NO_ERROR;
}

wxCondition::wxCondition()
{
}

wxCondition::~wxCondition()
{
}

void wxCondition::Wait(wxMutex& WXUNUSED(mutex))
{
}

bool wxCondition::Wait(wxMutex& WXUNUSED(mutex), unsigned long WXUNUSED(sec),
                      unsigned long WXUNUSED(nsec))
{
  return FALSE;
}

void wxCondition::Signal()
{
}

void wxCondition::Broadcast()
{
}

struct wxThreadInternal {
	int thread_id;
	void* exit_status;
};

wxThreadError wxThread::Create()
{
  p_internal->exit_status = Entry();
  OnExit();
  return THREAD_NO_ERROR;
}

wxThreadError wxThread::Destroy()
{
  return THREAD_NOT_RUNNING;
}

wxThreadError wxThread::Pause()
{
  return THREAD_NOT_RUNNING;
}

wxThreadError wxThread::Resume()
{
  return THREAD_NOT_RUNNING;
}

void wxThread::DeferDestroy( bool WXUNUSED(on) )
{
}

void wxThread::TestDestroy()
{
}

void *wxThread::Join()
{
  return p_internal->exit_status;
}

unsigned long wxThread::GetID() const
{
  return 0;
}

wxThread *wxThread::GetThreadFromID(unsigned long WXUNUSED(id)) const
{
  return NULL;
}

bool wxThread::IsMain()
{
  return TRUE;
}

bool wxThread::IsRunning() const
{
  return FALSE;
}

bool wxThread::IsAlive() const
{
  return FALSE;
}

void wxThread::SetPriority(int WXUNUSED(prio)) { }
int wxThread::GetPriority() const { return 0; }

wxMutex wxMainMutex; // controls access to all GUI functions

wxThread::wxThread()
{
  p_internal = new wxThreadInternal();
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
  bool OnInit();
  void OnExit();
};

bool wxThreadModule::OnInit() {
  wxMainMutex.Lock();
  return TRUE;
}

void wxThreadModule::OnExit()
{
  wxMainMutex.Unlock();
}

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)
