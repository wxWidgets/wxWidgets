/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

#ifdef __GNUG__
#pragma interface "dcclient.h"
#endif

#include "wx/dc.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC;
class WXDLLEXPORT wxWindow;

// Under Windows, wxClientDC, wxPaintDC and wxWindowDC are implemented differently.
// On many platforms, however, they will be the same.

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxWindowDC)

  public:

    wxWindowDC(void);
    wxWindowDC( wxWindow *win );
    
    ~wxWindowDC(void);
    
    virtual void FloodFill( long x1, long y1, const wxColour& col, int style=wxFLOOD_SURFACE );
    inline void FloodFill(const wxPoint& pt, const wxColour& col, int style=wxFLOOD_SURFACE)
    {
        FloodFill(pt.x, pt.y, col, style);
    }

    virtual bool GetPixel( long x1, long y1, wxColour *col ) const;
    inline bool GetPixel(const wxPoint& pt, wxColour *col) const
    {
        return GetPixel(pt.x, pt.y, col);
    }

    virtual void DrawLine( long x1, long y1, long x2, long y2 );
    inline void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
    {
        DrawLine(pt1.x, pt1.y, pt2.x, pt2.y);
    }

    virtual void CrossHair( long x, long y );
    inline void CrossHair(const wxPoint& pt)
    {
        CrossHair(pt.x, pt.y);
    }

    virtual void DrawArc( long x1, long y1, long x2, long y2, long xc, long yc );
    inline void DrawArc(const wxPoint& pt1, const wxPoint& pt2, const wxPoint& centre)
    {
        DrawArc(pt1.x, pt1.y, pt2.x, pt2.y, centre.x, centre.y);
    }

    virtual void DrawEllipticArc( long x, long y, long width, long height, double sa, double ea );
    virtual void DrawEllipticArc (const wxPoint& pt, const wxSize& sz, double sa, double ea)
    {
        DrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea);
    }

    virtual void DrawPoint( long x, long y );
    inline void DrawPoint( wxPoint& point )
      { DrawPoint(point.x, point.y); }
    
    virtual void DrawLines( int n, wxPoint points[], long xoffset = 0, long yoffset = 0 );
    virtual void DrawLines( wxList *points, long xoffset = 0, long yoffset = 0 );
    virtual void DrawPolygon( int n, wxPoint points[], long xoffset = 0, long yoffset = 0, 
                              int fillStyle=wxODDEVEN_RULE );
    virtual void DrawPolygon( wxList *lines, long xoffset = 0, long yoffset = 0, 
                              int fillStyle=wxODDEVEN_RULE );
    
    virtual void DrawRectangle( long x, long y, long width, long height );
    inline void DrawRectangle(const wxPoint& pt, const wxSize& sz)
    {
        DrawRectangle(pt.x, pt.y, sz.x, sz.y);
    }
    inline void DrawRectangle(const wxRect& rect)
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }

    virtual void DrawRoundedRectangle( long x, long y, long width, long height, double radius = 20.0 );
    inline void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz, double radius = 20.0)
    {
        DrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius);
    }
    inline void DrawRoundedRectangle(const wxRect& rect, double radius = 20.0)
    {
        DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, radius);
    }

    virtual void DrawEllipse( long x, long y, long width, long height );
    inline void DrawEllipse(const wxPoint& pt, const wxSize& sz)
    {
        DrawEllipse(pt.x, pt.y, sz.x, sz.y);
    }
    inline void DrawEllipse(const wxRect& rect)
    {
        DrawEllipse(rect.x, rect.y, rect.width, rect.height);
    }
    
    virtual bool CanDrawBitmap(void) const;

    virtual void DrawIcon( const wxIcon &icon, long x, long y);
    inline void DrawIcon(const wxIcon& icon, const wxPoint& pt)
    {
        DrawIcon(icon, pt.x, pt.y);
    }

    virtual bool Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int logical_func = wxCOPY, bool useMask=FALSE );
    inline bool Blit(const wxPoint& destPt, const wxSize& sz,
            wxDC *source, const wxPoint& srcPt, int rop = wxCOPY, bool useMask = FALSE)
    {
        return Blit(destPt.x, destPt.y, sz.x, sz.y, source, srcPt.x, srcPt.y, rop, useMask);
    }

    virtual void DrawText( const wxString &text, long x, long y, bool use16 = FALSE );
    inline void DrawText(const wxString& text, const wxPoint& pt, bool use16bit = FALSE)
    {
        DrawText(text, pt.x, pt.y, use16bit);
    }

    virtual bool CanGetTextExtent(void) const;
    virtual void GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16 = FALSE );
    virtual long GetCharWidth(void);
    virtual long GetCharHeight(void);
    
    virtual void Clear(void);
    virtual void Clear(const wxRect& rect);
            
    virtual void SetFont( const wxFont &font );
    virtual void SetPen( const wxPen &pen );
    virtual void SetBrush( const wxBrush &brush );
    virtual void SetBackground( const wxBrush &brush );
    virtual void SetLogicalFunction( int function );
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );
    virtual void SetBackgroundMode( int mode );
    virtual void SetPalette( const wxPalette& palette );
    
    virtual void SetClippingRegion( long x, long y, long width, long height );
    virtual void SetClippingRegion( const wxRegion& region );
    virtual void DestroyClippingRegion(void);
    
    virtual void DrawSpline( wxList *points );
    virtual void DrawSpline( long x1, long y1, long x2, long y2, long x3, long y3 )
    { wxDC::DrawSpline(x1, y1, x2, y2, x3, y3); }
    virtual void DrawSpline( int n, wxPoint points[] )
    { wxDC::DrawSpline(n, points); }

    // Motif-specific
    void SetDCClipping (); // Helper function for setting clipping

    inline WXGC GetGC() const { return m_gc; }
    inline WXGC GetBackingGC() const { return m_gcBacking; }
    inline WXDisplay* GetDisplay() const { return m_display; }
    inline bool GetAutoSetting() const { return m_autoSetting; }
    inline void SetAutoSetting(bool flag) { m_autoSetting = flag; }

protected:
    WXGC         m_gc;
    WXGC         m_gcBacking;
    WXDisplay*   m_display;
    wxWindow*    m_window;
    WXRegion     m_currentRegion; // Current clipping region (incl. paint clip region)
    WXRegion     m_userRegion;    // User-defined clipping region
    WXPixmap     m_pixmap;        // Pixmap for drawing on

    // Not sure if we'll need all of these
    int          m_backgroundPixel;
    wxColour     m_currentColour;
//    int          m_currentBkMode;
    int          m_currentPenWidth ;
    int          m_currentPenJoin ;
    int          m_currentPenCap ;
    int          m_currentPenDashCount ;
    char*        m_currentPenDash ;
    wxBitmap     m_currentStipple ;
    int          m_currentStyle ;
    int          m_currentFill ;
    int          m_autoSetting ; // See comment in dcclient.cpp
    WXFont       m_oldFont;
};

class WXDLLEXPORT wxPaintDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)
public:
  wxPaintDC() {}
  wxPaintDC(wxWindow* win);
  ~wxPaintDC();
};

class WXDLLEXPORT wxClientDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxClientDC)
public:
  wxClientDC() {}
  wxClientDC(wxWindow* win): wxWindowDC(win) {}
};

#endif
    // _WX_DCCLIENT_H_
