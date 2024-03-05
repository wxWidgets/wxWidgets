/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_bmpbt.cpp
// Purpose:     XRC resource for bitmap buttons
// Author:      Brian Gavin
// Created:     2000/09/09
// Copyright:   (c) 2000 Brian Gavin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_BMPBUTTON

#include "wx/xrc/xh_bmpbt.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapButtonXmlHandler, wxXmlResourceHandler);

wxBitmapButtonXmlHandler::wxBitmapButtonXmlHandler()
: wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxBU_AUTODRAW);
    XRC_ADD_STYLE(wxBU_LEFT);
    XRC_ADD_STYLE(wxBU_RIGHT);
    XRC_ADD_STYLE(wxBU_TOP);
    XRC_ADD_STYLE(wxBU_BOTTOM);
    XRC_ADD_STYLE(wxBU_EXACTFIT);
    AddWindowStyles();
}

// Function calls the given setter with the contents of the node with the given
// name, if present.
//
// If alternative parameter name is specified, it is used too.
void
wxBitmapButtonXmlHandler::SetBitmapIfSpecified(wxBitmapButton* button,
                                               BitmapSetter setter,
                                               const char* paramName,
                                               const char* paramNameAlt)
{
    if ( wxXmlNode* const node = GetParamNode(paramName) )
    {
        (button->*setter)(GetBitmapBundle(node));
    }
    else if ( paramNameAlt )
    {
        if ( wxXmlNode* const nodeAlt = GetParamNode(paramNameAlt) )
            (button->*setter)(GetBitmap(nodeAlt));
    }
}

wxObject *wxBitmapButtonXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(button, wxBitmapButton)

    if ( GetBool("close", 0) )
    {
        button->CreateCloseButton(m_parentAsWindow,
                                  GetID(),
                                  GetName());
    }
    else
    {
        button->Create(m_parentAsWindow,
                       GetID(),
                       GetBitmapBundle(wxT("bitmap"), wxART_BUTTON),
                       GetPosition(), GetSize(),
                       GetStyle(wxT("style")),
                       wxDefaultValidator,
                       GetName());
    }

    if (GetBool(wxT("default"), 0))
        button->SetDefault();
    SetupWindow(button);

    SetBitmapIfSpecified(button, &wxBitmapButton::SetBitmapPressed,
                         "pressed", "selected");
    SetBitmapIfSpecified(button, &wxBitmapButton::SetBitmapFocus, "focus");
    SetBitmapIfSpecified(button, &wxBitmapButton::SetBitmapDisabled, "disabled");
    SetBitmapIfSpecified(button, &wxBitmapButton::SetBitmapCurrent,
                         "current", "hover");

    return button;
}

bool wxBitmapButtonXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxBitmapButton"));
}

#endif // wxUSE_XRC && wxUSE_BMPBUTTON
