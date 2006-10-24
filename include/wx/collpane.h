/////////////////////////////////////////////////////////////////////////////
// Name:        wx/collpane.h
// Purpose:     wxCollapsiblePane
// Author:      Francesco Montorsi
// Modified by:
// Created:     8/10/2006
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLLAPSABLE_PANE_H_BASE_
#define _WX_COLLAPSABLE_PANE_H_BASE_

#include "wx/control.h"


// ----------------------------------------------------------------------------
// wxCollapsiblePaneBase: interface for wxCollapsiblePane
// ----------------------------------------------------------------------------

#define wxCP_DEFAULT_STYLE          (0)

class WXDLLEXPORT wxCollapsiblePaneBase : public wxControl
{
public:
    wxCollapsiblePaneBase() {}

    virtual void Collapse(bool collapse = true) = 0;
    void Expand() { Collapse(false); }

    virtual bool IsCollapsed() const = 0;
    bool IsExpanded() const { return !IsCollapsed(); }

    virtual wxWindow *GetPane() const = 0;

    virtual wxString GetLabel() const = 0;
    virtual void SetLabel(const wxString& label) = 0;
};


// ----------------------------------------------------------------------------
// event types and macros
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_ADV, wxEVT_COMMAND_COLLPANE_CHANGED, 1102)
END_DECLARE_EVENT_TYPES()

class WXDLLIMPEXP_ADV wxCollapsiblePaneEvent : public wxCommandEvent
{
public:
    wxCollapsiblePaneEvent() {}
    wxCollapsiblePaneEvent(wxObject *generator, int id, bool collapsed)
        : wxCommandEvent(wxEVT_COMMAND_COLLPANE_CHANGED, id),
        m_bCollapsed(collapsed)
    {
        SetEventObject(generator);
    }

    bool GetCollapsed() const { return m_bCollapsed; }
    void SetCollapsed(bool c) { m_bCollapsed = c; }


    // default copy ctor, assignment operator and dtor are ok
    virtual wxEvent *Clone() const { return new wxCollapsiblePaneEvent(*this); }

private:
    bool m_bCollapsed;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxCollapsiblePaneEvent)
};

// ----------------------------------------------------------------------------
// event types and macros
// ----------------------------------------------------------------------------

typedef void (wxEvtHandler::*wxCollapsiblePaneEventFunction)(wxCollapsiblePaneEvent&);

#define wxCollapsiblePaneEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCollapsiblePaneEventFunction, &func)

#define EVT_COLLAPSIBLEPANE_CHANGED(id, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_COLLPANE_CHANGED, id, wxCollapsiblePaneEventFunction(fn))


#if defined(__WXGTK24__)
    #include "wx/gtk/collpane.h"
#else
    #include "wx/generic/collpaneg.h"
    #define wxCollapsiblePane   wxGenericCollapsiblePane
#endif

#endif
    // _WX_COLLAPSABLE_PANE_H_BASE_
