/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROWDRAGPL_G__
#define __ROWDRAGPL_G__

#include "controlbar.h"

/*
 * Plugin adds row-dragging fuctionality to the pane.
 * Handles mouse/movement and pane-background erasing plugin-events.
 * Behaviour and appearence resembles drag & drop posotioning 
 * of the toolbar-rows int Netscape Comunicator 4.xx.
 */

class cbRowDragPlugin : public cbPluginBase
{
	DECLARE_DYNAMIC_CLASS( cbRowDragPlugin )
public:
	// background colours for the highlighted/unhighlighted icons

	wxColour mHightColor;      // light-blue for NC-look
	wxColour mLowColor;        // light-gray -/-
	wxColour mTrianInnerColor; // blue       -/-
	wxPen    mTrianInnerPen;   // black      -/-

protected:
	friend class cbRowDragPluginSerializer;

	// drag & drop state variables
	bool    mDragStarted;
	bool    mDecisionMode;
	wxPoint mDragOrigin;
	int     mCurDragOfs;
	bool    mCaptureIsOn;

	// saved margins of the pane
	int mSvTopMargin;
	int mSvBottomMargin;
	int mSvLeftMargin;
	int mSvRightMargin;

	//on-screen drawing state variables
	wxBitmap* mpPaneImage;
	wxBitmap* mpRowImage;
	wxBitmap* mpCombinedImage;

	wxScreenDC* mpScrDc;
	wxRect    mCombRect;
	wxSize    mRowImgDim;
	int       mInitalRowOfs;

	// NOTE:: if mpRowInFocus is not NULL, then mCollapsedIconInFocus is -1,
	//        and v.v. (two different items cannot be in focus at the same time)

	cbRowInfo* mpRowInFocus;
	int        mCollapsedIconInFocus;

	cbDockPane* mpPane; // is set up temorarely, while handling event

	wxList      mHiddenBars;

	wxBitmap* CaptureDCArea( wxDC& dc, wxRect& area );

	// helpers for drag&drop

	int GetHRowsCountForPane( cbDockPane* pPane );

	void SetMouseCapture( bool captureOn );
	void PrepareForRowDrag();
	void ShowDraggedRow( int offset );
	void ShowPaneImage();
	void FinishOnScreenDraw();
	void CollapseRow( cbRowInfo* pRow );
	void ExpandRow( int collapsedIconIdx );
	void InsertDraggedRowBefore( cbRowInfo* pBeforeRow );
	bool ItemIsInFocus();
	void CheckPrevItemInFocus( cbRowInfo* pRow, int iconIdx );
	void UnhiglightItemInFocus();

	cbRowInfo* GetFirstRow();

	// "hard-coded metafile" for NN-look

	virtual void DrawTrianUp( wxRect& inRect, wxDC& dc );
	virtual void DrawTrianDown( wxRect& inRect, wxDC& dc );
	virtual void DrawTrianRight( wxRect& inRect, wxDC& dc );
	virtual void Draw3DPattern( wxRect& inRect, wxDC& dc );
	virtual void DrawRombShades( wxPoint& p1, wxPoint& p2, wxPoint& p3, wxPoint& p4, wxDC& dc );
	virtual void DrawOrtoRomb( wxRect& inRect, wxDC& dc, wxBrush& bkBrush );
	virtual void DrawRomb( wxRect& inRect, wxDC& dc, wxBrush& bkBrush );
	virtual void Draw3DRect( wxRect& inRect, wxDC& dc, wxBrush& bkBrush );
	virtual void DrawRectShade( wxRect& inRect, wxDC& dc, 
								int level, wxPen& upperPen, wxPen& lowerPen );

	virtual void GetRowHintRect( cbRowInfo* pRow, wxRect& rect );
	virtual void GetCollapsedInconRect( int iconIdx, wxRect& rect );

	virtual int  GetCollapsedIconsPos();

public:

	cbRowDragPlugin(void);

	cbRowDragPlugin( wxFrameLayout* pLayout, int paneMask = wxALL_PANES );
	virtual ~cbRowDragPlugin();

	virtual cbPluginBase* Clone() { return new cbRowDragPlugin(NULL,0); }

	virtual void OnInitPlugin();

	// handlers for plugin events (appearence-independent logic)

	void OnMouseMove  ( cbMotionEvent& event );
	void OnLButtonDown( cbLeftDownEvent& event );
	void OnLButtonUp  ( cbLeftUpEvent& event );
	void OnDrawPaneBackground( cbDrawPaneDecorEvent& event );

	// overridables (appearence-depedent)

	virtual void DrawCollapsedRowIcon( int index, wxDC& dc, bool isHighlighted );
	virtual void DrawCollapsedRowsBorder( wxDC& dc );
	virtual void DrawRowsDragHintsBorder( wxDC& dc );
	virtual void DrawRowDragHint( cbRowInfo* pRow, wxDC& dc, bool isHighlighted );
	virtual void DrawEmptyRow( wxDC& dc, wxRect& rowBounds );

	virtual int GetCollapsedRowIconHeight();
	virtual int GetRowDragHintWidth();

	virtual void SetPaneMargins();


	virtual bool HitTestCollapsedRowIcon( int iconIdx, const wxPoint& pos );
	virtual bool HitTestRowDragHint( cbRowInfo* pRow, const wxPoint& pos );

	DECLARE_EVENT_TABLE()
};

// internal helper-class

class cbHiddenBarInfo : public wxObject
{
	DECLARE_DYNAMIC_CLASS( cbHiddenBarInfo )
public:
	cbBarInfo* mpBar;
	int        mRowNo;
	int        mIconNo;
	int        mAlignment;
};

#endif