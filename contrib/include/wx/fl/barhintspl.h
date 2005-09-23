/////////////////////////////////////////////////////////////////////////////
// Name:        barhintspl.h
// Purpose:     cbBarHintsPlugin class declaration
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     30/11/98 (my 22th birthday :-)
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DRAGHINTSPL_G__
#define __DRAGHINTSPL_G__

#include "wx/fl/controlbar.h"
#include "wx/fl/toolwnd.h"

/*
This class intercepts bar-decoration and sizing events, and draws 3D hints
around fixed and flexible bars, similar to those in Microsoft DevStudio 6.x
*/

class WXDLLIMPEXP_FL cbBarHintsPlugin : public cbPluginBase
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
        // Helper function: draws a 3D box.
    void Draw3DBox      ( wxDC& dc, const wxPoint& pos, bool pressed );

        // Helper function: draws a close box.
    void DrawCloseBox   ( wxDC& dc, const wxPoint& pos, bool pressed );

        // Helper function: draws a collapse box.
    void DrawCollapseBox( wxDC& dc, const wxPoint& pos,
                          bool atLeft, bool disabled, bool pressed );

        // Helper function: draws grooves.
    void DrawGrooves    ( wxDC& dc, const wxPoint& pos, int length );

        // Helper function: draws a hint.
    void DoDrawHint( wxDC& dc, wxRect& rect, int pos, int boxOfs, int grooveOfs, bool isFixed );

        // Helper function: gets the layout of a hint.
    void GetHintsLayout( wxRect& rect, cbBarInfo& info,
                         int& boxOfs, int& grooveOfs, int& pos );

        // Helper function: returns information about the hint under the given position.
    int HitTestHints( cbBarInfo& info, const wxPoint& pos );

        // Helper function.
    void ExcludeHints( wxRect& rect, cbBarInfo& info );

        // Helper function: creates close and collapse boxes.
    void CreateBoxes();

public:
    /* public properties */

    bool mCloseBoxOn;    // default: ON
    bool mCollapseBoxOn; // default: ON
    int  mGrooveCount;   // default: 2 (two shaded bars)
    int  mHintGap;       // default: 5 (pixels from above, below, right and left)
    int  mXWeight;       // default: 2 (width in pixels of lines which used for drawing cross)

public:
        // Default constructor.
    cbBarHintsPlugin(void);

        // Constructor, taking parent frame and pane mask flag.
    cbBarHintsPlugin( wxFrameLayout* pLayout, int paneMask = wxALL_PANES );

        // Destructor.
    ~cbBarHintsPlugin();

        // Set the number of grooves to be shown in the pane.
    void SetGrooveCount( int nGrooves );

        // Called to initialize this plugin.
    void OnInitPlugin();

        // Handles a plugin event.
    void OnSizeBarWindow( cbSizeBarWndEvent& event );

        // Handles a plugin event.
    void OnDrawBarDecorations( cbDrawBarDecorEvent& event );

        // Handles a plugin event.
    void OnLeftDown( cbLeftDownEvent& event );

        // Handles a plugin event.
    void OnLeftUp  ( cbLeftUpEvent&   event );

        // Handles a plugin event.
    void OnMotion  ( cbMotionEvent&   event );

    DECLARE_EVENT_TABLE()
};

#endif /* __DRAGHINTSPL_G__ */

