/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/scrolwin.h
// Purpose:     wxScrolledWindow, wxScrolledControl and wxScrollHelper
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLWIN_H_BASE_
#define _WX_SCROLWIN_H_BASE_

#include "wx/window.h"

class WXDLLEXPORT wxScrollHelperEvtHandler;
class WXDLLEXPORT wxTimer;

// ----------------------------------------------------------------------------
// wxScrollHelper: this class implements the scrolling logic which is used by
// wxScrolledWindow and wxScrolledControl. It is a mix-in: just derive from it
// to implement scrolling in your class.
// ----------------------------------------------------------------------------
#if !defined(__WXGTK__) || defined(__WXUNIVERSAL__)

class WXDLLEXPORT wxScrollHelper
{
public:
    // ctor and dtor
    wxScrollHelper(wxWindow *winToScroll = (wxWindow *)NULL);
    void SetWindow(wxWindow *winToScroll);
    virtual ~wxScrollHelper();

    // configure the scrolling
    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                               int noUnitsX, int noUnitsY,
                               int xPos = 0, int yPos = 0,
                               bool noRefresh = false );

    // scroll to the given (in logical coords) position
    virtual void Scroll(int x, int y);

    // get/set the page size for this orientation (wxVERTICAL/wxHORIZONTAL)
    int GetScrollPageSize(int orient) const;
    void SetScrollPageSize(int orient, int pageSize);

    // Set the x, y scrolling increments.
    void SetScrollRate( int xstep, int ystep );

    // get the size of one logical unit in physical ones
    virtual void GetScrollPixelsPerUnit(int *pixelsPerUnitX,
                                        int *pixelsPerUnitY) const;

    // Enable/disable Windows scrolling in either direction. If true, wxWidgets
    // scrolls the canvas and only a bit of the canvas is invalidated; no
    // Clear() is necessary. If false, the whole canvas is invalidated and a
    // Clear() is necessary. Disable for when the scroll increment is used to
    // actually scroll a non-constant distance
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);

    // Get the view start
    virtual void GetViewStart(int *x, int *y) const;

    // Set the scale factor, used in PrepareDC
    void SetScale(double xs, double ys) { m_scaleX = xs; m_scaleY = ys; }
    double GetScaleX() const { return m_scaleX; }
    double GetScaleY() const { return m_scaleY; }

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

    // Adjust the scrollbars
    virtual void AdjustScrollbars(void);

    // Calculate scroll increment
    virtual int CalcScrollInc(wxScrollWinEvent& event);

    // Normally the wxScrolledWindow will scroll itself, but in some rare
    // occasions you might want it to scroll [part of] another window (e.g. a
    // child of it in order to scroll only a portion the area between the
    // scrollbars (spreadsheet: only cell area will move).
    virtual void SetTargetWindow(wxWindow *target);
    virtual wxWindow *GetTargetWindow() const;

    void SetTargetRect(const wxRect& rect) { m_rectToScroll = rect; }
    wxRect GetTargetRect() const { return m_rectToScroll; }

    // Override this function to draw the graphic (or just process EVT_PAINT)
    virtual void OnDraw(wxDC& WXUNUSED(dc)) { }

    // change the DC origin according to the scroll position.
    virtual void DoPrepareDC(wxDC& dc);

    // are we generating the autoscroll events?
    bool IsAutoScrolling() const { return m_timerAutoScroll != NULL; }

    // stop generating the scroll events when mouse is held outside the window
    void StopAutoScrolling();

    // this method can be overridden in a derived class to forbid sending the
    // auto scroll events - note that unlike StopAutoScrolling() it doesn't
    // stop the timer, so it will be called repeatedly and will typically
    // return different values depending on the current mouse position
    //
    // the base class version just returns true
    virtual bool SendAutoScrollEvents(wxScrollWinEvent& event) const;

    // the methods to be called from the window event handlers
    void HandleOnScroll(wxScrollWinEvent& event);
    void HandleOnSize(wxSizeEvent& event);
    void HandleOnPaint(wxPaintEvent& event);
    void HandleOnChar(wxKeyEvent& event);
    void HandleOnMouseEnter(wxMouseEvent& event);
    void HandleOnMouseLeave(wxMouseEvent& event);
#if wxUSE_MOUSEWHEEL
    void HandleOnMouseWheel(wxMouseEvent& event);
#endif // wxUSE_MOUSEWHEEL

    // FIXME: this is needed for now for wxPlot compilation, should be removed
    //        once it is fixed!
    void OnScroll(wxScrollWinEvent& event) { HandleOnScroll(event); }

#if WXWIN_COMPATIBILITY_2_2
    // Compatibility only, don't use
    wxDEPRECATED( void ViewStart(int *x, int *y) const );
#endif // WXWIN_COMPATIBILITY_2_2

protected:
    // get pointer to our scroll rect if we use it or NULL
    const wxRect *GetScrollRect() const
    {
        return m_rectToScroll.width != 0 ? &m_rectToScroll : NULL;
    }

    // get the size of the target window
    wxSize GetTargetSize() const
    {
        return m_rectToScroll.width != 0 ? m_rectToScroll.GetSize()
                                         : m_targetWindow->GetClientSize();
    }

    void GetTargetSize(int *w, int *h)
    {
        wxSize size = GetTargetSize();
        if ( w )
            *w = size.x;
        if ( h )
            *h = size.y;
    }

    // change just the target window (unlike SetWindow which changes m_win as
    // well)
    void DoSetTargetWindow(wxWindow *target);

    // delete the event handler we installed
    void DeleteEvtHandler();

    wxWindow             *m_win,
                         *m_targetWindow;

    wxRect                m_rectToScroll;

    wxTimer              *m_timerAutoScroll;

    int                   m_xScrollPixelsPerLine;
    int                   m_yScrollPixelsPerLine;
    int                   m_xScrollPosition;
    int                   m_yScrollPosition;
    int                   m_xScrollLines;
    int                   m_yScrollLines;
    int                   m_xScrollLinesPerPage;
    int                   m_yScrollLinesPerPage;

    bool                  m_xScrollingEnabled;
    bool                  m_yScrollingEnabled;

    double                m_scaleX;
    double                m_scaleY;

#if wxUSE_MOUSEWHEEL
    int m_wheelRotation;
#endif // wxUSE_MOUSEWHEEL

    wxScrollHelperEvtHandler *m_handler;

    DECLARE_NO_COPY_CLASS(wxScrollHelper)
};

#endif

// ----------------------------------------------------------------------------
// wxScrolledWindow: a wxWindow which knows how to scroll
// ----------------------------------------------------------------------------

#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    #include "wx/gtk/scrolwin.h"
#else // !wxGTK
    #include "wx/generic/scrolwin.h"

    class WXDLLEXPORT wxScrolledWindow : public wxGenericScrolledWindow
    {
    public:
        wxScrolledWindow() { }
        wxScrolledWindow(wxWindow *parent,
                         wxWindowID winid = wxID_ANY,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxScrolledWindowStyle,
                         const wxString& name = wxPanelNameStr)
            : wxGenericScrolledWindow(parent, winid, pos, size, style, name)
        {
        }

    private:
        DECLARE_DYNAMIC_CLASS_NO_COPY(wxScrolledWindow)
    };

    #define wxSCROLLED_WINDOW_IS_GENERIC 1
#endif

#endif
    // _WX_SCROLWIN_H_BASE_

