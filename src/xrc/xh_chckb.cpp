/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_chckb.cpp
// Purpose:     XRC resource for wxCheckBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_CHECKBOX

#include "wx/xrc/xh_chckb.h"

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxCheckBoxXmlHandler, wxXmlResourceHandler);

wxCheckBoxXmlHandler::wxCheckBoxXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxCHK_2STATE);
    XRC_ADD_STYLE(wxCHK_3STATE);
    XRC_ADD_STYLE(wxCHK_ALLOW_3RD_STATE_FOR_USER);
    XRC_ADD_STYLE(wxALIGN_RIGHT);
    AddWindowStyles();
}

wxObject *wxCheckBoxXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxCheckBox)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("label")),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    GetName());

    switch (GetLong("checked", wxCHK_UNCHECKED))
    {
        case wxCHK_UNCHECKED:
            // Nothing to do here, we could call SetValue() but the default state
            // is unchecked anyhow.
            break;

        case wxCHK_CHECKED:
            control->SetValue(true);
            break;

        case wxCHK_UNDETERMINED:
            // While just trying to set it would generate an assert if wxCHK_3STATE
            // is not set, prefer to give an error here as we have more information
            // about the problem.
            if ( !control->HasFlag(wxCHK_3STATE) )
            {
                ReportParamError("checked",
                    "A checkbox must have wxCHK_3STATE style to use wxCHK_UNDETERMINED");
                break;
            }

            control->Set3StateValue(wxCHK_UNDETERMINED);
            break;

        default:
            ReportParamError("checked",
                             wxString::Format("Unknown checkbox state: \"%s\"",
                                              GetParamValue("checked")));
            break;
    }

    SetupWindow(control);

    return control;
}

bool wxCheckBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxCheckBox"));
}

#endif // wxUSE_XRC && wxUSE_CHECKBOX
