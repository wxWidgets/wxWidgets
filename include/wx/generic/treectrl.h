/*
 * Author: Robert Roebling
 *
 * Copyright: (C) 1997,1998 Robert Roebling
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the wxWindows Licence, which
 * you have received with this library (see Licence.htm).
 *
 */

#ifndef __GTKTREECTRLH_G__
#define __GTKTREECTRLH_G__

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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxTreeItem;
class wxTreeEvent;

class wxGenericTreeItem;
class wxTreeCtrl;

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

/* defined in "wx/event.h"
 wxEVT_COMMAND_TREE_BEGIN_DRAG,
 wxEVT_COMMAND_TREE_BEGIN_RDRAG,
 wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,
 wxEVT_COMMAND_TREE_END_LABEL_EDIT,
 wxEVT_COMMAND_TREE_DELETE_ITEM,
 wxEVT_COMMAND_TREE_GET_INFO,
 wxEVT_COMMAND_TREE_SET_INFO,
 wxEVT_COMMAND_TREE_ITEM_EXPANDED,
 wxEVT_COMMAND_TREE_ITEM_EXPANDING,
 wxEVT_COMMAND_TREE_SEL_CHANGED,
 wxEVT_COMMAND_TREE_SEL_CHANGING,
 wxEVT_COMMAND_TREE_KEY_DOWN
*/

//-----------------------------------------------------------------------------
// wxTreeItem
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxTreeItem: public wxObject
{
public:
  long            m_mask;
  long            m_itemId;
  long            m_state;
  long            m_stateMask;
  wxString        m_text;
  int             m_image;
  int             m_selectedImage;
  int             m_children;
  long            m_data;

  wxTreeItem();

  // Accessors
  inline long GetMask() const { return m_mask; }
  inline long GetItemId() const { return m_itemId; }
  inline long GetState() const { return m_state; }
  inline long GetStateMask() const { return m_stateMask; }
  inline wxString GetText() const { return m_text; }
  inline int GetImage() const { return m_image; }
  inline int GetSelectedImage() const { return m_selectedImage; }
  inline int GetChildren() const { return m_children; }
  inline long GetData() const { return m_data; }

  inline void SetMask(long mask) { m_mask = mask; }
  inline void SetItemId(long id) { m_itemId = m_itemId = id; }
  inline void GetState(long state) { m_state = state; }
  inline void SetStateMask(long stateMask) { m_stateMask = stateMask; }
  inline void GetText(const wxString& text) { m_text = text; }
  inline void SetImage(int image) { m_image = image; }
  inline void SetSelectedImage(int selImage) { m_selectedImage = selImage; }
  inline void SetChildren(int children) { m_children = children; }
  inline void SetData(long data) { m_data = data; }

  DECLARE_DYNAMIC_CLASS(wxTreeItem)
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
#define EVT_TREE_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_DELETE_ITEM(id, fn) { wxEVT_COMMAND_TREE_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_GET_INFO(id, fn) { wxEVT_COMMAND_TREE_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SET_INFO(id, fn) { wxEVT_COMMAND_TREE_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDED(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_EXPANDING(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSED(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_ITEM_COLLAPSING(id, fn) { wxEVT_COMMAND_TREE_ITEM_COLLAPSING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },
#define EVT_TREE_KEY_DOWN(id, fn) { wxEVT_COMMAND_TREE_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn, (wxObject *) NULL },

//-----------------------------------------------------------------------------
// wxGenericTreeItem
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericTreeItem: public wxTreeItem
{
  DECLARE_DYNAMIC_CLASS(wxGenericTreeItem)

  public:
    bool                m_hasChildren,
                        m_isCollapsed;

    int                 m_x,m_y;
    int                 m_height,m_width;
    int                 m_xCross,m_yCross;
    int                 m_level;
    wxList              m_children;
    wxGenericTreeItem  *m_parent;
    bool                m_hasHilight;

    wxGenericTreeItem() {};
    wxGenericTreeItem( wxGenericTreeItem *parent );
    wxGenericTreeItem( wxGenericTreeItem *parent, const wxTreeItem& item, wxDC *dc );
    void SetItem( const wxTreeItem &item, wxDC *dc );
    void SetText( const wxString &text, wxDC *dc );
    void Reset();
    void GetItem( wxTreeItem &item ) const;
    void AddChild( const wxTreeItem &item );
    bool HasChildren();
    bool HasPlus();
    int NumberOfVisibleDescendents();
    int NumberOfVisibleChildren();
    wxGenericTreeItem *FindItem( long itemId ) const;
    void AddChild( wxGenericTreeItem *child );
    void SetCross( int x, int y );
    void GetSize( int &x, int &y );
    long HitTest( const wxPoint& point, int &flags );
    void PrepareEvent( wxTreeEvent &event );
    void SendKeyDown( wxWindow *target );
    void SendSelected( wxWindow *target );
    void SendDelete( wxWindow *target );
    void SendExpand( wxWindow *target );
    void SendCollapse( wxWindow *target );
    void SetHilight( bool set = TRUE );
    bool HasHilight();
    bool IsExpanded() const { return !m_isCollapsed; }
};

//-----------------------------------------------------------------------------
// wxTreeCtrl
//-----------------------------------------------------------------------------

class wxTreeCtrl: public wxScrolledWindow
{
public:
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
  long InsertItem( long parent, const wxString& label, int image = -1,
                   int selImage = -1, long insertAfter = wxTREE_INSERT_LAST );
  long InsertItem( long parent, wxTreeItem &info,
                   long insertAfter = wxTREE_INSERT_LAST );
  void DeleteItem( long item );
  void DeleteChildren( long item );
  bool DeleteAllItems();
  bool ExpandItem(long item)   { return ExpandItem(item, wxTREE_EXPAND_EXPAND);   }
  bool CollapseItem(long item) { return ExpandItem(item, wxTREE_EXPAND_COLLAPSE); }
  bool ToggleItem(long item)   { return ExpandItem(item, wxTREE_EXPAND_TOGGLE);   }
  bool ExpandItem( long item, int action );

    // is the item expanded now?
  bool IsItemExpanded(long item)
  {
    wxGenericTreeItem *pItem = FindItem(item);
    return pItem && (pItem->GetState() & wxTREE_STATE_EXPANDED);
  }

  bool GetItem( wxTreeItem &info ) const;
  long GetItemData( long item ) const;
  wxString GetItemText( long item ) const;
  int  GetItemImage(long item) const;
  long GetParent( long item ) const;
  long GetRootItem() const;
  long GetChild(long item) const;
  long GetNextItem(long item, int code) const;
  long GetSelection() const;
  bool SelectItem( long item );
  bool ItemHasChildren( long item ) const;
  void SetIndent( int indent );
  int GetIndent() const;
  bool SetItem( wxTreeItem &info );
  bool SetItemData( long item, long data );
  bool SetItemText( long item, const wxString &text );
  void SetItemImage(long item, int image, int imageSel) const;
  long HitTest( const wxPoint& point, int &flags );
  
  int  GetItemState(long item, long stateMask) const;
  bool SetItemState(long item, long state, long stateMask);

  void AdjustMyScrollbars();
  void PaintLevel( wxGenericTreeItem *item, wxPaintDC &dc, int level, int &y );
  void OnPaint( const wxPaintEvent &event );
  void OnSetFocus( const wxFocusEvent &event );
  void OnKillFocus( const wxFocusEvent &event );
  void OnChar( wxKeyEvent &event );
  void OnMouse( const wxMouseEvent &event );

  wxImageList *GetImageList(int which = wxIMAGE_LIST_NORMAL) const;
  void SetImageList(wxImageList *imageList, int which = wxIMAGE_LIST_NORMAL);

private:
  // set the selection to the specified item generating appropriate event(s)
  void SelectItem(wxGenericTreeItem *item);

  wxGenericTreeItem   *m_anchor;
  wxGenericTreeItem   *m_current;
  bool                 m_hasFocus;
  int                  m_xScroll,m_yScroll;
  int                  m_indent;
  long                 m_lastId;
  int                  m_lineHeight;
  wxPen                m_dottedPen;
  bool                 m_isCreated;
  wxBrush             *m_hilightBrush;
  wxImageList         *m_imageList;
  wxImageList         *m_smallImageList;

  void CalculateLevel( wxGenericTreeItem *item, wxPaintDC &dc, int level, int &y );
  void CalculatePositions();
  wxGenericTreeItem *FindItem( long itemId ) const;
  void RefreshLine( wxGenericTreeItem *item );

  DECLARE_EVENT_TABLE()
  DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
};

#endif
    // __GTKTREECTRLH_G__
