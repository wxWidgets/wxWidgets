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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/timer.h"
#endif

#if wxUSE_FSWATCHER

#include "wx/evtloop.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/stdpaths.h"
#include "wx/fswatcher.h"

#include "testfile.h"

/*
This test used to be disabled on OS X as it hung. Work around the apparent
wxOSX differences between a non-GUI event loop and a GUI event loop (where
the tests do run fine) until this gets resolved.
*/
#ifdef __WXOSX__
    #define OSX_EVENT_LOOP_WORKAROUND
#endif

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
        CPPUNIT_ASSERT(m_file.FileExists());

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
        CPPUNIT_ASSERT(m_file.FileExists());

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
        CPPUNIT_ASSERT(f.IsOpened());

        char buf[1];
        ssize_t count = f.Read(buf, sizeof(buf));
        CPPUNIT_ASSERT(count > 0);

        return true;
    }

    bool ModifyFile()
    {
        CPPUNIT_ASSERT(m_file.FileExists());

        wxFile file(m_file.GetFullPath(), wxFile::write_append);
        CPPUNIT_ASSERT(file.IsOpened());

        CPPUNIT_ASSERT(file.Write("Words of Wisdom, Lloyd. Words of wisdom\n"));
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
        static wxFileName dir;

        if (dir.DirExists())
            return dir;

        wxString tmp = wxStandardPaths::Get().GetTempDir();
        dir.AssignDir(tmp);

        // XXX look for more unique name? there is no function to generate
        // unique filename, the file always get created...
        dir.AppendDir("fswatcher_test");
        CPPUNIT_ASSERT(!dir.DirExists());
        CPPUNIT_ASSERT(dir.Mkdir());

        return dir;
    }

    static void RemoveWatchDir()
    {
        wxFileName dir = GetWatchDir();
        CPPUNIT_ASSERT(dir.DirExists());

        // just to be really sure we know what we remove
        CPPUNIT_ASSERT_EQUAL( "fswatcher_test", dir.GetDirs().Last() );

        CPPUNIT_ASSERT( dir.Rmdir(wxPATH_RMDIR_RECURSIVE) );
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
};

EventGenerator* EventGenerator::ms_instance = 0;


// custom event handler
class EventHandler : public wxEvtHandler
{
public:
    enum { WAIT_DURATION = 3 };

    EventHandler(int types = wxFSW_EVENT_ALL) :
        eg(EventGenerator::Get()), m_loop(0),
#ifdef OSX_EVENT_LOOP_WORKAROUND
        m_loopActivator(NULL),
#endif
        m_count(0), m_watcher(0), m_eventTypes(types)
    {
        m_loop = new wxEventLoop();
#ifdef OSX_EVENT_LOOP_WORKAROUND
        m_loopActivator = new wxEventLoopActivator(m_loop);
#endif
        Connect(wxEVT_IDLE, wxIdleEventHandler(EventHandler::OnIdle));
        Connect(wxEVT_FSWATCHER, wxFileSystemWatcherEventHandler(
                                            EventHandler::OnFileSystemEvent));
    }

    virtual ~EventHandler()
    {
        delete m_watcher;
#ifdef OSX_EVENT_LOOP_WORKAROUND
        delete m_loopActivator;
#endif
        if (m_loop)
        {
            if (m_loop->IsRunning())
                m_loop->Exit();
            delete m_loop;
        }
    }

    void Exit()
    {
        m_loop->Exit();
    }

    // sends idle event, so we get called in a moment
    void SendIdle()
    {
        wxIdleEvent* e = new wxIdleEvent();
        QueueEvent(e);

#ifdef OSX_EVENT_LOOP_WORKAROUND
        // The fs watcher test cases will hang on OS X if Yield() is not called.
        // It seems that the OS X event loop and / or queueing behaves
        // differently than on MSW and Linux.
        m_loop->Yield(true);
#endif
    }

    void Run()
    {
        SendIdle();
        m_loop->Run();
    }

    void OnIdle(wxIdleEvent& /*evt*/)
    {
        bool more = Action();
        m_count++;

        if (more)
        {
            SendIdle();
        }
    }

    // returns whether we should produce more idle events
    virtual bool Action()
    {
        switch (m_count)
        {
        case 0:
            CPPUNIT_ASSERT(Init());
            break;
        case 1:
            GenerateEvent();
            break;
        case 2:
            // actual test
            CheckResult();
            Exit();
            break;

        // TODO a mechanism that will break the loop in case we
        // don't receive a file system event
        // this below doesn't quite work, so all tests must pass :-)
#if 0
        case 2:
            m_loop.Yield();
            m_loop.WakeUp();
            CPPUNIT_ASSERT(KeepWaiting());
            m_loop.Yield();
            break;
        case 3:
            break;
        case 4:
            CPPUNIT_ASSERT(AfterWait());
            break;
#endif
        } // switch (m_count)

        return m_count <= 0;
    }

    virtual bool Init()
    {
        // test we're good to go
        CPPUNIT_ASSERT(wxEventLoopBase::GetActive());

        // XXX only now can we construct Watcher, because we need
        // active loop here
        m_watcher = new wxFileSystemWatcher();
        m_watcher->SetOwner(this);

        // add dir to be watched
        wxFileName dir = EventGenerator::GetWatchDir();
        CPPUNIT_ASSERT(m_watcher->Add(dir, m_eventTypes));

        return true;
    }

    virtual bool KeepWaiting()
    {
        // did we receive event already?
        if (!tested)
        {
            // well, let's wait a bit more
            wxSleep(WAIT_DURATION);
        }

        return true;
    }

    virtual bool AfterWait()
    {
        // fail if still no events
        WX_ASSERT_MESSAGE
        (
             ("No events during %d seconds!", static_cast<int>(WAIT_DURATION)),
             tested
        );

        return true;
    }

    virtual void OnFileSystemEvent(wxFileSystemWatcherEvent& evt)
    {
        wxLogDebug("--- %s ---", evt.ToString());
        m_lastEvent = wxDynamicCast(evt.Clone(), wxFileSystemWatcherEvent);
        m_events.Add(m_lastEvent);

        // test finished
        SendIdle();
        tested = true;
    }

    virtual void CheckResult()
    {
        CPPUNIT_ASSERT_MESSAGE( "No events received", !m_events.empty() );

        const wxFileSystemWatcherEvent * const e = m_events.front();

        // this is our "reference event"
        const wxFileSystemWatcherEvent expected = ExpectedEvent();

        CPPUNIT_ASSERT_EQUAL( expected.GetChangeType(), e->GetChangeType() );

        CPPUNIT_ASSERT_EQUAL((int)wxEVT_FSWATCHER, e->GetEventType());

        // XXX this needs change
        CPPUNIT_ASSERT_EQUAL(wxEVT_CATEGORY_UNKNOWN, e->GetEventCategory());

        CPPUNIT_ASSERT_EQUAL(expected.GetPath(), e->GetPath());
        CPPUNIT_ASSERT_EQUAL(expected.GetNewPath(), e->GetNewPath());

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

    }

    virtual void GenerateEvent() = 0;

    virtual wxFileSystemWatcherEvent ExpectedEvent() = 0;


protected:
    EventGenerator& eg;
    wxEventLoopBase* m_loop;    // loop reference
#ifdef OSX_EVENT_LOOP_WORKAROUND
    wxEventLoopActivator* m_loopActivator;
#endif
    int m_count;                // idle events count

    wxFileSystemWatcher* m_watcher;
    int m_eventTypes;  // Which event-types to watch. Normally all of them
    bool tested;  // indicates, whether we have already passed the test

    #include "wx/arrimpl.cpp"
    WX_DEFINE_ARRAY_PTR(wxFileSystemWatcherEvent*, wxArrayEvent);
    wxArrayEvent m_events;
    wxFileSystemWatcherEvent* m_lastEvent;
};


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FileSystemWatcherTestCase : public CppUnit::TestCase
{
public:
    FileSystemWatcherTestCase() { }

    virtual void setUp();
    virtual void tearDown();

protected:
    wxEventLoopBase* m_loop;

private:
    CPPUNIT_TEST_SUITE( FileSystemWatcherTestCase );
        CPPUNIT_TEST( TestEventCreate );
        CPPUNIT_TEST( TestEventDelete );
        CPPUNIT_TEST( TestTrees );

        // kqueue-based implementation doesn't collapse create/delete pairs in
        // renames and doesn't detect neither modifications nor access to the
        // files reliably currently so disable these tests
        //
        // FIXME: fix the code and reenable them
#ifndef wxHAS_KQUEUE
        CPPUNIT_TEST( TestEventRename );
        CPPUNIT_TEST( TestEventModify );

        // MSW implementation doesn't detect file access events currently
#ifndef __WINDOWS__
        CPPUNIT_TEST( TestEventAccess );
#endif // __WINDOWS__
#endif // !wxHAS_KQUEUE

#ifdef wxHAS_INOTIFY
        CPPUNIT_TEST( TestEventAttribute );
        CPPUNIT_TEST( TestSingleWatchtypeEvent );
#endif // wxHAS_INOTIFY

        CPPUNIT_TEST( TestNoEventsAfterRemove );
    CPPUNIT_TEST_SUITE_END();

    void TestEventCreate();
    void TestEventDelete();
    void TestEventRename();
    void TestEventModify();
    void TestEventAccess();
#ifdef wxHAS_INOTIFY
    void TestEventAttribute();
    void TestSingleWatchtypeEvent();
#endif // wxHAS_INOTIFY
    void TestTrees();
    void TestNoEventsAfterRemove();

    wxDECLARE_NO_COPY_CLASS(FileSystemWatcherTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileSystemWatcherTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileSystemWatcherTestCase,
                                        "FileSystemWatcherTestCase" );

void FileSystemWatcherTestCase::setUp()
{
    wxLog::AddTraceMask(wxTRACE_FSWATCHER);

    // Before each test, remove the dir if it exists.
    // It would exist if the previous test run was aborted.
    wxString tmp = wxStandardPaths::Get().GetTempDir();
    wxFileName dir;
    dir.AssignDir(tmp);
    dir.AppendDir("fswatcher_test");
    dir.Rmdir(wxPATH_RMDIR_RECURSIVE);
    EventGenerator::Get().GetWatchDir();
}

void FileSystemWatcherTestCase::tearDown()
{
    EventGenerator::Get().RemoveWatchDir();
}

// ----------------------------------------------------------------------------
// TestEventCreate
// ----------------------------------------------------------------------------
void FileSystemWatcherTestCase::TestEventCreate()
{
    wxLogDebug("TestEventCreate()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.CreateFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_CREATE);
            event.SetPath(eg.m_file);
            event.SetNewPath(eg.m_file);
            return event;
        }
    };

    EventTester tester;

    wxLogTrace(wxTRACE_FSWATCHER, "TestEventCreate tester created()");

    tester.Run();
}

// ----------------------------------------------------------------------------
// TestEventDelete
// ----------------------------------------------------------------------------
void FileSystemWatcherTestCase::TestEventDelete()
{
    wxLogDebug("TestEventDelete()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.DeleteFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
        {
            wxFileSystemWatcherEvent event(wxFSW_EVENT_DELETE);
            event.SetPath(eg.m_old);

            // CHECK maybe new path here could be NULL or sth?
            event.SetNewPath(eg.m_old);
            return event;
        }
    };

    // we need to create a file now, so we can delete it
    EventGenerator::Get().CreateFile();

    EventTester tester;
    tester.Run();
}

// ----------------------------------------------------------------------------
// TestEventRename
// ----------------------------------------------------------------------------
void FileSystemWatcherTestCase::TestEventRename()
{
    wxLogDebug("TestEventRename()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.RenameFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
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
void FileSystemWatcherTestCase::TestEventModify()
{
    wxLogDebug("TestEventModify()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.ModifyFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
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

// ----------------------------------------------------------------------------
// TestEventAccess
// ----------------------------------------------------------------------------
void FileSystemWatcherTestCase::TestEventAccess()
{
    wxLogDebug("TestEventAccess()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.ReadFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
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

#ifdef wxHAS_INOTIFY
// ----------------------------------------------------------------------------
// TestEventAttribute
// ----------------------------------------------------------------------------
void FileSystemWatcherTestCase::TestEventAttribute()
{
    wxLogDebug("TestEventAttribute()");

    class EventTester : public EventHandler
    {
    public:
        virtual void GenerateEvent()
        {
            CPPUNIT_ASSERT(eg.TouchFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
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
void FileSystemWatcherTestCase::TestSingleWatchtypeEvent()
{
    wxLogDebug("TestSingleWatchtypeEvent()");

    class EventTester : public EventHandler
    {
    public:
        // We could pass wxFSW_EVENT_CREATE or MODIFY instead, but not RENAME or
        // DELETE as the event path fields would be wrong in CheckResult()
        EventTester() : EventHandler(wxFSW_EVENT_ACCESS) {}

        virtual void GenerateEvent()
        {
            // As wxFSW_EVENT_ACCESS is passed to the ctor only ReadFile() will
            // generate an event. Without it they all will, and the test fails
            CPPUNIT_ASSERT(eg.CreateFile());
            CPPUNIT_ASSERT(eg.ModifyFile());
            CPPUNIT_ASSERT(eg.ReadFile());
        }

        virtual wxFileSystemWatcherEvent ExpectedEvent()
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

void FileSystemWatcherTestCase::TestTrees()
{
    class TreeTester : public EventHandler
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
            CPPUNIT_ASSERT(dir.Mkdir());
            // Now add a subdir with an easy name to remember in WatchTree()
            dir.AppendDir("child");
            CPPUNIT_ASSERT(dir.Mkdir());
            wxFileName child(dir);  // Create a copy to which to symlink

            // Create a branch of 5 numbered subdirs, each containing 3
            // numbered files
            for ( unsigned d = 0; d < subdirs; ++d )
            {
                dir.AppendDir(wxString::Format("subdir%u", d+1));
                CPPUNIT_ASSERT(dir.Mkdir());

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
                    CPPUNIT_ASSERT_EQUAL(0,
                        symlink(wxString(prefix + "file1").c_str(),
                        wxString(prefix + "file.lnk").c_str()));
                    CPPUNIT_ASSERT_EQUAL(0,
                        symlink(child.GetFullPath().c_str(),
                        wxString(prefix + "dir.lnk").c_str()));
                }
#endif // __UNIX__
            }
        }

        void RmDir(wxFileName dir)
        {
            CPPUNIT_ASSERT(dir.DirExists());

            CPPUNIT_ASSERT(dir.Rmdir(wxPATH_RMDIR_RECURSIVE));
        }

        void WatchDir(wxFileName dir)
        {
            CPPUNIT_ASSERT(m_watcher);

            // Store the initial count; there may already be some watches
            const int initial = m_watcher->GetWatchedPathsCount();

            m_watcher->Add(dir);
            CPPUNIT_ASSERT_EQUAL(initial + 1,
                                 m_watcher->GetWatchedPathsCount());
        }

        void RemoveSingleWatch(wxFileName dir)
        {
            CPPUNIT_ASSERT(m_watcher);

            const int initial = m_watcher->GetWatchedPathsCount();

            m_watcher->Remove(dir);
            CPPUNIT_ASSERT_EQUAL(initial - 1,
                                 m_watcher->GetWatchedPathsCount());
        }

        void WatchTree(const wxFileName& dir)
        {
            CPPUNIT_ASSERT(m_watcher);

            size_t treeitems = 1; // the trunk
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

            CPPUNIT_ASSERT_EQUAL(initial + treeitems, plustree);

            m_watcher->RemoveTree(dir);
            CPPUNIT_ASSERT_EQUAL(initial, m_watcher->GetWatchedPathsCount());

            // Now test the refcount mechanism by watching items more than once
            wxFileName child(dir);
            child.AppendDir("child");
            m_watcher->AddTree(child);
            // Check some watches were added; we don't care about the number
            CPPUNIT_ASSERT(initial < m_watcher->GetWatchedPathsCount());
            // Now watch the whole tree and check that the count is the same
            // as it was the first time, despite also adding 'child' separately
            // Except that in wxMSW this isn't true: each watch will be a
            // single, recursive dir; so fudge the count
            size_t fudge = 0;
#if defined(__WINDOWS__) || defined(wxHAVE_FSEVENTS_FILE_NOTIFICATIONS)
            fudge = 1;
#endif // __WINDOWS__ || wxHAVE_FSEVENTS_FILE_NOTIFICATIONS
            m_watcher->AddTree(dir);
            CPPUNIT_ASSERT_EQUAL(plustree + fudge, m_watcher->GetWatchedPathsCount());
            m_watcher->RemoveTree(child);
            CPPUNIT_ASSERT(initial < m_watcher->GetWatchedPathsCount());
            m_watcher->RemoveTree(dir);
            CPPUNIT_ASSERT_EQUAL(initial, m_watcher->GetWatchedPathsCount());
#if defined(__UNIX__)
            // Finally, test a tree containing internal symlinks
            RmDir(dir);
            GrowTree(dir, true /* test symlinks */);

            // Without the DontFollowLink() call AddTree() would now assert
            // (and without the assert, it would infinitely loop)
            wxFileName fn = dir;
            fn.DontFollowLink();
            CPPUNIT_ASSERT(m_watcher->AddTree(fn));
            CPPUNIT_ASSERT(m_watcher->RemoveTree(fn));

            // Regrow the tree without symlinks, ready for the next test
            RmDir(dir);
            GrowTree(dir, false);
#endif // __UNIX__
        }

        void WatchTreeWithFilespec(const wxFileName& dir)
        {
            CPPUNIT_ASSERT(m_watcher);
            CPPUNIT_ASSERT(dir.DirExists()); // Was built in WatchTree()

            // Store the initial count; there may already be some watches
            const int initial = m_watcher->GetWatchedPathsCount();

            // When we use a filter, both wxMSW and wxGTK implementations set
            // an additional watch for each subdir (+1 for the root dir itself
            // and another +1 for "child").
            // On OS X, if we use FSEvents then we still only have 1 watch.
#ifdef wxHAVE_FSEVENTS_FILE_NOTIFICATIONS
            const size_t treeitems = 1;
#else
            const size_t treeitems = subdirs + 2;
#endif
            m_watcher->AddTree(dir, wxFSW_EVENT_ALL, "*.txt");

            const int plustree = m_watcher->GetWatchedPathsCount();
            CPPUNIT_ASSERT_EQUAL(initial + treeitems, plustree);

            // RemoveTree should try to remove only those files that were added
            m_watcher->RemoveTree(dir);
            CPPUNIT_ASSERT_EQUAL(initial, m_watcher->GetWatchedPathsCount());
        }

        void RemoveAllWatches()
        {
            CPPUNIT_ASSERT(m_watcher);

            m_watcher->RemoveAll();
            CPPUNIT_ASSERT_EQUAL(0, m_watcher->GetWatchedPathsCount());
        }

        virtual void GenerateEvent()
        {
            // We don't use this function for events. Just run the tests

            wxFileName watchdir = EventGenerator::GetWatchDir();
            CPPUNIT_ASSERT(watchdir.DirExists());

            wxFileName treedir(watchdir);
            treedir.AppendDir("treetrunk");
            CPPUNIT_ASSERT(!treedir.DirExists());

            wxFileName singledir(watchdir);
            singledir.AppendDir("single");
            CPPUNIT_ASSERT(!singledir.DirExists());
            CPPUNIT_ASSERT(singledir.Mkdir());

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

        virtual wxFileSystemWatcherEvent ExpectedEvent()
        {
            CPPUNIT_FAIL("Shouldn't be called");

            return wxFileSystemWatcherEvent(wxFSW_EVENT_ERROR);
        }

        virtual void CheckResult()
        {
            // Do nothing. We override this to prevent receiving events in
            // ExpectedEvent()
        }
    };

    TreeTester tester;

// The fs watcher test cases will hang on OS X if we call Run().
// This is likely due to differences between the event loop
// between OS X and the other ports.
#ifdef OSX_EVENT_LOOP_WORKAROUND
    tester.Init();
    tester.GenerateEvent();
#else
    tester.Run();
#endif
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
class NoEventsAfterRemoveEventTester : public EventHandler,
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

    virtual void GenerateEvent()
    {
        m_watcher->Remove(EventGenerator::GetWatchDir());
        CPPUNIT_ASSERT(eg.CreateFile());
    }

    virtual void CheckResult()
    {
        CPPUNIT_ASSERT( m_events.empty() );
    }

    virtual wxFileSystemWatcherEvent ExpectedEvent()
    {
        CPPUNIT_FAIL( "Shouldn't be called" );

        return wxFileSystemWatcherEvent(wxFSW_EVENT_ERROR);
    }

    virtual void Notify()
    {
        SendIdle();
    }
};

} // anonymous namespace

void FileSystemWatcherTestCase::TestNoEventsAfterRemove()
{
    NoEventsAfterRemoveEventTester tester;
    tester.Run();
}

#endif // wxUSE_FSWATCHER
