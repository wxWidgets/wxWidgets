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

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

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
    menu->m_title = title; 

    int pos;
    do 
    {
        pos = menu->m_title.First( '&' );
        if (pos != -1) menu->m_title.Remove( pos, 1 );
    } while (pos != -1);

    GtkWidget *root_menu;
    root_menu = gtk_menu_item_new_with_label( WXSTRINGCAST(menu->m_title) );
    gtk_widget_show( root_menu );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(root_menu), menu->m_menu );

    gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), root_menu );
}

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
    if (menu->m_title == menuString)
    {
        int res = menu->FindItem( itemString );
        if (res != -1) return res;
    }
    
    wxNode *node = menu->m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->IsSubMenu())
            return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);
        node = node->Next();
    }

    return -1;
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

/* Find a wxMenuItem using its id. Recurses down into sub-menus */
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
    wxMenuItem* result = menu->FindItem(id);

    wxNode *node = menu->m_items.First();
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

wxMenuItem* wxMenuBar::FindMenuItemById( int id ) const
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
    if (item) item->Check(check);
}

bool wxMenuBar::Checked( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );
    if (item) return item->IsChecked();
    return FALSE;
}

void wxMenuBar::Enable( int id, bool enable )
{
    wxMenuItem* item = FindMenuItemById( id );
    if (item) item->Enable(enable);
}

bool wxMenuBar::Enabled( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );
    if (item) return item->IsEnabled();
    return FALSE;
}

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_menu_clicked_callback( GtkWidget *widget, wxMenu *menu )
{
    int id = menu->FindMenuIdByMenuItem(widget);

    /* should find it for normal (not popup) menu */
    wxASSERT( (id != -1) || (menu->GetInvokingWindow() != NULL) ); 

    if (!menu->IsEnabled(id)) return;

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, id );
    event.SetEventObject( menu );
    event.SetInt(id );

    if (menu->m_callback)
    {
        (void) (*(menu->m_callback)) (*menu, event);
        return;
    }

    if (menu->GetEventHandler()->ProcessEvent(event)) return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win) win->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "select"
//-----------------------------------------------------------------------------

static void gtk_menu_hilight_callback( GtkWidget *widget, wxMenu *menu )
{
    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id)) return;

    wxCommandEvent event( wxEVT_MENU_HIGHLIGHT, id );
    event.SetEventObject( menu );
    event.SetInt(id );

/*   wxMSW doesn't call callback here either

     if (menu->m_callback)
     {
        (void) (*(menu->m_callback)) (*menu, event);
        return;
     }
*/

    if (menu->GetEventHandler()->ProcessEvent(event)) return;

    wxWindow *win = menu->GetInvokingWindow();
    if (win) win->GetEventHandler()->ProcessEvent( event );
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

/* it's valid for this function to be called even if m_menuItem == NULL */
void wxMenuItem::SetName( const wxString& str )
{
    m_text = "";
    for ( const char *pc = str; *pc != '\0'; pc++ )
    {
        if (*pc == '&') pc++; /* skip it */
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

    wxASSERT( bIsChecked == m_isChecked ); // consistency check

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
    m_menu = gtk_menu_new();  // Do not show!

    m_callback = func;
    m_eventHandler = this;
    m_clientData = (void*) NULL;

    if (m_title.IsNull()) m_title = "";
    if (m_title != "")
    {
        Append(-2, m_title);
        AppendSeparator();
    }
}

void wxMenu::SetTitle( const wxString& title )
{
    /* Waiting for something better. */
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
    GtkWidget *menuItem = checkable ? gtk_check_menu_item_new_with_label(text)
                                    : gtk_menu_item_new_with_label(text);

    mitem->SetMenuItem(menuItem);

    gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                        GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                        (gpointer*)this );

    gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                        GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                        (gpointer*)this );

    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    m_items.Append( mitem );
}

void wxMenu::Append( int id, const wxString &text, wxMenu *subMenu, const wxString &helpStr )
{
    wxMenuItem *mitem = new wxMenuItem();
    mitem->SetId(id);
    mitem->SetText(text);

    GtkWidget *menuItem = gtk_menu_item_new_with_label(mitem->GetText());
    mitem->SetHelp(helpStr);
    mitem->SetMenuItem(menuItem);
    mitem->SetSubMenu(subMenu);

    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), subMenu->m_menu );
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
    m_items.Append( mitem );
}

int wxMenu::FindItem( const wxString itemString ) const
{
    wxString s( itemString );

    int pos;
    do 
    {
        pos = s.First( '&' );
        if (pos != -1) s.Remove( pos, 1 );
    } while (pos != -1);

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

    return -1;
}

void wxMenu::Enable( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);
    if (item)
    { 
        item->Enable(enable);
    }
}

bool wxMenu::IsEnabled( int id ) const
{
    wxMenuItem *item = FindItem(id);
    if (item)
    {
        return item->IsEnabled();
    }
    else
    {
        return FALSE;
    }
}

void wxMenu::Check( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);
    if (item)
    {
        item->Check(enable);
    }
}

bool wxMenu::IsChecked( int id ) const
{
    wxMenuItem *item = FindItem(id);
    if (item)
    {
        return item->IsChecked();
    }
    else
    {
        return FALSE;
    }
}

void wxMenu::SetLabel( int id, const wxString &label )
{
    wxMenuItem *item = FindItem(id);
    if (item)
    {
        item->SetText(label);
    }
}

wxString wxMenu::GetLabel( int id ) const
{
    wxMenuItem *item = FindItem(id);
    if (item)
    { 
        return item->GetText();
    }
    else
    {
        return "";
    }
}

void wxMenu::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem *item = FindItem(id);
    if (item) item->SetHelp( helpString );
}

wxString wxMenu::GetHelpString( int id ) const
{
    wxMenuItem *item = FindItem(id);
    if (item)
    { 
        return item->GetHelp();
    }
    else
    {
        return "";
    }
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

    return -1;
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


