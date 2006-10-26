/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/graphicc.cpp
// Purpose:     cairo device context class
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-03
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

static inline double dmin(double a, double b)
{
    return a < b ? a : b;
}
static inline double dmax(double a, double b)
{
    return a > b ? a : b;
}

static inline double DegToRad(double deg)
{
    return (deg * M_PI) / 180.0;
}
static inline double RadToDeg(double deg)
{
    return (deg * 180.0) / M_PI;
}

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxCairoContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxGraphicsPath implementation
//-----------------------------------------------------------------------------

// TODO remove this dependency (gdiplus needs the macros)

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <cairo.h>
#include <gtk/gtk.h>

class WXDLLEXPORT wxCairoPath : public wxGraphicsPath
{
    DECLARE_NO_COPY_CLASS(wxCairoPath)
public :
    wxCairoPath();
    ~wxCairoPath();


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

    /*

    // appends a rectangle as a new closed subpath 
    virtual void AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) ;
    // appends an ellipsis as a new closed subpath fitting the passed rectangle
    virtual void AddEllipsis( wxDouble x, wxDouble y, wxDouble w , wxDouble h ) ;

    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )  ;
    */

    // returns the native path
    virtual void * GetNativePath() const ;

    // give the native path returned by GetNativePath() back (there might be some deallocations necessary)
    virtual void UnGetNativePath(void *p) ;

private :
    cairo_t* m_pathContext;
};

wxCairoPath::wxCairoPath()
{
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,1,1);
    m_pathContext = cairo_create(surface);
    cairo_surface_destroy (surface);
}

wxCairoPath::~wxCairoPath()
{
    cairo_destroy(m_pathContext);
}

void* wxCairoPath::GetNativePath() const
{
    return cairo_copy_path(m_pathContext) ;
}

void wxCairoPath::UnGetNativePath(void *p)
{
    cairo_path_destroy((cairo_path_t*)p);
}

//
// The Primitives
//

void wxCairoPath::MoveToPoint( wxDouble x , wxDouble y )
{
    cairo_move_to(m_pathContext,x,y);
}

void wxCairoPath::AddLineToPoint( wxDouble x , wxDouble y )
{
    cairo_line_to(m_pathContext,x,y);
}

void wxCairoPath::CloseSubpath()
{
    cairo_close_path(m_pathContext);
}

void wxCairoPath::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    cairo_curve_to(m_pathContext,cx1,cy1,cx2,cy2,x,y);
}

// gets the last point of the current path, (0,0) if not yet set
void wxCairoPath::GetCurrentPoint( wxDouble& x, wxDouble&y)
{
    double dx,dy;
    cairo_get_current_point(m_pathContext,&dx,&dy);
    x = dx;
    y = dy;
}

void wxCairoPath::AddArc( wxDouble x, wxDouble y, wxDouble r, double startAngle, double endAngle, bool clockwise )
{
    // as clockwise means positive in our system (y pointing downwards) 
    // TODO make this interpretation dependent of the
    // real device trans
    if ( clockwise||(endAngle-startAngle)>=2*M_PI)
        cairo_arc(m_pathContext,x,y,r,startAngle,endAngle);
    else
        cairo_arc_negative(m_pathContext,x,y,r,startAngle,endAngle);
}

/*
void wxCairoPath::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    m_path->AddRectangle(RectF(x,y,w,h));
}
*/
//
//
//
/*
// closes the current subpath
void wxCairoPath::AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r ) 
{
//    CGPathAddArcToPoint( m_path, NULL , x1, y1, x2, y2, r); 
}
 
*/

class WXDLLEXPORT wxCairoContext : public wxGraphicsContext
{
    DECLARE_NO_COPY_CLASS(wxCairoContext)

public:
    wxCairoContext( const wxWindowDC& dc );
    wxCairoContext();
    virtual ~wxCairoContext();

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
    virtual void SetTextColour( const wxColour &col );
    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y);
    virtual void GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                wxDouble *descent, wxDouble *externalLeading ) const;
    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const;

private:
    cairo_t* m_context;
    bool m_penTransparent;
    bool m_brushTransparent;

    wxPen m_pen;
    wxBrush m_brush;
    cairo_pattern_t* m_brushPattern;
    wxColour m_textColour;
};




//-----------------------------------------------------------------------------
// wxCairoContext implementation
//-----------------------------------------------------------------------------

wxCairoContext::wxCairoContext( const wxWindowDC& dc  )
{
    m_context = gdk_cairo_create( dc.m_window ) ;
    PushState();
    PushState();
    m_penTransparent = true;
    m_brushTransparent = true;
    m_brushPattern = NULL ;
}

wxCairoContext::~wxCairoContext()
{
    if ( m_context )
    {
        PopState();
        PopState();
        cairo_destroy(m_context);
        if ( m_brushPattern )
            cairo_pattern_destroy(m_brushPattern);
    }
}


void wxCairoContext::Clip( const wxRegion & WXUNUSED(region) )
{
// TODO
}

void wxCairoContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
// TODO
}

void wxCairoContext::ResetClip()
{
// TODO
}


void wxCairoContext::StrokePath( const wxGraphicsPath *path )
{
    if ( m_penTransparent )
        return;

    cairo_path_t* cp = (cairo_path_t*) path->GetNativePath() ;
    cairo_append_path(m_context,cp);

    // setup pen

    // TODO: * m_dc->m_scaleX
    double penWidth = m_pen.GetWidth();
    if (penWidth <= 0.0)
        penWidth = 0.1;

    cairo_set_line_width(m_context,penWidth);
    cairo_set_source_rgba(m_context,m_pen.GetColour().Red()/255.0,
                          m_pen.GetColour().Green()/255.0, m_pen.GetColour().Blue()/255.0,m_pen.GetColour().Alpha()/255.0);

    cairo_line_cap_t cap;
    switch ( m_pen.GetCap() )
    {
    case wxCAP_ROUND :
        cap = CAIRO_LINE_CAP_ROUND;
        break;

    case wxCAP_PROJECTING :
        cap = CAIRO_LINE_CAP_SQUARE;
        break;

    case wxCAP_BUTT :
        cap = CAIRO_LINE_CAP_BUTT;
        break;

    default :
        cap = CAIRO_LINE_CAP_BUTT;
        break;
    }
    cairo_set_line_cap(m_context,cap);

    cairo_line_join_t join;
    switch ( m_pen.GetJoin() )
    {
    case wxJOIN_BEVEL :
        join = CAIRO_LINE_JOIN_BEVEL;
        break;

    case wxJOIN_MITER :
        join = CAIRO_LINE_JOIN_MITER;
        break;

    case wxJOIN_ROUND :
        join = CAIRO_LINE_JOIN_ROUND;
        break;

    default :
        join = CAIRO_LINE_JOIN_MITER;
        break;
    }
    cairo_set_line_join(m_context,join);

    int num_dashes = 0;
    const double * dashes = NULL;
    double offset = 0.0;

    const double dashUnit = penWidth < 1.0 ? 1.0 : penWidth;

    double *userLengths = NULL;
    const double dotted[] =
        {
            dashUnit , dashUnit + 2.0
        };
    const double short_dashed[] =
        {
            9.0 , 6.0
        };
    const double dashed[] =
        {
            19.0 , 9.0
        };
    const double dotted_dashed[] =
        {
            9.0 , 6.0 , 3.0 , 3.0
        };

    switch ( m_pen.GetStyle() )
    {
    case wxSOLID :
        break;

    case wxDOT :
        dashes = dotted ;
        num_dashes = WXSIZEOF(dotted)
                     ;
        break;

    case wxLONG_DASH :
        dashes = dotted ;
        num_dashes = WXSIZEOF(dashed);
        break;

    case wxSHORT_DASH :
        dashes = dotted ;
        num_dashes = WXSIZEOF(short_dashed);
        break;

    case wxDOT_DASH :
        dashes = dotted ;
        num_dashes = WXSIZEOF(dotted_dashed);
        break;

    case wxUSER_DASH :
        {
            wxDash *wxdashes ;
            num_dashes = m_pen.GetDashes( &wxdashes ) ;
            if ((wxdashes != NULL) && (num_dashes > 0))
            {
                userLengths = new double[num_dashes] ;
                for ( int i = 0 ; i < num_dashes ; ++i )
                {
                    userLengths[i] = wxdashes[i] * dashUnit ;

                    if ( i % 2 == 1 && userLengths[i] < dashUnit + 2.0 )
                        userLengths[i] = dashUnit + 2.0 ;
                    else if ( i % 2 == 0 && userLengths[i] < dashUnit )
                        userLengths[i] = dashUnit ;
                }
            }
            dashes = userLengths ;
        }
        break;
    case wxSTIPPLE :
        {
            /*
                        wxBitmap* bmp = pen.GetStipple();
                        if ( bmp && bmp->Ok() )
                        {
                            wxDELETE( m_penImage );
                            wxDELETE( m_penBrush );
                            m_penImage = Bitmap::FromHBITMAP((HBITMAP)bmp->GetHBITMAP(),(HPALETTE)bmp->GetPalette()->GetHPALETTE());
                            m_penBrush = new TextureBrush(m_penImage);
                            m_pen->SetBrush( m_penBrush );
                        }
            */
        }
        break;
    default :
        if ( m_pen.GetStyle() >= wxFIRST_HATCH && m_pen.GetStyle() <= wxLAST_HATCH )
        {
            /*
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
                            pen.GetColour().Green() , pen.GetColour().Blue() ), Color.Transparent );
                        m_pen->SetBrush( m_penBrush )
            */
        }
        break;
    }

    cairo_set_dash(m_context,(double*)dashes,num_dashes,offset);
    if ( userLengths )
        delete[] userLengths;
    cairo_stroke(m_context);
    wxConstCast(path, wxGraphicsPath)->UnGetNativePath(cp);
}

void wxCairoContext::FillPath( const wxGraphicsPath *path , int fillStyle )
{
    if ( !m_brushTransparent )
    {
        cairo_path_t* cp = (cairo_path_t*) path->GetNativePath() ;
        cairo_append_path(m_context,cp);

        if ( m_brushPattern )
        {
            cairo_set_source(m_context,m_brushPattern);
        }
        else
        {
            cairo_set_source_rgba(m_context,m_brush.GetColour().Red()/255.0,
                                  m_brush.GetColour().Green()/255.0,
                                  m_brush.GetColour().Blue()/255.0,
                                  m_brush.GetColour().Alpha()/255.0);
        }

        cairo_set_fill_rule(m_context,fillStyle==wxODDEVEN_RULE ? CAIRO_FILL_RULE_EVEN_ODD : CAIRO_FILL_RULE_WINDING);
        cairo_fill(m_context);
        wxConstCast(path, wxGraphicsPath)->UnGetNativePath(cp);
    }
}

wxGraphicsPath* wxCairoContext::CreatePath()
{
    return new wxCairoPath();
}

void wxCairoContext::Rotate( wxDouble angle )
{
    cairo_rotate(m_context,angle);
}

void wxCairoContext::Translate( wxDouble dx , wxDouble dy )
{
    cairo_translate(m_context,dx,dy);
}

void wxCairoContext::Scale( wxDouble xScale , wxDouble yScale )
{
    cairo_scale(m_context,xScale,yScale);
    /*
        PointF penWidth( m_pen->GetWidth(), 0);
        Matrix matrix ;
        if ( !m_penTransparent )
        {
                m_context->GetTransform(&matrix);
                matrix.TransformVectors(&penWidth);
        }
        m_context->ScaleTransform(xScale,yScale);
        if ( !m_penTransparent )
        {
            m_context->GetTransform(&matrix);
            matrix.Invert();
            matrix.TransformVectors(&penWidth) ;
            m_pen->SetWidth( sqrt( penWidth.X*penWidth.X  + penWidth.Y*penWidth.Y));
        }
    */
}

void wxCairoContext::PushState()
{
    cairo_save(m_context);
}

void wxCairoContext::PopState()
{
    cairo_restore(m_context);
}

void wxCairoContext::SetTextColour( const wxColour &col )
{
    m_textColour = col;
}

void wxCairoContext::SetPen( const wxPen &pen )
{
    m_pen = pen ;
    m_penTransparent = pen.GetStyle() == wxTRANSPARENT;
    if ( m_penTransparent )
        return;

    /*
     
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
                    pen.GetColour().Green() , pen.GetColour().Blue() ), Color.Transparent );
                m_pen->SetBrush( m_penBrush );
            }
            break;
        } 
        if ( dashStyle != DashStyleSolid )
            m_pen->SetDashStyle(dashStyle);
    */
}

void wxCairoContext::SetBrush( const wxBrush &brush )
{
    m_brush = brush;
    if (m_brushPattern)
    {
        cairo_pattern_destroy(m_brushPattern);
        m_brushPattern = NULL;
    }
    m_brushTransparent = brush.GetStyle() == wxTRANSPARENT;

    if ( m_brushTransparent )
        return;
    /*
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
                brush.GetColour().Green() , brush.GetColour().Blue() ), Color.Transparent );
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
    */
}

void wxCairoContext::SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, const wxColour&c1, const wxColour&c2)
{
    if ( m_brushPattern )
    {
        cairo_pattern_destroy(m_brushPattern);
        m_brushPattern=NULL;
    }
    
    m_brushTransparent = false;
    m_brushPattern = cairo_pattern_create_linear(x1,y1,x2,y2);
    cairo_pattern_add_color_stop_rgba(m_brushPattern,0.0,c1.Red()/255.0,
                          c1.Green()/255.0, c1.Blue()/255.0,c1.Alpha()/255.0);
    cairo_pattern_add_color_stop_rgba(m_brushPattern,1.0,c2.Red()/255.0,
                          c2.Green()/255.0, c2.Blue()/255.0,c2.Alpha()/255.0);
    wxASSERT_MSG(cairo_pattern_status(m_brushPattern) == CAIRO_STATUS_SUCCESS, wxT("Couldn't create cairo pattern"));
}

void wxCairoContext::SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor)
{
   if ( m_brushPattern )
    {
        cairo_pattern_destroy(m_brushPattern);
        m_brushPattern=NULL;
    }
    
    m_brushTransparent = false;
    m_brushPattern = cairo_pattern_create_radial(xo,yo,0.0,xc,yc,radius);
    cairo_pattern_add_color_stop_rgba(m_brushPattern,0.0,oColor.Red()/255.0,
                          oColor.Green()/255.0, oColor.Blue()/255.0,oColor.Alpha()/255.0);
    cairo_pattern_add_color_stop_rgba(m_brushPattern,1.0,cColor.Red()/255.0,
                          cColor.Green()/255.0, cColor.Blue()/255.0,cColor.Alpha()/255.0);
    wxASSERT_MSG(cairo_pattern_status(m_brushPattern) == CAIRO_STATUS_SUCCESS, wxT("Couldn't create cairo pattern"));
 }

void wxCairoContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    /*
        Bitmap* image = Bitmap::FromHBITMAP((HBITMAP)bmp.GetHBITMAP(),(HPALETTE)bmp.GetPalette()->GetHPALETTE());
        m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;
        delete image ;
    */
}

void wxCairoContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    /*
        Bitmap* image = Bitmap::FromHICON((HICON)icon.GetHICON());
        m_context->DrawImage(image,(REAL) x,(REAL) y,(REAL) w,(REAL) h) ;
        delete image ;
    */
}


void wxCairoContext::DrawText( const wxString &str, wxDouble x, wxDouble y )
{
    if ( str.IsEmpty())
        return ;
    cairo_move_to(m_context,x,y);
    const wxWX2MBbuf buf(str.mb_str(wxConvUTF8));

    cairo_set_source_rgba(m_context,m_textColour.Red()/255.0,
                          m_textColour.Green()/255.0, m_textColour.Blue()/255.0,m_textColour.Alpha()/255.0);
    cairo_show_text(m_context,buf);

    // TODO m_backgroundMode == wxSOLID
}

void wxCairoContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                                    wxDouble *descent, wxDouble *externalLeading ) const
{
    /*
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
    */
}

void wxCairoContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return;
    /*
        wxWCharBuffer ws = text.wc_str( *wxConvUI );
        size_t len = wcslen( ws ) ;
        wxASSERT_MSG(text.length() == len , wxT("GetPartialTextExtents not yet implemented for multichar situations"));
     
        RectF layoutRect(0,0, 100000.0f, 100000.0f);
        StringFormat strFormat;
     
        CharacterRange* ranges = new CharacterRange[len] ;
        Region* regions = new Region[len];
        for( int i = 0 ; i < len ; ++i)
        {
            ranges[i].First = i ;
            ranges[i].Length = 1 ;
        }
        strFormat.SetMeasurableCharacterRanges(len,ranges);
        m_context->MeasureCharacterRanges(ws, -1 , m_font,layoutRect, &strFormat,1,regions) ;
     
        RectF bbox ;
        for ( int i = 0 ; i < len ; ++i)
        {
            regions[i].GetBounds(&bbox,m_context);
            widths[i] = bbox.GetRight()-bbox.GetLeft();
        }
    */
}

void wxCairoContext::SetFont( const wxFont &font )
{
    cairo_select_font_face(m_context,font.GetFaceName().mb_str(wxConvUTF8),
                           font.GetStyle() == wxFONTSTYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC:CAIRO_FONT_SLANT_NORMAL,
                           font.GetWeight() == wxFONTWEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD:CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(m_context,font.GetPointSize());
    // TODO UNDERLINE
    // TODO FIX SIZE
}

void * wxCairoContext::GetNativeContext() 
{
    return m_context;
}

wxGraphicsContext* wxGraphicsContext::Create( const wxWindowDC& dc )
{
    return new wxCairoContext(dc);
}

wxGraphicsContext* wxGraphicsContext::Create( wxWindow * window )
{
    return NULL; // TODO
}

wxGraphicsContext* wxGraphicsContext::CreateFromNative( void * context )
{
    return NULL; // TODO
}

#endif  // wxUSE_GRAPHICS_CONTEXT
