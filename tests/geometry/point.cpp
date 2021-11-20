///////////////////////////////////////////////////////////////////////////////
// Name:        tests/geometry/point.cpp
// Purpose:     wxPoint unit test
// Author:      Wlodzimierz ABX Skiba
// Created:     2004-12-14
// Copyright:   (c) 2004 wxWindows
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif // WX_PRECOMP

#include "wx/math.h"

TEST_CASE("wxPoint::Operators", "[point]")
{
    wxPoint p1(1,2);
    wxPoint p2(6,3);
    wxPoint p3(7,5);
    wxPoint p4(5,1);
    wxPoint p5 = p2 + p1;
    wxPoint p6 = p2 - p1;
    CHECK( p3.x == p5.x );
    CHECK( p3.y == p5.y );
    CHECK( p4.x == p6.x );
    CHECK( p4.y == p6.y );
    CHECK( p3 == p5 );
    CHECK( p4 == p6 );
    CHECK( p3 != p4 );
    p5 = p2; p5 += p1;
    p6 = p2; p6 -= p1;
    CHECK( p3 == p5 );
    CHECK( p4 == p6 );
    wxSize s(p1.x,p1.y);
    p5 = p2; p5 = p2 + s;
    p6 = p2; p6 = p2 - s;
    CHECK( p3 == p5 );
    CHECK( p4 == p6 );
    p5 = p2; p5 = s + p2;
    p6 = p2; p6 = s - p2;
    CHECK( p3 == p5 );
    CHECK( p4 == -p6 );
    p5 = p2; p5 += s;
    p6 = p2; p6 -= s;
    CHECK( p3 == p5 );
    CHECK( p4 == p6 );
}

TEST_CASE("wxRealPoint::Operators", "[point]")
{
    wxRealPoint p1(1.2,3.4);
    wxRealPoint p2(8.7,5.4);
    wxRealPoint p3(9.9,8.8);
    wxRealPoint p4(7.5,2.0);
    wxRealPoint p5 = p2 + p1;
    wxRealPoint p6 = p2 - p1;
    CHECK( p3.x == Approx(p5.x) );
    CHECK( p3.y == Approx(p5.y) );
    CHECK( p4.x == Approx(p6.x) );
    CHECK( p4.y == Approx(p6.y) );
    CHECK( p3.x != Approx(p4.x) );
}
