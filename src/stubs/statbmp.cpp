/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

#include "wx/statbmp.h"

IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

/*
 * wxStaticBitmap
 */

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_messageBitmap = bitmap;
    SetName(name);
    if (parent) parent->AddChild(this);

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style;

    // TODO: create static bitmap control
    return FALSE;
}

void wxStaticBitmap::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    m_messageBitmap = bitmap;

    // TODO: redraw bitmap
}

