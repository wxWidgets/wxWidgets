/////////////////////////////////////////////////////////////////////////////
// Name:        xh_notbk.cpp
// Purpose:     XML resource for wxNotebook
// Author:      Vaclav Slavik
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_notbk.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_notbk.h"

#if wxUSE_NOTEBOOK

#include "wx/log.h"
#include "wx/notebook.h"
#include "wx/sizer.h"

wxNotebookXmlHandler::wxNotebookXmlHandler() 
: wxXmlResourceHandler(), m_isInside(FALSE), m_notebook(NULL)
{
    ADD_STYLE(wxNB_FIXEDWIDTH);
    ADD_STYLE(wxNB_LEFT);
    ADD_STYLE(wxNB_RIGHT);
    ADD_STYLE(wxNB_BOTTOM);
    AddWindowStyles();
}



wxObject *wxNotebookXmlHandler::DoCreateResource()
{ 
    if (m_class == wxT("notebookpage"))
    {
        wxXmlNode *n = GetParamNode(wxT("object"));

        if (n)
        {
            bool old_ins = m_isInside;
            m_isInside = FALSE;
            m_isInside = old_ins;
            wxObject *item = CreateResFromNode(n, m_notebook, NULL);
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
                m_notebook->AddPage(wnd, GetText(wxT("label")),
                                         GetBool(wxT("selected"), 0));
            else 
                wxLogError(wxT("Error in resource."));              
            return wnd;
        }
        else
        {
            wxLogError(wxT("Error in resource: no control within notebook's <page> tag."));
            return NULL;
        }
    }
    
    else {
        wxNotebook *nb = new wxNotebook(m_parentAsWindow, 
                                        GetID(),
                                        GetPosition(), GetSize(),
                                        GetStyle( wxT("style" )),
                                        GetName());
    
        wxNotebook *old_par = m_notebook;
        m_notebook = nb;
        bool old_ins = m_isInside;
        m_isInside = TRUE;
        CreateChildren(m_notebook, TRUE/*only this handler*/);
        m_isInside = old_ins;
        m_notebook = old_par;

        if (GetBool(wxT("usenotebooksizer"), FALSE))
            return new wxNotebookSizer(nb);
        else
            return nb;
    }
}



bool wxNotebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_isInside && IsOfClass(node, wxT("wxNotebook"))) ||
            (m_isInside && IsOfClass(node, wxT("notebookpage"))));
}

#endif
