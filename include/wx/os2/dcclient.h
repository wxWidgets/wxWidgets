/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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

class WXDLLEXPORT wxPaintDC;
class WXDLLEXPORT wxWindow;

// Under Windows, wxClientDC, wxPaintDC and wxWindowDC are implemented differently.
// On many platforms, however, they will be the same.

class WXDLLEXPORT wxWindowDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxWindowDC)

  public:

    wxWindowDC(void);
    wxWindowDC( wxWindow *win );

    ~wxWindowDC(void);

    virtual void FloodFill( long x1, long y1, const wxColour& col, int style=wxFLOOD_SURFACE );
    virtual bool GetPixel( long x1, long y1, wxColour *col ) const;

    virtual void DrawLine( long x1, long y1, long x2, long y2 );
    virtual void CrossHair( long x, long y );
    virtual void DrawArc( long x1, long y1, long x2, long y2, long xc, long yc );
    virtual void DrawEllipticArc( long x, long y, long width, long height, double sa, double ea );
    virtual void DrawPoint( long x, long y );

    virtual void DrawLines( int n, wxPoint points[], long xoffset = 0, long yoffset = 0 );
    virtual void DrawLines( wxList *points, long xoffset = 0, long yoffset = 0 );
    virtual void DrawPolygon( int n, wxPoint points[], long xoffset = 0, long yoffset = 0,
                              int fillStyle=wxODDEVEN_RULE );
    virtual void DrawPolygon( wxList *lines, long xoffset = 0, long yoffset = 0,
                              int fillStyle=wxODDEVEN_RULE );

    virtual void DrawRectangle( long x, long y, long width, long height );
    virtual void DrawRoundedRectangle( long x, long y, long width, long height, double radius = 20.0 );
    virtual void DrawEllipse( long x, long y, long width, long height );

    virtual bool CanDrawBitmap(void) const;
    virtual void DrawIcon( const wxIcon &icon, long x, long y, bool useMask=FALSE );
    virtual bool Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int logical_func = wxCOPY, bool useMask=FALSE );

    virtual void DrawText( const wxString &text, long x, long y, bool use16 = FALSE );
    virtual bool CanGetTextExtent(void) const;
    virtual void GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16 = FALSE ) const;
    virtual long GetCharWidth(void) const;
    virtual long GetCharHeight(void) const;

    virtual void Clear(void);

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
    virtual void SetClippingRegion( const wxRegion& region ) ;
    virtual void DestroyClippingRegion(void);

    virtual void DrawSpline( wxList *points );
private:
    // to supress virtual function hiding, do not use
    void DrawEllipticArc(const wxPoint& pt, const wxSize& sz,
                         double sa, double ea)
    { wxDC::DrawEllipticArc(pt, sz, sa, ea); };
    void DrawPoint(wxPoint& pt)
    { wxDC:DrawPoint(pt); };
    void DrawSpline(int n, wxPoint points[])
    { wxDC::DrawSpline(n, points); };
    void DrawSpline(long x1, long y1, long x2, long y2, long x3, long y3)
    { wxDC::DrawSpline(x1, y1, x2, y2, x3, y3); };
    virtual void GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL) const
    { GetTextExtent(string, width, height, descent, externalLeading, theFont, FALSE); };
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPaintDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)

  public:

    wxPaintDC(void):wxWindowDC() {};
    wxPaintDC( wxWindow *win ): wxWindowDC(win) {};

};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxClientDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxClientDC)

  public:

    wxClientDC(void):wxWindowDC() {};
    wxClientDC( wxWindow *win ): wxWindowDC(win) {};

};

#endif
    // _WX_DCCLIENT_H_
