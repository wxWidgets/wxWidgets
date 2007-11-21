/////////////////////////////////////////////////////////////////////////////
// Name:        panedrawpl.h
// Purpose:     cbPaneDrawPlugin class header.
// Author:      Aleksandras Gluchovas <mailto:alex@soften.ktu.lt>
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __PANEDRAWPL_G__
#define __PANEDRAWPL_G__

#include "wx/fl/controlbar.h"

/*
Simple but all-in-one plugin implementation. Resembles the look and feel of
MFC control-bars. The class handles painting of the pane and the items in it;
it generates bar/layout customization events, when the user right-clicks the bar/pane.
Hooking an instance of this and row-layout plugins for each pane
would be enough for the frame layout to function properly
(they are plugged in automatically by the wxFrameLayout class).
*/

class WXDLLIMPEXP_FL cbPaneDrawPlugin : public cbPluginBase
{
public:
    DECLARE_DYNAMIC_CLASS( cbPaneDrawPlugin )
protected:

    // resizing bars/rows state variables
    bool           mResizeStarted;
    bool           mResizeCursorOn;
    wxPoint        mDragOrigin;

    cbBarInfo*     mpDraggedBar; // also used when in bar-drag action
    cbRowInfo*     mpResizedRow;

    bool           mRowHandleHitted;
    bool           mIsUpperHandle;
    bool           mBarHandleHitted;
    bool           mIsLeftHandle;
    bool           mBarContentHitted;

    // contstraints for dragging the handle
    wxRect         mHandleDragArea;
    bool           mHandleIsVertical;
    int            mHandleOfs;
    int            mDraggedDelta;
    wxPoint        mPrevPos;

    // used for handling, start-draw-in-area events
    wxClientDC*    mpClntDc;

    cbDockPane*    mpPane; // is set up temorary short-cut, while handling event

protected:
        // Internal helper: draws the dragged handle.
    void DrawDraggedHandle( const wxPoint& pos, cbDockPane& pane );

        // Internal helper: draws the pane shading.
    virtual void DrawPaneShade( wxDC& dc, int alignment );

        // Internal helper: draws the pane shading for a row.
    virtual void DrawPaneShadeForRow( cbRowInfo* pRow, wxDC& dc );

        // Internal helper: draws the upper row handle.
    virtual void DrawUpperRowHandle( cbRowInfo* pRow, wxDC& dc );

        // Internal helper: draws the lower row handle.
    virtual void DrawLowerRowHandle( cbRowInfo* pRow, wxDC& dc );

        // Internal helper: draws the upper row shading.
    virtual void DrawUpperRowShades( cbRowInfo* pRow, wxDC& dc, int level );

        // Internal helper: draws the lower row shading.
    virtual void DrawLowerRowShades( cbRowInfo* pRow, wxDC& dc, int level );

        // Internal helper: draws the inner bar shading.
    virtual void DrawBarInnerShadeRect( cbBarInfo* pBar, wxDC& dc );

        // Internal helper: draws shading.
    virtual void DrawShade( int level, wxRect& rect, int alignment, wxDC& dc );

        // Internal helper: draws shading.
    virtual void DrawShade1( int level, wxRect& rect, int alignment, wxDC& dc );

        // Internal helper: sets a light pixel at the given location.
    inline void SetLightPixel( int x, int y, wxDC& dc );

        // Internal helper: sets a dark pixel at the given location.
    inline void SetDarkPixel ( int x, int y, wxDC& dc );

public:
        // Default constructor.
    cbPaneDrawPlugin();

        // Constructor taking frame layout pane and a pane mask.
    cbPaneDrawPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Destructor.
    virtual ~cbPaneDrawPlugin();

        // Clone function, returning a new instance of this class.
    virtual cbPluginBase* Clone() { return new cbPaneDrawPlugin(0,0); }

        // Handler for left mouse button down events.
    void OnLButtonDown( cbLeftDownEvent&   event );

        // Handler for left double-click mouse button down events.
    void OnLDblClick  ( cbLeftDClickEvent& event );

        // Handler for left mouse button up events.
    void OnLButtonUp  ( cbLeftUpEvent&     event );

        // Handler for right mouse button up events.
    void OnRButtonUp  ( cbRightUpEvent&    event );

        // Handler for mouse move events.
    void OnMouseMove  ( cbMotionEvent&     event );

        // Handler for draw pane background events.
    void OnDrawPaneBackground ( cbDrawPaneBkGroundEvent& event );

        // Handler for draw pane decoration events.
    void OnDrawPaneDecorations( cbDrawPaneDecorEvent&    event );

        // Handler for draw row decoration events.
    void OnDrawRowDecorations ( cbDrawRowDecorEvent&     event );

        // Handler for draw row handles events.
    void OnDrawRowHandles     ( cbDrawRowHandlesEvent&   event );

        // Handler for draw row background events.
    void OnDrawRowBackground  ( cbDrawRowBkGroundEvent&  event );

        // Handler for bar size events.
    void OnSizeBarWindow      ( cbSizeBarWndEvent&       event );

        // Handler for draw bar decorations events.
    void OnDrawBarDecorations ( cbDrawBarDecorEvent&     event );

        // Handler for draw bar handles events.
    void OnDrawBarHandles     ( cbDrawBarHandlesEvent&   event );

        // Handler for start draw in area events.
    void OnStartDrawInArea    ( cbStartDrawInAreaEvent&  event );

        // Handler for finish draw in area events.
    void OnFinishDrawInArea   ( cbFinishDrawInAreaEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __PANEDRAWPL_G__ */

