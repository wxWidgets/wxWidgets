///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listctrl.h
// Purpose:     wxListCtrl class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.12.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTCTRL_H_BASE_
#define _WX_LISTCTRL_H_BASE_

#ifdef __GNUG__
    #pragma interface "listctrlbase.h"
#endif

#if wxUSE_LISTCTRL

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// type of compare function for wxListCtrl sort operation
typedef int (wxCALLBACK *wxListCtrlCompare)(long item1, long item2, long sortData);

// ----------------------------------------------------------------------------
// wxListCtrl constants
// ----------------------------------------------------------------------------

// style flags
#define wxLC_VRULES          0x0001
#define wxLC_HRULES          0x0002

#define wxLC_ICON            0x0004
#define wxLC_SMALL_ICON      0x0008
#define wxLC_LIST            0x0010
#define wxLC_REPORT          0x0020

#define wxLC_ALIGN_TOP       0x0040
#define wxLC_ALIGN_LEFT      0x0080
#define wxLC_AUTOARRANGE     0x0100
#define wxLC_VIRTUAL         0x0200
#define wxLC_EDIT_LABELS     0x0400
#define wxLC_NO_HEADER       0x0800
#define wxLC_NO_SORT_HEADER  0x1000
#define wxLC_SINGLE_SEL      0x2000
#define wxLC_SORT_ASCENDING  0x4000
#define wxLC_SORT_DESCENDING 0x8000

#define wxLC_MASK_TYPE       (wxLC_ICON | wxLC_SMALL_ICON | wxLC_LIST | wxLC_REPORT)
#define wxLC_MASK_ALIGN      (wxLC_ALIGN_TOP | wxLC_ALIGN_LEFT)
#define wxLC_MASK_SORT       (wxLC_SORT_ASCENDING | wxLC_SORT_DESCENDING)

// for compatibility only
#define wxLC_USER_TEXT       wxLC_VIRTUAL

// Omitted because
//  (a) too much detail
//  (b) not enough style flags
//  (c) not implemented anyhow in the generic version
//
// #define wxLC_NO_SCROLL
// #define wxLC_NO_LABEL_WRAP
// #define wxLC_OWNERDRAW_FIXED
// #define wxLC_SHOW_SEL_ALWAYS

// Mask flags to tell app/GUI what fields of wxListItem are valid
#define wxLIST_MASK_STATE           0x0001
#define wxLIST_MASK_TEXT            0x0002
#define wxLIST_MASK_IMAGE           0x0004
#define wxLIST_MASK_DATA            0x0008
#define wxLIST_SET_ITEM             0x0010
#define wxLIST_MASK_WIDTH           0x0020
#define wxLIST_MASK_FORMAT          0x0040

// State flags for indicating the state of an item
#define wxLIST_STATE_DONTCARE       0x0000
#define wxLIST_STATE_DROPHILITED    0x0001      // MSW only
#define wxLIST_STATE_FOCUSED        0x0002
#define wxLIST_STATE_SELECTED       0x0004
#define wxLIST_STATE_CUT            0x0008      // MSW only

// Hit test flags, used in HitTest
#define wxLIST_HITTEST_ABOVE            0x0001  // Above the client area.
#define wxLIST_HITTEST_BELOW            0x0002  // Below the client area.
#define wxLIST_HITTEST_NOWHERE          0x0004  // In the client area but below the last item.
#define wxLIST_HITTEST_ONITEMICON       0x0020  // On the bitmap associated with an item.
#define wxLIST_HITTEST_ONITEMLABEL      0x0080  // On the label (string) associated with an item.
#define wxLIST_HITTEST_ONITEMRIGHT      0x0100  // In the area to the right of an item.
#define wxLIST_HITTEST_ONITEMSTATEICON  0x0200  // On the state icon for a tree view item that is in a user-defined state.
#define wxLIST_HITTEST_TOLEFT           0x0400  // To the left of the client area.
#define wxLIST_HITTEST_TORIGHT          0x0800  // To the right of the client area.

#define wxLIST_HITTEST_ONITEM (wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL | wxLIST_HITTEST_ONITEMSTATEICON)

// Flags for GetNextItem (MSW only except wxLIST_NEXT_ALL)
enum
{
    wxLIST_NEXT_ABOVE,          // Searches for an item above the specified item
    wxLIST_NEXT_ALL,            // Searches for subsequent item by index
    wxLIST_NEXT_BELOW,          // Searches for an item below the specified item
    wxLIST_NEXT_LEFT,           // Searches for an item to the left of the specified item
    wxLIST_NEXT_RIGHT           // Searches for an item to the right of the specified item
};

// Alignment flags for Arrange (MSW only except wxLIST_ALIGN_LEFT)
enum
{
    wxLIST_ALIGN_DEFAULT,
    wxLIST_ALIGN_LEFT,
    wxLIST_ALIGN_TOP,
    wxLIST_ALIGN_SNAP_TO_GRID
};

// Column format (MSW only except wxLIST_FORMAT_LEFT)
enum wxListColumnFormat
{
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER = wxLIST_FORMAT_CENTRE
};

// Autosize values for SetColumnWidth
enum
{
    wxLIST_AUTOSIZE = -1,
    wxLIST_AUTOSIZE_USEHEADER = -2      // partly supported by generic version
};

// Flag values for GetItemRect
enum
{
    wxLIST_RECT_BOUNDS,
    wxLIST_RECT_ICON,
    wxLIST_RECT_LABEL
};

// Flag values for FindItem (MSW only)
enum
{
    wxLIST_FIND_UP,
    wxLIST_FIND_DOWN,
    wxLIST_FIND_LEFT,
    wxLIST_FIND_RIGHT
};

// ----------------------------------------------------------------------------
// wxListItemAttr: a structure containing the visual attributes of an item
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListItemAttr
{
public:
    // ctors
    wxListItemAttr() { }
    wxListItemAttr(const wxColour& colText,
                   const wxColour& colBack,
                   const wxFont& font)
        : m_colText(colText), m_colBack(colBack), m_font(font) { }

    // setters
    void SetTextColour(const wxColour& colText) { m_colText = colText; }
    void SetBackgroundColour(const wxColour& colBack) { m_colBack = colBack; }
    void SetFont(const wxFont& font) { m_font = font; }

    // accessors
    bool HasTextColour() const { return m_colText.Ok(); }
    bool HasBackgroundColour() const { return m_colBack.Ok(); }
    bool HasFont() const { return m_font.Ok(); }

    const wxColour& GetTextColour() const { return m_colText; }
    const wxColour& GetBackgroundColour() const { return m_colBack; }
    const wxFont& GetFont() const { return m_font; }

private:
    wxColour m_colText,
             m_colBack;
    wxFont   m_font;
};

// ----------------------------------------------------------------------------
// wxListItem: the item or column info, used to exchange data with wxListCtrl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListItem : public wxObject
{
public:
    wxListItem();
    ~wxListItem() { delete m_attr; }

    // resetting
    void Clear();
    void ClearAttributes();

    // setters
    void SetMask(long mask) { m_mask = mask; }
    void SetId(long id) { m_itemId = id; }
    void SetColumn(int col) { m_col = col; }
    void SetState(long state) { m_state = state; m_stateMask |= state; }
    void SetStateMask(long stateMask) { m_stateMask = stateMask; }
    void SetText(const wxString& text) { m_text = text; }
    void SetImage(int image) { m_image = image; }
    void SetData(long data) { m_data = data; }
    void SetData(void *data) { m_data = (long)data; }

    void SetWidth(int width) { m_width = width; }
    void SetAlign(wxListColumnFormat align) { m_format = align; }

    void SetTextColour(const wxColour& colText)
        { Attributes().SetTextColour(colText); }
    void SetBackgroundColour(const wxColour& colBack)
        { Attributes().SetBackgroundColour(colBack); }
    void SetFont(const wxFont& font)
        { Attributes().SetFont(font); }

    // accessors
    long GetMask() const { return m_mask; }
    long GetId() const { return m_itemId; }
    int GetColumn() const { return m_col; }
    long GetState() const { return m_state & m_stateMask; }
    const wxString& GetText() const { return m_text; }
    int GetImage() const { return m_image; }
    long GetData() const { return m_data; }

    int GetWidth() const { return m_width; }
    wxListColumnFormat GetAlign() const { return (wxListColumnFormat)m_format; }

    wxListItemAttr *GetAttributes() const { return m_attr; }
    bool HasAttributes() const { return m_attr != NULL; }

    wxColour GetTextColour() const
        { return HasAttributes() ? m_attr->GetTextColour() : wxNullColour; }
    wxColour GetBackgroundColour() const
        { return HasAttributes() ? m_attr->GetBackgroundColour()
                                 : wxNullColour; }
    wxFont GetFont() const
        { return HasAttributes() ? m_attr->GetFont() : wxNullFont; }

    // this conversion is necessary to make old code using GetItem() to
    // compile
    operator long() const { return m_itemId; }

    // these members are public for compatibility

    long            m_mask;     // Indicates what fields are valid
    long            m_itemId;   // The zero-based item position
    int             m_col;      // Zero-based column, if in report mode
    long            m_state;    // The state of the item
    long            m_stateMask;// Which flags of m_state are valid (uses same flags)
    wxString        m_text;     // The label/header text
    int             m_image;    // The zero-based index into an image list
    long            m_data;     // App-defined data

    // For columns only
    int             m_format;   // left, right, centre
    int             m_width;    // width of column

protected:
    // creates m_attr if we don't have it yet
    wxListItemAttr& Attributes()
    {
        if ( !m_attr )
            m_attr = new wxListItemAttr;

        return *m_attr;
    }

    wxListItemAttr *m_attr;     // optional pointer to the items style

private:
    DECLARE_DYNAMIC_CLASS(wxListItem)
};

// ----------------------------------------------------------------------------
// include the wxListCtrl class declaration
// ----------------------------------------------------------------------------

#if defined(__WIN32__) && !defined(__WXUNIVERSAL__)
    #include "wx/msw/listctrl.h"
#else
    #include "wx/generic/listctrl.h"
#endif

// ----------------------------------------------------------------------------
// wxListView: a class which provides a better API for list control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListView : public wxListCtrl
{
public:
    wxListView() { }
    wxListView( wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = "listctrl" )
    {
        Create(parent, id, pos, size, style, wxDefaultValidator, name);
    }

    // focus/selection stuff
    // ---------------------

    // [de]select an item
    void Select(long n, bool on = TRUE)
    {
        SetItemState(n, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
    }

    // focus and show the given item
    void Focus(long index)
    {
        SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        EnsureVisible(index);
    }

    // get the currently focused item or -1 if none
    long GetFocusedItem() const
    {
        return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    }

    // get first and subsequent selected items, return -1 when no more
    long GetNextSelected(long item) const
        { return GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); }
    long GetFirstSelected() const
        { return GetNextSelected(-1); }

    // return TRUE if the item is selected
    bool IsSelected(long index)
        { return GetItemState(index, wxLIST_STATE_SELECTED) != 0; }

    // columns
    // -------

    void SetColumnImage(int col, int image)
    {
        wxListItem item;
        item.SetMask(wxLIST_MASK_IMAGE);
        item.SetImage(image);
        SetColumn(col, item);
    }

    void ClearColumnImage(int col) { SetColumnImage(col, -1); }

private:
    DECLARE_DYNAMIC_CLASS(wxListView)
};

// ----------------------------------------------------------------------------
// wxListEvent - the event class for the wxListCtrl notifications
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListEvent : public wxNotifyEvent
{
public:
    wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxNotifyEvent(commandType, id)
        {
            m_code = 0;
            m_itemIndex =
            m_oldItemIndex = 0;
            m_col = 0;
        }

    int GetCode() const { return m_code; }
    long GetIndex() const { return m_itemIndex; }
    int GetColumn() const { return m_col; }
    wxPoint GetPoint() const { return m_pointDrag; }
    const wxString& GetLabel() const { return m_item.m_text; }
    const wxString& GetText() const { return m_item.m_text; }
    int GetImage() const { return m_item.m_image; }
    long GetData() const { return m_item.m_data; }
    long GetMask() const { return m_item.m_mask; }
    const wxListItem& GetItem() const { return m_item; }

    // for wxEVT_COMMAND_LIST_CACHE_HINT only
    long GetCacheFrom() const { return m_oldItemIndex; }
    long GetCacheTo() const { return m_itemIndex; }

    // these methods don't do anything at all
#if WXWIN_COMPATIBILITY_2_2
    long GetOldIndex() const { return 0; }
    long GetOldItem() const { return 0; }
#endif // WXWIN_COMPATIBILITY_2_2

    virtual wxEvent *Clone() const { return new wxListEvent(*this); }

//protected: -- not for backwards compatibility
    int           m_code;
    long          m_oldItemIndex; // only for wxEVT_COMMAND_LIST_CACHE_HINT
    long          m_itemIndex;
    int           m_col;
    wxPoint       m_pointDrag;

    wxListItem    m_item;

private:
    DECLARE_DYNAMIC_CLASS(wxListEvent)
};

// ----------------------------------------------------------------------------
// wxListCtrl event macros
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_DRAG, 700)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_RDRAG, 701)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, 702)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_END_LABEL_EDIT, 703)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ITEM, 704)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, 705)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_GET_INFO, 706)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_SET_INFO, 707)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_SELECTED, 708)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_DESELECTED, 709)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_KEY_DOWN, 710)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_INSERT_ITEM, 711)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_CLICK, 712)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, 713)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, 714)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, 715)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_CACHE_HINT, 716)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_RIGHT_CLICK, 717)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, 718)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_DRAGGING, 719)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_COL_END_DRAG, 720)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_LIST_ITEM_FOCUSED, 721)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxListEventFunction)(wxListEvent&);

#define EVT_LIST_BEGIN_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_BEGIN_RDRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_BEGIN_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_END_LABEL_EDIT(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_DELETE_ITEM(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_DELETE_ALL_ITEMS(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_GET_INFO(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_SET_INFO(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_KEY_DOWN(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_INSERT_ITEM(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_INSERT_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),

#define EVT_LIST_COL_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_COL_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_COL_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_COL_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_COL_BEGIN_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_COL_DRAGGING(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_COL_DRAGGING, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_COL_END_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_COL_END_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),

#define EVT_LIST_ITEM_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_ITEM_DESELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL ),
#define EVT_LIST_ITEM_RIGHT_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LIST_ITEM_MIDDLE_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LIST_ITEM_ACTIVATED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL ),
#define EVT_LIST_ITEM_FOCUSED(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_ITEM_FOCUSED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL ),

#define EVT_LIST_CACHE_HINT(id, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_COMMAND_LIST_CACHE_HINT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL ),

#endif // wxUSE_LISTCTRL

#endif
    // _WX_LISTCTRL_H_BASE_
