/////////////////////////////////////////////////////////////////////////////
// Name:        dcsvg.h
// Purpose:     interface of wxSVGFileDC
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    SVG shape rendering mode.

    These options represent the values defined in the SVG specification:
    https://svgwg.org/svg2-draft/painting.html#ShapeRenderingProperty
*/
enum wxSVGShapeRenderingMode
{
    wxSVG_SHAPE_RENDERING_AUTO = 0,
    wxSVG_SHAPE_RENDERING_OPTIMIZE_SPEED,
    wxSVG_SHAPE_RENDERING_CRISP_EDGES,
    wxSVG_SHAPE_RENDERING_GEOMETRIC_PRECISION,

    wxSVG_SHAPE_RENDERING_OPTIMISE_SPEED = wxSVG_SHAPE_RENDERING_OPTIMIZE_SPEED
};

/**
    Value indicating the default dpi resolution of wxSVGFileDC.

    @since 3.3.2
 */
constexpr double wxSVG_DEFAULT_DPI = 72.0;


/**
    @class wxSVGAttributes

    Helper class for building a set of SVG and ARIA attributes to attach to
    an accessible group in the output of wxSVGFileDC.

    ARIA (Accessible Rich Internet Applications) is a W3C specification of
    attributes that describe the content to assistive technology such as screen readers.

    The class exposes a chainable builder API so multiple attributes can be
    specified inline. The resulting object is passed to
    wxSVGFileDC::BeginAccessibleGroup() or to the wxSVGAccessibleGroup RAII
    helper.

    Example:
    @code
    dc.BeginAccessibleGroup(
        wxSVGAttributes().Role("img").AriaLabel("Quarterly sales"),
        "Quarterly sales",
        "Bar chart showing revenue for Q1-Q4 2026.");
    // ... drawing commands ...
    dc.EndAccessibleGroup();
    @endcode

    @library{wxcore}
    @category{dc}
    @since 3.3.3
*/
class wxSVGAttributes
{
public:
    /// Default constructor creates an empty set of attributes.
    wxSVGAttributes();

    /**
        Sets the @c role attribute, which classifies the element for
        assistive technology. Common SVG values include @c "img" (the
        subtree is a single graphical image), @c "graphics-document", and
        @c "graphics-symbol".
    */
    wxSVGAttributes& Role(const wxString& role);

    /**
        Sets the @c aria-label attribute: a short accessible name read
        aloud by screen readers. Use when no visible text label exists.
    */
    wxSVGAttributes& AriaLabel(const wxString& label);

    /**
        Sets the @c aria-labelledby attribute: instead of supplying a name
        inline, points at the @c id of another element whose text content
        provides the accessible name.
    */
    wxSVGAttributes& AriaLabelledBy(const wxString& id);

    /**
        Sets the @c aria-describedby attribute: points at the @c id of
        another element whose text content provides a longer description,
        announced after the accessible name.
    */
    wxSVGAttributes& AriaDescribedBy(const wxString& id);

    /**
        Sets the @c aria-hidden attribute.

        The default @a hidden value of @true is the common case (hide
        decorative content from assistive technology). Passing @false is
        only meaningful when overriding an inherited @c aria-hidden="true"
        on an ancestor.
    */
    wxSVGAttributes& AriaHidden(bool hidden = true);

    /**
        Sets the @c aria-details attribute: points at the @c id of another
        element containing extended information about this one
        (e.g., a data table describing a chart).
    */
    wxSVGAttributes& AriaDetails(const wxString& id);

    /**
        Sets the @c aria-roledescription attribute: a human-readable,
        optionally localized phrase that replaces the default spoken role
        (for example @c "pie chart" instead of @c "graphic").
    */
    wxSVGAttributes& AriaRoleDescription(const wxString& desc);

    /**
        Sets the @c id attribute, giving the element a unique identifier
        other attributes (such as @c aria-labelledby) or stylesheets can
        reference.
    */
    wxSVGAttributes& Id(const wxString& id);

    /**
        Sets the @c class attribute, used to associate the element with
        one or more CSS classes for styling the SVG output.
    */
    wxSVGAttributes& Class(const wxString& classname);

    /**
        Adds or updates an arbitrary attribute.

        If @a name is already set, its value is replaced; otherwise a new
        entry is appended.
    */
    wxSVGAttributes& Add(const wxString& name, const wxString& value);

    /// Returns @true if no attributes have been set.
    bool IsEmpty() const;
};


/**
    @class wxSVGFileDC

    A wxSVGFileDC is a device context onto which graphics and text can be
    drawn, and the output produced as a vector file, in SVG format.

    This format can be read by a range of programs, including most web
    browsers and the open source Inkscape program (https://inkscape.org/).
    Full details are given in the W3C SVG recommendation (https://www.w3.org/TR/SVG/).

    The intention behind wxSVGFileDC is that it can be used to produce a file
    corresponding to the screen display context, wxSVGFileDC, by passing the
    wxSVGFileDC as a parameter instead of a wxDC. Thus the wxSVGFileDC
    is a write-only class.

    As the wxSVGFileDC is a vector format, raster operations like GetPixel()
    are unlikely to be supported. However, the SVG specification allows for
    raster files to be embedded in the SVG, and so bitmaps, icons and blit
    operations in wxSVGFileDC are supported. By default only PNG format bitmaps
    are supported and these are saved as separate files in the same folder
    as the SVG file, however it is possible to change this behaviour by
    replacing the built in bitmap handler using wxSVGFileDC::SetBitmapHandler().

    More substantial SVG libraries (for reading and writing) are available at
    <a href="https://wxart2d.sourceforge.net/" target="_blank">wxArt2D</a> and
    <a href="https://wxsvg.sourceforge.net/" target="_blank">wxSVG</a>.

    @library{wxcore}
    @category{dc}
*/

class wxSVGFileDC : public wxDC
{
public:
    /**
        Initializes a wxSVGFileDC with the given @a filename, @a width and
        @a height at @a dpi resolution, and an optional @a title.

        The title provides a readable name for the SVG document.
        The filename is allowed to be empty, in which case no SVG file will be
        written. Call GetSVGDocument() to retrieve the generated content.
    */
    wxSVGFileDC(const wxString& filename, int width = 320, int height = 240,
                double dpi = wxSVG_DEFAULT_DPI, const wxString& title = wxString());

    /**
        Initializes a wxSVGFileDC with the given @a size, an optional @a filename,
        an optional @a title, and an optional @a dpi resolution.

        The title provides a readable name for the SVG document.
        The filename is allowed to be empty, in which case no SVG file will be
        written. Call GetSVGDocument() to retrieve the generated content.

        @since 3.3.2
    */
    wxSVGFileDC(const wxSize size, const wxString& filename = wxString(),
                const wxString& title = wxString(), double dpi = wxSVG_DEFAULT_DPI);

    /**
        Draws a rectangle the size of the SVG using the wxDC::SetBackground() brush.
    */
    void Clear();

    /**
        Replaces the default bitmap handler with @a handler.

        By default, an object of wxSVGBitmapFileHandler class is used as bitmap
        handler. You may want to replace it with an object of predefined
        wxSVGBitmapEmbedHandler class to embed the bitmaps in the generated SVG
        instead of storing them in separate files like this:
        @code
        mySVGFileDC->SetBitmapHandler(new wxSVGBitmapEmbedHandler());
        @endcode

        or derive your own bitmap handler class and use it if you need to
        customize the bitmap handling further.

        @param handler The new bitmap handler. If non-null, this object takes
            ownership of this handler and will delete it when it is not needed
            any more.

        @since 3.1.0
    */
    void SetBitmapHandler(wxSVGBitmapHandler* handler);

    /**
        Set the shape rendering mode of the generated SVG.
        All subsequent drawing calls will have this rendering mode set in the
        SVG file.

        The default mode is wxSVG_SHAPE_RENDERING_AUTO.

        @since 3.1.3
    */
    void SetShapeRenderingMode(wxSVGShapeRenderingMode renderingMode);

    /**
        Opens an accessible group wrapping all subsequent drawing until the
        matching EndAccessibleGroup() call.

        The group is emitted as an SVG `<g>` element carrying the given
        @a attributes, optionally followed by `<title>` and `<desc>`
        children supplying an accessible name and long description.

        Prefer wxSVGAccessibleGroup for RAII-style scoping, which
        guarantees EndAccessibleGroup() is called on scope exit.

        @since 3.3.3
    */
    void BeginAccessibleGroup(const wxSVGAttributes& attributes,
                              const wxString& title = wxString(),
                              const wxString& desc = wxString());

    /**
        Closes the accessible group opened by the most recent
        BeginAccessibleGroup() call.

        @since 3.3.3
    */
    void EndAccessibleGroup();

    /**
        Returns the SVG document as a string.

        This can be used to retrieve the generated SVG content regardless of
        whether it was also written to a file. This can be called after all
        drawing commands to get the current SVG content.

        @since 3.3.3
    */
    wxString GetSVGDocument() const;

    /**
        Saves the SVG document to the file specified in the constructor.

        This is called automatically by the destructor if a filename was
        provided, but calling it explicitly allows checking for errors.
        Does nothing if no filename was provided or if the file has already
        been saved.

        @return @true if the file was written successfully, @false otherwise.

        @since 3.3.3
    */
    bool Save();

    /**
        Destroys the current clipping region so that none of the DC is clipped.
        Since intersections arising from sequential calls to SetClippingRegion are represented
        with nested SVG group elements (\<g\>), all such groups are closed when
        DestroyClippingRegion is called.
    */
    void DestroyClippingRegion();

    ///@{
    /**
        Function not implemented in this DC class.
    */
    void CrossHair(wxCoord x, wxCoord y);
    bool FloodFill(wxCoord x, wxCoord y, const wxColour& colour,
                   wxFloodFillStyle style = wxFLOOD_SURFACE);
    bool GetPixel(wxCoord x, wxCoord y, wxColour* colour) const;
    void SetPalette(const wxPalette& palette);
    int GetDepth() const;
    void SetLogicalFunction(wxRasterOperationMode function);
    wxRasterOperationMode GetLogicalFunction() const;
    bool StartDoc(const wxString& message);
    void EndDoc();
    void StartPage();
    void EndPage();
    ///@}
};

/**
    @class wxSVGAccessibleGroup

    RAII helper that opens an accessible group on a wxSVGFileDC when
    constructed and closes it when destroyed. Use it in place of a manual
    wxSVGFileDC::BeginAccessibleGroup() / wxSVGFileDC::EndAccessibleGroup()
    pair so the group is reliably closed on scope exit.

    Example:
    @code
    {
        wxSVGAccessibleGroup group(dc,
            wxSVGAttributes().Role("img").AriaLabel("Quarterly sales"),
            "Quarterly sales",
            "Bar chart showing revenue for Q1-Q4 2026.");

        dc.DrawRectangle(10, 10, 100, 60);
        // ... further drawing ...
    } // group closes here
    @endcode

    @library{wxcore}
    @category{dc}
    @since 3.3.3
*/
class wxSVGAccessibleGroup
{
public:
    /**
        Opens an accessible group on @a dc with the given @a attributes
        and optional @a title and @a desc children.
    */
    wxSVGAccessibleGroup(wxSVGFileDC& dc,
                         const wxSVGAttributes& attributes,
                         const wxString& title = wxString(),
                         const wxString& desc = wxString());

    /**
        Closes the accessible group opened by the constructor.
    */
    ~wxSVGAccessibleGroup();
};

/**
    Abstract base class for handling bitmaps inside a wxSVGFileDC.

    To use it you need to derive a new class from it and override
    ProcessBitmap() to generate a properly a formed SVG image element (see
    http://www.w3.org/TR/SVG/struct.html#ImageElement).

    Two example bitmap handlers are provided in wx/dcsvg.h. The first (default)
    handler will create PNG files in the same folder as the SVG file and uses
    links to them in the SVG. The second handler (wxSVGBitmapEmbedHandler) will
    embed the PNG image in the SVG file using base 64 encoding.

    The handler can be changed by calling wxSVGFileDC::SetBitmapHandler().

    @library{wxcore}
    @category{dc}

    @since 3.1.0
*/
class wxSVGBitmapHandler
{
public:
    /**
        Writes the bitmap representation as SVG to the given stream.

        The XML generated by this function will be inserted into the SVG file
        inline with the XML generated by the main wxSVGFileDC class so it is
        important that the XML is properly formed.

        @param bitmap A valid bitmap to add to SVG.
        @param x Horizontal position of the bitmap.
        @param y Vertical position of the bitmap.
        @param stream The stream to write SVG contents to.
    */
    virtual bool ProcessBitmap(const wxBitmap& bitmap,
                               wxCoord x, wxCoord y,
                               wxOutputStream& stream) const = 0;
};

/**
    Handler embedding bitmaps as base64-encoded PNGs into the SVG.

    @see wxSVGFileDC::SetBitmapHandler().

    @library{wxcore}
    @category{dc}

    @since 3.1.0
*/
class wxSVGBitmapEmbedHandler : public wxSVGBitmapHandler
{
public:
    virtual bool ProcessBitmap(const wxBitmap& bitmap,
                               wxCoord x, wxCoord y,
                               wxOutputStream& stream) const;
};

/**
    Handler saving bitmaps to external PNG files and linking to it from the
    SVG.

    This handler is used by default by wxSVGFileDC. PNG files are created in
    the same folder as the SVG file and are named using the SVG filename
    appended with ``_image#.png``.

    When using wxSVGFileDC::SetBitmapHandler() to set this handler with the
    default constructor, the PNG files are created in the runtime location of
    the application. The save location can be customized by using the
    wxSVGBitmapFileHandler(const wxFileName&) constructor.

    @see wxSVGFileDC::SetBitmapHandler().

    @library{wxcore}
    @category{dc}

    @since 3.1.0
*/
class wxSVGBitmapFileHandler : public wxSVGBitmapHandler
{
public:
    /**
        Create a wxSVGBitmapFileHandler and specify the location where the file
        will be saved.

        @param path The path of the save location. If @a path contains a
        filename, the autogenerated filename will be appended to this name.

        @since 3.1.3
    */
    wxSVGBitmapFileHandler(const wxFileName& path);

    virtual bool ProcessBitmap(const wxBitmap& bitmap,
                               wxCoord x, wxCoord y,
                               wxOutputStream& stream) const;
};
