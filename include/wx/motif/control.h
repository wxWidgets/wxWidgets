/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#ifdef __GNUG__
#pragma interface "control.h"
#endif

#include "wx/window.h"
#include "wx/list.h"
#include "wx/validate.h"

#define wxControlNameStr _T("control")

// General item class
class WXDLLEXPORT wxControl: public wxWindow
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
    wxControl();
    wxControl( wxWindow *parent,
               wxWindowID id,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxControlNameStr );

    ~wxControl();

    // simulates the event, returns TRUE if the event was processed
    virtual void Command(wxCommandEvent& WXUNUSED(event)) { }

    // calls the callback and appropriate event handlers, returns TRUE if
    // event was processed
    virtual bool ProcessCommand(wxCommandEvent& event);

    virtual void SetLabel(const wxString& label);
    virtual wxString GetLabel() const ;

#if WXWIN_COMPATIBILITY
    void Callback(const wxFunction function) { m_callback = function; }; // Adds callback

    wxFunction GetCallback() { return m_callback; }
#endif // WXWIN_COMPATIBILITY

    bool InSetValue() const { return m_inSetValue; }

protected:
#if WXWIN_COMPATIBILITY
    wxFunction          m_callback;     // Callback associated with the window
#endif // WXWIN_COMPATIBILITY

    bool                m_inSetValue;   // Motif: prevent callbacks being called while
                                        // in SetValue

    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_CONTROL_H_
