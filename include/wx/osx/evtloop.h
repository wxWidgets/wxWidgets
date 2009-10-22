///////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/osx/evtloop.h
// Purpose:     simply forwards to wx/mac/carbon/evtloop.h for consistency with
//              the other Mac headers
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2006-01-12
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_EVTLOOP_H_
#define _WX_OSX_EVTLOOP_H_

typedef struct __CFRunLoop * CFRunLoopRef;

class WXDLLIMPEXP_BASE wxCFEventLoop : public wxEventLoopManual
{
public:
#if wxUSE_EVENTLOOP_SOURCE
    virtual wxEventLoopSource *
      AddSourceForFD(int fd, wxEventLoopSourceHandler *handler, int flags);
#endif // wxUSE_EVENTLOOP_SOURCE

protected:
    // get the currently executing CFRunLoop
    virtual CFRunLoopRef CFGetCurrentRunLoop() const = 0;
};

#if wxUSE_GUI
    #ifdef __WXOSX_COCOA__
        #include "wx/osx/cocoa/evtloop.h"
    #else
        #include "wx/osx/carbon/evtloop.h"
    #endif
#endif // wxUSE_GUI

#endif // _WX_OSX_EVTLOOP_H_
