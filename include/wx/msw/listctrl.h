/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.h
// Purpose:     wxListCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __LISTCTRLH__
#define __LISTCTRLH__

#ifdef __GNUG__
#pragma interface "listctrl.h"
#endif

#include "wx/control.h"
#include "wx/event.h"
#include "wx/imaglist.h"

/*
    The wxListCtrl can show lists of items in four different modes:
    wxLC_LIST:   multicolumn list view, with optional small icons (icons could be
                 optional for some platforms). Columns are computed automatically,
                 i.e. you don't set columns as in wxLC_REPORT. In other words,
                 the list wraps, unlike a wxListBox.
    wxLC_REPORT: single or multicolumn report view (with optional header)
    wxLC_ICON:   large icon view, with optional labels
    wxLC_SMALL_ICON: small icon view, with optional labels

    You can change the style dynamically, either with SetSingleStyle or
    SetWindowStyleFlag.

    Further window styles:

    wxLC_ALIGN_TOP          icons align to the top (default)
    wxLC_ALIGN_LEFT         icons align to the left
    wxLC_AUTOARRANGE        icons arrange themselves
    wxLC_USER_TEXT          the app provides label text on demand, except for column headers
    wxLC_EDIT_LABELS        labels are editable: app will be notified.
    wxLC_NO_HEADER          no header in report mode
    wxLC_NO_SORT_HEADER     can't click on header
    wxLC_SINGLE_SEL         single selection
    wxLC_SORT_ASCENDING     sort ascending (must still supply a comparison callback in SortItems)
    wxLC_SORT_DESCENDING    sort descending (ditto)

    Items are referred to by their index (position in the list starting from zero).

    Label text is supplied via insertion/setting functions and is stored by the
    control, unless the wxLC_USER_TEXT style has been specified, in which case
    the app will be notified when text is required (see sample).

    Images are dealt with by (optionally) associating 3 image lists with the control.
    Zero-based indexes into these image lists indicate which image is to be used for
    which item. Each image in an image list can contain a mask, and can be made out
    of either a bitmap, two bitmaps or an icon. See ImagList.h for more details.

    Notifications are passed via the wxWindows 2.0 event system, or using virtual
    functions in wxWindows 1.66.

    See the sample wxListCtrl app for API usage.

    TODO:
     - addition of further convenience functions
       to avoid use of wxListItem in some functions
     - state/overlay images: probably not needed.
     - in Win95, you can be called back to supply other information
       besides text, such as state information. This saves no memory
       and is probably superfluous to requirements.
     - discover why SetWindowLong doesn't properly change the
       style, requiring RecreateWindow instead.
     - testing of whole API, extending current sample.


 */

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
#define wxLIST_STATE_DROPHILITED    0x0001
#define wxLIST_STATE_FOCUSED        0x0002
#define wxLIST_STATE_SELECTED       0x0004
#define wxLIST_STATE_CUT            0x0008

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

#define wxLIST_HITTEST_ONITEM (wxLIST_HITTEST_ONITEMICON | wxLIST_HITTEST_ONITEMLABEL wxLIST_HITTEST_ONITEMSTATEICON)

// Flags for GetNextItem
enum {
    wxLIST_NEXT_ABOVE,          // Searches for an item above the specified item
    wxLIST_NEXT_ALL,            // Searches for subsequent item by index
    wxLIST_NEXT_BELOW,          // Searches for an item below the specified item
    wxLIST_NEXT_LEFT,           // Searches for an item to the left of the specified item
    wxLIST_NEXT_RIGHT,          // Searches for an item to the right of the specified item
};

// Alignment flags for Arrange
enum {
    wxLIST_ALIGN_DEFAULT,
    wxLIST_ALIGN_LEFT,
    wxLIST_ALIGN_TOP,
    wxLIST_ALIGN_SNAP_TO_GRID
};

// Column format
enum {
    wxLIST_FORMAT_LEFT,
    wxLIST_FORMAT_RIGHT,
    wxLIST_FORMAT_CENTRE,
    wxLIST_FORMAT_CENTER = wxLIST_FORMAT_CENTRE
};

// Autosize values for SetColumnWidth
enum {
    wxLIST_AUTOSIZE = -1,
    wxLIST_AUTOSIZE_USEHEADER = -2
};

// Flag values for GetItemRect
enum {
    wxLIST_RECT_BOUNDS,
    wxLIST_RECT_ICON,
    wxLIST_RECT_LABEL
};

// Flag values for FindItem
enum {
    wxLIST_FIND_UP,
    wxLIST_FIND_DOWN,
    wxLIST_FIND_LEFT,
    wxLIST_FIND_RIGHT
};

// wxListItem: data representing an item, or report field.
// It also doubles up to represent entire column information
// when inserting or setting a column.
class WXDLLEXPORT wxListItem: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxListItem)
public:
    long            m_mask;     // Indicates what fields are valid
    long            m_itemId;   // The zero-based item position
    int             m_col;      // Zero-based column, if in report mode
    long            m_state;    // The state of the item
    long            m_stateMask; // Which flags of m_state are valid (uses same flags)
    wxString        m_text;     // The label/header text
    int             m_image;    // The zero-based index into an image list
    long            m_data;     // App-defined data

    // For columns only
    int             m_format;   // left, right, centre
    int             m_width;    // width of column

    wxListItem(void);
};

// type of compare function for wxListCtrl sort operation
typedef int (*wxListCtrlCompare)(long item1, long item2, long sortData);

class WXDLLEXPORT wxListCtrl: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxListCtrl)
 public:
  /*
   * Public interface
   */

  wxListCtrl(void);

  inline wxListCtrl(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator,
            const wxString& name = "listCtrl")
  {
    Create(parent, id, pos, size, style, validator, name);
  }
  ~wxListCtrl(void);

  bool Create(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "wxListCtrl");


  // Attributes
  ////////////////////////////////////////////////////////////////////////////

  // Sets the background colour (GetBackgroundColour already implicit in
  // wxWindow class)
  void SetBackgroundColour(const wxColour& col);

  // Gets information about this column
  bool GetColumn(int col, wxListItem& item) const;

  // Sets information about this column
  bool SetColumn(int col, wxListItem& item) ;

  // Gets the column width
  int GetColumnWidth(int col) const;

  // Sets the column width
  bool SetColumnWidth(int col, int width) ;

  // Gets the number of items that can fit vertically in the
  // visible area of the list control (list or report view)
  // or the total number of items in the list control (icon
  // or small icon view)
  int GetCountPerPage(void) const;

  // Gets the edit control for editing labels.
  wxTextCtrl& GetEditControl(void) const;

  // Gets information about the item
  bool GetItem(wxListItem& info) const ;

  // Sets information about the item
  bool SetItem(wxListItem& info) ;

  // Sets a string field at a particular column
  long SetItem(long index, int col, const wxString& label, int imageId = -1);

  // Gets the item state
  int  GetItemState(long item, long stateMask) const ;

  // Sets the item state
  bool SetItemState(long item, long state, long stateMask) ;

  // Sets the item image
  bool SetItemImage(long item, int image, int selImage) ;

  // Gets the item text
  wxString GetItemText(long item) const ;

  // Sets the item text
  void SetItemText(long item, const wxString& str) ;

  // Gets the item data
  long GetItemData(long item) const ;

  // Sets the item data
  bool SetItemData(long item, long data) ;

  // Gets the item rectangle
  bool GetItemRect(long item, wxRectangle& rect, int code = wxLIST_RECT_BOUNDS) const ;

  // Gets the item position
  bool GetItemPosition(long item, wxPoint& pos) const ;

  // Sets the item position
  bool SetItemPosition(long item, const wxPoint& pos) ;

  // Gets the number of items in the list control
  int GetItemCount(void) const;

  // Gets the number of columns in the list control
  int GetColumnCount(void) const;

  // Retrieves the spacing between icons in pixels.
  // If small is TRUE, gets the spacing for the small icon
  // view, otherwise the large icon view.
  int GetItemSpacing(bool isSmall) const;

  // Gets the number of selected items in the list control
  int GetSelectedItemCount(void) const;

  // Gets the text colour of the listview
  wxColour GetTextColour(void) const;

  // Sets the text colour of the listview
  void SetTextColour(const wxColour& col);

  // Gets the index of the topmost visible item when in
  // list or report view
  long GetTopItem(void) const ;

  // Add or remove a single window style
  void SetSingleStyle(long style, bool add = TRUE) ;

  // Set the whole window style
  void SetWindowStyleFlag(long style) ;

  // Searches for an item, starting from 'item'.
  // item can be -1 to find the first item that matches the
  // specified flags.
  // Returns the item or -1 if unsuccessful.
  long GetNextItem(long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE) const ;

  // Implementation: converts wxWindows style to MSW style.
  // Can be a single style flag or a bit list.
  // oldStyle is 'normalised' so that it doesn't contain
  // conflicting styles.
  long ConvertToMSWStyle(long& oldStyle, long style) const;

  // Gets one of the three image lists
  wxImageList *GetImageList(int which) const ;

  // Sets the image list
  // N.B. There's a quirk in the Win95 list view implementation.
  // If in wxLC_LIST mode, it'll *still* display images by the labels if
  // there's a small-icon image list set for the control - even though you
  // haven't specified wxLIST_MASK_IMAGE when inserting.
  // So you have to set a NULL small-icon image list to be sure that
  // the wxLC_LIST mode works without icons. Of course, you may want icons...
  void SetImageList(wxImageList *imageList, int which) ;

  // Operations
  ////////////////////////////////////////////////////////////////////////////

  // Arranges the items
  bool Arrange(int flag = wxLIST_ALIGN_DEFAULT);

  // Deletes an item
  bool DeleteItem(long item);

  // Deletes all items
  bool DeleteAllItems(void) ;

  // Deletes a column
  bool DeleteColumn(int col);

  // Deletes all columns
  bool DeleteAllColumns(void);

  // Clears items, and columns if there are any.
  void ClearAll(void);

  // Edits a label
  wxTextCtrl& Edit(long item) ;

  // Ensures this item is visible
  bool EnsureVisible(long item) ;

  // Find an item whose label matches this string, starting from the item after 'start'
  // or the beginning if 'start' is -1.
  long FindItem(long start, const wxString& str, bool partial = FALSE);

  // Find an item whose data matches this data, starting from the item after 'start'
  // or the beginning if 'start' is -1.
  long FindItem(long start, long data);

  // Find an item nearest this position in the specified direction, starting from
  // the item after 'start' or the beginning if 'start' is -1.
  long FindItem(long start, const wxPoint& pt, int direction);

  // Determines which item (if any) is at the specified point,
  // giving details in 'flags' (see wxLIST_HITTEST_... flags above)
  long HitTest(const wxPoint& point, int& flags);

  // Inserts an item, returning the index of the new item if successful,
  // -1 otherwise.
  // TOD: Should also have some further convenience functions
  // which don't require setting a wxListItem object
  long InsertItem(wxListItem& info);

  // Insert a string item
  long InsertItem(long index, const wxString& label);

  // Insert an image item
  long InsertItem(long index, int imageIndex);

  // Insert an image/string item
  long InsertItem(long index, const wxString& label, int imageIndex);

  // For list view mode (only), inserts a column.
  long InsertColumn(long col, wxListItem& info);

  long InsertColumn(long col, const wxString& heading, int format = wxLIST_FORMAT_LEFT,
    int width = -1);

  // Scrolls the list control. If in icon, small icon or report view mode,
  // x specifies the number of pixels to scroll. If in list view mode, x
  // specifies the number of columns to scroll.
  // If in icon, small icon or list view mode, y specifies the number of pixels
  // to scroll. If in report view mode, y specifies the number of lines to scroll.
  bool ScrollList(int dx, int dy);

  // Sort items.

  // fn is a function which takes 3 long arguments: item1, item2, data.
  // item1 is the long data associated with a first item (NOT the index).
  // item2 is the long data associated with a second item (NOT the index).
  // data is the same value as passed to SortItems.
  // The return value is a negative number if the first item should precede the second
  // item, a positive number of the second item should precede the first,
  // or zero if the two items are equivalent.

  // data is arbitrary data to be passed to the sort function.
  bool SortItems(wxListCtrlCompare fn, long data);

/* Why should we need this function? Leave for now.
 * WE NEED IT because item data may have changed,
 * but the display needs refreshing (in string callback mode)
  // Updates an item. If the list control has the wxLI_AUTO_ARRANGE style,
  // the items will be rearranged.
  bool Update(long item);
*/

  void Command(wxCommandEvent& event) { ProcessCommand(event); };

  // IMPLEMENTATION
  bool MSWCommand(WXUINT param, WXWORD id);
  bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam);

  // Recreate window - seems to be necessary when changing a style.
  void RecreateWindow(void);

  // Add to pool: necessary because Windows needs to have a string
  // still exist across 3 callbacks.
  char *AddPool(const wxString& str);

protected:
  wxTextCtrl        m_textCtrl;        // The control used for editing a label
  wxImageList *     m_imageListNormal; // The image list for normal icons
  wxImageList *     m_imageListSmall;  // The image list for small icons
  wxImageList *     m_imageListState;  // The image list state icons (not implemented yet)

  long              m_baseStyle;  // Basic Windows style flags, for recreation purposes
  wxStringList      m_stringPool; // Pool of 3 strings to satisfy Windows callback
                                  // requirements
  int               m_colCount;   // Windows doesn't have GetColumnCount so must
                                  // keep track of inserted/deleted columns

};

class WXDLLEXPORT wxListEvent: public wxCommandEvent
{
  DECLARE_DYNAMIC_CLASS(wxListEvent)

 public:
  wxListEvent(wxEventType commandType = wxEVT_NULL, int id = 0);

  int           m_code;
  long          m_itemIndex;
  long          m_oldItemIndex;
  int           m_col;
  bool          m_cancelled;
  wxPoint       m_pointDrag;

  wxListItem    m_item;
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

#endif
    // __LISTCTRLH__
