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
    #include "wx/msgdlg.h"
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

void wxValidator::DoProcessEvent(wxValidationErrorEvent& event)
{
    // First, give a chance to the m_validatorWindow to process the event
    // on its own. If no handler was found to process the event, or it was
    // just skipped, process it here by just popping up the error message.

    if ( !m_validatorWindow->ProcessWindowEvent(event) )
    {
        const wxString errormsg = event.GetErrorMessage();

        // N.B. event may carry empty error messages to signal transitions
        //      from Invalid to Valid state (i.e. non-error event).
        if ( errormsg.empty() )
            return;

        m_validatorWindow->SetFocus();
        wxMessageBox(errormsg, _("Validation conflict"),
                     wxOK | wxICON_EXCLAMATION, NULL);
    }
}

// ----------------------------------------------------------------------------
// wxValidationErrorEvent
// ----------------------------------------------------------------------------
wxIMPLEMENT_DYNAMIC_CLASS(wxValidationErrorEvent, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_VALIDATE, wxValidationErrorEvent);

wxValidationErrorEvent::wxValidationErrorEvent(
    wxValidator *val, wxEventType type, wxWindow *win)
    : wxCommandEvent(type, win->GetId())
{
    SetEventObject(val);
}

#endif
  // wxUSE_VALIDATORS
