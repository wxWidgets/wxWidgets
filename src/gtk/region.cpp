/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/region.cpp
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

#ifdef __GNUG__
    #pragma implementation "region.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/region.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

// Unfortunately the new way of implementing the region iterator
// doesn't work with GTK+ 2.0 or above (can't access a Region in
// GdkPrivateRegion)
#ifdef __WXGTK20__
#define OLDCODE 1
#else
#define OLDCODE 0
#endif

#include "wx/log.h"

// ----------------------------------------------------------------------------
// wxRegionRefData: private class containing the information about the region
// ----------------------------------------------------------------------------

class wxRegionRefData : public wxObjectRefData
{
public:
    wxRegionRefData();
    wxRegionRefData(const wxRegionRefData& refData);
    virtual ~wxRegionRefData();

    GdkRegion  *m_region;
#if OLDCODE
    wxList      m_rects;
#endif
};

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)
#define M_REGIONDATA_OF(rgn) ((wxRegionRefData *)(rgn.m_refData))

IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject);
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRegionRefData
// ----------------------------------------------------------------------------

wxRegionRefData::wxRegionRefData()
{
    m_region = (GdkRegion *) NULL;
}

wxRegionRefData::wxRegionRefData(const wxRegionRefData& refData)
{
#ifdef __WXGTK20__
    m_region = gdk_region_copy(refData.m_region);
#else
    m_region = gdk_region_new();
    GdkRegion *regCopy = gdk_regions_union(m_region, refData.m_region);
    gdk_region_destroy(m_region);
    m_region = regCopy;
#endif

#if OLDCODE
    wxNode *node = refData.m_rects.First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        m_rects.Append( (wxObject*) new wxRect(*r) );
        node = node->Next();
    }
#endif
}

wxRegionRefData::~wxRegionRefData()
{
    if (m_region)
        gdk_region_destroy( m_region );

#if OLDCODE
    wxNode *node = m_rects.First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        delete r;
        node = node->Next();
    }
#endif
}

// ----------------------------------------------------------------------------
// wxRegion construction
// ----------------------------------------------------------------------------

#define M_REGIONDATA ((wxRegionRefData *)m_refData)

wxRegion::wxRegion()
{
}

void wxRegion::InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData();
    GdkRegion *reg = gdk_region_new();
    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
#ifdef __WXGTK20__
    gdk_region_union_with_rect( reg, &rect );
    M_REGIONDATA->m_region = reg;
#else
    M_REGIONDATA->m_region = gdk_region_union_with_rect( reg, &rect );
    gdk_region_destroy( reg );
#endif

#if OLDCODE
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(x, y, w, h) );
#endif
}

wxRegion::wxRegion( size_t n, const wxPoint *points, int fillStyle )
{
    GdkPoint *gdkpoints = new GdkPoint[n];
    for ( size_t i = 0 ; i < n ; i++ )
    {
        gdkpoints[i].x = points[i].x;
        gdkpoints[i].y = points[i].y;
    }

    m_refData = new wxRegionRefData();

    GdkRegion* reg = gdk_region_polygon
                     (
                        gdkpoints,
                        n,
                        fillStyle == wxWINDING_RULE ? GDK_WINDING_RULE
                                                    : GDK_EVEN_ODD_RULE
                     );

    M_REGIONDATA->m_region = reg;

    delete [] gdkpoints;
}

wxRegion::~wxRegion()
{
}

// ----------------------------------------------------------------------------
// wxRegion comparison
// ----------------------------------------------------------------------------

bool wxRegion::operator==( const wxRegion& region )
{
    // compare the regions themselves, not the pointers to ref data!
    return gdk_region_equal(M_REGIONDATA->m_region,
                            M_REGIONDATA_OF(region)->m_region);
}

bool wxRegion::operator != ( const wxRegion& region )
{
    return !(*this == region);
}

// ----------------------------------------------------------------------------
// wxRegion operations
// ----------------------------------------------------------------------------

void wxRegion::Clear()
{
    UnRef();
}

bool wxRegion::Union( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        GdkRegion *reg = gdk_region_new();
#ifdef __WXGTK20__
        gdk_region_union_with_rect( reg, &rect );
        M_REGIONDATA->m_region = reg;
#else
        M_REGIONDATA->m_region = gdk_region_union_with_rect( reg, &rect );
        gdk_region_destroy( reg );
#endif
    }
    else
    {
        AllocExclusive();

#ifdef __WXGTK20__
        gdk_region_union_with_rect( M_REGIONDATA->m_region, &rect );
#else
        GdkRegion *reg = gdk_region_union_with_rect( M_REGIONDATA->m_region, &rect );
        gdk_region_destroy( M_REGIONDATA->m_region );
        M_REGIONDATA->m_region = reg;
#endif
    }

#if OLDCODE
    M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(x,y,width,height) );
#endif

    return TRUE;
}

bool wxRegion::Union( const wxRect& rect )
{
    return Union( rect.x, rect.y, rect.width, rect.height );
}

bool wxRegion::Union( const wxRegion& region )
{
    if (region.IsNull())
        return FALSE;

    AllocExclusive();

#ifdef __WXGTK20__
    gdk_region_union( M_REGIONDATA->m_region, region.GetRegion() );
#else
    GdkRegion *reg = gdk_regions_union( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
#endif

#if OLDCODE
    wxNode *node = region.GetRectList()->First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(r->x,r->y,r->width,r->height) );
        node = node->Next();
    }
#endif

    return TRUE;
}

bool wxRegion::Intersect( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );

    return Intersect( reg );
}

bool wxRegion::Intersect( const wxRect& rect )
{
    wxRegion reg( rect );
    return Intersect( reg );
}

// this helper function just computes the region intersection without updating
// the list of rectangles each region maintaints: this allows us to call it
// from Intersect() itself without going into infinite recursion as we would
// if we called Intersect() itself recursively
bool wxRegion::IntersectRegionOnly(const wxRegion& region)
{
    AllocExclusive();

#ifdef __WXGTK20__
    gdk_region_intersect( M_REGIONDATA->m_region, region.GetRegion() );
#else
    GdkRegion *reg = gdk_regions_intersect( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
#endif

    return TRUE;
}

bool wxRegion::Intersect( const wxRegion& region )
{
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = gdk_region_new();
        return TRUE;
    }

    if ( !IntersectRegionOnly(region) )
    {
        GetRectList()->Clear();

        return FALSE;
    }

    // we need to update the rect list as well
#if OLDCODE
    wxList& list = *GetRectList();
    wxNode *node = list.First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();

        wxRegion regCopy = region;
        if ( regCopy.IntersectRegionOnly(*r) )
        {
            // replace the node with the intersection
            *r = regCopy.GetBox();
        }
        else
        {
            // TODO remove the rect from the list
            r->width = 0;
            r->height = 0;
        }

        node = node->Next();
    }
#endif // OLDCODE

    return TRUE;
}

bool wxRegion::Subtract( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    return Subtract( reg );
}

bool wxRegion::Subtract( const wxRect& rect )
{
    wxRegion reg( rect );
    return Subtract( reg );
}

bool wxRegion::Subtract( const wxRegion& region )
{
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = gdk_region_new();
    }

    AllocExclusive();

#ifdef __WXGTK20__
    gdk_region_subtract( M_REGIONDATA->m_region, region.GetRegion() );
#else
    GdkRegion *reg = gdk_regions_subtract( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
#endif

    return TRUE;
}

bool wxRegion::Xor( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    return Xor( reg );
}

bool wxRegion::Xor( const wxRect& rect )
{
    wxRegion reg( rect );
    return Xor( reg );
}

bool wxRegion::Xor( const wxRegion& region )
{
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = gdk_region_new();
    }
    else
    {
        AllocExclusive();
    }

#ifdef __WXGTK20__
    gdk_region_xor( M_REGIONDATA->m_region, region.GetRegion() );
#else
    GdkRegion *reg = gdk_regions_xor( M_REGIONDATA->m_region, region.GetRegion() );
    gdk_region_destroy( M_REGIONDATA->m_region );
    M_REGIONDATA->m_region = reg;
#endif

#if OLDCODE
    wxNode *node = region.GetRectList()->First();
    while (node)
    {
        wxRect *r = (wxRect*)node->Data();
        M_REGIONDATA->m_rects.Append( (wxObject*) new wxRect(r->x,r->y,r->width,r->height) );
        node = node->Next();
    }
#endif // OLDCODE

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRegion tests
// ----------------------------------------------------------------------------

void wxRegion::GetBox( wxCoord &x, wxCoord &y, wxCoord &w, wxCoord &h ) const
{
    if ( m_refData )
    {
        GdkRectangle rect;
        gdk_region_get_clipbox( M_REGIONDATA->m_region, &rect );
        x = rect.x;
        y = rect.y;
        w = rect.width;
        h = rect.height;
    }
    else
    {
        x = 0;
        y = 0;
        w = -1;
        h = -1;
    }
}

wxRect wxRegion::GetBox() const
{
    wxCoord x, y, w, h;
    GetBox( x, y, w, h );
    return wxRect( x, y, w, h );
}

bool wxRegion::Offset( wxCoord x, wxCoord y )
{
    if (!m_refData)
        return FALSE;

    AllocExclusive();

    gdk_region_offset( M_REGIONDATA->m_region, x, y );

    return TRUE;
}

bool wxRegion::Empty() const
{
    if (!m_refData)
        return TRUE;

    return gdk_region_empty( M_REGIONDATA->m_region );
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y ) const
{
    if (!m_refData)
        return wxOutRegion;

    if (gdk_region_point_in( M_REGIONDATA->m_region, x, y ))
        return wxInRegion;
    else
        return wxOutRegion;
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y, wxCoord w, wxCoord h ) const
{
    if (!m_refData)
        return wxOutRegion;

    GdkRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;
    GdkOverlapType res = gdk_region_rect_in( M_REGIONDATA->m_region, &rect );
    switch (res)
    {
        case GDK_OVERLAP_RECTANGLE_IN:   return wxInRegion;
        case GDK_OVERLAP_RECTANGLE_OUT:  return wxOutRegion;
        case GDK_OVERLAP_RECTANGLE_PART: return wxPartRegion;
    }
    return wxOutRegion;
}

wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    return Contains( pt.x, pt.y );
}

wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    return Contains( rect.x, rect.y, rect.width, rect.height );
}

GdkRegion *wxRegion::GetRegion() const
{
    if (!m_refData)
        return (GdkRegion*) NULL;

    return M_REGIONDATA->m_region;
}

wxList *wxRegion::GetRectList() const
{
#if OLDCODE
    if (!m_refData)
        return (wxList*) NULL;

    return &(M_REGIONDATA->m_rects);
#else
    return (wxList*) NULL;
#endif
}

// ----------------------------------------------------------------------------
// wxRegionIterator
// ----------------------------------------------------------------------------

#if OLDCODE

wxRegionIterator::wxRegionIterator()
{
    Reset();
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    Reset(region);
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    Reset();
}

wxRegionIterator::operator bool () const
{
    return m_region.GetRectList() && m_current < (size_t)m_region.GetRectList()->Number();
}

bool wxRegionIterator::HaveRects() const
{
    return m_region.GetRectList() && m_current < (size_t)m_region.GetRectList()->Number();
}

void wxRegionIterator::operator ++ ()
{
    if (HaveRects()) ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    if (HaveRects()) ++m_current;
}

wxCoord wxRegionIterator::GetX() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->x;
}

wxCoord wxRegionIterator::GetY() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->y;
}

wxCoord wxRegionIterator::GetW() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->width;
}

wxCoord wxRegionIterator::GetH() const
{
    wxNode *node = m_region.GetRectList()->Nth( m_current );
    if (!node) return 0;
    wxRect *r = (wxRect*)node->Data();
    return r->height;
}

#else // !OLDCODE

// the following structures must match the private structures
// in X11 region code ( xc/lib/X11/region.h )

// this makes the Region type transparent
// and we have access to the region rectangles

struct _XBox {
    short x1, x2, y1, y2;
};

struct _XRegion {
    long   size , numRects;
    _XBox *rects, extents;
};

class wxRIRefData: public wxObjectRefData
{
public:

    wxRIRefData() : m_rects(0), m_numRects(0){}
   ~wxRIRefData();

    wxRect *m_rects;
    size_t  m_numRects;

    void CreateRects( const wxRegion& r );
};

wxRIRefData::~wxRIRefData()
{
    delete m_rects;
}

#include <gdk/gdkprivate.h>

void wxRIRefData::CreateRects( const wxRegion& region )
{
  if( m_rects )
    delete m_rects;
  m_rects   = 0;
  m_numRects= 0;
  GdkRegion *gdkregion= region.GetRegion();
  if( gdkregion ){
    Region r= ((GdkRegionPrivate *)gdkregion)->xregion;
    if( r ){
      m_numRects= r->numRects;
      if( m_numRects )
      {
        m_rects= new wxRect[m_numRects];
        for( size_t i=0; i<m_numRects; ++i )
        {
          _XBox &xr= r->rects[i];
          wxRect&wr= m_rects[i];
          wr.x     = xr.x1;
          wr.y     = xr.y1;
          wr.width = xr.x2-xr.x1;
          wr.height= xr.y2-xr.y1;
        }
      }
    }
  }
}

wxRegionIterator::wxRegionIterator()
{
    m_refData = new wxRIRefData();
    Reset();
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    m_refData = new wxRIRefData();
    Reset(region);
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    ((wxRIRefData*)m_refData)->CreateRects(region);
    Reset();
}

bool wxRegionIterator::HaveRects() const
{
    return m_current < ((wxRIRefData*)m_refData)->m_numRects;
}

wxRegionIterator::operator bool () const
{
    return HaveRects();
}

void wxRegionIterator::operator ++ ()
{
    if (HaveRects()) ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    if (HaveRects()) ++m_current;
}

wxCoord wxRegionIterator::GetX() const
{
    if( !HaveRects() ) return 0;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].x;
}

wxCoord wxRegionIterator::GetY() const
{
    if( !HaveRects() ) return 0;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].y;
}

wxCoord wxRegionIterator::GetW() const
{
    if( !HaveRects() ) return -1;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].width;
}

wxCoord wxRegionIterator::GetH() const
{
    if( !HaveRects() ) return -1;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].height;
}

wxRect wxRegionIterator::GetRect() const
{
    wxRect r;
    if( HaveRects() )
        r = ((wxRIRefData*)m_refData)->m_rects[m_current];

    return r;
}

#endif // OLDCODE/!OLDCODE

