/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dc.h"
#endif

#include "wx/dc.h"
#include "wx/app.h"
#include "wx/mac/uma.h"
#include "wx/dcmemory.h"
#include "wx/region.h"
#include "wx/image.h"

#if __MSL__ >= 0x6000
#include "math.h"
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define mm2inches		0.0393700787402
#define inches2mm		25.4
#define mm2twips		56.6929133859
#define twips2mm		0.0176388888889
#define mm2pt			2.83464566929
#define pt2mm			0.352777777778
#ifndef __DARWIN__
const double M_PI = 3.14159265358979 ;
#endif
const double RAD2DEG  = 180.0 / M_PI;

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

static inline double dmin(double a, double b) { return a < b ? a : b; }
static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC()
{
  m_ok = FALSE;
  m_colour = TRUE;
  
  m_mm_to_pix_x = mm2pt;
  m_mm_to_pix_y = mm2pt;
  
  m_internalDeviceOriginX = 0;
  m_internalDeviceOriginY = 0;
  m_externalDeviceOriginX = 0;
  m_externalDeviceOriginY = 0;
  
  m_logicalScaleX = 1.0;
  m_logicalScaleY = 1.0;
  m_userScaleX = 1.0;
  m_userScaleY = 1.0;
  m_scaleX = 1.0;
  m_scaleY = 1.0;
  
  m_needComputeScaleX = FALSE;
  m_needComputeScaleY = FALSE;
  
  m_maxX = m_maxY = -100000;
  m_minY = m_minY =  100000;

  m_macPort = NULL ;
  m_macMask = NULL ;
  m_ok = FALSE ;
  
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
	
	m_macLocalOrigin.h = m_macLocalOrigin.v = 0 ;
	m_macBoundaryClipRgn = NewRgn() ;
	m_macCurrentClipRgn = NewRgn() ;

	SetRectRgn( m_macBoundaryClipRgn , -32000 , -32000 , 32000 , 32000 ) ;
	SetRectRgn( m_macCurrentClipRgn , -32000 , -32000 , 32000 , 32000 ) ;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;
}
wxMacPortSetter::wxMacPortSetter( const wxDC* dc ) :
	m_ph( dc->m_macPort ) 
{
	wxASSERT( dc->Ok() ) ;
	
	dc->MacSetupPort(&m_ph) ;
}

wxMacPortSetter::~wxMacPortSetter() 
{
}

wxDC::~wxDC(void)
{
  DisposeRgn( m_macBoundaryClipRgn ) ;
  DisposeRgn( m_macCurrentClipRgn ) ;
}
void wxDC::MacSetupPort(AGAPortHelper* help) const
{
	SetClip( m_macCurrentClipRgn);

	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;	
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
     wxCHECK_RET( Ok(), wxT("invalid window dc") );
 
     wxCHECK_RET( bmp.Ok(), wxT("invalid bitmap") );
 
     wxMacPortSetter helper(this) ;
 
     wxCoord xx = XLOG2DEVMAC(x);
     wxCoord yy = YLOG2DEVMAC(y);
     wxCoord w = bmp.GetWidth();
     wxCoord h = bmp.GetHeight();
     wxCoord ww = XLOG2DEVREL(w);
     wxCoord hh = YLOG2DEVREL(h);
 
     // Set up drawing mode
     short  mode = (m_logicalFunction == wxCOPY ? srcCopy :
                    //m_logicalFunction == wxCLEAR ? WHITENESS :
                    //m_logicalFunction == wxSET ? BLACKNESS :
                    m_logicalFunction == wxINVERT ? hilite :
                   //m_logicalFunction == wxAND ? MERGECOPY :
                    m_logicalFunction == wxOR ? srcOr :
                    m_logicalFunction == wxSRC_INVERT ? notSrcCopy :
                    m_logicalFunction == wxXOR ? srcXor :
                    m_logicalFunction == wxOR_REVERSE ? notSrcOr :
                    //m_logicalFunction == wxAND_REVERSE ? SRCERASE :
                    //m_logicalFunction == wxSRC_OR ? srcOr :
                    //m_logicalFunction == wxSRC_AND ? SRCAND :
                    srcCopy );
 
     if ( bmp.GetBitmapType() == kMacBitmapTypePict ) {
         Rect bitmaprect = { 0 , 0 , hh, ww };
         ::OffsetRect( &bitmaprect, xx, yy ) ;
         ::DrawPicture( bmp.GetPict(), &bitmaprect ) ;
    }
     else if ( bmp.GetBitmapType() == kMacBitmapTypeGrafWorld )
     {
         GWorldPtr    bmapworld = bmp.GetHBITMAP();
         PixMapHandle bmappixels ;
 
         // Set foreground and background colours (for bitmaps depth = 1)
         if(bmp.GetDepth() == 1)
        {
             RGBColor fore = m_textForegroundColour.GetPixel();
             RGBColor back = m_textBackgroundColour.GetPixel();
             RGBForeColor(&fore);
             RGBBackColor(&back);
         }
         else
         {
             RGBColor white = { 0xFFFF, 0xFFFF,0xFFFF} ;
             RGBColor black = { 0,0,0} ;
             RGBForeColor( &black ) ;
             RGBBackColor( &white ) ;
         }
 
         bmappixels = GetGWorldPixMap( bmapworld ) ;
 
         wxCHECK_RET(LockPixels(bmappixels),
                     wxT("DoDrawBitmap:  Unable to lock pixels"));
 
         Rect source = { 0, 0, h, w };
         Rect dest   = { yy, xx, yy + hh, xx + ww };
 
         if ( useMask && bmp.GetMask() )
         {
             if( LockPixels(GetGWorldPixMap(bmp.GetMask()->GetMaskBitmap())))
             {
                 CopyDeepMask
                     (
                      GetPortBitMapForCopyBits(bmapworld),
                      GetPortBitMapForCopyBits(bmp.GetMask()->GetMaskBitmap()),
                      GetPortBitMapForCopyBits( m_macPort ),
                      &source, &source, &dest, mode, NULL
                      );
                 UnlockPixels(GetGWorldPixMap(bmp.GetMask()->GetMaskBitmap()));
             }
         }
         else {
             CopyBits( GetPortBitMapForCopyBits( bmapworld ),
                       GetPortBitMapForCopyBits( m_macPort ),
                       &source, &dest, mode, NULL ) ;
         }
         UnlockPixels( bmappixels ) ;
     }
     else if ( bmp.GetBitmapType() == kMacBitmapTypeIcon )
     {
        Rect bitmaprect = { 0 , 0 , bmp.GetHeight(), bmp.GetWidth() } ;
        OffsetRect( &bitmaprect, xx, yy ) ;
        PlotCIconHandle( &bitmaprect , atNone , ttNone , bmp.GetHICON() ) ;
     }
     m_macPenInstalled = false ;
     m_macBrushInstalled = false ;
     m_macFontInstalled = false ;

}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
     wxCHECK_RET(Ok(), wxT("Invalid dc  wxDC::DoDrawIcon"));
 
     wxCHECK_RET(icon.Ok(), wxT("Invalid icon wxDC::DoDrawIcon"));
 
    DoDrawBitmap( icon , x , y , icon.GetMask() != NULL ) ;
}
void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoSetClippingRegion  Invalid DC"));
    wxCoord xx, yy, ww, hh;

    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(width);
    hh = YLOG2DEVREL(height);

    SetRectRgn( m_macCurrentClipRgn , xx , yy , xx + ww , yy + hh ) ;
    SectRgn( m_macCurrentClipRgn , m_macBoundaryClipRgn , m_macCurrentClipRgn ) ;
    
    if( m_clipping )
    {
        m_clipX1 = wxMax( m_clipX1 , xx );
        m_clipY1 = wxMax( m_clipY1 , yy );
        m_clipX2 = wxMin( m_clipX2, (xx + ww));
        m_clipY2 = wxMin( m_clipY2, (yy + hh));
    }
    else
    {
        m_clipping = TRUE;
        m_clipX1 = xx;
        m_clipY1 = yy;
        m_clipX2 = xx + ww;
        m_clipY2 = yy + hh;
    }

}
void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") ) ;

  	wxMacPortSetter helper(this) ;
    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    wxCoord x, y, w, h;
    region.GetBox( x, y, w, h );
    wxCoord xx, yy, ww, hh;

    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(w);
    hh = YLOG2DEVREL(h);

    // if we have a scaling that we cannot map onto native regions
    // we must use the box

    if ( ww != w || hh != h )
    {
        wxDC::DoSetClippingRegion( x, y, w, h );
    }
    else
    {
        CopyRgn( region.GetWXHRGN() , m_macCurrentClipRgn ) ;
        if ( xx != x || yy != y )
        {
            OffsetRgn( m_macCurrentClipRgn , xx - x , yy - y ) ;
        }
        SectRgn( m_macCurrentClipRgn , m_macBoundaryClipRgn , m_macCurrentClipRgn ) ;
        if( m_clipping )
        {
            m_clipX1 = wxMax( m_clipX1 , xx );
            m_clipY1 = wxMax( m_clipY1 , yy );
            m_clipX2 = wxMin( m_clipX2, (xx + ww));
            m_clipY2 = wxMin( m_clipY2, (yy + hh));
        }
        else
        {
            m_clipping = TRUE;
            m_clipX1 = xx;
            m_clipY1 = yy;
            m_clipX2 = xx + ww;
            m_clipY2 = yy + hh;
        }
    }

}

void wxDC::DestroyClippingRegion()
{
  wxMacPortSetter helper(this) ;
  CopyRgn( m_macBoundaryClipRgn , m_macCurrentClipRgn ) ;
  m_clipping = FALSE;
}    
void wxDC::DoGetSize( int* width, int* height ) const
{
  *width = m_maxX-m_minX;
  *height = m_maxY-m_minY;
}
void wxDC::DoGetSizeMM( int* width, int* height ) const
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );
  *width = long( double(w) / (m_scaleX*m_mm_to_pix_x) );
  *height = long( double(h) / (m_scaleY*m_mm_to_pix_y) );
}
void wxDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    m_textForegroundColour = col;
    m_macFontInstalled = false ;
}
void wxDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    m_textBackgroundColour = col;
    m_macFontInstalled = false ;
}
void wxDC::SetMapMode( int mode )
{
  switch (mode) 
  {
    case wxMM_TWIPS:
      SetLogicalScale( twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y );
      break;
    case wxMM_POINTS:
      SetLogicalScale( pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y );
      break;
    case wxMM_METRIC:
      SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
      break;
    case wxMM_LOMETRIC:
      SetLogicalScale( m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0 );
      break;
    default:
    case wxMM_TEXT:
      SetLogicalScale( 1.0, 1.0 );
      break;
  }
  if (mode != wxMM_TEXT)
  {
    m_needComputeScaleX = TRUE;
    m_needComputeScaleY = TRUE;
  }
}
void wxDC::SetUserScale( double x, double y )
{
  // allow negative ? -> no
  m_userScaleX = x;
  m_userScaleY = y;
  ComputeScaleAndOrigin();
}
void wxDC::SetLogicalScale( double x, double y )
{
  // allow negative ?
  m_logicalScaleX = x;
  m_logicalScaleY = y;
  ComputeScaleAndOrigin();
}
void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
  m_logicalOriginX = x * m_signX;   // is this still correct ?
  m_logicalOriginY = y * m_signY;
  ComputeScaleAndOrigin();
}
void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  m_externalDeviceOriginX = x;
  m_externalDeviceOriginY = y;
  ComputeScaleAndOrigin();
}

#if 0
void wxDC::SetInternalDeviceOrigin( long x, long y )
{
  m_internalDeviceOriginX = x;
  m_internalDeviceOriginY = y;
  ComputeScaleAndOrigin();
}
void wxDC::GetInternalDeviceOrigin( long *x, long *y )
{
  if (x) *x = m_internalDeviceOriginX;
  if (y) *y = m_internalDeviceOriginY;
}
#endif
void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
}/*
    
void wxDC::CalcBoundingBox( long x, long y )
{
  if (x < m_minX) m_minX = x;
  if (y < m_minY) m_minY = y;
  if (x > m_maxX) m_maxX = x;
  if (y > m_maxY) m_maxY = y;
}*/
wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxDC::GetDepth() const
{
	return wxDisplayDepth() ;
}

void wxDC::ComputeScaleAndOrigin()
{
  // CMB: copy scale to see if it changes
  double origScaleX = m_scaleX;
  double origScaleY = m_scaleY;

  m_scaleX = m_logicalScaleX * m_userScaleX;
  m_scaleY = m_logicalScaleY * m_userScaleY;

  m_deviceOriginX = m_internalDeviceOriginX + m_externalDeviceOriginX;
  m_deviceOriginY = m_internalDeviceOriginY + m_externalDeviceOriginY;

  // CMB: if scale has changed call SetPen to recalulate the line width 
  if (m_scaleX != origScaleX || m_scaleY != origScaleY)
  {
    // this is a bit artificial, but we need to force wxDC to think
    // the pen has changed
    wxPen* pen = & GetPen();
    wxPen tempPen;
    m_pen = tempPen;
    SetPen(* pen);
  }
}
void  wxDC::SetPalette( const wxPalette& palette )
{
}

void  wxDC::SetBackgroundMode( int mode )
{
	m_backgroundMode = mode ;
}

void  wxDC::SetFont( const wxFont &font )
{
    m_font = font;
    m_macFontInstalled = false ;
}

void  wxDC::SetPen( const wxPen &pen )
{
	if ( m_pen == pen )
		return ;
		
  m_pen = pen;

  m_macPenInstalled = false ;
}

void  wxDC::SetBrush( const wxBrush &brush )
{
  if (m_brush == brush) 
  	return;
  
  m_brush = brush;
  m_macBrushInstalled = false ;
}

void  wxDC::SetBackground( const wxBrush &brush )
{
  if (m_backgroundBrush == brush) 
  	return;
  
  m_backgroundBrush = brush;
  
  if (!m_backgroundBrush.Ok()) 
  	return;
   m_macBrushInstalled = false ;
}

void  wxDC::SetLogicalFunction( int function )
{
 	if (m_logicalFunction == function) 
 		return;

	m_logicalFunction = function ;
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
}

void  wxDC::DoFloodFill( wxCoord x, wxCoord y, const wxColour& col,
                             int style )
{
}

bool  wxDC::DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const 
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC::DoGetPixel  Invalid DC") );
    wxMacPortSetter helper(this) ;

    RGBColor colour;

    GetCPixel( XLOG2DEVMAC(x), YLOG2DEVMAC(y), &colour );

    // Convert from Mac colour to wx
    col->Set( colour.red   >> 8,
              colour.green >> 8,
              colour.blue  >> 8);

    return true ;
}

void  wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  	
  wxMacPortSetter helper(this) ;
  
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
		MacInstallPen() ;
		wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
                            m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2;

        wxCoord xx1 = XLOG2DEVMAC(x1) - offset;
        wxCoord yy1 = YLOG2DEVMAC(y1) - offset;
        wxCoord xx2 = XLOG2DEVMAC(x2) - offset;
        wxCoord yy2 = YLOG2DEVMAC(y2) - offset;

        if ((m_pen.GetCap() == wxCAP_ROUND) &&
            (m_pen.GetWidth() <= 1))
    	{
    	    // Implement LAST_NOT for MAC at least for
    	    // orthogonal lines. RR.
    	 	if (xx1 == xx2)
    	 	{
    	 	  	if (yy1 < yy2)
    	 	  		yy2--;
    	 	    if (yy1 > yy2)
    	 	    	yy2++;
    	 	}
    	 	if (yy1 == yy2)
    	 	{
    	 	  	if (xx1 < xx2)
    	 	  		xx2--;
    	 	    if (xx1 > xx2)
    	 	    	xx2++;
    	 	}
    	}
    	
		::MoveTo(xx1, yy1);
		::LineTo(xx2, yy2);
  }
}

void  wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoCrossHair  Invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        wxCoord xx = XLOG2DEVMAC(x);
        wxCoord yy = YLOG2DEVMAC(y);

        MacInstallPen();
        ::MoveTo( XLOG2DEVMAC(0), yy );
        ::LineTo( XLOG2DEVMAC(w), yy );
        ::MoveTo( xx, YLOG2DEVMAC(0) );
        ::LineTo( xx, YLOG2DEVMAC(h) );
    }
}

/*
 * To draw arcs properly the angles need to be converted from the WX style:
 * Angles start on the +ve X axis and go anti-clockwise (As you would draw on
 * a normal axis on paper).
 * TO
 * the Mac style:
 * Angles start on the +ve y axis and go clockwise.
 * To achive this I work out which quadrant the angle lies in then map this to
 * the equivalent quadrant on the Mac.  (Sin and Cos values reveal which
 * quadrant you are in).
 */
static double wxConvertWXangleToMACangle(double angle)
{
    double sin_a, cos_a;

    sin_a = sin(angle / RAD2DEG);
    cos_a = cos(angle / RAD2DEG);

    if( (sin_a >= 0.0) && (cos_a >= 0.0) ) {
        angle = acos(sin_a) * RAD2DEG;
    }
    else if( (sin_a >= 0.0) && (cos_a <= 0.0) ) {
        sin_a *= -1;
        angle = acos(sin_a) * RAD2DEG + 180;
    }
    else if( (sin_a <= 0.0) && (cos_a >=  0.0) ) {
        angle = acos(sin_a) * RAD2DEG + 180;
    }
    else if( (sin_a <  0.0) && (cos_a <  0.0) ) {
        sin_a *= -1;
        angle = acos(sin_a) * RAD2DEG + 180;
    }
    return angle;
}

void  wxDC::DoDrawArc( wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawArc  Invalid DC"));

    wxCoord xx1 = XLOG2DEVMAC(x1);
    wxCoord yy1 = YLOG2DEVMAC(y1);
    wxCoord xx2 = XLOG2DEVMAC(x2);
    wxCoord yy2 = YLOG2DEVMAC(y2);
    wxCoord xxc = XLOG2DEVMAC(xc);
    wxCoord yyc = YLOG2DEVMAC(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord rad   = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else if (radius == 0.0)
    {
        radius1 = radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy1-yyc), double(xx1-xxc)) * RAD2DEG;
         radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy2-yyc), double(xx2-xxc)) * RAD2DEG;
     }
     wxCoord alpha2 = wxCoord(radius2 - radius1);
    wxCoord alpha1 = wxCoord(wxConvertWXangleToMACangle(radius1));
     if( (xx1 > xx2) || (yy1 > yy2) ) {
        alpha2 *= -1;
    }

    Rect r = { yyc - rad, xxc - rad, yyc + rad, xxc + rad };

    if(m_brush.GetStyle() != wxTRANSPARENT) {
        MacInstallBrush();
        PaintArc(&r, alpha1, alpha2);
    }
    if(m_pen.GetStyle() != wxTRANSPARENT) {
        MacInstallPen();
        FrameArc(&r, alpha1, alpha2);
    }
}

void  wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawEllepticArc  Invalid DC"));

    Rect r;
    double angle = sa - ea;  // Order important Mac in opposite direction to wx
 
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(w);
    wxCoord hh = m_signY * YLOG2DEVREL(h);

    // handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    sa = wxConvertWXangleToMACangle(sa);

    r.top    = yy;
    r.left   = xx;
    r.bottom = yy + hh;
    r.right  = xx + ww;

    if(m_brush.GetStyle() != wxTRANSPARENT) {
        MacInstallBrush();
        PaintArc(&r, (short)sa, (short)angle);
    }
    if(m_pen.GetStyle() != wxTRANSPARENT) {
        MacInstallPen();
        FrameArc(&r, (short)sa, (short)angle);
    }
}

void  wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  	
  wxMacPortSetter helper(this) ;
  
  if (m_pen.GetStyle() != wxTRANSPARENT) 
  {
		MacInstallPen() ;
        wxCoord xx1 = XLOG2DEVMAC(x); 
        wxCoord yy1 = YLOG2DEVMAC(y);
	
		::MoveTo(xx1,yy1);
		::LineTo(xx1+1, yy1+1);
  }
}

void  wxDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset)
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  wxMacPortSetter helper(this) ;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) 
  	return;

  MacInstallPen() ;
  
  wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
                      m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2 ;

  wxCoord x1, x2 , y1 , y2 ;
  x1 = XLOG2DEVMAC(points[0].x + xoffset);
  y1 = YLOG2DEVMAC(points[0].y + yoffset);   
  ::MoveTo(x1 - offset, y1 - offset );
  
  for (int i = 0; i < n-1; i++)
  {
    x2 = XLOG2DEVMAC(points[i+1].x + xoffset);
    y2 = YLOG2DEVMAC(points[i+1].y + yoffset);
    ::LineTo( x2 - offset, y2 - offset );
  }
}

void  wxDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle )
{
  	wxCHECK_RET(Ok(), wxT("Invalid DC"));
  	wxMacPortSetter helper(this) ;
  	
  	wxCoord x1, x2 , y1 , y2 ;
  
  	if (m_brush.GetStyle() != wxTRANSPARENT)
  	{
  		PolyHandle polygon = OpenPoly();
  		
  		x1 = XLOG2DEVMAC(points[0].x + xoffset);
  		y1 = YLOG2DEVMAC(points[0].y + yoffset);   
  		::MoveTo(x1,y1);
  
  		for (int i = 0; i < n-1; i++)
  		{
    		x2 = XLOG2DEVMAC(points[i+1].x + xoffset);
    		y2 = YLOG2DEVMAC(points[i+1].y + yoffset);
    		::LineTo(x2, y2);
  		}

  		ClosePoly();

		MacInstallBrush();
		::PaintPoly( polygon );
		
		KillPoly( polygon );
	}
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
  		PolyHandle polygon = OpenPoly();
  		
  		x1 = XLOG2DEVMAC(points[0].x + xoffset);
  		y1 = YLOG2DEVMAC(points[0].y + yoffset);   
  		::MoveTo(x1,y1);
  
  		for (int i = 0; i < n-1; i++)
  		{
    		x2 = XLOG2DEVMAC(points[i+1].x + xoffset);
    		y2 = YLOG2DEVMAC(points[i+1].y + yoffset);
    		::LineTo(x2, y2);
  		}
  		
  		// return to origin to close path
  		::LineTo(x1,y1);

  		ClosePoly();
	
		MacInstallPen() ;
		::FramePoly( polygon ) ;
		
		KillPoly( polygon );
	}
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacPortSetter helper(this) ;

	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);
	
	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0) 
		return;
	
	// CMB: handle -ve width and/or height
	if (ww < 0) 
	{ 
		ww = -ww; 
		xx = xx - ww; 
	}
	
	if (hh < 0) 
	{ 
		hh = -hh; 
		yy = yy - hh; 
	}
	
	Rect rect = { yy , xx , yy + hh , xx + ww } ;
	
	if (m_brush.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallBrush() ;
		::PaintRect( &rect ) ;
	}
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FrameRect( &rect ) ;
	}
}

void  wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacPortSetter helper(this) ;
	
    if (radius < 0.0) 
  	    radius = - radius * ((width < height) ? width : height);
	
	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);
	
	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0) 
		return;
	
	// CMB: handle -ve width and/or height
	if (ww < 0) 
	{ 
		ww = -ww; 
		xx = xx - ww; 
	}
	
	if (hh < 0) 
	{ 
		hh = -hh; 
		yy = yy - hh; 
	}
	
	Rect rect = { yy , xx , yy + hh , xx + ww } ;
	
	if (m_brush.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallBrush() ;
		::PaintRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
	}
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FrameRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
	}
}

void  wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacPortSetter helper(this) ;

	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);

	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0)
		return;

	// CMB: handle -ve width and/or height
	if (ww < 0)
	{
		ww = -ww;
		xx = xx - ww;
	}

	if (hh < 0)
	{
		hh = -hh;
		yy = yy - hh;
	}

	Rect rect = { yy , xx , yy + hh , xx + ww } ;

	if (m_brush.GetStyle() != wxTRANSPARENT)
	{
		MacInstallBrush() ;
		::PaintOval( &rect ) ;
	}

	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FrameOval( &rect ) ;
	}
}


    
bool  wxDC::CanDrawBitmap(void) const 
{
	return true ;
}


bool  wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool useMask,
                        wxCoord xsrcMask,  wxCoord ysrcMask )
{
    wxCHECK_MSG(Ok(), false, wxT("wxDC::DoBlit Illegal dc"));
    wxCHECK_MSG(source->Ok(), false, wxT("wxDC::DoBlit  Illegal source DC"));
    wxMacPortSetter helper(this) ;

    /* TODO: use the mask origin when drawing transparently */
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }

	CGrafPtr			sourcePort = (CGrafPtr) source->m_macPort ;
	PixMapHandle	bmappixels =  GetGWorldPixMap( sourcePort ) ; 
	RGBColor	white = { 0xFFFF, 0xFFFF,0xFFFF} ;
	RGBColor	black = { 0,0,0} ;
	RGBColor        forecolor = m_textForegroundColour.GetPixel();
	RGBColor        backcolor = m_textBackgroundColour.GetPixel();
	RGBForeColor( &forecolor ) ;
	RGBBackColor( &backcolor ) ;

	if ( LockPixels(bmappixels) )
	{
		Rect srcrect , dstrect ;
		srcrect.top = source->YLOG2DEVMAC(ysrc) ;
		srcrect.left = source->XLOG2DEVMAC(xsrc)  ;
		srcrect.right = source->XLOG2DEVMAC(xsrc + width ) ;
		srcrect.bottom = source->YLOG2DEVMAC(ysrc + height) ;
		dstrect.top = YLOG2DEVMAC(ydest) ;
		dstrect.left = XLOG2DEVMAC(xdest) ;
		dstrect.bottom = YLOG2DEVMAC(ydest + height )  ;
		dstrect.right = XLOG2DEVMAC(xdest + width ) ;

    	short  mode = (logical_func == wxCOPY ? srcCopy :
 //   	logical_func == wxCLEAR ? WHITENESS :
 //   	logical_func == wxSET ? BLACKNESS :
   		logical_func == wxINVERT ? hilite :
  //  	logical_func == wxAND ? MERGECOPY :
    	logical_func == wxOR ? srcOr :
    	logical_func == wxSRC_INVERT ? notSrcCopy :
    	logical_func == wxXOR ? srcXor :
  //  	logical_func == wxOR_REVERSE ? MERGEPAINT :
  //  	logical_func == wxAND_REVERSE ? SRCERASE :
  //  	logical_func == wxSRC_OR ? srcOr :
  //  	logical_func == wxSRC_AND ? SRCAND :
    		srcCopy );

		if ( useMask && source->m_macMask )
		{
		    if ( mode == srcCopy )
		    {
    			if ( LockPixels( GetGWorldPixMap( source->m_macMask ) ) )
    			{
    				CopyMask( GetPortBitMapForCopyBits( sourcePort ) , 
    				            GetPortBitMapForCopyBits( source->m_macMask ) , 
    				            GetPortBitMapForCopyBits( m_macPort ) ,
    					        &srcrect, &srcrect , &dstrect ) ;
    				UnlockPixels( GetGWorldPixMap( source->m_macMask )  ) ;
    			}
		    }
		    else
		    {
                RgnHandle clipRgn = NewRgn() ;
                
                LockPixels( GetGWorldPixMap( source->m_macMask ) ) ;
                BitMapToRegion( clipRgn , (BitMap*) *GetGWorldPixMap( source->m_macMask ) ) ;
                UnlockPixels( GetGWorldPixMap( source->m_macMask ) ) ;
                //OffsetRgn( clipRgn , -source->m_macMask->portRect.left , -source->m_macMask->portRect.top ) ;
                OffsetRgn( clipRgn , -srcrect.left + dstrect.left, -srcrect.top +  dstrect.top ) ;
    			CopyBits( GetPortBitMapForCopyBits( sourcePort ) , 
    			    GetPortBitMapForCopyBits( m_macPort ) ,
    				&srcrect, &dstrect, mode, clipRgn ) ;
    		    DisposeRgn( clipRgn ) ;
    		}
		}
		else
		{
			CopyBits( GetPortBitMapForCopyBits( sourcePort ) , 
			    GetPortBitMapForCopyBits( m_macPort ) ,
				&srcrect, &dstrect, mode, NULL ) ;
		}
		UnlockPixels( bmappixels ) ;
	} 
	
	m_macPenInstalled = false ;
	m_macBrushInstalled = false ;
	m_macFontInstalled = false ;
  	
  return TRUE;
}

void  wxDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                              double angle)
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoDrawRotatedText  Invalid window dc") );

    if (angle == 0.0)
    {
        DrawText(text, x, y);
        return;
    }

    MacInstallFont();

    // the size of the text
    wxCoord w, h;
    GetTextExtent(text, &w, &h);

    // draw the string normally
    wxBitmap src(w, h);
    wxMemoryDC dc;
    dc.SelectObject(src);
    dc.SetFont(GetFont());
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.Clear();
    dc.DrawText(text, 0, 0);
    dc.SetFont(wxNullFont);
    dc.SelectObject(wxNullBitmap);

    wxMacPortSetter helper(this) ;

    // Calculate the size of the rotated bounding box.
    double rad = DegToRad(angle);
    double dx = cos(rad);
    double dy = sin(rad);

    // the rectngle vertices are counted clockwise with the first one being at
    // (0, 0) (or, rather, at (x, y))
    double x2 = w * dx;
    double y2 = -w * dy;      // y axis points to the bottom, hence minus
    double x4 = h * dy;
    double y4 = h * dx;
    double x3 = x4 + x2;
    double y3 = y4 + y2;

    // calc max and min
    wxCoord maxX = (wxCoord)(dmax(x2, dmax(x3, x4)) + 0.5);
    wxCoord maxY = (wxCoord)(dmax(y2, dmax(y3, y4)) + 0.5);
    wxCoord minX = (wxCoord)(dmin(x2, dmin(x3, x4)) - 0.5);
    wxCoord minY = (wxCoord)(dmin(y2, dmin(y3, y4)) - 0.5);
 
    // prepare to blit-with-rotate the bitmap to the DC
    wxImage image(src);

    RGBColor colText = m_textForegroundColour.GetPixel();
    RGBColor colBack = m_textBackgroundColour.GetPixel();

    unsigned char *data = image.GetData();

    wxCoord dstX, dstY;
    double r, angleOrig;
    bool textPixel;

    // paint pixel by pixel
    for ( wxCoord srcX = 0; srcX < w; srcX++ )
    {
        for ( wxCoord srcY = 0; srcY < h; srcY++ )
        {
            // transform source coords to dest coords
            r         = sqrt( (double)(srcX * srcX + srcY * srcY) );
            angleOrig = atan2((double)srcY, (double)srcX) - rad;
            dstX      = (wxCoord)(r * cos(angleOrig) + 0.5);
            dstY      = (wxCoord)(r * sin(angleOrig) + 0.5);

            // black pixel?
            textPixel = data[(srcY*w + srcX)*3] == 0;
            if ( textPixel || (m_backgroundMode == wxSOLID) )
            {
                SetCPixel(XLOG2DEVMAC(x + dstX), YLOG2DEVMAC(y + dstY),
                          textPixel ? &colText : &colBack);
            }
        }
    }

    // it would be better to draw with non underlined font and draw the line
    // manually here (it would be more straight...)
#if 0
    if ( m_font.GetUnderlined() )
    {
        ::MoveTo(XLOG2DEVMAC(x + x4), YLOG2DEVMAC(y + y4 + font->descent));
        ::LineTo(XLOG2DEVMAC(x + x3), YLOG2DEVMAC(y + y3 + font->descent));
    }
#endif // 0

    // update the bounding box
    CalcBoundingBox(x + minX, y + minY);
    CalcBoundingBox(x + maxX, y + maxY);
}
void  wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawText  Invalid DC"));
    wxMacPortSetter helper(this) ;

	long xx = XLOG2DEVMAC(x);
	long yy = YLOG2DEVMAC(y);
  
//	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallFont() ;
		/*
		Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
			
		  ::ClipRect( &clip ) ;
		*/
		
		FontInfo fi ;
		::GetFontInfo( &fi ) ;
		
		yy += fi.ascent ;
		::MoveTo( xx , yy );
		if (  m_backgroundMode == wxTRANSPARENT )
		{
			::TextMode( srcOr) ;
		}
		else
		{
			::TextMode( srcCopy ) ;
		}

		const char *text = NULL ;
		int length = 0 ;
		wxString macText ;

		if ( wxApp::s_macDefaultEncodingIsPC )
		{
			macText = wxMacMakeMacStringFromPC( strtext ) ;
			text = macText ;
			length = macText.Length() ;
		}
		else
		{
			text = strtext ;
			length = strtext.Length() ;
		}
		
		int laststop = 0 ;
		int i = 0 ;
		int line = 0 ;
		
		while( i < length )
		{
			if( text[i] == 13 || text[i] == 10)
			{
				::DrawText( text , laststop , i - laststop ) ;
				line++ ;
				::MoveTo( xx , yy + line*(fi.descent + fi.ascent + fi.leading) );
				laststop = i+1 ;
			}
			i++ ;
		}
				
		::DrawText( text , laststop , i - laststop ) ;
		::TextMode( srcOr ) ;
	}
}

bool  wxDC::CanGetTextExtent() const 
{
	wxCHECK_MSG(Ok(), false, wxT("Invalid DC"));
		
	return true ;
}

void  wxDC::DoGetTextExtent( const wxString &string, wxCoord *width, wxCoord *height,
                     wxCoord *descent, wxCoord *externalLeading ,
                     wxFont *theFont ) const
{
   wxCHECK_RET(Ok(), wxT("Invalid DC"));
   wxMacPortSetter helper(this) ;

 	wxFont formerFont = m_font ; 
 	
	if ( theFont )
	{
		wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;
	
		if ( font )
		{
			::TextFont( font->m_macFontNum ) ;
			::TextSize( YLOG2DEVREL( font->m_macFontSize) ) ;
			::TextFace( font->m_macFontStyle ) ;
		}
	}
	else
	{
		MacInstallFont() ;
	}

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	if ( height )
		*height = YDEV2LOGREL( fi.descent + fi.ascent ) ;
	if ( descent )
		*descent =YDEV2LOGREL( fi.descent );
	if ( externalLeading )
		*externalLeading = YDEV2LOGREL( fi.leading ) ;
	
	const char *text = NULL ;
	int length = 0 ;
	wxString macText ;
	if ( wxApp::s_macDefaultEncodingIsPC )
	{
		macText = wxMacMakeMacStringFromPC( string ) ;
		text = macText ;
		length = macText.Length() ;
	}
	else
	{
		text = string ;
		length = string.Length() ;
	}
	
	int laststop = 0 ;
	int i = 0 ;
	int curwidth = 0 ;
	if ( width )
	{
		*width = 0 ;
	
		while( i < length )
		{
			if( text[i] == 13 || text[i] == 10)
			{
				if ( height )
					*height += YDEV2LOGREL( fi.descent + fi.ascent + fi.leading ) ;
				curwidth = ::TextWidth( text , laststop , i - laststop ) ;
				if ( curwidth > *width )
					*width = XDEV2LOGREL( curwidth ) ;
				laststop = i+1 ;
			}
			i++ ;
		}
				
		curwidth = ::TextWidth( text , laststop , i - laststop ) ;
		if ( curwidth > *width )
			*width = XDEV2LOGREL( curwidth ) ;
	}

	if ( theFont )
	{
		m_macFontInstalled = false ;
	}
}

wxCoord   wxDC::GetCharWidth(void) const
{
    wxCHECK_MSG(Ok(), 1, wxT("Invalid DC"));
 
   wxMacPortSetter helper(this) ;

	MacInstallFont() ;

    int width = ::TextWidth( "n" , 0 , 1 ) ;

	return YDEV2LOGREL(width) ;
}

wxCoord   wxDC::GetCharHeight(void) const
{
    wxCHECK_MSG(Ok(), 1, wxT("Invalid DC"));
 
    wxMacPortSetter helper(this) ;

	MacInstallFont() ;

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	return YDEV2LOGREL( fi.descent + fi.ascent );
}

void  wxDC::Clear(void)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));  
    wxMacPortSetter helper(this) ;
	Rect rect = { -32767 , -32767 , 32767 , 32767 } ;
	
	if (m_backgroundBrush.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallBrush() ;
		::EraseRect( &rect ) ;
	}
}

void wxDC::MacInstallFont() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
//	if ( m_macFontInstalled )
//		return ;
	Pattern blackColor ;
	
	wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;

	if ( font )
	{
		::TextFont( font->m_macFontNum ) ;
		::TextSize( short(m_scaleY * font->m_macFontSize) ) ;
		::TextFace( font->m_macFontStyle ) ;
	
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;

		RGBColor forecolor = m_textForegroundColour.GetPixel();
		RGBColor backcolor = m_textBackgroundColour.GetPixel();
		::RGBForeColor( &forecolor );
		::RGBBackColor( &backcolor );
	}
	else
	{
		short fontnum ;
		
		GetFNum( "\pGeneva" , &fontnum ) ;
		::TextFont( fontnum ) ;
		::TextSize( short(m_scaleY * 10) ) ;
		::TextFace( 0 ) ;
	
		// todo reset after spacing changes - or store the current spacing somewhere
	
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;

		RGBColor forecolor = m_textForegroundColour.GetPixel();
		RGBColor backcolor = m_textBackgroundColour.GetPixel();
		::RGBForeColor( &forecolor );
		::RGBBackColor( &backcolor );
	}

	short mode = patCopy ;

	// todo :
	
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(GetQDGlobalsBlack(&blackColor));
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;

		// unsupported TODO

		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
}

static void wxMacGetHatchPattern(int hatchStyle, Pattern *pattern)
{
	int thePatListID = sysPatListID;
	int theIndex;
	switch(hatchStyle)
	{
		case wxBDIAGONAL_HATCH:
			theIndex = 34; // WCH: this is not good
			break;
		case wxFDIAGONAL_HATCH:
			theIndex = 26;
			break;
		case wxCROSS_HATCH:
			theIndex = 5;
			break;
		case wxHORIZONTAL_HATCH:
			theIndex = 25;
			break;
		case wxVERTICAL_HATCH:
			theIndex = 6;
			break;
		case wxCROSSDIAG_HATCH:
			theIndex = 4; // WCH: this is not good
			break;
		default:
			theIndex = 1; // solid pattern
			break;
	}
	GetIndPattern( pattern, thePatListID, theIndex);	
}

void wxDC::MacInstallPen() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

	Pattern	 blackColor;

//	if ( m_macPenInstalled )
//		return ;

	RGBColor forecolor = m_pen.GetColour().GetPixel();
	RGBColor backcolor = m_backgroundBrush.GetColour().GetPixel();
	::RGBForeColor( &forecolor );
	::RGBBackColor( &backcolor );
	
	::PenNormal() ;
	int penWidth = m_pen.GetWidth() * (int) m_scaleX ;

	// null means only one pixel, at whatever resolution
	if ( penWidth == 0 )
		penWidth = 1 ; 
	::PenSize(penWidth, penWidth);

	int penStyle = m_pen.GetStyle();
	
	if (penStyle == wxSOLID)
	{
		::PenPat(GetQDGlobalsBlack(&blackColor));
	}
	else if (IS_HATCH(penStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(penStyle, &pat);
		::PenPat(&pat);
	}
	else
	{
		::PenPat(GetQDGlobalsBlack(&blackColor));
	}

	short mode = patCopy ;
	
	// todo :
	
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(GetQDGlobalsBlack(&blackColor));
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;

		// unsupported TODO

		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
	m_macPenInstalled = true ;
	m_macBrushInstalled = false ;
	m_macFontInstalled = false ;
}

void wxDC::MacSetupBackgroundForCurrentPort(const wxBrush& background ) 
{
    Pattern whiteColor ;
    switch( background.MacGetBrushKind() )
    {
      case kwxMacBrushTheme :
        {
	        ::SetThemeBackground( background.GetMacTheme() , wxDisplayDepth() , true ) ;
          break ;
        }
      case kwxMacBrushThemeBackground :
        {
	        Rect extent ;
	        ThemeBackgroundKind bg = background.GetMacThemeBackground( &extent ) ;
	        ::ApplyThemeBackground( bg , &extent ,kThemeStateActive , wxDisplayDepth() , true ) ;
          break ;
        }
      case kwxMacBrushColour :
        {
       	  ::RGBBackColor( &background.GetColour().GetPixel() );
        	int brushStyle = background.GetStyle();
        	if (brushStyle == wxSOLID)
        		::BackPat(GetQDGlobalsWhite(&whiteColor));
        	else if (IS_HATCH(brushStyle))
        	{
        		Pattern pat ;
        		wxMacGetHatchPattern(brushStyle, &pat);
        		::BackPat(&pat);
        	}
        	else
        	{
        		::BackPat(GetQDGlobalsWhite(&whiteColor));
        	}
          break ;
        }
    }
}

void wxDC::MacInstallBrush() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

	Pattern	 blackColor ;
//	if ( m_macBrushInstalled )
//		return ;

	// foreground

	::RGBForeColor( &m_brush.GetColour().GetPixel() );

	int brushStyle = m_brush.GetStyle();
	if (brushStyle == wxSOLID)
		::PenPat(GetQDGlobalsBlack(&blackColor));
	else if (IS_HATCH(brushStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(brushStyle, &pat);
		::PenPat(&pat);
	}
	else
	{
		::PenPat(GetQDGlobalsBlack(&blackColor));
	}

	
	// background
	
    MacSetupBackgroundForCurrentPort( m_backgroundBrush ) ;
	

	// todo :
	
	short mode = patCopy ;
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(GetQDGlobalsBlack(&blackColor));
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;

		// unsupported TODO

		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
	m_macBrushInstalled = true ;
	m_macPenInstalled = false ;
	m_macFontInstalled = false ;
}

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------


wxCoord wxDCBase::DeviceToLogicalX(wxCoord x) const
{
    return ((wxDC *)this)->XDEV2LOG(x);
}

wxCoord wxDCBase::DeviceToLogicalY(wxCoord y) const
{
    return ((wxDC *)this)->YDEV2LOG(y);
}

wxCoord wxDCBase::DeviceToLogicalXRel(wxCoord x) const
{
    return ((wxDC *)this)->XDEV2LOGREL(x);
}

wxCoord wxDCBase::DeviceToLogicalYRel(wxCoord y) const
{
    return ((wxDC *)this)->YDEV2LOGREL(y);
}

wxCoord wxDCBase::LogicalToDeviceX(wxCoord x) const
{
    return ((wxDC *)this)->XLOG2DEV(x);
}

wxCoord wxDCBase::LogicalToDeviceY(wxCoord y) const
{
    return ((wxDC *)this)->YLOG2DEV(y);
}

wxCoord wxDCBase::LogicalToDeviceXRel(wxCoord x) const
{
    return ((wxDC *)this)->XLOG2DEVREL(x);
}

wxCoord wxDCBase::LogicalToDeviceYRel(wxCoord y) const
{
    return ((wxDC *)this)->YLOG2DEVREL(y);
}
