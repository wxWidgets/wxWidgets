/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/tglbtn.cpp
// Purpose:     Definition of the wxToggleButton class, which implements a
//              toggle button under wxGTK.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     Rocketeer license
/////////////////////////////////////////////////////////////////////////////

#include "wx/tglbtn.h"

#if wxUSE_TOGGLEBTN

#include <gdk/gdk.h>
#include <gtk/gtk.h>

extern void wxapp_install_idle_handler();
extern bool g_isIdle;
extern bool      g_blockEventsOnDrag;
extern wxCursor   g_globalCursor;

// void gtk_togglebutton_clicked_callback(GtkWidget *widget, wxToggleButton *cb)
// Callback function given to gtk.
void wxToggleButton::gtk_togglebutton_clicked_callback(GtkWidget *WXUNUSED(widget), wxToggleButton *cb)
{
   if (g_isIdle)
      wxapp_install_idle_handler();

   if (!cb->m_hasVMT || g_blockEventsOnDrag)
      return;

   // Generate a wx event.
   wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, cb->GetId());
   event.SetInt(cb->GetValue());
   event.SetEventObject(cb);
   cb->GetEventHandler()->ProcessEvent(event);
}

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

// bool Create(wxWindow *parent, wxWindowID id, const wxString &label,
//             const wxPoint &pos, const wxSize &size, long style,
//             const wxValidator& validator, const wxString &name)
// Create the control.
bool wxToggleButton::Create(wxWindow *parent, wxWindowID id,
                            const wxString &label, const wxPoint &pos,
                            const wxSize &size, long style,
                            const wxValidator& validator,
                            const wxString &name)
{
   m_needParent = TRUE;
   m_acceptsFocus = TRUE;

   if (!PreCreation(parent, pos, size) ||
       !CreateBase(parent, id, pos, size, style, validator, name )) {
      wxFAIL_MSG(wxT("wxToggleButton creation failed"));
      return FALSE;
   }

   wxControl::SetLabel(label);

   // Create the gtk widget.
   m_widget = gtk_toggle_button_new_with_label(m_label.mbc_str());

   gtk_signal_connect(GTK_OBJECT(m_widget), "clicked",
                      GTK_SIGNAL_FUNC(gtk_togglebutton_clicked_callback),
                      (gpointer *)this);

   m_parent->DoAddChild(this);

   PostCreation();

   SetFont(parent->GetFont());

   wxSize size_best(DoGetBestSize());
   wxSize new_size(size);
   if (new_size.x == -1)
      new_size.x = size_best.x;
   if (new_size.y == -1)
      new_size.y = size_best.y;
   if ((new_size.x != size.x) || (new_size.y != size.y))
      SetSize(new_size.x, new_size.y);

   SetBackgroundColour(parent->GetBackgroundColour());
   SetForegroundColour(parent->GetForegroundColour());

   Show(TRUE);

   return TRUE;
}

// void SetValue(bool state)
// Set the value of the toggle button.
void wxToggleButton::SetValue(bool state)
{
   wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

   if (state == GetValue())
      return;

   gtk_signal_disconnect_by_func(GTK_OBJECT(m_widget),
                                 GTK_SIGNAL_FUNC(gtk_togglebutton_clicked_callback),
                                 (gpointer *)this);

   gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(m_widget), state);

   gtk_signal_connect(GTK_OBJECT(m_widget), "clicked",
                      GTK_SIGNAL_FUNC(gtk_togglebutton_clicked_callback),
                      (gpointer *)this);
}

// bool GetValue() const
// Get the value of the toggle button.
bool wxToggleButton::GetValue() const
{
   wxCHECK_MSG(m_widget != NULL, FALSE, wxT("invalid toggle button"));

   return GTK_TOGGLE_BUTTON(m_widget)->active;
}

// void SetLabel(const wxString& label)
// Set the button's label.
void wxToggleButton::SetLabel(const wxString& label)
{
   wxCHECK_RET(m_widget != NULL, wxT("invalid toggle button"));

   wxControl::SetLabel(label);

   gtk_label_set(GTK_LABEL(GTK_BUTTON(m_widget)->child),
                 GetLabel().mbc_str());
}

// bool Enable(bool enable)
// Enable (or disable) the control.
bool wxToggleButton::Enable(bool enable /*=TRUE*/)
{
   if (!wxControl::Enable(enable))
      return FALSE;

   gtk_widget_set_sensitive(GTK_BUTTON(m_widget)->child, enable);

   return TRUE;
}

// void ApplyWidgetStyle()
// I don't really know what this does.
void wxToggleButton::ApplyWidgetStyle()
{
   SetWidgetStyle();
   gtk_widget_set_style(m_widget, m_widgetStyle);
   gtk_widget_set_style(GTK_BUTTON(m_widget)->child, m_widgetStyle);
}

// bool IsOwnGtkWindow(GdkWindow *window)
// I'm not really sure what this is for, either.
bool wxToggleButton::IsOwnGtkWindow(GdkWindow *window)
{
   return (window == GTK_TOGGLE_BUTTON(m_widget)->event_window);
}

// void OnInternalIdle()
// Apparently gtk cursors are difficult to deal with.
void wxToggleButton::OnInternalIdle()
{
   wxCursor cursor = m_cursor;
   if (g_globalCursor.Ok())
      cursor = g_globalCursor;

   if (GTK_TOGGLE_BUTTON(m_widget)->event_window && cursor.Ok()) {
      /* I now set the cursor the anew in every OnInternalIdle call
         as setting the cursor in a parent window also effects the
         windows above so that checking for the current cursor is
         not possible. */

      gdk_window_set_cursor(GTK_TOGGLE_BUTTON(m_widget)->event_window,
                            cursor.GetCursor());
   }

   UpdateWindowUI();
}

// wxSize DoGetBestSize() const
// Get the "best" size for this control.
wxSize wxToggleButton::DoGetBestSize() const
{
   wxSize ret(wxControl::DoGetBestSize());
   if (ret.x < 80)
      ret.x = 80;

   return ret;
}

#endif // wxUSE_TOGGLEBTN

