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
                                    GetBitmap(_T("bitmap")),
                                    GetPosition(), GetSize(),
                                    GetStyle(_T("style"), wxBU_AUTODRAW),
                                    wxDefaultValidator,
                                    GetName());
    if (GetBool(_T("default"), 0) == 1) button->SetDefault();
    SetupWindow(button);
    
    if (!GetParamValue(_T("selected")).IsEmpty())
        button->SetBitmapSelected(GetBitmap(_T("selected")));
    if (!GetParamValue(_T("focus")).IsEmpty())
        button->SetBitmapFocus(GetBitmap(_T("focus")));
    if (!GetParamValue(_T("disabled")).IsEmpty())
        button->SetBitmapDisabled(GetBitmap(_T("disabled")));
    
    return button;
}



bool wxBitmapButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxBitmapButton"));
}


