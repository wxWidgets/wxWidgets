/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tabctrl.h"
#endif

#include "wx/tabctrl.h"
#include "wx/utils.h"
#include "wx/imaglist.h"

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// wxTabPage
//-----------------------------------------------------------------------------

class wxTabPage: public wxObject
{
  public:
  
   int                m_id;
   wxString           m_text;
   int                m_image;
   void              *m_clientData;
   GtkNotebookPage   *m_page;
   GtkWidget         *m_wxwindow;
   wxList             m_children;
   
   wxTabPage(void)
   {
     m_id = -1;
     m_text = "";
     m_image = -1;
     m_clientData = NULL;
     m_page = NULL;
     m_wxwindow = NULL;
   };
   
};

//-----------------------------------------------------------------------------
// wxTabCtrl
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl,wxControl)

wxTabCtrl::wxTabCtrl(void)
{
  m_imageList = NULL;
  m_pages.DeleteContents( TRUE );
};

wxTabCtrl::wxTabCtrl( wxWindow *parent, const wxWindowID id, 
      const wxPoint& pos, const wxSize& size,
      const long style, const wxString& name )
{
  m_imageList = NULL;
  m_pages.DeleteContents( TRUE );
  Create( parent, id, pos, size, style, name );
};

wxTabCtrl::~wxTabCtrl(void)
{
  if (m_imageList) delete m_imageList;
  DeleteAllItems();
};

bool wxTabCtrl::Create(wxWindow *parent, const wxWindowID id, 
      const wxPoint& pos, const wxSize& size,
      const long style, const wxString& name )
{
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );

  m_widget = gtk_notebook_new();
  
  PostCreation();
  
  Show( TRUE );
  
  return TRUE;
};

int wxTabCtrl::GetSelection(void) const
{
  return gtk_notebook_current_page( GTK_NOTEBOOK(m_widget) );
};

wxImageList* wxTabCtrl::GetImageList(void) const
{
  return m_imageList;
};

int wxTabCtrl::GetItemCount(void) const
{
  return m_pages.Number();
};

bool wxTabCtrl::GetItemRect( const int WXUNUSED(item), wxRect& WXUNUSED(rect) ) const
{
  return FALSE;
};

int wxTabCtrl::GetRowCount(void) const
{
  return 1;
};

wxString wxTabCtrl::GetItemText( const int item ) const
{
  wxNode *node = m_pages.Nth( item );
  if (node)
  {
    wxTabPage *page = (wxTabPage*)node->Data();
    return page->m_text;
  };
  return "";
};

int wxTabCtrl::GetItemImage( const int item ) const
{
  wxNode *node = m_pages.Nth( item );
  if (node)
  {
    wxTabPage *page = (wxTabPage*)node->Data();
    return page->m_image;
  };
  return -1;
};

void* wxTabCtrl::GetItemData( const int item ) const
{
  wxNode *node = m_pages.Nth( item );
  if (node)
  {
    wxTabPage *page = (wxTabPage*)node->Data();
    return page->m_clientData;
  };
  return NULL;
};

int wxTabCtrl::SetSelection( const int item )
{
  wxTabPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxTabPage*)node->Data();
    if (page->m_id == item) break;
    page = NULL;
    node = node->Next();
  };
  if (!page) return -1;
  
  int page_num = 0;
  GList *child = GTK_NOTEBOOK(m_widget)->children;
  while (child)
  {
    if (page->m_page == (GtkNotebookPage*)child->data) break;
    page_num++;
    child = child->next;
  };
   
  if (!child) return -1;
  
  gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page_num );
  
  return item;
};

void wxTabCtrl::SetImageList( wxImageList* imageList )
{
  m_imageList = imageList;
};

bool wxTabCtrl::SetItemText( const int WXUNUSED(item), const wxString& WXUNUSED(text) )
{
  return TRUE;
};

bool wxTabCtrl::SetItemImage( const int WXUNUSED(item), const int WXUNUSED(image) )
{
  return TRUE;
};

bool wxTabCtrl::SetItemData( const int WXUNUSED(item), void* WXUNUSED(data) )
{
  return TRUE;
};

void wxTabCtrl::SetItemSize( const wxSize &WXUNUSED(size) )
{
};

void wxTabCtrl::SetPadding( const wxSize &WXUNUSED(padding) )
{
};

bool wxTabCtrl::DeleteAllItems(void)
{
  wxNode *page_node = m_pages.First();
  while (page_node)
  {
    wxTabPage *page = (wxTabPage*)page_node->Data();
    
    wxNode *node = page->m_children.First();
    while (node)
    {
      wxWindow *child = (wxWindow*)node->Data();
      delete child;
      
      node = node->Next();
    };
    
    gtk_widget_destroy( page->m_wxwindow );
  
    m_pages.DeleteObject( page );

    page_node = m_pages.First();
  };
  
  return TRUE;
};

bool wxTabCtrl::DeleteItem( const int item )
{
  wxTabPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxTabPage*)node->Data();
    if (page->m_id == item) break;
    node = node->Next();
  };
  if (!node) return FALSE;

  node = page->m_children.First();
  while (node)
  {
    wxWindow *child = (wxWindow*)node->Data();
    delete child;
      
    node = node->Next();
  };
  
  gtk_widget_destroy( page->m_wxwindow );
  
  m_pages.DeleteObject( page );
    
  return TRUE;
};

int wxTabCtrl::HitTest( const wxPoint &WXUNUSED(pt), long &WXUNUSED(flags) )
{
  return wxTAB_HITTEST_NOWHERE;
};

int wxTabCtrl::InsertItem( const int item, const wxString &text, const int imageId, void* data )
{
  wxTabPage *page = new wxTabPage;
  
  page->m_text = text;
  if (page->m_text.IsNull()) page->m_text = "";
  page->m_id = item;
  page->m_image = imageId;
  page->m_clientData = data;
  
  GtkWidget *label_widget = gtk_label_new( page->m_text );
  gtk_misc_set_alignment( GTK_MISC(label_widget), 0.0, 0.5 );

  page->m_wxwindow = gtk_myfixed_new();
  gtk_widget_show( page->m_wxwindow );
  gtk_widget_set_usize( page->m_wxwindow, 100, 100 );
  
  gtk_notebook_append_page( GTK_NOTEBOOK(m_widget), page->m_wxwindow, label_widget );
  
  page->m_page = GTK_NOTEBOOK(m_widget)->cur_page;
  
  m_pages.Append( page );
  
  return item;
};

void wxTabCtrl::AddChild( wxWindow *win )
{
  GtkNotebookPage *g_page = GTK_NOTEBOOK(m_widget)->cur_page;
  
  wxTabPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxTabPage*)node->Data();
    if (page->m_page == g_page) break;
    node = node->Next();
  };
  
  if (!page) wxFatalError( "wxTabCtrl error" );
  
  gtk_myfixed_put( GTK_MYFIXED(page->m_wxwindow), win->m_widget, win->m_x, win->m_y );
  gtk_widget_set_usize( win->m_widget, win->m_width, win->m_height );
  
  page->m_children.Append( win );
};

//-----------------------------------------------------------------------------
// wxTabEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxCommandEvent)

wxTabEvent::wxTabEvent( WXTYPE commandType, int id ) :
  wxCommandEvent(commandType, id)
{
};