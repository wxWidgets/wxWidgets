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

wxMutex::wxMutex(void)
{
  m_locked = FALSE;
}

wxMutex::~wxMutex(void)
{
}

MutexError wxMutex::Lock(void)
{
  m_locked = TRUE;
  return NO_ERROR;
}

MutexError wxMutex::TryLock(void)
{
  m_locked = TRUE;
  return NO_ERROR;
}

MutexError wxMutex::Unlock(void)
{
  m_locked = FALSE;
  return NO_ERROR;
}

wxCondition::wxCondition(void)
{
}

wxCondition::~wxCondition(void)
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

void wxCondition::Signal(void)
{
}

void wxCondition::Broadcast(void)
{
}

struct wxThreadPrivate {
	int thread_id;
	void* exit_status;
};

ThreadError wxThread::Create(void)
{
  p_internal->exit_status = Entry();
  OnExit();
  return NO_ERROR;
}

ThreadError wxThread::Destroy(void)
{
  return RUNNING;
}

void wxThread::DifferDestroy(void)
{
}

void wxThread::TestDestroy(void)
{
}

void *wxThread::Join()
{
  return p_internal->exit_status;
}

unsigned long wxThread::GetID()
{
  return 0;
}

bool wxThread::IsMain(void)
{
  return TRUE;
}

bool wxThread::IsAlive(void)
{
  return FALSE;
}

void wxThread::SetPriority(int WXUNUSED(prio)) { }
int wxThread::GetPriority(void) { }

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
