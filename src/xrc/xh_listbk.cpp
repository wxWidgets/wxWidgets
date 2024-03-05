/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_listbk.cpp
// Purpose:     XRC resource for wxListbook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_LISTBOOK

#include "wx/xrc/xh_listbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/listbook.h"
#include "wx/imaglist.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxListbookXmlHandler, wxXmlResourceHandler);

wxListbookXmlHandler::wxListbookXmlHandler()
                    : m_listbook(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    XRC_ADD_STYLE(wxLB_DEFAULT);
    XRC_ADD_STYLE(wxLB_LEFT);
    XRC_ADD_STYLE(wxLB_RIGHT);
    XRC_ADD_STYLE(wxLB_TOP);
    XRC_ADD_STYLE(wxLB_BOTTOM);

    AddWindowStyles();
}

wxObject *wxListbookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("listbookpage"))
    {
        return DoCreatePage(m_listbook);
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxListbook)

        nb->Create(m_parentAsWindow,
                   GetID(),
                   GetPosition(), GetSize(),
                   GetStyle(wxT("style")),
                   GetName());

        wxListbook *old_par = m_listbook;
        m_listbook = nb;

        DoCreatePages(m_listbook);

        m_listbook = old_par;

        return nb;
    }
}

bool wxListbookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxListbook"))) ||
            (IsInside() && IsOfClass(node, wxT("listbookpage"))));
}

#endif // wxUSE_XRC && wxUSE_LISTBOOK
