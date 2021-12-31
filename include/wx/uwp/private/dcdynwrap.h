///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/dcdynwrap.h
// Purpose:     Private dynamically loaded HDC-related functions
// Author:      Vadim Zeitlin
// Created:     2016-05-26 (extracted from src/msw/dc.cpp)
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_DCDYNWRAP_H_
#define _WX_MSW_PRIVATE_DCDYNWRAP_H_

#include "wx/msw/wrapwin.h"

// Namespace for the wrapper functions, hopefully one day we'll be able to get
// rid of all of them and then it will be easy to find all occurrences of their
// use by just searching for this namespace name.
//
// All of the functions in this namespace must work *exactly* like the standard
// functions with the same name and just return an error if dynamically loading
// them failed.
//
// And they're all implemented in src/msw/dc.cpp.
namespace wxDynLoadWrappers
{

DWORD GetLayout(HDC hdc);
DWORD SetLayout(HDC hdc, DWORD dwLayout);

BOOL AlphaBlend(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
                HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc,
                BLENDFUNCTION bf);

BOOL GradientFill(HDC hdc, PTRIVERTEX pVert, ULONG numVert,
                  PVOID pMesh, ULONG numMesh, ULONG mode);

} // namespace wxDynLoadWrappers

#endif // _WX_MSW_PRIVATE_DCDYNWRAP_H_
