/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"
#include "wx/dialog.h"
#include "wx/menu.h"
#include <wx/intl.h>

#include "glib.h"
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const int wxMENU_HEIGHT = 27;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame,wxFrame)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
END_EVENT_TABLE()

wxMDIParentFrame::wxMDIParentFrame()
{
    m_justInserted = FALSE;
    m_clientWindow = (wxMDIClientWindow *) NULL;
}

wxMDIParentFrame::wxMDIParentFrame( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    m_justInserted = FALSE;
    m_clientWindow = (wxMDIClientWindow *) NULL;
    Create( parent, id, title, pos, size, style, name );
}

wxMDIParentFrame::~wxMDIParentFrame()
{
}

bool wxMDIParentFrame::Create( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    wxFrame::Create( parent, id, title, pos, size, style, name );

    OnCreateClient();

    return TRUE;
}

void wxMDIParentFrame::GtkOnSize( int x, int y, int width, int height )
{
    wxFrame::GtkOnSize( x, y, width, height );

    wxMDIChildFrame *child_frame = GetActiveChild();
    if (!child_frame) return;

    wxMenuBar *menu_bar = child_frame->m_menuBar;
    if (!menu_bar) return;
    if (!menu_bar->m_widget) return;

    menu_bar->m_x = 0;
    menu_bar->m_y = 0;
    menu_bar->m_width = m_width;
    menu_bar->m_height = wxMENU_HEIGHT;
    gtk_myfixed_set_size( GTK_MYFIXED(m_mainWidget), 
                          menu_bar->m_widget, 
                          0, 0, m_width, wxMENU_HEIGHT );
}

void wxMDIParentFrame::OnInternalIdle()
{
    /* if a an MDI child window has just been inserted
       it has to be brought to the top in idle time. we
       simply set the last notebook page active as new
       pages can only be appended at the end */

    if (m_justInserted)
    {
        GtkNotebook *notebook = GTK_NOTEBOOK(m_clientWindow->m_widget);
        gtk_notebook_set_page( notebook, g_list_length( notebook->children ) - 1 );

        m_justInserted = FALSE;
        return;
    }

    wxFrame::OnInternalIdle();

    wxMDIChildFrame *active_child_frame = GetActiveChild();
    bool visible_child_menu = FALSE;

    wxNode *node = m_clientWindow->GetChildren().First();
    while (node)
    {
        wxMDIChildFrame *child_frame = (wxMDIChildFrame *)node->Data();
        if (child_frame->m_menuBar)
        {
            if (child_frame == active_child_frame)
            {
               gtk_widget_show( child_frame->m_menuBar->m_widget );
               visible_child_menu = TRUE;
            }
            else
               gtk_widget_hide( child_frame->m_menuBar->m_widget );
        }
        node = node->Next();
    }

    /* show/hide parent menu bar as required */
    if (m_frameMenuBar) m_frameMenuBar->Show( !visible_child_menu );
}

void wxMDIParentFrame::GetClientSize(int *width, int *height ) const
{
    wxFrame::GetClientSize( width, height );
}

wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
    if (!m_clientWindow) return (wxMDIChildFrame*) NULL;

    GtkNotebook *notebook = GTK_NOTEBOOK(m_clientWindow->m_widget);
    if (!notebook) return (wxMDIChildFrame*) NULL;

#if (GTK_MINOR_VERSION > 0)
    gint i = gtk_notebook_get_current_page( notebook );
#else
    gint i = gtk_notebook_current_page( notebook );
#endif
    if (i < 0) return (wxMDIChildFrame*) NULL;

    GtkNotebookPage* page = (GtkNotebookPage*) (g_list_nth(notebook->children,i)->data);
    if (!page) return (wxMDIChildFrame*) NULL;

    wxNode *node = m_clientWindow->GetChildren().First();
    while (node)
    {
        wxMDIChildFrame *child_frame = (wxMDIChildFrame *)node->Data();
        if (child_frame->m_page == page)
            return child_frame;
        node = node->Next();
    }

    return (wxMDIChildFrame*) NULL;
}

wxMDIClientWindow *wxMDIParentFrame::GetClientWindow() const
{
    return m_clientWindow;
}

wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
    m_clientWindow = new wxMDIClientWindow( this );
    return m_clientWindow;
}

void wxMDIParentFrame::ActivateNext()
{
    if (m_clientWindow)
      gtk_notebook_next_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
}

void wxMDIParentFrame::ActivatePrevious()
{
    if (m_clientWindow)
      gtk_notebook_prev_page( GTK_NOTEBOOK(m_clientWindow->m_widget) );
}

void wxMDIParentFrame::OnActivate( wxActivateEvent& WXUNUSED(event) )
{
}

void wxMDIParentFrame::OnSysColourChanged( wxSysColourChangedEvent& WXUNUSED(event) )
{
}

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxFrame)

BEGIN_EVENT_TABLE(wxMDIChildFrame, wxFrame)
    EVT_ACTIVATE(wxMDIChildFrame::OnActivate)
END_EVENT_TABLE()

wxMDIChildFrame::wxMDIChildFrame()
{
    m_menuBar = (wxMenuBar *) NULL;
    m_page = (GtkNotebookPage *) NULL;
}

wxMDIChildFrame::wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
    m_menuBar = (wxMenuBar *) NULL;
    m_page = (GtkNotebookPage *) NULL;
    Create( parent, id, title, wxDefaultPosition, size, style, name );
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    if (m_menuBar)
        delete m_menuBar;
}

bool wxMDIChildFrame::Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& WXUNUSED(pos), const wxSize& size,
      long style, const wxString& name )
{
    m_title = title;

    return wxWindow::Create( parent->GetClientWindow(), id, wxDefaultPosition, size, style, name );
}

void wxMDIChildFrame::GetClientSize( int *width, int *height ) const
{
    wxWindow::GetClientSize( width, height );
}

void wxMDIChildFrame::AddChild( wxWindow *child )
{
    wxWindow::AddChild( child );
}

static void SetInvokingWindow( wxMenu *menu, wxWindow *win )
{
    menu->SetInvokingWindow( win );
    wxNode *node = menu->GetItems().First();
    while (node)
    {
        wxMenuItem *menuitem = (wxMenuItem*)node->Data();
        if (menuitem->IsSubMenu())
            SetInvokingWindow( menuitem->GetSubMenu(), win );
        node = node->Next();
    }
}

void wxMDIChildFrame::SetMenuBar( wxMenuBar *menu_bar )
{
    m_menuBar = menu_bar;

    if (m_menuBar)
    {
        wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->GetParent();

       if (m_menuBar->GetParent() != this)
       {
            wxNode *node = m_menuBar->GetMenus().First();
            while (node)
            {
                wxMenu *menu = (wxMenu*)node->Data();
                SetInvokingWindow( menu, this );
                node = node->Next();
            }

            m_menuBar->SetParent( mdi_frame );
        }

        /* the menu bar of the child window is shown in idle time as needed */
        gtk_widget_hide( m_menuBar->m_widget );

        /* insert the invisible menu bar into the _parent_ mdi frame */
        gtk_myfixed_put( GTK_MYFIXED(mdi_frame->m_mainWidget), 
                         m_menuBar->m_widget, 
                         0, 0,  mdi_frame->m_width, wxMENU_HEIGHT );
    }
}

wxMenuBar *wxMDIChildFrame::GetMenuBar() const
{
    return m_menuBar;
}

void wxMDIChildFrame::Activate()
{
}

void wxMDIChildFrame::OnActivate( wxActivateEvent &WXUNUSED(event) )
{
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if ((win->m_x == alloc->x) &&
        (win->m_y == alloc->y) &&
        (win->m_width == alloc->width) &&
        (win->m_height == alloc->height) &&
        (win->m_sizeSet))
    {
        return;
    }

    win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxMDIClientWindow
//-----------------------------------------------------------------------------

static void wxInsertChildInMDI( wxMDIClientWindow* parent, wxMDIChildFrame* child )
{
    wxString s = child->m_title;
    if (s.IsNull()) s = _("MDI child");

    GtkWidget *label_widget = gtk_label_new( s.mbc_str() );
    gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );

    gtk_signal_connect( GTK_OBJECT(child->m_widget), "size_allocate",
      GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)child );

    GtkNotebook *notebook = GTK_NOTEBOOK(parent->m_widget);

    gtk_notebook_append_page( notebook, child->m_widget, label_widget );

    child->m_page = (GtkNotebookPage*) (g_list_last(notebook->children)->data);

    wxMDIParentFrame *parent_frame = (wxMDIParentFrame*) parent->GetParent();
    parent_frame->m_justInserted = TRUE;
}

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow,wxWindow)

wxMDIClientWindow::wxMDIClientWindow()
{
}

wxMDIClientWindow::wxMDIClientWindow( wxMDIParentFrame *parent, long style )
{
    CreateClient( parent, style );
}

wxMDIClientWindow::~wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient( wxMDIParentFrame *parent, long style )
{
    m_needParent = TRUE;

    m_insertCallback = (wxInsertChildFunction)wxInsertChildInMDI;

    PreCreation( parent, -1, wxPoint(10,10), wxSize(100,100), style, "wxMDIClientWindow" );

    m_widget = gtk_notebook_new();

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    m_parent->DoAddChild( this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

