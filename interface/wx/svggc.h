/////////////////////////////////////////////////////////////////////////////
// Name:        svggc.h
// Purpose:     interface of wxSVGGraphicsContext
// Author:      Blake Madden
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSVGGraphicsContext

    wxSVGGraphicsContext is a specialized @ref wxGraphicsContext that allows drawing
    to an SVG file using the @ref wxGraphicsContext API. This context provides
    access to advanced vector features not available in the standard
    @ref wxDC API, such as cubic and quadratic Bézier curves, gradient
    brushes, and alpha blending.

    This context is used when using @ref wxGraphicsContext on a @ref wxSVGFileDC.

    Example of use:
    @code
    wxSVGFileDC dc("output.svg", 800, 600);
    wxGraphicsContext* gc = dc.GetGraphicsContext();
    if ( gc != nullptr )
    {
        gc->SetPen(*wxRED_PEN);
        gc->SetBrush(*wxCYAN_BRUSH);
        gc->DrawRectangle(10, 10, 100, 100);

        // Draw a Bézier curve
        wxGraphicsPath path = gc->CreatePath();
        path.MoveToPoint(150, 50);
        path.AddCurveToPoint(200, 0, 250, 100, 300, 50);
        gc->StrokePath(path);
    }
    @endcode

    @library{wxcore}
    @category{gdi}
    @since 3.3.4
*/
class wxSVGGraphicsContext : public wxGraphicsContext
{
public:
    /** @name State stack */
    ///@{
    /**
        Push the current state (like transformations, clipping region, and quality
        settings) of the context on a stack.
        Multiple balanced calls to PushState() and PopState() can be nested.
    */
    virtual void PushState() override;

    /**
        Pop the last state from the stack and restore the previous state.
    */
    virtual void PopState() override;
    ///@}

    /** @name Clipping */
    ///@{
    /**
        Sets the clipping region to the intersection of the given region
        and the previously set clipping region.
    */
    virtual void Clip(const wxRegion& region) override;

    /**
        Sets the clipping region to the intersection of the given rectangle
        and the previously set clipping region.
    */
    virtual void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

    /**
        Resets the clipping to original shape.
    */
    virtual void ResetClip() override;

    /**
        Returns a bounding box of the current clipping region.
    */
    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) override;
    ///@}

    /**
        This method is not applicable to the SVG renderer and always returns @NULL.
    */
    virtual void* GetNativeContext() override;

    /**
        Sets the antialiasing mode.

        For the SVG renderer, this method maps the @ref wxAntialiasMode to the
        SVG @c shape-rendering attribute. (For example, @c wxANTIALIAS_NONE
        maps to @c wxSVG_SHAPE_RENDERING_CRISP_EDGES.)

        Returns @true.
    */
    virtual bool SetAntialiasMode(wxAntialiasMode antialias) override;

    /**
        Sets the interpolation quality.

        For the SVG renderer, this method maps the @ref wxInterpolationQuality to the
        SVG @c shape-rendering attribute as a quality hint. (For example, @c wxINTERPOLATION_BEST
        maps to @c wxSVG_SHAPE_RENDERING_GEOMETRIC_PRECISION.)

        Returns @true.
    */
    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) override;

    /**
        Sets the composition mode.

        For the SVG renderer, the following modes are supported:
        - @c wxCOMPOSITION_ADD
        - @c wxCOMPOSITION_DIFF
        - @c wxCOMPOSITION_OVER (the default)

        Returns @true if the mode is supported, @false otherwise.
    */
    virtual bool SetCompositionMode(wxCompositionMode op) override;

    /**
        Starts a new layer with the given opacity.
    */
    virtual void BeginLayer(wxDouble opacity) override;

    /**
        Composites back the drawings into the context with the opacity given at
        the BeginLayer() call.
    */
    virtual void EndLayer() override;

    /** @name Transformations */
    ///@{
    /**
        Translates (i.e., moves) the current transformation matrix.
    */
    virtual void Translate(wxDouble dx, wxDouble dy) override;

    /**
        Scales (i.e., shrinks or grows) the current transformation matrix.
    */
    virtual void Scale(wxDouble xScale, wxDouble yScale) override;

    /**
        Rotates the current transformation matrix (in radians).
    */
    virtual void Rotate(wxDouble angle) override;

    /**
        Concatenates the passed-in transform with the current transform of this context.
    */
    virtual void ConcatTransform(const wxGraphicsMatrix& matrix) override;

    /**
        Sets the current transformation matrix of this context.
    */
    virtual void SetTransform(const wxGraphicsMatrix& matrix) override;

    /**
        Gets the current transformation matrix of this context.
    */
    virtual wxGraphicsMatrix GetTransform() const override;
    ///@}

    /** @name Paint state */
    ///@{
    /**
        Sets the pen used for stroking.

        @note Stippled pen styles are not supported by the SVG renderer.
    */
    virtual void SetPen(const wxGraphicsPen& pen) override;

    /**
        Sets the brush for filling paths.

        @note Stippled brush styles are not supported by the SVG renderer.
    */
    virtual void SetBrush(const wxGraphicsBrush& brush) override;

    /**
        Sets the font for drawing text.
    */
    virtual void SetFont(const wxGraphicsFont& font) override;
    ///@}

    /** @name Paths */
    ///@{
    /**
        Strokes the path with the current pen.
    */
    virtual void StrokePath(const wxGraphicsPath& path) override;

    /**
        Fills the path with the current brush.
    */
    virtual void FillPath(const wxGraphicsPath& path,
                          wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;
    ///@}

    /** @name Text */
    ///@{
    /**
        Returns the dimensions of the string using the current font.
    */
    virtual void GetTextExtent(const wxString& text, wxDouble* width, wxDouble* height,
                               wxDouble* descent = nullptr,
                               wxDouble* externalLeading = nullptr) const override;

    /**
        Returns the widths of each character in the string.
    */
    virtual void GetPartialTextExtents(const wxString& text,
                                       wxArrayDouble& widths) const override;
    ///@}

    /** @name Images */
    ///@{
    /**
        Draws the bitmap.
    */
    virtual void DrawBitmap(const wxGraphicsBitmap& bmp,
                            wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

    /**
        @overload
    */
    virtual void DrawBitmap(const wxBitmap& bmp,
                            wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;

    /**
        Draws the icon.
    */
    virtual void DrawIcon(const wxIcon& icon,
                          wxDouble x, wxDouble y, wxDouble w, wxDouble h) override;
    ///@}
};

/**
    @class wxSVGGraphicsRenderer

    The renderer used to create @ref wxSVGGraphicsContext objects.

    This renderer is a singleton and is used by @ref wxGraphicsContext::Create()
    when a @ref wxSVGFileDC is passed to it.

    @library{wxcore}
    @category{gdi}
    @since 3.3.4
*/
class wxSVGGraphicsRenderer : public wxGraphicsRenderer
{
public:
    /**
        Returns the singleton instance of the SVG renderer.
    */
    static wxGraphicsRenderer* Get();

    /** @name Context creation */
    ///@{
    /**
        This method is not supported by the SVG renderer and always returns @NULL.
        SVG contexts are created automatically when calling wxSVGFileDC::GetGraphicsContext().
    */
    virtual wxGraphicsContext* CreateContext(const wxWindowDC& dc) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
        SVG contexts are created automatically when calling wxSVGFileDC::GetGraphicsContext().
    */
    virtual wxGraphicsContext* CreateContext(const wxMemoryDC& dc) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
        SVG contexts are created automatically when calling wxSVGFileDC::GetGraphicsContext().
    */
    virtual wxGraphicsContext* CreateContext(const wxPrinterDC& dc) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateContextFromNativeContext(void* context) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateContextFromNativeWindow(void* window) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateContext(wxWindow* window) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateContextFromImage(wxImage& image) override;

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateMeasuringContext() override;
    ///@}

    /** @name Path and matrix creation */
    ///@{
    /**
        Creates a native graphics path.
    */
    virtual wxGraphicsPath CreatePath() override;

    /**
        Creates a native affine transformation matrix from the passed-in values.
    */
    virtual wxGraphicsMatrix CreateMatrix(wxDouble a = 1.0, wxDouble b = 0.0,
                                          wxDouble c = 0.0, wxDouble d = 1.0,
                                          wxDouble tx = 0.0, wxDouble ty = 0.0) override;
    ///@}

    /** @name Resource creation */
    ///@{
    /**
        Creates a native pen from a @ref wxGraphicsPenInfo.
    */
    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) override;

    /**
        Creates a native brush from a @ref wxBrush.
    */
    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush) override;

    /**
        Creates a native brush with a linear gradient.
    */
    virtual wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) override;

    /**
        Creates a native brush with a radial gradient.
    */
    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& matrix = wxNullGraphicsMatrix) override;

    /**
        Creates a native graphics font from a @ref wxFont and a colour.
    */
    virtual wxGraphicsFont CreateFont(const wxFont& font,
                                      const wxColour& col = *wxBLACK) override;

    /**
        Creates a native graphics font with the given size, face name and flags.
    */
    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) override;

    /**
        Creates a native graphics font from a @ref wxFont at a specific DPI.
    */
    virtual wxGraphicsFont CreateFontAtDPI(const wxFont& font,
                                           const wxRealPoint& dpi,
                                           const wxColour& col = *wxBLACK) override;
    ///@}

    /** @name Bitmap creation */
    ///@{
    /**
        Creates a native graphics bitmap from a @ref wxBitmap.
    */
    virtual wxGraphicsBitmap CreateBitmap(const wxBitmap& bitmap) override;

    /**
        Creates a native graphics bitmap from a @ref wxImage.
    */
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) override;

    /**
        Creates a @ref wxImage from a @ref wxGraphicsBitmap.
    */
    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) override;

    /**
        Creates a native graphics bitmap from a native bitmap handle.
    */
    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap(void* bitmap) override;

    /**
        Creates a native graphics sub-bitmap from an existing bitmap.
    */
    virtual wxGraphicsBitmap CreateSubBitmap(const wxGraphicsBitmap& bitmap,
                                             wxDouble x, wxDouble y,
                                             wxDouble w, wxDouble h) override;
    ///@}

    /**
        Returns the name of the renderer, which is always "svg".
    */
    virtual wxString GetName() const override;

    /**
        Returns the version of the renderer.
    */
    virtual void GetVersion(int* major, int* minor = nullptr, int* micro = nullptr) const override;
};
