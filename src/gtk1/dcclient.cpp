/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem, Chris Breeze
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include <math.h>

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
void gdk_draw_bitmap	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  GdkDrawable  *src,
			  gint		xsrc,
			  gint		ysrc,
			  gint		xdest,
			  gint		ydest,
			  gint		width,
			  gint		height)
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

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC,wxDC)

wxWindowDC::wxWindowDC(void)
{
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
    m_isMemDC = FALSE;
}

wxWindowDC::wxWindowDC( wxWindow *window )
{
    m_penGC = (GdkGC *) NULL;
    m_brushGC = (GdkGC *) NULL;
    m_textGC = (GdkGC *) NULL;
    m_bgGC = (GdkGC *) NULL;
    m_cmap = (GdkColormap *) NULL;
  
    if (!window) return;
    GtkWidget *widget = window->m_wxwindow;
    if (!widget) return;
    m_window = widget->window;
    if (!m_window) return;
    if (window->m_wxwindow)
        m_cmap = gtk_widget_get_colormap( window->m_wxwindow );
    else
        m_cmap = gtk_widget_get_colormap( window->m_widget );
    
    m_isMemDC = FALSE;
        
    SetUpDC();
    
    wxRegion update = window->GetUpdateRegion();
    if (update.Empty()) return;
    
    gdk_gc_set_clip_region( m_penGC, update.GetRegion() );
    gdk_gc_set_clip_region( m_brushGC, update.GetRegion() );
    gdk_gc_set_clip_region( m_textGC, update.GetRegion() );
    gdk_gc_set_clip_region( m_bgGC, update.GetRegion() );
}

wxWindowDC::~wxWindowDC(void)
{
    Destroy();
}

void wxWindowDC::FloodFill( long WXUNUSED(x1), long WXUNUSED(y1), 
                           wxColour *WXUNUSED(col), int WXUNUSED(style) )
{
    wxFAIL_MSG( "wxWindowDC::FloodFill not implemented" );
}

bool wxWindowDC::GetPixel( long WXUNUSED(x1), long WXUNUSED(y1), wxColour *WXUNUSED(col) ) const
{
    wxFAIL_MSG( "wxWindowDC::GetPixel not implemented" );
    return FALSE;
}

void wxWindowDC::DrawLine( long x1, long y1, long x2, long y2 )
{
    if (!Ok()) return;
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        gdk_draw_line( m_window, m_penGC, 
                       XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2) );
    }
    
    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxWindowDC::CrossHair( long x, long y )
{
    if (!Ok()) return;
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        long xx = XLOG2DEV(x);
        long yy = YLOG2DEV(y);
        gdk_draw_line( m_window, m_penGC, 0, yy, XLOG2DEVREL(w), yy );
        gdk_draw_line( m_window, m_penGC, xx, 0, xx, YLOG2DEVREL(h) );
    }
}

void wxWindowDC::DrawArc( long x1, long y1, long x2, long y2, double xc, double yc )
{
    if (!Ok()) return;
  
    long xx1 = XLOG2DEV(x1); 
    long yy1 = YLOG2DEV(y1);
    long xx2 = XLOG2DEV(x2); 
    long yy2 = YLOG2DEV(y2);
    long xxc = XLOG2DEV((long)xc); 
    long yyc = YLOG2DEV((long)yc);
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

    if (m_brush.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_brushGC, TRUE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
    
    if (m_pen.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_penGC, FALSE, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
      
    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDC::DrawEllipticArc( long x, long y, long width, long height, double sa, double ea )
{
    if (!Ok()) return;
  
    long xx = XLOG2DEV(x);    
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width); 
    long hh = m_signY * YLOG2DEVREL(height);
  
    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
  
    long start = long(sa * 64.0);
    long end = long(ea * 64.0);
    if (m_brush.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, start, end );
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, start, end );
	
    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDC::DrawPoint( long x, long y )
{
    if (!Ok()) return;
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
        gdk_draw_point( m_window, m_penGC, XLOG2DEV(x), YLOG2DEV(y) );
	
    CalcBoundingBox (x, y);
}

void wxWindowDC::DrawLines( int n, wxPoint points[], long xoffset, long yoffset )
{
    if (!Ok()) return;
  
    if (m_pen.GetStyle() == wxTRANSPARENT) return;
    if (n <= 0) return;
  
    CalcBoundingBox( points[0].x + xoffset, points[0].y + yoffset );
    
    for (int i = 0; i < n-1; i++)
    {
        long x1 = XLOG2DEV(points[i].x + xoffset);
        long x2 = XLOG2DEV(points[i+1].x + xoffset);
        long y1 = YLOG2DEV(points[i].y + yoffset);     // oh, what a waste
        long y2 = YLOG2DEV(points[i+1].y + yoffset);
        gdk_draw_line( m_window, m_penGC, x1, y1, x2, y2 );
	
        CalcBoundingBox( points[i+1].x + xoffset, points[i+1].y + yoffset );
    }
}

void wxWindowDC::DrawLines( wxList *points, long xoffset, long yoffset )
{
    if (!Ok()) return;
  
    if (m_pen.GetStyle() == wxTRANSPARENT) return;
  
    wxNode *node = points->First();
    if (!node) return;
    
    wxPoint *pt = (wxPoint*)node->Data();
    CalcBoundingBox( pt->x + xoffset, pt->y + yoffset );
	
    while (node->Next())
    {
        wxPoint *point = (wxPoint*)node->Data();
        wxPoint *npoint = (wxPoint*)node->Next()->Data();
        long x1 = XLOG2DEV(point->x + xoffset);
        long x2 = XLOG2DEV(npoint->x + xoffset);
        long y1 = YLOG2DEV(point->y + yoffset);    // and a waste again...
        long y2 = YLOG2DEV(npoint->y + yoffset);
        gdk_draw_line( m_window, m_penGC, x1, y1, x2, y2 );
        node = node->Next();
	
        CalcBoundingBox( npoint->x + xoffset, npoint->y + yoffset );
    }
}

void wxWindowDC::DrawPolygon( int n, wxPoint points[], long xoffset, long yoffset, int WXUNUSED(fillStyle) )
{
    if (!Ok()) return;
  
    if (!n) return;
    
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
   
    if (m_pen.GetStyle() != wxTRANSPARENT)
        for (i = 0 ; i < n ; i++)
	{
            gdk_draw_line( m_window, m_penGC, 
 		           gdkpoints[i%n].x,
 		           gdkpoints[i%n].y,
 		           gdkpoints[(i+1)%n].x,
 		           gdkpoints[(i+1)%n].y);
        }
	
    delete[] gdkpoints;
}

void wxWindowDC::DrawPolygon( wxList *lines, long xoffset, long yoffset, int WXUNUSED(fillStyle))
{
    if (!Ok()) return;
  
    int n = lines->Number();
    if (!n) return;
    
    GdkPoint *gdkpoints = new GdkPoint[n];
    wxNode *node = lines->First();
    int cnt = 0;
    while (node)
    {
        wxPoint *p = (wxPoint *) node->Data();
        gdkpoints[cnt].x = XLOG2DEV(p->x + xoffset);
        gdkpoints[cnt].y = YLOG2DEV(p->y + yoffset);
        node = node->Next();
        cnt++;
	
        CalcBoundingBox( p->x + xoffset, p->y + yoffset );
    }
    
    if (m_brush.GetStyle() != wxTRANSPARENT)
        gdk_draw_polygon (m_window, m_brushGC, TRUE, gdkpoints, n);
	
    // To do: Fillstyle
    
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int i;
        for (i = 0 ; i < n ; i++)
	{
 	    gdk_draw_line( m_window, m_penGC, 
 		           gdkpoints[i%n].x,
 		           gdkpoints[i%n].y,
 		           gdkpoints[(i+1)%n].x,
 		           gdkpoints[(i+1)%n].y );
        }
    }
    delete[] gdkpoints;
}

void wxWindowDC::DrawRectangle( long x, long y, long width, long height )
{
    if (!Ok()) return;

    long xx = XLOG2DEV(x);
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width);
    long hh = m_signY * YLOG2DEVREL(height);
    
    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_brush.GetStyle() != wxTRANSPARENT)
      gdk_draw_rectangle( m_window, m_brushGC, TRUE, xx, yy, ww, hh );
    
    if (m_pen.GetStyle() != wxTRANSPARENT)
      gdk_draw_rectangle( m_window, m_penGC, FALSE, xx, yy, ww-1, hh-1 );
 
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DrawRoundedRectangle( long x, long y, long width, long height, double radius )
{
    if (!Ok()) return;
  
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
    
    // this ignores the radius
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DrawEllipse( long x, long y, long width, long height )
{
    if (!Ok()) return;
  
    long xx = XLOG2DEV(x);    
    long yy = YLOG2DEV(y);
    long ww = m_signX * XLOG2DEVREL(width); 
    long hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
  
    if (m_brush.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_brushGC, TRUE, xx, yy, ww, hh, 0, 360*64 );
  
    if (m_pen.GetStyle() != wxTRANSPARENT)
        gdk_draw_arc( m_window, m_penGC, FALSE, xx, yy, ww, hh, 0, 360*64 );
	
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

bool wxWindowDC::CanDrawBitmap(void) const
{
  return TRUE;
}

void wxWindowDC::DrawIcon( const wxIcon &icon, long x, long y, bool useMask )
{
    if (!Ok()) return;
  
    if (!icon.Ok()) return;
  
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);
  
    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (icon.GetMask()) mask = icon.GetMask()->GetBitmap();
    
    if (useMask && mask) 
    {
        gdk_gc_set_clip_mask( m_penGC, mask );
        gdk_gc_set_clip_origin( m_penGC, xx, yy );
    }
  
    GdkPixmap *pm = icon.GetPixmap();
    gdk_draw_pixmap( m_window, m_penGC, pm, 0, 0, xx, yy, -1, -1 );
  
    if (useMask && mask) 
    {
        gdk_gc_set_clip_mask( m_penGC, (GdkBitmap *) NULL );
        gdk_gc_set_clip_origin( m_penGC, 0, 0 );
    }
    
    CalcBoundingBox( x, y );
    int width = icon.GetWidth();
    int height = icon.GetHeight();
    CalcBoundingBox( x + width, y + height );
}

bool wxWindowDC::Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int WXUNUSED(logical_func), bool useMask )
{
    if (!Ok()) return FALSE;
    
    CalcBoundingBox( xdest, ydest );
    CalcBoundingBox( xdest + width, ydest + height );
    
    wxClientDC *csrc = (wxClientDC*)source;
  
    if (csrc->m_isMemDC)
    {
        wxMemoryDC* srcDC = (wxMemoryDC*)source;
        GdkPixmap* pmap = srcDC->m_selected.GetPixmap();
        if (pmap)
        {
            long xx = XLOG2DEV(xdest);
            long yy = YLOG2DEV(ydest);
    
            GdkBitmap *mask = (GdkBitmap *) NULL;
            if (srcDC->m_selected.GetMask()) mask = srcDC->m_selected.GetMask()->GetBitmap();
    
            if (useMask && mask) 
            {
                gdk_gc_set_clip_mask( m_textGC, mask );
                gdk_gc_set_clip_origin( m_textGC, xx, yy );
            }
	    
            gdk_draw_pixmap( m_window, m_textGC, pmap,
                             source->DeviceToLogicalX(xsrc), 
	                     source->DeviceToLogicalY(ysrc),
                             xx, 
	                     yy,
                             source->DeviceToLogicalXRel(width), 
	                     source->DeviceToLogicalYRel(height) );
	  
            if (useMask && mask) 
            {
                gdk_gc_set_clip_mask( m_textGC, (GdkBitmap *) NULL );
                gdk_gc_set_clip_origin( m_textGC, 0, 0 );
            }
      
            return TRUE;
        }
	
        GdkBitmap* bmap = srcDC->m_selected.GetBitmap();
        if (bmap)
        {
            long xx = XLOG2DEV(xdest);
            long yy = YLOG2DEV(ydest);
    
            GdkBitmap *mask = (GdkBitmap *) NULL;
            if (srcDC->m_selected.GetMask()) mask = srcDC->m_selected.GetMask()->GetBitmap();
    
            if (useMask && mask) 
            {
                gdk_gc_set_clip_mask( m_textGC, mask );
                gdk_gc_set_clip_origin( m_textGC, xx, yy );
            }
  
            gdk_draw_bitmap( m_window, m_textGC, bmap,
                             source->DeviceToLogicalX(xsrc), 
	                     source->DeviceToLogicalY(ysrc),
                             xx, 
	                     yy,
                             source->DeviceToLogicalXRel(width), 
	                     source->DeviceToLogicalYRel(height) );
	  
            if (useMask && mask) 
            {
                gdk_gc_set_clip_mask( m_textGC, (GdkBitmap *) NULL );
                gdk_gc_set_clip_origin( m_textGC, 0, 0 );
            }
      
            return TRUE;
        }
    }

    gdk_window_copy_area ( m_window, m_textGC,
                           XLOG2DEV(xdest), YLOG2DEV(ydest),
                           csrc->GetWindow(),
                           source->DeviceToLogicalX(xsrc), 
			   source->DeviceToLogicalY(ysrc),
                           source->DeviceToLogicalXRel(width), 
			   source->DeviceToLogicalYRel(height) );

/*    
    gdk_window_copy_area ( m_window, m_textGC,
                           XLOG2DEV(xdest), YLOG2DEV(ydest),
                           csrc->GetWindow(),
                           xsrc, ysrc,
                           width, height );
*/
    
  return TRUE;
}

void wxWindowDC::DrawText( const wxString &text, long x, long y, bool WXUNUSED(use16) )
{
    if (!Ok()) return;

    GdkFont *font = m_font.GetInternalFont( m_scaleY );

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);

    // CMB 21/5/98: draw text background if mode is wxSOLID
    if (m_backgroundMode == wxSOLID)
    {
        long width = gdk_string_width( font, text );
        long height = font->ascent + font->descent;
        gdk_gc_set_foreground( m_textGC, m_textBackgroundColour.GetColor() );
        gdk_draw_rectangle( m_window, m_textGC, TRUE, x, y, width, height );
        gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
    }
    gdk_draw_string( m_window, font, m_textGC, x, y + font->ascent, text );

    // CMB 17/7/98: simple underline: ignores scaling and underlying
    // X font's XA_UNDERLINE_POSITION and XA_UNDERLINE_THICKNESS
    // properties (see wxXt implementation)
    if (m_font.GetUnderlined())
    {
        long width = gdk_string_width( font, text );
        long ul_y = y + font->ascent;
        if (font->descent > 0) ul_y++;
        gdk_draw_line( m_window, m_textGC, x, ul_y, x + width, ul_y);
    }
    
    long w, h;
    GetTextExtent (text, &w, &h);
    CalcBoundingBox (x + w, y + h);
    CalcBoundingBox (x, y);
}

bool wxWindowDC::CanGetTextExtent(void) const
{
    return TRUE;
}

void wxWindowDC::GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent, long *externalLeading,
                     wxFont *theFont, bool WXUNUSED(use16) )
{
    if (!Ok()) return;
  
    wxFont fontToUse = m_font;
    if (theFont) fontToUse = *theFont;
  
    GdkFont *font = fontToUse.GetInternalFont( m_scaleY );
    if (width) (*width) = long(gdk_string_width( font, string ) / m_scaleX);
    if (height) (*height) = long((font->ascent + font->descent) / m_scaleY);
    if (descent) (*descent) = long(font->descent / m_scaleY);
    if (externalLeading) (*externalLeading) = 0;  // ??
}

long wxWindowDC::GetCharWidth(void)
{
    if (!Ok()) return 0;
  
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    return long(gdk_string_width( font, "H" ) / m_scaleX);
}

long wxWindowDC::GetCharHeight(void)
{
    if (!Ok()) return 0;
  
    GdkFont *font = m_font.GetInternalFont( m_scaleY );
    return long((font->ascent + font->descent) / m_scaleY);
}

void wxWindowDC::Clear(void)
{
    if (!Ok()) return;
  
    if (!m_isMemDC)
    {
        gdk_window_clear( m_window );
    }
    else
    {
        int width,height;
        GetSize( &width, &height );
        gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
    }
}

void wxWindowDC::SetFont( const wxFont &font )
{
    if (!Ok()) return;
  
    m_font = font;
}

void wxWindowDC::SetPen( const wxPen &pen )
{
    if (!Ok()) return;

    if (m_pen == pen) return;
  
    m_pen = pen;
  
    if (!m_pen.Ok()) return;
  
    gint width = m_pen.GetWidth();
    // CMB: if width is non-zero scale it with the dc
    if (width <= 0)
    {
        width = 1;
    }
    else
    {
        // X doesn't allow different width in x and y and so we take
        // the average
        double w = 0.5 + (abs(XLOG2DEVREL(width)) + abs(YLOG2DEVREL(width))) / 2.0;
        width = (int)w;
    }
  
    GdkLineStyle lineStyle = GDK_LINE_SOLID;
    switch (m_pen.GetStyle())
    {
        case wxSOLID:      { lineStyle = GDK_LINE_SOLID;       break; }
        case wxDOT:        { lineStyle = GDK_LINE_ON_OFF_DASH; break; }
        case wxLONG_DASH:  { lineStyle = GDK_LINE_ON_OFF_DASH; break; }
        case wxSHORT_DASH: { lineStyle = GDK_LINE_ON_OFF_DASH; break; }
        case wxDOT_DASH:   { lineStyle = GDK_LINE_DOUBLE_DASH; break; }
    }
  
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
    if (!Ok()) return;
  
    if (m_brush == brush) return;
  
    m_brush = brush;
  
    if (!m_brush.Ok()) return;
  
    m_brush.GetColour().CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_brushGC, m_brush.GetColour().GetColor() );
  
    GdkFill fillStyle = GDK_SOLID;
    switch (m_brush.GetStyle())
    {
      case wxSOLID:
      case wxTRANSPARENT:
         break;
      default:
         fillStyle = GDK_STIPPLED;
    }
  
    gdk_gc_set_fill( m_brushGC, fillStyle );
  
    if (m_brush.GetStyle() == wxSTIPPLE)
    {
        gdk_gc_set_stipple( m_brushGC, m_brush.GetStipple()->GetPixmap() );
    }
  
    if (IS_HATCH(m_brush.GetStyle()))
    {
        int num = m_brush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_brushGC, hatches[num] );
    }
}

// CMB 21/7/98: Added SetBackground. Sets background brush
// for Clear() and bg colour for shapes filled with cross-hatch brush
void wxWindowDC::SetBackground( const wxBrush &brush )
{
    if (!Ok()) return;
  
    if (m_backgroundBrush == brush) return;
  
    m_backgroundBrush = brush;
  
    if (!m_backgroundBrush.Ok()) return;
  
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
  
    if (m_backgroundBrush.GetStyle() == wxSTIPPLE)
    {
        gdk_gc_set_stipple( m_bgGC, m_backgroundBrush.GetStipple()->GetPixmap() );
    }
  
    if (IS_HATCH(m_backgroundBrush.GetStyle()))
    {
        int num = m_backgroundBrush.GetStyle() - wxBDIAGONAL_HATCH;
        gdk_gc_set_stipple( m_bgGC, hatches[num] );
     }
}

void wxWindowDC::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function) return;
    GdkFunction mode = GDK_COPY;
    switch (function)
    {
        case wxXOR:    mode = GDK_INVERT; break;
        case wxINVERT: mode = GDK_INVERT; break;
        default:       break;
    }
    m_logicalFunction = function;
    gdk_gc_set_function( m_penGC, mode );
    gdk_gc_set_function( m_brushGC, mode );
    gdk_gc_set_function( m_textGC, mode );
}

void wxWindowDC::SetTextForeground( const wxColour &col )
{
    if (!Ok()) return;
  
    if (m_textForegroundColour == col) return;
  
    m_textForegroundColour = col;
    if (!m_textForegroundColour.Ok()) return;
  
    m_textForegroundColour.CalcPixel( m_cmap );
    gdk_gc_set_foreground( m_textGC, m_textForegroundColour.GetColor() );
}

void wxWindowDC::SetTextBackground( const wxColour &col )
{
    if (!Ok()) return;
  
    if (m_textBackgroundColour == col) return;
  
    m_textBackgroundColour = col;
    if (!m_textBackgroundColour.Ok()) return;
  
    m_textBackgroundColour.CalcPixel( m_cmap );
    gdk_gc_set_background( m_textGC, m_textBackgroundColour.GetColor() );
}

void wxWindowDC::SetBackgroundMode( int mode )
{
    m_backgroundMode = mode;

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
}

void wxWindowDC::SetClippingRegion( long x, long y, long width, long height )
{
    wxDC::SetClippingRegion( x, y, width, height );
  
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

void wxWindowDC::DestroyClippingRegion(void)
{
    wxDC::DestroyClippingRegion();
  
    gdk_gc_set_clip_rectangle( m_penGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_brushGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_textGC, (GdkRectangle *) NULL );
    gdk_gc_set_clip_rectangle( m_bgGC, (GdkRectangle *) NULL );
}

void wxWindowDC::SetUpDC(void)
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
    
    tmp_brush = m_backgroundBrush;
    m_backgroundBrush = wxNullBrush;
    SetBackground( tmp_brush );
  
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

void wxWindowDC::Destroy(void)
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

GdkWindow *wxWindowDC::GetWindow(void)
{
    return m_window;
}

// ----------------------------------- spline code ----------------------------------------

void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
void wx_clear_stack(void);
int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);

wxList wx_spline_point_list;

#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

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

void wx_clear_stack(void)
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

void wxWindowDC::DrawSpline( wxList *points )
{
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


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC,wxWindowDC)

wxPaintDC::wxPaintDC(void)
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

wxClientDC::wxClientDC(void)
  : wxWindowDC()
{
}

wxClientDC::wxClientDC( wxWindow *win )
  : wxWindowDC( win )
{
}

