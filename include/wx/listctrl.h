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

#if defined(__WXMSW__)
    #ifdef __WIN16__
        #include "wx/generic/listctrl.h"
    #else
        #include "wx/msw/listctrl.h"
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/generic/listctrl.h"
#elif defined(__WXGTK__)
    #include "wx/generic/listctrl.h"
#elif defined(__WXQT__)
    #include "wx/generic/listctrl.h"
#elif defined(__WXMAC__)
    #include "wx/generic/listctrl.h"
#elif defined(__WXPM__)
    #include "wx/generic/listctrl.h"
#elif defined(__WXSTUBS__)
    #include "wx/generic/listctrl.h"
#endif

// ----------------------------------------------------------------------------
// wxListEvent - the event class for the wxListCtrl notifications
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListEvent : public wxNotifyEvent
{
public:
    wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

    int           m_code;
    long          m_itemIndex;
    long          m_oldItemIndex;
    int           m_col;
    bool          m_cancelled;
    wxPoint       m_pointDrag;

    wxListItem    m_item;

    int GetCode() const { return m_code; }
    long GetIndex() const { return m_itemIndex; }
    long GetOldIndex() const { return m_oldItemIndex; }
    long GetOldItem() const { return m_oldItemIndex; }
    int GetColumn() const { return m_col; }
    bool Cancelled() const { return m_cancelled; }
    wxPoint GetPoint() const { return m_pointDrag; }
    const wxString& GetLabel() const { return m_item.m_text; }
    const wxString& GetText() const { return m_item.m_text; }
    int GetImage() const { return m_item.m_image; }
    long GetData() const { return m_item.m_data; }
    long GetMask() const { return m_item.m_mask; }
    const wxListItem& GetItem() const { return m_item; }

    void CopyObject(wxObject& object_dest) const;

private:
    DECLARE_DYNAMIC_CLASS(wxListEvent)
};

typedef void (wxEvtHandler::*wxListEventFunction)(wxListEvent&);

#define EVT_LIST_BEGIN_DRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_DRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_RDRAG(id, fn) { wxEVT_COMMAND_LIST_BEGIN_RDRAG, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_BEGIN_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_END_LABEL_EDIT(id, fn) { wxEVT_COMMAND_LIST_END_LABEL_EDIT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ITEM(id, fn) { wxEVT_COMMAND_LIST_DELETE_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_DELETE_ALL_ITEMS(id, fn) { wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_GET_INFO(id, fn) { wxEVT_COMMAND_LIST_GET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_SET_INFO(id, fn) { wxEVT_COMMAND_LIST_SET_INFO, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_SELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_SELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_DESELECTED(id, fn) { wxEVT_COMMAND_LIST_ITEM_DESELECTED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_KEY_DOWN(id, fn) { wxEVT_COMMAND_LIST_KEY_DOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_INSERT_ITEM(id, fn) { wxEVT_COMMAND_LIST_INSERT_ITEM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_COL_CLICK(id, fn) { wxEVT_COMMAND_LIST_COL_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, NULL },
#define EVT_LIST_ITEM_RIGHT_CLICK(id, fn) { wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_MIDDLE_CLICK(id, fn) { wxEVT_COMMAND_LIST_ITEM_MIDDLE_CLICK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },
#define EVT_LIST_ITEM_ACTIVATED(id, fn) { wxEVT_COMMAND_LIST_ITEM_ACTIVATED, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxListEventFunction) & fn, (wxObject *) NULL },

#endif // wxUSE_LISTCTRL

#endif
    // _WX_LISTCTRL_H_BASE_
