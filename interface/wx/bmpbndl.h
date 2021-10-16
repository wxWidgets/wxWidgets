/////////////////////////////////////////////////////////////////////////////
// Name:        wx/bmpbndl.h
// Purpose:     Interface of wxBitmapBundle.
// Author:      Vadim Zeitlin
// Created:     2021-09-24
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Contains representations of the same bitmap in different resolutions.

    This class generalizes wxBitmap for applications supporting multiple DPIs
    and allows to operate with multiple versions of the same bitmap, in the
    sizes appropriate to the currently used display resolution, as a single
    unit. Notably, an entire wxBitmapBundle can be passed to functions such as
    wxToolBar::AddTool() to allow toolbar to select the best available bitmap
    to be shown.

    Objects of this class are typically created by the application and then
    passed to wxWidgets functions, but not used by the application itself.
    Currently bitmap bundles can be created from:

        - A vector of bitmaps, of any provenance.
        - A single wxBitmap or wxImage for backwards compatibility.

    More functions for creating bitmap bundles will be added in the future.

    Objects of wxBitmapBundle class have value-like semantics, i.e. they can be
    copied around freely (and cheaply) and don't need to be allocated on the
    heap. However they usually are created using static factory functions
    (known as "pseudo-constructors") such as FromBitmaps() instead of using the
    real constructors.

    Example of using this class to initialize a toolbar in a frame constructor:
    @code
    MyFrame::MyFrame()
        : wxFrame(nullptr, wxID_ANY, "My frame")
    {
        ...
        wxToolBar* toolBar = CreateToolBar();

        wxVector<wxBitmap> bitmaps;
        bitmaps.push_back(wxBITMAP_PNG(open_32x32));
        bitmaps.push_back(wxBITMAP_PNG(open_48x48));
        bitmaps.push_back(wxBITMAP_PNG(open_64x64));

        toolBar->AddTool(wxID_OPEN, wxBitmapBundle::FromBitmaps(bitmaps));
    }
    @endcode

    The code shown above will use 32 pixel bitmap in normal DPI, 64 pixel
    bitmap in "high DPI", i.e. pixel-doubling or 200% resolution, and 48 pixel
    bitmap in 150% resolution. For all the other resolutions, the best matching
    bitmap will be created dynamically from the best available match, e.g. for
    175% resolution, 64 pixel bitmap will be rescaled to 56 pixels.

    Of course, this code relies on actually having the resources with the
    corresponding names (i.e. @c open_NxN) in MSW .rc file or Mac application
    bundle and @c open_NxN_png arrays being defined in the program code, e.g.
    by including a file generated with @c bin2c (see wxBITMAP_PNG_FROM_DATA()),
    on the other platforms.

    For the platforms with resources support, you can also create the bundle
    from the bitmaps defined in the resources, which has the advantage of not
    having to explicitly list all the bitmaps, e.g. the code above becomes
    @code
    #ifdef wxHAS_IMAGE_RESOURCES
        toolBar->AddTool(wxID_OPEN, wxBitmapBundle::FromResources("open"));
    #else
        ... same code as shown above ...
    #endif
    @endcode
    and will load all resources called @c open, @c open_2x, @c open_1_5x etc
    (at least the first one of them must be available). See also
    wxBITMAP_BUNDLE_2() macro which can avoid the need to check for
    wxHAS_IMAGE_RESOURCES explicitly in the code in a common case of having
    only 2 embedded resources (for standard and high DPI).

    Also note that the existing code using wxBitmap is compatible with the
    functions taking wxBitmapBundle in wxWidgets 3.1.6 and later because
    bitmaps are implicitly convertible to the objects of this class, so just
    passing wxBitmap to the functions taking wxBitmapBundle continues to work
    and if high resolution versions of bitmap are not (yet) available for the
    other toolbar tools, single bitmaps can continue to be used instead.

    @library{wxcore}
    @category{gdi}

    @since 3.1.6
*/
class wxBitmapBundle
{
public:
    /**
        Default constructor constructs an empty bundle.

        An empty bundle can't be used for anything, but can be assigned
        something else later.
     */
    wxBitmapBundle();

    /**
        Conversion constructor from a single bitmap.

        This constructor does the same thing as FromBitmap() and only exists
        for interoperability with the existing code using wxBitmap.
     */
    wxBitmapBundle(const wxBitmap& bitmap);

    /**
        Conversion constructor from a single image.

        Similarly to the constructor from wxBitmap, this constructor only
        exists for interoperability with the existing code using wxImage and
        can be replaced with more readable FromImage() in the new code.
     */
    wxBitmapBundle(const wxImage& image);

    /**
        Copy constructor creates a copy of another bundle.
     */
    wxBitmapBundle(const wxBitmapBundle& other);

    /**
        Assignment operator makes this bundle a copy of another bundle.
     */
    wxBitmapBundle& operator=(const wxBitmapBundle& other);


    /**
        Create a bundle from the given collection of bitmaps.

        If the @a bitmaps vector is empty, an invalid, empty bundle is
        returned, otherwise initialize the bundle with all the bitmaps in this
        vector which must be themselves valid.
     */
    static wxBitmapBundle FromBitmaps(const wxVector<wxBitmap>& bitmaps);

    /// @overload
    static wxBitmapBundle FromBitmaps(const wxBitmap& bitmap1,
                                      const wxBitmap& bitmap2);

    /**
        Create a bundle from a single bitmap.

        This is only useful for compatibility with the existing code using
        wxBitmap.

        If @a bitmap is invalid, empty bundle is returned.
     */
    static wxBitmapBundle FromBitmap(const wxBitmap& bitmap);

    /**
        Create a bundle from a single image.

        This is only useful for compatibility with the existing code using
        wxImage.

        If @a image is invalid, empty bundle is returned.
     */
    static wxBitmapBundle FromImage(const wxImage& image);

    /**
        Create a bundle from a custom bitmap bundle implementation.

        This function can be used to create bundles implementing custom logic
        for creating the bitmaps, e.g. creating them on the fly rather than
        using predefined bitmaps.

        See wxBitmapBundleImpl.

        @param impl A valid, i.e. non-null, pointer. This function takes
            ownership of it, so the caller must @e not call DecRef() on it.
     */
    static wxBitmapBundle FromImpl(wxBitmapBundleImpl* impl);

    /**
        Create a bundle from the bitmaps in the application resources.

        This function can only be used on the platforms supporting storing
        bitmaps in resources, and currently only works under MSW and simply
        returns an empty bundle on the other platforms.

        Under MSW, for this function to create a valid bundle, you must have @c
        RCDATA resource with the given @a name in your application resource
        file (with the extension @c .rc) containing PNG file, and any other
        resources using @a name as prefix and suffix with the scale, e.g. "_2x"
        or "_1_5x" (for 150% DPI) will be also loaded as part of the bundle.
     */
    static wxBitmapBundle FromResources(const wxString& name);

    /**
        Create a bundle from the SVG image.

        Please note that the current implementation uses NanoSVG library
        (https://github.com/memononen/nanosvg) for parsing and rasterizing SVG
        images which imposes the following limitations:

        - Text elements are not supported at all.
        - SVG 1.1 filters are not supported.

        These limitations will be relaxed in the future wxWidgets versions.

        Please also note that this method is only available in the ports
        providing raw bitmap access via wxPixelData. This is the case for all
        tier-1 ports, but not all of them, check if @c wxHAS_SVG is defined
        before using this method if for maximum portability.

        @param data This data may, or not, have the XML document preamble, i.e.
            it can start either with @c "<?xml" processing instruction or
            directly with @c svg tag. Notice that two overloads of this
            function, taking const and non-const data, are provided: as the
            current implementation modifies the data while parsing, using the
            non-const variant is more efficient, as it avoids making copy of
            the data, but the data is consumed by it and can't be reused any
            more.
        @param sizeDef The default size to return from GetDefaultSize() for
            this bundle. As SVG images usually don't have any natural
            default size, it should be provided when creating the bundle.
     */
    static wxBitmapBundle FromSVG(char* data, const wxSize& sizeDef);

    /// @overload
    static wxBitmapBundle FromSVG(const char* data, const wxSize& sizeDef);

    /**
        Check if bitmap bundle is non-empty.

        Return @true if the bundle contains any bitmaps or @false if it is
        empty.
     */
    bool IsOk() const;

    /**
        Get the size of the bitmap represented by this bundle in default
        resolution or, equivalently, at 100% scaling.

        When creating the bundle from a number of bitmaps, this will be just
        the size of the smallest bitmap in it.

        Note that this function is mostly used by wxWidgets itself and not the
        application.
     */
    wxSize GetDefaultSize() const;

    /**
        Get bitmap of the specified size, creating a new bitmap from the closest
        available size by rescaling it if necessary.

        This function is mostly used by wxWidgets itself and not the
        application. As all bitmaps created by it dynamically are currently
        cached, avoid calling it for many different sizes if you do use it, as
        this will create many bitmaps that will never be deleted and will
        consume resources until the application termination.
     */
    wxBitmap GetBitmap(const wxSize& size) const;
};

/**
    Base class for custom implementations of wxBitmapBundle.

    This class shouldn't be used directly in the application code, but may be
    derived from to implement custom bitmap bundles.

    Example of use:
    @code
        class MyCustomBitmapBundleImpl : public wxBitmapBundleImpl
        {
        public:
            MyCustomBitmapBundleImpl()
            {
            }

            wxSize GetDefaultSize() const wxOVERRIDE
            {
                ... determine the minimum/default size for bitmap to use ...
            }

            wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE
            {
                ... get the bitmap of the requested size from somewhere and
                    cache it if necessary, i.e. if getting it is expensive ...
            }
        };

        toolBar->AddTool(wxID_OPEN, wxBitmapBundle::FromImpl(new MyCustomBitmapBundleImpl());
    @endcode

    Full (but still very simple) example of using it can be found in the
    toolbar sample code.

    @library{wxcore}
    @category{gdi}

    @since 3.1.6
*/
class wxBitmapBundleImpl : public wxRefCounter
{
public:
    /**
        Return the size of the bitmaps represented by this bundle in the default
        DPI.

        Must always return a valid size.
     */
    virtual wxSize GetDefaultSize() const = 0;

    /**
        Retrieve the bitmap of exactly the given size.

        Note that this function is non-const because it may generate the bitmap
        on demand and cache it.
     */
    virtual wxBitmap GetBitmap(const wxSize& size) = 0;
};

/**
    Creates a wxBitmapBundle from resources on the platforms supporting them or
    from two embedded bitmaps otherwise.

    This macro use wxBitmapBundle::FromResources() with the provide @a name,
    which must be an @e identifier and not a string, i.e. used without quotes,
    on the platforms where it works and wxBitmapBundle::FromBitmaps() with @c
    name_png and @c name_2x_png arrays containing PNG data elsewhere.

    Using it allows to avoid using preprocessor checks in the common case when
    just two bitmaps (for standard and high DPI) are embedded in the
    application code. Note that all bitmaps defined in the resources, even if
    there are more than 2 of them.

    Example of use:
    @code
        toolBar->AddTool(wxID_OPEN, wxBITMAP_BUNDLE_2(open));
    @endcode

    @header{wx/bmpbndl.h}

    @since 3.1.6
 */
#define wxBITMAP_BUNDLE_2(name)
