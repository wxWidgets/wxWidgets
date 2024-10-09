/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_vlistbox.cpp
// Purpose:     XRC resource for vlistboxes
// Author:      Vaclav Slavik, Bill Su
// Created:     2000/03/05, 2024/10/07
// Copyright:   (c) 2000 Vaclav Slavik, 2024 Bill Su
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_LISTBOX

#include "wx/xrc/xh_vlistbox.h"

#include "wx/vlbox.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxVListBoxXmlHandler, wxXmlResourceHandler);

wxVListBoxXmlHandler::wxVListBoxXmlHandler() : wxXmlResourceHandler()
{
    // panel styles
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    /* I don't think it makes much sense for wxVListBox to have children
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    */

    // listbox styles
    /* docs say the only special style is */
    XRC_ADD_STYLE(wxLB_MULTIPLE);

    AddWindowStyles();
}

wxObject * wxVListBoxXmlHandler::DoCreateResource()
{
    /* non-standard because wxVListBox is an abstract
        class, so "subclass" must be used */
    wxVListBox *vlistbox = nullptr;
    wxCHECK_MSG(m_instance,
                nullptr,
                "wxVListBox requires \"subclass\" attribute");
    vlistbox = wxStaticCast(m_instance, wxVListBox);
    if (GetBool(wxT("hidden"), 0) == 1)
        vlistbox->Hide();

    vlistbox->Create(m_parentAsWindow,
                  GetID(),
                  GetPosition(), GetSize(),
                  GetStyle(wxT("style"), wxTAB_TRAVERSAL),
                  GetName());

    SetupWindow(vlistbox);
    CreateChildren(vlistbox);

    return vlistbox;
}

bool wxVListBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxVListBox"));
}

#endif // wxUSE_XRC && wxUSE_LISTBOX
