/////////////////////////////////////////////////////////////////////////////
// Name:        wx/graphics.h
// Purpose:     graphics context header
// Author:      Stefan Csomor
// Modified by:
// Created:
// Copyright:   (c) Stefan Csomor
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GRAPHICS_H_
#define _WX_GRAPHICS_H_

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/geometry.h"
#include "wx/dynarray.h"

class WXDLLEXPORT wxWindowDC;

/*
 * notes about the graphics context apis
 *
 * angles : are measured in radians, 0.0 being in direction of positiv x axis, PI/2 being 
 * in direction of positive y axis.
 */
 
class WXDLLEXPORT wxGraphicsPath
{
public :
    wxGraphicsPath() {}
    virtual ~wxGraphicsPath() {}
    
    //
    // These are the path primitives from which everything else can be constructed
    //

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y ) = 0;

    // adds a straight line from the current point to (x,y) 
    virtual void AddLineToPoint( wxDouble x, wxDouble y ) = 0;

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y ) = 0;

    // closes the current sub-path
    virtual void CloseSubpath() = 0;

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble& x, wxDouble&y) = 0;

    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise ) = 0;

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
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r ) ;

    // wrappers using wxPoint2DDoubles

    wxPoint2DDouble GetCurrentPoint();

    void MoveToPoint( const wxPoint2DDouble& p);

    void AddLineToPoint( const wxPoint2DDouble& p);

    void AddCurveToPoint( const wxPoint2DDouble& c1, const wxPoint2DDouble& c2, const wxPoint2DDouble& e);

    void AddArc( const wxPoint2DDouble& c, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise);
	
    DECLARE_NO_COPY_CLASS(wxGraphicsPath)
};

/*
class WXDLLEXPORT wxGraphicsMatrix
{
public :
	wxGraphicsMatrix() {}
	
	virtual ~wxGraphicsMatrix() {}
	
	wxGraphicsMatrix* Concat( const wxGraphicsMatrix *t ) const;
	
	// returns the inverse matrix
	wxGraphicsMatrix* Invert() const;
	
	// returns true if the elements of the transformation matrix are equal ?
    bool operator==(const wxGraphicsMatrix& t) const;
	
	// return true if this is the identity matrix
	bool IsIdentity();
	
    //
    // transformation
    //
    
    // translate
    virtual void Translate( wxDouble dx , wxDouble dy ) = 0;

    // scale
    virtual void Scale( wxDouble xScale , wxDouble yScale ) = 0;

    // rotate (radians)
    virtual void Rotate( wxDouble angle ) = 0;	
} ;
*/

class WXDLLEXPORT wxGraphicsContext
{
public:
    wxGraphicsContext() {}

    virtual ~wxGraphicsContext() {}
    
    static wxGraphicsContext* Create( const wxWindowDC& dc) ;
	
    static wxGraphicsContext* CreateFromNative( void * context ) ;

#ifdef __WXMAC__
    static wxGraphicsContext* CreateFromNativeWindow( void * window ) ;
#endif
    static wxGraphicsContext* Create( wxWindow* window ) ;

    // creates a path instance that corresponds to the type of graphics context, ie GDIPlus, cairo, CoreGraphics ...
    virtual wxGraphicsPath * CreatePath() = 0;
	
	/*
	// create a 'native' matrix corresponding to these values
	virtual wxGraphicsMatrix* CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0) = 0;
	*/
	
    // push the current state of the context, ie the transformation matrix on a stack
    virtual void PushState() = 0;

    // pops a stored state from the stack
    virtual void PopState() = 0;

    // clips drawings to the region, combined to current clipping region
    virtual void Clip( const wxRegion &region ) = 0;

    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) = 0;
	
	// resets the clipping to original extent
	virtual void ResetClip() = 0 ;

	// returns the native context
	virtual void * GetNativeContext() = 0;

    //
    // transformation : changes the current transformation matrix CTM of the context
    //
    
    // translate
    virtual void Translate( wxDouble dx , wxDouble dy ) = 0;

    // scale
    virtual void Scale( wxDouble xScale , wxDouble yScale ) = 0;

    // rotate (radians)
    virtual void Rotate( wxDouble angle ) = 0;

    //
    // setting the paint
    //
    
    // sets the pan
    virtual void SetPen( const wxPen &pen ) = 0;

    // sets the brush for filling
    virtual void SetBrush( const wxBrush &brush ) = 0;

    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual void SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2) = 0;

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
    // with radius r and color cColor
    virtual void SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor) = 0;

    // sets the font
    virtual void SetFont( const wxFont &font ) = 0;
    
    // sets the text color
    virtual void SetTextColor( const wxColour &col ) = 0;

    // strokes along a path with the current pen
    virtual void StrokePath( const wxGraphicsPath *path ) = 0;

    // fills a path with the current brush
    virtual void FillPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE ) = 0;

    // draws a path by first filling and then stroking
    virtual void DrawPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE );
	
    //
    // text
    //
    
    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y ) = 0;

    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle );

    virtual void GetTextExtent( const wxString &text, wxDouble *width, wxDouble *height,
        wxDouble *descent, wxDouble *externalLeading ) const  = 0;

    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const = 0;

    //
    // image support
    //

    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) = 0;

    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) = 0;

    //
    // convenience methods
    //
    
    // strokes a single line
    virtual void StrokeLine( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2);

    // stroke lines connecting each of the points
    virtual void StrokeLines( size_t n, const wxPoint2DDouble *points);

    // stroke disconnected lines from begin to end points
    virtual void StrokeLines( size_t n, const wxPoint2DDouble *beginPoints, const wxPoint2DDouble *endPoints);

    // draws a polygon
    virtual void DrawLines( size_t n, const wxPoint2DDouble *points, int fillStyle = wxWINDING_RULE );

    // draws a polygon
    virtual void DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    // draws an ellipse
    virtual void DrawEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    // draws a rounded rectangle
    virtual void DrawRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius);

     // wrappers using wxPoint2DDouble TODO

    DECLARE_NO_COPY_CLASS(wxGraphicsContext)
};

#endif

#endif // _WX_GRAPHICS_H_
