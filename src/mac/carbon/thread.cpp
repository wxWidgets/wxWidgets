/////////////////////////////////////////////////////////////////////////////
// Name:        thread.cpp
// Purpose:     wxThread Implementation. For Unix ports, see e.g. src/gtk
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

#include "wx/module.h"
#include "wx/thread.h"
#include "wx/utils.h"

enum thread_state {
  STATE_IDLE = 0,
  STATE_RUNNING,
  STATE_CANCELED,
  STATE_EXITED
};

/////////////////////////////////////////////////////////////////////////////
// Static variables
/////////////////////////////////////////////////////////////////////////////

wxMutex *wxMainMutex; // controls access to all GUI functions

/////////////////////////////////////////////////////////////////////////////
// Windows implementation
/////////////////////////////////////////////////////////////////////////////

class wxMutexInternal {
public:
  // TODO: internal mutex handle
};

wxMutex::wxMutex()
{
    p_internal = new wxMutexInternal;
    // TODO: create internal mutext handle
    m_locked = 0;
}

wxMutex::~wxMutex()
{
    if (m_locked > 0)
        wxDebugMsg("wxMutex warning: freeing a locked mutex (%d locks)\n", m_locked);
    // TODO: free internal mutext handle
}

wxMutexError wxMutex::Lock()
{
    // TODO
    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::TryLock()
{
    // TODO
    m_locked++;
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutex::Unlock()
{
    if (m_locked > 0)
        m_locked--;

    // TODO
    return wxMUTEX_NO_ERROR;
}

class wxConditionInternal {
public:
  // TODO: internal handle
  int waiters;
};

wxCondition::wxCondition()
{
    p_internal = new wxConditionInternal;
    // TODO: create internal handle
    p_internal->waiters = 0;
}

wxCondition::~wxCondition()
{
    // TODO: destroy internal handle
}

void wxCondition::Wait(wxMutex& mutex)
{
    mutex.Unlock();
    p_internal->waiters++;
    // TODO wait here
    p_internal->waiters--;
    mutex.Lock();
}

bool wxCondition::Wait(wxMutex& mutex, unsigned long sec,
                       unsigned long nsec)
{
    mutex.Unlock();
    p_internal->waiters++;

    // TODO wait here
    p_internal->waiters--;
    mutex.Lock();

    return FALSE;
}

void wxCondition::Signal()
{
    // TODO
}

void wxCondition::Broadcast()
{
    // TODO
}

class wxThreadInternal {
public:
    // TODO
};

wxThreadError wxThread::Create()
{
    // TODO
    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Destroy()
{
    // TODO
    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Pause()
{
    // TODO
    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Resume()
{
    // TODO
    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(void *status)
{
    // TODO
}

void wxThread::SetPriority(int prio)
{
    // TODO
}

int wxThread::GetPriority() const
{
    // TODO
    return 0;
}

void wxThread::DeferDestroy(bool on)
{
    // TODO
}

void wxThread::TestDestroy()
{
    // TODO
}

void *wxThread::Join()
{
    // TODO
    return (void*) NULL;
}

unsigned long wxThread::GetID() const
{
    // TODO
    return 0;
}

/* is this needed somewhere ?
wxThread *wxThread::GetThreadFromID(unsigned long id)
{
    // TODO
    return NULL;
}
*/

bool wxThread::IsAlive() const
{
    // TODO
    return FALSE;
}

bool wxThread::IsRunning() const
{
    // TODO
    return FALSE;
}

bool wxThread::IsMain()
{
    // TODO
    return FALSE;
}

wxThread::wxThread()
{
    p_internal = new wxThreadInternal();

    // TODO
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
    /* TODO p_mainid = GetCurrentThread(); */
    wxMainMutex = new wxMutex();
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

