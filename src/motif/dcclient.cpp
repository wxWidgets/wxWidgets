/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   About pens, brushes, and the autoSetting flag:

   Under X, pens and brushes control some of the same X drawing 
   parameters. Therefore, it is impossible to independently maintain
   the current pen and the current brush. Also, some settings depend
   on the current logical function. The m_currentFill, etc. instance
   variables remember state across the brush and pen.

   Since pens are used more than brushes, the autoSetting flag
   is used to indicate that a brush was recently used, and SetPen
   must be called to reinstall the current pen's parameters.
   If autoSetting includes 0x2, then the pens color may need
   to be set based on XOR.

   There is, unfortunately, some confusion between setting the
   current pen/brush and actually installing the brush/pen parameters.
   Both functionalies are perform by SetPen and SetBrush. C'est la vie.
*/

#ifdef __GNUG__
#pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/window.h"
#include "wx/app.h"
#include <math.h>

#include <Xm/Xm.h>

#include "wx/motif/private.h"

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"

static Pixmap bdiag, cdiag, fdiag, cross, horiz, verti;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define RAD2DEG 57.2957795131
// Fudge factor. Obsolete?
#define WX_GC_CF 0

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)
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
//    m_currentBkMode = wxTRANSPARENT;
    m_colour = wxColourDisplay();
    m_display = (WXDisplay*) NULL;
    m_currentRegion = (WXRegion) 0;
    m_userRegion = (WXRegion) 0;
    m_pixmap = (WXPixmap) 0;
    m_autoSetting = 0;
    m_oldFont = (WXFont) 0;
};

wxWindowDC::wxWindowDC( wxWindow *window )
{
    wxASSERT_MSG( (window != (wxWindow*) NULL), "You must pass a valid wxWindow to wxWindowDC/wxClientDC/wxPaintDC constructor." );

    m_window = window;
    m_gc = (WXGC) 0;
    m_gcBacking = (WXGC) 0;
    m_backgroundPixel = -1;
    m_currentPenWidth = 1;
    m_currentPenJoin = -1;
    m_currentPenDashCount = -1;
    m_currentPenDash = (char*) NULL;
    m_currentStyle = -1;
    m_currentFill = -1;
//    m_currentBkMode = wxTRANSPARENT;
    m_colour = wxColourDisplay();
    m_currentRegion = (WXRegion) 0;
    m_userRegion = (WXRegion) 0;
    m_ok = TRUE;
    m_autoSetting = 0;

    m_display = window->GetXDisplay();
    m_pixmap = window->GetXWindow();
    Display* display = (Display*) m_display;

    XSetWindowColormap (display, (Pixmap) m_pixmap, (Colormap) wxTheApp->GetMainColormap(m_display));

    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel (display, DefaultScreen (display));
    gcvalues.background = WhitePixel (display, DefaultScreen (display));
    gcvalues.graphics_exposures = False;
    gcvalues.line_width = 1;
    m_gc = (WXGC) XCreateGC (display, RootWindow (display, DefaultScreen (display)),
        GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth,
          &gcvalues);

    if (m_window->GetBackingPixmap())
    {
      m_gcBacking = (WXGC) XCreateGC (display, RootWindow (display,
                          DefaultScreen (display)),
        GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth,
             &gcvalues);
    }

    m_backgroundPixel = (int) gcvalues.background;

    // Get the current Font so we can set it back later
    XGCValues valReturn;
    XGetGCValues((Display*) m_display, (GC) m_gc, GCFont, &valReturn);
    m_oldFont = (WXFont) valReturn.font;
};

wxWindowDC::~wxWindowDC(void)
{
    if (m_gc && (m_oldFont != (WXFont) 0) && ((long) m_oldFont != -1))
    {
      XSetFont ((Display*) m_display, (GC) m_gc, (Font) m_oldFont);

      if (m_window && m_window->GetBackingPixmap())
          XSetFont ((Display*) m_display,(GC) m_gcBacking, (Font) m_oldFont);
    }

    if (m_gc)
        XFreeGC ((Display*) m_display, (GC) m_gc);
    m_gc = (WXGC) 0;

    if (m_gcBacking)
        XFreeGC ((Display*) m_display, (GC) m_gcBacking);
    m_gcBacking = (WXGC) 0;

    if (m_currentRegion)
        XDestroyRegion ((Region) m_currentRegion);
    m_currentRegion = (WXRegion) 0;

    if (m_userRegion)
        XDestroyRegion ((Region) m_userRegion);
    m_userRegion = (WXRegion) 0;
};

void wxWindowDC::FloodFill( long WXUNUSED(x1), long WXUNUSED(y1), 
  wxColour* WXUNUSED(col), int WXUNUSED(style) )
{
  // TODO
};

bool wxWindowDC::GetPixel( long WXUNUSED(x1), long WXUNUSED(y1), wxColour *WXUNUSED(col) ) const
{
  // TODO
  return FALSE;
};

void wxWindowDC::DrawLine( long x1, long y1, long x2, long y2 )
{
  if (!Ok()) return;

  int x1d, y1d, x2d, y2d;

  //  FreeGetPixelCache();

  x1d = XLOG2DEV(x1);
  y1d = YLOG2DEV(y1);
  x2d = XLOG2DEV(x2);
  y2d = YLOG2DEV(y2);

  if (m_autoSetting)
    SetPen (m_pen);

  XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, x1d, y1d, x2d, y2d);

  if (m_window && m_window->GetBackingPixmap())
    XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking, 
           XLOG2DEV_2(x1), YLOG2DEV_2(y1),
           XLOG2DEV_2(x2), YLOG2DEV_2(y2));

  CalcBoundingBox(x1, y1);
  CalcBoundingBox(x2, y2);
};

void wxWindowDC::CrossHair( long x, long y )
{
  if (!Ok()) return;

  if (m_autoSetting)
    SetPen (m_pen);

  int xx = XLOG2DEV (x);
  int yy = YLOG2DEV (y);
  int ww, hh;
  wxDisplaySize (&ww, &hh);
  XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, 0, yy,
         ww, yy);
  XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xx, 0,
         xx, hh);

  if (m_window && m_window->GetBackingPixmap())
    {
      xx = XLOG2DEV_2 (x);
      yy = YLOG2DEV_2 (y);
      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
         0, yy,
         ww, yy);
      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
         xx, 0,
         xx, hh);
    }
};

void wxWindowDC::DrawArc( long x1, long y1, long x2, long y2, long xc, long yc )
{
  if (!Ok()) return;
  
//  FreeGetPixelCache();

  int xx1 = XLOG2DEV (x1);
  int yy1 = YLOG2DEV (y1);
  int xx2 = XLOG2DEV (x2);
  int yy2 = YLOG2DEV (y2);
  int xxc = XLOG2DEV (xc);
  int yyc = YLOG2DEV (yc);
  int xxc_2 = XLOG2DEV_2 (xc);
  int yyc_2 = YLOG2DEV_2 (yc);

  long dx = xx1 - xxc;
  long dy = yy1 - yyc;
  double radius = sqrt (dx * dx + dy * dy);
  long r = (long) radius;

  double radius1, radius2;

  if (xx1 == xx2 && yy1 == yy2)
    {
      radius1 = 0.0;
      radius2 = 360.0;
    }
  else if (radius == 0.0)
    radius1 = radius2 = 0.0;
  else
    {
      if (xx1 - xxc == 0)
    if (yy1 - yyc < 0)
      radius1 = 90.0;
    else
      radius1 = -90.0;
      else
    radius1 = -atan2 ((double) (yy1 - yyc), (double) (xx1 - xxc)) * 360.0 / (2 * M_PI);

      if (xx2 - xxc == 0)
    if (yy2 - yyc < 0)
      radius2 = 90.0;
    else
      radius2 = -90.0;
      else
    radius2 = -atan2 ((double) (yy2 - yyc), (double) (xx2 - xxc)) * 360.0 / (2 * M_PI);
    }
  radius1 *= 64.0;
  radius2 *= 64.0;
  int alpha1 = (int) radius1;
  int alpha2 = (int) (radius2 - radius1);
  while (alpha2 <= 0)
    alpha2 += 360 * 64;
  while (alpha2 > 360 * 64)
    alpha2 -= 360 * 64;

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) (GC) m_gc,
        xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);

      if (m_window && m_window->GetBackingPixmap())
      XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
         xxc_2 - r, yyc_2 - r, 2 * r, 2 * r, alpha1, alpha2);

    }

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
        if (m_autoSetting)
            SetPen (m_pen);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc,
           xxc - r, yyc - r, 2 * r, 2 * r, alpha1, alpha2);

      if (m_window && m_window->GetBackingPixmap())
        XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
           xxc_2 - r, yyc_2 - r, 2 * r, 2 * r, alpha1, alpha2);
    }
  CalcBoundingBox (x1, y1);
  CalcBoundingBox (x2, y2);
};

void wxWindowDC::DrawEllipticArc( long x, long y, long width, long height, double sa, double ea )
{
  if (!Ok()) return;
  
  int xd, yd, wd, hd;

  xd = XLOG2DEV(x);
  yd = YLOG2DEV(y);
  wd = XLOG2DEVREL(width);
  hd = YLOG2DEVREL(height);

  if (sa>=360 || sa<=-360) sa=sa-int(sa/360)*360;
  if (ea>=360 || ea<=-360) ea=ea-int(ea/360)*360;
  int start = int(sa*64);
  int end   = int(ea*64);
  if (start<0) start+=360*64;
  if (end  <0) end  +=360*64;
  if (end>start) end-=start;
  else end+=360*64-start;

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      m_autoSetting = TRUE;    // must be reset

      SetBrush (m_brush);
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wd, hd, start, end);

      if (m_window && m_window->GetBackingPixmap())
       XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
          XLOG2DEV_2 (x), YLOG2DEV_2 (y),wd,hd,start,end);
    }

  if (!m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      if (m_autoSetting)
    SetPen (m_pen);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wd, hd, start,end);
      if (m_window && m_window->GetBackingPixmap())
        XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
          XLOG2DEV_2 (x), YLOG2DEV_2 (y),wd,hd,start,end);
    }
  CalcBoundingBox (x, y);
  CalcBoundingBox (x + width, y + height);
};

void wxWindowDC::DrawPoint( long x, long y )
{
  if (!Ok()) return;
  
//  FreeGetPixelCache();

  if (m_pen.Ok() && m_autoSetting)
    SetPen (m_pen);

  XDrawPoint ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, XLOG2DEV (x), YLOG2DEV (y));
  if (m_window && m_window->GetBackingPixmap())
    XDrawPoint ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking, XLOG2DEV_2 (x), YLOG2DEV_2 (y));

  CalcBoundingBox (x, y);
};

void wxWindowDC::DrawLines( int n, wxPoint points[], long xoffset, long yoffset )
{
  if (!Ok()) return;
  
//  FreeGetPixelCache();

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      if (m_autoSetting)
    SetPen (m_pen);

      XPoint *xpoints = new XPoint[n];
      int i;

      for (i = 0; i < n; i++)
    {
      xpoints[i].x = XLOG2DEV (points[i].x + xoffset);
      xpoints[i].y = YLOG2DEV (points[i].y + yoffset);
    }
      XDrawLines ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xpoints, n, 0);

    if (m_window && m_window->GetBackingPixmap())
    {
      for (i = 0; i < n; i++)
        {
          xpoints[i].x = XLOG2DEV_2 (points[i].x + xoffset);
          xpoints[i].y = YLOG2DEV_2 (points[i].y + yoffset);
        }
      XDrawLines ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking, xpoints, n, 0);
    }
    delete[]xpoints;
  }
};

void wxWindowDC::DrawLines( wxList *list, long xoffset, long yoffset )
{
  if (!Ok()) return;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) return;
  
  int n = list->Number();
  wxPoint *points = new wxPoint[n];

  int i = 0;
  for(wxNode *node = list->First(); node; node = node->Next()) {
    wxPoint *point = (wxPoint *)node->Data();
    points[i].x = point->x;
    points[i++].y = point->y;
  }
  DrawLines(n, points, xoffset, yoffset);
  delete []points;
};

void wxWindowDC::DrawPolygon( int n, wxPoint points[],
  long xoffset, long yoffset, int fillStyle )
{
//  FreeGetPixelCache();

  XPoint *xpoints1 = new XPoint[n + 1];
  XPoint *xpoints2 = new XPoint[n + 1];
  int i;
  for (i = 0; i < n; i++)
    {
      xpoints1[i].x = XLOG2DEV (points[i].x + xoffset);
      xpoints1[i].y = YLOG2DEV (points[i].y + yoffset);
      xpoints2[i].x = XLOG2DEV_2 (points[i].x + xoffset);
      xpoints2[i].y = YLOG2DEV_2 (points[i].y + yoffset);
      CalcBoundingBox (points[i].x + xoffset, points[i].y + yoffset);
    }

  // Close figure for XDrawLines (not needed for XFillPolygon)
  xpoints1[i].x = xpoints1[0].x;
  xpoints1[i].y = xpoints1[0].y;
  xpoints2[i].x = xpoints2[0].x;
  xpoints2[i].y = xpoints2[0].y;

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);
      XSetFillRule ((Display*) m_display, (GC) m_gc, fillStyle == wxODDEVEN_RULE ? EvenOddRule : WindingRule);
      XFillPolygon ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xpoints1, n, Complex, 0);
      XSetFillRule ((Display*) m_display, (GC) m_gc, EvenOddRule);    // default mode
      if (m_window && m_window->GetBackingPixmap())
      {
         XSetFillRule ((Display*) m_display,(GC) m_gcBacking,
             fillStyle == wxODDEVEN_RULE ? EvenOddRule : WindingRule);
         XFillPolygon ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking, xpoints2, n, Complex, 0);
         XSetFillRule ((Display*) m_display,(GC) m_gcBacking, EvenOddRule);    // default mode
      }
  }

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      if (m_autoSetting)
        SetPen (m_pen);
      XDrawLines ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xpoints1, n + 1, 0);

      if (m_window && m_window->GetBackingPixmap())
        XDrawLines ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking, xpoints2, n + 1, 0);
    }

  delete[]xpoints1;
  delete[]xpoints2;
};

void wxWindowDC::DrawPolygon( wxList *list, long xoffset,
                             long yoffset, int fillStyle )
{
  if (!Ok()) return;

  int n = list->Number();
  wxPoint *points = new wxPoint[n];

  int i = 0;
  for(wxNode *node = list->First(); node; node = node->Next()) {
    wxPoint *point = (wxPoint *)node->Data();
    points[i].x = point->x;
    points[i++].y = point->y;
  }
  DrawPolygon(n, points, xoffset, yoffset,fillStyle);
  delete[] points;
};

void wxWindowDC::DrawRectangle( long x, long y, long width, long height )
{
  if (!Ok()) return;

//  FreeGetPixelCache();

  int xd, yd, wfd, hfd, wd, hd;

  xd = XLOG2DEV(x);
  yd = YLOG2DEV(y);
  wfd = XLOG2DEVREL(width);
  wd = wfd - WX_GC_CF;
  hfd = YLOG2DEVREL(height);
  hd = hfd - WX_GC_CF;

  if (wfd == 0 || hfd == 0) return;
  if (wd < 0) { wd = - wd; xd = xd - wd; }
  if (hd < 0) { hd = - hd; yd = yd - hd; }

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);
      XFillRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wfd, hfd);

      if (m_window && m_window->GetBackingPixmap())
    XFillRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            XLOG2DEV_2 (x), YLOG2DEV_2 (y),
            wfd, hfd);
    }

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      if (m_autoSetting)
    SetPen (m_pen);
      XDrawRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wd, hd);

      if (m_window && m_window->GetBackingPixmap())
    XDrawRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            XLOG2DEV_2 (x), YLOG2DEV_2 (y),
            wd, hd);
    }
  CalcBoundingBox (x, y);
  CalcBoundingBox (x + width, y + height);
};

void wxWindowDC::DrawRoundedRectangle( long x, long y, long width, long height, double radius )
{
  if (!Ok()) return;
  
//  FreeGetPixelCache();

  // If radius is negative, it's a proportion of the smaller dimension.

  if (radius < 0.0) radius = - radius * ((width < height) ? width : height);

  int xd = XLOG2DEV (x);
  int yd = YLOG2DEV (y);
  int rd = XLOG2DEVREL ((long) radius);
  int wd = XLOG2DEVREL (width) - WX_GC_CF;
  int hd = YLOG2DEVREL (height) - WX_GC_CF;

  int rw_d = rd * 2;
  int rh_d = rw_d;

  // If radius is zero use DrawRectangle() instead to avoid
  // X drawing errors with small radii
  if (rd == 0)
  {
    DrawRectangle( x, y, width, height );
    return;
  }

  // Draw nothing if transformed w or h is 0
  if (wd == 0 || hd == 0) return;

  // CMB: adjust size if outline is drawn otherwise the result is
  // 1 pixel too wide and high
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
    wd--;
    hd--;
  }

  // CMB: ensure dd is not larger than rectangle otherwise we
  // get an hour glass shape
  if (rw_d > wd) rw_d = wd;
  if (rw_d > hd) rw_d = hd;
  rd = rw_d / 2;

  // For backing pixmap
  int xd2 = XLOG2DEV_2 (x);
  int yd2 = YLOG2DEV_2 (y);
  int rd2 = XLOG2DEVREL ((long) radius);
  int wd2 = XLOG2DEVREL (width) ;
  int hd2 = YLOG2DEVREL (height) ;

  int rw_d2 = rd2 * 2;
  int rh_d2 = rw_d2;

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
  {
      SetBrush (m_brush);

      XFillRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + rd, yd,
              wd - rw_d, hd);
      XFillRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd + rd,
              wd, hd - rh_d);

      // Arcs start from 3 o'clock, positive angles anticlockwise
      // Top-left
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd,
        rw_d, rh_d, 90 * 64, 90 * 64);
      // Top-right
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + wd - rw_d, yd,
//        rw_d, rh_d, 0, 90 * 64);
        rw_d, rh_d, 0, 91 * 64);
      // Bottom-right
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + wd - rw_d,
        yd + hd - rh_d,
//        rw_d, rh_d, 270 * 64, 90 * 64);
        rw_d, rh_d, 269 * 64, 92 * 64);
      // Bottom-left
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd + hd - rh_d,
        rw_d, rh_d, 180 * 64, 90 * 64);

    if (m_window && m_window->GetBackingPixmap())
    {
      XFillRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
              xd2 + rd2, yd2, wd2 - rw_d2, hd2);
      XFillRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
              xd2, yd2 + rd2, wd2, hd2 - rh_d2);

      XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
       xd2, yd2, rw_d2, rh_d2, 90 * 64, 90 * 64);
      XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2 + wd2 - rw_d2, yd2,
//            rw_d2, rh_d2, 0, 90 * 64);
            rw_d2, rh_d2, 0, 91 * 64);
      XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2 + wd2 - rw_d2,
            yd2 + hd2 - rh_d2,
//            rw_d2, rh_d2, 270 * 64, 90 * 64);
            rw_d2, rh_d2, 269 * 64, 92 * 64);
      XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2, yd2 + hd2 - rh_d2,
            rw_d2, rh_d2, 180 * 64, 90 * 64);
    }
  }

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
  {
      SetPen (m_pen);
      XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + rd, yd,
         xd + wd - rd + 1, yd);
      XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + rd, yd + hd,
         xd + wd - rd, yd + hd);

      XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd + rd,
         xd, yd + hd - rd);
      XDrawLine ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + wd, yd + rd,
         xd + wd, yd + hd - rd + 1);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd,
        rw_d, rh_d, 90 * 64, 90 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + wd - rw_d, yd,
//        rw_d, rh_d, 0, 90 * 64);
        rw_d, rh_d, 0, 91 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd + wd - rw_d,
        yd + hd - rh_d,
        rw_d, rh_d, 269 * 64, 92 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd + hd - rh_d,
        rw_d, rh_d, 180 * 64, 90 * 64);

      if (m_window && m_window->GetBackingPixmap())
    {
      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
             xd2 + rd2, yd2,
             xd2 + wd2 - rd2 + 1, yd2);
      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
             xd2 + rd2, yd2 + hd2,
          xd2 + wd2 - rd2, yd2 + hd2);

      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
             xd2, yd2 + rd2,
             xd2, yd2 + hd2 - rd2);
      XDrawLine ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
             xd2 + wd2, yd2 + rd2,
          xd2 + wd2, yd2 + hd2 - rd2 + 1);
      XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2, yd2,
            rw_d2, rh_d2, 90 * 64, 90 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2 + wd2 - rw_d2, yd2,
//            rw_d2, rh_d2, 0, 90 * 64);
            rw_d2, rh_d2, 0, 91 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2 + wd2 - rw_d2,
            yd2 + hd2 - rh_d2,
            rw_d2, rh_d2, 269 * 64, 92 * 64);
      XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            xd2, yd2 + hd2 - rh_d2,
            rw_d2, rh_d2, 180 * 64, 90 * 64);
    }
  }
  CalcBoundingBox (x, y);
  CalcBoundingBox (x + width, y + height);


};

void wxWindowDC::DrawEllipse( long x, long y, long width, long height )
{
  if (!Ok()) return;
  
  // Check for negative width and height
  if (height < 0)
  {
    y = y + height;
    height = - height ;
  }

  if (width < 0)
  {
    x = x + width;
    width = - width ;
  }

//  FreeGetPixelCache();

  static const int angle = 23040;

  int xd, yd, wd, hd;

  xd = XLOG2DEV(x);
  yd = YLOG2DEV(y);
  wd = XLOG2DEVREL(width) ;
  hd = YLOG2DEVREL(height) ;

  if (m_brush.Ok() && m_brush.GetStyle () != wxTRANSPARENT)
    {
      SetBrush (m_brush);
      XFillArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wd, hd, 0, angle);
      if (m_window && m_window->GetBackingPixmap())
        XFillArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
          XLOG2DEV_2 (x), YLOG2DEV_2 (y),
          XLOG2DEVREL (width) - WX_GC_CF,
          YLOG2DEVREL (height) - WX_GC_CF, 0, angle);
    }

  if (m_pen.Ok() && m_pen.GetStyle () != wxTRANSPARENT)
    {
      if (m_autoSetting)
    SetPen (m_pen);
      XDrawArc ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, xd, yd, wd, hd, 0, angle);
      if (m_window && m_window->GetBackingPixmap())
        XDrawArc ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
          XLOG2DEV_2 (x), YLOG2DEV_2 (y),
          XLOG2DEVREL (width) - WX_GC_CF,
          YLOG2DEVREL (height) - WX_GC_CF, 0, angle);
    }
  CalcBoundingBox (x, y);
  CalcBoundingBox (x + width, y + height);

};

bool wxWindowDC::CanDrawBitmap(void) const
{
  return TRUE;
};

/* Used when copying between drawables on different (Display*) m_displays.
   Not very fast, but better than giving up.
 */

static void XCopyRemote(Display *src_display, Display *dest_display,
            Drawable src, Drawable dest,
            GC destgc,
            int srcx, int srcy,
            unsigned int w, unsigned int h,
            int destx, int desty,
            bool more, XImage **cache)
{
  XImage *image, *destimage;
  Colormap destcm, srccm;
#define CACHE_SIZE 256
  unsigned int i, j;
  unsigned long cachesrc[CACHE_SIZE], cachedest[CACHE_SIZE];
  int k, cache_pos, all_cache;

  if (!cache || !*cache)
    image = XGetImage(src_display, src, srcx, srcy, w, h, AllPlanes, ZPixmap);
  else
    image = *cache;

  destimage = XGetImage(dest_display, dest, destx, desty, w, h, AllPlanes, ZPixmap);

  srccm = (Colormap) wxTheApp->GetMainColormap((WXDisplay*) src_display);
  destcm = (Colormap) wxTheApp->GetMainColormap((WXDisplay*) dest_display);

  cache_pos = 0;
  all_cache = FALSE;

  for (i = 0; i < w; i++)
    for (j = 0; j < h; j++) {
      unsigned long pixel;
      XColor xcol;

      pixel = XGetPixel(image, i, j);
      for (k = cache_pos; k--; )
    if (cachesrc[k] == pixel) {
      pixel = cachedest[k];
      goto install;
    }
      if (all_cache)
    for (k = CACHE_SIZE; k-- > cache_pos; )
      if (cachesrc[k] == pixel) {
        pixel = cachedest[k];
        goto install;
      }

      cachesrc[cache_pos] = xcol.pixel = pixel;
      XQueryColor(src_display, srccm, &xcol);
      if (!XAllocColor(dest_display, destcm, &xcol))
    xcol.pixel = 0;
      cachedest[cache_pos] = pixel = xcol.pixel;
      
      if (++cache_pos >= CACHE_SIZE) {
    cache_pos = 0;
    all_cache = TRUE;
      }

    install:
      XPutPixel(destimage, i, j, pixel);
    }

  XPutImage(dest_display, dest, destgc, destimage, 0, 0, destx, desty, w, h);
  XDestroyImage(destimage);

  if (more)
    *cache = image;
  else
    XDestroyImage(image);
}

void wxWindowDC::DrawIcon( const wxIcon &icon, long x, long y, bool useMask )
{
  if (!Ok()) return;
  
  if (!icon.Ok()) return;
  
//  FreeGetPixelCache();

  // Be sure that foreground pixels (1) of
  // the Icon will be painted with pen colour. [m_pen.SetColour()]
  // Background pixels (0) will be painted with
  // last selected background color. [::SetBackground]
  if (m_pen.Ok() && m_autoSetting)
    SetPen (m_pen);

  int width, height;
  Pixmap iconPixmap = (Pixmap) icon.GetPixmap();
  width = icon.GetWidth();
  height = icon.GetHeight();
  if (icon.GetDisplay() == m_display)
  {
   if (icon.GetDepth() <= 1)
   {
       XCopyPlane  ((Display*) m_display, iconPixmap, (Pixmap) m_pixmap, (GC) m_gc,
                    0, 0, width, height,
                    (int) XLOG2DEV (x), (int) YLOG2DEV (y), 1);
   }
   else
   {
       XCopyArea  ((Display*) m_display, iconPixmap, (Pixmap) m_pixmap, (GC) m_gc,
                  0, 0, width, height,
                  (int) XLOG2DEV (x), (int) YLOG2DEV (y));
   }


  if (m_window && m_window->GetBackingPixmap())
  {
     if (icon.GetDepth() <= 1)
     {
        XCopyPlane ((Display*) m_display, iconPixmap, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
          0, 0, width, height, (int) XLOG2DEV_2 (x), (int) YLOG2DEV_2 (y), 1);
     }
     else
     {
       XCopyArea  ((Display*) m_display, iconPixmap, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
                  0, 0, width, height,
                  (int) XLOG2DEV_2 (x), (int) YLOG2DEV_2 (y));
     }
  }
  } else { /* Remote copy (different (Display*) m_displays) */
    XImage *cache = NULL;
    if (m_window && m_window->GetBackingPixmap())
      XCopyRemote((Display*) icon.GetDisplay(), (Display*) m_display, iconPixmap, (Pixmap) m_window->GetBackingPixmap(),
         (GC) m_gcBacking, 0, 0, width, height,
          (int) XLOG2DEV_2 (x), (int) YLOG2DEV_2 (y), TRUE, &cache);
    XCopyRemote((Display*) icon.GetDisplay(), (Display*) m_display, iconPixmap, (Pixmap) m_pixmap, (GC) m_gc,
        0, 0, width, height,
        (int) XLOG2DEV (x), (int) YLOG2DEV (y), FALSE, &cache);
  }
  CalcBoundingBox (x, y);

};

bool wxWindowDC::Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int rop, bool useMask )
{
  if (!Ok()) return FALSE;

  wxASSERT_MSG( (source->IsKindOf(CLASSINFO(wxWindowDC))), "Blit source DC must be wxWindowDC or derived class." );

  wxWindowDC* sourceDC = (wxWindowDC*) source;
  
//  FreeGetPixelCache();

  // Be sure that foreground pixels (1) of
  // the Icon will be painted with pen colour. [m_pen.SetColour()]
  // Background pixels (0) will be painted with
  // last selected background color. [::SetBackground]
  if (m_pen.Ok() && m_autoSetting)
    SetPen (m_pen);

  if (m_pixmap && sourceDC->m_pixmap)
    {
      /* MATTHEW: [9] */
      int orig = m_logicalFunction;

      SetLogicalFunction (rop);

      if (m_display != sourceDC->m_display)
      {
        XImage *cache = NULL;

        if (m_window && m_window->GetBackingPixmap())
            XCopyRemote((Display*) sourceDC->m_display, (Display*) m_display,
              (Pixmap) sourceDC->m_pixmap, (Pixmap) m_window->GetBackingPixmap(),
             (GC) m_gcBacking,
              source->LogicalToDeviceX (xsrc),
              source->LogicalToDeviceY (ysrc),
              source->LogicalToDeviceXRel(width),
              source->LogicalToDeviceYRel(height),
              XLOG2DEV_2 (xdest), YLOG2DEV_2 (ydest),
              TRUE, &cache);

        if ( useMask && source->IsKindOf(CLASSINFO(wxMemoryDC)) )
        {
            wxMemoryDC *memDC = (wxMemoryDC *)source;
            wxBitmap& sel = memDC->GetBitmap();
            if ( sel.Ok() && sel.GetMask() && sel.GetMask()->GetPixmap() )
            {
                XSetClipMask   ((Display*) m_display, (GC) m_gc, (Pixmap) sel.GetMask()->GetPixmap());
                XSetClipOrigin ((Display*) m_display, (GC) m_gc, XLOG2DEV (xdest), YLOG2DEV (ydest));
            }
        }

        XCopyRemote((Display*) sourceDC->m_display, (Display*) m_display, (Pixmap) sourceDC->m_pixmap, (Pixmap) m_pixmap, (GC) m_gc,
            source->LogicalToDeviceX (xsrc),
            source->LogicalToDeviceY (ysrc),
            source->LogicalToDeviceXRel(width),
            source->LogicalToDeviceYRel(height),
            XLOG2DEV (xdest), YLOG2DEV (ydest),
            FALSE, &cache);

        if ( useMask )
        {
            XSetClipMask   ((Display*) m_display, (GC) m_gc, None);
            XSetClipOrigin ((Display*) m_display, (GC) m_gc, 0, 0);
        }
    
    } else
    {
      if (m_window && m_window->GetBackingPixmap())
      {
// +++ MARKUS (mho@comnets.rwth-aachen): error on blitting bitmaps with depth 1
        if (source->IsKindOf(CLASSINFO(wxMemoryDC)) && ((wxMemoryDC*) source)->GetBitmap().GetDepth() == 1)
        {
           XCopyPlane ((Display*) m_display, (Pixmap) sourceDC->m_pixmap, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
                   source->LogicalToDeviceX (xsrc),
                  source->LogicalToDeviceY (ysrc),
                  source->LogicalToDeviceXRel(width),
                  source->LogicalToDeviceYRel(height),
                  XLOG2DEV_2 (xdest), YLOG2DEV_2 (ydest), 1);
        }
      else
        {
           XCopyArea ((Display*) m_display, (Pixmap) sourceDC->m_pixmap, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
                  source->LogicalToDeviceX (xsrc),
                 source->LogicalToDeviceY (ysrc),
                 source->LogicalToDeviceXRel(width),
                 source->LogicalToDeviceYRel(height),
                 XLOG2DEV_2 (xdest), YLOG2DEV_2 (ydest));
        }
    }
    if ( useMask && source->IsKindOf(CLASSINFO(wxMemoryDC)) )
    {
        wxMemoryDC *memDC = (wxMemoryDC *)source;
        wxBitmap& sel = memDC->GetBitmap();
        if ( sel.Ok() && sel.GetMask() && sel.GetMask()->GetPixmap() )
        {
            XSetClipMask   ((Display*) m_display, (GC) m_gc, (Pixmap) sel.GetMask()->GetPixmap());
            XSetClipOrigin ((Display*) m_display, (GC) m_gc, XLOG2DEV (xdest), YLOG2DEV (ydest));
        }
    }

      // Check if we're copying from a mono bitmap
      if (source->IsKindOf(CLASSINFO(wxMemoryDC)) &&
          ((wxMemoryDC*)source)->GetBitmap().Ok() && (((wxMemoryDC*)source)->GetBitmap().GetDepth () == 1))
      {
        XCopyPlane ((Display*) m_display, (Pixmap) sourceDC->m_pixmap, (Pixmap) m_pixmap, (GC) m_gc,
              source->LogicalToDeviceX (xsrc),
              source->LogicalToDeviceY (ysrc),
              source->LogicalToDeviceXRel(width),
              source->LogicalToDeviceYRel(height),
              XLOG2DEV (xdest), YLOG2DEV (ydest), 1);
      }
      else
      {
        XCopyArea ((Display*) m_display, (Pixmap) sourceDC->m_pixmap, (Pixmap) m_pixmap, (GC) m_gc,
             source->LogicalToDeviceX (xsrc),
             source->LogicalToDeviceY (ysrc),
             source->LogicalToDeviceXRel(width),
             source->LogicalToDeviceYRel(height),
             XLOG2DEV (xdest), YLOG2DEV (ydest));

      }
    if ( useMask )
    {
        XSetClipMask   ((Display*) m_display, (GC) m_gc, None);
        XSetClipOrigin ((Display*) m_display, (GC) m_gc, 0, 0);
    }

    } /* Remote/local (Display*) m_display */
    CalcBoundingBox (xdest, ydest);
    CalcBoundingBox (xdest + width, ydest + height);
    
    SetLogicalFunction(orig);

    return TRUE;
  }
  return FALSE;
};

/* Helper function for 16-bit fonts */
static int str16len(const char *s)
{
  int count = 0;

  while (s[0] && s[1]) {
    count++;
    s += 2;
  }

  return count;
}

void wxWindowDC::DrawText( const wxString &text, long x, long y, bool use16 )
{
  if (!Ok()) return;

  // Since X draws from the baseline of the text, must
  // add the text height
  int cx = 0;
  int cy = 0;
  int ascent = 0;
  int slen;

  if (use16)
    slen = str16len(text);
  else
    slen = strlen(text);

  if (m_font.Ok())
    {
      WXFontStructPtr pFontStruct = m_font.GetFontStruct(m_userScaleY*m_logicalScaleY, m_display);
      int direction, descent;
      XCharStruct overall_return;
      if (use16)
        (void)XTextExtents16((XFontStruct*) pFontStruct, (XChar2b *)(const char*) text, slen, &direction,
                 &ascent, &descent, &overall_return);
      else
        (void)XTextExtents((XFontStruct*) pFontStruct, (char*) (const char*) text, slen, &direction,
               &ascent, &descent, &overall_return);
      cx = overall_return.width;
      cy = ascent + descent;
    }

  // First draw a rectangle representing the text background,
  // if a text background is specified
  if (m_textBackgroundColour.Ok () && (m_backgroundMode != wxTRANSPARENT))
    {
      wxColour oldPenColour = m_currentColour;
      m_currentColour = m_textBackgroundColour;
      bool sameColour = (oldPenColour.Ok () && m_textBackgroundColour.Ok () &&
        (oldPenColour.Red () == m_textBackgroundColour.Red ()) &&
          (oldPenColour.Blue () == m_textBackgroundColour.Blue ()) &&
        (oldPenColour.Green () == m_textBackgroundColour.Green ()));

      // This separation of the big && test required for gcc2.7/HP UX 9.02
      // or pixel value can be corrupted!
      sameColour = (sameColour &&
           (oldPenColour.GetPixel() == m_textBackgroundColour.GetPixel()));

      if (!sameColour || !GetOptimization())
      {
        int pixel = m_textBackgroundColour.AllocColour(m_display);
        m_currentColour = m_textBackgroundColour;

        // Set the GC to the required colour
        if (pixel > -1)
        {
          XSetForeground ((Display*) m_display, (GC) m_gc, pixel);
          if (m_window && m_window->GetBackingPixmap())
            XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel);
        }
      }
      else
        m_textBackgroundColour = oldPenColour ;

      XFillRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, XLOG2DEV (x), YLOG2DEV (y), cx, cy);
      if (m_window && m_window->GetBackingPixmap())
        XFillRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking,
            XLOG2DEV_2 (x), YLOG2DEV_2 (y), cx, cy);
    }

  // Now set the text foreground and draw the text
  if (m_textForegroundColour.Ok ())
  {
      wxColour oldPenColour = m_currentColour;
      m_currentColour = m_textForegroundColour;
      bool sameColour = (oldPenColour.Ok () && m_currentColour.Ok () &&
              (oldPenColour.Red () == m_currentColour.Red ()) &&
               (oldPenColour.Blue () == m_currentColour.Blue ()) &&
             (oldPenColour.Green () == m_currentColour.Green ()) &&
              (oldPenColour.GetPixel() == m_currentColour.GetPixel()));

      if (!sameColour || !GetOptimization())
      {
        int pixel = -1;
        if (!m_colour) // Mono display
        {
          // Unless foreground is really white, draw it in black
          unsigned char red = m_textForegroundColour.Red ();
          unsigned char blue = m_textForegroundColour.Blue ();
          unsigned char green = m_textForegroundColour.Green ();
          if (red == (unsigned char) 255 && blue == (unsigned char) 255
                && green == (unsigned char) 255)
          {
            m_currentColour = *wxWHITE;
            pixel = (int) WhitePixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
            m_currentColour.SetPixel(pixel);
            m_textForegroundColour.SetPixel(pixel);
          }
          else
          {
            m_currentColour = *wxBLACK;
            pixel = (int) BlackPixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
            m_currentColour.SetPixel(pixel);
            m_textForegroundColour.SetPixel(pixel);
          }
        }
        else
        {
            pixel = m_textForegroundColour.AllocColour((Display*) m_display);
            m_currentColour.SetPixel(pixel);
        }

        // Set the GC to the required colour
        if (pixel > -1)
        {
            XSetForeground ((Display*) m_display, (GC) m_gc, pixel);
            if (m_window && m_window->GetBackingPixmap())
              XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel);
        }
      }
      else
        m_textForegroundColour = oldPenColour;
  }

  // We need to add the ascent, not the whole height, since X draws
  // at the point above the descender.
  if (use16)
    XDrawString16((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, XLOG2DEV (x), YLOG2DEV (y) + ascent, 
          (XChar2b *)(char*) (const char*) text, slen);
  else
    XDrawString((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, XLOG2DEV (x), YLOG2DEV (y) + ascent, 
        text, slen);

  if (m_window && m_window->GetBackingPixmap()) {
    if (use16)
      XDrawString16((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
            XLOG2DEV_2 (x), YLOG2DEV_2 (y) + ascent,
            (XChar2b *)(char*) (const char*) text, slen);
    else
      XDrawString((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(), (GC) m_gcBacking,
          XLOG2DEV_2 (x), YLOG2DEV_2 (y) + ascent, (char*) (const char*) text, slen);
  }

  long w, h;
  GetTextExtent (text, &w, &h);
  CalcBoundingBox (x + w, y + h);
  CalcBoundingBox (x, y);
};

bool wxWindowDC::CanGetTextExtent(void) const
{
  return TRUE;
};

void wxWindowDC::GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent, long *externalLeading,
                     wxFont *font, bool use16 )
{
  if (!Ok()) return;

  wxFont* theFont = font;
  if (!theFont)
     theFont = & m_font;
  
  if (!theFont->Ok())
  {
    // TODO: this should be an error log function
    cerr << "wxWindows warning - set a valid font before calling GetTextExtent!\n";
    *width = -1;
    *height = -1;
    return;
  }

  WXFontStructPtr pFontStruct = theFont->GetFontStruct(m_userScaleY*m_logicalScaleY, m_display);

  int direction, ascent, descent2;
  XCharStruct overall;
  int slen;
  
  if (use16) slen = str16len(string); else slen = strlen(string);

  if (use16)
    XTextExtents16((XFontStruct*) pFontStruct, (XChar2b *) (char*) (const char*) string, slen, &direction,
           &ascent, &descent2, &overall);
  else
     XTextExtents((XFontStruct*) pFontStruct, (char*) (const char*) string, slen, &direction,
         &ascent, &descent2, &overall);

  *width = XDEV2LOGREL (overall.width);
  *height = YDEV2LOGREL (ascent + descent2);
  if (descent)
    *descent = descent2;
  if (externalLeading)
    *externalLeading = 0;
};

long wxWindowDC::GetCharWidth(void)
{
  if (!Ok()) return 0;

  if (!m_font.Ok())
    return 0;

  WXFontStructPtr pFontStruct = m_font.GetFontStruct(m_userScaleY * m_logicalScaleY, m_display);

  int direction, ascent, descent;
  XCharStruct overall;
  XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
        &descent, &overall);
  return XDEV2LOGREL(overall.width);
};

long wxWindowDC::GetCharHeight(void)
{
  if (!Ok()) return 0;

  if (!m_font.Ok())
    return 0;

  WXFontStructPtr pFontStruct = m_font.GetFontStruct(m_userScaleY*m_logicalScaleY, m_display);

  int direction, ascent, descent;
  XCharStruct overall;
  XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
        &descent, &overall);
//  return XDEV2LOGREL(overall.ascent + overall.descent);
  return XDEV2LOGREL(ascent + descent);
};

void wxWindowDC::Clear(void)
{
  if (!Ok()) return;
  
  int w, h;
  if (m_window)
    {
      // TODO: should we get the virtual size?
      m_window->GetSize(&w, &h);

      if (m_window && m_window->GetBackingPixmap())
      {
        w = m_window->GetPixmapWidth();
        h = m_window->GetPixmapHeight();
      }
    }
  else
    {
        if (this->IsKindOf(CLASSINFO(wxMemoryDC)))
        {
           wxMemoryDC* memDC = (wxMemoryDC*) this;
           w = memDC->GetBitmap().GetWidth();
           h = memDC->GetBitmap().GetHeight();
	}
        else
          return;
    }

  wxBrush saveBrush = m_brush;
  SetBrush (m_backgroundBrush);

  XFillRectangle ((Display*) m_display, (Pixmap) m_pixmap, (GC) m_gc, 0, 0, w, h);

  if (m_window && m_window->GetBackingPixmap())
    XFillRectangle ((Display*) m_display, (Pixmap) m_window->GetBackingPixmap(),(GC) m_gcBacking, 0, 0, w, h);

  m_brush = saveBrush;
};

void wxWindowDC::SetFont( const wxFont &font )
{
  if (!Ok()) return;
  
  m_font = font;

  if (!m_font.Ok())
  {
    if ((m_oldFont != (WXFont) 0) && ((long) m_oldFont != -1))
    {
      XSetFont ((Display*) m_display, (GC) m_gc, (Font) m_oldFont);

      if (m_window && m_window->GetBackingPixmap())
          XSetFont ((Display*) m_display,(GC) m_gcBacking, (Font) m_oldFont);
    }
    return;
  }

  WXFontStructPtr pFontStruct = m_font.GetFontStruct(m_userScaleY*m_logicalScaleY, m_display);

  Font fontId = ((XFontStruct*)pFontStruct)->fid;
  XSetFont ((Display*) m_display, (GC) m_gc, fontId);

  if (m_window && m_window->GetBackingPixmap())
      XSetFont ((Display*) m_display,(GC) m_gcBacking, fontId);
};

void wxWindowDC::SetPen( const wxPen &pen )
{
  if (!Ok()) return;

  m_pen = pen;
  if (!m_pen.Ok())
    return;

  wxBitmap oldStipple = m_currentStipple;
  int oldStyle = m_currentStyle;
  int oldFill = m_currentFill;
  int old_pen_width = m_currentPenWidth;
  int old_pen_join = m_currentPenJoin;
  int old_pen_cap = m_currentPenCap;
  int old_pen_nb_dash = m_currentPenDashCount;
  char *old_pen_dash = m_currentPenDash;

  wxColour oldPenColour = m_currentColour;
  m_currentColour = m_pen.GetColour ();
  m_currentStyle = m_pen.GetStyle ();
  m_currentFill = m_pen.GetStyle (); // TODO?
  m_currentPenWidth = m_pen.GetWidth ();
  m_currentPenJoin = m_pen.GetJoin ();
  m_currentPenCap = m_pen.GetCap ();
  m_currentPenDashCount = m_pen.GetDashCount();
  m_currentPenDash = m_pen.GetDash();

  if (m_currentStyle == wxSTIPPLE)
    m_currentStipple = m_pen.GetStipple ();

  bool sameStyle = (oldStyle == m_currentStyle &&
             oldFill == m_currentFill &&
             old_pen_join == m_currentPenJoin &&
             old_pen_cap == m_currentPenCap &&
             old_pen_nb_dash == m_currentPenDashCount &&
             old_pen_dash == m_currentPenDash &&
             old_pen_width == m_currentPenWidth);

  bool sameColour = (oldPenColour.Ok () &&
              (oldPenColour.Red () == m_currentColour.Red ()) &&
              (oldPenColour.Blue () == m_currentColour.Blue ()) &&
              (oldPenColour.Green () == m_currentColour.Green ()) &&
              (oldPenColour.GetPixel() == m_currentColour.GetPixel()));

  if (!sameStyle || !GetOptimization())
    {
      int scaled_width = (int) XLOG2DEVREL (m_pen.GetWidth ());
      if (scaled_width < 0)
        scaled_width = 0;

      int style;
      int join;
      int cap;
      static char dotted[] =
      {2, 5};
      static char short_dashed[] =
      {4, 4};
      static char long_dashed[] =
      {4, 8};
      static char dotted_dashed[] =
      {6, 6, 2, 6};

      // We express dash pattern in pen width unit, so we are
      // independent of zoom factor and so on...
      int req_nb_dash;
      char *req_dash;

      switch (m_pen.GetStyle ())
      {
        case wxUSER_DASH:
          req_nb_dash = m_currentPenDashCount;
          req_dash = m_currentPenDash;
          style = LineOnOffDash;
          break;
        case wxDOT:
          req_nb_dash = 2;
          req_dash = dotted;
          style = LineOnOffDash;
          break;
        case wxSHORT_DASH:
          req_nb_dash = 2;
          req_dash = short_dashed;
          style = LineOnOffDash;
          break;
        case wxLONG_DASH:
          req_nb_dash = 2;
          req_dash = long_dashed;
          style = LineOnOffDash;
          break;
        case wxDOT_DASH:
          req_nb_dash = 4;
          req_dash = dotted_dashed;
          style = LineOnOffDash;
          break;
        case wxSTIPPLE:
        case wxSOLID:
        case wxTRANSPARENT:
        default:
          style = LineSolid;
          req_dash = NULL;
          req_nb_dash = 0;
      }

      if (req_dash && req_nb_dash)
      {
        char *real_req_dash = new char[req_nb_dash];
        if (real_req_dash)
        {
          int factor = scaled_width == 0 ? 1 : scaled_width;
          for (int i = 0; i < req_nb_dash; i++)
          real_req_dash[i] = req_dash[i] * factor;
          XSetDashes ((Display*) m_display, (GC) m_gc, 0, real_req_dash, req_nb_dash);

          if (m_window && m_window->GetBackingPixmap())
          XSetDashes ((Display*) m_display,(GC) m_gcBacking, 0, real_req_dash, req_nb_dash);
          delete[]real_req_dash;
        }
        else
        {
          // No Memory. We use non-scaled dash pattern...
          XSetDashes ((Display*) m_display, (GC) m_gc, 0, req_dash, req_nb_dash);

          if (m_window && m_window->GetBackingPixmap())
          XSetDashes ((Display*) m_display,(GC) m_gcBacking, 0, req_dash, req_nb_dash);
        }
      }

      switch (m_pen.GetCap ())
      {
        case wxCAP_PROJECTING:
          cap = CapProjecting;
          break;
        case wxCAP_BUTT:
          cap = CapButt;
          break;
        case wxCAP_ROUND:
        default:
          cap = CapRound;
          break;
      }

      switch (m_pen.GetJoin ())
      {
        case wxJOIN_BEVEL:
          join = JoinBevel;
          break;
        case wxJOIN_MITER:
          join = JoinMiter;
          break;
        case wxJOIN_ROUND:
        default:
          join = JoinRound;
          break;
      }

      XSetLineAttributes ((Display*) m_display, (GC) m_gc, scaled_width, style, cap, join);

      if (m_window && m_window->GetBackingPixmap())
        XSetLineAttributes ((Display*) m_display,(GC) m_gcBacking, scaled_width, style, cap, join);
    }

    if (IS_HATCH(m_currentFill) && ((m_currentFill != oldFill) || !GetOptimization()))
    {
      Pixmap myStipple;

      oldStipple = (wxBitmap*) NULL;    // For later reset!!

      switch (m_currentFill)
      {
        case wxBDIAGONAL_HATCH:
          if (bdiag == (Pixmap) 0)
            bdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         bdiag_bits, bdiag_width, bdiag_height);
          myStipple = bdiag;
          break;
        case wxFDIAGONAL_HATCH:
          if (fdiag == (Pixmap) 0)
            fdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         fdiag_bits, fdiag_width, fdiag_height);
          myStipple = fdiag;
          break;
        case wxCROSS_HATCH:
          if (cross == (Pixmap) 0)
            cross = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         cross_bits, cross_width, cross_height);
          myStipple = cross;
          break;
        case wxHORIZONTAL_HATCH:
          if (horiz == (Pixmap) 0)
            horiz = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         horiz_bits, horiz_width, horiz_height);
          myStipple = horiz;
          break;
        case wxVERTICAL_HATCH:
          if (verti == (Pixmap) 0)
            verti = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         verti_bits, verti_width, verti_height);
          myStipple = verti;
          break;
        case wxCROSSDIAG_HATCH:
        default:
          if (cdiag == (Pixmap) 0)
            cdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         cdiag_bits, cdiag_width, cdiag_height);
          myStipple = cdiag;
          break;
      }
      XSetStipple ((Display*) m_display, (GC) m_gc, myStipple);

      if (m_window && m_window->GetBackingPixmap())
        XSetStipple ((Display*) m_display,(GC) m_gcBacking, myStipple);
    }
    else if (m_currentStipple.Ok()
       && ((m_currentStipple != oldStipple) || !GetOptimization()))
    {
      XSetStipple ((Display*) m_display, (GC) m_gc, (Pixmap) m_currentStipple.GetPixmap());

      if (m_window && m_window->GetBackingPixmap())
        XSetStipple ((Display*) m_display,(GC) m_gcBacking, (Pixmap) m_currentStipple.GetPixmap());
    }

    if ((m_currentFill != oldFill) || !GetOptimization())
    {
      int fill_style;

      if (m_currentFill == wxSTIPPLE)
        fill_style = FillStippled;
      else if (IS_HATCH (m_currentFill))
        fill_style = FillStippled;
      else
        fill_style = FillSolid;
      XSetFillStyle ((Display*) m_display, (GC) m_gc, fill_style);
      if (m_window && m_window->GetBackingPixmap())
        XSetFillStyle ((Display*) m_display,(GC) m_gcBacking, fill_style);
    }

    // must test m_logicalFunction, because it involves background!
    if (!sameColour || !GetOptimization()
      || ((m_logicalFunction == wxXOR) || (m_autoSetting & 0x2)))
    {
      int pixel = -1;
      if (m_pen.GetStyle () == wxTRANSPARENT)
        pixel = m_backgroundPixel;
      else if (!m_colour)
      {
        unsigned char red = m_pen.GetColour ().Red ();
        unsigned char blue = m_pen.GetColour ().Blue ();
        unsigned char green = m_pen.GetColour ().Green ();
        if (red == (unsigned char) 255 && blue == (unsigned char) 255
            && green == (unsigned char) 255)
        {
          pixel = (int) WhitePixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
          m_currentColour = *wxWHITE;
          m_pen.GetColour().SetPixel(pixel);
          m_currentColour.SetPixel(pixel);
        }
        else
        {
          pixel = (int) BlackPixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
          m_currentColour = *wxBLACK;
          m_pen.GetColour().SetPixel(pixel);
        }
      }
      else
      {
        pixel = m_pen.GetColour ().AllocColour(m_display);
        m_currentColour.SetPixel(pixel);
      }

      // Finally, set the GC to the required colour
      if (pixel > -1)
      {
        if (m_logicalFunction == wxXOR)
        {
          XGCValues values;
          XGetGCValues ((Display*) m_display, (GC) m_gc, GCBackground, &values);
          XSetForeground ((Display*) m_display, (GC) m_gc, pixel ^ values.background);
          if (m_window && m_window->GetBackingPixmap())
            XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel ^ values.background);
        }
      else
        {
          XSetForeground ((Display*) m_display, (GC) m_gc, pixel);
          if (m_window && m_window->GetBackingPixmap())
            XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel);
        }
    }
  }
  else
    m_pen.GetColour().SetPixel(oldPenColour.GetPixel());

  m_autoSetting = 0;
};

void wxWindowDC::SetBrush( const wxBrush &brush )
{
  if (!Ok()) return;
  
  m_brush = brush;

  if (!m_brush.Ok() || m_brush.GetStyle () == wxTRANSPARENT)
    return;

  int oldFill = m_currentFill;
  wxBitmap oldStipple = m_currentStipple;

  m_autoSetting |= 0x1;

  m_currentFill = m_brush.GetStyle ();
  if (m_currentFill == wxSTIPPLE)
    m_currentStipple = m_brush.GetStipple ();

  wxColour oldBrushColour(m_currentColour);
  m_currentColour = m_brush.GetColour ();

  bool sameColour = (oldBrushColour.Ok () &&
              (oldBrushColour.Red () == m_currentColour.Red ()) &&
              (oldBrushColour.Blue () == m_currentColour.Blue ()) &&
           (oldBrushColour.Green () == m_currentColour.Green ()) &&
              (oldBrushColour.GetPixel() == m_currentColour.GetPixel()));

  if ((oldFill != m_brush.GetStyle ()) || !GetOptimization())
  {
      switch (brush.GetStyle ())
      {
        case wxTRANSPARENT:
          break;
        case wxBDIAGONAL_HATCH:
        case wxCROSSDIAG_HATCH:
        case wxFDIAGONAL_HATCH:
        case wxCROSS_HATCH:
        case wxHORIZONTAL_HATCH:
        case wxVERTICAL_HATCH:
        case wxSTIPPLE:
          {
            // Chris Breeze 23/07/97: use background mode to determine whether
            // fill style should be solid or transparent
            int style = (m_backgroundMode == wxSOLID ? FillOpaqueStippled : FillStippled);
            XSetFillStyle ((Display*) m_display, (GC) m_gc, style);
            if (m_window && m_window->GetBackingPixmap())
              XSetFillStyle ((Display*) m_display,(GC) m_gcBacking, style);
          }
          break;
        case wxSOLID:
        default:
          XSetFillStyle ((Display*) m_display, (GC) m_gc, FillSolid);
          if (m_window && m_window->GetBackingPixmap())
            XSetFillStyle ((Display*) m_display,(GC) m_gcBacking, FillSolid);
      }
  }

  if (IS_HATCH(m_currentFill) && ((m_currentFill != oldFill) || !GetOptimization()))
    {
      Pixmap myStipple;

      switch (m_currentFill)
      {
        case wxBDIAGONAL_HATCH:
          if (bdiag == (Pixmap) 0)
            bdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         bdiag_bits, bdiag_width, bdiag_height);
          myStipple = bdiag;
          break;
        case wxFDIAGONAL_HATCH:
          if (fdiag == (Pixmap) 0)
            fdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         fdiag_bits, fdiag_width, fdiag_height);
          myStipple = fdiag;
          break;
        case wxCROSS_HATCH:
          if (cross == (Pixmap) 0)
            cross = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         cross_bits, cross_width, cross_height);
          myStipple = cross;
          break;
        case wxHORIZONTAL_HATCH:
          if (horiz == (Pixmap) 0)
            horiz = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         horiz_bits, horiz_width, horiz_height);
          myStipple = horiz;
          break;
        case wxVERTICAL_HATCH:
          if (verti == (Pixmap) 0)
            verti = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         verti_bits, verti_width, verti_height);
          myStipple = verti;
          break;
        case wxCROSSDIAG_HATCH:
        default:
          if (cdiag == (Pixmap) 0)
            cdiag = XCreateBitmapFromData ((Display*) m_display,
                      RootWindow ((Display*) m_display, DefaultScreen ((Display*) m_display)),
                         cdiag_bits, cdiag_width, cdiag_height);
          myStipple = cdiag;
          break;
      }
      XSetStipple ((Display*) m_display, (GC) m_gc, myStipple);

      if (m_window && m_window->GetBackingPixmap())
        XSetStipple ((Display*) m_display,(GC) m_gcBacking, myStipple);
    }
    // X can forget the stipple value when resizing a window (apparently)
    // so always set the stipple.
    else if (m_currentStipple.Ok()) // && m_currentStipple != oldStipple)
    {
      XSetStipple ((Display*) m_display, (GC) m_gc, (Pixmap) m_currentStipple.GetPixmap());
      if (m_window && m_window->GetBackingPixmap())
        XSetStipple ((Display*) m_display,(GC) m_gcBacking, (Pixmap) m_currentStipple.GetPixmap());
    }

    // must test m_logicalFunction, because it involves background!
    if (!sameColour || !GetOptimization() || m_logicalFunction == wxXOR)
    {
      int pixel = -1;
      if (!m_colour)
      {
          // Policy - on a monochrome screen, all brushes are white,
          // except when they're REALLY black!!!
          unsigned char red = m_brush.GetColour ().Red ();
          unsigned char blue = m_brush.GetColour ().Blue ();
          unsigned char green = m_brush.GetColour ().Green ();

          if (red == (unsigned char) 0 && blue == (unsigned char) 0
              && green == (unsigned char) 0)
            {
              pixel = (int) BlackPixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
              m_currentColour = *wxBLACK;
              m_brush.GetColour().SetPixel(pixel);
              m_currentColour.SetPixel(pixel);
            }
          else
            {
              pixel = (int) WhitePixel ((Display*) m_display, DefaultScreen ((Display*) m_display));
              m_currentColour = *wxWHITE;
              m_brush.GetColour().SetPixel(pixel);
              m_currentColour.SetPixel(pixel);
            }

          // N.B. comment out the above line and uncomment the following lines
          // if you want non-white colours to be black on a monochrome display.
          /*
             if (red == (unsigned char )255 && blue == (unsigned char)255
             && green == (unsigned char)255)
             pixel = (int)WhitePixel((Display*) m_display, DefaultScreen((Display*) m_display));
             else
             pixel = (int)BlackPixel((Display*) m_display, DefaultScreen((Display*) m_display));
           */
      }
      else if (m_brush.GetStyle () != wxTRANSPARENT)
      {
        pixel = m_brush.GetColour().AllocColour(m_display);
        m_currentColour.SetPixel(pixel);
      }
      if (pixel > -1)
      {
        // Finally, set the GC to the required colour
        if (m_logicalFunction == wxXOR)
        {
          XGCValues values;
          XGetGCValues ((Display*) m_display, (GC) m_gc, GCBackground, &values);
          XSetForeground ((Display*) m_display, (GC) m_gc, pixel ^ values.background);
          if (m_window && m_window->GetBackingPixmap())
            XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel ^ values.background);
        }
        else
        {
          XSetForeground ((Display*) m_display, (GC) m_gc, pixel);
          if (m_window && m_window->GetBackingPixmap())
            XSetForeground ((Display*) m_display,(GC) m_gcBacking, pixel);
        }
      }
    }
  else
    m_brush.GetColour().SetPixel(oldBrushColour.GetPixel());
};

void wxWindowDC::SetBackground( const wxBrush &brush )
{
  if (!Ok()) return;
  
  m_backgroundBrush = brush;

  if (!m_backgroundBrush.Ok())
    return;

  int pixel = m_backgroundBrush.GetColour().AllocColour(m_display);

  // XSetWindowBackground doesn't work for non-Window pixmaps
  if (!this->IsKindOf(CLASSINFO(wxMemoryDC)))
      XSetWindowBackground ((Display*) m_display, (Pixmap) m_pixmap, pixel);

  // Necessary for ::DrawIcon, which use fg/bg pixel or the GC.
  // And Blit,... (Any fct that use XCopyPlane, in fact.)
  XSetBackground ((Display*) m_display, (GC) m_gc, pixel);
  if (m_window && m_window->GetBackingPixmap())
     XSetBackground ((Display*) m_display,(GC) m_gcBacking, pixel);
};

void wxWindowDC::SetLogicalFunction( int function )
{
  int x_function;

  /* MATTHEW: [9] */
  if (m_logicalFunction == function)
    return;

  switch (function)
    {
    case wxCLEAR:
      x_function = GXclear;
      break;
    case wxXOR:
      x_function = GXxor;
      break;
    case wxINVERT:
      x_function = GXinvert;
      break;
    case wxOR_REVERSE:
      x_function = GXorReverse;
      break;
    case wxAND_REVERSE:
      x_function = GXandReverse;
      break;
    case wxAND:
      x_function = GXand;
      break;
    case wxOR:
      x_function = GXor;
      break;
    case wxAND_INVERT:
      x_function = GXandInverted;
      break;
    case wxNO_OP:
      x_function = GXnoop;
      break;
    case wxNOR:
      x_function = GXnor;
      break;
    case wxEQUIV:
      x_function = GXequiv;
      break;
    case wxSRC_INVERT:
      x_function = GXcopyInverted;
      break;
    case wxOR_INVERT:
      x_function = GXorInverted;
      break;
    case wxNAND:
      x_function = GXnand;
      break;
    case wxSET:
      x_function = GXset;
      break;
    case wxCOPY:
    default:
      x_function = GXcopy;
      break;
    }

  XSetFunction((Display*) m_display, (GC) m_gc, x_function);
  if (m_window && m_window->GetBackingPixmap())
    XSetFunction((Display*) m_display, (GC) m_gcBacking, x_function);

  if ((m_logicalFunction == wxXOR) != (function == wxXOR))
    /* MATTHEW: [9] Need to redo pen simply */
    m_autoSetting |= 0x2;

  m_logicalFunction = function;

};

void wxWindowDC::SetTextForeground( const wxColour &col )
{
  if (!Ok()) return;
  
  if (m_textForegroundColour == col) return;
  
  m_textForegroundColour = col;

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

};

void wxWindowDC::SetPalette( const wxPalette& palette )
{
  if (m_window)
  {
    if (palette.Ok())
      /* Use GetXColormap */
      XSetWindowColormap ((Display*) m_display, (Window) m_window->GetXWindow(),
              (Colormap) palette.GetXColormap());
    else
      /* Use wxGetMainColormap */
      XSetWindowColormap ((Display*) m_display, (Window) m_window->GetXWindow(),
              (Colormap) wxTheApp->GetMainColormap(m_display));
  }
};

// Helper function
void wxWindowDC:: SetDCClipping ()
{
  // m_userRegion is the region set by calling SetClippingRegion

  if (m_currentRegion)
    XDestroyRegion ((Region) m_currentRegion);

  // We need to take into account
  // clipping imposed on a window by a repaint.
  // We'll combine it with the user region. But for now,
  // just use the currently-defined user clipping region.
  if (m_userRegion || (m_window && m_window->GetUpdateRegion().Ok()) )
    m_currentRegion = (WXRegion) XCreateRegion ();
  else
    m_currentRegion = (WXRegion) NULL;

  if ((m_window && m_window->GetUpdateRegion().Ok()) && m_userRegion)
    XIntersectRegion ((Region) m_window->GetUpdateRegion().GetXRegion(), (Region) m_userRegion, (Region) m_currentRegion);
  else if (m_userRegion)
    XIntersectRegion ((Region) m_userRegion, (Region) m_userRegion, (Region) m_currentRegion);
  else if (m_window && m_window->GetUpdateRegion().Ok())
    XIntersectRegion ((Region) m_window->GetUpdateRegion().GetXRegion(), (Region) m_window->GetUpdateRegion().GetXRegion(), 
(Region) m_currentRegion);

  if (m_currentRegion)
    {
      XSetRegion ((Display*) m_display, (GC) m_gc, (Region) m_currentRegion);
    }
  else
    {
      XSetClipMask ((Display*) m_display, (GC) m_gc, None);
    }

}

void wxWindowDC::SetClippingRegion( long x, long y, long width, long height )
{
  wxDC::SetClippingRegion( x, y, width, height );

  if (m_userRegion)
    XDestroyRegion ((Region) m_userRegion);
  m_userRegion = (WXRegion) XCreateRegion ();
  XRectangle r;
  r.x = XLOG2DEV (x);
  r.y = YLOG2DEV (y);
  r.width = XLOG2DEVREL(width);
  r.height = YLOG2DEVREL(height);
  XUnionRectWithRegion (&r, (Region) m_userRegion, (Region) m_userRegion);

  SetDCClipping ();

  // Needs to work differently for Pixmap: without this,
  // there's a nasty (Display*) m_display bug. 8/12/94
  if (m_window && m_window->GetBackingPixmap())
  {
    XRectangle rects[1];
    rects[0].x = XLOG2DEV_2(x); 
    rects[0].y = YLOG2DEV_2(y);
    rects[0].width = XLOG2DEVREL(width);
    rects[0].height = YLOG2DEVREL(height);
    XSetClipRectangles((Display*) m_display, (GC) m_gcBacking, 0, 0, rects, 1, Unsorted);
  }
};

void wxWindowDC::DestroyClippingRegion(void)
{
  wxDC::DestroyClippingRegion();
  
  if (m_userRegion)
    XDestroyRegion ((Region) m_userRegion);
  m_userRegion = NULL;

  SetDCClipping ();

  XGCValues gc_val;
  gc_val.clip_mask = None;
  if (m_window && m_window->GetBackingPixmap())
    XChangeGC((Display*) m_display, (GC) m_gcBacking, GCClipMask, &gc_val);
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

#define        half(z1, z2)    ((z1+z2)/2.0)
#define        THRESHOLD    5

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
};

/*
 * wxPaintDC
 */

wxPaintDC::wxPaintDC(wxWindow* win): wxWindowDC(win)
{
    wxRegion* region = NULL;

    // Combine all the update rects into a region
    if (win->m_updateRects.Number() > 0)
    {
        int i = 0;
        for (i = 0; i < win->m_updateRects.Number(); i++)
        {
            wxRect* rect = (wxRect*) win->m_updateRects.Nth(i)->Data();
	    /*
            cout << "wxPaintDC. wxRect: " << rect->x << ", " << rect->y << ", ";
            cout << rect->width << ", " << rect->height << "\n\n";
	    */

            if (!region)
                region = new wxRegion(*rect);
            else
                // TODO: is this correct? In SetDCClipping above,
                // XIntersectRegion is used to combine paint and user
                // regions. XIntersectRegion appears to work in that case...
                region->Union(*rect);
        }
    }
    else
    {
        int cw, ch;
        win->GetClientSize(&cw, &ch);
        region = new wxRegion(wxRect(0, 0, cw, ch));
    }

    win->m_updateRegion = *region;

    // Set the clipping region. Any user-defined region will be combined with this
    // one in SetDCClipping.
    XSetRegion ((Display*) m_display, (GC) m_gc, (Region) region->GetXRegion());

    delete region;
}

wxPaintDC::~wxPaintDC()
{
    XSetClipMask ((Display*) m_display, (GC) m_gc, None);
    if (m_window)
        m_window->m_updateRegion.Clear();
}

