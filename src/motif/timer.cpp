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

#include <Xm/Xm.h>

#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

static wxList wxTimerList(wxKEY_INTEGER);

void wxTimerCallback (wxTimer * timer)
{
  // Check to see if it's still on
  if (!wxTimerList.Find((long)timer))
    return;

  if (timer->m_id == 0)
    return;			// Avoid to process spurious timer events

  if (!timer->m_oneShot)
    timer->m_id = XtAppAddTimeOut ((XtAppContext) wxTheApp->GetAppContext(), timer->m_milli,
		  (XtTimerCallbackProc) wxTimerCallback, (XtPointer) timer);
  else
    timer->m_id = 0;
  timer->Notify ();
}

wxTimer::wxTimer()
{
    m_id = 0;
    m_milli = 0 ;
    m_id = 0;
    m_oneShot = FALSE;
}

wxTimer::~wxTimer()
{
    Stop();
    wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds, bool mode)
{
    Stop();

    m_oneShot = mode;
    if (milliseconds < 0)
        milliseconds = m_lastMilli;

    if (milliseconds <= 0)
        return FALSE;

    m_lastMilli = m_milli = milliseconds;

    if (!wxTimerList.Find((long)this))
        wxTimerList.Append((long)this, this);

    m_id = XtAppAddTimeOut ((XtAppContext) wxTheApp->GetAppContext(), milliseconds,
		   (XtTimerCallbackProc) wxTimerCallback, (XtPointer) this);
    return TRUE;
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


