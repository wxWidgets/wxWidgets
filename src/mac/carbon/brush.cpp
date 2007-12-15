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
public:
    wxBrushRefData(const wxColour& colour = wxNullColour, int style = wxSOLID);
    wxBrushRefData(const wxBitmap& stipple);
    wxBrushRefData(const wxBrushRefData& data);
    virtual ~wxBrushRefData();

    bool operator==(const wxBrushRefData& data) const;

    const wxColour& GetColour() const { return m_colour; }
    int GetStyle() const { return m_style; }
    wxBitmap *GetStipple() { return &m_stipple; }
        
    void SetColour(const wxColour& colour) { m_colour = colour; }
    void SetStyle(int style) { m_style = style; }
    void SetStipple(const wxBitmap& stipple) { DoSetStipple(stipple); }
    
protected:
    void DoSetStipple(const wxBitmap& stipple);

    wxBitmap      m_stipple ;
    wxColour      m_colour;
    int           m_style;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

wxBrushRefData::wxBrushRefData(const wxColour& colour, int style)
    : m_colour(colour), m_style( style )
{
}

wxBrushRefData::wxBrushRefData(const wxBitmap& stipple)
{
    DoSetStipple( stipple );
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
    : wxGDIRefData() ,
        m_stipple(data.m_stipple),
        m_colour(data.m_colour),
        m_style(data.m_style)
{
}

wxBrushRefData::~wxBrushRefData()
{
}

bool wxBrushRefData::operator==(const wxBrushRefData& data) const
{
    return m_style == data.m_style &&
        m_colour == data.m_colour &&
        m_stipple.IsSameAs(data.m_stipple);
}

void wxBrushRefData::DoSetStipple(const wxBitmap& stipple)
{
    m_stipple = stipple;
    m_style = stipple.GetMask() ? wxSTIPPLE_MASK_OPAQUE : wxSTIPPLE;
}
//
//
//

wxBrush::wxBrush()
{
}

wxBrush::~wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, int style)
{
    m_refData = new wxBrushRefData( col, style );
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData( stipple );
}

// ----------------------------------------------------------------------------
// wxBrush house keeping stuff
// ----------------------------------------------------------------------------

bool wxBrush::operator==(const wxBrush& brush) const
{
    const wxBrushRefData *brushData = (wxBrushRefData *)brush.m_refData;
    
    // an invalid brush is considered to be only equal to another invalid brush
    return m_refData ? (brushData && *M_BRUSHDATA == *brushData) : !brushData;
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(const wxBrushRefData *)data);
}

// ----------------------------------------------------------------------------
// wxBrush accessors
// ----------------------------------------------------------------------------

const wxColour& wxBrush::GetColour() const
{
    wxCHECK_MSG( Ok(), wxNullColour, _T("invalid brush") );
    
    return M_BRUSHDATA->GetColour();
}

int wxBrush::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid brush") );
    
    return M_BRUSHDATA->GetStyle();
}

wxBitmap *wxBrush::GetStipple() const
{
    wxCHECK_MSG( Ok(), NULL, _T("invalid brush") );
    
    return M_BRUSHDATA->GetStipple();
}

// ----------------------------------------------------------------------------
// wxBrush setters
// ----------------------------------------------------------------------------

void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
    
    M_BRUSHDATA->SetColour(col);
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    
    M_BRUSHDATA->SetColour(wxColour(r, g, b));
}

void wxBrush::SetStyle(int style)
{
    AllocExclusive();
    
    M_BRUSHDATA->SetStyle(style);
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
    
    M_BRUSHDATA->SetStipple(stipple);
}
