/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCCLIENTH__
#define __GTKDCCLIENTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/dc.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxWindowDC;
class wxPaintDC;
class wxClientDC;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class wxWindowDC : public wxDC
{
public:
    wxWindowDC();
    wxWindowDC( wxWindow *win );

    ~wxWindowDC();

    virtual bool CanDrawBitmap() const { return TRUE; }
    virtual bool CanGetTextExtent() const { return TRUE; }

//protected:
    virtual void DoFloodFill( wxCoord x, wxCoord y, const wxColour& col, int style=wxFLOOD_SURFACE );
    virtual bool DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const;

    virtual void DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 );
    virtual void DoCrossHair( wxCoord x, wxCoord y );
    virtual void DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc );
    virtual void DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height,
                                    double sa, double ea );
    virtual void DoDrawPoint( wxCoord x, wxCoord y );

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

    virtual void DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height );
    virtual void DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius = 20.0 );
    virtual void DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height );

    virtual void DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y );
    virtual void DoDrawBitmap( const wxBitmap &bitmap, wxCoord x, wxCoord y,
                               bool useMask = FALSE );

    virtual bool DoBlit( wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                         wxDC *source, wxCoord xsrc, wxCoord ysrc,
                         int logical_func = wxCOPY, bool useMask = FALSE );

    virtual void DoDrawText( const wxString &text, wxCoord x, wxCoord y );
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);
    virtual void DoGetTextExtent( const wxString &string,
                                wxCoord *width, wxCoord *height,
                                wxCoord *descent = (wxCoord *) NULL,
                                wxCoord *externalLeading = (wxCoord *) NULL,
                                wxFont *theFont = (wxFont *) NULL) const;
    virtual wxCoord GetCharWidth() const;
    virtual wxCoord GetCharHeight() const;

    virtual void Clear();

    virtual void SetFont( const wxFont &font );
    virtual void SetPen( const wxPen &pen );
    virtual void SetBrush( const wxBrush &brush );
    virtual void SetBackground( const wxBrush &brush );
    virtual void SetLogicalFunction( int function );
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );
    virtual void SetBackgroundMode( int mode );
    virtual void SetPalette( const wxPalette& palette );

    virtual void DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height );
    virtual void DestroyClippingRegion();
    virtual void DoSetClippingRegionAsRegion( const wxRegion &region  );

#if wxUSE_SPLINES
    virtual void DoDrawSpline( wxList *points );
#endif

    // Resolution in pixels per logical inch
    virtual wxSize GetPPI() const;
    virtual int GetDepth() const;

    // implementation
    // --------------

    GdkWindow    *m_window;
    GdkGC        *m_penGC;
    GdkGC        *m_brushGC;
    GdkGC        *m_textGC;
    GdkGC        *m_bgGC;
    GdkColormap  *m_cmap;
    bool          m_isMemDC;
    wxWindow     *m_owner;

    void SetUpDC();
    void Destroy();
    void ComputeScaleAndOrigin();

    GdkWindow *GetWindow() { return m_window; }

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class wxPaintDC : public wxWindowDC
{
public:
    wxPaintDC();
    wxPaintDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class wxClientDC : public wxWindowDC
{
public:
    wxClientDC();
    wxClientDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

#endif // __GTKDCCLIENTH__
