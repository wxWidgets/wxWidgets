/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_bmp.cpp
// Purpose:     XRC resource for wxBitmap and wxIcon
// Author:      Vaclav Slavik
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "wx/xrc/xh_bmp.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxBitmapXmlHandler, wxXmlResourceHandler)

wxBitmapXmlHandler::wxBitmapXmlHandler()
                   :wxXmlResourceHandler()
{
}

wxObject *wxBitmapXmlHandler::DoCreateResource()
{
    // NB: empty parameter name means "take directly from this node's next
    //     instead of from subnode with given name"
    return new wxBitmap(GetBitmap(wxEmptyString));
}

bool wxBitmapXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxBitmap"));
}

IMPLEMENT_DYNAMIC_CLASS(wxIconXmlHandler, wxXmlResourceHandler)

wxIconXmlHandler::wxIconXmlHandler()
: wxXmlResourceHandler()
{
}

wxObject *wxIconXmlHandler::DoCreateResource()
{
    // NB: empty parameter name means "take directly from this node's next
    //     instead of from subnode with given name"
    return new wxIcon(GetIcon(wxEmptyString));
}

bool wxIconXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxIcon"));
}

#endif // wxUSE_XRC
