/////////////////////////////////////////////////////////////////////////////
// Name:        xh_text.cpp
// Purpose:     XRC resource for wxTextCtrl
// Author:      Aleksandras Gluchovas
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_text.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_text.h"
#include "wx/textctrl.h"

wxTextCtrlXmlHandler::wxTextCtrlXmlHandler() : wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxTE_PROCESS_ENTER);
    XRC_ADD_STYLE(wxTE_PROCESS_TAB);
    XRC_ADD_STYLE(wxTE_MULTILINE);
    XRC_ADD_STYLE(wxTE_PASSWORD);
    XRC_ADD_STYLE(wxTE_READONLY);
    XRC_ADD_STYLE(wxHSCROLL);
    XRC_ADD_STYLE(wxTE_RICH);
    XRC_ADD_STYLE(wxTE_RICH2);
    XRC_ADD_STYLE(wxTE_AUTO_URL);
    XRC_ADD_STYLE(wxTE_NOHIDESEL);
    XRC_ADD_STYLE(wxTE_LEFT);
    XRC_ADD_STYLE(wxTE_CENTRE);
    XRC_ADD_STYLE(wxTE_RIGHT);
    XRC_ADD_STYLE(wxTE_DONTWRAP);
    XRC_ADD_STYLE(wxTE_LINEWRAP);
    XRC_ADD_STYLE(wxTE_WORDWRAP);
    AddWindowStyles();
}

wxObject *wxTextCtrlXmlHandler::DoCreateResource()
{ 
    XRC_MAKE_INSTANCE(text, wxTextCtrl)

    text->Create(m_parentAsWindow,
                 GetID(),
                 GetText(wxT("value")),
                 GetPosition(), GetSize(),
                 GetStyle(),
                 wxDefaultValidator,
                 GetName());

    SetupWindow(text);
    
    return text;
}

bool wxTextCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxTextCtrl"));
}
