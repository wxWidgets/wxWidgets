/////////////////////////////////////////////////////////////////////////////
// Name:        xh_chckb.cpp
// Purpose:     XML resource for wxCheckBox
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_chckb.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_chckb.h"
#include "wx/checkbox.h"

#if wxUSE_CHECKBOX

wxCheckBoxXmlHandler::wxCheckBoxXmlHandler() 
: wxXmlResourceHandler() 
{
}

wxObject *wxCheckBoxXmlHandler::DoCreateResource()
{ 
    wxCheckBox *control = new wxCheckBox(m_ParentAsWindow,
                                    GetID(),
                                    GetText(_T("label")),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    control->SetValue( GetBool( _T("checked")));
    SetupWindow(control);
    
    return control;
}



bool wxCheckBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("checkbox");
}

#endif
