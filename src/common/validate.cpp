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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_VALIDATORS

#include "wx/validate.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxValidator, wxEvtHandler);

// VZ: personally, I think true would be more appropriate - these bells are
//     _annoying_
bool wxValidator::ms_isSilent = false;

wxValidator::wxValidator()
{
  m_validatorWindow = NULL;
}

wxValidator::~wxValidator()
{
}

bool wxValidator::ProcessEvent(wxEvent& event)
{
    // We want any custom handler (if any) to take precedence over
    // the validator itself when handling wxEVT_VALIDATE_XXX events.

    const wxEventType eventType = event.GetEventType();
    if ( eventType == wxEVT_VALIDATE_OK ||
         eventType == wxEVT_VALIDATE_ERROR )
    {
        // N.B. According to "How Events are Processed" documentation,
        //      step (2) says:
        //
        //   2. If the object is a wxWindow and has an associated validator,
        //      wxValidator gets a chance to process the event.
        //
        // It is clear that this will prevent any custom handling of
        // wxEVT_VALIDATE_XXX events, and (temporarily) removing the
        // validator solves the problem.

        wxValidatorDisabler noValidation(m_validatorWindow);

        if ( m_validatorWindow->ProcessWindowEvent(event) )
        {
            return true;
        }
    }

    return ProcessEventLocally(event);
}

void wxValidator::SendEvent(wxEventType type, const wxString& errormsg)
{
    if ( !m_validatorWindow )
        return;

    wxValidationStatusEvent event(type, m_validatorWindow);
    event.SetErrorMessage(errormsg);

    m_validatorWindow->ProcessWindowEvent(event);
}

// ----------------------------------------------------------------------------
// wxValidatorDisabler
// ----------------------------------------------------------------------------
wxValidatorDisabler::wxValidatorDisabler(wxWindow* win)
{
    m_validator = NULL;

    if ( win )
    {
        m_validator = win->SetValidator(NULL);
    }
}

wxValidatorDisabler::~wxValidatorDisabler()
{
    if ( m_validator )
    {
        wxWindow* const win = m_validator->GetWindow();

        if ( win )
            win->SetValidator(m_validator);
    }
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

#else

class WXDLLIMPEXP_CORE wxValidator { };

#endif // wxUSE_VALIDATORS

const wxValidator wxDefaultValidator;
