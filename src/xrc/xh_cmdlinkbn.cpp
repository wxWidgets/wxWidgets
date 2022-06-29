/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_cmdlinkbn.cpp
// Purpose:     XRC resource for command link buttons
// Author:      Kinaou Herve
// Created:     2010/10/20
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_COMMANDLINKBUTTON

#include "wx/xrc/xh_cmdlinkbn.h"

#include "wx/commandlinkbutton.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxCommandLinkButtonXmlHandler, wxXmlResourceHandler);

wxCommandLinkButtonXmlHandler::wxCommandLinkButtonXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject *wxCommandLinkButtonXmlHandler::DoCreateResource()
{
   XRC_MAKE_INSTANCE(button, wxCommandLinkButton)

   button->Create(m_parentAsWindow,
                    GetID(),
                    GetText(wxS("label")),
                    GetText(wxS("note")),
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

    return button;
}

bool wxCommandLinkButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxS("wxCommandLinkButton"));
}

#endif // wxUSE_XRC && wxUSE_COMMANDLINKBUTTON
