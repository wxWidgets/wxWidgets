/////////////////////////////////////////////////////////////////////////////
// Name:        xh_toolb.cpp
// Purpose:     XML resource for wxBoxSizer
// Author:      Vaclav Slavik
// Created:     2000/08/11
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_toolb.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_toolb.h"
#include "wx/toolbar.h"


#if wxUSE_TOOLBAR

wxToolBarXmlHandler::wxToolBarXmlHandler() 
: wxXmlResourceHandler(), m_IsInside(FALSE), m_Toolbar(NULL)
{
    ADD_STYLE(wxTB_FLAT);
    ADD_STYLE(wxTB_DOCKABLE);
    ADD_STYLE(wxTB_VERTICAL);
    ADD_STYLE(wxTB_HORIZONTAL);
}



wxObject *wxToolBarXmlHandler::DoCreateResource()
{ 
    if (m_Class == _T("tool"))
    {
        wxCHECK_MSG(m_Toolbar, NULL, _T("Incorrect syntax of XML resource: tool not within a toolbar!"));
        m_Toolbar->AddTool(GetID(),
                           GetBitmap(_T("bitmap")),
                           GetBitmap(_T("bitmap2")),
                           GetBool(_T("toggle")),
                           GetPosition().x,
                           GetPosition().y,
                           NULL,
                           GetText(_T("tooltip")),
                           GetText(_T("longhelp")));
        return m_Toolbar; // must return non-NULL
    }
    
    else if (m_Class == _T("separator"))
    {
        wxCHECK_MSG(m_Toolbar, NULL, _T("Incorrect syntax of XML resource: separator not within a toolbar!"));
        m_Toolbar->AddSeparator();
        return m_Toolbar; // must return non-NULL
    }
    
    else /*<object class="wxToolBar">*/
    {
        int style = GetStyle(_T("style"), wxNO_BORDER | wxTB_HORIZONTAL);
#ifdef __WXMSW__
        if (!(style & wxNO_BORDER)) style |= wxNO_BORDER;
#endif
        wxToolBar *toolbar = new wxToolBar(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(),
                                    GetSize(),
                                    style,
                                    GetName());

        wxSize bmpsize = GetSize(_T("bitmapsize"));
        if (!(bmpsize == wxDefaultSize))
            toolbar->SetToolBitmapSize(bmpsize);
        wxSize margins = GetSize(_T("margins"));
        if (!(margins == wxDefaultSize))
            toolbar->SetMargins(margins.x, margins.y);
        long packing = GetLong(_T("packing"), -1);
        if (packing != -1)
            toolbar->SetToolPacking(packing);
        long separation = GetLong(_T("separation"), -1);
        if (separation != -1)
            toolbar->SetToolSeparation(separation);

        wxXmlNode *children_node = GetParamNode(_T("object"));
        if (children_node == NULL) return toolbar;

        m_IsInside = TRUE;
        m_Toolbar = toolbar;

        wxXmlNode *n = children_node;

        while (n)
        {
            if (n->GetType() == wxXML_ELEMENT_NODE && 
                n->GetName() == _T("object"))
            {
                wxObject *created = CreateResFromNode(n, toolbar, NULL);
                wxControl *control = wxDynamicCast(created, wxControl);
                if (IsOfClass(n, _T("tool")) &&
                    IsOfClass(n, _T("separator")) &&
                    control != NULL)
                    toolbar->AddControl(control);
            }
            n = n->GetNext();
        }

        m_IsInside = FALSE;
        m_Toolbar = NULL;

        toolbar->Realize();
        return toolbar;
    }
}



bool wxToolBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return ((!m_IsInside && IsOfClass(node, _T("wxToolBar"))) ||
            (m_IsInside && IsOfClass(node, _T("tool"))) || 
            (m_IsInside && IsOfClass(node, _T("separator"))));
}

#endif
