/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/treectrl.cpp
// Purpose:     wxTreeCtrl. See also Robert's generic wxTreeCtrl.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/stubs/textctrl.h"
#include "wx/treebase.h"
#include "wx/stubs/treectrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTreeItem, wxObject)

wxTreeCtrl::wxTreeCtrl()
{
    m_imageListNormal = NULL;
    m_imageListState = NULL;
    m_textCtrl = NULL;
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxValidator& validator, const wxString& name)
{
    SetName(name);
    SetValidator(validator);

    m_imageListNormal = NULL;
    m_imageListState = NULL;
    m_textCtrl = NULL;

    m_windowStyle = style;

    SetParent(parent);

    m_windowId = (id == wxID_ANY) ? NewControlId() : id;

    if (parent) parent->AddChild(this);

    // TODO create tree control

    return false;
}

wxTreeCtrl::~wxTreeCtrl()
{
    if (m_textCtrl)
    {
        delete m_textCtrl;
    }
}

// Attributes
unsigned int wxTreeCtrl::GetCount() const
{
    // TODO
    return 0;
}

int wxTreeCtrl::GetIndent() const
{
    // TODO
    return 0;
}

void wxTreeCtrl::SetIndent(int indent)
{
    // TODO
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
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        m_imageListNormal = imageList;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        m_imageListState = imageList;
    }
    // TODO
}

long wxTreeCtrl::GetNextItem(long item, int code) const
{
    // TODO
    return 0;
}

bool wxTreeCtrl::ItemHasChildren(long item) const
{
    // TODO
    return false;
}

long wxTreeCtrl::GetChild(long item) const
{
    // TODO
    return 0;
}

long wxTreeCtrl::GetItemParent(long item) const
{
    // TODO
    return 0;
}

long wxTreeCtrl::GetFirstVisibleItem() const
{
    // TODO
    return 0;
}

long wxTreeCtrl::GetNextVisibleItem(long item) const
{
    // TODO
    return 0;
}

long wxTreeCtrl::GetSelection() const
{
    // TODO
    return 0;
}

long wxTreeCtrl::GetRootItem() const
{
    // TODO
    return 0;
}

bool wxTreeCtrl::GetItem(wxTreeItem& info) const
{
    // TODO
    return false;
}

bool wxTreeCtrl::SetItem(wxTreeItem& info)
{
    // TODO
    return false;
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
        return wxEmptyString;
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

bool wxTreeCtrl::GetItemRect(long item, wxRect& rect, bool textOnly) const
{
    // TODO
    return false;
}

wxTextCtrl* wxTreeCtrl::GetEditControl() const
{
    return m_textCtrl;
}

// Operations
bool wxTreeCtrl::DeleteItem(long item)
{
    // TODO
    return false;
}

bool wxTreeCtrl::ExpandItem(long item, int action)
{
    // TODO
    switch ( action )
    {
    case wxTREE_EXPAND_EXPAND:
        break;

    case wxTREE_EXPAND_COLLAPSE:
        break;

    case wxTREE_EXPAND_COLLAPSE_RESET:
        break;

    case wxTREE_EXPAND_TOGGLE:
        break;

    default:
        wxFAIL_MSG("unknown action in wxTreeCtrl::ExpandItem");
    }

    bool bOk = false; // TODO expand item

    // May not send messages, so emulate them
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
    // TODO
    return 0;
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
    // TODO
    return false;
}

bool wxTreeCtrl::ScrollTo(long item)
{
    // TODO
    return false;
}

bool wxTreeCtrl::DeleteAllItems()
{
    // TODO
    return false;
}

wxTextCtrl* wxTreeCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    // TODO
    return NULL;
}

// End label editing, optionally cancelling the edit
bool wxTreeCtrl::EndEditLabel(bool cancel)
{
    // TODO
    return false;
}

long wxTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
    // TODO
    return 0;
}

bool wxTreeCtrl::SortChildren(long item)
{
    // TODO
    return false;
}

bool wxTreeCtrl::EnsureVisible(long item)
{
    // TODO
    return false;
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

// Tree event
IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxCommandEvent)

wxTreeEvent::wxTreeEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id)
{
    m_code = 0;
    m_oldItem = 0;
}
