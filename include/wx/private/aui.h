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

// This is a utility function that creates a bitmap with mask corresponding to
// the black bits of the given (as raw bits in XBM format) monochrome bitmap
// and white bits mapped to the given colour.
wxBitmap wxAuiCreateBitmap(const unsigned char bits[], int w, int h,
                           const wxColour& color);

#endif // _WX_PRIVATE_AUI_H_
