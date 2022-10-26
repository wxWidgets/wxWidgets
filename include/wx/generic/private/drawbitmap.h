///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/drawbitmap.h
// Purpose:     Small helper for drawing images.
// Author:      Vadim Zeitlin
// Created:     2022-10-25
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_DRAWBITMAP_H_
#define _WX_GENERIC_PRIVATE_DRAWBITMAP_H_

#include "wx/dc.h"
#include "wx/window.h"
#include "wx/withimages.h"

// Just a trivial wrapper for wxDC::DrawBitmap() using wxWithImages: this is
// used in several places in the generic wxListCtrl and wxTreeCtrl code.
inline void
wxDrawImageBitmap(wxWindow* window,
                  const wxWithImages& images,
                  int image,
                  wxDC& dc,
                  int x,
                  int y)
{
    dc.DrawBitmap(images.GetImageBitmapFor(window, image),
                  x, y,
                  true /* use mask */);
}

// Overload for the controls deriving from both wxWindow and wxWithImages, as
// both wxListCtrl and wxTreeCtrl do.
template <typename T>
inline void
wxDrawImageBitmap(T* window, int image, wxDC& dc, int x, int y)
{
    wxDrawImageBitmap(window, *window, image, dc, x, y);
}

#endif // _WX_GENERIC_PRIVATE_DRAWBITMAP_H_
