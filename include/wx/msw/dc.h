/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#ifdef __GNUG__
#pragma interface "dc.h"
#endif

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

class WXDLLEXPORT wxDC: public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxDC)
 protected:
public:
  wxDC(void);
  ~wxDC(void);

  inline void wxDC::BeginDrawing(void) {}
  inline void wxDC::EndDrawing(void) {}

  virtual void FloodFill(long x1, long y1, const wxColour& col, int style=wxFLOOD_SURFACE) ;
  inline void FloodFill(const wxPoint& pt, const wxColour& col, int style=wxFLOOD_SURFACE)
  {
    FloodFill(pt.x, pt.y, col, style);
  }

  virtual bool GetPixel(long x1, long y1, wxColour *col) const ;
  inline bool GetPixel(const wxPoint& pt, wxColour *col) const
  {
    return GetPixel(pt.x, pt.y, col);
  }

  virtual void DrawLine(long x1, long y1, long x2, long y2);
  inline void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
  {
    DrawLine(pt1.x, pt1.y, pt2.x, pt2.y);
  }

  virtual void CrossHair(long x, long y) ;
  inline void CrossHair(const wxPoint& pt)
  {
    CrossHair(pt.x, pt.y);
  }

  virtual void DrawArc(long x1,long y1,long x2,long y2,long xc, long yc);
  inline void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
  {
    DrawArc(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y);
  }

  virtual void DrawEllipticArc (long x, long y, long w, long h, double sa, double ea);
  virtual void DrawEllipticArc (const wxPoint& pt, const wxSize& sz, double sa, double ea)
  {
    DrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea);
  }

  virtual void DrawPoint(long x, long y);
  inline void DrawPoint(const wxPoint& pt)
  {
    DrawPoint(pt.x, pt.y);
  }

  virtual void DrawLines(int n, wxPoint points[], long xoffset = 0, long yoffset = 0);

  virtual void DrawPolygon(int n, wxPoint points[], long xoffset = 0, long yoffset = 0, int fillStyle=wxODDEVEN_RULE);

  virtual void DrawRectangle(long x, long y, long width, long height);
  inline void DrawRectangle(const wxPoint& pt, const wxSize& sz)
  {
    DrawRectangle(pt.x, pt.y, sz.x, sz.y);
  }
  inline void DrawRectangle(const wxRect& rect)
  {
    DrawRectangle(rect.x, rect.y, rect.width, rect.height);
  }

  virtual void DrawRoundedRectangle(long x, long y, long width, long height, double radius = 20.0);
  inline void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz, double radius = 20.0)
  {
    DrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius);
  }
  inline void DrawRoundedRectangle(const wxRect& rect, double radius = 20.0)
  {
    DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, radius);
  }

  virtual void DrawEllipse(long x, long y, long width, long height);
  inline void DrawEllipse(const wxPoint& pt, const wxSize& sz)
  {
    DrawEllipse(pt.x, pt.y, sz.x, sz.y);
  }
  inline void DrawEllipse(const wxRect& rect)
  {
    DrawEllipse(rect.x, rect.y, rect.width, rect.height);
  }

  virtual void DrawIcon(const wxIcon& icon, long x, long y);
  inline void DrawIcon(const wxIcon& icon, const wxPoint& pt)
  {
    DrawIcon(icon, pt.x, pt.y);
  }

  inline void DrawPoint(wxPoint& point) { DrawPoint(point.x, point.y); }
  virtual void DrawLines(wxList *list, long xoffset = 0, long yoffset = 0);
  virtual void DrawPolygon(wxList *list, long xoffset = 0, long yoffset = 0, int fillStyle=wxODDEVEN_RULE);

  virtual void DrawText(const wxString& text, long x, long y, bool use16bit = FALSE);
  inline void DrawText(const wxString& text, const wxPoint& pt, bool use16bit = FALSE)
  {
    DrawText(text, pt.x, pt.y, use16bit);
  }

  virtual bool Blit(long xdest, long ydest, long width, long height,
            wxDC *source, long xsrc, long ysrc, int rop = wxCOPY, bool useMask = FALSE);
  inline bool Blit(const wxPoint& destPt, const wxSize& sz,
            wxDC *source, const wxPoint& srcPt, int rop = wxCOPY, bool useMask = FALSE)
  {
    return Blit(destPt.x, destPt.y, sz.x, sz.y, source, srcPt.x, srcPt.y, rop, useMask);
  }

#if wxUSE_SPLINES
  // Splines
  // 3-point spline
  virtual void DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3);
  // Any number of control points - a list of pointers to wxPoints
  virtual void DrawSpline(wxList *points);
  virtual void DrawSpline(int n, wxPoint points[]);
#endif
  virtual void Clear(void);
  virtual void SetFont(const wxFont& font);
  virtual void SetPen(const wxPen& pen);
  virtual void SetBrush(const wxBrush& brush);
  virtual void SetLogicalFunction(int function);
  virtual void SetBackground(const wxBrush& brush);
  virtual void SetBackgroundMode(int mode);

  virtual void SetClippingRegion(long x, long y, long width, long height);
  inline void SetClippingRegion(const wxPoint& pt, const wxSize& sz)
  {
    SetClippingRegion(pt.x, pt.y, sz.x, sz.y);
  }
  inline void SetClippingRegion(const wxRect& rect)
  {
    SetClippingRegion(rect.x, rect.y, rect.width, rect.height);
  }
  virtual void SetClippingRegion(const wxRegion& region);

  virtual void SetPalette(const wxPalette& palette);
#if WXWIN_COMPATIBILITY
  virtual inline void SetColourMap(const wxPalette& palette) { SetPalette(palette); };
#endif
  virtual void DestroyClippingRegion(void);
  virtual long GetCharHeight(void) const;
  virtual long GetCharWidth(void) const;
  virtual void GetTextExtent(const wxString& string, long *x, long *y,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16bit = FALSE) const;
#if WXWIN_COMPATIBILITY
  void GetTextExtent(const wxString& string, float *x, float *y,
                     float *descent = NULL, float *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16bit = FALSE) const ;
#endif

  // Size in device units
  virtual void GetSize(int* width, int* height) const;
  inline wxSize GetSize(void) const { int w, h; GetSize(&w, &h); return wxSize(w, h); }

  // Size in mm
  virtual void GetSizeMM(long* width, long* height) const ;

  // Compatibility
#if WXWIN_COMPATIBILITY
  inline void GetSize(float* width, float* height) const { int w, h; GetSize(& w, & h); *width = w; *height = h; }
  inline void GetSizeMM(float *width, float *height) const { long w, h; GetSizeMM(& w, & h); *width = (float) w; *height = (float) h; }
#endif

  virtual bool StartDoc(const wxString& message);
  virtual void EndDoc(void);
  virtual void StartPage(void);
  virtual void EndPage(void);
  virtual void SetMapMode(int mode);
  virtual void SetUserScale(double x, double y);
  virtual void SetSystemScale(double x, double y);
  virtual void SetLogicalOrigin(long x, long y);
  virtual void SetDeviceOrigin(long x, long y);
  virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);

  // This group of functions does actual conversion
  // of the input, as you'd expect.

  long DeviceToLogicalX(long x) const;
  long DeviceToLogicalY(long y) const;
  long DeviceToLogicalXRel(long x) const;
  long DeviceToLogicalYRel(long y) const;
  long LogicalToDeviceX(long x) const;
  long LogicalToDeviceY(long y) const;
  long LogicalToDeviceXRel(long x) const;
  long LogicalToDeviceYRel(long y) const;

  // This group of functions may not do any conversion
  // if m_scaleGDI is TRUE, since the HDC does the
  // conversion automatically.
  // m_scaleGDI NOW OBSOLETE
  long ImplDeviceToLogicalX(long x) const;
  long ImplDeviceToLogicalY(long y) const;
  long ImplDeviceToLogicalXRel(long x) const;
  long ImplDeviceToLogicalYRel(long y) const;
  long ImplLogicalToDeviceX(long x) const;
  long ImplLogicalToDeviceY(long y) const;
  long ImplLogicalToDeviceXRel(long x) const;
  long ImplLogicalToDeviceYRel(long y) const;

  virtual bool CanDrawBitmap(void) const;
  virtual bool CanGetTextExtent(void) const;

  virtual void SetTextForeground(const wxColour& colour);
  virtual void SetTextBackground(const wxColour& colour);
  inline virtual bool Ok(void) const {return m_ok;};
  inline virtual int  GetMapMode(void) const {return m_mappingMode;};

  inline virtual wxBrush *GetBackground(void) const { return (wxBrush*) &m_backgroundBrush ;}
  inline virtual wxBrush *GetBrush(void) const { return (wxBrush*) &m_brush ;}
  inline virtual wxFont  *GetFont(void) const { return (wxFont*) &m_font ;}
  inline virtual int      GetLogicalFunction(void) const { return m_logicalFunction ;}
  inline virtual wxPen   *GetPen(void) const { return (wxPen*) &m_pen ;}
  inline virtual wxColour&GetTextBackground(void) const { return (wxColour&) m_textBackgroundColour ;}
  inline virtual wxColour&GetTextForeground(void) const { return (wxColour&) m_textForegroundColour ;}

  virtual void SetLogicalScale(double x, double y);
  virtual inline  void GetUserScale(double* x, double *y) const { *x = m_userScaleX; *y = m_userScaleY; }
  virtual void CalcBoundingBox(long x, long y);
  // Get the final bounding box of the PostScript or Metafile picture.
  virtual inline long MinX(void) const { return m_minX; }
  virtual inline long MaxX(void) const { return m_maxX; }
  virtual inline long MinY(void) const { return m_minY; }
  virtual inline long MaxY(void) const { return m_maxY; }
  // Sometimes we need to override optimization, e.g.
  // if other software is drawing onto our surface and we
  // can't be sure of who's done what.
  virtual inline void SetOptimization(bool WXUNUSED(opt)) { }
  virtual inline bool GetOptimization(void) { return FALSE; }

  virtual void GetClippingBox(long *x,long *y,long *w,long *h) const ;
  inline void GetClippingBox(wxRect& rect) const
  {
    long x, y, w, h;
    GetClippingBox(&x, &y, &w, &h); rect.x = x; rect.y = y; rect.width = w; rect.height = h;
  }

  // This should probably be made available on other platforms
  int wxDC::GetDepth(void) const ;

// Implementation
  virtual void SetRop(WXHDC cdc);
  virtual void DoClipping(WXHDC cdc);
  virtual void SelectOldObjects(WXHDC dc);

  inline wxWindow *GetWindow(void) const { return m_canvas; }
  inline void SetWindow(wxWindow *win) { m_canvas = win; }
  inline WXHDC GetHDC(void) const { return m_hDC; }
  inline void SetHDC(WXHDC dc, bool bOwnsDC = FALSE) { m_hDC = dc; m_bOwnsDC = bOwnsDC; }

protected:
  bool              m_colour;
  bool              m_ok;
  bool              m_clipping;
  bool              m_isInteractive;

  // Coordinate system variables
  long             m_logicalOriginX;
  long             m_logicalOriginY;

  long             m_deviceOriginX;
  long             m_deviceOriginY;

  double             m_logicalScaleX;
  double             m_logicalScaleY;

  double             m_userScaleX;
  double             m_userScaleY;

  int                m_signX;		// Used by SetAxisOrientation() to
  int                m_signY;		// invert the axes

  int               m_mappingMode;

  long             m_minX;          // bounding box
  long             m_minY;
  long             m_maxX;
  long             m_maxY;

  int               m_logicalFunction;
  int               m_backgroundMode;

  wxPen             m_pen;
  wxBrush           m_brush;
  wxBrush           m_backgroundBrush;
  wxColour          m_textForegroundColour;
  wxColour          m_textBackgroundColour;
  wxFont            m_font;
  wxPalette         m_palette;
  int               m_clipX1;
  int               m_clipY1;
  int               m_clipX2;
  int               m_clipY2;
//  bool              m_dontDelete;
  int               m_windowExtX;
  int               m_windowExtY;
  double            m_systemScaleX;
  double            m_systemScaleY;

  wxWindow *        m_canvas;
  wxBitmap          m_selectedBitmap;
  wxString          m_filename;

  // TRUE => DeleteDC() in dtor, FALSE => only ReleaseDC() it
  bool              m_bOwnsDC;

  WXHDC             m_hDC;
  int               m_hDCCount;

  // Store all old GDI objects when do a SelectObject,
  // so we can select them back in (this unselecting user's
  // objects) so we can safely delete the DC.
  WXHBITMAP         m_oldBitmap;
  WXHPEN            m_oldPen;
  WXHBRUSH          m_oldBrush;
  WXHFONT           m_oldFont;
  WXHPALETTE        m_oldPalette;

  // Stores scaling, translation, rotation
//  wxTransformMatrix	m_transformMatrix;

  // Do we wish to scale GDI objects too, e.g. pen width?
//  bool				m_scaleGDI;
};

// Logical to device
// Absolute
#define XLOG2DEV(x) ImplLogicalToDeviceX(x)

#define YLOG2DEV(y) ImplLogicalToDeviceY(y)

// Relative
#define XLOG2DEVREL(x) ImplLogicalToDeviceXRel(x)
#define YLOG2DEVREL(y) ImplLogicalToDeviceYRel(y)

// Device to logical
// Absolute
#define XDEV2LOG(x) ImplDeviceToLogicalX(x)

#define YDEV2LOG(y) ImplDeviceToLogicalY(y)

// Relative
#define XDEV2LOGREL(x) ImplDeviceToLogicalXRel(x)
#define YDEV2LOGREL(y) ImplDeviceToLogicalYRel(y)

/*
 * Have the same macros as for XView but not for every operation:
 * just for calculating window/viewport extent (a better way of scaling).
 */

// Logical to device
// Absolute
#define MS_XLOG2DEV(x) LogicalToDevice(x)

#define MS_YLOG2DEV(y) LogicalToDevice(y)

// Relative
#define MS_XLOG2DEVREL(x) LogicalToDeviceXRel(x)
#define MS_YLOG2DEVREL(y) LogicalToDeviceYRel(y)

// Device to logical
// Absolute
#define MS_XDEV2LOG(x) DeviceToLogicalX(x)

#define MS_YDEV2LOG(y) DeviceToLogicalY(y)

// Relative
#define MS_XDEV2LOGREL(x) DeviceToLogicalXRel(x)
#define MS_YDEV2LOGREL(y) DeviceToLogicalYRel(y)

#define MM_POINTS      7
#define MM_METRIC      8

extern int wxPageNumber;

// Conversion
#define METRIC_CONVERSION_CONSTANT  0.0393700787

// Scaling factors for various unit conversions
#define mm2inches (METRIC_CONVERSION_CONSTANT)
#define inches2mm (1/METRIC_CONVERSION_CONSTANT)

#define mm2twips (METRIC_CONVERSION_CONSTANT*1440)
#define twips2mm (1/(METRIC_CONVERSION_CONSTANT*1440))

#define mm2pt (METRIC_CONVERSION_CONSTANT*72)
#define pt2mm (1/(METRIC_CONVERSION_CONSTANT*72))

#define     wx_round(a)    (int)((a)+.5)


#endif
    // _WX_DC_H_
