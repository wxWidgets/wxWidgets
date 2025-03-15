///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/aui.h
// Purpose:     Private wxAUI declarations.
// Author:      Vadim Zeitlin
// Created:     2025-03-13
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_AUI_H_
#define _WX_PRIVATE_AUI_H_

#include "wx/bmpbndl.h"

// wxAuiCreateBitmap() is a utility function that creates a bitmap using the
// given colour from monochrome image defined by either SVG (if supported in
// this build) or XBM data.

#ifdef wxHAS_SVG
// SVG data must start with a new line (this is convenient when embedding it as
// a raw string) and use "currentColor" for the colour to be mapped to the
// given colour.
wxBitmapBundle wxAuiCreateBitmap(const char* svgData, int w, int h,
                                 const wxColour& color);
#else // !wxHAS_SVG
// When using XBM, the black bits of the given monochrome bitmap define the
// mask of the returned bitmap and white bits are mapped to the given colour.
wxBitmap wxAuiCreateBitmap(const unsigned char bits[], int w, int h,
                           const wxColour& color);
#endif // wxHAS_SVG/!wxHAS_SVG

// Define some specialized functions to create bitmaps used in both dockart.cpp
// and tabart.cpp.
wxBitmapBundle wxAuiCreateCloseButtonBitmap(const wxColour& color);
wxBitmapBundle wxAuiCreatePinButtonBitmap(const wxColour& color);

#endif // _WX_PRIVATE_AUI_H_
