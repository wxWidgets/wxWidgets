/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/listctrl.cpp
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

#if wxUSE_LISTCTRL && defined(__WIN95__)

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/textctrl.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/dcclient.h"

#include "wx/msw/private.h"

#if ((defined(__GNUWIN32_OLD__) || defined(__TWIN32__)) && !defined(__CYGWIN10__))
    #include "wx/msw/gnuwin32/extra.h"
#else
    #include <commctrl.h>
#endif

#include "wx/msw/missing.h"


// ----------------------------------------------------------------------------
// private globals (yuck!)
// ----------------------------------------------------------------------------

// Some versions of comctl32.dll don't do what MSDN says it should and still
// sends LVN_DELETEITEM after LVN_DELETEALLITEMS has returned TRUE.  This flag
// will be used to enable us to ignore the LVN_DELETEITEM message in these cases.
// Also note that sometimes when there are large numbers of items in the listctrl
// and items have attribute data then when the data is being deleted a
// LVN_ITEMCHANGING message can be sent that will have a bogus value, causing a
// memory fault.  This flag will also be used to ignore those change messages.
//
// 2.5 will have a better fix that avoids the use of a global.  It was unavoidable
// for 2.4.x because of binary compatibility concerns.
static bool gs_ignoreChangeDeleteItem = FALSE;


// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// convert our state and mask flags to LV_ITEM constants
static void wxConvertToMSWFlags(long state, long mask, LV_ITEM& lvItem);

// convert wxListItem to LV_ITEM
static void wxConvertToMSWListItem(const wxListCtrl *ctrl,
                                   const wxListItem& info, LV_ITEM& lvItem);

// convert LV_ITEM to wxListItem
static void wxConvertFromMSWListItem(HWND hwndListCtrl,
                                     wxListItem& info,
                                     /* const */ LV_ITEM& lvItem);

// convert our wxListItem to LV_COLUMN
static void wxConvertToMSWListCol(int col, const wxListItem& item,
                                  LV_COLUMN& lvCol);

// ----------------------------------------------------------------------------
// private helper classes
// ----------------------------------------------------------------------------

// We have to handle both fooW and fooA notifications in several cases
// because of broken commctl.dll and/or unicows.dll. This class is used to
// convert LV_ITEMA and LV_ITEMW to LV_ITEM (which is either LV_ITEMA or
// LV_ITEMW depending on wxUSE_UNICODE setting), so that it can be processed
// by wxConvertToMSWListItem().
class wxLV_ITEM
{
public:
    ~wxLV_ITEM() { delete m_buf; }
    operator LV_ITEM&() const { return *m_item; }

#if wxUSE_UNICODE
    wxLV_ITEM(LV_ITEMW &item) : m_buf(NULL), m_item(&item) {}
    wxLV_ITEM(LV_ITEMA &item)
    {
        m_item = new LV_ITEM((LV_ITEM&)item);
        if ( (item.mask & LVIF_TEXT) && item.pszText )
        {
            m_buf = new wxMB2WXbuf(wxConvLocal.cMB2WX(item.pszText));
            m_item->pszText = (wxChar*)m_buf->data();
        }
        else
            m_buf = NULL;
    }
private:
    wxMB2WXbuf *m_buf;

#else // !wxUSE_UNICODE
    wxLV_ITEM(LV_ITEMW &item)
    {
        m_item = new LV_ITEM((LV_ITEM&)item);

        // the code below doesn't compile without wxUSE_WCHAR_T and as I don't
        // know if it's useful to have it at all (do we ever get Unicode
        // notifications in ANSI mode? I don't think so...) I'm not going to
        // write alternative implementation right now
        //
        // but if it is indeed used, we should simply directly use
        // ::WideCharToMultiByte() here
#if wxUSE_WCHAR_T
        if ( (item.mask & LVIF_TEXT) && item.pszText )
        {
#ifdef __WXWINE__
            // FIXME
            m_buf = new wxWC2WXbuf(wxConvLocal.cWC2WX((const __wchar_t* ) item.pszText));
#else
            m_buf = new wxWC2WXbuf(wxConvLocal.cWC2WX(item.pszText));
#endif
            m_item->pszText = (wxChar*)m_buf->data();
        }
        else
#endif // wxUSE_WCHAR_T
            m_buf = NULL;
    }
    wxLV_ITEM(LV_ITEMA &item) : m_buf(NULL), m_item(&item) {}
private:
    wxWC2WXbuf *m_buf;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

    LV_ITEM *m_item;
};

///////////////////////////////////////////////////////
// Problem:
// The MSW version had problems with SetTextColour() et
// al as the wxListItemAttr's were stored keyed on the
// item index. If a item was inserted anywhere but the end
// of the list the the text attributes (colour etc) for
// the following items were out of sync.
//
// Solution:
// Under MSW the only way to associate data with a List
// item independant of its position in the list is to
// store a pointer to it in its lParam attribute. However
// user programs are already using this (via the
// SetItemData() GetItemData() calls).
//
// However what we can do is store a pointer to a
// structure which contains the attributes we want *and*
// a lParam for the users data, e.g.
//
// class wxListItemInternalData
// {
// public:
//   wxListItemAttr *attr;
//   long lParam; // user data
// };
//
// To conserve memory, a wxListItemInternalData is
// only allocated for a LV_ITEM if text attributes or
// user data(lparam) are being set.


// class wxListItemInternalData
class wxListItemInternalData
{
public:
   wxListItemAttr *attr;
   LPARAM lParam; // user data

   wxListItemInternalData() : attr(NULL), lParam(0) {}
   ~wxListItemInternalData()
   {
       if (attr)
           delete attr;
   };
};

// Get the internal data structure
static wxListItemInternalData *wxGetInternalData(HWND hwnd, long itemId);
static wxListItemInternalData *wxGetInternalData(wxListCtrl *ctl, long itemId);
static wxListItemAttr *wxGetInternalDataAttr(wxListCtrl *ctl, long itemId);
static void wxDeleteInternalData(wxListCtrl* ctl, long itemId);


// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_RDRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_END_LABEL_EDIT)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_GET_INFO)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_SET_INFO)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_DESELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_KEY_DOWN)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_INSERT_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_DRAGGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_END_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_ACTIVATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_FOCUSED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LIST_CACHE_HINT)

IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxListView, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent)

BEGIN_EVENT_TABLE(wxListCtrl, wxControl)
    EVT_PAINT(wxListCtrl::OnPaint)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListCtrl construction
// ----------------------------------------------------------------------------

void wxListCtrl::Init()
{
    m_imageListNormal = NULL;
    m_imageListSmall = NULL;
    m_imageListState = NULL;
    m_ownsImageListNormal = m_ownsImageListSmall = m_ownsImageListState = FALSE;
    m_baseStyle = 0;
    m_colCount = 0;
    m_textCtrl = NULL;
    m_AnyInternalData = FALSE;
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

    WXDWORD exStyle = 0;
    WXDWORD standardStyle = MSWGetStyle(GetWindowStyle(), & exStyle) ;

    long oldStyle = 0; // Dummy
    wstyle |= ConvertToMSWStyle(oldStyle, m_windowStyle);
    wstyle |= standardStyle;

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

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
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

void wxListCtrl::FreeAllInternalData()
{
    if (m_AnyInternalData)
    {
        int n = GetItemCount();
        int i = 0;

        for (i = 0; i < n; i++)
            wxDeleteInternalData(this, i);

        m_AnyInternalData = FALSE;
    }
}

wxListCtrl::~wxListCtrl()
{
    gs_ignoreChangeDeleteItem = TRUE;
    FreeAllInternalData();
    gs_ignoreChangeDeleteItem = FALSE;

    if ( m_textCtrl )
    {
        m_textCtrl->UnsubclassWin();
        m_textCtrl->SetHWND(0);
        delete m_textCtrl;
        m_textCtrl = NULL;
    }

    if (m_ownsImageListNormal) delete m_imageListNormal;
    if (m_ownsImageListSmall) delete m_imageListSmall;
    if (m_ownsImageListState) delete m_imageListState;
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

#if !( defined(__GNUWIN32__) && !wxCHECK_W32API_VERSION( 1, 0 ) )
    if ( style & wxLC_VIRTUAL )
    {
        int ver = wxTheApp->GetComCtl32Version();
        if ( ver < 470 )
        {
            wxLogWarning(_("Please install a newer version of comctl32.dll\n(at least version 4.70 is required but you have %d.%02d)\nor this program won't operate correctly."),
                        ver / 100, ver % 100);
        }

        wstyle |= LVS_OWNERDATA;
    }
#endif

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
    wxZeroMemory(lvCol);

    lvCol.mask = LVCF_WIDTH;

    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
        lvCol.mask |= LVCF_TEXT;
        lvCol.pszText = new wxChar[513];
        lvCol.cchTextMax = 512;
    }

    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        lvCol.mask |= LVCF_FMT;
    }

    if ( item.m_mask & wxLIST_MASK_IMAGE )
    {
        lvCol.mask |= LVCF_IMAGE;
    }

    bool success = ListView_GetColumn(GetHwnd(), col, &lvCol) != 0;

    //  item.m_subItem = lvCol.iSubItem;
    item.m_width = lvCol.cx;

    if ( (item.m_mask & wxLIST_MASK_TEXT) && lvCol.pszText )
    {
        item.m_text = lvCol.pszText;
        delete[] lvCol.pszText;
    }

    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        switch (lvCol.fmt & LVCFMT_JUSTIFYMASK) {
            case LVCFMT_LEFT:
                item.m_format = wxLIST_FORMAT_LEFT;
                break;
            case LVCFMT_RIGHT:
                item.m_format = wxLIST_FORMAT_RIGHT;
                break;
            case LVCFMT_CENTER:
                item.m_format = wxLIST_FORMAT_CENTRE;
                break;
            default:
                item.m_format = -1;  // Unknown?
                break;
        }
    }

#if _WIN32_IE >= 0x0300
    if ( item.m_mask & wxLIST_MASK_IMAGE )
    {
        item.m_image = lvCol.iImage;
    }
#endif

    return success;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
    LV_COLUMN lvCol;
    wxConvertToMSWListCol(col, item, lvCol);

    return ListView_SetColumn(GetHwnd(), col, &lvCol) != 0;
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

    return ListView_SetColumnWidth(GetHwnd(), col2, width2) != 0;
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

    if (info.m_mask & wxLIST_MASK_IMAGE)
        lvItem.mask |= LVIF_IMAGE;

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
        // give NULL as hwnd as we already have everything we need
        wxConvertFromMSWListItem(NULL, info, lvItem);
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

    // we never update the lParam if it contains our pointer
    // to the wxListItemInternalData structure
    item.mask &= ~LVIF_PARAM;

    // check if setting attributes or lParam
    if (info.HasAttributes() || (info.m_mask  & wxLIST_MASK_DATA))
    {
        // get internal item data
        // perhaps a cache here ?
        wxListItemInternalData *data = wxGetInternalData(this, info.m_itemId);

        if (! data)
        {
            // need to set it
            m_AnyInternalData = TRUE;
            data = new wxListItemInternalData();
            item.lParam = (LPARAM) data;
            item.mask |= LVIF_PARAM;
        };


        // user data
        if (info.m_mask  & wxLIST_MASK_DATA)
            data->lParam = info.m_data;

        // attributes
        if (info.HasAttributes())
        {
            if (data->attr)
                *data->attr = *info.GetAttributes();
            else
                data->attr = new wxListItemAttr(*info.GetAttributes());
        };
    };


    // we could be changing only the attribute in which case we don't need to
    // call ListView_SetItem() at all
    if ( item.mask )
    {
        item.cchTextMax = 0;
        if ( !ListView_SetItem(GetHwnd(), &item) )
        {
            wxLogDebug(_T("ListView_SetItem() failed"));

            return FALSE;
        }
    }

    // we need to update the item immediately to show the new image
    bool updateNow = (info.m_mask & wxLIST_MASK_IMAGE) != 0;

    // check whether it has any custom attributes
    if ( info.HasAttributes() )
    {
        m_hasAnyAttr = TRUE;

        // if the colour has changed, we must redraw the item
        updateNow = TRUE;
    }

    if ( updateNow )
    {
        // we need this to make the change visible right now
        RefreshItem(item.iItem);
    }

    return TRUE;
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
    // NB: don't use SetItem() here as it doesn't work with the virtual list
    //     controls
    LV_ITEM lvItem;
    wxZeroMemory(lvItem);

    wxConvertToMSWFlags(state, stateMask, lvItem);

    // for the virtual list controls we need to refresh the previously focused
    // item manually when changing focus without changing selection
    // programmatically because otherwise it keeps its focus rectangle until
    // next repaint (yet another comctl32 bug)
    long focusOld;
    if ( IsVirtual() &&
         (stateMask & wxLIST_STATE_FOCUSED) &&
         (state & wxLIST_STATE_FOCUSED) )
    {
        focusOld = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    }
    else
    {
        focusOld = -1;
    }

    if ( !::SendMessage(GetHwnd(), LVM_SETITEMSTATE,
                        (WPARAM)item, (LPARAM)&lvItem) )
    {
        wxLogLastError(_T("ListView_SetItemState"));

        return FALSE;
    }

    if ( focusOld != -1 )
    {
        // no need to refresh the item if it was previously selected, it would
        // only result in annoying flicker
        if ( !(GetItemState(focusOld,
                            wxLIST_STATE_SELECTED) & wxLIST_STATE_SELECTED) )
        {
            RefreshItem(focusOld);
        }
    }

    return TRUE;
}

// Sets the item image
bool wxListCtrl::SetItemImage(long item, int image, int WXUNUSED(selImage))
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
        return wxEmptyString;
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
    RECT rectWin;

    int codeWin;
    if ( code == wxLIST_RECT_BOUNDS )
        codeWin = LVIR_BOUNDS;
    else if ( code == wxLIST_RECT_ICON )
        codeWin = LVIR_ICON;
    else if ( code == wxLIST_RECT_LABEL )
        codeWin = LVIR_LABEL;
    else
    {
        wxFAIL_MSG( _T("incorrect code in GetItemRect()") );

        codeWin = LVIR_BOUNDS;
    }

    bool success = ListView_GetItemRect(GetHwnd(), (int) item, &rectWin, codeWin) != 0;

    rect.x = rectWin.left;
    rect.y = rectWin.top;
    rect.width = rectWin.right - rectWin.left;
    rect.height = rectWin.bottom - rectWin.top;

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

void wxListCtrl::SetItemTextColour( long item, const wxColour &col )
{
    wxListItem info;
    info.m_itemId = item;
    info.SetTextColour( col );
    SetItem( info );
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    GetItem( info );
    return info.GetTextColour();
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col )
{
    wxListItem info;
    info.m_itemId = item;
    info.SetBackgroundColour( col );
    SetItem( info );
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
    wxListItem info;
    info.m_itemId = item;
    GetItem( info );
    return info.GetBackgroundColour();
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
        if (m_ownsImageListNormal) delete m_imageListNormal;
        m_imageListNormal = imageList;
        m_ownsImageListNormal = FALSE;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        flags = LVSIL_SMALL;
        if (m_ownsImageListSmall) delete m_imageListSmall;
        m_imageListSmall = imageList;
        m_ownsImageListSmall = FALSE;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        flags = LVSIL_STATE;
        if (m_ownsImageListState) delete m_imageListState;
        m_imageListState = imageList;
        m_ownsImageListState = FALSE;
    }
    ListView_SetImageList(GetHwnd(), (HIMAGELIST) imageList ? imageList->GetHIMAGELIST() : 0, flags);
}

void wxListCtrl::AssignImageList(wxImageList *imageList, int which)
{
    SetImageList(imageList, which);
    if ( which == wxIMAGE_LIST_NORMAL )
        m_ownsImageListNormal = TRUE;
    else if ( which == wxIMAGE_LIST_SMALL )
        m_ownsImageListSmall = TRUE;
    else if ( which == wxIMAGE_LIST_STATE )
        m_ownsImageListState = TRUE;
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
    if ( !ListView_DeleteItem(GetHwnd(), (int) item) )
    {
        wxLogLastError(_T("ListView_DeleteItem"));
        return FALSE;
    }

    // the virtual list control doesn't refresh itself correctly, help it
    if ( IsVirtual() )
    {
        // we need to refresh all the lines below the one which was deleted
        wxRect rectItem;
        if ( item > 0 && GetItemCount() )
        {
            GetItemRect(item - 1, rectItem);
        }
        else
        {
            rectItem.y =
            rectItem.height = 0;
        }

        wxRect rectWin = GetRect();
        rectWin.height = rectWin.GetBottom() - rectItem.GetBottom();
        rectWin.y = rectItem.GetBottom();

        RefreshRect(rectWin);
    }

    return TRUE;
}

// Deletes all items
bool wxListCtrl::DeleteAllItems()
{
    gs_ignoreChangeDeleteItem = TRUE;
    FreeAllInternalData();
    bool rval = ListView_DeleteAllItems(GetHwnd()) != 0;
    gs_ignoreChangeDeleteItem = FALSE;
    return rval;
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

    // ListView_EditLabel requires that the list has focus.
    SetFocus();

    WXHWND hWnd = (WXHWND) ListView_EditLabel(GetHwnd(), item);
    if ( !hWnd )
    {
        // failed to start editing
        return NULL;
    }

    // [re]create the text control wrapping the HWND we got
    if ( m_textCtrl )
    {
        m_textCtrl->UnsubclassWin();
        m_textCtrl->SetHWND(0);
        delete m_textCtrl;
    }

    m_textCtrl = (wxTextCtrl *)textControlClass->CreateObject();
    m_textCtrl->SetHWND(hWnd);
    m_textCtrl->SubclassWin(hWnd);
    m_textCtrl->SetParent(this);

    // we must disallow TABbing away from the control while the edit contol is
    // shown because this leaves it in some strange state (just try removing
    // this line and then pressing TAB while editing an item in  listctrl
    // inside a panel)
    m_textCtrl->SetWindowStyle(m_textCtrl->GetWindowStyle() | wxTE_PROCESS_TAB);

    return m_textCtrl;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool WXUNUSED(cancel))
{
    wxFAIL_MSG( _T("not implemented") );

    return FALSE;
}

// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
    return ListView_EnsureVisible(GetHwnd(), (int) item, FALSE) != 0;
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
    // inconsistent with the generic version - so we adjust the index
    if (start != -1)
        start --;
    return ListView_FindItem(GetHwnd(), (int) start, &findInfo);
}

// Find an item whose data matches this data, starting from the item after 'start'
// or the beginning if 'start' is -1.
// NOTE : Lindsay Mathieson - 14-July-2002
//        No longer use ListView_FindItem as the data attribute is now stored
//        in a wxListItemInternalData structure refernced by the actual lParam
long wxListCtrl::FindItem(long start, long data)
{
    long  idx = start + 1;
    long count = GetItemCount();

    while (idx < count)
    {
        if (GetItemData(idx) == data)
            return idx;
        idx++;
    };

    return -1;
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
    wxASSERT_MSG( !IsVirtual(), _T("can't be used with virtual controls") );

    LV_ITEM item;
    wxConvertToMSWListItem(this, info, item);
    item.mask &= ~LVIF_PARAM;

    // check wether we need to allocate our internal data
    bool needInternalData = ((info.m_mask & wxLIST_MASK_DATA) || info.HasAttributes());
    if (needInternalData)
    {
        m_AnyInternalData = TRUE;
        item.mask |= LVIF_PARAM;

        // internal stucture that manages data
        wxListItemInternalData *data = new wxListItemInternalData();
        item.lParam = (LPARAM) data;

        if (info.m_mask & wxLIST_MASK_DATA)
            data->lParam = info.m_data;

    // check whether it has any custom attributes
    if ( info.HasAttributes() )
    {
            // take copy of attributes
            data->attr = new wxListItemAttr(*info.GetAttributes());
    }
    };


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
    wxConvertToMSWListCol(col, item, lvCol);

    if ( !(lvCol.mask & LVCF_WIDTH) )
    {
        // always give some width to the new column: this one is compatible
        // with the generic version
        lvCol.mask |= LVCF_WIDTH;
        lvCol.cx = 80;
    }

    // NB: this is wrong, according to the docs we should return the index of
    //     the new column and not a bool, but don't change it in 2.4 branch
    //     as it's an incompatible change risking to break people's code
    bool success = ListView_InsertColumn(GetHwnd(), col, &lvCol) != -1;
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

// scroll the control by the given number of pixels (exception: in list view,
// dx is interpreted as number of columns)
bool wxListCtrl::ScrollList(int dx, int dy)
{
    if ( !ListView_Scroll(GetHwnd(), dx, dy) )
    {
        wxLogDebug(_T("ListView_Scroll(%d, %d) failed"), dx, dy);

        return FALSE;
    }

    return TRUE;
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

// Internal structures for proxying the user compare function
// so that we can pass it the *real* user data

// translate lParam data and call user func
struct wxInternalDataSort
{
    wxListCtrlCompare user_fn;
    long data;
};

int CALLBACK wxInternalDataCompareFunc(LPARAM lParam1, LPARAM lParam2,  LPARAM lParamSort)
{
    struct wxInternalDataSort *internalData = (struct wxInternalDataSort *) lParamSort;

    wxListItemInternalData *data1 = (wxListItemInternalData *) lParam1;
    wxListItemInternalData *data2 = (wxListItemInternalData *) lParam2;

    long d1 = (data1 == NULL ? 0 : data1->lParam);
    long d2 = (data2 == NULL ? 0 : data2->lParam);

    return internalData->user_fn(d1, d2, internalData->data);

};

bool wxListCtrl::SortItems(wxListCtrlCompare fn, long data)
{
    struct wxInternalDataSort internalData;
    internalData.user_fn = fn;
    internalData.data = data;

    // WPARAM cast is needed for mingw/cygwin
    if ( !ListView_SortItems(GetHwnd(),
                             wxInternalDataCompareFunc,
                             (WPARAM) &internalData) )
    {
        wxLogDebug(_T("ListView_SortItems() failed"));

        return FALSE;
    }

    return TRUE;
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
    event.SetEventObject(this);

    wxEventType eventType = wxEVT_NULL;

   NMHDR *nmhdr = (NMHDR *)lParam;

    // if your compiler is as broken as this, you should really change it: this
    // code is needed for normal operation! #ifdef below is only useful for
    // automatic rebuilds which are done with a very old compiler version
#ifdef HDN_BEGINTRACKA

    // check for messages from the header (in report view)
    HWND hwndHdr = ListView_GetHeader(GetHwnd());

    // is it a message from the header?
    if ( nmhdr->hwndFrom == hwndHdr )
    {
        HD_NOTIFY *nmHDR = (HD_NOTIFY *)nmhdr;

        event.m_itemIndex = -1;

        switch ( nmhdr->code )
        {
            // yet another comctl32.dll bug: under NT/W2K it sends Unicode
            // TRACK messages even to ANSI programs: on my system I get
            // HDN_BEGINTRACKW and HDN_ENDTRACKA and no HDN_TRACK at all!
            //
            // work around is to simply catch both versions and hope that it
            // works (why should this message exist in ANSI and Unicode is
            // beyond me as it doesn't deal with strings at all...)
            //
            // note that fr HDN_TRACK another possibility could be to use
            // HDN_ITEMCHANGING but it is sent even after HDN_ENDTRACK and when
            // something other than the item width changes so we'd have to
            // filter out the unwanted events then
            case HDN_BEGINTRACKA:
            case HDN_BEGINTRACKW:
                eventType = wxEVT_COMMAND_LIST_COL_BEGIN_DRAG;
                // fall through

            case HDN_TRACKA:
            case HDN_TRACKW:
                if ( eventType == wxEVT_NULL )
                    eventType = wxEVT_COMMAND_LIST_COL_DRAGGING;
                // fall through

            case HDN_ENDTRACKA:
            case HDN_ENDTRACKW:
                if ( eventType == wxEVT_NULL )
                    eventType = wxEVT_COMMAND_LIST_COL_END_DRAG;

                event.m_item.m_width = nmHDR->pitem->cxy;
                event.m_col = nmHDR->iItem;
                break;

            case NM_RCLICK:
                {
                    eventType = wxEVT_COMMAND_LIST_COL_RIGHT_CLICK;
                    event.m_col = -1;

                    // find the column clicked: we have to search for it
                    // ourselves as the notification message doesn't provide
                    // this info

                    // where did the click occur?
                    POINT ptClick;
                    if ( !::GetCursorPos(&ptClick) )
                    {
                        wxLogLastError(_T("GetCursorPos"));
                    }

                    if ( !::ScreenToClient(hwndHdr, &ptClick) )
                    {
                        wxLogLastError(_T("ScreenToClient(listctrl header)"));
                    }

                    event.m_pointDrag.x = ptClick.x;
                    event.m_pointDrag.y = ptClick.y;

                    int colCount = Header_GetItemCount(hwndHdr);

                    RECT rect;
                    for ( int col = 0; col < colCount; col++ )
                    {
                        if ( Header_GetItemRect(hwndHdr, col, &rect) )
                        {
                            if ( ::PtInRect(&rect, ptClick) )
                            {
                                event.m_col = col;
                                break;
                            }
                        }
                    }
                }
                break;

            case HDN_GETDISPINFOW:
                {
                    LPNMHDDISPINFOW info = (LPNMHDDISPINFOW) lParam;
                    // This is a fix for a strange bug under XP.
                    // Normally, info->iItem is a valid index, but
                    // sometimes this is a silly (large) number
                    // and when we return FALSE via wxControl::MSWOnNotify
                    // to indicate that it hasn't yet been processed,
                    // there's a GPF in Windows.
                    // By returning TRUE here, we avoid further processing
                    // of this strange message.
                    if ( info->iItem >= GetColumnCount() )
                        return TRUE;
                }
                // fall through

            default:
                return wxControl::MSWOnNotify(idCtrl, lParam, result);
        }
    }
    else
#endif // defined(HDN_BEGINTRACKA)
    if ( nmhdr->hwndFrom == GetHwnd() )
    {
        // almost all messages use NM_LISTVIEW
        NM_LISTVIEW *nmLV = (NM_LISTVIEW *)nmhdr;

        const int iItem = nmLV->iItem;


        // FreeAllInternalData will cause LVN_ITEMCHANG* messages, which can be
        // ignored for efficiency.  It is done here because the internal data is in the
        // process of being deleted so we don't want to try and access it below.
        if ( gs_ignoreChangeDeleteItem &&
             ( (nmLV->hdr.code == LVN_ITEMCHANGED) || (nmLV->hdr.code == LVN_ITEMCHANGING)))
        {
            return TRUE;
        }


        // If we have a valid item then check if there is a data value
        // associated with it and put it in the event.
        if ( iItem >= 0 && iItem < GetItemCount() )
        {
            wxListItemInternalData *internaldata =
                wxGetInternalData(GetHwnd(), iItem);

            if ( internaldata )
                event.m_item.m_data = internaldata->lParam;
        }


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

                event.m_itemIndex = iItem;
                event.m_pointDrag.x = nmLV->ptAction.x;
                event.m_pointDrag.y = nmLV->ptAction.y;
                break;

            // NB: we have to handle both *A and *W versions here because some
            //    versions of comctl32.dll send ANSI message to an Unicode app
            case LVN_BEGINLABELEDITA:
                {
                    eventType = wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT;
                    wxLV_ITEM item(((LV_DISPINFOA *)lParam)->item);
                    wxConvertFromMSWListItem(GetHwnd(), event.m_item, item);
                    event.m_itemIndex = event.m_item.m_itemId;
                }
                break;
            case LVN_BEGINLABELEDITW:
                {
                    eventType = wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT;
                    wxLV_ITEM item(((LV_DISPINFOW *)lParam)->item);
                    wxConvertFromMSWListItem(GetHwnd(), event.m_item, item);
                    event.m_itemIndex = event.m_item.m_itemId;
                }
                break;

            case LVN_ENDLABELEDITA:
                {
                    eventType = wxEVT_COMMAND_LIST_END_LABEL_EDIT;
                    wxLV_ITEM item(((LV_DISPINFOA *)lParam)->item);
                    wxConvertFromMSWListItem(NULL, event.m_item, item);
                    if ( ((LV_ITEM)item).pszText == NULL ||
                         ((LV_ITEM)item).iItem == -1 )
                    {
                        // don't keep a stale wxTextCtrl around
                        if ( m_textCtrl )
                        {
                            // EDIT control will be deleted by the list control itself so
                            // prevent us from deleting it as well
                            m_textCtrl->UnsubclassWin();
                            m_textCtrl->SetHWND(0);
                            delete m_textCtrl;
                            m_textCtrl = NULL;
                        }
                        return FALSE;
                    }

                    event.m_itemIndex = event.m_item.m_itemId;
                }
                break;
            case LVN_ENDLABELEDITW:
                {
                    eventType = wxEVT_COMMAND_LIST_END_LABEL_EDIT;
                    wxLV_ITEM item(((LV_DISPINFOW *)lParam)->item);
                    wxConvertFromMSWListItem(NULL, event.m_item, item);
                    if ( ((LV_ITEM)item).pszText == NULL ||
                         ((LV_ITEM)item).iItem == -1 )
                    {
                        // don't keep a stale wxTextCtrl around
                        if ( m_textCtrl )
                        {
                            // EDIT control will be deleted by the list control itself so
                            // prevent us from deleting it as well
                            m_textCtrl->UnsubclassWin();
                            m_textCtrl->SetHWND(0);
                            delete m_textCtrl;
                            m_textCtrl = NULL;
                        }
                        return FALSE;
                    }

                    event.m_itemIndex = event.m_item.m_itemId;
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
                break;

            case LVN_DELETEITEM:
                if ( gs_ignoreChangeDeleteItem )
                    return FALSE;

                eventType = wxEVT_COMMAND_LIST_DELETE_ITEM;
                event.m_itemIndex = iItem;
                // delete the assoicated internal data
                wxDeleteInternalData(this, iItem);
                break;

            case LVN_SETDISPINFO:
                {
                    eventType = wxEVT_COMMAND_LIST_SET_INFO;
                    LV_DISPINFO *info = (LV_DISPINFO *)lParam;
                    wxConvertFromMSWListItem(GetHwnd(), event.m_item, info->item);
                }
                break;

            case LVN_INSERTITEM:
                eventType = wxEVT_COMMAND_LIST_INSERT_ITEM;
                event.m_itemIndex = iItem;
                break;

            case LVN_ITEMCHANGED:
                // we translate this catch all message into more interesting
                // (and more easy to process) wxWindows events

                // first of all, we deal with the state change events only and
                // only for valid items (item == -1 for the virtual list
                // control)
                if ( nmLV->uChanged & LVIF_STATE && iItem != -1 )
                {
                    // temp vars for readability
                    const UINT stOld = nmLV->uOldState;
                    const UINT stNew = nmLV->uNewState;

                    event.m_item.SetId(iItem);
                    event.m_item.SetMask(wxLIST_MASK_TEXT |
                                         wxLIST_MASK_IMAGE |
                                         wxLIST_MASK_DATA);
                    GetItem(event.m_item);

                    // has the focus changed?
                    if ( !(stOld & LVIS_FOCUSED) && (stNew & LVIS_FOCUSED) )
                    {
                        eventType = wxEVT_COMMAND_LIST_ITEM_FOCUSED;
                        event.m_itemIndex = iItem;
                    }

                    if ( (stNew & LVIS_SELECTED) != (stOld & LVIS_SELECTED) )
                    {
                        if ( eventType != wxEVT_NULL )
                        {
                            // focus and selection have both changed: send the
                            // focus event from here and the selection one
                            // below
                            event.SetEventType(eventType);
                            (void)GetEventHandler()->ProcessEvent(event);
                        }
                        else // no focus event to send
                        {
                            // then need to set m_itemIndex as it wasn't done
                            // above
                            event.m_itemIndex = iItem;
                        }

                        eventType = stNew & LVIS_SELECTED
                                        ? wxEVT_COMMAND_LIST_ITEM_SELECTED
                                        : wxEVT_COMMAND_LIST_ITEM_DESELECTED;
                    }
                }

                if ( eventType == wxEVT_NULL )
                {
                    // not an interesting event for us
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

                    // <Enter> or <Space> activate the selected item if any (but
                    // not with Shift and/or Ctrl as then they have a predefined
                    // meaning for the list view)
                    if ( lItem != -1 &&
                         (wVKey == VK_RETURN || wVKey == VK_SPACE) &&
                         !(wxIsShiftDown() || wxIsCtrlDown()) )
                    {
                        eventType = wxEVT_COMMAND_LIST_ITEM_ACTIVATED;
                    }
                    else
                    {
                        eventType = wxEVT_COMMAND_LIST_KEY_DOWN;

                        // wxCharCodeMSWToWX() returns 0 if the key is an ASCII
                        // value which should be used as is
                        int code = wxCharCodeMSWToWX(wVKey);
                        event.m_code = code ? code : wVKey;
                    }

                    event.m_itemIndex =
                    event.m_item.m_itemId = lItem;

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
                if ( iItem == -1 )
                {
                    // not on item
                    return FALSE;
                }

                eventType = wxEVT_COMMAND_LIST_ITEM_ACTIVATED;
                event.m_itemIndex = iItem;
                event.m_item.m_text = GetItemText(iItem);
                event.m_item.m_data = GetItemData(iItem);
                break;

            case NM_RCLICK:
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
                        event.m_pointDrag.x = lvhti.pt.x;
                        event.m_pointDrag.y = lvhti.pt.y;
                    }
                }
                break;

#if defined(_WIN32_IE) && _WIN32_IE >= 0x300 \
        && !( defined(__GNUWIN32__) && !wxCHECK_W32API_VERSION( 1, 0 ) )
            case NM_CUSTOMDRAW:
                *result = OnCustomDraw(lParam);

                return TRUE;
#endif // _WIN32_IE >= 0x300

            case LVN_ODCACHEHINT:
                {
                    const NM_CACHEHINT *cacheHint = (NM_CACHEHINT *)lParam;

                    eventType = wxEVT_COMMAND_LIST_CACHE_HINT;

                    // we get some really stupid cache hints like ones for items in
                    // range 0..0 for an empty control or, after deleting an item,
                    // for items in invalid range - filter this garbage out
                    if ( cacheHint->iFrom < cacheHint->iTo )
                    {
                        event.m_oldItemIndex = cacheHint->iFrom;

                        long iMax = GetItemCount();
                        event.m_itemIndex = cacheHint->iTo < iMax ? cacheHint->iTo
                                                                  : iMax - 1;
                    }
                    else
                    {
                        return FALSE;
                    }
                }
                break;

            case LVN_GETDISPINFO:
                if ( IsVirtual() )
                {
                    LV_DISPINFO *info = (LV_DISPINFO *)lParam;

                    LV_ITEM& lvi = info->item;
                    long item = lvi.iItem;

                    if ( lvi.mask & LVIF_TEXT )
                    {
                        wxString text = OnGetItemText(item, lvi.iSubItem);
                        wxStrncpy(lvi.pszText, text, lvi.cchTextMax);
                    }

#if defined(_WIN32_IE) && _WIN32_IE >= 0x300 \
        && !( defined(__GNUWIN32__) && !wxCHECK_W32API_VERSION( 1, 1 ) )
                    if ( lvi.mask & LVIF_IMAGE )
                    {
                        lvi.iImage = OnGetItemImage(item);
                    }
#endif

                    // a little dose of healthy paranoia: as we never use
                    // LVM_SETCALLBACKMASK we're not supposed to get these ones
                    wxASSERT_MSG( !(lvi.mask & LVIF_STATE),
                                  _T("we don't support state callbacks yet!") );

                    return TRUE;
                }
                // fall through

            default:
                return wxControl::MSWOnNotify(idCtrl, lParam, result);
        }
    }
    else
    {
        // where did this one come from?
        return FALSE;
    }

    // process the event
    // -----------------

    event.SetEventType(eventType);

    bool processed = GetEventHandler()->ProcessEvent(event);

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

        case LVN_ENDLABELEDITA:
        case LVN_ENDLABELEDITW:
            // logic here is inversed compared to all the other messages
            *result = event.IsAllowed();

            // don't keep a stale wxTextCtrl around
            if ( m_textCtrl )
            {
                // EDIT control will be deleted by the list control itself so
                // prevent us from deleting it as well
                m_textCtrl->UnsubclassWin();
                m_textCtrl->SetHWND(0);
                delete m_textCtrl;
                m_textCtrl = NULL;
            }

            return TRUE;
    }

    if ( processed )
        *result = !event.IsAllowed();

    return processed;
}

#if defined(_WIN32_IE) && _WIN32_IE >= 0x300

WXLPARAM wxListCtrl::OnCustomDraw(WXLPARAM lParam)
{
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
    NMCUSTOMDRAW& nmcd = lplvcd->nmcd;
    switch ( nmcd.dwDrawStage )
    {
        case CDDS_PREPAINT:
            // if we've got any items with non standard attributes,
            // notify us before painting each item
            //
            // for virtual controls, always suppose that we have attributes as
            // there is no way to check for this
            return IsVirtual() || m_hasAnyAttr ? CDRF_NOTIFYITEMDRAW
                                               : CDRF_DODEFAULT;

        case CDDS_ITEMPREPAINT:
            {
                size_t item = (size_t)nmcd.dwItemSpec;
                if ( item >= (size_t)GetItemCount() )
                {
                    // we get this message with item == 0 for an empty control,
                    // we must ignore it as calling OnGetItemAttr() would be
                    // wrong
                    return CDRF_DODEFAULT;
                }

                wxListItemAttr *attr =
                    IsVirtual() ? OnGetItemAttr(item)
                                : wxGetInternalDataAttr(this, item);

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

                lplvcd->clrText = wxColourToRGB(colText);
                lplvcd->clrTextBk = wxColourToRGB(colBack);

                // note that if we wanted to set colours for
                // individual columns (subitems), we would have
                // returned CDRF_NOTIFYSUBITEMREDRAW from here
                if ( hFont )
                {
                    ::SelectObject(nmcd.hdc, hFont);

                    return CDRF_NEWFONT;
                }
            }
            // fall through to return CDRF_DODEFAULT

        default:
            return CDRF_DODEFAULT;
    }
}

#endif // NM_CUSTOMDRAW supported

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

    wxPen pen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
    dc.SetPen(pen);
    dc.SetBrush(* wxTRANSPARENT_BRUSH);

    wxSize clientSize = GetClientSize();
    wxRect itemRect;
    int cy=0;

    int itemCount = GetItemCount();
    int i;
    if (drawHRules)
    {
        long top = GetTopItem();
        for (i = top; i < top + GetCountPerPage() + 1; i++)
        {
            if (GetItemRect(i, itemRect))
            {
                cy = itemRect.GetTop();
                if (i != 0) // Don't draw the first one
                {
                    dc.DrawLine(0, cy, clientSize.x, cy);
                }
                // Draw last line
                if (i == itemCount - 1)
                {
                    cy = itemRect.GetBottom();
                    dc.DrawLine(0, cy, clientSize.x, cy);
                }
            }
        }
    }
    i = itemCount - 1;
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
                dc.DrawLine(x-1, firstItemRect.GetY() - 2, x-1, itemRect.GetBottom());
            }
        }
    }
}

// ----------------------------------------------------------------------------
// virtual list controls
// ----------------------------------------------------------------------------

wxString wxListCtrl::OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG( _T("wxListCtrl::OnGetItemText not supposed to be called") );

    return wxEmptyString;
}

int wxListCtrl::OnGetItemImage(long WXUNUSED(item)) const
{
    // same as above
    wxFAIL_MSG( _T("wxListCtrl::OnGetItemImage not supposed to be called") );

    return -1;
}

wxListItemAttr *wxListCtrl::OnGetItemAttr(long WXUNUSED_UNLESS_DEBUG(item)) const
{
    wxASSERT_MSG( item >= 0 && item < GetItemCount(),
                  _T("invalid item index in OnGetItemAttr()") );

    // no attributes by default
    return NULL;
}

void wxListCtrl::SetItemCount(long count)
{
    wxASSERT_MSG( IsVirtual(), _T("this is for virtual controls only") );

    if ( !::SendMessage(GetHwnd(), LVM_SETITEMCOUNT, (WPARAM)count, LVSICF_NOSCROLL) )
    {
        wxLogLastError(_T("ListView_SetItemCount"));
    }
}

void wxListCtrl::RefreshItem(long item)
{
    // strangely enough, ListView_Update() results in much more flicker here
    // than a dumb Refresh() -- why?
#if 0
    if ( !ListView_Update(GetHwnd(), item) )
    {
        wxLogLastError(_T("ListView_Update"));
    }
#else // 1
    wxRect rect;
    GetItemRect(item, rect);
    RefreshRect(rect);
#endif // 0/1
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
    wxRect rect1, rect2;
    GetItemRect(itemFrom, rect1);
    GetItemRect(itemTo, rect2);

    wxRect rect = rect1;
    rect.height = rect2.GetBottom() - rect1.GetTop();

    RefreshRect(rect);
}

static wxListItemInternalData *wxGetInternalData(HWND hwnd, long itemId)
{
    LV_ITEM it;
    it.mask = LVIF_PARAM;
    it.iItem = itemId;

    bool success = ListView_GetItem(hwnd, &it) != 0;
    if (success)
        return (wxListItemInternalData *) it.lParam;
    else
        return NULL;
};

static wxListItemInternalData *wxGetInternalData(wxListCtrl *ctl, long itemId)
{
    return wxGetInternalData((HWND) ctl->GetHWND(), itemId);
};

static wxListItemAttr *wxGetInternalDataAttr(wxListCtrl *ctl, long itemId)
{
    wxListItemInternalData *data = wxGetInternalData(ctl, itemId);
    if (data)
        return data->attr;
    else
        return NULL;
};

static void wxDeleteInternalData(wxListCtrl* ctl, long itemId)
{
    wxListItemInternalData *data = wxGetInternalData(ctl, itemId);
    if (data)
    {
        LV_ITEM item;
        memset(&item, 0, sizeof(item));
        item.iItem = itemId;
        item.mask = LVIF_PARAM;
        item.lParam = (LPARAM) 0;
        ListView_SetItem((HWND)ctl->GetHWND(), &item);
        delete data;
    }
}

static void wxConvertFromMSWListItem(HWND hwndListCtrl,
                                     wxListItem& info,
                                     LV_ITEM& lvItem)
{
    wxListItemInternalData *internaldata =
        (wxListItemInternalData *) lvItem.lParam;

    if (internaldata)
        info.m_data = internaldata->lParam;

    info.m_mask = 0;
    info.m_state = 0;
    info.m_stateMask = 0;
    info.m_itemId = lvItem.iItem;

    long oldMask = lvItem.mask;

    bool needText = FALSE;
    if (hwndListCtrl != 0)
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
        lvItem.mask |= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        ::SendMessage(hwndListCtrl, LVM_GETITEM, 0, (LPARAM)& lvItem);
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

static void wxConvertToMSWFlags(long state, long stateMask, LV_ITEM& lvItem)
{
    if (stateMask & wxLIST_STATE_CUT)
    {
        lvItem.stateMask |= LVIS_CUT;
        if (state & wxLIST_STATE_CUT)
            lvItem.state |= LVIS_CUT;
    }
    if (stateMask & wxLIST_STATE_DROPHILITED)
    {
        lvItem.stateMask |= LVIS_DROPHILITED;
        if (state & wxLIST_STATE_DROPHILITED)
            lvItem.state |= LVIS_DROPHILITED;
    }
    if (stateMask & wxLIST_STATE_FOCUSED)
    {
        lvItem.stateMask |= LVIS_FOCUSED;
        if (state & wxLIST_STATE_FOCUSED)
            lvItem.state |= LVIS_FOCUSED;
    }
    if (stateMask & wxLIST_STATE_SELECTED)
    {
        lvItem.stateMask |= LVIS_SELECTED;
        if (state & wxLIST_STATE_SELECTED)
            lvItem.state |= LVIS_SELECTED;
    }
}

static void wxConvertToMSWListItem(const wxListCtrl *ctrl,
                                   const wxListItem& info,
                                   LV_ITEM& lvItem)
{
    lvItem.iItem = (int) info.m_itemId;

    lvItem.iImage = info.m_image;
    lvItem.stateMask = 0;
    lvItem.state = 0;
    lvItem.mask = 0;
    lvItem.iSubItem = info.m_col;

    if (info.m_mask & wxLIST_MASK_STATE)
    {
        lvItem.mask |= LVIF_STATE;

        wxConvertToMSWFlags(info.m_state, info.m_stateMask, lvItem);
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
            // pszText is not const, hence the cast
            lvItem.pszText = (wxChar *)info.m_text.c_str();
            if ( lvItem.pszText )
                lvItem.cchTextMax = info.m_text.Length();
            else
                lvItem.cchTextMax = 0;
        }
    }
    if (info.m_mask & wxLIST_MASK_IMAGE)
        lvItem.mask |= LVIF_IMAGE;
}

static void wxConvertToMSWListCol(int WXUNUSED(col), const wxListItem& item,
                                  LV_COLUMN& lvCol)
{
    wxZeroMemory(lvCol);

    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
        lvCol.mask |= LVCF_TEXT;
        lvCol.pszText = (wxChar *)item.m_text.c_str(); // cast is safe
    }

    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
        lvCol.mask |= LVCF_FMT;

        if ( item.m_format == wxLIST_FORMAT_LEFT )
            lvCol.fmt = LVCFMT_LEFT;
        else if ( item.m_format == wxLIST_FORMAT_RIGHT )
            lvCol.fmt = LVCFMT_RIGHT;
        else if ( item.m_format == wxLIST_FORMAT_CENTRE )
            lvCol.fmt = LVCFMT_CENTER;
    }

    if ( item.m_mask & wxLIST_MASK_WIDTH )
    {
        lvCol.mask |= LVCF_WIDTH;
        if ( item.m_width == wxLIST_AUTOSIZE)
            lvCol.cx = LVSCW_AUTOSIZE;
        else if ( item.m_width == wxLIST_AUTOSIZE_USEHEADER)
            lvCol.cx = LVSCW_AUTOSIZE_USEHEADER;
        else
            lvCol.cx = item.m_width;
    }

#if defined(_WIN32_IE) && _WIN32_IE >= 0x300 \
        && !( defined(__GNUWIN32__) && !wxCHECK_W32API_VERSION( 1, 1 ) )
    if ( item.m_mask & wxLIST_MASK_IMAGE )
    {
        if ( wxTheApp->GetComCtl32Version() >= 470 )
        {
            lvCol.mask |= LVCF_IMAGE | LVCF_FMT;

            // we use LVCFMT_BITMAP_ON_RIGHT because thei mages on the right
            // seem to be generally nicer than on the left and the generic
            // version only draws them on the right (we don't have a flag to
            // specify the image location anyhow)
            //
            // we don't use LVCFMT_COL_HAS_IMAGES because it doesn't seem to
            // make any difference in my tests -- but maybe we should?
            lvCol.fmt |= LVCFMT_BITMAP_ON_RIGHT | LVCFMT_IMAGE;

            lvCol.iImage = item.m_image;
        }
        //else: it doesn't support item images anyhow
    }
#endif
}

#endif // wxUSE_LISTCTRL
