///////////////////////////////////////////////////////////////////////////////
// Name:        tests/geometry/size.cpp
// Purpose:     wxSize unit test
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

TEST_CASE("wxSize::Operators", "[size]")
{
    wxSize s1(1,2);
    wxSize s2(3,4);
    wxSize s3;

    s3 = s1 + s2;
    CHECK( s3.GetWidth()==4 );
    CHECK( s3.GetHeight()==6 );
    s3 = s2 - s1;
    CHECK( s3.GetWidth()==2 );
    CHECK( s3.GetHeight()==2 );
    s3 = s1 * 2;
    CHECK( s3.GetWidth()==2 );
    CHECK( s3.GetHeight()==4 );
    s3 = 2 * s1;
    CHECK( s3.GetWidth()==2 );
    CHECK( s3.GetHeight()==4 );
    s3 = s3 / 2;
    CHECK( s3.GetWidth()==1 );
    CHECK( s3.GetHeight()==2 );

    s3 = s2;
    CHECK( s3 != s1 );
    s3 = s1;
    CHECK( s3 == s1 );
    s3 += s2;
    CHECK( s3.GetWidth()==4 );
    CHECK( s3.GetHeight()==6 );
    s3 -= s2;
    CHECK( s3 == s1 );
    s3 *= 2;
    CHECK( s3.GetWidth()==2 );
    CHECK( s3.GetHeight()==4 );
    s3 /= 2;
    CHECK( s3 == s1 );
}
