///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/evtloop.h
// Purpose:     declaration of wxGUIEventLoop for wxOSX/Cocoa
// Author:      Vadim Zeitlin
// Created:     2008-12-28
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_COCOA_EVTLOOP_H_
#define _WX_OSX_COCOA_EVTLOOP_H_

class WXDLLIMPEXP_BASE wxGUIEventLoop : public wxEventLoopManual
{
public:
    typedef wxMacEventLoopSource Source;

    wxGUIEventLoop();

    // implement/override base class pure virtual
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual int DispatchTimeout(unsigned long timeout);

    virtual void WakeUp();
    virtual bool YieldFor(long eventsToProcess);

#if wxUSE_EVENTLOOP_SOURCE
    virtual wxMacEventLoopSource* CreateSource() const
    {
        return new wxMacEventLoopSource();
    }

    virtual wxMacEventLoopSource* CreateSource(int res,
                                           wxEventLoopSourceHandler* handler,
                                           int flags) const;

    virtual wxMacEventLoopSource* CreateSource(CFRunLoopSourceRef res,
                                           wxEventLoopSourceHandler* handler,
                                           int flags) const
    {
        return new wxMacEventLoopSource(res, handler, flags);
    }
#endif

protected:
#if wxUSE_EVENTLOOP_SOURCE
    // adding/removing sources
    virtual bool DoAddSource(wxAbstractEventLoopSource* source);
    virtual bool DoRemoveSource(wxAbstractEventLoopSource* source);
#endif

private:
    double m_sleepTime;
};

#endif // _WX_OSX_COCOA_EVTLOOP_H_

