///////////////////////////////////////////////////////////////////////////////
// Name:        withimages.h
// Purpose:     Interface of  wxWithImages class.
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
   A mixin class to be used with other classes that use a wxImageList.
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
        Returns the associated image list, may be NULL.

        @see wxImageList, SetImageList()
    */
    wxImageList* GetImageList() const;

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
