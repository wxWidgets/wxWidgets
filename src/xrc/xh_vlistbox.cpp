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

    // listbox styles
    XRC_ADD_STYLE(wxLB_MULTIPLE);

    AddWindowStyles();
}

wxObject * wxVListBoxXmlHandler::DoCreateResource()
{
    /* non-standard because wxVListBox is an abstract
        class, so "subclass" must be used */
    wxCHECK_MSG(m_instance,
                nullptr,
                "wxVListBox requires \"subclass\" attribute");
    wxVListBox* const vlistbox = wxStaticCast(m_instance, wxVListBox);
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
