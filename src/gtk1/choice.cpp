/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#include "wx/choice.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "activate"
//-----------------------------------------------------------------------------

static void gtk_choice_clicked_callback( GtkWidget *WXUNUSED(widget), wxChoice *choice )
{
  if (!choice->HasVMT()) return;
  if (g_blockEventsOnDrag) return;
  
  wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, choice->GetId() );
  event.SetInt( choice->GetSelection() );
  wxString tmp( choice->GetStringSelection() );
  event.SetString( WXSTRINGCAST(tmp) );
  event.SetEventObject(choice);
  choice->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxChoice,wxControl)

wxChoice::wxChoice()
{
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size,
                       int n, const wxString choices[],
                       long style, const wxValidator& validator, const wxString &name )
{
    m_needParent = TRUE;
  
    PreCreation( parent, id, pos, size, style, name );
    
    SetValidator( validator );

    m_widget = gtk_option_menu_new();
  
    wxSize newSize = size;
    if (newSize.x == -1) newSize.x = 80;
    if (newSize.y == -1) newSize.y = 26;
    SetSize( newSize.x, newSize.y );
  
    GtkWidget *menu = gtk_menu_new();
  
    for (int i = 0; i < n; i++)
    {
        m_clientDataList.Append( (wxObject*) NULL );
        m_clientObjectList.Append( (wxObject*) NULL );
  
        GtkWidget *item = gtk_menu_item_new_with_label( choices[i] );
        gtk_menu_append( GTK_MENU(menu), item );
    
        gtk_widget_realize( item );
        gtk_widget_realize( GTK_BIN(item)->child );
    
        gtk_widget_show( item );
    
        gtk_signal_connect( GTK_OBJECT( item ), "activate", 
          GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );
    }
    gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
  
    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );
  
    PostCreation();
  
    SetBackgroundColour( parent->GetBackgroundColour() );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );
    
    return TRUE;
}
      
wxChoice::~wxChoice()
{
    Clear();
}

void wxChoice::AppendCommon( const wxString &item )
{
    wxCHECK_RET( m_widget != NULL, "invalid choice" );
  
    GtkWidget *menu = gtk_option_menu_get_menu( GTK_OPTION_MENU(m_widget) );
    GtkWidget *menu_item = gtk_menu_item_new_with_label( item );
  
    gtk_menu_append( GTK_MENU(menu), menu_item );
  
    gtk_widget_realize( menu_item );
    gtk_widget_realize( GTK_BIN(menu_item)->child );
  
    if (m_widgetStyle) ApplyWidgetStyle();
  
    gtk_signal_connect( GTK_OBJECT( menu_item ), "activate", 
      GTK_SIGNAL_FUNC(gtk_choice_clicked_callback), (gpointer*)this );
    
    gtk_widget_show( menu_item );
}

void wxChoice::Append( const wxString &item )
{
    m_clientDataList.Append( (wxObject*) NULL );
    m_clientObjectList.Append( (wxObject*) NULL );
  
    AppendCommon( item );
}

void wxChoice::Append( const wxString &item, void *clientData )
{
    m_clientDataList.Append( (wxObject*) clientData );
    m_clientObjectList.Append( (wxObject*) NULL );
  
    AppendCommon( item );
}

void wxChoice::Append( const wxString &item, wxClientData *clientData )
{
    m_clientObjectList.Append( (wxObject*) clientData );
    m_clientDataList.Append( (wxObject*) NULL );
  
    AppendCommon( item );
}

void wxChoice::SetClientData( int n, void* clientData )
{
    wxCHECK_RET( m_widget != NULL, "invalid combobox" );
  
    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return;
  
    node->SetData( (wxObject*) clientData );
}

void* wxChoice::GetClientData( int n )
{
    wxCHECK_MSG( m_widget != NULL, NULL, "invalid combobox" );
  
    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return NULL;
    
    return node->Data();
}

void wxChoice::SetClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, "invalid combobox" );
    
    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return;
  
    wxClientData *cd = (wxClientData*) node->Data();
    if (cd) delete cd;
    
    node->SetData( (wxObject*) clientData );
}

wxClientData* wxChoice::GetClientObject( int n )
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*) NULL, "invalid combobox" );
  
    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return (wxClientData*) NULL;
    
    return (wxClientData*) node->Data();
}
 
void wxChoice::Clear()
{
    wxCHECK_RET( m_widget != NULL, "invalid choice" );

    gtk_option_menu_remove_menu( GTK_OPTION_MENU(m_widget) );
    GtkWidget *menu = gtk_menu_new();
    gtk_option_menu_set_menu( GTK_OPTION_MENU(m_widget), menu );
    
    wxNode *node = m_clientObjectList.First();
    while (node)
    {
        wxClientData *cd = (wxClientData*)node->Data();
        if (cd) delete cd;
        node = node->Next();
    }
    m_clientObjectList.Clear();
    
    m_clientDataList.Clear();
}

void wxChoice::Delete( int WXUNUSED(n) )
{
    wxFAIL_MSG( "wxChoice:Delete not implemented" );
}

int wxChoice::FindString( const wxString &string ) const
{
    wxCHECK_MSG( m_widget != NULL, -1, "invalid choice" );

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
    
        wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
    
       if (string == label->label) return count;
       child = child->next;
       count++;
    }
  
    wxFAIL_MSG( "wxChoice: string not found" );
  
    return -1;
}

int wxChoice::GetColumns() const
{
    return 1;
}

int wxChoice::GetSelection()
{
    wxCHECK_MSG( m_widget != NULL, -1, "invalid choice" );

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
  
    wxFAIL_MSG( "wxChoice: no selection" );
  
    return -1;
}

wxString wxChoice::GetString( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, "", "invalid choice" );

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
      
            wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
      
            return label->label;
        }
        child = child->next;
        count++;
    }
  
    wxFAIL_MSG( "wxChoice: string not found" );
  
    return "";
}

wxString wxChoice::GetStringSelection() const
{
    wxCHECK_MSG( m_widget != NULL, "", "invalid choice" );

    GtkLabel *label = GTK_LABEL( GTK_BUTTON(m_widget)->child );
  
    wxASSERT_MSG( label != NULL , "wxChoice: invalid label" );
  
    return label->label;
}

int wxChoice::Number() const
{
    wxCHECK_MSG( m_widget != NULL, 0, "invalid choice" );

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

void wxChoice::SetColumns( int WXUNUSED(n) )
{
}

void wxChoice::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, "invalid choice" );

    int tmp = n;
    gtk_option_menu_set_history( GTK_OPTION_MENU(m_widget), (gint)tmp );
  
    gtk_choice_clicked_callback( (GtkWidget *) NULL, this );
}

void wxChoice::SetStringSelection( const wxString &string )
{
    wxCHECK_RET( m_widget != NULL, "invalid choice" );

    int n = FindString( string );
    if (n != -1) SetSelection( n );
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

