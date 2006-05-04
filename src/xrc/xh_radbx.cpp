/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_radbx.cpp
// Purpose:     XRC resource for wxRadioBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_RADIOBOX

#include "wx/xrc/xh_radbx.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/radiobox.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioBoxXmlHandler, wxXmlResourceHandler)

wxRadioBoxXmlHandler::wxRadioBoxXmlHandler()
: wxXmlResourceHandler(), m_insideBox(false)
{
    XRC_ADD_STYLE(wxRA_SPECIFY_COLS);
    XRC_ADD_STYLE(wxRA_HORIZONTAL);
    XRC_ADD_STYLE(wxRA_SPECIFY_ROWS);
    XRC_ADD_STYLE(wxRA_VERTICAL);
    AddWindowStyles();
}

wxObject *wxRadioBoxXmlHandler::DoCreateResource()
{
    if ( m_class == wxT("wxRadioBox"))
    {
        // find the selection
        long selection = GetLong( wxT("selection"), -1 );

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately( NULL, GetParamNode(wxT("content")));

        wxString *strings;
        if ( !labels.empty() )
        {
            strings = new wxString[labels.size()];
            const unsigned count = labels.size();
            for( unsigned i = 0; i < count; i++ )
                strings[i] = labels[i];
        }
        else
        {
            strings = NULL;
        }

        XRC_MAKE_INSTANCE(control, wxRadioBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetText(wxT("label")),
                        GetPosition(), GetSize(),
                        labels.size(),
                        strings,
                        GetLong(wxT("dimension"), 1),
                        GetStyle(),
                        wxDefaultValidator,
                        GetName());

        delete[] strings;

        if (selection != -1)
            control->SetSelection(selection);

        SetupWindow(control);

        const unsigned count = labels.size();
        for( unsigned i = 0; i < count; i++ )
        {
            if ( !tooltips[i].empty() )
                control->SetItemToolTip(i, tooltips[i]);
        }

        labels.clear();    // dump the strings
        tooltips.clear();    // dump the tooltips

        return control;
    }
    else // inside the radiobox element
    {
        // we handle <item tooltip="...">Label</item> constructs here

        wxString str = GetNodeContent(m_node);
        wxString tooltip;
        m_node->GetPropVal(wxT("tooltip"), &tooltip);

        if (m_resource->GetFlags() & wxXRC_USE_LOCALE)
        {
            str = wxGetTranslation(str);
            if ( !tooltip.empty() )
                tooltip = wxGetTranslation(tooltip);
        }

        labels.push_back(str);
        tooltips.push_back(tooltip);

        return NULL;
    }

}

bool wxRadioBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxRadioBox")) ||
           (m_insideBox && node->GetName() == wxT("item")));
}

#endif // wxUSE_XRC && wxUSE_RADIOBOX
