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
    m_id = -1;
    m_text = "";
    m_image = -1;
    m_page = (GtkNotebookPage *) NULL;
    m_client = (wxWindow *) NULL;
    m_parent = (GtkNotebook *) NULL;
    m_box = (GtkWidget *) NULL;
    m_added = FALSE;
  }

  /*
     mark page as "added' to the notebook, return FALSE if the page was
     already added
   */

  bool Add()
  {
    if ( WasAdded() )
      return FALSE;

    m_added = TRUE;
    return TRUE;
  }

  bool WasAdded() const { return m_added; }

  int                m_id;
  wxString           m_text;
  int                m_image;
  GtkNotebookPage   *m_page;
  GtkLabel          *m_label;
  wxWindow          *m_client;
  GtkNotebook       *m_parent;
  GtkWidget         *m_box;     // in which the label and image are packed

private:
  bool m_added;
};

//-----------------------------------------------------------------------------
// "switch_page"
//-----------------------------------------------------------------------------

static void gtk_notebook_page_change_callback(GtkNotebook *WXUNUSED(widget),
                                              GtkNotebookPage *WXUNUSED(page),
                                              gint nPage,
                                              gpointer data)
{
    if (g_isIdle) wxapp_install_idle_handler();

    wxNotebook *notebook = (wxNotebook *)data;

    int old = notebook->GetSelection();

    // TODO: emulate PAGE_CHANGING event

    wxNotebookEvent event( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                           notebook->GetId(),  nPage, old );
    event.SetEventObject( notebook );
    notebook->GetEventHandler()->ProcessEvent( event );
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
        (win->m_height == alloc->height))
    {
        return;
    }
    
    win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );

    if (win->GetAutoLayout()) win->Layout();
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

static gint
gtk_notebook_key_press_callback( GtkWidget *widget, GdkEventKey *gdk_event, wxNotebook *notebook )
{
    if (g_isIdle) wxapp_install_idle_handler();

    if (g_blockEventsOnDrag) return FALSE;

    if (!notebook->HasVMT()) return FALSE;

    /* this code makes jumping down from the handles of the notebooks
       to the actual items in the visible notebook page possible with
       the down-arrow key */

    if (gdk_event->keyval != GDK_Down) return FALSE;

    if (notebook != notebook->FindFocus()) return FALSE;

    if (notebook->m_pages.GetCount() == 0) return FALSE;

    wxNode *node = notebook->m_pages.Nth( notebook->GetSelection() );

    if (!node) return FALSE;

    wxNotebookPage *page = (wxNotebookPage*) node->Data();

    // don't let others the key event
    gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "key_press_event" );

    page->m_client->SetFocus();

    return TRUE;
}

//-----------------------------------------------------------------------------
// InsertChild callback for wxNotebook
//-----------------------------------------------------------------------------

static void wxInsertChildInNotebook( wxNotebook* parent, wxWindow* child )
{
    wxNotebookPage *page = new wxNotebookPage();

    page->m_id = parent->GetPageCount();

    page->m_box = gtk_hbox_new (FALSE, 0);
    gtk_container_border_width(GTK_CONTAINER(page->m_box), 2);

    GtkNotebook *notebook = GTK_NOTEBOOK(parent->m_widget);

    page->m_client = child;
    gtk_notebook_append_page( notebook, child->m_widget, page->m_box );

    page->m_page = (GtkNotebookPage*) (g_list_last(notebook->children)->data);

    page->m_parent = notebook;

    gtk_signal_connect( GTK_OBJECT(child->m_widget), "size_allocate",
      GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)child );

    wxASSERT_MSG( page->m_page, _T("Notebook page creation error") );

    parent->m_pages.Append( page );
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
    m_idHandler = 0;
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
    // don't generate change page events any more
    if (m_idHandler != 0)
      gtk_signal_disconnect(GTK_OBJECT(m_widget), m_idHandler);

    DeleteAllPages();
}

bool wxNotebook::Create(wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;
    m_insertCallback = (wxInsertChildFunction)wxInsertChildInNotebook;

    PreCreation( parent, id, pos, size, style, name );

    m_widget = gtk_notebook_new();

#ifdef __WXDEBUG__
    debug_focus_in( m_widget, _T("wxNotebook::m_widget"), name );
#endif

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    m_idHandler = gtk_signal_connect (
                  GTK_OBJECT(m_widget), "switch_page",
                  GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback),
                  (gpointer)this );

    m_parent->AddChild( this );

    (m_parent->m_insertCallback)( m_parent, this );

    gtk_signal_connect( GTK_OBJECT(m_widget), "key_press_event",
      GTK_SIGNAL_FUNC(gtk_notebook_key_press_callback), (gpointer)this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

int wxNotebook::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, _T("invalid notebook") );

    if (m_pages.Number() == 0) return -1;

    GtkNotebookPage *g_page = GTK_NOTEBOOK(m_widget)->cur_page;
    if (!g_page) return -1;

    wxNotebookPage *page = (wxNotebookPage *) NULL;

    wxNode *node = m_pages.First();
    while (node)
    {
        page = (wxNotebookPage*)node->Data();

        if ((page->m_page == g_page) || (page->m_page == (GtkNotebookPage*)NULL))
        {
            // page->m_page is NULL directly after gtk_notebook_append. gtk emits
            // "switch_page" then and we ask for GetSelection() in the handler for
            // "switch_page". otherwise m_page should never be NULL. all this
            // might also be wrong.
            break;
        }
        node = node->Next();
    }

    wxCHECK_MSG( node != NULL, -1, _T("wxNotebook: no selection?") );

    return page->m_id;
}

int wxNotebook::GetPageCount() const
{
    // count only the pages which were already added to the notebook for MSW
    // compatibility (and, in fact, this behaviour makes more sense anyhow
    // because only the added pages are shown)

    int n = 0;
    for ( wxNode *node = m_pages.First(); node; node = node->Next() )
    {
        wxNotebookPage *page = (wxNotebookPage*)node->Data();

        if (page->WasAdded()) n++;
    }

    return n;
}

int wxNotebook::GetRowCount() const
{
    return 1;
}

wxString wxNotebook::GetPageText( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, _T(""), _T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_text;
    else
        return "";
}

int wxNotebook::GetPageImage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, 0, _T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_image;
    else
        return 0;
}

wxNotebookPage* wxNotebook::GetNotebookPage(int page) const
{
    wxCHECK_MSG( m_widget != NULL, (wxNotebookPage*)NULL, _T("invalid notebook") );

    wxNotebookPage *nb_page = (wxNotebookPage *) NULL;

    wxNode *node = m_pages.First();
    while (node)
    {
        nb_page = (wxNotebookPage*)node->Data();
        if (nb_page->m_id == page)
            return nb_page;
        node = node->Next();
    }

    wxFAIL_MSG( _T("Notebook page not found!") );

    return (wxNotebookPage *) NULL;
}

int wxNotebook::SetSelection( int page )
{
    wxCHECK_MSG( m_widget != NULL, -1, _T("invalid notebook") );

    int selOld = GetSelection();
    wxNotebookPage* nb_page = GetNotebookPage(page);

    if (!nb_page) return -1;

    int page_num = 0;
    GList *child = GTK_NOTEBOOK(m_widget)->children;
    while (child)
    {
        if (nb_page->m_page == (GtkNotebookPage*)child->data)  break;
        page_num++;
        child = child->next;
    }

    if (!child) return -1;

    gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page_num );

    return selOld;
}

void wxNotebook::AdvanceSelection( bool bForward )
{
    wxCHECK_RET( m_widget != NULL, _T("invalid notebook") );

    int sel = GetSelection();
    int max = GetPageCount();

    if (bForward)
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
    wxCHECK_MSG( m_widget != NULL, FALSE, _T("invalid notebook") );

    wxNotebookPage* nb_page = GetNotebookPage(page);

    wxCHECK_MSG( nb_page, FALSE, _T("SetPageText: invalid page index") );

    nb_page->m_text = text;

    gtk_label_set(nb_page->m_label, nb_page->m_text.mbc_str());
   
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
    wxFAIL_MSG( _T("wxNotebook::SetPageSize not implemented") );
}

void wxNotebook::SetPadding( const wxSize &WXUNUSED(padding) )
{
    wxFAIL_MSG( _T("wxNotebook::SetPadding not implemented") );
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    wxFAIL_MSG( _T("wxNotebook::SetTabSize not implemented") );
}

bool wxNotebook::DeleteAllPages()
{
    wxCHECK_MSG( m_widget != NULL, FALSE, _T("invalid notebook") );

    wxNode *page_node = m_pages.First();
    while (page_node)
    {
        wxNotebookPage *page = (wxNotebookPage*)page_node->Data();

        DeletePage( page->m_id );

        page_node = m_pages.First();
    }

    return TRUE;
}

bool wxNotebook::DeletePage( int page )
{
    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (!nb_page) return FALSE;

    int page_num = 0;
    GList *child = GTK_NOTEBOOK(m_widget)->children;
    while (child)
    {
        if (nb_page->m_page == (GtkNotebookPage*)child->data) break;
        page_num++;
        child = child->next;
    }

    wxCHECK_MSG( child != NULL, FALSE, _T("illegal notebook index") );

    delete nb_page->m_client;

    m_pages.DeleteObject( nb_page );

    /* adjust the notebook page numbers so that
       m_id reflects the current position, Daniel Paull */ 
    int count = 0;
    wxNode *node = m_pages.First();
    wxNotebookPage *pagePtr = (wxNotebookPage *) NULL;
    while (node)
    {
        pagePtr = (wxNotebookPage*)node->Data();
        pagePtr->m_id = count++;
        node = node->Next();
    }
    
    return TRUE;
}

bool wxNotebook::RemovePage( int page )
{
    wxNotebookPage* nb_page = GetNotebookPage(page);
    if (!nb_page) return FALSE;

    int page_num = 0;
    GList *child = GTK_NOTEBOOK(m_widget)->children;
    while (child)
    {
        if (nb_page->m_page == (GtkNotebookPage*)child->data) break;
        page_num++;
        child = child->next;
    }

    wxCHECK_MSG( child != NULL, FALSE, _T("illegal notebook index") );

    gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page_num );

    m_pages.DeleteObject( nb_page );

    return TRUE;
}

bool wxNotebook::AddPage(wxWindow* win, const wxString& text,
                         bool select, int imageId)
{
    wxCHECK_MSG( m_widget != NULL, FALSE, _T("invalid notebook") );

    /* we've created the notebook page in AddChild(). Now we just have to set
       the caption for the page and set the others parameters. */

    wxNotebookPage *page = (wxNotebookPage *) NULL;

    wxNode *node = m_pages.First();
    while (node)
    {
        page = (wxNotebookPage*)node->Data();
        if ( page->m_client == win ) break;
        node = node->Next();
    }

    wxCHECK_MSG( page != NULL, FALSE,
               _T("Can't add a page whose parent is not the notebook!") );

    wxCHECK_MSG( page->Add(), FALSE,
               _T("Can't add the same page twice to a notebook.") );

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

    /* then set the attributes */
    page->m_text = text;
    if (page->m_text.IsEmpty()) page->m_text = _T("");
    page->m_image = imageId;
    page->m_label = (GtkLabel *)gtk_label_new(page->m_text.mbc_str());
    gtk_box_pack_end( GTK_BOX(page->m_box), (GtkWidget *)page->m_label, FALSE, FALSE, 3);

    /* @@@: what does this do? do we still need it?
    gtk_misc_set_alignment(GTK_MISC(page->m_label), 0.0, 0.5); */

    gtk_widget_show((GtkWidget *)page->m_label);

    if (select) SetSelection( GetPageCount()-1 );

    return TRUE;
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
    wxCHECK_MSG( m_widget != NULL, (wxWindow*) NULL, _T("invalid notebook") );

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

