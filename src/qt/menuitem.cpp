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
#include "wx/menu.h"
#include "wx/bitmap.h"
#include "wx/qt/converter.h"

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu, int id, const wxString& text,
        const wxString& help, wxItemKind kind, wxMenu *subMenu)
    : wxMenuItemBase( parentMenu, id, text, help, kind, subMenu )
{
    m_qtAction = new QAction( wxQtConvertString( text ), parentMenu->GetHandle() );
    m_qtAction->setStatusTip( wxQtConvertString( help ));
    switch ( kind )
    {
        case wxITEM_SEPARATOR:
            m_qtAction->setSeparator( true );
            break;
        case wxITEM_CHECK:
            m_qtAction->setChecked( true );
            break;
        case wxITEM_RADIO:
            wxMISSING_IMPLEMENTATION( "wxITEM_RADIO" );
            break;
        case wxITEM_NORMAL:
            // Normal for a menu item.
            break;
        case wxITEM_DROPDOWN:
        case wxITEM_MAX:
            // Not applicable for menu items.
            break;
    }
    // QObject::connect( m_qtAction, SIGNAL( triggered() ), this, SLOT( OnTriggered() ));
}

void wxMenuItem::SetBitmap(const wxBitmap& bitmap)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

const wxBitmap &wxMenuItem::GetBitmap() const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    static wxBitmap s_bitmap;

    return s_bitmap;
}

QAction *wxMenuItem::GetHandle() const
{
    return m_qtAction;
}

