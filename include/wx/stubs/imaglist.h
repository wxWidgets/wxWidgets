/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.h
// Purpose:     wxImageList class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGLIST_H_
#define _WX_IMAGLIST_H_

#ifdef __GNUG__
#pragma interface "imaglist.h"
#endif

#include "wx/bitmap.h"

/*
 * wxImageList is used for wxListCtrl, wxTreeCtrl. These controls refer to
 * images for their items by an index into an image list.
 * A wxImageList is capable of creating images with optional masks from
 * a variety of sources - a single bitmap plus a colour to indicate the mask,
 * two bitmaps, or an icon.
 *
 */

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

// Eventually we'll make this a reference-counted wxGDIObject. For
// now, the app must take care of ownership issues. That is, the
// image lists must be explicitly deleted after the control(s) that uses them
// is (are) deleted, or when the app exits.
class WXDLLEXPORT wxImageList: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxImageList)
 public:
  /*
   * Public interface
   */

  wxImageList();

  // Creates an image list.
  // Specify the width and height of the images in the list,
  // whether there are masks associated with them (e.g. if creating images
  // from icons), and the initial size of the list.
  inline wxImageList(int width, int height, bool mask = TRUE, int initialCount = 1)
  {
    Create(width, height, mask, initialCount);
  }
  ~wxImageList();


  // Attributes
  ////////////////////////////////////////////////////////////////////////////

  // Returns the number of images in the image list.
  int GetImageCount() const;

  // Operations
  ////////////////////////////////////////////////////////////////////////////

  // Creates an image list
  // width, height specify the size of the images in the list (all the same).
  // mask specifies whether the images have masks or not.
  // initialNumber is the initial number of images to reserve.
  bool Create(int width, int height, bool mask = TRUE, int initialNumber = 1);

  // Adds a bitmap, and optionally a mask bitmap.
  // Note that wxImageList creates *new* bitmaps, so you may delete
  // 'bitmap' and 'mask' after calling Add.
  int Add(const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);

  // Adds a bitmap, using the specified colour to create the mask bitmap
  // Note that wxImageList creates *new* bitmaps, so you may delete
  // 'bitmap' after calling Add.
  int Add(const wxBitmap& bitmap, const wxColour& maskColour);

  // Adds a bitmap and mask from an icon.
  int Add(const wxIcon& icon);

  // Replaces a bitmap, optionally passing a mask bitmap.
  // Note that wxImageList creates new bitmaps, so you may delete
  // 'bitmap' and 'mask' after calling Replace.
  bool Replace(int index, const wxBitmap& bitmap, const wxBitmap& mask = wxNullBitmap);

/* Not supported by Win95
  // Replacing a bitmap, using the specified colour to create the mask bitmap
  // Note that wxImageList creates new bitmaps, so you may delete
  // 'bitmap'.
  bool Replace(int index, const wxBitmap& bitmap, const wxColour& maskColour);
*/

  // Replaces a bitmap and mask from an icon.
  // You can delete 'icon' after calling Replace.
  bool Replace(int index, const wxIcon& icon);

  // Removes the image at the given index.
  bool Remove(int index);

  // Remove all images
  bool RemoveAll();

  // Draws the given image on a dc at the specified position.
  // If 'solidBackground' is TRUE, Draw sets the image list background
  // colour to the background colour of the wxDC, to speed up
  // drawing by eliminating masked drawing where possible.
  bool Draw(int index, wxDC& dc, int x, int y,
    int flags = wxIMAGELIST_DRAW_NORMAL, bool solidBackground = FALSE);

/* TODO (optional?)
  wxIcon *MakeIcon(int index);
*/

/* TODO
  // Implementation
  ////////////////////////////////////////////////////////////////////////////

  // Returns the native image list handle
  inline WXHIMAGELIST GetHIMAGELIST() const { return m_hImageList; }

protected:
  WXHIMAGELIST m_hImageList;
*/

};

#endif
    // _WX_IMAGLIST_H_
