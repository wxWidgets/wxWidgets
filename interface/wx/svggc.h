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
    wxSVGGraphicsContext gc("output.svg", 800, 600);
    gc.SetPen(*wxRED_PEN);
    gc.SetBrush(*wxCYAN_BRUSH);
    gc.DrawRectangle(10, 10, 100, 100);

    // Draw a Bézier curve
    wxGraphicsPath path = gc.CreatePath();
    path.MoveToPoint(150, 50);
    path.AddCurveToPoint(200, 0, 250, 100, 300, 50);
    gc.StrokePath(path);
    @endcode

    @library{wxcore}
    @category{gdi}
    @since 3.3.3
*/
class wxSVGGraphicsContext : public wxGraphicsContext
{
public:
    /**
        Constructor.

        The DC must outlive the context.

        @since 3.3.3
    */
    explicit wxSVGGraphicsContext(wxSVGFileDC& dc);

    /**
        Constructor for creating an SVG file directly.

        @since 3.3.3
    */
    explicit wxSVGGraphicsContext(const wxString& filename,
                         int width = 320,
                         int height = 240,
                         double dpi = wxSVG_DEFAULT_DPI,
                         const wxString& title = wxString());

    /**
        Constructor for creating an SVG file directly.

        @since 3.3.3
    */
    wxSVGGraphicsContext(const wxSize& size,
                         const wxString& filename = wxString(),
                         const wxString& title = wxString(),
                         double dpi = wxSVG_DEFAULT_DPI);

    /**
        Creates a graphics context that draws into the given SVG DC.

        Equivalent to calling @ref wxSVGFileDC::GetGraphicsContext().
        The DC must outlive the returned context.

        Example:
        @code
        wxSVGFileDC dc("output.svg", 800, 600);
        wxGraphicsContext* gc = wxSVGGraphicsContext::Create(dc);
        @endcode

        @since 3.3.3
    */
    static wxGraphicsContext* Create(wxSVGFileDC& dc);

    /**
        Returns the SVG document as a string.

        @since 3.3.3
    */
    wxString GetSVGDocument() const;

    /**
        Writes the document to the configured filename.

        @return @true if the file was written successfully, @false otherwise.

        @since 3.3.3
    */
    bool Save();

    /**
        Sets a handler for bitmap processing (e.g., embedding).
        Takes ownership of the handler.

        @since 3.3.3
    */
    void SetBitmapHandler(wxSVGBitmapHandler* handler);

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

    ///@}
};

/**
    @class wxSVGGraphicsRenderer

    The renderer used to create @ref wxSVGGraphicsContext objects.

    This renderer is a singleton and is used by @ref wxGraphicsContext::Create()
    when a @ref wxSVGFileDC is passed to it.

    @library{wxcore}
    @category{gdi}
    @since 3.3.3
*/
class wxSVGGraphicsRenderer : public wxGraphicsRenderer
{
public:
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
        Creates a graphics context that draws into the given SVG DC.
        The DC must outlive the returned context.

        @since 3.3.3
    */
    wxGraphicsContext* CreateContext(wxSVGFileDC& dc);

    /**
        Creates a graphics context that draws into the given file.

        @since 3.3.3
    */
    wxGraphicsContext* CreateContext(const wxString& filename,
                                     int width = 320,
                                     int height = 240,
                                     double dpi = wxSVG_DEFAULT_DPI,
                                     const wxString& title = wxString());

    /**
        Creates a graphics context that draws into the given file.

        @since 3.3.3
    */
    wxGraphicsContext* CreateContext(const wxSize& size,
                                     const wxString& filename = wxString(),
                                     const wxString& title = wxString(),
                                     double dpi = wxSVG_DEFAULT_DPI);

    /**
        This method is not supported by the SVG renderer and always returns @NULL.
    */
    virtual wxGraphicsContext* CreateMeasuringContext() override;
    ///@}

    /**
        Returns the name of the renderer, which is always "svg".
    */
    virtual wxString GetName() const override;

    /**
        Returns the version of the SVG specification targeted by this renderer (currently 1.0).
    */
    virtual void GetVersion(int* major, int* minor = nullptr, int* micro = nullptr) const override;
};
