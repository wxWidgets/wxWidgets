/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/fswatcher.h
// Purpose:     wxInotifyFileSystemWatcher
// Author:      Bartosz Bekier
// Created:     2009-05-26
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FSWATCHER_UNIX_H_
#define _WX_FSWATCHER_UNIX_H_

#include "wx/defs.h"

#if wxUSE_FSWATCHER

#include "wx/evtloop.h"

class WXDLLIMPEXP_BASE wxInotifyFileSystemWatcher :
        public wxFileSystemWatcherBase
{
public:
    wxInotifyFileSystemWatcher();

    wxInotifyFileSystemWatcher(const wxFileName& path,
                                int events = wxFSW_EVENT_ALL);

    virtual ~wxInotifyFileSystemWatcher();

    virtual bool AddTree(const wxFileName& path, int events = wxFSW_EVENT_ALL,
                         const wxString& filter = wxEmptyString);

protected:
    virtual bool Init();

    virtual wxFSWatchEntry* CreateWatch(const wxFileName& path, int events);

    virtual bool DoAdd(const wxFSWatchEntry& watch);

    virtual bool DoRemove(const wxFSWatchEntry& watch);
};

#endif

#endif /* _WX_FSWATCHER_UNIX_H_ */
