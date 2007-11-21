/////////////////////////////////////////////////////////////////////////////
// Name:        xh_tglbtn.cpp
// Purpose:     XRC resource for wxToggleButton
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_TOGGLEBTN

#include "wx/xrc/xh_tglbtn.h"
#include "wx/tglbtn.h"

IMPLEMENT_DYNAMIC_CLASS(wxToggleButtonXmlHandler, wxXmlResourceHandler)

wxToggleButtonXmlHandler::wxToggleButtonXmlHandler()
: wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject *wxToggleButtonXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxToggleButton)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("label")),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    GetName());

    control->SetValue(GetBool( wxT("checked")));
    SetupWindow(control);

    return control;
}

bool wxToggleButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxToggleButton"));
}

#endif // wxUSE_XRC && wxUSE_TOGGLEBTN
