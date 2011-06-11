/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/vscroll.h
// Purpose:     wxMoVScrolledWindow class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_VSCROLL_H_
#define _WX_MOBILE_GENERIC_VSCROLL_H_

// ----------------------------------------------------------------------------
// headers and constants
// ----------------------------------------------------------------------------

#include "wx/scrolwin.h"
#include "wx/timer.h"
#include "wx/scrolbar.h"

#include "wx/mobile/window.h"
#include "wx/mobile/scrollwin.h"

#ifdef _WX_VSCROLL_H_
#undef _WX_VSCROLL_H_
#endif


#define wxPanel wxMoPanel

// This include defines a new class wxMoVScrolledWindowBase by using
// the define above. We then use it as the base class for wxMoVScrolledWindow.
// We need to do this so that the class uses wxMoPanel as its base instead
// of wxPanel, so we get more iPhone-like scroll facilities.
//
// See above for OS X note.
#include "wx/vscroll.h"


/**
    @class wxMoTouchVScrollHelper

    A scroll helper for wxMoScrolledWindow.

    @category{wxMobileImplementation}
*/

class wxMoTouchVScrollHelper: public wxMoTouchScrollHelperBase
{
    DECLARE_CLASS(wxMoTouchVScrollHelper)
public:
    wxMoTouchVScrollHelper(wxWindow* scrolledWindow): wxMoTouchScrollHelperBase(scrolledWindow) {}

    void Init();

    /// Returns the details to be used for the transient scrollbars.
    /// Returns false if this orientation not supported.
    virtual bool GetScrollBarDetails(int orientation,
        int* range, int* thumbPosition, int* thumbSize);

    /// Does the scrolling for the specific scrolled window type.
    virtual bool DoScrolling(const wxPoint& lastPos, const wxPoint& newPos);

    /// Override to do processing on start pos
    virtual void OnStartScrolling(const wxPoint& startPos);

protected:

    int m_firstVisibleLine;
};

/**
    @class wxMoVScrolledWindow

    A scrolled window implementation; use this in place of wxScrolledWindow.
    When you use SetScrollbars, use 1 pixel per unit for smooth scrolling.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoVScrolledWindow: public wxVScrolledWindow
{
public:
    /// Default constructor.
    wxMoVScrolledWindow();

    /// Constructor.
    wxMoVScrolledWindow(wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr): m_touchScrollHelper(this)
    {
        Init();

        Create(parent, winid, pos, size, style, name);
    }

    virtual ~wxMoVScrolledWindow();

    void Init();

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxPanelNameStr);

    /// Sets the flag determining whether the horizontal scroll indicator is displayed.
    void SetShowHorizontalScrollIndicator(bool show) { m_showHorizontalScrollIndicator = show; }

    /// Gets the flag determining whether the horizontal scroll indicator is displayed.
    bool GetShowHorizontalScrollIndicator() const { return m_showHorizontalScrollIndicator; }

    /// Sets the flag determining whether the vertical scroll indicator is displayed.
    void SetShowVerticalScrollIndicator(bool show) { m_showVerticalScrollIndicator = show; }

    /// Gets the flag determining whether the vertical scroll indicator is displayed.
    bool GetShowVerticalScrollIndicator() const { return m_showVerticalScrollIndicator; }

    /// Set the delay before assuming touch is for content, in milliseconds.
    void SetContentProcessingDelay(int delay) { m_touchScrollHelper.SetContentProcessingDelay(delay); }

    /// Get the delay before assuming touch is for content, in milliseconds.
    int GetContentProcessingDelay() const { return m_touchScrollHelper.GetContentProcessingDelay(); }

    int GetAverageLineHeight() const;
    int GetEstimatedTotalHeight() const;

protected:

    wxMoTouchVScrollHelper m_touchScrollHelper;

private:
    DECLARE_CLASS(wxMoVScrolledWindow)
    DECLARE_EVENT_TABLE()

    bool    m_showHorizontalScrollIndicator;
    bool    m_showVerticalScrollIndicator;
};

#endif // _WX_MOBILE_GENERIC_VSCROLL_H_

