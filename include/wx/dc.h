/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05/25/99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_BASE_
#define _WX_DC_H_BASE_

#ifdef __GNUG__
    #pragma interface "dcbase.h"
#endif

// ----------------------------------------------------------------------------
// headers which we must include here
// ----------------------------------------------------------------------------

#include "wx/object.h"          // the base class

#include "wx/cursor.h"          // we have member variables of these classes
#include "wx/font.h"            // so we can't do without them
#include "wx/colour.h"
#include "wx/brush.h"
#include "wx/pen.h"
#include "wx/palette.h"

#include "wx/list.h"            // we use wxList in inline functions

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern int) wxPageNumber;

// ---------------------------------------------------------------------------
// wxDC is the device context - object on which any drawing is done
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxDCBase : public wxObject
{
public:
    wxDCBase()
    {
        m_clipping = FALSE;
        m_ok = TRUE;

        m_minX = m_minY = m_maxX = m_maxY = 0;

        m_signX = m_signY = 1;

        m_logicalOriginX = m_logicalOriginY =
        m_deviceOriginX = m_deviceOriginY = 0;

        m_logicalScaleX = m_logicalScaleY =
        m_userScaleX = m_userScaleY =
        m_scaleX = m_scaleY = 1.0;

        m_logicalFunction = -1;

        m_backgroundMode = wxTRANSPARENT;

        m_mappingMode = wxMM_TEXT;

        m_backgroundBrush = *wxTRANSPARENT_BRUSH;

        m_textForegroundColour = *wxBLACK;
        m_textBackgroundColour = *wxWHITE;

        m_colour = wxColourDisplay();
    }

    ~wxDCBase() { }

    virtual void BeginDrawing() { }
    virtual void EndDrawing() { }

    // graphic primitives
    // ------------------

    void FloodFill(wxCoord x, wxCoord y, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        { DoFloodFill(x, y, col, style); }
    void FloodFill(const wxPoint& pt, const wxColour& col,
                   int style = wxFLOOD_SURFACE)
        { DoFloodFill(pt.x, pt.y, col, style); }

    bool GetPixel(wxCoord x, wxCoord y, wxColour *col) const
        { return DoGetPixel(x, y, col); }
    bool GetPixel(const wxPoint& pt, wxColour *col) const
        { return DoGetPixel(pt.x, pt.y, col); }

    void DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
        { DoDrawLine(x1, y1, x2, y2); }
    void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
        { DoDrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }

    void CrossHair(wxCoord x, wxCoord y)
        { DoCrossHair(x, y); }
    void CrossHair(const wxPoint& pt)
        { DoCrossHair(pt.x, pt.y); }

    void DrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                 wxCoord xc, wxCoord yc)
        { DoDrawArc(x1, y1, x2, y2, xc, yc); }
    void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
        { DoDrawArc(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y); }

    void DrawCheckMark(wxCoord x, wxCoord y,
                       wxCoord width, wxCoord height)
        { DoDrawCheckMark(x, y, width, height); }
    void DrawCheckMark(const wxRect& rect)
        { DoDrawCheckMark(rect.x, rect.y, rect.width, rect.height); }

    void DrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                         double sa, double ea)
        { DoDrawEllipticArc(x, y, w, h, sa, ea); }
    void DrawEllipticArc(const wxPoint& pt, const wxSize& sz,
                         double sa, double ea)
        { DoDrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea); }

    void DrawPoint(wxCoord x, wxCoord y)
        { DoDrawPoint(x, y); }
    void DrawPoint(const wxPoint& pt)
        { DoDrawPoint(pt.x, pt.y); }

    void DrawLines(int n, wxPoint points[],
                   wxCoord xoffset = 0, wxCoord yoffset = 0)
        { DoDrawLines(n, points, xoffset, yoffset); }
    void DrawLines(const wxList *list,
                   wxCoord xoffset = 0, wxCoord yoffset = 0);

    void DrawPolygon(int n, wxPoint points[],
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE)
        { DoDrawPolygon(n, points, xoffset, yoffset, fillStyle); }

    void DrawPolygon(const wxList *list,
                     wxCoord xoffset = 0, wxCoord yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE);

    void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { DoDrawRectangle(x, y, width, height); }
    void DrawRectangle(const wxPoint& pt, const wxSize& sz)
        { DoDrawRectangle(pt.x, pt.y, sz.x, sz.y); }
    void DrawRectangle(const wxRect& rect)
        { DoDrawRectangle(rect.x, rect.y, rect.width, rect.height); }

    void DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height,
                              double radius)
        { DoDrawRoundedRectangle(x, y, width, height, radius); }
    void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz,
                             double radius)
        { DoDrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius); }
    void DrawRoundedRectangle(const wxRect& r, double radius)
        { DoDrawRoundedRectangle(r.x, r.y, r.width, r.height, radius); }

    void DrawCircle(wxCoord x, wxCoord y, wxCoord radius)
        { DoDrawEllipse(x - radius, y - radius, 2*radius, 2*radius); }
    void DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { DoDrawEllipse(x, y, width, height); }
    void DrawEllipse(const wxPoint& pt, const wxSize& sz)
        { DoDrawEllipse(pt.x, pt.y, sz.x, sz.y); }
    void DrawEllipse(const wxRect& rect)
        { DoDrawEllipse(rect.x, rect.y, rect.width, rect.height); }

    void DrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
        { DoDrawIcon(icon, x, y); }
    void DrawIcon(const wxIcon& icon, const wxPoint& pt)
        { DoDrawIcon(icon, pt.x, pt.y); }

    void DrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                    bool useMask = FALSE)
        { DoDrawBitmap(bmp, x, y, useMask); }
    void DrawBitmap(const wxBitmap &bmp, const wxPoint& pt,
                    bool useMask = FALSE)
        { DoDrawBitmap(bmp, pt.x, pt.y, useMask); }

    void DrawText(const wxString& text, wxCoord x, wxCoord y)
        { DoDrawText(text, x, y); }
    void DrawText(const wxString& text, const wxPoint& pt)
        { DoDrawText(text, pt.x, pt.y); }

    void DrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
        { DoDrawRotatedText(text, x, y, angle); }
    void DrawRotatedText(const wxString& text, const wxPoint& pt, double angle)
        { DoDrawRotatedText(text, pt.x, pt.y, angle); }

    bool Blit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
              wxDC *source, wxCoord xsrc, wxCoord ysrc,
              int rop = wxCOPY, bool useMask = FALSE)
    {
        return DoBlit(xdest, ydest, width, height,
                      source, xsrc, ysrc, rop, useMask);
    }
    bool Blit(const wxPoint& destPt, const wxSize& sz,
              wxDC *source, const wxPoint& srcPt,
              int rop = wxCOPY, bool useMask = FALSE)
    {
        return DoBlit(destPt.x, destPt.y, sz.x, sz.y,
                      source, srcPt.x, srcPt.y, rop, useMask);
    }

#if wxUSE_SPLINES
    // TODO: this API needs fixing (wxPointList, why (!const) "wxList *"?)
    void DrawSpline(wxCoord x1, wxCoord y1,
                    wxCoord x2, wxCoord y2,
                    wxCoord x3, wxCoord y3);
    void DrawSpline(int n, wxPoint points[]);

    void DrawSpline(wxList *points) { DoDrawSpline(points); }
#endif // wxUSE_SPLINES

    // global DC operations
    // --------------------

    virtual void Clear() = 0;

    virtual bool StartDoc(const wxString& WXUNUSED(message)) { return TRUE; }
    virtual void EndDoc() { }

    virtual void StartPage() { }
    virtual void EndPage() { }

    // set objects to use for drawing
    // ------------------------------

    virtual void SetFont(const wxFont& font) = 0;
    virtual void SetPen(const wxPen& pen) = 0;
    virtual void SetBrush(const wxBrush& brush) = 0;
    virtual void SetBackground(const wxBrush& brush) = 0;
    virtual void SetBackgroundMode(int mode) = 0;
    virtual void SetPalette(const wxPalette& palette) = 0;

    // clipping region
    // ---------------

    void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
        { DoSetClippingRegion(x, y, width, height); }
    void SetClippingRegion(const wxPoint& pt, const wxSize& sz)
        { DoSetClippingRegion(pt.x, pt.y, sz.x, sz.y); }
    void SetClippingRegion(const wxRect& rect)
        { DoSetClippingRegion(rect.x, rect.y, rect.width, rect.height); }
    void SetClippingRegion(const wxRegion& region)
        { DoSetClippingRegionAsRegion(region); }

    virtual void DestroyClippingRegion() = 0;

    void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *w, wxCoord *h) const
        { DoGetClippingBox(x, y, w, h); }
    void GetClippingBox(wxRect& rect) const
        {
          // Necessary to use intermediate variables for 16-bit compilation
          wxCoord x, y, w, h;
          DoGetClippingBox(&x, &y, &w, &h);
          rect.x = x; rect.y = y; rect.width = w; rect.height = h;
        }

    // text extent
    // -----------

    virtual wxCoord GetCharHeight() const = 0;
    virtual wxCoord GetCharWidth() const = 0;

    void GetTextExtent(const wxString& string,
                       wxCoord *x, wxCoord *y,
                       wxCoord *descent = NULL,
                       wxCoord *externalLeading = NULL,
                       wxFont *theFont = NULL) const
        { DoGetTextExtent(string, x, y, descent, externalLeading, theFont); }

    // size and resolution
    // -------------------

    // in device units
    void GetSize(int *width, int *height) const
        { DoGetSize(width, height); }
    wxSize GetSize() const
    {
        int w, h;
        DoGetSize(&w, &h);

        return wxSize(w, h);
    }

    // in mm
    void GetSizeMM(int* width, int* height) const
        { DoGetSizeMM(width, height); }
    wxSize GetSizeMM() const
    {
        int w, h;
        DoGetSizeMM(&w, &h);

        return wxSize(w, h);
    }

    // coordinates conversions
    // -----------------------

    // This group of functions does actual conversion of the input, as you'd
    // expect.
    wxCoord DeviceToLogicalX(wxCoord x) const;
    wxCoord DeviceToLogicalY(wxCoord y) const;
    wxCoord DeviceToLogicalXRel(wxCoord x) const;
    wxCoord DeviceToLogicalYRel(wxCoord y) const;
    wxCoord LogicalToDeviceX(wxCoord x) const;
    wxCoord LogicalToDeviceY(wxCoord y) const;
    wxCoord LogicalToDeviceXRel(wxCoord x) const;
    wxCoord LogicalToDeviceYRel(wxCoord y) const;

    // query DC capabilities
    // ---------------------

    virtual bool CanDrawBitmap() const = 0;
    virtual bool CanGetTextExtent() const = 0;

    // colour depth
    virtual int GetDepth() const = 0;

    // Resolution in Pixels per inch
    virtual wxSize GetPPI() const = 0;

    virtual bool Ok() const { return m_ok; }

    // accessors
    // ---------

        // const...
    const wxBrush&  GetBackground() const { return m_backgroundBrush; }
    const wxBrush&  GetBrush() const { return m_brush; }
    const wxFont&   GetFont() const { return m_font; }
    const wxPen&    GetPen() const { return m_pen; }
    const wxColour& GetTextBackground() const { return m_textBackgroundColour; }
    const wxColour& GetTextForeground() const { return m_textForegroundColour; }

        // ... and non const
    wxBrush&  GetBackground() { return m_backgroundBrush; }
    wxBrush&  GetBrush() { return m_brush; }
    wxFont&   GetFont() { return m_font; }
    wxPen&    GetPen() { return m_pen; }
    wxColour& GetTextBackground() { return m_textBackgroundColour; }
    wxColour& GetTextForeground() { return m_textForegroundColour; }

    virtual void SetTextForeground(const wxColour& colour)
        { m_textForegroundColour = colour; }
    virtual void SetTextBackground(const wxColour& colour)
        { m_textBackgroundColour = colour; }

    int GetMapMode() const { return m_mappingMode; }
    virtual void SetMapMode(int mode) = 0;

    virtual void GetUserScale(double *x, double *y) const
    {
        if ( x ) *x = m_userScaleX;
        if ( y ) *y = m_userScaleY;
    }
    virtual void SetUserScale(double x, double y) = 0;

    virtual void GetLogicalScale(double *x, double *y)
    {
        if ( x ) *x = m_logicalScaleX;
        if ( y ) *y = m_logicalScaleY;
    }
    virtual void SetLogicalScale(double x, double y)
    {
        m_logicalScaleX = x;
        m_logicalScaleY = y;
    }

    void GetLogicalOrigin(wxCoord *x, wxCoord *y) const
        { DoGetLogicalOrigin(x, y); }
    wxPoint GetLogicalOrigin() const
        { wxCoord x, y; DoGetLogicalOrigin(&x, &y); return wxPoint(x, y); }
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y) = 0;

    void GetDeviceOrigin(wxCoord *x, wxCoord *y) const
        { DoGetDeviceOrigin(x, y); }
    wxPoint GetDeviceOrigin() const
        { wxCoord x, y; DoGetDeviceOrigin(&x, &y); return wxPoint(x, y); }
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y) = 0;

    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp) = 0;

    int GetLogicalFunction() const { return m_logicalFunction; }
    virtual void SetLogicalFunction(int function) = 0;

    // Sometimes we need to override optimization, e.g. if other software is
    // drawing onto our surface and we can't be sure of who's done what.
    //
    // FIXME: is this (still) used?
    virtual void SetOptimization(bool WXUNUSED(opt)) { }
    virtual bool GetOptimization() { return FALSE; }

    // bounding box
    // ------------

    virtual void CalcBoundingBox(wxCoord x, wxCoord y)
    {
        if ( x < m_minX ) m_minX = x;
        if ( y < m_minY ) m_minY = y;
        if ( x > m_maxX ) m_maxX = x;
        if ( y > m_maxY ) m_maxY = y;
    }

    // Get the final bounding box of the PostScript or Metafile picture.
    wxCoord MinX() const { return m_minX; }
    wxCoord MaxX() const { return m_maxX; }
    wxCoord MinY() const { return m_minY; }
    wxCoord MaxY() const { return m_maxY; }

    // misc old functions
    // ------------------

    // for compatibility with the old code when wxCoord was long everywhere
#ifndef __WIN16__
    void GetTextExtent(const wxString& string,
                       long *x, long *y,
                       long *descent = NULL,
                       long *externalLeading = NULL,
                       wxFont *theFont = NULL) const
    {
        wxCoord x2, y2, descent2, externalLeading2;
        DoGetTextExtent(string, &x2, &y2,
                        &descent2, &externalLeading2,
                        theFont);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
        if ( descent )
            *descent = descent2;
        if ( externalLeading )
            *externalLeading = externalLeading2;
    }

    void GetLogicalOrigin(long *x, long *y) const
    {
        wxCoord x2, y2;
        DoGetLogicalOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    }

    void GetDeviceOrigin(long *x, long *y) const
    {
        wxCoord x2, y2;
        DoGetDeviceOrigin(&x2, &y2);
        if ( x )
            *x = x2;
        if ( y )
            *y = y2;
    }
    void GetClippingBox(long *x, long *y, long *w, long *h) const
    {
        wxCoord xx,yy,ww,hh;
        DoGetClippingBox(&xx, &yy, &ww, &hh);
        if (x) *x = xx;
        if (y) *y = yy;
        if (w) *w = ww;
        if (h) *h = hh;
    }
#endif // !Win16

#if WXWIN_COMPATIBILITY
    virtual void SetColourMap(const wxPalette& palette) { SetPalette(palette); }
    void GetTextExtent(const wxString& string, float *x, float *y,
            float *descent = NULL, float *externalLeading = NULL,
            wxFont *theFont = NULL, bool use16bit = FALSE) const ;
    void GetSize(float* width, float* height) const { int w, h; GetSize(& w, & h); *width = w; *height = h; }
    void GetSizeMM(float *width, float *height) const { long w, h; GetSizeMM(& w, & h); *width = (float) w; *height = (float) h; }
#endif // WXWIN_COMPATIBILITY

protected:
    // the pure virtual functions which should be implemented by wxDC
    virtual void DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE) = 0;

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const = 0;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) = 0;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) = 0;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) = 0;
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height);
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) = 0;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) = 0;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) = 0;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height) = 0;

    virtual void DoCrossHair(wxCoord x, wxCoord y) = 0;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) = 0;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE) = 0;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) = 0;
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle) = 0;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE) = 0;

    virtual void DoGetSize(int *width, int *height) const = 0;
    virtual void DoGetSizeMM(int* width, int* height) const = 0;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) = 0;
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE) = 0;

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region) = 0;
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) = 0;

    // FIXME are these functions really different?
    virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y,
                                     wxCoord *w, wxCoord *h)
        { DoGetClippingBox(x, y, w, h); }
    virtual void DoGetClippingBox(wxCoord *x, wxCoord *y,
                                  wxCoord *w, wxCoord *h) const
    {
        if ( m_clipping )
        {
            if ( x ) *x = m_clipX1;
            if ( y ) *y = m_clipY1;
            if ( w ) *w = m_clipX2 - m_clipX1;
            if ( h ) *h = m_clipY2 - m_clipY1;
        }
        else
        {
            *x = *y = *w = *h = 0;
        }
    }

    virtual void DoGetLogicalOrigin(wxCoord *x, wxCoord *y) const
    {
        if ( x ) *x = m_logicalOriginX;
        if ( y ) *y = m_logicalOriginY;
    }

    virtual void DoGetDeviceOrigin(wxCoord *x, wxCoord *y) const
    {
        if ( x ) *x = m_deviceOriginX;
        if ( y ) *y = m_deviceOriginY;
    }

    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const = 0;

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points) = 0;
#endif

protected:
    // flags
    bool m_colour:1;
    bool m_ok:1;
    bool m_clipping:1;
    bool m_isInteractive:1;

    // coordinate system variables

    // TODO short descriptions of what exactly they are would be nice...

    wxCoord m_logicalOriginX, m_logicalOriginY;
    wxCoord m_deviceOriginX, m_deviceOriginY;

    double m_logicalScaleX, m_logicalScaleY;
    double m_userScaleX, m_userScaleY;
    double m_scaleX, m_scaleY;

    // Used by SetAxisOrientation() to invert the axes
    int m_signX, m_signY;

    // bounding and clipping boxes
    wxCoord m_minX, m_minY, m_maxX, m_maxY;
    wxCoord m_clipX1, m_clipY1, m_clipX2, m_clipY2;

    int m_logicalFunction;
    int m_backgroundMode;
    int m_mappingMode;

    // GDI objects
    wxPen             m_pen;
    wxBrush           m_brush;
    wxBrush           m_backgroundBrush;
    wxColour          m_textForegroundColour;
    wxColour          m_textBackgroundColour;
    wxFont            m_font;
    wxPalette         m_palette;

private:
    DECLARE_NO_COPY_CLASS(wxDCBase);
    DECLARE_ABSTRACT_CLASS(wxDCBase)
};

// ----------------------------------------------------------------------------
// now include the declaration of wxDC class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/dc.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/dc.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/dc.h"
#elif defined(__WXQT__)
    #include "wx/qt/dc.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dc.h"
#elif defined(__WXPM__)
    #include "wx/os2/dc.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dc.h"
#endif

#endif
    // _WX_DC_H_BASE_
