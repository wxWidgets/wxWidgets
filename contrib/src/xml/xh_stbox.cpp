/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stbox.cpp
// Purpose:     XML resource for wxStaticBox
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_stbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_stbox.h"
#include "wx/statbox.h"

wxStaticBoxXmlHandler::wxStaticBoxXmlHandler() 
: wxXmlResourceHandler() 
{
    AddWindowStyles();
}

wxObject *wxStaticBoxXmlHandler::DoCreateResource()
{ 
    wxStaticBox *box = new wxStaticBox(m_ParentAsWindow,
                                    GetID(),
                                    GetText(wxT("label")),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    GetName()
                                    );
    SetupWindow(box);
    
    return box;
}



bool wxStaticBoxXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxStaticBox"));
}


