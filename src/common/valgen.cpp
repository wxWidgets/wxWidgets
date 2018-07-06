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

#if wxUSE_TOGGLEBTN
#include "wx/tglbtn.h"
#endif // wxUSE_TOGGLEBTN

#if wxUSE_COLLPANE
#include "wx/collpane.h"
#endif // wxUSE_COLLPANE


wxIMPLEMENT_CLASS(wxGenericValidatorBase, wxValidator);

wxGenericValidatorBase::wxGenericValidatorBase(const wxGenericValidatorBase& val)
    : wxValidator()
{
    Copy(val);
}

bool wxGenericValidatorBase::Copy(const wxGenericValidatorBase& val)
{
    wxValidator::Copy(val);

    // deep copy
    m_data.reset(val.m_data->Clone());

    return true;
}

// Called to transfer data to the window
bool wxGenericValidatorBase::TransferToWindow()
{
    return m_validatorWindow && m_validatorWindow->DoTransferDataToWindow(m_data);
}

// Called to transfer data from the window
bool wxGenericValidatorBase::TransferFromWindow()
{
    return m_validatorWindow && m_validatorWindow->DoTransferDataFromWindow(m_data);
}

// TODO:
//   move these implementations to their respective files.

#if wxUSE_TOGGLEBTN

bool wxToggleButtonBase::DoTransferDataToWindow(const wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<bool>(), "Expected type: 'bool'");
    SetValue(ptr->GetValue<bool>());
    return true; 
}

bool wxToggleButtonBase::DoTransferDataFromWindow(wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<bool>(), "Expected type: 'bool'");
    ptr->SetValue<bool>(GetValue());
    return true; 
}

#endif // wxUSE_TOGGLEBTN

#if wxUSE_COLLPANE

bool wxCollapsiblePaneBase::DoTransferDataToWindow(const wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<bool>(), "Expected type: 'bool'");
    Collapse(ptr->GetValue<bool>());
    return true; 
}

bool wxCollapsiblePaneBase::DoTransferDataFromWindow(wxValidator::DataPtr& ptr)
{
    wxASSERT_MSG(ptr->IsOfType<bool>(), "Expected type: 'bool'");
    ptr->SetValue<bool>(IsCollapsed());
    return true; 
}

#endif // wxUSE_COLLPANE

#endif // wxUSE_VALIDATORS
