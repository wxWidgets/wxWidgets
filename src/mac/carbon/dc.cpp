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

long wxDC::m_macCurrentPortId = 1 ;

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC()
{
  m_ok = FALSE;
//  m_optimize = FALSE;
//  m_autoSetting = FALSE;
  m_colour = TRUE;
  m_clipping = FALSE;
  
  m_mm_to_pix_x = mm2pt;
  m_mm_to_pix_y = mm2pt;
  
  m_logicalOriginX = 0;
  m_logicalOriginY = 0;
  m_deviceOriginX = 0;
  m_deviceOriginY = 0;
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
  
  m_mappingMode = wxMM_TEXT;
  m_needComputeScaleX = FALSE;
  m_needComputeScaleY = FALSE;
  
  m_signX = 1;  // default x-axis left to right
  m_signY = 1;  // default y-axis top down

  m_maxX = m_maxY = -100000;
  m_minY = m_minY =  100000;

  m_logicalFunction = wxCOPY;
//  m_textAlignment = wxALIGN_TOP_LEFT;
  m_backgroundMode = wxTRANSPARENT;
  
  m_textForegroundColour = *wxBLACK;
  m_textBackgroundColour = *wxWHITE;
  m_pen = *wxBLACK_PEN;
  m_font = *wxNORMAL_FONT;
  m_brush = *wxTRANSPARENT_BRUSH;
  m_backgroundBrush = *wxWHITE_BRUSH;
  
//  m_palette = wxAPP_COLOURMAP;
  m_macPort = NULL ;
  m_macMask = NULL ;
  m_ok = FALSE ;
  
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
	
	m_macPortId = 0 ;
	m_macLocalOrigin.h = m_macLocalOrigin.v = 0 ;
	m_macClipRect.left = -32000 ;
	m_macClipRect.top = -32000 ;
	m_macClipRect.right = 32000 ;
	m_macClipRect.bottom = 32000 ;
	::GetPort( &m_macOrigPort ) ;
};

wxDC::~wxDC(void)
{
	if ( !m_macPortHelper.IsCleared() )
	{
	GrafPtr port ;
	GetPort( &port ) ;
	SetPort( m_macPortHelper.GetCurrentPort() ) ;
	SetOrigin( 0 , 0 ) ;
	SetPort( port ) ;
	}
	/*
	if ( m_macPort )
	{
		::SetPort( m_macPort ) ;
		::SetOrigin( 0 , 0 ) ;
		::ClipRect( &m_macPort->portRect ) ;
		::PenNormal() ;
		::SetPort( m_macOrigPort ) ;
	}
	*/
	++m_macCurrentPortId ;
};

void wxDC::MacSetupPort() const
{
	AGAPortHelper* help = (AGAPortHelper*) &m_macPortHelper ;
	help->Setup( m_macPort ) ;
	m_macPortId = ++m_macCurrentPortId ;
	::SetOrigin(-m_macLocalOrigin.h, -m_macLocalOrigin.v);
	::ClipRect(&m_macClipRect);

	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;	
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
	float scale = 1.0 ;
	
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
  long xx1 = XLOG2DEV(x); 
  long yy1 = YLOG2DEV(y);
	
	{
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( bmp.GetRefData()) ;
	   	
   	if ( bmap )
    {
			if ( bmap->m_bitmapType == kMacBitmapTypePict )
			{ 
    		Rect bitmaprect = { 0 , 0 , int(bmap->m_height * scale) , int(bmap->m_width * scale)} ;
				::OffsetRect( &bitmaprect , xx1 , yy1 ) ;
				::DrawPicture( bmap->m_hPict , &bitmaprect ) ;
			}
			else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
			{
				if ( bmap->m_hBitmap )
				{
					GWorldPtr 		bmapworld = bmap->m_hBitmap ;
					PixMapHandle	bmappixels ;
					RGBColor		white = { 0xFFFF, 0xFFFF,0xFFFF} ;
					RGBColor		black = { 0,0,0} ;
					RGBForeColor( &black ) ;
					RGBBackColor( &white ) ;
			
					bmappixels = GetGWorldPixMap( bmapworld ) ;
					if ( LockPixels(bmappixels) )
					{
						Rect source , dest ;
						source.top = 0 ;
						source.left = 0 ;
						source.right = bmap->m_width ;
						source.bottom = bmap->m_height ;
						dest.top = YLOG2DEV(y) ;
						dest.left = XLOG2DEV(x) ;
						dest.bottom = YLOG2DEV(y + bmap->m_height * scale)  ;
						dest.right = XLOG2DEV(x + bmap->m_width * scale ) ;
	
						if ( useMask && bmp.GetMask() )
						{
							if ( LockPixels( GetGWorldPixMap( bmp.GetMask()->GetMaskBitmap( ) ) ) )
							{
								CopyMask( GetPortBitMapForCopyBits( bmapworld ) , GetPortBitMapForCopyBits( bmp.GetMask()->GetMaskBitmap( ) ) ,
								 GetPortBitMapForCopyBits( m_macPort ) ,
									&source, &source , &dest ) ;
								UnlockPixels( GetGWorldPixMap( bmp.GetMask()->GetMaskBitmap( ) )  ) ;
							}
						}
						else
							CopyBits( GetPortBitMapForCopyBits( bmapworld ) , GetPortBitMapForCopyBits( m_macPort ),
								&source, &dest, srcCopy, NULL ) ;

						UnlockPixels( bmappixels ) ;
					} 
					m_macPenInstalled = false ;
					m_macBrushInstalled = false ;
					m_macFontInstalled = false ;
				}
			}
    }
	}
}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
  long xx1 = XLOG2DEV(x); 
  long yy1 = YLOG2DEV(y);
	
	{
		wxIconRefData * iconref = (wxIconRefData*) ( icon.GetRefData()) ;
	   	
   	if ( iconref && iconref->m_ok && iconref->m_hIcon )
    {
    	Rect bitmaprect = { 0 , 0 , iconref->m_height , iconref->m_width } ;
			OffsetRect( &bitmaprect , xx1 , yy1 ) ;
    	PlotCIconHandle( &bitmaprect , atNone , ttNone , iconref->m_hIcon ) ;
    }
	}
};

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
  	MacVerifySetup() ;
	if( m_clipping )
	{
		m_clipX1 = wxMax( m_clipX1 , x ) ;
  		m_clipY1 = wxMax( m_clipY1 ,y );
  		m_clipX2 = wxMin( m_clipX2, (x + width));
  		m_clipY2 = wxMin( m_clipY2,(y + height));
		
	}
	else
	{
	  m_clipping = TRUE;
	  m_clipX1 = x;
	  m_clipY1 = y;
	  m_clipX2 = x + width;
	  m_clipY2 = y + height;
	}
  
	long x1 = XLOG2DEV(m_clipX1);
	long y1 = YLOG2DEV(m_clipY1);
	long x2 = XLOG2DEV(m_clipX2);
	long y2 = YLOG2DEV(m_clipY2);
	
	Rect clip = { y1 , x1 , y2 , x2 } ;
	
  	::ClipRect( &clip ) ;

};

void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

  	MacVerifySetup() ;
    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    wxCoord xx, yy, ww, hh;
    region.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );
}

void wxDC::DestroyClippingRegion(void)
{
  MacVerifySetup() ;
  m_clipping = FALSE;
//	Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
	::ClipRect(&m_macClipRect);
};
    
void wxDC::DoGetSize( int* width, int* height ) const
{
  *width = m_maxX-m_minX;
  *height = m_maxY-m_minY;
};

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );
  *width = long( double(w) / (m_scaleX*m_mm_to_pix_x) );
  *height = long( double(h) / (m_scaleY*m_mm_to_pix_y) );
};

void wxDC::SetTextForeground( const wxColour &col )
{
  if (!Ok()) return;
  m_textForegroundColour = col;
  m_macFontInstalled = false ;
};

void wxDC::SetTextBackground( const wxColour &col )
{
  if (!Ok()) return;
  m_textBackgroundColour = col;
  m_macFontInstalled = false ;
};

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
  };
  if (mode != wxMM_TEXT)
  {
    m_needComputeScaleX = TRUE;
    m_needComputeScaleY = TRUE;
  };
};

void wxDC::SetUserScale( double x, double y )
{
  // allow negative ? -> no
  m_userScaleX = x;
  m_userScaleY = y;
  ComputeScaleAndOrigin();
};

void wxDC::SetLogicalScale( double x, double y )
{
  // allow negative ?
  m_logicalScaleX = x;
  m_logicalScaleY = y;
  ComputeScaleAndOrigin();
};

void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
  m_logicalOriginX = x * m_signX;   // is this still correct ?
  m_logicalOriginY = y * m_signY;
  ComputeScaleAndOrigin();
};

void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
  m_externalDeviceOriginX = x;
  m_externalDeviceOriginY = y;
  ComputeScaleAndOrigin();
};
/*
void wxDC::SetInternalDeviceOrigin( long x, long y )
{
  m_internalDeviceOriginX = x;
  m_internalDeviceOriginY = y;
  ComputeScaleAndOrigin();
};

void wxDC::GetInternalDeviceOrigin( long *x, long *y )
{
  if (x) *x = m_internalDeviceOriginX;
  if (y) *y = m_internalDeviceOriginY;
};
*/
void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
};
/*
    
void wxDC::CalcBoundingBox( long x, long y )
{
  if (x < m_minX) m_minX = x;
  if (y < m_minY) m_minY = y;
  if (x > m_maxX) m_maxX = x;
  if (y > m_maxY) m_maxY = y;
};
*/
wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxDC::GetDepth() const
{
	return wxDisplayDepth() ;
}

void wxDC::ComputeScaleAndOrigin(void)
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
};

void  wxDC::SetPalette( const wxPalette& palette )
{
}

void  wxDC::SetBackgroundMode( int mode )
{
	m_backgroundMode = mode ;
}

void  wxDC::SetFont( const wxFont &font )
{
  if (!Ok()) 
  	return;
  	
  MacVerifySetup() ;
  
  m_font = font;
  m_macFontInstalled = false ;
}

void  wxDC::SetPen( const wxPen &pen )
{
  if (!Ok()  ) 
  	return;

  MacVerifySetup() ;

	if ( m_pen == pen )
		return ;
		
  m_pen = pen;
/*  
  if (!m_pen.Ok()) 
  	return;
*/  	
  m_macPenInstalled = false ;
}

void  wxDC::SetBrush( const wxBrush &brush )
{
  if (!Ok() ) 
  	return;
  MacVerifySetup() ;
  
  if (m_brush == brush) 
  	return;
  
  m_brush = brush;
  m_macBrushInstalled = false ;
}

void  wxDC::SetBackground( const wxBrush &brush )
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
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
	return true ;
}

void  wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
  if (!Ok()) 
  	return;
  	
  MacVerifySetup() ;
  
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
		MacInstallPen() ;
		int offset = ( (m_pen.GetWidth() == 0 ? 1 : m_pen.GetWidth() ) * m_scaleX - 1) / 2 ;	
  	long xx1 = XLOG2DEV(x1); 
  	long yy1 = YLOG2DEV(y1);
  	long xx2 = XLOG2DEV(x2); 
  	long yy2 = YLOG2DEV(y2);
	
		::MoveTo(xx1 - offset ,yy1 - offset);
		::LineTo(xx2 - offset , yy2 - offset );
  };
}

void  wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
}

void  wxDC::DoDrawArc( wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc )
{
}

void  wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea )
{
}

void  wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
  if (!Ok()) 
  	return;
  	
  MacVerifySetup() ;
  
  if (m_pen.GetStyle() != wxTRANSPARENT) 
  {
		MacInstallPen() ;
  	long xx1 = XLOG2DEV(x); 
  	long yy1 = YLOG2DEV(y);
	
		::MoveTo(xx1,yy1);
		::LineTo(xx1+1, yy1+1);
  };
}

void  wxDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) 
  	return;

  MacInstallPen() ;
  
  int offset = (m_pen.GetWidth() - 1 ) / 2 ;	
  long x1, x2 , y1 , y2 ;
  x1 = XLOG2DEV(points[0].x + xoffset);
  y1 = YLOG2DEV(points[0].y + yoffset);   
  ::MoveTo(x1 - offset ,y1 - offset );
  
  for (int i = 0; i < n-1; i++)
  {
    x2 = XLOG2DEV(points[i+1].x + xoffset);
    y2 = YLOG2DEV(points[i+1].y + yoffset);
    ::LineTo(x2 - offset  , y2 - offset );
  }
}

void  wxDC::DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle )
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
  PolyHandle polygon = OpenPoly() ;
  long x1, x2 , y1 , y2 ;
  x1 = XLOG2DEV(points[0].x + xoffset);
  y1 = YLOG2DEV(points[0].y + yoffset);   
  ::MoveTo(x1,y1);
  
  for (int i = 0; i < n-1; i++)
  {
    x2 = XLOG2DEV(points[i+1].x + xoffset);
    y2 = YLOG2DEV(points[i+1].y + yoffset);
    ::LineTo(x2, y2);
  }

  ClosePoly() ;
	if (m_brush.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallBrush() ;
		::PaintPoly( polygon ) ;
	};
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FramePoly( polygon ) ;
	};
  KillPoly( polygon ) ;
}

void  wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
	
	long xx = XLOG2DEV(x);
	long yy = YLOG2DEV(y);
	long ww = m_signX * XLOG2DEVREL(width);
	long hh = m_signY * YLOG2DEVREL(height);
	
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
	};
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FrameRect( &rect ) ;
	};
}

void  wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
	
  if (radius < 0.0) 
  	radius = - radius * ((width < height) ? width : height);
	
	long xx = XLOG2DEV(x);
	long yy = YLOG2DEV(y);
	long ww = m_signX * XLOG2DEVREL(width);
	long hh = m_signY * YLOG2DEVREL(height);
	
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
	};
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FrameRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
	};
}

void  wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;

	long xx = XLOG2DEV(x);
	long yy = YLOG2DEV(y);
	long ww = m_signX * XLOG2DEVREL(width);
	long hh = m_signY * YLOG2DEVREL(height);

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
	};

	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FrameOval( &rect ) ;
	};
}


    
bool  wxDC::CanDrawBitmap(void) const 
{
	return true ;
}


bool  wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool useMask )
{
  if (!Ok()) return FALSE;
  MacVerifySetup() ;

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
		srcrect.top = source->YLOG2DEV(ysrc) ;
		srcrect.left = source->XLOG2DEV(xsrc) ;
		srcrect.right = source->XLOG2DEV(xsrc + width ) ;
		srcrect.bottom = source->YLOG2DEV(ysrc + height) ;
		dstrect.top = YLOG2DEV(ydest) ;
		dstrect.left = XLOG2DEV(xdest) ;
		dstrect.bottom = YLOG2DEV(ydest + height )  ;
		dstrect.right = XLOG2DEV(xdest + width ) ;

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
			wxASSERT( mode == srcCopy ) ;
			if ( LockPixels( GetGWorldPixMap( source->m_macMask ) ) )
			{
				CopyMask( GetPortBitMapForCopyBits( sourcePort ) , GetPortBitMapForCopyBits( source->m_macMask ) , 
				GetPortBitMapForCopyBits( m_macPort ) ,
					&srcrect, &srcrect , &dstrect ) ;
				UnlockPixels( GetGWorldPixMap( source->m_macMask )  ) ;
			}
		}
		else
		{
			CopyBits( GetPortBitMapForCopyBits( sourcePort ) , GetPortBitMapForCopyBits( m_macPort ) ,
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
}
void  wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;

	long xx = XLOG2DEV(x);
	long yy = YLOG2DEV(y);
  
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

bool  wxDC::CanGetTextExtent(void) const 
{
	if ( !Ok() )
		return false  ;
		
	return true ;
}

void  wxDC::DoGetTextExtent( const wxString &string, wxCoord *width, wxCoord *height,
                     wxCoord *descent, wxCoord *externalLeading ,
                     wxFont *theFont ) const
{
  if (!Ok()) 
  	return;
 
   MacVerifySetup() ;

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
  if (!Ok()) 
  	return 1;
 
   MacVerifySetup() ;

	MacInstallFont() ;

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	return YDEV2LOGREL((fi.descent + fi.ascent) / 2) ;
}

wxCoord   wxDC::GetCharHeight(void) const
{
  if (!Ok()) 
  	return 1;
 
   MacVerifySetup() ;

	MacInstallFont() ;

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	return YDEV2LOGREL( fi.descent + fi.ascent );
}

void  wxDC::Clear(void)
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
	Rect rect = { -32767 , -32767 , 32767 , 32767 } ;
	
	if (m_backgroundBrush.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallBrush() ;
		::EraseRect( &rect ) ;
	};
}

void wxDC::MacInstallFont() const
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;

	if ( m_macFontInstalled )
		return ;
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
  if (!Ok()) 
  	return;
  MacVerifySetup() ;

	Pattern	 blackColor;

	if ( m_macPenInstalled )
		return ;

	RGBColor forecolor = m_pen.GetColour().GetPixel();
	RGBColor backcolor = m_backgroundBrush.GetColour().GetPixel();
	::RGBForeColor( &forecolor );
	::RGBBackColor( &backcolor );
	
	::PenNormal() ;
	int penWidth = m_pen.GetWidth() * m_scaleX ;

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

void wxDC::MacInstallBrush() const
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
	Pattern	 blackColor, whiteColor ;
	if ( m_macBrushInstalled )
		return ;

	// foreground

	RGBColor forecolor = m_brush.GetColour().GetPixel();
	RGBColor backcolor = m_backgroundBrush.GetColour().GetPixel();
	::RGBForeColor( &forecolor );
	::RGBBackColor( &backcolor );

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
	
	brushStyle = m_backgroundBrush.GetStyle();
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
