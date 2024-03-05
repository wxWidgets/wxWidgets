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
#if defined(__WXOSX__) || defined(__WXQT__)
// 32 bpp xRGB bitmaps are native ones
typedef wxNativePixelData wxNative32PixelData;
#endif // __WXOSX__ || __WXQT__

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
    REQUIRE(bmp.GetMask() == nullptr);
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
        REQUIRE(bmp.GetMask() != nullptr);
    }

    return bmp;
}

wxBitmap CreateBitmapRGB(int w, int h, bool withMask)
{
    return DoCreateBitmapRGB(w, h, 24, withMask);
}

#if defined(__WXMSW__) || defined(__WXOSX__) || defined(__WXQT__)
// 32-bit RGB bitmap
wxBitmap CreateBitmapXRGB(int w, int h, bool withMask)
{
    return DoCreateBitmapRGB(w, h, 32, withMask);
}
#endif // __WXMSW__ || __WXOSX__ || __WXQT__

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

#ifdef wxHAS_PREMULTIPLIED_ALPHA
        // premultiplied values
        const wxColour clrFgAlpha(((clrFg.Red() * alpha) + 127) / 255, ((clrFg.Green() * alpha) + 127) / 255, ((clrFg.Blue() * alpha) + 127) / 255);
#else
        const wxColour clrFgAlpha(clrFg);
#endif // wxHAS_PREMULTIPLIED_ALPHA

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
    REQUIRE(bmp.GetMask() == nullptr);
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
        REQUIRE(bmp.GetMask() != nullptr);
    }

    return bmp;
}

wxImage MakeReferenceImage(const wxImage& img)
{
    wxImage refImg = img;
    if ( refImg.HasMask() )
    {
        // wxGraphicsBitmap doesn't have a mask so we need wxImage
        // without mask to be compared with created wxGraphicsBitmap.
        if ( refImg.HasAlpha() )
        {
            // We need to blend mask with alpha values
            size_t numPixels = refImg.GetWidth() * refImg.GetHeight();
            unsigned char* oldAlpha = new unsigned char[numPixels];
            memcpy(oldAlpha, refImg.GetAlpha(), numPixels);

            refImg.ClearAlpha();
            refImg.InitAlpha();
            unsigned char* newAlpha = refImg.GetAlpha();
            for ( size_t i = 0; i < numPixels; i++ )
            {
                if ( newAlpha[i] == wxIMAGE_ALPHA_OPAQUE )
                    newAlpha[i] = oldAlpha[i];
            }
            delete[]oldAlpha;
        }
        else
        {
            refImg.InitAlpha();
        }
    }

    if ( refImg.HasAlpha() )
    {
        // We need also to remove mask colour from transparent pixels
        // for compatibility with wxGraphicsBitmap.
        for ( int y = 0; y < refImg.GetHeight(); y++ )
            for ( int x = 0; x < refImg.GetWidth(); x++ )
            {
                if ( refImg.GetAlpha(x, y) == wxALPHA_TRANSPARENT )
                {
                    refImg.SetRGB(x, y, 0, 0, 0);
                }
            }
    }

    return refImg;
}

inline void CheckCreateGraphBitmap(wxGraphicsRenderer* gr, const wxBitmap& srcBmp, const wxImage& srcImg)
{
    wxGraphicsBitmap gbmp = gr->CreateBitmap(srcBmp);

    wxImage gimage = gbmp.ConvertToImage();

    // Reference image needs to be in the format compatible with wxGraphicsBitmap.
    wxImage refImg = MakeReferenceImage(srcImg);

    CHECK_THAT(gimage, RGBASameAs(refImg));
}

inline void CheckCreateGraphSubBitmap(wxGraphicsRenderer* gr, const wxBitmap& srcBmp,
                                      double x, double y, double w, double h,
                                      const wxImage& srcImg)
{
    wxGraphicsBitmap gbmp = gr->CreateBitmap(srcBmp);
    wxGraphicsBitmap gSubBmp = gr->CreateSubBitmap(gbmp, x, y, w, h);

    wxImage gimage = gSubBmp.ConvertToImage();

    // Reference image needs to be in the format compatible with wxGraphicsBitmap.
    wxImage refImg = MakeReferenceImage(srcImg);

    CHECK_THAT(gimage, RGBASameAs(refImg));
}

inline void CheckCreateGraphBitmapFromImage(wxGraphicsRenderer* gr, const wxImage& srcImg)
{
    wxGraphicsBitmap gbmp = gr->CreateBitmapFromImage(srcImg);

    wxImage gimage = gbmp.ConvertToImage();

    // Reference image needs to be in the format compatible with wxGraphicsBitmap.
    wxImage refImg = MakeReferenceImage(srcImg);

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
        REQUIRE(bmp.GetMask() == nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGB bitmap with mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, true);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

#if defined(__WXMSW__) || defined(__WXOSX__) || defined(__WXQT__)
    SECTION("xRGB bitmap without mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, false);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
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
        REQUIRE(bmp.GetMask() != nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE_FALSE(image.HasAlpha());
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
#endif // _WXMSW__ || __WXOSX__ || __WXQT__

    SECTION("RGBA bitmap without mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, false);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE_FALSE(image.HasMask());
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
    }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA bitmap with mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, true);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != nullptr);

        // Reference image
        wxImage image = bmp.ConvertToImage();
        REQUIRE(image.HasAlpha() == true);
        REQUIRE(image.HasMask() == true);
        REQUIRE(image.GetWidth() == bmp.GetWidth());
        REQUIRE(image.GetHeight() == bmp.GetHeight());
        const wxColour maskCol(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
        REQUIRE(maskCol.IsOk());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmap(gr, bmp, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
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
        REQUIRE(bmp.GetMask() == nullptr);

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
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGB bitmap with mask")
    {
        // RGB bitmap
        wxBitmap bmp = CreateBitmapRGB(8, 8, true);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() != nullptr);

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

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

#if defined(__WXMSW__) || defined(__WXOSX__) || defined(__WXQT__)
    SECTION("xRGB bitmap without mask")
    {
        // xRGB bitmap
        wxBitmap bmp = CreateBitmapXRGB(8, 8, false);
        REQUIRE(bmp.GetDepth() == 32);
        REQUIRE_FALSE(bmp.HasAlpha());
        REQUIRE(bmp.GetMask() == nullptr);

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
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
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
        REQUIRE(bmp.GetMask() != nullptr);

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
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
#endif // __WXMSW__ || __WXOSX__ || __WXQT__

    SECTION("RGBA bitmap without mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, false);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() == nullptr);

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
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA bitmap with mask")
    {
        // RGBA Bitmap
        wxBitmap bmp = CreateBitmapRGBA(8, 8, true);
        REQUIRE(bmp.HasAlpha() == true);
        REQUIRE(bmp.GetMask() != nullptr);

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

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            if ( wxIsRunningUnderWine() )
                return;

            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphSubBitmap(gr, bmp, subX, subY, subW, subH, image);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
}

TEST_CASE("GraphicsBitmapTestCase::CreateFromImage", "[graphbitmap][create][fromimage]")
{
    const wxColour maskCol(*wxRED);
    const wxColour fillCol(*wxGREEN);

    SECTION("RGB image without mask")
    {
        wxImage img(4, 4);
        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                if ( x < img.GetWidth() / 2 )
                    img.SetRGB(x, y, maskCol.Red(), maskCol.Green(), maskCol.Blue());
                else
                    img.SetRGB(x, y, fillCol.Red(), fillCol.Green(), fillCol.Blue());
            }
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE_FALSE(img.HasMask());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGB image with mask")
    {
        wxImage img(4, 4);
        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                if ( x < img.GetWidth() / 2 )
                    img.SetRGB(x, y, maskCol.Red(), maskCol.Green(), maskCol.Blue());
                else
                    img.SetRGB(x, y, fillCol.Red(), fillCol.Green(), fillCol.Blue());
            }
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE_FALSE(img.HasAlpha());
        REQUIRE(img.HasMask() == true);

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA image without mask")
    {
        wxImage img(4, 4);
        img.SetAlpha();
        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                if ( x < img.GetWidth() / 2 )
                    img.SetRGB(x, y, maskCol.Red(), maskCol.Green(), maskCol.Blue());
                else
                    img.SetRGB(x, y, fillCol.Red(), fillCol.Green(), fillCol.Blue());

                if ( y < img.GetHeight() / 2 )
                    img.SetAlpha(x, y, 128);
                else
                    img.SetAlpha(x, y, 0);
            }
        REQUIRE(img.HasAlpha() == true);
        REQUIRE_FALSE(img.HasMask());

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }

    SECTION("RGBA image with mask")
    {
        wxImage img(4, 4);
        img.SetAlpha();
        for ( int y = 0; y < img.GetHeight(); y++ )
            for ( int x = 0; x < img.GetWidth(); x++ )
            {
                if ( x < img.GetWidth() / 2 )
                    img.SetRGB(x, y, maskCol.Red(), maskCol.Green(), maskCol.Blue());
                else
                    img.SetRGB(x, y, fillCol.Red(), fillCol.Green(), fillCol.Blue());

                if ( y < img.GetHeight() / 2 )
                    img.SetAlpha(x, y, 128);
                else
                    img.SetAlpha(x, y, 0);
            }
        img.SetMaskColour(maskCol.Red(), maskCol.Green(), maskCol.Blue());
        REQUIRE(img.HasAlpha() == true);
        REQUIRE(img.HasMask() == true);

        SECTION("Default GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDefaultRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }

#if defined(__WXMSW__)
#if wxUSE_GRAPHICS_GDIPLUS
        SECTION("GDI+ GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetGDIPlusRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_GDIPLUS

#if wxUSE_GRAPHICS_DIRECT2D
        SECTION("Direct2D GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetDirect2DRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_DIRECT2D
#endif // __WXMSW__

#if wxUSE_CAIRO
        SECTION("Cairo GC")
        {
            wxGraphicsRenderer* gr = wxGraphicsRenderer::GetCairoRenderer();
            REQUIRE(gr != nullptr);
            CheckCreateGraphBitmapFromImage(gr, img);
        }
#endif // wxUSE_GRAPHICS_CAIRO
    }
}
#endif // wxUSE_GRAPHICS_CONTEXT

#endif // wxHAS_RAW_BITMAP
