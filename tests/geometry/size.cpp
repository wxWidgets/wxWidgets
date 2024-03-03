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

#include "asserthelper.h"

TEST_CASE("wxSize::Operators", "[size]")
{
    wxSize s1(1,2);
    wxSize s2(3,4);
    wxSize s3;

    s3 = s1 + s2;
    CHECK( s3 == wxSize(4, 6) );
    s3 = s2 - s1;
    CHECK( s3 == wxSize(2, 2) );
    s3 = s1 * 2;
    CHECK( s3 == wxSize(2, 4) );
    s3 = 2 * s1;
    CHECK( s3 == wxSize(2, 4) );
    s3 = s3 / 2;
    CHECK( s3 == wxSize(1, 2) );

    s3 = s2;
    CHECK( s3 != s1 );
    s3 = s1;
    CHECK( s3 == s1 );
    s3 += s2;
    CHECK( s3 == wxSize(4, 6) );
    s3 -= s2;
    CHECK( s3 == s1 );
    s3 *= 2;
    CHECK( s3 == wxSize(2, 4) );
    s3 /= 2;
    CHECK( s3 == s1 );

    CHECK( wxSize(6, 9) / 1.5 == wxSize(4, 6) );
}

TEST_CASE("wxSize::Functions", "[size]")
{
    CHECK( wxSize(10, 10).IsAtLeast(wxDefaultSize) );
    CHECK( wxSize(10, 10).IsAtLeast(wxSize()) );
    CHECK( wxSize(10, 10).IsAtLeast(wxSize(10, 5)) );
    CHECK( wxSize(10, 10).IsAtLeast(wxSize(10, 10)) );

    CHECK_FALSE( wxSize(10, 10).IsAtLeast(wxSize(11, 10)) );
    CHECK_FALSE( wxSize(10, 10).IsAtLeast(wxSize(10, 11)) );
    CHECK_FALSE( wxSize(10, 10).IsAtLeast(wxSize(11, 11)) );
    CHECK_FALSE( wxDefaultSize.IsAtLeast(wxSize()) );
}
