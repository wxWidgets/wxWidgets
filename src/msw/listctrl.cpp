/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "listctrl.h"
    #pragma implementation "listctrlbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef __WIN95__

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/textctrl.h"
#include "wx/imaglist.h"

#include "wx/listctrl.h"

#include "wx/msw/private.h"

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#else
    #include <commctrl.h>
#endif

#ifndef LVHT_ONITEM
    #define LVHT_ONITEM \
                (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON)
#endif

#ifndef LVM_SETEXTENDEDLISTVIEWSTYLE
    #define LVM_SETEXTENDEDLISTVIEWSTYLE (0x1000 + 54)
#endif

#ifndef LVS_EX_FULLROWSELECT
    #define LVS_EX_FULLROWSELECT 0x00000020
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void wxConvertToMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& tvItem);
static void wxConvertFromMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& tvItem, HWND getFullInfo = 0);

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

BEGIN_EVENT_TABLE(wxListCtrl, wxControl)
    EVT_PAINT(wxListCtrl::OnPaint)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListEvent
// ----------------------------------------------------------------------------

void wxListEvent::CopyObject(wxObject& object_dest) const
{
    wxListEvent *obj = (wxListEvent *)&object_dest;

    wxNotifyEvent::CopyObject(object_dest);

    obj->m_code = m_code;
    obj->m_itemIndex = m_itemIndex;
    obj->m_oldItemIndex = m_oldItemIndex;
    obj->m_col = m_col;
    obj->m_cancelled = m_cancelled;
    obj->m_pointDrag = m_pointDrag;
    obj->m_item.m_mask = m_item.m_mask;
    obj->m_item.m_itemId = m_item.m_itemId;
    obj->m_item.m_col = m_item.m_col;
    obj->m_item.m_state = m_item.m_state;
    obj->m_item.m_stateMask = m_item.m_stateMask;
    obj->m_item.m_text = m_item.m_text;
    obj->m_item.m_image = m_item.m_image;
    obj->m_item.m_data = m_item.m_data;
    obj->m_item.m_format = m_item.m_format;
    obj->m_item.m_width = m_item.m_width;

    if ( m_item.HasAttributes() )
    {
        obj->m_item.SetTextColour(m_item.GetTextColour());
        obj->m_item.SetBackgroundColour(m_item.GetBackgroundColour());
        obj->m_item.SetFont(m_item.GetFont());
    }
}

// ----------------------------------------------------------------------------
// wxListCtrl construction
// ----------------------------------------------------------------------------

void wxListCtrl::Init()
{
    m_imageListNormal = NULL;
    m_imageListSmall = NULL;
    m_imageListState = NULL;
    m_baseStyle = 0;
    m_colCount = 0;
    m_textCtrl = NULL;
    m_hasAnyAttr = FALSE;
}

bool wxListCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif // wxUSE_VALIDATORS

    SetName(name);

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

    m_windowId = (id == -1) ? NewControlId() : id;

    DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP |
                   LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS;
    if ( wxStyleHasBorder(m_windowStyle) )
        wstyle |= WS_BORDER;
    m_baseStyle = wstyle;

    if ( !DoCreateControl(x, y, width, height) )
        return FALSE;

    if (parent)
        parent->AddChild(this);

    return TRUE;
}

bool wxListCtrl::DoCreateControl(int x, int y, int w, int h)
{
    DWORD wstyle = m_baseStyle;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D);

    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    if ( want3D )
        wstyle |= WS_BORDER;

    long oldStyle = 0; // Dummy
    wstyle |= ConvertToMSWStyle(oldStyle, m_windowStyle);

    // Create the ListView control.
    m_hWnd = (WXHWND)CreateWindowEx(exStyle,
                                    WC_LISTVIEW,
                                    wxT(""),
                                    wstyle,
                                    x, y, w, h,
                                    GetWinHwnd(GetParent()),
                                    (HMENU)m_windowId,
                                    wxGetInstance(),
                                    NULL);

    if ( !m_hWnd )
    {
        wxLogError(_("Can't create list control window, check that comctl32.dll is installed."));

        return FALSE;
    }

    // for comctl32.dll v 4.70+ we want to have this attribute because it's
    // prettier (and also because wxGTK does it like this)
    if ( (wstyle & LVS_REPORT) && wxTheApp->GetComCtl32Version() >= 470 )
    {
        ::SendMessage(GetHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE,
                      0, LVS_EX_FULLROWSELECT);
    }

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());

    SubclassWin(m_hWnd);

    return TRUE;
}

void wxListCtrl::UpdateStyle()
{
    if ( GetHWND() )
    {
        // The new window view style
        long dummy;
        DWORD dwStyleNew = ConvertToMSWStyle(dummy, m_windowStyle);
        dwStyleNew |= m_baseStyle;

        // Get the current window style.
        DWORD dwStyleOld = ::GetWindowLong(GetHwnd(), GWL_STYLE);

        // Only set the window style if the view bits have changed.
        if ( dwStyleOld != dwStyleNew )
        {
            ::SetWindowLong(GetHwnd(), GWL_STYLE, dwStyleNew);
        }
    }
}

void wxListCtrl::FreeAllAttrs(bool dontRecreate)
{
    if ( m_hasAnyAttr )
    {
        for ( wxNode *node = m_attrs.Next(); node; node = m_attrs.Next() )
        {
            delete (wxListItemAttr *)node->Data();
        }

        m_attrs.Destroy();
        if ( !dontRecreate )
        {
            m_attrs.Create(wxKEY_INTEGER, 1000);        // just as def ctor
        }

        m_hasAnyAttr = FALSE;
    }
}

wxListCtrl::~wxListCtrl()
{
    FreeAllAttrs(TRUE /* no need to recreate hash any more */);

    if ( m_textCtrl )
    {
        m_textCtrl->UnsubclassWin();
        m_textCtrl->SetHWND(0);
        delete m_textCtrl;
        m_textCtrl = NULL;
    }
}

// ----------------------------------------------------------------------------
// set/get/change style
// ----------------------------------------------------------------------------

// Add or remove a single window style
void wxListCtrl::SetSingleStyle(long style, bool add)
{
    long flag = GetWindowStyleFlag();

    // Get rid of conflicting styles
    if ( add )
    {
        if ( style & wxLC_MASK_TYPE)
            flag = flag & ~wxLC_MASK_TYPE;
        if ( style & wxLC_MASK_ALIGN )
            flag = flag & ~wxLC_MASK_ALIGN;
        if ( style & wxLC_MASK_SORT )
            flag = flag & ~wxLC_MASK_SORT;
    }

    if ( flag & style )
    {
        if ( !add )
            flag -= style;
    }
    else
    {
        if ( add )
        {
            flag |= style;
        }
    }

    m_windowStyle = flag;

    UpdateStyle();
}

// Set the whole window style
void wxListCtrl::SetWindowStyleFlag(long flag)
{
    m_windowStyle = flag;

    UpdateStyle();
}

// Can be just a single style, or a bitlist
long wxListCtrl::ConvertToMSWStyle(long& oldStyle, long style) const
{
    long wstyle = 0;
    if ( style & wxLC_ICON )
    {
        if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
            oldStyle -= LVS_SMALLICON;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
            oldStyle -= LVS_REPORT;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
            oldStyle -= LVS_LIST;
        wstyle |= LVS_ICON;
    }

    if ( style & wxLC_SMALL_ICON )
    {
        if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
            oldStyle -= LVS_ICON;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
            oldStyle -= LVS_REPORT;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
            oldStyle -= LVS_LIST;
        wstyle |= LVS_SMALLICON;
    }

    if ( style & wxLC_LIST )
    {
        if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
            oldStyle -= LVS_ICON;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_REPORT )
            oldStyle -= LVS_REPORT;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
            oldStyle -= LVS_SMALLICON;
        wstyle |= LVS_LIST;
    }

    if ( style & wxLC_REPORT )
    {
        if ( (oldStyle & LVS_TYPEMASK) == LVS_ICON )
            oldStyle -= LVS_ICON;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_LIST )
            oldStyle -= LVS_LIST;
        if ( (oldStyle & LVS_TYPEMASK) == LVS_SMALLICON )
            oldStyle -= LVS_SMALLICON;

        wstyle |= LVS_REPORT;
    }

    if ( style & wxLC_ALIGN_LEFT )
    {
        if ( oldStyle & LVS_ALIGNTOP )
            oldStyle -= LVS_ALIGNTOP;
        wstyle |= LVS_ALIGNLEFT;
    }

    if ( style & wxLC_ALIGN_TOP )
    {
        if ( oldStyle & LVS_ALIGNLEFT )
            oldStyle -= LVS_ALIGNLEFT;
        wstyle |= LVS_ALIGNTOP;
    }

    if ( style & wxLC_AUTOARRANGE )
        wstyle |= LVS_AUTOARRANGE;

    // Apparently, no such style (documentation wrong?)
    /*
       if ( style & wxLC_BUTTON )
       wstyle |= LVS_BUTTON;
     */

    if ( style & wxLC_NO_SORT_HEADER )
        wstyle |= LVS_NOSORTHEADER;

    if ( style & wxLC_NO_HEADER )
        wstyle |= LVS_NOCOLUMNHEADER;

    if ( style & wxLC_EDIT_LABELS )
        wstyle |= LVS_EDITLABELS;

    if ( style & wxLC_SINGLE_SEL )
        wstyle |= LVS_SINGLESEL;

    if ( style & wxLC_SORT_ASCENDING )
    {
        if ( oldStyle & LVS_SORTDESCENDING )
            oldStyle -= LVS_SORTDESCENDING;
        wstyle |= LVS_SORTASCENDING;
    }

    if ( style & wxLC_SORT_DESCENDING )
    {
        if ( oldStyle & LVS_SORTASCENDING )
            oldStyle -= LVS_SORTASCENDING;
        wstyle |= LVS_SORTDESCENDING;
    }

    return wstyle;
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// Sets the foreground, i.e. text, colour
bool wxListCtrl::SetForegroundColour(const wxColour& col)
{
    if ( !wxWindow::SetForegroundColour(col) )
        return FALSE;

    ListView_SetTextColor(GetHwnd(), wxColourToRGB(col));

    return TRUE;
}

// Sets the background colour
bool wxListCtrl::SetBackgroundColour(const wxColour& col)
{
    if ( !wxWindow::SetBackgroundColour(col) )
        return FALSE;

    // we set the same colour for both the "empty" background and the items
    // background
    COLORREF color = wxColourToRGB(col);
    ListView_SetBkColor(GetHwnd(), color);
    ListView_SetTextBkColor(GetHwnd(), color);

    return TRUE;
}

// Gets information about this column
bool wxListCtrl::GetColumn(int col, wxListItem& item) const
{
    LV_COLUMN lvCol;
    lvCol.mask = 0;
    lvCol.fmt = 0;
    lvCol.pszText = NULL;

    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
        lvCol.mask |= LVCF_TEXT;
        lvCol.pszText = new wxChar[513];
        lvCol.cchTextMax = 512;
    }

    bool success = (ListView_GetColumn(GetHwnd(), col, & lvCol) != 0);

    //  item.m_subItem = lvCol.iSubItem;
    item.m_width = lvCol.cx;

    if ( (item.m_mask & wxLIST_MASK_TEXT) && lvCol.pszText )
    {
        item.m_text = lvCol.pszText;
        delete[] lvCol.pszText;
    }

    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        if (lvCol.fmt == LVCFMT_LEFT)
            item.m_format = wxLIST_FORMAT_LEFT;
        else if (lvCol.fmt == LVCFMT_RIGHT)
            item.m_format = wxLIST_FORMAT_RIGHT;
        else if (lvCol.fmt == LVCFMT_CENTER)
            item.m_format = wxLIST_FORMAT_CENTRE;
    }

    return success;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
    LV_COLUMN lvCol;
    lvCol.mask = 0;
    lvCol.fmt = 0;
    lvCol.pszText = NULL;

    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
        lvCol.mask |= LVCF_TEXT;
        lvCol.pszText = WXSTRINGCAST item.m_text;
        lvCol.cchTextMax = 0; // Ignored
    }
    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        lvCol.mask |= LVCF_FMT;

        if ( item.m_format == wxLIST_FORMAT_LEFT )
            lvCol.fmt = LVCFMT_LEFT;
        if ( item.m_format == wxLIST_FORMAT_RIGHT )
            lvCol.fmt = LVCFMT_RIGHT;
        if ( item.m_format == wxLIST_FORMAT_CENTRE )
            lvCol.fmt = LVCFMT_CENTER;
    }

    if ( item.m_mask & wxLIST_MASK_WIDTH )
    {
        lvCol.mask |= LVCF_WIDTH;
        lvCol.cx = item.m_width;

        if ( lvCol.cx == wxLIST_AUTOSIZE)
            lvCol.cx = LVSCW_AUTOSIZE;
        else if ( lvCol.cx == wxLIST_AUTOSIZE_USEHEADER)
            lvCol.cx = LVSCW_AUTOSIZE_USEHEADER;
    }
    lvCol.mask |= LVCF_SUBITEM;
    lvCol.iSubItem = col;
    return (ListView_SetColumn(GetHwnd(), col, & lvCol) != 0);
}

// Gets the column width
int wxListCtrl::GetColumnWidth(int col) const
{
    return ListView_GetColumnWidth(GetHwnd(), col);
}

// Sets the column width
bool wxListCtrl::SetColumnWidth(int col, int width)
{
    int col2 = col;
    if ( m_windowStyle & wxLC_LIST )
        col2 = -1;

    int width2 = width;
    if ( width2 == wxLIST_AUTOSIZE)
        width2 = LVSCW_AUTOSIZE;
    else if ( width2 == wxLIST_AUTOSIZE_USEHEADER)
        width2 = LVSCW_AUTOSIZE_USEHEADER;

    return (ListView_SetColumnWidth(GetHwnd(), col2, width2) != 0);
}

// Gets the number of items that can fit vertically in the
// visible area of the list control (list or report view)
// or the total number of items in the list control (icon
// or small icon view)
int wxListCtrl::GetCountPerPage() const
{
    return ListView_GetCountPerPage(GetHwnd());
}

// Gets the edit control for editing labels.
wxTextCtrl* wxListCtrl::GetEditControl() const
{
    return m_textCtrl;
}

// Gets information about the item
bool wxListCtrl::GetItem(wxListItem& info) const
{
    LV_ITEM lvItem;
    wxZeroMemory(lvItem);

    lvItem.iItem = info.m_itemId;
    lvItem.iSubItem = info.m_col;

    if ( info.m_mask & wxLIST_MASK_TEXT )
    {
        lvItem.mask |= LVIF_TEXT;
        lvItem.pszText = new wxChar[513];
        lvItem.cchTextMax = 512;
    }
    else
    {
        lvItem.pszText = NULL;
    }

    if (info.m_mask & wxLIST_MASK_DATA)
        lvItem.mask |= LVIF_PARAM;

    if ( info.m_mask & wxLIST_MASK_STATE )
    {
        lvItem.mask |= LVIF_STATE;
        // the other bits are hardly interesting anyhow
        lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    }

    bool success = ListView_GetItem((HWND)GetHWND(), &lvItem) != 0;
    if ( !success )
    {
        wxLogError(_("Couldn't retrieve information about list control item %d."),
                lvItem.iItem);
    }
    else
    {
        wxConvertFromMSWListItem(this, info, lvItem);
    }

    if (lvItem.pszText)
        delete[] lvItem.pszText;

    return success;
}

// Sets information about the item
bool wxListCtrl::SetItem(wxListItem& info)
{
    LV_ITEM item;
    wxConvertToMSWListItem(this, info, item);

    // check whether it has any custom attributes
    if ( info.HasAttributes() )
    {

        wxListItemAttr *attr;
        attr = (wxListItemAttr*) m_attrs.Get(item.iItem);

        if (attr == NULL)

            m_attrs.Put(item.iItem, (wxObject *)new wxListItemAttr(*info.GetAttributes()));

        else *attr = *info.GetAttributes();

        m_hasAnyAttr = TRUE;
    }

    item.cchTextMax = 0;
    bool ok = ListView_SetItem(GetHwnd(), &item) != 0;
    if ( ok && (info.m_mask & wxLIST_MASK_IMAGE) )
    {
        // make the change visible
        ListView_Update(GetHwnd(), item.iItem);
    }

    return ok;
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    info.m_col = col;
    if ( imageId > -1 )
    {
        info.m_image = imageId;
        info.m_mask |= wxLIST_MASK_IMAGE;
    }
    return SetItem(info);
}


// Gets the item state
int wxListCtrl::GetItemState(long item, long stateMask) const
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_itemId = item;

    if (!GetItem(info))
        return 0;

    return info.m_state;
}

// Sets the item state
bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_state = state;
    info.m_stateMask = stateMask;
    info.m_itemId = item;

    return SetItem(info);
}

// Sets the item image
bool wxListCtrl::SetItemImage(long item, int image, int selImage)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_image = image;
    info.m_itemId = item;

    return SetItem(info);
}

// Gets the item text
wxString wxListCtrl::GetItemText(long item) const
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = item;

    if (!GetItem(info))
        return wxString("");
    return info.m_text;
}

// Sets the item text
void wxListCtrl::SetItemText(long item, const wxString& str)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = item;
    info.m_text = str;

    SetItem(info);
}

// Gets the item data
long wxListCtrl::GetItemData(long item) const
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;

    if (!GetItem(info))
        return 0;
    return info.m_data;
}

// Sets the item data
bool wxListCtrl::SetItemData(long item, long data)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;
    info.m_data = data;

    return SetItem(info);
}

// Gets the item rectangle
bool wxListCtrl::GetItemRect(long item, wxRect& rect, int code) const
{
    RECT rect2;

    int code2 = LVIR_BOUNDS;
    if ( code == wxLIST_RECT_BOUNDS )
        code2 = LVIR_BOUNDS;
    else if ( code == wxLIST_RECT_ICON )
        code2 = LVIR_ICON;
    else if ( code == wxLIST_RECT_LABEL )
        code2 = LVIR_LABEL;

#ifdef __WXWINE__
    bool success = (ListView_GetItemRect(GetHwnd(), (int) item, &rect2 ) != 0);
#else
    bool success = (ListView_GetItemRect(GetHwnd(), (int) item, &rect2, code2) != 0);
#endif

    rect.x = rect2.left;
    rect.y = rect2.top;
    rect.width = rect2.right - rect2.left;
    rect.height = rect2.bottom - rect2.top;
    return success;
}

// Gets the item position
bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    POINT pt;

    bool success = (ListView_GetItemPosition(GetHwnd(), (int) item, &pt) != 0);

    pos.x = pt.x; pos.y = pt.y;
    return success;
}

// Sets the item position.
bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
{
    return (ListView_SetItemPosition(GetHwnd(), (int) item, pos.x, pos.y) != 0);
}

// Gets the number of items in the list control
int wxListCtrl::GetItemCount() const
{
    return ListView_GetItemCount(GetHwnd());
}

// Retrieves the spacing between icons in pixels.
// If small is TRUE, gets the spacing for the small icon
// view, otherwise the large icon view.
int wxListCtrl::GetItemSpacing(bool isSmall) const
{
    return ListView_GetItemSpacing(GetHwnd(), (BOOL) isSmall);
}

// Gets the number of selected items in the list control
int wxListCtrl::GetSelectedItemCount() const
{
    return ListView_GetSelectedCount(GetHwnd());
}

// Gets the text colour of the listview
wxColour wxListCtrl::GetTextColour() const
{
    COLORREF ref = ListView_GetTextColor(GetHwnd());
    wxColour col(GetRValue(ref), GetGValue(ref), GetBValue(ref));
    return col;
}

// Sets the text colour of the listview
void wxListCtrl::SetTextColour(const wxColour& col)
{
    ListView_SetTextColor(GetHwnd(), PALETTERGB(col.Red(), col.Green(), col.Blue()));
}

// Gets the index of the topmost visible item when in
// list or report view
long wxListCtrl::GetTopItem() const
{
    return (long) ListView_GetTopIndex(GetHwnd());
}

// Searches for an item, starting from 'item'.
// 'geometry' is one of
// wxLIST_NEXT_ABOVE/ALL/BELOW/LEFT/RIGHT.
// 'state' is a state bit flag, one or more of
// wxLIST_STATE_DROPHILITED/FOCUSED/SELECTED/CUT.
// item can be -1 to find the first item that matches the
// specified flags.
// Returns the item or -1 if unsuccessful.
long wxListCtrl::GetNextItem(long item, int geom, int state) const
{
    long flags = 0;

    if ( geom == wxLIST_NEXT_ABOVE )
        flags |= LVNI_ABOVE;
    if ( geom == wxLIST_NEXT_ALL )
        flags |= LVNI_ALL;
    if ( geom == wxLIST_NEXT_BELOW )
        flags |= LVNI_BELOW;
    if ( geom == wxLIST_NEXT_LEFT )
        flags |= LVNI_TOLEFT;
    if ( geom == wxLIST_NEXT_RIGHT )
        flags |= LVNI_TORIGHT;

    if ( state & wxLIST_STATE_CUT )
        flags |= LVNI_CUT;
    if ( state & wxLIST_STATE_DROPHILITED )
        flags |= LVNI_DROPHILITED;
    if ( state & wxLIST_STATE_FOCUSED )
        flags |= LVNI_FOCUSED;
    if ( state & wxLIST_STATE_SELECTED )
        flags |= LVNI_SELECTED;

    return (long) ListView_GetNextItem(GetHwnd(), item, flags);
}


wxImageList *wxListCtrl::GetImageList(int which) const
{
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        return m_imageListNormal;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        return m_imageListSmall;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        return m_imageListState;
    }
    return NULL;
}

void wxListCtrl::SetImageList(wxImageList *imageList, int which)
{
    int flags = 0;
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        flags = LVSIL_NORMAL;
        m_imageListNormal = imageList;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        flags = LVSIL_SMALL;
        m_imageListSmall = imageList;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        flags = LVSIL_STATE;
        m_imageListState = imageList;
    }
    ListView_SetImageList(GetHwnd(), (HIMAGELIST) imageList ? imageList->GetHIMAGELIST() : 0, flags);
}

// ----------------------------------------------------------------------------
// Operations
// ----------------------------------------------------------------------------

// Arranges the items
bool wxListCtrl::Arrange(int flag)
{
    UINT code = 0;
    if ( flag == wxLIST_ALIGN_LEFT )
        code = LVA_ALIGNLEFT;
    else if ( flag == wxLIST_ALIGN_TOP )
        code = LVA_ALIGNTOP;
    else if ( flag == wxLIST_ALIGN_DEFAULT )
        code = LVA_DEFAULT;
    else if ( flag == wxLIST_ALIGN_SNAP_TO_GRID )
        code = LVA_SNAPTOGRID;

    return (ListView_Arrange(GetHwnd(), code) != 0);
}

// Deletes an item
bool wxListCtrl::DeleteItem(long item)
{
    return (ListView_DeleteItem(GetHwnd(), (int) item) != 0);
}

// Deletes all items
bool wxListCtrl::DeleteAllItems()
{
    return (ListView_DeleteAllItems(GetHwnd()) != 0);
}

// Deletes all items
bool wxListCtrl::DeleteAllColumns()
{
    while ( m_colCount > 0 )
    {
        if ( ListView_DeleteColumn(GetHwnd(), 0) == 0 )
        {
            wxLogLastError(wxT("ListView_DeleteColumn"));

            return FALSE;
        }

        m_colCount--;
    }

    wxASSERT_MSG( m_colCount == 0, wxT("no columns should be left") );

    return TRUE;
}

// Deletes a column
bool wxListCtrl::DeleteColumn(int col)
{
    bool success = (ListView_DeleteColumn(GetHwnd(), col) != 0);

    if ( success && (m_colCount > 0) )
        m_colCount --;
    return success;
}

// Clears items, and columns if there are any.
void wxListCtrl::ClearAll()
{
    DeleteAllItems();
    if ( m_colCount > 0 )
        DeleteAllColumns();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    wxASSERT( (textControlClass->IsKindOf(CLASSINFO(wxTextCtrl))) );

    HWND hWnd = (HWND) ListView_EditLabel(GetHwnd(), item);

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

    return m_textCtrl;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool cancel)
{
    wxFAIL;

    /* I don't know how to implement this: there's no such macro as ListView_EndEditLabelNow.
     * ???
     bool success = (ListView_EndEditLabelNow(GetHwnd(), cancel) != 0);

     if (m_textCtrl)
     {
     m_textCtrl->UnsubclassWin();
     m_textCtrl->SetHWND(0);
     delete m_textCtrl;
     m_textCtrl = NULL;
     }
     return success;
     */
    return FALSE;
}


// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
    return (ListView_EnsureVisible(GetHwnd(), (int) item, FALSE) != 0);
}

// Find an item whose label matches this string, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    LV_FINDINFO findInfo;

    findInfo.flags = LVFI_STRING;
    if ( partial )
        findInfo.flags |= LVFI_PARTIAL;
    findInfo.psz = str;

    // ListView_FindItem() excludes the first item from search and to look
    // through all the items you need to start from -1 which is unnatural and
    // inconsitent with the generic version - so we adjust the index
    return ListView_FindItem(GetHwnd(), (int) start - 1, &findInfo);
}

// Find an item whose data matches this data, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, long data)
{
    LV_FINDINFO findInfo;

    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = data;

    return ListView_FindItem(GetHwnd(), (int) start, & findInfo);
}

// Find an item nearest this position in the specified direction, starting from
// the item after 'start' or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxPoint& pt, int direction)
{
    LV_FINDINFO findInfo;

    findInfo.flags = LVFI_NEARESTXY;
    findInfo.pt.x = pt.x;
    findInfo.pt.y = pt.y;
    findInfo.vkDirection = VK_RIGHT;

    if ( direction == wxLIST_FIND_UP )
        findInfo.vkDirection = VK_UP;
    else if ( direction == wxLIST_FIND_DOWN )
        findInfo.vkDirection = VK_DOWN;
    else if ( direction == wxLIST_FIND_LEFT )
        findInfo.vkDirection = VK_LEFT;
    else if ( direction == wxLIST_FIND_RIGHT )
        findInfo.vkDirection = VK_RIGHT;

    return ListView_FindItem(GetHwnd(), (int) start, & findInfo);
}

// Determines which item (if any) is at the specified point,
// giving details in 'flags' (see wxLIST_HITTEST_... flags above)
long wxListCtrl::HitTest(const wxPoint& point, int& flags)
{
    LV_HITTESTINFO hitTestInfo;
    hitTestInfo.pt.x = (int) point.x;
    hitTestInfo.pt.y = (int) point.y;

    ListView_HitTest(GetHwnd(), & hitTestInfo);

    flags = 0;
    if ( hitTestInfo.flags & LVHT_ABOVE )
        flags |= wxLIST_HITTEST_ABOVE;
    if ( hitTestInfo.flags & LVHT_BELOW )
        flags |= wxLIST_HITTEST_BELOW;
    if ( hitTestInfo.flags & LVHT_NOWHERE )
        flags |= wxLIST_HITTEST_NOWHERE;
    if ( hitTestInfo.flags & LVHT_ONITEMICON )
        flags |= wxLIST_HITTEST_ONITEMICON;
    if ( hitTestInfo.flags & LVHT_ONITEMLABEL )
        flags |= wxLIST_HITTEST_ONITEMLABEL;
    if ( hitTestInfo.flags & LVHT_ONITEMSTATEICON )
        flags |= wxLIST_HITTEST_ONITEMSTATEICON;
    if ( hitTestInfo.flags & LVHT_TOLEFT )
        flags |= wxLIST_HITTEST_TOLEFT;
    if ( hitTestInfo.flags & LVHT_TORIGHT )
        flags |= wxLIST_HITTEST_TORIGHT;

    return (long) hitTestInfo.iItem;
}

// Inserts an item, returning the index of the new item if successful,
// -1 otherwise.
long wxListCtrl::InsertItem(wxListItem& info)
{
    LV_ITEM item;
    wxConvertToMSWListItem(this, info, item);

    // check whether it has any custom attributes
    if ( info.HasAttributes() )
    {

        wxListItemAttr *attr;
        attr = (wxListItemAttr*) m_attrs.Get(item.iItem);

        if (attr == NULL)

            m_attrs.Put(item.iItem, (wxObject *)new wxListItemAttr(*info.GetAttributes()));

        else *attr = *info.GetAttributes();

        m_hasAnyAttr = TRUE;
    }

    return (long) ListView_InsertItem(GetHwnd(), & item);
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem(info);
}

// Inserts an image item
long wxListCtrl::InsertItem(long index, int imageIndex)
{
    wxListItem info;
    info.m_image = imageIndex;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_itemId = index;
    return InsertItem(info);
}

// Inserts an image/string item
long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
    wxListItem info;
    info.m_image = imageIndex;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem(info);
}

// For list view mode (only), inserts a column.
long wxListCtrl::InsertColumn(long col, wxListItem& item)
{
    LV_COLUMN lvCol;
    lvCol.mask = 0;
    lvCol.fmt = 0;
    lvCol.pszText = NULL;

    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
        lvCol.mask |= LVCF_TEXT;
        lvCol.pszText = WXSTRINGCAST item.m_text;
        lvCol.cchTextMax = 0; // Ignored
    }
    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        lvCol.mask |= LVCF_FMT;

        if ( item.m_format == wxLIST_FORMAT_LEFT )
            lvCol.fmt = LVCFMT_LEFT;
        if ( item.m_format == wxLIST_FORMAT_RIGHT )
            lvCol.fmt = LVCFMT_RIGHT;
        if ( item.m_format == wxLIST_FORMAT_CENTRE )
            lvCol.fmt = LVCFMT_CENTER;
    }

    lvCol.mask |= LVCF_WIDTH;
    if ( item.m_mask & wxLIST_MASK_WIDTH )
    {
        if ( item.m_width == wxLIST_AUTOSIZE)
            lvCol.cx = LVSCW_AUTOSIZE;
        else if ( item.m_width == wxLIST_AUTOSIZE_USEHEADER)
            lvCol.cx = LVSCW_AUTOSIZE_USEHEADER;
        else
            lvCol.cx = item.m_width;
    }
    else
    {
        // always give some width to the new column: this one is compatible
        // with wxGTK
        lvCol.cx = 80;
    }

    lvCol.mask |= LVCF_SUBITEM;
    lvCol.iSubItem = col;

    bool success = ListView_InsertColumn(GetHwnd(), col, & lvCol) != -1;
    if ( success )
    {
        m_colCount++;
    }
    else
    {
        wxLogDebug(wxT("Failed to insert the column '%s' into listview!"),
                   lvCol.pszText);
    }

    return success;
}

long wxListCtrl::InsertColumn(long col,
                              const wxString& heading,
                              int format,
                              int width)
{
    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT;
    item.m_text = heading;
    if ( width > -1 )
    {
        item.m_mask |= wxLIST_MASK_WIDTH;
        item.m_width = width;
    }
    item.m_format = format;

    return InsertColumn(col, item);
}

// Scrolls the list control. If in icon, small icon or report view mode,
// x specifies the number of pixels to scroll. If in list view mode, x
// specifies the number of columns to scroll.
// If in icon, small icon or list view mode, y specifies the number of pixels
// to scroll. If in report view mode, y specifies the number of lines to scroll.
bool wxListCtrl::ScrollList(int dx, int dy)
{
    return (ListView_Scroll(GetHwnd(), dx, dy) != 0);
}

// Sort items.

// fn is a function which takes 3 long arguments: item1, item2, data.
// item1 is the long data associated with a first item (NOT the index).
// item2 is the long data associated with a second item (NOT the index).
// data is the same value as passed to SortItems.
// The return value is a negative number if the first item should precede the second
// item, a positive number of the second item should precede the first,
// or zero if the two items are equivalent.

// data is arbitrary data to be passed to the sort function.
bool wxListCtrl::SortItems(wxListCtrlCompare fn, long data)
{
    return (ListView_SortItems(GetHwnd(), (PFNLVCOMPARE) fn, data) != 0);
}

// ----------------------------------------------------------------------------
// message processing
// ----------------------------------------------------------------------------

bool wxListCtrl::MSWCommand(WXUINT cmd, WXWORD id)
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
    else
        return FALSE;
}

bool wxListCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    // prepare the event
    // -----------------

    wxListEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;

    NMHDR *nmhdr = (NMHDR *)lParam;

    // almost all messages use NM_LISTVIEW
    NM_LISTVIEW *nmLV = (NM_LISTVIEW *)nmhdr;

    // this is true for almost all events
    event.m_item.m_data = nmLV->lParam;

    switch ( nmhdr->code )
    {
        case LVN_BEGINRDRAG:
            eventType = wxEVT_COMMAND_LIST_BEGIN_RDRAG;
            // fall through

        case LVN_BEGINDRAG:
            if ( eventType == wxEVT_NULL )
            {
                eventType = wxEVT_COMMAND_LIST_BEGIN_DRAG;
            }

            event.m_itemIndex = nmLV->iItem;
            event.m_pointDrag.x = nmLV->ptAction.x;
            event.m_pointDrag.y = nmLV->ptAction.y;
            break;

        case LVN_BEGINLABELEDIT:
            {
                eventType = wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT;
                LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                wxConvertFromMSWListItem(this, event.m_item, info->item, GetHwnd());
            }
            break;

        case LVN_COLUMNCLICK:
            eventType = wxEVT_COMMAND_LIST_COL_CLICK;
            event.m_itemIndex = -1;
            event.m_col = nmLV->iSubItem;
            break;

        case LVN_DELETEALLITEMS:
            eventType = wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS;
            event.m_itemIndex = -1;

            FreeAllAttrs();

            break;

        case LVN_DELETEITEM:
            eventType = wxEVT_COMMAND_LIST_DELETE_ITEM;
            event.m_itemIndex = nmLV->iItem;

            if ( m_hasAnyAttr )
            {
                delete (wxListItemAttr *)m_attrs.Delete(nmLV->iItem);
            }
            break;

        case LVN_ENDLABELEDIT:
            {
                eventType = wxEVT_COMMAND_LIST_END_LABEL_EDIT;
                LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                wxConvertFromMSWListItem(this, event.m_item, info->item);
                if ( info->item.pszText == NULL || info->item.iItem == -1 )
                    return FALSE;
            }
            break;

        case LVN_SETDISPINFO:
            {
                eventType = wxEVT_COMMAND_LIST_SET_INFO;
                LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                wxConvertFromMSWListItem(this, event.m_item, info->item, GetHwnd());
            }
            break;

        case LVN_GETDISPINFO:
                // this provokes stack overflow: indeed, wxConvertFromMSWListItem()
                // sends us WM_NOTIFY! As it doesn't do anything for now, just leave
                // it out.
#if 0
            {
                // TODO: some text buffering here, I think
                // TODO: API for getting Windows to retrieve values
                // on demand.
                eventType = wxEVT_COMMAND_LIST_GET_INFO;
                LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                wxConvertFromMSWListItem(this, event.m_item, info->item, GetHwnd());
                break;
            }
#endif // 0
                return FALSE;


        case LVN_INSERTITEM:
            eventType = wxEVT_COMMAND_LIST_INSERT_ITEM;
            event.m_itemIndex = nmLV->iItem;
            break;

        case LVN_ITEMCHANGED:
            // This needs to be sent to wxListCtrl as a rather more concrete
            // event. For now, just detect a selection or deselection.
            if ( (nmLV->uNewState & LVIS_SELECTED) && !(nmLV->uOldState & LVIS_SELECTED) )
            {
                eventType = wxEVT_COMMAND_LIST_ITEM_SELECTED;
                event.m_itemIndex = nmLV->iItem;
            }
            else if ( !(nmLV->uNewState & LVIS_SELECTED) && (nmLV->uOldState & LVIS_SELECTED) )
            {
                eventType = wxEVT_COMMAND_LIST_ITEM_DESELECTED;
                event.m_itemIndex = nmLV->iItem;
            }
            else
            {
                return FALSE;
            }
            break;

        case LVN_KEYDOWN:
            {
                LV_KEYDOWN *info = (LV_KEYDOWN *)lParam;
                WORD wVKey = info->wVKey;

                // get the current selection
                long lItem = GetNextItem(-1,
                                         wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);

                // <Enter> or <Space> activate the selected item if any
                if ( lItem != -1 && (wVKey == VK_RETURN || wVKey == VK_SPACE) )
                {
                    // TODO this behaviour probably should be optional
                    eventType = wxEVT_COMMAND_LIST_ITEM_ACTIVATED;
                    event.m_itemIndex = lItem;
                }
                else
                {
                    eventType = wxEVT_COMMAND_LIST_KEY_DOWN;
                    event.m_code = wxCharCodeMSWToWX(wVKey);
                }

                if ( lItem != -1 )
                {
                    // fill the other fields too
                    event.m_item.m_text = GetItemText(lItem);
                    event.m_item.m_data = GetItemData(lItem);
                }
            }
            break;

        case NM_DBLCLK:
            // if the user processes it in wxEVT_COMMAND_LEFT_CLICK(), don't do
            // anything else
            if ( wxControl::MSWOnNotify(idCtrl, lParam, result) )
            {
                return TRUE;
            }

            // else translate it into wxEVT_COMMAND_LIST_ITEM_ACTIVATED event
            // if it happened on an item (and not on empty place)
            if ( nmLV->iItem == -1 )
            {
                // not on item
                return FALSE;
            }

            eventType = wxEVT_COMMAND_LIST_ITEM_ACTIVATED;
            event.m_itemIndex = nmLV->iItem;
            event.m_item.m_text = GetItemText(nmLV->iItem);
            event.m_item.m_data = GetItemData(nmLV->iItem);
            break;

        case NM_RCLICK:
            /* TECH NOTE: NM_RCLICK isn't really good enough here. We want to
               subclass and check for the actual WM_RBUTTONDOWN message,
               because NM_RCLICK waits for the WM_RBUTTONUP message as well
               before firing off. We want to have notify events for both down
               -and- up. */
            {
                // if the user processes it in wxEVT_COMMAND_RIGHT_CLICK(),
                // don't do anything else
                if ( wxControl::MSWOnNotify(idCtrl, lParam, result) )
                {
                    return TRUE;
                }

                // else translate it into wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK event
                LV_HITTESTINFO lvhti;
                wxZeroMemory(lvhti);

                ::GetCursorPos(&(lvhti.pt));
                ::ScreenToClient(GetHwnd(),&(lvhti.pt));
                if ( ListView_HitTest(GetHwnd(),&lvhti) != -1 )
                {
                    if ( lvhti.flags & LVHT_ONITEM )
                    {
                        eventType = wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK;
                        event.m_itemIndex = lvhti.iItem;
                    }
                }
            }
            break;

#if 0
        case NM_MCLICK: // ***** THERE IS NO NM_MCLICK. Subclass anyone? ******
            {
                // if the user processes it in wxEVT_COMMAND_MIDDLE_CLICK(), don't do
                // anything else
                if ( wxControl::MSWOnNotify(idCtrl, lParam, result) )
                {
                    return TRUE;
                }

                // else translate it into wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK event
                eventType = wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK;
                NMITEMACTIVATE* hdr = (NMITEMACTIVATE*)lParam;
                event.m_itemIndex = hdr->iItem;
            }
            break;
#endif // 0

#if defined(_WIN32_IE) && _WIN32_IE >= 0x300
        case NM_CUSTOMDRAW:
            {
                LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
                NMCUSTOMDRAW& nmcd = lplvcd->nmcd;
                switch( nmcd.dwDrawStage )
                {
                    case CDDS_PREPAINT:
                        // if we've got any items with non standard attributes,
                        // notify us before painting each item
                        *result = m_hasAnyAttr ? CDRF_NOTIFYITEMDRAW
                                               : CDRF_DODEFAULT;
                        return TRUE;

                    case CDDS_ITEMPREPAINT:
                        {
                            wxListItemAttr *attr =
                                (wxListItemAttr *)m_attrs.Get(nmcd.dwItemSpec);

                            if ( !attr )
                            {
                                // nothing to do for this item
                                return CDRF_DODEFAULT;
                            }

                            HFONT hFont;
                            wxColour colText, colBack;
                            if ( attr->HasFont() )
                            {
                                wxFont font = attr->GetFont();
                                hFont = (HFONT)font.GetResourceHandle();
                            }
                            else
                            {
                                hFont = 0;
                            }

                            if ( attr->HasTextColour() )
                            {
                                colText = attr->GetTextColour();
                            }
                            else
                            {
                                colText = GetTextColour();
                            }

                            if ( attr->HasBackgroundColour() )
                            {
                                colBack = attr->GetBackgroundColour();
                            }
                            else
                            {
                                colBack = GetBackgroundColour();
                            }

                            // note that if we wanted to set colours for
                            // individual columns (subitems), we would have
                            // returned CDRF_NOTIFYSUBITEMREDRAW from here
                            if ( hFont )
                            {
                                ::SelectObject(nmcd.hdc, hFont);

                                *result = CDRF_NEWFONT;
                            }
                            else
                            {
                                *result = CDRF_DODEFAULT;
                            }

                            lplvcd->clrText = wxColourToRGB(colText);
                            lplvcd->clrTextBk = wxColourToRGB(colBack);

                            return TRUE;
                        }

                    default:
                        *result = CDRF_DODEFAULT;
                        return TRUE;
                }
            }
            break;
#endif // _WIN32_IE >= 0x300

        default:
            return wxControl::MSWOnNotify(idCtrl, lParam, result);
    }

    // process the event
    // -----------------

    event.SetEventObject( this );
    event.SetEventType(eventType);

    if ( !GetEventHandler()->ProcessEvent(event) )
        return FALSE;

    // post processing
    // ---------------

    switch ( nmhdr->code )
    {
        case LVN_DELETEALLITEMS:
            // always return TRUE to suppress all additional LVN_DELETEITEM
            // notifications - this makes deleting all items from a list ctrl
            // much faster
            *result = TRUE;

            return TRUE;

        case LVN_GETDISPINFO:
            {
                LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                if ( info->item.mask & LVIF_TEXT )
                {
                    if ( !event.m_item.m_text.IsNull() )
                    {
                        info->item.pszText = AddPool(event.m_item.m_text);
                        info->item.cchTextMax = wxStrlen(info->item.pszText) + 1;
                    }
                }
                //    wxConvertToMSWListItem(this, event.m_item, info->item);
                break;
            }
        case LVN_ENDLABELEDIT:
            {
                *result = event.IsAllowed();
                return TRUE;
            }
    }

    *result = !event.IsAllowed();

    return TRUE;
}

wxChar *wxListCtrl::AddPool(const wxString& str)
{
    // Remove the first element if 3 strings exist
    if ( m_stringPool.Number() == 3 )
    {
        wxNode *node = m_stringPool.First();
        delete[] (char *)node->Data();
        delete node;
    }
    wxNode *node = m_stringPool.Add(WXSTRINGCAST str);
    return (wxChar *)node->Data();
}

// Necessary for drawing hrules and vrules, if specified
void wxListCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxControl::OnPaint(event);

    // Reset the device origin since it may have been set
    dc.SetDeviceOrigin(0, 0);

    bool drawHRules = ((GetWindowStyle() & wxLC_HRULES) != 0);
    bool drawVRules = ((GetWindowStyle() & wxLC_VRULES) != 0);

    if (!drawHRules && !drawVRules)
        return;
    if ((GetWindowStyle() & wxLC_REPORT) == 0)
        return;

    wxPen pen(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
    dc.SetPen(pen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    wxSize clientSize = GetClientSize();
    wxRect itemRect;
    int cy=0;

    int lastH = 0;
    int itemCount = GetItemCount();
    int i;
    for (i = 0; i < itemCount; i++)
    {
        if (GetItemRect(i, itemRect))
        {
            cy = itemRect.GetTop();
            if (i != 0) // Don't draw the first one
            {
                dc.DrawLine(0, cy, clientSize.x, cy);

                // Draw last line
                if (i == (GetItemCount() - 1))
                {
                    cy = itemRect.GetBottom();
                    dc.DrawLine(0, cy, clientSize.x, cy);
                }
            }
        }
    }
    i = (GetItemCount() - 1);
    if (drawVRules && (i > -1))
    {
        wxRect firstItemRect;
        GetItemRect(0, firstItemRect);

        if (GetItemRect(i, itemRect))
        {
            int col;
            int x = itemRect.GetX();
            for (col = 0; col < GetColumnCount(); col++)
            {
                int colWidth = GetColumnWidth(col);
                x += colWidth ;
                dc.DrawLine(x, firstItemRect.GetY() - 2, x, itemRect.GetBottom());
            }
        }
    }
}

// ----------------------------------------------------------------------------
// wxListItem
// ----------------------------------------------------------------------------

// List item structure
wxListItem::wxListItem()
{
    m_mask = 0;
    m_itemId = 0;
    m_col = 0;
    m_state = 0;
    m_stateMask = 0;
    m_image = 0;
    m_data = 0;

    m_format = wxLIST_FORMAT_CENTRE;
    m_width = 0;

    m_attr = NULL;
}

void wxListItem::Clear()
{
    m_mask = 0;
    m_itemId = 0;
    m_col = 0;
    m_state = 0;
    m_stateMask = 0;
    m_image = 0;
    m_data = 0;
    m_format = wxLIST_FORMAT_CENTRE;
    m_width = 0;
    m_text = wxEmptyString;

    if (m_attr) delete m_attr;
    m_attr = NULL;
}

void wxListItem::ClearAttributes()
{
    if (m_attr) delete m_attr;
    m_attr = NULL;
}

static void wxConvertFromMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& lvItem, HWND getFullInfo)
{
    info.m_data = lvItem.lParam;
    info.m_mask = 0;
    info.m_state = 0;
    info.m_stateMask = 0;
    info.m_itemId = lvItem.iItem;

    long oldMask = lvItem.mask;

    bool needText = FALSE;
    if (getFullInfo != 0)
    {
        if ( lvItem.mask & LVIF_TEXT )
            needText = FALSE;
        else
            needText = TRUE;

        if ( needText )
        {
            lvItem.pszText = new wxChar[513];
            lvItem.cchTextMax = 512;
        }
        //    lvItem.mask |= TVIF_HANDLE | TVIF_STATE | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
        lvItem.mask |= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        ::SendMessage(getFullInfo, LVM_GETITEM, 0, (LPARAM)& lvItem);
    }

    if ( lvItem.mask & LVIF_STATE )
    {
        info.m_mask |= wxLIST_MASK_STATE;

        if ( lvItem.stateMask & LVIS_CUT)
        {
            info.m_stateMask |= wxLIST_STATE_CUT;
            if ( lvItem.state & LVIS_CUT )
                info.m_state |= wxLIST_STATE_CUT;
        }
        if ( lvItem.stateMask & LVIS_DROPHILITED)
        {
            info.m_stateMask |= wxLIST_STATE_DROPHILITED;
            if ( lvItem.state & LVIS_DROPHILITED )
                info.m_state |= wxLIST_STATE_DROPHILITED;
        }
        if ( lvItem.stateMask & LVIS_FOCUSED)
        {
            info.m_stateMask |= wxLIST_STATE_FOCUSED;
            if ( lvItem.state & LVIS_FOCUSED )
                info.m_state |= wxLIST_STATE_FOCUSED;
        }
        if ( lvItem.stateMask & LVIS_SELECTED)
        {
            info.m_stateMask |= wxLIST_STATE_SELECTED;
            if ( lvItem.state & LVIS_SELECTED )
                info.m_state |= wxLIST_STATE_SELECTED;
        }
    }

    if ( lvItem.mask & LVIF_TEXT )
    {
        info.m_mask |= wxLIST_MASK_TEXT;
        info.m_text = lvItem.pszText;
    }
    if ( lvItem.mask & LVIF_IMAGE )
    {
        info.m_mask |= wxLIST_MASK_IMAGE;
        info.m_image = lvItem.iImage;
    }
    if ( lvItem.mask & LVIF_PARAM )
        info.m_mask |= wxLIST_MASK_DATA;
    if ( lvItem.mask & LVIF_DI_SETITEM )
        info.m_mask |= wxLIST_SET_ITEM;
    info.m_col = lvItem.iSubItem;

    if (needText)
    {
        if (lvItem.pszText)
            delete[] lvItem.pszText;
    }
    lvItem.mask = oldMask;
}

static void wxConvertToMSWListItem(const wxListCtrl *ctrl, wxListItem& info, LV_ITEM& lvItem)
{
    lvItem.iItem = (int) info.m_itemId;

    lvItem.iImage = info.m_image;
    lvItem.lParam = info.m_data;
    lvItem.stateMask = 0;
    lvItem.state = 0;
    lvItem.mask = 0;
    lvItem.iSubItem = info.m_col;

    if (info.m_mask & wxLIST_MASK_STATE)
    {
        lvItem.mask |= LVIF_STATE;
        if (info.m_stateMask & wxLIST_STATE_CUT)
        {
            lvItem.stateMask |= LVIS_CUT;
            if (info.m_state & wxLIST_STATE_CUT)
                lvItem.state |= LVIS_CUT;
        }
        if (info.m_stateMask & wxLIST_STATE_DROPHILITED)
        {
            lvItem.stateMask |= LVIS_DROPHILITED;
            if (info.m_state & wxLIST_STATE_DROPHILITED)
                lvItem.state |= LVIS_DROPHILITED;
        }
        if (info.m_stateMask & wxLIST_STATE_FOCUSED)
        {
            lvItem.stateMask |= LVIS_FOCUSED;
            if (info.m_state & wxLIST_STATE_FOCUSED)
                lvItem.state |= LVIS_FOCUSED;
        }
        if (info.m_stateMask & wxLIST_STATE_SELECTED)
        {
            lvItem.stateMask |= LVIS_SELECTED;
            if (info.m_state & wxLIST_STATE_SELECTED)
                lvItem.state |= LVIS_SELECTED;
        }
    }

    if (info.m_mask & wxLIST_MASK_TEXT)
    {
        lvItem.mask |= LVIF_TEXT;
        if ( ctrl->GetWindowStyleFlag() & wxLC_USER_TEXT )
        {
            lvItem.pszText = LPSTR_TEXTCALLBACK;
        }
        else
        {
            lvItem.pszText = WXSTRINGCAST info.m_text;
            if ( lvItem.pszText )
                lvItem.cchTextMax = info.m_text.Length();
            else
                lvItem.cchTextMax = 0;
        }
    }
    if (info.m_mask & wxLIST_MASK_IMAGE)
        lvItem.mask |= LVIF_IMAGE;
    if (info.m_mask & wxLIST_MASK_DATA)
        lvItem.mask |= LVIF_PARAM;
}

// ----------------------------------------------------------------------------
// List event
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent)

wxListEvent::wxListEvent(wxEventType commandType, int id)
           : wxNotifyEvent(commandType, id)
{
    m_code = 0;
    m_itemIndex = 0;
    m_oldItemIndex = 0;
    m_col = 0;
    m_cancelled = FALSE;
}

#endif // __WIN95__

