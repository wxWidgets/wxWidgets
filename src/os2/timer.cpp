/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/os2/private.h"

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

#include <sys/timeb.h>

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

wxList wxTimerList(wxKEY_INTEGER);
UINT wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

wxTimer::wxTimer()
{
    milli = 0 ;
    id = 0;
    oneShot = FALSE;
}

wxTimer::~wxTimer()
{
    Stop();

    wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds,bool mode)
{
    oneShot = mode;
    if (milliseconds < 0)
        milliseconds = lastMilli;

    wxCHECK_MSG( milliseconds > 0, FALSE, wxT("invalid value for timer timeour") );

    lastMilli = milli = milliseconds;

    wxTimerList.DeleteObject(this);
// TODO:
/*
    TIMERPROC wxTimerProcInst = (TIMERPROC)
        MakeProcInstance((FARPROC)wxTimerProc, wxGetInstance());

    id = SetTimer(NULL, (UINT)(id ? id : 1),
                  (UINT)milliseconds, wxTimerProcInst);
*/
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
//        KillTimer(NULL, (UINT)id);
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

UINT wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
    wxNode *node = wxTimerList.Find((long)idTimer);

    wxCHECK_MSG( node, 0, wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(wxTimer *)node->Data());

    return 0;
}

