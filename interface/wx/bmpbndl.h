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
        - An SVG (as memory block, file, or resource) which will be rasterized at required resolutions.
        - A custom bitmap source using wxBitmapBundleImpl.
        - A single wxBitmap or wxImage for backwards compatibility.

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

        toolBar->AddTool(wxID_OPEN, "Open", wxBitmapBundle::FromBitmaps(bitmaps));
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
        toolBar->AddTool(wxID_OPEN, "Open", wxBitmapBundle::FromResources("open"));
    #else
        ... same code as shown above ...
    #endif
    @endcode
    and will load all resources called @c open, @c open_2x, @c open_1_5x etc
    (at least the first one of them must be available). See also
    wxBITMAP_BUNDLE_2() macro which can avoid the need to check for
    wxHAS_IMAGE_RESOURCES explicitly in the code in a common case of having
    only 2 embedded resources (for standard and high DPI).
    See also FromSVGResource().

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
        Conversion constructor from a single icon.

        This constructor does the same thing as FromBitmap() and only exists
        for interoperability with the existing code using wxIcon.
     */
    wxBitmapBundle(const wxIcon& icon);

    /**
        Conversion constructor from a single image.

        Similarly to the constructor from wxBitmap, this constructor only
        exists for interoperability with the existing code using wxImage and
        can be replaced with more readable FromImage() in the new code.
     */
    wxBitmapBundle(const wxImage& image);

    /**
        Conversion constructor from XPM data.

        This constructor overload exists only for compatibility with the
        existing code passing XPM data (e.g. @c foo_xpm after including @c
        foo.xpm) directly to the functions expecting a bitmap. Don't use it in
        the new code, as it is likely to be deprecated in the future.

        @since 3.2.0
     */
    wxBitmapBundle(const char* const* xpm);

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
        Create a bundle from an icon bundle.

        If @a iconBundle is invalid or empty, empty bundle is returned.

        @since 3.1.7
     */
    static wxBitmapBundle FromIconBundle(const wxIconBundle& iconBundle);

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
        bitmaps in resources, and currently only works under MSW and MacOS
        and returns an empty bundle on the other platforms.

        Under MSW, for this function to create a valid bundle, you must have @c
        RCDATA resource with the given @a name in your application resource
        file (with the extension @c .rc) containing PNG file, and any other
        resources using @a name as prefix and suffix with the scale, e.g. "_2x"
        or "_1_5x" (for 150% DPI) will be also loaded as part of the bundle.

         @see FromSVGResource()
     */
    static wxBitmapBundle FromResources(const wxString& name);

     /**
        Create a bundle from bitmaps stored as files.

        Looking in @a path for files using @a filename as prefix and potentionally a
        suffix with scale, e.g. "_2x" or "@2x"

        @param path     Path of the directory containing the files
        @param filename Bitmap's filename without any scale suffix
        @param extension File extension, without leading dot (`png` by default)
    */
    static wxBitmapBundle FromFiles(const wxString& path, const wxString& filename, const wxString& extension = "png");

    /// @overload
    static wxBitmapBundle FromFiles(const wxString& fullpathname);

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
        Create a bundle from the SVG image loaded from the given file.

        This function loads the SVG data from the given @a path and calls
        FromSVG() with it. As it is just a wrapper for FromSVG(), please see
        that function documentation for more information about SVG support.

        @param path Path to the SVG file. Notice that it should a local file,
            not an URL.
        @param sizeDef The default size to return from GetDefaultSize() for
            this bundle.
     */
    static wxBitmapBundle FromSVGFile(const wxString& path, const wxSize& sizeDef);

    /**
        Create a bundle from the SVG image loaded from an application resource.
        Available only on the platforms supporting images in resources, i.e.,
        MSW and MacOS.

        @param name On MSW, it must be a resource with @c RT_RCDATA type.
            On MacOS, it must be a file with an extension "svg" placed in
            the "Resources" subdirectory of the application bundle.
        @param sizeDef The default size to return from GetDefaultSize() for
            this bundle.

        @see FromResources(), FromSVGFile()
     */
    static wxBitmapBundle FromSVGResource(const wxString& name, const wxSize& sizeDef);

    /**
        Clear the existing bundle contents.

        After calling this function IsOk() returns @false.

        This is the same as assigning a default-constructed bitmap bundle to
        this object but slightly more explicit.

        @since 3.1.7
     */
    void Clear();

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
        Get the size that would be best to use for this bundle at the given DPI
        scaling factor.

        For bundles containing some number of the fixed-size bitmaps, this
        function returns the size of an existing bitmap closest to the ideal
        size at the given scale, i.e. GetDefaultSize() multiplied by @a scale.

        Passing a size returned by this function to GetBitmap() ensures that
        bitmap doesn't need to be rescaled, which typically significantly
        lowers its quality.
     */
    wxSize GetPreferredBitmapSizeAtScale(double scale) const;

    /**
        Get the size that would be best to use for this bundle at the DPI
        scaling factor used by the given window.

        This is just a convenient wrapper for GetPreferredBitmapSizeAtScale() calling
        that function with the result of wxWindow::GetDPIScaleFactor().

        @param window Non-null and fully created window.
     */
    wxSize GetPreferredBitmapSizeFor(const wxWindow* window) const;

    /**
        Get the size that would be best to use for this bundle at the DPI
        scaling factor used by the given window in logical size.

        This is just call GetPreferredBitmapSizeAtScale() with the result of
        wxWindow::GetDPIScaleFactor() and convert returned value with
        wxWindow::FromPhys().

        @param window Non-null and fully created window.
     */
    wxSize GetPreferredLogicalSizeFor(const wxWindow* window) const;

    /**
        Get bitmap of the specified size, creating a new bitmap from the closest
        available size by rescaling it if necessary.

        This function is mostly used by wxWidgets itself and not the
        application. As all bitmaps created by it dynamically are currently
        cached, avoid calling it for many different sizes if you do use it, as
        this will create many bitmaps that will never be deleted and will
        consume resources until the application termination.

        @param size The size of the bitmap to return, in physical pixels. If
            this parameter is wxDefaultSize, default bundle size is used.
     */
    wxBitmap GetBitmap(const wxSize& size) const;

    /**
        Get bitmap of the size appropriate for the DPI scaling used by the
        given window.

        This helper function simply combines GetPreferredBitmapSizeFor() and
        GetBitmap(), i.e. it returns a (normally unscaled) bitmap
        from the bundle of the closest size to the size that should be used at
        the DPI scaling of the provided window.

        @param window Non-null and fully created window.
     */
    wxBitmap GetBitmapFor(const wxWindow* window) const;

    /**
        Get icon of the specified size.

        This is just a convenient wrapper for GetBitmap() and simply converts
        the returned bitmap to wxIcon.
     */
    wxIcon GetIcon(const wxSize& size) const;

    /**
        Get icon of the size appropriate for the DPI scaling used by the
        given window.

        This is similar to GetBitmapFor(), but returns a wxIcon, as GetIcon()
        does.

        @param window Non-null and fully created window.
     */
    wxIcon GetIconFor(const wxWindow* window) const;

    /**
        Check if the two bundles refer to the same object.

        Bundles are considered to be same only if they actually use the same
        underlying object, i.e. are copies of each other. If the two bundles
        were independently constructed, they're @e not considered to be the
        same, even if they were created from the same bitmap.
     */
    bool IsSameAs(const wxBitmapBundle& other) const;
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

            wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE
            {
                // If it's ok to scale the bitmap, just use the standard size
                // at the given scale:
                return GetDefaultSize()*scale;

                ... otherwise, an existing bitmap of the size closest to the
                    one above would need to be found and its size returned,
                    possibly by letting DoGetPreferredSize() choose it ...
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
        Return the preferred size that should be used at the given scale.

        Must always return a valid size.
     */
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const = 0;

    /**
        Retrieve the bitmap of exactly the given size.

        Note that this function is non-const because it may generate the bitmap
        on demand and cache it.
     */
    virtual wxBitmap GetBitmap(const wxSize& size) = 0;

protected:
    /**
        Helper for implementing GetPreferredBitmapSizeAtScale() in the derived
        classes.

        This function implements the standard algorithm used inside wxWidgets
        itself and tries to find the scale closest to the given one, while also
        trying to choose one of the available scales, to avoid actually
        rescaling the bitmaps.

        It relies on GetNextAvailableScale() to get information about the
        available bitmaps, so that function must be overridden if this one is
        used.

        Typically this function is used in the derived classes implementation
        to forward GetPreferredBitmapSizeAtScale() to it and when this is done,
        GetBitmap() may also use GetIndexToUpscale() to choose the bitmap to
        upscale if necessary:
        @code
        class MyCustomBitmapBundleImpl : public wxBitmapBundleImpl
        {
        public:
            wxSize GetDefaultSize() const
            {
                return wxSize(32, 32);
            }

            wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE
            {
                return DoGetPreferredSize(scale);
            }

            wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE
            {
                // For consistency with GetNextAvailableScale(), we must have
                // bitmap variants for 32, 48 and 64px sizes.
                const wxSize availableSizes[] = { 32, 48, 64 };
                if ( size.y <= 64 )
                {
                    ... get the bitmap from somewhere ...
                }
                else
                {
                    size_t n = GetIndexToUpscale(size);
                    bitmap = ... get bitmap for availableSizes[n] ...;
                    wxBitmap::Rescale(bitmap, size);
                }

                return bitmap;
            }

        protected:
            double GetNextAvailableScale(size_t& i) const wxOVERRIDE
            {
                const double availableScales[] = { 1, 1.5, 2, 0 };

                // We can rely on not being called again once we return 0.
                return availableScales[i++];
            }

            ...
        };
        @endcode

        @param scale The required scale, typically the same one as passed to
            GetPreferredBitmapSizeAtScale().

        @since 3.1.7
     */
    wxSize DoGetPreferredSize(double scale) const;

    /**
        Return the index of the available scale most suitable to be upscaled to
        the given size.

        See DoGetPreferredSize() for an example of using this function.

        @param size The required size, typically the same one as passed to
            GetBitmap()

        @since 3.1.7
     */
    size_t GetIndexToUpscale(const wxSize& size) const;

    /**
        Return information about the available bitmaps.

        Overriding this function is optional and only needs to be done if
        either DoGetPreferredSize() or GetIndexToUpscale() are called. If you
        do override it, this function must return the next available scale or
        0.0 if there are no more.

        The returned scales must be in ascending order and the first returned
        scale, for the initial @a i value of 0, should be 1. The function must
        change @a i, but the values of this index don't have to be consecutive
        and it's only used by this function itself, the caller only initializes
        it to 0 before the first call.

        See DoGetPreferredSize() for an example of implementing this function.

        @since 3.1.7
     */
    virtual double GetNextAvailableScale(size_t& i) const;
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
