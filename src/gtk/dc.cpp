/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
    #pragma implementation "dc.h"
#endif

#include "wx/dc.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define mm2inches        0.0393700787402
#define inches2mm        25.4
#define mm2twips         56.6929133859
#define twips2mm         0.0176388888889
#define mm2pt            2.83464566929
#define pt2mm            0.352777777778

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxDCBase)

wxDC::wxDC()
{
    m_ok = FALSE;

    m_mm_to_pix_x = 1.0;
    m_mm_to_pix_y = 1.0;

    m_needComputeScaleX = FALSE; /* not used yet */
    m_needComputeScaleY = FALSE; /* not used yet */

    m_logicalFunction = wxCOPY;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxTRANSPARENT_BRUSH;
}

void wxDC::DoSetClippingRegion( long x, long y, long width, long height )
{
    m_clipping = TRUE;
    m_clipX1 = x;
    m_clipY1 = y;
    m_clipX2 = x + width;
    m_clipY2 = y + height;
}

void wxDC::DestroyClippingRegion()
{
    m_clipping = FALSE;
}

// ---------------------------------------------------------------------------
// get DC capabilities
// ---------------------------------------------------------------------------

void wxDC::DoGetSize( int* width, int* height ) const
{
    if (width) *width = m_maxX-m_minX;
    if (height) *height = m_maxY-m_minY;
}

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0;
    int h = 0;
    GetSize( &w, &h );
    if (width) *width = int( double(w) / (m_scaleX*m_mm_to_pix_x) );
    if (height) *height = int( double(h) / (m_scaleY*m_mm_to_pix_y) );
}

// Resolution in pixels per logical inch
wxSize wxDC::GetPPI() const
{
    // TODO (should probably be pure virtual)
    return wxSize(0, 0);
}

// ---------------------------------------------------------------------------
// set various DC parameters
// ---------------------------------------------------------------------------

void wxDC::ComputeScaleAndOrigin()
{
    /* CMB: copy scale to see if it changes */
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;

    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;

    /* CMB: if scale has changed call SetPen to recalulate the line width */
    if (m_scaleX != origScaleX || m_scaleY != origScaleY)
    {
      /* this is a bit artificial, but we need to force wxDC to think
         the pen has changed */
      wxPen pen = m_pen;
      m_pen = wxNullPen;
      SetPen( pen );
  }
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
    m_mappingMode = mode;
    
/*  we don't do this mega optimisation
    if (mode != wxMM_TEXT)
    {
        m_needComputeScaleX = TRUE;
        m_needComputeScaleY = TRUE;
    }
*/
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

void wxDC::SetLogicalOrigin( long x, long y )
{
    m_logicalOriginX = x * m_signX;   // is this still correct ?
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxDC::SetDeviceOrigin( long x, long y )
{
    // only wxPostScripDC has m_signX = -1, we override SetDeviceOrigin there
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // only wxPostScripDC has m_signX = -1, we override SetAxisOrientation there
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}

// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

long wxDCBase::DeviceToLogicalX(long x) const
{
    return ((wxDC *)this)->XDEV2LOG(x);
}

long wxDCBase::DeviceToLogicalY(long y) const
{
    return ((wxDC *)this)->YDEV2LOG(y);
}

long wxDCBase::DeviceToLogicalXRel(long x) const
{
    return ((wxDC *)this)->XDEV2LOGREL(x);
}

long wxDCBase::DeviceToLogicalYRel(long y) const
{
    return ((wxDC *)this)->YDEV2LOGREL(y);
}

long wxDCBase::LogicalToDeviceX(long x) const
{
    return ((wxDC *)this)->XLOG2DEV(x);
}

long wxDCBase::LogicalToDeviceY(long y) const
{
    return ((wxDC *)this)->YLOG2DEV(y);
}

long wxDCBase::LogicalToDeviceXRel(long x) const
{
    return ((wxDC *)this)->XLOG2DEVREL(x);
}

long wxDCBase::LogicalToDeviceYRel(long y) const
{
    return ((wxDC *)this)->YLOG2DEVREL(y);
}

