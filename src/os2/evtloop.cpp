///////////////////////////////////////////////////////////////////////////////
// Name:        os2/evtloop.cpp
// Purpose:     implements wxEventLoop for PM
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "evtloop.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"
#include "wx/tooltip.h"

#include "wx/os2/private.h"

#if wxUSE_THREADS
    // define the array of QMSG strutures
    WX_DECLARE_OBJARRAY(QMSG, wxMsgArray);
    // VS: this is a bit dirty - it duplicates same declaration in app.cpp
    //     (and there's no WX_DEFINE_OBJARRAY for that reason - it is already
    //     defined in app.cpp).
#endif

extern HAB vHabMain;

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl() { SetExitCode(0); }

    // process a message
    void ProcessMessage(QMSG *msg);

    // generate an idle message, return TRUE if more idle time requested
    bool SendIdleMessage();

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

private:
    // preprocess a message, return TRUE if processed (i.e. no further
    // dispatching required)
    bool PreProcessMessage(QMSG *msg);

    // the exit code of the event loop
    int m_exitcode;
};

// ============================================================================
// wxEventLoopImpl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoopImpl message processing
// ----------------------------------------------------------------------------

void wxEventLoopImpl::ProcessMessage(QMSG *msg)
{
    // give us the chance to preprocess the message first
    if ( !PreProcessMessage(msg) )
    {
        // if it wasn't done, dispatch it to the corresponding window
        ::WinDispatchMsg(vHabMain, msg);
    }
}

bool wxEventLoopImpl::PreProcessMessage(QMSG *msg)
{
    HWND hWnd = msg->hwnd;
    wxWindow *wndThis = wxFindWinFromHandle((WXHWND)hWnd);

#if wxUSE_TOOLTIPS
    // we must relay WM_MOUSEMOVE events to the tooltip ctrl if we want it to
    // popup the tooltip bubbles
    if ( wndThis && (msg->message == WM_MOUSEMOVE) )
    {
        wxToolTip *tt = wndThis->GetToolTip();
        if ( tt )
        {
            tt->RelayEvent((WXMSG *)msg);
        }
    }
#endif // wxUSE_TOOLTIPS

    // try translations first; find the youngest window with a translation
    // table.
    wxWindow *wnd;
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->OS2TranslateMessage((WXMSG *)msg) )
            return TRUE;
    }

    // Anyone for a non-translation message? Try youngest descendants first.
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->OS2ProcessMessage((WXMSG *)msg) )
            return TRUE;
    }

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxEventLoopImpl idle event processing
// ----------------------------------------------------------------------------

bool wxEventLoopImpl::SendIdleMessage()
{
    wxIdleEvent event;

    bool processed = wxTheApp->ProcessEvent(event) ;

    wxUpdateUIEvent::ResetUpdateTime();    
    
    return processed && event.MoreRequested();
}

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxEventLoop *wxEventLoop::ms_activeLoop = NULL;

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

bool wxEventLoop::IsRunning() const
{
    return m_impl != NULL;
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    m_impl = new wxEventLoopImpl;

    wxEventLoop *oldLoop = ms_activeLoop;
    ms_activeLoop = this;

    for ( ;; )
    {
#if wxUSE_THREADS
        wxMutexGuiLeaveOrEnter();
#endif // wxUSE_THREADS

        // generate and process idle events for as long as we don't have
        // anything else to do
        while ( !Pending() && m_impl->SendIdleMessage() )
            ;

        // a message came or no more idle processing to do, sit in Dispatch()
        // waiting for the next message
        if ( !Dispatch() )
        {
            // we got WM_QUIT
            break;
        }
    }

    int exitcode = m_impl->GetExitCode();
    delete m_impl;
    m_impl = NULL;

    ms_activeLoop = oldLoop;

    return exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_impl->SetExitCode(rc);

    ::WinPostMsg(NULL, WM_QUIT, 0, 0);
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    QMSG msg;
    return ::WinPeekMsg(vHabMain, &msg, 0, 0, 0, PM_NOREMOVE) != 0;
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), FALSE, _T("can't call Dispatch() if not running") );

    QMSG msg;
    BOOL rc = ::WinGetMsg(vHabMain, &msg, (HWND) NULL, 0, 0);

    if ( rc == 0 )
    {
        // got WM_QUIT
        return FALSE;
    }

    if ( rc == -1 )
    {
        // should never happen, but let's test for it nevertheless
        wxLogLastError(wxT("GetMessage"));

        // still break from the loop
        return FALSE;
    }

#if wxUSE_THREADS
    wxASSERT_MSG( wxThread::IsMain(),
                  wxT("only the main thread can process Windows messages") );

    static bool s_hadGuiLock = TRUE;
    static wxMsgArray s_aSavedMessages;

    // if a secondary thread owning the mutex is doing GUI calls, save all
    // messages for later processing - we can't process them right now because
    // it will lead to recursive library calls (and we're not reentrant)
    if ( !wxGuiOwnedByMainThread() )
    {
        s_hadGuiLock = FALSE;

        // leave out WM_COMMAND messages: too dangerous, sometimes
        // the message will be processed twice
        if ( !wxIsWaitingForThread() || msg.message != WM_COMMAND )
        {
            s_aSavedMessages.Add(msg);
        }

        return TRUE;
    }
    else
    {
        // have we just regained the GUI lock? if so, post all of the saved
        // messages
        //
        // FIXME of course, it's not _exactly_ the same as processing the
        //       messages normally - expect some things to break...
        if ( !s_hadGuiLock )
        {
            s_hadGuiLock = TRUE;

            size_t count = s_aSavedMessages.Count();
            for ( size_t n = 0; n < count; n++ )
            {
                MSG& msg = s_aSavedMessages[n];
                m_impl->ProcessMessage(&msg);
            }

            s_aSavedMessages.Empty();
        }
    }
#endif // wxUSE_THREADS

    m_impl->ProcessMessage(&msg);

    return TRUE;
}

