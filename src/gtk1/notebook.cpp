/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

#include "wx/notebook.h"
#include "wx/panel.h"
#include "wx/utils.h"
#include "wx/imaglist.h"

//-----------------------------------------------------------------------------
// wxNotebookPage
//-----------------------------------------------------------------------------

class wxNotebookPage: public wxObject
{
  public:
  
   int                m_id;
   wxString           m_text;
   int                m_image;
   void              *m_clientData;
   GtkNotebookPage   *m_page;
   wxPanel           *m_clientPanel;
   
   wxNotebookPage(void)
   {
     m_id = -1;
     m_text = "";
     m_image = -1;
     m_clientData = NULL;
     m_page = NULL;
     m_clientPanel = NULL;
   };
   
};

//-----------------------------------------------------------------------------
// wxNotebook
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNotebook,wxControl)

wxNotebook::wxNotebook(void)
{
  m_imageList = NULL;
  m_frame = NULL;
  m_pages.DeleteContents( TRUE );
};

wxNotebook::wxNotebook( wxWindow *parent, const wxWindowID id, 
      const wxPoint& pos, const wxSize& size,
      const long style, const wxString& name )
{
  m_imageList = NULL;
  m_frame = NULL;
  m_pages.DeleteContents( TRUE );
  Create( parent, id, pos, size, style, name );
};

wxNotebook::~wxNotebook(void)
{
  if (m_imageList) delete m_imageList;
  DeleteAllPages();
};

bool wxNotebook::Create(wxWindow *parent, const wxWindowID id, 
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

int wxNotebook::GetSelection(void) const
{
  if (m_pages.Number() == 0) return -1;

  GtkNotebookPage *g_page = GTK_NOTEBOOK(m_widget)->cur_page;
  
  wxNotebookPage *page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    page = (wxNotebookPage*)node->Data();
    if (page->m_page == g_page) break;
    node = node->Next();
  };
  
  if (!node) wxFatalError( "Notebook error." );
  
  return page->m_id;
};

wxImageList* wxNotebook::GetImageList(void) const
{
  return m_imageList;
};

int wxNotebook::GetPageCount(void) const
{
  return m_pages.Number();
};

int wxNotebook::GetRowCount(void) const
{
  return 1;
};

wxString wxNotebook::GetPageText( const int page ) const
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return "";
  
  return nb_page->m_text;
};

int wxNotebook::GetPageImage( const int page ) const
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return -1;
  
  return nb_page->m_image;
};

void* wxNotebook::GetPageData( const int page ) const
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return NULL;
  
  return nb_page->m_clientData;
};

int wxNotebook::SetSelection( const int page )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return -1;
  
  int page_num = 0;
  GList *child = GTK_NOTEBOOK(m_widget)->children;
  while (child)
  {
    if (nb_page->m_page == (GtkNotebookPage*)child->data) break;
    page_num++;
    child = child->next;
  };
   
  if (!child) return -1;
  
  gtk_notebook_set_page( GTK_NOTEBOOK(m_widget), page_num );
  
  return page;
};

void wxNotebook::SetImageList( wxImageList* imageList )
{
  m_imageList = imageList;
};

bool wxNotebook::SetPageText( const int page, const wxString &text )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return FALSE;
  
  nb_page->m_text = text;
 
  // recreate
   
  return TRUE;
};

bool wxNotebook::SetPageImage( const int page, const int image )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return FALSE;
  
  nb_page->m_image = image;
 
  // recreate
   
  return TRUE;
};

bool wxNotebook::SetPageData( const int page, void* data )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return FALSE;
  
  nb_page->m_clientData = data;
  
  return TRUE;
};

void wxNotebook::SetPageSize( const wxSize &WXUNUSED(size) )
{
};

void wxNotebook::SetPadding( const wxSize &WXUNUSED(padding) )
{
 //  what's this ?
};

bool wxNotebook::DeleteAllPages(void)
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

bool wxNotebook::DeletePage( const int page )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return FALSE;

  int page_num = 0;
  GList *child = GTK_NOTEBOOK(m_widget)->children;
  while (child)
  {
    if (nb_page->m_page == (GtkNotebookPage*)child->data) break;
    page_num++;
    child = child->next;
  };

  wxASSERT( child );
        
  delete nb_page->m_clientPanel;
  
//  Amazingly, this is not necessary
//  gtk_notebook_remove_page( GTK_NOTEBOOK(m_widget), page_num );
  
  m_pages.DeleteObject( nb_page );
    
  return TRUE;
};

wxPanel *wxNotebook::CreatePage( const int item, const wxString &text, const int imageId, void* data )
{
  wxNotebookPage *page = new wxNotebookPage;
  
  page->m_text = text;
  if (page->m_text.IsNull()) page->m_text = "";
  page->m_id = item;
  page->m_image = imageId;
  page->m_clientData = data;
  
  m_frame = gtk_label_new( page->m_text );
  gtk_misc_set_alignment( GTK_MISC(m_frame), 0.0, 0.5 );

  page->m_clientPanel = new wxPanel( this, -1, wxPoint(0,0), wxSize(100,100) );
    
  m_frame = NULL;
  
  page->m_page = (GtkNotebookPage*)( g_list_last( GTK_NOTEBOOK(m_widget)->children )->data );
  
  if (!page->m_page) wxFatalError( "Notebook page creation error" );
  
  m_pages.Append( page );

  return page->m_clientPanel;
};

wxPanel *wxNotebook::GetPanel( const int page )
{
  wxNotebookPage *nb_page = NULL;

  wxNode *node = m_pages.First();
  while (node)
  {
    nb_page = (wxNotebookPage*)node->Data();
    if (nb_page->m_id == page) break;
    node = node->Next();
  };
  if (!node) return NULL;
  
  return nb_page->m_clientPanel;
};

void wxNotebook::AddChild( wxWindow *win )
{
  if (!m_frame) wxFatalError( "Notebook::Addchild() must not be called directly." );
  
  gtk_notebook_append_page( GTK_NOTEBOOK(m_widget), win->m_widget, m_frame );
};

//-----------------------------------------------------------------------------
// wxTabEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxCommandEvent)

wxTabEvent::wxTabEvent( WXTYPE commandType, int id ) :
  wxCommandEvent(commandType, id)
{
};

