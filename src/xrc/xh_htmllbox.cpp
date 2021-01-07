/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_simplehtmllbox.cpp
// Purpose:     XML resource handler for wxSimpleHtmlListBox
// Author:      Francesco Montorsi
// Created:     2006/10/21
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_HTML

#include "wx/xrc/xh_htmllbox.h"

#include "wx/htmllbox.h"
#include "wx/filesys.h"

#include "wx/xml/xml.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxSimpleHtmlListBoxXmlHandler, wxXmlResourceHandler);

wxSimpleHtmlListBoxXmlHandler::wxSimpleHtmlListBoxXmlHandler()
: wxXmlResourceHandler(), m_insideBox(false)
{
    XRC_ADD_STYLE(wxHLB_DEFAULT_STYLE);
    XRC_ADD_STYLE(wxHLB_MULTIPLE);
    AddWindowStyles();
}

wxObject *wxSimpleHtmlListBoxXmlHandler::DoCreateResource()
{
    if ( m_class == wxT("wxSimpleHtmlListBox"))
    {
        // find the selection
        long selection = GetLong(wxT("selection"), -1);

        // need to build the list of strings from children
        m_insideBox = true;
        CreateChildrenPrivately(NULL, GetParamNode(wxT("content")));
        m_insideBox = false;

        XRC_MAKE_INSTANCE(control, wxSimpleHtmlListBox)

        control->Create(m_parentAsWindow,
                        GetID(),
                        GetPosition(), GetSize(),
                        strList,
                        GetStyle(wxT("style"), wxHLB_DEFAULT_STYLE),
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

bool wxSimpleHtmlListBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return (IsOfClass(node, wxT("wxSimpleHtmlListBox")) ||
           (m_insideBox && node->GetName() == wxT("item")));
}

#endif // wxUSE_XRC && wxUSE_HTML
