/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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
public:
  wxDC();
  ~wxDC();

  // Compatibility (obsolete)
  inline void wxDC::BeginDrawing() {}
  inline void wxDC::EndDrawing() {}

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
  virtual void CrossHair(const wxPoint& pt)
  {
    CrossHair(pt.x, pt.y);
  }

  virtual void DrawArc(long x1,long y1,long x2,long y2,double xc, double yc);
  inline void DrawArc(const wxPoint& pt1, const wxPoint& pt2, double xc, double yc)
  {
    DrawArc(pt1.x, pt1.y, pt2.x, pt2.y, xc, yc);
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

#if USE_SPLINES
  // Splines
  // 3-point spline
  virtual void DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3);
  // Any number of control points - a list of pointers to wxPoints
  virtual void DrawSpline(wxList *points);
  virtual void DrawSpline(int n, wxPoint points[]);
#endif
  virtual void Clear();
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

  virtual void SetPalette(const wxPalette& palette);
  virtual void DestroyClippingRegion();
  virtual long GetCharHeight() const;
  virtual long GetCharWidth() const;
  virtual void GetTextExtent(const wxString& string, long *x, long *y,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16bit = FALSE) const;

  // Size in device units
  virtual void GetSize(int* width, int* height) const;
  inline wxSize GetSize() const { int w, h; GetSize(&w, &h); return wxSize(w, h); }

  // Size in mm
  virtual void GetSizeMM(long* width, long* height) const ;

  virtual bool StartDoc(const wxString& message);
  virtual void EndDoc();
  virtual void StartPage();
  virtual void EndPage();
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

  virtual bool CanDrawBitmap() const;
  virtual bool CanGetTextExtent() const;

  virtual void SetTextForeground(const wxColour& colour);
  virtual void SetTextBackground(const wxColour& colour);
  inline virtual bool Ok() const {return m_ok;};
  inline virtual int  GetMapMode() const {return m_mappingMode;};

  inline virtual wxBrush *GetBackground() const { return (wxBrush*) &m_backgroundBrush ;}
  inline virtual wxBrush *GetBrush() const { return (wxBrush*) &m_brush ;}
  inline virtual wxFont  *GetFont() const { return (wxFont*) &m_font ;}
  inline virtual int      GetLogicalFunction() const { return m_logicalFunction ;}
  inline virtual wxPen   *GetPen() const { return (wxPen*) &m_pen ;}
  inline virtual wxColour&GetTextBackground() const { return (wxColour&) m_textBackgroundColour ;}
  inline virtual wxColour&GetTextForeground() const { return (wxColour&) m_textForegroundColour ;}

  virtual void SetLogicalScale(double x, double y);
  virtual inline  void GetUserScale(double* x, double *y) const { *x = m_userScaleX; *y = m_userScaleY; }
  virtual void CalcBoundingBox(long x, long y);
  // Get the final bounding box of the PostScript or Metafile picture.
  virtual inline long MinX() const { return m_minX; }
  virtual inline long MaxX() const { return m_maxX; }
  virtual inline long MinY() const { return m_minY; }
  virtual inline long MaxY() const { return m_maxY; }

  // Sometimes we need to override optimization, e.g.
  // if other software is drawing onto our surface and we
  // can't be sure of who's done what.
  virtual inline void SetOptimization(bool WXUNUSED(opt)) { }
  virtual inline bool GetOptimization() { return FALSE; }

  virtual void GetClippingBox(long *x,long *y,long *w,long *h) const ;
  inline void GetClippingBox(wxRect& rect) const
  {
    long x, y, w, h;
    GetClippingBox(&x, &y, &w, &h); rect.x = x; rect.y = y; rect.width = w; rect.height = h;
  }

  inline wxWindow *GetWindow() const { return m_canvas; }
  inline void SetWindow(wxWindow *win) { m_canvas = win; }

protected:
  bool              m_colour;
  bool              m_ok;
  bool              m_clipping;
  bool              m_isInteractive;

  // Coordinate system variables
  long              m_logicalOriginX;
  long              m_logicalOriginY;

  long              m_deviceOriginX;
  long              m_deviceOriginY;

  double            m_logicalScaleX;
  double            m_logicalScaleY;

  double            m_userScaleX;
  double            m_userScaleY;

  int               m_signX;		// Used by SetAxisOrientation() to
  int               m_signY;		// invert the axes

  int               m_mappingMode;

  long              m_minX;          // bounding box
  long              m_minY;
  long              m_maxX;
  long              m_maxY;

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
  double            m_systemScaleX;
  double            m_systemScaleY;

  wxWindow *        m_canvas;
  wxBitmap          m_selectedBitmap;
  wxString          m_filename;

};

#endif
    // _WX_DC_H_
