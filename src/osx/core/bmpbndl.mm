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

#include "wx/private/bmpbndl.h"

#include "wx/osx/private.h"

#include <algorithm>

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

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
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;

    virtual WXImage OSXGetImage() const wxOVERRIDE;

private:
    WXImage    m_nsImage;
};

} // anonymouse namespace

// ============================================================================
// wxOSXImageBundleImpl implementation
// ============================================================================

wxOSXImageBundleImpl::wxOSXImageBundleImpl(WXImage image)
{
    m_nsImage = (WXImage) wxMacCocoaRetain(image);
}

wxOSXImageBundleImpl::~wxOSXImageBundleImpl()
{
    wxMacCocoaRelease(m_nsImage);
}

wxSize wxOSXImageBundleImpl::GetDefaultSize() const
{
    CGSize sz = wxOSXGetImageSize(m_nsImage);
    return wxSize(sz.width, sz.height);
}

wxBitmap wxOSXImageBundleImpl::GetBitmap(const wxSize& size)
{
    return wxBitmap();
}

WXImage wxOSXImageBundleImpl::OSXGetImage() const
{
    return m_nsImage;
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
    WXImage image = NULL;
    if ( bmp.Ok() )
    {
#if wxOSX_USE_COCOA
        double scale = bmp.GetScaleFactor();
        NSSize sz = NSMakeSize( bmp.GetWidth()*scale, bmp.GetHeight()*scale);
        image = [[NSImage alloc] initWithSize:sz];
        wxOSXAddBitmapToImage(image, bmp);
#else
        wxCFRef<CGImageRef> cgimage = bmp.CreateCGImage();
        image = [[UIImage alloc] initWithCGImage:cgimage scale:bmp.GetScaleFactor() orientation:UIImageOrientationUp];
#endif
        [image autorelease];
    }
    return image;
}

#if wxOSX_USE_COCOA
void wxOSXAddBitmapToImage( WXImage image, const wxBitmap& bmp)
{
    if ( bmp.Ok() && image != NULL )
    {
        wxCFRef<CGImageRef> cgimage = bmp.CreateCGImage();
        NSImageRep* nsrep = [[NSBitmapImageRep alloc] initWithCGImage:cgimage];
        [image addRepresentation:nsrep];
        [nsrep release];
    }
}
#endif

wxBitmapBundle wxBitmapBundle::FromResources(const wxString& name)
{
    wxVector<wxBitmap> resources;

    wxString ext = "png"; // TODO adapt as soon as we get res param
    wxCFStringRef restype(ext);

    for ( int dpiFactor = 1 ; dpiFactor <= 2 ; ++dpiFactor)
    {
        wxString filename;
        if ( dpiFactor == 1 )
            filename = name;
        else
            filename = wxString::Format("%s@%dx", name, dpiFactor);

        wxCFStringRef resname(filename);
        wxCFRef<CFURLRef> imageURL(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, NULL));
        if ( !imageURL.get() && dpiFactor != 1 )
        {
            // try alternate naming scheme
            filename = wxString::Format("%s_%dx", name, dpiFactor);
            resname = wxCFStringRef(filename);
            imageURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, NULL);
        }
        if ( imageURL.get() )
        {
            // Create the data provider object
            wxCFRef<CGDataProviderRef> provider(CGDataProviderCreateWithURL (imageURL) );
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
                resources.push_back(bmp);
            }
        }
    }

    return wxBitmapBundle::FromBitmaps(resources);
}

WXImage wxOSXGetImageFromBundle(const wxBitmapBundle& bundle)
{
    if (!bundle.IsOk())
        return NULL;

    WXImage image = bundle.GetImpl()->OSXGetImage();

    return image;
}
