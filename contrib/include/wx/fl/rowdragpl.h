/////////////////////////////////////////////////////////////////////////////
// Name:        rowdragpl.h
// Purpose:     cbRowDragPlugin class header.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROWDRAGPL_G__
#define __ROWDRAGPL_G__

#include "wx/fl/controlbar.h"

/*
This plugin adds row-dragging functionality to the pane.
It handles mouse movement and pane background-erasing plugin events.
The behaviour and appearance resembles drag and drop positioning
of the toolbar rows in Netscape Communicator 4.xx.
*/

class WXDLLIMPEXP_FL cbRowDragPlugin : public cbPluginBase
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

    // on-screen drawing state variables
    wxBitmap* mpPaneImage;
    wxBitmap* mpRowImage;
    wxBitmap* mpCombinedImage;

    wxScreenDC* mpScrDc;
    wxRect    mCombRect;
    wxSize    mRowImgDim;
    int       mInitialRowOfs;

    // NOTE:: if mpRowInFocus is not NULL, then mCollapsedIconInFocus is -1,
    //        and v.v. (two different items cannot be in focus at the same time)

    cbRowInfo* mpRowInFocus;
    int        mCollapsedIconInFocus;

    cbDockPane* mpPane; // is set up temorarely, while handling event

    wxList      mHiddenBars;

        // Helper for drag and drop.
    wxBitmap* CaptureDCArea( wxDC& dc, wxRect& area );

        // Helper for drag and drop.
    int GetHRowsCountForPane( cbDockPane* pPane );

        // Helper for drag and drop.
    void SetMouseCapture( bool captureOn );

        // Helper for drag and drop.
    void PrepareForRowDrag();

        // Helper for drag and drop.
    void ShowDraggedRow( int offset );

        // Helper for drag and drop.
    void ShowPaneImage();

        // Helper for drag and drop.
    void FinishOnScreenDraw();

        // Helper for drag and drop.
    void CollapseRow( cbRowInfo* pRow );

        // Helper for drag and drop.
    void ExpandRow( int collapsedIconIdx );

        // Helper for drag and drop.
    void InsertDraggedRowBefore( cbRowInfo* pBeforeRow );

        // Helper for drag and drop.
    bool ItemIsInFocus();

        // Helper for drag and drop.
    void CheckPrevItemInFocus( cbRowInfo* pRow, int iconIdx );

        // Helper for drag and drop.
    void UnhighlightItemInFocus();

        // Helper for drag and drop.
    cbRowInfo* GetFirstRow();

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawTrianUp( wxRect& inRect, wxDC& dc );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawTrianDown( wxRect& inRect, wxDC& dc );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawTrianRight( wxRect& inRect, wxDC& dc );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void Draw3DPattern( wxRect& inRect, wxDC& dc );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawRombShades( wxPoint& p1, wxPoint& p2, wxPoint& p3, wxPoint& p4, wxDC& dc );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawOrtoRomb( wxRect& inRect, wxDC& dc, const wxBrush& bkBrush );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawRomb( wxRect& inRect, wxDC& dc, const wxBrush& bkBrush );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void Draw3DRect( wxRect& inRect, wxDC& dc, const wxBrush& bkBrush );

        // Implements 'hard-coded metafile' for Netscape Navigator look.
    virtual void DrawRectShade( wxRect& inRect, wxDC& dc,
                                int level, const wxPen& upperPen, const wxPen& lowerPen );

        // Helper for drag and drop.
    virtual void GetRowHintRect( cbRowInfo* pRow, wxRect& rect );

        // Helper for drag and drop.
    virtual void GetCollapsedInconRect( int iconIdx, wxRect& rect );

        // Helper for drag and drop.
    virtual int GetCollapsedIconsPos();

public:

        // Default constructor.
    cbRowDragPlugin();

        // Constructor, taking paren layout frame and pane mask.
    cbRowDragPlugin( wxFrameLayout* pLayout, int paneMask = wxALL_PANES );

        // Destructor.
    virtual ~cbRowDragPlugin();

        // Clone function, returning a new instance of this class.
    virtual cbPluginBase* Clone() { return new cbRowDragPlugin(NULL,0); }

        // Called to initialize this plugin.
    virtual void OnInitPlugin();

        // Handles mouse move plugin events (appearance-independent logic).
    void OnMouseMove  ( cbMotionEvent& event );

        // Handles left button down plugin events (appearance-independent logic).
    void OnLButtonDown( cbLeftDownEvent& event );

        // Handles left button up plugin events (appearance-independent logic).
    void OnLButtonUp  ( cbLeftUpEvent& event );

        // Handles pane drawing plugin events (appearance-independent logic).
    void OnDrawPaneBackground( cbDrawPaneDecorEvent& event );

        // Draws collapsed row icon (appearance-dependent).
    virtual void DrawCollapsedRowIcon( int index, wxDC& dc, bool isHighlighted );

        // Draws collapsed rows border (appearance-dependent).
    virtual void DrawCollapsedRowsBorder( wxDC& dc );

        // Draws rows drag hints border (appearance-dependent).
    virtual void DrawRowsDragHintsBorder( wxDC& dc );

        // Draws row drag hint (appearance-dependent).
    virtual void DrawRowDragHint( cbRowInfo* pRow, wxDC& dc, bool isHighlighted );

        // Draws empty row (appearance-dependent).
    virtual void DrawEmptyRow( wxDC& dc, wxRect& rowBounds );

        // Gets the collapsed row icon height.
    virtual int GetCollapsedRowIconHeight();

        // Gets the row drag hint width.
    virtual int GetRowDragHintWidth();

        // Sets the pane margins.
    virtual void SetPaneMargins();

        // Test for the collapsed row icon position.
    virtual bool HitTestCollapsedRowIcon( int iconIdx, const wxPoint& pos );

        // Test for the row drag hint position.
    virtual bool HitTestRowDragHint( cbRowInfo* pRow, const wxPoint& pos );

    DECLARE_EVENT_TABLE()
};

/*
Internal helper class.
*/

class WXDLLIMPEXP_FL cbHiddenBarInfo : public wxObject
{
    DECLARE_DYNAMIC_CLASS( cbHiddenBarInfo )
public:
    cbBarInfo* mpBar;
    int        mRowNo;
    int        mIconNo;
    int        mAlignment;
};

#endif /* __ROWDRAGPL_G__ */

