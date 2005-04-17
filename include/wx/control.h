/////////////////////////////////////////////////////////////////////////////
// Name:        wx/control.h
// Purpose:     wxControl common interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_BASE_
#define _WX_CONTROL_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "controlbase.h"
#endif

#include "wx/defs.h"

#if wxUSE_CONTROLS

#include "wx/window.h"      // base class

extern WXDLLEXPORT_DATA(const wxChar*) wxControlNameStr;

// ----------------------------------------------------------------------------
// wxControl is the base class for all controls
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlBase : public wxWindow
{
public:
    wxControlBase() { }

    virtual ~wxControlBase();

    // Create() function adds the validator parameter
    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxControlNameStr);

    // get the control alignment (left/right/centre, top/bottom/centre)
    int GetAlignment() const { return m_windowStyle & wxALIGN_MASK; }


    // controls by default inherit the colours of their parents, if a
    // particular control class doesn't want to do it, it can override
    // ShouldInheritColours() to return false
    virtual bool ShouldInheritColours() const { return true; }


    // WARNING: this doesn't work for all controls nor all platforms!
    //
    // simulates the event of given type (i.e. wxButton::Command() is just as
    // if the button was clicked)
    virtual void Command(wxCommandEvent &event);

    virtual void SetLabel( const wxString &label );
    virtual bool SetFont(const wxFont& font);

    // Reserved for future use
    virtual void ReservedControlFunc1() {}
    virtual void ReservedControlFunc2() {}
    virtual void ReservedControlFunc3() {}
    virtual void ReservedControlFunc4() {}
    virtual void ReservedControlFunc5() {}
    virtual void ReservedControlFunc6() {}
    virtual void ReservedControlFunc7() {}
    virtual void ReservedControlFunc8() {}
    virtual void ReservedControlFunc9() {}

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

    // initialize the common fields of wxCommandEvent
    void InitCommandEvent(wxCommandEvent& event) const;

    // set the initial window size if none is given (i.e. at least one of the
    // components of the size passed to ctor/Create() is -1)
    //
    // normally just calls SetBestSize() but can be overridden not to do it for
    // the controls which have to do some additional initialization (e.g. add
    // strings to list box) before their best size can be accurately calculated
    virtual void SetInitialBestSize(const wxSize& size)
    {
        SetBestSize(size);
    }

    DECLARE_NO_COPY_CLASS(wxControlBase)
};

// ----------------------------------------------------------------------------
// include platform-dependent wxControl declarations
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/control.h"
#elif defined(__WXPALMOS__)
    #include "wx/palmos/control.h"
#elif defined(__WXMSW__)
    #include "wx/msw/control.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/control.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/control.h"
#elif defined(__WXMAC__)
    #include "wx/mac/control.h"
#elif defined(__WXCOCOA__)
    #include "wx/cocoa/control.h"
#elif defined(__WXPM__)
    #include "wx/os2/control.h"
#endif

#endif // wxUSE_CONTROLS

#endif
    // _WX_CONTROL_H_BASE_
