/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dragimag.h
// Purpose:     wxDragImage class: a kind of a cursor, that can cope
//              with more sophisticated images
// Author:      Julian Smart
// Modified by:
// Created:     08/04/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAGIMAG_H_
#define _WX_DRAGIMAG_H_

#ifdef __GNUG__
#pragma interface "dragimag.h"
#endif

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/treectrl.h"
#include "wx/listctrl.h"

/*
  To use this class, create a wxDragImage when you start dragging, for example:

  void MyTreeCtrl::OnBeginDrag(wxTreeEvent& event)
  {
#ifdef __WXMSW__
    ::UpdateWindow((HWND) GetHWND()); // We need to implement this in wxWindows
#endif

    CaptureMouse();

    m_dragImage = new wxDragImage(* this, itemId);
    m_dragImage->BeginDrag(wxPoint(0, 0), this);
    m_dragImage->Move(pt, this);
    m_dragImage->Show(this);
    ...
  }

  In your OnMouseMove function, hide the image, do any display updating required,
  then move and show the image again:

  void MyTreeCtrl::OnMouseMove(wxMouseEvent& event)
  {
    if (m_dragMode == MY_TREE_DRAG_NONE)
    {
        event.Skip();
        return;
    }

    // Prevent screen corruption by hiding the image
    if (m_dragImage)
        m_dragImage->Hide(this);

    // Do some updating of the window, such as highlighting the drop target
    ...

#ifdef __WXMSW__
    if (updateWindow)
        ::UpdateWindow((HWND) GetHWND());
#endif

    // Move and show the image again
    m_dragImage->Move(event.GetPosition(), this);
    m_dragImage->Show(this);
 }

 Eventually we end the drag and delete the drag image.

 void MyTreeCtrl::OnLeftUp(wxMouseEvent& event)
 {
    ...

    // End the drag and delete the drag image
    if (m_dragImage)
    {
        m_dragImage->EndDrag(this);
        delete m_dragImage;
        m_dragImage = NULL;
    }
    ReleaseMouse();
 }
*/

/*
 Notes for Unix version:
 Can we simply use cursors instead, creating a cursor dynamically, setting it into the window
 in BeginDrag, and restoring the old cursor in EndDrag?
 For a really bog-standard implementation, we could simply use a normal dragging cursor
 and ignore the image.
*/

/*
 * wxDragImage
 */

class WXDLLEXPORT wxDragImage: public wxObject
{
public:

    // Ctors & dtor
    ////////////////////////////////////////////////////////////////////////////

    wxDragImage();
    wxDragImage(const wxBitmap& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        m_hImageList = 0;
        Create(image, cursor);
    }
    wxDragImage(const wxIcon& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        m_hImageList = 0;
        Create(image, cursor);
    }
    wxDragImage(const wxString& str, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        m_hImageList = 0;
        Create(str, cursor);
    }
    wxDragImage(const wxTreeCtrl& treeCtrl, wxTreeItemId& id)
    {
        m_hImageList = 0;
        Create(treeCtrl, id);
    }
    wxDragImage(const wxListCtrl& listCtrl, long id)
    {
        m_hImageList = 0;
        Create(listCtrl, id);
    }
    ~wxDragImage();

    // Attributes
    ////////////////////////////////////////////////////////////////////////////

    // Operations
    ////////////////////////////////////////////////////////////////////////////

    // Create a drag image from a bitmap and optional cursor
    bool Create(const wxBitmap& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0));

    // Create a drag image from an icon and optional cursor
    bool Create(const wxIcon& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0));

    // Create a drag image from a string and optional cursor
    bool Create(const wxString& str, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0));

    // Create a drag image for the given tree control item
    bool Create(const wxTreeCtrl& treeCtrl, wxTreeItemId& id);

    // Create a drag image for the given list control item
    bool Create(const wxListCtrl& listCtrl, long id);

    // Begin drag. hotspot is the location of the drag position relative to the upper-left
    // corner of the image.
    bool BeginDrag(const wxPoint& hotspot, wxWindow* window);

    // End drag
    bool EndDrag(wxWindow* window);

    // Move the image: call from OnMouseMove. Pt is in window client coordinates if window
    // is non-NULL, or in screen coordinates if NULL.
    bool Move(const wxPoint& pt, wxWindow* window);

    // Show the image
    bool Show(wxWindow* window);

    // Hide the image
    bool Hide(wxWindow* window);

    // Implementation
    ////////////////////////////////////////////////////////////////////////////

    // Returns the native image list handle
    WXHIMAGELIST GetHIMAGELIST() const { return m_hImageList; }

protected:
    WXHIMAGELIST    m_hImageList;
    wxCursor        m_cursor;
    wxPoint         m_hotspot;
    wxPoint         m_position;

private:
    DECLARE_DYNAMIC_CLASS(wxDragImage)
};

#endif
    // _WX_DRAGIMAG_H_
