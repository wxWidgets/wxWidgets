/////////////////////////////////////////////////////////////////////////////
// Name:        xh_panel.cpp
// Purpose:     XML resource for panels
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_panel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_panel.h"
#include "wx/panel.h"


wxPanelXmlHandler::wxPanelXmlHandler() : wxXmlResourceHandler()
{
    ADD_STYLE(wxNO_3D);
    ADD_STYLE(wxTAB_TRAVERSAL);
    ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    ADD_STYLE(wxCLIP_CHILDREN);
    AddWindowStyles();
}



wxObject *wxPanelXmlHandler::DoCreateResource()
{ 
    wxPanel *panel = new wxPanel(m_ParentAsWindow,
                                 GetID(),
                                 GetPosition(), GetSize(),
                                 GetStyle(_T("style"), 0),
                                 GetName());
    SetupWindow(panel);
    CreateChildren(panel);
    
    return panel;
}


bool wxPanelXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("panel");
}
