/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/pen.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"
#include "wx/colour.h"
#include "wx/qt/utils.h"
#include <QtGui/QPen>

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxGDIRefData
{
    public:
        wxPenRefData()
        {
        }
        
        wxPenRefData( const wxPenRefData& data )
        : wxGDIRefData()
        {
            m_qtPen = data.m_qtPen;
        }
        
        bool operator == (const wxPenRefData& data) const
        {
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
    M_PENDATA.setStyle(wxQtPenStyle(style));
    M_PENDATA.setColor(colour.GetPixel());
}

#if FUTURE_WXWIN_COMPATIBILITY_3_0
wxPen::wxPen(const wxColour& col, int width, int style)
{
    m_refData = new wxPenRefData();
    M_PENDATA.setWidth(width);
    M_PENDATA.setStyle(wxQtPenStyle((wxPenStyle)style));
    M_PENDATA.setColor(col.GetPixel());
}

#endif

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
    M_PENDATA.setColor(col.GetPixel());
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
    M_PENDATA.setStyle(wxQtPenStyle(style));
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
    for (int i = 0; i < nb_dashes; i++) {
        dashes << dash[i];
    }
    
    M_PENDATA.setDashPattern(dashes);
}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();
    M_PENDATA.setJoinStyle(wxQtPenJoinStyle(join));
}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();
    M_PENDATA.setCapStyle(wxQtPenCapStyle(cap));
}

wxColour wxPen::GetColour() const
{
    wxColour c(M_PENDATA.color());
    return c;
}

wxBitmap *wxPen::GetStipple() const
{
    return NULL;
}

wxPenStyle wxPen::GetStyle() const
{
    return wxQtPenStyle(M_PENDATA.style());
}

wxPenJoin wxPen::GetJoin() const
{
    return wxQtPenJoinStyle(M_PENDATA.joinStyle());
}

wxPenCap wxPen::GetCap() const
{
    return wxQtPenCapStyle(M_PENDATA.capStyle());
}

int wxPen::GetWidth() const
{
    return M_PENDATA.width();
}

int wxPen::GetDashes(wxDash **ptr) const
{
    *ptr = (wxDash *)((wxPenRefData *)m_refData)->m_dashes;
    return ((wxPenRefData *)m_refData)->m_dashesSize;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}

Qt::PenStyle wxPen::wxQtPenStyle(wxPenStyle style)
{
    switch(style) {
        case wxPENSTYLE_SOLID: return Qt::SolidLine;
        case wxPENSTYLE_TRANSPARENT: return Qt::NoPen;
        case wxPENSTYLE_DOT: return Qt::DotLine;
        case wxPENSTYLE_LONG_DASH:
        case wxPENSTYLE_SHORT_DASH: return Qt::DashLine;
        case wxPENSTYLE_DOT_DASH: return Qt::DashDotLine;
        case wxPENSTYLE_USER_DASH: return Qt::CustomDashLine;
        
        case wxPENSTYLE_STIPPLE:
        case wxPENSTYLE_BDIAGONAL_HATCH:
        case wxPENSTYLE_CROSSDIAG_HATCH:
        case wxPENSTYLE_FDIAGONAL_HATCH:
        case wxPENSTYLE_CROSS_HATCH:
        case wxPENSTYLE_HORIZONTAL_HATCH:
        case wxPENSTYLE_VERTICAL_HATCH:
        default: wxMISSING_IMPLEMENTATION( "wxPen styles" ); return Qt::SolidLine;
    }
}

wxPenStyle wxPen::wxQtPenStyle(Qt::PenStyle style)
{
    switch (style) {
        case Qt::SolidLine: return wxPENSTYLE_SOLID;
        case Qt::NoPen: return wxPENSTYLE_TRANSPARENT;
        case Qt::DotLine: return wxPENSTYLE_DOT;
        case Qt::DashLine: return wxPENSTYLE_SHORT_DASH;
        case Qt::DashDotLine: return wxPENSTYLE_DOT_DASH;
        case Qt::CustomDashLine: return wxPENSTYLE_USER_DASH;
        
        default: wxFAIL_MSG( "Unknown QPen style" );
    }
    return wxPENSTYLE_SOLID;
}

Qt::PenCapStyle wxPen::wxQtPenCapStyle(wxPenCap style)
{
    switch (style) {
        case wxCAP_BUTT: return Qt::FlatCap;
        case wxCAP_PROJECTING: return Qt::SquareCap;
        case wxCAP_ROUND: return Qt::RoundCap;
        
        default: return Qt::SquareCap;
    }
}

wxPenCap wxPen::wxQtPenCapStyle(Qt::PenCapStyle style)
{
    switch (style) {
        case Qt::FlatCap: return wxCAP_BUTT;
        case Qt::SquareCap: return wxCAP_PROJECTING;
        case Qt::RoundCap: return wxCAP_ROUND;
        
        default: wxFAIL_MSG( "Unknown QPen cap style" );
    }
    return wxCAP_PROJECTING;
}

Qt::PenJoinStyle wxPen::wxQtPenJoinStyle(wxPenJoin style)
{
    switch (style) {
        case wxJOIN_BEVEL: return Qt::BevelJoin;
        case wxJOIN_MITER: return Qt::MiterJoin;
        case wxJOIN_ROUND: return Qt::RoundJoin;
        
        default: return Qt::BevelJoin;
    }
    
}

wxPenJoin wxPen::wxQtPenJoinStyle(Qt::PenJoinStyle style)
{
    switch (style) {
        case Qt::BevelJoin: return wxJOIN_BEVEL;
        case Qt::MiterJoin: return wxJOIN_MITER;
        case Qt::RoundJoin: return wxJOIN_ROUND;
        
        default: wxFAIL_MSG( "Unknown QPen join style" );
    }
    return wxJOIN_BEVEL;
}
