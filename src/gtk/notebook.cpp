/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
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
    m_page = NULL;
    m_client = NULL;
    m_parent = NULL;
  };

//private:
  int                m_id;
  wxString           m_text;
  int                m_image;
  GtkNotebookPage   *m_page;
  GtkLabel          *m_label;
  wxWindow          *m_client;
  GtkNotebook       *m_parent;
};

//-----------------------------------------------------------------------------
// GTK callbacks
//-----------------------------------------------------------------------------

// page change callback
static void gtk_notebook_page_change_callback(GtkNotebook *WXUNUSED(widget),
                                              GtkNotebookPage *WXUNUSED(page),
                                              gint nPage,
                                              gpointer data)
{
  wxNotebook *notebook = (wxNotebook *)data;

  int nOld = notebook->GetSelection();

  // TODO: emulate PAGE_CHANGING event
  wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
                        notebook->GetId(),
                        nPage,
                        nOld);
  event.SetEventObject(notebook);
  notebook->ProcessEvent(event);
}

static void gtk_page_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxWindow *win )
{
  if ( win->GetAutoLayout() )
    win->Layout();

  if ((win->m_x == alloc->x) &&
      (win->m_y == alloc->y) &&
      (win->m_width == alloc->width) &&
      (win->m_height == alloc->height))
  {
    return;
  };

/*
  printf( "OnResize from " );
  if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
    printf( win->GetClassInfo()->GetClassName() );
  printf( " .\n" );

  printf( "  Old: X: %d  Y: %d ", win->m_x, win->m_y );
  printf( "  W: %d  H: %d ", win->m_width, win->m_height );
  printf( " .\n" );

  printf( "  New: X: %d  Y: %d ", alloc->x, alloc->y );
  printf( "  W: %d  H: %d ", alloc->width, alloc->height );
  printf( " .\n" );
*/

  win->SetSize( alloc->x, alloc->y, alloc->width, alloc->height );

/*
  printf( "  Res: X: %d  Y: %d ", win->m_x, win->m_y );
  printf( "  W: %d  H: %d ", win->m_width, win->m_height );
  printf( " .\n" );
*/
};

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebook,wxControl)

void wxNotebook::Init()
{
  m_imageList = NULL;
  m_pages.DeleteContents( TRUE );
  m_idHandler = 0;
}

wxNotebook::wxNotebook()
{
  Init();
};

wxNotebook::wxNotebook( wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  Init();
  Create( parent, id, pos, size, style, name );
};

wxNotebook::~wxNotebook()
{
  // don't generate change page events any more
  if ( m_idHandler != 0 )
    gtk_signal_disconnect(GTK_OBJECT(m_widget), m_idHandler);

  DeleteAllPages();
};

bool wxNotebook::Create(wxWindow *parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size,
      long style, const wxString& name )
{
  m_needParent = TRUE;

  PreCreation( parent, id, pos, size, style, name );

  m_widget = gtk_notebook_new();

  gtk_notebook_set_scrollable( GTK_NOTEBOOK(m_widget), 1 );

  m_idHandler = gtk_signal_connect
                (
                  GTK_OBJECT(m_widget), "switch_page",
                  GTK_SIGNAL_FUNC(gtk_notebook_page_change_callback),
                  (gpointer)this
                );

  PostCreation();

  Show( TRUE );

  return TRUE;
};

int wxNotebook::GetSelection() const
{
  if (m_pages.Number() == 0)
    return -1;

  GtkNotebookPage *g_page = GTK_NOTEBOOK(m_widget)->cur_page;

  wxNotebookPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxNotebookPage*)node->Data();
    if (page->m_page == g_page)
      break;
    node = node->Next();
  };

  wxCHECK_MSG( node != NULL, -1, "wxNotebook: no selection?" );

  return page->m_id;
};

int wxNotebook::GetPageCount() const
{
  return m_pages.Number();
};

int wxNotebook::GetRowCount() const
{
  return 1;
};

wxString wxNotebook::GetPageText( int page ) const
{
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (nb_page)
    return nb_page->m_text;
  else
    return "";
};

int wxNotebook::GetPageImage( int page ) const
{
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (nb_page)
    return nb_page->m_image;
  else
    return 0;
};

wxNotebookPage* wxNotebook::GetNotebookPage(int page) const
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page)
      return nb_page;
    node = node->Next();
  };

  wxLogDebug( "Notebook page %d not found!", page );

  return NULL;
};

int wxNotebook::SetSelection( int page )
{
  int selOld = GetSelection();
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (!nb_page)
    return -1;

  int page_num = 0;
  GList *child = GTK_NOTEBOOK(m_widget)->children;
  while (child)
  {
    if (nb_page->m_page == (GtkNotebookPage*)child->data)
      break;
    page_num++;
    child = child->next;
  };

  if (!child) return -1;

  gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page_num );

  return selOld;
};

void wxNotebook::AdvanceSelection(bool bForward)
{
  int nSel = GetSelection(),
      nMax = GetPageCount();

  if ( bForward ) {
    SetSelection(nSel == nMax ? 0 : nSel + 1);
  }
  else {
    SetSelection(nSel == 0 ? nMax : nSel - 1);
  }
}

void wxNotebook::SetImageList( wxImageList* imageList )
{
  m_imageList = imageList;
};

bool wxNotebook::SetPageText( int page, const wxString &text )
{
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (!nb_page)
    return FALSE;

  nb_page->m_text = text;

  return TRUE;
};

bool wxNotebook::SetPageImage( int page, int image )
{
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (!nb_page)
    return FALSE;

  nb_page->m_image = image;

  return TRUE;
};

void wxNotebook::SetPageSize( const wxSize &WXUNUSED(size) )
{
  wxFAIL_MSG(_("wxNotebook::SetPageSize not implemented"));
};

void wxNotebook::SetPadding( const wxSize &WXUNUSED(padding) )
{
  wxFAIL_MSG(_("wxNotebook::SetPadding not implemented"));
};

bool wxNotebook::DeleteAllPages()
{
  wxNode *page_node = m_pages.First();
  while (page_node)
  {
    wxNotebookPage *page = (wxNotebookPage*)page_node->Data();

    DeletePage( page->m_id );

    page_node = m_pages.First();
  };

  return TRUE;
};

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
  };

  wxASSERT( child );

  delete nb_page->m_client;

//  Amazingly, this is not necessary
//  gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page_num );

  m_pages.DeleteObject( nb_page );

  return TRUE;
};

bool wxNotebook::AddPage(wxWindow* win, const wxString& text,
                         bool bSelect, int imageId)
{
  // we've created the notebook page in AddChild(). Now we just have to set
  // the caption for the page and set the others parameters.

  // first, find the page
  wxNotebookPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxNotebookPage*)node->Data();
    if ( page->m_client == win )
      break; // found
    node = node->Next();
  };

  wxCHECK_MSG(page != NULL, FALSE,
              _("Can't add a page whose parent is not the notebook!"));

  // then set the attributes
  page->m_text = text;
  if ( page->m_text.IsEmpty() )
    page->m_text = "";
  page->m_image = imageId;
  gtk_label_set(page->m_label, page->m_text);

  if ( bSelect ) {
    SetSelection(GetPageCount());
  }

  return TRUE;
};

wxWindow *wxNotebook::GetPage( int page ) const
{
  wxNotebookPage* nb_page = GetNotebookPage(page);
  if (!nb_page)
    return NULL;
  else
    return nb_page->m_client;
};

void wxNotebook::AddChild( wxWindow *win )
{
  m_children.Append(win);

  wxNotebookPage *page = new wxNotebookPage();

  page->m_id = GetPageCount();
  page->m_label = (GtkLabel *)gtk_label_new(_("Handle"));
  page->m_client = win;
  gtk_notebook_append_page( GTK_NOTEBOOK(m_widget), win->m_widget,
                            (GtkWidget *)page->m_label);
  gtk_misc_set_alignment(GTK_MISC(page->m_label), 0.0, 0.5);

  page->m_page =
    (GtkNotebookPage*) (g_list_last(GTK_NOTEBOOK(m_widget)->children)->data);

  page->m_parent = GTK_NOTEBOOK(m_widget);

  gtk_signal_connect( GTK_OBJECT(win->m_widget), "size_allocate",
    GTK_SIGNAL_FUNC(gtk_page_size_callback), (gpointer)win );

  if (!page->m_page)
  {
     wxLogFatalError( _("Notebook page creation error") );
     return;
  }

  m_pages.Append( page );
};

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

//-----------------------------------------------------------------------------
// wxNotebookEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)
