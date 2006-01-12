///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/evtloop.h
// Purpose:     declaration of wxEventLoop for wxMac
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAC_CARBON_EVTLOOP_H_
#define _WX_MAC_CARBON_EVTLOOP_H_

// set wxMAC_USE_RUN_APP_EVENT_LOOP to 1 if the standard
// RunApplicationEventLoop function should be used, otherwise
// the lower level CarbonEventLoop will be used
//
// in the long run we should make this 1 by default but we will have to clean
// up event handling to make sure we don't miss handling of things like pending
// events etc and perhaps we will also have to pipe events through an
// ueber-event-handler to make sure we have one place to do all these
// house-keeping functions
#define wxMAC_USE_RUN_APP_EVENT_LOOP 0

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

#if wxMAC_USE_RUN_APP_EVENT_LOOP

class WXDLLEXPORT wxEventLoop : public wxEventLoopBase
{
public:
    wxEventLoop() { m_exitcode = 0; }

    // implement base class pure virtuals
    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();

private:
    int m_exitcode;
};

#else // manual event loop

class WXDLLEXPORT wxEventLoop : public wxEventLoopManual
{
public:
    wxEventLoop() { }

    virtual bool Pending() const;
    virtual bool Dispatch();

protected:
    // implement base class pure virtual
    virtual void WakeUp();
};

#endif // auto/manual event loop

#endif // _WX_MAC_CARBON_EVTLOOP_H_

