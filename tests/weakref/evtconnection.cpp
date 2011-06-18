///////////////////////////////////////////////////////////////////////////////
// Name:        tests/weakref/evtconnection.cpp
// Purpose:     wxWeakRef<T> unit test
// Author:      Arne Steinarson
// Created:     2008-01-10
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Arne Steinarson
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/event.h"
#include "wx/weakref.h"

static int gs_value = 0; // Increased by 1 by first object and 0x10000 by 2nd object

static wxObject *gs_psrc1;
static wxObject *gs_psrc2;

// We need some event types
const wxEventType wxEVT_TEST = wxNewEventType(),
                  wxEVT_TEST1 = wxNewEventType(),
                  wxEVT_TEST2 = wxNewEventType();

class wxTestEvent : public wxEvent
{
public:
    wxTestEvent(wxEventType type = wxEVT_TEST) : wxEvent(0, type) { }
    virtual wxEvent *Clone() const { return new wxTestEvent(GetEventType()); }
};

class wxTestSink : public wxEvtHandler
{
public:
    void OnTestEvent(wxEvent& evt)
    {
        if ( evt.GetEventObject() == gs_psrc1 )
            gs_value += 1;
        else if ( evt.GetEventObject() == gs_psrc2 )
            gs_value += 0x10000;
    }

    void OnTestEvent1(wxEvent& )
    {
        gs_value += 0x00100;
    }

    void OnTestEvent2(wxEvent&)
    {
        gs_value += 0x01000000;
    }
};



// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class EvtConnectionTestCase : public CppUnit::TestCase
{
public:
    EvtConnectionTestCase() {}

private:
    CPPUNIT_TEST_SUITE( EvtConnectionTestCase );
        CPPUNIT_TEST( SinkTest );
        CPPUNIT_TEST( SourceDestroyTest );
        CPPUNIT_TEST( MultiConnectionTest );
    CPPUNIT_TEST_SUITE_END();

    void SinkTest();
    void SourceDestroyTest();
    void MultiConnectionTest();

    DECLARE_NO_COPY_CLASS(EvtConnectionTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EvtConnectionTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EvtConnectionTestCase, "EvtConnectionTestCase" );


// Helpers
void DoConnect( wxEvtHandler& eh1, wxEvtHandler& eh2, wxTestSink& ts ){
    eh1.Connect(wxEVT_TEST, (wxObjectEventFunction)&wxTestSink::OnTestEvent,
                NULL, &ts);
    eh2.Connect(wxEVT_TEST, (wxObjectEventFunction) &wxTestSink::OnTestEvent,
                NULL, &ts);
}

void DoDisconnect( wxEvtHandler& eh1, wxEvtHandler& eh2, wxTestSink& ts ){
    eh1.Disconnect(wxEVT_TEST, (wxObjectEventFunction) &wxTestSink::OnTestEvent,
                NULL, &ts);
    eh2.Disconnect(wxEVT_TEST, (wxObjectEventFunction) &wxTestSink::OnTestEvent,
                NULL, &ts);
}


void EvtConnectionTestCase::SinkTest()
{
    // Let the sink be destroyed before the sources

    // An event used below
    wxTestEvent evt;

    // Connect two event handlers to one sink
    wxEvtHandler eh1, eh2;
    gs_psrc1 = &eh1;
    gs_psrc2 = &eh2;

    {
        wxTestSink ts;
        CPPUNIT_ASSERT( !ts.GetFirst() );
        DoConnect(eh1, eh2, ts);

        DoDisconnect(eh1, eh2, ts);

        DoConnect(eh1, eh2, ts);

        // Fire events
        evt.SetEventObject(&eh1);
        eh1.ProcessEvent(evt);
        evt.SetEventObject(&eh2);
        eh2.ProcessEvent(evt);

        // Make sure they were processed correctly
        CPPUNIT_ASSERT_EQUAL( 0x00010001, gs_value );
    }

    // Fire events again, should be no sink connected now
    gs_value = 0;
    evt.SetEventObject(&eh1);
    eh1.ProcessEvent( evt );
    evt.SetEventObject(&eh2);
    eh2.ProcessEvent( evt );

    // Make sure no processing happened
    CPPUNIT_ASSERT_EQUAL( 0, gs_value );
}

void EvtConnectionTestCase::SourceDestroyTest()
{
    // Let the sources be destroyed before the sink
    wxTestSink ts;
    wxTestEvent evt;
    {
        wxEvtHandler eh1;
        {
            CPPUNIT_ASSERT( !ts.GetFirst() );

            // Connect two event handlers to one sink
            wxEvtHandler eh2;
            gs_psrc1 = &eh1;
            gs_psrc2 = &eh2;
            DoConnect( eh1, eh2, ts );

            // Fire events
            gs_value = 0;
            evt.SetEventObject(&eh1);
            eh1.ProcessEvent( evt );
            evt.SetEventObject(&eh2);
            eh2.ProcessEvent( evt );

            // Make sure they were processed correctly
            CPPUNIT_ASSERT_EQUAL( 0x00010001, gs_value );
        }

        gs_value = 0;
        evt.SetEventObject(&eh1);
        eh1.ProcessEvent( evt );

        // Make sure still connected
        CPPUNIT_ASSERT_EQUAL( 0x00000001, gs_value );
    }
    CPPUNIT_ASSERT( !ts.GetFirst() );
}

void EvtConnectionTestCase::MultiConnectionTest()
{
    // events used below
    wxTestEvent evt;
    wxTestEvent evt1(wxEVT_TEST1);
    wxTestEvent evt2(wxEVT_TEST2);

    // One source
    wxEvtHandler eh1;
    evt.SetEventObject(&eh1);
    gs_psrc1 = NULL;
    gs_psrc2 = &eh1;

    {
        // ...and one sink
        wxTestSink ts;

        eh1.Connect(wxEVT_TEST, (wxObjectEventFunction)&wxTestSink::OnTestEvent,
                    NULL, &ts);
        eh1.Connect(wxEVT_TEST1, (wxObjectEventFunction)&wxTestSink::OnTestEvent1,
                    NULL, &ts);
        eh1.Connect(wxEVT_TEST2, (wxObjectEventFunction)&wxTestSink::OnTestEvent2,
                    NULL, &ts);

        // Generate events
        gs_value = 0;
        eh1.ProcessEvent(evt);
        eh1.ProcessEvent(evt1);
        eh1.ProcessEvent(evt2);
        CPPUNIT_ASSERT( gs_value==0x01010100 );

        {
            // Declare weak references to the objects (using same list)
            wxEvtHandlerRef re(&eh1), rs(&ts);
        }
        // And now destroyed

        eh1.Disconnect(wxEVT_TEST, (wxObjectEventFunction)&wxTestSink::OnTestEvent,
                       NULL, &ts);
        eh1.ProcessEvent(evt);
        eh1.ProcessEvent(evt1);
        eh1.ProcessEvent(evt2);
        CPPUNIT_ASSERT_EQUAL( 0x02010200, gs_value );
    }

    // No connection should be left now
    gs_value = 0;
    eh1.ProcessEvent(evt);
    eh1.ProcessEvent(evt1);
    eh1.ProcessEvent(evt2);

    // Nothing should have been done
    CPPUNIT_ASSERT_EQUAL( 0, gs_value );
}

