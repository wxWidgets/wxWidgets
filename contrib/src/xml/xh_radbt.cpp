/////////////////////////////////////////////////////////////////////////////
// Name:        xh_radbt.cpp
// Purpose:     XML resource for wxRadioButton
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_radbt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_radbt.h"
#include "wx/radiobut.h"

#if wxUSE_RADIOBOX

wxRadioButtonXmlHandler::wxRadioButtonXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxRB_GROUP );
    AddWindowStyles();
}

wxObject *wxRadioButtonXmlHandler::DoCreateResource()
{ 
    /* BOBM - implementation note.
     * once the wxBitmapRadioButton is implemented.
     * look for a bitmap property. If not null, 
     * make it a wxBitmapRadioButton instead of the 
     * normal radio button.
     */ 

    wxRadioButton *control = new wxRadioButton(m_ParentAsWindow,
                                    GetID(),
                                    GetText(_T("label")),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    control->SetValue( GetBool(_T("value"), 0));
    SetupWindow(control);
    
    return control;
}



bool wxRadioButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("radiobutton");
}


#endif
