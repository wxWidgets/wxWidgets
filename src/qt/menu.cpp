/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/menu.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menu.h"

IMPLEMENT_DYNAMIC_CLASS( wxMenu, wxMenuBase )

wxMenu::wxMenu(long style)
{
}

wxMenu::wxMenu(const wxString& title, long style)
{
}

//##############################################################################

IMPLEMENT_DYNAMIC_CLASS( wxMenuBar, wxMenuBarBase )

wxMenuBar::wxMenuBar()
{
}

wxMenuBar::wxMenuBar(long style)
{
}

wxMenuBar::wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style)
{
}


void wxMenuBar::EnableTop(size_t pos, bool enable)
{
}


void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    return wxString();
}
