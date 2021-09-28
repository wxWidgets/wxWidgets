///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/bmpsvg.cpp
// Purpose:     Generic wxBitmapBundle::FromSVG() implementation
// Author:      Vadim Zeitlin, Gunter Königsmann
// Created:     2021-09-28
// Copyright:   (c) 2019 Gunter Königsmann <wxMaxima@physikbuch.de>
//              (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_RAW_BITMAP

#ifndef WX_PRECOMP
    #include "wx/utils.h"                   // Only for wxMin()
#endif // WX_PRECOMP

#include "wx/bmpbndl.h"
#include "wx/rawbmp.h"

#include "wx/private/bmpbndl.h"

// Disable some warnings inside NanoSVG code that we're not interested in.
#ifdef __VISUALC__
    #pragma warning(push)
    #pragma warning(disable:4456)
    #pragma warning(disable:4702)

    // Also make nanosvg.h compile with older MSVC versions which didn't have
    // strtoll().
    #if _MSC_VER < 1800
        #define strtoll _strtoi64
    #endif
#endif

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "../../3rdparty/nanosvg/src/nanosvg.h"
#include "../../3rdparty/nanosvg/src/nanosvgrast.h"

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace
{

class wxBitmapBundleImplSVG : public wxBitmapBundleImpl
{
public:
    // Ctor must be passed a valid NSVGimage and takes ownership of it.
    wxBitmapBundleImplSVG(NSVGimage* svgImage, const wxSize& sizeDef)
        : m_svgImage(svgImage),
          m_svgRasterizer(nsvgCreateRasterizer()),
          m_sizeDef(sizeDef)
    {
    }

    ~wxBitmapBundleImplSVG()
    {
        nsvgDeleteRasterizer(m_svgRasterizer);
        nsvgDelete(m_svgImage);
    }

    virtual wxSize GetDefaultSize() const wxOVERRIDE;
    virtual wxBitmap GetBitmap(const wxSize size) wxOVERRIDE;

private:
    NSVGimage* const m_svgImage;
    NSVGrasterizer* const m_svgRasterizer;

    const wxSize m_sizeDef;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplSVG);
};

} // anonymous namespace

// ============================================================================
// wxBitmapBundleImplSVG implementation
// ============================================================================

wxSize wxBitmapBundleImplSVG::GetDefaultSize() const
{
    return m_sizeDef;
}

wxBitmap wxBitmapBundleImplSVG::GetBitmap(const wxSize size)
{
    // TODO: Cache.

    wxVector<unsigned char> buffer(size.x*size.y*4);
    nsvgRasterize
    (
        m_svgRasterizer,
        m_svgImage,
        0.0, 0.0,           // no offset
        wxMin
        (
            size.x/m_svgImage->width,
            size.y/m_svgImage->height
        ),                  // scale
        &buffer[0],
        size.x, size.y,
        size.x*4            // stride -- we have no gaps between lines
    );

    wxBitmap bitmap(size, 32);
    wxAlphaPixelData bmpdata(bitmap);
    wxAlphaPixelData::Iterator dst(bmpdata);

    const unsigned char* src = &buffer[0];
    for ( int y = 0; y < size.y; ++y )
    {
        dst.MoveTo(bmpdata, 0, y);
        for ( int x = 0; x < size.x; ++x )
        {
            const unsigned char a = src[3];
            dst.Red()   = src[0] * a / 255;
            dst.Green() = src[1] * a / 255;
            dst.Blue()  = src[2] * a / 255;
            dst.Alpha() = a;

            ++dst;
            src += 4;
        }
    }

    return bitmap;
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(char* data, const wxSize sizeDef)
{
    NSVGimage* const svgImage = nsvgParse(data, "px", 96);
    if ( !svgImage )
        return wxBitmapBundle();

    return wxBitmapBundle(new wxBitmapBundleImplSVG(svgImage, sizeDef));
}

#endif // wxHAS_RAW_BITMAP
