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

wxDC::wxDC(void)
{
  m_ok = FALSE;
  m_optimize = FALSE;
  m_autoSetting = FALSE;
  m_colour = TRUE;
  m_clipping = FALSE;
  
  m_mm_to_pix_x = 1.0;
  m_mm_to_pix_y = 1.0;
  
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
	if ( m_macPort )
	{
		::SetOrigin( 0 , 0 ) ;
		::ClipRect( &m_macPort->portRect ) ;
		::PenNormal() ;
		::SetPort( m_macOrigPort ) ;
	}
	++m_macCurrentPortId ;
};

void wxDC::MacSetupPort() const
{
	m_macPortId = ++m_macCurrentPortId ;
	::SetPort(m_macPort);
	::SetOrigin(-m_macLocalOrigin.h, -m_macLocalOrigin.v);
	::ClipRect(&m_macClipRect);

	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;	
}

void wxDC::DrawBitmap( const wxBitmap &bmp, long x, long y, bool useMask )
{
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
    		Rect bitmaprect = { 0 , 0 , bmap->m_height , bmap->m_width } ;
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
			//		RGBForeColor( &m_textForegroundColour.GetPixel() ) ;
			//		RGBBackColor( &m_textBackgroundColour.GetPixel() ) ;
			
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
						dest.bottom = YLOG2DEV(y + bmap->m_height )  ;
						dest.right = XLOG2DEV(x + bmap->m_width ) ;
						// ::ClipRect(&m_macClipRect);
						CopyBits( &GrafPtr( bmapworld )->portBits , &GrafPtr( m_macPort )->portBits ,
							&source, &dest, srcCopy, NULL ) ;
						/*
						if ( m_clipping )
						{
							long x1 = XLOG2DEV(m_clipX1);
							long y1 = YLOG2DEV(m_clipY1);
							long x2 = XLOG2DEV(m_clipX2);
							long y2 = YLOG2DEV(m_clipY2);
	
							Rect clip = { y1 , x1 , y2 , x2 } ;
							::ClipRect(&clip);
						}
						*/
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

void wxDC::DrawIcon( const wxIcon &icon, long x, long y, bool useMask )
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

void wxDC::DrawPoint( wxPoint& point ) 
{ 
  DrawPoint( point.x, point.y ); 
};

void wxDC::DrawPolygon( wxList *list, long xoffset, long yoffset, int fillStyle )
{
  int n = list->Number();
  wxPoint *points = new wxPoint[n];

  int i = 0;
  for( wxNode *node = list->First(); node; node = node->Next() )
  {
    wxPoint *point = (wxPoint *)node->Data();
    points[i].x = point->x;
    points[i++].y = point->y;
  };
  DrawPolygon( n, points, xoffset, yoffset, fillStyle );
  delete[] points;
};

void wxDC::DrawLines( wxList *list, long xoffset, long yoffset )
{
  int n = list->Number();
  wxPoint *points = new wxPoint[n];

  int i = 0;
  for( wxNode *node = list->First(); node; node = node->Next() ) 
  {
    wxPoint *point = (wxPoint *)node->Data();
    points[i].x = point->x;
    points[i++].y = point->y;
  };
  DrawLines( n, points, xoffset, yoffset );
  delete []points;
};

void wxDC::DrawSpline( long x1, long y1, long x2, long y2, long x3, long y3 )
{
  wxList list;
  list.Append( (wxObject*)new wxPoint(x1, y1) );
  list.Append( (wxObject*)new wxPoint(x2, y2) );
  list.Append( (wxObject*)new wxPoint(x3, y3) );
  DrawSpline(&list);
  wxNode *node = list.First();
  while (node)
  {
    wxPoint *p = (wxPoint*)node->Data();
    delete p;
    node = node->Next();
  };
};

void wxDC::DrawSpline( int n, wxPoint points[] )
{
  wxList list;
  for (int i = 0; i < n; i++) list.Append( (wxObject*)&points[i] );
  DrawSpline( &list );
};

void wxDC::SetClippingRegion( long x, long y, long width, long height )
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

void wxDC::DestroyClippingRegion(void)
{
  MacVerifySetup() ;
  m_clipping = FALSE;
//	Rect clip = { -32000 , -32000 , 32000 , 32000 } ;
	::ClipRect(&m_macClipRect);
};

void wxDC::GetClippingBox( long *x, long *y, long *width, long *height ) const
{
  if (m_clipping)
  {
    if (x) *x = m_clipX1;
    if (y) *y = m_clipY1;
    if (width) *width = (m_clipX2 - m_clipX1);
    if (height) *height = (m_clipY2 - m_clipY1);
  }
  else
   *x = *y = *width = *height = 0;
};

void wxDC::GetSize( int* width, int* height ) const
{
  *width = m_maxX-m_minX;
  *height = m_maxY-m_minY;
};

void wxDC::GetSizeMM( long* width, long* height ) const
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

void wxDC::GetUserScale( double *x, double *y )
{
  if (x) *x = m_userScaleX;
  if (y) *y = m_userScaleY;
};

void wxDC::SetLogicalScale( double x, double y )
{
  // allow negative ?
  m_logicalScaleX = x;
  m_logicalScaleY = y;
  ComputeScaleAndOrigin();
};

void wxDC::GetLogicalScale( double *x, double *y )
{
  if (x) *x = m_logicalScaleX;
  if (y) *y = m_logicalScaleY;
};

void wxDC::SetLogicalOrigin( long x, long y )
{
  m_logicalOriginX = x * m_signX;   // is this still correct ?
  m_logicalOriginY = y * m_signY;
  ComputeScaleAndOrigin();
};

void wxDC::GetLogicalOrigin( long *x, long *y )
{
  if (x) *x = m_logicalOriginX;
  if (y) *y = m_logicalOriginY;
};

void wxDC::SetDeviceOrigin( long x, long y )
{
  m_externalDeviceOriginX = x;
  m_externalDeviceOriginY = y;
  ComputeScaleAndOrigin();
};

void wxDC::GetDeviceOrigin( long *x, long *y )
{
//  if (x) *x = m_externalDeviceOriginX;
//  if (y) *y = m_externalDeviceOriginY;
  if (x) *x = m_deviceOriginX;
  if (y) *y = m_deviceOriginY;
};

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

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
};

long wxDC::DeviceToLogicalX(long x) const
{
  return XDEV2LOG(x);
};

long wxDC::DeviceToLogicalY(long y) const
{
  return YDEV2LOG(y);
};

long wxDC::DeviceToLogicalXRel(long x) const
{
  return XDEV2LOGREL(x);
};

long wxDC::DeviceToLogicalYRel(long y) const
{
  return YDEV2LOGREL(y);
};

long wxDC::LogicalToDeviceX(long x) const
{
  return XLOG2DEV(x);
};

long wxDC::LogicalToDeviceY(long y) const
{
  return YLOG2DEV(y);
};

long wxDC::LogicalToDeviceXRel(long x) const
{
  return XLOG2DEVREL(x);
};

long wxDC::LogicalToDeviceYRel(long y) const
{
  return YLOG2DEVREL(y);
};
    
void wxDC::CalcBoundingBox( long x, long y )
{
  if (x < m_minX) m_minX = x;
  if (y < m_minY) m_minY = y;
  if (x > m_maxX) m_maxX = x;
  if (y > m_maxY) m_maxY = y;
};

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

void  wxDC::FloodFill( long x1, long y1, const wxColour& col, int style )
{
}

bool  wxDC::GetPixel( long x1, long y1, wxColour *col ) const 
{
	return true ;
}

void  wxDC::DrawLine( long x1, long y1, long x2, long y2 )
{
  if (!Ok()) 
  	return;
  	
  MacVerifySetup() ;
  
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
		MacInstallPen() ;
		int offset = (m_pen.GetWidth()	- 1) / 2 ;	
  	long xx1 = XLOG2DEV(x1); 
  	long yy1 = YLOG2DEV(y1);
  	long xx2 = XLOG2DEV(x2); 
  	long yy2 = YLOG2DEV(y2);
	
		::MoveTo(xx1 - offset ,yy1 - offset);
		::LineTo(xx2 - offset , yy2 - offset );
  };
}

void  wxDC::CrossHair( long x, long y )
{
}

void  wxDC::DrawArc( long x1, long y1, long x2, long y2, long xc, long yc )
{
}

void  wxDC::DrawEllipticArc( long x, long y, long width, long height, double sa, double ea )
{
}

void  wxDC::DrawPoint( long x, long y )
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

void  wxDC::DrawLines( int n, wxPoint points[], long xoffset , long yoffset  )
{
  if (!Ok()) 
  	return;
  MacVerifySetup() ;
  
  if (m_pen.GetStyle() == wxTRANSPARENT) 
  	return;

	MacInstallPen() ;
  
	int offset = (m_pen.GetWidth()	- 1 ) / 2 ;	
  long x1, x2 , y1 , y2 ;
  x1 = XLOG2DEV(points[0].x + xoffset);
	y1 = YLOG2DEV(points[0].y + yoffset);   
	::MoveTo(x1 - offset ,y1 - offset );
  
  for (int i = 0; i < n-1; i++)
  {
    long x2 = XLOG2DEV(points[i+1].x + xoffset);
    long y2 = YLOG2DEV(points[i+1].y + yoffset);
		::LineTo(x2 - offset  , y2 - offset );
  }
}

void  wxDC::DrawPolygon( int n, wxPoint points[], long xoffset , long yoffset , 
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
    long x2 = XLOG2DEV(points[i+1].x + xoffset);
    long y2 = YLOG2DEV(points[i+1].y + yoffset);
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

void  wxDC::DrawRectangle( long x, long y, long width, long height )
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

void  wxDC::DrawRoundedRectangle( long x, long y, long width, long height, double radius  )
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
		::PaintRoundRect( &rect , radius * 2 , radius * 2 ) ;
	};
	
	if (m_pen.GetStyle() != wxTRANSPARENT) 
	{
		MacInstallPen() ;
		::FrameRoundRect( &rect , radius * 2 , radius * 2 ) ;
	};
}

void  wxDC::DrawEllipse( long x, long y, long width, long height )
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

// ----------------------------------- spline code ----------------------------------------

static void wx_quadratic_spline(double a1, double b1, double a2, double b2,
                         double a3, double b3, double a4, double b4);
static void wx_clear_stack(void);
static int wx_spline_pop(double *x1, double *y1, double *x2, double *y2, double *x3,
        double *y3, double *x4, double *y4);
static void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3,
          double x4, double y4);
static bool wx_spline_add_point(double x, double y);
static void wx_spline_draw_point_array(wxDC *dc);

static wxList wx_spline_point_list;

#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

/* iterative version */

static void wx_quadratic_spline(double a1, double b1, double a2, double b2, double a3, double b3, double a4,
                 double b4)
{
    register double  xmid, ymid;
    double           x1, y1, x2, y2, x3, y3, x4, y4;

    wx_clear_stack();
    wx_spline_push(a1, b1, a2, b2, a3, b3, a4, b4);

    while (wx_spline_pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
        xmid = (double)half(x2, x3);
        ymid = (double)half(y2, y3);
	if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD &&
	    fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
            wx_spline_add_point( x1, y1 );
            wx_spline_add_point( xmid, ymid );
	} else {
            wx_spline_push(xmid, ymid, (double)half(xmid, x3), (double)half(ymid, y3),
                 (double)half(x3, x4), (double)half(y3, y4), x4, y4);
            wx_spline_push(x1, y1, (double)half(x1, x2), (double)half(y1, y2),
                 (double)half(x2, xmid), (double)half(y2, ymid), xmid, ymid);
	}
    }
}

/* utilities used by spline drawing routines */

typedef struct wx_spline_stack_struct {
    double           x1, y1, x2, y2, x3, y3, x4, y4;
} Stack;

#define         SPLINE_STACK_DEPTH             20
static Stack    wx_spline_stack[SPLINE_STACK_DEPTH];
static Stack   *wx_stack_top;
static int      wx_stack_count;

static void wx_clear_stack(void)
{
    wx_stack_top = wx_spline_stack;
    wx_stack_count = 0;
}

static void wx_spline_push(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    wx_stack_top->x1 = x1;
    wx_stack_top->y1 = y1;
    wx_stack_top->x2 = x2;
    wx_stack_top->y2 = y2;
    wx_stack_top->x3 = x3;
    wx_stack_top->y3 = y3;
    wx_stack_top->x4 = x4;
    wx_stack_top->y4 = y4;
    wx_stack_top++;
    wx_stack_count++;
}

static int wx_spline_pop(double *x1, double *y1, double *x2, double *y2,
                  double *x3, double *y3, double *x4, double *y4)
{
    if (wx_stack_count == 0)
	return (0);
    wx_stack_top--;
    wx_stack_count--;
    *x1 = wx_stack_top->x1;
    *y1 = wx_stack_top->y1;
    *x2 = wx_stack_top->x2;
    *y2 = wx_stack_top->y2;
    *x3 = wx_stack_top->x3;
    *y3 = wx_stack_top->y3;
    *x4 = wx_stack_top->x4;
    *y4 = wx_stack_top->y4;
    return (1);
}

static bool wx_spline_add_point(double x, double y)
{
  wxPoint *point = new wxPoint ;
  point->x = (int) x;
  point->y = (int) y;
  wx_spline_point_list.Append((wxObject*)point);
  return TRUE;
}

static void wx_spline_draw_point_array(wxDC *dc)
{
  dc->DrawLines(&wx_spline_point_list, 0, 0 );
  wxNode *node = wx_spline_point_list.First();
  while (node)
  {
    wxPoint *point = (wxPoint *)node->Data();
    delete point;
    delete node;
    node = wx_spline_point_list.First();
  }
}

void  wxDC::DrawSpline( wxList *points )
{
    wxPoint *p;
    double           cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double           x1, y1, x2, y2;

    wxNode *node = points->First();
    p = (wxPoint *)node->Data();

    x1 = p->x;
    y1 = p->y;

    node = node->Next();
    p = (wxPoint *)node->Data();

    x2 = p->x;
    y2 = p->y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    wx_spline_add_point(x1, y1);

    while ((node = node->Next()) != NULL)
    {
        p = (wxPoint *)node->Data();
	x1 = x2;
	y1 = y2;
	x2 = p->x;
	y2 = p->y;
        cx4 = (double)(x1 + x2) / 2;
        cy4 = (double)(y1 + y2) / 2;
        cx3 = (double)(x1 + cx4) / 2;
        cy3 = (double)(y1 + cy4) / 2;

        wx_quadratic_spline(cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4);

	cx1 = cx4;
	cy1 = cy4;
        cx2 = (double)(cx1 + x2) / 2;
        cy2 = (double)(cy1 + y2) / 2;
    }

    wx_spline_add_point( cx1, cy1 );
    wx_spline_add_point( x2, y2 );

    wx_spline_draw_point_array( this );
}


    
bool  wxDC::CanDrawBitmap(void) const 
{
	return true ;
}


bool  wxDC::Blit( long xdest, long ydest, long width, long height,
       wxDC *source, long xsrc, long ysrc, int logical_func , bool useMask )
{
  if (!Ok()) return FALSE;
  MacVerifySetup() ;

	CGrafPtr			sourcePort = (CGrafPtr) source->m_macPort ;
	PixMapHandle	bmappixels =  GetGWorldPixMap( sourcePort ) ; 
	RGBColor		white = { 0xFFFF, 0xFFFF,0xFFFF} ;
	RGBColor		black = { 0,0,0} ;
//		RGBForeColor( &black ) ;
//		RGBBackColor( &white ) ;
		RGBForeColor( &m_textForegroundColour.GetPixel() ) ;
		RGBBackColor( &m_textBackgroundColour.GetPixel() ) ;

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
//		::ClipRect(&m_macClipRect);
		CopyBits( &GrafPtr( sourcePort )->portBits , &GrafPtr( m_macPort )->portBits ,
			&srcrect, &dstrect, srcCopy, NULL ) ;
/*
						if ( m_clipping )
						{
							long x1 = XLOG2DEV(m_clipX1);
							long y1 = YLOG2DEV(m_clipY1);
							long x2 = XLOG2DEV(m_clipX2);
							long y2 = YLOG2DEV(m_clipY2);
	
							Rect clip = { y1 , x1 , y2 , x2 } ;
							::ClipRect(&clip);
						}
*/
		UnlockPixels( bmappixels ) ;
	} 
	
	m_macPenInstalled = false ;
	m_macBrushInstalled = false ;
	m_macFontInstalled = false ;
  	
  return TRUE;
}

void  wxDC::DrawText( const wxString &string, long x, long y, bool use16)
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

void  wxDC::GetTextExtent( const wxString &string, long *width, long *height,
                     long *descent, long *externalLeading ,
                     wxFont *theFont , bool use16  ) const
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
	  	long yy1 = YLOG2DEV(0);
	 		long yy2 = YLOG2DEV(font->m_macFontSize);
	
			::TextFont( font->m_macFontNum ) ;
			::TextSize( abs( yy2-yy1) ) ;
			::TextFace( font->m_macFontStyle ) ;
		}
	}
	else
	{
		MacInstallFont() ;
	}

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	*height = fi.descent + fi.ascent ;
	*descent = fi.descent ;
	*externalLeading = fi.leading ;
	
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
	*width = 0 ;
	
	while( i < length )
	{
		if( text[i] == 13 || text[i] == 10)
		{
			*height += fi.descent + fi.ascent + fi.leading;
			curwidth = ::TextWidth( text , laststop , i - laststop ) ;
			if ( curwidth > *width )
				*width = curwidth ;
			laststop = i+1 ;
		}
		i++ ;
	}
			
	curwidth = ::TextWidth( text , laststop , i - laststop ) ;
	if ( curwidth > *width )
		*width = curwidth ;

	if ( theFont )
	{
		m_macFontInstalled = false ;
	}
}

long   wxDC::GetCharWidth(void)
{
  if (!Ok()) 
  	return 1;
 
   MacVerifySetup() ;

	MacInstallFont() ;

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	return (fi.descent + fi.ascent) / 2 ;
}

long   wxDC::GetCharHeight(void)
{
  if (!Ok()) 
  	return 1;
 
   MacVerifySetup() ;

	MacInstallFont() ;

	FontInfo fi ;
	::GetFontInfo( &fi ) ;

	return fi.descent + fi.ascent ;
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
		
	wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;

	if ( font )
	{
		::TextFont( font->m_macFontNum ) ;
		::TextSize( m_scaleY * font->m_macFontSize ) ;
		::TextFace( font->m_macFontStyle ) ;
	
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;
	
		::RGBForeColor(&m_textForegroundColour.GetPixel() );
		::RGBBackColor(&m_textBackgroundColour.GetPixel() );
	}
	else
	{
		short fontnum ;
		
		GetFNum( "\pGeneva" , &fontnum ) ;
		::TextFont( fontnum ) ;
		::TextSize( m_scaleY * 10 ) ;
		::TextFace( 0 ) ;
	
		// todo reset after spacing changes - or store the current spacing somewhere
	
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;
		::RGBForeColor( &(m_textForegroundColour.GetPixel()) );
		::RGBBackColor(&m_textBackgroundColour.GetPixel() );
	}


	short mode = patCopy ;

	// todo :
	
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(&qd.black);
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
		case wxSRC_OR:     // source _bitmap_ OR destination
		case wxSRC_AND:     // source _bitmap_ AND destination
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

	if ( m_macPenInstalled )
		return ;

	::RGBForeColor(&m_pen.GetColour().GetPixel() );
	::RGBBackColor(&m_backgroundBrush.GetColour().GetPixel() );
	
	::PenNormal() ;
	int penWidth = m_pen.GetWidth();
	::PenSize(penWidth, penWidth);

	int penStyle = m_pen.GetStyle();
	
	if (penStyle == wxSOLID)
		::PenPat(&qd.black);
	else if (IS_HATCH(penStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(penStyle, &pat);
		::PenPat(&pat);
	}
	else
	{
		::PenPat(&qd.black);
	}

	short mode = patCopy ;
	
	// todo :
	
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(&qd.black);
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
		case wxSRC_OR:     // source _bitmap_ OR destination
		case wxSRC_AND:     // source _bitmap_ AND destination
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

	if ( m_macBrushInstalled )
		return ;

	// foreground

	::RGBForeColor(&m_brush.GetColour().GetPixel() );
	::RGBBackColor(&m_backgroundBrush.GetColour().GetPixel() );

	int brushStyle = m_brush.GetStyle();
	if (brushStyle == wxSOLID)
		::PenPat(&qd.black);
	else if (IS_HATCH(brushStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(brushStyle, &pat);
		::PenPat(&pat);
	}
	else
	{
		::PenPat(&qd.black);
	}

	
	// background
	
	brushStyle = m_backgroundBrush.GetStyle();
	if (brushStyle == wxSOLID)
		::BackPat(&qd.white);
	else if (IS_HATCH(brushStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(brushStyle, &pat);
		::BackPat(&pat);
	}
	else
	{
		::BackPat(&qd.white);
	}
	
	short mode = patCopy ;

	// todo :
	
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(&qd.black);
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
		case wxSRC_OR:     // source _bitmap_ OR destination
		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
	m_macBrushInstalled = true ;
	m_macPenInstalled = false ;
	m_macFontInstalled = false ;
}


