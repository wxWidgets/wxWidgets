/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:	    Implements wxTaskBarIcon class for manipulating icons on
//              the task bar. Optional.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "taskbar.h"
#endif

#include <wx/taskbar.h>

wxTaskBarIcon::wxTaskBarIcon()
{
    // TODO
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    // TODO
}

// Operations
bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    // TODO
    return FALSE;
}

bool wxTaskBarIcon::RemoveIcon()
{
    // TODO
    return FALSE;
}

// Overridables
void wxTaskBarIcon::OnMouseMove()
{
}

void wxTaskBarIcon::OnLButtonDown()
{
}

void wxTaskBarIcon::OnLButtonUp()
{
}

void wxTaskBarIcon::OnRButtonDown()
{
}

void wxTaskBarIcon::OnRButtonUp()
{
}

void wxTaskBarIcon::OnLButtonDClick()
{
}

void wxTaskBarIcon::OnRButtonDClick()
{
}

