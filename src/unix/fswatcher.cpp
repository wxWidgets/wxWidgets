/////////////////////////////////////////////////////////////////////////////
// Name:        unix/fswatcher.cpp
// Purpose:     wxInotifyFileSystemWatcher
// Author:      Bartosz Bekier
// Created:     2009-05-26
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Bartosz Bekier <bartosz.bekier@gmail.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FSWATCHER

#include <sys/inotify.h>
#include <unistd.h>
#include "wx/fswatcher.h"


// ============================================================================
// wxFSWatcherEntry implementation
// ============================================================================

class WXDLLIMPEXP_BASE wxFSWatchEntry
{
public:
    wxFSWatchEntry(const wxFileName& path, int events) :
        m_path(path), m_events(events)
    { }

    // default copy ctor, assignment operator and dtor are ok

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

// ============================================================================
// wxFSWatcherService implementation & helper wxFSWSourceHandler class
// ============================================================================

class wxFSWatcherService;

/**
 * Handler for reading from inotify descriptor
 */
class wxFSWSourceHandler : public wxEventLoopSourceHandler
{
public:
    wxFSWSourceHandler(wxFSWatcherService* service) :
        m_service(service)
    {  }

    virtual void OnReadWaiting();
    virtual void OnWriteWaiting();
    virtual void OnExceptionWaiting();

protected:
    wxFSWatcherService* m_service;
};


// inotify watch descriptor=>wxFSWatchEntry* map
WX_DECLARE_HASH_MAP(int, wxFSWatchEntry*, wxIntegerHash, wxIntegerEqual,
                                                   wxFSWatchEntryDescriptors);

/**
 * Helper class encapsulating inotify mechanism
 */
class wxFSWatcherService
{
public:
    wxFSWatcherService(wxFileSystemWatcherBase* watcher) :
        m_watcher(watcher)
    {
        m_handler = new wxFSWSourceHandler(this);
    }

    ~wxFSWatcherService()
    {
        delete m_handler;
    }

    bool Init()
    {
        int fd = inotify_init();
        if (fd == -1)
        {
            wxLogSysError(_("Unable to create inotify instance"));
            m_source.SetResource(wxEventLoopSource::INVALID_RESOURCE);
            return false;
        }

        m_source.SetResource(fd);
        wxEventLoopBase* loop = wxEventLoopBase::GetActive();
        wxCHECK_MSG( loop, false,
                        "wxFileSystemWatcher requires a running event loop" );

        return loop->AddSource(m_source.GetResource(), m_handler,
                            wxEVENT_SOURCE_INPUT | wxEVENT_SOURCE_EXCEPTION);
    }

    bool Close()
    {
        wxCHECK_MSG( m_source.IsOk(), false,
                    "Inotify not initialized or invalid inotify descriptor" );

        wxEventLoopBase* loop = wxEventLoopBase::GetActive();
        wxASSERT(loop);

        // ignore errors here
        loop->RemoveSource(m_source.GetResource());

        int ret = close(m_source.GetResource());
        if (ret == -1)
        {
            wxLogSysError(_("Unable to close inotify instance"));
        }

        return ret != -1;
    }

    bool Add(const wxFSWatchEntry* watch)
    {
        wxCHECK_MSG( m_source.IsOk(), false,
                    "Inotify not initialized or invalid inotify descriptor" );

        int flags = Watcher2NativeFlags(watch->GetFlags());
        int wd = inotify_add_watch(m_source.GetResource(),
                               watch->GetPath().GetFullPath().fn_str(), flags);
        if (wd == -1)
        {
            wxLogSysError(_("Unable to add inotify watch"));
            return false;
        }

        wxFSWatchEntryDescriptors::value_type val(wd, watch);
        if (!m_watchMap.insert(val).second)
        {
            wxFAIL_MSG( wxString::Format("Path %s is already watched",
                                            watch->GetPath().GetFullPath()) );
        }

        return true;
    }

    bool Remove(const wxFSWatchEntry* watch)
    {
        wxCHECK_MSG( m_source.IsOk(), false,
                    "Inotify not initialized or invalid inotify descriptor" );

        int wd = inotify_rm_watch(m_source.GetResource(), watch->GetWatchFD());
        if (wd == -1)
        {
            wxLogSysError(_("Unable to remove inotify watch"));
            return false;
        }

        if (m_watchMap.erase(wd) != 1)
        {
            wxFAIL_MSG( wxString::Format("Path %s is not watched",
                                            watch->GetPath().GetFullPath()) );
        }

        return true;
    }

    int ReadEvents()
    {
        wxCHECK_MSG( m_source.IsOk(), false,
                    "Inotify not initialized or invalid inotify descriptor" );

        // read events
        // TODO differentiate depending on params
        char buf[128 * sizeof(inotify_event)];
        int left = ReadEventsToBuf(buf, sizeof(buf));
        if (left == -1)
            return -1;

        // len > 0, we have events
        char* memory = buf;
        int event_count = 0;
        while (left > 0) // OPT checking 'memory' would suffice
        {
            event_count++;
            inotify_event* e = (inotify_event*)memory;

            // process one inotify_event
            ProcessNativeEvent(*e);

            int offset = sizeof(inotify_event) + e->len;
            left -= offset;
            memory += offset;
        }

        wxLogTrace(wxTRACE_FSWATCHER, "We had %d events", event_count);
        return event_count;
    }

    bool IsOk()
    {
        return m_source.IsOk();
    }

    wxEventLoopSource GetSource() const
    {
        return m_source;
    }

protected:
    void ProcessNativeEvent(const inotify_event& e)
    {
        wxLogTrace(wxTRACE_FSWATCHER, "Event: wd=%d, mask=%u, len=%u, name=%s",
    											e.wd, e.mask, e.len, e.name);

        // get watch entry for this event
        wxFSWatchEntryDescriptors::iterator it = m_watchMap.find(e.wd);
        wxASSERT_MSG(it != m_watchMap.end(),
    								  "Event for wd not on the watch list!");
        wxFSWatchEntry& w = *(it->second);

        // TODO we can have multiple flags set, possibly resulting in
        // multiple events to send
        int flags = Native2WatcherFlags(e.mask);
        if (flags & wxFSW_EVENT_WARNING || flags & wxFSW_EVENT_ERROR)
        {
            wxString errMsg = GetErrorDescription(flags);
            wxFileSystemWatcherEvent event(flags, errMsg);
            SendEvent(event);
        }
        // filter out ignored events and those not asked for.
        // we never filter out warnings or exceptions
        else if ((flags == 0) || !(flags & w.GetFlags()))
        {
    		return;
        }
        else
        {
            // only when dir is watched, we have non-empty e.name
    		wxFileName path = w.GetPath();
    		if (path.IsDir())
    			path = wxFileName(path.GetPath(), e.name);

    		// TODO figure out renames: proper paths!
    		wxFileSystemWatcherEvent event(flags, path, path);
    		SendEvent(event);
        }
    }

    void SendEvent(wxFileSystemWatcherEvent& evt)
    {
        switch (evt.GetChangeType())
        {
        case wxFSW_EVENT_WARNING:
            m_watcher->OnWarning(evt.GetErrorDescription());
            return;
        case wxFSW_EVENT_ERROR:
            m_watcher->OnError(evt.GetErrorDescription());
            return;
        default:
            m_watcher->OnChange(evt.GetChangeType(), evt.GetPath(),
                                                        evt.GetNewPath());
            return;
        }
    }

    int ReadEventsToBuf(char* buf, int size)
    {
       wxCHECK_MSG( m_source.IsOk(), false,
    				"Inotify not initialized or invalid inotify descriptor" );

        // TODO remove memset after making sure this code works reliably
        memset(buf, 0, size);

        ssize_t left = read(m_source.GetResource(), buf, size);
        if (left == -1)
        {
            wxLogSysError(_("Unable to read from inotify descriptor"));
            return -1;
        }
        else if (left == 0)
        {
            wxLogWarning(_("EOF while reading from inotify descriptor"));
            return -1;
        }

        return left;
    }

    static int Watcher2NativeFlags(int flags)
    {
        // FIXME this needs a bit of thinking, quick impl for now
        return IN_ALL_EVENTS;
    }

    static int Native2WatcherFlags(int flags)
    {
        // FIXME remove in the future. just for reference
#if 0
    #define IN_ACCESS    0x00000001 /* File was accessed.  */
    #define IN_MODIFY    0x00000002 /* File was modified.  */
    #define IN_ATTRIB    0x00000004 /* Metadata changed.  */
    #define IN_CLOSE_WRITE   0x00000008 /* Writtable file was closed.  */
    #define IN_CLOSE_NOWRITE 0x00000010 /* Unwrittable file closed.  */
    #define IN_CLOSE     (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
    #define IN_OPEN      0x00000020 /* File was opened.  */
    #define IN_MOVED_FROM    0x00000040 /* File was moved from X.  */
    #define IN_MOVED_TO      0x00000080 /* File was moved to Y.  */
    #define IN_MOVE      (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
    #define IN_CREATE    0x00000100 /* Subfile was created.  */
    #define IN_DELETE    0x00000200 /* Subfile was deleted.  */
    #define IN_DELETE_SELF   0x00000400 /* Self was deleted.  */
    #define IN_MOVE_SELF     0x00000800 /* Self was moved.  */

    /* Events sent by the kernel.  */
    #define IN_UNMOUNT	 0x00002000	/* Backing fs was unmounted.  */
    #define IN_Q_OVERFLOW	 0x00004000	/* Event queued overflowed.  */
    #define IN_IGNORED	 0x00008000	/* File was ignored.  */
#endif

        // TODO this needs more thinking
        static const int flag_mapping[][2] = {
            { IN_ACCESS,        wxFSW_EVENT_ACCESS }, // generated during read!
            { IN_MODIFY,        wxFSW_EVENT_MODIFY },
            { IN_ATTRIB,        0 }, // ignored for now
            { IN_CLOSE_WRITE,   0 }, // ignored for now
            { IN_CLOSE_NOWRITE, 0 }, // ignored for now
            { IN_OPEN,          0 }, // ignored for now
            { IN_MOVED_FROM,    wxFSW_EVENT_RENAME }, // coercion. rename?
            { IN_MOVED_TO,      wxFSW_EVENT_RENAME }, // coercion. rename?
            { IN_CREATE,        wxFSW_EVENT_CREATE },
            { IN_DELETE,        wxFSW_EVENT_DELETE },
            { IN_DELETE_SELF,   wxFSW_EVENT_DELETE }, // hm...
            { IN_MOVE_SELF,     wxFSW_EVENT_RENAME }, // better named "move"

            { IN_UNMOUNT,       wxFSW_EVENT_ERROR  },
            { IN_Q_OVERFLOW,    wxFSW_EVENT_WARNING},

            // ignored, because this is genereted mainly by watcher::Remove()
            { IN_IGNORED,        0 }
        };

        unsigned int i=0;
        for ( ; i < WXSIZEOF(flag_mapping); ++i) {
#if 0
            wxLogDebug("mapping: %d=%d", flag_mapping[i][0],
                                            flag_mapping[i][1]);
#endif
            // TODO account for more flags set at once
            if (flags & flag_mapping[i][0])
                return flag_mapping[i][1];
        }

        // never reached
        wxFAIL_MSG(wxString::Format("Unknown inotify event mask %u", flags));
        return -1;
    }

    /**
     * Returns error description for specified inotify mask
     */
    static const wxString GetErrorDescription(int flag)
    {
        switch ( flag )
        {
        case IN_UNMOUNT:
        	return _("File system containing watched object was unmounted");
        case IN_Q_OVERFLOW:
    		return _("Event queue overflowed");
        }

        // never reached
        wxFAIL_MSG(wxString::Format("Unknown inotify event mask %u", flag));
        return wxEmptyString;
    }

    wxFileSystemWatcherBase* m_watcher;   // watcher we communicate with
    wxFSWSourceHandler* m_handler;        // handler for inotify event source
    wxEventLoopSource m_source;           // our event loop source
    wxFSWatchEntryDescriptors m_watchMap; // inotify wd=>wxFSWatchEntry* map
};


// once we get signaled to read, actuall event reading occurs
void wxFSWSourceHandler::OnReadWaiting()
{
    wxLogTrace(wxTRACE_FSWATCHER, "--- OnReadWaiting ---");
    m_service->ReadEvents();
}

void wxFSWSourceHandler::OnWriteWaiting()
{
    wxFAIL_MSG("We never write to inotify descriptor.");
}

void wxFSWSourceHandler::OnExceptionWaiting()
{
    wxFAIL_MSG("We never receive exceptions on inotify descriptor.");
}


// ============================================================================
// wxInotifyFileSystemWatcher implementation
// ============================================================================

wxInotifyFileSystemWatcher::wxInotifyFileSystemWatcher() :
    wxFileSystemWatcherBase()
{
    Init();
}

wxInotifyFileSystemWatcher::wxInotifyFileSystemWatcher(const wxFileName& path,
    	                                               int events) :
    wxFileSystemWatcherBase()
{
    Init();
    Add(path, events);
}

bool wxInotifyFileSystemWatcher::Init()
{
    m_service = new wxFSWatcherService(this);
    return m_service->Init();
}

wxInotifyFileSystemWatcher::~wxInotifyFileSystemWatcher()
{
    delete m_service;
}

// actuall adding: just delegate to wxFSWatcherService
bool wxInotifyFileSystemWatcher::DoAdd(const wxFSWatchEntry& watch)
{
    return m_service->Add(&watch);
}

bool wxInotifyFileSystemWatcher::DoRemove(const wxFSWatchEntry& watch)
{
    return m_service->Remove(&watch);
}

wxFSWatchEntry* wxInotifyFileSystemWatcher::CreateWatch(const wxFileName& path,
    													int events)
{
    return new wxFSWatchEntry(path, events);
}

bool wxInotifyFileSystemWatcher::AddTree(const wxFileName& path, int events,
    									 const wxString& filter)
{
    if (!path.DirExists())
    	return false;

    // OPT could be optimised if we stored information about relationships
    // between paths
    // TODO apply filter
    class AddTraverser : public wxDirTraverser
    {
    public:
    	AddTraverser(wxFileSystemWatcherBase* watcher, int events,
    					const wxString& filter) :
    		m_watcher(watcher), m_events(events), m_filter(filter)
    	{
    	}

    	// CHECK we choose which files to delegate to Add(), maybe we should pass
    	// all of them to Add() and let it choose? this is useful when adding a
    	// file to a dir that is already watched, then not only should we know
    	// about that, but Add() should also behave well then
    	virtual wxDirTraverseResult OnFile(const wxString& filename)
    	{
    		return wxDIR_CONTINUE;
    	}

    	virtual wxDirTraverseResult OnDir(const wxString& dirname)
    	{
        	wxLogTrace(wxTRACE_FSWATCHER, "--- AddTree adding '%s' ---",
    																dirname);
        	// we add as much as possible and ignore errors
    		m_watcher->Add(wxFileName(dirname), m_events);
    		return wxDIR_CONTINUE;
    	}

    private:
    	wxFileSystemWatcherBase* m_watcher;
    	int m_events;
    	wxString m_filter;
    };

    wxDir dir(path.GetFullPath());
    AddTraverser traverser(this, events, filter);
    dir.Traverse(traverser);

    return true;
}

#endif // wxUSE_FSWATCHER
