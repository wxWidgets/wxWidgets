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

wxMutex::wxMutex()
{
  m_locked = 0;
}

wxMutex::~wxMutex()
{
  if (m_locked)
    wxDebugMsg("wxMutex warning: destroying a locked mutex (%d locks)\n", m_locked);
}

MutexError wxMutex::Lock()
{
  m_locked++;
  return MUTEX_NO_ERROR;
}

MutexError wxMutex::TryLock()
{
  if (m_locked > 0)
    return MUTEX_BUSY;
  m_locked++;
  return MUTEX_NO_ERROR;
}

MutexError wxMutex::Unlock()
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

struct wxThreadPrivate {
	int thread_id;
	void* exit_status;
};

ThreadError wxThread::Create()
{
  p_internal->exit_status = Entry();
  OnExit();
  return THREAD_NO_ERROR;
}

ThreadError wxThread::Destroy()
{
  return THREAD_RUNNING;
}

void wxThread::DeferDestroy()
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

bool wxThread::IsMain()
{
  return TRUE;
}

bool wxThread::IsAlive() const
{
  return FALSE;
}

void wxThread::SetPriority(int WXUNUSED(prio)) { }
int wxThread::GetPriority() const { }

wxMutex wxMainMutex; // controls access to all GUI functions

wxThread::wxThread()
{
  p_internal = new wxThreadPrivate();
}

wxThread::~wxThread()
{
  Cancel();
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

void wxThreadModule::wxThreadExit()
{
  wxMainMutex.Unlock();
}

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)
