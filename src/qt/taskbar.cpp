/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/taskbar.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/taskbar.h"

//=============================================================================

bool wxTaskBarIconBase::IsAvailable()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

//=============================================================================

IMPLEMENT_DYNAMIC_CLASS( wxTaskBarIcon, wxTaskBarIconBase )

wxTaskBarIcon::wxTaskBarIcon()
{
}

bool wxTaskBarIcon::SetIcon(const wxIcon& icon,
             const wxString& tooltip)
{
    return false;
}

bool wxTaskBarIcon::RemoveIcon()
{
    return false;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu)
{
    return false;
}

