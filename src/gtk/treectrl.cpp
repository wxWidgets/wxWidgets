/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:     wxTreeCtrl
// Author:      Denis Pershin
// Modified by:
// Created:     07/05/98
// RCS-ID:      $Id$
// Copyright:   (c) Denis Pershin
// Licence:     wxWindows licence
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
#include "wx/log.h"

#include <gtk/gtk.h>

static void wxConvertToGtkTreeItem(wxTreeCtrl *owner, wxTreeItem& info, GtkTreeItem **gtkItem);
static void wxConvertFromGtkTreeItem(wxTreeItem& info, GtkTreeItem *gtkItem);
static void gtk_treectrl_count_callback (GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_callback (GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_visible_callback (GtkWidget *widget, gpointer data);
// static void gtk_treectrl_next_selected_callback (GtkWidget *widget, gpointer data);

static void gtk_treeitem_expand_callback(GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem);
static void gtk_treeitem_collapse_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem);
static void gtk_treeitem_select_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem);

static void gtk_treeitem_expand_callback(GtkWidget *widget, wxTreeItem *treeitem)
{
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendExpanding(id);
  owner->SendExpanded(id);
};

static void gtk_treeitem_collapse_callback(GtkWidget *widget, wxTreeItem *treeitem)
{
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendCollapsing(id);
  owner->SendCollapsed(id);
};

static void gtk_treeitem_select_callback(GtkWidget *widget, wxTreeItem *treeitem)
{
  wxTreeCtrl *owner = (wxTreeCtrl *)gtk_object_get_data(GTK_OBJECT(widget), "owner");
  if (owner == NULL)
    return;

  long id = (long)gtk_object_get_data(GTK_OBJECT(widget), "id");
  owner->SendSelChanging(id);
  owner->SendSelChanged(id);
}

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxControl)
  IMPLEMENT_DYNAMIC_CLASS(wxTreeItem, wxObject)
#endif

wxTreeCtrl::wxTreeCtrl()
{
  m_imageListNormal = NULL;
  m_imageListState = NULL;
  m_textCtrl = NULL;
  m_curitemId = 1;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name)
{
  m_imageListNormal = NULL;
  m_imageListState = NULL;
  m_textCtrl = NULL;
  m_curitemId = 1;

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
  
  PreCreation( parent, id, pos, size, style, name );

  m_widget = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m_widget),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  m_tree = GTK_TREE(gtk_tree_new());

  gtk_container_add(GTK_CONTAINER(m_widget), GTK_WIDGET(m_tree));
  gtk_widget_show(GTK_WIDGET(m_tree));

  wxSystemSettings settings;
  SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_WINDOW));
  SetForegroundColour(parent->GetForegroundColour());

  SetName(name);
  SetValidator(validator);

  PostCreation();

  gtk_widget_realize(GTK_WIDGET(m_tree));

  Show(TRUE);
  
  return TRUE;
}

wxTreeCtrl::~wxTreeCtrl()
{
  wxDELETE(m_textCtrl);
}

// Attributes
static void gtk_treectrl_count_callback (GtkWidget *widget, gpointer data)
{
  int count = (*((int *)data));

  count++;
  if (GTK_IS_CONTAINER(widget))
    gtk_container_foreach(GTK_CONTAINER(widget), gtk_treectrl_count_callback, data);
}

int wxTreeCtrl::GetCount() const
{
  int count = 0;

  if (m_anchor != NULL)
    gtk_container_foreach(GTK_CONTAINER(m_anchor), gtk_treectrl_count_callback, &count);
  return count;
}

int wxTreeCtrl::GetIndent() const
{
  return m_tree->indent_value;
}

void wxTreeCtrl::SetIndent(int indent)
{
  m_tree->indent_value = indent;
}

wxImageList *wxTreeCtrl::GetImageList(int which) const
{
  if (which == wxIMAGE_LIST_NORMAL) {
    return m_imageListNormal;
  }
  else
  if (which == wxIMAGE_LIST_STATE) {
    return m_imageListState;
  }
  return NULL;
}

void wxTreeCtrl::SetImageList(wxImageList *imageList, int which)
{
  if (which == wxIMAGE_LIST_NORMAL)
    m_imageListNormal = imageList;
  else
    if (which == wxIMAGE_LIST_STATE)
      m_imageListState = imageList;
}

long wxTreeCtrl::GetNextItem(long item, int code) const
{
  switch (code) {
    case wxTREE_NEXT_CARET:
//      flag = TVGN_CARET;
      break;
    case wxTREE_NEXT_CHILD:
//      flag = TVGN_CHILD;
      break;
    case wxTREE_NEXT_DROPHILITE:
//      flag = TVGN_DROPHILITE;
      break;
    case wxTREE_NEXT_FIRSTVISIBLE:
//      flag = TVGN_FIRSTVISIBLE;
      break;
    case wxTREE_NEXT_NEXT:
//      flag = TVGN_NEXT;
      break;
    case wxTREE_NEXT_NEXTVISIBLE:
//      flag = TVGN_NEXTVISIBLE;
      break;
    case wxTREE_NEXT_PARENT:
//      flag = TVGN_PARENT;
      break;
    case wxTREE_NEXT_PREVIOUS:
//      flag = TVGN_PREVIOUS;
      break;
    case wxTREE_NEXT_PREVIOUSVISIBLE:
//      flag = TVGN_PREVIOUSVISIBLE;
      break;
    case wxTREE_NEXT_ROOT:
//      flag = TVGN_ROOT;
      break;

    default :
      break;
  }
//  return (long) TreeView_GetNextItem( (HWND) GetHWND(), (HTREEITEM) item, flag);
  return 0;
}

bool wxTreeCtrl::ItemHasChildren(long item) const
{
  GtkTreeItem *p;
  int count = 0;

  p = findGtkTreeItem(item);
  
  gtk_container_foreach(GTK_CONTAINER(p), gtk_treectrl_count_callback, &count);

  return (count != 0);
}

static GtkTreeItem *findItem(GtkTreeItem *p, long id)
{
  GtkTreeItem *q;

  if (((long)gtk_object_get_data(GTK_OBJECT(p), "id")) == id)
      return p;

  if (p->subtree == NULL)
    return NULL;

  GtkTree *tree = GTK_TREE(p->subtree);

  GList *list = gtk_container_children(GTK_CONTAINER(tree));
  guint len = g_list_length(list);

  for (guint i=0; i<len;i++) {
    GList *l = g_list_nth(list, i);
    if (!GTK_IS_TREE_ITEM(l->data))
      continue;
    q = GTK_TREE_ITEM(l->data);
    GtkTreeItem *ret = findItem(q, id);
    if (ret != NULL)
      return ret;
  }

  return NULL;
}

GtkTreeItem *wxTreeCtrl::findGtkTreeItem(long id) const
{
  return findItem(m_anchor, id);
}

long wxTreeCtrl::GetChild(long item) const
{
  GtkTreeItem *p;
  GtkTreeItem *next = NULL;

  p = findGtkTreeItem(item);
  GList *list = gtk_container_children(GTK_CONTAINER(p));
  next = GTK_TREE_ITEM(list->data);;

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

long wxTreeCtrl::GetParent(long item) const
{
  GtkTreeItem *p;

  p = findGtkTreeItem(item);
  if (p != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(p), "parent");

  return (-1);
}

long wxTreeCtrl::GetFirstVisibleItem() const
{
  GtkTreeItem *next = NULL;

  GList *list = gtk_container_children(GTK_CONTAINER(m_anchor));
  next = GTK_TREE_ITEM(list->data);;
//  gtk_container_foreach(GTK_CONTAINER(m_anchor), gtk_treectrl_next_visible_callback, &next);

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

long wxTreeCtrl::GetNextVisibleItem(long item) const
{
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

long wxTreeCtrl::GetSelection() const
{
  GtkTreeItem *next = NULL;

  GList *list = gtk_container_children(GTK_CONTAINER(m_anchor));
  next = GTK_TREE_ITEM(list->data);;
//  gtk_container_foreach(GTK_CONTAINER(m_anchor), gtk_treectrl_next_selected_callback, &next);

  if (next != NULL)
    return (long)gtk_object_get_data(GTK_OBJECT(next), "id");
  
  return (-1);
}

long wxTreeCtrl::GetRootItem() const
{
  return (long)gtk_object_get_data(GTK_OBJECT(m_anchor), "id");
}

bool wxTreeCtrl::GetItem(wxTreeItem& info) const
{
  GtkTreeItem *p;

  p = findGtkTreeItem(info.m_itemId);

  if (p == NULL) {
    wxLogError("TreeCtrl::GetItem failed.");
    return FALSE;
  }

  wxConvertFromGtkTreeItem(info, p);

  return TRUE;
}

bool wxTreeCtrl::SetItem(wxTreeItem& info)
{
  GtkTreeItem *p;

  p = findGtkTreeItem(info.m_itemId);

  if (p == NULL) {
    wxLogError("TreeCtrl::SetItem failed.");
    return FALSE;
  }

  wxConvertToGtkTreeItem(this, info, &p);

  return TRUE;
}

int wxTreeCtrl::GetItemState(long item, long stateMask) const
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_STATE ;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  if (!GetItem(info))
    return 0;

  return info.m_state;
}

bool wxTreeCtrl::SetItemState(long item, long state, long stateMask)
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_STATE ;
  info.m_state = state;
  info.m_stateMask = stateMask;
  info.m_itemId = item;

  return SetItem(info);
}

bool wxTreeCtrl::SetItemImage(long item, int image, int selImage)
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_IMAGE ;
  info.m_image = image;
  if (selImage > -1) {
    info.m_selectedImage = selImage;
    info.m_mask |= wxTREE_MASK_SELECTED_IMAGE;
  }
  info.m_itemId = item;

  return SetItem(info);
}

wxString wxTreeCtrl::GetItemText(long item) const
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_TEXT ;
  info.m_itemId = item;

  if (!GetItem(info))
    return wxString("");
  return info.m_text;
}

void wxTreeCtrl::SetItemText(long item, const wxString& str)
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_TEXT ;
  info.m_itemId = item;
  info.m_text = str;

  SetItem(info);
}

long wxTreeCtrl::GetItemData(long item) const
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_DATA ;
  info.m_itemId = item;

  if (!GetItem(info))
    return 0;
  return info.m_data;
}

bool wxTreeCtrl::SetItemData(long item, long data)
{
  wxTreeItem info;

  info.m_mask = wxTREE_MASK_DATA ;
  info.m_itemId = item;
  info.m_data = data;

  return SetItem(info);
}

bool wxTreeCtrl::GetItemRect(long item, wxRectangle& rect, bool textOnly) const
{
/*
  RECT rect2;

  *(HTREEITEM*)& rect2 = (HTREEITEM) item;
  bool success = (::SendMessage((HWND) GetHWND(), TVM_GETITEMRECT, (WPARAM)textOnly,
    (LPARAM)&rect2) != 0);

  rect.x = rect2.left;
  rect.y = rect2.top;
  rect.width = rect2.right - rect2.left;
  rect.height = rect2.bottom - rect2.left;
  return success;
*/
  wxFAIL_MSG("Not implemented");

  return FALSE;
}

wxTextCtrl* wxTreeCtrl::GetEditControl() const
{
  return m_textCtrl;
}

// Operations
bool wxTreeCtrl::DeleteItem(long item)
{
  GtkTreeItem *p;

  p = findGtkTreeItem(item);
  if (p == NULL)
    return FALSE;

  GtkTreeItem *parent = GTK_TREE_ITEM(GTK_WIDGET(p)->parent);
  if (parent == NULL)
    return FALSE;

  gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(p));

  return TRUE;
}

bool wxTreeCtrl::DeleteChildren(long item)
{
  GtkTreeItem *p;

  p = findGtkTreeItem(item);
  if (p == NULL)
    return FALSE;

  gtk_tree_item_remove_subtree(GTK_TREE_ITEM(p));

  return TRUE;
}

bool wxTreeCtrl::ExpandItem(long item, int action)
{
  GtkTreeItem *p;

  p = findGtkTreeItem(item);

  if (p == NULL)
    return FALSE;

  switch (action) {
    case wxTREE_EXPAND_EXPAND:
      gtk_tree_item_expand(GTK_TREE_ITEM(p));
      break;

    case wxTREE_EXPAND_COLLAPSE:
      gtk_tree_item_collapse(GTK_TREE_ITEM(p));
      break;

    case wxTREE_EXPAND_COLLAPSE_RESET:
      gtk_tree_item_collapse(GTK_TREE_ITEM(p));
      gtk_tree_item_remove_subtree(GTK_TREE_ITEM(p));
      break;

    case wxTREE_EXPAND_TOGGLE:
      if (p->expanded)
        gtk_tree_item_collapse(GTK_TREE_ITEM(p));
      else
        gtk_tree_item_expand(GTK_TREE_ITEM(p));
      break;

    default:
      wxFAIL_MSG("unknown action in wxTreeCtrl::ExpandItem");
  }

  return TRUE;
}

long wxTreeCtrl::InsertItem(long parent, wxTreeItem& info, long insertAfter)
{
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

bool wxTreeCtrl::SelectItem(long item)
{
  GtkTreeItem *p;

  p = findGtkTreeItem(item);
  if (p == NULL)
    return FALSE;

  gtk_tree_item_select(p);
  return TRUE;
}

bool wxTreeCtrl::ScrollTo(long item)
{
  wxFAIL_MSG("Not implemented");

  return FALSE; // Still unimplemented
}

bool wxTreeCtrl::DeleteAllItems()
{
  gtk_tree_item_remove_subtree(m_anchor);
  return TRUE;
}

wxTextCtrl* wxTreeCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    wxASSERT( (textControlClass->IsKindOf(CLASSINFO(wxTextCtrl))) );

/*
    HWND hWnd = (HWND) TreeView_EditLabel((HWND) GetHWND(), item);

    if (m_textCtrl)
    {
      m_textCtrl->UnsubclassWin();
      m_textCtrl->SetHWND(0);
      delete m_textCtrl;
      m_textCtrl = NULL;
    }

    m_textCtrl = (wxTextCtrl*) textControlClass->CreateObject();
    m_textCtrl->SetHWND((WXHWND) hWnd);
    m_textCtrl->SubclassWin((WXHWND) hWnd);

*/	
    wxFAIL_MSG("Not implemented");

    return m_textCtrl;
}

// End label editing, optionally cancelling the edit
bool wxTreeCtrl::EndEditLabel(bool cancel)
{
/*
    bool success = (TreeView_EndEditLabelNow((HWND) GetHWND(), cancel) != 0);

    if (m_textCtrl)
    {
      m_textCtrl->UnsubclassWin();
      m_textCtrl->SetHWND(0);
      delete m_textCtrl;
      m_textCtrl = NULL;
    }
    return success;
*/
  wxFAIL_MSG("Not implemented");

  return FALSE;
}

long wxTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
/*
    TV_HITTESTINFO hitTestInfo;
  hitTestInfo.pt.x = (int) point.x;
  hitTestInfo.pt.y = (int) point.y;

    TreeView_HitTest((HWND) GetHWND(), & hitTestInfo);

  flags = 0;
  if ( hitTestInfo.flags & TVHT_ABOVE )
    flags |= wxTREE_HITTEST_ABOVE;
  if ( hitTestInfo.flags & TVHT_BELOW )
    flags |= wxTREE_HITTEST_BELOW;
  if ( hitTestInfo.flags & TVHT_NOWHERE )
    flags |= wxTREE_HITTEST_NOWHERE;
  if ( hitTestInfo.flags & TVHT_ONITEMBUTTON )
    flags |= wxTREE_HITTEST_ONITEMBUTTON;
  if ( hitTestInfo.flags & TVHT_ONITEMICON )
    flags |= wxTREE_HITTEST_ONITEMICON;
  if ( hitTestInfo.flags & TVHT_ONITEMINDENT )
    flags |= wxTREE_HITTEST_ONITEMINDENT;
  if ( hitTestInfo.flags & TVHT_ONITEMLABEL )
    flags |= wxTREE_HITTEST_ONITEMLABEL;
  if ( hitTestInfo.flags & TVHT_ONITEMRIGHT )
    flags |= wxTREE_HITTEST_ONITEMRIGHT;
  if ( hitTestInfo.flags & TVHT_ONITEMSTATEICON )
    flags |= wxTREE_HITTEST_ONITEMSTATEICON;
  if ( hitTestInfo.flags & TVHT_TOLEFT )
    flags |= wxTREE_HITTEST_TOLEFT;
  if ( hitTestInfo.flags & TVHT_TORIGHT )
    flags |= wxTREE_HITTEST_TORIGHT;

  return (long) hitTestInfo.hItem ;
*/
  wxFAIL_MSG("Not implemented");

  return 0;
}

/*
wxImageList *wxTreeCtrl::CreateDragImage(long item)
{
}
*/

bool wxTreeCtrl::SortChildren(long item)
{
  wxFAIL_MSG("Not implemented");

  return FALSE; // Still unimplemented
}

bool wxTreeCtrl::EnsureVisible(long item)
{
  wxFAIL_MSG("Not implemented");

  return FALSE; // Still unimplemented
}

void wxTreeCtrl::SendExpanding(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_EXPANDING, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

void wxTreeCtrl::SendExpanded(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_EXPANDED, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

void wxTreeCtrl::SendCollapsing(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

void wxTreeCtrl::SendCollapsed(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

void wxTreeCtrl::SendSelChanging(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGED, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

void wxTreeCtrl::SendSelChanged(long item)
{
  wxTreeEvent event(wxEVT_COMMAND_TREE_SEL_CHANGING, GetId());
  event.SetEventObject(this);
  ProcessEvent(event);
}

// Tree item structure
wxTreeItem::wxTreeItem()
{
  m_mask = 0;
  m_itemId = 0;
  m_state = 0;
  m_stateMask = 0;
  m_image = -1;
  m_selectedImage = -1;
  m_children = 0;
  m_data = 0;
}

// If getFullInfo is TRUE, we explicitly query for more info if we haven't got it all.

static void wxConvertFromGtkTreeItem(wxTreeItem& info, GtkTreeItem *gtkItem)
{
  GtkLabel *l;
  char *t;

  info.m_data = (long)gtk_object_get_data(GTK_OBJECT(gtkItem), "data");
  info.m_itemId = (long)gtk_object_get_data(GTK_OBJECT(gtkItem), "id");
  info.m_image = (int)gtk_object_get_data(GTK_OBJECT(gtkItem), "image");
  info.m_selectedImage = (int)gtk_object_get_data(GTK_OBJECT(gtkItem), "selectedImage");

  info.m_mask = 0;
  info.m_state = 0;
  info.m_stateMask = 0;
  l = GTK_LABEL(gtk_object_get_data(GTK_OBJECT(gtkItem), "label"));
  gtk_label_get(l, &t);
  info.m_text = t;
}

static void wxConvertToGtkTreeItem(wxTreeCtrl *owner, wxTreeItem& info, GtkTreeItem **gtkItem)
{
  GtkTreeItem *item = (*gtkItem);

  if (item == NULL) {
    item = GTK_TREE_ITEM(gtk_tree_item_new());

    GtkHBox *m_box = GTK_HBOX(gtk_hbox_new(FALSE, 0));
    gtk_container_add (GTK_CONTAINER (item), GTK_WIDGET(m_box));

    gtk_object_set_data(GTK_OBJECT(item), "box", m_box);

    const wxBitmap *bmp;
    const wxImageList *list;
    if (owner != NULL)
    if ((list = owner->GetImageList(wxIMAGE_LIST_NORMAL)) != NULL)
      if ((bmp = list->GetBitmap(info.m_image)) != NULL)
        if (bmp->Ok()) {
          GdkBitmap *mask = NULL;
          if (bmp->GetMask())
            mask = bmp->GetMask()->GetBitmap();
          GtkPixmap *m_image_widget = GTK_PIXMAP(gtk_pixmap_new(bmp->GetPixmap(), mask));
          gtk_misc_set_alignment (GTK_MISC (m_image_widget), 0.0, 0.5);
          gtk_box_pack_start(GTK_BOX(m_box), GTK_WIDGET(m_image_widget), FALSE, FALSE, 0);
          gtk_object_set_data(GTK_OBJECT(item), "image", m_image_widget);
          gtk_widget_show (GTK_WIDGET(m_image_widget));
        }

    GtkLabel *m_label_widget = GTK_LABEL(gtk_label_new ((char *)(const char *)info.m_text));
    gtk_misc_set_alignment (GTK_MISC (m_label_widget), 0.5, 0.5);

    gtk_box_pack_start(GTK_BOX(m_box), GTK_WIDGET(m_label_widget), FALSE, FALSE, 0);
    gtk_object_set_data(GTK_OBJECT(item), "label", m_label_widget);

    gtk_widget_show (GTK_WIDGET(m_label_widget));

    gtk_widget_show(GTK_WIDGET(m_box));
    gtk_object_set_data(GTK_OBJECT(item), "id", (gpointer)info.m_itemId);
    gtk_object_set_data(GTK_OBJECT(item), "owner", owner);
    (*gtkItem) = item;
  }
}

// Tree event
IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxCommandEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id)
           : wxCommandEvent(commandType, id)
{
  m_code = 0;
  m_oldItem = 0;
}

