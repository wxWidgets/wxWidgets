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

#include "wx/log.h"
#include "wx/intl.h"
#include "wx/app.h"
#include "wx/menu.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include <gdk/gdk.h>
#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

#if (GTK_MINOR_VERSION > 0) && wxUSE_ACCEL
static wxString GetHotKey( const wxMenuItem& item );
#endif

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuBar,wxWindow)

wxMenuBar::wxMenuBar( long style )
{
    /* the parent window is known after wxFrame::SetMenu() */
    m_needParent = FALSE;
    m_style = style;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menus.DeleteContents( TRUE );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", m_accel );
    m_menubar = gtk_item_factory_get_widget( m_factory, "<main>" );
#else
    m_menubar = gtk_menu_bar_new();
#endif

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

    ApplyWidgetStyle();
}

wxMenuBar::wxMenuBar()
{
    /* the parent window is known after wxFrame::SetMenu() */
    m_needParent = FALSE;
    m_style = 0;
    m_invokingWindow = (wxWindow*) NULL;

    if (!PreCreation( (wxWindow*) NULL, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( (wxWindow*) NULL, -1, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    m_menus.DeleteContents( TRUE );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<main>", m_accel );
    m_menubar = gtk_item_factory_get_widget( m_factory, "<main>" );
#else
    m_menubar = gtk_menu_bar_new();
#endif

    m_widget = GTK_WIDGET(m_menubar);

    PostCreation();

    ApplyWidgetStyle();
}

wxMenuBar::~wxMenuBar()
{
//    gtk_object_unref( GTK_OBJECT(m_factory) );  why not ?
}

static void wxMenubarUnsetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( (wxWindow*) NULL );

#if (GTK_MINOR_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native hot keys  */
    gtk_accel_group_detach( menu->m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarUnsetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->GetNext();
    }
}

static void wxMenubarSetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( win );

#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native hot keys  */
    GtkObject *obj = GTK_OBJECT(top_frame->m_widget);
    if ( !g_slist_find( menu->m_accel->attach_objects, obj ) )
        gtk_accel_group_attach( menu->m_accel, obj );
#endif

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *menuitem = node->GetData();
        if (menuitem->IsSubMenu())
            wxMenubarSetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->GetNext();
    }
}

void wxMenuBar::SetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = win;
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    GtkObject *obj = GTK_OBJECT(top_frame->m_widget);
    if ( !g_slist_find( m_accel->attach_objects, obj ) )
        gtk_accel_group_attach( m_accel, obj );
#endif

    wxMenuList::Node *node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        wxMenubarSetInvokingWindow( menu, win );
        node = node->GetNext();
    }
}

void wxMenuBar::UnsetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = (wxWindow*) NULL;
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    gtk_accel_group_detach( m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxMenuList::Node *node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        wxMenubarUnsetInvokingWindow( menu, win );
        node = node->GetNext();
    }
}

bool wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
    if ( !wxMenuBarBase::Append( menu, title ) )
        return FALSE;

    return GtkAppend(menu, title);
}

bool wxMenuBar::GtkAppend(wxMenu *menu, const wxString& title)
{
    const wxChar *pc;

    /* GTK 1.2 wants to have "_" instead of "&" for accelerators */
    wxString str;
    for ( pc = title; *pc != wxT('\0'); pc++ )
    {
        if (*pc == wxT('&'))
        {
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
            str << wxT('_');
        }
        else if (*pc == wxT('/'))
        {
            str << wxT('\\');
#endif
        }
        else
        {
#if __WXGTK12__
            if ( *pc == wxT('_') )
            {
                // underscores must be doubled to prevent them from being
                // interpreted as accelerator character prefix by GTK
                str << *pc;
            }
#endif // GTK+ 1.2

            str << *pc;
        }
    }

    /* this doesn't have much effect right now */
    menu->SetTitle( str );

    /* GTK 1.2.0 doesn't have gtk_item_factory_get_item(), but GTK 1.2.1 has. */
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)

    /* local buffer in multibyte form */
    wxString buf;
    buf << wxT('/') << str.c_str();

    char *cbuf = new char[buf.Length()+1];
    strcpy(cbuf, buf.mbc_str());

    GtkItemFactoryEntry entry;
    entry.path = (gchar *)cbuf;  // const_cast
    entry.accelerator = (gchar*) NULL;
    entry.callback = (GtkItemFactoryCallback) NULL;
    entry.callback_action = 0;
    entry.item_type = "<Branch>";

    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */
    /* in order to get the pointer to the item we need the item text _without_ underscores */
    wxString tmp = wxT("<main>/");
    for ( pc = str; *pc != wxT('\0'); pc++ )
    {
       // contrary to the common sense, we must throw out _all_ underscores,
       // (i.e. "Hello__World" => "HelloWorld" and not "Hello_World" as we
       // might naively think). IMHO it's a bug in GTK+ (VZ)
       while (*pc == wxT('_'))
           pc++;
       tmp << *pc;
    }
    menu->m_owner = gtk_item_factory_get_item( m_factory, tmp.mb_str() );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );
    delete [] cbuf;
#else

    menu->m_owner = gtk_menu_item_new_with_label( str.mb_str() );
    gtk_widget_show( menu->m_owner );
    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu->m_owner), menu->m_menu );

    gtk_menu_bar_append( GTK_MENU_BAR(m_menubar), menu->m_owner );

#endif

    // m_invokingWindow is set after wxFrame::SetMenuBar(). This call enables
    // adding menu later on.
    if (m_invokingWindow)
        wxMenubarSetInvokingWindow( menu, m_invokingWindow );

    return TRUE;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert(pos, menu, title) )
        return FALSE;

#if __WXGTK12__
    // GTK+ doesn't have a function to insert a menu using GtkItemFactory (as
    // of version 1.2.6), so we first append the item and then change its
    // index
    if ( !GtkAppend(menu, title) )
        return FALSE;

    if (pos+1 >= m_menus.GetCount())
        return TRUE;

    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);
    gpointer data = g_list_last(menu_shell->children)->data;
    menu_shell->children = g_list_remove(menu_shell->children, data);
    menu_shell->children = g_list_insert(menu_shell->children, data, pos);

    return TRUE;
#else  // GTK < 1.2
    // this should be easy to do with GTK 1.0 - can use standard functions for
    // this and don't need any hacks like above, but as I don't have GTK 1.0
    // any more I can't do it
    wxFAIL_MSG( wxT("TODO") );

    return FALSE;
#endif // GTK 1.2/1.0
}

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
    // remove the old item and insert a new one
    wxMenu *menuOld = Remove(pos);
    if ( menuOld && !Insert(pos, menu, title) )
    {
        return (wxMenu*) NULL;
    }

    // either Insert() succeeded or Remove() failed and menuOld is NULL
    return menuOld;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return (wxMenu*) NULL;

/*
    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);

    printf( "factory entries before %d\n", (int)g_slist_length(m_factory->items) );
    printf( "menu shell entries before %d\n", (int)g_list_length( menu_shell->children ) );
*/

    // unparent calls unref() and that would delete the widget so we raise
    // the ref count to 2 artificially before invoking unparent.
    gtk_widget_ref( menu->m_menu );
    gtk_widget_unparent( menu->m_menu );

    gtk_widget_destroy( menu->m_owner );

/*
    printf( "factory entries after %d\n", (int)g_slist_length(m_factory->items) );
    printf( "menu shell entries after %d\n", (int)g_list_length( menu_shell->children ) );
*/

    return menu;
}

static int FindMenuItemRecursive( const wxMenu *menu, const wxString &menuString, const wxString &itemString )
{
    if (menu->GetTitle() == menuString)
    {
        int res = menu->FindItem( itemString );
        if (res != wxNOT_FOUND)
            return res;
    }

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem *item = node->GetData();
        if (item->IsSubMenu())
            return FindMenuItemRecursive(item->GetSubMenu(), menuString, itemString);

        node = node->GetNext();
    }

    return wxNOT_FOUND;
}

int wxMenuBar::FindMenuItem( const wxString &menuString, const wxString &itemString ) const
{
    wxMenuList::Node *node = m_menus.GetFirst();
    while (node)
    {
        wxMenu *menu = node->GetData();
        int res = FindMenuItemRecursive( menu, menuString, itemString);
        if (res != -1)
            return res;
        node = node->GetNext();
    }

    return wxNOT_FOUND;
}

// Find a wxMenuItem using its id. Recurses down into sub-menus
static wxMenuItem* FindMenuItemByIdRecursive(const wxMenu* menu, int id)
{
    wxMenuItem* result = menu->FindChildItem(id);

    wxMenuItemList::Node *node = menu->GetMenuItems().GetFirst();
    while ( node && result == NULL )
    {
        wxMenuItem *item = node->GetData();
        if (item->IsSubMenu())
        {
            result = FindMenuItemByIdRecursive( item->GetSubMenu(), id );
        }
        node = node->GetNext();
    }

    return result;
}

wxMenuItem* wxMenuBar::FindItem( int id, wxMenu **menuForItem ) const
{
    wxMenuItem* result = 0;
    wxMenuList::Node *node = m_menus.GetFirst();
    while (node && result == 0)
    {
        wxMenu *menu = node->GetData();
        result = FindMenuItemByIdRecursive( menu, id );
        node = node->GetNext();
    }

    if ( menuForItem )
    {
        *menuForItem = result ? result->GetMenu() : (wxMenu *)NULL;
    }

    return result;
}

void wxMenuBar::EnableTop( size_t pos, bool flag )
{
    wxMenuList::Node *node = m_menus.Item( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = node->GetData();

    if (menu->m_owner)
        gtk_widget_set_sensitive( menu->m_owner, flag );
}

wxString wxMenuBar::GetLabelTop( size_t pos ) const
{
    wxMenuList::Node *node = m_menus.Item( pos );

    wxCHECK_MSG( node, wxT("invalid"), wxT("menu not found") );

    wxMenu* menu = node->GetData();

    return menu->GetTitle();
}

void wxMenuBar::SetLabelTop( size_t pos, const wxString& label )
{
    wxMenuList::Node *node = m_menus.Item( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = node->GetData();

    menu->SetTitle( label );
}

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_menu_clicked_callback( GtkWidget *widget, wxMenu *menu )
{
    if (g_isIdle) wxapp_install_idle_handler();

    int id = menu->FindMenuIdByMenuItem(widget);

    /* should find it for normal (not popup) menu */
    wxASSERT( (id != -1) || (menu->GetInvokingWindow() != NULL) );

    if (!menu->IsEnabled(id))
        return;

    wxMenuItem* item = menu->FindChildItem( id );
    wxCHECK_RET( item, wxT("error in menu item callback") );

    if (item->IsCheckable())
    {
        bool isReallyChecked = item->IsChecked();
        if ( item->wxMenuItemBase::IsChecked() == isReallyChecked )
        {
            /* the menu item has been checked by calling wxMenuItem->Check() */
            return;
        }
        else
        {
            /* the user pressed on the menu item -> report and make consistent
             * again */
            item->wxMenuItemBase::Check(isReallyChecked);
        }
    }

    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, id );
    event.SetEventObject( menu );
    event.SetInt(id );

#if wxUSE_MENU_CALLBACK
    if (menu->GetCallback())
    {
        (void) (*(menu->GetCallback())) (*menu, event);
        return;
    }
#endif // wxUSE_MENU_CALLBACK

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
    if (g_isIdle) wxapp_install_idle_handler();

    int id = menu->FindMenuIdByMenuItem(widget);

    wxASSERT( id != -1 ); // should find it!

    if (!menu->IsEnabled(id))
        return;

    wxMenuEvent event( wxEVT_MENU_HIGHLIGHT, id );
    event.SetEventObject( menu );

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
    if (g_isIdle) wxapp_install_idle_handler();

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

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxMenuItemBase)

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                bool isCheckable,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, isCheckable, subMenu);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
{
    m_id = id;
    m_isCheckable = isCheckable;
    m_isChecked = FALSE;
    m_isEnabled = TRUE;
    m_subMenu = subMenu;
    m_parentMenu = parentMenu;
    m_help = help;

    m_menuItem = (GtkWidget *) NULL;

    DoSetText(text);
}

wxMenuItem::~wxMenuItem()
{
   // don't delete menu items, the menus take care of that
}

// return the menu item text without any menu accels
/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    wxString label;
#if (GTK_MINOR_VERSION > 0)
    for ( const wxChar *pc = text.c_str(); *pc; pc++ )
    {
        if ( *pc == wxT('_') )
        {
            // this is the escape character for GTK+ - skip it
            continue;
        }

        label += *pc;
    }
#else // GTK+ 1.0
    label = text;
#endif // GTK+ 1.2/1.0

    return label;
}

void wxMenuItem::SetText( const wxString& str )
{
    DoSetText(str);

    if (m_menuItem)
    {
        GtkLabel *label = GTK_LABEL( GTK_BIN(m_menuItem)->child );

        /* set new text */
        gtk_label_set( label, m_text.mb_str());

        /* reparse key accel */
        (void)gtk_label_parse_uline (GTK_LABEL(label), m_text.mb_str() );
        gtk_accel_label_refetch( GTK_ACCEL_LABEL(label) );
    }
}

// it's valid for this function to be called even if m_menuItem == NULL
void wxMenuItem::DoSetText( const wxString& str )
{
    /* '\t' is the deliminator indicating a hot key */
    m_text.Empty();
    const wxChar *pc = str;
    for (; (*pc != wxT('\0')) && (*pc != wxT('\t')); pc++ )
    {
        if (*pc == wxT('&'))
        {
#if (GTK_MINOR_VERSION > 0)
            m_text << wxT('_');
        }
        else if ( *pc == wxT('_') )    // escape underscores
        {
            m_text << wxT("__");
        }
        else if (*pc == wxT('/'))      /* we have to filter out slashes ... */
        {
            m_text << wxT('\\');  /* ... and replace them with back slashes */
#endif
        }
        else
            m_text << *pc;
    }

    /* only GTK 1.2 knows about hot keys */
    m_hotKey = wxT("");
#if (GTK_MINOR_VERSION > 0)
    if(*pc == wxT('\t'))
    {
       pc++;
       m_hotKey = pc;
    }
#endif
}

#if wxUSE_ACCEL

wxAcceleratorEntry *wxMenuItem::GetAccel() const
{
    if ( !GetHotKey() )
    {
        // nothing
        return (wxAcceleratorEntry *)NULL;
    }

    // as wxGetAccelFromString() looks for TAB, insert a dummy one here
    wxString label;
    label << wxT('\t') << GetHotKey();

    return wxGetAccelFromString(label);
}

#endif // wxUSE_ACCEL

void wxMenuItem::Check( bool check )
{
    wxCHECK_RET( m_menuItem, wxT("invalid menu item") );

    wxCHECK_RET( IsCheckable(), wxT("Can't check uncheckable item!") )

    if (check == m_isChecked)
        return;

    wxMenuItemBase::Check( check );
    gtk_check_menu_item_set_state( (GtkCheckMenuItem*)m_menuItem, (gint)check );
}

void wxMenuItem::Enable( bool enable )
{
    wxCHECK_RET( m_menuItem, wxT("invalid menu item") );

    gtk_widget_set_sensitive( m_menuItem, enable );
    wxMenuItemBase::Enable( enable );
}

bool wxMenuItem::IsChecked() const
{
    wxCHECK_MSG( m_menuItem, FALSE, wxT("invalid menu item") );

    wxCHECK_MSG( IsCheckable(), FALSE,
                 wxT("can't get state of uncheckable item!") );

    return ((GtkCheckMenuItem*)m_menuItem)->active != 0;
}

wxString wxMenuItem::GetFactoryPath() const
{
    /* in order to get the pointer to the item we need the item text _without_
       underscores */
    wxString path( wxT("<main>/") );
    path += GetLabel();

    return path;
}

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

void wxMenu::Init()
{
#if (GTK_MINOR_VERSION > 0)
    m_accel = gtk_accel_group_new();
    m_factory = gtk_item_factory_new( GTK_TYPE_MENU, "<main>", m_accel );
    m_menu = gtk_item_factory_get_widget( m_factory, "<main>" );
#else
    m_menu = gtk_menu_new();  // Do not show!
#endif

    m_owner = (GtkWidget*) NULL;

#if (GTK_MINOR_VERSION > 0)
    /* Tearoffs are entries, just like separators. So if we want this
       menu to be a tear-off one, we just append a tearoff entry
       immediately. */
    if(m_style & wxMENU_TEAROFF)
    {
       GtkItemFactoryEntry entry;
       entry.path = "/tearoff";
       entry.callback = (GtkItemFactoryCallback) NULL;
       entry.callback_action = 0;
       entry.item_type = "<Tearoff>";
       entry.accelerator = (gchar*) NULL;
       gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */
       //GtkWidget *menuItem = gtk_item_factory_get_widget( m_factory, "<main>/tearoff" );
    }
#endif

    // append the title as the very first entry if we have it
    if ( !!m_title )
    {
        Append(-2, m_title);
        AppendSeparator();
    }
}

wxMenu::~wxMenu()
{
   m_items.Clear();

   gtk_widget_destroy( m_menu );

   gtk_object_unref( GTK_OBJECT(m_factory) );
}

bool wxMenu::GtkAppend(wxMenuItem *mitem)
{
    GtkWidget *menuItem;

    if ( mitem->IsSeparator() )
    {
#if (GTK_MINOR_VERSION > 0)
        GtkItemFactoryEntry entry;
        entry.path = "/sep";
        entry.callback = (GtkItemFactoryCallback) NULL;
        entry.callback_action = 0;
        entry.item_type = "<Separator>";
        entry.accelerator = (gchar*) NULL;

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        /* this will be wrong for more than one separator. do we care? */
        menuItem = gtk_item_factory_get_widget( m_factory, "<main>/sep" );
#else // GTK+ 1.0
        menuItem = gtk_menu_item_new();
#endif // GTK 1.2/1.0
    }
    else if ( mitem->IsSubMenu() )
    {
#if (GTK_MINOR_VERSION > 0)
        /* text has "_" instead of "&" after mitem->SetText() */
        wxString text( mitem->GetText() );

        /* local buffer in multibyte form */
        char buf[200];
        strcpy( buf, "/" );
        strcat( buf, text.mb_str() );

        GtkItemFactoryEntry entry;
        entry.path = buf;
        entry.callback = (GtkItemFactoryCallback) 0;
        entry.callback_action = 0;
        entry.item_type = "<Branch>";
        entry.accelerator = (gchar*) NULL;

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        wxString path( mitem->GetFactoryPath() );
        menuItem = gtk_item_factory_get_item( m_factory, path.mb_str() );
#else // GTK+ 1.0
        menuItem = gtk_menu_item_new_with_label(mitem->GetText().mbc_str());
#endif // GTK 1.2/1.0

        gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), mitem->GetSubMenu()->m_menu );
    }
    else // a normal item
    {
#if (GTK_MINOR_VERSION > 0)
        /* text has "_" instead of "&" after mitem->SetText() */
        wxString text( mitem->GetText() );

        /* local buffer in multibyte form */
        char buf[200];
        strcpy( buf, "/" );
        strcat( buf, text.mb_str() );

        GtkItemFactoryEntry entry;
        entry.path = buf;
        entry.callback = (GtkItemFactoryCallback) gtk_menu_clicked_callback;
        entry.callback_action = 0;
        if ( mitem->IsCheckable() )
            entry.item_type = "<CheckItem>";
        else
            entry.item_type = "<Item>";
        entry.accelerator = (gchar*) NULL;

#if wxUSE_ACCEL
        // due to an apparent bug in GTK+, we have to use a static buffer here -
        // otherwise GTK+ 1.2.2 manages to override the memory we pass to it
        // somehow! (VZ)
        static char s_accel[32]; // must be big enough for <control><alt><shift>F12
        strncpy(s_accel, GetHotKey(*mitem).mb_str(), WXSIZEOF(s_accel));
        entry.accelerator = s_accel;
#else // !wxUSE_ACCEL
        entry.accelerator = (char*) NULL;
#endif // wxUSE_ACCEL/!wxUSE_ACCEL

        gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

        wxString path( mitem->GetFactoryPath() );
        menuItem = gtk_item_factory_get_widget( m_factory, path.mb_str() );
#else // GTK+ 1.0
        menuItem = checkable ? gtk_check_menu_item_new_with_label( mitem->GetText().mb_str() )
                             : gtk_menu_item_new_with_label( mitem->GetText().mb_str() );

        gtk_signal_connect( GTK_OBJECT(menuItem), "activate",
                            GTK_SIGNAL_FUNC(gtk_menu_clicked_callback),
                            (gpointer)this );
#endif // GTK+ 1.2/1.0
    }

    if ( !mitem->IsSeparator() )
    {
        gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                            GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                            (gpointer)this );

        gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                            GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                            (gpointer)this );
    }

#if GTK_MINOR_VERSION == 0
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
#endif // GTK+ 1.0

    mitem->SetMenuItem(menuItem);

    return TRUE;
}

bool wxMenu::DoAppend(wxMenuItem *mitem)
{
    return GtkAppend(mitem) && wxMenuBase::DoAppend(mitem);
}

bool wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if ( !wxMenuBase::DoInsert(pos, item) )
        return FALSE;

#ifdef __WXGTK12__
    // GTK+ doesn't have a function to insert a menu using GtkItemFactory (as
    // of version 1.2.6), so we first append the item and then change its
    // index
    if ( !GtkAppend(item) )
        return FALSE;

    GtkMenuShell *menu_shell = GTK_MENU_SHELL(m_factory->widget);
    gpointer data = g_list_last(menu_shell->children)->data;
    menu_shell->children = g_list_remove(menu_shell->children, data);
    menu_shell->children = g_list_insert(menu_shell->children, data, pos);

    return TRUE;
#else // GTK < 1.2
    // this should be easy to do...
    wxFAIL_MSG( wxT("not implemented") );

    return FALSE;
#endif // GTK 1.2/1.0
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    if ( !wxMenuBase::DoRemove(item) )
        return (wxMenuItem *)NULL;

    // TODO: this code doesn't delete the item factory item and this seems
    //       impossible as of GTK 1.2.6.
    gtk_widget_destroy( item->GetMenuItem() );

    return item;
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

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if (GTK_MINOR_VERSION > 0) && wxUSE_ACCEL
static wxString GetHotKey( const wxMenuItem& item )
{
    wxString hotkey;

    wxAcceleratorEntry *accel = item.GetAccel();
    if ( accel )
    {
        int flags = accel->GetFlags();
        if ( flags & wxACCEL_ALT )
            hotkey += wxT("<alt>");
        if ( flags & wxACCEL_CTRL )
            hotkey += wxT("<control>");
        if ( flags & wxACCEL_SHIFT )
            hotkey += wxT("<shift>");

        int code = accel->GetKeyCode();
        switch ( code )
        {
            case WXK_F1:
            case WXK_F2:
            case WXK_F3:
            case WXK_F4:
            case WXK_F5:
            case WXK_F6:
            case WXK_F7:
            case WXK_F8:
            case WXK_F9:
            case WXK_F10:
            case WXK_F11:
            case WXK_F12:
                hotkey << wxT('F') << code - WXK_F1 + 1;
                break;

                // if there are any other keys wxGetAccelFromString() may return,
                // we should process them here

            default:
                if ( wxIsalnum(code) )
                {
                    hotkey << (wxChar)code;

                    break;
                }

                wxFAIL_MSG( wxT("unknown keyboard accel") );
        }

        delete accel;
    }

    return hotkey;
}
#endif // wxUSE_ACCEL

