/////////////////////////////////////////////////////////////////////////////
// Name:        _graphics.i
// Purpose:     Wrapper definitions for wx.GraphicsPath, wx.GraphicsContext
//
// Author:      Robin Dunn
//
// Created:     2-Oct-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/graphics.h>
%}

// Turn off the aquisition of the Global Interpreter Lock for the classes and
// functions in this file
%threadWrapperOff


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

%{
#if !wxUSE_GRAPHICS_CONTEXT
// C++ stub classes for platforms or build configurations that don't have
// wxGraphicsContext yet.

class wxGraphicsRenderer;
class wxGraphicsMatrix;


class wxGraphicsObject : public wxObject
{
public :
    wxGraphicsObject() {}
    wxGraphicsObject( wxGraphicsRenderer*  ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsObject is not available on this platform.");
    }
    wxGraphicsObject( const wxGraphicsObject&  ) {}
    virtual ~wxGraphicsObject() {}
    bool IsNull() const { return false; }
    wxGraphicsRenderer* GetRenderer() const { return NULL; }
} ;



class wxGraphicsPen : public wxGraphicsObject
{
public:
    wxGraphicsPen()  {}
    virtual ~wxGraphicsPen() {}
} ;
wxGraphicsPen wxNullGraphicsPen;



class wxGraphicsBrush : public wxGraphicsObject
{
public :
    wxGraphicsBrush() {}
    virtual ~wxGraphicsBrush() {}
} ;
wxGraphicsBrush wxNullGraphicsBrush;



class wxGraphicsFont : public wxGraphicsObject
{
public :
    wxGraphicsFont() {}
    virtual ~wxGraphicsFont() {}
} ;
wxGraphicsFont wxNullGraphicsFont;



class wxGraphicsPath : public wxGraphicsObject
{
public :
    wxGraphicsPath() { }
    wxGraphicsPath(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsPath is not available on this platform.");
    }
    virtual ~wxGraphicsPath() {}

    void MoveToPoint( wxDouble, wxDouble ) {}
    void MoveToPoint( const wxPoint2DDouble& ) {}
    void AddLineToPoint( wxDouble, wxDouble ) {}
    void AddLineToPoint( const wxPoint2DDouble& ) {}
    void AddCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddCurveToPoint( const wxPoint2DDouble&, const wxPoint2DDouble&, const wxPoint2DDouble&) {}
    void AddPath( const wxGraphicsPath& ) {}
    void CloseSubpath() {}
    void GetCurrentPoint( wxDouble&, wxDouble&) const {}
    wxPoint2DDouble GetCurrentPoint() const { return wxPoint2D(0,0); }
    void AddArc( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, bool ) {}
    void AddArc( const wxPoint2DDouble& , wxDouble, wxDouble , wxDouble , bool ) {}

    void AddQuadCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddRectangle( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddCircle( wxDouble, wxDouble, wxDouble ) {}
    void AddArcToPoint( wxDouble, wxDouble , wxDouble, wxDouble, wxDouble )  {}

    void AddEllipse( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void AddRoundedRectangle( wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void * GetNativePath() const { return NULL; }
    void UnGetNativePath(void *) const {}
    void Transform( const wxGraphicsMatrix& ) {}
    void GetBox(wxDouble *, wxDouble *, wxDouble *, wxDouble *) const {}
    wxRect2D GetBox() const { return wxRect2D(0,0,0,0); }

    bool Contains( wxDouble , wxDouble , int ) const { return false; }
    bool Contains( const wxPoint2DDouble& , int ) const { return false; }
};
wxGraphicsPath wxNullGraphicsPath;


class wxGraphicsMatrix : public wxGraphicsObject
{
public :
    wxGraphicsMatrix() { }
    wxGraphicsMatrix(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsMatrix is not available on this platform.");
    }
    virtual ~wxGraphicsMatrix() {}
    virtual void Concat( const wxGraphicsMatrix & ) {}
    virtual void Copy( const wxGraphicsMatrix & )  {}
    virtual void Set(wxDouble , wxDouble , wxDouble , wxDouble ,
                     wxDouble , wxDouble ) {}
    virtual void Get(wxDouble*, wxDouble*, wxDouble*,
                     wxDouble*, wxDouble*, wxDouble*) {}
    virtual void Invert() {}
    virtual bool IsEqual( const wxGraphicsMatrix& t) const  { return false; }
    virtual bool IsIdentity() const { return false; }
    virtual void Translate( wxDouble , wxDouble ) {}
    virtual void Scale( wxDouble , wxDouble  ) {}
    virtual void Rotate( wxDouble  ) {}
    virtual void TransformPoint( wxDouble *, wxDouble * ) const {}
    virtual void TransformDistance( wxDouble *, wxDouble * ) const {}
    virtual void * GetNativeMatrix() const { return NULL; }
};
wxGraphicsMatrix wxNullGraphicsMatrix;


class wxGraphicsContext : public wxGraphicsObject
{
public:

    wxGraphicsContext(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    virtual ~wxGraphicsContext() {}

    static wxGraphicsContext* Create()   {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
        return NULL;
    }
    static wxGraphicsContext* Create( const wxWindowDC& )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
        return NULL;
    }

    static wxGraphicsContext* CreateFromNative( void *  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
        return NULL;
    }

    static wxGraphicsContext* CreateFromNativeWindow( void *  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
        return NULL;
    }

    static wxGraphicsContext* Create( wxWindow*  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
        return NULL;
    }

    wxGraphicsPath CreatePath()  { return wxNullGraphicsPath; }

    virtual wxGraphicsPen CreatePen(const wxPen& )  { return wxNullGraphicsPen; }

    virtual wxGraphicsBrush CreateBrush(const wxBrush& ) { return wxNullGraphicsBrush; }

    virtual wxGraphicsBrush CreateLinearGradientBrush( wxDouble , wxDouble , wxDouble , wxDouble ,
                                                        const wxColour&, const wxColour&) { return wxNullGraphicsBrush; }

    virtual wxGraphicsBrush CreateRadialGradientBrush( wxDouble xo, wxDouble yo,
                                                        wxDouble xc, wxDouble yc, wxDouble radius,
                                                        const wxColour &oColor, const wxColour &cColor) { return wxNullGraphicsBrush; }

    virtual wxGraphicsFont CreateFont( const wxFont &, const wxColour & )  { return wxNullGraphicsFont; }

    virtual wxGraphicsMatrix CreateMatrix( wxDouble, wxDouble, wxDouble, wxDouble,
                                            wxDouble, wxDouble)  { return wxNullGraphicsMatrix; }

    virtual void PushState() {}
    virtual void PopState() {}
    virtual void Clip( const wxRegion & ) {}
    virtual void Clip( wxDouble , wxDouble , wxDouble , wxDouble  ) {}
    virtual void ResetClip() {}
    virtual void * GetNativeContext() { return NULL; }
    virtual int GetLogicalFunction() const { return 0; }
    virtual bool SetLogicalFunction(int ) {}    
    virtual void Translate( wxDouble , wxDouble ) {}
    virtual void Scale( wxDouble , wxDouble ) {}
    virtual void Rotate( wxDouble ) {}
    virtual void ConcatTransform( const wxGraphicsMatrix& ) {}
    virtual void SetTransform( const wxGraphicsMatrix& ) {}
    virtual wxGraphicsMatrix GetTransform() const { return wxNullGraphicsMatrix; }

    virtual void SetPen( const wxGraphicsPen& ) {}
    void SetPen( const wxPen& ) {}

    virtual void SetBrush( const wxGraphicsBrush& ) {}
    void SetBrush( const wxBrush& ) {}

    virtual void SetFont( const wxGraphicsFont& ) {}
    void SetFont( const wxFont&, const wxColour& ) {}

    virtual void StrokePath( const wxGraphicsPath & ) {}
    virtual void FillPath( const wxGraphicsPath &, int ) {}
    virtual void DrawPath( const wxGraphicsPath &, int ) {}

    virtual void DrawText( const wxString &, wxDouble , wxDouble  )  {}
    virtual void DrawText( const wxString &, wxDouble , wxDouble , wxDouble ) {}
    virtual void DrawText( const wxString &, wxDouble , wxDouble , wxGraphicsBrush )  {}
    virtual void DrawText( const wxString &, wxDouble , wxDouble , wxDouble , wxGraphicsBrush ) {}
    virtual void GetTextExtent( const wxString &, wxDouble *, wxDouble *,
                                wxDouble *, wxDouble * ) const {}
    virtual void GetPartialTextExtents(const wxString& , wxArrayDouble& ) const  {}

    virtual void DrawBitmap( const wxBitmap &, wxDouble , wxDouble , wxDouble , wxDouble  )  {}
    virtual void DrawIcon( const wxIcon &, wxDouble , wxDouble , wxDouble , wxDouble  )  {}

    virtual void StrokeLine( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    virtual void StrokeLines( size_t , const wxPoint2DDouble *) {}
    virtual void StrokeLines( size_t , const wxPoint2DDouble *, const wxPoint2DDouble *) {}
    virtual void DrawLines( size_t , const wxPoint2DDouble *, int ) {}
    virtual void DrawRectangle( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    virtual void DrawRoundedRectangle( wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ) {}
    virtual void DrawEllipse( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    virtual void DrawRoundedRectangle( wxDouble  wxDouble , wxDouble , wxDouble , wxDouble ) {}
    virtual bool ShouldOffset() const { return false; }
};


class wxGraphicsRenderer : public wxObject
{
public :
    wxGraphicsRenderer() {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsRenderer is not available on this platform.");
    }

    virtual ~wxGraphicsRenderer() {}

    static wxGraphicsRenderer* GetDefaultRenderer() {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsRenderer is not available on this platform.");
        return NULL;
    }

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& ) { return NULL; }
    virtual wxGraphicsContext * CreateContextFromNativeContext( void *  ) { return NULL; }
    virtual wxGraphicsContext * CreateContextFromNativeWindow( void *  )  { return NULL; }
    virtual wxGraphicsContext * CreateContext( wxWindow*  ) { return NULL; }
    virtual wxGraphicsContext * CreateMeasuringContext() { return NULL; }

    virtual wxGraphicsPath CreatePath()  { return wxNullGraphicsPath; }

    virtual wxGraphicsMatrix CreateMatrix( wxDouble , wxDouble , wxDouble , wxDouble ,
                                             wxDouble , wxDouble ) { return wxNullGraphicsMatrix; }

    virtual wxGraphicsPen CreatePen(const wxPen& )  { return wxNullGraphicsPen; }
    virtual wxGraphicsBrush CreateBrush(const wxBrush&  )  { return wxNullGraphicsBrush; }
    virtual wxGraphicsBrush CreateLinearGradientBrush(wxDouble , wxDouble , wxDouble , wxDouble ,
                                                      const wxColour&, const wxColour&)  { return wxNullGraphicsBrush; }
    virtual wxGraphicsBrush CreateRadialGradientBrush(wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ,
                                                      const wxColour &, const wxColour &)  { return wxNullGraphicsBrush; }
    virtual wxGraphicsFont CreateFont( const wxFont & , const wxColour & ) { return wxNullGraphicsFont; }
};



class wxGCDC: public wxWindowDC
{
public:
    wxGCDC(const wxWindowDC&) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGCDC is not available on this platform.");
        wxPyEndBlockThreads(blocked);
     }

    wxGCDC(const wxWindow*) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGCDC is not available on this platform.");
        wxPyEndBlockThreads(blocked);
     }

    wxGCDC() {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGCDC is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }

    virtual ~wxGCDC() {}

    wxGraphicsContext* GetGraphicsContext() { return NULL; }
    void SetGraphicsContext( wxGraphicsContext* ) {}
};

#endif
%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


%typemap(in) (size_t points, wxPoint2D* points_array ) {
    $2 = wxPoint2D_LIST_helper($input, &$1);
    if ($2 == NULL) SWIG_fail;
}
%typemap(freearg) (size_t points, wxPoint2D* points_array ) {
    if ($2) delete [] $2;
}



MustHaveApp(wxGraphicsPath);
MustHaveApp(wxGraphicsContext);
MustHaveApp(wxGCDC);

typedef double wxDouble;


//---------------------------------------------------------------------------


DocStr(wxGraphicsObject,
"This class is the superclass of native graphics objects like pens
etc. It provides the internal reference counting.  It is not to be
instantiated by user code.", "");
class wxGraphicsObject : public wxObject
{
public :
    wxGraphicsObject( wxGraphicsRenderer* renderer = NULL );
    virtual ~wxGraphicsObject();
    
    DocDeclStr(
        bool , IsNull() const ,
        "Is this object valid (false) or still empty (true)?", "");
    
    DocDeclStr(
        wxGraphicsRenderer* , GetRenderer() const,
        "Returns the renderer that was used to create this instance, or
``None`` if it has not been initialized yet.", "");
    
};


DocStr(wxGraphicsPen,
"A wx.GraphicsPen is a native representation of a pen. It is used for
stroking a path on a `wx.GraphicsContext`. The contents are specific and
private to the respective renderer. The only way to get a valid instance
is via a CreatePen call on the graphics context or the renderer
instance.", "");
class wxGraphicsPen : public wxGraphicsObject
{
public :
    wxGraphicsPen();
    virtual ~wxGraphicsPen();
};

DocStr(wxGraphicsBrush,
"A wx.GraphicsBrush is a native representation of a brush. It is used
for filling a path on a `wx.GraphicsContext`. The contents are
specific and private to the respective renderer. The only way to get a
valid instance is via a Create...Brush call on the graphics context or
the renderer instance.", "");
class wxGraphicsBrush : public wxGraphicsObject
{
public :
    wxGraphicsBrush();
    virtual ~wxGraphicsBrush();
};


DocStr(wxGraphicsFont,
"A `wx.GraphicsFont` is a native representation of a font (including
text colour). The contents are specific an private to the respective
renderer.  The only way to get a valid instance is via a CreateFont
call on the graphics context or the renderer instance.", "");
class wxGraphicsFont : public wxGraphicsObject
{
public :
    wxGraphicsFont();
    virtual ~wxGraphicsFont();
};


//---------------------------------------------------------------------------

DocStr(wxGraphicsMatrix,
"A wx.GraphicsMatrix is a native representation of an affine
matrix. The contents are specific an private to the respective
renderer. The only way to get a valid instance is via a CreateMatrix
call on the graphics context or the renderer instance.", "");
class wxGraphicsMatrix : public wxGraphicsObject
{
public :
    wxGraphicsMatrix(); 
    virtual ~wxGraphicsMatrix();

    DocDeclStr(
        virtual void , Concat( const wxGraphicsMatrix& t ),
        "Concatenates the passed in matrix to the current matrix.", "");

//     %extend  {
//         DocStr(Copy,
//                "Copy the passed in matrix to this one.", "");
//         void Copy( const wxGraphicsMatrix& t ) {
//             *self = t;
//         }
//     }


    DocDeclStr(
        virtual void , Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
                           wxDouble tx=0.0, wxDouble ty=0.0),
        "Sets the matrix to the specified values (default values are the
identity matrix.)", "");

    
    DocDeclAStr(
        virtual void , Get(wxDouble* OUTPUT, wxDouble* OUTPUT, wxDouble* OUTPUT,
                           wxDouble* OUTPUT, wxDouble* OUTPUT, wxDouble* OUTPUT),
        "Get(self) --> (a, b, c, d, tx, ty)",
        "Gets the component values of the matrix and returns them as a tuple.", "");
    

    DocDeclStr(
        virtual void , Invert(),
        "Inverts the matrix.", "");


    DocDeclStr(
        virtual bool , IsEqual( const wxGraphicsMatrix& t) const,
        "Returns ``True`` if the elements of the transformation matrix are equal", "");


    DocDeclStr(
        virtual bool , IsIdentity() const,
        "Returns ``True`` if this is the identity matrix", "");


    DocDeclStr(
        virtual void , Translate( wxDouble dx , wxDouble dy ),
        "Add a translation to this matrix.", "");


    DocDeclStr(
        virtual void , Scale( wxDouble xScale , wxDouble yScale ),
        "Scales this matrix.", "");


    DocDeclStr(
        virtual void , Rotate( wxDouble angle ),
        "Rotates this matrix.  The angle should be specified in radians.", "");


    DocDeclAStr(
        virtual void , TransformPoint( wxDouble *INOUT, wxDouble *INOUT ) const,
        "TransformPoint(self, x, y) --> (x, y)",
        "Applies this matrix to a point, returns the resulting point values", "");


    DocDeclAStr(
        virtual void , TransformDistance( wxDouble *INOUT, wxDouble *INOUT ) const,
        "TransformDistance(self, dx, dy) --> (dx, dy)",
        "Applies this matrix to a distance (ie. performs all transforms except
translations)", "");


    DocDeclStr(
        virtual void * , GetNativeMatrix() const,
        "Returns the native representation of the matrix. For CoreGraphics this
is a CFAffineMatrix pointer. For GDIPlus a Matrix Pointer and for
Cairo a cairo_matrix_t pointer.  NOTE: For wxPython we still need a
way to make this value usable.", "");
};

//---------------------------------------------------------------------------

class wxGraphicsPath : public wxGraphicsObject
{
public :
    wxGraphicsPath();
    virtual ~wxGraphicsPath();


    %nokwargs MoveToPoint;
    DocStr(MoveToPoint,
           "Begins a new subpath at the specified point.", "");
    virtual void  MoveToPoint( wxDouble x, wxDouble y );
    void MoveToPoint( const wxPoint2D& p);


    %nokwargs AddLineToPoint;
    DocStr(AddLineToPoint,
        "Adds a straight line from the current point to the specified point.", "");
    virtual void AddLineToPoint( wxDouble x, wxDouble y );
    void AddLineToPoint( const wxPoint2D& p);


    %nokwargs AddCurveToPoint;
    DocStr(AddCurveToPoint,
        "Adds a cubic Bezier curve from the current point, using two control
points and an end point", "");
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1,
                                  wxDouble cx2, wxDouble cy2,
                                  wxDouble x, wxDouble y );
    void AddCurveToPoint( const wxPoint2D& c1, const wxPoint2D& c2, const wxPoint2D& e);



    DocDeclStr(
        virtual void , AddPath( const wxGraphicsPath& path ),
        "Adds another path", "");


    DocDeclStr(
        virtual void , CloseSubpath(),
        "Closes the current sub-path.", "");


    DocDeclStr(
        wxPoint2D , GetCurrentPoint() const,
        "Gets the last point of the current path, (0,0) if not yet set", "");


    %nokwargs AddArc;
    DocStr(AddArc,
        "Adds an arc of a circle centering at (x,y) with radius (r) from
startAngle to endAngle", "");
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r,
                         wxDouble startAngle, wxDouble endAngle, bool clockwise );
    void AddArc( const wxPoint2D& c, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise);


    DocDeclStr(
        virtual void , AddQuadCurveToPoint( wxDouble cx, wxDouble cy, wxDouble x, wxDouble y ),
        "Adds a quadratic Bezier curve from the current point, using a control
point and an end point", "");


    DocDeclStr(
        virtual void , AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "Appends a rectangle as a new closed subpath.", "");


    DocDeclStr(
        virtual void , AddCircle( wxDouble x, wxDouble y, wxDouble r ),
        "Appends a circle around (x,y) with radius r as a new closed subpath.", "");


    DocDeclStr(
        virtual void , AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r ) ,
        "Appends an arc to two tangents connecting (current) to (x1,y1) and (x1,y1)
to (x2,y2), also a straight line from (current) to (x1,y1)", "");


    DocDeclStr(
        virtual void , AddEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "Appends an ellipse fitting into the passed in rectangle.", "");


    DocDeclStr(
        virtual void , AddRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius),
        "Appends a rounded rectangle.", "");


    DocDeclStr(
        virtual void * , GetNativePath() const,
        "Returns the native path (CGPathRef for Core Graphics, Path pointer for
GDIPlus and a cairo_path_t pointer for cairo).  NOTE: For wxPython we
still need a way to make this value usable.", "");


    DocDeclStr(
        virtual void , UnGetNativePath(void *p) const,
        "Gives back the native path returned by GetNativePath() because there
might be some deallocations necessary (eg on cairo the native path
returned by GetNativePath is newly allocated each time).", "");


    DocDeclStr(
        virtual void , Transform( const wxGraphicsMatrix& matrix ),
        "Transforms each point of this path by the matrix", "");


    DocDeclStr(
        wxRect2DDouble , GetBox() const,
        "Gets the bounding box enclosing all points (possibly including control points)", "");


    %nokwargs Contains;
    DocStr(Contains,
        "Returns ``True`` if the point is within the path.", "");
    virtual bool Contains( wxDouble x, wxDouble y, int fillStyle = wxODDEVEN_RULE) const;
    bool Contains( const wxPoint2DDouble& c, int fillStyle = wxODDEVEN_RULE) const;

};


//---------------------------------------------------------------------------

%immutable;
const wxGraphicsPen     wxNullGraphicsPen;
const wxGraphicsBrush   wxNullGraphicsBrush;
const wxGraphicsFont    wxNullGraphicsFont;
const wxGraphicsMatrix  wxNullGraphicsMatrix;
const wxGraphicsPath    wxNullGraphicsPath;
%mutable;

//---------------------------------------------------------------------------

DocStr(wxGraphicsContext,
"A `wx.GraphicsContext` instance is the object that is drawn upon. It is
created by a renderer using the CreateContext calls, this can be done
either directly using a renderer instance, or indirectly using the
static convenience CreateXXX functions of wx.GraphicsContext that
always delegate the task to the default renderer.", "");

class wxGraphicsContext : public wxGraphicsObject
{
public:
    //wxGraphicsContext();         This is an ABC, use Create to make an instance...
    virtual ~wxGraphicsContext();

    %newobject Create;
    %nokwargs Create;
    %pythonAppend Create
        "val.__dc = args[0] # save a ref so the dc will not be deleted before self";
    DocStr(Create,
           "Creates a wx.GraphicsContext either from a window or a DC.", "");
    static wxGraphicsContext* Create( const wxWindowDC& dc);
    static wxGraphicsContext* Create( wxWindow* window ) ;

    DocDeclStrName(
        static wxGraphicsContext* , Create(),
        "Create a lightwieght context that can be used for measuring text only.", "",
        CreateMeasuringContext);
    
    %newobject CreateFromNative;
    DocDeclStr(
        static wxGraphicsContext* , CreateFromNative( void * context ) ,
        "Creates a wx.GraphicsContext from a native context. This native context
must be eg a CGContextRef for Core Graphics, a Graphics pointer for
GDIPlus or a cairo_t pointer for Cairo.", "");
    

    %newobject CreateFromNative;
    DocDeclStr(
        static wxGraphicsContext* , CreateFromNativeWindow( void * window ) ,
        "Creates a wx.GraphicsContext from a native window.", "");
    


    DocDeclStr(
        virtual wxGraphicsPath , CreatePath(),
        "Creates a native graphics path which is initially empty.", "");


    DocDeclStr(
        virtual wxGraphicsPen , CreatePen(const wxPen& pen),
        "Creates a native pen from a `wx.Pen`.", "");


    DocDeclStr(
        virtual wxGraphicsBrush , CreateBrush(const wxBrush& brush ),
        "Creates a native brush from a `wx.Brush`.", "");


    DocDeclStr(
        virtual wxGraphicsBrush ,
        CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                                   const wxColour& c1, const wxColour& c2),
        "Creates a native brush, having a linear gradient, starting at (x1,y1)
with color c1 to (x2,y2) with color c2.", "");

    
    DocDeclStr(
        virtual wxGraphicsBrush ,
        CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
                                   const wxColour &oColor, const wxColour &cColor),
        "Creates a native brush, having a radial gradient originating at 
point (xo,yc) with color oColour and ends on a circle around (xc,yc) with
radius r and color cColour.", "");


    DocDeclStr(
        virtual wxGraphicsFont , CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ),
        "Creates a native graphics font from a `wx.Font` and a text colour.", "");


    DocDeclStr(
        virtual wxGraphicsMatrix , CreateMatrix( wxDouble a=1.0, wxDouble b=0.0,
                                                 wxDouble c=0.0, wxDouble d=1.0,
                                                 wxDouble tx=0.0, wxDouble ty=0.0),
        "Creates a native affine transformation matrix from the passed in
values. The defaults result in an identity matrix.", "");



    DocDeclStr(
        virtual void , PushState(),
        "push the current state of the context, ie the transformation matrix on a stack", "");


    DocDeclStr(
        virtual void , PopState(),
        "pops a stored state from the stack", "");


    DocDeclStrName(
        virtual void , Clip( const wxRegion &region ),
        "Clips drawings to the region, combined to current clipping region.", "",
        ClipRegion);

    
    DocDeclStr(
        virtual void , Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "Clips drawings to the rectangle.", "");


    DocDeclStr(
        virtual void , ResetClip(),
        "Resets the clipping to original shape.", "");


    DocDeclStr(
        virtual void * , GetNativeContext(),
        "Returns the native context (CGContextRef for Core Graphics, Graphics
pointer for GDIPlus and cairo_t pointer for cairo).", "");

    
    DocDeclStr(
        virtual int , GetLogicalFunction() const,
        "Returns the current logical function.", "");
    
    
    DocDeclStr(
        virtual bool , SetLogicalFunction(int function) ,
        "Sets the current logical function, returns ``True`` if it supported", "");
       
    
    DocDeclStr(
        virtual void , Translate( wxDouble dx , wxDouble dy ),
        "Translates the current transformation matrix.", "");


    DocDeclStr(
        virtual void , Scale( wxDouble xScale , wxDouble yScale ),
        "Scale the current transformation matrix of the context.", "");


    DocDeclStr(
        virtual void , Rotate( wxDouble angle ),
        "Rotate the current transformation matrix of the context.  ``angle`` is
specified in radians.", "");


    DocDeclStr(
        virtual void , ConcatTransform( const wxGraphicsMatrix& matrix ),
        "Concatenates the passed in transform with the current transform of
this context.", "");
    

    DocDeclStr(
        virtual void , SetTransform( const wxGraphicsMatrix& matrix ),
        "Sets the current transform of this context.", "");
    

    DocDeclStr(
        virtual wxGraphicsMatrix , GetTransform() const,
        "Gets the current transformation matrix of this context.", "");
    
    


    DocStr(SetPen, "Sets the stroke pen", "");
    %nokwargs SetPen;
    virtual void SetPen( const wxGraphicsPen& pen );
    void SetPen( const wxPen& pen );

    
    DocStr(SetBrush, "Sets the brush for filling", "");
    %nokwargs SetBrush;
    virtual void SetBrush( const wxGraphicsBrush& brush );
    void SetBrush( const wxBrush& brush );

    
    DocStr(SetFont, "Sets the font", "");
    %nokwargs SetFont;
    virtual void SetFont( const wxGraphicsFont& font );
    void SetFont( const wxFont& font, const wxColour& colour = *wxBLACK);

    
   
    DocDeclStr(
        virtual void , StrokePath( const wxGraphicsPath& path ),
        "Strokes along a path with the current pen.", "");

    
    DocDeclStr(
        virtual void , FillPath( const wxGraphicsPath& path, int fillStyle = wxODDEVEN_RULE ),
        "Fills a path with the current brush.", "");

   
    DocDeclStr(
        virtual void , DrawPath( const wxGraphicsPath& path, int fillStyle = wxODDEVEN_RULE ),
        "Draws the path by first filling and then stroking.", "");


    %extend {
        DocStr(DrawText,
               "Draws a text string at the defined position.", "");
        void DrawText( const wxString &str, wxDouble x, wxDouble y,
                       const wxGraphicsBrush& backgroundBrush = wxNullGraphicsBrush )
        {
            if ( !backgroundBrush.IsNull() )
                self->DrawText(str, x, y, backgroundBrush);
            else
                self->DrawText(str, x, y);
        }

        DocStr(DrawRotatedText,
               "Draws a text string at the defined position, at the specified angle,
which is given in radians.", "");
        void DrawRotatedText(  const wxString &str, wxDouble x, wxDouble y, wxDouble angle,
                               const wxGraphicsBrush& backgroundBrush = wxNullGraphicsBrush )
        {
            if ( !backgroundBrush.IsNull() )
                self->DrawText(str, x, y, angle, backgroundBrush);
            else
                self->DrawText(str, x, y, angle);
        }
    }
   


    DocDeclAStrName(
        virtual void , GetTextExtent( const wxString &text,
                                      wxDouble *OUTPUT /*width*/,
                                      wxDouble *OUTPUT /*height*/,
                                      wxDouble *OUTPUT /*descent*/,
                                      wxDouble *OUTPUT /*externalLeading*/ ) const ,
        "GetFullTextExtent(self, text) --> (width, height, descent, externalLeading)",
        "Gets the dimensions of the string using the currently selected
font. ``text`` is the string to measure, ``w`` and ``h`` are the total
width and height respectively, ``descent`` is the dimension from the
baseline of the font to the bottom of the descender, and
``externalLeading`` is any extra vertical space added to the font by
the font designer (usually is zero).", "",
        GetFullTextExtent);

    %extend {
        DocAStr(GetTextExtent,
                "GetTextExtent(self, text) --> (width, height)",
                "Gets the dimensions of the string using the currently selected
font. ``text`` is the string to measure, ``w`` and ``h`` are the total
width and height respectively.", "");

        PyObject* GetTextExtent( const wxString &text )
        {
            wxDouble width = 0.0,
                     height = 0.0;
            self->GetTextExtent(text, &width, &height, NULL, NULL);
            // thread wrapers are turned off for this .i file, so no need to acquire GIL...
            PyObject* rv = PyTuple_New(2);
            PyTuple_SET_ITEM(rv, 0, PyFloat_FromDouble(width));
            PyTuple_SET_ITEM(rv, 1, PyFloat_FromDouble(height));
            return rv;
        }
    }


    %extend {
        DocAStr(GetPartialTextExtents,
                "GetPartialTextExtents(self, text) -> [widths]",
                "Returns a list of widths from the beginning of ``text`` to the
coresponding character in ``text``.", "");
        wxArrayDouble GetPartialTextExtents(const wxString& text) {
            wxArrayDouble widths;
            self->GetPartialTextExtents(text, widths);
            return widths;
        }
    }


    DocDeclStr(
        virtual void , DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "Draws the bitmap. In case of a mono bitmap, this is treated as a mask
and the current brush is used for filling.", "");


    DocDeclStr(
        virtual void , DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "Draws the icon.", "");



    DocDeclStr(
        virtual void , StrokeLine( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2),
        "Strokes a single line.", "");

    
    DocDeclAStr(
        virtual void , StrokeLines( size_t points, const wxPoint2D *points_array),
        "StrokeLines(self, List points)",
        "Stroke lines connecting each of the points", "");


    %extend {
        DocStr(StrokeLineSegements,
               "Stroke disconnected lines from begin to end points", "");
        void StrokeLineSegements(PyObject* beginPoints, PyObject* endPoints)
        {
            size_t c1, c2, count;
            wxPoint2D* beginP = wxPoint2D_LIST_helper(beginPoints, &c1);
            wxPoint2D* endP =   wxPoint2D_LIST_helper(endPoints, &c2);

            if ( beginP != NULL && endP != NULL )
            {
                count = wxMin(c1, c2);
                self->StrokeLines(count, beginP, endP);
            }
            delete [] beginP;
            delete [] endP;
        }
    }


    DocDeclStr(
        virtual void , DrawLines( size_t points, const wxPoint2D *points_array,
                                  int fillStyle = wxODDEVEN_RULE ),
        "Draws a polygon.", "");


    DocDeclStr(
        virtual void , DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "Draws a rectangle.", "");


    DocDeclStr(
        virtual void , DrawEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "Draws an ellipse.", "");


    DocDeclStr(
        virtual void , DrawRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius),
        "Draws a rounded rectangle", "");

    

    DocDeclStr(
        virtual bool , ShouldOffset() const,
        "helper to determine if a 0.5 offset should be applied for the drawing operation", "");
    
};


//---------------------------------------------------------------------------

class wxGraphicsRenderer : public wxObject
{
public :
    // wxGraphicsRenderer();  This is an ABC, use GetDefaultRenderer

    virtual ~wxGraphicsRenderer();

    // %newobject GetDefaultRenderer;  ???
    static wxGraphicsRenderer* GetDefaultRenderer();

    %nokwargs CreateContext;
    %newobject CreateContext;
    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) ;
    virtual wxGraphicsContext * CreateContext( wxWindow* window );
    
    // create a context that can be used for measuring texts only, no drawing allowed
    virtual wxGraphicsContext * CreateMeasuringContext();
    
    %newobject CreateContextFromNativeContext;
    virtual wxGraphicsContext * CreateContextFromNativeContext( void * context );

    %newobject CreateContextFromNativeWindow;
    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window );


    virtual wxGraphicsPath CreatePath();

    virtual wxGraphicsMatrix CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
                                           wxDouble tx=0.0, wxDouble ty=0.0);
        
    virtual wxGraphicsPen CreatePen(const wxPen& pen) ;
    
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush ) ;
    
    virtual wxGraphicsBrush CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
                                                        const wxColour&c1, const wxColour&c2);

    virtual wxGraphicsBrush CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
                                                        const wxColour &oColor, const wxColour &cColor);

    virtual wxGraphicsFont CreateFont( const wxFont &font , const wxColour &col = *wxBLACK );
    
};



//---------------------------------------------------------------------------

%{
#include "wx/dcgraph.h"
%}

class wxGCDC: public wxDC
{
public:
    %nokwargs wxGCDC;
    %pythonAppend wxGCDC
        "self.__dc = args[0] # save a ref so the other dc will not be deleted before self";
    wxGCDC(const wxWindowDC& dc);
    wxGCDC(wxWindow* window);
    //wxGCDC();
    virtual ~wxGCDC();

    wxGraphicsContext* GetGraphicsContext();
    virtual void SetGraphicsContext( wxGraphicsContext* ctx );

    %property(GraphicsContext, GetGraphicsContext, SetGraphicsContext);
};


//---------------------------------------------------------------------------

// Turn GIL acquisition back on.
%threadWrapperOn

