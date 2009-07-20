/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/fswatcher.h
// Purpose:     wxMSWFileSystemWatcher
// Author:      Bartosz Bekier
// Created:     2009-05-26
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FSWATCHER_MSW_H_
#define _WX_FSWATCHER_MSW_H_

#include "wx/defs.h"

#if wxUSE_FSWATCHER

class wxFSWatcherService;

class WXDLLIMPEXP_BASE wxMSWFileSystemWatcher : public wxFileSystemWatcherBase
{
public:
    wxMSWFileSystemWatcher();

    wxMSWFileSystemWatcher(const wxFileName& path,
                                    int events = wxFSW_EVENT_ALL);

    virtual ~wxMSWFileSystemWatcher();

    virtual bool AddTree(const wxFileName& path, int events = wxFSW_EVENT_ALL,
                         const wxString& filter = wxEmptyString);

protected:
    bool Init();

    virtual wxFSWatchEntry* CreateWatch(const wxFileName& path, int events);

    virtual bool DoAdd(wxFSWatchEntry& watch);

    virtual bool DoRemove(wxFSWatchEntry& watch);

    wxFSWatcherService* m_service;
};

#endif // wxUSE_FSWATCHER

#endif /* _WX_FSWATCHER_MSW_H_ */
