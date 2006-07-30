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

// Under Windows, wxClientDC, wxPaintDC and wxWindowDC are implemented differently.
// On many platforms, however, they will be the same.

typedef wxPaintDC wxClientDC;
typedef wxPaintDC wxWindowDC;

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPaintDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)

  public:

    wxPaintDC(void);
    wxPaintDC( wxWindow *win );
    
    ~wxPaintDC(void);
    
    virtual void FloodFill( long x1, long y1, wxColour *col, int style=wxFLOOD_SURFACE );
    virtual bool GetPixel( long x1, long y1, wxColour *col ) const;

    virtual void DrawLine( long x1, long y1, long x2, long y2 );
    virtual void CrossHair( long x, long y );
    virtual void DrawArc( long x1, long y1, long x2, long y2, double xc, double yc );
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
                     wxFont *theFont = NULL, bool use16 = FALSE );
    virtual long GetCharWidth(void);
    virtual long GetCharHeight(void);
    
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
    virtual void DestroyClippingRegion(void);
    
    virtual void DrawOpenSpline( wxList *points );
};

#endif
    // _WX_DCCLIENT_H_
