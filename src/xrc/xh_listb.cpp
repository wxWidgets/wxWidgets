/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_listb.cpp
// Purpose:     XRC resource for wxListBox
// Author:      Bob Mitchell & Vaclav Slavik
// Created:     2000/07/29
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_LISTBOX

#include "wx/xrc/xh_listb.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/listbox.h"
#endif

#include "wx/xml/xml.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxListBoxXmlHandler, wxXmlResourceHandler);

wxListBoxXmlHandler::wxListBoxXmlHandler()
                   : wxXmlResourceHandler(),
                     m_insideBox(false)
{
    XRC_ADD_STYLE(wxLB_SINGLE);
    XRC_ADD_STYLE(wxLB_MULTIPLE);
    XRC_ADD_STYLE(wxLB_EXTENDED);
    XRC_ADD_STYLE(wxLB_HSCROLL);
    XRC_ADD_STYLE(wxLB_ALWAYS_SB);
    XRC_ADD_STYLE(wxLB_NEEDED_SB);
    XRC_ADD_STYLE(wxLB_SORT);
    AddWindowStyles();
}

wxObject *wxListBoxXmlHandler::DoCreateResource()
{
    if ( m_class == wxT("wxListBox"))
    {
        // find the selection
        long selection = GetLong(wxT("selection"), -1);

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));
        m_insideBox = false;

        XRC_MAKE_INSTANCE(control, wxListBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        strList,
                        GetStyle(),
                        wxDefaultValidator,
                        GetName());

        if (selection != -1)
            control->SetSelection(selection);

        SetupWindow(control);
        strList.Clear();    // dump the strings

        return control;
    }
    else
    {
        // on the inside now.
        // handle <item>Label</item>

        // add to the list
        strList.Add(GetNodeText(m_node, wxXRC_TEXT_NO_ESCAPE));

        return NULL;
    }
}

bool wxListBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxListBox")) ||
           (m_insideBox && node->GetName() == wxT("item")));
}

#endif // wxUSE_XRC && wxUSE_LISTBOX
