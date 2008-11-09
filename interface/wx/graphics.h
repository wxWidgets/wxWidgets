/////////////////////////////////////////////////////////////////////////////
// Name:        graphics.h
// Purpose:     interface of wxGraphicsPath
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGraphicsPath

    A wxGraphicsPath is a native representation of a geometric path. The
    contents are specific an private to the respective renderer. Instances are
    reference counted and can therefore be assigned as usual. The only way to
    get a valid instance is by using wxGraphicsContext::CreatePath() or
    wxGraphicsRenderer::CreatePath().

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsPath : public wxGraphicsObject
{
public:
    /**
        Adds an arc of a circle centering at (@a x,@a y) with radius (@a r)
        from @a startAngle to @a endAngle.
    */
    virtual void AddArc(wxDouble x, wxDouble y, wxDouble r,
                        wxDouble startAngle, wxDouble endAngle,
                        bool clockwise);
    /**
        Adds an arc of a circle centering at @a c with radius (@a r)
        from @a startAngle to @a endAngle.
    */
    void AddArc(const wxPoint2DDouble& c, wxDouble r,
                wxDouble startAngle, wxDouble endAngle, bool clockwise);

    /**
        Appends a an arc to two tangents connecting (current) to (@a x1,@a y1)
        and (@a x1,@a y1) to (@a x2,@a y2), also a straight line from (current)
        to (@a x1,@a y1).
    */
    virtual void AddArcToPoint(wxDouble x1, wxDouble y1, wxDouble x2,
                               wxDouble y2, wxDouble r);

    /**
        Appends a circle around (@a x,@a y) with radius @a r as a new closed
        subpath.
    */
    virtual void AddCircle(wxDouble x, wxDouble y, wxDouble r);

    /**
        Adds a cubic bezier curve from the current point, using two control
        points and an end point.
    */
    virtual void AddCurveToPoint(wxDouble cx1, wxDouble cy1,
                                 wxDouble cx2, wxDouble cy2,
                                 wxDouble x, wxDouble y);
    /**
        Adds a cubic bezier curve from the current point, using two control
        points and an end point.
    */
    void AddCurveToPoint(const wxPoint2DDouble& c1,
                         const wxPoint2DDouble& c2,
                         const wxPoint2DDouble& e);

    /**
        Appends an ellipse fitting into the passed in rectangle.
    */
    virtual void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        Adds a straight line from the current point to (@a x,@a y).
    */
    virtual void AddLineToPoint(wxDouble x, wxDouble y);
    /**
        Adds a straight line from the current point to @a p.
    */
    void AddLineToPoint(const wxPoint2DDouble& p);

    /**
        Adds another path.
    */
    virtual void AddPath(const wxGraphicsPath& path);

    /**
        Adds a quadratic bezier curve from the current point, using a control
        point and an end point.
    */
    virtual void AddQuadCurveToPoint(wxDouble cx, wxDouble cy,
                                     wxDouble x, wxDouble y);

    /**
        Appends a rectangle as a new closed subpath.
    */
    virtual void AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        Appends a rounded rectangle as a new closed subpath.
    */
    virtual void AddRoundedRectangle(wxDouble x, wxDouble y, wxDouble w,
                                     wxDouble h, wxDouble radius);

    /**
        Closes the current sub-path.
    */
    virtual void CloseSubpath();

    /**
        @return @true if the point is within the path.
    */
    bool Contains(const wxPoint2DDouble& c,
                  int fillStyle = wxODDEVEN_RULE) const;
    /**
        @return @true if the point is within the path.
    */
    virtual bool Contains(wxDouble x, wxDouble y,
                          int fillStyle = wxODDEVEN_RULE) const;

    /**
        Gets the bounding box enclosing all points (possibly including control
        points).
    */
    wxRect2DDouble GetBox() const;
    /**
        Gets the bounding box enclosing all points (possibly including control
        points).
    */
    virtual void GetBox(wxDouble* x, wxDouble* y,
                        wxDouble* w, wxDouble* h) const;

    /**
        Gets the last point of the current path, (0,0) if not yet set.
    */
    virtual void GetCurrentPoint(wxDouble* x, wxDouble* y) const;
    /**
        Gets the last point of the current path, (0,0) if not yet set.
    */
    wxPoint2DDouble GetCurrentPoint() const;

    /**
        Returns the native path (CGPathRef for Core Graphics, Path pointer for
        GDIPlus and a cairo_path_t pointer for cairo).
    */
    virtual void* GetNativePath() const;

    /**
        Begins a new subpath at (@a x,@a y).
    */
    virtual void MoveToPoint(wxDouble x, wxDouble y);
    /**
        Begins a new subpath at @a p.
    */
    void MoveToPoint(const wxPoint2DDouble& p);

    /**
        Transforms each point of this path by the matrix.
    */
    virtual void Transform(const wxGraphicsMatrix& matrix);

    /**
        Gives back the native path returned by GetNativePath() because there
        might be some deallocations necessary (e.g. on cairo the native path
        returned by GetNativePath() is newly allocated each time).
    */
    virtual void UnGetNativePath(void* p) const;
};



/**
    @class wxGraphicsObject

    This class is the superclass of native graphics objects like pens etc. It
    allows reference counting. Not instantiated by user code.

    @library{wxcore}
    @category{gdi}

    @see wxGraphicsBrush, wxGraphicsPen, wxGraphicsMatrix, wxGraphicsPath
*/
class wxGraphicsObject : public wxObject
{
public:
    /**
        Returns the renderer that was used to create this instance, or @NULL
        if it has not been initialized yet.
    */
    wxGraphicsRenderer* GetRenderer() const;

    /**
        @return @false if this object is valid, otherwise returns @true.
    */
    bool IsNull() const;
};



/**
    @class wxGraphicsContext

    A wxGraphicsContext instance is the object that is drawn upon. It is created by
    a renderer using wxGraphicsRenderer::CreateContext(). This can be either directly
    using a renderer instance, or indirectly using the static convenience Create()
    functions of wxGraphicsContext that always delegate the task to the default renderer.

    @code
    void MyCanvas::OnPaint(wxPaintEvent &event)
    {
        // Create paint DC
        wxPaintDC dc(this);

        // Create graphics context from it
        wxGraphicsContext *gc = wxGraphicsContext::Create( dc );

        if (gc)
        {
            // make a path that contains a circle and some lines
            gc->SetPen( *wxRED_PEN );
            wxGraphicsPath path = gc->CreatePath();
            path.AddCircle( 50.0, 50.0, 50.0 );
            path.MoveToPoint(0.0, 50.0);
            path.AddLineToPoint(100.0, 50.0);
            path.MoveToPoint(50.0, 0.0);
            path.AddLineToPoint(50.0, 100.0 );
            path.CloseSubpath();
            path.AddRectangle(25.0, 25.0, 50.0, 50.0);

            gc->StrokePath(path);

            delete gc;
        }
    }
    @endcode


    @library{wxcore}
    @category{gdi,dc}

    @see wxGraphicsRenderer::CreateContext(), wxGCDC, wxDC
*/
class wxGraphicsContext : public wxGraphicsObject
{
public:
    /**
        Creates a wxGraphicsContext from a wxWindow.

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create( wxWindow* window ) ;

    /**
        Creates a wxGraphicsContext from a wxWindowDC

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create( const wxWindowDC& dc) ;

    /**
        Creates a wxGraphicsContext from a wxMemoryDC

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext * Create( const wxMemoryDC& dc) ;

    /**
        Creates a wxGraphicsContext from a wxPrinterDC. Under
        GTK+, this will only work when using the GtkPrint
        printing backend which is available since GTK+ 2.10.

        @see wxGraphicsRenderer::CreateContext(), @ref overview_unixprinting "Printing under Unix"
    */
    static wxGraphicsContext * Create( const wxPrinterDC& dc) ;

    /**
        Clips drawings to the region
    */
    virtual void Clip(const wxRegion& region) = 0;

    /**
        Clips drawings to the rectangle.
    */
    virtual void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h) = 0;

    /**
        Concatenates the passed in transform with the current transform of this context
    */
    virtual void ConcatTransform(const wxGraphicsMatrix& matrix) = 0;


    /**
        Creates a native brush from a wxBrush.
    */
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) const;

    /**
        Creates a native graphics font from a wxFont and a text colour.
    */
    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) const;

    /**
        Creates a wxGraphicsContext from a native context. This native context must be
        eg a CGContextRef for Core Graphics, a Graphics pointer for GDIPlus or a
        cairo_t pointer for cairo.

        @see wxGraphicsRenderer:: CreateContextFromNativeContext
    */
    static wxGraphicsContext* CreateFromNative(void* context);

    /**
        Creates a wxGraphicsContext from a native window.

        @see wxGraphicsRenderer:: CreateContextFromNativeWindow
    */
    static wxGraphicsContext* CreateFromNativeWindow(void* window);

    /**
        Creates a native brush, having a linear gradient, starting at (x1,y1) with
        color c1 to (x2,y2) with color c2
    */
    virtual wxGraphicsBrush CreateLinearGradientBrush(wxDouble x1,
                                                      wxDouble y1,
                                                      wxDouble x2,
                                                      wxDouble y2,
                                                      const wxColour& c1,
                                                      const wxColour& c2) const;

    /**
        Creates a native affine transformation matrix from the passed in values. The
        defaults result in an identity matrix.
    */
    virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0,
                                          wxDouble c = 0.0, wxDouble d = 1.0,
                                          wxDouble tx = 0.0,
                                          wxDouble ty = 0.0) const;

    /**
        Creates a native graphics path which is initially empty.
    */
    wxGraphicsPath CreatePath() const;

    /**
        Creates a native pen from a wxPen.
    */
    virtual wxGraphicsPen CreatePen(const wxPen& pen) const;

    /**
        Creates a native brush, having a radial gradient originating at (xo,yc) with
        color oColour and ends on a circle around (xc,yc) with radius r and color cColour
    */
    virtual wxGraphicsBrush CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                                                      wxDouble xc, wxDouble yc,
                                                      wxDouble radius,
                                                      const wxColour& oColor,
                                                      const wxColour& cColor) const;

    /**
        Draws the bitmap. In case of a mono bitmap, this is treated as a mask and the
        current brushed is used for filling.
    */
    virtual void DrawBitmap(const wxBitmap& bmp, wxDouble x, wxDouble y,
                            wxDouble w, wxDouble h) = 0;

    /**
        Draws an ellipse.
    */
    virtual void DrawEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        Draws the icon.
    */
    virtual void DrawIcon(const wxIcon& icon, wxDouble x, wxDouble y,
                          wxDouble w, wxDouble h) = 0;

    /**
        Draws a polygon.
    */
    virtual void DrawLines(size_t n, const wxPoint2DDouble* points,
                           int fillStyle = wxODDEVEN_RULE);

    /**
        Draws the path by first filling and then stroking.
    */
    virtual void DrawPath(const wxGraphicsPath& path,
                          int fillStyle = wxODDEVEN_RULE);

    /**
        Draws a rectangle.
    */
    virtual void DrawRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        Draws a rounded rectangle.
    */
    virtual void DrawRoundedRectangle(wxDouble x, wxDouble y, wxDouble w,
                                      wxDouble h, wxDouble radius);

    //@{
    /**
        Draws a text at the defined position, at the given angle.
    */
    void DrawText(const wxString& str, wxDouble x, wxDouble y,
                  wxDouble angle);
    void DrawText(const wxString& str, wxDouble x, wxDouble y);
    //@}

    /**
        Fills the path with the current brush.
    */
    virtual void FillPath(const wxGraphicsPath& path,
                          int fillStyle = wxODDEVEN_RULE) = 0;

    /**
        Returns the native context (CGContextRef for Core Graphics, Graphics pointer
        for GDIPlus and cairo_t pointer for cairo).
    */
    virtual void* GetNativeContext() = 0;

    /**
        Fills the @a widths array with the widths from the beginning of
        @a text to the corresponding character of @e text.
    */
    virtual void GetPartialTextExtents(const wxString& text,
                                       wxArrayDouble& widths) const = 0;

    /**
        Gets the dimensions of the string using the currently selected font.
        @e string is the text string to measure, @e w and @e h are
        the total width and height respectively, @a descent is the
        dimension from the baseline of the font to the bottom of the
        descender, and @a externalLeading is any extra vertical space added
        to the font by the font designer (usually is zero).
    */
    virtual void GetTextExtent(const wxString& text, wxDouble* width,
                               wxDouble* height, wxDouble* descent,
                               wxDouble* externalLeading) const = 0;

    /**
        Gets the current transformation matrix of this context.
    */
    virtual wxGraphicsMatrix GetTransform() const = 0;

    /**
        Resets the clipping to original shape.
    */
    virtual void ResetClip() = 0;

    /**
        Rotates the current transformation matrix (radians),
    */
    virtual void Rotate(wxDouble angle) = 0;

    /**
        Scales the current transformation matrix.
    */
    virtual void Scale(wxDouble xScale, wxDouble yScale) = 0;

    //@{
    /**
        Sets the brush for filling paths.
    */
    void SetBrush(const wxBrush& brush);
    void SetBrush(const wxGraphicsBrush& brush);
    //@}

    //@{
    /**
        Sets the font for drawing text.
    */
    void SetFont(const wxFont& font, const wxColour& colour);
    void SetFont(const wxGraphicsFont& font);
    //@}

    //@{
    /**
        Sets the pen used for stroking.
    */
    void SetPen(const wxGraphicsPen& pen);
    void SetPen(const wxPen& pen);
    //@}

    /**
        Sets the current transformation matrix of this context
    */
    virtual void SetTransform(const wxGraphicsMatrix& matrix) = 0;

    /**
        Strokes a single line.
    */
    virtual void StrokeLine(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2);

    //@{
    /**
        Stroke disconnected lines from begin to end points, fastest method available
        for this purpose.
    */
    void StrokeLines(size_t n, const wxPoint2DDouble* beginPoints,
                     const wxPoint2DDouble* endPoints);
    void StrokeLines(size_t n, const wxPoint2DDouble* points);
    //@}

    /**
        Strokes along a path with the current pen.
    */
    virtual void StrokePath(const wxGraphicsPath& path) = 0;

    /**
        Translates the current transformation matrix.
    */
    virtual void Translate(wxDouble dx, wxDouble dy) = 0;
};



/**
    @class wxGraphicsRenderer

    A wxGraphicsRenderer is the instance corresponding to the rendering engine
    used. There may be multiple instances on a system, if there are different
    rendering engines present, but there is always only one instance per engine.
    This instance is pointed back to by all objects created by it (wxGraphicsContext,
    wxGraphicsPath etc) and can be retrieved through their wxGraphicsObject::GetRenderer()
    method. Therefore you can create an additional instance of a path etc. by calling
    wxGraphicsObject::GetRenderer() and then using the appropriate CreateXXX function
    of that renderer.

    @code
    wxGraphicsPath *path = // from somewhere
    wxGraphicsBrush *brush = path->GetRenderer()->CreateBrush( *wxBLACK_BRUSH );
    @endcode

    @library{wxcore}
    @category{FIXME}
*/
class wxGraphicsRenderer : public wxObject
{
public:
    /**
        Creates a wxGraphicsContext from a wxWindow.
    */
    virtual wxGraphicsContext* CreateContext(wxWindow* window) = 0;

    /**
        Creates a wxGraphicsContext from a wxWindowDC
    */
    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) = 0 ;

    /**
        Creates a wxGraphicsContext from a wxMemoryDC
    */
    virtual wxGraphicsContext * CreateContext( const wxMemoryDC& dc) = 0 ;

    /**
        Creates a wxGraphicsContext from a wxPrinterDC
    */
    virtual wxGraphicsContext * CreateContext( const wxPrinterDC& dc) = 0 ;

    /**
        Creates a native brush from a wxBrush.
    */
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) = 0;


    /**
        Creates a wxGraphicsContext from a native context. This native context must be
        eg a CGContextRef for Core Graphics, a Graphics pointer for GDIPlus or a cairo_t
        pointer for cairo.
    */
    virtual wxGraphicsContext* CreateContextFromNativeContext(void* context) = 0;

    /**
        Creates a wxGraphicsContext from a native window.
    */
    virtual wxGraphicsContext* CreateContextFromNativeWindow(void* window) = 0;

    /**
        Creates a native graphics font from a wxFont and a text colour.
    */
    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) = 0;

    /**
        Creates a native brush, having a linear gradient, starting at (x1,y1) with
        color c1 to (x2,y2) with color c2
    */
    wxGraphicsBrush CreateLinearGradientBrush(wxDouble x1,
                                              wxDouble y1,
                                              wxDouble x2,
                                              wxDouble y2,
                                              const wxColour& c1,
                                              const wxColour& c2) = 0;

    /**
        Creates a native affine transformation matrix from the passed in values. The
        defaults result in an identity matrix.
    */
    virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0,
                                          wxDouble c = 0.0, wxDouble d = 1.0,
                                          wxDouble tx = 0.0,
                                          wxDouble ty = 0.0) = 0;

    /**
        Creates a native graphics path which is initially empty.
    */
    virtual wxGraphicsPath CreatePath() = 0;

    /**
        Creates a native pen from a wxPen.
    */
    virtual wxGraphicsPen CreatePen(const wxPen& pen) = 0;

    /**
        Creates a native brush, having a radial gradient originating at (xo,yc) with
        color oColour and ends on a circle around (xc,yc) with radius r and color cColour
    */
    virtual wxGraphicsBrush CreateRadialGradientBrush(wxDouble xo, wxDouble yo,
                                                      wxDouble xc, wxDouble yc,
                                                      wxDouble radius,
                                                      const wxColour& oColour,
                                                      const wxColour& cColour) = 0;

    /**
        Returns the default renderer on this platform. On OS X this is the Core
        Graphics (a.k.a. Quartz 2D) renderer, on MSW the GDIPlus renderer, and on GTK we currently default to the cairo renderer.
    */
    static wxGraphicsRenderer* GetDefaultRenderer();
};



/**
    @class wxGraphicsBrush

    A wxGraphicsBrush is a native representation of a brush. The contents
    are specific and private to the respective renderer. Instances are ref counted and can
    therefore be assigned as usual. The only way to get a valid instance is via
    wxGraphicsContext::CreateBrush or wxGraphicsRenderer::CreateBrush.

    @library{wxcore}
    @category{FIXME}
*/
class wxGraphicsBrush : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsFont

    A wxGraphicsFont is a native representation of a font. The contents
    are specific and private to the respective renderer. Instances are ref counted and can
    therefore be assigned as usual. The only way to get a valid instance is via
    wxGraphicsContext::CreateFont or wxGraphicsRenderer::CreateFont.

    @library{wxcore}
    @category{FIXME}
*/
class wxGraphicsFont : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsPen

    A wxGraphicsPen is a native representation of a pen. The contents
    are specific and private to the respective renderer. Instances are ref counted and can
    therefore be assigned as usual. The only way to get a valid instance is via
    wxGraphicsContext::CreatePen or wxGraphicsRenderer::CreatePen.

    @library{wxcore}
    @category{FIXME}
*/
class wxGraphicsPen : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsMatrix

    A wxGraphicsMatrix is a native representation of an affine matrix. The contents
    are specific and private to the respective renderer. Instances are ref counted and can
    therefore be assigned as usual. The only way to get a valid instance is via
    wxGraphicsContext::CreateMatrix or wxGraphicsRenderer::CreateMatrix.

    @library{wxcore}
    @category{FIXME}
*/
class wxGraphicsMatrix : public wxGraphicsObject
{
public:
    //@{
    /**

    */
    void Concat(const wxGraphicsMatrix* t);
    void Concat(const wxGraphicsMatrix& t);
    //@}

    /**
        Returns the component values of the matrix via the argument pointers.
    */
    virtual void Get(wxDouble* a = NULL, wxDouble* b = NULL, wxDouble* c = NULL,
                     wxDouble* d = NULL, wxDouble* tx = NULL,
                     wxDouble* ty = NULL) const;

    /**
        Returns the native representation of the matrix. For CoreGraphics this is a
        CFAffineMatrix pointer. For GDIPlus a Matrix Pointer and for Cairo a cairo_matrix_t pointer.
    */
    virtual void* GetNativeMatrix() const;

    /**
        Inverts the matrix.
    */
    virtual void Invert();

    /**
        Returns @true if the elements of the transformation matrix are equal.
    */
    bool IsEqual(const wxGraphicsMatrix& t) const;

    /**
        Return @true if this is the identity matrix.
    */
    virtual bool IsIdentity() const;

    /**
        Rotates this matrix (radians).
    */
    virtual void Rotate(wxDouble angle);

    /**
        Scales this matrix.
    */
    virtual void Scale(wxDouble xScale, wxDouble yScale);

    /**
        Sets the matrix to the respective values (default values are the identity
        matrix)
    */
    virtual void Set(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0,
                     wxDouble d = 1.0, wxDouble tx = 0.0, wxDouble ty = 0.0);

    /**
        Applies this matrix to a distance (ie. performs all transforms except
        translations)
    */
    virtual void TransformDistance(wxDouble* dx, wxDouble* dy) const;

    /**
        Applies this matrix to a point.
    */
    virtual void TransformPoint(wxDouble* x, wxDouble* y) const;

    /**
        Translates this matrix.
    */
    virtual void Translate(wxDouble dx, wxDouble dy);
};
