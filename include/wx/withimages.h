///////////////////////////////////////////////////////////////////////////////
// Name:        wx/withimages.h
// Purpose:     Declaration of a simple wxWithImages class.
// Author:      Vadim Zeitlin
// Created:     2011-08-17
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WITHIMAGES_H_
#define _WX_WITHIMAGES_H_

#include "wx/defs.h"
#include "wx/bmpbndl.h"
#include "wx/icon.h"
#include "wx/imaglist.h"
#include "wx/vector.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;

// ----------------------------------------------------------------------------
// wxWithImages: mix-in for classes using indices for image access
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWithImages
{
public:
    enum
    {
        NO_IMAGE = -1
    };

    typedef wxVector<wxBitmapBundle> Images;

    wxWithImages()
    {
        m_imageList = NULL;
        m_ownsImageList = false;
    }

    virtual ~wxWithImages()
    {
        FreeIfNeeded();
    }

    // Return the number of images, possibly 0.
    int GetImageCount() const
    {
        if ( !m_images.empty() )
        {
            // Cast is safe, we don't risk having more than INT_MAX images.
            return static_cast<int>(m_images.size());
        }

        return m_imageList ? m_imageList->GetImageCount() : 0;
    }

    // Return true if we are using any images.
    bool HasImages() const
    {
        // Note that the sole presence of the image list indicates that we're
        // using images, even if it is currently empty, because images can be
        // added to it at any moment (it's common and valid to create an image
        // list and associate it with the control first and fill it later) and
        // it's better to err on the side of having the images and not showing
        // anything if there are really none than decide that we don't have any
        // and not show those that we do have.
        return !m_images.empty() || m_imageList;
    }

    // Sets the images to use.
    //
    // Override OnImagesChanged() in the derived class to update the actually
    // shown images.
    void SetImages(const Images& images)
    {
        m_images = images;

        OnImagesChanged();
    }

    // Sets the image list to use, it is *not* deleted by the control.
    //
    // This function is virtual for compatibility, as it could be overridden in
    // the existing application code, however it should not be overridden in wx
    // itself, where OnImagesChanged() should be overridden instead.
    virtual void SetImageList(wxImageList* imageList)
    {
        FreeIfNeeded();
        m_imageList = imageList;

        OnImagesChanged();
    }

    // As SetImageList() but we will delete the image list ourselves.
    void AssignImageList(wxImageList* imageList)
    {
        SetImageList(imageList);
        m_ownsImageList = true;
    }

    // This function can be used for implementing AssignImageList()-like
    // methods in the classes using this one and tells us to simply take
    // ownership of the image list that we already have.
    //
    // Avoid using it if possible.
    void TakeOwnership() { m_ownsImageList = true; }

    // Get pointer (may be NULL) to the associated image list.
    wxImageList* GetImageList() const { return m_imageList; }

    // This helper function can be used from OnImagesChanged() if the derived
    // class actually needs to use wxImageList: it ensures that m_imageList is
    // updated from m_images, if the latter is not empty, using the images of
    // the appropriate size for the given window, and returns it.
    wxImageList* GetUpdatedImageListFor(wxWindow* win)
    {
        if ( !m_images.empty() )
        {
            // Note that we can't just call AssignImageList() here to avoid
            // infinite recursion.
            FreeIfNeeded();
            m_imageList = wxBitmapBundle::CreateImageList(win, m_images);

            // We always own it as we created it ourselves.
            m_ownsImageList = true;
        }

        return m_imageList;
    }

protected:
    // This function is called when the images associated with the control
    // change, due to either SetImages() or SetImageList() being called.
    //
    // It ought to be pure virtual, but isn't because there could be existing
    // application code inheriting from this class and not overriding it
    // (because this function hadn't existed when this code was written).
    virtual void OnImagesChanged() { }

    // This function can be used as event handle for wxEVT_DPI_CHANGED event
    // and simply calls OnImagesChanged() to refresh the images when it happens.
    void WXHandleDPIChanged(wxDPIChangedEvent& event)
    {
        if ( HasImages() )
            OnImagesChanged();

        event.Skip();
    }


    // Return true if we have a valid image list.
    bool HasImageList() const { return m_imageList != NULL; }

    // Return the image with the given index from the image list.
    //
    // If there is no image list or if index == NO_IMAGE, silently returns
    // wxNullIcon.
    wxIcon GetImage(int iconIndex) const
    {
        return m_imageList && iconIndex != NO_IMAGE
                    ? m_imageList->GetIcon(iconIndex)
                    : wxNullIcon;
    }

    // Return the bitmap bundle for the image with the given index.
    //
    // If index == NO_IMAGE or there are no images at all, returns an empty
    // bundle (but, unlike GetImage() above, asserts if the index is valid but
    // there is no image, as this probably indicates a programming mistake).
    //
    // If there is no bundle, but there is an image list, returns a bundle
    // containing just the bitmap from the image list.
    wxBitmapBundle GetBitmapBundle(int iconIndex) const
    {
        wxBitmapBundle bundle;

        if ( iconIndex != NO_IMAGE )
        {
            if ( !m_images.empty() )
            {
                bundle = m_images.at(iconIndex);
            }
            else if ( m_imageList )
            {
                bundle = m_imageList->GetIcon(iconIndex);
            }
            else
            {
                wxFAIL_MSG
                (
                    "Image index specified, but there are no images.\n"
                    "\n"
                    "Did you forget to call SetImages()?"
                );
            }
        }

        return bundle;
    }

    // Accessor to the images for the derived classes: this is only useful when
    // delegating SetImages() to another object.
    const Images& GetImages() const { return m_images; }

private:
    // Free the image list if necessary, i.e. if we own it.
    void FreeIfNeeded()
    {
        if ( m_ownsImageList )
        {
            delete m_imageList;
            m_imageList = NULL;

            // We don't own it any more.
            m_ownsImageList = false;
        }
    }


    // The images we use: if this vector is not empty, m_imageList is not used.
    Images m_images;

    // The associated image list or NULL.
    wxImageList* m_imageList;

    // False by default, if true then we delete m_imageList.
    bool m_ownsImageList;

    wxDECLARE_NO_COPY_CLASS(wxWithImages);
};

#endif // _WX_WITHIMAGES_H_
