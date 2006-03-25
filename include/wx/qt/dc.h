/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#ifdef __GNUG__
#pragma interface "dc.h"
#endif

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define MM_TEXT			0
#define MM_ISOTROPIC	1
#define MM_ANISOTROPIC	2
#define MM_LOMETRIC		3
#define MM_HIMETRIC		4
#define MM_TWIPS		5
#define MM_POINTS		6
#define MM_METRIC		7

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

extern int wxPageNumber;

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC: public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxDC)

  public:

    wxDC(void);
    ~wxDC(void);
    
    void BeginDrawing(void) {};
    void EndDrawing(void) {};
    
    virtual bool Ok(void) const { return m_ok; };

    virtual void FloodFill( long x1, long y1, wxColour *col, int style=wxFLOOD_SURFACE ) = 0;
    inline void FloodFill(const wxPoint& pt, const wxColour& col, int style=wxFLOOD_SURFACE)
    {
        FloodFill(pt.x, pt.y, col, style);
    }
    virtual bool GetPixel( long x1, long y1, wxColour *col ) const = 0;
    inline bool GetPixel(const wxPoint& pt, wxColour *col) const
    {
        return GetPixel(pt.x, pt.y, col);
    }

    virtual void DrawLine( long x1, long y1, long x2, long y2 ) = 0;
    inline void DrawLine(const wxPoint& pt1, const wxPoint& pt2)
    {
        DrawLine(pt1.x, pt1.y, pt2.x, pt2.y);
    }

    virtual void CrossHair( long x, long y ) = 0;
    inline void CrossHair(const wxPoint& pt)
    {
        CrossHair(pt.x, pt.y);
    }

    virtual void DrawArc( long x1, long y1, long x2, long y2, double xc, double yc ) = 0;
    inline void DrawArc(const wxPoint& pt1, const wxPoint& pt2, double xc, double yc)
    {
        DrawArc(pt1.x, pt1.y, pt2.x, pt2.y, xc, yc);
    }

    virtual void DrawEllipticArc( long x, long y, long width, long height, double sa, double ea ) = 0;
    virtual void DrawEllipticArc (const wxPoint& pt, const wxSize& sz, double sa, double ea)
    {
        DrawEllipticArc(pt.x, pt.y, sz.x, sz.y, sa, ea);
    }

    virtual void DrawPoint( long x, long y ) = 0;
    virtual void DrawPoint( wxPoint& point );
    
    virtual void DrawLines( int n, wxPoint points[], long xoffset = 0, long yoffset = 0 ) = 0;
    virtual void DrawLines( wxList *points, long xoffset = 0, long yoffset = 0 );
    virtual void DrawPolygon( int n, wxPoint points[], long xoffset = 0, long yoffset = 0, 
                              int fillStyle=wxODDEVEN_RULE ) = 0;
    virtual void DrawPolygon( wxList *lines, long xoffset = 0, long yoffset = 0, 
                              int fillStyle=wxODDEVEN_RULE );
    
    virtual void DrawRectangle( long x, long y, long width, long height ) = 0;
    inline void DrawRectangle(const wxPoint& pt, const wxSize& sz)
    {
        DrawRectangle(pt.x, pt.y, sz.x, sz.y);
    }
    inline void DrawRectangle(const wxRect& rect)
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    virtual void DrawRoundedRectangle( long x, long y, long width, long height, double radius = 20.0 ) = 0;
    inline void DrawRoundedRectangle(const wxPoint& pt, const wxSize& sz, double radius = 20.0)
    {
        DrawRoundedRectangle(pt.x, pt.y, sz.x, sz.y, radius);
    }
    inline void DrawRoundedRectangle(const wxRect& rect, double radius = 20.0)
    {
        DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, radius);
    }

    virtual void DrawEllipse( long x, long y, long width, long height ) = 0;
    inline void DrawEllipse(const wxPoint& pt, const wxSize& sz)
    {
        DrawEllipse(pt.x, pt.y, sz.x, sz.y);
    }
    inline void DrawEllipse(const wxRect& rect)
    {
        DrawEllipse(rect.x, rect.y, rect.width, rect.height);
    }

    virtual void DrawIcon(const wxIcon& icon, long x, long y) = 0;

    virtual void DrawSpline( long x1, long y1, long x2, long y2, long x3, long y3 );
    virtual void DrawSpline( wxList *points );
    virtual void DrawSpline( int n, wxPoint points[] );
    
    virtual bool CanDrawBitmap(void) const = 0;

    virtual void DrawIcon( const wxIcon &icon, long x, long y, bool useMask=FALSE );
    inline void DrawIcon(const wxIcon& icon, const wxPoint& pt)
    {
        DrawIcon(icon, pt.x, pt.y);
    }

    // TODO DrawBitmap is not always the same as DrawIcon, especially if bitmaps and
    // icons are implemented differently.
    void DrawBitmap( const wxBitmap &bmp, long x, long y, bool useMask=FALSE )
	      { DrawIcon( *((wxIcon*)(&bmp)), x, y, useMask ); }

    virtual bool Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int logical_func = wxCOPY, bool useMask=FALSE ) = 0;
    inline bool Blit(const wxPoint& destPt, const wxSize& sz,
            wxDC *source, const wxPoint& srcPt, int rop = wxCOPY, bool useMask = FALSE)
    {
        return Blit(destPt.x, destPt.y, sz.x, sz.y, source, srcPt.x, srcPt.y, rop, useMask);
    }

    virtual void DrawText( const wxString &text, long x, long y, bool use16 = FALSE ) = 0;
    inline void DrawText(const wxString& text, const wxPoint& pt, bool use16bit = FALSE)
    {
        DrawText(text, pt.x, pt.y, use16bit);
    }

    virtual bool CanGetTextExtent(void) const = 0;
    virtual void GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent = NULL, long *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16 = FALSE ) = 0;
    virtual long GetCharWidth(void) = 0;
    virtual long GetCharHeight(void) = 0;
    
    virtual void Clear(void) = 0;
            
    virtual void SetFont( const wxFont &font ) = 0;
    virtual wxFont *GetFont(void) const { return &m_font; };
    
    virtual void SetPen( const wxPen &pen ) = 0;
    virtual wxPen *GetPen(void) const { return &m_pen; };
    
    virtual void SetBrush( const wxBrush &brush ) = 0;
    virtual wxBrush *GetBrush(void) const { return &m_brush; };

    virtual void SetBackground( const wxBrush &brush ) = 0;
    virtual wxBrush *GetBackground(void) const { return &m_backgroundBrush; };

    virtual void SetLogicalFunction( int function ) = 0;
    virtual int GetLogicalFunction(void) const { return m_logicalFunction; };
    
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );
    virtual wxColour& GetTextBackground(void) const { return (wxColour&)m_textBackgroundColour; };
    virtual wxColour& GetTextForeground(void) const { return (wxColour&)m_textForegroundColour; };
    
    virtual void SetBackgroundMode( int mode ) = 0;
    virtual int GetBackgroundMode(void) const { return m_backgroundMode; };
    
    virtual void SetPalette( const wxPalette& palette ) = 0;
    void SetColourMap( const wxPalette& palette ) { SetPalette(palette); };
    
    // the first two must be overridden and called
    virtual void SetClippingRegion( long x, long y, long width, long height );
    virtual void DestroyClippingRegion(void);
    virtual void GetClippingBox( long *x, long *y, long *width, long *height ) const;
    
    virtual inline long MinX(void) const { return m_minX; }
    virtual inline long MaxX(void) const { return m_maxX; }
    virtual inline long MinY(void) const { return m_minY; }
    virtual inline long MaxY(void) const { return m_maxY; }

    virtual void GetSize( int* width, int* height ) const;
    inline wxSize GetSize(void) const { int w, h; GetSize(&w, &h); return wxSize(w, h); }
    virtual void GetSizeMM( long* width, long* height ) const;
    
    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return TRUE; };
    virtual void EndDoc(void) {};
    virtual void StartPage(void) {};
    virtual void EndPage(void) {};
    
    virtual void SetMapMode( int mode );
    virtual int GetMapMode(void) const { return m_mappingMode; };
    
    virtual void SetUserScale( double x, double y );
    virtual void GetUserScale( double *x, double *y );
    virtual void SetLogicalScale( double x, double y );
    virtual void GetLogicalScale( double *x, double *y );
    
    virtual void SetLogicalOrigin( long x, long y );
    virtual void GetLogicalOrigin( long *x, long *y );
    virtual void SetDeviceOrigin( long x, long y );
    virtual void GetDeviceOrigin( long *x, long *y );
    virtual void SetInternalDeviceOrigin( long x, long y );
    virtual void GetInternalDeviceOrigin( long *x, long *y );

    virtual void SetAxisOrientation( bool xLeftRight, bool yBottomUp );
    
    virtual void SetOptimization( bool WXUNUSED(optimize) ) {};
    virtual bool GetOptimization(void) { return m_optimize; };
    
    virtual long DeviceToLogicalX(long x) const;
    virtual long DeviceToLogicalY(long y) const;
    virtual long DeviceToLogicalXRel(long x) const;
    virtual long DeviceToLogicalYRel(long y) const;
    virtual long LogicalToDeviceX(long x) const;
    virtual long LogicalToDeviceY(long y) const;
    virtual long LogicalToDeviceXRel(long x) const;
    virtual long LogicalToDeviceYRel(long y) const;

  public:
  
    void CalcBoundingBox( long x, long y );
    void ComputeScaleAndOrigin(void);
    
    long XDEV2LOG(long x) const
	{
	  long new_x = x - m_deviceOriginX;
	  if (new_x > 0) 
	    return (long)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
	  else
	    return (long)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
	}
    long XDEV2LOGREL(long x) const
	{ 
	  if (x > 0) 
	    return (long)((double)(x) / m_scaleX + 0.5);
	  else
	    return (long)((double)(x) / m_scaleX - 0.5);
	}
    long YDEV2LOG(long y) const
	{
	  long new_y = y - m_deviceOriginY;
	  if (new_y > 0)
	    return (long)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
	  else
	    return (long)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
	}
    long YDEV2LOGREL(long y) const
	{ 
	  if (y > 0)
	    return (long)((double)(y) / m_scaleY + 0.5);
	  else
	    return (long)((double)(y) / m_scaleY - 0.5);
	}
    long XLOG2DEV(long x) const
	{ 
	  long new_x = x - m_logicalOriginX;
	  if (new_x > 0)
	    return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
	  else
	    return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
	}
    long XLOG2DEVREL(long x) const
	{ 
	  if (x > 0)
	    return (long)((double)(x) * m_scaleX + 0.5);
	  else
	    return (long)((double)(x) * m_scaleX - 0.5);
	}
    long YLOG2DEV(long y) const
	{
	  long new_y = y - m_logicalOriginY;
	  if (new_y > 0)
	    return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
	  else
	    return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
	}
    long YLOG2DEVREL(long y) const
	{ 
	  if (y > 0)
	    return (long)((double)(y) * m_scaleY + 0.5);
	  else
	    return (long)((double)(y) * m_scaleY - 0.5);
	}
  
    virtual void DrawOpenSpline( wxList *points ) = 0;
	
  public:
    
    bool         m_ok;
    bool         m_colour;
    
    // not sure, what these mean
    bool         m_clipping;      // Is clipping on right now ?
    bool         m_isInteractive; // Is GetPixel possible ?
    bool         m_autoSetting;   // wxMSW only ?
    bool         m_dontDelete;    // wxMSW only ?
    bool         m_optimize;      // wxMSW only ?
    wxString     m_filename;      // Not sure where this belongs.
    
    wxPen        m_pen;
    wxBrush      m_brush;
    wxBrush      m_backgroundBrush;
    wxColour     m_textForegroundColour;
    wxColour     m_textBackgroundColour;
    wxFont       m_font;
    
    int          m_logicalFunction;
    int          m_backgroundMode;
    int          m_textAlignment;    // gone in wxWin 2.0 ?
    
    int          m_mappingMode;
    
    // not sure what for, but what is a mm on a screen you don't know the size of?
    double       m_mm_to_pix_x,m_mm_to_pix_y; 
    
    long         m_internalDeviceOriginX,m_internalDeviceOriginY;   // If un-scrolled is non-zero or
								    // d.o. changes with scrolling.
								    // Set using SetInternalDeviceOrigin().
								    
    long         m_externalDeviceOriginX,m_externalDeviceOriginY;   // To be set by external classes
                                                                    // such as wxScrolledWindow
								    // using SetDeviceOrigin()
								    
    long         m_deviceOriginX,m_deviceOriginY;                   // Sum of the two above.
    
    long         m_logicalOriginX,m_logicalOriginY;                 // User defined.

    double       m_scaleX,m_scaleY;
    double       m_logicalScaleX,m_logicalScaleY;
    double       m_userScaleX,m_userScaleY;
    long         m_signX,m_signY;
    
    bool         m_needComputeScaleX,m_needComputeScaleY;         // not yet used
    
    long         m_clipX1,m_clipY1,m_clipX2,m_clipY2;
    long         m_minX,m_maxX,m_minY,m_maxY;
};

#endif
    // _WX_DC_H_
