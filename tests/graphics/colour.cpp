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

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{
    // by default colour components values are output incorrectly because they
    // are unsigned chars, define a small helper struct which formats them in
    // a more useful way
    struct ColourChannel
    {
        ColourChannel(unsigned char value) : m_value(value) { }

        unsigned char m_value;
    };

    std::ostream& operator<<(std::ostream& os, const ColourChannel& cc)
    {
        os.width(2);
        os.fill('0');
        os << static_cast<int>(cc.m_value);
        return os;
    }
} // anonymous namespace

// this operator is needed to use CPPUNIT_ASSERT_EQUAL with wxColour objects
std::ostream& operator<<(std::ostream& os, const wxColour& c)
{
    os << std::hex << std::noshowbase
       << "("
       << ColourChannel(c.Red()) << ", "
       << ColourChannel(c.Green()) << ", "
       << ColourChannel(c.Blue());

    if ( const unsigned char a = c.Alpha() )
    {
        os << ", " << ColourChannel(c.Alpha());
    }

    os << ")";

    return os;
}

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
    CPPUNIT_TEST_SUITE_END();

    void GetSetRGB();

    DECLARE_NO_COPY_CLASS(ColourTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ColourTestCase );

// also include in it's own registry so that these tests can be run alone
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
    CPPUNIT_ASSERT_EQUAL( 0xaa, (int)c.Alpha() );

    // FIXME: at least under wxGTK wxColour ctor doesn't take alpha channel
    //        into account: bug or feature?
    //CPPUNIT_ASSERT_EQUAL( wxColour(0xaabbccdd), c );
    CPPUNIT_ASSERT_EQUAL( 0xbbccdd, c.GetRGB() );
    CPPUNIT_ASSERT_EQUAL( 0xaabbccdd, c.GetRGBA() );
}

