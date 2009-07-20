/////////////////////////////////////////////////////////////////////////////
// Name:        msw/fswatcher.cpp
// Purpose:     wxMSWFileSystemWatcher
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

#include "wx/fswatcher.h"
#include "wx/thread.h"
#include "wx/msw/private.h"

// ============================================================================
// wxFSWatcherEntry implementation
// ============================================================================

class WXDLLIMPEXP_BASE wxFSWatchEntry
{
public:
    enum
    {
        BUFFER_SIZE = 4096   // TODO parametrize
    };

    wxFSWatchEntry(const wxFileName& path, int events) :
        m_path(path), m_events(events)
    {
        // get handle for this path
        m_handle = OpenDir(m_path);
        m_overlapped = (OVERLAPPED*)calloc(1, sizeof(OVERLAPPED));
		wxZeroMemory(m_buffer);
    }

    ~wxFSWatchEntry()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            if (!CloseHandle(m_handle))
            {
                wxLogSysError(_("Unable to close the handle for '%s'"),
                                m_path.GetFullPath());
            }
        }
        delete m_overlapped;
    }

    bool IsOk() const
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    const wxFileName& GetPath() const
    {
        return m_path;
    }

    int GetFlags() const
    {
        return m_events;
    }

    HANDLE GetHandle() const
    {
        return m_handle;
    }

    void SetHandle(HANDLE handle)
    {
        m_handle = handle;
    }

    void* GetBuffer()
    {
        return m_buffer;
    }

    OVERLAPPED* GetOverlapped() const
    {
        return m_overlapped;
    }

private:
    // opens dir with all flags, attributes etc. necessary to be later
    // asynchronous watched with ReadDirectoryChangesW
    static HANDLE OpenDir(const wxFileName& filename)
    {
        wxString path = filename.GetFullPath();
        HANDLE handle = CreateFile(path, FILE_LIST_DIRECTORY,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE |
                                   FILE_SHARE_DELETE,
                                   NULL, OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS |
                                   FILE_FLAG_OVERLAPPED,
                                   NULL);
        if (handle == INVALID_HANDLE_VALUE)
        {
            wxLogSysError(_("Unable to open dir '%s'"), path);
        }

        return handle;
    }

    wxFileName m_path;
    int m_events;

    HANDLE m_handle;
    char m_buffer[BUFFER_SIZE];
    OVERLAPPED* m_overlapped;

    DECLARE_NO_COPY_CLASS(wxFSWatchEntry);
};


// ============================================================================
// wxFSWatcherService implementation
// ============================================================================

// open dir HANDLE => wxFSWatchEntry* map
WX_DECLARE_VOIDPTR_HASH_MAP(wxFSWatchEntry*, wxFSWatchEntryHandles);

class wxIOCPService
{
public:
    wxIOCPService() :
        m_iocp(INVALID_HANDLE_VALUE)
    {
        Init();
    }

    ~wxIOCPService()
    {
        if (m_iocp != INVALID_HANDLE_VALUE)
        {
            if (!CloseHandle(m_iocp))
            {
                wxLogSysError(_("Unable to close I/O completion port handle"));
            }
        }
    }

    // associates a handle with completion port
    bool Add(HANDLE handle, void* completionKey)
    {
        wxCHECK_MSG( m_iocp != INVALID_HANDLE_VALUE, false, "IOCP not init" );
        wxCHECK_MSG( handle != INVALID_HANDLE_VALUE, false, "Invalid handle" );

        wxLogTrace(wxTRACE_FSWATCHER, "Adding handle=%p", handle);

        // associate with IOCP
        HANDLE ret = CreateIoCompletionPort(handle, m_iocp, (ULONG_PTR)completionKey, 0);
        if (ret == NULL)
        {
            wxLogSysError(_("Unable to associate handle with "
                            "I/O completion port"));
            return false;
        }
        else if (ret != m_iocp)
        {
            wxFAIL_MSG(_("Unexptedly new I/O completion port was created"));
            return false;
        }

        return true;
    }

    // post completion packet
    bool PostEmptyStatus()
    {
        wxCHECK_MSG( m_iocp != INVALID_HANDLE_VALUE, false, "IOCP not init" );
        int ret = PostQueuedCompletionStatus(m_iocp, 0, NULL, NULL);
        if (!ret)
        {
            wxLogSysError(_("Unable to post completion packet"));
        }

        return ret;
    }

    // Wait for completion status to arrive.
    // This function can block forever in it's wait for completion status.
    // Use PostEmptyStatus() to wake it up (and end the thread)
    bool GetStatus(unsigned long* count, wxFSWatchEntry** watch,
                   OVERLAPPED** overlapped)
    {
        wxCHECK_MSG( m_iocp != INVALID_HANDLE_VALUE, false, "IOCP not init" );
        wxCHECK_MSG( count != NULL, false, "Null out parameter 'count'");
        wxCHECK_MSG( watch != NULL, false,
                     "Null out parameter 'watch'");
        wxCHECK_MSG( overlapped != NULL, false,
                     "Null out parameter 'overlapped'");

        wxLogTrace(wxTRACE_FSWATCHER, "Waiting on a completion port...");

        int ret = GetQueuedCompletionStatus(m_iocp, count,
                                            (PULONG_PTR)watch,
                                            overlapped, INFINITE);
        if (!ret)
        {
            wxLogSysError(_("Unable to dequeue completion packet"));
        }

        //wxLogTrace(wxTRACE_FSWATCHER, "Read packet: count=%u, key=%p, overlapped=%p",
        //          *count, *watch, *overlapped);

        return ret;
    }

protected:
    bool Init()
    {
        m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (m_iocp == NULL)
        {
            wxLogSysError(_("Unable to create I/O completion port"));
        }

        return m_iocp != NULL;
    }

    HANDLE m_iocp;
};


class wxIOCPThread : public wxThread
{
public:
    wxIOCPThread(wxFSWatcherService* service, wxIOCPService* iocp) :
        wxThread(wxTHREAD_JOINABLE),
        m_service(service), m_iocp(iocp)
    {
    }

    // finishes this thread
    bool Finish()
    {
        wxLogTrace(wxTRACE_FSWATCHER, "Post empty status!");

        // send "empty" packet to ourselves, just to wake
        return m_iocp->PostEmptyStatus();
    }

protected:
    virtual ExitCode Entry()
    {
        wxLogTrace(wxTRACE_FSWATCHER, "[iocp] Started IOCP thread");

        // read events in a loop until we get false, which means we should exit
        while ( ReadEvents() );

        wxLogTrace(wxTRACE_FSWATCHER, "[iocp] Ended IOCP thread");
        return (ExitCode)0;
    }

    // wait for events to occur, read them and send to interested parties
    // returns false it empty status was read, which means we whould exit
    //         true otherwise
    bool ReadEvents();

    void ProcessNativeEvent(const FILE_NOTIFY_INFORMATION& e,
                            wxFSWatchEntry* watch)
    {
        wxLogTrace(wxTRACE_FSWATCHER, "Event: off=%d, action=%d, len=%d, name='%s'",
                  e.NextEntryOffset, e.Action, e.FileNameLength, e.FileName);

        // CHECK can we have multiple flags set? rather no, but check
        int flags = Native2WatcherFlags(e.Action);
        if (flags & wxFSW_EVENT_WARNING || flags & wxFSW_EVENT_ERROR)
        {
            // TODO think about this...do we ever have any errors to report?
            wxString errMsg = "Error occurred";
            wxFileSystemWatcherEvent event(flags, errMsg);
            SendEvent(event);
        }
        // filter out ignored events and those not asked for.
        // we never filter out warnings or exceptions
        else if ((flags == 0) || !(flags & watch->GetFlags()))
        {
            return;
        }
        else
        {
            // FIXME actually now only adding dirs work
			// CHECK I heard that returned path can be either in short on long
			// form...need to account for that!
            wxFileName path = watch->GetPath();
            if (path.IsDir())
                path = wxFileName(path.GetPath(), e.FileName);

            // TODO figure out renames: proper paths!
            wxFileSystemWatcherEvent event(flags, path, path);
            SendEvent(event);
        }
    }

    void SendEvent(wxFileSystemWatcherEvent& evt);

    static int Native2WatcherFlags(int flags)
    {
        // FIXME remove in the future. just for reference
#if 0
        FILE_ACTION_ADDED
        0x00000001
        The file was added to the directory.

        FILE_ACTION_REMOVED
        0x00000002
        The file was removed from the directory.

        FILE_ACTION_MODIFIED
        0x00000003
        The file was modified. Can be a change in the time stamp or attributes.

        FILE_ACTION_RENAMED_OLD_NAME
        0x00000004
        The file was renamed and this is the old name.

        FILE_ACTION_RENAMED_NEW_NAME
        0x00000005
        The file was renamed and this is the new name.
#endif

        // TODO this needs more thinking
        // XXX from what can be seen now, we don't have notification
        // to match wxFSW_EVENT_ACCESS
        static const int flag_mapping[][2] = {
            { FILE_ACTION_ADDED,            wxFSW_EVENT_CREATE },
            { FILE_ACTION_REMOVED,          wxFSW_EVENT_DELETE },
            { FILE_ACTION_MODIFIED,         wxFSW_EVENT_MODIFY },

            // XXX temporary to pass the test
            { FILE_ACTION_RENAMED_OLD_NAME, wxFSW_EVENT_RENAME },

            { FILE_ACTION_RENAMED_NEW_NAME, 0 }, // ignored for now
        };

        for (unsigned int i=0; i < WXSIZEOF(flag_mapping); ++i) {
#if 0
            wxLogDebug("mapping: %d=%d", flag_mapping[i][0],
                                            flag_mapping[i][1]);
#endif
            // TODO need to account for more flags set at once?
            // rather not, because actions have numbers 1,2,3...
            // it wouldn't work as flags
            if (flags == flag_mapping[i][0])
                return flag_mapping[i][1];
        }

        // never reached
        wxFAIL_MSG(wxString::Format("Unknown file notify change %u", flags));
        return -1;
    }

    wxFSWatcherService* m_service;
    wxIOCPService* m_iocp;
};

class wxFSWatcherService
{
public:
    wxFSWatcherService(wxFileSystemWatcherBase* watcher) :
        m_watcher(watcher), m_workerThread(this, &m_iocp)
    {
    }

    ~wxFSWatcherService()
    {
        // order the thread to finish & wait
        m_workerThread.Finish();
        wxThread::ExitCode rc = m_workerThread.Wait();
        if (rc != 0)
        {
            wxLogError(_("Ungraceful wokrer thread termination"));
        }

        // we need to remove them as the last thing here, because IOCP thread
        // may still try to access some of them
        // (actually watches should already be removed)
        (void) RemoveAll();
    }

    // remove all watches
    bool RemoveAll()
    {
        // TODO add _logic_ remove?
        wxFSWatchEntryHandles::iterator it = m_watchMap.begin();
        for ( ; it != m_watchMap.end(); ++it )
        {
            delete it->second;
        }
        m_watchMap.clear();

        return true;
    }

    bool Init()
    {
        wxCHECK_MSG( !m_workerThread.IsAlive(), false,
                     "Watcher service appears to be already initialized" );

        if (m_workerThread.Create() != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Unable to create IOCP worker thread"));
            return false;
        }

        // we have valid iocp service and thread
        if (m_workerThread.Run() != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Unable to start IOCP worker thread"));
            return false;
        }

        return true;
    }

    // adds watch to be monitored for file system changes
    bool Add(wxFSWatchEntry* watch)
    {
        wxCHECK_MSG( watch->IsOk(), false, "Uninitiliazed watch");
        wxCHECK_MSG( m_watchMap.find(watch->GetHandle()) == m_watchMap.end(),
                     false, "Path '%s' is already watched");

        // setting up wait for directory changes
        if (!DoSetUpWatch(*watch))
            return false;

        // associating handle with completion port
        if (!m_iocp.Add(watch->GetHandle(), watch))
            return false;

        // add watch to our map (always succeedes, checked above)
        wxFSWatchEntryHandles::value_type val(watch->GetHandle(), watch);
        return m_watchMap.insert(val).second;
    }

	// TODO what should be done here?
    bool Remove(wxFSWatchEntry* watch)
    {
        // TODO logic implementation, as for now we only have _basic_ cleanup

        m_watchMap.erase(watch->GetHandle());
        delete watch;

        return true;
    }

    // TODO ensuring that we have not already set watch for this handle/dir?
    bool SetUpWatch(wxFSWatchEntry& watch)
    {
        wxCHECK_MSG( watch.GetHandle() != INVALID_HANDLE_VALUE, false,
                     "Unitialized watch: invalid handle" );
        wxCHECK_MSG( m_watchMap.find(watch.GetHandle()) != m_watchMap.end(),
                     false, "Watch not on the watch list");

        wxLogTrace(wxTRACE_FSWATCHER, "Setting up watch for file system changes...");
        return DoSetUpWatch(watch);
    }

	void SendEvent(wxFileSystemWatcherEvent& evt)
    {
        // called from worker thread, so posting event in thread-safe way
        wxQueueEvent(m_watcher->GetOwner(), evt.Clone());
    }

protected:
    bool DoSetUpWatch(wxFSWatchEntry& watch)
    {
        int flags = Watcher2NativeFlags(watch.GetFlags());
        int ret = ReadDirectoryChangesW(watch.GetHandle(), watch.GetBuffer(),
                                        wxFSWatchEntry::BUFFER_SIZE, TRUE,
                                        flags, NULL,
                                        watch.GetOverlapped(), NULL);
        if (!ret)
        {
            wxLogSysError(_("Unable to set up watch for '%s'"),
                            watch.GetPath().GetFullPath());
        }

        return ret;
    }

    // TODO we should only specify those flags, which interest us
    static int Watcher2NativeFlags(int flags)
    {
        static DWORD all_events = FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
                FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION |
                FILE_NOTIFY_CHANGE_SECURITY;

        // FIXME this needs a bit of thinking, quick impl for now
        return all_events;
    }

    wxFileSystemWatcherBase* m_watcher;
    wxIOCPService m_iocp;
    wxIOCPThread m_workerThread;
    wxFSWatchEntryHandles m_watchMap;
};

bool wxIOCPThread::ReadEvents()
{
    unsigned long count = 0;
    wxFSWatchEntry* watch = NULL;
    OVERLAPPED* overlapped = NULL;
    if (!m_iocp->GetStatus(&count, &watch, &overlapped))
        return true; // error was logged already, we don't want to exit

    // this is our exit condition, so we return false
    if (!count || !watch)
    {
        return false;
    }

    wxLogTrace(wxTRACE_FSWATCHER, "Read entry: path='%s'", watch->GetPath().GetFullPath());

    // reissue the watch. ignore possible errors, we will return true anyway
    (void) m_service->SetUpWatch(*watch);

    // extract events from buffer
    // we copy buffer because we cannot ensure someone will not remove a watch
    // from event handler CHECK maybe use wxMemoryBuffer? it'd be cool
    char buffer[wxFSWatchEntry::BUFFER_SIZE];
    memcpy(buffer, watch->GetBuffer(), sizeof(buffer));
	char* memory = buffer;
    int offset = 0;
    int event_count = 0;
    do
    {
        event_count++;
        FILE_NOTIFY_INFORMATION* e = static_cast<FILE_NOTIFY_INFORMATION*>((void*)memory);

        // process one native event
        ProcessNativeEvent(*e, watch);

        offset = e->NextEntryOffset;
        memory += offset;
    }
    while (offset);

    return true;
}

void wxIOCPThread::SendEvent(wxFileSystemWatcherEvent& evt)
{
    wxLogTrace(wxTRACE_FSWATCHER, "EVT: %s", evt.ToString());
    m_service->SendEvent(evt);
}

// ============================================================================
// wxMSWFileSystemWatcher implementation
// ============================================================================

wxMSWFileSystemWatcher::wxMSWFileSystemWatcher() :
    wxFileSystemWatcherBase()
{
    (void) Init();
}

wxMSWFileSystemWatcher::wxMSWFileSystemWatcher(const wxFileName& path,
                                               int events) :
    wxFileSystemWatcherBase()
{
    if (!Init())
        return;
    Add(path, events);
}

bool wxMSWFileSystemWatcher::Init()
{
    m_service = new wxFSWatcherService(this);
    bool ret = m_service->Init();
    if (!ret)
    {
        delete m_service;
    }
    return ret;
}

wxMSWFileSystemWatcher::~wxMSWFileSystemWatcher()
{
    // it is subclass resposibility to remove all paths - we own wxFSWatchEntry
    // structures
    RemoveAll();
    delete m_service;
}

// actual adding: just delegate to wxFSWatcherService
bool wxMSWFileSystemWatcher::DoAdd(wxFSWatchEntry& watch)
{
    return m_service->Add(&watch);
}

bool wxMSWFileSystemWatcher::DoRemove(wxFSWatchEntry& watch)
{
    return m_service->Remove(&watch);
}

wxFSWatchEntry* wxMSWFileSystemWatcher::CreateWatch(const wxFileName& path,
                                                    int events)
{
    return new wxFSWatchEntry(path, events);
}


bool wxMSWFileSystemWatcher::AddTree(const wxFileName& path, int events,
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
        virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
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
