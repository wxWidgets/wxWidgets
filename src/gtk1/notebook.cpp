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
#include "wx/bitmap.h"

#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

#include <gdk/gdkkeysyms.h>

// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// wxGtkNotebookPage
//-----------------------------------------------------------------------------

// VZ: this is rather ugly as we keep the pages themselves in an array (it
//     allows us to have quite a few functions implemented in the base class)
//     but the page data is kept in a separate list, so we must maintain them
//     in sync manually... of course, the list had been there before the base
//     class which explains it but it still would be nice to do something
//     about this one day

class wxGtkNotebookPage: public wxObject
{
public:
  wxGtkNotebookPage()
  {
    m_image = -1;
    m_page = (GtkNotebookPage *) NULL;
    m_box = (GtkWidget *) NULL;
  }

  wxString           m_text;
  int                m_image;
  GtkNotebookPage   *m_page;
  GtkLabel          *m_label;
  GtkWidget         *m_box;     // in which the label and image are packed
};

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxGtkNotebookPagesList);

//-----------------------------------------------------------------------------
// "switch_page"
//-----------------------------------------------------------------------------

static void gtk_notebook_page_change_callback(GtkNotebook *WXUNUSED(widget),
                                              GtkNotebookPage *WXUNUSED(page),
                                              gint page,
                                              wxNotebook *notebook )
{
    // are you trying to call SetSelection() from a notebook event handler?
    // you shouldn't!
    wxCHECK_RET( !notebook->m_inSwitchPage,
                 _T("gtk_notebook_page_change_callback reentered") );

    notebook->m_inSwitchPage = TRUE;
    if (g_isIdle)
        wxapp_install_idle_handler();

    int old = notebook->GetSelection();

    wxNotebookEvent eventChanging( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
                                   notebook->GetId(), page, old );
    eventChanging.SetEventObject( notebook );

    if ( (notebook->GetEventHandler()->ProcessEvent(eventChanging)) &&
         !eventChanging.IsAllowed() )
    {
        /* program doesn't allow the page change */
        gtk_signal_emit_stop_by_name( GTK_OBJECT(notebook->m_widget),
                                      "switch_page" );
    }
    else // change allowed
    {
        // make wxNotebook::GetSelection() return the correct (i.e. consistent
        // with wxNotebookEvent::GetSelection()) value even though the page is
        // not really changed in GTK+
        notebook->m_selection = page;

        wxNotebookEvent eventChanged( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                                      notebook->GetId(), page, old );
        eventChanged.SetEventObject( notebook );
        notebook->GetEventHandler()->ProcessEvent( eventChanged );
    }

    notebook->m_inSwitchPage = FALSE;
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

    /* GTK 1.2 up to version 1.2.5 is broken so that we have to call allocate
       here in order to make repositioning after resizing to take effect. */
    if ((gtk_major_version == 1) &&
        (gtk_minor_version == 2) &&
        (gtk_micro_version < 6) &&
        (win->m_wxwindow) &&
        (GTK_WIDGET_REALIZED(win->m_wxwindow)))
    {
        gtk_widget_size_allocate( win->m_wxwindow, alloc );
    }
}

//-----------------------------------------------------------------------------
// "realize" from m_widget
//-----------------------------------------------------------------------------

static gint
gtk_notebook_realized_callback( GtkWidget * WXUNUSED(widget), wxWindow *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    /* GTK 1.2 up to version 1.2.5 is broken so that we have to call a queue_resize
       here in order to make repositioning before showing to take effect. */
    gtk_widget_queue_resize( win->m_widget );

    return FALSE;
}

//-----------------------------------------------------------------------------
// "key_press_event"
//-----------------------------------------------------------------------------

static gint gtk_notebook_key_press_callback( GtkWidget *widget, GdkEventKey *gdk_event, wxNotebook *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT) return FALSE;
    if (g_blockEventsOnDrag) return FALSE;

    /* win is a control: tab can be propagated up */
    if ((gdk_event->keyval == GDK_Tab) || (gdk_event->keyval == GDK_ISO_Left_Tab))
    {
        int sel = win->GetSelection();
        if (sel == -1)
            return TRUE;
        wxGtkNotebookPage *nb_page = win->GetNotebookPage(sel);
        wxCHECK_MSG( nb_page, FALSE, _T("invalid selection in wxNotebook") );

        wxNavigationKeyEvent event;
        event.SetEventObject( win );
        /* GDK reports GDK_ISO_Left_Tab for SHIFT-TAB */
        event.SetDirection( (gdk_event->keyval == GDK_Tab) );
        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
        event.SetWindowChange( (gdk_event->state & GDK_CONTROL_MASK) );
        event.SetCurrentFocus( win );

        wxNotebookPage *client = win->GetPage(sel);
        if ( !client->GetEventHandler()->ProcessEvent( event ) )
        {
             client->SetFocus();
        }

        gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "key_press_event" );
        return TRUE;
    }

    return FALSE;
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
    m_padding = 0;
    m_inSwitchPage = FALSE;

    m_imageList = (wxImageList *) NULL;
    m_selection = -1;
    m_themeEnabled = TRUE;
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
        wxFAIL_MSG( wxT("wxNoteBook creation failed") );
        return FALSE;
    }


    m_widget = gtk_notebook_new();

    gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

    gtk_signal_connect( GTK_OBJECT(m_widget), "switch_page",
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer)this );

    m_parent->DoAddChild( this );

    if (m_windowStyle & wxNB_RIGHT)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_RIGHT );
    if (m_windowStyle & wxNB_LEFT)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_LEFT );
    if (m_windowStyle & wxNB_BOTTOM)
        gtk_notebook_set_tab_pos( GTK_NOTEBOOK(m_widget), GTK_POS_BOTTOM );

    gtk_signal_connect( GTK_OBJECT(m_widget), "key_press_event",
      GTK_SIGNAL_FUNC(gtk_notebook_key_press_callback), (gpointer)this );

    PostCreation();

    SetFont( parent->GetFont() );

    gtk_signal_connect( GTK_OBJECT(m_widget), "realize",
                            GTK_SIGNAL_FUNC(gtk_notebook_realized_callback), (gpointer) this );

    Show( TRUE );

    return TRUE;
}

int wxNotebook::GetSelection() const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid notebook") );

    if ( m_selection == -1 )
    {
        GList *nb_pages = GTK_NOTEBOOK(m_widget)->children;

        if (g_list_length(nb_pages) != 0)
        {
            GtkNotebook *notebook = GTK_NOTEBOOK(m_widget);

            gpointer cur = notebook->cur_page;
            if ( cur != NULL )
            {
                wxConstCast(this, wxNotebook)->m_selection =
                    g_list_index( nb_pages, cur );
            }
        }
    }

    return m_selection;
}

wxString wxNotebook::GetPageText( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, wxT(""), wxT("invalid notebook") );

    wxGtkNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_text;
    else
        return wxT("");
}

int wxNotebook::GetPageImage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid notebook") );

    wxGtkNotebookPage* nb_page = GetNotebookPage(page);
    if (nb_page)
        return nb_page->m_image;
    else
        return -1;
}

wxGtkNotebookPage* wxNotebook::GetNotebookPage( int page ) const
{
    wxCHECK_MSG( m_widget != NULL, (wxGtkNotebookPage*) NULL, wxT("invalid notebook") );

    wxCHECK_MSG( page < (int)m_pagesData.GetCount(), (wxGtkNotebookPage*) NULL, wxT("invalid notebook index") );

    return m_pagesData.Item(page)->GetData();
}

int wxNotebook::SetSelection( int page )
{
    wxCHECK_MSG( m_widget != NULL, -1, wxT("invalid notebook") );

    wxCHECK_MSG( page >= 0 && page < (int)m_pagesData.GetCount(), -1, wxT("invalid notebook index") );

    int selOld = GetSelection();

    // cache the selection
    m_selection = page;
    gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page );

    wxNotebookPage *client = GetPage(page);
    if ( client )
        client->SetFocus();

    return selOld;
}

bool wxNotebook::SetPageText( int page, const wxString &text )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid notebook") );

    wxGtkNotebookPage* nb_page = GetNotebookPage(page);

    wxCHECK_MSG( nb_page, FALSE, wxT("SetPageText: invalid page index") );

    nb_page->m_text = text;

    gtk_label_set( nb_page->m_label, wxGTK_CONV( nb_page->m_text ) );

    return TRUE;
}

bool wxNotebook::SetPageImage( int page, int image )
{
    /* HvdH 28-12-98: now it works, but it's a bit of a kludge */

    wxGtkNotebookPage* nb_page = GetNotebookPage(page);

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
        gtk_box_pack_start(GTK_BOX(nb_page->m_box), pixmapwid, FALSE, FALSE, m_padding);
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
    wxFAIL_MSG( wxT("wxNotebook::SetPageSize not implemented") );
}

void wxNotebook::SetPadding( const wxSize &padding )
{
    wxCHECK_RET( m_widget != NULL, wxT("invalid notebook") );

    m_padding = padding.GetWidth();

    int i;
    for (i=0; i<int(GetPageCount()); i++)
    {
        wxGtkNotebookPage* nb_page = GetNotebookPage(i);
        wxASSERT(nb_page != NULL);

        if (nb_page->m_image != -1)
        {
            // gtk_box_set_child_packing sets padding on BOTH sides
            // icon provides left padding, label provides center and right
            int image = nb_page->m_image;
            SetPageImage(i,-1);
            SetPageImage(i,image);
        }
        wxASSERT(nb_page->m_label);
        gtk_box_set_child_packing(GTK_BOX(nb_page->m_box),
                                  GTK_WIDGET(nb_page->m_label),
                                  FALSE, FALSE, m_padding, GTK_PACK_END);
    }
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxNotebook::SetTabSize not implemented") );
}

bool wxNotebook::DeleteAllPages()
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid notebook") );

    while (m_pagesData.GetCount() > 0)
        DeletePage( m_pagesData.GetCount()-1 );

    wxASSERT_MSG( GetPageCount() == 0, _T("all pages must have been deleted") );

    return wxNotebookBase::DeleteAllPages();
}

bool wxNotebook::DeletePage( int page )
{
    // GTK sets GtkNotebook.cur_page to NULL before sending the switch page
    // event so we have to store the selection internally
    if ( m_selection == -1 )
    {
        m_selection = GetSelection();
        if ( m_selection == (int)m_pagesData.GetCount() - 1 )
        {
            // the index will become invalid after the page is deleted
            m_selection = -1;
        }
    }

    // it will call our DoRemovePage() to do the real work
    return wxNotebookBase::DeletePage(page);
}

wxNotebookPage *wxNotebook::DoRemovePage( int page )
{
    wxNotebookPage *client = wxNotebookBase::DoRemovePage(page);
    if ( !client )
        return NULL;

    gtk_widget_ref( client->m_widget );
    gtk_widget_unrealize( client->m_widget );
    gtk_widget_unparent( client->m_widget );

    gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page );

    wxGtkNotebookPage* p = GetNotebookPage(page);
    m_pagesData.DeleteObject(p);
    delete p;

    return client;
}

bool wxNotebook::InsertPage( int position,
                             wxNotebookPage* win,
                             const wxString& text,
                             bool select,
                             int imageId )
{
    wxCHECK_MSG( m_widget != NULL, FALSE, wxT("invalid notebook") );

    wxCHECK_MSG( win->GetParent() == this, FALSE,
               wxT("Can't add a page whose parent is not the notebook!") );

    wxCHECK_MSG( position >= 0 && position <= GetPageCount(), FALSE,
                 _T("invalid page index in wxNotebookPage::InsertPage()") );

    /* don't receive switch page during addition */
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
      GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback), (gpointer) this );

    if (m_themeEnabled)
        win->SetThemeEnabled(TRUE);

    GtkNotebook *notebook = GTK_NOTEBOOK(m_widget);

    wxGtkNotebookPage *nb_page = new wxGtkNotebookPage();

    if ( position == GetPageCount() )
        m_pagesData.Append( nb_page );
    else
        m_pagesData.Insert( m_pagesData.Item( position ), nb_page );

    m_pages.Insert(win, position);

    nb_page->m_box = gtk_hbox_new( FALSE, 1 );
    gtk_container_border_width( GTK_CONTAINER(nb_page->m_box), 2 );

    gtk_signal_connect( GTK_OBJECT(win->m_widget), "size_allocate",
      GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)win );

    if (position < 0)
        gtk_notebook_append_page( notebook, win->m_widget, nb_page->m_box );
    else
        gtk_notebook_insert_page( notebook, win->m_widget, nb_page->m_box, position );

    nb_page->m_page = (GtkNotebookPage*) g_list_last(notebook->children)->data;

    /* set the label image */
    nb_page->m_image = imageId;

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

        gtk_box_pack_start(GTK_BOX(nb_page->m_box), pixmapwid, FALSE, FALSE, m_padding);

        gtk_widget_show(pixmapwid);
    }

    /* set the label text */
    nb_page->m_text = text;
    if (nb_page->m_text.IsEmpty()) nb_page->m_text = wxT("");
  
    nb_page->m_label = GTK_LABEL( gtk_label_new(nb_page->m_text.mbc_str()) );
    gtk_box_pack_end( GTK_BOX(nb_page->m_box), GTK_WIDGET(nb_page->m_label), FALSE, FALSE, m_padding );
  
    /* show the label */
    gtk_widget_show( GTK_WIDGET(nb_page->m_label) );
    if (select && (m_pagesData.GetCount() > 1))
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

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if (event.IsWindowChange())
        AdvanceSelection( event.GetDirection() );
    else
        event.Skip();
}

#if wxUSE_CONSTRAINTS

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

#endif

void wxNotebook::ApplyWidgetStyle()
{
    // TODO, font for labels etc

    SetWidgetStyle();
    gtk_widget_set_style( m_widget, m_widgetStyle );
}

bool wxNotebook::IsOwnGtkWindow( GdkWindow *window )
{
    return ((m_widget->window == window) ||
            (NOTEBOOK_PANEL(m_widget) == window));
}

//-----------------------------------------------------------------------------
// wxNotebookEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

#endif
