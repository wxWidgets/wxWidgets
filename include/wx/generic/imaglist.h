/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/imaglist.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGELISTH_G__
#define __IMAGELISTH_G__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "imaglist.h"
#endif

#include "wx/defs.h"
#include "wx/list.h"

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxColour;

/*
 * wxImageList is used for wxListCtrl, wxTreeCtrl. These controls refer to
 * images for their items by an index into an image list.
 * A wxImageList is capable of creating images with optional masks from
 * a variety of sources - a single bitmap plus a colour to indicate the mask,
 * two bitmaps, or an icon.
 *
 * Image lists can also create and draw images used for drag and drop functionality.
 * This is not yet implemented in wxImageList. We need to discuss a generic API
 * for doing drag and drop and see whether it ties in with the Win95 view of it.
 * See below for candidate functions and an explanation of how they might be
 * used.
 */

#if !defined(wxIMAGELIST_DRAW_NORMAL)

// Flags for Draw
#define wxIMAGELIST_DRAW_NORMAL         0x0001
#define wxIMAGELIST_DRAW_TRANSPARENT    0x0002
#define wxIMAGELIST_DRAW_SELECTED       0x0004
#define wxIMAGELIST_DRAW_FOCUSED        0x0008

// Flag values for Set/GetImageList
enum {
    wxIMAGE_LIST_NORMAL, // Normal icons
    wxIMAGE_LIST_SMALL,  // Small icons
    wxIMAGE_LIST_STATE   // State icons: unimplemented (see WIN32 documentation)
};

#endif

class WXDLLEXPORT wxGenericImageList: public wxObject
{
public:
    wxGenericImageList() { m_width = m_height = 0; }
    wxGenericImageList( int width, int height, bool mask = true, int initialCount = 1 );
    ~wxGenericImageList();
    bool Create( int width, int height, bool mask = true, int initialCount = 1 );
    bool Create();

    virtual int GetImageCount() const;
    virtual bool GetSize( int index, int &width, int &height ) const;

    int Add( const wxBitmap& bitmap );
    int Add( const wxBitmap& bitmap, const wxBitmap& mask );
    int Add( const wxBitmap& bitmap, const wxColour& maskColour );
    const wxBitmap *GetBitmap(int index) const;
    bool Replace( int index, const wxBitmap &bitmap );
    bool Remove( int index );
    bool RemoveAll();

    virtual bool Draw(int index, wxDC& dc, int x, int y,
              int flags = wxIMAGELIST_DRAW_NORMAL,
              bool solidBackground = false);

private:
    wxList  m_images;

    int     m_width;
    int     m_height;

    DECLARE_DYNAMIC_CLASS(wxGenericImageList)
};

#if !defined(__WXMSW__) || defined(__WXUNIVERSAL__)
/*
 * wxImageList has to be a real class or we have problems with
 * the run-time information.
 */

class WXDLLEXPORT wxImageList: public wxGenericImageList
{
    DECLARE_DYNAMIC_CLASS(wxImageList)

public:
    wxImageList() {}

    wxImageList( int width, int height, bool mask = true, int initialCount = 1 )
        : wxGenericImageList(width, height, mask, initialCount)
    {
    }
};
#endif // !__WXMSW__ || __WXUNIVERSAL__

#endif  // __IMAGELISTH_G__

