/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/timer.cpp
// Purpose:     wxTimer implementation
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "timer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TIMER

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/hashmap.h"

#include "wx/timer.h"

#include "wx/palmos/private.h"

// from utils.cpp
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

// define a hash containing all the timers: it is indexed by timer id and
// contains the corresponding timer
WX_DECLARE_HASH_MAP(unsigned long, wxTimer *, wxIntegerHash, wxIntegerEqual,
                    wxTimerMap);

static wxTimerMap g_timerMap;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void WINAPI wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxEvtHandler)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

void wxTimer::Init()
{
}

wxTimer::~wxTimer()
{
}

bool wxTimer::Start(int milliseconds, bool oneShot)
{
    return false;
}

void wxTimer::Stop()
{
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxTimer& timer)
{
}

void WINAPI wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
}

#endif // wxUSE_TIMER

