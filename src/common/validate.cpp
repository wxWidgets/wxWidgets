/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/validate.cpp
// Purpose:     wxValidator
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_VALIDATORS

#include "wx/validate.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/scopeguard.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxValidator, wxEvtHandler);

const wxValidator wxDefaultValidator;

// VZ: personally, I think true would be more appropriate - these bells are
//     _annoying_
bool wxValidator::ms_isSilent = false;

int  wxValidator::ms_validationMethod;

wxValidator::wxValidator()
{
    m_validatorWindow  = NULL;
    m_validationStatus = Validation_Unknown;
}

wxValidator::~wxValidator()
{
}

bool wxValidator::DoValidate(wxWindow *parent, bool canPopup)
{
    // We do nothing if the window is already validated
    // and in a valid state.
    if ( IsOk() )
        return true;

    if ( !canPopup )
        m_validationStatus |= Validation_NoPopup;

    if ( Validate(parent) )
    {
        SendOkEvent();
        return true;
    }

    // Send an error event if Validate() did not send one already.
    if ( m_validationStatus != Validation_Error )
        SendErrorEvent(_("Validation failed."));

    return false;
}

bool wxValidator::ProcessEvent(wxEvent& event)
{
    if ( m_validationStatus & Validation_Skip )
    {
        // The validator window is given a chance to process the event first
        // (i.e. wxEVT_VALIDATE_XXX events), so just return false.
        // See wxWindowBase::TryBefore().
        return false;
    }

    const wxEventType eventType = event.GetEventType();
    if ( eventType == wxEVT_VALIDATE_OK ||
         eventType == wxEVT_VALIDATE_ERROR )
    {
        // We want any custom handler (if any) to take precedence over
        // the validator itself when handling wxEVT_VALIDATE_XXX events.

        // Validation status should be restored on block exit.
        const int status = m_validationStatus;
        wxON_BLOCK_EXIT_SET(m_validationStatus, status);

        m_validationStatus |= Validation_Skip;

        if ( m_validatorWindow->ProcessWindowEvent(event) )
        {
            return true;
        }
    }

    return ProcessEventLocally(event);
}

void wxValidator::SendValidationEvent(wxEventType type, const wxString& errormsg)
{
    if ( type == wxEVT_VALIDATE_OK && IsOk() )
    {
        // We don't send the 'Ok' event needlessly. i.e:
        // the event need not be sent unless there was a
        // previous validation error.
        return;
    }

    const bool canPopup = (m_validationStatus & Validation_NoPopup) == 0;

    m_validationStatus = type == wxEVT_VALIDATE_ERROR ? Validation_Error
                                                      : Validation_Ok;

    wxValidationStatusEvent event(type, m_validatorWindow);
    event.SetErrorMessage(errormsg);
    event.SetCanPopup(canPopup);

    m_validatorWindow->ProcessWindowEvent(event);
}

// ----------------------------------------------------------------------------
// wxValidationStatusEvent
// ----------------------------------------------------------------------------
wxIMPLEMENT_DYNAMIC_CLASS(wxValidationStatusEvent, wxCommandEvent);

wxDEFINE_EVENT(wxEVT_VALIDATE_OK, wxValidationStatusEvent);
wxDEFINE_EVENT(wxEVT_VALIDATE_ERROR, wxValidationStatusEvent);

wxValidationStatusEvent::wxValidationStatusEvent(wxEventType type, wxWindow *win)
    : wxCommandEvent(type, win->GetId())
{
    SetEventObject(win);
}

wxWindow* wxValidationStatusEvent::GetWindow() const
{
    return static_cast<wxWindow*>(GetEventObject());
}

#endif // wxUSE_VALIDATORS
