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


class wxGraphicsObject : public wxObject
{
public :
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
    wxGraphicsPath(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsPath is not available on this platform.");
    }
    virtual ~wxGraphicsPath() {}

    virtual wxGraphicsPath *Clone() const { return NULL; }

    void MoveToPoint( wxDouble, wxDouble ) {}
    void MoveToPoint( const wxPoint2DDouble& ) {}
    void AddLineToPoint( wxDouble, wxDouble ) {}
    void AddLineToPoint( const wxPoint2DDouble& ) {}
    void AddCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddCurveToPoint( const wxPoint2DDouble&, const wxPoint2DDouble&, const wxPoint2DDouble&) {}
    void AddPath( const wxGraphicsPath* ) {}
    void CloseSubpath() {}
    void GetCurrentPoint( wxDouble&, wxDouble&) {}
    wxPoint2DDouble GetCurrentPoint() { reutrn wxPoint2D(0,0); }
    void AddArc( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, bool ) {}
    void AddArc( const wxPoint2DDouble& , wxDouble, wxDouble , wxDouble , bool ) {}

    void AddQuadCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddRectangle( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddCircle( wxDouble, wxDouble, wxDouble ) {}
    void AddArcToPoint( wxDouble, wxDouble , wxDouble, wxDouble, wxDouble )  {}

    void AddEllipse( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void AddRoundedRectangle( wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void * GetNativePath() const { return NULL; }
    void UnGetNativePath(void *) {}
    void Transform( wxGraphicsMatrix* ) {}
    void GetBox(wxDouble *, wxDouble *, wxDouble *, wxDouble *) {}
    wxRect2D GetBox() { return wxRect2D(0,0,0,0); }

    bool Contains( wxDouble , wxDouble , int ) { return false; }
    bool Contains( const wxPoint2DDouble& , int ) { return false; }
};


class wxGraphicsMatrix : public wxGraphicsObject
{
public :
    wxGraphicsMatrix(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsMatrix is not available on this platform.");
    }
    virtual ~wxGraphicsMatrix() {}
    virtual wxGraphicsMatrix *Clone() const { return NULL; }
    virtual void Concat( const wxGraphicsMatrix * ) {}
    virtual void Copy( const wxGraphicsMatrix * )  {}
    virtual void Set(wxDouble , wxDouble , wxDouble , wxDouble ,
                     wxDouble , wxDouble ) {}
    virtual void Invert() {}
    virtual bool IsEqual( const wxGraphicsMatrix* t) const  {}
    virtual bool IsIdentity() { return false; }
    virtual void Translate( wxDouble , wxDouble ) {}
    virtual void Scale( wxDouble , wxDouble  ) {}
    virtual void Rotate( wxDouble  ) {}
    virtual void TransformPoint( wxDouble *, wxDouble * ) {}
    virtual void TransformDistance( wxDouble *, wxDouble * ) {}
    virtual void * GetNativeMatrix() const { return NULL; }
};



class wxGraphicsContext : public wxGraphicsObject
{
public:

    wxGraphicsContext(wxGraphicsRenderer* ) {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    virtual ~wxGraphicsContext() {}

    static wxGraphicsContext* Create( const wxWindowDC& )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    static wxGraphicsContext* CreateFromNative( void *  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    static wxGraphicsContext* CreateFromNativeWindow( void *  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    static wxGraphicsContext* Create( wxWindow*  )  {
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsContext is not available on this platform.");
    }

    wxGraphicsPath * CreatePath()  { return NULL; }

    virtual wxGraphicsPen CreatePen(const wxPen& )  { return NULL; }

    virtual wxGraphicsBrush CreateBrush(const wxBrush& ) { return NULL; }

    virtual wxGraphicsBrush CreateLinearGradientBrush( wxDouble , wxDouble , wxDouble , wxDouble ,
                                                        const wxColour&, const wxColour&) { return NULL; }

    virtual wxGraphicsBrush CreateRadialGradientBrush( wxDouble xo, wxDouble yo,
                                                        wxDouble xc, wxDouble yc, wxDouble radius,
                                                        const wxColour &oColor, const wxColour &cColor) { return NULL; }

    virtual wxGraphicsFont CreateFont( const wxFont &, const wxColour & )  { return NULL; }

    virtual wxGraphicsMatrix* CreateMatrix( wxDouble, wxDouble, wxDouble, wxDouble,
                                            wxDouble, wxDouble)  { return NULL; }

    virtual void PushState() {}
    virtual void PopState() {}
    virtual void Clip( const wxRegion & ) {}
    virtual void Clip( wxDouble , wxDouble , wxDouble , wxDouble  ) {}
    virtual void ResetClip() {}
    virtual void * GetNativeContext() { return NULL; }
    virtual void Translate( wxDouble , wxDouble ) {}
    virtual void Scale( wxDouble , wxDouble ) {}
    virtual void Rotate( wxDouble ) {}
    virtual void ConcatTransform( const wxGraphicsMatrix* ) {}
    virtual void SetTransform( const wxGraphicsMatrix* ) {}
    virtual void GetTransform( wxGraphicsMatrix* ) {}

    virtual void SetPen( const wxGraphicsPen& ) {}
    void SetPen( const wxPen& ) {}

    virtual void SetBrush( const wxGraphicsBrush& ) {}
    void SetBrush( const wxBrush& ) {}

    virtual void SetFont( const wxGraphicsFont& ) {}
    void SetFont( const wxFont&, const wxColour& ) {}

    virtual void StrokePath( const wxGraphicsPath * ) {}
    virtual void FillPath( const wxGraphicsPath *, int ) {}
    virtual void DrawPath( const wxGraphicsPath *, int ) {}

    virtual void DrawText( const wxString &, wxDouble , wxDouble  )  {}
    virtual void DrawText( const wxString &, wxDouble , wxDouble , wxDouble ) {}
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

    static wxGraphicsRenderer* GetDefaultRenderer(
        PyErr_SetString(PyExc_NotImplementedError,
                        "wx.GraphicsRenderer is not available on this platform.");
    );

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& ) { return NULL; }
    virtual wxGraphicsContext * CreateContextFromNativeContext( void *  ) { return NULL; }
    virtual wxGraphicsContext * CreateContextFromNativeWindow( void *  )  { return NULL; }
    virtual wxGraphicsContext * CreateContext( wxWindow*  ) { return NULL; }

    virtual wxGraphicsPath * CreatePath()  { return NULL; }

    virtual wxGraphicsMatrix * CreateMatrix( wxDouble , wxDouble , wxDouble , wxDouble ,
                                             wxDouble , wxDouble ) { return NULL; }

    virtual wxGraphicsPen CreatePen(const wxPen& )  { return wxNullGaphicsPen; }
    virtual wxGraphicsBrush CreateBrush(const wxBrush&  )  { return wxNullGaphicsBrush; }
    virtual wxGraphicsBrush CreateLinearGradientBrush(xDouble , wxDouble , wxDouble , wxDouble ,
                                                      const wxColour&, const wxColour&)  { return wxNullGaphicsBrush; }
    virtual wxGraphicsBrush CreateRadialGradientBrush(wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ,
                                                      const wxColour &, const wxColour &)  { return wxNullGaphicsBrush; }
    virtual wxGraphicsFont CreateFont( const wxFont & , const wxColour & ) { return wxNullGaphicsFont; }
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


class wxGraphicsObject : public wxObject
{
public :
    wxGraphicsObject( wxGraphicsRenderer* renderer = NULL );
    virtual ~wxGraphicsObject();
    bool IsNull() const ;
    wxGraphicsRenderer* GetRenderer() const;
};


class wxGraphicsPen : public wxGraphicsObject
{
public :
    wxGraphicsPen();
    virtual ~wxGraphicsPen();
};


class wxGraphicsBrush : public wxGraphicsObject
{
public :
    wxGraphicsBrush();
    virtual ~wxGraphicsBrush();
};


class wxGraphicsFont : public wxGraphicsObject
{
public :
    wxGraphicsFont();
    virtual ~wxGraphicsFont();
};


%immutable;
const wxGraphicsPen wxNullGraphicsPen;
const wxGraphicsBrush wxNullGraphicsBrush;
const wxGraphicsFont wxNullGraphicsFont;
%mutable;

//---------------------------------------------------------------------------

class wxGraphicsPath : public wxGraphicsObject
{
public :
    //wxGraphicsPath(wxGraphicsRenderer* renderer);            *** This class is an ABC, so we can't allow instances to be created directly
    virtual ~wxGraphicsPath();

    virtual wxGraphicsPath *Clone() const = 0;


    %nokwargs MoveToPoint;
    DocStr(MoveToPoint,
        "Begins a new subpath at (x,y)", "");
    virtual void  MoveToPoint( wxDouble x, wxDouble y );
    void MoveToPoint( const wxPoint2D& p);


    %nokwargs AddLineToPoint;
    DocStr(AddLineToPoint,
        "Adds a straight line from the current point to (x,y) ", "");
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
        virtual void , AddPath( const wxGraphicsPath* path ),
        "adds another path", "");


    DocDeclStr(
        virtual void , CloseSubpath(),
        "closes the current sub-path", "");


    DocDeclStr(
        wxPoint2D , GetCurrentPoint(),
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
        "Appends a rectangle as a new closed subpath", "");


    DocDeclStr(
        virtual void , AddCircle( wxDouble x, wxDouble y, wxDouble r ),
        "Appends a circle as a new closed subpath with the given radius.", "");


    DocDeclStr(
        virtual void , AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r ) ,
        "Draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1)
to (x2,y2), also a straight line from (current) to (x1,y1)", "");


    DocDeclStr(
        virtual void , AddEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "appends an ellipse", "");


    DocDeclStr(
        virtual void , AddRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius),
        "appends a rounded rectangle", "");


    DocDeclStr(
        virtual void * , GetNativePath() const,
        "returns the native path", "");


    DocDeclStr(
        virtual void , UnGetNativePath(void *p),
        "give the native path returned by GetNativePath() back (there might be some
deallocations necessary)", "");


    DocDeclStr(
        virtual void , Transform( wxGraphicsMatrix* matrix ),
        "transforms each point of this path by the matrix", "");


    DocDeclStr(
        wxRect2DDouble , GetBox(),
        "gets the bounding box enclosing all points (possibly including control points)", "");


    %nokwargs Contains;
    DocStr(Contains,
        "", "");
    virtual bool Contains( wxDouble x, wxDouble y, int fillStyle = wxWINDING_RULE);
    bool Contains( const wxPoint2DDouble& c, int fillStyle = wxWINDING_RULE);

};

//---------------------------------------------------------------------------

class wxGraphicsMatrix : public wxGraphicsObject
{
public :
    // wxGraphicsMatrix(wxGraphicsRenderer* renderer);     *** This class is an ABC

    virtual ~wxGraphicsMatrix();

    virtual wxGraphicsMatrix *Clone() const;

    DocDeclStr(
        virtual void , Concat( const wxGraphicsMatrix *t ),
        "concatenates the matrix", "");


    DocDeclStr(
        virtual void , Copy( const wxGraphicsMatrix *t ),
        "copies the passed in matrix", "");


    DocDeclStr(
        virtual void , Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
                           wxDouble tx=0.0, wxDouble ty=0.0),
        "sets the matrix to the respective values", "");


    DocDeclStr(
        virtual void , Invert(),
        "makes this the inverse matrix", "");


    DocDeclStr(
        virtual bool , IsEqual( const wxGraphicsMatrix* t) const,
        "returns true if the elements of the transformation matrix are equal", "");


    DocDeclStr(
        virtual bool , IsIdentity(),
        "return true if this is the identity matrix", "");


    DocDeclStr(
        virtual void , Translate( wxDouble dx , wxDouble dy ),
        "add the translation to this matrix", "");


    DocDeclStr(
        virtual void , Scale( wxDouble xScale , wxDouble yScale ),
        "add the scale to this matrix", "");


    DocDeclStr(
        virtual void , Rotate( wxDouble angle ),
        "add the rotation to this matrix (radians)", "");


    DocDeclAStr(
        virtual void , TransformPoint( wxDouble *INOUT, wxDouble *INOUT ),
        "TransformPoint(self, x, y) --> (x, y)",
        "applies that matrix to the point", "");


    DocDeclAStr(
        virtual void , TransformDistance( wxDouble *INOUT, wxDouble *INOUT ),
        "TransformDistance(self, dx, dy) --> (dx, dy)",
        "applies the matrix except for translations", "");


    DocDeclStr(
        virtual void * , GetNativeMatrix() const,
        "returns the native representation", "");
};

//---------------------------------------------------------------------------


class wxGraphicsContext : public wxGraphicsObject
{
public:
    // wxGraphicsContext()         This is also an ABC, use Create to make an instance...
    virtual ~wxGraphicsContext();

    %newobject Create;
    %nokwargs Create;
    %pythonAppend Create
        "val.__dc = args[0] # save a ref so the dc will not be deleted before self";
    static wxGraphicsContext* Create( const wxWindowDC& dc);

    static wxGraphicsContext* Create( wxWindow* window ) ;

    %newobject CreateFromNative;
    static wxGraphicsContext* CreateFromNative( void * context ) ;

    %newobject CreateFromNative;
    static wxGraphicsContext* CreateFromNativeWindow( void * window ) ;


    %newobject CreatePath;
    DocDeclStr(
        virtual wxGraphicsPath * , CreatePath(),
        "creates a path instance that corresponds to the type of graphics context, ie GDIPlus, Cairo, CoreGraphics ...", "");


    DocDeclStr(
        virtual wxGraphicsPen , CreatePen(const wxPen& pen),
        "", "");


    DocDeclStr(
        virtual wxGraphicsBrush , CreateBrush(const wxBrush& brush ),
        "", "");


    DocDeclStr(
        virtual wxGraphicsBrush ,
        CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                                   const wxColour& c1, const wxColour& c2),
        "sets the brush to a linear gradient, starting at (x1,y1) with color c1
to (x2,y2) with color c2", "");

    
    DocDeclStr(
        virtual wxGraphicsBrush ,
        CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
                                   const wxColour &oColor, const wxColour &cColor),
        "sets the brush to a radial gradient originating at (xo,yc) with color
oColor and ends on a circle around (xc,yc) with radius r and color
cColor
", "");


    DocDeclStr(
        virtual wxGraphicsFont , CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ),
        "sets the font", "");


    %newobject CreateMatrix;
    DocDeclStr(
        virtual wxGraphicsMatrix* , CreateMatrix( wxDouble a=1.0, wxDouble b=0.0,
                                                  wxDouble c=0.0, wxDouble d=1.0,
                                                  wxDouble tx=0.0, wxDouble ty=0.0),
        "create a 'native' matrix corresponding to these values", "");



    DocDeclStr(
        virtual void , PushState(),
        "push the current state of the context, ie the transformation matrix on a stack", "");


    DocDeclStr(
        virtual void , PopState(),
        "pops a stored state from the stack", "");


    DocDeclStrName(
        virtual void , Clip( const wxRegion &region ),
        "clips drawings to the region", "",
        ClipRegion);

    
    DocDeclStr(
        virtual void , Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "clips drawings to the rect", "");


    DocDeclStr(
        virtual void , ResetClip(),
        "resets the clipping to original extent", "");


    DocDeclStr(
        virtual void * , GetNativeContext(),
        "returns the native context", "");

    
    DocDeclStr(
        virtual void , Translate( wxDouble dx , wxDouble dy ),
        "translate the current transformation matrix CTM of the context", "");


    DocDeclStr(
        virtual void , Scale( wxDouble xScale , wxDouble yScale ),
        "scale the current transformation matrix CTM of the context", "");


    DocDeclStr(
        virtual void , Rotate( wxDouble angle ),
        "rotate (radians) the current transformation matrix CTM of the context", "");



    DocStr(SetPen, "sets the stroke pen", "");
    %nokwargs SetPen;
    virtual void SetPen( const wxGraphicsPen& pen );
    void SetPen( const wxPen& pen );

    
    DocStr(SetBrush, "sets the brush for filling", "");
    %nokwargs SetBrush;
    virtual void SetBrush( const wxGraphicsBrush& brush );
    void SetBrush( const wxBrush& brush );

    
    DocStr(SetFont, "sets the font", "");
    %nokwargs SetFont;
    virtual void SetFont( const wxGraphicsFont& font );
    void SetFont( const wxFont& font, const wxColour& colour = *wxBLACK);

    
   
    DocDeclStr(
        virtual void , StrokePath( const wxGraphicsPath *path ),
        "strokes along a path with the current pen", "");

    
    DocDeclStr(
        virtual void , FillPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE ),
        "fills a path with the current brush", "");

   
    DocDeclStr(
        virtual void , DrawPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE ),
        "draws a path by first filling and then stroking", "");


    DocDeclStr(
        virtual void , DrawText( const wxString &str, wxDouble x, wxDouble y ),
        "", "");


    DocDeclStrName(
        virtual void , DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle ),
        "", "",
        DrawRotatedText);


    DocDeclAStrName(
        virtual void , GetTextExtent( const wxString &text,
                                      wxDouble *OUTPUT /*width*/,
                                      wxDouble *OUTPUT /*height*/,
                                      wxDouble *OUTPUT /*descent*/,
                                      wxDouble *OUTPUT /*externalLeading*/ ) const ,
        "GetFullTextExtent(self, text) --> (width, height, descent, externalLeading)",
        "", "",
        GetFullTextExtent);

    %extend {
        DocAStr(GetTextExtent,
                "GetTextExtent(self, text) --> (width, height)",
                "", "");

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
                "", "");
        wxArrayDouble GetPartialTextExtents(const wxString& text) {
            wxArrayDouble widths;
            self->GetPartialTextExtents(text, widths);
            return widths;
        }
    }


    DocDeclStr(
        virtual void , DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "", "");


    DocDeclStr(
        virtual void , DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "", "");



    DocDeclStr(
        virtual void , StrokeLine( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2),
        "strokes a single line", "");

    
    DocDeclAStr(
        virtual void , StrokeLines( size_t points, const wxPoint2D *points_array),
        "StrokeLines(self, List points)",
        "stroke lines connecting each of the points", "");


    %extend {
        DocStr(StrokeLineSegements,
               "stroke disconnected lines from begin to end points", "");
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
        virtual void , DrawLines( size_t points, const wxPoint2D *points_array, int fillStyle = wxWINDING_RULE ),
        "draws a polygon", "");


    DocDeclStr(
        virtual void , DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "draws a rectangle", "");


    DocDeclStr(
        virtual void , DrawEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "draws an ellipse", "");


    DocDeclStr(
        virtual void , DrawRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius),
        "draws a rounded rectangle", "");

    

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
    
    %newobject CreateContextFromNativeContext;
    virtual wxGraphicsContext * CreateContextFromNativeContext( void * context );

    %newobject CreateContextFromNativeWindow;
    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window );


    %newobject CreatePath;
    virtual wxGraphicsPath * CreatePath();

    %newobject CreateMatrix;
    virtual wxGraphicsMatrix * CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
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
    %pythonAppend wxGCDC
        "self.__dc = args[0] # save a ref so the other dc will not be deleted before self";
    wxGCDC(const wxWindowDC& dc);
    //wxGCDC();
    virtual ~wxGCDC();

    wxGraphicsContext* GetGraphicsContext();
    virtual void SetGraphicsContext( wxGraphicsContext* ctx );

    %property(GraphicsContext, GetGraphicsContext, SetGraphicsContext);
};


//---------------------------------------------------------------------------

// Turn GIL acquisition back on.
%threadWrapperOn

