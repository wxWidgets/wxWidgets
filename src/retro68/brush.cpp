/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/brush.cpp
// Purpose:     wxBrush class implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/brush.h"

#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif

//-----------------------------------------------------------------------------
// wxBrushRefData
//-----------------------------------------------------------------------------

class wxBrushRefData : public wxGDIRefData
{
public:
    wxBrushRefData(const wxColour& clr = wxNullColour,
                   wxBrushStyle style = wxBRUSHSTYLE_SOLID)
        : m_colour(clr),
          m_style(style)
    {
    }

    wxBrushRefData(const wxBrushRefData& data)
        : wxGDIRefData(),
          m_colour(data.m_colour),
          m_style(data.m_style),
          m_stipple(data.m_stipple)
    {
    }

    virtual bool IsOk() const override { return m_colour.IsOk(); }

    bool operator==(const wxBrushRefData& data) const
    {
        return m_style == data.m_style &&
               m_colour == data.m_colour &&
               m_stipple.IsSameAs(data.m_stipple);
    }

    wxColour       m_colour;
    wxBrushStyle   m_style;
    wxBitmap       m_stipple;
};

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject);

wxBrush::wxBrush(const wxColour &colour, wxBrushStyle style)
{
    m_refData = new wxBrushRefData(colour, style);
}

wxBrush::wxBrush(const wxColour& col, int style)
{
    m_refData = new wxBrushRefData(col, (wxBrushStyle)style);
}

wxBrush::wxBrush(const wxBitmap &stippleBitmap)
{
    m_refData = new wxBrushRefData(*wxBLACK, wxBRUSHSTYLE_STIPPLE);
    M_BRUSHDATA->m_stipple = stippleBitmap;
}

bool wxBrush::operator==(const wxBrush& brush) const
{
    if ( m_refData == brush.m_refData )
        return true;

    if ( !m_refData || !brush.m_refData )
        return false;

    return *M_BRUSHDATA == *(wxBrushRefData *)brush.m_refData;
}

wxBrushStyle wxBrush::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxBRUSHSTYLE_INVALID, wxT("invalid brush") );

    return M_BRUSHDATA->m_style;
}

wxColour wxBrush::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid brush") );

    return M_BRUSHDATA->m_colour;
}

wxBitmap *wxBrush::GetStipple() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid brush") );

    return &M_BRUSHDATA->m_stipple;
}

void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_BRUSHDATA->m_colour = col;
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_BRUSHDATA->m_colour.Set(r, g, b);
}

void wxBrush::SetStyle(wxBrushStyle style)
{
    AllocExclusive();
    M_BRUSHDATA->m_style = style;
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
    M_BRUSHDATA->m_stipple = stipple;
    M_BRUSHDATA->m_style = wxBRUSHSTYLE_STIPPLE;
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
