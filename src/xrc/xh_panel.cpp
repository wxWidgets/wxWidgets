/////////////////////////////////////////////////////////////////////////////
// Name:        xh_panel.cpp
// Purpose:     XRC resource for panels
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

#include "wx/xrc/xh_panel.h"
#include "wx/panel.h"


wxPanelXmlHandler::wxPanelXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxNO_3D);
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);
    XRC_ADD_STYLE(wxCLIP_CHILDREN);
    AddWindowStyles();
}

wxObject *wxPanelXmlHandler::DoCreateResource()
{ 
    XRC_MAKE_INSTANCE(panel, wxPanel)

    panel->Create(m_parentAsWindow,
                  GetID(),
                  GetPosition(), GetSize(),
                  GetStyle(wxT("style"), wxTAB_TRAVERSAL),
                  GetName());

    SetupWindow(panel);
    CreateChildren(panel);
    
    return panel;
}

bool wxPanelXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxPanel"));
}
