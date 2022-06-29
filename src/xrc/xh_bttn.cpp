/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_bttn.cpp
// Purpose:     XRC resource for buttons
// Author:      Vaclav Slavik
// Created:     2000/03/05
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_BUTTON

#include "wx/xrc/xh_bttn.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxButtonXmlHandler, wxXmlResourceHandler);

wxButtonXmlHandler::wxButtonXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxBU_LEFT);
    XRC_ADD_STYLE(wxBU_RIGHT);
    XRC_ADD_STYLE(wxBU_TOP);
    XRC_ADD_STYLE(wxBU_BOTTOM);
    XRC_ADD_STYLE(wxBU_EXACTFIT);
    XRC_ADD_STYLE(wxBU_NOTEXT);
    AddWindowStyles();
}

wxObject *wxButtonXmlHandler::DoCreateResource()
{
   XRC_MAKE_INSTANCE(button, wxButton)

   button->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxT("label")),
                    GetPosition(), GetSize(),
                    GetStyle(),
                    wxDefaultValidator,
                    GetName());

    if (GetBool(wxT("default"), 0))
        button->SetDefault();

    if ( GetParamNode("bitmap") )
    {
        button->SetBitmap(GetBitmapBundle("bitmap", wxART_BUTTON),
                          GetDirection("bitmapposition"));
    }

    SetupWindow(button);

    const wxXmlNode* node = GetParamNode("pressed");
    if (node)
        button->SetBitmapPressed(GetBitmapBundle(node));
    node = GetParamNode("focus");
    if (node)
        button->SetBitmapFocus(GetBitmapBundle(node));
    node = GetParamNode("disabled");
    if (node)
        button->SetBitmapDisabled(GetBitmapBundle(node));
    node = GetParamNode("current");
    if (node)
        button->SetBitmapCurrent(GetBitmapBundle(node));

    const wxSize margins = GetSize("margins");
    if (margins != wxDefaultSize)
        button->SetBitmapMargins(margins);

    return button;
}

bool wxButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxButton"));
}

#endif // wxUSE_XRC && wxUSE_BUTTON
