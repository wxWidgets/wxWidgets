/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valgen.cpp
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by:
// Created:     Jan 22 1999
// Copyright:   (c) 1999 Kevin Smith
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_VALIDATORS

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/valgen.h"

wxIMPLEMENT_CLASS(wxGenericValidator, wxValidator);

wxGenericValidator::wxGenericValidator(const wxGenericValidator& val)
    : wxValidator()
{
    Copy(val);
}

bool wxGenericValidator::Copy(const wxGenericValidator& val)
{
    wxValidator::Copy(val);

    m_value = val.m_value;
    m_type = val.m_type;

    return true;
}

// Called to transfer data to the window
bool wxGenericValidator::TransferToWindow()
{
    return m_validatorWindow && m_value &&
    	   m_validatorWindow->DoTransferDataToWindow(m_value, m_type);
}

// Called to transfer data from the window
bool wxGenericValidator::TransferFromWindow()
{
    return m_validatorWindow && m_value &&
    	   m_validatorWindow->DoTransferDataFromWindow(m_value, m_type);
}


#endif // wxUSE_VALIDATORS
