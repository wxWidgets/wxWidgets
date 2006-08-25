/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxGTK.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
#endif

// FIXME: Use GtkImage instead of GtkPixmap.
#include <gtk/gtkversion.h>
#ifdef GTK_DISABLE_DEPRECATED
#undef GTK_DISABLE_DEPRECATED
#endif

#include "wx/gtk/private.h"

extern bool      g_blockEventsOnDrag;

extern "C" {
static void gtk_togglebutton_clicked_callback(GtkWidget *WXUNUSED(widget), wxToggleButton *cb)
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!cb->m_hasVMT || g_blockEventsOnDrag)
        return;

    if (cb->m_blockEvent) return;

    // Generate a wx event.
    wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, cb->GetId());
    event.SetInt(cb->GetValue());
    event.SetEventObject(cb);
    cb->GetEventHandler()->ProcessEvent(event);
}
}

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

// ------------------------------------------------------------------------
// wxToggleBitmapButton
// ------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToggleBitmapButton, wxControl)

bool wxToggleBitmapButton::Create(wxWindow *parent, wxWindowID id,
                            const wxBitmap &label, const wxPoint &pos,
                            const wxSize &size, long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    m_needParent = true;
    m_acceptsFocus = true;

    m_blockEvent = false;

    if (!PreCreation(parent, pos, size) ||
       !CreateBase(parent, id, pos, size, style, validator, name ))
    {
        wxFAIL_MSG(wxT("wxToggleBitmapButton creation failed"));
        return false;
    }

    m_bitmap = label;

    // Create the gtk widget.
    m_widget = gtk_toggle_button_new();

    if (style & wxNO_BORDER)
       gtk_button_set_relief( GTK_BUTTON(m_widget), GTK_RELIEF_NONE );

    if (m_bitmap.Ok())
    {
        OnSetBitmap();
    }

    g_signal_connect (m_widget, "clicked",
                      G_CALLBACK (gtk_togglebutton_clicked_callback),
                      this);

    m_parent->DoAddChild(this);

    PostCreation(size);

    return true;
}

// void SetValue(bool state)
// Set the value of the toggle button.
void wxToggleBitmapButton::SetValue(bool state)
{
    wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

    if (state == GetValue())
        return;

    m_blockEvent = true;

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_widget), state);

    m_blockEvent = false;
}

// bool GetValue() const
// Get the value of the toggle button.
bool wxToggleBitmapButton::GetValue() const
{
    wxCHECK_MSG(m_widget != NULL, false, wxT("invalid toggle button"));

    return GTK_TOGGLE_BUTTON(m_widget)->active;
}

void wxToggleBitmapButton::SetLabel(const wxBitmap& label)
{
    wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

    m_bitmap = label;
    InvalidateBestSize();

    OnSetBitmap();
}

void wxToggleBitmapButton::OnSetBitmap()
{
    if (!m_bitmap.Ok()) return;

    GdkBitmap *mask = (GdkBitmap *) NULL;
    if (m_bitmap.GetMask()) mask = m_bitmap.GetMask()->GetBitmap();

    GtkWidget *child = GTK_BIN(m_widget)->child;
    if (child == NULL)
    {
        // initial bitmap
        GtkWidget *pixmap = gtk_pixmap_new(m_bitmap.GetPixmap(), mask);
        gtk_widget_show(pixmap);
        gtk_container_add(GTK_CONTAINER(m_widget), pixmap);
    }
    else
    {   // subsequent bitmaps
        GtkPixmap *g_pixmap = GTK_PIXMAP(child);
        gtk_pixmap_set(g_pixmap, m_bitmap.GetPixmap(), mask);
    }
}

bool wxToggleBitmapButton::Enable(bool enable /*=true*/)
{
    if (!wxControl::Enable(enable))
        return false;

    gtk_widget_set_sensitive(GTK_BIN(m_widget)->child, enable);

    return true;
}

void wxToggleBitmapButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_widget, style);
    gtk_widget_modify_style(GTK_BIN(m_widget)->child, style);
}

GdkWindow *
wxToggleBitmapButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return GTK_BUTTON(m_widget)->event_window;
}

// Get the "best" size for this control.
wxSize wxToggleBitmapButton::DoGetBestSize() const
{
    wxSize best;

    if (m_bitmap.Ok())
    {
        int border = HasFlag(wxNO_BORDER) ? 4 : 10;
        best.x = m_bitmap.GetWidth()+border;
        best.y = m_bitmap.GetHeight()+border;
    }
    CacheBestSize(best);
    return best;
}


// static
wxVisualAttributes
wxToggleBitmapButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_toggle_button_new);
}


// ------------------------------------------------------------------------
// wxToggleButton
// ------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)

bool wxToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxString &label, const wxPoint &pos,
                            const wxSize &size, long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    m_needParent = true;
    m_acceptsFocus = true;

    m_blockEvent = false;

    if (!PreCreation(parent, pos, size) ||
        !CreateBase(parent, id, pos, size, style, validator, name )) {
        wxFAIL_MSG(wxT("wxToggleButton creation failed"));
        return false;
    }

    wxControl::SetLabel(label);

    // Create the gtk widget.
    m_widget = gtk_toggle_button_new_with_label( wxGTK_CONV( m_label ) );

    g_signal_connect (m_widget, "clicked",
                      G_CALLBACK (gtk_togglebutton_clicked_callback),
                      this);

    m_parent->DoAddChild(this);

    PostCreation(size);

    return true;
}

// void SetValue(bool state)
// Set the value of the toggle button.
void wxToggleButton::SetValue(bool state)
{
    wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

    if (state == GetValue())
        return;

    m_blockEvent = true;

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_widget), state);

    m_blockEvent = false;
}

// bool GetValue() const
// Get the value of the toggle button.
bool wxToggleButton::GetValue() const
{
    wxCHECK_MSG(m_widget != NULL, false, wxT("invalid toggle button"));

    return GTK_TOGGLE_BUTTON(m_widget)->active;
}

void wxToggleButton::SetLabel(const wxString& label)
{
    wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

    wxControl::SetLabel(label);

    gtk_label_set_text(GTK_LABEL(GTK_BIN(m_widget)->child), wxGTK_CONV(GetLabel()));
}

bool wxToggleButton::Enable(bool enable /*=true*/)
{
    if (!wxControl::Enable(enable))
        return false;

    gtk_widget_set_sensitive(GTK_BIN(m_widget)->child, enable);

    return true;
}

void wxToggleButton::DoApplyWidgetStyle(GtkRcStyle *style)
{
    gtk_widget_modify_style(m_widget, style);
    gtk_widget_modify_style(GTK_BIN(m_widget)->child, style);
}

GdkWindow *
wxToggleButton::GTKGetWindow(wxArrayGdkWindows& WXUNUSED(windows)) const
{
    return GTK_BUTTON(m_widget)->event_window;
}

// Get the "best" size for this control.
wxSize wxToggleButton::DoGetBestSize() const
{
    wxSize ret(wxControl::DoGetBestSize());

    if (!HasFlag(wxBU_EXACTFIT))
    {
        if (ret.x < 80) ret.x = 80;
    }

    CacheBestSize(ret);
    return ret;
}

// static
wxVisualAttributes
wxToggleButton::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_toggle_button_new);
}

#endif // wxUSE_TOGGLEBTN
