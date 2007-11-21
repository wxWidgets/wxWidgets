/////////////////////////////////////////////////////////////////////////////
// File:      src/mac/carbon/region.cpp
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

class WXDLLEXPORT wxRegionRefData : public wxGDIRefData
{
public:
    wxRegionRefData()
    { m_macRgn = NewRgn(); }

    wxRegionRefData(const wxRegionRefData& data)
        : wxGDIRefData()
    {
        m_macRgn = NewRgn();
        CopyRgn( data.m_macRgn , m_macRgn );
    }

    virtual ~wxRegionRefData()
    { DisposeRgn( m_macRgn ); }

    RgnHandle    m_macRgn;
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
    SetRectRgn( (RgnHandle) M_REGION , x , y , x + w , y + h ) ;
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( (RgnHandle) M_REGION , topLeft.x , topLeft.y , bottomRight.x , bottomRight.y ) ;
}

wxRegion::wxRegion(const wxRect& rect)
{
    m_refData = new wxRegionRefData;
    SetRectRgn( (RgnHandle) M_REGION , rect.x , rect.y , rect.x + rect.width , rect.y + rect.height ) ;
}

wxRegion::wxRegion(size_t n, const wxPoint *points, int WXUNUSED(fillStyle))
{
    m_refData = new wxRegionRefData;

#ifndef __LP64__
    // TODO : any APIs ?
    // OS X somehow does not collect the region invisibly as before, so sometimes things
    // get drawn on screen instead of just being combined into a region, therefore we allocate a temp gworld now

    GWorldPtr gWorld = NULL;
    GWorldPtr oldWorld;
    GDHandle oldGDHandle;
    OSStatus err;
    Rect destRect = { 0, 0, 1, 1 };

    ::GetGWorld( &oldWorld, &oldGDHandle );
    err = ::NewGWorld( &gWorld, 32, &destRect, NULL, NULL, 0 );
    if ( err == noErr )
    {
        ::SetGWorld( gWorld, GetGDevice() );

        OpenRgn();

        wxCoord x1, x2 , y1 , y2 ;
        x2 = x1 = points[0].x ;
        y2 = y1 = points[0].y ;

        ::MoveTo( x1, y1 );
        for (size_t i = 1; i < n; i++)
        {
            x2 = points[i].x ;
            y2 = points[i].y ;
            ::LineTo( x2, y2 );
        }

        // close the polyline if necessary
        if ( x1 != x2 || y1 != y2 )
            ::LineTo( x1, y1 ) ;

        CloseRgn( M_REGION ) ;

        ::SetGWorld( oldWorld, oldGDHandle );
    }
#endif
}

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

// Move the region
bool wxRegion::DoOffset(wxCoord x, wxCoord y)
{
    wxCHECK_MSG( M_REGION, false, _T("invalid wxRegion") );

    if ( !x && !y )
        // nothing to do
        return true;

    OffsetRgn( M_REGION , x , y ) ;

    return true ;
}


//! Union /e region with this.
bool wxRegion::DoCombine(const wxRegion& region, wxRegionOp op)
{
    wxCHECK_MSG( region.Ok(), false, _T("invalid wxRegion") );

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
            CopyRgn( OTHER_M_REGION(region) , M_REGION ) ;
            break ;
    }

    return true;
}

//-----------------------------------------------------------------------------
//# Information on region
//-----------------------------------------------------------------------------

bool wxRegion::DoIsEqual(const wxRegion& region) const
{
    wxFAIL_MSG( _T("not implemented") );

    return false;
}

// Outer bounds of region
bool wxRegion::DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    if (m_refData)
    {
        Rect box ;
        GetRegionBounds( M_REGION , &box ) ;
        x = box.left ;
        y = box.top ;
        w = box.right - box.left ;
        h = box.bottom - box.top ;

        return true;
    }
    else
    {
        x = y = w = h = 0;

        return false;
    }
}

// Is region empty?
bool wxRegion::IsEmpty() const
{
    if ( m_refData )
        return EmptyRgn( M_REGION ) ;
    else
        return true ;
}

const WXHRGN wxRegion::GetWXHRGN() const
{
    return M_REGION ;
}

//-----------------------------------------------------------------------------
//# Tests
//-----------------------------------------------------------------------------

// Does the region contain the point?
wxRegionContain wxRegion::DoContainsPoint(wxCoord x, wxCoord y) const
{
    if (!m_refData)
        return wxOutRegion;

    Point p = { y , x } ;
    if (PtInRgn( p , M_REGION ) )
        return wxInRegion;

    return wxOutRegion;
}

// Does the region contain the rectangle (x, y, w, h)?
wxRegionContain wxRegion::DoContainsRect(const wxRect& r) const
{
    if (!m_refData)
        return wxOutRegion;

    Rect rect = { r.y , r.x , r.y + r.height , r.x + r.width } ;
    if (RectInRgn( &rect , M_REGION ) )
        return wxInRegion;
    else
        return wxOutRegion;
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
    if (m_rects)
    {
        delete [] m_rects;
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
    if (m_rects)
    {
        delete [] m_rects;
        m_rects = NULL;
    }

    if (rects && (numRects > 0))
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

OSStatus wxMacRegionToRectsCounterCallback(
    UInt16 message, RgnHandle region, const Rect *rect, void *data )
{
    long *m_numRects = (long*) data ;
    if ( message == kQDRegionToRectsMsgInit )
    {
        (*m_numRects) = 0 ;
    }
    else if (message == kQDRegionToRectsMsgParse)
    {
        (*m_numRects) += 1 ;
    }

    return noErr;
}

class RegionToRectsCallbackData
{
public :
    wxRect* m_rects ;
    long m_current ;
};

OSStatus wxMacRegionToRectsSetterCallback(
    UInt16 message, RgnHandle region, const Rect *rect, void *data )
{
    if (message == kQDRegionToRectsMsgParse)
    {
        RegionToRectsCallbackData *cb = (RegionToRectsCallbackData*) data ;
        cb->m_rects[cb->m_current++] = wxRect( rect->left , rect->top , rect->right - rect->left , rect->bottom - rect->top ) ;
    }

    return noErr;
}

void wxRegionIterator::Reset(const wxRegion& region)
{
    m_current = 0;
    m_region = region;

    if (m_rects)
    {
        delete [] m_rects;
        m_rects = NULL;
    }

    if (m_region.IsEmpty())
    {
        m_numRects = 0;
    }
    else
    {
        RegionToRectsUPP proc = 
#ifdef __MACH__
        (RegionToRectsUPP) wxMacRegionToRectsCounterCallback;
#else
        NewRegionToRectsUPP( wxMacRegionToRectsCounterCallback );
#endif

        OSStatus err = noErr;
        err = QDRegionToRects (OTHER_M_REGION( region ) , kQDParseRegionFromTopLeft, proc, (void*)&m_numRects);
        if (err == noErr)
        {
#ifndef __MACH__
            DisposeRegionToRectsUPP (proc);
#endif
            proc = 
#ifdef __MACH__
            (RegionToRectsUPP) wxMacRegionToRectsSetterCallback;
#else
            NewRegionToRectsUPP (wxMacRegionToRectsSetterCallback);
#endif
            m_rects = new wxRect[m_numRects];
            RegionToRectsCallbackData data ;
            data.m_rects = m_rects ;
            data.m_current = 0 ;
            QDRegionToRects( OTHER_M_REGION( region ) , kQDParseRegionFromTopLeft, proc, (void*)&data );
        }
        else
        {
            m_numRects = 0;
        }

#ifndef __MACH__
        DisposeRegionToRectsUPP( proc );
#endif
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
