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

#include "wx/xml/xh_notbk.h"

#if wxUSE_NOTEBOOK

#include "wx/log.h"
#include "wx/notebook.h"
#include "wx/sizer.h"

wxNotebookXmlHandler::wxNotebookXmlHandler() 
: wxXmlResourceHandler(), m_IsInside(FALSE), m_Notebook(NULL)
{
    ADD_STYLE(wxNB_FIXEDWIDTH);
    ADD_STYLE(wxNB_LEFT);
    ADD_STYLE(wxNB_RIGHT);
    ADD_STYLE(wxNB_BOTTOM);
    AddWindowStyles();
}



wxObject *wxNotebookXmlHandler::DoCreateResource()
{ 
    if (m_Class == _T("notebookpage"))
    {
        wxXmlNode *n = GetParamNode(_T("object"));

        if (n)
        {
            bool old_ins = m_IsInside;
            m_IsInside = FALSE;
            m_IsInside = old_ins;
            wxObject *item = CreateResFromNode(n, m_Notebook, NULL);
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
                m_Notebook->AddPage(wnd, GetText(_T("label")),
                                         GetBool(_T("selected"), 0));
            else 
                wxLogError(_T("Error in resource."));              
            return wnd;
        }
        else
        {
            wxLogError(_T("Error in resource: no control within notebook's <page> tag."));
            return NULL;
        }
    }
    
    else {
        wxNotebook *nb = new wxNotebook(m_ParentAsWindow, 
                                        GetID(),
                                        GetPosition(), GetSize(),
                                        GetStyle( _T("style" )),
                                        GetName());
    
        wxNotebook *old_par = m_Notebook;
        m_Notebook = nb;
        bool old_ins = m_IsInside;
        m_IsInside = TRUE;
        CreateChildren(m_Notebook, TRUE/*only this handler*/);
        m_IsInside = old_ins;
        m_Notebook = old_par;

        if (GetBool(_T("usenotebooksizer"), FALSE))
            return new wxNotebookSizer(nb);
        else
            return nb;
    }
}



bool wxNotebookXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_IsInside && IsOfClass(node, _T("wxNotebook"))) ||
            (m_IsInside && IsOfClass(node, _T("notebookpage"))));
}

#endif
