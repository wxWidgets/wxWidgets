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

#include "wx/window.h"      // base class

// ----------------------------------------------------------------------------
// wxControl is the base class for all controls
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlBase : public wxWindow
{
public:
    // simulates the event of given type (i.e. wxButton::Command() is just as
    // if the button was clicked)
    virtual void Command(wxCommandEvent &event);

protected:
    // creates the control (calls wxWindowBase::CreateBase inside) and adds it
    // to the list of parents children
    bool CreateControl(wxWindowBase *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
                       const wxValidator* validator,
#  else
                       const wxValidator& validator,
#  endif
#endif
                       const wxString& name);

    // an overloaded version for the controls without validators
    bool CreateControl(wxWindowBase *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
    {
        return CreateControl(parent, id, pos, size, style,
                             wxDefaultValidator, name);
    }

    // inherit colour and font settings from the parent window
    void InheritAttributes();
};

// ----------------------------------------------------------------------------
// include platform-dependent wxControl declarations
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
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

#endif
    // _WX_CONTROL_H_BASE_
