/////////////////////////////////////////////////////////////////////////////
// Name:        wx/control.h
// Purpose:     wxControl common interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_BASE_
#define _WX_CONTROL_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "controlbase.h"
#endif

#if wxUSE_CONTROLS

#include "wx/window.h"      // base class

extern const wxChar* wxControlNameStr; // the default name

// ----------------------------------------------------------------------------
// wxControl is the base class for all controls
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlBase : public wxWindow
{
public:
    // Create() function adds the validator parameter
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxControlNameStr);

    // simulates the event of given type (i.e. wxButton::Command() is just as
    // if the button was clicked)
    virtual void Command(wxCommandEvent &event);

    // get the control alignment (left/right/centre, top/bottom/centre)
    int GetAlignment() const { return m_windowStyle & wxALIGN_MASK; }

protected:
    // creates the control (calls wxWindowBase::CreateBase inside) and adds it
    // to the list of parents children
    bool CreateControl(wxWindowBase *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name);

    // inherit colour and font settings from the parent window
    void InheritAttributes();

    // initialize the common fields of wxCommandEvent
    void InitCommandEvent(wxCommandEvent& event) const;
};

// ----------------------------------------------------------------------------
// include platform-dependent wxControl declarations
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/control.h"
#elif defined(__WXMSW__)
    #include "wx/msw/control.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/control.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/control.h"
#elif defined(__WXQT__)
    #include "wx/qt/control.h"
#elif defined(__WXMAC__)
    #include "wx/mac/control.h"
#elif defined(__WXPM__)
    #include "wx/os2/control.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/control.h"
#endif

#endif // wxUSE_CONTROLS

#endif
    // _WX_CONTROL_H_BASE_
