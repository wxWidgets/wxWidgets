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

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/event.h"


// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class EvtHandlerTestCase : public CppUnit::TestCase
{
public:
    EvtHandlerTestCase() {}

private:
    CPPUNIT_TEST_SUITE( EvtHandlerTestCase );
        CPPUNIT_TEST( TestConnectCompilation );
        CPPUNIT_TEST( TestEventFunctorCompare );
    CPPUNIT_TEST_SUITE_END();

    void TestConnectCompilation();
    void TestEventFunctorCompare();

    DECLARE_NO_COPY_CLASS(EvtHandlerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EvtHandlerTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EvtHandlerTestCase, "EvtHandlerTestCase" );

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

class MyFunctor
{
public:
    void operator()(MyEvent &) { g_called.functor = true; }

    bool operator==(const MyFunctor &) const { return true; }
};

class MyHandler : public wxEvtHandler
{
public:
    void OnMyEvent(MyEvent&) { g_called.method = true; }

    static void StaticOnMyEvent(MyEvent &) { g_called.smethod = true; }

    void OnEvent(wxEvent &) { }
    void OnAnotherEvent(AnotherEvent&) { }
};

} // anonymous namespace

void EvtHandlerTestCase::TestConnectCompilation()
{
    // Test that connecting the 'legacy' events still compiles:

    MyHandler handler;
    MyEvent e;

    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent, NULL, &handler );



    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::OnEvent );

    // Call (and therefore instantiate) all Connect() variants to detect template
    // errors:

#if !wxEVENTS_COMPATIBILITY_2_8
    handler.Connect( EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( g_called.smethod );
    handler.Disconnect( EVT_MYEVENT, &MyHandler::StaticOnMyEvent );

    g_called.Reset();
    handler.ProcessEvent(e);
    CPPUNIT_ASSERT( !g_called.smethod ); // check that it was disconnected

    handler.Connect( 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );

    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::StaticOnMyEvent );



    handler.Connect( EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Connect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent );

    handler.Disconnect( EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent );



    handler.Connect( EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Connect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Connect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );

    handler.Disconnect( EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Disconnect( 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );


    wxEvtHandler::Connect( &handler, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );

    wxEvtHandler::Disconnect( &handler, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, 0, EVT_MYEVENT, &MyHandler::OnMyEvent, NULL, &handler );



    MyFunctor functor;

    handler.Connect( EVT_MYEVENT, functor );
    handler.Connect( 0, EVT_MYEVENT, functor );
    handler.Connect( 0, 0, EVT_MYEVENT, functor );

    handler.Disconnect( EVT_MYEVENT, functor );
    handler.Disconnect( 0, EVT_MYEVENT, functor );
    handler.Disconnect( 0, 0, EVT_MYEVENT, functor );

    // these calls shouldn't compile but we unfortunately can't check this
    // automatically, you need to uncomment them manually and test that
    // compilation does indeed fail
    //handler.Connect(EVT_MYEVENT, &MyHandler::OnAnotherEvent, NULL, &handler);
#endif // !wxEVENTS_COMPATIBILITY_2_8
}

void EvtHandlerTestCase::TestEventFunctorCompare()
{
//#if !wxEVENTS_COMPATIBILITY_2_8
//    MyHandler handler1;
//    wxEventFunctor *connectFunctor = wxNewEventFunctor( EVT_MYEVENT, &MyHandler::OnMyEvent, &handler1 );
//    wxEventFunctor *disconnectFunctor = wxNewEventFunctor( EVT_MYEVENT, &MyHandler::OnMyEvent, &handler1 );
//    wxEventFunctor *nullFunctor = wxNewEventFunctor( EVT_MYEVENT, &MyHandler::OnMyEvent );
//   
//    CPPUNIT_ASSERT( connectFunctor->Matches( *disconnectFunctor ));
//    CPPUNIT_ASSERT( disconnectFunctor->Matches( *connectFunctor ));
//
//    CPPUNIT_ASSERT( connectFunctor->Matches( *nullFunctor ));
//    CPPUNIT_ASSERT( nullFunctor->Matches( *connectFunctor ));
//
//    CPPUNIT_ASSERT( disconnectFunctor->Matches( *nullFunctor ));
//    CPPUNIT_ASSERT( nullFunctor->Matches( *disconnectFunctor ));
//#endif
}


