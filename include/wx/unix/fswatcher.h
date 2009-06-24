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

class WXDLLIMPEXP_BASE wxWatch
{
public:
    wxWatch(const wxFileName& path, int events) :
        m_path(path), m_events(events)
    { }

    // default copy ctor, assignment operator and dtor are ok :-)

    const wxFileName& GetPath() const
    {
        return m_path;
    }

    int GetFlags() const
    {
        return m_events;
    }

    int GetWatchFD() const
    {
        return m_wd;
    }

private:
    wxFileName m_path;
    int m_events;
    int m_wd;
};


class WXDLLIMPEXP_BASE wxInotifyFileSystemWatcher :
        public wxFileSystemWatcherBase
{
public:
    wxInotifyFileSystemWatcher();

    wxInotifyFileSystemWatcher(const wxFileName& path,
                                int events = wxFSW_EVENT_ALL);

    virtual ~wxInotifyFileSystemWatcher();

    virtual bool Add(const wxFileName& path, int events = wxFSW_EVENT_ALL);

    virtual bool AddTree(const wxFileName& path, int events = wxFSW_EVENT_ALL,
                         const wxString& filter = wxEmptyString);

    virtual bool Remove(const wxFileName& path);

    virtual bool RemoveTree(const wxFileName& path);

    virtual bool RemoveAll();

protected:
    virtual bool Init();

    virtual wxWatch* CreateWatch(const wxFileName& path, int events)
    {
        return new wxWatch(path, events);
    }

    virtual bool DoAdd(const wxWatch& watch);
};

#endif

#endif /* _WX_FSWATCHER_UNIX_H_ */
