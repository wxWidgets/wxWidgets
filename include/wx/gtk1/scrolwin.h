/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/scrolwin.h
// Purpose:     wxScrolledWindow class
// Author:      Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SCROLLWIN_H_
#define _WX_GTK_SCROLLWIN_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "scrolwin.h"
#endif

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/window.h"
#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxPanelNameStr;

// default scrolled window style
#ifndef wxScrolledWindowStyle
#define wxScrolledWindowStyle (wxHSCROLL | wxVSCROLL)
#endif

// ----------------------------------------------------------------------------
// wxScrolledWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrolledWindow : public wxPanel
{
public:
    wxScrolledWindow()
        { Init(); }

    wxScrolledWindow(wxWindow *parent,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
        { Create(parent, id, pos, size, style, name); }

    void Init();

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
    virtual void SetTargetWindow( wxWindow *target, bool pushEventHandler = FALSE );
    virtual wxWindow *GetTargetWindow() const;

    // Set the scrolled area of the window.
    virtual void DoSetVirtualSize( int x, int y );

    // Set the x, y scrolling increments.
    void SetScrollRate( int xstep, int ystep );

    // Number of pixels per user unit (0 or -1 for no scrollbar)
    // Length of virtual canvas in user units
    // Length of page in user units
    // Default action is to set the virtual size and alter scrollbars
    // accordingly.
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
    // If TRUE, wxWidgets scrolls the canvas and only a bit of
    // the canvas is invalidated; no Clear() is necessary.
    // If FALSE, the whole canvas is invalidated and a Clear() is
    // necessary. Disable for when the scroll increment is used
    // to actually scroll a non-constant distance
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);

    // Get the view start
    virtual void GetViewStart(int *x, int *y) const;

    // translate between scrolled and unscrolled coordinates
    void CalcScrolledPosition(int x, int y, int *xx, int *yy) const
        {  DoCalcScrolledPosition(x, y, xx, yy); }
    wxPoint CalcScrolledPosition(const wxPoint& pt) const
    {
        wxPoint p2;
        DoCalcScrolledPosition(pt.x, pt.y, &p2.x, &p2.y);
        return p2;
    }

    void CalcUnscrolledPosition(int x, int y, int *xx, int *yy) const
        {  DoCalcUnscrolledPosition(x, y, xx, yy); }
    wxPoint CalcUnscrolledPosition(const wxPoint& pt) const
    {
        wxPoint p2;
        DoCalcUnscrolledPosition(pt.x, pt.y, &p2.x, &p2.y);
        return p2;
    }

    virtual void DoCalcScrolledPosition(int x, int y, int *xx, int *yy) const;
    virtual void DoCalcUnscrolledPosition(int x, int y, int *xx, int *yy) const;

    // Override this function to draw the graphic (or just process EVT_PAINT)
    virtual void OnDraw(wxDC& WXUNUSED(dc)) {}

    // Override this function if you don't want to have wxScrolledWindow
    // automatically change the origin according to the scroll position.
    void PrepareDC(wxDC& dc) { DoPrepareDC(dc); }

    // lay out the window and its children
    virtual bool Layout();

    // Adjust the scrollbars
    virtual void AdjustScrollbars();

    // Set the scale factor, used in PrepareDC
    void SetScale(double xs, double ys) { m_scaleX = xs; m_scaleY = ys; }
    double GetScaleX() const { return m_scaleX; }
    double GetScaleY() const { return m_scaleY; }

    // implementation from now on
    void OnScroll(wxScrollWinEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnChar(wxKeyEvent& event);

    void GtkVScroll( float value, unsigned int scroll_type );
    void GtkHScroll( float value, unsigned int scroll_type );
    void GtkVConnectEvent();
    void GtkHConnectEvent();
    void GtkVDisconnectEvent();
    void GtkHDisconnectEvent();

    // Calculate scroll increment
    virtual int CalcScrollInc(wxScrollWinEvent& event);

    // Overridden from wxWidgets due callback being static
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );

#if WXWIN_COMPATIBILITY_2_2
    // Compatibility
    void ViewStart(int *x, int *y) const { GetViewStart( x, y ); }
#endif // WXWIN_COMPATIBILITY_2_2

    virtual void DoPrepareDC(wxDC& dc);

protected:
    wxWindow             *m_targetWindow;
    int                   m_xScrollPixelsPerLine;
    int                   m_yScrollPixelsPerLine;
    bool                  m_xScrollingEnabled;
    bool                  m_yScrollingEnabled;

    // FIXME: these next four members are duplicated in the GtkAdjustment
    //        members of wxWindow.  Can they be safely removed from here?

    int                   m_xScrollPosition;
    int                   m_yScrollPosition;
    int                   m_xScrollLinesPerPage;
    int                   m_yScrollLinesPerPage;

    double                m_scaleY,m_scaleX;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_ABSTRACT_CLASS(wxScrolledWindow)
};

#endif
    // _WX_GTK_SCROLLWIN_H_

// vi:sts=4:sw=4:et
