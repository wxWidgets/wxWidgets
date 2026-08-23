///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/colour.h
// Purpose:     Private colour-related helper functions.
// Author:      Vadim Zeitlin
// Created:     2026-07-29 (extracted src/aui/tabart.cpp)
// Copyright:   (c) 2026 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_COLOUR_H_
#define _WX_PRIVATE_COLOUR_H_

#include "wx/colour.h"

#include <cmath>

// Helpers used only in this file itself.
namespace wxPrivate
{

inline float wxGetSRGB(float r)
{
    return r <= 0.04045f ? r / 12.92f : std::pow((r + 0.055f) / 1.055f, 2.4f);
}

inline float wxGetRelativeLuminance(const wxColour& c)
{
    // See https://www.w3.org/TR/WCAG21/#dfn-relative-luminance
    return
        0.2126f * wxGetSRGB(c.Red()   / 255.0f) +
        0.7152f * wxGetSRGB(c.Green() / 255.0f) +
        0.0722f * wxGetSRGB(c.Blue()  / 255.0f);
}

inline float wxComputeContrast(const wxColour& c1, const wxColour& c2)
{
    // See https://www.w3.org/TR/WCAG21/#dfn-contrast-ratio
    float L1 = wxGetRelativeLuminance(c1);
    float L2 = wxGetRelativeLuminance(c2);
    return L1 > L2 ? (L1 + 0.05f) / (L2 + 0.05f) : (L2 + 0.05f) / (L1 + 0.05f);
}

} // anonymous namespace

// Return the colour if it has sufficient contrast ratio (4.5 recommended)
// with the background or return either white or black if it doesn't.
// (see https://www.w3.org/TR/WCAG21/#contrast-minimum)
inline wxColour
wxGetContrastingFgColour(const wxColour& fg, const wxColour& bg)
{
    using namespace wxPrivate;

    // No need to change the colour if it has sufficient contrast.
    if ( wxComputeContrast(fg, bg) >= 4.5f )
        return fg;

    // Otherwise pick the colour that provides better contrast.
    return wxComputeContrast(*wxWHITE, bg) > wxComputeContrast(*wxBLACK, bg)
            ? *wxWHITE
            : *wxBLACK;
}

// Version of the above modifying the foregorund colour in place.
inline void
wxEnsureSufficientContrast(wxColour* fg, const wxColour& bg)
{
    *fg = wxGetContrastingFgColour(*fg, bg);
}

#endif // _WX_PRIVATE_COLOUR_H_
