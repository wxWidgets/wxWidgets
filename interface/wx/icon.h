/////////////////////////////////////////////////////////////////////////////
// Name:        icon.h
// Purpose:     interface of wxIcon
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxIcon
    @wxheader{icon.h}

    An icon is a small rectangular bitmap usually used for denoting a
    minimized application. It differs from a wxBitmap in always
    having a mask associated with it for transparent drawing. On some platforms,
    icons and bitmaps are implemented identically, since there is no real
    distinction between
    a wxBitmap with a mask and an icon; and there is no specific icon format on
    some platforms (X-based applications usually standardize on XPMs for small
    bitmaps
    and icons). However, some platforms (such as Windows) make the distinction, so
    a separate class is provided.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::wxNullIcon

    @see @ref overview_wxbitmapoverview "Bitmap and icon overview", @ref
    overview_supportedbitmapformats "supported bitmap file formats", wxDC::DrawIcon, wxCursor
*/
class wxIcon : public wxBitmap
{
public:
    //@{
    /**
        Loads an icon from the specified location().

        @param bits
            Specifies an array of pixel values.
        @param width
            Specifies the width of the icon.
        @param height
            Specifies the height of the icon.
        @param desiredWidth
            Specifies the desired width of the icon. This
            parameter only has an effect in Windows (32-bit) where icon resources can
        contain
            several icons of different sizes.
        @param desiredWidth
            Specifies the desired height of the icon. This
            parameter only has an effect in Windows (32-bit) where icon resources can
        contain
            several icons of different sizes.
        @param depth
            Specifies the depth of the icon. If this is omitted, the display depth of
        the
            screen is used.
        @param name
            This can refer to a resource name under MS Windows, or a filename under MS
        Windows and X.
            Its meaning is determined by the flags parameter.
        @param loc
            The object describing the location of the native icon, see
            wxIconLocation.
        @param type
            May be one of the following:







            wxBITMAP_TYPE_ICO




            Load a Windows icon file.





            wxBITMAP_TYPE_ICO_RESOURCE




            Load a Windows icon from the resource database.





            wxBITMAP_TYPE_GIF




            Load a GIF bitmap file.





            wxBITMAP_TYPE_XBM




            Load an X bitmap file.





            wxBITMAP_TYPE_XPM




            Load an XPM bitmap file.





            The validity of these flags depends on the platform and wxWidgets
        configuration.
            If all possible wxWidgets settings are used, the Windows platform supports
        ICO file, ICO resource,
            XPM data, and XPM file. Under wxGTK, the available formats are BMP file,
        XPM data, XPM file, and PNG file.
            Under wxMotif, the available formats are XBM data, XBM file, XPM data, XPM
        file.

        @remarks The first form constructs an icon object with no data; an
                 assignment or another member function such as Create or
                 LoadFile must be called subsequently.
    */
    wxIcon();
    wxIcon(const wxIcon& icon);
    wxIcon(void* data, int type, int width, int height,
           int depth = -1);
    wxIcon(const char bits[], int width, int height,
           int depth = 1);
    wxIcon(int width, int height, int depth = -1);
    wxIcon(const char* const* bits);
    wxIcon(const wxString& name, wxBitmapType type,
           int desiredWidth = -1,
           int desiredHeight = -1);
    wxIcon(const wxIconLocation& loc);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
        If the application omits to delete the icon explicitly, the icon will be
        destroyed automatically by wxWidgets when the application exits.
        Do not delete an icon that is selected into a memory device context.
    */
    ~wxIcon();

    /**
        Copies @a bmp bitmap to this icon. Under MS Windows the bitmap
        must have mask colour set.

        LoadFile()

        Wx::Icon-new( width, height, depth = -1 )
        Wx::Icon-new( name, type, desiredWidth = -1, desiredHeight = -1 )
        Wx::Icon-newFromBits( bits, width, height, depth = 1 )
        Wx::Icon-newFromXPM( data )
    */
    void CopyFromBitmap(const wxBitmap& bmp);

    /**
        Gets the colour depth of the icon. A value of 1 indicates a
        monochrome icon.
    */
    int GetDepth() const;

    /**
        Gets the height of the icon in pixels.
    */
    int GetHeight() const;

    /**
        Gets the width of the icon in pixels.

        @see GetHeight()
    */
    int GetWidth() const;

    /**
        Returns @true if icon data is present.
    */
    bool IsOk() const;

    /**
        Loads an icon from a file or resource.

        @param name
            Either a filename or a Windows resource name.
            The meaning of name is determined by the type parameter.
        @param type
            One of the following values:







            wxBITMAP_TYPE_ICO




            Load a Windows icon file.





            wxBITMAP_TYPE_ICO_RESOURCE




            Load a Windows icon from the resource database.





            wxBITMAP_TYPE_GIF




            Load a GIF bitmap file.





            wxBITMAP_TYPE_XBM




            Load an X bitmap file.





            wxBITMAP_TYPE_XPM




            Load an XPM bitmap file.





            The validity of these flags depends on the platform and wxWidgets
        configuration.

        @return @true if the operation succeeded, @false otherwise.

        @see wxIcon()
    */
    bool LoadFile(const wxString& name, wxBitmapType type);

    /**
        Sets the depth member (does not affect the icon data).

        @param depth
            Icon depth.
    */
    void SetDepth(int depth);

    /**
        Sets the height member (does not affect the icon data).

        @param height
            Icon height in pixels.
    */
    void SetHeight(int height);

    /**
        Sets the width member (does not affect the icon data).

        @param width
            Icon width in pixels.
    */
    void SetWidth(int width);

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".

        @param icon
            Icon to assign.
    */
    wxIcon operator =(const wxIcon& icon);
};

/**
    An empty wxIcon.
*/
wxIcon wxNullIcon;


