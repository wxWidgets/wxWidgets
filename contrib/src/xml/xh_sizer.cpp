/////////////////////////////////////////////////////////////////////////////
// Name:        xh_sizer.cpp
// Purpose:     XML resource for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_sizer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_sizer.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/statbox.h"
#include "wx/notebook.h"

wxSizerXmlHandler::wxSizerXmlHandler() 
: wxXmlResourceHandler(), m_IsInside(FALSE), m_ParentSizer(NULL)
{
    ADD_STYLE(wxHORIZONTAL);
    ADD_STYLE(wxVERTICAL);

    // and flags
    ADD_STYLE(wxLEFT);
    ADD_STYLE(wxRIGHT);
    ADD_STYLE(wxTOP);
    ADD_STYLE(wxBOTTOM);
    ADD_STYLE(wxNORTH);
    ADD_STYLE(wxSOUTH);
    ADD_STYLE(wxEAST);
    ADD_STYLE(wxWEST);
    ADD_STYLE(wxALL);

    ADD_STYLE(wxGROW);
    ADD_STYLE(wxEXPAND);
    ADD_STYLE(wxSHAPED);
    ADD_STYLE(wxSTRETCH_NOT);

    ADD_STYLE(wxALIGN_CENTER);
    ADD_STYLE(wxALIGN_CENTRE);
    ADD_STYLE(wxALIGN_LEFT);
    ADD_STYLE(wxALIGN_TOP);
    ADD_STYLE(wxALIGN_RIGHT);
    ADD_STYLE(wxALIGN_BOTTOM);
    ADD_STYLE(wxALIGN_CENTER_HORIZONTAL);
    ADD_STYLE(wxALIGN_CENTRE_HORIZONTAL);
    ADD_STYLE(wxALIGN_CENTER_VERTICAL);
    ADD_STYLE(wxALIGN_CENTRE_VERTICAL);
}



wxObject *wxSizerXmlHandler::DoCreateResource()
{ 
    if (m_Node->GetName() == _T("sizeritem"))
    {
        wxXmlNode *n = GetParamNode(_T("window"))->GetChildren();

        while (n)
        {
            if (n->GetType() == wxXML_ELEMENT_NODE)
            {        
                bool old_ins = m_IsInside;
                m_IsInside = FALSE;
                wxObject *item = CreateResFromNode(n, m_Parent, NULL);
                m_IsInside = old_ins;
                wxSizer *sizer = wxDynamicCast(item, wxSizer);
                wxWindow *wnd = wxDynamicCast(item, wxWindow);
                
                if (sizer)
                    m_ParentSizer->Add(sizer, GetLong(_T("option")), 
                                       GetStyle(_T("flag")), GetLong(_T("border")));
                else if (wnd)
                    m_ParentSizer->Add(wnd, GetLong(_T("option")), 
                                       GetStyle(_T("flag")), GetLong(_T("border")));
                else 
                    wxLogError(_T("Error in resource."));
                
                return item;
            }
            n = n->GetNext();
        }
        wxLogError(_T("Error in resource: no control/sizer within sizer's <item> tag."));
        return NULL;
    }
    
    else if (m_Node->GetName() == _T("spacer"))
    {
        wxCHECK_MSG(m_ParentSizer, NULL, _T("Incorrect syntax of XML resource: spacer not within sizer!"));
        wxSize sz = GetSize();
        m_ParentSizer->Add(sz.x, sz.y,
            GetLong(_T("option")), GetStyle(_T("flag")), GetLong(_T("border")));
        return NULL;
    }
    
#if wxUSE_NOTEBOOK
    else if (m_Node->GetName() == _T("notebooksizer"))
    {
        wxCHECK_MSG(m_ParentSizer, NULL, _T("Incorrect syntax of XML resource: notebooksizer not within sizer!"));        

        wxSizer *old_par = m_ParentSizer;
        m_ParentSizer = NULL;

        wxNotebook *nb = NULL;
        wxObject *item;
        wxXmlNode *n = GetParamNode(_T("window"))->GetChildren();
        while (n)
        {
            if (n->GetType() == wxXML_ELEMENT_NODE)
            {        
                item = CreateResFromNode(n, m_Parent, NULL);
                nb = wxDynamicCast(item, wxNotebook);
                break;
            }
            n = n->GetNext();
        }

        m_ParentSizer = old_par;
        
        wxCHECK_MSG(nb, NULL, _T("Incorrect syntax of XML resource: notebooksizer must contain a notebook!"));
        return new wxNotebookSizer(nb);
    }
#endif
    
    else {
        wxSizer *sizer = NULL;
        
        wxXmlNode *parentNode = m_Node->GetParent()->GetParent();

        wxCHECK_MSG(m_ParentSizer != NULL ||
                ((parentNode->GetName() == _T("panel") ||
                  parentNode->GetName() == _T("dialog")) &&
                 parentNode->GetType() == wxXML_ELEMENT_NODE), NULL,
                _T("Incorrect use of sizer: parent is not 'dialog' or 'panel'."));

        if (m_Node->GetName() == _T("boxsizer"))
            sizer = new wxBoxSizer(GetStyle(_T("orient"), wxHORIZONTAL));

        else if (m_Node->GetName() == _T("staticboxsizer"))
        {
            sizer = new wxStaticBoxSizer(
                         new wxStaticBox(m_ParentAsWindow, -1, GetText(_T("label"))),
                         GetStyle(_T("orient"), wxHORIZONTAL));
        }
        
        else if (m_Node->GetName() == _T("gridsizer"))
            sizer = new wxGridSizer(GetLong(_T("rows")), GetLong(_T("cols")),
                                    GetLong(_T("vgap")), GetLong(_T("hgap")));
                                    
        else if (m_Node->GetName() == _T("flexgridsizer"))
            sizer = new wxFlexGridSizer(GetLong(_T("rows")), GetLong(_T("cols")),
                                    GetLong(_T("vgap")), GetLong(_T("hgap")));

        wxSizer *old_par = m_ParentSizer;
        m_ParentSizer = sizer;
        bool old_ins = m_IsInside;
        m_IsInside = TRUE;
        CreateChildren(m_Parent, TRUE/*only this handler*/);
        m_IsInside = old_ins;
        m_ParentSizer = old_par;
        
        if (m_ParentSizer == NULL) // setup window:
        {
            m_ParentAsWindow->SetAutoLayout(TRUE);
            m_ParentAsWindow->SetSizer(sizer);

            wxXmlNode *nd = m_Node;
            m_Node = parentNode;
            if (GetSize() == wxDefaultSize)
                sizer->Fit(m_ParentAsWindow);
            m_Node = nd;

            if (m_ParentAsWindow->GetWindowStyle() & (wxRESIZE_BOX | wxRESIZE_BORDER))
                sizer->SetSizeHints(m_ParentAsWindow);
        }
        
        return sizer;
    }
}



bool wxSizerXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_IsInside && node->GetName() == _T("boxsizer")) ||
            (!m_IsInside && node->GetName() == _T("staticboxsizer")) ||
            (!m_IsInside && node->GetName() == _T("gridsizer")) ||
            (!m_IsInside && node->GetName() == _T("flexgridsizer")) ||
#if wxUSE_NOTEBOOK
            (!m_IsInside && node->GetName() == _T("notebooksizer")) ||
#endif
            (m_IsInside && node->GetName() == _T("sizeritem")) ||
            (m_IsInside && node->GetName() == _T("spacer")));
}
