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

#include "wx/dc.h"
#include "wx/region.h"

// -----------------------------------------------------------------------------
// fwd declarations
// -----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindow;

class WXDLLIMPEXP_CORE wxWindowDC;
class WXDLLIMPEXP_CORE wxPaintDC;
class WXDLLIMPEXP_CORE wxClientDC;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDC : public wxDC
{
public:
    wxWindowDC();
    wxWindowDC( wxWindow *win );

    virtual ~wxWindowDC();

    virtual bool CanDrawBitmap() const { return true; }
    virtual bool CanGetTextExtent() const { return true; }

protected:
    virtual void DoGetSize(int *width, int *height) const;
    virtual bool DoFloodFill( wxCoord x, wxCoord y, const wxColour& col, int style = wxFLOOD_SURFACE );
    virtual bool DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y );
    virtual void DoDrawBitmap( const wxBitmap &bitmap, wxCoord x, wxCoord y,
                               bool useMask = false );

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
        wxCoord x2, wxCoord y2,
        wxCoord xc, wxCoord yc);
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
        double sa, double ea);

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height,
        double radius);
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    virtual void DoCrossHair(wxCoord x, wxCoord y);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString &text, wxCoord x, wxCoord y, double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
        wxDC *source, wxCoord xsrc, wxCoord ysrc,
        int rop = wxCOPY, bool useMask = false, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
        wxCoord width, wxCoord height);

    virtual void DoDrawLines(int n, wxPoint points[],
        wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
        wxCoord xoffset, wxCoord yoffset,
        int fillStyle = wxODDEVEN_RULE);


public:
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

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;

    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void DestroyClippingRegion();
    WXWindow GetWindow() const { return m_window; }

    virtual void ComputeScaleAndOrigin();

protected:
    // implementation
    // --------------
    virtual void DoGetTextExtent(const wxString& string,
        wxCoord *x, wxCoord *y,
        wxCoord *descent = NULL,
        wxCoord *externalLeading = NULL,
        wxFont *theFont = NULL) const;

    WXDisplay    *m_display;
    WXWindow      m_window;
    WXGC          m_penGC;
    WXGC          m_brushGC;
    WXGC          m_textGC;
    WXGC          m_bgGC;
    WXColormap    m_cmap;
    bool          m_isMemDC;
    bool          m_isScreenDC;
    wxWindow     *m_owner;
    wxRegion      m_currentClippingRegion;
    wxRegion      m_paintClippingRegion;

#if wxUSE_UNICODE
    PangoContext *m_context;
    PangoFontDescription *m_fontdesc;
#endif

    void SetUpDC();
    void Destroy();

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDC : public wxWindowDC
{
public:
    wxClientDC() { }
    wxClientDC( wxWindow *win );

protected:
    virtual void DoGetSize(int *width, int *height) const;

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDC : public wxClientDC
{
public:
    wxPaintDC() { }
    wxPaintDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

#endif
// _WX_DCCLIENT_H_
