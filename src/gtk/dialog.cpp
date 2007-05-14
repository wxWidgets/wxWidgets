/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dialog.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dialog.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/cursor.h"
#endif // WX_PRECOMP

#include "wx/evtloop.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

// Don't allow window closing if there are open dialogs
int g_openDialogs;

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDialog,wxTopLevelWindow)

void wxDialog::Init()
{
    m_returnCode = 0;
    m_sizeSet = false;
    m_modalShowing = false;
    m_themeEnabled = true;
}

wxDialog::wxDialog( wxWindow *parent,
                    wxWindowID id, const wxString &title,
                    const wxPoint &pos, const wxSize &size,
                    long style, const wxString &name )
{
    Init();

    (void)Create( parent, id, title, pos, size, style, name );
}

bool wxDialog::Create( wxWindow *parent,
                       wxWindowID id, const wxString &title,
                       const wxPoint &pos, const wxSize &size,
                       long style, const wxString &name )
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // all dialogs should have tab traversal enabled
    style |= wxTAB_TRAVERSAL;

    return wxTopLevelWindow::Create(parent, id, title, pos, size, style, name);
}

bool wxDialog::Show( bool show )
{
    if (!show && IsModal())
    {
        EndModal( wxID_CANCEL );
    }

    if (show && !m_sizeSet)
    {
        /* by calling GtkOnSize here, we don't have to call
           either after showing the frame, which would entail
           much ugly flicker nor from within the size_allocate
           handler, because GTK 1.1.X forbids that. */

        GtkOnSize();
    }

    bool ret = wxWindow::Show( show );

    if (show) InitDialog();

    return ret;
}

bool wxDialog::IsModal() const
{
    return m_modalShowing;
}

void wxDialog::SetModal( bool WXUNUSED(flag) )
{
    wxFAIL_MSG( wxT("wxDialog:SetModal obsolete now") );
}

int wxDialog::ShowModal()
{
    if (IsModal())
    {
       wxFAIL_MSG( wxT("wxDialog:ShowModal called twice") );
       return GetReturnCode();
    }

    // use the apps top level window as parent if none given unless explicitly
    // forbidden
    if ( !GetParent() && !(GetWindowStyleFlag() & wxDIALOG_NO_PARENT) )
    {
        wxWindow * const parent = GetParentForModalDialog();
        if ( parent && parent != this )
        {
            gtk_window_set_transient_for( GTK_WINDOW(m_widget),
                                          GTK_WINDOW(parent->m_widget) );
        }
    }

    wxBusyCursorSuspender cs; // temporarily suppress the busy cursor

    Show( true );

    m_modalShowing = true;

    g_openDialogs++;

    // NOTE: gtk_window_set_modal internally calls gtk_grab_add() !
    gtk_window_set_modal(GTK_WINDOW(m_widget), TRUE);

    wxGUIEventLoop().Run();

    gtk_window_set_modal(GTK_WINDOW(m_widget), FALSE);

    g_openDialogs--;

    return GetReturnCode();
}

void wxDialog::EndModal( int retCode )
{
    SetReturnCode( retCode );

    if (!IsModal())
    {
        wxFAIL_MSG( wxT("wxDialog:EndModal called twice") );
        return;
    }

    m_modalShowing = false;

    gtk_main_quit();

    Show( false );
}
