/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __NEWBMPBTN_G__
#define __NEWBMPBTN_G__

#include "wx/button.h"
#include "wx/string.h"

// button lable-text alignment types

#define NB_ALIGN_TEXT_RIGHT  0
#define NB_ALIGN_TEXT_BOTTOM 1
#define NB_NO_TEXT           2
#define NB_NO_IMAGE          3

// classes declared in this header file

class wxNewBitmapButton;
class wxBorderLessBitmapButton;

// alternative class for wxBmpButton

class wxNewBitmapButton: public wxPanel
{
	DECLARE_DYNAMIC_CLASS(wxNewBitmapButton)

protected:

	friend class wxNewBitmapButtonSerializer;

	int      mTextToLabelGap;
	int      mMarginX;
	int      mMarginY;
	int      mTextAlignment;
	bool     mIsSticky;

	wxString mLabelText;
	wxString mImageFileName;
	int      mImageFileType;
	bool     mIsFlat;

	wxBitmap mDepressedBmp; // source image for rendering
	                        // labels for particular state

	wxBitmap mFocusedBmp;   // may not be always present -
							// only if mHasFocusedBmp is TRUE

	wxBitmap* mpDepressedImg;
	wxBitmap* mpPressedImg;
	wxBitmap* mpDisabledImg;
	wxBitmap* mpFocusedImg;

	// button state variables;
	bool      mDragStarted;
	bool      mIsPressed; 
	bool      mIsInFocus;
	bool      mPrevPressedState;

	bool      mHasFocusedBmp;

	// type of event which is fired upon depression of this button
	int       mFiredEventType;

	// pens for drawing decorations (borders)
	wxPen     mBlackPen;
	wxPen     mDarkPen;
	wxPen     mGrayPen;
	wxPen     mLightPen;

	bool      mIsCreated;
	int       mSizeIsSet;

protected:
	void DestroyLabels();

	// returns the label which match the current button state
	virtual wxBitmap* GetStateLabel();

	virtual void DrawShade( int outerLevel,
							wxDC&  dc,
		                    wxPen& upperLeftSidePen,
						    wxPen& lowerRightSidePen );

	bool IsInWindow( int x,int y );

public:

	wxNewBitmapButton( const wxBitmap& labelBitmap = wxNullBitmap, 
					   const wxString& labelText   = "",
		               int   alignText             = NB_ALIGN_TEXT_BOTTOM,
					   bool  isFlat                = TRUE,
					   // this is the default type of fired events
					   int firedEventType = wxEVT_COMMAND_MENU_SELECTED,
					   int marginX        = 2,
	                   int marginY        = 2,
					   int textToLabelGap = 2,
					   bool isSticky      = FALSE
		             );

	// use this constructor if buttons have to be persistant 
	 
	wxNewBitmapButton( const wxString& bitmapFileName,
		               const int       bitmapFileType = wxBITMAP_TYPE_BMP,
					   const wxString& labelText      = "",
		               int alignText                  = NB_ALIGN_TEXT_BOTTOM,
					   bool  isFlat                   = TRUE,
					   // this is the default type of fired events
					   int firedEventType = wxEVT_COMMAND_MENU_SELECTED,
					   int marginX        = 2,
	                   int marginY        = 2,
					   int textToLabelGap = 2,
					   bool isSticky      = FALSE
		             );

	~wxNewBitmapButton();

	// should be called after Create();
	virtual void Reshape();

	// overridables
	virtual void SetLabel(const wxBitmap& labelBitmap, const wxString& labelText = "" );
	
	virtual void SetAlignments( int alignText = NB_ALIGN_TEXT_BOTTOM,
								int marginX        = 2,
								int marginY        = 2,
								int textToLabelGap = 2);

	virtual void DrawDecorations( wxDC& dc );
	virtual void DrawLabel( wxDC& dc );

	virtual void RenderLabelImage( wxBitmap*& destBmp, wxBitmap* srcBmp, 
								   bool isEnabled = TRUE,
								   bool isPressed = FALSE);

	virtual void RenderLabelImages();

	// event handlers
	void OnLButtonDown( wxMouseEvent& event );
	void OnLButtonUp( wxMouseEvent& event );
	void OnMouseMove( wxMouseEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnPaint( wxPaintEvent& event );
	void OnEraseBackground( wxEraseEvent& event );
	void OnKillFocus( wxFocusEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
