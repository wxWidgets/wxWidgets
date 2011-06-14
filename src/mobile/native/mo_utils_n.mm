/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_utils_n.mm
// Purpose:     iPhone 'emulator' utilities.
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/mobile/native/utils.h"

bool wxMoRenderer::DrawButtonBackground(wxWindow* WXUNUSED(window), wxDC& dc,
                                        const wxColour& backgroundColour,
                                        const wxColour& colour,
                                        const wxColour& borderColour1, 
                                        const wxRect& rect1,
                                        int state, int buttonStyle, int inc)
{
    // FIXME stub

    return true;
}

unsigned char wxMoRenderer::BlendColour(unsigned char fg, unsigned char bg, double alpha)
{
    double result = bg + (alpha * (fg - bg));
    if (result < 0.0)
        result = 0.0;
    if (result > 255)
        result = 255;
    return (unsigned char)result;
}

bool wxMoRenderer::IncColourValues(wxColour& colour, int inc)
{
    // FIXME stub

    return true;
}

wxBitmap wxMoRenderer::MakeDisabledBitmap(wxBitmap& bmp)
{
    // FIXME stub

	wxBitmap empty;
	return empty;
}

bool wxMoButtonPressDetector::ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event, int id)
{
    // FIXME stub

    return false;
}
