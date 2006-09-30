/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/brush.cpp
// Purpose:     wxBrush class implementation
// Author:      Vaclav Slavik
// Created:     2006-08-04
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/brush.h"

#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif


//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrushRefData: public wxObjectRefData
{
public:
    wxBrushRefData(const wxColour& clr = wxNullColour, int style = wxSOLID)
    {
        m_colour = clr;
        SetStyle(style);
    }

    wxBrushRefData(const wxBrushRefData& data)
    {
        m_colour = data.m_colour;
        m_style = data.m_style;
    }

    void SetStyle(int style)
    {
        if ( style != wxSOLID && style != wxTRANSPARENT )
        {
            wxFAIL_MSG( _T("only wxSOLID and wxTRANSPARENT styles are supported") );
            style = wxSOLID;
        }

        m_style = style;
    }

    wxColour       m_colour;
    int            m_style;
};

//-----------------------------------------------------------------------------

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)

wxBrush::wxBrush(const wxColour &colour, int style)
{
    m_refData = new wxBrushRefData(colour, style);
}

wxBrush::wxBrush(const wxBitmap &stippleBitmap)
{
    wxFAIL_MSG( "brushes with stipple bitmaps not implemented" );

    m_refData = new wxBrushRefData(*wxBLACK);
}

bool wxBrush::operator==(const wxBrush& brush) const
{
#warning "this is incorrect (MGL too)"
    return m_refData == brush.m_refData;
}

bool wxBrush::Ok() const
{
    return ((m_refData) && M_BRUSHDATA->m_colour.Ok());
}

int wxBrush::GetStyle() const
{
    if (m_refData == NULL)
    {
        wxFAIL_MSG( wxT("invalid brush") );
        return 0;
    }

    return M_BRUSHDATA->m_style;
}

wxColour& wxBrush::GetColour() const
{
    if (m_refData == NULL)
    {
        wxFAIL_MSG( wxT("invalid brush") );
        return wxNullColour;
    }

    return M_BRUSHDATA->m_colour;
}

wxBitmap *wxBrush::GetStipple() const
{
    wxFAIL_MSG( "brushes with stipple bitmaps not implemented" );
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

void wxBrush::SetStyle(int style)
{
    AllocExclusive();
    M_BRUSHDATA->SetStyle(style);
}

void wxBrush::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( "brushes with stipple bitmaps not implemented" );
}

wxObjectRefData *wxBrush::CreateRefData() const
{
    return new wxBrushRefData;
}

wxObjectRefData *wxBrush::CloneRefData(const wxObjectRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
