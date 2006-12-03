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

#if wxUSE_GRAPHICS_CONTEXT && wxMAC_USE_CORE_GRAPHICS

#include "wx/graphics.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/log.h"
    #include "wx/region.h"
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
#ifndef wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    #define wxMAC_USE_CORE_GRAPHICS_BLEND_MODES 1
#else
    #define wxMAC_USE_CORE_GRAPHICS_BLEND_MODES 0
#endif
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#if !defined( __DARWIN__ ) || defined(__MWERKS__)
#ifndef M_PI
const double M_PI = 3.14159265358979;
#endif
#endif

static const double RAD2DEG = 180.0 / M_PI;

//
// Pen, Brushes and Fonts
//

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
    ImagePattern( const wxBitmap* bmp , const CGAffineTransform& transform )
    {
        wxASSERT( bmp && bmp->Ok() );

        Init( (CGImageRef) bmp->CGImageCreate() , transform );
    }

    // ImagePattern takes ownership of CGImageRef passed in
    ImagePattern( CGImageRef image , const CGAffineTransform& transform )
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
    void Init( CGImageRef image, const CGAffineTransform& transform )
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
    HatchPattern( int hatchstyle, const CGAffineTransform& transform )
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
        if ( CGContextStrokeLineSegments!=NULL  )
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

class wxMacCoreGraphicsPenData : public wxGraphicsObjectRefData
{
public:
    wxMacCoreGraphicsPenData( wxGraphicsRenderer* renderer, const wxPen &pen );
    ~wxMacCoreGraphicsPenData();

    void Init();
    virtual void Apply( wxGraphicsContext* context );
    virtual wxDouble GetWidth() { return m_width; }

protected :
    CGLineCap m_cap;
    wxMacCFRefHolder<CGColorRef> m_color;
    wxMacCFRefHolder<CGColorSpaceRef> m_colorSpace;

    CGLineJoin m_join;
    CGFloat m_width;

    int m_count;
    const CGFloat *m_lengths;
    CGFloat *m_userLengths;


    bool m_isPattern;
    wxMacCFRefHolder<CGPatternRef> m_pattern;
    CGFloat* m_patternColorComponents;
};

wxMacCoreGraphicsPenData::wxMacCoreGraphicsPenData( wxGraphicsRenderer* renderer, const wxPen &pen ) :
    wxGraphicsObjectRefData( renderer )
{
    Init();

    float components[4] = { pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 ,
            pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 } ;
    m_color.Set( CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ) ;

    // TODO: * m_dc->m_scaleX
    m_width = pen.GetWidth();
    if (m_width <= 0.0)
        m_width = 0.1;

    switch ( pen.GetCap() )
    {
        case wxCAP_ROUND :
            m_cap = kCGLineCapRound;
            break;

        case wxCAP_PROJECTING :
            m_cap = kCGLineCapSquare;
            break;

        case wxCAP_BUTT :
            m_cap = kCGLineCapButt;
            break;

        default :
            m_cap = kCGLineCapButt;
            break;
    }

    switch ( pen.GetJoin() )
    {
        case wxJOIN_BEVEL :
            m_join = kCGLineJoinBevel;
            break;

        case wxJOIN_MITER :
            m_join = kCGLineJoinMiter;
            break;

        case wxJOIN_ROUND :
            m_join = kCGLineJoinRound;
            break;

        default :
            m_join = kCGLineJoinMiter;
            break;
    }

    const CGFloat dashUnit = m_width < 1.0 ? 1.0 : m_width;

    const CGFloat dotted[] = { dashUnit , dashUnit + 2.0 };
    static const CGFloat short_dashed[] = { 9.0 , 6.0 };
    static const CGFloat dashed[] = { 19.0 , 9.0 };
    static const CGFloat dotted_dashed[] = { 9.0 , 6.0 , 3.0 , 3.0 };

    switch ( pen.GetStyle() )
    {
        case wxSOLID :
            break;

        case wxDOT :
            m_count = WXSIZEOF(dotted);
            m_userLengths = new CGFloat[ m_count ] ;
            memcpy( m_userLengths, dotted, sizeof(dotted) );
            m_lengths = m_userLengths;
            break;

        case wxLONG_DASH :
            m_count = WXSIZEOF(dashed);
            m_lengths = dashed;
            break;

        case wxSHORT_DASH :
            m_count = WXSIZEOF(short_dashed);
            m_lengths = short_dashed;
            break;

        case wxDOT_DASH :
            m_count = WXSIZEOF(dotted_dashed);
            m_lengths = dotted_dashed;
            break;

        case wxUSER_DASH :
            wxDash *dashes;
            m_count = pen.GetDashes( &dashes );
            if ((dashes != NULL) && (m_count > 0))
            {
                m_userLengths = new CGFloat[m_count];
                for ( int i = 0; i < m_count; ++i )
                {
                    m_userLengths[i] = dashes[i] * dashUnit;

                    if ( i % 2 == 1 && m_userLengths[i] < dashUnit + 2.0 )
                        m_userLengths[i] = dashUnit + 2.0;
                    else if ( i % 2 == 0 && m_userLengths[i] < dashUnit )
                        m_userLengths[i] = dashUnit;
                }
            }
            m_lengths = m_userLengths;
            break;

        case wxSTIPPLE :
            {
                wxBitmap* bmp = pen.GetStipple();
                if ( bmp && bmp->Ok() )
                {
                    m_colorSpace.Set( CGColorSpaceCreatePattern( NULL ) );
                    m_pattern.Set( *( new ImagePattern( bmp , CGAffineTransformMakeScale( 1,-1 ) ) ) );
                    m_patternColorComponents = new CGFloat[1] ;
                    m_patternColorComponents[0] = 1.0;
                    m_isPattern = true;
                }
            }
            break;

        default :
            {
                m_isPattern = true;
                m_colorSpace.Set( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
                m_pattern.Set( *( new HatchPattern( pen.GetStyle() , CGAffineTransformMakeScale( 1,-1 ) ) ) );
                m_patternColorComponents = new CGFloat[4] ;
                m_patternColorComponents[0] = pen.GetColour().Red() / 255.0;
                m_patternColorComponents[1] = pen.GetColour().Green() / 255.0;
                m_patternColorComponents[2] = pen.GetColour().Blue() / 255.0;
                m_patternColorComponents[3] =  pen.GetColour().Alpha() / 255.0;
            }
            break;
    }
    if ((m_lengths != NULL) && (m_count > 0))
    {
        // force the line cap, otherwise we get artifacts (overlaps) and just solid lines
        m_cap = kCGLineCapButt;
    }
}

wxMacCoreGraphicsPenData::~wxMacCoreGraphicsPenData()
{
    delete[] m_userLengths;
    delete[] m_patternColorComponents;
}

void wxMacCoreGraphicsPenData::Init()
{
    m_lengths = NULL;
    m_userLengths = NULL;
    m_width = 0;
    m_count = 0;
    m_patternColorComponents = NULL;
    m_isPattern = false;
}

void wxMacCoreGraphicsPenData::Apply( wxGraphicsContext* context )
{
    CGContextRef cg = (CGContextRef) context->GetNativeContext();
    CGContextSetLineWidth( cg , m_width );
    CGContextSetLineJoin( cg , m_join );

    CGContextSetLineDash( cg , 0 , m_lengths , m_count );
    CGContextSetLineCap( cg , m_cap );

    if ( m_isPattern )
    {
        CGAffineTransform matrix = CGContextGetCTM( cg );
        CGContextSetPatternPhase( cg, CGSizeMake(matrix.tx, matrix.ty) );
        CGContextSetStrokeColorSpace( cg , m_colorSpace );
        CGContextSetStrokePattern( cg, m_pattern , m_patternColorComponents );
    }
    else
    {
        if ( context->GetLogicalFunction() == wxINVERT || context->GetLogicalFunction() == wxXOR )
        {
            CGContextSetRGBStrokeColor( cg , 1.0, 1.0 , 1.0, 1.0 );
        }
        else
            CGContextSetStrokeColorWithColor( cg , m_color );
    }
}

//
// Brush
//

class wxMacCoreGraphicsBrushData : public wxGraphicsObjectRefData
{
public:
    wxMacCoreGraphicsBrushData( wxGraphicsRenderer* renderer );
    wxMacCoreGraphicsBrushData( wxGraphicsRenderer* renderer, const wxBrush &brush );
    ~wxMacCoreGraphicsBrushData ();

    virtual void Apply( wxGraphicsContext* context );
    void CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
        const wxColour&c1, const wxColour&c2 );
    void CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor );

    virtual bool IsShading() { return m_isShading; }
    CGShadingRef GetShading() { return m_shading; }
protected:
    CGFunctionRef CreateGradientFunction( const wxColour& c1, const wxColour& c2 );
    static void CalculateShadingValues (void *info, const CGFloat *in, CGFloat *out);
    virtual void Init();

    wxMacCFRefHolder<CGColorRef> m_color;
    wxMacCFRefHolder<CGColorSpaceRef> m_colorSpace;

    bool m_isPattern;
    wxMacCFRefHolder<CGPatternRef> m_pattern;
    CGFloat* m_patternColorComponents;

    bool m_isShading;
    CGFunctionRef m_gradientFunction;
    CGShadingRef m_shading;
    CGFloat *m_gradientComponents;
};

wxMacCoreGraphicsBrushData::wxMacCoreGraphicsBrushData( wxGraphicsRenderer* renderer) : wxGraphicsObjectRefData( renderer )
{
    Init();
}

void wxMacCoreGraphicsBrushData::CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
        const wxColour&c1, const wxColour&c2 )
{
    m_gradientFunction = CreateGradientFunction( c1, c2 );
    m_shading = CGShadingCreateAxial( wxMacGetGenericRGBColorSpace(), CGPointMake(x1,y1), CGPointMake(x2,y2), m_gradientFunction, true, true ) ;
    m_isShading = true ;
}

void wxMacCoreGraphicsBrushData::CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor )
{
    m_gradientFunction = CreateGradientFunction( oColor, cColor );
    m_shading = CGShadingCreateRadial( wxMacGetGenericRGBColorSpace(), CGPointMake(xo,yo), 0, CGPointMake(xc,yc), radius, m_gradientFunction, true, true ) ;
    m_isShading = true ;
}

wxMacCoreGraphicsBrushData::wxMacCoreGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush &brush) : wxGraphicsObjectRefData( renderer )
{
    Init();

    if ( brush.GetStyle() == wxSOLID )
    {
        if ( brush.MacGetBrushKind() == kwxMacBrushTheme )
        {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
            if ( HIThemeBrushCreateCGColor != 0 )
            {
                CGColorRef color ;
                HIThemeBrushCreateCGColor( brush.MacGetTheme(), &color );
                m_color.Set( color ) ;
            }
            else
#endif
            {
                // as close as we can get, unfortunately < 10.4 things get difficult
                RGBColor color;
                GetThemeBrushAsColor( brush.MacGetTheme(), 32, true, &color );
                float components[4] = {  (CGFloat) color.red / 65536,
                    (CGFloat) color.green / 65536, (CGFloat) color.blue / 65536, 1 } ;
                m_color.Set( CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ) ;
            }
        }
        else
        {
            float components[4] = { brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 ,
                brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 } ;
            m_color.Set( CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ) ;
        }
    }
    else if ( brush.IsHatch() )
    {
        m_isPattern = true;
        m_colorSpace.Set( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
        m_pattern.Set( *( new HatchPattern( brush.GetStyle() , CGAffineTransformMakeScale( 1,-1 ) ) ) );

        m_patternColorComponents = new CGFloat[4] ;
        m_patternColorComponents[0] = brush.GetColour().Red() / 255.0;
        m_patternColorComponents[1] = brush.GetColour().Green() / 255.0;
        m_patternColorComponents[2] = brush.GetColour().Blue() / 255.0;
        m_patternColorComponents[3] = brush.GetColour().Alpha() / 255.0;
    }
    else
    {
        // now brush is a bitmap
        wxBitmap* bmp = brush.GetStipple();
        if ( bmp && bmp->Ok() )
        {
            m_isPattern = true;
            m_patternColorComponents = new CGFloat[1] ;
            m_patternColorComponents[0] = 1.0;
            m_colorSpace.Set( CGColorSpaceCreatePattern( NULL ) );
            m_pattern.Set( *( new ImagePattern( bmp , CGAffineTransformMakeScale( 1,-1 ) ) ) );
        }
    }
}

wxMacCoreGraphicsBrushData::~wxMacCoreGraphicsBrushData()
{
    if ( m_shading )
        CGShadingRelease(m_shading);

    if( m_gradientFunction )
        CGFunctionRelease(m_gradientFunction);

    delete[] m_gradientComponents;
    delete[] m_patternColorComponents;
}

void wxMacCoreGraphicsBrushData::Init()
{
    m_patternColorComponents = NULL;
    m_gradientFunction = NULL;
    m_shading = NULL;
    m_isPattern = false;
    m_gradientComponents = NULL;
    m_isShading = false;
}

void wxMacCoreGraphicsBrushData::Apply( wxGraphicsContext* context )
{
    CGContextRef cg = (CGContextRef) context->GetNativeContext();

    if ( m_isShading )
    {
        // nothing to set as shades are processed by clipping using the path and filling
    }
    else
    {
        if ( m_isPattern )
        {
            CGAffineTransform matrix = CGContextGetCTM( cg );
            CGContextSetPatternPhase( cg, CGSizeMake(matrix.tx, matrix.ty) );
            CGContextSetFillColorSpace( cg , m_colorSpace );
            CGContextSetFillPattern( cg, m_pattern , m_patternColorComponents );
        }
        else
        {
            CGContextSetFillColorWithColor( cg, m_color );
        }
    }
}

void wxMacCoreGraphicsBrushData::CalculateShadingValues (void *info, const CGFloat *in, CGFloat *out)
{
    CGFloat* colors = (CGFloat*) info ;
    CGFloat f = *in;
    for( int i = 0 ; i < 4 ; ++i )
    {
        out[i] = colors[i] + ( colors[4+i] - colors[i] ) * f;
    }
}

CGFunctionRef wxMacCoreGraphicsBrushData::CreateGradientFunction( const wxColour& c1, const wxColour& c2 )
{
    static const CGFunctionCallbacks callbacks = { 0, &CalculateShadingValues, NULL };
    static const CGFloat input_value_range [2] = { 0, 1 };
    static const CGFloat output_value_ranges [8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
    m_gradientComponents = new CGFloat[8] ;
    m_gradientComponents[0] = c1.Red() / 255.0;
    m_gradientComponents[1] = c1.Green() / 255.0;
    m_gradientComponents[2] = c1.Blue() / 255.0;
    m_gradientComponents[3] = c1.Alpha() / 255.0;
    m_gradientComponents[4] = c2.Red() / 255.0;
    m_gradientComponents[5] = c2.Green() / 255.0;
    m_gradientComponents[6] = c2.Blue() / 255.0;
    m_gradientComponents[7] = c2.Alpha() / 255.0;

    return CGFunctionCreate ( m_gradientComponents,  1,
                            input_value_range,
                            4,
                            output_value_ranges,
                            &callbacks);
}

//
// Font
//

class wxMacCoreGraphicsFontData : public wxGraphicsObjectRefData
{
public:
    wxMacCoreGraphicsFontData( wxGraphicsRenderer* renderer, const wxFont &font, const wxColour& col );
    ~wxMacCoreGraphicsFontData();

    virtual ATSUStyle GetATSUStyle() { return m_macATSUIStyle; }
private :
    ATSUStyle m_macATSUIStyle;
};

wxMacCoreGraphicsFontData::wxMacCoreGraphicsFontData(wxGraphicsRenderer* renderer, const wxFont &font, const wxColour& col) : wxGraphicsObjectRefData( renderer )
{
    m_macATSUIStyle = NULL;

    OSStatus status;

    status = ATSUCreateAndCopyStyle( (ATSUStyle) font.MacGetATSUStyle() , &m_macATSUIStyle );

    wxASSERT_MSG( status == noErr, wxT("couldn't create ATSU style") );

    // we need the scale here ...

    Fixed atsuSize = IntToFixed( int( 1 * font.MacGetFontSize()) );
    RGBColor atsuColor = MAC_WXCOLORREF( col.GetPixel() );
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
        m_macATSUIStyle, sizeof(atsuTags) / sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") );
}

wxMacCoreGraphicsFontData::~wxMacCoreGraphicsFontData()
{
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL;
    }
}

//
// Graphics Matrix
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsMatrix declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMacCoreGraphicsMatrixData : public wxGraphicsMatrixData
{
public :
    wxMacCoreGraphicsMatrixData(wxGraphicsRenderer* renderer) ;

    virtual ~wxMacCoreGraphicsMatrixData() ;

    virtual wxGraphicsObjectRefData *Clone() const ;

    // concatenates the matrix
    virtual void Concat( const wxGraphicsMatrixData *t );

    // sets the matrix to the respective values
    virtual void Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0);

    // makes this the inverse matrix
    virtual void Invert();

    // returns true if the elements of the transformation matrix are equal ?
    virtual bool IsEqual( const wxGraphicsMatrixData* t) const ;

    // return true if this is the identity matrix
    virtual bool IsIdentity() const;

    //
    // transformation
    //

    // add the translation to this matrix
    virtual void Translate( wxDouble dx , wxDouble dy );

    // add the scale to this matrix
    virtual void Scale( wxDouble xScale , wxDouble yScale );

    // add the rotation to this matrix (radians)
    virtual void Rotate( wxDouble angle );

    //
    // apply the transforms
    //

    // applies that matrix to the point
    virtual void TransformPoint( wxDouble *x, wxDouble *y ) const;

    // applies the matrix except for translations
    virtual void TransformDistance( wxDouble *dx, wxDouble *dy ) const;

    // returns the native representation
    virtual void * GetNativeMatrix() const;

private :
    CGAffineTransform m_matrix;
} ;

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsMatrix implementation
//-----------------------------------------------------------------------------

wxMacCoreGraphicsMatrixData::wxMacCoreGraphicsMatrixData(wxGraphicsRenderer* renderer) : wxGraphicsMatrixData(renderer)
{
}

wxMacCoreGraphicsMatrixData::~wxMacCoreGraphicsMatrixData()
{
}

wxGraphicsObjectRefData *wxMacCoreGraphicsMatrixData::Clone() const
{
    wxMacCoreGraphicsMatrixData* m = new wxMacCoreGraphicsMatrixData(GetRenderer()) ;
    m->m_matrix = m_matrix ;
    return m;
}

// concatenates the matrix
void wxMacCoreGraphicsMatrixData::Concat( const wxGraphicsMatrixData *t )
{
    m_matrix = CGAffineTransformConcat(m_matrix, *((CGAffineTransform*) t->GetNativeMatrix()) );
}

// sets the matrix to the respective values
void wxMacCoreGraphicsMatrixData::Set(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
    wxDouble tx, wxDouble ty)
{
    m_matrix = CGAffineTransformMake(a,b,c,d,tx,ty);
}

// makes this the inverse matrix
void wxMacCoreGraphicsMatrixData::Invert()
{
    m_matrix = CGAffineTransformInvert( m_matrix );
}

// returns true if the elements of the transformation matrix are equal ?
bool wxMacCoreGraphicsMatrixData::IsEqual( const wxGraphicsMatrixData* t) const
{
    const CGAffineTransform* tm = (CGAffineTransform*) t->GetNativeMatrix();
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    if ( CGAffineTransformEqualToTransform!=NULL )
    {
        return CGAffineTransformEqualToTransform(m_matrix, *((CGAffineTransform*) t->GetNativeMatrix()));
    }
    else
#endif
    {
        return (
            m_matrix.a == tm->a &&
            m_matrix.b == tm->b &&
            m_matrix.c == tm->c &&
            m_matrix.d == tm->d &&
            m_matrix.tx == tm->tx &&
            m_matrix.ty == tm->ty ) ;
    }
}

// return true if this is the identity matrix
bool wxMacCoreGraphicsMatrixData::IsIdentity() const
{
    return ( m_matrix.a == 1 && m_matrix.d == 1 &&
        m_matrix.b == 0 && m_matrix.d == 0 && m_matrix.tx == 0 && m_matrix.ty == 0);
}

//
// transformation
//

// add the translation to this matrix
void wxMacCoreGraphicsMatrixData::Translate( wxDouble dx , wxDouble dy )
{
    m_matrix = CGAffineTransformTranslate( m_matrix, dx, dy);
}

// add the scale to this matrix
void wxMacCoreGraphicsMatrixData::Scale( wxDouble xScale , wxDouble yScale )
{
    m_matrix = CGAffineTransformScale( m_matrix, xScale, yScale);
}

// add the rotation to this matrix (radians)
void wxMacCoreGraphicsMatrixData::Rotate( wxDouble angle )
{
    m_matrix = CGAffineTransformRotate( m_matrix, angle);
}

//
// apply the transforms
//

// applies that matrix to the point
void wxMacCoreGraphicsMatrixData::TransformPoint( wxDouble *x, wxDouble *y ) const
{
    CGPoint pt = CGPointApplyAffineTransform( CGPointMake(*x,*y), m_matrix);

    *x = pt.x;
    *y = pt.y;
}

// applies the matrix except for translations
void wxMacCoreGraphicsMatrixData::TransformDistance( wxDouble *dx, wxDouble *dy ) const
{
    CGSize sz = CGSizeApplyAffineTransform( CGSizeMake(*dx,*dy) , m_matrix );
    *dx = sz.width;
    *dy = sz.height;
}

// returns the native representation
void * wxMacCoreGraphicsMatrixData::GetNativeMatrix() const
{
    return (void*) &m_matrix;
}

//
// Graphics Path
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsPath declaration
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMacCoreGraphicsPathData : public wxGraphicsPathData
{
public :
    wxMacCoreGraphicsPathData( wxGraphicsRenderer* renderer, CGMutablePathRef path = NULL);

    ~wxMacCoreGraphicsPathData();

    virtual wxGraphicsObjectRefData *Clone() const;

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y );

    // adds a straight line from the current point to (x,y)
    virtual void AddLineToPoint( wxDouble x, wxDouble y );

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y );

    // closes the current sub-path
    virtual void CloseSubpath();

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble* x, wxDouble* y) const;

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

    // adds another path
    virtual void AddPath( const wxGraphicsPathData* path );

    // returns the native path
    virtual void * GetNativePath() const { return m_path; }

    // give the native path returned by GetNativePath() back (there might be some deallocations necessary)
    virtual void UnGetNativePath(void *p) const {}

    // transforms each point of this path by the matrix
    virtual void Transform( const wxGraphicsMatrixData* matrix );

    // gets the bounding box enclosing all points (possibly including control points)
    virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *y) const;

    virtual bool Contains( wxDouble x, wxDouble y, int fillStyle = wxODDEVEN_RULE) const;
private :
    CGMutablePathRef m_path;
};

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsPath implementation
//-----------------------------------------------------------------------------

wxMacCoreGraphicsPathData::wxMacCoreGraphicsPathData( wxGraphicsRenderer* renderer, CGMutablePathRef path) : wxGraphicsPathData(renderer)
{
    if ( path )
        m_path = path;
    else
        m_path = CGPathCreateMutable();
}

wxMacCoreGraphicsPathData::~wxMacCoreGraphicsPathData()
{
    CGPathRelease( m_path );
}

wxGraphicsObjectRefData* wxMacCoreGraphicsPathData::Clone() const
{
    wxMacCoreGraphicsPathData* clone = new wxMacCoreGraphicsPathData(GetRenderer(),CGPathCreateMutableCopy(m_path));
    return clone ;
}


// opens (starts) a new subpath
void wxMacCoreGraphicsPathData::MoveToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathMoveToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPathData::AddLineToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathAddLineToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPathData::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    CGPathAddCurveToPoint( m_path , NULL , cx1 , cy1 , cx2, cy2, x , y );
}

void wxMacCoreGraphicsPathData::AddQuadCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble x, wxDouble y )
{
    CGPathAddQuadCurveToPoint( m_path , NULL , cx1 , cy1 , x , y );
}

void wxMacCoreGraphicsPathData::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    CGRect cgRect = { { x , y } , { w , h } };
    CGPathAddRect( m_path , NULL , cgRect );
}

void wxMacCoreGraphicsPathData::AddCircle( wxDouble x, wxDouble y , wxDouble r )
{
    CGPathAddArc( m_path , NULL , x , y , r , 0.0 , 2 * M_PI , true );
}

// adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
void wxMacCoreGraphicsPathData::AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise )
{
    // inverse direction as we the 'normal' state is a y axis pointing down, ie mirrored to the standard core graphics setup
    CGPathAddArc( m_path, NULL , x, y, r, startAngle, endAngle, !clockwise);
}

void wxMacCoreGraphicsPathData::AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )
{
    CGPathAddArcToPoint( m_path, NULL , x1, y1, x2, y2, r);
}

void wxMacCoreGraphicsPathData::AddPath( const wxGraphicsPathData* path )
{
    CGPathAddPath( m_path , NULL, (CGPathRef) path->GetNativePath() );
}

// closes the current subpath
void wxMacCoreGraphicsPathData::CloseSubpath()
{
    CGPathCloseSubpath( m_path );
}

// gets the last point of the current path, (0,0) if not yet set
void wxMacCoreGraphicsPathData::GetCurrentPoint( wxDouble* x, wxDouble* y) const
{
    CGPoint p = CGPathGetCurrentPoint( m_path );
    *x = p.x;
    *y = p.y;
}

// transforms each point of this path by the matrix
void wxMacCoreGraphicsPathData::Transform( const wxGraphicsMatrixData* matrix )
{
    CGMutablePathRef p = CGPathCreateMutable() ;
    CGPathAddPath( p, (CGAffineTransform*) matrix->GetNativeMatrix() , m_path );
    CGPathRelease( m_path );
    m_path = p;
}

// gets the bounding box enclosing all points (possibly including control points)
void wxMacCoreGraphicsPathData::GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const
{
    CGRect bounds = CGPathGetBoundingBox( m_path ) ;
    *x = bounds.origin.x;
    *y = bounds.origin.y;
    *w = bounds.size.width;
    *h = bounds.size.height;
}

bool wxMacCoreGraphicsPathData::Contains( wxDouble x, wxDouble y, int fillStyle) const
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    if ( CGPathContainsPoint!=NULL )
    {
        return CGPathContainsPoint( m_path, NULL, CGPointMake(x,y), fillStyle == wxODDEVEN_RULE );
    }
    else
#endif
    {
        // TODO : implementation for 10.3
        CGRect bounds = CGPathGetBoundingBox( m_path ) ;
        return CGRectContainsPoint( bounds, CGPointMake(x,y) ) == 1;
    }
}

//
// Graphics Context
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsContext declaration
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMacCoreGraphicsContext : public wxGraphicsContext
{
public:
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer);

    wxMacCoreGraphicsContext();

    ~wxMacCoreGraphicsContext();

    void Init();

    // push the current state of the context, ie the transformation matrix on a stack
    virtual void PushState();

    // pops a stored state from the stack
    virtual void PopState();

    // clips drawings to the region
    virtual void Clip( const wxRegion &region );

    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    // resets the clipping to original extent
    virtual void ResetClip();

    virtual void * GetNativeContext();

    bool SetLogicalFunction( int function );
    //
    // transformation
    //

    // translate
    virtual void Translate( wxDouble dx , wxDouble dy );

    // scale
    virtual void Scale( wxDouble xScale , wxDouble yScale );

    // rotate (radians)
    virtual void Rotate( wxDouble angle );

    // concatenates this transform with the current transform of this context
    virtual void ConcatTransform( const wxGraphicsMatrix& matrix );

    // sets the transform of this context
    virtual void SetTransform( const wxGraphicsMatrix& matrix );

    // gets the matrix of this context
    virtual wxGraphicsMatrix GetTransform() const;
    //
    // setting the paint
    //

    // strokes along a path with the current pen
    virtual void StrokePath( const wxGraphicsPath &path );

    // fills a path with the current brush
    virtual void FillPath( const wxGraphicsPath &path, int fillStyle = wxODDEVEN_RULE );

    // draws a path by first filling and then stroking
    virtual void DrawPath( const wxGraphicsPath &path, int fillStyle = wxODDEVEN_RULE );

    virtual bool ShouldOffset() const
    {     
        int penwidth = 0 ;
        if ( !m_pen.IsNull() )
        {
            penwidth = (int)((wxMacCoreGraphicsPenData*)m_pen.GetRefData())->GetWidth();
            if ( penwidth == 0 )
                penwidth = 1;
        }
        return ( penwidth % 2 ) == 1;
    }
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

    void SetNativeContext( CGContextRef cg );

    DECLARE_NO_COPY_CLASS(wxMacCoreGraphicsContext)
    DECLARE_DYNAMIC_CLASS(wxMacCoreGraphicsContext)

private:
    void EnsureIsValid();

    CGContextRef m_cgContext;
    WindowRef m_windowRef;
    bool m_releaseContext;
    CGAffineTransform m_windowTransform;

    wxMacCFRefHolder<HIShapeRef> m_clipRgn;
};

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
// wxMacCoreGraphicsContext implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsContext, wxGraphicsContext)

void wxMacCoreGraphicsContext::Init()
{
    m_cgContext = NULL;
    m_releaseContext = false;
    m_windowRef = NULL;

    HIRect r = CGRectMake(0,0,0,0);
    m_clipRgn.Set(HIShapeCreateWithRect(&r));
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext ) : wxGraphicsContext(renderer)
{
    Init();
    SetNativeContext(cgcontext);
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window ): wxGraphicsContext(renderer)
{
    Init();
    m_windowRef = window;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window ): wxGraphicsContext(renderer)
{
    Init();
    m_windowRef = (WindowRef) window->MacGetTopLevelWindowRef();
    int originX , originY;
    originX = originY = 0;
    window->MacWindowToRootWindow( &originX , &originY );
    Rect bounds;
    GetWindowBounds( m_windowRef, kWindowContentRgn, &bounds );

    m_windowTransform = CGAffineTransformMakeTranslation( 0 , bounds.bottom - bounds.top );
    m_windowTransform = CGAffineTransformScale( m_windowTransform , 1 , -1 );
    m_windowTransform = CGAffineTransformTranslate( m_windowTransform, originX, originY ) ;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext(wxGraphicsRenderer* renderer) : wxGraphicsContext(renderer)
{
    Init();
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext() : wxGraphicsContext(NULL)
{
    Init();
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsContext::~wxMacCoreGraphicsContext()
{
    SetNativeContext(NULL);
}

void wxMacCoreGraphicsContext::EnsureIsValid()
{
    if ( !m_cgContext )
    {
        OSStatus status = QDBeginCGContext( GetWindowPort( m_windowRef ) , &m_cgContext );
        wxASSERT_MSG( status == noErr , wxT("Cannot nest wxDCs on the same window") );

        CGContextConcatCTM( m_cgContext, m_windowTransform );
		CGContextSaveGState( m_cgContext );
		m_releaseContext = true;
		if ( !HIShapeIsEmpty(m_clipRgn) )
		{
            // the clip region is in device coordinates, so we convert this again to user coordinates
            wxMacCFRefHolder<HIMutableShapeRef> hishape ;
            hishape.Set( HIShapeCreateMutableCopy( m_clipRgn ) );
            CGPoint transformedOrigin = CGPointApplyAffineTransform( CGPointZero,m_windowTransform);
            HIShapeOffset( hishape, -transformedOrigin.x, -transformedOrigin.y );
			HIShapeReplacePathInCGContext( hishape, m_cgContext );
			CGContextClip( m_cgContext );
		}
		CGContextSaveGState( m_cgContext );
	}
}

bool wxMacCoreGraphicsContext::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return true;

    EnsureIsValid();
    
    bool retval = false;

    if ( function == wxCOPY )
    {
        retval = true;
#if wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
        if ( CGContextSetBlendMode != NULL )
        {
            CGContextSetBlendMode( m_cgContext, kCGBlendModeNormal );
            CGContextSetShouldAntialias( m_cgContext, true );
        }
#endif
    }
    else if ( function == wxINVERT || function == wxXOR )
    {
#if wxMAC_USE_CORE_GRAPHICS_BLEND_MODES
        if ( CGContextSetBlendMode != NULL )
        {
            // change color to white
            CGContextSetBlendMode( m_cgContext, kCGBlendModeExclusion );
            CGContextSetShouldAntialias( m_cgContext, false );
            retval = true;
        }
#endif
    }
    
    if (retval)
        m_logicalFunction = function;
    return retval ;
}

void wxMacCoreGraphicsContext::Clip( const wxRegion &region )
{
    if( m_cgContext )
    {
        HIShapeRef shape = HIShapeCreateWithQDRgn( (RgnHandle) region.GetWXHRGN() );
        HIShapeReplacePathInCGContext( shape, m_cgContext );
        CGContextClip( m_cgContext );
        CFRelease( shape );
    }
    else
    {
        // this offsetting to device coords is not really correct, but since we cannot apply affine transforms
        // to regions we try at least to have correct translations
        wxMacCFRefHolder<HIShapeRef> hishape ;
        hishape.Set( HIShapeCreateWithQDRgn( (RgnHandle) region.GetWXHRGN() ));
        HIMutableShapeRef mutableShape = HIShapeCreateMutableCopy( hishape );
        
        CGPoint transformedOrigin = CGPointApplyAffineTransform( CGPointZero, m_windowTransform );
        HIShapeOffset( mutableShape, transformedOrigin.x, transformedOrigin.y );
        m_clipRgn.Set(mutableShape);
    }
}

// clips drawings to the rect
void wxMacCoreGraphicsContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    HIRect r = CGRectMake( x , y , w , h );
    if ( m_cgContext )
    {
        CGContextClipToRect( m_cgContext, r );
    }
    else
    {
        // the clipping itself must be stored as device coordinates, otherwise 
        // we cannot apply it back correctly
        r.origin= CGPointApplyAffineTransform( r.origin, m_windowTransform );
        m_clipRgn.Set(HIShapeCreateWithRect(&r));
    }
}

    // resets the clipping to original extent
void wxMacCoreGraphicsContext::ResetClip()
{
    if ( m_cgContext )
    {
        // there is no way for clearing the clip, we can only revert to the stored
        // state, but then we have to make sure everything else is NOT restored
        CGAffineTransform transform = CGContextGetCTM( m_cgContext );
        CGContextRestoreGState( m_cgContext );
        CGContextSaveGState( m_cgContext );
        CGAffineTransform transformNew = CGContextGetCTM( m_cgContext );
        transformNew = CGAffineTransformInvert( transformNew ) ;
        CGContextConcatCTM( m_cgContext, transformNew);
        CGContextConcatCTM( m_cgContext, transform);
    }
    else
    {
        HIRect r = CGRectMake(0,0,0,0);
        m_clipRgn.Set(HIShapeCreateWithRect(&r));
    }
}

void wxMacCoreGraphicsContext::StrokePath( const wxGraphicsPath &path )
{
    if ( m_pen.IsNull() )
        return ;

    EnsureIsValid();

    bool offset = ShouldOffset();
    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    ((wxMacCoreGraphicsPenData*)m_pen.GetRefData())->Apply(this);
    CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
    CGContextStrokePath( m_cgContext );

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
}

void wxMacCoreGraphicsContext::DrawPath( const wxGraphicsPath &path , int fillStyle )
{
    if ( !m_brush.IsNull() && ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->IsShading() )
    {
        // when using shading, we cannot draw pen and brush at the same time
        // revert to the base implementation of first filling and then stroking
        wxGraphicsContext::DrawPath( path, fillStyle );
        return;
    }

    CGPathDrawingMode mode = kCGPathFill ;
    if ( m_brush.IsNull() )
    {
        if ( m_pen.IsNull() )
            return;
        else
            mode = kCGPathStroke;
    }
    else
    {
        if ( m_pen.IsNull() )
        {
            if ( fillStyle == wxODDEVEN_RULE )
                mode = kCGPathEOFill;
            else
                mode = kCGPathFill;
        }
        else
        {
            if ( fillStyle == wxODDEVEN_RULE )
                mode = kCGPathEOFillStroke;
            else
                mode = kCGPathFillStroke;
        }
    }

    EnsureIsValid();

    if ( !m_brush.IsNull() )
        ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->Apply(this);
    if ( !m_pen.IsNull() )
        ((wxMacCoreGraphicsPenData*)m_pen.GetRefData())->Apply(this);

    bool offset = ShouldOffset();

    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
    CGContextDrawPath( m_cgContext , mode );

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
}

void wxMacCoreGraphicsContext::FillPath( const wxGraphicsPath &path , int fillStyle )
{
    if ( m_brush.IsNull() )
        return;

    EnsureIsValid();

    if ( ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->IsShading() )
    {
        CGContextSaveGState( m_cgContext );
        CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
        CGContextClip( m_cgContext );
        CGContextDrawShading( m_cgContext, ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->GetShading() );
        CGContextRestoreGState( m_cgContext);
    }
    else
    {
        ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->Apply(this);
        CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
        if ( fillStyle == wxODDEVEN_RULE )
            CGContextEOFillPath( m_cgContext );
        else
            CGContextFillPath( m_cgContext );
    }
}

void wxMacCoreGraphicsContext::SetNativeContext( CGContextRef cg )
{
    // we allow either setting or clearing but not replacing
    wxASSERT( m_cgContext == NULL || cg == NULL );

    if ( m_cgContext )
    {
        // TODO : when is this necessary - should we add a Flush() method ? CGContextSynchronize( m_cgContext );
        CGContextRestoreGState( m_cgContext );
        CGContextRestoreGState( m_cgContext );
        if ( m_releaseContext )
            QDEndCGContext( GetWindowPort( m_windowRef ) , &m_cgContext);
        else
            CGContextRelease(m_cgContext);
    }


    m_cgContext = cg;

    // FIXME: This check is needed because currently we need to use a DC/GraphicsContext
    // in order to get font properties, like wxFont::GetPixelSize, but since we don't have
    // a native window attached to use, I create a wxGraphicsContext with a NULL CGContextRef
    // for this one operation.

    // When wxFont::GetPixelSize on Mac no longer needs a graphics context, this check
    // can be removed.
    if (m_cgContext)
    {
        CGContextRetain(m_cgContext);
        CGContextSaveGState( m_cgContext );
        CGContextSaveGState( m_cgContext );
        m_releaseContext = false;
    }
}

void wxMacCoreGraphicsContext::Translate( wxDouble dx , wxDouble dy )
{
    if ( m_cgContext )
        CGContextTranslateCTM( m_cgContext, dx, dy );
    else
        m_windowTransform = CGAffineTransformTranslate(m_windowTransform,dx,dy);
}

void wxMacCoreGraphicsContext::Scale( wxDouble xScale , wxDouble yScale )
{
    if ( m_cgContext )
        CGContextScaleCTM( m_cgContext , xScale , yScale );
    else
        m_windowTransform = CGAffineTransformScale(m_windowTransform,xScale,yScale);
}

void wxMacCoreGraphicsContext::Rotate( wxDouble angle )
{
    if ( m_cgContext )
        CGContextRotateCTM( m_cgContext , angle );
    else
        m_windowTransform = CGAffineTransformRotate(m_windowTransform,angle);
}

void wxMacCoreGraphicsContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    EnsureIsValid();

    CGImageRef image = (CGImageRef)( bmp.CGImageCreate() );
    HIRect r = CGRectMake( x , y , w , h );
    if ( bmp.GetDepth() == 1 )
    {
        // is is a mask, the '1' in the mask tell where to draw the current brush
        if (  !m_brush.IsNull() )
        {
            if ( ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->IsShading() )
            {
                // TODO clip to mask
            /*
                CGContextSaveGState( m_cgContext );
                CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
                CGContextClip( m_cgContext );
                CGContextDrawShading( m_cgContext, ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->GetShading() );
                CGContextRestoreGState( m_cgContext);
            */
            }
            else
            {
                ((wxMacCoreGraphicsBrushData*)m_brush.GetRefData())->Apply(this);
                HIViewDrawCGImage( m_cgContext , &r , image );
            }
        }
    }
    else
    {
        HIViewDrawCGImage( m_cgContext , &r , image );
    }
    CGImageRelease( image );
}

void wxMacCoreGraphicsContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    EnsureIsValid();

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
    EnsureIsValid();

    CGContextSaveGState( m_cgContext );
}

void wxMacCoreGraphicsContext::PopState()
{
    EnsureIsValid();

    CGContextRestoreGState( m_cgContext );
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y )
{
    DrawText(str, x, y, 0.0);
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle )
{
    if ( m_font.IsNull() )
        return;

    EnsureIsValid();

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

    ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

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
    x += (int)(sin(angle) * FixedToInt(ascent));
    y += (int)(cos(angle) * FixedToInt(ascent));

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
    wxCHECK_RET( !m_font.IsNull(), wxT("wxDC(cg)::DoGetTextExtent - no valid font set") );

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

    ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

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

    ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
    ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

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

void * wxMacCoreGraphicsContext::GetNativeContext()
{
    return m_cgContext;
}

// concatenates this transform with the current transform of this context
void wxMacCoreGraphicsContext::ConcatTransform( const wxGraphicsMatrix& matrix )
{
    if ( m_cgContext )
        CGContextConcatCTM( m_cgContext, *(CGAffineTransform*) matrix.GetNativeMatrix());
    else
        m_windowTransform = CGAffineTransformConcat(m_windowTransform, *(CGAffineTransform*) matrix.GetNativeMatrix());
}

// sets the transform of this context
void wxMacCoreGraphicsContext::SetTransform( const wxGraphicsMatrix& matrix )
{
    if ( m_cgContext )
    {
        CGAffineTransform transform = CGContextGetCTM( m_cgContext );
        transform = CGAffineTransformInvert( transform ) ;
        CGContextConcatCTM( m_cgContext, transform);
        CGContextConcatCTM( m_cgContext, *(CGAffineTransform*) matrix.GetNativeMatrix());
    }
    else
    {
        m_windowTransform = *(CGAffineTransform*) matrix.GetNativeMatrix();
    }
}

// gets the matrix of this context
wxGraphicsMatrix wxMacCoreGraphicsContext::GetTransform() const
{
    wxGraphicsMatrix m = CreateMatrix();
    *((CGAffineTransform*) m.GetNativeMatrix()) = ( m_cgContext == NULL ? m_windowTransform :
        CGContextGetCTM( m_cgContext ));
    return m;
}

//
// Renderer
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsRenderer declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMacCoreGraphicsRenderer : public wxGraphicsRenderer
{
public :
    wxMacCoreGraphicsRenderer() {}

    virtual ~wxMacCoreGraphicsRenderer() {}

    // Context

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc);

    virtual wxGraphicsContext * CreateContextFromNativeContext( void * context );

    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window );

    virtual wxGraphicsContext * CreateContext( wxWindow* window );
    
    virtual wxGraphicsContext * CreateMeasuringContext();

    // Path

    virtual wxGraphicsPath CreatePath();

    // Matrix

    virtual wxGraphicsMatrix CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0);


    virtual wxGraphicsPen CreatePen(const wxPen& pen) ;

    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush ) ;

    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual wxGraphicsBrush CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
        const wxColour&c1, const wxColour&c2) ;

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc)
    // with radius r and color cColor
    virtual wxGraphicsBrush CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor) ;

   // sets the font
    virtual wxGraphicsFont CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ) ;

private :
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsRenderer)
} ;

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsRenderer implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsRenderer,wxGraphicsRenderer)

static wxMacCoreGraphicsRenderer gs_MacCoreGraphicsRenderer;

wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return &gs_MacCoreGraphicsRenderer;
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( const wxWindowDC& dc)
{
   return new wxMacCoreGraphicsContext(this,(CGContextRef)dc.GetWindow()->MacGetCGContextRef() );
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContextFromNativeContext( void * context )
{
    return new wxMacCoreGraphicsContext(this,(CGContextRef)context);
}


wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContextFromNativeWindow( void * window )
{
    return new wxMacCoreGraphicsContext(this,(WindowRef)window);
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( wxWindow* window )
{
    return new wxMacCoreGraphicsContext(this, window );
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateMeasuringContext()
{
    return new wxMacCoreGraphicsContext(this);
}

// Path

wxGraphicsPath wxMacCoreGraphicsRenderer::CreatePath()
{
    wxGraphicsPath m;
    m.SetRefData( new wxMacCoreGraphicsPathData(this));
    return m;
}


// Matrix

wxGraphicsMatrix wxMacCoreGraphicsRenderer::CreateMatrix( wxDouble a, wxDouble b, wxDouble c, wxDouble d,
    wxDouble tx, wxDouble ty)
{
    wxGraphicsMatrix m;
    wxMacCoreGraphicsMatrixData* data = new wxMacCoreGraphicsMatrixData( this );
    data->Set( a,b,c,d,tx,ty ) ;
    m.SetRefData(data);
    return m;
}

wxGraphicsPen wxMacCoreGraphicsRenderer::CreatePen(const wxPen& pen)
{
    if ( !pen.Ok() || pen.GetStyle() == wxTRANSPARENT )
        return wxNullGraphicsPen;
    else
    {
        wxGraphicsPen p;
        p.SetRefData(new wxMacCoreGraphicsPenData( this, pen ));
        return p;
    }
}

wxGraphicsBrush wxMacCoreGraphicsRenderer::CreateBrush(const wxBrush& brush )
{
    if ( !brush.Ok() || brush.GetStyle() == wxTRANSPARENT )
        return wxNullGraphicsBrush;
    else
    {
        wxGraphicsBrush p;
        p.SetRefData(new wxMacCoreGraphicsBrushData( this, brush ));
        return p;
    }
}

// sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
wxGraphicsBrush wxMacCoreGraphicsRenderer::CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
    const wxColour&c1, const wxColour&c2)
{
    wxGraphicsBrush p;
    wxMacCoreGraphicsBrushData* d = new wxMacCoreGraphicsBrushData( this );
    d->CreateLinearGradientBrush(x1, y1, x2, y2, c1, c2);
    p.SetRefData(d);
    return p;
}

// sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc)
// with radius r and color cColor
wxGraphicsBrush wxMacCoreGraphicsRenderer::CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor)
{
    wxGraphicsBrush p;
    wxMacCoreGraphicsBrushData* d = new wxMacCoreGraphicsBrushData( this );
    d->CreateRadialGradientBrush(xo,yo,xc,yc,radius,oColor,cColor);
    p.SetRefData(d);
    return p;
}

// sets the font
wxGraphicsFont wxMacCoreGraphicsRenderer::CreateFont( const wxFont &font , const wxColour &col )
{
    if ( font.Ok() )
    {
        wxGraphicsFont p;
        p.SetRefData(new wxMacCoreGraphicsFontData( this , font, col ));
        return p;
    }
    else
        return wxNullGraphicsFont;
}



#endif // wxMAC_USE_CORE_GRAPHICS
