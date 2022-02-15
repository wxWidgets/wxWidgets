/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/taskbar.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/taskbar.h"

#include <QtWidgets/QSystemTrayIcon>

//=============================================================================

bool wxTaskBarIconBase::IsAvailable()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

//=============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType WXUNUSED(iconType))
{
    m_qtSystemTrayIcon = new QSystemTrayIcon;
}

wxTaskBarIcon::~wxTaskBarIcon()
{
    delete m_qtSystemTrayIcon;
}

bool wxTaskBarIcon::SetIcon(const wxBitmapBundle& WXUNUSED(icon),
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

