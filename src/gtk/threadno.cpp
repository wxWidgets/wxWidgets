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
  m_locked = FALSE;
}

wxMutex::~wxMutex()
{
}

MutexError wxMutex::Lock()
{
  m_locked = TRUE;
  return NO_ERROR;
}

MutexError wxMutex::TryLock()
{
  m_locked = TRUE;
  return NO_ERROR;
}

MutexError wxMutex::Unlock()
{
  m_locked = FALSE;
  return NO_ERROR;
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
  return NO_ERROR;
}

ThreadError wxThread::Destroy()
{
  return RUNNING;
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

// Global initialization
static void wxThreadInit(void *WXUNUSED(client))
{
  wxMainMutex.Lock();
}

// Global cleanup
static void wxThreadExit(void *WXUNUSED(client))
{
  wxMainMutex.Unlock();
}

// Let automatic initialization be performed from wxCommonInit().
static struct
wxThreadGlobal {
  wxThreadGlobal() {
      wxRegisterModuleFunction(wxThreadInit, wxThreadExit, NULL);
  }
} dummy;
