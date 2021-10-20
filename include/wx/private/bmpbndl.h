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

#include "wx/bmpbndl.h"

#ifdef __WXOSX__

// this methods are wx-private, may change in the future

WXImage WXDLLIMPEXP_CORE wxOSXGetImageFromBundle(const  wxBitmapBundle& bundle);
wxBitmapBundle WXDLLIMPEXP_CORE wxOSXMakeBundleFromImage(WXImage image);
WXImage WXDLLIMPEXP_CORE wxOSXImageFromBitmap(const wxBitmap& bmp);
#if wxOSX_USE_COCOA
void WXDLLIMPEXP_CORE wxOSXAddBitmapToImage(WXImage image, const wxBitmap& bmp);
#endif

// for hiding the storage of the NSImage with wxBitmapBundleImpls from public API

WXImage WXDLLIMPEXP_CORE wxOSXGetImageFromBundleImpl(const wxBitmapBundleImpl* impl);
void WXDLLIMPEXP_CORE wxOSXSetImageForBundleImpl(const wxBitmapBundleImpl* impl, WXImage image);
void WXDLLIMPEXP_CORE wxOSXBundleImplDestroyed(const wxBitmapBundleImpl* impl);

#endif

#endif // _WX_PRIVATE_BMPBNDL_H_
