/////////////////////////////////////////////////////////////////////////////
// Name:        splitter.h
// Purpose:     wxSplitterWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __SPLITTERH_G__
#define __SPLITTERH_G__

#ifdef __GNUG__
#pragma interface "splitter.h"
#endif

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/string.h"

#define WXSPLITTER_VERSION      1.0

#define wxSPLIT_HORIZONTAL      1
#define wxSPLIT_VERTICAL        2

#define wxSPLIT_DRAG_NONE       0
#define wxSPLIT_DRAG_DRAGGING   1
#define wxSPLIT_DRAG_LEFT_DOWN  2

/*
 * wxSplitterWindow maintains one or two panes, with
 * an optional vertical or horizontal split which
 * can be used with the mouse or programmatically.
 */

// TODO:
// 1) Perhaps make the borders sensitive to dragging in order to create a split.
//    The MFC splitter window manages scrollbars as well so is able to
//    put sash buttons on the scrollbars, but we probably don't want to go down
//    this path.
// 2) for wxWindows 2.0, we must find a way to set the WS_CLIPCHILDREN style
//    to prevent flickering. (WS_CLIPCHILDREN doesn't work in all cases so can't be
//    standard).

class WXDLLEXPORT wxSplitterWindow: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxSplitterWindow)

 public:

////////////////////////////////////////////////////////////////////////////
// Public API

    // Default constructor
    wxSplitterWindow(void);

    // Normal constructor
    wxSplitterWindow(wxWindow *parent, const wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, const long style = wxSP_3D|wxCLIP_CHILDREN, const wxString& name = "splitter");
    ~wxSplitterWindow(void);

    // Gets the only or left/top pane
    inline wxWindow *GetWindow1(void) { return m_windowOne; }

    // Gets the right/bottom pane
    inline wxWindow *GetWindow2(void) { return m_windowTwo; }

    // Sets the split mode
    inline void SetSplitMode(const int mode) { m_splitMode = mode; }

    // Gets the split mode
    inline int GetSplitMode(void) const { return m_splitMode; };

    // Initialize with one window
    void Initialize(wxWindow *window);

    // Associates the given window with window 2, drawing the appropriate sash
    // and changing the split mode.
    // Does nothing and returns FALSE if the window is already split.
    // A sashPosition of -1 means choose a default sash position.
    bool SplitVertically(wxWindow *window1, wxWindow *window2, const int sashPosition = -1);
    bool SplitHorizontally(wxWindow *window1, wxWindow *window2, const int sashPosition = -1);

    // Removes the specified (or second) window from the view
    // Doesn't actually delete the window.
    bool Unsplit(wxWindow *toRemove = NULL);

    // Is the window split?
    inline bool IsSplit(void) const { return (m_windowTwo != NULL); }

    // Sets the sash size
    inline void SetSashSize(const int width) { m_sashSize = width; }

    // Sets the border size
    inline void SetBorderSize(const int width) { m_borderSize = width; }

    // Gets the sash size
    inline int GetSashSize(void) const { return m_sashSize; }

    // Gets the border size
    inline int GetBorderSize(void) const { return m_borderSize; }

    // Set the sash position
    void SetSashPosition(const int position, const bool redaw = TRUE);

    // Gets the sash position
    inline int GetSashPosition(void) const { return m_sashPosition; }

    // If this is zero, we can remove panes by dragging the sash.
    inline void SetMinimumPaneSize(const int min) { m_minimumPaneSize = min; }
    inline int GetMinimumPaneSize(void) const { return m_minimumPaneSize; }

    // If the sash is moved to an extreme position, a subwindow
    // is removed from the splitter window, and the app is
    // notified. The app should delete or hide the window.
    virtual void OnUnsplit(wxWindow *removed) { removed->Show(FALSE); }

    // Called when the sash is double-clicked.
    // The default behaviour is to remove the sash if the
    // minimum pane size is zero.
    virtual void OnDoubleClickSash(int x, int y);

////////////////////////////////////////////////////////////////////////////
// Implementation

    // Paints the border and sash
    void OnPaint(wxPaintEvent& event);

    // Handles mouse events
    void OnMouseEvent(wxMouseEvent& ev);

    // Adjusts the panes
    void OnSize(wxSizeEvent& event);

    // Draws borders
    void DrawBorders(wxDC& dc);

    // Draws the sash
    void DrawSash(wxDC& dc);

    // Draws the sash tracker (for whilst moving the sash)
    void DrawSashTracker(const int x, const int y);

    // Tests for x, y over sash
    bool SashHitTest(const int x, const int y, const int tolerance = 2);

    // Resizes subwindows
    void SizeWindows(void);

    // Initialize colours
    void InitColours(void);

 protected:
    int         m_splitMode;
    wxWindow*   m_windowOne;
    wxWindow*   m_windowTwo;
    int         m_dragMode;
    int         m_oldX;
    int         m_oldY;
    int         m_borderSize;
    int         m_sashSize;     // Sash width or height
    int         m_sashPosition; // Number of pixels from left or top
    int         m_firstX;
    int         m_firstY;
    int         m_minimumPaneSize;
    wxCursor*   m_sashCursorWE;
    wxCursor*   m_sashCursorNS;
    wxPen*      m_sashTrackerPen;
    wxPen*      m_lightShadowPen;
    wxPen*      m_mediumShadowPen;
    wxPen*      m_darkShadowPen;
    wxPen*      m_hilightPen;
    wxBrush*    m_faceBrush;
    wxPen*      m_facePen;
DECLARE_EVENT_TABLE()
};

#endif
