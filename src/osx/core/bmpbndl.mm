///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/bmpbndl.mm
// Purpose:     OSX-specific part of wxBitmapBundle class.
// Author:      Vadim Zeitlin and Stefan Csomor
// Created:     2021-09-22
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
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

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/bmpbndl.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

#include "wx/private/bmpbndl.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#include <algorithm>
#include <unordered_map>

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace {

class wxOSXImageHolder
{
public:
    wxOSXImageHolder() : m_nsImage(nullptr)
    {
    }

    explicit wxOSXImageHolder( WXImage image) : m_nsImage(image)
    {
        [m_nsImage retain];
    }

    wxOSXImageHolder( const wxOSXImageHolder& other ) : m_nsImage(other.m_nsImage)
    {
        [m_nsImage retain];
    }

    ~wxOSXImageHolder()
    {
        [m_nsImage release];
    }

    wxOSXImageHolder& operator=(const wxOSXImageHolder& other)
    {
        if ( other.m_nsImage != m_nsImage )
        {
            [m_nsImage release];
            m_nsImage = other.m_nsImage;
            [m_nsImage retain];
        }
        return *this;
    }

    WXImage GetImage() const { return m_nsImage; }
private:
    WXImage    m_nsImage;
};

} // anonymouse namespace

std::unordered_map< const wxBitmapBundleImpl*, wxOSXImageHolder> gs_nativeImages;

WXImage WXDLLIMPEXP_CORE wxOSXGetImageFromBundleImpl(const wxBitmapBundleImpl* impl)
{
    auto image = gs_nativeImages.find(impl);
    if (image != gs_nativeImages.end())
        return image->second.GetImage();
    else
        return nullptr;
}

void WXDLLIMPEXP_CORE wxOSXSetImageForBundleImpl(const wxBitmapBundleImpl* impl, WXImage image)
{
    gs_nativeImages[impl] = wxOSXImageHolder(image);
}

void WXDLLIMPEXP_CORE wxOSXBundleImplDestroyed(const wxBitmapBundleImpl* impl)
{
    gs_nativeImages.erase(impl);
}


namespace
{
// Bundle implementation using PNG bitmaps from Windows resources.
class wxOSXImageBundleImpl : public wxBitmapBundleImpl
{
public:
    // Ctor takes the vector containing all the existing resources starting
    // with the given name and the valid bitmap corresponding to the base name
    // resource itself.
    wxOSXImageBundleImpl(WXImage image);
    ~wxOSXImageBundleImpl();

    virtual wxSize GetDefaultSize() const override;
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const override;
    virtual wxBitmap GetBitmap(const wxSize& size) override;
};

} // anonymouse namespace

// ============================================================================
// wxOSXImageBundleImpl implementation
// ============================================================================

wxOSXImageBundleImpl::wxOSXImageBundleImpl(WXImage image)
{
    wxOSXSetImageForBundleImpl(this, image);
}

wxOSXImageBundleImpl::~wxOSXImageBundleImpl()
{
}

wxSize wxOSXImageBundleImpl::GetDefaultSize() const
{
    CGSize sz = wxOSXGetImageSize(wxOSXGetImageFromBundleImpl(this));
    return wxSize(sz.width, sz.height);
}

wxSize wxOSXImageBundleImpl::GetPreferredBitmapSizeAtScale(double scale) const
{
    // The system always performs scaling, as the scaling factor is integer and
    // so it doesn't make sense to round it up or down, hence we should use the
    // theoretical best size for given scale.
    return GetDefaultSize()*scale;
}

wxBitmap wxOSXImageBundleImpl::GetBitmap(const wxSize& WXUNUSED(size))
{
    return wxBitmap(wxOSXGetImageFromBundleImpl(this));
}

wxBitmapBundle wxOSXMakeBundleFromImage( WXImage img)
{
    return wxBitmapBundle::FromImpl( new wxOSXImageBundleImpl(img) );
}

// ============================================================================
// wxOSXSFSymbolBundleImpl
// ============================================================================
//
// Bundle implementation for macOS SF Symbols. Unlike wxOSXImageBundleImpl,
// this keeps the symbol name around so we can regenerate the underlying
// NSImage at any requested size.  SF symbols are effectively vector images,
// so regenerating produces crisp output for every display scale.  The native
// image cache is pre-populated with the symbol at the bundle's default size,
// ensuring widgets retrieving the bundle's native NSImage still receive a
// proper template image that adapts to light/dark appearance.

#if wxOSX_USE_COCOA

namespace
{

class wxOSXSFSymbolBundleImpl : public wxBitmapBundleImpl
{
public:
    wxOSXSFSymbolBundleImpl(const wxString& symbolName, const wxSize& defaultSize)
        : m_symbolName(symbolName), m_defaultSize(defaultSize)
    {
        // Pre-populate the native image cache so widget code paths that
        // retrieve the NSImage directly (via wxOSXGetImageFromBundle) get
        // the SF symbol image with its template flag intact.
        WXImage image = CreateSymbolImage(defaultSize);
        if ( image )
            wxOSXSetImageForBundleImpl(this, image);
    }

    virtual wxSize GetDefaultSize() const override { return m_defaultSize; }

    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const override
    {
        return wxSize(wxRound(m_defaultSize.x * scale),
                      wxRound(m_defaultSize.y * scale));
    }

    virtual wxBitmap GetBitmap(const wxSize& size) override
    {
        const wxSize sz = (size == wxDefaultSize) ? m_defaultSize : size;

        const bool darkTint = IsDarkDrawingAppearance();
        if ( m_cachedBitmap.IsOk() && m_cachedBitmap.GetSize() == sz &&
                m_cachedForDark == darkTint )
            return m_cachedBitmap;

        // The requested size is in pixels, while NSImage sizes are in
        // points, so we can't simply wrap an NSImage of this size in
        // wxBitmap: it would be rasterized using the main screen scale
        // factor and end up e.g. twice as big as requested on a Retina
        // display. Instead, rasterize the symbol ourselves at exactly the
        // requested pixel size, just as the SVG-based bundle implementation
        // does, and let wxBitmapBundle::GetBitmap() adjust the scale factor
        // of the returned bitmap if needed.
        wxBitmap bmp;
        WXImage image = CreateSymbolImage(sz);
        if ( image )
        {
            CGContextRef context = CGBitmapContextCreate(
                nullptr, sz.x, sz.y, 8, 0,
                wxMacGetGenericRGBColorSpace(),
                kCGImageAlphaPremultipliedFirst);
            if ( context )
            {
                CGContextClearRect(context, CGRectMake(0, 0, sz.x, sz.y));

                NSGraphicsContext* const previous = NSGraphicsContext.currentContext;
                NSGraphicsContext.currentContext =
                    [NSGraphicsContext graphicsContextWithCGContext:context
                                                            flipped:NO];
                [image drawInRect:NSMakeRect(0, 0, sz.x, sz.y)
                         fromRect:NSZeroRect
                        operation:NSCompositingOperationSourceOver
                         fraction:1.0];

                // When AppKit draws a symbol image, it renders it using the
                // label color of the current appearance (e.g. light in dark
                // mode). The rasterized bitmap loses that machinery, so
                // bake the label color in here, keeping the alpha channel as
                // the symbol shape.
                CGContextSetBlendMode(context, kCGBlendModeSourceIn);
                CGContextSetFillColorWithColor(context,
                                               NSColor.labelColor.CGColor);
                CGContextFillRect(context, CGRectMake(0, 0, sz.x, sz.y));

                NSGraphicsContext.currentContext = previous;

                CGImageRef cgImage = CGBitmapContextCreateImage(context);
                if ( cgImage )
                {
                    // Mark the bitmap as a template so that, when it is
                    // drawn, it is tinted for the current (light or dark)
                    // appearance just as the SF symbol NSImage itself
                    // would be.
                    bmp = wxBitmap(cgImage, 1.0, true /* template */);
                    CGImageRelease(cgImage);
                }
                CGContextRelease(context);
            }
        }

        // Cache only the last used bitmap, as the SVG implementation does,
        // to avoid unbounded growth while still helping the common case of
        // the same size being requested repeatedly.
        m_cachedBitmap = bmp;
        m_cachedForDark = darkTint;

        return bmp;
    }

    // Return true if the symbol name resolved to an actual SF symbol.
    bool IsOk() const { return wxOSXGetImageFromBundleImpl(this) != nullptr; }

    // Return true if the current drawing appearance is a dark one, i.e. if
    // the label color used for tinting resolves to a light color.
    static bool IsDarkDrawingAppearance()
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
        if ( WX_IS_MACOS_AVAILABLE(11, 0) )
        {
            NSAppearance* const appearance =
                NSAppearance.currentDrawingAppearance;
            return [appearance bestMatchFromAppearancesWithNames:
                        @[NSAppearanceNameAqua, NSAppearanceNameDarkAqua]]
                    == NSAppearanceNameDarkAqua;
        }
#endif
        return false;
    }

private:
    WXImage CreateSymbolImage(const wxSize& size) const
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
        if ( WX_IS_MACOS_AVAILABLE(11, 0) )
        {
            wxCFStringRef cfname(m_symbolName);
            NSImage* symbol =
                [NSImage imageWithSystemSymbolName:cfname.AsNSString()
                          accessibilityDescription:nil];
            if ( symbol )
            {
                // Configure the symbol at a point size matching the
                // requested height (symbols are laid out relative to the cap
                // height, so the height, and not the width, determines the
                // appropriate stroke weight for the rendered size).
                NSImageSymbolConfiguration* config =
                    [NSImageSymbolConfiguration
                        configurationWithPointSize:size.GetHeight()
                                            weight:NSFontWeightRegular];
                NSImage* configured =
                    [symbol imageWithSymbolConfiguration:config];
                if ( configured )
                    symbol = configured;

                [symbol setSize:NSMakeSize(size.x, size.y)];
                // SF symbols are template images; ensure the flag is set
                // so AppKit tints them for the current light/dark mode.
                [symbol setTemplate:YES];
                return symbol;
            }
        }
#else
        wxUnusedVar(size);
#endif
        return nullptr;
    }

    const wxString m_symbolName;
    const wxSize   m_defaultSize;

    // Last returned bitmap and the appearance it was tinted for, see
    // GetBitmap().
    wxBitmap       m_cachedBitmap;
    bool           m_cachedForDark = false;
};

} // anonymous namespace

#endif // wxOSX_USE_COCOA

wxBitmapBundle wxOSXMakeBundleForSystemSymbol(const wxString& name, const wxSize& defaultSize)
{
#if wxOSX_USE_COCOA
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_16
    if ( WX_IS_MACOS_AVAILABLE(11, 0) )
    {
        wxSize sz = defaultSize;
        if ( sz == wxDefaultSize )
            sz = wxSize(32, 32);

        // The ctor tries to create the symbol image to pre-populate the
        // native image cache, so it also serves as the existence check for
        // the symbol name, without requiring a separate lookup here.
        wxOSXSFSymbolBundleImpl* const impl =
            new wxOSXSFSymbolBundleImpl(name, sz);
        if ( impl->IsOk() )
            return wxBitmapBundle::FromImpl(impl);

        impl->DecRef();
    }
#endif
#else
    wxUnusedVar(name);
    wxUnusedVar(defaultSize);
#endif
    return wxBitmapBundle();
}

// ============================================================================
// wxOSXImageBundleImpl implementation
// ============================================================================

WXImage wxOSXImageFromBitmap( const wxBitmap& bmp)
{
    WXImage image;
#if wxOSX_USE_COCOA
    NSSize sz = NSMakeSize(bmp.GetLogicalWidth(), bmp.GetLogicalHeight());
    image = [[NSImage alloc] initWithSize:sz];
    wxOSXAddBitmapToImage(image, bmp);
#else
    wxCFRef<CGImageRef> cgimage = bmp.CreateCGImage();
    image = [[UIImage alloc] initWithCGImage:cgimage scale:bmp.GetScaleFactor() orientation:UIImageOrientationUp];
#endif
    [image autorelease];
    return image;
}

#if wxOSX_USE_COCOA
void wxOSXAddBitmapToImage( WXImage image, const wxBitmap& bmp)
{
    wxCFRef<CGImageRef> cgimage = bmp.CreateCGImage();
    NSImageRep* nsrep = [[NSBitmapImageRep alloc] initWithCGImage:cgimage];
    [image addRepresentation:nsrep];
    [nsrep release];
}
#endif

wxBitmapBundle wxBitmapBundle::FromFiles(const wxString& path, const wxString& filename, const wxString& extension)
{
    wxVector<wxBitmap> bitmaps;

    wxFileName fn(path, filename, extension);
    wxString ext = extension.Lower();

    for ( int dpiFactor = 1 ; dpiFactor <= 2 ; ++dpiFactor)
    {
        if ( dpiFactor == 1 )
            fn.SetName(filename);
        else
            fn.SetName(wxString::Format("%s@%dx", filename, dpiFactor));

        if ( !fn.FileExists() && dpiFactor != 1 )
        {
            // try alternate naming scheme
            fn.SetName(wxString::Format("%s_%dx", filename, dpiFactor));
        }

        if ( fn.FileExists() )
        {
            wxCFRef<CFURLRef> imageURL(wxOSXCreateURLFromFileSystemPath(fn.GetFullPath()));
            // Create the data provider object
            wxCFRef<CGDataProviderRef> provider(CGDataProviderCreateWithURL(imageURL));
            CGImageRef image = nullptr;

            if ( ext == "jpeg" )
                image = CGImageCreateWithJPEGDataProvider (provider, nullptr, true,
                                                       kCGRenderingIntentDefault);
            else if ( ext == "png" )
                image = CGImageCreateWithPNGDataProvider (provider, nullptr, true,
                                                           kCGRenderingIntentDefault);
            if ( image != nullptr )
            {
                wxBitmap bmp(image, dpiFactor);
                CGImageRelease(image);
                bitmaps.push_back(bmp);
            }
        }
    }

    return wxBitmapBundle::FromBitmaps(bitmaps);
}

wxBitmapBundle wxBitmapBundle::FromResources(const wxString& name)
{
    return wxBitmapBundle::FromFiles(wxStandardPaths::Get().GetResourcesDir(), name, "png");
}

WXImage wxOSXGetImageFromBundle(const wxBitmapBundle& bundle)
{
    if (!bundle.IsOk())
        return nullptr;

    wxBitmapBundleImpl* impl = bundle.GetImpl();

    WXImage image = wxOSXGetImageFromBundleImpl(impl);

    if (image == nullptr)
    {
        wxSize sz = impl->GetDefaultSize();

#if wxOSX_USE_COCOA
        wxBitmap bmp = const_cast<wxBitmapBundleImpl*>(impl)->GetBitmap(sz);
        image = wxOSXImageFromBitmap(bmp);

        // unconditionally try to add a 2x version, if there really is a different one
        wxSize doublesz = impl->GetPreferredBitmapSizeAtScale(2.0);
        if ( doublesz != sz )
        {
            bmp = const_cast<wxBitmapBundleImpl*>(impl)->GetBitmap(doublesz);
            if ( bmp.IsOk() )
                wxOSXAddBitmapToImage(image, bmp);
        }
#else
        double scale = wxOSXGetMainScreenContentScaleFactor();
        wxSize scaledSize = sz * scale;
        wxBitmap bmp = const_cast<wxBitmapBundleImpl*>(impl)->GetBitmap(scaledSize);
        if ( bmp.IsOk() )
            image = wxOSXImageFromBitmap(bmp);
        else if ( scale > 1.9 )
        {
            // if we are on a high dpi device and no matching bitmap is available
            // use scale 1x
            bmp = const_cast<wxBitmapBundleImpl*>(impl)->GetBitmap(sz);
            if ( bmp.IsOk() )
                image = wxOSXImageFromBitmap(bmp);
        }
#endif
        if ( image )
            wxOSXSetImageForBundleImpl(impl, image);
    }

    return image;
}

#ifdef wxHAS_SVG
wxBitmapBundle wxBitmapBundle::FromSVGResource(const wxString& name, const wxSize &sizeDef)
{
    return wxBitmapBundle::FromSVGFile(wxFileName(wxStandardPaths::Get().GetResourcesDir(), name, "svg").GetFullPath(), sizeDef);
}
#endif // #ifdef wxHAS_SVG
