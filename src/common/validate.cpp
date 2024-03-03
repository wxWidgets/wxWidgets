/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/validate.cpp
// Purpose:     wxValidator
// Author:      Julian Smart
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

wxIMPLEMENT_DYNAMIC_CLASS(wxValidator, wxEvtHandler);

const wxValidator wxDefaultValidator;

// VZ: personally, I think true would be more appropriate - these bells are
//     _annoying_
bool wxValidator::ms_isSilent = false;

wxValidator::wxValidator()
{
  m_validatorWindow = nullptr;
}

wxValidator::~wxValidator()
{
}

#endif // wxUSE_VALIDATORS
