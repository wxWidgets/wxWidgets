/////////////////////////////////////////////////////////////////////////////
// Name:        xh_bttn.cpp
// Purpose:     XRC resource for buttons
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_bttn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_bttn.h"
#include "wx/button.h"


wxButtonXmlHandler::wxButtonXmlHandler() 
: wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxBU_LEFT);
    XRC_ADD_STYLE(wxBU_RIGHT);
    XRC_ADD_STYLE(wxBU_TOP);
    XRC_ADD_STYLE(wxBU_BOTTOM);
    AddWindowStyles();
}

wxObject *wxButtonXmlHandler::DoCreateResource()
{ 
   XRC_MAKE_INSTANCE(button, wxButton)

   button->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("label")),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    GetName());

    if (GetBool(wxT("default"), 0))
        button->SetDefault();
    SetupWindow(button);
    
    return button;
}

bool wxButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxButton"));
}
