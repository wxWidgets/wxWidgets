/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#include "wx/choice.h"

#if wxUSE_CHOICE

#include <gdk/gdk.h>
#include <gtk/gtk.h>

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

    SetSizeOrDefault( size );

    if ( style & wxCB_SORT )
    {
        // if our m_strings != NULL, DoAppend() will check for it and insert
        // items in the correct order
        m_strings = new wxSortedArrayString;
    }

    GtkWidget *menu = gtk_menu_new();

    for (int i = 0; i < n; i++)
    {
        GtkAppendHelper(menu, choices[i]);
    }

    gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );

    m_parent->DoAddChild( this );

    PostCreation();

    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );
    SetFont( parent->GetFont() );

    Show( TRUE );

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

    return GtkAppendHelper(menu, item);
}

void wxChoice::DoSetItemClientData( int n, void* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice control") );

    wxNode *node = m_clientList.Nth( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientData") );

    node->SetData( (wxObject*) clientData );
}

void* wxChoice::DoGetItemClientData( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, NULL, wxT("invalid choice control") );

    wxNode *node = m_clientList.Nth( n );
    wxCHECK_MSG( node, NULL, wxT("invalid index in wxChoice::DoGetItemClientData") );

    return node->Data();
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice control") );

    wxNode *node = m_clientList.Nth( n );
    wxCHECK_RET( node, wxT("invalid index in wxChoice::DoSetItemClientObject") );

    wxClientData *cd = (wxClientData*) node->Data();
    delete cd;

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxChoice::DoGetItemClientObject( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*) NULL, wxT("invalid choice control") );

    wxNode *node = m_clientList.Nth( n );
    wxCHECK_MSG( node, (wxClientData *)NULL,
                 wxT("invalid index in wxChoice::DoGetItemClientObject") );

    return (wxClientData*) node->Data();
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
        wxNode *node = m_clientList.First();
        while ( node )
        {
            delete (wxClientData *)node->Data();
            node = node->Next();
        }
    }
    m_clientList.Clear();

    if ( m_strings )
        m_strings->Clear();
}

void wxChoice::Delete( int WXUNUSED(n) )
{
    wxFAIL_MSG( wxT("wxChoice:Delete not implemented") );
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
        if (bin->child) label = GTK_LABEL(bin->child);
        if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );

        wxASSERT_MSG( label != NULL , wxT("wxChoice: invalid label") );

       if (string == wxString(label->label,*wxConvCurrent))
           return count;

       child = child->next;
       count++;
    }

    return -1;
}

int wxChoice::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid choice") );

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
}

void wxChoice::SetString( int WXUNUSED(n), const wxString& WXUNUSED(string) )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid choice") );

    wxFAIL_MSG(wxT("not implemented"));
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
            if (bin->child) label = GTK_LABEL(bin->child);
            if (!label) label = GTK_LABEL( GTK_BUTTON(m_widget)->child );

            wxASSERT_MSG( label != NULL , wxT("wxChoice: invalid label") );

            return wxString(label->label,*wxConvCurrent);
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

void wxChoice::ApplyWidgetStyle()
{
    SetWidgetStyle();

    GtkMenuShell *menu_shell = GTK_MENU_SHELL( gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) ) );

    gtk_widget_set_style( m_widget, m_widgetStyle );
    gtk_widget_set_style( GTK_WIDGET( menu_shell ), m_widgetStyle );

    GList *child = menu_shell->children;
    while (child)
    {
        gtk_widget_set_style( GTK_WIDGET( child->data ), m_widgetStyle );

        GtkBin *bin = GTK_BIN( child->data );
        GtkWidget *label = (GtkWidget *) NULL;
        if (bin->child) label = bin->child;
        if (!label) label = GTK_BUTTON(m_widget)->child;

        gtk_widget_set_style( label, m_widgetStyle );

        child = child->next;
    }
}

size_t wxChoice::GtkAppendHelper(GtkWidget *menu, const wxString& item)
{
    GtkWidget *menu_item = gtk_menu_item_new_with_label( item.mbc_str() );

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
        // normal control, just append
        gtk_menu_append( GTK_MENU(menu), menu_item );

        m_clientList.Append( (wxObject*) NULL );

        // don't call wxChoice::GetCount() from here because it doesn't work
        // if we're called from ctor (and GtkMenuShell is still NULL)
        index = m_clientList.GetCount() - 1;
    }

    if (GTK_WIDGET_REALIZED(m_widget))
    {
        gtk_widget_realize( menu_item );
        gtk_widget_realize( GTK_BIN(menu_item)->child );

        if (m_widgetStyle) ApplyWidgetStyle();
    }

    gtk_signal_connect( GTK_OBJECT( menu_item ), "activate",
      GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );

    gtk_widget_show( menu_item );

    // return the index of the item in the control
    return index;
}

wxSize wxChoice::DoGetBestSize() const
{
    return wxSize(80, 26);
}

#endif
