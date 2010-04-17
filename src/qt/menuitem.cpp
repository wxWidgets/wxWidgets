/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/menuitem.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/bitmap.h"

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
           int id,
           const wxString& text,
           const wxString& help,
           wxItemKind kind,
           wxMenu *subMenu)
{
}

void wxMenuItem::SetBitmap(const wxBitmap& bitmap)
{
}

const wxBitmap &wxMenuItem::GetBitmap() const
{
    static wxBitmap s_bitmap;

    return s_bitmap;
}

