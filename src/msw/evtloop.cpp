///////////////////////////////////////////////////////////////////////////////
// Name:        msw/evtloop.cpp
// Purpose:     implements wxEventLoop for MSW
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include "wx/except.h"
#include "wx/ptr_scpd.h"

#include "wx/msw/private.h"

#if wxUSE_THREADS
    #include "wx/thread.h"

    // define the list of MSG strutures
    WX_DECLARE_LIST(MSG, wxMsgList);

    #include "wx/listimpl.cpp"

    WX_DEFINE_LIST(wxMsgList);
#endif // wxUSE_THREADS

// ----------------------------------------------------------------------------
// helper class
// ----------------------------------------------------------------------------

// this object sets the wxEventLoop given to the ctor as the currently active
// one and unsets it in its dtor
class wxEventLoopActivator
{
public:
    wxEventLoopActivator(wxEventLoop **pActive,
                         wxEventLoop *evtLoop)
    {
        m_pActive = pActive;
        m_evtLoopOld = *pActive;
        *pActive = evtLoop;
    }

    ~wxEventLoopActivator()
    {
        // restore the previously active event loop
        *m_pActive = m_evtLoopOld;
    }

private:
    wxEventLoop *m_evtLoopOld;
    wxEventLoop **m_pActive;
};

// ============================================================================
// wxEventLoop implementation
// ============================================================================

wxEventLoop *wxEventLoopBase::ms_activeLoop = NULL;

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxEventLoop::wxEventLoop()
{
    m_shouldExit = false;
    m_exitcode = 0;
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing
// ----------------------------------------------------------------------------

void wxEventLoop::ProcessMessage(WXMSG *msg)
{
    // give us the chance to preprocess the message first
    if ( !PreProcessMessage(msg) )
    {
        // if it wasn't done, dispatch it to the corresponding window
        ::TranslateMessage(msg);
        ::DispatchMessage(msg);
    }
}

bool wxEventLoop::PreProcessMessage(WXMSG *msg)
{
    HWND hwnd = msg->hwnd;
    wxWindow *wndThis = wxGetWindowFromHWND((WXHWND)hwnd);

    // this may happen if the event occured in a standard modeless dialog (the
    // only example of which I know of is the find/replace dialog) - then call
    // IsDialogMessage() to make TAB navigation in it work
    if ( !wndThis )
    {
        // we need to find the dialog containing this control as
        // IsDialogMessage() just eats all the messages (i.e. returns true for
        // them) if we call it for the control itself
        while ( hwnd && ::GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD )
        {
            hwnd = ::GetParent(hwnd);
        }

        return hwnd && ::IsDialogMessage(hwnd, msg) != 0;
    }

#if wxUSE_TOOLTIPS
    // we must relay WM_MOUSEMOVE events to the tooltip ctrl if we want it to
    // popup the tooltip bubbles
    if ( msg->message == WM_MOUSEMOVE )
    {
        wxToolTip *tt = wndThis->GetToolTip();
        if ( tt )
        {
            tt->RelayEvent((WXMSG *)msg);
        }
    }
#endif // wxUSE_TOOLTIPS

    // allow the window to prevent certain messages from being
    // translated/processed (this is currently used by wxTextCtrl to always
    // grab Ctrl-C/V/X, even if they are also accelerators in some parent)
    if ( !wndThis->MSWShouldPreProcessMessage((WXMSG *)msg) )
    {
        return false;
    }

    // try translations first: the accelerators override everything
    wxWindow *wnd;

    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if ( wnd->MSWTranslateMessage((WXMSG *)msg))
            return true;

        // stop at first top level window, i.e. don't try to process the key
        // strokes originating in a dialog using the accelerators of the parent
        // frame - this doesn't make much sense
        if ( wnd->IsTopLevel() )
            break;
    }

    // now try the other hooks (kbd navigation is handled here)
    for ( wnd = wndThis; wnd; wnd = wnd->GetParent() )
    {
        if (wnd != wndThis) // Skip the first since wndThis->MSWProcessMessage() was called above
        {
            if ( wnd->MSWProcessMessage((WXMSG *)msg) )
                return true;
        }
        
        // Stop at first top level window (as per comment above).
        // If we don't do this, pressing ESC on a modal dialog shown as child of a modal
        // dialog with wxID_CANCEL will cause the parent dialog to be closed, for example
        if (wnd->IsTopLevel())
            break;
    }

    // no special preprocessing for this message, dispatch it normally
    return false;
}

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

bool wxEventLoop::IsRunning() const
{
    return ms_activeLoop == this;
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    // ProcessIdle() and Dispatch() below may throw so the code here should
    // be exception-safe, hence we must use local objects for all actions we
    // should undo
    wxEventLoopActivator activate(&ms_activeLoop, this);

    // we must ensure that OnExit() is called even if an exception is thrown
    // from inside Dispatch() but we must call it from Exit() in normal
    // situations because it is supposed to be called synchronously,
    // wxModalEventLoop depends on this (so we can't just use ON_BLOCK_EXIT or
    // something similar here)
#if wxUSE_EXCEPTIONS
    for ( ;; )
    {
        try
        {
#endif // wxUSE_EXCEPTIONS

            // this is the event loop itself
            for ( ;; )
            {
                #if wxUSE_THREADS
                    wxMutexGuiLeaveOrEnter();
                #endif // wxUSE_THREADS

                // generate and process idle events for as long as we don't
                // have anything else to do
                while ( !Pending() && (wxTheApp && wxTheApp->ProcessIdle()) )
                    ;

                // if the "should exit" flag is set, the loop should terminate
                // but not before processing any remaining messages so while
                // Pending() returns true, do process them
                if ( m_shouldExit )
                {
                    while ( Pending() )
                        Dispatch();

                    break;
                }

                // a message came or no more idle processing to do, sit in
                // Dispatch() waiting for the next message
                if ( !Dispatch() )
                {
                    // we got WM_QUIT
                    break;
                }
            }

#if wxUSE_EXCEPTIONS
            // exit the outer loop as well
            break;
        }
        catch ( ... )
        {
            try
            {
                if ( !wxTheApp || !wxTheApp->OnExceptionInMainLoop() )
                {
                    OnExit();
                    break;
                }
                //else: continue running the event loop
            }
            catch ( ... )
            {
                // OnException() throwed, possibly rethrowing the same
                // exception again: very good, but we still need OnExit() to
                // be called
                OnExit();
                throw;
            }
        }
    }
#endif // wxUSE_EXCEPTIONS

    return m_exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_exitcode = rc;
    m_shouldExit = true;

    OnExit();

    // all we have to do to exit from the loop is to (maybe) wake it up so that
    // it can notice that Exit() had been called
    //
    // in particular, we do *not* use PostQuitMessage() here because we're not
    // sure that WM_QUIT is going to be processed by the correct event loop: it
    // is possible that another one is started before this one has a chance to
    // process WM_QUIT
    ::PostMessage(NULL, WM_NULL, 0, 0);
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    MSG msg;
    return ::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0;
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), false, _T("can't call Dispatch() if not running") );

    MSG msg;
    BOOL rc = ::GetMessage(&msg, (HWND) NULL, 0, 0);

    if ( rc == 0 )
    {
        // got WM_QUIT
        return false;
    }

    if ( rc == -1 )
    {
        // should never happen, but let's test for it nevertheless
        wxLogLastError(wxT("GetMessage"));

        // still break from the loop
        return false;
    }

#if wxUSE_THREADS
    wxASSERT_MSG( wxThread::IsMain(),
                  wxT("only the main thread can process Windows messages") );

    static bool s_hadGuiLock = true;
    static wxMsgList s_aSavedMessages;

    // if a secondary thread owning the mutex is doing GUI calls, save all
    // messages for later processing - we can't process them right now because
    // it will lead to recursive library calls (and we're not reentrant)
    if ( !wxGuiOwnedByMainThread() )
    {
        s_hadGuiLock = false;

        // leave out WM_COMMAND messages: too dangerous, sometimes
        // the message will be processed twice
        if ( !wxIsWaitingForThread() || msg.message != WM_COMMAND )
        {
            MSG* pMsg = new MSG(msg);
            s_aSavedMessages.Append(pMsg);
        }

        return true;
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
            s_hadGuiLock = true;

            wxMsgList::compatibility_iterator node = s_aSavedMessages.GetFirst();
            while (node)
            {
                MSG* pMsg = node->GetData();
                s_aSavedMessages.Erase(node);

                ProcessMessage(pMsg);
                delete pMsg;

                node = s_aSavedMessages.GetFirst();
            }
        }
    }
#endif // wxUSE_THREADS

    ProcessMessage(&msg);

    return true;
}

