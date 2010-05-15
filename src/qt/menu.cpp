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
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

IMPLEMENT_DYNAMIC_CLASS( wxMenu, wxMenuBase )

static void ApplyStyle( QMenu *qtMenu, long style )
{
    if ( style & wxMENU_TEAROFF )
        qtMenu->setTearOffEnabled( true );
}

wxMenu::wxMenu(long style)
    : wxMenuBase()
{
    m_qtMenu = new QMenu();

    ApplyStyle( m_qtMenu, style );
}

wxMenu::wxMenu(const wxString& title, long style)
    : wxMenuBase( title, style )
{
    m_qtMenu = new QMenu( wxQtConvertString( title ));

    ApplyStyle( m_qtMenu, style );
}


wxMenuItem *wxMenu::DoAppend(wxMenuItem *item)
{
    m_qtMenu->addAction( item->GetHandle() );

    return item;
}


wxMenuItem *wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
}

bool wxMenu::DoDelete(wxMenuItem *item)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
}

bool wxMenu::DoDestroy(wxMenuItem *item)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return 0;
}

QMenu *wxMenu::GetHandle() const
{
    return m_qtMenu;
}

//##############################################################################

IMPLEMENT_DYNAMIC_CLASS( wxMenuBar, wxMenuBarBase )

wxMenuBar::wxMenuBar()
{
    m_qtMenuBar = new QMenuBar();
}

wxMenuBar::wxMenuBar(long style)
{
    m_qtMenuBar = new QMenuBar();
}

wxMenuBar::wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

wxMenuBar::~wxMenuBar()
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}


bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Append( menu, title ))
        return false;

    // Override the stored menu title with the given one:

    QMenu *qtMenu = menu->GetHandle();
    qtMenu->setTitle( wxQtConvertString( title ));
    m_qtMenuBar->addMenu( qtMenu );
    
    return true;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}


void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );

    return wxString();
}

QMenuBar *wxMenuBar::GetHandle() const
{
    return m_qtMenuBar;
}

