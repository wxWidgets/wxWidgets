/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/taskbar.cpp
// Author:      Peter Most
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

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType WXUNUSED(iconType))
{
}

bool wxTaskBarIcon::SetIcon(const wxIcon& WXUNUSED(icon),
             const wxString& WXUNUSED(tooltip))
{
    return false;
}

bool wxTaskBarIcon::RemoveIcon()
{
    return false;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *WXUNUSED(menu))
{
    return false;
}

