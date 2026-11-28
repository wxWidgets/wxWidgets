/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_hyperlink.cpp
// Purpose:     Hyperlink control
// Author:      David Norris <danorris@gmail.com>
// Modified by: Ryan Norton, Francesco Montorsi
// Created:     04/02/2005
// Copyright:   (c) 2005 David Norris
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
// Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_XRC && wxUSE_HYPERLINKCTRL

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include "wx/xrc/xh_hyperlink.h"

#ifndef WX_PRECOMP
#endif

#include "wx/hyperlink.h"
#include "wx/weakref.h"
#include "wx/xrc/xmlres.h"

//===========================================================================
// Implementation
//===========================================================================

//---------------------------------------------------------------------------
// wxHyperlinkCtrlXmlHandler
//---------------------------------------------------------------------------

// Register with wxWindows' dynamic class subsystem.
wxIMPLEMENT_DYNAMIC_CLASS(wxHyperlinkCtrlXmlHandler, wxXmlResourceHandler);

wxHyperlinkCtrlXmlHandler::wxHyperlinkCtrlXmlHandler()
{
    XRC_ADD_STYLE(wxHL_CONTEXTMENU);
    XRC_ADD_STYLE(wxHL_ALIGN_LEFT);
    XRC_ADD_STYLE(wxHL_ALIGN_RIGHT);
    XRC_ADD_STYLE(wxHL_ALIGN_CENTRE);
    XRC_ADD_STYLE(wxHL_DEFAULT_STYLE);

    AddWindowStyles();
}

wxObject *wxHyperlinkCtrlXmlHandler::DoCreateResource()
{
    const wxString label = GetText(wxT("label"));
    const wxString url = GetParamValue(wxT("url"));
    const wxPoint position = GetPosition();
    const wxSize size = GetSize();
    const long style = GetStyle(wxT("style"), wxHL_DEFAULT_STYLE);
    const wxString name = GetName();
    const bool allocatedByHandler = !m_instance;

    wxHyperlinkCtrlBase* control = nullptr;
    if ( m_class == "wxHyperlinkCtrl" )
    {
        control =
            m_instance
                ? wxStaticCast(m_instance, wxHyperlinkCtrl)
                : new wxHyperlinkCtrl;
    }
    else // m_class must be "wxGenericHyperlinkCtrl"
    {
        control =
            m_instance
                ? wxStaticCast(m_instance, wxGenericHyperlinkCtrl)
                : new wxGenericHyperlinkCtrl;
    }

    wxWeakRef<wxHyperlinkCtrlBase> controlAlive(control);
    const bool created =
        m_class == "wxHyperlinkCtrl"
            ? static_cast<wxHyperlinkCtrl *>(control)->Create(
                  m_parentAsWindow, GetID(), label, url,
                  position, size, style, name)
            : static_cast<wxGenericHyperlinkCtrl *>(control)->Create(
                  m_parentAsWindow, GetID(), label, url,
                  position, size, style, name);

    // m_instance is a default-constructed XRC subclass. It must go through
    // the same two-step Create() path as an object allocated by this handler;
    // returning the bare C++ object leaves it without a parent or native peer.
    if ( !created || !controlAlive )
    {
        ReportError("could not create hyperlink control");
        if ( allocatedByHandler && controlAlive )
            delete controlAlive.get();
        return nullptr;
    }

    if ( GetBool("hidden", 0) == 1 )
    {
        controlAlive->Hide();
        if ( !controlAlive )
            return nullptr;
    }

    SetupWindow(controlAlive.get());
    if ( !controlAlive )
        return nullptr;

    return controlAlive.get();
}

bool wxHyperlinkCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxHyperlinkCtrl")) ||
           IsOfClass(node, wxT("wxGenericHyperlinkCtrl"));
}

#endif // wxUSE_XRC && wxUSE_HYPERLINKCTRL
