///////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/evtloop.mm
// Purpose:     implements wxEventLoop for Cocoa
// Author:      David Elliott
// Modified by:
// Created:     2003/10/02
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott <dfe@cox.net>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"

#import <AppKit/NSApplication.h>

// ========================================================================
// wxEventLoopImpl
// ========================================================================

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl() { SetExitCode(0); }

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }

private:
    // the exit code of the event loop
    int m_exitcode;
};

// ========================================================================
// wxEventLoop
// ========================================================================

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop *wxEventLoop::ms_activeLoop = NULL;

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

bool wxEventLoop::IsRunning() const
{
    return m_impl;
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    wxEventLoop *oldLoop = ms_activeLoop;
    ms_activeLoop = this;

    m_impl = new wxEventLoopImpl;

    [[NSApplication sharedApplication] run];

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

    NSApplication *cocoaApp = [NSApplication sharedApplication];
    wxLogDebug("wxEventLoop::Exit isRunning=%d", (int)[cocoaApp isRunning]);
    [cocoaApp stop: cocoaApp];
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    return 0;
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), false, _T("can't call Dispatch() if not running") );
    return false;
}

