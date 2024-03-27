///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/bmpsvg.cpp
// Purpose:     Generic wxBitmapBundle::FromSVG() implementation
// Author:      Vadim Zeitlin, Gunter Königsmann
// Created:     2021-09-28
// Copyright:   (c) 2019 Gunter Königsmann <wxMaxima@physikbuch.de>
//              (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
//              (c) 2024 PB
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

#if defined(wxHAS_SVG) && !wxUSE_NANOSVG_EXTERNAL && !wxUSE_LUNASVG

// Try to help people updating their sources from Git and forgetting to
// initialize new submodules, if possible: if you get this error, it means that
// your source tree doesn't contain 3rdparty/nanosvg and you should initialize
// and update the corresponding submodule.
#ifdef __has_include
    #if ! __has_include("../../3rdparty/nanosvg/src/nanosvg.h")
        #error You need to run "git submodule update --init 3rdparty/nanosvg" from the wxWidgets directory.
        #undef wxHAS_SVG
    #endif
#endif // __has_include

#endif // wxHAS_SVG

#ifdef wxHAS_SVG

#if wxUSE_LUNASVG

#include <memory>

#include "wx/buffer.h"
#include "wx/log.h"

#include "wx/bmpbndl.h"
#if wxUSE_FFILE
    #include "wx/ffile.h"
#elif wxUSE_FILE
    #include "wx/file.h"
#else
    #define wxNO_SVG_FILE
#endif

#include "wx/rawbmp.h"
#include "wx/private/bmpbndl.h"

// Try to help people updating their sources from Git and forgetting to
// initialize new submodules, if possible: if you get this error, it means that
// your source tree doesn't contain 3rdparty/lunasvg and you should initialize
// and update the corresponding submodule.
#ifdef __has_include
    #if ! __has_include("../../3rdparty/lunasvg/include/lunasvg.h")
        #error You need to run "git submodule update --init 3rdparty/lunasvg" from the wxWidgets directory.
        #undef wxHAS_SVG
    #endif
#endif // __has_include

#include "../../3rdparty/lunasvg/include/lunasvg.h"

class wxBitmapBundleImplSVG : public wxBitmapBundleImpl
{
public:
    // data must be 0 terminated. wxBitmapBundleImplSVG doesn't take ownership
    // so it can be deleted after the ctor is called.
    wxBitmapBundleImplSVG(const char* data, const wxSize& sizeDef);

    // data must be 0 terminated. wxBitmapBundleImplSVG doesn't take ownership
    // so it can be deleted after the ctor is called.
    wxBitmapBundleImplSVG(char* data, const wxSize& sizeDef);

    // wxBitmapBundleImplSVG doesn't take ownership of data so it can be deleted
    // after the ctor is called.
    wxBitmapBundleImplSVG(const wxByte* data, size_t data_len, const wxSize& sizeDef);

    virtual wxSize GetDefaultSize() const override;
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const override;

    virtual wxBitmap GetBitmap(const wxSize& size) override;

    bool IsOk() const;

private:
    wxBitmap DoRasterize(const wxSize& size);

    std::unique_ptr<lunasvg::Document> m_svgDocument;

    const wxSize m_sizeDef;

    // Cache the last used bitmap (may be invalid if not used yet).
    //
    // Note that we cache only the last bitmap and not all the bitmaps ever
    // requested from GetBitmap() for the different sizes because there would be
    // no way to clear such cache and its growth could be unbounded, resulting
    // in too many bitmap objects being used in an application using SVG for all
    // of its icons.
    wxBitmap m_cachedBitmap;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplSVG);
};

// ============================================================================
// wxBitmapBundleImplSVG implementation
// ============================================================================

wxBitmapBundleImplSVG::wxBitmapBundleImplSVG(const char* data, const wxSize& sizeDef)
    : m_sizeDef(sizeDef)
{
    wxCHECK_RET(data != nullptr, "null data");
    wxCHECK_RET(sizeDef.GetWidth() > 0 && sizeDef.GetHeight() > 0, "invalid default size");

    m_svgDocument = lunasvg::Document::loadFromData(data);
}

wxBitmapBundleImplSVG::wxBitmapBundleImplSVG(char* data, const wxSize& sizeDef)
    : m_sizeDef(sizeDef)
{
    wxCHECK_RET(data != nullptr, "null data");
    wxCHECK_RET(sizeDef.GetWidth() > 0 && sizeDef.GetHeight() > 0, "invalid default size");

    m_svgDocument = lunasvg::Document::loadFromData(data);
}

wxBitmapBundleImplSVG::wxBitmapBundleImplSVG(const wxByte* data, size_t len, const wxSize& sizeDef)
    : m_sizeDef(sizeDef)
{
    wxCHECK_RET(data != nullptr, "null data");
    wxCHECK_RET(len > 0, "zero length");
    wxCHECK_RET(sizeDef.GetWidth() > 0 && sizeDef.GetHeight() > 0, "invalid default size");

    m_svgDocument = lunasvg::Document::loadFromData(reinterpret_cast<const char*>(data), len);
}

wxSize wxBitmapBundleImplSVG::GetDefaultSize() const
{
    return m_sizeDef;
};

wxSize wxBitmapBundleImplSVG::GetPreferredBitmapSizeAtScale(double scale) const
{
    return m_sizeDef * scale;
}

wxBitmap wxBitmapBundleImplSVG::GetBitmap(const wxSize& size)
{
    if ( !m_cachedBitmap.IsOk() || m_cachedBitmap.GetSize() != size )
        m_cachedBitmap = DoRasterize(size);

    return m_cachedBitmap;
}

bool wxBitmapBundleImplSVG::IsOk() const
{
    return m_svgDocument != nullptr;
}

wxBitmap wxBitmapBundleImplSVG::DoRasterize(const wxSize& size)
{
    if ( IsOk() )
    {
        // conversion to wxBitmap is based on the code in
        // lunasvg::Bitmap::convert()
        const lunasvg::Bitmap lbmp = m_svgDocument->renderToBitmap(size.x, size.y);

        if ( lbmp.valid() )
        {
            const auto width = lbmp.width();
            const auto height = lbmp.height();
            const auto stride = lbmp.stride();
            auto rowData = lbmp.data();

            wxBitmap bmp(width, height, 32);
            wxAlphaPixelData bmpdata(bmp);
            wxAlphaPixelData::Iterator dst(bmpdata);

            for ( std::uint32_t y = 0; y < height; ++y )
            {
                auto data = rowData;
                dst.MoveTo(bmpdata, 0, y);

                for ( std::uint32_t x = 0; x < width; ++x, ++dst )
                {
                    auto b = data[0];
                    auto g = data[1];
                    auto r = data[2];
                    auto a = data[3];
#ifndef wxHAS_PREMULTIPLIED_ALPHA
                    if (a != 0 )
                    {
                        r = (r * 255) / a;
                        g = (g * 255) / a;
                        b = (b * 255) / a;
                    }
#endif
                    dst.Red()   = r;
                    dst.Green() = g;
                    dst.Blue()  = b;
                    dst.Alpha() = a;

                    data += 4;
                }

                rowData += stride;
            }

            return bmp;
        }
        else
            wxLogDebug("invalid lunasvg::Bitmap");
    }

    return wxBitmap();
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(char* data, const wxSize& sizeDef)
{
    return wxBitmapBundle::FromImpl(new wxBitmapBundleImplSVG(data, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const char* data, const wxSize& sizeDef)
{
    return wxBitmapBundle::FromImpl(new wxBitmapBundleImplSVG(data, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const wxByte* data, size_t len, const wxSize& sizeDef)
{
    return wxBitmapBundle::FromImpl(new wxBitmapBundleImplSVG(data, len, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVGFile(const wxString& path, const wxSize& sizeDef)
{
#if wxUSE_FFILE
    wxFFile file(path, "rb");
#elif wxUSE_FILE
    wxFile file(path);
#else
    #error "wxWidgets must be built with support for wxFFile or wxFile."
#endif
    if (file.IsOpened())
    {
        const wxFileOffset lenAsOfs = file.Length();
        if (lenAsOfs != wxInvalidOffset)
        {
            const size_t len = static_cast<size_t>(lenAsOfs);
            wxMemoryBuffer buf(len);

            if (file.Read(static_cast<char*>(buf.GetWriteBuf(len)), len) == len)
            {
                buf.UngetWriteBuf(len);
                return wxBitmapBundle::FromSVG(static_cast<wxByte*>(buf.GetData()),
                                               len, sizeDef);
            }
        }
    }

    return wxBitmapBundle();
}

#else // !wxUSE_LUNASVG

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
#ifdef wxHAS_PREMULTIPLIED_ALPHA
            // Some platforms require premultiplication by alpha.
            dst.Red()   = src[0] * a / 255;
            dst.Green() = src[1] * a / 255;
            dst.Blue()  = src[2] * a / 255;
            dst.Alpha() = a;
#else
            // Other platforms store bitmaps with straight alpha.
            dst.Alpha() = a;
            if ( a )
            {
                dst.Red()   = src[0];
                dst.Green() = src[1];
                dst.Blue()  = src[2];
            }
            else
                // A more canonical form for completely transparent pixels.
                dst.Red() = dst.Green() = dst.Blue() = 0;
#endif
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

#endif // !wxUSE_LUNASVG

#endif // wxHAS_SVG
