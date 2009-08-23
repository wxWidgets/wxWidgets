///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/evtloop.h
// Purpose:     wxGTK event loop implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-27
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_EVTLOOP_H_
#define _WX_GTK_EVTLOOP_H_

// ----------------------------------------------------------------------------
// wxGUIEventLoop for wxGTK
// ----------------------------------------------------------------------------

typedef union  _GdkEvent        GdkEvent;

#if wxUSE_EVENTLOOP_SOURCE
// maps event loop sources to gtk source ids
WX_DECLARE_HASH_MAP(wxUnixEventLoopSource*, int, wxPointerHash, wxPointerEqual,
                    wxEventLoopSourceIdMap);
#endif

class WXDLLIMPEXP_BASE wxGUIEventLoop : public wxEventLoopBase
{
public:
    typedef wxUnixEventLoopSource Source;

    wxGUIEventLoop();

    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();
    virtual int DispatchTimeout(unsigned long timeout);
    virtual void WakeUp();
    virtual bool YieldFor(long eventsToProcess);

    void StoreGdkEventForLaterProcessing(GdkEvent* ev)
        { m_arrGdkEvents.Add(ev); }

#if wxUSE_EVENTLOOP_SOURCE
    virtual wxUnixEventLoopSource* CreateSource() const
    {
        return new wxUnixEventLoopSource();
    }

    virtual wxUnixEventLoopSource* CreateSource(int res,
                                           wxEventLoopSourceHandler* handler,
                                           int flags) const
    {
        return new wxUnixEventLoopSource(res, handler, flags);
    }
#endif

protected:
#if wxUSE_EVENTLOOP_SOURCE
    // adding/removing sources
    virtual bool DoAddSource(wxAbstractEventLoopSource* source);
    virtual bool DoRemoveSource(wxAbstractEventLoopSource* source);

    // map of event loop sources gtk ids
    wxEventLoopSourceIdMap m_sourceIdMap;
#endif

    // the exit code of this event loop
    int m_exitcode;

    // used to temporarily store events in DoYield()
    wxArrayPtrVoid m_arrGdkEvents;

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_GTK_EVTLOOP_H_
