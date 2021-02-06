///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/graphbitmap.cpp
// Purpose:     wxGraphicsBitmap unit test
// Author:      Artur Wieczorek
// Created:     2021-02-05
// Copyright:   (c) 2021 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef wxHAS_RAW_BITMAP

#include "wx/bitmap.h"
#include "wx/rawbmp.h"
#include "wx/dcmemory.h"
#include "wx/graphics.h"

#include "testimage.h"

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

#if wxUSE_GRAPHICS_CONTEXT

namespace
{
wxBitmap DoCreateBitmapRGB(int w, int h, int bpp, bool withMask)
{
    wxBitmap bmp(w, h, bpp);
    {
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxBLUE_BRUSH);
        dc.Clear();
        dc.SetPen(*wxYELLOW_PEN);
        dc.SetBrush(*wxYELLOW_BRUSH);
        dc.DrawRectangle(2, 2, bmp.GetWidth() - 2, bmp.GetHeight() - 2);
    }
    REQUIRE_FALSE(bmp.HasAlpha());
    REQUIRE(bmp.GetMask() == NULL);
    if ( withMask )
    {
        // Mask
        wxBitmap bmask(bmp.GetWidth(), bmp.GetHeight(), 1);
        {
            wxMemoryDC dc(bmask);
            wxGraphicsContext* gc = dc.GetGraphicsContext();
            if ( gc )
            {
                gc->SetAntialiasMode(wxANTIALIAS_NONE);
            }
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
            dc.SetPen(*wxWHITE_PEN);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.DrawRectangle(4, 4, 4, 4);
        }
        bmp.SetMask(new wxMask(bmask));
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != NULL);
    }

    return bmp;
}

wxBitmap CreateBitmapRGB(int w, int h, bool withMask)
{
    return DoCreateBitmapRGB(w, h, 24, withMask);
}

#if defined(__WXMSW__) || defined(__WXOSX__)
// 32-bit RGB bitmap
wxBitmap CreateBitmapXRGB(int w, int h, bool withMask)
{
    return DoCreateBitmapRGB(w, h, 32, withMask);
}
#endif // __WXMSW__ || __WXOSX__

wxBitmap CreateBitmapRGBA(int w, int h, bool withMask)
{
    wxBitmap bmp(w, h, 32);
#if defined(__WXMSW__) || defined(__WXOSX__)
    bmp.UseAlpha();
#endif // __WXMSW__ || __WXOSX__
    {
        const wxColour clrFg(*wxCYAN);
        const wxColour clrBg(*wxGREEN);
        const unsigned char alpha = 51;

#if defined(__WXMSW__) || defined(__WXOSX__)
        // premultiplied values
        const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
        const wxColour clrFgAlpha(clrFg);
#endif // __WXMSW__ || __WXOSX__

        wxAlphaPixelData data(bmp);
        REQUIRE(data);
        wxAlphaPixelData::Iterator p(data);
        for ( int y = 0; y < bmp.GetHeight(); y++ )
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
    REQUIRE(bmp.GetMask() == NULL);
    if ( withMask )
    {
        // Mask
        wxBitmap bmask(bmp.GetWidth(), bmp.GetHeight(), 1);
        {
            wxMemoryDC dc(bmask);
            wxGraphicsContext* gc = dc.GetGraphicsContext();
            if ( gc )
            {
                gc->SetAntialiasMode(wxANTIALIAS_NONE);
            }
            dc.SetBackground(*wxBLACK_BRUSH);
            dc.Clear();
            dc.SetPen(*wxWHITE_PEN);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.DrawRectangle(4, 4, 4, 4);
        }
        bmp.SetMask(new wxMask(bmask));
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != NULL);
    }

    return bmp;
}

void BlendMaskWithAlpha(wxImage& img)
{
    if ( img.HasAlpha() && img.HasMask() )
    {
        // We need to blend mask with alpha values
        size_t numPixels = img.GetWidth() * img.GetHeight();
        unsigned char* oldAlpha = new unsigned char[numPixels];
        memcpy(oldAlpha, img.GetAlpha(), numPixels);

        img.ClearAlpha();
        img.InitAlpha();
        unsigned char* newAlpha = img.GetAlpha();
        for ( size_t i = 0; i < numPixels; i++ )
        {
            if ( newAlpha[i] == wxIMAGE_ALPHA_OPAQUE )
                newAlpha[i] = oldAlpha[i];
        }
        delete[]oldAlpha;
    }
    else
    {
        img.InitAlpha();
    }
}

inline void CheckCreateGraphBitmap(wxGraphicsRenderer* gr, const wxBitmap& srcBmp, const wxImage& refImg)
{
    wxGraphicsBitmap gbmp = gr->CreateBitmap(srcBmp);

    wxImage gimage = gbmp.ConvertToImage();

    CHECK_THAT(gimage, RGBASameAs(refImg));
}

inline void CheckCreateGraphSubBitmap(wxGraphicsRenderer* gr, const wxBitmap& srcBmp,
                                      double x, double y, double w, double h,
                                      const wxImage& refImg)
{
    wxGraphicsBitmap gbmp = gr->CreateBitmap(srcBmp);
    wxGraphicsBitmap gSubBmp = gr->CreateSubBitmap(gbmp, x, y, w, h);

    wxImage gimage = gSubBmp.ConvertToImage();

    CHECK_THAT(gimage, RGBASameAs(refImg));
}
};

TEST_CASE("GraphicsBitmapTestCase::Create", "[graphbitmap][create]")
{
    SECTION("RGB bitmap without mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, false);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGB bitmap with mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, true);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        image.InitAlpha();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

#if defined(__WXMSW__) || defined(__WXOSX__)
    SECTION("xRGB bitmap without mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, false);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("xRGB bitmap with mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, true);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        image.InitAlpha();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
#endif // _WXMSW__ || __WXOSX__

    SECTION("RGBA bitmap without mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, false);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
    }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA bitmap with mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, true);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        BlendMaskWithAlpha(image);
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
}

TEST_CASE("GraphicsBitmapTestCase::SubBitmap", "[graphbitmap][subbitmap][create]")
{
    SECTION("RGB bitmap without mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, false);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        const int subX = 1;
        const int subY = 1;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGB bitmap with mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, true);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        const int subX = 2;
        const int subY = 2;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        image.InitAlpha();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

#if defined(__WXMSW__) || defined(__WXOSX__)
    SECTION("xRGB bitmap without mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, false);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        const int subX = 1;
        const int subY = 1;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("xRGB bitmap with mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, true);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        const int subX = 2;
        const int subY = 2;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        image.InitAlpha();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
#endif // __WXMSW__ || __WXOSX__

    SECTION("RGBA bitmap without mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, false);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == NULL);

        // Reference image
        const int subX = 2;
        const int subY = 2;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA bitmap with mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, true);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != NULL);

        // Reference image
        const int subX = 2;
        const int subY = 2;
        const int subW = 4;
        const int subH = 5;
        wxImage image = bmp.ConvertToImage().GetSubImage(wxRect(subX, subY, subW, subH));
        REQUIRE(image.HasAlpha() == true);
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == subW);
        REQUIRE(image.GetHeight() == subH);
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        // wxGraphicsBitmap doesn't have a mask so we need wxImage without mask
        // to be compared with created wxGraphicsBitmap.
        BlendMaskWithAlpha(image);
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsMask.
        for ( int y = 0; y < image.GetHeight(); y++ )
            for ( int x = 0; x < image.GetWidth(); x++ )
            {
                if ( image.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    image.SetRGB(x, y, 0, 0, 0);
                }
            }

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != NULL);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
}
#endif // wxUSE_GRAPHICS_CONTEXT

#endif // wxHAS_RAW_BITMAP
