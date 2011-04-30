///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/colour.cpp
// Purpose:     wxColour unit test
// Author:      Vadim Zeitlin
// Created:     2009-09-19
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/colour.h"
#include "asserthelper.h"

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

// Check the colour components, with and without alpha.
//
// NB: These are macros and not functions to have correct line numbers in case
//     of failure.
#define ASSERT_EQUAL_RGB(c, r, g, b) \
    CPPUNIT_ASSERT_EQUAL( r, (int)c.Red() ); \
    CPPUNIT_ASSERT_EQUAL( g, (int)c.Green() ); \
    CPPUNIT_ASSERT_EQUAL( b, (int)c.Blue() )

#define ASSERT_EQUAL_RGBA(c, r, g, b, a) \
    ASSERT_EQUAL_RGB(c, r, g, b); \
    CPPUNIT_ASSERT_EQUAL( a, (int)c.Alpha() )

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ColourTestCase : public CppUnit::TestCase
{
public:
    ColourTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ColourTestCase );
        CPPUNIT_TEST( GetSetRGB );
        CPPUNIT_TEST( FromString );
    CPPUNIT_TEST_SUITE_END();

    void GetSetRGB();
    void FromString();

    DECLARE_NO_COPY_CLASS(ColourTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ColourTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ColourTestCase, "ColourTestCase" );

void ColourTestCase::GetSetRGB()
{
    wxColour c;
    c.SetRGB(0x123456);

    CPPUNIT_ASSERT_EQUAL( 0x56, (int)c.Red() );
    CPPUNIT_ASSERT_EQUAL( 0x34, (int)c.Green() );
    CPPUNIT_ASSERT_EQUAL( 0x12, (int)c.Blue() );
    CPPUNIT_ASSERT_EQUAL( wxALPHA_OPAQUE, c.Alpha() );

    CPPUNIT_ASSERT_EQUAL( wxColour(0x123456), c );
    CPPUNIT_ASSERT_EQUAL( 0x123456, c.GetRGB() );

    c.SetRGBA(0xaabbccdd);

    CPPUNIT_ASSERT_EQUAL( 0xdd, (int)c.Red() );
    CPPUNIT_ASSERT_EQUAL( 0xcc, (int)c.Green() );
    CPPUNIT_ASSERT_EQUAL( 0xbb, (int)c.Blue() );

    // wxX11 doesn't support alpha at all currently.
#ifndef __WXX11__
    CPPUNIT_ASSERT_EQUAL( 0xaa, (int)c.Alpha() );
#endif // __WXX11__

    // FIXME: at least under wxGTK wxColour ctor doesn't take alpha channel
    //        into account: bug or feature?
    //CPPUNIT_ASSERT_EQUAL( wxColour(0xaabbccdd), c );
    CPPUNIT_ASSERT_EQUAL( 0xbbccdd, c.GetRGB() );
#ifndef __WXX11__
    CPPUNIT_ASSERT_EQUAL( 0xaabbccdd, c.GetRGBA() );
#endif // __WXX11__
}

void ColourTestCase::FromString()
{
    ASSERT_EQUAL_RGB( wxColour("rgb(11, 22, 33)"), 11, 22, 33 );
    ASSERT_EQUAL_RGBA( wxColour("rgba(11, 22, 33, 0.5)"), 11, 22, 33, 128 );
    ASSERT_EQUAL_RGBA( wxColour("rgba( 11, 22, 33, 0.5 )"), 11, 22, 33, 128 );

    ASSERT_EQUAL_RGB( wxColour("#aabbcc"), 0xaa, 0xbb, 0xcc );

    ASSERT_EQUAL_RGB( wxColour("red"), 0xff, 0, 0 );

    wxColour col;
    CPPUNIT_ASSERT( !wxFromString("rgb(1, 2)", &col) );
    CPPUNIT_ASSERT( !wxFromString("rgba(1, 2, 3.456)", &col) );
    CPPUNIT_ASSERT( !wxFromString("rgba(1, 2, 3.456, foo)", &col) );
}

