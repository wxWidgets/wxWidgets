/////////////////////////////////////////////////////////////////////////////
// Name:        xh_bmpbt.cpp
// Purpose:     XML resource for bitmap buttons
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_bmpbt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_bmpbt.h"
#include <wx/bmpbuttn.h>

wxBitmapButtonXmlHandler::wxBitmapButtonXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE(wxBU_AUTODRAW);
    ADD_STYLE(wxBU_LEFT);
    ADD_STYLE(wxBU_RIGHT);
    ADD_STYLE(wxBU_TOP);
    ADD_STYLE(wxBU_BOTTOM);
    AddWindowStyles();
}


wxObject *wxBitmapButtonXmlHandler::DoCreateResource()
{ 
    wxBitmapButton *button = new wxBitmapButton(m_ParentAsWindow,
                                    GetID(),
                                    GetBitmap(wxT("bitmap")),
                                    GetPosition(), GetSize(),
                                    GetStyle(wxT("style"), wxBU_AUTODRAW),
                                    wxDefaultValidator,
                                    GetName());
    if (GetBool(wxT("default"), 0) == 1) button->SetDefault();
    SetupWindow(button);
    
    if (!GetParamValue(wxT("selected")).IsEmpty())
        button->SetBitmapSelected(GetBitmap(wxT("selected")));
    if (!GetParamValue(wxT("focus")).IsEmpty())
        button->SetBitmapFocus(GetBitmap(wxT("focus")));
    if (!GetParamValue(wxT("disabled")).IsEmpty())
        button->SetBitmapDisabled(GetBitmap(wxT("disabled")));
    
    return button;
}



bool wxBitmapButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxBitmapButton"));
}


