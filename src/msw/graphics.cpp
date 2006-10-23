/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/graphics.cpp
// Purpose:     wxGCDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-09-30
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dc.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/msw/wrapcdlg.h"
#include "wx/image.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/log.h"
#include "wx/icon.h"
#include "wx/dcprint.h"
#include "wx/module.h"
#endif

#include "wx/graphics.h"

#if wxUSE_GRAPHICS_CONTEXT

#include <vector>

using namespace std;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const double RAD2DEG = 180.0 / M_PI;

//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------

static inline double dmin(double a, double b) { return a < b ? a : b; }
static inline double dmax(double a, double b) { return a > b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }
static inline double RadToDeg(double deg) { return (deg * 180.0) / M_PI; }

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxGDIPlusContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicsPath implementation
//-----------------------------------------------------------------------------

#include "wx/msw/private.h" // needs to be before #include <commdlg.h>

#if wxUSE_COMMON_DIALOGS && !defined(__WXMICROWIN__)
#include <commdlg.h>
#endif

// TODO remove this dependency (gdiplus needs the macros)

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include "gdiplus.h"
using namespace Gdiplus;

class GDILoader
{
public :
    GDILoader() 
    {
        m_loaded = false;
        m_gditoken = NULL;
    }

    ~GDILoader()
    {
        if (m_loaded)
        {
            Unload();
        }
    }
    void EnsureIsLoaded()
    {
        if (!m_loaded)
        {
            Load();
        }
    }
    void Load()
    {
        GdiplusStartupInput input;
        GdiplusStartupOutput output;
        GdiplusStartup(&m_gditoken,&input,&output);
        m_loaded = true;
    }
    void Unload()
    {
        if ( m_gditoken )
            GdiplusShutdown(m_gditoken);
    }
private :
    bool m_loaded;
    DWORD m_gditoken;

};

static GDILoader gGDILoader;

class WXDLLEXPORT wxGDIPlusPath : public wxGraphicsPath
{
public :
    wxGDIPlusPath();
    ~wxGDIPlusPath();


    //
    // These are the path primitives from which everything else can be constructed
    //

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y );

    // adds a straight line from the current point to (x,y) 
    virtual void AddLineToPoint( wxDouble x, wxDouble y );

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y );


    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle 
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise ) ;

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble& x, wxDouble&y) ;

    // closes the current sub-path
    virtual void CloseSubpath();

    //
    // These are convenience functions which - if not available natively will be assembled 
    // using the primitives from above
    //

    // appends a rectangle as a new closed subpath 
    virtual void AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) ;
    /*

    // appends an ellipsis as a new closed subpath fitting the passed rectangle
    virtual void AddEllipsis( wxDouble x, wxDouble y, wxDouble w , wxDouble h ) ;

    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )  ;
*/

	// returns the native path
	virtual void * GetNativePath() const { return m_path; }
	
	// give the native path returned by GetNativePath() back (there might be some deallocations necessary)
	virtual void UnGetNativePath(void * WXUNUSED(path)) {}

private :
    GraphicsPath* m_path;
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGDIPlusPath)
};

class WXDLLEXPORT wxGDIPlusContext : public wxGraphicsContext
{
public:
    wxGDIPlusContext( HDC hdc );
    wxGDIPlusContext( HWND hwnd );
    wxGDIPlusContext( Graphics* gr);
    wxGDIPlusContext();

    virtual ~wxGDIPlusContext();

    virtual void Clip( const wxRegion &region );
    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h );
	
	// resets the clipping to original extent
	virtual void ResetClip();

	virtual void * GetNativeContext();
	
    virtual void StrokePath( const wxGraphicsPath *p );
    virtual void FillPath( const wxGraphicsPath *p , int fillStyle = wxWINDING_RULE );

    virtual wxGraphicsPath* CreatePath();
    virtual void SetPen( const wxPen &pen );
    virtual void SetBrush( const wxBrush &brush );
    virtual void SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, const wxColour&c1, const wxColour&c2) ;
    virtual void SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor);

    virtual void Translate( wxDouble dx , wxDouble dy );    
    virtual void Scale( wxDouble xScale , wxDouble yScale );    
    virtual void Rotate( wxDouble angle );    

    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h );
    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h );
    virtual void PushState();    
    virtual void PopState();

    virtual void SetFont( const wxFont &font );
    virtual void SetTextColor( const wxColour &col );    
    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y);
    virtual void GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
        wxDouble *descent, wxDouble *externalLeading ) const;
    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const;

private:
    void    Init();
    void    SetDefaults();

    Graphics* m_context;
    vector<GraphicsState> m_stateStack;
    GraphicsState m_state1;
    GraphicsState m_state2;

    Pen* m_pen;
    bool m_penTransparent;
    Image* m_penImage;
    Brush* m_penBrush;
 
    Brush* m_brush;
    bool m_brushTransparent;
    Image* m_brushImage;
    GraphicsPath* m_brushPath;

    Brush* m_textBrush;
    Font* m_font;
    // wxPen m_pen;
    // wxBrush m_brush;
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxGDIPlusContext)
};

IMPLEMENT_DYNAMIC_CLASS(wxGDIPlusPath,wxGraphicsPath)

wxGDIPlusPath::wxGDIPlusPath()
{
    m_path = new GraphicsPath();
}

wxGDIPlusPath::~wxGDIPlusPath()
{
    delete m_path;
}

//
// The Primitives
//

void wxGDIPlusPath::MoveToPoint( wxDouble x , wxDouble y )
{
    m_path->StartFigure();
    m_path->AddLine((REAL) x,(REAL) y,(REAL) x,(REAL) y);
}

void wxGDIPlusPath::AddLineToPoint( wxDouble x , wxDouble y )
{
    m_path->AddLine((REAL) x,(REAL) y,(REAL) x,(REAL) y);
}

void wxGDIPlusPath::CloseSubpath()
{
    m_path->CloseFigure();
}

void wxGDIPlusPath::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    PointF c1(cx1,cy1);
    PointF c2(cx2,cy2);
    PointF end(x,y);
    PointF start;
    m_path->GetLastPoint(&start);
    m_path->AddBezier(start,c1,c2,end);
}

// gets the last point of the current path, (0,0) if not yet set
void wxGDIPlusPath::GetCurrentPoint( wxDouble& x, wxDouble&y) 
{
    PointF start;
    m_path->GetLastPoint(&start);
    x = start.X ;
    y = start.Y ;
}

void wxGDIPlusPath::AddArc( wxDouble x, wxDouble y, wxDouble r, double startAngle, double endAngle, bool clockwise ) 
{
    double sweepAngle = endAngle - startAngle ;
    if( abs(sweepAngle) >= 2*M_PI)
    {
        sweepAngle = 2 * M_PI;
    }
    else
    {
        if ( clockwise )
        {
            if( sweepAngle < 0 )
                sweepAngle += 2 * M_PI;
        }
        else
        {
            if( sweepAngle > 0 )
                sweepAngle -= 2 * M_PI;

        }
   }
   m_path->AddArc((REAL) (x-r),(REAL) (y-r),(REAL) (2*r),(REAL) (2*r),RadToDeg(startAngle),RadToDeg(sweepAngle)); 
}

void wxGDIPlusPath::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    m_path->AddRectangle(RectF(x,y,w,h));
}

//-----------------------------------------------------------------------------
// wxGDIPlusContext implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGDIPlusContext,wxGraphicsContext)

wxGDIPlusContext::wxGDIPlusContext( HDC hdc  )
{
    Init();
    m_context = new Graphics( hdc);
    SetDefaults();
}

wxGDIPlusContext::wxGDIPlusContext( HWND hwnd  )
{
    Init();
    m_context = new Graphics( hwnd);
    SetDefaults();
}

wxGDIPlusContext::wxGDIPlusContext( Graphics* gr  )
{
    Init();
    m_context = gr;
    SetDefaults();
}

void wxGDIPlusContext::Init()
{
    gGDILoader.EnsureIsLoaded();
    m_context = NULL;
    m_state1 = 0;
    m_state2= 0;

    m_pen = NULL;
    m_penTransparent = true;
    m_penImage = NULL;
    m_penBrush = NULL;

    m_brush = NULL;
    m_brushTransparent = true;
    m_brushImage = NULL;
    m_brushPath = NULL;

    m_textBrush = NULL;
    m_font = NULL;
}

void wxGDIPlusContext::SetDefaults()
{
    m_context->SetSmoothingMode(SmoothingModeHighQuality);
    m_state1 = m_context->Save();
    m_state2 = m_context->Save();
    // set defaults

    m_penTransparent = false;
    m_pen = new Pen((ARGB)Color::Black);
    m_penImage = NULL;
    m_penBrush = NULL;

    m_brushTransparent = false;
    m_brush = new SolidBrush((ARGB)Color::White);
    m_brushImage = NULL;
    m_brushPath = NULL;
    m_textBrush = new SolidBrush((ARGB)Color::Black);
    m_font = new Font( L"Arial" , 9 , FontStyleRegular );
}

wxGDIPlusContext::~wxGDIPlusContext()
{
    if ( m_context )
    {
        m_context->Restore( m_state2 );
        m_context->Restore( m_state1 );
        delete m_context;
        delete m_pen;
        delete m_penImage;
        delete m_penBrush;
        delete m_brush;
        delete m_brushImage;
        delete m_brushPath;
        delete m_textBrush;
        delete m_font;
    }
}


void wxGDIPlusContext::Clip( const wxRegion &region )
{
    m_context->SetClip((HRGN)region.GetHRGN(),CombineModeIntersect);
}

void wxGDIPlusContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    m_context->SetClip(RectF(x,y,w,h),CombineModeIntersect);
}
	
void wxGDIPlusContext::ResetClip()
{
    m_context->ResetClip();
}

void wxGDIPlusContext::StrokePath( const wxGraphicsPath *path )
{
    if ( m_penTransparent )
        return;

    m_context->DrawPath( m_pen , (GraphicsPath*) path->GetNativePath() );
}

void wxGDIPlusContext::FillPath( const wxGraphicsPath *path , int fillStyle )
{
    if ( !m_brushTransparent )
    {
        ((GraphicsPath*) path->GetNativePath())->SetFillMode( fillStyle == wxODDEVEN_RULE ? FillModeAlternate : FillModeWinding);
        m_context->FillPath( m_brush , (GraphicsPath*) path->GetNativePath());
    }
}

wxGraphicsPath* wxGDIPlusContext::CreatePath()
{
    return new wxGDIPlusPath();
}

void wxGDIPlusContext::Rotate( wxDouble angle ) 
{
    m_context->RotateTransform( RadToDeg(angle) );
}

void wxGDIPlusContext::Translate( wxDouble dx , wxDouble dy ) 
{
    m_context->TranslateTransform( dx , dy );
}

void wxGDIPlusContext::Scale( wxDouble xScale , wxDouble yScale )
{
    m_context->ScaleTransform(xScale,yScale);
}

void wxGDIPlusContext::PushState()
{
    GraphicsState state = m_context->Save();
    m_stateStack.push_back(state);
}

void wxGDIPlusContext::PopState() 
{
    GraphicsState state = m_stateStack.back();
    m_stateStack.pop_back();
    m_context->Restore(state);
}

void wxGDIPlusContext::SetTextColor( const wxColour &col ) 
{
    delete m_textBrush;
    m_textBrush = new SolidBrush( Color( col.Alpha() , col.Red() ,
        col.Green() , col.Blue() ));
}

void wxGDIPlusContext::SetPen( const wxPen &pen )
{
    m_penTransparent = pen.GetStyle() == wxTRANSPARENT;
    if ( m_penTransparent )
        return;
    
    m_pen->SetColor( Color( pen.GetColour().Alpha() , pen.GetColour().Red() ,
        pen.GetColour().Green() , pen.GetColour().Blue() ) );

    // TODO: * m_dc->m_scaleX
    double penWidth = pen.GetWidth();
    if (penWidth <= 0.0)
        penWidth = 0.1;

    m_pen->SetWidth(penWidth);
    
    LineCap cap;
    switch ( pen.GetCap() )
    {
    case wxCAP_ROUND :
        cap = LineCapRound;
        break;

    case wxCAP_PROJECTING :
        cap = LineCapSquare;
        break;

    case wxCAP_BUTT :
        cap = LineCapFlat; // TODO verify
        break;

    default :
        cap = LineCapFlat;
        break;
    }
    m_pen->SetLineCap(cap,cap, DashCapFlat);

    LineJoin join;
    switch ( pen.GetJoin() )
    {
    case wxJOIN_BEVEL :
        join = LineJoinBevel;
        break;

    case wxJOIN_MITER :
        join = LineJoinMiter;
        break;

    case wxJOIN_ROUND :
        join = LineJoinRound;
        break;

    default :
        join = LineJoinMiter;
        break;
    }
    
    m_pen->SetLineJoin(join);

    m_pen->SetDashStyle(DashStyleSolid);

    DashStyle dashStyle = DashStyleSolid;
    switch ( pen.GetStyle() )
    {
    case wxSOLID :
        break;

    case wxDOT :
        dashStyle = DashStyleDot;
        break;

    case wxLONG_DASH :
        dashStyle = DashStyleDash; // TODO verify
        break;

    case wxSHORT_DASH :
        dashStyle = DashStyleDash;
        break;

    case wxDOT_DASH :
        dashStyle = DashStyleDashDot;
        break;
    case wxUSER_DASH :
        {
            dashStyle = DashStyleCustom;
            wxDash *dashes;
            int count = pen.GetDashes( &dashes );
            if ((dashes != NULL) && (count > 0))
            {
                REAL *userLengths = new REAL[count];
                for ( int i = 0; i < count; ++i )
                {
                    userLengths[i] = dashes[i];
                }
                m_pen->SetDashPattern( userLengths, count);
                delete[] userLengths;
            }
        }
        break;
    case wxSTIPPLE :
        {
            wxBitmap* bmp = pen.GetStipple();
            if ( bmp && bmp->Ok() )
            {
                wxDELETE( m_penImage );
                wxDELETE( m_penBrush );
                m_penImage = Bitmap::FromHBITMAP((HBITMAP)bmp->GetHBITMAP(),(HPALETTE)bmp->GetPalette()->GetHPALETTE());
                m_penBrush = new TextureBrush(m_penImage);
                m_pen->SetBrush( m_penBrush );
            }

        }
        break;
    default :
        if ( pen.GetStyle() >= wxFIRST_HATCH && pen.GetStyle() <= wxLAST_HATCH )
        {
            wxDELETE( m_penBrush );
            HatchStyle style = HatchStyleHorizontal;
            switch( pen.GetStyle() )
            {
            case wxBDIAGONAL_HATCH :
                style = HatchStyleBackwardDiagonal;
                break ;
            case wxCROSSDIAG_HATCH :
                style = HatchStyleDiagonalCross;
                break ;
            case wxFDIAGONAL_HATCH :
                style = HatchStyleForwardDiagonal;
                break ;
            case wxCROSS_HATCH :
                style = HatchStyleCross;
                break ;
            case wxHORIZONTAL_HATCH :
                style = HatchStyleHorizontal;
                break ;
            case wxVERTICAL_HATCH :
                style = HatchStyleVertical;
                break ;

            }
            m_penBrush = new HatchBrush(style,Color( pen.GetColour().Alpha() , pen.GetColour().Red() ,
                pen.GetColour().Green() , pen.GetColour().Blue() ), Color::Transparent );
            m_pen->SetBrush( m_penBrush );
        }
        break;
    } 
    if ( dashStyle != DashStyleSolid )
        m_pen->SetDashStyle(dashStyle);
}

void wxGDIPlusContext::SetBrush( const wxBrush &brush )
{
//    m_brush = brush;
    if ( m_context == NULL )
        return;

    m_brushTransparent = brush.GetStyle() == wxTRANSPARENT;

    if ( m_brushTransparent )
        return;

    wxDELETE(m_brush);

    if ( brush.GetStyle() == wxSOLID)
    {
        m_brush = new SolidBrush( Color( brush.GetColour().Alpha() , brush.GetColour().Red() ,
            brush.GetColour().Green() , brush.GetColour().Blue() ) );
    }
    else if ( brush.IsHatch() )
    {
        HatchStyle style = HatchStyleHorizontal;
        switch( brush.GetStyle() )
        {
        case wxBDIAGONAL_HATCH :
            style = HatchStyleBackwardDiagonal;
            break ;
        case wxCROSSDIAG_HATCH :
            style = HatchStyleDiagonalCross;
            break ;
        case wxFDIAGONAL_HATCH :
            style = HatchStyleForwardDiagonal;
            break ;
        case wxCROSS_HATCH :
            style = HatchStyleCross;
            break ;
        case wxHORIZONTAL_HATCH :
            style = HatchStyleHorizontal;
            break ;
        case wxVERTICAL_HATCH :
            style = HatchStyleVertical;
            break ;

        }
        m_brush = new HatchBrush(style,Color( brush.GetColour().Alpha() , brush.GetColour().Red() ,
            brush.GetColour().Green() , brush.GetColour().Blue() ), Color::Transparent );
    }
    else 
    {
        wxBitmap* bmp = brush.GetStipple();
        if ( bmp && bmp->Ok() )
        {
            wxDELETE( m_brushImage );
            m_brushImage = Bitmap::FromHBITMAP((HBITMAP)bmp->GetHBITMAP(),(HPALETTE)bmp->GetPalette()->GetHPALETTE());
            m_brush = new TextureBrush(m_brushImage);
        }
    }
}

void wxGDIPlusContext::SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, const wxColour&c1, const wxColour&c2) 
{
    m_brushTransparent = false ;

    wxDELETE(m_brush);

    m_brush = new LinearGradientBrush( PointF( x1,y1) , PointF( x2,y2),
        Color( c1.Alpha(), c1.Red(),c1.Green() , c1.Blue() ),
        Color( c2.Alpha(), c2.Red(),c2.Green() , c2.Blue() ));
}

void wxGDIPlusContext::SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
                                    const wxColour &oColor, const wxColour &cColor)
{
    m_brushTransparent = false ;

    wxDELETE(m_brush);
    wxDELETE(m_brushPath);

    // Create a path that consists of a single circle.
    m_brushPath = new GraphicsPath();
    m_brushPath->AddEllipse( (REAL)(xc-radius), (REAL)(yc-radius), (REAL)(2*radius), (REAL)(2*radius));

    PathGradientBrush *b = new PathGradientBrush(m_brushPath);
    m_brush = b;
    b->SetCenterPoint( PointF(xo,yo));
    b->SetCenterColor(Color( oColor.Alpha(), oColor.Red(),oColor.Green() , oColor.Blue() ));

    Color colors[] = {Color( cColor.Alpha(), cColor.Red(),cColor.Green() , cColor.Blue() )};
    int count = 1;
    b->SetSurroundColors(colors, &count);
}

// the built-in conversions functions create non-premultiplied bitmaps, while GDIPlus needs them in the 
// premultiplied format, therefore in the failing cases we create a new bitmap using the non-premultiplied 
// bytes as parameter

void wxGDIPlusContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
    Bitmap* image = NULL;
    Bitmap* helper = NULL;
    if ( bmp.GetMask() )
    { 
        Bitmap interim((HBITMAP)bmp.GetHBITMAP(),(HPALETTE)bmp.GetPalette()->GetHPALETTE()) ;

        size_t width = interim.GetWidth();
        size_t height = interim.GetHeight();
        Rect bounds(0,0,width,height);

        image = new Bitmap(width,height,PixelFormat32bppPARGB) ;

        Bitmap interimMask((HBITMAP)bmp.GetMask()->GetMaskBitmap(),NULL);
        wxASSERT(interimMask.GetPixelFormat() == PixelFormat1bppIndexed);

        BitmapData dataMask ;
        interimMask.LockBits(&bounds,ImageLockModeRead, 
            interimMask.GetPixelFormat(),&dataMask);


        BitmapData imageData ;
        image->LockBits(&bounds,ImageLockModeWrite, PixelFormat32bppPARGB, &imageData);

        BYTE maskPattern = 0 ;
        BYTE maskByte = 0;
        size_t maskIndex ;

        for ( size_t y = 0 ; y < height ; ++y)
        {
            maskIndex = 0 ;
            for( size_t x = 0 ; x < width; ++x)
            {
                if ( x % 8 == 0)
                {
                    maskPattern = 0x80;
                    maskByte = *((BYTE*)dataMask.Scan0 + dataMask.Stride*y + maskIndex);
                    maskIndex++;
                }
                else
                    maskPattern = maskPattern >> 1;

                ARGB *dest = (ARGB*)((BYTE*)imageData.Scan0 + imageData.Stride*y + x*4);
                if ( (maskByte & maskPattern) == 0 )
                    *dest = 0x00000000;
                else
                {
                    Color c ;
                    interim.GetPixel(x,y,&c) ;
                    *dest = (c.GetValue() | Color::AlphaMask);
                }
            }
        }

        image->UnlockBits(&imageData);

        interimMask.UnlockBits(&dataMask);
        interim.UnlockBits(&dataMask);
    }
    else
    {
        image = Bitmap::FromHBITMAP((HBITMAP)bmp.GetHBITMAP(),(HPALETTE)bmp.GetPalette()->GetHPALETTE());        
        if ( GetPixelFormatSize(image->GetPixelFormat()) == 32 )
        {
            size_t width = image->GetWidth();
            size_t height = image->GetHeight();
            Rect bounds(0,0,width,height);
            BitmapData data ;

            helper = image ;
            image = NULL ;
            helper->LockBits(&bounds, ImageLockModeRead,
                helper->GetPixelFormat(),&data);

            image = new Bitmap(data.Width, data.Height, data.Stride, 
                PixelFormat32bppARGB , (BYTE*) data.Scan0);

            helper->UnlockBits(&data);
        }
    }
    if ( image )
        m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;
    delete image ;
    delete helper ;
}

void wxGDIPlusContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
    HICON hIcon = (HICON)icon.GetHICON();
    ICONINFO iconInfo ;
    // IconInfo creates the bitmaps for color and mask, we must dispose of them after use
    if (!GetIconInfo(hIcon,&iconInfo))
        return;

    BITMAP iconBmpData ;
    GetObject(iconInfo.hbmColor,sizeof(BITMAP),&iconBmpData);
    Bitmap interim(iconInfo.hbmColor,NULL);

    Bitmap* image = NULL ;

    if( GetPixelFormatSize(interim.GetPixelFormat())!= 32 )
    {
        image = Bitmap::FromHICON(hIcon);
    }
    else
    {
        size_t width = interim.GetWidth();
        size_t height = interim.GetHeight();
        Rect bounds(0,0,width,height);
        BitmapData data ;

        interim.LockBits(&bounds, ImageLockModeRead,
            interim.GetPixelFormat(),&data);
        image = new Bitmap(data.Width, data.Height, data.Stride, 
            PixelFormat32bppARGB , (BYTE*) data.Scan0);
        interim.UnlockBits(&data);
    }

    m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;

    delete image ;
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
}


void wxGDIPlusContext::DrawText( const wxString &str, wxDouble x, wxDouble y ) 
{
    if ( str.IsEmpty())
        return ;

    wxWCharBuffer s = str.wc_str( *wxConvUI );
    m_context->DrawString( s , -1 , m_font , PointF( x , y ) , m_textBrush );
    // TODO m_backgroundMode == wxSOLID
}

void wxGDIPlusContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                     wxDouble *descent, wxDouble *externalLeading ) const
{
    wxWCharBuffer s = str.wc_str( *wxConvUI );
    FontFamily ffamily ;
    
    m_font->GetFamily(&ffamily) ;

    REAL factorY = m_context->GetDpiY() / 72.0 ;

    REAL rDescent = ffamily.GetCellDescent(FontStyleRegular) *
        m_font->GetSize() / ffamily.GetEmHeight(FontStyleRegular);
    REAL rAscent = ffamily.GetCellAscent(FontStyleRegular) *
        m_font->GetSize() / ffamily.GetEmHeight(FontStyleRegular);
    REAL rHeight = ffamily.GetLineSpacing(FontStyleRegular) *
        m_font->GetSize() / ffamily.GetEmHeight(FontStyleRegular);

    if ( height )
        *height = rHeight * factorY + 0.5 ;
    if ( descent )
        *descent = rDescent * factorY + 0.5 ;
    if ( externalLeading )
        *externalLeading = (rHeight - rAscent - rDescent) * factorY + 0.5 ;
    // measuring empty strings is not guaranteed, so do it by hand
    if ( str.IsEmpty()) 
    {
        if ( width )
            *width = 0 ;
    }
    else
    {
        // MeasureString does return a rectangle that is way too large, so it is
        // not usable here
        RectF layoutRect(0,0, 100000.0f, 100000.0f);
        StringFormat strFormat;
        CharacterRange strRange(0,wcslen(s));
        strFormat.SetMeasurableCharacterRanges(1,&strRange);
        Region region ;
        m_context->MeasureCharacterRanges(s, -1 , m_font,layoutRect, &strFormat,1,&region) ;
        RectF bbox ;
        region.GetBounds(&bbox,m_context);
        if ( width )
            *width = bbox.GetRight()-bbox.GetLeft()+0.5;
    }
}

void wxGDIPlusContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const 
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return;

    wxWCharBuffer ws = text.wc_str( *wxConvUI );
    size_t len = wcslen( ws ) ;
    wxASSERT_MSG(text.length() == len , wxT("GetPartialTextExtents not yet implemented for multichar situations"));

    RectF layoutRect(0,0, 100000.0f, 100000.0f);
    StringFormat strFormat;

    CharacterRange* ranges = new CharacterRange[len] ;
    Region* regions = new Region[len];
    for( size_t i = 0 ; i < len ; ++i)
    {
        ranges[i].First = i ;
        ranges[i].Length = 1 ;
    }
    strFormat.SetMeasurableCharacterRanges(len,ranges);
    m_context->MeasureCharacterRanges(ws, -1 , m_font,layoutRect, &strFormat,1,regions) ;

    RectF bbox ;
    for ( size_t i = 0 ; i < len ; ++i)
    {
        regions[i].GetBounds(&bbox,m_context);
        widths[i] = bbox.GetRight()-bbox.GetLeft();
    }
}

void wxGDIPlusContext::SetFont( const wxFont &font ) 
{
    wxASSERT( font.Ok());
    delete m_font;
    wxWCharBuffer s = font.GetFaceName().wc_str( *wxConvUI );
    int size = font.GetPointSize();
    int style = FontStyleRegular;
    if ( font.GetStyle() == wxFONTSTYLE_ITALIC )
        style |= FontStyleItalic;
    if ( font.GetUnderlined() )
        style |= FontStyleUnderline;
    if ( font.GetWeight() == wxFONTWEIGHT_BOLD )
        style |= FontStyleBold;
    m_font = new Font( s , size , style );
}

void* wxGDIPlusContext::GetNativeContext() 
{
	return m_context;
}

wxGraphicsContext* wxGraphicsContext::Create( const wxWindowDC& dc)
{
    return new wxGDIPlusContext( (HDC) dc.GetHDC() );
}

wxGraphicsContext* wxGraphicsContext::Create( wxWindow * window )
{
    return new wxGDIPlusContext( (HWND) window->GetHWND() );
}

wxGraphicsContext* wxGraphicsContext::CreateFromNative( void * context )
{
    return new wxGDIPlusContext( (Graphics*) context );
}



#endif  // wxUSE_GRAPHICS_CONTEXT
