/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/graphcmn.cpp
// Purpose:     graphics context methods common to all platforms
// Author:      Stefan Csomor
// Modified by:
// Created:     
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/graphics.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/bitmap.h"
    #include "wx/dcmemory.h"
    #include "wx/region.h"
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

static const double RAD2DEG = 180.0 / M_PI;

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

static inline double DegToRad(double deg)
{
    return (deg * M_PI) / 180.0;
}

//-----------------------------------------------------------------------------
// wxDC bridge class
//-----------------------------------------------------------------------------

#ifdef __WXMAC__
IMPLEMENT_DYNAMIC_CLASS(wxGCDC, wxDCBase)
#else
IMPLEMENT_DYNAMIC_CLASS(wxGCDC, wxDC)
#endif

wxGCDC::wxGCDC()
{
    Init();
}

void wxGCDC::SetGraphicsContext( wxGraphicsContext* ctx )
{ 
    delete m_graphicContext;
    m_graphicContext = ctx;
}

wxGCDC::wxGCDC(const wxWindowDC& dc)
{
    Init();
    m_graphicContext = wxGraphicsContext::Create(dc);
    m_ok = true;
    if ( dc.GetFont().Ok())
        m_graphicContext->SetFont( m_graphicContext->CreateFont(dc.GetFont(),dc.GetTextForeground()));
    if ( dc.GetPen().Ok())
        m_graphicContext->SetPen( m_graphicContext->CreatePen(dc.GetPen()));
    if ( dc.GetBrush().Ok())
        m_graphicContext->SetBrush( m_graphicContext->CreateBrush(dc.GetBrush()));
}

void wxGCDC::Init()
{
    m_ok = false;
    m_colour = true;
    m_mm_to_pix_x = mm2pt;
    m_mm_to_pix_y = mm2pt;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_graphicContext = NULL;
}


wxGCDC::~wxGCDC()
{
    delete m_graphicContext;
}

void wxGCDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool WXUNUSED(useMask) )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawBitmap - invalid DC") );
    wxCHECK_RET( bmp.Ok(), wxT("wxGCDC(cg)::DoDrawBitmap - invalid bitmap") );

    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord w = bmp.GetWidth();
    wxCoord h = bmp.GetHeight();
    wxCoord ww = LogicalToDeviceXRel(w);
    wxCoord hh = LogicalToDeviceYRel(h);

    m_graphicContext->DrawBitmap( bmp, xx , yy , ww , hh );
}

void wxGCDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawIcon - invalid DC") );
    wxCHECK_RET( icon.Ok(), wxT("wxGCDC(cg)::DoDrawIcon - invalid icon") );

    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord w = icon.GetWidth();
    wxCoord h = icon.GetHeight();
    wxCoord ww = LogicalToDeviceXRel(w);
    wxCoord hh = LogicalToDeviceYRel(h);

    m_graphicContext->DrawIcon( icon , xx, yy, ww, hh );
}

void wxGCDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoSetClippingRegion - invalid DC") );

    wxCoord xx, yy, ww, hh;
    xx = LogicalToDeviceX(x);
    yy = LogicalToDeviceY(y);
    ww = LogicalToDeviceXRel(width);
    hh = LogicalToDeviceYRel(height);

    m_graphicContext->Clip( xx, yy, ww, hh );
    if ( m_clipping )
    {
        m_clipX1 = wxMax( m_clipX1, xx );
        m_clipY1 = wxMax( m_clipY1, yy );
        m_clipX2 = wxMin( m_clipX2, (xx + ww) );
        m_clipY2 = wxMin( m_clipY2, (yy + hh) );
    }
    else
    {
        m_clipping = true;

        m_clipX1 = xx;
        m_clipY1 = yy;
        m_clipX2 = xx + ww;
        m_clipY2 = yy + hh;
    }
}

void wxGCDC::DoSetClippingRegionAsRegion( const wxRegion &region )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoSetClippingRegionAsRegion - invalid DC") );

    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    wxCoord x, y, w, h;
    region.GetBox( x, y, w, h );
    wxCoord xx, yy, ww, hh;
    xx = LogicalToDeviceX(x);
    yy = LogicalToDeviceY(y);
    ww = LogicalToDeviceXRel(w);
    hh = LogicalToDeviceYRel(h);

    // if we have a scaling that we cannot map onto native regions
    // we must use the box
    if ( ww != w || hh != h )
    {
        wxGCDC::DoSetClippingRegion( x, y, w, h );
    }
    else
    {
        m_graphicContext->Clip( region );
        if ( m_clipping )
        {
            m_clipX1 = wxMax( m_clipX1, xx );
            m_clipY1 = wxMax( m_clipY1, yy );
            m_clipX2 = wxMin( m_clipX2, (xx + ww) );
            m_clipY2 = wxMin( m_clipY2, (yy + hh) );
        }
        else
        {
            m_clipping = true;

            m_clipX1 = xx;
            m_clipY1 = yy;
            m_clipX2 = xx + ww;
            m_clipY2 = yy + hh;
        }
    }
}

void wxGCDC::DestroyClippingRegion()
{
    m_graphicContext->ResetClip();
    m_graphicContext->SetPen( m_pen );
    m_graphicContext->SetBrush( m_brush );

    m_clipping = false;
}

void wxGCDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0, h = 0;

    GetSize( &w, &h );
    if (width)
        *width = long( double(w) / (m_scaleX * m_mm_to_pix_x) );
    if (height)
        *height = long( double(h) / (m_scaleY * m_mm_to_pix_y) );
}

void wxGCDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::SetTextForeground - invalid DC") );

    if ( col != m_textForegroundColour )
    {
        m_textForegroundColour = col;
        m_graphicContext->SetFont( m_font, m_textForegroundColour );
    }
}

void wxGCDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::SetTextBackground - invalid DC") );

    m_textBackgroundColour = col;
}

void wxGCDC::SetMapMode( int mode )
{
    switch (mode)
    {
    case wxMM_TWIPS:
        SetLogicalScale( twips2mm * m_mm_to_pix_x, twips2mm * m_mm_to_pix_y );
        break;

    case wxMM_POINTS:
        SetLogicalScale( pt2mm * m_mm_to_pix_x, pt2mm * m_mm_to_pix_y );
        break;

    case wxMM_METRIC:
        SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
        break;

    case wxMM_LOMETRIC:
        SetLogicalScale( m_mm_to_pix_x / 10.0, m_mm_to_pix_y / 10.0 );
        break;

    case wxMM_TEXT:
    default:
        SetLogicalScale( 1.0, 1.0 );
        break;
    }

    ComputeScaleAndOrigin();
}

void wxGCDC::SetUserScale( double x, double y )
{
    // allow negative ? -> no

    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}

void wxGCDC::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxGCDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    m_logicalOriginX = x * m_signX;   // is this still correct ?
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxGCDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxGCDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp ? -1 :  1);
    ComputeScaleAndOrigin();
}

wxSize wxGCDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxGCDC::GetDepth() const
{
    return 32;
}

void wxGCDC::ComputeScaleAndOrigin()
{
    // CMB: copy scale to see if it changes
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
    m_deviceOriginX = m_deviceOriginX + m_logicalOriginX;
    m_deviceOriginY = m_deviceOriginY + m_logicalOriginY;

    // CMB: if scale has changed call SetPen to recalulate the line width
    if (m_scaleX != origScaleX || m_scaleY != origScaleY)
    {
        // this is a bit artificial, but we need to force wxDC to think
        // the pen has changed
        wxPen pen(GetPen());
        m_pen = wxNullPen;
        SetPen(pen);
        SetFont(m_font);
    }
}

void wxGCDC::SetPalette( const wxPalette& WXUNUSED(palette) )
{

}

void wxGCDC::SetBackgroundMode( int mode )
{
    m_backgroundMode = mode;
}

void wxGCDC::SetFont( const wxFont &font )
{
    m_font = font;
    if ( m_graphicContext )
    {
        wxFont f = font;
        if ( f.Ok() )
            f.SetPointSize( LogicalToDeviceYRel(font.GetPointSize()));
        m_graphicContext->SetFont( f, m_textForegroundColour );
    }
}

void wxGCDC::SetPen( const wxPen &pen )
{
    if ( m_pen == pen )
        return;

    m_pen = pen;
    if ( m_graphicContext )
    {
        if ( m_pen.GetStyle() == wxSOLID || m_pen.GetStyle() == wxTRANSPARENT )
        {
            m_graphicContext->SetPen( m_pen );
        }
        else
        {
            // we have to compensate for moved device origins etc. otherwise patterned pens are standing still
            // eg when using a wxScrollWindow and scrolling around
            int origX = LogicalToDeviceX( 0 );
            int origY = LogicalToDeviceY( 0 );
            m_graphicContext->Translate( origX , origY );
            m_graphicContext->SetPen( m_pen );
            m_graphicContext->Translate( -origX , -origY );
        }
    }
}

void wxGCDC::SetBrush( const wxBrush &brush )
{
    if (m_brush == brush)
        return;

    m_brush = brush;
    if ( m_graphicContext )
    {
        if ( brush.GetStyle() == wxSOLID || brush.GetStyle() == wxTRANSPARENT )
        {
            m_graphicContext->SetBrush( m_brush );
        }
        else
        {
            // we have to compensate for moved device origins etc. otherwise patterned brushes are standing still
            // eg when using a wxScrollWindow and scrolling around
            // TODO on MSW / GDIPlus this still occurs with hatched brushes
            int origX = LogicalToDeviceX(0);
            int origY = LogicalToDeviceY(0);
            m_graphicContext->Translate( origX , origY );
            m_graphicContext->SetBrush( m_brush );
            m_graphicContext->Translate( -origX , -origY );
        }
    }
}

void wxGCDC::SetBackground( const wxBrush &brush )
{
    if (m_backgroundBrush == brush)
        return;

    m_backgroundBrush = brush;
    if (!m_backgroundBrush.Ok())
        return;
}

void wxGCDC::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return;

    m_logicalFunction = function;
#if wxMAC_USE_CORE_GRAPHICS_BLEND_MODES

    CGContextRef cgContext = ((wxCairoContext*)(m_graphicContext))->GetNativeContext();
    if ( m_logicalFunction == wxCOPY )
        CGContextSetBlendMode( cgContext, kCGBlendModeNormal );
    else if ( m_logicalFunction == wxINVERT )
        CGContextSetBlendMode( cgContext, kCGBlendModeExclusion );
    else
        CGContextSetBlendMode( cgContext, kCGBlendModeNormal );
#endif

}

bool wxGCDC::DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                         const wxColour& WXUNUSED(col), int WXUNUSED(style))
{
    return false;
}

bool wxGCDC::DoGetPixel( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxColour *WXUNUSED(col) ) const
{
    //  wxCHECK_MSG( 0 , false, wxT("wxGCDC(cg)::DoGetPixel - not implemented") );
    return false;
}

void wxGCDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawLine - invalid DC") );

#if !wxMAC_USE_CORE_GRAPHICS_BLEND_MODES

    if ( m_logicalFunction != wxCOPY )
        return;
#endif

    wxCoord xx1 = LogicalToDeviceX(x1);
    wxCoord yy1 = LogicalToDeviceY(y1);
    wxCoord xx2 = LogicalToDeviceX(x2);
    wxCoord yy2 = LogicalToDeviceY(y2);

    m_graphicContext->StrokeLine(xx1,yy1,xx2,yy2);

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxGCDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoCrossHair - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    int w = 0, h = 0;

    GetSize( &w, &h );

    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord xw = LogicalToDeviceX(w);
    wxCoord x0 = LogicalToDeviceX(0);
    wxCoord y0 = LogicalToDeviceY(0);
    wxCoord yh = LogicalToDeviceY(h);

    m_graphicContext->StrokeLine(x0,yy,xw,yy);
    m_graphicContext->StrokeLine(xx,y0,xx,yh);

    CalcBoundingBox(x0, y0);
    CalcBoundingBox(x0+xw, y0+yh);
}

void wxGCDC::DoDrawArc( wxCoord x1, wxCoord y1,
                        wxCoord x2, wxCoord y2,
                        wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawArc - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    wxCoord xx1 = LogicalToDeviceX(x1);
    wxCoord yy1 = LogicalToDeviceY(y1);
    wxCoord xx2 = LogicalToDeviceX(x2);
    wxCoord yy2 = LogicalToDeviceY(y2);
    wxCoord xxc = LogicalToDeviceX(xc);
    wxCoord yyc = LogicalToDeviceY(yc);

    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx * dx + dy * dy));
    wxCoord rad = (wxCoord)radius;
    double sa, ea;
    if (xx1 == xx2 && yy1 == yy2)
    {
        sa = 0.0;
        ea = 360.0;
    }
    else if (radius == 0.0)
    {
        sa = ea = 0.0;
    }
    else
    {
        sa = (xx1 - xxc == 0) ?
     (yy1 - yyc < 0) ? 90.0 : -90.0 :
             -atan2(double(yy1 - yyc), double(xx1 - xxc)) * RAD2DEG;
        ea = (xx2 - xxc == 0) ?
     (yy2 - yyc < 0) ? 90.0 : -90.0 :
             -atan2(double(yy2 - yyc), double(xx2 - xxc)) * RAD2DEG;
    }

    bool fill = m_brush.GetStyle() != wxTRANSPARENT;

    wxGraphicsPath path = m_graphicContext->CreatePath();
    if ( fill && ((x1!=x2)||(y1!=y2)) )
        path.MoveToPoint( xxc, yyc );
    path.AddArc( xxc, yyc , rad , DegToRad(sa) , DegToRad(ea), false );
    if ( fill && ((x1!=x2)||(y1!=y2)) )
        path.AddLineToPoint( xxc, yyc );
    m_graphicContext->DrawPath(path);
}

void wxGCDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                double sa, double ea )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawEllipticArc - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord ww = m_signX * LogicalToDeviceXRel(w);
    wxCoord hh = m_signY * LogicalToDeviceYRel(h);

    // handle -ve width and/or height
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

    bool fill = m_brush.GetStyle() != wxTRANSPARENT;

    wxGraphicsPath path = m_graphicContext->CreatePath();
    m_graphicContext->PushState();
    m_graphicContext->Translate(xx+ww/2,yy+hh/2);
    wxDouble factor = ((wxDouble) ww) / hh;
    m_graphicContext->Scale( factor , 1.0);
    if ( fill && (sa!=ea) )
        path.MoveToPoint(0,0);
    // since these angles (ea,sa) are measured counter-clockwise, we invert them to
    // get clockwise angles
    path.AddArc( 0, 0, hh/2 , DegToRad(-sa) , DegToRad(-ea), sa > ea );
    if ( fill && (sa!=ea) )
        path.AddLineToPoint(0,0);
    m_graphicContext->DrawPath( path );
    m_graphicContext->PopState();
}

void wxGCDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawPoint - invalid DC") );

    DoDrawLine( x , y , x + 1 , y + 1 );
}

void wxGCDC::DoDrawLines(int n, wxPoint points[],
                         wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawLines - invalid DC") );

#if !wxMAC_USE_CORE_GRAPHICS_BLEND_MODES

    if ( m_logicalFunction != wxCOPY )
        return;
#endif

    wxPoint2DDouble* pointsD = new wxPoint2DDouble[n];
    for( int i = 0; i < n; ++i)
    {
        pointsD[i].m_x = LogicalToDeviceX(points[i].x + xoffset);
        pointsD[i].m_y = LogicalToDeviceY(points[i].y + yoffset);
    }

    m_graphicContext->StrokeLines( n , pointsD);
    delete[] pointsD;
}

#if wxUSE_SPLINES
void wxGCDC::DoDrawSpline(wxList *points)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawSpline - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    wxGraphicsPath path = m_graphicContext->CreatePath();

    wxList::compatibility_iterator node = points->GetFirst();
    if (node == wxList::compatibility_iterator())
        // empty list
        return;

    wxPoint *p = (wxPoint *)node->GetData();

    wxCoord x1 = p->x;
    wxCoord y1 = p->y;

    node = node->GetNext();
    p = (wxPoint *)node->GetData();

    wxCoord x2 = p->x;
    wxCoord y2 = p->y;
    wxCoord cx1 = ( x1 + x2 ) / 2;
    wxCoord cy1 = ( y1 + y2 ) / 2;

    path.MoveToPoint( LogicalToDeviceX( x1 ) , LogicalToDeviceY( y1 ) );
    path.AddLineToPoint( LogicalToDeviceX( cx1 ) , LogicalToDeviceY( cy1 ) );
#if !wxUSE_STL

    while ((node = node->GetNext()) != NULL)
#else

    while ((node = node->GetNext()))
#endif // !wxUSE_STL

    {
        p = (wxPoint *)node->GetData();
        x1 = x2;
        y1 = y2;
        x2 = p->x;
        y2 = p->y;
        wxCoord cx4 = (x1 + x2) / 2;
        wxCoord cy4 = (y1 + y2) / 2;

        path.AddQuadCurveToPoint(
            LogicalToDeviceX( x1 ) , LogicalToDeviceY( y1 ) ,
            LogicalToDeviceX( cx4 ) , LogicalToDeviceY( cy4 ) );

        cx1 = cx4;
        cy1 = cy4;
    }

    path.AddLineToPoint( LogicalToDeviceX( x2 ) , LogicalToDeviceY( y2 ) );

    m_graphicContext->StrokePath( path );
}
#endif // wxUSE_SPLINES

void wxGCDC::DoDrawPolygon( int n, wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset,
                            int fillStyle )
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawPolygon - invalid DC") );

    if ( n <= 0 || (m_brush.GetStyle() == wxTRANSPARENT && m_pen.GetStyle() == wxTRANSPARENT ) )
        return;
    if ( m_logicalFunction != wxCOPY )
        return;

    bool closeIt = false;
    if (points[n-1] != points[0])
        closeIt = true;

    wxPoint2DDouble* pointsD = new wxPoint2DDouble[n+(closeIt?1:0)];
    for( int i = 0; i < n; ++i)
    {
        pointsD[i].m_x = LogicalToDeviceX(points[i].x + xoffset);
        pointsD[i].m_y = LogicalToDeviceY(points[i].y + yoffset);
    }
    if ( closeIt )
        pointsD[n] = pointsD[0];

    m_graphicContext->DrawLines( n+(closeIt?1:0) , pointsD, fillStyle);
    delete[] pointsD;
}

void wxGCDC::DoDrawPolyPolygon(int n,
                               int count[],
                               wxPoint points[],
                               wxCoord xoffset,
                               wxCoord yoffset,
                               int fillStyle)
{
    wxASSERT(n > 1);
    wxGraphicsPath path = m_graphicContext->CreatePath();

    int i = 0;
    for ( int j = 0; j < n; ++j)
    {
        wxPoint start = points[i];
        path.MoveToPoint(LogicalToDeviceX(start.x+ xoffset), LogicalToDeviceY(start.y+ yoffset));
        ++i;
        int l = count[j];
        for ( int k = 1; k < l; ++k)
        {
            path.AddLineToPoint( LogicalToDeviceX(points[i].x+ xoffset), LogicalToDeviceY(points[i].y+ yoffset));
            ++i;
        }
        // close the polygon
        if ( start != points[i-1])
            path.AddLineToPoint( LogicalToDeviceX(start.x+ xoffset), LogicalToDeviceY(start.y+ yoffset));
    }
    m_graphicContext->DrawPath( path , fillStyle);
}

void wxGCDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawRectangle - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord ww = m_signX * LogicalToDeviceXRel(width);
    wxCoord hh = m_signY * LogicalToDeviceYRel(height);

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
    if ( m_graphicContext->ShouldOffset() )
    {
        // if we are offsetting the entire rectangle is moved 0.5, so the
        // border line gets off by 1
        ww -= 1;
        hh -= 1;
    }
    m_graphicContext->DrawRectangle( xx,yy,ww,hh);
}

void wxGCDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawRoundedRectangle - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    if (radius < 0.0)
        radius = - radius * ((width < height) ? width : height);
    wxCoord xx = LogicalToDeviceX(x);
    wxCoord yy = LogicalToDeviceY(y);
    wxCoord ww = m_signX * LogicalToDeviceXRel(width);
    wxCoord hh = m_signY * LogicalToDeviceYRel(height);

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

    m_graphicContext->DrawRoundedRectangle( xx,yy,ww,hh,radius);
}

void wxGCDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawEllipse - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return;

    wxDouble xx = LogicalToDeviceX(x);
    wxDouble yy = LogicalToDeviceY(y);
    wxDouble ww = m_signX * LogicalToDeviceXRel(width);
    wxDouble hh = m_signY * LogicalToDeviceYRel(height);

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

    m_graphicContext->DrawEllipse(xx,yy,ww,hh);
}

bool wxGCDC::CanDrawBitmap() const
{
    return true;
}

bool wxGCDC::DoBlit(
    wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
    wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool WXUNUSED(useMask),
    wxCoord xsrcMask, wxCoord ysrcMask )
{
    wxCHECK_MSG( Ok(), false, wxT("wxGCDC(cg)::DoBlit - invalid DC") );
    wxCHECK_MSG( source->Ok(), false, wxT("wxGCDC(cg)::DoBlit - invalid source DC") );

    if ( logical_func == wxNO_OP )
        return true;

    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc;
        ysrcMask = ysrc;
    }

    wxCoord yysrc = source-> LogicalToDeviceY(ysrc);
    wxCoord xxsrc = source-> LogicalToDeviceX(xsrc);
    wxCoord wwsrc = source-> LogicalToDeviceXRel(width);
    wxCoord hhsrc = source-> LogicalToDeviceYRel(height);

    wxCoord yydest = LogicalToDeviceY(ydest);
    wxCoord xxdest = LogicalToDeviceX(xdest);
    wxCoord wwdest = LogicalToDeviceXRel(width);
    wxCoord hhdest = LogicalToDeviceYRel(height);

    wxMemoryDC* memdc = wxDynamicCast(source,wxMemoryDC);
    if ( memdc && logical_func == wxCOPY )
    {
        wxBitmap blit = memdc->GetSelectedBitmap();

        wxASSERT_MSG( blit.Ok() , wxT("Invalid bitmap for blitting") );

        wxCoord bmpwidth = blit.GetWidth();
        wxCoord bmpheight = blit.GetHeight();

        if ( xxsrc != 0 || yysrc != 0 || bmpwidth != wwsrc || bmpheight != hhsrc )
        {
            wwsrc = wxMin( wwsrc , bmpwidth - xxsrc );
            hhsrc = wxMin( hhsrc , bmpheight - yysrc );
            if ( wwsrc > 0 && hhsrc > 0 )
            {
                if ( xxsrc >= 0 && yysrc >= 0 )
                {
                    wxRect subrect( xxsrc, yysrc, wwsrc , hhsrc );
                    // TODO we perhaps could add a DrawSubBitmap call to dc for performance reasons
                    blit = blit.GetSubBitmap( subrect );
                }
                else
                {
                    // in this case we'd probably have to adjust the different coordinates, but
                    // we have to find out proper contract first
                    blit = wxNullBitmap;
                }
            }
            else
            {
                blit = wxNullBitmap;
            }
        }

        if ( blit.Ok() )
        {
            m_graphicContext->DrawBitmap( blit, xxdest , yydest , wwdest , hhdest );
        }
    }
    else
    {
        wxFAIL_MSG( wxT("Blitting is only supported from bitmap contexts, and only with wxCOPY logical operation.") );
        return false;
    }

    return true;
}

void wxGCDC::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y,
                               double angle)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawRotatedText - invalid DC") );

    if ( str.length() == 0 )
        return;
    if ( m_logicalFunction != wxCOPY )
        return;

    int drawX = LogicalToDeviceX(x);
    int drawY = LogicalToDeviceY(y);

    m_graphicContext->DrawText( str, drawX ,drawY , DegToRad(angle ));
}

void wxGCDC::DoDrawText(const wxString& str, wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoDrawRotatedText - invalid DC") );

    if ( str.length() == 0 )
        return;
    if ( m_logicalFunction != wxCOPY )
        return;

    int drawX = LogicalToDeviceX(x);
    int drawY = LogicalToDeviceY(y);

    m_graphicContext->DrawText( str, drawX ,drawY);
}

bool wxGCDC::CanGetTextExtent() const
{
    wxCHECK_MSG( Ok(), false, wxT("wxGCDC(cg)::CanGetTextExtent - invalid DC") );

    return true;
}

void wxGCDC::DoGetTextExtent( const wxString &str, wxCoord *width, wxCoord *height,
                              wxCoord *descent, wxCoord *externalLeading ,
                              wxFont *theFont ) const
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::DoGetTextExtent - invalid DC") );

    if ( theFont )
    {
        m_graphicContext->SetFont( *theFont, m_textForegroundColour );
    }

    wxDouble h , d , e , w;

    m_graphicContext->GetTextExtent( str, &w, &h, &d, &e );

    if ( height )
        *height = DeviceToLogicalYRel((wxCoord)h);
    if ( descent )
        *descent = DeviceToLogicalYRel((wxCoord)d);
    if ( externalLeading )
        *externalLeading = DeviceToLogicalYRel((wxCoord)e);
    if ( width )
        *width = DeviceToLogicalXRel((wxCoord)w);

    if ( theFont )
    {
        m_graphicContext->SetFont( m_font, m_textForegroundColour );
    }
}

bool wxGCDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxGCDC(cg)::DoGetPartialTextExtents - invalid DC") );
    widths.Clear();
    widths.Add(0,text.Length());
    if ( text.IsEmpty() )
        return true;

    wxArrayDouble widthsD;

    m_graphicContext->GetPartialTextExtents( text, widthsD );
    for ( size_t i = 0; i < widths.GetCount(); ++i )
        widths[i] = DeviceToLogicalXRel((wxCoord)(widthsD[i] + 0.5));

    return true;
}

wxCoord wxGCDC::GetCharWidth(void) const
{
    wxCoord width;
    DoGetTextExtent( wxT("g") , &width , NULL , NULL , NULL , NULL );

    return width;
}

wxCoord wxGCDC::GetCharHeight(void) const
{
    wxCoord height;
    DoGetTextExtent( wxT("g") , NULL , &height , NULL , NULL , NULL );

    return height;
}

void wxGCDC::Clear(void)
{
    wxCHECK_RET( Ok(), wxT("wxGCDC(cg)::Clear - invalid DC") );
    // TODO better implementation / incorporate size info into wxGCDC or context
    m_graphicContext->SetBrush( m_backgroundBrush );
    wxPen p = *wxTRANSPARENT_PEN;
    m_graphicContext->SetPen( p );
    DoDrawRectangle( 0, 0, 32000 , 32000 );
    m_graphicContext->SetPen( m_pen );    
    m_graphicContext->SetBrush( m_brush );
}

void wxGCDC::DoGetSize(int *width, int *height) const
{
    *width = 1000;
    *height = 1000;
}

void wxGCDC::DoGradientFillLinear(const wxRect& rect,
                                  const wxColour& initialColour,
                                  const wxColour& destColour,
                                  wxDirection nDirection )
{
    wxPoint start;
    wxPoint end;
    switch( nDirection)
    {
    case wxWEST :
        start = rect.GetRightBottom();
        start.x++;
        end = rect.GetLeftBottom();
        break;
    case wxEAST :
        start = rect.GetLeftBottom();
        end = rect.GetRightBottom();
        end.x++;
        break;
    case wxNORTH :
        start = rect.GetLeftBottom();
        start.y++;
        end = rect.GetLeftTop();
        break;
    case wxSOUTH :
        start = rect.GetLeftTop();
        end = rect.GetLeftBottom();
        end.y++;
        break;
    default :
        break;
    }

    m_graphicContext->SetBrush( m_graphicContext->CreateLinearGradientBrush(
        LogicalToDeviceX(start.x),LogicalToDeviceY(start.y),
        LogicalToDeviceX(end.x),LogicalToDeviceY(end.y), initialColour, destColour));

    wxDouble xx = LogicalToDeviceX(rect.x);
    wxDouble yy = LogicalToDeviceY(rect.y);
    wxDouble ww = m_signX * LogicalToDeviceXRel(rect.width);
    wxDouble hh = m_signY * LogicalToDeviceYRel(rect.height);

    if (ww == 0 || hh == 0)
        return;

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

    m_graphicContext->SetPen(*wxTRANSPARENT_PEN);
    m_graphicContext->DrawRectangle(xx,yy,ww,hh);
    m_graphicContext->SetPen(m_pen);
}

void wxGCDC::DoGradientFillConcentric(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      const wxPoint& circleCenter)
{
    //Radius
    wxInt32 cx = rect.GetWidth() / 2;
    wxInt32 cy = rect.GetHeight() / 2;
    wxInt32 nRadius;
    if (cx < cy)
        nRadius = cx;
    else
        nRadius = cy;

    wxDouble xx = LogicalToDeviceX(rect.x);
    wxDouble yy = LogicalToDeviceY(rect.y);
    wxDouble ww = m_signX * LogicalToDeviceXRel(rect.width);
    wxDouble hh = m_signY * LogicalToDeviceYRel(rect.height);

    if (ww == 0 || hh == 0)
        return;

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

    m_graphicContext->SetPen(*wxTRANSPARENT_PEN);
    m_graphicContext->SetBrush( wxBrush( destColour) );
    m_graphicContext->DrawRectangle( xx,yy,ww,hh);

    m_graphicContext->SetBrush( m_graphicContext->CreateRadialGradientBrush(
        xx+LogicalToDeviceX(circleCenter.x),yy+LogicalToDeviceY(circleCenter.y),
        xx+LogicalToDeviceX(circleCenter.x),yy+LogicalToDeviceY(circleCenter.y),
        LogicalToDeviceXRel(nRadius),
        initialColour,destColour));

    m_graphicContext->DrawRectangle( xx,yy,ww,hh);
    m_graphicContext->SetPen(m_pen);
}

void wxGCDC::DoDrawCheckMark(wxCoord x, wxCoord y,
                             wxCoord width, wxCoord height)
{
    wxDCBase::DoDrawCheckMark(x,y,width,height);
}

#endif // wxUSE_GRAPHICS_CONTEXT
