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

// maps event loop sources to gtk source ids
//
// TODO maybe there should be only one map for sources, storing different
// information for different implementations
WX_DECLARE_HASH_MAP(wxEventLoopSource, int,
              wxEventLoopSource::Hash, wxEventLoopSource::Equal,
              wxEventLoopSourceIdMap);

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopBase
{
public:
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

protected:
    // adding/removing sources
    virtual bool DoAddSource(const wxEventLoopSource& source,
                                wxEventLoopSourceHandler* handler, int flags);
    virtual bool DoRemoveSource(const wxEventLoopSource& source);

    // the exit code of this event loop
    int m_exitcode;

    // used to temporarily store events in DoYield()
    wxArrayPtrVoid m_arrGdkEvents;

    // map of event loop sources gtk ids
    wxEventLoopSourceIdMap m_sourceIdMap;

    wxDECLARE_NO_COPY_CLASS(wxGUIEventLoop);
};

#endif // _WX_GTK_EVTLOOP_H_
