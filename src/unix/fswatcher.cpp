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
// wxWatcherService implementation & helper wxSourceIOHandler class
// ============================================================================

class wxWatcherService;

/**
 * Handler for reading from intoify descriptor
 */
class wxSourceIOHandler : public wxEventLoopSourceHandler
{
public:
    wxSourceIOHandler(wxFSWatcherService* service) :
        m_service(service)
    {  }

    virtual void OnReadWaiting();
    virtual void OnWriteWaiting();
    virtual void OnExceptionWaiting();

protected:
    wxFSWatcherService* m_service;
};


// watch descriptor=>wxFSWatchEntry* map
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
        m_handler = new wxSourceIOHandler(this);
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

        // TODO extract method
        wxFSWatchEntryDescriptors::value_type val(wd, watch);

        if (!m_watchMap.insert(val).second)
        {
            wxFAIL_MSG( wxString::Format("Path %s is already watched",
                                            watch->GetPath().GetFullPath()) );
        }

        return true;
    }

    bool Remove(wxFSWatchEntry* watch)
    {
        wxCHECK_MSG( m_source.IsOk(), false,
                    "Inotify not initialized or invalid inotify descriptor" );

        int wd = inotify_rm_watch(m_source.GetResource(),
                                    watch->GetWatchFD());
        if (-1 == wd)
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

        // TODO remove memset after making sure this code works reliably
        memset(buf, 0, sizeof(buf));

        ssize_t left = read(m_source.GetResource(), &buf, sizeof(buf));
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

        // assert len > 0, we have events
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

        wxLogDebug("We had %d events", event_count);
        return event_count;
    }

    // CHECK which error when? we have no case of this OnExceptionWaiting
    void OnError()
    {
        wxFileSystemWatcherEvent event(wxFSW_EVENT_ERROR);
        SendEvent(event);
    }

    bool IsOk()
    {
        return m_source.IsOk();
    }

    wxEventLoopSource GetSource() const
    {
        return m_source;
    }

    static int Watcher2NativeFlags(int flags)
    {
        // FIXME this needs a bit of thinking, quick impl for now
        return IN_ALL_EVENTS;
    }

    static int Native2WatcherFlags(int flags)
    {
        // FIXME remove. just for reference
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
#endif

        // TODO this needs more thinking
        static const int flag_mapping[][2] = {
            { IN_ACCESS,        wxFSW_EVENT_ACCESS },
            { IN_MODIFY,        wxFSW_EVENT_MODIFY },
            { IN_ATTRIB,        0 }, // ignored for now
            { IN_CLOSE_WRITE,   0 }, // ignored for now
            { IN_CLOSE_NOWRITE, 0 }, // ignored for now
            { IN_OPEN,          0 }, // ignored for now
            { IN_MOVED_FROM,    wxFSW_EVENT_RENAME }, // coercion. rename?
            { IN_MOVED_TO,      wxFSW_EVENT_DELETE }, // coercion. rename?
            { IN_CREATE,        wxFSW_EVENT_CREATE },
            { IN_DELETE,        wxFSW_EVENT_DELETE },
            { IN_DELETE_SELF,   wxFSW_EVENT_DELETE }, // hm...
            { IN_MOVE_SELF,     wxFSW_EVENT_RENAME }, // better named "move"
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
        return -1;
    }

protected:
    void ProcessNativeEvent(const inotify_event& e)
    {
        wxLogDebug("Event: wd=%d, mask=%u, len=%u, name=%s",
                                            e.wd, e.mask, e.len, e.name);

        wxFSWatchEntryDescriptors::iterator it = m_watchMap.find(e.wd);
        wxASSERT_MSG(it != m_watchMap.end(),
                                    "Event for wd not on the watch list!");
        wxFSWatchEntry& w = *(it->second);

        // TODO we can have multiple flags set, possibly resulting in
        // multiple events to send
        int flags = Native2WatcherFlags(e.mask);

        // TODO we just temporarly ignore some kinds of events
        if (flags == 0)
            return;

        // TODO filtering events to contain only those asked for

        // only when dir is watched, we have non-empty e.name
        wxFileName path = w.GetPath();
        if (path.IsDir())
            path = wxFileName(path.GetPath(), e.name);

        wxLogDebug("path=%s", path.GetFullPath());

        // TODO figure out renames: proper paths!
        wxFileSystemWatcherEvent event(flags, path, path);
        SendEvent(event);
    }

    void SendEvent(wxFileSystemWatcherEvent& evt)
    {
        switch (evt.GetChangeType())
        {
        case wxFSW_EVENT_WARNING:
            m_watcher->OnWarning(_("Warning"));
            return;
        case wxFSW_EVENT_ERROR:
            m_watcher->OnError(_("Error"));
            return;
        default:
            m_watcher->OnChange(evt.GetChangeType(), evt.GetPath(),
                                                        evt.GetNewPath());
            return;
        }
    }

    wxFileSystemWatcherBase* m_watcher; // reference to watcher we communicate with
    wxSourceIOHandler* m_handler;       // handler for our inotify event loop source
    wxEventLoopSource m_source;         // our event loop source
    wxFSWatchEntryDescriptors m_watchMap;    // inotify watch descriptor=>wxFSWatchEntry* map
};


// once we get signaled to read, actuall event reading occurs
void wxSourceIOHandler::OnReadWaiting()
{
    wxLogTrace(wxTRACE_FSWATCHER, "--- OnReadWaiting ---");
    m_service->ReadEvents();
}

void wxSourceIOHandler::OnWriteWaiting()
{
    wxFAIL_MSG("We never write to inotify descriptor.");
}

// TODO how to read/pass information about the error/warning?
void wxSourceIOHandler::OnExceptionWaiting()
{
    wxLogTrace(wxTRACE_FSWATCHER, "--- OnExceptionWaiting ---");
    m_service->OnError();
}


// ============================================================================
// wxInotifyFileSystemWatcher implementation
// ============================================================================

wxInotifyFileSystemWatcher::wxInotifyFileSystemWatcher() :
    wxFileSystemWatcherBase()
{
    Init();
}

wxInotifyFileSystemWatcher::wxInotifyFileSystemWatcher(
                                        const wxFileName& path, int events) :
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

wxFSWatchEntry* wxInotifyFileSystemWatcher::CreateWatch(const wxFileName& path,
                                                                    int events)
{
    return new wxFSWatchEntry(path, events);
}

// TODO if nothing more comes in here, this can be simple removed
bool wxInotifyFileSystemWatcher::Add(const wxFileName& path, int events)
{
    return wxFileSystemWatcherBase::Add(path, events);
}

bool wxInotifyFileSystemWatcher::AddTree(const wxFileName& path,
                        int events,
                        const wxString& filter)
{
    wxFAIL_MSG("not implemented");
    return true;
}

bool wxInotifyFileSystemWatcher::Remove(const wxFileName& path)
{
    wxFAIL_MSG("not implemented");
    return true;
}

bool wxInotifyFileSystemWatcher::RemoveTree(const wxFileName& path)
{
    wxFAIL_MSG("not implemented");
    return true;
}

bool wxInotifyFileSystemWatcher::RemoveAll()
{
    wxFAIL_MSG("not implemented");
    return true;
}

#endif // wxUSE_FSWATCHER
