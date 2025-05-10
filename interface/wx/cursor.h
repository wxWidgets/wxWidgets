/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     interface of wxCursor
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCursor

    A cursor is a small bitmap used for denoting where the mouse pointer is,
    with a picture that indicates the point of a mouse click.

    A cursor may be associated either with the given window (and all its
    children, unless any of them defines its own cursor) using
    wxWindow::SetCursor(), or set globally using wxSetCursor(). It is also
    common to temporarily change the cursor to a "busy cursor" indicating that
    some lengthy operation is in progress and wxBusyCursor can be used for
    this.

    Because a custom cursor of a fixed size would look either inappropriately
    big in standard resolution or too small in high resolution, wxCursorBundle
    class allows to define a set of cursors of different sizes, letting
    wxWidgets to automatically select the most appropriate one for the current
    resolution and user's preferred cursor size. Using this class with
    wxWindow::SetCursorBundle() is the recommended way to use custom cursors.

    @section cursor_custom Creating a Custom Cursor

    The following is an example of creating a cursor from 32x32 bitmap data
    (down_bits) and a mask (down_mask) where 1 is black and 0 is white for the
    bits, and 1 is opaque and 0 is transparent for the mask.
    It works on Windows and GTK+.

    @code
    static char down_bits[] = { 255, 255, 255, 255, 31,
        255, 255, 255, 31, 255, 255, 255, 31, 255, 255, 255,
        31, 255, 255, 255, 31, 255, 255, 255, 31, 255, 255,
        255, 31, 255, 255, 255, 31, 255, 255, 255, 25, 243,
        255, 255, 19, 249, 255, 255, 7, 252, 255, 255, 15, 254,
        255, 255, 31, 255, 255, 255, 191, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255 };

    static char down_mask[] = { 240, 1, 0, 0, 240, 1,
        0, 0, 240, 1, 0, 0, 240, 1, 0, 0, 240, 1, 0, 0, 240, 1,
        0, 0, 240, 1, 0, 0, 240, 1, 0, 0, 255, 31, 0, 0, 255,
        31, 0, 0, 254, 15, 0, 0, 252, 7, 0, 0, 248, 3, 0, 0,
        240, 1, 0, 0, 224, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0 };

    #ifdef __WXMSW__
        wxBitmap down_bitmap(down_bits, 32, 32);
        wxBitmap down_mask_bitmap(down_mask, 32, 32);

        down_bitmap.SetMask(new wxMask(down_mask_bitmap));
        wxImage down_image = down_bitmap.ConvertToImage();
        down_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
        down_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 14);
        wxCursor down_cursor = wxCursor(down_image);
    #elif defined(__WXGTK__)
        wxCursor down_cursor = wxCursor(down_bits, 32, 32, 6, 14,
                                        down_mask, wxWHITE, wxBLACK);
    #endif
    @endcode

    @library{wxcore}
    @category{gdi}

    @stdobjects
    - ::wxNullCursor
    - ::wxSTANDARD_CURSOR
    - ::wxHOURGLASS_CURSOR
    - ::wxCROSS_CURSOR

    @see wxBitmap, wxIcon, wxWindow::SetCursor(), wxSetCursor(), ::wxStockCursor
*/
class wxCursor : public wxGDIObject
{
public:
    /**
        Default constructor.
    */
    wxCursor();

    /**
        Constructs a cursor from the provided bitmap and hotspot position.

        @param bitmap
            The bitmap to use for the cursor, should be valid.
        @param hotSpotX
            Hotspot x coordinate (relative to the top left of the image).
        @param hotSpotY
            Hotspot y coordinate (relative to the top left of the image).

        @since 3.3.0
     */
    wxCursor(const wxBitmap& bitmap, int hotSpotX = 0, int hotSpotY = 0);

    /**
        @overload

        @since 3.3.0
     */
    wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot);

    /**
        Constructs a cursor by passing a string resource name or filename.

        The arguments @a hotSpotX and @a hotSpotY are only used when there's no
        hotspot info in the resource/image-file to load (e.g. when using
        @c wxBITMAP_TYPE_ICO under wxMSW or @c wxBITMAP_TYPE_XPM under wxGTK).

        @param cursorName
            The name of the resource or the image file to load.
        @param type
            Icon type to load. It defaults to @c wxCURSOR_DEFAULT_TYPE,
            which is a @#define associated to different values on different
            platforms:
            - under Windows, it defaults to @c wxBITMAP_TYPE_CUR_RESOURCE.
              Other permitted types under Windows are @c wxBITMAP_TYPE_CUR
              (to load a cursor from a .cur cursor file), @c wxBITMAP_TYPE_ICO
              (to load a cursor from a .ico icon file) and @c wxBITMAP_TYPE_ANI
              (to load a cursor from a .ani icon file).
            - under MacOS, it defaults to @c wxBITMAP_TYPE_MACCURSOR_RESOURCE;
              when specifying a string resource name, first a PNG and then a CUR
              image is searched in resources.
            - under GTK, it defaults to @c wxBITMAP_TYPE_XPM.
              See the wxCursor(const wxImage& image) ctor for more info.
            - under X11, it defaults to @c wxBITMAP_TYPE_XPM.
        @param hotSpotX
            Hotspot x coordinate (relative to the top left of the image).
        @param hotSpotY
            Hotspot y coordinate (relative to the top left of the image).
    */
    wxCursor(const wxString& cursorName,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);

    /**
        @overload

        @since 3.3.0
     */
    wxCursor(const wxString& name, wxBitmapType type, const wxPoint& hotSpot);

    /**
        Constructs a cursor using a cursor identifier.

        @param cursorId
            A stock cursor identifier. See ::wxStockCursor.
    */
    wxCursor(wxStockCursor cursorId);

    /**
        Constructs a cursor from a wxImage. If cursor are monochrome on the
        current platform, colors with the RGB elements all greater than 127
        will be foreground, colors less than this background. The mask (if any)
        will be used to specify the transparent area.

        In wxMSW the foreground will be white and the background black.
        If the cursor is larger than 32x32 it is resized.

        In wxGTK, colour cursors and alpha channel are supported (starting from
        GTK+ 2.2). Otherwise the two most frequent colors will be used for
        foreground and background. In any case, the cursor will be displayed
        at the size of the image.

        Under wxMac (Cocoa), large cursors are supported.

        Notice that the @a image can define the cursor hot spot. To set it you
        need to use wxImage::SetOption() with @c wxIMAGE_OPTION_CUR_HOTSPOT_X
        or @c wxIMAGE_OPTION_CUR_HOTSPOT_Y, e.g.
        @code
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX);
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotY);
        @endcode
    */
    wxCursor(const wxImage& image);

    /**
        Constructs a cursor from XPM data.

        In versions of wxWidgets until 3.1.6 constructing wxCursor from XPM
        data implicitly used wxImage constructor from XPM data and wxCursor
        constructor from wxImage. Since 3.1.6 this constructor overload is
        available to allow constructing wxCursor from XPM to still work, even
        though wxImage constructor from XPM is now @c explicit.
     */
    wxCursor(const char* const* xpmData);

    /**
        wxGTK-specific constructor from data in XBM format.

        The parameters @a fg and @a bg have an effect only on GTK+, and force
        the cursor to use particular background and foreground colours.

        @param bits
            An array of XBM data bits.
        @param width
            Cursor width.
        @param height
            Cursor height.
        @param hotSpotX
            Hotspot x coordinate (relative to the top left of the image).
        @param hotSpotY
            Hotspot y coordinate (relative to the top left of the image).
        @param maskBits
            Bits for a mask bitmap.
        @param fg
            Foreground colour.
        @param bg
            Background colour.

        @onlyfor{wxgtk}

        @beginWxPerlOnly
        In wxPerl use Wx::Cursor->newData(bits, width, height, hotSpotX = -1, hotSpotY = -1, maskBits = 0).
        @endWxPerlOnly
    */
    wxCursor(const char bits[], int width, int height,
             int hotSpotX = -1, int hotSpotY = -1,
             const char maskBits[] = nullptr,
             const wxColour* fg = nullptr, const wxColour* bg = nullptr);

    /**
        Copy constructor, uses @ref overview_refcount "reference counting".

        @param cursor
            Pointer or reference to a cursor to copy.
    */
    wxCursor(const wxCursor& cursor);

    /**
        Returns @true if cursor data is present.
    */
    virtual bool IsOk() const;

    /**
        Returns the coordinates of the cursor hot spot.

        The hot spot is the point at which the mouse is actually considered to
        be when this cursor is used.

        This method is currently implemented in wxMSW, wxGTK and wxOSX (since
        wxWidgets 3.3.0) and returns ::wxDefaultPosition in the other ports.

        @since 3.1.0
     */
    wxPoint GetHotSpot() const;

    /**
        Assignment operator, using @ref overview_refcount "reference counting".
    */
    wxCursor& operator =(const wxCursor& cursor);
};


/**
    @name Predefined cursors.

    @see wxStockCursor
*/
///@{
wxCursor wxNullCursor;
wxCursor* wxSTANDARD_CURSOR;
wxCursor* wxHOURGLASS_CURSOR;
wxCursor* wxCROSS_CURSOR;
///@}

/**
    @class wxCursorBundle

    A cursor bundle is a set of different versions of the same cursor at
    different sizes.

    This class relationship with wxCursor is similar to that of wxBitmapBundle
    with wxBitmap, but it has a simpler interface because cursors are never
    scaled and always use the closest available size. It is typically used like
    the following:

    @code
    MyFrame::MyFrame()
    {
        SetCursorBundle(wxCursorBundle(wxBitmapBundle::FromResources("mycursor"),
                                       wxPoint(1, 1)));
    }
    @endcode

    Please see wxBitmapBundle documentation for more information about
    different ways of creating it.

    @library{wxcore}
    @category{gdi}

    @since 3.3.0
 */
class wxCursorBundle
{
public:
    /**
        Default ctor constructs an empty bundle.

        Such bundle represents the absence of any custom cursor but not an
        empty cursor (::wxCURSOR_BLANK can be used if this is really needed).

        You can use the assignment operator to set the bundle contents later.
     */
    wxCursorBundle();

    /**
        Create a cursor bundle from the given bitmap bundle.

        @param bitmaps
            The bitmap bundle to use for the cursor, typically containing
            bitmap in at least two sizes.
        @param hotSpot
            Hotspot coordinates (relative to the top left of the image).
            The coordinates are relative to the default size of the bitmap
            bundle and are scaled by wxWidgets for other sizes.
     */
    explicit wxCursorBundle(const wxBitmapBundle& bitmaps,
                            const wxPoint& hotSpot);

    /// @overload
    explicit wxCursorBundle(const wxBitmapBundle& bitmaps,
                            int hotSpotX = 0, int hotSpotY = 0);

    /**
        Copy constructor performs a shallow copy of the bundle.

        This operation is cheap as it doesn't copy any bitmaps.
     */
    wxCursorBundle(const wxCursorBundle& other);

    /**
        Assignment operator performs a shallow copy of the bundle.

        This operation is cheap as it doesn't copy any bitmaps.
     */
    wxCursorBundle& operator=(const wxCursorBundle& other);

    /**
        Check if cursor bundle is non-empty.
     */
    bool IsOk() const;

    /**
        Clear the bundle contents.

        IsOk() will return false after doing this.

        Use the assignment operator to set the bundle contents later.
     */
    void Clear();

    /**
        Get the cursor of the size suitable for the given window.
     */
    wxCursor GetCursorFor(const wxWindow* window) const;

    /**
        Get the cursor of the default size.

        Prefer to use GetCursorFor() instead if there is a suitable window
        available, this function only exists as last resort.
     */
    wxCursor GetCursorForMainWindow() const;

    /**
        Check if two objects refer to the same bundle.

        Note that this compares the object identity, i.e. this function returns
        @true only for copies of the same bundle, but @false for two bundles
        created from the same bitmap bundle and same hotspot coordinates.
     */
    bool IsSameAs(const wxCursorBundle& other) const;
};
