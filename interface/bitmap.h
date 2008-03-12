/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     interface of wxBitmapHandler
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxBitmapHandler
    @wxheader{bitmap.h}

    Overview()

    This is the base class for implementing bitmap file loading/saving, and bitmap
    creation from data.
    It is used within wxBitmap and is not normally seen by the application.

    If you wish to extend the capabilities of wxBitmap, derive a class from
    wxBitmapHandler
    and add the handler using wxBitmap::AddHandler in your
    application initialisation.

    @library{wxcore}
    @category{FIXME}

    @see wxBitmap, wxIcon, wxCursor
*/
class wxBitmapHandler : public wxObject
{
public:
    /**
        Default constructor. In your own default constructor, initialise the members
        m_name, m_extension and m_type.
    */
    wxBitmapHandler();

    /**
        Destroys the wxBitmapHandler object.
    */
    ~wxBitmapHandler();

    /**
        Creates a bitmap from the given data, which can be of arbitrary type. The
        wxBitmap object @a bitmap is
        manipulated by this function.
        
        @param bitmap
            The wxBitmap object.
        @param width
            The width of the bitmap in pixels.
        @param height
            The height of the bitmap in pixels.
        @param depth
            The depth of the bitmap in pixels. If this is -1, the screen depth is used.
        @param data
            Data whose type depends on the value of type.
        @param type
            A bitmap type identifier - see wxBitmapHandler() for a list
            of possible values.
        
        @returns @true if the call succeeded, @false otherwise (the default).
    */
    virtual bool Create(wxBitmap* bitmap, const void* data, int type,
                        int width,
                        int height,
                        int depth = -1);

    /**
        Gets the file extension associated with this handler.
    */
    const wxString GetExtension() const;

    /**
        Gets the name of this handler.
    */
    const wxString GetName() const;

    /**
        Gets the bitmap type associated with this handler.
    */
    long GetType() const;

    /**
        Loads a bitmap from a file or resource, putting the resulting data into @e
        bitmap.
        
        @param bitmap
            The bitmap object which is to be affected by this operation.
        @param name
            Either a filename or a Windows resource name.
            The meaning of name is determined by the type parameter.
        @param type
            See wxBitmap::wxBitmap for values this can take.
        
        @returns @true if the operation succeeded, @false otherwise.
        
        @see wxBitmap::LoadFile, wxBitmap::SaveFile, SaveFile()
    */
    bool LoadFile(wxBitmap* bitmap, const wxString& name, long type);

    /**
        Saves a bitmap in the named file.
        
        @param bitmap
            The bitmap object which is to be affected by this operation.
        @param name
            A filename. The meaning of name is determined by the type parameter.
        @param type
            See wxBitmap::wxBitmap for values this can take.
        @param palette
            An optional palette used for saving the bitmap.
        
        @returns @true if the operation succeeded, @false otherwise.
        
        @see wxBitmap::LoadFile, wxBitmap::SaveFile, LoadFile()
    */
    bool SaveFile(wxBitmap* bitmap, const wxString& name, int type,
                  wxPalette* palette = NULL);

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
        
        @param name
            Handler type.
    */
    void SetType(long type);
};



/**
    @class wxBitmap
    @ingroup group_class_gdi
    @wxheader{bitmap.h}

    This class encapsulates the concept of a platform-dependent bitmap,
    either monochrome or colour or colour with alpha channel support.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::Objects:, ::wxNullBitmap,

    @see @ref overview_wxbitmapoverview "wxBitmap overview", @ref
    overview_supportedbitmapformats "supported bitmap file formats", wxDC::Blit, wxIcon, wxCursor, wxBitmap, wxMemoryDC
*/
class wxBitmap : public wxGDIObject
{
public:
    //@{
    /**
        Creates bitmap object from the image. This has to be done
        to actually display an image as you cannot draw an image directly on a window.
        The resulting bitmap will use the provided colour depth (or that of the
        current system if depth is -1) which entails that a colour reduction has
        to take place.
        When in 8-bit mode (PseudoColour mode), the GTK port will use a color cube
        created
        on program start-up to look up colors. This ensures a very fast conversion, but
        the image quality won't be perfect (and could be better for photo images using
        more
        sophisticated dithering algorithms).
        On Windows, if there is a palette present (set with SetPalette), it will be
        used when
        creating the wxBitmap (most useful in 8-bit display mode). On other platforms,
        the palette is currently ignored.
        
        @param bits
            Specifies an array of pixel values.
        @param width
            Specifies the width of the bitmap.
        @param height
            Specifies the height of the bitmap.
        @param depth
            Specifies the depth of the bitmap. If this is omitted, the display depth of
        the
            screen is used.
        @param name
            This can refer to a resource name under MS Windows, or a filename under MS
        Windows and X.
            Its meaning is determined by the type parameter.
        @param type
            May be one of the following:
        
        
        
        
        
        
        
            wxBITMAP_TYPE_BMP
        
        
        
        
            Load a Windows bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_BMP_RESOURCE
        
        
        
        
            Load a Windows bitmap resource from the executable. Windows only.
        
        
        
        
        
            wxBITMAP_TYPE_PICT_RESOURCE
        
        
        
        
            Load a PICT image resource from the executable. Mac OS only.
        
        
        
        
        
            wxBITMAP_TYPE_GIF
        
        
        
        
            Load a GIF bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XBM
        
        
        
        
            Load an X bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XPM
        
        
        
        
            Load an XPM bitmap file.
        
        
        
        
        
            The validity of these flags depends on the platform and wxWidgets
        configuration.
            If all possible wxWidgets settings are used, the Windows platform supports
        BMP file, BMP resource,
            XPM data, and XPM. Under wxGTK, the available formats are BMP file, XPM
        data, XPM file, and PNG file.
            Under wxMotif, the available formats are XBM data, XBM file, XPM data, XPM
        file.
            In addition, wxBitmap can read all formats that wxImage can, which
        currently include
            wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_TIF, wxBITMAP_TYPE_PNG,
        wxBITMAP_TYPE_GIF, wxBITMAP_TYPE_PCX,
            and wxBITMAP_TYPE_PNM. Of course, you must have wxImage handlers loaded.
        @param img
            Platform-independent wxImage object.
        
        @remarks The first form constructs a bitmap object with no data; an
                 assignment or another member function such as Create or
                 LoadFile must be called subsequently.
        
        @see LoadFile()
    */
    wxBitmap();
    wxBitmap(const wxBitmap& bitmap);
    wxBitmap(const void* data, int type, int width, int height,
             int depth = -1);
    wxBitmap(const char bits[], int width, int height,
             int depth = 1);
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char* const* bits);
    wxBitmap(const wxString& name, long type);
    wxBitmap(const wxImage& img, int depth = -1);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
        If the application omits to delete the bitmap explicitly, the bitmap will be
        destroyed automatically by wxWidgets when the application exits.
        Do not delete a bitmap that is selected into a memory device context.
    */
    ~wxBitmap();

    /**
        Adds a handler to the end of the static list of format handlers.
        
        @param handler
            A new bitmap format handler object. There is usually only one instance
            of a given handler class in an application session.
        
        @see wxBitmapHandler
    */
    static void AddHandler(wxBitmapHandler* handler);

    /**
        Deletes all bitmap handlers.
        This function is called by wxWidgets on exit.
    */
    static void CleanUpHandlers();

    /**
        Creates an image from a platform-dependent bitmap. This preserves
        mask information so that bitmaps and images can be converted back
        and forth without loss in that respect.
    */
    wxImage ConvertToImage();

    /**
        Creates the bitmap from an icon.
    */
    bool CopyFromIcon(const wxIcon& icon);

    //@{
    /**
        Creates a bitmap from the given data, which can be of arbitrary type.
        
        @param width
            The width of the bitmap in pixels.
        @param height
            The height of the bitmap in pixels.
        @param depth
            The depth of the bitmap in pixels. If this is -1, the screen depth is used.
        @param data
            Data whose type depends on the value of type.
        @param type
            A bitmap type identifier - see wxBitmap() for a list
            of possible values.
        
        @returns @true if the call succeeded, @false otherwise.
        
        @remarks The first form works on all platforms. The portability of the
                 second form depends on the type of data.
        
        @see wxBitmap()
    */
    virtual bool Create(int width, int height, int depth = -1);
    virtual bool Create(const void* data, int type, int width,
                        int height,
                        int depth = -1);
    //@}

    //@{
    /**
        Finds the handler associated with the given bitmap type.
        
        @param name
            The handler name.
        @param extension
            The file extension, such as "bmp".
        @param bitmapType
            The bitmap type, such as wxBITMAP_TYPE_BMP.
        
        @returns A pointer to the handler if found, @NULL otherwise.
        
        @see wxBitmapHandler
    */
    static wxBitmapHandler* FindHandler(const wxString& name);
    static wxBitmapHandler* FindHandler(const wxString& extension,
                                        wxBitmapType bitmapType);
    static wxBitmapHandler* FindHandler(wxBitmapType bitmapType);
    //@}

    /**
        Gets the colour depth of the bitmap. A value of 1 indicates a
        monochrome bitmap.
    */
    int GetDepth() const;

    /**
        Returns the static list of bitmap format handlers.
        
        @see wxBitmapHandler
    */
    static wxList GetHandlers();

    /**
        Gets the height of the bitmap in pixels.
    */
    int GetHeight() const;

    /**
        Gets the associated mask (if any) which may have been loaded from a file
        or set for the bitmap.
        
        @see SetMask(), wxMask
    */
    wxMask* GetMask() const;

    /**
        Gets the associated palette (if any) which may have been loaded from a file
        or set for the bitmap.
        
        @see wxPalette
    */
    wxPalette* GetPalette() const;

    /**
        Returns a sub bitmap of the current one as long as the rect belongs entirely to
        the bitmap. This function preserves bit depth and mask information.
    */
    wxBitmap GetSubBitmap(const wxRect& rect) const;

    /**
        Gets the width of the bitmap in pixels.
        
        @see GetHeight()
    */
    int GetWidth() const;

    /**
        Adds the standard bitmap format handlers, which, depending on wxWidgets
        configuration, can be handlers for Windows bitmap, Windows bitmap resource, and
        XPM.
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
    bool IsOk() const;

    /**
        Loads a bitmap from a file or resource.
        
        @param name
            Either a filename or a Windows resource name.
            The meaning of name is determined by the type parameter.
        @param type
            One of the following values:
        
        
        
        
        
        
        
            wxBITMAP_TYPE_BMP
        
        
        
        
            Load a Windows bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_BMP_RESOURCE
        
        
        
        
            Load a Windows bitmap resource from the executable.
        
        
        
        
        
            wxBITMAP_TYPE_PICT_RESOURCE
        
        
        
        
            Load a PICT image resource from the executable. Mac OS only.
        
        
        
        
        
            wxBITMAP_TYPE_GIF
        
        
        
        
            Load a GIF bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XBM
        
        
        
        
            Load an X bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XPM
        
        
        
        
            Load an XPM bitmap file.
        
        
        
        
        
            The validity of these flags depends on the platform and wxWidgets
        configuration.
            In addition, wxBitmap can read all formats that wxImage can
            (wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG, wxBITMAP_TYPE_GIF,
        wxBITMAP_TYPE_PCX, wxBITMAP_TYPE_PNM).
            (Of course you must have wxImage handlers loaded.)
        
        @returns @true if the operation succeeded, @false otherwise.
        
        @remarks A palette may be associated with the bitmap if one exists
                 (especially for colour Windows bitmaps), and if the
                 code supports it. You can check if one has been created
                 by using the GetPalette member.
        
        @see SaveFile()
    */
    bool LoadFile(const wxString& name, wxBitmapType type);

    /**
        Finds the handler with the given name, and removes it. The handler
        is not deleted.
        
        @param name
            The handler name.
        
        @returns @true if the handler was found and removed, @false otherwise.
        
        @see wxBitmapHandler
    */
    static bool RemoveHandler(const wxString& name);

    /**
        Saves a bitmap in the named file.
        
        @param name
            A filename. The meaning of name is determined by the type parameter.
        @param type
            One of the following values:
        
        
        
        
        
        
        
            wxBITMAP_TYPE_BMP
        
        
        
        
            Save a Windows bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_GIF
        
        
        
        
            Save a GIF bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XBM
        
        
        
        
            Save an X bitmap file.
        
        
        
        
        
            wxBITMAP_TYPE_XPM
        
        
        
        
            Save an XPM bitmap file.
        
        
        
        
        
            The validity of these flags depends on the platform and wxWidgets
        configuration.
            In addition, wxBitmap can save all formats that wxImage can
            (wxBITMAP_TYPE_JPEG, wxBITMAP_TYPE_PNG).
            (Of course you must have wxImage handlers loaded.)
        @param palette
            An optional palette used for saving the bitmap.
        
        @returns @true if the operation succeeded, @false otherwise.
        
        @remarks Depending on how wxWidgets has been configured, not all formats
                 may be available.
        
        @see LoadFile()
    */
    bool SaveFile(const wxString& name, wxBitmapType type,
                  wxPalette* palette = NULL);

    /**
        Sets the depth member (does not affect the bitmap data).
        
        @param depth
            Bitmap depth.
    */
    void SetDepth(int depth);

    /**
        Sets the height member (does not affect the bitmap data).
        
        @param height
            Bitmap height in pixels.
    */
    void SetHeight(int height);

    /**
        Sets the mask for this bitmap.
        
        @remarks The bitmap object owns the mask once this has been called.
        
        @see GetMask(), wxMask
    */
    void SetMask(wxMask* mask);

    /**
        Sets the associated palette. (Not implemented under GTK+).
        
        @param palette
            The palette to set.
        
        @see wxPalette
    */
    void SetPalette(const wxPalette& palette);

    /**
        Sets the width member (does not affect the bitmap data).
        
        @param width
            Bitmap width in pixels.
    */
    void SetWidth(int width);

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
        
        @param bitmap
            Bitmap to assign.
    */
    wxBitmap operator =(const wxBitmap& bitmap);
};


/**
    FIXME
*/
wxBitmap Objects:
;

/**
    FIXME
*/
wxBitmap wxNullBitmap;




/**
    @class wxMask
    @ingroup group_class_gdi
    @wxheader{bitmap.h}

    This class encapsulates a monochrome mask bitmap, where the masked area is
    black and
    the unmasked area is white. When associated with a bitmap and drawn in a device
    context,
    the unmasked area of the bitmap will be drawn, and the masked area will not be
    drawn.

    @library{wxcore}
    @category{gdi}

    @see wxBitmap, wxDC::Blit, wxMemoryDC
*/
class wxMask : public wxObject
{
public:
    //@{
    /**
        Constructs a mask from a bitmap and a palette index that indicates the
        background. Not
        yet implemented for GTK.
        
        @param bitmap
            A valid bitmap.
        @param colour
            A colour specifying the transparency RGB values.
        @param index
            Index into a palette, specifying the transparency colour.
    */
    wxMask();
    wxMask(const wxBitmap& bitmap);
    wxMask(const wxBitmap& bitmap,
           const wxColour& colour);
    wxMask(const wxBitmap& bitmap, int index);
    //@}

    /**
        Destroys the wxMask object and the underlying bitmap data.
    */
    ~wxMask();

    //@{
    /**
        Constructs a mask from a bitmap and a palette index that indicates the
        background. Not
        yet implemented for GTK.
        
        @param bitmap
            A valid bitmap.
        @param colour
            A colour specifying the transparency RGB values.
        @param index
            Index into a palette, specifying the transparency colour.
    */
    bool Create(const wxBitmap& bitmap);
    bool Create(const wxBitmap& bitmap, const wxColour& colour);
    bool Create(const wxBitmap& bitmap, int index);
    //@}
};

