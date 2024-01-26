/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/pen.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"
#include "wx/colour.h"
#include "wx/qt/private/utils.h"
#include <QtGui/QPen>

wxIMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject);

static Qt::PenStyle ConvertPenStyle(wxPenStyle style)
{
    switch(style)
    {
        case wxPENSTYLE_SOLID:
            return Qt::SolidLine;

        case wxPENSTYLE_TRANSPARENT:
            return Qt::NoPen;

        case wxPENSTYLE_DOT:
            return Qt::DotLine;

        case wxPENSTYLE_LONG_DASH:
        case wxPENSTYLE_SHORT_DASH:
            return Qt::DashLine;

        case wxPENSTYLE_DOT_DASH:
            return Qt::DotLine;

        case wxPENSTYLE_USER_DASH:
            return Qt::CustomDashLine;

        case wxPENSTYLE_STIPPLE:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_STIPPLE" );
            break;

        case wxPENSTYLE_BDIAGONAL_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_BDIAGONAL_HATCH" );
            break;

        case wxPENSTYLE_CROSSDIAG_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_CROSSDIAG_HATCH" );
            break;

        case wxPENSTYLE_FDIAGONAL_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_FDIAGONAL_HATCH" );
            break;

        case wxPENSTYLE_CROSS_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_CROSS_HATCH" );
            break;

        case wxPENSTYLE_HORIZONTAL_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_HORIZONTAL_HATCH" );
            break;

        case wxPENSTYLE_VERTICAL_HATCH:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_VERTICAL_HATCH" );
            break;

        case wxPENSTYLE_STIPPLE_MASK:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_STIPPLE_MASK" );
            break;

        case wxPENSTYLE_STIPPLE_MASK_OPAQUE:
            wxMISSING_IMPLEMENTATION( "wxPENSTYLE_STIPPLE_MASK_OPAQUE" );
            break;

        case wxPENSTYLE_INVALID:
            wxFAIL_MSG( "Invalid pen style value" );
            break;
    }
    return Qt::SolidLine;
}

static wxPenStyle ConvertPenStyle(Qt::PenStyle style)
{
    switch (style)
    {
        case Qt::SolidLine:
            return wxPENSTYLE_SOLID;

        case Qt::NoPen:
            return wxPENSTYLE_TRANSPARENT;

        case Qt::DotLine:
            return wxPENSTYLE_DOT;

        case Qt::DashLine:
            return wxPENSTYLE_SHORT_DASH;

        case Qt::DashDotLine:
            return wxPENSTYLE_DOT_DASH;

        case Qt::DashDotDotLine:
            wxMISSING_IMPLEMENTATION( "Qt::DashDotDotLine" );
            return wxPENSTYLE_DOT_DASH;

        case Qt::CustomDashLine:
            return wxPENSTYLE_USER_DASH;

        case Qt::MPenStyle:
            wxMISSING_IMPLEMENTATION( "Qt::MPenStyle" );
            break;
    }
    return wxPENSTYLE_SOLID;
}

static Qt::PenCapStyle ConvertPenCapStyle(wxPenCap style)
{
    switch (style)
    {
        case wxCAP_BUTT:
            return Qt::FlatCap;

        case wxCAP_PROJECTING:
            return Qt::SquareCap;

        case wxCAP_ROUND:
            return Qt::RoundCap;

        case wxCAP_INVALID:
            wxFAIL_MSG( "Invalid pen cap value" );
            break;
    }
    return Qt::SquareCap;
}

static wxPenCap ConvertPenCapStyle(Qt::PenCapStyle style)
{
    switch (style)
    {
        case Qt::FlatCap:
            return wxCAP_BUTT;

        case Qt::SquareCap:
            return wxCAP_PROJECTING;

        case Qt::RoundCap:
            return wxCAP_ROUND;

        case Qt::MPenCapStyle:
            wxMISSING_IMPLEMENTATION( "Qt::MPenCapStyle" );
            break;
    }
    return wxCAP_PROJECTING;
}

static Qt::PenJoinStyle ConvertPenJoinStyle(wxPenJoin style)
{
    switch (style)
    {
        case wxJOIN_BEVEL:
            return Qt::BevelJoin;

        case wxJOIN_MITER:
            return Qt::MiterJoin;

        case wxJOIN_ROUND:
            return Qt::RoundJoin;

        case wxJOIN_INVALID:
            wxFAIL_MSG( "Invalid pen join value" );
            break;
    }
    return Qt::BevelJoin;
}

static wxPenJoin ConvertPenJoinStyle(Qt::PenJoinStyle style)
{
    switch (style)
    {
        case Qt::BevelJoin:
            return wxJOIN_BEVEL;

        case Qt::MiterJoin:
            return wxJOIN_MITER;

        case Qt::RoundJoin:
            return wxJOIN_ROUND;

        case Qt::SvgMiterJoin:
            wxMISSING_IMPLEMENTATION( "Qt::SvgMiterJoin" );
            return wxJOIN_MITER;

        case Qt::MPenJoinStyle:
            wxMISSING_IMPLEMENTATION( "Qt::MPenJoinStyle" );
            break;
    }
    return wxJOIN_BEVEL;
}

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxGDIRefData
{
public:
    void defaultPen()
    {
        m_qtPen.setCapStyle(Qt::RoundCap);
        m_qtPen.setJoinStyle(Qt::RoundJoin);
        m_dashes = nullptr;
        m_dashesSize = 0;
    }

    wxPenRefData()
    {
        defaultPen();
    }

    wxPenRefData( const wxPenRefData& data )
    : wxGDIRefData()
        , m_qtPen(data.m_qtPen)
    {
        defaultPen();
    }

    wxPenRefData(const wxPenInfo& info)
    {
        m_qtPen.setWidth(info.GetWidth());
        m_qtPen.setStyle(ConvertPenStyle(info.GetStyle()));
        m_qtPen.setCapStyle(ConvertPenCapStyle(info.GetCap()));
        m_qtPen.setJoinStyle(ConvertPenJoinStyle(info.GetJoin()));
        m_qtPen.setColor(info.GetColour().GetQColor());

        m_dashesSize = info.GetDashes(const_cast<wxDash**>(&m_dashes));
    }

    bool operator == (const wxPenRefData& data) const
    {
        if ( m_dashesSize != data.m_dashesSize )
            return false;

        if ( m_dashes )
        {
            if ( !data.m_dashes ||
                 memcmp(m_dashes, data.m_dashes, m_dashesSize*sizeof(wxDash)) )
            {
                return false;
            }
        }
        else if ( data.m_dashes )
        {
            return false;
        }

        return m_qtPen == data.m_qtPen;
    }

    QPen m_qtPen;
    const wxDash *m_dashes;
    int m_dashesSize;
};

//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)->m_qtPen

wxPen::wxPen()
{
}

wxPen::wxPen( const wxColour &colour, int width, wxPenStyle style)
{
    m_refData = new wxPenRefData();
    M_PENDATA.setWidth(width);
    M_PENDATA.setStyle(ConvertPenStyle(style));
    M_PENDATA.setColor(colour.GetQColor());
}

wxPen::wxPen(const wxColour& col, int width, int style)
{
    m_refData = new wxPenRefData();
    M_PENDATA.setWidth(width);
    M_PENDATA.setStyle(ConvertPenStyle((wxPenStyle)style));
    M_PENDATA.setColor(col.GetQColor());
}

wxPen::wxPen(const wxPenInfo& info)
{
    m_refData = new wxPenRefData(info);
}

bool wxPen::operator==(const wxPen& pen) const
{
    if (m_refData == pen.m_refData) return true;

    if (!m_refData || !pen.m_refData) return false;

    return ( *(wxPenRefData*)m_refData == *(wxPenRefData*)pen.m_refData );
}

bool wxPen::operator!=(const wxPen& pen) const
{
    return !(*this == pen);
}

void wxPen::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_PENDATA.setColor(col.GetQColor());
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_PENDATA.setColor(QColor(r, g, b));
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();
    M_PENDATA.setWidth(width);
}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();
    M_PENDATA.setStyle(ConvertPenStyle(style));
}

void wxPen::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( "stippled pens not supported" );
}

void wxPen::SetDashes(int nb_dashes, const wxDash *dash)
{
    AllocExclusive();
    ((wxPenRefData *)m_refData)->m_dashes = dash;
    ((wxPenRefData *)m_refData)->m_dashesSize = nb_dashes;

    QVector<qreal> dashes;
    if (dash)
    {
        for (int i = 0; i < nb_dashes; i++)
            dashes << dash[i];
    }

    M_PENDATA.setDashPattern(dashes);
}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();
    M_PENDATA.setJoinStyle(ConvertPenJoinStyle(join));
}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();
    M_PENDATA.setCapStyle(ConvertPenCapStyle(cap));
}

wxColour wxPen::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, "invalid pen" );

    wxColour c(M_PENDATA.color());
    return c;
}

wxBitmap *wxPen::GetStipple() const
{
    return nullptr;
}

wxPenStyle wxPen::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxPENSTYLE_INVALID, "invalid pen" );

    return ConvertPenStyle(M_PENDATA.style());
}

wxPenJoin wxPen::GetJoin() const
{
    wxCHECK_MSG( IsOk(), wxJOIN_INVALID, "invalid pen" );

    return ConvertPenJoinStyle(M_PENDATA.joinStyle());
}

wxPenCap wxPen::GetCap() const
{
    wxCHECK_MSG( IsOk(), wxCAP_INVALID, "invalid pen" );

    return ConvertPenCapStyle(M_PENDATA.capStyle());
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, "invalid pen" );

    return M_PENDATA.width();
}

int wxPen::GetDashes(wxDash **ptr) const
{
    wxCHECK_MSG( IsOk(), -1, "invalid pen" );

    *ptr = (wxDash *)((wxPenRefData *)m_refData)->m_dashes;
    return ((wxPenRefData *)m_refData)->m_dashesSize;
}

QPen wxPen::GetHandle() const
{
    return IsOk() ? M_PENDATA : QPen();
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
