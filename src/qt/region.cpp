/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/region.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"
#include "wx/qt/converter.h"
#include "wx/qt/utils.h"

#include <QtGui/QRegion>
#include <QtGui/QBitmap>

class wxRegionRefData: public wxGDIRefData
{
    public:
        wxRegionRefData()
        {
        }

        wxRegionRefData( QRect r ) : m_qtRegion( r )
        {
        }

        wxRegionRefData( QBitmap b ) : m_qtRegion ( b )
        {
        }

        wxRegionRefData( QPolygon p, Qt::FillRule fr ) : m_qtRegion( p, fr )
        {
        }
        
        wxRegionRefData( const wxRegionRefData& data )
        : wxGDIRefData()
        {
            m_qtRegion = data.m_qtRegion;
        }
        
        bool operator == (const wxRegionRefData& data) const
        {
            return m_qtRegion == data.m_qtRegion;
        }
        
        QRegion m_qtRegion;
};

#define M_REGIONDATA ((wxRegionRefData *)m_refData)->m_qtRegion

wxRegion::wxRegion()
{
    m_refData = new wxRegionRefData();
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData( QRect( x, y, w, h ) );
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData( QRect( wxQtConvertPoint( topLeft ),
                                           wxQtConvertPoint( bottomRight ) ) );
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData( wxQtConvertRect( rect ) );
}

wxRegion::wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle)
{
    QVector< QPoint > qtPoints;
    for ( uint i = 0; i < n; i++)
    {
        qtPoints << wxQtConvertPoint( points[i] );
    }
    QPolygon p( qtPoints );

    Qt::FillRule fillingRule = fillStyle == wxODDEVEN_RULE ? Qt::OddEvenFill : Qt::WindingFill;
    m_refData = new wxRegionRefData( p, fillingRule );
}

wxRegion::wxRegion(const wxBitmap& bmp)
{
    if ( bmp.GetMask() != NULL )
        m_refData = new wxRegionRefData( *bmp.GetMask()->GetHandle() );
    else
        m_refData = new wxRegionRefData( QRect( 0, 0, bmp.GetWidth(), bmp.GetHeight() ) );
}

wxRegion::wxRegion(const wxBitmap& bmp, const wxColour& transp, int tolerance)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

bool wxRegion::IsEmpty() const
{
    wxCHECK_MSG( IsOk(), true, "Invalid region" );
    
    return M_REGIONDATA.isEmpty();
}

void wxRegion::Clear()
{
    wxCHECK_RET( IsOk(), "Invalid region" );

    AllocExclusive();
    M_REGIONDATA = QRegion();
}

wxGDIRefData *wxRegion::CreateGDIRefData() const
{
    return new wxRegionRefData;
}

wxGDIRefData *wxRegion::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxRegionRefData(*(wxRegionRefData *)data);
}

bool wxRegion::DoIsEqual(const wxRegion& region) const
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
    
    return M_REGIONDATA == region.GetHandle();
}

bool wxRegion::DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );

    QRect bounding = M_REGIONDATA.boundingRect();
    x = bounding.x();
    y = bounding.y();
    w = bounding.width();
    h = bounding.height();
    
    return true;
}

wxRegionContain wxRegion::DoContainsPoint(wxCoord x, wxCoord y) const
{
    wxCHECK_MSG( IsOk(), wxOutRegion, "Invalid region" );
    
    return M_REGIONDATA.contains( QPoint( x, y ) ) ? wxInRegion : wxOutRegion;
}

wxRegionContain wxRegion::DoContainsRect(const wxRect& rect) const
{
    wxCHECK_MSG( IsOk(), wxOutRegion, "Invalid region" );
    
    return M_REGIONDATA.contains( wxQtConvertRect( rect ) ) ? wxInRegion : wxOutRegion;
}

bool wxRegion::DoOffset(wxCoord x, wxCoord y)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );

    M_REGIONDATA.translate( x, y );
    return true;
}

bool wxRegion::DoUnionWithRect(const wxRect& rect)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );

    M_REGIONDATA = M_REGIONDATA.united( wxQtConvertRect( rect ) );
    return true;
}

bool wxRegion::DoUnionWithRegion(const wxRegion& region)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
    
    M_REGIONDATA = M_REGIONDATA.united( region.GetHandle() );
    return true;
}

bool wxRegion::DoIntersect(const wxRegion& region)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
    
    M_REGIONDATA = M_REGIONDATA.intersected( region.GetHandle() );
    return true;
}

bool wxRegion::DoSubtract(const wxRegion& region)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
    
    M_REGIONDATA = M_REGIONDATA.subtracted( region.GetHandle() );
    return true;
}

bool wxRegion::DoXor(const wxRegion& region)
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
    
    M_REGIONDATA = M_REGIONDATA.xored( region.GetHandle() );
    return true;
}

QRegion wxRegion::GetHandle() const
{
    wxCHECK_MSG( IsOk(), QRegion(), "Invalid region" );

    return M_REGIONDATA;
}

//##############################################################################

wxRegionIterator::wxRegionIterator()
{
}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
}

wxRegionIterator::wxRegionIterator(const wxRegionIterator& ri)
{
}

wxRegionIterator::~wxRegionIterator()
{
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    return *this;
}

void wxRegionIterator::Reset()
{
}

void wxRegionIterator::Reset(const wxRegion& region)
{
}

bool wxRegionIterator::HaveRects() const
{
    return false;
}

wxRegionIterator::operator bool () const
{
    return false;
}

wxRegionIterator& wxRegionIterator::operator ++ ()
{
    return *this;
}

wxRegionIterator wxRegionIterator::operator ++ (int)
{
    return *this;
}

wxCoord wxRegionIterator::GetX() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetY() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetW() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetWidth() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetH() const
{
    return wxCoord();
}

wxCoord wxRegionIterator::GetHeight() const
{
    return wxCoord();
}

wxRect wxRegionIterator::GetRect() const
{
    return wxRect();
}

