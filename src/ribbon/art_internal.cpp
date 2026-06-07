///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_internal.cpp
// Purpose:     Helper functions & classes used by ribbon art providers
// Author:      Peter Cawley
// Created:     2009-08-04
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/ribbon/art_internal.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

wxRibbonArtProvider::wxRibbonArtProvider() {}
wxRibbonArtProvider::~wxRibbonArtProvider() {}

wxColour wxRibbonInterpolateColour(const wxColour& startColour,
                                const wxColour& endColour,
                                int position,
                                int startPosition,
                                int endPosition)
{
    if ( position <= startPosition )
    {
        return startColour;
    }
    if ( position >= endPosition )
    {
        return endColour;
    }
    position -= startPosition;
    endPosition -= startPosition;
    int r = endColour.Red() - startColour.Red();
    int g = endColour.Green() - startColour.Green();
    int b = endColour.Blue() - startColour.Blue();
    // Scale by 100 to avoid integer truncation during the interpolation.
    r = startColour.Red()   + (((r * position * 100) / endPosition) / 100);
    g = startColour.Green() + (((g * position * 100) / endPosition) / 100);
    b = startColour.Blue()  + (((b * position * 100) / endPosition) / 100);
    return wxColour(r, g, b);
}

bool wxRibbonCanLabelBreakAtPosition(const wxString& label, size_t pos)
{
    return label[pos] == ' ';
}

void wxRibbonDrawParallelGradientLines(wxDC& dc,
                                    int nLines,
                                    const wxPoint* lineOrigins,
                                    int stepX,
                                    int stepY,
                                    int numSteps,
                                    int offsetX,
                                    int offsetY,
                                    const wxColour& startColour,
                                    const wxColour& endColour)
{
    // Pre-compute per-channel deltas across the full gradient range.
    int rd, gd, bd;
    rd = endColour.Red() - startColour.Red();
    gd = endColour.Green() - startColour.Green();
    bd = endColour.Blue() - startColour.Blue();

    for ( int step = 0; step < numSteps; ++step )
    {
        int r, g, b;

        // Scale by 100 to avoid integer truncation during interpolation.
        r = startColour.Red()   + (((step * rd * 100) / numSteps) / 100);
        g = startColour.Green() + (((step * gd * 100) / numSteps) / 100);
        b = startColour.Blue()  + (((step * bd * 100) / numSteps) / 100);

        wxPen p(wxColour((unsigned char)r,
                         (unsigned char)g,
                         (unsigned char)b));
        dc.SetPen(p);

        for ( int n = 0; n < nLines; ++n )
        {
            dc.DrawLine(offsetX + lineOrigins[n].x,
                        offsetY + lineOrigins[n].y,
                        offsetX + lineOrigins[n].x + stepX,
                        offsetY + lineOrigins[n].y + stepY);
        }

        offsetX += stepX;
        offsetY += stepY;
    }
}

wxRibbonHSLColour wxRibbonShiftLuminance(wxRibbonHSLColour colour,
                                                float amount)
{
    // amount < 1 darkens, amount > 1 lightens (inverted in dark mode).
    if ( amount <= 1.0f )
        return colour.Darker(colour.m_luminance * (1.0f - amount));
    else if ( wxSystemSettings::GetAppearance().IsDark() )
        return colour.Darker(colour.m_luminance * (amount - 1.0f));
    else
        return colour.Lighter((1.0f - colour.m_luminance) * (amount - 1.0f));
}

wxBitmap wxRibbonLoadPixmap(const char* const* bits, wxColour fore)
{
    wxImage xpm = wxBitmap(bits).ConvertToImage();
    xpm.Replace(255, 0, 255, fore.Red(), fore.Green(), fore.Blue());
    return wxBitmap(xpm);
}

wxRibbonHSLColour::wxRibbonHSLColour(const wxColour& col)
{
    float red = col.Red() / 255.0f;
    float green = col.Green() / 255.0f;
    float blue = col.Blue() / 255.0f;
    float minComp = wxMin(red, wxMin(green, blue));
    float maxComp = wxMax(red, wxMax(green, blue));
    m_luminance = 0.5f * (maxComp + minComp);
    if ( minComp == maxComp )
    {
        // colour is a shade of grey
        m_hue = 0;
        m_saturation = 0;
    }
    else
    {
        // HSL saturation formula differs above and below 0.5 luminance.
        if ( m_luminance <= 0.5f )
            m_saturation = (maxComp - minComp) / (maxComp + minComp);
        else
            m_saturation = (maxComp - minComp) / (2.0f - (maxComp + minComp));

        if ( maxComp == red )
        {
            m_hue = 60.0f * (green - blue) / (maxComp - minComp);
            if ( m_hue < 0 )
                m_hue += 360.0f;
        }
        else if ( maxComp == green )
        {
            m_hue = 60.0f * (blue - red) / (maxComp - minComp);
            m_hue += 120.0f;
        }
        else // maxComp == blue
        {
            m_hue = 60.0f * (red - green) / (maxComp - minComp);
            m_hue += 240.0f;
        }
    }
}

wxColour wxRibbonHSLColour::ToRGB() const
{
    // Normalise hue to [0, 360) and clamp saturation/luminance to [0, 1].
    float hue = (m_hue - float(floor(m_hue / 360.0f)) * 360.0f);
    float saturation = m_saturation;
    float luminance = m_luminance;
    if ( saturation > 1 ) saturation = 1;
    if ( saturation < 0 ) saturation = 0;
    if ( luminance > 1 ) luminance = 1;
    if ( luminance < 0 ) luminance = 0;

    float red, blue, green;
    if ( saturation == 0 )
    {
        // colour is a shade of grey
        red = blue = green = luminance;
    }
    else
    {
        // Standard HSL-to-RGB conversion using two temporary values.
        float tmp2 = (luminance < 0.5f)
            ? luminance * (1.0f + saturation)
            : (luminance + saturation) - (luminance * saturation);
        float tmp1 = 2.0f * luminance - tmp2;

        // Each channel uses a hue offset: red +120, green +0, blue +240.
        float tmp3R = hue + 120.0f;
        if ( tmp3R > 360 )
            tmp3R -= 360.0f;
        if ( tmp3R < 60 )
            red = tmp1 + (tmp2 - tmp1) * tmp3R / 60.0f;
        else if ( tmp3R < 180 )
            red = tmp2;
        else if ( tmp3R < 240 )
            red = tmp1 + (tmp2 - tmp1) * (240.0f - tmp3R) / 60.0f;
        else
            red = tmp1;

        float tmp3G = hue;
        if ( tmp3G > 360 )
            tmp3G -= 360.0f;
        if ( tmp3G < 60 )
            green = tmp1 + (tmp2 - tmp1) * tmp3G / 60.0f;
        else if ( tmp3G < 180 )
            green = tmp2;
        else if ( tmp3G < 240 )
            green = tmp1 + (tmp2 - tmp1) * (240.0f - tmp3G) / 60.0f;
        else
            green = tmp1;

        float tmp3B = hue + 240.0f;
        if ( tmp3B > 360 )
            tmp3B -= 360.0f;
        if ( tmp3B < 60 )
            blue = tmp1 + (tmp2 - tmp1) * tmp3B / 60.0f;
        else if ( tmp3B < 180 )
            blue = tmp2;
        else if ( tmp3B < 240 )
            blue = tmp1 + (tmp2 - tmp1) * (240.0f - tmp3B) / 60.0f;
        else
            blue = tmp1;
    }
    return wxColour(
        static_cast<unsigned char>(red * 255.0f),
        static_cast<unsigned char>(green * 255.0f),
        static_cast<unsigned char>(blue * 255.0f));
}

wxRibbonHSLColour wxRibbonHSLColour::Darker(float delta) const
{
    return Lighter(-delta);
}

wxRibbonHSLColour wxRibbonHSLColour::Lighter(float delta) const
{
    return wxRibbonHSLColour(m_hue, m_saturation, m_luminance + delta);
}

wxRibbonHSLColour wxRibbonHSLColour::Saturated(float delta) const
{
    return wxRibbonHSLColour(m_hue, m_saturation + delta, m_luminance);
}

wxRibbonHSLColour wxRibbonHSLColour::Desaturated(float delta) const
{
    return Saturated(-delta);
}

wxRibbonHSLColour wxRibbonHSLColour::ShiftHue(float delta) const
{
    return wxRibbonHSLColour(m_hue + delta, m_saturation, m_luminance);
}

#endif // wxUSE_RIBBON
