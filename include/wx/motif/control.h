/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#ifdef __GNUG__
#pragma interface "control.h"
#endif

#include "wx/window.h"
#include "wx/list.h"
#include "wx/validate.h"

// General item class
class WXDLLEXPORT wxControl: public wxWindow
{
    DECLARE_ABSTRACT_CLASS(wxControl)
public:
    wxControl();
    ~wxControl();

    virtual void Command(wxCommandEvent& WXUNUSED(event)) {};        // Simulates an event
    virtual void ProcessCommand(wxCommandEvent& event); // Calls the callback and
                                                                 // appropriate event handlers
    virtual void SetLabel(const wxString& label);
    virtual wxString GetLabel() const ;

    // Places item in centre of panel - so can't be used BEFORE panel->Fit()
    void Centre(int direction = wxHORIZONTAL);
    inline void Callback(const wxFunction function) { m_callback = function; }; // Adds callback

    inline wxFunction GetCallback() { return m_callback; }

    inline bool InSetValue() const { return m_inSetValue; }
protected:
    wxFunction          m_callback;     // Callback associated with the window

    bool                m_inSetValue;   // Motif: prevent callbacks being called while
                                        // in SetValue

    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_CONTROL_H_
