/////////////////////////////////////////////////////////////////////////////
// Name:        xh_html.cpp
// Purpose:     XML resource for wxHtmlWindow
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_html.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_html.h"

#if wxUSE_HTML

#include "wx/html/htmlwin.h"


wxHtmlWindowXmlHandler::wxHtmlWindowXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxHW_SCROLLBAR_NEVER );
    ADD_STYLE( wxHW_SCROLLBAR_AUTO );
    AddWindowStyles();
}

wxObject *wxHtmlWindowXmlHandler::DoCreateResource()
{ 
    wxHtmlWindow *control = new wxHtmlWindow(m_ParentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    GetStyle( _T("style" ), wxHW_SCROLLBAR_AUTO),
                                    GetName()
                                    );

    if( HasParam( _T("borders") ))
    {
        control->SetBorders( GetDimension( _T("borders" )));
    }

    if( HasParam( _T("url") ))
    {
        control->LoadPage( GetParamValue( _T("url" )));
    }
    else if( HasParam( _T("htmlcode") ))
    {
        control->SetPage( GetText(_T("htmlcode")) );
    }

    SetupWindow(control);
    
    return control;
}



bool wxHtmlWindowXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("htmlwindow");
}

#endif // wxUSE_HTML
