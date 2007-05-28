/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/trackingrectmanager.h
// Purpose:     wxCocoaTrackingRectManager
// Notes:       Source in window.mm
// Author:      David Elliott <dfe@cox.net>
// Modified by:
// Created:     2007/05/02
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Software 2000 Ltd.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __WX_COCOA_TRACKINGRECTMANAGER_H__
#define __WX_COCOA_TRACKINGRECTMANAGER_H__

#include <CoreFoundation/CFRunLoop.h>

class wxCocoaTrackingRectManager
{
    DECLARE_NO_COPY_CLASS(wxCocoaTrackingRectManager)
public:
    wxCocoaTrackingRectManager(wxWindow *window);
    void ClearTrackingRect();
    void BuildTrackingRect();
    void RebuildTrackingRect();
    bool IsOwnerOfEvent(NSEvent *anEvent);
    ~wxCocoaTrackingRectManager();
    void BeginSynthesizingEvents();
    void StopSynthesizingEvents();
protected:
    wxWindow *m_window;
    bool m_isTrackingRectActive;
    int m_trackingRectTag;
    CFRunLoopObserverRef m_runLoopObserver;
private:
    wxCocoaTrackingRectManager();
};

#endif // ndef __WX_COCOA_TRACKINGRECTMANAGER_H__
