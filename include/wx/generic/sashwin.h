/////////////////////////////////////////////////////////////////////////////
// Name:        sashwin.h
// Purpose:     wxSashWindow implementation. A sash window has an optional
//              sash on each edge, allowing it to be dragged. An event
//              is generated when the sash is released.
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SASHWIN_H_G_
#define _WX_SASHWIN_H_G_

#ifdef __GNUG__
#pragma interface "sashwin.h"
#endif

#if wxUSE_SASH

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/string.h"

#define wxSASH_DRAG_NONE       0
#define wxSASH_DRAG_DRAGGING   1
#define wxSASH_DRAG_LEFT_DOWN  2

enum wxSashEdgePosition {
    wxSASH_TOP = 0,
    wxSASH_RIGHT,
    wxSASH_BOTTOM,
    wxSASH_LEFT,
    wxSASH_NONE = 100
};

/*
 * wxSashEdge represents one of the four edges of a window.
 */

class WXDLLEXPORT wxSashEdge
{
public:
    wxSashEdge() { m_show = FALSE; m_border = FALSE; m_margin = 0; }

    bool    m_show;     // Is the sash showing?
    bool    m_border;   // Do we draw a border?
    int     m_margin;   // The margin size
};

/*
 * wxSashWindow flags
 */

#define wxSW_NOBORDER         0x0000
//#define wxSW_3D               0x0010
#define wxSW_BORDER           0x0020
#define wxSW_3DSASH           0x0040
#define wxSW_3DBORDER         0x0080
#define wxSW_3D (wxSW_3DSASH | wxSW_3DBORDER)

/*
 * wxSashWindow allows any of its edges to have a sash which can be dragged
 * to resize the window. The actual content window will be created as a child
 * of wxSashWindow.
 */

class WXDLLEXPORT wxSashWindow: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxSashWindow)

public:

////////////////////////////////////////////////////////////////////////////
// Public API

    // Default constructor
    wxSashWindow()
    {
        Init();
    }

    // Normal constructor
    wxSashWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxSW_3D|wxCLIP_CHILDREN, const wxString& name = "sashWindow")
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    ~wxSashWindow();

    bool Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxSW_3D|wxCLIP_CHILDREN, const wxString& name = "sashWindow");

    // Set whether there's a sash in this position
    void SetSashVisible(wxSashEdgePosition edge, bool sash);

    // Get whether there's a sash in this position
    inline bool GetSashVisible(wxSashEdgePosition edge) const { return m_sashes[edge].m_show; }

    // Set whether there's a border in this position
    inline void SetSashBorder(wxSashEdgePosition edge, bool border) { m_sashes[edge].m_border = border; }

    // Get whether there's a border in this position
    inline bool HasBorder(wxSashEdgePosition edge) const { return m_sashes[edge].m_border; }

    // Get border size
    inline int GetEdgeMargin(wxSashEdgePosition edge) const { return m_sashes[edge].m_margin; }

    // Sets the default sash border size
    inline void SetDefaultBorderSize(int width) { m_borderSize = width; }

    // Gets the default sash border size
    inline int GetDefaultBorderSize() const { return m_borderSize; }

    // Sets the addition border size between child and sash window
    inline void SetExtraBorderSize(int width) { m_extraBorderSize = width; }

    // Gets the addition border size between child and sash window
    inline int GetExtraBorderSize() const { return m_extraBorderSize; }

    virtual void SetMinimumSizeX(int min) { m_minimumPaneSizeX = min; }
    virtual void SetMinimumSizeY(int min) { m_minimumPaneSizeY = min; }
    virtual int GetMinimumSizeX() const { return m_minimumPaneSizeX; }
    virtual int GetMinimumSizeY() const { return m_minimumPaneSizeY; }

    virtual void SetMaximumSizeX(int max) { m_maximumPaneSizeX = max; }
    virtual void SetMaximumSizeY(int max) { m_maximumPaneSizeY = max; }
    virtual int GetMaximumSizeX() const { return m_maximumPaneSizeX; }
    virtual int GetMaximumSizeY() const { return m_maximumPaneSizeY; }

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

    // Draws the sashes
    void DrawSash(wxSashEdgePosition edge, wxDC& dc);

    // Draws the sashes
    void DrawSashes(wxDC& dc);

    // Draws the sash tracker (for whilst moving the sash)
    void DrawSashTracker(wxSashEdgePosition edge, int x, int y);

    // Tests for x, y over sash
    wxSashEdgePosition SashHitTest(int x, int y, int tolerance = 2);

    // Resizes subwindows
    void SizeWindows();

    // Initialize colours
    void InitColours();

private:
    void Init();

    wxSashEdge  m_sashes[4];
    int         m_dragMode;
    wxSashEdgePosition m_draggingEdge;
    int         m_oldX;
    int         m_oldY;
    int         m_borderSize;
    int         m_extraBorderSize;
    int         m_firstX;
    int         m_firstY;
    int         m_minimumPaneSizeX;
    int         m_minimumPaneSizeY;
    int         m_maximumPaneSizeX;
    int         m_maximumPaneSizeY;
    wxCursor*   m_sashCursorWE;
    wxCursor*   m_sashCursorNS;
    wxColour    m_lightShadowColour;
    wxColour    m_mediumShadowColour;
    wxColour    m_darkShadowColour;
    wxColour    m_hilightColour;
    wxColour    m_faceColour;
    bool        m_mouseCaptured;

DECLARE_EVENT_TABLE()
};

#define wxEVT_SASH_DRAGGED (wxEVT_FIRST + 1200)

enum wxSashDragStatus
{
    wxSASH_STATUS_OK,
    wxSASH_STATUS_OUT_OF_RANGE
};

class WXDLLEXPORT wxSashEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxSashEvent)

 public:
    inline wxSashEvent(int id = 0, wxSashEdgePosition edge = wxSASH_NONE) {
     m_eventType = (wxEventType) wxEVT_SASH_DRAGGED; m_id = id; m_edge = edge; }

    inline void SetEdge(wxSashEdgePosition edge) { m_edge = edge; }
    inline wxSashEdgePosition GetEdge() const { return m_edge; }

    //// The rectangle formed by the drag operation
    inline void SetDragRect(const wxRect& rect) { m_dragRect = rect; }
    inline wxRect GetDragRect() const { return m_dragRect; }

    //// Whether the drag caused the rectangle to be reversed (e.g.
    //// dragging the top below the bottom)
    inline void SetDragStatus(wxSashDragStatus status) { m_dragStatus = status; }
    inline wxSashDragStatus GetDragStatus() const { return m_dragStatus; }
 private:
    wxSashEdgePosition  m_edge;
    wxRect              m_dragRect;
    wxSashDragStatus    m_dragStatus;
};

typedef void (wxEvtHandler::*wxSashEventFunction)(wxSashEvent&);

#define EVT_SASH_DRAGGED(id, fn) { wxEVT_SASH_DRAGGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSashEventFunction) & fn, NULL },
#define EVT_SASH_DRAGGED_RANGE(id1, id2, fn) { wxEVT_SASH_DRAGGED, id1, id2, (wxObjectEventFunction) (wxEventFunction) (wxSashEventFunction) & fn, NULL },

#endif // wxUSE_SASH

#endif
  // _WX_SASHWIN_H_G_
