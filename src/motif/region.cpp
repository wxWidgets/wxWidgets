/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    Markus Holzem/Julian Smart
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:	  $Id$
// Copyright: (c) 1997 Markus Holzem/Julian Smart
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"
#include "wx/gdicmn.h"

#include <Xm/Xm.h>
// #include "wx/motif/private.h"

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
		m_region = XCreateRegion();
	}

	wxRegionRefData(const wxRegionRefData& data)
	{
		m_region = XCreateRegion();
		XUnionRegion(m_region, data.m_region, m_region);
	}

	~wxRegionRefData()
	{
		XDestroyRegion(m_region);
	}
    Region  m_region;
};

#define M_REGION (((wxRegionRefData*)m_refData)->m_region)

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

/*!
 * Create an empty region.
 */
wxRegion::wxRegion()
{
}

wxRegion::wxRegion(long x, long y, long w, long h)
{
    m_refData = new wxRegionRefData;

	XRectangle rect;
	rect.x		= x;
	rect.y		= y;
	rect.width	= w;
	rect.height = h;
	XUnionRectWithRegion(&rect, M_REGION, M_REGION);
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;

	XRectangle rect;
	rect.x		= topLeft.x;
	rect.y		= topLeft.y;
	rect.width	= bottomRight.x - topLeft.x;
	rect.height = bottomRight.y - topLeft.y;
	XUnionRectWithRegion(&rect, M_REGION, M_REGION);
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;

    XRectangle rect1;
    rect1.x		= rect.x;
    rect1.y		= rect.y;
    rect1.width	= rect.width;
    rect1.height = rect.height;
    XUnionRectWithRegion(&rect1, M_REGION, M_REGION);
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
	if (!m_refData) {
		m_refData = new wxRegionRefData();
	} else if (m_refData->GetRefCount() > 1) {
		wxRegionRefData* ref = (wxRegionRefData*)m_refData;
		UnRef();
		m_refData = new wxRegionRefData(*ref);
	}
    // If ref count is 1, that means it's 'ours' anyway so no action.

    Region rectRegion = XCreateRegion();

	XRectangle rect;
	rect.x		= x;
	rect.y		= y;
	rect.width	= width;
	rect.height = height;
	XUnionRectWithRegion(&rect, rectRegion, rectRegion);

    int mode = 0; // TODO platform-specific code
    switch (op)
    {
        case wxRGN_AND:
	        XIntersectRegion(M_REGION, rectRegion, M_REGION);
            break ;
        case wxRGN_OR:
	        XUnionRegion(M_REGION, rectRegion, M_REGION);
            break ;
        case wxRGN_XOR:
            // TODO
            break ;
        case wxRGN_DIFF:
            // TODO
            break ;
        case wxRGN_COPY: // Don't have to do this one
        default:
            // TODO
            break ;
    }

    return FALSE;
}

//! Union /e region with this.
bool wxRegion::Combine(const wxRegion& region, wxRegionOp op)
{
	if (region.Empty())
		return FALSE;

	// Don't change shared data
	if (!m_refData) {
		m_refData = new wxRegionRefData();
	} else	if (m_refData->GetRefCount() > 1) {
		wxRegionRefData* ref = (wxRegionRefData*)m_refData;
		UnRef();
		m_refData = new wxRegionRefData(*ref);
	}

    int mode = 0; // TODO platform-specific code
    switch (op)
    {
        case wxRGN_AND:
	        XIntersectRegion(M_REGION, ((wxRegionRefData*)region.m_refData)->m_region,
				 M_REGION);
            break ;
        case wxRGN_OR:
	        XUnionRegion(M_REGION, ((wxRegionRefData*)region.m_refData)->m_region,
				 M_REGION);
            break ;
        case wxRGN_XOR:
            // TODO
            break ;
        case wxRGN_DIFF:
            // TODO
            break ;
        case wxRGN_COPY: // Don't have to do this one
        default:
            // TODO
            break ;
    }

    // TODO combine region

	return FALSE;
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
	if (m_refData) {
		XRectangle rect;
		XClipBox(M_REGION, &rect);
		x = rect.x;
		y = rect.y;
		w = rect.width;
		h = rect.height;
	} else {
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
	return m_refData ? XEmptyRegion(M_REGION) : FALSE;
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

	return XPointInRegion(M_REGION, pt.x, pt.y) ? wxInRegion : wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(long x, long y, long w, long h) const
{
	if (!m_refData)
		return wxOutRegion;

	switch (XRectInRegion(M_REGION, x, y, w, h)) {
	    case RectangleIn:	return wxInRegion;
	    case RectanglePart: return wxPartRegion;
	}
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
        // TODO create m_rects and fill with rectangles for this region
        m_numRects = 0;
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

