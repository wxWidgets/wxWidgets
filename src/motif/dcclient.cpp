/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/window.h"
#include <math.h>

#include <Xm/Xm.h>

#include "wx/motif/private.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define RAD2DEG 57.2957795131

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
//IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxDC)
//IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
#endif


wxWindowDC::wxWindowDC(void)
{
    m_gc = (WXGC) 0;
    m_gcBacking = (WXGC) 0;
    m_window = NULL;
    m_backgroundPixel = -1;
    m_currentPenWidth = 1;
    m_currentPenJoin = -1;
    m_currentPenDashCount = -1;
    m_currentPenDash = (char*) NULL;
    m_currentStyle = -1;
    m_currentFill = -1;
    m_currentBkMode = wxTRANSPARENT;
    m_colour = wxColourDisplay();
    m_display = (WXDisplay*) NULL;
    m_clippingRegion = (WXRegion) 0;
};

wxWindowDC::wxWindowDC( wxWindow *window )
{
    m_window = window;
    m_gc = (WXGC) 0;
    m_gcBacking = (WXGC) 0;
    m_window = NULL;
    m_backgroundPixel = -1;
    m_currentPenWidth = 1;
    m_currentPenJoin = -1;
    m_currentPenDashCount = -1;
    m_currentPenDash = (char*) NULL;
    m_currentStyle = -1;
    m_currentFill = -1;
    m_currentBkMode = wxTRANSPARENT;
    m_colour = wxColourDisplay();
    m_display = window->GetXDisplay();
    m_clippingRegion = (WXRegion) 0;
    m_ok = TRUE;
};

wxWindowDC::~wxWindowDC(void)
{
    if (m_gc)
        XFreeGC ((Display*) m_display, (GC) m_gc);
    m_gc = (WXGC) 0;

    if (m_gcBacking)
        XFreeGC ((Display*) m_display, (GC) m_gcBacking);
    m_gcBacking = (WXGC) 0;

    if (m_clippingRegion)
        XDestroyRegion ((Region) m_clippingRegion);
    m_clippingRegion = (WXRegion) 0;
};

void wxWindowDC::FloodFill( long WXUNUSED(x1), long WXUNUSED(y1), 
  wxColour* WXUNUSED(col), int WXUNUSED(style) )
{
};

bool wxWindowDC::GetPixel( long WXUNUSED(x1), long WXUNUSED(y1), wxColour *WXUNUSED(col) ) const
{
  return FALSE;
};

void wxWindowDC::DrawLine( long x1, long y1, long x2, long y2 )
{
  if (!Ok()) return;
#if 0  
  int x1d, y1d, x2d, y2d;

  /* MATTHEW: [7] Implement GetPixel */
  FreeGetPixelCache();

  x1d = XLOG2DEV(x1);
  y1d = YLOG2DEV(y1);
  x2d = XLOG2DEV(x2);
  y2d = YLOG2DEV(y2);

  if (current_pen && autoSetting)
    SetPen (current_pen);
  XDrawLine (display, pixmap, gc, x1d, y1d, x2d, y2d);

  if (canvas && canvas->is_retained)
    XDrawLine (display, canvas->backingPixmap, gcBacking, 
	       XLOG2DEV_2(x1), YLOG2DEV_2(y1),
	       XLOG2DEV_2(x2), YLOG2DEV_2(y2));

  CalcBoundingBox(x1, y1);
  CalcBoundingBox(x2, y2);
#endif
};

void wxWindowDC::CrossHair( long x, long y )
{
  if (!Ok()) return;
  
};

void wxWindowDC::DrawArc( long x1, long y1, long x2, long y2, long xc, long yc )
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
  };
  long alpha1 = long(radius1 * 64.0);
  long alpha2 = long((radius2 - radius1) * 64.0);
  while (alpha2 <= 0) alpha2 += 360*64;
  while (alpha1 > 360*64) alpha1 -= 360*64;

  if (m_brush.GetStyle() != wxTRANSPARENT) {};
    
  if (m_pen.GetStyle() != wxTRANSPARENT) {};
  
};

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
  if (m_brush.GetStyle() != wxTRANSPARENT) {};
  
  if (m_pen.GetStyle() != wxTRANSPARENT) {};
};

void wxWindowDC::DrawPoint( long x, long y )
{
  if (!Ok()) return;
  
  if (m_pen.GetStyle() != wxTRANSPARENT) {};
};

void wxWindowDC::DrawLines( int n, wxPoint points[], long xoffset, long yoffset )
{
  if (!Ok()) return;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) return;
  
  for (int i = 0; i < n-1; i++)
  {
    long x1 = XLOG2DEV(points[i].x + xoffset);
    long x2 = XLOG2DEV(points[i+1].x + xoffset);
    long y1 = YLOG2DEV(points[i].y + yoffset);     // oh, what a waste
    long y2 = YLOG2DEV(points[i+1].y + yoffset);
  };
};

void wxWindowDC::DrawLines( wxList *points, long xoffset, long yoffset )
{
  if (!Ok()) return;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) return;
  
  wxNode *node = points->First();
  while (node->Next())
  {
    wxPoint *point = (wxPoint*)node->Data();
    wxPoint *npoint = (wxPoint*)node->Next()->Data();
    long x1 = XLOG2DEV(point->x + xoffset);
    long x2 = XLOG2DEV(npoint->x + xoffset);
    long y1 = YLOG2DEV(point->y + yoffset);    // and again...
    long y2 = YLOG2DEV(npoint->y + yoffset);
    node = node->Next();
  };
};

void wxWindowDC::DrawPolygon( int WXUNUSED(n), wxPoint WXUNUSED(points)[], 
  long WXUNUSED(xoffset), long WXUNUSED(yoffset), int WXUNUSED(fillStyle) )
{
  if (!Ok()) return;
};

void wxWindowDC::DrawPolygon( wxList *WXUNUSED(lines), long WXUNUSED(xoffset), 
                             long WXUNUSED(yoffset), int WXUNUSED(fillStyle) )
{
  if (!Ok()) return;
};

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

  if (m_brush.GetStyle() != wxTRANSPARENT) {};
    
  if (m_pen.GetStyle() != wxTRANSPARENT) {};
};

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
  };
  
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
  };
};

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
  
  if (m_brush.GetStyle() != wxTRANSPARENT) {};
  
  if (m_pen.GetStyle() != wxTRANSPARENT) {};
};

bool wxWindowDC::CanDrawBitmap(void) const
{
  return TRUE;
};

void wxWindowDC::DrawIcon( const wxIcon &icon, long x, long y, bool useMask )
{
  if (!Ok()) return;
  
  if (!icon.Ok()) return;
  
  int xx = XLOG2DEV(x);
  int yy = YLOG2DEV(y);
  
};

bool wxWindowDC::Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int WXUNUSED(logical_func), bool WXUNUSED(useMask) )
{
  if (!Ok()) return FALSE;
  
  // CMB 20/5/98: add blitting of bitmaps
  if (source->IsKindOf(CLASSINFO(wxMemoryDC)))
  {
    wxMemoryDC* srcDC = (wxMemoryDC*)source;
    /*
     GdkBitmap* bmap = srcDC->m_selected.GetBitmap();
    if (bmap)
    {
      gdk_draw_bitmap (
          m_window,
          m_textGC,
          bmap,
          source->DeviceToLogicalX(xsrc), source->DeviceToLogicalY(ysrc),
          XLOG2DEV(xdest), YLOG2DEV(ydest),
          source->DeviceToLogicalXRel(width), source->DeviceToLogicalYRel(height)
          );
      return TRUE;
    }
    */
  }

  return TRUE;
};

void wxWindowDC::DrawText( const wxString &text, long x, long y, bool
WXUNUSED(use16) )
{
  if (!Ok()) return;

};



bool wxWindowDC::CanGetTextExtent(void) const
{
  return TRUE;
};

void wxWindowDC::GetTextExtent( const wxString &string, long *width, long *height,
                     long *WXUNUSED(descent), long *WXUNUSED(externalLeading),
                     wxFont *WXUNUSED(theFont), bool WXUNUSED(use16) )
{
  if (!Ok()) return;
  
};

long wxWindowDC::GetCharWidth(void)
{
  if (!Ok()) return 0;
  return 0;
};

long wxWindowDC::GetCharHeight(void)
{
  if (!Ok()) return 0;
  return 0;
};

void wxWindowDC::Clear(void)
{
  if (!Ok()) return;
  
};

void wxWindowDC::SetFont( const wxFont &font )
{
  if (!Ok()) return;
  
  m_font = font;
};

void wxWindowDC::SetPen( const wxPen &pen )
{
  if (!Ok()) return;

  if (m_pen == pen) return;
  
  m_pen = pen;
  
  if (!m_pen.Ok()) return;
};

void wxWindowDC::SetBrush( const wxBrush &brush )
{
  if (!Ok()) return;
  
  if (m_brush == brush) return;
  
  m_brush = brush;
  
  if (!m_brush.Ok()) return;
  
};

void wxWindowDC::SetBackground( const wxBrush &brush )
{
  if (!Ok()) return;
  
  if (m_backgroundBrush == brush) return;
  
  m_backgroundBrush = brush;
  
  if (!m_backgroundBrush.Ok()) return;
  
};

void wxWindowDC::SetLogicalFunction( int function )
{
  if (m_logicalFunction == function) return;
};

void wxWindowDC::SetTextForeground( const wxColour &col )
{
  if (!Ok()) return;
  
  if (m_textForegroundColour == col) return;
  
  m_textForegroundColour = col;
  if (!m_textForegroundColour.Ok()) return;
};

void wxWindowDC::SetTextBackground( const wxColour &col )
{
  if (!Ok()) return;
  
  if (m_textBackgroundColour == col) return;
  
  m_textBackgroundColour = col;
  if (!m_textBackgroundColour.Ok()) return;
};

void wxWindowDC::SetBackgroundMode( int mode )
{
  m_backgroundMode = mode;

  if (m_brush.GetStyle() != wxSOLID && m_brush.GetStyle() != wxTRANSPARENT)
  {
  }
};

void wxWindowDC::SetPalette( const wxPalette& WXUNUSED(palette) )
{
};

void wxWindowDC::SetClippingRegion( long x, long y, long width, long height )
{
  wxDC::SetClippingRegion( x, y, width, height );
  
};

void wxWindowDC::DestroyClippingRegion(void)
{
  wxDC::DestroyClippingRegion();
  
};

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

void wxWindowDC::DrawOpenSpline( wxList *points )
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
};
