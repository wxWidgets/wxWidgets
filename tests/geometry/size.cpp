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

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SizeTestCase : public CppUnit::TestCase
{
public:
    SizeTestCase() { }

private:
    CPPUNIT_TEST_SUITE( SizeTestCase );
        CPPUNIT_TEST( Operators );
    CPPUNIT_TEST_SUITE_END();

    void Operators();

    wxDECLARE_NO_COPY_CLASS(SizeTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SizeTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SizeTestCase, "SizeTestCase" );

void SizeTestCase::Operators()
{
    wxSize s1(1,2);
    wxSize s2(3,4);
    wxSize s3;

    s3 = s1 + s2;
    CPPUNIT_ASSERT( s3.GetWidth()==4 );
    CPPUNIT_ASSERT( s3.GetHeight()==6 );
    s3 = s2 - s1;
    CPPUNIT_ASSERT( s3.GetWidth()==2 );
    CPPUNIT_ASSERT( s3.GetHeight()==2 );
    s3 = s1 * 2;
    CPPUNIT_ASSERT( s3.GetWidth()==2 );
    CPPUNIT_ASSERT( s3.GetHeight()==4 );
    s3 = 2 * s1;
    CPPUNIT_ASSERT( s3.GetWidth()==2 );
    CPPUNIT_ASSERT( s3.GetHeight()==4 );
    s3 = s3 / 2;
    CPPUNIT_ASSERT( s3.GetWidth()==1 );
    CPPUNIT_ASSERT( s3.GetHeight()==2 );

    s3 = s2;
    CPPUNIT_ASSERT( s3 != s1 );
    s3 = s1;
    CPPUNIT_ASSERT( s3 == s1 );
    s3 += s2;
    CPPUNIT_ASSERT( s3.GetWidth()==4 );
    CPPUNIT_ASSERT( s3.GetHeight()==6 );
    s3 -= s2;
    CPPUNIT_ASSERT( s3 == s1 );
    s3 *= 2;
    CPPUNIT_ASSERT( s3.GetWidth()==2 );
    CPPUNIT_ASSERT( s3.GetHeight()==4 );
    s3 /= 2;
    CPPUNIT_ASSERT( s3 == s1 );
}
