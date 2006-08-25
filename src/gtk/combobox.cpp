/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/combobox.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"    // for wxEVT_COMMAND_TEXT_UPDATED
    #include "wx/arrstr.h"
#endif

// We use GtkCombo which has been deprecated since GTK+ 2.3.0
// in favour of GtkComboBox for <GTK2.4 runtime
// We also use GtkList
#ifdef GTK_DISABLE_DEPRECATED
#undef GTK_DISABLE_DEPRECATED
#endif
#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;
static int    g_SelectionBeforePopup = wxID_NONE; // this means the popup is hidden

//-----------------------------------------------------------------------------
//  "changed" - typing and list item matches get changed, select-child
//              if it doesn't match an item then just get a single changed
//-----------------------------------------------------------------------------

extern "C" {
static void
gtkcombo_text_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (combo->m_ignoreNextUpdate)
    {
        combo->m_ignoreNextUpdate = false;
        return;
    }

    if (!combo->m_hasVMT) return;

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
    event.SetString( combo->GetValue() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}
}

extern "C" {
static void
gtkcombo_dummy_callback(GtkEntry *WXUNUSED(entry), GtkCombo *WXUNUSED(combo))
{
}
}

extern "C" {
static void
gtkcombo_popup_hide_callback(GtkCombo *WXUNUSED(gtk_combo), wxComboBox *combo)
{
    // when the popup is hidden, throw a SELECTED event only if the combobox
    // selection changed.
    const int curSelection = combo->GetCurrentSelection();

    const bool hasChanged = curSelection != g_SelectionBeforePopup;

    // reset the selection flag to value meaning that it is hidden and do it
    // now, before generating the events, so that GetSelection() returns the
    // new value from the event handler
    g_SelectionBeforePopup = wxID_NONE;

    if ( hasChanged )
    {
        wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, combo->GetId() );
        event.SetInt( curSelection );
        event.SetString( combo->GetStringSelection() );
        event.SetEventObject( combo );
        combo->GetEventHandler()->ProcessEvent( event );

        // for consistency with the other ports, send TEXT event
        wxCommandEvent event2( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
        event2.SetString( combo->GetStringSelection() );
        event2.SetEventObject( combo );
        combo->GetEventHandler()->ProcessEvent( event2 );
    }
}
}

extern "C" {
static void
gtkcombo_popup_show_callback(GtkCombo *WXUNUSED(gtk_combo), wxComboBox *combo)
{
    // store the combobox selection value before the popup is shown
    g_SelectionBeforePopup = combo->GetCurrentSelection();
}
}

//-----------------------------------------------------------------------------
// "select-child" - click/cursor get select-child, changed, select-child
//-----------------------------------------------------------------------------

extern "C" {
static void
gtkcombo_combo_select_child_callback( GtkList *WXUNUSED(list), GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!combo->m_hasVMT) return;

    if (g_blockEventsOnDrag) return;

    int curSelection = combo->GetCurrentSelection();

    if (combo->m_prevSelection == curSelection) return;

    GtkWidget *list = GTK_COMBO(combo->m_widget)->list;
    gtk_list_unselect_item( GTK_LIST(list), combo->m_prevSelection );

    combo->m_prevSelection = curSelection;

    // Quickly set the value of the combo box
    // as GTK+ does that only AFTER the event
    // is sent.
    g_signal_handlers_disconnect_by_func (GTK_COMBO (combo->GetHandle())->entry,
                                          (gpointer) gtkcombo_text_changed_callback,
                                          combo);
    combo->SetValue( combo->GetStringSelection() );
    g_signal_connect_after (GTK_COMBO (combo->GetHandle())->entry, "changed",
                            G_CALLBACK (gtkcombo_text_changed_callback), combo);

    // throw a SELECTED event only if the combobox popup is hidden (wxID_NONE)
    // because when combobox popup is shown, gtkcombo_combo_select_child_callback is
    // called each times the mouse is over an item with a pressed button so a lot
    // of SELECTED event could be generated if the user keep the mouse button down
    // and select other items ...
    if (g_SelectionBeforePopup == wxID_NONE)
    {
        wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, combo->GetId() );
        event.SetInt( curSelection );
        event.SetString( combo->GetStringSelection() );
        event.SetEventObject( combo );
        combo->GetEventHandler()->ProcessEvent( event );

        // for consistency with the other ports, don't generate text update
        // events while the user is browsing the combobox neither
        wxCommandEvent event2( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
        event2.SetString( combo->GetValue() );
        event2.SetEventObject( combo );
        combo->GetEventHandler()->ProcessEvent( event2 );
    }
}
}

#ifdef __WXGTK24__
extern "C" {
static void
gtkcombobox_text_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!combo->m_hasVMT) return;

    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, combo->GetId() );
    event.SetString( combo->GetValue() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}
}

extern "C" {
static void
gtkcombobox_changed_callback( GtkWidget *WXUNUSED(widget), wxComboBox *combo )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (!combo->m_hasVMT) return;

    wxCommandEvent event( wxEVT_COMMAND_COMBOBOX_SELECTED, combo->GetId() );
    event.SetInt( combo->GetSelection() );
    event.SetString( combo->GetStringSelection() );
    event.SetEventObject( combo );
    combo->GetEventHandler()->ProcessEvent( event );
}
}
#endif

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox,wxControl)

BEGIN_EVENT_TABLE(wxComboBox, wxControl)
    EVT_SIZE(wxComboBox::OnSize)
    EVT_CHAR(wxComboBox::OnChar)

    EVT_MENU(wxID_CUT, wxComboBox::OnCut)
    EVT_MENU(wxID_COPY, wxComboBox::OnCopy)
    EVT_MENU(wxID_PASTE, wxComboBox::OnPaste)
    EVT_MENU(wxID_UNDO, wxComboBox::OnUndo)
    EVT_MENU(wxID_REDO, wxComboBox::OnRedo)
    EVT_MENU(wxID_CLEAR, wxComboBox::OnDelete)
    EVT_MENU(wxID_SELECTALL, wxComboBox::OnSelectAll)

    EVT_UPDATE_UI(wxID_CUT, wxComboBox::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxComboBox::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxComboBox::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxComboBox::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxComboBox::OnUpdateRedo)
    EVT_UPDATE_UI(wxID_CLEAR, wxComboBox::OnUpdateDelete)
    EVT_UPDATE_UI(wxID_SELECTALL, wxComboBox::OnUpdateSelectAll)
END_EVENT_TABLE()

bool wxComboBox::Create( wxWindow *parent, wxWindowID id,
                         const wxString& value,
                         const wxPoint& pos, const wxSize& size,
                         const wxArrayString& choices,
                         long style, const wxValidator& validator,
                         const wxString& name )
{
    wxCArrayString chs(choices);

    return Create( parent, id, value, pos, size, chs.GetCount(),
                   chs.GetStrings(), style, validator, name );
}

bool wxComboBox::Create( wxWindow *parent, wxWindowID id, const wxString& value,
                         const wxPoint& pos, const wxSize& size,
                         int n, const wxString choices[],
                         long style, const wxValidator& validator,
                         const wxString& name )
{
    m_ignoreNextUpdate = false;
    m_needParent = true;
    m_acceptsFocus = true;
    m_prevSelection = 0;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxComboBox creation failed") );
        return false;
    }

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        m_widget = gtk_combo_box_entry_new_text();
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );

        gtk_entry_set_editable( GTK_ENTRY( GTK_BIN(m_widget)->child ), TRUE );

        for (int i = 0; i < n; i++)
        {
            gtk_combo_box_append_text( combobox,  wxGTK_CONV( choices[i] ) );

            m_clientDataList.Append( (wxObject*)NULL );
            m_clientObjectList.Append( (wxObject*)NULL );
        }
    }
    else
#endif
    {
        m_widget = gtk_combo_new();
        GtkCombo* combo = GTK_COMBO(m_widget);

        // Disable GTK's broken events ...
        g_signal_handler_disconnect (combo->entry, combo->entry_change_id);
        // ... and add surrogate handler.
        combo->entry_change_id = g_signal_connect (combo->entry, "changed",
                                               G_CALLBACK (gtkcombo_dummy_callback),
                                               combo);

        // make it more useable
        gtk_combo_set_use_arrows_always( GTK_COMBO(m_widget), TRUE );

        // and case-sensitive
        gtk_combo_set_case_sensitive( GTK_COMBO(m_widget), TRUE );

        if (style & wxNO_BORDER)
            g_object_set (combo->entry, "has-frame", FALSE, NULL );

        GtkWidget *list = combo->list;

        for (int i = 0; i < n; i++)
        {
            GtkWidget *list_item = gtk_list_item_new_with_label( wxGTK_CONV( choices[i] ) );

            m_clientDataList.Append( (wxObject*)NULL );
            m_clientObjectList.Append( (wxObject*)NULL );

            gtk_container_add( GTK_CONTAINER(list), list_item );

            gtk_widget_show( list_item );
        }
    }


    m_parent->DoAddChild( this );

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    m_focusWidget = GTK_WIDGET( entry );

    PostCreation(size);

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        ConnectWidget( m_widget );
    else
#endif
        ConnectWidget( GTK_COMBO(m_widget)->button );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        gtk_entry_set_text( entry, wxGTK_CONV(value) );

        if (style & wxCB_READONLY)
            gtk_entry_set_editable( entry, FALSE );

        g_signal_connect_after (entry, "changed",
                            G_CALLBACK (gtkcombobox_text_changed_callback), this);

        g_signal_connect_after (m_widget, "changed",
                            G_CALLBACK (gtkcombobox_changed_callback), this);
    }
    else
#endif
    {
        GtkCombo *combo = GTK_COMBO(m_widget);
        // MSW's combo box shows the value and the selection is -1
        gtk_entry_set_text( entry, wxGTK_CONV(value) );
        gtk_list_unselect_all( GTK_LIST(combo->list) );

        if (style & wxCB_READONLY)
            gtk_entry_set_editable( entry, FALSE );

        // "show" and "hide" events are generated when user click on the combobox button which popups a list
        // this list is the "popwin" gtk widget
        g_signal_connect (GTK_COMBO(combo)->popwin, "hide",
                      G_CALLBACK (gtkcombo_popup_hide_callback), this);
        g_signal_connect (GTK_COMBO(combo)->popwin, "show",
                      G_CALLBACK (gtkcombo_popup_show_callback), this);
        g_signal_connect_after (combo->list, "select-child",
                            G_CALLBACK (gtkcombo_combo_select_child_callback),
                            this);
        g_signal_connect_after (entry, "changed",
                            G_CALLBACK (gtkcombo_text_changed_callback), this);

        // This is required for tool bar support
        // Doesn't currently work
//        wxSize setsize = GetSize();
//        gtk_widget_set_size_request( m_widget, setsize.x, setsize.y );
    }

    SetBestSize(size); // need this too because this is a wxControlWithItems


    return true;
}

wxComboBox::~wxComboBox()
{
    wxList::compatibility_iterator node = m_clientObjectList.GetFirst();
    while (node)
    {
        wxClientData *cd = (wxClientData*)node->GetData();
        if (cd) delete cd;
        node = node->GetNext();
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

int wxComboBox::DoAppend( const wxString &item )
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        gtk_combo_box_append_text( combobox,  wxGTK_CONV( item ) );
    }
    else
#endif
    {
        DisableEvents();

        GtkWidget *list = GTK_COMBO(m_widget)->list;
        GtkWidget *list_item = gtk_list_item_new_with_label( wxGTK_CONV( item ) );

        gtk_container_add( GTK_CONTAINER(list), list_item );

        if (GTK_WIDGET_REALIZED(m_widget))
        {
            gtk_widget_realize( list_item );
            gtk_widget_realize( GTK_BIN(list_item)->child );
        }

        // Apply current widget style to the new list_item
        GtkRcStyle *style = CreateWidgetStyle();
        if (style)
        {
            gtk_widget_modify_style( GTK_WIDGET( list_item ), style );
            GtkBin *bin = GTK_BIN( list_item );
            GtkWidget *label = GTK_WIDGET( bin->child );
            gtk_widget_modify_style( label, style );
            gtk_rc_style_unref( style );
        }

        gtk_widget_show( list_item );

        EnableEvents();
    }

    const unsigned int count = GetCount();

    if ( m_clientDataList.GetCount() < count )
        m_clientDataList.Append( (wxObject*) NULL );
    if ( m_clientObjectList.GetCount() < count )
        m_clientObjectList.Append( (wxObject*) NULL );

    InvalidateBestSize();

    return count - 1;
}

int wxComboBox::DoInsert(const wxString &item, unsigned int pos)
{
    wxCHECK_MSG( !(GetWindowStyle() & wxCB_SORT), -1,
                    wxT("can't insert into sorted list"));

    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );
    wxCHECK_MSG( IsValidInsert(pos), -1, wxT("invalid index") );

    unsigned int count = GetCount();

    if (pos == count)
        return Append(item);

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        gtk_combo_box_insert_text( combobox, pos, wxGTK_CONV( item ) );
    }
    else
#endif
    {
        DisableEvents();

        GtkWidget *list = GTK_COMBO(m_widget)->list;
        GtkWidget *list_item = gtk_list_item_new_with_label( wxGTK_CONV( item ) );

        GList *gitem_list = g_list_alloc ();
        gitem_list->data = list_item;
        gtk_list_insert_items( GTK_LIST (list), gitem_list, pos );

        if (GTK_WIDGET_REALIZED(m_widget))
        {
            gtk_widget_realize( list_item );
            gtk_widget_realize( GTK_BIN(list_item)->child );

            ApplyWidgetStyle();
        }

        gtk_widget_show( list_item );

        EnableEvents();
    }

    count = GetCount();

    if ( m_clientDataList.GetCount() < count )
        m_clientDataList.Insert( pos, (wxObject*) NULL );
    if ( m_clientObjectList.GetCount() < count )
        m_clientObjectList.Insert( pos, (wxObject*) NULL );

    InvalidateBestSize();

    return pos;
}

void wxComboBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    wxList::compatibility_iterator node = m_clientDataList.Item( n );
    if (!node) return;

    node->SetData( (wxObject*) clientData );
}

void* wxComboBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( m_widget != NULL, NULL, wxT("invalid combobox") );

    wxList::compatibility_iterator node = m_clientDataList.Item( n );

    return node ? node->GetData() : NULL;
}

void wxComboBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    wxList::compatibility_iterator node = m_clientObjectList.Item( n );
    if (!node) return;

    // wxItemContainer already deletes data for us

    node->SetData( (wxObject*) clientData );
}

wxClientData* wxComboBox::DoGetItemClientObject(unsigned int n) const
{
    wxCHECK_MSG( m_widget != NULL, (wxClientData*)NULL, wxT("invalid combobox") );

    wxList::compatibility_iterator node = m_clientObjectList.Item( n );

    return node ? (wxClientData*) node->GetData() : NULL;
}

void wxComboBox::Clear()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        const unsigned int count = GetCount();
        for (unsigned int i = 0; i < count; i++)
            gtk_combo_box_remove_text( combobox, 0 );
    }
    else // GTK+ < 2.4.0
#endif // __WXGTK24__
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;
        gtk_list_clear_items( GTK_LIST(list), 0, GetCount() );
    }

    wxList::compatibility_iterator node = m_clientObjectList.GetFirst();
    while (node)
    {
        wxClientData *cd = (wxClientData*)node->GetData();
        delete cd;
        node = node->GetNext();
    }
    m_clientObjectList.Clear();

    m_clientDataList.Clear();

    EnableEvents();

    InvalidateBestSize();
}

void wxComboBox::Delete(unsigned int n)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        wxCHECK_RET( IsValid(n), wxT("invalid index") );

        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        gtk_combo_box_remove_text( combobox, n );
    }
    else
#endif
    {
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

        EnableEvents();
    }

    wxList::compatibility_iterator node = m_clientObjectList.Item( n );
    if (node)
    {
        wxClientData *cd = (wxClientData*)node->GetData();
        if (cd) delete cd;
        m_clientObjectList.Erase( node );
    }

    node = m_clientDataList.Item( n );
    if (node)
        m_clientDataList.Erase( node );

    InvalidateBestSize();
}

void wxComboBox::SetString(unsigned int n, const wxString &text)
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        wxCHECK_RET( IsValid(n), wxT("invalid index") );

        GtkTreeModel *model = gtk_combo_box_get_model( combobox );
        GtkTreeIter iter;
        if (gtk_tree_model_iter_nth_child (model, &iter, NULL, n))
        {
            GValue value = { 0, };
            g_value_init( &value, G_TYPE_STRING );
            g_value_set_string( &value, wxGTK_CONV( text ) );
            gtk_list_store_set_value( GTK_LIST_STORE(model), &iter, 0, &value );
            g_value_unset( &value );
        }
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;

        GList *child = g_list_nth( GTK_LIST(list)->children, n );
        if (child)
        {
            GtkBin *bin = GTK_BIN( child->data );
            GtkLabel *label = GTK_LABEL( bin->child );
            gtk_label_set_text(label, wxGTK_CONV(text));
        }
        else
        {
            wxFAIL_MSG( wxT("wxComboBox: wrong index") );
        }
    }

    InvalidateBestSize();
}

int wxComboBox::FindString( const wxString &item, bool bCase ) const
{
    wxCHECK_MSG( m_widget != NULL, wxNOT_FOUND, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        GtkTreeModel* model = gtk_combo_box_get_model( combobox );
        GtkTreeIter iter;
        gtk_tree_model_get_iter_first( model, &iter );
        if (!gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter ))
            return -1;
        int count = 0;
        do
        {
            GValue value = { 0, };
            gtk_tree_model_get_value( model, &iter, 0, &value );
            wxString str = wxGTK_CONV_BACK( g_value_get_string( &value ) );
            g_value_unset( &value );

            if (item.IsSameAs( str, bCase ) )
                return count;

            count++;

        } while (gtk_tree_model_iter_next( model, &iter ));
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;

        GList *child = GTK_LIST(list)->children;
        int count = 0;
        while (child)
        {
            GtkBin *bin = GTK_BIN( child->data );
            GtkLabel *label = GTK_LABEL( bin->child );
            wxString str( wxGTK_CONV_BACK( gtk_label_get_text(label) ) );

            if (item.IsSameAs( str , bCase ) )
                return count;

            count++;
            child = child->next;
        }
    }

    return wxNOT_FOUND;
}

int wxComboBox::GetSelection() const
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        return gtk_combo_box_get_active( combobox );
    }
    else
#endif
        // if the popup is currently opened, use the selection as it had been
        // before it dropped down
        return g_SelectionBeforePopup == wxID_NONE ? GetCurrentSelection()
                                               : g_SelectionBeforePopup;
}

int wxComboBox::GetCurrentSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        return gtk_combo_box_get_active( combobox );
    }
    else
#endif
    {
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
    }

    return -1;
}

wxString wxComboBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( m_widget != NULL, wxEmptyString, wxT("invalid combobox") );

    wxString str;

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        GtkTreeModel *model = gtk_combo_box_get_model( combobox );
        GtkTreeIter iter;
        if (gtk_tree_model_iter_nth_child (model, &iter, NULL, n))
        {
            GValue value = { 0, };
            gtk_tree_model_get_value( model, &iter, 0, &value );
            wxString tmp = wxGTK_CONV_BACK( g_value_get_string( &value ) );
            g_value_unset( &value );
            return tmp;
        }
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;

        GList *child = g_list_nth( GTK_LIST(list)->children, n );
        if (child)
        {
            GtkBin *bin = GTK_BIN( child->data );
            GtkLabel *label = GTK_LABEL( bin->child );
            str = wxGTK_CONV_BACK( gtk_label_get_text(label) );
        }
        else
        {
            wxFAIL_MSG( wxT("wxComboBox: wrong index") );
        }
    }

    return str;
}

wxString wxComboBox::GetStringSelection() const
{
    wxCHECK_MSG( m_widget != NULL, wxEmptyString, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        int sel = gtk_combo_box_get_active( combobox );
        if (sel == -1)
            return wxEmptyString;
        return GetString(sel);
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;

        GList *selection = GTK_LIST(list)->selection;
        if (selection)
        {
            GtkBin *bin = GTK_BIN( selection->data );
            GtkLabel *label = GTK_LABEL( bin->child );
            wxString tmp( wxGTK_CONV_BACK( gtk_label_get_text(label) ) );
            return tmp;
        }

        wxFAIL_MSG( wxT("wxComboBox: no selection") );
    }

    return wxEmptyString;
}

unsigned int wxComboBox::GetCount() const
{
    wxCHECK_MSG( m_widget != NULL, 0, wxT("invalid combobox") );

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        GtkTreeModel* model = gtk_combo_box_get_model( combobox );
        GtkTreeIter iter;
        gtk_tree_model_get_iter_first( model, &iter );
        if (!gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter ))
            return 0;
        unsigned int ret = 1;
        while (gtk_tree_model_iter_next( model, &iter ))
            ret++;
        return ret;
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;

        GList *child = GTK_LIST(list)->children;
        unsigned int count = 0;
        while (child)
        {
            count++;
            child = child->next;
        }
        return count;
    }

    return 0;
}

void wxComboBox::SetSelection( int n )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    DisableEvents();

#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        gtk_combo_box_set_active( combobox, n );
    }
    else
#endif
    {
        GtkWidget *list = GTK_COMBO(m_widget)->list;
        gtk_list_unselect_item( GTK_LIST(list), m_prevSelection );
        gtk_list_select_item( GTK_LIST(list), n );
        m_prevSelection = n;
    }

    EnableEvents();
}

wxString wxComboBox::GetValue() const
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

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

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    wxString tmp;
    if (!value.IsNull()) tmp = value;
    gtk_entry_set_text( entry, wxGTK_CONV( tmp ) );

    InvalidateBestSize();
}

void wxComboBox::Copy()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_editable_copy_clipboard(GTK_EDITABLE(entry));
}

void wxComboBox::Cut()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_editable_cut_clipboard(GTK_EDITABLE(entry));
}

void wxComboBox::Paste()
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_editable_paste_clipboard(GTK_EDITABLE(entry));
}

void wxComboBox::Undo()
{
    // TODO
}

void wxComboBox::Redo()
{
    // TODO
}

void wxComboBox::SelectAll()
{
    SetSelection(0, GetLastPosition());
}

bool wxComboBox::CanUndo() const
{
    // TODO
    return false;
}

bool wxComboBox::CanRedo() const
{
    // TODO
    return false;
}

bool wxComboBox::HasSelection() const
{
    long from, to;
    GetSelection(&from, &to);
    return from != to;
}

bool wxComboBox::CanCopy() const
{
    // Can copy if there's a selection
    return HasSelection();
}

bool wxComboBox::CanCut() const
{
    return CanCopy() && IsEditable();
}

bool wxComboBox::CanPaste() const
{
    // TODO: check for text on the clipboard
    return IsEditable() ;
}

bool wxComboBox::IsEditable() const
{
    return !HasFlag(wxCB_READONLY);
}


void wxComboBox::SetInsertionPoint( long pos )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    if ( pos == GetLastPosition() )
        pos = -1;

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_entry_set_position( entry, (int)pos );
}

long wxComboBox::GetInsertionPoint() const
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    return (long) gtk_editable_get_position(GTK_EDITABLE(entry));
}

wxTextPos wxComboBox::GetLastPosition() const
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    int pos = entry->text_length;
    return (long) pos-1;
}

void wxComboBox::Replace( long from, long to, const wxString& value )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid combobox") );

    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_editable_delete_text( GTK_EDITABLE(entry), (gint)from, (gint)to );
    if (value.IsNull()) return;
    gint pos = (gint)to;

#if wxUSE_UNICODE
    wxCharBuffer buffer = wxConvUTF8.cWX2MB( value );
    gtk_editable_insert_text( GTK_EDITABLE(entry), (const char*) buffer, strlen( (const char*) buffer ), &pos );
#else
    gtk_editable_insert_text( GTK_EDITABLE(entry), value.c_str(), value.length(), &pos );
#endif
}

void wxComboBox::SetSelection( long from, long to )
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_editable_select_region( GTK_EDITABLE(entry), (gint)from, (gint)to );
}

void wxComboBox::GetSelection( long* from, long* to ) const
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    if (IsEditable())
    {
        GtkEditable *editable = GTK_EDITABLE(entry);
        gint start, end;
        gtk_editable_get_selection_bounds(editable, & start, & end);
        *from = start;
        *to = end;
    }
}

void wxComboBox::SetEditable( bool editable )
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    gtk_entry_set_editable( GTK_ENTRY(entry), editable );
}

void wxComboBox::OnChar( wxKeyEvent &event )
{
    if ( event.GetKeyCode() == WXK_RETURN )
    {
        // GTK automatically selects an item if its in the list
        wxCommandEvent eventEnter(wxEVT_COMMAND_TEXT_ENTER, GetId());
        eventEnter.SetString( GetValue() );
        eventEnter.SetInt( GetSelection() );
        eventEnter.SetEventObject( this );

        if (!GetEventHandler()->ProcessEvent( eventEnter ))
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
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        g_signal_handlers_disconnect_by_func (GTK_BIN(m_widget)->child,
                (gpointer)gtkcombobox_text_changed_callback, this);

        g_signal_handlers_disconnect_by_func (m_widget,
                (gpointer)gtkcombobox_changed_callback, this);
    }
    else
#endif
    {
        g_signal_handlers_disconnect_by_func (GTK_COMBO(m_widget)->list,
                (gpointer) gtkcombo_combo_select_child_callback, this);

        g_signal_handlers_disconnect_by_func (GTK_COMBO(m_widget)->entry,
                (gpointer) gtkcombo_text_changed_callback, this);
    }
}

void wxComboBox::EnableEvents()
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        g_signal_connect_after (GTK_BIN(m_widget)->child, "changed",
                            G_CALLBACK (gtkcombobox_text_changed_callback), this);

        g_signal_connect_after (m_widget, "changed",
                            G_CALLBACK (gtkcombobox_changed_callback), this);
    }
    else
#endif
    {
        g_signal_connect_after (GTK_COMBO(m_widget)->list, "select-child",
                            G_CALLBACK (gtkcombo_combo_select_child_callback),
                            this);
        g_signal_connect_after (GTK_COMBO(m_widget)->entry, "changed",
                            G_CALLBACK (gtkcombo_text_changed_callback),
                            this );
    }
}

void wxComboBox::OnSize( wxSizeEvent &event )
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        // Do nothing
    }
    else
#endif
    {
        // NB: In some situations (e.g. on non-first page of a wizard, if the
        //     size used is default size), GtkCombo widget is resized correctly,
        //     but it's look is not updated, it's rendered as if it was much wider.
        //     No other widgets are affected, so it looks like a bug in GTK+.
        //     Manually requesting resize calculation (as gtk_pizza_set_size does)
        //     fixes it.
        if (GTK_WIDGET_VISIBLE(m_widget))
            gtk_widget_queue_resize(m_widget);
    }

    event.Skip();
}

void wxComboBox::DoApplyWidgetStyle(GtkRcStyle *style)
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        // Do nothing
    }
    else
#endif
    {
//    gtk_widget_modify_style( GTK_COMBO(m_widget)->button, syle );

        gtk_widget_modify_style( GTK_COMBO(m_widget)->entry, style );
        gtk_widget_modify_style( GTK_COMBO(m_widget)->list, style );

        GtkList *list = GTK_LIST( GTK_COMBO(m_widget)->list );
        GList *child = list->children;
        while (child)
        {
            gtk_widget_modify_style( GTK_WIDGET(child->data), style );

            GtkBin *bin = GTK_BIN(child->data);
            gtk_widget_modify_style( bin->child, style );

            child = child->next;
        }
    }
}

GtkWidget* wxComboBox::GetConnectWidget()
{
    GtkEntry *entry = NULL;
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        entry = GTK_ENTRY( GTK_BIN(m_widget)->child );
    else
#endif
        entry = GTK_ENTRY( GTK_COMBO(m_widget)->entry );

    return GTK_WIDGET( entry );
}

GdkWindow *wxComboBox::GTKGetWindow(wxArrayGdkWindows& windows) const
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        wxUnusedVar(windows);

        return GTK_ENTRY(GTK_BIN(m_widget)->child)->text_area;
    }
    else
#endif // GTK+ 2.4
    {
        windows.push_back(GTK_ENTRY(GTK_COMBO(m_widget)->entry)->text_area);
        windows.push_back(GTK_COMBO(m_widget)->button->window);

        // indicate that we return multiple windows in the windows array
        return NULL;
    }
}

wxSize wxComboBox::DoGetBestSize() const
{
    wxSize ret( wxControl::DoGetBestSize() );

    // we know better our horizontal extent: it depends on the longest string
    // in the combobox
    if ( m_widget )
    {
        int width;
        unsigned int count = GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            GetTextExtent(GetString(n), &width, NULL, NULL, NULL );
            if ( width > ret.x )
                ret.x = width;
        }
    }

    // empty combobox should have some reasonable default size too
    if ( ret.x < 100 )
        ret.x = 100;

    CacheBestSize(ret);
    return ret;
}

// static
wxVisualAttributes
wxComboBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        return GetDefaultAttributesFromGTKWidget(gtk_combo_box_entry_new, true);
    else
#endif
        return GetDefaultAttributesFromGTKWidget(gtk_combo_new, true);
}

// ----------------------------------------------------------------------------
// standard event handling
// ----------------------------------------------------------------------------

void wxComboBox::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxComboBox::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxComboBox::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxComboBox::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxComboBox::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    Redo();
}

void wxComboBox::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    long from, to;
    GetSelection(& from, & to);
    if (from != -1 && to != -1)
        Remove(from, to);
}

void wxComboBox::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    SetSelection(-1, -1);
}

void wxComboBox::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxComboBox::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxComboBox::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxComboBox::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxComboBox::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

void wxComboBox::OnUpdateDelete(wxUpdateUIEvent& event)
{
    event.Enable(HasSelection() && IsEditable()) ;
}

void wxComboBox::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

#endif
