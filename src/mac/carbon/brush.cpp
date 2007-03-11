/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/brush.cpp
// Purpose:     wxBrush
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/brush.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif

#include "wx/mac/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;

public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& data);
    virtual ~wxBrushRefData();

    bool operator == ( const wxBrushRefData& brush ) const
    {
        return m_style == brush.m_style &&
                m_stipple.IsSameAs(brush.m_stipple) &&
                m_colour == brush.m_colour &&
                m_macBrushKind == brush.m_macBrushKind &&
                m_macThemeBrush == brush.m_macThemeBrush &&
                m_macThemeBackground == brush.m_macThemeBackground &&
                EqualRect(&m_macThemeBackgroundExtent, &brush.m_macThemeBackgroundExtent);
    }


protected:
    wxMacBrushKind m_macBrushKind ;
    int           m_style;
    wxBitmap      m_stipple ;
    wxColour      m_colour;

    ThemeBrush    m_macThemeBrush ;

    ThemeBackgroundKind m_macThemeBackground ;
    Rect         m_macThemeBackgroundExtent ;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)


wxBrushRefData::wxBrushRefData()
    : m_style(wxSOLID)
{
    m_macBrushKind = kwxMacBrushColour ;
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
    : wxGDIRefData()
    , m_style(data.m_style)
{
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
  m_macBrushKind = data.m_macBrushKind ;
  m_macThemeBrush = data.m_macThemeBrush ;
  m_macThemeBackground = data.m_macThemeBackground ;
  m_macThemeBackgroundExtent = data.m_macThemeBackgroundExtent ;
}

wxBrushRefData::~wxBrushRefData()
{
}

wxBrush::wxBrush()
{
}

wxBrush::~wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, int Style)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_colour = col;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_colour = *wxBLACK;
    M_BRUSHDATA->m_stipple = stipple;

    if (M_BRUSHDATA->m_stipple.GetMask())
        M_BRUSHDATA->m_style = wxSTIPPLE_MASK_OPAQUE;
    else
        M_BRUSHDATA->m_style = wxSTIPPLE;

    RealizeResource();
}

wxBrush::wxBrush( ThemeBrush macThemeBrush )
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushTheme;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RealizeResource();
}

wxObjectRefData* wxBrush::CreateRefData() const
{
    return new wxBrushRefData;
}

wxObjectRefData* wxBrush::CloneRefData(const wxObjectRefData* data) const
{
    return new wxBrushRefData(*wx_static_cast(const wxBrushRefData*, data));
}

void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_colour = col;

    RealizeResource();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_colour.Set(r, g, b);

    RealizeResource();
}

void wxBrush::SetStyle(int Style)
{
    AllocExclusive();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

void wxBrush::SetStipple(const wxBitmap& Stipple)
{
    AllocExclusive();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_stipple = Stipple;

    RealizeResource();
}

void wxBrush::MacSetTheme(ThemeBrush macThemeBrush)
{
    AllocExclusive();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushTheme;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RGBColor color ;
    GetThemeBrushAsColor( macThemeBrush , 32, true, &color );
    M_BRUSHDATA->m_colour = color;

    RealizeResource();
}

void wxBrush::MacSetThemeBackground(unsigned long macThemeBackground, const WXRECTPTR extent)
{
    AllocExclusive();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushThemeBackground;
    M_BRUSHDATA->m_macThemeBackground = macThemeBackground;
    M_BRUSHDATA->m_macThemeBackgroundExtent = *(Rect*)extent;

    RealizeResource();
}

bool wxBrush::RealizeResource()
{
    return true;
}

unsigned long wxBrush::MacGetThemeBackground(WXRECTPTR extent) const
{
    if ( M_BRUSHDATA && M_BRUSHDATA->m_macBrushKind == kwxMacBrushThemeBackground )
    {
        if ( extent )
            *(Rect*)extent = M_BRUSHDATA->m_macThemeBackgroundExtent;

        return M_BRUSHDATA->m_macThemeBackground;
    }
    else
    {
        return 0;
    }
}

short wxBrush::MacGetTheme() const
{
    return (M_BRUSHDATA ? ((M_BRUSHDATA->m_macBrushKind == kwxMacBrushTheme) ? M_BRUSHDATA->m_macThemeBrush : kThemeBrushBlack) : kThemeBrushBlack);
}

wxColour& wxBrush::GetColour() const
{
    return (M_BRUSHDATA ? M_BRUSHDATA->m_colour : wxNullColour);
}

int wxBrush::GetStyle() const
{
    return (M_BRUSHDATA ? M_BRUSHDATA->m_style : 0);
}

wxBitmap *wxBrush::GetStipple() const
{
    return (M_BRUSHDATA ? & M_BRUSHDATA->m_stipple : 0);
}

wxMacBrushKind wxBrush::MacGetBrushKind() const
{
    return (M_BRUSHDATA ? M_BRUSHDATA->m_macBrushKind : kwxMacBrushColour);
}

bool wxBrush::operator == ( const wxBrush& brush ) const
{
    if (m_refData == brush.m_refData) return true;

    if (!m_refData || !brush.m_refData) return false;

    return ( *(wxBrushRefData*)m_refData == *(wxBrushRefData*)brush.m_refData );
}
