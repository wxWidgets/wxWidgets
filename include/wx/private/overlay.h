/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_OVERLAY_H_
#define _WX_PRIVATE_OVERLAY_H_

#include "wx/overlay.h"

#if defined(__WXDFB__)
    #define wxHAS_NATIVE_OVERLAY 1
#elif defined(__WXOSX__) && wxOSX_USE_COCOA
    #define wxHAS_NATIVE_OVERLAY 1
#elif defined(__WXGTK3__)
    #define wxHAS_NATIVE_OVERLAY 1
    #define wxHAS_GENERIC_OVERLAY 1
#else
    #define wxHAS_GENERIC_OVERLAY 1
#endif

class wxOverlay::Impl
{
public:
    virtual ~Impl();
    virtual bool IsNative() const;
    virtual bool IsOk() = 0;
    virtual void Init(wxDC* dc, int x, int y, int width, int height) = 0;
    virtual void BeginDrawing(wxDC* dc) = 0;
    virtual void EndDrawing(wxDC* dc) = 0;
    virtual void Clear(wxDC* dc) = 0;
    virtual void Reset() = 0;
};

#endif // _WX_PRIVATE_OVERLAY_H_
