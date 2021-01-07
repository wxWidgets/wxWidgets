/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/brush.cpp
// Purpose:     wxBrush class implementation
// Author:      Vaclav Slavik
// Created:     2006-08-04
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/brush.h"

#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif


//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrushRefData : public wxGDIRefData
{
public:
    wxBrushRefData(const wxColour& clr = wxNullColour, wxBrushStyle style = wxBRUSHSTYLE_SOLID)
    {
        m_colour = clr;
        SetStyle(style);
    }

    wxBrushRefData(const wxBrushRefData& data)
    {
        m_colour = data.m_colour;
        m_style = data.m_style;
    }

    virtual bool IsOk() const { return m_colour.IsOk(); }

    void SetStyle(wxBrushStyle style)
    {
        if ( style != wxBRUSHSTYLE_SOLID && style != wxBRUSHSTYLE_TRANSPARENT )
        {
            wxFAIL_MSG( wxT("only wxBRUSHSTYLE_SOLID and wxBRUSHSTYLE_TRANSPARENT styles are supported") );
            style = wxBRUSHSTYLE_SOLID;
        }

        m_style = style;
    }

    wxColour       m_colour;
    wxBrushStyle   m_style;
};

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
    wxFAIL_MSG( wxT("brushes with stipple bitmaps not implemented") );

    m_refData = new wxBrushRefData(*wxBLACK);
}

bool wxBrush::operator==(const wxBrush& brush) const
{
#warning "this is incorrect"
    return m_refData == brush.m_refData;
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
    wxFAIL_MSG( wxT("brushes with stipple bitmaps not implemented") );
    return &wxNullBitmap;
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
    M_BRUSHDATA->SetStyle(style);
}

void wxBrush::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( wxT("brushes with stipple bitmaps not implemented") );
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
