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

class wxQtAction : public QAction, public wxQtSignalHandler
{

public:
    wxQtAction( wxMenu *handler, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *menuItem );

    // Set the action shortcut to correspond to the accelerator specified by
    // the given label. They set the primary shortcut the first time they are
    // called, and set additional shortcuts/accels for subsequent calls. if
    // text is empty, any axtra accelerators will be removed.
    void UpdateShortcuts(const wxString& text);
    void UpdateShortcutsFromLabel(const wxString& text);

    wxMenu* GetMenu() const
    {
        return static_cast<wxMenu*>(wxQtSignalHandler::GetHandler());
    }

    // Convert hyphenated shortcuts to use the plus sign (+) which Qt understands.
    // Example: [ Ctrl-Shift-- ] should be converted to [ Ctrl+Shift+- ]
    static wxString Normalize(const wxString& text)
    {
        QString normalized = wxQtConvertString( text );
        normalized.replace(QRegExp("([^+-])[-](.)"), "\\1+\\2");
        return wxQtConvertString( normalized );
    }

private:
    void onActionToggled( bool checked );
    void onActionTriggered( bool checked );

    const wxWindowID m_mitemId;
    const bool m_isCheckable;
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
    m_qtAction = new wxQtAction( parentMenu, id,
                                 wxQtAction::Normalize( text ),
                                 help, kind, subMenu, this );
}



void wxMenuItem::SetItemLabel( const wxString &label )
{
    const wxString qtlabel = wxQtAction::Normalize( label );

    wxMenuItemBase::SetItemLabel( qtlabel );

    m_qtAction->UpdateShortcutsFromLabel( qtlabel );

    m_qtAction->setText( wxQtConvertString( qtlabel ));
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


void wxMenuItem::SetBitmap(const wxBitmapBundle& bitmap)
{
    if ( m_kind == wxITEM_NORMAL )
    {
        m_bitmap = bitmap;
        if ( m_bitmap.IsOk() )
        {
            m_qtAction->setIcon( QIcon(*GetBitmapFromBundle(m_bitmap).GetHandle()) );
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

#if wxUSE_ACCEL
void wxMenuItem::AddExtraAccel(const wxAcceleratorEntry& accel)
{
    wxMenuItemBase::AddExtraAccel(accel);

    m_qtAction->UpdateShortcuts( accel.ToRawString() );
}

void wxMenuItem::ClearExtraAccels()
{
    wxMenuItemBase::ClearExtraAccels();

    m_qtAction->UpdateShortcuts( wxString() );
}
#endif // wxUSE_ACCEL

//=============================================================================

wxQtAction::wxQtAction( wxMenu *handler, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *menuItem )
    : QAction( wxQtConvertString( text ), handler->GetHandle() ),
      wxQtSignalHandler( handler ),
      m_mitemId(menuItem->GetId()), m_isCheckable(menuItem->IsCheckable())
{
    setStatusTip( wxQtConvertString( help ));

    if ( subMenu != nullptr )
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

    connect( this, &QAction::toggled, this, &wxQtAction::onActionToggled );
    connect( this, &QAction::triggered, this, &wxQtAction::onActionTriggered );

    UpdateShortcutsFromLabel( text );
}

void wxQtAction::UpdateShortcutsFromLabel(const wxString& text)
{
#if wxUSE_ACCEL
    const wxString accelStr = text.AfterFirst('\t');
    if ( !accelStr.empty() )
    {
        UpdateShortcuts(accelStr);
    }
#else
    wxUnusedVar(text);
#endif // wxUSE_ACCEL
}

void wxQtAction::UpdateShortcuts(const wxString& text)
{
#if wxUSE_ACCEL
    QList<QKeySequence> shortcuts = this->shortcuts();

    if ( text.empty() )
    {
        if ( shortcuts.size() > 1 )
        {
            // Keep the primary shortcut only and get rid of the rest
            setShortcut( shortcuts.first() );
        }
    }
    else
    {
        QKeySequence keySequence = QKeySequence( wxQtConvertString( text ) );

        if ( !shortcuts.contains(keySequence) )
        {
            shortcuts.push_back(keySequence);
            setShortcuts( shortcuts );
        }
    }
#else
    wxUnusedVar(text);
#endif // wxUSE_ACCEL
}

void wxQtAction::onActionToggled( bool checked )
{
    GetMenu()->Check(m_mitemId, checked);
}

void wxQtAction::onActionTriggered( bool checked )
{
    GetMenu()->SendEvent(m_mitemId, m_isCheckable ? checked : -1 );
}
