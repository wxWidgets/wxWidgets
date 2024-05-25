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


#include "wx/bitmap.h"
#include "wx/rawbmp.h"
#include "wx/dcmemory.h"
#include "wx/dcsvg.h"
#if wxUSE_GRAPHICS_CONTEXT
#include "wx/graphics.h"
#endif // wxUSE_GRAPHICS_CONTEXT

#include "testfile.h"
#include "testimage.h"

#define ASSERT_EQUAL_RGB(c, r, g, b) \
    CHECK( (int)r == (int)c.Red() ); \
    CHECK( (int)g == (int)c.Green() ); \
    CHECK( (int)b == (int)c.Blue() )

#define ASSERT_EQUAL_COLOUR_RGB(c1, c2) \
    CHECK( (int)c1.Red()   == (int)c2.Red() ); \
    CHECK( (int)c1.Green() == (int)c2.Green() ); \
    CHECK( (int)c1.Blue()  == (int)c2.Blue() )

#define ASSERT_EQUAL_COLOUR_RGBA(c1, c2) \
    CHECK( (int)c1.Red()   == (int)c2.Red() ); \
    CHECK( (int)c1.Green() == (int)c2.Green() ); \
    CHECK( (int)c1.Blue()  == (int)c2.Blue() ); \
    CHECK( (int)c1.Alpha() == (int)c2.Alpha() )

#define CHECK_EQUAL_COLOUR_RGB(c1, c2)  ASSERT_EQUAL_COLOUR_RGB(c1, c2)
#define CHECK_EQUAL_COLOUR_RGBA(c1, c2) ASSERT_EQUAL_COLOUR_RGBA(c1, c2)

#ifdef __WXMSW__
// Support for iteration over 32 bpp 0RGB bitmaps
typedef wxPixelFormat<unsigned char, 32, 2, 1, 0> wxNative32PixelFormat;
typedef wxPixelData<wxBitmap, wxNative32PixelFormat> wxNative32PixelData;
#endif // __WXMSW__
#if defined(__WXOSX__) || defined(__WXQT__)
// 32 bpp xRGB bitmaps are native ones
typedef wxNativePixelData wxNative32PixelData;
#endif // __WXOSX__ || __WXQT__

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("BitmapTestCase::Monochrome", "[bitmap][monochrome]")
{
#ifdef __WXGTK__
    WARN("Skipping test known not to work in wxGTK.");
#elif defined(__WXOSX__)
    WARN("Skipping test known not to work in wxOSX.");
#else
    wxBitmap color;
    color.LoadFile("horse.bmp", wxBITMAP_TYPE_BMP);
    REQUIRE(color.IsOk());
    REQUIRE(color.GetDepth() == 32);

    wxImage imgQuant = color.ConvertToImage();
    wxBitmap bmpQuant(imgQuant, 1);
    REQUIRE(bmpQuant.GetDepth() == 1);
    TempFile mono_horse("mono_horse.bmp");
    REQUIRE(bmpQuant.SaveFile(mono_horse.GetName(), wxBITMAP_TYPE_BMP));

    wxBitmap mono;
    REQUIRE(mono.LoadFile(mono_horse.GetName(), wxBITMAP_TYPE_BMP));
    REQUIRE(mono.IsOk());
    REQUIRE(mono.GetDepth() == 1);

    // wxMonoPixelData only exists in wxMSW and wxQt
#if defined(__WXMSW__) || defined(__WXQT__)
    // draw lines on top and left, but leaving blank top and left lines
    {
        wxMonoPixelData data(mono);
        wxMonoPixelData::Iterator p(data);
        p.OffsetY(data, 1);
        for ( int i = 0; i < data.GetWidth() - 2; ++i )
        {
            ++p;
            p.Pixel() = 0;
        }
        p.MoveTo(data, 1, 1);
        for ( int i = 0; i < data.GetHeight() - 3; ++i )
        {
            p.OffsetY(data, 1);
            p.Pixel() = 1;
        }
    }
    TempFile mono_lines_horse("mono_lines_horse.bmp");
    REQUIRE(mono.SaveFile(mono_lines_horse.GetName(), wxBITMAP_TYPE_BMP));
#endif // __WXMSW__ || __WXQT__
#endif // !__WXGTK__
}

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
    wxMask *mask2 = nullptr;
    REQUIRE_NOTHROW(mask2 = new wxMask(*mask));
    bmp.SetMask(mask2);
    REQUIRE(bmp.GetMask() == mask2);
}

TEST_CASE("BitmapTestCase::ToImage", "[bitmap][image][convertto]")
{
    SECTION("RGB bitmap without mask")
    {
        // RGB bitmap
        wxBitmap bmp(16, 16, 24);
        {
            wxMemoryDC dc(bmp);
            dc.SetPen(*wxYELLOW_PEN);
            dc.SetBrush(*wxYELLOW_BRUSH);
            dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
        }
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == nullptr);

        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        wxNativePixelData dataBmp(bmp);
        wxNativePixelData::Iterator rowStartBmp(dataBmp);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxNativePixelData::Iterator iBmp = rowStartBmp;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue());
                wxColour imgc(image.GetRed(x, y), image.GetGreen(x, y), image.GetBlue(x, y));
                CHECK_EQUAL_COLOUR_RGB(imgc, bmpc);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
        }
    }

    SECTION("RGB bitmap with mask")
    {
        // RGB bitmap
        wxBitmap bmp(16, 16, 24);
        {
            wxMemoryDC dc(bmp);
            dc.SetPen(*wxYELLOW_PEN);
            dc.SetBrush(*wxYELLOW_BRUSH);
            dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
        }
        // Mask
        wxBitmap bmask(bmp.GetWidth(), bmp.GetHeight(), 1);
        {
            wxMemoryDC dc(bmask);
#if wxUSE_GRAPHICS_CONTEXT
            wxGraphicsContext* gc = dc.GetGraphicsContext();
            if (gc)
            {
                gc->SetAntialiasMode(wxANTIALIAS_NONE);
            }
#endif // wxUSE_GRAPHICS_CONTEXT
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
            dc.SetPen(*wxWHITE_PEN);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.DrawRectangle(4, 4, 8, 8);
        }
        bmp.SetMask(new wxMask(bmask));
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != nullptr);
        const int numUnmaskedPixels = 8 * 8;

        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        wxNativePixelData dataBmp(bmp);
        wxNativePixelData::Iterator rowStartBmp(dataBmp);
        wxBitmap mask = bmp.GetMask()->GetBitmap();
        wxNativePixelData dataMask(mask);
        wxNativePixelData::Iterator rowStartMask(dataMask);

        int unmaskedPixelsCount = 0;
        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxNativePixelData::Iterator iBmp = rowStartBmp;
            wxNativePixelData::Iterator iMask = rowStartMask;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp, ++iMask )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue());
                wxColour maskc(iMask.Red(), iMask.Green(), iMask.Blue());
                wxColour imgc(image.GetRed(x, y), image.GetGreen(x, y), image.GetBlue(x, y));
                if ( maskc == *wxWHITE )
                {
                    CHECK_EQUAL_COLOUR_RGB(imgc, bmpc);
                    unmaskedPixelsCount++;
                }
                else
                {
                    CHECK_EQUAL_COLOUR_RGB(imgc, maskCol);
                }
            }
            rowStartBmp.OffsetY(dataBmp, 1);
            rowStartMask.OffsetY(dataMask, 1);
        }
        CHECK(unmaskedPixelsCount == numUnmaskedPixels);
    }

    SECTION("RGBA bitmap without mask")
    {
        // RGBA Bitmap
        wxBitmap bmp(16, 16, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
        bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
        {
            const wxColour clrFg(*wxCYAN);
            const wxColour clrBg(*wxGREEN);
            const unsigned char alpha = 92;

#ifdef wxHAS_PREMULTIPLIED_ALPHA
            // premultiplied values
            const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
            const wxColour clrFgAlpha(clrFg);
#endif // wxHAS_PREMULTIPLIED_ALPHA

            wxAlphaPixelData data(bmp);
            REQUIRE(data);
            wxAlphaPixelData::Iterator p(data);
            for ( int y = 0; y < bmp.GetHeight(); y++)
            {
                wxAlphaPixelData::Iterator rowStart = p;
                for ( int x = 0; x < bmp.GetWidth(); x++, ++p )
                {
                    if ( x < bmp.GetWidth() / 2 )
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
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == nullptr);

        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        wxAlphaPixelData dataBmp(bmp);
        wxAlphaPixelData::Iterator rowStartBmp(dataBmp);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxAlphaPixelData::Iterator iBmp = rowStartBmp;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue(), iBmp.Alpha());
                wxColour imgc(image.GetRed(x, y), image.GetGreen(x, y), image.GetBlue(x, y), image.GetAlpha(x,y));
#ifdef wxHAS_PREMULTIPLIED_ALPHA
                // Premultiplied values
                unsigned char r = ((imgc.Red() * imgc.Alpha()) + 127) / 255;
                unsigned char g = ((imgc.Green() * imgc.Alpha()) + 127) / 255;
                unsigned char b = ((imgc.Blue() * imgc.Alpha()) + 127) / 255;
                imgc.Set(r, g, b, imgc.Alpha());
#endif // wxHAS_PREMULTIPLIED_ALPHA
                CHECK_EQUAL_COLOUR_RGBA(imgc, bmpc);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
        }
    }

    SECTION("RGBA bitmap with mask")
    {
        // RGBA Bitmap
        wxBitmap bmp(16, 16, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
        bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
        {
            const wxColour clrFg(*wxCYAN);
            const wxColour clrBg(*wxGREEN);
            const unsigned char alpha = 92;
#ifdef wxHAS_PREMULTIPLIED_ALPHA
            // premultiplied values
            const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
            const wxColour clrFgAlpha(clrFg);
#endif // wxHAS_PREMULTIPLIED_ALPHA

            wxAlphaPixelData data(bmp);
            REQUIRE(data);
            wxAlphaPixelData::Iterator p(data);
            for ( int y = 0; y < bmp.GetHeight(); y++)
            {
                wxAlphaPixelData::Iterator rowStart = p;
                for ( int x = 0; x < bmp.GetWidth(); x++, ++p )
                {
                    if ( x < bmp.GetWidth() / 2 )
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
        // Mask
        wxBitmap bmask(bmp.GetWidth(), bmp.GetHeight(), 1);
        {
            wxMemoryDC dc(bmask);
#if wxUSE_GRAPHICS_CONTEXT
            wxGraphicsContext* gc = dc.GetGraphicsContext();
            if (gc)
            {
                gc->SetAntialiasMode(wxANTIALIAS_NONE);
            }
#endif // wxUSE_GRAPHICS_CONTEXT
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
            dc.SetPen(*wxWHITE_PEN);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.DrawRectangle(4, 4, 8, 8);
        }
        bmp.SetMask(new wxMask(bmask));
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != nullptr);
        const int numUnmaskedPixels = 8 * 8;

        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        wxAlphaPixelData dataBmp(bmp);
        wxAlphaPixelData::Iterator rowStartBmp(dataBmp);
        wxBitmap mask = bmp.GetMask()->GetBitmap();
        wxNativePixelData dataMask(mask);
        wxNativePixelData::Iterator rowStartMask(dataMask);

        int unmaskedPixelsCount = 0;
        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxAlphaPixelData::Iterator iBmp = rowStartBmp;
            wxNativePixelData::Iterator iMask = rowStartMask;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp, ++iMask )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue(), iBmp.Alpha());
                wxColour maskc(iMask.Red(), iMask.Green(), iMask.Blue());
                wxColour imgc(image.GetRed(x, y), image.GetGreen(x, y), image.GetBlue(x, y), image.GetAlpha(x,y));
                if ( maskc == *wxWHITE )
                {
#ifdef wxHAS_PREMULTIPLIED_ALPHA
                    // Premultiplied values
                    unsigned char r = ((imgc.Red() * imgc.Alpha()) + 127) / 255;
                    unsigned char g = ((imgc.Green() * imgc.Alpha()) + 127) / 255;
                    unsigned char b = ((imgc.Blue() * imgc.Alpha()) + 127) / 255;
                    imgc.Set(r, g, b, imgc.Alpha());
#endif // wxHAS_PREMULTIPLIED_ALPHA
                    CHECK_EQUAL_COLOUR_RGBA(imgc, bmpc);
                    unmaskedPixelsCount++;
                }
                else
                {
                    CHECK_EQUAL_COLOUR_RGB(imgc, maskCol);
                    CHECK(imgc.Alpha() == bmpc.Alpha());
                }
            }
            rowStartBmp.OffsetY(dataBmp, 1);
            rowStartMask.OffsetY(dataMask, 1);
        }
        CHECK(unmaskedPixelsCount == numUnmaskedPixels);
    }
}

TEST_CASE("BitmapTestCase::FromImage", "[bitmap][image][convertfrom]")
{
    const wxColour maskCol(*wxRED);
    const wxColour fillCol(*wxGREEN);

    SECTION("RGB image without mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE_FALSE(img.HasMask());

        wxBitmap bmp(img);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == nullptr);
        REQUIRE(bmp.GetWidth() == img.GetWidth());
        REQUIRE(bmp.GetHeight() == img.GetHeight());

        wxNativePixelData dataBmp(bmp);
        wxNativePixelData::Iterator rowStartBmp(dataBmp);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxNativePixelData::Iterator iBmp = rowStartBmp;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue());
                wxColour imgc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y));
                CHECK_EQUAL_COLOUR_RGB(bmpc, imgc);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
        }
    }

    SECTION("RGB image with mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE(img.HasMask() == true);

        wxBitmap bmp(img);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != nullptr);
        REQUIRE(bmp.GetWidth() == img.GetWidth());
        REQUIRE(bmp.GetHeight() == img.GetHeight());

        wxNativePixelData dataBmp(bmp);
        wxNativePixelData::Iterator rowStartBmp(dataBmp);

        wxBitmap mask = bmp.GetMask()->GetBitmap();
        wxNativePixelData dataMask(mask);
        wxNativePixelData::Iterator rowStartMask(dataMask);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxNativePixelData::Iterator iBmp = rowStartBmp;
            wxNativePixelData::Iterator iMask = rowStartMask;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp, ++iMask )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue());
                wxColour maskc(iMask.Red(), iMask.Green(), iMask.Blue());
                wxColour imgc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y));
                CHECK_EQUAL_COLOUR_RGB(bmpc, imgc);
                wxColour c = maskc == *wxWHITE ? fillCol : maskCol;
                CHECK_EQUAL_COLOUR_RGB(bmpc, c);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
            rowStartMask.OffsetY(dataMask, 1);
        }
    }

    SECTION("RGBA image without mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetAlpha();
        img.SetAlpha(0, 0, 128);
        img.SetAlpha(0, 1, 0);
        img.SetAlpha(1, 0, 128);
        img.SetAlpha(1, 1, 0);
        REQUIRE(img.HasAlpha() == true);
        REQUIRE_FALSE(img.HasMask());

        wxBitmap bmp(img);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == nullptr);
        REQUIRE(bmp.GetWidth() == img.GetWidth());
        REQUIRE(bmp.GetHeight() == img.GetHeight());

        wxAlphaPixelData dataBmp(bmp);
        wxAlphaPixelData::Iterator rowStartBmp(dataBmp);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxAlphaPixelData::Iterator iBmp = rowStartBmp;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue(), iBmp.Alpha());
                wxColour imgc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y), img.GetAlpha(x, y));
#ifdef wxHAS_PREMULTIPLIED_ALPHA
                // Premultiplied values
                unsigned char r = ((imgc.Red() * imgc.Alpha()) + 127) / 255;
                unsigned char g = ((imgc.Green() * imgc.Alpha()) + 127) / 255;
                unsigned char b = ((imgc.Blue() * imgc.Alpha()) + 127) / 255;
                imgc.Set(r, g, b, imgc.Alpha());
#endif // wxHAS_PREMULTIPLIED_ALPHA
                CHECK_EQUAL_COLOUR_RGBA(bmpc, imgc);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
        }
    }

    SECTION("RGBA image with mask")
    {
        wxImage img(2, 2);
        img.SetRGB(0, 0, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(0, 1, maskCol.Red(), maskCol.Green(), maskCol.Blue());
        img.SetRGB(1, 0, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetRGB(1, 1, fillCol.Red(), fillCol.Green(), fillCol.Blue());
        img.SetAlpha();
        img.SetAlpha(0, 0, 128);
        img.SetAlpha(0, 1, 0);
        img.SetAlpha(1, 0, 128);
        img.SetAlpha(1, 1, 0);
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE(img.HasAlpha() == true);
        REQUIRE(img.HasMask() == true);

        wxBitmap bmp(img);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != nullptr);
        REQUIRE(bmp.GetWidth() == img.GetWidth());
        REQUIRE(bmp.GetHeight() == img.GetHeight());

        wxAlphaPixelData dataBmp(bmp);
        wxAlphaPixelData::Iterator rowStartBmp(dataBmp);

        wxBitmap mask = bmp.GetMask()->GetBitmap();
        wxNativePixelData dataMask(mask);
        wxNativePixelData::Iterator rowStartMask(dataMask);

        for ( int y = 0; y < bmp.GetHeight(); ++y )
        {
            wxAlphaPixelData::Iterator iBmp = rowStartBmp;
            wxNativePixelData::Iterator iMask = rowStartMask;
            for ( int x = 0; x < bmp.GetWidth(); ++x, ++iBmp, ++iMask )
            {
                wxColour bmpc(iBmp.Red(), iBmp.Green(), iBmp.Blue(), iBmp.Alpha());
                wxColour maskc(iMask.Red(), iMask.Green(), iMask.Blue());
                wxColour imgc(img.GetRed(x, y), img.GetGreen(x, y), img.GetBlue(x, y), img.GetAlpha(x, y));
#ifdef wxHAS_PREMULTIPLIED_ALPHA
                // Premultiplied values
                unsigned char r = ((imgc.Red() * imgc.Alpha()) + 127) / 255;
                unsigned char g = ((imgc.Green() * imgc.Alpha()) + 127) / 255;
                unsigned char b = ((imgc.Blue() * imgc.Alpha()) + 127) / 255;
                imgc.Set(r, g, b, imgc.Alpha());
#endif // wxHAS_PREMULTIPLIED_ALPHA
                CHECK_EQUAL_COLOUR_RGBA(bmpc, imgc);

                wxColour c = maskc == *wxWHITE ? fillCol : maskCol;
#ifdef wxHAS_PREMULTIPLIED_ALPHA
                // Premultiplied values
                r = ((c.Red() * imgc.Alpha()) + 127) / 255;
                g = ((c.Green() * imgc.Alpha()) + 127) / 255;
                b = ((c.Blue() * imgc.Alpha()) + 127) / 255;
                c.Set(r, g, b);
#endif // wxHAS_PREMULTIPLIED_ALPHA
                CHECK_EQUAL_COLOUR_RGB(bmpc, c);
            }
            rowStartBmp.OffsetY(dataBmp, 1);
            rowStartMask.OffsetY(dataMask, 1);
        }
    }
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
    REQUIRE(bmp.GetMask() == nullptr);

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
    REQUIRE(bmp.GetMask() == nullptr);
    bmp.SetMask(new wxMask(bmask));
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != nullptr);

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
        ASSERT_EQUAL_COLOUR_RGB(p, clrLeft);
        p.OffsetX(data, w / 2); // drawn area - right side
        ASSERT_EQUAL_COLOUR_RGB(p, clrRight);
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // masked area - left side
        ASSERT_EQUAL_COLOUR_RGB(p, clrBg);
        p.OffsetX(data, w / 2); // masked area - right side
        ASSERT_EQUAL_COLOUR_RGB(p, clrBg);
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
        ASSERT_EQUAL_COLOUR_RGB(p, clrLeft);
        p.OffsetX(data, w / 2); // right upper side
        ASSERT_EQUAL_COLOUR_RGB(p, clrRight);
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_COLOUR_RGB(p, clrLeft);
        p.OffsetX(data, w / 2); // right lower side - same colour as upper
        ASSERT_EQUAL_COLOUR_RGB(p, clrRight);
    }
}

TEST_CASE("BitmapTestCase::DrawAlpha", "[bitmap][draw][alpha]")
{
    const int w = 16;
    const int h = 16;

    const wxColour clrFg(*wxCYAN);
    const wxColour clrBg(*wxGREEN);
    const unsigned char alpha = 92;

#ifdef wxHAS_PREMULTIPLIED_ALPHA
    // premultiplied values
    const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
    const wxColour clrFgAlpha(clrFg);
#endif // wxHAS_PREMULTIPLIED_ALPHA

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
    REQUIRE(bmp.GetMask() == nullptr);

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
    ASSERT_EQUAL_COLOUR_RGB(p1, clrFg);
    p1.OffsetX(data24, w / 2); // right side is with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
    // premultiplied values
    ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha)  + 127) / 255);
#else
    ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                         (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                         (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA

#if defined(__WXMSW__) || defined(__WXOSX__) || defined(__WXQT__)
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
    ASSERT_EQUAL_COLOUR_RGB(p2, clrFg);
    p2.OffsetX(data32, w / 2); // right side is with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
    // premultiplied values
    ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                         clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
    ASSERT_EQUAL_RGB(p2, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                         (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                         (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
#endif // __WXMSW__ || __WXOSX__ || __WXQT__
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

#ifdef wxHAS_PREMULTIPLIED_ALPHA
     // premultiplied values
     const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
     const wxColour clrFgAlpha(clrFg);
#endif // wxHAS_PREMULTIPLIED_ALPHA

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
    REQUIRE(bmp.GetMask() == nullptr);
    bmp.SetMask(new wxMask(bmask));
    REQUIRE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != nullptr);

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
        ASSERT_EQUAL_COLOUR_RGB(p1, clrFg);
        p1.OffsetX(data24, w / 2); // drawn area - right side with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
        // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
        p1 = rowStart1;
        p1.OffsetY(data24, h / 2);
        p1.OffsetX(data24, w / 4); // masked area - left side
        ASSERT_EQUAL_COLOUR_RGB(p1, clrBg);
        p1.OffsetX(data24, w / 2); // masked area - right side
        ASSERT_EQUAL_COLOUR_RGB(p1, clrBg);
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
        ASSERT_EQUAL_COLOUR_RGB(p1, clrFg);
        p1.OffsetX(data24, w / 2); // right upper side with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
        // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
        p1 = rowStart1;
        p1.OffsetY(data24, h / 2);
        p1.OffsetX(data24, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_COLOUR_RGB(p1, clrFg);
        p1.OffsetX(data24, w / 2); // right lower side - same colour as upper
#ifdef wxHAS_PREMULTIPLIED_ALPHA
         // premultiplied values
        ASSERT_EQUAL_RGB(p1, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p1, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
    }

#if defined(__WXMSW__) || defined(__WXOSX__) || defined(__WXQT__)
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
        ASSERT_EQUAL_COLOUR_RGB(p2, clrFg);
        p2.OffsetX(data32, w / 2); // drawn area - right side with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
        // premultiplied values
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p2, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
        p2 = rowStart2;
        p2.OffsetY(data32, h / 2);
        p2.OffsetX(data32, w / 4); // masked area - left side
        ASSERT_EQUAL_COLOUR_RGB(p2, clrBg);
        p2.OffsetX(data32, w / 2); // masked area - right side
        ASSERT_EQUAL_COLOUR_RGB(p2, clrBg);
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
        ASSERT_EQUAL_COLOUR_RGB(p2, clrFg);
        p2.OffsetX(data32, w / 2); // right upper side with alpha
#ifdef wxHAS_PREMULTIPLIED_ALPHA
        // premultiplied values
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p2, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
        p2 = rowStart2;
        p2.OffsetY(data32, h / 2);
        p2.OffsetX(data32, w / 4); // left lower side - same colour as upper
        ASSERT_EQUAL_COLOUR_RGB(p2, clrFg);
        p2.OffsetX(data32, w / 2); // right lower side - same colour as upper
        // premultiplied values
#ifdef wxHAS_PREMULTIPLIED_ALPHA
        ASSERT_EQUAL_RGB(p2, clrFgAlpha.Red() + (clrBg.Red() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Green() + (clrBg.Green() * (255 - alpha) + 127) / 255,
                             clrFgAlpha.Blue() + (clrBg.Blue() * (255 - alpha) + 127) / 255);
#else
        ASSERT_EQUAL_RGB(p2, (clrFg.Red() * alpha + clrBg.Red() * (255 - alpha) + 127) / 255,
                             (clrFg.Green() * alpha + clrBg.Green() * (255 - alpha) + 127) / 255,
                             (clrFg.Blue() * alpha + clrBg.Blue() * (255 - alpha) + 127) / 255);
#endif // wxHAS_PREMULTIPLIED_ALPHA
    }
#endif // __WXMSW__ || __WXOSX__ || __WXQT__
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
    REQUIRE(bmp.GetMask() == nullptr);

    // Get sub bitmap
    wxBitmap subBmp = bmp.GetSubBitmap(wxRect(w/4, h/4, w/2, h/2));
    // Check sub bitmap attributes
    REQUIRE(subBmp.GetWidth() == w/2);
    REQUIRE(subBmp.GetHeight() == h/2);
    REQUIRE(subBmp.GetDepth() == bmp.GetDepth());
    REQUIRE(subBmp.HasAlpha() == bmp.HasAlpha());
    REQUIRE((subBmp.GetMask() == nullptr) == (bmp.GetMask() == nullptr));

    const int w2 = w / 2;
    const int h2 = h / 2;
    // Check sub bitmap pixels
    wxNativePixelData data(subBmp);
    REQUIRE(data);
    wxNativePixelData::Iterator p(data);
    p.OffsetY(data, h2 / 4);
    wxNativePixelData::Iterator rowStart = p;
    p.OffsetX(data, w2 / 4); // top-left point
    ASSERT_EQUAL_COLOUR_RGB(p, clrTopLeft);
    p.OffsetX(data, w2 / 2); // top-right point
    ASSERT_EQUAL_COLOUR_RGB(p, clrTopRight);
    p = rowStart;
    p.OffsetY(data, h2 / 2);
    p.OffsetX(data, w2 / 4); // bottom-left point
    ASSERT_EQUAL_COLOUR_RGB(p, clrBottomLeft);
    p.OffsetX(data, w2 / 2); // bottom-right point
    ASSERT_EQUAL_COLOUR_RGB(p, clrBottomRight);
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
    REQUIRE(bmp.GetMask() == nullptr);
    bmp.SetMask(new wxMask(bmpMask));
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() != nullptr);

    // Get sub bitmap
    wxBitmap subBmp = bmp.GetSubBitmap(wxRect(w/4, h/4, w/2, h/2));
    const int w2 = w / 2;
    const int h2 = h / 2;
    // Check sub bitmap attributes
    REQUIRE(subBmp.GetWidth() == w2);
    REQUIRE(subBmp.GetHeight() == h2);
    REQUIRE(subBmp.GetDepth() == bmp.GetDepth());
    REQUIRE(subBmp.HasAlpha() == bmp.HasAlpha());
    REQUIRE((subBmp.GetMask() == nullptr) == (bmp.GetMask() == nullptr));

    // Check sub bitmap pixels
    {
        wxNativePixelData data(subBmp);
        REQUIRE(data);
        wxNativePixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxNativePixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_COLOUR_RGB(p, clrTopLeft);
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_COLOUR_RGB(p, clrTopRight);
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_COLOUR_RGB(p, clrBottomLeft);
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_COLOUR_RGB(p, clrBottomRight);
    }

    // Check sub bitmap mask
    wxColour maskClrTopLeft;
    wxColour maskClrTopRight;
    wxColour maskClrBottomLeft;
    wxColour maskClrBottomRight;
#if !defined(__WXOSX__)
    REQUIRE(bmpMask.GetDepth() == 1);
#endif
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

    // wxMonoPixelData only exists in wxMSW and wxQt
#if defined(__WXMSW__) || defined(__WXQT__)
    bool maskValueTopLeft;
    bool maskValueTopRight;
    bool maskValueBottomLeft;
    bool maskValueBottomRight;
    // Fetch sample original mask pixels
    {
        REQUIRE(bmpMask.GetDepth() == 1);
        wxMonoPixelData data(bmpMask);
        REQUIRE(data);
        wxMonoPixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxMonoPixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // top-left point
        maskValueTopLeft = p.Pixel();
        p.OffsetX(data, w / 2); // top-right point
        maskValueTopRight = p.Pixel();
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // bottom-left point
        maskValueBottomLeft = p.Pixel();
        p.OffsetX(data, w / 2); // bottom-right point
        maskValueBottomRight = p.Pixel();
    }
    REQUIRE(bmpMask.GetDepth() == 1);
    CHECK(maskValueTopLeft == true);
    CHECK(maskValueTopRight == true);
    CHECK(maskValueBottomLeft == false);
    CHECK(maskValueBottomRight == false);
#endif // __WXMSW__ || __WXQT__

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
        ASSERT_EQUAL_COLOUR_RGB(p, maskClrTopLeft);
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_COLOUR_RGB(p, maskClrTopRight);
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_COLOUR_RGB(p, maskClrBottomLeft);
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_COLOUR_RGB(p, maskClrBottomRight);
    }

    // wxMonoPixelData only exists in wxMSW and wxQt
#if defined(__WXMSW__) || defined(__WXQT__)
    {
        REQUIRE(subBmpMask.GetDepth() == 1);
        wxMonoPixelData data(subBmpMask);
        REQUIRE(data);
        wxMonoPixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxMonoPixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        CHECK(p.Pixel() == maskValueTopLeft);
        p.OffsetX(data, w2 / 2); // top-right point
        CHECK(p.Pixel() == maskValueTopRight);
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        CHECK(p.Pixel() == maskValueBottomLeft);
        p.OffsetX(data, w2 / 2); // bottom-right point
        CHECK(p.Pixel() == maskValueBottomRight);
    }
    REQUIRE(subBmpMask.GetDepth() == 1);
#endif // __WXMSW__ || __WXQT__
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

#ifdef wxHAS_PREMULTIPLIED_ALPHA
    // premultiplied values
    const wxColour clrRight(((clrLeft.Red() * alpha) + 127) / 255, ((clrLeft.Green() * alpha) + 127) / 255, ((clrLeft.Blue() * alpha) + 127) / 255, alpha);
#else
    const wxColour clrRight(clrLeft.Red(), clrLeft.Green(), clrLeft.Blue(), alpha);
#endif // wxHAS_PREMULTIPLIED_ALPHA

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
    REQUIRE((subBmp.GetMask() == nullptr) == (bmp.GetMask() == nullptr));

    // Check sub bitmap pixels
    {
        wxAlphaPixelData data(subBmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxAlphaPixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        ASSERT_EQUAL_COLOUR_RGBA(p, clrLeft);
        p.OffsetX(data, w2 / 2); // top-right point
        ASSERT_EQUAL_COLOUR_RGBA(p, clrRight);
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        ASSERT_EQUAL_COLOUR_RGBA(p, clrLeft);
        p.OffsetX(data, w2 / 2); // bottom-right point
        ASSERT_EQUAL_COLOUR_RGBA(p, clrRight);
    }

    // Check sub bitmap mask
    wxColour maskClrTopLeft;
    wxColour maskClrTopRight;
    wxColour maskClrBottomLeft;
    wxColour maskClrBottomRight;
#if !defined(__WXOSX__)
    REQUIRE(bmpMask.GetDepth() == 1);
#endif
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

    // wxMonoPixelData only exists in wxMSW and wxQt
#if defined(__WXMSW__) || defined(__WXQT__)
    bool maskValueTopLeft;
    bool maskValueTopRight;
    bool maskValueBottomLeft;
    bool maskValueBottomRight;
    // Fetch sample original mask pixels
    {
        REQUIRE(bmpMask.GetDepth() == 1);
        wxMonoPixelData data(bmpMask);
        REQUIRE(data);
        wxMonoPixelData::Iterator p(data);
        p.OffsetY(data, h / 4);
        wxMonoPixelData::Iterator rowStart = p;
        p.OffsetX(data, w / 4); // top-left point
        maskValueTopLeft = p.Pixel();
        p.OffsetX(data, w / 2); // top-right point
        maskValueTopRight = p.Pixel();
        p = rowStart;
        p.OffsetY(data, h / 2);
        p.OffsetX(data, w / 4); // bottom-left point
        maskValueBottomLeft = p.Pixel();
        p.OffsetX(data, w / 2); // bottom-right point
        maskValueBottomRight = p.Pixel();
    }
    REQUIRE(bmpMask.GetDepth() == 1);
    CHECK(maskValueTopLeft == true);
    CHECK(maskValueTopRight == true);
    CHECK(maskValueBottomLeft == false);
    CHECK(maskValueBottomRight == false);
#endif // __WXMSW__ || __WXQT__

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

    // wxMonoPixelData only exists in wxMSW and wxQt
#if defined(__WXMSW__) || defined(__WXQT__)
    {
        REQUIRE(subBmpMask.GetDepth() == 1);
        wxMonoPixelData data(subBmpMask);
        REQUIRE(data);
        wxMonoPixelData::Iterator p(data);
        p.OffsetY(data, h2 / 4);
        wxMonoPixelData::Iterator rowStart = p;
        p.OffsetX(data, w2 / 4); // top-left point
        CHECK(p.Pixel() == maskValueTopLeft);
        p.OffsetX(data, w2 / 2); // top-right point
        CHECK(p.Pixel() == maskValueTopRight);
        p = rowStart;
        p.OffsetY(data, h2 / 2);
        p.OffsetX(data, w2 / 4); // bottom-left point
        CHECK(p.Pixel() == maskValueBottomLeft);
        p.OffsetX(data, w2 / 2); // bottom-right point
        CHECK(p.Pixel() == maskValueBottomRight);
    }
    REQUIRE(subBmpMask.GetDepth() == 1);
#endif // __WXMSW__ || __WXQT__
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

    bool match(const wxBitmap& bmp) const override
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

    std::string describe() const override
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

TEST_CASE("Bitmap::DC", "[bitmap][dc]")
{
#if wxUSE_SVG
    TempFile dummySVG("dummy.svg");
    wxSVGFileDC dc(dummySVG.GetName());
    wxBitmap bmp(10, 10, dc);
    CHECK( bmp.IsOk() );

    wxImage image(10, 10);
    wxBitmap bmpFromImage(image, dc);
    CHECK( bmpFromImage.IsOk() );
#endif // wxUSE_SVG
}

#if wxUSE_GRAPHICS_CONTEXT

inline void DrawScaledBmp(wxBitmap& bmp, float scale, wxGraphicsRenderer* renderer)
{
    if ( !renderer )
        return;

    wxBitmap canvas(bmp.GetWidth() * scale, bmp.GetHeight() * scale, 24);
    {
        wxMemoryDC mdc(canvas);
        mdc.SetBackground(*wxBLACK_BRUSH);
        mdc.Clear();
        wxGraphicsContext* gc = renderer->CreateContext(mdc);
        gc->DrawBitmap(bmp, 0, 0, canvas.GetSize().GetWidth(), canvas.GetSize().GetHeight());
        delete gc;
    }

    wxNativePixelData bmpData(bmp);
    REQUIRE(bmpData);
    wxNativePixelData::Iterator bmpP(bmpData);

    wxNativePixelData canvasData(canvas);
    REQUIRE(canvasData);
    wxNativePixelData::Iterator canvasP(canvasData);

    bmpP.MoveTo(bmpData, 0, 0);
    canvasP.MoveTo(canvasData, 0, 0);
    ASSERT_EQUAL_COLOUR_RGB(bmpP, canvasP);

    bmpP.MoveTo(bmpData, bmp.GetWidth() / 2, bmp.GetHeight() / 2);
    canvasP.MoveTo(canvasData, canvas.GetWidth() / 2, canvas.GetHeight() / 2);
    ASSERT_EQUAL_COLOUR_RGB(bmpP, canvasP);

    bmpP.MoveTo(bmpData, bmp.GetWidth() - 1, bmp.GetHeight() - 1);
    canvasP.MoveTo(canvasData, canvas.GetWidth() - 1, canvas.GetHeight() - 1);
    ASSERT_EQUAL_COLOUR_RGB(bmpP, canvasP);
}

TEST_CASE("GC::DrawBitmap", "[bitmap][drawbitmap]")
{
    // Draw a red rectangle to a bitmap, draw the bitmap using a GC to a larger
    // canvas and test if the bitmap scaled correctly by checking pixels
    // inside and outside the rectangle.

    wxBitmap bmp(100, 100, 24);
    {
        wxMemoryDC mdc(bmp);
        mdc.SetBackground(*wxBLACK_BRUSH);
        mdc.Clear();
        mdc.SetBrush(*wxRED_BRUSH);
        mdc.DrawRectangle(20, 20, 60, 60);
    }

    SECTION("Draw bitmap using default GC")
    {
        DrawScaledBmp(bmp, 1, wxGraphicsRenderer::GetDefaultRenderer());
    }

    SECTION("Draw bitmap 0.5x scaled using default GC")
    {
        DrawScaledBmp(bmp, 0.5, wxGraphicsRenderer::GetDefaultRenderer());
    }

    SECTION("Draw bitmap 5x scaled using default GC")
    {
        DrawScaledBmp(bmp, 5, wxGraphicsRenderer::GetDefaultRenderer());
    }

#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D

    SECTION("Draw bitmap using Direct2D GC")
    {
        DrawScaledBmp(bmp, 1, wxGraphicsRenderer::GetDirect2DRenderer());
    }

    SECTION("Draw bitmap 0.5x scaled using Direct2D GC")
    {
        DrawScaledBmp(bmp, 0.5, wxGraphicsRenderer::GetDirect2DRenderer());
    }

    SECTION("Draw bitmap 5x scaled using Direct2D GC")
    {
        DrawScaledBmp(bmp, 5, wxGraphicsRenderer::GetDirect2DRenderer());
    }

#endif //defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D

}

#endif //wxUSE_GRAPHICS_CONTEXT

#endif //wxHAS_RAW_BITMAP

#if defined(wxHAS_DPI_INDEPENDENT_PIXELS) || defined(__WXMSW__)

TEST_CASE("Bitmap::ScaleFactor", "[bitmap][dc][scale]")
{
    // Create a bitmap with scale factor != 1.
    wxBitmap bmp;
    bmp.CreateWithDIPSize(8, 8, 2);
    REQUIRE( bmp.GetScaleFactor() == 2 );
    CHECK( bmp.GetSize() == wxSize(16, 16) );

    // wxMemoryDC should use the same scale factor as the bitmap.
    wxMemoryDC dc(bmp);
    CHECK( dc.GetContentScaleFactor() == 2 );

    // A bitmap "compatible" with this DC should also use the same scale factor.
    wxBitmap bmp2(4, 4, dc);
    CHECK( bmp2.GetScaleFactor() == 2 );
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    CHECK( bmp2.GetSize() == wxSize(8, 8) );
#else
    CHECK( bmp2.GetSize() == wxSize(4, 4) );
#endif

    // A compatible bitmap created from wxImage and this DC should also inherit
    // the same scale factor, but its size should be still the same as that of
    // the image.
    wxImage img(16, 16);
    wxBitmap bmp3(img, dc);
    CHECK( bmp3.GetScaleFactor() == 2 );
    CHECK( bmp3.GetSize() == wxSize(16, 16) );

    // And another way to create a bitmap with specified scale factor.
    const wxSize sizeLog(10, 10);

    wxBitmap bmp4;
    bmp4.CreateWithLogicalSize(sizeLog, 2);
    CHECK( bmp4.GetScaleFactor() == 2 );
    CHECK( bmp4.GetLogicalSize() == sizeLog );

#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    CHECK( bmp4.GetSize() == 2*sizeLog );
#else
    CHECK( bmp4.GetSize() == sizeLog );
#endif
}

TEST_CASE("wxBitmap::GetSubBitmap", "[bitmap]")
{
    // Make the logical size odd to test correct rounding.
    const double scale = 1.5;
    const wxSize sizeLog(15, 15);
    const wxSize sizePhy(23, 23);

    // Prepare the main bitmap.
    wxBitmap bmp;
    bmp.CreateWithDIPSize(sizeLog, scale);
    CHECK( bmp.GetDIPSize() == sizeLog );
    CHECK( bmp.GetSize() == sizePhy );
    CHECK( bmp.GetScaleFactor() == scale );

    // Extracting sub-bitmap of the entire bitmap size should return the bitmap
    // of the same size.
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    const wxRect rectAll(wxPoint(0, 0), sizeLog);
#else
    const wxRect rectAll(wxPoint(0, 0), sizePhy);
#endif

    const wxBitmap sub = bmp.GetSubBitmap(rectAll);
    CHECK( sub.GetDIPSize() == sizeLog );
    CHECK( sub.GetSize() == sizePhy );
    CHECK( sub.GetScaleFactor() == scale );

    // Using incorrect bounds should assert.
    wxRect rectInvalid = rectAll;
    rectInvalid.Offset(1, 0);
    WX_ASSERT_FAILS_WITH_ASSERT( bmp.GetSubBitmap(rectInvalid) );
}

#endif // ports with scaled bitmaps support
