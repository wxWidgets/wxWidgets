/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/control.h
// Purpose:     wxControl class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "control.h"
#endif

#include "wx/dynarray.h"

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
public:
    wxControl() { }

    wxControl(wxWindow *parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxControlNameStr)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxControlNameStr);

    virtual ~wxControl();

    // Simulates an event
    virtual void Command(wxCommandEvent& event) { ProcessCommand(event); }

    virtual bool Enable( bool enable = true );
    virtual bool IsEnabled() const;

    virtual bool Show( bool show = true );
    virtual bool IsShown() const;

    virtual void SetLabel(const wxString& label);
    virtual wxString GetLabel();

    // implementation from now on
    // --------------------------

    virtual wxVisualAttributes GetDefaultAttributes() const
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // Calls the callback and appropriate event handlers
    bool ProcessCommand(wxCommandEvent& event);

    const wxArrayLong& GetSubcontrols() const { return m_subControls; }

    void OnEraseBackground(wxEraseEvent& event);

    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const;

    // return default best size (doesn't really make any sense, override this)
    virtual wxSize DoGetBestSize() const;

    // create the control of the given ControlStyleType: this is typically called
    // from Create() method of the derived class passing its label, pos and
    // size parameter (style parameter is not needed because m_windowStyle is
    // supposed to had been already set and so is used instead when this
    // function is called)
    bool PalmCreateControl(ControlStyleType style,
                           wxWindow *parent,
                           wxWindowID id,
                           const wxString& label,
                           const wxPoint& pos,
                           const wxSize& size);

    // this is a helper for the derived class GetClassDefaultAttributes()
    // implementation: it returns the right colours for the classes which
    // contain something else (e.g. wxListBox, wxTextCtrl, ...) instead of
    // being simple controls (such as wxButton, wxCheckBox, ...)
    static wxVisualAttributes
        GetCompositeControlsDefaultAttributes(wxWindowVariant variant);


    // for controls like radiobuttons which are really composite this array
    // holds the ids (not HWNDs!) of the sub controls
    wxArrayLong m_subControls;

    ControlType *m_control;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxControl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_CONTROL_H_
