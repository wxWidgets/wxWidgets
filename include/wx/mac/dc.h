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
#include "wx/mac/aga.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#ifndef MM_TEXT
#define MM_TEXT			0
#define MM_ISOTROPIC	1
#define MM_ANISOTROPIC	2
#define MM_LOMETRIC		3
#define MM_HIMETRIC		4
#define MM_TWIPS		5
#define MM_POINTS		6
#define MM_METRIC		7
#endif

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

extern int wxPageNumber;

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC: public wxDCBase
{
  DECLARE_DYNAMIC_CLASS(wxDC)

  public:

    wxDC();
    ~wxDC();
    

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear();

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return TRUE; };
    virtual void EndDoc(void) {};
    
    virtual void StartPage(void) {};
    virtual void EndPage(void) {};

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);

    virtual void DestroyClippingRegion();

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);

    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    virtual void SetLogicalFunction(int function);

    virtual void SetTextForeground(const wxColour& colour) ;
    virtual void SetTextBackground(const wxColour& colour) ;

//
//

/*
    void BeginDrawing(void) {};
    void EndDrawing(void) {};
    
    virtual bool Ok(void) const { return m_ok; };

    virtual void FloodFill( long x1, long y1, const wxColour& col, int style=wxFLOOD_SURFACE );
    inline void FloodFill(const wxPoint& pt, const wxColour& col, int style=wxFLOOD_SURFACE)
    {
        FloodFill(pt.x, pt.y, col, style);
    }

    virtual bool GetPixel( long x1, long y1, wxColour *col ) const ;
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
    virtual void DrawPoint( wxPoint& point );
    
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

    virtual void DrawSpline( long x1, long y1, long x2, long y2, long x3, long y3 );
    virtual void DrawSpline( wxList *points );
    virtual void DrawSpline( int n, wxPoint points[] );
    
    virtual bool CanDrawBitmap(void) const ;

    virtual void DrawIcon( const wxIcon &icon, long x, long y, bool useMask=FALSE );
    inline void DrawIcon(const wxIcon& icon, const wxPoint& pt)
    {
        DrawIcon(icon, pt.x, pt.y);
    }

    void DrawBitmap( const wxBitmap &bmp, long x, long y, bool useMask=FALSE ) ;

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

    virtual bool CanGetTextExtent(void) const ;
    virtual void GetTextExtent( const wxString &string, int *width, int *height,
                     int *descent = NULL, int *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16 = FALSE ) const ;
    virtual wxCoord GetCharWidth(void) const;
    virtual wxCoord GetCharHeight(void) const;
    
    virtual void Clear(void);
            
    virtual void SetFont( const wxFont &font );
    virtual wxFont& GetFont(void) const { return (wxFont&) m_font; };
    
    virtual void SetPen( const wxPen &pen );
    virtual wxPen& GetPen(void) const { return (wxPen&) m_pen; };
    
    virtual void SetBrush( const wxBrush &brush );
    virtual wxBrush& GetBrush(void) const { return (wxBrush&) m_brush; };

    virtual void SetBackground( const wxBrush &brush );
    virtual wxBrush& GetBackground(void) const { return (wxBrush&) m_backgroundBrush; };

    virtual void SetLogicalFunction( int function );
    virtual int GetLogicalFunction(void) const { return m_logicalFunction; };
    
    virtual void SetTextForeground( const wxColour &col );
    virtual void SetTextBackground( const wxColour &col );
    virtual wxColour& GetTextBackground(void) const { return (wxColour&)m_textBackgroundColour; };
    virtual wxColour& GetTextForeground(void) const { return (wxColour&)m_textForegroundColour; };
    
    virtual void SetBackgroundMode( int mode );
    virtual int GetBackgroundMode(void) const { return m_backgroundMode; };
    
    virtual void SetPalette( const wxPalette& palette );
    void SetColourMap( const wxPalette& palette ) { SetPalette(palette); };
    
    // the first two must be overridden and called
    virtual void SetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height );
    virtual void SetClippingRegion( const wxRect& rect );
    virtual void DestroyClippingRegion(void);
    virtual void GetClippingBox( wxCoord *x, wxCoord *y, wxCoord *width, wxCoord *height ) const;
    virtual void GetClippingBox( long *x, long *y, long *width, long *height ) const;
    virtual void GetClippingBox(wxRect& rect) const;
    
    virtual inline long MinX(void) const { return m_minX; }
    virtual inline long MaxX(void) const { return m_maxX; }
    virtual inline long MinY(void) const { return m_minY; }
    virtual inline long MaxY(void) const { return m_maxY; }

    virtual void GetSize( int* width, int* height ) const;
    inline wxSize GetSize(void) const { int w, h; GetSize(&w, &h); return wxSize(w, h); }
    virtual void GetSizeMM( long* width, long* height ) const;
    
    
    virtual int GetMapMode(void) const { return m_mappingMode; };
    
    virtual void GetUserScale( double *x, double *y );
    virtual void GetLogicalScale( double *x, double *y );
    
    virtual void GetLogicalOrigin( long *x, long *y );
    virtual void GetDeviceOrigin( long *x, long *y );
    virtual void SetInternalDeviceOrigin( long x, long y );
    virtual void GetInternalDeviceOrigin( long *x, long *y );
    
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

    void CalcBoundingBox( long x, long y );
*/

    void ComputeScaleAndOrigin(void);
  public:
  
    
    wxCoord XDEV2LOG(wxCoord x) const
	{
	  long new_x = x - m_deviceOriginX;
	  if (new_x > 0) 
	    return (wxCoord)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
	  else
	    return (wxCoord)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
	}
    wxCoord XDEV2LOGREL(wxCoord x) const
	{ 
	  if (x > 0) 
	    return (wxCoord)((double)(x) / m_scaleX + 0.5);
	  else
	    return (wxCoord)((double)(x) / m_scaleX - 0.5);
	}
    wxCoord YDEV2LOG(wxCoord y) const
	{
	  long new_y = y - m_deviceOriginY;
	  if (new_y > 0)
	    return (wxCoord)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
	  else
	    return (wxCoord)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
	}
    wxCoord YDEV2LOGREL(wxCoord y) const
	{ 
	  if (y > 0)
	    return (wxCoord)((double)(y) / m_scaleY + 0.5);
	  else
	    return (wxCoord)((double)(y) / m_scaleY - 0.5);
	}
    wxCoord XLOG2DEV(wxCoord x) const
	{ 
	  long new_x = x - m_logicalOriginX;
	  if (new_x > 0)
	    return (wxCoord)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
	  else
	    return (wxCoord)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
	}
    wxCoord XLOG2DEVREL(wxCoord x) const
	{ 
	  if (x > 0)
	    return (wxCoord)((double)(x) * m_scaleX + 0.5);
	  else
	    return (wxCoord)((double)(x) * m_scaleX - 0.5);
	}
    wxCoord YLOG2DEV(wxCoord y) const
	{
	  long new_y = y - m_logicalOriginY;
	  if (new_y > 0)
	    return (wxCoord)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
	  else
	    return (wxCoord)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
	}
    wxCoord YLOG2DEVREL(wxCoord y) const
	{ 
	  if (y > 0)
	    return (wxCoord)((double)(y) * m_scaleY + 0.5);
	  else
	    return (wxCoord)((double)(y) * m_scaleY - 0.5);
	}
  
//

protected:
    virtual void DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

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

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);
    virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y,
                                     wxCoord *width, wxCoord *height)
    {
        GetClippingBox(x, y, width, height);
    }

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

#if wxUSE_SPLINES
    virtual void DoDrawSpline(wxList *points);
#endif // wxUSE_SPLINES


//

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
    
    float        m_scaleFactor;  // wxPSDC wants to have this. Will disappear.
    
    long         m_clipX1,m_clipY1,m_clipX2,m_clipY2;
    long         m_minX,m_maxX,m_minY,m_maxY;

//begin wxmac
	GrafPtr				m_macPort ;
	GWorldPtr			m_macMask ;

	// in order to preserve the const inheritance of the virtual functions, we have to 
	// use mutable variables starting from CWPro 5

	void					MacInstallFont() const ;
	void					MacInstallPen() const ;
	void					MacInstallBrush() const ;
	
	mutable bool	m_macFontInstalled ;
	mutable bool	m_macPenInstalled ;
	mutable bool	m_macBrushInstalled ;
	
	mutable long	m_macPortId ;
	GrafPtr				m_macOrigPort ;
	Rect					m_macClipRect ;
	Point					m_macLocalOrigin ;
	mutable AGAPortHelper	m_macPortHelper ;
	void					MacSetupPort() const ;
	void					MacVerifySetup() const { if ( m_macPortId != m_macCurrentPortId ) MacSetupPort() ; } 

	static long m_macCurrentPortId ;
};

#endif
    // _WX_DC_H_
