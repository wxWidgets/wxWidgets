///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/evthandler.cpp
// Purpose:     Test the new event types and wxEvtHandler-methods
// Author:      Peter Most
// Created:     2009-01-24
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Peter Most
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/event.h"

// ----------------------------------------------------------------------------
// test events and their handlers
// ----------------------------------------------------------------------------

const wxEventType EVT_LEGACY = wxNewEventType();

class MyEvent;
wxDEFINE_EVENT( EVT_MYEVENT, MyEvent )

class MyEvent : public wxEvent
{
public:
    MyEvent() : wxEvent(0, EVT_MYEVENT) { }

    virtual wxEvent *Clone() const { return new MyEvent; }
};

class AnotherEvent : public wxEvent
{
};

namespace
{

struct Called
{
    Called() { Reset(); }

    void Reset()
    {
        function =
        functor =
        method =
        smethod = false;
    }

    bool function,
         functor,
         method,
         smethod;
} g_called;

void GlobalOnMyEvent(MyEvent&)
{
    g_called.function = true;
}

void GlobalOnAnotherEvent(AnotherEvent&);

void GlobalOnIdle(wxIdleEvent&)
{
    g_called.function = true;
}

struct MyFunctor
{
    void operator()(MyEvent &) { g_called.functor = true; }
};

struct IdleFunctor
{
    void operator()(wxIdleEvent &) { g_called.functor = true; }
};

class MyHandler : public wxEvtHandler
{
public:
    static void StaticOnMyEvent(MyEvent &) { g_called.smethod = true; }
    static void StaticOnAnotherEvent(AnotherEvent &);
    static void StaticOnIdle(wxIdleEvent&) { g_called.smethod = true; }

    void OnMyEvent(MyEvent&) { g_called.method = true; }
    void OnEvent(wxEvent&) { g_called.method = true; }
    void OnAnotherEvent(AnotherEvent&);
    void OnIdle(wxIdleEvent&) { g_called.method = true; }
};

// we can also handle events in classes not deriving from wxEvtHandler
struct MySink
{
    void OnMyEvent(MyEvent&) { g_called.method = true; }
    void OnIdle(wxIdleEvent&) { g_called.method = true; }
};

// also test event table compilation
class MyClassWithEventTable : public wxEvtHandler
{
public:
    void OnMyEvent(MyEvent&) { g_called.method = true; }
    void OnEvent(wxEvent&) { g_called.method = true; }
    void OnAnotherEvent(AnotherEvent&);
    void OnIdle(wxIdleEvent&) { g_called.method = true; }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyClassWithEventTable, wxEvtHandler)
    EVT_IDLE(MyClassWithEventTable::OnIdle)

    // this shouldn't compile:
    //EVT_IDLE(MyClassWithEventTable::OnAnotherEvent)
END_EVENT_TABLE()

} // anonymous namespace


// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class EvtHandlerTestCase : public CppUnit::TestCase
{
public:
    EvtHandlerTestCase() {}

private:
    CPPUNIT_TEST_SUITE( EvtHandlerTestCase );
        CPPUNIT_TEST( BuiltinConnect );
        CPPUNIT_TEST( LegacyConnect );
#if !wxEVENTS_COMPATIBILITY_2_8
        CPPUNIT_TEST( ConnectFunction );
        CPPUNIT_TEST( ConnectStaticMethod );
        CPPUNIT_TEST( ConnectFunctor );
        CPPUNIT_TEST( ConnectMethod );
        CPPUNIT_TEST( ConnectMethodWithSink );
        CPPUNIT_TEST( ConnectNonHandler );
        CPPUNIT_TEST( StaticConnect );
        CPPUNIT_TEST( InvalidConnect );
#endif // !wxEVENTS_COMPATIBILITY_2_8
    CPPUNIT_TEST_SUITE_END();

    void BuiltinConnect();
    void LegacyConnect();
#if !wxEVENTS_COMPATIBILITY_2_8
    void ConnectFunction();
    void ConnectStaticMethod();
    void ConnectFunctor();
    void ConnectMethod();
    void ConnectMethodWithSink();
    void ConnectNonHandler();
    void StaticConnect();
    void InvalidConnect();
#endif // !wxEVENTS_COMPATIBILITY_2_8


    // these member variables exceptionally don't use "m_" prefix because
    // they're used so many times
    MyHandler handler;
    MyEvent e;

    DECLARE_NO_COPY_CLASS(EvtHandlerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EvtHandlerTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EvtHandlerTestCase, "EvtHandlerTestCase" );

void EvtHandlerTestCase::BuiltinConnect()
{
    handler.Connect(wxEVT_IDLE, wxIdleEventHandler(MyHandler::OnIdle));
    handler.Disconnect(wxEVT_IDLE, wxIdleEventHandler(MyHandler::OnIdle));

    handler.Connect(wxEVT_IDLE, wxIdleEventHandler(MyHandler::OnIdle), NULL, &handler);
    handler.Disconnect(wxEVT_IDLE, wxIdleEventHandler(MyHandler::OnIdle), NULL, &handler);

#if !wxEVENTS_COMPATIBILITY_2_8
    handler.Connect(wxEVT_IDLE, GlobalOnIdle);
    handler.Disconnect(wxEVT_IDLE, GlobalOnIdle);

    IdleFunctor f;
    handler.Connect(wxEVT_IDLE, f);
    handler.Disconnect(wxEVT_IDLE, f);

    handler.Connect(wxEVT_IDLE, &MyHandler::OnIdle);
    handler.Disconnect(wxEVT_IDLE, &MyHandler::OnIdle);

    handler.Connect(wxEVT_IDLE, &MyHandler::StaticOnIdle);
    handler.Disconnect(wxEVT_IDLE, &MyHandler::StaticOnIdle);
#endif // !wxEVENTS_COMPATIBILITY_2_8
}

void EvtHandlerTestCase::LegacyConnect()
{
    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );


    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
}

#if !wxEVENTS_COMPATIBILITY_2_8

void EvtHandlerTestCase::ConnectFunction()
{
    // function tests
    handler.Connect( EVT_MYEVENT, GlobalOnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.function );
    handler.Disconnect( EVT_MYEVENT, GlobalOnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.function ); // check that it was disconnected

    handler.Connect( 0, EVT_MYEVENT, GlobalOnMyEvent );
    handler.Disconnect( 0, EVT_MYEVENT, GlobalOnMyEvent );

    handler.Connect( 0, 0, EVT_MYEVENT, GlobalOnMyEvent );
    handler.Disconnect( 0, 0, EVT_MYEVENT, GlobalOnMyEvent );
}

void EvtHandlerTestCase::ConnectStaticMethod()
{
    // static method tests (this is same as functions but still test it just in
    // case we hit some strange compiler bugs)
    handler.Connect( EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.smethod );
    handler.Disconnect( EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.smethod );

    handler.Connect( 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );

    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
}

void EvtHandlerTestCase::ConnectFunctor()
{
    // generalized functor tests
    MyFunctor functor;

    handler.Connect( EVT_MYEVENT, functor );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.functor );
    handler.Disconnect( EVT_MYEVENT, functor );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.functor );

    handler.Connect( 0, EVT_MYEVENT, functor );
    handler.Disconnect( 0, EVT_MYEVENT, functor );

    handler.Connect( 0, 0, EVT_MYEVENT, functor );
    handler.Disconnect( 0, 0, EVT_MYEVENT, functor );
}

void EvtHandlerTestCase::ConnectMethod()
{
    // class method tests
    handler.Connect( EVT_MYEVENT, &MyHandler::OnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.method );
    handler.Disconnect( EVT_MYEVENT, &MyHandler::OnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.method );

    handler.Connect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent );

    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent );
}

void EvtHandlerTestCase::ConnectMethodWithSink()
{
    handler.Connect( EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Connect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );

    handler.Disconnect( EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
}

void EvtHandlerTestCase::ConnectNonHandler()
{
    // class method tests for class not derived from wxEvtHandler
    MySink sink;

    handler.Connect( EVT_MYEVENT, &MySink::OnMyEvent, NULL, &sink );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.method );
    handler.Disconnect( EVT_MYEVENT, &MySink::OnMyEvent, NULL, &sink );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.method );
}

void EvtHandlerTestCase::StaticConnect()
{
    wxEvtHandler::Connect( &handler, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );

    wxEvtHandler::Disconnect( &handler, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
}

void EvtHandlerTestCase::InvalidConnect()
{
    // these calls shouldn't compile but we unfortunately can't check this
    // automatically, you need to uncomment them manually and test that
    // compilation does indeed fail
    //handler.Connect(EVT_MYEVENT, GlobalOnAnotherEvent);
    //IdleFunctor f; handler.Connect(EVT_MYEVENT, f);
    //handler.Connect(EVT_MYEVENT, &MyHandler::StaticOnAnotherEvent);
    //handler.Connect(EVT_MYEVENT, &MyHandler::OnAnotherEvent);
}

#endif // !wxEVENTS_COMPATIBILITY_2_8
