/*
 * File:  TreeCtrl.cpp
 * Purpose: Tree control
 * Author:  Julian Smart
 * Created: 1997
 * Updated: 
 * Copyright:
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation "treectrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx/log.h"

#if defined(__WIN95__)

#include "wx/treectrl.h"
#include "wx/msw/private.h"

#ifndef __GNUWIN32__
#include <commctrl.h>
#endif

// Bug in headers, sometimes
#ifndef TVIS_FOCUSED
#define TVIS_FOCUSED            0x0001
#endif

static void wxConvertToMSWTreeItem(wxTreeItem& info, TV_ITEM& tvItem);
static void wxConvertFromMSWTreeItem(wxTreeItem& info, TV_ITEM& tvItem, HWND getFullInfo = 0);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTreeItem, wxObject)

#endif

wxTreeCtrl::wxTreeCtrl(void)
{
  m_imageListNormal = NULL;
  m_imageListState = NULL;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name)
{
  wxSystemSettings settings;
  SetBackgroundColour(settings.GetSystemColour(wxSYS_COLOUR_WINDOW));
  SetForegroundColour(parent->GetDefaultForegroundColour());

  SetName(name);
  SetValidator(validator);

  m_imageListNormal = NULL;
  m_imageListState = NULL;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

//  SetFont(wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL));

  SetParent(parent);

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  m_windowId = (id == -1) ? NewControlId() : id;

  DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | TVS_HASLINES;
  
  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if (want3D || (m_windowStyle & wxSIMPLE_BORDER) || (m_windowStyle & wxRAISED_BORDER) ||
       (m_windowStyle & wxSUNKEN_BORDER) || (m_windowStyle & wxDOUBLE_BORDER))
    wstyle |= WS_BORDER;

  if ( m_windowStyle & wxTR_HAS_BUTTONS )
    wstyle |= TVS_HASBUTTONS;

  if ( m_windowStyle & wxTR_EDIT_LABELS )
    wstyle |= TVS_EDITLABELS;

  if ( m_windowStyle & wxTR_LINES_AT_ROOT )
    wstyle |= TVS_LINESATROOT;

  // Create the toolbar control.
  HWND hWndTreeControl = CreateWindowEx(exStyle,
    WC_TREEVIEW,
    "",
    wstyle,
    x, y, width, height,
    (HWND) parent->GetHWND(),
    (HMENU)m_windowId,
    wxGetInstance(),
    NULL );

  m_hWnd = (WXHWND) hWndTreeControl;
  if (parent) parent->AddChild(this);
  
  SubclassWin((WXHWND) m_hWnd);

  return TRUE;
}

wxTreeCtrl::~wxTreeCtrl(void)
{
  m_textCtrl.SetHWND((WXHWND) NULL);
}

// Attributes
int wxTreeCtrl::GetCount(void) const
{
  return (int) TreeView_GetCount((HWND) GetHWND());
}

int wxTreeCtrl::GetIndent(void) const
{
  return (int) TreeView_GetIndent((HWND) GetHWND());
}

void wxTreeCtrl::SetIndent(int indent)
{
  TreeView_SetIndent((HWND) GetHWND(), indent);
}

wxImageList *wxTreeCtrl::GetImageList(int which) const
{
  if ( which == wxIMAGE_LIST_NORMAL )
    {
    return m_imageListNormal;
  }
  else if ( which == wxIMAGE_LIST_STATE )
    {
    return m_imageListState;
  }
  return NULL;
}

void wxTreeCtrl::SetImageList(wxImageList *imageList, int which)
{
  int flags = 0;
  if ( which == wxIMAGE_LIST_NORMAL )
    {
    flags = TVSIL_NORMAL;
    m_imageListNormal = imageList;
  }
  else if ( which == wxIMAGE_LIST_STATE )
    {
    flags = TVSIL_STATE;
    m_imageListState = imageList;
  }
  TreeView_SetImageList((HWND) GetHWND(), (HIMAGELIST) imageList ? imageList->GetHIMAGELIST() : 0, flags);
}

long wxTreeCtrl::GetNextItem(long item, int code) const
{
  UINT flag = 0;
  switch ( code )
  {
    case wxTREE_NEXT_CARET:
      flag = TVGN_CARET;
      break;
    case wxTREE_NEXT_CHILD:
      flag = TVGN_CHILD;
      break;
    case wxTREE_NEXT_DROPHILITE:
      flag = TVGN_DROPHILITE;
      break;
    case wxTREE_NEXT_FIRSTVISIBLE:
      flag = TVGN_FIRSTVISIBLE;
      break;
    case wxTREE_NEXT_NEXT:
      flag = TVGN_NEXT;
      break;
    case wxTREE_NEXT_NEXTVISIBLE:
      flag = TVGN_NEXTVISIBLE;
      break;
    case wxTREE_NEXT_PARENT:
      flag = TVGN_PARENT;
      break;
    case wxTREE_NEXT_PREVIOUS:
      flag = TVGN_PREVIOUS;
      break;
    case wxTREE_NEXT_PREVIOUSVISIBLE:
      flag = TVGN_PREVIOUSVISIBLE;
      break;
    case wxTREE_NEXT_ROOT:
      flag = TVGN_ROOT;
      break;

    default :
      break;
  }
  return (long) TreeView_GetNextItem( (HWND) GetHWND(), (HTREEITEM) item, flag);
}

bool wxTreeCtrl::ItemHasChildren(long item) const
{
  TV_ITEM item2;
  item2.hItem = (HTREEITEM) item;
  item2.mask = TVIF_CHILDREN;
  TreeView_GetItem((HWND) GetHWND(), &item2);
  return (item2.cChildren != 0);
}

long wxTreeCtrl::GetChild(long item) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)item);
}

long wxTreeCtrl::GetParent(long item) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)item);
}

long wxTreeCtrl::GetFirstVisibleItem(void) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
}

long wxTreeCtrl::GetNextVisibleItem(long item) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)item);
}

long wxTreeCtrl::GetSelection(void) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_CARET, 0);
}

long wxTreeCtrl::GetRootItem(void) const
{
  return (long) ::SendMessage((HWND) GetHWND(), TVM_GETNEXTITEM, TVGN_ROOT, 0);
}

bool wxTreeCtrl::GetItem(wxTreeItem& info) const
{
  TV_ITEM tvItem;
  tvItem.hItem   = (HTREEITEM)info.m_itemId;
  tvItem.pszText = NULL;
  tvItem.mask    = 
  tvItem.stateMask = 0;

  // TODO: convert other bits in the mask
  if ( info.m_mask & wxTREE_MASK_TEXT )
  {
    tvItem.mask |= TVIF_TEXT;
    tvItem.pszText = new char[513];
    tvItem.cchTextMax = 512;
  }
  if ( info.m_mask & wxTREE_MASK_DATA )
    tvItem.mask |= TVIF_PARAM;
  if ( info.m_stateMask & wxTREE_STATE_EXPANDED ) {
    tvItem.mask |= TVIF_STATE;
    tvItem.stateMask |= TVIS_EXPANDED;
  }

  bool success = TreeView_GetItem((HWND)GetHWND(), &tvItem) != 0;

  if ( !success )
  {
    wxLogSysError("TreeView_GetItem failed");

    if (tvItem.pszText)
      delete[] tvItem.pszText;

    return FALSE;
  }

  wxConvertFromMSWTreeItem(info, tvItem);

  if (tvItem.pszText)
    delete[] tvItem.pszText;

  return success;
}

bool wxTreeCtrl::SetItem(wxTreeItem& info)
{
  TV_ITEM item;
    wxConvertToMSWTreeItem(info, item);
  return (::SendMessage((HWND) GetHWND(), TVM_SETITEM, 0, (LPARAM)&item) != 0);
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
  if ( selImage > -1)
  {
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
  RECT rect2;

  *(HTREEITEM*)& rect2 = (HTREEITEM) item;
  bool success = (::SendMessage((HWND) GetHWND(), TVM_GETITEMRECT, (WPARAM)textOnly,
    (LPARAM)&rect2) != 0);

  rect.x = rect2.left;
  rect.y = rect2.top;
  rect.width = rect2.right - rect2.left;
  rect.height = rect2.bottom - rect2.left;
  return success;
}

wxTextCtrl& wxTreeCtrl::GetEditControl(void) const
{
  HWND hWnd = (HWND) TreeView_GetEditControl((HWND) GetHWND());
  ((wxTreeCtrl *)this)->m_textCtrl.SetHWND((WXHWND) hWnd);
  return (wxTextCtrl&) m_textCtrl;
}

// Operations
bool wxTreeCtrl::DeleteItem(long item)
{
  return (TreeView_DeleteItem((HWND) GetHWND(), (HTREEITEM) item) != 0);
}

bool wxTreeCtrl::ExpandItem(long item, int action)
{
  UINT mswAction = TVE_EXPAND;
  switch ( action )
  {
    case wxTREE_EXPAND_EXPAND:
      mswAction = TVE_EXPAND;
      break;

    case wxTREE_EXPAND_COLLAPSE:
      mswAction = TVE_COLLAPSE;
      break;

    case wxTREE_EXPAND_COLLAPSE_RESET:
      // @@@ it should also delete all the items! currently, if you do use this
      //     code your program will probaly crash. Feel free to remove this if
      //     it does work
      wxFAIL_MSG("wxTREE_EXPAND_COLLAPSE_RESET probably doesn't work.");
      mswAction = TVE_COLLAPSERESET;
      break;

    case wxTREE_EXPAND_TOGGLE:
      mswAction = TVE_TOGGLE;
      break;

    default:
      wxFAIL_MSG("unknown action in wxTreeCtrl::ExpandItem");
  }

  bool bOk = TreeView_Expand((HWND)GetHWND(), (HTREEITEM)item, mswAction) != 0;

  // TreeView_Expand doesn't send TVN_EXPAND(ING) messages, so emulate them
  if ( bOk ) {
    wxTreeEvent event(wxEVT_NULL, m_windowId);
    event.m_item.m_itemId  = item;
    event.m_item.m_mask      =
    event.m_item.m_stateMask = 0xffff; // get all
    GetItem(event.m_item);

    bool bIsExpanded = (event.m_item.m_state & wxTREE_STATE_EXPANDED) != 0;

    event.m_code = action;
    event.SetEventObject(this);

    // @@@ return values of {EXPAND|COLLAPS}ING event handler is discarded
    event.SetEventType(bIsExpanded ? wxEVT_COMMAND_TREE_ITEM_EXPANDING
                                   : wxEVT_COMMAND_TREE_ITEM_COLLAPSING);
    GetEventHandler()->ProcessEvent(event);

    event.SetEventType(bIsExpanded ? wxEVT_COMMAND_TREE_ITEM_EXPANDED
                                   : wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    GetEventHandler()->ProcessEvent(event);
  }

  return bOk;
}

long wxTreeCtrl::InsertItem(long parent, wxTreeItem& info, long insertAfter)
{
  TV_INSERTSTRUCT tvInsertStruct;
  tvInsertStruct.hParent = (HTREEITEM) parent ;
  tvInsertStruct.hInsertAfter = (HTREEITEM) insertAfter ;

  wxConvertToMSWTreeItem(info, tvInsertStruct.item);

  return (long) TreeView_InsertItem((HWND) GetHWND(), & tvInsertStruct);
}

long wxTreeCtrl::InsertItem(long parent, const wxString& label, int image, int selImage,
  long insertAfter)
{
  wxTreeItem info;
  info.m_text = label;
  info.m_mask = wxTREE_MASK_TEXT;
  if ( image > -1 )
  {
    info.m_mask |= wxTREE_MASK_IMAGE | wxTREE_MASK_SELECTED_IMAGE;
    info.m_image = image;
    if ( selImage == -1 )
      info.m_selectedImage = image;
    else
      info.m_selectedImage = selImage;
  }

  return InsertItem(parent, info, insertAfter);
}

bool wxTreeCtrl::SelectItem(long item)
{
  return (TreeView_SelectItem((HWND) GetHWND(), (HTREEITEM) item) != 0);
}

bool wxTreeCtrl::ScrollTo(long item)
{
  return (TreeView_SelectSetFirstVisible((HWND) GetHWND(), (HTREEITEM) item) != 0);
}

bool wxTreeCtrl::DeleteAllItems(void)
{
  return (TreeView_DeleteAllItems((HWND) GetHWND()) != 0);
}

wxTextCtrl& wxTreeCtrl::Edit(long item)
{
  HWND hWnd = (HWND) TreeView_EditLabel((HWND) GetHWND(), item);
  m_textCtrl.SetHWND((WXHWND) hWnd);
  return m_textCtrl;
}

long wxTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
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
}

/*
wxImageList *wxTreeCtrl::CreateDragImage(long item)
{
}
*/

bool wxTreeCtrl::SortChildren(long item)
{
  return (TreeView_SortChildren((HWND) GetHWND(), (HTREEITEM) item, 0) != 0);
}

bool wxTreeCtrl::EnsureVisible(long item)
{
  return (TreeView_EnsureVisible((HWND) GetHWND(), (HTREEITEM) item) != 0);
}

bool wxTreeCtrl::MSWCommand(WXUINT cmd, WXWORD id)
{
  if (cmd == EN_UPDATE)
  {
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, id);
    event.SetEventObject( this );
    ProcessCommand(event);
    return TRUE;
  }
  else if (cmd == EN_KILLFOCUS)
  {
    wxCommandEvent event(wxEVT_KILL_FOCUS, id);
    event.SetEventObject( this );
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

bool wxTreeCtrl::MSWNotify(WXWPARAM wParam, WXLPARAM lParam)
{
  wxTreeEvent event(wxEVT_NULL, m_windowId);
  wxEventType eventType = wxEVT_NULL;
  NMHDR* hdr1 = (NMHDR*) lParam;
  switch ( hdr1->code )
  {
    case TVN_BEGINDRAG:
    {
      eventType = wxEVT_COMMAND_TREE_BEGIN_DRAG;
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());
      event.m_pointDrag.x = hdr->ptDrag.x;
      event.m_pointDrag.y = hdr->ptDrag.y;
      break;
    }
    case TVN_BEGINLABELEDIT:
    {
      eventType = wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT;
      TV_DISPINFO *info = (TV_DISPINFO *)lParam;
      wxConvertFromMSWTreeItem(event.m_item, info->item, (HWND) GetHWND());
      break;
    }
    case TVN_BEGINRDRAG:
    {
      eventType = wxEVT_COMMAND_TREE_BEGIN_RDRAG;
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());
      event.m_pointDrag.x = hdr->ptDrag.x;
      event.m_pointDrag.y = hdr->ptDrag.y;
      break;
    }
    case TVN_DELETEITEM:
    {
      eventType = wxEVT_COMMAND_TREE_DELETE_ITEM;
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemOld, (HWND) GetHWND());
      event.m_pointDrag.x = hdr->ptDrag.x;
      event.m_pointDrag.y = hdr->ptDrag.y;
      break;
    }
    case TVN_ENDLABELEDIT:
    {
      eventType = wxEVT_COMMAND_TREE_END_LABEL_EDIT;
      TV_DISPINFO *info = (TV_DISPINFO *)lParam;
      wxConvertFromMSWTreeItem(event.m_item, info->item, (HWND) GetHWND());
      break;
    }
    case TVN_GETDISPINFO:
    {
      eventType = wxEVT_COMMAND_TREE_GET_INFO;
      TV_DISPINFO *info = (TV_DISPINFO *)lParam;
      wxConvertFromMSWTreeItem(event.m_item, info->item, (HWND) GetHWND());
      break;
    }
    case TVN_ITEMEXPANDING:
    {
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());

      switch ( hdr->action )
      {
        case TVE_EXPAND:
          eventType = wxEVT_COMMAND_TREE_ITEM_EXPANDING;
          break;

        case TVE_COLLAPSE:
          eventType = wxEVT_COMMAND_TREE_ITEM_COLLAPSING;
          break;

        case TVE_COLLAPSERESET:
        case TVE_TOGGLE:
          wxLogDebug("unexpected code in TVN_ITEMEXPANDING message");
          break;
      }
      break;
    }

    case TVN_ITEMEXPANDED:
    {
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());
      switch ( hdr->action )
      {
        case TVE_EXPAND:
          eventType = wxEVT_COMMAND_TREE_ITEM_EXPANDED;
          break;

        case TVE_COLLAPSE:
          eventType = wxEVT_COMMAND_TREE_ITEM_COLLAPSED;
          break;

        case TVE_COLLAPSERESET:
        case TVE_TOGGLE:
          wxLogDebug("unexpected code in TVN_ITEMEXPANDED message");
          break;
      }
      break;
    }
    case TVN_KEYDOWN:
    {
      eventType = wxEVT_COMMAND_TREE_KEY_DOWN;
      TV_KEYDOWN *info = (TV_KEYDOWN *)lParam;
      event.m_code = wxCharCodeMSWToWX(info->wVKey);
      break;
    }
    case TVN_SELCHANGED:
    {
      eventType = wxEVT_COMMAND_TREE_SEL_CHANGED;
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());
      event.m_oldItem = (long) hdr->itemNew.hItem;

      break;
    }
    case TVN_SELCHANGING:
    {
      eventType = wxEVT_COMMAND_TREE_SEL_CHANGING;
      NM_TREEVIEW* hdr = (NM_TREEVIEW*)lParam;
      wxConvertFromMSWTreeItem(event.m_item, hdr->itemNew, (HWND) GetHWND());
      event.m_oldItem = (long) hdr->itemNew.hItem;
      break;
    }
    case TVN_SETDISPINFO:
    {
      eventType = wxEVT_COMMAND_TREE_SET_INFO;
      TV_DISPINFO *info = (TV_DISPINFO *)lParam;
      wxConvertFromMSWTreeItem(event.m_item, info->item, (HWND) GetHWND());
      break;
    }

    default :
      return wxControl::MSWNotify(wParam, lParam);
      break;
  }

  event.SetEventObject(this);
  event.SetEventType(eventType);

  if ( !GetEventHandler()->ProcessEvent(event) )
    return FALSE;

  if (hdr1->code == TVN_GETDISPINFO)
  {
    TV_DISPINFO *info = (TV_DISPINFO *)lParam;
    wxConvertToMSWTreeItem(event.m_item, info->item);
  }

  return TRUE;
}

// Tree item structure
wxTreeItem::wxTreeItem(void)
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
static void wxConvertFromMSWTreeItem(wxTreeItem& info, TV_ITEM& tvItem, HWND getFullInfo)
{
  info.m_data = tvItem.lParam;
  info.m_mask = 0;
  info.m_state = 0;
  info.m_stateMask = 0;

  long oldMask = tvItem.mask;

  bool needText = FALSE;
  if (getFullInfo != 0)
  {
    if ( tvItem.mask & TVIF_TEXT )
      needText = FALSE;
    else
      needText = TRUE;

    if ( needText )
    {
      tvItem.pszText = new char[513];
      tvItem.cchTextMax = 512;
    }
    tvItem.mask |= TVIF_HANDLE | TVIF_STATE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM ;
    ::SendMessage(getFullInfo, TVM_GETITEM, 0, (LPARAM)& tvItem) ;
  }

  if ( tvItem.mask & TVIF_HANDLE )
  {
    info.m_mask |= wxTREE_MASK_HANDLE;
    info.m_itemId = (long) tvItem.hItem;
  }
  if ( tvItem.mask & TVIF_STATE )
  {
    info.m_mask |= wxTREE_MASK_STATE;
      if ( tvItem.stateMask & TVIS_BOLD)
    {
      info.m_stateMask |= wxTREE_STATE_BOLD ;
      if ( tvItem.state & TVIS_BOLD )
        info.m_state |= wxTREE_STATE_BOLD ;
    }

      if ( tvItem.stateMask & TVIS_CUT)
    {
      info.m_stateMask |= wxTREE_STATE_CUT ;
      if ( tvItem.state & TVIS_CUT )
        info.m_state |= wxTREE_STATE_CUT ;
    }

      if ( tvItem.stateMask & TVIS_DROPHILITED)
    {
      info.m_stateMask |= wxTREE_STATE_DROPHILITED ;
      if ( tvItem.state & TVIS_DROPHILITED )
        info.m_state |= wxTREE_STATE_DROPHILITED ;
    }
      if ( tvItem.stateMask & TVIS_EXPANDED)
    {
      info.m_stateMask |= wxTREE_STATE_EXPANDED ;
      if ( tvItem.state & TVIS_EXPANDED )
        info.m_state |= wxTREE_STATE_EXPANDED ;
    }
      if ( tvItem.stateMask & TVIS_EXPANDEDONCE)
    {
      info.m_stateMask |= wxTREE_STATE_EXPANDEDONCE ;
      if ( tvItem.state & TVIS_EXPANDEDONCE )
        info.m_state |= wxTREE_STATE_EXPANDEDONCE ;
    }
      if ( tvItem.stateMask & TVIS_FOCUSED)
    {
      info.m_stateMask |= wxTREE_STATE_FOCUSED ;
      if ( tvItem.state & TVIS_FOCUSED )
        info.m_state |= wxTREE_STATE_FOCUSED ;
    }
      if ( tvItem.stateMask & TVIS_SELECTED)
    {
      info.m_stateMask |= wxTREE_STATE_SELECTED ;
      if ( tvItem.state & TVIS_SELECTED )
        info.m_state |= wxTREE_STATE_SELECTED ;
    }
  }

  if ( tvItem.mask & TVIF_TEXT )
  {
    info.m_mask |= wxTREE_MASK_TEXT;
    info.m_text = tvItem.pszText;
  }
  if ( tvItem.mask & TVIF_IMAGE )
  {
    info.m_mask |= wxTREE_MASK_IMAGE;
    info.m_image = tvItem.iImage;
  }
  if ( tvItem.mask & TVIF_SELECTEDIMAGE )
  {
    info.m_mask |= wxTREE_MASK_SELECTED_IMAGE;
    info.m_selectedImage = tvItem.iSelectedImage;
  }
  if ( tvItem.mask & TVIF_CHILDREN )
  {
    info.m_mask |= wxTREE_MASK_CHILDREN;
    info.m_children = tvItem.cChildren;
  }
  if ( tvItem.mask & TVIF_PARAM )
    info.m_mask |= wxTREE_MASK_DATA;

  if (needText)
  {
        if (tvItem.pszText)
      delete[] tvItem.pszText;
  }
  tvItem.mask = oldMask ;
}

static void wxConvertToMSWTreeItem(wxTreeItem& info, TV_ITEM& tvItem)
{
  tvItem.hItem = (HTREEITEM) info.m_itemId ;

  tvItem.iImage = info.m_image ;
  tvItem.iSelectedImage = info.m_selectedImage;
  tvItem.cChildren = info.m_children;
  tvItem.lParam = info.m_data;
  tvItem.mask = 0;
  tvItem.stateMask = 0;
  tvItem.state = 0;

  if (info.m_mask & wxTREE_MASK_HANDLE)
    tvItem.mask |= TVIF_HANDLE ;
  if (info.m_mask & wxTREE_MASK_STATE)
    tvItem.mask |= TVIF_STATE ;
  if (info.m_mask & wxTREE_MASK_TEXT)
  {
    tvItem.mask |= TVIF_TEXT ;
    tvItem.pszText = (char *) (const char *)info.m_text ;
    if ( tvItem.pszText )
      tvItem.cchTextMax = info.m_text.Length();
    else
      tvItem.cchTextMax = 0;
  }
  if (info.m_mask & wxTREE_MASK_IMAGE)
    tvItem.mask |= TVIF_IMAGE ;
  if (info.m_mask & wxTREE_MASK_SELECTED_IMAGE)
    tvItem.mask |= TVIF_SELECTEDIMAGE ;
  if (info.m_mask & wxTREE_MASK_CHILDREN)
    tvItem.mask |= TVIF_CHILDREN ;
  if (info.m_mask & wxTREE_MASK_DATA)
    tvItem.mask |= TVIF_PARAM ;

  if (info.m_stateMask & wxTREE_STATE_BOLD)
  {
      tvItem.stateMask |= TVIS_BOLD ;
    tvItem.state |= TVIS_BOLD;
  }
  if (info.m_stateMask & wxTREE_STATE_CUT)
  {
      tvItem.stateMask |= TVIS_CUT ;
    if ( info.m_state & wxTREE_STATE_CUT )
      tvItem.state |= TVIS_CUT;
  }
  if (info.m_stateMask & wxTREE_STATE_DROPHILITED)
  {
      tvItem.stateMask |= TVIS_DROPHILITED;
    if ( info.m_state & wxTREE_STATE_DROPHILITED )
        tvItem.state |= TVIS_DROPHILITED;
  }
  if (info.m_stateMask & wxTREE_STATE_EXPANDED)
  {
      tvItem.stateMask |= TVIS_EXPANDED;
    if ( info.m_state & wxTREE_STATE_EXPANDED )
        tvItem.state |= TVIS_EXPANDED;
  }
  if (info.m_stateMask & wxTREE_STATE_EXPANDEDONCE)
  {
      tvItem.stateMask |= TVIS_EXPANDEDONCE;
    if ( info.m_state & wxTREE_STATE_EXPANDEDONCE )
        tvItem.state |= TVIS_EXPANDEDONCE;
  }
  if (info.m_stateMask & wxTREE_STATE_FOCUSED)
  {
      tvItem.stateMask |= TVIS_FOCUSED;
    if ( info.m_state & wxTREE_STATE_FOCUSED )
        tvItem.state |= TVIS_FOCUSED;
  }
  if (info.m_stateMask & wxTREE_STATE_SELECTED)
  {
      tvItem.stateMask |= TVIS_SELECTED;
    if ( info.m_state & wxTREE_STATE_SELECTED )
        tvItem.state |= TVIS_SELECTED;
  }
}

// Tree event
IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxCommandEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id)
{
  m_code = 0;
  m_oldItem = 0;
}

#endif

