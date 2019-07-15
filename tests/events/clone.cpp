///////////////////////////////////////////////////////////////////////////////
// Name:        tests/events/clone.cpp
// Purpose:     Test wxEvent::Clone() implementation by all event classes
// Author:      Vadim Zeitlin, based on the code by Francesco Montorsi
// Created:     2009-03-22
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
    #include "wx/timer.h"
#endif // WX_PRECOMP

TEST_CASE("EventClone", "[wxEvent][clone]")
{
    // Dummy timer needed just to create a wxTimerEvent.
    wxTimer dummyTimer;

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

        INFO("Event class \"" << cn << "\"");

        wxEvent* test;
        if ( ci->IsDynamic() )
        {
            test = wxDynamicCast(ci->CreateObject(),wxEvent);
        }
        else if ( cn == "wxTimerEvent" )
        {
            test = new wxTimerEvent(dummyTimer);
        }
        else
        {
            FAIL("Can't create objects of type " + cn);
            continue;
        }

        REQUIRE( test );

        wxEvent * const cloned = test->Clone();
        delete test;

        REQUIRE( cloned );
        CHECK( cloned->GetClassInfo() == ci );

        delete cloned;
    }
}

