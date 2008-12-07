/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Chris Breeze
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XCopyPlane XCOPYPLANE
#endif

#include "wx/gtk/dcclient.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/math.h"
    #include "wx/image.h"
    #include "wx/module.h"
#endif

#include "wx/fontutil.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/object.h"

#include <gdk/gdkx.h>

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define XLOG2DEV(x)    LogicalToDeviceX(x)
#define XLOG2DEVREL(x) LogicalToDeviceXRel(x)
#define YLOG2DEV(y)    LogicalToDeviceY(y)
#define YLOG2DEVREL(y) LogicalToDeviceYRel(y)

#define USE_PAINT_REGION 1

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"

static GdkPixmap* hatches[wxBRUSHSTYLE_LAST_HATCH - wxBRUSHSTYLE_FIRST_HATCH + 1];

extern GtkWidget *wxGetRootWindow();

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

static const double RAD2DEG  = 180.0 / M_PI;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

static GdkPixmap* GetHatch(int style)
{
    wxASSERT(style >= wxBRUSHSTYLE_FIRST_HATCH && style <= wxBRUSHSTYLE_LAST_HATCH);
    const int i = style - wxBRUSHSTYLE_FIRST_HATCH;
    if (hatches[i] == NULL)
    {
        switch (style)
        {
        case wxBRUSHSTYLE_BDIAGONAL_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, bdiag_bits, bdiag_width, bdiag_height);
            break;
        case wxBRUSHSTYLE_CROSSDIAG_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, cdiag_bits, cdiag_width, cdiag_height);
            break;
        case wxBRUSHSTYLE_CROSS_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, cross_bits, cross_width, cross_height);
            break;
        case wxBRUSHSTYLE_FDIAGONAL_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, fdiag_bits, fdiag_width, fdiag_height);
            break;
        case wxBRUSHSTYLE_HORIZONTAL_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, horiz_bits, horiz_width, horiz_height);
            break;
        case wxBRUSHSTYLE_VERTICAL_HATCH:
            hatches[i] = gdk_bitmap_create_from_data(NULL, verti_bits, verti_width, verti_height);
            break;
        }
    }
    return hatches[i];
}

//-----------------------------------------------------------------------------
// temporary implementation of the missing GDK function
//-----------------------------------------------------------------------------

static
void gdk_wx_draw_bitmap(GdkDrawable  *drawable,
                        GdkGC        *gc,
                        GdkDrawable  *src,
                        gint         xsrc,
                        gint         ysrc)
{
    wxCHECK_RET( drawable, _T("NULL drawable in gdk_wx_draw_bitmap") );
    wxCHECK_RET( src, _T("NULL src in gdk_wx_draw_bitmap") );
    wxCHECK_RET( gc, _T("NULL gc in gdk_wx_draw_bitmap") );

    gint src_width, src_height;
    gdk_drawable_get_size(src, &src_width, &src_height);

    XCopyPlane( GDK_WINDOW_XDISPLAY(drawable),
                GDK_WINDOW_XID(src),
                GDK_WINDOW_XID(drawable),
                GDK_GC_XGC(gc),
                xsrc, ysrc,
                src_width, src_height,
                0, 0,
                1 );
}

//-----------------------------------------------------------------------------
// Implement Pool of Graphic contexts. Creating them takes too much time.
//-----------------------------------------------------------------------------

enum wxPoolGCType
{
   wxGC_ERROR = 0,
   wxTEXT_MONO,
   wxBG_MONO,
   wxPEN_MONO,
   wxBRUSH_MONO,
   wxTEXT_COLOUR,
   wxBG_COLOUR,
   wxPEN_COLOUR,
   wxBRUSH_COLOUR,
   wxTEXT_SCREEN,
   wxBG_SCREEN,
   wxPEN_SCREEN,
   wxBRUSH_SCREEN
};

struct wxGC
{
    GdkGC        *m_gc;
    wxPoolGCType  m_type;
    bool          m_used;
};

#define GC_POOL_ALLOC_SIZE 100

static int wxGCPoolSize = 0;

static wxGC *wxGCPool = NULL;

static void wxInitGCPool()
{
    // This really could wait until the first call to
    // wxGetPoolGC, but we will make the first allocation
    // now when other initialization is being performed.

    // Set initial pool size.
    wxGCPoolSize = GC_POOL_ALLOC_SIZE;

    // Allocate initial pool.
    wxGCPool = (wxGC *)malloc(wxGCPoolSize * sizeof(wxGC));
    if (wxGCPool == NULL)
    {
        // If we cannot malloc, then fail with error
        // when debug is enabled.  If debug is not enabled,
        // the problem will eventually get caught
        // in wxGetPoolGC.
        wxFAIL_MSG( wxT("Cannot allocate GC pool") );
        return;
    }

    // Zero initial pool.
    memset(wxGCPool, 0, wxGCPoolSize * sizeof(wxGC));
}

static void wxCleanUpGCPool()
{
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (wxGCPool[i].m_gc)
            g_object_unref (wxGCPool[i].m_gc);
    }

    free(wxGCPool);
    wxGCPool = NULL;
    wxGCPoolSize = 0;
}

static GdkGC* wxGetPoolGC( GdkWindow *window, wxPoolGCType type )
{
    wxGC *pptr;

    // Look for an available GC.
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (!wxGCPool[i].m_gc)
        {
            wxGCPool[i].m_gc = gdk_gc_new( window );
            gdk_gc_set_exposures( wxGCPool[i].m_gc, FALSE );
            wxGCPool[i].m_type = type;
            wxGCPool[i].m_used = false;
        }
        if ((!wxGCPool[i].m_used) && (wxGCPool[i].m_type == type))
        {
            wxGCPool[i].m_used = true;
            return wxGCPool[i].m_gc;
        }
    }

    // We did not find an available GC.
    // We need to grow the GC pool.
    pptr = (wxGC *)realloc(wxGCPool,
        (wxGCPoolSize + GC_POOL_ALLOC_SIZE)*sizeof(wxGC));
    if (pptr != NULL)
    {
        // Initialize newly allocated pool.
        wxGCPool = pptr;
        memset(&wxGCPool[wxGCPoolSize], 0,
            GC_POOL_ALLOC_SIZE*sizeof(wxGC));

        // Initialize entry we will return.
        wxGCPool[wxGCPoolSize].m_gc = gdk_gc_new( window );
        gdk_gc_set_exposures( wxGCPool[wxGCPoolSize].m_gc, FALSE );
        wxGCPool[wxGCPoolSize].m_type = type;
        wxGCPool[wxGCPoolSize].m_used = true;

        // Set new value of pool size.
        wxGCPoolSize += GC_POOL_ALLOC_SIZE;

        // Return newly allocated entry.
        return wxGCPool[wxGCPoolSize-GC_POOL_ALLOC_SIZE].m_gc;
    }

    // The realloc failed.  Fall through to error.
    wxFAIL_MSG( wxT("No GC available") );

    return (GdkGC*) NULL;
}

static void wxFreePoolGC( GdkGC *gc )
{
    for (int i = 0; i < wxGCPoolSize; i++)
    {
        if (wxGCPool[i].m_gc == gc)
        {
            wxGCPool[i].m_used = false;
            return;
        }
    }

    wxFAIL_MSG( wxT("Wrong GC") );
}

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxWindowDCImpl, wxGTKDCImpl)

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner ) :
   wxGTKDCImpl( owner )
{
    m_gdkwindow = (GdkWindow*) NULL;
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_isScreenDC = false;
    m_context = (PangoContext *)NULL;
    m_layout = (PangoLayout *)NULL;
    m_fontdesc = (PangoFontDescription *)NULL;
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *window ) :
   wxGTKDCImpl( owner )
{
    wxASSERT_MSG( window, wxT("DC needs a window") );

    m_gdkwindow = (GdkWindow*) NULL;
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_isScreenDC = false;
    m_font = window->GetFont();

    GtkWidget *widget = window->m_wxwindow;

    // Some controls don't have m_wxwindow - like wxStaticBox, but the user
    // code should still be able to create wxClientDCs for them, so we will
    // use the parent window here then.
    if ( !widget )
    {
        window = window->GetParent();
        widget = window->m_wxwindow;
    }

    wxASSERT_MSG( widget, wxT("DC needs a widget") );

    m_context = window->GtkGetPangoDefaultContext();
    m_layout = pango_layout_new( m_context );
    m_fontdesc = pango_font_description_copy( widget->style->font_desc );

    m_gdkwindow = widget->window;

    // Window not realized ?
    if (!m_gdkwindow)
    {
         // Don't report problems as per MSW.
         m_ok = true;

         return;
    }

    m_cmap = gtk_widget_get_colormap( widget ? widget : window->m_widget );

    SetUpDC();

    /* this must be done after SetUpDC, bacause SetUpDC calls the
       repective SetBrush, SetPen, SetBackground etc functions
       to set up the DC. SetBackground call m_owner->SetBackground
       and this might not be desired as the standard dc background
       is white whereas a window might assume gray to be the
       standard (as e.g. wxStatusBar) */

    m_window = window;

    if (m_window && m_window->m_wxwindow && 
        (m_window->GetLayoutDirection() == wxLayout_RightToLeft))
    {
        // reverse sense
        m_signX = -1;

        // origin in the upper right corner
        m_deviceOriginX = m_window->GetClientSize().x;
    }
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    Destroy();

    if (m_layout)
        g_object_unref (m_layout);
    if (m_fontdesc)
        pango_font_description_free( m_fontdesc );
}

void wxWindowDCImpl::SetUpDC( bool isMemDC )
{
    m_ok = true;

    wxASSERT_MSG( !m_penGC, wxT("GCs already created") );

    bool done = false;

    if ((isMemDC) && (GetSelectedBitmap().IsOk()))
    {
        if (GetSelectedBitmap().GetDepth() == 1)
        {
            m_penGC = wxGetPoolGC( m_gdkwindow, wxPEN_MONO );
            m_brushGC = wxGetPoolGC( m_gdkwindow, wxBRUSH_MONO );
            m_textGC = wxGetPoolGC( m_gdkwindow, wxTEXT_MONO );
            m_bgGC = wxGetPoolGC( m_gdkwindow, wxBG_MONO );
            done = true;
        }
    }

    if (!done)
    {    
        if (m_isScreenDC)
        {
            m_penGC = wxGetPoolGC( m_gdkwindow, wxPEN_SCREEN );
            m_brushGC = wxGetPoolGC( m_gdkwindow, wxBRUSH_SCREEN );
            m_textGC = wxGetPoolGC( m_gdkwindow, wxTEXT_SCREEN );
            m_bgGC = wxGetPoolGC( m_gdkwindow, wxBG_SCREEN );
        }
        else
        {
            m_penGC = wxGetPoolGC( m_gdkwindow, wxPEN_COLOUR );
            m_brushGC = wxGetPoolGC( m_gdkwindow, wxBRUSH_COLOUR );
            m_textGC = wxGetPoolGC( m_gdkwindow, wxTEXT_COLOUR );
            m_bgGC = wxGetPoolGC( m_gdkwindow, wxBG_COLOUR );
        }
    }

    /* background colour */
    m_backgroundBrush = *wxWHITE_BRUSH;
    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    const GdkColor *bg_col = m_backgroundBrush.GetColour().GetColor();

    /* m_textGC */
    m_textForegroundColour.CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );

    m_textBackgroundColour.CalcPixel( m_cmap );
    gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );

    gdk_gc_set_fill( m_textGC, GDK_SOLID );

    gdk_gc_set_colormap( m_textGC, m_cmap );

    /* m_penGC */
    m_pen.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_penGC, m_pen.GetColour().GetColor() );
    gdk_gc_set_background( m_penGC, bg_col );

    gdk_gc_set_line_attributes( m_penGC, 0, GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_ROUND );

    /* m_brushGC */
    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );
    gdk_gc_set_background( m_brushGC, bg_col );

    gdk_gc_set_fill( m_brushGC, GDK_SOLID );

    /* m_bgGC */
    gdk_gc_set_background( m_bgGC, bg_col );
    gdk_gc_set_foreground( m_bgGC, bg_col );

    gdk_gc_set_fill( m_bgGC, GDK_SOLID );

    /* ROPs */
    gdk_gc_set_function( m_textGC, GDK_COPY );
    gdk_gc_set_function( m_brushGC, GDK_COPY );
    gdk_gc_set_function( m_penGC, GDK_COPY );

    /* clipping */
    gdk_gc_set_clip_rectangle( m_penGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_brushGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_textGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_bgGC, (GdkRectangle *) NULL );
}

void wxWindowDCImpl::DoGetSize( int* width, int* height ) const
{
    wxCHECK_RET( m_window, _T("GetSize() doesn't work without window") );

    m_window->GetSize(width, height);
}

bool wxWindowDCImpl::DoFloodFill(wxCoord x, wxCoord y,
                             const wxColour& col, int style)
{
#if wxUSE_IMAGE
    extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                              const wxColour & col, int style);

    return wxDoFloodFill( GetOwner(), x, y, col, style);
#else
    wxUnusedVar(x);
    wxUnusedVar(y);
    wxUnusedVar(col);
    wxUnusedVar(style);

    return false;
#endif
}

bool wxWindowDCImpl::DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const
{
#if wxUSE_IMAGE
    // Generic (and therefore rather inefficient) method.
    // Could be improved.
    wxMemoryDC memdc;
    wxBitmap bitmap(1, 1);
    memdc.SelectObject(bitmap);
    memdc.Blit(0, 0, 1, 1, GetOwner(), x1, y1);
    memdc.SelectObject(wxNullBitmap);

    wxImage image = bitmap.ConvertToImage();
    col->Set(image.GetRed(0, 0), image.GetGreen(0, 0), image.GetBlue(0, 0));
    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(x1);
    wxUnusedVar(y1);
    wxUnusedVar(col);

    return false;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

void wxWindowDCImpl::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );
    
    if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
    {
        if (m_gdkwindow)
            gdk_draw_line( m_gdkwindow, m_penGC, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2) );

        CalcBoundingBox(x1, y1);
        CalcBoundingBox(x2, y2);
    }
}

void wxWindowDCImpl::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetOwner()->GetSize( &w, &h );
        wxCoord xx = XLOG2DEV(x);
        wxCoord yy = YLOG2DEV(y);
        if (m_gdkwindow)
        {
            gdk_draw_line( m_gdkwindow, m_penGC, 0, yy, XLOG2DEVREL(w), yy );
            gdk_draw_line( m_gdkwindow, m_penGC, xx, 0, xx, YLOG2DEVREL(h) );
        }
    }
}

void wxWindowDCImpl::DrawingSetup(GdkGC*& gc, bool& originChanged)
{
    gc = m_brushGC;
    GdkPixmap* pixmap = NULL;
    const int style = m_brush.GetStyle();

    if (style == wxBRUSHSTYLE_STIPPLE || style == wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE)
    {
        const wxBitmap* stipple = m_brush.GetStipple();
        if (stipple->IsOk())
        {
            if (style == wxBRUSHSTYLE_STIPPLE)
                pixmap = stipple->GetPixmap();
            else if (stipple->GetMask())
            {
                pixmap = stipple->GetPixmap();
                gc = m_textGC;
            }
        }
    }
    else if (m_brush.IsHatch())
    {
        pixmap = GetHatch(style);
    }

    int origin_x = 0;
    int origin_y = 0;
    if (pixmap)
    {
        int w, h;
        gdk_drawable_get_size(pixmap, &w, &h);
        origin_x = m_deviceOriginX % w;
        origin_y = m_deviceOriginY % h;
    }

    originChanged = origin_x || origin_y;
    if (originChanged)
        gdk_gc_set_ts_origin(gc, origin_x, origin_y);
}

void wxWindowDCImpl::DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCoord xx1 = XLOG2DEV(x1);
    wxCoord yy1 = YLOG2DEV(y1);
    wxCoord xx2 = XLOG2DEV(x2);
    wxCoord yy2 = YLOG2DEV(y2);
    wxCoord xxc = XLOG2DEV(xc);
    wxCoord yyc = YLOG2DEV(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord   r      = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else if ( wxIsNullDouble(radius) )
    {
        radius1 =
        radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy1-yyc), double(xx1-xxc)) * RAD2DEG;
        radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy2-yyc), double(xx2-xxc)) * RAD2DEG;
    }
    wxCoord alpha1 = wxCoord(radius1 * 64.0);
    wxCoord alpha2 = wxCoord((radius2 - radius1) * 64.0);
    while (alpha2 <= 0) alpha2 += 360*64;
    while (alpha1 > 360*64) alpha1 -= 360*64;

    if (m_gdkwindow)
    {
        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            gdk_draw_arc(m_gdkwindow, gc, true, xxc-r, yyc-r, 2*r, 2*r, alpha1, alpha2);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
        {
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );

            if ((m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT) && (alpha2 - alpha1 != 360*64))
            {
                gdk_draw_line( m_gdkwindow, m_penGC, xx1, yy1, xxc, yyc );
                gdk_draw_line( m_gdkwindow, m_penGC, xxc, yyc, xx2, yy2 );
            }
        }
    }

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDCImpl::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double sa, double ea )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_gdkwindow)
    {
        wxCoord start = wxCoord(sa * 64.0);
        wxCoord end = wxCoord((ea-sa) * 64.0);

        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            gdk_draw_arc(m_gdkwindow, gc, true, xx, yy, ww, hh, start, end);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xx, yy, ww, hh, start, end );
    }

    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDCImpl::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if ((m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT) && m_gdkwindow)
        gdk_draw_point( m_gdkwindow, m_penGC, XLOG2DEV(x), YLOG2DEV(y) );

    CalcBoundingBox (x, y);
}

void wxWindowDCImpl::DoDrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_pen.GetStyle() == wxPENSTYLE_TRANSPARENT) return;
    if (n <= 0) return;

    //Check, if scaling is necessary
    const bool doScale =
        xoffset != 0 || yoffset != 0 || XLOG2DEV(10) != 10 || YLOG2DEV(10) != 10;

    // GdkPoint and wxPoint have the same memory layout, so we can cast one to the other
    GdkPoint* gpts = reinterpret_cast<GdkPoint*>(points);

    if (doScale)
        gpts = new GdkPoint[n];

    for (int i = 0; i < n; i++)
    {
        if (doScale)
        {
            gpts[i].x = XLOG2DEV(points[i].x + xoffset);
            gpts[i].y = YLOG2DEV(points[i].y + yoffset);
        }
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }

    if (m_gdkwindow)
        gdk_draw_lines( m_gdkwindow, m_penGC, gpts, n);

    if (doScale)
        delete[] gpts;
}

void wxWindowDCImpl::DoDrawPolygon( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int WXUNUSED(fillStyle) )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (n <= 0) return;

    //Check, if scaling is necessary
    const bool doScale =
        xoffset != 0 || yoffset != 0 || XLOG2DEV(10) != 10 || YLOG2DEV(10) != 10;

    // GdkPoint and wxPoint have the same memory layout, so we can cast one to the other
    GdkPoint* gdkpoints = reinterpret_cast<GdkPoint*>(points);

    if (doScale)
        gdkpoints = new GdkPoint[n];

    int i;
    for (i = 0 ; i < n ; i++)
    {
        if (doScale)
        {
            gdkpoints[i].x = XLOG2DEV(points[i].x + xoffset);
            gdkpoints[i].y = YLOG2DEV(points[i].y + yoffset);
        }
        CalcBoundingBox(points[i].x + xoffset, points[i].y + yoffset);
    }

    if (m_gdkwindow)
    {
        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            gdk_draw_polygon(m_gdkwindow, gc, true, gdkpoints, n);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
        {
/*
            for (i = 0 ; i < n ; i++)
            {
                gdk_draw_line( m_gdkwindow, m_penGC,
                               gdkpoints[i%n].x,
                               gdkpoints[i%n].y,
                               gdkpoints[(i+1)%n].x,
                               gdkpoints[(i+1)%n].y);
            }
*/
            gdk_draw_polygon( m_gdkwindow, m_penGC, FALSE, gdkpoints, n );

        }
    }

    if (doScale)
        delete[] gdkpoints;
}

void wxWindowDCImpl::DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_gdkwindow)
    {
        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            gdk_draw_rectangle(m_gdkwindow, gc, true, xx, yy, ww, hh);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
        {
#if 1
            if ((m_pen.GetWidth() == 2) && (m_pen.GetCap() == wxCAP_ROUND) &&
                (m_pen.GetJoin() == wxJOIN_ROUND) && (m_pen.GetStyle() == wxPENSTYLE_SOLID))
            {
                // Use 2 1-line rects instead
                gdk_gc_set_line_attributes( m_penGC, 1, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND );

                if (m_signX == -1)
                {
                    // Different for RTL
                    gdk_draw_rectangle( m_gdkwindow, m_penGC, FALSE, xx+1, yy, ww-2, hh-2 );
                    gdk_draw_rectangle( m_gdkwindow, m_penGC, FALSE, xx, yy-1, ww, hh );
                }
                else
                {
                    gdk_draw_rectangle( m_gdkwindow, m_penGC, FALSE, xx, yy, ww-2, hh-2 );
                    gdk_draw_rectangle( m_gdkwindow, m_penGC, FALSE, xx-1, yy-1, ww, hh );
                }

                // reset
                gdk_gc_set_line_attributes( m_penGC, 2, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND );
            }
            else
#endif
            {
                // Just use X11 for other cases
                gdk_draw_rectangle( m_gdkwindow, m_penGC, FALSE, xx, yy, ww-1, hh-1 );
            }
        }
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDCImpl::DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (radius < 0.0) radius = - radius * ((width < height) ? width : height);

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);
    wxCoord rr = XLOG2DEVREL((wxCoord)radius);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    // CMB: if radius is zero use DrawRectangle() instead to avoid
    // X drawing errors with small radii
    if (rr == 0)
    {
        DoDrawRectangle( x, y, width, height );
        return;
    }

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: adjust size if outline is drawn otherwise the result is
    // 1 pixel too wide and high
    if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
    {
        ww--;
        hh--;
    }

    if (m_gdkwindow)
    {
        // CMB: ensure dd is not larger than rectangle otherwise we
        // get an hour glass shape
        wxCoord dd = 2 * rr;
        if (dd > ww) dd = ww;
        if (dd > hh) dd = hh;
        rr = dd / 2;

        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            gdk_draw_rectangle(m_gdkwindow, gc, true, xx+rr, yy, ww-dd+1, hh);
            gdk_draw_rectangle(m_gdkwindow, gc, true, xx, yy+rr, ww, hh-dd+1);
            gdk_draw_arc(m_gdkwindow, gc, true, xx, yy, dd, dd, 90*64, 90*64);
            gdk_draw_arc(m_gdkwindow, gc, true, xx+ww-dd, yy, dd, dd, 0, 90*64);
            gdk_draw_arc(m_gdkwindow, gc, true, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64);
            gdk_draw_arc(m_gdkwindow, gc, true, xx, yy+hh-dd, dd, dd, 180*64, 90*64);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
        {
            gdk_draw_line( m_gdkwindow, m_penGC, xx+rr+1, yy, xx+ww-rr, yy );
            gdk_draw_line( m_gdkwindow, m_penGC, xx+rr+1, yy+hh, xx+ww-rr, yy+hh );
            gdk_draw_line( m_gdkwindow, m_penGC, xx, yy+rr+1, xx, yy+hh-rr );
            gdk_draw_line( m_gdkwindow, m_penGC, xx+ww, yy+rr+1, xx+ww, yy+hh-rr );
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xx, yy, dd, dd, 90*64, 90*64 );
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
            gdk_draw_arc( m_gdkwindow, m_penGC, FALSE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
        }
    }

    // this ignores the radius
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDCImpl::DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_gdkwindow)
    {
        if (m_brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT)
        {
            GdkGC* gc;
            bool originChanged;
            DrawingSetup(gc, originChanged);

            // If the pen is transparent pen we increase the size
            // for better compatibility with other platforms.
            if (m_pen.GetStyle() == wxPENSTYLE_TRANSPARENT)
            {
                ++ww;
                ++hh;
            }

            gdk_draw_arc(m_gdkwindow, gc, true, xx, yy, ww, hh, 0, 360*64);

            if (originChanged)
                gdk_gc_set_ts_origin(gc, 0, 0);
        }

        if (m_pen.GetStyle() != wxPENSTYLE_TRANSPARENT)
            gdk_draw_arc( m_gdkwindow, m_penGC, false, xx, yy, ww, hh, 0, 360*64 );
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDCImpl::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    // VZ: egcs 1.0.3 refuses to compile this without cast, no idea why
    DoDrawBitmap( (const wxBitmap&)icon, x, y, true );
}

// compare to current clipping region
bool wxWindowDCImpl::IsOutsideOfClippingRegion(int x, int y, int w, int h)
{
    if (m_currentClippingRegion.IsNull())
        return false;

    wxRegion region(x, y, w, h);
    region.Intersect( m_currentClippingRegion );
    return region.IsEmpty();
}

void wxWindowDCImpl::RemoveClipMask(GdkGC *gc)
{
    gdk_gc_set_clip_mask(gc, NULL);
    gdk_gc_set_clip_origin(gc, 0, 0);
    if (!m_currentClippingRegion.IsNull())
        gdk_gc_set_clip_region(gc, m_currentClippingRegion.GetRegion());
}

// For drawing a mono-bitmap (XBitmap) we use the current text GC
void wxWindowDCImpl::DoDrawMonoBitmap(const wxBitmap& bitmap,
                                      int bmp_w, int bmp_h,
                                      int xsrc, int ysrc,
                                      int xdest, int ydest,
                                      int width, int height)
{
    wxGtkObject<GdkPixmap>
        bitmap2(gdk_pixmap_new( wxGetRootWindow()->window, bmp_w, bmp_h, -1 ));
    wxGtkObject<GdkGC> gc(gdk_gc_new( bitmap2 ));
    gdk_gc_set_foreground( gc, m_textForegroundColour.GetColor() );
    gdk_gc_set_background( gc, m_textBackgroundColour.GetColor() );
    gdk_wx_draw_bitmap(bitmap2, gc, bitmap.GetPixmap(), 0, 0);

    gdk_draw_drawable(m_gdkwindow, m_textGC, bitmap2, xsrc, ysrc, xdest, ydest,
                      width, height);
}

// Returns a new mask that is the intersection of the old mask
// and m_currentClippingRegion with proper offsets
GdkBitmap* wxWindowDCImpl::GetClippedMask(GdkBitmap* mask, int w, int h,
                                          int x, int y,
                                          int xsrcMask, int ysrcMask)
{
    // create monochrome bitmap that will be used as the new mask
    GdkBitmap *new_mask = gdk_pixmap_new( wxGetRootWindow()->window, w, h, 1 );

    GdkColor c0, c1;
    c0.pixel = 0;
    c1.pixel = 1;
    wxGtkObject<GdkGC> gc(gdk_gc_new( new_mask ));

    // zero-ing new_mask
    gdk_gc_set_foreground( gc, &c0 );
    gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, w, h );

    // clipping region
    gdk_gc_set_clip_region( gc, m_currentClippingRegion.GetRegion() );
    gdk_gc_set_clip_origin( gc, -x, -y );

    // copy the old mask to the new_mask in the clip region area
    gdk_gc_set_background( gc, &c0 );
    gdk_gc_set_foreground( gc, &c1 );
    gdk_gc_set_fill( gc, GDK_OPAQUE_STIPPLED );
    gdk_gc_set_ts_origin( gc, -xsrcMask, -ysrcMask );
    gdk_gc_set_stipple( gc, mask );
    gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, w, h );

    return new_mask;
}

void wxWindowDCImpl::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxCHECK_RET( bitmap.IsOk(), wxT("invalid bitmap") );

    bool is_mono = bitmap.GetDepth() == 1;

    // scale/translate size and position
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );

    if (!m_gdkwindow) return;

    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);

    if (m_window && m_window->GetLayoutDirection() == wxLayout_RightToLeft)
        xx -= ww;

    if (IsOutsideOfClippingRegion( xx,yy,ww,hh ))
        return;

    // scale bitmap if required
    wxBitmap use_bitmap = bitmap;
    if ((w != ww) || (h != hh))
        use_bitmap = use_bitmap.Rescale( 0, 0, w, h, ww, hh );

    // get mask if any
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (useMask && use_bitmap.GetMask())
        mask = use_bitmap.GetMask()->GetBitmap();

    // for drawing a mono-bitmap we use the current text GC
    GdkGC* use_gc = is_mono ? m_textGC : m_penGC;

    bool mask_owned = false;

    if (mask != NULL)
    {
        if (!m_currentClippingRegion.IsNull())
        {
            mask = GetClippedMask(mask, ww, hh, xx, yy, 0, 0);
            mask_owned = true;
        }

        gdk_gc_set_clip_mask(use_gc, mask);
        gdk_gc_set_clip_origin(use_gc, xx, yy);
    }

    // Draw XPixmap or XBitmap, depending on what the wxBitmap contains.
    if (is_mono)
    {
        DoDrawMonoBitmap(use_bitmap, ww, hh, 0, 0, xx, yy, -1, -1);
    }
    else
    {
        if (use_bitmap.HasPixbuf())
        {
            gdk_draw_pixbuf(m_gdkwindow, use_gc,
                            use_bitmap.GetPixbuf(),
                            0, 0, xx, yy, -1, -1,
                            GDK_RGB_DITHER_NORMAL, xx, yy);
        }
        else
        {
            gdk_draw_drawable(m_gdkwindow, use_gc,
                              use_bitmap.GetPixmap(),
                              0, 0, xx, yy, -1, -1);
        }
    }

    // remove mask again if any
    if (mask != NULL)
    {
        RemoveClipMask(use_gc);
        if (mask_owned)
            g_object_unref(mask);
    }
}

bool wxWindowDCImpl::DoBlit( wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         int logical_func,
                         bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask )
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid window dc") );

    wxCHECK_MSG( source, false, wxT("invalid source dc") );

    if (!m_gdkwindow) return false;

    // transform the source DC coords to the device ones
    xsrc = source->LogicalToDeviceX(xsrc);
    ysrc = source->LogicalToDeviceY(ysrc);

    wxBitmap selected;
    wxMemoryDC *memDC = wxDynamicCast(source, wxMemoryDC);
    if ( memDC )
    {
        selected = memDC->GetSelectedBitmap();
        if ( !selected.IsOk() )
            return false;
    }

    bool use_bitmap_method = false;
    bool is_mono = false;

    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc;
        ysrcMask = ysrc;
    }

    if (selected.IsOk())
    {
        is_mono = (selected.GetDepth() == 1);

        // we use the "XCopyArea" way to copy a memory dc into
        // a different window if the memory dc BOTH
        // a) doesn't have any mask or its mask isn't used
        // b) it is clipped
        // c) is not 1-bit

        if (useMask && (selected.GetMask()))
        {
            // we HAVE TO use the direct way for memory dcs
            // that have mask since the XCopyArea doesn't know
            // about masks
            use_bitmap_method = true;
        }
        else if (is_mono)
        {
            // we HAVE TO use the direct way for memory dcs
            // that are bitmaps because XCopyArea doesn't cope
            // with different bit depths
            use_bitmap_method = true;
        }
        else if ((xsrc == 0) && (ysrc == 0) &&
                 (width == selected.GetWidth()) &&
                 (height == selected.GetHeight()))
        {
            // we SHOULD use the direct way if all of the bitmap
            // in the memory dc is copied in which case XCopyArea
            // wouldn't be able able to boost performace by reducing
            // the area to be scaled
            use_bitmap_method = true;
        }
    }

    CalcBoundingBox( xdest, ydest );
    CalcBoundingBox( xdest + width, ydest + height );

    // scale/translate size and position
    wxCoord xx = XLOG2DEV(xdest);
    wxCoord yy = YLOG2DEV(ydest);

    wxCoord ww = XLOG2DEVREL(width);
    wxCoord hh = YLOG2DEVREL(height);

    // compare to current clipping region
    if (IsOutsideOfClippingRegion( xx,yy,ww,hh ))
        return true;

    int old_logical_func = m_logicalFunction;
    SetLogicalFunction( logical_func );

    if (use_bitmap_method)
    {
        // scale/translate bitmap size
        wxCoord bm_width = selected.GetWidth();
        wxCoord bm_height = selected.GetHeight();

        // Get clip coords for the bitmap. If we don't
        // use wxBitmap::Rescale(), which can clip the
        // bitmap, these are the same as the original
        // coordinates
        wxCoord cx = xx;
        wxCoord cy = yy;
        wxCoord cw = ww;
        wxCoord ch = hh;

        // interpret userscale of src too
        double xsc,ysc;
        memDC->GetUserScale(&xsc,&ysc);
        bm_width = (int) (bm_width / xsc);
        bm_height = (int) (bm_height / ysc);

        wxCoord bm_ww = XLOG2DEVREL( bm_width );
        wxCoord bm_hh = YLOG2DEVREL( bm_height );

        // Scale bitmap if required
        wxBitmap use_bitmap = selected;
        if ((selected.GetWidth()!= bm_ww) || ( selected.GetHeight()!= bm_hh))
        {
            // This indicates that the blitting code below will get
            // a clipped bitmap and therefore needs to move the origin
            // accordingly
            wxRegion tmp( xx,yy,ww,hh );
            if (!m_currentClippingRegion.IsNull())
                tmp.Intersect( m_currentClippingRegion );
            tmp.GetBox(cx,cy,cw,ch);

            // Scale and clipped bitmap
            use_bitmap = selected.Rescale(cx-xx,cy-yy,cw,ch,bm_ww,bm_hh);
        }

        // apply mask if any
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (useMask && use_bitmap.GetMask())
            mask = use_bitmap.GetMask()->GetBitmap();

        GdkGC* use_gc = is_mono ? m_textGC : m_penGC;

        bool mask_owned = false;

        if (mask != NULL)
        {
            if (!m_currentClippingRegion.IsNull())
            {
                mask = GetClippedMask(mask, bm_ww, bm_hh, cx, cy,
                                      xsrcMask, ysrcMask);
                mask_owned = true;
            }

            gdk_gc_set_clip_mask(use_gc, mask);
            if (mask_owned)
                gdk_gc_set_clip_origin(use_gc, cx, cy);
            else
                gdk_gc_set_clip_origin(use_gc, cx - xsrcMask, cy - ysrcMask);
        }

        // Draw XPixmap or XBitmap, depending on what the wxBitmap contains.
        if (is_mono)
        {
            DoDrawMonoBitmap(use_bitmap, bm_ww, bm_hh,
                             xsrc, ysrc, cx, cy, cw, ch);
        }
        else
        {
            // was: gdk_draw_drawable( m_gdkwindow, m_penGC, use_bitmap.GetPixmap(), xsrc, ysrc, xx, yy, ww, hh );
            gdk_draw_drawable(m_gdkwindow, use_gc, use_bitmap.GetPixmap(), xsrc, ysrc, cx, cy, cw, ch);
        }

        // remove mask again if any
        if (mask != NULL)
        {
            RemoveClipMask(use_gc);
            if (mask_owned)
                g_object_unref (mask);
        }
    }
    else // use_bitmap_method
    {
        if (selected.IsOk() && ((width != ww) || (height != hh)))
        {
            wxBitmap bitmap = selected.Rescale( xsrc, ysrc, width, height, ww, hh );
            // draw scaled bitmap
            gdk_draw_drawable( m_gdkwindow, m_penGC, bitmap.GetPixmap(), 0, 0, xx, yy, -1, -1 );
        }
        else
        {
            // No scaling and not a memory dc with a mask either
            GdkWindow* window = NULL;
            wxDCImpl *impl = source->GetImpl();
            wxWindowDCImpl *gtk_impl = wxDynamicCast(impl, wxWindowDCImpl);
            if (gtk_impl)
                window = gtk_impl->GetGDKWindow();
            if ( !window )
            {
                SetLogicalFunction( old_logical_func );
                return false;
            }

            // copy including child window contents
            gdk_gc_set_subwindow( m_penGC, GDK_INCLUDE_INFERIORS );
            gdk_draw_drawable( m_gdkwindow, m_penGC,
                               window,
                               xsrc, ysrc, xx, yy,
                               width, height );
            gdk_gc_set_subwindow( m_penGC, GDK_CLIP_BY_CHILDREN );
        }
    }

    SetLogicalFunction( old_logical_func );

    return true;
}

void wxWindowDCImpl::DoDrawText( const wxString &text, wxCoord x, wxCoord y )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (!m_gdkwindow) return;

    if (text.empty()) return;

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);

    wxCHECK_RET( m_context, wxT("no Pango context") );
    wxCHECK_RET( m_layout, wxT("no Pango layout") );
    wxCHECK_RET( m_fontdesc, wxT("no Pango font description") );

    gdk_pango_context_set_colormap( m_context, m_cmap );  // not needed in gtk+ >= 2.6

    bool underlined = m_font.IsOk() && m_font.GetUnderlined();

    wxCharBuffer data = wxGTK_CONV(text);
    if ( !data )
        return;
    size_t datalen = strlen(data);

    // in Pango >= 1.16 the "underline of leading/trailing spaces" bug
    // has been fixed and thus the hack implemented below should never be used
    static bool pangoOk = !wx_pango_version_check(1, 16, 0);

    bool needshack = underlined && !pangoOk;

    if (needshack)
    {
        // a PangoLayout which has leading/trailing spaces with underlined font
        // is not correctly drawn by this pango version: Pango won't underline the spaces.
        // This can be a problem; e.g. wxHTML rendering of underlined text relies on
        // this behaviour. To workaround this problem, we use a special hack here
        // suggested by pango maintainer Behdad Esfahbod: we prepend and append two
        // empty space characters and give them a dummy colour attribute.
        // This will force Pango to underline the leading/trailing spaces, too.

        wxCharBuffer data_tmp(datalen + 6);
        // copy the leading U+200C ZERO WIDTH NON-JOINER encoded in UTF8 format
        memcpy(data_tmp.data(), "\342\200\214", 3);
        // copy the user string
        memcpy(data_tmp.data() + 3, data, datalen);
        // copy the trailing U+200C ZERO WIDTH NON-JOINER encoded in UTF8 format
        memcpy(data_tmp.data() + 3 + datalen, "\342\200\214", 3);

        data = data_tmp;
        datalen += 6;
    }

    pango_layout_set_text(m_layout, data, datalen);

    if (underlined)
    {
        PangoAttrList *attrs = pango_attr_list_new();
        PangoAttribute *a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        a->start_index = 0;
        a->end_index = datalen;
        pango_attr_list_insert(attrs, a);

        if (needshack)
        {
            // dummy colour for the leading space
            a = pango_attr_foreground_new (0x0057, 0x52A9, 0xD614);
            a->start_index = 0;
            a->end_index = 1;
            pango_attr_list_insert(attrs, a);

            // dummy colour for the trailing space
            a = pango_attr_foreground_new (0x0057, 0x52A9, 0xD614);
            a->start_index = datalen - 1;
            a->end_index = datalen;
            pango_attr_list_insert(attrs, a);
        }

        pango_layout_set_attributes(m_layout, attrs);
        pango_attr_list_unref(attrs);
    }

    int oldSize = 0;
    const bool isScaled = fabs(m_scaleY - 1.0) > 0.00001;
    if (isScaled)
    {
        // If there is a user or actually any scale applied to
        // the device context, scale the font.

        // scale font description
        oldSize = pango_font_description_get_size(m_fontdesc);
        pango_font_description_set_size(m_fontdesc, int(oldSize * m_scaleY));

        // actually apply scaled font
        pango_layout_set_font_description( m_layout, m_fontdesc );
    }

    int w, h;
    pango_layout_get_pixel_size(m_layout, &w, &h);

    // Draw layout.
    int x_rtl = x;
    if (m_window && m_window->GetLayoutDirection() == wxLayout_RightToLeft)
        x_rtl -= w;

    const GdkColor* bg_col = NULL;
    if (m_backgroundMode == wxBRUSHSTYLE_SOLID) 
        bg_col = m_textBackgroundColour.GetColor();
 
    gdk_draw_layout_with_colors(m_gdkwindow, m_textGC, x_rtl, y, m_layout, NULL, bg_col);

    if (isScaled)
    {
         // reset unscaled size
         pango_font_description_set_size( m_fontdesc, oldSize );

         // actually apply unscaled font
         pango_layout_set_font_description( m_layout, m_fontdesc );
    }
    if (underlined)
    {
        // undo underline attributes setting:
        pango_layout_set_attributes(m_layout, NULL);
    }

    CalcBoundingBox(x + int(w / m_scaleX), y + int(h / m_scaleY));
    CalcBoundingBox(x, y);
}

// TODO: When GTK2.6 is required, merge DoDrawText and DoDrawRotatedText to 
// avoid code duplication
void wxWindowDCImpl::DoDrawRotatedText( const wxString &text, wxCoord x, wxCoord y, double angle )
{
    if (!m_gdkwindow || text.empty())
        return;

    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

#if __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        x = XLOG2DEV(x);
        y = YLOG2DEV(y);

        pango_layout_set_text(m_layout, wxGTK_CONV(text), -1);

        if (m_font.GetUnderlined())
        {
            PangoAttrList *attrs = pango_attr_list_new();
            PangoAttribute *a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
            pango_attr_list_insert(attrs, a);
            pango_layout_set_attributes(m_layout, attrs);
            pango_attr_list_unref(attrs);
        }

        int oldSize = 0;
        const bool isScaled = fabs(m_scaleY - 1.0) > 0.00001;
        if (isScaled)
        {
            //TODO: when Pango >= 1.6 is required, use pango_matrix_scale()
             // If there is a user or actually any scale applied to
             // the device context, scale the font.

             // scale font description
            oldSize = pango_font_description_get_size(m_fontdesc);
            pango_font_description_set_size(m_fontdesc, int(oldSize * m_scaleY));

             // actually apply scaled font
             pango_layout_set_font_description( m_layout, m_fontdesc );
        }

        int w, h;
        pango_layout_get_pixel_size(m_layout, &w, &h);

        const GdkColor* bg_col = NULL;
        if (m_backgroundMode == wxBRUSHSTYLE_SOLID) 
            bg_col = m_textBackgroundColour.GetColor();

        // rotate the text
        PangoMatrix matrix = PANGO_MATRIX_INIT;
        pango_matrix_rotate (&matrix, angle);
        pango_context_set_matrix (m_context, &matrix);
        pango_layout_context_changed (m_layout);

        // To be compatible with MSW, the rotation axis must be in the old 
        // top-left corner.
        // Calculate the vertices of the rotated rectangle containing the text, 
        // relative to the old top-left vertex.
        // We could use the matrix for this, but it's simpler with trignonometry. 
        double rad = DegToRad(angle);
        // the rectangle vertices are counted clockwise with the first one 
        // being at (0, 0)
        double x2 = w * cos(rad);
        double y2 = -w * sin(rad);   // y axis points to the bottom, hence minus
        double x4 = h * sin(rad);
        double y4 = h * cos(rad);
        double x3 = x4 + x2;
        double y3 = y4 + y2;
        // Then we calculate max and min of the rotated rectangle.
        wxCoord maxX = (wxCoord)(dmax(dmax(0, x2), dmax(x3, x4)) + 0.5),
                maxY = (wxCoord)(dmax(dmax(0, y2), dmax(y3, y4)) + 0.5),
                minX = (wxCoord)(dmin(dmin(0, x2), dmin(x3, x4)) - 0.5),
                minY = (wxCoord)(dmin(dmin(0, y2), dmin(y3, y4)) - 0.5);

        gdk_draw_layout_with_colors(m_gdkwindow, m_textGC, x+minX, y+minY, 
                                    m_layout, NULL, bg_col);

        if (m_font.GetUnderlined())
            pango_layout_set_attributes(m_layout, NULL);

        // clean up the transformation matrix
        pango_context_set_matrix(m_context, NULL);

        if (isScaled)
        {
             // reset unscaled size
             pango_font_description_set_size( m_fontdesc, oldSize );

             // actually apply unscaled font
             pango_layout_set_font_description( m_layout, m_fontdesc );
        }

        CalcBoundingBox(x+minX, y+minY);
        CalcBoundingBox(x+maxX, y+maxY);
    }
    else
#endif //__WXGTK26__
    {
#if wxUSE_IMAGE
    if ( wxIsNullDouble(angle) )
    {
        DoDrawText(text, x, y);
        return;
    }

    wxCoord w;
    wxCoord h;

    // TODO: implement later without GdkFont for GTK 2.0
    DoGetTextExtent(text, &w, &h, NULL,NULL, &m_font);

    // draw the string normally
    wxBitmap src(w, h);
    wxMemoryDC dc;
    dc.SelectObject(src);
    dc.SetFont(GetFont());
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetTextForeground( *wxWHITE );
    dc.DrawText(text, 0, 0);
    dc.SelectObject(wxNullBitmap);

    // Calculate the size of the rotated bounding box.
    double rad = DegToRad(angle);
    double dx = cos(rad),
           dy = sin(rad);

    // the rectngle vertices are counted clockwise with the first one being at
    // (0, 0) (or, rather, at (x, y))
    double x2 = w*dx,
           y2 = -w*dy;      // y axis points to the bottom, hence minus
    double x4 = h*dy,
           y4 = h*dx;
    double x3 = x4 + x2,
           y3 = y4 + y2;

    // calc max and min
    wxCoord maxX = (wxCoord)(dmax(x2, dmax(x3, x4)) + 0.5),
            maxY = (wxCoord)(dmax(y2, dmax(y3, y4)) + 0.5),
            minX = (wxCoord)(dmin(x2, dmin(x3, x4)) - 0.5),
            minY = (wxCoord)(dmin(y2, dmin(y3, y4)) - 0.5);


    wxImage image = src.ConvertToImage();

    image.ConvertColourToAlpha( m_textForegroundColour.Red(),
                                m_textForegroundColour.Green(),
                                m_textForegroundColour.Blue() );
    image = image.Rotate( rad, wxPoint(0,0) );

    int i_angle = (int) angle;
    i_angle = i_angle % 360;
    if (i_angle < 0)
        i_angle += 360;
    int xoffset = 0;
    if ((i_angle >= 90.0) && (i_angle < 270.0))
        xoffset = image.GetWidth();
    int yoffset = 0;
    if ((i_angle >= 0.0) && (i_angle < 180.0))
        yoffset = image.GetHeight();

    if ((i_angle >= 0) && (i_angle < 90))
        yoffset -= (int)( cos(rad)*h );
    if ((i_angle >= 90) && (i_angle < 180))
        xoffset -= (int)( sin(rad)*h );
    if ((i_angle >= 180) && (i_angle < 270))
        yoffset -= (int)( cos(rad)*h );
    if ((i_angle >= 270) && (i_angle < 360))
        xoffset -= (int)( sin(rad)*h );

    int i_x = x - xoffset;
    int i_y = y - yoffset;

    src = image;
    DoDrawBitmap( src, i_x, i_y, true );


    // it would be better to draw with non underlined font and draw the line
    // manually here (it would be more straight...)
#if 0
    if ( m_font.GetUnderlined() )
    {
        gdk_draw_line( m_gdkwindow, m_textGC,
                       XLOG2DEV(x + x4), YLOG2DEV(y + y4 + font->descent),
                       XLOG2DEV(x + x3), YLOG2DEV(y + y3 + font->descent));
    }
#endif // 0

    // update the bounding box
    CalcBoundingBox(x + minX, y + minY);
    CalcBoundingBox(x + maxX, y + maxY);
#else // !wxUSE_IMAGE
    wxUnusedVar(text);
    wxUnusedVar(x);
    wxUnusedVar(y);
    wxUnusedVar(angle);
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
    }
}

void wxWindowDCImpl::DoGetTextExtent(const wxString &string,
                                 wxCoord *width, wxCoord *height,
                                 wxCoord *descent, wxCoord *externalLeading,
                                 const wxFont *theFont) const
{
    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    if (string.empty())
        return;

    // ensure that theFont is always non-NULL
    if ( !theFont || !theFont->IsOk() )
        theFont = &m_font;

    // and use it if it's valid
    if ( theFont->IsOk() )
    {
        pango_layout_set_font_description
        (
            m_layout,
            theFont->GetNativeFontInfo()->description
        );
    }

    // Set layout's text
    const wxCharBuffer dataUTF8 = wxGTK_CONV_FONT(string, *theFont);
    if ( !dataUTF8 )
    {
        // hardly ideal, but what else can we do if conversion failed?
        return;
    }

    pango_layout_set_text(m_layout, dataUTF8, -1);

    int h;
    pango_layout_get_pixel_size(m_layout, width, &h);
    if (descent)
    {
        PangoLayoutIter *iter = pango_layout_get_iter(m_layout);
        int baseline = pango_layout_iter_get_baseline(iter);
        pango_layout_iter_free(iter);
        *descent = h - PANGO_PIXELS(baseline);
    }
    if (height)
        *height = h;

    // Reset old font description
    if (theFont->IsOk())
        pango_layout_set_font_description( m_layout, m_fontdesc );
}


bool wxWindowDCImpl::DoGetPartialTextExtents(const wxString& text,
                                         wxArrayInt& widths) const
{
    const size_t len = text.length();
    widths.Empty();
    widths.Add(0, len);

    if (text.empty())
        return true;

    // Set layout's text
    const wxCharBuffer dataUTF8 = wxGTK_CONV_FONT(text, m_font);
    if ( !dataUTF8 )
    {
        // hardly ideal, but what else can we do if conversion failed?
        wxLogLastError(wxT("DoGetPartialTextExtents"));
        return false;
    }

    pango_layout_set_text(m_layout, dataUTF8, -1);

    // Calculate the position of each character based on the widths of
    // the previous characters

    // Code borrowed from Scintilla's PlatGTK
    PangoLayoutIter *iter = pango_layout_get_iter(m_layout);
    PangoRectangle pos;
    pango_layout_iter_get_cluster_extents(iter, NULL, &pos);
    size_t i = 0;
    while (pango_layout_iter_next_cluster(iter))
    {
        pango_layout_iter_get_cluster_extents(iter, NULL, &pos);
        int position = PANGO_PIXELS(pos.x);
        widths[i++] = position;
    }
    while (i < len)
        widths[i++] = PANGO_PIXELS(pos.x + pos.width);
    pango_layout_iter_free(iter);

    return true;
}


wxCoord wxWindowDCImpl::GetCharWidth() const
{
    pango_layout_set_text( m_layout, "H", 1 );
    int w;
    pango_layout_get_pixel_size( m_layout, &w, NULL );
    return w;
}

wxCoord wxWindowDCImpl::GetCharHeight() const
{
    PangoFontMetrics *metrics = pango_context_get_metrics (m_context, m_fontdesc, pango_context_get_language(m_context));
    wxCHECK_MSG( metrics, -1, _T("failed to get pango font metrics") );

    wxCoord h = PANGO_PIXELS (pango_font_metrics_get_descent (metrics) +
                pango_font_metrics_get_ascent (metrics));
    pango_font_metrics_unref (metrics);
    return h;
}

void wxWindowDCImpl::Clear()
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (!m_gdkwindow) return;

    int width,height;
    DoGetSize( &width, &height );
    gdk_draw_rectangle( m_gdkwindow, m_bgGC, TRUE, 0, 0, width, height );
}

void wxWindowDCImpl::SetFont( const wxFont &font )
{
    m_font = font;

    if (m_font.IsOk())
    {
        if (m_fontdesc)
            pango_font_description_free( m_fontdesc );

        m_fontdesc = pango_font_description_copy( m_font.GetNativeFontInfo()->description );


        if (m_window)
        {
            PangoContext *oldContext = m_context;

            m_context = m_window->GtkGetPangoDefaultContext();

            // If we switch back/forth between different contexts
            // we also have to create a new layout. I think so,
            // at least, and it doesn't hurt to do it.
            if (oldContext != m_context)
            {
                if (m_layout)
                    g_object_unref (m_layout);

                m_layout = pango_layout_new( m_context );
            }
        }

        pango_layout_set_font_description( m_layout, m_fontdesc );
    }
}

void wxWindowDCImpl::SetPen( const wxPen &pen )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_pen == pen) return;

    m_pen = pen;

    if (!m_pen.IsOk()) return;

    if (!m_gdkwindow) return;

    gint width = m_pen.GetWidth();
    if (width <= 0)
    {
        // CMB: if width is non-zero scale it with the dc
        width = 1;
    }
    else
    {
        // X doesn't allow different width in x and y and so we take
        // the average
        double w = 0.5 +
                   ( fabs((double) XLOG2DEVREL(width)) +
                     fabs((double) YLOG2DEVREL(width)) ) / 2.0;
        width = (int)w;
        if ( !width )
        {
            // width can't be 0 or an internal GTK error occurs inside
            // gdk_gc_set_dashes() below
            width = 1;
        }
    }

    static const wxGTKDash dotted[] = {1, 1};
    static const wxGTKDash short_dashed[] = {2, 2};
    static const wxGTKDash wxCoord_dashed[] = {2, 4};
    static const wxGTKDash dotted_dashed[] = {3, 3, 1, 3};

    // We express dash pattern in pen width unit, so we are
    // independent of zoom factor and so on...
    int req_nb_dash;
    const wxGTKDash *req_dash;

    GdkLineStyle lineStyle = GDK_LINE_ON_OFF_DASH;
    switch (m_pen.GetStyle())
    {
        case wxPENSTYLE_USER_DASH:
            req_nb_dash = m_pen.GetDashCount();
            req_dash = (wxGTKDash*)m_pen.GetDash();
            break;
        case wxPENSTYLE_DOT:
            req_nb_dash = 2;
            req_dash = dotted;
            break;
        case wxPENSTYLE_LONG_DASH:
            req_nb_dash = 2;
            req_dash = wxCoord_dashed;
            break;
        case wxPENSTYLE_SHORT_DASH:
            req_nb_dash = 2;
            req_dash = short_dashed;
            break;
        case wxPENSTYLE_DOT_DASH:
            req_nb_dash = 4;
            req_dash = dotted_dashed;
            break;

        case wxPENSTYLE_TRANSPARENT:
        case wxPENSTYLE_STIPPLE_MASK_OPAQUE:
        case wxPENSTYLE_STIPPLE:
        case wxPENSTYLE_SOLID:
        default:
            lineStyle = GDK_LINE_SOLID;
            req_dash = (wxGTKDash*)NULL;
            req_nb_dash = 0;
            break;
    }

    if (req_dash && req_nb_dash)
    {
        wxGTKDash *real_req_dash = new wxGTKDash[req_nb_dash];
        if (real_req_dash)
        {
            for (int i = 0; i < req_nb_dash; i++)
                real_req_dash[i] = req_dash[i] * width;
            gdk_gc_set_dashes( m_penGC, 0, real_req_dash, req_nb_dash );
            delete[] real_req_dash;
        }
        else
        {
            // No Memory. We use non-scaled dash pattern...
            gdk_gc_set_dashes( m_penGC, 0, (wxGTKDash*)req_dash, req_nb_dash );
        }
    }

    GdkCapStyle capStyle = GDK_CAP_ROUND;
    switch (m_pen.GetCap())
    {
        case wxCAP_PROJECTING: { capStyle = GDK_CAP_PROJECTING; break; }
        case wxCAP_BUTT:       { capStyle = GDK_CAP_BUTT;       break; }
        case wxCAP_ROUND:
        default:
            if (width <= 1)
            {
                width = 0;
                capStyle = GDK_CAP_NOT_LAST;
            }
            break;
    }

    GdkJoinStyle joinStyle = GDK_JOIN_ROUND;
    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL: { joinStyle = GDK_JOIN_BEVEL; break; }
        case wxJOIN_MITER: { joinStyle = GDK_JOIN_MITER; break; }
        case wxJOIN_ROUND:
        default:           { joinStyle = GDK_JOIN_ROUND; break; }
    }

    gdk_gc_set_line_attributes( m_penGC, width, lineStyle, capStyle, joinStyle );

    m_pen.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_penGC, m_pen.GetColour().GetColor() );
}

void wxWindowDCImpl::SetBrush( const wxBrush &brush )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_brush == brush) return;

    m_brush = brush;

    if (!m_brush.IsOk()) return;

    if (!m_gdkwindow) return;

    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );

    gdk_gc_set_fill( m_brushGC, GDK_SOLID );

    if ((m_brush.GetStyle() == wxBRUSHSTYLE_STIPPLE) && (m_brush.GetStipple()->IsOk()))
    {
        if (m_brush.GetStipple()->GetDepth() != 1)
        {
            gdk_gc_set_fill( m_brushGC, GDK_TILED );
            gdk_gc_set_tile( m_brushGC, m_brush.GetStipple()->GetPixmap() );
        }
        else
        {
            gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
            gdk_gc_set_stipple( m_brushGC, m_brush.GetStipple()->GetPixmap() );
        }
    }

    if ((m_brush.GetStyle() == wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
    {
        gdk_gc_set_fill( m_textGC, GDK_OPAQUE_STIPPLED);
        gdk_gc_set_stipple( m_textGC, m_brush.GetStipple()->GetMask()->GetBitmap() );
    }

    if (m_brush.IsHatch())
    {
        gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
        gdk_gc_set_stipple(m_brushGC, GetHatch(m_brush.GetStyle()));
    }
}

void wxWindowDCImpl::SetBackground( const wxBrush &brush )
{
   /* CMB 21/7/98: Added SetBackground. Sets background brush
    * for Clear() and bg colour for shapes filled with cross-hatch brush */

    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_backgroundBrush == brush) return;

    m_backgroundBrush = brush;

    if (!m_backgroundBrush.IsOk()) return;

    if (!m_gdkwindow) return;

    wxColor color = m_backgroundBrush.GetColour();
    color.CalcPixel(m_cmap);
    const GdkColor* gdkColor = color.GetColor();
    gdk_gc_set_background(m_brushGC, gdkColor);
    gdk_gc_set_background(m_penGC,   gdkColor);
    gdk_gc_set_background(m_bgGC,    gdkColor);
    gdk_gc_set_foreground(m_bgGC,    gdkColor);


    gdk_gc_set_fill( m_bgGC, GDK_SOLID );

    if (m_backgroundBrush.GetStyle() == wxBRUSHSTYLE_STIPPLE)
    {
        const wxBitmap* stipple = m_backgroundBrush.GetStipple();
        if (stipple->IsOk())
        {
            if (stipple->GetDepth() != 1)
            {
                gdk_gc_set_fill(m_bgGC, GDK_TILED);
                gdk_gc_set_tile(m_bgGC, stipple->GetPixmap());
            }
            else
            {
                gdk_gc_set_fill(m_bgGC, GDK_STIPPLED);
                gdk_gc_set_stipple(m_bgGC, stipple->GetPixmap());
            }
        }
    }
    else if (m_backgroundBrush.IsHatch())
    {
        gdk_gc_set_fill( m_bgGC, GDK_STIPPLED );
        gdk_gc_set_stipple(m_bgGC, GetHatch(m_backgroundBrush.GetStyle()));
    }
}

void wxWindowDCImpl::SetLogicalFunction( int function )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (m_logicalFunction == function)
        return;

    // VZ: shouldn't this be a CHECK?
    if (!m_gdkwindow)
        return;

    GdkFunction mode;
    switch (function)
    {
        case wxXOR:          mode = GDK_XOR;           break;
        case wxINVERT:       mode = GDK_INVERT;        break;
        case wxOR_REVERSE:   mode = GDK_OR_REVERSE;    break;
        case wxAND_REVERSE:  mode = GDK_AND_REVERSE;   break;
        case wxCLEAR:        mode = GDK_CLEAR;         break;
        case wxSET:          mode = GDK_SET;           break;
        case wxOR_INVERT:    mode = GDK_OR_INVERT;     break;
        case wxAND:          mode = GDK_AND;           break;
        case wxOR:           mode = GDK_OR;            break;
        case wxEQUIV:        mode = GDK_EQUIV;         break;
        case wxNAND:         mode = GDK_NAND;          break;
        case wxAND_INVERT:   mode = GDK_AND_INVERT;    break;
        case wxCOPY:         mode = GDK_COPY;          break;
        case wxNO_OP:        mode = GDK_NOOP;          break;
        case wxSRC_INVERT:   mode = GDK_COPY_INVERT;   break;
        case wxNOR:          mode = GDK_NOR;           break;
        default:
           wxFAIL_MSG( wxT("unsupported logical function") );
           mode = GDK_COPY;
    }

    m_logicalFunction = function;

    gdk_gc_set_function( m_penGC, mode );
    gdk_gc_set_function( m_brushGC, mode );

    // to stay compatible with wxMSW, we don't apply ROPs to the text
    // operations (i.e. DrawText/DrawRotatedText).
    // True, but mono-bitmaps use the m_textGC and they use ROPs as well.
    gdk_gc_set_function( m_textGC, mode );
}

void wxWindowDCImpl::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    // don't set m_textForegroundColour to an invalid colour as we'd crash
    // later then (we use m_textForegroundColour.GetColor() without checking
    // in a few places)
    if ( !col.IsOk() || (m_textForegroundColour == col) )
        return;

    m_textForegroundColour = col;

    if ( m_gdkwindow )
    {
        m_textForegroundColour.CalcPixel( m_cmap );
        gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
    }
}

void wxWindowDCImpl::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    // same as above
    if ( !col.IsOk() || (m_textBackgroundColour == col) )
        return;

    m_textBackgroundColour = col;

    if ( m_gdkwindow )
    {
        m_textBackgroundColour.CalcPixel( m_cmap );
        gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );
    }
}

void wxWindowDCImpl::SetBackgroundMode( int mode )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    m_backgroundMode = mode;
}

void wxWindowDCImpl::SetPalette( const wxPalette& WXUNUSED(palette) )
{
    wxFAIL_MSG( wxT("wxWindowDCImpl::SetPalette not implemented") );
}

void wxWindowDCImpl::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (!m_gdkwindow) return;

    wxRect rect;
    rect.x = XLOG2DEV(x);
    rect.y = YLOG2DEV(y);
    rect.width = XLOG2DEVREL(width);
    rect.height = YLOG2DEVREL(height);

    if (m_window && m_window->m_wxwindow && 
        (m_window->GetLayoutDirection() == wxLayout_RightToLeft))
    {
        rect.x -= rect.width;
    }

    DoSetDeviceClippingRegion(wxRegion(rect));
}

void wxWindowDCImpl::DoSetDeviceClippingRegion( const wxRegion &region  )
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    if (!m_gdkwindow) return;

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( region );
    else
        m_currentClippingRegion.Union( region );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxGTKDCImpl::DoSetClippingRegion( xx, yy, ww, hh );

    GdkRegion* gdkRegion = m_currentClippingRegion.GetRegion();
    gdk_gc_set_clip_region(m_penGC,   gdkRegion);
    gdk_gc_set_clip_region(m_brushGC, gdkRegion);
    gdk_gc_set_clip_region(m_textGC,  gdkRegion);
    gdk_gc_set_clip_region(m_bgGC,    gdkRegion);
}

void wxWindowDCImpl::DestroyClippingRegion()
{
    wxCHECK_RET( IsOk(), wxT("invalid window dc") );

    wxDCImpl::DestroyClippingRegion();

    m_currentClippingRegion.Clear();

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsEmpty())
        m_currentClippingRegion.Union( m_paintClippingRegion );
#endif

    if (!m_gdkwindow) return;

    GdkRegion* gdkRegion = NULL;
    if (!m_currentClippingRegion.IsEmpty())
        gdkRegion = m_currentClippingRegion.GetRegion();

    gdk_gc_set_clip_region(m_penGC,   gdkRegion);
    gdk_gc_set_clip_region(m_brushGC, gdkRegion);
    gdk_gc_set_clip_region(m_textGC,  gdkRegion);
    gdk_gc_set_clip_region(m_bgGC,    gdkRegion);
}

void wxWindowDCImpl::Destroy()
{
    if (m_penGC) wxFreePoolGC( m_penGC );
    m_penGC = (GdkGC*) NULL;
    if (m_brushGC) wxFreePoolGC( m_brushGC );
    m_brushGC = (GdkGC*) NULL;
    if (m_textGC) wxFreePoolGC( m_textGC );
    m_textGC = (GdkGC*) NULL;
    if (m_bgGC) wxFreePoolGC( m_bgGC );
    m_bgGC = (GdkGC*) NULL;
}

void wxWindowDCImpl::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    m_deviceOriginX = x;
    m_deviceOriginY = y;

    ComputeScaleAndOrigin();
}

void wxWindowDCImpl::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);

    if (m_window && m_window->m_wxwindow && 
        (m_window->GetLayoutDirection() == wxLayout_RightToLeft))
        m_signX = -m_signX;

    ComputeScaleAndOrigin();
}

void wxWindowDCImpl::ComputeScaleAndOrigin()
{
    const wxRealPoint origScale(m_scaleX, m_scaleY);

    wxDCImpl::ComputeScaleAndOrigin();

    // if scale has changed call SetPen to recalulate the line width
    if ( wxRealPoint(m_scaleX, m_scaleY) != origScale && m_pen.IsOk() )
    {
        // this is a bit artificial, but we need to force wxDC to think the pen
        // has changed
        wxPen pen = m_pen;
        m_pen = wxNullPen;
        SetPen( pen );
    }
}

// Resolution in pixels per logical inch
wxSize wxWindowDCImpl::GetPPI() const
{
    return wxSize( (int) (m_mm_to_pix_x * 25.4 + 0.5), (int) (m_mm_to_pix_y * 25.4 + 0.5));
}

int wxWindowDCImpl::GetDepth() const
{
    return gdk_drawable_get_depth(m_gdkwindow);
}


//-----------------------------------------------------------------------------
// wxClientDCImpl
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxClientDCImpl, wxWindowDCImpl)

wxClientDCImpl::wxClientDCImpl( wxDC *owner )
          : wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win )
          : wxWindowDCImpl( owner, win )
{
    wxCHECK_RET( win, _T("NULL window in wxClientDCImpl::wxClientDC") );

#ifdef __WXUNIVERSAL__
    wxPoint ptOrigin = win->GetClientAreaOrigin();
    SetDeviceOrigin(ptOrigin.x, ptOrigin.y);
    wxSize size = win->GetClientSize();
    DoSetClippingRegion(0, 0, size.x, size.y);
#endif 
    // __WXUNIVERSAL__
}

void wxClientDCImpl::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_window, _T("GetSize() doesn't work without window") );

    m_window->GetClientSize( width, height );
}

//-----------------------------------------------------------------------------
// wxPaintDCImpl
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPaintDCImpl, wxClientDCImpl)

// Limit the paint region to the window size. Sometimes
// the paint region is too big, and this risks X11 errors
static void wxLimitRegionToSize(wxRegion& region, const wxSize& sz)
{
    wxRect originalRect = region.GetBox();
    wxRect rect(originalRect);
    if (rect.width + rect.x > sz.x)
        rect.width = sz.x - rect.x;
    if (rect.height + rect.y > sz.y)
        rect.height = sz.y - rect.y;
    if (rect != originalRect)
    {
        region = wxRegion(rect);
        wxLogTrace(wxT("painting"), wxT("Limiting region from %d, %d, %d, %d to %d, %d, %d, %d\n"),
                   originalRect.x, originalRect.y, originalRect.width, originalRect.height,
                   rect.x, rect.y, rect.width, rect.height);
    }
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
         : wxClientDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
         : wxClientDCImpl( owner, win )
{
#if USE_PAINT_REGION
    if (!win->m_clipPaintRegion)
        return;

    wxSize sz = win->GetSize();
    m_paintClippingRegion = win->m_nativeUpdateRegion;
    wxLimitRegionToSize(m_paintClippingRegion, sz);

    GdkRegion *region = m_paintClippingRegion.GetRegion();
    if ( region )
    {
        m_currentClippingRegion.Union( m_paintClippingRegion );
        wxLimitRegionToSize(m_currentClippingRegion, sz);

        if (sz.x <= 0 || sz.y <= 0)
            return ;

        gdk_gc_set_clip_region( m_penGC, region );
        gdk_gc_set_clip_region( m_brushGC, region );
        gdk_gc_set_clip_region( m_textGC, region );
        gdk_gc_set_clip_region( m_bgGC, region );
    }
#endif 
}

// ----------------------------------------------------------------------------
// wxDCModule
// ----------------------------------------------------------------------------

class wxDCModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxDCModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule)

bool wxDCModule::OnInit()
{
    wxInitGCPool();
    return true;
}

void wxDCModule::OnExit()
{
    wxCleanUpGCPool();

    for (int i = wxBRUSHSTYLE_LAST_HATCH - wxBRUSHSTYLE_FIRST_HATCH; i--; )
    {
        if (hatches[i])
            g_object_unref(hatches[i]);
    }
}
