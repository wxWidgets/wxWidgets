///////////////////////////////////////////////////////////////////////////////
// Name:        tests/vectors/vectors.cpp
// Purpose:     wxVector<T> unit test
// Author:      Vaclav Slavik
// Created:     2007-07-07
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vaclav Slavik
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

#include "wx/vector.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class VectorsTestCase : public CppUnit::TestCase
{
public:
    VectorsTestCase() {}

private:
    CPPUNIT_TEST_SUITE( VectorsTestCase );
        CPPUNIT_TEST( PushTest );
    CPPUNIT_TEST_SUITE_END();

    void PushTest();

    DECLARE_NO_COPY_CLASS(VectorsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VectorsTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VectorsTestCase, "VectorsTestCase" );

void VectorsTestCase::PushTest()
{
    wxVector<int> v;

    CPPUNIT_ASSERT( v.size() == 0 );
    v.push_back(1);
    CPPUNIT_ASSERT( v.size() == 1 );
    v.push_back(2);
    CPPUNIT_ASSERT( v.size() == 2 );
    v.push_back(42);
    CPPUNIT_ASSERT( v.size() == 3 );

    CPPUNIT_ASSERT( v[0] == 1 );
    CPPUNIT_ASSERT( v[1] == 2 );
    CPPUNIT_ASSERT( v[2] == 42 );
}
