/////////////////////////////////////////////////////////////////////////////
// Name:      msw/region.cpp
// Purpose:   Region handling for wxWindows/X11
// Author:    Markus Holzem
// Modified by:
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:    $Id$
// Copyright: (c) 1997 Julian Smart and Markus Holzem
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/msw/region.h"
#include "wx/gdicmn.h"

#include "wx/window.h"
#include "wx/msw/private.h"

    IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
    IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

//-----------------------------------------------------------------------------
// wxRegionRefData implementation
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData
{
public:
    wxRegionRefData()
    {
        m_region = 0;
    }

    wxRegionRefData(const wxRegionRefData& data)
    {
#if defined(__WIN32__)
        DWORD noBytes = ::GetRegionData(data.m_region, 0, NULL);
        RGNDATA *rgnData = (RGNDATA*) new char[noBytes];
        ::GetRegionData(data.m_region, noBytes, rgnData);
        m_region = ::ExtCreateRegion(NULL, noBytes, rgnData);
        delete[] (char*) rgnData;
#else
        RECT rect;
        ::GetRgnBox(data.m_region, &rect);
        m_region = ::CreateRectRgnIndirect(&rect);
#endif
    }

    ~wxRegionRefData()
    {
        ::DeleteObject(m_region);
        m_region = 0;
    }

    HRGN m_region;
};

#define M_REGION (((wxRegionRefData*)m_refData)->m_region)

//-----------------------------------------------------------------------------
// wxRegion
//-----------------------------------------------------------------------------

/*
 * Create an empty region.
 */
wxRegion::wxRegion()
{
    m_refData = new wxRegionRefData;
    M_REGION = ::CreateRectRgn(0, 0, 0, 0);
}

wxRegion::wxRegion(WXHRGN hRegion)
{
    m_refData = new wxRegionRefData;
    M_REGION = (HRGN) hRegion;
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData;
    M_REGION = ::CreateRectRgn(x, y, x + w, y + h);
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;
    M_REGION = ::CreateRectRgn(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;
    M_REGION = ::CreateRectRgn(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
}

/*
 * Destroy the region.
 */
wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

//-----------------------------------------------------------------------------
// Modify region
//-----------------------------------------------------------------------------

// Clear current region
void wxRegion::Clear()
{
    UnRef();
}

// Combine rectangle (x, y, w, h) with this.
bool wxRegion::Combine(wxCoord x, wxCoord y, wxCoord width, wxCoord height, wxRegionOp op)
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

    HRGN rectRegion = ::CreateRectRgn(x, y, x + width, y + height);

    int mode = 0;
    switch (op)
    {
        case wxRGN_AND: mode = RGN_AND; break ;
        case wxRGN_OR: mode = RGN_OR; break ;
        case wxRGN_XOR: mode = RGN_XOR; break ;
        case wxRGN_DIFF: mode = RGN_DIFF; break ;
        case wxRGN_COPY:
        default:
            mode = RGN_COPY; break ;
    }

    bool success = (ERROR != ::CombineRgn(M_REGION, M_REGION, rectRegion, mode));

    ::DeleteObject(rectRegion);

    return success;
}

// Union /e region with this.
bool wxRegion::Combine(const wxRegion& region, wxRegionOp op)
{
    if (region.Empty())
        return FALSE;

    // Don't change shared data
    if (!m_refData) {
        m_refData = new wxRegionRefData();
    } else    if (m_refData->GetRefCount() > 1) {
        wxRegionRefData* ref = (wxRegionRefData*)m_refData;
        UnRef();
        m_refData = new wxRegionRefData(*ref);
    }

    int mode = 0;
    switch (op)
    {
        case wxRGN_AND: mode = RGN_AND; break ;
        case wxRGN_OR: mode = RGN_OR; break ;
        case wxRGN_XOR: mode = RGN_XOR; break ;
        case wxRGN_DIFF: mode = RGN_DIFF; break ;
        case wxRGN_COPY:
        default:
            mode = RGN_COPY; break ;
    }

    return (ERROR != ::CombineRgn(M_REGION, M_REGION, ((wxRegionRefData*)region.m_refData)->m_region, mode));
}

bool wxRegion::Combine(const wxRect& rect, wxRegionOp op)
{
    return Combine(rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), op);
}

//-----------------------------------------------------------------------------
// Information on region
//-----------------------------------------------------------------------------

// Outer bounds of region
void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const
{
    if (m_refData) {
        RECT rect;
        ::GetRgnBox(M_REGION, & rect);
        x = rect.left;
        y = rect.top;
        w = rect.right - rect.left;
        h = rect.bottom - rect.top;
    } else {
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
    if (M_REGION == 0)
        return TRUE;
    wxCoord x, y, w, h;
    GetBox(x, y, w, h);

    return ((w == 0) && (h == 0));
}

//-----------------------------------------------------------------------------
// Tests
//-----------------------------------------------------------------------------

// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y) const
{
    if (!m_refData)
        return wxOutRegion;

    if (::PtInRegion(M_REGION, (int) x, (int) y))
        return wxInRegion;
    else
        return wxOutRegion;
}

// Does the region contain the point pt?
wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    if (!m_refData)
        return wxOutRegion;

    if (::PtInRegion(M_REGION, (int) pt.x, (int) pt.y))
        return wxInRegion;
    else
        return wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h) const
{
    if (!m_refData)
        return wxOutRegion;

    RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + w;
    rect.bottom = y + h;

    if (::RectInRegion(M_REGION, & rect))
        return wxInRegion;
    else
        return wxOutRegion;
}

// Does the region contain the rectangle rect
wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    if (!m_refData)
        return wxOutRegion;

    wxCoord x, y, w, h;
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
//                                                                             //
//                               wxRegionIterator                                 //
//                                                                             //
///////////////////////////////////////////////////////////////////////////////

/*
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

/*
 * Initialize iterator for region
 */
wxRegionIterator::wxRegionIterator(const wxRegion& region)
{
    m_rects = NULL;

    Reset(region);
}

/*
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
#if defined(__WIN32__)
        DWORD noBytes = ::GetRegionData(((wxRegionRefData*)region.m_refData)->m_region, 0, NULL);
        RGNDATA *rgnData = (RGNDATA*) new char[noBytes];
        ::GetRegionData(((wxRegionRefData*)region.m_refData)->m_region, noBytes, rgnData);

        RGNDATAHEADER* header = (RGNDATAHEADER*) rgnData;

        m_rects = new wxRect[header->nCount];

        RECT* rect = (RECT*) ((char*)rgnData + sizeof(RGNDATAHEADER)) ;
        size_t i;
        for (i = 0; i < header->nCount; i++)
        {
            m_rects[i] = wxRect(rect->left, rect->top,
                                 rect->right - rect->left, rect->bottom - rect->top);
            rect ++; // Advances pointer by sizeof(RECT)
        }

        m_numRects = header->nCount;

        delete[] (char*) rgnData;
#else
        RECT rect;
        ::GetRgnBox(((wxRegionRefData*)region.m_refData)->m_region, &rect);
        m_rects = new wxRect[1];
        m_rects[0].x = rect.left;
        m_rects[0].y = rect.top;
        m_rects[0].width = rect.right - rect.left;
        m_rects[0].height = rect.bottom - rect.top;

        m_numRects = 1;
#endif
    }
}

/*
 * Increment iterator. The rectangle returned is the one after the
 * incrementation.
 */
void wxRegionIterator::operator ++ ()
{
    if (m_current < m_numRects)
        ++m_current;
}

/*
 * Increment iterator. The rectangle returned is the one before the
 * incrementation.
 */
void wxRegionIterator::operator ++ (int)
{
    if (m_current < m_numRects)
        ++m_current;
}

wxCoord wxRegionIterator::GetX() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].x;
    return 0;
}

wxCoord wxRegionIterator::GetY() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].y;
    return 0;
}

wxCoord wxRegionIterator::GetW() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].width ;
    return 0;
}

wxCoord wxRegionIterator::GetH() const
{
    if (m_current < m_numRects)
        return m_rects[m_current].height;
    return 0;
}

