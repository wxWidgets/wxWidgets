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

const wxValidator wxDefaultValidator;

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

void wxValidator::DoProcessEvent(wxValidationStatusEvent& event)
{
    // First, give a chance to the m_validatorWindow to process the event
    // on its own. If no handler was found to process the event, or it was
    // just skipped, pass the event to the validator itself to process it.

    if ( !m_validatorWindow->ProcessWindowEvent(event) )
    {
        ProcessEventLocally(event);
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
    // N.B. According to "How Events are Processed" documentation, step (2) says:
    //
    //   2. If the object is a wxWindow and has an associated validator,
    //      wxValidator gets a chance to process the event.
    //
    // So, by setting the event object to the window validator and not to the
    //     window itself, we make sure that this step is skipped, and any user
    //     defined handler will get a chance to handle the event before the
    //     validator, which will take over the event, and the processing would
    //     stop there. see wxWindowBase::TryBefore().

    SetEventObject(win->GetValidator());
}

#endif
  // wxUSE_VALIDATORS
