/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dccg.cpp
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dc.h"

#if wxMAC_USE_CORE_GRAPHICS

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/dcmemory.h"
    #include "wx/dcprint.h"
    #include "wx/region.h"
    #include "wx/image.h"
#endif

#include "wx/mac/uma.h"


#ifdef __MSL__
    #if __MSL__ >= 0x6000
        #include "math.h"
        // in case our functions were defined outside std, we make it known all the same
        namespace std { }
        using namespace std ;
    #endif
#endif

#include "wx/mac/private.h"

IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

#ifndef wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
#define wxMAC_USE_CORE_GRAPHICS_BLEND_MODES 0
#endif

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
typedef float CGFloat ;
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#if !defined( __DARWIN__ ) || defined(__MWERKS__)
#ifndef M_PI
const double M_PI = 3.14159265358979 ;
#endif
#endif

const double RAD2DEG = 180.0 / M_PI;
const short kEmulatedMode = -1 ;
const short kUnsupportedMode = -2 ;

extern TECObjectRef s_TECNativeCToUnicode ;

#ifndef __LP64__

// TODO: update
// The textctrl implementation still needs that (needs what?) for the non-HIView implementation
//
wxMacWindowClipper::wxMacWindowClipper( const wxWindow* win ) :
    wxMacPortSaver( (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) )
{
    m_newPort = (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) ;
    m_formerClip = NewRgn() ;
    m_newClip = NewRgn() ;
    GetClip( m_formerClip ) ;

    if ( win )
    {
        // guard against half constructed objects, this just leads to a empty clip
        if ( win->GetPeer() )
        {
            int x = 0 , y = 0;
            win->MacWindowToRootWindow( &x, &y ) ;

            // get area including focus rect
            CopyRgn( (RgnHandle) ((wxWindow*)win)->MacGetVisibleRegion(true).GetWXHRGN() , m_newClip ) ;
            if ( !EmptyRgn( m_newClip ) )
                OffsetRgn( m_newClip , x , y ) ;
        }

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
    m_newPort = (GrafPtr) GetWindowPort( (WindowRef) win->MacGetTopLevelWindowRef() ) ;
    GetThemeDrawingState( &m_themeDrawingState ) ;
}

wxMacWindowStateSaver::~wxMacWindowStateSaver()
{
    SetPort( m_newPort ) ;
    SetThemeDrawingState( m_themeDrawingState , true ) ;
}

// minimal implementation only used for appearance drawing < 10.3

wxMacPortSetter::wxMacPortSetter( const wxDC* dc ) :
    m_ph( (GrafPtr) dc->m_macPort )
{
    wxASSERT( dc->Ok() ) ;
    m_dc = dc ;

//    dc->MacSetupPort(&m_ph) ;
}

wxMacPortSetter::~wxMacPortSetter()
{
//    m_dc->MacCleanupPort(&m_ph) ;
}
#endif

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

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicPath implementation
//-----------------------------------------------------------------------------

wxMacCGPath::wxMacCGPath()
{
    m_path = CGPathCreateMutable() ;
}

wxMacCGPath::~wxMacCGPath()
{
    CGPathRelease( m_path ) ;
}

// opens (starts) a new subpath
void wxMacCGPath::MoveToPoint( wxCoord x1 , wxCoord y1 )
{
    CGPathMoveToPoint( m_path , NULL , x1 , y1 ) ;
}

void wxMacCGPath::AddLineToPoint( wxCoord x1 , wxCoord y1 )
{
    CGPathAddLineToPoint( m_path , NULL , x1 , y1 ) ;
}

void wxMacCGPath::AddQuadCurveToPoint( wxCoord cx1, wxCoord cy1, wxCoord x1, wxCoord y1 )
{
    CGPathAddQuadCurveToPoint( m_path , NULL , cx1 , cy1 , x1 , y1 );
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

void wxMacCGPath::AddArcToPoint( wxCoord x1, wxCoord y1 , wxCoord x2, wxCoord y2, wxCoord r ) 
{
    CGPathAddArcToPoint( m_path, NULL , x1, y1, x2, y2, r); 
}

void wxMacCGPath::AddArc( wxCoord x, wxCoord y, wxCoord r, double startAngle, double endAngle, bool clockwise ) 
{
    CGPathAddArc( m_path, NULL , x, y, r, startAngle, endAngle, clockwise); 
}

//-----------------------------------------------------------------------------
// wxGraphicContext implementation
//-----------------------------------------------------------------------------

wxMacCGContext::wxMacCGContext( CGrafPtr port )
{
    m_qdPort = port ;
    m_cgContext = NULL ;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL ;
}

wxMacCGContext::wxMacCGContext( CGContextRef cgcontext )
{
    m_qdPort = NULL ;
    m_cgContext = cgcontext ;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL ;
    CGContextSaveGState( m_cgContext ) ;
    CGContextSaveGState( m_cgContext ) ;
}

wxMacCGContext::wxMacCGContext()
{
    m_qdPort = NULL ;
    m_cgContext = NULL ;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL ;
}

wxMacCGContext::~wxMacCGContext()
{
    if ( m_cgContext )
    {
        CGContextSynchronize( m_cgContext ) ;
        CGContextRestoreGState( m_cgContext ) ;
        CGContextRestoreGState( m_cgContext ) ;
    }
#ifndef __LP64__
    if ( m_qdPort )
        QDEndCGContext( m_qdPort, &m_cgContext ) ;
#endif
}


void wxMacCGContext::Clip( const wxRegion &region )
{
//    ClipCGContextToRegion ( m_cgContext, &bounds , (RgnHandle) dc->m_macCurrentClipRgn ) ;
}

void wxMacCGContext::StrokePath( const wxGraphicPath *p )
{
    const wxMacCGPath* path = dynamic_cast< const wxMacCGPath*>( p ) ;

    int width = m_pen.GetWidth();
    if ( width == 0 )
        width = 1 ;
    if ( m_pen.GetStyle() == wxTRANSPARENT )
        width = 0 ;
        
    bool offset = ( width % 2 ) == 1 ; 

    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    CGContextAddPath( m_cgContext , path->GetPath() ) ;
    CGContextStrokePath( m_cgContext ) ;

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
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

    int width = m_pen.GetWidth();
    if ( width == 0 )
        width = 1 ;
    if ( m_pen.GetStyle() == wxTRANSPARENT )
        width = 0 ;
        
    bool offset = ( width % 2 ) == 1 ; 

    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    CGContextAddPath( m_cgContext , path->GetPath() ) ;
    CGContextDrawPath( m_cgContext , mode ) ;

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
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
    CGContextRef cg = GetNativeContext() ;
    cg = NULL ;

    return new wxMacCGPath() ;
}

// in case we only got a QDPort only create a cgref now

CGContextRef wxMacCGContext::GetNativeContext()
{
    if ( m_cgContext == NULL )
    {
        Rect bounds ;
        OSStatus status = noErr;
#ifndef __LP64__
        GetPortBounds( (CGrafPtr) m_qdPort , &bounds ) ;
        status = QDBeginCGContext((CGrafPtr) m_qdPort , &m_cgContext) ;
#endif
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
    // we allow either setting or clearing but not replacing
    wxASSERT( m_cgContext == NULL || cg == NULL ) ;

    if ( cg )
        CGContextSaveGState( cg ) ;
    m_cgContext = cg ;
}

void wxMacCGContext::Translate( wxCoord dx , wxCoord dy ) 
{
    CGContextTranslateCTM( m_cgContext, dx, dy );
}

void wxMacCGContext::Scale( wxCoord xScale , wxCoord yScale )
{
    CGContextScaleCTM( m_cgContext , xScale , yScale ) ;
}

void wxMacCGContext::DrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, wxCoord w, wxCoord h ) 
{
    CGImageRef image = (CGImageRef)( bmp.CGImageCreate() ) ;
    HIRect r = CGRectMake( 0 , 0 , w , h ); 

    CGContextSaveGState( m_cgContext );
    CGContextTranslateCTM( m_cgContext, x , y + h );
    CGContextScaleCTM( m_cgContext, 1, -1 );
        
    // in case image is a mask, set the foreground color
    CGContextSetRGBFillColor( m_cgContext , m_textForegroundColor.Red() / 255.0 , m_textForegroundColor.Green() / 255.0 , 
        m_textForegroundColor.Blue() / 255.0 , m_textForegroundColor.Alpha() / 255.0 ) ;
    CGContextDrawImage( m_cgContext, r, image );
    CGContextRestoreGState( m_cgContext );

    CGImageRelease( image ) ;
}

void wxMacCGContext::DrawIcon( const wxIcon &icon, wxCoord x, wxCoord y, wxCoord w, wxCoord h ) 
{
    CGRect r = CGRectMake( 0 , 0 , w , h ) ;
    CGContextSaveGState( m_cgContext );
    CGContextTranslateCTM( m_cgContext, x , y + h );
    CGContextScaleCTM( m_cgContext, 1, -1 );
    PlotIconRefInContext( m_cgContext , &r , kAlignNone , kTransformNone ,
        NULL , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) ) ;
    CGContextRestoreGState( m_cgContext ) ;
}

void wxMacCGContext::PushState()
{
    CGContextSaveGState( m_cgContext );
}

void wxMacCGContext::PopState() 
{
    CGContextRestoreGState( m_cgContext );
}

void wxMacCGContext::SetTextColor( const wxColour &col ) 
{
    m_textForegroundColor = col ;
}

#pragma mark -
#pragma mark wxMacCGPattern, ImagePattern, HatchPattern classes

// CGPattern wrapper class: always allocate on heap, never call destructor

class wxMacCGPattern
{
public :
    wxMacCGPattern() {}

    // is guaranteed to be called only with a non-Null CGContextRef
    virtual void Render( CGContextRef ctxRef ) = 0 ;

    operator CGPatternRef() const { return m_patternRef ; }

protected :
    virtual ~wxMacCGPattern()
    {
        // as this is called only when the m_patternRef is been released;
        // don't release it again
    }

    static void _Render( void *info, CGContextRef ctxRef )
    {
        wxMacCGPattern* self = (wxMacCGPattern*) info ;
        if ( self && ctxRef )
            self->Render( ctxRef ) ;
    }

    static void _Dispose( void *info )
    {
        wxMacCGPattern* self = (wxMacCGPattern*) info ;
        delete self ;
    }

    CGPatternRef m_patternRef ;

    static const CGPatternCallbacks ms_Callbacks ;
} ;

const CGPatternCallbacks wxMacCGPattern::ms_Callbacks = { 0, &wxMacCGPattern::_Render, &wxMacCGPattern::_Dispose };

class ImagePattern : public wxMacCGPattern
{
public :
    ImagePattern( const wxBitmap* bmp , CGAffineTransform transform )
    {
        wxASSERT( bmp && bmp->Ok() ) ;

        Init( (CGImageRef) bmp->CGImageCreate() , transform ) ;
    }

    // ImagePattern takes ownership of CGImageRef passed in
    ImagePattern( CGImageRef image , CGAffineTransform transform )
    {
        if ( image )
            CFRetain( image ) ;

        Init( image , transform ) ;
    }

    virtual void Render( CGContextRef ctxRef )
    {
        if (m_image != NULL)
            HIViewDrawCGImage( ctxRef, &m_imageBounds, m_image );
    }

protected :
    void Init( CGImageRef image, CGAffineTransform transform )
    {
        m_image = image ;
        if ( m_image )
        {
            m_imageBounds = CGRectMake( 0.0, 0.0, (CGFloat)CGImageGetWidth( m_image ), (CGFloat)CGImageGetHeight( m_image ) ) ;
            m_patternRef = CGPatternCreate(
                this , m_imageBounds, transform ,
                m_imageBounds.size.width, m_imageBounds.size.height,
                kCGPatternTilingNoDistortion, true , &wxMacCGPattern::ms_Callbacks ) ;
        }
    }

    virtual ~ImagePattern()
    {
        if ( m_image )
            CGImageRelease( m_image ) ;
    }

    CGImageRef m_image ;
    CGRect m_imageBounds ;
} ;

class HatchPattern : public wxMacCGPattern
{
public :
    HatchPattern( int hatchstyle, CGAffineTransform transform )
    {
        m_hatch = hatchstyle ;
        m_imageBounds = CGRectMake( 0.0, 0.0, 8.0 , 8.0 ) ;
        m_patternRef = CGPatternCreate(
            this , m_imageBounds, transform ,
            m_imageBounds.size.width, m_imageBounds.size.height,
            kCGPatternTilingNoDistortion, false , &wxMacCGPattern::ms_Callbacks ) ;
    }

    void StrokeLineSegments( CGContextRef ctxRef , const CGPoint pts[] , size_t count )
    {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            CGContextStrokeLineSegments( ctxRef , pts , count ) ;
        }
        else
#endif
        {
            CGContextBeginPath( ctxRef );
            for (size_t i = 0; i < count; i += 2)
            {
                CGContextMoveToPoint(ctxRef, pts[i].x, pts[i].y);
                CGContextAddLineToPoint(ctxRef, pts[i+1].x, pts[i+1].y);
            }
            CGContextStrokePath(ctxRef);
        }
    }

    virtual void Render( CGContextRef ctxRef )
    {
        switch ( m_hatch )
        {
            case wxBDIAGONAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 8.0 , 0.0 } , { 0.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 2 ) ;
                }
                break ;

            case wxCROSSDIAG_HATCH :
                {
                    CGPoint pts[] =
                    {
                        { 0.0 , 0.0 } , { 8.0 , 8.0 } ,
                        { 8.0 , 0.0 } , { 0.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 4 ) ;
                }
                break ;

            case wxFDIAGONAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 0.0 } , { 8.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 2 ) ;
                }
                break ;

            case wxCROSS_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 4 ) ;
                }
                break ;

            case wxHORIZONTAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 ) ;
                }
                break ;

            case wxVERTICAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 ) ;
                }
                break ;

            default:
                break;
        }
    }

protected :
    virtual ~HatchPattern() {}

    CGRect      m_imageBounds ;
    int         m_hatch ;
};

#pragma mark -

void wxMacCGContext::SetPen( const wxPen &pen )
{
    m_pen = pen ;
    if ( m_cgContext == NULL )
        return ;

    bool fill = m_brush.GetStyle() != wxTRANSPARENT ;
    bool stroke = pen.GetStyle() != wxTRANSPARENT ;

#if 0
    // we can benchmark performance; should go into a setting eventually
    CGContextSetShouldAntialias( m_cgContext , false ) ;
#endif

    if ( fill | stroke )
    {
        // set up brushes
        m_mode = kCGPathFill ; // just a default

        if ( stroke )
        {
            CGContextSetRGBStrokeColor( m_cgContext , pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 , 
                    pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 ) ;

            // TODO: * m_dc->m_scaleX
            CGFloat penWidth = pen.GetWidth();
            if (penWidth <= 0.0)
                penWidth = 0.1;
            CGContextSetLineWidth( m_cgContext , penWidth ) ;

            CGLineCap cap ;
            switch ( pen.GetCap() )
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

            CGLineJoin join ;
            switch ( pen.GetJoin() )
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

            m_mode = kCGPathStroke ;
            int count = 0 ;

            const CGFloat *lengths = NULL ;
            CGFloat *userLengths = NULL ;

            const CGFloat dashUnit = penWidth < 1.0 ? 1.0 : penWidth;

            const CGFloat dotted[] = { dashUnit , dashUnit + 2.0 };
            const CGFloat short_dashed[] = { 9.0 , 6.0 };
            const CGFloat dashed[] = { 19.0 , 9.0 };
            const CGFloat dotted_dashed[] = { 9.0 , 6.0 , 3.0 , 3.0 };

            switch ( pen.GetStyle() )
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
                    if ((dashes != NULL) && (count > 0))
                    {
                        userLengths = new CGFloat[count] ;
                        for ( int i = 0 ; i < count ; ++i )
                        {
                            userLengths[i] = dashes[i] * dashUnit ;

                            if ( i % 2 == 1 && userLengths[i] < dashUnit + 2.0 )
                                userLengths[i] = dashUnit + 2.0 ;
                            else if ( i % 2 == 0 && userLengths[i] < dashUnit )
                                userLengths[i] = dashUnit ;
                        }
                    }
                    lengths = userLengths ;
                    break ;

                case wxSTIPPLE :
                    {
                        CGFloat  alphaArray[1] = { 1.0 } ;
                        wxBitmap* bmp = pen.GetStipple() ;
                        if ( bmp && bmp->Ok() )
                        {
                            wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( NULL ) ) ;
                            CGContextSetStrokeColorSpace( m_cgContext , patternSpace ) ;
                            wxMacCFRefHolder<CGPatternRef> pattern( *( new ImagePattern( bmp , CGContextGetCTM( m_cgContext ) ) ) );
                            CGContextSetStrokePattern( m_cgContext, pattern , alphaArray ) ;
                        }
                    }
                    break ;

                default :
                    {
                        wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) ) ;
                        CGContextSetStrokeColorSpace( m_cgContext , patternSpace ) ;
                        wxMacCFRefHolder<CGPatternRef> pattern( *( new HatchPattern( pen.GetStyle() , CGContextGetCTM( m_cgContext ) ) ) );

                        CGFloat  colorArray[4] = { pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 , 
                            pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 } ;

                        CGContextSetStrokePattern( m_cgContext, pattern , colorArray ) ;
                    }
                    break ;
            }

            if ((lengths != NULL) && (count > 0))
            {
                CGContextSetLineDash( m_cgContext , 0 , lengths , count ) ;
                // force the line cap, otherwise we get artifacts (overlaps) and just solid lines
                cap = kCGLineCapButt ;
            }
            else
            {
               CGContextSetLineDash( m_cgContext , 0 , NULL , 0 ) ;
            }

            CGContextSetLineCap( m_cgContext , cap ) ;
            CGContextSetLineJoin( m_cgContext , join ) ;

            delete[] userLengths ;
        }

        if ( fill && stroke )
            m_mode = kCGPathFillStroke ;
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
            if ( brush.GetStyle() == wxSOLID )
            {
                CGContextSetRGBFillColor( m_cgContext , brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 , 
                    brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 ) ;
            }
            else if ( brush.IsHatch() )
            {
                wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) ) ;
                CGContextSetFillColorSpace( m_cgContext , patternSpace ) ;
                wxMacCFRefHolder<CGPatternRef> pattern( *( new HatchPattern( brush.GetStyle() , CGContextGetCTM( m_cgContext ) ) ) );

                CGFloat  colorArray[4] = { brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 , 
                    brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 } ;

                CGContextSetFillPattern( m_cgContext, pattern , colorArray ) ;
            }
            else
            {
                // now brush is a bitmap
                CGFloat  alphaArray[1] = { 1.0 } ;
                wxBitmap* bmp = brush.GetStipple() ;
                if ( bmp && bmp->Ok() )
                {
                    wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( NULL ) ) ;
                    CGContextSetFillColorSpace( m_cgContext , patternSpace ) ;
                    wxMacCFRefHolder<CGPatternRef> pattern( *( new ImagePattern( bmp , CGContextGetCTM( m_cgContext ) ) ) );
                    CGContextSetFillPattern( m_cgContext, pattern , alphaArray ) ;
                }
            }

            m_mode = kCGPathFill ;
        }

        if ( fill && stroke )
            m_mode = kCGPathFillStroke ;
        else if ( stroke )
            m_mode = kCGPathStroke ;
    }
}

void wxMacCGContext::DrawText( const wxString &str, wxCoord x, wxCoord y, double angle ) 
{
    OSStatus status = noErr ;
    ATSUTextLayout atsuLayout ;
    UniCharCount chars = str.length() ;
    UniChar* ubuf = NULL ;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter ;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 ) ;
    ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 ) ;
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 ) ;
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

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the rotated text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true ) ;
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    int iAngle = int( angle );
    if ( abs(iAngle) > 0 )
    {
        Fixed atsuAngle = IntToFixed( iAngle ) ;
        ATSUAttributeTag atsuTags[] =
        {
            kATSULineRotationTag ,
        } ;
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( Fixed ) ,
        } ;
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &atsuAngle ,
        } ;
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues ) ;
    }

    {
        ATSUAttributeTag atsuTags[] =
        {
            kATSUCGContextTag ,
        } ;
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( CGContextRef ) ,
        } ;
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &m_cgContext ,
        } ;
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues ) ;
    }

    ATSUTextMeasurement textBefore, textAfter ;
    ATSUTextMeasurement ascent, descent ;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &ascent , &descent );

    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    Rect rect ;
/*
    // TODO
    if ( m_backgroundMode == wxSOLID )
    {
        wxGraphicPath* path = m_graphicContext->CreatePath() ;
        path->MoveToPoint( drawX , drawY ) ;
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent)) ) ;
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent ) + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent) - sin(angle / RAD2DEG) * FixedToInt(textAfter)) ) ;
        path->AddLineToPoint(
            (int) (drawX + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY - sin(angle / RAD2DEG) * FixedToInt(textAfter)) ) ;

        m_graphicContext->FillPath( path , m_textBackgroundColour ) ;
        delete path ;
    }
*/
    x += (int)(sin(angle / RAD2DEG) * FixedToInt(ascent));
    y += (int)(cos(angle / RAD2DEG) * FixedToInt(ascent));

    status = ::ATSUMeasureTextImage( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(x) , IntToFixed(y) , &rect );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    CGContextSaveGState(m_cgContext);
    CGContextTranslateCTM(m_cgContext, x, y);
    CGContextScaleCTM(m_cgContext, 1, -1);
    status = ::ATSUDrawText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(0) , IntToFixed(0) );

    wxASSERT_MSG( status == noErr , wxT("couldn't draw the rotated text") );

    CGContextRestoreGState(m_cgContext) ;

    ::ATSUDisposeTextLayout(atsuLayout);

#if SIZEOF_WCHAR_T == 4
    free( ubuf ) ;
#endif
}
    
void wxMacCGContext::GetTextExtent( const wxString &str, wxCoord *width, wxCoord *height,
                            wxCoord *descent, wxCoord *externalLeading ) const
{
    wxCHECK_RET( m_macATSUIStyle != NULL, wxT("wxDC(cg)::DoGetTextExtent - no valid font set") ) ;

    OSStatus status = noErr ;
    
    ATSUTextLayout atsuLayout ;
    UniCharCount chars = str.length() ;
    UniChar* ubuf = NULL ;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter ;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 ) ;
    ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 ) ;
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal ) ;
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 ) ;
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

    ATSUTextMeasurement textBefore, textAfter ;
    ATSUTextMeasurement textAscent, textDescent ;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &textAscent , &textDescent );

    if ( height )
        *height = FixedToInt(textAscent + textDescent) ;
    if ( descent )
        *descent = FixedToInt(textDescent) ;
    if ( externalLeading )
        *externalLeading = 0 ;
    if ( width )
        *width = FixedToInt(textAfter - textBefore) ;

    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxMacCGContext::GetPartialTextExtents(const wxString& text, wxArrayInt& widths) const 
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return ;

    ATSUTextLayout atsuLayout ;
    UniCharCount chars = text.length() ;
    UniChar* ubuf = NULL ;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter ;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , text.wc_str() , 0 ) ;
    ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
    converter.WC2MB( (char*) ubuf , text.wc_str(), unicharlen + 2 ) ;
#else
    const wxWCharBuffer wchar = text.wc_str( wxConvLocal ) ;
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 ) ;
    ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 ) ;
#endif
    chars = unicharlen / 2 ;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) text.wc_str() ;
#else
    wxWCharBuffer wchar = text.wc_str( wxConvLocal ) ;
    chars = wxWcslen( wchar.data() ) ;
    ubuf = (UniChar*) wchar.data() ;
#endif
#endif

    OSStatus status;
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;

    for ( int pos = 0; pos < (int)chars; pos ++ )
    {
        unsigned long actualNumberOfBounds = 0;
        ATSTrapezoid glyphBounds;

        // We get a single bound, since the text should only require one. If it requires more, there is an issue
        OSStatus result;
        result = ATSUGetGlyphBounds( atsuLayout, 0, 0, kATSUFromTextBeginning, pos + 1,
            kATSUseDeviceOrigins, 1, &glyphBounds, &actualNumberOfBounds );
        if (result != noErr || actualNumberOfBounds != 1 )
            return ;

        widths[pos] = FixedToInt( glyphBounds.upperRight.x - glyphBounds.upperLeft.x );
        //unsigned char uch = s[i];
    }

    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxMacCGContext::SetFont( const wxFont &font ) 
{
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }

    if ( font.Ok() )
    {
        OSStatus status ;

        status = ATSUCreateAndCopyStyle( (ATSUStyle) font.MacGetATSUStyle() , (ATSUStyle*) &m_macATSUIStyle ) ;

        wxASSERT_MSG( status == noErr, wxT("couldn't create ATSU style") ) ;

        // we need the scale here ...

        Fixed atsuSize = IntToFixed( int( /*m_scaleY*/ 1 * font.MacGetFontSize()) ) ;
        RGBColor atsuColor = MAC_WXCOLORREF( m_textForegroundColor.GetPixel() ) ;
        ATSUAttributeTag atsuTags[] =
        {
                kATSUSizeTag ,
                kATSUColorTag ,
        } ;
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
                sizeof( Fixed ) ,
                sizeof( RGBColor ) ,
        } ;
        ATSUAttributeValuePtr atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
                &atsuSize ,
                &atsuColor ,
        } ;

        status = ::ATSUSetAttributes(
            (ATSUStyle)m_macATSUIStyle, sizeof(atsuTags) / sizeof(ATSUAttributeTag) ,
            atsuTags, atsuSizes, atsuValues);

        wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") ) ;
    }
}


#pragma mark -

wxDC::wxDC()
{
    m_ok = false ;
    m_colour = true;
    m_mm_to_pix_x = mm2pt;
    m_mm_to_pix_y = mm2pt;

    m_externalDeviceOriginX = 0;
    m_externalDeviceOriginY = 0;
    m_logicalScaleX = 1.0;
    m_logicalScaleY = 1.0;
    m_userScaleX = 1.0;
    m_userScaleY = 1.0;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
    m_needComputeScaleX =
    m_needComputeScaleY = false;

    m_macPort = 0 ;
    m_macLocalOrigin.x =
    m_macLocalOrigin.y = 0 ;

    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;

    m_macATSUIStyle = NULL ;
    m_graphicContext = NULL ;
}

wxDC::~wxDC()
{
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }

    delete m_graphicContext ;
}

void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawBitmap - invalid DC") );
    wxCHECK_RET( bmp.Ok(), wxT("wxDC(cg)::DoDrawBitmap - invalid bitmap") );

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord w = bmp.GetWidth();
    wxCoord h = bmp.GetHeight();
    wxCoord ww = XLOG2DEVREL(w);
    wxCoord hh = YLOG2DEVREL(h);

    if ( bmp.GetDepth()==1 )
    {
        wxGraphicPath* path = m_graphicContext->CreatePath() ;
        path->AddRectangle( xx , yy , ww , hh ) ;
        m_graphicContext->FillPath( path , m_textBackgroundColour, wxODDEVEN_RULE) ;
        delete path;
        m_graphicContext->DrawBitmap( bmp, xx , yy , ww , hh ) ;
    }
    else
    {
        m_graphicContext->DrawBitmap( bmp, xx , yy , ww , hh ) ;
    }
}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawIcon - invalid DC") );
    wxCHECK_RET( icon.Ok(), wxT("wxDC(cg)::DoDrawIcon - invalid icon") );

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord w = icon.GetWidth();
    wxCoord h = icon.GetHeight();
    wxCoord ww = XLOG2DEVREL(w);
    wxCoord hh = YLOG2DEVREL(h);

    m_graphicContext->DrawIcon( icon , xx, yy, ww, hh ) ;
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoSetClippingRegion - invalid DC") );

    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(width);
    hh = YLOG2DEVREL(height);

    CGContextRef cgContext = ((wxMacCGContext*)(m_graphicContext))->GetNativeContext() ;
    CGRect clipRect = CGRectMake( xx , yy , ww, hh ) ;
    CGContextClipToRect( cgContext , clipRect ) ;

//    SetRectRgn( (RgnHandle) m_macCurrentClipRgn , xx , yy , xx + ww , yy + hh ) ;
//    SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;

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

    // TODO: as soon as we don't reset the context for each operation anymore
    // we have to update the context as well
}

void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoSetClippingRegionAsRegion - invalid DC") );

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
#if 0
        CopyRgn( (RgnHandle) region.GetWXHRGN() , (RgnHandle) m_macCurrentClipRgn ) ;
        if ( xx != x || yy != y )
            OffsetRgn( (RgnHandle) m_macCurrentClipRgn , xx - x , yy - y ) ;
        SectRgn( (RgnHandle)m_macCurrentClipRgn , (RgnHandle)m_macBoundaryClipRgn , (RgnHandle)m_macCurrentClipRgn ) ;
#endif

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

void wxDC::DestroyClippingRegion()
{
//    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;

    CGContextRef cgContext = ((wxMacCGContext*)(m_graphicContext))->GetNativeContext() ;
    CGContextRestoreGState( cgContext );
    CGContextSaveGState( cgContext );

    m_graphicContext->SetPen( m_pen ) ;
    m_graphicContext->SetBrush( m_brush ) ;

    m_clipping = false;
}

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w = 0, h = 0;

    GetSize( &w, &h );
    if (width)
        *width = long( double(w) / (m_scaleX * m_mm_to_pix_x) );
    if (height)
        *height = long( double(h) / (m_scaleY * m_mm_to_pix_y) );
}

void wxDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::SetTextForeground - invalid DC") );

    if ( col != m_textForegroundColour )
    {
        m_textForegroundColour = col;
        m_graphicContext->SetTextColor( col );
        // in the current implementation the font contains the text color
        m_graphicContext->SetFont(m_font);
    }
}

void wxDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::SetTextBackground - invalid DC") );

    m_textBackgroundColour = col;
}

void wxDC::SetMapMode( int mode )
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

    if (mode != wxMM_TEXT)
    {
        m_needComputeScaleX =
        m_needComputeScaleY = true;
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
    m_signY = (yBottomUp ? -1 :  1);
    ComputeScaleAndOrigin();
}

wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}

int wxDC::GetDepth() const
{
    return 32;
}

void wxDC::ComputeScaleAndOrigin()
{
    // CMB: copy scale to see if it changes
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
    m_deviceOriginX = m_externalDeviceOriginX;
    m_deviceOriginY = m_externalDeviceOriginY;

    // CMB: if scale has changed call SetPen to recalulate the line width
    if (m_scaleX != origScaleX || m_scaleY != origScaleY)
    {
        // this is a bit artificial, but we need to force wxDC to think
        // the pen has changed
        wxPen pen( GetPen() );

        m_pen = wxNullPen;
        SetPen( pen );
    }
}

void wxDC::SetPalette( const wxPalette& palette )
{
}

void wxDC::SetBackgroundMode( int mode )
{
    m_backgroundMode = mode ;
}

void wxDC::SetFont( const wxFont &font )
{
    m_font = font;
    if ( m_graphicContext )
        m_graphicContext->SetFont( font ) ;
}

void wxDC::SetPen( const wxPen &pen )
{
    if ( m_pen == pen )
        return ;
    
    m_pen = pen;
    if ( m_graphicContext )
    {
        if ( m_pen.GetStyle() == wxSOLID || m_pen.GetStyle() == wxTRANSPARENT )
        {
            m_graphicContext->SetPen( m_pen ) ;
        }
        else
        {
            // we have to compensate for moved device origins etc. otherwise patterned pens are standing still
            // eg when using a wxScrollWindow and scrolling around
            int origX = XLOG2DEVMAC( 0 ) ;
            int origY = YLOG2DEVMAC( 0 ) ;
            m_graphicContext->Translate( origX , origY ) ;
            m_graphicContext->SetPen( m_pen ) ;
            m_graphicContext->Translate( -origX , -origY ) ;
        }
    }
}

void wxDC::SetBrush( const wxBrush &brush )
{
    if (m_brush == brush)
        return;

    m_brush = brush;
    if ( m_graphicContext )
    {
        if ( brush.GetStyle() == wxSOLID || brush.GetStyle() == wxTRANSPARENT )
        {
            m_graphicContext->SetBrush( m_brush ) ;
        }
        else
        {
            // we have to compensate for moved device origins etc. otherwise patterned brushes are standing still
            // eg when using a wxScrollWindow and scrolling around
            int origX = XLOG2DEVMAC(0) ;
            int origY = YLOG2DEVMAC(0) ;
            m_graphicContext->Translate( origX , origY ) ;
            m_graphicContext->SetBrush( m_brush ) ;
            m_graphicContext->Translate( -origX , -origY ) ;
        }
    }
}

void wxDC::SetBackground( const wxBrush &brush )
{
    if (m_backgroundBrush == brush)
        return;

    m_backgroundBrush = brush;
    if (!m_backgroundBrush.Ok())
        return;
}

void wxDC::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return;

    m_logicalFunction = function ;
#if wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
    CGContextRef cgContext = ((wxMacCGContext*)(m_graphicContext))->GetNativeContext() ;
    if ( m_logicalFunction == wxCOPY )
        CGContextSetBlendMode( cgContext, kCGBlendModeNormal ) ;
    else if ( m_logicalFunction == wxINVERT )
        CGContextSetBlendMode( cgContext, kCGBlendModeExclusion ) ;
    else
        CGContextSetBlendMode( cgContext, kCGBlendModeNormal ) ;
#endif
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);

bool wxDC::DoFloodFill(wxCoord x, wxCoord y,
                       const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxDC::DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC(cg)::DoGetPixel - invalid DC") );

    RGBColor colour;
#ifndef __LP64__
    wxMacPortSaver helper((CGrafPtr)m_macPort) ;
    // NB: GetCPixel is a deprecated QD call, and a slow one at that
    GetCPixel(
        XLOG2DEVMAC(x) + m_macLocalOriginInPort.x - m_macLocalOrigin.x,
        YLOG2DEVMAC(y) + m_macLocalOriginInPort.y - m_macLocalOrigin.y, &colour );
#endif
    // convert from Mac colour to wx
    col->Set( colour.red >> 8, colour.green >> 8, colour.blue >> 8 );

    return true ;
}

void wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawLine - invalid DC") );

#if !wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
    if ( m_logicalFunction != wxCOPY )
        return ;
#endif

    wxCoord xx1 = XLOG2DEVMAC(x1) ;
    wxCoord yy1 = YLOG2DEVMAC(y1) ;
    wxCoord xx2 = XLOG2DEVMAC(x2) ;
    wxCoord yy2 = YLOG2DEVMAC(y2) ;

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( xx1, yy1 ) ;
    path->AddLineToPoint( xx2 , yy2 ) ;
    path->CloseSubpath() ;
    m_graphicContext->StrokePath( path ) ;
    delete path ;

    CalcBoundingBox(x1, y1);
    CalcBoundingBox(x2, y2);
}

void wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoCrossHair - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

    int w = 0, h = 0;

    GetSize( &w, &h );
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    path->MoveToPoint( XLOG2DEVMAC(0), yy ) ;
    path->AddLineToPoint( XLOG2DEVMAC(w), yy ) ;
    path->CloseSubpath() ;
    path->MoveToPoint( xx, YLOG2DEVMAC(0) ) ;
    path->AddLineToPoint( xx, YLOG2DEVMAC(h) ) ;
    path->CloseSubpath() ;
    m_graphicContext->StrokePath( path ) ;
    delete path ;

    CalcBoundingBox(x, y);
    CalcBoundingBox(x + w, y + h);
}

void wxDC::DoDrawArc( wxCoord x1, wxCoord y1,
                      wxCoord x2, wxCoord y2,
                      wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawArc - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

    wxCoord xx1 = XLOG2DEVMAC(x1);
    wxCoord yy1 = YLOG2DEVMAC(y1);
    wxCoord xx2 = XLOG2DEVMAC(x2);
    wxCoord yy2 = YLOG2DEVMAC(y2);
    wxCoord xxc = XLOG2DEVMAC(xc);
    wxCoord yyc = YLOG2DEVMAC(yc);

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

    bool fill = m_brush.GetStyle() != wxTRANSPARENT ;
    
    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    m_graphicContext->PushState() ;
    m_graphicContext->Translate( xxc, yyc ) ;
    m_graphicContext->Scale( 1, -1 ) ;
    if ( fill )
        path->MoveToPoint( 0, 0 ) ;
    path->AddArc( 0, 0, rad , DegToRad(sa) , DegToRad(ea), false ) ;
    if ( fill )
        path->AddLineToPoint( 0, 0 ) ;
    m_graphicContext->DrawPath( path ) ;
    m_graphicContext->PopState() ;
    delete path ;
}

void wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                              double sa, double ea )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawEllipticArc - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(w);
    wxCoord hh = m_signY * YLOG2DEVREL(h);

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

    bool fill = m_brush.GetStyle() != wxTRANSPARENT ;

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    m_graphicContext->PushState() ;
    m_graphicContext->Translate( xx + ww / 2, yy + hh / 2 ) ;
    m_graphicContext->Scale( 1 * ww / 2 , -1 * hh / 2 ) ;
    if ( fill )
        path->MoveToPoint( 0, 0 ) ;
    path->AddArc( 0, 0, 1 , DegToRad(sa) , DegToRad(ea), false ) ;
    if ( fill )
        path->AddLineToPoint( 0, 0 ) ;
    m_graphicContext->DrawPath( path ) ;
    m_graphicContext->PopState() ;
    delete path ;
}

void wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawPoint - invalid DC") );

    DoDrawLine( x , y , x + 1 , y + 1 ) ;
}

void wxDC::DoDrawLines(int n, wxPoint points[],
                        wxCoord xoffset, wxCoord yoffset)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawLines - invalid DC") );

#if !wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
    if ( m_logicalFunction != wxCOPY )
        return ;
#endif

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

#if wxUSE_SPLINES
void wxDC::DoDrawSpline(wxList *points)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawSpline - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

    wxGraphicPath* path = m_graphicContext->CreatePath() ;

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

    path->MoveToPoint( XLOG2DEVMAC( x1 ) , YLOG2DEVMAC( y1 ) ) ;
    path->AddLineToPoint( XLOG2DEVMAC( cx1 ) , YLOG2DEVMAC( cy1 ) ) ;

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

        path->AddQuadCurveToPoint(
            XLOG2DEVMAC( x1 ) , YLOG2DEVMAC( y1 ) ,
            XLOG2DEVMAC( cx4 ) , YLOG2DEVMAC( cy4 ) ) ;

        cx1 = cx4;
        cy1 = cy4;
    }

    path->AddLineToPoint( XLOG2DEVMAC( x2 ) , YLOG2DEVMAC( y2 ) ) ;

    m_graphicContext->StrokePath( path ) ;
    delete path ;
}
#endif

void wxDC::DoDrawPolygon( int n, wxPoint points[],
                          wxCoord xoffset, wxCoord yoffset,
                          int fillStyle )
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawPolygon - invalid DC") );

    if ( n <= 0 || (m_brush.GetStyle() == wxTRANSPARENT && m_pen.GetStyle() == wxTRANSPARENT ) )
        return ;
    if ( m_logicalFunction != wxCOPY )
        return ;

    wxCoord x1, x2 , y1 , y2 ;
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
        path->AddLineToPoint( x1, y1 ) ;

    path->CloseSubpath() ;
    m_graphicContext->DrawPath( path , fillStyle ) ;

    delete path ;
}

void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawRectangle - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

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

    int penwidth = m_pen.GetWidth();
    if ( penwidth == 0 )
        penwidth = 1 ;
    if ( m_pen.GetStyle() == wxTRANSPARENT )
        penwidth = 0 ;
        
    bool offset = ( penwidth % 2 ) == 1 ; 

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    // if we are offsetting the entire rectangle is moved 0.5, so the border line gets off by 1
    if ( offset )
        path->AddRectangle( xx , yy , ww-1 , hh-1 ) ;
    else
        path->AddRectangle( xx , yy , ww , hh ) ;
        
    m_graphicContext->DrawPath( path ) ;
    delete path ;
}

void wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                   wxCoord width, wxCoord height,
                                   double radius)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawRoundedRectangle - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

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

    wxGraphicPath* path = m_graphicContext->CreatePath() ;
    if ( radius == 0)
    {
        path->AddRectangle( xx , yy , ww , hh ) ;
        m_graphicContext->DrawPath( path ) ;
    }
    else
    {
        path->MoveToPoint( xx + ww, yy + hh / 2);
        path->AddArcToPoint(xx + ww, yy + hh, xx + ww / 2,yy +  hh, radius);
        path->AddArcToPoint(xx , yy + hh, xx , yy + hh / 2, radius);
        path->AddArcToPoint(xx , yy , xx + ww / 2, yy , radius);
        path->AddArcToPoint(xx + ww, yy , xx + ww, yy + hh / 2, radius);
        path->CloseSubpath();
        m_graphicContext->DrawPath( path ) ;
    }
    delete path ;
}

void wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawEllipse - invalid DC") );

    if ( m_logicalFunction != wxCOPY )
        return ;

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
    m_graphicContext->PushState() ;
    m_graphicContext->Translate(xx + ww / 2, yy + hh / 2);
    m_graphicContext->Scale(ww / 2 , hh / 2);
    path->AddArc( 0, 0, 1, 0 , 2 * M_PI , false ) ;
    m_graphicContext->DrawPath( path ) ;
    m_graphicContext->PopState() ;
    delete path ;
}

bool wxDC::CanDrawBitmap() const
{
    return true ;
}

bool wxDC::DoBlit(
    wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
    wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool useMask,
    wxCoord xsrcMask, wxCoord ysrcMask )
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC(cg)::DoBlit - invalid DC") );
    wxCHECK_MSG( source->Ok(), false, wxT("wxDC(cg)::DoBlit - invalid source DC") );

    if ( logical_func == wxNO_OP )
        return true ;

    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc;
        ysrcMask = ysrc;
    }

    wxCoord yysrc = source->YLOG2DEVMAC(ysrc) ;
    wxCoord xxsrc = source->XLOG2DEVMAC(xsrc) ;
    wxCoord wwsrc = source->XLOG2DEVREL(width) ;
    wxCoord hhsrc = source->YLOG2DEVREL(height) ;

    wxCoord yydest = YLOG2DEVMAC(ydest) ;
    wxCoord xxdest = XLOG2DEVMAC(xdest) ;
    wxCoord wwdest = XLOG2DEVREL(width) ;
    wxCoord hhdest = YLOG2DEVREL(height) ;

    wxMemoryDC* memdc = dynamic_cast<wxMemoryDC*>(source) ;
    if ( memdc && logical_func == wxCOPY )
    {
        wxBitmap blit = memdc->GetSelectedObject() ;

        wxASSERT_MSG( blit.Ok() , wxT("Invalid bitmap for blitting") ) ;

        wxCoord bmpwidth = blit.GetWidth();
        wxCoord bmpheight = blit.GetHeight();

        if ( xxsrc != 0 || yysrc != 0 || bmpwidth != wwsrc || bmpheight != hhsrc )
        {
            wwsrc = wxMin( wwsrc , bmpwidth - xxsrc ) ;
            hhsrc = wxMin( hhsrc , bmpheight - yysrc ) ;
            if ( wwsrc > 0 && hhsrc > 0 )
            {
                if ( xxsrc >= 0 && yysrc >= 0 )
                {
                    wxRect subrect( xxsrc, yysrc, wwsrc , hhsrc ) ;
                    // TODO we perhaps could add a DrawSubBitmap call to dc for performance reasons
                    blit = blit.GetSubBitmap( subrect ) ;
                }
                else
                {
                    // in this case we'd probably have to adjust the different coordinates, but
                    // we have to find out proper contract first
                    blit = wxNullBitmap ;
                }
            }
            else
            {
                blit = wxNullBitmap ;
            }
        }

        if ( blit.Ok() )
        {
            m_graphicContext->DrawBitmap( blit, xxdest , yydest , wwdest , hhdest ) ;
        }
    }
    else
    {
        wxFAIL_MSG( wxT("Blitting is only supported from bitmap contexts, and only with wxCOPY logical operation.") ) ;
        return false ; 
    }

    return true;
}

void wxDC::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y,
                              double angle)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawRotatedText - invalid DC") );
//    wxCHECK_RET( m_macATSUIStyle != NULL, wxT("wxDC(cg)::DoDrawRotatedText - no valid font set") );

    if ( str.length() == 0 )
        return ;
    if ( m_logicalFunction != wxCOPY )
        return ;

    int drawX = XLOG2DEVMAC(x) ;
    int drawY = YLOG2DEVMAC(y) ;

    m_graphicContext->DrawText( str, drawX ,drawY , angle ) ;
}

void wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoDrawText - invalid DC") );

    DoDrawRotatedText( strtext , x , y , 0.0 ) ;
}

bool wxDC::CanGetTextExtent() const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC(cg)::CanGetTextExtent - invalid DC") );

    return true ;
}

void wxDC::DoGetTextExtent( const wxString &str, wxCoord *width, wxCoord *height,
                            wxCoord *descent, wxCoord *externalLeading ,
                            wxFont *theFont ) const
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::DoGetTextExtent - invalid DC") );

    wxFont formerFont = m_font ;
    if ( theFont )
    {
        m_graphicContext->SetFont( *theFont ) ;
    }

    wxCoord h , d , e , w ;
    
    m_graphicContext->GetTextExtent( str, &w, &h, &d, &e ) ;

    if ( height )
        *height = YDEV2LOGREL( h ) ;
    if ( descent )
        *descent =YDEV2LOGREL( d);
    if ( externalLeading )
        *externalLeading = YDEV2LOGREL( e);
    if ( width )
        *width = XDEV2LOGREL( w ) ;

    if ( theFont )
    {
        m_graphicContext->SetFont( m_font ) ;
    }
}

bool wxDC::DoGetPartialTextExtents(const wxString& text, wxArrayInt& widths) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC(cg)::DoGetPartialTextExtents - invalid DC") );

    m_graphicContext->GetPartialTextExtents( text, widths ) ;
    for ( size_t i = 0 ; i < widths.GetCount() ; ++i )
        widths[i] = XDEV2LOGREL( widths[i] );

    return true;
}

wxCoord wxDC::GetCharWidth(void) const
{
    wxCoord width ;
    DoGetTextExtent( wxT("g") , &width , NULL , NULL , NULL , NULL ) ;

    return width ;
}

wxCoord wxDC::GetCharHeight(void) const
{
    wxCoord height ;
    DoGetTextExtent( wxT("g") , NULL , &height , NULL , NULL , NULL ) ;

    return height ;
}

void wxDC::Clear(void)
{
    wxCHECK_RET( Ok(), wxT("wxDC(cg)::Clear - invalid DC") );

    if (m_backgroundBrush.Ok() && m_backgroundBrush.GetStyle() != wxTRANSPARENT)
    {
        HIRect rect = CGRectMake( -10000 , -10000 , 20000 , 20000 ) ;
        CGContextRef cg = ((wxMacCGContext*)(m_graphicContext))->GetNativeContext() ;
        switch ( m_backgroundBrush.MacGetBrushKind() )
        {
            case kwxMacBrushTheme :
                {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
                    if ( HIThemeSetFill != 0 )
                    {
                        HIThemeSetFill( m_backgroundBrush.MacGetTheme(), NULL, cg, kHIThemeOrientationNormal );
                        CGContextFillRect(cg, rect);

                    }
                    else
#endif
                    {
                        RGBColor color;
                        GetThemeBrushAsColor( m_backgroundBrush.MacGetTheme(), 32, true, &color );
                        CGContextSetRGBFillColor( cg, (CGFloat) color.red / 65536,
                            (CGFloat) color.green / 65536, (CGFloat) color.blue / 65536, 1 );
                            CGContextFillRect( cg, rect );
                    }

                    // reset to normal value
                    RGBColor col = MAC_WXCOLORREF( GetBrush().GetColour().GetPixel() ) ;
                    CGContextSetRGBFillColor( cg, col.red / 65536.0, col.green / 65536.0, col.blue / 65536.0, 1.0 );
                }
                break ;

            case kwxMacBrushThemeBackground :
                {
                    wxFAIL_MSG( wxT("There shouldn't be theme backgrounds under Quartz") ) ;

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3
                    if ( UMAGetSystemVersion() >= 0x1030 )
                    {
                        HIThemeBackgroundDrawInfo drawInfo ;
                        drawInfo.version = 0 ;
                        drawInfo.state = kThemeStateActive ;
                        drawInfo.kind = m_backgroundBrush.MacGetThemeBackground( NULL ) ;
                        if ( drawInfo.kind == kThemeBackgroundMetal )
                        {
                            HIThemeDrawBackground( &rect, &drawInfo, cg, kHIThemeOrientationNormal ) ;
                            HIThemeApplyBackground( &rect, &drawInfo, cg, kHIThemeOrientationNormal ) ;
                        }
                    }
#endif
                }
                break ;

            case kwxMacBrushColour :
                {
                    // FIXME: doesn't correctly render stippled brushes !!
                    // FIXME: should this be replaced by ::SetBrush() ??

                    RGBColor col = MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel()) ;
                    CGContextSetRGBFillColor( cg , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
                    CGContextFillRect(cg, rect);

                    // reset to normal value
                    col = MAC_WXCOLORREF( GetBrush().GetColour().GetPixel() ) ;
                    CGContextSetRGBFillColor( cg , col.red / 65536.0 , col.green / 65536.0 , col.blue / 65536.0 , 1.0 ) ;
                }
                break ;

            default :
                wxFAIL_MSG( wxT("unknown brush kind") ) ;
                break ;
        }
    }
}

#pragma mark -

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
