///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/evtloop.h
// Purpose:     wxEventLoop class for MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-07-31
// RCS-ID:      $Id$
// Copyright:   (c) 2003-2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_EVTLOOP_H_
#define _WX_MSW_EVTLOOP_H_

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoop : public wxEventLoopBase
{
public:
    wxEventLoop();

    // implement base class pure virtuals
    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual bool IsRunning() const;

    // MSW-specific methods
    // --------------------

    // preprocess a message, return true if processed (i.e. no further
    // dispatching required)
    virtual bool PreProcessMessage(WXMSG *msg);

    // process a single message
    virtual void ProcessMessage(WXMSG *msg);

protected:
    // should we exit the loop?
    bool m_shouldExit;

    // the loop exit code
    int m_exitcode;
};

#endif // _WX_MSW_EVTLOOP_H_

