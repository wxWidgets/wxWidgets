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

class WXDLLIMPEXP_CORE wxWindowDC;
class WXDLLIMPEXP_CORE wxGraphicsContext;
class WXDLLIMPEXP_CORE wxGraphicsPath;
class WXDLLIMPEXP_CORE wxGraphicsMatrix;
class WXDLLIMPEXP_CORE wxGraphicsFigure;
class WXDLLIMPEXP_CORE wxGraphicsRenderer;
class WXDLLIMPEXP_CORE wxGraphicsPen;
class WXDLLIMPEXP_CORE wxGraphicsBrush;
class WXDLLIMPEXP_CORE wxGraphicsFont;

/*
 * notes about the graphics context apis
 *
 * angles : are measured in radians, 0.0 being in direction of positiv x axis, PI/2 being 
 * in direction of positive y axis.
 */
 
// Base class of all objects used for drawing in the new graphics API, the always point back to their
// originating rendering engine, there is no dynamic unloading of a renderer currently allowed, 
// these references are not counted
 
class WXDLLIMPEXP_CORE wxGraphicsObject : public wxObject
{
public :
	wxGraphicsObject( wxGraphicsRenderer* renderer = NULL ) : m_renderer(renderer) {}
	
	wxGraphicsObject( const wxGraphicsObject& obj ) : m_renderer(obj.GetRenderer()) {}
	
	virtual ~wxGraphicsObject() {}
	
	wxGraphicsRenderer* GetRenderer() const { return m_renderer ; }
protected :
	wxGraphicsRenderer* m_renderer;
	DECLARE_DYNAMIC_CLASS(wxGraphicsObject);
} ;

class WXDLLIMPEXP_CORE wxGraphicsPen : public wxGraphicsObject
{
public :
    wxGraphicsPen(wxGraphicsRenderer* renderer) : wxGraphicsObject(renderer) {}
    virtual ~wxGraphicsPen() {}
    virtual void Apply( wxGraphicsContext* context) = 0;
    virtual wxDouble GetWidth() = 0;
private :
    DECLARE_NO_COPY_CLASS(wxGraphicsPen)
    DECLARE_ABSTRACT_CLASS(wxGraphicsPen)
} ;

class WXDLLIMPEXP_CORE wxGraphicsBrush : public wxGraphicsObject
{
public :
    wxGraphicsBrush(wxGraphicsRenderer* renderer) : wxGraphicsObject(renderer) {}
    virtual ~wxGraphicsBrush() {}
    virtual void Apply( wxGraphicsContext* context) = 0;
private :
    DECLARE_NO_COPY_CLASS(wxGraphicsBrush)
    DECLARE_ABSTRACT_CLASS(wxGraphicsBrush)
} ;

class WXDLLIMPEXP_CORE wxGraphicsFont : public wxGraphicsObject
{
public :
    wxGraphicsFont(wxGraphicsRenderer* renderer) : wxGraphicsObject(renderer) {}
    virtual ~wxGraphicsFont() {}
    virtual void Apply( wxGraphicsContext* context) = 0;
private :
    DECLARE_NO_COPY_CLASS(wxGraphicsFont)
    DECLARE_ABSTRACT_CLASS(wxGraphicsFont)
} ;

class WXDLLIMPEXP_CORE wxGraphicsPath : public wxGraphicsObject
{
public :
    wxGraphicsPath(wxGraphicsRenderer* renderer) : wxGraphicsObject(renderer) {}
    virtual ~wxGraphicsPath() {}
    
    virtual wxGraphicsPath *Clone() const = 0;

    //
    // These are the path primitives from which everything else can be constructed
    //

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y ) = 0;
    void MoveToPoint( const wxPoint2DDouble& p);

    // adds a straight line from the current point to (x,y) 
    virtual void AddLineToPoint( wxDouble x, wxDouble y ) = 0;
    void AddLineToPoint( const wxPoint2DDouble& p);

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y ) = 0;
    void AddCurveToPoint( const wxPoint2DDouble& c1, const wxPoint2DDouble& c2, const wxPoint2DDouble& e);
	
	// adds another path
	virtual void AddPath( const wxGraphicsPath* path ) =0;

    // closes the current sub-path
    virtual void CloseSubpath() = 0;

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble& x, wxDouble&y) = 0;
    wxPoint2DDouble GetCurrentPoint();

    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise ) = 0;
	void AddArc( const wxPoint2DDouble& c, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise);

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
	
	// returns the native path
	virtual void * GetNativePath() const = 0;
	
	// give the native path returned by GetNativePath() back (there might be some deallocations necessary)
	virtual void UnGetNativePath(void *p) = 0;
	
	// transforms each point of this path by the matrix
	virtual void Transform( wxGraphicsMatrix* matrix ) =0;
	
	// gets the bounding box enclosing all points (possibly including control points)
	virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *y) =0;
	wxRect2DDouble GetBox();
	
	virtual bool Contains( wxDouble x, wxDouble y, int fillStyle = wxWINDING_RULE) =0;
	bool Contains( const wxPoint2DDouble& c, int fillStyle = wxWINDING_RULE);
	
    DECLARE_NO_COPY_CLASS(wxGraphicsPath)
    DECLARE_ABSTRACT_CLASS(wxGraphicsPath)
};

class WXDLLIMPEXP_CORE wxGraphicsMatrix : public wxGraphicsObject
{
public :
	wxGraphicsMatrix(wxGraphicsRenderer* renderer) : wxGraphicsObject(renderer) {}
	
	virtual ~wxGraphicsMatrix() {}
	
    virtual wxGraphicsMatrix *Clone() const = 0;

	// concatenates the matrix
	virtual void Concat( const wxGraphicsMatrix *t ) = 0;
	void Concat( const wxGraphicsMatrix &t ) { Concat( &t ); }
	
	// copies the passed in matrix
	virtual void Copy( const wxGraphicsMatrix *t )  = 0;
	void Copy( const wxGraphicsMatrix &t ) { Copy( &t ); }

	// sets the matrix to the respective values
	virtual void Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0) = 0;

	// makes this the inverse matrix
	virtual void Invert() = 0;
	
	// returns true if the elements of the transformation matrix are equal ?
    virtual bool IsEqual( const wxGraphicsMatrix* t) const  = 0;
	bool IsEqual( const wxGraphicsMatrix& t) const { return IsEqual( &t ); }
	
	// return true if this is the identity matrix
	virtual bool IsIdentity()  = 0;
	
    //
    // transformation
    //
    
    // add the translation to this matrix
    virtual void Translate( wxDouble dx , wxDouble dy ) = 0;

    // add the scale to this matrix
    virtual void Scale( wxDouble xScale , wxDouble yScale ) = 0;

    // add the rotation to this matrix (radians)
    virtual void Rotate( wxDouble angle ) = 0;	
	
    //
    // apply the transforms
    //
	
	// applies that matrix to the point
	virtual void TransformPoint( wxDouble *x, wxDouble *y ) = 0;
	
	// applies the matrix except for translations
	virtual void TransformDistance( wxDouble *dx, wxDouble *dy ) =0;
	
	// returns the native representation
	virtual void * GetNativeMatrix() const = 0;
	
    DECLARE_NO_COPY_CLASS(wxGraphicsMatrix)
    DECLARE_ABSTRACT_CLASS(wxGraphicsMatrix)
} ;

class WXDLLIMPEXP_CORE wxGraphicsContext : public wxGraphicsObject
{
public:
    wxGraphicsContext(wxGraphicsRenderer* renderer);

    virtual ~wxGraphicsContext();
    
    static wxGraphicsContext* Create( const wxWindowDC& dc) ;
	
    static wxGraphicsContext* CreateFromNative( void * context ) ;

#ifdef __WXMAC__
    static wxGraphicsContext* CreateFromNativeWindow( void * window ) ;
#endif
    static wxGraphicsContext* Create( wxWindow* window ) ;

    wxGraphicsPath * CreatePath();
	
    virtual wxGraphicsPen* CreatePen(const wxPen& pen);
    
    virtual wxGraphicsBrush* CreateBrush(const wxBrush& brush );
    
    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual wxGraphicsBrush* CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2);

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
    // with radius r and color cColor
    virtual wxGraphicsBrush* CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor);

    // sets the font
    virtual wxGraphicsFont* CreateFont( const wxFont &font , const wxColour &col = *wxBLACK );

	// create a 'native' matrix corresponding to these values
	virtual wxGraphicsMatrix* CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0);
	
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
	
	// concatenates this transform with the current transform of this context
	virtual void ConcatTransform( const wxGraphicsMatrix* matrix ) = 0;

	// sets the transform of this context
	virtual void SetTransform( const wxGraphicsMatrix* matrix ) = 0;

	// gets the matrix of this context
	virtual void GetTransform( wxGraphicsMatrix* matrix ) = 0;
    //
    // setting the paint
    //
    
    // sets the pen
    virtual void SetPen( wxGraphicsPen* pen , bool release = true );
    
    void SetPen( const wxPen& pen );

    // sets the brush for filling
    virtual void SetBrush( wxGraphicsBrush* brush , bool release = true );
    
    void SetBrush( const wxBrush& brush );

    // sets the font
    virtual void SetFont( wxGraphicsFont* font, bool release = true );
    
    void SetFont( const wxFont& font, const wxColour& colour );
    
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

protected :
    // helper to determine if a 0.5 offset should be applied for the drawing operation
    virtual bool ShouldOffset() const { return false; }

    wxGraphicsPen* m_pen;
    bool m_releasePen;
    wxGraphicsBrush* m_brush;
    bool m_releaseBrush;
    wxGraphicsFont* m_font;
    bool m_releaseFont;

private :
    DECLARE_NO_COPY_CLASS(wxGraphicsContext)
    DECLARE_ABSTRACT_CLASS(wxGraphicsContext)
};

#if 0 

//
// A graphics figure allows to cache path, pen etc creations, also will be a basis for layering/grouping elements
//

class WXDLLIMPEXP_CORE wxGraphicsFigure : public wxGraphicsObject
{
public :
	wxGraphicsFigure(wxGraphicsRenderer* renderer) ;
		
	virtual ~wxGraphicsFigure() ;
	
	void SetPath( wxGraphicsMatrix* matrix );
	
	void SetMatrix( wxGraphicsPath* path);

	// draws this object on the context
	virtual void Draw( wxGraphicsContext* cg );
	
	// returns the path of this object
	wxGraphicsPath* GetPath() { return m_path; }
	
	// returns the transformation matrix of this object, may be null if there is no transformation necessary
	wxGraphicsMatrix* GetMatrix() { return m_matrix; }
	
private :
	wxGraphicsMatrix* m_matrix;
	wxGraphicsPath* m_path;
    
    DECLARE_DYNAMIC_CLASS(wxGraphicsFigure)
} ;
  
#endif  

//
// The graphics renderer is the instance corresponding to the rendering engine used, eg there is ONE core graphics renderer
// instance on OSX. This instance is pointed back to by all objects created by it. Therefore you can create eg additional
// paths at any point from a given matrix etc.
//

class WXDLLIMPEXP_CORE wxGraphicsRenderer : public wxObject
{
public :
    wxGraphicsRenderer() {}

    virtual ~wxGraphicsRenderer() {}

	static wxGraphicsRenderer* GetDefaultRenderer();

	// Context

	virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) = 0 ;
	
	virtual wxGraphicsContext * CreateContextFromNativeContext( void * context ) = 0;

	virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window ) = 0;

	virtual wxGraphicsContext * CreateContext( wxWindow* window ) = 0;

	// Path
	
    virtual wxGraphicsPath * CreatePath() = 0;

	// Matrix
	
	virtual wxGraphicsMatrix * CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0) = 0;
        
    // Paints
    
    virtual wxGraphicsPen* CreatePen(const wxPen& pen) = 0 ;
    
    virtual wxGraphicsBrush* CreateBrush(const wxBrush& brush ) = 0 ;
    
    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual wxGraphicsBrush* CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2) = 0;

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
    // with radius r and color cColor
    virtual wxGraphicsBrush* CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor) = 0;

   // sets the font
    virtual wxGraphicsFont* CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ) = 0;
    
private :
	DECLARE_NO_COPY_CLASS(wxGraphicsRenderer)
    DECLARE_ABSTRACT_CLASS(wxGraphicsRenderer)
} ;

#endif

#endif // _WX_GRAPHICS_H_
