///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/clrpickercmn.cpp
// Purpose:     wxColourPickerCtrl class implementation
// Author:      Francesco Montorsi (readapted code written by Vadim Zeitlin)
// Created:     15/04/2006
// Copyright:   (c) Vadim Zeitlin, Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_COLOURPICKERCTRL

#include "wx/clrpicker.h"
#include "wx/weakref.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif

const char wxColourPickerCtrlNameStr[] = "colourpicker";
const char wxColourPickerWidgetNameStr[] = "colourpickerwidget";

// ============================================================================
// implementation
// ============================================================================

wxDEFINE_EVENT(wxEVT_COLOURPICKER_CHANGED, wxColourPickerEvent);
wxDEFINE_EVENT(wxEVT_COLOURPICKER_CURRENT_CHANGED, wxColourPickerEvent);
wxDEFINE_EVENT(wxEVT_COLOURPICKER_DIALOG_CANCELLED, wxColourPickerEvent);

wxIMPLEMENT_DYNAMIC_CLASS(wxColourPickerCtrl, wxPickerBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxColourPickerEvent, wxEvent);

// ----------------------------------------------------------------------------
// wxColourPickerCtrl
// ----------------------------------------------------------------------------

#define M_PICKER     ((wxColourPickerWidget*)m_picker)

bool wxColourPickerCtrl::Create( wxWindow *parent, wxWindowID id,
                        const wxColour &col,
                        const wxPoint &pos, const wxSize &size,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    if (!wxPickerBase::CreateBase(parent, id, col.GetAsString(), pos, size,
                                  style, validator, name))
        return false;

    // we are not interested to the ID of our picker as we connect
    // to its "changed" event dynamically...
    m_picker = new wxColourPickerWidget(this, wxID_ANY, col,
                                        wxDefaultPosition, wxDefaultSize,
                                        GetPickerStyle(style));

    // complete sizer creation
    wxPickerBase::PostCreation();

    m_picker->Bind(wxEVT_COLOURPICKER_CHANGED,
            &wxColourPickerCtrl::OnColourChange, this);
    m_picker->Bind(wxEVT_COLOURPICKER_CURRENT_CHANGED,
            &wxColourPickerCtrl::OnColourChange, this);
    m_picker->Bind(wxEVT_COLOURPICKER_DIALOG_CANCELLED,
            &wxColourPickerCtrl::OnColourChange, this);

    return true;
}

void wxColourPickerCtrl::SetColour(const wxColour &col)
{
    const wxWeakRef<wxColourPickerCtrl> weakThis(this);
    M_PICKER->SetColour(col);
    if ( wxColourPickerCtrl * const live = weakThis.get() )
        live->UpdateTextCtrlFromPicker();
}

bool wxColourPickerCtrl::SetColour(const wxString &text)
{
    wxColour col(text);     // smart wxString->wxColour conversion
    if ( !col.IsOk() )
        return false;

    const wxWeakRef<wxColourPickerCtrl> weakThis(this);
    M_PICKER->SetColour(col);
    if ( wxColourPickerCtrl * const live = weakThis.get() )
        live->UpdateTextCtrlFromPicker();

    return true;
}

void wxColourPickerCtrl::UpdatePickerFromTextCtrl()
{
    wxASSERT(m_text);

    // wxString -> wxColour conversion
    wxColour col(m_text->GetValue());
    if ( !col.IsOk() )
        return;     // invalid user input

    if (M_PICKER->GetColour() != col)
    {
        const wxWeakRef<wxColourPickerCtrl> weakThis(this);
        M_PICKER->SetColour(col);
        wxColourPickerCtrl * const live = weakThis.get();
        if ( !live )
            return;

        // fire an event
        wxColourPickerEvent event(live, live->GetId(), col);

        // Application handlers may synchronously destroy the picker, so this
        // notification must remain the final operation in this method.
        live->GetEventHandler()->ProcessEvent(event);
    }
}

void wxColourPickerCtrl::UpdateTextCtrlFromPicker()
{
    if (!m_text)
        return;     // no textctrl to update

    // Take care to use ChangeValue() here and not SetValue() to avoid
    // infinite recursion.
    m_text->ChangeValue(M_PICKER->GetColour().GetAsString());
}



// ----------------------------------------------------------------------------
// wxColourPickerCtrl - event handlers
// ----------------------------------------------------------------------------

void wxColourPickerCtrl::OnColourChange(wxColourPickerEvent &ev)
{
    const wxWeakRef<wxColourPickerCtrl> weakThis(this);
    UpdateTextCtrlFromPicker();
    wxColourPickerCtrl * const live = weakThis.get();
    if ( !live )
    {
        // Do not propagate the implementation event after the public
        // composite disappeared: its child identity is never application API.
        return;
    }

    // Some native/generic picker widgets already construct the event with the
    // public composite as its source. Preserve this established path without
    // manufacturing a duplicate notification.
    if ( ev.GetEventObject() == live && ev.GetId() == live->GetId() )
    {
        ev.Skip();
        return;
    }

    // The WinUI pair is a real child control with wxID_ANY. Never expose this
    // implementation detail to applications: all picker event variants use
    // the public control object and ID.
    wxColourPickerEvent event(
        live, live->GetId(), ev.GetColour(), ev.GetEventType());

    // User handlers may synchronously destroy the picker.
    live->GetEventHandler()->ProcessEvent(event);
}

#endif  // wxUSE_COLOURPICKERCTRL
