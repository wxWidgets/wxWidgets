///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/infobar.cpp
// Purpose:     wxInfoBar implementation for GTK
// Author:      Vadim Zeitlin
// Created:     2009-09-27
// RCS-ID:      $Id: wxhead.cpp,v 1.10 2009-06-29 10:23:04 zeitlin Exp $
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/infobar.h"

#if wxUSE_INFOBAR && defined(wxHAS_NATIVE_INFOBAR)

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/vector.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/messagetype.h"

// ----------------------------------------------------------------------------
// local classes
// ----------------------------------------------------------------------------

class wxInfoBarGTKImpl
{
public:
    wxInfoBarGTKImpl()
    {
        m_label = NULL;
    }

    GtkWidget *m_label;

    struct Button
    {
        Button(GtkWidget *button_, int id_)
            : button(button_),
              id(id_)
        {
        }

        GtkWidget *button;
        int id;
    };
    typedef wxVector<Button> Buttons;

    Buttons m_buttons;
};

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

namespace
{

inline bool UseNative()
{
    // native GtkInfoBar widget is only available in GTK+ 2.18 and later
    return gtk_check_version(2, 18, 0) == 0;
}

} // anonymous namespace

extern "C"
{

static void wxgtk_infobar_response(GtkInfoBar * WXUNUSED(infobar),
                                   gint btnid,
                                   wxInfoBar *win)
{
    win->GTKResponse(btnid);
}

static void wxgtk_infobar_close(GtkInfoBar * WXUNUSED(infobar),
                                wxInfoBar *win)
{
    win->GTKResponse(wxID_CANCEL);
}

} // extern "C" section with GTK+ callbacks

// ============================================================================
// wxInfoBar implementation
// ============================================================================

bool wxInfoBar::Create(wxWindow *parent, wxWindowID winid)
{
    if ( !UseNative() )
        return wxInfoBarGeneric::Create(parent, winid);

    m_impl = new wxInfoBarGTKImpl;

    // this control is created initially hidden
    Hide();
    if ( !CreateBase(parent, winid) )
        return false;

    // create the info bar widget itself
    m_widget = gtk_info_bar_new();
    wxCHECK_MSG( m_widget, false, "failed to create GtkInfoBar" );
    g_object_ref(m_widget);

    // also create a label which will be used to show our message
    m_impl->m_label = gtk_label_new("");
    gtk_widget_show(m_impl->m_label);

    GtkWidget * const
        contentArea = gtk_info_bar_get_content_area(GTK_INFO_BAR(m_widget));
    wxCHECK_MSG( contentArea, false, "failed to get GtkInfoBar content area" );
    gtk_container_add(GTK_CONTAINER(contentArea), m_impl->m_label);

    // finish creation and connect to all the signals we're interested in
    m_parent->DoAddChild(this);

    PostCreation(wxDefaultSize);

    GTKConnectWidget("response", G_CALLBACK(wxgtk_infobar_response));
    GTKConnectWidget("close", G_CALLBACK(wxgtk_infobar_close));

    return false;
}

wxInfoBar::~wxInfoBar()
{
    delete m_impl;
}

void wxInfoBar::ShowMessage(const wxString& msg, int flags)
{
    if ( !UseNative() )
    {
        wxInfoBarGeneric::ShowMessage(msg, flags);
        return;
    }

    GtkMessageType type;
    if ( wxGTKImpl::ConvertMessageTypeFromWX(flags, &type) )
        gtk_info_bar_set_message_type(GTK_INFO_BAR(m_widget), type);
    gtk_label_set_text(GTK_LABEL(m_impl->m_label), wxGTK_CONV(msg));

    if ( !IsShown() )
        Show();

    UpdateParent();
}

void wxInfoBar::GTKResponse(int WXUNUSED(btnid))
{
    Hide();

    UpdateParent();
}

void wxInfoBar::AddButton(wxWindowID btnid, const wxString& label)
{
    if ( !UseNative() )
    {
        wxInfoBarGeneric::AddButton(btnid, label);
        return;
    }

    GtkWidget *button = gtk_info_bar_add_button
                        (
                            GTK_INFO_BAR(m_widget),
                            label.empty()
                                ? GTKConvertMnemonics(wxGetStockGtkID(btnid))
                                : label,
                            btnid
                        );
    wxCHECK_RET( button, "unexpectedly failed to add button to info bar" );

    g_object_ref(button);
    m_impl->m_buttons.push_back(wxInfoBarGTKImpl::Button(button, btnid));
}

void wxInfoBar::RemoveButton(wxWindowID btnid)
{
    if ( !UseNative() )
    {
        wxInfoBarGeneric::RemoveButton(btnid);
        return;
    }

    // as in the generic version, look for the button starting from the end
    wxInfoBarGTKImpl::Buttons& buttons = m_impl->m_buttons;
    for ( wxInfoBarGTKImpl::Buttons::reverse_iterator i = buttons.rbegin();
          i != buttons.rend();
          ++i )
    {
        GtkWidget * const button = i->button;
        buttons.erase(i.base());
        gtk_widget_destroy(button);
        g_object_unref(button);
        return;
    }

    wxFAIL_MSG( wxString::Format("button with id %d not found", btnid) );
}

#endif // wxUSE_INFOBAR
