///////////////////////////////////////////////////////////////////////////////
// Name:        tests/weakref/weakref.cpp
// Purpose:     wxWeakRef<T> unit test
// Author:      Arne Steinarson
// Created:     2008-01-10
// Copyright:   (c) 2007 Arne Steinarson
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

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
// helpers
// --------------------------------------------------------------------------

// Test weak reference to an incomplete type, this should work if the type is
// fully defined before it is used (but currently doesn't, see #11916)
struct ForwardDeclaredClass;
wxWeakRef<ForwardDeclaredClass> g_incompleteWeakRef;

struct ForwardDeclaredClass : wxEvtHandler { };

// A incomplete class that would be defined in other compilation units
struct IncompleteClass;

// --------------------------------------------------------------------------
// tests
// --------------------------------------------------------------------------

TEST_CASE("WeakRef::Declare", "[weakref]")
{
    {
        // Not initializing or initializing with nullptr should work too
        wxWeakRef<wxEvtHandler> wroDef;
        wxWeakRef<wxEvtHandler> wro0(nullptr);

        wxObject o; // Should not work
        wxEvtHandler eh;
        wxObjectTrackable ot;

        // Test declare when T is wxObject
        // wxWeakRef<wxObject> wro1(&o);    // Gives compile time failure
        wxWeakRef<wxEvtHandler> wro2(&eh);
        wxWeakRef<wxObjectTrackable> wro3(&ot);

        CHECK( wro2.get() == &eh );
        CHECK( wro3.get() == &ot );

        // Test accessing wxObject members
        CHECK( !wro2->GetRefData() );
        CHECK( !wro3->GetRefData() );

        wxWeakRef<wxEvtHandler> wreh(&eh);
        wxWeakRef<wxObjectTrackable> wrot(&ot);

        CHECK( wreh.get() == &eh );
        CHECK( wrot.get() == &ot );
    }

    // This test requires a working dynamic_cast<>
#ifndef wxNO_RTTI
    {
        ForwardDeclaredClass fdc;
        g_incompleteWeakRef = &fdc;
        CHECK( g_incompleteWeakRef );
    }

    CHECK( !g_incompleteWeakRef );
#endif // RTTI enabled

    {
        // Construction of a wxWeakRef to an incomplete class should be fine
        wxWeakRef<IncompleteClass> p;

        // Copying should be also OK
        wxCLANG_WARNING_SUPPRESS(self-assign-overloaded)
        p = p;
        wxCLANG_WARNING_RESTORE(self-assign-overloaded)

        // Assigning a raw pointer should cause compile error
#ifdef TEST_INVALID_INCOMPLETE_WEAKREF
        p = static_cast<IncompleteClass*>(0);
#endif

        // Releasing should be OK
    }
}

TEST_CASE("WeakRef::Assign", "[weakref]")
{
    wxWeakRef<wxEvtHandler> wro1;
    wxWeakRef<wxObjectTrackable> wro2;

    {   // Scope for object destruction
        wxEvtHandler eh;
        wxObjectTrackable ot;

        wro1 = &eh;
        wro2 = &ot;

        CHECK( wro1.get() == &eh );
        CHECK( wro2.get() == &ot );
    }

    // Should be reset now
    CHECK( !wro1 );
    CHECK( !wro2 );

    // Explicitly resetting should work too
    wxEvtHandler eh;
    wxObjectTrackable ot;

    wro1 = &eh;
    wro2 = &ot;

    wro1 = nullptr;
    wro2 = nullptr;

    CHECK( !wro1 );
    CHECK( !wro2 );
}

TEST_CASE("WeakRef::AssignWeakRef", "[weakref]")
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

        CHECK( wro1.get() == &eh );
        CHECK( wro2.get() == &ot );
        CHECK( wro3.get() == &eh );
        CHECK( wro4.get() == &ot );

        wro4.Release();
        CHECK( !wro4.get() );
    }

    // Should be reset now
    CHECK( !wro1 );
    CHECK( !wro2 );
}

TEST_CASE("WeakRef::MultiAssign", "[weakref]")
{
    // Object is tracked by several refs
    wxEvtHandler *peh = new wxEvtHandler;

    // Test declare when T is wxObject
    wxWeakRef<wxEvtHandler> wro1(peh);
    wxWeakRef<wxEvtHandler> wro2(peh);

    wxObjectTrackable *pot = new wxObjectTrackable;
    wxWeakRef<wxObjectTrackable> wro3 = pot;
    wxWeakRef<wxObjectTrackable> wro4 = pot;

    CHECK( wro1.get() == peh );
    CHECK( wro2.get() == peh );
    CHECK( wro3.get() == pot );
    CHECK( wro4.get() == pot );

    delete peh;
    delete pot;

    // Should be reset now
    CHECK( !wro1 );
    CHECK( !wro2 );
    CHECK( !wro3 );
    CHECK( !wro4 );
}

TEST_CASE("WeakRef::Cleanup", "[weakref]")
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

        CHECK( eh.GetFirst()==&wro2 );
        CHECK( ots.wxTrackable::GetFirst()==&wro3 );
        CHECK( otd.wxTrackable::GetFirst()==&wro4 );
    }

    // Should be reset now
    CHECK( !eh.GetFirst() );
    CHECK( !ots.wxTrackable::GetFirst() );
    CHECK( !otd.wxTrackable::GetFirst() );
}

TEST_CASE("WeakRef::Delete", "[weakref]")
{
    // Object is tracked by several refs
    wxEvtHandler *peh = new wxEvtHandler;

    // Declared derived type of object and test deleting it
    wxEvtHandlerRef wre(peh);
    wxWeakRef<wxEvtHandler> wro(peh);

    CHECK( wre.get() == peh );
    CHECK( wro.get() == peh );

    delete wre.get();

    CHECK( !wre );
    CHECK( !wro );
}

#ifdef HAVE_DYNAMIC_CAST

TEST_CASE("WeakRef::DynamicRef", "[weakref]")
{
    wxWeakRefDynamic<wxEvtHandler> wro1;
    wxWeakRefDynamic<wxObjectTrackable> wro2;
    wxWeakRefDynamic<wxObjectTrackable> wro3;

    {   // Scope for object destruction
        {
            wxEvtHandler eh;
            wro1 = &eh;
        }

        CHECK( !wro1 );

        wxObjectTrackable otd1;
        wxObjectTrackable otd2;
        wro2 = &otd1;
        wro3 = &otd2;

        CHECK( wro2.get() == &otd1 );
        CHECK( wro3.get() == &otd2 );

        wro3 = wro2;
        CHECK( wro2.get() == &otd1 );
        CHECK( wro3.get() == &otd1 );
    }

    // Should be reset now
    CHECK( !wro2 );
    CHECK( !wro3 );
}

#endif // HAVE_DYNAMIC_CAST
