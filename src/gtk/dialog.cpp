/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/app.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

bool gtk_dialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxDialog *win )
{
/*
    printf( "OnDelete from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    win->Close();

    return TRUE;
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_dialog_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxDialog *win )
{
    if (!win->HasVMT()) return;

/*
    printf( "OnDialogResize from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

   if ((win->m_width != alloc->width) || (win->m_height != alloc->height))
   {
       win->m_sizeSet = FALSE;
       win->m_width = alloc->width;
       win->m_height = alloc->height;
   }
}

//-----------------------------------------------------------------------------
// "configure_event"
//-----------------------------------------------------------------------------

static gint gtk_dialog_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *event, wxDialog *win )
{
    if (!win->HasVMT()) return FALSE;

    win->m_x = event->x;
    win->m_y = event->y;

    wxMoveEvent mevent( wxPoint(win->m_x,win->m_y), win->GetId() );
    mevent.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( mevent );

    return FALSE;
}

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDialog,wxPanel)
    EVT_BUTTON  (wxID_OK,       wxDialog::OnOK)
    EVT_BUTTON  (wxID_CANCEL,   wxDialog::OnCancel)
    EVT_BUTTON  (wxID_APPLY,    wxDialog::OnApply)
    EVT_SIZE    (wxDialog::OnSize)
    EVT_CLOSE   (wxDialog::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDialog,wxPanel)

void wxDialog::Init()
{
    m_sizeSet = FALSE;
    m_modalShowing = FALSE;
}

wxDialog::wxDialog( wxWindow *parent,
                    wxWindowID id, const wxString &title,
                    const wxPoint &pos, const wxSize &size,
                    long style, const wxString &name )
{
    Init();

    Create( parent, id, title, pos, size, style, name );
}

bool wxDialog::Create( wxWindow *parent,
                       wxWindowID id, const wxString &title,
                       const wxPoint &pos, const wxSize &size,
                       long style, const wxString &name )
{
    wxTopLevelWindows.Append( this );

    m_needParent = FALSE;

    PreCreation( parent, id, pos, size, style, name );

    m_widget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    GTK_WIDGET_UNSET_FLAGS( m_widget, GTK_CAN_FOCUS );

    gtk_widget_set( m_widget, "GtkWindow::allow_shrink", TRUE, NULL);

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_dialog_delete_callback), (gpointer)this );

    m_wxwindow = gtk_myfixed_new();
    gtk_widget_show( m_wxwindow );
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );

    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );

    SetTitle( title );

    if (m_parent) m_parent->AddChild( this );

    PostCreation();
    
    gtk_widget_realize( m_widget );

    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate",
        GTK_SIGNAL_FUNC(gtk_dialog_size_callback), (gpointer)this );

    gtk_signal_connect( GTK_OBJECT(m_widget), "configure_event",
        GTK_SIGNAL_FUNC(gtk_dialog_configure_callback), (gpointer)this );

    return TRUE;
}

wxDialog::~wxDialog()
{
    wxTopLevelWindows.DeleteObject( this );

    if (wxTheApp->GetTopWindow() == this)
    {
        wxTheApp->SetTopWindow( (wxWindow*) NULL );
    }

    if (wxTopLevelWindows.Number() == 0)
    {
        wxTheApp->ExitMainLoop();
    }
}

void wxDialog::SetTitle( const wxString& title )
{
    m_title = title;
    if (m_title.IsNull()) m_title = "";
    gtk_window_set_title( GTK_WINDOW(m_widget), m_title );
}

wxString wxDialog::GetTitle() const
{
    return (wxString&)m_title;
}

void wxDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
    if (Validate()) TransferDataFromWindow();
}

void wxDialog::OnCancel( wxCommandEvent &WXUNUSED(event) )
{
    if (IsModal())
    {
        EndModal(wxID_CANCEL);
    }
    else
    {
        SetReturnCode(wxID_CANCEL);
        this->Show(FALSE);
    }
}

void wxDialog::OnOK( wxCommandEvent &WXUNUSED(event) )
{
    if ( Validate() && TransferDataFromWindow())
    {
        if (IsModal())
        {
            EndModal(wxID_OK);
        }
        else
        {
            SetReturnCode(wxID_OK);
            this->Show(FALSE);
        }
    }
}

void wxDialog::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // yes
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.

    static wxList closing;

    if (closing.Member(this))
        return;   // no loops

    closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent);
    closing.DeleteObject(this);
}

bool wxDialog::Destroy()
{
    if (!wxPendingDelete.Member(this)) wxPendingDelete.Append(this);

    return TRUE;
}

void wxDialog::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid dialog" );

    if (GetAutoLayout())
    {
        Layout();
    }
    else
    {
        /* no child: go out ! */
        if (!GetChildren().First()) return;

        /* do we have exactly one child? */
        wxWindow *child = (wxWindow *) NULL;
        for(wxNode *node = GetChildren().First(); node; node = node->Next())
        {
            wxWindow *win = (wxWindow *)node->Data();
            if (!wxIS_KIND_OF(win,wxFrame) && !wxIS_KIND_OF(win,wxDialog))
            {
                /* it's the second one: do nothing */
                if (child) return;
                child = win;
            }
        }

        /* yes: set it's size to fill all the frame */
        int client_x, client_y;
        GetClientSize( &client_x, &client_y );
        child->SetSize( 1, 1, client_x-2, client_y);
    }
}

void wxDialog::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid dialog" );
    wxASSERT_MSG( (m_wxwindow != NULL), "invalid dialog" );

    if (m_resizing) return; /* I don't like recursions */
    m_resizing = TRUE;

    int old_x = m_x;
    int old_y = m_y;
    int old_width = m_width;
    int old_height = m_height;

    if ((sizeFlags & wxSIZE_USE_EXISTING) == wxSIZE_USE_EXISTING)
    {
        if (x != -1) m_x = x;
        if (y != -1) m_y = y;
        if (width != -1) m_width = width;
        if (height != -1) m_height = height;
    }
    else
    {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
    }

    if ((sizeFlags & wxSIZE_AUTO_WIDTH) == wxSIZE_AUTO_WIDTH)
    {
        if (width == -1) m_width = 80;
    }

    if ((sizeFlags & wxSIZE_AUTO_HEIGHT) == wxSIZE_AUTO_HEIGHT)
    {
       if (height == -1) m_height = 26;
    }

    if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
    if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_maxWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_maxHeight;

    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y))
	{
            /* m_sizeSet = FALSE; */
            gtk_widget_set_uposition( m_widget, m_x, m_y ); 
	}
    }

    if ((m_width != old_width) || (m_height != old_height))
    {
        m_sizeSet = FALSE;
    }

    m_resizing = FALSE;
}

void wxDialog::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y), int width, int height )
{
    // due to a bug in gtk, x,y are always 0
    // m_x = x;
    // m_y = y;

    if ((m_height == height) && (m_width == width) && (m_sizeSet)) return;
    if (!m_wxwindow) return;

    m_width = width;
    m_height = height;

    if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
    if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_maxWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_maxHeight;

    /* we actually set the size of a frame here and no-where else */
    gtk_widget_set_usize( m_widget, m_width, m_height );

    m_sizeSet = TRUE;

    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );
}

void wxDialog::Centre( int direction )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid dialog" );

    int x = 0;
    int y = 0;

    if ((direction & wxHORIZONTAL) == wxHORIZONTAL) x = (gdk_screen_width () - m_width) / 2;
    if ((direction & wxVERTICAL) == wxVERTICAL) y = (gdk_screen_height () - m_height) / 2;

    Move( x, y );
}

void wxDialog::OnInternalIdle()
{
    if (!m_sizeSet)
        GtkOnSize( m_x, m_y, m_width, m_height );
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

        GtkOnSize( m_x, m_y, m_width, m_height );
    }

    wxWindow::Show( show );
    
    if (show) InitDialog();

    return TRUE;
}

bool wxDialog::IsModal() const
{
    return m_modalShowing;
}

void wxDialog::SetModal( bool WXUNUSED(flag) )
{
/*
  if (flag)
    m_windowStyle |= wxDIALOG_MODAL;
  else
    if (m_windowStyle & wxDIALOG_MODAL) m_windowStyle -= wxDIALOG_MODAL;
*/
    wxFAIL_MSG( "wxDialog:SetModal obsolete now" );
}

int wxDialog::ShowModal()
{
    if (IsModal())
    {
       wxFAIL_MSG( "wxDialog:ShowModal called twice" );
       return GetReturnCode();
    }
    
    Show( TRUE );

    m_modalShowing = TRUE;

    gtk_grab_add( m_widget );
    gtk_main();
    gtk_grab_remove( m_widget );

    return GetReturnCode();
}

void wxDialog::EndModal( int retCode )
{
    SetReturnCode( retCode );

    if (!IsModal())
    {
        wxFAIL_MSG( "wxDialog:EndModal called twice" );
        return;
    }

    m_modalShowing = FALSE;

    gtk_main_quit();

    Show( FALSE );
}

void wxDialog::InitDialog()
{
    wxWindow::InitDialog();
}

void wxDialog::SetIcon( const wxIcon &icon )
{
    m_icon = icon;
    if (!icon.Ok()) return;

    wxMask *mask = icon.GetMask();
    GdkBitmap *bm = (GdkBitmap *) NULL;
    if (mask) bm = mask->GetBitmap();

    gdk_window_set_icon( m_widget->window, (GdkWindow *) NULL, icon.GetPixmap(), bm );
}
