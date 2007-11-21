/////////////////////////////////////////////////////////////////////////////
// Name:        splittree.h
// Purpose:     Classes to achieve a remotely-scrolled tree in a splitter
//              window that can be scrolled by a scrolled window higher in the
//              hierarchy
// Author:      Julian Smart
// Modified by:
// Created:     8/7/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPLITTREE_H_
#define _WX_SPLITTREE_H_

#include "wx/gizmos/gizmos.h"

// Set this to 1 to use generic tree control (doesn't yet work properly)
#define USE_GENERIC_TREECTRL 0

#include "wx/wx.h"
#include "wx/treectrl.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"

#if USE_GENERIC_TREECTRL
#include "wx/generic/treectlg.h"
#ifndef wxTreeCtrl
#define wxTreeCtrl wxGenericTreeCtrl
#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif
#endif

class wxRemotelyScrolledTreeCtrl;
class wxThinSplitterWindow;
class wxSplitterScrolledWindow;

/*
 * wxRemotelyScrolledTreeCtrl
 *
 * This tree control disables its vertical scrollbar and catches scroll
 * events passed by a scrolled window higher in the hierarchy.
 * It also updates the scrolled window vertical scrollbar as appropriate.
 */

class WXDLLIMPEXP_GIZMOS wxRemotelyScrolledTreeCtrl: public wxTreeCtrl
{
    DECLARE_CLASS(wxRemotelyScrolledTreeCtrl)
public:
    wxRemotelyScrolledTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt = wxDefaultPosition,
        const wxSize& sz = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
    ~wxRemotelyScrolledTreeCtrl();

//// Events
    void OnSize(wxSizeEvent& event);
    void OnExpand(wxTreeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnPaint(wxPaintEvent& event);

//// Overrides
    // Override this in case we're using the generic tree control.
    // Calls to this should disable the vertical scrollbar.

    // Number of pixels per user unit (0 or -1 for no scrollbar)
    // Length of virtual canvas in user units
    // Length of page in user units
    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                             int noUnitsX, int noUnitsY,
                             int xPos = 0, int yPos = 0,
                             bool noRefresh = false );

    // In case we're using the generic tree control.
    // Get the view start
    virtual void GetViewStart(int *x, int *y) const;

    // In case we're using the generic tree control.
    virtual void PrepareDC(wxDC& dc);

    // In case we're using the generic tree control.
    virtual int GetScrollPos(int orient) const;

//// Helpers
    void HideVScrollbar();

    // Calculate the tree overall size so we can set the scrollbar
    // correctly
    void CalcTreeSize(wxRect& rect);
    void CalcTreeSize(const wxTreeItemId& id, wxRect& rect);

    // Adjust the containing wxScrolledWindow's scrollbars appropriately
    void AdjustRemoteScrollbars();

    // Find the scrolled window that contains this control
    wxScrolledWindow* GetScrolledWindow() const;

    // Scroll to the given line (in scroll units where each unit is
    // the height of an item)
    void ScrollToLine(int posHoriz, int posVert);

//// Accessors

    // The companion window is one which will get notified when certain
    // events happen such as node expansion
    void SetCompanionWindow(wxWindow* companion) { m_companionWindow = companion; }
    wxWindow* GetCompanionWindow() const { return m_companionWindow; }


    DECLARE_EVENT_TABLE()
protected:
    wxWindow*   m_companionWindow;
    bool        m_drawRowLines;
};

/*
 * wxTreeCompanionWindow
 *
 * A window displaying values associated with tree control items.
 */

class WXDLLIMPEXP_GIZMOS wxTreeCompanionWindow: public wxWindow
{
public:
    DECLARE_CLASS(wxTreeCompanionWindow)

    wxTreeCompanionWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& sz = wxDefaultSize,
      long style = 0);

//// Overrides
    virtual void DrawItem(wxDC& dc, wxTreeItemId id, const wxRect& rect);

//// Events
    void OnPaint(wxPaintEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnExpand(wxTreeEvent& event);

//// Operations

//// Accessors
    wxRemotelyScrolledTreeCtrl* GetTreeCtrl() const { return m_treeCtrl; };
    void SetTreeCtrl(wxRemotelyScrolledTreeCtrl* treeCtrl) { m_treeCtrl = treeCtrl; }

//// Data members
protected:
    wxRemotelyScrolledTreeCtrl* m_treeCtrl;

    DECLARE_EVENT_TABLE()
};


/*
 * wxThinSplitterWindow
 *
 * Implements a splitter with a less obvious sash
 * than the usual one.
 */

class WXDLLIMPEXP_GIZMOS wxThinSplitterWindow: public wxSplitterWindow
{
public:
    DECLARE_DYNAMIC_CLASS(wxThinSplitterWindow)

    wxThinSplitterWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& sz = wxDefaultSize,
      long style = wxSP_3D | wxCLIP_CHILDREN);
    ~wxThinSplitterWindow();

//// Overrides

    void SizeWindows();
    // Tests for x, y over sash. Overriding this allows us to increase
    // the tolerance.
    bool SashHitTest(int x, int y, int tolerance = 2);
    void DrawSash(wxDC& dc);

//// Events

    void OnSize(wxSizeEvent& event);

//// Operations

//// Accessors

//// Data members
protected:
    wxPen*      m_facePen;
    wxBrush*    m_faceBrush;

    DECLARE_EVENT_TABLE()
};

/*
 * wxSplitterScrolledWindow
 *
 * This scrolled window is aware of the fact that one of its
 * children is a splitter window. It passes on its scroll events
 * (after some processing) to both splitter children for them
 * scroll appropriately.
 */

class WXDLLIMPEXP_GIZMOS wxSplitterScrolledWindow: public wxScrolledWindow
{
public:
    DECLARE_DYNAMIC_CLASS(wxSplitterScrolledWindow)

    wxSplitterScrolledWindow(wxWindow* parent, wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& sz = wxDefaultSize,
      long style = 0);

//// Overrides

//// Events

    void OnScroll(wxScrollWinEvent& event);
    void OnSize(wxSizeEvent& event);

//// Operations

//// Accessors

//// Data members
public:
    DECLARE_EVENT_TABLE()
};

#endif
        // _SPLITTREE_H_
