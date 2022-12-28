/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     interface of wxBitmap* classes
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


/**
    In wxBitmap and wxBitmapHandler context this value means: "use the screen depth".
*/
#define wxBITMAP_SCREEN_DEPTH       (-1)

/**
    @class wxBitmapHandler

    This is the base class for implementing bitmap file loading/saving, and
    bitmap creation from data.
    It is used within wxBitmap and is not normally seen by the application.

    If you wish to extend the capabilities of wxBitmap, derive a class from
    wxBitmapHandler and add the handler using wxBitmap::AddHandler() in your
    application initialization.

    Note that all wxBitmapHandlers provided by wxWidgets are part of the
    @ref page_libs_wxcore library.
    For details about the default handlers, please see the note in the
    wxBitmap class documentation.

    @library{wxcore}
    @category{gdi}

    @see @ref overview_bitmap, wxBitmap, wxIcon, wxCursor
*/
class wxBitmapHandler : public wxObject
{
public:
    /**
        Default constructor.

        In your own default constructor, initialise the members m_name,
        m_extension and m_type.
    */
    wxBitmapHandler();

    /**
        Destroys the wxBitmapHandler object.
    */
    virtual ~wxBitmapHandler();

    /**
        Creates a bitmap from the given data, which can be of arbitrary type.
        The wxBitmap object @a bitmap is manipulated by this function.

        @param bitmap
            The wxBitmap object.
        @param width
            The width of the bitmap in pixels.
        @param height
            The height of the bitmap in pixels.
        @param depth
            The depth of the bitmap in pixels.
            If this is ::wxBITMAP_SCREEN_DEPTH, the screen depth is used.
        @param data
            Data whose type depends on the value of type.
        @param type
            A bitmap type identifier - see ::wxBitmapType for a list
            of possible values.

        @return @true if the call succeeded, @false otherwise (the default).
    */
    virtual bool Create(wxBitmap* bitmap, const void* data, wxBitmapType type,
                        int width, int height, int depth = 1);

    /**
        Gets the file extension associated with this handler.
    */
    const wxString& GetExtension() const;

    /**
        Gets the name of this handler.
    */
    const wxString& GetName() const;

    /**
        Gets the bitmap type associated with this handler.
    */
    wxBitmapType GetType() const;

    /**
        Loads a bitmap from a file or resource, putting the resulting data into
        @a bitmap.

        @note Under MSW, when loading a bitmap from resources (i.e. using @c
            wxBITMAP_TYPE_BMP_RESOURCE as @a type), the light grey colour is
            considered to be transparent, for historical reasons. If you want
            to handle the light grey pixels normally instead, call
            `SetMask(nullptr)` after loading the bitmap.

        @param bitmap
            The bitmap object which is to be affected by this operation.
        @param name
            Either a filename or a Windows resource name.
            The meaning of name is determined by the type parameter.
        @param type
            See ::wxBitmapType for values this can take.
        @param desiredWidth
            The desired width for the loaded bitmap.
        @param desiredHeight
            The desired height for the loaded bitmap.

        @return @true if the operation succeeded, @false otherwise.

        @see wxBitmap::LoadFile, wxBitmap::SaveFile, SaveFile()
    */
    virtual bool LoadFile(wxBitmap* bitmap, const wxString& name, wxBitmapType type,
                          int desiredWidth, int desiredHeight);

    /**
        Saves a bitmap in the named file.

        @param bitmap
            The bitmap object which is to be affected by this operation.
        @param name
            A filename. The meaning of name is determined by the type parameter.
        @param type
            See ::wxBitmapType for values this can take.
        @param palette
            An optional palette used for saving the bitmap.

        @return @true if the operation succeeded, @false otherwise.

        @see wxBitmap::LoadFile, wxBitmap::SaveFile, LoadFile()
    */
    virtual bool SaveFile(const wxBitmap* bitmap, const wxString& name, wxBitmapType type,
                          const wxPalette* palette = nullptr) const;

    /**
        Sets the handler extension.

        @param extension
            Handler extension.
    */
    void SetExtension(const wxString& extension);

    /**
        Sets the handler name.

        @param name
            Handler name.
    */
    void SetName(const wxString& name);

    /**
        Sets the handler type.

        @param type
            Handler type.
    */
    void SetType(wxBitmapType type);
};


/**
    @class wxBitmap

    This class encapsulates the concept of a platform-dependent bitmap,
    either monochrome or colour or colour with alpha channel support.

    If you need direct access the bitmap data instead going through
    drawing to it using wxMemoryDC you need to use the wxPixelData
    class (either wxNativePixelData for RGB bitmaps or wxAlphaPixelData
    for bitmaps with an additionally alpha channel).

    Note that many wxBitmap functions take a @e type parameter, which is a
    value of the ::wxBitmapType enumeration.
    The validity of those values depends however on the platform where your program
    is running and from the wxWidgets configuration.
    If all possible wxWidgets settings are used:
    - wxMSW supports BMP and ICO files, BMP and ICO resources;
    - wxGTK supports any file supported by gdk-pixbuf;
    - wxMac supports PICT resources;
    - wxX11 supports XPM files, XPM data, XBM data;

    In addition, wxBitmap can load and save all formats that wxImage can; see wxImage
    for more info. Of course, you must have loaded the wxImage handlers
    (see ::wxInitAllImageHandlers() and wxImage::AddHandler).
    Note that all available wxBitmapHandlers for a given wxWidgets port are
    automatically loaded at startup so you won't need to use wxBitmap::AddHandler.

    More on the difference between wxImage and wxBitmap: wxImage is just a
    buffer of RGB bytes with an optional buffer for the alpha bytes. It is all
    generic, platform independent and image file format independent code. It
    includes generic code for scaling, resizing, clipping, and other manipulations
    of the image data. OTOH, wxBitmap is intended to be a wrapper of whatever is
    the native image format that is quickest/easiest to draw to a DC or to be the
    target of the drawing operations performed on a wxMemoryDC. By splitting the
    responsibilities between wxImage/wxBitmap like this then it's easier to use
    generic code shared by all platforms and image types for generic operations and
    platform specific code where performance or compatibility is needed.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::wxNullBitmap

    @see @ref overview_bitmap, @ref overview_bitmap_supportedformats,
         wxDC::Blit, wxIcon, wxCursor, wxMemoryDC, wxImage, wxPixelData
*/
class wxBitmap : public wxGDIObject
{
public:
    /**
        Default constructor.

        Constructs a bitmap object with no data; an assignment or another member
        function such as Create() or LoadFile() must be called subsequently.
    */
    wxBitmap();

    /**
        Copy constructor, uses @ref overview_refcount "reference counting".
        To make a real copy, you can use:

        @code
        wxBitmap newBitmap = oldBitmap.GetSubBitmap(
                             wxRect(0, 0, oldBitmap.GetWidth(), oldBitmap.GetHeight()));
        @endcode
    */
    wxBitmap(const wxBitmap& bitmap);


    /*
        Creates a bitmap from the given @a data which is interpreted in
        platform-dependent manner.

        @param data
            Specifies the bitmap data in a platform-dependent format.
        @param type
            May be one of the ::wxBitmapType values and indicates which type of
            bitmap does @a data contains. See the note in the class
            detailed description.
        @param width
            Specifies the width of the bitmap.
        @param height
            Specifies the height of the bitmap.
        @param depth
            Specifies the depth of the bitmap.
            If this is omitted, the display depth of the screen is used.
    wxBitmap(const void* data, int type, int width, int height, int depth = -1);


        NOTE: this ctor is not implemented by all ports, is somewhat useless
              without further description of the "data" supported formats and
              uses 'int type' instead of wxBitmapType, so don't document it.
    */

    /**
        Creates a bitmap from the given array @a bits.
        You should only use this function for monochrome bitmaps (depth 1) in
        portable programs: in this case the bits parameter should contain an XBM image.

        For other bit depths, the behaviour is platform dependent: under Windows,
        the data is passed without any changes to the underlying CreateBitmap() API.
        Under other platforms, only monochrome bitmaps may be created using this
        constructor and wxImage should be used for creating colour bitmaps from
        static data.

        @param bits
            Specifies an array of pixel values.
        @param width
            Specifies the width of the bitmap.
        @param height
            Specifies the height of the bitmap.
        @param depth
            Specifies the depth of the bitmap.
            If this is omitted, then a value of 1 (monochrome bitmap) is used.

        @beginWxPerlOnly
        In wxPerl use Wx::Bitmap->newFromBits(bits, width, height, depth).
        @endWxPerlOnly
    */
    wxBitmap(const char bits[], int width, int height, int depth = 1);

    /**
        Creates a new bitmap. A depth of ::wxBITMAP_SCREEN_DEPTH indicates the
        depth of the current screen or visual.

        Some platforms only support 1 for monochrome and ::wxBITMAP_SCREEN_DEPTH for
        the current colour setting.

        A depth of 32 including an alpha channel is supported under MSW, Mac and GTK+.

        @param width
            The width of the bitmap in pixels, must be strictly positive.
        @param height
            The height of the bitmap in pixels, must be strictly positive.
        @param depth
            The number of bits used to represent each bitmap pixel.
    */
    wxBitmap(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        @overload
    */
    wxBitmap(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        Create a bitmap compatible with the given DC, inheriting its magnification factor

        @param width
            The width of the bitmap in pixels, must be strictly positive.
        @param height
            The height of the bitmap in pixels, must be strictly positive.
        @param dc
            DC from which the scaling factor is inherited

        @since 3.1.7 (previously available only in wxMSW and wxOSX ports).
     */
    wxBitmap(int width, int height, const wxDC& dc);

    /**
        Creates a bitmap from XPM data.

        @beginWxPerlOnly
        In wxPerl use Wx::Bitmap->newFromXPM(data).
        @endWxPerlOnly
    */
    wxBitmap(const char* const* bits);

    /**
        Loads a bitmap from a file or resource.

        @param name
            This can refer to a resource name or a filename under MS Windows and X.
            Its meaning is determined by the @a type parameter.
        @param type
            May be one of the ::wxBitmapType values and indicates which type of
            bitmap should be loaded. See the note in the class detailed description.
            Note that the wxBITMAP_DEFAULT_TYPE constant has different value under
            different wxWidgets ports. See the bitmap.h header for the value it takes
            for a specific port.

        @see LoadFile()
    */
    wxBitmap(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

    /**
        Creates this bitmap object from the given image.
        This has to be done to actually display an image as you cannot draw an
        image directly on a window.

        The resulting bitmap will use the provided colour depth (or that of the
        current system if depth is ::wxBITMAP_SCREEN_DEPTH) which entails that a
        colour reduction may take place.

        On Windows, if there is a palette present (set with SetPalette), it will be
        used when creating the wxBitmap (most useful in 8-bit display mode).
        On other platforms, the palette is currently ignored.

        @param img
            Platform-independent wxImage object.
        @param depth
            Specifies the depth of the bitmap.
            If this is omitted, the display depth of the screen is used.
    */
    wxBitmap(const wxImage& img, int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        Creates a bitmap compatible with the given DC from the given image.

        This constructor initializes the bitmap with the data of the given
        image, which must be valid, but inherits the scaling factor from the
        given device context instead of simply using the default factor of 1.

        @param img
            Platform-independent wxImage object.
        @param dc
            DC from which the scaling factor is inherited

        @since 3.1.7 (previously this constructor overload was only available
            in wxMSW port)
     */
    wxBitmap(const wxImage& img, const wxDC& dc);

    /**
        Creates bitmap corresponding to the given cursor.

        This can be useful to display a cursor as it cannot be drawn directly
        on a window.

        @param cursor A valid wxCursor.

        @since 3.1.0
    */
    explicit wxBitmap(const wxCursor& cursor);

    /**
        Destructor.
        See @ref overview_refcount_destruct for more info.

        If the application omits to delete the bitmap explicitly, the bitmap will be
        destroyed automatically by wxWidgets when the application exits.

        @warning
        Do not delete a bitmap that is selected into a memory device context.
    */
    virtual ~wxBitmap();

    /**
        Adds a handler to the end of the static list of format handlers.

        @param handler
            A new bitmap format handler object. There is usually only one instance
            of a given handler class in an application session.

        Note that unlike wxImage::AddHandler, there's no documented list of
        the wxBitmapHandlers available in wxWidgets.
        This is because they are platform-specific and most important, they are
        all automatically loaded at startup.

        If you want to be sure that wxBitmap can load a certain type of image,
        you'd better use wxImage::AddHandler.

        @see wxBitmapHandler
    */
    static void AddHandler(wxBitmapHandler* handler);

    /**
        Deletes all bitmap handlers.
        This function is called by wxWidgets on exit.
    */
    static void CleanUpHandlers();

    /**
        Returns disabled (dimmed) version of the bitmap.

        This method is not available when <code>wxUSE_IMAGE == 0</code>.

        @since 2.9.0
    */
    wxBitmap ConvertToDisabled(unsigned char brightness = 255) const;

    /**
        Creates an image from a platform-dependent bitmap. This preserves
        mask information so that bitmaps and images can be converted back
        and forth without loss in that respect.
    */
    virtual wxImage ConvertToImage() const;

    /**
        Creates the bitmap from an icon.
    */
    virtual bool CopyFromIcon(const wxIcon& icon);

    /**
        Creates a fresh bitmap.
        If the final argument is omitted, the display depth of the screen is used.

        @param width
            The width of the bitmap in pixels, must be strictly positive.
        @param height
            The height of the bitmap in pixels, must be strictly positive.
        @param depth
            The number of bits used to represent each bitmap pixel.

        @return @true if the creation was successful.
    */
    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        @overload
    */
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        Create a bitmap compatible with the given DC, inheriting its magnification factor

        @param width
            The width of the bitmap in pixels, must be strictly positive.
        @param height
            The height of the bitmap in pixels, must be strictly positive.
        @param dc
            DC from which the scaling factor is inherited

        @return @true if the creation was successful.

        @since 3.1.0
    */
    bool Create(int width, int height, const wxDC& dc);

    /**
        Create a bitmap specifying its size in DPI-independent pixels and the
        scale factor to use.

        The physical size of the bitmap is obtained by multiplying the given
        @a size by @a scale and rounding it to the closest integer.

        After using this function the following postconditions are true:

        - GetSize() returns @a size multiplied by @a scale
        - GetDIPSize() returns @a size
        - GetScaleFactor() returns @a scale

        @param size
            The size of the bitmap in DPI-independent pixels. Both width and
            height must be strictly positive.
        @param scale
            Scale factor used by the bitmap, see SetScaleFactor().
        @param depth
            The number of bits used to represent each bitmap pixel.

        @return @true if the creation was successful.

        @since 3.1.6
     */
    bool CreateWithDIPSize(const wxSize& size,
                           double scale,
                           int depth = wxBITMAP_SCREEN_DEPTH);

    /// @overload
    bool CreateWithDIPSize(int width, int height,
                           double scale,
                           int depth = wxBITMAP_SCREEN_DEPTH);

    /**
        Create a bitmap with a scale factor.

        This is an older synonym for CreateWithDIPSize(), use the new
        function in the new code.

        @param width
            The width of the bitmap in pixels, must be strictly positive.
        @param height
            The height of the bitmap in pixels, must be strictly positive.
        @param depth
            The number of bits used to represent each bitmap pixel.
        @param logicalScale
            Scale factor used by the bitmap, see SetScaleFactor().

        @return @true if the creation was successful.

        @since 3.1.0
    */
    bool CreateScaled(int width, int height, int depth, double logicalScale);

    /*
        Creates a bitmap from the given data, which can be of arbitrary type.

        @param data
            Data whose type depends on the value of type.
        @param type
            A bitmap type identifier; see ::wxBitmapType for the list of values.
            See the note in the class detailed description for more info.
        @param width
            The width of the bitmap in pixels.
        @param height
            The height of the bitmap in pixels.
        @param depth
            The depth of the bitmap in pixels. If this is -1, the screen depth is used.

        @return @true if the call succeeded, @false otherwise.

        This overload depends on the @a type of data.

    virtual bool Create(const void* data, int type, int width,
                        int height, int depth = -1);

        NOTE: leave this undoc for the same reason of the relative ctor.
    */

    /**
        Finds the handler with the given @a name.

        @return A pointer to the handler if found, @NULL otherwise.
    */
    static wxBitmapHandler* FindHandler(const wxString& name);

    /**
        Finds the handler associated with the given @a extension and @a type.

        @param extension
            The file extension, such as "bmp" (without the dot).
        @param bitmapType
            The bitmap type managed by the handler, see ::wxBitmapType.

        @return A pointer to the handler if found, @NULL otherwise.
    */
    static wxBitmapHandler* FindHandler(const wxString& extension,
                                        wxBitmapType bitmapType);

    /**
        Finds the handler associated with the given bitmap type.

        @param bitmapType
            The bitmap type managed by the handler, see ::wxBitmapType.

        @return A pointer to the handler if found, @NULL otherwise.

        @see wxBitmapHandler
    */

    static wxBitmapHandler* FindHandler(wxBitmapType bitmapType);

    /**
        Gets the colour depth of the bitmap.
        A value of 1 indicates a monochrome bitmap.
    */
    virtual int GetDepth() const;

    /**
        Returns the size of bitmap in DPI-independent units.

        This assumes that the bitmap was created using the value of scale
        factor corresponding to the current DPI (see CreateWithDIPSize()
        and SetScaleFactor()) and returns its physical size divided by this
        scale factor.

        Unlike GetLogicalSize(), this function returns the same value under all
        platforms and so its result should @e not be used as window or device
        context coordinates.

        @since 3.1.6
     */
    wxSize GetDIPSize() const;

    /**
        Returns the static list of bitmap format handlers.

        @see wxBitmapHandler
    */
    static wxList& GetHandlers();

    /**
        Returns the height of the bitmap in physical pixels.

        @see GetWidth(), GetSize(), GetLogicalHeight()
    */
    virtual int GetHeight() const;

    /**
        Returns the height of the bitmap in logical pixels.

        See GetLogicalSize() for more information.

        @see GetLogicalWidth(), GetWidth()

        @since 3.1.6
     */
    double GetLogicalHeight() const;

    /**
        Returns the size of the bitmap in logical pixels.

        For the platforms using DPI-independent pixels, i.e. those where @c
        wxHAS_DPI_INDEPENDENT_PIXELS is defined, such as wxOSX or wxGTK 3,
        this function returns the physical size of the bitmap, as returned by
        GetSize(), divided by its scale factor, as returned by
        GetScaleFactor(), while for the other platforms, it simply returns the
        same thing as GetSize().

        This ensures that the result of this function is always expressed in
        the pixel coordinates appropriate for the current platform, i.e. its
        return value is always in logical pixels, used for window and wxDC
        coordinates, whether these pixels are the same as physical pixels,
        which are returned by GetSize(), or not.

        @see GetLogicalWidth(), GetLogicalHeight(), GetSize()

        @since 2.9.5
     */
    wxSize GetLogicalSize() const;

    /**
        Returns the width of the bitmap in logical pixels.

        See GetLogicalSize() for more information.

        @see GetLogicalHeight(), GetWidth()

        @since 3.1.6
     */
    double GetLogicalWidth() const;

    /**
        Gets the associated mask (if any) which may have been loaded from a file
        or set for the bitmap.

        @see SetMask(), wxMask
    */
    virtual wxMask* GetMask() const;

    /**
        Gets the associated palette (if any) which may have been loaded from a file
        or set for the bitmap.

        @see wxPalette
    */
    virtual wxPalette* GetPalette() const;

    /**
        Returns a sub bitmap of the current one as long as the rect belongs entirely to
        the bitmap. This function preserves bit depth and mask information.
    */
    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    /**
        Returns the scale factor of this bitmap.

        Scale factor is 1 by default, but can be greater to indicate that the
        size of bitmap in logical, DPI-independent pixels is smaller than its
        actual size in physical pixels. Bitmaps with scale factor greater than
        1 must be used in high DPI to appear sharp on the screen.

        Note that the scale factor is only used in the ports where logical
        pixels are not the same as physical ones, such as wxOSX or wxGTK3, and
        this function always returns 1 under the other platforms.

        @see SetScaleFactor(), GetLogicalWidth(), GetLogicalHeight(), GetLogicalSize()

        @since 2.9.5
     */
    virtual double GetScaleFactor() const;

    /**
        Returns the height of the bitmap in logical pixels.

        This is an older synonym for GetLogicalHeight(), use the new function
        in the new code.

        @since 2.9.5
     */
    double GetScaledHeight() const;

    /**
        Returns the size of the bitmap in logical pixels.

        This is an older synonym for GetLogicalSize(), use the new function in
        the new code.

        @since 2.9.5
     */
    wxSize GetScaledSize() const;

    /**
        Returns the width of the bitmap in logical pixels.

        This is an older synonym for GetLogicalWidth(), use the new function in
        the new code.

        @since 2.9.5
     */
    double GetScaledWidth() const;

    /**
        Returns the size of the bitmap in physical pixels.

        The return value of this function doesn't depend on the scale factor,
        it is always the physical size of the bitmap, i.e. corresponding to the
        actual number of pixels in it.

        @since 2.9.0

        @see GetHeight(), GetWidth(), GetLogicalSize()
    */
    wxSize GetSize() const;

    /**
        Returns the width of the bitmap in physical pixels.

        @see GetHeight(), GetSize(), GetLogicalWidth()
    */
    virtual int GetWidth() const;

    /**
        Returns true if the bitmap has an alpha channel.

        Note that the fact that a bitmap has an alpha channel doesn't
        necessarily mean that it has any transparency, as all of its pixels
        could be using wxALPHA_OPAQUE value. To actually examine the alpha
        values, the bitmap can be converted to wxImage.
     */
    bool HasAlpha() const;

    /**
        Adds the standard bitmap format handlers, which, depending on wxWidgets
        configuration, can be handlers for Windows bitmap, Windows bitmap resource,
        and XPM.

        This function is called by wxWidgets on startup.

        @see wxBitmapHandler
    */
    static void InitStandardHandlers();

    /**
        Adds a handler at the start of the static list of format handlers.

        @param handler
            A new bitmap format handler object. There is usually only one instance
            of a given handler class in an application session.

        @see wxBitmapHandler
    */
    static void InsertHandler(wxBitmapHandler* handler);

    /**
        Returns @true if bitmap data is present.
    */
    virtual bool IsOk() const;

    /**
        Loads a bitmap from a file or resource.

        @param name
            Either a filename or a Windows resource name.
            The meaning of name is determined by the @a type parameter.
        @param type
            One of the ::wxBitmapType values; see the note in the class
            detailed description.
            Note that the wxBITMAP_DEFAULT_TYPE constant has different value under
            different wxWidgets ports. See the bitmap.h header for the value it takes
            for a specific port.

        @return @true if the operation succeeded, @false otherwise.

        @remarks A palette may be associated with the bitmap if one exists
                 (especially for colour Windows bitmaps), and if the
                 code supports it. You can check if one has been created
                 by using the GetPalette() member.

        @see SaveFile()
    */
    virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

    /**
        Loads a bitmap from the memory containing image data in PNG format.

        This helper function provides the simplest way to create a wxBitmap
        from PNG image data. On most platforms, it's simply a wrapper around
        wxImage loading functions and so requires the PNG image handler to be
        registered by either calling wxInitAllImageHandlers() which also
        registers all the other image formats or including the necessary
        header:
        @code
            #include <wx/imagpng.h>
        @endcode
        and calling
        @code
            wxImage::AddHandler(new wxPNGHandler);
        @endcode
        in your application startup code.

        However under macOS this function uses native image loading and so
        doesn't require wxWidgets PNG support.

        @since 2.9.5
     */
    static wxBitmap NewFromPNGData(const void* data, size_t size);

    /**
        Finds the handler with the given name, and removes it.
        The handler is not deleted.

        @param name
            The handler name.

        @return @true if the handler was found and removed, @false otherwise.

        @see wxBitmapHandler
    */
    static bool RemoveHandler(const wxString& name);

    /**
        Rescale the given bitmap to the requested size.

        This function is just a convenient wrapper for wxImage::Rescale() used
        to resize the given @a bmp to the requested size. If you need more
        control over resizing, e.g. to specify the quality option different
        from ::wxIMAGE_QUALITY_NEAREST used by this function, please use the
        wxImage function directly instead.

        Both the bitmap itself and size must be valid.

        @since 3.1.6
     */
    static void Rescale(wxBitmap& bmp, const wxSize& sizeNeeded);

    /**
        Remove alpha channel from the bitmap.

        This is the same as calling UseAlpha() with @false argument.
     */
    void ResetAlpha();

    /**
        Saves a bitmap in the named file.

        @param name
            A filename. The meaning of name is determined by the type parameter.
        @param type
            One of the ::wxBitmapType values; see the note in the class
            detailed description.
        @param palette
            An optional palette used for saving the bitmap.

        @return @true if the operation succeeded, @false otherwise.

        @remarks Depending on how wxWidgets has been configured, not all formats
                 may be available.

        @see LoadFile()
    */
    virtual bool SaveFile(const wxString& name, wxBitmapType type,
                          const wxPalette* palette = nullptr) const;

    /**
         @deprecated This function is deprecated since version 3.1.2, dimensions
            and depth can only be set at construction time.

        Sets the depth member (does not affect the bitmap data).

        @param depth
            Bitmap depth.

    */
    virtual void SetDepth(int depth);

    /**
        @deprecated This function is deprecated since version 3.1.2, dimensions
            and depth can only be set at construction time.

        Sets the height member (does not affect the bitmap data).

        @param height
            Bitmap height in pixels.
    */
    virtual void SetHeight(int height);

    /**
        Sets the bitmap scale factor.

        This doesn't change the bitmap actual size or its contents, but changes
        its scale factor, so that it appears in a smaller size when it is drawn
        on screen: e.g. setting @a scale to 2 means that the bitmap will be
        twice smaller (in each direction) when drawn on screen in the ports in
        which logical and physical pixels differ (i.e. wxOSX and wxGTK3, but
        not wxMSW).

        When creating a new bitmap, CreateWithDIPSize() can be used to
        specify the correct scale factor from the beginning.

        @since 3.1.6
     */
    virtual void SetScaleFactor(double scale);

    /**
        Sets the mask for this bitmap.

        @remarks The bitmap object owns the mask once this has been called.

        @note A mask can be set also for bitmap with an alpha channel but
        doing so under wxMSW is not recommended because performance of drawing
        such bitmap is not very good.

        @see GetMask(), wxMask
    */
    virtual void SetMask(wxMask* mask);

    /**
        Sets the associated palette. (Not implemented under GTK+).

        @param palette
            The palette to set.

        @see wxPalette
    */
    virtual void SetPalette(const wxPalette& palette);

    /**
        @deprecated This function is deprecated since version 3.1.2, dimensions
            and depth can only be set at construction time.

        Sets the width member (does not affect the bitmap data).

        @param width
            Bitmap width in pixels.
    */
    virtual void SetWidth(int width);

    /**
        Enable or disable use of alpha channel in this bitmap.

        This function is only useful for 32bpp bitmaps and changes their format
        to use, or not use, the fourth byte of the pixel data for the alpha
        channel.

        It currently is only implemented in wxMSW and wxOSX and simply always
        returns @false under the other platforms.

        @return @true if the operation succeeded, @false otherwise, e.g. when
            trying to enable alpha channel support for a non-32bpp bitmap or if
            this operation is simply not supported by the current platform.

        @see HasAlpha(), ResetAlpha()
     */
    bool UseAlpha(bool use = true);
};

/**
    An empty wxBitmap object.
*/
wxBitmap wxNullBitmap;




/**
    @class wxMask

    This class encapsulates a monochrome mask bitmap, where the masked area is
    black and the unmasked area is white.

    When associated with a bitmap and drawn in a device context, the unmasked
    area of the bitmap will be drawn, and the masked area will not be drawn.

    @note A mask can be associated also with a bitmap with an alpha channel
    but drawing such bitmaps under wxMSW may be slow so using them should be
    avoided if drawing performance is an important factor.

    @library{wxcore}
    @category{gdi}

    @see wxBitmap, wxDC::Blit, wxMemoryDC
*/
class wxMask : public wxObject
{
public:

    /**
        Default constructor.
    */
    wxMask();

    /**
        Constructs a mask from a bitmap and a palette index that indicates the
        background.
        Not implemented for GTK.

        @param bitmap
            A valid bitmap.
        @param index
            Index into a palette, specifying the transparency colour.
    */
    wxMask(const wxBitmap& bitmap, int index);

    /**
        Constructs a mask from a monochrome bitmap.
    */
    wxMask(const wxBitmap& bitmap);

    /**
        Constructs a mask from a bitmap and a colour that indicates the background.
    */
    wxMask(const wxBitmap& bitmap, const wxColour& colour);

    /**
        Destroys the wxMask object and the underlying bitmap data.
    */
    virtual ~wxMask();

    /**
        Constructs a mask from a bitmap and a palette index that indicates the
        background.
        Not implemented for GTK.

        @param bitmap
            A valid bitmap.
        @param index
            Index into a palette, specifying the transparency colour.
    */
    bool Create(const wxBitmap& bitmap, int index);

    /**
        Constructs a mask from a monochrome bitmap.
    */
    bool Create(const wxBitmap& bitmap);

    /**
        Constructs a mask from a bitmap and a colour that indicates the background.
    */
    bool Create(const wxBitmap& bitmap, const wxColour& colour);

    /**
        Returns the mask as a monochrome bitmap.
        Currently this method is implemented in wxMSW, wxGTK and wxOSX.

        @since 2.9.5
    */
    wxBitmap GetBitmap() const;
};

