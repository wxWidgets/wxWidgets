/////////////////////////////////////////////////////////////////////////////
// Name:        xh_statpict.cpp
// Purpose:     XRC resource handler for wxStaticPicture
// Author:      David A. Norris
// Created:     2005/03/13
// RCS-ID:      $Id$
// Copyright:   David A. Norris
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include "wx/gizmos/statpict.h"
#include "wx/gizmos/xh_statpict.h"

// Register with wxWindows' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(wxStaticPictureXmlHandler, wxXmlResourceHandler)

// Constructor.
wxStaticPictureXmlHandler::wxStaticPictureXmlHandler()
{
    AddWindowStyles();
}

// Creates the control and returns a pointer to it.
wxObject *wxStaticPictureXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxStaticPicture)

    control->Create(m_parentAsWindow, GetID(),
        GetBitmap(wxT("bitmap"), wxART_OTHER, GetSize()),
        GetPosition(), GetSize(), GetStyle(), GetName());

    SetupWindow(control);

    return control;
}

// Returns true if we know how to create a control for the given node.
bool wxStaticPictureXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxStaticPicture"));
}

#endif // wxUSE_XRC
