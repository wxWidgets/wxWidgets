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

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/window.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/timer.h"

#include "wx/msw/private.h"

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

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

void wxTimer::Init()
{
    m_id = 0;
}

wxTimer::~wxTimer()
{
    wxTimer::Stop();

    wxTimerList.DeleteObject(this);
}

bool wxTimer::Start(int milliseconds, bool oneShot)
{
    (void)wxTimerBase::Start(milliseconds, oneShot);

    wxCHECK_MSG( m_milli > 0, FALSE, wxT("invalid value for timer timeour") );

    wxTimerList.DeleteObject(this);
    TIMERPROC wxTimerProcInst = (TIMERPROC)
        MakeProcInstance((FARPROC)wxTimerProc, wxGetInstance());

    m_id = SetTimer(NULL, (UINT)(m_id ? m_id : 1),
                    (UINT)milliseconds, wxTimerProcInst);
    if ( m_id > 0 )
    {
        wxTimerList.Append(m_id, this);

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
    if ( m_id )
    {
        KillTimer(NULL, (UINT)m_id);
        wxTimerList.DeleteObject(this);
    }

    m_id = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxTimer& timer)
{
    // Avoid to process spurious timer events
    if ( timer.m_id == 0)
        return;

    if ( timer.IsOneShot() )
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
