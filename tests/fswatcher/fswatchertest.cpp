///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fswatcher/fswatchertest.cpp
// Purpose:     wxFileSystemWatcher unit test
// Author:      Bartosz Bekier
// Created:     2009-06-11
// RCS-ID:      $Id$
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

#include "wx/evtloop.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/stdpaths.h"
#include "wx/fswatcher.h"

#include "testfile.h"

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

        // FIXME-VC6: using non-static Rmdir() results in ICE
        CPPUNIT_ASSERT( wxFileName::Rmdir(dir.GetFullPath(), wxPATH_RMDIR_RECURSIVE) );
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

    EventHandler() :
        eg(EventGenerator::Get()), m_loop(0), m_count(0), m_watcher(0)
    {
        m_loop = new wxEventLoop();
        Connect(wxEVT_IDLE, wxIdleEventHandler(EventHandler::OnIdle));
        Connect(wxEVT_FSWATCHER, wxFileSystemWatcherEventHandler(
                                            EventHandler::OnFileSystemEvent));
    }

    virtual ~EventHandler()
    {
        delete m_watcher;
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
        CPPUNIT_ASSERT(m_watcher->Add(dir, wxFSW_EVENT_ALL));

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

        WX_ASSERT_EQUAL_MESSAGE
        (
            (
                "Extra events received, first is of type %x, for path=\"%s\","
                "last is of type %x, path=\"%s\"",
                e->GetChangeType(),
                e->GetPath().GetFullPath(),
                m_events.back()->GetChangeType(),
                m_events.back()->GetPath().GetFullPath()
            ),
            1, m_events.size()
        );

        // this is our "reference event"
        const wxFileSystemWatcherEvent expected = ExpectedEvent();

        CPPUNIT_ASSERT_EQUAL( expected.GetChangeType(), e->GetChangeType() );

        CPPUNIT_ASSERT_EQUAL((int)wxEVT_FSWATCHER, e->GetEventType());

        // XXX this needs change
        CPPUNIT_ASSERT_EQUAL(wxEVT_CATEGORY_UNKNOWN, e->GetEventCategory());

        CPPUNIT_ASSERT_EQUAL(expected.GetPath(), e->GetPath());
        CPPUNIT_ASSERT_EQUAL(expected.GetNewPath(), e->GetNewPath());
    }

    virtual void GenerateEvent() = 0;

    virtual wxFileSystemWatcherEvent ExpectedEvent() = 0;


protected:
    EventGenerator& eg;
    wxEventLoopBase* m_loop;    // loop reference
    int m_count;                // idle events count

    wxFileSystemWatcher* m_watcher;
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

        // kqueue-based implementation doesn't collapse create/delete pairs in
        // renames and doesn't detect neither modifications nor access to the
        // files reliably currently so disable these tests
        //
        // FIXME: fix the code and reenable them
#ifndef wxHAS_KQUEUE
        CPPUNIT_TEST( TestEventRename );
        CPPUNIT_TEST( TestEventModify );

        // MSW implementation doesn't detect file access events currently
#ifndef __WXMSW__
        CPPUNIT_TEST( TestEventAccess );
#endif // __WXMSW__
#endif // !wxHAS_KQUEUE

        CPPUNIT_TEST( TestNoEventsAfterRemove );
    CPPUNIT_TEST_SUITE_END();

    void TestEventCreate();
    void TestEventDelete();
    void TestEventRename();
    void TestEventModify();
    void TestEventAccess();

    void TestNoEventsAfterRemove();

    DECLARE_NO_COPY_CLASS(FileSystemWatcherTestCase)
};

// the test currently hangs under OS X for some reason and this prevents tests
// ran by buildbot from completing so disable it until someone has time to
// debug it
//
// FIXME: debug and fix this!
#ifndef __WXOSX__
// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FileSystemWatcherTestCase );
#endif

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FileSystemWatcherTestCase,
                                        "FileSystemWatcherTestCase" );

void FileSystemWatcherTestCase::setUp()
{
    wxLog::AddTraceMask(wxTRACE_FSWATCHER);
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
