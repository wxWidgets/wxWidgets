/////////////////////////////////////////////////////////////////////////////
// Name:        xh_scrol.cpp
// Purpose:     XRC resource for wxScrollBar
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_scrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_scrol.h"
#include "wx/scrolbar.h"


wxScrollBarXmlHandler::wxScrollBarXmlHandler() 
: wxXmlResourceHandler() 
{
    XRC_ADD_STYLE(wxSB_HORIZONTAL);
    XRC_ADD_STYLE(wxSB_VERTICAL);
    AddWindowStyles();
}

wxObject *wxScrollBarXmlHandler::DoCreateResource()
{ 
    XRC_MAKE_INSTANCE(control, wxScrollBar)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    GetName());

    control->SetScrollbar(GetLong( wxT("value"), 0), 
                          GetLong( wxT("thumbsize"),1),
                          GetLong( wxT("range"), 10),
                          GetLong( wxT("pagesize"),1));

    SetupWindow(control);
    
    return control;
}

bool wxScrollBarXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxScrollBar"));
}
