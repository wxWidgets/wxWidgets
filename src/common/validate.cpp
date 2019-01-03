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

#include <wx/richtooltip.h>

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

void wxValidator::DoProcessEvent(wxValidationEvent& event)
{
    // First, give a chance to the m_validatorWindow to process the event
    // by itself. If no handler was found to process the event, or it was
    // just skipped, process it here by popping up the error message using
    // a wxRichToolTip or wxMessageBox (depending on the configuration)

    if ( !m_validatorWindow->GetEventHandler()->ProcessEvent(event) )
    {
    #if wxUSE_RICHTOOLTIP
        wxRichToolTip tip(_("Validation conflict"), event.GetErrorMessage());
        tip.SetIcon(wxICON_ERROR);
        tip.ShowFor(m_validatorWindow);
    #else
        m_validatorWindow->SetFocus();
        wxMessageBox(event.GetErrorMessage(), _("Validation conflict"),
                     wxOK | wxICON_EXCLAMATION, NULL);
    #endif // wxUSE_RICHTOOLTIP
    }
}

// ----------------------------------------------------------------------------
// wxValidationEvent
// ----------------------------------------------------------------------------
wxIMPLEMENT_DYNAMIC_CLASS(wxValidationEvent, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_VALIDATE, wxValidationEvent);

wxValidationEvent::wxValidationEvent(
    wxValidator *validator, wxEventType type, wxWindow *win)
    : wxCommandEvent(type, win->GetId())
{
    SetEventObject(validator);
}

#endif
  // wxUSE_VALIDATORS
