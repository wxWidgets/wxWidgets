/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/valgen.cpp
// Purpose:     wxGenericValidator class
// Author:      Kevin Smith
// Modified by: Ali Kettab (2018-04-07)
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


wxIMPLEMENT_CLASS(wxGenericValidatorBase, wxValidator);

wxGenericValidatorBase::wxGenericValidatorBase(const wxGenericValidatorBase& val)
    : wxValidator()
{
    Copy(val);
}

bool wxGenericValidatorBase::Copy(const wxGenericValidatorBase& val)
{
    wxValidator::Copy(val);

    m_data = val.m_data;

    return true;
}


#endif // wxUSE_VALIDATORS
