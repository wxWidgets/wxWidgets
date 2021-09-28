///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/bmpbndl.h
// Purpose:     wxBitmapBundleImpl declaration.
// Author:      Vadim Zeitlin
// Created:     2021-09-22
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_BMPBNDL_H_
#define _WX_PRIVATE_BMPBNDL_H_

#include "wx/object.h"

// ----------------------------------------------------------------------------
// wxBitmapBundleImpl is the base class for all wxBitmapBundle implementations
// ----------------------------------------------------------------------------

// This class inherits from wxRefCounter to make it possible to use it with
// wxObjectDataPtr in wxBitmapBundle.
class wxBitmapBundleImpl : public wxRefCounter
{
public:
    // Return the size of the bitmaps represented by this bundle in the default
    // DPI (a.k.a. 100% resolution).
    //
    // Must always return a valid size.
    virtual wxSize GetDefaultSize() const = 0;

    // Retrieve the bitmap of exactly the given size.
    //
    // Note that this function is non-const because it may generate the bitmap
    // on demand and cache it.
    virtual wxBitmap GetBitmap(const wxSize size) = 0;

#ifdef __WXOSX__
    // returns the native representation of the bitmap bundle
    virtual WXImage OSXGetImage() const { return NULL; }
#endif
};

#ifdef __WXOSX__

// this methods are wx-private, may change in the future

WXImage WXDLLIMPEXP_CORE wxOSXGetImageFromBundle(const  wxBitmapBundle& bundle);
wxBitmapBundle WXDLLIMPEXP_CORE wxOSXMakeBundleFromImage(WXImage image);
WXImage WXDLLIMPEXP_CORE wxOSXImageFromBitmap(const wxBitmap& bmp);
#if wxOSX_USE_COCOA
void WXDLLIMPEXP_CORE wxOSXAddBitmapToImage(WXImage image, const wxBitmap& bmp);
#endif

#endif

#endif // _WX_PRIVATE_BMPBNDL_H_
