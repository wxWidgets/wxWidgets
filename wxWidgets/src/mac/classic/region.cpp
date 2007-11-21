/////////////////////////////////////////////////////////////////////////////
// File:      src/mac/classic/region.cpp
// Purpose:   Region class
// Author:    Stefan Csomor
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:    $Id$
// Copyright: (c) 1997 Stefan Csomor
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/region.h"

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

//-----------------------------------------------------------------------------
// wxRegionRefData implementation
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData {
public:
    wxRegionRefData()
    {
        m_macRgn = NewRgn() ;
    }

    wxRegionRefData(const wxRegionRefData& data)
        : wxGDIRefData()
    {
        m_macRgn = NewRgn() ;
        CopyRgn( data.m_macRgn , m_macRgn ) ;
    }

    virtual ~wxRegionRefData()
    {
        DisposeRgn( m_macRgn ) ;
    }
    RgnHandle    m_macRgn ;
};

#define M_REGION (((wxRegionRefData*)m_refData)->m_macRgn)
#define OTHER_M_REGION(a) (((wxRegionRefData*)(a.m_refData))->m_macRgn)

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

/*!
 * Create an empty region.
 */
wxRegion::wxRegion()
{
    m_refData = new wxRegionRefData;
}

wxRegion::wxRegion(WXHRGN hRegion )
{
    m_refData = new wxRegionRefData;
    CopyRgn( (RgnHandle) hRegion , (RgnHandle) M_REGION ) ;
}

wxRegion::wxRegion(long x, long y, long w, long h)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( (RgnHandle) M_REGION , x , y , x+w , y+h ) ;
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( (RgnHandle) M_REGION , topLeft.x , topLeft.y , bottomRight.x , bottomRight.y ) ;
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( (RgnHandle) M_REGION , rect.x , rect.y , rect.x+rect.width , rect.y+rect.height ) ;
}

/*!
 * Destroy the region.
 */
wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

//-----------------------------------------------------------------------------
//# Modify region
//-----------------------------------------------------------------------------

//! Clear current region
void wxRegion::Clear()
{
    UnRef();
}

//! Combine rectangle (x, y, w, h) with this.
bool wxRegion::Combine(long x, long y, long width, long height, wxRegionOp op)
{
    // Don't change shared data
    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
    }
    else if (m_refData->GetRefCount() > 1)
    {
        wxRegionRefData* ref = (wxRegionRefData*)m_refData;
        UnRef();
        m_refData = new wxRegionRefData(*ref);
    }
    RgnHandle rgn = NewRgn() ;
        SetRectRgn( rgn , x , y, x+width,y + height ) ;

    switch (op)
    {
        case wxRGN_AND:
            SectRgn( M_REGION , rgn , M_REGION ) ;
            break ;
        case wxRGN_OR:
            UnionRgn( M_REGION , rgn , M_REGION ) ;
            break ;
        case wxRGN_XOR:
             XorRgn( M_REGION , rgn , M_REGION ) ;
            break ;
        case wxRGN_DIFF:
            DiffRgn( M_REGION , rgn , M_REGION ) ;
            break ;
        case wxRGN_COPY:
        default:
                   CopyRgn( rgn ,M_REGION ) ;
            break ;
    }

        DisposeRgn( rgn ) ;

    return true;
}

//! Union /e region with this.
bool wxRegion::Combine(const wxRegion& region, wxRegionOp op)
{
    if (region.Empty())
        return false;

    // Don't change shared data
    if (!m_refData) {
        m_refData = new wxRegionRefData();
    }
    else if (m_refData->GetRefCount() > 1)
    {
        wxRegionRefData* ref = (wxRegionRefData*)m_refData;
        UnRef();
        m_refData = new wxRegionRefData(*ref);
    }

    switch (op)
    {
        case wxRGN_AND:
            SectRgn( M_REGION , OTHER_M_REGION(region) , M_REGION ) ;
            break ;
        case wxRGN_OR:
            UnionRgn( M_REGION , OTHER_M_REGION(region) , M_REGION ) ;
            break ;
        case wxRGN_XOR:
             XorRgn( M_REGION , OTHER_M_REGION(region) , M_REGION ) ;
            break ;
        case wxRGN_DIFF:
            DiffRgn( M_REGION , OTHER_M_REGION(region) , M_REGION ) ;
            break ;
        case wxRGN_COPY:
        default:
                   CopyRgn( OTHER_M_REGION(region) ,M_REGION ) ;
            break ;
    }

    return true;
}

bool wxRegion::Combine(const wxRect& rect, wxRegionOp op)
{
    return Combine(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), op);
}

//-----------------------------------------------------------------------------
//# Information on region
//-----------------------------------------------------------------------------

// Outer bounds of region
void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    if (m_refData)
    {
        Rect box ;
        GetRegionBounds( M_REGION , &box ) ;
        x = box.left ;
        y = box.top ;
        w = box.right - box.left ;
        h = box.bottom - box.top ;
    }
    else
    {
        x = y = w = h = 0;
    }
}

wxRect wxRegion::GetBox() const
{
    wxCoord x, y, w, h;
    GetBox(x, y, w, h);
    return wxRect(x, y, w, h);
}

// Is region empty?
bool wxRegion::Empty() const
{
    return EmptyRgn( M_REGION ) ;
}

const WXHRGN wxRegion::GetWXHRGN() const
{
    return M_REGION ;
}

//-----------------------------------------------------------------------------
//# Tests
//-----------------------------------------------------------------------------

// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(long x, long y) const
{
    if (!m_refData)
        return wxOutRegion;

    // TODO. Return wxInRegion if within region.
    if (0)
        return wxInRegion;
    return wxOutRegion;
}

// Does the region contain the point pt?
wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    if (!m_refData)
        return wxOutRegion;

    Point p = { pt.y , pt.x } ;
    if (PtInRgn( p , M_REGION ) )
        return wxInRegion;

    return wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(long x, long y, long w, long h) const
{
    if (!m_refData)
        return wxOutRegion;

    Rect rect = { y , x , y + h , x + w } ;
    if (RectInRgn( &rect , M_REGION ) )
        return wxInRegion;
    else
        return wxOutRegion;
}

// Does the region contain the rectangle rect
wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    if (!m_refData)
        return wxOutRegion;

    long x, y, w, h;
    x = rect.x;
    y = rect.y;
    w = rect.GetWidth();
    h = rect.GetHeight();
    return Contains(x, y, w, h);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                               wxRegionIterator                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*!
 * Initialize empty iterator
 */
wxRegionIterator::wxRegionIterator()
    : m_current(0), m_numRects(0), m_rects(NULL)
{
}

wxRegionIterator::~wxRegionIterator()
{
    if (m_rects) {
        delete[] m_rects;
        m_rects = NULL;
    }
}

wxRegionIterator::wxRegionIterator(const wxRegionIterator& iterator)
    : wxObject()
    , m_current(iterator.m_current)
    , m_numRects(0)
    , m_rects(NULL)
{
    SetRects(iterator.m_numRects, iterator.m_rects);
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& iterator)
{
    m_current  = iterator.m_current;
    SetRects(iterator.m_numRects, iterator.m_rects);
    return *this;
}

/*!
 * Set iterator rects for region
 */
void wxRegionIterator::SetRects(long numRects, wxRect *rects)
{
    if (m_rects) {
        delete[] m_rects;
        m_rects = NULL;
    }
    if (rects)
    {
        int i;
        m_rects = new wxRect[numRects];
        for (i = 0; i < numRects; i++)
            m_rects[i] = rects[i];
    }
    m_numRects = numRects;
}

/*!
 * Initialize iterator for region
 */
wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
    m_rects = NULL;

    Reset(region);
}

/*!
 * Reset iterator for a new /e region.
 */
void wxRegionIterator::Reset(const wxRegion& region)
{
    m_current = 0;
    m_region = region;

    if (m_rects) {
        delete[] m_rects;
        m_rects = NULL;
    }

    if (m_region.Empty())
        m_numRects = 0;
    else
    {
        // we cannot dissolve it into rects on mac
        m_rects = new wxRect[1];
        Rect rect ;
        GetRegionBounds( OTHER_M_REGION( region ) , &rect ) ;
        m_rects[0].x = rect.left;
        m_rects[0].y = rect.top;
        m_rects[0].width = rect.right - rect.left;
        m_rects[0].height = rect.bottom - rect.top;
        m_numRects = 1;
    }
}

/*!
 * Increment iterator. The rectangle returned is the one after the
 * incrementation.
 */
wxRegionIterator& wxRegionIterator::operator ++ ()
{
    if (m_current < m_numRects)
        ++m_current;
    return *this;
}

/*!
 * Increment iterator. The rectangle returned is the one before the
 * incrementation.
 */
wxRegionIterator wxRegionIterator::operator ++ (int)
{
    wxRegionIterator previous(*this);

    if (m_current < m_numRects)
        ++m_current;

    return previous;
}

long wxRegionIterator::GetX() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].x;
    return 0;
}

long wxRegionIterator::GetY() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].y;
    return 0;
}

long wxRegionIterator::GetW() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].width ;
    return 0;
}

long wxRegionIterator::GetH() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].height;
    return 0;
}
