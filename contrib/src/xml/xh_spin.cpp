/////////////////////////////////////////////////////////////////////////////
// Name:        xh_spin.cpp
// Purpose:     XML resource for wxSpinButton
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_spin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_spin.h"
#include "wx/spinctrl.h"

#if wxUSE_SPINBTN

wxSpinButtonXmlHandler::wxSpinButtonXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxSP_HORIZONTAL );
    ADD_STYLE( wxSP_VERTICAL );
    ADD_STYLE( wxSP_ARROW_KEYS );
    ADD_STYLE( wxSP_WRAP );
}

wxObject *wxSpinButtonXmlHandler::DoCreateResource()
{ 
    wxSpinButton *control = new wxSpinButton(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    GetStyle( _T("style"), wxSP_VERTICAL | wxSP_ARROW_KEYS ),
                                    GetName()
                                    );

    control->SetValue( GetLong( _T("value"), wxSP_DEFAULT_VALUE) );
    control->SetRange( GetLong( _T("min"), wxSP_DEFAULT_MIN),
            GetLong( _T("max"), wxSP_DEFAULT_MAX) );
    SetupWindow(control);
    
    return control;
}



bool wxSpinButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("spinbutton");
}

#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL

wxSpinCtrlXmlHandler::wxSpinCtrlXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxSP_HORIZONTAL );
    ADD_STYLE( wxSP_VERTICAL );
    ADD_STYLE( wxSP_ARROW_KEYS );
    ADD_STYLE( wxSP_WRAP );
}

wxObject *wxSpinCtrlXmlHandler::DoCreateResource()
{ 
    wxSpinCtrl *control = new wxSpinCtrl(m_ParentAsWindow,
                                    GetID(),
                                    GetText(_T("label")),
                                    GetPosition(), GetSize(),
                                    GetStyle( _T("style"), wxSP_ARROW_KEYS ),
                                    GetLong( _T("min"), wxSP_DEFAULT_MIN),
                                    GetLong( _T("max"), wxSP_DEFAULT_MAX),
                                    GetLong( _T("value"), wxSP_DEFAULT_VALUE),
                                    GetName()
                                    );

    SetupWindow(control);
    
    return control;
}



bool wxSpinCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("spinctrl");
}

#endif // wxUSE_SPINCTRL
