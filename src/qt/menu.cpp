/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/menu.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menu.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include "wx/stockitem.h"

#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

static void ApplyStyle( QMenu *qtMenu, long style )
{
    if ( style & wxMENU_TEAROFF )
        qtMenu->setTearOffEnabled( true );
}

// wxQtActionGroup: an exclusive group which synchronizes QActions in
// QActionGroup with their wx wrappers.
class wxQtActionGroup : public QActionGroup
{

public:
    explicit wxQtActionGroup( wxMenu* handler )
        : QActionGroup( handler->GetHandle() )
    {
        setExclusive(true);

        connect( this, &QActionGroup::triggered, this, &wxQtActionGroup::triggered );
    }

    void AddAction( QAction* action )
    {
        m_activeAction = QActionGroup::addAction(action);
    }

private:
    void triggered ( QAction* action )
    {
        if ( action != m_activeAction )
        {
            if ( m_activeAction->isCheckable() )
                m_activeAction->setChecked(false);

            m_activeAction = action;
        }
    }

    QAction* m_activeAction;
};

//-----------------------------------------------------------------------------
// wxMenu implementation
//-----------------------------------------------------------------------------

wxMenu::wxMenu(long style)
    : wxMenuBase( style )
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



static wxMenuItem *GetMenuItemAt( const wxMenu *menu, size_t position )
{
    // FindItemByPosition() is doing the same test, but we want to prevent
    // the warning message it prints when an illegal index is used.

    if ( position < menu->GetMenuItemCount() )
        return menu->FindItemByPosition( position );
    else
        return nullptr;
}

static void AddItemActionToGroup( const wxMenuItem *groupItem, QAction *itemAction )
{
    QAction *groupItemAction = groupItem->GetHandle();
    QActionGroup *itemActionGroup = groupItemAction->actionGroup();
    wxASSERT_MSG( itemActionGroup != nullptr, "An action group should have been setup" );
    itemActionGroup->addAction( itemAction );
}

static void InsertMenuItemAction( const wxMenu *menu, const wxMenuItem *previousItem,
    wxMenuItem *item, const wxMenuItem *successiveItem )
{
    QMenu *qtMenu = menu->GetHandle();
    QAction *itemAction = item->GetHandle();
    switch ( item->GetKind() )
    {
        case wxITEM_RADIO:
            // If a neighbouring menu item is a radio item then add this item to the
            // same action group, otherwise start a new group:

            if ( previousItem != nullptr && previousItem->GetKind() == wxITEM_RADIO )
            {
                AddItemActionToGroup( previousItem, itemAction );
            }
            else if ( successiveItem != nullptr && successiveItem->GetKind() == wxITEM_RADIO )
            {
                AddItemActionToGroup( successiveItem, itemAction );
            }
            else
            {
                auto actionGroup = new wxQtActionGroup( const_cast<wxMenu*>(menu) );
                actionGroup->AddAction( itemAction );
                item->Check();
                wxASSERT_MSG( itemAction->actionGroup() == actionGroup, "Must be the same action group" );
            }
            break;
        case wxITEM_NORMAL:
        {
            // If the inserted action is a submenu, set the owner for the submenu.
            if ( item->IsSubMenu() )
            {
                item->GetSubMenu()->GetHandle()->setParent(qtMenu, Qt::Popup);
            }

            wxWindowID id = item->GetId();
            if ( wxIsStockID( id ) )
            {
                itemAction->setText( wxQtConvertString( wxGetStockLabel( id ) ) );
#if wxUSE_ACCEL
                wxAcceleratorEntry accel = wxGetStockAccelerator( id );
                QString shortcut;
                if ( id == wxID_EXIT )
                {
                    shortcut = QStringLiteral("Ctrl+Q");
                }
                else if ( accel.IsOk() )
                {
                    shortcut = wxQtConvertString( accel.ToRawString() );
                }
                if ( !shortcut.isEmpty() )
                {
                    itemAction->setShortcut( QKeySequence( shortcut ) );
                }
#endif // wxUSE_ACCEL
            }
            break;
        }
        default:
            break;
    }
    // Insert the action into the actual menu:
    QAction *successiveItemAction = ( successiveItem != nullptr ) ? successiveItem->GetHandle() : nullptr;
    qtMenu->insertAction( successiveItemAction, itemAction );
    // Menu items in Qt can be part of multiple menus, so a menu will not take ownership
    // when one is added to it. Take it explicitly, otherwise it will create a memory leak.
    itemAction->setParent(qtMenu);
}

wxMenuItem *wxMenu::DoAppend(wxMenuItem *item)
{
    // Get the previous/successive items *before* we call the base class methods,
    // because afterwards it is less clear where these items end up:

    wxMenuItem *previousItem = GetMenuItemAt( this, GetMenuItemCount() - 1 );
    wxMenuItem *successiveItem = GetMenuItemAt( this, GetMenuItemCount() );

    if ( wxMenuBase::DoAppend( item ) == nullptr )
        return nullptr;

    item->QtCreateAction( this );

    InsertMenuItemAction( this, previousItem, item, successiveItem );

    return item;
}


wxMenuItem *wxMenu::DoInsert(size_t insertPosition, wxMenuItem *item)
{
    // Get the previous/successive items *before* we call the base class methods,
    // because afterwards it is less clear where these items end up:

    wxMenuItem *previousItem = GetMenuItemAt( this, insertPosition - 1 );
    wxMenuItem *successiveItem = GetMenuItemAt( this, insertPosition );

    if ( wxMenuBase::DoInsert( insertPosition, item ) == nullptr )
        return nullptr;

    item->QtCreateAction( this );

    InsertMenuItemAction( this, previousItem, item, successiveItem );

    return item;
}


wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    if ( wxMenuBase::DoRemove( item ) == nullptr )
        return nullptr;

    m_qtMenu->removeAction( item->GetHandle() );

    return item;
}


QMenu *wxMenu::GetHandle() const
{
    return m_qtMenu;
}


//##############################################################################

wxMenuBar::wxMenuBar()
{
    m_qtWindow = new QMenuBar();

    wxMenuBarBase::Create(nullptr, wxID_ANY);
}

wxMenuBar::wxMenuBar( long style )
{
    m_qtWindow = new QMenuBar();

    wxMenuBarBase::Create(nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long style)
{
    m_qtWindow = new QMenuBar();

    for ( size_t i = 0; i < count; ++i )
    {
        Append( menus[ i ], titles[ i ] );
    }

    wxMenuBarBase::Create(nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
}

QMenuBar* wxMenuBar::GetQMenuBar() const
{
    return static_cast<QMenuBar*>(m_qtWindow);
}

static QMenu *SetTitle( wxMenu *menu, const wxString &title )
{
    menu->SetTitle(title);

    QMenu *qtMenu = menu->GetHandle();
    qtMenu->setTitle( wxQtConvertString( title ));

    return qtMenu;
}


bool wxMenuBar::Append( wxMenu *menu, const wxString& title )
{
    if ( !wxMenuBarBase::Append( menu, title ))
        return false;

    // Override the stored menu title with the given one:

    QMenu *qtMenu = SetTitle( menu, title );
    GetQMenuBar()->addMenu( qtMenu );
    // Menus in Qt can be reused as popups, so a menu bar will not take ownership when
    // one is added to it. Take it explicitly, otherwise there will be a memory leak.
    qtMenu->setParent(GetQMenuBar(), Qt::Popup); // must specify window type for correct display!

    return true;
}


static QAction *GetActionAt( const QWidget *qtWidget, size_t pos )
{
    QList< QAction * > actions = qtWidget->actions();
    return pos < static_cast< unsigned >( actions.size() ) ? actions.at( pos ) : nullptr;
}


bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert( pos, menu, title ))
        return false;

    // Override the stored menu title with the given one:

    QMenu *qtMenu = SetTitle( menu, title );
    QAction *qtAction = GetActionAt( GetQMenuBar(), pos );
    GetQMenuBar()->insertMenu( qtAction, qtMenu );
    qtMenu->setParent(GetQMenuBar(), Qt::Popup); // must specify window type for correct display!

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu;

    if (( menu = wxMenuBarBase::Remove( pos )) == nullptr )
        return nullptr;

    GetQMenuBar()->removeAction( GetActionAt( GetQMenuBar(), pos ));
    return menu;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    QAction *qtAction = GetActionAt( GetQMenuBar(), pos );
    qtAction->setEnabled( enable );
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    QAction *qtAction = GetActionAt( GetQMenuBar(), pos );
    return qtAction->isEnabled();
}


void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    QAction *qtAction = GetActionAt( GetQMenuBar(), pos );
    QMenu *qtMenu = qtAction->menu();
    qtMenu->setTitle( wxQtConvertString( label ));
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    QAction *qtAction = GetActionAt( GetQMenuBar(), pos );
    QMenu *qtMenu = qtAction->menu();

    return wxQtConvertString( qtMenu->title() );
}

void wxMenuBar::Attach(wxFrame *frame)
{
    // sanity check as setMenuBar takes ownership
    wxCHECK_RET( GetHandle(), "Menu bar has been previously deleted by Qt");
    wxMenuBarBase::Attach(frame);
}

void wxMenuBar::Detach()
{
    // the QMenuBar probably was deleted by Qt as setMenuBar takes ownership
    m_qtWindow = nullptr;
    wxMenuBarBase::Detach();
}
