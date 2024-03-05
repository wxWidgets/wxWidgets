/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.h
// Purpose:     interface of wxImageList
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
   Flag values for Set/GetImageList
*/
enum
{
    wxIMAGE_LIST_NORMAL, // Normal icons
    wxIMAGE_LIST_SMALL,  // Small icons
    wxIMAGE_LIST_STATE   // State icons: unimplemented (see WIN32 documentation)
};

/**
   Flags for Draw
*/
#define wxIMAGELIST_DRAW_NORMAL         0x0001
#define wxIMAGELIST_DRAW_TRANSPARENT    0x0002
#define wxIMAGELIST_DRAW_SELECTED       0x0004
#define wxIMAGELIST_DRAW_FOCUSED        0x0008


/**
    @class wxImageList

    A wxImageList contains a list of images, which are stored in an unspecified
    form. Images can use alpha channel or masks for transparent drawing, and
    can be made from a variety of sources including bitmaps and icons.

    wxImageList is used principally in conjunction with wxTreeCtrl and
    wxListCtrl classes.

    Use of this class is not recommended in the new code as it doesn't support
    showing DPI-dependent bitmaps. Please use wxWithImages::SetImages() instead
    of wxWithImages::SetImageList().

    @library{wxcore}
    @category{gdi}

    @see wxTreeCtrl, wxListCtrl
*/
class wxImageList : public wxObject
{
public:
    /**
        Default ctor.

        Note that the object created using the default ctor is invalid and
        calling any methods other than Create() on it will result in an
        assertion failure.
    */
    wxImageList();

    /**
        Constructor specifying the image size, whether image masks should be created,
        and the initial size of the list.

        Note that the size is specified in physical pixels and must correspond
        to the size of bitmaps, in pixels, that will be added to this list.

        @param width
            Width of the images in the list.
        @param height
            Height of the images in the list.
        @param mask
            If @true, all images will have masks, with the mask being created
            from the light grey pixels if not specified otherwise, i.e. if the
            image doesn't have neither alpha channel nor mask and no mask is
            explicitly specified when adding it. Note that if an image does
            have alpha channel or mask, it will always be used, whether this
            parameter is @true or @false.
        @param initialCount
            The initial size of the list.

        @see Create()
    */
    wxImageList(int width, int height, bool mask = true,
                int initialCount = 1);

    /**
        Adds a new image or images using a bitmap and optional mask bitmap.

        The physical size of the bitmap should be the same as the size specified
        when constructing wxImageList. If the width of the bitmap is greater
        than the image list width, bitmap is split into smaller images of the
        required width, allowing to add multiple images from a single bitmap.

        @param bitmap
            Bitmap representing the opaque areas of the image.
        @param mask
            Monochrome mask bitmap, representing the transparent areas of the image.

        @return The new zero-based image index.
    */
    int Add(const wxBitmap& bitmap,
            const wxBitmap& mask = wxNullBitmap);

    /**
        Adds a new image or images using a bitmap and mask colour.

        The physical size of the bitmap should be the same as the size specified
        when constructing wxImageList. If the width of the bitmap is greater
        than the image list width, bitmap is split into smaller images of the
        required width, allowing to add multiple images from a single bitmap.

        @param bitmap
            Bitmap representing the opaque areas of the image.
        @param maskColour
            Colour indicating which parts of the image are transparent.

        @return The new zero-based image index.
    */
    int Add(const wxBitmap& bitmap, const wxColour& maskColour);

    /**
        Adds a new image using an icon.

        The physical size of the icon should be the same as the size specified
        when constructing wxImageList.

        @param icon
            Icon to use as the image.

        @return The new zero-based image index.

        @onlyfor{wxmsw,wxosx}
    */
    int Add(const wxIcon& icon);

    /**
        Initializes the list.

        See wxImageList() for details.

        This function can be called only once after creating the object using
        its default ctor or after calling Destroy().
    */
    bool Create(int width, int height, bool mask = true,
                int initialCount = 1);

    /**
        Destroys the current list.

        This function resets the object to its initial state and does more than
        just RemoveAll() in the native wxMSW version.

        After calling it, Create() may be called again to recreate the image
        list, e.g. using a different size.

        @since 3.1.6
     */
    void Destroy();

    /**
        Draws a specified image onto a device context.

        @param index
            Image index, starting from zero.
        @param dc
            Device context to draw on.
        @param x
            X position on the device context.
        @param y
            Y position on the device context.
        @param flags
            How to draw the image. A bitlist of a selection of the following:
            - wxIMAGELIST_DRAW_NORMAL: Draw the image normally.
            - wxIMAGELIST_DRAW_TRANSPARENT: Draw the image with transparency.
            - wxIMAGELIST_DRAW_SELECTED: Draw the image in selected state.
            - wxIMAGELIST_DRAW_FOCUSED: Draw the image in a focused state.
        @param solidBackground
            For optimisation - drawing can be faster if the function is told
            that the background is solid.
    */
    virtual bool Draw(int index, wxDC& dc, int x, int y,
                      int flags = wxIMAGELIST_DRAW_NORMAL,
                      bool solidBackground = false);

    /**
        Returns the bitmap corresponding to the given index.
    */
    wxBitmap GetBitmap(int index) const;

    /**
        Returns the icon corresponding to the given index.
    */
    wxIcon GetIcon(int index) const;

    /**
        Returns the number of images in the list.
    */
    virtual int GetImageCount() const;

    /**
        Retrieves the size of the images in the list. Currently, the @a index
        parameter is ignored as all images in the list have the same size.

        @param index
            currently unused, should be 0
        @param width
            receives the width of the images in the list in pixels
        @param height
            receives the height of the images in the list in pixels

        @return @true if the function succeeded, @false if it failed
                (for example, if the image list was not yet initialized).
    */
    virtual bool GetSize(int index, int& width, int& height) const;

    /**
        Retrieves the size of the image list as passed to Create().

        @return the size of the image list, which may be zero if the image list
            was not yet initialised.
    */
    virtual wxSize GetSize() const;

    /**
        Removes the image at the given position.
    */
    bool Remove(int index);

    /**
        Removes all the images in the list.
    */
    bool RemoveAll();

    /**
        Replaces the existing image with the new image.

        @param index
            The index of the bitmap to be replaced.
        @param bitmap
            Bitmap representing the opaque areas of the image.
        @param mask
            Monochrome mask bitmap, representing the transparent areas of the image.

        @return @true if the replacement was successful, @false otherwise.

        @remarks The original bitmap or icon is not affected by the Replace()
                 operation, and can be deleted afterwards.
    */
    bool Replace(int index, const wxBitmap& bitmap,
                 const wxBitmap& mask = wxNullBitmap);

    /**
        Replaces the existing image with the new image.

        @param index
            The index of the bitmap to be replaced.
        @param icon
            Icon to use as the image.

        @return @true if the replacement was successful, @false otherwise.

        @remarks The original bitmap or icon is not affected by the Replace()
                 operation, and can be deleted afterwards.

        @onlyfor{wxmsw,wxosx}
    */
    bool Replace(int index, const wxIcon& icon);
};

