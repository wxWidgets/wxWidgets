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

#ifdef wxHAS_SVG

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

    // Also make nanosvg.h compile with older MSVC versions which didn't have
    // strtoll().
    #if _MSC_VER < 1800
        #define strtoll _strtoi64
    #endif
#endif

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "../../3rdparty/nanosvg/src/nanosvg.h"
#include "../../3rdparty/nanosvg/src/nanosvgrast.h"

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

#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
    #ifdef __has_include
        #if __has_include(<d2d1_3.h>)
            // Ensure no previous defines interfere with the Direct2D API headers
            #undef GetHwnd
            #include <d2d1_3.h>

            #ifdef _D2D1_SVG_ // defined by a Direct2D header with SVG APIs
                #define wxHAS_BMPBUNDLE_IMPL_SVG_D2D
            #endif // #ifdef _D2D1_SVG_

        #endif // #if __has_include(<d2d1_3.h>)
    #endif // #ifdef __has_include
#endif // #if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D

#ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D
    #include "wx/platinfo.h"
    //#include "wx/msw/private.h"
    #include "wx/msw/private/comptr.h"
    #include "wx/msw/private/graphicsd2d.h"

    #include <combaseapi.h> // for CreateStreamOnHGlobal()
#endif // #ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace
{

// ============================================================================
// wxBitmapBundleImplSVG
// ============================================================================

class wxBitmapBundleImplSVG : public wxBitmapBundleImpl
{
public:
    wxBitmapBundleImplSVG(const wxSize& sizeDef)
        : m_sizeDef(sizeDef)
    {
    }

    virtual wxSize GetDefaultSize() const wxOVERRIDE
    {
        return m_sizeDef;
    };

    virtual wxSize GetPreferredSizeAtScale(double scale) const wxOVERRIDE
    {
        return m_sizeDef*scale;
    }

    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE
    {
        if ( !m_cachedBitmap.IsOk() || m_cachedBitmap.GetSize() != size )
        {
            m_cachedBitmap = DoRasterize(size);
        }

        return m_cachedBitmap;
    }

protected:
    virtual wxBitmap DoRasterize(const wxSize& size) = 0;

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
// wxBitmapBundleImplSVGNano
// ============================================================================

class wxBitmapBundleImplSVGNano : public wxBitmapBundleImplSVG
{
public:
    // Ctor must be passed a valid NSVGimage and takes ownership of it.
    wxBitmapBundleImplSVGNano(NSVGimage* svgImage, const wxSize& sizeDef)
        : wxBitmapBundleImplSVG(sizeDef),
          m_svgImage(svgImage),
          m_svgRasterizer(nsvgCreateRasterizer())
    {
    }

    ~wxBitmapBundleImplSVGNano()
    {
        nsvgDeleteRasterizer(m_svgRasterizer);
        nsvgDelete(m_svgImage);
    }

protected:
    virtual wxBitmap DoRasterize(const wxSize& size) wxOVERRIDE;

private:
    NSVGimage* const m_svgImage;
    NSVGrasterizer* const m_svgRasterizer;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplSVGNano);
};


wxBitmap wxBitmapBundleImplSVGNano::DoRasterize(const wxSize& size)
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

#ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D

// ============================================================================
// wxBitmapBundleImplSVGD2D declaration
// ============================================================================

/*
    wxBitmapBundleImpl using SVG support in Direct2D to
    rasterize an SVG to wxBitmap at given size.

    Run-time limitations: requires Windows 10 Fall Creators Update
    and newer, for limitations of the Direct2D SVG renderer see
    https://docs.microsoft.com/en-us/windows/win32/direct2d/svg-support
    For example, text elements are not supported at all.

    wxBitmapBundleImplSVGD2D implementation limitations: maximum size of
    bitmap for rasterization cannot be larger then wxBitmapBundleImplSVGD2D::ms_maxBitmapSize.
    The SVG must have its width and height specified, if it does not, its viewBox is used
    for the dimensions. If it does noth have width/height nor viewBox, the SVG cannot be rendered.
 */

class wxBitmapBundleImplSVGD2D : public wxBitmapBundleImplSVG
{
public:
    // data must be 0 terminated
    wxBitmapBundleImplSVGD2D(const char* data, const wxSize& sizeDef);

    bool IsOk() const;

    // only available on Windows 10 Fall Creators Update and newer
    static bool IsAvailable();

private:
    // maximum dimension of the bitmap an SVG can be rasterized to
    static const wxSize ms_maxBitmapSize;

    // whether there was an attempt to initialize, regardless of its success
    static bool ms_initialized;
    // large bitmap shared by all instances of wxBitmapBundleImplSVGD2D
    // on which the SVGs are rendered onto
    static wxCOMPtr<IWICBitmap>          ms_bitmap;
    static wxCOMPtr<ID2D1DeviceContext5> ms_context;

    static void Initialize();
    static void Uninitialize();
    static bool IsInitialized();

    // can rasterize bitmaps only up to ms_maxBitmapSize
    static bool CanRasterizeAtSize(const wxSize& size);

    static bool CreateIStreamFromPtr(const char* data, wxCOMPtr<IStream>& SVGStream);

    wxCOMPtr<ID2D1SvgDocument> m_SVGDocument;
    wxSize                     m_SVGDocumentDimensions;

    virtual wxBitmap DoRasterize(const wxSize& size) wxOVERRIDE;

    bool CreateSVGDocument(const wxCOMPtr<IStream>& SVGStream);

    bool GetSVGBitmapFromSharedBitmap(const wxSize& size, wxBitmap& bmp);

    friend class wxBitmapBundleImplSVGD2DModule;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplSVGD2D);
};

// ============================================================================
// wxBitmapBundleImplSVGD2D implementation
// ============================================================================

// This is a maximum size wxBitmapBundleImplSVGD2D can rasterize to.
// The larger the size, the larger memory needed and the memory is
// allocated (ms_bitmap and ms_context)  upon the first use of
// wxBitmapBundleImplSVGD2D and freed only when wxWidgets shuts down.
// The size should be large enough for wxBitmapBundle purpose.
const wxSize wxBitmapBundleImplSVGD2D::ms_maxBitmapSize(512, 512);

bool wxBitmapBundleImplSVGD2D::ms_initialized = false;
wxCOMPtr<IWICBitmap>wxBitmapBundleImplSVGD2D::ms_bitmap;
wxCOMPtr<ID2D1DeviceContext5>wxBitmapBundleImplSVGD2D::ms_context;

wxBitmapBundleImplSVGD2D::wxBitmapBundleImplSVGD2D(const char* data, const wxSize& sizeDef)
    : wxBitmapBundleImplSVG(sizeDef)
{
    wxCHECK_RET(data, "null data");
    wxCHECK_RET(IsAvailable(), "Don't create instances of wxBitmapBundleImplSVGD2D when wxBitmapBundleImplSVGD2D::IsAvailable() returns false");

    wxCOMPtr<IStream> SVGStream;

    if ( CreateIStreamFromPtr(data, SVGStream) )
        CreateSVGDocument(SVGStream);
}

bool wxBitmapBundleImplSVGD2D::IsOk() const
{
    return m_SVGDocument.get() != NULL;
}

bool wxBitmapBundleImplSVGD2D::CreateSVGDocument(const wxCOMPtr<IStream>& SVGStream)
{
    HRESULT     hr;
    D2D1_SIZE_F viewportSize = D2D1::SizeF(32, 32); // viewportSize is ignored when creating SVGDocument

    hr = ms_context->CreateSvgDocument(SVGStream, viewportSize, &m_SVGDocument);
    if ( FAILED (hr) )
    {
        wxLogApiError("ID2D1DeviceContext5::CreateSvgDocument", hr);
        return false;
    }

    // @TODO: Deal with units?

    wxCOMPtr<ID2D1SvgElement> root;
    D2D1_SVG_LENGTH           width, height;

    m_SVGDocument->GetRoot(&root);

    width.value = height.value = 0;
    if ( root->IsAttributeSpecified(L"width") )
        root->GetAttributeValue(L"width", &width);
    if ( root->IsAttributeSpecified(L"height") )
        root->GetAttributeValue(L"height", &height);

    // If the SVG is missing width/height attributes,
    // try to get the viewbox
    if ( (width.value == 0. || height.value == 0.)
         && root->IsAttributeSpecified(L"viewBox") )
    {
        D2D1_SVG_VIEWBOX viewBox;

        hr = root->GetAttributeValue(L"viewBox", D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX, &viewBox, sizeof(viewBox));
        if ( SUCCEEDED(hr) )
        {
            width.value = viewBox.width;
            height.value = viewBox.height;

            wxLogDebug("SVG doesn't have width/height specified, using viewBox instead");
            m_SVGDocument->SetViewportSize(D2D1::SizeF(viewBox.width, viewBox.height));
        }
    }

    if ( width.value == 0. || height.value == 0. )
    {
        wxLogDebug("Couldn't obtain SVG dimensions");
        m_SVGDocument.reset();
        return false;
    }

    m_SVGDocumentDimensions = wxSize(width.value, height.value);
    return true;
}

// Obtains the bitmap drawn on ms_bitmap at (0, 0, size.x, size.y)
bool wxBitmapBundleImplSVGD2D::GetSVGBitmapFromSharedBitmap(const wxSize& size, wxBitmap& bmp)
{
    const WICRect lockRect = { 0, 0, size.x, size.y };

    HRESULT                  hr;
    wxCOMPtr<IWICBitmapLock> lock;

    hr = ms_bitmap->Lock(&lockRect, WICBitmapLockRead, &lock);
    if ( FAILED(hr) )
    {
        wxLogApiError("IWICBitmap::Lock", hr);
        return false;
    }

    wxBitmap bmpOut = wxBitmap(size, 32);

    if ( !bmpOut.IsOk() )
    {
        wxLogDebug("Failed to created wxBitmap bmpOut");
        return false;
    }

    UINT  stride     = 0;
    UINT  bufferSize = 0;
    BYTE* buffer     = NULL;

    hr = lock->GetStride(&stride);
    if ( FAILED(hr) )
    {
        wxLogApiError("IWICBitmapLock::GetStride", hr);
        return false;
    }

    if ( stride != static_cast<UINT>(ms_maxBitmapSize.x) * 4 )
    {
        wxLogDebug("Unexpected bitmap stride");
        return false;
    }

    hr = lock->GetDataPointer(&bufferSize, &buffer);
    if ( FAILED(hr) )
    {
        wxLogApiError("IWICBitmapLock::GetDataPointer", hr);
        return false;
    }

    const unsigned char* src = &buffer[0];
    // we may not want the whole row
    const size_t         rowBytesToSkip = (ms_maxBitmapSize.x - size.x) * 4;

    wxAlphaPixelData           bmpdata(bmpOut);
    wxAlphaPixelData::Iterator dst(bmpdata);

    for ( int y = 0; y < size.y; ++y )
    {
        dst.MoveTo(bmpdata, 0, y);

        // @TODO: Make sure the alpha are OK

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
        src += rowBytesToSkip;
    }

    bmp = bmpOut;
    return true;
}

wxBitmap wxBitmapBundleImplSVGD2D::DoRasterize(const wxSize& size)
{
    if ( !IsOk() )
    {
        wxLogDebug("m_SVGDocument is invalid");
    }
    else
    if ( !CanRasterizeAtSize(size) )
    {
        wxLogDebug("Couldn't rasterize to bitmap with dimensions %dx%d", size.x, size.y);
    }
    else
    {
        const float         scaleValue = wxMin((float)size.x / m_SVGDocumentDimensions.x, (float)size.y / m_SVGDocumentDimensions.y);
        const D2D1_POINT_2F scaleCenter = D2D1::Point2F(size.x / 2.0f, size.y / 2.0f);
        const D2D1_SIZE_F   transl = D2D1::SizeF((size.x - m_SVGDocumentDimensions.x) / 2.0f, (size.y - m_SVGDocumentDimensions.y) / 2.0f);

        D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Identity();
        wxBitmap          bmp;

        // center
        transform = transform * D2D1::Matrix3x2F::Translation(transl);
        // scale
        transform = transform * D2D1::Matrix3x2F::Scale(scaleValue, scaleValue, scaleCenter);

        ms_context->BeginDraw();
        ms_context->Clear();
        ms_context->SetTransform(transform);
        ms_context->DrawSvgDocument(m_SVGDocument);
        ms_context->EndDraw();

        if ( GetSVGBitmapFromSharedBitmap(size, bmp) )
            return bmp;
    }

    return wxBitmap();
}


// static
void wxBitmapBundleImplSVGD2D::Initialize()
{
    if ( IsInitialized() )
        return;
    ms_initialized = true;

    const wxPlatformInfo& platformInfo = wxPlatformInfo::Get();

    // we need at least Windows 10 Fall Creators Update
    if ( !platformInfo.CheckOSVersion(10, 0, 16299) )
        return;

    HRESULT hr;
    UINT    width, height;

    width  = static_cast<UINT>(ms_maxBitmapSize.x);
    height = static_cast<UINT>(ms_maxBitmapSize.y);

    if ( wxWICImagingFactory() == NULL || wxD2D1Factory() == NULL )
    {
        wxLogDebug("wxDirect2D is not available even when it should be.");
        return;
    }

    hr = wxWICImagingFactory()->CreateBitmap(width, height, GUID_WICPixelFormat32bppPRGBA, WICBitmapCacheOnDemand, &ms_bitmap);
    if ( FAILED(hr) )
    {
        wxLogApiError("IWICImagingFactory::CreateBitmap", hr);
        return;
    }

    D2D1_RENDER_TARGET_PROPERTIES props;
    wxCOMPtr<ID2D1RenderTarget>   target;

    props.type                  = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat.format    = DXGI_FORMAT_UNKNOWN;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_UNKNOWN;
    props.dpiX = props.dpiY     = 96;
    props.usage                 = D2D1_RENDER_TARGET_USAGE_NONE;
    props.minLevel              = D2D1_FEATURE_LEVEL_DEFAULT;

    hr = wxD2D1Factory()->CreateWicBitmapRenderTarget(ms_bitmap, props, &target);
    if ( FAILED(hr) )
    {
        wxLogApiError("ID2D1Factory::CreateWicBitmapRenderTarget", hr);
        ms_bitmap.reset();
        return;
    }

    hr = target->QueryInterface(wxIID_PPV_ARGS(ID2D1DeviceContext5, &ms_context));
    if ( FAILED(hr) )
    {
        wxLogApiError("ID2D1RenderTarget::QueryInterface(ID2D1DeviceContext5)", hr);
        target.reset();
        ms_bitmap.reset();
        return;
    }
}

// static
void wxBitmapBundleImplSVGD2D::Uninitialize()
{
    ms_context.reset();
    ms_bitmap.reset();
    ms_initialized = false;
}

// static
bool wxBitmapBundleImplSVGD2D::IsInitialized()
{
    return ms_initialized;
}

// static
bool wxBitmapBundleImplSVGD2D::IsAvailable()
{
    if ( !IsInitialized() )
        Initialize();

    return ms_context.get() != NULL;
}

// static
bool wxBitmapBundleImplSVGD2D::wxBitmapBundleImplSVGD2D::CanRasterizeAtSize(const wxSize& size)
{
    const int width  = size.x;
    const int height = size.y;

    return width > 0 && height > 0
           && width <= ms_maxBitmapSize.x
           && height <= ms_maxBitmapSize.y;
}

// static
 bool wxBitmapBundleImplSVGD2D::CreateIStreamFromPtr(const char* data, wxCOMPtr<IStream>& SVGStream)
 {
     const size_t dataLen = strlen(data);

    HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, dataLen);

    if ( !hMem )
    {
        wxLogDebug("Failed to allocate memory for SVG stream");
        return false;
    }

    // It could be better to use SHCreateMemStream, but more complicated
    // to implement: https://docs.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-shcreatememstream#remarks

    LPVOID buff = ::GlobalLock(hMem);

    if ( !buff )
    {
        wxLogDebug("Failed to lock memory for SVG stream");
        ::GlobalFree(hMem);
        return false;
    }

    memcpy(buff, data, dataLen);
    ::GlobalUnlock(hMem);

    const HRESULT hr = ::CreateStreamOnHGlobal(hMem, TRUE, &SVGStream);

    if ( FAILED(hr) )
    {
        ::GlobalFree(hMem);
        wxLogApiError("::CreateStreamOnHGlobal", hr);
        return false;
    }

    ULARGE_INTEGER largeSize;

    largeSize.QuadPart = dataLen;
    SVGStream->SetSize(largeSize);
    return true;
 }

// wxBitmapBundleImplSVGD2DModule ensures that wxBitmapBundleImplSVGD2D::Uninitialize()
// is called before wxDirect2D shuts down.
class wxBitmapBundleImplSVGD2DModule : public wxModule
{
public:
    wxBitmapBundleImplSVGD2DModule()
    {
        AddDependency("wxDirect2DModule");
    }

    virtual bool OnInit() wxOVERRIDE
    {
        // we will initialize static members of wxBitmapBundleImplSVGD2D
        // only on demand, in wxBitmapBundleImplSVGD2D::Initialize()
        return true;
    }

    virtual void OnExit() wxOVERRIDE
    {
        wxBitmapBundleImplSVGD2D::Uninitialize();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapBundleImplSVGD2DModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapBundleImplSVGD2DModule, wxModule);

#endif // #ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(char* data, const wxSize& sizeDef)
{
#ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D

    if ( wxBitmapBundleImplSVGD2D::IsAvailable() )
    {
        wxBitmapBundle bundle(new wxBitmapBundleImplSVGD2D(data, sizeDef));

        if ( bundle.IsOk() )
            return bundle;
    }

#endif // #ifdef wxHAS_BMPBUNDLE_IMPL_SVG_D2D

    NSVGimage* const svgImage = nsvgParse(data, "px", 96);
    if ( !svgImage )
        return wxBitmapBundle();

    return wxBitmapBundle(new wxBitmapBundleImplSVGNano(svgImage, sizeDef));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromSVG(const char* data, const wxSize& sizeDef)
{
    wxCharBuffer copy(data);

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
