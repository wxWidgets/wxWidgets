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

#include "wx/xrc/xh_html.h"

#if wxUSE_HTML

#include "wx/html/htmlwin.h"
#include "wx/filesys.h"


wxHtmlWindowXmlHandler::wxHtmlWindowXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxHW_SCROLLBAR_NEVER );
    ADD_STYLE( wxHW_SCROLLBAR_AUTO );
    AddWindowStyles();
}

wxObject *wxHtmlWindowXmlHandler::DoCreateResource()
{ 
    wxHtmlWindow *control = new wxHtmlWindow(m_parentAsWindow,
                                    GetID(),
                                    GetPosition(), GetSize(),
                                    GetStyle( wxT("style" ), wxHW_SCROLLBAR_AUTO),
                                    GetName()
                                    );

    if( HasParam( wxT("borders") ))
    {
        control->SetBorders( GetDimension( wxT("borders" )));
    }

    if( HasParam( wxT("url") ))
    {
        wxString url = GetParamValue(wxT("url" ));
        wxFileSystem& fsys = GetCurFileSystem();
        
        wxFSFile *f = fsys.OpenFile(url);
        if (f)
        {
            control->LoadPage(f->GetLocation());
            delete f;
        }
        else
            control->LoadPage(url);
    }
    
    else if( HasParam( wxT("htmlcode") ))
    {
        control->SetPage( GetText(wxT("htmlcode")) );
    }

    SetupWindow(control);
    
    return control;
}



bool wxHtmlWindowXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxHtmlWindow"));
}

#endif // wxUSE_HTML
