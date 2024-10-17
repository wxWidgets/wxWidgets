/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dcclient.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKDCCLIENT_H_
#define _WX_GTKDCCLIENT_H_

#include "wx/gtk/dc.h"

//-----------------------------------------------------------------------------
// wxWindowDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDCImpl : public wxGTKDCImpl
{
public:
    wxWindowDCImpl( wxDC *owner );
    wxWindowDCImpl( wxDC *owner, wxWindow *win );

    virtual ~wxWindowDCImpl();

    virtual bool CanDrawBitmap() const override { return true; }
    virtual bool CanGetTextExtent() const override { return true; }

    virtual void DoGetSize(int *width, int *height) const override;
    virtual bool DoFloodFill( wxCoord x, wxCoord y, const wxColour& col,
                              wxFloodFillStyle style=wxFLOOD_SURFACE ) override;
    virtual bool DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const override;

    virtual void DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 ) override;
    virtual void DoCrossHair( wxCoord x, wxCoord y ) override;
    virtual void DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc ) override;
    virtual void DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height,
                                    double sa, double ea ) override;
    virtual void DoDrawPoint( wxCoord x, wxCoord y ) override;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset) override;
    virtual void DoDrawPolygon(int n, const wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;

    virtual void DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height ) override;
    virtual void DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20.0 ) override;
    virtual void DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height ) override;

    virtual void DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y ) override;
    virtual void DoDrawBitmap( const wxBitmap &bitmap, wxCoord x, wxCoord y,
                               bool useMask = false ) override;

    virtual bool DoBlit( wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source, wxCoord xsrc, wxCoord ysrc,
                         wxRasterOperationMode logical_func = wxCOPY,
                         bool useMask = false,
                         wxCoord xsrcMask = -1, wxCoord ysrcMask = -1 ) override;

    virtual void DoDrawText( const wxString &text, wxCoord x, wxCoord y ) override;
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle) override;
    virtual void DoGetTextExtent( const wxString &string,
                                wxCoord *width, wxCoord *height,
                                wxCoord *descent = nullptr,
                                wxCoord *externalLeading = nullptr,
                                const wxFont *theFont = nullptr) const override;
    virtual bool DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const override;
    virtual void DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height ) override;
    virtual void DoSetDeviceClippingRegion( const wxRegion &region ) override;
    virtual bool DoGetClippingRect(wxRect& rect) const override;

    virtual wxCoord GetCharWidth() const override;
    virtual wxCoord GetCharHeight() const override;

    virtual void Clear() override;

    virtual void SetFont( const wxFont &font ) override;
    virtual void SetPen( const wxPen &pen ) override;
    virtual void SetBrush( const wxBrush &brush ) override;
    virtual void SetBackground( const wxBrush &brush ) override;
    virtual void SetLogicalFunction( wxRasterOperationMode function ) override;
    virtual void SetTextForeground( const wxColour &col ) override;
    virtual void SetTextBackground( const wxColour &col ) override;
    virtual void SetBackgroundMode( int mode ) override;

#if wxUSE_PALETTE
    virtual void SetPalette( const wxPalette& palette ) override;
#endif

    virtual void DestroyClippingRegion() override;

    // Resolution in pixels per logical inch
    virtual wxSize GetPPI() const override;
    virtual int GetDepth() const override;

    // overridden here for RTL
    virtual void SetDeviceOrigin( wxCoord x, wxCoord y ) override;
    virtual void SetAxisOrientation( bool xLeftRight, bool yBottomUp ) override;

// protected:
    // implementation
    // --------------

    GdkWindow    *m_gdkwindow;
    GdkGC        *m_penGC;
    GdkGC        *m_brushGC;
    GdkGC        *m_textGC;
    GdkGC        *m_bgGC;
    GdkColormap  *m_cmap;
    bool          m_isScreenDC;
    wxRegion      m_currentClippingRegion;
    wxRegion      m_paintClippingRegion;
    bool          m_isClipBoxValid;

    // PangoContext stuff for GTK 2.0
    PangoContext *m_context;
    PangoLayout *m_layout;
    PangoFontDescription *m_fontdesc;

    void SetUpDC( bool ismem = false );
    void DontClipSubWindows();
    void Destroy();

    virtual void ComputeScaleAndOrigin() override;

    virtual GdkWindow *GetGDKWindow() const override { return m_gdkwindow; }

    // Update the internal clip box variables
    void UpdateClipBox();

private:
    void DrawingSetup(GdkGC*& gc, bool& originChanged);
    GdkPixmap* MonoToColor(GdkPixmap* monoPixmap, int x, int y, int w, int h) const;

    wxDECLARE_ABSTRACT_CLASS(wxWindowDCImpl);
};

//-----------------------------------------------------------------------------
// wxClientDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDCImpl : public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *win );

    virtual void DoGetSize(int *width, int *height) const override;

    static bool
    CanBeUsedForDrawing(const wxWindow* WXUNUSED(window)) { return true; }

    wxDECLARE_ABSTRACT_CLASS(wxClientDCImpl);
};

//-----------------------------------------------------------------------------
// wxPaintDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDCImpl : public wxClientDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );

    wxDECLARE_ABSTRACT_CLASS(wxPaintDCImpl);
};

#endif // _WX_GTKDCCLIENT_H_
