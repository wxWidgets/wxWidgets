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

class MyEvent : public wxEvent
{
};

wxDEFINE_EVENT( EVT_EVENT, MyEvent )

// An arbitrary functor:

class MyFunctor
{
    public:
        void operator () ( MyEvent & )
        { }

        bool operator == ( const MyFunctor & ) const
        { return ( true ); }
};


class MyHandler : public wxEvtHandler
{
    public:
        void handleMethod( MyEvent & )
        { }

        static void handleFunction( MyEvent & )
        { }

        void handleEvent( wxEvent & )
        { }

};

void EvtHandlerTestCase::TestConnectCompilation()
{
    // Test that connecting the 'legacy' events still compiles:

    MyHandler handler;

    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent, NULL, &handler );



    handler.Connect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );
    handler.Connect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );
    handler.Connect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );

    handler.Disconnect( EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );
    handler.Disconnect( 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );
    handler.Disconnect( 0, 0, EVT_LEGACY, (wxObjectEventFunction)&MyHandler::handleEvent );

    // Call (and therefore instantiate) all Connect() variants to detect template
    // errors:

#if !wxEVENTS_COMPATIBILITY_2_8

    handler.Connect( EVT_EVENT, &MyHandler::handleFunction );
    handler.Connect( 0, EVT_EVENT, &MyHandler::handleFunction );
    handler.Connect( 0, 0, EVT_EVENT, &MyHandler::handleFunction );

    handler.Disconnect( EVT_EVENT, &MyHandler::handleFunction );
    handler.Disconnect( 0, EVT_EVENT, &MyHandler::handleFunction );
    handler.Disconnect( 0, 0, EVT_EVENT, &MyHandler::handleFunction );



    handler.Connect( EVT_EVENT, &MyHandler::handleMethod );
    handler.Connect( 0, EVT_EVENT, &MyHandler::handleMethod );
    handler.Connect( 0, 0, EVT_EVENT, &MyHandler::handleMethod );

    handler.Disconnect( EVT_EVENT, &MyHandler::handleMethod );
    handler.Disconnect( 0, EVT_EVENT, &MyHandler::handleMethod );
    handler.Disconnect( 0, 0, EVT_EVENT, &MyHandler::handleMethod );



    handler.Connect( EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    handler.Connect( 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    handler.Connect( 0, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );

    handler.Disconnect( EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    handler.Disconnect( 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    handler.Disconnect( 0, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );



    wxEvtHandler::Connect( &handler, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    wxEvtHandler::Connect( &handler, 0, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );

    wxEvtHandler::Disconnect( &handler, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );
    wxEvtHandler::Disconnect( &handler, 0, 0, EVT_EVENT, &MyHandler::handleMethod, NULL, &handler );



    MyFunctor functor;

    handler.Connect( EVT_EVENT, functor );
    handler.Connect( 0, EVT_EVENT, functor );
    handler.Connect( 0, 0, EVT_EVENT, functor );

    handler.Disconnect( EVT_EVENT, functor );
    handler.Disconnect( 0, EVT_EVENT, functor );
    handler.Disconnect( 0, 0, EVT_EVENT, functor );
#endif
}

void EvtHandlerTestCase::TestEventFunctorCompare()
{
//#if !wxEVENTS_COMPATIBILITY_2_8
//    MyHandler handler1;
//    wxEventFunctor *connectFunctor = wxNewEventFunctor( EVT_EVENT, &MyHandler::handleMethod, &handler1 );
//    wxEventFunctor *disconnectFunctor = wxNewEventFunctor( EVT_EVENT, &MyHandler::handleMethod, &handler1 );
//    wxEventFunctor *nullFunctor = wxNewEventFunctor( EVT_EVENT, &MyHandler::handleMethod );
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


