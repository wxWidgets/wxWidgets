/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stbox.cpp
// Purpose:     XML resource for wxStaticLine
// Author:      Brian Gavin
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_stlin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_stlin.h"
#include "wx/statline.h"

#if wxUSE_STATLINE

wxStaticLineXmlHandler::wxStaticLineXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE(wxLI_HORIZONTAL);
    ADD_STYLE(wxLI_VERTICAL);
    AddWindowStyles();
}

wxObject *wxStaticLineXmlHandler::DoCreateResource()
{ 
    wxStaticLine *line = new wxStaticLine(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    GetStyle(_T("style"), wxLI_HORIZONTAL),
                                    GetName()
                                    );
    SetupWindow(line);
    
    return line;
}



bool wxStaticLineXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxStaticLine"));
}

#endif
