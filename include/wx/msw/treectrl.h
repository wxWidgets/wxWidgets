/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TREECTRLH__
#define __TREECTRLH__

#ifdef __GNUG__
#pragma interface "treectrl.h"
#endif

#include "wx/control.h"
#include "wx/event.h"
#include "wx/imaglist.h"

// WXDLLEXPORT_DATA(extern const char*) wxTreeNameStr;

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

#define wxTREE_HITTEST_ONITEM (wxTREE_HITTEST_ONITEMICON | wxTREE_HITTEST_ONITEMLABEL wxTREE_HITTEST_ONITEMSTATEICON)

// Flags for GetNextItem
enum {
    wxTREE_NEXT_CARET,                 // Retrieves the currently selected item.
    wxTREE_NEXT_CHILD,                 // Retrieves the first child item. The hItem parameter must be NULL.
    wxTREE_NEXT_DROPHILITE,            // Retrieves the item that is the target of a drag-and-drop operation.
    wxTREE_NEXT_FIRSTVISIBLE,          // Retrieves the first visible item.
    wxTREE_NEXT_NEXT,                 // Retrieves the next sibling item.
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

class WXDLLEXPORT wxTreeItem: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxTreeItem)
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

    wxTreeItem(void);
};

class WXDLLEXPORT wxTreeCtrl: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxTreeCtrl)
 public:
  /*
   * Public interface
   */

  wxTreeCtrl(void);

  inline wxTreeCtrl(wxWindow *parent, const wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxTR_HAS_BUTTONS,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = "wxTreeCtrl")
  {
    Create(parent, id, pos, size, style, validator, name);
  }
  ~wxTreeCtrl(void);

  bool Create(wxWindow *parent, const wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const long style = wxTR_HAS_BUTTONS,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = "wxTreeCtrl");

  // Attributes
  int GetCount(void) const ;
  int GetIndent(void) const ;
  void SetIndent(int indent) ;
  wxImageList *GetImageList(const int which = wxIMAGE_LIST_NORMAL) const ;
  void SetImageList(wxImageList *imageList, const int which = wxIMAGE_LIST_NORMAL) ;
  long GetNextItem(const long item, int code) const ;
  bool ItemHasChildren(const long item) const ;
  long GetChild(const long item) const ;
  long GetParent(const long item) const ;
  long GetFirstVisibleItem(void) const ;
  long GetNextVisibleItem(const long item) const ;
  long GetSelection(void) const ;
  long GetRootItem(void) const ;
  bool GetItem(wxTreeItem& info) const ;
  bool SetItem(wxTreeItem& info) ;
  int  GetItemState(const long item, const long stateMask) const ;
  bool SetItemState(const long item, const long state, const long stateMask) ;
  bool SetItemImage(const long item, const int image, const int selImage) ;
  wxString GetItemText(const long item) const ;
  void SetItemText(const long item, const wxString& str) ;
  long GetItemData(const long item) const ;
  bool SetItemData(const long item, long data) ;
  bool GetItemRect(const long item, wxRectangle& rect, bool textOnly = FALSE) const;
  wxTextCtrl& GetEditControl(void) const;

  // Operations
  bool DeleteItem(const long item);
  bool ExpandItem(const long item, const int action);
  long InsertItem(const long parent, wxTreeItem& info, const long insertAfter = wxTREE_INSERT_LAST);

  // If image > -1 and selImage == -1, the same image is used for
  // both selected and unselected items.
  long InsertItem(const long parent, const wxString& label, const int image = -1, const int selImage = -1, const long insertAfter = wxTREE_INSERT_LAST);
  bool SelectItem(const long item);
  bool ScrollTo(const long item);
  bool DeleteAllItems(void) ;
  wxTextCtrl& Edit(const long item) ;
  long HitTest(const wxPoint& point, int& flags);
//  wxImageList *CreateDragImage(const long item) ;
  bool SortChildren(const long item) ;
  bool EnsureVisible(const long item) ;

  void Command(wxCommandEvent& event) { ProcessCommand(event); };

  // IMPLEMENTATION
  bool MSWCommand(const WXUINT param, const WXWORD id);
  bool MSWNotify(const WXWPARAM wParam, const WXLPARAM lParam);

protected:
  wxTextCtrl m_textCtrl;
  wxImageList *m_imageListNormal;
  wxImageList *m_imageListState;
};

/*
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

class WXDLLEXPORT wxTreeEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxTreeEvent)

 public:
  wxTreeEvent(WXTYPE commandType = 0, int id = 0);

  int           m_code;
  wxTreeItem    m_item;
  long          m_oldItem;
  wxPoint       m_pointDrag;
};

typedef void (wxEvtHandler::*wxTreeEventFunction)(wxTreeEvent&);

#define EVT_TREE_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_TREE_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_TREE_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_DELETE_ITEM(id, fn) { wxEVT_COMMAND_TREE_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_GET_INFO(id, fn) { wxEVT_COMMAND_TREE_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_SET_INFO(id, fn) { wxEVT_COMMAND_TREE_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_ITEM_EXPANDED(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_ITEM_EXPANDING(id, fn) { wxEVT_COMMAND_TREE_ITEM_EXPANDING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_SEL_CHANGED(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_SEL_CHANGING(id, fn) { wxEVT_COMMAND_TREE_SEL_CHANGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },
#define EVT_TREE_KEY_DOWN(id, fn) { wxEVT_COMMAND_TREE_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxTreeEventFunction) & fn },

#endif
    // __TREECTRLH__
