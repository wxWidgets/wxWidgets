/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPINBUTT_H_
#define _WX_SPINBUTT_H_

#ifdef __GNUG__
#pragma interface "spinbutt.h"
#endif

#include "wx/control.h"
#include "wx/event.h"

#if defined(__WIN95__)

/*
    The wxSpinButton is like a small scrollbar than is often placed next
    to a text control.

    wxSP_HORIZONTAL:   horizontal spin button
    wxSP_VERTICAL:     vertical spin button (the default)
    wxSP_ARROW_KEYS:   arrow keys increment/decrement value
    wxSP_WRAP:         value wraps at either end
 */

class WXDLLEXPORT wxSpinButton : public wxControl
{
DECLARE_DYNAMIC_CLASS(wxSpinButton)

public:
    /*
     * Public interface
     */
    wxSpinButton();
    
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL | wxSP_ARROW_KEYS,
                 const wxString& name = "wxSpinButton")
    {
        Create(parent, id, pos, size, style, name);
    }
    
    virtual ~wxSpinButton();
    
    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL | wxSP_ARROW_KEYS, 
                const wxString& name = "wxSpinButton");
    
    
    // Attributes
    ////////////////////////////////////////////////////////////////////////////
    
    int GetValue() const ;
    void SetValue(int val) ;
    void SetRange(int minVal, int maxVal);
    int GetMin() const { return m_min; }
    int GetMax() const { return m_max; }
    
    // Operations
    ////////////////////////////////////////////////////////////////////////////
    
    void Command(wxCommandEvent& event) { ProcessCommand(event); };
    
    // IMPLEMENTATION
    virtual bool MSWCommand(WXUINT param, WXWORD id);
    virtual bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam, WXLPARAM *result);
    virtual void MSWOnVScroll(WXWORD wParam, WXWORD pos, WXHWND control);
    virtual void MSWOnHScroll(WXWORD wParam, WXWORD pos, WXHWND control);
    
protected:
    int   m_min;
    int   m_max;
};

class WXDLLEXPORT wxSpinEvent: public wxScrollEvent
{
    DECLARE_DYNAMIC_CLASS(wxSpinEvent)
        
public:
    wxSpinEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
};

typedef void (wxEvtHandler::*wxSpinEventFunction)(wxSpinEvent&);

// Spin events

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

#endif
    // _WX_WIN95__
#endif
    // _WX_SPINBUTT_H_
