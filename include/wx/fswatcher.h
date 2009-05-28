/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fswatcher.h
// Purpose:     wxFileSystemWatcherBase
// Author:      Bartosz Bekier
// Created:     2009-05-23
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FSWATCHER_BASE_H_
#define _WX_FSWATCHER_BASE_H_

#include "wx/defs.h"

#if wxUSE_FSWATCHER

#include "wx/event.h"

/**
    Main entry point for clients interested in file system events.
    Defines interface that can be used to receive that kind of events.
 */
class WXDLLIMPEXP_BASE wxFileSystemWatcherBase: public wxEvtHandler
{
public:

};


// include the platform specific file defining wxFileSystemWatcher
// inheriting from wxFileSystemWatcherBase
#ifdef HAVE_INOTIFY
    #include "wx/unix/fswatcher.h"
    #define wxFileSystemWatcher wxInotifyFileSystemWatcher
#elif defined(__WXMSW__)
    #include "wx/msw/fswatcher.h"
    #define wxFileSystemWatcher wxMswFileSystemWatcher
#elif defined(__WXCOCOA__)
    #include "wx/msw/fswatcher.h"
    #define wxFileSystemWatcher wxKqueueFileSystemWatcher
#else
    #include "wx/generic/fswatcher.h"
    #define wxFileSystemWatcher wxPollingFileSystemWatcher
#endif

#endif // wxUSE_FSWATCHER

#endif /* _WX_FSWATCHER_BASE_H_ */
