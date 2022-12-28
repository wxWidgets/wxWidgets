/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_notbk.cpp
// Purpose:     XRC resource for wxNotebook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_NOTEBOOK

#include "wx/xrc/xh_notbk.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/sizer.h"
#endif

#include "wx/notebook.h"
#include "wx/imaglist.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxNotebookXmlHandler, wxXmlResourceHandler);

wxNotebookXmlHandler::wxNotebookXmlHandler()
                    : m_notebook(nullptr)
{
    XRC_ADD_STYLE(wxBK_DEFAULT);
    XRC_ADD_STYLE(wxBK_LEFT);
    XRC_ADD_STYLE(wxBK_RIGHT);
    XRC_ADD_STYLE(wxBK_TOP);
    XRC_ADD_STYLE(wxBK_BOTTOM);

    // provide the old synonyms for these fields as well
    XRC_ADD_STYLE(wxNB_DEFAULT);
    XRC_ADD_STYLE(wxNB_LEFT);
    XRC_ADD_STYLE(wxNB_RIGHT);
    XRC_ADD_STYLE(wxNB_TOP);
    XRC_ADD_STYLE(wxNB_BOTTOM);

    XRC_ADD_STYLE(wxNB_FIXEDWIDTH);
    XRC_ADD_STYLE(wxNB_MULTILINE);
    XRC_ADD_STYLE(wxNB_NOPAGETHEME);

    AddWindowStyles();
}

wxObject *wxNotebookXmlHandler::DoCreateResource()
{
    if (m_class == wxT("notebookpage"))
    {
        return DoCreatePage(m_notebook);
    }

    else
    {
        XRC_MAKE_INSTANCE(nb, wxNotebook)

        nb->Create(m_parentAsWindow,
                   GetID(),
                   GetPosition(), GetSize(),
                   GetStyle(wxT("style")),
                   GetName());

        SetupWindow(nb);

        wxNotebook *old_par = m_notebook;
        m_notebook = nb;

        DoCreatePages(m_notebook);

        m_notebook = old_par;

        return nb;
    }
}

bool wxNotebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!IsInside() && IsOfClass(node, wxT("wxNotebook"))) ||
            (IsInside() && IsOfClass(node, wxT("notebookpage"))));
}

#endif // wxUSE_XRC && wxUSE_NOTEBOOK
