/////////////////////////////////////////////////////////////////////////////
// Name:        wx/spinbutt.h
// Purpose:     wxSpinButtonBase class
// Author:      Julian Smart, Vadim Zeitlin
// Modified by:
// Created:     23.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_BASE_
#define _WX_SPINBUTT_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#ifdef wxUSE_SPINBTN

#include "wx/control.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
//  The wxSpinButton is like a small scrollbar than is often placed next
//  to a text control.
//
//  Styles:
//  wxSP_HORIZONTAL:   horizontal spin button
//  wxSP_VERTICAL:     vertical spin button (the default)
//  wxSP_ARROW_KEYS:   arrow keys increment/decrement value
//  wxSP_WRAP:         value wraps at either end
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinButtonBase : public wxControl
{
public:
    wxSpinButtonBase() { InitBase(); }

    // accessors
    virtual int GetValue() const = 0;
    virtual int GetMin() const { return m_min; }
    virtual int GetMax() const { return m_max; }

    // operations
    virtual void SetValue(int val) = 0;
    virtual void SetRange(int minVal, int maxVal)
    {
        m_min = minVal;
        m_max = maxVal;
    }

protected:
    // init the base part of the control
    void InitBase()
    {
        m_min = 0;
        m_max = 100;
    }

    // the range value
    int   m_min;
    int   m_max;
};

// ----------------------------------------------------------------------------
// include the declaration of the real class
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/spinbutt.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/spinbutt.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/spinbutt.h"
#elif defined(__WXQT__)
    #include "wx/qt/spinbutt.h"
#elif defined(__WXMAC__)
    #include "wx/mac/spinbutt.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/spinbutt.h"
#endif

// ----------------------------------------------------------------------------
// the wxSpinButton event
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinEvent : public wxScrollEvent
{
    DECLARE_DYNAMIC_CLASS(wxSpinEvent)

public:
    wxSpinEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
           : wxScrollEvent(commandType, id)
    {
    }
};

typedef void (wxEvtHandler::*wxSpinEventFunction)(wxSpinEvent&);

// macros for handling spin events
#define EVT_SPIN_UP(id, func) { wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func }
#define EVT_SPIN_DOWN(id, func) { wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func }

#define EVT_SPIN(id, func) \
  { wxEVT_SCROLL_TOP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_BOTTOM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_PAGEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_PAGEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_THUMBTRACK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },

#endif // wxUSE_SPINBTN

#endif
    // _WX_SPINBUTT_H_BASE_
