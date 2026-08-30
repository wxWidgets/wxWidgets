///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/metatest.cpp
// Purpose:     Test template meta-programming constructs
// Author:      Jaakko Salli
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "wx/object.h"
#include "wx/utils.h"
#include "wx/meta/pod.h"
#include "wx/meta/movable.h"

#ifndef wxNO_RTTI
#include <typeinfo>
#endif

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("MetaProgramming::IsPod", "[meta]")
{
    CHECK(wxIsPod<bool>::value);
    CHECK(wxIsPod<signed int>::value);
    CHECK(wxIsPod<double>::value);
    CHECK(wxIsPod<wxObject*>::value);
    CHECK(!wxIsPod<wxObject>::value);
}

TEST_CASE("MetaProgramming::IsMovable", "[meta]")
{
    CHECK(wxIsMovable<bool>::value);
    CHECK(wxIsMovable<signed int>::value);
    CHECK(wxIsMovable<double>::value);
    CHECK(wxIsMovable<wxObject*>::value);
    CHECK(!wxIsMovable<wxObject>::value);
}

TEST_CASE("MetaProgramming::ImplicitConversion", "[meta]")
{
#ifndef wxNO_RTTI
    CHECK(typeid(wxImplicitConversionType<char,int>::value) == typeid(int));
    CHECK(typeid(wxImplicitConversionType<int,unsigned>::value) == typeid(unsigned));
    CHECK(typeid(wxImplicitConversionType<wxLongLong_t,float>::value) == typeid(float));
#endif // !wxNO_RTTI
}

TEST_CASE("MetaProgramming::MinMax", "[meta]")
{
    // test that wxMax(1.1,1) returns float, not long int
    float f = wxMax(1.1f, 1l);
    CHECK( f == 1.1f );

    // test that comparing signed and unsigned correctly returns unsigned: this
    // may seem counterintuitive in this case but this is consistent with the
    // standard C conversions
    CHECK( wxMin(-1, 1u) == 1 );

    CHECK( wxClip(-1.5, -1, 1) == -1. );
    CHECK( wxClip(0, -1, 1) == 0 );
    CHECK( wxClip(2l, -1, 1) == 1 );
}
