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
// types
// ---------------------------------------------------------------------------

// type which should be used (whenever possible, i.e. as long as it doesn't
// break compatibility) for screen coordinates
typedef int wxCoord;

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern int) wxPageNumber;

// ---------------------------------------------------------------------------
// wxDC is the device context - object on which any drawing is done
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxDCBase : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxDCBase)

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

    void FloodFill(long x, long y, const wxColour& col,
            int style = wxFLOOD_SURFACE)
        { DoFloodFill(x, y, col, style); }
    void FloodFill(const wxPoint& pt, const wxColour& col,
            int style = wxFLOOD_SURFACE)
        { DoFloodFill(pt.x, pt.y, col, style); }

    bool GetPixel(long x, long y, wxColour *col) const
        { return DoGetPixel(x, y, col); }
    bool GetPixel(const wxPoint& pt, wxColour *col) const
        { return DoGetPixel(pt.x, pt.y, col); }

    void DrawLine(long x1, long y1, long x2, long y2)
        { DoDrawLine(x1, y1, x2, y2); }
    void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
        { DoDrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }

    void CrossHair(long x, long y)
        { DoCrossHair(x, y); }
    void CrossHair(const wxPoint& pt)
        { DoCrossHair(pt.x, pt.y); }

    void DrawArc(long x1, long y1, long x2, long y2, long xc, long yc)
        { DoDrawArc(x1, y1, x2, y2, xc, yc); }
    void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
        { DoDrawArc(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y); }

    void DrawEllipticArc(long x, long y, long w, long h, double sa, double ea)
        { DoDrawEllipticArc(x, y, w, h, sa, ea); }
    void DrawEllipticArc(const wxPoint& pt, const wxSize& sz,
                         double sa, double ea)
        { DoDrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea); }

    void DrawPoint(long x, long y)
        { DoDrawPoint(x, y); }
    void DrawPoint(const wxPoint& pt)
        { DoDrawPoint(pt.x, pt.y); }

    void DrawLines(int n, wxPoint points[], long xoffset = 0, long yoffset = 0)
        { DoDrawLines(n, points, xoffset, yoffset); }
    void DrawLines(const wxList *list, long xoffset = 0, long yoffset = 0);

    void DrawPolygon(int n, wxPoint points[],
                     long xoffset = 0, long yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE)
        { DoDrawPolygon(n, points, xoffset, yoffset, fillStyle); }

    void DrawPolygon(const wxList *list,
                     long xoffset = 0, long yoffset = 0,
                     int fillStyle = wxODDEVEN_RULE);

    void DrawRectangle(long x, long y, long width, long height)
        { DoDrawRectangle(x, y, width, height); }
    void DrawRectangle(const wxPoint& pt, const wxSize& sz)
        { DoDrawRectangle(pt.x, pt.y, sz.x, sz.y); }
    void DrawRectangle(const wxRect& rect)
        { DoDrawRectangle(rect.x, rect.y, rect.width, rect.height); }

    void DrawRoundedRectangle(long x, long y, long width, long height,
                              double radius)
        { DoDrawRoundedRectangle(x, y, width, height, radius); }
    void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz,
                             double radius)
        { DoDrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius); }
    void DrawRoundedRectangle(const wxRect& r, double radius)
        { DoDrawRoundedRectangle(r.x, r.y, r.width, r.height, radius); }

    void DrawCircle(long x, long y, long radius)
        { DoDrawEllipse(x - radius, y - radius, 2*radius, 2*radius); }
    void DrawEllipse(long x, long y, long width, long height)
        { DoDrawEllipse(x, y, width, height); }
    void DrawEllipse(const wxPoint& pt, const wxSize& sz)
        { DoDrawEllipse(pt.x, pt.y, sz.x, sz.y); }
    void DrawEllipse(const wxRect& rect)
        { DoDrawEllipse(rect.x, rect.y, rect.width, rect.height); }

    void DrawIcon(const wxIcon& icon, long x, long y)
        { DoDrawIcon(icon, x, y); }
    void DrawIcon(const wxIcon& icon, const wxPoint& pt)
        { DoDrawIcon(icon, pt.x, pt.y); }

    void DrawBitmap(const wxBitmap &bmp, long x, long y, bool useMask = FALSE)
        { DoDrawBitmap(bmp, x, y, useMask); }
    void DrawBitmap(const wxBitmap &bmp, const wxPoint& pt,
                    bool useMask = FALSE)
        { DoDrawBitmap(bmp, pt.x, pt.y, useMask); }

    void DrawText(const wxString& text, long x, long y)
        { DoDrawText(text, x, y); }
    void DrawText(const wxString& text, const wxPoint& pt)
        { DoDrawText(text, pt.x, pt.y); }

    bool Blit(long xdest, long ydest, long width, long height,
              wxDC *source, long xsrc, long ysrc,
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
    void DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3);
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

    void SetClippingRegion(long x, long y, long width, long height)
        { DoSetClippingRegion(x, y, width, height); }
    void SetClippingRegion(const wxPoint& pt, const wxSize& sz)
        { DoSetClippingRegion(pt.x, pt.y, sz.x, sz.y); }
    void SetClippingRegion(const wxRect& rect)
        { DoSetClippingRegion(rect.x, rect.y, rect.width, rect.height); }
    void SetClippingRegion(const wxRegion& region)
        { DoSetClippingRegionAsRegion(region); }

    virtual void DestroyClippingRegion() = 0;

    void GetClippingBox(long *x, long *y, long *w, long *h) const
        { DoGetClippingBox(x, y, w, h); }
    void GetClippingBox(wxRect& rect) const
        { DoGetClippingBox(&rect.x, &rect.y, &rect.width, &rect.height); }

    // text extent
    // -----------

    virtual long GetCharHeight() const = 0;
    virtual long GetCharWidth() const = 0;
    virtual void GetTextExtent(const wxString& string,
                               long *x, long *y,
                               long *descent = NULL,
                               long *externalLeading = NULL,
                               wxFont *theFont = NULL) const = 0;

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
    long DeviceToLogicalX(long x) const;
    long DeviceToLogicalY(long y) const;
    long DeviceToLogicalXRel(long x) const;
    long DeviceToLogicalYRel(long y) const;
    long LogicalToDeviceX(long x) const;
    long LogicalToDeviceY(long y) const;
    long LogicalToDeviceXRel(long x) const;
    long LogicalToDeviceYRel(long y) const;

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

    void GetLogicalOrigin(long *x, long *y) const
        { DoGetLogicalOrigin(x, y); }
    wxPoint GetLogicalOrigin() const
        { long x, y; DoGetLogicalOrigin(&x, &y); return wxPoint(x, y); }
    virtual void SetLogicalOrigin(long x, long y) = 0;

    void GetDeviceOrigin(long *x, long *y) const
        { DoGetDeviceOrigin(x, y); }
    wxPoint GetDeviceOrigin() const
        { long x, y; DoGetDeviceOrigin(&x, &y); return wxPoint(x, y); }
    virtual void SetDeviceOrigin(long x, long y) = 0;

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

    virtual void CalcBoundingBox(long x, long y)
    {
        if (x < m_minX) m_minX = x;
        if (y < m_minY) m_minY = y;
        if (x > m_maxX) m_maxX = x;
        if (y > m_maxY) m_maxY = y;
    }

    // Get the final bounding box of the PostScript or Metafile picture.
    long MinX() const { return m_minX; }
    long MaxX() const { return m_maxX; }
    long MinY() const { return m_minY; }
    long MaxY() const { return m_maxY; }

    // misc old functions
    // ------------------

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
    virtual void DoFloodFill(long x, long y, const wxColour& col,
                             int style = wxFLOOD_SURFACE) = 0;

    virtual bool DoGetPixel(long x, long y, wxColour *col) const = 0;

    virtual void DoDrawPoint(long x, long y) = 0;
    virtual void DoDrawLine(long x1, long y1, long x2, long y2) = 0;

    virtual void DoDrawArc(long x1, long y1,
                           long x2, long y2,
                           long xc, long yc) = 0;
    virtual void DoDrawEllipticArc(long x, long y, long w, long h,
                                   double sa, double ea) = 0;

    virtual void DoDrawRectangle(long x, long y, long width, long height) = 0;
    virtual void DoDrawRoundedRectangle(long x, long y,
                                        long width, long height,
                                        double radius) = 0;
    virtual void DoDrawEllipse(long x, long y, long width, long height) = 0;

    virtual void DoCrossHair(long x, long y) = 0;

    virtual void DoDrawIcon(const wxIcon& icon, long x, long y) = 0;
    virtual void DoDrawBitmap(const wxBitmap &bmp, long x, long y,
                              bool useMask = FALSE) = 0;

    virtual void DoDrawText(const wxString& text, long x, long y) = 0;

    virtual bool DoBlit(long xdest, long ydest, long width, long height,
                        wxDC *source, long xsrc, long ysrc,
                        int rop = wxCOPY, bool useMask = FALSE) = 0;

    virtual void DoGetSize(int *width, int *height) const = 0;
    virtual void DoGetSizeMM(int* width, int* height) const = 0;

    virtual void DoDrawLines(int n, wxPoint points[],
                             long xoffset, long yoffset) = 0;
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               long xoffset, long yoffset,
                               int fillStyle = wxODDEVEN_RULE) = 0;

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region) = 0;
    virtual void DoSetClippingRegion(long x, long y,
                                     long width, long height) = 0;

    // FIXME are these functions really different?
    virtual void DoGetClippingRegion(long *x, long *y,
                                     long *w, long *h)
        { DoGetClippingBox(x, y, w, h); }
    virtual void DoGetClippingBox(long *x, long *y,
                                  long *w, long *h) const
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

    virtual void DoGetLogicalOrigin(long *x, long *y) const
    {
        if ( x ) *x = m_logicalOriginX;
        if ( y ) *y = m_logicalOriginY;
    }

    virtual void DoGetDeviceOrigin(long *x, long *y) const
    {
        if ( x ) *x = m_deviceOriginX;
        if ( y ) *y = m_deviceOriginY;
    }

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

    long m_logicalOriginX, m_logicalOriginY;
    long m_deviceOriginX, m_deviceOriginY;

    double m_logicalScaleX, m_logicalScaleY;
    double m_userScaleX, m_userScaleY;
    double m_scaleX, m_scaleY;

    // Used by SetAxisOrientation() to invert the axes
    int m_signX, m_signY;

    // bounding and clipping boxes
    long m_minX, m_minY, m_maxX, m_maxY;
    long m_clipX1, m_clipY1, m_clipX2, m_clipY2;

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
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dc.h"
#endif

#endif
    // _WX_DC_H_BASE_
