/////////////////////////////////////////////////////////////////////////////
// Name:        _dragimg.i
// Purpose:     SWIG defs for wxDragImage
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/generic/dragimgg.h>
%}

//---------------------------------------------------------------------------



MustHaveApp(wxGenericDragImage);

%rename (DragImage) wxGenericDragImage;
class wxGenericDragImage : public wxObject
{
public:

    wxGenericDragImage(const wxBitmap& image,
                       const wxCursor& cursor = wxNullCursor);
    
    %RenameCtor(DragIcon, wxGenericDragImage(const wxIcon& image,
                                        const wxCursor& cursor = wxNullCursor));

    %RenameCtor(DragString, wxGenericDragImage(const wxString& str,
                                          const wxCursor& cursor = wxNullCursor));

    %RenameCtor(DragTreeItem, wxGenericDragImage(const wxPyTreeCtrl& treeCtrl, wxTreeItemId& id));

    %RenameCtor(DragListItem, wxGenericDragImage(const wxPyListCtrl& listCtrl, long id));

    ~wxGenericDragImage();

    // For efficiency, tell wxGenericDragImage to use a bitmap that's already
    // created (e.g. from last drag)
    void SetBackingBitmap(wxBitmap* bitmap);

    // Begin drag. hotspot is the location of the drag position relative to the upper-left
    // corner of the image.
    bool BeginDrag(const wxPoint& hotspot, wxWindow* window,
                   bool fullScreen = false, wxRect* rect = NULL);

    // Begin drag. hotspot is the location of the drag position relative to the upper-left
    // corner of the image. This is full screen only. fullScreenRect gives the
    // position of the window on the screen, to restrict the drag to.
    %Rename(BeginDragBounded,  bool, BeginDrag(const wxPoint& hotspot, wxWindow* window,
                                           wxWindow* boundingWindow));


    // End drag
    bool EndDrag();

    // Move the image: call from OnMouseMove. Pt is in window client coordinates if window
    // is non-NULL, or in screen coordinates if NULL.
    bool Move(const wxPoint& pt);

    
    // Show the image
    bool Show();

    // Hide the image
    bool Hide();


// TODO, make the rest of these overridable
    
    // Override this if you are using a virtual image (drawing your own image)
    virtual wxRect GetImageRect(const wxPoint& pos) const;

    // Override this if you are using a virtual image (drawing your own image)
    virtual bool DoDrawImage(wxDC& dc, const wxPoint& pos) const;

    // Override this if you wish to draw the window contents to the backing bitmap
    // yourself. This can be desirable if you wish to avoid flicker by not having to
    // redraw the window itself before dragging in order to be graphic-minus-dragged-objects.
    // Instead, paint the drag image's backing bitmap to be correct, and leave the window
    // to be updated only when dragging the objects away (thus giving a smoother appearance).
    virtual bool UpdateBackingFromWindow(wxDC& windowDC, wxMemoryDC& destDC,
                                         const wxRect& sourceRect, const wxRect& destRect) const;

    // Erase and redraw simultaneously if possible
    virtual bool RedrawImage(const wxPoint& oldPos, const wxPoint& newPos, bool eraseOld, bool drawNew);

    %property(ImageRect, GetImageRect, doc="See `GetImageRect`");
};


//---------------------------------------------------------------------------
%init %{
    wxPyPtrTypeMap_Add("wxDragImage", "wxGenericDragImage");
%}
//---------------------------------------------------------------------------
