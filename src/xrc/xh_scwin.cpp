/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_scwin.cpp
// Purpose:     XRC resource for wxScrolledWindow
// Author:      Vaclav Slavik
// Created:     2002/10/18
// Copyright:   (c) 2002 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC

#include "wx/xrc/xh_scwin.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/scrolwin.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxScrolledWindowXmlHandler, wxXmlResourceHandler);

wxScrolledWindowXmlHandler::wxScrolledWindowXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxHSCROLL);
    XRC_ADD_STYLE(wxVSCROLL);

    // wxPanel styles
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);

    AddWindowStyles();
}

wxObject *wxScrolledWindowXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxScrolledWindow)

    control->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(), GetSize(),
                    GetStyle(wxT("style"), wxHSCROLL | wxVSCROLL),
                    GetName());

    SetupWindow(control);
    CreateChildren(control);

    if ( HasParam(wxT("scrollrate")) )
    {
        wxSize rate = GetSize(wxT("scrollrate"));
        control->SetScrollRate(rate.x, rate.y);
    }

    return control;
}

bool wxScrolledWindowXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxScrolledWindow"));
}

#endif // wxUSE_XRC
