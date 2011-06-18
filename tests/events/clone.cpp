///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/clone.cpp
// Purpose:     Test wxEvent::Clone() implementation by all event classes
// Author:      Vadim Zeitlin, based on the code by Francesco Montorsi
// Created:     2009-03-22
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif // WX_PRECOMP

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class EventCloneTestCase : public CppUnit::TestCase
{
public:
    EventCloneTestCase() {}

private:
    CPPUNIT_TEST_SUITE( EventCloneTestCase );
        CPPUNIT_TEST( CheckAll );
    CPPUNIT_TEST_SUITE_END();

    void CheckAll();

    DECLARE_NO_COPY_CLASS(EventCloneTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( EventCloneTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( EventCloneTestCase, "EventCloneTestCase" );

void EventCloneTestCase::CheckAll()
{
    // check if event classes implement Clone() correctly
    // NOTE: the check is done against _all_ event classes which are linked to
    //       the executable currently running, which are not necessarily all
    //       wxWidgets event classes.
    const wxClassInfo *ci = wxClassInfo::GetFirst();
    for (; ci; ci = ci->GetNext())
    {
        wxString cn = wxString(ci->GetClassName());
    
        // is this class derived from wxEvent?
        if ( !ci->IsKindOf(CLASSINFO(wxEvent)) ||
             cn == "wxEvent" )
            continue;

        const std::string
            msg = std::string("Event class \"") + 
                  std::string(cn.c_str()) + "\"";

        CPPUNIT_ASSERT_MESSAGE( msg, ci->IsDynamic() );

        wxEvent * const test = wxDynamicCast(ci->CreateObject(),wxEvent);
        CPPUNIT_ASSERT_MESSAGE( msg, test );

        wxEvent * const cloned = test->Clone();
        delete test;

        CPPUNIT_ASSERT_MESSAGE( msg, cloned );
        CPPUNIT_ASSERT_MESSAGE( msg, cloned->GetClassInfo() == ci );

        delete cloned;
    }
}

