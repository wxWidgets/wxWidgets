/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

#include "wx/notebook.h"

#if wxUSE_NOTEBOOK

#include "wx/panel.h"
#include "wx/utils.h"
#include "wx/imaglist.h"
#include "wx/intl.h"
#include "wx/log.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include "wx/gtk/win_gtk.h"
#include "gdk/gdkkeysyms.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// debug
//-----------------------------------------------------------------------------

#ifdef __WXDEBUG__

extern void debug_focus_in( GtkWidget* widget, const wxChar* name, const wxChar *window );

#endif

//-----------------------------------------------------------------------------
// wxNotebookPage
//-----------------------------------------------------------------------------

class wxNotebookPage: public wxObject
{
public:
  wxNotebookPage()
  {
    m_text = "";
    m_image = -1;
    m_page = (GtkNotebookPage *) NULL;
    m_client = (wxWindow *) NULL;
    m_box = (GtkWidget *) NULL;
  }

  wxString           m_text;
  int                m_image;
  GtkNotebookPage   *m_page;
  GtkLabel          *m_label;
  wxWindow          *m_client;
  GtkWidget         *m_box;     // in which the label and image are packed
};

//-----------------------------------------------------------------------------
// "switch_page"
//-----------------------------------------------------------------------------

static void gtk_notebook_page_change_callback(GtkNotebook *WXUNUSED(widget),
                                              GtkNotebookPage *WXUNUSED(page),
                                              gint page,
                                              wxNotebook *notebook )
{
    if (g_isIdle) 
        wxapp_install_idle_handler();

    int old = notebook->GetSelection();

    wxNotebookEvent event1( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
                            notebook->GetId(), page, old );
    event1.SetEventObject( notebook );
    
    if ((notebook->GetEventHandler()->ProcessEvent( event1 )) &&
        !event1.IsAllowed() )
    {
        /* program doesn't allow the page change */
        gtk_signal_emit_stop_by_name( GTK_OBJECT(notebook->m_widget), "switch_page" );
        return;
    }

    wxNotebookEvent event2( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                            notebook->GetId(), page, old );
    event2.SetEventObject( notebook );
    notebook->GetEventHandler()->ProcessEvent( event2 );
}

//-----------------------------------------------------------------------------
// "size_allocate"
//-----------------------------------------------------------------------------

static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{
    if (g_isIdle) 
        wxapp_install_idle_handler();
    
    if ((win->m_x == alloc->x) &&
        (win->m_y == alloc->y) &&
        (win->m_width == alloc->width) &&
        (win->m_height == alloc->height))
    {
        return;
    }
    
    win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );

    if (win->GetAutoLayout()) win->Layout();
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxNotebook
//-----------------------------------------------------------------------------

static void wxInsertChildInNotebook( wxNotebook* WXUNUSED(parent), wxWindow* WXUNUSED(child) )
{
    /* we don't do anything here but pray */
}

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebook,wxControl)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

void wxNotebook::Init()
{
    m_imageList = (wxImageList *) NULL;
    m_pages.DeleteContents( TRUE );
    m_lastSelection = -1;
}

wxNotebook::wxNotebook()
{
    Init();
}

wxNotebook::wxNotebook( wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    Init();
    Create( parent, id, pos, size, style, name );
}

wxNotebook::~wxNotebook()
{
    /* don't generate change page events any more */
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget), 
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer) this );

    DeleteAllPages();
}

bool wxNotebook::Create(wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
    m_insertCallback = (wxInsertChildFunction)wxInsertChildInNotebook;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( T("wxNoteBook creation failed") );
	return FALSE;
    }


    m_widget = gtk_notebook_new();

#ifdef __WXDEBUG__
    debug_focus_in( m_widget, T("wxNotebook::m_widget"), name );
#endif

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    gtk_signal_connect( GTK_OBJECT(m_widget), "switch_page",
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer)this );

    m_parent->DoAddChild( this );

	if(m_windowStyle & wxNB_RIGHT)
		gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_RIGHT );
	if(m_windowStyle & wxNB_LEFT)
		gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_LEFT );
	if(m_windowStyle & wxNB_BOTTOM)
		gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_BOTTOM );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

void wxNotebook::SetFocus()
{
    if (m_pages.GetCount() == 0) return;

    wxNode *node = m_pages.Nth( GetSelection() );

    if (!node) return;

    wxNotebookPage *page = (wxNotebookPage*) node->Data();

    page->m_client->SetFocus();
}

int wxNotebook::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, T("invalid notebook") );

    GList *pages = GTK_NOTEBOOK(m_widget)->children;

    if (g_list_length(pages) == 0) return -1;

    GtkNotebook *notebook = GTK_NOTEBOOK(m_widget);
    
    if (notebook->cur_page == NULL) return m_lastSelection;
    
    return g_list_index( pages, (gpointer)(notebook->cur_page) );
}

int wxNotebook::GetPageCount() const
{
    return (int) g_list_length( GTK_NOTEBOOK(m_widget)->children );
}

int wxNotebook::GetRowCount() const
{
    return 1;
}

wxString wxNotebook::GetPageText( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, T(""), T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_text;
    else
        return T("");
}

int wxNotebook::GetPageImage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, -1, T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_image;
    else
        return -1;
}

wxNotebookPage* wxNotebook::GetNotebookPage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, (wxNotebookPage*) NULL, T("invalid notebook") );

    wxCHECK_MSG( page < (int)m_pages.GetCount(), (wxNotebookPage*) NULL, T("invalid notebook index") );
    
    wxNode *node = m_pages.Nth( page );
    
    return (wxNotebookPage *) node->Data();
}

int wxNotebook::SetSelection( int page )
{
    wxCHECK_MSG( m_widget != NULL, -1, T("invalid notebook") );

    wxCHECK_MSG( page < (int)m_pages.GetCount(), -1, T("invalid notebook index") );

    int selOld = GetSelection();
    
    gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page );

    return selOld;
}

void wxNotebook::AdvanceSelection( bool forward )
{
    wxCHECK_RET( m_widget != NULL, T("invalid notebook") );

    int sel = GetSelection();
    int max = GetPageCount();

    if (forward)
        SetSelection( sel == max ? 0 : sel + 1 );
    else
        SetSelection( sel == 0 ? max-1 : sel - 1 );
}

void wxNotebook::SetImageList( wxImageList* imageList )
{
    m_imageList = imageList;
}

bool wxNotebook::SetPageText( int page, const wxString &text )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);

    wxCHECK_MSG( nb_page, FALSE, T("SetPageText: invalid page index") );

    nb_page->m_text = text;

    gtk_label_set( nb_page->m_label, nb_page->m_text.mbc_str() );
   
    return TRUE;
}

bool wxNotebook::SetPageImage( int page, int image )
{
    /* HvdH 28-12-98: now it works, but it's a bit of a kludge */

    wxNotebookPage* nb_page = GetNotebookPage(page);

    if (!nb_page) return FALSE;

    /* Optimization posibility: return immediately if image unchanged.
     * Not enabled because it may break existing (stupid) code that
     * manipulates the imagelist to cycle images */

    /* if (image == nb_page->m_image) return TRUE; */

    /* For different cases:
       1) no image -> no image
       2) image -> no image
       3) no image -> image
       4) image -> image */

    if (image == -1 && nb_page->m_image == -1)
        return TRUE; /* Case 1): Nothing to do. */

    GtkWidget *pixmapwid = (GtkWidget*) NULL;

    if (nb_page->m_image != -1)
    {
        /* Case 2) or 4). There is already an image in the gtkhbox. Let's find it */

        GList *child = gtk_container_children(GTK_CONTAINER(nb_page->m_box));
        while (child)
	{
            if (GTK_IS_PIXMAP(child->data))
	    {
 	        pixmapwid = GTK_WIDGET(child->data);
 	        break;
            }
	    child = child->next;
	}

        /* We should have the pixmap widget now */
        wxASSERT(pixmapwid != NULL);

        if (image == -1)
	{
            /* If there's no new widget, just remove the old from the box */
            gtk_container_remove(GTK_CONTAINER(nb_page->m_box), pixmapwid);
            nb_page->m_image = -1;

            return TRUE; /* Case 2) */
        }
    }

    /* Only cases 3) and 4) left */
    wxASSERT( m_imageList != NULL ); /* Just in case */

    /* Construct the new pixmap */
    const wxBitmap *bmp = m_imageList->GetBitmap(image);
    GdkPixmap *pixmap = bmp->GetPixmap();
    GdkBitmap *mask = (GdkBitmap*) NULL;
    if ( bmp->GetMask() )
    {
        mask = bmp->GetMask()->GetBitmap();
    }

    if (pixmapwid == NULL)
    {
        /* Case 3) No old pixmap. Create a new one and prepend it to the hbox */
        pixmapwid = gtk_pixmap_new (pixmap, mask );

        /* CHECKME: Are these pack flags okay? */
        gtk_box_pack_start(GTK_BOX(nb_page->m_box), pixmapwid, FALSE, FALSE, 3);
        gtk_widget_show(pixmapwid);
    }
    else
    {
        /* Case 4) Simply replace the pixmap */
        gtk_pixmap_set(GTK_PIXMAP(pixmapwid), pixmap, mask);
    }

    nb_page->m_image = image;

    return TRUE;
}

void wxNotebook::SetPageSize( const wxSize &WXUNUSED(size) )
{
    wxFAIL_MSG( T("wxNotebook::SetPageSize not implemented") );
}

void wxNotebook::SetPadding( const wxSize &WXUNUSED(padding) )
{
    wxFAIL_MSG( T("wxNotebook::SetPadding not implemented") );
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
    wxFAIL_MSG( T("wxNotebook::SetTabSize not implemented") );
}

bool wxNotebook::DeleteAllPages()
{
    wxCHECK_MSG( m_widget != NULL, FALSE, T("invalid notebook") );

    while (m_pages.GetCount() > 0)
        DeletePage( m_pages.GetCount()-1 );

    return TRUE;
}

bool wxNotebook::DeletePage( int page )
{
    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (!nb_page) return FALSE;

    /* GTK sets GtkNotebook.cur_page to NULL before sending
       the switvh page event */
    m_lastSelection = GetSelection();
    
    nb_page->m_client->Destroy();
    m_pages.DeleteObject( nb_page );
    
    m_lastSelection = -1;

    return TRUE;
}

bool wxNotebook::RemovePage( int page )
{
    wxNotebookPage* nb_page = GetNotebookPage(page);
    
    if (!nb_page) return FALSE;

    gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page );

    m_pages.DeleteObject( nb_page );

    return TRUE;
}

bool wxNotebook::InsertPage( int position, wxWindow* win, const wxString& text,
                             bool select, int imageId )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, T("invalid notebook") );

    wxCHECK_MSG( win->GetParent() == this, FALSE,
               T("Can't add a page whose parent is not the notebook!") );

    /* don't receive switch page during addition */	       
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget), 
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer) this );
      
    GtkNotebook *notebook = GTK_NOTEBOOK(m_widget);

    wxNotebookPage *page = new wxNotebookPage();
    
    if (position < 0)
        m_pages.Append( page );
    else
        m_pages.Insert( m_pages.Nth( position ), page );
    
    page->m_client = win;

    page->m_box = gtk_hbox_new( FALSE, 0 );
    gtk_container_border_width( GTK_CONTAINER(page->m_box), 2 );

    gtk_signal_connect( GTK_OBJECT(win->m_widget), "size_allocate",
      GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)win );

    if (position < 0)
        gtk_notebook_append_page( notebook, win->m_widget, page->m_box );
    else 
        gtk_notebook_insert_page( notebook, win->m_widget, page->m_box, position );

    page->m_page = (GtkNotebookPage*) g_list_last(notebook->children)->data;

    /* set the label image */
    page->m_image = imageId;
    
    if (imageId != -1)
    {
        wxASSERT( m_imageList != NULL );

        const wxBitmap *bmp = m_imageList->GetBitmap(imageId);
        GdkPixmap *pixmap = bmp->GetPixmap();
        GdkBitmap *mask = (GdkBitmap*) NULL;
        if ( bmp->GetMask() )
        {
            mask = bmp->GetMask()->GetBitmap();
        }

        GtkWidget *pixmapwid = gtk_pixmap_new (pixmap, mask );

        gtk_box_pack_start(GTK_BOX(page->m_box), pixmapwid, FALSE, FALSE, 3);

        gtk_widget_show(pixmapwid);
    }

    /* set the label text */
    page->m_text = text;
    if (page->m_text.IsEmpty()) page->m_text = T("");
    
    page->m_label = GTK_LABEL( gtk_label_new(page->m_text.mbc_str()) );
    gtk_box_pack_end( GTK_BOX(page->m_box), GTK_WIDGET(page->m_label), FALSE, FALSE, 3 );

    /* show the label */
    gtk_widget_show( GTK_WIDGET(page->m_label) );

    if (select && (m_pages.GetCount() > 1))
    {
        if (position < 0)
            SetSelection( GetPageCount()-1 );
	else
            SetSelection( position );
    }

    gtk_signal_connect( GTK_OBJECT(m_widget), "switch_page",
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer)this );

    return TRUE;
}

bool wxNotebook::AddPage(wxWindow* win, const wxString& text,
                         bool select, int imageId)
{
    return InsertPage( -1, win, text, select, imageId );
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if (event.IsWindowChange())
        AdvanceSelection( event.GetDirection() );
    else
        event.Skip();
}

wxWindow *wxNotebook::GetPage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, (wxWindow*) NULL, T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (!nb_page)
        return (wxWindow *) NULL;
    else
        return nb_page->m_client;
}

// override these 2 functions to do nothing: everything is done in OnSize
void wxNotebook::SetConstraintSizes( bool WXUNUSED(recurse) )
{
    // don't set the sizes of the pages - their correct size is not yet known
    wxControl::SetConstraintSizes(FALSE);
}

bool wxNotebook::DoPhase( int WXUNUSED(nPhase) )
{
    return TRUE;
}

void wxNotebook::ApplyWidgetStyle()
{
    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

bool wxNotebook::IsOwnGtkWindow( GdkWindow *window )
{
    return ((m_widget->window == window) ||
            (GTK_NOTEBOOK(m_widget)->panel == window));
}

//-----------------------------------------------------------------------------
// wxNotebookEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

#endif
