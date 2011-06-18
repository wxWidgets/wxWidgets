///////////////////////////////////////////////////////////////////////////////
// Name:        tests/weakref/weakref.cpp
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

// A statically trackable derived wxObject
class wxObjectTrackable : public wxObject, public wxTrackable
{
public:
    // Test member access
    void TestFunc(){ }

    // Make sure this does not clash with wxTrackableBase method
    int GetFirst() { return 0; }
};

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class WeakRefTestCase : public CppUnit::TestCase
{
public:
    WeakRefTestCase() {}

private:
    CPPUNIT_TEST_SUITE( WeakRefTestCase );
        CPPUNIT_TEST( DeclareTest );
        CPPUNIT_TEST( AssignTest );
        CPPUNIT_TEST( AssignWeakRefTest );
        CPPUNIT_TEST( MultiAssignTest );
        CPPUNIT_TEST( CleanupTest );
        CPPUNIT_TEST( DeleteTest );
#ifdef HAVE_DYNAMIC_CAST
        CPPUNIT_TEST( DynamicRefTest );
#endif
    CPPUNIT_TEST_SUITE_END();

    void DeclareTest();
    void AssignTest();
    void AssignWeakRefTest();
    void MultiAssignTest();
    void CleanupTest();
    void DeleteTest();
#ifdef HAVE_DYNAMIC_CAST
    void DynamicRefTest();
#endif

    DECLARE_NO_COPY_CLASS(WeakRefTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( WeakRefTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( WeakRefTestCase, "WeakRefTestCase" );


// Test weak reference to an incomplete type, this should work if the type is
// fully defined before it is used (but currently doesn't, see #11916)
struct ForwardDeclaredClass;
wxWeakRef<ForwardDeclaredClass> g_incompleteWeakRef;

struct ForwardDeclaredClass : wxEvtHandler { };

void WeakRefTestCase::DeclareTest()
{
    {
        // Not initializing or initializing with NULL should work too
        //
        // FIXME-VC6: but it doesn't with VC6, see comment in wx/weakref.h
#ifndef __VISUALC6__
        wxWeakRef<wxEvtHandler> wroDef;
        wxWeakRef<wxEvtHandler> wro0(NULL);
#endif // __VISUALC6__

        wxObject o; // Should not work
        wxEvtHandler eh;
        wxObjectTrackable ot;

        // Test declare when T is wxObject
        // wxWeakRef<wxObject> wro1(&o);    // Gives compile time failure
        wxWeakRef<wxEvtHandler> wro2(&eh);
        wxWeakRef<wxObjectTrackable> wro3(&ot);

        CPPUNIT_ASSERT( wro2.get() == &eh );
        CPPUNIT_ASSERT( wro3.get() == &ot );

        // Test accessing wxObject members
        CPPUNIT_ASSERT( !wro2->GetRefData() );
        CPPUNIT_ASSERT( !wro3->GetRefData() );


        wxWeakRef<wxEvtHandler> wreh(&eh);
        wxWeakRef<wxObjectTrackable> wrot(&ot);

        CPPUNIT_ASSERT( wreh.get() == &eh );
        CPPUNIT_ASSERT( wrot.get() == &ot );
    }

    // This test requires a working dynamic_cast<>
#ifndef wxNO_RTTI
    {
        ForwardDeclaredClass fdc;
        g_incompleteWeakRef = &fdc;
        CPPUNIT_ASSERT( g_incompleteWeakRef );
    }

    CPPUNIT_ASSERT( !g_incompleteWeakRef );
#endif // RTTI enabled
}

void WeakRefTestCase::AssignTest()
{
    wxWeakRef<wxEvtHandler> wro1;
    wxWeakRef<wxObjectTrackable> wro2;

    {   // Scope for object destruction
        wxEvtHandler eh;
        wxObjectTrackable ot;

        wro1 = &eh;
        wro2 = &ot;

        CPPUNIT_ASSERT( wro1.get() == &eh );
        CPPUNIT_ASSERT( wro2.get() == &ot );
    }

    // Should be reset now
    CPPUNIT_ASSERT( !wro1 );
    CPPUNIT_ASSERT( !wro2 );

    // Explicitly resetting should work too
    //
    // FIXME-VC6: as above, it doesn't work with VC6, see wx/weakref.h
#ifndef __VISUALC6__
    wxEvtHandler eh;
    wxObjectTrackable ot;

    wro1 = &eh;
    wro2 = &ot;

    wro1 = NULL;
    wro2 = NULL;

    CPPUNIT_ASSERT( !wro1 );
    CPPUNIT_ASSERT( !wro2 );
#endif // __VISUALC6__
}

void WeakRefTestCase::AssignWeakRefTest()
{
    // Test declare when T is wxObject
    wxWeakRef<wxEvtHandler> wro1;
    wxWeakRef<wxObjectTrackable> wro2;

    {   // Scope for object destruction
        wxEvtHandler eh;
        wxObjectTrackable ot;
        wxWeakRef<wxEvtHandler> wro3;
        wxWeakRef<wxObjectTrackable> wro4;

        wro1 = &eh;
        wro2 = &ot;
        wro3 = wro1;
        wro4 = wro2;

        CPPUNIT_ASSERT( wro1.get() == &eh );
        CPPUNIT_ASSERT( wro2.get() == &ot );
        CPPUNIT_ASSERT( wro3.get() == &eh );
        CPPUNIT_ASSERT( wro4.get() == &ot );

        wro4.Release();
        CPPUNIT_ASSERT( !wro4.get() );
    }

    // Should be reset now
    CPPUNIT_ASSERT( !wro1 );
    CPPUNIT_ASSERT( !wro2 );
}

void WeakRefTestCase::MultiAssignTest()
{
    // Object is tracked by several refs
    wxEvtHandler *peh = new wxEvtHandler;

    // Test declare when T is wxObject
    wxWeakRef<wxEvtHandler> wro1(peh);
    wxWeakRef<wxEvtHandler> wro2(peh);

    wxObjectTrackable *pot = new wxObjectTrackable;
    wxWeakRef<wxObjectTrackable> wro3 = pot;
    wxWeakRef<wxObjectTrackable> wro4 = pot;

    CPPUNIT_ASSERT( wro1.get() == peh );
    CPPUNIT_ASSERT( wro2.get() == peh );
    CPPUNIT_ASSERT( wro3.get() == pot );
    CPPUNIT_ASSERT( wro4.get() == pot );

    delete peh;
    delete pot;

    // Should be reset now
    CPPUNIT_ASSERT( !wro1 );
    CPPUNIT_ASSERT( !wro2 );
    CPPUNIT_ASSERT( !wro3 );
    CPPUNIT_ASSERT( !wro4 );
}

void WeakRefTestCase::CleanupTest()
{
    // Make sure that trackable objects have no left over tracker nodes after use.
    // This time the references goes out of scope before the objects.
    wxEvtHandler eh;
    wxObjectTrackable ots;
    wxObjectTrackable otd;

    {   // Scope for object destruction
        wxWeakRef<wxEvtHandler> wro1;
        wxWeakRef<wxEvtHandler> wro2;
        wxWeakRef<wxObjectTrackable> wro3;
        wxWeakRef<wxObjectTrackable> wro4;

        wro1 = &eh;
        wro2 = &eh; // Has two tracker nodes now
        wro3 = &ots;
        wro4 = &otd;

        // Access members of reffed object
        wro3->TestFunc();

        CPPUNIT_ASSERT( eh.GetFirst()==&wro2 );
        CPPUNIT_ASSERT( ots.wxTrackable::GetFirst()==&wro3 );
        CPPUNIT_ASSERT( otd.wxTrackable::GetFirst()==&wro4 );
    }

    // Should be reset now
    CPPUNIT_ASSERT( !eh.GetFirst() );
    CPPUNIT_ASSERT( !ots.wxTrackable::GetFirst() );
    CPPUNIT_ASSERT( !otd.wxTrackable::GetFirst() );
}

void WeakRefTestCase::DeleteTest()
{
    // Object is tracked by several refs
    wxEvtHandler *peh = new wxEvtHandler;

    // Declared derived type of object and test deleting it
    wxEvtHandlerRef wre(peh);
    wxWeakRef<wxEvtHandler> wro(peh);

    CPPUNIT_ASSERT( wre.get() == peh );
    CPPUNIT_ASSERT( wro.get() == peh );

    delete wre.get();

    CPPUNIT_ASSERT( !wre );
    CPPUNIT_ASSERT( !wro );
}

#ifdef HAVE_DYNAMIC_CAST

void WeakRefTestCase::DynamicRefTest()
{
    wxWeakRefDynamic<wxEvtHandler> wro1;
    wxWeakRefDynamic<wxObjectTrackable> wro2;
    wxWeakRefDynamic<wxObjectTrackable> wro3;

    {   // Scope for object destruction
        {
            wxEvtHandler eh;
            wro1 = &eh;
        }

        CPPUNIT_ASSERT( !wro1 );

        wxObjectTrackable otd1;
        wxObjectTrackable otd2;
        wro2 = &otd1;
        wro3 = &otd2;

        CPPUNIT_ASSERT( wro2.get() == &otd1 );
        CPPUNIT_ASSERT( wro3.get() == &otd2 );

        wro3 = wro2;
        CPPUNIT_ASSERT( wro2.get() == &otd1 );
        CPPUNIT_ASSERT( wro3.get() == &otd1 );
    }

    // Should be reset now
    CPPUNIT_ASSERT( !wro2 );
    CPPUNIT_ASSERT( !wro3 );
}

#endif // HAVE_DYNAMIC_CAST
