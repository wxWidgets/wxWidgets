/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:
// Author:      Denis Pershin
// Created:     08/08/98
// Id:          $Id$
// Copyright:   (c) 1998 Denis Pershin and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKTREECTRL_H__
#define __GTKTREECTRL_H__

#ifdef __GNUG__
#pragma interface "treectrl.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/event.h"
#include "wx/imaglist.h"
#include "wx/scrolwin.h"
#include "wx/dcclient.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxTreeItem;
class wxTreeCtrl;
class wxTreeEvent;

class wxImageList;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

// WXDLLEXPORT extern const char *wxTreeNameStr;

#define wxTREE_MASK_HANDLE          0x0001
#define wxTREE_MASK_STATE           0x0002
#define wxTREE_MASK_TEXT            0x0004
#define wxTREE_MASK_IMAGE           0x0008
#define wxTREE_MASK_SELECTED_IMAGE  0x0010
#define wxTREE_MASK_CHILDREN        0x0020
#define wxTREE_MASK_DATA            0x0040

#define wxTREE_STATE_BOLD           0x0001
#define wxTREE_STATE_DROPHILITED    0x0002
#define wxTREE_STATE_EXPANDED       0x0004
#define wxTREE_STATE_EXPANDEDONCE   0x0008
#define wxTREE_STATE_FOCUSED        0x0010
#define wxTREE_STATE_SELECTED       0x0020
#define wxTREE_STATE_CUT            0x0040

#define wxTREE_HITTEST_ABOVE            0x0001  // Above the client area.
#define wxTREE_HITTEST_BELOW            0x0002  // Below the client area.
#define wxTREE_HITTEST_NOWHERE          0x0004  // In the client area but below the last item.
#define wxTREE_HITTEST_ONITEMBUTTON     0x0010  // On the button associated with an item.
#define wxTREE_HITTEST_ONITEMICON       0x0020  // On the bitmap associated with an item.
#define wxTREE_HITTEST_ONITEMINDENT     0x0040  // In the indentation associated with an item.
#define wxTREE_HITTEST_ONITEMLABEL      0x0080  // On the label (string) associated with an item.
#define wxTREE_HITTEST_ONITEMRIGHT      0x0100  // In the area to the right of an item.
#define wxTREE_HITTEST_ONITEMSTATEICON  0x0200  // On the state icon for a tree view item that is in a user-defined state.
#define wxTREE_HITTEST_TOLEFT           0x0400  // To the right of the client area.
#define wxTREE_HITTEST_TORIGHT          0x0800  // To the left of the client area.

#define wxTREE_HITTEST_ONITEM (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL | wxTREE_HITTEST_ONITEMSTATEICON)

// Flags for GetNextItem
enum {
    wxTREE_NEXT_CARET,                 // Retrieves the currently selected item.
    wxTREE_NEXT_CHILD,                 // Retrieves the first child item. The hItem parameter must be NULL.
    wxTREE_NEXT_DROPHILITE,            // Retrieves the item that is the target of a drag-and-drop operation.
    wxTREE_NEXT_FIRSTVISIBLE,          // Retrieves the first visible item.
    wxTREE_NEXT_NEXT,                  // Retrieves the next sibling item.
    wxTREE_NEXT_NEXTVISIBLE,           // Retrieves the next visible item that follows the specified item.
    wxTREE_NEXT_PARENT,                // Retrieves the parent of the specified item.
    wxTREE_NEXT_PREVIOUS,              // Retrieves the previous sibling item.
    wxTREE_NEXT_PREVIOUSVISIBLE,       // Retrieves the first visible item that precedes the specified item.
    wxTREE_NEXT_ROOT                   // Retrieves the first child item of the root item of which the specified item is a part.
};

// Flags for ExpandItem
enum {
    wxTREE_EXPAND_EXPAND,
    wxTREE_EXPAND_COLLAPSE,
    wxTREE_EXPAND_COLLAPSE_RESET,
    wxTREE_EXPAND_TOGGLE
};

// Flags for InsertItem
enum {
    wxTREE_INSERT_LAST = -1,
    wxTREE_INSERT_FIRST = -2,
    wxTREE_INSERT_SORT = -3
};

//-----------------------------------------------------------------------------
// wxTreeItem
//-----------------------------------------------------------------------------


class WXDLLEXPORT wxTreeItem: public wxObject
{
public:
  wxTreeCtrl  *m_owner;
  GtkWidget      *m_parentwidget;
  GtkTreeItem    *m_widget;
  GtkTree        *m_tree;
  long            m_mask;
  long            m_itemId;
  long            m_state;
  long            m_stateMask;
  wxString        m_text;
  int             m_image;
  int             m_selectedImage;
  int             m_children;
  wxList          m_childlist;
  long            m_data;

  wxTreeItem();
  wxTreeItem(GtkWidget *parent, const wxTreeItem &item);
  ~wxTreeItem();

  void Create();

  void AddChild(wxTreeItem *child);
  void DeleteChildren();
  bool HasChildren();
  wxTreeItem *FindItem(long itemId) const;
  wxTreeItem *FindItem(GtkTreeItem *item) const;

  // Accessors
  inline long GetMask() const { return m_mask; }
  inline long GetItemId() const { return m_itemId; }
  inline long GetState() const { return m_state; }
  inline long GetStateMask() const { return m_stateMask; }
  inline wxString GetText() const { return m_text; }
  inline int GetImage() const { return m_image; }
  inline int GetSelectedImage() const { return m_selectedImage; }
  inline const wxList &GetChildren() const { return m_childlist; }
  inline long GetData() const { return m_data; }

  inline void SetMask(long mask) { m_mask = mask; }
  inline void SetItemId(long id) { m_itemId = m_itemId = id; }
  inline void GetState(long state) { m_state = state; }
  inline void SetStateMask(long stateMask) { m_stateMask = stateMask; }
  inline void GetText(const wxString& text) { m_text = text; }
  inline void SetImage(int image) { m_image = image; }
  inline void SetSelectedImage(int selImage) { m_selectedImage = selImage; }
  inline void SetChildren(const wxList &children) { m_childlist = children; }
  inline void SetData(long data) { m_data = data; }

  int NumberOfVisibleDescendents();
private:
  friend wxTreeCtrl;
  friend void gtk_treeitem_expand_callback(GtkWidget *widget, wxTreeItem *treeitem);
  friend void gtk_treeitem_collapse_callback(GtkWidget *widget, wxTreeItem *treeitem);
  friend void gtk_treeitem_select_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem);
  friend void gtk_treeitem_deselect_callback( GtkWidget *WXUNUSED(widget), wxTreeItem *treeitem);

  void AddSubtree();
  void PrepareEvent(wxTreeEvent &event);
  void SendDelete(wxWindow *target);
  void SendExpand(wxWindow *target);
  void SendExpanding(wxWindow *target);
  void SendCollapse(wxWindow *target);
  void SendCollapsing(wxWindow *target);
  void SendSelChanged(wxWindow *target);
  void SendSelChanging(wxWindow *target);

  inline wxTreeItem *GetChild(long no) {
    return (wxTreeItem *)(m_childlist.Nth(no)->Data());
  }
  inline long GetChildrenNumber() {
    return m_childlist.Number();
  }

  guint expand_handler;
  guint collapse_handler;

// It is a quick hack to make TreeCtrl working... I do not know why
// but signals is GTK does not disconnected...
  bool ignore;

  DECLARE_DYNAMIC_CLASS(wxTreeItem)
};

//-----------------------------------------------------------------------------
// wxTreeCtrl
//-----------------------------------------------------------------------------

class wxTreeCtrl: public wxScrolledWindow {
public:
  GtkTree             *m_tree;

  wxTreeCtrl();
  wxTreeCtrl(wxWindow *parent, const wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTR_HAS_BUTTONS,
      const wxString& name = "wxTreeCtrl" );
  ~wxTreeCtrl();
  bool Create(wxWindow *parent, const wxWindowID id = -1,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTR_HAS_BUTTONS,
      const wxString& name = "wxTreeCtrl");

  int GetCount() const;

  long InsertItem( long parent, const wxString& label,
                   int image = -1, int selImage = -1, 
                   long insertAfter = wxTREE_INSERT_LAST );

  long InsertItem( long parent, wxTreeItem &info,
                   long insertAfter = wxTREE_INSERT_LAST );
  void DeleteItem( long item );
  void DeleteChildren( long item );
  bool DeleteAllItems();

  bool ExpandItem(long item)   { return ExpandItem(item, wxTREE_EXPAND_EXPAND);   }
  bool ExpandItem( long item, int action );

  bool CollapseItem(long item) { return ExpandItem(item, wxTREE_EXPAND_COLLAPSE); }
  bool ToggleItem(long item)   { return ExpandItem(item, wxTREE_EXPAND_TOGGLE);   }

    // is the item expanded now?
  bool IsItemExpanded(long item)
  {
    wxTreeItem *pItem = FindItem(item);
    return pItem && (pItem->GetState() & wxTREE_STATE_EXPANDED);
  }

/*
  bool GetItem( wxTreeItem &info ) const;
*/

  long GetItemData( long item ) const;
  wxString GetItemText( long item ) const;
  int  GetItemImage(long item) const;
  long GetParent( long item ) const;
  long GetRootItem() const;
/*
  long GetSelection() const;
  bool SelectItem( long item );
*/

  bool ItemHasChildren( long item ) const;
  void SetIndent( int indent );
  int GetIndent() const;

/*
  bool SetItem( wxTreeItem &info );
  bool SetItemData( long item, long data );
  bool SetItemText( long item, const wxString &text );
  void SetItemImage(long item, int image, int imageSel) const;
  long HitTest( const wxPoint& point, int &flags );
*/

  wxTreeItem *FindItem(GtkTreeItem *item) const;

  wxImageList *GetImageList(int which = wxIMAGE_LIST_NORMAL) const;
  void SetImageList(wxImageList *imageList, int which = wxIMAGE_LIST_NORMAL);

private:
  // set the selection to the specified item generating appropriate event(s) if
  // not disabled
  void SelectItem(wxTreeItem *item, bool bDoEvents = TRUE);

  wxTreeItem       *m_anchor;
  wxTreeItem       *m_current;
  int                  m_indent;
  long                 m_lastId;
  bool                 m_isCreated;
  wxImageList         *m_imageList;
  wxImageList         *m_smallImageList;

  void CalculateLevel( wxTreeItem *item, wxPaintDC &dc, int level, int &y );
  void CalculatePositions();
  wxTreeItem *FindItem( long itemId ) const;

//  DECLARE_EVENT_TABLE()
  DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
};

//-----------------------------------------------------------------------------
// wxTreeEvent
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxTreeEvent)

 public:
  wxTreeEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

  int           m_code;
  wxTreeItem    m_item;
  long          m_oldItem;
  wxPoint       m_pointDrag;

  inline long GetOldItem() const { return m_oldItem; }
  inline wxTreeItem& GetItem() const { return (wxTreeItem&) m_item; }
  inline wxPoint GetPoint() const { return m_pointDrag; }
  inline void SetCode(int code) { m_code = code; }
  inline int GetCode() const { return m_code; }
};

typedef void (wxEvtHandler::*wxTreeEventFunction)(wxTreeEvent&);

#define EVT_TREE_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn , (wxObject *) NULL },
#define EVT_TREE_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_DELETE_ITEM(id, fn) { wxEVT_COMMAND_TREE_DELETE_ITEM, id, -1, ( wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_GET_INFO(id, fn) { wxEVT_COMMAND_TREE_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SET_INFO(id, fn) { wxEVT_COMMAND_TREE_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDED(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDING(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSED(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSING(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGED, id, -1, ( wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_KEY_DOWN(id, fn) { wxEVT_COMMAND_TREE_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

#endif
    // __GTKTREECTRL_H__
