/////////////////////////////////////////////////////////////////////////////
// Name:        imaglist.h
// Purpose:
// Author:      Robert Roebling, Stefan Csomor
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGLIST_H_
#define _WX_IMAGLIST_H_

#include "wx/defs.h"
#include "wx/list.h"
#include "wx/icon.h"

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

class WXDLLEXPORT wxImageList: public wxObject
{
public:
    wxImageList() { m_width = m_height = 0; }
    wxImageList( int width, int height, bool mask = true, int initialCount = 1 );
    virtual ~wxImageList();
    bool Create( int width, int height, bool mask = true, int initialCount = 1 );
    bool Create();
    
    virtual int GetImageCount() const;
    virtual bool GetSize( int index, int &width, int &height ) const;
    
    int Add( const wxIcon& bitmap );
    int Add( const wxBitmap& bitmap );
    int Add( const wxBitmap& bitmap, const wxBitmap& mask );
    int Add( const wxBitmap& bitmap, const wxColour& maskColour );
    wxBitmap GetBitmap(int index) const;
    wxIcon GetIcon(int index) const;
    bool Replace( int index, const wxIcon &bitmap );
    bool Replace( int index, const wxBitmap &bitmap );
    bool Replace( int index, const wxBitmap &bitmap, const wxBitmap &mask );
    bool Remove( int index );
    bool RemoveAll();
    
    virtual bool Draw(int index, wxDC& dc, int x, int y,
                      int flags = wxIMAGELIST_DRAW_NORMAL,
                      bool solidBackground = false);
    
private:
    wxList  m_images;
    
    int     m_width;
    int     m_height;
    
    DECLARE_DYNAMIC_CLASS(wxImageList)
};

#endif  // _WX_IMAGLIST_H_

