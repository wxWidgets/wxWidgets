/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:
// Author:      Denis Pershin
// Created:     07/05/98
// Id:          $Id$
// Copyright:   (c) 1998 Denis Pershin and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "treectrl.h"
#endif

#include "wx/gtk/treectrl.h"
#include "wx/settings.h"
#include "wx/log.h"

#ifdef USE_GDK_IMLIB
#include "../gdk_imlib/gdk_imlib.h"
#endif

//-----------------------------------------------------------------------------
// wxTreeItem
//-----------------------------------------------------------------------------

// clicked

void gtk_treeitem_expand_callback(GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem) {
  treeitem->SendExpanding(treeitem->m_owner);
  treeitem->SendExpand(treeitem->m_owner);
};

void gtk_treeitem_collapse_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem) {
  treeitem->SendCollapsing(treeitem->m_owner);
  treeitem->SendCollapse(treeitem->m_owner);
};

void gtk_treeitem_select_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem) {
  treeitem->SendSelChanging(treeitem->m_owner);
  treeitem->m_state |= wxTREE_STATE_SELECTED;
  treeitem->SendSelChanged(treeitem->m_owner);
}

void gtk_treeitem_deselect_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem) {
  treeitem->SendSelChanging(treeitem->m_owner);
  treeitem->m_state &= !wxTREE_STATE_SELECTED;
  treeitem->SendSelChanged(treeitem->m_owner);
}

IMPLEMENT_DYNAMIC_CLASS(wxTreeItem, wxObject)

wxTreeItem::wxTreeItem() {
  m_mask = 0;
  m_itemId = 0;
  m_state = 0;
  m_stateMask = 0;
  m_image = -1;
  m_children = 0;
  m_selectedImage = -1;
  m_tree = NULL;
  m_parentwidget = NULL;
  m_widget = NULL;
  m_data = 0;
  m_owner = 0;
  expand_handler = 0;
  collapse_handler = 0;
};

wxTreeItem::wxTreeItem(GtkWidget *parent, const wxTreeItem &item) {
  m_mask = item.m_mask;
  m_text = item.m_text;
  m_itemId = item.m_itemId;
  m_state = item.m_state;
  m_stateMask = item.m_stateMask;
  m_image = item.m_image;
  m_tree = NULL;
  m_owner = 0;
  m_selectedImage = item.m_selectedImage;
  m_children = item.m_children;
  m_childlist = item.m_childlist;
  m_data = item.m_data;
  m_parentwidget = parent;
  expand_handler = 0;
  collapse_handler = 0;

  Create();
}

void wxTreeItem::Create() {
  wxASSERT(m_parentwidget != NULL);

  m_widget = GTK_TREE_ITEM(gtk_tree_item_new_with_label ((char *)(const char *)m_text));
  gtk_container_add (GTK_CONTAINER(m_parentwidget), GTK_WIDGET(m_widget));
  gtk_widget_show(GTK_WIDGET(m_widget));

  gtk_signal_connect(GTK_OBJECT(m_widget), "select",
      GTK_SIGNAL_FUNC(gtk_treeitem_select_callback), (gpointer)this );

  gtk_signal_connect(GTK_OBJECT(m_widget), "deselect",
      GTK_SIGNAL_FUNC(gtk_treeitem_deselect_callback), (gpointer)this );

  if ((m_mask & wxTREE_MASK_CHILDREN) != 0)
    AddSubtree();
}

wxTreeItem::~wxTreeItem() {
  if (m_owner != NULL)
    SendDelete(m_owner);
  DeleteChildren();
  if ((m_widget != NULL) && (m_parentwidget != NULL))
    gtk_container_remove(GTK_CONTAINER(m_parentwidget),
                         GTK_WIDGET(m_widget));
//  if (m_tree != NULL) {
//    gtk_widget_destroy(GTK_WIDGET(m_tree));
//    m_tree = NULL;
//  }
    
//  if (m_widget != NULL)
//    gtk_widget_destroy(GTK_WIDGET(m_widget));
}

void wxTreeItem::AddSubtree() {
  if (m_widget == NULL)
    return;

  m_tree = GTK_TREE(gtk_tree_new());
   
  if (expand_handler == 0)
    expand_handler = gtk_signal_connect(GTK_OBJECT(m_widget), "expand",
      GTK_SIGNAL_FUNC(gtk_treeitem_expand_callback), (gpointer)this );

  if (collapse_handler == 0)
    collapse_handler = gtk_signal_connect( GTK_OBJECT(m_widget), "collapse",
      GTK_SIGNAL_FUNC(gtk_treeitem_collapse_callback), (gpointer)this );

  gtk_tree_item_set_subtree(GTK_TREE_ITEM(m_widget), GTK_WIDGET(m_tree));
  gtk_widget_show(GTK_WIDGET(m_tree));
}

void wxTreeItem::AddChild(wxTreeItem *child) {
  wxASSERT(child != NULL);

  m_childlist.Append(child);
}

bool wxTreeItem::HasChildren() {
  return (m_childlist.Number() != 0);
}

void wxTreeItem::DeleteChildren() {
  wxTreeItem *item;
  long no = GetChildrenNumber();
  for (long i=0; i<no; i++)
    if ((item = GetChild(i)) != 0)
      delete item;
  m_childlist.Clear();
  m_tree = NULL;
  
  if ((m_mask & wxTREE_MASK_CHILDREN) != 0)
    if (m_widget != NULL)
      if (m_widget->subtree == 0)
        AddSubtree();
}

int wxTreeItem::NumberOfVisibleDescendents() {
  wxTreeItem *item;
  long no = GetChildrenNumber();
  long num = 0;
  for (long i=0; i<no; i++)
    if ((item = GetChild(i)) != 0)
      num += item->NumberOfVisibleDescendents();
  
  num+=no;

  return num;
}

wxTreeItem *wxTreeItem::FindItem(long itemId) const {
  if (m_itemId == itemId) return (wxTreeItem*)(this);
  wxNode *node = m_childlist.First();
  while (node) {
    wxTreeItem *item = (wxTreeItem*)node->Data();
    wxTreeItem *res = item->FindItem( itemId );
    if (res) return (wxTreeItem*)(res);
    node = node->Next();
  };
  return NULL;
};

wxTreeItem *wxTreeItem::FindItem(GtkTreeItem *item) const {
  if (m_widget == item)
    return (wxTreeItem*)(this);
  wxNode *node = m_childlist.First();
  while (node) {
    wxTreeItem *i = (wxTreeItem*)node->Data();
    wxTreeItem *res = i->FindItem(item);
    if (res) return (wxTreeItem*)(res);
    node = node->Next();
  };
  return NULL;
};

void wxTreeItem::PrepareEvent(wxTreeEvent &event) {
  event.m_item.m_itemId = m_itemId;
  event.m_item.m_state = m_state;
  event.m_item.m_text = m_text;
  event.m_item.m_image = m_image;
  event.m_item.m_selectedImage = m_selectedImage;
  event.m_item.m_children = (GetChildrenNumber() > 0);
  event.m_item.m_data = m_data;
  event.m_oldItem = 0;
  event.m_code = 0;
//  event.m_pointDrag.x = 0;
//  event.m_pointDrag.y = 0;
};

void wxTreeItem::SendDelete(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_DELETE_ITEM, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendExpand(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_EXPANDED, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendExpanding(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_EXPANDING, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendCollapse(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendCollapsing(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendSelChanged(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGED, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

void wxTreeItem::SendSelChanging(wxWindow *target) {
  wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGING, target->GetId());
  PrepareEvent(event);
  event.SetEventObject(target);
  target->ProcessEvent(event);
};

//-----------------------------------------------------------------------------
// wxTreeCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxScrolledWindow)

wxTreeCtrl::wxTreeCtrl() {
  m_current = NULL;
  m_lastId = 0;
  m_isCreated = FALSE;
  m_imageList = NULL;
  m_smallImageList = NULL;
};

wxTreeCtrl::wxTreeCtrl(wxWindow *parent, wxWindowID id,
            const wxPoint& pos, const wxSize& size,
            long style, const wxString& name ) {
  m_current = NULL;
  m_lastId = 0;
  m_isCreated = FALSE;
  m_imageList = NULL;
  m_smallImageList = NULL;
  Create( parent, id, pos, size, style, name );
};

wxTreeCtrl::~wxTreeCtrl() {
  DeleteAllItems();
};

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos, const wxSize& size,
            long style, const wxString& name ) {
  m_needParent = TRUE;
  
  PreCreation( parent, id, pos, size, style, name );
    
  m_widget = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m_widget),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	    
  m_tree = GTK_TREE(gtk_tree_new());
  
/*
  gtk_signal_connect( GTK_OBJECT(m_tree), "selection_changed",
      GTK_SIGNAL_FUNC(gtk_treectrl_selection_changed_callback), (gpointer)this);
  gtk_signal_connect( GTK_OBJECT(m_tree), "select_child",
      GTK_SIGNAL_FUNC(gtk_treectrl_select_child_callback), (gpointer)this);
  gtk_signal_connect( GTK_OBJECT(m_tree), "unselect_child",
      GTK_SIGNAL_FUNC(gtk_treectrl_unselect_child_callback), (gpointer)this);
*/

  gtk_container_add(GTK_CONTAINER(m_widget), GTK_WIDGET(m_tree));
  gtk_widget_show(GTK_WIDGET(m_tree));

  PostCreation();

  gtk_widget_realize(GTK_WIDGET(m_tree));
  
  Show(TRUE);

  return TRUE;
};

int wxTreeCtrl::GetCount() const
{
  if (!m_anchor) return 0;
  return m_anchor->NumberOfVisibleDescendents();
};

long wxTreeCtrl::InsertItem(long parent, const wxString& label, long data,
      int image, int selImage, long insertAfter) {
  wxTreeItem item;
  item.m_data = data;
  if (!label.IsNull() || (label == "")) {
    item.m_text = label;
    item.m_mask |= wxTREE_MASK_TEXT;
  };

  if (image >= 0) {
    item.m_image = image;
    item.m_mask |= wxTREE_MASK_IMAGE;
  };

  if (selImage >= 0) {
    item.m_selectedImage = selImage;
    item.m_mask |= wxTREE_MASK_SELECTED_IMAGE;
  };

  return InsertItem(parent, item, insertAfter);
};

long wxTreeCtrl::InsertItem( long parent, wxTreeItem &info, long WXUNUSED(insertAfter) ) {
  int oldMask = info.m_mask;
  long ret = 0;
  wxTreeItem *p = NULL;
  wxTreeItem *new_child;

  if (parent == 0) {
    if (m_anchor)
      return -1;
  } else {
    p = FindItem(parent);
    if (!p) return -1;
  };

  if ((info.m_mask & wxTREE_MASK_HANDLE) == 0) {
    m_lastId++;
    info.m_itemId = m_lastId;
    info.m_mask |= wxTREE_MASK_HANDLE;
    ret = m_lastId;
  } else
    ret = info.m_itemId;

  if (p) {
    if (p->m_tree == NULL)
      p->AddSubtree();
  
    new_child = new wxTreeItem(GTK_WIDGET(p->m_tree), info);
    p->AddChild(new_child);
  } else {
    new_child = new wxTreeItem(GTK_WIDGET(m_tree), info);
    m_anchor = new_child;
  }

/* Disabled until wxImageList q solved
  if ((info.m_mask & wxTREE_MASK_IMAGE) == 0) {
    wxBitmap *bmp;

    if ((bmp = m_imageList->GetItem(info.m_image))->Ok()) {
      GdkBitmap *mask = NULL;
      if (bmp->GetMask()) mask = bmp->GetMask()->GetBitmap();
      GtkWidget *pixmap = gtk_pixmap_new( bmp->GetPixmap(), mask );

      gtk_widget_set_parent(pixmap, GTK_WIDGET(new_child->m_widget));
      gtk_widget_show(pixmap);
      GTK_TREE_ITEM(new_child->m_widget)->pixmaps_box = pixmap;
    }
  }
*/

  new_child->m_owner = this;

  info.m_mask = oldMask;
  return ret;
};


bool wxTreeCtrl::ExpandItem( long item, int action ) {
  wxTreeItem *i = FindItem( item );
  if (!i)
    return FALSE;

  switch (action) {
    case wxTREE_EXPAND_EXPAND:
      gtk_tree_item_expand(GTK_TREE_ITEM(i->m_widget));
      break;

    case wxTREE_EXPAND_COLLAPSE_RESET:
    case wxTREE_EXPAND_COLLAPSE:
      gtk_tree_item_collapse(GTK_TREE_ITEM(i->m_widget));
      break;

    case wxTREE_EXPAND_TOGGLE:
      if ((i->GetState() & wxTREE_STATE_EXPANDED) == 0)
        gtk_tree_item_expand(GTK_TREE_ITEM(i->m_widget));
      else
        gtk_tree_item_collapse(GTK_TREE_ITEM(i->m_widget));
      break;
  }

  return TRUE;
};

void wxTreeCtrl::DeleteItem( long item )
{
  wxTreeItem *pItem = FindItem( item );
  wxCHECK_RET( pItem != NULL, "wxTreeCtrl::DeleteItem: no such pItem." );

//  pItem->m_parent->m_childlist.DeleteObject(pItem);
}

void wxTreeCtrl::DeleteChildren( long item )
{
  wxTreeItem *pItem = FindItem( item );
  wxCHECK_RET( pItem != NULL, "wxTreeCtrl::DeleteChildren: no such pItem." );

  pItem->DeleteChildren();
}

bool wxTreeCtrl::DeleteAllItems()
{
  delete m_anchor;
  m_anchor = NULL;
  return TRUE;
};

/*
bool wxTreeCtrl::GetItem( wxTreeItem &info ) const
{
  wxTreeItem *i = FindItem( info.m_itemId );
  if (!i) return FALSE;
  i->GetItem( info );
  return TRUE;
};
*/

long wxTreeCtrl::GetItemData( long item ) const
{
  wxTreeItem *i = FindItem( item );
  if (!i) return 0;
  return i->m_data;
};

wxString wxTreeCtrl::GetItemText( long item ) const
{
  wxTreeItem *i = FindItem( item );
  if (!i) return "";
  return i->m_text;
};

int wxTreeCtrl::GetItemImage(long item) const
{
  wxTreeItem *i = FindItem( item );
  return i == 0 ? -1 : i->GetImage();
}

long wxTreeCtrl::GetParent( long item ) const
{
  wxTreeItem *i = FindItem( item );
  if (!i) return -1;
/*
  i = i->m_parent;
  if (!i) return -1;
  return i->m_parent->m_itemId;
*/
  return -1;
};

long wxTreeCtrl::GetRootItem() const
{
  if (m_anchor) return m_anchor->m_itemId;
  return -1;
};

/*
long wxTreeCtrl::GetSelection() const
{
  return m_current ? m_current->GetItemId() : -1;
};

bool wxTreeCtrl::SelectItem(long itemId)
{
  wxTreeItem *pItem = FindItem(itemId);
  if ( !pItem ) {
    wxLogDebug("Can't select an item %d which doesn't exist.", itemId);

    return FALSE;
  }

  SelectItem(pItem);

  return TRUE;
};

void wxTreeCtrl::SelectItem(wxTreeItem *item, bool bDoEvents )
{
  if (m_current != item)
  {
    if (m_current)
    {
      m_current->SetHilight( FALSE );
//      RefreshLine( m_current );
    };
    m_current = item;
    m_current->SetHilight( TRUE );
//    RefreshLine( m_current );

    if (bDoEvents) m_current->SendSelected( this );
  }
}
*/

bool wxTreeCtrl::ItemHasChildren( long item ) const
{
  wxTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  return i->HasChildren();
};

void wxTreeCtrl::SetIndent( int indent )
{
  m_indent = indent;
  Refresh();
};

int wxTreeCtrl::GetIndent() const
{
  return m_indent;
};

/*
bool wxTreeCtrl::SetItem( wxTreeItem &info )
{
  wxTreeItem *i = FindItem( info.m_itemId );
  if (!i) return FALSE;
  wxClientDC dc(this);
  i->SetItem( info, &dc );
  Refresh();
  return TRUE;
};

bool wxTreeCtrl::SetItemData( long item, long data )
{
  wxTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  i->m_data = data;
  return TRUE;
};

bool wxTreeCtrl::SetItemText( long item, const wxString &text )
{
  wxTreeItem *i = FindItem( item );
  if (!i) return FALSE;
  wxClientDC dc(this);
  i->SetText( text, &dc );
  return TRUE;
};

void wxTreeCtrl::SetItemImage(long item, int image, int imageSel) const
{
  wxTreeItem *i = FindItem( item );
  if ( i != 0 ) {
    i->SetImage(image);
    i->SetSelectedImage(imageSel);
  }
}

long wxTreeCtrl::HitTest( const wxPoint& point, int &flags )
{
  flags = 0;
  if (!m_anchor) return -1;
  return m_anchor->HitTest( point, flags );
};
*/

wxImageList *wxTreeCtrl::GetImageList( int which ) const
{
  if (which == wxIMAGE_LIST_NORMAL) return m_imageList;
  return m_smallImageList;
};

void wxTreeCtrl::SetImageList( wxImageList *imageList, int which )
{
  if (which == wxIMAGE_LIST_NORMAL)
  {
    if (m_imageList) delete m_imageList;
    m_imageList = imageList;
  }
  else
  {
    if (m_smallImageList) delete m_smallImageList;
    m_smallImageList = imageList;
  };
};

wxTreeItem *wxTreeCtrl::FindItem( long itemId ) const {
  if (!m_anchor) return NULL;
  return m_anchor->FindItem( itemId );
  return 0;
};

wxTreeItem *wxTreeCtrl::FindItem(GtkTreeItem *item) const {
  if (!m_anchor) return NULL;
  return m_anchor->FindItem(item);
  return 0;
};

//-----------------------------------------------------------------------------
// wxTreeEvent
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent,wxCommandEvent)

wxTreeEvent::wxTreeEvent( wxEventType commandType, int id ) :
  wxCommandEvent( commandType, id )
{
  m_code = 0;
  m_oldItem = 0;
};

/* Bunch of old code might be useful */
/*
  wxBitmap *bmp;

  if ((bmp = m_imageList->GetItem(0))->Ok()) {
    GdkBitmap *mask = NULL;
    if (bmp->GetMask()) mask = bmp->GetMask()->GetBitmap();
    GtkWidget *pixmap = gtk_pixmap_new( bmp->GetPixmap(), mask );

    gtk_widget_set_parent(pixmap, GTK_WIDGET(new_child->m_widget));
    gtk_widget_show(pixmap);
    GTK_TREE_ITEM(new_child->m_widget)->pixmaps_box = pixmap;
  }

  if ((bmp = m_imageList->GetItem(1))->Ok()) {
    GdkBitmap *mask = NULL;
    if (bmp->GetMask()) mask = bmp->GetMask()->GetBitmap();
    GtkWidget *pixmap = gtk_pixmap_new( bmp->GetPixmap(), mask );

    gtk_widget_set_parent(pixmap, GTK_WIDGET(new_child->m_widget));
    gtk_widget_show(pixmap);
    GTK_TREE_ITEM(new_child->m_widget)->plus_pix_widget = pixmap;
  }

  if ((bmp = m_imageList->GetItem(2))->Ok()) {
    GdkBitmap *mask = NULL;
    if (bmp->GetMask()) mask = bmp->GetMask()->GetBitmap();
    GtkWidget *pixmap = gtk_pixmap_new( bmp->GetPixmap(), mask );

    gtk_widget_set_parent(pixmap, GTK_WIDGET(new_child->m_widget));
    gtk_widget_show(pixmap);
    GTK_TREE_ITEM(new_child->m_widget)->minus_pix_widget = pixmap;
  }

  if (p)
    if (p->m_childlist.Number() == 1) {
      gtk_tree_item_collapse(GTK_TREE_ITEM(p->m_widget));
      gtk_tree_item_expand(GTK_TREE_ITEM(p->m_widget));
    }
*/
