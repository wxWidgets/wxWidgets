/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dragimgg.h
// Purpose:     wxDragImage class: a kind of a cursor, that can cope
//              with more sophisticated images
// Author:      Julian Smart
// Modified by:
// Created:     29/2/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DRAGIMGG_H_
#define _WX_DRAGIMGG_H_

#ifdef __GNUG__
#pragma interface "dragimgg.h"
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
 * wxGenericDragImage
 */

class WXDLLEXPORT wxGenericDragImage: public wxObject
{
public:

    // Ctors & dtor
    ////////////////////////////////////////////////////////////////////////////

    wxGenericDragImage(const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        Init();
        Create(cursor, hotspot);
    }
    wxGenericDragImage(const wxBitmap& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        Init();

        Create(image, cursor, hotspot);
    }
    wxGenericDragImage(const wxIcon& image, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        Init();

        Create(image, cursor, hotspot);
    }
    wxGenericDragImage(const wxString& str, const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0))
    {
        Init();

        Create(str, cursor, hotspot);
    }
    wxGenericDragImage(const wxTreeCtrl& treeCtrl, wxTreeItemId& id)
    {
        Init();

        Create(treeCtrl, id);
    }
    wxGenericDragImage(const wxListCtrl& listCtrl, long id)
    {
        Init();

        Create(listCtrl, id);
    }
    ~wxGenericDragImage();

    // Attributes
    ////////////////////////////////////////////////////////////////////////////

    // Operations
    ////////////////////////////////////////////////////////////////////////////

    // Create a drag image with a virtual image (need to override DoDrawImage, GetImageRect)
    bool Create(const wxCursor& cursor = wxNullCursor, const wxPoint& hotspot = wxPoint(0, 0));

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
    bool BeginDrag(const wxPoint& hotspot, wxWindow* window, bool fullScreen = FALSE, wxRect* rect = (wxRect*) NULL);

    // Begin drag. hotspot is the location of the drag position relative to the upper-left
    // corner of the image. This is full screen only. fullScreenRect gives the
    // position of the window on the screen, to restrict the drag to.
    bool BeginDrag(const wxPoint& hotspot, wxWindow* window, wxWindow* fullScreenRect);

    // End drag
    bool EndDrag();

    // Move the image: call from OnMouseMove. Pt is in window client coordinates if window
    // is non-NULL, or in screen coordinates if NULL.
    bool Move(const wxPoint& pt);

    // Show the image
    bool Show();

    // Hide the image
    bool Hide();

    // Implementation
    ////////////////////////////////////////////////////////////////////////////

    void Init();

    // Override this if you are using a virtual image (drawing your own image)
    virtual wxRect GetImageRect(const wxPoint& pos) const;

    // Override this if you are using a virtual image (drawing your own image)
    virtual bool DoDrawImage(wxDC& dc, const wxPoint& pos) const;

    // Erase and redraw simultaneously if possible
    virtual bool RedrawImage(const wxPoint& oldPos, const wxPoint& newPos, bool eraseOld, bool drawNew);

protected:
    wxBitmap        m_bitmap;
    wxIcon          m_icon;
    wxCursor        m_cursor;
    wxCursor        m_oldCursor;
    wxPoint         m_hotspot;
    wxPoint         m_position;
    bool            m_isDirty;
    bool            m_isShown;
    wxWindow*       m_window;
    wxDC*           m_windowDC;

    // Stores the window contents while we're dragging the image around
    wxBitmap        m_backingBitmap;
    // A temporary bitmap for repairing/redrawing
    wxBitmap        m_repairBitmap;

    wxRect          m_boundingRect;
    bool            m_fullScreen;

private:
    DECLARE_DYNAMIC_CLASS(wxGenericDragImage)
};

#endif
    // _WX_DRAGIMGG_H_
