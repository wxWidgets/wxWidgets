/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/pen.cpp
// Purpose:     wxPen class implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/colour.h"
#endif

#include <vector>

//-----------------------------------------------------------------------------
// wxPenRefData
//-----------------------------------------------------------------------------

class wxPenRefData : public wxGDIRefData
{
public:
    wxPenRefData(const wxColour& clr = wxNullColour,
                 wxPenStyle style = wxPENSTYLE_SOLID,
                 int width = 1)
        : m_colour(clr),
          m_style(style),
          m_width(width)
    {
    }

    explicit wxPenRefData(const wxPenInfo& info)
        : m_colour(info.GetColour()),
          m_style(info.GetStyle()),
          m_width(info.GetWidth()),
          m_cap(info.GetCap()),
          m_join(info.GetJoin()),
          m_stipple(info.GetStipple())
    {
        wxDash* dashes;
        const int n = info.GetDashes(&dashes);
        if ( n > 0 )
            m_dashes.assign(dashes, dashes + n);
    }

    wxPenRefData(const wxPenRefData& data)
        : wxGDIRefData(),
          m_colour(data.m_colour),
          m_style(data.m_style),
          m_width(data.m_width),
          m_cap(data.m_cap),
          m_join(data.m_join),
          m_dashes(data.m_dashes),
          m_stipple(data.m_stipple)
    {
    }

    virtual bool IsOk() const override { return m_colour.IsOk(); }

    bool operator==(const wxPenRefData& data) const
    {
        return m_style == data.m_style &&
               m_width == data.m_width &&
               m_cap == data.m_cap &&
               m_join == data.m_join &&
               m_colour == data.m_colour &&
               m_dashes == data.m_dashes &&
               m_stipple.IsSameAs(data.m_stipple);
    }

    wxColour m_colour;
    wxPenStyle m_style;
    int m_width;
    wxPenCap m_cap = wxCAP_ROUND;
    wxPenJoin m_join = wxJOIN_ROUND;
    std::vector<wxDash> m_dashes;
    wxBitmap m_stipple;
};

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject);

wxPen::wxPen(const wxColour &colour, int width, wxPenStyle style)
{
    m_refData = new wxPenRefData(colour, style, width);
}

wxPen::wxPen(const wxColour& colour, int width, int style)
{
    m_refData = new wxPenRefData(colour, (wxPenStyle)style, width);
}

wxPen::wxPen(const wxPenInfo& info)
{
    m_refData = new wxPenRefData(info);
}

wxPen::wxPen(const wxBitmap& stipple, int width)
{
    m_refData = new wxPenRefData(*wxBLACK, wxPENSTYLE_STIPPLE, width);
    M_PENDATA->m_stipple = stipple;
}

bool wxPen::operator==(const wxPen& pen) const
{
    if ( m_refData == pen.m_refData )
        return true;

    if ( !m_refData || !pen.m_refData )
        return false;

    return *M_PENDATA == *(wxPenRefData *)pen.m_refData;
}

void wxPen::SetColour(const wxColour &colour)
{
    AllocExclusive();
    M_PENDATA->m_colour = colour;
}

void wxPen::SetDashes(int number_of_dashes, const wxDash *dash)
{
    AllocExclusive();
    if ( number_of_dashes > 0 && dash )
        M_PENDATA->m_dashes.assign(dash, dash + number_of_dashes);
    else
        M_PENDATA->m_dashes.clear();
}

void wxPen::SetColour(unsigned char red, unsigned char green, unsigned char blue)
{
    AllocExclusive();
    M_PENDATA->m_colour.Set(red, green, blue);
}

void wxPen::SetCap(wxPenCap capStyle)
{
    AllocExclusive();
    M_PENDATA->m_cap = capStyle;
}

void wxPen::SetJoin(wxPenJoin joinStyle)
{
    AllocExclusive();
    M_PENDATA->m_join = joinStyle;
}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();
    M_PENDATA->m_style = style;
}

void wxPen::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
    M_PENDATA->m_stipple = stipple;
    M_PENDATA->m_style = wxPENSTYLE_STIPPLE;
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();
    M_PENDATA->m_width = width;
}

int wxPen::GetDashes(wxDash **ptr) const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid pen") );

    *ptr = M_PENDATA->m_dashes.empty() ? nullptr : &M_PENDATA->m_dashes[0];
    return static_cast<int>(M_PENDATA->m_dashes.size());
}

int wxPen::GetDashCount() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid pen") );

    return static_cast<int>(M_PENDATA->m_dashes.size());
}

wxDash* wxPen::GetDash() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid pen") );

    return M_PENDATA->m_dashes.empty() ? nullptr : &M_PENDATA->m_dashes[0];
}

wxPenCap wxPen::GetCap() const
{
    wxCHECK_MSG( IsOk(), wxCAP_INVALID, wxT("invalid pen") );

    return M_PENDATA->m_cap;
}

wxPenJoin wxPen::GetJoin() const
{
    wxCHECK_MSG( IsOk(), wxJOIN_INVALID, wxT("invalid pen") );

    return M_PENDATA->m_join;
}

wxPenStyle wxPen::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxPENSTYLE_INVALID, wxT("invalid pen") );

    return M_PENDATA->m_style;
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid pen") );

    return M_PENDATA->m_width;
}

wxColour wxPen::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid pen") );

    return M_PENDATA->m_colour;
}

wxBitmap *wxPen::GetStipple() const
{
    wxCHECK_MSG( IsOk(), nullptr, wxT("invalid pen") );

    return &M_PENDATA->m_stipple;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
