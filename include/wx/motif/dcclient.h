/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

#ifdef __GNUG__
    #pragma interface "dcclient.h"
#endif

#include "wx/dc.h"

// -----------------------------------------------------------------------------
// fwd declarations
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC;
class WXDLLEXPORT wxWindow;

// Under Windows, wxClientDC, wxPaintDC and wxWindowDC are implemented
// differently. On many platforms, however, they will be the same.

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC : public wxDC
{
    DECLARE_DYNAMIC_CLASS(wxWindowDC)

public:
    wxWindowDC();
    wxWindowDC( wxWindow *win );

    ~wxWindowDC();

    // TODO this function is Motif-only for now - should it go into base class?
    void Clear(const wxRect& rect);

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear();

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);
    virtual void SetLogicalFunction( int function );

    virtual void SetTextForeground(const wxColour& colour);
    virtual void SetTextBackground(const wxColour& colour);

    virtual long GetCharHeight() const;
    virtual long GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               long *x, long *y,
                               long *descent = NULL,
                               long *externalLeading = NULL,
                               wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;

    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void DestroyClippingRegion();

    // Helper function for setting clipping
    void SetDCClipping();

    // implementation from now on
    // --------------------------

    WXGC GetGC() const { return m_gc; }
    WXGC GetBackingGC() const { return m_gcBacking; }
    WXDisplay* GetDisplay() const { return m_display; }
    bool GetAutoSetting() const { return m_autoSetting; }
    void SetAutoSetting(bool flag) { m_autoSetting = flag; }

protected:
    virtual void DoFloodFill(long x, long y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(long x, long y, wxColour *col) const;

    virtual void DoDrawPoint(long x, long y);
    virtual void DoDrawLine(long x1, long y1, long x2, long y2);

    virtual void DoDrawArc(long x1, long y1,
                           long x2, long y2,
                           long xc, long yc);
    virtual void DoDrawEllipticArc(long x, long y, long w, long h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(long x, long y, long width, long height);
    virtual void DoDrawRoundedRectangle(long x, long y,
                                        long width, long height,
                                        double radius);
    virtual void DoDrawEllipse(long x, long y, long width, long height);

    virtual void DoCrossHair(long x, long y);

    virtual void DoDrawText(const wxString& text, long x, long y);

    virtual bool DoBlit(long xdest, long ydest, long width, long height,
            wxDC *source, long xsrc, long ysrc,
            int rop = wxCOPY, bool useMask = FALSE);

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(long x, long y,
            long width, long height);

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
            long xoffset, long yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
            long xoffset, long yoffset,
            int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif // wxUSE_SPLINES

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
    wxPaintDC() { }
    wxPaintDC(wxWindow* win);

    ~wxPaintDC();
};

class WXDLLEXPORT wxClientDC: public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxClientDC)

public:
    wxClientDC() { }
    wxClientDC(wxWindow* win) : wxWindowDC(win) { }
};

#endif
    // _WX_DCCLIENT_H_
