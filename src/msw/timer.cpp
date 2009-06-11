/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/timer.cpp
// Purpose:     wxTimer implementation
// Author:      Julian Smart
// Modified by: Vadim Zeitlin (use hash map instead of list, global rewrite)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TIMER

#include "wx/msw/private/timer.h"

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/hashmap.h"
    #include "wx/module.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

// define a hash containing all the timers: it is indexed by timer id and
// contains the corresponding timer
WX_DECLARE_HASH_MAP(unsigned long, wxMSWTimerImpl *, wxIntegerHash, wxIntegerEqual,
                    wxTimerMap);

// instead of using a global here, wrap it in a static function as otherwise it
// could have been used before being initialized if a timer object were created
// globally
static wxTimerMap& TimerMap()
{
    static wxTimerMap s_timerMap;

    return s_timerMap;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

LRESULT APIENTRY _EXPORT wxTimerWndProc(HWND hWnd, UINT message,
                                        WPARAM wParam, LPARAM lParam);

// implemented in utils.cpp
extern "C" WXDLLIMPEXP_BASE HWND
wxCreateHiddenWindow(LPCTSTR *pclassname, LPCTSTR classname, WNDPROC wndproc);


// ----------------------------------------------------------------------------
// wxTimerHiddenWindowModule: used to manage the hidden window used for
// catching timer messages (we need a module to ensure that the window is
// always deleted)
// ----------------------------------------------------------------------------

class wxTimerHiddenWindowModule : public wxModule
{
public:
    // module init/finalize
    virtual bool OnInit();
    virtual void OnExit();

    // get the hidden window (creates on demand)
    static HWND GetHWND();

private:
    // the HWND of the hidden window
    static HWND ms_hwnd;

    // the class used to create it
    static const wxChar *ms_className;

    DECLARE_DYNAMIC_CLASS(wxTimerHiddenWindowModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxTimerHiddenWindowModule, wxModule)

// ============================================================================
// implementation
// ============================================================================


// ----------------------------------------------------------------------------
// wxMSWTimerImpl class
// ----------------------------------------------------------------------------

bool wxMSWTimerImpl::Start(int milliseconds, bool oneShot)
{
    if ( !wxTimerImpl::Start(milliseconds, oneShot) )
        return false;

    m_id = ::SetTimer(
        wxTimerHiddenWindowModule::GetHWND(),  // window to send the messages to
        GetId(),                               // timer ID
        (UINT)m_milli,                         // delay
        NULL                                   // timer proc.  Not used since we pass hwnd
        );

    if ( !m_id )
    {
        wxLogSysError(_("Couldn't create a timer"));

        return false;
    }

    // check that SetTimer() didn't reuse an existing id: according to the MSDN
    // this can happen and this would be catastrophic to us as we rely on ids
    // uniquely identifying the timers because we use them as keys in the hash
    //
    // notice that this also happens if the same id is reused for multiple
    // timers: this used to work in previous versions but was never supported
    // and absolutely shouldn't be done, use wxID_ANY to assign an id to the
    // timer automatically or ensure that all your timers have unique ids
    if ( TimerMap().find(m_id) != TimerMap().end() )
    {
        wxLogError(_("Timer creation failed."));

        ::KillTimer(wxTimerHiddenWindowModule::GetHWND(), m_id);
        m_id = 0;

        return false;
    }

    TimerMap()[m_id] = this;

    return true;
}

void wxMSWTimerImpl::Stop()
{
    wxASSERT_MSG( m_id, _T("should be running") );

    ::KillTimer(wxTimerHiddenWindowModule::GetHWND(), m_id);

    TimerMap().erase(m_id);

    m_id = 0;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

void wxProcessTimer(wxMSWTimerImpl& timer)
{
    wxASSERT_MSG( timer.IsRunning(), _T("bogus timer id") );

    if ( timer.IsOneShot() )
        timer.Stop();

    timer.Notify();
}


LRESULT APIENTRY _EXPORT wxTimerWndProc(HWND hWnd, UINT message,
                                        WPARAM wParam, LPARAM lParam)
{
    if ( message == WM_TIMER )
    {
        wxTimerMap::iterator node = TimerMap().find((unsigned long)wParam);

        wxCHECK_MSG( node != TimerMap().end(), 0, wxT("bogus timer id in wxTimerProc") );

        wxProcessTimer(*(node->second));
    }
    else
    {
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ----------------------------------------------------------------------------
// wxTimerHiddenWindowModule functions
// ----------------------------------------------------------------------------


HWND wxTimerHiddenWindowModule::ms_hwnd = NULL;

const wxChar *wxTimerHiddenWindowModule::ms_className = NULL;

bool wxTimerHiddenWindowModule::OnInit()
{
    // do not initialize ms_hwnd to ms_className to NULL here: it may happen
    // that our GetHWND() is called before the modules are initialized if a
    // timer is created from wxApp-derived class ctor and in this case we
    // shouldn't overwrite it

    return true;
}

void wxTimerHiddenWindowModule::OnExit()
{
    if ( ms_hwnd )
    {
        if ( !::DestroyWindow(ms_hwnd) )
        {
            wxLogLastError(_T("DestroyWindow(wxTimerHiddenWindow)"));
        }

        ms_hwnd = NULL;
    }

    if ( ms_className )
    {
        if ( !::UnregisterClass(ms_className, wxGetInstance()) )
        {
            wxLogLastError(_T("UnregisterClass(\"wxTimerHiddenWindow\")"));
        }

        ms_className = NULL;
    }
}

/* static */
HWND wxTimerHiddenWindowModule::GetHWND()
{
    static const wxChar *HIDDEN_WINDOW_CLASS = _T("wxTimerHiddenWindow");
    if ( !ms_hwnd )
    {
        ms_hwnd = wxCreateHiddenWindow(&ms_className, HIDDEN_WINDOW_CLASS,
                                     wxTimerWndProc);
    }

    return ms_hwnd;
}

#endif // wxUSE_TIMER
