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
#include "wx/bitmap.h"
#include "wx/log.h"

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

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC(void)
{
  m_ok = FALSE;
  m_colour = TRUE;
  m_clipping = FALSE;
  
  m_logicalOriginX = 0;
  m_logicalOriginY = 0;
  m_deviceOriginX = 0;
  m_deviceOriginY = 0;
  
  m_logicalScaleX = 1.0;
  m_logicalScaleY = 1.0;
  m_userScaleX = 1.0;
  m_userScaleY = 1.0;
  m_scaleX = 1.0;
  m_scaleY = 1.0;
  
  m_mappingMode = wxMM_TEXT;
  
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
};

wxDC::~wxDC(void)
{
};

void wxDC::DoDrawIcon( const wxIcon &WXUNUSED(icon), int WXUNUSED(x), int WXUNUSED(y) )
{
};

void wxDC::DoDrawPoint( int x, int y ) 
{ 
};

void wxDC::DoDrawPolygon( int, wxPoint *, int, int, int)
{
};

void wxDC::DoDrawLines( int, wxPoint *, int, int )
{
}

void wxDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
}

int wxDC::GetDepth() const
{
    return 0;
}

wxSize wxDC::GetPPI() const
{
    return wxSize(0,0);
}

bool wxDC::CanGetTextExtent() const
{
    return false;
}

void wxDC::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y, wxCoord *descent, wxCoord *externalLeading, wxFont *theFont) const
{
    wxLogDebug("wxDC::DoGetTextExtent(%s)",string.c_str());
    if(x)
        *x=0;
    if(y)
        *y=0;
    if(descent)
        *descent=0;
    if(externalLeading)
        *externalLeading=0;
}

wxCoord wxDC::GetCharHeight() const
{
    return 0;
}

wxCoord wxDC::GetCharWidth() const
{
    return 0;
}

bool wxDC::CanDrawBitmap() const
{
    return false;
}

bool wxDC::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    return false;
}

void wxDC::DoDrawArc(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc)
{
}
    
void wxDC::SetPen(const wxPen& pen)
{
}

void wxDC::SetBrush(const wxBrush& brush)
{
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
}

void wxDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DestroyClippingRegion()
{
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
}

void wxDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
}

void wxDC::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h, double sa, double ea)
{
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxDC::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask)
{
}

bool wxDC::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, int style)
{
    return false;
}

void wxDC::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
}


bool wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height, wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask , wxCoord xsrcMask, wxCoord ysrcMask)
{
    return false;
}

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
};

void wxDC::SetTextForeground( const wxColour &col )
{
  if (!Ok()) return;
  m_textForegroundColour = col;
};

void wxDC::SetTextBackground( const wxColour &col )
{
  if (!Ok()) return;
  m_textBackgroundColour = col;
};

void wxDC::Clear()
{
}

void wxDC::SetBackground(const wxBrush&)
{
}

void wxDC::SetPalette(const wxPalette&)
{
}

void wxDC::SetLogicalFunction(int)
{
}


void wxDC::SetMapMode( int mode )
{
  switch (mode) 
  {
    case wxMM_TWIPS:
      break;
    case wxMM_POINTS:
      break;
    case wxMM_METRIC:
      break;
    case wxMM_LOMETRIC:
      break;
    default:
    case wxMM_TEXT:
      SetLogicalScale( 1.0, 1.0 );
      break;
  };
  if (mode != wxMM_TEXT)
  {
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
  ComputeScaleAndOrigin();
};

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
};

void wxDC::ComputeScaleAndOrigin(void)
{
  // CMB: copy scale to see if it changes
  double origScaleX = m_scaleX;
  double origScaleY = m_scaleY;

  m_scaleX = m_logicalScaleX * m_userScaleX;
  m_scaleY = m_logicalScaleY * m_userScaleY;

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

int wxDCBase::DeviceToLogicalX(int x) const
{
    return x;
}

int wxDCBase::DeviceToLogicalY(int y) const
{
    return y;
}

// vim:sts=4:sw=4:et
