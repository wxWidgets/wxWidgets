/////////////////////////////////////////////////////////////////////////////
// Name:        menu.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "menu.h"
#pragma implementation "menuitem.h"
#endif

#include "wx/menu.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/app.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar( long style )
{
    m_needParent = FALSE; // hmmm

    PreCreation( (wxWindow *) NULL, -1, wxDefaultPosition, wxDefaultSize, 0, "menu" );

    m_menus.DeleteContents( TRUE );

    m_menubar = gtk_menu_bar_new();

    if (style & wxMB_DOCKABLE)
    {
        m_widget = gtk_handle_box_new();
        gtk_container_add( GTK_CONTAINER(m_widget), GTK_WIDGET(m_menubar) );
        gtk_widget_show( GTK_WIDGET(m_menubar) );
    }
    else
    {
        m_widget = GTK_WIDGET(m_menubar);
    }

    PostCreation();

    Show( TRUE );
}

wxMenuBar::wxMenuBar()
{
    m_needParent = FALSE; // hmmm

    PreCreation( (wxWindow *) NULL, -1, wxDefaultPosition, wxDefaultSize, 0, "menu" );

    m_menus.DeleteContents( TRUE );

    m_menubar = gtk_menu_bar_new();

    m_widget = GTK_WIDGET(m_menubar);

    PostCreation();

    Show( TRUE );
}

void wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
    m_menus.Append( menu );
    
    wxString s = "";
    for ( const char *pc = title; *pc != '\0'; pc++ )
    {
        if (*pc == '&')
	{
	    pc++; /* skip it */
#if (GTK_MINOR_VERSION > 0)
    //      s << '_';  not yet
#endif
        }
        s << *pc;
    }

    menu->SetTitle(s);
    menu->m_owner = gtk_menu_item_new_with_label( WXSTRINGCAST(s) );
    gtk_widget_show( menu->m_owner );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );

    gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), menu->m_owner );
}

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
    if (menu->GetTitle() == menuString)
    {
        int res = menu->FindItem( itemString );
        if (res != wxNOT_FOUND)
            return res;
    }

    wxNode *node = ((wxMenu *)menu)->GetItems().First();    // const_cast
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->IsSubMenu())
            return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);

        node = node->Next();
    }

    return wxNOT_FOUND;
}

wxMenuItem *wxMenuBar::FindItemForId(int itemId, wxMenu **menuForItem = NULL) const
{
    if ( menuForItem )
    {
        // TODO return the pointer to the menu

        *menuForItem = NULL;
    }

    return FindItem(itemId);
}

int wxMenuBar::FindMenuItem( const wxString &menuString, const wxString &itemString ) const
{
    wxNode *node = m_menus.First();
    while (node)
    {
        wxMenu *menu = (wxMenu*)node->Data();
        int res = FindMenuItemRecursive( menu, menuString, itemString);
        if (res != -1) return res;
        node = node->Next();
    }
    return -1;
}

// Find a wxMenuItem using its id. Recurses down into sub-menus
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
    wxMenuItem* result = menu->FindItem(id);

    wxNode *node = ((wxMenu *)menu)->GetItems().First(); // const_cast
    while ( node && result == NULL )
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->IsSubMenu())
        {
            result = FindMenuItemByIdRecursive( item->GetSubMenu(), id );
        }
        node = node->Next();
    }

    return result;
}

wxMenuItem* wxMenuBar::FindItem( int id ) const
{
    wxMenuItem* result = 0;
    wxNode *node = m_menus.First();
    while (node && result == 0)
    {
        wxMenu *menu = (wxMenu*)node->Data();
        result = FindMenuItemByIdRecursive( menu, id );
        node = node->Next();
    }

    return result;
}

void wxMenuBar::Check( int id, bool check )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, "wxMenuBar::Check: no such item" );

    item->Check(check);
}

bool wxMenuBar::IsChecked( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, FALSE, "wxMenuBar::IsChecked: no such item" );

    return item->IsChecked();
}

void wxMenuBar::Enable( int id, bool enable )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, "wxMenuBar::Enable: no such item" );

    item->Enable(enable);
}

bool wxMenuBar::IsEnabled( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, FALSE, "wxMenuBar::IsEnabled: no such item" );

    return item->IsEnabled();
}

wxString wxMenuBar::GetLabel( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, "", "wxMenuBar::GetLabel: no such item" );

    return item->GetText();
}

void wxMenuBar::SetLabel( int id, const wxString &label )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, "wxMenuBar::SetLabel: no such item" );

    item->SetText( label );
}

void wxMenuBar::EnableTop( int pos, bool flag )
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_RET( node, "menu not found" );

    wxMenu* menu = (wxMenu*)node->Data();

    if (menu->m_owner)
        gtk_widget_set_sensitive( menu->m_owner, flag );
}

wxString wxMenuBar::GetLabelTop( int pos ) const
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_MSG( node, "invalid", "menu not found" );

    wxMenu* menu = (wxMenu*)node->Data();

    return menu->GetTitle();
}

void wxMenuBar::SetLabelTop( int pos, const wxString& label )
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_RET( node, "menu not found" );

    wxMenu* menu = (wxMenu*)node->Data();

    menu->SetTitle( label );
}

void wxMenuBar::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, "wxMenuBar::SetHelpString: no such item" );

    item->SetHelp( helpString );
}

wxString wxMenuBar::GetHelpString( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, "", "wxMenuBar::GetHelpString: no such item" );

    return item->GetHelp();
}

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_menu_clicked_callback( GtkWidget *widget, wxMenu *menu )
{
    int id = menu->FindMenuIdByMenuItem(widget);

    /* should find it for normal (not popup) menu */
    wxASSERT( (id != -1) || (menu->GetInvokingWindow() != NULL) );

    if (!menu->IsEnabled(id))
        return;

    wxMenuItem* item = menu->FindItem( id );
    wxCHECK_RET( item, "error in menu item callback" );

    if (item->IsCheckable())
    {
        if (item->GetCheckedFlag() == item->IsChecked())
        {
            /* the menu item has been checked by calling wxMenuItem->Check() */
            return;
        }
        else
        {
            /* the user pressed on the menu item -> report */
            item->SetCheckedFlag(item->IsChecked());  /* make consistent again */
        }
    }

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, id );
    event.SetEventObject( menu );
    event.SetInt(id );

    if (menu->GetCallback())
    {
        (void) (*(menu->GetCallback())) (*menu, event);
        return;
    }

    if (menu->GetEventHandler()->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win)
        win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "select"
//-----------------------------------------------------------------------------

static void gtk_menu_hilight_callback( GtkWidget *widget, wxMenu *menu )
{
    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id))
        return;

    wxMenuEvent event( wxEVT_MENU_HIGHLIGHT, id );
    event.SetEventObject( menu );

/*   wxMSW doesn't call callback here either

     if (menu->m_callback)
     {
        (void) (*(menu->m_callback)) (*menu, event);
        return;
     }
*/

    if (menu->GetEventHandler()->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win) win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "deselect"
//-----------------------------------------------------------------------------

static void gtk_menu_nolight_callback( GtkWidget *widget, wxMenu *menu )
{
    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id))
        return;

    wxMenuEvent event( wxEVT_MENU_HIGHLIGHT, -1 );
    event.SetEventObject( menu );

    if (menu->GetEventHandler()->ProcessEvent(event))
        return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win)
        win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem,wxObject)

wxMenuItem::wxMenuItem()
{
    m_id = ID_SEPARATOR;
    m_isCheckMenu = FALSE;
    m_isChecked = FALSE;
    m_isEnabled = TRUE;
    m_subMenu = (wxMenu *) NULL;
    m_menuItem = (GtkWidget *) NULL;
}

// it's valid for this function to be called even if m_menuItem == NULL
void wxMenuItem::SetName( const wxString& str )
{
    m_text = "";
    for ( const char *pc = str; *pc != '\0'; pc++ )
    {
        if (*pc == '&')
	{
	    pc++; /* skip it */
#if (GTK_MINOR_VERSION > 0)
            m_text << '_';
#endif
        }
        m_text << *pc;
    }

    if (m_menuItem)
    {
        GtkLabel *label = GTK_LABEL( GTK_BIN(m_menuItem)->child );
        gtk_label_set( label, m_text.c_str());
    }
}

void wxMenuItem::Check( bool check )
{
    wxCHECK_RET( m_menuItem, "invalid menu item" );

    wxCHECK_RET( IsCheckable(), "Can't check uncheckable item!" )

    if (check == m_isChecked) return;

    m_isChecked = check;
    gtk_check_menu_item_set_state( (GtkCheckMenuItem*)m_menuItem, (gint)check );
}

void wxMenuItem::Enable( bool enable )
{
    wxCHECK_RET( m_menuItem, "invalid menu item" );

    gtk_widget_set_sensitive( m_menuItem, enable );
    m_isEnabled = enable;
}

bool wxMenuItem::IsChecked() const
{
    wxCHECK_MSG( m_menuItem, FALSE, "invalid menu item" );

    wxCHECK( IsCheckable(), FALSE ); // can't get state of uncheckable item!

    bool bIsChecked = ((GtkCheckMenuItem*)m_menuItem)->active != 0;

    return bIsChecked;
}

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

wxMenu::wxMenu( const wxString& title, const wxFunction func )
{
    m_title = title;
    m_items.DeleteContents( TRUE );
    m_invokingWindow = (wxWindow *) NULL;
    
#if (GTK_MINOR_VERSION > 0)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU, "<main>", m_accel );
    m_menu = gtk_item_factory_get_widget( m_factory, "<main>" );
#else    
    m_menu = gtk_menu_new();  // Do not show!
#endif

    m_callback = func;
    m_eventHandler = this;
    m_clientData = (void*) NULL;

    if (m_title.IsNull()) m_title = "";
    if (m_title != "")
    {
        Append(-2, m_title);
        AppendSeparator();
    }

    m_owner = (GtkWidget*) NULL;
}

wxMenu::~wxMenu()
{
    /* how do we delete an item-factory ? */
}

void wxMenu::SetTitle( const wxString& title )
{
    // TODO Waiting for something better
    m_title = title;
}

const wxString wxMenu::GetTitle() const
{
    return m_title;
}

void wxMenu::AppendSeparator()
{
    wxMenuItem *mitem = new wxMenuItem();
    mitem->SetId(ID_SEPARATOR);

    GtkWidget *menuItem = gtk_menu_item_new();
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    
    mitem->SetMenuItem(menuItem);
    m_items.Append( mitem );
}

void wxMenu::Append( int id, const wxString &item, const wxString &helpStr, bool checkable )
{
    wxMenuItem *mitem = new wxMenuItem();
    mitem->SetId(id);
    mitem->SetText(item);
    mitem->SetHelp(helpStr);
    mitem->SetCheckable(checkable);
    const char *text = mitem->GetText();
    
#if (GTK_MINOR_VERSION > 0)
    char buf[100];
    strcpy( buf, "/" );
    strcat( buf, text );
    
    GtkItemFactoryEntry entry;
    entry.path = buf;
    entry.accelerator = (gchar*) NULL;
    entry.callback = (GtkItemFactoryCallback) gtk_menu_clicked_callback;
    entry.callback_action = 0;
    if (checkable)
        entry.item_type = "<CheckItem>";
    else
        entry.item_type = "<Item>";
    
    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */
    
    /* in order to get the pointer to the item we need the item text _without_ underscores */
    wxString s = "<main>/";
    for ( const char *pc = text; *pc != '\0'; pc++ )
    {
        if (*pc == '_') pc++; /* skip it */
        s << *pc;
    }
    
    GtkWidget *menuItem = gtk_item_factory_get_widget( m_factory, s );
    
#else

    GtkWidget *menuItem = checkable ? gtk_check_menu_item_new_with_label(text)
                                    : gtk_menu_item_new_with_label(text);
				    
    gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                        GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                        (gpointer)this );
			
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    
#endif

    gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                        GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                        (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                        GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                        (gpointer)this );

    mitem->SetMenuItem(menuItem);

    m_items.Append( mitem );
}

void wxMenu::Append( int id, const wxString &text, wxMenu *subMenu, const wxString &helpStr )
{
    wxMenuItem *mitem = new wxMenuItem();
    mitem->SetId(id);
    mitem->SetText(text);
    mitem->SetHelp(helpStr);

    GtkWidget *menuItem = gtk_menu_item_new_with_label(mitem->GetText());
    mitem->SetMenuItem(menuItem);
    mitem->SetSubMenu(subMenu);

    gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                        GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                        (gpointer*)this );

    gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                        GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                        (gpointer*)this );

    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), subMenu->m_menu );
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    m_items.Append( mitem );
}

void wxMenu::Append( wxMenuItem *item )
{
    m_items.Append( item );

    GtkWidget *menuItem = (GtkWidget*) NULL;

    if (item->IsSeparator())
        menuItem = gtk_menu_item_new();
    else if (item->IsSubMenu())
        menuItem = gtk_menu_item_new_with_label(item->GetText());
    else
        menuItem = item->IsCheckable() ? gtk_check_menu_item_new_with_label(item->GetText())
                                       : gtk_menu_item_new_with_label(item->GetText());

    if (!item->IsSeparator())
    {
        gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                            GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                            (gpointer*)this );

        gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                            GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                            (gpointer*)this );

        if (!item->IsSubMenu())
        {
            gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                                GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                                (gpointer*)this );
        }
    }

    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    item->SetMenuItem(menuItem);
}

int wxMenu::FindItem( const wxString itemString ) const
{
    wxString s = "";
    for ( const char *pc = itemString; *pc != '\0'; pc++ )
    {
        if (*pc == '&')
	{
	    pc++; /* skip it */
#if (GTK_MINOR_VERSION > 0)
            s << '_';
#endif
        }
        s << *pc;
    }

    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->GetText() == s)
        {
            return item->GetId();
        }
        node = node->Next();
    }

    return wxNOT_FOUND;
}

void wxMenu::Enable( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, "wxMenu::Enable: no such item" );

    item->Enable(enable);
}

bool wxMenu::IsEnabled( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, "wxMenu::IsEnabled: no such item" );

    return item->IsEnabled();
}

void wxMenu::Check( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, "wxMenu::Check: no such item" );

    item->Check(enable);
}

bool wxMenu::IsChecked( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, "wxMenu::IsChecked: no such item" );

    return item->IsChecked();
}

void wxMenu::SetLabel( int id, const wxString &label )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, "wxMenu::SetLabel: no such item" );

    item->SetText(label);
}

wxString wxMenu::GetLabel( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, "", "wxMenu::GetLabel: no such item" );

    return item->GetText();
}

void wxMenu::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, "wxMenu::SetHelpString: no such item" );

    item->SetHelp( helpString );
}

wxString wxMenu::GetHelpString( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, "", "wxMenu::GetHelpString: no such item" );

    return item->GetHelp();
}

int wxMenu::FindMenuIdByMenuItem( GtkWidget *menuItem ) const
{
    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->GetMenuItem() == menuItem)
           return item->GetId();
        node = node->Next();
    }

    return wxNOT_FOUND;
}

wxMenuItem *wxMenu::FindItem(int id) const
{
    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->GetId() == id)
        {
            return item;
        }
        node = node->Next();
    }

    /* Not finding anything here can be correct
     * when search the entire menu system for
     * an entry -> no error message. */

    return (wxMenuItem *) NULL;
}

void wxMenu::SetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = win;
}

wxWindow *wxMenu::GetInvokingWindow()
{
    return m_invokingWindow;
}

// Update a menu and all submenus recursively. source is the object that has
// the update event handlers defined for it. If NULL, the menu or associated
// window will be used.
void wxMenu::UpdateUI(wxEvtHandler* source)
{
  if (!source && GetInvokingWindow())
    source = GetInvokingWindow()->GetEventHandler();
  if (!source)
    source = GetEventHandler();
  if (!source)
    source = this;

  wxNode* node = GetItems().First();
  while (node)
  {
    wxMenuItem* item = (wxMenuItem*) node->Data();
    if ( !item->IsSeparator() )
    {
      wxWindowID id = item->GetId();
      wxUpdateUIEvent event(id);
      event.SetEventObject( source );

      if (source->ProcessEvent(event))
      {
        if (event.GetSetText())
          SetLabel(id, event.GetText());
        if (event.GetSetChecked())
          Check(id, event.GetChecked());
        if (event.GetSetEnabled())
          Enable(id, event.GetEnabled());
      }

      if (item->GetSubMenu())
        item->GetSubMenu()->UpdateUI(source);
    }
    node = node->Next();
  }
}

