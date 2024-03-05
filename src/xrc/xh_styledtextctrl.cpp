/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_styledtextctrl.cpp
// Purpose:     XRC resource for wxStyledTextCtrl
// Author:      Alexander Koshelev
// Created:     2021-09-22
// Copyright:   (c) 2021 TT-Solutions SARL
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_STC

#include "wx/xrc/xh_styledtextctrl.h"
#include "wx/stc/stc.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxStyledTextCtrlXmlHandler, wxXmlResourceHandler);

wxStyledTextCtrlXmlHandler::wxStyledTextCtrlXmlHandler()
{
    XRC_ADD_STYLE(wxSTC_WRAP_NONE);
    XRC_ADD_STYLE(wxSTC_WRAP_WORD);
    XRC_ADD_STYLE(wxSTC_WRAP_CHAR);
    XRC_ADD_STYLE(wxSTC_WRAP_WHITESPACE);

    AddWindowStyles();
}

wxObject *wxStyledTextCtrlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxStyledTextCtrl)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(),
                    GetSize(),
                    GetStyle(),
                    GetName());

    SetupWindow(control);

    if (HasParam("wrapmode"))
    {
        control->SetWrapMode(GetStyle("wrapmode"));
    }

    return control;
}

bool wxStyledTextCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, "wxStyledTextCtrl");
}

#endif // wxUSE_XRC && wxUSE_STC
