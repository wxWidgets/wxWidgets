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
    unsigned char data[] = { 0,0,0,192,192,192, 192,192,192,0,0,0 };
    wxImage img(2,2,data,true);
    return wxBitmap(img);
}

// This is used by both wxSplitterWindow and wxAuiManager to draw the resize
// hint using the overlay associated with the given window.
inline void
wxDrawOverlayResizeHint(wxWindow* win, wxOverlay& overlay, const wxRect& rect)
{
    wxOverlayDC dc(overlay, win);
    dc.Clear();

    wxBitmap stipple = wxPaneCreateStippleBitmap();
    wxBrush brush(stipple);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.DrawRectangle(rect);
}

#endif // _WX_GENERIC_PRIVATE_DRAWRESIZE_H_
