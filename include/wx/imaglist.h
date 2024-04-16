/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imaglist.h
// Purpose:     wxImageList base header
// Author:      Julian Smart
// Created:
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGLIST_H_BASE_
#define _WX_IMAGLIST_H_BASE_

#include "wx/defs.h"

#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxIcon;
class WXDLLIMPEXP_FWD_CORE wxColour;


/*
 * wxImageList is used for wxListCtrl, wxTreeCtrl. These controls refer to
 * images for their items by an index into an image list.
 * A wxImageList is capable of creating images with optional masks from
 * a variety of sources - a single bitmap plus a colour to indicate the mask,
 * two bitmaps, or an icon.
 *
 * Image lists can also create and draw images used for drag and drop functionality.
 * This is not yet implemented in wxImageList. We need to discuss a generic API
 * for doing drag and drop.
 * See below for candidate functions and an explanation of how they might be
 * used.
 */

// Flag values for Set/GetImageList
enum
{
    wxIMAGE_LIST_NORMAL, // Normal icons
    wxIMAGE_LIST_SMALL,  // Small icons
    wxIMAGE_LIST_STATE   // State icons: unimplemented (see WIN32 documentation)
};

// Flags for Draw
#define wxIMAGELIST_DRAW_NORMAL         0x0001
#define wxIMAGELIST_DRAW_TRANSPARENT    0x0002
#define wxIMAGELIST_DRAW_SELECTED       0x0004
#define wxIMAGELIST_DRAW_FOCUSED        0x0008

// Define the interface of platform-specific wxImageList class.
class WXDLLIMPEXP_CORE wxImageListBase : public wxObject
{
public:
    /*
        This class should provide default ctor as well as the following ctor:

        wxImageList(int width, int height, bool mask = true, int initialCount = 1)

        and Create() member function taking the same parameters and returning
        bool.
     */

    virtual void Destroy() = 0;

    // Returns the size the image list was created with.
    wxSize GetSize() const { return m_size; }

    virtual int GetImageCount() const = 0;
    virtual bool GetSize(int index, int &width, int &height) const = 0;

    virtual int Add(const wxBitmap& bitmap) = 0;
    virtual int Add(const wxBitmap& bitmap, const wxBitmap& mask) = 0;
    virtual int Add(const wxBitmap& bitmap, const wxColour& maskColour) = 0;

    virtual bool Replace(int index,
                         const wxBitmap& bitmap,
                         const wxBitmap& mask = wxNullBitmap) = 0;
    virtual bool Remove(int index) = 0;
    virtual bool RemoveAll() = 0;

    virtual bool Draw(int index, wxDC& dc, int x, int y,
                      int flags = wxIMAGELIST_DRAW_NORMAL,
                      bool solidBackground = false) = 0;

    virtual wxBitmap GetBitmap(int index) const = 0;
    virtual wxIcon GetIcon(int index) const = 0;

protected:
    // Size of a single bitmap in the list in physical pixels.
    wxSize m_size;

    bool m_useMask = false;
};

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/imaglist.h"
    #define wxHAS_NATIVE_IMAGELIST
#else
    #include "wx/generic/imaglist.h"
#endif

#endif // _WX_IMAGLIST_H_BASE_
