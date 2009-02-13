/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/button.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/button.h"
#endif

#include "wx/stockitem.h"

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// "clicked"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_button_clicked_callback( GtkWidget *WXUNUSED(widget), wxButton *button )
{
    if (!button->m_hasVMT) return;
    if (g_blockEventsOnDrag) return;

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, button->GetId());
    event.SetEventObject(button);
    button->HandleWindowEvent(event);
}
}

//-----------------------------------------------------------------------------
// "style_set" from m_widget
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_button_style_set_callback(GtkWidget* widget, GtkStyle*, wxButton* win)
{
    /* the default button has a border around it */
    wxWindow* parent = win->GetParent();
    if (parent && parent->m_wxwindow && GTK_WIDGET_CAN_DEFAULT(widget))
    {
        GtkBorder* border = NULL;
        gtk_widget_style_get(widget, "default_border", &border, NULL);
        if (border)
        {
            win->MoveWindow(
                win->m_x - border->left,
                win->m_y - border->top,
                win->m_width + border->left + border->right,
                win->m_height + border->top + border->bottom);
            gtk_border_free(border);
        }
    }
}
}

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxButton,wxControl)

wxButton::wxButton()
{
}

wxButton::~wxButton()
{
}

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString &label,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG( wxT("wxButton creation failed") );
        return false;
    }

    m_widget = gtk_button_new_with_mnemonic("");
    g_object_ref(m_widget);

    float x_alignment = 0.5;
    if (HasFlag(wxBU_LEFT))
        x_alignment = 0.0;
    else if (HasFlag(wxBU_RIGHT))
        x_alignment = 1.0;

    float y_alignment = 0.5;
    if (HasFlag(wxBU_TOP))
        y_alignment = 0.0;
    else if (HasFlag(wxBU_BOTTOM))
        y_alignment = 1.0;

    gtk_button_set_alignment(GTK_BUTTON(m_widget), x_alignment, y_alignment);

    SetLabel(label);

    if (style & wxNO_BORDER)
       gtk_button_set_relief( GTK_BUTTON(m_widget), GTK_RELIEF_NONE );

    g_signal_connect_after (m_widget, "clicked",
                            G_CALLBACK (gtk_button_clicked_callback),
                            this);

    g_signal_connect_after (m_widget, "style_set",
                            G_CALLBACK (gtk_button_style_set_callback),
                            this);

    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}


wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    gtk_widget_grab_default( m_widget );

    // resize for default border
    gtk_button_style_set_callback( m_widget, NULL, this );

    return oldDefault;
}

/* static */
wxSize wxButtonBase::GetDefaultSize()
{
    static wxSize size = wxDefaultSize;
    if (size == wxDefaultSize)
    {
        // NB: Default size of buttons should be same as size of stock
        //     buttons as used in most GTK+ apps. Unfortunately it's a little
        //     tricky to obtain this size: stock button's size may be smaller
        //     than size of button in GtkButtonBox and vice versa,
        //     GtkButtonBox's minimal button size may be smaller than stock
        //     button's size. We have to retrieve both values and combine them.

        GtkWidget *wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *box = gtk_hbutton_box_new();
        GtkWidget *btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
        gtk_container_add(GTK_CONTAINER(box), btn);
        gtk_container_add(GTK_CONTAINER(wnd), box);
        GtkRequisition req;
        gtk_widget_size_request(btn, &req);

        gint minwidth, minheight;
        gtk_widget_style_get(box,
                             "child-min-width", &minwidth,
                             "child-min-height", &minheight,
                             NULL);

        size.x = wxMax(minwidth, req.width);
        size.y = wxMax(minheight, req.height);

        gtk_widget_destroy(wnd);
    }
    return size;
}

void wxButton::SetLabel( const wxString &lbl )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid button") );

    wxString label(lbl);

    if (label.empty() && wxIsStockID(m_windowId))
        label = wxGetStockLabel(m_windowId);

    wxControl::SetLabel(label);

    if (wxIsStockID(m_windowId) && wxIsStockLabel(m_windowId, label))
    {
        const char *stock = wxGetStockGtkID(m_windowId);
        if (stock)
        {
            gtk_button_set_label(GTK_BUTTON(m_widget), stock);
            gtk_button_set_use_stock(GTK_BUTTON(m_widget), TRUE);
            return;
        }
    }

    const wxString labelGTK = GTKConvertMnemonics(label);
    gtk_button_set_label(GTK_BUTTON(m_widget), wxGTK_CONV(labelGTK));
    gtk_button_set_use_stock(GTK_BUTTON(m_widget), FALSE);

    GTKApplyWidgetStyle( false );
}

bool wxButton::Enable( bool enable )
{
    bool isEnabled = IsEnabled();

    if ( !wxControl::Enable( enable ) )
        return false;

    gtk_widget_set_sensitive(GTK_BIN(m_widget)->child, enable);

    if (!isEnabled && enable)
    {
        GTKFixSensitivity();
    }

    return true;
}

GdkWindow *wxButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return GTK_BUTTON(m_widget)->event_window;
}

void wxButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_widget, style);
    GtkWidget *child = GTK_BIN(m_widget)->child;
    gtk_widget_modify_style(child, style);

    // for buttons with images, the path to the label is (at least in 2.12)
    // GtkButton -> GtkAlignment -> GtkHBox -> GtkLabel
    if ( GTK_IS_ALIGNMENT(child) )
    {
        GtkWidget *box = GTK_BIN(child)->child;
        if ( GTK_IS_BOX(box) )
        {
            for (GList* item = GTK_BOX(box)->children; item; item = item->next)
            {
                GtkBoxChild* boxChild = static_cast<GtkBoxChild*>(item->data);
                gtk_widget_modify_style(boxChild->widget, style);
            }
        }
    }
}

wxSize wxButton::DoGetBestSize() const
{
    // the default button in wxGTK is bigger than the other ones because of an
    // extra border around it, but we don't want to take it into account in
    // our size calculations (otherwsie the result is visually ugly), so
    // always return the size of non default button from here
    const bool isDefault = GTK_WIDGET_HAS_DEFAULT(m_widget);
    if ( isDefault )
    {
        // temporarily unset default flag
        GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    }

    wxSize ret( wxControl::DoGetBestSize() );

    if ( isDefault )
    {
        // set it back again
        GTK_WIDGET_SET_FLAGS( m_widget, GTK_CAN_DEFAULT );
    }

    if (!HasFlag(wxBU_EXACTFIT))
    {
        wxSize defaultSize = GetDefaultSize();
        if (ret.x < defaultSize.x) ret.x = defaultSize.x;
        if (ret.y < defaultSize.y) ret.y = defaultSize.y;
    }

    CacheBestSize(ret);
    return ret;
}

// static
wxVisualAttributes
wxButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_button_new);
}

#endif // wxUSE_BUTTON
