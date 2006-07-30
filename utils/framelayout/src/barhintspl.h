/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     30/11/98 (my 22th birthday :-)
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DRAGHINTSPL_G__
#define __DRAGHINTSPL_G__

#include "controlbar.h"
#include "toolwnd.h"

/*
 * Intercepts bar-decoration and sizing events, draws 3d-hints
 * around fixed and flexible bars, similar to those in Microsoft DevStudio 6.x
 */

class cbBarHintsPlugin : public cbPluginBase
{
	DECLARE_DYNAMIC_CLASS( cbBarHintsPlugin )

protected:
	cbDockPane* mpPane; // is set up temorarely, while handling event

	cbMiniButton* mBoxes[2];

	bool        mBtnPressed;
	bool        mClosePressed;
	cbBarInfo*  mpClickedBar;
	bool        mDepressed;

protected:
	// drawing helpers

	void Draw3DBox      ( wxDC& dc, const wxPoint& pos, bool pressed );
	void DrawCloseBox   ( wxDC& dc, const wxPoint& pos, bool pressed );
	void DrawCollapseBox( wxDC& dc, const wxPoint& pos, 
						  bool atLeft, bool disabled, bool pressed );

	void DrawGrooves    ( wxDC& dc, const wxPoint& pos, int length );

	void DoDrawHint( wxDC& dc, wxRect& rect, int pos, int boxOfs, int grooveOfs, bool isFixed );
	
	void GetHintsLayout( wxRect& rect, cbBarInfo& info, 
						 int& boxOfs, int& grooveOfs, int& pos );

	int HitTestHints( cbBarInfo& info, const wxPoint& pos );

	void ExcludeHints( wxRect& rect, cbBarInfo& info );

	void CreateBoxes();

public:
	/* public properties */

	bool mCloseBoxOn;    // default: ON
	bool mCollapseBoxOn; // default: ON
	int  mGrooveCount;   // default: 2 (two shaded bars)
	int  mHintGap;       // default: 5 (pixels from above, below, right and left)
	int  mXWeight;       // default: 2 (width in pixels of lines which used for drawing cross)

public:

	cbBarHintsPlugin(void);

	cbBarHintsPlugin( wxFrameLayout* pLayout, int paneMask = wxALL_PANES );

	void SetGrooveCount( int nGrooves );

	void OnInitPlugin();

	// handlers of plugin-events

	void OnSizeBarWindow( cbSizeBarWndEvent& event );
	void OnDrawBarDecorations( cbDrawBarDecorEvent& event );

	void OnLeftDown( cbLeftDownEvent& event );
	void OnLeftUp  ( cbLeftUpEvent&   event );
	void OnMotion  ( cbMotionEvent&   event );

	DECLARE_EVENT_TABLE()
};

#endif