///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/paint.h
// Purpose:     Helpers for handling repainting
// Author:      Vadim Zeitlin
// Created:     2020-02-10
// Copyright:   (c) 2020 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_PAINT_H_
#define _WX_MSW_PRIVATE_PAINT_H_

#include "wx/stack.h"

namespace wxMSWImpl
{

// Data used by WM_PAINT handler
struct PaintData
{
    explicit PaintData(wxWindowMSW* window_)
        : window(window_),
          createdPaintDC(false)
    {
    }

    // The window being repainted (never null).
    wxWindowMSW* const window;

    // True if the user-defined paint handler created wxPaintDC.
    bool createdPaintDC;
};

// Stack storing data for the possibly nested WM_PAINT handlers.
extern wxStack<PaintData> paintStack;

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_PAINT_H_
