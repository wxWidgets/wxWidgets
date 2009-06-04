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
#include "wx/filename.h"

// ----------------------------------------------------------------------------
// wxFileSystemWatcherEventType & wxFileSystemWatcherEvent
// ----------------------------------------------------------------------------

/**
    Possible types of file system events.
    This is a subset that will work fine an all platforms (actually, we will
    see how it works on Mac).
 */
enum wxFileSystemWatcherEventType
{
    wxFSW_EVENT_CREATE = 0x01,
    wxFSW_EVENT_DELETE = 0x02,
    wxFSW_EVENT_RENAME = 0x04,
    wxFSW_EVENT_MODIFY = 0x08,
    wxFSW_EVENT_ACCESS = 0x10,
    wxFSW_EVENT_ALL = wxFSW_EVENT_CREATE | wxFSW_EVENT_DELETE |
                      wxFSW_EVENT_RENAME | wxFSW_EVENT_MODIFY |
                      wxFSW_EVENT_ACCESS,
};

typedef int wxFileSystemWatcherEventFlags;

/**
    Event containing information about file system change.
 */
class WXDLLIMPEXP_FWD_BASE wxFileSystemWatcherEvent;
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_BASE, wxEVT_FSWATCHER,
                         wxFileSystemWatcherEvent);

class WXDLLIMPEXP_BASE wxFileSystemWatcherEvent: public wxEvent
{
public:
    wxFileSystemWatcherEvent(int watchid = wxID_ANY) :
        wxEvent(watchid, wxEVT_FSWATCHER)
    {
    }

    /**
        Returns the path at which the event occurred.
     */
    const wxFileName& GetPath() const;

    /**
        In case of rename(move?) events, returns the new path related to the
        event. The "new" means newer in the sense of time. In case of other
        events it returns the same path as GetPath().
     */
    const wxFileName& GetNewPath() const;

    /**
        Returns the type of file system event that occurred.
     */
    wxFileSystemWatcherEventType GetChangeType() const;

    virtual wxEvent* Clone() const
    {
        return new wxFileSystemWatcherEvent(*this);
    }

    virtual wxEventCategory GetEventCategory() const;
};


// ----------------------------------------------------------------------------
// wxFileSystemWatcherBase: interface for wxFileSystemWatcher
// ----------------------------------------------------------------------------

/**
    Main entry point for clients interested in file system events.
    Defines interface that can be used to receive that kind of events.
 */
class WXDLLIMPEXP_BASE wxFileSystemWatcherBase: public wxEvtHandler
{
public:
    wxFileSystemWatcherBase()
    {
        Init();
    }

    wxFileSystemWatcherBase(const wxFileName& path,
                        wxFileSystemWatcherEventFlags events = wxFSW_EVENT_ALL)
    {
        Init();
        Add(path, events);
    }

    virtual ~wxFileSystemWatcherBase()
    {
    }

    /**
        Adds path to currently watched files. Any events concerning this
        particular path will be sent to handler. Optionally a filter can be
        specified to receive only events of particular type.
     */
    virtual int Add(const wxFileName& path,
                   wxFileSystemWatcherEventFlags events = wxFSW_EVENT_ALL) = 0;

    /**
        Like above, but recursively adds every file/dir in the tree rooted in
        path. Additionally a file mask can be specified to include only files
        of particular type.
     */
    virtual int AddTree(const wxFileName& path,
                        wxFileSystemWatcherEventFlags events = wxFSW_EVENT_ALL,
                        const wxString& filter = wxEmptyString) = 0;

    /**
        Removes path from the list of watched paths.
     */
    virtual int Remove(const wxFileName& path) = 0;

    /**
        Same as above, but also removes every file belonging to the tree rooted
        at path.
     */
    virtual int RemoveTree(const wxFileName& path) = 0;

    /**
        Stops watching all paths.
     */
    virtual int RemoveAll() = 0;

protected:
    int Init();
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
