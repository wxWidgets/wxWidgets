/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "choice.h"
#endif

#include "wx/defs.h"

#if wxUSE_CHOICE

#include "wx/choice.h"
#include "wx/arrstr.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_choice_clicked_callback( GtkWidget *WXUNUSED(widget), wxChoice *choice )
{
    if (g_isIdle)
      wxapp_install_idle_handler();

    if (!choice->m_hasVMT) return;

    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, choice->GetId() );
    int n = choice->GetSelection();

    event.SetInt( n );
    event.SetString( choice->GetStringSelection() );
    event.SetEventObject(choice);

    if ( choice->HasClientObjectData() )
        event.SetClientObject( choice->GetClientObject(n) );
    else if ( choice->HasClientUntypedData() )
        event.SetClientData( choice->GetClientData(n) );

    choice->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoice,wxControl)

wxChoice::wxChoice()
{
    m_strings = (wxSortedArrayString *)NULL;
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       const wxArrayString& choices,
                       long style, const wxValidator& validator,
                       const wxString &name )
{
    wxCArrayString chs(choices);

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                   style, validator, name );
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       int n, const wxString choices[],
                       long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
#if (GTK_MINOR_VERSION > 0)
    m_acceptsFocus = TRUE;
#endif

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxChoice creation failed") );
        return FALSE;
    }

    m_widget = gtk_option_menu_new();

    if ( style & wxCB_SORT )
    {
        // if our m_strings != NULL, DoAppend() will check for it and insert
        // items in the correct order
        m_strings = new wxSortedArrayString;
    }

    GtkWidget *menu = gtk_menu_new();

    for (int i = 0; i < n; i++)
    {
        GtkAddHelper(menu, i, choices[i]);
    }

    gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );

    m_parent->DoAddChild( this );

    PostCreation(size);
    SetBestSize(size); // need this too because this is a wxControlWithItems

    return TRUE;
}

wxChoice::~wxChoice()
{
    Clear();

    delete m_strings;
}

int wxChoice::DoAppend( const wxString &item )
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid choice control") );

    GtkWidget *menu = gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) );

    return GtkAddHelper(menu, GetCount(), item);
}

int wxChoice::DoInsert( const wxString &item, int pos )
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid choice control") );
    wxCHECK_MSG( (pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    if (pos == GetCount())
        return DoAppend(item);

    GtkWidget *menu = gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) );

    return GtkAddHelper(menu, pos, item);
}

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice control") );

    wxList::compatibility_iterator node = m_clientList.Item( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientData") );

    node->SetData( (wxObject*) clientData );
}

void* wxChoice::DoGetItemClientData( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, NULL, wxT("invalid choice control") );

    wxList::compatibility_iterator node = m_clientList.Item( n );
    wxCHECK_MSG( node, NULL, wxT("invalid index in wxChoice::DoGetItemClientData") );

    return node->GetData();
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice control") );

    wxList::compatibility_iterator node = m_clientList.Item( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientObject") );

    // wxItemContainer already deletes data for us

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxChoice::DoGetItemClientObject( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*) NULL, wxT("invalid choice control") );

    wxList::compatibility_iterator node = m_clientList.Item( n );
    wxCHECK_MSG( node, (wxClientData *)NULL,
                 wxT("invalid index in wxChoice::DoGetItemClientObject") );

    return (wxClientData*) node->GetData();
}

void wxChoice::Clear()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice") );

    gtk_option_menu_remove_menu( GTK_OPTION_MENU(m_widget) );
    GtkWidget *menu = gtk_menu_new();
    gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );

    if ( HasClientObjectData() )
    {
        // destroy the data (due to Robert's idea of using wxList<wxObject>
        // and not wxList<wxClientData> we can't just say
        // m_clientList.DeleteContents(TRUE) - this would crash!
        wxList::compatibility_iterator node = m_clientList.GetFirst();
        while ( node )
        {
            delete (wxClientData *)node->GetData();
            node = node->GetNext();
        }
    }
    m_clientList.Clear();

    if ( m_strings )
        m_strings->Clear();
}

void wxChoice::Delete( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice") );

    // VZ: apparently GTK+ doesn't have a built-in function to do it (not even
    //     in 2.0), hence this dumb implementation -- still better than nothing
    int i,
        count = GetCount();

    wxCHECK_RET( n >= 0 && n < count, _T("invalid index in wxChoice::Delete") );

    const bool hasClientData = m_clientDataItemsType != wxClientData_None;
    const bool hasObjectData = m_clientDataItemsType == wxClientData_Object;

    wxList::compatibility_iterator node = m_clientList.GetFirst();

    wxArrayString items;
    wxArrayPtrVoid itemsData;
    items.Alloc(count);
    for ( i = 0; i < count; i++ )
    {
        if ( i != n )
        {
            items.Add(GetString(i));
            if ( hasClientData )
            {
                // also save the client data
                itemsData.Add(node->GetData());
            }
        }
        else // need to delete the client object too
        {
            if ( hasObjectData )
            {
                delete (wxClientData *)node->GetData();
            }
        }

        if ( hasClientData )
        {
            node = node->GetNext();
        }
    }

    if ( hasObjectData )
    {
        // prevent Clear() from destroying all client data
        m_clientDataItemsType = wxClientData_None;
    }

    Clear();

    for ( i = 0; i < count - 1; i++ )
    {
        Append(items[i]);

        if ( hasObjectData )
            SetClientObject(i, (wxClientData *)itemsData[i]);
        else if ( hasClientData )
            SetClientData(i, itemsData[i]);
    }
}

int wxChoice::FindString( const wxString &string ) const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid choice") );

    // If you read this code once and you think you understand
    // it, then you are very wrong. Robert Roebling.

    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
    int count = 0;
    GList *child = menu_shell->children;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = (GtkLabel *) NULL;
        if (bin->child)
            label = GTK_LABEL(bin->child);
        if (!label)
            label = GTK_LABEL( BUTTON_CHILD(m_widget) );

        wxASSERT_MSG( label != NULL , wxT("wxChoice: invalid label") );

#ifdef __WXGTK20__
         wxString tmp( wxGTK_CONV_BACK( gtk_label_get_text( label) ) );
#else
         wxString tmp( label->label );
#endif
        if (string == tmp)
            return count;

        child = child->next;
        count++;
    }

    return -1;
}

int wxChoice::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid choice") );

#ifdef __WXGTK20__

    return gtk_option_menu_get_history( GTK_OPTION_MENU(m_widget) );

#else
    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
    int count = 0;

    GList *child = menu_shell->children;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        if (!bin->child) return count;
        child = child->next;
        count++;
    }

    return -1;
#endif
}

void wxChoice::SetString( int n, const wxString& str )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice") );

    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
    int count = 0;
    GList *child = menu_shell->children;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        if (count == n)
        {
            GtkLabel *label = (GtkLabel *) NULL;
            if (bin->child)
                label = GTK_LABEL(bin->child);
            if (!label)
                label = GTK_LABEL( BUTTON_CHILD(m_widget) );

            wxASSERT_MSG( label != NULL , wxT("wxChoice: invalid label") );

            gtk_label_set_text( label, wxGTK_CONV( str ) ); 
            
            return;
        }
        child = child->next;
        count++;
    }
}

wxString wxChoice::GetString( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid choice") );

    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
    int count = 0;
    GList *child = menu_shell->children;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        if (count == n)
        {
            GtkLabel *label = (GtkLabel *) NULL;
            if (bin->child)
                label = GTK_LABEL(bin->child);
            if (!label)
                label = GTK_LABEL( BUTTON_CHILD(m_widget) );

            wxASSERT_MSG( label != NULL , wxT("wxChoice: invalid label") );

#ifdef __WXGTK20__
            return wxString( wxGTK_CONV_BACK( gtk_label_get_text( label) ) );
#else
            return wxString( label->label );
#endif
        }
        child = child->next;
        count++;
    }

    wxFAIL_MSG( wxT("wxChoice: invalid index in GetString()") );

    return wxT("");
}

int wxChoice::GetCount() const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid choice") );

    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );
    int count = 0;
    GList *child = menu_shell->children;
    while (child)
    {
        count++;
        child = child->next;
    }
    return count;
}

void wxChoice::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice") );

    int tmp = n;
    gtk_option_menu_set_history( GTK_OPTION_MENU(m_widget), (gint)tmp );
}

void wxChoice::DoApplyWidgetStyle(GtkRcStyle *style)
{
    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );

    gtk_widget_modify_style( m_widget, style );
    gtk_widget_modify_style( GTK_WIDGET( menu_shell ), style );

    GList *child = menu_shell->children;
    while (child)
    {
        gtk_widget_modify_style( GTK_WIDGET( child->data ), style );

        GtkBin *bin = GTK_BIN( child->data );
        GtkWidget *label = (GtkWidget *) NULL;
        if (bin->child)
            label = bin->child;
        if (!label)
            label = BUTTON_CHILD(m_widget);

        gtk_widget_modify_style( label, style );

        child = child->next;
    }
}

int wxChoice::GtkAddHelper(GtkWidget *menu, int pos, const wxString& item)
{
    wxCHECK_MSG((pos>=0) && (pos<=(int)m_clientList.GetCount()), -1, wxT("invalid index"));

    GtkWidget *menu_item = gtk_menu_item_new_with_label( wxGTK_CONV( item ) );

    size_t index;
    if ( m_strings )
    {
        // sorted control, need to insert at the correct index
        index = m_strings->Add(item);

        gtk_menu_insert( GTK_MENU(menu), menu_item, index );

        if ( index )
        {
            m_clientList.Insert( m_clientList.Item(index - 1),
                                 (wxObject*) NULL );
        }
        else
        {
            m_clientList.Insert( (wxObject*) NULL );
        }
    }
    else
    {
        // don't call wxChoice::GetCount() from here because it doesn't work
        // if we're called from ctor (and GtkMenuShell is still NULL)

        // normal control, just append
        if (pos == (int)m_clientList.GetCount())
        {
        gtk_menu_append( GTK_MENU(menu), menu_item );
        m_clientList.Append( (wxObject*) NULL );
        index = m_clientList.GetCount() - 1;
        }
        else
        {
            gtk_menu_insert( GTK_MENU(menu), menu_item, pos );
            m_clientList.Insert( pos, (wxObject*) NULL );
            index = pos;
        }
    }

    if (GTK_WIDGET_REALIZED(m_widget))
    {
        gtk_widget_realize( menu_item );
        gtk_widget_realize( GTK_BIN(menu_item)->child );

        ApplyWidgetStyle();
    }

    // The best size of a wxChoice should probably
    // be changed everytime the control has been
    // changed, but at least after adding an item
    // it has to change. Adapted from Matt Ownby.
    InvalidateBestSize();
    
    gtk_signal_connect( GTK_OBJECT( menu_item ), "activate",
      GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );

    gtk_widget_show( menu_item );

    // return the index of the item in the control
    return index;
}

wxSize wxChoice::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );

    // we know better our horizontal extent: it depends on the longest string
    // we have
    ret.x = 0;
    if ( m_widget )
    {
        int width;
        size_t count = GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            GetTextExtent( GetString(n), &width, NULL, NULL, NULL );
            if ( width > ret.x )
                ret.x = width;
        }

        // add extra for the choice "=" button

        // VZ: I don't know how to get the right value, it seems to be in
        //     GtkOptionMenuProps struct from gtkoptionmenu.c but we can't get
        //     to it - maybe we can use gtk_option_menu_size_request() for this
        //     somehow?
        //
        //     This default value works only for the default GTK+ theme (i.e.
        //     no theme at all) (FIXME)
        static const int widthChoiceIndicator = 35;
        ret.x += widthChoiceIndicator;
    }

    // but not less than the minimal width
    if ( ret.x < 80 )
        ret.x = 80;

    // If this request_size is called with no entries then
    // the returned height is wrong. Give it a reasonable
    // default value.
    if (ret.y <= 18)
        ret.y = 8 + GetCharHeight();

    CacheBestSize(ret);
    return ret;
}

bool wxChoice::IsOwnGtkWindow( GdkWindow *window )
{
#ifdef __WXGTK20__
    return GTK_BUTTON(m_widget)->event_window;
#else
    return (window == m_widget->window);
#endif
}

// static
wxVisualAttributes
wxChoice::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_option_menu_new);
}


#endif // wxUSE_CHOICE

