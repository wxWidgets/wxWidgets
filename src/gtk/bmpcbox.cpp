/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/bmpcbox.cpp
// Purpose:     wxBitmapComboBox
// Author:      Jaakko Salli
// Created:     2008-05-19
// Copyright:   (c) 2008 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_BITMAPCOMBOBOX

#include "wx/bmpcbox.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include "wx/gtk/private.h"
#include "wx/gtk/private/value.h"
#include "wx/gtk/private/gtk3-compat.h"
#if GTK_CHECK_VERSION(3,10,0)
    #include <cairo-gobject.h>
#endif

// ============================================================================
// implementation
// ============================================================================


wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapComboBox, wxComboBox);


// ----------------------------------------------------------------------------
// wxBitmapComboBox creation
// ----------------------------------------------------------------------------

void wxBitmapComboBox::Init()
{
    m_bitmapCellIndex = 0;
    m_stringCellIndex = 1;
    m_bitmapSize = wxSize(-1, -1);
}

wxBitmapComboBox::wxBitmapComboBox(wxWindow *parent,
                                  wxWindowID id,
                                  const wxString& value,
                                  const wxPoint& pos,
                                  const wxSize& size,
                                  const wxArrayString& choices,
                                  long style,
                                  const wxValidator& validator,
                                  const wxString& name)
    : wxComboBox(),
      wxBitmapComboBoxBase()
{
    Init();

    Create(parent,id,value,pos,size,choices,style,validator,name);
}

bool wxBitmapComboBox::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& value,
                              const wxPoint& pos,
                              const wxSize& size,
                              const wxArrayString& choices,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

bool wxBitmapComboBox::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& value,
                              const wxPoint& pos,
                              const wxSize& size,
                              int n,
                              const wxString choices[],
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    if ( !wxComboBox::Create(parent, id, value, pos, size,
                             n, choices, style, validator, name) )
        return false;

    // Select 'value' in entry-less mode
    if ( !GetEntry() )
    {
        int i = FindString(value);
        if (i != wxNOT_FOUND)
            SetSelection(i);
    }

    return true;
}

void wxBitmapComboBox::GTKCreateComboBoxWidget()
{
    GtkListStore *store;

    GType imageType;
    const char* imageAttr;
#if GTK_CHECK_VERSION(3,10,0)
    if (wx_is_at_least_gtk3(10))
    {
        imageType = CAIRO_GOBJECT_TYPE_SURFACE;
        imageAttr = "surface";
    }
    else
#endif
    {
        imageType = G_TYPE_OBJECT;
        imageAttr = "pixbuf";
    }
    store = gtk_list_store_new(2, imageType, G_TYPE_STRING);

    if ( HasFlag(wxCB_READONLY) )
    {
        m_widget = gtk_combo_box_new_with_model( GTK_TREE_MODEL(store) );
    }
    else
    {
#ifdef __WXGTK3__
        m_widget = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(store));
        gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(m_widget), m_stringCellIndex);
#else
        m_widget = gtk_combo_box_entry_new_with_model( GTK_TREE_MODEL(store), m_stringCellIndex );
#endif
        m_entry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_widget)));
        g_object_add_weak_pointer(G_OBJECT(m_entry), (void**)&m_entry);
        gtk_editable_set_editable(GTK_EDITABLE(m_entry), true);
    }
    g_object_ref(m_widget);

    // This must be called as gtk_combo_box_entry_new_with_model adds
    // automatically adds one text column.
    gtk_cell_layout_clear( GTK_CELL_LAYOUT(m_widget) );

    GtkCellRenderer* imageRenderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(m_widget),
                                imageRenderer, FALSE);
    gtk_cell_layout_add_attribute( GTK_CELL_LAYOUT(m_widget),
                                   imageRenderer, imageAttr, 0);

    GtkCellRenderer* textRenderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_end( GTK_CELL_LAYOUT(m_widget),
                              textRenderer, TRUE );
    gtk_cell_layout_add_attribute( GTK_CELL_LAYOUT(m_widget),
                                   textRenderer, "text", 1);
}

wxBitmapComboBox::~wxBitmapComboBox()
{
}

GtkWidget* wxBitmapComboBox::GetConnectWidget()
{
    if ( GetEntry() )
        return wxComboBox::GetConnectWidget();

    return wxChoice::GetConnectWidget();
}

GdkWindow *wxBitmapComboBox::GTKGetWindow(wxArrayGdkWindows& windows) const
{
    if ( GetEntry() )
        return wxComboBox::GTKGetWindow(windows);

    return wxChoice::GTKGetWindow(windows);
}

wxSize wxBitmapComboBox::DoGetBestSize() const
{
    wxSize best = wxComboBox::DoGetBestSize();

    int delta = GetBitmapSize().y - GetCharHeight();
    if ( delta > 0 )
        best.y += delta;

    return best;
}

// ----------------------------------------------------------------------------
// Item manipulation
// ----------------------------------------------------------------------------

void wxBitmapComboBox::SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap)
{
    wxBitmap bmp = bitmap.GetBitmapFor(this);
    if ( bmp.IsOk() )
    {
        if ( m_bitmapSize.x < 0 )
        {
            m_bitmapSize.x = bmp.GetLogicalWidth();
            m_bitmapSize.y = bmp.GetLogicalHeight();
        }

        GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
        GtkTreeModel *model = gtk_combo_box_get_model( combobox );
        GtkTreeIter iter;

        if ( gtk_tree_model_iter_nth_child( model, &iter, nullptr, n ) )
        {
            wxGtkValue value0;
#if GTK_CHECK_VERSION(3,10,0)
            if (wx_is_at_least_gtk3(10))
            {
                g_value_init(value0, CAIRO_GOBJECT_TYPE_SURFACE);
                cairo_surface_t* surface = gdk_cairo_surface_create_from_pixbuf(
                    bmp.GetPixbuf(), 1, gtk_widget_get_window(m_widget));
                const double scaleFactor = bmp.GetScaleFactor();
                cairo_surface_set_device_scale(surface, scaleFactor, scaleFactor);
                g_value_set_boxed(value0, surface);
                cairo_surface_destroy(surface);
            }
            else
#endif
            {
                g_value_init(value0, G_TYPE_OBJECT);
                g_value_set_object( value0, bmp.GetPixbuf() );
            }
            gtk_list_store_set_value( GTK_LIST_STORE(model), &iter,
                                      m_bitmapCellIndex, value0 );
        }
    }
}

wxBitmap wxBitmapComboBox::GetItemBitmap(unsigned int n) const
{
    wxBitmap bitmap;

    GtkComboBox* combobox = GTK_COMBO_BOX( m_widget );
    GtkTreeModel *model = gtk_combo_box_get_model( combobox );
    GtkTreeIter iter;

    if (gtk_tree_model_iter_nth_child (model, &iter, nullptr, n))
    {
        wxGtkValue value;
        gtk_tree_model_get_value( model, &iter,
                                  m_bitmapCellIndex, value );
#if GTK_CHECK_VERSION(3,10,0)
        if (wx_is_at_least_gtk3(10))
        {
            cairo_surface_t* surface = static_cast<cairo_surface_t*>(g_value_get_boxed(value));
            if (surface)
            {
                const int w = cairo_image_surface_get_width(surface);
                const int h = cairo_image_surface_get_height(surface);
                bitmap = wxBitmap(gdk_pixbuf_get_from_surface(surface, 0, 0, w, h));
                double sx, sy;
                cairo_surface_get_device_scale(surface, &sx, &sy);
                bitmap.SetScaleFactor(sx);
            }
        }
        else
#endif
        {
            GdkPixbuf* pixbuf = (GdkPixbuf*) g_value_get_object( value );
            if ( pixbuf )
            {
                g_object_ref( pixbuf );
                bitmap = wxBitmap(pixbuf);
            }
        }
    }

    return bitmap;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap)
{
    const int n = wxComboBox::Append(item);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             void *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             wxClientData *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item,
                             const wxBitmapBundle& bitmap,
                             unsigned int pos)
{
    const int n = wxComboBox::Insert(item, pos);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, wxClientData *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, void *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

// ----------------------------------------------------------------------------
// wxTextEntry interface override
// ----------------------------------------------------------------------------

void wxBitmapComboBox::WriteText(const wxString& value)
{
    if ( GetEntry() )
        wxComboBox::WriteText(value);
    else
        SetStringSelection(value);
}

wxString wxBitmapComboBox::GetValue() const
{
    if ( GetEntry() )
        return wxComboBox::GetValue();

    return GetStringSelection();
}

void wxBitmapComboBox::Remove(long from, long to)
{
    if ( GetEntry() )
        wxComboBox::Remove(from, to);
}

void wxBitmapComboBox::SetInsertionPoint(long pos)
{
    if ( GetEntry() )
        wxComboBox::SetInsertionPoint(pos);
}

long wxBitmapComboBox::GetInsertionPoint() const
{
    if ( GetEntry() )
        return wxComboBox::GetInsertionPoint();

    return 0;
 }
long wxBitmapComboBox::GetLastPosition() const
{
    if ( GetEntry() )
        return wxComboBox::GetLastPosition();

    return 0;
 }

void wxBitmapComboBox::SetSelection(long from, long to)
{
    if ( GetEntry() )
        wxComboBox::SetSelection(from, to);
}

void wxBitmapComboBox::GetSelection(long *from, long *to) const
{
    if ( GetEntry() )
        wxComboBox::GetSelection(from, to);
}

bool wxBitmapComboBox::IsEditable() const
{
    if ( GetEntry() )
        return wxTextEntry::IsEditable();

    return false;
}

void wxBitmapComboBox::SetEditable(bool editable)
{
    if ( GetEntry() )
        wxComboBox::SetEditable(editable);
}

#endif // wxUSE_BITMAPCOMBOBOX
