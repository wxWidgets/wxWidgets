///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/custompaint.h
// Purpose:     Helper function for customizing the standard WM_PAINT handling.
// Author:      Vadim Zeitlin
// Created:     2023-06-02
// Copyright:   (c) 2023 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_CUSTOMPAINT_H_
#define _WX_MSW_PRIVATE_CUSTOMPAINT_H_

#include "wx/dcmemory.h"
#include "wx/image.h"

#include "wx/msw/dc.h"

namespace wxMSWImpl
{

// This function can be used as CustomPaint() callback to post process the
// bitmap by applying the specific functor to each of its pixels.
template <typename FuncProcessPixel>
wxBitmap
PostPaintEachPixel(const wxBitmap& bmp, FuncProcessPixel processPixel)
{
#if wxUSE_IMAGE
    wxImage image = bmp.ConvertToImage();

    unsigned char *data = image.GetData();
    unsigned char *alpha = image.GetAlpha();
    unsigned char alphaOpaque = wxALPHA_OPAQUE;
    const int len = image.GetWidth()*image.GetHeight();
    for ( int i = 0; i < len; ++i, data += 3 )
    {
        processPixel(data[0], data[1], data[2], alpha ? *alpha++ : alphaOpaque);
    }

    return wxBitmap(image);
#else // !wxUSE_IMAGE
    return bmp;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

// This function uses the default WM_PAINT handler to paint the window contents
// into a bitmap and then the provided function to tweak the pixels of this
// bitmap.
//
// The first argument is a functor (typically a lambda) to paint the on the
// given HDC and the second one is another functor called to post process the
// bitmap before actually drawing it.
//
// It can only be called from WM_PAINT handler for a native control and assumes
// that this control handles WPARAM argument of WM_PAINT as HDC to paint on.
template <typename FuncDefPaint, typename FuncPostProcess>
void
CustomPaint(HWND hwnd, FuncDefPaint defPaint, FuncPostProcess postProcess)
{
    const RECT rc = wxGetClientRect(hwnd);
    const wxSize size{rc.right - rc.left, rc.bottom - rc.top};

    // Don't bother doing anything with the empty windows.
    if ( size == wxSize() )
        return;

    // Ask the control to paint itself on the given bitmap.
    wxBitmap bmp(size);
    {
        wxMemoryDC mdc(bmp);

        defPaint(hwnd, (WPARAM)GetHdcOf(mdc));
    }

    PAINTSTRUCT ps;
    wxDCTemp dc(::BeginPaint(hwnd, &ps), size);
    dc.DrawBitmap(postProcess(bmp), 0, 0);
    ::EndPaint(hwnd, &ps);
}

} // namespace wxMSWImpl

#endif // _WX_MSW_PRIVATE_CUSTOMPAINT_H_
