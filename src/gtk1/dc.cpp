/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "dc.h"
#endif

#include "wx/dc.h"

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

IMPLEMENT_ABSTRACT_CLASS(wxDC,wxObject)

wxDC::wxDC()
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
  
    m_logicalScaleX = 1.0;
    m_logicalScaleY = 1.0;
    m_userScaleX = 1.0;
    m_userScaleY = 1.0;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
  
    m_mappingMode = MM_TEXT;
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
}

wxDC::~wxDC()
{
}

bool wxDC::Ok() const 
{ 
    return m_ok; 
}

void wxDC::DrawArc( long WXUNUSED(x1), long WXUNUSED(y1), long WXUNUSED(x2), long WXUNUSED(y2), 
  double WXUNUSED(xc), double WXUNUSED(yc) )
{
}

void wxDC::DrawPoint( wxPoint& point ) 
{ 
    DrawPoint( point.x, point.y ); 
}

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
    }
    
    DrawPolygon( n, points, xoffset, yoffset, fillStyle );
    delete[] points;
}

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
    }
    
    DrawLines( n, points, xoffset, yoffset );
    delete []points;
}

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
    }
}

void wxDC::DrawSpline( int n, wxPoint points[] )
{
    wxList list;
    for (int i = 0; i < n; i++) list.Append( (wxObject*)&points[i] );
    DrawSpline( &list );
}

void wxDC::SetClippingRegion( long x, long y, long width, long height )
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
    {
        *x = *y = *width = *height = 0;
    }
}

void wxDC::GetSize( int* width, int* height ) const
{
    if (width) *width = m_maxX-m_minX;
    if (height) *height = m_maxY-m_minY;
}

void wxDC::GetSizeMM( long* width, long* height ) const
{
    int w = 0;
    int h = 0;
    GetSize( &w, &h );
    if (width) *width = long( double(w) / (m_scaleX*m_mm_to_pix_x) );
    if (height) *height = long( double(h) / (m_scaleY*m_mm_to_pix_y) );
}

void wxDC::SetTextForeground( const wxColour &col )
{
    m_textForegroundColour = col;
}

void wxDC::SetTextBackground( const wxColour &col )
{
    m_textBackgroundColour = col;
}

void wxDC::SetMapMode( int mode )
{
    switch (mode) 
    {
        case MM_TWIPS:
          SetLogicalScale( twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y );
          break;
        case MM_POINTS:
          SetLogicalScale( pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y );
          break;
        case MM_METRIC:
          SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
          break;
        case MM_LOMETRIC:
          SetLogicalScale( m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0 );
          break;
        default:
        case MM_TEXT:
          SetLogicalScale( 1.0, 1.0 );
          break;
    }
/*  we don't do this mega optimisation
    if (mode != MM_TEXT)
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

void wxDC::GetUserScale( double *x, double *y )
{
    if (x) *x = m_userScaleX;
    if (y) *y = m_userScaleY;
}

void wxDC::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDC::GetLogicalScale( double *x, double *y )
{
    if (x) *x = m_logicalScaleX;
    if (y) *y = m_logicalScaleY;
}

void wxDC::SetLogicalOrigin( long x, long y )
{
    m_logicalOriginX = x * m_signX;   // is this still correct ?
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxDC::GetLogicalOrigin( long *x, long *y )
{
    if (x) *x = m_logicalOriginX;
    if (y) *y = m_logicalOriginY;
}

void wxDC::SetDeviceOrigin( long x, long y )
{
    // only wxPostScripDC has m_signX = -1, we override SetDeviceOrigin there
    m_deviceOriginX = x;  
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDC::GetDeviceOrigin( long *x, long *y )
{
    if (x) *x = m_deviceOriginX;
    if (y) *y = m_deviceOriginY;
}

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    // only wxPostScripDC has m_signX = -1, we override SetAxisOrientation there
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}

long wxDC::DeviceToLogicalX(long x) const
{
    return XDEV2LOG(x);
}

long wxDC::DeviceToLogicalY(long y) const
{
    return YDEV2LOG(y);
}

long wxDC::DeviceToLogicalXRel(long x) const
{
    return XDEV2LOGREL(x);
}

long wxDC::DeviceToLogicalYRel(long y) const
{
    return YDEV2LOGREL(y);
}

long wxDC::LogicalToDeviceX(long x) const
{
    return XLOG2DEV(x);
}

long wxDC::LogicalToDeviceY(long y) const
{
    return YLOG2DEV(y);
}

long wxDC::LogicalToDeviceXRel(long x) const
{
    return XLOG2DEVREL(x);
}

long wxDC::LogicalToDeviceYRel(long y) const
{
    return YLOG2DEVREL(y);
}
    
void wxDC::CalcBoundingBox( long x, long y )
{
    if (x < m_minX) m_minX = x;
    if (y < m_minY) m_minY = y;
    if (x > m_maxX) m_maxX = x;
    if (y > m_maxY) m_maxY = y;
}

void wxDC::ComputeScaleAndOrigin()
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
        // It gives an Assert, Robert Roebling
/*	
      wxPen pen = m_pen;
      m_pen = wxNullPen;
      SetPen( pen );
*/
  }
}

