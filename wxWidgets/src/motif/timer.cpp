/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/timer.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/hashmap.h"
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

WX_DECLARE_VOIDPTR_HASH_MAP(wxTimer*, wxTimerHashMap);

static wxTimerHashMap s_timers;

void wxTimerCallback (wxTimer * timer)
{
  // Check to see if it's still on
  if (s_timers.find(timer) == s_timers.end())
    return;

  if (timer->m_id == 0)
    return;            // Avoid to process spurious timer events

  if (!timer->m_oneShot)
    timer->m_id = XtAppAddTimeOut((XtAppContext) wxTheApp->GetAppContext(),
                                  timer->m_milli,
                                  (XtTimerCallbackProc) wxTimerCallback,
                                  (XtPointer) timer);
  else
    timer->m_id = 0;

  timer->Notify();
}

void wxTimer::Init()
{
    m_id = 0;
    m_milli = 1000;
}

wxTimer::~wxTimer()
{
    Stop();
    s_timers.erase(this);
}

bool wxTimer::Start(int milliseconds, bool mode)
{
    Stop();

    (void)wxTimerBase::Start(milliseconds, mode);

    if (s_timers.find(this) == s_timers.end())
        s_timers[this] = this;

    m_id = XtAppAddTimeOut((XtAppContext) wxTheApp->GetAppContext(),
                            m_milli,
                            (XtTimerCallbackProc) wxTimerCallback,
                            (XtPointer) this);
    return true;
}

void wxTimer::Stop()
{
    if (m_id > 0)
    {
        XtRemoveTimeOut (m_id);
        m_id = 0;
    }
    m_milli = 0 ;
}
