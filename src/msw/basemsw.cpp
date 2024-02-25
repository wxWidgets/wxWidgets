///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/basemsw.cpp
// Purpose:     misc stuff only used in console applications under MSW
// Author:      Vadim Zeitlin
// Created:     22.06.2003
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/msw/private/timer.h"
// MBN: this is a workaround for MSVC 5: if it is not #included in
// some wxBase file, wxRecursionGuard methods won't be exported from
// wxBase.dll, and MSVC 5 will give linker errors
#include "wx/recguard.h"

#include "wx/crt.h"
#include "wx/msw/private.h"

// ============================================================================
// wxAppTraits implementation
// ============================================================================

bool wxAppTraits::SafeMessageBox(const wxString& text,
                                 const wxString& title)
{
    const HWND hwndParent = GetMainHWND();
    int flags = MB_OK | MB_ICONSTOP;

    // Using MB_TASKMODAL with valid parent doesn't work well because it
    // prevents the typical behaviour of modal message boxes, e.g. the message
    // box doesn't come up to front when the parent is clicked. But if we don't
    // have any parent anyhow, we can just as well use it, as we don't lose
    // anything and it has a useful side effect of disabling any existing TLWs
    // if there are any.
    //
    // Note that we also might have chosen to always use MB_TASKMODAL and null
    // parent. This would have the advantage of always disabling all the window
    // which, but at the cost of the behaviour mentioned above and other
    // related problems, e.g. showing ugly default icon in Alt-Tab list and an
    // extra taskbar button for the message box, so we don't do this, although
    // perhaps we still should, at least in case when there is more than one
    // TLW (but we can't check for this easily as this is non-GUI code and
    // wxTopLevelWindows is not accessible from it).
    if ( !hwndParent )
        flags |= MB_TASKMODAL;

    ::MessageBox(hwndParent, text.t_str(), title.t_str(), flags);

    return true;
}

#if wxUSE_THREADS
WXDWORD wxAppTraits::DoSimpleWaitForThread(WXHANDLE hThread)
{
    return ::WaitForSingleObject((HANDLE)hThread, INFINITE);
}
#endif // wxUSE_THREADS

// ============================================================================
// wxConsoleAppTraits implementation
// ============================================================================

void *wxConsoleAppTraits::BeforeChildWaitLoop()
{
    // nothing to do here
    return nullptr;
}

void wxConsoleAppTraits::AfterChildWaitLoop(void * WXUNUSED(data))
{
    // nothing to do here
}

#if wxUSE_THREADS
bool wxConsoleAppTraits::DoMessageFromThreadWait()
{
    // nothing to process here
    return true;
}

WXDWORD wxConsoleAppTraits::WaitForThread(WXHANDLE hThread, int WXUNUSED(flags))
{
    return DoSimpleWaitForThread(hThread);
}
#endif // wxUSE_THREADS

#if wxUSE_TIMER

wxTimerImpl *wxConsoleAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return new wxMSWTimerImpl(timer);
}

#endif // wxUSE_TIMER

// Why can't this be disabled for __WXQT__ ??? There is an implementation in src/qt/apptraits.cpp
wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
{
#if wxUSE_CONSOLE_EVENTLOOP
    return new wxEventLoop();
#else // !wxUSE_CONSOLE_EVENTLOOP
    return nullptr;
#endif // wxUSE_CONSOLE_EVENTLOOP/!wxUSE_CONSOLE_EVENTLOOP
}


bool wxConsoleAppTraits::WriteToStderr(const wxString& text)
{
    return wxFprintf(stderr, "%s", text) != -1;
}
