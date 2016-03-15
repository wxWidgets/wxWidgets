/////////////////////////////////////////////////////////////////////////////
// Name:        dcsvg.h
// Purpose:     interface of wxSVGFileDC
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxSVGFileDC

    A wxSVGFileDC is a device context onto which graphics and text can be
    drawn, and the output produced as a vector file, in SVG format.

    This format can be read by a range of programs, including a Netscape plugin
    (Adobe) and the open source Inkscape program (http://inkscape.org/).  Full
    details are given in the W3C SVG recommendation (http://www.w3.org/TR/SVG/).

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

    A more substantial SVG library (for reading and writing) is available at
    the wxArt2D website <http://wxart2d.sourceforge.net/>.

    @library{wxcore}
    @category{dc}
*/

class wxSVGFileDC : public wxDC
{
public:
    /**
        Initializes a wxSVGFileDC with the given @a f filename with the given
        @a Width and @a Height at @a dpi resolution, and an optional @a title.
        The title provides a readable name for the SVG document.
    */
    wxSVGFileDC(const wxString& filename, int width = 320, int height = 240,
                double dpi = 72, const wxString& title = wxString());

    /**
        Destructor.
    */
    virtual ~wxSVGFileDC();

    /**
        Does nothing.
    */
    void EndDoc();

    /**
        Does nothing.
    */
    void EndPage();

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

        @param handler The new bitmap handler. If non-NULL, this object takes
            ownership of this handler and will delete it when it is not needed
            any more.

        @since 3.1.0
    */
    void SetBitmapHandler(wxSVGBitmapHandler* handler);

    /**
        Does the same as wxDC::SetLogicalFunction(), except that only wxCOPY is
        available. Trying to set one of the other values will fail.
    */
    void SetLogicalFunction(wxRasterOperationMode function);

    /**
        Sets the clipping region for this device context to the intersection of
        the given region described by the parameters of this method and the previously
        set clipping region.
        Clipping is implemented in the SVG output using SVG group elements (\<g\>), with
        nested group elements being used to represent clipping region intersections when
        two or more calls are made to SetClippingRegion().
    */
    void SetClippingRegion(wxCoord x, wxCoord y, wxCoord width,
                           wxCoord height);

    /**
        This is an overloaded member function, provided for convenience. It differs from the
        above function only in what argument(s) it accepts.
    */
    void SetClippingRegion(const wxPoint& pt, const wxSize& sz);

    /**
        This is an overloaded member function, provided for convenience. It differs from the
        above function only in what argument(s) it accepts.
    */
    void SetClippingRegion(const wxRect& rect);

    /**
        This function is not implemented in this DC class.
        It could be implemented in future if a GetPoints() function were made available on wxRegion.
    */
    void SetClippingRegion(const wxRegion& region);

    /**
        Destroys the current clipping region so that none of the DC is clipped.
        Since intersections arising from sequential calls to SetClippingRegion are represented
        with nested SVG group elements (\<g\>), all such groups are closed when
        DestroyClippingRegion is called.
    */
    void DestroyClippingRegion();

    //@{
    /**
        Functions not implemented in this DC class.
    */
    void CrossHair(wxCoord x, wxCoord y);
    bool FloodFill(wxCoord x, wxCoord y, const wxColour& colour,
                   wxFloodFillStyle style = wxFLOOD_SURFACE);
    void GetClippingBox(wxCoord *x, wxCoord *y, wxCoord *width, wxCoord *height) const;
    bool GetPixel(wxCoord x, wxCoord y, wxColour* colour) const;
    void SetPalette(const wxPalette& palette);
    bool StartDoc(const wxString& message);
    //@}
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
    Handler saving a bitmap to an external file and linking to it from the SVG.

    This handler is used by default by wxSVGFileDC.

    @see wxSVGFileDC::SetBitmapHandler().

    @library{wxcore}
    @category{dc}

    @since 3.1.0
*/
class wxSVGBitmapFileHandler : public wxSVGBitmapHandler
{
public:
    virtual bool ProcessBitmap(const wxBitmap& bitmap,
                               wxCoord x, wxCoord y,
                               wxOutputStream& stream) const;
};
