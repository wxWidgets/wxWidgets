/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/region.cpp
// Purpose:
// Author:      Robert Roebling
// Modified:    VZ at 05.10.00: use AllocExclusive(), comparison fixed
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/gtk/private.h"


// ----------------------------------------------------------------------------
// wxRegionRefData: private class containing the information about the region
// ----------------------------------------------------------------------------

class wxRegionRefData : public wxGDIRefData
{
public:
    wxRegionRefData()
    {
        m_region = NULL;
    }

    wxRegionRefData(const wxRegionRefData& refData)
        : wxGDIRefData()
    {
#ifdef __WXGTK30__
        m_region = cairo_region_copy(refData.m_region);
#else
        m_region = gdk_region_copy(refData.m_region);
#endif
    }

    virtual ~wxRegionRefData()
    {
        if (m_region) {
#ifdef __WXGTK30__
            cairo_region_destroy( m_region );
#else
            gdk_region_destroy( m_region );
#endif
        }
    }

#ifdef __WXGTK30__
    cairo_region_t  *m_region;
#else
    GdkRegion  *m_region;
#endif
};

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)
#define M_REGIONDATA_OF(rgn) ((wxRegionRefData *)(rgn.m_refData))

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject)

// ----------------------------------------------------------------------------
// wxRegion construction
// ----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)

void wxRegion::InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    GdkRectangle rect; // In gtk+3, GdkRectangle is identical to cairo_rectangle_int_t
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;

    m_refData = new wxRegionRefData();

#ifdef __WXGTK30__
    M_REGIONDATA->m_region = cairo_region_create_rectangle( &rect );
#else
    M_REGIONDATA->m_region = gdk_region_rectangle( &rect );
#endif
}

#ifdef __WXGTK30__
wxRegion::wxRegion( cairo_region_t *region )
#else
wxRegion::wxRegion( GdkRegion *region )
#endif
{
    m_refData = new wxRegionRefData();
#ifdef __WXGTK30__
    M_REGIONDATA->m_region = cairo_region_copy( region );
#else
    M_REGIONDATA->m_region = gdk_region_copy( region );
#endif
}

wxRegion::wxRegion( size_t n, const wxPoint *points,
                    wxPolygonFillMode fillStyle )
{
    GdkPoint *gdkpoints = new GdkPoint[n];
    for ( size_t i = 0 ; i < n ; i++ )
    {
        gdkpoints[i].x = points[i].x;
        gdkpoints[i].y = points[i].y;
    }

    m_refData = new wxRegionRefData();


#ifdef __WXGTK30__
    wxFAIL_MSG("This method is not implemented in wxGTK with gtk+3.0");
#else
    GdkRegion* reg = gdk_region_polygon
                     (
                        gdkpoints,
                        n,
                        fillStyle == wxWINDING_RULE ? GDK_WINDING_RULE
                                                    : GDK_EVEN_ODD_RULE
                     );

    M_REGIONDATA->m_region = reg;
#endif

    delete [] gdkpoints;
}

wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

wxGDIRefData *wxRegion::CreateGDIRefData() const
{
    return new wxRegionRefData;
}

wxGDIRefData *wxRegion::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxRegionRefData(*(wxRegionRefData *)data);
}

// ----------------------------------------------------------------------------
// wxRegion comparison
// ----------------------------------------------------------------------------

bool wxRegion::DoIsEqual(const wxRegion& region) const
{
#ifdef __WXGTK30__
    return cairo_region_equal(M_REGIONDATA->m_region,
                            M_REGIONDATA_OF(region)->m_region);
#else
    return gdk_region_equal(M_REGIONDATA->m_region,
                            M_REGIONDATA_OF(region)->m_region);
#endif
}

// ----------------------------------------------------------------------------
// wxRegion operations
// ----------------------------------------------------------------------------

void wxRegion::Clear()
{
    UnRef();
}

bool wxRegion::DoUnionWithRect(const wxRect& r)
{
    // workaround for a strange GTK/X11 bug: taking union with an empty
    // rectangle results in an empty region which is definitely not what we
    // want
    if ( r.IsEmpty() )
        return true;

    if ( !m_refData )
    {
        InitRect(r.x, r.y, r.width, r.height);
    }
    else
    {
        AllocExclusive();

        GdkRectangle rect;
        rect.x = r.x;
        rect.y = r.y;
        rect.width = r.width;
        rect.height = r.height;

#ifdef __WXGTK30__
        cairo_region_union_rectangle( M_REGIONDATA->m_region, &rect );
#else
        gdk_region_union_with_rect( M_REGIONDATA->m_region, &rect );
#endif
    }

    return true;
}

bool wxRegion::DoUnionWithRegion( const wxRegion& region )
{
    wxCHECK_MSG( region.IsOk(), false, wxT("invalid region") );

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
#ifdef __WXGTK30__
        M_REGIONDATA->m_region = cairo_region_create();
#else
        M_REGIONDATA->m_region = gdk_region_new();
#endif
    }
    else
    {
        AllocExclusive();
    }

#ifdef __WXGTK30__
    cairo_region_union( M_REGIONDATA->m_region, region.GetRegion() );
#else
    gdk_region_union( M_REGIONDATA->m_region, region.GetRegion() );
#endif

    return true;
}

bool wxRegion::DoIntersect( const wxRegion& region )
{
    wxCHECK_MSG( region.IsOk(), false, wxT("invalid region") );

    if (!m_refData)
    {
        // intersecting with invalid region doesn't make sense
        return false;
    }

    AllocExclusive();

#ifdef __WXGTK30__
    cairo_region_intersect( M_REGIONDATA->m_region, region.GetRegion() );
#else
    gdk_region_intersect( M_REGIONDATA->m_region, region.GetRegion() );
#endif

    return true;
}

bool wxRegion::DoSubtract( const wxRegion& region )
{
    wxCHECK_MSG( region.IsOk(), false, wxT("invalid region") );

    if (!m_refData)
    {
        // subtracting from an invalid region doesn't make sense
        return false;
    }

    AllocExclusive();

#ifdef __WXGTK30__
    cairo_region_subtract( M_REGIONDATA->m_region, region.GetRegion() );
#else
    gdk_region_subtract( M_REGIONDATA->m_region, region.GetRegion() );
#endif

    return true;
}

bool wxRegion::DoXor( const wxRegion& region )
{
    wxCHECK_MSG( region.IsOk(), false, wxT("invalid region") );

    if (!m_refData)
    {
        return false;
    }

    AllocExclusive();

#ifdef __WXGTK30__
    cairo_region_xor( M_REGIONDATA->m_region, region.GetRegion() );
#else
    gdk_region_xor( M_REGIONDATA->m_region, region.GetRegion() );
#endif

    return true;
}

bool wxRegion::DoOffset( wxCoord x, wxCoord y )
{
    if (!m_refData)
        return false;

    AllocExclusive();

#ifdef __WXGTK30__
    cairo_region_translate( M_REGIONDATA->m_region, x, y );
#else
    gdk_region_offset( M_REGIONDATA->m_region, x, y );
#endif

    return true;
}

// ----------------------------------------------------------------------------
// wxRegion tests
// ----------------------------------------------------------------------------

bool wxRegion::DoGetBox( wxCoord &x, wxCoord &y, wxCoord &w, wxCoord &h ) const
{
    if ( m_refData )
    {
        GdkRectangle rect;
#ifdef __WXGTK30__
        cairo_region_get_extents( M_REGIONDATA->m_region, &rect );
#else
        gdk_region_get_clipbox( M_REGIONDATA->m_region, &rect );
#endif
        x = rect.x;
        y = rect.y;
        w = rect.width;
        h = rect.height;

        return true;
    }
    else
    {
        x = 0;
        y = 0;
        w = -1;
        h = -1;

        return false;
    }
}

bool wxRegion::IsEmpty() const
{
    if (!m_refData)
        return true;

#ifdef __WXGTK30__
    return cairo_region_is_empty( M_REGIONDATA->m_region );
#else
    return gdk_region_empty( M_REGIONDATA->m_region );
#endif
}

wxRegionContain wxRegion::DoContainsPoint( wxCoord x, wxCoord y ) const
{
    if (!m_refData)
        return wxOutRegion;

#ifdef __WXGTK30__
    if (cairo_region_contains_point( M_REGIONDATA->m_region, x, y ))
#else
    if (gdk_region_point_in( M_REGIONDATA->m_region, x, y ))
#endif
        return wxInRegion;
    else
        return wxOutRegion;
}

wxRegionContain wxRegion::DoContainsRect(const wxRect& r) const
{
    if (!m_refData)
        return wxOutRegion;

    GdkRectangle rect;
    rect.x = r.x;
    rect.y = r.y;
    rect.width = r.width;
    rect.height = r.height;
#ifdef __WXGTK30__
    cairo_region_overlap_t res = cairo_region_contains_rectangle( M_REGIONDATA->m_region, &rect );
    switch (res)
    {
        case CAIRO_REGION_OVERLAP_IN:   return wxInRegion;
        case CAIRO_REGION_OVERLAP_OUT:  return wxOutRegion;
        case CAIRO_REGION_OVERLAP_PART: return wxPartRegion;
    }
#else
    GdkOverlapType res = gdk_region_rect_in( M_REGIONDATA->m_region, &rect );
    switch (res)
    {
        case GDK_OVERLAP_RECTANGLE_IN:   return wxInRegion;
        case GDK_OVERLAP_RECTANGLE_OUT:  return wxOutRegion;
        case GDK_OVERLAP_RECTANGLE_PART: return wxPartRegion;
    }
#endif
    return wxOutRegion;
}

#ifdef __WXGTK30__
cairo_region_t *wxRegion::GetRegion() const
#else
GdkRegion *wxRegion::GetRegion() const
#endif
{
    if (!m_refData)
        return NULL;

    return M_REGIONDATA->m_region;
}

// ----------------------------------------------------------------------------
// wxRegionIterator
// ----------------------------------------------------------------------------

wxRegionIterator::wxRegionIterator()
{
    Init();
    Reset();
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    Init();
    Reset(region);
}

void wxRegionIterator::Init()
{
    m_rects = NULL;
    m_numRects = 0;
}

wxRegionIterator::~wxRegionIterator()
{
    wxDELETEA(m_rects);
}

void wxRegionIterator::CreateRects( const wxRegion& region )
{
    wxDELETEA(m_rects);
    m_numRects = 0;

    GdkRectangle *gdkrects = NULL;
    gint numRects = 0;

#ifdef __WXGTK30__
    cairo_region_t *cairoRegion = region.GetRegion();
    if (!cairoRegion)
        return;
    m_numRects = cairo_region_num_rectangles( cairoRegion );
     
    if (m_numRects)
    {
        m_rects = new wxRect[m_numRects];
        for (size_t i=0; i < m_numRects; ++i)
        {
            GdkRectangle gr;
            cairo_region_get_rectangle( cairoRegion, i, &gr );
            wxRect &wr = m_rects[i];
            wr.x = gr.x;
            wr.y = gr.y;
            wr.width = gr.width;
            wr.height = gr.height;
        }
    }
#else
    GdkRegion *gdkregion = region.GetRegion();
    if (!gdkregion)
        return;
    gdk_region_get_rectangles( gdkregion, &gdkrects, &numRects );
    m_numRects = numRects;
    if (numRects)
    {
        m_rects = new wxRect[m_numRects];
        for (size_t i=0; i < m_numRects; ++i)
        {
            GdkRectangle &gr = gdkrects[i];
            wxRect &wr = m_rects[i];
            wr.x = gr.x;
            wr.y = gr.y;
            wr.width = gr.width;
            wr.height = gr.height;
        }
    }
    g_free( gdkrects );
#endif
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    CreateRects(region);
    Reset();
}

bool wxRegionIterator::HaveRects() const
{
    return m_current < m_numRects;
}

wxRegionIterator& wxRegionIterator::operator ++ ()
{
    if (HaveRects())
        ++m_current;

    return *this;
}

wxRegionIterator wxRegionIterator::operator ++ (int)
{
    wxRegionIterator tmp = *this;

    if (HaveRects())
        ++m_current;

    return tmp;
}

wxCoord wxRegionIterator::GetX() const
{
    wxCHECK_MSG( HaveRects(), 0, wxT("invalid wxRegionIterator") );

    return m_rects[m_current].x;
}

wxCoord wxRegionIterator::GetY() const
{
    wxCHECK_MSG( HaveRects(), 0, wxT("invalid wxRegionIterator") );

    return m_rects[m_current].y;
}

wxCoord wxRegionIterator::GetW() const
{
    wxCHECK_MSG( HaveRects(), 0, wxT("invalid wxRegionIterator") );

    return m_rects[m_current].width;
}

wxCoord wxRegionIterator::GetH() const
{
    wxCHECK_MSG( HaveRects(), 0, wxT("invalid wxRegionIterator") );

    return m_rects[m_current].height;
}

wxRect wxRegionIterator::GetRect() const
{
    wxRect r;
    if( HaveRects() )
        r = m_rects[m_current];

    return r;
}

wxRegionIterator& wxRegionIterator::operator=(const wxRegionIterator& ri)
{
    wxDELETEA(m_rects);

    m_current = ri.m_current;
    m_numRects = ri.m_numRects;
    if ( m_numRects )
    {
        m_rects = new wxRect[m_numRects];
        for ( unsigned int n = 0; n < m_numRects; n++ )
            m_rects[n] = ri.m_rects[n];
    }
    else
    {
        m_rects = NULL;
    }

    return *this;
}
