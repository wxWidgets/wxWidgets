/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __BARDRAGPL_G__
#define __BARDRAGPL_G__

#include "controlbar.h"
#include "toolwnd.h"

class cbBarDragPlugin : public cbPluginBase
{
	DECLARE_DYNAMIC_CLASS( cbBarDragPlugin )
protected:

	// plugin is active only in bar-dragging state
	bool           mBarDragStarted;
	bool           mCanStick;          // flag used to prevent "bouncing" of hint-rectangle
	wxScreenDC*    mpScrDc;            // created while tracking hint-rect
	wxCursor*      mpCurCursor;

	// rectnagle shows the position/dimensions of the bar, 
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


	void AdjustHintRect( wxPoint& mousePos );

	void ClipRectInFrame( wxRect& rect );
	void ClipPosInFrame( wxPoint& pos );

	cbDockPane* HitTestPanes( wxRect& rect );
	cbDockPane* HitTestPanes( wxPoint& pos );
	bool HitsPane( cbDockPane* pPane, wxRect& rect );

	void CalcOnScreenDims( wxRect& rect );

	int  GetDistanceToPane( cbDockPane* pPane, wxPoint& mousePos );

	bool IsInOtherPane ( wxPoint& mousePos );
	bool IsInClientArea( wxPoint& mousePos );
	bool IsInClientArea( wxRect& rect );

	void StickToPane( cbDockPane* pPane, wxPoint& mousePos );
	void UnstickFromPane( cbDockPane* pPane, wxPoint& mousePos );

	int GetBarWidthInPane( cbDockPane* pPane );
	int GetBarHeightInPane( cbDockPane* pPane );

	// on-screen hint-tracking related methods

	void StartTracking();

	void DrawHintRect ( wxRect& rect, bool isInClientRect);
	void EraseHintRect( wxRect& rect, bool isInClientRect);

	void FinishTracking();

	void DoDrawHintRect( wxRect& rect, bool isInClientRect);

	void RectToScr( wxRect& frameRect, wxRect& scrRect );

	void ShowHint( bool prevWasInClient );

public:
	cbBarDragPlugin(void);

	cbBarDragPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	virtual ~cbBarDragPlugin();

	// handlers for plugin events

	void OnMouseMove( cbMotionEvent& event );
	void OnLButtonUp( cbLeftUpEvent& event );
	void OnLButtonDown( cbLeftDownEvent& event );
	void OnLDblClick( cbLeftDClickEvent& event );

	// handles event, which oriniates from itself
	void OnDrawHintRect( cbDrawHintRectEvent& event );        

	void OnStartBarDragging( cbStartBarDraggingEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
