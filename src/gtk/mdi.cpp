/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/mdi.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MDI

#include "wx/mdi.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/menu.h"
    #include "wx/dialog.h"
#endif

#include "wx/notebook.h"
#include "wx/gtk/private.h"

#include <gtk/gtk.h>
#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const int wxMENU_HEIGHT = 27;

//-----------------------------------------------------------------------------
// globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// "switch_page"
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_mdi_page_change_callback( GtkNotebook *WXUNUSED(widget),
                              GtkNotebookPage *page,
                              gint WXUNUSED(page_num),
                              wxMDIParentFrame *parent )
{
    // send deactivate event to old child

    wxMDIChildFrame *child = parent->GetActiveChild();
    if (child)
    {
        wxActivateEvent event1( wxEVT_ACTIVATE, false, child->GetId() );
        event1.SetEventObject( child);
        child->GetEventHandler()->ProcessEvent( event1 );
    }

    // send activate event to new child

    wxMDIClientWindow *client_window = parent->GetClientWindow();
    if (!client_window)
        return;

    child = (wxMDIChildFrame*) NULL;

    wxWindowList::compatibility_iterator node = client_window->GetChildren().GetFirst();
    while ( node )
    {
        wxMDIChildFrame *child_frame = wxDynamicCast( node->GetData(), wxMDIChildFrame );

        // child_frame can be NULL when this is called from dtor, probably
        // because g_signal_connect (m_widget, "switch_page", (see below)
        // isn't deleted early enough
        if ( child_frame && child_frame->m_page == page )
        {
            child = child_frame;
            break;
        }
        node = node->GetNext();
    }

    if (!child)
         return;

    wxActivateEvent event2( wxEVT_ACTIVATE, true, child->GetId() );
    event2.SetEventObject( child);
    child->GetEventHandler()->ProcessEvent( event2 );
}
}

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame,wxFrame)

void wxMDIParentFrame::Init()
{
    m_justInserted = false;
    m_clientWindow = (wxMDIClientWindow *) NULL;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
}

bool wxMDIParentFrame::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name )
{
    wxFrame::Create( parent, id, title, pos, size, style, name );

    OnCreateClient();

    return true;
}

void wxMDIParentFrame::GtkOnSize()
{
    wxFrame::GtkOnSize();

    wxMDIChildFrame *child_frame = GetActiveChild();
    if (!child_frame) return;

    wxMenuBar *menu_bar = child_frame->m_menuBar;
    if (!menu_bar) return;
    if (!menu_bar->m_widget) return;

    menu_bar->m_x = 0;
    menu_bar->m_y = 0;
    menu_bar->m_width = m_width;
    menu_bar->m_height = wxMENU_HEIGHT;
    gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
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
        gtk_notebook_set_current_page( notebook, g_list_length( notebook->children ) - 1 );

        /* need to set the menubar of the child */
        wxMDIChildFrame *active_child_frame = GetActiveChild();
        if (active_child_frame != NULL)
        {
            wxMenuBar *menu_bar = active_child_frame->m_menuBar;
            if (menu_bar)
            {
                menu_bar->m_width = m_width;
                menu_bar->m_height = wxMENU_HEIGHT;
                gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                    menu_bar->m_widget,
                                    0, 0, m_width, wxMENU_HEIGHT );
                menu_bar->SetInvokingWindow(active_child_frame);
            }
        }
        m_justInserted = false;
        return;
    }

    wxFrame::OnInternalIdle();

    wxMDIChildFrame *active_child_frame = GetActiveChild();
    bool visible_child_menu = false;

    wxWindowList::compatibility_iterator node = m_clientWindow->GetChildren().GetFirst();
    while (node)
    {
        wxMDIChildFrame *child_frame = wxDynamicCast( node->GetData(), wxMDIChildFrame );

        if ( child_frame )
        {
            wxMenuBar *menu_bar = child_frame->m_menuBar;
            if ( menu_bar )
            {
                if (child_frame == active_child_frame)
                {
                    if (menu_bar->Show(true))
                    {
                        menu_bar->m_width = m_width;
                        menu_bar->m_height = wxMENU_HEIGHT;
                        gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                            menu_bar->m_widget,
                                            0, 0, m_width, wxMENU_HEIGHT );
                        menu_bar->SetInvokingWindow( child_frame );
                    }
                    visible_child_menu = true;
                }
                else
                {
                    if (menu_bar->Show(false))
                    {
                        menu_bar->UnsetInvokingWindow( child_frame );
                    }
                }
            }
        }

        node = node->GetNext();
    }

    /* show/hide parent menu bar as required */
    if ((m_frameMenuBar) &&
        (m_frameMenuBar->IsShown() == visible_child_menu))
    {
        if (visible_child_menu)
        {
            m_frameMenuBar->Show( false );
            m_frameMenuBar->UnsetInvokingWindow( this );
        }
        else
        {
            m_frameMenuBar->Show( true );
            m_frameMenuBar->SetInvokingWindow( this );

            m_frameMenuBar->m_width = m_width;
            m_frameMenuBar->m_height = wxMENU_HEIGHT;
            gtk_pizza_set_size( GTK_PIZZA(m_mainWidget),
                                m_frameMenuBar->m_widget,
                                0, 0, m_width, wxMENU_HEIGHT );
        }
    }
}

wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
    if (!m_clientWindow) return (wxMDIChildFrame*) NULL;

    GtkNotebook *notebook = GTK_NOTEBOOK(m_clientWindow->m_widget);
    if (!notebook) return (wxMDIChildFrame*) NULL;

    gint i = gtk_notebook_get_current_page( notebook );
    if (i < 0) return (wxMDIChildFrame*) NULL;

    GtkNotebookPage* page = (GtkNotebookPage*) (g_list_nth(notebook->children,i)->data);
    if (!page) return (wxMDIChildFrame*) NULL;

    wxWindowList::compatibility_iterator node = m_clientWindow->GetChildren().GetFirst();
    while (node)
    {
        if ( wxPendingDelete.Member(node->GetData()) )
            return (wxMDIChildFrame*) NULL;

        wxMDIChildFrame *child_frame = wxDynamicCast( node->GetData(), wxMDIChildFrame );

        if (!child_frame)
            return (wxMDIChildFrame*) NULL;

        if (child_frame->m_page == page)
            return child_frame;

        node = node->GetNext();
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

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxFrame)

BEGIN_EVENT_TABLE(wxMDIChildFrame, wxFrame)
    EVT_ACTIVATE(wxMDIChildFrame::OnActivate)
    EVT_MENU_HIGHLIGHT_ALL(wxMDIChildFrame::OnMenuHighlight)
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

bool wxMDIChildFrame::Destroy()
{
    // delayed destruction: the frame will be deleted during
    // the next idle loop iteration.
    // I'm not sure if delayed destruction really makes so
    // much sense for MDI child frames, actually, but hiding
    // it doesn't make any sense.
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    return true;
}

void wxMDIChildFrame::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxWindow::DoSetSize( x, y, width, height, sizeFlags );
}

void wxMDIChildFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize( width, height );
}

void wxMDIChildFrame::DoGetClientSize( int *width, int *height ) const
{
    wxWindow::DoGetClientSize( width, height );
}

void wxMDIChildFrame::AddChild( wxWindowBase *child )
{
    wxWindow::AddChild(child);
}

void wxMDIChildFrame::SetMenuBar( wxMenuBar *menu_bar )
{
    wxASSERT_MSG( m_menuBar == NULL, wxT("Only one menubar allowed") );

    m_menuBar = menu_bar;

    if (m_menuBar)
    {
        wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->GetParent();

        m_menuBar->SetParent( mdi_frame );

        /* insert the invisible menu bar into the _parent_ mdi frame */
        gtk_pizza_put( GTK_PIZZA(mdi_frame->m_mainWidget),
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
    wxMDIParentFrame* parent = (wxMDIParentFrame*) GetParent();
    GtkNotebook* notebook = GTK_NOTEBOOK(parent->m_widget);
    gint pageno = gtk_notebook_page_num( notebook, m_widget );
    gtk_notebook_set_current_page( notebook, pageno );
}

void wxMDIChildFrame::OnActivate( wxActivateEvent& WXUNUSED(event) )
{
}

void wxMDIChildFrame::OnMenuHighlight( wxMenuEvent& event )
{
#if wxUSE_STATUSBAR
    wxMDIParentFrame *mdi_frame = (wxMDIParentFrame*)m_parent->GetParent();
    if ( !ShowMenuHelp(mdi_frame->GetStatusBar(), event.GetMenuId()) )
    {
        // we don't have any help text for this item, but may be the MDI frame
        // does?
        mdi_frame->OnMenuHighlight(event);
    }
#endif // wxUSE_STATUSBAR
}

void wxMDIChildFrame::SetTitle( const wxString &title )
{
    if ( title == m_title )
        return;

    m_title = title;

    wxMDIParentFrame* parent = (wxMDIParentFrame*) GetParent();
    GtkNotebook* notebook = GTK_NOTEBOOK(parent->m_widget);
    gtk_notebook_set_tab_label_text(notebook, m_widget, wxGTK_CONV( title ) );
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{
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
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxMDIClientWindow
//-----------------------------------------------------------------------------

static void wxInsertChildInMDI( wxMDIClientWindow* parent, wxMDIChildFrame* child )
{
    wxString s = child->GetTitle();
    if (s.IsNull()) s = _("MDI child");

    GtkWidget *label_widget = gtk_label_new( s.mbc_str() );
    gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );

    g_signal_connect (child->m_widget, "size_allocate",
                      G_CALLBACK (gtk_page_size_callback), child);

    GtkNotebook *notebook = GTK_NOTEBOOK(parent->m_widget);

    gtk_notebook_append_page( notebook, child->m_widget, label_widget );

    child->m_page = (GtkNotebookPage*) (g_list_last(notebook->children)->data);

    wxMDIParentFrame *parent_frame = (wxMDIParentFrame*) parent->GetParent();
    parent_frame->m_justInserted = true;
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
    m_needParent = true;

    m_insertCallback = (wxInsertChildFunction)wxInsertChildInMDI;

    if (!PreCreation( parent, wxDefaultPosition, wxDefaultSize ) ||
        !CreateBase( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("wxMDIClientWindow") ))
    {
        wxFAIL_MSG( wxT("wxMDIClientWindow creation failed") );
        return false;
    }

    m_widget = gtk_notebook_new();

    g_signal_connect (m_widget, "switch_page",
                      G_CALLBACK (gtk_mdi_page_change_callback), parent);

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    m_parent->DoAddChild( this );

    PostCreation();

    Show( true );

    return true;
}

#endif
