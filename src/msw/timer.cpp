/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "timer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/window.h"
#include "wx/msw/private.h"

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
#endif

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/timer.h"

#include <time.h>
#include <sys/types.h>

#if !defined(__SC__) && !defined(__GNUWIN32__) && !defined(__MWERKS__)
    #include <sys/timeb.h>
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

wxList wxTimerList(wxKEY_INTEGER);
UINT WINAPI _EXPORT wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#ifdef __WIN32__
    #define _EXPORT
#else
    #define _EXPORT _export
#endif

#if !USE_SHARED_LIBRARY
    IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

wxTimer::wxTimer()
{
    milli = 0;
    lastMilli = -1;
    id = 0;
}

wxTimer::~wxTimer()
{
    Stop();

    wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds, bool mode)
{
    oneShot = mode;
    if (milliseconds < 0)
        milliseconds = lastMilli;

    wxCHECK_MSG( milliseconds > 0, FALSE, wxT("invalid value for timer timeour") );

    lastMilli = milli = milliseconds;

    wxTimerList.DeleteObject(this);
    TIMERPROC wxTimerProcInst = (TIMERPROC)
        MakeProcInstance((FARPROC)wxTimerProc, wxGetInstance());

    id = SetTimer(NULL, (UINT)(id ? id : 1),
                  (UINT)milliseconds, wxTimerProcInst);
    if (id > 0)
    {
        wxTimerList.Append(id, this);

        return TRUE;
    }
    else
    {
        wxLogSysError(_("Couldn't create a timer"));

        return FALSE;
    }
}

void wxTimer::Stop()
{
    if ( id )
    {
        KillTimer(NULL, (UINT)id);
        wxTimerList.DeleteObject(this);
    }
    id = 0;
    milli = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxTimer& timer)
{
    // Avoid to process spurious timer events
    if ( timer.id == 0)
        return;

    if ( timer.oneShot )
        timer.Stop();

    timer.Notify();
}

UINT WINAPI _EXPORT wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
    wxNode *node = wxTimerList.Find((long)idTimer);

    wxCHECK_MSG( node, 0, wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(wxTimer *)node->Data());

    return 0;
}
