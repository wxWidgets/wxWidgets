///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/misctests.cpp
// Purpose:     test miscellaneous GUI functions
// Author:      Vadim Zeitlin
// Created:     2008-09-22
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif // !PCH

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MiscGUIFuncsTestCase : public CppUnit::TestCase
{
public:
    MiscGUIFuncsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MiscGUIFuncsTestCase );
        CPPUNIT_TEST( DisplaySize );
    CPPUNIT_TEST_SUITE_END();

    void DisplaySize();

    DECLARE_NO_COPY_CLASS(MiscGUIFuncsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MiscGUIFuncsTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MiscGUIFuncsTestCase, "MiscGUIFuncsTestCase" );

void MiscGUIFuncsTestCase::DisplaySize()
{
    // test that different (almost) overloads return the same results
    int w, h;
    wxDisplaySize(&w, &h);
    wxSize sz = wxGetDisplaySize();

    CPPUNIT_ASSERT_EQUAL( w, sz.x );
    CPPUNIT_ASSERT_EQUAL( h, sz.y );

    // test that passing NULL works as expected, e.g. doesn't crash
    wxDisplaySize(NULL, NULL);
    wxDisplaySize(&w, NULL);
    wxDisplaySize(NULL, &h);

    CPPUNIT_ASSERT_EQUAL( w, sz.x );
    CPPUNIT_ASSERT_EQUAL( h, sz.y );

    // test that display PPI is something reasonable
    sz = wxGetDisplayPPI();
    CPPUNIT_ASSERT( sz.x < 1000 && sz.y < 1000 );
}

