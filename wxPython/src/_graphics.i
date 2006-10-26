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


%{
#if !wxUSE_GRAPHICS_CONTEXT
// C++ stub classes for platforms that don't have wxGraphicsContext yet.

class wxGraphicsPath
{
public :
    wxGraphicsPath() {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGraphicsPath is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    virtual ~wxGraphicsPath() {}
    
    void MoveToPoint( wxDouble, wxDouble ) {}
    void AddLineToPoint( wxDouble, wxDouble ) {} 
    void AddCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void CloseSubpath() {}
    void GetCurrentPoint( wxDouble&, wxDouble&) {}
    void AddArc( wxDouble, wxDouble, wxDouble, wxDouble, wxDouble, bool ) {}

    void AddQuadCurveToPoint( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddRectangle( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void AddCircle( wxDouble, wxDouble, wxDouble ) {}
    void AddArcToPoint( wxDouble, wxDouble , wxDouble, wxDouble, wxDouble )  {}

    wxPoint2DDouble GetCurrentPoint() { return wxPoint2DDouble(0,0); }
    void MoveToPoint( const wxPoint2DDouble& ) {}
    void AddLineToPoint( const wxPoint2DDouble&) {}
    void AddCurveToPoint( const wxPoint2DDouble&, const wxPoint2DDouble&, const wxPoint2DDouble&) {}
    void AddArc( const wxPoint2DDouble&, wxDouble, wxDouble, wxDouble, bool) {}
};


class wxGraphicsContext
{
public:
    wxGraphicsContext() {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGraphicsContext is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    virtual ~wxGraphicsContext() {}
    
    static wxGraphicsContext* Create( const wxWindowDC&) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGraphicsPath is not available on this platform.");
        wxPyEndBlockThreads(blocked);
        return NULL;
    }

    static wxGraphicsContext* CreateFromNative( void *  ) {
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxGraphicsContext is not available on this platform.");
        wxPyEndBlockThreads(blocked);
        return NULL;
    }        

    wxGraphicsPath * CreatePath() { return NULL; }
    void PushState() {}
    void PopState() {}
    void Clip( const wxRegion & ) {}
    void Clip( wxDouble, wxDouble, wxDouble, wxDouble ) {}
    void ResetClip() {}
    void * GetNativeContext() { return NULL; }
    void Translate( wxDouble  , wxDouble  ) {}
    void Scale( wxDouble  , wxDouble  ) {}
    void Rotate( wxDouble  ) {}
    void SetPen( const wxPen & ) {}
    void SetBrush( const wxBrush & ) {}
    void SetLinearGradientBrush( wxDouble , wxDouble , wxDouble , wxDouble , 
        const wxColour&, const wxColour&) {}
    void SetRadialGradientBrush( wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ,
        const wxColour &, const wxColour &) {}
    void SetFont( const wxFont & ) {}
    void SetTextColour( const wxColour & ) {}
    void StrokePath( const wxGraphicsPath * ) {}
    void FillPath( const wxGraphicsPath *, int  ) {}
    void DrawPath( const wxGraphicsPath *, int  ) {}
    void DrawText( const wxString &, wxDouble , wxDouble  ) {}
    void DrawText( const wxString &, wxDouble , wxDouble , wxDouble  ) {}
    void GetTextExtent( const wxString &, wxDouble *, wxDouble *,
                        wxDouble *, wxDouble * ) const {}
    void GetPartialTextExtents(const wxString& , wxArrayDouble& ) const {}
    void DrawBitmap( const wxBitmap &, wxDouble , wxDouble , wxDouble , wxDouble  ) {}
    void DrawIcon( const wxIcon &, wxDouble , wxDouble , wxDouble , wxDouble  ) {}
    void StrokeLine( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void StrokeLines( size_t , const wxPoint2DDouble *) {}
    void StrokeLines( size_t , const wxPoint2DDouble *, const wxPoint2DDouble *) {}
    void DrawLines( size_t , const wxPoint2DDouble *, int  ) {}
    void DrawRectangle( wxDouble , wxDouble , wxDouble , wxDouble ) {}
    void DrawEllipse( wxDouble , wxDouble,  wxDouble , wxDouble) {}
    void DrawRoundedRectangle( wxDouble , wxDouble , wxDouble , wxDouble , wxDouble ) {}
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

//#define wxDouble double
typedef double wxDouble;


// TODO:  Decide which of the overloaded methods should use the primary names

class wxGraphicsPath
{
public :
    //wxGraphicsPath();            *** This class is an ABC, so we can't allow instances to be created directly
    virtual ~wxGraphicsPath();
    

    DocDeclStr(
        virtual void , MoveToPoint( wxDouble x, wxDouble y ),
        "Begins a new subpath at (x,y)", "");
//    void MoveToPoint( const wxPoint2D& p);
    

    DocDeclStr(
        virtual void , AddLineToPoint( wxDouble x, wxDouble y ),
        "Adds a straight line from the current point to (x,y) ", "");
//    void AddLineToPoint( const wxPoint2D& p);
    

    DocDeclStr(
        virtual void , AddCurveToPoint( wxDouble cx1, wxDouble cy1,
                                        wxDouble cx2, wxDouble cy2,
                                        wxDouble x, wxDouble y ),
        "Adds a cubic Bezier curve from the current point, using two control
points and an end point", "");
//    void AddCurveToPoint( const wxPoint2D& c1, const wxPoint2D& c2, const wxPoint2D& e);
    

    DocDeclStr(
        virtual void , CloseSubpath(),
        "closes the current sub-path", "");
    

    //virtual void , GetCurrentPoint( wxDouble& x, wxDouble&y),
    DocDeclStr(
        wxPoint2D , GetCurrentPoint(),
        "Gets the last point of the current path, (0,0) if not yet set", "");
    

    DocDeclStr(
        virtual void , AddArc( wxDouble x, wxDouble y, wxDouble r,
                               wxDouble startAngle, wxDouble endAngle, bool clockwise ),
        "Adds an arc of a circle centering at (x,y) with radius (r) from
startAngle to endAngle", "");
//    void AddArc( const wxPoint2D& c, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise);
  

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
    

};

//---------------------------------------------------------------------------

/*
class wxGraphicsMatrix
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


//---------------------------------------------------------------------------


class wxGraphicsContext
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


    %newobject CreatePath;
    // creates a path instance that corresponds to the type of graphics context, ie GDIPlus, cairo, CoreGraphics ...
    DocDeclStr(
        virtual wxGraphicsPath * , CreatePath(),
        "", "");
    
    /*
    // create a 'native' matrix corresponding to these values
    virtual wxGraphicsMatrix* CreateMatrix( wxDouble a=1.0, wxDouble b=0.0,
                                            wxDouble c=0.0, wxDouble d=1.0, 
                                            wxDouble tx=0.0, wxDouble ty=0.0) = 0;
    */

    
    // push the current state of the context, ie the transformation matrix on a stack
    DocDeclStr(
        virtual void , PushState(),
        "", "");
    

    // pops a stored state from the stack
    DocDeclStr(
        virtual void , PopState(),
        "", "");
    

    // clips drawings to the region
    DocDeclStrName(
        virtual void , Clip( const wxRegion &region ),
        "", "",
        ClipRegion);
    
    // clips drawings to the rect
    DocDeclStr(
        virtual void , Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "", "");
    
	
    // resets the clipping to original extent
    DocDeclStr(
        virtual void , ResetClip(),
        "", "");
    

    // returns the native context
    DocDeclStr(
        virtual void * , GetNativeContext(),
        "", "");
    

    
    //
    // transformation: changes the current transformation matrix CTM of the context
    //
    
    // translate
    DocDeclStr(
        virtual void , Translate( wxDouble dx , wxDouble dy ),
        "", "");
    

    // scale
    DocDeclStr(
        virtual void , Scale( wxDouble xScale , wxDouble yScale ),
        "", "");
    

    // rotate (radians)
    DocDeclStr(
        virtual void , Rotate( wxDouble angle ),
        "", "");
    

    //
    // setting the paint
    //
    
    // sets the pan
    DocDeclStr(
        virtual void , SetPen( const wxPen &pen ),
        "", "");
    

    // sets the brush for filling
    DocDeclStr(
        virtual void , SetBrush( const wxBrush &brush ),
        "", "");
    

    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    DocDeclStr(
        virtual void , SetLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
                                               const wxColour&c1, const wxColour&c2),
        "", "");
    

    // sets the brush to a radial gradient originating at (xo,yc) with color oColour and ends on a circle around (xc,yc) 
    // with radius r and color cColour
    DocDeclStr(
        virtual void , SetRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc,
                                               wxDouble radius,
                                               const wxColour &oColour, const wxColour &cColour),
        "", "");
    

    // sets the font
    DocDeclStr(
        virtual void , SetFont( const wxFont &font ),
        "", "");
    
    
    // sets the text color
    DocDeclStr(
        virtual void , SetTextColour( const wxColour &col ),
        "", "");
    

    // strokes along a path with the current pen
    DocDeclStr(
        virtual void , StrokePath( const wxGraphicsPath *path ),
        "", "");
    

    // fills a path with the current brush
    DocDeclStr(
        virtual void , FillPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE ),
        "", "");
    

    // draws a path by first filling and then stroking
    DocDeclStr(
        virtual void , DrawPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE ),
        "", "");
    

    //
    // text
    //
    
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
    

    //
    // image support
    //

    DocDeclStr(
        virtual void , DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "", "");
    

    DocDeclStr(
        virtual void , DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ),
        "", "");
    

    //
    // convenience methods
    //
    
    // strokes a single line
    DocDeclStr(
        virtual void , StrokeLine( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2),
        "", "");

    
    // stroke lines connecting each of the points
    DocDeclAStr(
        virtual void , StrokeLines( size_t points, const wxPoint2D *points_array),
        "StrokeLines(self, List points)",
        "", "");
    

//     // stroke disconnected lines from begin to end points
//     virtual void StrokeLines( size_t n, const wxPoint2D *beginPoints, const wxPoint2D *endPoints);

    %extend {
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

    // draws a polygon
    DocDeclStr(
        virtual void , DrawLines( size_t points, const wxPoint2D *points_array, int fillStyle = wxWINDING_RULE ),
        "", "");
    

    // draws a polygon
    DocDeclStr(
        virtual void , DrawRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "", "");
    

    // draws an ellipse
    DocDeclStr(
        virtual void , DrawEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h),
        "", "");
    

    // draws a rounded rectangle
    DocDeclStr(
        virtual void , DrawRoundedRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h, wxDouble radius),
        "", "");
    
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

