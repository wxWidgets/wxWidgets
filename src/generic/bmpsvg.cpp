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

#if defined(wxHAS_SVG) && !wxUSE_NANOSVG_EXTERNAL

// Try to help people updating their sources from Git and forgetting to
// initialize new submodules, if possible: if you get this error, it means that
// your source tree doesn't contain 3rdparty/nanosvg and you should initialize
// and update the corresponding submodule.
#ifdef __has_include
    #if ! __has_include("../../3rdparty/nanosvg/src/nanosvg.h")
        #error You need to run "git submodule update --init 3rdparty/nanosvg".
        #undef wxHAS_SVG
    #endif
#endif // __has_include

#endif // wxHAS_SVG

#ifdef wxHAS_SVG

// Note that we have to include NanoSVG headers before including any of wx
// headers, notably wx/unichar.h which defines global operator==() overloads
// for wxUniChar that confuse OpenVMS C++ compiler and break compilation of
// these headers with errors about ambiguous operator==(char,enum).

// This is required by NanoSVG headers, but not included by them.
#include <stdio.h>

// Disable some warnings inside NanoSVG code that we're not interested in.
#ifdef __VISUALC__
    #pragma warning(push)
    #pragma warning(disable:4456)
    #pragma warning(disable:4702)
#endif

#if !wxUSE_NANOSVG_EXTERNAL || defined(wxUSE_NANOSVG_EXTERNAL_ENABLE_IMPL)
    #define NANOSVG_IMPLEMENTATION
    #define NANOSVGRAST_IMPLEMENTATION
    #define NANOSVG_ALL_COLOR_KEYWORDS
#endif

#if wxUSE_NANOSVG_EXTERNAL
    #include <nanosvg.h>
    #include <nanosvgrast.h>
#else
    #include "../../3rdparty/nanosvg/src/nanosvg.h"
    #include "../../3rdparty/nanosvg/src/nanosvgrast.h"
#endif

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"                   // Only for wxMin()
#endif // WX_PRECOMP

#include "wx/bmpbndl.h"
#ifdef wxUSE_FFILE
    #include "wx/ffile.h"
#elif wxUSE_FILE
    #include "wx/file.h"
#else
    #define wxNO_SVG_FILE
#endif
#include "wx/rawbmp.h"

#include "wx/private/bmpbndl.h"

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

    virtual wxSize GetDefaultSize() const override;
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const override;
    virtual wxBitmap GetBitmap(const wxSize& size) override;

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

wxSize wxBitmapBundleImplSVG::GetPreferredBitmapSizeAtScale(double scale) const
{
    // We consider that we can render at any scale.
    return m_sizeDef*scale;
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

    // Somewhat unexpectedly, a non-null but empty image is returned even if
    // the data is not SVG at all, e.g. without this check creating a bundle
    // from any random file with FromSVGFile() would "work".
    if ( svgImage->width == 0 && svgImage->height == 0 && !svgImage->shapes )
    {
        nsvgDelete(svgImage);
        return wxBitmapBundle();
    }

    return wxBitmapBundle(new wxBitmapBundleImplSVG(svgImage, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const char* data, const wxSize& sizeDef)
{
    wxCharBuffer copy(data);

    return FromSVG(copy.data(), sizeDef);
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const wxByte* data, size_t len, const wxSize& sizeDef)
{
    wxCharBuffer copy(len);
    memcpy(copy.data(), data, len);

    return FromSVG(copy.data(), sizeDef);
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVGFile(const wxString& path, const wxSize& sizeDef)
{
    // There is nsvgParseFromFile(), but it doesn't work with Unicode filenames
    // under MSW and does exactly the same thing that we do here in any case,
    // so it seems better to use our code.
#ifndef wxNO_SVG_FILE
#if wxUSE_FFILE
    wxFFile file(path, "rb");
#elif wxUSE_FILE
    wxFile file(path);
#endif
    if ( file.IsOpened() )
    {
        const wxFileOffset lenAsOfs = file.Length();
        if ( lenAsOfs != wxInvalidOffset )
        {
            const size_t len = static_cast<size_t>(lenAsOfs);

            wxCharBuffer buf(len);
            char* const ptr = buf.data();
            if ( file.Read(ptr, len) == len )
                return wxBitmapBundle::FromSVG(ptr, sizeDef);
        }
    }
#endif // !wxNO_SVG_FILE

    return wxBitmapBundle();
}

#endif // wxHAS_SVG
