/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"
#include "wx/app.h"
#include "wx/list.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif

#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/x11/private.h"

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)

static wxList wxTimerList(wxKEY_INTEGER);

void wxTimerCallback (wxTimer * timer)
{
  // Check to see if it's still on
  if (!wxTimerList.Find((long)timer))
    return;

  if (timer->m_id == 0)
    return;			// Avoid to process spurious timer events

  // TODO
#if 0
  if (!timer->m_oneShot)
    timer->m_id = XtAppAddTimeOut((XtAppContext) wxTheApp->GetAppContext(),
                                  timer->m_milli,
                                  (XtTimerCallbackProc) wxTimerCallback,
                                  (XtPointer) timer);
  else
#endif
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
    wxTimer::Stop();
    wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds, bool mode)
{
    Stop();

    (void)wxTimerBase::Start(milliseconds, mode);

    if (!wxTimerList.Find((long)this))
        wxTimerList.Append((long)this, this);
    // TODO
#if 0
    m_id = XtAppAddTimeOut((XtAppContext) wxTheApp->GetAppContext(),
                            m_milli,
                            (XtTimerCallbackProc) wxTimerCallback,
                            (XtPointer) this);
#endif
    return TRUE;
}

void wxTimer::Stop()
{
    if (m_id > 0)
    {
    // TODO
#if 0
        XtRemoveTimeOut (m_id);
#endif
        m_id = 0;
    }
    m_milli = 0 ;
}


