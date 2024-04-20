///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/drawresize.h
// Purpose:     Helper function for drawing "resize hint".
// Author:      Vadim Zeitlin
// Created:     2024-04-20
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_DRAWRESIZE_H_
#define _WX_GENERIC_PRIVATE_DRAWRESIZE_H_

// This function is also used by wxAuiManager when not using overlay, so it has
// to remain separate from wxDrawOverlayResizeHint().
inline wxBitmap wxPaneCreateStippleBitmap()
{
    // Notice that wxOverlay, under wxMSW, uses the wxBLACK colour i.e.(0,0,0)
    // as the key colour for transparency. and using it for the stipple bitmap
    // will make the sash feedback totaly invisible if the window's background
    // colour is (192,192,192) or so. (1,1,1) is used instead.
    unsigned char data[] = { 1,1,1,192,192,192, 192,192,192,1,1,1 };
    wxImage img(2,2,data,true);
    return wxBitmap(img);
}

// This is used by both wxSplitterWindow and wxAuiManager to draw the resize
// hint using the overlay associated with the given window.
inline void
wxDrawOverlayResizeHint(wxWindow* win, wxOverlay& overlay, const wxRect& rect)
{
    wxClientDC dc{win};
    wxDCOverlay overlaydc(overlay, &dc);
    overlaydc.Clear();

    wxBitmap stipple = wxPaneCreateStippleBitmap();
    wxBrush brush(stipple);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.DrawRectangle(rect);
}

#endif // _WX_GENERIC_PRIVATE_DRAWRESIZE_H_
