/////////////////////////////////////////////////////////////////////////////
// Name:        laywin.h
// Purpose:     Implements a simple layout algorithm, plus
//              wxSashLayoutWindow which is an example of a window with
//              layout-awareness (via event handlers). This is suited to
//              IDE-style window layout.
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LAYWIN_H_G_
#define _WX_LAYWIN_H_G_

#ifdef __GNUG__
#pragma interface "laywin.h"
#endif

#include "wx/sashwin.h"

const wxEventType wxEVT_QUERY_LAYOUT_INFO =     wxEVT_FIRST + 1500;
const wxEventType wxEVT_CALCULATE_LAYOUT =      wxEVT_FIRST + 1501;

enum wxLayoutOrientation {
    wxLAYOUT_HORIZONTAL,
    wxLAYOUT_VERTICAL
};

enum wxLayoutAlignment {
    wxLAYOUT_NONE,
    wxLAYOUT_TOP,
    wxLAYOUT_LEFT,
    wxLAYOUT_RIGHT,
    wxLAYOUT_BOTTOM,
};

// Not sure this is necessary
// Tell window which dimension we're sizing on
#define wxLAYOUT_LENGTH_Y       0x0008
#define wxLAYOUT_LENGTH_X       0x0000

// Use most recently used length
#define wxLAYOUT_MRU_LENGTH     0x0010

// Only a query, so don't actually move it.
#define wxLAYOUT_QUERY          0x0100

/*
 * This event is used to get information about window alignment,
 * orientation and size.
 */

class wxQueryLayoutInfoEvent: public wxEvent
{
DECLARE_DYNAMIC_CLASS(wxQueryLayoutInfoEvent)
public:

    wxQueryLayoutInfoEvent(wxWindowID id = 0)
    {
        SetEventType(wxEVT_QUERY_LAYOUT_INFO);
        m_requestedLength = 0;
        m_flags = 0;
        m_id = id;
        m_alignment = wxLAYOUT_TOP;
        m_orientation = wxLAYOUT_HORIZONTAL;
    }
// Read by the app
    inline void SetRequestedLength(int length) { m_requestedLength = length; }
    inline int GetRequestedLength() const { return m_requestedLength; }

    inline void SetFlags(int flags) { m_flags = flags; }
    inline int GetFlags() const { return m_flags; }

// Set by the app
    inline void SetSize(const wxSize& size) { m_size = size; }
    inline wxSize GetSize() const { return m_size; }

    inline void SetOrientation(wxLayoutOrientation orient) { m_orientation = orient; }
    inline wxLayoutOrientation GetOrientation() const { return m_orientation; }

    inline void SetAlignment(wxLayoutAlignment align) { m_alignment = align; }
    inline wxLayoutAlignment GetAlignment() const { return m_alignment; }
protected:
    int                     m_flags;
    int                     m_requestedLength;
    wxSize                  m_size;
    wxLayoutOrientation     m_orientation;
    wxLayoutAlignment       m_alignment;

};

typedef void (wxEvtHandler::*wxQueryLayoutInfoEventFunction)(wxQueryLayoutInfoEvent&);

#define EVT_QUERY_LAYOUT_INFO(func)  { wxEVT_QUERY_LAYOUT_INFO, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxQueryLayoutInfoEventFunction) & func, NULL },

/*
 * This event is used to take a bite out of the available client area.
 */

class wxCalculateLayoutEvent: public wxEvent
{
DECLARE_DYNAMIC_CLASS(wxCalculateLayoutEvent)
public:
    wxCalculateLayoutEvent(wxWindowID id = 0)
    {
        SetEventType(wxEVT_CALCULATE_LAYOUT);
        m_flags = 0;
	m_id = id;
    }
// Read by the app
    inline void SetFlags(int flags) { m_flags = flags; }
    inline int GetFlags() const { return m_flags; }

// Set by the app
    inline void SetRect(const wxRect& rect) { m_rect = rect; }
    inline wxRect GetRect() const { return m_rect; }
protected:
    int                     m_flags;
    wxRect                  m_rect;
};

typedef void (wxEvtHandler::*wxCalculateLayoutEventFunction)(wxCalculateLayoutEvent&);

#define EVT_CALCULATE_LAYOUT(func)  { wxEVT_CALCULATE_LAYOUT, -1, -1, (wxObjectEventFunction) (wxEventFunction) (wxCalculateLayoutEventFunction) & func, NULL },

// This is window that can remember alignment/orientation, does its own layout,
// and can provide sashes too. Useful for implementing docked windows with sashes in
// an IDE-style interface.
class wxSashLayoutWindow: public wxSashWindow
{
    DECLARE_CLASS(wxSashLayoutWindow)
public:
    wxSashLayoutWindow(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxSW_3D|wxCLIP_CHILDREN, const wxString& name = "layoutWindow");

// Accessors
    inline wxLayoutAlignment GetAlignment() const { return m_alignment; };
    inline wxLayoutOrientation GetOrientation() const { return m_orientation; };

    inline void SetAlignment(wxLayoutAlignment align) { m_alignment = align; };
    inline void SetOrientation(wxLayoutOrientation orient) { m_orientation = orient; };

    // Give the window default dimensions
    inline void SetDefaultSize(const wxSize& size) { m_defaultSize = size; }

// Event handlers
    // Called by layout algorithm to allow window to take a bit out of the
    // client rectangle, and size itself if not in wxLAYOUT_QUERY mode.
    void OnCalculateLayout(wxCalculateLayoutEvent& event);

    // Called by layout algorithm to retrieve information about the window.
    void OnQueryLayoutInfo(wxQueryLayoutInfoEvent& event);
protected:
    wxLayoutAlignment           m_alignment;
    wxLayoutOrientation         m_orientation;
    wxSize                      m_defaultSize;

DECLARE_EVENT_TABLE()
};

class WXDLLEXPORT wxMDIParentFrame;
class WXDLLEXPORT wxFrame;

// This class implements the layout algorithm
class wxLayoutAlgorithm: public wxObject
{
public:
    wxLayoutAlgorithm() {}

    // The MDI client window is sized to whatever's left over.
    bool LayoutMDIFrame(wxMDIParentFrame* frame);

    // mainWindow is sized to whatever's left over.
    bool LayoutFrame(wxFrame* frame, wxWindow* mainWindow);
};

#endif
    // _WX_LAYWIN_H_G_
