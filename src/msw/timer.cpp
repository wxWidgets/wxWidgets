/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/app.h"
#endif

#include "wx/timer.h"
#include "wx/msw/private.h"

#include <time.h>
#include <sys/types.h>

#if !defined(__SC__) && !defined(__GNUWIN32__)
#include <sys/timeb.h>
#endif
#ifdef __WIN32__
#define _EXPORT /**/
#else
#define _EXPORT _export
#endif

wxList wxTimerList(wxKEY_INTEGER);
UINT WINAPI _EXPORT wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

wxTimer::wxTimer(void)
{
  milli = 0 ;
  lastMilli = -1 ;
  id = 0;
}

wxTimer::~wxTimer(void)
{
  Stop();

  wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds,bool mode)
{
  oneShot = mode ;
  if (milliseconds < 0)
    milliseconds = lastMilli;

  if (milliseconds <= 0)
    return FALSE;

  lastMilli = milli = milliseconds;

  wxTimerList.DeleteObject(this);
  TIMERPROC wxTimerProcInst = (TIMERPROC) MakeProcInstance((FARPROC)wxTimerProc,
                                          wxGetInstance());

  id = SetTimer(NULL, (UINT)(id ? id : 1), (UINT)milliseconds, wxTimerProcInst);
  if (id > 0)
  {
    wxTimerList.Append(id, this);
    return TRUE;
  }
  else return FALSE;
}

void wxTimer::Stop(void)
{
  if (id) {
    KillTimer(NULL, (UINT)id);
    wxTimerList.DeleteObject(this); /* @@@@ */
  }
  id = 0 ;
  milli = 0 ;
}

UINT WINAPI _EXPORT wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
  wxNode *node = wxTimerList.Find((long)idTimer);
  if (node)
  {
    wxTimer *timer = (wxTimer *)node->Data();
    if (timer->id==0)
      return(0) ; // Avoid to process spurious timer events
    if (timer->oneShot)
      timer->Stop() ;
    timer->Notify();
  }
  return 0;
}

