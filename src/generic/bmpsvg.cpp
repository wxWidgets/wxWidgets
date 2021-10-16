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

#include "wx/bmpbndl.h"

#ifdef wxHAS_SVG

#ifndef WX_PRECOMP
    #include "wx/utils.h"                   // Only for wxMin()
#endif // WX_PRECOMP

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
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;

private:
    wxBitmap DoRasterize(const wxSize& size);

    NSVGimage* const m_svgImage;
    NSVGrasterizer* const m_svgRasterizer;

    const wxSize m_sizeDef;

    // Cache the last used bitmap (may be invalid if not used yet).
    //
    // Note that we cache only the last bitmap and not all the bitmaps ever
    // requested from GetBitmap() for the different sizes because there would
    // be no way to clear such cache and its growth could be unbounded,
    // resulting in too many bitmap objects being used in an application using
    // SVG for all of its icons.
    wxBitmap m_cachedBitmap;

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

wxBitmap wxBitmapBundleImplSVG::GetBitmap(const wxSize& size)
{
    if ( !m_cachedBitmap.IsOk() || m_cachedBitmap.GetSize() != size )
    {
        m_cachedBitmap = DoRasterize(size);
    }

    return m_cachedBitmap;
}

wxBitmap wxBitmapBundleImplSVG::DoRasterize(const wxSize& size)
{
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
wxBitmapBundle wxBitmapBundle::FromSVG(char* data, const wxSize& sizeDef)
{
    NSVGimage* const svgImage = nsvgParse(data, "px", 96);
    if ( !svgImage )
        return wxBitmapBundle();

    return wxBitmapBundle(new wxBitmapBundleImplSVG(svgImage, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const char* data, const wxSize& sizeDef)
{
    wxCharBuffer copy(data);

    return FromSVG(copy.data(), sizeDef);
}

#endif // wxHAS_SVG
