/////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/stc_gtk.cpp
// Purpose:     wxGTK implementation of wxStyledTextCtrl
// Author:      Filip Hejsek
// Created:     2026-03-07
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STC

#include "wx/stc/stc.h"

#ifndef WX_PRECOMP
    #include "wx/module.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>

#include "Scintilla.h"
#include "ScintillaWidget.h"

static void
scintilla_notify_callback(GtkWidget* WXUNUSED(widget), gint WXUNUSED(id), SCNotification *scn, wxStyledTextCtrl *stc)
{
    stc->NotifyParent(scn);
}

static void
scintilla_command_callback(GtkWidget* WXUNUSED(widget), gint param, GtkWidget* WXUNUSED(widget2), wxStyledTextCtrl *stc)
{
    int command = param >> 16;
    if (command == SCEN_CHANGE)
        stc->NotifyChange();
}

static gboolean
scintilla_button_press_callback(GtkWidget* widget, GdkEventButton* event, wxStyledTextCtrl* WXUNUSED(stc))
{
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY)
    {
        GtkWidgetClass* klass = GTK_WIDGET_GET_CLASS(widget);
        if (klass && klass->button_press_event)
            return klass->button_press_event(widget, event);
    }

    return FALSE;
}

bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    style &= ~(wxVSCROLL | wxHSCROLL);
    style |= wxWANTS_CHARS | wxCLIP_CHILDREN;

    if (!PreCreation(parent, pos, size) ||
        !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name))
    {
        return false;
    }

    m_widget = scintilla_new();
    if (!m_widget)
    {
        return false;
    }

    g_object_ref(m_widget);

    m_parent->DoAddChild(this);

    // Forward Scintilla notifications and text change events to wxWidgets.
    g_signal_connect(m_widget, "sci-notify", G_CALLBACK(scintilla_notify_callback), this);
    g_signal_connect(m_widget, "command", G_CALLBACK(scintilla_command_callback), this);

    // Connect before PostCreation() so right-clicks reach Scintilla's built-in
    // context menu instead of wxWindowGTK intercepting them.
    g_signal_connect(m_widget, "button-press-event", G_CALLBACK(scintilla_button_press_callback), this);

    PostCreation(size);

    m_sciFn = reinterpret_cast<void*>(
        scintilla_send_message(SCINTILLA(m_widget), SCI_GETDIRECTFUNCTION, 0, 0));
    m_sciPtr = scintilla_send_message(SCINTILLA(m_widget), SCI_GETDIRECTPOINTER, 0, 0);

    SetDefaultAttributes();

    return true;
}

wxStyledTextCtrl::~wxStyledTextCtrl()
{
    if (m_widget)
    {
        g_signal_handlers_disconnect_by_data(m_widget, this);
    }
}

class wxSTCModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        scintilla_release_resources();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxSTCModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSTCModule, wxModule);

#endif // wxUSE_STC
