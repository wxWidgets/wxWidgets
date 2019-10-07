/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_infobar.cpp
// Purpose:     XML resource handler for wxInfoBar
// Author:      Ilya Sinitsyn
// Created:     2019-09-25
// Copyright:   (c) 2019 TT-Solutions SARL
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_INFOBAR

#include "wx/xrc/xh_infobar.h"

#include "wx/infobar.h"
#include "wx/xml/xml.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxInfoBarXmlHandler, wxXmlResourceHandler);

#define XRC_ADD_SHOW_EFFECT(style) m_effectNames[style] = #style;

wxInfoBarXmlHandler::wxInfoBarXmlHandler()
    : wxXmlResourceHandler(), m_insideBar(false)
{
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_NONE);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_ROLL_TO_LEFT);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_ROLL_TO_RIGHT);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_ROLL_TO_TOP);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_ROLL_TO_BOTTOM);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_SLIDE_TO_LEFT);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_SLIDE_TO_RIGHT);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_SLIDE_TO_TOP);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_SLIDE_TO_BOTTOM);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_BLEND);
    XRC_ADD_SHOW_EFFECT(wxSHOW_EFFECT_EXPAND);
}

wxObject *wxInfoBarXmlHandler::DoCreateResource()
{
    if ( m_class == "wxInfoBar" )
    {
        XRC_MAKE_INSTANCE(control, wxInfoBar)

        control->Create(m_parentAsWindow, GetID());

        SetupWindow(control);

        wxShowEffect showEffect = GetShowEffect("showeffect");
        wxShowEffect hideEffect = GetShowEffect("hideeffect");

        if ( showEffect != wxSHOW_EFFECT_NONE || hideEffect != wxSHOW_EFFECT_NONE )
            control->SetShowHideEffects(showEffect, hideEffect);

        if ( HasParam("effectduration") )
            control->SetEffectDuration(GetLong("effectduration"));

        m_insideBar = true;
        CreateChildrenPrivately(control);
        m_insideBar = false;

        return control;
    }
    else
    {
        // inside the element now,
        // handle buttons

        wxInfoBar * const infoBar = wxDynamicCast(m_parentAsWindow, wxInfoBar);
        wxCHECK_MSG(infoBar, NULL, "must have wxInfoBar parent");

        infoBar->AddButton(GetID(), GetText("label"));

        return NULL;
    }
}

bool wxInfoBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, "wxInfoBar") ||
           (m_insideBar && IsOfClass(node, "button")));
}

wxShowEffect wxInfoBarXmlHandler::GetShowEffect(wxString const& param)
{
    if ( !HasParam(param) )
        return wxSHOW_EFFECT_NONE;

    wxString const& value = GetParamValue(param);

    for ( int i = 0; i < wxSHOW_EFFECT_MAX; ++i )
    {
        if ( value == m_effectNames[i] )
            return static_cast<wxShowEffect>(i);
    }

    ReportParamError
    (
        param,
        wxString::Format("unknown show effect \"%s\"", value)
    );

    return wxSHOW_EFFECT_NONE;
}

#endif // wxUSE_XRC && wxUSE_INFOBAR
