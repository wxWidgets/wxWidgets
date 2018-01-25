/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/graphicsd2d.h
// Purpose:     Allow functions from graphicsd2d.cpp to be used in othe files
// Author:      New Pagodi
// Created:     2017-10-31
// Copyright:   (c) 2017 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_MSW_PRIVATE_GRAPHICSD2D_H_
#define WX_MSW_PRIVATE_GRAPHICSD2D_H_

#if wxUSE_GRAPHICS_DIRECT2D

// Ensure no previous defines interfere with the Direct2D API headers
#undef GetHwnd

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

WXDLLIMPEXP_CORE IWICImagingFactory* wxWICImagingFactory();
WXDLLIMPEXP_CORE ID2D1Factory* wxD2D1Factory();
WXDLLIMPEXP_CORE IDWriteFactory* wxDWriteFactory();

#endif // wxUSE_GRAPHICS_DIRECT2D

#endif // WX_MSW_PRIVATE_GRAPHICSD2D_H_
