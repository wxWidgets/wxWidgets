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
    #include "wx/module.h"
#endif

const wxValidator wxDefaultValidator;

wxIMPLEMENT_DYNAMIC_CLASS(wxValidator, wxEvtHandler);

// VZ: personally, I think true would be more appropriate - these bells are
//     _annoying_
bool wxValidator::ms_isSilent = false;

// created on demand and destroyed by wxValidatorPopupModule
static wxValidatorPopup* gs_valPopup = NULL;

// ----------------------------------------------------------------------------
// wxValidatorPopupModule is used to cleanup gs_valPopup
// ----------------------------------------------------------------------------

class wxValidatorPopupModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE
    {
        wxDELETE(gs_valPopup);
    }

    wxDECLARE_DYNAMIC_CLASS(wxValidatorPopupModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxValidatorPopupModule, wxModule);

// ----------------------------------------------------------------------------

void wxValidatorPopup::ShowFor(wxWindow* win)
{
    DoShowFor(win);
    // wipe the error message
    m_errmsg.clear();
}

wxValidator::wxValidator()
{
  	m_validatorWindow = NULL;
}

wxValidator::~wxValidator()
{
}

void wxValidator::Popup()
{
	if ( gs_valPopup && m_validatorWindow )
		gs_valPopup->ShowFor(m_validatorWindow);
}

/*static*/
bool wxValidator::SetErrorMsg(const wxString& errmsg)
{
	if ( gs_valPopup )
	{
		gs_valPopup->SetErrorMsg(errmsg);
		return true;
	}

	return false;
}

/*static*/
bool wxValidator::SetCustomErrorReporting(wxValidatorPopup* popup)
{
	wxDELETE(gs_valPopup);

	return (gs_valPopup = popup);
}

/*static*/
bool wxValidator::HasCustomErrorReporting(){ return (gs_valPopup = NULL); }

#endif
  // wxUSE_VALIDATORS
