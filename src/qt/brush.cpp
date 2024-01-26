/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/brush.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/qt/private/utils.h"
#include "wx/bitmap.h"

#include <QtGui/QBrush>

wxIMPLEMENT_DYNAMIC_CLASS(wxBrush,wxGDIObject);

static Qt::BrushStyle ConvertBrushStyle(wxBrushStyle style)
{
    switch (style)
    {
        case wxBRUSHSTYLE_INVALID:
        case wxBRUSHSTYLE_SOLID:
            return Qt::SolidPattern;

        case wxBRUSHSTYLE_TRANSPARENT:
            return Qt::NoBrush;

        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
            return Qt::BDiagPattern;

        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
            return Qt::DiagCrossPattern;

        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
            return Qt::FDiagPattern;

        case wxBRUSHSTYLE_CROSS_HATCH:
            return Qt::CrossPattern;

        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            return Qt::HorPattern;

        case wxBRUSHSTYLE_VERTICAL_HATCH:
            return Qt::VerPattern;

        case wxBRUSHSTYLE_STIPPLE:
        case wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE:
        case wxBRUSHSTYLE_STIPPLE_MASK:
            return Qt::TexturePattern;
            break;
    }
    return Qt::SolidPattern;
}

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrushRefData: public wxGDIRefData
{
public:
    wxBrushRefData() :
        m_style(wxBRUSHSTYLE_INVALID)
    {
    }

    wxBrushRefData( const wxBrushRefData& data )
        : wxGDIRefData(),
          m_qtBrush(data.m_qtBrush)
    {
        m_style = data.m_style;
    }

    bool operator == (const wxBrushRefData& data) const
    {
        return m_qtBrush == data.m_qtBrush;
    }

    QBrush m_qtBrush;

    // To keep if mask is stippled
    wxBrushStyle m_style;
};

//-----------------------------------------------------------------------------

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)->m_qtBrush
#define M_STYLEDATA ((wxBrushRefData *)m_refData)->m_style

wxBrush::wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, wxBrushStyle style )
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA.setColor(col.GetQColor());
    M_BRUSHDATA.setStyle(ConvertBrushStyle(style));
    M_STYLEDATA = style;
}

wxBrush::wxBrush(const wxColour& col, int style)
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA.setColor(col.GetQColor());
    M_BRUSHDATA.setStyle(ConvertBrushStyle((wxBrushStyle)style));
    M_STYLEDATA = (wxBrushStyle)style;
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA.setTexture(*stipple.GetHandle());
    if (stipple.GetMask() != nullptr)
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE;
    else
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE;
}


void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_BRUSHDATA.setColor(col.GetQColor());
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_BRUSHDATA.setColor(QColor(r, g, b));
}

void wxBrush::SetStyle(wxBrushStyle style)
{
    AllocExclusive();
    M_BRUSHDATA.setStyle(ConvertBrushStyle((wxBrushStyle)style));
    M_STYLEDATA = style;
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
    M_BRUSHDATA.setTexture(*stipple.GetHandle());

    if (stipple.GetMask() != nullptr)
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE;
    else
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE;
}


bool wxBrush::operator==(const wxBrush& brush) const
{
    if (m_refData == brush.m_refData) return true;

    if (!m_refData || !brush.m_refData) return false;

    return ( *(wxBrushRefData*)m_refData == *(wxBrushRefData*)brush.m_refData );
}


wxColour wxBrush::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid brush") );

    return wxColour(M_BRUSHDATA.color());
}

wxBrushStyle wxBrush::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxBRUSHSTYLE_INVALID, "invalid brush" );

    return M_STYLEDATA;
}

wxBitmap *wxBrush::GetStipple() const
{
    wxCHECK_MSG( IsOk(), nullptr, "invalid brush" );

    QPixmap p = M_BRUSHDATA.texture();

    if (p.isNull())
        return new wxBitmap();
    else
        return new wxBitmap(p);
}

QBrush wxBrush::GetHandle() const
{
    return IsOk() ? M_BRUSHDATA : QBrush();
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
