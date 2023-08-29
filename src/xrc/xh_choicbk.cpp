/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_choicbk.cpp
// Purpose:     XRC resource for wxChoicebook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_CHOICEBOOK

#include "wx/xrc/xh_choicbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/choicebk.h"
#include "wx/imaglist.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxChoicebookXmlHandler, wxXmlResourceHandler);

wxChoicebookXmlHandler::wxChoicebookXmlHandler()
                      : m_choicebook(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    XRC_ADD_STYLE(wxCHB_DEFAULT);
    XRC_ADD_STYLE(wxCHB_LEFT);
    XRC_ADD_STYLE(wxCHB_RIGHT);
    XRC_ADD_STYLE(wxCHB_TOP);
    XRC_ADD_STYLE(wxCHB_BOTTOM);

    AddWindowStyles();
}

wxObject *wxChoicebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("choicebookpage"))
    {
        return DoCreatePage(m_choicebook);
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxChoicebook)

        nb->Create(m_parentAsWindow,
                   GetID(),
                   GetPosition(), GetSize(),
                   GetStyle(wxT("style")),
                   GetName());

        wxChoicebook *old_par = m_choicebook;
        m_choicebook = nb;

        DoCreatePages(m_choicebook);

        m_choicebook = old_par;

        return nb;
    }
}

bool wxChoicebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxChoicebook"))) ||
            (IsInside() && IsOfClass(node, wxT("choicebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_CHOICEBOOK
