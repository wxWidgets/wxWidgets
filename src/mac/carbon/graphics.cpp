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

#include "wx/graphics.h"

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
        using namespace std;
    #endif
#endif

#include "wx/mac/private.h"

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
typedef float CGFloat;
#endif

//
// Graphics Path
//

class WXDLLEXPORT wxMacCoreGraphicsPath : public wxGraphicsPath
{
    DECLARE_NO_COPY_CLASS(wxMacCoreGraphicsPath)
public :
    wxMacCoreGraphicsPath();
    ~wxMacCoreGraphicsPath();

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y );

    // adds a straight line from the current point to (x,y) 
    virtual void AddLineToPoint( wxDouble x, wxDouble y );

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y );

    // closes the current sub-path
    virtual void CloseSubpath();

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble& x, wxDouble&y);

    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise );

    //
    // These are convenience functions which - if not available natively will be assembled 
    // using the primitives from above
    //

    // adds a quadratic Bezier curve from the current point, using a control point and an end point
    virtual void AddQuadCurveToPoint( wxDouble cx, wxDouble cy, wxDouble x, wxDouble y );

    // appends a rectangle as a new closed subpath 
    virtual void AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    // appends an ellipsis as a new closed subpath fitting the passed rectangle
    virtual void AddCircle( wxDouble x, wxDouble y, wxDouble r );

    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r );
    
    CGPathRef GetPath() const;
private :
    CGMutablePathRef m_path;
};

wxMacCoreGraphicsPath::wxMacCoreGraphicsPath()
{
    m_path = CGPathCreateMutable();
}

wxMacCoreGraphicsPath::~wxMacCoreGraphicsPath()
{
    CGPathRelease( m_path );
}

// opens (starts) a new subpath
void wxMacCoreGraphicsPath::MoveToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathMoveToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPath::AddLineToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathAddLineToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPath::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    CGPathAddCurveToPoint( m_path , NULL , cx1 , cy1 , cx2, cy2, x , y );
}

void wxMacCoreGraphicsPath::AddQuadCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble x, wxDouble y )
{
    CGPathAddQuadCurveToPoint( m_path , NULL , cx1 , cy1 , x , y );
}

void wxMacCoreGraphicsPath::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    CGRect cgRect = { { x , y } , { w , h } };
    CGPathAddRect( m_path , NULL , cgRect );
}

void wxMacCoreGraphicsPath::AddCircle( wxDouble x, wxDouble y , wxDouble r )
{
    CGPathAddArc( m_path , NULL , x , y , r , 0.0 , 2 * M_PI , true );
}

// adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
void wxMacCoreGraphicsPath::AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise )
{
    // inverse direction as we the 'normal' state is a y axis pointing down, ie mirrored to the standard core graphics setup
    CGPathAddArc( m_path, NULL , x, y, r, startAngle, endAngle, !clockwise); 
}

void wxMacCoreGraphicsPath::AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )
{
    CGPathAddArcToPoint( m_path, NULL , x1, y1, x2, y2, r); 
}

// closes the current subpath
void wxMacCoreGraphicsPath::CloseSubpath()
{
    CGPathCloseSubpath( m_path );
}

CGPathRef wxMacCoreGraphicsPath::GetPath() const
{
    return m_path;
}

// gets the last point of the current path, (0,0) if not yet set
void wxMacCoreGraphicsPath::GetCurrentPoint( wxDouble& x, wxDouble&y)
{
    CGPoint p = CGPathGetCurrentPoint( m_path );
    x = p.x;
    y = p.y;
}

//
// Graphics Context
//

class WXDLLEXPORT wxMacCoreGraphicsContext : public wxGraphicsContext
{
    DECLARE_NO_COPY_CLASS(wxMacCoreGraphicsContext)

public:
    wxMacCoreGraphicsContext( CGrafPtr port );
    
    wxMacCoreGraphicsContext( CGContextRef cgcontext );
    
    wxMacCoreGraphicsContext();
    
    ~wxMacCoreGraphicsContext();


    // creates a path instance that corresponds to the type of graphics context, ie GDIPlus, cairo, CoreGraphics ...
    virtual wxGraphicsPath * CreatePath();

    // push the current state of the context, ie the transformation matrix on a stack
    virtual void PushState();

    // pops a stored state from the stack
    virtual void PopState();

    // clips drawings to the region
    virtual void Clip( const wxRegion &region );

    //
    // transformation
    //
    
    // translate
    virtual void Translate( wxDouble dx , wxDouble dy );

    // scale
    virtual void Scale( wxDouble xScale , wxDouble yScale );

    // rotate (radians)
    virtual void Rotate( wxDouble angle );

    //
    // setting the paint
    //
    
    // sets the pan
    virtual void SetPen( const wxPen &pen );

    // sets the brush for filling
    virtual void SetBrush( const wxBrush &brush );

    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual void SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2);

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
    // with radius r and color cColor
    virtual void SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor);

    // sets the font
    virtual void SetFont( const wxFont &font );
    
    // sets the text color
    virtual void SetTextColor( const wxColour &col );

    // strokes along a path with the current pen
    virtual void StrokePath( const wxGraphicsPath *path );

    // fills a path with the current brush
    virtual void FillPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE );

    // draws a path by first filling and then stroking
    virtual void DrawPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE );

    //
    // text
    //
    
    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y );

    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle );

    virtual void GetTextExtent( const wxString &text, wxDouble *width, wxDouble *height,
        wxDouble *descent, wxDouble *externalLeading ) const;

    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const;

    //
    // image support
    //

    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    CGContextRef GetNativeContext();
    void SetNativeContext( CGContextRef cg );
    CGPathDrawingMode GetDrawingMode() const { return m_mode; }
    
private:
    CGContextRef m_cgContext;
    CGrafPtr m_qdPort;
    CGPathDrawingMode m_mode;
    ATSUStyle m_macATSUIStyle;
    wxPen m_pen;
    wxBrush m_brush;
    wxColor m_textForegroundColor;
};

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#if !defined( __DARWIN__ ) || defined(__MWERKS__)
#ifndef M_PI
const double M_PI = 3.14159265358979;
#endif
#endif

const double RAD2DEG = 180.0 / M_PI;
const short kEmulatedMode = -1;
const short kUnsupportedMode = -2;

extern TECObjectRef s_TECNativeCToUnicode;

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
// the appropricate wxMacCoreGraphicsContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicsPath implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// wxGraphicsContext implementation
//-----------------------------------------------------------------------------

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( CGrafPtr port )
{
    m_qdPort = port;
    m_cgContext = NULL;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( CGContextRef cgcontext )
{
    m_qdPort = NULL;
    m_cgContext = cgcontext;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL;
    CGContextSaveGState( m_cgContext );
    CGContextSaveGState( m_cgContext );
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext()
{
    m_qdPort = NULL;
    m_cgContext = NULL;
    m_mode = kCGPathFill;
    m_macATSUIStyle = NULL;
}

wxMacCoreGraphicsContext::~wxMacCoreGraphicsContext()
{
    if ( m_cgContext )
    {
        CGContextSynchronize( m_cgContext );
        CGContextRestoreGState( m_cgContext );
        CGContextRestoreGState( m_cgContext );
    }

    if ( m_qdPort )
        CGContextRelease( m_cgContext );
}

void wxMacCoreGraphicsContext::Clip( const wxRegion &region )
{
//    ClipCGContextToRegion ( m_cgContext, &bounds , (RgnHandle) dc->m_macCurrentClipRgn );
}

void wxMacCoreGraphicsContext::StrokePath( const wxGraphicsPath *p )
{
    const wxMacCoreGraphicsPath* path = dynamic_cast< const wxMacCoreGraphicsPath*>( p );
    CGContextAddPath( m_cgContext , path->GetPath() );
    CGContextStrokePath( m_cgContext );
}

void wxMacCoreGraphicsContext::DrawPath( const wxGraphicsPath *p , int fillStyle )
{
    const wxMacCoreGraphicsPath* path = dynamic_cast< const wxMacCoreGraphicsPath*>( p );
    CGPathDrawingMode mode = m_mode;

    if ( fillStyle == wxODDEVEN_RULE )
    {
        if ( mode == kCGPathFill )
            mode = kCGPathEOFill;
        else if ( mode == kCGPathFillStroke )
            mode = kCGPathEOFillStroke;
    }

    CGContextAddPath( m_cgContext , path->GetPath() );
    CGContextDrawPath( m_cgContext , mode );
}

void wxMacCoreGraphicsContext::FillPath( const wxGraphicsPath *p , int fillStyle )
{
    const wxMacCoreGraphicsPath* path = dynamic_cast< const wxMacCoreGraphicsPath*>( p );

    CGContextAddPath( m_cgContext , path->GetPath() );
    if ( fillStyle == wxODDEVEN_RULE )
        CGContextEOFillPath( m_cgContext );
    else
        CGContextFillPath( m_cgContext );
}

wxGraphicsPath* wxMacCoreGraphicsContext::CreatePath()
{
    // make sure that we now have a real cgref, before doing
    // anything with paths
    CGContextRef cg = GetNativeContext();
    cg = NULL;

    return new wxMacCoreGraphicsPath();
}

CGContextRef wxMacCoreGraphicsContext::GetNativeContext()
{
    return m_cgContext;
}

void wxMacCoreGraphicsContext::SetNativeContext( CGContextRef cg )
{
    // we allow either setting or clearing but not replacing
    wxASSERT( m_cgContext == NULL || cg == NULL );

    if ( cg )
        CGContextSaveGState( cg );
    m_cgContext = cg;
}

void wxMacCoreGraphicsContext::Translate( wxDouble dx , wxDouble dy ) 
{
    CGContextTranslateCTM( m_cgContext, dx, dy );
}

void wxMacCoreGraphicsContext::Scale( wxDouble xScale , wxDouble yScale )
{
    CGContextScaleCTM( m_cgContext , xScale , yScale );
}

void wxMacCoreGraphicsContext::Rotate( wxDouble angle )
{
    CGContextRotateCTM( m_cgContext , angle );
}

void wxMacCoreGraphicsContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
    CGImageRef image = (CGImageRef)( bmp.CGImageCreate() );
    HIRect r = CGRectMake( x , y , w , h );
    HIViewDrawCGImage( m_cgContext , &r , image );
    CGImageRelease( image );
}

void wxMacCoreGraphicsContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
    CGRect r = CGRectMake( 00 , 00 , w , h );
    CGContextSaveGState( m_cgContext );
    CGContextTranslateCTM( m_cgContext, x , y + h );
    CGContextScaleCTM( m_cgContext, 1, -1 );
    PlotIconRefInContext( m_cgContext , &r , kAlignNone , kTransformNone ,
        NULL , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) );
    CGContextRestoreGState( m_cgContext );
}

void wxMacCoreGraphicsContext::PushState()
{
    CGContextSaveGState( m_cgContext );
}

void wxMacCoreGraphicsContext::PopState() 
{
    CGContextRestoreGState( m_cgContext );
}

void wxMacCoreGraphicsContext::SetTextColor( const wxColour &col ) 
{
    m_textForegroundColor = col;
}

#pragma mark -
#pragma mark wxMacCoreGraphicsPattern, ImagePattern, HatchPattern classes

// CGPattern wrapper class: always allocate on heap, never call destructor

class wxMacCoreGraphicsPattern
{
public :
    wxMacCoreGraphicsPattern() {}

    // is guaranteed to be called only with a non-Null CGContextRef
    virtual void Render( CGContextRef ctxRef ) = 0;

    operator CGPatternRef() const { return m_patternRef; }

protected :
    virtual ~wxMacCoreGraphicsPattern()
    {
        // as this is called only when the m_patternRef is been released;
        // don't release it again
    }

    static void _Render( void *info, CGContextRef ctxRef )
    {
        wxMacCoreGraphicsPattern* self = (wxMacCoreGraphicsPattern*) info;
        if ( self && ctxRef )
            self->Render( ctxRef );
    }

    static void _Dispose( void *info )
    {
        wxMacCoreGraphicsPattern* self = (wxMacCoreGraphicsPattern*) info;
        delete self;
    }

    CGPatternRef m_patternRef;

    static const CGPatternCallbacks ms_Callbacks;
};

const CGPatternCallbacks wxMacCoreGraphicsPattern::ms_Callbacks = { 0, &wxMacCoreGraphicsPattern::_Render, &wxMacCoreGraphicsPattern::_Dispose };

class ImagePattern : public wxMacCoreGraphicsPattern
{
public :
    ImagePattern( const wxBitmap* bmp , CGAffineTransform transform )
    {
        wxASSERT( bmp && bmp->Ok() );

        Init( (CGImageRef) bmp->CGImageCreate() , transform );
    }

    // ImagePattern takes ownership of CGImageRef passed in
    ImagePattern( CGImageRef image , CGAffineTransform transform )
    {
        if ( image )
            CFRetain( image );

        Init( image , transform );
    }

    virtual void Render( CGContextRef ctxRef )
    {
        if (m_image != NULL)
            HIViewDrawCGImage( ctxRef, &m_imageBounds, m_image );
    }

protected :
    void Init( CGImageRef image, CGAffineTransform transform )
    {
        m_image = image;
        if ( m_image )
        {
            m_imageBounds = CGRectMake( 0.0, 0.0, (CGFloat)CGImageGetWidth( m_image ), (CGFloat)CGImageGetHeight( m_image ) );
            m_patternRef = CGPatternCreate(
                this , m_imageBounds, transform ,
                m_imageBounds.size.width, m_imageBounds.size.height,
                kCGPatternTilingNoDistortion, true , &wxMacCoreGraphicsPattern::ms_Callbacks );
        }
    }

    virtual ~ImagePattern()
    {
        if ( m_image )
            CGImageRelease( m_image );
    }

    CGImageRef m_image;
    CGRect m_imageBounds;
};

class HatchPattern : public wxMacCoreGraphicsPattern
{
public :
    HatchPattern( int hatchstyle, CGAffineTransform transform )
    {
        m_hatch = hatchstyle;
        m_imageBounds = CGRectMake( 0.0, 0.0, 8.0 , 8.0 );
        m_patternRef = CGPatternCreate(
            this , m_imageBounds, transform ,
            m_imageBounds.size.width, m_imageBounds.size.height,
            kCGPatternTilingNoDistortion, false , &wxMacCoreGraphicsPattern::ms_Callbacks );
    }

    void StrokeLineSegments( CGContextRef ctxRef , const CGPoint pts[] , size_t count )
    {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            CGContextStrokeLineSegments( ctxRef , pts , count );
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
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxCROSSDIAG_HATCH :
                {
                    CGPoint pts[] =
                    {
                        { 0.0 , 0.0 } , { 8.0 , 8.0 } ,
                        { 8.0 , 0.0 } , { 0.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 4 );
                }
                break;

            case wxFDIAGONAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 0.0 } , { 8.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxCROSS_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 4 );
                }
                break;

            case wxHORIZONTAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxVERTICAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            default:
                break;
        }
    }

protected :
    virtual ~HatchPattern() {}

    CGRect      m_imageBounds;
    int         m_hatch;
};

#pragma mark -

void wxMacCoreGraphicsContext::SetPen( const wxPen &pen )
{
    m_pen = pen;
    if ( m_cgContext == NULL )
        return;

    bool fill = m_brush.GetStyle() != wxTRANSPARENT;
    bool stroke = pen.GetStyle() != wxTRANSPARENT;

#if 0
    // we can benchmark performance; should go into a setting eventually
    CGContextSetShouldAntialias( m_cgContext , false );
#endif

    if ( fill | stroke )
    {
        // set up brushes
        m_mode = kCGPathFill; // just a default

        if ( stroke )
        {
            CGContextSetRGBStrokeColor( m_cgContext , pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 , 
                    pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 );

            // TODO: * m_dc->m_scaleX
            CGFloat penWidth = pen.GetWidth();
            if (penWidth <= 0.0)
                penWidth = 0.1;
            CGContextSetLineWidth( m_cgContext , penWidth );

            CGLineCap cap;
            switch ( pen.GetCap() )
            {
                case wxCAP_ROUND :
                    cap = kCGLineCapRound;
                    break;

                case wxCAP_PROJECTING :
                    cap = kCGLineCapSquare;
                    break;

                case wxCAP_BUTT :
                    cap = kCGLineCapButt;
                    break;

                default :
                    cap = kCGLineCapButt;
                    break;
            }

            CGLineJoin join;
            switch ( pen.GetJoin() )
            {
                case wxJOIN_BEVEL :
                    join = kCGLineJoinBevel;
                    break;

                case wxJOIN_MITER :
                    join = kCGLineJoinMiter;
                    break;

                case wxJOIN_ROUND :
                    join = kCGLineJoinRound;
                    break;

                default :
                    join = kCGLineJoinMiter;
                    break;
            }

            m_mode = kCGPathStroke;
            int count = 0;

            const CGFloat *lengths = NULL;
            CGFloat *userLengths = NULL;

            const CGFloat dashUnit = penWidth < 1.0 ? 1.0 : penWidth;

            const CGFloat dotted[] = { dashUnit , dashUnit + 2.0 };
            const CGFloat short_dashed[] = { 9.0 , 6.0 };
            const CGFloat dashed[] = { 19.0 , 9.0 };
            const CGFloat dotted_dashed[] = { 9.0 , 6.0 , 3.0 , 3.0 };

            switch ( pen.GetStyle() )
            {
                case wxSOLID :
                    break;

                case wxDOT :
                    lengths = dotted;
                    count = WXSIZEOF(dotted);
                    break;

                case wxLONG_DASH :
                    lengths = dashed;
                    count = WXSIZEOF(dashed);
                    break;

                case wxSHORT_DASH :
                    lengths = short_dashed;
                    count = WXSIZEOF(short_dashed);
                    break;

                case wxDOT_DASH :
                    lengths = dotted_dashed;
                    count = WXSIZEOF(dotted_dashed);
                    break;

                case wxUSER_DASH :
                    wxDash *dashes;
                    count = pen.GetDashes( &dashes );
                    if ((dashes != NULL) && (count > 0))
                    {
                        userLengths = new CGFloat[count];
                        for ( int i = 0; i < count; ++i )
                        {
                            userLengths[i] = dashes[i] * dashUnit;

                            if ( i % 2 == 1 && userLengths[i] < dashUnit + 2.0 )
                                userLengths[i] = dashUnit + 2.0;
                            else if ( i % 2 == 0 && userLengths[i] < dashUnit )
                                userLengths[i] = dashUnit;
                        }
                    }
                    lengths = userLengths;
                    break;

                case wxSTIPPLE :
                    {
                        CGFloat  alphaArray[1] = { 1.0 };
                        wxBitmap* bmp = pen.GetStipple();
                        if ( bmp && bmp->Ok() )
                        {
                            wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( NULL ) );
                            CGContextSetStrokeColorSpace( m_cgContext , patternSpace );
                            wxMacCFRefHolder<CGPatternRef> pattern( *( new ImagePattern( bmp , CGContextGetCTM( m_cgContext ) ) ) );
                            CGContextSetStrokePattern( m_cgContext, pattern , alphaArray );
                        }
                    }
                    break;

                default :
                    {
                        wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
                        CGContextSetStrokeColorSpace( m_cgContext , patternSpace );
                        wxMacCFRefHolder<CGPatternRef> pattern( *( new HatchPattern( pen.GetStyle() , CGContextGetCTM( m_cgContext ) ) ) );

                        CGFloat  colorArray[4] = { pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 , 
                            pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 };

                        CGContextSetStrokePattern( m_cgContext, pattern , colorArray );
                    }
                    break;
            }

            if ((lengths != NULL) && (count > 0))
            {
                CGContextSetLineDash( m_cgContext , 0 , lengths , count );
                // force the line cap, otherwise we get artifacts (overlaps) and just solid lines
                cap = kCGLineCapButt;
            }
            else
            {
               CGContextSetLineDash( m_cgContext , 0 , NULL , 0 );
            }

            CGContextSetLineCap( m_cgContext , cap );
            CGContextSetLineJoin( m_cgContext , join );

            delete[] userLengths;
        }

        if ( fill && stroke )
            m_mode = kCGPathFillStroke;
    }
}

void wxMacCoreGraphicsContext::SetBrush( const wxBrush &brush )
{
    m_brush = brush;
    if ( m_cgContext == NULL )
        return;

    bool fill = brush.GetStyle() != wxTRANSPARENT;
    bool stroke = m_pen.GetStyle() != wxTRANSPARENT;

#if 0
    // we can benchmark performance, should go into a setting later
    CGContextSetShouldAntialias( m_cgContext , false );
#endif

    if ( fill | stroke )
    {
        // setup brushes
        m_mode = kCGPathFill; // just a default

        if ( fill )
        {
            if ( brush.GetStyle() == wxSOLID )
            {
                CGContextSetRGBFillColor( m_cgContext , brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 , 
                    brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 );
            }
            else if ( brush.IsHatch() )
            {
                wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
                CGContextSetFillColorSpace( m_cgContext , patternSpace );
                wxMacCFRefHolder<CGPatternRef> pattern( *( new HatchPattern( brush.GetStyle() , CGContextGetCTM( m_cgContext ) ) ) );

                CGFloat  colorArray[4] = { brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 , 
                    brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 };

                CGContextSetFillPattern( m_cgContext, pattern , colorArray );
            }
            else
            {
                // now brush is a bitmap
                CGFloat  alphaArray[1] = { 1.0 };
                wxBitmap* bmp = brush.GetStipple();
                if ( bmp && bmp->Ok() )
                {
                    wxMacCFRefHolder<CGColorSpaceRef> patternSpace( CGColorSpaceCreatePattern( NULL ) );
                    CGContextSetFillColorSpace( m_cgContext , patternSpace );
                    wxMacCFRefHolder<CGPatternRef> pattern( *( new ImagePattern( bmp , CGContextGetCTM( m_cgContext ) ) ) );
                    CGContextSetFillPattern( m_cgContext, pattern , alphaArray );
                }
            }

            m_mode = kCGPathFill;
        }

        if ( fill && stroke )
            m_mode = kCGPathFillStroke;
        else if ( stroke )
            m_mode = kCGPathStroke;
    }
}

// sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
void wxMacCoreGraphicsContext::SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2)
{
}

// sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
// with radius r and color cColor
void wxMacCoreGraphicsContext::SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor)
{
}


void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y ) 
{
    DrawText(str, x, y, 0.0);
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle ) 
{
    OSStatus status = noErr;
    ATSUTextLayout atsuLayout;
    UniCharCount chars = str.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str();
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout );

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the rotated text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true );
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    int iAngle = int( angle * RAD2DEG );
    if ( abs(iAngle) > 0 )
    {
        Fixed atsuAngle = IntToFixed( iAngle );
        ATSUAttributeTag atsuTags[] =
        {
            kATSULineRotationTag ,
        };
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( Fixed ) ,
        };
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &atsuAngle ,
        };
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues );
    }

    {
        ATSUAttributeTag atsuTags[] =
        {
            kATSUCGContextTag ,
        };
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( CGContextRef ) ,
        };
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &m_cgContext ,
        };
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues );
    }

    ATSUTextMeasurement textBefore, textAfter;
    ATSUTextMeasurement ascent, descent;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &ascent , &descent );

    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    Rect rect;
/*
    // TODO
    if ( m_backgroundMode == wxSOLID )
    {
        wxGraphicsPath* path = m_graphicContext->CreatePath();
        path->MoveToPoint( drawX , drawY );
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent)) );
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent ) + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent) - sin(angle / RAD2DEG) * FixedToInt(textAfter)) );
        path->AddLineToPoint(
            (int) (drawX + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY - sin(angle / RAD2DEG) * FixedToInt(textAfter)) );

        m_graphicContext->FillPath( path , m_textBackgroundColour );
        delete path;
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

    CGContextRestoreGState(m_cgContext);

    ::ATSUDisposeTextLayout(atsuLayout);

#if SIZEOF_WCHAR_T == 4
    free( ubuf );
#endif
}
    
void wxMacCoreGraphicsContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                            wxDouble *descent, wxDouble *externalLeading ) const
{
    wxCHECK_RET( m_macATSUIStyle != NULL, wxT("wxDC(cg)::DoGetTextExtent - no valid font set") );

    OSStatus status = noErr;
    
    ATSUTextLayout atsuLayout;
    UniCharCount chars = str.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str();
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout );

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

    ATSUTextMeasurement textBefore, textAfter;
    ATSUTextMeasurement textAscent, textDescent;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &textAscent , &textDescent );

    if ( height )
        *height = FixedToInt(textAscent + textDescent);
    if ( descent )
        *descent = FixedToInt(textDescent);
    if ( externalLeading )
        *externalLeading = 0;
    if ( width )
        *width = FixedToInt(textAfter - textBefore);

    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxMacCoreGraphicsContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const 
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return;

    ATSUTextLayout atsuLayout;
    UniCharCount chars = text.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , text.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , text.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = text.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) text.wc_str();
#else
    wxWCharBuffer wchar = text.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    OSStatus status;
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout );

    for ( int pos = 0; pos < (int)chars; pos ++ )
    {
        unsigned long actualNumberOfBounds = 0;
        ATSTrapezoid glyphBounds;

        // We get a single bound, since the text should only require one. If it requires more, there is an issue
        OSStatus result;
        result = ATSUGetGlyphBounds( atsuLayout, 0, 0, kATSUFromTextBeginning, pos + 1,
            kATSUseDeviceOrigins, 1, &glyphBounds, &actualNumberOfBounds );
        if (result != noErr || actualNumberOfBounds != 1 )
            return;

        widths[pos] = FixedToInt( glyphBounds.upperRight.x - glyphBounds.upperLeft.x );
        //unsigned char uch = s[i];
    }

    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxMacCoreGraphicsContext::SetFont( const wxFont &font ) 
{
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL;
    }

    if ( font.Ok() )
    {
        OSStatus status;

        status = ATSUCreateAndCopyStyle( (ATSUStyle) font.MacGetATSUStyle() , (ATSUStyle*) &m_macATSUIStyle );

        wxASSERT_MSG( status == noErr, wxT("couldn't create ATSU style") );

        // we need the scale here ...

        Fixed atsuSize = IntToFixed( int( /*m_scaleY*/ 1 * font.MacGetFontSize()) );
        RGBColor atsuColor = MAC_WXCOLORREF( m_textForegroundColor.GetPixel() );
        ATSUAttributeTag atsuTags[] =
        {
                kATSUSizeTag ,
                kATSUColorTag ,
        };
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
                sizeof( Fixed ) ,
                sizeof( RGBColor ) ,
        };
        ATSUAttributeValuePtr atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
                &atsuSize ,
                &atsuColor ,
        };

        status = ::ATSUSetAttributes(
            (ATSUStyle)m_macATSUIStyle, sizeof(atsuTags) / sizeof(ATSUAttributeTag) ,
            atsuTags, atsuSizes, atsuValues);

        wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") );
    }
}

wxGraphicsContext* wxGraphicsContext::Create( const wxWindowDC &dc )
{
    return new wxMacCoreGraphicsContext((CGContextRef)dc.GetWindow()->MacGetCGContextRef() );
}

#endif // wxMAC_USE_CORE_GRAPHICS
