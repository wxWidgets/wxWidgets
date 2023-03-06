///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fswatcher/fswatchertest.cpp
// Purpose:     wxFileSystemWatcher unit test
// Author:      Bartosz Bekier
// Created:     2009-06-11
// Copyright:   (c) 2009 Bartosz Bekier
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/timer.h"
#endif

#if wxUSE_FSWATCHER

#include "wx/evtloop.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/fswatcher.h"
#include "wx/log.h"
#include "wx/stdpaths.h"
#include "wx/vector.h"

#include "testfile.h"

#include <memory>

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

// class generating file system events
class EventGenerator
{
public:
    static EventGenerator& Get()
    {
        if (!ms_instance)
            ms_instance = new EventGenerator(GetWatchDir());

        return *ms_instance;
    }

    EventGenerator(const wxFileName& path) : m_base(path)
    {
        m_old = wxFileName();
        m_file = RandomName();
        m_new = RandomName();
    }

    // operations
    bool CreateFile()
    {
        wxFile file(m_file.GetFullPath(), wxFile::write);
        return file.IsOpened() && m_file.FileExists();
    }

    bool RenameFile()
    {
        REQUIRE(m_file.FileExists());

        wxLogDebug("Renaming %s=>%s", m_file.GetFullPath(), m_new.GetFullPath());

        bool ret = wxRenameFile(m_file.GetFullPath(), m_new.GetFullPath());
        if (ret)
        {
            m_old = m_file;
            m_file = m_new;
            m_new = RandomName();
        }

        return ret;
    }

    bool DeleteFile()
    {
        REQUIRE(m_file.FileExists());

        bool ret =  wxRemoveFile(m_file.GetFullPath());
        if (ret)
        {
            m_old = m_file;
            m_file = m_new;
            m_new = RandomName();
        }

        return ret;
    }

    bool TouchFile()
    {
        return m_file.Touch();
    }

    bool ReadFile()
    {
        wxFile f(m_file.GetFullPath());
        REQUIRE(f.IsOpened());

        char buf[1];
        ssize_t count = f.Read(buf, sizeof(buf));
        CHECK(count > 0);

        return true;
    }

    bool ModifyFile()
    {
        REQUIRE(m_file.FileExists());

        wxFile file(m_file.GetFullPath(), wxFile::write_append);
        REQUIRE(file.IsOpened());

        CHECK(file.Write("Words of Wisdom, Lloyd. Words of wisdom\n"));
        return file.Close();
    }

    // helpers
    wxFileName RandomName(int length = 10)
    {
        return RandomName(m_base, length);
    }

    // static helpers
    static const wxFileName& GetWatchDir()
    {
        if (ms_watchDir.DirExists())
            return ms_watchDir;

        wxString tmp = wxStandardPaths::Get().GetTempDir();
        ms_watchDir.AssignDir(tmp);

        // XXX look for more unique name? there is no function to generate
        // unique filename, the file always get created...
        ms_watchDir.AppendDir("fswatcher_test");
        REQUIRE(!ms_watchDir.DirExists());

        TestLogEnabler enableLogs;
        REQUIRE(ms_watchDir.Mkdir());

        REQUIRE(ms_watchDir.DirExists());

        return ms_watchDir;
    }

    static void RemoveWatchDir()
    {
        REQUIRE(ms_watchDir.DirExists());

        // just to be really sure we know what we remove
        REQUIRE( ms_watchDir.GetDirs().Last() == "fswatcher_test" );

        // Sometimes the directory can't be destroyed immediately because,
        // apparently, Windows itself keeps a handle to it (or one of the files
        // in it?), so retry a few times.
        TestLogEnabler enableLogs;
        for ( int i = 0; i < 3; ++i )
        {
            if ( ms_watchDir.Rmdir(wxPATH_RMDIR_RECURSIVE) )
            {
                ms_watchDir = wxFileName();
                return;
            }

            wxMilliSleep(200);
        }

        FAIL( "Failed to remove " << ms_watchDir.GetFullPath() );
    }

    static wxFileName RandomName(const wxFileName& base, int length = 10)
    {
        static int ALFA_CNT = 'z' - 'a';

        wxString s;
        for (int i = 0 ; i < length; ++i)
        {
            char c = 'a' + (rand() % ALFA_CNT);
            s += c;
        }

        return wxFileName(base.GetFullPath(), s);
    }

public:
    wxFileName m_base;     // base dir for doing operations
    wxFileName m_file;     // current file name
    wxFileName m_old;      // previous file name
    wxFileName m_new;      // name after renaming

protected:
    static EventGenerator* ms_instance;

private:
    static wxFileName ms_watchDir;
};

EventGenerator* EventGenerator::ms_instance = nullptr;
wxFileName EventGenerator::ms_watchDir;


// Abstract base class from which concrete event tests inherit.
//
// This class provides the common test skeleton which various virtual hooks
// that should or can be reimplemented by the derived classes.
class FSWTesterBase : public wxEvtHandler
{
public:
    FSWTesterBase(int types = wxFSW_EVENT_ALL) :
        eg(EventGenerator::Get()),
        m_eventTypes(types)
    {
        Bind(wxEVT_FSWATCHER, &FSWTesterBase::OnFileSystemEvent, this);

        // wxFileSystemWatcher can be created only once the event loop is
        // running, so we can't do it from here and will do it from inside the
        // loop when this event handler is invoked.
        CallAfter(&FSWTesterBase::OnIdleInit);
    }

    virtual ~FSWTesterBase()
    {
        if (m_loop.IsRunning())
            m_loop.Exit();
    }

    void Exit()
    {
        m_loop.Exit();
    }

    // sends idle event, so we get called in a moment
    void SendIdle()
    {
        wxIdleEvent* e = new wxIdleEvent();
        QueueEvent(e);
    }

    void Run()
    {
        m_loop.Run();
    }

    void OnIdleInit()
    {
        REQUIRE(Init());

        GenerateEvent();

        // Check the result when the next idle event comes: note that we can't
        // use CallAfter() here, unfortunately, because OnIdleCheckResult()
        // would then be called immediately, from the same event loop iteration
        // as we're called from, because the idle/pending events are processed
        // for as long as there any. Instead, we need to return to the event
        // loop itself to give it a chance to dispatch wxFileSystemWatcherEvent
        // and wait until our handler for it calls SendIdle() which will then
        // end up calling OnIdleCheckResult() afterwards.
        Bind(wxEVT_IDLE, &FSWTesterBase::OnIdleCheckResult, this);
    }

    void OnIdleCheckResult(wxIdleEvent& WXUNUSED(event))
    {
        Unbind(wxEVT_IDLE, &FSWTesterBase::OnIdleCheckResult, this);

        CheckResult();
        Exit();
    }

    virtual bool Init()
    {
        // test we're good to go
        CHECK(wxEventLoopBase::GetActive());

        // XXX only now can we construct Watcher, because we need
        // active loop here
        m_watcher.reset(new wxFileSystemWatcher());
        m_watcher->SetOwner(this);

        // add dir to be watched
        wxFileName dir = EventGenerator::GetWatchDir();
        CHECK(m_watcher->Add(dir, m_eventTypes));

        return true;
    }

    virtual void CheckResult()
    {
        REQUIRE( !m_events.empty() );

        const wxFileSystemWatcherEvent * const e = m_events.front();

        // this is our "reference event"
        const wxFileSystemWatcherEvent expected = ExpectedEvent();

        CHECK( e->GetChangeType() == expected.GetChangeType() );

        CHECK( e->GetEventType() == wxEVT_FSWATCHER );

        // XXX this needs change
        CHECK( e->GetEventCategory() == wxEVT_CATEGORY_UNKNOWN );

        CHECK( e->GetPath() == expected.GetPath() );
        CHECK( e->GetNewPath() == expected.GetNewPath() );

        // Under MSW extra modification events are sometimes reported after a
        // rename and we just can't get rid of them, so ignore them in this
        // test if they do happen.
        if ( e->GetChangeType() == wxFSW_EVENT_RENAME &&
                m_events.size() == 2 )
        {
            const wxFileSystemWatcherEvent* const e2 = m_events.back();
            if ( e2->GetChangeType() == wxFSW_EVENT_MODIFY &&
                    e2->GetPath() == e->GetNewPath() )
            {
                // This is a modify event for the new file, ignore it.
                return;
            }
        }

        WX_ASSERT_EQUAL_MESSAGE
        (
            (
                "Extra events received, last one is of type %x, path=\"%s\" "
                "(the original event was for \"%s\" (\"%s\")",
                m_events.back()->GetChangeType(),
                m_events.back()->GetPath().GetFullPath(),
                e->GetPath().GetFullPath(),
                e->GetNewPath().GetFullPath()
            ),
            1, m_events.size()
        );

        delete e;
    }

    virtual void GenerateEvent() = 0;

    virtual wxFileSystemWatcherEvent ExpectedEvent() = 0;


protected:
    EventGenerator& eg;
    wxEventLoop m_loop;    // loop reference

    std::unique_ptr<wxFileSystemWatcher> m_watcher;

    int m_eventTypes;  // Which event-types to watch. Normally all of them

    wxVector<wxFileSystemWatcherEvent*> m_events;

private:
    void OnFileSystemEvent(wxFileSystemWatcherEvent& evt)
    {
        wxLogDebug("--- %s ---", evt.ToString());
        m_events.push_back(wxDynamicCast(evt.Clone(), wxFileSystemWatcherEvent));

        // test finished
        SendIdle();
    }
};


// ----------------------------------------------------------------------------
// test fixture
// ----------------------------------------------------------------------------

class FileSystemWatcherTestCase
{
public:
    FileSystemWatcherTestCase()
    {
        // Before each test, remove the dir if it exists.
        // It would exist if the previous test run was aborted.
        wxString tmp = wxStandardPaths::Get().GetTempDir();
        wxFileName dir;
        dir.AssignDir(tmp);
        dir.AppendDir("fswatcher_test");
        dir.Rmdir(wxPATH_RMDIR_RECURSIVE);
        EventGenerator::Get().GetWatchDir();
    }

    ~FileSystemWatcherTestCase()
    {
        EventGenerator::Get().RemoveWatchDir();
    }
};

// ----------------------------------------------------------------------------
// TestEventCreate
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventCreate", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.CreateFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_CREATE);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    EventTester tester;

    tester.Run();
}

// ----------------------------------------------------------------------------
// TestEventDelete
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventDelete", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.DeleteFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_DELETE);
            event.SetPath(eg.m_old);

            // CHECK maybe new path here could be null or sth?
            event.SetNewPath(eg.m_old);
            return event;
        }
    };

    // we need to create a file now, so we can delete it
    EventGenerator::Get().CreateFile();

    EventTester tester;
    tester.Run();
}

// kqueue-based implementation doesn't collapse create/delete pairs in
// renames and detects neither modifications nor access to the
// files reliably currently so disable these tests
//
// FIXME: fix the code and reenable them
#ifndef wxHAS_KQUEUE

// ----------------------------------------------------------------------------
// TestEventRename
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventRename", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.RenameFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_RENAME);
            event.SetPath(eg.m_old);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    // need a file to rename later
    EventGenerator::Get().CreateFile();

    EventTester tester;
    tester.Run();
}

// ----------------------------------------------------------------------------
// TestEventModify
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventModify", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.ModifyFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_MODIFY);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    // we need to create a file to modify
    EventGenerator::Get().CreateFile();

    EventTester tester;
    tester.Run();
}

// MSW implementation doesn't detect file access events currently
#ifndef __WINDOWS__

// ----------------------------------------------------------------------------
// TestEventAccess
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventAccess", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.ReadFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_ACCESS);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    // we need to create a file to read from it and write sth to it
    EventGenerator::Get().CreateFile();
    EventGenerator::Get().ModifyFile();

    EventTester tester;
    tester.Run();
}

#endif // __WINDOWS__

#endif // !wxHAS_KQUEUE

#ifdef wxHAS_INOTIFY
// ----------------------------------------------------------------------------
// TestEventAttribute
// ----------------------------------------------------------------------------
TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::EventAttribute", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        virtual void GenerateEvent() override
        {
            CHECK(eg.TouchFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_ATTRIB);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    // we need to create a file to touch
    EventGenerator::Get().CreateFile();

    EventTester tester;
    tester.Run();
}

// ----------------------------------------------------------------------------
// TestSingleWatchtypeEvent: Watch only wxFSW_EVENT_ACCESS
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::SingleWatchtypeEvent", "[fsw]")
{
    class EventTester : public FSWTesterBase
    {
    public:
        // We could pass wxFSW_EVENT_CREATE or MODIFY instead, but not RENAME or
        // DELETE as the event path fields would be wrong in CheckResult()
        EventTester() : FSWTesterBase(wxFSW_EVENT_ACCESS) {}

        virtual void GenerateEvent() override
        {
            // As wxFSW_EVENT_ACCESS is passed to the ctor only ReadFile() will
            // generate an event. Without it they all will, and the test fails
            CHECK(eg.CreateFile());
            CHECK(eg.ModifyFile());
            CHECK(eg.ReadFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_ACCESS);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    EventTester tester;
    tester.Run();
}
#endif // wxHAS_INOTIFY

// ----------------------------------------------------------------------------
// TestTrees
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::Trees", "[fsw]")
{
    class TreeTester : public FSWTesterBase
    {
        const size_t subdirs;
        const size_t files;

    public:
        TreeTester() : subdirs(5), files(3) {}

        void GrowTree(wxFileName dir
#ifdef __UNIX__
                      , bool withSymlinks = false
#endif
                      )
        {
            REQUIRE(dir.Mkdir());
            // Now add a subdir with an easy name to remember in WatchTree()
            dir.AppendDir("child");
            REQUIRE(dir.Mkdir());
            wxFileName child(dir);  // Create a copy to which to symlink

            // Create a branch of 5 numbered subdirs, each containing 3
            // numbered files
            for ( unsigned d = 0; d < subdirs; ++d )
            {
                dir.AppendDir(wxString::Format("subdir%u", d+1));
                REQUIRE(dir.Mkdir());

                const wxString prefix = dir.GetPathWithSep();
                const wxString ext[] = { ".txt", ".log", "" };
                for ( unsigned f = 0; f < files; ++f )
                {
                    // Just create the files.
                    wxFile(prefix + wxString::Format("file%u", f+1) + ext[f],
                           wxFile::write);
                }
#if defined(__UNIX__)
                if ( withSymlinks )
                {
                    // Create a symlink to a files, and another to 'child'
                    CHECK
                    (
                        symlink(wxString(prefix + "file1").c_str(),
                                wxString(prefix + "file.lnk").c_str()) == 0
                    );

                    CHECK
                    (
                        symlink(child.GetFullPath().c_str(),
                                wxString(prefix + "dir.lnk").c_str()) == 0
                    );
                }
#endif // __UNIX__
            }
        }

        void RmDir(wxFileName dir)
        {
            REQUIRE(dir.DirExists());

            CHECK(dir.Rmdir(wxPATH_RMDIR_RECURSIVE));
        }

        void WatchDir(wxFileName dir)
        {
            REQUIRE(m_watcher);

            // Store the initial count; there may already be some watches
            const int initial = m_watcher->GetWatchedPathsCount();

            m_watcher->Add(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == initial + 1 );
        }

        void RemoveSingleWatch(wxFileName dir)
        {
            REQUIRE(m_watcher);

            const int initial = m_watcher->GetWatchedPathsCount();

            m_watcher->Remove(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == initial - 1 );
        }

        void WatchTree(const wxFileName& dir)
        {
            REQUIRE(m_watcher);

            int treeitems = 1; // the trunk
#if !defined(__WINDOWS__) && !defined(wxHAVE_FSEVENTS_FILE_NOTIFICATIONS)
            // When there's no file mask, wxMSW and wxOSX set a single watch
            // on the trunk which is implemented recursively.
            // wxGTK always sets an additional watch for each subdir
            treeitems += subdirs + 1; // +1 for 'child'
#endif // !__WINDOWS__ && !wxHAVE_FSEVENTS_FILE_NOTIFICATIONS

            // Store the initial count; there may already be some watches
            const int initial = m_watcher->GetWatchedPathsCount();

            GrowTree(dir);

            m_watcher->AddTree(dir);
            const int plustree = m_watcher->GetWatchedPathsCount();

            CHECK( plustree == initial + treeitems );

            m_watcher->RemoveTree(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == initial );

            // Now test the refcount mechanism by watching items more than once
            wxFileName child(dir);
            child.AppendDir("child");
            m_watcher->AddTree(child);
            // Check some watches were added; we don't care about the number
            CHECK(initial < m_watcher->GetWatchedPathsCount());
            // Now watch the whole tree and check that the count is the same
            // as it was the first time, despite also adding 'child' separately
            // Except that in wxMSW this isn't true: each watch will be a
            // single, recursive dir; so fudge the count
            int fudge = 0;
#if defined(__WINDOWS__) || defined(wxHAVE_FSEVENTS_FILE_NOTIFICATIONS)
            fudge = 1;
#endif // __WINDOWS__ || wxHAVE_FSEVENTS_FILE_NOTIFICATIONS
            m_watcher->AddTree(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == plustree + fudge );
            m_watcher->RemoveTree(child);
            CHECK(initial < m_watcher->GetWatchedPathsCount());
            m_watcher->RemoveTree(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == initial );
#if defined(__UNIX__)
            // Finally, test a tree containing internal symlinks
            RmDir(dir);
            GrowTree(dir, true /* test symlinks */);

            // Without the DontFollowLink() call AddTree() would now assert
            // (and without the assert, it would infinitely loop)
            wxFileName fn = dir;
            fn.DontFollowLink();
            CHECK(m_watcher->AddTree(fn));
            CHECK(m_watcher->RemoveTree(fn));

            // Regrow the tree without symlinks, ready for the next test
            RmDir(dir);
            GrowTree(dir, false);
#endif // __UNIX__
        }

        void WatchTreeWithFilespec(const wxFileName& dir)
        {
            REQUIRE(m_watcher);
            REQUIRE(dir.DirExists()); // Was built in WatchTree()

            // Store the initial count; there may already be some watches
            const int initial = m_watcher->GetWatchedPathsCount();

            // When we use a filter, both wxMSW and wxGTK implementations set
            // an additional watch for each subdir (+1 for the root dir itself
            // and another +1 for "child").
            // On OS X, if we use FSEvents then we still only have 1 watch.
#ifdef wxHAVE_FSEVENTS_FILE_NOTIFICATIONS
            const int treeitems = 1;
#else
            const int treeitems = subdirs + 2;
#endif
            m_watcher->AddTree(dir, wxFSW_EVENT_ALL, "*.txt");

            CHECK( m_watcher->GetWatchedPathsCount() == initial + treeitems );

            // RemoveTree should try to remove only those files that were added
            m_watcher->RemoveTree(dir);
            CHECK( m_watcher->GetWatchedPathsCount() == initial );
        }

        void RemoveAllWatches()
        {
            REQUIRE(m_watcher);

            m_watcher->RemoveAll();
            CHECK( m_watcher->GetWatchedPathsCount() == 0 );
        }

        virtual void GenerateEvent() override
        {
            // We don't use this function for events. Just run the tests

            wxFileName watchdir = EventGenerator::GetWatchDir();

            wxFileName treedir(watchdir);
            treedir.AppendDir("treetrunk");
            CHECK(!treedir.DirExists());

            wxFileName singledir(watchdir);
            singledir.AppendDir("single");
            CHECK(!singledir.DirExists());
            CHECK(singledir.Mkdir());

            WatchDir(singledir);
            WatchTree(treedir);
            // Now test adding and removing a tree using a filespec
            // wxMSW uses the generic method to add matching files; which fails
            // as it doesn't support adding files :/ So disable the test
#ifndef __WINDOWS__
            WatchTreeWithFilespec(treedir);
#endif // __WINDOWS__

            RemoveSingleWatch(singledir);
            // Add it back again, ready to test RemoveAll()
            WatchDir(singledir);

            RemoveAllWatches();

            // Clean up
            RmDir(treedir);
            RmDir(singledir);

            Exit();
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent() override
        {
            FAIL("Shouldn't be called");

            return wxFileSystemWatcherEvent(wxFSW_EVENT_ERROR);
        }

        virtual void CheckResult() override
        {
            // Do nothing. We override this to prevent receiving events in
            // ExpectedEvent()
        }
    };

    TreeTester tester;

    tester.Run();
}


namespace
{

// We can't define this class locally inside TestNoEventsAfterRemove() for some
// reason with g++ 4.0 under OS X 10.5, it results in the following mysterious
// error:
//
// /var/tmp//ccTkNCkc.s:unknown:Non-global symbol:
// __ZThn80_ZN25FileSystemWatcherTestCase23TestNoEventsAfterRemoveEvEN11EventTester6NotifyEv.eh
// can't be a weak_definition
//
// So define this class outside the function instead.
class NoEventsAfterRemoveEventTester : public FSWTesterBase,
                                       public wxTimer
{
public:
    NoEventsAfterRemoveEventTester()
    {
        // We need to use an inactivity timer as we never get any file
        // system events in this test, so we consider that the test is
        // finished when this 1s timeout expires instead of, as usual,
        // stopping after getting the file system events.
        Start(1000, true);
    }

    virtual void GenerateEvent() override
    {
        m_watcher->Remove(EventGenerator::GetWatchDir());
        CHECK(eg.CreateFile());
    }

    virtual void CheckResult() override
    {
        REQUIRE( m_events.empty() );
    }

    virtual wxFileSystemWatcherEvent ExpectedEvent() override
    {
        FAIL( "Shouldn't be called" );

        return wxFileSystemWatcherEvent(wxFSW_EVENT_ERROR);
    }

    virtual void Notify() override
    {
        SendIdle();
    }
};

} // anonymous namespace

TEST_CASE_METHOD(FileSystemWatcherTestCase,
                 "wxFileSystemWatcher::NoEventsAfterRemove", "[fsw]")
{
    NoEventsAfterRemoveEventTester tester;
    tester.Run();
}

#endif // wxUSE_FSWATCHER
