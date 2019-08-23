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

#else

class WXDLLIMPEXP_CORE wxValidator { };

#endif // wxUSE_VALIDATORS

const wxValidator wxDefaultValidator;
