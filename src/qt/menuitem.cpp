/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/menuitem.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/bitmap.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>

class wxQtAction : public QAction, public wxQtSignalHandler< wxMenuItem >
{

public:
    wxQtAction( wxMenu *parent, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *handler );

    // Set the action shortcut to correspond to the accelerator specified by
    // the given label.
    void UpdateShortcutsFromLabel(const wxString& text);

private:
    void onActionTriggered( bool checked );
};


wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}



wxMenuItem::wxMenuItem(wxMenu *parentMenu, int id, const wxString& text,
        const wxString& help, wxItemKind kind, wxMenu *subMenu)
    : wxMenuItemBase( parentMenu, id, text, help, kind, subMenu )
{
    m_qtAction = new wxQtAction( parentMenu, id, text, help, kind, subMenu, this );
}



void wxMenuItem::SetItemLabel( const wxString &label )
{
    wxMenuItemBase::SetItemLabel( label );

    m_qtAction->UpdateShortcutsFromLabel( label );

    m_qtAction->setText( wxQtConvertString( label ));
}



void wxMenuItem::SetCheckable( bool checkable )
{
    wxMenuItemBase::SetCheckable( checkable );

    m_qtAction->setCheckable( checkable );
}



void wxMenuItem::Enable( bool enable )
{
    wxMenuItemBase::Enable( enable );

    m_qtAction->setEnabled( enable );
}



bool wxMenuItem::IsEnabled() const
{
    bool isEnabled = m_qtAction->isEnabled();

    // Make sure the enabled stati are in synch:
    wxASSERT( isEnabled == wxMenuItemBase::IsEnabled() );

    return isEnabled;
}



void wxMenuItem::Check( bool checked )
{
    wxMenuItemBase::Check( checked );

    m_qtAction->setChecked( checked );
}



bool wxMenuItem::IsChecked() const
{
    bool isChecked = m_qtAction->isChecked();

    // Make sure the checked stati are in synch:
    wxASSERT( isChecked == wxMenuItemBase::IsChecked() );

    return isChecked;
}


void wxMenuItem::SetBitmap(const wxBitmap& bitmap)
{
    if ( m_kind == wxITEM_NORMAL )
    {
        m_bitmap = bitmap;
        if ( !m_bitmap.IsNull() )
        {
            m_qtAction->setIcon( QIcon(*m_bitmap.GetHandle()) );
        }
    }
    else
    {
        wxFAIL_MSG("only normal menu items can have bitmaps");
    }
}

void wxMenuItem::SetFont(const wxFont& font)
{
    m_qtAction->setFont(font.GetHandle());
}

QAction *wxMenuItem::GetHandle() const
{
    return m_qtAction;
}

//=============================================================================

wxQtAction::wxQtAction( wxMenu *parent, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *handler )
    : QAction( wxQtConvertString( text ), parent->GetHandle() ),
      wxQtSignalHandler< wxMenuItem >( handler )
{
    setStatusTip( wxQtConvertString( help ));

    if ( subMenu != NULL )
        setMenu( subMenu->GetHandle() );

    if ( id == wxID_SEPARATOR )
        setSeparator( true );

    switch ( kind )
    {
        case wxITEM_SEPARATOR:
            setSeparator( true );
            break;
        case wxITEM_CHECK:
        case wxITEM_RADIO:
            setCheckable( true );
            break;
        case wxITEM_NORMAL:
            // Normal for a menu item.
            break;
        case wxITEM_DROPDOWN:
        case wxITEM_MAX:
            // Not applicable for menu items.
            break;
    }

    connect( this, &QAction::triggered, this, &wxQtAction::onActionTriggered );

    UpdateShortcutsFromLabel( text );
}

void wxQtAction::UpdateShortcutsFromLabel(const wxString& text)
{
#if wxUSE_ACCEL
    const wxString accelStr = text.AfterFirst('\t');
    if ( !accelStr.empty() )
    {
        setShortcut(  QKeySequence( wxQtConvertString(accelStr) ) );
    }
#endif // wxUSE_ACCEL
}

void wxQtAction::onActionTriggered( bool checked )
{
    wxMenuItem *handler = GetHandler();
    wxMenu *menu = handler->GetMenu();
    if ( handler->IsCheckable() )
        handler->Check(checked);
    menu->SendEvent( handler->GetId(), handler->IsCheckable() ? checked : -1 );
}
