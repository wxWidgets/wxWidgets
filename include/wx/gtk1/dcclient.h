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
    DECLARE_DYNAMIC_CLASS(wxWindowDC)

public:
    wxWindowDC();
    wxWindowDC( wxWindow *win );

    ~wxWindowDC();

    virtual bool CanDrawBitmap() const { return TRUE; }
    virtual bool CanGetTextExtent() const { return TRUE; }

    virtual void DoFloodFill( long x, long y, const wxColour& col, int style=wxFLOOD_SURFACE );
    virtual bool DoGetPixel( long x1, long y1, wxColour *col ) const;

    virtual void DoDrawLine( long x1, long y1, long x2, long y2 );
    virtual void DoCrossHair( long x, long y );
    virtual void DoDrawArc( long x1, long y1, long x2, long y2,
                            long xc, long yc );
    virtual void DoDrawEllipticArc( long x, long y, long width, long height,
                                    double sa, double ea );
    virtual void DoDrawPoint( long x, long y );

    virtual void DoDrawLines(int n, wxPoint points[],
                             long xoffset, long yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               long xoffset, long yoffset,
                               int fillStyle = wxODDEVEN_RULE);

    virtual void DoDrawRectangle( long x, long y, long width, long height );
    virtual void DoDrawRoundedRectangle( long x, long y, long width, long height, double radius = 20.0 );
    virtual void DoDrawEllipse( long x, long y, long width, long height );

    virtual void DoDrawIcon( const wxIcon &icon, long x, long y );
    virtual void DoDrawBitmap( const wxBitmap &bitmap, long x, long y,
                               bool useMask = FALSE );

    virtual bool DoBlit( long xdest, long ydest, long width, long height,
                         wxDC *source, long xsrc, long ysrc,
                         int logical_func = wxCOPY, bool useMask = FALSE );

    virtual void DoDrawText( const wxString &text, long x, long y );
    virtual void GetTextExtent( const wxString &string,
                                long *width, long *height,
                                long *descent = (long *) NULL,
                                long *externalLeading = (long *) NULL,
                                wxFont *theFont = (wxFont *) NULL) const;
    virtual long GetCharWidth() const;
    virtual long GetCharHeight() const;

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

    virtual void DoSetClippingRegion( long x, long y, long width, long height );
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
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class wxPaintDC : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxPaintDC)

public:
    wxPaintDC();
    wxPaintDC( wxWindow *win );
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class wxClientDC : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxClientDC)

public:
    wxClientDC();
    wxClientDC( wxWindow *win );
};


#endif // __GTKDCCLIENTH__
