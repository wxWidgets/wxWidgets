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

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxEventLoopManual
{
public:
    wxGUIEventLoop();

    virtual bool Pending() const;
    virtual bool Dispatch();

    // implement base class pure virtual
    virtual void WakeUp();
private:
    double      m_sleepTime;
};

#endif // _WX_MAC_CARBON_EVTLOOP_H_

