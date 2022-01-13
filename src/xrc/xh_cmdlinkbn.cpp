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
    XRC_ADD_STYLE(wxBU_LEFT);
    XRC_ADD_STYLE(wxBU_RIGHT);
    XRC_ADD_STYLE(wxBU_TOP);
    XRC_ADD_STYLE(wxBU_BOTTOM);
    XRC_ADD_STYLE(wxBU_EXACTFIT);
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
        button->SetBitmap(GetBitmap("bitmap", wxART_BUTTON),
                          GetDirection("bitmapposition"));
    }

    SetupWindow(button);

    return button;
}

bool wxCommandLinkButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxS("wxCommandLinkButton"));
}

#endif // wxUSE_XRC && wxUSE_COMMANDLINKBUTTON
