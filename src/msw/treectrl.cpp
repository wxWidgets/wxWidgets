/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.cpp
// Purpose:     wxTreeCtrl
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to be less MSW-specific on 10.10.98
// Created:     1997
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
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

#if defined(__WIN95__)

#include "wx/log.h"
#include "wx/dynarray.h"
#include "wx/imaglist.h"
#include "wx/msw/treectrl.h"

#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__)
    #include <commctrl.h>
#endif

#ifdef GetFirstChild
#undef GetFirstChild
#endif

#ifdef GetNextChild
#undef GetNextChild
#endif

#ifdef GetNextSibling
#undef GetNextSibling
#endif

#ifdef GetClassInfo
#undef GetClassInfo
#endif

// Bug in headers, sometimes
#ifndef TVIS_FOCUSED
    #define TVIS_FOCUSED            0x0001
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// a convenient wrapper around TV_ITEM struct which adds a ctor
struct wxTreeViewItem : public TV_ITEM
{
    wxTreeViewItem(const wxTreeItemId& item,
                   UINT mask_, UINT stateMask_ = 0)
    {
        mask = mask_;
        stateMask = stateMask_;
        hItem = (HTREEITEM) (WXHTREEITEM) item;
    }
};

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxControl)
#endif

// hide the ugly cast (of course, the macro is _quite_ ugly too...)
#define wxhWnd    ((HWND)m_hWnd)

// ----------------------------------------------------------------------------
// variables
// ----------------------------------------------------------------------------

// handy table for sending events
static const wxEventType g_events[2][2] =
{
    { wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxEVT_COMMAND_TREE_ITEM_COLLAPSING },
    { wxEVT_COMMAND_TREE_ITEM_EXPANDED,  wxEVT_COMMAND_TREE_ITEM_EXPANDING  }
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction and destruction
// ----------------------------------------------------------------------------

void wxTreeCtrl::Init()
{
    m_imageListNormal = NULL;
    m_imageListState = NULL;
    m_textCtrl = NULL;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxValidator& validator,
                        const wxString& name)
{
    Init();

    wxSystemSettings settings;

    SetName(name);
    SetValidator(validator);

    m_windowStyle = style;

    SetParent(parent);

    m_windowId = (id == -1) ? NewControlId() : id;

    DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | TVS_HASLINES;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    if ( want3D || wxStyleHasBorder(m_windowStyle) )
    {
        wstyle |= WS_BORDER;
    }

    if ( m_windowStyle & wxTR_HAS_BUTTONS )
        wstyle |= TVS_HASBUTTONS;

    if ( m_windowStyle & wxTR_EDIT_LABELS )
        wstyle |= TVS_EDITLABELS;

    if ( m_windowStyle & wxTR_LINES_AT_ROOT )
        wstyle |= TVS_LINESATROOT;

    // Create the tree control.
    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                        exStyle,
                        WC_TREEVIEW,
                        "",
                        wstyle,
                        pos.x, pos.y, size.x, size.y,
                        (HWND)parent->GetHWND(),
                        (HMENU)m_windowId,
                        wxGetInstance(),
                        NULL
                       );

    wxCHECK_MSG( m_hWnd, FALSE, "Failed to create tree ctrl" );

    if ( parent )
        parent->AddChild(this);

    SubclassWin(m_hWnd);

    return TRUE;
}

wxTreeCtrl::~wxTreeCtrl()
{
    DeleteTextCtrl();

    // delete user data to prevent memory leaks
    DeleteAllItems();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// simple wrappers which add error checking in debug mode

bool wxTreeCtrl::DoGetItem(wxTreeViewItem* tvItem) const
{
    if ( !TreeView_GetItem(wxhWnd, tvItem) )
    {
        wxLogLastError("TreeView_GetItem");

        return FALSE;
    }

    return TRUE;
}

void wxTreeCtrl::DoSetItem(wxTreeViewItem* tvItem)
{
    if ( TreeView_SetItem(wxhWnd, tvItem) == -1 )
    {
        wxLogLastError("TreeView_SetItem");
    }
}

size_t wxTreeCtrl::GetCount() const
{
    return (size_t)TreeView_GetCount(wxhWnd);
}

unsigned int wxTreeCtrl::GetIndent() const
{
    return TreeView_GetIndent(wxhWnd);
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
    TreeView_SetIndent(wxhWnd, indent);
}

wxImageList *wxTreeCtrl::GetImageList() const
{
    return m_imageListNormal;
}

wxImageList *wxTreeCtrl::GetStateImageList() const
{
    return m_imageListNormal;
}

void wxTreeCtrl::SetAnyImageList(wxImageList *imageList, int which)
{
    // no error return
    TreeView_SetImageList(wxhWnd,
                          imageList ? imageList->GetHIMAGELIST() : 0,
                          which);
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
    SetAnyImageList(m_imageListNormal = imageList, TVSIL_NORMAL);
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    SetAnyImageList(m_imageListState = imageList, TVSIL_STATE);
}

size_t wxTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively)
{
    long cookie;

    size_t result = 0;

    wxArrayLong children;
    wxTreeItemId child = GetFirstChild(item, cookie);
    while ( child.IsOk() )
    {
        if ( recursively )
        {
            // recursive call
            result += GetChildrenCount(child, TRUE);
        }

        // add the child to the result in any case
        result++;

        child = GetNextChild(item, cookie);
    }

    return result;
}

// ----------------------------------------------------------------------------
// Item access
// ----------------------------------------------------------------------------

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    char buf[512];  // the size is arbitrary...

    wxTreeViewItem tvItem(item, TVIF_TEXT);
    tvItem.pszText = buf;
    tvItem.cchTextMax = WXSIZEOF(buf);
    if ( !DoGetItem(&tvItem) )
    {
        // don't return some garbage which was on stack, but an empty string
        buf[0] = '\0';
    }

    return wxString(buf);
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxTreeViewItem tvItem(item, TVIF_TEXT);
    tvItem.pszText = (char *)text.c_str();  // conversion is ok
    DoSetItem(&tvItem);
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_IMAGE);
    DoGetItem(&tvItem);

    return tvItem.iImage;
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item, int image)
{
    wxTreeViewItem tvItem(item, TVIF_IMAGE);
    tvItem.iImage = image;
    DoSetItem(&tvItem);
}

int wxTreeCtrl::GetItemSelectedImage(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_SELECTEDIMAGE);
    DoGetItem(&tvItem);

    return tvItem.iSelectedImage;
}

void wxTreeCtrl::SetItemSelectedImage(const wxTreeItemId& item, int image)
{
    wxTreeViewItem tvItem(item, TVIF_SELECTEDIMAGE);
    tvItem.iSelectedImage = image;
    DoSetItem(&tvItem);
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_PARAM);
    if ( !DoGetItem(&tvItem) )
    {
        return NULL;
    }

    return (wxTreeItemData *)tvItem.lParam;
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxTreeViewItem tvItem(item, TVIF_PARAM);
    tvItem.lParam = (LPARAM)data;
    DoSetItem(&tvItem);
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxTreeViewItem tvItem(item, TVIF_CHILDREN);
    tvItem.cChildren = (int)has;
    DoSetItem(&tvItem);
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxTreeViewItem tvItem(item, TVIF_STATE, TVIS_BOLD);
    tvItem.state = bold ? TVIS_BOLD : 0;
    DoSetItem(&tvItem);
}

// ----------------------------------------------------------------------------
// Item status
// ----------------------------------------------------------------------------

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    // Bug in Gnu-Win32 headers, so don't use the macro TreeView_GetItemRect
    RECT rect;
    return SendMessage(wxhWnd, TVM_GETITEMRECT, FALSE, (LPARAM)&rect) != 0;

}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_CHILDREN);
    DoGetItem(&tvItem);

    return tvItem.cChildren != 0;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    // probably not a good idea to put it here
    //wxASSERT( ItemHasChildren(item) );

    wxTreeViewItem tvItem(item, TVIF_STATE, TVIS_EXPANDED);
    DoGetItem(&tvItem);

    return (tvItem.state & TVIS_EXPANDED) != 0;
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_STATE, TVIS_SELECTED);
    DoGetItem(&tvItem);

    return (tvItem.state & TVIS_SELECTED) != 0;
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxTreeViewItem tvItem(item, TVIF_STATE, TVIS_BOLD);
    DoGetItem(&tvItem);

    return (tvItem.state & TVIS_BOLD) != 0;
}

// ----------------------------------------------------------------------------
// navigation
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetRoot(wxhWnd));
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetSelection(wxhWnd));
}

wxTreeItemId wxTreeCtrl::GetParent(const wxTreeItemId& item) const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetParent(wxhWnd, (HTREEITEM) (WXHTREEITEM) item));
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item,
                                       long& _cookie) const
{
    // remember the last child returned in 'cookie'
    _cookie = (long)TreeView_GetChild(wxhWnd, (HTREEITEM) (WXHTREEITEM)item);

    return wxTreeItemId((WXHTREEITEM)_cookie);
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& WXUNUSED(item),
                                      long& _cookie) const
{
    wxTreeItemId l = wxTreeItemId((WXHTREEITEM)TreeView_GetNextSibling(wxhWnd,
                                   (HTREEITEM)(WXHTREEITEM)_cookie));
    _cookie = (long)l;

    return l;
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    // can this be done more efficiently?
    long cookie;

    wxTreeItemId childLast,
    child = GetFirstChild(item, cookie);
    while ( child.IsOk() )
    {
        childLast = child;
        child = GetNextChild(item, cookie);
    }

    return childLast;
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetNextSibling(wxhWnd, (HTREEITEM) (WXHTREEITEM) item));
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetPrevSibling(wxhWnd, (HTREEITEM) (WXHTREEITEM) item));
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
    return wxTreeItemId((WXHTREEITEM) TreeView_GetFirstVisible(wxhWnd));
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxASSERT_MSG( IsVisible(item), "The item you call GetNextVisible() "
                                   "for must be visible itself!");

    return wxTreeItemId((WXHTREEITEM) TreeView_GetNextVisible(wxhWnd, (HTREEITEM) (WXHTREEITEM) item));
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxASSERT_MSG( IsVisible(item), "The item you call GetPrevVisible() "
                                   "for must be visible itself!");

    return wxTreeItemId((WXHTREEITEM) TreeView_GetPrevVisible(wxhWnd, (HTREEITEM) (WXHTREEITEM) item));
}

// ----------------------------------------------------------------------------
// Usual operations
// ----------------------------------------------------------------------------

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                      wxTreeItemId hInsertAfter,
                                      const wxString& text,
                                      int image, int selectedImage,
                                      wxTreeItemData *data)
{
    TV_INSERTSTRUCT tvIns;
    tvIns.hParent = (HTREEITEM) (WXHTREEITEM)parent;
    tvIns.hInsertAfter = (HTREEITEM) (WXHTREEITEM) hInsertAfter;
    UINT mask = 0;
    if ( !text.IsEmpty() )
    {
        mask |= TVIF_TEXT;
        tvIns.item.pszText = (char *)text.c_str();  // cast is ok
    }

    if ( image != -1 )
    {
        mask |= TVIF_IMAGE;
        tvIns.item.iImage = image;

        if ( selectedImage == -1 )
        {
            // take the same image for selected icon if not specified
            selectedImage = image;
        }
    }

    if ( selectedImage != -1 )
    {
        mask |= TVIF_SELECTEDIMAGE;
        tvIns.item.iSelectedImage = selectedImage;
    }

    if ( data != NULL )
    {
        mask |= TVIF_PARAM;
        tvIns.item.lParam = (LPARAM)data;
    }

    tvIns.item.mask = mask;

    HTREEITEM id = (HTREEITEM) TreeView_InsertItem(wxhWnd, &tvIns);
    if ( id == 0 )
    {
        wxLogLastError("TreeView_InsertItem");
    }

    if ( data != NULL )
    {
        // associate the application tree item with Win32 tree item handle
        data->SetId((WXHTREEITEM)id);
    }

    return wxTreeItemId((WXHTREEITEM)id);
}

// for compatibility only
wxTreeItemId wxTreeCtrl::InsertItem(const wxTreeItemId& parent,
                                    const wxString& text,
                                    int image, int selImage,
                                    long insertAfter)
{
    return DoInsertItem(parent, (WXHTREEITEM)insertAfter, text,
                        image, selImage, NULL);
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                                 int image, int selectedImage,
                                 wxTreeItemData *data)
{
    return DoInsertItem(wxTreeItemId((WXHTREEITEM) 0), (WXHTREEITEM) 0,
                        text, image, selectedImage, data);
}

wxTreeItemId wxTreeCtrl::PrependItem(const wxTreeItemId& parent,
                                     const wxString& text,
                                     int image, int selectedImage,
                                     wxTreeItemData *data)
{
    return DoInsertItem(parent, (WXHTREEITEM) TVI_FIRST,
                        text, image, selectedImage, data);
}

wxTreeItemId wxTreeCtrl::InsertItem(const wxTreeItemId& parent,
                                    const wxTreeItemId& idPrevious,
                                    const wxString& text,
                                    int image, int selectedImage,
                                    wxTreeItemData *data)
{
    return DoInsertItem(parent, idPrevious, text, image, selectedImage, data);
}

wxTreeItemId wxTreeCtrl::AppendItem(const wxTreeItemId& parent,
                                    const wxString& text,
                                    int image, int selectedImage,
                                    wxTreeItemData *data)
{
    return DoInsertItem(parent, (WXHTREEITEM) TVI_LAST,
                        text, image, selectedImage, data);
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    if ( !TreeView_DeleteItem(wxhWnd, (HTREEITEM)(WXHTREEITEM)item) )
    {
        wxLogLastError("TreeView_DeleteItem");
    }
}

// delete all children (but don't delete the item itself)
void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    long cookie;

    wxArrayLong children;
    wxTreeItemId child = GetFirstChild(item, cookie);
    while ( child.IsOk() )
    {
        children.Add((long)(WXHTREEITEM)child);

        child = GetNextChild(item, cookie);
    }

    size_t nCount = children.Count();
    for ( size_t n = 0; n < nCount; n++ )
    {
        if ( !TreeView_DeleteItem(wxhWnd, (HTREEITEM)children[n]) )
        {
            wxLogLastError("TreeView_DeleteItem");
        }
    }
}

void wxTreeCtrl::DeleteAllItems()
{
    if ( !TreeView_DeleteAllItems(wxhWnd) )
    {
        wxLogLastError("TreeView_DeleteAllItems");
    }
}

void wxTreeCtrl::DoExpand(const wxTreeItemId& item, int flag)
{
    wxASSERT_MSG( flag == TVE_COLLAPSE || flag == TVE_COLLAPSERESET ||
                  flag == TVE_EXPAND   || flag == TVE_TOGGLE,
                  "Unknown flag in wxTreeCtrl::DoExpand" );

    // TreeView_Expand doesn't send TVN_ITEMEXPAND(ING) messages, so we must
    // emulate them
    if ( TreeView_Expand(wxhWnd, (HTREEITEM) (WXHTREEITEM) item, flag) != 0 )
    {
        wxTreeEvent event(wxEVT_NULL, m_windowId);
        event.m_item = item;

        bool isExpanded = IsExpanded(item);

        event.SetEventObject(this);

        // @@@ return values of {EXPAND|COLLAPS}ING event handler is discarded
        event.SetEventType(g_events[isExpanded][TRUE]);
        GetEventHandler()->ProcessEvent(event);

        event.SetEventType(g_events[isExpanded][FALSE]);
        GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        // I wonder if it really ever happens...
        wxLogDebug("TreeView_Expand: change didn't took place.");
    }
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    DoExpand(item, TVE_EXPAND);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    DoExpand(item, TVE_COLLAPSE);
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    DoExpand(item, TVE_COLLAPSERESET);
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item)
{
    DoExpand(item, TVE_TOGGLE);
}

void wxTreeCtrl::ExpandItem(const wxTreeItemId& item, int action)
{
	DoExpand(item, action);
}

void wxTreeCtrl::Unselect()
{
    SelectItem(wxTreeItemId((WXHTREEITEM) 0));
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item)
{
    if ( !TreeView_SelectItem(wxhWnd, (HTREEITEM) (WXHTREEITEM) item) )
    {
        wxLogLastError("TreeView_SelectItem");
    }
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    // no error return
    TreeView_EnsureVisible(wxhWnd, (HTREEITEM) (WXHTREEITEM) item);
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    if ( !TreeView_SelectSetFirstVisible(wxhWnd, (HTREEITEM) (WXHTREEITEM) item) )
    {
        wxLogLastError("TreeView_SelectSetFirstVisible");
    }
}

wxTextCtrl* wxTreeCtrl::GetEditControl() const
{
    return m_textCtrl;
}

void wxTreeCtrl::DeleteTextCtrl()
{
    if ( m_textCtrl )
    {
        m_textCtrl->UnsubclassWin();
        m_textCtrl->SetHWND(0);
        delete m_textCtrl;
        m_textCtrl = NULL;
    }
}

wxTextCtrl* wxTreeCtrl::EditLabel(const wxTreeItemId& item,
                                  wxClassInfo* textControlClass)
{
    wxASSERT( textControlClass->IsKindOf(CLASSINFO(wxTextCtrl)) );

    HWND hWnd = (HWND) TreeView_EditLabel(wxhWnd, (HTREEITEM) (WXHTREEITEM) item);

    wxCHECK_MSG( hWnd, NULL, "Can't edit tree ctrl label" );

    DeleteTextCtrl();

    m_textCtrl = (wxTextCtrl *)textControlClass->CreateObject();
    m_textCtrl->SetHWND((WXHWND)hWnd);
    m_textCtrl->SubclassWin((WXHWND)hWnd);

    return m_textCtrl;
}

// End label editing, optionally cancelling the edit
void wxTreeCtrl::EndEditLabel(const wxTreeItemId& item, bool discardChanges)
{
    TreeView_EndEditLabelNow(wxhWnd, discardChanges);

    DeleteTextCtrl();
}

wxTreeItemId wxTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
    TV_HITTESTINFO hitTestInfo;
    hitTestInfo.pt.x = (int)point.x;
    hitTestInfo.pt.y = (int)point.y;

    TreeView_HitTest(wxhWnd, &hitTestInfo);

    flags = 0;

    // avoid repetition
    #define TRANSLATE_FLAG(flag) if ( hitTestInfo.flags & TVHT_##flag ) \
                                    flags |= wxTREE_HITTEST_##flag

    TRANSLATE_FLAG(ABOVE);
    TRANSLATE_FLAG(BELOW);
    TRANSLATE_FLAG(NOWHERE);
    TRANSLATE_FLAG(ONITEMBUTTON);
    TRANSLATE_FLAG(ONITEMICON);
    TRANSLATE_FLAG(ONITEMINDENT);
    TRANSLATE_FLAG(ONITEMLABEL);
    TRANSLATE_FLAG(ONITEMRIGHT);
    TRANSLATE_FLAG(ONITEMSTATEICON);
    TRANSLATE_FLAG(TOLEFT);
    TRANSLATE_FLAG(TORIGHT);

    #undef TRANSLATE_FLAG

    return wxTreeItemId((WXHTREEITEM) hitTestInfo.hItem);
}

// ----------------------------------------------------------------------------
// sorting stuff
// ----------------------------------------------------------------------------
static int CALLBACK TreeView_CompareCallback(wxTreeItemData *pItem1,
                                             wxTreeItemData *pItem2,
                                             wxTreeCtrl *tree)
{
    return tree->OnCompareItems(pItem1->GetId(), pItem2->GetId());
}

int wxTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    return strcmp(GetItemText(item1), GetItemText(item2));
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    // rely on the fact that TreeView_SortChildren does the same thing as our
    // default behaviour, i.e. sorts items alphabetically and so call it
    // directly if we're not in derived class (much more efficient!)
    if ( GetClassInfo() == CLASSINFO(wxTreeCtrl) )
    {
        TreeView_SortChildren(wxhWnd, (HTREEITEM)(WXHTREEITEM)item, 0);
    }
    else
    {
        TV_SORTCB tvSort;
        tvSort.hParent = (HTREEITEM)(WXHTREEITEM)item;
        tvSort.lpfnCompare = (PFNTVCOMPARE)TreeView_CompareCallback;
        tvSort.lParam = (LPARAM)this;
        TreeView_SortChildrenCB(wxhWnd, &tvSort, 0 /* reserved */);
    }
}

// ----------------------------------------------------------------------------
// implementation
// ----------------------------------------------------------------------------

bool wxTreeCtrl::MSWCommand(WXUINT cmd, WXWORD id)
{
    if ( cmd == EN_UPDATE )
    {
        wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, id);
        event.SetEventObject( this );
        ProcessCommand(event);
    }
    else if ( cmd == EN_KILLFOCUS )
    {
        wxCommandEvent event(wxEVT_KILL_FOCUS, id);
        event.SetEventObject( this );
        ProcessCommand(event);
    }
    else
    {
        // nothing done
        return FALSE;
    }

    // command processed
    return TRUE;
}

// process WM_NOTIFY Windows message
bool wxTreeCtrl::MSWNotify(WXWPARAM wParam, WXLPARAM lParam, WXLPARAM *result)
{
    wxTreeEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;
    NMHDR *hdr = (NMHDR *)lParam;

    switch ( hdr->code )
    {
        case TVN_BEGINDRAG:
            eventType = wxEVT_COMMAND_TREE_BEGIN_DRAG;
            // fall through

        case TVN_BEGINRDRAG:
            {
                if ( eventType == wxEVT_NULL )
                    eventType = wxEVT_COMMAND_TREE_BEGIN_RDRAG;
                //else: left drag, already set above

                NM_TREEVIEW *tv = (NM_TREEVIEW *)lParam;

                event.m_item = (WXHTREEITEM) tv->itemNew.hItem;
                event.m_pointDrag = wxPoint(tv->ptDrag.x, tv->ptDrag.y);
                break;
            }

        case TVN_BEGINLABELEDIT:
            {
                eventType = wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT;
                TV_DISPINFO *info = (TV_DISPINFO *)lParam;

                event.m_item = (WXHTREEITEM) info->item.hItem;
                break;
            }

        case TVN_DELETEITEM:
            {
                eventType = wxEVT_COMMAND_TREE_DELETE_ITEM;
                NM_TREEVIEW *tv = (NM_TREEVIEW *)lParam;

                event.m_item = (WXHTREEITEM) tv->itemOld.hItem;
                break;
            }

        case TVN_ENDLABELEDIT:
            {
                eventType = wxEVT_COMMAND_TREE_END_LABEL_EDIT;
                TV_DISPINFO *info = (TV_DISPINFO *)lParam;

                event.m_item = (WXHTREEITEM) info->item.hItem;
                break;
            }

        case TVN_GETDISPINFO:
            eventType = wxEVT_COMMAND_TREE_GET_INFO;
            // fall through

        case TVN_SETDISPINFO:
            {
                if ( eventType == wxEVT_NULL )
                    eventType = wxEVT_COMMAND_TREE_SET_INFO;
                //else: get, already set above

                TV_DISPINFO *info = (TV_DISPINFO *)lParam;

                event.m_item = (WXHTREEITEM) info->item.hItem;
                break;
            }

        case TVN_ITEMEXPANDING:
            event.m_code = FALSE;
            // fall through

        case TVN_ITEMEXPANDED:
            {
                NM_TREEVIEW* tv = (NM_TREEVIEW*)lParam;

                bool expand = FALSE;
                switch ( tv->action )
                {
                    case TVE_EXPAND:
                        expand = TRUE;
                        break;

                    case TVE_COLLAPSE:
                        expand = FALSE;
                        break;

                    default:
                        wxLogDebug("unexpected code %d in TVN_ITEMEXPAND "
                                   "message", tv->action);
                }

                bool ing = (hdr->code == TVN_ITEMEXPANDING);
                eventType = g_events[expand][ing];

                event.m_item = (WXHTREEITEM) tv->itemNew.hItem;
                break;
            }

        case TVN_KEYDOWN:
            {
                eventType = wxEVT_COMMAND_TREE_KEY_DOWN;
                TV_KEYDOWN *info = (TV_KEYDOWN *)lParam;

                event.m_code = wxCharCodeMSWToWX(info->wVKey);

                // a separate event for this case
                if ( info->wVKey == VK_SPACE || info->wVKey == VK_RETURN )
                {
                    wxTreeEvent event2(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,
                                       m_windowId);
                    event2.SetEventObject(this);

                    GetEventHandler()->ProcessEvent(event2);
                }
                break;
            }

        case TVN_SELCHANGED:
            eventType = wxEVT_COMMAND_TREE_SEL_CHANGED;
            // fall through

        case TVN_SELCHANGING:
            {
                if ( eventType == wxEVT_NULL )
                    eventType = wxEVT_COMMAND_TREE_SEL_CHANGING;
                //else: already set above

                NM_TREEVIEW* tv = (NM_TREEVIEW *)lParam;

                event.m_item = (WXHTREEITEM) tv->itemNew.hItem;
                event.m_itemOld = (WXHTREEITEM) tv->itemOld.hItem;
                break;
            }

        default:
            return wxControl::MSWNotify(wParam, lParam, result);
    }

    event.SetEventObject(this);
    event.SetEventType(eventType);

    bool processed = GetEventHandler()->ProcessEvent(event);

    // post processing
    if ( hdr->code == TVN_DELETEITEM )
    {
        // NB: we might process this message using wxWindows event tables, but
        //     due to overhead of wxWin event system we prefer to do it here
        //     (otherwise deleting a tree with many items is just too slow)
        NM_TREEVIEW* tv = (NM_TREEVIEW *)lParam;
        wxTreeItemData *data = (wxTreeItemData *)tv->itemOld.lParam;
        delete data; // may be NULL, ok
    }

    *result = !event.IsAllowed();

    return processed;
}

// ----------------------------------------------------------------------------
// Tree event
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxNotifyEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id)
           : wxNotifyEvent(commandType, id)
{
    m_code = 0;
    m_itemOld = 0;
}

#endif // __WIN95__

