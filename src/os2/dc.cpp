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

//-----------------------------------------------------------------------------
// wxDCBase
//-----------------------------------------------------------------------------

long wxDCBase::DeviceToLogicalX(long x) const
{
  return XDEV2LOG(x);
};

long wxDCBase::DeviceToLogicalY(long y) const
{
  return YDEV2LOG(y);
};

long wxDCBase::DeviceToLogicalXRel(long x) const
{
  return XDEV2LOGREL(x);
};

long wxDCBase::DeviceToLogicalYRel(long y) const
{
  return YDEV2LOGREL(y);
};

long wxDCBase::LogicalToDeviceX(long x) const
{
  return XLOG2DEV(x);
};

long wxDCBase::LogicalToDeviceY(long y) const
{
  return YLOG2DEV(y);
};

long wxDCBase::LogicalToDeviceXRel(long x) const
{
  return XLOG2DEVREL(x);
};

long wxDCBase::LogicalToDeviceYRel(long y) const
{
  return YLOG2DEVREL(y);
};

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
};

wxDC::~wxDC(void)
{
};

void wxDC::DestroyClippingRegion(void)
{
  m_clipping = FALSE;
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

void wxDC::SetLogicalOrigin( long x, long y )
{
  m_logicalOriginX = x * m_signX;   // is this still correct ?
  m_logicalOriginY = y * m_signY;
  ComputeScaleAndOrigin();
};

void wxDC::SetDeviceOrigin( long x, long y )
{
  m_externalDeviceOriginX = x;
  m_externalDeviceOriginY = y;
  ComputeScaleAndOrigin();
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


int wxDC::GetDepth() const
{
   // TODO:
   return (1);
}

wxSize wxDC::GetPPI() const
{
   // TODO:
   return (1);
}
void wxDC::GetTextExtent( const wxString& string
                         ,long* x
                         ,long* y
                         ,long* decent
                         ,long* externalLeading
                         ,wxFont* theFont
                        ) const
{
   // TODO:
}

void wxDC::GetCharWidth() const
{
    // TODO
    return(1);
}

void wxDC::GetCharHeight() const
{
    // TODO
    return(1);
}

void wxDC::Clear()
{
   // TODO
}

void wxDC::SetFont(const wxFont& font)
{
   // TODO
}

void wxDC::SetPen(const wxPen& pen)
{
   // TODO
}
void wxDC::SetBrush(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetBackground(const wxBrush& brush)
{
   // TODO
}

void wxDC::SetLogicalFunction(int function)
{
   // TODO
}

void wxDC::SetBackgroundMode(int mode)
{
   // TODO
}

void wxDC::SetPalette(const wxPalette& palette)
{
   // TODO
}

void wxDC::DoFloodFill( long x
                       ,long y
                       ,const wxColour& col
                       ,int style = wxFLOOD_SURFACE
                      )
{
   // TODO
}

bool wxDC::DoGetPixel(long x, long y, wxColour *col) const
{
   // TODO
   return(TRUE);
}

void wxDC::DoDrawPoint(long x, long y)
{
   // TODO
}

void wxDC::DoDrawLine(long x1, long y1, long x2, long y2)
{
   // TODO
}

void wxDC::DoDrawArc( long x1, long y1
                     ,long x2, long y2
                     ,long xc, long yc
                    )
{
   // TODO
}

void wxDC::DoDrawEllipticArc( long x
                             ,long y
                             ,long w
                             ,long h
                             ,double sa
                             ,double ea
                            )
{
   // TODO
}

void wxDC::DoDrawRectangle(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoDrawRoundedRectangle( long x, long y
                                  ,long width, long height
                                  ,double radius
                                 )
{
   // TODO
}

void wxDC::DoDrawEllipse(long x, long y, long width, long height)
{
   // TODO
}

void wxDC::DoCrossHair(long x, long y)
{
   // TODO
}

void wxDC::DoDrawIcon(const wxIcon& icon, long x, long y)
{
   // TODO
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp
                        ,long x, long y
                        ,bool useMask = FALSE
                       )
{
   // TODO
}

void wxDC::DoDrawText(const wxString& text, long x, long y)
{
   // TODO
}

bool wxDC::DoBlit( long xdest
                  ,long ydest
                  ,long width
                  ,long height
                  ,wxDC *source
                  ,long xsrc
                  ,long ysrc
                  ,int  rop = wxCOPY
                  ,bool useMask = FALSE
                 )
{
   // TODO
   return(TRUE);
}

void wxDC::DoGetSize(int *width, int *height) const
{
   // TODO
}

void wxDC::DoGetSizeMM(int* width, int* height) const
{
   // TODO
}

void wxDC::DoDrawLines( int n, wxPoint points[]
                       ,long xoffset, long yoffset
                      )
{
   // TODO
}

void wxDC::DoDrawPolygon(int n, wxPoint points[]
                         ,long xoffset, long yoffset
                         ,int fillStyle = wxODDEVEN_RULE
                        )
{
   // TODO
}

void wxDC::DoSetClippingRegionAsRegion(const wxRegion& region)
{
   // TODO
}

void wxDC::DoSetClippingRegion( long x, long y
                               ,long width, long height
                              )
{
   // TODO
}

#if wxUSE_SPLINES
void wxDC::DoDrawSpline(wxList *points)
{
   // TODO
}
#endif

//
// Private functions
//
long wxDC::XDEV2LOG(long x) const
{
    long new_x = x - m_deviceOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
    else
        return (long)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
}

long wxDC::XDEV2LOGREL(long x) const
{
    if (x > 0)
        return (long)((double)(x) / m_scaleX + 0.5);
    else
        return (long)((double)(x) / m_scaleX - 0.5);
}

long wxDC::YDEV2LOG(long y) const
{
    long new_y = y - m_deviceOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
    else
        return (long)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
}

long wxDC::YDEV2LOGREL(long y) const
{
    if (y > 0)
        return (long)((double)(y) / m_scaleY + 0.5);
    else
        return (long)((double)(y) / m_scaleY - 0.5);
}

long wxDC::XLOG2DEV(long x) const
{
    long new_x = x - m_logicalOriginX;
    if (new_x > 0)
        return (long)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX;
    else
    return (long)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX;
}

long wxDC::XLOG2DEVREL(long x) const
{
    if (x > 0)
        return (long)((double)(x) * m_scaleX + 0.5);
    else
        return (long)((double)(x) * m_scaleX - 0.5);
}

long wxDC::YLOG2DEV(long y) const
{
    long new_y = y - m_logicalOriginY;
    if (new_y > 0)
        return (long)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY;
    else
        return (long)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY;
}

long wxDC::YLOG2DEVREL(long y) const
{
    if (y > 0)
        return (long)((double)(y) * m_scaleY + 0.5);
    else
        return (long)((double)(y) * m_scaleY - 0.5);
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

