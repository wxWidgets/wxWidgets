/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Central header file for control-bar related classes
//
// Author:      Aleksandras Gluchovas <mailto:alex@soften.ktu.lt>
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __PANEDRAWPL_G__
#define __PANEDRAWPL_G__

#ifdef __GNUG__
#pragma interface "panedrawpl.h"
#endif

#include "controlbar.h"

/*
 * Simple, but all-in-one plugin implementation. Resembles look & feel of
 * to MFC control-bars. Handles painting of pane and items in it.
 * Fires bar/layout customization event, when user right-clicks bar/pane. 
 * Hooking an instance of this and row-layouting plugins per each pane, 
 * would be enough for the frame layout to function properly.
 * (they are plugged in autimatically by wxFrameLayout class)
 */

class cbPaneDrawPlugin : public cbPluginBase
{
public:
	DECLARE_DYNAMIC_CLASS( cbPaneDrawPlugin )
protected:

	// resizing bars/rows state variables
	bool           mResizeStarted;
	bool           mResizeCursorOn;
	wxPoint        mDragOrigin;

	bool           mRowHandleHitted;
	bool           mIsUpperHandle;
	bool           mBarHandleHitted;
	bool           mIsLeftHandle;
	bool           mBarContentHitted;

	cbBarInfo*     mpDraggedBar; // also used when in bar-drag action
	cbRowInfo*     mpResizedRow;

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
	// helpers
	void DrawDraggedHandle( const wxPoint& pos, cbDockPane& pane );

	virtual void DrawPaneShade( wxDC& dc, int alignment );
	virtual void DrawPaneShadeForRow( cbRowInfo* pRow, wxDC& dc );

	virtual void DrawUpperRowHandle( cbRowInfo* pRow, wxDC& dc );
	virtual void DrawLowerRowHandle( cbRowInfo* pRow, wxDC& dc );

	virtual void DrawUpperRowShades( cbRowInfo* pRow, wxDC& dc, int level );
	virtual void DrawLowerRowShades( cbRowInfo* pRow, wxDC& dc, int level );

	virtual void DrawBarInnerShadeRect( cbBarInfo* pBar, wxDC& dc );

	virtual void DrawShade( int level, wxRect& rect, int alignment, wxDC& dc );
	virtual void DrawShade1( int level, wxRect& rect, int alignment, wxDC& dc );

	inline void SetLightPixel( int x, int y, wxDC& dc );
	inline void SetDarkPixel ( int x, int y, wxDC& dc );

public:
	cbPaneDrawPlugin(void);

	cbPaneDrawPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	virtual ~cbPaneDrawPlugin();

	virtual cbPluginBase* Clone() { return new cbPaneDrawPlugin(0,0); }

	// handlers for plugin-events

	void OnLButtonDown( cbLeftDownEvent&   event );
	void OnLDblClick  ( cbLeftDClickEvent& event );
	void OnLButtonUp  ( cbLeftUpEvent&     event );
	void OnRButtonUp  ( cbRightUpEvent&    event );
	void OnMouseMove  ( cbMotionEvent&     event );

	void OnDrawPaneBackground ( cbDrawPaneBkGroundEvent& event );
	void OnDrawPaneDecorations( cbDrawPaneDecorEvent&    event );

	void OnDrawRowDecorations ( cbDrawRowDecorEvent&     event );
	void OnDrawRowHandles     ( cbDrawRowHandlesEvent&   event );
	void OnDrawRowBackground  ( cbDrawRowBkGroundEvent&  event );
							  
	void OnSizeBarWindow      ( cbSizeBarWndEvent&       event );
	void OnDrawBarDecorations ( cbDrawBarDecorEvent&     event );
	void OnDrawBarHandles     ( cbDrawBarHandlesEvent&   event );

	void OnStartDrawInArea    ( cbStartDrawInAreaEvent&  event );
	void OnFinishDrawInArea   ( cbFinishDrawInAreaEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif