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

    wxRegionRefData( wxCoord x, wxCoord y, wxCoord w, wxCoord h )
    {
        // Rectangle needs to be defined in the canonical form,
        // with (x,y) pointing to the top-left corner of the box
        // and with non-negative width and height.
        // Notice that we would simply use QRect::normalized() here,
        // but we don't, because the normalized rectangle is an off-by-one
        // (width or height) for some inputs! which wx doesn't expect.

        if ( w < 0 )
        {
            w = -w;
            x -= (w - 1);
        }

        if ( h < 0 )
        {
            h = -h;
            y -= (h - 1);
        }

        m_qtRegion = QRegion( QRect{x, y, w, h} );
    }

    wxRegionRefData( QBitmap b ) : m_qtRegion ( b )
    {
    }

    wxRegionRefData( QPolygon p, Qt::FillRule fr ) : m_qtRegion( p, fr )
    {
    }

    wxRegionRefData( const wxRegionRefData& data )
        : wxGDIRefData()
        , m_qtRegion(data.m_qtRegion)
    {
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
    m_refData = nullptr;
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData( x, y, w, h );
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData( topLeft.x,
                                     topLeft.y,
                                     bottomRight.x - topLeft.x,
                                     bottomRight.y - topLeft.y );
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData( rect.x, rect.y, rect.width, rect.height );
}

wxRegion::wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle)
{
    QPolygon polygon;
    for ( size_t i = 0; i < n; ++i )
    {
        polygon << wxQtConvertPoint( points[i] );
    }

    Qt::FillRule fillingRule = fillStyle == wxODDEVEN_RULE ? Qt::OddEvenFill : Qt::WindingFill;
    m_refData = new wxRegionRefData( polygon, fillingRule );
}

wxRegion::wxRegion(const wxBitmap& bmp)
{
    if ( bmp.GetMask() != nullptr )
        m_refData = new wxRegionRefData( *bmp.GetMask()->GetHandle() );
    else
        m_refData = new wxRegionRefData( 0, 0, bmp.GetWidth(), bmp.GetHeight() );
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
    const int r = transp.Red(), g = transp.Green(), b = transp.Blue();
    for ( int y = 0; y < img.height(); y++ )
    {
        for ( int x = 0; x < img.width(); x++ )
        {
            const QColor c = img.pixel(x, y);
            if ( abs(c.red()   - r ) > tolerance ||
               abs(c.green() - g) > tolerance ||
               abs(c.blue()  - b) > tolerance) {
                    const int ind = y*img.width()+x;
                    raw[ind>>3] |= 1<<(ind&7);
            }
        }
    }

    m_refData = new wxRegionRefData(QBitmap::fromData(bmp.GetHandle()->size(), raw.get()));
}

bool wxRegion::IsEmpty() const
{
    if ( IsNull() )
        return true;

    wxCHECK_MSG(IsOk(), true, "Invalid region" );

    return M_REGIONDATA.isEmpty();
}

void wxRegion::Clear()
{
    if ( IsNull() )
        return;

    wxCHECK_RET(IsOk(), "Invalid region" );

    AllocExclusive();
    M_REGIONDATA = QRegion();
}

void wxRegion::QtSetRegion(QRegion region)
{
    AllocExclusive();
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
    if ( m_refData == nullptr )
    {
        x =
        y =
        w =
        h = 0;
        return false;
    }

    wxCHECK_MSG( IsOk(), false, "Invalid region" );

    const QRect bounding = M_REGIONDATA.boundingRect();
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

// combine another region with this one
bool wxRegion::DoCombine(const wxRegion& region, wxRegionOp op)
{
    // we can't use the API functions if we don't have a valid region
    if ( !m_refData )
    {
        // combining with an empty/invalid region works differently depending
        // on the operation
        switch ( op )
        {
            case wxRGN_COPY:
            case wxRGN_OR:
            case wxRGN_XOR:
                *this = region;
                break;

            default:
                wxFAIL_MSG(wxT("unknown region operation"));
                wxFALLTHROUGH;

            case wxRGN_AND:
            case wxRGN_DIFF:
                // leave empty/invalid
                return false;
        }
    }
    else // we have a valid region
    {
        AllocExclusive();

        switch ( op )
        {
        case wxRGN_AND:
            M_REGIONDATA = M_REGIONDATA.intersected(region.GetHandle());
            break;

        case wxRGN_OR:
            M_REGIONDATA = M_REGIONDATA.united(region.GetHandle());
            break;

        case wxRGN_XOR:
            M_REGIONDATA = M_REGIONDATA.xored(region.GetHandle());
            break;

        case wxRGN_DIFF:
            M_REGIONDATA = M_REGIONDATA.subtracted(region.GetHandle());
            break;

        default:
            wxFAIL_MSG(wxT("unknown region operation"));
            wxFALLTHROUGH;

        case wxRGN_COPY:
            M_REGIONDATA = QRegion(region.GetHandle());
            break;
        }
    }
    return true;
}

bool wxRegion::DoUnionWithRegion(const wxRegion& region)
{
    return DoCombine(region, wxRGN_OR);
}

bool wxRegion::DoIntersect(const wxRegion& region)
{
    return DoCombine(region, wxRGN_AND);
}

bool wxRegion::DoSubtract(const wxRegion& region)
{
    return DoCombine(region, wxRGN_DIFF);
}

bool wxRegion::DoXor(const wxRegion& region)
{
    return DoCombine(region, wxRGN_XOR);
}

bool wxRegion::DoUnionWithRect(const wxRect& rect)
{
    if ( m_refData == nullptr )
    {
        m_refData = new wxRegionRefData(rect.x, rect.y, rect.width, rect.height);
        return true;
    }

    wxCHECK_MSG( IsOk(), false, "Invalid region" );

    AllocExclusive();
    M_REGIONDATA = M_REGIONDATA.united( wxQtConvertRect( rect ) );
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
    m_qtRects = nullptr;
    m_pos = 0;
}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
    m_qtRects = nullptr;
    Reset(region);
}

wxRegionIterator::wxRegionIterator(const wxRegionIterator& ri)
    : wxObject()
{
    m_qtRects = new QVector< QRect >( *ri.m_qtRects );
    m_pos = ri.m_pos;
}

wxRegionIterator::~wxRegionIterator()
{
    delete m_qtRects;
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    if (this != &ri)
    {
        delete m_qtRects;
        m_qtRects = new QVector< QRect >( *ri.m_qtRects );
        m_pos = ri.m_pos;
    }
    return *this;
}

void wxRegionIterator::Reset()
{
    m_pos = 0;
}

void wxRegionIterator::Reset(const wxRegion& region)
{
    delete m_qtRects;

    auto qtRegion = region.GetHandle();
    m_qtRects = new QVector< QRect >();
    m_qtRects->reserve(qtRegion.rectCount());
    for (const auto& r : qtRegion)
        m_qtRects->push_back(r);

    m_pos = 0;
}

bool wxRegionIterator::HaveRects() const
{
    wxCHECK_MSG( m_qtRects != nullptr, false, "Invalid iterator" );

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
    wxCHECK_MSG( m_qtRects != nullptr, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );

    return m_qtRects->at( m_pos ).x();
}

wxCoord wxRegionIterator::GetY() const
{
    wxCHECK_MSG( m_qtRects != nullptr, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );

    return m_qtRects->at( m_pos ).y();
}

wxCoord wxRegionIterator::GetW() const
{
    return GetWidth();
}

wxCoord wxRegionIterator::GetWidth() const
{
    wxCHECK_MSG( m_qtRects != nullptr, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );

    return m_qtRects->at( m_pos ).width();
}

wxCoord wxRegionIterator::GetH() const
{
    return GetHeight();
}

wxCoord wxRegionIterator::GetHeight() const
{
    wxCHECK_MSG( m_qtRects != nullptr, 0, "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), 0, "Invalid position" );

    return m_qtRects->at( m_pos ).height();
}

wxRect wxRegionIterator::GetRect() const
{
    wxCHECK_MSG( m_qtRects != nullptr, wxRect(), "Invalid iterator" );
    wxCHECK_MSG( m_pos < m_qtRects->size(), wxRect(), "Invalid position" );

    return wxQtConvertRect( m_qtRects->at( m_pos ) );
}

