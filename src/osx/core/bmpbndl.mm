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

#include <algorithm>
#include <unordered_map>

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace {

class wxOSXImageHolder
{
public:
    wxOSXImageHolder() : m_nsImage(NULL)
    {
    }

    explicit wxOSXImageHolder( WXImage image) : m_nsImage(image)
    {
        [m_nsImage retain];
    }

    wxOSXImageHolder( const wxOSXImageHolder& other ) : m_nsImage(other.m_nsImage)
    {
        [m_nsImage retain];;
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
        return NULL;
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

    virtual wxSize GetDefaultSize() const wxOVERRIDE;
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE;
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;
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
            CGImageRef image = NULL;

            if ( ext == "jpeg" )
                image = CGImageCreateWithJPEGDataProvider (provider, NULL, true,
                                                       kCGRenderingIntentDefault);
            else if ( ext == "png" )
                image = CGImageCreateWithPNGDataProvider (provider, NULL, true,
                                                           kCGRenderingIntentDefault);
            if ( image != NULL )
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
        return NULL;

    wxBitmapBundleImpl* impl = bundle.GetImpl();

    WXImage image = wxOSXGetImageFromBundleImpl(impl);

    if (image == 0)
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
