/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/scrolwin.h
// Purpose:     wxScrolledWindow class
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SCROLLWIN_H_
#define _WX_GTK_SCROLLWIN_H_

#ifdef __GNUG__
    #pragma interface "scrolwin.h"
#endif

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/window.h"
#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

// default scrolled window style
#define wxScrolledWindowStyle (wxHSCROLL | wxVSCROLL)

// ----------------------------------------------------------------------------
// wxScrolledWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrolledWindow : public wxPanel
{
public:
    wxScrolledWindow();
    wxScrolledWindow(wxWindow *parent,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
    {
        Create(parent, id, pos, size, style, name);
    }

    ~wxScrolledWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxPanelNameStr);

    // Normally the wxScrolledWindow will scroll itself, but in
    // some rare occasions you might want it to scroll another
    // window (e.g. a child of it in order to scroll only a portion
    // the area between the scrollbars (spreadsheet: only cell area
    // will move).
    virtual void SetTargetWindow( wxWindow *target );
    virtual wxWindow *GetTargetWindow();

    // Number of pixels per user unit (0 or -1 for no scrollbar)
    // Length of virtual canvas in user units
    // Length of page in user units
    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                             int noUnitsX, int noUnitsY,
                             int xPos = 0, int yPos = 0,
                             bool noRefresh = FALSE );

    // Physically scroll the window
    virtual void Scroll(int x_pos, int y_pos);

    int GetScrollPageSize(int orient) const;
    void SetScrollPageSize(int orient, int pageSize);

    virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) const;

    // Enable/disable Windows scrolling in either direction.
    // If TRUE, wxWindows scrolls the canvas and only a bit of
    // the canvas is invalidated; no Clear() is necessary.
    // If FALSE, the whole canvas is invalidated and a Clear() is
    // necessary. Disable for when the scroll increment is used
    // to actually scroll a non-constant distance
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);

    // Get the view start
    virtual void GetViewStart(int *x, int *y) const;
    // Compatibility
    void ViewStart(int *x, int *y) const
       { GetViewStart( x, y ); }

    // Actual size in pixels when scrolling is taken into account
    virtual void GetVirtualSize(int *x, int *y) const;

    virtual void CalcScrolledPosition(int x, int y, int *xx, int *yy) const;
    virtual void CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const;

    // Override this function to draw the graphic (or just process EVT_PAINT)
    virtual void OnDraw(wxDC& WXUNUSED(dc)) {}

    // Override this function if you don't want to have wxScrolledWindow
    // automatically change the origin according to the scroll position.
    virtual void PrepareDC(wxDC& dc);

    // Adjust the scrollbars
    virtual void AdjustScrollbars();
    
    // implementation from now on
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnChar(wxKeyEvent& event);
    
    void GtkVScroll( float value );
    void GtkHScroll( float value );

protected:
    wxWindow             *m_targetWindow;
    int                   m_xScrollPixelsPerLine;
    int                   m_yScrollPixelsPerLine;
    bool                  m_xScrollingEnabled;
    bool                  m_yScrollingEnabled;
    int                   m_xScrollPosition;
    int                   m_yScrollPosition;
    int                   m_xScrollLines;
    int                   m_yScrollLines;
    int                   m_xScrollLinesPerPage;
    int                   m_yScrollLinesPerPage;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_ABSTRACT_CLASS(wxScrolledWindow)
};

#endif
    // _WX_GTK_SCROLLWIN_H_
