///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/renderer.h
// Purpose:     Private helpers for wxRendererNative implementation in wxMSW
// Author:      Vadim Zeitlin
// Created:     2026-10-02
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_RENDERER_H_
#define _WX_MSW_PRIVATE_RENDERER_H_

#include "wx/msw/wrapwin.h"

// Helper to render Windows 10/11 title bar glyphs onto an HDC using MDL2/Fluent fonts.
// Attempts font fallback: primary (Win10/11-appropriate) → secondary → Segoe UI Symbol.
//
// Parameters:
//  hdc       - Device context to draw to
//  rc        - Rectangle defining the button bounding box
//  glyphChar - Unicode character code to draw (0 = no-op)
//  textCol   - Text color (COLORREF) to use for drawing
//
// Returns:
//  true if glyph was drawn successfully using one of the preferred fonts,
//  false if no suitable font was found or glyphChar was null
bool wxMSWDrawCaptionGlyph(HDC hdc, const RECT& rc, wchar_t glyphChar, COLORREF textCol);

#endif // _WX_MSW_PRIVATE_RENDERER_H_
