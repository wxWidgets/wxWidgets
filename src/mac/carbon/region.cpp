/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    Markus Holzem/Julian Smart/AUTHOR
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:	  $Id$
// Copyright: (c) 1997 Markus Holzem/Julian Smart/AUTHOR
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"
#include "wx/gdicmn.h"

#if !USE_SHARED_LIBRARY
	IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
	IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)
#endif

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
	{
		m_macRgn = NewRgn() ;
        CopyRgn( data.m_macRgn , m_macRgn ) ;
	}

	~wxRegionRefData()
	{
        DisposeRgn( m_macRgn ) ;
	}
	RgnHandle	m_macRgn ;
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
    CopyRgn( hRegion , M_REGION ) ;
}

wxRegion::wxRegion(long x, long y, long w, long h)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( M_REGION , x , y , x+w , y+h ) ;
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( M_REGION , topLeft.x , topLeft.y , bottomRight.x , bottomRight.y ) ;
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( M_REGION , rect.x , rect.y , rect.x+rect.width , rect.y+rect.height ) ;
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

    return TRUE;
}

//! Union /e region with this.
bool wxRegion::Combine(const wxRegion& region, wxRegionOp op)
{
	if (region.Empty())
		return FALSE;

	// Don't change shared data
	if (!m_refData) {
		m_refData = new wxRegionRefData();
	} 
	else	if (m_refData->GetRefCount() > 1) 
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

	return TRUE;
}

bool wxRegion::Combine(const wxRect& rect, wxRegionOp op)
{
    return Combine(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), op);
}

//-----------------------------------------------------------------------------
//# Information on region
//-----------------------------------------------------------------------------

// Outer bounds of region
void wxRegion::GetBox(long& x, long& y, long&w, long &h) const
{
	if (m_refData) 
	{
		Rect box = (**M_REGION).rgnBBox ;
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
    long x, y, w, h;
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
//																			 //
//							   wxRegionIterator								 //
//																			 //
///////////////////////////////////////////////////////////////////////////////

/*!
 * Initialize empty iterator
 */
wxRegionIterator::wxRegionIterator() : m_current(0), m_numRects(0), m_rects(NULL)
{
}

wxRegionIterator::~wxRegionIterator()
{
    if (m_rects)
        delete[] m_rects;
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

    if (m_rects)
        delete[] m_rects;

    m_rects = NULL;

	if (m_region.Empty())
		m_numRects = 0;
	else
    {
    	// we cannot dissolve it into rects on mac
        m_rects = new wxRect[1];
        Rect rect = (**OTHER_M_REGION( region )).rgnBBox ;
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
void wxRegionIterator::operator ++ ()
{
	if (m_current < m_numRects)
		++m_current;
}

/*!
 * Increment iterator. The rectangle returned is the one before the
 * incrementation.
 */
void wxRegionIterator::operator ++ (int)
{
	if (m_current < m_numRects)
		++m_current;
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

