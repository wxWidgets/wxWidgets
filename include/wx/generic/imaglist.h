/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGELISTH_G__
#define __IMAGELISTH_G__

#ifdef __GNUG__
#pragma interface "imaglist.h"
#endif

#include "wx/defs.h"
#include "wx/gdicmn.h"
#include "wx/bitmap.h"
#include "wx/dc.h"

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

// Flags for Draw
#define wxIMAGELIST_DRAW_NORMAL         0x0001
#define wxIMAGELIST_DRAW_TRANSPARENT    0x0002
#define wxIMAGELIST_DRAW_SELECTED       0x0004
#define wxIMAGELIST_DRAW_FOCUSED        0x0008

class wxImageList: public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxImageList)
   
  public:

    wxImageList::wxImageList() { }
    wxImageList(int width, int height, bool mask = TRUE, int initialCount = 1);
    ~wxImageList();
    bool Create();
    int GetImageCount() const;
    int Add( const wxBitmap &bitmap );
    bool Replace( int index, const wxBitmap &bitmap );
    bool Remove( int index );
    bool RemoveAll();
    bool GetSize( int index, int &width, int &height ) const;
    bool Draw(int index, wxDC& dc, int x, int y,
      int flags = wxIMAGELIST_DRAW_NORMAL, const bool solidBackground = FALSE );
    
  private:

    wxList  m_images;
};

#endif  // __IMAGELISTH_G__

