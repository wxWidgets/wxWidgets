/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/app.h"
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

    win->GtkOnSize( alloc->x, alloc->y, alloc->width, alloc->height );
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

wxDialog::wxDialog()
{
    m_title = "";
    m_modalShowing = FALSE;
}

wxDialog::wxDialog( wxWindow *parent, 
                    wxWindowID id, const wxString &title,
                    const wxPoint &pos, const wxSize &size, 
                    long style, const wxString &name )
{
    m_modalShowing = FALSE;
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
    
    gtk_signal_connect( GTK_OBJECT(m_widget), "size_allocate", 
        GTK_SIGNAL_FUNC(gtk_dialog_size_callback), (gpointer)this );
    
    m_wxwindow = gtk_myfixed_new();
    gtk_widget_show( m_wxwindow );
    GTK_WIDGET_UNSET_FLAGS( m_wxwindow, GTK_CAN_FOCUS );
  
    gtk_container_add( GTK_CONTAINER(m_widget), m_wxwindow );
  
    SetTitle( title );
  
    if ((m_x != -1) || (m_y != -1))
        gtk_widget_set_uposition( m_widget, m_x, m_y );
     
    gtk_widget_set_usize( m_widget, m_width, m_height );
     
    if (m_parent) m_parent->AddChild( this );
  
  
    PostCreation();
  
    return TRUE;
}

wxDialog::~wxDialog()
{
    wxTopLevelWindows.DeleteObject( this );
    if (wxTopLevelWindows.Number() == 0) wxTheApp->ExitMainLoop();
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

bool wxDialog::OnClose()
{
    static wxList closing;

    if (closing.Member(this)) return FALSE;   // no loops
  
    closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent);
    closing.DeleteObject(this);
  
    return FALSE;
}

bool wxDialog::Destroy()
{
    if (!wxPendingDelete.Member(this)) wxPendingDelete.Append(this);

    return TRUE;
}

void wxDialog::OnCloseWindow( wxCloseEvent& event )
{
    if (GetEventHandler()->OnClose() || event.GetForce())
    {
        this->Destroy();
    }
}

void wxDialog::GtkOnSize( int WXUNUSED(x), int WXUNUSED(y), int width, int height )
{
    // due to a bug in gtk, x,y are always 0
    // m_x = x;
    // m_y = y;

    if ((m_height == height) && (m_width == width) &&
        (m_sizeSet)) return;
    if (!m_wxwindow) return;
  
    m_width = width;
    m_height = height;
  
    if ((m_minWidth != -1) && (m_width < m_minWidth)) m_width = m_minWidth;
    if ((m_minHeight != -1) && (m_height < m_minHeight)) m_height = m_minHeight;
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_minWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_minHeight;

    gtk_widget_set_usize( m_widget, m_width, m_height );

    m_sizeSet = TRUE;
    
    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );
}

void wxDialog::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
    if (GetAutoLayout())
    {
        Layout();
    }
    else 
    {
        // no child: go out !
        if (!GetChildren().First()) return;
      
        // do we have exactly one child?
        wxWindow *child = (wxWindow *) NULL;
        for(wxNode *node = GetChildren().First(); node; node = node->Next())
        {
            wxWindow *win = (wxWindow *)node->Data();
            if (!wxIS_KIND_OF(win,wxFrame) && !wxIS_KIND_OF(win,wxDialog))
            {
	        // it's the second one: do nothing
                if (child) return;
                child = win;
            }
        }

        // yes: set it's size to fill all the frame
        int client_x, client_y;
        GetClientSize( &client_x, &client_y );
        child->SetSize( 1, 1, client_x-2, client_y);
    }
}

void wxDialog::SetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid window" );
  
    // Don't do anything for children of wxMDIChildFrame
    if (!m_wxwindow) return;

    if (m_resizing) return; // I don't like recursions
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
    if ((m_maxWidth != -1) && (m_width > m_maxWidth)) m_width = m_minWidth;
    if ((m_maxHeight != -1) && (m_height > m_maxHeight)) m_height = m_minHeight;

    if ((m_x != -1) || (m_y != -1))
    {
        if ((m_x != old_x) || (m_y != old_y)) 
            gtk_widget_set_uposition( m_widget, m_x, m_y );
    }
  
    if ((m_width != old_width) || (m_height != old_height))
    {
        gtk_widget_set_usize( m_widget, m_width, m_height );
    }
  
    m_sizeSet = TRUE;

    wxSizeEvent event( wxSize(m_width,m_height), GetId() );
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent( event );

    m_resizing = FALSE;
}

void wxDialog::SetSize( int width, int height )
{
    SetSize( -1, -1, width, height, wxSIZE_USE_EXISTING );
}

void wxDialog::Centre( int direction )
{
    wxASSERT_MSG( (m_widget != NULL), "invalid frame" );
  
    int x = 0;
    int y = 0;
    
    if (direction & wxHORIZONTAL == wxHORIZONTAL) x = (gdk_screen_width () - m_width) / 2;
    if (direction & wxVERTICAL == wxVERTICAL) y = (gdk_screen_height () - m_height) / 2;
  
    Move( x, y );
}

bool wxDialog::Show( bool show )
{
    if (!show && IsModal())
    {
      EndModal( wxID_CANCEL );
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
