/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem, Chris Breeze
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/image.h"
#include <math.h>

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"
#define  num_hatches 6

static GdkPixmap  *hatches[num_hatches];
static GdkPixmap **hatch_bitmap = (GdkPixmap **) NULL;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define RAD2DEG 57.2957795131

//-----------------------------------------------------------------------------
// temporary implementation of the missing GDK function
//-----------------------------------------------------------------------------

#include "gdk/gdkprivate.h"

void gdk_draw_bitmap     (GdkDrawable  *drawable,
                          GdkGC               *gc,
                          GdkDrawable  *src,
                          gint                xsrc,
                          gint                ysrc,
                          gint                xdest,
                          gint                ydest,
                          gint                width,
                          gint                height)
{
    GdkWindowPrivate *drawable_private;
    GdkWindowPrivate *src_private;
    GdkGCPrivate *gc_private;

    g_return_if_fail (drawable != NULL);
    g_return_if_fail (src != NULL);
    g_return_if_fail (gc != NULL);

    drawable_private = (GdkWindowPrivate*) drawable;
    src_private = (GdkWindowPrivate*) src;
    if (drawable_private->destroyed || src_private->destroyed)
        return;
        
    gc_private = (GdkGCPrivate*) gc;

    if (width == -1) width = src_private->width;
    if (height == -1) height = src_private->height;

    XCopyPlane( drawable_private->xdisplay,
                src_private->xwindow,
                drawable_private->xwindow,
                gc_private->xgc,
                xsrc, ysrc,
                width, height,
                xdest, ydest,
                1 );
}

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC()
{
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_isMemDC = FALSE;
    m_owner = (wxWindow *)NULL;
}

wxWindowDC::wxWindowDC( wxWindow *window )
{
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_owner = (wxWindow *)NULL;
    m_isMemDC = FALSE;
    m_font = window->GetFont();
  
    wxASSERT_MSG( window, _T("DC needs a window") );
    
    GtkWidget *widget = window->m_wxwindow;
    
    wxASSERT_MSG( widget, _T("DC needs a widget") );
    
    m_window = widget->window;
    
    /* not realized ? */
    if (!m_window)
    {
         /* don't report problems */
	 m_ok = TRUE;
	 
	 return;
    }
    
    if (window->m_wxwindow)
        m_cmap = gtk_widget_get_colormap( window->m_wxwindow );
    else
        m_cmap = gtk_widget_get_colormap( window->m_widget );
        
    SetUpDC();

    /* this must be done after SetUpDC, bacause SetUpDC calls the
       repective SetBrush, SetPen, SetBackground etc functions
       to set up the DC. SetBackground call m_owner->SetBackground
       and this might not be desired as the standard dc background
       is white whereas a window might assume gray to be the
       standard (as e.g. wxStatusBar) */
    
    m_owner = window;
}

wxWindowDC::~wxWindowDC()
{
    Destroy();
}

void wxWindowDC::DoFloodFill( long WXUNUSED(x), long WXUNUSED(y), 
                           const wxColour &WXUNUSED(col), int WXUNUSED(style) )
{
    wxFAIL_MSG( _T("wxWindowDC::DoFloodFill not implemented") );
}

bool wxWindowDC::DoGetPixel( long WXUNUSED(x1), long WXUNUSED(y1), wxColour *WXUNUSED(col) ) const
{
    wxFAIL_MSG( _T("wxWindowDC::DoGetPixel not implemented") );
    return FALSE;
}

void wxWindowDC::DoDrawLine( long x1, long y1, long x2, long y2 )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        if (m_window)
            gdk_draw_line( m_window, m_penGC, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2) );
                       
        CalcBoundingBox(x1, y1);
        CalcBoundingBox(x2, y2);
    }
}

void wxWindowDC::DoCrossHair( long x, long y )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        long xx = XLOG2DEV(x);
        long yy = YLOG2DEV(y);
        if (m_window)
	{
            gdk_draw_line( m_window, m_penGC, 0, yy, XLOG2DEVREL(w), yy );
            gdk_draw_line( m_window, m_penGC, xx, 0, xx, YLOG2DEVREL(h) );
	}
    }
}

void wxWindowDC::DoDrawArc( long x1, long y1, long x2, long y2,
                            long xc, long yc )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    long xx1 = XLOG2DEV(x1); 
    long yy1 = YLOG2DEV(y1);
    long xx2 = XLOG2DEV(x2); 
    long yy2 = YLOG2DEV(y2);
    long xxc = XLOG2DEV(xc); 
    long yyc = YLOG2DEV(yc);
    double dx = xx1 - xxc; 
    double dy = yy1 - yyc;
    double radius = sqrt(dx*dx+dy*dy);
    long   r      = (long)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2) 
    {
        radius1 = 0.0;
        radius2 = 360.0;
    } 
    else 
    if (radius == 0.0) 
    {
        radius1 = radius2 = 0.0;
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
    long alpha1 = long(radius1 * 64.0);
    long alpha2 = long((radius2 - radius1) * 64.0);
    while (alpha2 <= 0) alpha2 += 360*64;
    while (alpha1 > 360*64) alpha1 -= 360*64;

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
    
        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_penGC, FALSE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
    }
      
    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDC::DoDrawEllipticArc( long x, long y, long width, long height, double sa, double ea )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    long xx = XLOG2DEV(x);    
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width); 
    long hh = m_signY * YLOG2DEVREL(height);
  
    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
  
    if (m_window)
    {
        long start = long(sa * 64.0);
        long end = long(ea * 64.0);
    
        if (m_brush.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
  
        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, start, end );
    }
        
    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDC::DoDrawPoint( long x, long y )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if ((m_pen.GetStyle() != wxTRANSPARENT) && m_window)
        gdk_draw_point( m_window, m_penGC, XLOG2DEV(x), YLOG2DEV(y) );
        
    CalcBoundingBox (x, y);
}

void wxWindowDC::DoDrawLines( int n, wxPoint points[], long xoffset, long yoffset )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_pen.GetStyle() == wxTRANSPARENT) return;
    if (n <= 0) return;
  
    CalcBoundingBox( points[0].x + xoffset, points[0].y + yoffset );
    
    for (int i = 0; i < n-1; i++)
    {
        long x1 = XLOG2DEV(points[i].x + xoffset);
        long x2 = XLOG2DEV(points[i+1].x + xoffset);
        long y1 = YLOG2DEV(points[i].y + yoffset);     // oh, what a waste
        long y2 = YLOG2DEV(points[i+1].y + yoffset);
	if (m_window)
            gdk_draw_line( m_window, m_penGC, x1, y1, x2, y2 );
        
        CalcBoundingBox( points[i+1].x + xoffset, points[i+1].y + yoffset );
    }
}

void wxWindowDC::DoDrawPolygon( int n, wxPoint points[], long xoffset, long yoffset, int WXUNUSED(fillStyle) )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (n <= 0) return;
    
    GdkPoint *gdkpoints = new GdkPoint[n+1];
    int i;
    for (i = 0 ; i < n ; i++)
    {
        gdkpoints[i].x = XLOG2DEV(points[i].x + xoffset);
        gdkpoints[i].y = YLOG2DEV(points[i].y + yoffset);
        
        CalcBoundingBox( points[i].x + xoffset, points[i].y + yoffset );
    }
    
    if (m_brush.GetStyle() != wxTRANSPARENT)
        gdk_draw_polygon (m_window, m_brushGC, TRUE, gdkpoints, n);
     
    // To do: Fillstyle
   
    if ((m_pen.GetStyle() != wxTRANSPARENT) && m_window)
    {
        for (i = 0 ; i < n ; i++)
        {
            gdk_draw_line( m_window, m_penGC, 
                           gdkpoints[i%n].x,
                           gdkpoints[i%n].y,
                           gdkpoints[(i+1)%n].x,
                           gdkpoints[(i+1)%n].y);
        }
    }
        
    delete[] gdkpoints;
}

void wxWindowDC::DoDrawRectangle( long x, long y, long width, long height )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );

    long xx = XLOG2DEV(x);
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width);
    long hh = m_signY * YLOG2DEVREL(height);
    
    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
            gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
    
        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_rectangle( m_window, m_penGC, FALSE, xx, yy, ww-1, hh-1 );
    }
 
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawRoundedRectangle( long x, long y, long width, long height, double radius )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (radius < 0.0) radius = - radius * ((width < height) ? width : height);
  
    long xx = XLOG2DEV(x);    
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width); 
    long hh = m_signY * YLOG2DEVREL(height);
    long rr = XLOG2DEVREL((long)radius);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    // CMB: if radius is zero use DrawRectangle() instead to avoid
    // X drawing errors with small radii
    if (rr == 0)
    {
        DrawRectangle( x, y, width, height );
        return;
    }

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: adjust size if outline is drawn otherwise the result is
    // 1 pixel too wide and high
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        ww--;
        hh--;
    }

    if (m_window)
    {
        // CMB: ensure dd is not larger than rectangle otherwise we
        // get an hour glass shape
        long dd = 2 * rr;
        if (dd > ww) dd = ww;
        if (dd > hh) dd = hh;
        rr = dd / 2;

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx+rr, yy, ww-dd+1, hh );
            gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy+rr, ww, hh-dd+1 );
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, dd, dd, 90*64, 90*64 );
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
        }
  
        if (m_pen.GetStyle() != wxTRANSPARENT)
        {    
            gdk_draw_line( m_window, m_penGC, xx+rr, yy, xx+ww-rr, yy );
            gdk_draw_line( m_window, m_penGC, xx+rr, yy+hh, xx+ww-rr, yy+hh );
            gdk_draw_line( m_window, m_penGC, xx, yy+rr, xx, yy+hh-rr );
            gdk_draw_line( m_window, m_penGC, xx+ww, yy+rr, xx+ww, yy+hh-rr );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, dd, dd, 90*64, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx+ww-dd, yy, dd, dd, 0, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx+ww-dd, yy+hh-dd, dd, dd, 270*64, 90*64 );
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy+hh-dd, dd, dd, 180*64, 90*64 );
	}
    }
    
    // this ignores the radius
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawEllipse( long x, long y, long width, long height )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    long xx = XLOG2DEV(x);    
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width); 
    long hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
  
    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
  
        if (m_pen.GetStyle() != wxTRANSPARENT)
            gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, 0, 360*64 );
    }
        
    CalcBoundingBox( x - width, y - height );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawIcon( const wxIcon &icon, long x, long y )
{
    // VZ: egcs 1.0.3 refuses to compile this without cast, no idea why
    DoDrawBitmap( (const wxBitmap&)icon, x, y, (bool)TRUE );
}

void wxWindowDC::DoDrawBitmap( const wxBitmap &bitmap,
                               long x, long y,
                               bool useMask )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    wxCHECK_RET( bitmap.Ok(), _T("invalid bitmap") );
    
    /* scale/translate size and position */
  
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);
  
    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();
    
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );
    
    if (!m_window) return;
    
    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);
    
    /* scale bitmap if required */
    
    wxBitmap use_bitmap;
    
    if ((w != ww) || (h != hh))
    {
        wxImage image( bitmap );
        image = image.Scale( ww, hh );
        
        use_bitmap = image.ConvertToBitmap();
    }
    else
    {
        use_bitmap = bitmap;
    }
    
    /* apply mask if any */
    
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();
    
    if (useMask && mask) 
    {
        gdk_gc_set_clip_mask( m_penGC, mask );
        gdk_gc_set_clip_origin( m_penGC, xx, yy );
    }
  
    /* draw XPixmap or XBitmap, depending on what the wxBitmap contains */
    
    GdkPixmap *pm = use_bitmap.GetPixmap();
    if (pm)
    {
        gdk_draw_pixmap( m_window, m_penGC, pm, 0, 0, xx, yy, -1, -1 );
    }
    else
    {
        GdkBitmap *bm = use_bitmap.GetBitmap();
        if (bm)
        {
            gdk_draw_bitmap( m_window, m_penGC, bm, 0, 0, xx, yy, -1, -1 );
        }
    }
    
    /* remove mask again if any */
    
    if (useMask && mask) 
    {
        gdk_gc_set_clip_mask( m_penGC, (GdkBitmap *) NULL );
        gdk_gc_set_clip_origin( m_penGC, 0, 0 );
    }
}

bool wxWindowDC::DoBlit( long xdest, long ydest, long width, long height,
                         wxDC *source, long xsrc, long ysrc,
                         int logical_func, bool useMask )
{
   /* this is the nth try to get this utterly useless function to
      work. it now completely ignores the scaling or translation
      of the source dc, but scales correctly on the target dc and
      knows about possible mask information in a memory dc. */

    wxCHECK_MSG( Ok(), FALSE, _T("invalid window dc") );
    
    wxCHECK_MSG( source, FALSE, _T("invalid source dc") );
    
    if (!m_window) return FALSE;
    
    wxClientDC *srcDC = (wxClientDC*)source;
    wxMemoryDC *memDC = (wxMemoryDC*)source;
  
    bool use_bitmap_method = FALSE;
  
    if (srcDC->m_isMemDC)
    {
        if (!memDC->m_selected.Ok()) return FALSE;
        
        /* we use the "XCopyArea" way to copy a memory dc into
           y different window if the memory dc BOTH
           a) doesn't have any mask or its mask isn't used
           b) it is clipped
           c) is not 1-bit */
    
        if (useMask && (memDC->m_selected.GetMask()))
        {
           /* we HAVE TO use the direct way for memory dcs
              that have mask since the XCopyArea doesn't know
              about masks */
            use_bitmap_method = TRUE;
        }
        else if (memDC->m_selected.GetDepth() == 1)
        {
           /* we HAVE TO use the direct way for memory dcs
              that are bitmaps because XCopyArea doesn't cope
              with different bit depths */
            use_bitmap_method = TRUE;
        }
        else if ((xsrc == 0) && (ysrc == 0) &&
                 (width == memDC->m_selected.GetWidth()) &&
                 (height == memDC->m_selected.GetHeight()))
        {
           /* we SHOULD use the direct way if all of the bitmap 
              in the memory dc is copied in which case XCopyArea 
              wouldn't be able able to boost performace by reducing 
              the area to be scaled */
            use_bitmap_method = TRUE;
        }
        else
        {
            use_bitmap_method = FALSE;
        }
    }
    
    CalcBoundingBox( xdest, ydest );
    CalcBoundingBox( xdest + width, ydest + height );
    
    int old_logical_func = m_logicalFunction;
    SetLogicalFunction( logical_func );
    
    if (use_bitmap_method)
    {
        /* scale/translate bitmap size */
  
        long bm_width = memDC->m_selected.GetWidth();
        long bm_height = memDC->m_selected.GetHeight();
        
        long bm_ww = XLOG2DEVREL( bm_width );
        long bm_hh = YLOG2DEVREL( bm_height );
        
        /* scale bitmap if required */
    
        wxBitmap use_bitmap;
    
        if ((bm_width != bm_ww) || (bm_height != bm_hh))
        {
            wxImage image( memDC->m_selected );
            image = image.Scale( bm_ww, bm_hh );
        
            use_bitmap = image.ConvertToBitmap();
        }
        else
        {
            use_bitmap = memDC->m_selected;
        }
        
        /* scale/translate size and position */
  
        long xx = XLOG2DEV(xdest);
        long yy = YLOG2DEV(ydest);
        
        long ww = XLOG2DEVREL(width);
        long hh = YLOG2DEVREL(height);
        
        /* apply mask if any */
    
        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();
    
        if (useMask && mask) 
        {
            gdk_gc_set_clip_mask( m_penGC, mask );
            gdk_gc_set_clip_origin( m_penGC, xx, yy );
        }
        
        /* draw XPixmap or XBitmap, depending on what the wxBitmap contains */
    
        GdkPixmap *pm = use_bitmap.GetPixmap();
        if (pm)
        {
            gdk_draw_pixmap( m_window, m_penGC, pm, xsrc, ysrc, xx, yy, ww, hh );
        }
        else
        {
            GdkBitmap *bm = use_bitmap.GetBitmap();
            if (bm)
            {
                /* we use the textGC here because blitting a bitmap is done
                   using the current text colour */
                gdk_draw_bitmap( m_window, m_textGC, bm, xsrc, ysrc, xx, yy, ww, hh );
            }
        }
    
        /* remove mask again if any */
    
        if (useMask && mask) 
        {
            gdk_gc_set_clip_mask( m_penGC, (GdkBitmap *) NULL );
            gdk_gc_set_clip_origin( m_penGC, 0, 0 );
        }
    }
    else /* use_bitmap_method */
    {
        /* scale/translate size and position */
  
        long xx = XLOG2DEV(xdest);
        long yy = YLOG2DEV(ydest);
        
        long ww = XLOG2DEVREL(width);
        long hh = YLOG2DEVREL(height);
        
        if ((width != ww) || (height != hh))
        {
            /* draw source window into a bitmap as we cannot scale
               a window in contrast to a bitmap. this would actually
               work with memory dcs as well, but we'd lose the mask
               information and waste one step in this process since
               a memory already has a bitmap. all this is slightly
               inefficient as we could take an XImage directly from
               an X window, but we'd then also have to care that
               the window is not outside the screen (in which case
               we'd get a BadMatch or what not).
               Is a double XGetImage and combined XGetPixel and
               XPutPixel really faster? I'm not sure. look at wxXt
               for a different implementation of the same problem. */
   
            wxBitmap bitmap( width, height );
            gdk_window_copy_area( bitmap.GetPixmap(), m_penGC, 0, 0, 
                                  srcDC->GetWindow(),
                                  xsrc, ysrc, width, height );
            
            /* scale image */
            
            wxImage image( bitmap );
            image = image.Scale( ww, hh );
            
            /* convert to bitmap */
            
            bitmap = image.ConvertToBitmap();
            
            /* draw scaled bitmap */
            
            gdk_draw_pixmap( m_window, m_penGC, bitmap.GetPixmap(), 0, 0, xx, yy, -1, -1 );
            
        }
        else
        {
            /* no scaling and not a memory dc with a mask either */
        
            gdk_window_copy_area( m_window, m_penGC, xx, yy,
                                  srcDC->GetWindow(),
                                  xsrc, ysrc, width, height );
        }
    }

    SetLogicalFunction( old_logical_func );
    return TRUE;
}

void wxWindowDC::DoDrawText( const wxString &text, long x, long y )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );

    if (!m_window) return;
    
    GdkFont *font = m_font.GetInternalFont( m_scaleY );

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);

    /* CMB 21/5/98: draw text background if mode is wxSOLID */
    if (m_backgroundMode == wxSOLID)
    {
        long width = gdk_string_width( font, text.mbc_str() );
        long height = font->ascent + font->descent;
        gdk_gc_set_foreground( m_textGC, m_textBackgroundColour.GetColor() );
        gdk_draw_rectangle( m_window, m_textGC, TRUE, x, y, width, height );
        gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
    }
    gdk_draw_string( m_window, font, m_textGC, x, y + font->ascent, text.mbc_str() );

    /* CMB 17/7/98: simple underline: ignores scaling and underlying
       X font's XA_UNDERLINE_POSITION and XA_UNDERLINE_THICKNESS
       properties (see wxXt implementation) */
    if (m_font.GetUnderlined())
    {
        long width = gdk_string_width( font, text.mbc_str() );
        long ul_y = y + font->ascent;
        if (font->descent > 0) ul_y++;
        gdk_draw_line( m_window, m_textGC, x, ul_y, x + width, ul_y);
    }
    
    long w, h;
    GetTextExtent (text, &w, &h);
    CalcBoundingBox (x + w, y + h);
    CalcBoundingBox (x, y);
}

void wxWindowDC::GetTextExtent( const wxString &string, long *width, long *height,
                                long *descent, long *externalLeading,
                                wxFont *theFont ) const
{
    wxFont fontToUse = m_font;
    if (theFont) fontToUse = *theFont;
  
    GdkFont *font = fontToUse.GetInternalFont( m_scaleY );
    if (width) (*width) = long(gdk_string_width( font, string.mbc_str() ) / m_scaleX);
    if (height) (*height) = long((font->ascent + font->descent) / m_scaleY);
    if (descent) (*descent) = long(font->descent / m_scaleY);
    if (externalLeading) (*externalLeading) = 0;  // ??
}

long wxWindowDC::GetCharWidth() const
{
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    return long(gdk_string_width( font, "H" ) / m_scaleX);
}

long wxWindowDC::GetCharHeight() const
{
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    return long((font->ascent + font->descent) / m_scaleY);
}

void wxWindowDC::Clear()
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (!m_window) return;
    
    /* - we either are a memory dc or have a window as the
       owner. anything else shouldn't happen.
       - we don't use gdk_window_clear() as we don't set
       the window's background colour anymore. it is too
       much pain to keep the DC's and the window's back-
       ground colour in synch. */
  
    if (m_owner)
    {
        int width,height;
        m_owner->GetSize( &width, &height );
        gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
        return;
    }

    if (m_isMemDC)
    {
        int width,height;
        GetSize( &width, &height );
        gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
        return;
    }
}

void wxWindowDC::SetFont( const wxFont &font )
{
    m_font = font;
}

void wxWindowDC::SetPen( const wxPen &pen )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_pen == pen) return;
  
    m_pen = pen;
  
    if (!m_pen.Ok()) return;
  
    if (!m_window) return;
    
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
        double w = 0.5 + (abs(XLOG2DEVREL(width)) + abs(YLOG2DEVREL(width))) / 2.0;
        width = (int)w;
    }
  
    const static char dotted[] = {1, 1};
    const static char short_dashed[] = {2, 2};
    const static char long_dashed[] = {2, 4};
    const static char dotted_dashed[] = {3, 3, 1, 3};
    
    // We express dash pattern in pen width unit, so we are
    // independent of zoom factor and so on...
    int req_nb_dash;
    const char *req_dash;
    
    GdkLineStyle lineStyle = GDK_LINE_SOLID;
    switch (m_pen.GetStyle())
    {
        case wxUSER_DASH:
	{
	    lineStyle = GDK_LINE_ON_OFF_DASH; 
            req_nb_dash = m_pen.GetDashCount();
            req_dash = m_pen.GetDash();
            break;
	}
        case wxDOT:        
	{ 
	    lineStyle = GDK_LINE_ON_OFF_DASH; 
            req_nb_dash = 2;
            req_dash = dotted;
	    break; 
	}
        case wxLONG_DASH:  
	{ 
	    lineStyle = GDK_LINE_ON_OFF_DASH; 
            req_nb_dash = 2;
            req_dash = long_dashed;
	    break; 
	}
        case wxSHORT_DASH: 
	{ 
	    lineStyle = GDK_LINE_ON_OFF_DASH; 
            req_nb_dash = 2;
            req_dash = short_dashed;
	    break; 
	}
        case wxDOT_DASH:   
	{ 
//	    lineStyle = GDK_LINE_DOUBLE_DASH; 
	    lineStyle = GDK_LINE_ON_OFF_DASH; 
            req_nb_dash = 4;
            req_dash = dotted_dashed;
	    break; 
	}
	
	case wxTRANSPARENT:
	case wxSTIPPLE:
        case wxSOLID: 
	default:     
	{ 
	    lineStyle = GDK_LINE_SOLID;
            req_dash = (wxDash*)NULL;
            req_nb_dash = 0;
	    break; 
	}
    }
  
#if (GTK_MINOR_VERSION > 0)
    if (req_dash && req_nb_dash)
    {
        char *real_req_dash = new char[req_nb_dash];
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
	    gdk_gc_set_dashes( m_penGC, 0, (char*)req_dash, req_nb_dash );
        }
    }
#endif
	
    GdkCapStyle capStyle = GDK_CAP_ROUND;
    switch (m_pen.GetCap())
    {
        case wxCAP_ROUND:      { capStyle = (width <= 1) ? GDK_CAP_NOT_LAST : GDK_CAP_ROUND; break; }
        case wxCAP_PROJECTING: { capStyle = GDK_CAP_PROJECTING; break; }
        case wxCAP_BUTT:       { capStyle = GDK_CAP_BUTT;       break; }
    }
  
    GdkJoinStyle joinStyle = GDK_JOIN_ROUND;
    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL: { joinStyle = GDK_JOIN_BEVEL; break; }
        case wxJOIN_ROUND: { joinStyle = GDK_JOIN_ROUND; break; }
        case wxJOIN_MITER: { joinStyle = GDK_JOIN_MITER; break; }
    }
  
    gdk_gc_set_line_attributes( m_penGC, width, lineStyle, capStyle, joinStyle );
  
    m_pen.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_penGC, m_pen.GetColour().GetColor() );
}

void wxWindowDC::SetBrush( const wxBrush &brush )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_brush == brush) return;
  
    m_brush = brush;
  
    if (!m_brush.Ok()) return;
  
    if (!m_window) return;
    
    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );
  
    gdk_gc_set_fill( m_brushGC, GDK_SOLID );
  
    if ((m_brush.GetStyle() == wxSTIPPLE) && (m_brush.GetStipple()->Ok()))
    {
        if (m_brush.GetStipple()->GetPixmap())
	{
            gdk_gc_set_fill( m_brushGC, GDK_TILED );
            gdk_gc_set_tile( m_brushGC, m_brush.GetStipple()->GetPixmap() );
	}
        else
	{
            gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
            gdk_gc_set_stipple( m_brushGC, m_brush.GetStipple()->GetBitmap() );
	}
    }
  
    if (IS_HATCH(m_brush.GetStyle()))
    {
        gdk_gc_set_fill( m_brushGC, GDK_STIPPLED );
        int num = m_brush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_brushGC, hatches[num] );
    }
}

void wxWindowDC::SetBackground( const wxBrush &brush )
{
   /* CMB 21/7/98: Added SetBackground. Sets background brush
    * for Clear() and bg colour for shapes filled with cross-hatch brush */
   
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_backgroundBrush == brush) return;
  
    m_backgroundBrush = brush;
  
    if (!m_backgroundBrush.Ok()) return;
  
    if (!m_window) return;
    
    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_background( m_brushGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_background( m_penGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_background( m_bgGC, m_backgroundBrush.GetColour().GetColor() );
    gdk_gc_set_foreground( m_bgGC, m_backgroundBrush.GetColour().GetColor() );
  
    GdkFill fillStyle = GDK_SOLID;
    switch (m_backgroundBrush.GetStyle())
    {
        case wxSOLID:
        case wxTRANSPARENT:
            break;
        default:
            fillStyle = GDK_STIPPLED;
    }
 
    gdk_gc_set_fill( m_bgGC, fillStyle );
  
    if ((m_backgroundBrush.GetStyle() == wxSTIPPLE) && (m_backgroundBrush.GetStipple()->Ok()))
    {
        if (m_backgroundBrush.GetStipple()->GetPixmap())
            gdk_gc_set_tile( m_bgGC, m_backgroundBrush.GetStipple()->GetPixmap() );
        else
            gdk_gc_set_stipple( m_bgGC, m_backgroundBrush.GetStipple()->GetBitmap() );
    }
  
    if (IS_HATCH(m_backgroundBrush.GetStyle()))
    {
        int num = m_backgroundBrush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_bgGC, hatches[num] );
    }
}

void wxWindowDC::SetLogicalFunction( int function )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_logicalFunction == function) return;

    GdkFunction mode = GDK_COPY;
    switch (function)
    {
        case wxXOR:          mode = GDK_XOR;           break;
        case wxINVERT:       mode = GDK_INVERT;        break;
#if (GTK_MINOR_VERSION > 0)
        case wxOR_REVERSE:   mode = GDK_OR_REVERSE;    break;
        case wxAND_REVERSE:  mode = GDK_AND_REVERSE;   break;
        case wxCLEAR:        mode = GDK_CLEAR;         break;
        case wxSET:          mode = GDK_SET;           break;
        case wxOR_INVERT:    mode = GDK_OR_INVERT;     break;
	case wxSRC_AND:
        case wxAND:          mode = GDK_AND;           break;
	case wxSRC_OR:
        case wxOR:           mode = GDK_OR;            break;
        case wxEQUIV:        mode = GDK_EQUIV;         break;
        case wxNAND:         mode = GDK_NAND;          break;
        case wxAND_INVERT:   mode = GDK_AND_INVERT;    break;
	case wxCOPY:         mode = GDK_COPY;          break;
	case wxNO_OP:        mode = GDK_NOOP;          break;
	case wxSRC_INVERT:   mode = GDK_COPY_INVERT;   break;
#endif
        default:
	{
	   wxFAIL_MSG( _T("unsupported logical function") );
	   break;
	}
    }
    
    m_logicalFunction = function;
    
    if (!m_window) return;
    
    gdk_gc_set_function( m_penGC, mode );
    gdk_gc_set_function( m_brushGC, mode );
    gdk_gc_set_function( m_textGC, mode );
}

void wxWindowDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_textForegroundColour == col) return;
  
    m_textForegroundColour = col;
    if (!m_textForegroundColour.Ok()) return;
  
    if (!m_window) return;
    
    m_textForegroundColour.CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
}

void wxWindowDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (m_textBackgroundColour == col) return;
  
    m_textBackgroundColour = col;
    if (!m_textBackgroundColour.Ok()) return;
  
    if (!m_window) return;
    
    m_textBackgroundColour.CalcPixel( m_cmap );
    gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );
}

void wxWindowDC::SetBackgroundMode( int mode )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    m_backgroundMode = mode;

    if (!m_window) return;
    
    // CMB 21/7/98: fill style of cross-hatch brushes is affected by
    // transparent/solid background mode
    
    if (m_brush.GetStyle() != wxSOLID && m_brush.GetStyle() != wxTRANSPARENT)
    {
        gdk_gc_set_fill( m_brushGC,
          (m_backgroundMode == wxTRANSPARENT) ? GDK_STIPPLED : GDK_OPAQUE_STIPPLED);
    }
}

void wxWindowDC::SetPalette( const wxPalette& WXUNUSED(palette) )
{
    wxFAIL_MSG( _T("wxWindowDC::SetPalette not implemented") );
}

void wxWindowDC::DoSetClippingRegion( long x, long y, long width, long height )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    wxDC::DoSetClippingRegion( x, y, width, height );
  
    if (!m_window) return;
    
    GdkRectangle rect;
    rect.x = XLOG2DEV(x);
    rect.y = YLOG2DEV(y);
    rect.width = XLOG2DEVREL(width);
    rect.height = YLOG2DEVREL(height);
    gdk_gc_set_clip_rectangle( m_penGC, &rect );
    gdk_gc_set_clip_rectangle( m_brushGC, &rect );
    gdk_gc_set_clip_rectangle( m_textGC, &rect );
    gdk_gc_set_clip_rectangle( m_bgGC, &rect );
}

void wxWindowDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }
    
    if (!m_window) return;
    
    gdk_gc_set_clip_region( m_penGC, region.GetRegion() );
    gdk_gc_set_clip_region( m_brushGC, region.GetRegion() );
    gdk_gc_set_clip_region( m_textGC, region.GetRegion() );
    gdk_gc_set_clip_region( m_bgGC, region.GetRegion() );
}

void wxWindowDC::DestroyClippingRegion()
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    wxDC::DestroyClippingRegion();
  
    if (!m_window) return;
    
    gdk_gc_set_clip_rectangle( m_penGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_brushGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_textGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_bgGC, (GdkRectangle *) NULL );
}

void wxWindowDC::SetUpDC()
{
    Destroy();
    m_ok = TRUE;
    m_logicalFunction = wxCOPY;
    m_penGC = gdk_gc_new( m_window );
    m_brushGC = gdk_gc_new( m_window );
    m_textGC = gdk_gc_new( m_window );
    m_bgGC = gdk_gc_new( m_window );
    
    wxColour tmp_col( m_textForegroundColour );
    m_textForegroundColour = wxNullColour;
    SetTextForeground( tmp_col );
    tmp_col = m_textBackgroundColour;
    m_textBackgroundColour = wxNullColour;
    SetTextBackground( tmp_col );
    
    wxPen tmp_pen( m_pen );
    m_pen = wxNullPen;
    SetPen( tmp_pen );
    
    wxFont tmp_font( m_font );
    m_font = wxNullFont;
    SetFont( tmp_font );
    
    wxBrush tmp_brush( m_brush );
    m_brush = wxNullBrush;
    SetBrush( tmp_brush );
    
/*
    tmp_brush = m_backgroundBrush;
    m_backgroundBrush = wxNullBrush;
    SetBackground( tmp_brush );
*/
    tmp_brush = m_backgroundBrush;
    m_backgroundBrush = wxNullBrush;
    SetBackground( *wxWHITE_BRUSH );
    m_backgroundBrush = tmp_brush;
  
    if (!hatch_bitmap) 
    {
        hatch_bitmap    = hatches;
        hatch_bitmap[0] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, bdiag_bits, bdiag_width, bdiag_height );
        hatch_bitmap[1] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, cdiag_bits, cdiag_width, cdiag_height );
        hatch_bitmap[2] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, fdiag_bits, fdiag_width, fdiag_height );
        hatch_bitmap[3] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, cross_bits, cross_width, cross_height );
        hatch_bitmap[4] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, horiz_bits, horiz_width, horiz_height );
        hatch_bitmap[5] = gdk_bitmap_create_from_data( (GdkWindow *) NULL, verti_bits, verti_width, verti_height );
    }
}

void wxWindowDC::Destroy()
{
    if (m_penGC) gdk_gc_unref( m_penGC );
    m_penGC = (GdkGC*) NULL;
    if (m_brushGC) gdk_gc_unref( m_brushGC );
    m_brushGC = (GdkGC*) NULL;
    if (m_textGC) gdk_gc_unref( m_textGC );
    m_textGC = (GdkGC*) NULL;
    if (m_bgGC) gdk_gc_unref( m_bgGC );
    m_bgGC = (GdkGC*) NULL;
}

void wxWindowDC::ComputeScaleAndOrigin()
{
    /* CMB: copy scale to see if it changes */
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;

    wxDC::ComputeScaleAndOrigin();

    /* CMB: if scale has changed call SetPen to recalulate the line width */
    if ((m_scaleX != origScaleX || m_scaleY != origScaleY) &&
        (m_pen.Ok()))
    {
      /* this is a bit artificial, but we need to force wxDC to think
         the pen has changed */
      wxPen pen = m_pen;
      m_pen = wxNullPen;
      SetPen( pen );
  }
}

// Resolution in pixels per logical inch
wxSize wxWindowDC::GetPPI() const
{
    return wxSize(100, 100);
}

int wxWindowDC::GetDepth() const
{
    wxFAIL_MSG(_T("not implemented"));

    return -1;
}

#if wxUSE_SPLINES
// ----------------------------------- spline code ----------------------------------------

void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
void wx_clear_stack();
int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);

wxList wx_spline_point_list;

#define                half(z1, z2)        ((z1+z2)/2.0)
#define                THRESHOLD        5

/* iterative version */

void wx_quadratic_spline(double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                 double b4)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;

    wx_clear_stack();
    wx_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (wx_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
        xmid = (double)half(x2, x3);
        ymid = (double)half(y2, y3);
        if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD &&
            fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
            wx_spline_add_point( x1, y1 );
            wx_spline_add_point( xmid, ymid );
        } else {
            wx_spline_push(xmid, ymid, (double)half(xmid, x3), (double)half(ymid, y3),
                 (double)half(x3, x4), (double)half(y3, y4), x4, y4);
            wx_spline_push(x1, y1, (double)half(x1, x2), (double)half(y1, y2),
                 (double)half(x2, xmid), (double)half(y2, ymid), xmid, ymid);
        }
    }
}

/* utilities used by spline drawing routines */

typedef struct wx_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
} Stack;

#define         SPLINE_STACK_DEPTH             20
static Stack    wx_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *wx_stack_top;
static int      wx_stack_count;

void wx_clear_stack()
{
    wx_stack_top = wx_spline_stack;
    wx_stack_count = 0;
}

void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    wx_stack_top->x1 = x1;
    wx_stack_top->y1 = y1;
    wx_stack_top->x2 = x2;
    wx_stack_top->y2 = y2;
    wx_stack_top->x3 = x3;
    wx_stack_top->y3 = y3;
    wx_stack_top->x4 = x4;
    wx_stack_top->y4 = y4;
    wx_stack_top++;
    wx_stack_count++;
}

int wx_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (wx_stack_count == 0)
        return (0);
    wx_stack_top--;
    wx_stack_count--;
    *x1 = wx_stack_top->x1;
    *y1 = wx_stack_top->y1;
    *x2 = wx_stack_top->x2;
    *y2 = wx_stack_top->y2;
    *x3 = wx_stack_top->x3;
    *y3 = wx_stack_top->y3;
    *x4 = wx_stack_top->x4;
    *y4 = wx_stack_top->y4;
    return (1);
}

static bool wx_spline_add_point(double x, double y)
{
  wxPoint *point = new wxPoint ;
  point->x = (int) x;
  point->y = (int) y;
  wx_spline_point_list.Append((wxObject*)point);
  return TRUE;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
  dc->DrawLines(&wx_spline_point_list, 0, 0 );
  wxNode *node = wx_spline_point_list.First();
  while (node)
  {
    wxPoint *point = (wxPoint *)node->Data();
    delete point;
    delete node;
    node = wx_spline_point_list.First();
  }
}

void wxWindowDC::DoDrawSpline( wxList *points )
{
    wxCHECK_RET( Ok(), _T("invalid window dc") );
  
    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxNode *node = points->First();
    p = (wxPoint *)node->Data();

    x1 = p->x;
    y1 = p->y;

    node = node->Next();
    p = (wxPoint *)node->Data();

    x2 = p->x;
    y2 = p->y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ((node = node->Next()) != NULL)
    {
        p = (wxPoint *)node->Data();
        x1 = x2;
        y1 = y2;
        x2 = p->x;
        y2 = p->y;
        cx4 = (double)(x1 + x2) / 2;
        cy4 = (double)(y1 + y2) / 2;
        cx3 = (double)(x1 + cx4) / 2;
        cy3 = (double)(y1 + cy4) / 2;

        wx_quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);

        cx1 = cx4;
        cy1 = cy4;
        cx2 = (double)(cx1 + x2) / 2;
        cy2 = (double)(cy1 + y2) / 2;
    }

    wx_spline_add_point( cx1, cy1 );
    wx_spline_add_point( x2, y2 );

    wx_spline_draw_point_array( this );
}

#endif // wxUSE_SPLINE

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC,wxWindowDC)

wxPaintDC::wxPaintDC()
  : wxWindowDC()
{
}

wxPaintDC::wxPaintDC( wxWindow *win )
  : wxWindowDC( win )
{
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC,wxWindowDC)

wxClientDC::wxClientDC()
  : wxWindowDC()
{
}

wxClientDC::wxClientDC( wxWindow *win )
  : wxWindowDC( win )
{
}

