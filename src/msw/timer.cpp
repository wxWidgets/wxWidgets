/////////////////////////////////////////////////////////////////////////////
// Name:        msw/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/module.h"

#include "wx/timer.h"

#include "wx/msw/private.h"

// from utils.cpp
extern "C" HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

WX_DECLARE_HASH_MAP( long,
                     wxTimer *,
                     wxIntegerHash,
                     wxIntegerEqual,
                     wxTimerMap );

wxTimerMap wxTimerList;

void WINAPI wxTimerProc(HWND hwnd, WORD, int idTimer, DWORD);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// these variables are for timer shared hwnd management
static const wxChar *wxMSWTIMER_WNDCLASSNAME = wxT("_wxTimer_Internal_Class");
static LPCTSTR s_classnameTimerWnd = NULL;
static HWND s_hwndTimer = NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxTimerModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit()
    {
        if ( s_hwndTimer )
        {
            ::DestroyWindow(s_hwndTimer);
            s_hwndTimer = NULL;

            if ( !::UnregisterClass(wxMSWTIMER_WNDCLASSNAME, wxGetInstance()) )
            {
                wxLogLastError(_T("UnregisterClass(wxTimerClass)"));
            }

            s_classnameTimerWnd = NULL;
        }
    }

private:
    DECLARE_DYNAMIC_CLASS(wxTimerModule)
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTimer class
// ----------------------------------------------------------------------------

void wxTimer::Init()
{
    m_id = 0;
    m_hwnd = NULL;
}

wxTimer::~wxTimer()
{
    // save id as Stop() changes it
    long id = m_id;

    wxTimer::Stop();

    wxTimerList.erase(id);
}

bool wxTimer::Start(int milliseconds, bool oneShot)
{
    (void)wxTimerBase::Start(milliseconds, oneShot);

    wxCHECK_MSG( m_milli > 0, false, wxT("invalid value for timer timeour") );

    // find a window for SetTimer(): it should be a valid HWND owned by this
    // thread (even if we had a non NULL m_hwnd before, reset it in case the
    // owner has changed)
    m_hwnd = NULL;

    // first try the owner window
    if ( m_owner )
    {
        wxWindow *win = wxDynamicCast(m_owner, wxWindow);
        if ( win )
        {
            m_hwnd = win->GetHWND();
        }
    }

    // if not, use a shared hidden window
    if ( !m_hwnd )
    {
        if ( !s_hwndTimer )
        {
            s_hwndTimer = wxCreateHiddenWindow
                          (
                            &s_classnameTimerWnd,
                            wxMSWTIMER_WNDCLASSNAME,
                            ::DefWindowProc
                          );

            if ( !s_hwndTimer )
            {
                wxASSERT_MSG( s_hwndTimer, wxT("can't create a HWND for wxTimer") );
                return false;
            }
        }

        m_hwnd = (WXHWND)s_hwndTimer;

    }

    m_id = ::SetTimer
             (
                (HWND)m_hwnd,
                (UINT)(m_id ? m_id : 1),
                (UINT)m_milli,
                (TIMERPROC)wxTimerProc
             );

    if ( !m_id )
    {
        wxLogSysError(_("Couldn't create a timer"));

        return false;
    }

    wxTimerList[m_id] = this;

    return true;
}

void wxTimer::Stop()
{
    if ( m_id )
    {
        ::KillTimer((HWND)m_hwnd, (UINT)m_id);
        m_hwnd = NULL;

        wxTimerList.erase(m_id);
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

void WINAPI wxTimerProc(HWND WXUNUSED(hwnd), WORD, int idTimer, DWORD)
{
    wxTimerMap::iterator node = wxTimerList.find((long)idTimer);

    wxASSERT_MSG( node != wxTimerList.end(), wxT("bogus timer id in wxTimerProc") );

    wxProcessTimer(*(node->second));
}

#endif // wxUSE_TIMER

