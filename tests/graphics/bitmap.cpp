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
    REQUIRE( r == (int)c.Red() ); \
    REQUIRE( g == (int)c.Green() ); \
    REQUIRE( b == (int)c.Blue() )

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("BitmapTestCase::Mask", "[bitmap][mask]")
{
    wxBitmap bmp(10, 10);
    {
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxWHITE);
        dc.Clear();
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(4, 4, 2, 2);
        dc.SetPen(*wxRED_PEN);
        dc.DrawLine(0, 0, 10, 10);
        dc.DrawLine(10, 0, 0, 10);
    }

    wxMask *mask = new wxMask(bmp, *wxBLACK);
    bmp.SetMask(mask);
    REQUIRE(bmp.GetMask() == mask);

    // copying masks should work
    wxMask *mask2 = NULL;
    REQUIRE_NOTHROW(mask2 = new wxMask(*mask));
    bmp.SetMask(mask2);
    REQUIRE(bmp.GetMask() == mask2);
}

TEST_CASE("BitmapTestCase::OverlappingBlit", "[bitmap][blit]")
{
    wxBitmap bmp(10, 10);
    {
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxWHITE);
        dc.Clear();
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(4, 4, 2, 2);
        dc.SetPen(*wxRED_PEN);
        dc.DrawLine(0, 0, 10, 10);
        dc.DrawLine(10, 0, 0, 10);
    }
    REQUIRE(bmp.GetMask() == NULL);

    // Clear to white.
    {
        wxMemoryDC dc(bmp);
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

    if ( bmp.GetDepth() == 32 )
    {
        wxAlphaPixelData npd( bmp );
        REQUIRE( npd );
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
        wxNativePixelData npd( bmp );
        REQUIRE( npd );
        wxNativePixelData::Iterator it( npd );

        ASSERT_EQUAL_RGB( it, 255, 0, 0 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 0, 0 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 255, 255 );
        it.OffsetY( npd, 1 );
        ASSERT_EQUAL_RGB( it, 255, 255, 255 );
    }
}

#endif //wxHAS_RAW_BITMAP
