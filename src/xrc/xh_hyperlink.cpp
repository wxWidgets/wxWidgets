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
    wxHyperlinkCtrlBase* control = nullptr;
    if ( m_instance )
        control = wxStaticCast(m_instance, wxHyperlinkCtrlBase);

    if ( !control )
    {
        if ( m_class == "wxHyperlinkCtrl" )
        {
            control = new wxHyperlinkCtrl
                          (
                           m_parentAsWindow,
                           GetID(),
                           GetText(wxT("label")),
                           GetParamValue(wxT("url")),
                           GetPosition(), GetSize(),
                           GetStyle(wxT("style"), wxHL_DEFAULT_STYLE),
                           GetName()
                          );
        }
        else // m_class must be "wxGenericHyperlinkCtrl"
        {
            control = new wxGenericHyperlinkCtrl
                          (
                           m_parentAsWindow,
                           GetID(),
                           GetText(wxT("label")),
                           GetParamValue(wxT("url")),
                           GetPosition(), GetSize(),
                           GetStyle(wxT("style"), wxHL_DEFAULT_STYLE),
                           GetName()
                          );
        }
    }

    if ( GetBool("hidden", 0) == 1 )
        control->Hide();

    SetupWindow(control);

    return control;
}

bool wxHyperlinkCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxHyperlinkCtrl")) ||
           IsOfClass(node, wxT("wxGenericHyperlinkCtrl"));
}

#endif // wxUSE_XRC && wxUSE_HYPERLINKCTRL
