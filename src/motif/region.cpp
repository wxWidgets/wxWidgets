/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    Markus Holzem/Julian Smart
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:      $Id$
// Copyright: (c) 1997 Markus Holzem/Julian Smart
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "region.h"
#endif

#include "wx/region.h"
#include "wx/gdicmn.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif
// #include "wx/motif/private.h"

    IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
    IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

// ----------------------------------------------------------------------------
// list types
// ----------------------------------------------------------------------------

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxRectList);

//-----------------------------------------------------------------------------
// wxRegionRefData implementation
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData {
public:
    wxRegionRefData()
    {
        m_region = XCreateRegion();
        m_usingRects = FALSE;
        m_rects = (wxRect*) NULL;
        m_rectCount = 0;
    }

    wxRegionRefData(const wxRegionRefData& data)
    {
        m_region = XCreateRegion();
        m_rects = (wxRect*) NULL;
        m_rectCount = 0;
        XUnionRegion(m_region, data.m_region, m_region);

        SetRects(data.m_rectCount, data.m_rects);
    }

    ~wxRegionRefData()
    {
        XDestroyRegion(m_region);
        DeleteRects();
    }

    wxRect* GetRects() { return m_rects; };
    void SetRects(const wxRectList& rectList);
    void SetRects(int count, const wxRect* rects);
    bool UsingRects() const { return m_usingRects; }
    int GetRectCount() const { return m_rectCount; }

    void DeleteRects();

    Region      m_region;
    wxRect*     m_rects;
    int         m_rectCount;
    bool        m_usingRects;  // TRUE if we're using the above.
};

void wxRegionRefData::SetRects(const wxRectList& rectList)
{
    DeleteRects();
    m_usingRects = (rectList.Number() > 0);
    if (m_usingRects)
    {
      m_rectCount = rectList.Number();
      m_rects = new wxRect[m_rectCount];
    }

    wxRectList::Node* node = rectList.GetFirst();
    int i = 0;
    while (node) {
        wxRect* rect = node->GetData();
        m_rects[i] = * rect;
        node = node->GetNext();
        i ++;
    }
}

void wxRegionRefData::SetRects(int count, const wxRect* rects)
{
    DeleteRects();
    m_usingRects = (count > 0);
    if (m_usingRects)
    {
      m_rectCount = count;
      m_rects = new wxRect[m_rectCount];
      int i;
      for (i = 0; i < m_rectCount; i++)
        m_rects[i] = rects[i];
    }
}

void wxRegionRefData::DeleteRects()
{
   if (m_rects)
   {
      delete[] m_rects;
      m_rects = (wxRect*) NULL;
   }
   m_rectCount = 0;
   m_usingRects = FALSE;
 }

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

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData;

    XRectangle rect;
    rect.x        = x;
    rect.y        = y;
    rect.width    = w;
    rect.height = h;
    XUnionRectWithRegion(&rect, M_REGION, M_REGION);
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;

    XRectangle rect;
    rect.x        = topLeft.x;
    rect.y        = topLeft.y;
    rect.width    = bottomRight.x - topLeft.x;
    rect.height = bottomRight.y - topLeft.y;
    XUnionRectWithRegion(&rect, M_REGION, M_REGION);
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;

    XRectangle rect1;
    rect1.x        = rect.x;
    rect1.y        = rect.y;
    rect1.width    = rect.width;
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

// Get the internal region handle
WXRegion wxRegion::GetXRegion() const
{
    wxASSERT( m_refData !=NULL );

    return (WXRegion) ((wxRegionRefData*)m_refData)->m_region;
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

    Region rectRegion = XCreateRegion();

    XRectangle rect;
    rect.x        = x;
    rect.y        = y;
    rect.width    = width;
    rect.height = height;
    XUnionRectWithRegion(&rect, rectRegion, rectRegion);

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
    } else    if (m_refData->GetRefCount() > 1) {
        wxRegionRefData* ref = (wxRegionRefData*)m_refData;
        UnRef();
        m_refData = new wxRegionRefData(*ref);
    }

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
void wxRegion::GetBox(wxCoord& x, wxCoord& y, wxCoord&w, wxCoord &h) const
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
    wxCoord x, y, w, h;
    GetBox(x, y, w, h);
    return wxRect(x, y, w, h);
}

// Is region empty?
bool wxRegion::Empty() const
{
    return m_refData ? XEmptyRegion(M_REGION) : TRUE;
}

//-----------------------------------------------------------------------------
//# Tests
//-----------------------------------------------------------------------------

// Does the region contain the point (x,y)?
wxRegionContain wxRegion::Contains(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y)) const
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
wxRegionContain wxRegion::Contains(wxCoord x, wxCoord y, wxCoord w, wxCoord h) const
{
    if (!m_refData)
        return wxOutRegion;

    switch (XRectInRegion(M_REGION, x, y, w, h)) {
        case RectangleIn:    return wxInRegion;
        case RectanglePart: return wxPartRegion;
    }
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

bool wxRegion::UsingRects() const
{
    return ((wxRegionRefData*)m_refData)->UsingRects();
}

/*
wxRectList& wxRegion::GetRectList()
{
    return ((wxRegionRefData*)m_refData)->GetRectList();
}
*/

wxRect* wxRegion::GetRects()
{
    return ((wxRegionRefData*)m_refData)->GetRects();
}

int wxRegion::GetRectCount() const
{
    return ((wxRegionRefData*)m_refData)->GetRectCount();
}

void wxRegion::SetRects(const wxRectList& rectList)
{
    ((wxRegionRefData*)m_refData)->SetRects(rectList);
}

void wxRegion::SetRects(int count, const wxRect* rects)
{
    ((wxRegionRefData*)m_refData)->SetRects(count, rects);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                               wxRegionIterator                                 //
//                                                                             //
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
        // Create m_rects and fill with rectangles for this region.
        // Since we can't find the rectangles in a region, we cheat
        // by retrieving the rectangles explicitly set in wxPaintDC::wxPaintDC
        // (dcclient.cpp).
        if (m_region.UsingRects())
        {
            wxRect* rects = m_region.GetRects();
            int count = m_region.GetRectCount();
            m_numRects = count;
            m_rects = new wxRect[m_numRects];

            int i = 0;
            for (i = 0; i < m_numRects; i++)
               m_rects[i] = rects[i];

	    /*
            int i = 0;
            wxRectList::Node* node = rectList.GetFirst();
            while (node) {
                wxRect* rect = node->GetData();
                m_rects[i] = * rect;
                node = node->GetNext();
                i ++;
            }
	    */
        }
        else
        {
            // For now, fudge by getting the whole bounding box.
            m_rects = new wxRect[1];
            m_numRects = 1;
            m_rects[0] = m_region.GetBox();
        }
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

