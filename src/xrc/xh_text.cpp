/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_text.cpp
// Purpose:     XRC resource for wxTextCtrl
// Author:      Aleksandras Gluchovas
// Created:     2000/03/21
// Copyright:   (c) 2000 Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_TEXTCTRL

#include "wx/xrc/xh_text.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxTextCtrlXmlHandler, wxXmlResourceHandler);

wxTextCtrlXmlHandler::wxTextCtrlXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxTE_NO_VSCROLL);
    XRC_ADD_STYLE(wxTE_PROCESS_ENTER);
    XRC_ADD_STYLE(wxTE_PROCESS_TAB);
    XRC_ADD_STYLE(wxTE_MULTILINE);
    XRC_ADD_STYLE(wxTE_PASSWORD);
    XRC_ADD_STYLE(wxTE_READONLY);
    XRC_ADD_STYLE(wxHSCROLL);
    XRC_ADD_STYLE(wxTE_RICH);
    XRC_ADD_STYLE(wxTE_RICH2);
    XRC_ADD_STYLE(wxTE_AUTO_URL);
    XRC_ADD_STYLE(wxTE_NOHIDESEL);
    XRC_ADD_STYLE(wxTE_LEFT);
    XRC_ADD_STYLE(wxTE_CENTRE);
    XRC_ADD_STYLE(wxTE_RIGHT);
    XRC_ADD_STYLE(wxTE_DONTWRAP);
    XRC_ADD_STYLE(wxTE_CHARWRAP);
    XRC_ADD_STYLE(wxTE_WORDWRAP);

    // this style doesn't exist since wx 2.9.0 but we still support it (by
    // ignoring it silently) in XRC files to avoid unimportant warnings when
    // using XRC produced by old tools
    AddStyle(wxT("wxTE_AUTO_SCROLL"), 0);

    AddWindowStyles();
}

wxObject *wxTextCtrlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(text, wxTextCtrl)

    text->Create(m_parentAsWindow,
                 GetID(),
                 GetText(wxT("value")),
                 GetPosition(), GetSize(),
                 GetStyle(),
                 wxDefaultValidator,
                 GetName());

    SetupWindow(text);

    if (HasParam(wxT("maxlength")))
        text->SetMaxLength(GetLong(wxT("maxlength")));
    if (GetBool(wxS("forceupper")))
        text->ForceUpper();

    const wxString hint = GetText(wxS("hint"));
    if (!hint.empty())
        text->SetHint(hint);

    return text;
}

bool wxTextCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxTextCtrl"));
}

#endif // wxUSE_XRC && wxUSE_TEXTCTRL
