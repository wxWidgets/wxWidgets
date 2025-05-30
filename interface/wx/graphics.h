/////////////////////////////////////////////////////////////////////////////
// Name:        graphics.h
// Purpose:     interface of various wxGraphics* classes
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGraphicsPath

    A wxGraphicsPath is a native representation of a geometric path. The
    contents are specific and private to the respective renderer. Instances are
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
        Adds an arc of a circle.

        The circle is defined by the coordinates of its centre (@a x, @a y) or
        @a c and its radius @a r. The arc goes from the starting angle @a
        startAngle to @a endAngle either clockwise or counter-clockwise,
        depending on the value of @a clockwise argument.

        The angles are measured in radians but, contrary to the usual
        mathematical convention, are always @e clockwise from the horizontal
        axis.

        If clockwise and arc @a endAngle is less than @a startAngle, it will be
        progressively increased by 2*pi until it is greater than @a startAngle.
        If counter-clockwise and arc @a endAngle is greater than @a startAngle,
        it will be progressively decreased by 2*pi until it is less than
        @a startAngle.

        If there is a current point set, an initial line segment will be added
        to the path to connect the current point to the beginning of the arc.
    */
    ///@{
    virtual void AddArc(wxDouble x, wxDouble y, wxDouble r,
                        wxDouble startAngle, wxDouble endAngle,
                        bool clockwise);
    void AddArc(const wxPoint2DDouble& c, wxDouble r,
                wxDouble startAngle, wxDouble endAngle, bool clockwise);
    ///@}

    /**
        Adds an arc (of a circle with radius @a r) that is tangent
        to the line connecting the current point and (@a x1, @a y1) and
        to the line connecting (@a x1, @a y1) and (@a x2, @a y2).
        If the current point and the starting point of the arc are different,
        then a straight line connecting these points is also appended.
        If there is no current point before the call to AddArcToPoint(), then
        this function will behave as if preceded by a call to MoveToPoint(0, 0).
        After this call, the current point will be at the ending point
        of the arc.
        @image html drawing-addarctopoint.png
    */
    virtual void AddArcToPoint(wxDouble x1, wxDouble y1, wxDouble x2,
                               wxDouble y2, wxDouble r);

    /**
        Appends a circle around (@a x,@a y) with radius @a r as a new closed
        subpath.
        After this call, the current point will be at (@a x+@a r, @a y).
    */
    virtual void AddCircle(wxDouble x, wxDouble y, wxDouble r);

    /**
        Adds a cubic Bézier curve from the current point, using two control
        points and an end point.
        If there is no current point before the call to AddCurveToPoint(), then
        this function will behave as if preceded by a call to
        MoveToPoint(@a cx1, @a cy1).

        This is useful for pulling a section of a path in two directions,
        providing a smoother bump (or wave) in its appearance compared to a
        quadratic Bézier curve.

        @param cx1
            The x coordinate of the first control point.
        @param cy1
            The y coordinate of the first control point.
        @param cx2
            The x coordinate of the second control point.
        @param cy2
            The y coordinate of the second control point.
        @param x
            The x coordinate of the end point.
        @param y
            The y coordinate of the end point.
    */
    virtual void AddCurveToPoint(wxDouble cx1, wxDouble cy1,
                                 wxDouble cx2, wxDouble cy2,
                                 wxDouble x, wxDouble y);
    /**
        Adds a cubic Bézier curve from the current point, using two control
        points and an end point.
        If there is no current point before the call to AddCurveToPoint(), then
        this function will behave as if preceded by a call to MoveToPoint(@a c1).

        This is useful for pulling a section of a path in two directions,
        providing a smoother bump (or wave) in its appearance compared to a
        quadratic Bézier curve.

        @param c1
            The first control point.
        @param c2
            The second control point.
        @param e
            The end point.
    */
    void AddCurveToPoint(const wxPoint2DDouble& c1,
                         const wxPoint2DDouble& c2,
                         const wxPoint2DDouble& e);

    /**
        Appends an ellipse fitting into the passed-in rectangle as a new
        closed subpath.
        After this call, the current point will be at (@a x+@a w, @a y+@a h/2).
    */
    virtual void AddEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        @overload

        @since 3.3.0
    */
    void AddEllipse(const wxRect2DDouble& rect);

    /**
        Adds a straight line from the current point to (@a x,@a y).
        If current point is not yet set before the call to AddLineToPoint(),
        then this function will behave as MoveToPoint().
    */
    virtual void AddLineToPoint(wxDouble x, wxDouble y);
    /**
        Adds a straight line from the current point to @a p.
        If current point is not yet set before the call to AddLineToPoint(),
        then this function will behave as MoveToPoint().
    */
    void AddLineToPoint(const wxPoint2DDouble& p);

    /**
        Adds another path onto the current path. After this call, the current
        point will be at the added path's current point.
        For Direct2D, the path being appended shouldn't contain
        a started non-empty subpath when this function is called.
    */
    virtual void AddPath(const wxGraphicsPath& path);

    /**
        Adds a quadratic Bézier curve from the current point, using a control
        point and an end point.
        If there is no current point before the call to AddQuadCurveToPoint(),
        then this function will behave as if preceded by a call to
        MoveToPoint(@a cx, @a cy).

        This is useful for pulling a section of a path in one direction,
        providing a sharper bump in its appearance compared to a
        cubic Bézier curve.

        @param cx
            The x coordinate of the control point.
        @param cy
            The y coordinate of the control point.
        @param x
            The x coordinate of the end point.
        @param y
            The y coordinate of the end point.
    */
    virtual void AddQuadCurveToPoint(wxDouble cx, wxDouble cy,
                                     wxDouble x, wxDouble y);

    /**
        @overload

        @since 3.3.0
    */
    void AddQuadCurveToPoint(const wxPoint2DDouble& cp, const wxPoint2DDouble& e);

    /**
        Appends a rectangle as a new closed subpath. After this call
        the current point will be at (@a x, @a y).
    */
    virtual void AddRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        @overload

        @since 3.3.0
    */
    void AddRectangle(const wxRect2DDouble& rect);

    /**
        Appends a rounded rectangle as a new closed subpath.
        If @a radius equals 0, then this function will behave as AddRectangle();
        otherwise, after this call, the current point will be at
        (@a x+@a w, @a y+@a h/2).
    */
    virtual void AddRoundedRectangle(wxDouble x, wxDouble y, wxDouble w,
                                     wxDouble h, wxDouble radius);

    /**
        @overload

        @since 3.3.0
    */
    void AddRoundedRectangle(const wxRect2DDouble& rect, wxDouble radius);

    /**
        Closes the current sub-path. After this call, the current point will be
        at the joined end point of the sub-path.
    */
    virtual void CloseSubpath();

    /**
        @return @true if the point is within the path.
    */
    bool Contains(const wxPoint2DDouble& c,
                  wxPolygonFillMode fillStyle = wxODDEVEN_RULE) const;
    /**
        @return @true if the point is within the path.
    */
    virtual bool Contains(wxDouble x, wxDouble y,
                          wxPolygonFillMode fillStyle = wxODDEVEN_RULE) const;

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
        Returns the native path (@c CGPathRef for Core Graphics, @c Path pointer for
        GDI+ and a @c cairo_path_t pointer for Cairo).
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
        For Direct2D, the current path shouldn't contain
        a started non-empty subpath when this function is called.
    */
    virtual void Transform(const wxGraphicsMatrix& matrix);

    /**
        Gives back the native path returned by GetNativePath() because there
        might be some deallocations necessary (e.g., on Cairo, the native path
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
        @return @false if this object is valid; otherwise, returns @true.
    */
    bool IsNull() const;
};

/**
    Anti-aliasing modes used by wxGraphicsContext::SetAntialiasMode().
*/
enum wxAntialiasMode
{
    /** No anti-aliasing */
    wxANTIALIAS_NONE,

    /** The default anti-aliasing */
    wxANTIALIAS_DEFAULT,
};

/**
    Interpolation quality used by wxGraphicsContext::SetInterpolationQuality().
 */
enum wxInterpolationQuality
{
    /** default interpolation, based on type of context, in general medium quality */
    wxINTERPOLATION_DEFAULT,
    /** no interpolation */
    wxINTERPOLATION_NONE,
    /** fast interpolation, suited for interactivity */
    wxINTERPOLATION_FAST,
    /** better quality */
    wxINTERPOLATION_GOOD,
    /** best quality, not suited for interactivity */
    wxINTERPOLATION_BEST
};

/**
    Compositing is done using Porter-Duff compositions
    (see http://keithp.com/~keithp/porterduff/p253-porter.pdf) with
    wxGraphicsContext::SetCompositionMode().

    The description gives a short equation on how the values of a resulting
    pixel are calculated.
    @e R = Result, @e S = Source, @e D = Destination, colors premultiplied with alpha
    @e Ra, @e Sa, @e Da their alpha components
*/
enum wxCompositionMode
{
    /**
        Indicates invalid or unsupported composition mode.

        This value can't be passed to wxGraphicsContext::SetCompositionMode().

        @since 2.9.2
     */
    wxCOMPOSITION_INVALID = -1,
    wxCOMPOSITION_CLEAR, /**< @e R = 0 */
    wxCOMPOSITION_SOURCE, /**< @e R = S */
    wxCOMPOSITION_OVER, /**< @e R = @e S + @e D*(1 - @e Sa) */
    wxCOMPOSITION_IN, /**< @e R = @e S*@e Da */
    wxCOMPOSITION_OUT, /**< @e R = @e S*(1 - @e Da) */
    wxCOMPOSITION_ATOP, /**< @e R = @e S*@e Da + @e D*(1 - @e Sa) */

    wxCOMPOSITION_DEST, /**< @e R = @e D, essentially a noop */
    wxCOMPOSITION_DEST_OVER, /**< @e R = @e S*(1 - @e Da) + @e D */
    wxCOMPOSITION_DEST_IN, /**< @e R = @e D*@e Sa */
    wxCOMPOSITION_DEST_OUT, /**< @e R = @e D*(1 - @e Sa) */
    wxCOMPOSITION_DEST_ATOP, /**< @e R = @e S*(1 - @e Da) + @e D*@e Sa */
    wxCOMPOSITION_XOR, /**< @e R = @e S*(1 - @e Da) + @e D*(1 - @e Sa) */
    wxCOMPOSITION_ADD, /**< @e R = @e S + @e D */

    /**
        Result is the absolute value of the difference between the source and
        the destination.

        This composition mode is only supported by Cairo and CoreGraphics-based
        implementations, i.e. in wxGTK and wxOSX only (unless Cairo-based
        renderer is explicitly under the other platforms).

        When the source colour is white, this mode can be used to emulate the
        wxINVERT logical function of wxDC; drawing using this mode twice
        restores the original contents.

        @since 3.2.0
     */
    wxCOMPOSITION_DIFF
};

/**
   Used to indicate what kind of gradient is set in a wxGraphicsPenInfo
   object.

   @since 3.1.3
 */
enum wxGradientType {
    /** No gradient */
    wxGRADIENT_NONE,
    /** A gradient that blends two or more colors along an axis.
        Basically, this creates a color gradient from a start an end point. */
    wxGRADIENT_LINEAR,
    /** A gradient that blends two or more colors across an ellipse.
        This creates a color gradient using a starting point and
        a circle's center and radius. */
    wxGRADIENT_RADIAL
};


/**
    Represents a bitmap.

    The objects of this class are not created directly but only via
    wxGraphicsContext or wxGraphicsRenderer CreateBitmap(),
    CreateBitmapFromImage() or CreateSubBitmap() methods. They can subsequently
    be used with wxGraphicsContext::DrawBitmap(). The only other operation is
    testing for the bitmap validity, which can be performed using IsNull()
    inherited from the base class.
 */
class wxGraphicsBitmap : public wxGraphicsObject
{
public:
    /**
        Default constructor creates an invalid bitmap.
     */
    wxGraphicsBitmap();

    /**
        Return the contents of this bitmap as a wxImage.

        Using this method is more efficient than converting a wxGraphicsBitmap to
        a wxBitmap first and then to a wxImage. This can be useful if, for example,
        you want to save a wxGraphicsBitmap in a file format not
        directly supported by wxBitmap.

        Invalid image is returned if the bitmap is invalid.

        @since 2.9.3
     */
    wxImage ConvertToImage() const;

    /**
        Return the pointer to the native bitmap data. (@c CGImageRef for Core Graphics,
        @c cairo_surface_t for Cairo, @c Bitmap* for GDI+.)

        @since 2.9.4
     */
    void* GetNativeBitmap() const;
};

/**
    @class wxGraphicsContext

    A wxGraphicsContext instance is the object that is drawn upon. It is
    created by a renderer using wxGraphicsRenderer::CreateContext(). This can
    be either directly using a renderer instance, or indirectly using the
    static convenience Create() functions of wxGraphicsContext that always
    delegate the task to the default renderer.

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

    @remarks For some renderers (like Direct2D or Cairo), the processing
    of drawing operations may be deferred. For example, Direct2D render
    targets normally build up a batch of rendering commands but defers
    processing them, while Cairo operates on a separate surface.
    To make drawing results visible, you need to update the context's
    content by either calling wxGraphicsContext::Flush()
    or by destroying the context.

    @library{wxcore}
    @category{gdi,dc}

    @see wxGraphicsRenderer::CreateContext(), wxGCDC, wxDC
*/
class wxGraphicsContext : public wxGraphicsObject
{
public:
    /**
        @name Creating a context

        @{
    */

    /**
        Creates a wxGraphicsContext from a wxWindow.

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create(wxWindow* window);

    /**
        Creates a wxGraphicsContext from a wxWindowDC.

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create(const wxWindowDC& windowDC);

    /**
        Creates a wxGraphicsContext from a wxMemoryDC.

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create(const wxMemoryDC& memoryDC);

    /**
        Creates a wxGraphicsContext from a wxPrinterDC. Under GTK+, this will
        only work when using the @c GtkPrint printing backend (which is available
        since GTK+ 2.10).

        @see wxGraphicsRenderer::CreateContext(), @ref overview_unixprinting
    */
    static wxGraphicsContext* Create(const wxPrinterDC& printerDC);

    /**
        Creates a wxGraphicsContext from a wxEnhMetaFileDC.

        This function, as wxEnhMetaFileDC itself, is only available only
        under MSW.

        @see wxGraphicsRenderer::CreateContext()
    */
    static wxGraphicsContext* Create(const wxEnhMetaFileDC& metaFileDC);

    /**
        Creates a wxGraphicsContext from a DC of unknown specific type.

        Creates a wxGraphicsContext if @a dc is a supported type (i.e. has a
        corresponding Create() method, e.g. wxWindowDC or wxMemoryDC).
        Returns @NULL if the DC is unsupported.

        This method is only useful as a helper in generic code that operates
        with wxDC and doesn't known its exact type. Use Create() instead if
        you know what the DC is (e.g., wxWindowDC).

        @see wxGraphicsRenderer::CreateContextFromUnknownDC()

        @since 3.1.1
     */
    static wxGraphicsContext* CreateFromUnknownDC(wxDC& dc);

    /**
        Creates a wxGraphicsContext associated with a wxImage.

        The image specifies the size of the context, as well as whether alpha is
        supported (if wxImage::HasAlpha()) or not and the initial contents of
        the context. The @a image object must have a life time greater than
        that of the new context as the context copies its contents back to the
        image when it is destroyed.

        @since 2.9.3
     */
    static wxGraphicsContext* Create(wxImage& image);

    /**
        Creates a wxGraphicsContext from a native context. This native context
        must be a @c CGContextRef for Core Graphics, a @c Graphics pointer for
        GDI+, or a @c cairo_t pointer for Cairo.

        @see wxGraphicsRenderer::CreateContextFromNativeContext()
    */
    static wxGraphicsContext* CreateFromNative(void* context);

    /**
        Creates a wxGraphicsContext from a native window.

        @see wxGraphicsRenderer::CreateContextFromNativeWindow()
    */
    static wxGraphicsContext* CreateFromNativeWindow(void* window);

    /**
        Creates a wxGraphicsContext from a native DC handle. Windows only.

        @see wxGraphicsRenderer::CreateContextFromNativeHDC()

        @since 3.1.1
    */
    static wxGraphicsContext* CreateFromNativeHDC(WXHDC dc);

    /**
       Create a lightweight context that can be used only for measuring text.
    */
    static wxGraphicsContext* Create();

    /** @}
    */

    /**
        @name Clipping region functions

        @{
    */

    /**
        Resets the clipping to original shape.

        @remarks
        Use GetClipBox() to cache and restore a previous clipping area.
    */
    virtual void ResetClip() = 0;

    /**
        Sets the clipping region to the intersection of the given region
        and the previously set clipping region.
        The clipping region is an area to which drawing is restricted.

        @remarks
        - Clipping region should be given in logical coordinates.

        - Calling this function can only make the clipping region smaller,
        never larger.

        - You need to call ResetClip() first if you want to set the clipping
        region exactly to the region specified.

        - If resulting clipping region is empty, then all drawing upon the context
        is clipped out (all changes made by drawing operations are masked out).
    */
    virtual void Clip(const wxRegion& region) = 0;

    /**
        @overload
    */
    virtual void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h) = 0;

    /**
        @overload

        @since 3.3.0
    */
    void Clip(const wxRect2DDouble& rect);

    /**
        Returns a bounding box of the current clipping region.

        This is useful for rolling back to a previous clipping region.
        For example, if you want to clip to an already clipped shape,
        but then need to revert to the previous clipping area after your
        drawing commands, do the following:
        - Call GetClipBox() to cache the current clipping box.
        - Call Clip() with your new clipping area (which will be the
        intersection of that area and the previous clipping box).
        - Call your rendering commands.
        - Call ResetClip() to remove all clipping.
        - Call Clip() with the rectangle cached from GetClipBox() to
        restore the previous clipping area.

        @remarks
        If clipping region is empty, then an empty rectangle is returned
        (@a x, @a y, @a w, @a h are set to zero).

        @since 3.1.1
    */
    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) = 0;

    /**
        @overload

        @since 3.3.0
    */
    wxRect2DDouble GetClipBox();

    /** @}
    */

    /**
        @name Transformation matrix

        @{
    */

    /**
        Creates a native affine transformation matrix from the passed-in
        values. The default parameters result in an identity matrix.
    */
    virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0,
                                          wxDouble c = 0.0, wxDouble d = 1.0,
                                          wxDouble tx = 0.0,
                                          wxDouble ty = 0.0) const;

    /**
        Creates a native affine transformation matrix from the passed
        generic one.

        @since 2.9.4
    */
    wxGraphicsMatrix CreateMatrix(const wxAffineMatrix2DBase& mat) const;

    /**
        Concatenates the passed-in transform with the current transform of this
        context.
    */
    virtual void ConcatTransform(const wxGraphicsMatrix& matrix) = 0;

    /**
        Gets the current transformation matrix of this context.
    */
    virtual wxGraphicsMatrix GetTransform() const = 0;

    /**
        Rotates the current transformation matrix (in radians).

        This can be useful for applying a tilt to your drawing
        commands or for drawing radial patterns.

        @param angle
            Rotation angle in radians, clockwise.

        @sa ::wxDegToRad()
    */
    virtual void Rotate(wxDouble angle) = 0;

    /**
        Scales (i.e., shrinks or grows) the current transformation matrix.
    */
    virtual void Scale(wxDouble xScale, wxDouble yScale) = 0;

    /**
        Sets the current transformation matrix of this context.
    */
    virtual void SetTransform(const wxGraphicsMatrix& matrix) = 0;

    /**
        Translates (i.e., moves) the current transformation matrix.
    */
    virtual void Translate(wxDouble dx, wxDouble dy) = 0;

    /**
        @overload

        @since 3.3.0
    */
    void Translate(const wxPoint2DDouble& pt);

    /** @}
    */

    /**
        @name Brush and pen functions

        @{
    */

    /**
        Creates a native brush from a wxBrush.
    */
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) const;

    /**
        Creates a native brush with a linear gradient.

        The brush starts at (@a x1, @a y1) and ends at (@a x2, @a y2). Either
        just the start and end gradient colours (@a c1 and @a c2) or full set
        of gradient @a stops can be specified.

        The version taking wxGraphicsGradientStops is new in wxWidgets 2.9.1.

        The @a matrix parameter was added in wxWidgets 3.1.3.
    */
    wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxColour& c1, const wxColour& c2,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) const;

    /**
        @overload
    */
    wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) const;

    /**
        Creates a native brush with a radial gradient.

        The brush originates at (@a startX, @a startY) and ends on a circle around
        (@a endX, @a endY) with the given @a radius.

        The gradient may be specified either by its start and end colours, @a
        oColor and @a cColor, or by a full set of gradient @a stops.

        The version taking wxGraphicsGradientStops is new in wxWidgets 2.9.1.

        The ability to apply a transformation matrix to the gradient was added in 3.1.3.
    */
    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxColour& oColor,
                              const wxColour& cColor,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) const;

    /**
        @overload
    */
    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) = 0;

    /**
        Sets the brush for filling paths.
    */
    void SetBrush(const wxBrush& brush);

    /**
        Sets the brush for filling paths.
    */
    virtual void SetBrush(const wxGraphicsBrush& brush);

    /**
        Creates a native pen from a wxPen.

        Prefer to use the overload taking wxGraphicsPenInfo unless you already
        have a wxPen as constructing one only to pass it to this method is
        wasteful.
    */
    wxGraphicsPen CreatePen(const wxPen& pen) const;

    /**
        Creates a native pen from a wxGraphicsPenInfo.

        @since 3.1.1
    */
    wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) const;

    /**
        Sets the pen used for stroking.
    */
    void SetPen(const wxPen& pen);

    /**
        Sets the pen used for stroking.
    */
    virtual void SetPen(const wxGraphicsPen& pen);

    /** @}
    */

    /**
        @name Drawing functions

        @{
    */

    /**
        Draws the bitmap. In case of a mono bitmap, this is treated as a mask
        and the current brushed is used for filling.
    */
    virtual void DrawBitmap(const wxGraphicsBitmap& bmp,
                            wxDouble x, wxDouble y,
                            wxDouble w, wxDouble h ) = 0;

    /**
        @overload
    */
    virtual void DrawBitmap(const wxBitmap& bmp,
                            wxDouble x, wxDouble y,
                            wxDouble w, wxDouble h) = 0;

    /**
        @overload

        @since 3.3.0
    */
    void DrawBitmap(const wxGraphicsBitmap& bmp, const wxRect2DDouble& rect);

    /**
        Draws an ellipse.
    */
    virtual void DrawEllipse(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        @overload

        @since 3.3.0
    */
    void DrawEllipse(const wxRect2DDouble& rect);

    /**
        Draws the icon.
    */
    virtual void DrawIcon(const wxIcon& icon, wxDouble x, wxDouble y,
                          wxDouble w, wxDouble h) = 0;

    /**
        @overload

        @since 3.3.0
    */
    void DrawIcon(const wxIcon& icon, const wxRect2DDouble& rect);

    /**
        Draws a polygon.
    */
    virtual void DrawLines(size_t n, const wxPoint2DDouble* points,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE);

    /**
        Draws the path by first filling and then stroking.
    */
    virtual void DrawPath(const wxGraphicsPath& path,
                          wxPolygonFillMode fillStyle = wxODDEVEN_RULE);

    /**
        Draws a rectangle.
    */
    virtual void DrawRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        @overload

        @since 3.3.0
    */
    void DrawRectangle(const wxRect2DDouble& rect);

    /**
        Draws a rounded rectangle.
    */
    virtual void DrawRoundedRectangle(wxDouble x, wxDouble y, wxDouble w,
                                      wxDouble h, wxDouble radius);

    /**
        @overload

        @since 3.3.0
    */
    void DrawRoundedRectangle(const wxRect2DDouble& rect, wxDouble radius);

    /**
        Draws text at the defined position.
    */
    void DrawText(const wxString& str, wxDouble x, wxDouble y);

    /**
        @overload

        @since 3.3.0
    */
    void DrawText(const wxString& str, const wxPoint2DDouble& pt);

    /**
        Draws text at the defined position.

        @param str
            The text to draw.
        @param x
            The x coordinate position to draw the text at.
        @param y
            The y coordinate position to draw the text at.
        @param angle
            The angle, in radians, relative to the (default) horizontal
            direction to draw the string.
    */
    void DrawText(const wxString& str, wxDouble x, wxDouble y, wxDouble angle);

    /**
        Draws text at the defined position.

        @param str
            The text to draw.
        @param x
            The x coordinate position to draw the text at.
        @param y
            The y coordinate position to draw the text at.
        @param backgroundBrush
            Brush to fill the text with.
    */
    void DrawText(const wxString& str, wxDouble x, wxDouble y,
                  const wxGraphicsBrush& backgroundBrush);
    /**
        Draws text at the defined position.

        @param str
            The text to draw.
        @param x
            The x coordinate position to draw the text at.
        @param y
            The y coordinate position to draw the text at.
        @param angle
            The angle, in radians, relative to the (default) horizontal
            direction to draw the string.
        @param backgroundBrush
            Brush to fill the text with.
    */
    void DrawText(const wxString& str, wxDouble x, wxDouble y,
                  wxDouble angle, const wxGraphicsBrush& backgroundBrush);

    /**
        Paints a transparent rectangle (only useful for bitmaps or windows).
    */
    virtual void ClearRectangle(wxDouble x, wxDouble y, wxDouble w, wxDouble h);

    /**
        @overload

        @since 3.3.0
    */
    void ClearRectangle(const wxRect2DDouble& rect);

    /**
        Creates a native graphics path which is initially empty.
    */
    wxGraphicsPath CreatePath() const;

    /**
        Fills the path with the current brush.
    */
    virtual void FillPath(const wxGraphicsPath& path,
                          wxPolygonFillMode fillStyle = wxODDEVEN_RULE) = 0;

    /**
        Strokes a single line.
    */
    virtual void StrokeLine(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2);

    /**
        @overload

        @since 3.3.0
    */
    void StrokeLine(const wxPoint2DDouble& pt1, const wxPoint2DDouble& pt2);

    /**
        Stroke disconnected lines from begin to end points, using the
        fastest method available.
    */

    virtual void StrokeLines(size_t n, const wxPoint2DDouble* beginPoints,
                             const wxPoint2DDouble* endPoints);
    /**
        Stroke lines connecting all the points.

        Unlike the other overload of this function, this method draws a single
        polyline and not a number of disconnected lines.
    */
    virtual void StrokeLines(size_t n, const wxPoint2DDouble* points);

    /**
        Strokes along a path with the current pen.
    */
    virtual void StrokePath(const wxGraphicsPath& path) = 0;

    /** @}
    */

    /**
        @name Text functions

        @{
    */

    /**
        Creates a native graphics font from a wxFont and a text colour.

        @remarks
        For Direct2D graphics, fonts can be created from TrueType fonts only.
    */
    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) const;

    /**
        Creates a font object with the specified attributes.

        The use of overload taking wxFont is preferred, see
        wxGraphicsRenderer::CreateFont() for more details.

        @remarks
        For Direct2D, graphics fonts can be created from TrueType fonts only.

        @since 2.9.3
    */
    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) const;

    /**
        Sets the font for drawing text.

        @remarks
        For Direct2D, only TrueType fonts can be used.
    */
    void SetFont(const wxFont& font, const wxColour& colour);

    /**
        Sets the font for drawing text.
    */
    virtual void SetFont(const wxGraphicsFont& font);

    /**
        Fills the @a widths array with the widths from the beginning of
        @a text to the corresponding character of @a text.
    */
    virtual void GetPartialTextExtents(const wxString& text,
                                       wxArrayDouble& widths) const = 0;

    /**
        Gets the dimensions of the string using the currently selected font.

        @param text
            The text string to measure.
        @param[out] width
            Variable to store the total calculated width of the text.
        @param[out] height
            Variable to store the total calculated height of the text.
        @param[out] descent
            Variable to store the dimension from the baseline of the font to
            the bottom of the descender.
        @param[out] externalLeading
            Any extra vertical space added to the font by the font designer
            (usually is zero).
    */
    virtual void GetTextExtent(const wxString& text, wxDouble* width,
                               wxDouble* height, wxDouble* descent,
                               wxDouble* externalLeading) const = 0;

    /** @}
    */

    /**
        @name Page and document start/end functions

        @{
    */

    /**
       Begin a new document (relevant only for printing / pdf / etc.)
       If there is a progress dialog, @a message will be shown.
    */
    virtual bool StartDoc( const wxString& message );

    /**
       Done with that document (relevant only for printing / pdf / etc.)
    */
    virtual void EndDoc();

    /**
       Opens a new page (relevant only for printing / pdf etc.) with the given
       size in points. (If both are @NULL, then the default page size will be used.)
    */
    virtual void StartPage( wxDouble width = 0, wxDouble height = 0 );

    /**
       Ends the current page  (relevant only for printing / pdf / etc.)
    */
    virtual void EndPage();

    /** @}
    */

    /**
        @name Bitmap functions

        @{
    */

    /**
        Creates wxGraphicsBitmap from an existing wxBitmap.

        @return An invalid wxNullGraphicsBitmap on failure.
     */
    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) = 0;

    /**
        Creates wxGraphicsBitmap from an existing wxImage.

        This method is more efficient than converting wxImage to wxBitmap first
        and then calling CreateBitmap(), but otherwise has the same effect.

        @return An invalid wxNullGraphicsBitmap on failure.

        @since 2.9.3
     */
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image);

    /**
        Extracts a sub-bitmap from an existing bitmap.
     */
    virtual wxGraphicsBitmap CreateSubBitmap(const wxGraphicsBitmap& bitmap,
                                             wxDouble x, wxDouble y,
                                             wxDouble w, wxDouble h) = 0;

    /** @}
    */

    /**
        @name Modifying the state

        @{
    */

    /**
        All rendering will be done into a fully transparent, temporary context.
        Layers can be nested by making balanced calls to BeginLayer()/EndLayer().
    */
    virtual void BeginLayer(wxDouble opacity) = 0;

    /**
        Composites back the drawings into the context with the opacity given at
        the BeginLayer() call.
    */
    virtual void EndLayer() = 0;

    /**
       Push the current state (like transformations, clipping region and quality
       settings) of the context on a stack.
       Multiple balanced calls to PushState() and PopState() can be nested.

       @see PopState()
    */
    virtual void PushState() = 0;

    /**
       Sets current state of the context to the state saved by a preceding call
       to PushState() and removes that state from the stack of saved states.

       @see PushState()
    */
    virtual void PopState() = 0;

    /**
       Make sure that the current content of this context is immediately visible.
    */
    virtual void Flush();

    /** @}
    */

    /**
        @name Getting/setting parameters

        @{
    */

    /**
        Returns the native context (@c CGContextRef for Core Graphics, @c Graphics
        pointer for GDI+ and @c cairo_t pointer for Cairo).
    */
    virtual void* GetNativeContext() = 0;

    /**
        Sets the antialiasing mode; returns @true if it supported.
    */
    virtual bool SetAntialiasMode(wxAntialiasMode antialias) = 0;

    /**
        Returns the current shape antialiasing mode.
    */
    virtual wxAntialiasMode GetAntialiasMode() const ;

    /**
        Sets the interpolation quality; returns @true if it is supported.

        @remarks
        Not implemented in the Cairo backend currently.
     */
    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) = 0;

    /**
        Returns the current interpolation quality.
     */
    virtual wxInterpolationQuality GetInterpolationQuality() const;

    /**
        Sets the compositing operator, returns @true if it supported.
    */
    virtual bool SetCompositionMode(wxCompositionMode op) = 0;

    /**
        Returns the current compositing operator.
    */
    virtual wxCompositionMode GetCompositionMode() const;

    /**
       Returns the size of the graphics context in device coordinates.
    */
    void GetSize(wxDouble* width, wxDouble* height) const;

    /**
       Returns the resolution of the graphics context in device points per inch.
    */
    virtual void GetDPI( wxDouble* dpiX, wxDouble* dpiY) const;

    /**
        Returns the associated window, if any.

        If this context was created using Create() overload taking wxWindow or
        wxWindowDC, this method returns the corresponding window. Otherwise,
        returns @NULL.

        @return A possibly @NULL window pointer.

        @since 3.1.2
     */
    wxWindow* GetWindow() const;

    /** @}
    */

    /**
        @name Offset management

        @{
    */

    /**
        Helper to determine if a 0.5 offset should be applied
        for the drawing operation.
    */
    virtual bool ShouldOffset() const;

    /**
        Indicates whether the context should try to offset for pixel
        boundaries. This only makes sense on bitmap devices like screen.
        By default, this is turned off.
    */
    virtual void EnableOffset(bool enable = true);

    void DisableOffset();
    bool OffsetEnabled() const;

    /**
        Convert DPI-independent pixel values to the value in pixels appropriate
        for the graphics context.

        See wxWindow::FromDIP() and wxDC::FromDIP()
        for more info about converting device independent pixel values.

        @since 3.1.7
     */
    wxSize FromDIP(const wxSize& sz) const;

    /// @overload
    wxPoint FromDIP(const wxPoint& pt) const;

    /**
        Convert DPI-independent value in pixels to the value in pixels
        appropriate for the graphics context.

        This is the same as the `FromDIP(const wxSize&)` overload, but assumes
        that the resolution is the same in horizontal and vertical directions.

        @since 3.1.7
     */
    int FromDIP(int d) const;

    /**
        Convert pixel values of the current graphics context to DPI-independent
        pixel values.

        See wxWindow::ToDIP() and wxDC::ToDIP()
        for more info about converting device independent pixel values.

        @since 3.1.7
     */
    wxSize ToDIP(const wxSize& sz) const;

    /// @overload
    wxPoint ToDIP(const wxPoint& pt) const;

    /**
        Convert pixel values of the current graphics context to DPI-independent
        pixel values.

        This is the same as ToDIP(const wxSize&) overload, but assumes
        that the resolution is the same in horizontal and vertical directions.

        @since 3.1.7
     */
    int ToDIP(int d) const;

    /** @}
    */
};

/**
    Represents a single gradient stop in a collection of gradient stops as
    represented by wxGraphicsGradientStops.

    @library{wxcore}
    @category{gdi}

    @since 2.9.1
*/
class wxGraphicsGradientStop
{
public:
    /**
        Creates a stop with the given colour and position.

        @param col The colour of this stop. Note that the alpha component of
            the colour is honoured, thus allowing the background colours to
            partially show through the gradient.
        @param pos The stop position. Must be in the [0, 1] range, with 0 being the
            beginning and 1 the end of the gradient.
    */
    wxGraphicsGradientStop(wxColour col = wxTransparentColour, float pos = 0.);

    /// Return the stop colour.
    const wxColour& GetColour() const;

    /**
        Change the stop colour.

        @param col The new colour.
    */
    void SetColour(const wxColour& col);

    /// Return the stop position.
    float GetPosition() const;

    /**
        Change the stop position.

        @param pos The new position, must always be in [0, 1] range.
    */
    void SetPosition(float pos);
};

/**
    Represents a collection of wxGraphicGradientStop values for use with
    CreateLinearGradientBrush and CreateRadialGradientBrush.

    The stops are maintained in order of position. If two or more stops are
    added with the same position, then the one(s) added later come later.
    This can be useful for producing discontinuities in the colour gradient.

    Notice that this class is write-once; you can't modify the stops once they
    had been added.

    @library{wxcore}
    @category{gdi}

    @since 2.9.1
*/
class wxGraphicsGradientStops
{
public:
    /**
        Initializes the gradient stops with the given boundary colours.

        Creates a wxGraphicsGradientStops instance with start colour given
        by @a startCol and end colour given by @a endCol.
    */
    wxGraphicsGradientStops(wxColour startCol = wxTransparentColour,
                            wxColour endCol = wxTransparentColour);

    /**
        Add a new stop.
    */
    ///@{
    void Add(const wxGraphicsGradientStop& stop);
    void Add(wxColour col, float pos);
    ///@}

    /**
        @return The stop at the given index.

        @param n The index, must be in [0, GetCount()) range.
    */
    wxGraphicsGradientStop Item(unsigned n) const;

    /**
        @return The number of stops.
    */
    size_t GetCount() const;

    /**
        Set the start colour to @a col
    */
    void SetStartColour(wxColour col);

    /**
        @return The start colour.
    */
    wxColour GetStartColour() const;

    /**
        Set the end colour to @a col
    */
    void SetEndColour(wxColour col);

    /**
        @return The end colour.
    */
    wxColour GetEndColour() const;
};

/**
    @class wxGraphicsRenderer

    A wxGraphicsRenderer is the instance corresponding to the rendering engine
    used. There may be multiple instances on a system, if there are different
    rendering engines present, but there is always only one instance per
    engine. This instance is pointed back to by all objects created by it
    (wxGraphicsContext, wxGraphicsPath, etc.) and can be retrieved through their
    wxGraphicsObject::GetRenderer() method. Therefore, you can create an
    additional instance of a path (or other objects) by calling
    wxGraphicsObject::GetRenderer() and then using the appropriate CreateXXX()
    function of that renderer.

    @code
    wxGraphicsPath *path = // from somewhere
    wxGraphicsBrush *brush = path->GetRenderer()->CreateBrush( *wxBLACK_BRUSH );
    @endcode

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsRenderer : public wxObject
{
public:
    /**
        Creates wxGraphicsBitmap from an existing wxBitmap.

        @return An invalid wxNullGraphicsBitmap on failure.
     */
    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) = 0;

    /**
        Creates wxGraphicsBitmap from an existing wxImage.

        This method is more efficient than converting a wxImage to a wxBitmap first
        and then calling CreateBitmap(), but otherwise has the same effect.

        @return An invalid wxNullGraphicsBitmap on failure.

        @since 2.9.3
     */
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) = 0;

    /**
       Creates a wxImage from a wxGraphicsBitmap.

       This method is used by the more convenient wxGraphicsBitmap::ConvertToImage.
    */
    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) = 0;

    /**
        Creates wxGraphicsBitmap from a native bitmap handle.

        @a bitmap meaning is platform-dependent. Currently, it's a GDI+ @c
        Bitmap pointer under MSW, a @c CGImage pointer under macOS or a @c
        cairo_surface_t pointer when using Cairo under any platform.

        Notice that this method takes ownership of @a bitmap, i.e. it will be
        destroyed when the returned wxGraphicsBitmap is.
     */
    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap( void* bitmap ) = 0;

    /**
        Creates a wxGraphicsContext from a wxWindow.
    */
    virtual wxGraphicsContext* CreateContext(wxWindow* window) = 0;

    /**
        Creates a wxGraphicsContext from a wxWindowDC.
    */
    virtual wxGraphicsContext* CreateContext(const wxWindowDC& windowDC) = 0 ;

    /**
        Creates a wxGraphicsContext from a wxMemoryDC.
    */
    virtual wxGraphicsContext* CreateContext(const wxMemoryDC& memoryDC) = 0 ;

    /**
        Creates a wxGraphicsContext from a wxPrinterDC.
        @remarks Not implemented for the Direct2D renderer (on MSW).
    */
    virtual wxGraphicsContext* CreateContext(const wxPrinterDC& printerDC) = 0 ;

    /**
        Creates a wxGraphicsContext from a wxEnhMetaFileDC.

        This function, as wxEnhMetaFileDC itself, is only available only
        under MSW (but not for the Direct2D renderer).
    */
    virtual wxGraphicsContext* CreateContext(const wxEnhMetaFileDC& metaFileDC) = 0;

    /**
        Creates a wxGraphicsContext from a DC of unknown specific type.

        Creates a wxGraphicsContext if @a dc is a supported type (i.e. has a
        corresponding CreateContext() method, e.g. wxWindowDC or wxMemoryDC).

        This method is only useful as a helper in generic code that operates
        with wxDC and doesn't known its exact type. Use the appropriate
        CreateContext() overload instead if you know what the DC is (e.g.,
        wxWindowDC).

        @return @NULL if the DC is unsupported.

        @see wxGraphicsContext::CreateFromUnknownDC()

        @since 3.1.3
     */
     wxGraphicsContext* CreateContextFromUnknownDC(wxDC& dc);

    /**
        Creates a wxGraphicsContext associated with a wxImage.

        This function is used by wxContext::CreateFromImage() and is not
        normally called directly.

        @since 2.9.3
     */
    wxGraphicsContext* CreateContextFromImage(wxImage& image);

    /**
        Creates a native brush from a wxBrush.
    */
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) = 0;

    /**
        Creates a wxGraphicsContext from a native context. This native context
        must be a @c CGContextRef for Core Graphics, a @c Graphics pointer for
        GDI+, an @c ID2D1RenderTarget pointer for Direct2D, a @c cairo_t pointer
        or @c HDC for Cairo on MSW, or a @c cairo_t pointer for Cairo on any other
        platform.
    */
    virtual wxGraphicsContext* CreateContextFromNativeContext(void* context) = 0;

    /**
        Creates a wxGraphicsContext from a native window.
    */
    virtual wxGraphicsContext* CreateContextFromNativeWindow(void* window) = 0;

    /**
        Creates a wxGraphicsContext from a native DC handle. Windows only.

        @since 3.1.1
    */
    static wxGraphicsContext* CreateContextFromNativeHDC(WXHDC dc);

    /**
        Creates a wxGraphicsContext that can be used for measuring texts only.
        No drawing commands are allowed.
    */
    virtual wxGraphicsContext * CreateMeasuringContext() = 0;

    /**
        Creates a native graphics font from a wxFont and a text colour.
    */
    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) = 0;

    /**
        Creates a graphics font with the given characteristics.

        If possible, the CreateFont() overload taking wxFont should be used
        instead. The main advantage of this overload is that it can be used
        without X server connection under Unix when using Cairo.

        @param sizeInPixels
            Height of the font in user space units (normally pixels).
            Notice that this is different from the overload taking wxFont as
            wxFont's size is specified in points.
        @param facename
            The name of the font. The same font name might not be available
            under all platforms so the font name can also be empty to use the
            default platform font.
        @param flags
            Combination of wxFontFlag enum elements. Currently only
            @c wxFONTFLAG_ITALIC and @c wxFONTFLAG_BOLD are supported. By
            default, the normal font version is used.
        @param col
            The font colour, black by default.

        @since 2.9.3
     */
    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) = 0;

    /**
        Creates a native graphics font from a wxFont and a text colour.

        The specified DPI is used to convert the (fractional) wxFont point-size
        to a fractional pixel-size.

        @since 3.1.3
    */
    virtual wxGraphicsFont CreateFontAtDPI(const wxFont& font,
                                           const wxRealPoint& dpi,
                                           const wxColour& col = *wxBLACK) = 0;

    /**
        Creates a native brush with a linear gradient.

        Stops support is new since wxWidgets 2.9.1; previously, only the start
        and end colours could be specified.

        The ability to apply a transformation matrix to the gradient was added in 3.1.3.
    */
    virtual wxGraphicsBrush CreateLinearGradientBrush(wxDouble x1,
                                                      wxDouble y1,
                                                      wxDouble x2,
                                                      wxDouble y2,
                                                      const wxGraphicsGradientStops& stops,
                                                      const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) = 0;

    /**
        Creates a native affine transformation matrix from the passed in
        values. The defaults result in an identity matrix.
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
        Creates a native pen from its description.

        @since 3.1.1
    */
    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) = 0;

    /**
        Creates a native brush with a radial gradient.

        Stops support is new since wxWidgets 2.9.1; previously, only the start
        and end colours could be specified.

        The ability to apply a transformation matrix to the gradient was added in 3.1.3.
    */
    virtual wxGraphicsBrush CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                                                      wxDouble endX, wxDouble endY,
                                                      wxDouble radius,
                                                      const wxGraphicsGradientStops& stops,
                                                      const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) = 0;

    /**
        Extracts a sub-bitmap from an existing bitmap.
     */
    virtual wxGraphicsBitmap CreateSubBitmap(const wxGraphicsBitmap& bitmap,
                                             wxDouble x, wxDouble y,
                                             wxDouble w, wxDouble h) = 0;

    /**
        Returns the name of the technology used by the renderer.

        Currently, this function returns "gdiplus" for the Windows GDI+
        implementation, "direct2d" for Windows Direct2D,
        "cairo" for Cairo and "cg" for macOS CoreGraphics.

        @remarks The string returned by this method is not user-readable and is
        expected to be used internally by the program only.

        @since 3.1.0
     */
    virtual wxString GetName() const = 0;

    /**
        Returns the version major, minor and micro/build of the technology used
        by the renderer.

        Currently, this function returns the OS major and minor versions in
        the parameters with the matching names and sets @a micro to 0 for
        the GDI+ and CoreGraphics engines, which are considered to be parts of
        their respective OS.

        For Cairo, this is the major,minor,micro version of the Cairo library
        which is returned.
     */
    virtual void GetVersion(int* major, int* minor = nullptr, int* micro = nullptr) const = 0;

    /**
        Returns the default renderer on this platform. On macOS, this is the Core
        Graphics (a.k.a. Quartz 2D) renderer, on MSW the GDI+ renderer, and
        on GTK we currently default to the Cairo renderer.
    */
    static wxGraphicsRenderer* GetDefaultRenderer();
    /**
        Returns the Cairo renderer.
    */
    static wxGraphicsRenderer* GetCairoRenderer();
    /**
        Returns the GDI+ renderer (MSW only).
    */
    static wxGraphicsRenderer* GetGDIPlusRenderer();
    /**
        Returns the Direct2D renderer (MSW only).
    */
    static wxGraphicsRenderer* GetDirect2DRenderer();
};



/**
    @class wxGraphicsBrush

    A wxGraphicsBrush is a native representation of a brush. The contents are
    specific and private to the respective renderer. Instances are ref counted
    and can therefore be assigned as usual. The only way to get a valid
    instance is via wxGraphicsContext::CreateBrush() or
    wxGraphicsRenderer::CreateBrush().

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsBrush : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsFont

    A wxGraphicsFont is a native representation of a font. The contents are
    specific and private to the respective renderer. Instances are ref counted
    and can therefore be assigned as usual. The only way to get a valid
    instance is via wxGraphicsContext::CreateFont() or
    wxGraphicsRenderer::CreateFont().

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsFont : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsPenInfo

    This class is a helper used for wxGraphicsPen creation using the named parameter
    idiom: it allows specifying various wxGraphicsPen attributes using the chained
    calls to its clearly named methods instead of passing them in the fixed
    order to wxGraphicsPen constructors.

    Typically, you would use wxGraphicsPenInfo with a wxGraphicsContext.
    For example, to start drawing with a dotted blue pen slightly wider
    than normal you could, write the following:
    @code
    wxGraphicsContext ctx = wxGraphicsContext::Create(dc);

    ctx.SetPen(wxGraphicsPenInfo(*wxBLUE).Width(1.25).Style(wxPENSTYLE_DOT));
    @endcode

    @since 3.1.1
 */
class wxGraphicsPenInfo
{
public:
    /**
        Constructor, which can set the colour, width and style for the pen.
    */
    explicit wxGraphicsPenInfo(const wxColour& colour = wxColour(),
                               wxDouble width = 1.0,
                               wxPenStyle style = wxPENSTYLE_SOLID);

    /**
        Sets the colour for the pen.
    */
    wxGraphicsPenInfo& Colour(const wxColour& col);

    /**
        Sets the line width for the pen.
    */
    wxGraphicsPenInfo& Width(wxDouble width);

    /**
        Sets the style for the pen.
    */
    wxGraphicsPenInfo& Style(wxPenStyle style);

    /**
        Sets the bitmap used for stippling.

        When the pen is used, a repeating pattern of this bitmap will be drawn.
    */
    wxGraphicsPenInfo& Stipple(const wxBitmap& stipple);

    /**
        Sets the dash pattern used to draw the pen's line.
    */
    wxGraphicsPenInfo& Dashes(int nb_dashes, const wxDash *dash);

    /**
        Sets the join for the pen, which is the appearance of
        where two lines meet or overlap.
    */
    wxGraphicsPenInfo& Join(wxPenJoin join);

    /**
        Sets the cap (i.e., the end point) for the pen.
    */
    wxGraphicsPenInfo& Cap(wxPenCap cap);

    /**
        Applies a linear gradient to the pen.

        @param x1
            The x coordinate of the starting point of the gradient.
        @param y1
            The y coordinate of the starting point of the gradient.
        @param x2
            The x coordinate of the ending point of the gradient.
        @param y2
            The y coordinate of the ending point of the gradient.
        @param c1
            The starting colour.
        @param c2
            The ending colour.
        @param matrix
            An optional transformation to apply to the gradient.

        @remarks The starting and ending coordinates define the direction
            of the gradient. Along with north, south, east and west,
            diagonal directions can also be used.
    */
    wxGraphicsPenInfo&
    LinearGradient(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                   const wxColour& c1, const wxColour& c2,
                   const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);

    /**
        Applies a linear gradient to the pen, including discontinuous
        stops in the pattern.

        @param x1
            The x coordinate of the starting point of the gradient.
        @param y1
            The y coordinate of the starting point of the gradient.
        @param x2
            The x coordinate of the ending point of the gradient.
        @param y2
            The y coordinate of the ending point of the gradient.
        @param stops
            A series of stops to include in the pattern.
        @param matrix
            An optional transformation to apply to the gradient.

        @remarks The starting and ending coordinates define the direction
            of the gradient. Along with north, south, east and west,
            diagonal directions can also be used.
    */
    wxGraphicsPenInfo&
    LinearGradient(wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2,
                   const wxGraphicsGradientStops& stops,
                   const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);

    /**
        Applies a radial (i.e., circular) gradient to the pen.

        @param startX
            The x coordinate of the starting point of the gradient.
        @param startY
            The startY coordinate of the starting point of the gradient.
        @param endX
            The endX coordinate of the end of the circle.
        @param endY
            The endY coordinate of the end of the circle.
        @param radius
            The radius of the circle of the gradient (around @a endX and @a endY).
        @param oColor
            The outer colour.
        @param cColor
            The center colour.
        @param matrix
            An optional transformation to apply to the gradient.

        @remarks The starting and ending coordinates define the direction
            of the gradient. Along with north, south, east and west,
            diagonal directions can also be used.
    */
    wxGraphicsPenInfo&
    RadialGradient(wxDouble startX, wxDouble startY,
                   wxDouble endX, wxDouble endY, wxDouble radius,
                   const wxColour& oColor, const wxColour& cColor,
                   const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);

    /**
        Applies a radial (i.e., circular) gradient to the pen, including
        discontinuous stops in the pattern.

        @param startX
            The x coordinate of the starting point of the gradient.
        @param startY
            The startY coordinate of the starting point of the gradient.
        @param endX
            The endX coordinate of the end of the circle.
        @param endY
            The endY coordinate of the end of the circle.
        @param radius
            The radius of the circle of the gradient (around @a endX and @a endY).
        @param stops
            A series of stops to include in the pattern.
        @param matrix
            An optional transformation to apply to the gradient.

        @remarks The starting and ending coordinates define the direction
            of the gradient. Along with north, south, east and west,
            diagonal directions can also be used.
    */
    wxGraphicsPenInfo&
    RadialGradient(wxDouble startX, wxDouble startY,
                   wxDouble endX, wxDouble endY,
                   wxDouble radius, const wxGraphicsGradientStops& stops,
                   const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix);

    /**
        Returns the pen's colour.
    */
    wxColour GetColour() const;
    /**
        Returns the pen's stipple bitmap.
    */
    wxBitmap GetStipple() const;
    /**
        Returns the pen's style.
    */
    wxPenStyle GetStyle() const;
    /**
        Returns the pen's joining method.
    */
    wxPenJoin GetJoin() const;
    /**
        Returns the pen's cap (i.e., end-point style).
    */
    wxPenCap GetCap() const;
    /**
        Returns the pen's dash pattern and
        number of dashes in the pattern.
    */
    int GetDashes(wxDash **ptr);
    /**
        Returns the number of dashes in the
        pen's dash pattern.
    */
    int GetDashCount() const;
    /**
        Returns the pen's dash pattern.
    */
    wxDash* GetDash() const;
    /**
        Returns whether the pen is transparent.
    */
    bool IsTransparent() const;
    /**
        Returns the pen's line width.
    */
    wxDouble GetWidth() const;
    /**
        Returns the pen's gradient type.
    */
    wxGradientType GetGradientType() const;
    /**
        Returns the x coordinate of the starting point
        (if using a gradient).
    */
    wxDouble GetX1() const;
    /**
        Returns the y coordinate of the starting point
        (if using a gradient).
    */
    wxDouble GetY1() const;
    /**
        Returns the x coordinate of the ending point
        (if using a gradient).
    */
    wxDouble GetX2() const;
    /**
        Returns the y coordinate of the ending point
        (if using a gradient).
    */
    wxDouble GetY2() const;
    /**
        Returns the x coordinate of the starting point
        (if using a gradient).
    */
    wxDouble GetStartX() const;
    /**
        Returns the y coordinate of the starting point
        (if using a gradient).
    */
    wxDouble GetStartY() const;
    /**
        Returns the x coordinate of the ending point
        (if using a gradient).
    */
    wxDouble GetEndX() const;
    /**
        Returns the y coordinate of the ending point
        (if using a gradient).
    */
    wxDouble GetEndY() const;
    /**
        Returns the radius of the radial gradient.
    */
    wxDouble GetRadius() const;
    /**
        Returns the stops of the gradient.
    */
    const wxGraphicsGradientStops& GetStops() const;
};



/**
    @class wxGraphicsPen

    A wxGraphicsPen is a native representation of a pen. The contents are
    specific and private to the respective renderer. Instances are ref counted
    and can therefore be assigned as usual. The only way to get a valid
    instance is via wxGraphicsContext::CreatePen() or
    wxGraphicsRenderer::CreatePen().

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsPen : public wxGraphicsObject
{
public:

};



/**
    @class wxGraphicsMatrix

    A wxGraphicsMatrix is a native representation of an affine matrix. The
    contents are specific and private to the respective renderer. Instances are
    ref counted and can therefore be assigned as usual. The only way to get a
    valid instance is via wxGraphicsContext::CreateMatrix() or
    wxGraphicsRenderer::CreateMatrix().

    @library{wxcore}
    @category{gdi}
*/
class wxGraphicsMatrix : public wxGraphicsObject
{
public:
    /**
        Concatenates the matrix passed with the current matrix.
        The effect of the resulting transformation is to first apply
        the transformation in @a t to the coordinates and then apply
        the transformation in the current matrix to the coordinates.

        @code
        matrix = t x matrix
        @endcode

        @param t
            The parameter matrix is the multiplicand.
    */
    virtual void Concat(const wxGraphicsMatrix* t);

    /**
        @overload
    */
    void Concat(const wxGraphicsMatrix& t);

    /**
        Returns the component values of the matrix via the argument pointers.
    */
    virtual void Get(wxDouble* a = nullptr, wxDouble* b = nullptr,
                     wxDouble* c = nullptr, wxDouble* d = nullptr,
                     wxDouble* tx = nullptr, wxDouble* ty = nullptr) const;

    /**
        Returns the native representation of the matrix. For CoreGraphics, this
        is a @c CFAffineMatrix pointer, for GDI+ a @c Matrix Pointer, and for
        Cairo a @c cairo_matrix_t pointer.
    */
    virtual void* GetNativeMatrix() const;

    /**
        Inverts the matrix.
    */
    virtual void Invert();

    /**
        Returns @true if the elements of the transformation matrix are equal.
    */
    virtual bool IsEqual(const wxGraphicsMatrix* t) const;
    /**
        Returns @true if the elements of the transformation matrix are equal.
    */
    bool IsEqual(const wxGraphicsMatrix& t) const;

    /**
        Return @true if this is the identity matrix.
    */
    virtual bool IsIdentity() const;

    /**
        Rotates this matrix clockwise (in radians).

        This can be useful for applying a tilt to your drawing
        commands or for drawing radial patterns.

        @param angle
            Rotation angle in radians, clockwise.

        @sa ::wxDegToRad()
    */
    virtual void Rotate(wxDouble angle);

    /**
        Scales this matrix.
    */
    virtual void Scale(wxDouble xScale, wxDouble yScale);

    /**
        Sets the matrix to the respective values (default values are the
        identity matrix).
    */
    virtual void Set(wxDouble a = 1.0, wxDouble b = 0.0, wxDouble c = 0.0,
                     wxDouble d = 1.0, wxDouble tx = 0.0, wxDouble ty = 0.0);

    /**
        Applies this matrix to a distance (i.e., performs all transforms except
        translations).
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

    /**
        @overload

        @since 3.3.0
    */
    void Translate(const wxPoint2DDouble& pt);
};

/// An empty wxGraphicsPen object.
const wxGraphicsPen     wxNullGraphicsPen;
/// An empty wxGraphicsBrush object.
const wxGraphicsBrush   wxNullGraphicsBrush;
/// An empty wxGraphicsFont object.
const wxGraphicsFont    wxNullGraphicsFont;
/// An empty wxGraphicsBitmap object.
const wxGraphicsBitmap  wxNullGraphicsBitmap;
/// An empty wxGraphicsMatrix object.
const wxGraphicsMatrix  wxNullGraphicsMatrix;
/// An empty wxGraphicsPath object.
const wxGraphicsPath    wxNullGraphicsPath;
