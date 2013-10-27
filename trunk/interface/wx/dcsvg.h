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
    are unlikely to be supported. However, the SVG specification allows for PNG
    format raster files to be embedded in the SVG, and so bitmaps, icons and
    blit operations in wxSVGFileDC are supported.

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
        @a Width and @a Height at @a dpi resolution.
    */
    wxSVGFileDC(const wxString& filename, int width = 320, int height = 240, double dpi = 72);

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
        This makes no sense in wxSVGFileDC and does nothing.
    */
    void Clear();

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

