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

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

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
    while (top_frame->GetParent() && !(top_frame->GetParent()->m_isFrame))
        top_frame = top_frame->GetParent();

    /* support for native hot keys  */
    gtk_accel_group_detach( menu->m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxNode *node = menu->GetItems().First();
    while (node)
    {
        wxMenuItem *menuitem = (wxMenuItem*)node->Data();
        if (menuitem->IsSubMenu())
            wxMenubarUnsetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->Next();
    }
}

static void wxMenubarSetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( win );

#if (GTK_MINOR_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->GetParent()->m_isFrame))
        top_frame = top_frame->GetParent();

    /* support for native hot keys  */
    gtk_accel_group_attach( menu->m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxNode *node = menu->GetItems().First();
    while (node)
    {
        wxMenuItem *menuitem = (wxMenuItem*)node->Data();
        if (menuitem->IsSubMenu())
            wxMenubarSetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->Next();
    }
}

void wxMenuBar::SetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = win;
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->GetParent()->m_isFrame))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    gtk_accel_group_attach( m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxNode *node = m_menus.First();
    while (node)
    {
        wxMenu *menu = (wxMenu*)node->Data();
        wxMenubarSetInvokingWindow( menu, win );
        node = node->Next();
    }
}

void wxMenuBar::UnsetInvokingWindow( wxWindow *win )
{
    m_invokingWindow = (wxWindow*) NULL;
#if (GTK_MINOR_VERSION > 0) && (GTK_MICRO_VERSION > 0)
    wxWindow *top_frame = win;
    while (top_frame->GetParent() && !(top_frame->GetParent()->m_isFrame))
        top_frame = top_frame->GetParent();

    /* support for native key accelerators indicated by underscroes */
    gtk_accel_group_detach( m_accel, GTK_OBJECT(top_frame->m_widget) );
#endif

    wxNode *node = m_menus.First();
    while (node)
    {
        wxMenu *menu = (wxMenu*)node->Data();
        wxMenubarUnsetInvokingWindow( menu, win );
        node = node->Next();
    }
}

void wxMenuBar::Append( wxMenu *menu, const wxString &title )
{
    m_menus.Append( menu );

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

wxMenuItem *wxMenuBar::FindItemForId(int itemId, wxMenu **menuForItem ) const
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

    wxCHECK_RET( item, wxT("wxMenuBar::Check: no such item") );

    item->Check(check);
}

bool wxMenuBar::IsChecked( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, FALSE, wxT("wxMenuBar::IsChecked: no such item") );

    return item->IsChecked();
}

void wxMenuBar::Enable( int id, bool enable )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, wxT("wxMenuBar::Enable: no such item") );

    item->Enable(enable);
}

bool wxMenuBar::IsEnabled( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, FALSE, wxT("wxMenuBar::IsEnabled: no such item") );

    return item->IsEnabled();
}

wxString wxMenuBar::GetLabel( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, wxT(""), wxT("wxMenuBar::GetLabel: no such item") );

    return item->GetText();
}

void wxMenuBar::SetLabel( int id, const wxString &label )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, wxT("wxMenuBar::SetLabel: no such item") );

    item->SetText( label );
}

void wxMenuBar::EnableTop( int pos, bool flag )
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = (wxMenu*)node->Data();

    if (menu->m_owner)
        gtk_widget_set_sensitive( menu->m_owner, flag );
}

wxString wxMenuBar::GetLabelTop( int pos ) const
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_MSG( node, wxT("invalid"), wxT("menu not found") );

    wxMenu* menu = (wxMenu*)node->Data();

    return menu->GetTitle();
}

void wxMenuBar::SetLabelTop( int pos, const wxString& label )
{
    wxNode *node = m_menus.Nth( pos );

    wxCHECK_RET( node, wxT("menu not found") );

    wxMenu* menu = (wxMenu*)node->Data();

    menu->SetTitle( label );
}

void wxMenuBar::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_RET( item, wxT("wxMenuBar::SetHelpString: no such item") );

    item->SetHelp( helpString );
}

wxString wxMenuBar::GetHelpString( int id ) const
{
    wxMenuItem* item = FindMenuItemById( id );

    wxCHECK_MSG( item, wxT(""), wxT("wxMenuBar::GetHelpString: no such item") );

    return item->GetHelp();
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

    wxMenuItem* item = menu->FindItem( id );
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

    // call it after initializing m_menuItem to NULL
    DoSetText(text);
}

wxMenuItem::~wxMenuItem()
{
   // don't delete menu items, the menus take care of that
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

    if (m_menuItem)
    {
        GtkLabel *label = GTK_LABEL( GTK_BIN(m_menuItem)->child );
        gtk_label_set( label, m_text.mb_str());
    }
}

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

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenu,wxEvtHandler)

void
wxMenu::Init( const wxString& title,
              long style,
              const wxFunction func
            )
{
    m_title = title;
    m_items.DeleteContents( TRUE );
    m_invokingWindow = (wxWindow *) NULL;
    m_style = style;

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

    if (m_title.IsNull()) m_title = wxT("");
    if (m_title != wxT(""))
    {
        Append(-2, m_title);
        AppendSeparator();
    }

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
}

wxMenu::~wxMenu()
{
    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        wxMenu *submenu = item->GetSubMenu();
        if (submenu)
           delete submenu;
        node = node->Next();
    }

   gtk_widget_destroy( m_menu );

   gtk_object_unref( GTK_OBJECT(m_factory) );
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
    wxMenuItem *mitem = new wxMenuItem(this, wxID_SEPARATOR);

#if (GTK_MINOR_VERSION > 0)
    GtkItemFactoryEntry entry;
    entry.path = "/sep";
    entry.callback = (GtkItemFactoryCallback) NULL;
    entry.callback_action = 0;
    entry.item_type = "<Separator>";
    entry.accelerator = (gchar*) NULL;

    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

    /* this will be wrong for more than one separator. do we care? */
    GtkWidget *menuItem = gtk_item_factory_get_widget( m_factory, "<main>/sep" );
#else
    GtkWidget *menuItem = gtk_menu_item_new();
    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );
#endif

    mitem->SetMenuItem(menuItem);
    m_items.Append( mitem );
}

#if (GTK_MINOR_VERSION > 0) && wxUSE_ACCEL
static wxString GetHotKey( const wxMenuItem& item )
{
    wxString hotkey;

    // as wxGetAccelFromString() looks for TAB, insert a dummy one here
    wxString label;
    label << wxT('\t') << item.GetHotKey();
    wxAcceleratorEntry *accel = wxGetAccelFromString(label);
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
                hotkey << wxT('F') << code = WXK_F1 + 1;
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
    }
    if (accel)
        delete accel;

    return hotkey;
}
#endif // wxUSE_ACCEL

void wxMenu::Append( int id, const wxString &item, const wxString &helpStr, bool checkable )
{
    wxMenuItem *mitem = new wxMenuItem(this, id, item, helpStr, checkable);

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
    if (checkable)
        entry.item_type = "<CheckItem>";
    else
        entry.item_type = "<Item>";

#if wxUSE_ACCEL
    // due to an apparent bug in GTK+, we have to use a static buffer here -
    // otherwise GTK+ 1.2.2 manages to override the memory we pass to it
    // somehow! (VZ)
    static char s_accel[32]; // must be big enough for <control><alt><shift>F12
    strncpy(s_accel, GetHotKey(*mitem).mb_str(), WXSIZEOF(s_accel));
    entry.accelerator = s_accel;
#else
    entry.accelerator = NULL;
#endif

    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

    /* in order to get the pointer to the item we need the item text _without_ underscores */
    wxString s = wxT("<main>/");
    for ( const wxChar *pc = text; *pc != wxT('\0'); pc++ )
    {
        while (*pc == wxT('_')) pc++; /* skip it */
        s << *pc;
    }

    GtkWidget *menuItem = gtk_item_factory_get_widget( m_factory, s.mb_str() );

#else

    GtkWidget *menuItem = checkable ? gtk_check_menu_item_new_with_label( mitem->GetText().mb_str() )
                                    : gtk_menu_item_new_with_label( mitem->GetText().mb_str() );

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

void wxMenu::Append( int id, const wxString &item, wxMenu *subMenu, const wxString &helpStr )
{
    wxMenuItem *mitem = new wxMenuItem(this, id, item, helpStr, FALSE, subMenu);

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

    gtk_item_factory_create_item( m_factory, &entry, (gpointer) this, 2 );  /* what is 2 ? */

    /* in order to get the pointer to the item we need the item text _without_ underscores */
    wxString s = wxT("<main>/");
    for ( const wxChar *pc = text; *pc != wxT('\0'); pc++ )
    {
        if (*pc == wxT('_')) pc++; /* skip it */
        s << *pc;
    }

    GtkWidget *menuItem = gtk_item_factory_get_item( m_factory, s.mb_str() );

#else

    GtkWidget *menuItem = gtk_menu_item_new_with_label(mitem->GetText().mbc_str());

    gtk_menu_append( GTK_MENU(m_menu), menuItem );
    gtk_widget_show( menuItem );

#endif

    gtk_signal_connect( GTK_OBJECT(menuItem), "select",
                        GTK_SIGNAL_FUNC(gtk_menu_hilight_callback),
                        (gpointer*)this );

    gtk_signal_connect( GTK_OBJECT(menuItem), "deselect",
                        GTK_SIGNAL_FUNC(gtk_menu_nolight_callback),
                        (gpointer*)this );

    gtk_menu_item_set_submenu( GTK_MENU_ITEM(menuItem), subMenu->m_menu );

    mitem->SetMenuItem(menuItem);

    m_items.Append( mitem );
}

void wxMenu::Append( wxMenuItem *item )
{
    m_items.Append( item );

    GtkWidget *menuItem = (GtkWidget*) NULL;

    if (item->IsSeparator())
        menuItem = gtk_menu_item_new();
    else if (item->IsSubMenu())
        menuItem = gtk_menu_item_new_with_label(item->GetText().mbc_str());
    else
        menuItem = item->IsCheckable() ? gtk_check_menu_item_new_with_label(item->GetText().mbc_str())
                                       : gtk_menu_item_new_with_label(item->GetText().mbc_str());

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

void wxMenu::Delete( int id )
{
    wxNode *node = m_items.First();
    while (node)
    {
        wxMenuItem *item = (wxMenuItem*)node->Data();
        if (item->GetId() == id)
        {
            gtk_widget_destroy( item->GetMenuItem() );
            m_items.DeleteNode( node );
            return;
        }
        node = node->Next();
    }
}

int wxMenu::FindItem( const wxString itemString ) const
{
    wxString s = wxT("");
    for ( const wxChar *pc = itemString; *pc != wxT('\0'); pc++ )
    {
        if (*pc == wxT('&'))
        {
            pc++; /* skip it */
#if (GTK_MINOR_VERSION > 0)
            s << wxT('_');
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

    wxCHECK_RET( item, wxT("wxMenu::Enable: no such item") );

    item->Enable(enable);
}

bool wxMenu::IsEnabled( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenu::IsEnabled: no such item") );

    return item->IsEnabled();
}

void wxMenu::Check( int id, bool enable )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::Check: no such item") );

    item->Check(enable);
}

bool wxMenu::IsChecked( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, FALSE, wxT("wxMenu::IsChecked: no such item") );

    return item->IsChecked();
}

void wxMenu::SetLabel( int id, const wxString &label )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::SetLabel: no such item") );

    item->SetText(label);
}

wxString wxMenu::GetLabel( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxT(""), wxT("wxMenu::GetLabel: no such item") );

    return item->GetText();
}

void wxMenu::SetHelpString( int id, const wxString& helpString )
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_RET( item, wxT("wxMenu::SetHelpString: no such item") );

    item->SetHelp( helpString );
}

wxString wxMenu::GetHelpString( int id ) const
{
    wxMenuItem *item = FindItem(id);

    wxCHECK_MSG( item, wxT(""), wxT("wxMenu::GetHelpString: no such item") );

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

