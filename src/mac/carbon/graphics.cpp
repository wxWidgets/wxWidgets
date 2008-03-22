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
#include "wx/private/graphics.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/log.h"
    #include "wx/region.h"
    #include "wx/image.h"
    #include "wx/icon.h"
#endif

#ifdef __MSL__
    #if __MSL__ >= 0x6000
        #include "math.h"
        // in case our functions were defined outside std, we make it known all the same
        namespace std { }
        using namespace std;
    #endif
#endif

#ifdef __WXMAC__
    #include "wx/mac/uma.h"
#else
    #include "CoreServices/CoreServices.h"
    #include "ApplicationServices/ApplicationServices.h"
    #include "wx/mac/corefoundation/cfstring.h"
    #include "wx/cocoa/dcclient.h"
#endif

#ifdef __WXCOCOA__

CGColorSpaceRef wxMacGetGenericRGBColorSpace()
{
    static wxCFRef<CGColorSpaceRef> genericRGBColorSpace;

    if (genericRGBColorSpace == NULL)
    {
        genericRGBColorSpace.reset( CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB ) );
    }

    return genericRGBColorSpace;
}

int UMAGetSystemVersion()
{
    return 0x1050;
}


#define wxMAC_USE_CORE_TEXT 1

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

OSStatus wxMacDrawCGImage(
                  CGContextRef    inContext,
                  const CGRect *  inBounds,
                  CGImageRef      inImage)
{
#if defined( __LP64__ ) || defined(__WXCOCOA__)
    // todo flip
    CGContextDrawImage(inContext, *inBounds, inImage );
    return noErr;
#else
    return HIViewDrawCGImage( inContext, inBounds, inImage );
#endif
}

CGColorRef wxMacCreateCGColor( const wxColour& col )
{
    CGColorRef retval = 0;
#ifdef __WXMAC__
    retval = col.CreateCGColor();
#else
// TODO add conversion NSColor - CGColorRef (obj-c)
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if ( CGColorCreateGenericRGB )
        retval = CGColorCreateGenericRGB( col.Red() / 255.0 , col.Green() / 255.0, col.Blue() / 255.0, col.Alpha() / 255.0 );
    else
#endif
    {
        CGFloat components[4] = { col.Red() / 255.0, col.Green() / 255.0, col.Blue()  / 255.0, col.Alpha() / 255.0 } ;
        retval = CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ;
    }

#endif
    return retval;
}

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5 && defined(wxMAC_USE_CORE_TEXT)

CTFontRef wxMacCreateCTFont( const wxFont& font )
{
#ifdef __WXMAC__
    return wxCFRetain((CTFontRef) font.MacGetCTFont());
#else
    return CTFontCreateWithName( wxCFStringRef( font.GetFaceName(), wxLocale::GetSystemEncoding() ) , font.GetPointSize() , NULL );
#endif
}

#endif

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
#ifdef __WXMAC__
        Init( (CGImageRef) bmp->CreateCGImage() , transform );
#endif
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
            wxMacDrawCGImage( ctxRef, &m_imageBounds, m_image );
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
        CGContextStrokeLineSegments( ctxRef , pts , count );
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
    wxCFRef<CGColorRef> m_color;
    wxCFRef<CGColorSpaceRef> m_colorSpace;

    CGLineJoin m_join;
    CGFloat m_width;

    int m_count;
    const CGFloat *m_lengths;
    CGFloat *m_userLengths;


    bool m_isPattern;
    wxCFRef<CGPatternRef> m_pattern;
    CGFloat* m_patternColorComponents;
};

wxMacCoreGraphicsPenData::wxMacCoreGraphicsPenData( wxGraphicsRenderer* renderer, const wxPen &pen ) :
    wxGraphicsObjectRefData( renderer )
{
    Init();

    m_color.reset( wxMacCreateCGColor( pen.GetColour() ) ) ;

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
                    m_colorSpace.reset( CGColorSpaceCreatePattern( NULL ) );
                    m_pattern.reset( (CGPatternRef) *( new ImagePattern( bmp , CGAffineTransformMakeScale( 1,-1 ) ) ) );
                    m_patternColorComponents = new CGFloat[1] ;
                    m_patternColorComponents[0] = 1.0;
                    m_isPattern = true;
                }
            }
            break;

        default :
            {
                m_isPattern = true;
                m_colorSpace.reset( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
                m_pattern.reset( (CGPatternRef) *( new HatchPattern( pen.GetStyle() , CGAffineTransformMakeScale( 1,-1 ) ) ) );
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

static const char *gs_stripedback_xpm[] = {
/* columns rows colors chars-per-pixel */
"4 4 2 1",
". c #F0F0F0",
"X c #ECECEC",
/* pixels */
"....",
"....",
"XXXX",
"XXXX"
};

wxBitmap gs_stripedback_bmp( wxImage( (const char* const* ) gs_stripedback_xpm  ), -1 ) ;

// make sure we all use one class for all conversions from wx to native colour

class wxMacCoreGraphicsColour
{
    public:
        wxMacCoreGraphicsColour();
        wxMacCoreGraphicsColour(const wxBrush &brush);
        ~wxMacCoreGraphicsColour();

        void Apply( CGContextRef cgContext );
    protected:
        void Init();
        wxCFRef<CGColorRef> m_color;
        wxCFRef<CGColorSpaceRef> m_colorSpace;

        bool m_isPattern;
        wxCFRef<CGPatternRef> m_pattern;
        CGFloat* m_patternColorComponents;
} ;

wxMacCoreGraphicsColour::~wxMacCoreGraphicsColour()
{
    delete[] m_patternColorComponents;
}

void wxMacCoreGraphicsColour::Init()
{
    m_isPattern = false;
    m_patternColorComponents = NULL;
}

void wxMacCoreGraphicsColour::Apply( CGContextRef cgContext )
{
    if ( m_isPattern )
    {
        CGAffineTransform matrix = CGContextGetCTM( cgContext );
        CGContextSetPatternPhase( cgContext, CGSizeMake(matrix.tx, matrix.ty) );
        CGContextSetFillColorSpace( cgContext , m_colorSpace );
        CGContextSetFillPattern( cgContext, m_pattern , m_patternColorComponents );
    }
    else
    {
        CGContextSetFillColorWithColor( cgContext, m_color );
    }
}

wxMacCoreGraphicsColour::wxMacCoreGraphicsColour()
{
    Init();
}

wxMacCoreGraphicsColour::wxMacCoreGraphicsColour( const wxBrush &brush )
{
    Init();
    if ( brush.GetStyle() == wxSOLID )
    {
        m_color.reset( wxMacCreateCGColor( brush.GetColour() ));
    }
    else if ( brush.IsHatch() )
    {
        m_isPattern = true;
        m_colorSpace.reset( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
        m_pattern.reset( (CGPatternRef) *( new HatchPattern( brush.GetStyle() , CGAffineTransformMakeScale( 1,-1 ) ) ) );

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
            m_colorSpace.reset( CGColorSpaceCreatePattern( NULL ) );
            m_pattern.reset( (CGPatternRef) *( new ImagePattern( bmp , CGAffineTransformMakeScale( 1,-1 ) ) ) );
        }
    }
}

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

    wxMacCoreGraphicsColour m_cgColor;

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

wxMacCoreGraphicsBrushData::wxMacCoreGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush &brush) : wxGraphicsObjectRefData( renderer ),
    m_cgColor( brush )
{
    Init();

}

wxMacCoreGraphicsBrushData::~wxMacCoreGraphicsBrushData()
{
    if ( m_shading )
        CGShadingRelease(m_shading);

    if( m_gradientFunction )
        CGFunctionRelease(m_gradientFunction);

    delete[] m_gradientComponents;
}

void wxMacCoreGraphicsBrushData::Init()
{
    m_gradientFunction = NULL;
    m_shading = NULL;
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
        m_cgColor.Apply( cg );
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

#if wxMAC_USE_ATSU_TEXT
    virtual ATSUStyle GetATSUStyle() { return m_macATSUIStyle; }
#endif
#if wxMAC_USE_CORE_TEXT
    CTFontRef GetCTFont() const { return m_ctFont ; }
#endif
    wxColour GetColour() const { return m_colour ; }

    bool GetUnderlined() const { return m_underlined ; }
private :
    wxColour m_colour;
    bool m_underlined;
#if wxMAC_USE_ATSU_TEXT
    ATSUStyle m_macATSUIStyle;
#endif
#if wxMAC_USE_CORE_TEXT
    wxCFRef< CTFontRef > m_ctFont;
#endif
};

wxMacCoreGraphicsFontData::wxMacCoreGraphicsFontData(wxGraphicsRenderer* renderer, const wxFont &font, const wxColour& col) : wxGraphicsObjectRefData( renderer )
{
    m_colour = col;
    m_underlined = font.GetUnderlined();

#if wxMAC_USE_CORE_TEXT
    m_ctFont.reset( wxMacCreateCTFont( font ) );
#endif
#if wxMAC_USE_ATSU_TEXT
    OSStatus status = noErr;
    m_macATSUIStyle = NULL;

    status = ATSUCreateAndCopyStyle( (ATSUStyle) font.MacGetATSUStyle() , &m_macATSUIStyle );

    wxASSERT_MSG( status == noErr, wxT("couldn't create ATSU style") );

    // we need the scale here ...

    Fixed atsuSize = IntToFixed( int( 1 * font.MacGetFontSize()) );
    RGBColor atsuColor ;
    col.GetRGBColor( &atsuColor );
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
#endif
#if wxMAC_USE_CG_TEXT
#endif
}

wxMacCoreGraphicsFontData::~wxMacCoreGraphicsFontData()
{
#if wxMAC_USE_CORE_TEXT
#endif
#if wxMAC_USE_ATSU_TEXT
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL;
    }
#endif
#if wxMAC_USE_CG_TEXT
#endif
}

class wxMacCoreGraphicsBitmapData : public wxGraphicsObjectRefData
{
public:
    wxMacCoreGraphicsBitmapData( wxGraphicsRenderer* renderer, CGImageRef bitmap );
    ~wxMacCoreGraphicsBitmapData();

    virtual CGImageRef GetBitmap() { return m_bitmap; }
private :
    CGImageRef m_bitmap;
};

wxMacCoreGraphicsBitmapData::wxMacCoreGraphicsBitmapData( wxGraphicsRenderer* renderer, CGImageRef bitmap ) : wxGraphicsObjectRefData( renderer )
{
    m_bitmap = bitmap;
}

wxMacCoreGraphicsBitmapData::~wxMacCoreGraphicsBitmapData()
{
    CGImageRelease( m_bitmap );
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

    // gets the component valuess of the matrix
    virtual void Get(wxDouble* a=NULL, wxDouble* b=NULL,  wxDouble* c=NULL,
                     wxDouble* d=NULL, wxDouble* tx=NULL, wxDouble* ty=NULL) const;

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

// gets the component valuess of the matrix
void wxMacCoreGraphicsMatrixData::Get(wxDouble* a, wxDouble* b,  wxDouble* c,
                                      wxDouble* d, wxDouble* tx, wxDouble* ty) const
{
    if (a)  *a = m_matrix.a;
    if (b)  *b = m_matrix.b;
    if (c)  *c = m_matrix.c;
    if (d)  *d = m_matrix.d;
    if (tx) *tx= m_matrix.tx;
    if (ty) *ty= m_matrix.ty;
}

// makes this the inverse matrix
void wxMacCoreGraphicsMatrixData::Invert()
{
    m_matrix = CGAffineTransformInvert( m_matrix );
}

// returns true if the elements of the transformation matrix are equal ?
bool wxMacCoreGraphicsMatrixData::IsEqual( const wxGraphicsMatrixData* t) const
{
    return CGAffineTransformEqualToTransform(m_matrix, *((CGAffineTransform*) t->GetNativeMatrix()));
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
    virtual void UnGetNativePath(void *WXUNUSED(p)) const {}

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
    return CGPathContainsPoint( m_path, NULL, CGPointMake(x,y), fillStyle == wxODDEVEN_RULE );
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
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext, wxDouble width = 0, wxDouble height = 0 );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window );

    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer);

    wxMacCoreGraphicsContext();

    ~wxMacCoreGraphicsContext();

    void Init();

    // returns the size of the graphics context in device coordinates
    virtual void GetSize( wxDouble* width, wxDouble* height);

    virtual void StartPage( wxDouble width, wxDouble height );

    virtual void EndPage();

    virtual void Flush();

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

    virtual void DrawBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h );

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
    wxDouble m_width;
    wxDouble m_height;

    wxCFRef<HIShapeRef> m_clipRgn;
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

class wxQuartzOffsetHelper
{
public :
    wxQuartzOffsetHelper( CGContextRef cg , bool offset )
    {
        m_cg = cg;
        m_offset = offset;
        if ( m_offset )
            CGContextTranslateCTM( m_cg, 0.5, 0.5 );
    }
    ~wxQuartzOffsetHelper( )
    {
        if ( m_offset )
            CGContextTranslateCTM( m_cg, -0.5, -0.5 );
    }
public :
    CGContextRef m_cg;
    bool m_offset;
} ;

void wxMacCoreGraphicsContext::Init()
{
    m_cgContext = NULL;
    m_releaseContext = false;
    m_windowRef = NULL;
    m_width = 0;
    m_height = 0;
    CGRect r = CGRectMake(0,0,0,0);
    m_clipRgn.reset(HIShapeCreateWithRect(&r));
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext, wxDouble width, wxDouble height ) : wxGraphicsContext(renderer)
{
    Init();
    SetNativeContext(cgcontext);
    m_width = width;
    m_height = height;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window ): wxGraphicsContext(renderer)
{
    Init();
    m_windowRef = window;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window ): wxGraphicsContext(renderer)
{
    Init();

    int originX , originY;
    originX = originY = 0;

    Rect bounds = { 0,0,0,0 };
#if defined( __LP64__ ) || defined(__WXCOCOA__)
#else
    m_windowRef = (WindowRef) window->MacGetTopLevelWindowRef();
    window->MacWindowToRootWindow( &originX , &originY );
    GetWindowBounds( m_windowRef, kWindowContentRgn, &bounds );
#endif
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

void wxMacCoreGraphicsContext::GetSize( wxDouble* width, wxDouble* height)
{
    *width = m_width;
    *height = m_height;
}


void wxMacCoreGraphicsContext::StartPage( wxDouble width, wxDouble height )
{
    CGRect r;
    if ( width != 0 && height != 0)
        r = CGRectMake( 0 , 0 , width  , height );
    else
        r = CGRectMake( 0 , 0 , m_width  , m_height );

    CGContextBeginPage(m_cgContext,  &r );
//    CGContextTranslateCTM( m_cgContext , 0 ,  height == 0 ? m_height : height );
//    CGContextScaleCTM( m_cgContext , 1 , -1 );
}

void wxMacCoreGraphicsContext::EndPage()
{
    CGContextEndPage(m_cgContext);
}

void wxMacCoreGraphicsContext::Flush()
{
    CGContextFlush(m_cgContext);
}

void wxMacCoreGraphicsContext::EnsureIsValid()
{
    if ( !m_cgContext )
    {
        OSStatus status =
#if ! ( defined( __LP64__ ) || defined(__WXCOCOA__) )
            QDBeginCGContext( GetWindowPort( m_windowRef ) , &m_cgContext );
#else
            paramErr;
#endif
        if ( status != noErr )
        {
            wxFAIL_MSG("Cannot nest wxDCs on the same window");
        }

        CGContextConcatCTM( m_cgContext, m_windowTransform );
        CGContextSaveGState( m_cgContext );
        m_releaseContext = true;
        if ( !HIShapeIsEmpty(m_clipRgn) )
        {
            // the clip region is in device coordinates, so we convert this again to user coordinates
            wxCFRef<HIMutableShapeRef> hishape( HIShapeCreateMutableCopy( m_clipRgn ) );
            CGPoint transformedOrigin = CGPointApplyAffineTransform( CGPointZero,m_windowTransform);
            HIShapeOffset( hishape, -transformedOrigin.x, -transformedOrigin.y );
            HIShapeReplacePathInCGContext( hishape, m_cgContext );
            CGContextClip( m_cgContext );
        }
        CGContextSaveGState( m_cgContext );
    }
}

// TODO test whether the private CGContextSetCompositeOperation works under 10.3 (using NSCompositingModes)

bool wxMacCoreGraphicsContext::SetLogicalFunction( int function )
{
    if (m_logicalFunction == function)
        return true;

    EnsureIsValid();

    bool retval = false;
    bool shouldAntiAlias = true;
    CGBlendMode mode = kCGBlendModeNormal;

#if defined(__WXMAC__) && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5 )
    if ( UMAGetSystemVersion() >= 0x1050 )
    {
        retval = true;
        switch ( function )
        {
            // TODO find best corresponding porter duff modes
            case wxCOPY :
                mode = kCGBlendModeCopy;
                break;
            case wxCLEAR :
                mode = kCGBlendModeClear;
                break;
            case wxXOR :
                mode = kCGBlendModeXOR;
                shouldAntiAlias = false;
                break;
            default :
                retval = false;
                break;
        }
    }
    else
#endif
    {
        if ( function == wxCOPY )
        {
            retval = true;
        }
        else if ( function == wxINVERT || function == wxXOR )
        {
            // change color to white
            mode = kCGBlendModeExclusion;
            shouldAntiAlias = false;
            retval = true;
        }
    }

    if (retval)
    {
        m_logicalFunction = function;
        CGContextSetBlendMode( m_cgContext, mode );
        CGContextSetShouldAntialias(m_cgContext, shouldAntiAlias);
    }
    return retval ;
}

void wxMacCoreGraphicsContext::Clip( const wxRegion &region )
{
#ifdef __WXMAC__
    if( m_cgContext )
    {
        HIShapeReplacePathInCGContext( region.GetWXHRGN() , m_cgContext );
        CGContextClip( m_cgContext );
    }
    else
    {
        // this offsetting to device coords is not really correct, but since we cannot apply affine transforms
        // to regions we try at least to have correct translations
        HIMutableShapeRef mutableShape = HIShapeCreateMutableCopy( region.GetWXHRGN() );

        CGPoint transformedOrigin = CGPointApplyAffineTransform( CGPointZero, m_windowTransform );
        HIShapeOffset( mutableShape, transformedOrigin.x, transformedOrigin.y );
        m_clipRgn.reset(mutableShape);
    }
#endif
}

// clips drawings to the rect
void wxMacCoreGraphicsContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    CGRect r = CGRectMake( x , y , w , h );
    if ( m_cgContext )
    {
        CGContextClipToRect( m_cgContext, r );
    }
    else
    {
        // the clipping itself must be stored as device coordinates, otherwise
        // we cannot apply it back correctly
        r.origin= CGPointApplyAffineTransform( r.origin, m_windowTransform );
        m_clipRgn.reset(HIShapeCreateWithRect(&r));
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
        CGRect r = CGRectMake(0,0,0,0);
        m_clipRgn.reset(HIShapeCreateWithRect(&r));
    }
}

void wxMacCoreGraphicsContext::StrokePath( const wxGraphicsPath &path )
{
    if ( m_pen.IsNull() )
        return ;

    EnsureIsValid();

    wxQuartzOffsetHelper helper( m_cgContext , ShouldOffset() );

    ((wxMacCoreGraphicsPenData*)m_pen.GetRefData())->Apply(this);
    CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
    CGContextStrokePath( m_cgContext );
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

    wxQuartzOffsetHelper helper( m_cgContext , ShouldOffset() );

    CGContextAddPath( m_cgContext , (CGPathRef) path.GetNativePath() );
    CGContextDrawPath( m_cgContext , mode );
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
        {
#if ! ( defined( __LP64__ ) || defined(__WXCOCOA__) )
            QDEndCGContext( GetWindowPort( m_windowRef ) , &m_cgContext);
#endif
        }
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
        CGContextSetTextMatrix( m_cgContext, CGAffineTransformIdentity );
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
    wxGraphicsBitmap bitmap = GetRenderer()->CreateBitmap(bmp);
    DrawBitmap(bitmap, x, y, w, h);
}

void wxMacCoreGraphicsContext::DrawBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    EnsureIsValid();
#ifdef __WXMAC__
    CGImageRef image = static_cast<wxMacCoreGraphicsBitmapData*>(bmp.GetRefData())->GetBitmap();
    CGRect r = CGRectMake( x , y , w , h );
    // if ( bmp.GetDepth() == 1 )
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
                wxMacDrawCGImage( m_cgContext , &r , image );
            }
        }
    }
    /*
    else
    {
        wxMacDrawCGImage( m_cgContext , &r , image );
    }
    CGImageRelease( image );
    */
#endif
}

void wxMacCoreGraphicsContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    EnsureIsValid();

    CGRect r = CGRectMake( 00 , 00 , w , h );
    CGContextSaveGState( m_cgContext );
    CGContextTranslateCTM( m_cgContext, x , y + h );
    CGContextScaleCTM( m_cgContext, 1, -1 );
#ifdef __WXMAC__
    PlotIconRefInContext( m_cgContext , &r , kAlignNone , kTransformNone ,
        NULL , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) );
#endif
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
    if ( m_font.IsNull() )
        return;

    EnsureIsValid();
#if wxMAC_USE_CORE_TEXT
    if ( UMAGetSystemVersion() >= 0x1050 )
    {
        wxMacCoreGraphicsFontData* fref = (wxMacCoreGraphicsFontData*)m_font.GetRefData();
        wxCFStringRef text(str, wxLocale::GetSystemEncoding() );
        CTFontRef font = fref->GetCTFont();
        CGColorRef col = wxMacCreateCGColor( fref->GetColour() );
        CTUnderlineStyle ustyle = fref->GetUnderlined() ? kCTUnderlineStyleSingle : kCTUnderlineStyleNone ;
        wxCFRef<CFNumberRef> underlined( CFNumberCreate(NULL, kCFNumberSInt32Type, &ustyle) );
         CFStringRef keys[] = { kCTFontAttributeName , kCTForegroundColorAttributeName, kCTUnderlineStyleAttributeName };
        CFTypeRef values[] = { font, col, underlined };
        wxCFRef<CFDictionaryRef> attributes( CFDictionaryCreate(kCFAllocatorDefault, (const void**) &keys, (const void**) &values,
                                                        WXSIZEOF( keys ), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks) );
        wxCFRef<CFAttributedStringRef> attrtext( CFAttributedStringCreate(kCFAllocatorDefault, text, attributes) );
        wxCFRef<CTLineRef> line( CTLineCreateWithAttributedString(attrtext) );

        y += CTFontGetAscent(font);

        CGContextSaveGState(m_cgContext);
        CGContextTranslateCTM(m_cgContext, x, y);
        CGContextScaleCTM(m_cgContext, 1, -1);
        CGContextSetTextPosition(m_cgContext, 0, 0);
        CTLineDraw( line, m_cgContext );
        CGContextRestoreGState(m_cgContext);
        CFRelease( col );
        return;
    }
#endif
#if wxMAC_USE_ATSU_TEXT
    {
        DrawText(str, x, y, 0.0);
        return;
    }
#endif
#if wxMAC_USE_CG_TEXT
    // TODO core graphics text implementation here
#endif
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle )
{
    if ( m_font.IsNull() )
        return;

    EnsureIsValid();
#if wxMAC_USE_CORE_TEXT
    if ( UMAGetSystemVersion() >= 0x1050 )
    {
        // default implementation takes care of rotation and calls non rotated DrawText afterwards
        wxGraphicsContext::DrawText( str, x, y, angle );
        return;
    }
#endif
#if wxMAC_USE_ATSU_TEXT
    {
        OSStatus status = noErr;
        ATSUTextLayout atsuLayout;
        wxMacUniCharBuffer unibuf( str );
        UniCharCount chars = unibuf.GetChars();

        ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
        status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
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

        return;
    }
#endif
#if wxMAC_USE_CG_TEXT
    // default implementation takes care of rotation and calls non rotated DrawText afterwards
    wxGraphicsContext::DrawText( str, x, y, angle );
#endif
}

void wxMacCoreGraphicsContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                            wxDouble *descent, wxDouble *externalLeading ) const
{
    wxCHECK_RET( !m_font.IsNull(), wxT("wxDC(cg)::DoGetTextExtent - no valid font set") );

    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    if (str.empty())
        return;

#if wxMAC_USE_CORE_TEXT
    if ( UMAGetSystemVersion() >= 0x1050 )
    {
        wxMacCoreGraphicsFontData* fref = (wxMacCoreGraphicsFontData*)m_font.GetRefData();
        CTFontRef font = fref->GetCTFont();

        wxCFStringRef text(str, wxLocale::GetSystemEncoding() );
        CFStringRef keys[] = { kCTFontAttributeName  };
        CFTypeRef values[] = { font };
        wxCFRef<CFDictionaryRef> attributes( CFDictionaryCreate(kCFAllocatorDefault, (const void**) &keys, (const void**) &values,
                                                                WXSIZEOF( keys ), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks) );
        wxCFRef<CFAttributedStringRef> attrtext( CFAttributedStringCreate(kCFAllocatorDefault, text, attributes) );
        wxCFRef<CTLineRef> line( CTLineCreateWithAttributedString(attrtext) );

        CGFloat w, a, d, l;

        w = CTLineGetTypographicBounds(line, &a, &d, &l) ;

        if ( height )
            *height = a+d+l;
        if ( descent )
            *descent = d;
        if ( externalLeading )
            *externalLeading = l;
        if ( width )
            *width = w;
        return;
    }
#endif
#if wxMAC_USE_ATSU_TEXT
    {
        OSStatus status = noErr;

        ATSUTextLayout atsuLayout;
        wxMacUniCharBuffer unibuf( str );
        UniCharCount chars = unibuf.GetChars();

        ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
        status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
                                                   &chars , &style , &atsuLayout );

        wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

        status = ::ATSUSetTransientFontMatching( atsuLayout , true );
        wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

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

        return;
    }
#endif
#if wxMAC_USE_CG_TEXT
    // TODO core graphics text implementation here
#endif
}

void wxMacCoreGraphicsContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return;

#if wxMAC_USE_CORE_TEXT
    {
        wxMacCoreGraphicsFontData* fref = (wxMacCoreGraphicsFontData*)m_font.GetRefData();
        CTFontRef font = fref->GetCTFont();

        wxCFStringRef t(text, wxLocale::GetSystemEncoding() );
        CFStringRef keys[] = { kCTFontAttributeName  };
        CFTypeRef values[] = { font };
        wxCFRef<CFDictionaryRef> attributes( CFDictionaryCreate(kCFAllocatorDefault, (const void**) &keys, (const void**) &values,
                                                                WXSIZEOF( keys ), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks) );
        wxCFRef<CFAttributedStringRef> attrtext( CFAttributedStringCreate(kCFAllocatorDefault, t, attributes) );
        wxCFRef<CTLineRef> line( CTLineCreateWithAttributedString(attrtext) );

        int chars = text.length();
        for ( int pos = 0; pos < (int)chars; pos ++ )
        {
            widths[pos] = CTLineGetOffsetForStringIndex( line, pos+1 , NULL )+0.5;
        }

        return;
    }
#endif
#if wxMAC_USE_ATSU_TEXT
    {
        OSStatus status = noErr;
        ATSUTextLayout atsuLayout;
        wxMacUniCharBuffer unibuf( text );
        UniCharCount chars = unibuf.GetChars();

        ATSUStyle style = (((wxMacCoreGraphicsFontData*)m_font.GetRefData())->GetATSUStyle());
        status = ::ATSUCreateTextLayoutWithTextPtr( unibuf.GetBuffer() , 0 , chars , chars , 1 ,
                                          &chars , &style , &atsuLayout );

        wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

        status = ::ATSUSetTransientFontMatching( atsuLayout , true );
        wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

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
#endif
#if wxMAC_USE_CG_TEXT
    // TODO core graphics text implementation here
#endif
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
    virtual wxGraphicsContext * CreateContext( const wxMemoryDC& dc);

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

    // create a native bitmap representation
    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) ;

    // create a native bitmap representation
    virtual wxGraphicsBitmap CreateSubBitmap( const wxGraphicsBitmap &bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h  ) ;
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

#ifdef __WXCOCOA__
extern CGContextRef wxMacGetContextFromCurrentNSContext() ;
#endif

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( const wxWindowDC& dc )
{
    const wxDCImpl* impl = dc.GetImpl();
    wxWindowDCImpl *win_impl = wxDynamicCast( impl, wxWindowDCImpl );
    if (win_impl)
    {
        int w, h;
        win_impl->GetSize( &w, &h );
        CGContextRef cgctx = 0;
#ifdef __WXMAC__
        cgctx =  (CGContextRef)(win_impl->GetWindow()->MacGetCGContextRef());
#else
        cgctx = wxMacGetContextFromCurrentNSContext() ;
#endif
        return new wxMacCoreGraphicsContext( this, cgctx, (wxDouble) w, (wxDouble) h );
    }
    return NULL;
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( const wxMemoryDC& dc )
{
#ifdef __WXMAC__
    const wxDCImpl* impl = dc.GetImpl();
    wxMemoryDCImpl *mem_impl = wxDynamicCast( impl, wxMemoryDCImpl );
    if (mem_impl)
    {
        int w, h;
        mem_impl->GetSize( &w, &h );
        return new wxMacCoreGraphicsContext( this,
            (CGContextRef)(mem_impl->GetGraphicsContext()->GetNativeContext()), (wxDouble) w, (wxDouble) h );
    }
#endif
    return NULL;
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

wxGraphicsBitmap wxMacCoreGraphicsRenderer::CreateBitmap( const wxBitmap& bmp )
{
    if ( bmp.Ok() )
    {
        wxGraphicsBitmap p;
#ifdef __WXMAC__
        p.SetRefData(new wxMacCoreGraphicsBitmapData( this , bmp.CreateCGImage() ) );
#endif
        return p;
    }
    else
        return wxNullGraphicsBitmap;
}

wxGraphicsBitmap wxMacCoreGraphicsRenderer::CreateSubBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h  )
{
    CGImageRef img = static_cast<wxMacCoreGraphicsBitmapData*>(bmp.GetRefData())->GetBitmap();
    if ( img )
    {
        wxGraphicsBitmap p;
        CGImageRef subimg = CGImageCreateWithImageInRect(img,CGRectMake( x , y , w , h ));
        p.SetRefData(new wxMacCoreGraphicsBitmapData( this , subimg ) );
        return p;
    }
    else
        return wxNullGraphicsBitmap;
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

//
// CoreGraphics Helper Methods
//

// Data Providers and Consumers

size_t UMAPutBytesCFRefCallback( void *info, const void *bytes, size_t count )
{
    CFMutableDataRef data = (CFMutableDataRef) info;
    if ( data )
    {
        CFDataAppendBytes( data, (const UInt8*) bytes, count );
    }
    return count;
}

void wxMacReleaseCFDataProviderCallback(void *info,
                                      const void *WXUNUSED(data),
                                      size_t WXUNUSED(count))
{
    if ( info )
        CFRelease( (CFDataRef) info );
}

void wxMacReleaseCFDataConsumerCallback( void *info )
{
    if ( info )
        CFRelease( (CFDataRef) info );
}

CGDataProviderRef wxMacCGDataProviderCreateWithCFData( CFDataRef data )
{
    if ( data == NULL )
        return NULL;

    return CGDataProviderCreateWithCFData( data );
}

CGDataConsumerRef wxMacCGDataConsumerCreateWithCFData( CFMutableDataRef data )
{
    if ( data == NULL )
        return NULL;

    return CGDataConsumerCreateWithCFData( data );
}

void
wxMacReleaseMemoryBufferProviderCallback(void *info,
                                         const void * WXUNUSED_UNLESS_DEBUG(data),
                                         size_t WXUNUSED(size))
{
    wxMemoryBuffer* membuf = (wxMemoryBuffer*) info ;

    wxASSERT( data == membuf->GetData() ) ;

    delete membuf ;
}

CGDataProviderRef wxMacCGDataProviderCreateWithMemoryBuffer( const wxMemoryBuffer& buf )
{
    wxMemoryBuffer* b = new wxMemoryBuffer( buf );
    if ( b->GetDataLen() == 0 )
        return NULL;

    return CGDataProviderCreateWithData( b , (const void *) b->GetData() , b->GetDataLen() ,
                                                 wxMacReleaseMemoryBufferProviderCallback );
}
