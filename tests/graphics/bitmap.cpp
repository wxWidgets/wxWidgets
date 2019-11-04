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

#include "testimage.h"

#define ASSERT_EQUAL_RGB(c, r, g, b) \
    CHECK( (int)r == (int)c.Red() ); \
    CHECK( (int)g == (int)c.Green() ); \
    CHECK( (int)b == (int)c.Blue() )

#define ASSERT_EQUAL_RGBA(c, r, g, b, a) \
    CHECK( (int)r == (int)c.Red() ); \
    CHECK( (int)g == (int)c.Green() ); \
    CHECK( (int)b == (int)c.Blue() ); \
    CHECK( (int)a == (int)c.Alpha() )

#ifdef __WXMSW__
// Support for iteration over 32 bpp 0RGB bitmaps
typedef wxPixelFormat<unsigned char, 32, 2, 1, 0> wxNative32PixelFormat;
typedef wxPixelData<wxBitmap, wxNative32PixelFormat> wxNative32PixelData;
#endif // __WXMSW__
#ifdef __WXOSX__
// 32 bpp xRGB bitmaps are native ones
typedef wxNativePixelData wxNative32PixelData;
#endif // __WXOSX__

// ----------------------------------------------------------------------------
// tests
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

static wxBitmap GetMask(int w, int h)
{
    wxBitmap bmask(w, h, 1);
    {
        wxMemoryDC dc(bmask);
#if wxUSE_GRAPHICS_CONTEXT
        wxGraphicsContext* gc = dc.GetGraphicsContext();
        if ( gc )
        {
            gc->SetAntialiasMode(wxANTIALIAS_NONE);
        }
#endif // wxUSE_GRAPHICS_CONTEXT
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();
        dc.SetPen(*wxWHITE_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawRectangle(0, 0, w, h / 2);
    }

    return bmask;
}

TEST_CASE("BitmapTestCase::DrawNonAlphaWithMask", "[bitmap][draw][nonalpha][withmask]")
{
    const int w = 16;
    const int h = 16;

    // Mask
    wxBitmap bmask = GetMask(w, h);

    const wxColour clrLeft(*wxBLUE);
    const wxColour clrRight(*wxRED);
    const wxColour clrBg(*wxCYAN);

    // Bitmap with mask to be drawn
    wxBitmap bmp(w, h, 24);
    {
        wxMemoryDC dc(bmp);
        dc.SetPen(wxPen(clrLeft));
        dc.SetBrush(wxBrush(clrLeft));
        dc.DrawRectangle(0, 0, w / 2, h);
        dc.SetPen(wxPen(clrRight));
        dc.SetBrush(wxBrush(clrRight));
        dc.DrawRectangle(w / 2, 0, w / 2, h);
    }
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);
    bmp.SetMask(new wxMask(bmask));
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != NULL);

    // Drawing the bitmap using mask
    {
        wxBitmap bmpOut(w, h, 24);
        {
            wxMemoryDC dc(bmpOut);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), true);
        }

        // Check pixels
        wxNativePixelData data(bmpOut);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // drawn area - left side
        ASSERT_EQUAL_RGB(p, clrLeft.Red(), clrLeft.Green(), clrLeft.Blue());
        p.OffsetX(data, w / 2); // drawn area - right side
        ASSERT_EQUAL_RGB(p, clrRight.Red(), clrRight.Green(), clrRight.Blue());
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // masked area - left side
        ASSERT_EQUAL_RGB(p, clrBg.Red(), clrBg.Green(), clrBg.Blue());
        p.OffsetX(data, w / 2); // masked area - right side
        ASSERT_EQUAL_RGB(p, clrBg.Red(), clrBg.Green(), clrBg.Blue());
    }

    // Drawing the bitmap not using mask
    {
        wxBitmap bmpOut(w, h, 24);
        {
            wxMemoryDC dc(bmpOut);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), false);
        }

        // Check pixels
        wxNativePixelData data(bmpOut);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // left upper side
        ASSERT_EQUAL_RGB(p, clrLeft.Red(), clrLeft.Green(), clrLeft.Blue());
        p.OffsetX(data, w / 2); // right upper side
        ASSERT_EQUAL_RGB(p, clrRight.Red(), clrRight.Green(), clrRight.Blue());
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_RGB(p, clrLeft.Red(), clrLeft.Green(), clrLeft.Blue());
        p.OffsetX(data, w / 2); // right lower side - same colour as upper
        ASSERT_EQUAL_RGB(p, clrRight.Red(), clrRight.Green(), clrRight.Blue());
    }
}

TEST_CASE("BitmapTestCase::DrawAlpha", "[bitmap][draw][alpha]")
{
    const int w = 16;
    const int h = 16;

    const wxColour clrFg(*wxCYAN);
    const wxColour clrBg(*wxGREEN);
    const unsigned char alpha = 92;

#if defined(__WXMSW__) || defined(__WXOSX__)
    // premultiplied values
    const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
    const wxColour clrFgAlpha(clrFg);
#endif // __WXMSW__ || __WXOSX__

    // Bitmap to be drawn
    wxBitmap bmp(w, h, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
    bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
    {
        wxAlphaPixelData data(bmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        for ( int y = 0; y < h; y++)
        {
            wxAlphaPixelData::Iterator rowStart = p;
            for ( int x = 0; x < w; x++, ++p )
            {
                if ( x < w / 2 )
                {   // opaque
                    p.Red() = clrFg.Red();
                    p.Green() = clrFg.Green();
                    p.Blue() = clrFg.Blue();
                    p.Alpha() = 255;
                }
                else
                {   // with transparency
                    p.Red() = clrFgAlpha.Red();
                    p.Green() = clrFgAlpha.Green();
                    p.Blue() = clrFgAlpha.Blue();
                    p.Alpha() = alpha;
                }
            }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }
    REQUIRE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);

    // Drawing the bitmap on 24 bpp RGB target
    wxBitmap bmpOut24(w, h, 24);
    REQUIRE_FALSE(bmpOut24.HasAlpha());
    {
        wxMemoryDC dc(bmpOut24);
        dc.SetBackground(wxBrush(clrBg));
        dc.Clear();

        dc.DrawBitmap(bmp, wxPoint(0, 0), true);
    }
    REQUIRE_FALSE(bmpOut24.HasAlpha());

    // Check pixels
    wxNativePixelData data24(bmpOut24);
    REQUIRE(data24);
    wxNativePixelData::Iterator p1(data24);
    p1.OffsetY(data24, h / 2);
    p1.OffsetX(data24, w / 4); // left side is opaque
    ASSERT_EQUAL_RGB(p1, clrFg.Red(), clrFg.Green(), clrFg.Blue());
    p1.OffsetX(data24, w / 2); // right side is with alpha
#if defined(__WXMSW__) || defined(__WXOSX__)
    // premultiplied values
    ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha)  + 127) / 255);
#else
    ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                         (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                         (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // __WXMSW__ || __WXOSX__

#if defined(__WXMSW__) || defined(__WXOSX__)
    // Drawing the bitmap on 32 bpp xRGB target
    wxBitmap bmpOut32(w, h, 32);
    REQUIRE_FALSE(bmpOut32.HasAlpha());
    {
        wxMemoryDC dc(bmpOut32);
        dc.SetBackground(wxBrush(clrBg));
        dc.Clear();

        dc.DrawBitmap(bmp, wxPoint(0, 0), true);
    }
    REQUIRE(bmpOut32.GetDepth() == 32);
    REQUIRE_FALSE(bmpOut32.HasAlpha());

    // Check pixels
    wxNative32PixelData data32(bmpOut32);
    REQUIRE(data32);
    wxNative32PixelData::Iterator p2(data32);
    p2.OffsetY(data32, h / 2);
    p2.OffsetX(data32, w / 4); // left side is opaque
    ASSERT_EQUAL_RGB(p2, clrFg.Red(), clrFg.Green(), clrFg.Blue());
    p2.OffsetX(data32, w / 2); // right side is with alpha
    // premultiplied values
    ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // __WXMSW__ || __WXOSX__
}

TEST_CASE("BitmapTestCase::DrawAlphaWithMask", "[bitmap][draw][alpha][withmask]")
{
    const int w = 16;
    const int h = 16;

    // Mask
    wxBitmap bmask = GetMask(w, h);

    const wxColour clrFg(*wxCYAN);
    const wxColour clrBg(*wxGREEN);
    const unsigned char alpha = 92;

#if defined(__WXMSW__) || defined(__WXOSX__)
     // premultiplied values
     const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
     const wxColour clrFgAlpha(clrFg);
#endif // __WXMSW__ || __WXOSX__

     // Bitmap with mask to be drawn
     wxBitmap bmp(w, h, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
     bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
     {
        wxAlphaPixelData data(bmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        for ( int y = 0; y < h; y++)
        {
            wxAlphaPixelData::Iterator rowStart = p;
            for ( int x = 0; x < w; x++, ++p )
            {
                if ( x < w / 2 )
                {   // opaque
                    p.Red() = clrFg.Red();
                    p.Green() = clrFg.Green();
                    p.Blue() = clrFg.Blue();
                    p.Alpha() = 255;
                }
                else
                {   // with transparency
                    p.Red() = clrFgAlpha.Red();
                    p.Green() = clrFgAlpha.Green();
                    p.Blue() = clrFgAlpha.Blue();
                    p.Alpha() = alpha;
                }
            }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }
    REQUIRE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);
    bmp.SetMask(new wxMask(bmask));
    REQUIRE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != NULL);

    // Drawing the bitmap on 24 bpp RGB target using mask
    {
        wxBitmap bmpOut24(w, h, 24);
        REQUIRE_FALSE(bmpOut24.HasAlpha());
        {
            wxMemoryDC dc(bmpOut24);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), true);
        }
        REQUIRE_FALSE(bmpOut24.HasAlpha());

        // Check pixels
        wxNativePixelData data24(bmpOut24);
        REQUIRE(data24);
        wxNativePixelData::Iterator p1(data24);
        p1.OffsetY(data24, h / 4);
        wxNativePixelData::Iterator rowStart1 = p1;
        p1.OffsetX(data24, w / 4); // drawn area - left side opaque
        ASSERT_EQUAL_RGB(p1, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p1.OffsetX(data24, w / 2); // drawn area - right side with alpha
#if defined(__WXMSW__) || defined(__WXOSX__)
        // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // __WXMSW__ || __WXOSX__
        p1 = rowStart1;
        p1.OffsetY(data24, h / 2);
        p1.OffsetX(data24, w / 4); // masked area - left side
        ASSERT_EQUAL_RGB(p1, clrBg.Red(), clrBg.Green(), clrBg.Blue());
        p1.OffsetX(data24, w / 2); // masked area - right side
        ASSERT_EQUAL_RGB(p1, clrBg.Red(), clrBg.Green(), clrBg.Blue());
    }

    // Drawing the bitmap on 24 bpp RGB target not using mask
    {
        wxBitmap bmpOut24(w, h, 24);
        REQUIRE_FALSE(bmpOut24.HasAlpha());
        {
            wxMemoryDC dc(bmpOut24);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), false);
        }
        REQUIRE_FALSE(bmpOut24.HasAlpha());

        // Check pixels
        wxNativePixelData data24(bmpOut24);
        REQUIRE(data24);
        wxNativePixelData::Iterator p1(data24);
        p1.OffsetY(data24, h / 4);
        wxNativePixelData::Iterator rowStart1 = p1;
        p1.OffsetX(data24, w / 4); // left upper side opaque
        ASSERT_EQUAL_RGB(p1, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p1.OffsetX(data24, w / 2); // right upper side with alpha
#if defined(__WXMSW__) || defined(__WXOSX__)
        // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // __WXMSW__ || __WXOSX__
        p1 = rowStart1;
        p1.OffsetY(data24, h / 2);
        p1.OffsetX(data24, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_RGB(p1, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p1.OffsetX(data24, w / 2); // right lower side - same colour as upper
#if defined(__WXMSW__) || defined(__WXOSX__)
         // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // __WXMSW__ || __WXOSX__
    }

#if defined(__WXMSW__) || defined(__WXOSX__)
    // Drawing the bitmap on 32 bpp xRGB target using mask
    {
        wxBitmap bmpOut32(w, h, 32);
        REQUIRE_FALSE(bmpOut32.HasAlpha());
        {
            wxMemoryDC dc(bmpOut32);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), true);
        }
        REQUIRE(bmpOut32.GetDepth() == 32);
        REQUIRE_FALSE(bmpOut32.HasAlpha());

        // Check pixels
        wxNative32PixelData data32(bmpOut32);
        REQUIRE(data32);
        wxNative32PixelData::Iterator p2(data32);
        p2.OffsetY(data32, h / 4);
        wxNative32PixelData::Iterator rowStart2 = p2;
        p2.OffsetX(data32, w / 4); // drawn area - left side opaque
        ASSERT_EQUAL_RGB(p2, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p2.OffsetX(data32, w / 2); // drawn area - right side with alpha
        // premultiplied values
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
        p2 = rowStart2;
        p2.OffsetY(data32, h / 2);
        p2.OffsetX(data32, w / 4); // masked area - left side
        ASSERT_EQUAL_RGB(p2, clrBg.Red(), clrBg.Green(), clrBg.Blue());
        p2.OffsetX(data32, w / 2); // masked area - right side
        ASSERT_EQUAL_RGB(p2, clrBg.Red(), clrBg.Green(), clrBg.Blue());
    }

    // Drawing the bitmap on 32 bpp xRGB target not using mask
    {
        wxBitmap bmpOut32(w, h, 32);
        REQUIRE_FALSE(bmpOut32.HasAlpha());
        {
            wxMemoryDC dc(bmpOut32);
            dc.SetBackground(wxBrush(clrBg));
            dc.Clear();

            dc.DrawBitmap(bmp, wxPoint(0, 0), false);
        }
        REQUIRE(bmpOut32.GetDepth() == 32);
        REQUIRE_FALSE(bmpOut32.HasAlpha());

        // Check pixels
        wxNative32PixelData data32(bmpOut32);
        REQUIRE(data32);
        wxNative32PixelData::Iterator p2(data32);
        p2.OffsetY(data32, h / 4);
        wxNative32PixelData::Iterator rowStart2 = p2;
        p2.OffsetX(data32, w / 4); // left upper side opaque
        ASSERT_EQUAL_RGB(p2, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p2.OffsetX(data32, w / 2); // right upper side with alpha
        // premultiplied values
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
        p2 = rowStart2;
        p2.OffsetY(data32, h / 2);
        p2.OffsetX(data32, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_RGB(p2, clrFg.Red(), clrFg.Green(), clrFg.Blue());
        p2.OffsetX(data32, w / 2); // right lower side - same colour as upper
        // premultiplied values
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
    }
#endif // __WXMSW__ || __WXOSX__
}

TEST_CASE("BitmapTestCase::SubBitmapNonAlpha", "[bitmap][subbitmap][nonalpha]")
{
    const int w = 16;
    const int h = 16;

    const wxColour clrTopLeft(*wxBLUE);
    const wxColour clrTopRight(*wxRED);
    const wxColour clrBottomLeft(*wxGREEN);
    const wxColour clrBottomRight(*wxCYAN);

    // Bitmap
    wxBitmap bmp(w, h, 24);
    {
        wxMemoryDC dc(bmp);
        dc.SetPen(wxPen(clrTopLeft));
        dc.SetBrush(wxBrush(clrTopLeft));
        dc.DrawRectangle(0, 0, w / 2, h / 2);
        dc.SetPen(wxPen(clrTopRight));
        dc.SetBrush(wxBrush(clrTopRight));
        dc.DrawRectangle(w / 2, 0, w / 2, h / 2);
        dc.SetPen(wxPen(clrBottomLeft));
        dc.SetBrush(wxBrush(clrBottomLeft));
        dc.DrawRectangle(0, h / 2, w / 2, h / 2);
        dc.SetPen(wxPen(clrBottomRight));
        dc.SetBrush(wxBrush(clrBottomRight));
        dc.DrawRectangle(w / 2, h / 2, w / 2, h / 2);
    }
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);

    // Get sub bitmap
    wxBitmap subBmp = bmp.GetSubBitmap(wxRect(w/4, h/4, w/2, h/2));
    // Check sub bitmap attributes
    REQUIRE(subBmp.GetWidth() == w/2);
    REQUIRE(subBmp.GetHeight() == h/2);
    REQUIRE(subBmp.GetDepth() == bmp.GetDepth());
    REQUIRE(subBmp.HasAlpha() == bmp.HasAlpha());
    REQUIRE((subBmp.GetMask() == NULL) == (bmp.GetMask() == NULL));

    const int w2 = w / 2;
    const int h2 = h / 2;
    // Check sub bitmap pixels
    wxNativePixelData data(subBmp);
    REQUIRE(data);
    wxNativePixelData::Iterator p(data);
    p.OffsetY(data, h2 / 4);
    wxNativePixelData::Iterator rowStart = p;
    p.OffsetX(data, w2 / 4); // top-left point
    ASSERT_EQUAL_RGB(p, clrTopLeft.Red(), clrTopLeft.Green(), clrTopLeft.Blue());
    p.OffsetX(data, w2 / 2); // top-right point
    ASSERT_EQUAL_RGB(p, clrTopRight.Red(), clrTopRight.Green(), clrTopRight.Blue());
    p = rowStart;
    p.OffsetY(data, h2 / 2);
    p.OffsetX(data, w2 / 4); // bottom-left point
    ASSERT_EQUAL_RGB(p, clrBottomLeft.Red(), clrBottomLeft.Green(), clrBottomLeft.Blue());
    p.OffsetX(data, w2 / 2); // bottom-right point
    ASSERT_EQUAL_RGB(p, clrBottomRight.Red(), clrBottomRight.Green(), clrBottomRight.Blue());
}

TEST_CASE("BitmapTestCase::SubBitmapNonAlphaWithMask", "[bitmap][subbitmap][nonalpha][withmask]")
{
    const int w = 16;
    const int h = 16;

    // Mask
    wxBitmap bmpMask = GetMask(w, h);

    const wxColour clrTopLeft(*wxBLUE);
    const wxColour clrTopRight(*wxRED);
    const wxColour clrBottomLeft(*wxGREEN);
    const wxColour clrBottomRight(*wxCYAN);

    // Bitmap
    wxBitmap bmp(w, h, 24);
    {
        wxMemoryDC dc(bmp);
        dc.SetPen(wxPen(clrTopLeft));
        dc.SetBrush(wxBrush(clrTopLeft));
        dc.DrawRectangle(0, 0, w / 2, h / 2);
        dc.SetPen(wxPen(clrTopRight));
        dc.SetBrush(wxBrush(clrTopRight));
        dc.DrawRectangle(w / 2, 0, w / 2, h / 2);
        dc.SetPen(wxPen(clrBottomLeft));
        dc.SetBrush(wxBrush(clrBottomLeft));
        dc.DrawRectangle(0, h / 2, w / 2, h / 2);
        dc.SetPen(wxPen(clrBottomRight));
        dc.SetBrush(wxBrush(clrBottomRight));
        dc.DrawRectangle(w / 2, h / 2, w / 2, h / 2);
    }
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);
    bmp.SetMask(new wxMask(bmpMask));
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != NULL);

    // Get sub bitmap
    wxBitmap subBmp = bmp.GetSubBitmap(wxRect(w/4, h/4, w/2, h/2));
    const int w2 = w / 2;
    const int h2 = h / 2;
    // Check sub bitmap attributes
    REQUIRE(subBmp.GetWidth() == w2);
    REQUIRE(subBmp.GetHeight() == h2);
    REQUIRE(subBmp.GetDepth() == bmp.GetDepth());
    REQUIRE(subBmp.HasAlpha() == bmp.HasAlpha());
    REQUIRE((subBmp.GetMask() == NULL) == (bmp.GetMask() == NULL));

    // Check sub bitmap pixels
    {
        wxNativePixelData data(subBmp);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_RGB(p, clrTopLeft.Red(), clrTopLeft.Green(), clrTopLeft.Blue());
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_RGB(p, clrTopRight.Red(), clrTopRight.Green(), clrTopRight.Blue());
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_RGB(p, clrBottomLeft.Red(), clrBottomLeft.Green(), clrBottomLeft.Blue());
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_RGB(p, clrBottomRight.Red(), clrBottomRight.Green(), clrBottomRight.Blue());
    }

    // Check sub bitmap mask
    wxColour maskClrTopLeft;
    wxColour maskClrTopRight;
    wxColour maskClrBottomLeft;
    wxColour maskClrBottomRight;
    // Fetch sample original mask pixels
    {
        wxNativePixelData data(bmpMask);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // top-left point
        maskClrTopLeft = wxColour(p.Red(), p.Green(), p.Blue());
        p.OffsetX(data, w / 2); // top-right point
        maskClrTopRight = wxColour(p.Red(), p.Green(), p.Blue());
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // bottom-left point
        maskClrBottomLeft = wxColour(p.Red(), p.Green(), p.Blue());
        p.OffsetX(data, w / 2); // bottom-right point
        maskClrBottomRight = wxColour(p.Red(), p.Green(), p.Blue());
    }
    CHECK(maskClrTopLeft == *wxWHITE);
    CHECK(maskClrTopRight == *wxWHITE);
    CHECK(maskClrBottomLeft == *wxBLACK);
    CHECK(maskClrBottomRight == *wxBLACK);

    wxBitmap subBmpMask = subBmp.GetMask()->GetBitmap();
    // Check sub bitmap mask attributes
    REQUIRE(subBmpMask.GetWidth() == subBmp.GetWidth());
    REQUIRE(subBmpMask.GetHeight() == subBmp.GetHeight());
#if !defined(__WXOSX__)
    REQUIRE(subBmpMask.GetDepth() == 1);
#endif // !__WXOSX__
    REQUIRE_FALSE(subBmpMask.HasAlpha());
    REQUIRE_FALSE(subBmpMask.GetMask());
    // Check sub bitmap mask pixels
    {
        wxNativePixelData data(subBmpMask);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_RGB(p, maskClrTopLeft.Red(), maskClrTopLeft.Green(), maskClrTopLeft.Blue());
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_RGB(p, maskClrTopRight.Red(), maskClrTopRight.Green(), maskClrTopRight.Blue());
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_RGB(p, maskClrBottomLeft.Red(), maskClrBottomLeft.Green(), maskClrBottomLeft.Blue());
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_RGB(p, maskClrBottomRight.Red(), maskClrBottomRight.Green(), maskClrBottomRight.Blue());
    }
}

TEST_CASE("BitmapTestCase::SubBitmapAlphaWithMask", "[bitmap][subbitmap][alpha][withmask]")
{
    const int w = 16;
    const int h = 16;

    // Mask
    wxBitmap bmpMask = GetMask(w, h);

    // Bitmap
    const wxColour clrLeft(*wxCYAN);
    const unsigned char alpha = 92;

#if defined(__WXMSW__) || defined(__WXOSX__)
    // premultiplied values
    const wxColour clrRight(((clrLeft.Red() * alpha) + 127) / 255, ((clrLeft.Green() * alpha) + 127) / 255, ((clrLeft.Blue() * alpha) + 127) / 255, alpha);
#else
    const wxColour clrRight(clrLeft.Red(), clrLeft.Green(), clrLeft.Blue(), alpha);
#endif // __WXMSW__ || __WXOSX__

    wxBitmap bmp(w, h, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
    bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
    {
        wxAlphaPixelData data(bmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        for ( int y = 0; y < h; y++)
        {
            wxAlphaPixelData::Iterator rowStart = p;
            for ( int x = 0; x < w; x++, ++p )
            {
                if ( x < w / 2 )
                {   // opaque
                    p.Red() = clrLeft.Red();
                    p.Green() = clrLeft.Green();
                    p.Blue() = clrLeft.Blue();
                    p.Alpha() = clrLeft.Alpha();
                }
                else
                {   // with transparency
                    p.Red() = clrRight.Red();
                    p.Green() = clrRight.Green();
                    p.Blue() = clrRight.Blue();
                    p.Alpha() = clrRight.Alpha();
                }
            }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }
    REQUIRE(bmp.HasAlpha());
    REQUIRE(!bmp.GetMask());
    bmp.SetMask(new wxMask(bmpMask));
    REQUIRE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask());

    // Get sub bitmap
    wxBitmap subBmp = bmp.GetSubBitmap(wxRect(w/4, h/4, w/2, h/2));
    const int w2 = w / 2;
    const int h2 = h / 2;
    // Check sub bitmap attributes
    REQUIRE(subBmp.GetWidth() == w2);
    REQUIRE(subBmp.GetHeight() == h2);
    REQUIRE(subBmp.GetDepth() == bmp.GetDepth());
    REQUIRE(subBmp.HasAlpha() == bmp.HasAlpha());
    REQUIRE((subBmp.GetMask() == NULL) == (bmp.GetMask() == NULL));

    // Check sub bitmap pixels
    {
        wxAlphaPixelData data(subBmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxAlphaPixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_RGBA(p, clrLeft.Red(), clrLeft.Green(), clrLeft.Blue(), clrLeft.Alpha());
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_RGBA(p, clrRight.Red(), clrRight.Green(), clrRight.Blue(), clrRight.Alpha());
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_RGBA(p, clrLeft.Red(), clrLeft.Green(), clrLeft.Blue(), clrLeft.Alpha());
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_RGBA(p, clrRight.Red(), clrRight.Green(), clrRight.Blue(), clrRight.Alpha());
    }

    // Check sub bitmap mask
    wxColour maskClrTopLeft;
    wxColour maskClrTopRight;
    wxColour maskClrBottomLeft;
    wxColour maskClrBottomRight;
    // Fetch sample original mask pixels
    {
        wxNativePixelData data(bmpMask);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // top-left point
        maskClrTopLeft = wxColour(p.Red(), p.Green(), p.Blue());
        p.OffsetX(data, w / 2); // top-right point
        maskClrTopRight = wxColour(p.Red(), p.Green(), p.Blue());
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // bottom-left point
        maskClrBottomLeft = wxColour(p.Red(), p.Green(), p.Blue());
        p.OffsetX(data, w / 2); // bottom-right point
        maskClrBottomRight = wxColour(p.Red(), p.Green(), p.Blue());
    }
    CHECK(maskClrTopLeft == *wxWHITE);
    CHECK(maskClrTopRight == *wxWHITE);
    CHECK(maskClrBottomLeft == *wxBLACK);
    CHECK(maskClrBottomRight == *wxBLACK);

    wxBitmap subBmpMask = subBmp.GetMask()->GetBitmap();
    // Check sub bitmap mask attributes
    REQUIRE(subBmpMask.GetWidth() == subBmp.GetWidth());
    REQUIRE(subBmpMask.GetHeight() == subBmp.GetHeight());
#if !defined(__WXOSX__)
    REQUIRE(subBmpMask.GetDepth() == 1);
#endif // !__WXOSX__
    REQUIRE_FALSE(subBmpMask.HasAlpha());
    REQUIRE_FALSE(subBmpMask.GetMask());
    // Check sub bitmap mask pixels
    {
        wxNativePixelData data(subBmpMask);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_RGB(p, maskClrTopLeft.Red(), maskClrTopLeft.Green(), maskClrTopLeft.Blue());
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_RGB(p, maskClrTopRight.Red(), maskClrTopRight.Green(), maskClrTopRight.Blue());
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_RGB(p, maskClrBottomLeft.Red(), maskClrBottomLeft.Green(), maskClrBottomLeft.Blue());
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_RGB(p, maskClrBottomRight.Red(), maskClrBottomRight.Green(), maskClrBottomRight.Blue());
    }
}

namespace Catch
{
    template <>
    struct StringMaker<wxBitmap>
    {
        static std::string convert(const wxBitmap& bmp)
        {
            return wxString::Format("bitmap of size %d*%d",
                                    bmp.GetWidth(),
                                    bmp.GetHeight()).ToStdString();
        }
    };
}

class BitmapColourMatcher : public Catch::MatcherBase<wxBitmap>
{
public:
    explicit BitmapColourMatcher(const wxColour& col)
        : m_col(col)
    {
    }

    bool match(const wxBitmap& bmp) const wxOVERRIDE
    {
        const wxImage img(bmp.ConvertToImage());

        const unsigned char* data = img.GetData();
        for ( int y = 0; y < img.GetHeight(); ++y )
        {
            for ( int x = 0; x < img.GetWidth(); ++x, data += 3 )
            {
                if ( wxColour(data[0], data[1], data[2]) != m_col )
                    return false;
            }
        }

        return true;
    }

    std::string describe() const wxOVERRIDE
    {
        return wxString::Format("doesn't have all %s pixels",
                                m_col.GetAsString()).ToStdString();
    }

private:
    const wxColour m_col;
};

inline BitmapColourMatcher AllPixelsAre(const wxColour& col)
{
    return BitmapColourMatcher(col);
}

TEST_CASE("DC::Clear", "[bitmap][dc]")
{
    // Just some arbitrary pixel data.
    static unsigned char data[] =
    {
        0xff, 0, 0,
        0, 0xff, 0,
        0, 0, 0xff,
        0x7f, 0, 0x7f
    };

    const wxImage img(2, 2, data, true /* don't take ownership of data */);

    wxBitmap bmp(img);

    SECTION("Clearing uses white by default")
    {
        {
            wxMemoryDC dc(bmp);
            dc.Clear();
        }

        CHECK_THAT(bmp, AllPixelsAre(*wxWHITE));
    }

    SECTION("Clearing with specified brush works as expected")
    {
        {
            wxMemoryDC dc(bmp);
            dc.SetBackground(*wxRED_BRUSH);
            dc.Clear();
        }
        CHECK_THAT(bmp, AllPixelsAre(*wxRED));
    }

    SECTION("Clearing with transparent brush does nothing")
    {
        {
            wxMemoryDC dc(bmp);
            dc.SetBackground(*wxTRANSPARENT_BRUSH);
            dc.Clear();
        }

        CHECK_THAT(bmp.ConvertToImage(), RGBSameAs(img));
    }

    SECTION("Clearing with invalid brush uses white too")
    {
        {
            wxMemoryDC dc(bmp);
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.SetBackground(wxBrush());
            dc.Clear();
        }

        CHECK_THAT(bmp, AllPixelsAre(*wxWHITE));
    }
}

#endif //wxHAS_RAW_BITMAP
