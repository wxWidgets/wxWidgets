/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:     wxTreeCtrl
// Author:      Denis Pershin
// Modified by:
// Created:     07/05/98
// RCS-ID:      $Id$
// Copyright:   (c) Denis Pershin
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "treectrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/gtk/treectrl.h"
#include <wx/textctrl.h>
#include "wx/log.h"

#include <gtk/gtk.h>

//static void wxConvertToGtkTreeItem(wxTreeCtrl *owner, wxTreeItem& info, GtkTreeItem **gtkItem);
//static void wxConvertFromGtkTreeItem(wxTreeItem& info, GtkTreeItem *gtkItem);
static void gtk_treectrl_count_callback (GtkWidget *widget, gpointer data);
static void gtk_treectrl_first_selected_callback(GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_callback (GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_visible_callback (GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_selected_callback (GtkWidget *widget, gpointer data);

static void gtk_treeitem_expand_callback(GtkWidget *widget, wxTreeItemId *treeitem);
static void gtk_treeitem_collapse_callback(GtkWidget *widget, wxTreeItemId *treeitem);
static void gtk_treeitem_select_callback(GtkWidget *widget, wxTreeItemId *treeitem);

static void gtk_treeitem_expand_callback(GtkWidget *widget, wxTreeItemId *treeitem) {
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

//  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendExpanding(GTK_TREE_ITEM(widget));
  owner->SendExpanded(GTK_TREE_ITEM(widget));
};

static void gtk_treeitem_collapse_callback(GtkWidget *widget, wxTreeItemId *treeitem) {
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

//  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendCollapsing(GTK_TREE_ITEM(widget));
  owner->SendCollapsed(GTK_TREE_ITEM(widget));
};

static void gtk_treeitem_select_callback(GtkWidget *widget, wxTreeItemId *treeitem) {
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

//  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendSelChanging(GTK_TREE_ITEM(widget));
  owner->SendSelChanged(GTK_TREE_ITEM(widget));
}

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxControl)

#endif

void wxTreeCtrl::Init() {
  m_imageListNormal = NULL;
  m_imageListState = NULL;
  m_textCtrl = NULL;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                        const wxSize& size, long style,
                        const wxValidator& validator, const wxString& name) {
  Init();

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  SetParent(parent);

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  m_needParent = TRUE;
  
printf("precreate\n");
  PreCreation( parent, id, pos, size, style, name );

printf("1\n");

  m_widget = gtk_scrolled_window_new(NULL, NULL);
printf("2\n");
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m_widget),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

printf("3\n");
  m_tree = GTK_TREE(gtk_tree_new());

printf("4\n");
  gtk_container_add(GTK_CONTAINER(m_widget), GTK_WIDGET(m_tree));
printf("5\n");
//  gtk_widget_set_parent(GTK_WIDGET(m_tree), m_widget);
printf("6\n");
  gtk_widget_show(GTK_WIDGET(m_tree));

  SetName(name);
  SetValidator(validator);

printf("postcreate\n");
  PostCreation();

  gtk_widget_realize(GTK_WIDGET(m_tree));

  Show(TRUE);
  
  return TRUE;
}

wxTreeCtrl::~wxTreeCtrl(void) {
  if (m_textCtrl)
    delete m_textCtrl;
}

// Attributes
static void gtk_treectrl_count_callback (GtkWidget *widget, gpointer data) {
  int count = (*((int *)data));

  count++;
  if (GTK_IS_CONTAINER(widget))
    gtk_container_foreach(GTK_CONTAINER(widget), gtk_treectrl_count_callback, data);
}

size_t wxTreeCtrl::GetCount() const {
  int count = 0;

  if (m_anchor != NULL)
    gtk_treectrl_count_callback(GTK_WIDGET(m_anchor), &count);
  return count;
}

unsigned int wxTreeCtrl::GetIndent() const {
  return m_tree->indent_value;
}

void wxTreeCtrl::SetIndent(unsigned int indent) {
  m_tree->indent_value = indent;
}

wxImageList *wxTreeCtrl::GetImageList() const {
  return m_imageListNormal;
}

wxImageList *wxTreeCtrl::GetStateImageList() const {
  return m_imageListState;
}

void wxTreeCtrl::SetImageList(wxImageList *imageList) {
  m_imageListNormal = imageList;
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList) {
  m_imageListState = imageList;
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId &item) const {
  char *t;

  if (!item.IsOk())
    return wxString("");

  GtkLabel *l = GTK_LABEL(gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "w_label"));
  gtk_label_get(l, &t);

  return t;
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item) const {
  if (!item.IsOk())
    return (-1);

  return (int)gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "image");
}

int wxTreeCtrl::GetItemSelectedImage(const wxTreeItemId& item) const {
  if (!item.IsOk())
    return (-1);

  return (int)gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "selectedImage");
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const {
  if (!item.IsOk())
    return NULL;

  return (wxTreeItemData *)gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "data");
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text) {
  if (!item.IsOk())
    return;

  GtkLabel *l = GTK_LABEL(gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "w_label"));
  gtk_label_set(l, text);
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item, int image) {
  if (!item.IsOk())
    return;

  gtk_object_set_data(GTK_OBJECT((GtkTreeItem *)item), "image", (void *)image);
}

void wxTreeCtrl::SetItemSelectedImage(const wxTreeItemId& item, int image) {
  if (!item.IsOk())
    return;

  gtk_object_set_data(GTK_OBJECT((GtkTreeItem *)item), "selectedImage", (void *)image);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data) {
  if (!item.IsOk())
    return;

  gtk_object_set_data(GTK_OBJECT((GtkTreeItem *)item), "data", data);
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const {
#warning "Need to implement IsVisible"
  return FALSE;
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const {
  GtkTreeItem *p = (GtkTreeItem *)item;

  if (p->subtree == NULL)
    return wxFalse;

  if (GTK_TREE(p->subtree)->children == NULL)
    return wxFalse;

  if (g_list_length(GTK_TREE(p->subtree)->children) == 0)
    return wxFalse;

  return wxTrue;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const {
  return (((GtkTreeItem *)item)->expanded != 0);
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const {
  GtkTreeItem *p = (GtkTreeItem *)item;
  GtkWidget *parent = GTK_WIDGET(p)->parent;

  if (!GTK_IS_TREE(parent))
    return wxFalse;

  if (g_list_index(GTK_TREE(parent)->children, p) == -1)
    return wxFalse;

  return wxTrue;
}

wxTreeItemId wxTreeCtrl::GetRootItem() const {
  return m_anchor;
}

static void gtk_treectrl_first_selected_callback(GtkWidget *widget, gpointer data) {
  GtkTreeItem *p = (*((GtkTreeItem **)data));

  GtkTree *tree = GTK_TREE(GTK_TREE_ITEM(widget)->subtree);

  if (tree->selection != NULL) {
    p = (GtkTreeItem *)tree->selection->data;
    return;
  }

  if (GTK_IS_CONTAINER(widget))
    gtk_container_foreach(GTK_CONTAINER(widget), gtk_treectrl_first_selected_callback, data);
}

wxTreeItemId wxTreeCtrl::GetSelection() const {
  GtkTreeItem *p = NULL;

  if (m_anchor == NULL)
    return NULL;

  gtk_treectrl_first_selected_callback(GTK_WIDGET(m_anchor), &p);

  return p;
}

wxTreeItemId wxTreeCtrl::GetParent(const wxTreeItemId& item) const {
  if (item.IsOk())
    return (GtkTreeItem *)gtk_object_get_data(GTK_OBJECT((GtkTreeItem *)item), "parent");

  return NULL;
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item, long& cookie) const {
  GtkTreeItem *p = (GtkTreeItem *)item;
  GtkWidget *parent = GTK_WIDGET(p)->parent;

  if (!GTK_IS_TREE(parent))
    return NULL;

  cookie = 0;
  return GTK_TREE_ITEM(g_list_first(GTK_TREE(parent)->children)->data);
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& item, long& cookie) const {
  GtkTreeItem *p = (GtkTreeItem *)item;
  GtkWidget *parent = GTK_WIDGET(p)->parent;

  if (!GTK_IS_TREE(parent))
    return NULL;

  cookie++;
  return GTK_TREE_ITEM(g_list_nth(GTK_TREE(parent)->children, cookie)->data);
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const {
  GtkTreeItem *p = (GtkTreeItem *)item;
  GtkWidget *parent = GTK_WIDGET(p)->parent;

  if (!GTK_IS_TREE(parent))
    return NULL;

  if (g_list_index(GTK_TREE(parent)->children, p) == -1)
    return NULL;

  return GTK_TREE_ITEM(g_list_next(g_list_find(GTK_TREE(parent)->children, p))->data);
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const {
  GtkTreeItem *p = (GtkTreeItem *)item;
  GtkWidget *parent = GTK_WIDGET(p)->parent;

  if (!GTK_IS_TREE(parent))
    return NULL;

  if (g_list_index(GTK_TREE(parent)->children, p) == -1)
    return NULL;

  return GTK_TREE_ITEM(g_list_previous(g_list_find(GTK_TREE(parent)->children, p))->data);
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const {
#warning "Need to implement GetFirstVisibleItem"
  return NULL;
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const {
#warning "Need to implement GetNextVisible"
  return NULL;
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const {
#warning "Need to implement GetPrevVisible"
  return NULL;
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text, int image,
                     int selectedImage, wxTreeItemData *data) {
  return p_InsertItem(0, text, image, selectedImage, data);
}

wxTreeItemId wxTreeCtrl::PrependItem(const wxTreeItemId& parent,
                         const wxString& text, int image, int selectedImage,
                         wxTreeItemData *data) {
#warning "Need to implement PrependItem"
  return NULL;
}

wxTreeItemId wxTreeCtrl::InsertItem(const wxTreeItemId& parent,
                        const wxTreeItemId& idPrevious, const wxString& text,
                        int image, int selectedImage, wxTreeItemData *data) {
#warning "Need to implement InsertItem"
  return NULL;
}

wxTreeItemId wxTreeCtrl::AppendItem(const wxTreeItemId& parent,
                        const wxString& text, int image, int selectedImage,
											  wxTreeItemData *data) {
  return p_InsertItem(parent, text, image, selectedImage, data);
}

wxTreeItemId wxTreeCtrl::p_InsertItem(GtkTreeItem *p,
                        const wxString& text, int image, int selectedImage,
											  wxTreeItemData *data) {
  GtkTreeItem *item;

printf("begin insert\n");

  item = GTK_TREE_ITEM(gtk_tree_item_new());

  GtkHBox *m_box = GTK_HBOX(gtk_hbox_new(FALSE, 0));
  gtk_container_add (GTK_CONTAINER (item), GTK_WIDGET(m_box));

  gtk_object_set_data(GTK_OBJECT(item), "w_box", m_box);

  const wxBitmap *bmp;
  const wxImageList *list;
  if ((list = GetImageList(wxIMAGE_LIST_NORMAL)) != NULL)
    if ((bmp = list->GetBitmap(image)) != NULL)
      if (bmp->Ok()) {
        GdkBitmap *mask = NULL;
        if (bmp->GetMask())
          mask = bmp->GetMask()->GetBitmap();
        GtkPixmap *m_image_widget = GTK_PIXMAP(gtk_pixmap_new(bmp->GetPixmap(), mask));
        gtk_misc_set_alignment (GTK_MISC (m_image_widget), 0.0, 0.5);
        gtk_box_pack_start(GTK_BOX(m_box), GTK_WIDGET(m_image_widget), FALSE, FALSE, 0);
        gtk_object_set_data(GTK_OBJECT(item), "w_image", (void *)m_image_widget);
        gtk_object_set_data(GTK_OBJECT(item), "image", (void *)image);
        gtk_widget_show (GTK_WIDGET(m_image_widget));
      }
  GtkLabel *m_label_widget = GTK_LABEL(gtk_label_new ((char *)(const char *)text));
  gtk_misc_set_alignment (GTK_MISC (m_label_widget), 0.5, 0.5);
  gtk_box_pack_start(GTK_BOX(m_box), GTK_WIDGET(m_label_widget), FALSE, FALSE, 0);
  gtk_object_set_data(GTK_OBJECT(item), "w_label", m_label_widget);
  gtk_widget_show (GTK_WIDGET(m_label_widget));

  gtk_widget_show(GTK_WIDGET(m_box));

  gtk_object_set_data(GTK_OBJECT(item), "owner", this);
  gtk_object_set_data(GTK_OBJECT(item), "data", data);
  gtk_object_set_data(GTK_OBJECT(item), "parent", p);

  if (p != 0) {
    if (p->subtree == NULL) {
      GtkTree *tree = GTK_TREE(gtk_tree_new());
      gtk_tree_item_set_subtree(GTK_TREE_ITEM(p), GTK_WIDGET(tree));
      gtk_widget_show(GTK_WIDGET(tree));
      p->expanded = 1;
    }

    gtk_container_add(GTK_CONTAINER(p->subtree), GTK_WIDGET(item));
  } else {
printf("Adding root\n");
printf("m_tree = %p\n", m_tree);
    m_anchor = item;
    gtk_container_add(GTK_CONTAINER(m_tree), GTK_WIDGET(item));
  }

  gtk_widget_show(GTK_WIDGET(item));

  gtk_signal_connect(GTK_OBJECT(item), "select",
    GTK_SIGNAL_FUNC(gtk_treeitem_select_callback), (gpointer)this );

  gtk_signal_connect(GTK_OBJECT(item), "deselect",
    GTK_SIGNAL_FUNC(gtk_treeitem_select_callback), (gpointer)this );

  gtk_signal_connect(GTK_OBJECT(item), "expand",
    GTK_SIGNAL_FUNC(gtk_treeitem_expand_callback), (gpointer)this );
  gtk_signal_connect(GTK_OBJECT(item), "collapse",
    GTK_SIGNAL_FUNC(gtk_treeitem_collapse_callback), (gpointer)this );

  return item;
}

void wxTreeCtrl::Delete(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  GtkTreeItem *parent = GTK_TREE_ITEM(GTK_WIDGET((GtkTreeItem *)item)->parent);
  if (parent == NULL)
    return;

  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET((GtkTreeItem *)item));

  return;
}

void wxTreeCtrl::DeleteAllItems() {
  gtk_tree_item_remove_subtree(m_anchor);
}

void wxTreeCtrl::Expand(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  gtk_tree_item_expand(GTK_TREE_ITEM((GtkTreeItem *)item));
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  gtk_tree_item_collapse(GTK_TREE_ITEM((GtkTreeItem *)item));
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  gtk_tree_item_collapse(GTK_TREE_ITEM((GtkTreeItem *)item));
  gtk_tree_item_remove_subtree(GTK_TREE_ITEM((GtkTreeItem *)item));
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  if (((GtkTreeItem *)item)->expanded)
    gtk_tree_item_collapse(GTK_TREE_ITEM((GtkTreeItem *)item));
  else
    gtk_tree_item_expand(GTK_TREE_ITEM((GtkTreeItem *)item));
}

void wxTreeCtrl::Unselect() {
#warning "Need to implement Unselect"
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item) {
  if (!item.IsOk())
    return;

  gtk_tree_item_select((GtkTreeItem *)item);
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item) {
#warning "Need to implement EnsureVisible"
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item) {
#warning "Need to implement ScrollTo"
}

wxTextCtrl* wxTreeCtrl::EditLabel(const wxTreeItemId& item,
                          wxClassInfo* textControlClass) {
    wxASSERT( textControlClass->IsKindOf(CLASSINFO(wxTextCtrl)) );
#warning "Need to implement EditLabel"
    return m_textCtrl;
}

wxTextCtrl* wxTreeCtrl::GetEditControl() const {
  return m_textCtrl;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& item, bool discardChanges) {
#warning "Need to implement EndEditLabel"
}

void wxTreeCtrl::ExpandItem(const wxTreeItemId& item, int action) {
  switch (action) {
    case wxTREE_EXPAND_EXPAND:
      Expand(item);
      break;

    case wxTREE_EXPAND_COLLAPSE:
      Collapse(item);
      break;

    case wxTREE_EXPAND_COLLAPSE_RESET:
      CollapseAndReset(item);
      break;

    case wxTREE_EXPAND_TOGGLE:
      Toggle(item);
      break;

    default:
      wxFAIL_MSG("unknown action in wxTreeCtrl::ExpandItem");
  }
}

wxTreeItemId wxTreeCtrl::InsertItem(const wxTreeItemId& parent,
                            const wxString& text, int image, int selImage,
                            long insertAfter) {
//  InsertItem(parent, insertAfter, text, image, selImage);
  #warning "Need to implement InsertItem"
  return NULL;
}

/* Old functions
long wxTreeCtrl::GetChild(long item) const {
  GtkTreeItem *p;
  GtkTreeItem *next = NULL;

  p = findGtkTreeItem(item);
  GList *list = gtk_container_children(GTK_CONTAINER(p));
  next = GTK_TREE_ITEM(list->data);;

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

long wxTreeCtrl::GetFirstVisibleItem(void) const {
  GtkTreeItem *next = NULL;

  GList *list = gtk_container_children(GTK_CONTAINER(m_anchor));
  next = GTK_TREE_ITEM(list->data);;
//  gtk_container_foreach(GTK_CONTAINER(m_anchor), gtk_treectrl_next_visible_callback, &next);

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

long wxTreeCtrl::GetNextVisibleItem(long item) const {
  GtkTreeItem *p;
  GtkTreeItem *next = NULL;

  p = findGtkTreeItem(item);
  GList *list = gtk_container_children(GTK_CONTAINER(p));
  next = GTK_TREE_ITEM(list->data);;
//  gtk_container_foreach(GTK_CONTAINER(p), gtk_treectrl_next_visible_callback, &next);

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

bool wxTreeCtrl::GetItem(wxTreeItem& info) const {
  GtkTreeItem *p;

  p = findGtkTreeItem(info.m_itemId);

  if (p == NULL) {
    wxLogSysError("TreeCtrl::GetItem failed");
    return FALSE;
  }

  wxConvertFromGtkTreeItem(info, p);

  return TRUE;
}

bool wxTreeCtrl::SetItem(wxTreeItem& info) {
  GtkTreeItem *p;

  p = findGtkTreeItem(info.m_itemId);

  if (p == NULL) {
    wxLogSysError("TreeCtrl::SetItem failed");
    return FALSE;
  }

  wxConvertToGtkTreeItem(this, info, &p);

  return TRUE;
}

int wxTreeCtrl::GetItemState(long item, long stateMask) const {
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_STATE ;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  if (!GetItem(info))
    return 0;

  return info.m_state;
}

bool wxTreeCtrl::SetItemState(long item, long state, long stateMask) {
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_STATE ;
  info.m_state = state;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  return SetItem(info);
}
*/

// Operations
/*
bool wxTreeCtrl::DeleteChildren(long item) {
  GtkTreeItem *p;

  p = findGtkTreeItem(item);
  if (p == NULL)
    return FALSE;

  gtk_tree_item_remove_subtree(GTK_TREE_ITEM(p));

  return TRUE;
}
*/

/*
long wxTreeCtrl::InsertItem(long parent, wxTreeItem& info, long insertAfter) {
  GtkTreeItem *p;
  GtkTreeItem *item = NULL;

  info.m_itemId = m_curitemId;
  m_curitemId++;

  wxConvertToGtkTreeItem(this, info, &item);

  if (parent != 0) {
    p = findGtkTreeItem(parent);
    if (p->subtree == NULL) {
      GtkTree *tree = GTK_TREE(gtk_tree_new());
      gtk_tree_item_set_subtree(GTK_TREE_ITEM(p), GTK_WIDGET(tree));
      gtk_widget_show(GTK_WIDGET(tree));
      p->expanded = 1;
    }

    gtk_container_add(GTK_CONTAINER(p->subtree), GTK_WIDGET(item));
  } else {
    m_anchor = item;
    gtk_container_add(GTK_CONTAINER(m_tree), GTK_WIDGET(item));
  }

  if ((info.m_mask & wxTREE_MASK_CHILDREN) != 0) {
    GtkTree *tree = GTK_TREE(gtk_tree_new());
    gtk_tree_item_set_subtree(GTK_TREE_ITEM(item), GTK_WIDGET(tree));
    gtk_widget_show(GTK_WIDGET(tree));
  }

  gtk_widget_show(GTK_WIDGET(item));

  gtk_signal_connect(GTK_OBJECT(item), "select",
    GTK_SIGNAL_FUNC(gtk_treeitem_select_callback), (gpointer)this );

  gtk_signal_connect(GTK_OBJECT(item), "deselect",
    GTK_SIGNAL_FUNC(gtk_treeitem_select_callback), (gpointer)this );

  gtk_signal_connect(GTK_OBJECT(item), "expand",
    GTK_SIGNAL_FUNC(gtk_treeitem_expand_callback), (gpointer)this );
  gtk_signal_connect(GTK_OBJECT(item), "collapse",
    GTK_SIGNAL_FUNC(gtk_treeitem_collapse_callback), (gpointer)this );

  return info.m_itemId;
}

long wxTreeCtrl::InsertItem(long parent, const wxString& label, int image,
  int selImage, long insertAfter) {

  wxTreeItem info;
  info.m_text = label;
  info.m_mask = wxTREE_MASK_TEXT;
  if (image > -1) {
    info.m_mask |= wxTREE_MASK_IMAGE | wxTREE_MASK_SELECTED_IMAGE;
    info.m_image = image;
    if (selImage == -1)
      info.m_selectedImage = image;
    else
      info.m_selectedImage = selImage;
  }

  return InsertItem(parent, info, insertAfter);
}
*/

void wxTreeCtrl::SendMessage(wxEventType command, const wxTreeItemId& item) {
  wxTreeEvent event(command, GetId());
  event.SetEventObject(this);
  event.m_item = item;
  ProcessEvent(event);
}

void wxTreeCtrl::SendExpanding(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_ITEM_EXPANDING, item);
}

void wxTreeCtrl::SendExpanded(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_ITEM_EXPANDED, item);
}

void wxTreeCtrl::SendCollapsing(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, item);
}

void wxTreeCtrl::SendCollapsed(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, item);
}

void wxTreeCtrl::SendSelChanging(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_SEL_CHANGED, item);
}

void wxTreeCtrl::SendSelChanged(const wxTreeItemId& item) {
  SendMessage(wxEVT_COMMAND_TREE_SEL_CHANGING, item);
}

// Tree event
IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxCommandEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id) {
  m_code = 0;
  m_itemOld = 0;
}
