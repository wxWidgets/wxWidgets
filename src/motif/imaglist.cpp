/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.cpp
// Purpose:     wxImageList. You may wish to use the generic version.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imaglist.h"
#endif

#include "wx/motif/imaglist.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxImageList, wxObject)
#endif

wxImageList::wxImageList()
{
    // TODO: init image list handle, if any
}

wxImageList::~wxImageList()
{
    // TODO: destroy image list handle, if any
}


// Attributes
////////////////////////////////////////////////////////////////////////////

// Returns the number of images in the image list.
int wxImageList::GetImageCount() const
{
    // TODO
	return 0;
}

// Operations
////////////////////////////////////////////////////////////////////////////

// Creates an image list
bool wxImageList::Create(int width, int height, bool mask, int initial)
{
    // TODO
    return FALSE;
}

// Adds a bitmap, and optionally a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
int wxImageList::Add(const wxBitmap& bitmap, const wxBitmap& mask)
{
    // TODO
    return 0;
}

// Adds a bitmap, using the specified colour to create the mask bitmap
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap'.
int wxImageList::Add(const wxBitmap& bitmap, const wxColour& maskColour)
{
    // TODO
    return 0;
}

// Adds a bitmap and mask from an icon.
int wxImageList::Add(const wxIcon& icon)
{
    // TODO
    return 0;
}

// Replaces a bitmap, optionally passing a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
bool wxImageList::Replace(int index, const wxBitmap& bitmap, const wxBitmap& mask)
{
    // TODO
    return 0;
}

// Replaces a bitmap and mask from an icon.
bool wxImageList::Replace(int index, const wxIcon& icon)
{
    // TODO
    return 0;
}

// Removes the image at the given index.
bool wxImageList::Remove(int index)
{
    // TODO
    return FALSE;
}

// Remove all images
bool wxImageList::RemoveAll()
{
    // TODO
    return FALSE;
}

// Draws the given image on a dc at the specified position.
// If 'solidBackground' is TRUE, Draw sets the image list background
// colour to the background colour of the wxDC, to speed up
// drawing by eliminating masked drawing where possible.
bool wxImageList::Draw(int index, wxDC& dc, int x, int y,
    int flags, bool solidBackground)
{
    // TODO
    return FALSE;
}

