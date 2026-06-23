////////////////////////////////////////////////////////////////////////////
// Name:        stc_gtk.cpp
// Purpose:     wxGTK implementation of wxStyledTextCtrl
//
// Author:      Filip Hejsek
//
// Created:     7-Mar-2026
// Copyright:   (c) 2026 TODO what do I put here?
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STC

#include "wx/stc/stc.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <gtk/gtk.h>

#include "Scintilla.h"
#include "ScintillaWidget.h"

static void
scintilla_notify_callback(GtkWidget* WXUNUSED(widget), gint WXUNUSED(id), SCNotification *scn, wxStyledTextCtrl *stc) {
    stc->NotifyParent(scn);
}

static void
scintilla_command_callback(GtkWidget* WXUNUSED(widget), gint param, GtkWidget* WXUNUSED(widget2), wxStyledTextCtrl *stc) {
    int command = param >> 16;
    if (command == SCEN_CHANGE)
        stc->NotifyChange();
}

//----------------------------------------------------------------------
// Constructor and Destructor

wxStyledTextCtrl::wxStyledTextCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxString& name)
{
    Create(parent, id, pos, size, style, name);
}


bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    style |= wxVSCROLL | wxHSCROLL | wxWANTS_CHARS | wxCLIP_CHILDREN;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStyledTextCtrl creation failed") );
        return false;
    }

    m_widget = scintilla_new();
    g_object_ref(m_widget);

    m_parent->DoAddChild( this );

    m_stopWatch.Start();
    m_lastKeyDownConsumed = false;
    m_vScrollBar = nullptr;
    m_hScrollBar = nullptr;
    // Put Scintilla into unicode (UTF-8) mode
    SetCodePage(wxSTC_CP_UTF8);

    PostCreation(size);

    // Make sure it can take the focus
    SetCanFocus(true);

    // STC doesn't support RTL languages at all
    SetLayoutDirection(wxLayout_LeftToRight);

    // Use colours appropriate for the current system colour theme.
    auto attr = wxTextCtrl::GetClassDefaultAttributes();
    StyleSetForeground(wxSTC_STYLE_DEFAULT, attr.colFg);
    StyleSetBackground(wxSTC_STYLE_DEFAULT, attr.colBg);
    SetCaretForeground(attr.colFg);

    // We also need to set this one because its foreground is hardcoded as
    // black in Scintilla sources.
    StyleSetForeground(wxSTC_STYLE_LINENUMBER, attr.colFg);

    // And foreground for this one is hardcoded as white.
    StyleSetForeground(wxSTC_STYLE_CALLTIP, attr.colFg);

    SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

    g_signal_connect(m_widget, "sci-notify", G_CALLBACK(scintilla_notify_callback), this);
    g_signal_connect(m_widget, "command", G_CALLBACK(scintilla_command_callback), this);

    return true;
}


wxStyledTextCtrl::~wxStyledTextCtrl() {
}


//----------------------------------------------------------------------

wxIntPtr wxStyledTextCtrl::SendMsg(int msg, wxUIntPtr wp, wxIntPtr lp) const
{
    return scintilla_send_message(SCINTILLA(m_widget), msg, wp, lp);
}

#endif // wxUSE_STC
