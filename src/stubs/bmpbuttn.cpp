/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

#include "wx/bmpbuttn.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_buttonBitmap = bitmap;
    SetName(name);
    SetValidator(validator);
    parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;
    m_windowStyle = style;
    m_marginX = 0;
    m_marginY = 0;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    if ( width == -1 && bitmap.Ok())
	width = bitmap.GetWidth() + 2*m_marginX;

    if ( height == -1 && bitmap.Ok())
	height = bitmap.GetHeight() + 2*m_marginY;

    /* TODO: create bitmap button
     */

    return FALSE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
    m_buttonBitmap = bitmap;
}

