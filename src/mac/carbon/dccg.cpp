/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dc.h"
#endif

#include "wx/wxprec.h"

#include "wx/dc.h"

#if wxMAC_USE_CORE_GRAPHICS

#include "wx/app.h"
#include "wx/mac/uma.h"
#include "wx/dcmemory.h"
#include "wx/dcprint.h"
#include "wx/region.h"
#include "wx/image.h"
#include "wx/log.h"


#if __MSL__ >= 0x6000
#include "math.h"
using namespace std ;
#endif

#include "wx/mac/private.h"
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>
#include <FixMath.h>
#include <CGContext.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#if !defined( __DARWIN__ ) || defined(__MWERKS__)
#ifndef M_PI
const double M_PI = 3.14159265358979 ;
#endif
#endif
const double RAD2DEG  = 180.0 / M_PI;
const short kEmulatedMode = -1 ;
const short kUnsupportedMode = -2 ;

extern TECObjectRef s_TECNativeCToUnicode ;

// TODO Update
// The text ctrl implementation still needs that for the non hiview implementation

wxMacWindowClipper::wxMacWindowClipper( const wxWindow* win ) :
    wxMacPortSaver( (GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) )
{
    m_newPort =(GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) ;
    m_formerClip = NewRgn() ;
    m_newClip = NewRgn() ;
    GetClip( m_formerClip ) ;
    
    if ( win )
    {
        int x = 0 , y = 0;
        win->MacWindowToRootWindow( &x,&y ) ;
        // get area including focus rect
        CopyRgn( (RgnHandle) ((wxWindow*)win)->MacGetVisibleRegion(true).GetWXHRGN() , m_newClip ) ;
        if ( !EmptyRgn( m_newClip ) )
            OffsetRgn( m_newClip , x , y ) ;

        SetClip( m_newClip ) ;
    }
}

wxMacWindowClipper::~wxMacWindowClipper() 
{
    SetPort( m_newPort ) ;
    SetClip( m_formerClip ) ;
    DisposeRgn( m_newClip ) ;
    DisposeRgn( m_formerClip ) ;
}

wxMacWindowStateSaver::wxMacWindowStateSaver( const wxWindow* win ) :
    wxMacWindowClipper( win )
{
    // the port is already set at this point
    m_newPort =(GrafPtr) GetWindowPort((WindowRef) win->MacGetTopLevelWindowRef()) ;
    GetThemeDrawingState( &m_themeDrawingState ) ;
}

wxMacWindowStateSaver::~wxMacWindowStateSaver() 
{
    SetPort( m_newPort ) ;
    SetThemeDrawingState( m_themeDrawingState , true ) ;
}

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

static inline double dmin(double a, double b) { return a < b ? a : b; }
static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxMacCGContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

wxMacCGPath::wxMacCGPath()
{
    m_path = CGPathCreateMutable() ;
}

wxMacCGPath::~wxMacCGPath()
{
    CGPathRelease( m_path ) ;
}

//  Starts a new subpath at
void wxMacCGPath::MoveToPoint( wxCoord x1 , wxCoord y1 )
{
    CGPathMoveToPoint( m_path , NULL , x1 , y1 ) ;
}

void wxMacCGPath::AddLineToPoint( wxCoord x1 , wxCoord y1 ) 
{
    CGPathAddLineToPoint( m_path , NULL , x1 , y1 ) ;
}

void wxMacCGPath::AddRectangle( wxCoord x, wxCoord y, wxCoord w, wxCoord h )
{
    CGRect cgRect = { { x , y } , { w , h } } ;
    CGPathAddRect( m_path , NULL , cgRect ) ;
}

void wxMacCGPath::AddCircle( wxCoord x, wxCoord y , wxCoord r )
{
    CGPathAddArc( m_path , NULL , x , y , r , 0.0 , 2 * M_PI , true ) ;
}

// closes the current subpath
void wxMacCGPath::CloseSubpath()
{
    CGPathCloseSubpath( m_path ) ;
}

CGPathRef wxMacCGPath::GetPath() const 
{
    return m_path ;
}

// we always stock two context states, one at entry, the other one after 
// changing to HI Graphics orientation (this one is used for getting back clippings etc)

wxMacCGContext::wxMacCGContext( CGrafPtr port ) 
{
    m_qdPort = port ;
    m_cgContext = NULL ;
}

wxMacCGContext::wxMacCGContext( CGContextRef cgcontext )
{
    m_qdPort = NULL ;
    m_cgContext = cgcontext ;
    CGContextSaveGState( m_cgContext ) ;
    CGContextSaveGState( m_cgContext ) ;
}

wxMacCGContext::wxMacCGContext()
{
    m_qdPort = NULL ;
    m_cgContext = NULL ;
}

wxMacCGContext::~wxMacCGContext() 
{
    if ( m_cgContext )
    {
        CGContextRestoreGState( m_cgContext ) ;
        CGContextRestoreGState( m_cgContext ) ;
    }
    if ( m_qdPort )
        QDEndCGContext( m_qdPort , &m_cgContext ) ;
}


void wxMacCGContext::Clip( const wxRegion &region )
{
//        ClipCGContextToRegion ( m_cgContext, &bounds , (RgnHandle) dc->m_macCurrentClipRgn ) ;
}

void wxMacCGContext::StrokePath( const wxGraphicPath *p ) 
{
    const wxMacCGPath* path = dynamic_cast< const wxMacCGPath*>( p ) ;
    CGContextBeginPath( m_cgContext ) ;
    CGContextAddPath( m_cgContext , path->GetPath() ) ;
    CGContextClosePath( m_cgContext ) ;
    CGContextStrokePath( m_cgContext ) ;
}

void wxMacCGContext::DrawPath( const wxGraphicPath *p , int fillStyle ) 
{
    const wxMacCGPath* path = dynamic_cast< const wxMacCGPath*>( p ) ;
    CGPathDrawingMode mode = m_mode ;
    if ( fillStyle == wxODDEVEN_RULE )
    {
        if ( mode == kCGPathFill )
            mode = kCGPathEOFill ;
        else if ( mode == kCGPathFillStroke )
            mode = kCGPathEOFillStroke ;
    }
    CGContextBeginPath( m_cgContext ) ;
    CGContextAddPath( m_cgContext , path->GetPath() ) ;
    CGContextClosePath( m_cgContext ) ;
    CGContextDrawPath( m_cgContext , mode ) ;
}

void wxMacCGContext::FillPath( const wxGraphicPath *p , const wxColor &fillColor , int fillStyle ) 
{
    const wxMacCGPath* path = dynamic_cast< const wxMacCGPath*>( p ) ;
    CGContextSaveGState( m_cgContext ) ;
    
    RGBColor col = MAC_WXCOLORREF( fillColor.GetPixel() ) ;
    CGContextSetRGBFillColor( m_cgContext , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
    CGPathDrawingMode mode = kCGPathFill ;

    if ( fillStyle == wxODDEVEN_RULE )
        mode = kCGPathEOFill ;
               
    CGContextBeginPath( m_cgContext ) ;
    CGContextAddPath( m_cgContext , path->GetPath() ) ;
    CGContextClosePath( m_cgContext ) ;
    CGContextDrawPath( m_cgContext , mode ) ;
    
    CGContextRestoreGState( m_cgContext ) ;
}
    
wxGraphicPath* wxMacCGContext::CreatePath() 
{ 
    // make sure that we now have a real cgref, before doing
    // anything with paths
    return new wxMacCGPath() ; 
}

// in case we only got a QDPort only create a cgref now

CGContextRef wxMacCGContext::GetNativeContext() 
{ 
    if( m_cgContext == NULL )
    {
        Rect bounds ;
        GetPortBounds( (CGrafPtr) m_qdPort , &bounds ) ;
        OSStatus status = QDBeginCGContext( (CGrafPtr) m_qdPort , &m_cgContext ) ;
        CGContextSaveGState( m_cgContext ) ;

        wxASSERT_MSG( status == noErr , wxT("Cannot nest wxDCs on the same window") ) ;
        CGContextTranslateCTM( m_cgContext , 0 , bounds.bottom - bounds.top ) ;
        CGContextScaleCTM( m_cgContext , 1 , -1 ) ;
        
        CGContextSaveGState( m_cgContext ) ;
        SetPen( m_pen ) ;
        SetBrush( m_brush ) ;
    }
    return m_cgContext ; 
}

void wxMacCGContext::SetNativeContext( CGContextRef cg ) 
{ 
    wxASSERT( m_cgContext == NULL ) ;
    m_cgContext = cg ; 
    CGContextSaveGState( m_cgContext ) ;
}

void wxMacCGContext::SetPen( const wxPen &pen )
{
    m_pen = pen ;
    if ( m_cgContext == NULL )
        return ;
    bool fill = m_brush.GetStyle() != wxTRANSPARENT ;
    bool stroke = pen.GetStyle() != wxTRANSPARENT ;
    
#if 0
    // we can benchmark performance, should go into a setting later
    CGContextSetShouldAntialias( m_cgContext , false ) ;
#endif
    if ( fill | stroke )
    {
        // setup brushes
        m_mode = kCGPathFill ; // just a default

        if ( fill )
        {
            m_mode = kCGPathFill ;
        }
        if ( stroke )
        {
            RGBColor col = MAC_WXCOLORREF( pen.GetColour().GetPixel() ) ;
            CGContextSetRGBStrokeColor( m_cgContext , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
            
            CGLineCap cap ;
            switch( pen.GetCap() )
            {
                case wxCAP_ROUND :
                    cap = kCGLineCapRound ;
                    break ;
                case wxCAP_PROJECTING :
                    cap = kCGLineCapSquare ;
                    break ;
                case wxCAP_BUTT :
                    cap = kCGLineCapButt ;
                    break ;
                default :
                    cap = kCGLineCapButt ;
                    break ;
            }
            CGContextSetLineCap( m_cgContext , cap ) ;

            CGLineJoin join ;
            switch( pen.GetJoin() )
            {
                case wxJOIN_BEVEL :
                    join = kCGLineJoinBevel ;
                    break ;
                case wxJOIN_MITER :
                    join = kCGLineJoinMiter ;
                    break ;
                case wxJOIN_ROUND :
                    join = kCGLineJoinRound ;
                    break ;
                default :
                    join = kCGLineJoinMiter ;
                    break; 
            }
            CGContextSetLineJoin( m_cgContext , join ) ;

            CGContextSetLineWidth( m_cgContext , pen.GetWidth() == 0 ? 0.1 :  pen.GetWidth() /* TODO * m_dc->m_scaleX */ ) ; 

            m_mode = kCGPathStroke ;
            int count = 0 ;
            const float *lengths = NULL ;
            float *userLengths = NULL ;
            
            const float dotted[] = { 3 , 3 };
            const float dashed[] = { 19 , 9 };
            const float short_dashed[] = { 9 , 6 };
            const float dotted_dashed[] = { 9 , 6 , 3 , 3 };
            
            switch( pen.GetStyle() )
            {
                case wxSOLID :
                    break ;
                case wxDOT :
                    lengths = dotted ;
                    count = WXSIZEOF(dotted);
                    break ;
                case wxLONG_DASH :
                    lengths = dashed ;
                    count = WXSIZEOF(dashed) ;
                    break ;
                case wxSHORT_DASH :
                    lengths = short_dashed ;
                    count = WXSIZEOF(short_dashed) ;
                    break ;
                case wxDOT_DASH :
                    lengths = dotted_dashed ;
                    count = WXSIZEOF(dotted_dashed);
                    break ;
                case wxUSER_DASH :
                    wxDash *dashes ;
                    count = pen.GetDashes( &dashes ) ;
                    if ( count >0 )
                    {
                        userLengths = new float[count] ;
                        for( int i = 0 ; i < count ; ++i )
                            userLengths[i] = dashes[i] ;
                    }
                    lengths = userLengths ;
                    break ;
                default :
                    break ; 
            }

            CGContextSetLineDash( m_cgContext , 0 , lengths , count ) ;
            delete[] userLengths ;
            // we need to change the cap, otherwise everything overlaps
            // and we get solid lines
            if ( count > 0 )
                CGContextSetLineCap( m_cgContext , kCGLineCapButt ) ;
        }
        if ( fill && stroke )
        {
            m_mode = kCGPathFillStroke ;
        }
        
    }
}
void wxMacCGContext::SetBrush( const wxBrush &brush )
{
    m_brush = brush ;
    if ( m_cgContext == NULL )
        return ;

    bool fill = brush.GetStyle() != wxTRANSPARENT ;
    bool stroke = m_pen.GetStyle() != wxTRANSPARENT ;

#if 0
    // we can benchmark performance, should go into a setting later
    CGContextSetShouldAntialias( m_cgContext , false ) ;
#endif
    if ( fill | stroke )
    {

        // setup brushes
        m_mode = kCGPathFill ; // just a default

        if ( fill )
        {
            RGBColor col = MAC_WXCOLORREF( brush.GetColour().GetPixel() ) ;
            CGContextSetRGBFillColor( m_cgContext , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
            m_mode = kCGPathFill ;
        }
        if ( stroke )
        {
            m_mode = kCGPathStroke ;
        }
        if ( fill && stroke )
        {
            m_mode = kCGPathFillStroke ;
        }
        
    }
}

// snippets from Sketch Sample from Apple :

#define	kGenericRGBProfilePathStr       "/System/Library/ColorSync/Profiles/Generic RGB Profile.icc"
/*
    This function locates, opens, and returns the profile reference for the calibrated 
    Generic RGB color space. It is up to the caller to call CMCloseProfile when done
    with the profile reference this function returns.
*/
CMProfileRef wxMacOpenGenericProfile(void)
{
    static CMProfileRef cachedRGBProfileRef = NULL;
    
    // we only create the profile reference once
    if (cachedRGBProfileRef == NULL)
    {
		CMProfileLocation 	loc;
	
		loc.locType = cmPathBasedProfile;
		strcpy(loc.u.pathLoc.path, kGenericRGBProfilePathStr);
	
		verify_noerr( CMOpenProfile(&cachedRGBProfileRef, &loc) );
    }

    if (cachedRGBProfileRef)
    {
		// clone the profile reference so that the caller has their own reference, not our cached one
		CMCloneProfileRef(cachedRGBProfileRef);   
    }

    return cachedRGBProfileRef;
}

/*
    Return the generic RGB color space. This is a 'get' function and the caller should
    not release the returned value unless the caller retains it first. Usually callers
    of this routine will immediately use the returned colorspace with CoreGraphics
    so they typically do not need to retain it themselves.
    
    This function creates the generic RGB color space once and hangs onto it so it can
    return it whenever this function is called.
*/

CGColorSpaceRef wxMacGetGenericRGBColorSpace()
{
    static CGColorSpaceRef genericRGBColorSpace = NULL;

	if (genericRGBColorSpace == NULL)
	{
		CMProfileRef genericRGBProfile = wxMacOpenGenericProfile();
	
		if (genericRGBProfile)
		{
			genericRGBColorSpace = CGColorSpaceCreateWithPlatformColorSpace(genericRGBProfile);
			wxASSERT_MSG( genericRGBColorSpace != NULL, wxT("couldn't create the generic RGB color space") ) ;
			
			// we opened the profile so it is up to us to close it
			CMCloseProfile(genericRGBProfile); 
		}
	}
    return genericRGBColorSpace;
}

void AddEllipticArcToPath(CGContextRef c, CGPoint center, float a, float b, float fromDegree , float toDegree )
{
    CGContextSaveGState(c);
    CGContextTranslateCTM(c, center.x, center.y);
    CGContextScaleCTM(c, a, b);
    CGContextMoveToPoint(c, 1, 0);
    CGContextAddArc(c, 0, 0, 1, DegToRad(fromDegree), DegToRad(toDegree), 0);
    CGContextClosePath(c);
    CGContextRestoreGState(c);
} 

void AddRoundedRectToPath(CGContextRef c, CGRect rect, float ovalWidth,
      float ovalHeight)
{
    float fw, fh;
    if (ovalWidth == 0 || ovalHeight == 0) 
    {
        CGContextAddRect(c, rect);
        return;
    }
    CGContextSaveGState(c);
    CGContextTranslateCTM(c, CGRectGetMinX(rect), CGRectGetMinY(rect));
    CGContextScaleCTM(c, ovalWidth, ovalHeight);
    fw = CGRectGetWidth(rect) / ovalWidth;
    fh = CGRectGetHeight(rect) / ovalHeight;
    CGContextMoveToPoint(c, fw, fh/2);
    CGContextAddArcToPoint(c, fw, fh, fw/2, fh, 1);
    CGContextAddArcToPoint(c, 0, fh, 0, fh/2, 1);
    CGContextAddArcToPoint(c, 0, 0, fw/2, 0, 1);
    CGContextAddArcToPoint(c, fw, 0, fw, fh/2, 1);
    CGContextClosePath(c);
    CGContextRestoreGState(c);
} 

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

    m_ok = FALSE ;

    m_macLocalOrigin.x = m_macLocalOrigin.y = 0 ;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_macATSUIStyle = NULL ;

    m_graphicContext = NULL ;
}

wxDC::~wxDC(void)
{
    if( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }

    delete m_graphicContext ;  
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );
    wxCHECK_RET( bmp.Ok(), wxT("invalid bitmap") );
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord w = bmp.GetWidth();
    wxCoord h = bmp.GetHeight();
    wxCoord ww = XLOG2DEVREL(w);
    wxCoord hh = YLOG2DEVREL(h);

    CGContextRef cg = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
    CGImageRef image = (CGImageRef)( bmp.CGImageCreate() ) ;
    HIRect r = CGRectMake( xx , yy , ww , hh ) ;
    HIViewDrawCGImage( cg , &r , image ) ;
    CGImageRelease( image ) ;
}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    wxCHECK_RET(Ok(), wxT("Invalid dc  wxDC::DoDrawIcon"));
    wxCHECK_RET(icon.Ok(), wxT("Invalid icon wxDC::DoDrawIcon"));

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord w = icon.GetWidth();
    wxCoord h = icon.GetHeight();
    wxCoord ww = XLOG2DEVREL(w);
    wxCoord hh = YLOG2DEVREL(h);

    CGContextRef cg = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
    CGRect r = CGRectMake( 00 , 00 , ww , hh ) ;
    CGContextSaveGState(cg);    
    CGContextTranslateCTM(cg, xx , yy + hh );
    CGContextScaleCTM(cg, 1, -1);
    PlotIconRefInContext( cg , &r , kAlignNone , kTransformNone ,
        NULL , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) ) ;
    CGContextRestoreGState( cg ) ;
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoSetClippingRegion  Invalid DC"));
    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(width);
    hh = YLOG2DEVREL(height);

    CGContextRef cgContext = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
    CGRect clipRect = CGRectMake( xx ,yy , ww, hh ) ;
    CGContextClipToRect( cgContext , clipRect ) ;

//    SetRectRgn( (RgnHandle) m_macCurrentClipRgn , xx , yy , xx + ww , yy + hh ) ;
//    SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
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
    // TODO as soon as we don't reset the context for each operation anymore
    // we have to update the context as well
}

void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") ) ;
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
        /*
        CopyRgn( (RgnHandle) region.GetWXHRGN() , (RgnHandle) m_macCurrentClipRgn ) ;
        if ( xx != x || yy != y )
        {
            OffsetRgn( (RgnHandle) m_macCurrentClipRgn , xx - x , yy - y ) ;
        }
        SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
        */
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
//    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
    CGContextRef cgContext = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
    CGContextRestoreGState( cgContext );    
    CGContextSaveGState( cgContext );    
    SetPen( m_pen ) ;
    SetBrush( m_brush ) ;
    m_clipping = FALSE;
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
    if ( col != m_textForegroundColour )
    {
        m_textForegroundColour = col;
        MacInstallFont() ;
    }
}

void wxDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    m_textBackgroundColour = col;
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

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    m_signX = (xLeftRight ?  1 : -1);
    m_signY = (yBottomUp  ? -1 :  1);
    ComputeScaleAndOrigin();
}

wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxDC::GetDepth() const
{
    return 32 ;
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
        wxPen pen(GetPen());
        m_pen = wxNullPen;
        SetPen(pen);
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
    MacInstallFont() ;
}

void  wxDC::SetPen( const wxPen &pen )
{
    if ( m_pen == pen )
        return ;
    m_pen = pen;
    if ( m_graphicContext )
    {
        m_graphicContext->SetPen( m_pen ) ;
    }
}

void  wxDC::SetBrush( const wxBrush &brush )
{
    if (m_brush == brush)
        return;
    m_brush = brush;
    if ( m_graphicContext )
    {
        m_graphicContext->SetBrush( m_brush ) ;
    }
}

void  wxDC::SetBackground( const wxBrush &brush )
{
    if (m_backgroundBrush == brush)
        return;
    m_backgroundBrush = brush;
    if (!m_backgroundBrush.Ok())
        return;
}

void  wxDC::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return;
    m_logicalFunction = function ;
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);

bool wxDC::DoFloodFill(wxCoord x, wxCoord y,
                       const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool  wxDC::DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC::DoGetPixel  Invalid DC") );
    wxFAIL_MSG( wxT("GetPixel not implemented on Core Graphics") ) ;
    return false ;
}

void  wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    
    wxCoord xx1 = XLOG2DEVMAC(x1) ;
    wxCoord yy1 = YLOG2DEVMAC(y1) ;
    wxCoord xx2 = XLOG2DEVMAC(x2) ;
    wxCoord yy2 = YLOG2DEVMAC(y2) ;

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( xx1 , yy1 ) ;
    path->AddLineToPoint( xx2 , yy2 ) ;
    path->CloseSubpath() ;
    m_graphicContext->StrokePath( path ) ;
    delete path ;

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void  wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoCrossHair  Invalid window dc") );

    
    int w = 0;
    int h = 0;
    GetSize( &w, &h );
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( XLOG2DEVMAC(0), yy  ) ;
    path->AddLineToPoint( XLOG2DEVMAC(w), yy ) ;
    path->CloseSubpath() ;
    path->MoveToPoint( xx, YLOG2DEVMAC(0) ) ;
    path->AddLineToPoint( xx, YLOG2DEVMAC(h) ) ;
    path->CloseSubpath() ;
    m_graphicContext->StrokePath( path ) ;
    delete path ;

    CalcBoundingBox(x, y);
    CalcBoundingBox(x+w, y+h);
}

/*
* To draw arcs properly the angles need to be converted from the WX style:
* Angles start on the +ve X axis and go anti-clockwise (As you would draw on
* a normal axis on paper).
* TO
* the Mac style:
* Angles start on the +ve y axis and go clockwise.
*/

static double wxConvertWXangleToMACangle(double angle)
{
    double newAngle = 90 - angle ;
    if ( newAngle < 0 )
        newAngle += 360 ;
    return newAngle ;
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
    if( (xx1 > xx2) || (yy1 > yy2) ) {
        alpha2 *= -1;
    }
    wxMacCGContext* mctx = ((wxMacCGContext*) m_graphicContext) ;
    CGContextRef ctx = mctx->GetNativeContext() ;
    AddEllipticArcToPath( ctx , CGPointMake( xxc , yyc ) , rad , rad , 0 , 180 ) ;
    CGContextDrawPath( ctx , mctx->GetDrawingMode() ) ;
}

void  wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                              double sa, double ea )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawEllepticArc  Invalid DC"));

    double angle = sa - ea;  // Order important Mac in opposite direction to wx
    // we have to make sure that the filling is always counter-clockwise
    if ( angle > 0 )
        angle -= 360 ;
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(w);
    wxCoord hh = m_signY * YLOG2DEVREL(h);
    // handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
    sa = wxConvertWXangleToMACangle(sa);
    wxMacCGContext* mctx = ((wxMacCGContext*) m_graphicContext) ;
    CGContextRef ctx = mctx->GetNativeContext() ;
    AddEllipticArcToPath( ctx  , CGPointMake( xx + ww / 2 , yy + hh / 2 ) , ww / 2 , hh / 2 , sa , angle) ;
    CGContextDrawPath( ctx , mctx->GetDrawingMode() ) ;
}

void  wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    DoDrawLine( x , y , x + 1 , y + 1 ) ;
}

void  wxDC::DoDrawLines(int n, wxPoint points[],
                        wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    
    wxCoord x1, x2 , y1 , y2 ;
    x1 = XLOG2DEVMAC(points[0].x + xoffset);
    y1 = YLOG2DEVMAC(points[0].y + yoffset);
    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( x1 , y1 ) ;
    for (int i = 1; i < n; i++)
    {
        x2 = XLOG2DEVMAC(points[i].x + xoffset);
        y2 = YLOG2DEVMAC(points[i].y + yoffset);

        path->AddLineToPoint( x2 , y2 ) ;
    }
    m_graphicContext->StrokePath( path ) ;
    delete path ;
}

void  wxDC::DoDrawPolygon(int n, wxPoint points[],
                          wxCoord xoffset, wxCoord yoffset,
                          int fillStyle )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
        wxCoord x1, x2 , y1 , y2 ;
    if ( n== 0 || (m_brush.GetStyle() == wxTRANSPARENT && m_pen.GetStyle() == wxTRANSPARENT ) )
        return ;
        
    x2 = x1 = XLOG2DEVMAC(points[0].x + xoffset);
    y2 = y1 = YLOG2DEVMAC(points[0].y + yoffset);
    
    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( x1 , y1 ) ;
    for (int i = 1; i < n; i++)
    {
        x2 = XLOG2DEVMAC(points[i].x + xoffset);
        y2 = YLOG2DEVMAC(points[i].y + yoffset);

        path->AddLineToPoint( x2 , y2 ) ;
    }
    if ( x1 != x2 || y1 != y2 )
    {
        path->AddLineToPoint( x1,y1 ) ;
    }
    path->CloseSubpath() ;
    m_graphicContext->DrawPath( path , fillStyle ) ;
    delete path ;
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
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
    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->AddRectangle(xx ,yy , ww , hh ) ;
    m_graphicContext->DrawPath( path ) ;
    delete path ;
}

void  wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                   wxCoord width, wxCoord height,
                                   double radius)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
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
    wxMacCGContext* mctx = ((wxMacCGContext*) m_graphicContext) ;
    CGContextRef ctx = mctx->GetNativeContext() ;
    AddRoundedRectToPath( ctx  , CGRectMake( xx , yy , ww , hh ) , 16 ,16  ) ;
    CGContextDrawPath( ctx , mctx->GetDrawingMode() ) ;
}

void  wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
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

    wxMacCGContext* mctx = ((wxMacCGContext*) m_graphicContext) ;
    CGContextRef ctx = mctx->GetNativeContext() ;
    if ( width == height )
    {
        CGContextBeginPath(ctx);
        CGContextAddArc(ctx ,
            xx + ww / 2,
            yy + hh / 2,
            ww / 2,
            0,
            2 * M_PI,
            0 ) ;
        CGContextClosePath(ctx);

        CGContextDrawPath( ctx , kCGPathFillStroke ) ;
    }
    else
    {
        AddEllipticArcToPath( ctx , CGPointMake( xx + ww / 2 , yy + hh / 2 ) , ww / 2 , hh / 2 , 0 , 360) ;
        CGContextDrawPath( ctx , mctx->GetDrawingMode() ) ;
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
    if ( logical_func == wxNO_OP )
        return TRUE ;
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }

    wxMemoryDC* memdc = dynamic_cast<wxMemoryDC*>(source) ;
    if ( memdc )
    {
        wxBitmap blit = memdc->GetSelectedObject() ;
        wxASSERT_MSG( blit.Ok() , wxT("Invalid bitmap for blitting") ) ;

        wxCoord xxdest = XLOG2DEVMAC(xdest);
        wxCoord yydest = YLOG2DEVMAC(ydest);
        wxCoord ww = XLOG2DEVREL(width);
        wxCoord hh = YLOG2DEVREL(height);

        wxCoord bmpwidth = blit.GetWidth();
        wxCoord bmpheight = blit.GetHeight();
        
        if ( xsrc != 0 || ysrc != 0 || bmpwidth != width || bmpheight != height )
        {
            wxRect subrect( xsrc, ysrc, width , height ) ;
            blit = blit.GetSubBitmap( subrect ) ;
        }
        
        CGContextRef cg = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
        CGImageRef image = (CGImageRef)( blit.CGImageCreate() ) ;
        HIRect r = CGRectMake( xxdest , yydest , ww , hh ) ;
        HIViewDrawCGImage( cg , &r , image ) ;
        CGImageRelease( image ) ;
           
    }
    else
    {
        wxFAIL_MSG( wxT("Blitting is only supported from bitmap contexts") ) ;
    }
    return TRUE;
}

void  wxDC::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y,
                              double angle)
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoDrawRotatedText  Invalid window dc") );

    if ( str.Length() == 0 )
        return ;
    
    wxCHECK_RET( m_macATSUIStyle != NULL , wxT("No valid font set") ) ;
    
    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;
    UniCharCount chars = str.Length() ;
    UniChar* ubuf = NULL ;
#if SIZEOF_WCHAR_T == 4
	wxMBConvUTF16BE converter ;
#if wxUSE_UNICODE
	size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 ) ;
	ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
	converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 ) ;
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
	size_t unicharlen = converter.WC2MB( NULL , wchar.data()  , 0 ) ;
	ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
	converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 ) ;
#endif
    chars = unicharlen / 2 ;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str() ;
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
    chars = wxWcslen( wchar.data() ) ;
    ubuf = (UniChar*) wchar.data() ;
#endif
#endif

    int drawX = XLOG2DEVMAC(x) ;
    int drawY = YLOG2DEVMAC(y) ;

    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the rotated text") );
    int iAngle = int( angle );
    

    
    if ( abs(iAngle) > 0 )
    {
        Fixed atsuAngle = IntToFixed( iAngle ) ;
        ATSUAttributeTag atsuTags[] =
        {
            kATSULineRotationTag ,
        } ;
        ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
        {
            sizeof( Fixed ) ,
        } ;
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
        {
            &atsuAngle ,
        } ;
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues ) ;
    }
    {
        CGContextRef cgContext = dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ;
        ATSUAttributeTag atsuTags[] =
        {
            kATSUCGContextTag ,
        } ;
        ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
        {
            sizeof( CGContextRef ) ,
        } ;
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
        {
            &cgContext ,
        } ;
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues ) ;
    }

    ATSUTextMeasurement textBefore ;
    ATSUTextMeasurement textAfter ;
    ATSUTextMeasurement ascent ;
    ATSUTextMeasurement descent ;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &ascent , &descent );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );
    
    Rect rect ;
    
    if ( m_backgroundMode == wxSOLID )
    {
        wxGraphicPath* path = m_graphicContext->CreatePath() ;
        path->MoveToPoint( 
            drawX , 
            drawY ) ;
        path->AddLineToPoint( 
            (int) (drawX + sin(angle/RAD2DEG) * FixedToInt(ascent + descent)) , 
            (int) (drawY + cos(angle/RAD2DEG) * FixedToInt(ascent + descent)) ) ;
        path->AddLineToPoint( 
            (int) (drawX + sin(angle/RAD2DEG) * FixedToInt(ascent + descent ) + cos(angle/RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY + cos(angle/RAD2DEG) * FixedToInt(ascent + descent) - sin(angle/RAD2DEG) * FixedToInt(textAfter)) ) ;
        path->AddLineToPoint( 
            (int) (drawX + cos(angle/RAD2DEG) * FixedToInt(textAfter)) , 
            (int) (drawY - sin(angle/RAD2DEG) * FixedToInt(textAfter)) ) ;
            
        m_graphicContext->FillPath( path , m_textBackgroundColour ) ;
        delete path ;
    }

    drawX += (int)(sin(angle/RAD2DEG) * FixedToInt(ascent));
    drawY += (int)(cos(angle/RAD2DEG) * FixedToInt(ascent));

    status = ::ATSUMeasureTextImage( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(drawX) , IntToFixed(drawY) , &rect );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    CGContextSaveGState(dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext());    
    CGContextTranslateCTM(dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext(), drawX, drawY);
    CGContextScaleCTM(dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext(), 1, -1);
    status = ::ATSUDrawText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(0) , IntToFixed(0) );
    wxASSERT_MSG( status == noErr , wxT("couldn't draw the rotated text") );
    CGContextRestoreGState( dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ) ;

    CalcBoundingBox(XDEV2LOG(rect.left), YDEV2LOG(rect.top) );
    CalcBoundingBox(XDEV2LOG(rect.right), YDEV2LOG(rect.bottom) );

    ::ATSUDisposeTextLayout(atsuLayout);
#if SIZEOF_WCHAR_T == 4
    free( ubuf ) ;
#endif
}

void  wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawText  Invalid DC"));
    DoDrawRotatedText( strtext , x , y , 0.0 ) ;
}

bool  wxDC::CanGetTextExtent() const
{
    wxCHECK_MSG(Ok(), false, wxT("Invalid DC"));
    return true ;
}

void  wxDC::DoGetTextExtent( const wxString &str, wxCoord *width, wxCoord *height,
                            wxCoord *descent, wxCoord *externalLeading ,
                            wxFont *theFont ) const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxFont formerFont = m_font ;
    if ( theFont )
    {
        // work around the constness
        *((wxFont*)(&m_font)) = *theFont ;
        MacInstallFont() ;   
    }

    if ( str.Length() == 0 )
        return ;
    
    wxCHECK_RET( m_macATSUIStyle != NULL , wxT("No valid font set") ) ;
    
    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;
    UniCharCount chars = str.Length() ;
    UniChar* ubuf = NULL ;
#if SIZEOF_WCHAR_T == 4
	wxMBConvUTF16BE converter ;
#if wxUSE_UNICODE
	size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 ) ;
	ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
	converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 ) ;
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
	size_t unicharlen = converter.WC2MB( NULL , wchar.data()  , 0 ) ;
	ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
	converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 ) ;
#endif
    chars = unicharlen / 2 ;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str() ;
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
    chars = wxWcslen( wchar.data() ) ;
    ubuf = (UniChar*) wchar.data() ;
#endif
#endif


    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );
    
    ATSUTextMeasurement textBefore ;
    ATSUTextMeasurement textAfter ;
    ATSUTextMeasurement textAscent ;
    ATSUTextMeasurement textDescent ;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &textAscent , &textDescent );
        
    if ( height )
        *height = YDEV2LOGREL( FixedToInt(textAscent + textDescent) ) ;
    if ( descent )
        *descent =YDEV2LOGREL( FixedToInt(textDescent) );
    if ( externalLeading )
        *externalLeading = 0 ;
    if ( width )
        *width = XDEV2LOGREL( FixedToInt(textAfter - textBefore) ) ;
    
    ::ATSUDisposeTextLayout(atsuLayout);
#if SIZEOF_WCHAR_T == 4
    free( ubuf ) ;
#endif
    if ( theFont )
    {
        // work around the constness
        *((wxFont*)(&m_font)) = formerFont ;
        MacInstallFont() ;
    }
}


bool wxDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxCHECK_MSG(Ok(), false, wxT("Invalid DC"));

    widths.Empty();
    widths.Add(0, text.Length());

    if (text.Length() == 0)
        return false;
    
    wxFAIL_MSG( wxT("Unimplemented for Core Graphics") ) ;
    
    return false;
}

wxCoord   wxDC::GetCharWidth(void) const
{
    wxCoord width ;
    DoGetTextExtent(wxT("g") , &width , NULL , NULL , NULL , NULL ) ;
    return width ;
}

wxCoord   wxDC::GetCharHeight(void) const
{
    wxCoord height ;
    DoGetTextExtent(wxT("g") , NULL , &height , NULL , NULL , NULL ) ;
    return height ;
}

void  wxDC::Clear(void)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

    if ( m_backgroundBrush.Ok() && m_backgroundBrush.GetStyle() != wxTRANSPARENT)
    {      
        HIRect rect = CGRectMake( -10000 , -10000 , 20000 , 20000 ) ;
        switch( m_backgroundBrush.MacGetBrushKind() )
        {
            case kwxMacBrushTheme :
                {
                }
            break ;
            case kwxMacBrushThemeBackground :
                {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
                    if ( HIThemeDrawBackground )
                    {
                        HIThemeBackgroundDrawInfo drawInfo ;
                        drawInfo.version = 0 ;
                        drawInfo.state = kThemeStateActive ;
                        drawInfo.kind = m_backgroundBrush.MacGetThemeBackground(NULL) ;
                        if ( drawInfo.kind == kThemeBackgroundMetal )
                            HIThemeDrawBackground( &rect , &drawInfo, dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ,
                                kHIThemeOrientationNormal) ;
                            HIThemeApplyBackground( &rect , &drawInfo, dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() ,
                                kHIThemeOrientationNormal) ;
                    }
#endif
                }
            break ;
            case kwxMacBrushColour :
            {
                RGBColor col = MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel()) ;
                CGContextSetRGBFillColor( dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
                CGContextFillRect(dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext(), rect);

                // reset to normal value
                col = MAC_WXCOLORREF( GetBrush().GetColour().GetPixel() ) ;
                CGContextSetRGBFillColor( dynamic_cast<wxMacCGContext*>(m_graphicContext)->GetNativeContext() , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
            }
            break ;
        }
    }
}

void wxDC::MacInstallFont() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

    if( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }

    OSStatus status = noErr ;
    status = ATSUCreateAndCopyStyle( (ATSUStyle) m_font.MacGetATSUStyle() , (ATSUStyle*) &m_macATSUIStyle ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't set create ATSU style") ) ;

    Fixed atsuSize = IntToFixed( int(m_scaleY * m_font.MacGetFontSize()) ) ;
    RGBColor atsuColor = MAC_WXCOLORREF( m_textForegroundColour.GetPixel() ) ;
    ATSUAttributeTag atsuTags[] =
    {
            kATSUSizeTag ,
            kATSUColorTag ,
    } ;
    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            sizeof( Fixed ) ,
            sizeof( RGBColor ) ,
    } ;
//    Boolean kTrue = true ;
//    Boolean kFalse = false ;

//    ATSUVerticalCharacterType kHorizontal = kATSUStronglyHorizontal;
    ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
            &atsuSize ,
            &atsuColor ,
    } ;
    status = ::ATSUSetAttributes((ATSUStyle)m_macATSUIStyle, sizeof(atsuTags)/sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't Modify ATSU style") ) ;
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

#endif // wxMAC_USE_CORE_GRAPHICS

