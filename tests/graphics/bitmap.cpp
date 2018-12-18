///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/bitmap.cpp
// Purpose:     wxBitmap unit test
// Author:      Vadim Zeitlin
// Created:     2010-03-29
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef wxHAS_RAW_BITMAP

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/bitmap.h"
#include "wx/rawbmp.h"
#include "wx/dcmemory.h"
#if wxUSE_GRAPHICS_CONTEXT
#include "wx/graphics.h"
#endif // wxUSE_GRAPHICS_CONTEXT

#define ASSERT_EQUAL_RGB(c, r, g, b) \
    CPPUNIT_ASSERT_EQUAL( r, (int)c.Red() ); \
    CPPUNIT_ASSERT_EQUAL( g, (int)c.Green() ); \
    CPPUNIT_ASSERT_EQUAL( b, (int)c.Blue() )

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class BitmapTestCase : public CppUnit::TestCase
{
public:
    BitmapTestCase() { }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( BitmapTestCase );
        CPPUNIT_TEST( Mask );
        CPPUNIT_TEST( OverlappingBlit );
    CPPUNIT_TEST_SUITE_END();

    void Mask();
    void OverlappingBlit();

    wxBitmap m_bmp;

    wxDECLARE_NO_COPY_CLASS(BitmapTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BitmapTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BitmapTestCase, "BitmapTestCase" );

void BitmapTestCase::setUp()
{
    m_bmp.Create(10, 10);

    wxMemoryDC dc(m_bmp);;
    dc.SetBackground(*wxWHITE);
    dc.Clear();

    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(4, 4, 2, 2);

    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(0, 0, 10, 10);
    dc.DrawLine(10, 0, 0, 10);
}

void BitmapTestCase::tearDown()
{
    m_bmp = wxNullBitmap;
}

void BitmapTestCase::Mask()
{
    wxMask *mask = new wxMask(m_bmp, *wxBLACK);
    m_bmp.SetMask(mask);

    // copying masks should work
    wxMask *mask2 = NULL;
    REQUIRE_NOTHROW(mask2 = new wxMask(*mask));
    m_bmp.SetMask(mask2);
}

void BitmapTestCase::OverlappingBlit()
{
    m_bmp.SetMask( NULL );

    // Clear to white.
    {
    wxMemoryDC dc(m_bmp);
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if ( gc )
    {
        gc->SetAntialiasMode(wxANTIALIAS_NONE);
    }
#endif // wxUSE_GRAPHICS_CONTEXT

    dc.SetBackground( *wxWHITE );
    dc.Clear();

    // Draw red line across the top.

    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(0, 0, 10, 0);

    // Scroll down one line.

    dc.Blit( 0, 1, 10, 9, &dc, 0, 0 );
    } // Select the bitmap out of the memory DC before using it directly.
    // Now, lines 0 and 1 should be red, lines 2++ should still be white.

    if ( m_bmp.GetDepth() == 32 )
    {
        wxAlphaPixelData npd( m_bmp );
        CPPUNIT_ASSERT_MESSAGE( "Expected raw pixels to not be NULL", npd );
        wxAlphaPixelData::Iterator it( npd );

        ASSERT_EQUAL_RGB( it, 255, 0, 0 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 0, 0 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 255, 255 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 255, 255 );
    }
    else
    {
        wxNativePixelData npd( m_bmp );
        wxNativePixelData::Iterator it( npd );
        if ( !npd )
        {
            CPPUNIT_FAIL( "Raw access to bitmap data unavailable" );
        }
        else
        {
            ASSERT_EQUAL_RGB( it, 255, 0, 0 );
            it.OffsetY( npd, 1 );
            ASSERT_EQUAL_RGB( it, 255, 0, 0 );
            it.OffsetY( npd, 1 );
            ASSERT_EQUAL_RGB( it, 255, 255, 255 );
            it.OffsetY( npd, 1 );
            ASSERT_EQUAL_RGB( it, 255, 255, 255 );
        }
    }
}

#endif //wxHAS_RAW_BITMAP
