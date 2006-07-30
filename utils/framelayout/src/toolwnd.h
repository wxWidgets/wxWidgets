/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TOOLWND_G__
#define __TOOLWND_G__

#include "wx/frame.h"
#include "wx/dynarray.h"

// fixed settings

#define BTN_BOX_HEIGHT       12
#define BTN_BOX_WIDTH        12
#define BTN_X_WIEGHT         2

class cbMiniButton;

typedef cbMiniButton* cbMinitButtonPtrT;

WX_DEFINE_ARRAY( cbMinitButtonPtrT, cbMiniButtonArrayT );

class wxToolWindow : public wxFrame
{
	DECLARE_DYNAMIC_CLASS( wxToolWindow )

public:	/** protected really, accesssed only by serializers **/

	cbMiniButtonArrayT mButtons;
	wxWindow* mpClientWnd;

	wxFont    mTitleFont;

	int       mTitleHeight;
	int       mClntHorizGap;
	int       mClntVertGap;
	int       mWndVertGap;
	int       mWndHorizGap;
	int       mButtonGap;
	int       mInTitleMargin;
	int       mHintBorder;

	bool      mResizeStarted;
	bool      mRealTimeUpdatesOn;

	int       mMTolerance;

	int       mCursorType;
	bool      mMouseCaptured;

	// drag&drop state variables

	wxPoint     mDragOrigin;
	wxRect      mInitialRect;
	wxRect      mPrevHintRect;
	wxScreenDC* mpScrDc;

protected:
	void GetScrWindowRect( wxRect& r );
	void GetScrMousePos  ( wxMouseEvent& event, wxPoint& pos );
	void SetHintCursor   ( int type );

	void CalcResizedRect( wxRect& rect, wxPoint& delta, const wxSize& minDim );
	void AdjustRectPos( const wxRect& original, const wxSize& newDim, wxRect& newRect );
	wxSize GetMinimalWndDim();

	void DrawHintRect( const wxRect& r );

	int HitTestWindow( wxMouseEvent& event );

	void LayoutMiniButtons();

public:
	
	wxToolWindow();
	~wxToolWindow();

	void SetClient( wxWindow* pWnd );
	wxWindow* GetClient();

	void SetTitleFont( wxFont& font );

	// buttons are added in right-to-left order
	void AddMiniButton( cbMiniButton* pBtn );

	void OnPaint( wxPaintEvent& event );

	void OnMotion( wxMouseEvent& event );
	void OnLeftDown( wxMouseEvent& event );
	void OnLeftUp( wxMouseEvent& event );
	void OnSize( wxSizeEvent& event );

	void OnEraseBackground( wxEraseEvent& event );

	// overridables:

	virtual wxSize GetPreferredSize( const wxSize& given );
	virtual void OnMiniButtonClicked( int btnIdx ) {}
	virtual bool HandleTitleClick( wxMouseEvent& event ) { return FALSE; }

	DECLARE_EVENT_TABLE()
};

// FIXME:: the code below should be moved to a separate file

#include "controlbar.h"

class cbMiniButton : public wxObject
{
public:
	wxPoint   mPos;
	wxSize    mDim;
	bool      mVisible;
	bool      mEnabled;

	wxFrameLayout* mpLayout;
	cbDockPane*    mpPane;
	cbPluginBase*  mpPlugin;

	wxWindow*      mpWnd;

	bool      mWasClicked;
	bool      mDragStarted;

	bool      mPressed;
public:
	cbMiniButton();

	void SetPos( const wxPoint& pos );
	bool HitTest( const wxPoint& pos );

	void OnLeftDown( const wxPoint& pos );
	void OnLeftUp( const wxPoint& pos );
	void OnMotion( const wxPoint& pos );

	void Refresh();
	virtual void Draw( wxDC& dc );

	bool WasClicked();
	void Reset();

	void Enable( bool enable ) { mEnabled = enable; }

	bool IsPressed() { return mPressed; }
};

// classes specific to wxFrameLayout engine (FOR NOW in here...)

class cbCloseBox : public cbMiniButton
{
public:
	virtual void Draw( wxDC& dc );
};

class cbCollapseBox  : public cbMiniButton
{
public:
	bool mIsAtLeft;

	virtual void Draw( wxDC& dc );
};

class cbDockBox : public cbMiniButton
{
public:
	virtual void Draw( wxDC& dc );
};

class cbFloatedBarWindow : public wxToolWindow
{
	DECLARE_DYNAMIC_CLASS( cbFloatedBarWindow )
protected:
	cbBarInfo*     mpBar;
	wxFrameLayout* mpLayout;

	friend class cbFloatedBarWindowSerializer;

public:
	cbFloatedBarWindow();

	void SetBar( cbBarInfo* pBar );
	void SetLayout( wxFrameLayout* pLayout );
	cbBarInfo* GetBar();

	// given coordinates are those of the bar itself
	// floated container window's position and size
	// are ajusted accordingly

	void PositionFloatedWnd( int scrX,  int scrY,
							 int width, int height );

	// overriden methods of wxToolWindow

	virtual wxSize GetPreferredSize( const wxSize& given );
	virtual void OnMiniButtonClicked( int btnIdx );
	virtual bool HandleTitleClick( wxMouseEvent& event );

	void OnDblClick( wxMouseEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
