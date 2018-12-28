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

void wxValidator::DoProcessEvent(wxValidationEvent& event)
{
    if ( !m_validatorWindow->GetEventHandler()->ProcessEvent(event) )
    {
        m_validatorWindow->SetFocus();
        wxMessageBox(event.GetErrorMessage(), _("Validation conflict"),
                     wxOK | wxICON_EXCLAMATION, NULL);
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
