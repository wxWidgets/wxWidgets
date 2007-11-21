/////////////////////////////////////////////////////////////////////////////
// Name:        bardragpl.h
// Purpose:     cbBarDragPlugin class
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __BARDRAGPL_G__
#define __BARDRAGPL_G__

#include "wx/fl/controlbar.h"
#include "wx/fl/toolwnd.h"

/*
Plugin class implementing bar dragging.
*/

class WXDLLIMPEXP_FL cbBarDragPlugin : public cbPluginBase
{
    DECLARE_DYNAMIC_CLASS( cbBarDragPlugin )
protected:

    // plugin is active only in bar-dragging state
    bool           mBarDragStarted;
    bool           mCanStick;          // flag used to prevent "bouncing" of hint-rectangle
    wxScreenDC*    mpScrDc;            // created while tracking hint-rect
    wxCursor*      mpCurCursor;

    // rectangle shows the position/dimensions of the bar,
    // if it would be docked now

    wxRect         mPrevHintRect;
    wxRect         mHintRect;


    int            mMouseInRectX;
    int            mMouseInRectY;

    cbDockPane*    mpSrcPane;          // pane, from which the bar was originally taken
    int            mBarWidthInSrcPane;

    cbDockPane*    mpCurPane;

    cbBarInfo*     mpDraggedBar;       // bar, which is being dragged
    bool           mBarWasFloating;
    wxRect         mFloatedBarBounds;

public: /*** public properties ***/

    int            mInClientHintBorder; // when hint-rect moves within client window area,
                                        // the thicker rectangle is drawn using hatched brush,
                                        // the default border width for this rectangle is 8 pix.

protected:

        // Internal implementation function.
    void AdjustHintRect( wxPoint& mousePos );

        // Internal implementation function.
    void ClipRectInFrame( wxRect& rect );

        // Internal implementation function.
    void ClipPosInFrame( wxPoint& pos );

        // Internal implementation function. Finds the pane
        // under the specified rectangle.
    cbDockPane* HitTestPanes( wxRect& rect );

        // Internal implementation function. Finds the pane
        // under the specified point.
    cbDockPane* HitTestPanes( wxPoint& pos );

        // Internal implementation function.
    bool HitsPane( cbDockPane* pPane, wxRect& rect );

        // Internal implementation function.
    void CalcOnScreenDims( wxRect& rect );

        // Internal implementation function.
    int  GetDistanceToPane( cbDockPane* pPane, wxPoint& mousePos );

        // Internal implementation function.
    bool IsInOtherPane ( wxPoint& mousePos );

        // Internal implementation function.
    bool IsInClientArea( wxPoint& mousePos );

        // Internal implementation function.
    bool IsInClientArea( wxRect& rect );

        // Internal implementation function.
    void StickToPane( cbDockPane* pPane, wxPoint& mousePos );

        // Internal implementation function.
    void UnstickFromPane( cbDockPane* pPane, wxPoint& mousePos );

        // Internal implementation function.
    int GetBarWidthInPane( cbDockPane* pPane );

        // Internal implementation function.
    int GetBarHeightInPane( cbDockPane* pPane );

        // on-screen hint-tracking related methods

        // Internal implementation function.
        // Start showing a visual hint while dragging.
    void StartTracking();

        // Internal implementation function.
        // Draw the visual hint while dragging.
    void DrawHintRect ( wxRect& rect, bool isInClientRect);

        // Internal implementation function.
        // Erase the visual hint while dragging.
    void EraseHintRect( wxRect& rect, bool isInClientRect);

        // Internal implementation function.
        // Stop showing the visual hint while dragging.
    void FinishTracking();

        // Internal implementation function.
        // Draw the hint rectangle.
    void DoDrawHintRect( wxRect& rect, bool isInClientRect);

        // Internal implementation function.
        // Converts the given rectangle from window to screen coordinates.
    void RectToScr( wxRect& frameRect, wxRect& scrRect );

        // Internal implementation function.
        // Show the hint; called within OnMouseMove.
    void ShowHint( bool prevWasInClient );

public:
        // Default constructor.
    cbBarDragPlugin(void);

        // Constructor taking a parent frame, and flag. See cbPluginBase.
    cbBarDragPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Destructor.
    virtual ~cbBarDragPlugin();

        // Handler for plugin event.
    void OnMouseMove( cbMotionEvent& event );

        // Handler for plugin event.
    void OnLButtonUp( cbLeftUpEvent& event );

        // Handler for plugin event.
    void OnLButtonDown( cbLeftDownEvent& event );

        // Handler for plugin event.
    void OnLDblClick( cbLeftDClickEvent& event );

        // Handles event, which originates from itself.
    void OnDrawHintRect( cbDrawHintRectEvent& event );

        // Handler for plugin event.
    void OnStartBarDragging( cbStartBarDraggingEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __BARDRAGPL_G__ */

