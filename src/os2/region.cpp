/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    David Webster
// Modified by:
// Created:   10/15/99
// RCS-ID:	  $Id$
// Copyright: (c) Davdi Webster
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/os2/region.h"
#include "wx/gdicmn.h"

#include "wx/window.h"
#include "wx/os2/private.h"

	IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
	IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

//-----------------------------------------------------------------------------
// wxRegionRefData implementation
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData {
public:
    wxRegionRefData()
    {
        m_region = 0;
    }

    wxRegionRefData(const wxRegionRefData& data)
    {
        // TODO
    }

    ~wxRegionRefData()
    {
        // TODO

    }

    HRGN m_region;
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
    m_refData = new wxRegionRefData;
    // TODO create empty region
}

wxRegion::wxRegion(WXHRGN hRegion)
{
    m_refData = new wxRegionRefData;
    M_REGION = (HRGN) hRegion;
}

wxRegion::wxRegion(long x, long y, long w, long h)
{
    m_refData = new wxRegionRefData;
    // TODO create rect region
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;
    // TODO create rect region
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;
    // TODO create rect region
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

    // TODO create rect region

    int mode = 0; // TODO platform-specific code
    switch (op)
    {
        case wxRGN_AND:
            // TODO
            break ;
        case wxRGN_OR:
            // TODO
            break ;
        case wxRGN_XOR:
            // TODO
            break ;
        case wxRGN_DIFF:
            // TODO
            break ;
        case wxRGN_COPY:
        default:
            // TODO
            break ;
    }

    // TODO do combine region

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
    } else  if (m_refData->GetRefCount() > 1) {
        wxRegionRefData* ref = (wxRegionRefData*)m_refData;
        UnRef();
        m_refData = new wxRegionRefData(*ref);
    }

    int mode = 0; // TODO platform-specific code
    switch (op)
    {
        case wxRGN_AND:
            // TODO
            break ;
        case wxRGN_OR:
            // TODO
            break ;
        case wxRGN_XOR:
            // TODO
            break ;
        case wxRGN_DIFF:
            // TODO
            break ;
        case wxRGN_COPY:
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
        // TODO get box
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
    // TODO
    return FALSE;
}

//-----------------------------------------------------------------------------
//# Tests
//-----------------------------------------------------------------------------

// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(long x, long y) const
{
    bool    bOK = FALSE; // temporary
    if (!m_refData)
        return wxOutRegion;

    // TODO. Return wxInRegion if within region.
    if (bOK)
        return wxInRegion;
    return wxOutRegion;
}

// Does the region contain the point pt?
wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    bool    bOK = FALSE; // temporary
    if (!m_refData)
        return wxOutRegion;

    // TODO. Return wxInRegion if within region.
    if (bOK)
        return wxInRegion;
    else
        return wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(long x, long y, long w, long h) const
{
    bool    bOK = FALSE; // temporary
    if (!m_refData)
        return wxOutRegion;

    // TODO. Return wxInRegion if within region.
    if (bOK)
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

// Get internal region handle
WXHRGN wxRegion::GetHRGN() const
{
    if (!m_refData)
        return (WXHRGN) 0;
    return (WXHRGN) M_REGION;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             wxRegionIterator                              //
//                                                                           //
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

