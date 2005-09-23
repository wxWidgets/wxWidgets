/////////////////////////////////////////////////////////////////////////////
// Name:        toolwnd.h
// Purpose:     wxToolWindow, cbMiniButton, cbCloseBox, cbCollapseBox,
//              cbDockBox, cbFloatedBarWindow class declarations.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __TOOLWND_G__
#define __TOOLWND_G__

#include "wx/frame.h"
#include "wx/dynarray.h"
#include "wx/fl/fldefs.h"

// fixed settings

#define BTN_BOX_HEIGHT       12
#define BTN_BOX_WIDTH        12
#define BTN_X_WEIGHT         2

class WXDLLIMPEXP_FL cbMiniButton;

typedef cbMiniButton* cbMinitButtonPtrT;

WXFL_DEFINE_ARRAY_PTR( cbMinitButtonPtrT, cbMiniButtonArrayT );

/*
A tool window is a special kind of frame that paints its own title, and
can be used to implement small floating windows.
*/

class WXDLLIMPEXP_FL wxToolWindow : public wxFrame
{
    DECLARE_DYNAMIC_CLASS( wxToolWindow )

public:    /** protected really, accessed only by serializers **/

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
        // Maps client coordinates to screen coordinates.
    void GetScrWindowRect( wxRect& r );

        // Gets the mouse position in screen coordinates.
    void GetScrMousePos  ( wxMouseEvent& event, wxPoint& pos );

        // Sets the hint cursor.
    void SetHintCursor   ( int type );

        // Calculate resized rectangle.
    void CalcResizedRect( wxRect& rect, wxPoint& delta, const wxSize& minDim );

        // Helper function.
    void AdjustRectPos( const wxRect& original, const wxSize& newDim, wxRect& newRect );

        // Helper function.
    wxSize GetMinimalWndDim();

        // Draws the hint rectangle.
    void DrawHintRect( const wxRect& r );

        // Tests if the mouse position is in this window.
    int HitTestWindow( wxMouseEvent& event );

        // Lays out the buttons.
    void LayoutMiniButtons();

public:

        // Default constructor.
    wxToolWindow();

        // Destructor.
    ~wxToolWindow();

        // Sets the client for this tool window.
    void SetClient( wxWindow* pWnd );

        // Returns the client window.
    wxWindow* GetClient();

        // Sets the title font.
    void SetTitleFont( wxFont& font );

        // Adds a button. Buttons are added in right-to-left order.
    void AddMiniButton( cbMiniButton* pBtn );

        // Responds to a paint event.
    void OnPaint( wxPaintEvent& event );

        // Responds to a mouse move event.
    void OnMotion( wxMouseEvent& event );

        // Responds to a mouse left down event.
    void OnLeftDown( wxMouseEvent& event );

        // Responds to a mouse left up event.
    void OnLeftUp( wxMouseEvent& event );

        // Responds to a size event.
    void OnSize( wxSizeEvent& event );

        // Responds to an erase background event.
    void OnEraseBackground( wxEraseEvent& event );

        // Returns the preferred size for the window.
    virtual wxSize GetPreferredSize( const wxSize& given );

        // Called when a mini button is clicked.
        // By default, does nothing.
    virtual void OnMiniButtonClicked( int WXUNUSED(btnIdx) ) {}

        // Handles clicking on the title. By default, does nothing.
    virtual bool HandleTitleClick( wxMouseEvent& WXUNUSED(event) ) { return false; }

    DECLARE_EVENT_TABLE()
};

// FIXME:: the code below should be moved to a separate file

#include "wx/fl/controlbar.h"

/*
cbMiniButton is the base class for a small button that can be placed in a wxToolWindow
titlebar.
*/

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
        // Default constructor.
    cbMiniButton();

        // Set the position of the button.
    void SetPos( const wxPoint& pos );

        // Returns true if the given position was over the button.
    bool HitTest( const wxPoint& pos );

        // Responds to a left down event.
    void OnLeftDown( const wxPoint& pos );

        // Responds to a left up event.
    void OnLeftUp( const wxPoint& pos );

        // Responds to a mouse move event.
    void OnMotion( const wxPoint& pos );

        // Refreshes the button.
    void Refresh();

        // Draws the button. Override this to implement
        // the desired appearance.
    virtual void Draw( wxDC& dc );

        // Returns true if the button was clicked.
    bool WasClicked();

        // Reset the button.
    void Reset();

        // Enable or disable the button.
    void Enable( bool enable ) { mEnabled = enable; }

        // Returns true if this button is pressed.
    bool IsPressed() { return mPressed; }
};

/*
cbCloseBox is a window close button, used in a wxToolWindow titlebar.
*/

class WXDLLIMPEXP_FL cbCloseBox : public cbMiniButton
{
public:
        // Draws the close button appearance.
    virtual void Draw( wxDC& dc );
};

/*
cbCollapseBox is a window collapse button, used in a wxToolWindow titlebar.
*/

class WXDLLIMPEXP_FL cbCollapseBox  : public cbMiniButton
{
public:
    bool mIsAtLeft;

        // Draws the collapse button appearance.
    virtual void Draw( wxDC& dc );
};

/*
cbDockBox is a window dock button, used in a wxToolWindow titlebar.
*/

class WXDLLIMPEXP_FL cbDockBox : public cbMiniButton
{
public:
        // Draws the dock button appearance.
    virtual void Draw( wxDC& dc );
};

/*
cbFloatedBarWindow is a kind of wxToolWindow,
implementing floating toolbars.
*/

class WXDLLIMPEXP_FL cbFloatedBarWindow : public wxToolWindow
{
    DECLARE_DYNAMIC_CLASS( cbFloatedBarWindow )
protected:
    cbBarInfo*     mpBar;
    wxFrameLayout* mpLayout;

    friend class cbFloatedBarWindowSerializer;

public:
        // Default constructor.
    cbFloatedBarWindow();

        // Sets the bar information for this window.
    void SetBar( cbBarInfo* pBar );

        // Sets the layout for this window.
    void SetLayout( wxFrameLayout* pLayout );

        // Returns the bar information for this window.
    cbBarInfo* GetBar();

        // Position the floating window. The given coordinates
        // are those of the bar itself; the floated container window's
        // position and size are ajusted accordingly.
    void PositionFloatedWnd( int scrX,  int scrY,
                             int width, int height );

        // Overridden function returning the preferred size.
    virtual wxSize GetPreferredSize( const wxSize& given );

        // Overridden function responding to mouse clicks on mini-buttons.
    virtual void OnMiniButtonClicked( int btnIdx );

        // Overridden function responding to mouse button clicks on the titlebar.
    virtual bool HandleTitleClick( wxMouseEvent& event );

        // Responds to double-click mouse events.
    void OnDblClick( wxMouseEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __TOOLWND_G__ */


