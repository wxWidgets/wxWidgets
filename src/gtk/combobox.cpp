/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

#include "wx/combobox.h"

#if wxUSE_COMBOBOX

#include "wx/settings.h"
#include "wx/intl.h"

#include "wx/textctrl.h"    // for wxEVT_COMMAND_TEXT_UPDATED

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
// "select-child" - click/cursor get select-child, changed, select-child
//-----------------------------------------------------------------------------

static void
gtk_combo_select_child_callback( GtkList *WXUNUSED(list), GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!combo->m_hasVMT) return;
    
    if (g_blockEventsOnDrag) return;

    int curSelection = combo->GetSelection();
    
    if (combo->m_prevSelection != curSelection)
    {
        GtkWidget *list = GTK_COMBO(combo->m_widget)->list;
        gtk_list_unselect_item( GTK_LIST(list), combo->m_prevSelection );
    }
    combo->m_prevSelection = curSelection;

    wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, combo->GetId() );
    event.SetInt( curSelection );
    event.SetString( combo->GetStringSelection() );
    event.SetEventObject( combo );

    combo->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
//  "changed" - typing and list item matches get changed, select-child
//              if it doesn't match an item then just get a single changed
//-----------------------------------------------------------------------------

static void
gtk_text_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!combo->m_hasVMT) return;

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
    event.SetString( combo->GetValue() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}

static void
gtk_dummy_callback(GtkEntry *WXUNUSED(entry), GtkCombo *WXUNUSED(combo))
{
}


//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

BEGIN_EVENT_TABLE(wxComboBox, wxControl)
    EVT_SIZE(wxComboBox::OnSize)
    EVT_CHAR(wxComboBox::OnChar)
END_EVENT_TABLE()

bool wxComboBox::Create( wxWindow *parent, wxWindowID id, const wxString& value,
                         const wxPoint& pos, const wxSize& size,
                         int n, const wxString choices[],
                         long style, const wxValidator& validator,
                         const wxString& name )
{
    m_alreadySent = FALSE;
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
    m_prevSelection = 0;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxComboBox creation failed") );
        return FALSE;
    }

    m_widget = gtk_combo_new();
    GtkCombo *combo = GTK_COMBO(m_widget);
    
    // Disable GTK's broken events ...
    gtk_signal_disconnect( GTK_OBJECT(combo->entry), combo->entry_change_id );
    // ... and add surogate handler.
    combo->entry_change_id = gtk_signal_connect (GTK_OBJECT (combo->entry), "changed",
			      (GtkSignalFunc) gtk_dummy_callback, combo);

    // make it more useable
    gtk_combo_set_use_arrows_always( GTK_COMBO(m_widget), TRUE );
    
    // and case-sensitive
    gtk_combo_set_case_sensitive( GTK_COMBO(m_widget), TRUE );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

#ifndef __WXGTK20__
    // gtk_list_set_selection_mode( GTK_LIST(list), GTK_SELECTION_MULTIPLE );
#endif

    for (int i = 0; i < n; i++)
    {
        GtkWidget *list_item = gtk_list_item_new_with_label( wxGTK_CONV( choices[i] ) );

        m_clientDataList.Append( (wxObject*)NULL );
        m_clientObjectList.Append( (wxObject*)NULL );

        gtk_container_add( GTK_CONTAINER(list), list_item );

        gtk_widget_show( list_item );
    }

    m_parent->DoAddChild( this );
    
    m_focusWidget = combo->entry;

    PostCreation();

    ConnectWidget( combo->button );

    // MSW's combo box shows the value and the selection is -1
    gtk_entry_set_text( GTK_ENTRY(combo->entry), wxGTK_CONV(value) );
    gtk_list_unselect_all( GTK_LIST(combo->list) );

    if (style & wxCB_READONLY)
        gtk_entry_set_editable( GTK_ENTRY( combo->entry ), FALSE );

    gtk_signal_connect( GTK_OBJECT(combo->entry), "changed",
      GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(combo->list), "select-child",
      GTK_SIGNAL_FUNC(gtk_combo_select_child_callback), (gpointer)this );

    wxSize size_best( DoGetBestSize() );
    wxSize new_size( size );
    if (new_size.x == -1)
        new_size.x = size_best.x;
    if (new_size.y == -1)
        new_size.y = size_best.y;
    if (new_size.y > size_best.y)
        new_size.y = size_best.y;
    if ((new_size.x != size.x) || (new_size.y != size.y))
    {
        SetSize( new_size.x, new_size.y );
        
        // This is required for tool bar support
        gtk_widget_set_usize( m_widget, new_size.x, new_size.y );
    }

    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
    SetForegroundColour( parent->GetForegroundColour() );

    Show( TRUE );

    return TRUE;
}

wxComboBox::~wxComboBox()
{
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

void wxComboBox::SetFocus()
{
    if ( m_hasFocus )
    {
        // don't do anything if we already have focus
        return;
    }

    gtk_widget_grab_focus( m_focusWidget );
}

void wxComboBox::AppendCommon( const wxString &item )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();
    
    GtkWidget *list = GTK_COMBO(m_widget)->list;

    GtkWidget *list_item = gtk_list_item_new_with_label( wxGTK_CONV( item ) );

    gtk_container_add( GTK_CONTAINER(list), list_item );

    if (GTK_WIDGET_REALIZED(m_widget))
    {
        gtk_widget_realize( list_item );
        gtk_widget_realize( GTK_BIN(list_item)->child );

        if (m_widgetStyle) ApplyWidgetStyle();
    }

    gtk_widget_show( list_item );
    
    EnableEvents();
}

void wxComboBox::Append( const wxString &item )
{
    m_clientDataList.Append( (wxObject*) NULL );
    m_clientObjectList.Append( (wxObject*) NULL );

    AppendCommon( item );
}

void wxComboBox::Append( const wxString &item, void *clientData )
{
    m_clientDataList.Append( (wxObject*) clientData );
    m_clientObjectList.Append( (wxObject*)NULL );

    AppendCommon( item );
}

void wxComboBox::Append( const wxString &item, wxClientData *clientData )
{
    m_clientDataList.Append( (wxObject*) NULL );
    m_clientObjectList.Append( (wxObject*) clientData );

    AppendCommon( item );
}

void wxComboBox::SetClientData( int n, void* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return;

    node->SetData( (wxObject*) clientData );
}

void* wxComboBox::GetClientData( int n )
{
    wxCHECK_MSG( m_widget != NULL, NULL, wxT("invalid combobox") );

    wxNode *node = m_clientDataList.Nth( n );
    if (!node) return NULL;

    return node->Data();
}

void wxComboBox::SetClientObject( int n, wxClientData* clientData )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return;

    wxClientData *cd = (wxClientData*) node->Data();
    if (cd) delete cd;

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxComboBox::GetClientObject( int n )
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*)NULL, wxT("invalid combobox") );

    wxNode *node = m_clientObjectList.Nth( n );
    if (!node) return (wxClientData*) NULL;

    return (wxClientData*) node->Data();
}

void wxComboBox::Clear()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();
    
    GtkWidget *list = GTK_COMBO(m_widget)->list;
    gtk_list_clear_items( GTK_LIST(list), 0, Number() );

    wxNode *node = m_clientObjectList.First();
    while (node)
    {
        wxClientData *cd = (wxClientData*)node->Data();
        if (cd) delete cd;
        node = node->Next();
    }
    m_clientObjectList.Clear();

    m_clientDataList.Clear();
    
    EnableEvents();
}

void wxComboBox::Delete( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkList *listbox = GTK_LIST( GTK_COMBO(m_widget)->list );

    GList *child = g_list_nth( listbox->children, n );

    if (!child)
    {
        wxFAIL_MSG(wxT("wrong index"));
        return;
    }

    DisableEvents();
    
    GList *list = g_list_append( (GList*) NULL, child->data );
    gtk_list_remove_items( listbox, list );
    g_list_free( list );

    wxNode *node = m_clientObjectList.Nth( n );
    if (node)
    {
        wxClientData *cd = (wxClientData*)node->Data();
        if (cd) delete cd;
        m_clientObjectList.DeleteNode( node );
    }

    node = m_clientDataList.Nth( n );
    if (node)
        m_clientDataList.DeleteNode( node );
    
    EnableEvents();
}

int wxComboBox::FindString( const wxString &item )
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

    GList *child = GTK_LIST(list)->children;
    int count = 0;
    while (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );
#ifdef __WXGTK20__
        wxString str( wxGTK_CONV_BACK( gtk_label_get_text(label) ) );
#else
        wxString str( label->label );
#endif
        if (item == str)
            return count;
            
        count++;
        child = child->next;
    }

    return wxNOT_FOUND;
}

int wxComboBox::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

    GList *selection = GTK_LIST(list)->selection;
    if (selection)
    {
        GList *child = GTK_LIST(list)->children;
        int count = 0;
        while (child)
        {
            if (child->data == selection->data) return count;
            count++;
            child = child->next;
        }
    }

    return -1;
}

wxString wxComboBox::GetString( int n ) const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid combobox") );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

    wxString str;
    GList *child = g_list_nth( GTK_LIST(list)->children, n );
    if (child)
    {
        GtkBin *bin = GTK_BIN( child->data );
        GtkLabel *label = GTK_LABEL( bin->child );
#ifdef __WXGTK20__
        str = wxGTK_CONV_BACK( gtk_label_get_text(label) );
#else
        str = wxString( label->label );
#endif
    }
    else
    {
        wxFAIL_MSG( wxT("wxComboBox: wrong index") );
    }

    return str;
}

wxString wxComboBox::GetStringSelection() const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid combobox") );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

    GList *selection = GTK_LIST(list)->selection;
    if (selection)
    {
        GtkBin *bin = GTK_BIN( selection->data );
        GtkLabel *label = GTK_LABEL( bin->child );
#ifdef __WXGTK20__
        wxString tmp( wxGTK_CONV_BACK( gtk_label_get_text(label) ) );
#else
        wxString tmp( label->label );
#endif
        return tmp;
    }

    wxFAIL_MSG( wxT("wxComboBox: no selection") );

    return wxT("");
}

int wxComboBox::Number() const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid combobox") );

    GtkWidget *list = GTK_COMBO(m_widget)->list;

    GList *child = GTK_LIST(list)->children;
    int count = 0;
    while (child) { count++; child = child->next; }
    return count;
}

void wxComboBox::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();

    GtkWidget *list = GTK_COMBO(m_widget)->list;
    gtk_list_unselect_item( GTK_LIST(list), m_prevSelection );
    gtk_list_select_item( GTK_LIST(list), n );
    m_prevSelection = n;

    EnableEvents();
}

void wxComboBox::SetStringSelection( const wxString &string )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    int res = FindString( string );
    if (res == -1) return;
    SetSelection( res );
}

wxString wxComboBox::GetValue() const
{
    GtkEntry *entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );
    wxString tmp( wxGTK_CONV_BACK( gtk_entry_get_text( entry ) ) );

#if 0    
    for (int i = 0; i < wxStrlen(tmp.c_str()) +1; i++)
    {
        wxChar c = tmp[i];
        printf( "%d ", (int) (c) );
    }
    printf( "\n" );
#endif
    
    return tmp;
}

void wxComboBox::SetValue( const wxString& value )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    wxString tmp = wxT("");
    if (!value.IsNull()) tmp = value;
    gtk_entry_set_text( GTK_ENTRY(entry), wxGTK_CONV( tmp ) );
}

void wxComboBox::Copy()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_copy_clipboard( GTK_EDITABLE(entry) DUMMY_CLIPBOARD_ARG );
}

void wxComboBox::Cut()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_cut_clipboard( GTK_EDITABLE(entry) DUMMY_CLIPBOARD_ARG );
}

void wxComboBox::Paste()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_paste_clipboard( GTK_EDITABLE(entry) DUMMY_CLIPBOARD_ARG);
}

void wxComboBox::SetInsertionPoint( long pos )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_entry_set_position( GTK_ENTRY(entry), (int)pos );
}

void wxComboBox::SetInsertionPointEnd()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    SetInsertionPoint( -1 );
}

long wxComboBox::GetInsertionPoint() const
{
    return (long) GET_EDITABLE_POS( GTK_COMBO(m_widget)->entry );
}

long wxComboBox::GetLastPosition() const
{
    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    int pos = GTK_ENTRY(entry)->text_length;
    return (long) pos-1;
}

void wxComboBox::Replace( long from, long to, const wxString& value )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
    if (value.IsNull()) return;
    gint pos = (gint)to;
    
#if wxUSE_UNICODE
    wxCharBuffer buffer = wxConvUTF8.cWX2MB( value );
    gtk_editable_insert_text( GTK_EDITABLE(entry), (const char*) buffer, strlen( (const char*) buffer ), &pos );
#else
    gtk_editable_insert_text( GTK_EDITABLE(entry), value.c_str(), value.Length(), &pos );
#endif
}

void wxComboBox::Remove(long from, long to)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
}

void wxComboBox::SetSelection( long from, long to )
{
    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_editable_select_region( GTK_EDITABLE(entry), (gint)from, (gint)to );
}

void wxComboBox::SetEditable( bool editable )
{
    GtkWidget *entry = GTK_COMBO(m_widget)->entry;
    gtk_entry_set_editable( GTK_ENTRY(entry), editable );
}

void wxComboBox::OnChar( wxKeyEvent &event )
{
    if ( event.KeyCode() == WXK_RETURN )
    {
        // GTK automatically selects an item if its in the list
        wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, GetId());
        event.SetString( GetValue() );
        event.SetInt( GetSelection() );
        event.SetEventObject( this );
        
        if (!GetEventHandler()->ProcessEvent( event ))
        {
            // This will invoke the dialog default action, such
            // as the clicking the default button.

            wxWindow *top_frame = m_parent;
            while (top_frame->GetParent() && !(top_frame->IsTopLevel()))
                top_frame = top_frame->GetParent();

            if (top_frame && GTK_IS_WINDOW(top_frame->m_widget))
            {
                GtkWindow *window = GTK_WINDOW(top_frame->m_widget);

                if (window->default_widget)
                    gtk_widget_activate (window->default_widget);
            }
        }
            
        // Catch GTK event so that GTK doesn't open the drop
        // down list upon RETURN.
        return;
    }
    
    event.Skip();
}

void wxComboBox::DisableEvents()
{
    gtk_signal_disconnect_by_func( GTK_OBJECT(GTK_COMBO(m_widget)->list),
      GTK_SIGNAL_FUNC(gtk_combo_select_child_callback), (gpointer)this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(GTK_COMBO(m_widget)->entry),
      GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this );
}

void wxComboBox::EnableEvents()
{
    gtk_signal_connect( GTK_OBJECT(GTK_COMBO(m_widget)->list), "select-child",
      GTK_SIGNAL_FUNC(gtk_combo_select_child_callback), (gpointer)this );
    gtk_signal_connect( GTK_OBJECT(GTK_COMBO(m_widget)->entry), "changed",
      GTK_SIGNAL_FUNC(gtk_text_changed_callback), (gpointer)this );
}

void wxComboBox::OnSize( wxSizeEvent &event )
{
    event.Skip();

#if 0
    int w = 21;
    gtk_widget_set_usize( GTK_COMBO(m_widget)->entry, m_width-w-1, m_height );

    gtk_widget_set_uposition( GTK_COMBO(m_widget)->button, m_x+m_width-w, m_y );
    gtk_widget_set_usize( GTK_COMBO(m_widget)->button, w, m_height );
#endif // 0
}

void wxComboBox::ApplyWidgetStyle()
{
    SetWidgetStyle();

//    gtk_widget_set_style( GTK_COMBO(m_widget)->button, m_widgetStyle );
    gtk_widget_set_style( GTK_COMBO(m_widget)->entry, m_widgetStyle );
    gtk_widget_set_style( GTK_COMBO(m_widget)->list, m_widgetStyle );

    GtkList *list = GTK_LIST( GTK_COMBO(m_widget)->list );
    GList *child = list->children;
    while (child)
    {
        gtk_widget_set_style( GTK_WIDGET(child->data), m_widgetStyle );

        GtkBin *bin = GTK_BIN(child->data);
        gtk_widget_set_style( bin->child, m_widgetStyle );

        child = child->next;
    }
}

GtkWidget* wxComboBox::GetConnectWidget()
{
    return GTK_COMBO(m_widget)->entry;
}

bool wxComboBox::IsOwnGtkWindow( GdkWindow *window )
{
    return ( (window == GTK_ENTRY( GTK_COMBO(m_widget)->entry )->text_area) ||
             (window == GTK_COMBO(m_widget)->button->window ) );
}

wxSize wxComboBox::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );

    // we know better our horizontal extent: it depends on the longest string
    // in the combobox
    ret.x = 0;
    if ( m_widget )
    {
        int width;
        size_t count = GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            GetTextExtent( GetString(n), &width, NULL, NULL, NULL, &m_font );
            if ( width > ret.x )
                ret.x = width;
        }
    }

    // empty combobox should have some reasonable default size too
    if ( ret.x < 100 )
        ret.x = 100;
    return ret;
}

#endif

