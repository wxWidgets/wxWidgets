/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_buttonbox.cpp
// Purpose:     XRC resource handler for wxButtonBox
// Author:      Wlodzimierz ABX Skiba, LLM supported
// Created:     2026-11-14
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_XRC && wxUSE_BUTTONBOX

#include "wx/xrc/xh_buttonbox.h"

#ifndef WX_PRECOMP
    #include "wx/artprov.h"
#endif

#include "wx/buttonbox.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxButtonBoxXmlHandler, wxXmlResourceHandler);

wxButtonBoxXmlHandler::wxButtonBoxXmlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxBB_SELECT_ONE);
    XRC_ADD_STYLE(wxBB_SELECT_ANY);
    XRC_ADD_STYLE(wxBB_MOMENTARY);

    AddWindowStyles();
}

wxObject *wxButtonBoxXmlHandler::DoCreateResource()
{
    if ( m_class == wxS("segment") )
    {
        // <object class="segment"> inside <object class="wxButtonBox">
        if ( !m_buttonBox )
        {
            ReportError("segment only allowed inside a wxButtonBox");
            return nullptr;
        }

        const size_t n = m_buttonBox->GetCount();

        m_buttonBox->Append(GetText(wxS("label")),
                            GetBitmapBundle(wxS("bitmap"), wxART_BUTTON));

        const long width = GetLong(wxS("width"), wxDefaultCoord);
        if ( width != wxDefaultCoord )
            m_buttonBox->SetItemWidth(n, static_cast<int>(width));

        const wxString tip = GetText(wxS("tooltip"));
        if ( !tip.empty() )
            m_buttonBox->SetItemToolTip(n, tip);

        if ( !GetBool(wxS("enabled"), true) )
            m_buttonBox->EnableItem(n, false);

        if ( GetBool(wxS("selected")) )
            m_buttonBox->SetSelected(n, true);

        return m_buttonBox; // must return non-null
    }
    else // <object class="wxButtonBox">
    {
        XRC_MAKE_INSTANCE(control, wxButtonBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        GetStyle(wxS("style"), wxBB_DEFAULT_STYLE),
                        GetName());

        SetupWindow(control);

        m_isInside = true;
        m_buttonBox = control;

        CreateChildren(control, true /* only this handler */);

        m_isInside = false;
        m_buttonBox = nullptr;

        return control;
    }
}

bool wxButtonBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (!m_isInside && IsOfClass(node, wxS("wxButtonBox"))) ||
           (m_isInside && IsOfClass(node, wxS("segment")));
}

#endif // wxUSE_XRC && wxUSE_BUTTONBOX
