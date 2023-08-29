///////////////////////////////////////////////////////////////////////////////
// Name:        withimages.h
// Purpose:     Interface of  wxWithImages class.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
   A mixin class to be used with other classes that use a wxImageList.

   This class is used by classes such as wxNotebook and wxTreeCtrl, that use
   image indices to specify the icons used for their items (page icons for the
   former or the items inside the control for the latter).

   The icon index can either be a special value NO_IMAGE to indicate that an
   item doesn't use an image at all or a small positive integer to specify the
   index of the icon in the list of images maintained by this class. Note that
   for many controls, either none of the items should have an icon or all of
   them should have one, i.e. mixing the items with and without an icon doesn't
   always work well and may result in less than ideal appearance.

   To initialize the list of icons used, call SetImages() method passing it a
   vector of wxBitmapBundle objects which can, in the simplest case, be just
   wxBitmap or wxIcon objects -- however, as always with wxBitmapBundle, either
   more than one bitmap or icon needs to be specified or the bitmap bundle
   needs to be created from SVG to obtain better appearance in high DPI.

   Alternative, traditional API which was the only one available until
   wxWidgets 3.1.6, is based on the use of wxImageList class. To use it, you
   need to create an object of this class and then call either
   AssignImageList() to set this image list and give the control its ownership
   or SetImageList() to retain the ownership of the image list, which can be
   useful if the same image list is shared by multiple controls, but requires
   deleting the image list later.

   @note wxImageList-based API is not formally deprecated, but its use is
         discouraged because it is more complicated than simply providing a
         vector of bitmaps and it doesn't allow specifying multiple images or
         using SVG, which is required for good high DPI support. Please don't
         use AssignImageList() and SetImageList() in the new code and use
         SetImages() instead.
*/
class wxWithImages
{
public:
    enum
    {
        NO_IMAGE = -1
    };

    wxWithImages();
    virtual ~wxWithImages();

    /**
        Return the number of images in this control.

        The returned value may be 0 if there are no images associated with the
        control.

        @see HasImages()

        @since 3.1.6
     */
    int GetImageCount() const;

    /**
        Return true if the control has any images associated with it.

        @see GetImageCount()

        @since 3.1.6
     */
    bool HasImages() const;

    /**
        Set the images to use for the items in the control.

        This function allows to specify the images to use in multiple different
        resolutions, letting the control to select the appropriate one for its
        DPI scaling. For this reason, it should be preferred to using the
        functions taking wxImageList, which has a fixed size, in the new code.

        @param images Non empty vector of bitmap bundles. Valid image indexes
            for the items in this control are determined by the size of this
            vector.

        @since 3.1.6
     */
    void SetImages(const wxVector<wxBitmapBundle>& images);

    /**
        Sets the image list for the page control and takes ownership of the list.

        This function exists for compatibility only, please use SetImages() in
        the new code.

        @see wxImageList, SetImageList()
    */
    void AssignImageList(wxImageList* imageList);

    /**
       Sets the image list to use. It does not take ownership of the image
       list, you must delete it yourself.

       This function exists for compatibility only, please use SetImages() in
       the new code.

       @see wxImageList, AssignImageList()
    */
    virtual void SetImageList(wxImageList* imageList);

    /**
        Returns the associated image list, may be @NULL.

        Note that the new code should use GetUpdatedImageListFor() instead.

        @see wxImageList, SetImageList()
    */
    wxImageList* GetImageList() const;

    /**
        Returns the image list updated to reflect the DPI scaling used for the
        given window if possible.

        If SetImages() has been called, this function creates the image list
        containing the images using the DPI scaling in effect for the provided
        @a win, which must be valid.

        Otherwise it behaves as GetImageList(), i.e. returns the image list
        previously set using SetImageList() or AssignImageList(), and just
        returns @NULL if none of them had been called.

        @return Possibly null pointer owned by this object, i.e. which must @e
            not be deleted by the caller.

        @since 3.1.6
     */
    wxImageList* GetUpdatedImageListFor(wxWindow* win);

protected:
    /**
       Return true if we have a valid image list.
    */
    bool HasImageList() const;

    /**
       Return the image with the given index from the image list.

       If there is no image list or if index == NO_IMAGE, silently returns
       wxNullIcon.
    */
    wxIcon GetImage(int iconIndex) const;
};
