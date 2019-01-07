/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/region.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"
#include "wx/bitmap.h"
#include "wx/scopedarray.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"

#include <QtGui/QRegion>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>

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

wxIMPLEMENT_DYNAMIC_CLASS(wxRegion,wxGDIObject);

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
    if ( !bmp.GetHandle() ) {
        m_refData = new wxRegionRefData();
        return;
    }

    if ( tolerance == 0 ) {
        m_refData = new wxRegionRefData(bmp.GetHandle()->createMaskFromColor(transp.GetQColor()));
        return;
    }

    wxScopedArray<unsigned char> raw(bmp.GetWidth()*bmp.GetHeight());
    memset(raw.get(), 0, bmp.GetWidth()*bmp.GetHeight());

    QImage img(bmp.GetHandle()->toImage());
    int r = transp.Red(), g = transp.Green(), b = transp.Blue();
    for(int y=0; y<img.height(); y++)
    {
        for(int x=0; x<img.width(); x++)
        {
            QColor c = img.pixel(x, y);
            if ( abs(c.red()   - r ) > tolerance ||
               abs(c.green() - g) > tolerance ||
               abs(c.blue()  - b) > tolerance) {
                    int ind = y*img.width()+x;
                    raw[ind>>3] |= 1<<(ind&7);
            }
        }
    }
            
    m_refData = new wxRegionRefData(QBitmap::fromData(bmp.GetHandle()->size(), raw.get()));
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

void wxRegion::QtSetRegion(QRegion region)
{
    M_REGIONDATA = region;
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

const QRegion &wxRegion::GetHandle() const
{
    wxCHECK_MSG( IsOk(), GetHandle(), "Invalid region" );

    return M_REGIONDATA;
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

wxRegionIterator::wxRegionIterator()
{
    m_qtRects = NULL;
    m_pos = 0;
}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
    m_qtRects = new QVector< QRect >( region.GetHandle().rects() );
    m_pos = 0;
}

wxRegionIterator::wxRegionIterator(const wxRegionIterator& ri)
{
    m_qtRects = new QVector< QRect >( *ri.m_qtRects );
    m_pos = ri.m_pos;
}

wxRegionIterator::~wxRegionIterator()
{
    if ( m_qtRects != NULL )
        delete m_qtRects;
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    if ( m_qtRects != NULL )
        delete m_qtRects;
    
    m_qtRects = new QVector< QRect >( *ri.m_qtRects );
    m_pos = ri.m_pos;    
    return *this;
}

void wxRegionIterator::Reset()
{
    m_pos = 0;
}

void wxRegionIterator::Reset(const wxRegion& region)
{
    if ( m_qtRects != NULL )
        delete m_qtRects;

    m_qtRects = new QVector< QRect >( region.GetHandle().rects() );
    m_pos = 0;
}

bool wxRegionIterator::HaveRects() const
{
    wxCHECK_MSG( m_qtRects != NULL, false, "Invalid iterator" );
    
    return m_pos < m_qtRects->size();
}

wxRegionIterator::operator bool () const
{
    return HaveRects();
}

wxRegionIterator& wxRegionIterator::operator ++ ()
{
    m_pos++;
    return *this;
}

wxRegionIterator wxRegionIterator::operator ++ (int)
{
    wxRegionIterator copy(*this);
    ++*this;
    return copy;
}

wxCoord wxRegionIterator::GetX() const
{
    wxCHECK_MSG( m_qtRects != NULL, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );
    
    return m_qtRects->at( m_pos ).x();
}

wxCoord wxRegionIterator::GetY() const
{
    wxCHECK_MSG( m_qtRects != NULL, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );
    
    return m_qtRects->at( m_pos ).y();
}

wxCoord wxRegionIterator::GetW() const
{
    return GetWidth();
}

wxCoord wxRegionIterator::GetWidth() const
{
    wxCHECK_MSG( m_qtRects != NULL, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );
    
    return m_qtRects->at( m_pos ).width();
}

wxCoord wxRegionIterator::GetH() const
{
    return GetHeight();
}

wxCoord wxRegionIterator::GetHeight() const
{
    wxCHECK_MSG( m_qtRects != NULL, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );
    
    return m_qtRects->at( m_pos ).height();
}

wxRect wxRegionIterator::GetRect() const
{
    wxCHECK_MSG( m_qtRects != NULL, wxRect(), "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), wxRect(), "Invalid position" );
    
    return wxQtConvertRect( m_qtRects->at( m_pos ) );
}

