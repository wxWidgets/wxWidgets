/////////////////////////////////////////////////////////////////////////////
// Name:        xh_spin.cpp
// Purpose:     XRC resource for wxSpinButton
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

#if wxUSE_XRC 

#include "wx/xrc/xh_spin.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinButtonXmlHandler, wxXmlResourceHandler)

wxSpinButtonXmlHandler::wxSpinButtonXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSP_HORIZONTAL);
    XRC_ADD_STYLE(wxSP_VERTICAL);
    XRC_ADD_STYLE(wxSP_ARROW_KEYS);
    XRC_ADD_STYLE(wxSP_WRAP);
    AddWindowStyles();
}

wxObject *wxSpinButtonXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxSpinButton)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(wxT("style"), wxSP_VERTICAL | wxSP_ARROW_KEYS),
                    GetName());

    control->SetValue(GetLong( wxT("value"), wxSP_DEFAULT_VALUE));
    control->SetRange(GetLong( wxT("min"), wxSP_DEFAULT_MIN),
                      GetLong(wxT("max"), wxSP_DEFAULT_MAX));
    SetupWindow(control);

    return control;
}

bool wxSpinButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxSpinButton"));
}

#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlXmlHandler, wxXmlResourceHandler)

wxSpinCtrlXmlHandler::wxSpinCtrlXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSP_HORIZONTAL);
    XRC_ADD_STYLE(wxSP_VERTICAL);
    XRC_ADD_STYLE(wxSP_ARROW_KEYS);
    XRC_ADD_STYLE(wxSP_WRAP);
}

wxObject *wxSpinCtrlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxSpinCtrl)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("value")),
                    GetPosition(), GetSize(),
                    GetStyle(wxT("style"), wxSP_ARROW_KEYS),
                    GetLong(wxT("min"), wxSP_DEFAULT_MIN),
                    GetLong(wxT("max"), wxSP_DEFAULT_MAX),
                    GetLong(wxT("value"), wxSP_DEFAULT_VALUE),
                    GetName());

    SetupWindow(control);

    return control;
}

bool wxSpinCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxSpinCtrl"));
}

#endif // wxUSE_SPINCTRL

#endif // wxUSE_XRC
